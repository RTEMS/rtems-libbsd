/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief TODO.
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <string.h>
#include <stdio.h>
#include <rtems.h>

u_int64_t
get_cyclecount(void)
{
  u_int64_t value;
  value = rtems_clock_get_ticks_since_boot();
  return value;
}

