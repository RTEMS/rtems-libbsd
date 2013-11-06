/*
 * Copyright (c) 2009-2012 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <machine/rtems-bsd-kernel-space.h>

#include <bsp.h>

#ifdef LIBBSP_ARM_LPC32XX_BSP_H

#include <bsp/irq.h>
#include <bsp/lpc32xx.h>

#include <sys/cdefs.h>
#include <sys/stdint.h>
#include <sys/stddef.h>
#include <rtems/bsd/sys/param.h>
#include <sys/queue.h>
#include <rtems/bsd/sys/types.h>
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
#include <dev/usb/controller/ohci.h>

#define USB_CTRL_SLAVE_HCLK_EN (1U << 24)
#define USB_CTRL_I2C_EN (1U << 23)
#define USB_CTRL_DEV_NEED_CLK_EN (1U << 22)
#define USB_CTRL_HOST_NEED_CLK_EN (1U << 21)
#define USB_CTRL_PC_MASK (0x3U << 19)
#define USB_CTRL_PC_PULL_UP (0x0U << 19)
#define USB_CTRL_PC_BUS_KEEPER (0x1U << 19)
#define USB_CTRL_PC_NONE (0x2U << 19)
#define USB_CTRL_PC_PULL_DOWN (0x3U << 19)
#define USB_CTRL_CLKEN2 (1U << 18)
#define USB_CTRL_CLKEN1 (1U << 17)
#define USB_CTRL_POWER_UP (1U << 16)
#define USB_CTRL_BYPASS (1U << 15)
#define USB_CTRL_DIRECT (1U << 14)
#define USB_CTRL_FEEDBACK (1U << 13)
#define USB_CTRL_P_SHIFT 11
#define USB_CTRL_P_MASK (0x3U << USB_CTRL_P_SHIFT)
#define USB_CTRL_P_1 (0x0U << USB_CTRL_P_SHIFT)
#define USB_CTRL_P_2 (0x1U << USB_CTRL_P_SHIFT)
#define USB_CTRL_P_4 (0x2U << USB_CTRL_P_SHIFT)
#define USB_CTRL_P_8 (0x3U << USB_CTRL_P_SHIFT)
#define USB_CTRL_N_SHIFT 9
#define USB_CTRL_N_MASK (0x3U << USB_CTRL_N_SHIFT)
#define USB_CTRL_N_1 (0x0U << USB_CTRL_N_SHIFT)
#define USB_CTRL_N_2 (0x1U << USB_CTRL_N_SHIFT)
#define USB_CTRL_N_3 (0x2U << USB_CTRL_N_SHIFT)
#define USB_CTRL_N_4 (0x3U << USB_CTRL_N_SHIFT)
#define USB_CTRL_M_SHIFT 1
#define USB_CTRL_M_MASK (0xffU << USB_CTRL_M_SHIFT)
#define USB_CTRL_PLL_LOCK (1U << 0)

#define I2C_CTL_SRST (1U << 8)

#define I2C_TX_DATA_MASK 0xffU
#define I2C_TX_ADDR_SHIFT 1
#define I2C_TX_ADDR_MASK 0x7fU
#define I2C_TX_READ (1U << 0)
#define I2C_TX_START (1U << 8)
#define I2C_TX_STOP (1U << 9)

#define I2C_STS_TDI (1U << 0)
#define I2C_STS_AFI (1U << 1)
#define I2C_STS_NAI (1U << 2)
#define I2C_STS_RFE (1U << 9)

#define PHY_VENDOR_ID_LOW 0x0U
#define PHY_VENDOR_ID_HIGH 0x1U
#define PHY_PRODUCT_ID_LOW 0x2U
#define PHY_PRODUCT_ID_HIGH 0x3U
#define PHY_MODE_CONTROL_1 0x4U
#define PHY_MODE_CONTROL_1_SET 0x4U
#define PHY_MODE_CONTROL_1_CLEAR 0x5U
#define PHY_MODE_CONTROL_X 0x12U
#define PHY_MODE_CONTROL_X_SET 0x12U
#define PHY_MODE_CONTROL_X_CLEAR 0x13U
#define PHY_OTG_CONTROL 0x6U
#define PHY_OTG_CONTROL_SET 0x6U
#define PHY_OTG_CONTROL_CLEAR 0x7U
#define PHY_INTERRUPT_SOURCE 0x8U
#define PHY_INTERRUPT_LATCH 0xaU
#define PHY_INTERRUPT_LATCH_SET 0xaU
#define PHY_INTERRUPT_LATCH_CLEAR 0xbU
#define PHY_INTERRUPT_ENABLE_LOW 0xcU
#define PHY_INTERRUPT_ENABLE_LOW_SET 0xcU
#define PHY_INTERRUPT_ENABLE_LOW_CLEAR 0xdU
#define PHY_INTERRUPT_ENABLE_HIGH 0xeU
#define PHY_INTERRUPT_ENABLE_HIGH_SET 0xeU
#define PHY_INTERRUPT_ENABLE_HIGH_CLEAR 0xfU

// ISP130x Specific...
#define ISP130x_VERSION_ID_LOW 0x14U
#define ISP130x_VERSION_ID_HIGH 0x15U
#define ISP130x_OTG_STATUS 0x10U

// ISP1302 Specific...
#define ISP1302_MISC_CONTROL_SET 0x18U
#define ISP1302_MISC_CONTROL_CLEAR 0x19U


#define PHY_MODE_CONTROL_1_SPEED_REG (1U << 0)
#define PHY_MODE_CONTROL_1_SUSPEND_REG (1U << 1)
#define PHY_MODE_CONTROL_1_DAT_SE0 (1U << 2)
#define PHY_MODE_CONTROL_1_TRANSP_EN (1U << 3)
#define PHY_MODE_CONTROL_1_BDIS_ACON_EN (1U << 4)
#define PHY_MODE_CONTROL_1_OE_INT_EN (1U << 5)
#define PHY_MODE_CONTROL_1_UART_EN (1U << 6)

// MODE CONTROL 'X'
//
// On ISP130x  this is register MODE CONTROL 2
// On STOTG04E this is register CONTROL 3
//
//  'rsrvd' = not available reserved bit.
//  'diff'  = different functionality .
//
// Bit  Function              ISP1301      ISP1302       STOTG04E
// --------------------------------------------------------------
//  0   PWR_DN                  Y            Y             rsrvd
//  1   SPD_SUSP_CTRL           Y           rsrvd          diff
//  2   BI_DI                   Y           rsrvd           Y
//  3   TRANSP_BDIR[0]          Y            Y              Y
//  4   TRANSP_BDIR[1]          Y            Y              Y
//  5   AUDIO_EN                Y            Y              Y
//  6   PSW_OE                  Y            Y              Y
//  7   EN2V7                   Y           rsrvd           Y


//ISP1301 & ISP1302 (Reserved on STOTG04E)
#define ISP130x_MODE_CONTROL_X_GLOBAL_PWR_DN (1U << 0)
//ISP1301   (ISP1302 reserved bit!)
#define ISP1301_MODE_CONTROL_X_SPD_SUSP_CTRL (1U << 1)
//STOTG04E (Different functionality!)
#define STOTG04E_MODE_CONTROL_X_RX_BIAS_EN ISP1301_MODE_CONTROL_X_SPD_SUSP_CTRL
//ISP1301  (ISP1302 reserved bit!)
#define ISP1301_MODE_CONTROL_X_BI_DI (1U << 2)
//STOTG04E
#define STOTG04E_MODE_CONTROL_X_BI_DI ISP1301_MODE_CONTROL_X_BI_DI
//ISP1301, ISP1302 & STOTG04E common definitions.
#define PHY_MODE_CONTROL_X_TRANSP_BDIR0 (1U << 3)
#define PHY_MODE_CONTROL_X_TRANSP_BDIR1 (1U << 4)
#define PHY_MODE_CONTROL_X_AUDIO_EN (1U << 5)
#define PHY_MODE_CONTROL_X_PSW_OE (1U << 6)
//ISP1301   (ISP1302 reserved bit.)
#define ISP1301_MODE_CONTROL_X_EN2V7 (1U << 7)
//STOTG04E
#define STOTG04E_MODE_CONTROL_X_EN2V7 ISP1301_MODE_CONTROL_X_EN2V7


// Note: STOTG04E Control register 2 = ISP130x OTG Control register
#define PHY_OTG_CONTROL_DP_PULLUP (1U << 0)
#define PHY_OTG_CONTROL_DM_PULLUP (1U << 1)
#define PHY_OTG_CONTROL_DP_PULLDOWN (1U << 2)
#define PHY_OTG_CONTROL_DM_PULLDOWN (1U << 3)
#define PHY_OTG_CONTROL_ID_PULLDOWN (1U << 4)
#define PHY_OTG_CONTROL_VBUS_DRV (1U << 5)
#define PHY_OTG_CONTROL_VBUS_DISCHRG (1U << 6)
#define PHY_OTG_CONTROL_VBUS_CHRG (1U << 7)

#define PHY_ADDR (0x2cU << I2C_TX_ADDR_SHIFT)

// ISP1302 Specific...
#define ISP1302_MISC_CONTROL_REG_BYPASS_DIS (1U << 0)
#define ISP1302_MISC_CONTROL_SRP_INIT (1U << 1)
#define ISP1302_MISC_CONTROL_DP_WKPU_EN (1U << 2)
#define ISP1302_MISC_CONTROL_IDPU_DIS (1U << 3)
#define ISP1302_MISC_CONTROL_UART_2V8_EN (1U << 4)
#define ISP1302_MISC_CONTROL_FORCE_DP_LOW (1U << 6)
#define ISP1302_MISC_CONTROL_FORCE_DP_HIGH (1U << 7)

// Product Indentifers.
#define ISP1301_PRODUCT_ID   0x1301
#define ISP1302_PRODUCT_ID   0x1302
#define STOTG04E_PRODUCT_ID  0xA0C4



typedef struct
{
  uint16_t VendorID;
  uint16_t ProductID;
  uint16_t VersionID;
} phy_Details_Typ;

static void
i2c_wait_for_receive_fifo_not_empty(void)
{
	while ((LPC32XX_I2C_STS & I2C_STS_RFE) != 0) {
		/* Wait */
	}
}

