/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief TODO.
 */

/*
 * Copyright (c) 2009-2013 embedded brains GmbH. All rights reserved.
 *
 *  Dornierstr. 4
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

#include <machine/rtems-bsd-config.h>

#include <rtems/score/objectimpl.h>
#include <rtems/score/threaddispatch.h>
#include <rtems/score/threadqimpl.h>
#include <rtems/posix/condimpl.h>

#include <rtems/bsd/sys/param.h>
#include <rtems/bsd/sys/types.h>
#include <sys/systm.h>
#include <rtems/bsd/sys/lock.h>
#include <sys/mutex.h>
#include <sys/condvar.h>

RTEMS_CHAIN_DEFINE_EMPTY(rtems_bsd_condvar_chain);

void
cv_init(struct cv *cv, const char *desc)
{
	int rv = pthread_cond_init(&cv->cv_id, NULL);

	BSD_ASSERT_RV(rv);

	cv->cv_description = desc;

	rtems_chain_append(&rtems_bsd_condvar_chain, &cv->cv_node);
}

void
cv_destroy(struct cv *cv)
{
	int rv = pthread_cond_destroy(&cv->cv_id);

	BSD_ASSERT_RV(rv);

	rtems_chain_extract(&cv->cv_node);
}

static int _cv_wait_support(struct cv *cv, struct lock_object *lock, int timo, bool relock)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;
	int eno = 0;
	Objects_Locations location = OBJECTS_ERROR;
	POSIX_Condition_variables_Control *pcv = _POSIX_Condition_variables_Get(&cv->cv_id, &location);

	if (location == OBJECTS_LOCAL) {
		if (pcv->Mutex != POSIX_CONDITION_VARIABLES_NO_MUTEX && pcv->Mutex != lock->lo_id) {
			_Thread_Enable_dispatch();

			BSD_ASSERT(false);

			return EINVAL;
		}

		sc = rtems_semaphore_release(lock->lo_id);
		if (sc != RTEMS_SUCCESSFUL) {
			_Thread_Enable_dispatch();

			BSD_ASSERT(false);

			return EINVAL;
		}

		pcv->Mutex = lock->lo_id;

		_Thread_queue_Enter_critical_section(&pcv->Wait_queue);
		_Thread_Executing->Wait.return_code = 0;
		_Thread_Executing->Wait.queue = &pcv->Wait_queue;
		_Thread_Executing->Wait.id = cv->cv_id;

		/* FIXME: Integer conversion */
		_Thread_queue_Enqueue(&pcv->Wait_queue, _Thread_Executing, (Watchdog_Interval) timo);

		DROP_GIANT();

		_Thread_Enable_dispatch();

		PICKUP_GIANT();

		eno = (int) _Thread_Executing->Wait.return_code;
		if (eno != 0) {
			if (eno == ETIMEDOUT) {
				eno = EWOULDBLOCK;
			} else {
				BSD_ASSERT(false);

				eno = EINVAL;
			}
		}

		if (relock) {
			sc = rtems_semaphore_obtain(lock->lo_id, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
			if (sc != RTEMS_SUCCESSFUL) {
				BSD_ASSERT(false);

				eno = EINVAL;
			}
		}

		return eno;
	}

	BSD_PANIC("unexpected object location");
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
	if (timo <= 0) {
		timo = 1;
	}

	return _cv_wait_support(cv, lock, timo, true);
}

void
cv_signal(struct cv *cv)
{
	int rv = pthread_cond_signal(&cv->cv_id);

	BSD_ASSERT_RV(rv);
}

void
cv_broadcastpri(struct cv *cv, int pri)
{
	int rv = 0;

	BSD_ASSERT(pri == 0);

	rv = pthread_cond_broadcast(&cv->cv_id);
	BSD_ASSERT_RV(rv);
}
int
_cv_wait_sig(struct cv *cvp, struct lock_object *lock)
{
  /* XXX */
	_cv_wait_support(cvp, lock, 0, true);
}

int
_cv_timedwait_sig(struct cv *cvp, struct lock_object *lock, int timo)
{
  /* XXX */
	if (timo <= 0) {
		timo = 1;
	}

	return _cv_wait_support(cvp, lock, timo, true);
}
