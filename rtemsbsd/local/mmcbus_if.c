#include <machine/rtems-bsd-kernel-space.h>
/*
 * This file is @generated automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   freebsd-org/sys/dev/mmc/mmcbus_if.m
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
#include <dev/mmc/mmcreg.h>
#include <rtems/bsd/local/mmcbus_if.h>

struct kobjop_desc mmcbus_retune_pause_desc = {
	0, { &mmcbus_retune_pause_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc mmcbus_retune_unpause_desc = {
	0, { &mmcbus_retune_unpause_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc mmcbus_wait_for_request_desc = {
	0, { &mmcbus_wait_for_request_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc mmcbus_acquire_bus_desc = {
	0, { &mmcbus_acquire_bus_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc mmcbus_release_bus_desc = {
	0, { &mmcbus_release_bus_desc, (kobjop_t)kobj_error_method }
};

