/*
 * This file is @generated automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   freebsd-org/sys/dev/pci/pcib_if.m
 * with
 *   makeobjops.awk
 *
 * See the source file for legal information
 */


#ifndef _pcib_if_h_
#define _pcib_if_h_


#include "pci_if.h"

/** @brief Unique descriptor for the PCIB_MAXSLOTS() method */
extern struct kobjop_desc pcib_maxslots_desc;
/** @brief A function implementing the PCIB_MAXSLOTS() method */
typedef int pcib_maxslots_t(device_t dev);

static __inline int PCIB_MAXSLOTS(device_t dev)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,pcib_maxslots);
	rc = ((pcib_maxslots_t *) _m)(dev);
	return (rc);
}

/** @brief Unique descriptor for the PCIB_MAXFUNCS() method */
extern struct kobjop_desc pcib_maxfuncs_desc;
/** @brief A function implementing the PCIB_MAXFUNCS() method */
typedef int pcib_maxfuncs_t(device_t dev);

static __inline int PCIB_MAXFUNCS(device_t dev)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,pcib_maxfuncs);
	rc = ((pcib_maxfuncs_t *) _m)(dev);
	return (rc);
}

/** @brief Unique descriptor for the PCIB_READ_CONFIG() method */
extern struct kobjop_desc pcib_read_config_desc;
/** @brief A function implementing the PCIB_READ_CONFIG() method */
typedef u_int32_t pcib_read_config_t(device_t dev, u_int bus, u_int slot,
                                     u_int func, u_int reg, int width);

static __inline u_int32_t PCIB_READ_CONFIG(device_t dev, u_int bus, u_int slot,
                                           u_int func, u_int reg, int width)
{
	kobjop_t _m;
	u_int32_t rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,pcib_read_config);
	rc = ((pcib_read_config_t *) _m)(dev, bus, slot, func, reg, width);
	return (rc);
}

/** @brief Unique descriptor for the PCIB_WRITE_CONFIG() method */
extern struct kobjop_desc pcib_write_config_desc;
/** @brief A function implementing the PCIB_WRITE_CONFIG() method */
typedef void pcib_write_config_t(device_t dev, u_int bus, u_int slot,
                                 u_int func, u_int reg, u_int32_t value,
                                 int width);

static __inline void PCIB_WRITE_CONFIG(device_t dev, u_int bus, u_int slot,
                                       u_int func, u_int reg, u_int32_t value,
                                       int width)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,pcib_write_config);
	((pcib_write_config_t *) _m)(dev, bus, slot, func, reg, value, width);
}

/** @brief Unique descriptor for the PCIB_ROUTE_INTERRUPT() method */
extern struct kobjop_desc pcib_route_interrupt_desc;
/** @brief A function implementing the PCIB_ROUTE_INTERRUPT() method */
typedef int pcib_route_interrupt_t(device_t pcib, device_t dev, int pin);

static __inline int PCIB_ROUTE_INTERRUPT(device_t pcib, device_t dev, int pin)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)pcib)->ops,pcib_route_interrupt);
	rc = ((pcib_route_interrupt_t *) _m)(pcib, dev, pin);
	return (rc);
}

/** @brief Unique descriptor for the PCIB_ALLOC_MSI() method */
extern struct kobjop_desc pcib_alloc_msi_desc;
/** @brief A function implementing the PCIB_ALLOC_MSI() method */
typedef int pcib_alloc_msi_t(device_t pcib, device_t dev, int count,
                             int maxcount, int *irqs);

static __inline int PCIB_ALLOC_MSI(device_t pcib, device_t dev, int count,
                                   int maxcount, int *irqs)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)pcib)->ops,pcib_alloc_msi);
	rc = ((pcib_alloc_msi_t *) _m)(pcib, dev, count, maxcount, irqs);
	return (rc);
}

/** @brief Unique descriptor for the PCIB_RELEASE_MSI() method */
extern struct kobjop_desc pcib_release_msi_desc;
/** @brief A function implementing the PCIB_RELEASE_MSI() method */
typedef int pcib_release_msi_t(device_t pcib, device_t dev, int count,
                               int *irqs);

static __inline int PCIB_RELEASE_MSI(device_t pcib, device_t dev, int count,
                                     int *irqs)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)pcib)->ops,pcib_release_msi);
	rc = ((pcib_release_msi_t *) _m)(pcib, dev, count, irqs);
	return (rc);
}

/** @brief Unique descriptor for the PCIB_ALLOC_MSIX() method */
extern struct kobjop_desc pcib_alloc_msix_desc;
/** @brief A function implementing the PCIB_ALLOC_MSIX() method */
typedef int pcib_alloc_msix_t(device_t pcib, device_t dev, int *irq);

static __inline int PCIB_ALLOC_MSIX(device_t pcib, device_t dev, int *irq)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)pcib)->ops,pcib_alloc_msix);
	rc = ((pcib_alloc_msix_t *) _m)(pcib, dev, irq);
	return (rc);
}

