#include <machine/rtems-bsd-kernel-space.h>

/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2021 embedded brains GmbH (http://www.embedded-brains.de)
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * STM32H7xx SDMMC controller. Documentation: ST RM0433 (Rev 6), Chapter 54
 *
 * According to Linux DTS, the SDMMC is compatible with an ARM Primecell PL18X
 * with peripheral ID 0x10153180.
 */
/*
 * Note: This driver is inspired by the NetBSD pl181 driver written by Jared D.
 * McNeill.
 */
/*
 * Note regarding DMA on STM32H7:
 *
 * The STM32H7 SDMMC doesn't have an interrupt for few received data (less than
 * half the FIFO size). So especially for short responses, it is not possible to
 * use the SDMMC without DMA.
 *
 * On the STM32H7 SDMMC there are two DMAs: One IDMA integrated into the SDMMC
 * and one MDMA that is a general purpose DMA. MDMA can only be used on first
 * instance of the SDMMC. For the second instance, the trigger signals are not
 * connected.
 *
 * The IDMA of SDMMC1 can only access AXI SRAM, QSPI and FMC (where SDRAM is
 * located). The IDMA of SDMMC2 could access memory in other domains too.
 *
 * MDMA is designed to be a companion for IDMA. It seems that ST thought of a
 * very specific software structure for that. It can be either used to change
 * the IDMA buffer addresses (which would allow some kind of scatter gather
 * functionality with fixed buffer sizes) or to refill IDMA buffers from some
 * RAM that can't be accessed directly by the IDMA. Take a look at ST AN5200 Rev
 * 1 "Getting started with STM32H7 Series SDMMC host controller" for more
 * details.
 */

#include <rtems/malloc.h>
#include <rtems/irq-extension.h>

#include <bsp.h>

#ifdef LIBBSP_ARM_STM32H7_BSP_H

#include <stm32h7/hal.h>
#include <stm32h7/memory.h>

#include <bsp/st-sdmmc-config.h>

#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <sys/param.h>
#include <sys/bus.h>
#include <sys/kernel.h>
#include <sys/module.h>
#include <sys/mutex.h>
#include <sys/resource.h>
#include <sys/rman.h>
#include <sys/condvar.h>

#include <pthread.h>

#include <machine/resource.h>

#include <dev/mmc/bridge.h>
#include <dev/mmc/mmcbrvar.h>

#define	ST_SDMMC_LOCK(_sc)		mtx_lock(&(_sc)->mtx)
#define	ST_SDMMC_UNLOCK(_sc)		mtx_unlock(&(_sc)->mtx)
#define	ST_SDMMC_LOCK_INIT(_sc) \
	mtx_init(&_sc->mtx, device_get_nameunit(_sc->dev), \
	    "st_sdmmc", MTX_DEF)

#define	SDMMC_INT_ERROR_MASK ( \
		SDMMC_MASK_CTIMEOUTIE | \
		SDMMC_MASK_CCRCFAILIE | \
		SDMMC_MASK_DTIMEOUTIE | \
		SDMMC_MASK_DCRCFAILIE | \
		SDMMC_MASK_ACKFAILIE | \
		SDMMC_MASK_RXOVERRIE | \
		SDMMC_MASK_TXUNDERRIE | \
		SDMMC_MASK_DABORTIE | \
		SDMMC_MASK_ACKTIMEOUTIE )
#define	SDMMC_INT_DATA_DONE_MASK ( SDMMC_MASK_IDMABTCIE | SDMMC_MASK_DATAENDIE )
#define	SDMMC_INT_CMD_DONE_MASK ( SDMMC_MASK_CMDSENTIE )
#define	SDMMC_INT_CMD_RESPONSE_DONE_MASK ( SDMMC_MASK_CMDRENDIE )

#define RES_MEM_SDMMC 0
#define RES_MEM_DLYB 1
#define RES_IRQ_SDMMC 2
#define RES_NR 3

/* Maximum non-aligned buffer is 512 byte from mmc_send_ext_csd() */
#define DMA_BUF_SIZE 512

