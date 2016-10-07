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
	KOBJOPLOOKUP(((kobj_t)dev)->ops,gpio_get_bus);
	return ((gpio_get_bus_t *) _m)(dev);
}

/** @brief Unique descriptor for the GPIO_PIN_MAX() method */
extern struct kobjop_desc gpio_pin_max_desc;
/** @brief A function implementing the GPIO_PIN_MAX() method */
typedef int gpio_pin_max_t(device_t dev, int *maxpin);

static __inline int GPIO_PIN_MAX(device_t dev, int *maxpin)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,gpio_pin_max);
	return ((gpio_pin_max_t *) _m)(dev, maxpin);
}

/** @brief Unique descriptor for the GPIO_PIN_SET() method */
extern struct kobjop_desc gpio_pin_set_desc;
/** @brief A function implementing the GPIO_PIN_SET() method */
typedef int gpio_pin_set_t(device_t dev, uint32_t pin_num, uint32_t pin_value);

static __inline int GPIO_PIN_SET(device_t dev, uint32_t pin_num,
                                 uint32_t pin_value)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,gpio_pin_set);
	return ((gpio_pin_set_t *) _m)(dev, pin_num, pin_value);
}

/** @brief Unique descriptor for the GPIO_PIN_GET() method */
extern struct kobjop_desc gpio_pin_get_desc;
/** @brief A function implementing the GPIO_PIN_GET() method */
typedef int gpio_pin_get_t(device_t dev, uint32_t pin_num, uint32_t *pin_value);

static __inline int GPIO_PIN_GET(device_t dev, uint32_t pin_num,
                                 uint32_t *pin_value)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,gpio_pin_get);
	return ((gpio_pin_get_t *) _m)(dev, pin_num, pin_value);
}

/** @brief Unique descriptor for the GPIO_PIN_TOGGLE() method */
extern struct kobjop_desc gpio_pin_toggle_desc;
/** @brief A function implementing the GPIO_PIN_TOGGLE() method */
typedef int gpio_pin_toggle_t(device_t dev, uint32_t pin_num);

static __inline int GPIO_PIN_TOGGLE(device_t dev, uint32_t pin_num)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,gpio_pin_toggle);
	return ((gpio_pin_toggle_t *) _m)(dev, pin_num);
}

/** @brief Unique descriptor for the GPIO_PIN_GETCAPS() method */
extern struct kobjop_desc gpio_pin_getcaps_desc;
/** @brief A function implementing the GPIO_PIN_GETCAPS() method */
typedef int gpio_pin_getcaps_t(device_t dev, uint32_t pin_num, uint32_t *caps);

static __inline int GPIO_PIN_GETCAPS(device_t dev, uint32_t pin_num,
                                     uint32_t *caps)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,gpio_pin_getcaps);
	return ((gpio_pin_getcaps_t *) _m)(dev, pin_num, caps);
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
	KOBJOPLOOKUP(((kobj_t)dev)->ops,gpio_pin_getflags);
	return ((gpio_pin_getflags_t *) _m)(dev, pin_num, flags);
}

/** @brief Unique descriptor for the GPIO_PIN_GETNAME() method */
extern struct kobjop_desc gpio_pin_getname_desc;
/** @brief A function implementing the GPIO_PIN_GETNAME() method */
typedef int gpio_pin_getname_t(device_t dev, uint32_t pin_num, char *name);

static __inline int GPIO_PIN_GETNAME(device_t dev, uint32_t pin_num, char *name)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,gpio_pin_getname);
	return ((gpio_pin_getname_t *) _m)(dev, pin_num, name);
}

/** @brief Unique descriptor for the GPIO_PIN_SETFLAGS() method */
extern struct kobjop_desc gpio_pin_setflags_desc;
/** @brief A function implementing the GPIO_PIN_SETFLAGS() method */
typedef int gpio_pin_setflags_t(device_t dev, uint32_t pin_num, uint32_t flags);

static __inline int GPIO_PIN_SETFLAGS(device_t dev, uint32_t pin_num,
                                      uint32_t flags)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,gpio_pin_setflags);
	return ((gpio_pin_setflags_t *) _m)(dev, pin_num, flags);
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
	KOBJOPLOOKUP(((kobj_t)bus)->ops,gpio_map_gpios);
	return ((gpio_map_gpios_t *) _m)(bus, dev, gparent, gcells, gpios, pin, flags);
}

#endif /* _gpio_if_h_ */
