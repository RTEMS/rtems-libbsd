#include <machine/rtems-bsd-kernel-space.h>

/*
 * This file is produced automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   freebsd-org/sys/dev/fb/fb_if.m
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
#include <sys/fbio.h>
#include <rtems/bsd/local/fb_if.h>

struct kobjop_desc fb_pin_max_desc = {
	0, { &fb_pin_max_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc fb_getinfo_desc = {
	0, { &fb_getinfo_desc, (kobjop_t)kobj_error_method }
};

