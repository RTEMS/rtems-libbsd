#include <machine/rtems-bsd-kernel-space.h>

/*
 * This file is produced automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   freebsd-org/sys/dev/ofw/ofw_bus_if.m
 * with
 *   makeobjops.awk
 *
 * See the source file for legal information
 */

#include <sys/param.h>
#include <sys/queue.h>
#include <sys/kernel.h>
#include <sys/kobj.h>
#include <sys/bus.h>
#include <dev/ofw/openfirm.h>
#include <rtems/bsd/local/ofw_bus_if.h>


static ofw_bus_get_devinfo_t ofw_bus_default_get_devinfo;
static ofw_bus_get_compat_t ofw_bus_default_get_compat;
static ofw_bus_get_model_t ofw_bus_default_get_model;
static ofw_bus_get_name_t ofw_bus_default_get_name;
static ofw_bus_get_node_t ofw_bus_default_get_node;
static ofw_bus_get_type_t ofw_bus_default_get_type;
static ofw_bus_map_intr_t ofw_bus_default_map_intr;

static const struct ofw_bus_devinfo *
ofw_bus_default_get_devinfo(device_t bus, device_t dev)
{

	return (NULL);
}

static const char *
ofw_bus_default_get_compat(device_t bus, device_t dev)
{

	return (NULL);
}

static const char *
ofw_bus_default_get_model(device_t bus, device_t dev)
{

	return (NULL);
}

static const char *
ofw_bus_default_get_name(device_t bus, device_t dev)
{

	return (NULL);
}

static phandle_t
ofw_bus_default_get_node(device_t bus, device_t dev)
{

	return (-1);
}

static const char *
ofw_bus_default_get_type(device_t bus, device_t dev)
{

	return (NULL);
}

int
ofw_bus_default_map_intr(device_t bus, device_t dev, phandle_t iparent,
    int icells, pcell_t *interrupt)
{
	/* Propagate up the bus hierarchy until someone handles it. */	
	if (device_get_parent(bus) != NULL)
		return OFW_BUS_MAP_INTR(device_get_parent(bus), dev,
		    iparent, icells, interrupt);

	/* If that fails, then assume a one-domain system */
	return (interrupt[0]);
}

struct kobjop_desc ofw_bus_get_devinfo_desc = {
	0, { &ofw_bus_get_devinfo_desc, (kobjop_t)ofw_bus_default_get_devinfo }
};

struct kobjop_desc ofw_bus_get_compat_desc = {
	0, { &ofw_bus_get_compat_desc, (kobjop_t)ofw_bus_default_get_compat }
};

struct kobjop_desc ofw_bus_get_model_desc = {
	0, { &ofw_bus_get_model_desc, (kobjop_t)ofw_bus_default_get_model }
};

struct kobjop_desc ofw_bus_get_name_desc = {
	0, { &ofw_bus_get_name_desc, (kobjop_t)ofw_bus_default_get_name }
};

struct kobjop_desc ofw_bus_get_node_desc = {
	0, { &ofw_bus_get_node_desc, (kobjop_t)ofw_bus_default_get_node }
};

struct kobjop_desc ofw_bus_get_type_desc = {
	0, { &ofw_bus_get_type_desc, (kobjop_t)ofw_bus_default_get_type }
};

struct kobjop_desc ofw_bus_map_intr_desc = {
	0, { &ofw_bus_map_intr_desc, (kobjop_t)ofw_bus_default_map_intr }
};

