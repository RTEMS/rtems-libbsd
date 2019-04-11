/*
 * Copyright (c) 2015 embedded brains GmbH.  All rights reserved.
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
#include <sys/condvar.h>

#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#include <rtems.h>
#include <rtems/test.h>

#define TEST_NAME "LIBBSD SMP 1"

#define CPU_COUNT 32

#define OBJ_COUNT (CPU_COUNT / 2)

typedef struct {
	uint32_t mtx_lock[CPU_COUNT];
	uint32_t mtx_try_success[CPU_COUNT];
	uint32_t mtx_try_failed[CPU_COUNT];
	uint32_t cv_signal[CPU_COUNT];
	uint32_t cv_broadcast[CPU_COUNT];
	uint32_t cv_timedwait_success[CPU_COUNT];
	uint32_t cv_timedwait_timeout[CPU_COUNT];
} test_stats;

typedef struct {
	rtems_test_parallel_context base;
	struct mtx mtx[OBJ_COUNT];
	struct cv cv[OBJ_COUNT];
	volatile uint32_t value[OBJ_COUNT];
	test_stats stats;
} test_context;

static test_context test_instance;

static rtems_interval
test_duration(void)
{

	return (10 * rtems_clock_get_ticks_per_second());
}

static uint32_t
simple_random(uint32_t v)
{
	v *= 1664525;
	v += 1013904223;

	return (v);
}

static rtems_interval
test_init(rtems_test_parallel_context *base, void *arg, size_t active_workers)
{

	return (test_duration());
}

static void
test_fini(rtems_test_parallel_context *base, void *arg, size_t active_workers)
{

	/* Do nothing */
}

static void
busy(void)
{
	int i;

	for (i = 0; i < 1000; ++i) {
		__asm__ volatile ("");
	}
}

static uint32_t
get_obj_count(void)
{
	return ((rtems_scheduler_get_processor_maximum() + 1) / 2);
}

static void
test_mtx_body(rtems_test_parallel_context *base, void *arg,
    size_t active_workers, size_t worker_index)
{
	test_context *ctx = (test_context *)base;
	uint32_t obj_count = get_obj_count();
	uint32_t r = worker_index;

	while (!rtems_test_parallel_stop_job(&ctx->base)) {
		uint32_t op = (r >> 17) % 2;
		uint32_t i = (r >> 7) % obj_count;
		struct mtx *mtx = &ctx->mtx[i];
		volatile uint32_t *value = &ctx->value[i];
		bool locked;

		switch (op) {
		case 0:
			locked = mtx_trylock(mtx) == 1;
			if (locked) {
				++ctx->stats.mtx_try_success[i];
			} else {
				++ctx->stats.mtx_try_failed[i];
			}

			break;
		case 1:
			mtx_lock(mtx);
			++ctx->stats.mtx_lock[i];
			locked = true;
			break;
		}

		if (locked) {
			assert(*value == i);
			*value = 0xdeadbeef;
			busy();
			*value = i;
			mtx_unlock(mtx);
		}

		r = simple_random(r);
	}
}

static void
test_cv_body(rtems_test_parallel_context *base, void *arg,
    size_t active_workers, size_t worker_index)
{
	test_context *ctx = (test_context *)base;
	uint32_t obj_count = get_obj_count();
	uint32_t r = worker_index;

	while (!rtems_test_parallel_stop_job(&ctx->base)) {
		uint32_t op = (r >> 17) % 3;
		uint32_t i = (r >> 7) % obj_count;
		struct mtx *mtx = &ctx->mtx[i];
		struct cv *cv = &ctx->cv[i];

		mtx_lock(mtx);

		switch (op) {
		case 0:
			cv_signal(cv);
			++ctx->stats.cv_signal[i];
			break;
		case 1:
			cv_broadcast(cv);
			++ctx->stats.cv_broadcast[i];
			break;
		case 2:
			if (cv_timedwait(cv, mtx, 1) == 0) {
				++ctx->stats.cv_timedwait_success[i];
			} else {
				++ctx->stats.cv_timedwait_timeout[i];
			}

			break;
		}

		mtx_unlock(mtx);

		r = simple_random(r);
	}
}

static const rtems_test_parallel_job test_jobs[] = {
	{
		.init = test_init,
		.body = test_cv_body,
		.fini = test_fini
	}, {
		.init = test_init,
		.body = test_mtx_body,
		.fini = test_fini
	}
};

static void setup_worker(
  rtems_test_parallel_context *base,
  size_t worker_index,
  rtems_id worker_id
)
{
	rtems_task_priority prio;
	rtems_status_code sc;

	prio = ((worker_index - 1) % 2) + 253;
	sc = rtems_task_set_priority(worker_id, prio, &prio);
	assert(sc == RTEMS_SUCCESSFUL);
}

static void
print_summary(const test_context *ctx)
{
	size_t i;

	for (i = 0; i < get_obj_count(); ++i) {
		printf("mtx lock[%zu]: %" PRIu32 "\n", i,
		    ctx->stats.mtx_lock[i]);
		printf("mtx try success[%zu]: %" PRIu32 "\n", i,
		    ctx->stats.mtx_try_success[i]);
		printf("mtx try failed[%zu]: %" PRIu32 "\n", i,
		    ctx->stats.mtx_try_failed[i]);
	}

	for (i = 0; i < get_obj_count(); ++i) {
		printf("cv signal[%zu]: %" PRIu32 "\n", i,
		    ctx->stats.cv_signal[i]);
		printf("cv broadcast[%zu]: %" PRIu32 "\n", i,
		    ctx->stats.cv_broadcast[i]);
		printf("cv wait success[%zu]: %" PRIu32 "\n", i,
		    ctx->stats.cv_timedwait_success[i]);
		printf("cv wait timeout[%zu]: %" PRIu32 "\n", i,
		    ctx->stats.cv_timedwait_timeout[i]);
	}
}

static void
test_main(void)
{
	test_context *ctx = &test_instance;
	size_t i;

	for (i = 0; i < OBJ_COUNT; ++i) {
		ctx->value[i] = i;
		mtx_init(&ctx->mtx[i], "test", NULL, MTX_DEF);
		cv_init(&ctx->cv[i], "test");
	}

	rtems_test_parallel(&ctx->base, setup_worker, &test_jobs[0],
	    RTEMS_ARRAY_SIZE(test_jobs));

	print_summary(ctx);

	for (i = 0; i < OBJ_COUNT; ++i) {
		mtx_destroy(&ctx->mtx[i]);
		cv_destroy(&ctx->cv[i]);
	}

	exit(0);
}

#define CONFIGURE_MAXIMUM_PROCESSORS CPU_COUNT

#include <rtems/bsd/test/default-init.h>
