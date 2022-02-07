#include <machine/rtems-bsd-kernel-space.h>

/*
 * This file is produced automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   xdma_if.m
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
#include <dev/fdt/fdt_common.h>
#include <dev/ofw/ofw_bus.h>
#include <dev/ofw/ofw_bus_subr.h>
#include <dev/xdma/xdma.h>
#include <rtems/bsd/local/xdma_if.h>

struct kobjop_desc xdma_channel_request_desc = {
	0, { &xdma_channel_request_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc xdma_channel_prep_sg_desc = {
	0, { &xdma_channel_prep_sg_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc xdma_channel_capacity_desc = {
	0, { &xdma_channel_capacity_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc xdma_channel_submit_sg_desc = {
	0, { &xdma_channel_submit_sg_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc xdma_ofw_md_data_desc = {
	0, { &xdma_ofw_md_data_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc xdma_channel_alloc_desc = {
	0, { &xdma_channel_alloc_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc xdma_channel_free_desc = {
	0, { &xdma_channel_free_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc xdma_channel_control_desc = {
	0, { &xdma_channel_control_desc, (kobjop_t)kobj_error_method }
};
