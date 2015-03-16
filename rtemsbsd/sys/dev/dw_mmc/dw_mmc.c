#include <machine/rtems-bsd-kernel-space.h>

/*-
 * Copyright (c) 2006 Bernd Walter.  All rights reserved.
 * Copyright (c) 2006 M. Warner Losh.  All rights reserved.
 * Copyright (c) 2010 Greg Ansley.  All rights reserved.
 * Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <rtems/bsd/sys/param.h>
#include <sys/systm.h>
#include <sys/bio.h>
#include <sys/bus.h>
#include <sys/conf.h>
#include <sys/endian.h>
#include <sys/kernel.h>
#include <sys/kthread.h>
#include <sys/lock.h>
#include <sys/malloc.h>
#include <sys/module.h>
#include <sys/mutex.h>
#include <sys/queue.h>
#include <sys/resource.h>
#include <sys/rman.h>
#include <sys/sysctl.h>
#include <sys/time.h>

#include <machine/bus.h>
#include <machine/cpu.h>
#include <machine/cpufunc.h>
#include <machine/resource.h>

#include <dev/dw_mmc/dw_mmcreg.h>

#include <dev/mmc/bridge.h>
#include <dev/mmc/mmcreg.h>
#include <dev/mmc/mmcbrvar.h>

#include <rtems/bsd/local/mmcbr_if.h>

#include <rtems/irq-extension.h>

#include <bsp.h>

#ifdef LIBBSP_ARM_ALTERA_CYCLONE_V_BSP_H

#define DW_MMC_ALTERA_CYCLONE_V

#include <bsp/socal/hps.h>
#include <bsp/socal/socal.h>
#include <bsp/socal/alt_sysmgr.h>
#include <bsp/alt_clock_manager.h>
#include <bsp/irq.h>

#endif /* DW_MMC_ALTERA_CYCLONE_V */

struct dw_mmc_softc {
	device_t dev;
	struct mtx sc_mtx;
	struct mtx bus_mtx;
	bus_space_handle_t bushandle;
	uint32_t biu_clock;
	uint32_t ciu_clock;
	uint32_t card_clock;
	struct mmc_host host;
	uint32_t cmdr_flags;
	volatile struct dw_mmc_des *des;
	rtems_id task_id;
};

#define DW_MMC_MAX_DES_COUNT 32

#define DW_MMC_MAX_DMA_TRANSFER_BYTES \
    (DW_MMC_MAX_DES_COUNT * 2 * DW_MMC_DES1_MAX_BS)

static inline uint32_t
RD4(struct dw_mmc_softc *sc, bus_size_t off)
{
	return (bus_space_read_4(0, sc->bushandle, off));
}

static inline void
WR4(struct dw_mmc_softc *sc, bus_size_t off, uint32_t val)
{
	bus_space_write_4(0, sc->bushandle, off, val);
}

/* bus entry points */
static int dw_mmc_probe(device_t dev);
static int dw_mmc_attach(device_t dev);
static int dw_mmc_detach(device_t dev);
static void dw_mmc_intr(void *);

static void
DW_MMC_LOCK(struct dw_mmc_softc *sc)
{
	mtx_lock(&sc->sc_mtx);
	sc->task_id = rtems_task_self();
}

#define	DW_MMC_UNLOCK(_sc)		mtx_unlock(&(_sc)->sc_mtx)
#define DW_MMC_LOCK_INIT(_sc) \
	mtx_init(&_sc->sc_mtx, device_get_nameunit(_sc->dev), \
	    "dw_mmc", MTX_DEF)

#define	DW_MMC_BUS_LOCK(_sc)		mtx_lock(&(_sc)->bus_mtx)
#define	DW_MMC_BUS_UNLOCK(_sc)		mtx_unlock(&(_sc)->bus_mtx)
#define DW_MMC_BUS_LOCK_INIT(_sc) \
	mtx_init(&_sc->bus_mtx, device_get_nameunit(_sc->dev), \
	    "dw_mmc", MTX_DEF)

static int
dw_mmc_poll_reset_completion(struct dw_mmc_softc *sc, uint32_t ctrl_resets)
{
	rtems_interval timeout = rtems_clock_tick_later_usec(250000);

	do {
		if ((RD4(sc, DW_MMC_CTRL) & ctrl_resets) == 0) {
			return 0;
		}
	} while (rtems_clock_tick_before(timeout));

	return EBUSY;
}

