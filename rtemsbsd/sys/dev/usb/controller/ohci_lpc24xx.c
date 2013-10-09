/*
 * Copyright (c) 2009-2012 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <machine/rtems-bsd-config.h>

#include <bsp.h>

#ifdef LIBBSP_ARM_LPC24XX_BSP_H

#include <bsp/irq.h>
#include <bsp/io.h>
#include <bsp/lpc24xx.h>

#include <sys/cdefs.h>
#include <sys/stdint.h>
#include <sys/stddef.h>
#include <rtems/bsd/sys/param.h>
#include <sys/queue.h>
#include <rtems/bsd/sys/types.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/bus.h>
#include <sys/linker_set.h>
#include <sys/module.h>
#include <rtems/bsd/sys/lock.h>
#include <sys/mutex.h>
#include <sys/condvar.h>
#include <sys/sysctl.h>
#include <sys/sx.h>
#include <rtems/bsd/sys/unistd.h>
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

static device_probe_t ohci_lpc24xx_probe;
static device_attach_t ohci_lpc24xx_attach;
static device_detach_t ohci_lpc24xx_detach;
static device_suspend_t ohci_lpc24xx_suspend;
static device_resume_t ohci_lpc24xx_resume;

static int
ohci_lpc24xx_suspend(device_t self)
{
	ohci_softc_t *e = device_get_softc(self);
	int eno = bus_generic_suspend(self);

	if (eno != 0) {
		return (eno);
	}

	ohci_suspend(e);

	return (0);
}

static int
ohci_lpc24xx_resume(device_t self)
{
	ohci_softc_t *e = device_get_softc(self);

	ohci_resume(e);

	bus_generic_resume(self);

	return (0);
}


static int
ohci_lpc24xx_probe(device_t self)
{
	device_set_desc(self, "LPC24XX OHCI controller");

	return (0);
}

static int
ohci_lpc24xx_attach(device_t self)
{
	static const lpc24xx_pin_range pins [] = {
		LPC24XX_PIN_USB_D_PLUS_1,
		LPC24XX_PIN_USB_D_MINUS_1,
		LPC24XX_PIN_USB_PPWR_1,
		LPC24XX_PIN_TERMINAL
	};

	rtems_status_code sc = RTEMS_SUCCESSFUL;
	ohci_softc_t *e = device_get_softc(self);
	usb_error_t ue = USB_ERR_NORMAL_COMPLETION;
	int eno = 0;

	memset(e, 0, sizeof(*e));

	BSD_PRINTF("XXX\n");

	/* Initialize some bus fields */
	e->sc_bus.parent = self;
	e->sc_bus.devices = e->sc_devices;
	e->sc_bus.devices_max = OHCI_MAX_DEVICES;

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
	device_set_desc(e->sc_bus.bdev, "LPC24XX OHCI bus");
	snprintf(e->sc_vendor, sizeof(e->sc_vendor), "NXP");

	/* Register space */
	e->sc_io_tag = 0U;
	e->sc_io_hdl = USBHC_BASE_ADDR;
	e->sc_io_size = 0x5cU;

	/* Enable USB module */
	sc = lpc24xx_module_enable(LPC24XX_MODULE_USB, LPC24XX_MODULE_PCLK_DEFAULT);
	BSD_ASSERT_SC(sc);

	/* Enable USB host and AHB clocks */
	OTG_CLK_CTRL = 0x19;
	while ((OTG_CLK_STAT & 0x19) != 0x19) {
		/* Wait */
	}

	/* Set OTG Status and Control Register */
	OTG_STAT_CTRL = 0x3;

	/* Configure IO pins */
	sc = lpc24xx_pin_config(&pins [0], LPC24XX_PIN_SET_FUNCTION);
	BSD_ASSERT_SC(sc);

	/* Install interrupt handler */
	sc = rtems_interrupt_server_handler_install(
		RTEMS_ID_NONE,
		LPC24XX_IRQ_USB,
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

	/* Probe and attach child */
	eno = device_probe_and_attach(e->sc_bus.bdev);
	if (eno != 0) {
		goto error;
	}

	return (0);

error:
	ohci_lpc24xx_detach(self);
	return (ENXIO);
}

static int
ohci_lpc24xx_detach(device_t self)
{
	ohci_softc_t *e = device_get_softc(self);

	BSD_PRINTF("XXX\n");

	return (0);
}

static device_method_t ohci_methods [] = {
	/* Device interface */
	DEVMETHOD(device_probe, ohci_lpc24xx_probe),
	DEVMETHOD(device_attach, ohci_lpc24xx_attach),
	DEVMETHOD(device_detach, ohci_lpc24xx_detach),
	DEVMETHOD(device_suspend, ohci_lpc24xx_suspend),
	DEVMETHOD(device_resume, ohci_lpc24xx_resume),
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

#endif /* LIBBSP_ARM_LPC24XX_BSP_H */
