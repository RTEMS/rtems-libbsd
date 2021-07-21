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
#include <machine/rtems-bsd-thread.h>
#include <machine/rtems-bsd-support.h>

#include <sys/param.h>
#include <sys/types.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/proc.h>
#include <sys/kthread.h>
#include <sys/malloc.h>
#include <sys/selinfo.h>
#include <sys/sleepqueue.h>

#include <rtems/bsd/bsd.h>

#include <rtems/score/objectimpl.h>
#include <rtems/score/statesimpl.h>
#include <rtems/score/threaddispatch.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/threadqimpl.h>

static size_t rtems_bsd_extension_index;

static CHAIN_DEFINE_EMPTY(rtems_bsd_thread_delay_start_chain);

static bool rtems_bsd_thread_ready_to_start;

struct thread *
rtems_bsd_get_thread(const Thread_Control *thread)
{
	return thread->extensions[rtems_bsd_extension_index];
}

static Thread_Control *
rtems_bsd_get_thread_by_id(rtems_id task_id)
{
	Thread_Control *thread;
	ISR_lock_Context lock_context;

	thread = _Thread_Get(task_id, &lock_context);
	if (thread != NULL) {
		_ISR_lock_ISR_enable(&lock_context);
	}

	return (thread);
}

struct thread *
rtems_bsd_thread_create(Thread_Control *thread, int wait)
{
	struct thread *td = malloc(sizeof(*td), M_TEMP, M_ZERO | wait);

	if (td != NULL) {
		struct sleepqueue *sq = sleepq_alloc();
		if (sq != NULL) {
			td->td_proc = &proc0;
			td->td_ucred = proc0.p_ucred;
			td->td_thread = thread;
			td->td_sleepqueue = sq;
			crhold(td->td_ucred);
		} else {
			free(td, M_TEMP);
			td = NULL;
		}
	}

	thread->extensions[rtems_bsd_extension_index] = td;

	return td;
}

static struct thread *
rtems_bsd_get_curthread(int wait)
{
	Thread_Control *executing = _Thread_Get_executing();
	struct thread *td = rtems_bsd_get_thread(executing);

	if (td == NULL) {
		td = rtems_bsd_thread_create(executing, wait);
	}

	return td;
}

struct thread *
rtems_bsd_get_curthread_or_wait_forever(void)
{
	return rtems_bsd_get_curthread(M_WAITOK);
}

struct thread *
rtems_bsd_get_curthread_or_null(void)
{
	return rtems_bsd_get_curthread(0);
}

static bool
rtems_bsd_is_bsd_thread(Thread_Control *thread)
{
	return thread->Object.name.name_u32 == BSD_TASK_NAME;
}

static bool
rtems_bsd_extension_thread_create(
	Thread_Control *executing,
	Thread_Control *created
)
{
	bool ok;

	if (rtems_bsd_is_bsd_thread(created)) {
		struct thread *td = rtems_bsd_thread_create(created, 0);

		ok = td != NULL;
	} else {
		ok = true;
	}

	return ok;
}

static void
rtems_bsd_extension_thread_delete(
	Thread_Control *executing,
	Thread_Control *deleted
)
{
	struct thread *td = rtems_bsd_get_thread(deleted);

	if (td != NULL) {
		seltdfini(td);
		sleepq_free(td->td_sleepqueue);
		crfree(td->td_ucred);
		free(td, M_TEMP);
	}
}

static const rtems_extensions_table rtems_bsd_extensions = {
	.thread_create = rtems_bsd_extension_thread_create,
	.thread_delete = rtems_bsd_extension_thread_delete
};

static void
rtems_bsd_threads_init_early(void *arg)
{
	rtems_id ext_id;
	rtems_status_code sc;

	(void) arg;

	sc = rtems_extension_create(
		BSD_TASK_NAME,
		&rtems_bsd_extensions,
		&ext_id
	);
	if (sc != RTEMS_SUCCESSFUL) {
		BSD_PANIC("cannot create extension");
	}

	rtems_bsd_extension_index = rtems_object_id_get_index(ext_id);
}

static void
rtems_bsd_threads_init_late(void *arg)
{
	Chain_Control *chain = &rtems_bsd_thread_delay_start_chain;
	Chain_Node *node;

	(void) arg;

	while ((node = _Chain_Get_unprotected(chain)) != NULL) {
		Thread_Control *thread = (Thread_Control *) node;
		rtems_status_code sc;

		sc = rtems_task_start(thread->Object.id,
		    thread->Start.Entry.Kinds.Numeric.entry,
		    thread->Start.Entry.Kinds.Numeric.argument);
		BSD_ASSERT(sc == RTEMS_SUCCESSFUL);
	}

	rtems_bsd_thread_ready_to_start = true;
}

