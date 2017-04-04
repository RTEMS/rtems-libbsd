/*
 * This file is produced automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   freebsd-org/sys/dev/mmc/mmcbr_if.m
 * with
 *   makeobjops.awk
 *
 * See the source file for legal information
 */


#ifndef _mmcbr_if_h_
#define _mmcbr_if_h_

/** @brief Unique descriptor for the MMCBR_UPDATE_IOS() method */
extern struct kobjop_desc mmcbr_update_ios_desc;
/** @brief A function implementing the MMCBR_UPDATE_IOS() method */
typedef int mmcbr_update_ios_t(device_t brdev, device_t reqdev);

static __inline int MMCBR_UPDATE_IOS(device_t brdev, device_t reqdev)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)brdev)->ops,mmcbr_update_ios);
	return ((mmcbr_update_ios_t *) _m)(brdev, reqdev);
}

/** @brief Unique descriptor for the MMCBR_SWITCH_VCCQ() method */
extern struct kobjop_desc mmcbr_switch_vccq_desc;
/** @brief A function implementing the MMCBR_SWITCH_VCCQ() method */
typedef int mmcbr_switch_vccq_t(device_t brdev, device_t reqdev);

static __inline int MMCBR_SWITCH_VCCQ(device_t brdev, device_t reqdev)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)brdev)->ops,mmcbr_switch_vccq);
	return ((mmcbr_switch_vccq_t *) _m)(brdev, reqdev);
}

/** @brief Unique descriptor for the MMCBR_REQUEST() method */
extern struct kobjop_desc mmcbr_request_desc;
/** @brief A function implementing the MMCBR_REQUEST() method */
typedef int mmcbr_request_t(device_t brdev, device_t reqdev,
                            struct mmc_request *req);

static __inline int MMCBR_REQUEST(device_t brdev, device_t reqdev,
                                  struct mmc_request *req)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)brdev)->ops,mmcbr_request);
	return ((mmcbr_request_t *) _m)(brdev, reqdev, req);
}

/** @brief Unique descriptor for the MMCBR_GET_RO() method */
extern struct kobjop_desc mmcbr_get_ro_desc;
/** @brief A function implementing the MMCBR_GET_RO() method */
typedef int mmcbr_get_ro_t(device_t brdev, device_t reqdev);

static __inline int MMCBR_GET_RO(device_t brdev, device_t reqdev)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)brdev)->ops,mmcbr_get_ro);
	return ((mmcbr_get_ro_t *) _m)(brdev, reqdev);
}

/** @brief Unique descriptor for the MMCBR_ACQUIRE_HOST() method */
extern struct kobjop_desc mmcbr_acquire_host_desc;
/** @brief A function implementing the MMCBR_ACQUIRE_HOST() method */
typedef int mmcbr_acquire_host_t(device_t brdev, device_t reqdev);

static __inline int MMCBR_ACQUIRE_HOST(device_t brdev, device_t reqdev)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)brdev)->ops,mmcbr_acquire_host);
	return ((mmcbr_acquire_host_t *) _m)(brdev, reqdev);
}

/** @brief Unique descriptor for the MMCBR_RELEASE_HOST() method */
extern struct kobjop_desc mmcbr_release_host_desc;
/** @brief A function implementing the MMCBR_RELEASE_HOST() method */
typedef int mmcbr_release_host_t(device_t brdev, device_t reqdev);

static __inline int MMCBR_RELEASE_HOST(device_t brdev, device_t reqdev)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)brdev)->ops,mmcbr_release_host);
	return ((mmcbr_release_host_t *) _m)(brdev, reqdev);
}

#endif /* _mmcbr_if_h_ */
