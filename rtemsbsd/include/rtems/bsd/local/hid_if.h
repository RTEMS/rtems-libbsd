/*
 * This file is @generated automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   freebsd-org/sys/dev/hid/hid_if.m
 * with
 *   makeobjops.awk
 *
 * See the source file for legal information
 */


#ifndef _hid_if_h_
#define _hid_if_h_

/** @brief Unique descriptor for the HID_INTR_SETUP() method */
extern struct kobjop_desc hid_intr_setup_desc;
/** @brief A function implementing the HID_INTR_SETUP() method */
typedef void hid_intr_setup_t(device_t dev, device_t child, hid_intr_t intr,
                              void *context, struct hid_rdesc_info *rdesc);

static __inline void HID_INTR_SETUP(device_t dev, device_t child,
                                    hid_intr_t intr, void *context,
                                    struct hid_rdesc_info *rdesc)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,hid_intr_setup);
	((hid_intr_setup_t *) _m)(dev, child, intr, context, rdesc);
}

/** @brief Unique descriptor for the HID_INTR_UNSETUP() method */
extern struct kobjop_desc hid_intr_unsetup_desc;
/** @brief A function implementing the HID_INTR_UNSETUP() method */
typedef void hid_intr_unsetup_t(device_t dev, device_t child);

static __inline void HID_INTR_UNSETUP(device_t dev, device_t child)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,hid_intr_unsetup);
	((hid_intr_unsetup_t *) _m)(dev, child);
}

/** @brief Unique descriptor for the HID_INTR_START() method */
extern struct kobjop_desc hid_intr_start_desc;
/** @brief A function implementing the HID_INTR_START() method */
typedef int hid_intr_start_t(device_t dev, device_t child);

static __inline int HID_INTR_START(device_t dev, device_t child)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,hid_intr_start);
	rc = ((hid_intr_start_t *) _m)(dev, child);
	return (rc);
}

/** @brief Unique descriptor for the HID_INTR_STOP() method */
extern struct kobjop_desc hid_intr_stop_desc;
/** @brief A function implementing the HID_INTR_STOP() method */
typedef int hid_intr_stop_t(device_t dev, device_t child);

static __inline int HID_INTR_STOP(device_t dev, device_t child)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,hid_intr_stop);
	rc = ((hid_intr_stop_t *) _m)(dev, child);
	return (rc);
}

/** @brief Unique descriptor for the HID_INTR_POLL() method */
extern struct kobjop_desc hid_intr_poll_desc;
/** @brief A function implementing the HID_INTR_POLL() method */
typedef void hid_intr_poll_t(device_t dev, device_t child);

static __inline void HID_INTR_POLL(device_t dev, device_t child)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,hid_intr_poll);
	((hid_intr_poll_t *) _m)(dev, child);
}

/** @brief Unique descriptor for the HID_GET_RDESC() method */
extern struct kobjop_desc hid_get_rdesc_desc;
/** @brief A function implementing the HID_GET_RDESC() method */
typedef int hid_get_rdesc_t(device_t dev, device_t child, void *data,
                            hid_size_t len);

static __inline int HID_GET_RDESC(device_t dev, device_t child, void *data,
                                  hid_size_t len)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,hid_get_rdesc);
	rc = ((hid_get_rdesc_t *) _m)(dev, child, data, len);
	return (rc);
}

/** @brief Unique descriptor for the HID_READ() method */
extern struct kobjop_desc hid_read_desc;
/** @brief A function implementing the HID_READ() method */
typedef int hid_read_t(device_t dev, device_t child, void *data,
                       hid_size_t maxlen, hid_size_t *actlen);

static __inline int HID_READ(device_t dev, device_t child, void *data,
                             hid_size_t maxlen, hid_size_t *actlen)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,hid_read);
	rc = ((hid_read_t *) _m)(dev, child, data, maxlen, actlen);
	return (rc);
}

/** @brief Unique descriptor for the HID_WRITE() method */
extern struct kobjop_desc hid_write_desc;
/** @brief A function implementing the HID_WRITE() method */
typedef int hid_write_t(device_t dev, device_t child, const void *data,
                        hid_size_t len);

static __inline int HID_WRITE(device_t dev, device_t child, const void *data,
                              hid_size_t len)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,hid_write);
	rc = ((hid_write_t *) _m)(dev, child, data, len);
	return (rc);
}

/** @brief Unique descriptor for the HID_GET_REPORT() method */
extern struct kobjop_desc hid_get_report_desc;
/** @brief A function implementing the HID_GET_REPORT() method */
typedef int hid_get_report_t(device_t dev, device_t child, void *data,
                             hid_size_t maxlen, hid_size_t *actlen,
                             uint8_t type, uint8_t id);

static __inline int HID_GET_REPORT(device_t dev, device_t child, void *data,
                                   hid_size_t maxlen, hid_size_t *actlen,
                                   uint8_t type, uint8_t id)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,hid_get_report);
	rc = ((hid_get_report_t *) _m)(dev, child, data, maxlen, actlen, type, id);
	return (rc);
}

/** @brief Unique descriptor for the HID_SET_REPORT() method */
extern struct kobjop_desc hid_set_report_desc;
/** @brief A function implementing the HID_SET_REPORT() method */
typedef int hid_set_report_t(device_t dev, device_t child, const void *data,
                             hid_size_t len, uint8_t type, uint8_t id);

static __inline int HID_SET_REPORT(device_t dev, device_t child,
                                   const void *data, hid_size_t len,
                                   uint8_t type, uint8_t id)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,hid_set_report);
	rc = ((hid_set_report_t *) _m)(dev, child, data, len, type, id);
	return (rc);
}

/** @brief Unique descriptor for the HID_SET_IDLE() method */
extern struct kobjop_desc hid_set_idle_desc;
/** @brief A function implementing the HID_SET_IDLE() method */
typedef int hid_set_idle_t(device_t dev, device_t child, uint16_t duration,
                           uint8_t id);

static __inline int HID_SET_IDLE(device_t dev, device_t child,
                                 uint16_t duration, uint8_t id)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,hid_set_idle);
	rc = ((hid_set_idle_t *) _m)(dev, child, duration, id);
	return (rc);
}

/** @brief Unique descriptor for the HID_SET_PROTOCOL() method */
extern struct kobjop_desc hid_set_protocol_desc;
/** @brief A function implementing the HID_SET_PROTOCOL() method */
typedef int hid_set_protocol_t(device_t dev, device_t child, uint16_t protocol);

static __inline int HID_SET_PROTOCOL(device_t dev, device_t child,
                                     uint16_t protocol)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,hid_set_protocol);
	rc = ((hid_set_protocol_t *) _m)(dev, child, protocol);
	return (rc);
}

/** @brief Unique descriptor for the HID_IOCTL() method */
extern struct kobjop_desc hid_ioctl_desc;
/** @brief A function implementing the HID_IOCTL() method */
typedef int hid_ioctl_t(device_t dev, device_t child, unsigned long cmd,
                        uintptr_t data);

static __inline int HID_IOCTL(device_t dev, device_t child, unsigned long cmd,
                              uintptr_t data)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,hid_ioctl);
	rc = ((hid_ioctl_t *) _m)(dev, child, cmd, data);
	return (rc);
}

#endif /* _hid_if_h_ */