#if 0
#define debug_print(sc, lvl, ...) \
    if (lvl <= 1) device_printf(sc->dev, __VA_ARGS__)
#else
#define debug_print(...)
#endif

struct st_sdmmc_softc;

typedef void (*st_sdmmc_dma_setup_transfer)(struct st_sdmmc_softc *, void *);

struct st_sdmmc_softc {
	device_t dev;
	struct mmc_host host;

	struct mtx mtx;
	rtems_binary_semaphore wait_done;
	int bus_busy;

	struct resource *res[RES_NR];
	SDMMC_TypeDef *sdmmc;
	DLYB_TypeDef *dlyb;
	rtems_vector_number irq;

	uint32_t sdmmc_ker_ck;
	struct st_sdmmc_config cfg;

	uint32_t intr_status;
	uint8_t *dmabuf;
};

void st_sdmmc_idma_txrx(struct st_sdmmc_softc *sc, void *buf)
{
	BSD_ASSERT(
	    (buf >= (void*) stm32h7_memory_sdram_1_begin &&
	     buf  < (void*) stm32h7_memory_sdram_1_end) ||
	    (buf >= (void*) stm32h7_memory_sram_axi_begin &&
	     buf  < (void*) stm32h7_memory_sram_axi_end) ||
	    (buf >= (void*) stm32h7_memory_sdram_2_begin &&
	     buf  < (void*) stm32h7_memory_sdram_2_end) ||
	    (buf >= (void*) stm32h7_memory_quadspi_begin &&
	     buf  < (void*) stm32h7_memory_quadspi_end));
	sc->sdmmc->IDMABASE0 = (uintptr_t) buf;
	sc->sdmmc->IDMACTRL = SDMMC_IDMA_IDMAEN;
}

void st_sdmmc_idma_stop(struct st_sdmmc_softc *sc)
{
	sc->sdmmc->IDMACTRL = 0;
}

static void
st_sdmmc_intr(void *arg)
{
	struct st_sdmmc_softc *sc;
	uint32_t status;

	sc = arg;

	status = sc->sdmmc->STA;
	sc->sdmmc->ICR = status;
	sc->intr_status |= status;

	/*
	 * There seems to be some odd combination where the status is zero but
	 * an interrupt occurred. In that case, the task shouldn't wake up.
	 * Therefore check for status != 0.
	 */
	if (status != 0 &&
	    ((status & SDMMC_STA_BUSYD0) == 0 ||
	    (sc->sdmmc->MASK & SDMMC_STA_BUSYD0END) == 0)) {
		rtems_binary_semaphore_post(&sc->wait_done);
	}
}

static int
st_sdmmc_probe(device_t dev)
{
	device_set_desc(dev, "STM32H7xx SDMMC Host");
	return (0);
}

static int
st_sdmmc_set_clock_and_bus(
	struct st_sdmmc_softc *sc,
	uint32_t freq,
	enum mmc_bus_width width
)
{
	uint32_t clk_div;
	uint32_t clkcr;

	clkcr = SDMMC_CLKCR_NEGEDGE | SDMMC_CLKCR_PWRSAV | SDMMC_CLKCR_HWFC_EN;
	clk_div = howmany(sc->sdmmc_ker_ck, freq) / 2;
	if (clk_div > SDMMC_CLKCR_CLKDIV >> SDMMC_CLKCR_CLKDIV_Pos) {
		clk_div = SDMMC_CLKCR_CLKDIV >> SDMMC_CLKCR_CLKDIV_Pos;
	}
	clkcr |= clk_div << SDMMC_CLKCR_CLKDIV_Pos;

	switch (width) {
	default:
		BSD_ASSERT(width == bus_width_1);
		clkcr |= 0 << SDMMC_CLKCR_WIDBUS_Pos;
		break;
	case bus_width_4:
		clkcr |= 1 << SDMMC_CLKCR_WIDBUS_Pos;
		break;
	case bus_width_8:
		clkcr |= 2 << SDMMC_CLKCR_WIDBUS_Pos;
		break;
	}

	sc->sdmmc->CLKCR = clkcr;

	return 0;
}

