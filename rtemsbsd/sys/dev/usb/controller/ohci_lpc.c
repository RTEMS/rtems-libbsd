/*
 * Copyright (c) 2009, 2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
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
#include <machine/rtems-bsd-support.h>

#include <bsp.h>

#include <errno.h>

#if defined(LIBBSP_ARM_LPC24XX_BSP_H)

#include <bsp/irq.h>
#include <bsp/io.h>
#include <bsp/lpc24xx.h>

#define LPC_USB_OHCI_BASE USBHC_BASE_ADDR

#define LPC_USB_I2C_BASE USBOTG_I2C_BASE_ADDR

#define LPC_OTG_CLK_CTRL OTG_CLK_CTRL

#define LPC_OTG_CLK_STAT OTG_CLK_STAT

#define LPC_USB_OHCI_IRQ LPC24XX_IRQ_USB

static void lpc_usb_module_enable(void)
{
	rtems_status_code sc;

	sc = lpc24xx_module_enable(
		LPC24XX_MODULE_USB,
		LPC24XX_MODULE_PCLK_DEFAULT
	);
	BSD_ASSERT_SC(sc);
}

static void lpc_usb_module_disable(void)
{
	rtems_status_code sc;

	sc = lpc24xx_module_disable(LPC24XX_MODULE_USB);
	BSD_ASSERT_SC(sc);
}

static void lpc_usb_pin_config(void)
{
	static const lpc24xx_pin_range pins [] = {
		LPC24XX_PIN_USB_D_PLUS_1,
		LPC24XX_PIN_USB_D_MINUS_1,
		LPC24XX_PIN_USB_PPWR_1,
		LPC24XX_PIN_USB_SCL_1,
		LPC24XX_PIN_USB_SDA_1,
		LPC24XX_PIN_TERMINAL
	};

	rtems_status_code sc;

	sc = lpc24xx_pin_config(&pins [0], LPC24XX_PIN_SET_FUNCTION);
	BSD_ASSERT_SC(sc);
}

static void lpc_usb_host_clock_enable(void)
{
	/* Nothing to do */
}

static void lpc_otg_status_and_control(void)
{
	OTG_STAT_CTRL = 0x3;
}

static rtems_interval lpc_usb_timeout_init(void)
{
	return rtems_clock_get_ticks_since_boot();
}

static bool lpc_usb_timeout_not_expired(rtems_interval start)
{
	rtems_interval elapsed = rtems_clock_get_ticks_since_boot() - start;

	return elapsed < rtems_clock_get_ticks_per_second() / 10;
}

#define LPC_OTG_CLK_HOST BSP_BIT32(0)
#define LPC_OTG_CLK_DEV BSP_BIT32(1)
#define LPC_OTG_CLK_I2C BSP_BIT32(2)
#define LPC_OTG_CLK_OTG BSP_BIT32(3)
#define LPC_OTG_CLK_AHB BSP_BIT32(4)

static int lpc_otg_clk_ctrl(uint32_t otg_clk_ctrl)
{
	rtems_interval start;
	bool not_ok;

	LPC_OTG_CLK_CTRL = otg_clk_ctrl;

	start = lpc_usb_timeout_init();
	while (
		(not_ok = (LPC_OTG_CLK_STAT & otg_clk_ctrl) != otg_clk_ctrl)
			&& lpc_usb_timeout_not_expired(start)
	) {
		/* Wait */
	}

	return not_ok ? EIO : 0;
}

#include <sys/cdefs.h>
#include <sys/stdint.h>
#include <sys/stddef.h>
#include <sys/param.h>
#include <sys/queue.h>
#include <sys/types.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/bus.h>
#include <sys/linker_set.h>
#include <sys/module.h>
#include <sys/lock.h>
#include <sys/mutex.h>
#include <sys/condvar.h>
#include <sys/sysctl.h>
#include <sys/sx.h>
#include <sys/unistd.h>
#include <sys/callout.h>
#include <sys/malloc.h>
#include <sys/priv.h>

#include <dev/usb/usb.h>
#include <dev/usb/usbdi.h>

#include <dev/usb/usb_core.h>
#include <dev/usb/usb_busdma.h>
#include <dev/usb/usb_process.h>
#include <dev/usb/usb_util.h>

