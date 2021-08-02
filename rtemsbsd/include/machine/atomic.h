/**
 * @file
 *
 * @ingroup rtems_bsd_machine
 *
 * @brief TODO.
 */

/*
 * Copyright (c) 2020 Chris Johns  All rights reserved.
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

/*
 * Load
 */

#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
#define _ATOMIC_LOAD(T, p, mo, barrier) \
	std::atomic_##T *q = \
	    reinterpret_cast<std::atomic_##T *>(const_cast<T *>(p)); \
	tmp = q->load(std::mo)
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
#define _ATOMIC_LOAD(T, p, mo, barrier) \
	atomic_##T *q = (atomic_##T *)RTEMS_DEVOLATILE(T *, p); \
	tmp = atomic_load_explicit(q, mo)
#else
#define _ATOMIC_LOAD(T, p, mo, barrier) \
	if (barrier) \
		RTEMS_COMPILER_MEMORY_BARRIER(); \
	tmp = *p
#endif

static inline int
atomic_load_int(volatile int *p)
{
	int tmp;
	_ATOMIC_LOAD(int, p, memory_order_relaxed, false);
	return (tmp);
}

static inline int
atomic_load_acq_int(volatile int *p)
{
	int tmp;
	_ATOMIC_LOAD(int, p, memory_order_acquire, true);
	return (tmp);
}

static inline uint32_t
atomic_load_32(volatile uint32_t *p)
{
	uint32_t tmp;
	_ATOMIC_LOAD(uint_fast32_t, p, memory_order_relaxed, false);
	return (tmp);
}

static inline uint32_t
atomic_load_acq_32(volatile uint32_t *p)
{
	uint32_t tmp;
	_ATOMIC_LOAD(uint_fast32_t, p, memory_order_acquire, true);
	return (tmp);
}

static inline long
atomic_load_long(volatile long *p)
{
	long tmp;
	_ATOMIC_LOAD(long, p, memory_order_relaxed, false);
	return (tmp);
}

static inline long
atomic_load_acq_long(volatile long *p)
{
	long tmp;
	_ATOMIC_LOAD(long, p, memory_order_acquire, true);
	return (tmp);
}

/*
 * Store
 */

#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
#define _ATOMIC_STORE(T, p, v, mo, barrier) \
	std::atomic_##T *q = \
	    reinterpret_cast<std::atomic_##T *>(const_cast<T *>(p)); \
	q->store(std::mo)
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
#define _ATOMIC_STORE(T, p, v, mo, barrier) \
	atomic_##T *q = (atomic_##T *)RTEMS_DEVOLATILE(T *, p); \
	atomic_store_explicit(q, v, mo)
#else
#define _ATOMIC_STORE(T, p, v, mo, barrier) \
	*p = v; \
	if (barrier) \
		RTEMS_COMPILER_MEMORY_BARRIER()
#endif

static inline void
atomic_store_int(volatile int *p, int v)
{
	_ATOMIC_STORE(int, p, v, memory_order_relaxed, false);
}

static inline void
atomic_store_rel_int(volatile int *p, int v)
{
	_ATOMIC_STORE(int, p, v, memory_order_release, true);
}

static inline void
atomic_store_rel_32(volatile uint32_t *p, uint32_t v)
{
	_ATOMIC_STORE(uint_fast32_t, p, v, memory_order_release, true);
}

static inline void
atomic_store_rel_long(volatile long *p, long v)
{
	_ATOMIC_STORE(long, p, v, memory_order_release, true);
}

static inline void
atomic_store_rel_ptr(volatile uintptr_t *p, uintptr_t v)
{
	/* XXX IPL32 ok with this? */
	_ATOMIC_STORE(uintptr_t, p, v, memory_order_release, true);
}

/*
 * Add
 */

