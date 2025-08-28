#include <machine/rtems-bsd-kernel-space.h>
/*
 * This file is @generated automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   freebsd-org/sys/dev/mmc/mmcbr_if.m
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
#include <rtems/bsd/local/mmcbr_if.h>


static int
null_switch_vccq(device_t brdev __unused, device_t reqdev __unused)
{

	return (0);
}

static int
null_retune(device_t brdev __unused, device_t reqdev __unused,
    bool reset __unused)
{

	return (0);
}

static int
null_tune(device_t brdev __unused, device_t reqdev __unused,
    bool hs400 __unused)
{

	return (0);
}

struct kobjop_desc mmcbr_update_ios_desc = {
	0, { &mmcbr_update_ios_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc mmcbr_switch_vccq_desc = {
	0, { &mmcbr_switch_vccq_desc, (kobjop_t)null_switch_vccq }
};

struct kobjop_desc mmcbr_tune_desc = {
	0, { &mmcbr_tune_desc, (kobjop_t)null_tune }
};

struct kobjop_desc mmcbr_retune_desc = {
	0, { &mmcbr_retune_desc, (kobjop_t)null_retune }
};

struct kobjop_desc mmcbr_request_desc = {
	0, { &mmcbr_request_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc mmcbr_get_ro_desc = {
	0, { &mmcbr_get_ro_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc mmcbr_acquire_host_desc = {
	0, { &mmcbr_acquire_host_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc mmcbr_release_host_desc = {
	0, { &mmcbr_release_host_desc, (kobjop_t)kobj_error_method }
};

