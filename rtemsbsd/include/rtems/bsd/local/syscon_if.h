/*
 * This file is @generated automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   freebsd-org/sys/dev/extres/syscon/syscon_if.m
 * with
 *   makeobjops.awk
 *
 * See the source file for legal information
 */


#ifndef _syscon_if_h_
#define _syscon_if_h_


struct syscon;
int syscon_get_handle_default(device_t dev, struct syscon **syscon);

/** @brief Unique descriptor for the SYSCON_INIT() method */
extern struct kobjop_desc syscon_init_desc;
/** @brief A function implementing the SYSCON_INIT() method */
typedef int syscon_init_t(struct syscon *syscon);

static __inline int SYSCON_INIT(struct syscon *syscon)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)syscon)->ops,syscon_init);
	rc = ((syscon_init_t *) _m)(syscon);
	return (rc);
}

/** @brief Unique descriptor for the SYSCON_UNINIT() method */
extern struct kobjop_desc syscon_uninit_desc;
/** @brief A function implementing the SYSCON_UNINIT() method */
typedef int syscon_uninit_t(struct syscon *syscon);

static __inline int SYSCON_UNINIT(struct syscon *syscon)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)syscon)->ops,syscon_uninit);
	rc = ((syscon_uninit_t *) _m)(syscon);
	return (rc);
}

/** @brief Unique descriptor for the SYSCON_READ_4() method */
extern struct kobjop_desc syscon_read_4_desc;
/** @brief A function implementing the SYSCON_READ_4() method */
typedef uint32_t syscon_read_4_t(struct syscon *syscon, bus_size_t offset);
/**
 * Accessor functions for syscon register space
 */

static __inline uint32_t SYSCON_READ_4(struct syscon *syscon, bus_size_t offset)
{
	kobjop_t _m;
	uint32_t rc;
	KOBJOPLOOKUP(((kobj_t)syscon)->ops,syscon_read_4);
	rc = ((syscon_read_4_t *) _m)(syscon, offset);
	return (rc);
}

/** @brief Unique descriptor for the SYSCON_WRITE_4() method */
extern struct kobjop_desc syscon_write_4_desc;
/** @brief A function implementing the SYSCON_WRITE_4() method */
typedef int syscon_write_4_t(struct syscon *syscon, bus_size_t offset,
                             uint32_t val);

static __inline int SYSCON_WRITE_4(struct syscon *syscon, bus_size_t offset,
                                   uint32_t val)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)syscon)->ops,syscon_write_4);
	rc = ((syscon_write_4_t *) _m)(syscon, offset, val);
	return (rc);
}

/** @brief Unique descriptor for the SYSCON_MODIFY_4() method */
extern struct kobjop_desc syscon_modify_4_desc;
/** @brief A function implementing the SYSCON_MODIFY_4() method */
typedef int syscon_modify_4_t(struct syscon *syscon, bus_size_t offset,
                              uint32_t clear_bits, uint32_t set_bits);

static __inline int SYSCON_MODIFY_4(struct syscon *syscon, bus_size_t offset,
                                    uint32_t clear_bits, uint32_t set_bits)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)syscon)->ops,syscon_modify_4);
	rc = ((syscon_modify_4_t *) _m)(syscon, offset, clear_bits, set_bits);
	return (rc);
}

/** @brief Unique descriptor for the SYSCON_UNLOCKED_READ_4() method */
extern struct kobjop_desc syscon_unlocked_read_4_desc;
/** @brief A function implementing the SYSCON_UNLOCKED_READ_4() method */
typedef uint32_t syscon_unlocked_read_4_t(struct syscon *syscon,
                                          bus_size_t offset);
/**
 * Unlocked verion of access function
 */

static __inline uint32_t SYSCON_UNLOCKED_READ_4(struct syscon *syscon,
                                                bus_size_t offset)
{
	kobjop_t _m;
	uint32_t rc;
	KOBJOPLOOKUP(((kobj_t)syscon)->ops,syscon_unlocked_read_4);
	rc = ((syscon_unlocked_read_4_t *) _m)(syscon, offset);
	return (rc);
}

/** @brief Unique descriptor for the SYSCON_UNLOCKED_WRITE_4() method */
extern struct kobjop_desc syscon_unlocked_write_4_desc;
/** @brief A function implementing the SYSCON_UNLOCKED_WRITE_4() method */
typedef int syscon_unlocked_write_4_t(struct syscon *syscon, bus_size_t offset,
                                      uint32_t val);

static __inline int SYSCON_UNLOCKED_WRITE_4(struct syscon *syscon,
                                            bus_size_t offset, uint32_t val)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)syscon)->ops,syscon_unlocked_write_4);
	rc = ((syscon_unlocked_write_4_t *) _m)(syscon, offset, val);
	return (rc);
}

/** @brief Unique descriptor for the SYSCON_UNLOCKED_MODIFY_4() method */
extern struct kobjop_desc syscon_unlocked_modify_4_desc;
/** @brief A function implementing the SYSCON_UNLOCKED_MODIFY_4() method */
typedef int syscon_unlocked_modify_4_t(struct syscon *syscon, bus_size_t offset,
                                       uint32_t clear_bits, uint32_t set_bits);

static __inline int SYSCON_UNLOCKED_MODIFY_4(struct syscon *syscon,
                                             bus_size_t offset,
                                             uint32_t clear_bits,
                                             uint32_t set_bits)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)syscon)->ops,syscon_unlocked_modify_4);
	rc = ((syscon_unlocked_modify_4_t *) _m)(syscon, offset, clear_bits, set_bits);
	return (rc);
}

/** @brief Unique descriptor for the SYSCON_DEVICE_LOCK() method */
extern struct kobjop_desc syscon_device_lock_desc;
/** @brief A function implementing the SYSCON_DEVICE_LOCK() method */
typedef void syscon_device_lock_t(device_t dev);
/**
* Locking for exclusive access to underlying device
*/

static __inline void SYSCON_DEVICE_LOCK(device_t dev)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,syscon_device_lock);
	((syscon_device_lock_t *) _m)(dev);
}

/** @brief Unique descriptor for the SYSCON_DEVICE_UNLOCK() method */
extern struct kobjop_desc syscon_device_unlock_desc;
/** @brief A function implementing the SYSCON_DEVICE_UNLOCK() method */
typedef void syscon_device_unlock_t(device_t dev);

static __inline void SYSCON_DEVICE_UNLOCK(device_t dev)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,syscon_device_unlock);
	((syscon_device_unlock_t *) _m)(dev);
}

/** @brief Unique descriptor for the SYSCON_GET_HANDLE() method */
extern struct kobjop_desc syscon_get_handle_desc;
/** @brief A function implementing the SYSCON_GET_HANDLE() method */
typedef int syscon_get_handle_t(device_t dev, struct syscon **syscon);
/**
 * Get syscon handle from parent driver
 */

static __inline int SYSCON_GET_HANDLE(device_t dev, struct syscon **syscon)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,syscon_get_handle);
	rc = ((syscon_get_handle_t *) _m)(dev, syscon);
	return (rc);
}

#endif /* _syscon_if_h_ */