static uint32_t
dw_mmc_poll_intsts(struct dw_mmc_softc *sc, uint32_t mask)
{
	uint32_t ret_intsts = 0;

	while (1) {
		uint32_t intsts = RD4(sc, DW_MMC_RINTSTS);

		if ((intsts & DW_MMC_INT_ERROR) != 0) {
			WR4(sc, DW_MMC_RINTSTS, intsts);
			ret_intsts = intsts;
			break;
		}

		if ((intsts & mask) != 0) {
			WR4(sc, DW_MMC_RINTSTS, intsts & mask);
			break;
		}
	}

	return ret_intsts;
}

static void
dw_mmc_wait_for_interrupt(struct dw_mmc_softc *sc, uint32_t intmask)
{
	rtems_status_code rs;

	WR4(sc, DW_MMC_INTMASK, intmask);

	rs = rtems_event_transient_receive(RTEMS_WAIT, RTEMS_NO_TIMEOUT);
	BSD_ASSERT(rs == RTEMS_SUCCESSFUL);
}

static int
dw_mmc_init(struct dw_mmc_softc *sc)
{
	uint32_t ctrl;
	uint32_t fifoth;
	int err;

	err = dw_mmc_poll_reset_completion(sc, DW_MMC_CTRL_RESET);
	if (err != 0) {
		return err;
	}

	sc->card_clock = UINT32_MAX;

	/* Clear interrupt status */
	WR4(sc, DW_MMC_RINTSTS, 0xffffffff);

	/* Disable all interrupts */
	WR4(sc, DW_MMC_INTMASK, 0x0);

	/* Enable interrupts in general */
	ctrl = RD4(sc, DW_MMC_CTRL);
	ctrl |= DW_MMC_CTRL_INT_ENABLE;
	WR4(sc, DW_MMC_CTRL, ctrl);

	/* Set data and response timeout to maximum values */
	WR4(sc, DW_MMC_TMOUT, 0xffffffff);

	/* Set debounce value to 25ms */
	WR4(sc, DW_MMC_DEBNCE, (sc->biu_clock / 1000) * 25);

	/* Set FIFO watermarks */
	fifoth = RD4(sc, DW_MMC_FIFOTH);
	fifoth &= ~(DW_MMC_FIFOTH_RX_WMARK_MSK | DW_MMC_FIFOTH_TX_WMARK_MSK);
	fifoth |= DW_MMC_FIFOTH_RX_WMARK(511) | DW_MMC_FIFOTH_TX_WMARK(512);
	WR4(sc, DW_MMC_FIFOTH, fifoth);

	/* Set DMA descriptor */
	WR4(sc, DW_MMC_DBADDR, (uint32_t) sc->des);

	return 0;
}

static void
dw_mmc_fini(struct dw_mmc_softc *sc)
{
	WR4(sc, DW_MMC_CTRL, DW_MMC_CTRL_FIFO_RESET | DW_MMC_CTRL_RESET);
}

static int
dw_mmc_probe(device_t dev)
{

	device_set_desc(dev, "DesignWare Mobile Storage Host");
	return (0);
}

static int
dw_mmc_platform_init(struct dw_mmc_softc *sc)
{
#ifdef DW_MMC_ALTERA_CYCLONE_V
	size_t des_size = DW_MMC_MAX_DES_COUNT * sizeof(*sc->des);
	ALT_STATUS_CODE as;

	/* Module base address */
	sc->bushandle = (bus_space_handle_t) ALT_SDMMC_ADDR;

	/* BIU clock */
	as = alt_clk_freq_get(ALT_CLK_L4_MP, &sc->biu_clock);
	BSD_ASSERT(as == ALT_E_SUCCESS);

	/* CIU clock */
	as = alt_clk_clock_enable(ALT_CLK_SDMMC);
	BSD_ASSERT(as == ALT_E_SUCCESS);
	as = alt_clk_freq_get(ALT_CLK_SDMMC, &sc->ciu_clock);
	BSD_ASSERT(as == ALT_E_SUCCESS);
	sc->ciu_clock /= 4;

	sc->des = rtems_cache_coherent_allocate(des_size, 0, 0);
	if (sc->des == NULL) {
		return (ENOMEM);
	}
	memset(__DEVOLATILE(void *, sc->des), 0, des_size);
#endif

	return (0);
}

static void
dw_mmc_platform_install_intr(struct dw_mmc_softc *sc)
{
	rtems_vector_number irq =
#ifdef DW_MMC_ALTERA_CYCLONE_V
	    ALT_INT_INTERRUPT_SDMMC_IRQ;
#else
	    UINT32_MAX;
#endif
	rtems_status_code rs;

	/*
	 * Activate the interrupt
	 */
	rs = rtems_interrupt_handler_install(irq, "DW MMC",
	    RTEMS_INTERRUPT_SHARED, dw_mmc_intr, sc);
	BSD_ASSERT(rs == RTEMS_SUCCESSFUL);
}

