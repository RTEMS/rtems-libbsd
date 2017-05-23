/*
 * Copyright (c) 2009-2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
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
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <machine/rtems-bsd-kernel-space.h>
#include <machine/rtems-bsd-support.h>

#include <bsp.h>

#if defined(__GEN83xx_BSP_h) || defined(LIBBSP_POWERPC_QORIQ_BSP_H)

#include <bsp/irq.h>
#include <bsp/utility.h>

#ifdef __GEN83xx_BSP_h
  #include <mpc83xx/mpc83xx.h>
  #define BSP_EHCI_REGISTER_BASE_ADDR ((unsigned) &mpc83xx.usb_dr.caplength)
  #define BSP_EHCI_IRQ_VECTOR BSP_IPIC_IRQ_USB_DR
#endif

#ifdef LIBBSP_POWERPC_QORIQ_BSP_H
  #include <bsp/qoriq.h>
  #define BSP_EHCI_REGISTER_BASE_ADDR ((unsigned) &qoriq.usb_1 + 0x100)
  #define BSP_EHCI_IRQ_VECTOR QORIQ_IRQ_USB_1
#endif

#include <sys/cdefs.h>
#include <sys/stdint.h>
#include <sys/stddef.h>
#include <sys/param.h>
#include <sys/queue.h>
#include <sys/types.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/bus.h>
#include <sys/linker_set.h>
#include <sys/module.h>
#include <rtems/bsd/sys/lock.h>
#include <sys/mutex.h>
#include <sys/condvar.h>
#include <sys/sysctl.h>
#include <sys/sx.h>
#include <rtems/bsd/sys/unistd.h>
#include <sys/callout.h>
#include <sys/malloc.h>
#include <sys/priv.h>

#include <dev/usb/usb.h>
#include <dev/usb/usbdi.h>

#include <dev/usb/usb_core.h>
#include <dev/usb/usb_busdma.h>
#include <dev/usb/usb_process.h>
#include <dev/usb/usb_util.h>

#include <dev/usb/usb_controller.h>
#include <dev/usb/usb_bus.h>
#include <dev/usb/controller/ehci.h>

static device_probe_t ehci_mpc83xx_probe;
static device_attach_t ehci_mpc83xx_attach;
static device_detach_t ehci_mpc83xx_detach;
static device_suspend_t ehci_mpc83xx_suspend;
static device_resume_t ehci_mpc83xx_resume;

static int
ehci_mpc83xx_suspend(device_t self)
{
	ehci_softc_t *e = device_get_softc(self);
	int eno = bus_generic_suspend(self);

	if (eno != 0) {
		return (eno);
	}

	ehci_suspend(e);

	return (0);
}

static int
ehci_mpc83xx_resume(device_t self)
{
	ehci_softc_t *e = device_get_softc(self);

	ehci_resume(e);

	bus_generic_resume(self);

	return (0);
}


static int
ehci_mpc83xx_probe(device_t self)
{
	device_set_desc(self, "EHCI");

	return (0);
}

#define CONTROL_ULPI_INT_EN BSP_BBIT32(31)
#define CONTROL_WU_INT_EN BSP_BBIT32(30)
#define CONTROL_USB_EN BSP_BBIT32(29)
#define CONTROL_LSF_EN BSP_BBIT32(28)
#define CONTROL_KEEP_OTG_ON BSP_BBIT32(27)
#define CONTROL_OTG_PORT BSP_BBIT32(26)
#define CONTROL_REFSEL(val) BSP_BFLD32(val, 24, 25)
#define CONTROL_PLL_RESET BSP_BBIT32(23)
#define CONTROL_UTMI_PHY_EN BSP_BBIT32(22)
#define CONTROL_PHY_CLOCK_SEL BSP_BBIT32(21)
#define CONTROL_CLKIN_SEL(val) BSP_BFLD32(val, 19, 20)
#define CONTROL_WU_INT BSP_BBIT32(15)
#define CONTROL_PHY_CLK_VALID BSP_BBIT32(14)

static void
ehci_mpc83xx_phy_init(void)
{
#ifdef __GEN83xx_BSP_h
	volatile uint32_t *control = &mpc83xx.usb_dr.control;

#ifdef BSP_USB_EHCI_MPC83XX_HAS_ULPI
	*control = CONTROL_PHY_CLOCK_SEL;
#else
	*control = CONTROL_PLL_RESET | CONTROL_REFSEL(0x2U);
	*control = CONTROL_UTMI_PHY_EN | CONTROL_REFSEL(0x2U);
#endif

	while ((*control & CONTROL_PHY_CLK_VALID) == 0) {
		/* Wait for PLL */
	}
#endif
}

#define USBMODE_SDIS BSP_BIT32(4)
#define USBMODE_SLOM BSP_BIT32(3)
#define USBMODE_CM_IDLE BSP_FLD32(0x0, 0, 1)
#define USBMODE_CM_DEVICE BSP_FLD32(0x2, 0, 1)
#define USBMODE_CM_HOST BSP_FLD32(0x3, 0, 1)

#define PORTSC_PTS(val) BSP_FLD32(val, 30, 31)
#define PORTSC_PTS_UTMI PORTSC_PTS(0x0)
#define PORTSC_PTS_ULPI PORTSC_PTS(0x2)
#define PORTSC_PE BSP_BIT32(2)

#define SNOOP_ADDR(val) BSP_BFLD32(val, 0, 19)
#define SNOOP_ENABLE(val) BSP_BFLD32(val, 27, 31)
#define SNOOP_SIZE_2GB SNOOP_ENABLE(0x1e)

