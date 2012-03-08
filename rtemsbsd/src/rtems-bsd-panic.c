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
#include <freebsd/sys/lock.h>
#include <freebsd/sys/mutex.h>
#include <freebsd/sys/proc.h>

static void
suspend_all_threads(void)
{
	rtems_chain_control *chain = &rtems_bsd_thread_chain;
	rtems_chain_node *node = rtems_chain_first(chain);
	rtems_id self = rtems_task_self();

	while (!rtems_chain_is_tail(chain, node)) {
		struct thread *td = (struct thread *) node;

		if (td->td_id != self && td->td_id != RTEMS_SELF) {
			rtems_task_suspend(td->td_id);
		}

		node = rtems_chain_next(node);
	}

	rtems_task_suspend(RTEMS_SELF);
}

void
panic(const char *fmt, ...)
{
	va_list ap;

	printf("*** BSD PANIC *** ");

	va_start(ap, fmt);
	vprintf(fmt, ap);
	va_end(ap);

	printf("\n");

	suspend_all_threads();

	/* FIXME */
	rtems_fatal_error_occurred(0xdeadbeef);
}
