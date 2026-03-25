#include <machine/rtems-bsd-kernel-space.h>
/*
 * Copyright (c) 2026 Aaron Nyholm <aaron.nyholm@unfoldedeffective.com>.
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

#include <sys/systm.h>
#include <sys/malloc.h>
#include <sys/sysctl.h>
#include <sys/kernel.h>
#include <sys/rman.h>
#include <sys/module.h>
#include <sys/bus.h>
#include <sys/endian.h>
#include <sys/mutex.h>
#include <sys/proc.h>

#include <rtems/rtems/intr.h>

#include <machine/intr.h>
#include <machine/bus.h>

#include <dev/ofw/openfirm.h>
#include <dev/ofw/ofw_bus.h>
#include <dev/ofw/ofw_bus_subr.h>

#include <rtems/bsd/local/ofw_bus_if.h>
#include <rtems/bsd/local/msi_if.h>
#include <rtems/bsd/local/pic_if.h>

struct rtems_pic_softc {
    device_t dev;
    struct intr_pic* rtems_pic;
    struct rtems_pic_irqsrc* isrcs;
};

struct rtems_pic_irqsrc {
    struct intr_irqsrc isrc;
    rtems_interrupt_entry rie;
    u_int irq;
    bool allocated;
    device_t dev;
};

static struct ofw_compat_data compat_data[] = {
    {"arm,gic-400", true},
    { NULL, 0 },
};

static int rtems_pic_probe(device_t dev) {
    if (!ofw_bus_status_okay(dev))
        return (ENXIO);

    if (ofw_bus_search_compatible(dev, compat_data)->ocd_data == 0)
        return (ENXIO);

    device_set_desc(dev, "RTEMS Portable Interrupt Controller");

    return (BUS_PROBE_DEFAULT);
}

static int rtems_pic_register_irq(struct rtems_pic_softc* sc) {
    const char* name;
    int error;
    int irq;

    sc->isrcs = malloc(sizeof(*sc->isrcs) * NIRQ,
        M_DEVBUF, M_WAITOK | M_ZERO);

    name = device_get_nameunit(sc->dev);

    for (irq = 0; irq < BSP_INTERRUPT_VECTOR_COUNT; irq++) {
        sc->isrcs[irq].irq = irq;
        sc->isrcs[irq].dev = sc->dev;
        error = intr_isrc_register(&sc->isrcs[irq].isrc, sc->dev, 0, "%s,%u",
            name, irq);
        if (error != 0) {
            return error;
        }
    }

    return 0;
}

static void rtems_pic_intr_handler(void* arg) {
    struct rtems_pic_irqsrc* isrc;
    struct rtems_pic_softc* sc;

    isrc = (struct rtems_pic_irqsrc*)arg;
    sc = device_get_softc(isrc->dev);

    rtems_interrupt_vector_disable(isrc->irq);
    if (intr_isrc_dispatch(&isrc->isrc, NULL) != 0) {
        device_printf(sc->dev, "disabled irq %u: stray\n", isrc->irq);
    }
}

static int rtems_pic_attach(device_t dev) {
    struct rtems_pic_softc* sc;
    phandle_t xref;
    int error;

    sc = device_get_softc(dev);
    sc->dev = dev;

    error = rtems_pic_register_irq(sc);
    if (error != 0) {
        return error;
    }

    xref = OF_xref_from_node(ofw_bus_get_node(dev));
    sc->rtems_pic = intr_pic_register(dev, xref);

    OF_device_register_xref(xref, dev);
    error = intr_pic_claim_root(dev, xref, NULL, sc);
    if (error != 0) {
        return ENXIO;
    }

    return bus_generic_attach(dev);
}

static void rtems_pic_disable_intr(device_t dev,
    struct intr_irqsrc *isrc) {
}

static void rtems_pic_enable_intr(device_t dev,
    struct intr_irqsrc *isrc) {
    rtems_status_code sc;
    struct rtems_pic_irqsrc* rtems_isrc = (struct rtems_pic_irqsrc*)isrc;

    sc = rtems_interrupt_entry_install(
        rtems_isrc->irq,
        RTEMS_INTERRUPT_SHARED,
        &rtems_isrc->rie
    );
    if (sc != RTEMS_SUCCESSFUL) {
        return;
    }
}

static int rtems_pic_map_intr(device_t dev, struct intr_map_data *data,
    struct intr_irqsrc **isrcp) {
    struct rtems_pic_softc* sc;
    int irq;
    const char* nameunit;
#ifdef FDT
    struct intr_map_data_fdt* daf;
#endif /* FDT */
    struct intr_map_data_rtems* dar;

    sc = device_get_softc(dev);

    switch (data->type) {
    case INTR_MAP_DATA_FDT:
        daf = (struct intr_map_data_fdt*)data;
        irq = ((int)bsp_fdt_map_intr(daf->cells, daf->ncells));
        break;
    case INTR_MAP_DATA_RTEMS:
        dar = (struct intr_map_data_rtems*)data;
        irq = dar->irq;
        break;
    default:
        return -1;
    }

    if (sc->isrcs[irq].allocated) {
       return EBUSY;
    }
    sc->isrcs[irq].allocated = true;
    *isrcp = (struct intr_irqsrc*)(&sc->isrcs[irq]);

    return 0;
}

