/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief TODO.
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

#include <machine/rtems-bsd-kernel-space.h>
#include <machine/rtems-bsd-rwlockimpl.h>

#include <rtems/score/schedulerimpl.h>

void
rtems_bsd_rwlock_wlock_more(const struct lock_object *lock,
    rtems_bsd_rwlock *rw, Thread_Control *executing,
    rtems_bsd_rwlock_context *context)
{
	Thread_Control *wowner;

	wowner = rtems_bsd_rwlock_wowner(rw);

	if (wowner == executing) {
		BSD_ASSERT(lock->lo_flags & LO_RECURSABLE);
		++rw->nest_level;

		_Thread_queue_Release(&rw->writer_queue, &context->writer);
	} else {
		_Thread_queue_Context_set_thread_state(&context->writer,
		    STATES_WAITING_FOR_RWLOCK);
		_Thread_queue_Context_set_enqueue_do_nothing_extra(
		    &context->writer);
		_Thread_queue_Context_set_deadlock_callout(&context->writer,
		    _Thread_queue_Deadlock_fatal);
		_Thread_queue_Enqueue(&rw->writer_queue.Queue,
		    &_Thread_queue_Operations_priority, executing,
		    &context->writer);
	}
}

static Thread_Control *
rtems_bsd_rwlock_flush_reader_filter(Thread_Control *reader,
    Thread_queue_Queue *queue, Thread_queue_Context *queue_context)
{
	rtems_bsd_rwlock *rw;

	rw = RTEMS_CONTAINER_OF(queue, rtems_bsd_rwlock, reader_queue.Queue);
	++rw->readers;
	_Thread_Resource_count_increment(reader);
	return (reader);
}

static void
rtems_bsd_rwlock_flush_reader_post_release(Thread_queue_Queue *queue,
    Thread_queue_Context *queue_context)
{
	rtems_bsd_rwlock *rw;
	rtems_bsd_rwlock_context *context;

	rw = RTEMS_CONTAINER_OF(queue, rtems_bsd_rwlock, reader_queue.Queue);
	context = RTEMS_CONTAINER_OF(queue_context, rtems_bsd_rwlock_context,
	    reader);
	_Thread_queue_Release(&rw->writer_queue, &context->writer);
}

void
rtems_bsd_rwlock_wunlock_more(rtems_bsd_rwlock *rw, Thread_Control *wowner,
    rtems_bsd_rwlock_context *context)
{

	if (!_Thread_queue_Is_empty(&rw->reader_queue.Queue)) {
		BSD_ASSERT(rw->readers == 0);
		rtems_bsd_rwlock_ready_waiting_readers(rw, context);
	} else {
		BSD_ASSERT(!_Thread_queue_Is_empty(&rw->writer_queue.Queue));
		_Thread_queue_Surrender(&rw->writer_queue.Queue,
		    rw->writer_queue.Queue.heads, wowner, &context->writer,
		    &_Thread_queue_Operations_priority);
	}
}

static void
rtems_bsd_rwlock_reader_enqueue(Thread_queue_Queue *queue,
    Thread_Control *executing, Per_CPU_Control *cpu_self,
    Thread_queue_Context *queue_context
)
{
	rtems_bsd_rwlock *rw;
	rtems_bsd_rwlock_context *context;

	rw = RTEMS_CONTAINER_OF(queue, rtems_bsd_rwlock, reader_queue.Queue);
	context = RTEMS_CONTAINER_OF(queue_context, rtems_bsd_rwlock_context,
	    reader);
	_Thread_queue_Release(&rw->writer_queue, &context->writer);
}

void
rtems_bsd_rwlock_rlock_more(rtems_bsd_rwlock *rw,
    rtems_bsd_rwlock_context *context)
{
	Thread_Control *executing;

	executing = _Thread_Executing;

	_Thread_queue_Context_initialize(&context->reader);
	_Thread_queue_Context_set_thread_state(&context->reader,
	    STATES_WAITING_FOR_RWLOCK);
	_Thread_queue_Context_set_enqueue_callout(
	    &context->reader, rtems_bsd_rwlock_reader_enqueue);
	_Thread_queue_Context_set_deadlock_callout(&context->reader,
	    _Thread_queue_Deadlock_fatal);
	_Thread_queue_Acquire(&rw->reader_queue, &context->reader);
	_Thread_queue_Enqueue(&rw->reader_queue.Queue,
	    &_Thread_queue_Operations_FIFO, executing, &context->reader);
}

void
rtems_bsd_rwlock_runlock_more(rtems_bsd_rwlock *rw,
    rtems_bsd_rwlock_context *context)
{

	if (!_Thread_queue_Is_empty(&rw->writer_queue.Queue)) {
		BSD_ASSERT(rw->readers == 0);

		_Thread_queue_Surrender(&rw->writer_queue.Queue,
		    rw->writer_queue.Queue.heads, NULL, &context->writer,
		    &_Thread_queue_Operations_priority);
	} else {
		BSD_ASSERT(!_Thread_queue_Is_empty(&rw->reader_queue.Queue));
		rtems_bsd_rwlock_ready_waiting_readers(rw, context);
	}
}

void
rtems_bsd_rwlock_ready_waiting_readers(rtems_bsd_rwlock *rw,
    rtems_bsd_rwlock_context *context)
{

	_Thread_queue_Context_initialize(&context->reader);
	_Thread_queue_Acquire(&rw->reader_queue, &context->reader);
	_Thread_queue_Flush_critical(&rw->reader_queue.Queue,
	    &_Thread_queue_Operations_FIFO,
	    rtems_bsd_rwlock_flush_reader_filter,
	    rtems_bsd_rwlock_flush_reader_post_release,
	    &context->reader);
}
