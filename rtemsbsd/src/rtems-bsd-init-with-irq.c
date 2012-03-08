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

#include <rtems/irq-extension.h>

#include <freebsd/bsd.h>

rtems_status_code
rtems_bsd_initialize_with_interrupt_server(void)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;

	sc = rtems_interrupt_server_initialize(
		BSD_TASK_PRIORITY_INTERRUPT,
		BSD_MINIMUM_TASK_STACK_SIZE,
		RTEMS_DEFAULT_MODES,
		RTEMS_DEFAULT_ATTRIBUTES,
		NULL
	);
	if (sc != RTEMS_SUCCESSFUL) {
		return RTEMS_UNSATISFIED;
	}

	return rtems_bsd_initialize();
}
