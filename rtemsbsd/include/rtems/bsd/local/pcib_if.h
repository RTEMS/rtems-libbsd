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


#ifndef _pcib_if_h_
#define _pcib_if_h_

/** @brief Unique descriptor for the PCIB_MAXSLOTS() method */
extern struct kobjop_desc pcib_maxslots_desc;
/** @brief A function implementing the PCIB_MAXSLOTS() method */
typedef int pcib_maxslots_t(device_t dev);

static __inline int PCIB_MAXSLOTS(device_t dev)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,pcib_maxslots);
	return ((pcib_maxslots_t *) _m)(dev);
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
	KOBJOPLOOKUP(((kobj_t)dev)->ops,pcib_read_config);
	return ((pcib_read_config_t *) _m)(dev, bus, slot, func, reg, width);
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
	KOBJOPLOOKUP(((kobj_t)pcib)->ops,pcib_route_interrupt);
	return ((pcib_route_interrupt_t *) _m)(pcib, dev, pin);
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
	KOBJOPLOOKUP(((kobj_t)pcib)->ops,pcib_alloc_msi);
	return ((pcib_alloc_msi_t *) _m)(pcib, dev, count, maxcount, irqs);
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
	KOBJOPLOOKUP(((kobj_t)pcib)->ops,pcib_release_msi);
	return ((pcib_release_msi_t *) _m)(pcib, dev, count, irqs);
}

/** @brief Unique descriptor for the PCIB_ALLOC_MSIX() method */
extern struct kobjop_desc pcib_alloc_msix_desc;
/** @brief A function implementing the PCIB_ALLOC_MSIX() method */
typedef int pcib_alloc_msix_t(device_t pcib, device_t dev, int *irq);

static __inline int PCIB_ALLOC_MSIX(device_t pcib, device_t dev, int *irq)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)pcib)->ops,pcib_alloc_msix);
	return ((pcib_alloc_msix_t *) _m)(pcib, dev, irq);
}

/** @brief Unique descriptor for the PCIB_RELEASE_MSIX() method */
extern struct kobjop_desc pcib_release_msix_desc;
/** @brief A function implementing the PCIB_RELEASE_MSIX() method */
typedef int pcib_release_msix_t(device_t pcib, device_t dev, int irq);

static __inline int PCIB_RELEASE_MSIX(device_t pcib, device_t dev, int irq)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)pcib)->ops,pcib_release_msix);
	return ((pcib_release_msix_t *) _m)(pcib, dev, irq);
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
	KOBJOPLOOKUP(((kobj_t)pcib)->ops,pcib_map_msi);
	return ((pcib_map_msi_t *) _m)(pcib, dev, irq, addr, data);
}

/** @brief Unique descriptor for the PCIB_POWER_FOR_SLEEP() method */
extern struct kobjop_desc pcib_power_for_sleep_desc;
/** @brief A function implementing the PCIB_POWER_FOR_SLEEP() method */
typedef int pcib_power_for_sleep_t(device_t pcib, device_t dev, int *pstate);

static __inline int PCIB_POWER_FOR_SLEEP(device_t pcib, device_t dev,
                                         int *pstate)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)pcib)->ops,pcib_power_for_sleep);
	return ((pcib_power_for_sleep_t *) _m)(pcib, dev, pstate);
}

#endif /* _pcib_if_h_ */
