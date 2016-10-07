#include <machine/rtems-bsd-kernel-space.h>

/*
 * This file is produced automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   freebsd-org/sys/dev/pci/pcib_if.m
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
#include <sys/rman.h>
#include <dev/pci/pcivar.h>
#include <dev/pci/pcib_private.h>
#include <rtems/bsd/local/pcib_if.h>


static int
null_route_interrupt(device_t pcib, device_t dev, int pin)
{
	return (PCI_INVALID_IRQ);
}

static int
pcib_null_ari_enabled(device_t pcib)
{

	return (0);
}

struct kobjop_desc pcib_maxslots_desc = {
	0, { &pcib_maxslots_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc pcib_maxfuncs_desc = {
	0, { &pcib_maxfuncs_desc, (kobjop_t)pcib_maxfuncs }
};

struct kobjop_desc pcib_read_config_desc = {
	0, { &pcib_read_config_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc pcib_write_config_desc = {
	0, { &pcib_write_config_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc pcib_route_interrupt_desc = {
	0, { &pcib_route_interrupt_desc, (kobjop_t)null_route_interrupt }
};

struct kobjop_desc pcib_alloc_msi_desc = {
	0, { &pcib_alloc_msi_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc pcib_release_msi_desc = {
	0, { &pcib_release_msi_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc pcib_alloc_msix_desc = {
	0, { &pcib_alloc_msix_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc pcib_release_msix_desc = {
	0, { &pcib_release_msix_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc pcib_map_msi_desc = {
	0, { &pcib_map_msi_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc pcib_power_for_sleep_desc = {
	0, { &pcib_power_for_sleep_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc pcib_get_id_desc = {
	0, { &pcib_get_id_desc, (kobjop_t)pcib_get_id }
};

struct kobjop_desc pcib_try_enable_ari_desc = {
	0, { &pcib_try_enable_ari_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc pcib_ari_enabled_desc = {
	0, { &pcib_ari_enabled_desc, (kobjop_t)pcib_null_ari_enabled }
};

struct kobjop_desc pcib_decode_rid_desc = {
	0, { &pcib_decode_rid_desc, (kobjop_t)pcib_decode_rid }
};

