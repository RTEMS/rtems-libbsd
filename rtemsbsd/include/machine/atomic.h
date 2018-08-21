/**
 * @file
 *
 * @ingroup rtems_bsd_machine
 *
 * @brief TODO.
 */

/*
 * Copyright (c) 2009, 2015 embedded brains GmbH.  All rights reserved.
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

#include <rtems.h>

#ifdef RTEMS_SMP
  #if defined(__cplusplus) \
    && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 9))
    /*
     * The GCC 4.9 ships its own <stdatomic.h> which is not C++ compatible.  The
     * suggested solution was to include <atomic> in case C++ is used.  This works
     * at least with GCC 4.9.  See also:
     *
     * http://gcc.gnu.org/bugzilla/show_bug.cgi?id=60932
     * http://gcc.gnu.org/bugzilla/show_bug.cgi?id=60940
     */
    #include <atomic>
    #define _RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC
  #else
    #include <stdatomic.h>
    #define _RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC
  #endif
#endif

static inline void
mb(void)
{
#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
	std::atomic_thread_fence(std::memory_order_seq_cst);
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
	atomic_thread_fence(memory_order_seq_cst);
#else
	RTEMS_COMPILER_MEMORY_BARRIER();
#endif
}

static inline void
wmb(void)
{
#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
	std::atomic_thread_fence(std::memory_order_release);
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
	atomic_thread_fence(memory_order_release);
#else
	RTEMS_COMPILER_MEMORY_BARRIER();
#endif
}

static inline void
rmb(void)
{
#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
	std::atomic_thread_fence(std::memory_order_acquire);
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
	atomic_thread_fence(memory_order_acquire);
#else
	RTEMS_COMPILER_MEMORY_BARRIER();
#endif
}

static inline void
atomic_add_int(volatile int *p, int v)
{
#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
	std::atomic_int *q =
	    reinterpret_cast<std::atomic_int *>(const_cast<int *>(p));

	q->fetch_add(v, std::memory_order_seq_cst);
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
	atomic_int *q = (atomic_int *)RTEMS_DEVOLATILE(int *, p);

	atomic_fetch_add_explicit(q, v, memory_order_seq_cst);
#else
	rtems_interrupt_level level;

	rtems_interrupt_disable(level);
	*p += v;
	rtems_interrupt_enable(level);
#endif
}

static inline void
atomic_add_acq_int(volatile int *p, int v)
{
#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
	std::atomic_int *q =
	    reinterpret_cast<std::atomic_int *>(const_cast<int *>(p));

	q->fetch_add(v, std::memory_order_acquire);
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
	atomic_int *q = (atomic_int *)RTEMS_DEVOLATILE(int *, p);

	atomic_fetch_add_explicit(q, v, memory_order_acquire);
#else
	rtems_interrupt_level level;

	rtems_interrupt_disable(level);
	*p += v;
	rtems_interrupt_enable(level);
#endif
}

static inline void
atomic_add_rel_int(volatile int *p, int v)
{
#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
	std::atomic_int *q =
	    reinterpret_cast<std::atomic_int *>(const_cast<int *>(p));

	q->fetch_add(v, std::memory_order_release);
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
	atomic_int *q = (atomic_int *)RTEMS_DEVOLATILE(int *, p);

	atomic_fetch_add_explicit(q, v, memory_order_release);
#else
	rtems_interrupt_level level;

	rtems_interrupt_disable(level);
	*p += v;
	rtems_interrupt_enable(level);
#endif
}

static inline void
atomic_subtract_int(volatile int *p, int v)
{
#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
	std::atomic_int *q =
	    reinterpret_cast<std::atomic_int *>(const_cast<int *>(p));

	q->fetch_sub(v, std::memory_order_seq_cst);
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
	atomic_int *q = (atomic_int *)RTEMS_DEVOLATILE(int *, p);

	atomic_fetch_sub_explicit(q, v, memory_order_seq_cst);
#else
	rtems_interrupt_level level;

	rtems_interrupt_disable(level);
	*p -= v;
	rtems_interrupt_enable(level);
#endif
}

static inline void
atomic_subtract_acq_int(volatile int *p, int v)
{
#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
	std::atomic_int *q =
	    reinterpret_cast<std::atomic_int *>(const_cast<int *>(p));

	q->fetch_sub(v, std::memory_order_acquire);
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
	atomic_int *q = (atomic_int *)RTEMS_DEVOLATILE(int *, p);

	atomic_fetch_sub_explicit(q, v, memory_order_acquire);
#else
	rtems_interrupt_level level;

	rtems_interrupt_disable(level);
	*p -= v;
	rtems_interrupt_enable(level);
#endif
}

static inline void
atomic_subtract_rel_int(volatile int *p, int v)
{
#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
	std::atomic_int *q =
	    reinterpret_cast<std::atomic_int *>(const_cast<int *>(p));

	q->fetch_sub(v, std::memory_order_release);
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
	atomic_int *q = (atomic_int *)RTEMS_DEVOLATILE(int *, p);

	atomic_fetch_sub_explicit(q, v, memory_order_release);
#else
	rtems_interrupt_level level;

	rtems_interrupt_disable(level);
	*p -= v;
	rtems_interrupt_enable(level);
#endif
}

