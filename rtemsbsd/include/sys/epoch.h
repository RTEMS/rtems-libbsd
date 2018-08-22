/*-
 * SPDX-License-Identifier: BSD-2-Clause-FreeBSD
 *
 * Copyright (c) 2018, Matthew Macy <mmacy@freebsd.org>
 * Copyright (c) 2018, embedded brains GmbH
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
 *
 * $FreeBSD$
 */

#ifndef _SYS_EPOCH_H_
#define _SYS_EPOCH_H_

#include <sys/cdefs.h>
#ifdef _KERNEL
#include <sys/lock.h>
#include <sys/pcpu.h>
#include <rtems/score/percpudata.h>
#include <rtems/score/threaddispatch.h>
#endif
#include <rtems/thread.h>
#include <ck_epoch.h>

struct epoch_record;

struct epoch_tracker_mutex {
	SLIST_ENTRY(epoch_tracker_mutex) etm_link; /* link to add to tracker */
	rtems_mutex etm_mtx;
	struct epoch_record *etm_record;
};

typedef struct epoch_tracker {
	ck_epoch_section_t et_section;
	TAILQ_ENTRY(epoch_tracker) et_link;	/* link to add to record */
	struct _Thread_Control *et_td;		/* owner of this tracker */
	SLIST_HEAD(, epoch_tracker_mutex) et_mtx;
} *epoch_tracker_t;
TAILQ_HEAD(epoch_tdlist, epoch_tracker);

typedef struct ck_epoch_entry *epoch_context_t;
#define	epoch_context ck_epoch_entry

#ifdef _KERNEL
#define	EPOCH_ALIGN CPU_CACHE_LINE_BYTES

struct epoch_record {
	ck_epoch_record_t er_record;
	struct epoch_tdlist er_tdlist;
	uint32_t er_cpuid;
} __aligned(EPOCH_ALIGN);

typedef struct epoch {
	struct ck_epoch e_epoch __aligned(EPOCH_ALIGN);
	uintptr_t e_pcpu_record_offset;
	int	e_flags;
	SLIST_ENTRY(epoch) e_link;	/* List of all epochs */
} *epoch_t;

extern struct epoch _bsd_global_epoch;
#define	global_epoch &_bsd_global_epoch

extern struct epoch _bsd_global_epoch_preempt;
#define	global_epoch_preempt &_bsd_global_epoch_preempt

#define	EPOCH_PREEMPT 0x1
#define	EPOCH_LOCKED 0x2

#define	EPOCH_DEFINE(name, flags) \
struct epoch name;							\
static PER_CPU_DATA_ITEM(struct epoch_record, name);			\
static void								\
epoch_##name##_sysinit(void)						\
{									\
									\
	_bsd_epoch_init(&name, PER_CPU_DATA_OFFSET(name), flags);	\
}									\
SYSINIT(epoch_##name, SI_SUB_TUNABLES, SI_ORDER_THIRD,			\
    epoch_##name##_sysinit, NULL)

void	_bsd_epoch_init(epoch_t epoch, uintptr_t pcpu_record_offset,
	    int flags);

void	epoch_enter_preempt(epoch_t epoch, epoch_tracker_t et);
void	epoch_exit_preempt(epoch_t epoch, epoch_tracker_t et);

void	epoch_wait(epoch_t epoch);
void	epoch_wait_preempt(epoch_t epoch);

void	epoch_call(epoch_t epoch, epoch_context_t ctx,
	    void (*callback) (epoch_context_t));

int	in_epoch(epoch_t epoch);
int	in_epoch_verbose(epoch_t epoch, int dump_onfail);

#define	EPOCH_GET_RECORD(cpu_self, epoch) PER_CPU_DATA_GET_BY_OFFSET( \
    cpu_self, struct epoch_record, epoch->e_pcpu_record_offset)

static __inline void
epoch_enter(epoch_t epoch)
{
	Per_CPU_Control *cpu_self;
	struct epoch_record *er;

	cpu_self = _Thread_Dispatch_disable();
	er = EPOCH_GET_RECORD(cpu_self, epoch);
	ck_epoch_begin(&er->er_record, NULL);
}

static __inline void
epoch_exit(epoch_t epoch)
{
	Per_CPU_Control *cpu_self;
	struct epoch_record *er;

	cpu_self = _Per_CPU_Get();
	er = EPOCH_GET_RECORD(cpu_self, epoch);
	ck_epoch_end(&er->er_record, NULL);
	_Thread_Dispatch_enable(cpu_self);
}
#endif /* _KERNEL */
#endif /* _SYS_EPOCH_H_ */
