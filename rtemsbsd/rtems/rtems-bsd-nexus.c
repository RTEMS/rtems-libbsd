/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief TODO.
 */

/*
 * Copyright (c) 2009, 2010 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
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

#include <freebsd/machine/rtems-bsd-config.h>
#include <freebsd/machine/rtems-bsd-sysinit.h>

#include <freebsd/sys/param.h>
#include <freebsd/sys/types.h>
#include <freebsd/sys/systm.h>
#include <freebsd/sys/bus.h>
#include <freebsd/sys/kernel.h>
#include <freebsd/sys/module.h>
#include <freebsd/sys/rman.h>
#include <freebsd/sys/malloc.h>

#include <bsp.h>
#include <freebsd/machine/rtems-bsd-devicet.h>
#include <bsp/irq.h>
#include <rtems/irq.h>
#include <rtems/irq-extension.h>
#include <freebsd/machine/bus.h>

/* XXX Note:  These defines should be moved. */
#if defined(__i386__) 
  #define	BUS_SPACE_IO	I386_BUS_SPACE_IO
  #define	BUS_SPACE_MEM	I386_BUS_SPACE_MEM
#elif defined(__amd64__)
  #define	BUS_SPACE_IO	AMD64_BUS_SPACE_IO
  #define	BUS_SPACE_MEM	AMD64_BUS_SPACE_MEM
#else
  #warning "Bus space information not implemented for this architecture!!"
  #warning "Defaulting Bus space information!!"
  #define	BUS_SPACE_IO    0	
  #define	BUS_SPACE_MEM	1
#endif

/* XXX - Just a guess */
#define NUM_IO_INTS   30 

#define DEVTONX(dev)	((struct nexus_device *)device_get_ivars(dev))

static MALLOC_DEFINE(M_NEXUSDEV, "nexusdev", "Nexus device");
struct rman irq_rman, drq_rman, port_rman, mem_rman;
struct nexus_device {
        struct resource_list    nx_resources;
};

void
nexus_init_resources(void)
{
	int irq;

	/*
	 * XXX working notes:
	 *
	 * - IRQ resource creation should be moved to the PIC/APIC driver.
	 * - DRQ resource creation should be moved to the DMAC driver.
	 * - The above should be sorted to probe earlier than any child busses.
	 *
	 * - Leave I/O and memory creation here, as child probes may need them.
	 *   (especially eg. ACPI)
	 */

	/*
	 * IRQ's are on the mainboard on old systems, but on the ISA part
	 * of PCI->ISA bridges.  There would be multiple sets of IRQs on
	 * multi-ISA-bus systems.  PCI interrupts are routed to the ISA
	 * component, so in a way, PCI can be a partial child of an ISA bus(!).
	 * APIC interrupts are global though.
	 */
	irq_rman.rm_start = 0;
	irq_rman.rm_type = RMAN_ARRAY;
	irq_rman.rm_descr = "Interrupt request lines";
	irq_rman.rm_end = NUM_IO_INTS - 1;
	if (rman_init(&irq_rman))
		panic("nexus_init_resources irq_rman");
	/*
	 * We search for regions of existing IRQs and add those to the IRQ
	 * resource manager.
	 */
	for (irq = 0; irq < NUM_IO_INTS; irq++)
          if (rman_manage_region(&irq_rman, irq, irq) != 0)
	    panic("nexus_init_resources irq_rmand");
 
	/*
	 * ISA DMA on PCI systems is implemented in the ISA part of each
	 * PCI->ISA bridge and the channels can be duplicated if there are
	 * multiple bridges.  (eg: laptops with docking stations)
	 */
	drq_rman.rm_start = 0;
#ifdef PC98
	drq_rman.rm_end = 3;
#else
	drq_rman.rm_end = 7;
#endif
	drq_rman.rm_type = RMAN_ARRAY;
	drq_rman.rm_descr = "DMA request lines";
	/* XXX drq 0 not available on some machines */
	if (rman_init(&drq_rman)
	    || rman_manage_region(&drq_rman,
				  drq_rman.rm_start, drq_rman.rm_end))
		panic("nexus_init_resources drq_rman");

	/*
	 * However, IO ports and Memory truely are global at this level,
	 * as are APIC interrupts (however many IO APICS there turn out
	 * to be on large systems..)
	 */
	port_rman.rm_start = 0;
	port_rman.rm_end = 0xffff;
	port_rman.rm_type = RMAN_ARRAY;
	port_rman.rm_descr = "I/O ports";
	if (rman_init(&port_rman)
	    || rman_manage_region(&port_rman, 0, 0xffff))
		panic("nexus_init_resources port_rman");

	mem_rman.rm_start = 0;
	mem_rman.rm_end = ~0u;
	mem_rman.rm_type = RMAN_ARRAY;
	mem_rman.rm_descr = "I/O memory addresses";
	if (rman_init(&mem_rman)
	    || rman_manage_region(&mem_rman, 0, ~0))
		panic("nexus_init_resources mem_rman");
}

