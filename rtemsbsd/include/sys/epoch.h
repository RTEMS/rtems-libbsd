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
#include <sys/queue.h>
#ifdef _KERNEL
#include <sys/lock.h>
#include <sys/_lock.h>
#include <sys/_mutex.h>
#include <sys/_sx.h>
#include <sys/pcpu.h>
#include <rtems/score/percpudata.h>
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

typedef	void epoch_callback_t(epoch_context_t);

#ifdef _KERNEL
#define	EPOCH_ALIGN CPU_CACHE_LINE_BYTES

struct epoch_record {
	ck_epoch_record_t er_record;
	struct epoch_tdlist er_tdlist;
	uint32_t er_cpuid;
	struct epoch_context er_drain_ctx;
	struct epoch *er_parent;
} __aligned(EPOCH_ALIGN);

typedef struct epoch {
	struct ck_epoch e_epoch __aligned(EPOCH_ALIGN);
	uintptr_t e_pcpu_record_offset;
	int	e_flags;
	SLIST_ENTRY(epoch) e_link;	/* List of all epochs */
	struct sx e_drain_sx;
	struct mtx e_drain_mtx;
	volatile int e_drain_count;
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

void	epoch_enter(epoch_t epoch);
void	epoch_enter_preempt(epoch_t epoch, epoch_tracker_t et);

void	epoch_exit(epoch_t epoch);
void	epoch_exit_preempt(epoch_t epoch, epoch_tracker_t et);

void	epoch_wait(epoch_t epoch);
void	epoch_wait_preempt(epoch_t epoch);

void	epoch_call(epoch_t epoch, void (*callback) (epoch_context_t),
	    epoch_context_t ctx);
void	epoch_drain_callbacks(epoch_t epoch);

int	_bsd_in_epoch(epoch_t epoch);
#define	in_epoch(epoch) _bsd_in_epoch(epoch)
#define	in_epoch_verbose(epoch, dump_onfail) _bsd_in_epoch(epoch)

extern struct epoch _bsd_net_epoch_preempt;
#define	net_epoch_preempt &_bsd_net_epoch_preempt
extern struct epoch _bsd_net_epoch;
#define	net_epoch &_bsd_net_epoch

/*
 * Globally recognized epochs in the FreeBSD kernel.
 */
/* Network preemptible epoch, declared in sys/net/if.c. */
#define	NET_EPOCH_ENTER(et)	epoch_enter_preempt(net_epoch_preempt, &(et))
#define	NET_EPOCH_EXIT(et)	epoch_exit_preempt(net_epoch_preempt, &(et))
#define	NET_EPOCH_WAIT()	epoch_wait_preempt(net_epoch_preempt)
#define	NET_EPOCH_CALL(f, c)	epoch_call(net_epoch_preempt, (f), (c))
#define	NET_EPOCH_DRAIN_CALLBACKS() epoch_drain_callbacks(net_epoch_preempt)
#define	NET_EPOCH_ASSERT()	MPASS(in_epoch(net_epoch_preempt))
#define	NET_TASK_INIT(t, p, f, c) TASK_INIT_FLAGS(t, p, f, c, TASK_NETWORK)
#define	NET_GROUPTASK_INIT(gtask, prio, func, ctx)			\
	    GTASK_INIT(&(gtask)->gt_task, TASK_NETWORK, (prio), (func), (ctx))

#endif /* _KERNEL */
#endif /* _SYS_EPOCH_H_ */