#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
#define _ATOMIC_ADD(T, p, v, mo) \
	std::atomic_##T *q = \
	    reinterpret_cast<std::atomic_##T *>(const_cast<T *>(p)); \
	q->fetch_add(v, std::mo)
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
#define _ATOMIC_ADD(T, p, v, mo) \
	atomic_##T *q = (atomic_##T *)RTEMS_DEVOLATILE(T *, p); \
	atomic_fetch_add_explicit(q, v, mo)
#else
#define _ATOMIC_ADD(T, p, v, mo) \
	rtems_interrupt_level level; \
	rtems_interrupt_disable(level); \
	*p += v; \
	rtems_interrupt_enable(level)
#endif

static inline void
atomic_add_int(volatile int *p, int v)
{
	_ATOMIC_ADD(int, p, v, memory_order_seq_cst);
}

static inline void
atomic_add_acq_int(volatile int *p, int v)
{
	_ATOMIC_ADD(int, p, v, memory_order_acquire);
}

static inline void
atomic_add_rel_int(volatile int *p, int v)
{
	_ATOMIC_ADD(int, p, v, memory_order_release);
}

static inline void
atomic_add_32(volatile uint32_t *p, uint32_t v)
{
	_ATOMIC_ADD(uint_fast32_t, p, v, memory_order_seq_cst);
}

static inline void
atomic_add_acq_32(volatile uint32_t *p, uint32_t v)
{
	_ATOMIC_ADD(uint_fast32_t, p, v, memory_order_acquire);
}

static inline void
atomic_add_rel_32(volatile uint32_t *p, uint32_t v)
{
	_ATOMIC_ADD(uint_fast32_t, p, v, memory_order_release);
}

static inline void
atomic_add_long(volatile long *p, long v)
{
	_ATOMIC_ADD(long, p, v, memory_order_seq_cst);
}

static inline void
atomic_add_acq_long(volatile long *p, long v)
{
	_ATOMIC_ADD(long, p, v, memory_order_acquire);
}

static inline void
atomic_add_rel_long(volatile long *p, long v)
{
	_ATOMIC_ADD(long, p, v, memory_order_release);
}

/*
 * Subtract
 */

#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
#define _ATOMIC_SUB(T, p, v, mo) \
	std::atomic_##T *q = \
	    reinterpret_cast<std::atomic_##T *>(const_cast<T *>(p)); \
	q->fetch_sub(v, std::mo)
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
#define _ATOMIC_SUB(T, p, v, mo) \
	atomic_##T *q = (atomic_##T *)RTEMS_DEVOLATILE(T *, p); \
	atomic_fetch_sub_explicit(q, v, mo)
#else
#define _ATOMIC_SUB(T, p, v, mo) \
	rtems_interrupt_level level; \
	rtems_interrupt_disable(level); \
	*p -= v; \
	rtems_interrupt_enable(level)
#endif

static inline void
atomic_subtract_int(volatile int *p, int v)
{
	_ATOMIC_SUB(int, p, v, memory_order_seq_cst);
}

static inline void
atomic_subtract_acq_int(volatile int *p, int v)
{
	_ATOMIC_SUB(int, p, v, memory_order_acquire);
}

static inline void
atomic_subtract_rel_int(volatile int *p, int v)
{
	_ATOMIC_SUB(int, p, v, memory_order_release);
}

static inline void
atomic_subtract_32(volatile uint32_t *p, uint32_t v)
{
	_ATOMIC_SUB(uint_fast32_t, p, v, memory_order_seq_cst);
}

static inline void
atomic_subtract_acq_32(volatile uint32_t *p, uint32_t v)
{
	_ATOMIC_SUB(uint_fast32_t, p, v, memory_order_acquire);
}

static inline void
atomic_subtract_rel_32(volatile uint32_t *p, uint32_t v)
{
	_ATOMIC_SUB(uint_fast32_t, p, v, memory_order_release);
}

static inline void
atomic_subtract_long(volatile long *p, long v)
{
	_ATOMIC_SUB(long, p, v, memory_order_seq_cst);
}

static inline void
atomic_subtract_acq_long(volatile long *p, long v)
{
	_ATOMIC_SUB(long, p, v, memory_order_acquire);
}

