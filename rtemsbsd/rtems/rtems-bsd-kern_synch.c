/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief TODO.
 */

/*
 * Copyright (c) 1982, 1986, 1990, 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 * (c) UNIX System Laboratories, Inc.
 * All or some portions of this file are derived from material licensed
 * to the University of California by American Telephone and Telegraph
 * Co. or Unix System Laboratories, Inc. and are reproduced herein with
 * the permission of UNIX System Laboratories, Inc.
 *
 * Copyright (c) 2004 John Baldwin <jhb@FreeBSD.org>
 * All rights reserved.
 *
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
 * 4. Neither the name of the author nor the names of any co-contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
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
 *
 *	@(#)kern_synch.c	8.9 (Berkeley) 5/19/95
 */

#include <machine/rtems-bsd-kernel-space.h>
#include <machine/rtems-bsd-thread.h>

#include <rtems/score/statesimpl.h>
#include <rtems/score/threaddispatch.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadqimpl.h>

#include <rtems/bsd/sys/param.h>
#include <rtems/bsd/sys/types.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <rtems/bsd/sys/lock.h>
#include <sys/mutex.h>

static int pause_wchan;

/*
 * Constants for the hash table of sleep queue chains.  These constants are
 * the same ones that 4BSD (and possibly earlier versions of BSD) used.
 * Basically, we ignore the lower 8 bits of the address since most wait
 * channel pointers are aligned and only look at the next 7 bits for the
 * hash.  SC_TABLESIZE must be a power of two for SC_MASK to work properly.
 */
#define	SC_TABLESIZE	128			/* Must be power of 2. */
#define	SC_MASK		(SC_TABLESIZE - 1)
#define	SC_SHIFT	8
#define	SC_HASH(wc)	(((uintptr_t)(wc) >> SC_SHIFT) & SC_MASK)
#define	SC_LOOKUP(wc)	&sleepq_chains[SC_HASH(wc)]

struct sleepqueue_chain {
	LIST_HEAD(, sleepqueue) sc_queues;	/* List of sleep queues. */
};

static struct sleepqueue_chain sleepq_chains[SC_TABLESIZE];

static void
init_sleepqueues(void)
{
	size_t i;

	for (i = 0; i < SC_TABLESIZE; i++) {
		LIST_INIT(&sleepq_chains[i].sc_queues);
	}
}

SYSINIT(rtems_bsd_sleep, SI_SUB_INTRINSIC, SI_ORDER_FIRST, init_sleepqueues, NULL);

/*
 * Look up the sleep queue associated with a given wait channel in the hash
 * table locking the associated sleep queue chain.  If no queue is found in
 * the table, NULL is returned.
 */
static struct sleepqueue *
sleepq_lookup(void *wchan)
{
	struct sleepqueue_chain *sc;
	struct sleepqueue *sq;

	KASSERT(wchan != NULL, ("%s: invalid NULL wait channel", __func__));
	sc = SC_LOOKUP(wchan);
	LIST_FOREACH(sq, &sc->sc_queues, sq_hash)
		if (sq->sq_wchan == wchan)
			return (sq);
	return (NULL);
}