static int rtems_pic_setup_intr(device_t dev, struct intr_irqsrc *isrc,
    struct resource* res, struct intr_map_data* data) {
    struct rtems_pic_irqsrc* rtems_isrc = (struct rtems_pic_irqsrc*)isrc;

    rtems_interrupt_entry_initialize(
        &rtems_isrc->rie,
        rtems_pic_intr_handler,
        rtems_isrc,
        device_get_nameunit(rman_get_device(res))
    );

    return 0;
}

static int rtems_pic_teardown_intr(device_t dev, struct intr_irqsrc *isrc,
    struct resource* res, struct intr_map_data* data) {
    return 0;
}

static void rtems_pic_post_filter(device_t dev,
    struct intr_irqsrc *isrc) {
}

static void rtems_pic_post_ithread(device_t dev,
    struct intr_irqsrc *isrc) {
    struct rtems_pic_irqsrc* risrc;

    risrc = (struct rtems_pic_irqsrc*)isrc;
    rtems_interrupt_vector_enable(risrc->irq);
}

static void rtems_pic_pre_ithread(device_t dev,
    struct intr_irqsrc *isrc) {
}

static device_method_t rtems_pic_methods[] = {
	/* Device interface */
	DEVMETHOD(device_probe,	rtems_pic_probe),
	DEVMETHOD(device_attach,	rtems_pic_attach),

	/* Interrupt controller interface */
	DEVMETHOD(pic_disable_intr,	rtems_pic_disable_intr),
	DEVMETHOD(pic_enable_intr,	rtems_pic_enable_intr),
	DEVMETHOD(pic_map_intr,		rtems_pic_map_intr),
	DEVMETHOD(pic_setup_intr,	rtems_pic_setup_intr),
	DEVMETHOD(pic_teardown_intr,	rtems_pic_teardown_intr),
	DEVMETHOD(pic_post_filter,	rtems_pic_post_filter),
	DEVMETHOD(pic_post_ithread,	rtems_pic_post_ithread),
	DEVMETHOD(pic_pre_ithread,	rtems_pic_pre_ithread),
};

DEFINE_CLASS_0(rtems_pic, rtems_pic_driver, rtems_pic_methods,
    sizeof(struct rtems_pic_softc));

EARLY_DRIVER_MODULE(rtems_pic, simplebus, rtems_pic_driver, 0, 0,
    BUS_PASS_INTERRUPT + BUS_PASS_ORDER_MIDDLE);
EARLY_DRIVER_MODULE(rtems_pic, ofwbus, rtems_pic_driver, 0, 0,
    BUS_PASS_INTERRUPT + BUS_PASS_ORDER_MIDDLE);
EARLY_DRIVER_MODULE(rtems_pic, nexus, rtems_pic_driver, 0, 0,
    BUS_PASS_INTERRUPT + BUS_PASS_ORDER_MIDDLE);
