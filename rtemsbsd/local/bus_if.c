#include <machine/rtems-bsd-kernel-space.h>

/*
 * This file is produced automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   freebsd-org/sys/kern/bus_if.m
 * with
 *   makeobjops.awk
 *
 * See the source file for legal information
 */

#include <sys/param.h>
#include <sys/queue.h>
#include <sys/kernel.h>
#include <sys/kobj.h>
#include <sys/types.h>
#include <sys/systm.h>
#include <sys/bus.h>
#include <rtems/bsd/local/bus_if.h>


static struct resource *
null_alloc_resource(device_t dev, device_t child,
    int type, int *rid, rman_res_t start, rman_res_t end,
    rman_res_t count, u_int flags)
{
    return (0);
}

static int
null_remap_intr(device_t bus, device_t dev, u_int irq)
{

	if (dev != NULL)
		return (BUS_REMAP_INTR(dev, NULL, irq));
	return (ENXIO);
}

static device_t
null_add_child(device_t bus, int order, const char *name,
    int unit)
{

	panic("bus_add_child is not implemented");
}

struct kobjop_desc bus_print_child_desc = {
	0, { &bus_print_child_desc, (kobjop_t)bus_generic_print_child }
};

struct kobjop_desc bus_probe_nomatch_desc = {
	0, { &bus_probe_nomatch_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc bus_read_ivar_desc = {
	0, { &bus_read_ivar_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc bus_write_ivar_desc = {
	0, { &bus_write_ivar_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc bus_child_deleted_desc = {
	0, { &bus_child_deleted_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc bus_child_detached_desc = {
	0, { &bus_child_detached_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc bus_driver_added_desc = {
	0, { &bus_driver_added_desc, (kobjop_t)bus_generic_driver_added }
};

struct kobjop_desc bus_add_child_desc = {
	0, { &bus_add_child_desc, (kobjop_t)null_add_child }
};

struct kobjop_desc bus_rescan_desc = {
	0, { &bus_rescan_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc bus_alloc_resource_desc = {
	0, { &bus_alloc_resource_desc, (kobjop_t)null_alloc_resource }
};

struct kobjop_desc bus_activate_resource_desc = {
	0, { &bus_activate_resource_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc bus_map_resource_desc = {
	0, { &bus_map_resource_desc, (kobjop_t)bus_generic_map_resource }
};

struct kobjop_desc bus_unmap_resource_desc = {
	0, { &bus_unmap_resource_desc, (kobjop_t)bus_generic_unmap_resource }
};

struct kobjop_desc bus_deactivate_resource_desc = {
	0, { &bus_deactivate_resource_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc bus_adjust_resource_desc = {
	0, { &bus_adjust_resource_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc bus_release_resource_desc = {
	0, { &bus_release_resource_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc bus_setup_intr_desc = {
	0, { &bus_setup_intr_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc bus_teardown_intr_desc = {
	0, { &bus_teardown_intr_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc bus_suspend_intr_desc = {
	0, { &bus_suspend_intr_desc, (kobjop_t)bus_generic_suspend_intr }
};

struct kobjop_desc bus_resume_intr_desc = {
	0, { &bus_resume_intr_desc, (kobjop_t)bus_generic_resume_intr }
};

struct kobjop_desc bus_set_resource_desc = {
	0, { &bus_set_resource_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc bus_get_resource_desc = {
	0, { &bus_get_resource_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc bus_delete_resource_desc = {
	0, { &bus_delete_resource_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc bus_get_resource_list_desc = {
	0, { &bus_get_resource_list_desc, (kobjop_t)bus_generic_get_resource_list }
};

struct kobjop_desc bus_child_present_desc = {
	0, { &bus_child_present_desc, (kobjop_t)bus_generic_child_present }
};

struct kobjop_desc bus_child_pnpinfo_str_desc = {
	0, { &bus_child_pnpinfo_str_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc bus_child_location_str_desc = {
	0, { &bus_child_location_str_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc bus_bind_intr_desc = {
	0, { &bus_bind_intr_desc, (kobjop_t)bus_generic_bind_intr }
};

struct kobjop_desc bus_config_intr_desc = {
	0, { &bus_config_intr_desc, (kobjop_t)bus_generic_config_intr }
};

struct kobjop_desc bus_describe_intr_desc = {
	0, { &bus_describe_intr_desc, (kobjop_t)bus_generic_describe_intr }
};

struct kobjop_desc bus_hinted_child_desc = {
	0, { &bus_hinted_child_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc bus_get_dma_tag_desc = {
	0, { &bus_get_dma_tag_desc, (kobjop_t)bus_generic_get_dma_tag }
};

struct kobjop_desc bus_get_bus_tag_desc = {
	0, { &bus_get_bus_tag_desc, (kobjop_t)bus_generic_get_bus_tag }
};

struct kobjop_desc bus_hint_device_unit_desc = {
	0, { &bus_hint_device_unit_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc bus_new_pass_desc = {
	0, { &bus_new_pass_desc, (kobjop_t)bus_generic_new_pass }
};

struct kobjop_desc bus_remap_intr_desc = {
	0, { &bus_remap_intr_desc, (kobjop_t)null_remap_intr }
};

struct kobjop_desc bus_suspend_child_desc = {
	0, { &bus_suspend_child_desc, (kobjop_t)bus_generic_suspend_child }
};

struct kobjop_desc bus_resume_child_desc = {
	0, { &bus_resume_child_desc, (kobjop_t)bus_generic_resume_child }
};

struct kobjop_desc bus_get_domain_desc = {
	0, { &bus_get_domain_desc, (kobjop_t)bus_generic_get_domain }
};

struct kobjop_desc bus_get_cpus_desc = {
	0, { &bus_get_cpus_desc, (kobjop_t)bus_generic_get_cpus }
};

