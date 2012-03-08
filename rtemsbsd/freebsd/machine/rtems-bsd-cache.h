/**
 * @file
 *
 * @ingroup rtems_bsd_machine
 *
 * @brief TODO.
 */

/*
 * Copyright (c) 2010 embedded brains GmbH.  All rights reserved.
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

#ifndef _RTEMS_BSD_MACHINE_RTEMS_BSD_CACHE_H_
#define _RTEMS_BSD_MACHINE_RTEMS_BSD_CACHE_H_

#include <bsp.h>

#if defined(LIBBSP_ARM_LPC24XX_BSP_H)
  /* No cache */
#elif defined(LIBBSP_ARM_LPC32XX_BSP_H)
  /* With cache, no coherency support in hardware */
  #include <libcpu/cache.h>
#elif defined(__GEN83xx_BSP_h)
  /* With cache, coherency support in hardware */
#endif

#endif /* _RTEMS_BSD_MACHINE_RTEMS_BSD_CACHE_H_ */