#include <dev/usb/usb_controller.h>
#include <dev/usb/usb_bus.h>
#include <dev/usb/controller/ohci.h>

#ifdef BSP_USB_OTG_TRANSCEIVER_I2C_ADDR

#define I2C_CTL_SRST (1U << 8)

#define I2C_TX_DATA_MASK 0xffU
#define I2C_TX_ADDR_SHIFT 1
#define I2C_TX_ADDR_MASK 0x7fU
#define I2C_TX_READ (1U << 0)
#define I2C_TX_START (1U << 8)
#define I2C_TX_STOP (1U << 9)

#define I2C_STS_TDI (1U << 0)
#define I2C_STS_AFI (1U << 1)
#define I2C_STS_NAI (1U << 2)
#define I2C_STS_RFE (1U << 9)

typedef struct {
	uint32_t rx_tx;
	uint32_t sts;
	uint32_t ctl;
	uint32_t clkhi;
	uint32_t clklo;
} i2c_regs;

static volatile i2c_regs *i2c =
	(volatile i2c_regs *) LPC_USB_I2C_BASE;

static int i2c_wait_for_receive_fifo_not_empty(void)
{
	rtems_interval start;
	bool not_ok;

	start = lpc_usb_timeout_init();
	while (
		(not_ok = (i2c->sts & I2C_STS_RFE) != 0)
			&& lpc_usb_timeout_not_expired(start)
	) {
		/* Wait */
	}

	return not_ok ? EIO : 0;
}

static int i2c_wait_for_transaction_done(void)
{
	rtems_interval start;
	bool not_ok;

	start = lpc_usb_timeout_init();
	while (
		(not_ok = (i2c->sts & I2C_STS_TDI) == 0)
			&& lpc_usb_timeout_not_expired(start)
	) {
		/* Wait */
	}

	return not_ok ? EIO : 0;
}

static int i2c_read(
	const struct usb_otg_transceiver *self,
	uint8_t reg_addr,
	uint8_t *value
)
{
	int eno;

	i2c->ctl = I2C_CTL_SRST;

	i2c->rx_tx = self->i2c_addr | I2C_TX_START;
	i2c->rx_tx = reg_addr;
	i2c->rx_tx = self->i2c_addr | I2C_TX_READ | I2C_TX_START;
	i2c->rx_tx = I2C_TX_STOP;

	eno = i2c_wait_for_receive_fifo_not_empty();

	if (eno == 0) {
		*value = (int) i2c->rx_tx;
	}

	return eno;
}

static int i2c_write(
	const struct usb_otg_transceiver *self,
	uint8_t reg_addr,
	uint8_t value
)
{
	int eno;

	i2c->ctl = I2C_CTL_SRST;
	i2c->sts = I2C_STS_TDI;

	i2c->rx_tx = self->i2c_addr | I2C_TX_START;
	i2c->rx_tx = reg_addr;
	i2c->rx_tx = value | I2C_TX_STOP;

	eno = i2c_wait_for_transaction_done();

	return eno;
}

#endif /* BSP_USB_OTG_TRANSCEIVER_I2C_ADDR */

static device_probe_t ohci_lpc_probe;
static device_attach_t ohci_lpc_attach;
static device_detach_t ohci_lpc_detach;
static device_resume_t ohci_lpc_resume;

static int
ohci_lpc_otg_transceiver_suspend(ohci_softc_t *e)
{
	int eno = 0;

#ifdef BSP_USB_OTG_TRANSCEIVER_I2C_ADDR
	if (eno == 0) {
		eno = lpc_otg_clk_ctrl(
			LPC_OTG_CLK_AHB | LPC_OTG_CLK_HOST | LPC_OTG_CLK_I2C
		);
	}

	if (eno == 0) {
		eno = usb_otg_transceiver_suspend(&e->sc_otg_trans);
	}

#ifdef BSP_USB_OTG_TRANSCEIVER_DUMP
	usb_otg_transceiver_dump(&e->sc_otg_trans);
#endif /* BSP_USB_OTG_TRANSCEIVER_DUMP */

	if (eno == 0) {
		eno = lpc_otg_clk_ctrl(LPC_OTG_CLK_AHB | LPC_OTG_CLK_HOST);
	}
#endif /* BSP_USB_OTG_TRANSCEIVER_I2C_ADDR */

	return eno;
}

