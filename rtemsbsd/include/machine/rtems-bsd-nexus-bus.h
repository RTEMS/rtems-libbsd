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
 *   RTEMS_BSD_DRIVER_XILINX_ZYNQ_SLCR
 *   RTEMS_BSD_DRIVER_XILINX_ZYNQMP_SLCR
 *   RTEMS_BSD_DRIVER_LPC32XX_PWR
 *   RTEMS_BSD_DRIVER_LPC32XX_TSC
 *
 *  Buses:
 *   RTEMS_BSD_DRIVER_PC_LEGACY
 *
 *  USB:
 *   RTEMS_BSD_DRIVER_DW_OTG
 *   RTEMS_BSD_DRIVER_LPC32XX_OHCI
 *   RTEMS_BSD_DRIVER_MMC
 *   RTEMS_BSD_DRIVER_USB
 *   RTEMS_BSD_DRIVER_USB_MASS
 *
 *  Networking:
 *   RTEMS_BSD_DRIVER_SMC0
 *    RTEMS_BSD_DRIVER_SMC0_BASE_ADDR
 *    RTEMS_BSD_DRIVER_SMC0_IRQ
 *   RTEMS_BSD_DRIVER_LPC32XX_LPE
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
 *   RTEMS_BSD_DRIVER_ICSPHY
 *   RTEMS_BSD_DRIVER_REPHY
 *   RTEMS_BSD_DRIVER_PHY_MIC
 *   RTEMS_BSD_DRIVER_UKPHY
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
 * Xilinx Zynq System Level Control Registers (SLCR).
 */
#if !defined(RTEMS_BSD_DRIVER_XILINX_ZYNQ_SLCR)
  /*
   * Hard IP part of the Zynq so a fixed address.
   */
  #define RTEMS_BSD_DRIVER_XILINX_ZYNQ_SLCR                            \
    static const rtems_bsd_device_resource zy7_slcr_res[] = {          \
      {                                                                 \
        .type = RTEMS_BSD_RES_MEMORY,                                   \
        .start_request = 0,                                             \
        .start_actual = 0xf8000000                                      \
      }                                                                 \
    };                                                                  \
    RTEMS_BSD_DEFINE_NEXUS_DEVICE(zy7_slcr, 0,                          \
                                  RTEMS_ARRAY_SIZE(zy7_slcr_res),      \
                                  &zy7_slcr_res[0])
#endif /* RTEMS_BSD_DRIVER_XILINX_ZYNQ_SLCR */

/*
 * Xilinx ZynqMP System Level Control Registers (SLCR).
 */
#if !defined(RTEMS_BSD_DRIVER_XILINX_ZYNQMP_SLCR)
  /*
   * Hard IP part of the ZynqMP so a fixed address.
   */
  #define RTEMS_BSD_DRIVER_XILINX_ZYNQMP_SLCR                            \
    static const rtems_bsd_device_resource zynqmp_slcr_res[] = {          \
      {                                                                 \
        .type = RTEMS_BSD_RES_MEMORY,                                   \
        .start_request = 0,                                             \
        .start_actual = 0xf0000000                                      \
      }                                                                 \
    };                                                                  \
    RTEMS_BSD_DEFINE_NEXUS_DEVICE(zynqmp_slcr, 0,                          \
                                  RTEMS_ARRAY_SIZE(zynqmp_slcr_res),      \
                                  &zynqmp_slcr_res[0])
#endif /* RTEMS_BSD_DRIVER_XILINX_ZYNQMP_SLCR */

/*
 * Xilinx Versal System Level Control Registers (SLCR).
 */
#if !defined(RTEMS_BSD_DRIVER_XILINX_VERSAL_SLCR)
  /*
   * Hard IP part of the Versal so a fixed address.
   */
  #define RTEMS_BSD_DRIVER_XILINX_VERSAL_SLCR                           \
    static const rtems_bsd_device_resource versal_slcr_res[] = {        \
      {                                                                 \
        .type = RTEMS_BSD_RES_MEMORY,                                   \
        .start_request = 0,                                             \
        .start_actual = 0xf0000000                                      \
      }                                                                 \
    };                                                                  \
    RTEMS_BSD_DEFINE_NEXUS_DEVICE(versal_slcr, 0,                       \
                                  RTEMS_ARRAY_SIZE(versal_slcr_res),   \
                                  &versal_slcr_res[0])
