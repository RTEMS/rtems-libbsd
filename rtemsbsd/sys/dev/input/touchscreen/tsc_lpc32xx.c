/*-
 * COPYRIGHT (c) 2017 Kevin Kirspel
 * All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <machine/rtems-bsd-kernel-space.h>

#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <rtems/bsd/local/opt_evdev.h>

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/rman.h>
#include <sys/bus.h>
#include <sys/kernel.h>
#include <sys/lock.h>
#include <sys/malloc.h>
#include <sys/module.h>
#include <sys/mutex.h>
#include <sys/condvar.h>
#include <sys/sysctl.h>
#include <sys/selinfo.h>
#include <sys/poll.h>
#include <sys/uio.h>
#include <sys/conf.h>

#include <dev/evdev/input.h>
#include <dev/evdev/evdev.h>

#include <machine/bus.h>

#include <bsp.h>
#if defined(LIBBSP_ARM_LPC32XX_BSP_H) && defined(EVDEV_SUPPORT)

#include <arm/lpc/lpcreg.h>
#include <arm/lpc/lpcvar.h>

//#define LPC32XX_TSC_USE_PCLK
/*
 * Touchscreen controller register offsets
 */
#define LPC32XX_TSC_STAT										0x00
#define LPC32XX_TSC_SEL											0x04
#define LPC32XX_TSC_CON											0x08
#define LPC32XX_TSC_FIFO										0x0C
#define LPC32XX_TSC_DTR											0x10
#define LPC32XX_TSC_RTR											0x14
#define LPC32XX_TSC_UTR											0x18
#define LPC32XX_TSC_TTR											0x1C
#define LPC32XX_TSC_DXP											0x20
#define LPC32XX_TSC_MIN_X										0x24
#define LPC32XX_TSC_MAX_X										0x28
#define LPC32XX_TSC_MIN_Y										0x2C
#define LPC32XX_TSC_MAX_Y										0x30
#define LPC32XX_TSC_AUX_UTR									0x34
#define LPC32XX_TSC_AUX_MIN									0x38
#define LPC32XX_TSC_AUX_MAX									0x3C

#define LPC32XX_CLKPWR_ADCCTRL1_PCLK_SEL		(1 << 8)
#define LPC32XX_CLKPWR_ADCCTRL1_RTDIV(n)		(((n) & 0xFF) << 0)
#define LPC32XX_TSC_CLK											400000UL

#define LPC32XX_TSC_STAT_FIFO_OVRRN					(1 << 8)
#define LPC32XX_TSC_STAT_FIFO_EMPTY					(1 << 7)

#define LPC32XX_TSC_SEL_DEFVAL							0x0284

#define LPC32XX_TSC_ADCCON_IRQ_TO_FIFO_4		(0x1 << 11)
#define LPC32XX_TSC_ADCCON_X_SAMPLE_SIZE(s)	((10 - (s)) << 7)
#define LPC32XX_TSC_ADCCON_Y_SAMPLE_SIZE(s)	((10 - (s)) << 4)
#define LPC32XX_TSC_ADCCON_POWER_UP					(1 << 2)
#define LPC32XX_TSC_ADCCON_AUTO_EN					(1 << 0)

#define LPC32XX_TSC_FIFO_TS_P_LEVEL		(1 << 31)
#define LPC32XX_TSC_FIFO_NORMALIZE_X_VAL(x)	(((x) & 0x03FF0000) >> 16)
#define LPC32XX_TSC_FIFO_NORMALIZE_Y_VAL(y)	((y) & 0x000003FF)

#define LPC32XX_TSC_ADCDAT_VALUE_MASK		0x000003FF

#define LPC32XX_TSC_MIN_XY_VAL			0x0
#define LPC32XX_TSC_MAX_XY_VAL			0x3FF

#define	LPC_TSC_LOCK(_sc)		\
	mtx_lock(&(_sc)->sc_mtx)
#define	LPC_TSC_UNLOCK(_sc)		\
	mtx_unlock(&(_sc)->sc_mtx)
#define	LPC_TSC_LOCK_INIT(_sc)	\
	mtx_init(&_sc->sc_mtx, device_get_nameunit(_sc->sc_dev), \
		  "lpctsc", MTX_DEF)
#define	LPC_TSC_LOCK_DESTROY(_sc)	\
	mtx_destroy(&_sc->sc_mtx);
#define	LPC_TSC_LOCK_ASSERT(_sc)	\
	mtx_assert(&(_sc)->sc_mtx, MA_OWNED)

#define	TSCBARR(sc) bus_space_barrier((sc)->sc_io_tag, (sc)->sc_io_hdl, 0, (sc)->sc_io_size, \
			BUS_SPACE_BARRIER_READ|BUS_SPACE_BARRIER_WRITE)