static uint8_t
phy_read(uint8_t reg)
{
	LPC32XX_I2C_CTL = I2C_CTL_SRST;

	LPC32XX_I2C_TX = PHY_ADDR | I2C_TX_START;

	LPC32XX_I2C_TX = reg;

	LPC32XX_I2C_TX = PHY_ADDR | I2C_TX_READ | I2C_TX_START;

	LPC32XX_I2C_TX = I2C_TX_STOP;

	i2c_wait_for_receive_fifo_not_empty();

	return (uint8_t) LPC32XX_I2C_RX;
}

static void
i2c_wait_for_transaction_done(void)
{
	while ((LPC32XX_I2C_STS & I2C_STS_TDI) == 0) {
		/* Wait */
	}

	LPC32XX_I2C_STS = I2C_STS_TDI;
}

static void
phy_write(uint8_t reg, uint8_t val)
{

	LPC32XX_I2C_CTL = I2C_CTL_SRST;

	LPC32XX_I2C_TX = PHY_ADDR | I2C_TX_START;

	LPC32XX_I2C_TX = reg;

	LPC32XX_I2C_TX = val | I2C_TX_STOP;

	i2c_wait_for_transaction_done();
}

static phy_Details_Typ
 phy_GetDetails(void)
{
	phy_Details_Typ PhyDetails;

	PhyDetails.VendorID  = (uint16_t)((phy_read(PHY_VENDOR_ID_HIGH) << 8) |
		phy_read(PHY_VENDOR_ID_LOW));

	PhyDetails.ProductID = (uint16_t)((phy_read(PHY_PRODUCT_ID_HIGH) << 8) |
		phy_read(PHY_PRODUCT_ID_LOW));

	if (PhyDetails.ProductID == STOTG04E_PRODUCT_ID)
	{
	  // STOTG04E - Does not support 'Version' thus default it here to 'zero'
	  PhyDetails.VersionID = 0;
	}
	else
	{
	  PhyDetails.VersionID = (uint16_t)((phy_read(ISP130x_VERSION_ID_HIGH) << 8) |
		phy_read(ISP130x_VERSION_ID_LOW));
	}

	return PhyDetails;
}

