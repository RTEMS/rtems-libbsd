/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief TODO.
 */

/*
 * Copyright (c) 2014, 2018 embedded brains GmbH.  All rights reserved.
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

void
rtems_bsd_mutex_lock_more(struct lock_object *lk,
    Thread_Control *owner, Thread_Control *executing,
    Thread_queue_Context *queue_context)
{
	rtems_bsd_mutex *m = &lk->lo_mtx;
	if (owner == executing) {
		if ((lk->lo_flags & LO_RECURSABLE) == 0) {
			_Thread_queue_Release(&m->queue, queue_context);
			panic("mutex lock: %s: not LO_RECURSABLE\n",
			    m->queue.Queue.name);
		}

		++m->nest_level;
		_Thread_queue_Release(&m->queue, queue_context);
	} else {
		_Thread_queue_Context_set_thread_state(queue_context,
		    STATES_WAITING_FOR_MUTEX);
		_Thread_queue_Context_set_enqueue_do_nothing_extra(
		    queue_context);
		_Thread_queue_Context_set_deadlock_callout(queue_context,
		    _Thread_queue_Deadlock_fatal);
		_Thread_queue_Enqueue(&m->queue.Queue,
		    RTEMS_BSD_MUTEX_TQ_OPERATIONS, executing, queue_context);
	}
}
