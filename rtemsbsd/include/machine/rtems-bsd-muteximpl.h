/**
 * @file
 *
 * @ingroup rtems_bsd_machine
 *
 * @brief Implementation of a mutex with a simple priority inheritance
 * protocol.
 */

/*
 * Copyright (c) 2014, 2017 embedded brains GmbH.  All rights reserved.
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
#include <rtems/score/threadqimpl.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define	RTEMS_BSD_MUTEX_TQ_OPERATIONS \
    &_Thread_queue_Operations_priority_inherit

static inline void
rtems_bsd_mutex_init(struct lock_object *lock, rtems_bsd_mutex *m,
    struct lock_class *class, const char *name, const char *type, int flags)
{
	_Thread_queue_Initialize(&m->queue);
	m->nest_level = 0;

	lock_init(lock, class, name, type, flags);
}

void rtems_bsd_mutex_lock_more(struct lock_object *lock, rtems_bsd_mutex *m,
    Thread_Control *owner, Thread_Control *executing,
    Thread_queue_Context *queue_context);

#define	rtems_bsd_mutex_isr_disable(isr_level, queue_context)	\
do {								\
	_ISR_Local_disable(isr_level);				\
	_ISR_lock_ISR_disable_profile(				\
	      &(queue_context)->Lock_context.Lock_context)	\
} while (0)

static inline void
rtems_bsd_mutex_acquire_critical(rtems_bsd_mutex *m,
    Thread_queue_Context *queue_context)
{

	_Thread_queue_Queue_acquire_critical(&m->queue.Queue,
	    &m->queue.Lock_stats, &queue_context->Lock_context.Lock_context);
#if defined(RTEMS_SMP) && defined(RTEMS_DEBUG)
	m->queue.owner = _SMP_lock_Who_am_I();
#endif
}

static inline void
rtems_bsd_mutex_release(rtems_bsd_mutex *m, ISR_Level isr_level,
    Thread_queue_Context *queue_context)
{

#if defined(RTEMS_SMP) && defined(RTEMS_DEBUG)
	_Assert( _Thread_queue_Is_lock_owner( &m->queue ) );
	m->queue.owner = SMP_LOCK_NO_OWNER;
#endif
	_Thread_queue_Queue_release_critical(&m->queue.Queue,
	    &queue_context->Lock_context.Lock_context);
	_ISR_Local_enable(isr_level);
}

static inline void
rtems_bsd_mutex_set_isr_level(Thread_queue_Context *queue_context,
    ISR_Level isr_level)
{

	_ISR_lock_Context_set_level(&queue_context->Lock_context.Lock_context,
	    isr_level);
}

static inline void
rtems_bsd_mutex_lock(struct lock_object *lock, rtems_bsd_mutex *m)
{
	ISR_Level isr_level;
	Thread_queue_Context queue_context;
	Thread_Control *executing;
	Thread_Control *owner;

	_Thread_queue_Context_initialize(&queue_context);
	rtems_bsd_mutex_isr_disable(isr_level, &queue_context);
	executing = _Thread_Executing;
	rtems_bsd_mutex_acquire_critical(m, &queue_context);

	owner = m->queue.Queue.owner;

	if (__predict_true(owner == NULL)) {
		m->queue.Queue.owner = executing;
		_Thread_Resource_count_increment(executing);
		rtems_bsd_mutex_release(m, isr_level, &queue_context);
	} else {
		rtems_bsd_mutex_set_isr_level(&queue_context, isr_level);
		rtems_bsd_mutex_lock_more(lock, m, owner, executing,
		    &queue_context);
	}
}

static inline int
rtems_bsd_mutex_trylock(struct lock_object *lock, rtems_bsd_mutex *m)
{
	int success;
	ISR_Level isr_level;
	Thread_queue_Context queue_context;
	Thread_Control *executing;
	Thread_Control *owner;

	_Thread_queue_Context_initialize(&queue_context);
	rtems_bsd_mutex_isr_disable(isr_level, &queue_context);
	executing = _Thread_Executing;
	rtems_bsd_mutex_acquire_critical(m, &queue_context);

	owner = m->queue.Queue.owner;

	if (owner == NULL) {
		m->queue.Queue.owner = executing;
		_Thread_Resource_count_increment(executing);
		success = 1;
	} else if (owner == executing) {
		BSD_ASSERT(lock->lo_flags & LO_RECURSABLE);
		++m->nest_level;
		success = 1;
	} else {
		success = 0;
	}

	rtems_bsd_mutex_release(m, isr_level, &queue_context);

	return (success);
}

static inline void
rtems_bsd_mutex_unlock(rtems_bsd_mutex *m)
{
	ISR_Level isr_level;
	Thread_queue_Context queue_context;
	Thread_Control *owner;
	int nest_level;

	_Thread_queue_Context_initialize(&queue_context);
	rtems_bsd_mutex_isr_disable(isr_level, &queue_context);
	rtems_bsd_mutex_acquire_critical(m, &queue_context);

	nest_level = m->nest_level;
	owner = m->queue.Queue.owner;

	BSD_ASSERT(owner == _Thread_Executing);

	if (__predict_true(nest_level == 0)) {
		Thread_queue_Heads *heads;

		heads = m->queue.Queue.heads;
		m->queue.Queue.owner = NULL;
		_Thread_Resource_count_decrement(owner);

		if (__predict_true(heads == NULL)) {
			rtems_bsd_mutex_release(m, isr_level, &queue_context);
		} else {
			rtems_bsd_mutex_set_isr_level(&queue_context, isr_level);
			_Thread_queue_Surrender(&m->queue.Queue, heads, owner,
			    &queue_context, RTEMS_BSD_MUTEX_TQ_OPERATIONS);
		}
	} else {
		m->nest_level = nest_level - 1;
		rtems_bsd_mutex_release(m, isr_level, &queue_context);
	}
}

static inline int
rtems_bsd_mutex_owned(rtems_bsd_mutex *m)
{

	return (m->queue.Queue.owner == _Thread_Get_executing());
}

static inline int
rtems_bsd_mutex_recursed(rtems_bsd_mutex *m)
{

	return (m->nest_level);
}

static inline void
rtems_bsd_mutex_destroy(struct lock_object *lock, rtems_bsd_mutex *m)
{
	BSD_ASSERT(m->queue.Queue.heads == NULL);

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
