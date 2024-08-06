/*-
 * SPDX-License-Identifier: BSD-2-Clause-FreeBSD
 *
 * Copyright (C) 2020 embedded brains GmbH (http://www.embedded-brains.de)
 * Copyright (c) 2012 Hans Petter Selasky. All rights reserved.
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
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/bus.h>
#include <sys/condvar.h>
#include <sys/kernel.h>
#include <sys/lock.h>
#include <sys/malloc.h>
#include <sys/module.h>
#include <sys/mutex.h>
#include <sys/rman.h>

#include <dev/usb/usb.h>
#include <dev/usb/usbdi.h>

#include <dev/usb/usb_core.h>
#include <dev/usb/usb_busdma.h>
#include <dev/usb/usb_process.h>
#include <dev/usb/usb_util.h>

#include <dev/usb/usb_controller.h>
#include <dev/usb/usb_bus.h>

#include <dev/usb/controller/dwc_otg.h>

static int dwc_otg_detach(device_t);

static int
dwc_otg_probe(device_t dev)
{

	device_set_desc(dev, "DWC OTG 2.0 integrated USB controller");
	return (BUS_PROBE_DEFAULT);
}

static int
dwc_otg_attach(device_t dev)
{
	struct dwc_otg_softc *sc;
	int err;
	int rid;

	sc = device_get_softc(dev);

	/* initialise some bus fields */
	sc->sc_bus.parent = dev;
	sc->sc_bus.devices = sc->sc_devices;
	sc->sc_bus.devices_max = DWC_OTG_MAX_DEVICES;
	sc->sc_bus.dma_bits = 32;
	sc->sc_mode = DWC_MODE_HOST;
	sc->sc_phy_type = DWC_OTG_PHY_INTERNAL;

	/* get all DMA memory */
	if (usb_bus_mem_alloc_all(&sc->sc_bus,
	    USB_GET_DMA_TAG(dev), NULL)) {
		return (ENOMEM);
	}

	rid = 0;
	sc->sc_io_res = bus_alloc_resource_any(dev, SYS_RES_MEMORY,
	    &rid, RF_ACTIVE);
	if (sc->sc_io_res == NULL) {
		goto error;
	}
	sc->sc_io_tag = rman_get_bustag(sc->sc_io_res);
	sc->sc_io_hdl = rman_get_bushandle(sc->sc_io_res);
	sc->sc_io_size = rman_get_size(sc->sc_io_res);

	rid = 0;
	sc->sc_irq_res = bus_alloc_resource_any(dev, SYS_RES_IRQ, &rid,
	    RF_ACTIVE);
	if (sc->sc_irq_res == NULL) {
		goto error;
	}

	sc->sc_bus.bdev = device_add_child(dev, "usbus", -1);
	if (sc->sc_bus.bdev == NULL) {
		goto error;
	}

	device_set_ivars(sc->sc_bus.bdev, &sc->sc_bus);
	dwc_otg_platform_init(sc);

	err = bus_setup_intr(dev, sc->sc_irq_res, INTR_TYPE_TTY | INTR_MPSAFE,
	    &dwc_otg_filter_interrupt, &dwc_otg_interrupt, sc, &sc->sc_intr_hdl);
	if (err) {
		sc->sc_intr_hdl = NULL;
		goto error;
	}
	err = dwc_otg_init(sc);
	if (err == 0) {
		err = device_probe_and_attach(sc->sc_bus.bdev);
	}
	if (err) {
		goto error;
	}


	return (0);

error:
	dwc_otg_detach(dev);
	return (ENXIO);
}

static int
dwc_otg_detach(device_t dev)
{

	(void)dev;
	BSD_ASSERT(0);
	return (0);
}

static device_method_t dwc_otg_methods[] = {
	/* Device interface */
	DEVMETHOD(device_probe, dwc_otg_probe),
	DEVMETHOD(device_attach, dwc_otg_attach),
	DEVMETHOD(device_detach, dwc_otg_detach),
	DEVMETHOD(device_suspend, bus_generic_suspend),
	DEVMETHOD(device_resume, bus_generic_resume),
	DEVMETHOD(device_shutdown, bus_generic_shutdown),

	DEVMETHOD_END
};

driver_t dwc_otg_driver = {
	.name = "dwcotg",
	.methods = dwc_otg_methods,
	.size = sizeof(struct dwc_otg_softc),
};

static devclass_t dwc_otg_devclass;

DRIVER_MODULE(dwcotg, nexus, dwc_otg_driver, 0, 0);
MODULE_DEPEND(dwcotg, usb, 1, 1, 1);