SYSINIT(rtems_bsd_threads_early, SI_SUB_TUNABLES, SI_ORDER_ANY,
    rtems_bsd_threads_init_early, NULL);

SYSINIT(rtems_bsd_threads_late, SI_SUB_LAST, SI_ORDER_ANY,
    rtems_bsd_threads_init_late, NULL);

static int
rtems_bsd_thread_start(struct thread **td_ptr, void (*func)(void *), void *arg,
    int flags, int pages, const char *fmt, va_list ap)
{
	int eno = 0;
	rtems_status_code sc;
	rtems_id task_id;
	struct thread *td;
	char name[32];

	BSD_ASSERT(pages >= 0);

	vsnprintf(name, sizeof(name), fmt, ap);

	sc = rtems_task_create(
		BSD_TASK_NAME,
		rtems_bsd_get_task_priority(name),
		rtems_bsd_get_task_stack_size(name)
			+ (size_t) pages * PAGE_SIZE,
		RTEMS_DEFAULT_MODES,
		RTEMS_DEFAULT_ATTRIBUTES,
		&task_id
	);
	if (sc == RTEMS_SUCCESSFUL) {
		Thread_Control *thread = rtems_bsd_get_thread_by_id(task_id);

		BSD_ASSERT(thread != NULL);

		td = rtems_bsd_get_thread(thread);
		BSD_ASSERT(td != NULL);

		_Thread_Set_name(thread, name);

		if (rtems_bsd_thread_ready_to_start) {
			sc = rtems_task_start(task_id, (rtems_task_entry) func,
			    (rtems_task_argument) arg);
			BSD_ASSERT(sc == RTEMS_SUCCESSFUL);
		} else {
			thread->Start.Entry.Kinds.Numeric.entry =
			    (void (*)(Thread_Entry_numeric_type))func;
			thread->Start.Entry.Kinds.Numeric.argument =
			    (Thread_Entry_numeric_type)arg;
			_Chain_Append_unprotected(
			    &rtems_bsd_thread_delay_start_chain,
			    &thread->Object.Node);
		}

		if (td_ptr != NULL) {
			*td_ptr = td;
		}
	} else {
		eno = ENOMEM;
	}

	return eno;
}

static __dead2 void
rtems_bsd_thread_delete(void)
{
	rtems_task_delete(RTEMS_SELF);
	BSD_PANIC("delete self failed");
}

void
kproc_start(const void *udata)
{
	const struct kproc_desc	*pd = udata;
	int eno = kproc_create((void (*)(void *))pd->func, NULL, pd->global_procpp, 0, 0, "%s", pd->arg0);

	BSD_ASSERT(eno == 0);
}

int
kproc_create(void (*func)(void *), void *arg, struct proc **newpp, int flags, int pages, const char *fmt, ...)
{
	int eno = 0;
	va_list ap;

	va_start(ap, fmt);
	eno = rtems_bsd_thread_start((struct thread**) newpp, func, arg, flags, pages, fmt, ap);
	va_end(ap);

	return eno;
}

void
kproc_exit(int ecode)
{
	rtems_bsd_thread_delete();
}

void
kthread_start(const void *udata)
{
	const struct kthread_desc *td = udata;
	int eno = kthread_add((void (*)(void *)) td->func, NULL, NULL, td->global_threadpp, 0, 0, "%s", td->arg0);

	BSD_ASSERT(eno == 0);
}

int
kthread_add(void (*func)(void *), void *arg, struct proc *p, struct thread **newtdp, int flags, int pages, const char *fmt, ...)
{
	int eno = 0;
	va_list ap;

	va_start(ap, fmt);
	/* the cast here is a hack but passing a proc as a thread struct is just wrong and I
	 * have no idea why it is like this */
	eno = rtems_bsd_thread_start(newtdp, func, arg, flags, pages, fmt, ap);
	va_end(ap);

	return eno;
}

void
kthread_exit(void)
{
	rtems_bsd_thread_delete();
}

int
kproc_kthread_add(void (*func)(void *), void *arg, struct proc **procptr, struct thread **tdptr, int flags, int pages, const char * procname, const char *fmt, ...)
{
	int eno = 0;
	va_list ap;

	va_start(ap, fmt);
	eno = rtems_bsd_thread_start(tdptr, func, arg, flags, pages, fmt, ap);
	va_end(ap);

	return eno;
}
