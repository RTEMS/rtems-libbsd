/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief TODO.
 */

/*
 * Copyright (c) 2020 Chris Johns.  All rights reserved.
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

#include <sys/types.h>
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/conf.h>
#include <sys/lock.h>
#include <sys/lockmgr.h>
#include <sys/mutex.h>
#include <sys/proc.h>

#ifdef DEBUG_LOCKS
#define STACK_PRINT(lk) printf("caller: %p\n", (lk)->lk_stack)
#define STACK_SAVE(lk) (lk)->lk_stack = __builtin_return_address(0)
#define STACK_ZERO(lk) (lk)->lk_stack = NULL
#else
#define STACK_PRINT(lk)
#define STACK_SAVE(lk)
#define STACK_ZERO(lk)
#endif

static void assert_lockmgr(const struct lock_object *lock, int how);
static void lock_lockmgr(struct lock_object *lock, uintptr_t how);
static uintptr_t unlock_lockmgr(struct lock_object *lock);

#define lockmgr_xlocked(lk)    \
	rtems_bsd_mutex_owned( \
	    RTEMS_DECONST(struct lock_object *, &lk->lock_object))
#define lockmgr_disowned(lk)    \
	!rtems_bsd_mutex_owned( \
	    RTEMS_DECONST(struct lock_object *, &lk->lock_object))

struct lock_class lock_class_lockmgr = {
	.lc_name = "lockmgr",
	.lc_flags = LC_RECURSABLE | LC_SLEEPABLE | LC_SLEEPLOCK | LC_UPGRADABLE,
	.lc_assert = assert_lockmgr,
#ifdef DDB
	.lc_ddb_show = db_show_lockmgr,
#endif
	.lc_lock = lock_lockmgr,
	.lc_unlock = unlock_lockmgr,
#ifdef KDTRACE_HOOKS
	.lc_owner = owner_lockmgr,
#endif
};

static void
assert_lockmgr(const struct lock_object *lock, int what)
{
	panic("lockmgr locks do not support assertions");
}

static void
lock_lockmgr(struct lock_object *lock, uintptr_t how)
{
	panic("lockmgr locks do not support sleep interlocking : lock");
}

static uintptr_t
unlock_lockmgr(struct lock_object *lock)
{
	panic("lockmgr locks do not support sleep interlocking: unlock");
}

static struct thread *
lockmgr_xholder(const struct lock *lk)
{
	uintptr_t x;
	x = lk->lk_lock;
	if ((x & LK_SHARE))
		return NULL;
	return rtems_bsd_get_thread(lk->lock_object.lo_mtx.queue.Queue.owner);
}

static void
lockmgr_exit(u_int flags, struct lock_object *ilk, int wakeup_swapper)
{
	if (flags & LK_INTERLOCK) {
		struct lock_class *class = LOCK_CLASS(ilk);
		class->lc_unlock(ilk);
	}
}

#ifdef RTEMS_BSD_LOCKMGR_TRACE
static void
lockmgr_trace(const char *label, const char dir, const struct lock *lk)
{
	printf("bsd: lck: %s %c lk=%p (%s) lk_lock=%08x rec=%d mtx=%c/%d\n",
	    label, dir, lk, lk->lock_object.lo_mtx.queue.Queue.name,
	    lk->lk_lock, lk->lk_recurse,
	    lk->lock_object.lo_mtx.queue.Queue.owner != NULL ? 'L' : 'U',
	    lk->lock_object.lo_mtx.nest_level);
}
#else
#define lockmgr_trace(lm, d, lk)
#endif

static int
lockmgr_slock_hard(struct lock *lk, u_int flags, struct lock_object *ilk,
    const char *file, int line)
{
	uintptr_t x;
	lockmgr_trace("slock", 'I', lk);
	rtems_bsd_mutex_lock(&lk->lock_object);
	x = lk->lk_lock;
	atomic_store_rel_ptr(&lk->lk_lock, x + LK_ONE_SHARER);
	lockmgr_trace("slock", 'O', lk);
	LOCK_LOG_LOCK("SLOCK", &lk->lock_object, 0, lk->lk_recurse, file, line);
	lockmgr_exit(flags, ilk, 0);
	return 0;
}

static int
lockmgr_xlock_hard(struct lock *lk, u_int flags, struct lock_object *ilk,
    const char *file, int line)
{
	int error = 0;
	lockmgr_trace("xlock", 'I', lk);
	if ((flags & LK_NOWAIT) != 0) {
		if (!rtems_bsd_mutex_trylock(&lk->lock_object))
			error = EBUSY;
	} else {
		rtems_bsd_mutex_lock(&lk->lock_object);
	}
	if (error == 0) {
		atomic_store_rel_ptr(&lk->lk_lock, 0);
		lockmgr_trace("xlock", 'O', lk);
	}
	lockmgr_exit(flags, ilk, 0);
	return error;
}

static int
lockmgr_upgrade(struct lock *lk, u_int flags, struct lock_object *ilk,
    const char *file, int line)
{
	uintptr_t x, v;
	int error = 0;
	/*
	 * RTEMS does not support shared locks so locking a shared
	 * lock is locking an exclusive lock. This logic is here to
	 * track the shared vs exclusive logic so the VFS does not get
	 * upset.
	 */
	LOCK_LOG_LOCK("XUPGRADE", &lk->lock_object, 0, 0, file, line);
	lockmgr_trace("xupgrade", 'I', lk);
	v = lk->lk_lock;
	x = v & ~LK_SHARE;
	atomic_store_rel_ptr(&lk->lk_lock, x);
	lockmgr_trace("xupgrade", 'O', lk);
	lockmgr_exit(flags, ilk, 0);
	return error;
}