static void
st_sdmmc_host_reset(struct st_sdmmc_softc *sc)
{
	sc->sdmmc->MASK = 0;
	sc->sdmmc->ICR = 0xFFFFFFFF;
}

static void
st_sdmmc_hw_init(struct st_sdmmc_softc *sc)
{
	st_sdmmc_set_clock_and_bus(sc, 400000, bus_width_1);
	sc->sdmmc->POWER = 0;
	if (sc->cfg.dirpol) {
		sc->sdmmc->POWER |= SDMMC_POWER_DIRPOL;
	}
	/* ST example code just set it on. So do the same. */
	sc->sdmmc->POWER |= SDMMC_POWER_PWRCTRL_0 | SDMMC_POWER_PWRCTRL_1;
	/*
	 * Wait at least 74 cycles; lowest freq is 400kHz
	 * -> 1/400kHz * 47 = 117us
	 */
	usleep(120000);

	st_sdmmc_host_reset(sc);
}

static void
st_sdmmc_board_init(void)
{
	HAL_SD_MspInit(NULL);
}

static int
st_sdmmc_attach(device_t dev)
{
	struct st_sdmmc_softc *sc;
	int error = 0;
	static pthread_once_t once = PTHREAD_ONCE_INIT;
	bool interrupt_installed = false;

	sc = device_get_softc(dev);

	memset(sc, 0, sizeof(*sc));

	if (error == 0) {
		sc->dev = dev;
	}

	if (error == 0) {
		ST_SDMMC_LOCK_INIT(sc);
		rtems_binary_semaphore_init(&sc->wait_done, "sdmmc-sem");
	}

	if (error == 0) {
		int rid = 0;
		sc->res[RES_MEM_SDMMC] = bus_alloc_resource(dev, SYS_RES_MEMORY,
		    &rid, 0, ~0, 1, RF_ACTIVE);
		if (sc->res[RES_MEM_SDMMC] == NULL) {
			device_printf(dev,
			    "could not allocate sdmmc resource\n");
			error = ENXIO;
		} else {
			sc->sdmmc = (SDMMC_TypeDef *)
			    sc->res[RES_MEM_SDMMC]->r_bushandle;
		}
	}
	if (error == 0) {
		int rid = 0;
		sc->res[RES_MEM_DLYB] = bus_alloc_resource(dev, SYS_RES_MEMORY,
		    &rid, 1, ~0, 1, RF_ACTIVE);
		if (sc->res[RES_MEM_DLYB] == NULL) {
			device_printf(dev,
			    "could not allocate dlyb resource\n");
			error = ENXIO;
		} else {
			sc->dlyb = (DLYB_TypeDef *)
			    sc->res[RES_MEM_DLYB]->r_bushandle;
		}
	}
	if (error == 0) {
		int rid = 0;
		sc->res[RES_IRQ_SDMMC] = bus_alloc_resource(dev, SYS_RES_IRQ,
		    &rid, 0, ~0, 1, RF_ACTIVE);
		if (sc->res[RES_IRQ_SDMMC] == NULL) {
			device_printf(dev,
			    "could not allocate interrupt resource\n");
			error = ENXIO;
		} else {
			sc->irq = sc->res[RES_IRQ_SDMMC]->r_bushandle;
		}
	}

	if (error == 0) {
		/*
		 * FIXME: This memory should be in AXI SRAM, QSPI or FMC. In the
		 * configurations for our BSP, the heap is either in AXI SRAM or
		 * in the SDRAM. So that is OK for now. Only assert that the
		 * assumption is true. A better solution (like fixed AXI SRAM)
		 * might would be a good idea.
		 */
		sc->dmabuf = rtems_heap_allocate_aligned_with_boundary(
		    DMA_BUF_SIZE, CPU_CACHE_LINE_BYTES, 0);
		if (sc->dmabuf == NULL) {
			device_printf(dev, "could not allocate dma buffer\n");
			error = ENOMEM;
		}
		BSD_ASSERT(
		    ((void*) sc->dmabuf >= (void*) stm32h7_memory_sram_axi_begin &&
		     (void*) sc->dmabuf < (void*) stm32h7_memory_sram_axi_end) ||
		    ((void*) sc->dmabuf >= (void*) stm32h7_memory_sdram_1_begin &&
		     (void*) sc->dmabuf < (void*) stm32h7_memory_sdram_1_end) ||
		    ((void*) sc->dmabuf >= (void*) stm32h7_memory_sdram_2_begin &&
		     (void*) sc->dmabuf < (void*) stm32h7_memory_sdram_2_end) ||
		    ((void*) sc->dmabuf >= (void*) stm32h7_memory_quadspi_begin &&
		     (void*) sc->dmabuf < (void*) stm32h7_memory_quadspi_end));
	}

	if (error == 0) {
		pthread_once(&once, st_sdmmc_board_init);
	}

	if (error == 0) {
		sc->sdmmc_ker_ck = HAL_RCCEx_GetPeriphCLKFreq(
		    RCC_PERIPHCLK_SDMMC);

		sc->host.f_min = 400000;
		sc->host.f_max = (int) sc->sdmmc_ker_ck;
		if (sc->host.f_max > 50000000)
			sc->host.f_max = 50000000;
	}

	if (error == 0) {
		st_sdmmc_get_config((uintptr_t)sc->sdmmc, &sc->cfg);
		if (sc->cfg.data_lines == 0) {
			device_printf(dev, "config not found!\n");
			error = EINVAL;
		}
	}

	if (error == 0) {
		st_sdmmc_hw_init(sc);
	}

	if (error == 0) {
		error = rtems_interrupt_handler_install(sc->irq, "SDMMC",
		    RTEMS_INTERRUPT_UNIQUE, st_sdmmc_intr, sc);
		if (error != 0) {
			device_printf(dev,
			    "could not setup interrupt handler.\n");
		} else {
			interrupt_installed = true;
		}
	}

	if (error == 0) {
		sc->host.host_ocr = sc->cfg.ocr_voltage &
		    ((1 << (MMC_OCR_MAX_VOLTAGE_SHIFT + 1)) - 1);

		sc->host.caps = MMC_CAP_HSPEED;
		if (sc->cfg.data_lines >= 4) {
			sc->host.caps |= MMC_CAP_4_BIT_DATA;
		}
		if (sc->cfg.data_lines >= 8) {
			sc->host.caps |= MMC_CAP_8_BIT_DATA;
		}
	}

	if (error == 0) {
		device_add_child(dev, "mmc", -1);
		error = bus_generic_attach(dev);
	}

	if (error != 0) {
		/* Undo relevant parts */
		if (interrupt_installed) {
			rtems_interrupt_handler_remove(sc->irq,
			    st_sdmmc_intr, sc);
		}

		free(sc->dmabuf);

		/*
		 * FIXME: Should free resources but RTEMS doesn't implement
		 * bus_free_resource().
		 */
	}

	return error;
}

