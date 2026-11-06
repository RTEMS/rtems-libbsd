/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * A timeout of a sleep which expires after a wakeup ended that sleep is
 * tested.  The timeout must not end the next sleep of the thread.  The next
 * sleep has a timeout of its own.  A sleep without a timeout returns zero
 * also after a spurious wakeup.
 */

/*
 * Copyright (C) 2026 embedded brains GmbH & Co. KG
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <machine/rtems-bsd-kernel-space.h>

#include <sys/param.h>
#include <sys/types.h>
#include <sys/systm.h>
#include <sys/kernel.h>

#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <rtems.h>
#include <rtems/score/percpu.h>
#include <rtems/score/statesimpl.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/watchdogimpl.h>

#define TEST_NAME "LIBBSD SLEEP 2"

#define CPU_COUNT 32

/*
 * The sleeper begins the first sleep on this processor.  The timeout of that
 * sleep therefore expires in the tick of this processor.
 */
#define CPU_TIMEOUT 0

/*
 * The runner and the waker execute on this processor.  The sleeper moves to it
 * after it began the first sleep.
 */
#define CPU_RUN 1

/*
 * The sleeper has the highest priority.  A sleep which a stale timeout ends
 * therefore returns before the runner wakes the sleeper up.
 */
#define PRIO_SLEEPER 1

#define PRIO_WAKER 2

#define PRIO_RUNNER 3

#define EVENT_WAKE RTEMS_EVENT_0

#define EVENT_TIMEOUT_DONE RTEMS_EVENT_1

#define EVENT_SLEEPER_DONE RTEMS_EVENT_2

#define SPIN_LIMIT_NS 1000000000ULL

typedef struct {
	rtems_id runner;
	rtems_id waker;
	rtems_id sleeper;
	Thread_Control *volatile sleeper_tcb;
	Watchdog_Service_routine_entry timeout;
	volatile bool first_done;
	volatile bool spin_timeout;
	int first_eno;
	int second_eno;
	int first_channel;
	int second_channel;
} test_context;

static test_context test_instance;

static void
set_affinity(rtems_id id, uint32_t cpu_index)
{
	rtems_status_code sc;
	cpu_set_t cpuset;

	CPU_ZERO(&cpuset);
	CPU_SET((int)cpu_index, &cpuset);
	sc = rtems_task_set_affinity(id, sizeof(cpuset), &cpuset);
	assert(sc == RTEMS_SUCCESSFUL);
}

static bool
waits_for_bsd_wakeup(const Thread_Control *tcb)
{

	return ((*(volatile const States_Control *)&tcb->current_state &
	    STATES_WAITING_FOR_BSD_WAKEUP) != 0);
}

static void
send_event(rtems_id id, rtems_event_set event)
{
	rtems_status_code sc;

	sc = rtems_event_send(id, event);
	assert(sc == RTEMS_SUCCESSFUL);
}

static void
receive_event(rtems_event_set event)
{
	rtems_status_code sc;
	rtems_event_set events;

	sc = rtems_event_receive(event, RTEMS_EVENT_ALL | RTEMS_WAIT,
	    10 * rtems_clock_get_ticks_per_second(), &events);
	assert(sc == RTEMS_SUCCESSFUL);
	assert(events == event);
}

/*
 * The tick of CPU_TIMEOUT calls this routine with thread dispatch disabled.
 * The sleeper therefore runs on CPU_RUN while this routine waits.
 *
 * The tick took the watchdog out of the tree and released the lock of the
 * header before the call.  This routine widens that window.  It lets the
 * first sleep end through a wakeup and waits until the sleeper begins the
 * second sleep.  Then it calls the timeout routine of libbsd with the token
 * of the first sleep.
 */
static void
timeout_wrapper(Watchdog_Control *wdg, unsigned int token)
{
	test_context *ctx;
	uint64_t start;
	bool done;

	ctx = &test_instance;
	(void)rtems_event_send(ctx->waker, EVENT_WAKE);

	start = rtems_clock_get_uptime_nanoseconds();
	do {
		done = ctx->first_done && waits_for_bsd_wakeup(ctx->sleeper_tcb);
	} while (!done &&
	    rtems_clock_get_uptime_nanoseconds() - start < SPIN_LIMIT_NS);

	ctx->spin_timeout = !done;
	(*ctx->timeout)(wdg, token);
	(void)rtems_event_send(ctx->runner, EVENT_TIMEOUT_DONE);
}

/* The waker executes on CPU_RUN. */
static void
waker_task(rtems_task_argument arg)
{
	test_context *ctx;

	ctx = (test_context *)arg;
	receive_event(EVENT_WAKE);
	wakeup(&ctx->first_channel);
	(void)rtems_task_suspend(RTEMS_SELF);
}

