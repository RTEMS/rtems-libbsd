/*
 * This file is produced automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   freebsd-org/sys/dev/iicbus/iicbus_if.m
 * with
 *   makeobjops.awk
 *
 * See the source file for legal information
 */


#ifndef _iicbus_if_h_
#define _iicbus_if_h_

/** @brief Unique descriptor for the IICBUS_INTR() method */
extern struct kobjop_desc iicbus_intr_desc;
/** @brief A function implementing the IICBUS_INTR() method */
typedef int iicbus_intr_t(device_t dev, int event, char *buf);

static __inline int IICBUS_INTR(device_t dev, int event, char *buf)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,iicbus_intr);
	rc = ((iicbus_intr_t *) _m)(dev, event, buf);
	return (rc);
}

/** @brief Unique descriptor for the IICBUS_CALLBACK() method */
extern struct kobjop_desc iicbus_callback_desc;
/** @brief A function implementing the IICBUS_CALLBACK() method */
typedef int iicbus_callback_t(device_t dev, int index, caddr_t data);

static __inline int IICBUS_CALLBACK(device_t dev, int index, caddr_t data)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,iicbus_callback);
	rc = ((iicbus_callback_t *) _m)(dev, index, data);
	return (rc);
}

/** @brief Unique descriptor for the IICBUS_REPEATED_START() method */
extern struct kobjop_desc iicbus_repeated_start_desc;
/** @brief A function implementing the IICBUS_REPEATED_START() method */
typedef int iicbus_repeated_start_t(device_t dev, u_char slave, int timeout);

static __inline int IICBUS_REPEATED_START(device_t dev, u_char slave,
                                          int timeout)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,iicbus_repeated_start);
	rc = ((iicbus_repeated_start_t *) _m)(dev, slave, timeout);
	return (rc);
}

/** @brief Unique descriptor for the IICBUS_START() method */
extern struct kobjop_desc iicbus_start_desc;
/** @brief A function implementing the IICBUS_START() method */
typedef int iicbus_start_t(device_t dev, u_char slave, int timeout);

static __inline int IICBUS_START(device_t dev, u_char slave, int timeout)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,iicbus_start);
	rc = ((iicbus_start_t *) _m)(dev, slave, timeout);
	return (rc);
}

/** @brief Unique descriptor for the IICBUS_STOP() method */
extern struct kobjop_desc iicbus_stop_desc;
/** @brief A function implementing the IICBUS_STOP() method */
typedef int iicbus_stop_t(device_t dev);

static __inline int IICBUS_STOP(device_t dev)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,iicbus_stop);
	rc = ((iicbus_stop_t *) _m)(dev);
	return (rc);
}

/** @brief Unique descriptor for the IICBUS_READ() method */
extern struct kobjop_desc iicbus_read_desc;
/** @brief A function implementing the IICBUS_READ() method */
typedef int iicbus_read_t(device_t dev, char *buf, int len, int *bytes,
                          int last, int delay);

static __inline int IICBUS_READ(device_t dev, char *buf, int len, int *bytes,
                                int last, int delay)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,iicbus_read);
	rc = ((iicbus_read_t *) _m)(dev, buf, len, bytes, last, delay);
	return (rc);
}

/** @brief Unique descriptor for the IICBUS_WRITE() method */
extern struct kobjop_desc iicbus_write_desc;
/** @brief A function implementing the IICBUS_WRITE() method */
typedef int iicbus_write_t(device_t dev, const char *buf, int len, int *bytes,
                           int timeout);

static __inline int IICBUS_WRITE(device_t dev, const char *buf, int len,
                                 int *bytes, int timeout)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,iicbus_write);
	rc = ((iicbus_write_t *) _m)(dev, buf, len, bytes, timeout);
	return (rc);
}

/** @brief Unique descriptor for the IICBUS_RESET() method */
extern struct kobjop_desc iicbus_reset_desc;
/** @brief A function implementing the IICBUS_RESET() method */
typedef int iicbus_reset_t(device_t dev, u_char speed, u_char addr,
                           u_char *oldaddr);

static __inline int IICBUS_RESET(device_t dev, u_char speed, u_char addr,
                                 u_char *oldaddr)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,iicbus_reset);
	rc = ((iicbus_reset_t *) _m)(dev, speed, addr, oldaddr);
	return (rc);
}

/** @brief Unique descriptor for the IICBUS_TRANSFER() method */
extern struct kobjop_desc iicbus_transfer_desc;
/** @brief A function implementing the IICBUS_TRANSFER() method */
typedef int iicbus_transfer_t(device_t dev, struct iic_msg *msgs,
                              uint32_t nmsgs);

static __inline int IICBUS_TRANSFER(device_t dev, struct iic_msg *msgs,
                                    uint32_t nmsgs)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,iicbus_transfer);
	rc = ((iicbus_transfer_t *) _m)(dev, msgs, nmsgs);
	return (rc);
}

/** @brief Unique descriptor for the IICBUS_GET_FREQUENCY() method */
extern struct kobjop_desc iicbus_get_frequency_desc;
/** @brief A function implementing the IICBUS_GET_FREQUENCY() method */
typedef u_int iicbus_get_frequency_t(device_t dev, u_char speed);

static __inline u_int IICBUS_GET_FREQUENCY(device_t dev, u_char speed)
{
	kobjop_t _m;
	u_int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,iicbus_get_frequency);
	rc = ((iicbus_get_frequency_t *) _m)(dev, speed);
	return (rc);
}

#endif /* _iicbus_if_h_ */
