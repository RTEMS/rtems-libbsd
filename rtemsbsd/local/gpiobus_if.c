#include <machine/rtems-bsd-kernel-space.h>

/*
 * This file is @generated automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   freebsd-org/sys/dev/gpio/gpiobus_if.m
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
#include <sys/gpio.h>
#include <rtems/bsd/local/gpiobus_if.h>

struct kobjop_desc gpiobus_acquire_bus_desc = {
	0, { &gpiobus_acquire_bus_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc gpiobus_release_bus_desc = {
	0, { &gpiobus_release_bus_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc gpiobus_pin_set_desc = {
	0, { &gpiobus_pin_set_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc gpiobus_pin_get_desc = {
	0, { &gpiobus_pin_get_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc gpiobus_pin_toggle_desc = {
	0, { &gpiobus_pin_toggle_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc gpiobus_pin_getcaps_desc = {
	0, { &gpiobus_pin_getcaps_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc gpiobus_pin_getflags_desc = {
	0, { &gpiobus_pin_getflags_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc gpiobus_pin_setflags_desc = {
	0, { &gpiobus_pin_setflags_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc gpiobus_pin_getname_desc = {
	0, { &gpiobus_pin_getname_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc gpiobus_pin_setname_desc = {
	0, { &gpiobus_pin_setname_desc, (kobjop_t)kobj_error_method }
};