static inline void
atomic_subtract_rel_long(volatile long *p, long v)
{
	_ATOMIC_SUB(long, p, v, memory_order_release);
}

/*
 * Set
 */

#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
#define _ATOMIC_SET(T, p, v, mo) \
	std::atomic_##T *q = \
	    reinterpret_cast<std::atomic_##T *>(const_cast<T *>(p)); \
	q->fetch_or(v, std::mo)
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
#define _ATOMIC_SET(T, p, v, mo) \
	atomic_##T *q = (atomic_##T *)RTEMS_DEVOLATILE(T *, p); \
	atomic_fetch_or_explicit(q, v, mo)
#else
#define _ATOMIC_SET(T, p, v, mo) \
	rtems_interrupt_level level; \
	rtems_interrupt_disable(level); \
	*p |= v; \
	rtems_interrupt_enable(level)
#endif

static inline void
atomic_set_int(volatile int *p, int v)
{
	_ATOMIC_SET(int, p, v, memory_order_seq_cst);
}

static inline void
atomic_set_acq_int(volatile int *p, int v)
{
	_ATOMIC_SET(int, p, v, memory_order_acquire);
}

static inline void
atomic_set_rel_int(volatile int *p, int v)
{
	_ATOMIC_SET(int, p, v, memory_order_release);
}

static inline void
atomic_set_32(volatile uint32_t *p, uint32_t v)
{
	_ATOMIC_SET(uint_fast32_t, p, v, memory_order_seq_cst);
}

static inline void
atomic_set_acq_32(volatile uint32_t *p, uint32_t v)
{
	_ATOMIC_SET(uint_fast32_t, p, v, memory_order_acquire);
}

static inline void
atomic_set_rel_32(volatile uint32_t *p, uint32_t v)
{
	_ATOMIC_SET(uint_fast32_t, p, v, memory_order_release);
}

static inline void
atomic_set_long(volatile long *p, long v)
{
	_ATOMIC_SET(long, p, v, memory_order_seq_cst);
}

static inline void
atomic_set_acq_long(volatile long *p, long v)
{
	_ATOMIC_SET(long, p, v, memory_order_acquire);
}

static inline void
atomic_set_rel_long(volatile long *p, long v)
{
	_ATOMIC_SET(long, p, v, memory_order_release);
}

/*
 * Clear
 */

#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
#define _ATOMIC_CLEAR(T, p, v, mo) \
	std::atomic_##T *q = \
	    reinterpret_cast<std::atomic_##T *>(const_cast<T *>(p)); \
	q->fetch_and(~v, std::mo)
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
#define _ATOMIC_CLEAR(T, p, v, mo) \
	atomic_##T *q = (atomic_##T *)RTEMS_DEVOLATILE(T *, p); \
	atomic_fetch_and_explicit(q, ~v, mo)
#else
#define _ATOMIC_CLEAR(T, p, v, mo) \
	rtems_interrupt_level level; \
	rtems_interrupt_disable(level); \
	*p &= ~v; \
	rtems_interrupt_enable(level)
#endif

static inline void
atomic_clear_int(volatile int *p, int v)
{
	_ATOMIC_CLEAR(int, p, v, memory_order_seq_cst);
}

static inline void
atomic_clear_acq_int(volatile int *p, int v)
{
	_ATOMIC_CLEAR(int, p, v, memory_order_acquire);
}

static inline void
atomic_clear_rel_int(volatile int *p, int v)
{
	_ATOMIC_CLEAR(int, p, v, memory_order_release);
}

static inline void
atomic_clear_32(volatile uint32_t *p, uint32_t v)
{
	_ATOMIC_CLEAR(uint_fast32_t, p, v, memory_order_seq_cst);
}

static inline void
atomic_clear_acq_32(volatile uint32_t *p, uint32_t v)
{
	_ATOMIC_CLEAR(uint_fast32_t, p, v, memory_order_acquire);
}