static int
lockmgr_downgrade(struct lock *lk, u_int flags, struct lock_object *ilk,
    const char *file, int line)
{
	uintptr_t x;
	LOCK_LOG_LOCK("XDOWNGRADE", &lk->lock_object, 0, 0, file, line);
	lockmgr_trace("xdowngrade", 'I', lk);
	x = lk->lk_lock;
	x &= LK_ALL_WAITERS;
	atomic_store_rel_ptr(&lk->lk_lock, x);
	lockmgr_trace("xdowngrade", 'O', lk);
	return 0;
}

int
lockmgr_lock_fast_path(struct lock *lk, u_int flags, struct lock_object *ilk,
    const char *file, int line)
{
	uintptr_t x, tid;
	u_int op;
	bool locked;

	if (__predict_false(panicstr != NULL))
		return (0);

	op = flags & LK_TYPE_MASK;
	locked = false;
	switch (op) {
	case LK_SHARED:
		if (!__predict_false(lk->lock_object.lo_flags & LK_NOSHARE))
			return (lockmgr_slock_hard(lk, flags, ilk, file, line));
		/* fall through */
	case LK_EXCLUSIVE:
		return (lockmgr_xlock_hard(lk, flags, ilk, file, line));
		break;
	case LK_UPGRADE:
	case LK_TRYUPGRADE:
		return (lockmgr_upgrade(lk, flags, ilk, file, line));
		break;
	case LK_DOWNGRADE:
		return (lockmgr_downgrade(lk, flags, ilk, file, line));
		break;
	default:
		break;
	}

	panic("unsupported lockmgr op: %d\n", op);
}

static int
lockmgr_sunlock_hard(struct lock *lk, uintptr_t x, u_int flags,
    struct lock_object *ilk, const char *file, int line)
{
	uintptr_t v;
	int error = 0;
	LOCK_LOG_LOCK(
	    "SUNLOCK", &lk->lock_object, 0, lk->lk_recurse, file, line);
	lockmgr_trace("sunlock", 'I', lk);
	x = lk->lk_lock;
	if (x == LK_UNLOCKED)
		panic("sunlock not locked");
	atomic_store_rel_ptr(&lk->lk_lock, x - LK_ONE_SHARER);
	rtems_bsd_mutex_unlock(&lk->lock_object);
	lockmgr_trace("sunlock", 'O', lk);
	lockmgr_exit(flags, ilk, 0);
	return error;
}

static int
lockmgr_xunlock_hard(struct lock *lk, uintptr_t x, u_int flags,
    struct lock_object *ilk, const char *file, int line)
{
	int error = 0;
	LOCK_LOG_LOCK(
	    "XUNLOCK", &lk->lock_object, 0, lk->lk_recurse, file, line);
	lockmgr_trace("xunlock", 'I', lk);
	if (lk->lk_recurse == 0) {
		uintptr_t v, x;
		x = lk->lk_lock;
		if (x != 0)
			x -= LK_ONE_SHARER;
		v = x | LK_SHARE;
		atomic_store_rel_ptr(&lk->lk_lock, v);
	}
	rtems_bsd_mutex_unlock(&lk->lock_object);
	lockmgr_trace("xunlock", 'O', lk);
	lockmgr_exit(flags, ilk, 0);
	return error;
}

int
lockmgr_unlock_fast_path(struct lock *lk, u_int flags, struct lock_object *ilk)
{
	struct lock_class *class;
	uintptr_t x, tid;
	const char *file;
	int line;

	if (__predict_false(panicstr != NULL))
		return (0);

	file = __FILE__;
	line = __LINE__;

	x = lk->lk_lock;
	if (__predict_true(x & LK_SHARE) != 0) {
		return (lockmgr_sunlock_hard(lk, x, flags, ilk, file, line));
	} else {
		return (lockmgr_xunlock_hard(lk, x, flags, ilk, file, line));
	}
}

