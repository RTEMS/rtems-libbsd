/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief TODO.
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef RTEMS_TASKQUEUE_H
#define RTEMS_TASKQUEUE_H

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

struct taskqueue;

typedef void (*task_fn)(void *ctxt, int pending);

/* forwarded 'ctxt' that was passed to taskqueue_create() */
typedef void (*tq_enq_fn)(void *ctxt);

struct task {
	struct task *ta_next;
	int		     ta_pending;
	int		     ta_priority;
	task_fn      ta_fn;
	void        *ta_fn_arg;
};

struct taskqueue *
taskqueue_create(const char *name, int mflags, tq_enq_fn, void *ctxt);

struct taskqueue *
taskqueue_create_fast(const char *name, int mflags, tq_enq_fn, void *ctxt);

int
taskqueue_enqueue(struct taskqueue *tq, struct task *ta);

void
taskqueue_thread_enqueue(void *ctxt);

#define PI_NET	150
/* Returns 0 on success */
int
taskqueue_start_threads(struct taskqueue **ptq, int count, int prio, const char *fmt, ...);

void
taskqueue_drain(struct taskqueue *tq, struct task *ta);

void
taskqueue_free(struct taskqueue *tq);

#define TASK_INIT(task, pri, fn, arg) \
	do { \
		(task)->ta_next     = 0; \
		(task)->ta_priority = (pri); \
		(task)->ta_pending  = 0; \
		(task)->ta_fn       = (fn); \
		(task)->ta_fn_arg   = (arg); \
	} while (0)

extern struct taskqueue *taskqueue_fast;

/* Initialize taskqueue facility [networking must have been initialized already] */
rtems_id
rtems_taskqueue_initialize();

#ifdef __cplusplus
}
#endif

#endif
