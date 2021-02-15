/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief TODO.
 */

/*
 * Copyright (c) 2009, 2017 embedded brains GmbH.  All rights reserved.
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

#include <sys/param.h>
#include <sys/types.h>
#include <sys/systm.h>
#include <sys/bus.h>
#include <sys/kernel.h>
#include <sys/module.h>
#include <sys/rman.h>
#include <sys/malloc.h>
#include <machine/bus.h>

#include <rtems/bsd/local/opt_platform.h>

#ifdef FDT
#include <dev/ofw/ofw_bus.h>
#endif

#include <rtems/bsd/bsd.h>
#include <rtems/irq-extension.h>

#include <bsp.h>

/*
 * BSP PCI Support
 *
 * The RTEMS Nexus bus support can optionaly support PCI spaces that
 * map to BSP speciic address spaces. The BSP needs to provide the
 * following:
 *
 * RTEMS_BSP_PCI_IO_REGION_BASE
 *   The base address of the IO port region of the address space
 *
 * RTEMS_BSP_PCI_MEM_REGION_BASE
 *  The base address of the memory region of the address space
 *
 * i386 (x86) BSPs have a special bus.h file and do not use these settings.
 */

/* #define DISABLE_INTERRUPT_EXTENSION */

#if defined(__i386__)
#define RTEMS_BSP_PCI_IO_REGION_BASE 0
#endif

#if !defined(RTEMS_BSP_PCI_MEM_REGION_BASE)
#define RTEMS_BSP_PCI_MEM_REGION_BASE 0
#endif

#if defined(RTEMS_BSP_PCI_IO_REGION_BASE) || defined(FDT)
#define ENABLE_RESOURCE_ACTIVATE_DEACTIVATE
#endif

RTEMS_BSD_DECLARE_SET(nexus, rtems_bsd_device);

RTEMS_BSD_DEFINE_SET(nexus, rtems_bsd_device);

RTEMS_STATIC_ASSERT(SYS_RES_MEMORY == RTEMS_BSD_RES_MEMORY, RTEMS_BSD_RES_MEMORY);

RTEMS_STATIC_ASSERT(SYS_RES_IRQ == RTEMS_BSD_RES_IRQ, RTEMS_BSD_RES_IRQ);

static struct rman mem_rman;

static struct rman irq_rman;

#if defined(RTEMS_BSP_PCI_IO_REGION_BASE)
static struct rman port_rman;
#endif

#ifndef DISABLE_INTERRUPT_EXTENSION
SYSINIT_REFERENCE(irqs);
#endif

static int
nexus_probe(device_t dev)
{
	int err;
	const rtems_bsd_device *nd;

	device_set_desc(dev, "RTEMS Nexus device");

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

#if defined(RTEMS_BSP_PCI_IO_REGION_BASE)
	port_rman.rm_start = 0;
	port_rman.rm_end = ~0UL;
	port_rman.rm_type = RMAN_ARRAY;
	port_rman.rm_descr = "I/O ports";
	err = rman_init(&port_rman) != 0;
	BSD_ASSERT(err == 0);
	err = rman_manage_region(&port_rman, port_rman.rm_start,
	    port_rman.rm_end);
	BSD_ASSERT(err == 0);
#endif

	SET_FOREACH(nd, nexus) {
		device_add_child(dev, nd->name, nd->unit);
	}

	return (0);
}

