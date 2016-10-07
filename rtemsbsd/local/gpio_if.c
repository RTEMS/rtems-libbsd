#include <machine/rtems-bsd-kernel-space.h>

/*
 * This file is produced automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   freebsd-org/sys/dev/gpio/gpio_if.m
 * with
 *   makeobjops.awk
 *
 * See the source file for legal information
 */

#include <rtems/bsd/sys/param.h>
#include <sys/queue.h>
#include <sys/kernel.h>
#include <sys/kobj.h>
#include <sys/bus.h>
#include <sys/gpio.h>
#include <rtems/bsd/local/gpio_if.h>


static device_t
gpio_default_get_bus(void)
{

	return (NULL);
}

static int
gpio_default_map_gpios(device_t bus, phandle_t dev,
    phandle_t gparent, int gcells, pcell_t *gpios, uint32_t *pin,
    uint32_t *flags)
{
	/* Propagate up the bus hierarchy until someone handles it. */  
	if (device_get_parent(bus) != NULL)
		return (GPIO_MAP_GPIOS(device_get_parent(bus), dev,
		    gparent, gcells, gpios, pin, flags));

	/* If that fails, then assume the FreeBSD defaults. */
	*pin = gpios[0];
	if (gcells == 2 || gcells == 3)
		*flags = gpios[gcells - 1];

	return (0);
}

struct kobjop_desc gpio_get_bus_desc = {
	0, { &gpio_get_bus_desc, (kobjop_t)gpio_default_get_bus }
};

struct kobjop_desc gpio_pin_max_desc = {
	0, { &gpio_pin_max_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc gpio_pin_set_desc = {
	0, { &gpio_pin_set_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc gpio_pin_get_desc = {
	0, { &gpio_pin_get_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc gpio_pin_toggle_desc = {
	0, { &gpio_pin_toggle_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc gpio_pin_getcaps_desc = {
	0, { &gpio_pin_getcaps_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc gpio_pin_getflags_desc = {
	0, { &gpio_pin_getflags_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc gpio_pin_getname_desc = {
	0, { &gpio_pin_getname_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc gpio_pin_setflags_desc = {
	0, { &gpio_pin_setflags_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc gpio_map_gpios_desc = {
	0, { &gpio_map_gpios_desc, (kobjop_t)gpio_default_map_gpios }
};