static char const *
get_PhyNameString(uint16_t ProductID)
{
	static char const * const ISP1301  = "ISP1301";
	static char const * const ISP1302  = "ISP1302";
	static char const * const STOTG04E = "STOTG04E";
	static char const * const UNKNOWN  = "Unknown!";
	
	char const * String_Ptr = ISP1301;
	
	switch (ProductID) {
		case ISP1301_PRODUCT_ID:
			String_Ptr = ISP1301;
			break;
		
		case ISP1302_PRODUCT_ID:
			String_Ptr = ISP1302;
			break;
		
		case STOTG04E_PRODUCT_ID:
			String_Ptr = STOTG04E;
			break;
		
		default:
			String_Ptr = UNKNOWN;
			break;
	}
	
	return String_Ptr;
}

static void
phy_dump(void)
{
	phy_Details_Typ PhyDetails = phy_GetDetails();

	switch (PhyDetails.ProductID) {
		case ISP1301_PRODUCT_ID:
		case ISP1302_PRODUCT_ID:
			// ISP130x has extra OTG status register.
			BSD_PRINTF(
				"Registers: mc1 %02x, mc2 %02x, otgctrl %02x, otgsts %02x, isrc %02x, iltch %02x, ienl %02x, ienh %02x\n",
				phy_read(PHY_MODE_CONTROL_1),
				phy_read(PHY_MODE_CONTROL_X),
				phy_read(PHY_OTG_CONTROL),
				phy_read(ISP130x_OTG_STATUS),
				phy_read(PHY_INTERRUPT_SOURCE),
				phy_read(PHY_INTERRUPT_LATCH),
				phy_read(PHY_INTERRUPT_ENABLE_LOW),
				phy_read(PHY_INTERRUPT_ENABLE_HIGH)
			);
			break;

		case STOTG04E_PRODUCT_ID:
			// Control register 2 is 'otgctrl', control register 3 is equivalent ISP130x control register 2.
			BSD_PRINTF(
				"Registers: mc1 %02x, mc3 %02x, otgctrl %02x, isrc %02x, iltch %02x, ienl %02x, ienh %02x\n",
				phy_read(PHY_MODE_CONTROL_1),
				phy_read(PHY_MODE_CONTROL_X),
				phy_read(PHY_OTG_CONTROL),
				phy_read(PHY_INTERRUPT_SOURCE),
				phy_read(PHY_INTERRUPT_LATCH),
				phy_read(PHY_INTERRUPT_ENABLE_LOW),
				phy_read(PHY_INTERRUPT_ENABLE_HIGH)
			);
			break;

		default:
			BSD_ASSERT_SC(RTEMS_UNSATISFIED);
			break;
	}
}

