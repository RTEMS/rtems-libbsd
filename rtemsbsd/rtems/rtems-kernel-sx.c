/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief TODO.
 */

/*
 * Copyright (c) 2009, 2017 embedded brains GmbH.  All rights reserved.
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
#include <machine/rtems-bsd-thread.h>

#include <sys/param.h>
#include <sys/types.h>
#include <sys/systm.h>
#include <sys/lock.h>
#include <sys/sx.h>

static void	assert_sx(const struct lock_object *lock, int what);
static void	lock_sx(struct lock_object *lock, uintptr_t how);
static uintptr_t unlock_sx(struct lock_object *lock);

struct lock_class lock_class_sx = {
	.lc_name = "sx",
	.lc_flags = LC_SLEEPLOCK | LC_SLEEPABLE | LC_RECURSABLE | LC_UPGRADABLE,
	.lc_assert = assert_sx,
	.lc_lock = lock_sx,
	.lc_unlock = unlock_sx,
};

#define	sx_xholder(sx) rtems_bsd_rwlock_wowner(&(sx)->rwlock)

#define	sx_recursed(sx) rtems_bsd_rwlock_recursed(&(sx)->rwlock)

void
assert_sx(const struct lock_object *lock, int what)
{

	sx_assert((const struct sx *)lock, what);
}

void
lock_sx(struct lock_object *lock, uintptr_t how)
{
	struct sx *sx;

	sx = (struct sx *)lock;
	if (how)
		sx_slock(sx);
	else
		sx_xlock(sx);
}

uintptr_t
unlock_sx(struct lock_object *lock)
{
	struct sx *sx;

	sx = (struct sx *)lock;
	sx_assert(sx, SA_LOCKED | SA_NOTRECURSED);
	if (sx->rwlock.readers > 0) {
		sx_sunlock(sx);
		return (1);
	} else {
		sx_xunlock(sx);
		return (0);
	}
}

void
sx_sysinit(void *arg)
{
	struct sx_args *sargs = arg;

	sx_init(sargs->sa_sx, sargs->sa_desc);
}

void
sx_init_flags(struct sx *sx, const char *description, int opts)
{
	int flags;

	flags = LO_SLEEPABLE | LO_UPGRADABLE;
	if (opts & SX_RECURSE)
		flags |= LO_RECURSABLE;

	rtems_bsd_rwlock_init(&sx->lock_object, &sx->rwlock, &lock_class_sx,
	    description, NULL, flags);
}

void
sx_destroy(struct sx *sx)
{

	rtems_bsd_rwlock_destroy(&sx->lock_object, &sx->rwlock);
}

int
_sx_xlock(struct sx *sx, int opts, const char *file, int line)
{

	rtems_bsd_rwlock_wlock(&sx->lock_object, &sx->rwlock);
	return (0);
}

int
sx_try_xlock_(struct sx *sx, const char *file, int line)
{

	return (rtems_bsd_rwlock_try_wlock(&sx->lock_object, &sx->rwlock));
}

void
_sx_xunlock(struct sx *sx, const char *file, int line)
{

	rtems_bsd_rwlock_wunlock(&sx->rwlock);
}

int
sx_try_upgrade_(struct sx *sx, const char *file, int line)
{

	return (rtems_bsd_rwlock_try_upgrade(&sx->rwlock));
}

void
sx_downgrade_(struct sx *sx, const char *file, int line)
{

	rtems_bsd_rwlock_downgrade(&sx->rwlock);
}

int
_sx_slock(struct sx *sx, int opts, const char *file, int line)
{

	rtems_bsd_rwlock_rlock(&sx->lock_object, &sx->rwlock);
	return (0);
}

int
sx_try_slock_(struct sx *sx, const char *file, int line)
{

	return (rtems_bsd_rwlock_try_rlock(&sx->lock_object, &sx->rwlock));
}

void
_sx_sunlock(struct sx *sx, const char *file, int line)
{

	rtems_bsd_rwlock_runlock(&sx->rwlock);
}

#ifdef INVARIANT_SUPPORT
/*
 * In the non-WITNESS case, sx_assert() can only detect that at least
 * *some* thread owns an slock, but it cannot guarantee that *this*
 * thread owns an slock.
 */
void
_sx_assert(const struct sx *sx, int what, const char *file, int line)
{
	const char *name = rtems_bsd_rwlock_name(&sx->rwlock);
	int slocked = 0;

	switch (what) {
	case SA_SLOCKED:
	case SA_SLOCKED | SA_NOTRECURSED:
	case SA_SLOCKED | SA_RECURSED:
		slocked = 1;
		/* FALLTHROUGH */
	case SA_LOCKED:
	case SA_LOCKED | SA_NOTRECURSED:
	case SA_LOCKED | SA_RECURSED:
		/*
		 * If some other thread has an exclusive lock or we
		 * have one and are asserting a shared lock, fail.
		 * Also, if no one has a lock at all, fail.
		 */
		if ((sx->rwlock.readers == 0 && sx_xholder(sx) == NULL) ||
		    (sx->rwlock.readers == 0 && (slocked ||
		    sx_xholder(sx) != _Thread_Get_executing())))
			panic("Lock %s not %slocked @ %s:%d\n",
			    name, slocked ? "share " : "",
			    file, line);

		if (sx->rwlock.readers == 0) {
			if (sx_recursed(sx)) {
				if (what & SA_NOTRECURSED)
					panic("Lock %s recursed @ %s:%d\n",
					    name, file,
					    line);
			} else if (what & SA_RECURSED)
				panic("Lock %s not recursed @ %s:%d\n",
				    name, file, line);
		}
		break;
	case SA_XLOCKED:
	case SA_XLOCKED | SA_NOTRECURSED:
	case SA_XLOCKED | SA_RECURSED:
		if (sx_xholder(sx) != _Thread_Get_executing())
			panic("Lock %s not exclusively locked @ %s:%d\n",
			    name, file, line);
		if (sx_recursed(sx)) {
			if (what & SA_NOTRECURSED)
				panic("Lock %s recursed @ %s:%d\n",
				    name, file, line);
		} else if (what & SA_RECURSED)
			panic("Lock %s not recursed @ %s:%d\n",
			    name, file, line);
		break;
	case SA_UNLOCKED:
		/*
		 * If we hold an exclusve lock fail.  We can't
		 * reliably check to see if we hold a shared lock or
		 * not.
		 */
		if (sx_xholder(sx) == _Thread_Get_executing())
			panic("Lock %s exclusively locked @ %s:%d\n",
			    name, file, line);
		break;
	default:
		panic("Unknown sx lock assertion: %d @ %s:%d", what, file,
		    line);
	}
}
#endif  /* INVARIANT_SUPPORT */

int
sx_xlocked(struct sx *sx)
{

	return (rtems_bsd_rwlock_wowned(&sx->rwlock));
}
