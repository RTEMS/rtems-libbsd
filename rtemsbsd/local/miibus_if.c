#include <machine/rtems-bsd-kernel-space.h>

/*
 * This file is produced automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   dev/mii/miibus_if.m
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
#include <rtems/bsd/local/miibus_if.h>

struct kobjop_desc miibus_readreg_desc = {
	0, { &miibus_readreg_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc miibus_writereg_desc = {
	0, { &miibus_writereg_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc miibus_statchg_desc = {
	0, { &miibus_statchg_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc miibus_linkchg_desc = {
	0, { &miibus_linkchg_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc miibus_mediainit_desc = {
	0, { &miibus_mediainit_desc, (kobjop_t)kobj_error_method }
};

