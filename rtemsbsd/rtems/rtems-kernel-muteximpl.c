/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief TODO.
 */

/*
 * Copyright (c) 2014, 2015 embedded brains GmbH.  All rights reserved.
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

#include <machine/rtems-bsd-kernel-space.h>
#include <machine/rtems-bsd-muteximpl.h>

#include <rtems/score/schedulerimpl.h>
#include <rtems/score/threadqimpl.h>

#define BSD_MUTEX_TQ_OPERATIONS &_Thread_queue_Operations_priority

void
rtems_bsd_mutex_lock_more(struct lock_object *lock, rtems_bsd_mutex *m,
    Thread_Control *owner, Thread_Control *executing,
    Thread_queue_Context *queue_context)
{
	if (owner == executing) {
		BSD_ASSERT(lock->lo_flags & LO_RECURSABLE);
		++m->nest_level;

		_Thread_queue_Release(&m->queue, &queue_context->Lock_context);
	} else {
		/* Priority inheritance */
		_Thread_Raise_priority(owner, executing->current_priority);

		++executing->resource_count;
		_Thread_queue_Context_set_expected_level(queue_context, 1);
		_Thread_queue_Context_set_no_timeout(queue_context);
		_Thread_queue_Enqueue_critical(&m->queue,
		    BSD_MUTEX_TQ_OPERATIONS, executing,
		    STATES_WAITING_FOR_MUTEX, queue_context);
	}
}

void
rtems_bsd_mutex_unlock_more(rtems_bsd_mutex *m, Thread_Control *owner,
    int keep_priority, Thread_queue_Heads *heads,
    Thread_queue_Context *queue_context)
{
	if (heads != NULL) {
		const Thread_queue_Operations *operations;
		Thread_Control *new_owner;

		operations = BSD_MUTEX_TQ_OPERATIONS;
		new_owner = ( *operations->first )( heads );
		m->owner = new_owner;
		_Thread_queue_Extract_critical(&m->queue, operations,
		    new_owner, queue_context);
	} else {
		_Thread_queue_Release(&m->queue, &queue_context->Lock_context);
	}

	if (!keep_priority) {
		Per_CPU_Control *cpu_self;

		cpu_self = _Thread_Dispatch_disable();
		_Thread_Restore_priority(owner);
		_Thread_Dispatch_enable(cpu_self);
	}
}