static bool
dw_mmc_platform_set_clock(struct dw_mmc_softc *sc, uint32_t card_clock)
{
	bool use_hold_reg;

#ifdef DW_MMC_ALTERA_CYCLONE_V
	uint32_t drvsel;
	uint32_t smplsel;
	uint32_t ctl;
	ALT_STATUS_CODE as;

	/* FIXME: Values taken from U-Boot, not clear how they are determined */
	if (card_clock > 25000000) {
		drvsel = 3;
		smplsel = 7;
	} else {
		drvsel = 3;
		smplsel = 0;
	}

	use_hold_reg = drvsel != 0;

	as = alt_clk_clock_disable(ALT_CLK_SDMMC);
	BSD_ASSERT(as == ALT_E_SUCCESS);

	ctl = alt_read_word(ALT_SYSMGR_SDMMC_CTL_ADDR);
	ctl &= ALT_SYSMGR_SDMMC_CTL_DRVSEL_CLR_MSK
	    & ALT_SYSMGR_SDMMC_CTL_SMPLSEL_CLR_MSK;
	ctl |= ALT_SYSMGR_SDMMC_CTL_DRVSEL_SET(drvsel)
	    | ALT_SYSMGR_SDMMC_CTL_SMPLSEL_SET(smplsel);
	alt_write_word(ALT_SYSMGR_SDMMC_CTL_ADDR, ctl);

	as = alt_clk_clock_enable(ALT_CLK_SDMMC);
	BSD_ASSERT(as == ALT_E_SUCCESS);
#else
	use_hold_reg = false;
#endif

	return use_hold_reg;
}

static void
dw_mmc_platform_fini(struct dw_mmc_softc *sc)
{
#ifdef DW_MMC_ALTERA_CYCLONE_V
	rtems_cache_coherent_free(__DEVOLATILE(void *, sc->des));
#endif
}

static int
dw_mmc_attach(device_t dev)
{
	struct dw_mmc_softc *sc = device_get_softc(dev);
	int err;

	sc->dev = dev;

	err = dw_mmc_platform_init(sc);
	if (err != 0) {
		return (err);
	}

	dw_mmc_fini(sc);
	err = dw_mmc_init(sc);
	if (err != 0) {
		dw_mmc_platform_fini(sc);

		return (err);
	}

	DW_MMC_LOCK_INIT(sc);
	DW_MMC_BUS_LOCK_INIT(sc);

	dw_mmc_platform_install_intr(sc);

	sc->host.f_min = 400000;
	sc->host.f_max = (int) sc->ciu_clock;
	if (sc->host.f_max > 50000000)
		sc->host.f_max = 50000000;	/* Limit to 50MHz */

	sc->host.host_ocr = MMC_OCR_320_330 | MMC_OCR_330_340;

	/* FIXME: MMC_CAP_8_BIT_DATA for eSDIO? */
	sc->host.caps = MMC_CAP_4_BIT_DATA | MMC_CAP_HSPEED;

	device_add_child(dev, "mmc", 0);
	device_set_ivars(dev, &sc->host);
	err = bus_generic_attach(dev);

	return (err);
}

static int
dw_mmc_detach(device_t dev)
{
	struct dw_mmc_softc *sc = device_get_softc(dev);

	dw_mmc_fini(sc);

	/* FIXME: Implement */
	BSD_ASSERT(0);

	return (EBUSY);
}

static int
dw_mmc_cmd_wait(struct dw_mmc_softc *sc)
{
	rtems_interval timeout = rtems_clock_tick_later_usec(250000);

	do {
		if ((RD4(sc, DW_MMC_CMD) & DW_MMC_CMD_START) == 0) {
			return 0;
		}
	} while (rtems_clock_tick_before(timeout));

	return EBUSY;
}

static void
dw_mmc_cmd_start(struct dw_mmc_softc *sc, uint32_t cmd, uint32_t cmdarg)
{
	WR4(sc, DW_MMC_CMDARG, cmdarg);
	cmd |= DW_MMC_CMD_START;
	WR4(sc, DW_MMC_CMD, cmd);
}

static int
dw_mmc_cmd_update_clock(struct dw_mmc_softc *sc)
{
	dw_mmc_cmd_start(sc,
	    DW_MMC_CMD_UPDATE_CLK | DW_MMC_CMD_PRV_DATA_WAIT, 0);

	return dw_mmc_cmd_wait(sc);
}

