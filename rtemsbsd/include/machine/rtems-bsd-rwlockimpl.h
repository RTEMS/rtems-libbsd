/**
 * @file
 *
 * @ingroup rtems_bsd_machine
 *
 * @brief Implementation of a reader/writer lock with priority inheritance for
 * exclusive owners (writer).
 */

/*
 * Copyright (c) 2017 embedded brains GmbH.  All rights reserved.
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

#ifndef _RTEMS_BSD_MACHINE_RTEMS_BSD_RWLOCKIMPL_H_
#define _RTEMS_BSD_MACHINE_RTEMS_BSD_RWLOCKIMPL_H_

#include <machine/rtems-bsd-rwlock.h>
#include <machine/rtems-bsd-support.h>

#include <sys/types.h>
#include <sys/lock.h>

#include <rtems/score/threadimpl.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
	Thread_queue_Context writer;
	Thread_queue_Context reader;
} rtems_bsd_rwlock_context;

static inline void
rtems_bsd_rwlock_context_init(rtems_bsd_rwlock_context *context)
{

	_Thread_queue_Context_initialize(&context->writer);
}

static inline void
rtems_bsd_rwlock_init(struct lock_object *lock, rtems_bsd_rwlock *rw,
    struct lock_class *class, const char *name, const char *type, int flags)
{
	_Thread_queue_Initialize(&rw->writer_queue, name);
	_Thread_queue_Initialize(&rw->reader_queue, name);
	rw->readers = 0;
	rw->nest_level = 0;
	lock_init(lock, class, name, type, flags);
}

void rtems_bsd_rwlock_wlock_more(const struct lock_object *lock,
    rtems_bsd_rwlock *rw, Thread_Control *executing,
    rtems_bsd_rwlock_context *context);

void rtems_bsd_rwlock_wunlock_more(rtems_bsd_rwlock *rw,
    Thread_Control *wowner, rtems_bsd_rwlock_context *context);

void rtems_bsd_rwlock_rlock_more(rtems_bsd_rwlock *rw,
    rtems_bsd_rwlock_context *context);

void rtems_bsd_rwlock_runlock_more(rtems_bsd_rwlock *rw,
    rtems_bsd_rwlock_context *context);

void rtems_bsd_rwlock_ready_waiting_readers(rtems_bsd_rwlock *rw,
    rtems_bsd_rwlock_context *context);

#define	rtems_bsd_rwlock_isr_disable(isr_level, context)	\
do {								\
	_ISR_Local_disable(isr_level);				\
	_ISR_lock_ISR_disable_profile(				\
	      &(context)->writer.Lock_context.Lock_context)	\
} while (0)

static inline void
rtems_bsd_rwlock_acquire_critical(rtems_bsd_rwlock *rw,
    rtems_bsd_rwlock_context *context)
{

	_Thread_queue_Queue_acquire_critical(&rw->writer_queue.Queue,
	    &rw->writer_queue.Lock_stats,
	    &context->writer.Lock_context.Lock_context);
#if defined(RTEMS_SMP) && defined(RTEMS_DEBUG)
	rw->writer_queue.owner = _SMP_lock_Who_am_I();
#endif
}

static inline void
rtems_bsd_rwlock_release(rtems_bsd_rwlock *rw, ISR_Level isr_level,
    rtems_bsd_rwlock_context *context)
{

#if defined(RTEMS_SMP) && defined(RTEMS_DEBUG)
	_Assert( _Thread_queue_Is_lock_owner( &rw->writer_queue ) );
	rw->writer_queue.owner = SMP_LOCK_NO_OWNER;
#endif
	_Thread_queue_Queue_release_critical(&rw->writer_queue.Queue,
	    &context->writer.Lock_context.Lock_context);
	_ISR_Local_enable(isr_level);
}

static inline void
rtems_bsd_rwlock_set_isr_level(rtems_bsd_rwlock_context *context,
    ISR_Level isr_level)
{

	_ISR_lock_Context_set_level(&context->writer.Lock_context.Lock_context,
	    isr_level);
}

static inline Thread_Control *
rtems_bsd_rwlock_wowner(const rtems_bsd_rwlock *rw)
{

	return (rw->writer_queue.Queue.owner);
}

static inline void
rtems_bsd_rwlock_set_wowner(rtems_bsd_rwlock *rw, Thread_Control *wowner)
{

	rw->writer_queue.Queue.owner = wowner;
}

static inline void
rtems_bsd_rwlock_wlock(struct lock_object *lock, rtems_bsd_rwlock *rw)
{
	ISR_Level isr_level;
	rtems_bsd_rwlock_context context;
	Thread_Control *executing;

	rtems_bsd_rwlock_context_init(&context);
	rtems_bsd_rwlock_isr_disable(isr_level, &context);
	executing = _Thread_Executing;
	rtems_bsd_rwlock_acquire_critical(rw, &context);

	if (__predict_true(rtems_bsd_rwlock_wowner(rw) == NULL &&
	    rw->readers == 0)) {
		rtems_bsd_rwlock_set_wowner(rw, executing);
		_Thread_Resource_count_increment(executing);
		rtems_bsd_rwlock_release(rw, isr_level, &context);
	} else {
		rtems_bsd_rwlock_set_isr_level(&context, isr_level);
		rtems_bsd_rwlock_wlock_more(lock, rw, executing,
		    &context);
	}
}

static inline int
rtems_bsd_rwlock_try_wlock(struct lock_object *lock, rtems_bsd_rwlock *rw)
{
	int success;
	ISR_Level isr_level;
	rtems_bsd_rwlock_context context;
	Thread_Control *executing;

	rtems_bsd_rwlock_context_init(&context);
	rtems_bsd_rwlock_isr_disable(isr_level, &context);
	executing = _Thread_Executing;
	rtems_bsd_rwlock_acquire_critical(rw, &context);

	if (rw->readers == 0) {
		Thread_Control *wowner;

		wowner = rtems_bsd_rwlock_wowner(rw);

		if (wowner == NULL) {
			rtems_bsd_rwlock_set_wowner(rw, executing);
			_Thread_Resource_count_increment(executing);
			success = 1;
		} else if (wowner == executing) {
			BSD_ASSERT(lock->lo_flags & LO_RECURSABLE);
			++rw->nest_level;
			success = 1;
		} else {
			success = 0;
		}
	} else {
		success = 0;
	}

	rtems_bsd_rwlock_release(rw, isr_level, &context);

	return (success);
}

static inline void
rtems_bsd_rwlock_wunlock(rtems_bsd_rwlock *rw)
{
	ISR_Level isr_level;
	rtems_bsd_rwlock_context context;
	Thread_Control *wowner;
	int nest_level;

	rtems_bsd_rwlock_context_init(&context);
	rtems_bsd_rwlock_isr_disable(isr_level, &context);
	rtems_bsd_rwlock_acquire_critical(rw, &context);

	nest_level = rw->nest_level;
	wowner = rtems_bsd_rwlock_wowner(rw);

	BSD_ASSERT(wowner == _Thread_Executing);

	if (__predict_true(nest_level == 0)) {
		rtems_bsd_rwlock_set_wowner(rw, NULL);
		_Thread_Resource_count_decrement(wowner);

		if (__predict_true(
		    _Thread_queue_Is_empty(&rw->writer_queue.Queue) &&
		    _Thread_queue_Is_empty(&rw->reader_queue.Queue))) {
			rtems_bsd_rwlock_release(rw, isr_level, &context);
		} else {
			rtems_bsd_rwlock_set_isr_level(&context,
			    isr_level);
			rtems_bsd_rwlock_wunlock_more(rw, wowner,
			    &context);
		}
	} else {
		rw->nest_level = nest_level - 1;
		rtems_bsd_rwlock_release(rw, isr_level, &context);
	}
}

static inline int
rtems_bsd_rwlock_wowned(const rtems_bsd_rwlock *rw)
{

	return (rtems_bsd_rwlock_wowner(rw) == _Thread_Get_executing());
}

static inline int
rtems_bsd_rwlock_recursed(const rtems_bsd_rwlock *rw)
{

	return (rw->nest_level != 0);
}

static inline void
rtems_bsd_rwlock_rlock(struct lock_object *lock, rtems_bsd_rwlock *rw)
{
	ISR_Level isr_level;
	rtems_bsd_rwlock_context context;

	rtems_bsd_rwlock_context_init(&context);
	rtems_bsd_rwlock_isr_disable(isr_level, &context);
	rtems_bsd_rwlock_acquire_critical(rw, &context);

	if (__predict_true(rtems_bsd_rwlock_wowner(rw) == NULL &&
	   _Thread_queue_Is_empty(&rw->writer_queue.Queue))) {
		++rw->readers;
		_Thread_Resource_count_increment(_Thread_Executing);
		rtems_bsd_rwlock_release(rw, isr_level, &context);
	} else {
		rtems_bsd_rwlock_set_isr_level(&context, isr_level);
		rtems_bsd_rwlock_rlock_more(rw, &context);
	}
}

static inline int
rtems_bsd_rwlock_try_rlock(struct lock_object *lock, rtems_bsd_rwlock *rw)
{
	int success;
	ISR_Level isr_level;
	rtems_bsd_rwlock_context context;

	rtems_bsd_rwlock_context_init(&context);
	rtems_bsd_rwlock_isr_disable(isr_level, &context);
	rtems_bsd_rwlock_acquire_critical(rw, &context);

	if (__predict_true(rtems_bsd_rwlock_wowner(rw) == NULL &&
	   _Thread_queue_Is_empty(&rw->writer_queue.Queue))) {
		++rw->readers;
		_Thread_Resource_count_increment(_Thread_Executing);
		success = 1;
	} else {
		success = 0;
	}

	rtems_bsd_rwlock_release(rw, isr_level, &context);

	return (success);
}

static inline void
rtems_bsd_rwlock_runlock(rtems_bsd_rwlock *rw)
{
	ISR_Level isr_level;
	rtems_bsd_rwlock_context context;
	int readers;

	rtems_bsd_rwlock_context_init(&context);
	rtems_bsd_rwlock_isr_disable(isr_level, &context);
	rtems_bsd_rwlock_acquire_critical(rw, &context);

	readers = rw->readers;
	_Thread_Resource_count_decrement(_Thread_Executing);

	if (__predict_true(readers == 1)) {
		rw->readers = 0;

		if (__predict_true(
		    _Thread_queue_Is_empty(&rw->writer_queue.Queue) &&
		    _Thread_queue_Is_empty(&rw->reader_queue.Queue))) {
			rtems_bsd_rwlock_release(rw, isr_level,
			    &context);
		} else {
			rtems_bsd_rwlock_set_isr_level(&context,
			    isr_level);
			rtems_bsd_rwlock_runlock_more(rw, &context);
		}
	} else {
		rw->readers = readers - 1;
		rtems_bsd_rwlock_release(rw, isr_level, &context);
	}
}

static inline int
rtems_bsd_rwlock_try_upgrade(rtems_bsd_rwlock *rw)
{
	int success;
	ISR_Level isr_level;
	rtems_bsd_rwlock_context context;
	Thread_Control *executing;
	Thread_Control *wowner;

	rtems_bsd_rwlock_context_init(&context);
	rtems_bsd_rwlock_isr_disable(isr_level, &context);
	executing = _Thread_Executing;
	rtems_bsd_rwlock_acquire_critical(rw, &context);

	wowner = rtems_bsd_rwlock_wowner(rw);
	BSD_ASSERT(wowner == NULL);

	if (rw->readers == 1) {
		rw->readers = 0;
		rtems_bsd_rwlock_set_wowner(rw, executing);
		/* FIXME: priority inheritance */
		success = 1;
	} else {
		success = 0;
	}

	rtems_bsd_rwlock_release(rw, isr_level, &context);

	return (success);
}