static inline void
atomic_set_int(volatile int *p, int v)
{
#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
	std::atomic_int *q =
	    reinterpret_cast<std::atomic_int *>(const_cast<int *>(p));

	q->fetch_or(v, std::memory_order_seq_cst);
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
	atomic_int *q = (atomic_int *)RTEMS_DEVOLATILE(int *, p);

	atomic_fetch_or_explicit(q, v, memory_order_seq_cst);
#else
	rtems_interrupt_level level;

	rtems_interrupt_disable(level);
	*p |= v;
	rtems_interrupt_enable(level);
#endif
}

static inline void
atomic_set_acq_int(volatile int *p, int v)
{
#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
	std::atomic_int *q =
	    reinterpret_cast<std::atomic_int *>(const_cast<int *>(p));

	q->fetch_or(v, std::memory_order_acquire);
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
	atomic_int *q = (atomic_int *)RTEMS_DEVOLATILE(int *, p);

	atomic_fetch_or_explicit(q, v, memory_order_acquire);
#else
	rtems_interrupt_level level;

	rtems_interrupt_disable(level);
	*p |= v;
	rtems_interrupt_enable(level);
#endif
}

static inline void
atomic_set_rel_int(volatile int *p, int v)
{
#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
	std::atomic_int *q =
	    reinterpret_cast<std::atomic_int *>(const_cast<int *>(p));

	q->fetch_or(v, std::memory_order_release);
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
	atomic_int *q = (atomic_int *)RTEMS_DEVOLATILE(int *, p);

	atomic_fetch_or_explicit(q, v, memory_order_release);
#else
	rtems_interrupt_level level;

	rtems_interrupt_disable(level);
	*p |= v;
	rtems_interrupt_enable(level);
#endif
}

static inline void
atomic_clear_int(volatile int *p, int v)
{
#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
	std::atomic_int *q =
	    reinterpret_cast<std::atomic_int *>(const_cast<int *>(p));

	q->fetch_and(~v, std::memory_order_seq_cst);
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
	atomic_int *q = (atomic_int *)RTEMS_DEVOLATILE(int *, p);

	atomic_fetch_and_explicit(q, ~v, memory_order_seq_cst);
#else
	rtems_interrupt_level level;

	rtems_interrupt_disable(level);
	*p &= ~v;
	rtems_interrupt_enable(level);
#endif
}

static inline void
atomic_clear_acq_int(volatile int *p, int v)
{
#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
	std::atomic_int *q =
	    reinterpret_cast<std::atomic_int *>(const_cast<int *>(p));

	q->fetch_and(~v, std::memory_order_acquire);
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
	atomic_int *q = (atomic_int *)RTEMS_DEVOLATILE(int *, p);

	atomic_fetch_and_explicit(q, ~v, memory_order_acquire);
#else
	rtems_interrupt_level level;

	rtems_interrupt_disable(level);
	*p &= ~v;
	rtems_interrupt_enable(level);
#endif
}

static inline void
atomic_clear_rel_int(volatile int *p, int v)
{
#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
	std::atomic_int *q =
	    reinterpret_cast<std::atomic_int *>(const_cast<int *>(p));

	q->fetch_and(~v, std::memory_order_release);
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
	atomic_int *q = (atomic_int *)RTEMS_DEVOLATILE(int *, p);

	atomic_fetch_and_explicit(q, ~v, memory_order_release);
#else
	rtems_interrupt_level level;

	rtems_interrupt_disable(level);
	*p &= ~v;
	rtems_interrupt_enable(level);
#endif
}

static inline int
atomic_cmpset_int(volatile int *p, int cmp, int set)
{
	int rv;

#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
	std::atomic_int *q =
	    reinterpret_cast<std::atomic_int *>(const_cast<int *>(p));

	rv = q->compare_exchange_strong(cmp, set, std::memory_order_seq_cst,
	    std::memory_order_relaxed);
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
	atomic_int *q = (atomic_int *)RTEMS_DEVOLATILE(int *, p);

	rv = atomic_compare_exchange_strong_explicit(q, &cmp, set,
	    memory_order_seq_cst, memory_order_relaxed);
#else
	rtems_interrupt_level level;

	rtems_interrupt_disable(level);
	rv = *p == cmp;
	if (rv) {
		*p = set;
	}
	rtems_interrupt_enable(level);
#endif

	return (rv);
}

static inline int
atomic_fcmpset_int(volatile int *p, int *cmp, int set)
{
	int rv;

#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
	std::atomic_int *q =
	    reinterpret_cast<std::atomic_int *>(const_cast<int *>(p));

	rv = q->compare_exchange_strong(*cmp, set, std::memory_order_seq_cst,
	    std::memory_order_relaxed);
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
	atomic_int *q = (atomic_int *)RTEMS_DEVOLATILE(int *, p);

	rv = atomic_compare_exchange_strong_explicit(q, cmp, set,
	    memory_order_seq_cst, memory_order_relaxed);
#else
	rtems_interrupt_level level;
	int actual;

	rtems_interrupt_disable(level);
	actual = *p;
	rv = actual == *cmp;
	*cmp = actual;
	if (rv) {
		*p = set;
	}
	rtems_interrupt_enable(level);
#endif

	return (rv);
}

