#include <machine/rtems-bsd-kernel-space.h>

/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2013 Ian Lepore <ian@freebsd.org>
 * Copyright (C) 2023 embedded brains GmbH & Co. KG
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

#include <bsp.h>
#if defined(LIBBSP_ARM_IMXRT_BSP_H)

#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

/*
 * USBPHY driver for Freescale i.MXRT1166. Most likely works with the whole
 * i.MXRT11xx family.
 *
 * Based on USBPHY driver for i.MX6.
 */

#include <rtems/bsd/local/opt_bus.h>

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/module.h>
#include <sys/bus.h>
#include <sys/rman.h>

#include <dev/ofw/ofw_bus.h>
#include <dev/ofw/ofw_bus_subr.h>

#include <machine/bus.h>

#include <dev/extres/regulator/regulator.h>

#include <fsl_device_registers.h>
#include <fsl_clock.h>

struct imxrt1166_usbphy_softc {
	device_t	dev;
	struct resource	*mem_res;
	regulator_t	supply_vbus;
	USBPHY_Type	*regs;
};

static struct ofw_compat_data compat_data[] = {
	{"fsl,imxrt1166-usbphy",	true},
	{NULL,				false}
};

static int
imxrt1166_usbphy_detach(device_t dev)
{
	struct imxrt1166_usbphy_softc *sc;

	sc = device_get_softc(dev);

	if (sc->mem_res != NULL)
		bus_release_resource(dev, SYS_RES_MEMORY, 0, sc->mem_res);

	return (0);
}

#define BUS_SPACE_PHYSADDR(res, offs) \
	((u_int)(rman_get_start(res)+(offs)))

static int
enable_vbus_supply(device_t dev, struct imxrt1166_usbphy_softc *sc)
{
	int rv;
	phandle_t node;

	node = ofw_bus_get_node(dev);
	if (OF_hasprop(node, "vbus-supply")) {
		rv = regulator_get_by_ofw_property(sc->dev, node, "vbus-supply",
		    &sc->supply_vbus);
		if (rv != 0) {
			device_printf(sc->dev,
			   "Cannot get \"vbus\" regulator\n");
			return ENXIO;
		}
		rv = regulator_enable(sc->supply_vbus);
		if (rv != 0) {
			device_printf(sc->dev,
			    "Cannot enable  \"vbus\" regulator\n");
			return ENXIO;
		}
	}

	return 0;
}

static int
imxrt1166_usbphy_attach(device_t dev)
{
	struct imxrt1166_usbphy_softc *sc;
	int err, rid;
#if IMXRT_IS_MIMXRT11xx
	uint32_t usbClockFreq;
#endif

	sc = device_get_softc(dev);
	err = 0;

	/* Allocate bus_space resources. */
	rid = 0;
	sc->mem_res = bus_alloc_resource_any(dev, SYS_RES_MEMORY, &rid,
	    RF_ACTIVE);
	if (sc->mem_res == NULL) {
		device_printf(dev, "Cannot allocate memory resources\n");
		err = ENXIO;
		goto out;
	}

	/* Enable VBUS Supply if a regulator is given */
	err = enable_vbus_supply(dev, sc);
	if (err != 0) {
		goto out;
	}

	sc->regs = (USBPHY_Type *)BUS_SPACE_PHYSADDR(sc->mem_res, 0);

#if IMXRT_IS_MIMXRT11xx
	/* Enable register clock */
	CLOCK_EnableClock(kCLOCK_Usb);

	usbClockFreq = CLOCK_GetFreq(kCLOCK_Osc24M);

	/*
	 * Set the software reset bit. It will be implicitly cleared when
	 * setting up the clock in the next steps.
	 */
	sc->regs->CTRL_SET = USBPHY_CTRL_SFTRST_MASK;

	/*
	 * Enable PLLs.
	 *
	 * FIXME: Hacky way to find out the module.
	 */
	if (sc->regs == USBPHY1) {
		CLOCK_EnableUsbhs0PhyPllClock(kCLOCK_Usbphy480M, usbClockFreq);
		CLOCK_EnableUsbhs0Clock(kCLOCK_Usb480M, usbClockFreq);
	} else {
		CLOCK_EnableUsbhs1PhyPllClock(kCLOCK_Usbphy480M, usbClockFreq);
		CLOCK_EnableUsbhs1Clock(kCLOCK_Usb480M, usbClockFreq);
	}
#else
	/* Not implemented */
#endif

	err = 0;

out:

	if (err != 0)
		imxrt1166_usbphy_detach(dev);

	return (err);
}

static int
imxrt1166_usbphy_probe(device_t dev)
{

	if (!ofw_bus_status_okay(dev))
		return (ENXIO);

	if (!ofw_bus_search_compatible(dev, compat_data)->ocd_data)
		return (ENXIO);

	device_set_desc(dev, "Freescale i.MXRT1166 USB PHY");

	return (BUS_PROBE_DEFAULT);
}

static device_method_t imxrt1166_usbphy_methods[] = {
	/* Device interface */
	DEVMETHOD(device_probe,  imxrt1166_usbphy_probe),
	DEVMETHOD(device_attach, imxrt1166_usbphy_attach),
	DEVMETHOD(device_detach, imxrt1166_usbphy_detach),

	DEVMETHOD_END
};

static driver_t imxrt1166_usbphy_driver = {
	"imxrt1166_usbphy",
	imxrt1166_usbphy_methods,
	sizeof(struct imxrt1166_usbphy_softc)
};

static devclass_t imxrt1166_usbphy_devclass;

/*
 * This driver needs to start before the ehci driver, but later than the usual
 * "special" drivers like clocks and cpu.  Ehci starts at DEFAULT so SUPPORTDEV
 * is where this driver fits most.
 */
EARLY_DRIVER_MODULE(imxrt1166_usbphy, simplebus, imxrt1166_usbphy_driver,
    imxrt1166_usbphy_devclass, 0, 0,
    BUS_PASS_SUPPORTDEV + BUS_PASS_ORDER_MIDDLE);

#endif /* LIBBSP_ARM_IMXRT_BSP_H */
