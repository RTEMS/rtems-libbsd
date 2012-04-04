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
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

/*
 * This violation is specifically for _Timespec_To_ticks
 */
#define __RTEMS_VIOLATE_KERNEL_VISIBILITY__ 
#include <freebsd/machine/rtems-bsd-config.h>

#include <rtems/score/timespec.h>

/*
 * Compute number of ticks in the specified amount of time.
 */
int
tvtohz(struct timeval *tv)
{
  struct timespec ts;

  ts.tv_sec = tv->tv_sec;
  ts.tv_nsec = tv->tv_usec * 1000;

  (int) _Timespec_To_ticks( &ts );
}