#endif /* RTEMS_BSD_DRIVER_XILINX_VERSAL_SLCR */

/*
 * Xilinx Arasan SDIO Driver.
 */
#if !defined(RTEMS_BSD_DRIVER_ARASAN_SDHCI)
  #define RTEMS_BSD_DRIVER_ARASAN_SDHCI(_num, _base, _irq)                     \
    static const rtems_bsd_device_resource arasan_sdhci ## _num ## _res[] = {       \
      {                                                                             \
        .type = RTEMS_BSD_RES_MEMORY,                                               \
        .start_request = 0,                                                         \
        .start_actual = (_base)                                                     \
      }, {                                                                          \
        .type = RTEMS_BSD_RES_IRQ,                                                  \
        .start_request = 0,                                                         \
        .start_actual = (_irq)                                                      \
      }                                                                             \
    };                                                                              \
    RTEMS_BSD_DEFINE_NEXUS_DEVICE(arasan_sdhci, _num,                               \
                                  RTEMS_ARRAY_SIZE(arasan_sdhci ## _num ## _res),      \
                                  &arasan_sdhci ## _num ## _res[0])
#endif /* RTEMS_BSD_DRIVER_ARASAN_SDHCI */
#if !defined(RTEMS_BSD_DRIVER_XILINX_ZYNQMP_SDHCI0)
  #define RTEMS_BSD_DRIVER_XILINX_ZYNQMP_SDHCI0                \
    RTEMS_BSD_DRIVER_ARASAN_SDHCI(0, 0xFF160000, 80)
#endif /* RTEMS_BSD_DRIVER_XILINX_ZYNQMP_SDHCI0 */
#if !defined(RTEMS_BSD_DRIVER_XILINX_ZYNQMP_SDHCI1)
  #define RTEMS_BSD_DRIVER_XILINX_ZYNQMP_SDHCI1                \
    RTEMS_BSD_DRIVER_ARASAN_SDHCI(1, 0xFF170000, 81)
#endif /* RTEMS_BSD_DRIVER_XILINX_ZYNQMP_SDHCI1 */

#if !defined(RTEMS_BSD_DRIVER_XILINX_ZYNQ_SDHCI0)
  #define RTEMS_BSD_DRIVER_XILINX_ZYNQ_SDHCI0                \
    RTEMS_BSD_DRIVER_ARASAN_SDHCI(0, 0xE0100000, 56)
#endif /* RTEMS_BSD_DRIVER_XILINX_ZYNQ_SDHCI0 */
#if !defined(RTEMS_BSD_DRIVER_XILINX_ZYNQ_SDHCI1)
  #define RTEMS_BSD_DRIVER_XILINX_ZYNQ_SDHCI1                \
    RTEMS_BSD_DRIVER_ARASAN_SDHCI(1, 0xE0101000, 79)
#endif /* RTEMS_BSD_DRIVER_XILINX_ZYNQ_SDHCI1 */

#if !defined(RTEMS_BSD_DRIVER_XILINX_VERSAL_SDHCI0)
  #define RTEMS_BSD_DRIVER_XILINX_VERSAL_SDHCI0                \
    RTEMS_BSD_DRIVER_ARASAN_SDHCI(0, 0xF1040000, 158)
#endif /* RTEMS_BSD_DRIVER_XILINX_VERSAL_SDHCI0 */
#if !defined(RTEMS_BSD_DRIVER_XILINX_VERSAL_SDHCI1)
  #define RTEMS_BSD_DRIVER_XILINX_VERSAL_SDHCI1                \
    RTEMS_BSD_DRIVER_ARASAN_SDHCI(1, 0xF1050000, 160)
#endif /* RTEMS_BSD_DRIVER_XILINX_VERSAL_SDHCI1 */

/*
 * LPC32XX Power Control (PWR).
 */
#if !defined(RTEMS_BSD_DRIVER_LPC32XX_PWR)
  #define RTEMS_BSD_DRIVER_LPC32XX_PWR                                        \
    static const rtems_bsd_device_resource lpc_pwr0_res[] = {                 \
      {                                                                       \
        .type = RTEMS_BSD_RES_MEMORY,                                         \
        .start_request = 0,                                                   \
        .start_actual = LPC32XX_BASE_SYSCON                                   \
      }                                                                       \
    };                                                                        \
    RTEMS_BSD_DEFINE_NEXUS_DEVICE_ORDERED(pwr, 0, RTEMS_SYSINIT_ORDER_FIRST,  \
                                  RTEMS_ARRAY_SIZE(lpc_pwr0_res),             \
                                  &lpc_pwr0_res[0])
#endif /* RTEMS_BSD_DRIVER_LPC32XX_PWR */

/*
 * LPC32XX TSC.
 */
#if !defined(RTEMS_BSD_DRIVER_LPC32XX_TSC)
  #define RTEMS_BSD_DRIVER_LPC32XX_TSC                                      \
    static const rtems_bsd_device_resource lpc_tsc0_res[] = {               \
      {                                                                     \
        .type = RTEMS_BSD_RES_MEMORY,                                       \
        .start_request = 0,                                                 \
        .start_actual = LPC32XX_BASE_ADC                                    \
      }, {                                                                  \
        .type = RTEMS_BSD_RES_IRQ,                                          \
        .start_request = 0,                                                 \
        .start_actual = LPC32XX_IRQ_TS_IRQ_OR_ADC                           \
      }                                                                     \
    };                                                                      \
    RTEMS_BSD_DEFINE_NEXUS_DEVICE(lpctsc, 0,                                \
                                  RTEMS_ARRAY_SIZE(lpc_tsc0_res),           \
                                  &lpc_tsc0_res[0])
#endif /* RTEMS_BSD_DRIVER_LPC32XX_TSC */

/**
 ** Physical Buses
 **/

/*
 * PC legacy bus.
 */
#if !defined(RTEMS_BSD_DRIVER_PC_LEGACY)
  #define RTEMS_BSD_DRIVER_PC_LEGACY                      \
    RTEMS_BSD_DEFINE_NEXUS_DEVICE(legacy, 0, 0, NULL);    \
    SYSINIT_DRIVER_REFERENCE(pcib, legacy);               \
    SYSINIT_DRIVER_REFERENCE(pci, pcib)
#endif /* RTEMS_BSD_DRIVER_PC_LEGACY */

/**
 ** USB
 **/

/*
 * Designware/Synopsys OTG USB Controller.
 */
#if !defined(RTEMS_BSD_DRIVER_DW_OTG)
  #define RTEMS_BSD_DRIVER_DW_OTG                                           \
    SYSINIT_DRIVER_REFERENCE(dwcotg, simplebus)
#endif /* RTEMS_BSD_DRIVER_DW_OTG */

/*
 * LPC32XX OHCI.
 */
#if !defined(RTEMS_BSD_DRIVER_LPC32XX_OHCI)
  #define RTEMS_BSD_DRIVER_LPC32XX_OHCI                                     \
    static const rtems_bsd_device_resource lpc_ohci0_res[] = {              \
      {                                                                     \
        .type = RTEMS_BSD_RES_MEMORY,                                       \
        .start_request = 0,                                                 \
        .start_actual = LPC32XX_BASE_USB                                    \
      }, {                                                                  \
        .type = RTEMS_BSD_RES_MEMORY,                                       \
        .start_request = 0,                                                 \
        .start_actual = (unsigned long)(&LPC32XX_I2C_RX)                    \
      }, {                                                                  \
        .type = RTEMS_BSD_RES_IRQ,                                          \
        .start_request = 0,                                                 \
        .start_actual = LPC32XX_IRQ_USB_HOST                                \
      }                                                                     \
    };                                                                      \
    RTEMS_BSD_DEFINE_NEXUS_DEVICE(ohci, 0,                                  \
                                  RTEMS_ARRAY_SIZE(lpc_ohci0_res),          \
                                  &lpc_ohci0_res[0])
#endif /* RTEMS_BSD_DRIVER_LPC32XX_OHCI */

/*
 * Designware/Synopsys MMC.
 */
#if !defined(RTEMS_BSD_DRIVER_DW_MMC)
  #define RTEMS_BSD_DRIVER_DW_MMC                                \
    SYSINIT_DRIVER_REFERENCE(dw_mmc, simplebus)
#endif /* RTEMS_BSD_DRIVER_DW_MMC */

/*
 * Atmel Media Card Interface (MCI).
 */
#if !defined(RTEMS_BSD_DRIVER_AT91_MCI0)
  #define RTEMS_BSD_DRIVER_AT91_MCI0(_base, _irq)                  \
    static const rtems_bsd_device_resource at91_mci0_res[] = {     \
      {                                                            \
        .type = RTEMS_BSD_RES_MEMORY,                              \
        .start_request = 0,                                        \
        .start_actual = (_base)                                    \
      }, {                                                         \
        .type = RTEMS_BSD_RES_IRQ,                                 \
        .start_request = 0,                                        \
        .start_actual = (_irq)                                     \
      }                                                            \
    };                                                             \
    RTEMS_BSD_DEFINE_NEXUS_DEVICE(at91_mci, 0,                     \
                                  RTEMS_ARRAY_SIZE(at91_mci0_res), \
                                  &at91_mci0_res[0])
#endif /* RTEMS_BSD_DRIVER_AT91_MCI0 */

/*
 * MMC Driver.
 */
#if !defined(RTEMS_BSD_DRIVER_MMC)
  #define RTEMS_BSD_DRIVER_MMC                    \
    SYSINIT_DRIVER_REFERENCE(mmcsd, mmc)
#endif /* RTEMS_BSD_DRIVER_MMC */

#if !defined(RTEMS_BSD_DRIVER_ST_SDMMC)
  #define RTEMS_BSD_DRIVER_ST_SDMMC(_num, _base, _dlyb, _irq)                 \
    static const rtems_bsd_device_resource st_sdmmc ## _num ## _res[] = {     \
      {                                                                       \
        .type = RTEMS_BSD_RES_MEMORY,                                         \
        .start_request = 0,                                                   \
        .start_actual = (_base)                                               \
      }, {                                                                    \
        .type = RTEMS_BSD_RES_MEMORY,                                         \
        .start_request = 1,                                                   \
        .start_actual = (_dlyb)                                               \
      }, {                                                                    \
        .type = RTEMS_BSD_RES_IRQ,                                            \
        .start_request = 0,                                                   \
        .start_actual = (_irq)                                                \
      }                                                                       \
    };                                                                        \
    RTEMS_BSD_DEFINE_NEXUS_DEVICE(st_sdmmc, 0,                                \
                                  RTEMS_ARRAY_SIZE(st_sdmmc ## _num ## _res), \
                                  &st_sdmmc ## _num ## _res[0])
#endif /* RTEMS_BSD_DRIVER_ST_SDMMC */

/*
 * USB Drivers.
 */
#if !defined(RTEMS_BSD_DRIVER_USB)
  #define RTEMS_BSD_DRIVER_USB                    \
    SYSINIT_REFERENCE(usb_quirk_init);            \
    SYSINIT_DRIVER_REFERENCE(uhub, usbus)
#endif /* RTEMS_BSD_DRIVER_USB */

/*
 * USB Mass Storage Class driver.
 */
#if !defined(RTEMS_BSD_DRIVER_USB_MASS)
  #define RTEMS_BSD_DRIVER_USB_MASS               \
    SYSINIT_DRIVER_REFERENCE(umass, uhub)
#endif /* RTEMS_BSD_DRIVER_USB_MASS */

/*
 * USB SAF1761 host controller driver.
 */
#if !defined(RTEMS_BSD_DRIVER_USB_SAF1761_OTG)
  #define RTEMS_BSD_DRIVER_USB_SAF1761_OTG(_base, _irq)                  \
    static const rtems_bsd_device_resource usb_saf1761_otg_res[] = {     \
      {                                                                  \
        .type = RTEMS_BSD_RES_MEMORY,                                    \
        .start_request = 0,                                              \
        .start_actual = (_base)                                          \
      }, {                                                               \
        .type = RTEMS_BSD_RES_IRQ,                                       \
        .start_request = 0,                                              \
        .start_actual = (_irq)                                           \
      }                                                                  \
    };                                                                   \
    RTEMS_BSD_DEFINE_NEXUS_DEVICE(saf1761otg, 0,                         \
                                  RTEMS_ARRAY_SIZE(usb_saf1761_otg_res), \
                                  &usb_saf1761_otg_res[0])
#endif /* RTEMS_BSD_DRIVER_USB_SAF1761_OTG */

/**
 ** Networking
 **/

/*
 * SMC0 driver
 */
#if !defined(RTEMS_BSD_DRIVER_SMC0)
  #define RTEMS_BSD_DRIVER_SMC0(_base, _irq)                     \
    static const rtems_bsd_device_resource smc0_res[] = {        \
      {                                                          \
        .type = RTEMS_BSD_RES_MEMORY,                            \
        .start_request = 0,                                      \
        .start_actual = (_base)                                  \
      }, {                                                       \
        .type = RTEMS_BSD_RES_IRQ,                               \
        .start_request = 0,                                      \
        .start_actual = (_irq)                                   \
      }                                                          \
    };                                                           \
    RTEMS_BSD_DEFINE_NEXUS_DEVICE(smc, 0,                        \
                                  RTEMS_ARRAY_SIZE(smc0_res),    \
                                  &smc0_res[0])
#endif /* RTEMS_BSD_DRIVER_SMC */

/*
 * LPC32XX LPE driver
 */
#if !defined(RTEMS_BSD_DRIVER_LPC32XX_LPE)
  #define RTEMS_BSD_DRIVER_LPC32XX_LPE                           \
    static const rtems_bsd_device_resource lpc_lpe0_res[] = {    \
      {                                                          \
        .type = RTEMS_BSD_RES_MEMORY,                            \
        .start_request = 0,                                      \
        .start_actual = LPC32XX_BASE_ETHERNET                    \
      }, {                                                       \
        .type = RTEMS_BSD_RES_IRQ,                               \
        .start_request = 0,                                      \
        .start_actual = LPC32XX_IRQ_ETHERNET                     \
      }                                                          \
    };                                                           \
    RTEMS_BSD_DEFINE_NEXUS_DEVICE(lpe, 0,                        \
                                  RTEMS_ARRAY_SIZE(lpc_lpe0_res),    \
                                  &lpc_lpe0_res[0])
#endif /* RTEMS_BSD_DRIVER_LPC32XX_LPE */

/*
 * Coldfire Fast Ethernet Controller (FEC) driver.
 */
#if !defined(RTEMS_BSD_DRIVER_FEC)
  #define RTEMS_BSD_DRIVER_FEC                            \
    RTEMS_BSD_DEFINE_NEXUS_DEVICE(fec, 0, 0, NULL);
#endif /* RTEMS_BSD_DRIVER_FEC */

/*
 * Atmel SAMv71 Ethernet Controller (sam) driver.
 */
#if !defined(RTEMS_BSD_DRIVER_IF_ATSAM)
  #define RTEMS_BSD_DRIVER_IF_ATSAM                       \
    RTEMS_BSD_DEFINE_NEXUS_DEVICE(if_atsam, 0, 0, NULL);
#endif /* RTEMS_BSD_DRIVER_IF_ATSAM */

/*
 * Xilinx Zynq Cadence Gigbit Ethernet MAC (CGEM).
 */
#if !defined(RTEMS_BSD_DRIVER_XILINX_ZYNQ_CGEM)
  #define RTEMS_BSD_DRIVER_XILINX_ZYNQ_CGEM(_num, _base, _irq)              \
    static const rtems_bsd_device_resource cgem ## _num ## _res[] = {       \
      {                                                                     \
        .type = RTEMS_BSD_RES_MEMORY,                                       \
        .start_request = 0,                                                 \
        .start_actual = (_base)                                             \
      }, {                                                                  \
        .type = RTEMS_BSD_RES_IRQ,                                          \
        .start_request = 0,                                                 \
        .start_actual = (_irq)                                              \
      }                                                                     \
    };                                                                      \
    RTEMS_BSD_DEFINE_NEXUS_DEVICE(cgem, _num,                               \
                                  RTEMS_ARRAY_SIZE(cgem ## _num ## _res),   \
                                  &cgem ## _num ## _res[0])
#endif /* RTEMS_BSD_DRIVER_XILINX_ZYNQ_CGEM */
#if !defined(RTEMS_BSD_DRIVER_XILINX_ZYNQ_CGEM0)
  #define RTEMS_BSD_DRIVER_XILINX_ZYNQ_CGEM0(_irq)                \
    RTEMS_BSD_DRIVER_XILINX_ZYNQ_CGEM(0, 0xe000b000, _irq)
#endif /* RTEMS_BSD_DRIVER_XILINX_ZYNQ_CGEM0 */
#if !defined(RTEMS_BSD_DRIVER_XILINX_ZYNQ_CGEM1)
  #define RTEMS_BSD_DRIVER_XILINX_ZYNQ_CGEM1(_irq)       \
    RTEMS_BSD_DRIVER_XILINX_ZYNQ_CGEM(1, 0xe000c000, _irq)
#endif /* RTEMS_BSD_DRIVER_XILINX_ZYNQ_CGEM1 */
#if !defined(RTEMS_BSD_DRIVER_XILINX_ZYNQMP_CGEM0)
  #define RTEMS_BSD_DRIVER_XILINX_ZYNQMP_CGEM0(_irq)                \
    RTEMS_BSD_DRIVER_XILINX_ZYNQ_CGEM(0, 0xff0b0000, _irq)
#endif /* RTEMS_BSD_DRIVER_XILINX_ZYNQMP_CGEM0 */
#if !defined(RTEMS_BSD_DRIVER_XILINX_ZYNQMP_CGEM1)
  #define RTEMS_BSD_DRIVER_XILINX_ZYNQMP_CGEM1(_irq)       \
    RTEMS_BSD_DRIVER_XILINX_ZYNQ_CGEM(1, 0xff0c0000, _irq)
#endif /* RTEMS_BSD_DRIVER_XILINX_ZYNQMP_CGEM1 */
#if !defined(RTEMS_BSD_DRIVER_XILINX_ZYNQMP_CGEM2)
  #define RTEMS_BSD_DRIVER_XILINX_ZYNQMP_CGEM2(_irq)                \
    RTEMS_BSD_DRIVER_XILINX_ZYNQ_CGEM(2, 0xff0d0000, _irq)
#endif /* RTEMS_BSD_DRIVER_XILINX_ZYNQMP_CGEM2 */
#if !defined(RTEMS_BSD_DRIVER_XILINX_ZYNQMP_CGEM3)
  #define RTEMS_BSD_DRIVER_XILINX_ZYNQMP_CGEM3(_irq)       \
    RTEMS_BSD_DRIVER_XILINX_ZYNQ_CGEM(3, 0xff0e0000, _irq)
#endif /* RTEMS_BSD_DRIVER_XILINX_ZYNQMP_CGEM3 */
#if !defined(RTEMS_BSD_DRIVER_XILINX_VERSAL_CGEM0)
  #define RTEMS_BSD_DRIVER_XILINX_VERSAL_CGEM0(_irq)       \
    RTEMS_BSD_DRIVER_XILINX_ZYNQ_CGEM(0, 0xff0c0000, _irq)
#endif /* RTEMS_BSD_DRIVER_XILINX_VERSAL_CGEM0 */
#if !defined(RTEMS_BSD_DRIVER_XILINX_VERSAL_CGEM1)
  #define RTEMS_BSD_DRIVER_XILINX_VERSAL_CGEM1(_irq)                \
    RTEMS_BSD_DRIVER_XILINX_ZYNQ_CGEM(1, 0xff0d0000, _irq)
#endif /* RTEMS_BSD_DRIVER_XILINX_VERSAL_CGEM1 */

/*
 * Designware/Synopsys Ethernet MAC Controller.
 */
#if !defined(RTEMS_BSD_DRIVER_DW_ETH)
  #define RTEMS_BSD_DRIVER_DW_ETH                                   \
    SYSINIT_DRIVER_REFERENCE(dwc, simplebus);
#endif /* RTEMS_BSD_DRIVER_DW_ETH */

/*
 * NXP QorIQ Network Driver.
 */
#if !defined(RTEMS_BSD_DRIVER_TSEC)
  #define RTEMS_BSD_DRIVER_TSEC(_base, _tx_irq, _rx_irq, _er_irq)   \
    static const rtems_bsd_device_resource tsec0_res[] = {          \
      {                                                             \
        .type = RTEMS_BSD_RES_MEMORY,                               \
        .start_request = 0,                                         \
        .start_actual = (_base)                                     \
      }, {                                                          \
        .type = RTEMS_BSD_RES_IRQ,                                  \
        .start_request = 0,                                         \
        .start_actual = (_tx_irq)                                   \
      }, {                                                          \
        .type = RTEMS_BSD_RES_IRQ,                                  \
        .start_request = 1,                                         \
        .start_actual = (_rx_irq)                                   \
      }, {                                                          \
        .type = RTEMS_BSD_RES_IRQ,                                  \
        .start_request = 2,                                         \
        .start_actual = (_er_irq)                                   \
      }                                                             \
    };                                                              \
    RTEMS_BSD_DEFINE_NEXUS_DEVICE(tsec, 0,                          \
                                  RTEMS_ARRAY_SIZE(tsec0_res),      \
                                  &tsec0_res[0])
#endif /* RTEMS_BSD_DRIVER_TSEC */

/*
 * Intel's Legacy EM driver.
 */
#if !defined(RTEMS_BSD_DRIVER_PCI_LEM)
  #define RTEMS_BSD_DRIVER_PCI_LEM                \
    SYSINIT_DRIVER_REFERENCE(lem, pci);
#endif /* RTEMS_BSD_DRIVER_PCI_LEM */

/*
 * Intel's Gigabit Driver.
 */
#if !defined(RTEMS_BSD_DRIVER_PCI_IGB)
  #define RTEMS_BSD_DRIVER_PCI_IGB                \
    SYSINIT_DRIVER_REFERENCE(igb, pci);
#endif /* RTEMS_BSD_DRIVER_PCI_IGB */

/*
 * Intel's EM Driver.
 */
#if !defined(RTEMS_BSD_DRIVER_PCI_EM)
  #define RTEMS_BSD_DRIVER_PCI_EM                 \
    SYSINIT_DRIVER_REFERENCE(em, pci);
#endif /* RTEMS_BSD_DRIVER_PCI_EM */

/*
 * Realtek Driver
 */
#if !defined(RTEMS_BSD_DRIVER_PCI_RE)
  #define RTEMS_BSD_DRIVER_PCI_RE                 \
    SYSINIT_DRIVER_REFERENCE(re, pci);
#endif /* RTEMS_BSD_DRIVER_PCI_RE */

/*
 * DEC Tulip Driver
 */
#if !defined(RTEMS_BSD_DRIVER_PCI_DC)
  #define RTEMS_BSD_DRIVER_PCI_DC                 \
    SYSINIT_DRIVER_REFERENCE(dc, pci);
#endif /* RTEMS_BSD_DRIVER_PCI_DC */

/**
 ** MMI Physical Layer Support.
 **/

/*
 * UK PHY (for unknown PHY devices)
 */
#if !defined(RTEMS_BSD_DRIVER_UKPHY)
  #define RTEMS_BSD_DRIVER_UKPHY               \
    SYSINIT_DRIVER_REFERENCE(ukphy, miibus);
#endif /* RTEMS_BSD_DRIVER_UKPHY */

/*
 * E1000 PHY
 */
#if !defined(RTEMS_BSD_DRIVER_E1000PHY)
  #define RTEMS_BSD_DRIVER_E1000PHY               \
    SYSINIT_DRIVER_REFERENCE(e1000phy, miibus);
#endif /* RTEMS_BSD_DRIVER_E1000PHY */

/*
 * ICS PHY
 */
#if !defined(RTEMS_BSD_DRIVER_ICSPHY)
  #define RTEMS_BSD_DRIVER_ICSPHY               \
    SYSINIT_DRIVER_REFERENCE(icsphy, miibus);
#endif /* RTEMS_BSD_DRIVER_ICSPHY */

/*
 * Reltek PHY
 */
#if !defined(RTEMS_BSD_DRIVER_REPHY)
  #define RTEMS_BSD_DRIVER_REPHY                  \
    SYSINIT_DRIVER_REFERENCE(rgephy, miibus);
#endif /* RTEMS_BSD_DRIVER_REPHY */

/*
 * MI PHY.
 */
#if !defined(RTEMS_BSD_DRIVER_PHY_MIC)
  #define RTEMS_BSD_DRIVER_PHY_MIC                  \
    SYSINIT_DRIVER_REFERENCE(micphy, miibus);
#endif /* RTEMS_BSD_DRIVER_PHY_MIC */

/*
 * UK PHY.
 */
#if !defined(RTEMS_BSD_DRIVER_UKPHY)
  #define RTEMS_BSD_DRIVER_UKPHY                  \
    SYSINIT_DRIVER_REFERENCE(ukphy, miibus);
#endif /* RTEMS_BSD_DRIVER_UKPHY */

/*
 * DC PHY.
 */
#if !defined(RTEMS_BSD_DRIVER_DCPHY)
  #define RTEMS_BSD_DRIVER_DCPHY                  \
    SYSINIT_DRIVER_REFERENCE(dcphy, miibus);
#endif /* RTEMS_BSD_DRIVER_DCPHY */

/*
 * PN PHY.
 */
#if !defined(RTEMS_BSD_DRIVER_PNPHY)
  #define RTEMS_BSD_DRIVER_PNPHY                  \
    SYSINIT_DRIVER_REFERENCE(pnphy, miibus);
#endif /* RTEMS_BSD_DRIVER_PNPHY */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
