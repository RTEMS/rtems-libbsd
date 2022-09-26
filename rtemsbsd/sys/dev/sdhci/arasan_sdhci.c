#include <machine/rtems-bsd-kernel-space.h>

/*-
 * Copyright (c) 2019 Navigation Technology Associates Inc.
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
#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

/*
 * SDHCI driver glue for Zynq 7000 SDHCI
 *
 */

#include <rtems/bsd/local/opt_mmccam.h>

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/types.h>
#include <sys/bus.h>
#include <sys/callout.h>
#include <sys/kernel.h>
#include <sys/libkern.h>
#include <sys/lock.h>
#include <sys/malloc.h>
#include <sys/module.h>
#include <sys/mutex.h>
#include <rtems/bsd/sys/resource.h>
#include <sys/rman.h>
#include <sys/sysctl.h>
#include <sys/taskqueue.h>
#include <sys/time.h>

#include <machine/bus.h>
#include <machine/resource.h>

#ifdef __arm__
#include <machine/intr.h>
#endif

#include <dev/ofw/ofw_bus.h>
#include <dev/ofw/ofw_bus_subr.h>

#include <dev/mmc/bridge.h>

#include <dev/sdhci/sdhci.h>

#include <rtems/bsd/local/mmcbr_if.h>
#include <rtems/bsd/local/sdhci_if.h>

struct arasan_sdhci_softc {
	device_t		dev;
	struct resource *	mem_res;
	struct resource *	irq_res;
	void *			intr_cookie;
	struct sdhci_slot	slot;
	bool			slot_init_done;
};

static inline uint16_t
RD2(struct arasan_sdhci_softc *sc, bus_size_t off)
{
	return (bus_read_2(sc->mem_res, off));
}

static inline void
WR2(struct arasan_sdhci_softc *sc, bus_size_t off, uint16_t val)
{
	bus_write_2(sc->mem_res, off, val);
}

static inline uint32_t
RD4(struct arasan_sdhci_softc *sc, bus_size_t off)
{
	return (bus_read_4(sc->mem_res, off));
}

static inline void
WR4(struct arasan_sdhci_softc *sc, bus_size_t off, uint32_t val)
{
	bus_write_4(sc->mem_res, off, val);
}

static uint8_t
arasan_sdhci_read_1(device_t dev, struct sdhci_slot *slot, bus_size_t off)
{
	struct arasan_sdhci_softc *sc = device_get_softc(dev);

	return ((RD4(sc, off & ~3) >> (off & 3) * 8) & 0xff);
}

static uint16_t
arasan_sdhci_read_2(device_t dev, struct sdhci_slot *slot, bus_size_t off)
{
	struct arasan_sdhci_softc *sc = device_get_softc(dev);

	return (RD2(sc, off));
}

static uint32_t
arasan_sdhci_read_4(device_t dev, struct sdhci_slot *slot, bus_size_t off)
{
	struct arasan_sdhci_softc *sc = device_get_softc(dev);

	return (RD4(sc, off));
}

static void
arasan_sdhci_read_multi_4(device_t dev, struct sdhci_slot *slot, bus_size_t off,
	uint32_t *data, bus_size_t count)
{
	struct arasan_sdhci_softc *sc = device_get_softc(dev);

	bus_read_multi_4(sc->mem_res, off, data, count);
}

static void
arasan_sdhci_write_1(device_t dev, struct sdhci_slot *slot, bus_size_t off, uint8_t val)
{
	struct arasan_sdhci_softc *sc = device_get_softc(dev);
	uint32_t val32;

	val32 = RD4(sc, off & ~3);
	val32 &= ~(0xff << (off & 3) * 8);
	val32 |= (val << (off & 3) * 8);

	WR4(sc, off & ~3, val32);
}

static void
arasan_sdhci_write_2(device_t dev, struct sdhci_slot *slot, bus_size_t off, uint16_t val)
{
	struct arasan_sdhci_softc *sc = device_get_softc(dev);

	WR2(sc, off, val);
}

static void
arasan_sdhci_write_4(device_t dev, struct sdhci_slot *slot, bus_size_t off, uint32_t val)
{
	struct arasan_sdhci_softc *sc = device_get_softc(dev);

	WR4(sc, off, val);
}

static void
arasan_sdhci_write_multi_4(device_t dev, struct sdhci_slot *slot, bus_size_t off,
	uint32_t *data, bus_size_t count)
{
	struct arasan_sdhci_softc *sc = device_get_softc(dev);

	bus_write_multi_4(sc->mem_res, off, data, count);
}

static void
arasan_sdhci_intr(void *arg)
{
	struct arasan_sdhci_softc *sc = arg;

	sdhci_generic_intr(&sc->slot);
}

static int
arasan_sdhci_get_ro(device_t bus, device_t child)
{
	struct arasan_sdhci_softc *sc = device_get_softc(bus);

	return (RD4(sc, SDHCI_PRESENT_STATE) & SDHCI_WRITE_PROTECT);
}

static bool
arasan_sdhci_get_card_present(device_t dev, struct sdhci_slot *slot)
{
	struct arasan_sdhci_softc *sc = device_get_softc(dev);

	return (RD4(sc, SDHCI_PRESENT_STATE) & SDHCI_CARD_PRESENT);
}

static int
arasan_sdhci_detach(device_t dev)
{
	struct arasan_sdhci_softc *sc = device_get_softc(dev);

	if (sc->slot_init_done)
		sdhci_cleanup_slot(&sc->slot);

	if (sc->intr_cookie != NULL)
		bus_teardown_intr(dev, sc->irq_res, sc->intr_cookie);

	if (sc->irq_res != NULL)
		bus_release_resource(dev, SYS_RES_IRQ,
			rman_get_rid(sc->irq_res), sc->irq_res);

	if (sc->mem_res != NULL) {
		bus_release_resource(dev, SYS_RES_MEMORY,
			rman_get_rid(sc->mem_res), sc->mem_res);
	}

	return (0);
}

