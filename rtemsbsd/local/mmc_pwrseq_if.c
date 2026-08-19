#include <machine/rtems-bsd-kernel-space.h>
/*
 * This file is @generated automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   freebsd-org/sys/dev/mmc/mmc_pwrseq_if.m
 * with
 *   makeobjops.awk
 *
 * See the source file for legal information
 */

#include <sys/param.h>
#include <sys/queue.h>
#include <sys/kernel.h>
#include <sys/kobj.h>
#include <rtems/bsd/local/mmc_pwrseq_if.h>

struct kobjop_desc mmc_pwrseq_set_power_desc = {
	0, { &mmc_pwrseq_set_power_desc, (kobjop_t)kobj_error_method }
};