/** @brief Unique descriptor for the PCIB_RELEASE_MSIX() method */
extern struct kobjop_desc pcib_release_msix_desc;
/** @brief A function implementing the PCIB_RELEASE_MSIX() method */
typedef int pcib_release_msix_t(device_t pcib, device_t dev, int irq);

static __inline int PCIB_RELEASE_MSIX(device_t pcib, device_t dev, int irq)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)pcib)->ops,pcib_release_msix);
	rc = ((pcib_release_msix_t *) _m)(pcib, dev, irq);
	return (rc);
}

/** @brief Unique descriptor for the PCIB_MAP_MSI() method */
extern struct kobjop_desc pcib_map_msi_desc;
/** @brief A function implementing the PCIB_MAP_MSI() method */
typedef int pcib_map_msi_t(device_t pcib, device_t dev, int irq, uint64_t *addr,
                           uint32_t *data);

static __inline int PCIB_MAP_MSI(device_t pcib, device_t dev, int irq,
                                 uint64_t *addr, uint32_t *data)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)pcib)->ops,pcib_map_msi);
	rc = ((pcib_map_msi_t *) _m)(pcib, dev, irq, addr, data);
	return (rc);
}

/** @brief Unique descriptor for the PCIB_POWER_FOR_SLEEP() method */
extern struct kobjop_desc pcib_power_for_sleep_desc;
/** @brief A function implementing the PCIB_POWER_FOR_SLEEP() method */
typedef int pcib_power_for_sleep_t(device_t pcib, device_t dev, int *pstate);

static __inline int PCIB_POWER_FOR_SLEEP(device_t pcib, device_t dev,
                                         int *pstate)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)pcib)->ops,pcib_power_for_sleep);
	rc = ((pcib_power_for_sleep_t *) _m)(pcib, dev, pstate);
	return (rc);
}

/** @brief Unique descriptor for the PCIB_GET_ID() method */
extern struct kobjop_desc pcib_get_id_desc;
/** @brief A function implementing the PCIB_GET_ID() method */
typedef int pcib_get_id_t(device_t pcib, device_t dev, enum pci_id_type type,
                          uintptr_t *id);

static __inline int PCIB_GET_ID(device_t pcib, device_t dev,
                                enum pci_id_type type, uintptr_t *id)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)pcib)->ops,pcib_get_id);
	rc = ((pcib_get_id_t *) _m)(pcib, dev, type, id);
	return (rc);
}

/** @brief Unique descriptor for the PCIB_TRY_ENABLE_ARI() method */
extern struct kobjop_desc pcib_try_enable_ari_desc;
/** @brief A function implementing the PCIB_TRY_ENABLE_ARI() method */
typedef int pcib_try_enable_ari_t(device_t pcib, device_t dev);

static __inline int PCIB_TRY_ENABLE_ARI(device_t pcib, device_t dev)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)pcib)->ops,pcib_try_enable_ari);
	rc = ((pcib_try_enable_ari_t *) _m)(pcib, dev);
	return (rc);
}

/** @brief Unique descriptor for the PCIB_ARI_ENABLED() method */
extern struct kobjop_desc pcib_ari_enabled_desc;
/** @brief A function implementing the PCIB_ARI_ENABLED() method */
typedef int pcib_ari_enabled_t(device_t pcib);

static __inline int PCIB_ARI_ENABLED(device_t pcib)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)pcib)->ops,pcib_ari_enabled);
	rc = ((pcib_ari_enabled_t *) _m)(pcib);
	return (rc);
}

/** @brief Unique descriptor for the PCIB_DECODE_RID() method */
extern struct kobjop_desc pcib_decode_rid_desc;
/** @brief A function implementing the PCIB_DECODE_RID() method */
typedef void pcib_decode_rid_t(device_t pcib, uint16_t rid, int *bus, int *slot,
                               int *func);

static __inline void PCIB_DECODE_RID(device_t pcib, uint16_t rid, int *bus,
                                     int *slot, int *func)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)pcib)->ops,pcib_decode_rid);
	((pcib_decode_rid_t *) _m)(pcib, rid, bus, slot, func);
}

/** @brief Unique descriptor for the PCIB_REQUEST_FEATURE() method */
extern struct kobjop_desc pcib_request_feature_desc;
/** @brief A function implementing the PCIB_REQUEST_FEATURE() method */
typedef int pcib_request_feature_t(device_t pcib, device_t dev,
                                   enum pci_feature feature);

static __inline int PCIB_REQUEST_FEATURE(device_t pcib, device_t dev,
                                         enum pci_feature feature)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)pcib)->ops,pcib_request_feature);
	rc = ((pcib_request_feature_t *) _m)(pcib, dev, feature);
	return (rc);
}

void pmap_unmapdev(void *, vm_size_t);
#endif /* _pcib_if_h_ */
