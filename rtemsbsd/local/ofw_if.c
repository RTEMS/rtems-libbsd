#include <machine/rtems-bsd-kernel-space.h>
/*
 * This file is @generated automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   freebsd-org/sys/dev/ofw/ofw_if.m
 * with
 *   makeobjops.awk
 *
 * See the source file for legal information
 */

#include <sys/param.h>
#include <sys/queue.h>
#include <sys/kernel.h>
#include <sys/kobj.h>
#include <dev/ofw/openfirm.h>
#include <dev/ofw/ofwvar.h>
#include <rtems/bsd/local/ofw_if.h>

struct kobjop_desc ofw_init_desc = {
	0, { &ofw_init_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc ofw_peer_desc = {
	0, { &ofw_peer_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc ofw_parent_desc = {
	0, { &ofw_parent_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc ofw_child_desc = {
	0, { &ofw_child_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc ofw_instance_to_package_desc = {
	0, { &ofw_instance_to_package_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc ofw_getproplen_desc = {
	0, { &ofw_getproplen_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc ofw_getprop_desc = {
	0, { &ofw_getprop_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc ofw_nextprop_desc = {
	0, { &ofw_nextprop_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc ofw_setprop_desc = {
	0, { &ofw_setprop_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc ofw_canon_desc = {
	0, { &ofw_canon_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc ofw_finddevice_desc = {
	0, { &ofw_finddevice_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc ofw_instance_to_path_desc = {
	0, { &ofw_instance_to_path_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc ofw_package_to_path_desc = {
	0, { &ofw_package_to_path_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc ofw_test_desc = {
	0, { &ofw_test_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc ofw_call_method_desc = {
	0, { &ofw_call_method_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc ofw_interpret_desc = {
	0, { &ofw_interpret_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc ofw_open_desc = {
	0, { &ofw_open_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc ofw_close_desc = {
	0, { &ofw_close_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc ofw_read_desc = {
	0, { &ofw_read_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc ofw_write_desc = {
	0, { &ofw_write_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc ofw_seek_desc = {
	0, { &ofw_seek_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc ofw_claim_desc = {
	0, { &ofw_claim_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc ofw_release_desc = {
	0, { &ofw_release_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc ofw_enter_desc = {
	0, { &ofw_enter_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc ofw_exit_desc = {
	0, { &ofw_exit_desc, (kobjop_t)kobj_error_method }
};

