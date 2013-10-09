#include <machine/rtems-bsd-config.h>

/*
 * This file is produced automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   dev/pci/pcib_if.m
 * with
 *   makeobjops.awk
 *
 * See the source file for legal information
 */

#include <rtems/bsd/sys/param.h>
#include <sys/queue.h>
#include <sys/kernel.h>
#include <sys/kobj.h>
#include <sys/bus.h>
#include <dev/pci/pcivar.h>
#include <rtems/bsd/local/pcib_if.h>


static int
null_route_interrupt(device_t pcib, device_t dev, int pin)
{
	return (PCI_INVALID_IRQ);
}

struct kobj_method pcib_maxslots_method_default = {
	&pcib_maxslots_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc pcib_maxslots_desc = {
	0, &pcib_maxslots_method_default
};

struct kobj_method pcib_read_config_method_default = {
	&pcib_read_config_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc pcib_read_config_desc = {
	0, &pcib_read_config_method_default
};

struct kobj_method pcib_write_config_method_default = {
	&pcib_write_config_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc pcib_write_config_desc = {
	0, &pcib_write_config_method_default
};

struct kobj_method pcib_route_interrupt_method_default = {
	&pcib_route_interrupt_desc, (kobjop_t) null_route_interrupt
};

struct kobjop_desc pcib_route_interrupt_desc = {
	0, &pcib_route_interrupt_method_default
};

struct kobj_method pcib_alloc_msi_method_default = {
	&pcib_alloc_msi_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc pcib_alloc_msi_desc = {
	0, &pcib_alloc_msi_method_default
};

struct kobj_method pcib_release_msi_method_default = {
	&pcib_release_msi_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc pcib_release_msi_desc = {
	0, &pcib_release_msi_method_default
};

struct kobj_method pcib_alloc_msix_method_default = {
	&pcib_alloc_msix_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc pcib_alloc_msix_desc = {
	0, &pcib_alloc_msix_method_default
};

struct kobj_method pcib_release_msix_method_default = {
	&pcib_release_msix_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc pcib_release_msix_desc = {
	0, &pcib_release_msix_method_default
};

struct kobj_method pcib_map_msi_method_default = {
	&pcib_map_msi_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc pcib_map_msi_desc = {
	0, &pcib_map_msi_method_default
};