static int
dw_mmc_set_clock(struct dw_mmc_softc *sc, uint32_t card_clock)
{
	uint32_t clkdiv;
	int err;

	if (sc->card_clock == card_clock) {
		return 0;
	}

	sc->card_clock = card_clock;

	/* Disable card clock */
	WR4(sc, DW_MMC_CLKENA, 0);

	err = dw_mmc_cmd_update_clock(sc);
	if (err != 0) {
		return err;
	}

	if (card_clock == 0) {
		return 0;
	}

	if (dw_mmc_platform_set_clock(sc, card_clock)) {
		sc->cmdr_flags |= DW_MMC_CMD_USE_HOLD_REG;
	} else {
		sc->cmdr_flags &= ~DW_MMC_CMD_USE_HOLD_REG;
	}

	if (card_clock == sc->ciu_clock) {
		clkdiv = 0;
	} else {
		uint32_t s = 2 * card_clock;

		clkdiv = (sc->ciu_clock + s - 1) / s;
	}

	WR4(sc, DW_MMC_CLKDIV, clkdiv);
	WR4(sc, DW_MMC_CLKSRC, 0);

	err = dw_mmc_cmd_update_clock(sc);
	if (err != 0) {
		return err;
	}

	/* Enable card clock */
	WR4(sc, DW_MMC_CLKENA, DW_MMC_CLKEN_ENABLE);

	return dw_mmc_cmd_update_clock(sc);
}

static int
dw_mmc_update_ios(device_t brdev, device_t reqdev)
{
	struct dw_mmc_softc *sc = device_get_softc(brdev);
	struct mmc_host *host;
	struct mmc_ios *ios;
	uint32_t ctype;
	int err;

	DW_MMC_LOCK(sc);

	host = &sc->host;
	ios = &host->ios;

	err = dw_mmc_set_clock(sc, (uint32_t) ios->clock);
	if (err != 0) {
		return (err);
	}

	if (ios->power_mode == power_off) {
		WR4(sc, DW_MMC_PWREN, 0);
	} else {
		sc->cmdr_flags |= DW_MMC_CMD_SEND_INIT;
		WR4(sc, DW_MMC_PWREN, DW_MMC_PWREN_ENABLE);
	}

	switch (ios->bus_width) {
	default:
		BSD_ASSERT(ios->bus_width == bus_width_1);
		ctype = DW_MMC_CTYPE_1BIT;
		break;
	case bus_width_4:
		ctype = DW_MMC_CTYPE_4BIT;
		break;
	case bus_width_8:
		ctype = DW_MMC_CTYPE_8BIT;
		break;
	}

	WR4(sc, DW_MMC_CTYPE, ctype);

	DW_MMC_UNLOCK(sc);

	return (0);
}

static int
dw_mmc_fifo_and_dma_reset(struct dw_mmc_softc *sc)
{
	uint32_t ctrl_resets = DW_MMC_CTRL_FIFO_RESET | DW_MMC_CTRL_DMA_RESET;
	uint32_t ctrl = RD4(sc, DW_MMC_CTRL);

	ctrl |= ctrl_resets;

	WR4(sc, DW_MMC_CTRL, ctrl);

	return dw_mmc_poll_reset_completion(sc, ctrl_resets);
}

static int
dw_mmc_cmd_read_response(struct dw_mmc_softc *sc, struct mmc_command *cmd,
    uint32_t intsts)
{
	if ((intsts & DW_MMC_INT_RTO) != 0) {
		return MMC_ERR_TIMEOUT;
	} else if ((intsts & DW_MMC_INT_RCRC) != 0
	    && (cmd->flags & MMC_RSP_CRC) != 0) {
		return MMC_ERR_BADCRC;
	} else if ((intsts & DW_MMC_INT_RE) != 0) {
		return MMC_ERR_FAILED;
	}

	if ((cmd->flags & MMC_RSP_PRESENT) != 0) {
		uint32_t *resp = &cmd->resp[0];

		if ((cmd->flags & MMC_RSP_136) != 0) {
			resp[3] = RD4(sc, DW_MMC_RESP0);
			resp[2] = RD4(sc, DW_MMC_RESP1);
			resp[1] = RD4(sc, DW_MMC_RESP2);
			resp[0] = RD4(sc, DW_MMC_RESP3);
		} else {
			resp[0] = RD4(sc, DW_MMC_RESP0);
		}
	}

	return MMC_ERR_NONE;
}

