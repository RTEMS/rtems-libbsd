#include <machine/rtems-bsd-kernel-space.h>

/*
 * This file is produced automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   freebsd-org/sys/dev/pci/pci_if.m
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
#include <dev/pci/pcivar.h>
#include <rtems/bsd/local/pci_if.h>


static int
null_msi_count(device_t dev, device_t child)
{
	return (0);
}

static int
null_msix_bar(device_t dev, device_t child)
{
	return (-1);
}

static device_t
null_create_iov_child(device_t bus, device_t pf, uint16_t rid,
    uint16_t vid, uint16_t did)
{
	device_printf(bus, "PCI_IOV not implemented on this bus.\n");
	return (NULL);
}

struct kobjop_desc pci_read_config_desc = {
	0, { &pci_read_config_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc pci_write_config_desc = {
	0, { &pci_write_config_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc pci_get_powerstate_desc = {
	0, { &pci_get_powerstate_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc pci_set_powerstate_desc = {
	0, { &pci_set_powerstate_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc pci_get_vpd_ident_desc = {
	0, { &pci_get_vpd_ident_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc pci_get_vpd_readonly_desc = {
	0, { &pci_get_vpd_readonly_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc pci_enable_busmaster_desc = {
	0, { &pci_enable_busmaster_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc pci_disable_busmaster_desc = {
	0, { &pci_disable_busmaster_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc pci_enable_io_desc = {
	0, { &pci_enable_io_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc pci_disable_io_desc = {
	0, { &pci_disable_io_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc pci_assign_interrupt_desc = {
	0, { &pci_assign_interrupt_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc pci_find_cap_desc = {
	0, { &pci_find_cap_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc pci_find_next_cap_desc = {
	0, { &pci_find_next_cap_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc pci_find_extcap_desc = {
	0, { &pci_find_extcap_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc pci_find_next_extcap_desc = {
	0, { &pci_find_next_extcap_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc pci_find_htcap_desc = {
	0, { &pci_find_htcap_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc pci_find_next_htcap_desc = {
	0, { &pci_find_next_htcap_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc pci_alloc_msi_desc = {
	0, { &pci_alloc_msi_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc pci_alloc_msix_desc = {
	0, { &pci_alloc_msix_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc pci_enable_msi_desc = {
	0, { &pci_enable_msi_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc pci_enable_msix_desc = {
	0, { &pci_enable_msix_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc pci_disable_msi_desc = {
	0, { &pci_disable_msi_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc pci_remap_msix_desc = {
	0, { &pci_remap_msix_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc pci_release_msi_desc = {
	0, { &pci_release_msi_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc pci_msi_count_desc = {
	0, { &pci_msi_count_desc, (kobjop_t)null_msi_count }
};

struct kobjop_desc pci_msix_count_desc = {
	0, { &pci_msix_count_desc, (kobjop_t)null_msi_count }
};

struct kobjop_desc pci_msix_pba_bar_desc = {
	0, { &pci_msix_pba_bar_desc, (kobjop_t)null_msix_bar }
};

struct kobjop_desc pci_msix_table_bar_desc = {
	0, { &pci_msix_table_bar_desc, (kobjop_t)null_msix_bar }
};

struct kobjop_desc pci_get_id_desc = {
	0, { &pci_get_id_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc pci_alloc_devinfo_desc = {
	0, { &pci_alloc_devinfo_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc pci_child_added_desc = {
	0, { &pci_child_added_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc pci_iov_attach_desc = {
	0, { &pci_iov_attach_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc pci_iov_detach_desc = {
	0, { &pci_iov_detach_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc pci_create_iov_child_desc = {
	0, { &pci_create_iov_child_desc, (kobjop_t)null_create_iov_child }
};

