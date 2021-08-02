/**
 * @file
 *
 * @brief The creation and execution of threads (RTEMS task, kproc, kthread) are tested.
 *
 * The main thread creates a worker and the waits until receives an event of the
 * created worker.
 */

/*
 * Copyright (c) 2013, 2018 embedded brains GmbH.  All rights reserved.
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <sys/types.h>
#include <sys/param.h>
#include <sys/proc.h>
#include <sys/kthread.h>
#include <sys/errno.h>

#include <vm/uma.h>

#include <rtems/bsd/bsd.h>

#include <rtems.h>
#include <rtems/libcsupport.h>
#include <rtems/score/threaddispatch.h>
#include <rtems/score/wkspace.h>

#define TEST_NAME "LIBBSD THREAD 1"

#define TEST_KTHREAD_ADD ((void *) 0xdeadbeef)

static rtems_id main_task_id;

static char test_kproc_name[] = "kproc";

static struct kproc_desc test_kproc_start_desc;

static char test_kthread_name[] = "kthread";

static struct kthread_desc test_kthread_start_desc;

static void
test_curthread(const char *name)
{
	struct thread *td_0 = rtems_bsd_get_curthread_or_null();
	struct thread *td_1 = rtems_bsd_get_curthread_or_wait_forever();
	struct thread *td_2 = curthread;

	assert(td_0 != NULL);
	assert(td_0 == td_1);
	assert(td_0 == td_2);
	assert(strcmp(td_0->td_thread->Join_queue.Queue.name, name) == 0);
}

static void
wake_up_main_thread(void)
{
	rtems_status_code sc;

	sc = rtems_event_transient_send(main_task_id);
	assert(sc == RTEMS_SUCCESSFUL);
}

static void
wait_for_worker_thread(void)
{
	rtems_status_code sc;

	sc = rtems_event_transient_receive(RTEMS_WAIT, RTEMS_NO_TIMEOUT);
	assert(sc == RTEMS_SUCCESSFUL);
}

static void
non_bsd_thread(rtems_task_argument arg)
{
	rtems_status_code sc;

	test_curthread("");
	wake_up_main_thread();

	sc = rtems_task_delete(RTEMS_SELF);
	assert(sc == RTEMS_SUCCESSFUL);
}

static void
test_non_bsd_thread(void)
{
	rtems_status_code sc;
	rtems_id task_id;
	rtems_resource_snapshot snapshot;

	rtems_resource_snapshot_take(&snapshot);

	sc = rtems_task_create(
		rtems_build_name('T', 'A', 'S', 'K'),
		RTEMS_MINIMUM_PRIORITY,
		RTEMS_MINIMUM_STACK_SIZE,
		RTEMS_DEFAULT_MODES,
		RTEMS_FLOATING_POINT,
		&task_id
	);
	assert(sc == RTEMS_SUCCESSFUL);

	sc = rtems_task_start(task_id, non_bsd_thread, 0);
	assert(sc == RTEMS_SUCCESSFUL);

	wait_for_worker_thread();

	assert(rtems_resource_snapshot_check(&snapshot));
}

static void
test_kproc_start_proc(void)
{
	test_curthread(&test_kproc_name[0]);
	wake_up_main_thread();
	kproc_exit(0);
}

static void
test_kproc_start(void)
{
	rtems_resource_snapshot snapshot;
	struct proc *pr = NULL;
	struct kproc_desc *kpd = &test_kproc_start_desc;

	puts("test kproc_start()");

	rtems_resource_snapshot_take(&snapshot);

	kpd->arg0 = &test_kproc_name[0];
	kpd->func = test_kproc_start_proc,
	kpd->global_procpp = &pr;

	kproc_start(kpd);
	wait_for_worker_thread();

	assert(pr != NULL);

	assert(rtems_resource_snapshot_check(&snapshot));
}

static void
test_kthread_start_thread(void)
{
	test_curthread(&test_kthread_name[0]);
	wake_up_main_thread();
	kthread_exit();
}

static void
test_kthread_start(void)
{
	rtems_resource_snapshot snapshot;
	struct thread *td = NULL;
	struct kthread_desc *ktd = &test_kthread_start_desc;

	puts("test kthread_start()");

	rtems_resource_snapshot_take(&snapshot);

	ktd->arg0 = &test_kthread_name[0];
	ktd->func = test_kthread_start_thread,
	ktd->global_threadpp = &td;

	kthread_start(ktd);
	wait_for_worker_thread();

	assert(td != NULL);

	assert(rtems_resource_snapshot_check(&snapshot));
}

static void
test_kthread_add_thread(void *arg)
{
	test_curthread(&test_kthread_name[0]);

	assert(arg == TEST_KTHREAD_ADD);

	wake_up_main_thread();
	kthread_exit();
}

static void
test_kthread_add(void)
{
	rtems_resource_snapshot snapshot;
	void *greedy;
	uintptr_t take_away = 2 * rtems_bsd_get_task_stack_size("");

	puts("test kthread_add()");

	greedy = rtems_workspace_greedy_allocate(&take_away, 1);

	rtems_resource_snapshot_take(&snapshot);

	assert(rtems_configuration_get_unified_work_area());

	while (take_away > 0) {
		void *away;
		bool ok;

		ok = rtems_workspace_allocate(take_away, &away);
		if (ok) {
			struct thread *td = NULL;
			int eno;

			eno = kthread_add(
				test_kthread_add_thread,
				TEST_KTHREAD_ADD,
				NULL,
				&td,
				0,
				0,
				"%s",
				&test_kthread_name[0]
			);

			ok = rtems_workspace_free(away);
			assert(ok);

			if (eno == 0) {
				wait_for_worker_thread();
				assert(td != NULL);

				break;
			} else {
				assert(eno == ENOMEM);
				assert(rtems_resource_snapshot_check(&snapshot));
			}
		}

		--take_away;
	}

	assert(take_away > 0);

	rtems_workspace_greedy_free(greedy);
}

static void
test_rtems_bsd_get_curthread_or_null(void)
{
#ifdef TEST_IS_BROKEN
	/*
	 * This part of test relies on nothing calling
	 * rtems_bsd_get_current.*() before getting here so the workspace
	 * can be consumed and the allocation fails. Changes in other
	 * areas of libbsd have resulted in `Init` having a struct thread
	 * object allocated.
	 *
	 * Maybe creating a new thread and using that would be a more stable test.
	 */
	rtems_resource_snapshot snapshot;
	void *greedy;

	puts("test rtems_bsd_get_curthread_or_null()");

	rtems_resource_snapshot_take(&snapshot);

	greedy = rtems_workspace_greedy_allocate(NULL, 0);
	assert(rtems_bsd_get_curthread_or_null() == NULL);
	rtems_workspace_greedy_free(greedy);

	rtems_resource_snapshot_take(&snapshot);
#endif
}

static void
test_main(void)
{
	main_task_id = rtems_task_self();

	/*
	 * Stop interferences of uma_timeout() which may need some dynamic
	 * memory.  This could disturb the no memory tests.
	 */
	rtems_uma_drain_timeout();

	test_non_bsd_thread();
	test_kproc_start();
	test_kthread_start();
	test_kthread_add();
	test_rtems_bsd_get_curthread_or_null();

	exit(0);
}

#include <rtems/bsd/test/default-init.h>
