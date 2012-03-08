#include <rtems/freebsd/machine/rtems-bsd-config.h>

/*
 * This file is produced automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   opencrypto/cryptodev_if.m
 * with
 *   makeobjops.awk
 *
 * See the source file for legal information
 */

#include <rtems/freebsd/sys/param.h>
#include <rtems/freebsd/sys/queue.h>
#include <rtems/freebsd/sys/kernel.h>
#include <rtems/freebsd/sys/kobj.h>
#include <rtems/freebsd/sys/malloc.h>
#include <rtems/freebsd/opencrypto/cryptodev.h>
#include <rtems/freebsd/local/cryptodev_if.h>

struct kobj_method cryptodev_newsession_method_default = {
	&cryptodev_newsession_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc cryptodev_newsession_desc = {
	0, &cryptodev_newsession_method_default
};

struct kobj_method cryptodev_freesession_method_default = {
	&cryptodev_freesession_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc cryptodev_freesession_desc = {
	0, &cryptodev_freesession_method_default
};

struct kobj_method cryptodev_process_method_default = {
	&cryptodev_process_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc cryptodev_process_desc = {
	0, &cryptodev_process_method_default
};

struct kobj_method cryptodev_kprocess_method_default = {
	&cryptodev_kprocess_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc cryptodev_kprocess_desc = {
	0, &cryptodev_kprocess_method_default
};

