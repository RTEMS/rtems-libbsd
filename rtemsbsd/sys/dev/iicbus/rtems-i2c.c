/*
 * Copyright (c) 2019 Vijay Kumar Banerjee <vijaykumar9597@gmail.com>.
 * All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <machine/rtems-bsd-kernel-space.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#include <sys/module.h>
#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/errno.h>
#include <sys/bus.h>

#include <dev/iicbus/iicbus.h>
#include <dev/iicbus/iic.h>
#include <dev/iicbus/iiconf.h>
#include <dev/ofw/ofw_bus.h>
#include <dev/ofw/ofw_bus_subr.h>
#include <dev/ofw/openfirm.h>

#include <rtems/bsd/local/iicbus_if.h>

#include <linux/i2c.h>
#include <linux/i2c-dev.h>

typedef struct i2c_msg i2c_msg;

struct i2c_softc {
	device_t dev;
	device_t sc_iicbus;
	char *path;
};

static int
rtems_i2c_probe(device_t dev)
{
	if (!ofw_bus_status_okay(dev)) {
		return (ENXIO);
	}
	if (!ofw_bus_is_compatible(dev, "rtems,bsp-i2c")){
		return (ENXIO);
	}

	device_set_desc(dev, "RTEMS libbsd I2C");
	return (BUS_PROBE_SPECIFIC);
}

static int
rtems_i2c_attach(device_t dev)
{
	phandle_t node;
	ssize_t compatlen;
	char *compat;
	char *curstr;
	struct i2c_softc *sc;
	int len;

	sc = device_get_softc(dev);
	sc->dev = dev;
	node = ofw_bus_get_node(sc->dev);

	len = OF_getprop_alloc(node, "rtems,i2c-path", &sc->path);
	if (len == -1){
		device_printf(sc->dev, "Path not found in Device Tree");
		OF_prop_free(sc->path);
		return (ENXIO);
	}

	if ((sc->sc_iicbus = device_add_child(sc->dev, "iicbus", -1)) == NULL) {
		device_printf(sc->dev, "could not allocate iicbus instance\n");
		OF_prop_free(sc->path);
		return (ENXIO);
	}
	config_intrhook_oneshot((ich_func_t)bus_generic_attach, sc->dev);

	return (0);
}

static int
rtems_i2c_detach(device_t dev)
{
	struct i2c_softc *sc;
	int error;

	sc = device_get_softc(dev);

	if ((error = bus_generic_detach(sc->dev)) != 0) {
		device_printf(sc->dev, "cannot detach child devices\n");
		return (error);
	}

	if (sc->sc_iicbus && (error = device_delete_child(dev, sc->sc_iicbus)) != 0)
		return (error);

	OF_prop_free(sc->path);

	return (0);
}

static int
rtems_i2c_transfer(device_t dev, struct iic_msg *msgs, u_int num)
{
	i2c_msg *messages;
	int err = 0;
	int fd;
	char *addr;
	struct i2c_softc *sc;
	struct i2c_rdwr_ioctl_data ioctl_data;
	uint16_t newflags = 0;

	sc = device_get_softc(dev);

	/* Open /dev/iic0 */
	fd = open(sc->path, O_RDWR);
	if (fd < 0) {
		device_printf(sc->dev, "RTEMS I2C device could not be opened  ");
		return errno;
	}

	/* cast iic_msg to i2c_msg */
	messages = (i2c_msg *) msgs;

	for(int i = 0; i < num; ++i){
		messages[i].addr = messages[i].addr >> 1;
		if (messages[i].flags & IIC_M_RD){
			newflags |= I2C_M_RD;
		}
		else if ((messages[i].flags & IIC_M_NOSTOP) == 0){
			newflags |= I2C_M_STOP;
		}
		else if (messages[i].flags & IIC_M_NOSTART){
			newflags |= I2C_M_NOSTART;
		}
		else
			newflags = 0;
		messages[i].flags = newflags;
	}

	ioctl_data.msgs = messages;
	ioctl_data.nmsgs = num;

	/* IOCTL call to write */
	err = ioctl(fd, I2C_RDWR, &ioctl_data);
	if (err == -1) {
		err = errno;
	}

	/* Close the device */
	close(fd);

	return (err);
}

static int
rtems_i2c_reset(device_t dev, u_char speed, u_char addr, u_char *oldaddr)
{
	return (0);
}

static phandle_t
rtems_i2c_get_node(device_t bus, device_t dev)
{

	/* Share controller node with iibus device. */
	return (ofw_bus_get_node(bus));
}

static device_method_t rtems_i2c_methods[] = {
	/* Device Interface */
	DEVMETHOD(device_probe,		rtems_i2c_probe),
	DEVMETHOD(device_attach,	rtems_i2c_attach),
	DEVMETHOD(device_detach,	rtems_i2c_detach),

	/* Bus interface */
	DEVMETHOD(bus_setup_intr,	bus_generic_setup_intr),
	DEVMETHOD(bus_teardown_intr,	bus_generic_teardown_intr),
	DEVMETHOD(bus_alloc_resource,	bus_generic_alloc_resource),
	DEVMETHOD(bus_release_resource, bus_generic_release_resource),
	DEVMETHOD(bus_activate_resource, bus_generic_activate_resource),
	DEVMETHOD(bus_deactivate_resource, bus_generic_deactivate_resource),
	DEVMETHOD(bus_adjust_resource,	bus_generic_adjust_resource),
	DEVMETHOD(bus_set_resource, bus_generic_rl_set_resource),
	DEVMETHOD(bus_get_resource, bus_generic_rl_get_resource),

	/* OFW methods */
	DEVMETHOD(ofw_bus_get_node, rtems_i2c_get_node),

	/* iicbus interface */
	DEVMETHOD(iicbus_callback,	iicbus_null_callback),
	DEVMETHOD(iicbus_reset,		rtems_i2c_reset),
	DEVMETHOD(iicbus_transfer,	rtems_i2c_transfer),

	DEVMETHOD_END
};

static driver_t rtems_i2c_driver = {
	"rtems_i2c",
	rtems_i2c_methods,
	sizeof(struct i2c_softc),
};

static devclass_t rtems_i2c_devclass;
DRIVER_MODULE(rtems_i2c, simplebus, rtems_i2c_driver, rtems_i2c_devclass, 0, 0);
DRIVER_MODULE(iicbus, rtems_i2c, iicbus_driver, iicbus_devclass, 0, 0);
DRIVER_MODULE(ofw_iicbus, rtems_i2c, ofw_iicbus_driver, ofw_iicbus_devclass, 0, 0);
