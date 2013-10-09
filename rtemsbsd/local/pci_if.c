#include <machine/rtems-bsd-config.h>

/*
 * This file is produced automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   dev/pci/pci_if.m
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
#include <rtems/bsd/local/pci_if.h>


static int
null_msi_count(device_t dev, device_t child)
{
	return (0);
}

struct kobj_method pci_read_config_method_default = {
	&pci_read_config_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc pci_read_config_desc = {
	0, &pci_read_config_method_default
};

struct kobj_method pci_write_config_method_default = {
	&pci_write_config_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc pci_write_config_desc = {
	0, &pci_write_config_method_default
};

struct kobj_method pci_get_powerstate_method_default = {
	&pci_get_powerstate_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc pci_get_powerstate_desc = {
	0, &pci_get_powerstate_method_default
};

struct kobj_method pci_set_powerstate_method_default = {
	&pci_set_powerstate_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc pci_set_powerstate_desc = {
	0, &pci_set_powerstate_method_default
};

struct kobj_method pci_get_vpd_ident_method_default = {
	&pci_get_vpd_ident_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc pci_get_vpd_ident_desc = {
	0, &pci_get_vpd_ident_method_default
};

struct kobj_method pci_get_vpd_readonly_method_default = {
	&pci_get_vpd_readonly_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc pci_get_vpd_readonly_desc = {
	0, &pci_get_vpd_readonly_method_default
};

struct kobj_method pci_enable_busmaster_method_default = {
	&pci_enable_busmaster_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc pci_enable_busmaster_desc = {
	0, &pci_enable_busmaster_method_default
};

struct kobj_method pci_disable_busmaster_method_default = {
	&pci_disable_busmaster_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc pci_disable_busmaster_desc = {
	0, &pci_disable_busmaster_method_default
};

struct kobj_method pci_enable_io_method_default = {
	&pci_enable_io_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc pci_enable_io_desc = {
	0, &pci_enable_io_method_default
};

struct kobj_method pci_disable_io_method_default = {
	&pci_disable_io_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc pci_disable_io_desc = {
	0, &pci_disable_io_method_default
};

struct kobj_method pci_assign_interrupt_method_default = {
	&pci_assign_interrupt_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc pci_assign_interrupt_desc = {
	0, &pci_assign_interrupt_method_default
};

struct kobj_method pci_find_extcap_method_default = {
	&pci_find_extcap_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc pci_find_extcap_desc = {
	0, &pci_find_extcap_method_default
};

struct kobj_method pci_alloc_msi_method_default = {
	&pci_alloc_msi_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc pci_alloc_msi_desc = {
	0, &pci_alloc_msi_method_default
};

struct kobj_method pci_alloc_msix_method_default = {
	&pci_alloc_msix_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc pci_alloc_msix_desc = {
	0, &pci_alloc_msix_method_default
};

struct kobj_method pci_remap_msix_method_default = {
	&pci_remap_msix_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc pci_remap_msix_desc = {
	0, &pci_remap_msix_method_default
};

struct kobj_method pci_release_msi_method_default = {
	&pci_release_msi_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc pci_release_msi_desc = {
	0, &pci_release_msi_method_default
};

struct kobj_method pci_msi_count_method_default = {
	&pci_msi_count_desc, (kobjop_t) null_msi_count
};

struct kobjop_desc pci_msi_count_desc = {
	0, &pci_msi_count_method_default
};

struct kobj_method pci_msix_count_method_default = {
	&pci_msix_count_desc, (kobjop_t) null_msi_count
};

struct kobjop_desc pci_msix_count_desc = {
	0, &pci_msix_count_method_default
};