static inline void
atomic_clear_rel_32(volatile uint32_t *p, uint32_t v)
{
	_ATOMIC_CLEAR(uint_fast32_t, p, v, memory_order_release);
}

static inline void
atomic_clear_long(volatile long *p, long v)
{
	_ATOMIC_CLEAR(long, p, v, memory_order_release);
}

static inline void
atomic_clear_acq_long(volatile long *p, long v)
{
	_ATOMIC_CLEAR(long, p, v, memory_order_acquire);
}

static inline void
atomic_clear_rel_long(volatile long *p, long v)
{
	_ATOMIC_CLEAR(long, p, v, memory_order_release);
}

/*
 * Compare and set
 */

#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
#define _ATOMIC_CMPSET(T, p, cmp, set, mo) \
	std::atomic_##T *q = \
	    reinterpret_cast<std::atomic_##T *>(const_cast<T *>(p)); \
	rv = q->compare_exchange_strong(cmp, set, \
            std::mo, std::memory_order_relaxed)
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
#define _ATOMIC_CMPSET(T, p, cmp, set, mo) \
	atomic_##T *q = (atomic_##T *)RTEMS_DEVOLATILE(T *, p); \
	atomic_compare_exchange_strong_explicit(q, &cmp, set, \
            mo, memory_order_relaxed)
#else
#define _ATOMIC_CMPSET(T, p, cmp, set, mo) \
	rtems_interrupt_level level; \
	rtems_interrupt_disable(level); \
	rv = *p == cmp; \
	if (rv) { \
		*p = set; \
	} \
	rtems_interrupt_enable(level);
#endif

static inline int
atomic_cmpset_int(volatile int *p, int cmp, int set)
{
	int rv;
	_ATOMIC_CMPSET(int, p, cmp, set, memory_order_seq_cst);
	return (rv);
}

static inline int
atomic_cmpset_acq_int(volatile int *p, int cmp, int set)
{
	int rv;
	_ATOMIC_CMPSET(int, p, cmp, set, memory_order_acquire);
	return (rv);
}

static inline int
atomic_cmpset_rel_int(volatile int *p, int cmp, int set)
{
	int rv;
	_ATOMIC_CMPSET(int, p, cmp, set, memory_order_release);
	return (rv);
}

static inline int
atomic_cmpset_32(volatile uint32_t *p, uint32_t cmp, uint32_t set)
{
	int rv;
	_ATOMIC_CMPSET(uint_least32_t, p, cmp, set, memory_order_seq_cst);
	return (rv);
}

static inline int
atomic_cmpset_acq_32(volatile uint32_t *p, uint32_t cmp, uint32_t set)
{
	int rv;
	_ATOMIC_CMPSET(uint_least32_t, p, cmp, set, memory_order_acquire);
	return (rv);
}

static inline int
atomic_cmpset_rel_32(volatile uint32_t *p, uint32_t cmp, uint32_t set)
{
	int rv;
	_ATOMIC_CMPSET(uint_least32_t, p, cmp, set, memory_order_release);
	return (rv);
}

static inline int
atomic_cmpset_64(volatile uint64_t *p, uint64_t cmp, uint64_t set)
{
	int rv;
	_ATOMIC_CMPSET(uint_least64_t, p, cmp, set, memory_order_seq_cst);
	return (rv);
}

static inline int
atomic_cmpset_acq_64(volatile uint64_t *p, uint64_t cmp, uint64_t set)
{
	int rv;
	_ATOMIC_CMPSET(uint_least64_t, p, cmp, set, memory_order_acquire);
	return (rv);
}

static inline int
atomic_cmpset_rel_64(volatile uint64_t *p, uint64_t cmp, uint64_t set)
{
	int rv;
	_ATOMIC_CMPSET(uint_least64_t, p, cmp, set, memory_order_release);
	return (rv);
}

static inline int
atomic_cmpset_long(volatile long *p, long cmp, long set)
{
	int rv;
	_ATOMIC_CMPSET(long, p, cmp, set, memory_order_seq_cst);
	return (rv);
}

