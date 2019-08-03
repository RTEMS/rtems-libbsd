/*
 * This file is @generated automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   freebsd-org/sys/dev/hdmi/hdmi_if.m
 * with
 *   makeobjops.awk
 *
 * See the source file for legal information
 */


#ifndef _hdmi_if_h_
#define _hdmi_if_h_


#include <sys/eventhandler.h>

typedef void (*hdmi_event_hook)(void *, device_t, int);
EVENTHANDLER_DECLARE(hdmi_event, hdmi_event_hook);

#define HDMI_EVENT_CONNECTED	0

/** @brief Unique descriptor for the HDMI_GET_EDID() method */
extern struct kobjop_desc hdmi_get_edid_desc;
/** @brief A function implementing the HDMI_GET_EDID() method */
typedef int hdmi_get_edid_t(device_t dev, uint8_t **edid,
                            uint32_t *edid_length);

static __inline int HDMI_GET_EDID(device_t dev, uint8_t **edid,
                                  uint32_t *edid_length)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,hdmi_get_edid);
	rc = ((hdmi_get_edid_t *) _m)(dev, edid, edid_length);
	return (rc);
}

/** @brief Unique descriptor for the HDMI_SET_VIDEOMODE() method */
extern struct kobjop_desc hdmi_set_videomode_desc;
/** @brief A function implementing the HDMI_SET_VIDEOMODE() method */
typedef int hdmi_set_videomode_t(device_t dev,
                                 const struct videomode *videomode);

static __inline int HDMI_SET_VIDEOMODE(device_t dev,
                                       const struct videomode *videomode)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,hdmi_set_videomode);
	rc = ((hdmi_set_videomode_t *) _m)(dev, videomode);
	return (rc);
}

/** @brief Unique descriptor for the HDMI_ENABLE() method */
extern struct kobjop_desc hdmi_enable_desc;
/** @brief A function implementing the HDMI_ENABLE() method */
typedef int hdmi_enable_t(device_t dev, int onoff);

static __inline int HDMI_ENABLE(device_t dev, int onoff)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,hdmi_enable);
	rc = ((hdmi_enable_t *) _m)(dev, onoff);
	return (rc);
}

#endif /* _hdmi_if_h_ */
