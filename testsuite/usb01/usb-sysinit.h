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

#include <freebsd/machine/rtems-bsd-sysinit.h>

#include <bsp.h>

#ifdef USB_SYSINIT_INIT

SYSINIT_NEED_FREEBSD_CORE;
SYSINIT_NEED_USB_CORE;
#if defined(LIBBSP_ARM_LPC24XX_BSP_H) || defined(LIBBSP_ARM_LPC32XX_BSP_H)
	SYSINIT_NEED_USB_OHCI;
#elif defined(__GEN83xx_BSP_h)
	SYSINIT_NEED_USB_EHCI;
#endif
SYSINIT_NEED_USB_MASS_STORAGE;

const char *const _bsd_nexus_devices [] = {
        #if defined(LIBBSP_ARM_LPC24XX_BSP_H) || defined(LIBBSP_ARM_LPC32XX_BSP_H)
                "ohci",
        #elif defined(__GEN83xx_BSP_h)
                "ehci",
        #endif
        NULL
};

#endif /* USB_SYSINIT_INIT */
