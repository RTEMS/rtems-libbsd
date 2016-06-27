/*
 * Copyright (c) 2013-2015 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * Copyright (c) 2016 Chris Johns <chrisj@rtems.org> All rights reserved.

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

/*
 * The Nexus bus devices.
 *
 * Driver Summary is:
 *
 *  Devices:
 *   RTEMS_BSD_DRIVER_XILINX_ZYNQ_SLCR0
 *
 *  Buses:
 *   RTEMS_BSD_DRIVER_PC_LEGACY
 *
 *  USB:
 *   RTEMS_BSD_DRIVER_DWCOTG0
 *   RTEMS_BSD_DRIVER_DWCOTG0_BASE_ADDR
 *    RTEMS_BSD_DRIVER_DWCOTG0_IRQ
 *   RTEMS_BSD_DRIVER_DWC_MMC
 *   RTEMS_BSD_DRIVER_MMC
 *   RTEMS_BSD_DRIVER_USB
 *   RTEMS_BSD_DRIVER_USB_MASS
 *
 *  Networking:
 *   RTEMS_BSD_DRIVER_SMC0
 *    RTEMS_BSD_DRIVER_SMC0_BASE_ADDR
 *    RTEMS_BSD_DRIVER_SMC0_IRQ
 *   RTEMS_BSD_DRIVER_FEC
 *   RTEMS_BSD_DRIVER_XILINX_ZYNQ_CGEM0
 *    RTEMS_BSD_DRIVER_CGEM0_IRQ
 *   RTEMS_BSD_DRIVER_DWC0
 *    RTEMS_BSD_DRIVER_DWC0_BASE_ADDR
 *    RTEMS_BSD_DRIVER_DWC0_IRQ
 *   RTEMS_BSD_DRIVER_TSEC
 *    RTEMS_BSD_DRIVER_TSEC_BASE_ADDR
 *    RTEMS_BSD_DRIVER_TSEC_TX_IRQ
 *    RTEMS_BSD_DRIVER_TSEC_RX_IRQ
 *    RTEMS_BSD_DRIVER_TSEC_ER_IRQ
 *   RTEMS_BSD_DRIVER_PCI_LEM
 *   RTEMS_BSD_DRIVER_PCI_IGB
 *   RTEMS_BSD_DRIVER_PCI_EM
 *   RTEMS_BSD_DRIVER_PCI_RE
 *
 *  MMI PHY:
 *   RTEMS_BSD_DRIVER_E1000PHY
 *   RTEMS_BSD_DRIVER_REPHY
 *   RTEMS_BSD_DRIVER_MIPHY
 */

#if !defined(RTEMS_BSD_NEXUS_BUS_h)
#define RTEMS_BSD_NEXUS_BUS_h

#include <rtems/bsd/bsd.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * Keep the order of the groups.
 **/

/**
 ** Devices
 **
 **/

/*
 * Xilinx Zynq System Level Control Core 0 (SLCR0).
 */
#if defined(RTEMS_BSD_DRIVER_XILINX_ZYNQ_SLCR0)
/*
 * Hard IP part of the Zynq so a fixed address.
 */
static const rtems_bsd_device_resource zy7_slcr0_res[] = {
  {
    .type = RTEMS_BSD_RES_MEMORY,
    .start_request = 0,
    .start_actual = 0xf8000000
  }
};
RTEMS_BSD_DEFINE_NEXUS_DEVICE(zy7_slcr, 0,
			      RTEMS_ARRAY_SIZE(zy7_slcr0_res),
			      &zy7_slcr0_res[0]);
#endif /* RTEMS_BSD_DRIVER_XILINX_ZYNQ_SLCR0 */

/**
 ** Physical Buses
 **/

/*
 * PC legacy bus.
 */
#if defined(RTEMS_BSD_DRIVER_PC_LEGACY)
#define RTEMS_BSD_DRIVER_HAS_PCI
RTEMS_BSD_DEFINE_NEXUS_DEVICE(legacy, 0, 0, NULL);
SYSINIT_DRIVER_REFERENCE(pcib, legacy);
SYSINIT_DRIVER_REFERENCE(pci, pcib);
#endif /* RTEMS_BSD_DRIVER_PC_LEGACY */

/**
 ** USB
 **/

/*
 * Designware/Synopsys OTG USB Controller.
 */
#if defined(RTEMS_BSD_DRIVER_DWCOTG0)
#define RTEMS_BSD_DRIVER_USB
#if !defined(RTEMS_BSD_DRIVER_DWCOTG0_BASE_ADDR)
 #error DWCOTG0 base address not defined (RTEMS_BSD_DRIVER_DWCOTG0_BASE_ADDR)
#endif
#if !defined(RTEMS_BSD_DRIVER_DWC0_IRQ)
 #error DWCOTG0 IRQ not defined (RTEMS_BSD_DRIVER_DWCOTG0_IRQ)
