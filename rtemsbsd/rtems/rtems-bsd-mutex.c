/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief TODO.
 */

/*
 * Copyright (c) 2009-2013 embedded brains GmbH.  All rights reserved.
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

#include <machine/rtems-bsd-config.h>

#include <rtems/score/objectimpl.h>
#include <rtems/rtems/attrimpl.h>
#include <rtems/rtems/semimpl.h>

#include <rtems/bsd/sys/param.h>
#include <rtems/bsd/sys/types.h>
#include <sys/systm.h>
#include <rtems/bsd/sys/lock.h>
#include <sys/mutex.h>
#include <sys/proc.h>

static void assert_mtx(struct lock_object *lock, int what);
static void lock_mtx(struct lock_object *lock, int how);
static void lock_spin(struct lock_object *lock, int how);
#ifdef KDTRACE_HOOKS
static int  owner_mtx(struct lock_object *lock, struct thread **owner);
#endif
static int  unlock_mtx(struct lock_object *lock);
static int  unlock_spin(struct lock_object *lock);

RTEMS_CHAIN_DEFINE_EMPTY(rtems_bsd_mtx_chain);

/*
 * Lock classes for sleep and spin mutexes.
 */
struct lock_class lock_class_mtx_sleep = {
  .lc_name = "sleep mutex",
  .lc_flags = LC_SLEEPLOCK | LC_RECURSABLE,
  .lc_assert = assert_mtx,
#ifdef DDB
  .lc_ddb_show = db_show_mtx,
#endif
  .lc_lock = lock_mtx,
  .lc_unlock = unlock_mtx,
#ifdef KDTRACE_HOOKS
  .lc_owner = owner_mtx,
#endif
};

struct lock_class lock_class_mtx_spin = {
  .lc_name = "spin mutex",
  .lc_flags = LC_SPINLOCK | LC_RECURSABLE,
  .lc_assert = assert_mtx,
#ifdef DDB
  .lc_ddb_show = db_show_mtx,
#endif
  .lc_lock = lock_spin,
  .lc_unlock = unlock_spin,
#ifdef KDTRACE_HOOKS
  .lc_owner = owner_mtx,
#endif
};

struct mtx Giant;

void
assert_mtx(struct lock_object *lock, int what)
{
  mtx_assert((struct mtx *)lock, what);
}

void
lock_mtx(struct lock_object *lock, int how)
{

  mtx_lock((struct mtx *)lock);
}

void
lock_spin(struct lock_object *lock, int how)
{

  panic("spin locks can only use msleep_spin");
}

int
unlock_mtx(struct lock_object *lock)
{
  struct mtx *m;

  m = (struct mtx *)lock;
  mtx_assert(m, MA_OWNED | MA_NOTRECURSED);
  mtx_unlock(m);
  return (0);
}

int
unlock_spin(struct lock_object *lock)
{

  panic("spin locks can only use msleep_spin");
}

#ifdef KDTRACE_HOOKS
int
owner_mtx(struct lock_object *lock, struct thread **owner)
{
  struct mtx *m = (struct mtx *)lock;

  *owner = mtx_owner(m);
  return (mtx_unowned(m) == 0);
}
#endif

void
mtx_init(struct mtx *m, const char *name, const char *type, int opts)
{
  struct lock_class *class;
  int i;
	rtems_status_code sc = RTEMS_SUCCESSFUL;
	rtems_id id = RTEMS_ID_NONE;
	/* rtems_attribute attr = RTEMS_LOCAL | RTEMS_PRIORITY | RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY_CEILING; */
  rtems_attribute attr = RTEMS_LOCAL | RTEMS_PRIORITY | RTEMS_BINARY_SEMAPHORE;

  if ((opts & MTX_RECURSE) != 0 )
  {
    /*FIXME*/
  }

  /* Determine lock class and lock flags. */
  if (opts & MTX_SPIN)
    class = &lock_class_mtx_spin;
  else
    class = &lock_class_mtx_sleep;

  /* Check for double-init and zero object. */
  KASSERT(!lock_initalized(&m->lock_object), ("lock \"%s\" %p already initialized", name, m->lock_object));

  /* Look up lock class to find its index. */
  for (i = 0; i < LOCK_CLASS_MAX; i++)
  {
    if (lock_classes[i] == class)
    {
      m->lock_object.lo_flags = i << LO_CLASSSHIFT;
      break;
    }
  }
  KASSERT(i < LOCK_CLASS_MAX, ("unknown lock class %p", class));

	sc = rtems_semaphore_create(
		rtems_build_name('_', 'M', 'T', 'X'),
		1,
		attr,
		BSD_TASK_PRIORITY_RESOURCE_OWNER,
		&id
	);
	BSD_ASSERT_SC(sc);

	m->lock_object.lo_name = name;
  m->lock_object.lo_flags |= LO_INITIALIZED;
  m->lock_object.lo_id = id;

	rtems_chain_append(&rtems_bsd_mtx_chain, &m->lock_object.lo_node);
}

