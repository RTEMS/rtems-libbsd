#include <machine/rtems-bsd-kernel-space.h>
/*
 * This file is @generated automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   freebsd-org/sys/kern/msi_if.m
 * with
 *   makeobjops.awk
 *
 * See the source file for legal information
 */

#include <sys/param.h>
#include <sys/queue.h>
#include <sys/kernel.h>
#include <sys/kobj.h>
#include <rtems/bsd/local/msi_if.h>


static int
iommu_init(device_t dev, device_t child, struct iommu_domain **domain)
{
	*domain = NULL;
	return (0);
}
static void
iommu_deinit(device_t dev, device_t child)
{
}

struct kobjop_desc msi_alloc_msi_desc = {
	0, { &msi_alloc_msi_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc msi_release_msi_desc = {
	0, { &msi_release_msi_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc msi_alloc_msix_desc = {
	0, { &msi_alloc_msix_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc msi_release_msix_desc = {
	0, { &msi_release_msix_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc msi_map_msi_desc = {
	0, { &msi_map_msi_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc msi_iommu_init_desc = {
	0, { &msi_iommu_init_desc, (kobjop_t)iommu_init }
};

struct kobjop_desc msi_iommu_deinit_desc = {
	0, { &msi_iommu_deinit_desc, (kobjop_t)iommu_deinit }
};

