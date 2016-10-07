/*
 * Copyright (c) 2016 embedded brains GmbH.  All rights reserved.
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

#ifndef __MACHINE_COUNTER_H__
#define __MACHINE_COUNTER_H__

#include <sys/pcpu.h>

#include <rtems/score/isrlevel.h>

#ifdef IN_SUBR_COUNTER_C

static inline uint64_t
counter_u64_read_one(uint64_t *p, int cpu)
{

	return (*((uint64_t *)((char *)p + sizeof(struct pcpu) * cpu)));
}

static inline uint64_t
counter_u64_fetch_inline(uint64_t *p)
{
	uint64_t r;
	uint32_t cpu;

	r = 0;
	for (cpu = 0; cpu < _SMP_Get_processor_count(); ++cpu) {
		r += counter_u64_read_one((uint64_t *)p, cpu);
	}

	return (r);
}

static inline void
counter_u64_zero_inline(counter_u64_t c)
{
	uint32_t cpu;

	for (cpu = 0; cpu < _SMP_Get_processor_count(); ++cpu) {
		*((uint64_t *)((char *)c + sizeof(struct pcpu) * cpu)) = 0;
	}
}
#endif

static inline void
counter_u64_add_protected(counter_u64_t c, int64_t inc)
{

	*(uint64_t *)zpcpu_get(c) += inc;
}

static inline void
counter_u64_add(counter_u64_t c, int64_t inc)
{
	ISR_Level level;

	_ISR_Local_disable(level);
	counter_u64_add_protected(c, inc);
	_ISR_Local_enable(level);
}

#endif	/* ! __MACHINE_COUNTER_H__ */
