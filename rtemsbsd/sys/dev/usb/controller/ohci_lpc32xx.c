/*-
 * Copyright (c) 2011 Jakub Wojciech Klama <jceel@FreeBSD.org>
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
 *
 */
#include <machine/rtems-bsd-kernel-space.h>

#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <sys/stdint.h>
#include <sys/stddef.h>
#include <sys/param.h>
#include <sys/queue.h>
#include <sys/types.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/bus.h>
#include <sys/module.h>
#include <sys/lock.h>
#include <sys/mutex.h>
#include <sys/condvar.h>
#include <sys/sysctl.h>
#include <sys/rman.h>
#include <sys/sx.h>
#include <rtems/bsd/sys/unistd.h>
#include <sys/callout.h>
#include <sys/malloc.h>
#include <sys/priv.h>

#include <sys/kdb.h>

#include <bsp.h>
#if defined(LIBBSP_ARM_LPC32XX_BSP_H)

#ifdef BSP_USB_OTG_TRANSCEIVER_I2C_ADDR
#include <dev/usb/usb_otg_transceiver.h>
#endif /* BSP_USB_OTG_TRANSCEIVER_I2C_ADDR */

#include <dev/usb/usb.h>
#include <dev/usb/usbdi.h>

#include <dev/usb/usb_core.h>
#include <dev/usb/usb_busdma.h>
#include <dev/usb/usb_process.h>
#include <dev/usb/usb_util.h>

#include <dev/usb/usb_controller.h>
#include <dev/usb/usb_bus.h>
#include <dev/usb/controller/ohci.h>
#include <dev/usb/controller/ohcireg.h>

#include <arm/lpc/probe.h>
#include <arm/lpc/lpcreg.h>
#include <arm/lpc/lpcvar.h>

#define	I2C_START_BIT		(1 << 8)
#define	I2C_STOP_BIT		(1 << 9)
#define	I2C_READ		0x01
#define	I2C_WRITE		0x00
#define	DUMMY_BYTE		0x55

#define	lpc_otg_read_4(_sc, _reg)					\
    bus_space_read_4(_sc->sc_io_tag, _sc->sc_io_hdl, _reg)
#define	lpc_otg_write_4(_sc, _reg, _value)				\
    bus_space_write_4(_sc->sc_io_tag, _sc->sc_io_hdl, _reg, _value)
#define	lpc_otg_wait_write_4(_sc, _wreg, _sreg, _value)			\
    do {								\
    	lpc_otg_write_4(_sc, _wreg, _value);				\
    	while ((lpc_otg_read_4(_sc, _sreg) & _value) != _value);    	\
    } while (0);

static int lpc_ohci_do_probe(device_t dev);
static int lpc_ohci_attach(device_t dev);
static int lpc_ohci_detach(device_t dev);

static void lpc_usb_module_enable(device_t dev, struct ohci_softc *);
static void lpc_usb_module_disable(device_t dev, struct ohci_softc *);
static void lpc_usb_pin_config(device_t dev, struct ohci_softc *);
static void lpc_usb_host_clock_enable(device_t dev, struct ohci_softc *);
static void lpc_otg_status_and_control(device_t dev, struct ohci_softc *);
static rtems_interval lpc_usb_timeout_init(void);
static bool lpc_usb_timeout_not_expired(rtems_interval start);
static int lpc_otg_clk_ctrl(device_t dev, struct ohci_softc *sc, uint32_t otg_clk_ctrl);
static int lpc_otg_i2c_wait_for_receive_fifo_not_empty(struct ohci_softc *sc);
static int lpc_otg_i2c_wait_for_transaction_done(struct ohci_softc *sc);
static int lpc_otg_i2c_read(const struct usb_otg_transceiver *self, uint8_t reg_addr, uint8_t *value);
static int lpc_otg_i2c_write(const struct usb_otg_transceiver *self, uint8_t reg_addr, uint8_t value);