int
lockstatus(const struct lock *lk)
{
	uintptr_t v, x;
	int ret = 0;

	ret = LK_SHARED;
	x = lk->lk_lock;

	if ((x & LK_SHARE) == 0) {
		v = rtems_bsd_mutex_owned(
		    RTEMS_DECONST(struct lock_object *, &lk->lock_object));
		if (v)
			ret = LK_EXCLUSIVE;
		else
			ret = LK_EXCLOTHER;
	} else if (x == LK_UNLOCKED) {
		ret = 0;
	}

	lockmgr_trace("status", 'O', lk);

	return (ret);
}

void
lockallowrecurse(struct lock *lk)
{
	lk->lock_object.lo_flags |= LO_RECURSABLE;
}

void
lockallowshare(struct lock *lk)
{
	lk->lock_object.lo_flags &= ~LK_NOSHARE;
}

void
lockdisablerecurse(struct lock *lk)
{
	lk->lock_object.lo_flags &= ~LO_RECURSABLE;
}

void
lockdisableshare(struct lock *lk)
{
	lk->lock_object.lo_flags |= LK_NOSHARE;
}

void
lockinit(struct lock *lk, int pri, const char *wmesg, int timo, int flags)
{
	int iflags;

	MPASS((flags & ~LK_INIT_MASK) == 0);
	ASSERT_ATOMIC_LOAD_PTR(lk->lk_lock,
	    ("%s: lockmgr not aligned for %s: %p", __func__, wmesg,
		&lk->lk_lock));

	iflags = LO_SLEEPABLE | LO_UPGRADABLE;
	if (flags & LK_CANRECURSE)
		iflags |= LO_RECURSABLE;
	if ((flags & LK_NODUP) == 0)
		iflags |= LO_DUPOK;
	if (flags & LK_NOPROFILE)
		iflags |= LO_NOPROFILE;
	if ((flags & LK_NOWITNESS) == 0)
		iflags |= LO_WITNESS;
	if (flags & LK_QUIET)
		iflags |= LO_QUIET;
	if (flags & LK_IS_VNODE)
		iflags |= LO_IS_VNODE;
	iflags |= flags & (LK_ADAPTIVE | LK_NOSHARE);

	rtems_bsd_mutex_init(
	    &lk->lock_object, &lock_class_lockmgr, wmesg, NULL, iflags);

	lk->lk_lock = LK_UNLOCKED;
	lk->lk_exslpfail = 0;
	lk->lk_timo = timo;
	lk->lk_pri = pri;
	STACK_ZERO(lk);
}

void
lockdestroy(struct lock *lk)
{
	KASSERT(lk->lk_lock == LK_UNLOCKED, ("lockmgr still held"));
	KASSERT(lk->lk_recurse == 0, ("lockmgr still recursed"));
	KASSERT(lk->lk_exslpfail == 0, ("lockmgr still exclusive waiters"));
	rtems_bsd_mutex_destroy(&lk->lock_object);
}

void
lockmgr_printinfo(const struct lock *lk)
{
	struct thread *td;
	uintptr_t x;

	if (lk->lk_lock == LK_UNLOCKED)
		printf("lock type %s: UNLOCKED\n", lk->lock_object.lo_name);
	else if (lk->lk_lock & LK_SHARE)
		printf("lock type %s: SHARED (count %ju)\n",
		    lk->lock_object.lo_name,
		    (uintmax_t)LK_SHARERS(lk->lk_lock));
	else {
		td = lockmgr_xholder(lk);
		if (td == NULL)
			printf("lock type %s: not owned\n",
			    lk->lock_object.lo_name);
		else
			printf("lock type %s: EXCL by thread %p "
			       "(pid %d, rtems, tid %d)\n",
			    lk->lock_object.lo_name, td, td->td_proc->p_pid,
			    td->td_tid);
	}

	x = lk->lk_lock;
	if (x & LK_EXCLUSIVE_WAITERS)
		printf(" with exclusive waiters pending\n");
	if (x & LK_SHARED_WAITERS)
		printf(" with shared waiters pending\n");
	if (x & LK_EXCLUSIVE_SPINNERS)
		printf(" with exclusive spinners pending\n");

	STACK_PRINT(lk);
}

