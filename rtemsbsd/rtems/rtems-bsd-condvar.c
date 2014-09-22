/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief TODO.
 */

/*
 * Copyright (c) 2009-2014 embedded brains GmbH. All rights reserved.
 *
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
#include <machine/rtems-bsd-support.h>

#include <rtems/score/threaddispatch.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/threadqimpl.h>

#include <rtems/bsd/sys/param.h>
#include <rtems/bsd/sys/types.h>
#include <sys/systm.h>
#include <rtems/bsd/sys/lock.h>
#include <sys/condvar.h>
#include <sys/mutex.h>

void
cv_init(struct cv *cv, const char *desc)
{

	cv->cv_description = desc;
	_Thread_queue_Initialize(&cv->cv_waiters, THREAD_QUEUE_DISCIPLINE_PRIORITY,
	    STATES_WAITING_FOR_CONDITION_VARIABLE, EWOULDBLOCK);
}

void
cv_destroy(struct cv *cv)
{

	BSD_ASSERT(_Thread_queue_First(&cv->cv_waiters) == NULL);
}

static int
_cv_wait_support(struct cv *cv, struct lock_object *lock, Watchdog_Interval timo, bool relock)
{
	int error;
	struct lock_class *class;
	int lock_state;
	Thread_Control *executing;

	_Thread_Disable_dispatch();

	class = LOCK_CLASS(lock);
	lock_state = (*class->lc_unlock)(lock);

	_Thread_queue_Enter_critical_section(&cv->cv_waiters);

	executing = _Thread_Executing;
	executing->Wait.return_code = 0;
	executing->Wait.queue = &cv->cv_waiters;

	_Thread_queue_Enqueue(&cv->cv_waiters, executing, timo);

	DROP_GIANT();

	_Thread_Enable_dispatch();

	PICKUP_GIANT();

	error = (int)executing->Wait.return_code;

	if (relock) {
		(*class->lc_lock)(lock, lock_state);
	}

	return (error);
}

void
_cv_wait(struct cv *cv, struct lock_object *lock)
{

	_cv_wait_support(cv, lock, 0, true);
}

void
_cv_wait_unlock(struct cv *cv, struct lock_object *lock)
{

	_cv_wait_support(cv, lock, 0, false);
}

int
_cv_timedwait(struct cv *cv, struct lock_object *lock, int timo)
{
	if (timo <= 0)
		timo = 1;

	return (_cv_wait_support(cv, lock, (Watchdog_Interval)timo, true));
}

void
cv_signal(struct cv *cv)
{

	_Thread_Disable_dispatch();
	_Thread_queue_Dequeue(&cv->cv_waiters);
	_Thread_Enable_dispatch();
}

void
cv_broadcastpri(struct cv *cv, int pri)
{

	_Thread_Disable_dispatch();

	while (_Thread_queue_Dequeue(&cv->cv_waiters) != NULL) {
		/* Again */
	}

	_Thread_Enable_dispatch();
}

int
_cv_wait_sig(struct cv *cv, struct lock_object *lock)
{

	return (_cv_wait_support(cv, lock, 0, true));
}

int
_cv_timedwait_sig(struct cv *cv, struct lock_object *lock, int timo)
{

	if (timo <= 0)
		timo = 1;

	return (_cv_wait_support(cv, lock, (Watchdog_Interval)timo, true));
}