/*
 * The sleeper begins the first sleep on CPU_TIMEOUT.  It returns from the first
 * sleep and begins the second sleep on CPU_RUN.
 */
static void
sleeper_task(rtems_task_argument arg)
{
	test_context *ctx;

	ctx = (test_context *)arg;
	ctx->sleeper_tcb = _Thread_Get_executing();

	/* The waker ends this sleep before its timeout ends it. */
	ctx->first_eno = tsleep(&ctx->first_channel, 0, "first", hz);
	ctx->first_done = true;

	/*
	 * The runner ends this sleep.  The stale timeout of the first sleep
	 * ends it with EWOULDBLOCK if libbsd does not discard that timeout.
	 */
	ctx->second_eno = tsleep(&ctx->second_channel, 0, "second", 5 * hz);
	send_event(ctx->runner, EVENT_SLEEPER_DONE);
	(void)rtems_task_suspend(RTEMS_SELF);
}

static rtems_id
start_task(rtems_name name, rtems_task_priority prio, uint32_t cpu_index,
    rtems_task_entry entry, test_context *ctx)
{
	rtems_status_code sc;
	rtems_id id;

	sc = rtems_task_create(name, prio, RTEMS_MINIMUM_STACK_SIZE,
	    RTEMS_DEFAULT_MODES, RTEMS_FLOATING_POINT, &id);
	assert(sc == RTEMS_SUCCESSFUL);
	set_affinity(id, cpu_index);
	sc = rtems_task_start(id, entry, (rtems_task_argument)ctx);
	assert(sc == RTEMS_SUCCESSFUL);
	return (id);
}

/*
 * A sleeper which waits for a BSD wakeup armed the timeout of its first sleep
 * on CPU_TIMEOUT.
 */
static void
wait_for_first_sleep(test_context *ctx)
{
	int i;

	for (i = 0; i < 100; ++i) {
		Thread_Control *tcb;

		tcb = ctx->sleeper_tcb;
		if (tcb != NULL && waits_for_bsd_wakeup(tcb)) {
			return;
		}

		(void)rtems_task_wake_after(1);
	}

	assert(0);
}

static void
install_timeout_wrapper(test_context *ctx)
{
	Thread_Control *tcb;
	Per_CPU_Control *cpu;
	ISR_lock_Context lock_context;
	bool scheduled;

	tcb = ctx->sleeper_tcb;
	cpu = _Watchdog_Get_CPU(&tcb->Timer.Watchdog);
	assert(cpu == _Per_CPU_Get_by_index(CPU_TIMEOUT));

	/* The tick reads the routine under the lock of the header. */
	_ISR_lock_ISR_disable(&lock_context);
	_Watchdog_Per_CPU_acquire_critical(cpu, &lock_context);
	scheduled = _Watchdog_Is_scheduled(&tcb->Timer.Watchdog);
	ctx->timeout = tcb->Timer.Watchdog.routine;
	tcb->Timer.Watchdog.routine = timeout_wrapper;
	_Watchdog_Per_CPU_release_critical(cpu, &lock_context);
	_ISR_lock_ISR_enable(&lock_context);

	assert(scheduled);
}

static void
test_stale_timeout(test_context *ctx)
{
	rtems_status_code sc;
	rtems_task_priority prio;

	puts("test stale timeout");

	ctx->runner = rtems_task_self();
	set_affinity(RTEMS_SELF, CPU_RUN);
	sc = rtems_task_set_priority(RTEMS_SELF, PRIO_RUNNER, &prio);
	assert(sc == RTEMS_SUCCESSFUL);

	ctx->waker = start_task(rtems_build_name('W', 'A', 'K', 'E'),
	    PRIO_WAKER, CPU_RUN, waker_task, ctx);
	ctx->sleeper = start_task(rtems_build_name('S', 'L', 'E', 'P'),
	    PRIO_SLEEPER, CPU_TIMEOUT, sleeper_task, ctx);

	wait_for_first_sleep(ctx);
	set_affinity(ctx->sleeper, CPU_RUN);
	install_timeout_wrapper(ctx);

	receive_event(EVENT_TIMEOUT_DONE);
	assert(!ctx->spin_timeout);

	wakeup(&ctx->second_channel);
	receive_event(EVENT_SLEEPER_DONE);

	printf("first sleep: %i, second sleep: %i\n", ctx->first_eno,
	    ctx->second_eno);
	assert(ctx->first_eno == 0);
	assert(ctx->second_eno == 0);
}

static void
test_main(void)
{

	if (rtems_scheduler_get_processor_maximum() < 2) {
		puts("test needs two processors");
		exit(0);
	}

	test_stale_timeout(&test_instance);
	exit(0);
}

#ifdef RTEMS_SMP
#define CONFIGURE_MAXIMUM_PROCESSORS CPU_COUNT
#endif

#include <rtems/bsd/test/default-init.h>