void
_mtx_lock_flags(struct mtx *m, int opts, const char *file, int line)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;

	sc = rtems_semaphore_obtain(m->lock_object.lo_id, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
	BSD_ASSERT_SC(sc);
}

int
_mtx_trylock(struct mtx *m, int opts, const char *file, int line)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;

	sc = rtems_semaphore_obtain(m->lock_object.lo_id, RTEMS_NO_WAIT, 0);
	if (sc == RTEMS_SUCCESSFUL) {
		return 1;
	} else if (sc == RTEMS_UNSATISFIED) {
		return 0;
	} else {
		BSD_ASSERT_SC(sc);

		return 0;
	}
}

void
_mtx_unlock_flags(struct mtx *m, int opts, const char *file, int line)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;

	sc = rtems_semaphore_release(m->lock_object.lo_id);
	BSD_ASSERT_SC(sc);
}

/*
 * The backing function for the INVARIANTS-enabled mtx_assert()
 */
#ifdef INVARIANT_SUPPORT
void
_mtx_assert(struct mtx *m, int what, const char *file, int line)
{

  if (panicstr != NULL || dumping)
    return;
  switch (what) {
  case MA_OWNED:
  case MA_OWNED | MA_RECURSED:
  case MA_OWNED | MA_NOTRECURSED:
    if (!mtx_owned(m))
      panic("mutex %s not owned at %s:%d",
          m->lock_object.lo_name, file, line);
    if (mtx_recursed(m)) {
      if ((what & MA_NOTRECURSED) != 0)
        panic("mutex %s recursed at %s:%d",
            m->lock_object.lo_name, file, line);
    } else if ((what & MA_RECURSED) != 0) {
      panic("mutex %s unrecursed at %s:%d",
          m->lock_object.lo_name, file, line);
    }
    break;
  case MA_NOTOWNED:
    if (mtx_owned(m))
      panic("mutex %s owned at %s:%d",
          m->lock_object.lo_name, file, line);
    break;
  default:
    panic("unknown mtx_assert at %s:%d", file, line);
  }
}
#endif

int mtx_owned(struct mtx *m)
{
	Objects_Locations location;
	Semaphore_Control *sema = _Semaphore_Get(m->lock_object.lo_id, &location);

	if (location == OBJECTS_LOCAL && !_Attributes_Is_counting_semaphore(sema->attribute_set)) {
		int owned = sema->Core_control.mutex.holder_id == rtems_task_self();

		_Thread_Enable_dispatch();

		return owned;
	} else {
		_Thread_Enable_dispatch();

		BSD_PANIC("unexpected semaphore location or attributes");
	}
}

int mtx_recursed(struct mtx *m)
{
	Objects_Locations location;
	Semaphore_Control *sema = _Semaphore_Get(m->lock_object.lo_id, &location);

	if (location == OBJECTS_LOCAL && !_Attributes_Is_counting_semaphore(sema->attribute_set)) {
		int recursed = sema->Core_control.mutex.nest_count != 0;

		_Thread_Enable_dispatch();

		return recursed;
	} else {
		_Thread_Enable_dispatch();

		BSD_PANIC("unexpected semaphore location or attributes");
	}
}

void
mtx_sysinit(void *arg)
{
	struct mtx_args *margs = arg;

	mtx_init(margs->ma_mtx, margs->ma_desc, NULL, margs->ma_opts);
}

void
mtx_destroy(struct mtx *m)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;

	sc = rtems_semaphore_delete(m->lock_object.lo_id);
	BSD_ASSERT_SC(sc);

	rtems_chain_extract(&m->lock_object.lo_node);

  m->lock_object.lo_id = 0;
  m->lock_object.lo_flags &= ~LO_INITIALIZED;
}

void
mutex_init(void)
{
	mtx_init(&Giant, "Giant", NULL, MTX_DEF | MTX_RECURSE);
}