static inline int
atomic_cmpset_acq_int(volatile int *p, int cmp, int set)
{
	int rv;

#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
	std::atomic_int *q =
	    reinterpret_cast<std::atomic_int *>(const_cast<int *>(p));

	rv = q->compare_exchange_strong(cmp, set, std::memory_order_acquire,
	    std::memory_order_relaxed);
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
	atomic_int *q = (atomic_int *)RTEMS_DEVOLATILE(int *, p);

	rv = atomic_compare_exchange_strong_explicit(q, &cmp, set,
	    memory_order_acquire, memory_order_relaxed);
#else
	rtems_interrupt_level level;

	rtems_interrupt_disable(level);
	rv = *p == cmp;
	if (rv) {
		*p = set;
	}
	rtems_interrupt_enable(level);
#endif

	return (rv);
}

static inline int
atomic_cmpset_rel_int(volatile int *p, int cmp, int set)
{
	int rv;

#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
	std::atomic_int *q =
	    reinterpret_cast<std::atomic_int *>(const_cast<int *>(p));

	rv = q->compare_exchange_strong(cmp, set, std::memory_order_release,
	    std::memory_order_relaxed);
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
	atomic_int *q = (atomic_int *)RTEMS_DEVOLATILE(int *, p);

	rv = atomic_compare_exchange_strong_explicit(q, &cmp, set,
	    memory_order_release, memory_order_relaxed);
#else
	rtems_interrupt_level level;

	rtems_interrupt_disable(level);
	rv = *p == cmp;
	if (rv) {
		*p = set;
	}
	rtems_interrupt_enable(level);
#endif

	return (rv);
}

static inline int
atomic_fetchadd_int(volatile int *p, int v)
{
	int tmp;

#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
	std::atomic_int *q =
	    reinterpret_cast<std::atomic_int *>(const_cast<int *>(p));

	tmp = q->fetch_add(v, std::memory_order_seq_cst);
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
	atomic_int *q = (atomic_int *)RTEMS_DEVOLATILE(int *, p);

	tmp = atomic_fetch_add_explicit(q, v, memory_order_seq_cst);
#else
	rtems_interrupt_level level;

	rtems_interrupt_disable(level);
	tmp = *p;
	*p += v;
	rtems_interrupt_enable(level);
#endif

	return (tmp);
}

static inline int
atomic_readandclear_int(volatile int *p)
{
	int tmp;

#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
	std::atomic_int *q =
	    reinterpret_cast<std::atomic_int *>(const_cast<int *>(p));

	tmp = q->exchange(0, std::memory_order_seq_cst);
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
	atomic_int *q = (atomic_int *)RTEMS_DEVOLATILE(int *, p);

	tmp = atomic_exchange_explicit(q, 0, memory_order_seq_cst);
#else
	rtems_interrupt_level level;

	rtems_interrupt_disable(level);
	tmp = *p;
	*p = 0;
	rtems_interrupt_enable(level);
#endif

	return (tmp);
}

static inline int
atomic_load_int(volatile int *p)
{
	int tmp;

#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
	std::atomic_int *q =
	    reinterpret_cast<std::atomic_int *>(const_cast<int *>(p));

	tmp = q->load(std::memory_order_relaxed);
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
	atomic_int *q = (atomic_int *)RTEMS_DEVOLATILE(int *, p);

	tmp = atomic_load_explicit(q, memory_order_relaxed);
#else
	tmp = *p;
#endif

	return (tmp);
}

static inline int
atomic_load_acq_int(volatile int *p)
{
	int tmp;

#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
	std::atomic_int *q =
	    reinterpret_cast<std::atomic_int *>(const_cast<int *>(p));

	tmp = q->load(std::memory_order_acquire);
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
	atomic_int *q = (atomic_int *)RTEMS_DEVOLATILE(int *, p);

	tmp = atomic_load_explicit(q, memory_order_acquire);
#else
	RTEMS_COMPILER_MEMORY_BARRIER();
	tmp = *p;
#endif

	return (tmp);
}

static inline void
atomic_store_int(volatile int *p, int v)
{
#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
	std::atomic_int *q =
	    reinterpret_cast<std::atomic_int *>(const_cast<int *>(p));

	q->store(v, std::memory_order_relaxed);
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
	atomic_int *q = (atomic_int *)RTEMS_DEVOLATILE(int *, p);

	atomic_store_explicit(q, v, memory_order_relaxed);
#else
	*p = v;
#endif
}

static inline void
atomic_store_rel_int(volatile int *p, int v)
{
#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
	std::atomic_int *q =
	    reinterpret_cast<std::atomic_int *>(const_cast<int *>(p));

	q->store(v, std::memory_order_release);
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
	atomic_int *q = (atomic_int *)RTEMS_DEVOLATILE(int *, p);

	atomic_store_explicit(q, v, memory_order_release);
#else
	*p = v;
	RTEMS_COMPILER_MEMORY_BARRIER();
#endif
}

static inline void
atomic_add_32(volatile uint32_t *p, uint32_t v)
{
#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
	std::atomic_uint_least32_t *q =
	    reinterpret_cast<std::atomic_uint_least32_t *>(const_cast<uint32_t *>(p));

	q->fetch_add(v, std::memory_order_seq_cst);
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
	atomic_uint_least32_t *q = (atomic_uint_least32_t *)RTEMS_DEVOLATILE(uint32_t *, p);

	atomic_fetch_add_explicit(q, v, memory_order_seq_cst);
#else
	rtems_interrupt_level level;

	rtems_interrupt_disable(level);
	*p += v;
	rtems_interrupt_enable(level);
#endif
}

