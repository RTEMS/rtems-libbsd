/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief TODO.
 */

/*
 * COPYRIGHT (c) 2012.
 * On-Line Applications Research Corporation (OAR).
 * All Rights Reserved.
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

#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <rtems.h>
#include <rtems/error.h>
#include <machine/rtems-bsd-taskqueue.h>

/*
#define STATIC static
*/
#undef  DEBUG

#ifdef DEBUG
#include <stdio.h>
#ifndef STATIC
#define STATIC
#endif
#else
#ifndef STATIC
#define STATIC static
#endif
#endif

#define TQ_WAKE_EVENT RTEMS_EVENT_0

/* This implementation is extremely simple; we assume
 * that all taskqueues (and as a matter of fact there is
 * only a single one) are manipulated with the rtems
 * bsdnet semaphore held. I.e.,
 *   taskqueue_enqueue()
 *   taskqueue_drain()
 *   etc.
 * are called from an environment that holds the
 * bsdnet semaphore.
 * Likewise, the thread that works the taskqueue
 * holds the semaphore while doing so.
 *
 */

/* use single-linked list; 'drain' which would benefit from
 * double-linked list is seldom used and performance doesn't
 * matter much there. OTOH, the frequent case of working
 * the list + enqueueing is more efficient for the single-linked
 * list.
struct task {
	struct task *ta_next;
	int		     ta_pending;
	int		     ta_priority;
	task_fn      ta_fn;
	void        *ta_fn_arg;
};
 */

struct taskqueue {
	struct task anchor;
	struct task *tail;
	tq_enq_fn   enq_fn;
	void       *enq_fn_arg;
	rtems_id    tid;
};


STATIC struct taskqueue the_taskqueue = {
	{ 0, 0, 0, 0, 0 },
	&the_taskqueue.anchor,
	taskqueue_thread_enqueue,
	&taskqueue_fast,
	0
};

struct taskqueue *taskqueue_fast = &the_taskqueue;
struct taskqueue *taskqueue_swi = NULL;

struct taskqueue *
taskqueue_create(const char *name, int mflags, tq_enq_fn enq_fn, void *arg)
{
	if ( enq_fn != taskqueue_thread_enqueue )
		rtems_panic("rtems_taskqueue: attempt to create non-standard TQ; implementation needs to be modified\n");
	return &the_taskqueue;
}

struct taskqueue *
taskqueue_create_fast(const char *name, int mflags, tq_enq_fn enq_fn, void *arg)
{
	return taskqueue_create(name, mflags, enq_fn, arg);
}

/* taskqueue_enqueue must be allowed from an ISR;
 * hence, all critical list manipulation must lock out
 * interrupts...
 */
int
taskqueue_enqueue(struct taskqueue *tq, struct task *ta)
{
rtems_interrupt_level l;

rtems_interrupt_disable(l);
	if ( 0 == ta->ta_pending ++ ) {
		/* hook into list */
		ta->ta_next       = 0;
		tq->tail->ta_next = ta;
		tq->tail          = ta;
	}
	tq->enq_fn(tq->enq_fn_arg);
rtems_interrupt_enable(l);
	return 0;
}

int
taskqueue_enqueue_fast(struct taskqueue *queue, struct task *task)
{
  return taskqueue_enqueue(queue, task);
}

void
taskqueue_thread_enqueue(void *ctxt)
{
int                   dopost;
/* pointer-to-pointer is what bsd provides; we currently
 * follow the scheme even we don't directly use the argument
 * passed to taskqueue_create...
 */
struct taskqueue *tq = *(struct taskqueue **)ctxt;
	/* If this is the first entry on the list then the
	 * task needs to be notified...
	 */
	dopost = ( tq->anchor.ta_next == tq->tail && 1 == tq->tail->ta_pending );

	if ( dopost )
		rtems_event_send(tq->tid, TQ_WAKE_EVENT);
}

/* Returns 0 on success */
int
taskqueue_start_threads(struct taskqueue **ptq, int count, int prio, const char *fmt, ...)
{
	if ( count != 1 )
		rtems_panic("rtems_taskqueue: taskqueue_start_threads cannot currently deal with count != 1\n");
	
	/* Do (non thread-safe) lazy init as a fallback */
	if ( ! the_taskqueue.tid )
		rtems_taskqueue_initialize();
	return 0;
}

