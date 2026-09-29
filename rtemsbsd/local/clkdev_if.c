#include <machine/rtems-bsd-kernel-space.h>

/*
 * This file is @generated automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   freebsd-org/sys/dev/extres/clk/clkdev_if.m
 * with
 *   makeobjops.awk
 *
 * See the source file for legal information
 */

#include <sys/param.h>
#include <sys/queue.h>
#include <sys/kernel.h>
#include <sys/kobj.h>
#include <machine/bus.h>
#include <rtems/bsd/local/clkdev_if.h>


#include <sys/systm.h>
#include <sys/bus.h>
static int
clkdev_default_write_4(device_t dev, bus_addr_t addr, uint32_t val)
{
	device_t pdev;

	pdev = device_get_parent(dev);
	if (pdev == NULL)
		return (ENXIO);

	return (CLKDEV_WRITE_4(pdev, addr, val));
}

static int
clkdev_default_read_4(device_t dev, bus_addr_t addr, uint32_t *val)
{
	device_t pdev;

	pdev = device_get_parent(dev);
	if (pdev == NULL)
		return (ENXIO);

	return (CLKDEV_READ_4(pdev, addr, val));
}

static int
clkdev_default_modify_4(device_t dev, bus_addr_t addr,
    uint32_t clear_mask, uint32_t set_mask)
{
	device_t pdev;

	pdev = device_get_parent(dev);
	if (pdev == NULL)
		return (ENXIO);

	return (CLKDEV_MODIFY_4(pdev, addr, clear_mask, set_mask));
}

static void
clkdev_default_device_lock(device_t dev)
{
	device_t pdev;

	pdev = device_get_parent(dev);
	if (pdev == NULL)
		panic("clkdev_device_lock not implemented");

	CLKDEV_DEVICE_LOCK(pdev);
}

static void
clkdev_default_device_unlock(device_t dev)
{
	device_t pdev;

	pdev = device_get_parent(dev);
	if (pdev == NULL)
		panic("clkdev_device_unlock not implemented");

	CLKDEV_DEVICE_UNLOCK(pdev);
}

struct kobjop_desc clkdev_write_4_desc = {
	0, { &clkdev_write_4_desc, (kobjop_t)clkdev_default_write_4 }
};

struct kobjop_desc clkdev_read_4_desc = {
	0, { &clkdev_read_4_desc, (kobjop_t)clkdev_default_read_4 }
};

struct kobjop_desc clkdev_modify_4_desc = {
	0, { &clkdev_modify_4_desc, (kobjop_t)clkdev_default_modify_4 }
};

struct kobjop_desc clkdev_device_lock_desc = {
	0, { &clkdev_device_lock_desc, (kobjop_t)clkdev_default_device_lock }
};

struct kobjop_desc clkdev_device_unlock_desc = {
	0, { &clkdev_device_unlock_desc, (kobjop_t)clkdev_default_device_unlock }
};