#define	TSCWRITE1(sc, r, x) \
 do { TSCBARR(sc); bus_space_write_1((sc)->sc_io_tag, (sc)->sc_io_hdl, (r), (x)); } while (0)
#define	TSCWRITE2(sc, r, x) \
 do { TSCBARR(sc); bus_space_write_2((sc)->sc_io_tag, (sc)->sc_io_hdl, (r), (x)); } while (0)
#define	TSCWRITE4(sc, r, x) \
 do { TSCBARR(sc); bus_space_write_4((sc)->sc_io_tag, (sc)->sc_io_hdl, (r), (x)); } while (0)
#define	TSCREAD1(sc, r) (TSCBARR(sc), bus_space_read_1((sc)->sc_io_tag, (sc)->sc_io_hdl, (r)))
#define	TSCREAD2(sc, r) (TSCBARR(sc), bus_space_read_2((sc)->sc_io_tag, (sc)->sc_io_hdl, (r)))
#define	TSCREAD4(sc, r) (TSCBARR(sc), bus_space_read_4((sc)->sc_io_tag, (sc)->sc_io_hdl, (r)))

struct lpc_tsc_softc {
	device_t sc_dev;
	struct mtx sc_mtx;
	struct resource	*sc_io_res;
	struct resource *sc_irq_res;
	void *sc_intr_hdl;
	bus_space_tag_t sc_io_tag;
	bus_space_handle_t sc_io_hdl;
	bus_size_t sc_io_size;
	struct evdev_dev *sc_evdev;
};

static evdev_open_t lpc_tsc_ev_open;
static evdev_close_t lpc_tsc_ev_close;

static const struct evdev_methods lpc_tsc_evdev_methods = {
	.ev_open = &lpc_tsc_ev_open,
	.ev_close = &lpc_tsc_ev_close,
};

static void
lpc_adc_module_enable(struct lpc_tsc_softc *sc)
{
	uint32_t pclk, divider;

  lpc_pwr_write(sc->sc_dev, LPC_CLKPWR_ADCLK_CTRL, 1);
#ifdef LPC32XX_TSC_USE_PCLK
  /* Get the clock frequency for the peripheral clock*/
  pclk = LPC32XX_PERIPH_CLK;
	/* compute the divider needed to ensure we are at or below 400KHz ADC Sampling rate*/
  divider = ( pclk / LPC32XX_TSC_CLK ) + 1;
  lpc_pwr_write(sc->sc_dev, LPC_CLKPWR_ADCLK_CTRL1,
      LPC32XX_CLKPWR_ADCCTRL1_PCLK_SEL |
      LPC32XX_CLKPWR_ADCCTRL1_RTDIV(divider));
#else
  lpc_pwr_write(sc->sc_dev, LPC_CLKPWR_ADCLK_CTRL1, 0);
#endif
}

static void
lpc_adc_module_disable(struct lpc_tsc_softc *sc)
{
  lpc_pwr_write(sc->sc_dev, LPC_CLKPWR_ADCLK_CTRL, 0);
}

static void
lpc_adc_fifo_clear(struct lpc_tsc_softc *sc)
{
	uint32_t tmp;

	while(!(TSCREAD4(sc, LPC32XX_TSC_STAT) & LPC32XX_TSC_STAT_FIFO_EMPTY)) {
		tmp = TSCREAD4(sc, LPC32XX_TSC_FIFO);
	}
}

static void
lpc_tsc_interrupt(void *arg)
{
	struct lpc_tsc_softc *sc = (struct lpc_tsc_softc *)arg;
	uint32_t tmp, rv[4], xs[4], ys[4];
	int idx;
	int id, i, x, y;

	LPC_TSC_LOCK(sc);

	tmp = TSCREAD4(sc, LPC32XX_TSC_STAT);
	if(tmp & LPC32XX_TSC_STAT_FIFO_OVRRN) {
		lpc_adc_fifo_clear(sc);
		LPC_TSC_UNLOCK(sc);
		return;
	}

	/*
	 * Gather and normalize 4 samples. Pen-up events may have less
	 * than 4 samples, but its ok to pop 4 and let the last sample
	 * pen status check drop the samples.
	 */
	idx = 0;
	while((idx < 4) && !(TSCREAD4(sc, LPC32XX_TSC_STAT) & LPC32XX_TSC_STAT_FIFO_EMPTY)) {
		tmp = TSCREAD4(sc, LPC32XX_TSC_FIFO);
		xs[idx] = LPC32XX_TSC_ADCDAT_VALUE_MASK - LPC32XX_TSC_FIFO_NORMALIZE_X_VAL(tmp);
		ys[idx] = LPC32XX_TSC_ADCDAT_VALUE_MASK - LPC32XX_TSC_FIFO_NORMALIZE_Y_VAL(tmp);
		rv[idx] = tmp;
		idx++;
	}

	/* Data is only valid if pen is still down in last sample */
	if(!(rv[3] & LPC32XX_TSC_FIFO_TS_P_LEVEL) && (idx == 4)) {
		/* Use average of 2nd and 3rd sample for position */
		evdev_push_event(sc->sc_evdev, EV_ABS, ABS_X, (xs[1] + xs[2]) / 2);
		evdev_push_event(sc->sc_evdev, EV_ABS, ABS_Y, (ys[1] + ys[2]) / 2);
		evdev_push_event(sc->sc_evdev, EV_KEY, BTN_TOUCH, 1);
	} else {
		evdev_push_event(sc->sc_evdev, EV_KEY, BTN_TOUCH, 0);
	}
	evdev_sync(sc->sc_evdev);

	LPC_TSC_UNLOCK(sc);
}