static uint32_t
dw_mmc_cmd_data_read(struct dw_mmc_softc *sc, struct mmc_data *data,
    uint32_t *data32, size_t count_bytes)
{
	uint32_t intsts = 0;

	while (count_bytes > 0) {
		uint32_t status;
		size_t available_words;
		size_t dangling_bytes = 0;
		size_t i;

		intsts = dw_mmc_poll_intsts(sc, DW_MMC_INT_RXDR
		    | DW_MMC_INT_DTO | DW_MMC_INT_HTO);

		if (intsts != 0) {
			return intsts;
		}

		status = RD4(sc, DW_MMC_STATUS);
		available_words = DW_MMC_STATUS_GET_FIFO_CNT(status);

		if (available_words * DW_MMC_FIFO_WIDTH > count_bytes) {
			dangling_bytes = count_bytes % DW_MMC_FIFO_WIDTH;
			--available_words;
		}

		for (i = 0; i < available_words; i++) {
			data32[i] = RD4(sc, DW_MMC_DATA);
		}

		data32 += available_words;
		count_bytes -= available_words * DW_MMC_FIFO_WIDTH;

		if (dangling_bytes != 0) {
			uint32_t tmp = RD4(sc, DW_MMC_DATA);

			memcpy(data32, &tmp, dangling_bytes);
			BSD_ASSERT(count_bytes == dangling_bytes);
			count_bytes = 0;
		}
	}

	if ((data->flags & MMC_DATA_MULTI) != 0) {
		intsts = dw_mmc_poll_intsts(sc, DW_MMC_INT_ACD);
	}

	return intsts;
}

static uint32_t
dw_mmc_cmd_data_write(struct dw_mmc_softc *sc, struct mmc_data *data,
    uint32_t *data32, size_t count_bytes)
{
	uint32_t intsts;

	while (count_bytes > 0) {
		uint32_t status;
		size_t pending_words = count_bytes / DW_MMC_FIFO_WIDTH;
		size_t free_words;
		size_t dangling_bytes;
		size_t words_to_write;
		size_t i;

		intsts = dw_mmc_poll_intsts(sc, DW_MMC_INT_TXDR
		    | DW_MMC_INT_HTO);

		if (intsts != 0) {
			return intsts;
		}

		status = RD4(sc, DW_MMC_STATUS);
		free_words = DW_MMC_FIFO_DEPTH - DW_MMC_STATUS_GET_FIFO_CNT(status);

		if (pending_words >= free_words) {
			words_to_write = free_words;
			dangling_bytes = 0;
		} else {
			words_to_write = pending_words;
			dangling_bytes = count_bytes % DW_MMC_FIFO_WIDTH;
		}

		for (i = 0; i < words_to_write; i++) {
			WR4(sc, DW_MMC_DATA, data32[i]);
		}

		data32 += words_to_write;
		count_bytes -= words_to_write * DW_MMC_FIFO_WIDTH;

		if (dangling_bytes != 0) {
			uint32_t tmp = 0;

			memcpy(&tmp, &data32[0], dangling_bytes);
			WR4(sc, DW_MMC_DATA, tmp);
			BSD_ASSERT(count_bytes == dangling_bytes);
			count_bytes = 0;
		}
	}

	intsts = dw_mmc_poll_intsts(sc, DW_MMC_INT_DTO);

	if ((data->flags & MMC_DATA_MULTI) != 0 && intsts == 0) {
		dw_mmc_poll_intsts(sc, DW_MMC_INT_ACD);
	}

	return intsts;
}

static uint32_t
dw_mmc_cmd_data_transfer(struct dw_mmc_softc *sc, struct mmc_data *data,
    size_t done_bytes, bool use_dma)
{
	uint32_t *data32 = (uint32_t *) ((char *) data->data + done_bytes);
	bool do_write = (data->flags & MMC_DATA_WRITE) != 0;
	uint32_t intsts;

	if (use_dma) {
		dw_mmc_wait_for_interrupt(sc, DW_MMC_INT_DTO);
		intsts = dw_mmc_poll_intsts(sc, DW_MMC_INT_DTO);

		if ((data->flags & MMC_DATA_MULTI) != 0 && intsts == 0) {
			dw_mmc_poll_intsts(sc, DW_MMC_INT_ACD);
		}

		if (!do_write) {
			rtems_cache_invalidate_multiple_data_lines(data->data,
			    data->len);
		}
	} else {
		size_t count_bytes = data->len - done_bytes;

		if (do_write) {
			intsts = dw_mmc_cmd_data_write(sc, data, data32, count_bytes);
		} else {
			intsts = dw_mmc_cmd_data_read(sc, data, data32, count_bytes);
		}
	}

	return intsts;
}

