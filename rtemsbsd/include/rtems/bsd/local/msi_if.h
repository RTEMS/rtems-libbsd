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


#ifndef _msi_if_h_
#define _msi_if_h_


#include <machine/bus.h>
#include <dev/iommu/iommu_msi.h>

struct intr_irqsrc;

/** @brief Unique descriptor for the MSI_ALLOC_MSI() method */
extern struct kobjop_desc msi_alloc_msi_desc;
/** @brief A function implementing the MSI_ALLOC_MSI() method */
typedef int msi_alloc_msi_t(device_t dev, device_t child, int count,
                            int maxcount, device_t *pic,
                            struct intr_irqsrc **srcs);

static __inline int MSI_ALLOC_MSI(device_t dev, device_t child, int count,
                                  int maxcount, device_t *pic,
                                  struct intr_irqsrc **srcs)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,msi_alloc_msi);
	rc = ((msi_alloc_msi_t *) _m)(dev, child, count, maxcount, pic, srcs);
	return (rc);
}

/** @brief Unique descriptor for the MSI_RELEASE_MSI() method */
extern struct kobjop_desc msi_release_msi_desc;
/** @brief A function implementing the MSI_RELEASE_MSI() method */
typedef int msi_release_msi_t(device_t dev, device_t child, int count,
                              struct intr_irqsrc **srcs);

static __inline int MSI_RELEASE_MSI(device_t dev, device_t child, int count,
                                    struct intr_irqsrc **srcs)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,msi_release_msi);
	rc = ((msi_release_msi_t *) _m)(dev, child, count, srcs);
	return (rc);
}

/** @brief Unique descriptor for the MSI_ALLOC_MSIX() method */
extern struct kobjop_desc msi_alloc_msix_desc;
/** @brief A function implementing the MSI_ALLOC_MSIX() method */
typedef int msi_alloc_msix_t(device_t dev, device_t child, device_t *pic,
                             struct intr_irqsrc **src);

static __inline int MSI_ALLOC_MSIX(device_t dev, device_t child, device_t *pic,
                                   struct intr_irqsrc **src)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,msi_alloc_msix);
	rc = ((msi_alloc_msix_t *) _m)(dev, child, pic, src);
	return (rc);
}

/** @brief Unique descriptor for the MSI_RELEASE_MSIX() method */
extern struct kobjop_desc msi_release_msix_desc;
/** @brief A function implementing the MSI_RELEASE_MSIX() method */
typedef int msi_release_msix_t(device_t dev, device_t child,
                               struct intr_irqsrc *src);

static __inline int MSI_RELEASE_MSIX(device_t dev, device_t child,
                                     struct intr_irqsrc *src)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,msi_release_msix);
	rc = ((msi_release_msix_t *) _m)(dev, child, src);
	return (rc);
}

/** @brief Unique descriptor for the MSI_MAP_MSI() method */
extern struct kobjop_desc msi_map_msi_desc;
/** @brief A function implementing the MSI_MAP_MSI() method */
typedef int msi_map_msi_t(device_t dev, device_t child, struct intr_irqsrc *src,
                          uint64_t *addr, uint32_t *data);

static __inline int MSI_MAP_MSI(device_t dev, device_t child,
                                struct intr_irqsrc *src, uint64_t *addr,
                                uint32_t *data)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,msi_map_msi);
	rc = ((msi_map_msi_t *) _m)(dev, child, src, addr, data);
	return (rc);
}

/** @brief Unique descriptor for the MSI_IOMMU_INIT() method */
extern struct kobjop_desc msi_iommu_init_desc;
/** @brief A function implementing the MSI_IOMMU_INIT() method */
typedef int msi_iommu_init_t(device_t dev, device_t child,
                             struct iommu_domain **domain);

static __inline int MSI_IOMMU_INIT(device_t dev, device_t child,
                                   struct iommu_domain **domain)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,msi_iommu_init);
	rc = ((msi_iommu_init_t *) _m)(dev, child, domain);
	return (rc);
}

/** @brief Unique descriptor for the MSI_IOMMU_DEINIT() method */
extern struct kobjop_desc msi_iommu_deinit_desc;
/** @brief A function implementing the MSI_IOMMU_DEINIT() method */
typedef void msi_iommu_deinit_t(device_t dev, device_t child);

static __inline void MSI_IOMMU_DEINIT(device_t dev, device_t child)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,msi_iommu_deinit);
	((msi_iommu_deinit_t *) _m)(dev, child);
}

#endif /* _msi_if_h_ */
