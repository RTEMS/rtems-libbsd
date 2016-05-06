/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief TODO.
 */

/*
 * Copyright (c) 2009-2015 embedded brains GmbH.  All rights reserved.
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

#include <rtems/bsd/sys/param.h>
#include <rtems/bsd/sys/types.h>
#include <sys/systm.h>
#include <rtems/bsd/sys/lock.h>
#include <sys/sx.h>

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

#define sx_xholder(sx) ((sx)->mutex.owner)

#define sx_recursed(sx) ((sx)->mutex.nest_level != 0)

void
assert_sx(struct lock_object *lock, int what)
{
  sx_assert((struct sx *)lock, what);
}

void
lock_sx(struct lock_object *lock, int how)
{
	sx_xlock((struct sx *)lock);
}

int
unlock_sx(struct lock_object *lock)
{
	sx_xunlock((struct sx *)lock);

	return (0);
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
	int flags;

	flags = LO_SLEEPABLE | LO_UPGRADABLE;
	if (opts & SX_RECURSE)
		flags |= LO_RECURSABLE;

	rtems_bsd_mutex_init(&sx->lock_object, &sx->mutex, &lock_class_sx,
	    description, NULL, flags);
}

void
sx_destroy(struct sx *sx)
{

	rtems_bsd_mutex_destroy(&sx->lock_object, &sx->mutex);
}

int
_sx_xlock(struct sx *sx, int opts, const char *file, int line)
{
	rtems_bsd_mutex_lock(&sx->lock_object, &sx->mutex);

	return (0);
}

int
_sx_try_xlock(struct sx *sx, const char *file, int line)
{
	return (rtems_bsd_mutex_trylock(&sx->lock_object, &sx->mutex));
}

void
_sx_xunlock(struct sx *sx, const char *file, int line)
{
	rtems_bsd_mutex_unlock(&sx->mutex);
}

int
_sx_try_upgrade(struct sx *sx, const char *file, int line)
{
	return (1);
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
#ifndef __rtems__
#ifndef WITNESS
  int slocked = 0;
#endif
#endif /* __rtems__ */

  if (panicstr != NULL)
    return;
  switch (what) {
  case SA_SLOCKED:
  case SA_SLOCKED | SA_NOTRECURSED:
  case SA_SLOCKED | SA_RECURSED:
#ifndef __rtems__
#ifndef WITNESS
    slocked = 1;
    /* FALLTHROUGH */
#endif
#endif /* __rtems__ */
  case SA_LOCKED:
  case SA_LOCKED | SA_NOTRECURSED:
  case SA_LOCKED | SA_RECURSED:
#ifndef __rtems__
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
#else /* __rtems__ */
    /* FALLTHROUGH */
#endif /* __rtems__ */
  case SA_XLOCKED:
  case SA_XLOCKED | SA_NOTRECURSED:
  case SA_XLOCKED | SA_RECURSED:
    if (sx_xholder(sx) != _Thread_Get_executing())
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
    if (sx_xholder(sx) == _Thread_Get_executing())
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
	return (rtems_bsd_mutex_owned(&sx->mutex));
}
