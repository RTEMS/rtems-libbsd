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

#include <sys/buf.h>
#include <sys/bus.h>
#include <sys/cdefs.h>
#include <sys/conf.h>
#include <sys/kernel.h>
#include <sys/module.h>
#include <sys/param.h>
#include <sys/rman.h>
#include <sys/systm.h>

#include <dev/pci/pcireg.h>
#include <dev/pci/pcivar.h>

#include <machine/bus.h>

#include <rtems/bsd/pci-iodev.h>
#include <rtems/timespec.h>

#include <dev/io/iodev.h>

#define PCI_IODEV_MAX_REGIONS (PCIR_MAX_BAR_0 + 1)

#define PCI_IODEV_EVENT_TRIGGER RTEMS_EVENT_0

static const char *region_names[] = {
    "BAR0",
    "BAR1",
    "BAR2",
    "BAR3",
    "BAR4",
    "BAR5"
};

struct pci_iodev_event {
    struct pci_iodev_softc *sc;
    int resource_id;
    struct resource *resource;
    char name[8];
    bool pending;
    rtems_id waiter;
    void *cookie;
};

struct pci_iodev_region_info {
    int bar;
    int resource_id;
    struct resource *resource;
};

struct pci_iodev_softc {
    device_t dev;
    rtems_iodev *iodev;
    struct pci_iodev_region_info regions_info[PCI_IODEV_MAX_REGIONS];
    rtems_iodev_region regions[PCI_IODEV_MAX_REGIONS];
    struct pci_iodev_event *events;
    size_t region_count;
    size_t event_count;
    rtems_interrupt_lock ilock;
};

static int
pci_iodev_get_device_info(
    rtems_iodev *iodev,
    void *arg
)
{
    device_t dev;
    struct pci_iodev_info *info;

    if (arg == NULL || iodev == NULL) {
        return EINVAL;
    }

    info = (struct pci_iodev_info*)arg;
    dev = iodev->driver;

    info->devid = pci_get_devid(dev);
    info->vendor = pci_get_vendor(dev);
    info->class_code = pci_get_class(dev);
    info->subclass_code = pci_get_subclass(dev);
    info->progif = pci_get_progif(dev);
    info->rev = pci_get_revid(dev);
    info->subvendor = pci_get_subvendor(dev);
    info->subdevice = pci_get_subdevice(dev);

    return 0;
}

static int
pci_iodev_get_event_count(
    rtems_iodev *iodev,
    size_t *event_count
)
{
    device_t dev;
    struct pci_iodev_softc *sc;
    if (iodev == NULL) {
        return EINVAL;
    }

    dev = iodev->driver;
    sc = device_get_softc(dev);

    if (event_count == NULL) {
        return EINVAL;
    }
    *event_count = sc->event_count;

    return 0;
}

static int
pci_iodev_get_event_info(
    rtems_iodev *iodev,
    struct rtems_iodev_event_info *info
)
{
    device_t dev;
    struct pci_iodev_softc *sc;
    if (info == NULL || iodev == NULL) {
        return EINVAL;
    }

    dev = iodev->driver;
    sc = device_get_softc(dev);

    if (info->index >= sc->event_count) {
        return EINVAL;
    }

    info->name = sc->events[info->index].name;
    info->args = NULL;

    return 0;
}

static int
pci_iodev_event_wait(
    rtems_iodev *iodev,
    struct rtems_iodev_event_args *args
)
{
    device_t dev;
    struct pci_iodev_softc *sc;
    struct pci_iodev_event *evt;
    rtems_status_code status;
    rtems_interrupt_lock_context lock_context;
    rtems_event_set event_out;
    rtems_interval timeout;
    bool pending = false;
    int error = 0;
    if (args == NULL || iodev == NULL) {
        return EINVAL;
    }

    dev = iodev->driver;
    sc = device_get_softc(dev);

    if (args->index >= sc->event_count) {
        return EINVAL;
    }

    if (args->timeout.tv_sec == 0 && args->timeout.tv_nsec == 0) {
        timeout = RTEMS_NO_TIMEOUT;
    } else {
        timeout = rtems_timespec_to_ticks(&args->timeout);
    }

    evt = &sc->events[args->index];
    rtems_interrupt_lock_acquire(&sc->ilock, &lock_context);
    if (evt->pending) {
        evt->pending = false;
        pending = true;
    } else {
        if (evt->waiter == RTEMS_ID_NONE) {
            evt->waiter = rtems_task_self();
        } else {
            error = EBUSY;
        }
    }
    rtems_interrupt_lock_release(&sc->ilock, &lock_context);

    if (error) {
        return error;
    }

    if (!pending) {
        status = rtems_event_system_receive(
            PCI_IODEV_EVENT_TRIGGER,
            RTEMS_WAIT,
            timeout,
            &event_out
        );
    }

    rtems_interrupt_lock_acquire(&sc->ilock, &lock_context);
    evt->waiter = RTEMS_ID_NONE;
    rtems_interrupt_lock_release(&sc->ilock, &lock_context);

    args->timedout = false;
    if (status == RTEMS_TIMEOUT) {
        args->timedout = true;
    }

    return error;
}

