/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief TODO.
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

#include <rtems/freebsd/machine/rtems-bsd-config.h>

#include <rtems/freebsd/sys/param.h>
#include <rtems/freebsd/sys/types.h>
#include <rtems/freebsd/sys/systm.h>
#include <rtems/freebsd/sys/kernel.h>
#include <rtems/freebsd/sys/lock.h>
#include <rtems/freebsd/sys/mutex.h>
#include <rtems/freebsd/sys/proc.h>

#include <rtems/freebsd/bsd.h>

/* In FreeBSD this is a local function */
void mi_startup(void);

int hz;
int tick;
int maxusers;     /* base tunable */

rtems_status_code
rtems_bsd_initialize(void)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;

  hz = (int) rtems_clock_get_ticks_per_second();
  tick = 1000000 / hz;
  maxusers = 1;

	sc =  rtems_timer_initiate_server(
		BSD_TASK_PRIORITY_TIMER,
		BSD_MINIMUM_TASK_STACK_SIZE,
		RTEMS_DEFAULT_ATTRIBUTES
	);
	if (sc != RTEMS_SUCCESSFUL) {
		return RTEMS_UNSATISFIED;
	}

	mutex_init();

	mi_startup();

	return RTEMS_SUCCESSFUL;
}
