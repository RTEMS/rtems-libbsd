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

#include <rtems/freebsd/sys/param.h>
#include <rtems/freebsd/sys/types.h>
#include <rtems/freebsd/sys/systm.h>
#include <rtems/freebsd/sys/lock.h>
#include <rtems/freebsd/sys/sx.h>

#ifndef INVARIANTS
#define _sx_assert(sx, what, file, line)
#endif

static void assert_sx(struct lock_object *lock, int what);
static void lock_sx(struct lock_object *lock, int how);
#ifdef KDTRACE_HOOKS
static int  owner_sx(struct lock_object *lock, struct thread **owner);
#endif
static int  unlock_sx(struct lock_object *lock);

struct lock_class lock_class_sx = {
  .lc_name = "sx",
  .lc_flags = LC_SLEEPLOCK | LC_SLEEPABLE | LC_RECURSABLE | LC_UPGRADABLE,
  .lc_assert = assert_sx,
#ifdef DDB
  .lc_ddb_show = db_show_sx,
#endif
  .lc_lock = lock_sx,
  .lc_unlock = unlock_sx,
#ifdef KDTRACE_HOOKS
  .lc_owner = owner_sx,
#endif
};

RTEMS_CHAIN_DEFINE_EMPTY(rtems_bsd_sx_chain);

void
assert_sx(struct lock_object *lock, int what)
{
  sx_assert((struct sx *)lock, what);
}

void
lock_sx(struct lock_object *lock, int how)
{
  struct sx *sx;

  sx = (struct sx *)lock;
  if (how)
    sx_xlock(sx);
  else
    sx_slock(sx);
}

int
unlock_sx(struct lock_object *lock)
{
  struct sx *sx;

  sx = (struct sx *)lock;
  sx_assert(sx, SA_LOCKED | SA_NOTRECURSED);
  if (sx_xlocked(sx)) {
    sx_xunlock(sx);
    return (1);
  } else {
    sx_sunlock(sx);
    return (0);
  }
}

#ifdef KDTRACE_HOOKS
int
owner_sx(struct lock_object *lock, struct thread **owner)
{
        struct sx *sx = (struct sx *)lock;
  uintptr_t x = sx->sx_lock;

        *owner = (struct thread *)SX_OWNER(x);
        return ((x & SX_LOCK_SHARED) != 0 ? (SX_SHARERS(x) != 0) :
      (*owner != NULL));
}
#endif

void
sx_sysinit(void *arg)
{
	struct sx_args *sargs = arg;

	sx_init(sargs->sa_sx, sargs->sa_desc);
}

void
sx_init_flags(struct sx *sx, const char *description, int opts)
{
  struct lock_class *class;
  int i;
	rtems_status_code sc = RTEMS_SUCCESSFUL;
	rtems_id id = RTEMS_ID_NONE;
	rtems_attribute attr = RTEMS_LOCAL | RTEMS_PRIORITY | RTEMS_BINARY_SEMAPHORE;

	if ((opts & SX_RECURSE) != 0) {
		/* FIXME */
	}

  class = &lock_class_sx;

  /* Check for double-init and zero object. */
  KASSERT(!lock_initalized(&sx->lock_object), ("lock \"%s\" %p already initialized", name, sx->lock_object));

  /* Look up lock class to find its index. */
  for (i = 0; i < LOCK_CLASS_MAX; i++)
  {
    if (lock_classes[i] == class)
    {
      sx->lock_object.lo_flags = i << LO_CLASSSHIFT;
      break;
    }
  }
  KASSERT(i < LOCK_CLASS_MAX, ("unknown lock class %p", class));

	sc = rtems_semaphore_create(
		rtems_build_name( '_', 'S', 'X', ' '),
		1,
		attr,
		0,
		&id
	);
	BSD_ASSERT_SC(sc);

	sx->lock_object.lo_name = description;
  sx->lock_object.lo_flags |= LO_INITIALIZED;
  sx->lock_object.lo_id = id;

	rtems_chain_append(&rtems_bsd_sx_chain, &sx->lock_object.lo_node);
}

void
sx_destroy(struct sx *sx)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;

	sc = rtems_semaphore_delete( sx->lock_object.lo_id);
	BSD_ASSERT_SC(sc);

	rtems_chain_extract(&sx->lock_object.lo_node);

  sx->lock_object.lo_id = 0;
  sx->lock_object.lo_flags &= ~LO_INITIALIZED;
}

