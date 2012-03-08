#include <rtems/freebsd/machine/rtems-bsd-config.h>

#include <rtems/freebsd/machine/rtems-bsd-config.h>

#include <bsp.h>

#ifdef LIBBSP_ARM_LPC3250_BSP_H

#include <bsp/irq.h>

#include <rtems/freebsd/sys/cdefs.h>
#include <rtems/freebsd/sys/stdint.h>
#include <rtems/freebsd/sys/stddef.h>
#include <rtems/freebsd/sys/param.h>
#include <rtems/freebsd/sys/queue.h>
#include <rtems/freebsd/sys/types.h>
#include <rtems/freebsd/sys/systm.h>
#include <rtems/freebsd/sys/kernel.h>
#include <rtems/freebsd/sys/bus.h>
#include <rtems/freebsd/sys/linker_set.h>
#include <rtems/freebsd/sys/module.h>
#include <rtems/freebsd/sys/lock.h>
#include <rtems/freebsd/sys/mutex.h>
#include <rtems/freebsd/sys/condvar.h>
#include <rtems/freebsd/sys/sysctl.h>
#include <rtems/freebsd/sys/sx.h>
#include <rtems/freebsd/sys/unistd.h>
#include <rtems/freebsd/sys/callout.h>
#include <rtems/freebsd/sys/malloc.h>
#include <rtems/freebsd/sys/priv.h>

#include <rtems/freebsd/dev/usb/usb.h>
#include <rtems/freebsd/dev/usb/usbdi.h>

#include <rtems/freebsd/dev/usb/usb_core.h>
#include <rtems/freebsd/dev/usb/usb_busdma.h>
#include <rtems/freebsd/dev/usb/usb_process.h>
#include <rtems/freebsd/dev/usb/usb_util.h>

#include <rtems/freebsd/dev/usb/usb_controller.h>
#include <rtems/freebsd/dev/usb/usb_bus.h>
#include <rtems/freebsd/dev/usb/controller/ohci.h>

typedef struct
{
  uint16_t VendorID;
  uint16_t ProductID;
  uint16_t VersionID;
} isp130x_PhyDetails_Typ;

static void
i2c_wait_for_receive_fifo_not_empty(void)
{
  while ((OTGI2CSTS & OTG_I2C_RFE) != 0) {
		/* Wait */
	}
}

static uint8_t
isp1301_read(uint8_t reg)
{
  OTGI2CCTL = OTG_I2C_RESET;

  OTGI2CTX = ISP1301_I2C_ADDR | OTG_I2C_START;

  OTGI2CTX = reg;

  OTGI2CTX = ISP1301_I2C_ADDR | OTG_I2C_READ | OTG_I2C_START;

  OTGI2CTX = OTG_I2C_STOP;

	i2c_wait_for_receive_fifo_not_empty();

  return (uint8_t) OTGI2CRX;
}

static void
i2c_wait_for_transaction_done(void)
{
  while ((OTGI2CSTS & OTG_I2C_TDI) == 0) {
		/* Wait */
	}

  OTGI2CSTS = OTG_I2C_TDI;
}

static void
isp1301_write(uint8_t reg, uint8_t val)
{

  OTGI2CCTL = OTG_I2C_RESET;

  OTGI2CTX = ISP1301_I2C_ADDR | OTG_I2C_START;

  OTGI2CTX = reg;

  OTGI2CTX = val | OTG_I2C_STOP;

	i2c_wait_for_transaction_done();
}

static void
isp1301_dump(void)
{
	BSD_PRINTF(
		"ISP1301: mc1 %02x, mc2 %02x, otgctrl %02x, otgsts %02x, isrc %02x, iltch %02x, ienl %02x, ienh %02x\n",
		isp1301_read(ISP1301_MODE_CONTROL_1),
		isp1301_read(ISP1301_MODE_CONTROL_2),
    isp1301_read(ISP1301_OTG_CONTROL_1),
		isp1301_read(ISP1301_OTG_STATUS),
    isp1301_read(ISP1301_I2C_INTERRUPT_SOURCE),
    isp1301_read(ISP1301_I2C_INTERRUPT_LATCH),
    isp1301_read(ISP1301_I2C_INTERRUPT_FALLING),
    isp1301_read(ISP1301_I2C_INTERRUPT_RISING)
	);
}


static isp130x_PhyDetails_Typ
 isp130x_GetPhyDetails(void)
{
  isp130x_PhyDetails_Typ PhyDetails;

  PhyDetails.VendorID  = (uint16_t)((isp1301_read(ISP1301_I2C_VENDOR_ID_HIGH) << 8) |
                                     isp1301_read(ISP1301_I2C_VENDOR_ID_LOW));

  PhyDetails.ProductID = (uint16_t)((isp1301_read(ISP1301_I2C_PRODUCT_ID_HIGH) << 8) |
                                     isp1301_read(ISP1301_I2C_PRODUCT_ID_LOW));

  PhyDetails.VersionID = (uint16_t)((isp1301_read(ISP1301_I2C_VERSION_ID_HIGH) << 8) |
                                     isp1301_read(ISP1301_I2C_VERSION_ID_LOW));

  return PhyDetails;
}


