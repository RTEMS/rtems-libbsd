#include <machine/rtems-bsd-kernel-space.h>
/*
 * This file is @generated automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   freebsd-org/sys/kern/pic_if.m
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
#include <sys/cpuset.h>
#include <sys/resource.h>
#include <sys/intr.h>
#include <rtems/bsd/local/pic_if.h>


static int
dflt_pic_bind_intr(device_t dev, struct intr_irqsrc *isrc)
{

	return (EOPNOTSUPP);
}

static int
null_pic_activate_intr(device_t dev, struct intr_irqsrc *isrc,
    struct resource *res, struct intr_map_data *data)
{

	return (0);
}

static int
null_pic_deactivate_intr(device_t dev, struct intr_irqsrc *isrc,
    struct resource *res, struct intr_map_data *data)
{

	return (0);
}

static int
null_pic_setup_intr(device_t dev, struct intr_irqsrc *isrc,
    struct resource *res, struct intr_map_data *data)
{

	return (0);
}

static int
null_pic_teardown_intr(device_t dev, struct intr_irqsrc *isrc,
    struct resource *res, struct intr_map_data *data)
{

	return (0);
}

static void
null_pic_init_secondary(device_t dev)
{
}

static void
null_pic_ipi_send(device_t dev, cpuset_t cpus, u_int ipi)
{
}

static int
dflt_pic_ipi_setup(device_t dev, u_int ipi, struct intr_irqsrc *isrc)
{

	return (EOPNOTSUPP);
}

struct kobjop_desc pic_activate_intr_desc = {
	0, { &pic_activate_intr_desc, (kobjop_t)null_pic_activate_intr }
};

struct kobjop_desc pic_bind_intr_desc = {
	0, { &pic_bind_intr_desc, (kobjop_t)dflt_pic_bind_intr }
};

struct kobjop_desc pic_disable_intr_desc = {
	0, { &pic_disable_intr_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc pic_enable_intr_desc = {
	0, { &pic_enable_intr_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc pic_map_intr_desc = {
	0, { &pic_map_intr_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc pic_deactivate_intr_desc = {
	0, { &pic_deactivate_intr_desc, (kobjop_t)null_pic_deactivate_intr }
};

struct kobjop_desc pic_setup_intr_desc = {
	0, { &pic_setup_intr_desc, (kobjop_t)null_pic_setup_intr }
};

struct kobjop_desc pic_teardown_intr_desc = {
	0, { &pic_teardown_intr_desc, (kobjop_t)null_pic_teardown_intr }
};

struct kobjop_desc pic_post_filter_desc = {
	0, { &pic_post_filter_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc pic_post_ithread_desc = {
	0, { &pic_post_ithread_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc pic_pre_ithread_desc = {
	0, { &pic_pre_ithread_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc pic_init_secondary_desc = {
	0, { &pic_init_secondary_desc, (kobjop_t)null_pic_init_secondary }
};

struct kobjop_desc pic_ipi_send_desc = {
	0, { &pic_ipi_send_desc, (kobjop_t)null_pic_ipi_send }
};

struct kobjop_desc pic_ipi_setup_desc = {
	0, { &pic_ipi_setup_desc, (kobjop_t)dflt_pic_ipi_setup }
};

