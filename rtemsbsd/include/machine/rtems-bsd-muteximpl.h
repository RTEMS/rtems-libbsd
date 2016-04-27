/**
 * @file
 *
 * @ingroup rtems_bsd_machine
 *
 * @brief Implementation of a mutex with a simple priority inheritance
 * protocol.
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

#ifndef _RTEMS_BSD_MACHINE_RTEMS_BSD_MUTEXIMPL_H_
#define _RTEMS_BSD_MACHINE_RTEMS_BSD_MUTEXIMPL_H_

#include <machine/rtems-bsd-mutex.h>
#include <machine/rtems-bsd-support.h>

#include <sys/types.h>
#include <rtems/bsd/sys/lock.h>

#include <rtems/score/threadimpl.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

static inline void
rtems_bsd_mutex_init(struct lock_object *lock, rtems_bsd_mutex *m,
    struct lock_class *class, const char *name, const char *type, int flags)
{
	_Thread_queue_Initialize(&m->queue);
	m->owner = NULL;
	m->nest_level = 0;

	lock_init(lock, class, name, type, flags);
}

void rtems_bsd_mutex_lock_more(struct lock_object *lock, rtems_bsd_mutex *m,
    Thread_Control *owner, Thread_Control *executing,
    Thread_queue_Context *queue_context);

static inline void
rtems_bsd_mutex_lock(struct lock_object *lock, rtems_bsd_mutex *m)
{
	Thread_queue_Context queue_context;
	Thread_Control *executing;
	Thread_Control *owner;

	_Thread_queue_Context_initialize(&queue_context, NULL);
	_Thread_queue_Acquire(&m->queue, &queue_context.Lock_context);

	owner = m->owner;
	executing = _Thread_Executing;

	if (__predict_true(owner == NULL)) {
		m->owner = executing;
		++executing->resource_count;

		_Thread_queue_Release(&m->queue, &queue_context.Lock_context);
	} else {
		rtems_bsd_mutex_lock_more(lock, m, owner, executing,
		    &queue_context);
	}
}

static inline int
rtems_bsd_mutex_trylock(struct lock_object *lock, rtems_bsd_mutex *m)
{
	int success;
	Thread_queue_Context queue_context;
	Thread_Control *executing;
	Thread_Control *owner;

	_Thread_queue_Context_initialize(&queue_context, NULL);
	_Thread_queue_Acquire(&m->queue, &queue_context.Lock_context);

	owner = m->owner;
	executing = _Thread_Executing;

	if (owner == NULL) {
		m->owner = executing;
		++executing->resource_count;
		success = 1;
	} else if (owner == executing) {
		BSD_ASSERT(lock->lo_flags & LO_RECURSABLE);
		++m->nest_level;
		success = 1;
	} else {
		success = 0;
	}

	_Thread_queue_Release(&m->queue, &queue_context.Lock_context);

	return (success);
}

void rtems_bsd_mutex_unlock_more(rtems_bsd_mutex *m, Thread_Control *owner,
    int keep_priority, Thread_queue_Heads *heads,
    Thread_queue_Context *queue_context);

static inline void
rtems_bsd_mutex_unlock(rtems_bsd_mutex *m)
{
	Thread_queue_Context queue_context;
	Thread_Control *owner;
	int nest_level;

	_Thread_queue_Context_initialize(&queue_context, NULL);
	_Thread_queue_Acquire(&m->queue, &queue_context.Lock_context);

	nest_level = m->nest_level;
	owner = m->owner;

	BSD_ASSERT(owner == _Thread_Executing);

	if (__predict_true(nest_level == 0)) {
		Thread_queue_Heads *heads;
		int keep_priority;

		--owner->resource_count;

		/*
		 * Ensure that the owner resource count is visible to all other
		 * processors and that we read the latest priority restore
		 * hint.
		 */
		_Atomic_Fence( ATOMIC_ORDER_ACQ_REL );

		heads = m->queue.heads;
		keep_priority = _Thread_Owns_resources(owner)
		    || !owner->priority_restore_hint;

		m->owner = NULL;

		if (__predict_true(heads == NULL && keep_priority)) {
			_Thread_queue_Release(&m->queue, &queue_context.Lock_context);
		} else {
			rtems_bsd_mutex_unlock_more(m, owner, keep_priority,
			    heads, &queue_context);
		}

	} else {
		m->nest_level = nest_level - 1;

		_Thread_queue_Release(&m->queue, &queue_context.Lock_context);
	}
}

static inline int
rtems_bsd_mutex_owned(rtems_bsd_mutex *m)
{

	return (m->owner == _Thread_Get_executing());
}

static inline int
rtems_bsd_mutex_recursed(rtems_bsd_mutex *m)
{

	return (m->nest_level);
}

static inline void
rtems_bsd_mutex_destroy(struct lock_object *lock, rtems_bsd_mutex *m)
{
	BSD_ASSERT(m->queue.heads == NULL);

	if (rtems_bsd_mutex_owned(m)) {
		m->nest_level = 0;
		rtems_bsd_mutex_unlock(m);
	}

	_Thread_queue_Destroy(&m->queue);
	lock_destroy(lock);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_BSD_MACHINE_RTEMS_BSD_MUTEXIMPL_H_ */