static inline int
atomic_cmpset_acq_long(volatile long *p, long cmp, long set)
{
	int rv;
	_ATOMIC_CMPSET(long, p, cmp, set, memory_order_acquire);
	return (rv);
}

static inline int
atomic_cmpset_rel_long(volatile long *p, long cmp, long set)
{
	int rv;
	_ATOMIC_CMPSET(long, p, cmp, set, memory_order_release);
	return (rv);
}

static inline int
atomic_cmpset_ptr(volatile uintptr_t *p, uintptr_t cmp, uintptr_t set)
{
	int rv;
	_ATOMIC_CMPSET(uintptr_t, p, cmp, set, memory_order_seq_cst);
	return (rv);
}

static inline int
atomic_cmpset_acq_ptr(volatile uintptr_t *p, uintptr_t cmp, uintptr_t set)
{
	int rv;
	_ATOMIC_CMPSET(uintptr_t, p, cmp, set, memory_order_acquire);
	return (rv);
}

static inline int
atomic_cmpset_rel_ptr(volatile uintptr_t *p, uintptr_t cmp, uintptr_t set)
{
	int rv;
	_ATOMIC_CMPSET(uintptr_t, p, cmp, set, memory_order_release);
	return (rv);
}

/*
 * Fetch compare and set
 */

#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
#define _ATOMIC_FCMPSET(T, p, cmp, set, mo) \
	std::atomic_##T *q = \
	    reinterpret_cast<std::atomic_##T *>(const_cast<T *>(p)); \
	rv = q->compare_exchange_strong(*cmp, set, \
            std::mo, std::memory_order_relaxed)
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
#define _ATOMIC_FCMPSET(T, p, cmp, set, mo) \
	atomic_##T *q = (atomic_##T *)RTEMS_DEVOLATILE(T *, p); \
	atomic_compare_exchange_strong_explicit(q, cmp, set, \
           mo, memory_order_relaxed)
#else
#define _ATOMIC_FCMPSET(T, p, cmp, set, mo) \
	rtems_interrupt_level level; \
        T actual; \
	rtems_interrupt_disable(level); \
	actual = *p; \
	rv = actual == *cmp; \
	*cmp = actual; \
	if (rv) { \
		*p = set; \
	} \
	rtems_interrupt_enable(level);
#endif

static inline int
atomic_fcmpset_int(volatile int *p, int *cmp, int set)
{
	int rv;
	_ATOMIC_FCMPSET(int, p, cmp, set, memory_order_seq_cst);
	return (rv);
}

static inline int
atomic_fcmpset_acq_int(volatile int *p, int *cmp, int set)
{
	int rv;
	_ATOMIC_FCMPSET(int, p, cmp, set, memory_order_acquire);
	return (rv);
}

static inline int
atomic_fcmpset_rel_int(volatile int *p, int *cmp, int set)
{
	int rv;
	_ATOMIC_FCMPSET(int, p, cmp, set, memory_order_release);
	return (rv);
}

static inline int
atomic_fcmpset_64(volatile uint64_t *p, uint64_t *cmp, uint64_t set)
{
	int rv;
	_ATOMIC_FCMPSET(uint_least64_t, p, cmp, set, memory_order_seq_cst);
	return (rv);
}

static inline int
atomic_fcmpset_acq_64(volatile uint64_t *p, uint64_t *cmp, uint64_t set)
{
	int rv;
	_ATOMIC_FCMPSET(uint_least64_t, p, cmp, set, memory_order_acquire);
	return (rv);
}

static inline int
atomic_fcmpset_rel_64(volatile uint64_t *p, uint64_t *cmp, uint64_t set)
{
	int rv;
	_ATOMIC_FCMPSET(uint_least64_t, p, cmp, set, memory_order_release);
	return (rv);
}

/*  XXX Is IPL32 ok with the .*_ptr versions? */
static inline int
atomic_fcmpset_ptr(volatile uintptr_t *p, uintptr_t *cmp, uintptr_t set)
{
	int rv;
	_ATOMIC_FCMPSET(uintptr_t, p, cmp, set, memory_order_seq_cst);
	return (rv);
}

