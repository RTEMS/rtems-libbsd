/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
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
#include <sys/systm.h>
#include <rtems/bsd/sys/lock.h>
#include <sys/mutex.h>

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include <rtems/libcsupport.h>

#define TEST_NAME "LIBBSD SLEEP 1"

#define SLAVE_COUNT 3

#define PRIO_HIGH 1

#define PRIO_LOW 5

/*
 * This priority assignment ensures that we can test the priority thread queue
 * order.
 */
static const rtems_task_priority slave_prios[SLAVE_COUNT] = { 4, 3, 2 };

static rtems_id slave_ids[SLAVE_COUNT];

static int slave_counters[SLAVE_COUNT];

static int channel;

static int counter;

static void
set_self_prio(rtems_task_priority prio)
{
	rtems_status_code sc;

	sc = rtems_task_set_priority(RTEMS_SELF, prio, &prio);
	assert(sc == RTEMS_SUCCESSFUL);
}

static void
slave_task(rtems_task_argument slave)
{
	while (true) {
		int timo = 0;
		int priority = 0;
		const char *wmesg = "slave_task";
		int eno;

		++counter;
		slave_counters[slave] = counter;

		printf("slave[%" PRIuPTR "]: %i\n", slave, counter);

		eno = tsleep(&channel, priority, wmesg, timo);
		assert(eno == 0);
	}
}

static void
start_slaves(void)
{
	size_t i;

	set_self_prio(PRIO_HIGH);

	for (i = 0; i < SLAVE_COUNT; ++i) {
		rtems_status_code sc;

		sc = rtems_task_create(
			rtems_build_name('S', 'L', 'A', 'V'),
			slave_prios[i],
			RTEMS_MINIMUM_STACK_SIZE,
			RTEMS_DEFAULT_MODES,
			RTEMS_FLOATING_POINT,
			&slave_ids[i]
		);
		assert(sc == RTEMS_SUCCESSFUL);

		sc = rtems_task_start(slave_ids[i], slave_task, i);
		assert(sc == RTEMS_SUCCESSFUL);
	}

	set_self_prio(PRIO_LOW);
}

static void
test_timeouts(void)
{
	int eno;
	struct mtx mtx;
	int priority = 0;
	const char *wmesg;
	int timo = 2;

	puts("test timeouts");

	mtx_init(&mtx, "test timeouts: mtx", NULL, MTX_DEF);
	mtx_lock(&mtx);
	wmesg = "test timeouts: msleep";
	eno = msleep(&channel, &mtx, priority, wmesg, timo);
	assert(eno == EWOULDBLOCK);
	mtx_unlock(&mtx);
	mtx_destroy(&mtx);

	mtx_init(&mtx, "test timeouts: mtx spin", NULL, MTX_SPIN);
	mtx_lock_spin(&mtx);
	wmesg = "test timeouts: msleep_spin";
	eno = msleep_spin(&channel, &mtx, wmesg, timo);
	assert(eno == EWOULDBLOCK);
	mtx_unlock_spin(&mtx);
	mtx_destroy(&mtx);

	wmesg = "test timeouts: pause";
	pause(wmesg, timo);

	wmesg = "test timeouts: tsleep";
	eno = tsleep(&channel, priority, wmesg, timo);
	assert(eno == EWOULDBLOCK);
}

static void
test_wakeup_without_waiter(void)
{
	int some_channel;

	puts("test wakeup without waiter");

	wakeup_one(&some_channel);
	wakeup(&some_channel);
}

static void
test_wakeup_one(void)
{
	puts("test wakeup one");

	assert(slave_counters[0] == 3);
	assert(slave_counters[1] == 2);
	assert(slave_counters[2] == 1);

	wakeup_one(&channel);

	assert(slave_counters[0] == 3);
	assert(slave_counters[1] == 2);
	assert(slave_counters[2] == 4);

	wakeup_one(&channel);

	assert(slave_counters[0] == 3);
	assert(slave_counters[1] == 2);
	assert(slave_counters[2] == 5);
}

static void
test_wakeup(void)
{
	puts("test wakeup");

	assert(slave_counters[0] == 3);
	assert(slave_counters[1] == 2);
	assert(slave_counters[2] == 5);

	wakeup(&channel);

	assert(slave_counters[0] == 8);
	assert(slave_counters[1] == 7);
	assert(slave_counters[2] == 6);
}

static void
alloc_basic_resources(void)
{
	curthread;
}

static void
test_main(void)
{
	rtems_resource_snapshot snapshot;

	alloc_basic_resources();
	start_slaves();

	rtems_resource_snapshot_take(&snapshot);

	test_timeouts();
	test_wakeup_without_waiter();
	test_wakeup_one();
	test_wakeup();

	assert(rtems_resource_snapshot_check(&snapshot));

	exit(0);
}

#include <rtems/bsd/test/default-init.h>
