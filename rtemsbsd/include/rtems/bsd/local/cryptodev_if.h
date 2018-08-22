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
typedef int cryptodev_newsession_t(device_t dev,
                                   crypto_session_t crypto_session,
                                   struct cryptoini *cri);
/**
 * Crypto driver method to initialize a new session object with the given
 * initialization parameters (cryptoini).  The driver's session memory object
 * is already allocated and zeroed, like driver softcs.  It is accessed with
 * crypto_get_driver_session().
 */

static __inline int CRYPTODEV_NEWSESSION(device_t dev,
                                         crypto_session_t crypto_session,
                                         struct cryptoini *cri)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,cryptodev_newsession);
	rc = ((cryptodev_newsession_t *) _m)(dev, crypto_session, cri);
	return (rc);
}

/** @brief Unique descriptor for the CRYPTODEV_FREESESSION() method */
extern struct kobjop_desc cryptodev_freesession_desc;
/** @brief A function implementing the CRYPTODEV_FREESESSION() method */
typedef void cryptodev_freesession_t(device_t dev,
                                     crypto_session_t crypto_session);
/**
 * Optional crypto driver method to release any additional allocations.  OCF
 * owns session memory itself; it is zeroed before release.
 */

static __inline void CRYPTODEV_FREESESSION(device_t dev,
                                           crypto_session_t crypto_session)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,cryptodev_freesession);
	((cryptodev_freesession_t *) _m)(dev, crypto_session);
}

/** @brief Unique descriptor for the CRYPTODEV_PROCESS() method */
extern struct kobjop_desc cryptodev_process_desc;
/** @brief A function implementing the CRYPTODEV_PROCESS() method */
typedef int cryptodev_process_t(device_t dev, struct cryptop *op, int flags);

static __inline int CRYPTODEV_PROCESS(device_t dev, struct cryptop *op,
                                      int flags)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,cryptodev_process);
	rc = ((cryptodev_process_t *) _m)(dev, op, flags);
	return (rc);
}

/** @brief Unique descriptor for the CRYPTODEV_KPROCESS() method */
extern struct kobjop_desc cryptodev_kprocess_desc;
/** @brief A function implementing the CRYPTODEV_KPROCESS() method */
typedef int cryptodev_kprocess_t(device_t dev, struct cryptkop *op, int flags);

static __inline int CRYPTODEV_KPROCESS(device_t dev, struct cryptkop *op,
                                       int flags)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,cryptodev_kprocess);
	rc = ((cryptodev_kprocess_t *) _m)(dev, op, flags);
	return (rc);
}

#endif /* _cryptodev_if_h_ */
