/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief TODO.
 */

/*
 * Copyright (c) 2009-2013 embedded brains GmbH.  All rights reserved.
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
#include <machine/rtems-bsd-thread.h>

#include <rtems/bsd/sys/param.h>
#include <rtems/bsd/sys/types.h>
#include <sys/systm.h>
#include <sys/bus.h>
#include <sys/kernel.h>
#include <sys/module.h>
#include <sys/rman.h>
#include <sys/malloc.h>

#include <rtems/bsd/bsd.h>
#include <rtems/irq-extension.h>

/* #define DISABLE_INTERRUPT_EXTENSION */

RTEMS_STATIC_ASSERT(SYS_RES_MEMORY == RTEMS_BSD_RES_MEMORY, RTEMS_BSD_RES_MEMORY);

RTEMS_STATIC_ASSERT(SYS_RES_IRQ == RTEMS_BSD_RES_IRQ, RTEMS_BSD_RES_IRQ);

static struct rman mem_rman;

static struct rman irq_rman;

static int
nexus_probe(device_t dev)
{
	rtems_status_code status;
	int err;
	size_t i;

	device_set_desc(dev, "RTEMS Nexus device");

#ifndef DISABLE_INTERRUPT_EXTENSION
	status = rtems_interrupt_server_initialize(
		BSD_TASK_PRIORITY_INTERRUPT,
		BSD_MINIMUM_TASK_STACK_SIZE,
		RTEMS_DEFAULT_MODES,
		RTEMS_DEFAULT_ATTRIBUTES,
		NULL
	);
	BSD_ASSERT(status == RTEMS_SUCCESSFUL);
#endif

	mem_rman.rm_start = 0;
	mem_rman.rm_end = ~0UL;
	mem_rman.rm_type = RMAN_ARRAY;
	mem_rman.rm_descr = "I/O memory addresses";
	err = rman_init(&mem_rman) != 0;
	BSD_ASSERT(err == 0);
	err = rman_manage_region(&mem_rman, mem_rman.rm_start, mem_rman.rm_end);
	BSD_ASSERT(err == 0);

	irq_rman.rm_start = 0;
	irq_rman.rm_end = ~0UL;
	irq_rman.rm_type = RMAN_ARRAY;
	irq_rman.rm_descr = "Interrupt vectors";
	err = rman_init(&irq_rman) != 0;
	BSD_ASSERT(err == 0);
	err = rman_manage_region(&irq_rman, irq_rman.rm_start, irq_rman.rm_end);
	BSD_ASSERT(err == 0);

	for (i = 0; i < rtems_bsd_nexus_device_count; ++i) {
		const rtems_bsd_device *nd = &rtems_bsd_nexus_devices[i];

		device_add_child(dev, nd->name, nd->unit);
	}

	return (0);
}

static bool
nexus_get_start(const rtems_bsd_device *nd, int type, u_long *start)
{
	u_long sr = *start;
	size_t i;

	for (i = 0; i < nd->resource_count; ++i) {
		const rtems_bsd_device_resource *dr = &nd->resources[i];

		if (dr->type == type && dr->start_request == sr) {
			*start = dr->start_actual;

			return (true);
		}
	}

	return (false);
}

static struct resource *
nexus_alloc_resource(device_t bus, device_t child, int type, int *rid,
    u_long start, u_long end, u_long count, u_int flags)
{
	struct rman *rm;
	size_t i;

	switch (type) {
	case SYS_RES_MEMORY:
		rm = &mem_rman;
		break;
	case SYS_RES_IRQ:
		rm = &irq_rman;
		break;
	default:
		return (NULL);
	}

	for (i = 0; i < rtems_bsd_nexus_device_count; ++i) {
		const rtems_bsd_device *nd = &rtems_bsd_nexus_devices[i];

		if (strcmp(device_get_name(child), nd->name) == 0
		    && device_get_unit(child) == nd->unit) {
			struct resource *res = NULL;

			if (nexus_get_start(nd, type, &start)) {
				res = rman_reserve_resource(rm, start, end,
				    count, flags, child);
				if (res != NULL) {
					rman_set_rid(res, *rid);
					rman_set_bushandle(res,
					    rman_get_start(res));
				}
			};

			return (res);
		}
	}

	return (NULL);
}