#endif
static const rtems_bsd_device_resource dwcotg0_res[] = {
  {
    .type = RTEMS_BSD_RES_MEMORY,
    .start_request = 0,
    .start_actual = RTEMS_BSD_DRIVER_DWCOTG0_BASE_ADDR
  }, {
    .type = RTEMS_BSD_RES_IRQ,
    .start_request = 0,
    .start_actual = RTEMS_BSD_DRIVER_DWCOTG0_IRQ
  }
};
RTEMS_BSD_DEFINE_NEXUS_DEVICE(dwcotg, 0,
			      RTEMS_ARRAY_SIZE(dwcotg0_res),
			      &dwcotg0_res[0]);
#endif /* RTEMS_BSD_DRIVER_DWCOTG0 */

/*
 * Designware/Synopsys MMC.
 */
#if defined(RTEMS_BSD_DRIVER_DWC_MMC)
#define RTEMS_BSD_DRIVER_MMC
RTEMS_BSD_DEFINE_NEXUS_DEVICE(dw_mmc, 0, 0, NULL);
SYSINIT_DRIVER_REFERENCE(mmc, dw_mmc);
#endif /* RTEMS_BSD_DRIVER_DWC_MMC */

/*
 * MMC Driver.
 */
#if defined(RTEMS_BSD_DRIVER_MMC)
SYSINIT_DRIVER_REFERENCE(mmcsd, mmc);
#endif /* RTEMS_BSD_DRIVER_MMC */

/*
 * USB Drivers.
 */
#if defined(RTEMS_BSD_DRIVER_USB)
SYSINIT_REFERENCE(usb_quirk_init);
SYSINIT_DRIVER_REFERENCE(uhub, usbus);
#endif /* RTEMS_BSD_DRIVER_USB */

/*
 * USB Mass Storage Class driver.
 */
#if defined(RTEMS_BSD_DRIVER_USB_MASS)
SYSINIT_DRIVER_REFERENCE(umass, uhub);
#endif /* RTEMS_BSD_DRIVER_USB_MASS */

/**
 ** Networking
 **/

/*
 * SMC0 driver
 */
#if defined(RTEMS_BSD_DRIVER_SMC0)
#if !defined(RTEMS_BSD_DRIVER_SMC0_BASE_ADDR)
 #error SMC base address not defined (RTEMS_BSD_DRIVER_SMC0_BASE_ADDR)
#endif
#if !defined(RTEMS_BSD_DRIVER_SMC0_IRQ)
 #error SMC IRQ not defined (RTEMS_BSD_DRIVER_SMC0_IRQ)
#endif
static const rtems_bsd_device_resource smc0_res[] = {
  {
    .type = RTEMS_BSD_RES_MEMORY,
    .start_request = 0,
    .start_actual = RTEMS_BSD_DRIVER_SMC0_BASE_ADDR
  }, {
    .type = RTEMS_BSD_RES_IRQ,
    .start_request = 0,
    .start_actual = RTEMS_BSD_DRIVER_SMC0_IRQ
  }
};
RTEMS_BSD_DEFINE_NEXUS_DEVICE(smc, 0,
			      RTEMS_ARRAY_SIZE(smc0_res),
			      &smc0_res[0]);
#endif /* RTEMS_BSD_DRIVER_SMC */

/*
 * Coldfire Fast Ethernet Controller (FEC) driver.
 */
#if defined(RTEMS_BSD_DRIVER_FEC)
RTEMS_BSD_DEFINE_NEXUS_DEVICE(fec, 0, 0, NULL);
#endif /* RTEMS_BSD_DRIVER_FEC */

/*
 * Xilinx Zynq Cadence Gigbit Ethernet MAC 0 (CGEM0).
 */
#if defined(RTEMS_BSD_DRIVER_XILINX_ZYNQ_CGEM0)
#if !defined(RTEMS_BSD_DRIVER_CGEM0_IRQ)
 #error Zynq CGEM0 IRQ not defined (RTEMS_BSD_DRIVER_CGEM0_IRQ)
#endif
static const rtems_bsd_device_resource cgem0_res[] = {
  {
    .type = RTEMS_BSD_RES_MEMORY,
    .start_request = 0,
    .start_actual = 0xe000b000
  }, {
    .type = RTEMS_BSD_RES_IRQ,
    .start_request = 0,
    .start_actual = RTEMS_BSD_DRIVER_CGEM0_IRQ
  }
};
RTEMS_BSD_DEFINE_NEXUS_DEVICE(cgem, 0,
			      RTEMS_ARRAY_SIZE(cgem0_res),
			      &cgem0_res[0]);
#endif /* RTEMS_BSD_DRIVER_XILINX_ZYNQ_CGEM0 */

/*
 * Designware/Synopsys Ethernet MAC Controller.
 */
#if defined(RTEMS_BSD_DRIVER_DWC0)
#define RTEMS_BSD_DRIVER_USB
#if !defined(RTEMS_BSD_DRIVER_DWC0_BASE_ADDR)
 #error DWC0 base address not defined (RTEMS_BSD_DRIVER_DWC0_BASE_ADDR)
#endif
#if !defined(RTEMS_BSD_DRIVER_DWC0_IRQ)
 #error DWC0 IRQ not defined (RTEMS_BSD_DRIVER_DWC0_IRQ)
