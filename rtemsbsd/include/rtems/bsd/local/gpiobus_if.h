/*
 * This file is @generated automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   freebsd-org/sys/dev/gpio/gpiobus_if.m
 * with
 *   makeobjops.awk
 *
 * See the source file for legal information
 */


#ifndef _gpiobus_if_h_
#define _gpiobus_if_h_

/** @brief Unique descriptor for the GPIOBUS_ACQUIRE_BUS() method */
extern struct kobjop_desc gpiobus_acquire_bus_desc;
/** @brief A function implementing the GPIOBUS_ACQUIRE_BUS() method */
typedef int gpiobus_acquire_bus_t(device_t busdev, device_t dev, int how);

static __inline int GPIOBUS_ACQUIRE_BUS(device_t busdev, device_t dev, int how)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)busdev)->ops,gpiobus_acquire_bus);
	rc = ((gpiobus_acquire_bus_t *) _m)(busdev, dev, how);
	return (rc);
}

/** @brief Unique descriptor for the GPIOBUS_RELEASE_BUS() method */
extern struct kobjop_desc gpiobus_release_bus_desc;
/** @brief A function implementing the GPIOBUS_RELEASE_BUS() method */
typedef void gpiobus_release_bus_t(device_t busdev, device_t dev);

static __inline void GPIOBUS_RELEASE_BUS(device_t busdev, device_t dev)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)busdev)->ops,gpiobus_release_bus);
	((gpiobus_release_bus_t *) _m)(busdev, dev);
}

/** @brief Unique descriptor for the GPIOBUS_PIN_SET() method */
extern struct kobjop_desc gpiobus_pin_set_desc;
/** @brief A function implementing the GPIOBUS_PIN_SET() method */
typedef int gpiobus_pin_set_t(device_t dev, device_t child, uint32_t pin_num,
                              uint32_t pin_value);

static __inline int GPIOBUS_PIN_SET(device_t dev, device_t child,
                                    uint32_t pin_num, uint32_t pin_value)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,gpiobus_pin_set);
	rc = ((gpiobus_pin_set_t *) _m)(dev, child, pin_num, pin_value);
	return (rc);
}

/** @brief Unique descriptor for the GPIOBUS_PIN_GET() method */
extern struct kobjop_desc gpiobus_pin_get_desc;
/** @brief A function implementing the GPIOBUS_PIN_GET() method */
typedef int gpiobus_pin_get_t(device_t dev, device_t child, uint32_t pin_num,
                              uint32_t *pin_value);

static __inline int GPIOBUS_PIN_GET(device_t dev, device_t child,
                                    uint32_t pin_num, uint32_t *pin_value)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,gpiobus_pin_get);
	rc = ((gpiobus_pin_get_t *) _m)(dev, child, pin_num, pin_value);
	return (rc);
}

/** @brief Unique descriptor for the GPIOBUS_PIN_TOGGLE() method */
extern struct kobjop_desc gpiobus_pin_toggle_desc;
/** @brief A function implementing the GPIOBUS_PIN_TOGGLE() method */
typedef int gpiobus_pin_toggle_t(device_t dev, device_t child,
                                 uint32_t pin_num);

static __inline int GPIOBUS_PIN_TOGGLE(device_t dev, device_t child,
                                       uint32_t pin_num)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,gpiobus_pin_toggle);
	rc = ((gpiobus_pin_toggle_t *) _m)(dev, child, pin_num);
	return (rc);
}

/** @brief Unique descriptor for the GPIOBUS_PIN_GETCAPS() method */
extern struct kobjop_desc gpiobus_pin_getcaps_desc;
/** @brief A function implementing the GPIOBUS_PIN_GETCAPS() method */
typedef int gpiobus_pin_getcaps_t(device_t dev, device_t child,
                                  uint32_t pin_num, uint32_t *caps);

static __inline int GPIOBUS_PIN_GETCAPS(device_t dev, device_t child,
                                        uint32_t pin_num, uint32_t *caps)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,gpiobus_pin_getcaps);
	rc = ((gpiobus_pin_getcaps_t *) _m)(dev, child, pin_num, caps);
	return (rc);
}

/** @brief Unique descriptor for the GPIOBUS_PIN_GETFLAGS() method */
extern struct kobjop_desc gpiobus_pin_getflags_desc;
/** @brief A function implementing the GPIOBUS_PIN_GETFLAGS() method */
typedef int gpiobus_pin_getflags_t(device_t dev, device_t child,
                                   uint32_t pin_num, uint32_t *flags);

static __inline int GPIOBUS_PIN_GETFLAGS(device_t dev, device_t child,
                                         uint32_t pin_num, uint32_t *flags)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,gpiobus_pin_getflags);
	rc = ((gpiobus_pin_getflags_t *) _m)(dev, child, pin_num, flags);
	return (rc);
}

/** @brief Unique descriptor for the GPIOBUS_PIN_SETFLAGS() method */
extern struct kobjop_desc gpiobus_pin_setflags_desc;
/** @brief A function implementing the GPIOBUS_PIN_SETFLAGS() method */
typedef int gpiobus_pin_setflags_t(device_t dev, device_t child,
                                   uint32_t pin_num, uint32_t flags);

static __inline int GPIOBUS_PIN_SETFLAGS(device_t dev, device_t child,
                                         uint32_t pin_num, uint32_t flags)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,gpiobus_pin_setflags);
	rc = ((gpiobus_pin_setflags_t *) _m)(dev, child, pin_num, flags);
	return (rc);
}

/** @brief Unique descriptor for the GPIOBUS_PIN_GETNAME() method */
extern struct kobjop_desc gpiobus_pin_getname_desc;
/** @brief A function implementing the GPIOBUS_PIN_GETNAME() method */
typedef int gpiobus_pin_getname_t(device_t dev, uint32_t pin_num, char *name);

static __inline int GPIOBUS_PIN_GETNAME(device_t dev, uint32_t pin_num,
                                        char *name)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,gpiobus_pin_getname);
	rc = ((gpiobus_pin_getname_t *) _m)(dev, pin_num, name);
	return (rc);
}

/** @brief Unique descriptor for the GPIOBUS_PIN_SETNAME() method */
extern struct kobjop_desc gpiobus_pin_setname_desc;
/** @brief A function implementing the GPIOBUS_PIN_SETNAME() method */
typedef int gpiobus_pin_setname_t(device_t dev, uint32_t pin_num,
                                  const char *name);

static __inline int GPIOBUS_PIN_SETNAME(device_t dev, uint32_t pin_num,
                                        const char *name)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,gpiobus_pin_setname);
	rc = ((gpiobus_pin_setname_t *) _m)(dev, pin_num, name);
	return (rc);
}

#endif /* _gpiobus_if_h_ */
