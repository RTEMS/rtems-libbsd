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

#include <machine/rtems-bsd-kernel-space.h>

#include <sys/types.h>
#include <sys/kernel.h>
#include <sys/mutex.h>
#include <sys/epoch.h>
#include <sys/sx.h>
#include <sys/systm.h>

#include <machine/atomic.h>
#include <machine/cpu.h>

#include <rtems.h>
#include <rtems/irq-extension.h>
#include <rtems/score/smpimpl.h>
#include <rtems/score/threaddispatch.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/watchdogimpl.h>

#define	EPOCH_GET_RECORD(cpu_self, epoch) PER_CPU_DATA_GET_BY_OFFSET( \
    cpu_self, struct epoch_record, epoch->e_pcpu_record_offset)

EPOCH_DEFINE(_bsd_global_epoch_preempt, EPOCH_PREEMPT);

EPOCH_DEFINE(_bsd_global_epoch, 0);

struct epoch_pcpu {
	int cb_count;
	Watchdog_Control wdg;
	rtems_interrupt_server_request irq_srv_req;
};

#ifdef PER_CPU_DATA_NEED_INITIALIZATION
PER_CPU_DATA_NEED_INITIALIZATION();
#endif

static PER_CPU_DATA_ITEM(struct epoch_pcpu, epoch);

static SLIST_HEAD(, epoch) epoch_list = SLIST_HEAD_INITIALIZER(epoch_list);

CK_STACK_CONTAINER(struct ck_epoch_entry, stack_entry,
    ck_epoch_entry_container)

void
_bsd_epoch_init(epoch_t epoch, uintptr_t pcpu_record_offset, int flags)
{
	uint32_t cpu_count;
	uint32_t cpu_index;

	ck_epoch_init(&epoch->e_epoch);
	epoch->e_flags = flags;
	epoch->e_pcpu_record_offset = pcpu_record_offset;
	sx_init(&epoch->e_drain_sx, "epoch-drain-sx");
	mtx_init(&epoch->e_drain_mtx, "epoch-drain-mtx", NULL, MTX_DEF);

	cpu_count = rtems_scheduler_get_processor_maximum();

	for (cpu_index = 0; cpu_index < cpu_count; ++cpu_index) {
		Per_CPU_Control *cpu;
		struct epoch_record *er;

		cpu = _Per_CPU_Get_by_index(cpu_index);
		er = EPOCH_GET_RECORD(cpu, epoch);
		bzero(er, sizeof(*er));
		ck_epoch_register(&epoch->e_epoch, &er->er_record, NULL);
		TAILQ_INIT(__DEVOLATILE(struct epoch_tdlist *,
		    &er->er_tdlist));
		er->er_cpuid = cpu_index;
		er->er_parent = epoch;
	}

	SLIST_INSERT_HEAD(&epoch_list, epoch, e_link);
}

static void
epoch_watchdog(Watchdog_Control *wdg)
{
	struct epoch_pcpu *epcpu;
	ISR_Level level;

	epcpu = __containerof(wdg, struct epoch_pcpu, wdg);

	_ISR_Local_disable(level);
	_Watchdog_Per_CPU_insert_ticks(&epcpu->wdg,
	    _Watchdog_Get_CPU(&epcpu->wdg), 1);
	_ISR_Local_enable(level);

	if (RTEMS_PREDICT_FALSE(epcpu->cb_count != 0)) {
		rtems_interrupt_server_request_submit(&epcpu->irq_srv_req);
	}
}