#endif
static const rtems_bsd_device_resource dwc0_res[] = {
  {
    .type = RTEMS_BSD_RES_MEMORY,
    .start_request = 0,
    .start_actual = RTEMS_BSD_DRIVER_DWC0_BASE_ADDR
  }, {
    .type = RTEMS_BSD_RES_IRQ,
    .start_request = 0,
    .start_actual = RTEMS_BSD_DRIVER_DWC0_IRQ
  }
};
RTEMS_BSD_DEFINE_NEXUS_DEVICE(dwc, 0,
			      RTEMS_ARRAY_SIZE(dwc0_res),
			      &dwc0_res[0]);
#endif /* RTEMS_BSD_DRIVER_DWC0 */

/*
 * NXP QorIQ Network Driver.
 */
#if defined(RTEMS_BSD_DRIVER_TSEC)
#if !defined(RTEMS_BSD_DRIVER_TSEC_BASE_ADDR)
 #error TSEC base address not defined (RTEMS_BSD_DRIVER_TSEC_BASE_ADDR)
#endif
#if !defined(RTEMS_BSD_DRIVER_TSEC_TX_IRQ)
 #error TSEC TX IRQ not defined (RTEMS_BSD_DRIVER_TSEC_TX_IRQ)
#endif
#if !defined(RTEMS_BSD_DRIVER_TSEC_RX_IRQ)
 #error TSEC RX IRQ not defined (RTEMS_BSD_DRIVER_TSEC_RX_IRQ)
#endif
#if !defined(RTEMS_BSD_DRIVER_TSEC_ER_IRQ)
 #error TSEC ER IRQ not defined (RTEMS_BSD_DRIVER_TSEC_ER_IRQ)
#endif
static const rtems_bsd_device_resource tsec0_res[] = {
  {
    .type = RTEMS_BSD_RES_MEMORY,
    .start_request = 0,
    .start_actual = RTEMS_BSD_DRIVER_TSEC_BASE_ADDR
  }, {
    .type = RTEMS_BSD_RES_IRQ,
    .start_request = 0,
    .start_actual = RTEMS_BSD_DRIVER_TSEC_TX_IRQ
  }, {
    .type = RTEMS_BSD_RES_IRQ,
    .start_request = 1,
    .start_actual = RTEMS_BSD_DRIVER_TSEC_RX_IRQ
  }, {
    .type = RTEMS_BSD_RES_IRQ,
    .start_request = 2,
    .start_actual = RTEMS_BSD_DRIVER_TSEC_ER_IRQ
  }
};
RTEMS_BSD_DEFINE_NEXUS_DEVICE(tsec, 0,
			      RTEMS_ARRAY_SIZE(tsec0_res),
			      &tsec0_res[0]);
#endif /* RTEMS_BSD_DRIVER_TSEC */

/*
 * Intel's Legacy EM driver.
 */
#if defined(RTEMS_BSD_DRIVER_PCI_LEM)
#if !defined(RTEMS_BSD_DRIVER_HAS_PCI)
 #error PCI bus not found, check BSP bus support.
#endif
SYSINIT_DRIVER_REFERENCE(lem, pci);
#endif /* RTEMS_BSD_DRIVER_PCI_LEM */

/*
 * Intel's Gigabit Driver.
 */
#if defined(RTEMS_BSD_DRIVER_PCI_IGB)
#if !defined(RTEMS_BSD_DRIVER_HAS_PCI)
 #error PCI bus not found, check BSP bus support.
#endif
SYSINIT_DRIVER_REFERENCE(igb, pci);
#endif /* RTEMS_BSD_DRIVER_PCI_IGB */

/*
 * Intel's EM Driver.
 */
#if defined(RTEMS_BSD_DRIVER_PCI_EM)
#if !defined(RTEMS_BSD_DRIVER_HAS_PCI)
 #error PCI bus not found, check BSP bus support.
#endif
SYSINIT_DRIVER_REFERENCE(em, pci);
#endif /* RTEMS_BSD_DRIVER_PCI_EM */

/*
 * Realtek Driver
 */
#if defined(RTEMS_BSD_DRIVER_PCI_RE)
#if !defined(RTEMS_BSD_DRIVER_HAS_PCI)
 #error PCI bus not found, check BSP bus support.
#endif
SYSINIT_DRIVER_REFERENCE(re, pci);
#endif /* RTEMS_BSD_DRIVER_PCI_RE */

/**
 ** MMI Physical Layer Support.
 **/

/*
 * E1000 PHY
 */
#if defined(RTEMS_BSD_DRIVER_E1000PHY)
SYSINIT_DRIVER_REFERENCE(e1000phy, miibus);
#endif /* RTEMS_BSD_DRIVER_E1000PHY */

/*
 * Reltek PHY
 */
#if defined(RTEMS_BSD_DRIVER_REPHY)
SYSINIT_DRIVER_REFERENCE(rgephy, miibus);
#endif /* RTEMS_BSD_DRIVER_REPHY */

/*
 * MI PHY.
 */
#if defined(RTEMS_BSD_DRIVER_MIPHY)
SYSINIT_DRIVER_REFERENCE(micphy, miibus);
#endif /* RTEMS_BSD_DRIVER_MIPHY */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
