/*
 * Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
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
#include <sys/mutex.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <rtems/libcsupport.h>
#include <rtems.h>

#define TEST_NAME "LIBBSD MUTEX 1"

#define PRIO_MASTER 3

#define EVENT_LOCK RTEMS_EVENT_0

#define EVENT_TRY_LOCK RTEMS_EVENT_1

#define EVENT_UNLOCK RTEMS_EVENT_2

#define EVENT_SLEEP RTEMS_EVENT_3

#define WORKER_COUNT 2

typedef struct {
	struct mtx mtx;
	struct mtx mtx2;
	int rv;
	int timo;
	rtems_id worker_task[WORKER_COUNT];
	bool done[WORKER_COUNT];
} test_context;

static test_context test_instance;

static const rtems_task_priority prio_worker[2] = { 2, 1 };

static void
set_self_prio(rtems_task_priority prio)
{
	rtems_status_code sc;

	sc = rtems_task_set_priority(RTEMS_SELF, prio, &prio);
	assert(sc == RTEMS_SUCCESSFUL);
}

static rtems_task_priority
get_self_prio(void)
{
	rtems_status_code sc;
	rtems_task_priority prio;

	sc = rtems_task_set_priority(RTEMS_SELF, RTEMS_CURRENT_PRIORITY, &prio);
	assert(sc == RTEMS_SUCCESSFUL);

	return prio;
}

static void
worker_task(rtems_task_argument index)
{
	test_context *ctx = &test_instance;
	struct mtx *mtx = &ctx->mtx;

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

		if ((events & EVENT_LOCK) != 0) {
			mtx_lock(mtx);
			ctx->done[index] = true;
		}

		if ((events & EVENT_TRY_LOCK) != 0) {
			ctx->rv = mtx_trylock(mtx);
			ctx->done[index] = true;
		}

		if ((events & EVENT_UNLOCK) != 0) {
			mtx_unlock(mtx);
			ctx->done[index] = true;
		}

		if ((events & EVENT_SLEEP) != 0) {
			ctx->rv = mtx_sleep(ctx, mtx, 0, "worker", ctx->timo);
			ctx->done[index] = true;
		}
	}
}

static void
send_events(test_context *ctx, rtems_event_set events, size_t index)
{
	rtems_status_code sc;

	sc = rtems_event_send(ctx->worker_task[index], events);
	assert(sc == RTEMS_SUCCESSFUL);
}

static void
start_worker(test_context *ctx)
{
	size_t i;

	for (i = 0; i < WORKER_COUNT; ++i) {
		rtems_status_code sc;

		sc = rtems_task_create(
			rtems_build_name('W', 'O', 'R', 'K'),
			prio_worker[i],
			RTEMS_MINIMUM_STACK_SIZE,
			RTEMS_DEFAULT_MODES,
			RTEMS_FLOATING_POINT,
			&ctx->worker_task[i]
		);
		assert(sc == RTEMS_SUCCESSFUL);

		sc = rtems_task_start(
			ctx->worker_task[i],
			worker_task,
			i
		);
		assert(sc == RTEMS_SUCCESSFUL);
	}
}

static void
delete_worker(test_context *ctx)
{
	size_t i;

	for (i = 0; i < WORKER_COUNT; ++i) {
		rtems_status_code sc;

		sc = rtems_task_delete(ctx->worker_task[i]);
		assert(sc == RTEMS_SUCCESSFUL);
	}
}

static void
test_mtx_non_recursive(test_context *ctx)
{
	struct mtx *mtx = &ctx->mtx;

	puts("test mtx non-recursive");

	assert(!mtx_initialized(mtx));
	mtx_init(mtx, "test", NULL, MTX_DEF);
	assert(mtx_initialized(mtx));

	assert(!mtx_owned(mtx));
	assert(!mtx_recursed(mtx));
	mtx_lock(mtx);

	assert(mtx_owned(mtx));
	assert(!mtx_recursed(mtx));

	mtx_unlock(mtx);
	assert(!mtx_owned(mtx));
	assert(!mtx_recursed(mtx));

	mtx_destroy(mtx);
	assert(!mtx_initialized(mtx));
}

static void
test_mtx_recursive(test_context *ctx)
{
	struct mtx *mtx = &ctx->mtx;

	puts("test mtx recursive");

	assert(!mtx_initialized(mtx));
	mtx_init(mtx, "test", NULL, MTX_DEF | MTX_RECURSE);
	assert(mtx_initialized(mtx));

	assert(!mtx_owned(mtx));
	assert(!mtx_recursed(mtx));
	mtx_lock(mtx);

	assert(mtx_owned(mtx));
	assert(!mtx_recursed(mtx));
	mtx_lock(mtx);

	assert(mtx_owned(mtx));
	assert(mtx_recursed(mtx));
	mtx_lock(mtx);

	assert(mtx_owned(mtx));
	assert(mtx_recursed(mtx));

	mtx_unlock(mtx);
	assert(mtx_owned(mtx));
	assert(mtx_recursed(mtx));

	mtx_unlock(mtx);
	assert(mtx_owned(mtx));
	assert(!mtx_recursed(mtx));

	mtx_unlock(mtx);
	assert(!mtx_owned(mtx));
	assert(!mtx_recursed(mtx));

	mtx_destroy(mtx);
	assert(!mtx_initialized(mtx));
}

static void
test_mtx_trylock(test_context *ctx)
{
	size_t index = 0;
	struct mtx *mtx = &ctx->mtx;
	int ok;

	puts("test mtx try lock");

	assert(!mtx_initialized(mtx));
	mtx_init(mtx, "test", NULL, MTX_DEF);
	assert(mtx_initialized(mtx));

	assert(!mtx_owned(mtx));
	assert(!mtx_recursed(mtx));
	ok = mtx_trylock(mtx);
	assert(ok != 0);
	assert(mtx_owned(mtx));
	assert(!mtx_recursed(mtx));

	mtx_unlock(mtx);
	assert(!mtx_owned(mtx));
	assert(!mtx_recursed(mtx));

	assert(!mtx_owned(mtx));
	assert(!mtx_recursed(mtx));
	mtx_lock(mtx);
	assert(mtx_owned(mtx));
	assert(!mtx_recursed(mtx));

	ctx->done[index] = false;
	ctx->rv = 1;
	send_events(ctx, EVENT_TRY_LOCK, index);
	assert(ctx->done[index]);
	assert(ctx->rv == 0);

	assert(mtx_owned(mtx));
	assert(!mtx_recursed(mtx));
	mtx_unlock(mtx);
	assert(!mtx_owned(mtx));
	assert(!mtx_recursed(mtx));

	mtx_destroy(mtx);
	assert(!mtx_initialized(mtx));
}

static void
test_mtx_lock(test_context *ctx)
{
	struct mtx *mtx = &ctx->mtx;
	struct mtx *mtx2 = &ctx->mtx2;
	size_t low = 0;
	size_t high = 1;

	puts("test mtx lock");

	assert(!mtx_initialized(mtx));
	mtx_init(mtx, "test", NULL, MTX_DEF);
	assert(mtx_initialized(mtx));

	assert(!mtx_initialized(mtx2));
	mtx_init(mtx2, "test 2", NULL, MTX_DEF);
	assert(mtx_initialized(mtx2));

	/* Resource count one */

	assert(!mtx_owned(mtx));
	assert(!mtx_recursed(mtx));
	mtx_lock(mtx);
	assert(mtx_owned(mtx));
	assert(!mtx_recursed(mtx));
	assert(get_self_prio() == PRIO_MASTER);

	ctx->done[low] = false;
	ctx->done[high] = false;

	send_events(ctx, EVENT_LOCK, low);
	assert(!ctx->done[low]);
	assert(!ctx->done[high]);
	assert(mtx_owned(mtx));
	assert(!mtx_recursed(mtx));
	assert(get_self_prio() == prio_worker[low]);

	send_events(ctx, EVENT_LOCK, high);
	assert(!ctx->done[low]);
	assert(!ctx->done[high]);
	assert(mtx_owned(mtx));
	assert(!mtx_recursed(mtx));
	assert(get_self_prio() == prio_worker[high]);

	mtx_unlock(mtx);
	assert(!ctx->done[low]);
	assert(ctx->done[high]);
	assert(!mtx_owned(mtx));
	assert(!mtx_recursed(mtx));
	assert(get_self_prio() == PRIO_MASTER);

	ctx->done[high] = false;
	send_events(ctx, EVENT_UNLOCK, high);
	assert(ctx->done[low]);
	assert(ctx->done[high]);

	ctx->done[low] = false;
	send_events(ctx, EVENT_UNLOCK, low);
	assert(ctx->done[low]);

	/* Resource count two */

	assert(!mtx_owned(mtx));
	assert(!mtx_recursed(mtx));
	mtx_lock(mtx);
	assert(mtx_owned(mtx));
	assert(!mtx_recursed(mtx));
	assert(get_self_prio() == PRIO_MASTER);

	assert(!mtx_owned(mtx2));
	assert(!mtx_recursed(mtx2));
	mtx_lock(mtx2);
	assert(mtx_owned(mtx2));
	assert(!mtx_recursed(mtx2));
	assert(get_self_prio() == PRIO_MASTER);

	ctx->done[low] = false;
	send_events(ctx, EVENT_LOCK, low);
	assert(!ctx->done[low]);
	assert(mtx_owned(mtx));
	assert(!mtx_recursed(mtx));
	assert(get_self_prio() == prio_worker[low]);

	mtx_unlock(mtx2);
	assert(!mtx_owned(mtx2));
	assert(!mtx_recursed(mtx2));
	assert(get_self_prio() == prio_worker[low]);

	mtx_unlock(mtx);
	assert(ctx->done[low]);
	assert(!mtx_owned(mtx));
	assert(!mtx_recursed(mtx));
	assert(get_self_prio() == PRIO_MASTER);

	ctx->done[low] = false;
	send_events(ctx, EVENT_UNLOCK, low);
	assert(ctx->done[low]);

	mtx_destroy(mtx2);
	assert(!mtx_initialized(mtx2));

	mtx_destroy(mtx);
	assert(!mtx_initialized(mtx));
}

