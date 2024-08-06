/*
 * This file is @generated automatically.
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
	int rc;
	KOBJOPLOOKUP(((kobj_t)brdev)->ops,mmcbr_update_ios);
	rc = ((mmcbr_update_ios_t *) _m)(brdev, reqdev);
	return (rc);
}

/** @brief Unique descriptor for the MMCBR_SWITCH_VCCQ() method */
extern struct kobjop_desc mmcbr_switch_vccq_desc;
/** @brief A function implementing the MMCBR_SWITCH_VCCQ() method */
typedef int mmcbr_switch_vccq_t(device_t brdev, device_t reqdev);

static __inline int MMCBR_SWITCH_VCCQ(device_t brdev, device_t reqdev)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)brdev)->ops,mmcbr_switch_vccq);
	rc = ((mmcbr_switch_vccq_t *) _m)(brdev, reqdev);
	return (rc);
}

/** @brief Unique descriptor for the MMCBR_TUNE() method */
extern struct kobjop_desc mmcbr_tune_desc;
/** @brief A function implementing the MMCBR_TUNE() method */
typedef int mmcbr_tune_t(device_t brdev, device_t reqdev, bool hs400);

static __inline int MMCBR_TUNE(device_t brdev, device_t reqdev, bool hs400)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)brdev)->ops,mmcbr_tune);
	rc = ((mmcbr_tune_t *) _m)(brdev, reqdev, hs400);
	return (rc);
}

/** @brief Unique descriptor for the MMCBR_RETUNE() method */
extern struct kobjop_desc mmcbr_retune_desc;
/** @brief A function implementing the MMCBR_RETUNE() method */
typedef int mmcbr_retune_t(device_t brdev, device_t reqdev, bool reset);

static __inline int MMCBR_RETUNE(device_t brdev, device_t reqdev, bool reset)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)brdev)->ops,mmcbr_retune);
	rc = ((mmcbr_retune_t *) _m)(brdev, reqdev, reset);
	return (rc);
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
	int rc;
	KOBJOPLOOKUP(((kobj_t)brdev)->ops,mmcbr_request);
	rc = ((mmcbr_request_t *) _m)(brdev, reqdev, req);
	return (rc);
}

/** @brief Unique descriptor for the MMCBR_GET_RO() method */
extern struct kobjop_desc mmcbr_get_ro_desc;
/** @brief A function implementing the MMCBR_GET_RO() method */
typedef int mmcbr_get_ro_t(device_t brdev, device_t reqdev);

static __inline int MMCBR_GET_RO(device_t brdev, device_t reqdev)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)brdev)->ops,mmcbr_get_ro);
	rc = ((mmcbr_get_ro_t *) _m)(brdev, reqdev);
	return (rc);
}

/** @brief Unique descriptor for the MMCBR_ACQUIRE_HOST() method */
extern struct kobjop_desc mmcbr_acquire_host_desc;
/** @brief A function implementing the MMCBR_ACQUIRE_HOST() method */
typedef int mmcbr_acquire_host_t(device_t brdev, device_t reqdev);

static __inline int MMCBR_ACQUIRE_HOST(device_t brdev, device_t reqdev)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)brdev)->ops,mmcbr_acquire_host);
	rc = ((mmcbr_acquire_host_t *) _m)(brdev, reqdev);
	return (rc);
}

/** @brief Unique descriptor for the MMCBR_RELEASE_HOST() method */
extern struct kobjop_desc mmcbr_release_host_desc;
/** @brief A function implementing the MMCBR_RELEASE_HOST() method */
typedef int mmcbr_release_host_t(device_t brdev, device_t reqdev);

static __inline int MMCBR_RELEASE_HOST(device_t brdev, device_t reqdev)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)brdev)->ops,mmcbr_release_host);
	rc = ((mmcbr_release_host_t *) _m)(brdev, reqdev);
	return (rc);
}

#endif /* _mmcbr_if_h_ */