static int
nexus_release_resource(device_t bus, device_t child, int type, int rid,
    struct resource *res)
{
	return (rman_release_resource(res));
}

struct nexus_intr {
	driver_filter_t *filt;
	driver_intr_t *intr;
	void *arg;
};

static void
nexus_intr_with_filter(void *arg)
{
	struct nexus_intr *ni;
	int status;

	ni = arg;

	status = (*ni->filt)(ni->arg);
	if ((status & FILTER_SCHEDULE_THREAD) != 0) {
		(*ni->intr)(ni->arg);
	}
}

static int
nexus_setup_intr(device_t dev, device_t child, struct resource *res, int flags,
    driver_filter_t *filt, driver_intr_t *intr, void *arg, void **cookiep)
{
	int err;
#ifndef DISABLE_INTERRUPT_EXTENSION
	struct nexus_intr *ni;

	ni = malloc(sizeof(*ni), M_TEMP, M_WAITOK);
	if (ni != NULL) {
		rtems_status_code sc;
		rtems_interrupt_handler rh;
		void *ra;

		ni->filt = filt;
		ni->intr = intr;
		ni->arg = arg;

		*cookiep = ni;

		if (filt == NULL) {
			rh = intr;
			ra = arg;
		} else {
			rh = nexus_intr_with_filter;
			ra = ni;
		}

		sc = rtems_interrupt_server_handler_install(RTEMS_ID_NONE,
		    rman_get_start(res), device_get_nameunit(child),
		    RTEMS_INTERRUPT_UNIQUE, rh, ra);
		if (sc == RTEMS_SUCCESSFUL) {
			err = 0;
		} else {
			free(ni, M_TEMP);

			err = EINVAL;
		}
	} else {
		err = ENOMEM;
	}
#else
	err = EINVAL;
#endif

	return (err);
}

static int
nexus_teardown_intr(device_t dev, device_t child, struct resource *res,
    void *cookie)
{
	int err;
#ifndef DISABLE_INTERRUPT_EXTENSION
	struct nexus_intr *ni;
	rtems_status_code sc;
	rtems_interrupt_handler rh;
	void *ra;

	ni = cookie;

	if (ni->filt == NULL) {
		rh = ni->intr;
		ra = ni->arg;
	} else {
		rh = nexus_intr_with_filter;
		ra = ni->arg;
	}

	sc = rtems_interrupt_server_handler_install(RTEMS_ID_NONE,
	    rman_get_start(res), device_get_nameunit(child),
	    RTEMS_INTERRUPT_UNIQUE, rh, ra);
	err = sc == RTEMS_SUCCESSFUL ? 0 : EINVAL;
#else
	err = EINVAL;
#endif

	return (err);
}

static device_method_t nexus_methods[] = {
	/* Device interface */
	DEVMETHOD(device_probe, nexus_probe),
	DEVMETHOD(device_attach, bus_generic_attach),
	DEVMETHOD(device_detach, bus_generic_detach),
	DEVMETHOD(device_shutdown, bus_generic_shutdown),
	DEVMETHOD(device_suspend, bus_generic_suspend),
	DEVMETHOD(device_resume, bus_generic_resume),

	/* Bus interface */
	DEVMETHOD(bus_print_child, bus_generic_print_child),
	DEVMETHOD(bus_add_child, bus_generic_add_child),
	DEVMETHOD(bus_alloc_resource, nexus_alloc_resource),
	DEVMETHOD(bus_release_resource, nexus_release_resource),
	DEVMETHOD(bus_setup_intr, nexus_setup_intr),
	DEVMETHOD(bus_teardown_intr, nexus_teardown_intr),

	{ 0, 0 }
};

static driver_t nexus_driver = {
	.name = "nexus",
	.methods = nexus_methods,
	.size = 0
};

static devclass_t nexus_devclass;

DRIVER_MODULE(nexus, root, nexus_driver, nexus_devclass, 0, 0);