int
__lockmgr_args(struct lock *lk, u_int flags, struct lock_object *ilk,
    const char *wmesg, int prio, int timo, const char *file, int line)
{
	struct lock_class *class;
	uintptr_t x;
	u_int op = (flags & LK_TYPE_MASK);
	int error = 0;

	if (panicstr != NULL)
		return (0);

	class = (flags & LK_INTERLOCK) ? LOCK_CLASS(ilk) : NULL;

	if (lk->lock_object.lo_flags & LK_NOSHARE) {
		switch (op) {
		case LK_SHARED:
			op = LK_EXCLUSIVE;
			break;
		case LK_UPGRADE:
		case LK_TRYUPGRADE:
		case LK_DOWNGRADE:
			if (flags & LK_INTERLOCK)
				class->lc_unlock(ilk);
			return (0);
		}
	}

	switch (op) {
	case LK_SHARED:
		return (lockmgr_slock_hard(lk, flags, ilk, file, line));
		break;
	case LK_UPGRADE:
	case LK_TRYUPGRADE:
		return (lockmgr_upgrade(lk, flags, ilk, file, line));
		break;
	case LK_EXCLUSIVE:
		return (lockmgr_xlock_hard(lk, flags, ilk, file, line));
		break;
	case LK_DOWNGRADE:
		error = lockmgr_downgrade(lk, flags, ilk, file, line);
		break;
	case LK_RELEASE:
		lockmgr_trace("release", '-', lk);
		x = lk->lk_lock;
		if (__predict_true(x & LK_SHARE) != 0) {
			return (lockmgr_sunlock_hard(
			    lk, x, flags, ilk, file, line));
		} else {
			return (lockmgr_xunlock_hard(
			    lk, x, flags, ilk, file, line));
		}
		break;
	case LK_DRAIN:
		break;
	default:
		if (flags & LK_INTERLOCK)
			class->lc_unlock(ilk);
		panic("%s: unknown lockmgr request 0x%x\n", __func__, op);
	}

	if (flags & LK_INTERLOCK)
		class->lc_unlock(ilk);

	return (error);
}

void
_lockmgr_disown(struct lock *lk, const char *file, int line)
{
}

#ifdef INVARIANT_SUPPORT

#ifndef INVARIANTS
#undef _lockmgr_assert
#endif

void
_lockmgr_assert(const struct lock *lk, int what, const char *file, int line)
{
	int slocked = 0;

	if (panicstr != NULL)
		return;
	switch (what) {
	case KA_SLOCKED:
	case KA_SLOCKED | KA_NOTRECURSED:
	case KA_SLOCKED | KA_RECURSED:
		slocked = 1;
	case KA_LOCKED:
	case KA_LOCKED | KA_NOTRECURSED:
	case KA_LOCKED | KA_RECURSED:
#ifdef WITNESS

		/*
		 * We cannot trust WITNESS if the lock is held in exclusive
		 * mode and a call to lockmgr_disown() happened.
		 * Workaround this skipping the check if the lock is held in
		 * exclusive mode even for the KA_LOCKED case.
		 */
		if (slocked || (lk->lk_lock & LK_SHARE)) {
			witness_assert(&lk->lock_object, what, file, line);
			break;
		}
#endif
		if (lk->lk_lock == LK_UNLOCKED ||
		    ((lk->lk_lock & LK_SHARE) == 0 &&
			(slocked ||
			    (!lockmgr_xlocked(lk) && !lockmgr_disowned(lk)))))
			panic("Lock %s not %slocked @ %s:%d\n",
			    lk->lock_object.lo_name, slocked ? "share" : "",
			    file, line);

		if ((lk->lk_lock & LK_SHARE) == 0) {
			if (lockmgr_recursed(lk)) {
				if (what & KA_NOTRECURSED)
					panic("Lock %s recursed @ %s:%d\n",
					    lk->lock_object.lo_name, file,
					    line);
			} else if (what & KA_RECURSED)
				panic("Lock %s not recursed @ %s:%d\n",
				    lk->lock_object.lo_name, file, line);
		}
		break;
	case KA_XLOCKED:
	case KA_XLOCKED | KA_NOTRECURSED:
	case KA_XLOCKED | KA_RECURSED:
		if (!lockmgr_xlocked(lk) && !lockmgr_disowned(lk))
			panic("Lock %s not exclusively locked @ %s:%d\n",
			    lk->lock_object.lo_name, file, line);
		if (lockmgr_recursed(lk)) {
			if (what & KA_NOTRECURSED)
				panic("Lock %s recursed @ %s:%d\n",
				    lk->lock_object.lo_name, file, line);
		} else if (what & KA_RECURSED)
			panic("Lock %s not recursed @ %s:%d\n",
			    lk->lock_object.lo_name, file, line);
		break;
	case KA_UNLOCKED:
		if (lockmgr_xlocked(lk) || lockmgr_disowned(lk))
			panic("Lock %s exclusively locked @ %s:%d\n",
			    lk->lock_object.lo_name, file, line);
		break;
	default:
		panic("Unknown lockmgr assertion: %d @ %s:%d\n", what, file,
		    line);
	}
}
#endif