static int
nexus_attach(device_t dev)
{

	nexus_init_resources();
	bus_generic_probe(dev);

	/*
	 * Explicitly add the legacy0 device here.  Other platform
	 * types (such as ACPI), use their own nexus(4) subclass
	 * driver to override this routine and add their own root bus.
	 */
	if (BUS_ADD_CHILD(dev, 10, "legacy", 0) == NULL)
		panic("legacy: could not attach");
	bus_generic_attach(dev);
	return 0;
}

static int
nexus_probe(device_t dev)
{
	size_t unit = 0;
#if 0
	/* FIXME */
	for (unit = 0; _bsd_nexus_devices [unit] != NULL; ++unit) {
		device_add_child(dev, _bsd_nexus_devices [unit], unit);
	}
#endif
	device_set_desc(dev, "RTEMS Nexus device");

	return (0);
}
static device_t
nexus_add_child(device_t bus, u_int order, const char *name, int unit)
{
	device_t		child;
	struct nexus_device	*ndev;

	ndev = malloc(sizeof(struct nexus_device), M_NEXUSDEV, M_NOWAIT|M_ZERO);
	if (!ndev)
		return(0);
	resource_list_init(&ndev->nx_resources);

	child = device_add_child_ordered(bus, order, name, unit);

	/* should we free this in nexus_child_detached? */
	device_set_ivars(child, ndev);

	return(child);
}

/*
 * Allocate a resource on behalf of child.  NB: child is usually going to be a
 * child of one of our descendants, not a direct child of nexus0.
 * (Exceptions include npx.)
 */
static struct resource *
nexus_alloc_resource(device_t bus, device_t child, int type, int *rid,
		     u_long start, u_long end, u_long count, u_int flags)
{
	struct nexus_device *ndev = DEVTONX(child);
	struct	resource *rv;
	struct resource_list_entry *rle;
	struct	rman *rm;
	int needactivate = flags & RF_ACTIVE;

	/*
	 * If this is an allocation of the "default" range for a given RID, and
	 * we know what the resources for this device are (ie. they aren't maintained
	 * by a child bus), then work out the start/end values.
	 */
	if ((start == 0UL) && (end == ~0UL) && (count == 1)) {
		if (ndev == NULL)
			return(NULL);
		rle = resource_list_find(&ndev->nx_resources, type, *rid);
		if (rle == NULL)
			return(NULL);
		start = rle->start;
		end = rle->end;
		count = rle->count;
	}

	flags &= ~RF_ACTIVE;

	switch (type) {
	case SYS_RES_IRQ:
		rm = &irq_rman;
		break;

	case SYS_RES_DRQ:
		rm = &drq_rman;
		break;

	case SYS_RES_IOPORT:
		rm = &port_rman;
		break;

	case SYS_RES_MEMORY:
		rm = &mem_rman;
		break;

	default:
		return 0;
	}

	rv = rman_reserve_resource(rm, start, end, count, flags, child);
	if (rv == 0)
		return 0;
	rman_set_rid(rv, *rid);

	if (needactivate) {
		if (bus_activate_resource(child, type, *rid, rv)) {
			rman_release_resource(rv);
			return 0;
		}
	}

	return rv;
}

static int
nexus_activate_resource(device_t bus, device_t child, int type, int rid,
			struct resource *r)
{
#ifdef PC98
	bus_space_handle_t bh;
	int error;
#endif
	void *vaddr;

