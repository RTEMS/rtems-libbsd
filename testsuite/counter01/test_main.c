/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * The counter(9) functions counter_u64_alloc(), counter_u64_add(),
 * counter_u64_fetch(), counter_u64_zero() and counter_u64_free() are tested
 * on each online processor.
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
#include <sys/counter.h>
#include <sys/malloc.h>
#include <sys/pcpu.h>

#include <assert.h>
#include <inttypes.h>
#include <stdio.h>

#include <rtems.h>

#define TEST_NAME "LIBBSD COUNTER 1"

#define CPU_COUNT 32

/* The counters fill more than one slab of the 8-byte per-CPU zone. */
#define COUNTER_COUNT 600

static counter_u64_t counters[COUNTER_COUNT];

static bool
set_processor(uint32_t cpu_index)
{
	rtems_status_code sc;
	rtems_id scheduler_id;
	cpu_set_t cpuset;

	sc = rtems_scheduler_ident_by_processor(cpu_index, &scheduler_id);
	if (sc != RTEMS_SUCCESSFUL) {
		return (false);
	}

	CPU_ZERO(&cpuset);
	CPU_SET((int)cpu_index, &cpuset);
	sc = rtems_task_set_affinity(RTEMS_SELF, sizeof(cpuset), &cpuset);
	assert(sc == RTEMS_SUCCESSFUL);
	assert(rtems_scheduler_get_processor() == cpu_index);
	return (true);
}

static void
restore_affinity(void)
{
	rtems_status_code sc;
	cpu_set_t cpuset;

	CPU_FILL(&cpuset);
	sc = rtems_task_set_affinity(RTEMS_SELF, sizeof(cpuset), &cpuset);
	assert(sc == RTEMS_SUCCESSFUL);
}

static uint64_t
increment(size_t i, uint32_t cpu_index)
{

	return ((uint64_t)(i + 1) << (cpu_index % 16));
}

static void
test_main(void)
{
	uint32_t cpu_max;
	uint32_t cpu_index;
	uint64_t sum[COUNTER_COUNT];
	bool online[CPU_COUNT];
	size_t i;

	cpu_max = rtems_scheduler_get_processor_maximum();
	assert(cpu_max <= CPU_COUNT);

	for (i = 0; i < COUNTER_COUNT; ++i) {
		counters[i] = counter_u64_alloc(M_WAITOK);
		assert(counters[i] != NULL);
		assert(counter_u64_fetch(counters[i]) == 0);
		sum[i] = 0;
	}

	for (cpu_index = 0; cpu_index < cpu_max; ++cpu_index) {
		online[cpu_index] = set_processor(cpu_index);
		if (!online[cpu_index]) {
			continue;
		}

		for (i = 0; i < COUNTER_COUNT; ++i) {
			counter_u64_add(counters[i],
			    (int64_t)increment(i, cpu_index));
			sum[i] += increment(i, cpu_index);
		}
	}

	restore_affinity();

	for (i = 0; i < COUNTER_COUNT; ++i) {
		assert(counter_u64_fetch(counters[i]) == sum[i]);

		for (cpu_index = 0; cpu_index < cpu_max; ++cpu_index) {
			uint64_t expected;

			expected = online[cpu_index] ?
			    increment(i, cpu_index) : 0;
			assert(*zpcpu_get_cpu(counters[i], cpu_index) ==
			    expected);
		}
	}

	for (i = 0; i < COUNTER_COUNT; i += 2) {
		counter_u64_zero(counters[i]);
	}

	for (i = 0; i < COUNTER_COUNT; ++i) {
		uint64_t expected;

		expected = (i % 2) == 0 ? 0 : sum[i];
		assert(counter_u64_fetch(counters[i]) == expected);
	}

	for (i = 0; i < COUNTER_COUNT; ++i) {
		counter_u64_free(counters[i]);
	}

	exit(0);
}

#ifdef RTEMS_SMP
#define CONFIGURE_MAXIMUM_PROCESSORS CPU_COUNT
#endif

#include <rtems/bsd/test/default-init.h>