static int
st_sdmmc_detach(device_t dev)
{
	struct st_sdmmc_softc *sc;

	sc = device_get_softc(dev);

	/* Always attached. So this is not necessary. */
	BSD_ASSERT(0);

	(void)sc;

	return (EBUSY);
}

static int
st_sdmmc_update_ios(device_t brdev, device_t reqdev)
{
	struct st_sdmmc_softc *sc;
	struct mmc_ios *ios;
	int err;

	sc = device_get_softc(brdev);

	ST_SDMMC_LOCK(sc);

	ios = &sc->host.ios;
	err = st_sdmmc_set_clock_and_bus(sc, ios->clock, ios->bus_width);
	if (err != 0) {
		return (err);
	}

	if (ios->power_mode == power_off) {
		/*
		 * FIXME: Maybe a reset of the module is necessary instead. But
		 * the ST samples use a power off too so it should work. But
		 * power saving hasn't been tested during development.
		 */
		sc->sdmmc->POWER &= ~(SDMMC_POWER_PWRCTRL);
	} else {
		sc->sdmmc->POWER |= SDMMC_POWER_PWRCTRL;
	}

	ST_SDMMC_UNLOCK(sc);

	return (EIO);
}

static int
st_sdmmc_wait_irq(struct st_sdmmc_softc *sc)
{
	int error = 0;

	error = rtems_binary_semaphore_wait_timed_ticks(&sc->wait_done,
	    RTEMS_MILLISECONDS_TO_TICKS(5000));

	if (error != 0) {
		error = MMC_ERR_TIMEOUT;
	} else if ((sc->intr_status &
	    (SDMMC_STA_DTIMEOUT | SDMMC_STA_CTIMEOUT)) != 0) {
		error = MMC_ERR_TIMEOUT;
	} else if ((sc->intr_status & SDMMC_INT_ERROR_MASK) != 0) {
		error = MMC_ERR_FAILED;
	}

	return error;
}

