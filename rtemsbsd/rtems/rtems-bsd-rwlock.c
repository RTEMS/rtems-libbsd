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
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
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
#include <machine/rtems-bsd-thread.h>
#include <machine/rtems-bsd-support.h>

#include <rtems.h>

#include <rtems/bsd/sys/param.h>
#include <rtems/bsd/sys/types.h>
#include <sys/systm.h>
#include <rtems/bsd/sys/lock.h>
#include <sys/rwlock.h>
#include <sys/mutex.h>

#ifndef INVARIANTS
#define _rw_assert(rw, what, file, line)
#endif

static void assert_rw(struct lock_object *lock, int what);
static void lock_rw(struct lock_object *lock, int how);
#ifdef KDTRACE_HOOKS
static int  owner_rw(struct lock_object *lock, struct thread **owner);
#endif
static int  unlock_rw(struct lock_object *lock);

struct lock_class lock_class_rw = {
  .lc_name = "rw",
  .lc_flags = LC_SLEEPLOCK | LC_RECURSABLE | LC_UPGRADABLE,
  .lc_assert = assert_rw,
#ifdef DDB
  .lc_ddb_show = db_show_rwlock,
#endif
  .lc_lock = lock_rw,
  .lc_unlock = unlock_rw,
#ifdef KDTRACE_HOOKS
  .lc_owner = owner_rw,
#endif
};

RTEMS_CHAIN_DEFINE_EMPTY(rtems_bsd_rwlock_chain);

void
assert_rw(struct lock_object *lock, int what)
{
  rw_assert((struct rwlock *)lock, what);
}

void
lock_rw(struct lock_object *lock, int how)
{
  struct rwlock *rw;

  rw = (struct rwlock *)lock;
  if (how)
    rw_wlock(rw);
  else
    rw_rlock(rw);
}

int
unlock_rw(struct lock_object *lock)
{
  struct rwlock *rw;

  rw = (struct rwlock *)lock;
  rw_assert(rw, RA_LOCKED | LA_NOTRECURSED);
  if (rw->rw_lock & RW_LOCK_READ) {
    rw_runlock(rw);
    return (0);
  } else {
    rw_wunlock(rw);
    return (1);
  }
}

#ifdef KDTRACE_HOOKS
int
owner_rw(struct lock_object *lock, struct thread **owner)
{
  struct rwlock *rw = (struct rwlock *)lock;
  uintptr_t x = rw->rw_lock;

  *owner = rw_wowner(rw);
  return ((x & RW_LOCK_READ) != 0 ?  (RW_READERS(x) != 0) :
      (*owner != NULL));
}
#endif

void
rw_init_flags(struct rwlock *rw, const char *name, int opts)
{
  struct lock_class *class;
  int i;
  rtems_status_code sc;
  rtems_id id;
  rtems_attribute attr = RTEMS_LOCAL | RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY | RTEMS_INHERIT_PRIORITY;

  if ((opts & RW_RECURSE) != 0) {
    /* FIXME */
  }

  class = &lock_class_rw;

  /* Check for double-init and zero object. */
  KASSERT(!lock_initalized(&rw->lock_object), ("lock \"%s\" %p already initialized", name, rw->lock_object));

  /* Look up lock class to find its index. */
  for (i = 0; i < LOCK_CLASS_MAX; i++)
  {
    if (lock_classes[i] == class)
    {
      rw->lock_object.lo_flags = i << LO_CLASSSHIFT;
      break;
    }
  }
  KASSERT(i < LOCK_CLASS_MAX, ("unknown lock class %p", class));

  sc = rtems_semaphore_create(
    rtems_build_name('_', '_', 'R', 'W'),
    1,
    attr,
    BSD_TASK_PRIORITY_RESOURCE_OWNER,
    &id
  );
  BSD_ASSERT_SC(sc);

  rw->lock_object.lo_name = name;
  rw->lock_object.lo_flags |= LO_INITIALIZED;
  rw->lock_object.lo_id = id;

  rtems_chain_append(&rtems_bsd_rwlock_chain, &rw->lock_object.lo_node);
}

void
rw_destroy(struct rwlock *rw)
{
  mtx_destroy((struct mtx *) rw);
}

void
rw_sysinit(void *arg)
{
  struct rw_args *args = arg;

  rw_init(args->ra_rw, args->ra_desc);
}