static int
ohci_lpc_resume(device_t self)
{
	int eno = 0;

#ifdef BSP_USB_OTG_TRANSCEIVER_I2C_ADDR
	if (eno == 0) {
		eno = lpc_otg_clk_ctrl(
			LPC_OTG_CLK_AHB | LPC_OTG_CLK_HOST | LPC_OTG_CLK_I2C
		);
	}

	if (eno == 0) {
		eno = usb_otg_transceiver_resume(&e->sc_otg_trans);
	}

#ifdef BSP_USB_OTG_TRANSCEIVER_VBUS
	if (eno == 0) {
		eno = usb_otg_transceiver_set_vbus(
			&e->sc_otg_trans,
			BSP_USB_OTG_TRANSCEIVER_VBUS
		);
	}
#endif /* BSP_USB_OTG_TRANSCEIVER_VBUS */

#ifdef BSP_USB_OTG_TRANSCEIVER_DUMP
	usb_otg_transceiver_dump(&e->sc_otg_trans);
#endif /* BSP_USB_OTG_TRANSCEIVER_DUMP */

	if (eno == 0) {
		eno = lpc_otg_clk_ctrl(LPC_OTG_CLK_AHB | LPC_OTG_CLK_HOST);
	}
#endif /* BSP_USB_OTG_TRANSCEIVER_I2C_ADDR */

	if (eno == 0) {
		eno = bus_generic_resume(self);
	}

	return (eno);
}

static int
ohci_lpc_probe(device_t self)
{
	device_set_desc(self, "LPC OHCI controller");

	return (0);
}

static int
ohci_lpc_attach(device_t self)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;
	ohci_softc_t *e = device_get_softc(self);
	usb_error_t ue = USB_ERR_NORMAL_COMPLETION;
	int eno = 0;

	memset(e, 0, sizeof(*e));

	/* Initialize some bus fields */
	e->sc_bus.parent = self;
	e->sc_bus.devices = e->sc_devices;
	e->sc_bus.devices_max = OHCI_MAX_DEVICES;
	e->sc_bus.dma_bits = 32;

	/* Get all DMA memory */
	if (usb_bus_mem_alloc_all(&e->sc_bus, USB_GET_DMA_TAG(self), &ohci_iterate_hw_softc)) {
		return (ENOMEM);
	}
	e->sc_dev = self;

	/* Child device */
	e->sc_bus.bdev = device_add_child(self, "usbus", -1);
	if (e->sc_bus.bdev == NULL) {
		device_printf(self, "Could not add USB device\n");
		goto error;
	}
	device_set_ivars(e->sc_bus.bdev, &e->sc_bus);
	device_set_desc(e->sc_bus.bdev, "LPC OHCI bus");
	snprintf(e->sc_vendor, sizeof(e->sc_vendor), "NXP");

	/* Register space */
	e->sc_io_tag = 0U;
	e->sc_io_hdl = LPC_USB_OHCI_BASE;
	e->sc_io_size = 0x5cU;

	lpc_usb_module_enable();

	eno = lpc_otg_clk_ctrl(LPC_OTG_CLK_AHB | LPC_OTG_CLK_I2C);
	if (eno != 0) {
		goto error;
	}

	lpc_usb_pin_config();

#ifdef BSP_USB_OTG_TRANSCEIVER_I2C_ADDR
	e->sc_otg_trans.read = i2c_read;
	e->sc_otg_trans.write = i2c_write;
	e->sc_otg_trans.i2c_addr = BSP_USB_OTG_TRANSCEIVER_I2C_ADDR;
	eno = usb_otg_transceiver_init(&e->sc_otg_trans);
	if (eno != 0) {
		goto error;
	}

#ifdef BSP_USB_OTG_TRANSCEIVER_DUMP
	usb_otg_transceiver_dump(&e->sc_otg_trans);
#endif /* BSP_USB_OTG_TRANSCEIVER_DUMP */

	eno = usb_otg_transceiver_resume(&e->sc_otg_trans);
	if (eno != 0) {
		goto error;
	}
