#include <machine/rtems-bsd-kernel-space.h>
/*
 * This file is @generated automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   freebsd-org/sys/dev/extres/regulator/regdev_if.m
 * with
 *   makeobjops.awk
 *
 * See the source file for legal information
 */

#include <sys/param.h>
#include <sys/queue.h>
#include <sys/kernel.h>
#include <sys/kobj.h>
#include <sys/types.h>
#include <dev/ofw/ofw_bus.h>
#include <machine/bus.h>
#include <rtems/bsd/local/regdev_if.h>

struct kobjop_desc regdev_map_desc = {
	0, { &regdev_map_desc, (kobjop_t)regdev_default_ofw_map }
};