void
rw_sysinit_flags(void *arg)
{
  struct rw_args_flags *args = arg;

  rw_init_flags(args->ra_rw, args->ra_desc, args->ra_flags);
}

int
rw_wowned(struct rwlock *rw)
{
  return mtx_owned((struct mtx *) rw);
}

void
_rw_wlock(struct rwlock *rw, const char *file, int line)
{
  _mtx_lock_flags((struct mtx *) rw, 0, file, line);
}

int
_rw_try_wlock(struct rwlock *rw, const char *file, int line)
{
  return _mtx_trylock((struct mtx *) rw, 0, file, line);
}

void
_rw_wunlock(struct rwlock *rw, const char *file, int line)
{
  _mtx_unlock_flags((struct mtx *) rw, 0, file, line);
}

void
_rw_rlock(struct rwlock *rw, const char *file, int line)
{
  _mtx_lock_flags((struct mtx *) rw, 0, file, line);
}

int
_rw_try_rlock(struct rwlock *rw, const char *file, int line)
{
  return _mtx_trylock((struct mtx *) rw, 0, file, line);
}

void
_rw_runlock(struct rwlock *rw, const char *file, int line)
{
  _mtx_unlock_flags((struct mtx *) rw, 0, file, line);
}

int
_rw_try_upgrade(struct rwlock *rw, const char *file, int line)
{
  return 1;
}

void
_rw_downgrade(struct rwlock *rw, const char *file, int line)
{
  /* Nothing to do */
}

#ifdef INVARIANT_SUPPORT
#ifndef INVARIANTS
#undef _rw_assert
#endif

/*
 * In the non-WITNESS case, rw_assert() can only detect that at least
 * *some* thread owns an rlock, but it cannot guarantee that *this*
 * thread owns an rlock.
 */
void
_rw_assert(struct rwlock *rw, int what, const char *file, int line)
{

  if (panicstr != NULL)
    return;
  switch (what) {
  case RA_LOCKED:
  case RA_LOCKED | RA_RECURSED:
  case RA_LOCKED | RA_NOTRECURSED:
  case RA_RLOCKED:
#ifdef WITNESS
    witness_assert(&rw->lock_object, what, file, line);
#else
    /*
     * If some other thread has a write lock or we have one
     * and are asserting a read lock, fail.  Also, if no one
     * has a lock at all, fail.
     */
    if (rw->rw_lock == RW_UNLOCKED ||
        (!(rw->rw_lock & RW_LOCK_READ) && (what == RA_RLOCKED ||
        rw_wowner(rw) != curthread)))
      panic("Lock %s not %slocked @ %s:%d\n",
          rw->lock_object.lo_name, (what == RA_RLOCKED) ?
          "read " : "", file, line);

    if (!(rw->rw_lock & RW_LOCK_READ)) {
      if (rw_recursed(rw)) {
        if (what & RA_NOTRECURSED)
          panic("Lock %s recursed @ %s:%d\n",
              rw->lock_object.lo_name, file,
              line);
      } else if (what & RA_RECURSED)
        panic("Lock %s not recursed @ %s:%d\n",
            rw->lock_object.lo_name, file, line);
    }
#endif
    break;
  case RA_WLOCKED:
  case RA_WLOCKED | RA_RECURSED:
  case RA_WLOCKED | RA_NOTRECURSED:
    if (rw_wowner(rw) != curthread)
      panic("Lock %s not exclusively locked @ %s:%d\n",
          rw->lock_object.lo_name, file, line);
    if (rw_recursed(rw)) {
      if (what & RA_NOTRECURSED)
        panic("Lock %s recursed @ %s:%d\n",
            rw->lock_object.lo_name, file, line);
    } else if (what & RA_RECURSED)
      panic("Lock %s not recursed @ %s:%d\n",
          rw->lock_object.lo_name, file, line);
    break;
  case RA_UNLOCKED:
#ifdef WITNESS
    witness_assert(&rw->lock_object, what, file, line);
#else
    /*
     * If we hold a write lock fail.  We can't reliably check
     * to see if we hold a read lock or not.
     */
    if (rw_wowner(rw) == curthread)
      panic("Lock %s exclusively locked @ %s:%d\n",
          rw->lock_object.lo_name, file, line);
#endif
    break;
  default:
    panic("Unknown rw lock assertion: %d @ %s:%d", what, file,
        line);
  }
}
#endif /* INVARIANT_SUPPORT */
