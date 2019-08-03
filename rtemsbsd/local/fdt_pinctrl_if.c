#include <machine/rtems-bsd-kernel-space.h>

/*
 * This file is produced automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   freebsd-org/sys/dev/fdt/fdt_pinctrl_if.m
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
#include <sys/bus.h>
#include <dev/ofw/openfirm.h>
#include <rtems/bsd/local/fdt_pinctrl_if.h>

struct kobjop_desc fdt_pinctrl_configure_desc = {
	0, { &fdt_pinctrl_configure_desc, (kobjop_t)kobj_error_method }
};

