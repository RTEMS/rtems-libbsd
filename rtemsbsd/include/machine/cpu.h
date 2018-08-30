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

#ifndef _RTEMS_BSD_MACHINE_CPU_H_
#define _RTEMS_BSD_MACHINE_CPU_H_

#ifndef _RTEMS_BSD_MACHINE_RTEMS_BSD_KERNEL_SPACE_H_
#error "the header file <machine/rtems-bsd-kernel-space.h> must be included first"
#endif

#include <rtems/counter.h>

#define cpu_spinwait() do { } while (0)

static __inline rtems_counter_ticks
get_cyclecount(void)
{
	return rtems_counter_read();
}

#endif /* _RTEMS_BSD_MACHINE_CPU_H_ */
