/**
 * @file
 *
 * @ingroup demo
 *
 * @brief USB system initialization.
 */

/*
 * Copyright (c) 2009, 2010 embedded brains GmbH.  All rights reserved.
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

#include <rtems/freebsd/machine/rtems-bsd-sysinit.h>

#include <bsp.h>

#ifdef USB_SYSINIT_INIT

#if defined(LIBBSP_ARM_LPC24XX_BSP_H) || defined(LIBBSP_ARM_LPC32XX_BSP_H)
	#define NEED_USB_OHCI
#elif defined(__GEN83xx_BSP_h) || defined(LIBBSP_POWERPC_QORIQ_BSP_H)
	#define NEED_USB_EHCI
#endif

#if defined(LIBBSP_POWERPC_QORIQ_BSP_H)
	#define NEED_SDHC
#endif

SYSINIT_NEED_FREEBSD_CORE;
SYSINIT_NEED_USB_CORE;
#ifdef NEED_USB_OHCI
	SYSINIT_NEED_USB_OHCI;
#endif
#ifdef NEED_USB_EHCI
	SYSINIT_NEED_USB_EHCI;
#endif
SYSINIT_NEED_USB_MASS_STORAGE;
#ifdef NEED_SDHC
	SYSINIT_NEED_SDHC;
#endif

const char *const _bsd_nexus_devices [] = {
	#ifdef NEED_USB_OHCI
		"ohci",
	#endif
	#ifdef NEED_USB_EHCI
		"ehci",
	#endif
	#ifdef NEED_SDHC
		"sdhci",
	#endif
	NULL
};

#endif /* USB_SYSINIT_INIT */