static void
st_sdmmc_cmd_do(struct st_sdmmc_softc *sc, struct mmc_command *cmd)
{
	uint32_t cmdval;
	uint32_t xferlen;
	uint32_t int_mask;
	uint32_t arg;
	void *data = NULL;
	bool short_xfer = false;

	debug_print(sc, 1, "cmd: %d, arg: %08x, flags: 0x%x\n",
	    cmd->opcode, cmd->arg, cmd->flags);

	xferlen = 0;
	sc->intr_status = 0;

	sc->sdmmc->CMD = 0;
	/*
	 * There should be a delay of "at least seven sdmmc_hclk clock periods"
	 * before CMD is written again. The sdmmc_hclk is the clock that is used
	 * to access the Registers. Some more registers are accessed before the
	 * next CMD write. So that should be no problem.
	 */
	sc->sdmmc->MASK = 0;
	sc->sdmmc->ICR = 0xFFFFFFFF;
	/*
	 * Make sure the semaphore is cleared at this point. There can be an
	 * error case where a previous command run into a timeout and still
	 * produced an interrupt before it has been disabled.
	 */
	if (rtems_binary_semaphore_try_wait(&sc->wait_done) == 0) {
		device_printf(sc->dev, "Semaphore set from last command\n");
	}

	int_mask = SDMMC_INT_ERROR_MASK;

	arg = cmd->arg;
	cmdval = (cmd->opcode & SDMMC_CMD_CMDINDEX_Msk) | SDMMC_CMD_CPSMEN;

	if ((cmd->flags & MMC_RSP_PRESENT) != 0) {
		if ((cmd->flags & MMC_RSP_136) != 0) {
			cmdval |= SDMMC_CMD_WAITRESP_0 | SDMMC_CMD_WAITRESP_1;
		} else if ((cmd->flags & MMC_RSP_CRC) != 0) {
			cmdval |= SDMMC_CMD_WAITRESP_0;
		} else {
			cmdval |= SDMMC_CMD_WAITRESP_1;
		}
		int_mask |= SDMMC_INT_CMD_RESPONSE_DONE_MASK;
	} else {
		int_mask |= SDMMC_INT_CMD_DONE_MASK;
	}

	if (cmd->opcode == MMC_STOP_TRANSMISSION) {
		cmdval |= SDMMC_CMD_CMDSTOP;
	}

	if ((cmd->flags & MMC_CMD_MASK) == MMC_CMD_ADTC) {
		cmdval |= SDMMC_CMD_CMDTRANS;
	}

	if ((cmd->flags & MMC_RSP_BUSY) != 0) {
		int_mask |= SDMMC_MASK_BUSYD0ENDIE;
	}

	if (cmd->data != NULL) {
		uint32_t blksize;
		uint32_t dctrl = 0;
		xferlen = cmd->data->len;

		if (xferlen > MMC_SECTOR_SIZE) {
			blksize = ffs(MMC_SECTOR_SIZE) - 1;
		} else {
			blksize = ffs(xferlen) - 1;
		}

		debug_print(sc, 1,
		    "data: len: %d, xferlen: %d, blksize: %d, dataflags: 0x%x\n",
		    cmd->data->len, xferlen, blksize, cmd->data->flags);

		BSD_ASSERT(xferlen % (1 << blksize) == 0);

		data = cmd->data->data;
		/*
		 * Check whether data have to be copied. Reason is either
		 * misaligned start address or misaligned length.
		 */
		if (((uintptr_t)data % CPU_CACHE_LINE_BYTES != 0) ||
		    (xferlen % CPU_CACHE_LINE_BYTES) != 0) {
			BSD_ASSERT(xferlen < DMA_BUF_SIZE);
			if ((cmd->data->flags & MMC_DATA_READ) == 0) {
				memcpy(sc->dmabuf, cmd->data->data, xferlen);
			}
			data = sc->dmabuf;
			short_xfer = true;
		}

		dctrl |= blksize << SDMMC_DCTRL_DBLOCKSIZE_Pos;
		if ((cmd->data->flags & MMC_DATA_READ) != 0) {
			dctrl |= SDMMC_DCTRL_DTDIR;
			rtems_cache_invalidate_multiple_data_lines(data,
			    roundup2(xferlen, CPU_CACHE_LINE_BYTES));
		} else {
			rtems_cache_flush_multiple_data_lines(data,
			    roundup2(xferlen, CPU_CACHE_LINE_BYTES));
		}
		st_sdmmc_idma_txrx(sc, data);

		sc->sdmmc->DTIMER = 0xFFFFFFFF;
		sc->sdmmc->DLEN = xferlen;
		sc->sdmmc->DCTRL = dctrl;

		int_mask &= ~(SDMMC_INT_CMD_DONE_MASK |
		    SDMMC_INT_CMD_RESPONSE_DONE_MASK);
		int_mask |= SDMMC_INT_DATA_DONE_MASK;
	}

	sc->sdmmc->MASK = int_mask;
	sc->sdmmc->ARG = arg;
	sc->sdmmc->CMD = cmdval | cmd->opcode;

	cmd->error = st_sdmmc_wait_irq(sc);
	if (cmd->error) {
		sleep(10);
		device_printf(sc->dev,
		    "error (%d) waiting for xfer: status %08x, cmd: %d, flags: %08x\n",
		    cmd->error, sc->intr_status, cmd->opcode, cmd->flags);
	} else {
		if ((cmd->flags & MMC_RSP_PRESENT) != 0) {
			if ((cmd->flags & MMC_RSP_136) != 0) {
				cmd->resp[0] = sc->sdmmc->RESP1;
				cmd->resp[1] = sc->sdmmc->RESP2;
				cmd->resp[2] = sc->sdmmc->RESP3;
				cmd->resp[3] = sc->sdmmc->RESP4;
				debug_print(sc, 2, "rsp: %08x %08x %08x %08x\n",
				    cmd->resp[0],
				    cmd->resp[1],
				    cmd->resp[2],
				    cmd->resp[3]);
			} else {
				cmd->resp[0] = sc->sdmmc->RESP1;
				debug_print(sc, 2, "rsp: %08x\n", cmd->resp[0]);
			}
		}

		if (short_xfer && cmd->data != NULL &&
		    (cmd->data->flags & MMC_DATA_READ) != 0) {
			memcpy(cmd->data->data, sc->dmabuf, xferlen);
		}
	}

	st_sdmmc_idma_stop(sc);
	sc->sdmmc->CMD = 0;
	sc->sdmmc->MASK = 0;
	sc->sdmmc->ICR = 0xFFFFFFFF;
}

