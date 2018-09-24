/*
 * This file is produced automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   freebsd-org/sys/dev/gpio/gpio_if.m
 * with
 *   makeobjops.awk
 *
 * See the source file for legal information
 */


#ifndef _gpio_if_h_
#define _gpio_if_h_


#include <dev/ofw/openfirm.h>

/** @brief Unique descriptor for the GPIO_GET_BUS() method */
extern struct kobjop_desc gpio_get_bus_desc;
/** @brief A function implementing the GPIO_GET_BUS() method */
typedef device_t gpio_get_bus_t(device_t dev);

static __inline device_t GPIO_GET_BUS(device_t dev)
{
	kobjop_t _m;
	device_t rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,gpio_get_bus);
	rc = ((gpio_get_bus_t *) _m)(dev);
	return (rc);
}

/** @brief Unique descriptor for the GPIO_PIN_MAX() method */
extern struct kobjop_desc gpio_pin_max_desc;
/** @brief A function implementing the GPIO_PIN_MAX() method */
typedef int gpio_pin_max_t(device_t dev, int *maxpin);

static __inline int GPIO_PIN_MAX(device_t dev, int *maxpin)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,gpio_pin_max);
	rc = ((gpio_pin_max_t *) _m)(dev, maxpin);
	return (rc);
}

/** @brief Unique descriptor for the GPIO_PIN_SET() method */
extern struct kobjop_desc gpio_pin_set_desc;
/** @brief A function implementing the GPIO_PIN_SET() method */
typedef int gpio_pin_set_t(device_t dev, uint32_t pin_num, uint32_t pin_value);

static __inline int GPIO_PIN_SET(device_t dev, uint32_t pin_num,
                                 uint32_t pin_value)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,gpio_pin_set);
	rc = ((gpio_pin_set_t *) _m)(dev, pin_num, pin_value);
	return (rc);
}

/** @brief Unique descriptor for the GPIO_PIN_GET() method */
extern struct kobjop_desc gpio_pin_get_desc;
/** @brief A function implementing the GPIO_PIN_GET() method */
typedef int gpio_pin_get_t(device_t dev, uint32_t pin_num, uint32_t *pin_value);

static __inline int GPIO_PIN_GET(device_t dev, uint32_t pin_num,
                                 uint32_t *pin_value)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,gpio_pin_get);
	rc = ((gpio_pin_get_t *) _m)(dev, pin_num, pin_value);
	return (rc);
}

/** @brief Unique descriptor for the GPIO_PIN_TOGGLE() method */
extern struct kobjop_desc gpio_pin_toggle_desc;
/** @brief A function implementing the GPIO_PIN_TOGGLE() method */
typedef int gpio_pin_toggle_t(device_t dev, uint32_t pin_num);

static __inline int GPIO_PIN_TOGGLE(device_t dev, uint32_t pin_num)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,gpio_pin_toggle);
	rc = ((gpio_pin_toggle_t *) _m)(dev, pin_num);
	return (rc);
}

/** @brief Unique descriptor for the GPIO_PIN_GETCAPS() method */
extern struct kobjop_desc gpio_pin_getcaps_desc;
/** @brief A function implementing the GPIO_PIN_GETCAPS() method */
typedef int gpio_pin_getcaps_t(device_t dev, uint32_t pin_num, uint32_t *caps);

static __inline int GPIO_PIN_GETCAPS(device_t dev, uint32_t pin_num,
                                     uint32_t *caps)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,gpio_pin_getcaps);
	rc = ((gpio_pin_getcaps_t *) _m)(dev, pin_num, caps);
	return (rc);
}

/** @brief Unique descriptor for the GPIO_PIN_GETFLAGS() method */
extern struct kobjop_desc gpio_pin_getflags_desc;
/** @brief A function implementing the GPIO_PIN_GETFLAGS() method */
typedef int gpio_pin_getflags_t(device_t dev, uint32_t pin_num,
                                uint32_t *flags);

