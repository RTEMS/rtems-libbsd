/*-
 * Copyright (c) 2010 Isilon Systems, Inc.
 * Copyright (c) 2010 iX Systems, Inc.
 * Copyright (c) 2010 Panasas, Inc.
 * Copyright (c) 2013, 2014 Mellanox Technologies, Ltd.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice unmodified, this list of conditions, and the following
 *    disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $FreeBSD$
 */
#ifndef	_LINUX_KTHREAD_H_
#define	_LINUX_KTHREAD_H_

#include <sys/param.h>
#include <sys/lock.h>
#include <sys/mutex.h>
#include <sys/kernel.h>
#include <sys/kthread.h>
#include <sys/sleepqueue.h>

#include <machine/rtems-bsd-thread.h>

#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/smp.h>
#include <linux/threads.h>

#define	KTHREAD_LOCK(task) mtx_lock(&(task)->lock)
#define	KTHREAD_UNLOCK(task) mtx_unlock(&(task)->lock)

static inline void
_kthread_fn(void *arg)
{
	struct task_struct *task;
	struct thread *c;

	task = arg;
	task_struct_set(curthread, task);
	c = task->task_thread;
	sleepq_lock(c);
	while (task->state == TASK_DORMANT)
		sleepq_wait(c, 0);
	sleepq_release(c);
	if (task->should_stop == 0)
		task->task_ret = task->task_fn(task->task_data);
	KTHREAD_LOCK(task);
	task->should_stop = TASK_STOPPED;
	wakeup(task);
	KTHREAD_UNLOCK(task);
	kthread_exit();
}

static inline struct task_struct *
_kthread_create(int (*threadfn)(void *data), void *data)
{
	struct task_struct *task;

	task = kzalloc(sizeof(*task), GFP_KERNEL);
	task->task_fn = threadfn;
	task->task_data = data;
	mtx_init(&task->lock, "kthread", NULL, MTX_DEF);

	return (task);
}

#define	kthread_create(fn, data, fmt, ...)					\
({									\
	struct task_struct *_task;					\
									\
	_task = _kthread_create((fn), (data));				\
	if (kthread_add(_kthread_fn, _task, NULL, &_task->task_thread,	\
	    0, 0, fmt, ## __VA_ARGS__)) {				\
		kfree(_task);						\
		_task = NULL;						\
	} else								\
		task_struct_set(_task->task_thread, _task);		\
	_task;								\
})

#define	kthread_should_stop()	current->should_stop

static inline int
kthread_stop(struct task_struct *task)
{

	KTHREAD_LOCK(task);
	task->should_stop = TASK_SHOULD_STOP;
	wake_up_process(task);
	while (task->should_stop != TASK_STOPPED)
		msleep(task, &task->lock, PWAIT, "kstop", hz);
	KTHREAD_UNLOCK(task);
	return task->task_ret;
}

static inline void
kthread_bind(struct task_struct *task, unsigned int cpu)
{
	/* FIXME */
	rtems_id task_id = rtems_bsd_get_task_id(task->task_thread);
	rtems_id sched_id;
	rtems_status_code sc;
	rtems_task_priority prio;

	sc = rtems_scheduler_ident(cpu, &sched_id);
	if (sc != RTEMS_SUCCESSFUL)
		panic("kthread_bind: scheduler ident");

	sc = rtems_task_set_priority(task_id, RTEMS_CURRENT_PRIORITY, &prio);
	if (sc != RTEMS_SUCCESSFUL)
		panic("kthread_bind: get priority");

	sc = rtems_task_set_scheduler(task_id, sched_id, prio);
	if (sc != RTEMS_SUCCESSFUL)
		panic("kthread_bind: set scheduler");
}

#endif	/* _LINUX_KTHREAD_H_ */
