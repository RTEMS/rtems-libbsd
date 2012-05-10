/*
 * Copyright (c) 2012 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
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

#include <freebsd/machine/rtems-bsd-config.h>

#include <freebsd/sys/param.h>
#include <freebsd/sys/systm.h>

#include <assert.h>
#include <malloc.h>

#include "timeout_helper.h"

void timeout_table_init()
{
	size_t size = 0;
	caddr_t v = 0;
	void* firstaddr = 0;

	/* calculates how much memory is needed */
	v = kern_timeout_callwheel_alloc(v);

	/* allocate memory */
	size = (size_t)v;
	firstaddr = malloc(round_page(size));
	assert(firstaddr != NULL);

	/* now set correct addresses for callwheel */
	v = (caddr_t) firstaddr;
	v = kern_timeout_callwheel_alloc(v);

	assert((size_t)((void *)v - firstaddr) == size);

	/* Initialize the callouts we just allocated. */
	kern_timeout_callwheel_init();
}

#define CALLOUT_TICK_TASK_PRIO		(PRIORITY_DEFAULT_MAXIMUM - 1)
#define CALLOUT_TICK_TASK_STACK_SIZE	(1024)
#define CALLOUT_TICK_TASK_NAME		rtems_build_name('C', 'O', 'U', 'T')
#define CALLOUT_TICKS_PER_CALLOUT_TICK	1

static const rtems_event_set TRIGGER_EVENT = RTEMS_EVENT_13;

static void callout_tick_task_trigger(rtems_id timer, void *arg)
{
	rtems_status_code status;
	rtems_id *task_id = arg;

	status = rtems_event_send(*task_id, TRIGGER_EVENT);
	assert(status == RTEMS_SUCCESSFUL);

	status = rtems_timer_reset(timer);
	assert(status == RTEMS_SUCCESSFUL);
}

rtems_task callout_tick_task(rtems_task_argument arg)
{
	rtems_name name;
	rtems_id timer;
	rtems_status_code status;
	const rtems_interval ticks_per_ms = CALLOUT_TICKS_PER_CALLOUT_TICK;
	rtems_id self_id = rtems_task_self();

	name = CALLOUT_TICK_TASK_NAME;

	status = rtems_timer_create( name, &timer );
	assert(status == RTEMS_SUCCESSFUL);

	status = rtems_timer_fire_after( timer, ticks_per_ms, callout_tick_task_trigger, &self_id );
	assert(status == RTEMS_SUCCESSFUL);

	while ( 1 ) {
		rtems_event_set event;

		status = rtems_event_receive(
				TRIGGER_EVENT,
				RTEMS_EVENT_ALL | RTEMS_WAIT,
				RTEMS_NO_TIMEOUT,
				&event
		);
		assert(status == RTEMS_SUCCESSFUL);
		assert(event == TRIGGER_EVENT);
		
		callout_tick();
	}
}


void callout_tick_task_init(void)
{
	static bool initialized = false;
	rtems_status_code sc = RTEMS_SUCCESSFUL;

	if (!initialized) {
		rtems_id id = RTEMS_ID_NONE;

		initialized = true;

		sc = rtems_task_create(
			CALLOUT_TICK_TASK_NAME,
			CALLOUT_TICK_TASK_PRIO,
			CALLOUT_TICK_TASK_STACK_SIZE,
			RTEMS_DEFAULT_MODES,
			RTEMS_DEFAULT_ATTRIBUTES,
			&id
		);
		assert(sc == RTEMS_SUCCESSFUL);

		sc = rtems_task_start(id, callout_tick_task, 0);
		assert(sc == RTEMS_SUCCESSFUL);
	}
}