static int
pci_iodev_get_regions(
    rtems_iodev *iodev,
    rtems_iodev_region **regions
)
{
    device_t dev;
    struct pci_iodev_softc *sc;
    if (regions == NULL || iodev == NULL) {
        return EINVAL;
    }

    dev = iodev->driver;
    sc = device_get_softc(dev);

    *regions = sc->regions;
}

static int
pci_iodev_get_region_count(
    rtems_iodev *iodev,
    size_t *region_count
)
{
    device_t dev;
    struct pci_iodev_softc *sc;
    if (iodev == NULL) {
        return EINVAL;
    }

    dev = iodev->driver;
    sc = device_get_softc(dev);

    if (region_count == NULL) {
        return EINVAL;
    }
    *region_count = sc->region_count;

    return 0;
}

static int
pci_iodev_probe(device_t device)
{
    return BUS_PROBE_HOOVER;
}

static int
pci_iodev_filter_handler(void *arg)
{
    struct pci_iodev_event *event = arg;
    struct pci_iodev_softc *sc = event->sc;
    rtems_interrupt_lock_context lock_context;

    rtems_interrupt_lock_acquire(&sc->ilock, &lock_context);

    if (event->waiter != RTEMS_ID_NONE) {
        rtems_event_system_send(event->waiter, PCI_IODEV_EVENT_TRIGGER);
    } else {
        event->pending = true;
    }

    rtems_interrupt_lock_release(&sc->ilock, &lock_context);

    return FILTER_HANDLED;
}