static void
lpc_tsc_init(struct lpc_tsc_softc *sc)
{
	uint32_t tmp;

	tmp = TSCREAD4(sc, LPC32XX_TSC_CON) & ~LPC32XX_TSC_ADCCON_POWER_UP;

	/* Set the TSC FIFO depth to 4 samples @ 10-bits per sample (max) */
	tmp = LPC32XX_TSC_ADCCON_IRQ_TO_FIFO_4 |
	      LPC32XX_TSC_ADCCON_X_SAMPLE_SIZE(10) |
	      LPC32XX_TSC_ADCCON_Y_SAMPLE_SIZE(10);
	TSCWRITE4(sc, LPC32XX_TSC_CON, tmp);

	/* These values are all preset */
	TSCWRITE4(sc, LPC32XX_TSC_SEL, LPC32XX_TSC_SEL_DEFVAL);
	TSCWRITE4(sc, LPC32XX_TSC_MIN_X, LPC32XX_TSC_MIN_XY_VAL);
	TSCWRITE4(sc, LPC32XX_TSC_MAX_X, LPC32XX_TSC_MAX_XY_VAL);
	TSCWRITE4(sc, LPC32XX_TSC_MIN_Y, LPC32XX_TSC_MIN_XY_VAL);
	TSCWRITE4(sc, LPC32XX_TSC_MAX_Y, LPC32XX_TSC_MAX_XY_VAL);

	/* Aux support is not used */
	TSCWRITE4(sc, LPC32XX_TSC_AUX_UTR, 0);
	TSCWRITE4(sc, LPC32XX_TSC_AUX_MIN, 0);
	TSCWRITE4(sc, LPC32XX_TSC_AUX_MAX, 0);

	/*
	 * Set sample rate to about 240Hz per X/Y pair. A single measurement
	 * consists of 4 pairs which gives about a 60Hz sample rate based on
	 * a stable 32768Hz clock source. Values are in clocks.
	 * Rate is (32768 / (RTR + XCONV + RTR + YCONV + DXP + TTR + UTR) / 4
	 */
	TSCWRITE4(sc, LPC32XX_TSC_RTR, 0x2);
	TSCWRITE4(sc, LPC32XX_TSC_DTR, 0x2);
	TSCWRITE4(sc, LPC32XX_TSC_TTR, 0x10);
	TSCWRITE4(sc, LPC32XX_TSC_DXP, 0x4);
	TSCWRITE4(sc, LPC32XX_TSC_UTR, 88);

	lpc_adc_fifo_clear(sc);

	/* Enable automatic ts event capture */
	TSCWRITE4(sc, LPC32XX_TSC_CON, tmp | LPC32XX_TSC_ADCCON_AUTO_EN);
}

static void
lpc_tsc_ev_close(struct evdev_dev *evdev, void *data)
{
	struct lpc_tsc_softc *sc = (struct lpc_tsc_softc *)data;
	uint32_t tmp;

	LPC_TSC_LOCK_ASSERT(sc);

	/* Disable auto mode */
	tmp = TSCREAD4(sc, LPC32XX_TSC_CON) & ~LPC32XX_TSC_ADCCON_AUTO_EN;
	TSCWRITE4(sc, LPC32XX_TSC_CON, tmp);

	lpc_adc_module_disable(sc);
}

static int
lpc_tsc_ev_open(struct evdev_dev *evdev, void *data)
{
	struct lpc_tsc_softc *sc = (struct lpc_tsc_softc *)data;

	LPC_TSC_LOCK_ASSERT(sc);

	lpc_adc_module_enable(sc);

	lpc_tsc_init(sc);

	return (0);
}

static int
lpc_tsc_probe(device_t dev)
{
  device_set_desc(dev, "LPC32x0 Touchscreen controller");
	return (BUS_PROBE_DEFAULT);
}