static int
dw_mmc_cmd_data_finish(struct dw_mmc_softc *sc, uint32_t intsts)
{
	int mmc_err = MMC_ERR_NONE;

	if ((intsts & DW_MMC_INT_ERROR) != 0) {
		if ((intsts & DW_MMC_INT_DCRC) != 0) {
			mmc_err = MMC_ERR_BADCRC;
		} else if ((intsts & DW_MMC_INT_EBE) != 0) {
			mmc_err = MMC_ERR_FAILED;
		} else if ((intsts & DW_MMC_INT_DRTO) != 0) {
			mmc_err = MMC_ERR_TIMEOUT;
		} else {
			mmc_err = MMC_ERR_FAILED;
		}
	}

	return mmc_err;
}

static int
dw_mmc_cmd_done(struct dw_mmc_softc *sc, struct mmc_command *cmd,
    struct mmc_data *data, size_t done_bytes, bool use_dma)
{
	uint32_t intsts;
	int mmc_err;

	dw_mmc_wait_for_interrupt(sc, DW_MMC_INT_CMD_DONE);

	intsts = RD4(sc, DW_MMC_RINTSTS);
	WR4(sc, DW_MMC_RINTSTS,
	    intsts & (DW_MMC_INT_ERROR | DW_MMC_INT_CMD_DONE));

	mmc_err = dw_mmc_cmd_read_response(sc, cmd, intsts);
	if (mmc_err != 0) {
		return mmc_err;
	}

	if (data != NULL) {
		intsts = dw_mmc_cmd_data_transfer(sc, data, done_bytes, use_dma);
		mmc_err = dw_mmc_cmd_data_finish(sc, intsts);
	}

	return mmc_err;
}

static size_t
dw_mmc_fill_fifo(struct dw_mmc_softc *sc, struct mmc_data *data)
{
	uint32_t *data32 = data->data;
	size_t count_bytes = data->len;
	size_t count_words = 0;
	size_t dangling_bytes;
	size_t i;

	if (count_bytes >= DW_MMC_FIFO_DEPTH * DW_MMC_FIFO_WIDTH) {
		count_words = DW_MMC_FIFO_DEPTH;
		dangling_bytes = 0;
	} else {
		count_words = count_bytes / DW_MMC_FIFO_WIDTH;
		dangling_bytes = count_bytes % DW_MMC_FIFO_WIDTH;
	}

	for (i = 0; i < count_words; ++i) {
		WR4(sc, DW_MMC_DATA, data32[i]);
	}

	if (dangling_bytes) {
		uint32_t tmp = 0;

		memcpy(&tmp, &data32[i], dangling_bytes);
		WR4(sc, DW_MMC_DATA, tmp);
	}

	return count_words * DW_MMC_FIFO_WIDTH + dangling_bytes;
}

static bool dw_mmc_dma_can_use(const struct mmc_data *data)
{
	uintptr_t cache_line = 32;

	return data->len >= cache_line
	    && ((data->len | (uintptr_t) data->data) & (cache_line - 1)) == 0;
}

static void
dw_mmc_dma_setup(struct dw_mmc_softc *sc, struct mmc_data *data)
{
	volatile struct dw_mmc_des *des = sc->des;
	uint32_t buf = (uint32_t) data->data;
	size_t count_bytes = data->len;
	uint32_t fs = DW_MMC_DES0_FS;
	size_t s = 2 * DW_MMC_DES1_MAX_BS;
	size_t n = (count_bytes + s - 1) / s;
	size_t m = count_bytes % s;
	size_t i;

	for (i = 0; i < n - 1; ++i) {
		des[i].des1 = DW_MMC_DES1_BS1(DW_MMC_DES1_MAX_BS)
		    | DW_MMC_DES1_BS2(DW_MMC_DES1_MAX_BS);
		des[i].des2 = buf;
		buf += DW_MMC_DES1_MAX_BS;
		des[i].des3 = buf;
		buf += DW_MMC_DES1_MAX_BS;
		des[i].des0 = DW_MMC_DES0_OWN | fs;
		fs = 0;
	}

	if (m > DW_MMC_DES1_MAX_BS) {
		des[i].des1 = DW_MMC_DES1_BS1(DW_MMC_DES1_MAX_BS)
		    | DW_MMC_DES1_BS2(m - DW_MMC_DES1_MAX_BS);
		des[i].des2 = buf;
		buf += DW_MMC_DES1_MAX_BS;
		des[i].des3 = buf;
	} else {
		des[i].des1 = DW_MMC_DES1_BS1(m);
		des[i].des2 = buf;
		des[i].des3 = 0;
	}

	des[i].des0 = DW_MMC_DES0_OWN | DW_MMC_DES0_ER | fs | DW_MMC_DES0_LD;

#ifdef __arm__
	_ARM_Data_synchronization_barrier();
#else
	/* TODO */
#endif
}


