/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief TODO.
 */

/*
 * Copyright (c) 2009-2015 embedded brains GmbH.  All rights reserved.
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
#include <machine/rtems-bsd-thread.h>

#include <sys/param.h>
#include <sys/types.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/ktr.h>
#include <sys/sysctl.h>
#include <sys/lock.h>
#include <sys/mutex.h>
#include <sys/proc.h>
#include <sys/stat.h>
#include <sys/kbio.h>
#include <sys/resourcevar.h>
#include <sys/jail.h>
#include <uuid/uuid.h>

#include <limits.h>

#include <dev/kbd/kbdreg.h>

#include <net80211/ieee80211_freebsd.h>

#include <rtems/bsd/bsd.h>

SYSINIT_REFERENCE(configure1);
SYSINIT_REFERENCE(module);
SYSINIT_REFERENCE(kobj);
SYSINIT_REFERENCE(linker_kernel);
SYSINIT_MODULE_REFERENCE(rootbus);
SYSINIT_DRIVER_REFERENCE(nexus, root);

RTEMS_BSD_DEFINE_SET(kbddriver_set, const keyboard_driver_t *);
RTEMS_BSD_DEFINE_SET(modmetadata_set, struct mod_metadata *);
RTEMS_BSD_DEFINE_SET(ieee80211_ioctl_getset, ieee80211_ioctl_getfunc *);
RTEMS_BSD_DEFINE_SET(ieee80211_ioctl_setset, ieee80211_ioctl_setfunc *);
typedef void (*ratectl_modevent)(int);
RTEMS_BSD_DEFINE_SET(ratectl_set, ratectl_modevent);
typedef void (*scanner_modevent)(int);
RTEMS_BSD_DEFINE_SET(scanner_set, scanner_modevent);
typedef void (*crypto_modevent)(int);
RTEMS_BSD_DEFINE_SET(crypto_set, crypto_modevent);
RTEMS_BSD_DEFINE_SET(sysctl_set, struct sysctl_oid *);

RTEMS_BSD_DEFINE_RWSET(sysinit_set, struct sysinit *);

/* In FreeBSD this is a local function */
void mi_startup(void);

int hz;
int tick;
volatile int ticks;
sbintime_t tick_sbt;
sbintime_t rtems_bsd_sbt_per_watchdog_tick;
struct bintime bt_timethreshold;
struct bintime bt_tickthreshold;
sbintime_t sbt_timethreshold;
sbintime_t sbt_tickthreshold;
struct bintime tc_tick_bt;
sbintime_t tc_tick_sbt;
int maxproc;
int tc_precexp;

static SYSCTL_NODE(_kern, OID_AUTO, smp, CTLFLAG_RD|CTLFLAG_CAPRD, NULL,
    "Kernel SMP");

static int maxid_maxcpus;

SYSCTL_INT(_kern_smp, OID_AUTO, maxid, CTLFLAG_RD|CTLFLAG_CAPRD,
    &maxid_maxcpus, 0, "Max CPU ID.");

SYSCTL_INT(_kern_smp, OID_AUTO, maxcpus, CTLFLAG_RD|CTLFLAG_CAPRD,
    &maxid_maxcpus, 0, "Max number of CPUs that the system was compiled for.");

/*
 * Create a single process. RTEMS is a single address, single process OS.
 */
static void
proc0_init(void* dummy)
{
	struct proc *p = &proc0;
	struct ucred *newcred;
	struct uidinfo tmpuinfo;
	uuid_t uuid;
	uihashinit();
	/* Create the file descriptor table. */
	newcred = crget();
	newcred->cr_uid = 0;
	newcred->cr_ruid = 0;
	newcred->cr_ngroups = 1;	/* group 0 */
	newcred->cr_groups[0] = 0;
	newcred->cr_rgid = 0;
	tmpuinfo.ui_uid = 1;
	curthread->td_ucred = newcred;
	newcred->cr_uidinfo = newcred->cr_ruidinfo = &tmpuinfo;
	newcred->cr_uidinfo = uifind(0);
	newcred->cr_ruidinfo = uifind(0);
	p->p_ucred = newcred;
	p->p_pid = getpid();
	p->p_fd = NULL;
	p->p_fdtol = NULL;
	uuid_generate(uuid);
	uuid_unparse(uuid, prison0.pr_hostuuid);
}
SYSINIT(p0init, SI_SUB_INTRINSIC, SI_ORDER_FIRST, proc0_init, NULL);

rtems_status_code
rtems_bsd_initialize(void)
{
	static const char name[] = "TIME";
	rtems_status_code sc;
	int tps;

	/*
	 * Limit the libbsd ticks per second to 100Hz.  This helps to reduce
	 * the processor load on low end targets which use 1000Hz for the RTEMS
	 * clock tick.
	 */
	tps = (int)rtems_clock_get_ticks_per_second();
	hz = MIN(100, tps);

	tick = 1000000 / hz;
	tick_sbt = SBT_1S / hz;
	rtems_bsd_sbt_per_watchdog_tick = SBT_1S / tps;
	FREQ2BT(hz, &tc_tick_bt);
	tc_tick_sbt = bttosbt(tc_tick_bt);
	tc_precexp = 31;
	bt_timethreshold.sec = INT_MAX;
	bt_timethreshold.frac = ~(uint64_t)0;
	bt_tickthreshold = bt_timethreshold;
	sbt_timethreshold = bttosbt(bt_timethreshold);
	sbt_tickthreshold = bttosbt(bt_tickthreshold);
	maxid_maxcpus = (int) rtems_scheduler_get_processor_maximum();

	maxproc = 16;

	mkdir("/etc", S_IRWXU | S_IRWXG | S_IRWXO);

	sc =  rtems_timer_initiate_server(
		rtems_bsd_get_task_priority(name),
		rtems_bsd_get_task_stack_size(name),
		RTEMS_DEFAULT_ATTRIBUTES
	);
	if (sc != RTEMS_SUCCESSFUL) {
		return RTEMS_UNSATISFIED;
	}

	mutex_init();
	mi_startup();

#ifdef KTR
	ktr_verbose = 10;
	ktr_mask = KTR_ALL;
	ktr_mask = KTR_GEN | KTR_LOCK | KTR_VFS | KTR_VOP | KTR_BUF | KTR_MALLOC | KTR_SYSC | KTR_RUNQ;
#endif

	return RTEMS_SUCCESSFUL;
}
