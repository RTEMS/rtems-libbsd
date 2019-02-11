/*
 * This file is produced automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   freebsd-org/sys/dev/mmc/mmcbus_if.m
 * with
 *   makeobjops.awk
 *
 * See the source file for legal information
 */


#ifndef _mmcbus_if_h_
#define _mmcbus_if_h_

/** @brief Unique descriptor for the MMCBUS_RETUNE_PAUSE() method */
extern struct kobjop_desc mmcbus_retune_pause_desc;
/** @brief A function implementing the MMCBUS_RETUNE_PAUSE() method */
typedef void mmcbus_retune_pause_t(device_t busdev, device_t reqdev,
                                   bool retune);

static __inline void MMCBUS_RETUNE_PAUSE(device_t busdev, device_t reqdev,
                                         bool retune)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)busdev)->ops,mmcbus_retune_pause);
	((mmcbus_retune_pause_t *) _m)(busdev, reqdev, retune);
}

/** @brief Unique descriptor for the MMCBUS_RETUNE_UNPAUSE() method */
extern struct kobjop_desc mmcbus_retune_unpause_desc;
/** @brief A function implementing the MMCBUS_RETUNE_UNPAUSE() method */
typedef void mmcbus_retune_unpause_t(device_t busdev, device_t reqdev);

static __inline void MMCBUS_RETUNE_UNPAUSE(device_t busdev, device_t reqdev)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)busdev)->ops,mmcbus_retune_unpause);
	((mmcbus_retune_unpause_t *) _m)(busdev, reqdev);
}

/** @brief Unique descriptor for the MMCBUS_WAIT_FOR_REQUEST() method */
extern struct kobjop_desc mmcbus_wait_for_request_desc;
/** @brief A function implementing the MMCBUS_WAIT_FOR_REQUEST() method */
typedef int mmcbus_wait_for_request_t(device_t busdev, device_t reqdev,
                                      struct mmc_request *req);

static __inline int MMCBUS_WAIT_FOR_REQUEST(device_t busdev, device_t reqdev,
                                            struct mmc_request *req)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)busdev)->ops,mmcbus_wait_for_request);
	rc = ((mmcbus_wait_for_request_t *) _m)(busdev, reqdev, req);
	return (rc);
}

/** @brief Unique descriptor for the MMCBUS_ACQUIRE_BUS() method */
extern struct kobjop_desc mmcbus_acquire_bus_desc;
/** @brief A function implementing the MMCBUS_ACQUIRE_BUS() method */
typedef int mmcbus_acquire_bus_t(device_t busdev, device_t reqdev);

static __inline int MMCBUS_ACQUIRE_BUS(device_t busdev, device_t reqdev)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)busdev)->ops,mmcbus_acquire_bus);
	rc = ((mmcbus_acquire_bus_t *) _m)(busdev, reqdev);
	return (rc);
}

/** @brief Unique descriptor for the MMCBUS_RELEASE_BUS() method */
extern struct kobjop_desc mmcbus_release_bus_desc;
/** @brief A function implementing the MMCBUS_RELEASE_BUS() method */
typedef int mmcbus_release_bus_t(device_t busdev, device_t reqdev);

static __inline int MMCBUS_RELEASE_BUS(device_t busdev, device_t reqdev)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)busdev)->ops,mmcbus_release_bus);
	rc = ((mmcbus_release_bus_t *) _m)(busdev, reqdev);
	return (rc);
}

#endif /* _mmcbus_if_h_ */
