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

#include <freebsd/machine/rtems-bsd-config.h>

#include <freebsd/sys/param.h>
#include <freebsd/sys/types.h>
#include <freebsd/sys/systm.h>
#include <freebsd/sys/kernel.h>

void
DELAY(int usec)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;

	/* FIXME: Integer conversion */
	rtems_interval ticks =
		((rtems_interval) usec * (rtems_interval) hz) / 1000000;

	if (ticks == 0) {
		ticks = 1;
	}

	sc = rtems_task_wake_after(ticks);
	BSD_ASSERT_SC(sc);
}