void
taskqueue_drain(struct taskqueue *tq, struct task *ta)
{
rtems_interrupt_level l;
struct task *p, *q;
int    i;

	/* find predecessor; searching the list should be
	 * safe; an ISR might append a new record to the tail
	 * while we are working but that should be OK.
	 */
	for ( p = &tq->anchor; (q = p->ta_next); p=q ) {
		if ( q == ta ) {
		rtems_interrupt_disable(l);
			/* found; do work */
			/* remember 'pending' count and extract */
			i              = ta->ta_pending;
			ta->ta_pending = 0;
			p->ta_next     = ta->ta_next;
			ta->ta_next    = 0;
			/* adjust tail */
			if ( tq->tail == q )
				tq->tail = p;
		rtems_interrupt_enable(l);
			for ( ; i>0; i-- ) {
				ta->ta_fn(ta->ta_fn_arg, i);
			}
			return;
		}
	}
}

/* work the task queue and return
 * nonzero if the list is not empty
 * (which means that some callback has
 * rescheduled itself)
 */
static void *
taskqueue_work(struct taskqueue *tq)
{
rtems_interrupt_level l;
struct task   *p, *q;
task_fn        f;
void        *arg;
int            i;

/* work off a temporary list in case any callback reschedules
 * itself or if new tasks are queued from an ISR.
 */
rtems_interrupt_disable(l);
	p = tq->anchor.ta_next;

	tq->anchor.ta_next = 0;
	tq->tail           = &tq->anchor;
rtems_interrupt_enable(l);

	while ( (q=p) ) {
	rtems_interrupt_disable(l);
		i = q->ta_pending;
		q->ta_pending = 0;
		/* extract */
		p          = q->ta_next;
		q->ta_next = 0;
		f          = q->ta_fn;
		arg        = q->ta_fn_arg;
	rtems_interrupt_enable(l);
		for ( ; i>0; i-- ) {
			f(arg, i);
		}
	}
	return tq->anchor.ta_next;
}

void
taskqueue_free(struct taskqueue *tq)
{
	taskqueue_work(tq);
}

static void
taskqueueDoWork(void *arg)
{
struct taskqueue *tq = arg;
rtems_event_set  evs;
rtems_status_code sc;
	while ( 1 ) {
		sc = rtems_event_receive(TQ_WAKE_EVENT, RTEMS_EVENT_ANY | RTEMS_WAIT, RTEMS_NO_TIMEOUT, &evs);
		if ( RTEMS_SUCCESSFUL != sc ) {
			rtems_error(sc,"rtems_taskqueue: taskqueueDoWork() unable to receive wakup event\n");
			rtems_panic("Can't proceed\n");
		}
		if ( taskqueue_work(tq) ) {
#if 0
			/* chance to reschedule */
			rtems_bsdnet_semaphore_release();
			rtems_task_wake_after(0);
			rtems_bsdnet_semaphore_obtain();
#else
			/* hopefully, releasing the semaphore (as part of bsdnet_event_receive)
			 * and obtaining the event (which has been posted already)
			 * yields the CPU if necessary...
			 */
#endif
		}
	}
}

#ifdef DEBUG
struct task_dbg {
	struct task t;
	char        *nm;
};

struct task_dbg taskA = {
	{0},
	"taskA"
};

struct task_dbg taskB = {
	{0},
	"taskB"
};

struct task_dbg taskC = {
	{0},
	"taskC"
};

static void the_task_fn(void *arg, int pending)
{
struct task_dbg *td = arg;
	printf("%s (pending: %i)\n", td->nm, pending);
	/* Test rescheduling */
	if ( pending > 3 )
		taskqueue_enqueue(&the_taskqueue,&td->t);
}

void taskqueue_dump()
{
struct task *p;
	printf("Anchor %p, Tail %p\n", &the_taskqueue.anchor, the_taskqueue.tail);
	for ( p = the_taskqueue.anchor.ta_next; p; p=p->ta_next ) {
		printf("%p: (pending %2i, next %p)\n",
			p, p->ta_pending, p->ta_next);
	}
}
#endif

rtems_id
rtems_taskqueue_initialize()
{
#ifdef DEBUG
	TASK_INIT( &taskA.t, 0, the_task_fn, &taskA );
	TASK_INIT( &taskB.t, 0, the_task_fn, &taskB );
	TASK_INIT( &taskC.t, 0, the_task_fn, &taskC );
#endif
	if ( ! the_taskqueue.tid )
		the_taskqueue.tid = rtems_bsdnet_newproc("tskq", 10000, taskqueueDoWork, &the_taskqueue);
	return the_taskqueue.tid;
}

#ifdef DEBUG
void
_cexpModuleInitialize(void *u)
{
	rtems_bsdnet_initialize_network();
	the_taskqueue.tid = rtems_taskqueue_initialize();
}
#endif