static inline void
atomic_add_acq_32(volatile uint32_t *p, uint32_t v)
{
#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
	std::atomic_uint_least32_t *q =
	    reinterpret_cast<std::atomic_uint_least32_t *>(const_cast<uint32_t *>(p));

	q->fetch_add(v, std::memory_order_acquire);
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
	atomic_uint_least32_t *q = (atomic_uint_least32_t *)RTEMS_DEVOLATILE(uint32_t *, p);

	atomic_fetch_add_explicit(q, v, memory_order_acquire);
#else
	rtems_interrupt_level level;

	rtems_interrupt_disable(level);
	*p += v;
	rtems_interrupt_enable(level);
#endif
}

static inline void
atomic_add_rel_32(volatile uint32_t *p, uint32_t v)
{
#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
	std::atomic_uint_least32_t *q =
	    reinterpret_cast<std::atomic_uint_least32_t *>(const_cast<uint32_t *>(p));

	q->fetch_add(v, std::memory_order_release);
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
	atomic_uint_least32_t *q = (atomic_uint_least32_t *)RTEMS_DEVOLATILE(uint32_t *, p);

	atomic_fetch_add_explicit(q, v, memory_order_release);
#else
	rtems_interrupt_level level;

	rtems_interrupt_disable(level);
	*p += v;
	rtems_interrupt_enable(level);
#endif
}

static inline void
atomic_subtract_32(volatile uint32_t *p, uint32_t v)
{
#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
	std::atomic_uint_least32_t *q =
	    reinterpret_cast<std::atomic_uint_least32_t *>(const_cast<uint32_t *>(p));

	q->fetch_sub(v, std::memory_order_seq_cst);
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
	atomic_uint_least32_t *q = (atomic_uint_least32_t *)RTEMS_DEVOLATILE(uint32_t *, p);

	atomic_fetch_sub_explicit(q, v, memory_order_seq_cst);
#else
	rtems_interrupt_level level;

	rtems_interrupt_disable(level);
	*p -= v;
	rtems_interrupt_enable(level);
#endif
}

static inline void
atomic_subtract_acq_32(volatile uint32_t *p, uint32_t v)
{
#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
	std::atomic_uint_least32_t *q =
	    reinterpret_cast<std::atomic_uint_least32_t *>(const_cast<uint32_t *>(p));

	q->fetch_sub(v, std::memory_order_acquire);
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
	atomic_uint_least32_t *q = (atomic_uint_least32_t *)RTEMS_DEVOLATILE(uint32_t *, p);

	atomic_fetch_sub_explicit(q, v, memory_order_acquire);
#else
	rtems_interrupt_level level;

	rtems_interrupt_disable(level);
	*p -= v;
	rtems_interrupt_enable(level);
#endif
}

static inline void
atomic_subtract_rel_32(volatile uint32_t *p, uint32_t v)
{
#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
	std::atomic_uint_least32_t *q =
	    reinterpret_cast<std::atomic_uint_least32_t *>(const_cast<uint32_t *>(p));

	q->fetch_sub(v, std::memory_order_release);
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
	atomic_uint_least32_t *q = (atomic_uint_least32_t *)RTEMS_DEVOLATILE(uint32_t *, p);

	atomic_fetch_sub_explicit(q, v, memory_order_release);
#else
	rtems_interrupt_level level;

	rtems_interrupt_disable(level);
	*p -= v;
	rtems_interrupt_enable(level);
#endif
}

static inline void
atomic_set_32(volatile uint32_t *p, uint32_t v)
{
#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
	std::atomic_uint_least32_t *q =
	    reinterpret_cast<std::atomic_uint_least32_t *>(const_cast<uint32_t *>(p));

	q->fetch_or(v, std::memory_order_seq_cst);
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
	atomic_uint_least32_t *q = (atomic_uint_least32_t *)RTEMS_DEVOLATILE(uint32_t *, p);

	atomic_fetch_or_explicit(q, v, memory_order_seq_cst);
#else
	rtems_interrupt_level level;

	rtems_interrupt_disable(level);
	*p |= v;
	rtems_interrupt_enable(level);
#endif
}

static inline void
atomic_set_acq_32(volatile uint32_t *p, uint32_t v)
{
#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
	std::atomic_uint_least32_t *q =
	    reinterpret_cast<std::atomic_uint_least32_t *>(const_cast<uint32_t *>(p));

	q->fetch_or(v, std::memory_order_acquire);
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
	atomic_uint_least32_t *q = (atomic_uint_least32_t *)RTEMS_DEVOLATILE(uint32_t *, p);

	atomic_fetch_or_explicit(q, v, memory_order_acquire);
#else
	rtems_interrupt_level level;

	rtems_interrupt_disable(level);
	*p |= v;
	rtems_interrupt_enable(level);
#endif
}

