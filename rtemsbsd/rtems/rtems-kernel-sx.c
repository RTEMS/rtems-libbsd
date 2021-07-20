/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief TODO.
 */

/*
 * Copyright (c) 2009, 2018 embedded brains GmbH.  All rights reserved.
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
#include <machine/rtems-bsd-muteximpl.h>
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

#define	sx_xholder(sx) rtems_bsd_mutex_owner(&(sx)->lock_object)

#define	sx_recursed(sx) rtems_bsd_mutex_recursed(&(sx)->lock_object)

void
assert_sx(const struct lock_object *lock, int what)
{

	sx_assert((const struct sx *)lock, what);
}

void
lock_sx(struct lock_object *lock, uintptr_t how)
{

	sx_xlock((struct sx *)lock);
}

uintptr_t
unlock_sx(struct lock_object *lock)
{

	sx_xunlock((struct sx *)lock);
	return (0);
}

void
sx_sysinit(void *arg)
{
	struct sx_args *sargs = arg;

	sx_init_flags(sargs->sa_sx, sargs->sa_desc, sargs->sa_flags);
}

void
sx_init_flags(struct sx *sx, const char *description, int opts)
{
	int flags;

	flags = LO_SLEEPABLE | LO_UPGRADABLE;
	if (opts & SX_RECURSE)
		flags |= LO_RECURSABLE;

	rtems_bsd_mutex_init(&sx->lock_object, &lock_class_sx,
	    description, NULL, flags);
}

void
sx_destroy(struct sx *sx)
{

	rtems_bsd_mutex_destroy(&sx->lock_object);
}

int
_sx_slock_int(struct sx *sx, int opts LOCK_FILE_LINE_ARG_DEF)
{

	rtems_bsd_mutex_lock(&sx->lock_object);
	return (0);
}

int
sx_try_slock_int(struct sx *sx LOCK_FILE_LINE_ARG_DEF)
{

	return (rtems_bsd_mutex_trylock(&sx->lock_object));
}

void
_sx_sunlock_int(struct sx *sx LOCK_FILE_LINE_ARG_DEF)
{

	rtems_bsd_mutex_unlock(&sx->lock_object);
}

int
sx_try_upgrade_int(struct sx *sx LOCK_FILE_LINE_ARG_DEF)
{

	return (1);
}

int
sx_try_upgrade_(struct sx *sx, const char *file, int line)
{

	return (1);
}

void
sx_downgrade_int(struct sx *sx LOCK_FILE_LINE_ARG_DEF)
{

	/* Do nothing */
}

int
sx_try_downgrade_(struct sx *sx, const char *file, int line)
{

	return (1);
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
	const char *name = rtems_bsd_mutex_name(&sx->lock_object);

	switch (what) {
	case SA_SLOCKED:
	case SA_SLOCKED | SA_NOTRECURSED:
	case SA_SLOCKED | SA_RECURSED:
	case SA_LOCKED:
	case SA_LOCKED | SA_NOTRECURSED:
	case SA_LOCKED | SA_RECURSED:
	case SA_XLOCKED:
	case SA_XLOCKED | SA_NOTRECURSED:
	case SA_XLOCKED | SA_RECURSED:
		if (sx_xholder(sx) != _Thread_Get_executing())
			panic("Lock %s not exclusively locked @ %s:%d\n", name,
			    file, line);
		if (sx_recursed(sx)) {
			if (what & SA_NOTRECURSED)
				panic("Lock %s recursed @ %s:%d\n", name, file,
				    line);
		} else if (what & SA_RECURSED)
			panic("Lock %s not recursed @ %s:%d\n", name, file,
			    line);
		break;
	case SA_UNLOCKED:
#ifdef WITNESS
		witness_assert(&sx->lock_object, what, file, line);
#else
		/*
		 * If we hold an exclusve lock fail.  We can't
		 * reliably check to see if we hold a shared lock or
		 * not.
		 */
		if (sx_xholder(sx) == _Thread_Get_executing())
			panic("Lock %s exclusively locked @ %s:%d\n", name,
			    file, line);
#endif
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
	return (rtems_bsd_mutex_owned(&sx->lock_object));
}