static bool
nexus_get_start(const rtems_bsd_device *nd, int type, rman_res_t *start)
{
	u_long sr = (u_long)*start;
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
    rman_res_t start, rman_res_t end, rman_res_t count, u_int flags)
{
	struct resource *res = NULL;
	struct rman *rm;
	const rtems_bsd_device *nd;
	rman_res_t base = RTEMS_BSP_PCI_MEM_REGION_BASE;

	switch (type) {
	case SYS_RES_MEMORY:
		rm = &mem_rman;
		break;
	case SYS_RES_IRQ:
		rm = &irq_rman;
		break;
#if defined(RTEMS_BSP_PCI_IO_REGION_BASE)
	case SYS_RES_IOPORT:
		rm = &port_rman;
		base = RTEMS_BSP_PCI_IO_REGION_BASE;
		break;
#endif
	default:
		return (res);
	}

	SET_FOREACH(nd, nexus) {
		const char *name;

		name = device_get_name(child);
		if (name != NULL && strcmp(name, nd->name) == 0
		    && device_get_unit(child) == nd->unit) {
			if (nexus_get_start(nd, type, &start)) {
				res = rman_reserve_resource(rm, start, end,
				    count, flags, child);
				if (res != NULL) {
					rman_set_rid(res, *rid);
					rman_set_bushandle(res,
					    rman_get_start(res) + base);
				}
			};

			return (res);
		}
	}

#if defined(RTEMS_BSP_PCI_IO_REGION_BASE)
	/*
	 * FIXME: This is a quick and dirty hack.  Simply reserve resources of
	 * this kind.  See also pci_reserve_map().
	 */
	if (start + count - end <= 1UL) {
		res = rman_reserve_resource(rm, start, end, count, flags,
		    child);
		if (res != NULL) {
			rman_set_rid(res, *rid);
			rman_set_bushandle(res, rman_get_start(res));
		}
	}
#endif

	return (res);
}

static int
nexus_release_resource(device_t bus, device_t child, int type, int rid,
    struct resource *res)
{
	return (rman_release_resource(res));
}

#ifdef ENABLE_RESOURCE_ACTIVATE_DEACTIVATE
static int
nexus_activate_resource(device_t bus, device_t child, int type, int rid,
    struct resource *res)
{

	switch (type) {
#if defined(RTEMS_BSP_PCI_IO_REGION_BASE)
	case SYS_RES_IOPORT:
#ifdef __i386__
		rman_set_bustag(res, X86_BUS_SPACE_IO);
#else
		rman_set_bushandle(res,
		   rman_get_start(res) + RTEMS_BSP_PCI_IO_REGION_BASE);
#endif
		break;
#endif
	case SYS_RES_MEMORY:
#ifdef __i386__
		rman_set_bustag(res, X86_BUS_SPACE_MEM);
#else
		rman_set_bushandle(res,
		   rman_get_start(res) + RTEMS_BSP_PCI_MEM_REGION_BASE);
#endif
		break;
	}
	return (rman_activate_resource(res));
}

static int
nexus_deactivate_resource(device_t bus, device_t child, int type, int rid,
    struct resource *res)
{

	return (rman_deactivate_resource(res));
}
#endif

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
		    RTEMS_INTERRUPT_SHARED, rh, ra);
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
	    RTEMS_INTERRUPT_SHARED, rh, ra);
	err = sc == RTEMS_SUCCESSFUL ? 0 : EINVAL;
#else
	err = EINVAL;
#endif

	return (err);
}

#ifdef FDT
static int
nexus_ofw_map_intr(device_t dev, device_t child, phandle_t iparent, int icells,
    pcell_t *intr)
{

	return ((int)bsp_fdt_map_intr(intr, (size_t)icells));
}
#endif /* FDT */

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
#ifdef ENABLE_RESOURCE_ACTIVATE_DEACTIVATE
	DEVMETHOD(bus_activate_resource, nexus_activate_resource),
	DEVMETHOD(bus_deactivate_resource, nexus_deactivate_resource),
#endif
	DEVMETHOD(bus_setup_intr, nexus_setup_intr),
	DEVMETHOD(bus_teardown_intr, nexus_teardown_intr),

#ifdef FDT
	/* OFW interface */
	DEVMETHOD(ofw_bus_map_intr, nexus_ofw_map_intr),
#endif

	{ 0, 0 }
};

static driver_t nexus_driver = {
	.name = "nexus",
	.methods = nexus_methods,
	.size = 0
};

static devclass_t nexus_devclass;

EARLY_DRIVER_MODULE_ORDERED(nexus, root, nexus_driver, nexus_devclass, 0, 0,
	SI_ORDER_FIRST, BUS_PASS_BUS + BUS_PASS_ORDER_FIRST);
