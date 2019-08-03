#include <machine/rtems-bsd-kernel-space.h>

/*
 * This file is @generated automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   freebsd-org/sys/dev/hdmi/hdmi_if.m
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
#include <dev/videomode/videomode.h>
#include <dev/videomode/edidvar.h>
#include <rtems/bsd/local/hdmi_if.h>

struct kobjop_desc hdmi_get_edid_desc = {
	0, { &hdmi_get_edid_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc hdmi_set_videomode_desc = {
	0, { &hdmi_set_videomode_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc hdmi_enable_desc = {
	0, { &hdmi_enable_desc, (kobjop_t)kobj_error_method }
};

