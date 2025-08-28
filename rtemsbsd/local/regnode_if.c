#include <machine/rtems-bsd-kernel-space.h>
/*
 * This file is @generated automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   freebsd-org/sys/dev/extres/regulator/regnode_if.m
 * with
 *   makeobjops.awk
 *
 * See the source file for legal information
 */

#include <sys/param.h>
#include <sys/queue.h>
#include <sys/kernel.h>
#include <sys/kobj.h>
#include <rtems/bsd/local/regnode_if.h>


static int
regnode_default_stop(struct regnode *regnode, int *udelay)
{

	return(REGNODE_ENABLE(regnode, false, udelay));
}

struct kobjop_desc regnode_init_desc = {
	0, { &regnode_init_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc regnode_enable_desc = {
	0, { &regnode_enable_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc regnode_status_desc = {
	0, { &regnode_status_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc regnode_set_voltage_desc = {
	0, { &regnode_set_voltage_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc regnode_get_voltage_desc = {
	0, { &regnode_get_voltage_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc regnode_check_voltage_desc = {
	0, { &regnode_check_voltage_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc regnode_stop_desc = {
	0, { &regnode_stop_desc, (kobjop_t)regnode_default_stop }
};

