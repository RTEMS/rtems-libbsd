#include <machine/rtems-bsd-kernel-space.h>
/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2020 embedded brains GmbH (http://www.embedded-brains.de)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <bsp.h>
#if defined(LIBBSP_ARM_IMX_BSP_H) || defined(LIBBSP_ARM_IMXRT_BSP_H)

#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <sys/param.h>
#include <sys/bus.h>
#include <sys/kernel.h>
#include <sys/module.h>
#include <sys/gpio.h>

#include <dev/fdt/fdt_common.h>
#include <dev/gpio/gpiobusvar.h>
#include <dev/ofw/ofw_bus.h>
#include <dev/ofw/ofw_bus_subr.h>

#include <bsp/imx-gpio.h>

#define MAXPIN		(32)

#define VALID_PIN(u)	((u) >= 0 && (u) <= MAXPIN)

struct imx_rtems_gpio_softc {
	device_t		dev;
	device_t		busdev;
	struct imx_gpio		*imx_gpio;
	struct {
		struct imx_gpio_pin	pin;
		bool			initialized;
		uint32_t		flags;
	} pin[MAXPIN];
};

static device_t
imx_rtems_gpio_get_bus(device_t dev)
{
	struct imx_rtems_gpio_softc *sc = device_get_softc(dev);

	return (sc->busdev);
}

static int
imx_rtems_gpio_pin_max(device_t dev, int *maxpin)
{

	*maxpin = MAXPIN;
	return (0);
}

static int
imx_rtems_gpio_pin_getcaps(device_t dev, uint32_t pin, uint32_t *caps)
{

	if (!VALID_PIN(pin))
		return (EINVAL);

	*caps = (GPIO_PIN_INPUT | GPIO_PIN_OUTPUT);

	return (0);
}

/* Get a specific pin's name. */
static int
imx_rtems_gpio_pin_getname(device_t dev, uint32_t pin, char *name)
{
	struct imx_rtems_gpio_softc *sc = device_get_softc(dev);

	if (!VALID_PIN(pin))
		return (EINVAL);

	snprintf(name, GPIOMAXNAME, "%s_%d",
	    imx_gpio_get_name(sc->imx_gpio), pin);
	name[GPIOMAXNAME-1] = '\0';

	return (0);
}

static void
imx_rtems_gpio_pin_init(struct imx_rtems_gpio_softc *sc, uint32_t pin)
{
	imx_gpio_init(&sc->pin[pin].pin);
	sc->pin[pin].initialized = true;
}

static int
imx_rtems_gpio_pin_set(device_t dev, uint32_t pin, unsigned int value)
{
	struct imx_rtems_gpio_softc *sc = device_get_softc(dev);

	if (!VALID_PIN(pin) || value > 1)
		return (EINVAL);

	if (!sc->pin[pin].initialized) {
		imx_rtems_gpio_pin_init(sc, pin);
	}

	imx_gpio_set_output(&sc->pin[pin].pin, value);

	return (0);
}

static int
imx_rtems_gpio_pin_get(device_t dev, uint32_t pin, unsigned int *value)
{
	struct imx_rtems_gpio_softc *sc = device_get_softc(dev);

	if (!VALID_PIN(pin))
		return (EINVAL);

	if (!sc->pin[pin].initialized) {
		imx_rtems_gpio_pin_init(sc, pin);
	}

	*value = imx_gpio_get_input(&sc->pin[pin].pin);

	return (0);
}

static int
imx_rtems_gpio_pin_toggle(device_t dev, uint32_t pin)
{
	struct imx_rtems_gpio_softc *sc = device_get_softc(dev);

	if (!VALID_PIN(pin))
		return (EINVAL);

	if (!sc->pin[pin].initialized) {
		imx_rtems_gpio_pin_init(sc, pin);
	}

	imx_gpio_toggle_output(&sc->pin[pin].pin);

	return (0);
}

static int
imx_rtems_gpio_pin_getflags(device_t dev, uint32_t pin, uint32_t *flags)
{
	struct imx_rtems_gpio_softc *sc = device_get_softc(dev);

	if (!VALID_PIN(pin))
		return (EINVAL);

	*flags = sc->pin[pin].flags;

	return (0);
}

static int
imx_rtems_gpio_pin_setflags(device_t dev, uint32_t pin, uint32_t flags)
{
	struct imx_rtems_gpio_softc *sc = device_get_softc(dev);

	if (!VALID_PIN(pin))
		return (EINVAL);

	if (flags & (GPIO_PIN_INPUT | GPIO_PIN_OUTPUT)) {
		if (flags & GPIO_PIN_PRESET_LOW) {
			imx_gpio_set_output(&sc->pin[pin].pin, 0);
		} else if (flags & GPIO_PIN_PRESET_HIGH) {
			imx_gpio_set_output(&sc->pin[pin].pin, 1);
		}
		if (flags & GPIO_PIN_OUTPUT) {
			sc->pin[pin].pin.mode = IMX_GPIO_MODE_OUTPUT;
		} else {
			sc->pin[pin].pin.mode = IMX_GPIO_MODE_INPUT;
		}
		imx_rtems_gpio_pin_init(sc, pin);
		sc->pin[pin].flags = flags;
	} else {
		return (EINVAL);
	}

	return (0);
}

static struct ofw_compat_data compat_data[] = {
	{"fsl,imx35-gpio",  1},
	{"fsl,imx7d-gpio",  1},
	{NULL,              0}
};

static int
imx_rtems_gpio_probe(device_t dev)
{
	if (!ofw_bus_status_okay(dev))
		return (ENXIO);

	if (ofw_bus_search_compatible(dev, compat_data)->ocd_data != 0) {
		device_set_desc(dev, "Freescale i.MX GPIO Controller (RTEMS)");
		return (BUS_PROBE_DEFAULT);
	}
	
	return (ENXIO);
}

static int
imx_rtems_gpio_attach(device_t dev)
{
	struct imx_rtems_gpio_softc *sc = device_get_softc(dev);
	u_long base, size;
	size_t i;

	sc->dev = dev;

	/* Get register address */
	if (fdt_regsize(ofw_bus_get_node(sc->dev), &base, &size) != 0) {
		device_printf(dev, "Can't get register address");
		return (EINVAL);
	}

	sc->imx_gpio = imx_gpio_get_by_register((void *)base);
	if (sc->imx_gpio == NULL) {
		device_printf(dev, "Can't get RTEMS imx_gpio handle");
		return (EINVAL);
	}

	memset(sc->pin, 0, sizeof(sc->pin));
	for (i = 0; i < MAXPIN; ++i) {
		sc->pin[i].pin.gpio = sc->imx_gpio;
		sc->pin[i].pin.mask = 1 << i;
		sc->pin[i].pin.shift = i;
		sc->pin[i].pin.mode = IMX_GPIO_MODE_INPUT;
	}
	/*
	 * Do _not_ initialize the pins here. Otherwise we would overwrite prior
	 * initializations done by other application parts.
	 */

	sc->busdev = gpiobus_attach_bus(dev);
	if (sc->busdev == NULL) {
		return (ENOMEM);
	}

	return (0);
}

static int
imx_rtems_gpio_detach(device_t dev)
{
	gpiobus_detach_bus(dev);

	return (0);
}

static device_method_t imx_rtems_gpio_methods[] = {
	/* device_if */
	DEVMETHOD(device_probe,		imx_rtems_gpio_probe),
	DEVMETHOD(device_attach,	imx_rtems_gpio_attach),
	DEVMETHOD(device_detach,	imx_rtems_gpio_detach),

	/* GPIO protocol */
	DEVMETHOD(gpio_get_bus,		imx_rtems_gpio_get_bus),
	DEVMETHOD(gpio_pin_max,		imx_rtems_gpio_pin_max),
	DEVMETHOD(gpio_pin_getname,	imx_rtems_gpio_pin_getname),
	DEVMETHOD(gpio_pin_getflags,	imx_rtems_gpio_pin_getflags),
	DEVMETHOD(gpio_pin_getcaps,	imx_rtems_gpio_pin_getcaps),
	DEVMETHOD(gpio_pin_get,		imx_rtems_gpio_pin_get),
	DEVMETHOD(gpio_pin_setflags,	imx_rtems_gpio_pin_setflags),
	DEVMETHOD(gpio_pin_set,		imx_rtems_gpio_pin_set),
	DEVMETHOD(gpio_pin_toggle,	imx_rtems_gpio_pin_toggle),

	DEVMETHOD_END
};

static driver_t imx_rtems_gpio_driver = {
	"gpio",
	imx_rtems_gpio_methods,
	sizeof(struct imx_rtems_gpio_softc),
};
static devclass_t imx_rtems_gpio_devclass;

EARLY_DRIVER_MODULE(imx_rtems_gpio, simplebus, imx_rtems_gpio_driver,
    imx_rtems_gpio_devclass, NULL, NULL,
    BUS_PASS_RESOURCE + BUS_PASS_ORDER_MIDDLE);

#endif /* LIBBSP_ARM_IMX_BSP_H || LIBBSP_ARM_IMXRT_BSP_H */