static inline void
rtems_bsd_rwlock_downgrade(rtems_bsd_rwlock *rw)
{
	ISR_Level isr_level;
	rtems_bsd_rwlock_context context;
	Thread_Control *wowner;

	rtems_bsd_rwlock_context_init(&context);
	rtems_bsd_rwlock_isr_disable(isr_level, &context);
	rtems_bsd_rwlock_acquire_critical(rw, &context);

	wowner = rtems_bsd_rwlock_wowner(rw);

	BSD_ASSERT(wowner == _Thread_Executing);
	BSD_ASSERT(rw->nest_level == 0);

	rtems_bsd_rwlock_set_wowner(rw, NULL);
	rw->readers = 1;

	if (__predict_true(_Thread_queue_Is_empty(&rw->reader_queue.Queue))) {
		rtems_bsd_rwlock_release(rw, isr_level, &context);
	} else {
		rtems_bsd_rwlock_set_isr_level(&context, isr_level);
		rtems_bsd_rwlock_ready_waiting_readers(rw, &context);
	}
}

static inline const char *
rtems_bsd_rwlock_name(const rtems_bsd_rwlock *rw)
{

	return (rw->writer_queue.Queue.name);
}

static inline void
rtems_bsd_rwlock_destroy(struct lock_object *lock, rtems_bsd_rwlock *rw)
{
	BSD_ASSERT(_Thread_queue_Is_empty(&rw->writer_queue.Queue));
	BSD_ASSERT(_Thread_queue_Is_empty(&rw->reader_queue.Queue));

	if (rtems_bsd_rwlock_wowned(rw)) {
		rw->nest_level = 0;
		rtems_bsd_rwlock_wunlock(rw);
	}

	_Thread_queue_Destroy(&rw->writer_queue);
	_Thread_queue_Destroy(&rw->reader_queue);
	lock_destroy(lock);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_BSD_MACHINE_RTEMS_BSD_RWLOCKIMPL_H_ */