static void
epoch_call_handler(void *arg)
{
	struct epoch_pcpu *epcpu;
	struct epoch *epoch;
	ck_stack_entry_t *cursor, *head, *next;
	ck_stack_t cb_stack;

	epcpu = arg;
	ck_stack_init(&cb_stack);

	SLIST_FOREACH(epoch, &epoch_list, e_link) {
		Per_CPU_Control *cpu_self;
		struct epoch_record *er;
		int npending;

		cpu_self = _Thread_Dispatch_disable();
		er = EPOCH_GET_RECORD(cpu_self, epoch);
		npending = er->er_record.n_pending;

		if (npending != 0) {
			ck_epoch_poll_deferred(&er->er_record, &cb_stack);
			epcpu->cb_count -= npending - er->er_record.n_pending;
		}

		_Thread_Dispatch_enable(cpu_self);
	}

	head = ck_stack_batch_pop_npsc(&cb_stack);
	for (cursor = head; cursor != NULL; cursor = next) {
		struct ck_epoch_entry *entry;

		entry = ck_epoch_entry_container(cursor);

		next = CK_STACK_NEXT(cursor);
		(*entry->function)(entry);
	}
}

static void
epoch_sysinit(void)
{
	uint32_t cpu_count;
	uint32_t cpu_index;

	cpu_count = rtems_scheduler_get_processor_maximum();

	for (cpu_index = 0; cpu_index < cpu_count; ++cpu_index) {
		Per_CPU_Control *cpu;
		struct epoch_pcpu *epcpu;
		ISR_Level level;

		cpu = _Per_CPU_Get_by_index(cpu_index);
		epcpu = PER_CPU_DATA_GET(cpu, struct epoch_pcpu, epoch);

		_Watchdog_Preinitialize(&epcpu->wdg, cpu);
		_Watchdog_Initialize(&epcpu->wdg, epoch_watchdog);
		_ISR_Local_disable(level);
		_Watchdog_Per_CPU_insert_ticks(&epcpu->wdg, cpu, 1);
		_ISR_Local_enable(level);

		rtems_interrupt_server_request_initialize(cpu_index,
		    &epcpu->irq_srv_req, epoch_call_handler, epcpu);
	}
}
SYSINIT(epoch, SI_SUB_TUNABLES, SI_ORDER_SECOND, epoch_sysinit, NULL);

void
epoch_enter(epoch_t epoch)
{
	Per_CPU_Control *cpu_self;
	struct epoch_record *er;

	cpu_self = _Thread_Dispatch_disable();
	er = EPOCH_GET_RECORD(cpu_self, epoch);
	ck_epoch_begin(&er->er_record, NULL);
}

void
epoch_exit(epoch_t epoch)
{
	Per_CPU_Control *cpu_self;
	struct epoch_record *er;

	cpu_self = _Per_CPU_Get();
	er = EPOCH_GET_RECORD(cpu_self, epoch);
	ck_epoch_end(&er->er_record, NULL);
	_Thread_Dispatch_enable(cpu_self);
}

void
epoch_enter_preempt(epoch_t epoch, epoch_tracker_t et)
{
	Per_CPU_Control *cpu_self;
	ISR_lock_Context lock_context;
	Thread_Control *executing;
	struct epoch_record *er;

	SLIST_INIT(&et->et_mtx);

	_ISR_lock_ISR_disable(&lock_context);
	cpu_self = _Thread_Dispatch_disable_critical(&lock_context);
	executing = _Per_CPU_Get_executing(cpu_self);
	er = EPOCH_GET_RECORD(cpu_self, epoch);
	TAILQ_INSERT_TAIL(&er->er_tdlist, et, et_link);
	et->et_td = executing;
	_ISR_lock_ISR_enable(&lock_context);

	ck_epoch_begin(&er->er_record, &et->et_section);

	_Thread_Pin(executing);
	_Thread_Dispatch_enable(cpu_self);
}

void
epoch_exit_preempt(epoch_t epoch, epoch_tracker_t et)
{
	Per_CPU_Control *cpu_self;
	ISR_lock_Context lock_context;
	Thread_Control *executing;
	struct epoch_record *er;
	struct epoch_tracker_mutex *etm;

	_ISR_lock_ISR_disable(&lock_context);
	cpu_self = _Thread_Dispatch_disable_critical(&lock_context);
	executing = _Per_CPU_Get_executing(cpu_self);
	er = EPOCH_GET_RECORD(cpu_self, epoch);
	TAILQ_REMOVE(&er->er_tdlist, et, et_link);
	_ISR_lock_ISR_enable(&lock_context);

	ck_epoch_end(&er->er_record, &et->et_section);

	_Thread_Unpin(executing, cpu_self);
	_Thread_Dispatch_enable(cpu_self);

	SLIST_FOREACH(etm, &et->et_mtx, etm_link) {
		rtems_mutex_unlock(&etm->etm_mtx);
	}
}

