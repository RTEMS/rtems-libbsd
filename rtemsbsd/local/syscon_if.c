#include <machine/rtems-bsd-kernel-space.h>

/*
 * This file is @generated automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   freebsd-org/sys/dev/extres/syscon/syscon_if.m
 * with
 *   makeobjops.awk
 *
 * See the source file for legal information
 */

#include <sys/param.h>
#include <sys/queue.h>
#include <sys/kernel.h>
#include <sys/kobj.h>
#include <machine/bus.h>
#include <rtems/bsd/local/syscon_if.h>


#include <sys/systm.h>
#include <sys/bus.h>

int
syscon_get_handle_default(device_t dev, struct syscon **syscon)
{
	device_t parent;

	parent = device_get_parent(dev);
	if (parent == NULL)
		return (ENODEV);
	return (SYSCON_GET_HANDLE(parent, syscon));
}

static void
syscon_device_lock_default(device_t dev)
{

	panic("syscon_device_lock is not implemented");
};

static void
syscon_device_unlock_default(device_t dev)
{

	panic("syscon_device_unlock is not implemented");
};

struct kobjop_desc syscon_init_desc = {
	0, { &syscon_init_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc syscon_uninit_desc = {
	0, { &syscon_uninit_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc syscon_read_4_desc = {
	0, { &syscon_read_4_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc syscon_write_4_desc = {
	0, { &syscon_write_4_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc syscon_modify_4_desc = {
	0, { &syscon_modify_4_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc syscon_unlocked_read_4_desc = {
	0, { &syscon_unlocked_read_4_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc syscon_unlocked_write_4_desc = {
	0, { &syscon_unlocked_write_4_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc syscon_unlocked_modify_4_desc = {
	0, { &syscon_unlocked_modify_4_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc syscon_device_lock_desc = {
	0, { &syscon_device_lock_desc, (kobjop_t)syscon_device_lock_default }
};

struct kobjop_desc syscon_device_unlock_desc = {
	0, { &syscon_device_unlock_desc, (kobjop_t)syscon_device_unlock_default }
};

struct kobjop_desc syscon_get_handle_desc = {
	0, { &syscon_get_handle_desc, (kobjop_t)syscon_get_handle_default }
};