static int
st_sdmmc_request(device_t brdev, device_t reqdev, struct mmc_request *req)
{
	struct st_sdmmc_softc *sc;

	sc = device_get_softc(brdev);

	ST_SDMMC_LOCK(sc);
	st_sdmmc_cmd_do(sc, req->cmd);
	if (req->stop != NULL) {
		st_sdmmc_cmd_do(sc, req->stop);
	}
	ST_SDMMC_UNLOCK(sc);

	(*req->done)(req);

	return (0);
}

static int
st_sdmmc_get_ro(device_t brdev, device_t reqdev)
{

	/*
	 * FIXME: Currently just ignore write protection. Micro-SD doesn't have
	 * it anyway and most boards are now using Micro-SD slots.
	 */
	return (0);
}

static int
st_sdmmc_acquire_host(device_t brdev, device_t reqdev)
{
	struct st_sdmmc_softc *sc;

	sc = device_get_softc(brdev);

	ST_SDMMC_LOCK(sc);
	while (sc->bus_busy)
		msleep(sc, &sc->mtx, PZERO, "stsdmmcah", hz / 5);
	sc->bus_busy++;
	ST_SDMMC_UNLOCK(sc);
	return (0);
}

static int
st_sdmmc_release_host(device_t brdev, device_t reqdev)
{
	struct st_sdmmc_softc *sc;

	sc = device_get_softc(brdev);

	ST_SDMMC_LOCK(sc);
	sc->bus_busy--;
	wakeup(sc);
	ST_SDMMC_UNLOCK(sc);
	return (0);
}

