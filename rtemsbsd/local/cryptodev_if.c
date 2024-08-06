#include <machine/rtems-bsd-kernel-space.h>

/*
 * This file is @generated automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   freebsd-org/sys/opencrypto/cryptodev_if.m
 * with
 *   makeobjops.awk
 *
 * See the source file for legal information
 */

#include <sys/param.h>
#include <sys/queue.h>
#include <sys/kernel.h>
#include <sys/kobj.h>
#include <sys/malloc.h>
#include <opencrypto/cryptodev.h>
#include <rtems/bsd/local/cryptodev_if.h>


static int null_freesession(device_t dev,
    crypto_session_t crypto_session)
{
	return 0;
}

struct kobjop_desc cryptodev_probesession_desc = {
	0, { &cryptodev_probesession_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc cryptodev_newsession_desc = {
	0, { &cryptodev_newsession_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc cryptodev_freesession_desc = {
	0, { &cryptodev_freesession_desc, (kobjop_t)null_freesession }
};

struct kobjop_desc cryptodev_process_desc = {
	0, { &cryptodev_process_desc, (kobjop_t)kobj_error_method }
};