static __inline int GPIO_PIN_GETFLAGS(device_t dev, uint32_t pin_num,
                                      uint32_t *flags)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,gpio_pin_getflags);
	rc = ((gpio_pin_getflags_t *) _m)(dev, pin_num, flags);
	return (rc);
}

/** @brief Unique descriptor for the GPIO_PIN_GETNAME() method */
extern struct kobjop_desc gpio_pin_getname_desc;
/** @brief A function implementing the GPIO_PIN_GETNAME() method */
typedef int gpio_pin_getname_t(device_t dev, uint32_t pin_num, char *name);

static __inline int GPIO_PIN_GETNAME(device_t dev, uint32_t pin_num, char *name)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,gpio_pin_getname);
	rc = ((gpio_pin_getname_t *) _m)(dev, pin_num, name);
	return (rc);
}

/** @brief Unique descriptor for the GPIO_PIN_SETFLAGS() method */
extern struct kobjop_desc gpio_pin_setflags_desc;
/** @brief A function implementing the GPIO_PIN_SETFLAGS() method */
typedef int gpio_pin_setflags_t(device_t dev, uint32_t pin_num, uint32_t flags);

static __inline int GPIO_PIN_SETFLAGS(device_t dev, uint32_t pin_num,
                                      uint32_t flags)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,gpio_pin_setflags);
	rc = ((gpio_pin_setflags_t *) _m)(dev, pin_num, flags);
	return (rc);
}

/** @brief Unique descriptor for the GPIO_MAP_GPIOS() method */
extern struct kobjop_desc gpio_map_gpios_desc;
/** @brief A function implementing the GPIO_MAP_GPIOS() method */
typedef int gpio_map_gpios_t(device_t bus, phandle_t dev, phandle_t gparent,
                             int gcells, pcell_t *gpios, uint32_t *pin,
                             uint32_t *flags);

static __inline int GPIO_MAP_GPIOS(device_t bus, phandle_t dev,
                                   phandle_t gparent, int gcells,
                                   pcell_t *gpios, uint32_t *pin,
                                   uint32_t *flags)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)bus)->ops,gpio_map_gpios);
	rc = ((gpio_map_gpios_t *) _m)(bus, dev, gparent, gcells, gpios, pin, flags);
	return (rc);
}

/** @brief Unique descriptor for the GPIO_PIN_ACCESS_32() method */
extern struct kobjop_desc gpio_pin_access_32_desc;
/** @brief A function implementing the GPIO_PIN_ACCESS_32() method */
typedef int gpio_pin_access_32_t(device_t dev, uint32_t first_pin,
                                 uint32_t clear_pins, uint32_t change_pins,
                                 uint32_t *orig_pins);

static __inline int GPIO_PIN_ACCESS_32(device_t dev, uint32_t first_pin,
                                       uint32_t clear_pins,
                                       uint32_t change_pins,
                                       uint32_t *orig_pins)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,gpio_pin_access_32);
	rc = ((gpio_pin_access_32_t *) _m)(dev, first_pin, clear_pins, change_pins, orig_pins);
	return (rc);
}

/** @brief Unique descriptor for the GPIO_PIN_CONFIG_32() method */
extern struct kobjop_desc gpio_pin_config_32_desc;
/** @brief A function implementing the GPIO_PIN_CONFIG_32() method */
typedef int gpio_pin_config_32_t(device_t dev, uint32_t first_pin,
                                 uint32_t num_pins, uint32_t *pin_flags);

static __inline int GPIO_PIN_CONFIG_32(device_t dev, uint32_t first_pin,
                                       uint32_t num_pins, uint32_t *pin_flags)
{
	kobjop_t _m;
	int rc;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,gpio_pin_config_32);
	rc = ((gpio_pin_config_32_t *) _m)(dev, first_pin, num_pins, pin_flags);
	return (rc);
}

#endif /* _gpio_if_h_ */
