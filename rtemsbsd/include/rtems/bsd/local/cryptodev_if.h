/*
 * This file is @generated automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   freebsd-org/sys/opencrypto/cryptodev_if.m
 * with
 *   makeobjops.awk
 *
 * See the source file for legal information
 */


#ifndef _cryptodev_if_h_
#define _cryptodev_if_h_

/** @brief Unique descriptor for the CRYPTODEV_PROBESESSION() method */
extern struct kobjop_desc cryptodev_probesession_desc;
/** @brief A function implementing the CRYPTODEV_PROBESESSION() method */
typedef int cryptodev_probesession_t(device_t dev,
                                     const struct crypto_session_params *csp);
/**
 * @brief Probe to see if a crypto driver supports a session.
 *
 * The crypto framework invokes this method on each crypto driver when
 * creating a session for symmetric crypto operations to determine if
 * the driver supports the algorithms and mode requested by the
 * session.
 *
 * If the driver does not support a session with the requested
 * parameters, this function should fail with an error.
 *
 * If the driver does support a session with the requested parameters,
 * this function should return a negative value indicating the
 * priority of this driver.  These negative values should be derived
 * from one of the CRYPTODEV_PROBE_* constants in
 * <opencrypto/cryptodev.h>.
 *
 * This function's return value is similar to that used by
 * DEVICE_PROBE(9).  However, a return value of zero is not supported
 * and should not be used.
 *
 * @param dev		the crypto driver device
 * @param csp		crypto session parameters
 *
 * @retval negative	if the driver supports this session - the
 *			least negative value is used to select the
 *			driver for the session
 * @retval EINVAL	if the driver does not support the session
 * @retval positive	if some other error occurs
 */

static __inline int CRYPTODEV_PROBESESSION(device_t dev,
                                           const struct crypto_session_params *csp)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,cryptodev_probesession);
	rc = ((cryptodev_probesession_t *) _m)(dev, csp);
	return (rc);
}

/** @brief Unique descriptor for the CRYPTODEV_NEWSESSION() method */
extern struct kobjop_desc cryptodev_newsession_desc;
/** @brief A function implementing the CRYPTODEV_NEWSESSION() method */
typedef int cryptodev_newsession_t(device_t dev,
                                   crypto_session_t crypto_session,
                                   const struct crypto_session_params *csp);
/**
 * @brief Initialize a new crypto session object
 *
 * Invoked by the crypto framework to initialize driver-specific data
 * for a crypto session.  The framework allocates and zeroes the
 * driver's per-session memory object prior to invoking this method.
 * The driver is able to access it's per-session memory object via
 * crypto_get_driver_session().
 *
 * @param dev		the crypto driver device
 * @param crypto_session session being initialized
 * @param csp		crypto session parameters
 *
 * @retval 0		success
 * @retval non-zero	if some kind of error occurred
 */

static __inline int CRYPTODEV_NEWSESSION(device_t dev,
                                         crypto_session_t crypto_session,
                                         const struct crypto_session_params *csp)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,cryptodev_newsession);
	rc = ((cryptodev_newsession_t *) _m)(dev, crypto_session, csp);
	return (rc);
}

/** @brief Unique descriptor for the CRYPTODEV_FREESESSION() method */
extern struct kobjop_desc cryptodev_freesession_desc;
/** @brief A function implementing the CRYPTODEV_FREESESSION() method */
typedef void cryptodev_freesession_t(device_t dev,
                                     crypto_session_t crypto_session);
/**
 * @brief Destroy a crypto session object
 *
 * The crypto framework invokes this method when tearing down a crypto
 * session.  After this callback returns, the framework will explicitly
 * zero and free the drvier's per-session memory object.  If the
 * driver requires additional actions to destroy a session, it should
 * perform those in this method.  If the driver does not require
 * additional actions it does not need to provide an implementation of
 * this method.
 *
 * @param dev		the crypto driver device
 * @param crypto_session session being destroyed
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
/**
 * @brief Perform a crypto operation
 *
 * The crypto framework invokes this method for each crypto
 * operation performed on a session.  A reference to the containing
 * session is stored as a member of 'struct cryptop'.  This routine
 * should not block, but queue the operation if necessary.
 *
 * This method may return ERESTART to indicate that any internal
 * queues are full so the operation should be queued in the crypto
 * framework and retried in the future.
 *
 * To report errors with a crypto operation, 'crp_etype' should be set
 * and the operation completed by calling 'crypto_done'.  This method
 * should then return zero.
 *
 * @param dev		the crypto driver device
 * @param op		crypto operation to perform
 * @param flags		set to CRYPTO_HINT_MORE if additional symmetric
 *			crypto operations are queued for this driver;
 *			otherwise set to zero.
 *
 * @retval 0		success
 * @retval ERESTART	internal queue is full
 */

static __inline int CRYPTODEV_PROCESS(device_t dev, struct cryptop *op,
                                      int flags)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,cryptodev_process);
	rc = ((cryptodev_process_t *) _m)(dev, op, flags);
	return (rc);
}

#endif /* _cryptodev_if_h_ */