static int
pci_iodev_attach(device_t dev)
{
    struct pci_iodev_softc* sc;
    int status;
    int msi_count;
    char name_buf[64];

    sc = device_get_softc(dev);

    sc->dev = dev;
    sc->iodev = rtems_iodev_alloc_and_init(sizeof(rtems_iodev));

    if (sc->iodev == NULL) {
        return ENOMEM;
    }

    sc->region_count = 0;
    for (int bar_id = 0; bar_id < PCI_IODEV_MAX_REGIONS; bar_id++) {
        struct resource *bar_res;

        sc->regions_info[sc->region_count].bar = bar_id;
        sc->regions_info[sc->region_count].resource_id = PCIR_BAR(bar_id);

        bar_res = bus_alloc_resource_any(
            sc->dev,
            SYS_RES_MEMORY,
            &sc->regions_info[sc->region_count].resource_id,
            RF_ACTIVE
        );
        sc->regions_info[sc->region_count].resource = bar_res;

        if(bar_res != NULL) {
            sc->regions[sc->region_count].index = sc->region_count;
            sc->regions[sc->region_count].address = rman_get_virtual(bar_res);
            sc->regions[sc->region_count].size = rman_get_size(bar_res);
            sc->regions[sc->region_count].name = region_names[bar_id];

            sc->region_count++;
        }
    }

    rtems_interrupt_lock_initialize(&sc->ilock, device_get_nameunit(sc->dev));

    msi_count = pci_msi_count(dev);
    if (pci_alloc_msi(dev, &msi_count) != 0) {
        for (int index = 0; index < sc->region_count; index++) {
            bus_release_resource(sc->dev, sc->regions_info[index].resource);
        }
        rtems_interrupt_lock_destroy(&sc->ilock);
        rtems_iodev_destroy_unregistered(sc->iodev);
        return ENOMEM;
    }
    sc->event_count = msi_count;

    sc->events = malloc(sizeof(struct pci_iodev_event) * sc->event_count,
        M_DEVBUF, M_NOWAIT | M_ZERO);
    if (sc->events == NULL) {
        for (int index = 0; index < sc->region_count; index++) {
            bus_release_resource(sc->dev, sc->regions_info[index].resource);
        }
        rtems_interrupt_lock_destroy(&sc->ilock);
        rtems_iodev_destroy_unregistered(sc->iodev);
        if (sc->event_count > 0) {
            pci_release_msi(dev);
        }
        return ENOMEM;
    }

    for (int i = 0; i < sc->event_count; i++) {
        /* MSI resource ids start at 1 */
        sc->events[i].resource_id = (i + 1);

        sc->events[i].resource = bus_alloc_resource_any(sc->dev, SYS_RES_IRQ,
            &sc->events[i].resource_id, RF_SHAREABLE | RF_ACTIVE);

        if (sc->events[i].resource == NULL) {
            for (int index = 0; index < sc->region_count; index++) {
                bus_release_resource(sc->dev, sc->regions_info[index].resource);
            }
            rtems_interrupt_lock_destroy(&sc->ilock);
            rtems_iodev_destroy_unregistered(sc->iodev);
            for (int index = 0; index < i; index++) {
                bus_teardown_intr(
                    sc->dev,
                    sc->events[index].resource,
                    sc->events[index].cookie
                );
                bus_release_resource(sc->dev, sc->events[index].resource);
            }
            if (sc->event_count > 0) {
                pci_release_msi(dev);
            }
            return ENOMEM;
        }
        status = bus_setup_intr(sc->dev, sc->events[i].resource,
            INTR_TYPE_MISC | INTR_MPSAFE, pci_iodev_filter_handler, NULL,
            &sc->events[i], &sc->events[i].cookie);
        if (status != 0) {
            for (int index = 0; index < sc->region_count; index++) {
                bus_release_resource(sc->dev, sc->regions_info[index].resource);
            }
            rtems_interrupt_lock_destroy(&sc->ilock);
            rtems_iodev_destroy_unregistered(sc->iodev);
            bus_release_resource(sc->dev, sc->events[i].resource);
            for (int index = 0; index < i; index++) {
                bus_teardown_intr(
                    sc->dev,
                    sc->events[index].resource,
                    sc->events[index].cookie
                );
                bus_release_resource(sc->dev, sc->events[index].resource);
            }
            if (sc->event_count > 0) {
                pci_release_msi(dev);
            }
            return ENOMEM;
        }

        sc->events[i].sc = sc;
        snprintf(sc->events[i].name, sizeof(sc->events[i].name), "MSI%d",
            sc->events[i].resource_id);
    }

    pci_enable_busmaster(dev);

    sc->iodev->get_device_info = pci_iodev_get_device_info;
    sc->iodev->event_wait = pci_iodev_event_wait;
    sc->iodev->get_regions = pci_iodev_get_regions;
    sc->iodev->get_region_count = pci_iodev_get_region_count;
    sc->iodev->get_event_count = pci_iodev_get_event_count;
    sc->iodev->get_event_info = pci_iodev_get_event_info;
    sc->iodev->priv_destroy = NULL;
    sc->iodev->driver = dev;

    snprintf(name_buf, sizeof(name_buf), "/dev/%s", device_get_nameunit(sc->dev));
    status = rtems_iodev_register(sc->iodev, name_buf);
}

static int
pci_iodev_detach(device_t dev)
{
    struct pci_iodev_softc* sc = device_get_softc(dev);

    for (int index = 0; index < sc->region_count; index++) {
        bus_release_resource(sc->dev, sc->regions_info[index].resource);
    }
    rtems_interrupt_lock_destroy(&sc->ilock);
    for (int index = 0; index < sc->event_count; index++) {
        bus_teardown_intr(
            sc->dev,
            sc->events[index].resource,
            sc->events[index].cookie
        );
        bus_release_resource(sc->dev, sc->events[index].resource);
    }
    if (sc->event_count > 0) {
        pci_release_msi(dev);
    }
}

static device_method_t pci_iodev_methods[] = {
    DEVMETHOD(device_probe,     pci_iodev_probe),
    DEVMETHOD(device_attach,    pci_iodev_attach),
    DEVMETHOD(device_detach,    pci_iodev_detach),
    { 0, 0 }
};

static driver_t pci_iodev_driver = {
    "pci_iodev",
    pci_iodev_methods,
    sizeof(struct pci_iodev_softc),
};

DRIVER_MODULE(pci_iodev, pci, pci_iodev_driver, NULL, NULL);
