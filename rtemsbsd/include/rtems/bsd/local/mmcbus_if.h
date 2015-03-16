/*
 * This file is produced automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   dev/mmc/mmcbus_if.m
 * with
 *   makeobjops.awk
 *
 * See the source file for legal information
 */


#ifndef _mmcbus_if_h_
#define _mmcbus_if_h_

/** @brief Unique descriptor for the MMCBUS_WAIT_FOR_REQUEST() method */
extern struct kobjop_desc mmcbus_wait_for_request_desc;
/** @brief A function implementing the MMCBUS_WAIT_FOR_REQUEST() method */
typedef int mmcbus_wait_for_request_t(device_t brdev, device_t reqdev,
                                      struct mmc_request *req);

static __inline int MMCBUS_WAIT_FOR_REQUEST(device_t brdev, device_t reqdev,
                                            struct mmc_request *req)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)brdev)->ops,mmcbus_wait_for_request);
	return ((mmcbus_wait_for_request_t *) _m)(brdev, reqdev, req);
}

/** @brief Unique descriptor for the MMCBUS_ACQUIRE_BUS() method */
extern struct kobjop_desc mmcbus_acquire_bus_desc;
/** @brief A function implementing the MMCBUS_ACQUIRE_BUS() method */
typedef int mmcbus_acquire_bus_t(device_t brdev, device_t reqdev);

static __inline int MMCBUS_ACQUIRE_BUS(device_t brdev, device_t reqdev)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)brdev)->ops,mmcbus_acquire_bus);
	return ((mmcbus_acquire_bus_t *) _m)(brdev, reqdev);
}

/** @brief Unique descriptor for the MMCBUS_RELEASE_BUS() method */
extern struct kobjop_desc mmcbus_release_bus_desc;
/** @brief A function implementing the MMCBUS_RELEASE_BUS() method */
typedef int mmcbus_release_bus_t(device_t brdev, device_t reqdev);

static __inline int MMCBUS_RELEASE_BUS(device_t brdev, device_t reqdev)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)brdev)->ops,mmcbus_release_bus);
	return ((mmcbus_release_bus_t *) _m)(brdev, reqdev);
}

#endif /* _mmcbus_if_h_ */