static void
phy_configure(void)
{
	phy_Details_Typ PhyDetails = phy_GetDetails();

	BSD_PRINTF(
		"USB-PHY: %s  (vendor 0x%04x, product 0x%04x, version 0x%04x)\n",
		get_PhyNameString(PhyDetails.ProductID),
		PhyDetails.VendorID,
		PhyDetails.ProductID,
		PhyDetails.VersionID
	);

	phy_write(PHY_MODE_CONTROL_1_CLEAR, 0xff);
	phy_write(PHY_MODE_CONTROL_1_SET, PHY_MODE_CONTROL_1_SPEED_REG);
	phy_write(PHY_MODE_CONTROL_X_CLEAR, 0xff);

	switch (PhyDetails.ProductID) {
		case ISP1301_PRODUCT_ID:
			phy_write(
				PHY_MODE_CONTROL_X_SET,
				ISP1301_MODE_CONTROL_X_BI_DI
					| PHY_MODE_CONTROL_X_PSW_OE
					| ISP1301_MODE_CONTROL_X_SPD_SUSP_CTRL
			);
			break;

		case ISP1302_PRODUCT_ID:
			// Do not set 'SPD_SUSP_CTRL' bit as per ISP1301 this bit is reserved in
			// ISP1302, setting it will cause problems.
			// also as we have cleared Control register 2 (above) we must reset the
			// reserved BI_DI bit otherwise it will not work.
			phy_write(
				PHY_MODE_CONTROL_X_SET,
				ISP1301_MODE_CONTROL_X_BI_DI | PHY_MODE_CONTROL_X_PSW_OE
			);

			// ISP1302 has an additonal register we should initialise it..
			phy_write(ISP1302_MISC_CONTROL_CLEAR, 0xff);
			phy_write(ISP1302_MISC_CONTROL_SET, ISP1302_MISC_CONTROL_UART_2V8_EN);
			break;

		case STOTG04E_PRODUCT_ID:
			phy_write(
				PHY_MODE_CONTROL_X_SET,
				STOTG04E_MODE_CONTROL_X_BI_DI | PHY_MODE_CONTROL_X_PSW_OE
			);
			break;

		default:
			BSD_ASSERT_SC(RTEMS_UNSATISFIED);
			break;
	}

	phy_write(PHY_OTG_CONTROL_CLEAR, 0xff);
	phy_write(PHY_MODE_CONTROL_1_SET, PHY_MODE_CONTROL_1_DAT_SE0);
	phy_write(PHY_OTG_CONTROL_SET, PHY_OTG_CONTROL_DM_PULLDOWN | PHY_OTG_CONTROL_DP_PULLDOWN);
	phy_write(PHY_INTERRUPT_LATCH_CLEAR, 0xff);
	phy_write(PHY_INTERRUPT_ENABLE_LOW_CLEAR, 0xff);
	phy_write(PHY_INTERRUPT_ENABLE_HIGH_CLEAR, 0xff);
}

