/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief TODO.
 */

/*
 *  COPYRIGHT (c) 2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems/freebsd/machine/rtems-bsd-config.h>

#include <sys/types.h>
#include <rtems/freebsd/sys/param.h>
#include <rtems/freebsd/sys/types.h>
#include <rtems/freebsd/sys/systm.h>
#include <rtems/freebsd/sys/lock.h>
#include <rtems/freebsd/sys/rmlock.h>
#include <pthread.h>

#define RMPF_ONQUEUE	1
#define RMPF_SIGNAL	2

/*
 * To support usage of rmlock in CVs and msleep yet another list for the
 * priority tracker would be needed.  Using this lock for cv and msleep also
 * does not seem very useful
 */

static __inline void compiler_memory_barrier(void) {
	__asm __volatile("":::"memory");
}

static void	assert_rm(struct lock_object *lock, int what);
static void	lock_rm(struct lock_object *lock, int how);
#ifdef KDTRACE_HOOKS
static int	owner_rm(struct lock_object *lock, struct thread **owner);
#endif
static int	unlock_rm(struct lock_object *lock);

struct lock_class lock_class_rm = {
	.lc_name = "rm",
	.lc_flags = LC_SLEEPLOCK | LC_RECURSABLE,
	.lc_assert = assert_rm,
#if 0
#ifdef DDB
	.lc_ddb_show = db_show_rwlock,
#endif
#endif
	.lc_lock = lock_rm,
	.lc_unlock = unlock_rm,
#ifdef KDTRACE_HOOKS
	.lc_owner = owner_rm,
#endif
};

static void
assert_rm(struct lock_object *lock, int what)
{

	panic("assert_rm called");
}

static void
lock_rm(struct lock_object *lock, int how)
{

	panic("lock_rm called");
}

static int
unlock_rm(struct lock_object *lock)
{

	panic("unlock_rm called");
}

#ifdef KDTRACE_HOOKS
static int
owner_rm(struct lock_object *lock, struct thread **owner)
{

	panic("owner_rm called");
}
#endif
