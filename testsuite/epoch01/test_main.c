/**
 * @file
 *
 * The following epoch functions are tested with parallel tasks: epoch_enter(),
 * epoch_exit(), epoch_call(), epoch_wait(), epoch_enter_preempt(),
 * epoch_exit_preempt(), epoch_wait_preempt() and epoch_drain_callbacks().
 */

/*
 * Copyright (c) 2018 embedded brains GmbH.  All rights reserved.
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
#include <sys/epoch.h>

#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ck_queue.h>

#include <rtems.h>
#include <rtems/cpuuse.h>
#include <rtems/test.h>
#include <rtems/thread.h>

#define TEST_NAME "LIBBSD EPOCH 1"

#define TEST_XML_NAME "TestEpoch01"

#define CPU_COUNT 32

typedef struct {
	uint32_t counter[CPU_COUNT];
	uint32_t removals[CPU_COUNT];
	uint32_t item_counter[CPU_COUNT][CPU_COUNT];
} test_stats;

typedef struct test_item {
	CK_SLIST_ENTRY(test_item) link;
	struct epoch_context ec;
	size_t index;
	size_t worker_index;
} test_item;

typedef struct {
	rtems_test_parallel_context base;
	size_t active_workers;
	CK_SLIST_HEAD(, test_item) item_list;
	rtems_mutex mtx[2];
	test_item items[CPU_COUNT];
	test_stats stats;
} test_context;

static test_context test_instance;

static rtems_interval
test_duration(void)
{

	return (1 * rtems_clock_get_ticks_per_second());
}

static rtems_interval
test_init(rtems_test_parallel_context *base, void *arg, size_t active_workers)
{
	test_context *ctx;

	ctx = (test_context *)base;
	memset(&ctx->stats, 0, sizeof(ctx->stats));
	return (test_duration());
}

static void
test_fini(rtems_test_parallel_context *base, const char *name,
    size_t active_workers)
{
	rtems_status_code sc;
	test_context *ctx;
	size_t i;

	sc = rtems_task_wake_after(CK_EPOCH_LENGTH);
	assert(sc == RTEMS_SUCCESSFUL);

	ctx = (test_context *)base;

	printf("  <%s activeWorker=\"%zu\">\n", name, active_workers);

	for (i = 0; i < active_workers; ++i) {
		printf("    <Counter worker=\"%zu\">%" PRIu32 "</Counter>\n",
		    i, ctx->stats.counter[i]);
	}

	for (i = 0; i < active_workers; ++i) {
		uint32_t r;

		r = ctx->stats.removals[i];
		if (r > 0) {
			printf("    <Removals worker=\"%zu\">%" PRIu32
			    "</Removals>\n", i, r);
		}
	}

	printf("  </%s>\n", name);
}

static void
test_enter_exit_body(rtems_test_parallel_context *base, void *arg,
    size_t active_workers, size_t worker_index)
{
	test_context *ctx = (test_context *)base;
	epoch_t e = global_epoch;
	uint32_t counter;

	ctx = (test_context *)base;
	e = global_epoch;
	counter = 0;

	while (!rtems_test_parallel_stop_job(&ctx->base)) {
		epoch_enter(e);
		++counter;
		epoch_exit(e);
	}

	ctx->stats.counter[worker_index] = counter;
}

static void
test_enter_exit_fini(rtems_test_parallel_context *base, void *arg,
    size_t active_workers)
{

	test_fini(base, "EnterExit", active_workers);
}

static rtems_interval
test_list_init(rtems_test_parallel_context *base, void *arg,
    size_t active_workers)
{
	test_context *ctx;
	size_t i;

	ctx = (test_context *)base;
	ctx->active_workers = active_workers;
	CK_SLIST_INIT(&ctx->item_list);

	for (i = 0; i < active_workers; ++i) {
		test_item *item = &ctx->items[i];

		CK_SLIST_INSERT_HEAD(&ctx->item_list, item, link);
		item->index = i;

		if (i == 0) {
			item->worker_index = 0;
		} else {
			item->worker_index = CPU_COUNT;
		}
	}

	return (test_init(&ctx->base, arg, active_workers));
}

static void
test_list_callback(epoch_context_t ec)
{
	test_context *ctx;
	test_item *item;
	test_item *next;

	ctx = &test_instance;
	item = __containerof(ec, struct test_item, ec);
	item->worker_index = (item->worker_index + 1) % ctx->active_workers;
	next = CK_SLIST_NEXT(item, link);

	if (next != NULL) {
		CK_SLIST_INSERT_AFTER(next, item, link);
	} else {
		CK_SLIST_INSERT_HEAD(&ctx->item_list, item, link);
	}
}

static test_item *
test_remove_item(test_context *ctx, uint32_t item_counter[CPU_COUNT],
    uint32_t *removals, size_t worker_index)
{
	test_item *prev;
	test_item *item;
	test_item *tmp;
	test_item *rm;

	prev = NULL;
	rm = NULL;
	CK_SLIST_FOREACH_SAFE(item, &ctx->item_list, link, tmp) {
		++item_counter[item->index];

		if (item->worker_index == worker_index) {
			++(*removals);
			rm = item;

			if (prev != NULL) {
				CK_SLIST_REMOVE_AFTER(prev, link);
			} else {
				CK_SLIST_REMOVE_HEAD(&ctx->item_list,
				    link);
			}
		}

		prev = item;
	}

	return rm;
}

static void
test_enter_list_op_exit_body(rtems_test_parallel_context *base, void *arg,
    size_t active_workers, size_t worker_index)
{
	test_context *ctx;
	epoch_t e;
	uint32_t counter;
	uint32_t removals;
	uint32_t item_counter[CPU_COUNT];

	ctx = (test_context *)base;
	e = global_epoch;
	counter = 0;
	removals = 0;
	memset(item_counter, 0, sizeof(item_counter));

	while (!rtems_test_parallel_stop_job(&ctx->base)) {
		test_item *rm;

		epoch_enter(e);
		++counter;
		rm = test_remove_item(ctx, item_counter, &removals,
		    worker_index);
		epoch_exit(e);

		if (rm != NULL) {
			epoch_call(e, &rm->ec, test_list_callback);
			epoch_wait(e);
		}
	}

	ctx->stats.counter[worker_index] = counter;
	ctx->stats.removals[worker_index] = removals;
	memcpy(ctx->stats.item_counter[worker_index], item_counter,
	    sizeof(ctx->stats.item_counter[worker_index]));
}

static void
test_enter_list_op_exit_fini(rtems_test_parallel_context *base, void *arg,
    size_t active_workers)
{

	test_fini(base, "EnterListOpExit", active_workers);
}

static void
test_enter_exit_preempt_body(rtems_test_parallel_context *base, void *arg,
    size_t active_workers, size_t worker_index)
{
	test_context *ctx = (test_context *)base;
	epoch_t e = global_epoch;
	uint32_t counter;

	ctx = (test_context *)base;
	e = global_epoch;
	counter = 0;

	while (!rtems_test_parallel_stop_job(&ctx->base)) {
		struct epoch_tracker et;

		epoch_enter_preempt(e, &et);
		++counter;
		epoch_exit_preempt(e, &et);
	}

	ctx->stats.counter[worker_index] = counter;
}

static void
test_enter_exit_preempt_fini(rtems_test_parallel_context *base,
    void *arg, size_t active_workers)
{

	test_fini(base, "EnterExitPreempt", active_workers);
}

static void
test_enter_list_op_exit_preempt_body(rtems_test_parallel_context *base,
    void *arg, size_t active_workers, size_t worker_index)
{
	test_context *ctx;
	epoch_t e;
	uint32_t counter;
	uint32_t removals;
	uint32_t item_counter[CPU_COUNT];

	ctx = (test_context *)base;
	e = global_epoch;
	counter = 0;
	removals = 0;
	memset(item_counter, 0, sizeof(item_counter));

	while (!rtems_test_parallel_stop_job(&ctx->base)) {
		struct epoch_tracker et;
		test_item *rm;

		epoch_enter_preempt(e, &et);
		++counter;
		rm = test_remove_item(ctx, item_counter, &removals,
		    worker_index);
		epoch_exit_preempt(e, &et);

		if (rm != NULL) {
			epoch_call(e, &rm->ec, test_list_callback);
			epoch_wait_preempt(e);
		}
	}

	ctx->stats.counter[worker_index] = counter;
	ctx->stats.removals[worker_index] = removals;
	memcpy(ctx->stats.item_counter[worker_index], item_counter,
	    sizeof(ctx->stats.item_counter[worker_index]));
}

static void
test_enter_list_op_exit_preempt_fini(rtems_test_parallel_context *base,
    void *arg, size_t active_workers)
{

	test_fini(base, "EnterListOpExitPreempt", active_workers);
}

static void
test_enter_list_op_exit_drain_body(rtems_test_parallel_context *base,
    void *arg, size_t active_workers, size_t worker_index)
{
	test_context *ctx;
	epoch_t e;
	uint32_t counter;
	uint32_t removals;
	uint32_t item_counter[CPU_COUNT];

	ctx = (test_context *)base;
	e = global_epoch;
	counter = 0;
	removals = 0;
	memset(item_counter, 0, sizeof(item_counter));

	while (!rtems_test_parallel_stop_job(&ctx->base)) {
		test_item *rm;

		epoch_enter(e);
		++counter;
		rm = test_remove_item(ctx, item_counter, &removals,
		    worker_index);
		epoch_exit(e);

		if (rm != NULL) {
			epoch_call(e, &rm->ec, test_list_callback);
			epoch_drain_callbacks(e);
		}
	}

	ctx->stats.counter[worker_index] = counter;
	ctx->stats.removals[worker_index] = removals;
	memcpy(ctx->stats.item_counter[worker_index], item_counter,
	    sizeof(ctx->stats.item_counter[worker_index]));
}

static void
test_enter_list_op_exit_drain_fini(rtems_test_parallel_context *base,
    void *arg, size_t active_workers)
{

	test_fini(base, "EnterListOpExitDrain", active_workers);
}

static void
test_thread_local_mutex_body(rtems_test_parallel_context *base, void *arg,
    size_t active_workers, size_t worker_index)
{
	test_context *ctx = (test_context *)base;
	rtems_mutex mtx;
	uint32_t counter;

	ctx = (test_context *)base;
	rtems_mutex_init(&mtx, "test");
	counter = 0;

	while (!rtems_test_parallel_stop_job(&ctx->base)) {
		rtems_mutex_lock(&mtx);
		++counter;
		rtems_mutex_unlock(&mtx);
	}

	rtems_mutex_destroy(&mtx);
	ctx->stats.counter[worker_index] = counter;
}

static void
test_thread_local_mutex_fini(rtems_test_parallel_context *base,
    void *arg, size_t active_workers)
{

	test_fini(base, "ThreadLocalMutex", active_workers);
}

static void
test_enter_mutex_exit_preempt_body(rtems_test_parallel_context *base,
    void *arg, size_t active_workers, size_t worker_index)
{
	test_context *ctx = (test_context *)base;
	epoch_t e = global_epoch;
	uint32_t counter;
	rtems_mutex *mtx;

	ctx = (test_context *)base;
	e = global_epoch;
	counter = 0;
	mtx = &ctx->mtx[worker_index % RTEMS_ARRAY_SIZE(ctx->mtx)];

	while (!rtems_test_parallel_stop_job(&ctx->base)) {
		struct epoch_tracker et;

		epoch_enter_preempt(e, &et);
		rtems_mutex_lock(mtx);
		++counter;
		rtems_mutex_unlock(mtx);
		epoch_exit_preempt(e, &et);
	}

	ctx->stats.counter[worker_index] = counter;
}

static void
test_enter_mutex_exit_preempt_fini(rtems_test_parallel_context *base,
    void *arg, size_t active_workers)
{

	test_fini(base, "EnterMutexExitPreempt", active_workers);
}

static const rtems_test_parallel_job test_jobs[] = {
	{
		.init = test_init,
		.body = test_enter_exit_body,
		.fini = test_enter_exit_fini,
		.cascade = true
	}, {
		.init = test_list_init,
		.body = test_enter_list_op_exit_body,
		.fini = test_enter_list_op_exit_fini,
		.cascade = true
	}, {
		.init = test_init,
		.body = test_enter_exit_preempt_body,
		.fini = test_enter_exit_preempt_fini,
		.cascade = true
	}, {
		.init = test_list_init,
		.body = test_enter_list_op_exit_preempt_body,
		.fini = test_enter_list_op_exit_preempt_fini,
		.cascade = true
	}, {
		.init = test_list_init,
		.body = test_enter_list_op_exit_drain_body,
		.fini = test_enter_list_op_exit_drain_fini,
		.cascade = true
	}, {
		.init = test_init,
		.body = test_thread_local_mutex_body,
		.fini = test_thread_local_mutex_fini,
		.cascade = true
	}, {
		.init = test_init,
		.body = test_enter_mutex_exit_preempt_body,
		.fini = test_enter_mutex_exit_preempt_fini,
		.cascade = true
	}
};

static void
set_affinity(rtems_id task, size_t cpu_index)
{
	rtems_status_code sc;
	cpu_set_t set;
	rtems_id sched;
	rtems_task_priority prio;

	sc = rtems_scheduler_ident_by_processor(cpu_index, &sched);
	assert(sc == RTEMS_SUCCESSFUL);

	sc = rtems_task_set_priority(task, RTEMS_CURRENT_PRIORITY, &prio);
	assert(sc == RTEMS_SUCCESSFUL);

	sc = rtems_task_set_scheduler(task, sched, prio);
	assert(sc == RTEMS_SUCCESSFUL);

	CPU_ZERO(&set);
	CPU_SET((int)cpu_index, &set);
	sc = rtems_task_set_affinity(task, sizeof(set), &set);
	assert(sc == RTEMS_SUCCESSFUL);
}

static void
setup_worker(rtems_test_parallel_context *base, size_t worker_index,
   rtems_id worker_id)
{

	set_affinity(worker_id, worker_index);
}

static void
test_main(void)
{
	test_context *ctx;

	ctx = &test_instance;
	rtems_mutex_init(&ctx->mtx[0], "test 0");
	rtems_mutex_init(&ctx->mtx[1], "test 1");

	printf("<" TEST_XML_NAME ">\n");

	setup_worker(&ctx->base, 0, rtems_task_self());
	rtems_test_parallel(&ctx->base, setup_worker, &test_jobs[0],
	    RTEMS_ARRAY_SIZE(test_jobs));

	printf("</" TEST_XML_NAME ">\n");
	rtems_mutex_destroy(&ctx->mtx[0]);
	rtems_mutex_destroy(&ctx->mtx[1]);
	exit(0);
}

#define CONFIGURE_MAXIMUM_PROCESSORS CPU_COUNT

#ifdef RTEMS_SMP

#define CONFIGURE_SCHEDULER_EDF_SMP

#include <rtems/scheduler.h>

RTEMS_SCHEDULER_EDF_SMP(a);
RTEMS_SCHEDULER_EDF_SMP(b);
RTEMS_SCHEDULER_EDF_SMP(c);
RTEMS_SCHEDULER_EDF_SMP(d);
RTEMS_SCHEDULER_EDF_SMP(e);
RTEMS_SCHEDULER_EDF_SMP(g);
RTEMS_SCHEDULER_EDF_SMP(h);
RTEMS_SCHEDULER_EDF_SMP(i);
RTEMS_SCHEDULER_EDF_SMP(j);
RTEMS_SCHEDULER_EDF_SMP(k);
RTEMS_SCHEDULER_EDF_SMP(l);
RTEMS_SCHEDULER_EDF_SMP(m);
RTEMS_SCHEDULER_EDF_SMP(n);
RTEMS_SCHEDULER_EDF_SMP(o);
RTEMS_SCHEDULER_EDF_SMP(p);
RTEMS_SCHEDULER_EDF_SMP(q);

#define CONFIGURE_SCHEDULER_TABLE_ENTRIES \
  RTEMS_SCHEDULER_TABLE_EDF_SMP(a, rtems_build_name(' ', ' ', ' ', 'a')), \
  RTEMS_SCHEDULER_TABLE_EDF_SMP(b, rtems_build_name(' ', ' ', ' ', 'b')), \
  RTEMS_SCHEDULER_TABLE_EDF_SMP(c, rtems_build_name(' ', ' ', ' ', 'c')), \
  RTEMS_SCHEDULER_TABLE_EDF_SMP(d, rtems_build_name(' ', ' ', ' ', 'd')), \
  RTEMS_SCHEDULER_TABLE_EDF_SMP(e, rtems_build_name(' ', ' ', ' ', 'e')), \
  RTEMS_SCHEDULER_TABLE_EDF_SMP(g, rtems_build_name(' ', ' ', ' ', 'g')), \
  RTEMS_SCHEDULER_TABLE_EDF_SMP(h, rtems_build_name(' ', ' ', ' ', 'h')), \
  RTEMS_SCHEDULER_TABLE_EDF_SMP(i, rtems_build_name(' ', ' ', ' ', 'i')), \
  RTEMS_SCHEDULER_TABLE_EDF_SMP(j, rtems_build_name(' ', ' ', ' ', 'j')), \
  RTEMS_SCHEDULER_TABLE_EDF_SMP(k, rtems_build_name(' ', ' ', ' ', 'k')), \
  RTEMS_SCHEDULER_TABLE_EDF_SMP(l, rtems_build_name(' ', ' ', ' ', 'l')), \
  RTEMS_SCHEDULER_TABLE_EDF_SMP(m, rtems_build_name(' ', ' ', ' ', 'm')), \
  RTEMS_SCHEDULER_TABLE_EDF_SMP(n, rtems_build_name(' ', ' ', ' ', 'n')), \
  RTEMS_SCHEDULER_TABLE_EDF_SMP(o, rtems_build_name(' ', ' ', ' ', 'o')), \
  RTEMS_SCHEDULER_TABLE_EDF_SMP(p, rtems_build_name(' ', ' ', ' ', 'p')), \
  RTEMS_SCHEDULER_TABLE_EDF_SMP(q, rtems_build_name(' ', ' ', ' ', 'q'))  \

#define CONFIGURE_SCHEDULER_ASSIGNMENTS \
  RTEMS_SCHEDULER_ASSIGN(0, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_MANDATORY), \
  RTEMS_SCHEDULER_ASSIGN(0, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(1, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(1, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(2, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(2, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(3, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(3, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(4, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(4, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(5, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(5, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(6, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(6, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(7, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(7, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(8, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(8, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(9, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(9, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(10, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(10, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(11, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(11, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(12, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(12, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(13, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(13, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(14, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(14, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(15, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN(15, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL)

#endif /* RTEMS_SMP */

#include <rtems/bsd/test/default-init.h>
