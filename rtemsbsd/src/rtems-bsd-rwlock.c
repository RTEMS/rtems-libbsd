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
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

/* Necessary to obtain some internal functions */
#define __RTEMS_VIOLATE_KERNEL_VISIBILITY__

#include <freebsd/machine/rtems-bsd-config.h>

#include <sys/types.h>
#include <freebsd/sys/param.h>
#include <freebsd/sys/types.h>
#include <freebsd/sys/systm.h>
#include <freebsd/sys/lock.h>
#include <freebsd/sys/rwlock.h>
#include <pthread.h>

#ifndef INVARIANTS
#define _rw_assert(rw, what, file, line)
#endif

static void assert_rw(struct lock_object *lock, int what);
static void lock_rw(struct lock_object *lock, int how);
#ifdef KDTRACE_HOOKS
static int  owner_rw(struct lock_object *lock, struct thread **owner);
#endif
static int  unlock_rw(struct lock_object *lock);

typedef uint32_t pthread_rwlock_t;

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
  pthread_rwlock_t lock;
  int iret;

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

  iret = pthread_rwlock_init( &lock, NULL );
  BSD_ASSERT( iret == 0 );

  rw->lock_object.lo_name = name;
  rw->lock_object.lo_flags |= LO_INITIALIZED;
  rw->lock_object.lo_id = lock;

  rtems_chain_append(&rtems_bsd_rwlock_chain, &rw->lock_object.lo_node);
}

void
rw_destroy(struct rwlock *rw)
{
  int iret;
  pthread_rwlock_destroy( rw->lock_object.lo_id );
  BSD_ASSERT( iret == 0 );
  rtems_chain_extract( &rw->lock_object.lo_node );
  rw->lock_object.lo_id = 0;
  rw->lock_object.lo_flags &= ~LO_INITIALIZED;
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
  Objects_Locations location;
  Semaphore_Control *sema = _Semaphore_Get(rw->lock_object.lo_id, &location);

  if (location == OBJECTS_LOCAL && !_Attributes_Is_counting_semaphore(sema->attribute_set)) {
    int owned = sema->Core_control.mutex.holder_id == rtems_task_self();

    _Thread_Enable_dispatch();

    return owned;
  } else {
    _Thread_Enable_dispatch();

    BSD_PANIC("unexpected semaphore location or attributes");
  }
}

void
_rw_wlock(struct rwlock *rw, const char *file, int line)
{
  int iret;

  pthread_rwlock_wrlock( &rw->lock_object.lo_id );
  BSD_ASSERT( iret == 0 );

  return 0;
}

int
_rw_try_wlock(struct rwlock *rw, const char *file, int line)
{
  int iret;

  iret = pthread_rwlock_trywrlock( &rw->lock_object.lo_id );
  if (iret == 0) {
    return 1;
  } else {
    return 0;
  }
}

void
_rw_wunlock(struct rwlock *rw, const char *file, int line)
{
  int iret;

  iret = pthread_rwlock_unlock( &rw->lock_object.lo_id );
  BSD_ASSERT( iret == 0 );
}

void
_rw_rlock(struct rwlock *rw, const char *file, int line)
{
  int iret;

  iret = pthread_rwlock_rdlock( &rw->lock_object.lo_id );
  BSD_ASSERT( iret == 0 );
}

int
_rw_try_rlock(struct rwlock *rw, const char *file, int line)
{
  int iret;

  iret = pthread_rwlock_tryrdlock( &rw->lock_object.lo_id );
  if (iret == 0) {
    return 1;
  } else {
    return 0;
  }
}

void
_rw_runlock(struct rwlock *rw, const char *file, int line)
{
  int iret;

  iret = pthread_rwlock_unlock( &rw->lock_object.lo_id );
  BSD_ASSERT( iret == 0 );
}

/*
 * Attempt to do a non-blocking upgrade from a read lock to a write
 * lock.  This will only succeed if this thread holds a single read
 * lock.  Returns true if the upgrade succeeded and false otherwise.
 */
int
_rw_try_upgrade(struct rwlock *rw, const char *file, int line)
{
  return 0; /* XXX */
}

/*
 * Downgrade a write lock into a single read lock.
 */
void
_rw_downgrade(struct rwlock *rw, const char *file, int line)
{
  /* XXX */ 
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