static int
arasan_sdhci_attach(device_t dev)
{
	struct arasan_sdhci_softc *sc = device_get_softc(dev);
	int rid, err;

	sc->dev = dev;

	rid = 0;
	sc->mem_res = bus_alloc_resource_any(dev, SYS_RES_MEMORY, &rid,
		RF_ACTIVE);
	if (!sc->mem_res) {
		device_printf(dev, "cannot allocate memory window\n");
		err = ENXIO;
		goto fail;
	}

	/*
	 * These devices may be disabled by being held in reset. If this is the
	 * case, a read attempt in its register range will result in a CPU hang.
	 * Detect this situation and avoid probing the device in this situation.
	 */
#if defined(LIBBSP_AARCH64_XILINX_ZYNQMP_BSP_H)
	volatile uint32_t *RST_LPD_IOU2_ptr = (uint32_t*)0xFF5E0238;
	uint32_t RST_LPD_IOU2 = *RST_LPD_IOU2_ptr;
	uint32_t SDIO0_disabled = RST_LPD_IOU2 & (1 << 5);
	uint32_t SDIO1_disabled = RST_LPD_IOU2 & (1 << 6);
	if ( sc->mem_res == 0xFF160000 ) {
		if ( SDIO0_disabled != 0 ) {
			device_printf(dev, "SDIO0 disabled\n");
			err = ENXIO;
			goto fail;
		}
	} else {
		if ( SDIO1_disabled != 0 ) {
			device_printf(dev, "SDIO1 disabled\n");
			err = ENXIO;
			goto fail;
		}
	}
#endif

	rid = 0;
	sc->irq_res = bus_alloc_resource_any(dev, SYS_RES_IRQ, &rid,
		RF_ACTIVE);
	if (!sc->irq_res) {
		device_printf(dev, "cannot allocate interrupt\n");
		err = ENXIO;
		goto fail;
	}

	if (bus_setup_intr(dev, sc->irq_res, INTR_TYPE_BIO | INTR_MPSAFE,
		NULL, arasan_sdhci_intr, sc, &sc->intr_cookie)) {
		device_printf(dev, "cannot setup interrupt handler\n");
		err = ENXIO;
		goto fail;
	}

	/*
	 * There are some combinations of board routing and eMMC memory that are
	 * not compatible with the HISPD mode. This disables HISPD mode for
	 * compatibility.
	 */
	sc->slot.quirks |= SDHCI_QUIRK_DONT_SET_HISPD_BIT;

	/*
	 * DMA is not really broken, it just isn't implemented yet.
	 */
	sc->slot.quirks |= SDHCI_QUIRK_BROKEN_DMA;

	sdhci_init_slot(dev, &sc->slot, 0);
	sc->slot_init_done = true;

	bus_generic_probe(dev);
	bus_generic_attach(dev);

	sdhci_start_slot(&sc->slot);

	return (0);

fail:
	arasan_sdhci_detach(dev);
	return (err);
}

static int
arasan_sdhci_probe(device_t dev)
{
	device_set_desc(dev, "Arasan SDIO");

	return (0);
}

static device_method_t arasan_sdhci_methods[] = {
	/* Device interface */
	DEVMETHOD(device_probe,		arasan_sdhci_probe),
	DEVMETHOD(device_attach,	arasan_sdhci_attach),
	DEVMETHOD(device_detach,	arasan_sdhci_detach),

	/* Bus interface */
	DEVMETHOD(bus_read_ivar,	sdhci_generic_read_ivar),
	DEVMETHOD(bus_write_ivar,	sdhci_generic_write_ivar),

	/* MMC bridge interface */
	DEVMETHOD(mmcbr_update_ios,	sdhci_generic_update_ios),
	DEVMETHOD(mmcbr_request,	sdhci_generic_request),
	DEVMETHOD(mmcbr_get_ro,		arasan_sdhci_get_ro),
	DEVMETHOD(mmcbr_acquire_host,	sdhci_generic_acquire_host),
	DEVMETHOD(mmcbr_release_host,	sdhci_generic_release_host),

	/* SDHCI accessors */
	DEVMETHOD(sdhci_read_1,		arasan_sdhci_read_1),
	DEVMETHOD(sdhci_read_2,		arasan_sdhci_read_2),
	DEVMETHOD(sdhci_read_4,		arasan_sdhci_read_4),
	DEVMETHOD(sdhci_read_multi_4,	arasan_sdhci_read_multi_4),
	DEVMETHOD(sdhci_write_1,	arasan_sdhci_write_1),
	DEVMETHOD(sdhci_write_2,	arasan_sdhci_write_2),
	DEVMETHOD(sdhci_write_4,	arasan_sdhci_write_4),
	DEVMETHOD(sdhci_write_multi_4,	arasan_sdhci_write_multi_4),
	DEVMETHOD(sdhci_get_card_present,arasan_sdhci_get_card_present),

	DEVMETHOD_END
};

static devclass_t arasan_sdhci_devclass;

static driver_t arasan_sdhci_driver = {
	"arasan_sdhci",
	arasan_sdhci_methods,
	sizeof(struct arasan_sdhci_softc),
};

DRIVER_MODULE(arasan_sdhci, nexus, arasan_sdhci_driver, arasan_sdhci_devclass, 0, 0);
MODULE_DEPEND(arasan_sdhci, sdhci, 1, 1, 1);

#ifndef MMCCAM
MMC_DECLARE_BRIDGE(arasan_sdhci);
#endif
