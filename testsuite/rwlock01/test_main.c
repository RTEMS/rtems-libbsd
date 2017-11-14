/*
 * Copyright (c) 2013, 2017 embedded brains GmbH.  All rights reserved.
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

#include <sys/param.h>
#include <sys/types.h>
#include <sys/systm.h>
#include <sys/lock.h>
#include <sys/rwlock.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <rtems/libcsupport.h>
#include <rtems.h>

#define TEST_NAME "LIBBSD RWLOCK 1"

#define PRIO_MASTER 2

#define PRIO_WORKER 1

#define EVENT_RLOCK RTEMS_EVENT_0

#define EVENT_WLOCK RTEMS_EVENT_1

#define EVENT_TRY_RLOCK RTEMS_EVENT_2

#define EVENT_TRY_WLOCK RTEMS_EVENT_3

#define EVENT_UNLOCK RTEMS_EVENT_4

#define EVENT_SLEEP RTEMS_EVENT_5

typedef struct {
	struct rwlock rw;
	bool done;
	int rv;
	bool done2;
	int rv2;
	int timo;
	rtems_id worker_task;
	rtems_id worker2_task;
} test_context;

static test_context test_instance;

static void
set_self_prio(rtems_task_priority prio)
{
	rtems_status_code sc;

	sc = rtems_task_set_priority(RTEMS_SELF, prio, &prio);
	assert(sc == RTEMS_SUCCESSFUL);
}

static void
worker(test_context *ctx, int *rv, bool *done)
{
	struct rwlock *rw = &ctx->rw;

	while (true) {
		rtems_status_code sc;
		rtems_event_set events;

		sc = rtems_event_receive(
			RTEMS_ALL_EVENTS,
			RTEMS_EVENT_ANY | RTEMS_WAIT,
			RTEMS_NO_TIMEOUT,
			&events
		);
		assert(sc == RTEMS_SUCCESSFUL);

		if ((events & EVENT_RLOCK) != 0) {
			rw_rlock(rw);
			*done = true;
		}

		if ((events & EVENT_WLOCK) != 0) {
			rw_wlock(rw);
			*done = true;
		}

		if ((events & EVENT_TRY_RLOCK) != 0) {
			*rv = rw_try_rlock(rw);
			*done = true;
		}

		if ((events & EVENT_TRY_WLOCK) != 0) {
			*rv = rw_try_wlock(rw);
			*done = true;
		}

		if ((events & EVENT_UNLOCK) != 0) {
			rw_unlock(rw);
			*done = true;
		}

		if ((events & EVENT_SLEEP) != 0) {
			*rv = rw_sleep(ctx, rw, 0, "worker", ctx->timo);
			*done = true;
		}
	}
}

static void
worker_task(rtems_task_argument arg)
{
	test_context *ctx = (test_context *) arg;

	worker(ctx, &ctx->rv, &ctx->done);
}

static void
worker2_task(rtems_task_argument arg)
{
	test_context *ctx = (test_context *) arg;

	worker(ctx, &ctx->rv2, &ctx->done2);
}

static void
send_events(test_context *ctx, rtems_event_set events)
{
	rtems_status_code sc;

	sc = rtems_event_send(ctx->worker_task, events);
	assert(sc == RTEMS_SUCCESSFUL);
}

static void
send_events2(test_context *ctx, rtems_event_set events)
{
	rtems_status_code sc;

	sc = rtems_event_send(ctx->worker2_task, events);
	assert(sc == RTEMS_SUCCESSFUL);
}

static void
start_worker(test_context *ctx)
{
	rtems_status_code sc;

	sc = rtems_task_create(
		rtems_build_name('W', 'R', 'K', '1'),
		PRIO_WORKER,
		RTEMS_MINIMUM_STACK_SIZE,
		RTEMS_DEFAULT_MODES,
		RTEMS_FLOATING_POINT,
		&ctx->worker_task
	);
	assert(sc == RTEMS_SUCCESSFUL);

	sc = rtems_task_start(
		ctx->worker_task,
		worker_task,
		(rtems_task_argument) ctx
	);
	assert(sc == RTEMS_SUCCESSFUL);

	sc = rtems_task_create(
		rtems_build_name('W', 'R', 'K', '2'),
		PRIO_WORKER,
		RTEMS_MINIMUM_STACK_SIZE,
		RTEMS_DEFAULT_MODES,
		RTEMS_FLOATING_POINT,
		&ctx->worker2_task
	);
	assert(sc == RTEMS_SUCCESSFUL);

	sc = rtems_task_start(
		ctx->worker2_task,
		worker2_task,
		(rtems_task_argument) ctx
	);
	assert(sc == RTEMS_SUCCESSFUL);
}

static void
delete_worker(test_context *ctx)
{
	rtems_status_code sc;

	sc = rtems_task_delete(ctx->worker_task);
	assert(sc == RTEMS_SUCCESSFUL);

	sc = rtems_task_delete(ctx->worker2_task);
	assert(sc == RTEMS_SUCCESSFUL);
}

static void
test_rw_non_recursive(test_context *ctx)
{
	struct rwlock *rw = &ctx->rw;
	int ok;

	puts("test rw non-recursive");

	assert(!rw_initialized(rw));
	rw_init(rw, "test");
	assert(rw_initialized(rw));

	rw_rlock(rw);
	assert(!rw_wowned(rw));
	rw_runlock(rw);

	rw_rlock(rw);
	rw_unlock(rw);

	rw_rlock(rw);
	rw_rlock(rw);
	rw_runlock(rw);
	rw_runlock(rw);

	rw_rlock(rw);
	ok = rw_try_upgrade(rw);
	assert(ok != 0);
	assert(rw_wowned(rw));
	rw_wunlock(rw);

	rw_rlock(rw);
	ok = rw_try_upgrade(rw);
	assert(ok != 0);
	assert(rw_wowned(rw));
	rw_unlock(rw);

	rw_rlock(rw);
	ok = rw_try_upgrade(rw);
	assert(ok != 0);
	assert(rw_wowned(rw));
	rw_downgrade(rw);
	assert(!rw_wowned(rw));
	rw_runlock(rw);

	rw_rlock(rw);
	ok = rw_try_upgrade(rw);
	assert(ok != 0);
	assert(rw_wowned(rw));
	rw_downgrade(rw);
	assert(!rw_wowned(rw));
	rw_unlock(rw);

	rw_wlock(rw);
	assert(rw_wowned(rw));
	rw_wunlock(rw);

	rw_wlock(rw);
	rw_unlock(rw);

	ok = rw_try_rlock(rw);
	assert(ok != 0);
	rw_unlock(rw);

	ok = rw_try_wlock(rw);
	assert(ok != 0);
	rw_unlock(rw);

	rw_destroy(rw);
}

static void
test_rw_recursive(test_context *ctx)
{
	struct rwlock *rw = &ctx->rw;

	puts("test rw recursive");

	assert(!rw_initialized(rw));
	rw_init_flags(rw, "test", RW_RECURSE);
	assert(rw_initialized(rw));

	rw_wlock(rw);
	rw_wlock(rw);
	rw_wunlock(rw);
	rw_wunlock(rw);

	rw_destroy(rw);
}

static void
test_rw_try_rlock(test_context *ctx)
{
	struct rwlock *rw = &ctx->rw;

	puts("test rw try rlock");

	rw_init(rw, "test");

	rw_rlock(rw);
	ctx->done = false;
	ctx->rv = 0;
	send_events(ctx, EVENT_TRY_RLOCK);
	assert(ctx->done);
	assert(ctx->rv == 1);
	rw_unlock(rw);
	ctx->done = false;
	send_events(ctx, EVENT_UNLOCK);
	assert(ctx->done);

	rw_wlock(rw);
	ctx->done = false;
	ctx->rv = 1;
	send_events(ctx, EVENT_TRY_RLOCK);
	assert(ctx->done);
	assert(ctx->rv == 0);
	rw_unlock(rw);

	rw_destroy(rw);
}

static void
test_rw_try_wlock(test_context *ctx)
{
	struct rwlock *rw = &ctx->rw;

	puts("test rw try wlock");

	rw_init(rw, "test");

	rw_rlock(rw);
	ctx->done = false;
	ctx->rv = 1;
	send_events(ctx, EVENT_TRY_WLOCK);
	assert(ctx->done);
	assert(ctx->rv == 0);
	rw_unlock(rw);

	rw_wlock(rw);
	ctx->done = false;
	ctx->rv = 1;
	send_events(ctx, EVENT_TRY_WLOCK);
	assert(ctx->done);
	assert(ctx->rv == 0);
	rw_unlock(rw);

	rw_destroy(rw);
}

static void
test_rw_rlock(test_context *ctx)
{
	struct rwlock *rw = &ctx->rw;

	puts("test rw rlock");

	rw_init(rw, "test");

	rw_rlock(rw);
	ctx->done = false;
	send_events(ctx, EVENT_RLOCK);
	assert(ctx->done);
	rw_unlock(rw);
	assert(ctx->done);
	ctx->done = false;
	send_events(ctx, EVENT_UNLOCK);
	assert(ctx->done);

	rw_wlock(rw);
	ctx->done = false;
	send_events(ctx, EVENT_RLOCK);
	assert(!ctx->done);
	rw_unlock(rw);
	assert(ctx->done);
	ctx->done = false;
	send_events(ctx, EVENT_UNLOCK);
	assert(ctx->done);

	rw_destroy(rw);
}

static void
test_rw_wlock(test_context *ctx)
{
	struct rwlock *rw = &ctx->rw;

	puts("test rw rlock");

	rw_init(rw, "test");

	rw_rlock(rw);
	ctx->done = false;
	send_events(ctx, EVENT_WLOCK);
	assert(!ctx->done);
	rw_unlock(rw);
	assert(ctx->done);
	ctx->done = false;
	send_events(ctx, EVENT_UNLOCK);
	assert(ctx->done);

	rw_wlock(rw);
	ctx->done = false;
	send_events(ctx, EVENT_WLOCK);
	assert(!ctx->done);
	rw_unlock(rw);
	assert(ctx->done);
	ctx->done = false;
	send_events(ctx, EVENT_UNLOCK);
	assert(ctx->done);

	rw_destroy(rw);
}

static void
test_rw_rlock_phase_fair(test_context *ctx)
{
	struct rwlock *rw = &ctx->rw;

	puts("test rw rlock phase fair");

	rw_init(rw, "test");

	rw_rlock(rw);

	ctx->done = false;
	send_events(ctx, EVENT_WLOCK);
	assert(!ctx->done);

	ctx->done2 = false;
	send_events2(ctx, EVENT_RLOCK);
	assert(!ctx->done2);

	rw_unlock(rw);
	assert(ctx->done);
	assert(!ctx->done2);

	ctx->done = false;
	send_events(ctx, EVENT_UNLOCK);
	assert(ctx->done);
	assert(ctx->done2);

	ctx->done2 = false;
	send_events2(ctx, EVENT_UNLOCK);
	assert(ctx->done2);

	rw_destroy(rw);
}

static void
test_rw_wlock_phase_fair(test_context *ctx)
{
	struct rwlock *rw = &ctx->rw;

	puts("test rw wlock phase fair");

	rw_init(rw, "test");

	rw_wlock(rw);

	ctx->done = false;
	send_events(ctx, EVENT_WLOCK);
	assert(!ctx->done);

	ctx->done2 = false;
	send_events2(ctx, EVENT_RLOCK);
	assert(!ctx->done2);

	rw_unlock(rw);
	assert(!ctx->done);
	assert(ctx->done2);

	ctx->done2 = false;
	send_events2(ctx, EVENT_UNLOCK);
	assert(ctx->done2);
	assert(ctx->done);

	ctx->done = false;
	send_events(ctx, EVENT_UNLOCK);
	assert(ctx->done);

	rw_destroy(rw);
}

static void
test_rw_try_upgrade(test_context *ctx)
{
	struct rwlock *rw = &ctx->rw;
	int ok;

	puts("test rw try upgrade");

	rw_init(rw, "test");

	rw_rlock(rw);

	ctx->done = false;
	send_events(ctx, EVENT_WLOCK);
	assert(!ctx->done);

	assert(!rw_wowned(rw));
	ok = rw_try_upgrade(rw);
	assert(ok != 0);
	assert(rw_wowned(rw));
	assert(!ctx->done);

	rw_unlock(rw);
	assert(!rw_wowned(rw));
	assert(ctx->done);

	ctx->done = false;
	send_events(ctx, EVENT_UNLOCK);
	assert(ctx->done);

	rw_rlock(rw);

	ctx->done = false;
	send_events(ctx, EVENT_WLOCK);
	assert(!ctx->done);

	ctx->done2 = false;
	send_events2(ctx, EVENT_RLOCK);
	assert(!ctx->done2);

	assert(!rw_wowned(rw));
	ok = rw_try_upgrade(rw);
	assert(ok != 0);
	assert(rw_wowned(rw));
	assert(!ctx->done);
	assert(!ctx->done2);

	rw_unlock(rw);
	assert(!rw_wowned(rw));
	assert(!ctx->done);
	assert(ctx->done2);

	ctx->done2 = false;
	send_events2(ctx, EVENT_UNLOCK);
	assert(ctx->done2);
	assert(ctx->done);

	ctx->done = false;
	send_events(ctx, EVENT_UNLOCK);
	assert(ctx->done);

	rw_rlock(rw);

	ctx->done = false;
	send_events(ctx, EVENT_RLOCK);
	assert(ctx->done);

	assert(!rw_wowned(rw));
	ok = rw_try_upgrade(rw);
	assert(ok == 0);
	assert(!rw_wowned(rw));

	ctx->done = false;
	send_events(ctx, EVENT_UNLOCK);
	assert(ctx->done);

	assert(!rw_wowned(rw));
	ok = rw_try_upgrade(rw);
	assert(ok != 0);
	assert(rw_wowned(rw));

	rw_unlock(rw);
	assert(!rw_wowned(rw));

	rw_destroy(rw);
}

static void
test_rw_downgrade(test_context *ctx)
{
	struct rwlock *rw = &ctx->rw;

	puts("test rw downgrade");

	rw_init(rw, "test");

	rw_wlock(rw);
	assert(rw_wowned(rw));

	rw_downgrade(rw);
	assert(!rw_wowned(rw));

	rw_unlock(rw);
	assert(!rw_wowned(rw));

	rw_wlock(rw);
	assert(rw_wowned(rw));

	ctx->done = false;
	send_events(ctx, EVENT_RLOCK);
	assert(!ctx->done);

	rw_downgrade(rw);
	assert(!rw_wowned(rw));
	assert(ctx->done);

	rw_unlock(rw);
	assert(!rw_wowned(rw));

	ctx->done = false;
	send_events(ctx, EVENT_UNLOCK);
	assert(ctx->done);

	rw_destroy(rw);
}

static void
test_rw_sleep_with_rlock(test_context *ctx)
{
	struct rwlock *rw = &ctx->rw;

	puts("test rw sleep with rlock");

	rw_init(rw, "test");

	ctx->done = false;
	send_events(ctx, EVENT_RLOCK);
	assert(ctx->done);

	ctx->done = false;
	ctx->timo = 0;
	send_events(ctx, EVENT_SLEEP);
	assert(!ctx->done);

	rw_rlock(rw);
	wakeup(ctx);
	assert(ctx->done);
	rw_unlock(rw);

	ctx->done = false;
	send_events(ctx, EVENT_UNLOCK);
	assert(ctx->done);

	rw_destroy(rw);
}

static void
test_rw_sleep_with_wlock(test_context *ctx)
{
	struct rwlock *rw = &ctx->rw;

	puts("test rw sleep with wlock");

	rw_init(rw, "test");

	ctx->done = false;
	send_events(ctx, EVENT_WLOCK);
	assert(ctx->done);

	ctx->done = false;
	ctx->timo = 0;
	send_events(ctx, EVENT_SLEEP);
	assert(!ctx->done);

	rw_rlock(rw);
	wakeup(ctx);
	assert(!ctx->done);
	rw_unlock(rw);
	assert(ctx->done);

	ctx->done = false;
	send_events(ctx, EVENT_UNLOCK);
	assert(ctx->done);

	rw_destroy(rw);
}

static void
test_rw_sleep_timeout(test_context *ctx)
{
	struct rwlock *rw = &ctx->rw;
	rtems_status_code sc;

	puts("test rw sleep timeout");

	rw_init(rw, "test");

	ctx->done = false;
	send_events(ctx, EVENT_RLOCK);
	assert(ctx->done);

	ctx->done = false;
	ctx->timo = 2;
	send_events(ctx, EVENT_SLEEP);
	assert(!ctx->done);

	sc = rtems_task_wake_after(ctx->timo);
	assert(sc == RTEMS_SUCCESSFUL);

	assert(ctx->done);

	ctx->done = false;
	send_events(ctx, EVENT_UNLOCK);
	assert(ctx->done);

	rw_destroy(rw);
}

static void
alloc_basic_resources(void)
{
	curthread;
}

static void
test_main(void)
{
	rtems_resource_snapshot snapshot_0;
	rtems_resource_snapshot snapshot_1;

	test_context *ctx = &test_instance;

	alloc_basic_resources();

	rtems_resource_snapshot_take(&snapshot_0);

	set_self_prio(PRIO_MASTER);
	start_worker(ctx);

	rtems_resource_snapshot_take(&snapshot_1);

	test_rw_non_recursive(ctx);
	test_rw_recursive(ctx);
	test_rw_try_rlock(ctx);
	test_rw_try_wlock(ctx);
	test_rw_rlock(ctx);
	test_rw_wlock(ctx);
	test_rw_rlock_phase_fair(ctx);
	test_rw_wlock_phase_fair(ctx);
	test_rw_try_upgrade(ctx);
	test_rw_downgrade(ctx);

	assert(rtems_resource_snapshot_check(&snapshot_1));

	test_rw_sleep_with_rlock(ctx);
	test_rw_sleep_with_wlock(ctx);
	test_rw_sleep_timeout(ctx);

	delete_worker(ctx);

	assert(rtems_resource_snapshot_check(&snapshot_0));

	exit(0);
}

#include <rtems/bsd/test/default-init.h>