static void
phy_vbus_on(void)
{
	phy_write(PHY_OTG_CONTROL_SET, PHY_OTG_CONTROL_VBUS_DRV);
}

static int
ohci_lpc32xx_suspend(device_t self)
{
	ohci_softc_t *e = device_get_softc(self);
	int eno = bus_generic_suspend(self);

	if (eno != 0) {
		return (eno);
	}

	ohci_suspend(e);

	return (0);
}

static int
ohci_lpc32xx_resume(device_t self)
{
	ohci_softc_t *e = device_get_softc(self);

	ohci_resume(e);

	bus_generic_resume(self);

	return (0);
}


static int
ohci_lpc32xx_probe(device_t self)
{
	device_set_desc(self, "LPC32XX OHCI controller");

	return (0);
}

static int
ohci_lpc32xx_detach(device_t self)
{
	BSD_PRINTF("FIXME\n");

	return (0);
}

static int
ohci_lpc32xx_attach(device_t self)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;
	ohci_softc_t *e = device_get_softc(self);
	usb_error_t ue = USB_ERR_NORMAL_COMPLETION;
	int eno = 0;

	memset(e, 0, sizeof(*e));

	/* Initialize some bus fields */
	e->sc_bus.parent = self;
	e->sc_bus.devices = e->sc_devices;
	e->sc_bus.devices_max = OHCI_MAX_DEVICES;

	/* Get all DMA memory */
	if (usb_bus_mem_alloc_all(&e->sc_bus, USB_GET_DMA_TAG(self), &ohci_iterate_hw_softc)) {
		return (ENOMEM);
	}
	e->sc_dev = self;

	/* Child device */
	e->sc_bus.bdev = device_add_child(self, "usbus", -1);
	if (e->sc_bus.bdev == NULL) {
		device_printf(self, "Could not add USB device\n");
		goto error;
	}
	device_set_ivars(e->sc_bus.bdev, &e->sc_bus);
	device_set_desc(e->sc_bus.bdev, "LPC32XX OHCI bus");
	snprintf(e->sc_vendor, sizeof(e->sc_vendor), "NXP");

	/* Register space */
	e->sc_io_tag = 0U;
	e->sc_io_hdl = LPC32XX_BASE_USB;
	e->sc_io_size = 0x5cU;

	/* Enable USB PLL */
	LPC32XX_USB_DIV = 0xc;
	LPC32XX_USB_CTRL = USB_CTRL_SLAVE_HCLK_EN
		| USB_CTRL_PC_BUS_KEEPER
		| USB_CTRL_CLKEN1
		| USB_CTRL_POWER_UP
		| USB_CTRL_P_2
		| USB_CTRL_N_1
		| (191U << USB_CTRL_M_SHIFT);
	while ((LPC32XX_USB_CTRL & USB_CTRL_PLL_LOCK) == 0) {
		/* Wait */
	}
	LPC32XX_USB_CTRL |= USB_CTRL_CLKEN2;

	/* Enable USB host and AHB clocks */
	LPC32XX_OTG_CLK_CTRL = 0x1c;
	while ((LPC32XX_OTG_CLK_STAT & 0x1c) != 0x1c) {
		/* Wait */
	}

	phy_configure();

	LPC32XX_USB_CTRL |= USB_CTRL_HOST_NEED_CLK_EN;

	LPC32XX_OTG_CLK_CTRL = 0x1d;
	while ((LPC32XX_OTG_CLK_STAT & 0x1d) != 0x1d) {
		/* Wait */
	}

	/* Set OTG Status and Control Register */
	LPC32XX_OTG_STAT_CTRL = 0x1;

	phy_vbus_on();

	/* Install interrupt handler */
	sc = rtems_interrupt_server_handler_install(
		RTEMS_ID_NONE,
		LPC32XX_IRQ_USB_HOST,
		"USB",
		RTEMS_INTERRUPT_UNIQUE,
		(rtems_interrupt_handler) ohci_interrupt,
		e
	);
	BSD_ASSERT_SC(sc);

	/* OHCI intitialization */
	ue = ohci_init(e);
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
	ohci_lpc32xx_detach(self);
	return (ENXIO);
}

static device_method_t ohci_methods [] = {
	/* Device interface */
	DEVMETHOD(device_probe, ohci_lpc32xx_probe),
	DEVMETHOD(device_attach, ohci_lpc32xx_attach),
	DEVMETHOD(device_detach, ohci_lpc32xx_detach),
	DEVMETHOD(device_suspend, ohci_lpc32xx_suspend),
	DEVMETHOD(device_resume, ohci_lpc32xx_resume),
	DEVMETHOD(device_shutdown, bus_generic_shutdown),

	/* Bus interface */
	DEVMETHOD(bus_print_child, bus_generic_print_child),

	{0, 0}
};

static driver_t ohci_driver = {
	.name = "ohci",
	.methods = ohci_methods,
	.size = sizeof(struct ohci_softc)
};

static devclass_t ohci_devclass;

DRIVER_MODULE(ohci, nexus, ohci_driver, ohci_devclass, 0, 0);
MODULE_DEPEND(ohci, usb, 1, 1, 1);

#endif /* LIBBSP_ARM_LPC32XX_BSP_H */