static void
dw_mmc_cmd_do(struct dw_mmc_softc *sc, struct mmc_request *req,
    struct mmc_command *cmd)
{
	size_t done_bytes = 0;
	bool use_dma = false;
	struct mmc_data *data;
	uint32_t cmdr;

	data = cmd->data;
	cmdr = cmd->opcode;

	if (cmd->opcode == MMC_STOP_TRANSMISSION) {
		cmdr |= DW_MMC_CMD_SEND_STOP;
	} else {
		cmdr |= DW_MMC_CMD_PRV_DATA_WAIT;
	}

	cmdr |= sc->cmdr_flags;
	sc->cmdr_flags &= ~DW_MMC_CMD_SEND_INIT;

	if (MMC_RSP(cmd->flags) != MMC_RSP_NONE) {
		cmdr |= DW_MMC_CMD_RESP_EXP;

		if ((cmd->flags & MMC_RSP_136) != 0) {
			cmdr |= DW_MMC_CMD_RESP_LONG;
		}
	}

	if ((cmd->flags & MMC_RSP_CRC) != 0) {
		cmdr |= DW_MMC_CMD_RESP_CRC;
	}

	if (data != NULL) {
		size_t count_bytes = data->len;
		uint32_t ctrl;
		int mmc_err;

		cmdr |= DW_MMC_CMD_DATA_EXP;

		if ((data->flags & MMC_DATA_MULTI) != 0) {
			cmdr |= DW_MMC_CMD_SEND_STOP;
		}

		mmc_err = dw_mmc_fifo_and_dma_reset(sc);
		if (mmc_err != 0) {
			cmd->error = mmc_err;
			return;
		}

		use_dma = dw_mmc_dma_can_use(data);

		ctrl = RD4(sc, DW_MMC_CTRL);

		if (use_dma) {
			ctrl |= DW_MMC_CTRL_DMA_ENABLE;
		} else {
			ctrl &= ~DW_MMC_CTRL_DMA_ENABLE;
		}

		WR4(sc, DW_MMC_CTRL, ctrl);
		WR4(sc, DW_MMC_BLKSIZ, MIN(count_bytes, MMC_SECTOR_SIZE));
		WR4(sc, DW_MMC_BYTCNT, count_bytes);

		if ((data->flags & MMC_DATA_WRITE) != 0) {
			cmdr |= DW_MMC_CMD_DATA_WR;

			if (use_dma) {
				rtems_cache_flush_multiple_data_lines(data->data,
				    count_bytes);
			} else {
				done_bytes = dw_mmc_fill_fifo(sc, data);
			}
		} else if (use_dma) {
			rtems_cache_invalidate_multiple_data_lines(data->data,
			    count_bytes);
		}

		if (use_dma) {
			if (count_bytes > DW_MMC_MAX_DMA_TRANSFER_BYTES) {
				cmd->error = MMC_ERR_INVALID;
				return;
			}

			dw_mmc_dma_setup(sc, data);
		}
	}

	dw_mmc_cmd_start(sc, cmdr, cmd->arg);

	if (use_dma) {
		WR4(sc, DW_MMC_PLDMND, 0);
	}

	cmd->error = dw_mmc_cmd_done(sc, cmd, data, done_bytes, use_dma);
}

static int
dw_mmc_request(device_t brdev, device_t reqdev, struct mmc_request *req)
{
	struct dw_mmc_softc *sc = device_get_softc(brdev);

	DW_MMC_LOCK(sc);
	dw_mmc_cmd_do(sc, req, req->cmd);
	DW_MMC_UNLOCK(sc);

	(*req->done)(req);

	return (0);
}

static int
dw_mmc_get_ro(device_t brdev, device_t reqdev)
{
	return (0);
}

static int
dw_mmc_acquire_host(device_t brdev, device_t reqdev)
{
	struct dw_mmc_softc *sc = device_get_softc(brdev);

	DW_MMC_BUS_LOCK(sc);

	return (0);
}

static int
dw_mmc_release_host(device_t brdev, device_t reqdev)
{
	struct dw_mmc_softc *sc = device_get_softc(brdev);

	DW_MMC_BUS_UNLOCK(sc);

	return (0);
}