#endif /* BSP_USB_OTG_TRANSCEIVER_I2C_ADDR */

	lpc_usb_host_clock_enable();

	eno = lpc_otg_clk_ctrl(
		LPC_OTG_CLK_AHB | LPC_OTG_CLK_HOST
			| LPC_OTG_CLK_I2C | LPC_OTG_CLK_OTG
	);
	if (eno != 0) {
		goto error;
	}

	lpc_otg_status_and_control();

#if defined(BSP_USB_OTG_TRANSCEIVER_I2C_ADDR) \
	&& defined(BSP_USB_OTG_TRANSCEIVER_VBUS)
	eno = usb_otg_transceiver_set_vbus(
		&e->sc_otg_trans,
		BSP_USB_OTG_TRANSCEIVER_VBUS
	);
	if (eno != 0) {
		goto error;
	}
#endif /* defined(BSP_USB_OTG_TRANSCEIVER_I2C_ADDR)
	  && defined(BSP_USB_OTG_TRANSCEIVER_VBUS) */

#if defined(BSP_USB_OTG_TRANSCEIVER_I2C_ADDR) \
	&& defined(BSP_USB_OTG_TRANSCEIVER_DUMP)
	usb_otg_transceiver_dump(&e->sc_otg_trans);
#endif /* defined(BSP_USB_OTG_TRANSCEIVER_I2C_ADDR)
	  && defined(BSP_USB_OTG_TRANSCEIVER_DUMP) */

	eno = lpc_otg_clk_ctrl(LPC_OTG_CLK_AHB | LPC_OTG_CLK_HOST);
	if (eno != 0) {
		goto error;
	}

	/* Install interrupt handler */
	sc = rtems_interrupt_server_handler_install(
		RTEMS_ID_NONE,
		LPC_USB_OHCI_IRQ,
		"USB",
		RTEMS_INTERRUPT_UNIQUE,
		(rtems_interrupt_handler) ohci_interrupt,
		e
	);
	BSD_ASSERT_SC(sc);

	/* OHCI intitialization */
	ue = ohci_init(e);
	if (ue != USB_ERR_NORMAL_COMPLETION) {
		goto error;
	}
	e->sc_init_done = 1;

	/* Probe and attach child */
	eno = device_probe_and_attach(e->sc_bus.bdev);
	if (eno != 0) {
		goto error;
	}

	return (0);

error:
	ohci_lpc_detach(self);

	return (ENXIO);
}

static int
ohci_lpc_detach(device_t self)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;
	ohci_softc_t *e = device_get_softc(self);

	if (e->sc_bus.bdev) {
		device_t bdev = e->sc_bus.bdev;

		device_detach(bdev);
		device_delete_child(self, bdev);
	}

	device_delete_children(self);

	if (e->sc_init_done) {
		ohci_detach(e);
	}

	sc = rtems_interrupt_server_handler_remove(
		RTEMS_ID_NONE,
		LPC_USB_OHCI_IRQ,
		(rtems_interrupt_handler) ohci_interrupt,
		e
	);
	BSD_ASSERT_SC(sc);

	ohci_lpc_otg_transceiver_suspend(e);

	lpc_otg_clk_ctrl(0);

	lpc_usb_module_disable();

	usb_bus_mem_free_all(&e->sc_bus, &ohci_iterate_hw_softc);

	return (0);
}

static device_method_t ohci_methods [] = {
	/* Device interface */
	DEVMETHOD(device_probe, ohci_lpc_probe),
	DEVMETHOD(device_attach, ohci_lpc_attach),
	DEVMETHOD(device_detach, ohci_lpc_detach),
	DEVMETHOD(device_suspend, bus_generic_suspend),
	DEVMETHOD(device_resume, ohci_lpc_resume),
	DEVMETHOD(device_shutdown, bus_generic_shutdown),

	/* Bus interface */
	DEVMETHOD(bus_print_child, bus_generic_print_child),

	{0, 0}
};

static driver_t ohci_driver = {
	.name = "ohci",
	.methods = ohci_methods,
	.size = sizeof(struct ohci_softc)
};

static devclass_t ohci_devclass;

DRIVER_MODULE(ohci, nexus, ohci_driver, ohci_devclass, 0, 0);
MODULE_DEPEND(ohci, usb, 1, 1, 1);

#endif /* defined(LIBBSP_ARM_LPC24XX_BSP_H) */