int
_sx_xlock(struct sx *sx, int opts, const char *file, int line)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;

	BSD_ASSERT((opts & SX_INTERRUPTIBLE) == 0);

	sc = rtems_semaphore_obtain( sx->lock_object.lo_id, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
	BSD_ASSERT_SC(sc);

	return 0;
}

int
_sx_try_xlock(struct sx *sx, const char *file, int line)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;

	sc = rtems_semaphore_obtain( sx->lock_object.lo_id, RTEMS_NO_WAIT, 0);
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
_sx_xunlock(struct sx *sx, const char *file, int line)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;

	sc = rtems_semaphore_release( sx->lock_object.lo_id);
	BSD_ASSERT_SC(sc);
}

int
_sx_try_upgrade(struct sx *sx, const char *file, int line)
{
	return 1;
}

void
_sx_downgrade(struct sx *sx, const char *file, int line)
{
	/* Do nothing */
}

#ifdef INVARIANT_SUPPORT
#ifndef INVARIANTS
#undef  _sx_assert
#endif

/*
 * In the non-WITNESS case, sx_assert() can only detect that at least
 * *some* thread owns an slock, but it cannot guarantee that *this*
 * thread owns an slock.
 */
void
_sx_assert(struct sx *sx, int what, const char *file, int line)
{
#ifndef WITNESS
  int slocked = 0;
#endif

  if (panicstr != NULL)
    return;
  switch (what) {
  case SA_SLOCKED:
  case SA_SLOCKED | SA_NOTRECURSED:
  case SA_SLOCKED | SA_RECURSED:
#ifndef WITNESS
    slocked = 1;
    /* FALLTHROUGH */
#endif
  case SA_LOCKED:
  case SA_LOCKED | SA_NOTRECURSED:
  case SA_LOCKED | SA_RECURSED:
#ifdef WITNESS
    witness_assert(&sx->lock_object, what, file, line);
#else
    /*
     * If some other thread has an exclusive lock or we
     * have one and are asserting a shared lock, fail.
     * Also, if no one has a lock at all, fail.
     */
    if (sx->sx_lock == SX_LOCK_UNLOCKED ||
        (!(sx->sx_lock & SX_LOCK_SHARED) && (slocked ||
        sx_xholder(sx) != curthread)))
      panic("Lock %s not %slocked @ %s:%d\n",
          sx->lock_object.lo_name, slocked ? "share " : "",
          file, line);

    if (!(sx->sx_lock & SX_LOCK_SHARED)) {
      if (sx_recursed(sx)) {
        if (what & SA_NOTRECURSED)
          panic("Lock %s recursed @ %s:%d\n",
              sx->lock_object.lo_name, file,
              line);
      } else if (what & SA_RECURSED)
        panic("Lock %s not recursed @ %s:%d\n",
            sx->lock_object.lo_name, file, line);
    }
#endif
    break;
  case SA_XLOCKED:
  case SA_XLOCKED | SA_NOTRECURSED:
  case SA_XLOCKED | SA_RECURSED:
    if (sx_xholder(sx) != curthread)
      panic("Lock %s not exclusively locked @ %s:%d\n",
          sx->lock_object.lo_name, file, line);
    if (sx_recursed(sx)) {
      if (what & SA_NOTRECURSED)
        panic("Lock %s recursed @ %s:%d\n",
            sx->lock_object.lo_name, file, line);
    } else if (what & SA_RECURSED)
      panic("Lock %s not recursed @ %s:%d\n",
          sx->lock_object.lo_name, file, line);
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
    if (sx_xholder(sx) == curthread)
      panic("Lock %s exclusively locked @ %s:%d\n",
          sx->lock_object.lo_name, file, line);
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
	Objects_Locations location;
	Semaphore_Control *sema = _Semaphore_Get(sx->lock_object.lo_id, &location);

	if (location == OBJECTS_LOCAL && !_Attributes_Is_counting_semaphore(sema->attribute_set)) {
		int xlocked = sema->Core_control.mutex.holder_id == rtems_task_self();

		_Thread_Enable_dispatch();

		return xlocked;
	} else {
		_Thread_Enable_dispatch();

		BSD_PANIC("unexpected semaphore location or attributes");
	}
}