__weak_symbol int
lpc_ohci_probe(int unit)
{

	(void)unit;
	return (BUS_PROBE_DEFAULT);
}

static int
lpc_ohci_do_probe(device_t dev)
{

	device_set_desc(dev, "LPC32x0 USB OHCI controller");
	return (lpc_ohci_probe(device_get_unit(dev)));
}

static int
lpc_ohci_attach(device_t dev)
{
	struct ohci_softc *sc = device_get_softc(dev);
	int err;
  int eno;
	int rid;
	int i = 0;
	uint32_t usbctrl;
	uint32_t otgstatus;

	sc->sc_bus.parent = dev;
	sc->sc_bus.devices = sc->sc_devices;
	sc->sc_bus.devices_max = OHCI_MAX_DEVICES;
	sc->sc_bus.dma_bits = 32;

	if (usb_bus_mem_alloc_all(&sc->sc_bus, USB_GET_DMA_TAG(dev),
	    &ohci_iterate_hw_softc))
		return (ENOMEM);

	rid = 0;
	sc->sc_io_res = bus_alloc_resource_any(dev, SYS_RES_MEMORY, &rid, RF_ACTIVE);
	if (!sc->sc_io_res) {
		device_printf(dev, "cannot map OHCI register space\n");
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

	sc->sc_bus.bdev = device_add_child(dev, "usbus", -1);
	if (!(sc->sc_bus.bdev))
		goto fail;

	device_set_ivars(sc->sc_bus.bdev, &sc->sc_bus);
	strlcpy(sc->sc_vendor, "NXP", sizeof(sc->sc_vendor));

	err = bus_setup_intr(dev, sc->sc_irq_res, INTR_TYPE_BIO | INTR_MPSAFE,
	    NULL, (void *)ohci_interrupt, sc, &sc->sc_intr_hdl);
	if (err) {
		sc->sc_intr_hdl = NULL;
		goto fail;
	}

  lpc_usb_module_enable(dev, sc);

  eno = lpc_otg_clk_ctrl(dev, sc, LPC_OTG_CLOCK_CTRL_AHB_EN | LPC_OTG_CLOCK_CTRL_I2C_EN);
  if (eno != 0) {
    goto fail;
  }

  lpc_usb_pin_config(dev, sc);

#ifdef BSP_USB_OTG_TRANSCEIVER_I2C_ADDR
  sc->sc_otg_trans.read = lpc_otg_i2c_read;
  sc->sc_otg_trans.write = lpc_otg_i2c_write;
  sc->sc_otg_trans.i2c_addr = BSP_USB_OTG_TRANSCEIVER_I2C_ADDR;
  sc->sc_otg_trans.softc = sc;
  eno = usb_otg_transceiver_init(&sc->sc_otg_trans);
  if (eno != 0) {
    goto fail;
  }

#ifdef BSP_USB_OTG_TRANSCEIVER_DUMP
  usb_otg_transceiver_dump(&sc->sc_otg_trans);
#endif /* BSP_USB_OTG_TRANSCEIVER_DUMP */

  eno = usb_otg_transceiver_resume(&sc->sc_otg_trans);
  if (eno != 0) {
    goto fail;
  }
#endif /* BSP_USB_OTG_TRANSCEIVER_I2C_ADDR */

  lpc_usb_host_clock_enable(dev, sc);

  eno = lpc_otg_clk_ctrl( dev, sc,
    LPC_OTG_CLOCK_CTRL_AHB_EN | LPC_OTG_CLOCK_CTRL_HOST_EN
      | LPC_OTG_CLOCK_CTRL_I2C_EN | LPC_OTG_CLOCK_CTRL_OTG_EN
  );
  if (eno != 0) {
    goto fail;
  }

  lpc_otg_status_and_control(dev, sc);

#if defined(BSP_USB_OTG_TRANSCEIVER_I2C_ADDR) \
  && defined(BSP_USB_OTG_TRANSCEIVER_VBUS)
  eno = usb_otg_transceiver_set_vbus(
    &sc->sc_otg_trans,
    BSP_USB_OTG_TRANSCEIVER_VBUS
  );
  if (eno != 0) {
    goto fail;
  }
#endif /* defined(BSP_USB_OTG_TRANSCEIVER_I2C_ADDR)
    && defined(BSP_USB_OTG_TRANSCEIVER_VBUS) */

#if defined(BSP_USB_OTG_TRANSCEIVER_I2C_ADDR) \
  && defined(BSP_USB_OTG_TRANSCEIVER_DUMP)
  usb_otg_transceiver_dump(&sc->sc_otg_trans);
#endif /* defined(BSP_USB_OTG_TRANSCEIVER_I2C_ADDR)
    && defined(BSP_USB_OTG_TRANSCEIVER_DUMP) */

  eno = lpc_otg_clk_ctrl(dev, sc, LPC_OTG_CLOCK_CTRL_AHB_EN | LPC_OTG_CLOCK_CTRL_HOST_EN);
  if (eno != 0) {
    goto fail;
  }

	err = ohci_init(sc);
	if (err)
		goto fail;

	err = device_probe_and_attach(sc->sc_bus.bdev);
	if (err)
		goto fail;

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
lpc_ohci_detach(device_t dev)
{
	return (0);
}

static void lpc_usb_module_enable(device_t dev, struct ohci_softc *sc)
{
  uint32_t usbctrl;

  lpc_pwr_write(dev, LPC_CLKPWR_USBDIV_CTRL, 0xc);
  lpc_pwr_write(dev, LPC_CLKPWR_USB_CTRL,
      LPC_CLKPWR_USB_CTRL_SLAVE_HCLK |
      LPC_CLKPWR_USB_CTRL_BUSKEEPER |
      LPC_CLKPWR_USB_CTRL_CLK_EN1 |
      LPC_CLKPWR_USB_CTRL_PLL_PDOWN |
      LPC_CLKPWR_USB_CTRL_POSTDIV(1) |
      LPC_CLKPWR_USB_CTRL_PREDIV(0) |
      LPC_CLKPWR_USB_CTRL_FDBKDIV(192));
  do {
    usbctrl = lpc_pwr_read(dev, LPC_CLKPWR_USB_CTRL);
  } while ((usbctrl & LPC_CLKPWR_USB_CTRL_PLL_LOCK) == 0);

  usbctrl = lpc_pwr_read(dev, LPC_CLKPWR_USB_CTRL);
  usbctrl |= LPC_CLKPWR_USB_CTRL_CLK_EN2;
  lpc_pwr_write(dev, LPC_CLKPWR_USB_CTRL, usbctrl);
}

static void lpc_usb_module_disable(device_t dev, struct ohci_softc *sc)
{
  lpc_otg_write_4(sc, LPC_OTG_CLOCK_CTRL, 0x0);
  lpc_pwr_write(dev, LPC_CLKPWR_USB_CTRL, LPC_CLKPWR_USB_CTRL_BUSKEEPER);
}

static void lpc_usb_pin_config(device_t dev, struct ohci_softc *sc)
{
}

static void lpc_usb_host_clock_enable(device_t dev, struct ohci_softc *sc)
{
  uint32_t usbctrl;

  usbctrl = lpc_pwr_read(dev, LPC_CLKPWR_USB_CTRL);
  usbctrl |= LPC_CLKPWR_USB_CTRL_HOST_NEED_CLK_EN;
  lpc_pwr_write(dev, LPC_CLKPWR_USB_CTRL, usbctrl);
}

static void lpc_otg_status_and_control(device_t dev, struct ohci_softc *sc)
{
  lpc_otg_write_4(sc, LPC_OTG_STATUS, 0x1);
}

static rtems_interval lpc_usb_timeout_init(void)
{
  return rtems_clock_get_ticks_since_boot();
}

static bool lpc_usb_timeout_not_expired(rtems_interval start)
{
  rtems_interval elapsed = rtems_clock_get_ticks_since_boot() - start;

  return elapsed < (rtems_clock_get_ticks_per_second() / 10);
}

static int lpc_otg_clk_ctrl(device_t dev, struct ohci_softc *sc, uint32_t otg_clk_ctrl)
{
  rtems_interval start;
  bool not_ok;

  lpc_otg_write_4(sc, LPC_OTG_CLOCK_CTRL, otg_clk_ctrl);

  start = lpc_usb_timeout_init();
  while (
    (not_ok = (lpc_otg_read_4(sc, LPC_OTG_CLOCK_STATUS) & otg_clk_ctrl) != otg_clk_ctrl)
      && lpc_usb_timeout_not_expired(start)
  ) {
    /* Wait */
  }

  return not_ok ? EIO : 0;
}

static int lpc_otg_i2c_wait_for_receive_fifo_not_empty(struct ohci_softc *sc)
{
  rtems_interval start;
  bool not_ok;

  start = lpc_usb_timeout_init();
  while (
    (not_ok = (lpc_otg_read_4(sc, LPC_OTG_I2C_STATUS) & LPC_OTG_I2C_STATUS_RFE) != 0)
      && lpc_usb_timeout_not_expired(start)
  ) {
    /* Wait */
  }

  return not_ok ? EIO : 0;
}

static int lpc_otg_i2c_wait_for_transaction_done(struct ohci_softc *sc)
{
  rtems_interval start;
  bool not_ok;

  start = lpc_usb_timeout_init();
  while (
    (not_ok = (lpc_otg_read_4(sc, LPC_OTG_I2C_STATUS) & LPC_OTG_I2C_STATUS_TDI) == 0)
      && lpc_usb_timeout_not_expired(start)
  ) {
    /* Wait */
  }

  return not_ok ? EIO : 0;
}

static int lpc_otg_i2c_read(const struct usb_otg_transceiver *self, uint8_t reg_addr, uint8_t *value)
{
  struct ohci_softc *sc = (struct ohci_softc *)self->softc;
  int eno;

  lpc_otg_write_4(sc, LPC_OTG_I2C_CTRL, LPC_OTG_I2C_CTRL_SRST);
  lpc_otg_write_4(sc, LPC_OTG_I2C_TXRX, self->i2c_addr | I2C_START_BIT);
  lpc_otg_write_4(sc, LPC_OTG_I2C_TXRX, reg_addr);
  lpc_otg_write_4(sc, LPC_OTG_I2C_TXRX, self->i2c_addr | I2C_START_BIT | I2C_READ);
  lpc_otg_write_4(sc, LPC_OTG_I2C_TXRX, I2C_STOP_BIT);

  eno = lpc_otg_i2c_wait_for_receive_fifo_not_empty(sc);

  if (eno == 0) {
    *value = (uint8_t)lpc_otg_read_4(sc, LPC_OTG_I2C_TXRX);
  }

  return eno;
}

static int lpc_otg_i2c_write(const struct usb_otg_transceiver *self, uint8_t reg_addr, uint8_t value)
{
  struct ohci_softc *sc = (struct ohci_softc *)self->softc;
  int eno;

  lpc_otg_write_4(sc, LPC_OTG_I2C_CTRL, LPC_OTG_I2C_CTRL_SRST);
  lpc_otg_write_4(sc, LPC_OTG_I2C_STATUS, LPC_OTG_I2C_STATUS_TDI);
  lpc_otg_write_4(sc, LPC_OTG_I2C_TXRX, self->i2c_addr | I2C_START_BIT);
  lpc_otg_write_4(sc, LPC_OTG_I2C_TXRX, reg_addr);
  lpc_otg_write_4(sc, LPC_OTG_I2C_TXRX, value | I2C_STOP_BIT);

  eno = lpc_otg_i2c_wait_for_transaction_done(sc);

  return eno;
}

static int ohci_lpc_otg_transceiver_suspend(device_t dev, struct ohci_softc *sc)
{
  int eno = 0;

#ifdef BSP_USB_OTG_TRANSCEIVER_I2C_ADDR
  if (eno == 0) {
    eno = lpc_otg_clk_ctrl( dev, sc,
      LPC_OTG_CLOCK_CTRL_AHB_EN | LPC_OTG_CLOCK_CTRL_HOST_EN | LPC_OTG_CLOCK_CTRL_I2C_EN
    );
  }

  if (eno == 0) {
    eno = usb_otg_transceiver_suspend(&sc->sc_otg_trans);
  }

#ifdef BSP_USB_OTG_TRANSCEIVER_DUMP
  usb_otg_transceiver_dump(&sc->sc_otg_trans);
#endif /* BSP_USB_OTG_TRANSCEIVER_DUMP */

  if (eno == 0) {
    eno = lpc_otg_clk_ctrl(dev, sc, LPC_OTG_CLOCK_CTRL_AHB_EN | LPC_OTG_CLOCK_CTRL_HOST_EN);
  }
#endif /* BSP_USB_OTG_TRANSCEIVER_I2C_ADDR */

  return eno;
}

static int
lpc_ohci_resume(device_t dev)
{
  struct ohci_softc *sc = device_get_softc(dev);
  int eno = 0;

#ifdef BSP_USB_OTG_TRANSCEIVER_I2C_ADDR
  if (eno == 0) {
    eno = lpc_otg_clk_ctrl( dev, sc,
      LPC_OTG_CLOCK_CTRL_AHB_EN | LPC_OTG_CLOCK_CTRL_HOST_EN | LPC_OTG_CLOCK_CTRL_I2C_EN
    );
  }

  if (eno == 0) {
    eno = usb_otg_transceiver_resume(&sc->sc_otg_trans);
  }

#ifdef BSP_USB_OTG_TRANSCEIVER_VBUS
  if (eno == 0) {
    eno = usb_otg_transceiver_set_vbus(
      &sc->sc_otg_trans,
      BSP_USB_OTG_TRANSCEIVER_VBUS
    );
  }
#endif /* BSP_USB_OTG_TRANSCEIVER_VBUS */

#ifdef BSP_USB_OTG_TRANSCEIVER_DUMP
  usb_otg_transceiver_dump(&sc->sc_otg_trans);
#endif /* BSP_USB_OTG_TRANSCEIVER_DUMP */

  if (eno == 0) {
    eno = lpc_otg_clk_ctrl(dev, sc, LPC_OTG_CLOCK_CTRL_AHB_EN | LPC_OTG_CLOCK_CTRL_HOST_EN);
  }
#endif /* BSP_USB_OTG_TRANSCEIVER_I2C_ADDR */

  if (eno == 0) {
    eno = bus_generic_resume(dev);
  }

  return (eno);
}

static device_method_t lpc_ohci_methods[] = {
	/* Device interface */
	DEVMETHOD(device_probe,		lpc_ohci_do_probe),
	DEVMETHOD(device_attach,	lpc_ohci_attach),
	DEVMETHOD(device_detach,	lpc_ohci_detach),
  DEVMETHOD(device_suspend, bus_generic_suspend),
  DEVMETHOD(device_resume,  lpc_ohci_resume),
	DEVMETHOD(device_shutdown,	bus_generic_shutdown),

	/* Bus interface */
	DEVMETHOD(bus_print_child,	bus_generic_print_child),
	{ 0, 0 }
};

static driver_t lpc_ohci_driver = {
	"ohci",
	lpc_ohci_methods,
	sizeof(struct ohci_softc),
};

static devclass_t lpc_ohci_devclass;

DRIVER_MODULE(ohci, nexus, lpc_ohci_driver, lpc_ohci_devclass, 0, 0);
MODULE_DEPEND(ohci, usb, 1, 1, 1);

#endif /* defined(LIBBSP_ARM_LPC32XX_BSP_H) */