static inline void
atomic_set_rel_32(volatile uint32_t *p, uint32_t v)
{
#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
	std::atomic_uint_least32_t *q =
	    reinterpret_cast<std::atomic_uint_least32_t *>(const_cast<uint32_t *>(p));

	q->fetch_or(v, std::memory_order_release);
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
	atomic_uint_least32_t *q = (atomic_uint_least32_t *)RTEMS_DEVOLATILE(uint32_t *, p);

	atomic_fetch_or_explicit(q, v, memory_order_release);
#else
	rtems_interrupt_level level;

	rtems_interrupt_disable(level);
	*p |= v;
	rtems_interrupt_enable(level);
#endif
}

static inline void
atomic_clear_32(volatile uint32_t *p, uint32_t v)
{
#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
	std::atomic_uint_least32_t *q =
	    reinterpret_cast<std::atomic_uint_least32_t *>(const_cast<uint32_t *>(p));

	q->fetch_and(~v, std::memory_order_seq_cst);
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
	atomic_uint_least32_t *q = (atomic_uint_least32_t *)RTEMS_DEVOLATILE(uint32_t *, p);

	atomic_fetch_and_explicit(q, ~v, memory_order_seq_cst);
#else
	rtems_interrupt_level level;

	rtems_interrupt_disable(level);
	*p &= ~v;
	rtems_interrupt_enable(level);
#endif
}

static inline void
atomic_clear_acq_32(volatile uint32_t *p, uint32_t v)
{
#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
	std::atomic_uint_least32_t *q =
	    reinterpret_cast<std::atomic_uint_least32_t *>(const_cast<uint32_t *>(p));

	q->fetch_and(~v, std::memory_order_acquire);
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
	atomic_uint_least32_t *q = (atomic_uint_least32_t *)RTEMS_DEVOLATILE(uint32_t *, p);

	atomic_fetch_and_explicit(q, ~v, memory_order_acquire);
#else
	rtems_interrupt_level level;

	rtems_interrupt_disable(level);
	*p &= ~v;
	rtems_interrupt_enable(level);
#endif
}

static inline void
atomic_clear_rel_32(volatile uint32_t *p, uint32_t v)
{
#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
	std::atomic_uint_least32_t *q =
	    reinterpret_cast<std::atomic_uint_least32_t *>(const_cast<uint32_t *>(p));

	q->fetch_and(~v, std::memory_order_release);
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
	atomic_uint_least32_t *q = (atomic_uint_least32_t *)RTEMS_DEVOLATILE(uint32_t *, p);

	atomic_fetch_and_explicit(q, ~v, memory_order_release);
#else
	rtems_interrupt_level level;

	rtems_interrupt_disable(level);
	*p &= ~v;
	rtems_interrupt_enable(level);
#endif
}

static inline int
atomic_cmpset_32(volatile uint32_t *p, uint32_t cmp, uint32_t set)
{
	int rv;

#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
	std::atomic_uint_least32_t *q =
	    reinterpret_cast<std::atomic_uint_least32_t *>(const_cast<uint32_t *>(p));

	rv = q->compare_exchange_strong(cmp, set, std::memory_order_seq_cst,
	    std::memory_order_relaxed);
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
	atomic_uint_least32_t *q = (atomic_uint_least32_t *)RTEMS_DEVOLATILE(uint32_t *, p);

	rv = atomic_compare_exchange_strong_explicit(q, &cmp, set,
	    memory_order_seq_cst, memory_order_relaxed);
#else
	rtems_interrupt_level level;

	rtems_interrupt_disable(level);
	rv = *p == cmp;
	if (rv) {
		*p = set;
	}
	rtems_interrupt_enable(level);
#endif

	return (rv);
}

static inline int
atomic_cmpset_acq_32(volatile uint32_t *p, uint32_t cmp, uint32_t set)
{
	int rv;

#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
	std::atomic_uint_least32_t *q =
	    reinterpret_cast<std::atomic_uint_least32_t *>(const_cast<uint32_t *>(p));

	rv = q->compare_exchange_strong(cmp, set, std::memory_order_acquire,
	    std::memory_order_relaxed);
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
	atomic_uint_least32_t *q = (atomic_uint_least32_t *)RTEMS_DEVOLATILE(uint32_t *, p);

	rv = atomic_compare_exchange_strong_explicit(q, &cmp, set,
	    memory_order_acquire, memory_order_relaxed);
#else
	rtems_interrupt_level level;

	rtems_interrupt_disable(level);
	rv = *p == cmp;
	if (rv) {
		*p = set;
	}
	rtems_interrupt_enable(level);
#endif

	return (rv);
}

static inline int
atomic_cmpset_rel_32(volatile uint32_t *p, uint32_t cmp, uint32_t set)
{
	int rv;

#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
	std::atomic_uint_least32_t *q =
	    reinterpret_cast<std::atomic_uint_least32_t *>(const_cast<uint32_t *>(p));

	rv = q->compare_exchange_strong(cmp, set, std::memory_order_release,
	    std::memory_order_relaxed);
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
	atomic_uint_least32_t *q = (atomic_uint_least32_t *)RTEMS_DEVOLATILE(uint32_t *, p);

	rv = atomic_compare_exchange_strong_explicit(q, &cmp, set,
	    memory_order_release, memory_order_relaxed);
#else
	rtems_interrupt_level level;

	rtems_interrupt_disable(level);
	rv = *p == cmp;
	if (rv) {
		*p = set;
	}
	rtems_interrupt_enable(level);
#endif

	return (rv);
}

