#include <machine/rtems-bsd-kernel-space.h>

/*
 * This file is @generated automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   sys/dev/clk/clknode_if.m
 * with
 *   makeobjops.awk
 *
 * See the source file for legal information
 */

#include <sys/param.h>
#include <sys/queue.h>
#include <sys/kernel.h>
#include <sys/kobj.h>
#include <rtems/bsd/local/clknode_if.h>

struct kobjop_desc clknode_init_desc = {
	0, { &clknode_init_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc clknode_recalc_freq_desc = {
	0, { &clknode_recalc_freq_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc clknode_set_freq_desc = {
	0, { &clknode_set_freq_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc clknode_set_gate_desc = {
	0, { &clknode_set_gate_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc clknode_get_gate_desc = {
	0, { &clknode_get_gate_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc clknode_set_mux_desc = {
	0, { &clknode_set_mux_desc, (kobjop_t)kobj_error_method }
};