static void
isp1301_configure(void)
{
    isp130x_PhyDetails_Typ PhyDetails = isp130x_GetPhyDetails();

    BSD_PRINTF("ISP130x: vendor 0x%04x, product 0x%04x, version 0x%04x\n",
               PhyDetails.VendorID,
               PhyDetails.ProductID,
               PhyDetails.VersionID);

	isp1301_write(ISP1301_MODE_CONTROL_1_CLEAR, 0xff);
  isp1301_write(ISP1301_MODE_CONTROL_1_SET, MC1_SPEED_REG);
	isp1301_write(ISP1301_MODE_CONTROL_2_CLEAR, 0xff);

    switch (PhyDetails.ProductID)
    {
      case ISP1301_PRODUCT_ID:
        isp1301_write(ISP1301_MODE_CONTROL_2_SET, MC2_BI_DI |
                                                  MC2_PSW_EN |
                                                  MC2_SPD_SUSP_CTRL);
        break;

      case ISP1302_PRODUCT_ID:
        // Do not set 'SPD_SUSP_CTRL' bit as per ISP1301 this bit is reserved in
        // ISP1302, setting it will cause problems.
        isp1301_write(ISP1301_MODE_CONTROL_2_SET, MC2_BI_DI |
                                                  MC2_PSW_EN);

        // ISP1302 has an additonal register we should initialise it..
        isp1301_write(ISP1302_MISC_CONTROL_CLEAR, 0xff);
        isp1301_write(ISP1302_MISC_CONTROL_SET, MISC_UART_2V8_EN);

        break;

      default:
        break;
    }

    isp1301_write(ISP1301_OTG_CONTROL_CLEAR, 0xff);
  isp1301_write(ISP1301_MODE_CONTROL_1_SET, MC1_DAT_SE0);
  isp1301_write(ISP1301_OTG_CONTROL_SET, OTG1_DM_PULLDOWN | OTG1_DP_PULLDOWN);
  isp1301_write(ISP1301_I2C_INTERRUPT_LATCH_CLEAR, 0xff);
  isp1301_write(ISP1301_I2C_INTERRUPT_FALLING_CLEAR, 0xff);
  isp1301_write(ISP1301_I2C_INTERRUPT_RISING_CLEAR, 0xff);
}

static void
isp1301_vbus_on(void)
{
  isp1301_write(ISP1301_OTG_CONTROL_SET, OTG1_VBUS_DRV);
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
  device_set_desc(self, "LPC3250 OHCI controller");

	return (0);
}

static int
ohci_lpc32xx_detach(device_t self)
{
	ohci_softc_t *e = device_get_softc(self);

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
  device_set_desc(e->sc_bus.bdev, "LPC3250 OHCI bus");
	snprintf(e->sc_vendor, sizeof(e->sc_vendor), "NXP");

	/* Register space */
	e->sc_io_tag = 0U;
  e->sc_io_hdl = OTGUSB_BASE;
	e->sc_io_size = 0x5cU;

	/* Enable USB PLL */
  USBDIVCTRL = 0xc;
  USBCTRL = USBCLK_SLAVE_HCLK_EN
    | USBCLK_PC_BUS_KEEPER
    | USBCLK_CLKEN1
    | USBCLK_POWER_UP
    | USBCLK_P_2
    | USBCLK_N_1
    | (191U << USBCLK_M_SHIFT);
  while ((USBCTRL & USBCLK_PLL_LOCK) == 0) {
		/* Wait */
	}
  USBCTRL |= USBCLK_CLKEN2;

	/* Enable USB host and AHB clocks */
  OTGCLKCTRL = 0x1c;
  while ((OTGCLKSTAT & 0x1c) != 0x1c) {
		/* Wait */
	}

	isp1301_configure();

  USBCTRL |= USBCLK_HOST_NEED_CLK_EN;

  OTGCLKCTRL = 0x1d;
  while ((OTGCLKSTAT & 0x1d) != 0x1d) {
		/* Wait */
	}

	/* Set OTG Status and Control Register */
  OTGSTAT = 0x1;

	isp1301_vbus_on();

	/* Install interrupt handler */
	sc = rtems_interrupt_server_handler_install(
		RTEMS_ID_NONE,
    IRQ_USB_HOST,
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

#endif /* LIBBSP_ARM_LPC3250_BSP_H */
