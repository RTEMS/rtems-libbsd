/**
 * @file
 *
 * @ingroup rtems_bsd_machine
 *
 * @brief TODO.
 */

/*
 * Copyright (c) 2010, 2015 embedded brains GmbH.  All rights reserved.
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

#ifndef _RTEMS_BSD_MACHINE_RTEMS_BSD_CACHE_H_
#define _RTEMS_BSD_MACHINE_RTEMS_BSD_CACHE_H_

#include <bsp.h>

#if defined(LIBBSP_ARM_LPC24XX_BSP_H) || (defined(LIBBSP_ARM_LPC32XX_BSP_H) && defined(LPC32XX_DISABLE_MMU))
  /* No cache */
#elif defined(LIBBSP_ARM_ALTERA_CYCLONE_V_BSP_H) || \
  defined(LIBBSP_ARM_XILINX_ZYNQ_BSP_H) || (defined(LIBBSP_ARM_LPC32XX_BSP_H) && !defined(LPC32XX_DISABLE_MMU)) || defined(LIBBSP_ARM_IMX_BSP_H)
  /* With cache, no coherency support in hardware */
  #define CPU_DATA_CACHE_ALIGNMENT 32
#elif defined(__GEN83xx_BSP_h)
  /* With cache, coherency support in hardware */
#endif

#endif /* _RTEMS_BSD_MACHINE_RTEMS_BSD_CACHE_H_ */
