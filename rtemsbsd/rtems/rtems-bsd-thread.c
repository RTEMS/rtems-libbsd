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

#include <machine/rtems-bsd-config.h>

#include <rtems/bsd/sys/param.h>
#include <rtems/bsd/sys/types.h>
#include <sys/systm.h>
#include <sys/proc.h>
#include <sys/kthread.h>
#include <sys/malloc.h>
#include <rtems/bsd/sys/lock.h>
#include <sys/mutex.h>
#include <sys/jail.h>
#include <sys/resourcevar.h>
#include <sys/filedesc.h>

RTEMS_CHAIN_DEFINE_EMPTY(rtems_bsd_thread_chain);

/* FIXME: What to do with the credentials? */
static struct ucred FIXME_ucred = {
  .cr_ref = 1				/* reference count */
};
static struct filedesc FIXME_fd = {
  .fd_ofiles = NULL	/* file structures for open files */
};
static struct proc  FIXME_proc = {
  .p_ucred = NULL /* (c) Process owner's identity. */
};
static int prison_init = 1;
static struct prison FIXME_prison = {
  .pr_parent = NULL
};

static struct uidinfo	FIXME_uidinfo;	/* per euid resource consumption */
static struct uidinfo	FIXME_ruidinfo;	/* per ruid resource consumption */

static struct thread *rtems_bsd_current_td = NULL;

static void rtems_bsd_thread_descriptor_dtor(void *td)
{
	// XXX are there other pieces to clean up?
	free(td, M_TEMP);
}

static struct thread *
rtems_bsd_thread_init( rtems_id id )
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;
	unsigned index = 0;
	char name [5] = "_???";
	struct thread *td;
	struct proc   *proc;

	td = malloc(sizeof(struct thread), M_TEMP, M_WAITOK | M_ZERO);
	if (td == NULL)
		return NULL;

	// Initialize the thread descriptor
	index = rtems_object_id_get_index(id);
	snprintf(name + 1, sizeof(name) - 1, "%03u", index);
	sc = rtems_object_set_name(id, name);
	if (sc != RTEMS_SUCCESSFUL) {
		// XXX does the thread get deleted? Seems wrong
		// rtems_task_delete(id);
		free(td, M_TEMP);
		return 	NULL;
	}

	td->td_id = id;
	td->td_ucred = crhold(&FIXME_ucred);
  
	td->td_proc = &FIXME_proc;
	if (td->td_proc->p_ucred == NULL) {
  		if ( prison_init ) {
			mtx_init(&FIXME_prison.pr_mtx, "prison lock", NULL, MTX_DEF | MTX_DUPOK);
    			prison_init = 0;
  		}
  		FIXME_ucred.cr_prison   = &FIXME_prison;    /* jail(2) */
		FIXME_ucred.cr_uidinfo  = uifind(0);
		FIXME_ucred.cr_ruidinfo = uifind(0);
		FIXME_ucred.cr_ngroups = 1;     /* group 0 */

		td->td_proc->p_ucred = crhold(&FIXME_ucred);
		mtx_init(&td->td_proc->p_mtx, "process lock", NULL, MTX_DEF | MTX_DUPOK);
		td->td_proc->p_pid = getpid();
		td->td_proc->p_fibnum = 0;
		td->td_proc->p_fd = &FIXME_fd;
		sx_init_flags(&FIXME_fd.fd_sx, "config SX thread lock", SX_DUPOK);
	}

	// Actually set the global pointer 
	rtems_bsd_current_td = td;

	// Now add the task descriptor as a per-task variable
	sc = rtems_task_variable_add(
		id,
		&rtems_bsd_current_td,
		rtems_bsd_thread_descriptor_dtor
	);
	if (sc != RTEMS_SUCCESSFUL) {
		free(td, M_TEMP);
		return NULL;
	}

  	return td;
}

/*
 *  Threads which delete themselves would leak the task
 *  descriptor so we are using the per-task variable so
 *  it can be cleaned up.
 */
struct thread *rtems_get_curthread(void)
{
	struct thread *td;

	/*
	 * If we already have a struct thread associated with this thread,
	 * obtain it. Otherwise, allocate and initialize one.
	 */
	td = rtems_bsd_current_td;
	if ( td == NULL ) {
		td = rtems_bsd_thread_init( rtems_task_self() );
		if ( td == NULL ){
			panic("rtems_get_curthread: Unable to thread descriptor\n");
		}
	}

  return td;
}

static int
rtems_bsd_thread_start(struct thread **td_ptr, void (*func)(void *), void *arg, int flags, int pages, const char *fmt, va_list ap)
{
	struct thread *td = malloc(sizeof(struct thread), M_TEMP, M_WAITOK | M_ZERO);

	if (td != NULL) {
		rtems_status_code sc = RTEMS_SUCCESSFUL;
		rtems_id id = RTEMS_ID_NONE;
		unsigned index = 0;
		char name [5] = "_???";

		BSD_ASSERT(pages >= 0);

		memset( td, 0, sizeof(struct thread) );

		sc = rtems_task_create(
			rtems_build_name('_', 'T', 'S', 'K'),
			BSD_TASK_PRIORITY_NORMAL,
			BSD_MINIMUM_TASK_STACK_SIZE + (size_t) pages * PAGE_SIZE,
			RTEMS_DEFAULT_ATTRIBUTES,
			RTEMS_DEFAULT_ATTRIBUTES,
			&id
		);
		if (sc != RTEMS_SUCCESSFUL) {
			free(td, M_TEMP);

			return ENOMEM;
		}

		td = rtems_bsd_thread_init( id );
		if (!td)
			return ENOMEM;
		
		sc = rtems_task_start(id, (rtems_task_entry) func, (rtems_task_argument) arg);
		if (sc != RTEMS_SUCCESSFUL) {
			rtems_task_delete(id);
			free(td, M_TEMP);

			return ENOMEM;
		}

		td->td_id = id;
		vsnprintf(td->td_name, sizeof(td->td_name), fmt, ap);
		td->td_ucred = crhold(&FIXME_ucred);

		rtems_chain_append(&rtems_bsd_thread_chain, &td->td_node);

		if (td_ptr != NULL) {
			*td_ptr = td;
		}

		return 0;
	}

	return ENOMEM;
}

static void rtems_bsd_thread_delete(void) __dead2;

static void
rtems_bsd_thread_delete(void)
{
	rtems_chain_control *chain = &rtems_bsd_thread_chain;
	rtems_chain_node *node = rtems_chain_first(chain);
	rtems_id id = rtems_task_self();
	struct thread *td = NULL;

	while (!rtems_chain_is_tail(chain, node)) {
		struct thread *cur = (struct thread *) node;

		if (cur->td_id == id) {
			td = cur;
			break;
		}

		node = rtems_chain_next(node);
	}

	if (td != NULL) {
		rtems_chain_extract(&td->td_node);

		free(td, M_TEMP);
	} else {
		BSD_PANIC("cannot find task entry");
	}

	rtems_task_delete(RTEMS_SELF);

	while (true) {
		/* Do nothing */
	}
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
	eno = rtems_bsd_thread_start(newpp, func, arg, flags, pages, fmt, ap);
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
