/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief TODO.
 */

/*
 * Copyright (c) 2011 OPTI Medical.  All rights reserved.
 *
 *  OPTI Medical
 *  235 Hembree Park Drive
 *  Roswell, GA 30076
 *  USA
 *  <kevin.kirspel@optimedical.com>
 *
 * Copyright (c) 2013, 2017 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * Copyright (c) 2006 John Baldwin <jhb@FreeBSD.org>
 * All rights reserved.
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

#include <sys/param.h>
#include <sys/types.h>
#include <sys/systm.h>
#include <sys/lock.h>
#include <sys/rwlock.h>

#ifndef INVARIANTS
#define _rw_assert(rw, what, file, line)
#endif

static void	assert_rw(const struct lock_object *lock, int what);
static void	lock_rw(struct lock_object *lock, uintptr_t how);
static uintptr_t unlock_rw(struct lock_object *lock);

struct lock_class lock_class_rw = {
	.lc_name = "rw",
	.lc_flags = LC_SLEEPLOCK | LC_RECURSABLE | LC_UPGRADABLE,
	.lc_assert = assert_rw,
	.lc_lock = lock_rw,
	.lc_unlock = unlock_rw,
};

#define	rw_wowner(rw) rtems_bsd_rwlock_wowner(&(rw)->rwlock)

#define	rw_recursed(rw) rtems_bsd_rwlock_recursed(&(rw)->rwlock)

void
assert_rw(const struct lock_object *lock, int what)
{

	rw_assert((const struct rwlock *)lock, what);
}

void
lock_rw(struct lock_object *lock, uintptr_t how)
{
	struct rwlock *rw;

	rw = (struct rwlock *)lock;
	if (how)
		rw_rlock(rw);
	else
		rw_wlock(rw);
}

uintptr_t
unlock_rw(struct lock_object *lock)
{
	struct rwlock *rw;

	rw = (struct rwlock *)lock;
	rw_assert(rw, RA_LOCKED | LA_NOTRECURSED);
	if (rw->rwlock.readers > 0) {
		rw_runlock(rw);
		return (1);
	} else {
		rw_wunlock(rw);
		return (0);
	}
}

void
rw_init_flags(struct rwlock *rw, const char *name, int opts)
{
	int flags;

	flags = LO_UPGRADABLE;
	if (opts & RW_RECURSE)
		flags |= LO_RECURSABLE;

	rtems_bsd_rwlock_init(&rw->lock_object, &rw->rwlock, &lock_class_rw,
	    name, NULL, flags);
}

void
rw_destroy(struct rwlock *rw)
{

	rtems_bsd_rwlock_destroy(&rw->lock_object, &rw->rwlock);
}

void
rw_sysinit(void *arg)
{
	struct rw_args *args = arg;

	rw_init((struct rwlock *)args->ra_rw, args->ra_desc);
}

void
rw_sysinit_flags(void *arg)
{
	struct rw_args_flags *args = arg;

	rw_init_flags((struct rwlock *)args->ra_rw, args->ra_desc,
	    args->ra_flags);
}

int
rw_wowned(struct rwlock *rw)
{

	return (rtems_bsd_rwlock_wowned(&rw->rwlock));
}

void
_rw_wlock(struct rwlock *rw, const char *file, int line)
{

	rtems_bsd_rwlock_wlock(&rw->lock_object, &rw->rwlock);
}

int
_rw_try_wlock(struct rwlock *rw, const char *file, int line)
{

	return (rtems_bsd_rwlock_try_wlock(&rw->lock_object, &rw->rwlock));
}

void
_rw_wunlock(struct rwlock *rw, const char *file, int line)
{

	rtems_bsd_rwlock_wunlock(&rw->rwlock);
}

void
_rw_rlock(struct rwlock *rw, const char *file, int line)
{

	rtems_bsd_rwlock_rlock(&rw->lock_object, &rw->rwlock);
}

int
_rw_try_rlock(struct rwlock *rw, const char *file, int line)
{

	return (rtems_bsd_rwlock_try_rlock(&rw->lock_object, &rw->rwlock));
}

void
_rw_runlock(struct rwlock *rw, const char *file, int line)
{

	rtems_bsd_rwlock_runlock(&rw->rwlock);
}

int
_rw_try_upgrade(struct rwlock *rw, const char *file, int line)
{

	return (rtems_bsd_rwlock_try_upgrade(&rw->rwlock));
}

void
_rw_downgrade(struct rwlock *rw, const char *file, int line)
{

	rtems_bsd_rwlock_downgrade(&rw->rwlock);
}

#ifdef INVARIANT_SUPPORT
/*
 * In the non-WITNESS case, rw_assert() can only detect that at least
 * *some* thread owns an rlock, but it cannot guarantee that *this*
 * thread owns an rlock.
 */
void
_rw_assert(const struct rwlock *rw, int what, const char *file, int line)
{
	const char *name = rtems_bsd_rwlock_name(&rw->rwlock);

	switch (what) {
	case RA_LOCKED:
	case RA_LOCKED | RA_RECURSED:
	case RA_LOCKED | RA_NOTRECURSED:
	case RA_RLOCKED:
	case RA_RLOCKED | RA_RECURSED:
	case RA_RLOCKED | RA_NOTRECURSED:
#ifdef WITNESS
		witness_assert(&rw->lock_object, what, file, line);
#else
		/*
		 * If some other thread has a write lock or we have one
		 * and are asserting a read lock, fail.  Also, if no one
		 * has a lock at all, fail.
		 */
		if ((rw->rwlock.readers == 0 && rw_wowner(rw) == NULL) ||
		    (rw->rwlock.readers == 0 && (what & RA_RLOCKED ||
		    rw_wowner(rw) != _Thread_Get_executing())))
			panic("Lock %s not %slocked @ %s:%d\n",
			    name, (what & RA_RLOCKED) ?
			    "read " : "", file, line);

		if (rw->rwlock.readers == 0 && !(what & RA_RLOCKED)) {
			if (rw_recursed(rw)) {
				if (what & RA_NOTRECURSED)
					panic("Lock %s recursed @ %s:%d\n",
					    name, file,
					    line);
			} else if (what & RA_RECURSED)
				panic("Lock %s not recursed @ %s:%d\n",
				    name, file, line);
		}
#endif
		break;
	case RA_WLOCKED:
	case RA_WLOCKED | RA_RECURSED:
	case RA_WLOCKED | RA_NOTRECURSED:
		if (rw_wowner(rw) != _Thread_Get_executing())
			panic("Lock %s not exclusively locked @ %s:%d\n",
			    name, file, line);
		if (rw_recursed(rw)) {
			if (what & RA_NOTRECURSED)
				panic("Lock %s recursed @ %s:%d\n",
				    name, file, line);
		} else if (what & RA_RECURSED)
			panic("Lock %s not recursed @ %s:%d\n",
			    name, file, line);
		break;
	case RA_UNLOCKED:
#ifdef WITNESS
		witness_assert(&rw->lock_object, what, file, line);
#else
		/*
		 * If we hold a write lock fail.  We can't reliably check
		 * to see if we hold a read lock or not.
		 */
		if (rw_wowner(rw) == _Thread_Get_executing())
			panic("Lock %s exclusively locked @ %s:%d\n",
			    name, file, line);
#endif
		break;
	default:
		panic("Unknown rw lock assertion: %d @ %s:%d", what, file,
		    line);
	}
}
#endif /* INVARIANT_SUPPORT */
