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

#include <rtems/freebsd/machine/rtems-bsd-config.h>

#include <rtems/freebsd/sys/param.h>
#include <rtems/freebsd/sys/types.h>
#include <rtems/freebsd/sys/systm.h>
#include <rtems/freebsd/sys/proc.h>
#include <rtems/freebsd/sys/kthread.h>
#include <rtems/freebsd/sys/malloc.h>

RTEMS_CHAIN_DEFINE_EMPTY(rtems_bsd_thread_chain);

static int
rtems_bsd_thread_start(struct thread **td_ptr, void (*func)(void *), void *arg, int flags, int pages, const char *fmt, va_list ap)
{
  struct proc *p = &proc0;;
	struct thread *td = malloc(sizeof(struct thread), M_TEMP, M_WAITOK | M_ZERO);

	if (td != NULL) {
		rtems_status_code sc = RTEMS_SUCCESSFUL;
		rtems_id id = RTEMS_ID_NONE;
		unsigned index = 0;
		char name [5] = "_???";

		BSD_ASSERT(pages >= 0);

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

    sc = rtems_task_set_note( id, RTEMS_NOTEPAD_0, ( uint32_t )td );
    if (sc != RTEMS_SUCCESSFUL) {
      free(td, M_TEMP);

      return ENOMEM;
    }

		index = rtems_object_id_get_index(id);
		snprintf(name + 1, sizeof(name) - 1, "%03u", index);
		sc = rtems_object_set_name(id, name);
		if (sc != RTEMS_SUCCESSFUL) {
			rtems_task_delete(id);
			free(td, M_TEMP);

			return ENOMEM;
		}

		sc = rtems_task_start(id, (rtems_task_entry) func, (rtems_task_argument) arg);
		if (sc != RTEMS_SUCCESSFUL) {
			rtems_task_delete(id);
			free(td, M_TEMP);

			return ENOMEM;
		}

		td->td_id = id;
		vsnprintf(td->td_name, sizeof(td->td_name), fmt, ap);
    bzero(&td->td_ru, sizeof(td->td_ru));
    td->td_ucred = crhold(p->p_ucred);
    td->td_proc = p;

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
