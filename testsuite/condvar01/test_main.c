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

#include <rtems/bsd/sys/param.h>
#include <rtems/bsd/sys/types.h>
#include <sys/systm.h>
#include <rtems/bsd/sys/lock.h>
#include <sys/mutex.h>
#include <sys/condvar.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <rtems/libcsupport.h>
#include <rtems.h>

#define TEST_NAME "LIBBSD CONDVAR 1"

#define PRIO_MASTER 3

#define EVENT_LOCK RTEMS_EVENT_0

#define EVENT_UNLOCK RTEMS_EVENT_1

#define EVENT_WAIT RTEMS_EVENT_2

#define EVENT_WAIT_SIG RTEMS_EVENT_3

#define EVENT_WAIT_UNLOCK RTEMS_EVENT_4

#define EVENT_TIMEDWAIT RTEMS_EVENT_5

#define EVENT_TIMEDWAIT_SIG RTEMS_EVENT_6

#define WORKER_COUNT 2

typedef struct {
	struct mtx mtx;
	struct cv cv;
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

static void
worker_task(rtems_task_argument index)
{
	test_context *ctx = &test_instance;
	struct mtx *mtx = &ctx->mtx;
	struct cv *cv = &ctx->cv;

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

		if ((events & EVENT_UNLOCK) != 0) {
			mtx_unlock(mtx);
			ctx->done[index] = true;
		}

		if ((events & EVENT_WAIT) != 0) {
			cv_wait(cv, mtx);
			ctx->done[index] = true;
		}

		if ((events & EVENT_WAIT_SIG) != 0) {
			int rv = cv_wait_sig(cv, mtx);
			assert(rv == 0);
			ctx->done[index] = true;
		}

		if ((events & EVENT_WAIT_UNLOCK) != 0) {
			cv_wait_unlock(cv, mtx);
			ctx->done[index] = true;
		}

		if ((events & EVENT_TIMEDWAIT) != 0) {
			ctx->rv = cv_timedwait(cv, mtx, ctx->timo);
			ctx->done[index] = true;
		}

		if ((events & EVENT_TIMEDWAIT_SIG) != 0) {
			ctx->rv = cv_timedwait_sig(cv, mtx, ctx->timo);
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
test_cv_wait_signal(test_context *ctx, rtems_event_set event, const char *name)
{
	struct mtx *mtx = &ctx->mtx;
	struct cv *cv = &ctx->cv;
	size_t low = 0;
	size_t high = 1;

	printf("test cv %s signal\n", name);

	mtx_lock(mtx);
	mtx_unlock(mtx);

	ctx->timo = 0;

	ctx->done[low] = false;
	send_events(ctx, EVENT_LOCK, low);
	assert(ctx->done[low]);

	ctx->done[high] = false;
	send_events(ctx, EVENT_LOCK, high);
	assert(!ctx->done[high]);

	ctx->done[low] = false;
	send_events(ctx, event, low);
	assert(!ctx->done[low]);
	assert(ctx->done[high]);

	ctx->done[low] = false;
	ctx->done[high] = false;
	send_events(ctx, event, high);
	assert(!ctx->done[low]);
	assert(!ctx->done[high]);

	cv_signal(cv);
	assert(!ctx->done[low]);
	assert(ctx->done[high]);

	if (event != EVENT_WAIT_UNLOCK) {
		cv_signal(cv);
		assert(!ctx->done[low]);

		ctx->done[high] = false;
		send_events(ctx, EVENT_UNLOCK, high);
		assert(ctx->done[high]);
		assert(ctx->done[low]);

		ctx->done[low] = false;
		send_events(ctx, EVENT_UNLOCK, low);
		assert(ctx->done[low]);
	} else {
		cv_signal(cv);
		assert(ctx->done[low]);
	}

	mtx_lock(mtx);
	mtx_unlock(mtx);
}

static void
test_cv_wait_broadcast(test_context *ctx, rtems_event_set event, const char *name)
{
	struct mtx *mtx = &ctx->mtx;
	struct cv *cv = &ctx->cv;
	size_t low = 0;
	size_t high = 1;

	printf("test cv %s broadcast\n", name);

	mtx_lock(mtx);
	mtx_unlock(mtx);

	ctx->timo = 0;

	ctx->done[low] = false;
	send_events(ctx, EVENT_LOCK, low);
	assert(ctx->done[low]);

	ctx->done[high] = false;
	send_events(ctx, EVENT_LOCK, high);
	assert(!ctx->done[high]);

	ctx->done[low] = false;
	send_events(ctx, event, low);
	assert(!ctx->done[low]);
	assert(ctx->done[high]);

	ctx->done[low] = false;
	ctx->done[high] = false;
	send_events(ctx, event, high);
	assert(!ctx->done[low]);
	assert(!ctx->done[high]);

	if (event != EVENT_WAIT_UNLOCK) {
		cv_broadcast(cv);
		assert(!ctx->done[low]);

		ctx->done[high] = false;
		send_events(ctx, EVENT_UNLOCK, high);
		assert(ctx->done[high]);
		assert(ctx->done[low]);

		ctx->done[low] = false;
		send_events(ctx, EVENT_UNLOCK, low);
		assert(ctx->done[low]);
	} else {
		cv_broadcast(cv);
		assert(ctx->done[low]);
		assert(ctx->done[high]);
	}

	mtx_lock(mtx);
	mtx_unlock(mtx);
}

static void
test_cv_wait(test_context *ctx, rtems_event_set event, const char *name)
{
	test_cv_wait_signal(ctx, event, name);
	test_cv_wait_broadcast(ctx, event, name);
}

static void
test_cv_wait_timeout(test_context *ctx)
{
	size_t index = 0;
	rtems_status_code sc;

	puts("test cv wait timeout");

	ctx->timo = 2;

	ctx->done[index] = false;
	send_events(ctx, EVENT_LOCK, index);
	assert(ctx->done[index]);

	ctx->done[index] = false;
	ctx->rv = 0;
	send_events(ctx, EVENT_TIMEDWAIT, index);
	assert(!ctx->done[index]);

	sc = rtems_task_wake_after(ctx->timo);
	assert(sc == RTEMS_SUCCESSFUL);
	assert(ctx->done[index]);
	assert(ctx->rv == EWOULDBLOCK);

	ctx->done[index] = false;
	ctx->rv = 0;
	send_events(ctx, EVENT_TIMEDWAIT_SIG, index);
	assert(!ctx->done[index]);

	sc = rtems_task_wake_after(ctx->timo);
	assert(sc == RTEMS_SUCCESSFUL);
	assert(ctx->done[index]);
	assert(ctx->rv == EWOULDBLOCK);

	ctx->done[index] = false;
	send_events(ctx, EVENT_UNLOCK, index);
	assert(ctx->done[index]);
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
	rtems_resource_snapshot snapshot;

	alloc_basic_resources();

	rtems_resource_snapshot_take(&snapshot);

	mtx_init(&ctx->mtx, "test", NULL, MTX_DEF);
	cv_init(&ctx->cv, "test");
	assert(strcmp(cv_wmesg(&ctx->cv), "test") == 0);

	set_self_prio(PRIO_MASTER);
	start_worker(ctx);

	test_cv_wait(ctx, EVENT_WAIT, "wait");
	test_cv_wait(ctx, EVENT_WAIT_SIG, "wait sig");
	test_cv_wait(ctx, EVENT_WAIT_UNLOCK, "wait unlock");
	test_cv_wait(ctx, EVENT_TIMEDWAIT, "timed wait");
	test_cv_wait(ctx, EVENT_TIMEDWAIT_SIG, "timed wait sig");

	test_cv_wait_timeout(ctx);

	delete_worker(ctx);
	cv_destroy(&ctx->cv);

	assert(rtems_resource_snapshot_check(&snapshot));

	exit(0);
}

#include <rtems/bsd/test/default-init.h>
