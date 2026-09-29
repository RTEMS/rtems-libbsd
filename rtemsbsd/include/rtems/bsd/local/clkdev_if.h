/*
 * This file is @generated automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   freebsd-org/sys/dev/extres/clk/clkdev_if.m
 * with
 *   makeobjops.awk
 *
 * See the source file for legal information
 */


#ifndef _clkdev_if_h_
#define _clkdev_if_h_

/** @brief Unique descriptor for the CLKDEV_WRITE_4() method */
extern struct kobjop_desc clkdev_write_4_desc;
/** @brief A function implementing the CLKDEV_WRITE_4() method */
typedef int clkdev_write_4_t(device_t dev, bus_addr_t addr, uint32_t val);

static __inline int CLKDEV_WRITE_4(device_t dev, bus_addr_t addr, uint32_t val)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,clkdev_write_4);
	rc = ((clkdev_write_4_t *) _m)(dev, addr, val);
	return (rc);
}

/** @brief Unique descriptor for the CLKDEV_READ_4() method */
extern struct kobjop_desc clkdev_read_4_desc;
/** @brief A function implementing the CLKDEV_READ_4() method */
typedef int clkdev_read_4_t(device_t dev, bus_addr_t addr, uint32_t *val);

static __inline int CLKDEV_READ_4(device_t dev, bus_addr_t addr, uint32_t *val)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,clkdev_read_4);
	rc = ((clkdev_read_4_t *) _m)(dev, addr, val);
	return (rc);
}

/** @brief Unique descriptor for the CLKDEV_MODIFY_4() method */
extern struct kobjop_desc clkdev_modify_4_desc;
/** @brief A function implementing the CLKDEV_MODIFY_4() method */
typedef int clkdev_modify_4_t(device_t dev, bus_addr_t addr,
                              uint32_t clear_mask, uint32_t set_mask);

static __inline int CLKDEV_MODIFY_4(device_t dev, bus_addr_t addr,
                                    uint32_t clear_mask, uint32_t set_mask)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,clkdev_modify_4);
	rc = ((clkdev_modify_4_t *) _m)(dev, addr, clear_mask, set_mask);
	return (rc);
}

/** @brief Unique descriptor for the CLKDEV_DEVICE_LOCK() method */
extern struct kobjop_desc clkdev_device_lock_desc;
/** @brief A function implementing the CLKDEV_DEVICE_LOCK() method */
typedef void clkdev_device_lock_t(device_t dev);

static __inline void CLKDEV_DEVICE_LOCK(device_t dev)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,clkdev_device_lock);
	((clkdev_device_lock_t *) _m)(dev);
}

/** @brief Unique descriptor for the CLKDEV_DEVICE_UNLOCK() method */
extern struct kobjop_desc clkdev_device_unlock_desc;
/** @brief A function implementing the CLKDEV_DEVICE_UNLOCK() method */
typedef void clkdev_device_unlock_t(device_t dev);

static __inline void CLKDEV_DEVICE_UNLOCK(device_t dev)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,clkdev_device_unlock);
	((clkdev_device_unlock_t *) _m)(dev);
}

#endif /* _clkdev_if_h_ */