	/*
	 * If this is a memory resource, map it into the kernel.
	 */
	switch (type) {
	case SYS_RES_IOPORT:
#ifdef PC98
		error = i386_bus_space_handle_alloc(I386_BUS_SPACE_IO,
		    rman_get_start(r), rman_get_size(r), &bh);
		if (error)
			return (error);
		rman_set_bushandle(r, bh);
#else
		rman_set_bushandle(r, rman_get_start(r));
#endif
		rman_set_bustag(r, BUS_SPACE_IO);
		break;
	case SYS_RES_MEMORY:
#ifdef PC98
		error = i386_bus_space_handle_alloc(I386_BUS_SPACE_MEM,
		    rman_get_start(r), rman_get_size(r), &bh);
		if (error)
			return (error);
#endif
		rman_set_bustag(r, BUS_SPACE_MEM);
#ifdef PC98
		/* PC-98: the type of bus_space_handle_t is the structure. */
		bh->bsh_base = (bus_addr_t) vaddr;
		rman_set_bushandle(r, bh);
#else
		/* IBM-PC: the type of bus_space_handle_t is u_int */
		rman_set_bushandle(r, (bus_space_handle_t) vaddr);
#endif
	}
	return (rman_activate_resource(r));
}

static int
nexus_deactivate_resource(device_t bus, device_t child, int type, int rid,
			  struct resource *r)
{

#ifdef PC98
	if (type == SYS_RES_MEMORY || type == SYS_RES_IOPORT) {
		bus_space_handle_t bh;

		bh = rman_get_bushandle(r);
		i386_bus_space_handle_free(rman_get_bustag(r), bh, bh->bsh_sz);
	}
#endif
	return (rman_deactivate_resource(r));
}

static int
nexus_release_resource(device_t bus, device_t child, int type, int rid,
		       struct resource *r)
{
	if (rman_get_flags(r) & RF_ACTIVE) {
		int error = bus_deactivate_resource(child, type, rid, r);
		if (error)
			return error;
	}
	return (rman_release_resource(r));
}

static void noop(const rtems_irq_connect_data *unused) {};
static int  noop1(const rtems_irq_connect_data *unused) { return 0;};

static int
bspExtInstallSharedISR(int irqLine, void (*isr)(void *), void * uarg, int flags)
{
  return rtems_interrupt_handler_install(
    irqLine, 
    "BSD Interrupt", 
    RTEMS_INTERRUPT_SHARED, 
    isr, 
    uarg 
  );
}

int
intr_add_handler(const char *name, int vector, driver_filter_t filter,
    driver_intr_t handler, void *arg, enum intr_type flags, void **cookiep)
{
  int  rval;
  
  rval = bspExtInstallSharedISR(vector, handler, arg, 0);
  return rval;
}

static int
nexus_setup_intr(device_t bus, device_t child, struct resource *irq,
		 int flags, driver_filter_t filter, void (*ihand)(void *),
		 void *arg, void **cookiep)
{
	int		error;

	/* somebody tried to setup an irq that failed to allocate! */
	if (irq == NULL)
		panic("nexus_setup_intr: NULL irq resource!");

	*cookiep = 0;
	if ((rman_get_flags(irq) & RF_SHAREABLE) == 0)
		flags |= INTR_EXCL;

	/*
	 * We depend here on rman_activate_resource() being idempotent.
	 */
	error = rman_activate_resource(irq);
	if (error)
		return (error);

	error = intr_add_handler(device_get_nameunit(child),
	    rman_get_start(irq), filter, ihand, arg, flags, cookiep);

	return (error);
}


static device_method_t nexus_methods [] = {
	/* Device interface */
	DEVMETHOD(device_probe, nexus_probe),
	DEVMETHOD(device_attach, nexus_attach),
	DEVMETHOD(device_detach, bus_generic_detach),
	DEVMETHOD(device_shutdown, bus_generic_shutdown),
	DEVMETHOD(device_suspend, bus_generic_suspend),
	DEVMETHOD(device_resume, bus_generic_resume),

	/* Bus interface */
	DEVMETHOD(bus_print_child, bus_generic_print_child),
	DEVMETHOD(bus_add_child,	nexus_add_child),
	DEVMETHOD(bus_alloc_resource,	nexus_alloc_resource),
	DEVMETHOD(bus_release_resource,	nexus_release_resource),
	DEVMETHOD(bus_activate_resource, nexus_activate_resource),
	DEVMETHOD(bus_deactivate_resource, nexus_deactivate_resource),
	DEVMETHOD(bus_setup_intr,	nexus_setup_intr),

	{ 0, 0 }
};

static driver_t nexus_driver = {
	.name = "nexus",
	.methods = nexus_methods,
	.size = 0
};

static devclass_t nexus_devclass;

DRIVER_MODULE(nexus, root, nexus_driver, nexus_devclass, 0, 0);