static inline int
atomic_fcmpset_acq_ptr(volatile uintptr_t *p, uintptr_t *cmp, uintptr_t set)
{
	int rv;
	_ATOMIC_FCMPSET(uintptr_t, p, cmp, set, memory_order_acquire);
	return (rv);
}

static inline int
atomic_fcmpset_rel_ptr(volatile uintptr_t *p, uintptr_t *cmp, uintptr_t set)
{
	int rv;
	_ATOMIC_FCMPSET(uintptr_t, p, cmp, set, memory_order_release);
	return (rv);
}

/*
 * Fetch add
 */

#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
#define _ATOMIC_FETCHADD(T, p, v, mo) \
	std::atomic_##T *q = \
	    reinterpret_cast<std::atomic_##T *>(const_cast<T *>(p)); \
	tmp = q->fetch_add(v, std::mo)
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
#define _ATOMIC_FETCHADD(T, p, v, mo) \
	atomic_##T *q = (atomic_##T *)RTEMS_DEVOLATILE(T *, p); \
	tmp = atomic_fetch_add_explicit(q, v, mo)
#else
#define _ATOMIC_FETCHADD(T, p, v, mo) \
	rtems_interrupt_level level; \
	rtems_interrupt_disable(level); \
	tmp = *p; \
	*p += v; \
	rtems_interrupt_enable(level);
#endif

static inline int
atomic_fetchadd_int(volatile int *p, int v)
{
	int tmp;
	_ATOMIC_FETCHADD(int, p, v, memory_order_seq_cst);
	return (tmp);
}

static inline uint32_t
atomic_fetchadd_32(volatile uint32_t *p, uint32_t v)
{
	uint32_t tmp;
	_ATOMIC_FETCHADD(uint_fast32_t, p, v, memory_order_seq_cst);
	return (tmp);
}

static inline uint64_t
atomic_fetchadd_64(volatile uint64_t *p, uint64_t v)
{
	uint64_t tmp;
	_ATOMIC_FETCHADD(uint_fast64_t, p, v, memory_order_seq_cst);
	return (tmp);
}

static inline long
atomic_fetchadd_long(volatile long *p, long v)
{
	long tmp;
	_ATOMIC_FETCHADD(long, p, v, memory_order_seq_cst);
	return (tmp);
}

/*
 * Read and clear
 */

#if defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_ATOMIC)
#define _ATOMIC_READANDCLEAR(T, p, mo) \
	std::atomic_##T *q = \
	    reinterpret_cast<std::atomic_##T *>(const_cast<T *>(p)); \
	tmp = q->exchange(0, std::mo)
#elif defined(_RTEMS_BSD_MACHINE_ATOMIC_USE_STDATOMIC)
#define _ATOMIC_READANDCLEAR(T, p, mo) \
	atomic_##T *q = (atomic_##T *)RTEMS_DEVOLATILE(T *, p); \
	tmp = atomic_exchange_explicit(q, 0, mo)
#else
#define _ATOMIC_READANDCLEAR(T, p, mo) \
	rtems_interrupt_level level; \
	rtems_interrupt_disable(level); \
	tmp = *p; \
	*p = 0; \
	rtems_interrupt_enable(level);
#endif

static inline int
atomic_readandclear_int(volatile int *p)
{
	int tmp;
	_ATOMIC_READANDCLEAR(int, p, memory_order_seq_cst);
	return (tmp);
}

static inline uint32_t
atomic_readandclear_32(volatile uint32_t *p)
{
	uint32_t tmp;
	_ATOMIC_READANDCLEAR(uint_least32_t, p, memory_order_seq_cst);
	return (tmp);
}

static inline long
atomic_readandclear_long(volatile long *p)
{
	long tmp;
	_ATOMIC_READANDCLEAR(long, p, memory_order_seq_cst);
	return (tmp);
}

/*
 * Thread fence
 */

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