static inline uint32_t
atomic_fetchadd_32(volatile uint32_t *p, uint32_t v)
{
	uint32_t tmp;

#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
	std::atomic_uint_least32_t *q =
	    reinterpret_cast<std::atomic_uint_least32_t *>(const_cast<uint32_t *>(p));

	tmp = q->fetch_add(v, std::memory_order_seq_cst);
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
	atomic_uint_least32_t *q = (atomic_uint_least32_t *)RTEMS_DEVOLATILE(uint32_t *, p);

	tmp = atomic_fetch_add_explicit(q, v, memory_order_seq_cst);
#else
	rtems_interrupt_level level;

	rtems_interrupt_disable(level);
	tmp = *p;
	*p += v;
	rtems_interrupt_enable(level);
#endif

	return (tmp);
}

static inline uint32_t
atomic_readandclear_32(volatile uint32_t *p)
{
	uint32_t tmp;

#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
	std::atomic_uint_least32_t *q =
	    reinterpret_cast<std::atomic_uint_least32_t *>(const_cast<uint32_t *>(p));

	tmp = q->exchange(0, std::memory_order_seq_cst);
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
	atomic_uint_least32_t *q = (atomic_uint_least32_t *)RTEMS_DEVOLATILE(uint32_t *, p);

	tmp = atomic_exchange_explicit(q, 0, memory_order_seq_cst);
#else
	rtems_interrupt_level level;

	rtems_interrupt_disable(level);
	tmp = *p;
	*p = 0;
	rtems_interrupt_enable(level);
#endif

	return (tmp);
}

static inline uint32_t
atomic_load_acq_32(volatile uint32_t *p)
{
	uint32_t tmp;

#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
	std::atomic_uint_least32_t *q =
	    reinterpret_cast<std::atomic_uint_least32_t *>(const_cast<uint32_t *>(p));

	tmp = q->load(std::memory_order_acquire);
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
	atomic_uint_least32_t *q = (atomic_uint_least32_t *)RTEMS_DEVOLATILE(uint32_t *, p);

	tmp = atomic_load_explicit(q, memory_order_acquire);
#else
	RTEMS_COMPILER_MEMORY_BARRIER();
	tmp = *p;
#endif

	return (tmp);
}

static inline void
atomic_store_rel_32(volatile uint32_t *p, uint32_t v)
{
#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
	std::atomic_uint_least32_t *q =
	    reinterpret_cast<std::atomic_uint_least32_t *>(const_cast<uint32_t *>(p));

	q->store(v, std::memory_order_release);
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
	atomic_uint_least32_t *q = (atomic_uint_least32_t *)RTEMS_DEVOLATILE(uint32_t *, p);

	atomic_store_explicit(q, v, memory_order_release);
#else
	*p = v;
	RTEMS_COMPILER_MEMORY_BARRIER();
#endif
}

static inline void
atomic_add_long(volatile long *p, long v)
{
#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
	std::atomic_long *q =
	    reinterpret_cast<std::atomic_long *>(const_cast<long *>(p));

	q->fetch_add(v, std::memory_order_seq_cst);
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
	atomic_long *q = (atomic_long *)RTEMS_DEVOLATILE(long *, p);

	atomic_fetch_add_explicit(q, v, memory_order_seq_cst);
#else
	rtems_interrupt_level level;

	rtems_interrupt_disable(level);
	*p += v;
	rtems_interrupt_enable(level);
#endif
}

static inline void
atomic_add_acq_long(volatile long *p, long v)
{
#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
	std::atomic_long *q =
	    reinterpret_cast<std::atomic_long *>(const_cast<long *>(p));

	q->fetch_add(v, std::memory_order_acquire);
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
	atomic_long *q = (atomic_long *)RTEMS_DEVOLATILE(long *, p);

	atomic_fetch_add_explicit(q, v, memory_order_acquire);
#else
	rtems_interrupt_level level;

	rtems_interrupt_disable(level);
	*p += v;
	rtems_interrupt_enable(level);
#endif
}

static inline void
atomic_add_rel_long(volatile long *p, long v)
{
#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
	std::atomic_long *q =
	    reinterpret_cast<std::atomic_long *>(const_cast<long *>(p));

	q->fetch_add(v, std::memory_order_release);
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
	atomic_long *q = (atomic_long *)RTEMS_DEVOLATILE(long *, p);

	atomic_fetch_add_explicit(q, v, memory_order_release);
#else
	rtems_interrupt_level level;

	rtems_interrupt_disable(level);
	*p += v;
	rtems_interrupt_enable(level);
#endif
}

static inline void
atomic_subtract_long(volatile long *p, long v)
{
#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
	std::atomic_long *q =
	    reinterpret_cast<std::atomic_long *>(const_cast<long *>(p));

	q->fetch_sub(v, std::memory_order_seq_cst);
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
	atomic_long *q = (atomic_long *)RTEMS_DEVOLATILE(long *, p);

	atomic_fetch_sub_explicit(q, v, memory_order_seq_cst);
#else
	rtems_interrupt_level level;

	rtems_interrupt_disable(level);
	*p -= v;
	rtems_interrupt_enable(level);
#endif
}