static int
lpc_tsc_attach(device_t dev)
{
	struct lpc_tsc_softc *sc;
	int rid;
	int err;

	/* set self dev */
	sc = device_get_softc(dev);
	sc->sc_dev = dev;

	rid = 0;
	sc->sc_io_res = bus_alloc_resource_any(dev, SYS_RES_MEMORY, &rid, RF_ACTIVE);
	if (!sc->sc_io_res) {
		device_printf(dev, "cannot map LPC TSC register space\n");
		goto fail;
	}

	sc->sc_io_tag = rman_get_bustag(sc->sc_io_res);
	sc->sc_io_hdl = rman_get_bushandle(sc->sc_io_res);
	sc->sc_io_size = rman_get_size(sc->sc_io_res);

	rid = 0;
	sc->sc_irq_res = bus_alloc_resource_any(dev, SYS_RES_IRQ, &rid, RF_ACTIVE);
	if (sc->sc_irq_res == NULL) {
		device_printf(dev, "cannot allocate interrupt\n");
		goto fail;
	}

	err = bus_setup_intr(dev, sc->sc_irq_res, INTR_TYPE_MISC | INTR_MPSAFE,
	    NULL, (void *)lpc_tsc_interrupt, sc, &sc->sc_intr_hdl);
	if (err) {
		sc->sc_intr_hdl = NULL;
		goto fail;
	}

	LPC_TSC_LOCK_INIT(sc);

	sc->sc_evdev = evdev_alloc();
	evdev_set_name(sc->sc_evdev, device_get_desc(sc->sc_dev));
	evdev_set_phys(sc->sc_evdev, device_get_nameunit(sc->sc_dev));
	evdev_set_id(sc->sc_evdev, BUS_HOST, 0, 0, 0);
	evdev_set_methods(sc->sc_evdev, sc, &lpc_tsc_evdev_methods);
	evdev_support_prop(sc->sc_evdev, INPUT_PROP_DIRECT);
	evdev_support_event(sc->sc_evdev, EV_SYN);
	evdev_support_event(sc->sc_evdev, EV_ABS);
	evdev_support_event(sc->sc_evdev, EV_KEY);

	evdev_support_abs(sc->sc_evdev, ABS_X, 0, LPC32XX_TSC_MIN_XY_VAL,
	    LPC32XX_TSC_MAX_XY_VAL, 0, 0, 0);
	evdev_support_abs(sc->sc_evdev, ABS_Y, 0, LPC32XX_TSC_MIN_XY_VAL,
	    LPC32XX_TSC_MAX_XY_VAL, 0, 0, 0);

	evdev_support_key(sc->sc_evdev, BTN_TOUCH);

	err = evdev_register_mtx(sc->sc_evdev, &sc->sc_mtx);
	if (err) {
		evdev_free(sc->sc_evdev);
		sc->sc_evdev = NULL;	/* Avoid double free */
		goto fail;
	}

	return (0);

fail:
	if (sc->sc_intr_hdl)
		bus_teardown_intr(dev, sc->sc_irq_res, sc->sc_intr_hdl);
	if (sc->sc_irq_res)
		bus_release_resource(dev, SYS_RES_IRQ, 0, sc->sc_irq_res);
	if (sc->sc_io_res)
		bus_release_resource(dev, SYS_RES_MEMORY, 0, sc->sc_io_res);

	return (ENXIO);
}

static int
lpc_tsc_detach(device_t dev)
{
	struct lpc_tsc_softc *sc;

	sc = device_get_softc(dev);

	evdev_free(sc->sc_evdev);

	bus_teardown_intr(dev, sc->sc_irq_res, sc->sc_intr_hdl);
	bus_release_resource(dev, SYS_RES_IRQ, 0, sc->sc_irq_res);
	bus_release_resource(dev, SYS_RES_MEMORY, 0, sc->sc_io_res);

	LPC_TSC_LOCK_DESTROY(sc);
	return (0);
}

static device_method_t lpc_tsc_methods[] = {
	/* Device interface */
  DEVMETHOD(device_probe,   lpc_tsc_probe),
  DEVMETHOD(device_attach,  lpc_tsc_attach),
  DEVMETHOD(device_detach,  lpc_tsc_detach),

	DEVMETHOD_END
};

static driver_t lpc_tsc_driver = {
	"lpctsc",
  lpc_tsc_methods,
	sizeof(struct lpc_tsc_softc),
};

static devclass_t lpc_tsc_devclass;

DRIVER_MODULE(lpctsc, nexus, lpc_tsc_driver, lpc_tsc_devclass, 0, 0);
MODULE_DEPEND(lpctsc, evdev, 1, 1, 1);

#endif /* defined(LIBBSP_ARM_LPC32XX_BSP_H) */