static void
epoch_block_handler(struct ck_epoch *g __unused, ck_epoch_record_t *c __unused,
    void *arg __unused)
{
	cpu_spinwait();
}

void
epoch_wait(epoch_t epoch)
{
	Per_CPU_Control *cpu_self;

	cpu_self = _Thread_Dispatch_disable();
	ck_epoch_synchronize_wait(&epoch->e_epoch, epoch_block_handler, NULL);
	_Thread_Dispatch_enable(cpu_self);
}

static void
epoch_register_mutex(void *arg)
{
	struct epoch_tracker_mutex *etm;
	struct epoch_record *er;
	struct epoch_tracker *et;

	etm = arg;
	er = etm->etm_record;
	et = TAILQ_FIRST(&er->er_tdlist);

	if (et != NULL) {
		etm->etm_mtx._Queue._owner = et->et_td;
		_Thread_Resource_count_increment( et->et_td );
		SLIST_INSERT_HEAD(&et->et_mtx, etm, etm_link);
	}
}

static void
epoch_block_handler_preempt(struct ck_epoch *g __unused,
    ck_epoch_record_t *cr, void *arg __unused)
{
	struct epoch_record *er;
	Per_CPU_Control *cpu_self;
	struct epoch_tracker_mutex etm;
#ifdef RTEMS_SMP
	uint32_t cpu_self_index;
#endif

	er = __containerof(cr, struct epoch_record, er_record);
	cpu_self = _Per_CPU_Get();
#ifdef RTEMS_SMP
	cpu_self_index = _Per_CPU_Get_index(cpu_self);
#endif

	rtems_mutex_init(&etm.etm_mtx, "epoch");
	etm.etm_record = er;

#ifdef RTEMS_SMP
	if (cpu_self_index != er->er_cpuid) {
		_SMP_Unicast_action(er->er_cpuid, epoch_register_mutex, &etm);
	} else {
		epoch_register_mutex(&etm);
	}
#else
	epoch_register_mutex(&etm);
#endif

	_Thread_Dispatch_enable(cpu_self);

	rtems_mutex_lock(&etm.etm_mtx);
	rtems_mutex_unlock(&etm.etm_mtx);
	rtems_mutex_destroy(&etm.etm_mtx);

	_Thread_Dispatch_disable();
}

void
epoch_wait_preempt(epoch_t epoch)
{

	_Thread_Dispatch_disable();
	ck_epoch_synchronize_wait(&epoch->e_epoch, epoch_block_handler_preempt,
	    NULL);
	_Thread_Dispatch_enable(_Per_CPU_Get());
}

void
epoch_call(epoch_t epoch, epoch_context_t ctx,
    void (*callback) (epoch_context_t))
{
	Per_CPU_Control *cpu_self;
	struct epoch_pcpu *epcpu;
	struct epoch_record *er;
#ifdef RTEMS_SMP
	ISR_Level level;
	Thread_Control *executing;

	_ISR_Local_disable(level);
	cpu_self = _Per_CPU_Get();
	executing = _Per_CPU_Get_executing(cpu_self);
	_Thread_Pin(executing);
	_ISR_Local_enable(level);
#endif

	epcpu = PER_CPU_DATA_GET(cpu_self, struct epoch_pcpu, epoch);
	epcpu->cb_count += 1;
	er = EPOCH_GET_RECORD(cpu_self, epoch);
	ck_epoch_call(&er->er_record, ctx, callback);

#ifdef RTEMS_SMP
	cpu_self = _Thread_Dispatch_disable();
	_Thread_Unpin(executing, cpu_self);
	_Thread_Dispatch_enable(cpu_self);
#endif
}

