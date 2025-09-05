#include <machine/rtems-bsd-kernel-space.h>
/*
 * This file is @generated automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   freebsd-org/sys/dev/usb/usb_if.m
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
#include <rtems/bsd/local/usb_if.h>

struct kobjop_desc usb_handle_request_desc = {
	0, { &usb_handle_request_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc usb_take_controller_desc = {
	0, { &usb_take_controller_desc, (kobjop_t)kobj_error_method }
};