static inline void
atomic_subtract_acq_long(volatile long *p, long v)
{
#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
	std::atomic_long *q =
	    reinterpret_cast<std::atomic_long *>(const_cast<long *>(p));

	q->fetch_sub(v, std::memory_order_acquire);
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
	atomic_long *q = (atomic_long *)RTEMS_DEVOLATILE(long *, p);

	atomic_fetch_sub_explicit(q, v, memory_order_acquire);
#else
	rtems_interrupt_level level;

	rtems_interrupt_disable(level);
	*p -= v;
	rtems_interrupt_enable(level);
#endif
}

static inline void
atomic_subtract_rel_long(volatile long *p, long v)
{
#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
	std::atomic_long *q =
	    reinterpret_cast<std::atomic_long *>(const_cast<long *>(p));

	q->fetch_sub(v, std::memory_order_release);
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
	atomic_long *q = (atomic_long *)RTEMS_DEVOLATILE(long *, p);

	atomic_fetch_sub_explicit(q, v, memory_order_release);
#else
	rtems_interrupt_level level;

	rtems_interrupt_disable(level);
	*p -= v;
	rtems_interrupt_enable(level);
#endif
}

static inline void
atomic_set_long(volatile long *p, long v)
{
#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
	std::atomic_long *q =
	    reinterpret_cast<std::atomic_long *>(const_cast<long *>(p));

	q->fetch_or(v, std::memory_order_seq_cst);
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
	atomic_long *q = (atomic_long *)RTEMS_DEVOLATILE(long *, p);

	atomic_fetch_or_explicit(q, v, memory_order_seq_cst);
#else
	rtems_interrupt_level level;

	rtems_interrupt_disable(level);
	*p |= v;
	rtems_interrupt_enable(level);
#endif
}

static inline void
atomic_set_acq_long(volatile long *p, long v)
{
#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
	std::atomic_long *q =
	    reinterpret_cast<std::atomic_long *>(const_cast<long *>(p));

	q->fetch_or(v, std::memory_order_acquire);
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
	atomic_long *q = (atomic_long *)RTEMS_DEVOLATILE(long *, p);

	atomic_fetch_or_explicit(q, v, memory_order_acquire);
#else
	rtems_interrupt_level level;

	rtems_interrupt_disable(level);
	*p |= v;
	rtems_interrupt_enable(level);
#endif
}

static inline void
atomic_set_rel_long(volatile long *p, long v)
{
#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
	std::atomic_long *q =
	    reinterpret_cast<std::atomic_long *>(const_cast<long *>(p));

	q->fetch_or(v, std::memory_order_release);
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
	atomic_long *q = (atomic_long *)RTEMS_DEVOLATILE(long *, p);

	atomic_fetch_or_explicit(q, v, memory_order_release);
#else
	rtems_interrupt_level level;

	rtems_interrupt_disable(level);
	*p |= v;
	rtems_interrupt_enable(level);
#endif
}

static inline void
atomic_clear_long(volatile long *p, long v)
{
#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
	std::atomic_long *q =
	    reinterpret_cast<std::atomic_long *>(const_cast<long *>(p));

	q->fetch_and(~v, std::memory_order_seq_cst);
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
	atomic_long *q = (atomic_long *)RTEMS_DEVOLATILE(long *, p);

	atomic_fetch_and_explicit(q, ~v, memory_order_seq_cst);
#else
	rtems_interrupt_level level;

	rtems_interrupt_disable(level);
	*p &= ~v;
	rtems_interrupt_enable(level);
#endif
}

static inline void
atomic_clear_acq_long(volatile long *p, long v)
{
#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
	std::atomic_long *q =
	    reinterpret_cast<std::atomic_long *>(const_cast<long *>(p));

	q->fetch_and(~v, std::memory_order_acquire);
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
	atomic_long *q = (atomic_long *)RTEMS_DEVOLATILE(long *, p);

	atomic_fetch_and_explicit(q, ~v, memory_order_acquire);
#else
	rtems_interrupt_level level;

	rtems_interrupt_disable(level);
	*p &= ~v;
	rtems_interrupt_enable(level);
#endif
}

static inline void
atomic_clear_rel_long(volatile long *p, long v)
{
#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
	std::atomic_long *q =
	    reinterpret_cast<std::atomic_long *>(const_cast<long *>(p));

	q->fetch_and(~v, std::memory_order_release);
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
	atomic_long *q = (atomic_long *)RTEMS_DEVOLATILE(long *, p);

	atomic_fetch_and_explicit(q, ~v, memory_order_release);
#else
	rtems_interrupt_level level;

	rtems_interrupt_disable(level);
	*p &= ~v;
	rtems_interrupt_enable(level);
#endif
}

static inline int
atomic_cmpset_long(volatile long *p, long cmp, long set)
{
	int rv;

#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
	std::atomic_long *q =
	    reinterpret_cast<std::atomic_long *>(const_cast<long *>(p));

	rv = q->compare_exchange_strong(cmp, set, std::memory_order_seq_cst,
	    std::memory_order_relaxed);
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
	atomic_long *q = (atomic_long *)RTEMS_DEVOLATILE(long *, p);

	rv = atomic_compare_exchange_strong_explicit(q, &cmp, set,
	    memory_order_seq_cst, memory_order_relaxed);
#else
	rtems_interrupt_level level;

	rtems_interrupt_disable(level);
	rv = *p == cmp;
	if (rv) {
		*p = set;
	}
	rtems_interrupt_enable(level);
#endif

	return (rv);
}

