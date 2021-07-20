/**
 * @file
 *
 * @ingroup rtems_bsd_machine
 *
 * @brief Implementation of a mutex with a simple priority inheritance
 * protocol.
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

#ifndef _RTEMS_BSD_MACHINE_RTEMS_BSD_MUTEXIMPL_H_
#define _RTEMS_BSD_MACHINE_RTEMS_BSD_MUTEXIMPL_H_

#include <machine/rtems-bsd-mutex.h>
#include <machine/rtems-bsd-support.h>

#include <sys/types.h>
#include <sys/lock.h>
#include <sys/systm.h>

#include <inttypes.h>

#include <rtems/thread.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/threadqimpl.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define	RTEMS_BSD_MUTEX_TQ_OPERATIONS \
    &_Thread_queue_Operations_priority_inherit

#if RTEMS_DEBUG
/*
 * Resource tracking. In GDB you can:
 *
 * define mutex-owned
 *  set $m = $arg0
 *  set $c = 0
 *  while $m != 0
 *   set $c = $c + 1
 *   if $m->queue.Queue.owner != 0
 *    printf "%08x %-40s\n", $m->queue.Queue.owner, $m->queue.Queue.name
 *   end
 *   set $m = $m->mutex_list.tqe_next
 *  end
 *  printf "Total: %d\n", $c
 * end
 *
 * (gdb) mutex-owned _bsd_bsd_mutexlist->tqh_first
 */
extern TAILQ_HEAD(_bsd_mutex_list, rtems_bsd_mutex) _bsd_mutexlist;
extern rtems_mutex _bsd_mutexlist_lock;
#endif /* RTEMS_DEBUG */

static inline void
rtems_bsd_mutex_init(struct lock_object *lk, struct lock_class *class,
   const char *name, const char *type, int flags)
{
	rtems_bsd_mutex *m = &lk->lo_mtx;

	_Thread_queue_Initialize(&m->queue, name);
	m->nest_level = 0;

	lock_init(lk, class, name, type, flags);

#if RTEMS_DEBUG
	rtems_mutex_lock(&_bsd_mutexlist_lock);
	TAILQ_INSERT_TAIL(&_bsd_mutexlist, m, mutex_list);
	rtems_mutex_unlock(&_bsd_mutexlist_lock);
#endif /* RTEMS_DEBUG */
}

void rtems_bsd_mutex_lock_more(struct lock_object *lk,
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
rtems_bsd_mutex_lock(struct lock_object *lk)
{
	ISR_Level isr_level;
	Thread_queue_Context queue_context;
	Thread_Control *executing;
	Thread_Control *owner;
	rtems_bsd_mutex *m = &lk->lo_mtx;

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
		rtems_bsd_mutex_lock_more(lk, owner, executing,
		    &queue_context);
	}
}

static inline int
rtems_bsd_mutex_trylock(struct lock_object *lk)
{
	int success;
	ISR_Level isr_level;
	Thread_queue_Context queue_context;
	Thread_Control *executing;
	Thread_Control *owner;
	rtems_bsd_mutex *m = &lk->lo_mtx;

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
		if ((lk->lo_flags & LO_RECURSABLE) == 0) {
			rtems_bsd_mutex_release(m, isr_level, &queue_context);
			panic("mutex trylock: %s: not LO_RECURSABLE\n",
			    m->queue.Queue.name);
		}

		++m->nest_level;
		success = 1;
	} else {
		success = 0;
	}

	rtems_bsd_mutex_release(m, isr_level, &queue_context);

	return (success);
}

static inline void
rtems_bsd_mutex_unlock(struct lock_object *lk)
{
	ISR_Level isr_level;
	Thread_queue_Context queue_context;
	Thread_Control *owner;
	Thread_Control *executing;
	int nest_level;
	rtems_bsd_mutex *m = &lk->lo_mtx;

	_Thread_queue_Context_initialize(&queue_context);
	rtems_bsd_mutex_isr_disable(isr_level, &queue_context);
	rtems_bsd_mutex_acquire_critical(m, &queue_context);

	nest_level = m->nest_level;
	owner = m->queue.Queue.owner;
	executing = _Thread_Executing;

	if (__predict_false(owner != executing)) {
		rtems_bsd_mutex_release(m, isr_level, &queue_context);
		panic("mutex unlock: %s: owner 0x%08" PRIx32
		    " != executing 0x%08" PRIx32 "\n", m->queue.Queue.name,
		    owner != NULL ? owner->Object.id : 0,
		    executing->Object.id);
	}

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

static inline Thread_Control *
rtems_bsd_mutex_owner(struct lock_object *lk)
{
	rtems_bsd_mutex *m = &lk->lo_mtx;
	return (m->queue.Queue.owner);
}

static inline int
rtems_bsd_mutex_owned(struct lock_object *lk)
{
	return (rtems_bsd_mutex_owner(lk) == _Thread_Get_executing());
}

static inline int
rtems_bsd_mutex_recursed(struct lock_object *lk)
{
	rtems_bsd_mutex *m = &lk->lo_mtx;
	return (m->nest_level != 0);
}

static inline const char *
rtems_bsd_mutex_name(struct lock_object *lk)
{
	rtems_bsd_mutex *m = &lk->lo_mtx;
	return (m->queue.Queue.name);
}

static inline void
rtems_bsd_mutex_destroy(struct lock_object *lk)
{
	rtems_bsd_mutex *m = &lk->lo_mtx;

	BSD_ASSERT(m->queue.Queue.heads == NULL);

	if (rtems_bsd_mutex_owned(lk)) {
		m->nest_level = 0;
		rtems_bsd_mutex_unlock(lk);
	}

#if RTEMS_DEBUG
	rtems_mutex_lock(&_bsd_mutexlist_lock);
	TAILQ_REMOVE(&_bsd_mutexlist, m, mutex_list);
	rtems_mutex_unlock(&_bsd_mutexlist_lock);
#endif /* RTEMS_DEBUG */

	_Thread_queue_Destroy(&m->queue);
	lock_destroy(lk);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_BSD_MACHINE_RTEMS_BSD_MUTEXIMPL_H_ */
