/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief TODO.
 */

/*
 * Copyright (c) 2009, 2010 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

/* Necessary to obtain some internal functions */
#define __RTEMS_VIOLATE_KERNEL_VISIBILITY__

#include <rtems/freebsd/machine/rtems-bsd-config.h>

#include <rtems/posix/cond.h>

#include <rtems/freebsd/sys/param.h>
#include <rtems/freebsd/sys/types.h>
#include <rtems/freebsd/sys/systm.h>
#include <rtems/freebsd/sys/lock.h>
#include <rtems/freebsd/sys/mutex.h>
#include <rtems/freebsd/sys/condvar.h>

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
		_Thread_queue_Enqueue(&pcv->Wait_queue, (Watchdog_Interval) timo);

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
