/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief TODO.
 */

/*
 * Copyright (c) 2009-2013 embedded brains GmbH.  All rights reserved.
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

#include <rtems/bsd/sys/param.h>
#include <rtems/bsd/sys/types.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <rtems/bsd/sys/lock.h>
#include <sys/mutex.h>
#include <sys/proc.h>
#include <sys/stat.h>

#include <rtems/bsd/bsd.h>

SYSINIT_REFERENCE(configure1);
SYSINIT_REFERENCE(module);
SYSINIT_REFERENCE(kobj);
SYSINIT_REFERENCE(linker_kernel);
SYSINIT_MODULE_REFERENCE(rootbus);
SYSINIT_DRIVER_REFERENCE(nexus, root);

RTEMS_BSD_DEFINE_SET(modmetadata_set, struct mod_metadata);
RTEMS_BSD_DEFINE_SET(sysctl_set, struct sysctl_oid);

RTEMS_BSD_DEFINE_RWSET(sysinit_set, struct sysinit);

/* In FreeBSD this is a local function */
void mi_startup(void);

int hz;
int tick;
int maxusers;     /* base tunable */

struct bintime boottimebin;
struct timeval boottime;

rtems_status_code
rtems_bsd_initialize(void)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;

	hz = (int) rtems_clock_get_ticks_per_second();
	tick = 1000000 / hz;
	maxusers = 1;

	gettimeofday(&boottime, NULL);
	timeval2bintime(&boottime, &boottimebin);

	mkdir("/etc", S_IRWXU | S_IRWXG | S_IRWXO);

	sc =  rtems_timer_initiate_server(
		BSD_TASK_PRIORITY_TIMER,
		BSD_MINIMUM_TASK_STACK_SIZE,
		RTEMS_DEFAULT_ATTRIBUTES
	);
	if (sc != RTEMS_SUCCESSFUL) {
		return RTEMS_UNSATISFIED;
	}

	mutex_init();
	mi_startup();

	return RTEMS_SUCCESSFUL;
}
