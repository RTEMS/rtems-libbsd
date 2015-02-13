/**
 * @file
 *
 * @ingroup rtems_bsd_machine
 *
 * @brief TODO.
 */

/*
 * Copyright (c) 2009, 2010 embedded brains GmbH.  All rights reserved.
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

#ifndef _RTEMS_BSD_MACHINE_ATOMIC_H_
#define _RTEMS_BSD_MACHINE_ATOMIC_H_

#ifndef _RTEMS_BSD_MACHINE_RTEMS_BSD_KERNEL_SPACE_H_
#error "the header file <machine/rtems-bsd-kernel-space.h> must be included first"
#endif

#include <rtems.h>

#define mb() RTEMS_COMPILER_MEMORY_BARRIER()
#define wmb() RTEMS_COMPILER_MEMORY_BARRIER()
#define rmb() RTEMS_COMPILER_MEMORY_BARRIER()

static inline void
atomic_add_int(volatile int *p, int v)
{
	rtems_interrupt_level level;

	rtems_interrupt_disable(level);
	*p += v;
	rtems_interrupt_enable(level);
}

#define atomic_add_acq_int atomic_add_int
#define atomic_add_rel_int atomic_add_int

static inline void
atomic_subtract_int(volatile int *p, int v)
{
	rtems_interrupt_level level;

	rtems_interrupt_disable(level);
	*p -= v;
	rtems_interrupt_enable(level);
}

#define atomic_subtract_acq_int atomic_subtract_int
#define atomic_subtract_rel_int atomic_subtract_int

static inline void
atomic_set_int(volatile int *p, int v)
{
	rtems_interrupt_level level;

	rtems_interrupt_disable(level);
	*p |= v;
	rtems_interrupt_enable(level);
}

#define atomic_set_acq_int atomic_set_int
#define atomic_set_rel_int atomic_set_int

static inline void
atomic_clear_int(volatile int *p, int v)
{
	rtems_interrupt_level level;

	rtems_interrupt_disable(level);
	*p &= ~v;
	rtems_interrupt_enable(level);
}

#define atomic_clear_acq_int atomic_clear_int
#define atomic_clear_rel_int atomic_clear_int

static inline int
atomic_cmpset_int(volatile int *p, int cmp, int set)
{
	rtems_interrupt_level level;
	int rv;

	rtems_interrupt_disable(level);
	rv = *p == cmp;
	if (rv) {
		*p = set;
	}
	rtems_interrupt_enable(level);

	return rv;
}

#define atomic_cmpset_acq_int atomic_cmpset_int
#define atomic_cmpset_rel_int atomic_cmpset_int

static inline int
atomic_fetchadd_int(volatile int *p, int v)
{
	rtems_interrupt_level level;
	int tmp;

	rtems_interrupt_disable(level);
	tmp = *p;
	*p += v;
	rtems_interrupt_enable(level);

	return tmp;
}

static inline int
atomic_readandclear_int(volatile int *p)
{
	rtems_interrupt_level level;
	int tmp;

	rtems_interrupt_disable(level);
	tmp = *p;
	*p = 0;
	rtems_interrupt_enable(level);

	return tmp;
}

static inline int
atomic_load_acq_int(volatile int *p)
{
	return *p;
}

static inline void
atomic_store_rel_int(volatile int *p, int v)
{
	*p = v;
}

static inline void
atomic_add_32(volatile uint32_t *p, uint32_t v)
{
	rtems_interrupt_level level;

	rtems_interrupt_disable(level);
	*p += v;
	rtems_interrupt_enable(level);
}

#define atomic_add_acq_32 atomic_add_32
#define atomic_add_rel_32 atomic_add_32

static inline void
atomic_subtract_32(volatile uint32_t *p, uint32_t v)
{
	rtems_interrupt_level level;

	rtems_interrupt_disable(level);
	*p -= v;
	rtems_interrupt_enable(level);
}

#define atomic_subtract_acq_32 atomic_subtract_32
#define atomic_subtract_rel_32 atomic_subtract_32

static inline void
atomic_set_32(volatile uint32_t *p, uint32_t v)
{
	rtems_interrupt_level level;

	rtems_interrupt_disable(level);
	*p |= v;
	rtems_interrupt_enable(level);
}

#define atomic_set_acq_32 atomic_set_32
#define atomic_set_rel_32 atomic_set_32

static inline void
atomic_clear_32(volatile uint32_t *p, uint32_t v)
{
	rtems_interrupt_level level;

	rtems_interrupt_disable(level);
	*p &= ~v;
	rtems_interrupt_enable(level);
}

#define atomic_clear_acq_32 atomic_clear_32
#define atomic_clear_rel_32 atomic_clear_32

static inline int
atomic_cmpset_32(volatile uint32_t *p, uint32_t cmp, uint32_t set)
{
	rtems_interrupt_level level;
	int rv;

	rtems_interrupt_disable(level);
	rv = *p == cmp;
	if (rv) {
		*p = set;
	}
	rtems_interrupt_enable(level);

	return rv;
}

#define atomic_cmpset_acq_32 atomic_cmpset_32
#define atomic_cmpset_rel_32 atomic_cmpset_32

static inline uint32_t
atomic_fetchadd_32(volatile uint32_t *p, uint32_t v)
{
	rtems_interrupt_level level;
	uint32_t tmp;

	rtems_interrupt_disable(level);
	tmp = *p;
	*p += v;
	rtems_interrupt_enable(level);

	return tmp;
}

static inline uint32_t
atomic_readandclear_32(volatile uint32_t *p)
{
	rtems_interrupt_level level;
	uint32_t tmp;

	rtems_interrupt_disable(level);
	tmp = *p;
	*p = 0;
	rtems_interrupt_enable(level);

	return tmp;
}

static inline uint32_t
atomic_load_acq_32(volatile uint32_t *p)
{
	return *p;
}

static inline void
atomic_store_rel_32(volatile uint32_t *p, uint32_t v)
{
	*p = v;
}

static inline void
atomic_add_long(volatile long *p, long v)
{
	rtems_interrupt_level level;

	rtems_interrupt_disable(level);
	*p += v;
	rtems_interrupt_enable(level);
}

#define atomic_add_acq_long atomic_add_long
#define atomic_add_rel_long atomic_add_long

static inline void
atomic_subtract_long(volatile long *p, long v)
{
	rtems_interrupt_level level;

	rtems_interrupt_disable(level);
	*p -= v;
	rtems_interrupt_enable(level);
}

#define atomic_subtract_acq_long atomic_subtract_long
#define atomic_subtract_rel_long atomic_subtract_long

static inline void
atomic_set_long(volatile long *p, long v)
{
	rtems_interrupt_level level;

	rtems_interrupt_disable(level);
	*p |= v;
	rtems_interrupt_enable(level);
}

#define atomic_set_acq_long atomic_set_long
#define atomic_set_rel_long atomic_set_long

static inline void
atomic_clear_long(volatile long *p, long v)
{
	rtems_interrupt_level level;

	rtems_interrupt_disable(level);
	*p &= ~v;
	rtems_interrupt_enable(level);
}

#define atomic_clear_acq_long atomic_clear_long
#define atomic_clear_rel_long atomic_clear_long

static inline int
atomic_cmpset_long(volatile long *p, long cmp, long set)
{
	rtems_interrupt_level level;
	int rv;

	rtems_interrupt_disable(level);
	rv = *p == cmp;
	if (rv) {
		*p = set;
	}
	rtems_interrupt_enable(level);

	return rv;
}

#define atomic_cmpset_acq_long atomic_cmpset_long
#define atomic_cmpset_rel_long atomic_cmpset_long

static inline long
atomic_fetchadd_long(volatile long *p, long v)
{
	rtems_interrupt_level level;
	long tmp;

	rtems_interrupt_disable(level);
	tmp = *p;
	*p += v;
	rtems_interrupt_enable(level);

	return tmp;
}

static inline long
atomic_readandclear_long(volatile long *p)
{
	rtems_interrupt_level level;
	long tmp;

	rtems_interrupt_disable(level);
	tmp = *p;
	*p = 0;
	rtems_interrupt_enable(level);

	return tmp;
}

static inline long
atomic_load_acq_long(volatile long *p)
{
	return *p;
}

static inline void
atomic_store_rel_long(volatile long *p, long v)
{
	*p = v;
}

#endif /* _RTEMS_BSD_MACHINE_ATOMIC_H_ */