#ifdef INVARIANTS
int
_bsd_in_epoch(epoch_t epoch)
{
	Per_CPU_Control *cpu_self;
	Thread_Control *executing;
	struct epoch_record *er;
	struct epoch_pcpu *epcpu;
	struct epoch_tracker *tdwait;
	int in;

	in = 0;
	cpu_self = _Thread_Dispatch_disable();
	executing = _Per_CPU_Get_executing(cpu_self);
	epcpu = PER_CPU_DATA_GET(cpu_self, struct epoch_pcpu, epoch);
	er = EPOCH_GET_RECORD(cpu_self, epoch);

	TAILQ_FOREACH(tdwait, &er->er_tdlist, et_link) {
		if (tdwait->et_td == executing) {
			in = 1;
			break;
		}
	}

	_Thread_Dispatch_enable(cpu_self);
	return (in);
}
#endif

static void
epoch_drain_cb(struct epoch_context *ctx)
{
	struct epoch *epoch =
	    __containerof(ctx, struct epoch_record, er_drain_ctx)->er_parent;

	if (atomic_fetchadd_int(&epoch->e_drain_count, -1) == 1) {
		mtx_lock(&epoch->e_drain_mtx);
		wakeup(epoch);
		mtx_unlock(&epoch->e_drain_mtx);
	}
}

#ifdef RTEMS_SMP
static void
epoch_call_drain_cb(void *arg)
{
	epoch_t epoch;
	Per_CPU_Control *cpu;
	struct epoch_record *er;

	epoch = arg;
	cpu = _Per_CPU_Get_snapshot();
	er = EPOCH_GET_RECORD(cpu, epoch);
	epoch_call(epoch, &er->er_drain_ctx, epoch_drain_cb);
}
#endif

void
epoch_drain_callbacks(epoch_t epoch)
{
#ifdef RTEMS_SMP
	uint32_t cpu_index;
	uint32_t cpu_max;
	rtems_id id;
	rtems_status_code sc;
	rtems_interrupt_server_request req[CPU_MAXIMUM_PROCESSORS];
#else
	struct epoch_record *er;
#endif

	sx_xlock(&epoch->e_drain_sx);
	mtx_lock(&epoch->e_drain_mtx);

#ifdef RTEMS_SMP
	memset(&req, 0, sizeof(req));
	cpu_max = rtems_scheduler_get_processor_maximum();

	for (cpu_index = 0; cpu_index <= cpu_max; ++cpu_index) {
		sc = rtems_scheduler_ident_by_processor(cpu_index, &id);
		if (sc == RTEMS_SUCCESSFUL) {
			epoch->e_drain_count++;
		}
	}

	for (cpu_index = 0; cpu_index <= cpu_max; ++cpu_index) {
		sc = rtems_scheduler_ident_by_processor(cpu_index, &id);
		if (sc == RTEMS_SUCCESSFUL) {
			sc = rtems_interrupt_server_request_initialize(
			    cpu_index, &req[cpu_index], epoch_call_drain_cb,
			    epoch);
			if (sc == RTEMS_SUCCESSFUL) {
				rtems_interrupt_server_request_submit(
				    &req[cpu_index]);
			} else {
				panic("no interrupt server for epoch drain");
			}
		}
	}
#else
	epoch->e_drain_count = 1;
	er = EPOCH_GET_RECORD(0, epoch);
	epoch_call(epoch, &er->er_drain_ctx, epoch_drain_cb);
#endif

	while (epoch->e_drain_count != 0) {
		msleep(epoch, &epoch->e_drain_mtx, PZERO, "EDRAIN", 0);
	}

	mtx_unlock(&epoch->e_drain_mtx);
	sx_xunlock(&epoch->e_drain_sx);

#ifdef RTEMS_SMP
	for (cpu_index = 0; cpu_index <= cpu_max; ++cpu_index) {
		if (req[cpu_index].action.handler != NULL) {
			rtems_interrupt_server_request_destroy(&req[cpu_index]);
		}
	}
#endif
}