int
_sleep(void *wchan, struct lock_object *lock, int priority, const char *wmesg, int timo)
{
	Thread_Control *executing;
	struct thread *td;
	struct lock_class *class;
	int lock_state;
	int rval;
	struct sleepqueue *sq;
	struct sleepqueue_chain *sc;

#ifdef KTRACE
	if (KTRPOINT(td, KTR_CSW))
		ktrcsw(1, 0);
#endif
	WITNESS_WARN(WARN_GIANTOK | WARN_SLEEPOK, lock,
	    "Sleeping on \"%s\"", wmesg);
	KASSERT(timo != 0 || mtx_owned(&Giant) || lock != NULL,
	    ("sleeping without a lock"));
	KASSERT(p != NULL, ("msleep1"));
	KASSERT(wchan != NULL && TD_IS_RUNNING(td), ("msleep"));
	if (priority & PDROP)
		KASSERT(lock != NULL && lock != &Giant.lock_object,
		    ("PDROP requires a non-Giant lock"));
	if (lock != NULL)
		class = LOCK_CLASS(lock);
	else
		class = NULL;

	if (lock == &Giant.lock_object)
		mtx_assert(&Giant, MA_OWNED);
	DROP_GIANT();

	td = curthread;

	_Thread_Disable_dispatch();

	if (lock != NULL) {
		lock_state = class->lc_unlock(lock);
	}

	sc = SC_LOOKUP(wchan);

	/* Look up the sleep queue associated with the wait channel 'wchan'. */
	sq = sleepq_lookup(wchan);

	/*
	 * If the wait channel does not already have a sleep queue, use
	 * this thread's sleep queue.  Otherwise, insert the current thread
	 * into the sleep queue already in use by this wait channel.
	 */
	if (sq == NULL) {
		sq = td->td_sleepqueue;
		LIST_INSERT_HEAD(&sc->sc_queues, sq, sq_hash);
		sq->sq_wchan = wchan;
	} else {
		LIST_INSERT_HEAD(&sq->sq_free, td->td_sleepqueue, sq_hash);
	}
	td->td_sleepqueue = NULL;

	_Thread_queue_Enter_critical_section(&sq->sq_blocked);
	executing = _Thread_Executing;
	executing->Wait.queue = &sq->sq_blocked;
	_Thread_queue_Enqueue(&sq->sq_blocked, executing, (Watchdog_Interval) timo);

	_Thread_Enable_dispatch();

	rval = (int) executing->Wait.return_code;

	_Thread_Disable_dispatch();

	/*
	 * Get a sleep queue for this thread.  If this is the last waiter,
	 * use the queue itself and take it out of the chain, otherwise,
	 * remove a queue from the free list.
	 */
	if (LIST_EMPTY(&sq->sq_free)) {
		td->td_sleepqueue = sq;
	} else
		td->td_sleepqueue = LIST_FIRST(&sq->sq_free);
	LIST_REMOVE(td->td_sleepqueue, sq_hash);

	_Thread_Enable_dispatch();

	PICKUP_GIANT();
	if (lock != NULL && lock != &Giant.lock_object && !(priority & PDROP)) {
		class->lc_lock(lock, lock_state);
		WITNESS_RESTORE(lock, lock_witness);
	}

	return (rval);
}

/*
 * pause() is like tsleep() except that the intention is to not be
 * explicitly woken up by another thread.  Instead, the current thread
 * simply wishes to sleep until the timeout expires.  It is
 * implemented using a dummy wait channel.
 */
int
pause(const char *wmesg, int timo)
{

	KASSERT(timo != 0, ("pause: timeout required"));
	return (tsleep(&pause_wchan, 0, wmesg, timo));
}

static void
rtems_bsd_sleepq_wakeup(struct sleepqueue *sq, Thread_Control *thread)
{
	thread->Wait.return_code = 0;
}

void
wakeup(void *wchan)
{
	struct sleepqueue *sq;

	_Thread_Disable_dispatch();

	sq = sleepq_lookup(wchan);
	if (sq != NULL) {
		Thread_Control *thread;

		while ((thread = _Thread_queue_Dequeue(&sq->sq_blocked)) != NULL) {
			rtems_bsd_sleepq_wakeup(sq, thread);
		}
	}

	_Thread_Enable_dispatch();
}

void
wakeup_one(void *wchan)
{
	struct sleepqueue *sq;

	_Thread_Disable_dispatch();

	sq = sleepq_lookup(wchan);
	if (sq != NULL) {
		Thread_Control *thread;

		thread = _Thread_queue_Dequeue(&sq->sq_blocked);
		if (thread != NULL) {
			rtems_bsd_sleepq_wakeup(sq, thread);
		}
	}

	_Thread_Enable_dispatch();
}