static void
ehci_mpc83xx_host_init(void)
{
#ifdef __GEN83xx_BSP_h
	mpc83xx.usb_dr.snoop1 = SNOOP_ADDR(0x0) | SNOOP_SIZE_2GB;
	mpc83xx.usb_dr.snoop2 = SNOOP_ADDR(0x80000) | SNOOP_SIZE_2GB;
#ifdef BSP_USB_EHCI_MPC83XX_HAS_ULPI
	mpc83xx.usb_dr.control = CONTROL_PHY_CLOCK_SEL | CONTROL_USB_EN;
	mpc83xx.usb_dr.portsc1 = htole32(PORTSC_PTS_ULPI);
#else
	mpc83xx.usb_dr.control = CONTROL_UTMI_PHY_EN | CONTROL_REFSEL(0x2) | CONTROL_USB_EN;
	mpc83xx.usb_dr.portsc1 = htole32(PORTSC_PTS_UTMI);
#endif
#if 0
	mpc83xx.usb_dr.pri_ctrl = 0xcU;
	mpc83xx.usb_dr.age_cnt_thresh = 0x40U;
	mpc83xx.usb_dr.si_ctrl = 0x1U;
#endif
	mpc83xx.usb_dr.usbmode = USBMODE_CM_HOST;
#endif
#ifdef LIBBSP_POWERPC_QORIQ_BSP_H
	qoriq.usb_1.snoop1 = SNOOP_ADDR(0x0) | SNOOP_SIZE_2GB;
	qoriq.usb_1.snoop2 = SNOOP_ADDR(0x80000) | SNOOP_SIZE_2GB;
	qoriq.usb_1.control = CONTROL_USB_EN;
	qoriq.usb_1.portsc1 = htole32(PORTSC_PTS_ULPI | BSP_BIT32(28) | PORTSC_PE);
	qoriq.usb_1.usbmode = USBMODE_CM_HOST;
#endif
}

static int
ehci_mpc83xx_attach(device_t self)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;
	ehci_softc_t *e = device_get_softc(self);
	usb_error_t ue = USB_ERR_NORMAL_COMPLETION;
	int eno = 0;

	memset(e, 0, sizeof(*e));

	/* Initialize some bus fields */
	e->sc_bus.parent = self;
	e->sc_bus.devices = e->sc_devices;
	e->sc_bus.devices_max = EHCI_MAX_DEVICES;
	e->sc_bus.usbrev = USB_REV_2_0;

	/* Get all DMA memory */
	if (usb_bus_mem_alloc_all(&e->sc_bus, USB_GET_DMA_TAG(self), &ehci_iterate_hw_softc)) {
		return (ENOMEM);
	}

	/* Child device */
	e->sc_bus.bdev = device_add_child(self, "usbus", -1);
	if (e->sc_bus.bdev == NULL) {
		device_printf(self, "Could not add USB device\n");
		goto error;
	}
	device_set_ivars(e->sc_bus.bdev, &e->sc_bus);
	device_set_desc(e->sc_bus.bdev, "EHCI");
	snprintf(e->sc_vendor, sizeof(e->sc_vendor), "Freescale");

	/* Register space */
	e->sc_io_tag = 0;
	e->sc_io_hdl = BSP_EHCI_REGISTER_BASE_ADDR;
	e->sc_io_size = 0x88;

	ehci_mpc83xx_phy_init();

	ehci_mpc83xx_host_init();

	/* Install interrupt handler */
	sc = rtems_interrupt_server_handler_install(
		RTEMS_ID_NONE,
		BSP_EHCI_IRQ_VECTOR,
		"USB",
		RTEMS_INTERRUPT_UNIQUE,
		(rtems_interrupt_handler) ehci_interrupt,
		e
	);
	BSD_ASSERT_SC(sc);

	e->sc_flags |= EHCI_SCFLG_NORESTERM | EHCI_SCFLG_TT;

	/* EHCI intitialization */
	ue = ehci_init(e);
	if (ue != USB_ERR_NORMAL_COMPLETION) {
		goto error;
	}

	/* Probe and attach child */
	eno = device_probe_and_attach(e->sc_bus.bdev);
	if (eno != 0) {
		goto error;
	}

	return (0);

error:
	ehci_mpc83xx_detach(self);

	return (ENXIO);
}

static int
ehci_mpc83xx_detach(device_t self)
{
	BSD_PRINTF("TODO\n");

	return (0);
}

static device_method_t ehci_methods [] = {
	/* Device interface */
	DEVMETHOD(device_probe, ehci_mpc83xx_probe),
	DEVMETHOD(device_attach, ehci_mpc83xx_attach),
	DEVMETHOD(device_detach, ehci_mpc83xx_detach),
	DEVMETHOD(device_suspend, ehci_mpc83xx_suspend),
	DEVMETHOD(device_resume, ehci_mpc83xx_resume),
	DEVMETHOD(device_shutdown, bus_generic_shutdown),

	/* Bus interface */
	DEVMETHOD(bus_print_child, bus_generic_print_child),

	{0, 0}
};

static driver_t ehci_driver = {
	.name = "ehci",
	.methods = ehci_methods,
	.size = sizeof(struct ehci_softc)
};

static devclass_t ehci_devclass;

DRIVER_MODULE(ehci, nexus, ehci_driver, ehci_devclass, 0, 0);
MODULE_DEPEND(ehci, usb, 1, 1, 1);

#endif /* defined(__GEN83xx_BSP_h) || defined(LIBBSP_POWERPC_QORIQ_BSP_H) */
