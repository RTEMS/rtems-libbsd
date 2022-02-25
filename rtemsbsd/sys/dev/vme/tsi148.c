/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2022 embedded brains GmbH (http://www.embedded-brains.de)
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

#include <machine/rtems-bsd-kernel-space.h>

#include <sys/cdefs.h>
#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/module.h>
#include <sys/bus.h>
#include <sys/rman.h>

#include <dev/pci/pcireg.h>
#include <dev/pci/pcivar.h>

struct tsi148 {
	device_t		dev;
	int			mem_rid;
	struct resource		*mem_res;
	int			irq_rid;
	struct resource		*irq_res;
	void			*irq_cookie;
};

static void
tsi148_intr(void *arg)
{
	struct tsi148 *sc;

	sc = arg;
}

static int
tsi148_probe(device_t dev)
{

	if (pci_get_devid(dev) == 0x014810e3) {
		device_set_desc(dev, "Tundra Tsi148 PCI-VME bridge");
		return (BUS_PROBE_GENERIC);
	}

	return (ENXIO);
}

static int
tsi148_attach(device_t dev)
{
	struct tsi148 *sc;

	sc = device_get_softc(dev);
	sc->dev = dev;

	sc->mem_rid = PCIR_BAR(0);
	sc->mem_res = bus_alloc_resource_any(sc->dev, SYS_RES_MEMORY,
	    &sc->mem_rid, RF_ACTIVE);
	if (sc->mem_res == NULL) {
		return (ENOMEM);
	}

	sc->irq_rid = 0;
	sc->irq_res = bus_alloc_resource_any(sc->dev, SYS_RES_IRQ,
	    &sc->irq_rid, RF_SHAREABLE | RF_ACTIVE);
	if (sc->irq_res == NULL) {
		return (ENOMEM);
	}

	bus_setup_intr(sc->dev, sc->irq_res, INTR_TYPE_MISC | INTR_MPSAFE,
	    NULL, tsi148_intr, sc, &sc->irq_cookie);
	if (sc->irq_cookie == NULL) {
		return (ENOMEM);
	}

	return (ENXIO);
}

static int
tsi148_detach(device_t dev)
{
	BSD_ASSERT(0);
	return (ENXIO);
}

static device_method_t tsi148_methods[] = {
	/* Device interface */
	DEVMETHOD(device_probe,     tsi148_probe),
	DEVMETHOD(device_attach,    tsi148_attach),
	DEVMETHOD(device_detach,    tsi148_detach),
	DEVMETHOD_END
};

static driver_t tsi148_driver = {
	"tsi148",
	tsi148_methods,
	sizeof(struct tsi148),
};

static devclass_t tsi148_devclass;

DRIVER_MODULE(tsi148, pci, tsi148_driver, tsi148_devclass, NULL, 0);
