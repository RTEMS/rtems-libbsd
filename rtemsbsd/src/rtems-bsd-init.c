/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief TODO.
 */

/*
 * Copyright (c) 2009, 2010 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
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

#include <freebsd/machine/rtems-bsd-config.h>

#include <freebsd/sys/param.h>
#include <freebsd/sys/types.h>
#include <freebsd/sys/systm.h>
#include <freebsd/sys/kernel.h>
#include <freebsd/sys/lock.h>
#include <freebsd/sys/mutex.h>
#include <freebsd/sys/proc.h>

#include <freebsd/bsd.h>

/* In FreeBSD this is a local function */
void mi_startup(void);

int hz;
int tick;
int maxusers;     /* base tunable */


static struct pcpu FIXME_pcpu[MAXCPU];

/*
 * Initialize per cpu data structures.  Based off
 * of the freeBSD mips method mips_pcpu0_init()
 */
static void
pcpu0_init()
{
        /* Initialize pcpu info of cpu-zero */
        pcpu_init((char *)&FIXME_pcpu[0], 0, sizeof(struct pcpu));
}
rtems_id  rtems_init_task_id;

rtems_status_code
rtems_bsd_initialize(void)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;

        rtems_init_task_id = rtems_task_self();
	hz = (int) rtems_clock_get_ticks_per_second();
	tick = 1000000 / hz;
	maxusers = 1;

	sc =  rtems_timer_initiate_server(
		BSD_TASK_PRIORITY_TIMER,
		BSD_MINIMUM_TASK_STACK_SIZE,
		RTEMS_DEFAULT_ATTRIBUTES
	);
	if (sc != RTEMS_SUCCESSFUL) {
		return RTEMS_UNSATISFIED;
	}

	mutex_init();
	pcpu0_init();
	mi_startup();

	return RTEMS_SUCCESSFUL;
}