static void
test_mtx_sleep_with_lock(test_context *ctx)
{
	size_t index = 0;
	struct mtx *mtx = &ctx->mtx;

	puts("test mtx sleep with lock");

	assert(!mtx_initialized(mtx));
	mtx_init(mtx, "test", NULL, MTX_DEF);
	assert(mtx_initialized(mtx));

	ctx->done[index] = false;
	send_events(ctx, EVENT_LOCK, index);
	assert(ctx->done[index]);

	ctx->done[index] = false;
	ctx->timo = 0;
	send_events(ctx, EVENT_SLEEP, index);
	assert(!ctx->done[index]);

	assert(!mtx_owned(mtx));
	assert(!mtx_recursed(mtx));
	mtx_lock(mtx);
	assert(mtx_owned(mtx));
	assert(!mtx_recursed(mtx));

	wakeup(ctx);
	assert(!ctx->done[index]);

	mtx_unlock(mtx);
	assert(ctx->done[index]);
	assert(!mtx_owned(mtx));
	assert(!mtx_recursed(mtx));

	ctx->done[index] = false;
	send_events(ctx, EVENT_UNLOCK, index);
	assert(ctx->done[index]);

	mtx_destroy(mtx);
	assert(!mtx_initialized(mtx));
}

static void
test_mtx_sleep_timeout(test_context *ctx)
{
	size_t index = 0;
	struct mtx *mtx = &ctx->mtx;
	rtems_status_code sc;

	puts("test mtx sleep timeout");

	assert(!mtx_initialized(mtx));
	mtx_init(mtx, "test", NULL, MTX_DEF);
	assert(mtx_initialized(mtx));

	ctx->done[index] = false;
	send_events(ctx, EVENT_LOCK, index);
	assert(ctx->done[index]);

	ctx->done[index] = false;
	ctx->timo = 2;
	send_events(ctx, EVENT_SLEEP, index);
	assert(!ctx->done[index]);

	sc = rtems_task_wake_after(ctx->timo);
	assert(sc == RTEMS_SUCCESSFUL);
	assert(ctx->done[index]);

	ctx->done[index] = false;
	send_events(ctx, EVENT_UNLOCK, index);
	assert(ctx->done[index]);

	mtx_destroy(mtx);
	assert(!mtx_initialized(mtx));
}

static void
alloc_basic_resources(void)
{
	curthread;
}

static void
test_main(void)
{
	test_context *ctx = &test_instance;
	rtems_resource_snapshot snapshot_0;
	rtems_resource_snapshot snapshot_1;

	alloc_basic_resources();

	rtems_resource_snapshot_take(&snapshot_0);

	set_self_prio(PRIO_MASTER);
	start_worker(ctx);

	rtems_resource_snapshot_take(&snapshot_1);

	test_mtx_non_recursive(ctx);
	test_mtx_recursive(ctx);
	test_mtx_trylock(ctx);
	test_mtx_lock(ctx);

	assert(rtems_resource_snapshot_check(&snapshot_1));

	test_mtx_sleep_with_lock(ctx);
	test_mtx_sleep_timeout(ctx);

	delete_worker(ctx);

	assert(rtems_resource_snapshot_check(&snapshot_0));

	exit(0);
}

#include <rtems/bsd/test/default-init.h>
