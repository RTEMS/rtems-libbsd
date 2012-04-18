/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief TODO.
 */

/*
 * COPYRIGHT (c) 2012.
 * On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef _MACHINE_CPU_H_
#define _MACHINE_CPU_H_


#include <string.h>
#include <stdio.h>
#include <rtems.h>

#ifdef _KERNEL
static __inline u_int64_t
get_cyclecount(void)
{
  return rtems_clock_get_ticks_since_boot();
}
#endif /* _KERNEL */

#endif /* !MACHINE_CPU_H */