static int
st_sdmmc_read_ivar(device_t bus, device_t child, int which, uintptr_t *result)
{
	struct st_sdmmc_softc *sc;

	sc = device_get_softc(bus);

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
	case MMCBR_IVAR_VCCQ:
		*(int *)result = sc->host.ios.vccq;
		break;
	case MMCBR_IVAR_CAPS:
		*(int *)result = sc->host.caps;
		break;
	case MMCBR_IVAR_MAX_DATA:
		/*
		 * Note: At the moment of writing this, RTEMS ignores this
		 * value. So it's quite irrelevant what is returned here.
		 */
		*(int *)result = (SDMMC_DLEN_DATALENGTH_Msk) / MMC_SECTOR_SIZE;
		break;
	case MMCBR_IVAR_TIMING:
		*(int *)result = sc->host.ios.timing;
		break;
	}
	return (0);
}

static int
st_sdmmc_write_ivar(device_t bus, device_t child, int which, uintptr_t value)
{
	struct st_sdmmc_softc *sc;

	sc = device_get_softc(bus);

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
	case MMCBR_IVAR_TIMING:
		sc->host.ios.timing = value;
		break;
	case MMCBR_IVAR_VCCQ:
		sc->host.ios.vccq = value;
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

static device_method_t st_sdmmc_methods[] = {
	/* device_if */
	DEVMETHOD(device_probe, st_sdmmc_probe),
	DEVMETHOD(device_attach, st_sdmmc_attach),
	DEVMETHOD(device_detach, st_sdmmc_detach),

	/* Bus interface */
	DEVMETHOD(bus_read_ivar, st_sdmmc_read_ivar),
	DEVMETHOD(bus_write_ivar, st_sdmmc_write_ivar),

	/* mmcbr_if */
	DEVMETHOD(mmcbr_update_ios, st_sdmmc_update_ios),
	DEVMETHOD(mmcbr_request, st_sdmmc_request),
	DEVMETHOD(mmcbr_get_ro, st_sdmmc_get_ro),
	DEVMETHOD(mmcbr_acquire_host, st_sdmmc_acquire_host),
	DEVMETHOD(mmcbr_release_host, st_sdmmc_release_host),

	DEVMETHOD_END
};

static driver_t st_sdmmc_driver = {
	"st_sdmmc",
	st_sdmmc_methods,
	sizeof(struct st_sdmmc_softc)
};

static devclass_t st_sdmmc_devclass;

DRIVER_MODULE(st_sdmmc, nexus, st_sdmmc_driver, st_sdmmc_devclass, NULL, NULL);
DRIVER_MODULE(mmc, st_sdmmc, mmc_driver, mmc_devclass, NULL, NULL);
MODULE_DEPEND(st_sdmmc, mmc, 1, 1, 1);

#endif /* LIBBSP_ARM_STM32H7_BSP_H */
