#include <machine/rtems-bsd-kernel-space.h>

/*
 * This file is @generated automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   freebsd-org/sys/dev/hid/hid_if.m
 * with
 *   makeobjops.awk
 *
 * See the source file for legal information
 */

#include <sys/param.h>
#include <sys/queue.h>
#include <sys/kernel.h>
#include <sys/kobj.h>
#include <sys/param.h>
#include <sys/bus.h>
#include <sys/kernel.h>
#include <sys/malloc.h>
#include <dev/hid/hid.h>
#include <rtems/bsd/local/hid_if.h>

struct kobjop_desc hid_intr_setup_desc = {
	0, { &hid_intr_setup_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc hid_intr_unsetup_desc = {
	0, { &hid_intr_unsetup_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc hid_intr_start_desc = {
	0, { &hid_intr_start_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc hid_intr_stop_desc = {
	0, { &hid_intr_stop_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc hid_intr_poll_desc = {
	0, { &hid_intr_poll_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc hid_get_rdesc_desc = {
	0, { &hid_get_rdesc_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc hid_read_desc = {
	0, { &hid_read_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc hid_write_desc = {
	0, { &hid_write_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc hid_get_report_desc = {
	0, { &hid_get_report_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc hid_set_report_desc = {
	0, { &hid_set_report_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc hid_set_idle_desc = {
	0, { &hid_set_idle_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc hid_set_protocol_desc = {
	0, { &hid_set_protocol_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc hid_ioctl_desc = {
	0, { &hid_ioctl_desc, (kobjop_t)kobj_error_method }
};

