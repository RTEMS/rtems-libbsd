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

#if !defined(BSP_NEXUS_DEVICES_h)
#define BSP_NEXUS_DEVICES_h

#include <rtems/bsd/bsd.h>

#include <bsp.h>

#if defined(LIBBSP_ARM_REALVIEW_PBX_A9_BSP_H)

#include <bsp/irq.h>

#define RTEMS_BSD_DRIVER_SMC0
#define RTEMS_BSD_DRIVER_SMC0_BASE_ADDR 0x4e000000
#define RTEMS_BSD_DRIVER_SMC0_IRQ       RVPBXA9_IRQ_ETHERNET

#elif defined(LIBBSP_M68K_GENMCF548X_BSP_H)

#define RTEMS_BSD_DRIVER_FEC

#elif defined(LIBBSP_ARM_XILINX_ZYNQ_BSP_H)

#include <bsp/irq.h>

#define RTEMS_BSD_DRIVER_XILINX_ZYNQ_SLCR0
#define RTEMS_BSD_DRIVER_XILINX_ZYNQ_CGEM0
#define RTEMS_BSD_DRIVER_CGEM0_IRQ         ZYNQ_IRQ_ETHERNET_0
#define RTEMS_BSD_DRIVER_E1000PHY

#elif defined(LIBBSP_ARM_ALTERA_CYCLONE_V_BSP_H)

#include <bsp/socal/hps.h>
#include <bsp/irq.h>

#define RTEMS_BSD_DRIVER_DWC0
#define RTEMS_BSD_DRIVER_DWC0_BASE_ADDR    (unsigned long) ALT_EMAC1_ADDR
#define RTEMS_BSD_DRIVER_DWC0_IRQ          ALT_INT_INTERRUPT_EMAC1_IRQ
#define RTEMS_BSD_DRIVER_MIPHY
#define RTEMS_BSD_DRIVER_DWCOTG0
#define RTEMS_BSD_DRIVER_DWCOTG0_BASE_ADDR (unsigned long) ALT_USB1_ADDR
#define RTEMS_BSD_DRIVER_DWCOTG0_IRQ       ALT_INT_INTERRUPT_USB1_IRQ
#define RTEMS_BSD_DRIVER_DWC_MMC
#define RTEMS_BSD_DRIVER_MMC
#define RTEMS_BSD_DRIVER_USB
#define RTEMS_BSD_DRIVER_USB_MASS

#elif defined(LIBBSP_I386_PC386_BSP_H)

#define RTEMS_BSD_DRIVER_PC_LEGACY
#define RTEMS_BSD_DRIVER_PCI_LEM
#define RTEMS_BSD_DRIVER_PCI_IGB
#define RTEMS_BSD_DRIVER_PCI_EM
#define RTEMS_BSD_DRIVER_PCI_RE
#define RTEMS_BSD_DRIVER_REPHY

#elif defined(LIBBSP_POWERPC_QORIQ_BSP_H)

#if !QORIQ_CHIP_IS_T_VARIANT(QORIQ_CHIP_VARIANT)

#include <bsp/irq.h>

#define RTEMS_BSD_DRIVER_TSEC
#define RTEMS_BSD_DRIVER_TSEC_BASE_ADDR 0xffeb0000
#define RTEMS_BSD_DRIVER_TSEC_TX_IRQ    QORIQ_IRQ_ETSEC_TX_1
#define RTEMS_BSD_DRIVER_TSEC_RX_IRQ    QORIQ_IRQ_ETSEC_RX_1
#define RTEMS_BSD_DRIVER_TSEC_ER_IRQ    QORIQ_IRQ_ETSEC_ER_1

#endif /* !QORIQ_CHIP_IS_T_VARIANT(QORIQ_CHIP_VARIANT) */

#endif

/*
 * Include the supported Nexus bus devices.
 */
#include <machine/rtems-bsd-nexus-bus.h>

#endif