static void
dw_mmc_intr(void *arg)
{
	struct dw_mmc_softc *sc = (struct dw_mmc_softc *) arg;
	rtems_status_code rs;

	WR4(sc, DW_MMC_INTMASK, 0);

	rs = rtems_event_transient_send(sc->task_id);
	BSD_ASSERT(rs == RTEMS_SUCCESSFUL);
}

static int
dw_mmc_read_ivar(device_t bus, device_t child, int which, uintptr_t *result)
{
	struct dw_mmc_softc *sc = device_get_softc(bus);

	switch (which) {
	default:
		return (EINVAL);
	case MMCBR_IVAR_BUS_MODE:
		*(int *)result = sc->host.ios.bus_mode;
		break;
	case MMCBR_IVAR_BUS_WIDTH:
		*(int *)result = sc->host.ios.bus_width;
		break;
	case MMCBR_IVAR_CHIP_SELECT:
		*(int *)result = sc->host.ios.chip_select;
		break;
	case MMCBR_IVAR_CLOCK:
		*(int *)result = sc->host.ios.clock;
		break;
	case MMCBR_IVAR_F_MIN:
		*(int *)result = sc->host.f_min;
		break;
	case MMCBR_IVAR_F_MAX:
		*(int *)result = sc->host.f_max;
		break;
	case MMCBR_IVAR_HOST_OCR:
		*(int *)result = sc->host.host_ocr;
		break;
	case MMCBR_IVAR_MODE:
		*(int *)result = sc->host.mode;
		break;
	case MMCBR_IVAR_OCR:
		*(int *)result = sc->host.ocr;
		break;
	case MMCBR_IVAR_POWER_MODE:
		*(int *)result = sc->host.ios.power_mode;
		break;
	case MMCBR_IVAR_VDD:
		*(int *)result = sc->host.ios.vdd;
		break;
	case MMCBR_IVAR_CAPS:
		*(int *)result = sc->host.caps;
		break;
	case MMCBR_IVAR_MAX_DATA:
		*(int *)result = 1;
		break;
	}
	return (0);
}

static int
dw_mmc_write_ivar(device_t bus, device_t child, int which, uintptr_t value)
{
	struct dw_mmc_softc *sc = device_get_softc(bus);

	switch (which) {
	default:
		return (EINVAL);
	case MMCBR_IVAR_BUS_MODE:
		sc->host.ios.bus_mode = value;
		break;
	case MMCBR_IVAR_BUS_WIDTH:
		sc->host.ios.bus_width = value;
		break;
	case MMCBR_IVAR_CHIP_SELECT:
		sc->host.ios.chip_select = value;
		break;
	case MMCBR_IVAR_CLOCK:
		sc->host.ios.clock = value;
		break;
	case MMCBR_IVAR_MODE:
		sc->host.mode = value;
		break;
	case MMCBR_IVAR_OCR:
		sc->host.ocr = value;
		break;
	case MMCBR_IVAR_POWER_MODE:
		sc->host.ios.power_mode = value;
		break;
	case MMCBR_IVAR_VDD:
		sc->host.ios.vdd = value;
		break;
	/* These are read-only */
	case MMCBR_IVAR_CAPS:
	case MMCBR_IVAR_HOST_OCR:
	case MMCBR_IVAR_F_MIN:
	case MMCBR_IVAR_F_MAX:
	case MMCBR_IVAR_MAX_DATA:
		return (EINVAL);
	}
	return (0);
}

static device_method_t dw_mmc_methods[] = {
	/* device_if */
	DEVMETHOD(device_probe, dw_mmc_probe),
	DEVMETHOD(device_attach, dw_mmc_attach),
	DEVMETHOD(device_detach, dw_mmc_detach),

	/* Bus interface */
	DEVMETHOD(bus_read_ivar, dw_mmc_read_ivar),
	DEVMETHOD(bus_write_ivar, dw_mmc_write_ivar),

	/* mmcbr_if */
	DEVMETHOD(mmcbr_update_ios, dw_mmc_update_ios),
	DEVMETHOD(mmcbr_request, dw_mmc_request),
	DEVMETHOD(mmcbr_get_ro, dw_mmc_get_ro),
	DEVMETHOD(mmcbr_acquire_host, dw_mmc_acquire_host),
	DEVMETHOD(mmcbr_release_host, dw_mmc_release_host),

	DEVMETHOD_END
};

static driver_t dw_mmc_driver = {
	"dw_mmc",
	dw_mmc_methods,
	sizeof(struct dw_mmc_softc)
};

static devclass_t dw_mmc_devclass;

DRIVER_MODULE(dw_mmc, nexus, dw_mmc_driver, dw_mmc_devclass, NULL, NULL);
