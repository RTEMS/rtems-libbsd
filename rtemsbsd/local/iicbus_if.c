#include <machine/rtems-bsd-kernel-space.h>

/*
 * This file is produced automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   freebsd-org/sys/dev/iicbus/iicbus_if.m
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
#include <dev/iicbus/iic.h>
#include <rtems/bsd/local/iicbus_if.h>


static int iicbus_nosupport(void)
{

	return (ENODEV);
}

static u_int
iicbus_default_frequency(device_t bus, u_char speed)
{

	return (100000);
}

struct kobjop_desc iicbus_intr_desc = {
	0, { &iicbus_intr_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc iicbus_callback_desc = {
	0, { &iicbus_callback_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc iicbus_repeated_start_desc = {
	0, { &iicbus_repeated_start_desc, (kobjop_t)iicbus_nosupport }
};

struct kobjop_desc iicbus_start_desc = {
	0, { &iicbus_start_desc, (kobjop_t)iicbus_nosupport }
};

struct kobjop_desc iicbus_stop_desc = {
	0, { &iicbus_stop_desc, (kobjop_t)iicbus_nosupport }
};

struct kobjop_desc iicbus_read_desc = {
	0, { &iicbus_read_desc, (kobjop_t)iicbus_nosupport }
};

struct kobjop_desc iicbus_write_desc = {
	0, { &iicbus_write_desc, (kobjop_t)iicbus_nosupport }
};

struct kobjop_desc iicbus_reset_desc = {
	0, { &iicbus_reset_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc iicbus_transfer_desc = {
	0, { &iicbus_transfer_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc iicbus_get_frequency_desc = {
	0, { &iicbus_get_frequency_desc, (kobjop_t)iicbus_default_frequency }
};