static inline int
atomic_cmpset_acq_long(volatile long *p, long cmp, long set)
{
	int rv;

#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
	std::atomic_long *q =
	    reinterpret_cast<std::atomic_long *>(const_cast<long *>(p));

	rv = q->compare_exchange_strong(cmp, set, std::memory_order_acquire,
	    std::memory_order_relaxed);
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
	atomic_long *q = (atomic_long *)RTEMS_DEVOLATILE(long *, p);

	rv = atomic_compare_exchange_strong_explicit(q, &cmp, set,
	    memory_order_acquire, memory_order_relaxed);
#else
	rtems_interrupt_level level;

	rtems_interrupt_disable(level);
	rv = *p == cmp;
	if (rv) {
		*p = set;
	}
	rtems_interrupt_enable(level);
#endif

	return (rv);
}

static inline int
atomic_cmpset_rel_long(volatile long *p, long cmp, long set)
{
	int rv;

#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
	std::atomic_long *q =
	    reinterpret_cast<std::atomic_long *>(const_cast<long *>(p));

	rv = q->compare_exchange_strong(cmp, set, std::memory_order_release,
	    std::memory_order_relaxed);
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
	atomic_long *q = (atomic_long *)RTEMS_DEVOLATILE(long *, p);

	rv = atomic_compare_exchange_strong_explicit(q, &cmp, set,
	    memory_order_release, memory_order_relaxed);
#else
	rtems_interrupt_level level;

	rtems_interrupt_disable(level);
	rv = *p == cmp;
	if (rv) {
		*p = set;
	}
	rtems_interrupt_enable(level);
#endif

	return (rv);
}

static inline long
atomic_fetchadd_long(volatile long *p, long v)
{
	long tmp;

#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
	std::atomic_long *q =
	    reinterpret_cast<std::atomic_long *>(const_cast<long *>(p));

	tmp = q->fetch_add(v, std::memory_order_seq_cst);
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
	atomic_long *q = (atomic_long *)RTEMS_DEVOLATILE(long *, p);

	tmp = atomic_fetch_add_explicit(q, v, memory_order_seq_cst);
#else
	rtems_interrupt_level level;

	rtems_interrupt_disable(level);
	tmp = *p;
	*p += v;
	rtems_interrupt_enable(level);
#endif

	return (tmp);
}

static inline long
atomic_readandclear_long(volatile long *p)
{
	long tmp;

#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
	std::atomic_long *q =
	    reinterpret_cast<std::atomic_long *>(const_cast<long *>(p));

	tmp = q->exchange(0, std::memory_order_seq_cst);
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
	atomic_long *q = (atomic_long *)RTEMS_DEVOLATILE(long *, p);

	tmp = atomic_exchange_explicit(q, 0, memory_order_seq_cst);
#else
	rtems_interrupt_level level;

	rtems_interrupt_disable(level);
	tmp = *p;
	*p = 0;
	rtems_interrupt_enable(level);
#endif

	return (tmp);
}

static inline long
atomic_load_acq_long(volatile long *p)
{
	long tmp;

#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
	std::atomic_long *q =
	    reinterpret_cast<std::atomic_long *>(const_cast<long *>(p));

	tmp = q->load(std::memory_order_acquire);
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
	atomic_long *q = (atomic_long *)RTEMS_DEVOLATILE(long *, p);

	tmp = atomic_load_explicit(q, memory_order_acquire);
#else
	RTEMS_COMPILER_MEMORY_BARRIER();
	tmp = *p;
#endif

	return (tmp);
}

static inline void
atomic_store_rel_long(volatile long *p, long v)
{
#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
	std::atomic_long *q =
	    reinterpret_cast<std::atomic_long *>(const_cast<long *>(p));

	q->store(v, std::memory_order_release);
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
	atomic_long *q = (atomic_long *)RTEMS_DEVOLATILE(long *, p);

	atomic_store_explicit(q, v, memory_order_release);
#else
	*p = v;
	RTEMS_COMPILER_MEMORY_BARRIER();
#endif
}

static inline void
atomic_thread_fence_acq(void)
{

#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
	std::atomic_thread_fence(std::memory_order_acquire);
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
	atomic_thread_fence(memory_order_acquire);
#else
	RTEMS_COMPILER_MEMORY_BARRIER();
#endif
}

static inline void
atomic_thread_fence_rel(void)
{

#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
	std::atomic_thread_fence(std::memory_order_release);
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
	atomic_thread_fence(memory_order_release);
#else
	RTEMS_COMPILER_MEMORY_BARRIER();
#endif
}

static inline void
atomic_thread_fence_acq_rel(void)
{

#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
	std::atomic_thread_fence(std::memory_order_acq_rel);
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
	atomic_thread_fence(memory_order_acq_rel);
#else
	RTEMS_COMPILER_MEMORY_BARRIER();
#endif
}

static inline void
atomic_thread_fence_seq_cst(void)
{

#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
	std::atomic_thread_fence(std::memory_order_seq_cst);
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
	atomic_thread_fence(memory_order_seq_cst);
#else
	RTEMS_COMPILER_MEMORY_BARRIER();
#endif
}

#endif /* _RTEMS_BSD_MACHINE_ATOMIC_H_ */
