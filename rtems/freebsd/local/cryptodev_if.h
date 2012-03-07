/*
 * This file is produced automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   opencrypto/cryptodev_if.m
 * with
 *   makeobjops.awk
 *
 * See the source file for legal information
 */


#ifndef _cryptodev_if_h_
#define _cryptodev_if_h_

/** @brief Unique descriptor for the CRYPTODEV_NEWSESSION() method */
extern struct kobjop_desc cryptodev_newsession_desc;
/** @brief A function implementing the CRYPTODEV_NEWSESSION() method */
typedef int cryptodev_newsession_t(device_t dev, uint32_t *sid,
                                   struct cryptoini *cri);

static __inline int CRYPTODEV_NEWSESSION(device_t dev, uint32_t *sid,
                                         struct cryptoini *cri)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,cryptodev_newsession);
	return ((cryptodev_newsession_t *) _m)(dev, sid, cri);
}

/** @brief Unique descriptor for the CRYPTODEV_FREESESSION() method */
extern struct kobjop_desc cryptodev_freesession_desc;
/** @brief A function implementing the CRYPTODEV_FREESESSION() method */
typedef int cryptodev_freesession_t(device_t dev, uint64_t sid);

static __inline int CRYPTODEV_FREESESSION(device_t dev, uint64_t sid)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,cryptodev_freesession);
	return ((cryptodev_freesession_t *) _m)(dev, sid);
}

/** @brief Unique descriptor for the CRYPTODEV_PROCESS() method */
extern struct kobjop_desc cryptodev_process_desc;
/** @brief A function implementing the CRYPTODEV_PROCESS() method */
typedef int cryptodev_process_t(device_t dev, struct cryptop *op, int flags);

static __inline int CRYPTODEV_PROCESS(device_t dev, struct cryptop *op,
                                      int flags)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,cryptodev_process);
	return ((cryptodev_process_t *) _m)(dev, op, flags);
}

/** @brief Unique descriptor for the CRYPTODEV_KPROCESS() method */
extern struct kobjop_desc cryptodev_kprocess_desc;
/** @brief A function implementing the CRYPTODEV_KPROCESS() method */
typedef int cryptodev_kprocess_t(device_t dev, struct cryptkop *op, int flags);

static __inline int CRYPTODEV_KPROCESS(device_t dev, struct cryptkop *op,
                                       int flags)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,cryptodev_kprocess);
	return ((cryptodev_kprocess_t *) _m)(dev, op, flags);
}

#endif /* _cryptodev_if_h_ */
