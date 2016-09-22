/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief TODO.
 */

/*
 * Copyright (c) 2009-2014 embedded brains GmbH.  All rights reserved.
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
#include <machine/rtems-bsd-support.h>

#include <rtems/bsd/sys/param.h>
#include <sys/types.h>
#include <sys/systm.h>
#include <sys/proc.h>

#include <inttypes.h>

#include <rtems/netcmds-config.h>
#include <rtems/score/threadimpl.h>

static void
rtems_bsd_dump_thread(Thread_Control *thread)
{
	const struct thread *td = rtems_bsd_get_thread(thread);

	if (td != NULL) {
		char buf[5];
		const char *name = td->td_name;

		if (name == NULL || name[0] == '\0') {
			rtems_object_get_name(thread->Object.id, sizeof(buf), &buf[0]);
			name = &buf[0];
		}

		fprintf(
			stdout,
			" 0x%08" PRIx32 " | %8" PRIu32 " | %s\n",
			thread->Object.id,
			_Thread_Get_priority(thread),
			name
		);
	}
}

static void
rtems_bsd_dump_threads(void)
{
	fprintf(
		stdout,
		"-------------------------------------------------------------------------------\n"
		"                                  BSD THREADS\n"
		"------------+----------+-------------------------------------------------------\n"
		" ID         | PRIORITY | NAME\n"
		"------------+----------+-------------------------------------------------------\n"
	);

	rtems_iterate_over_all_threads(rtems_bsd_dump_thread);

	fprintf(
		stdout,
		"------------+----------+-------------------------------------------------------\n"
	);
}

static const char rtems_bsd_usage[] = "bsd";

#define CMP(s) all || strcasecmp(argv [1], s) == 0

static int
rtems_bsd_info(int argc, char **argv)
{
	rtems_bsd_dump_threads();

	return 0;
}

rtems_shell_cmd_t rtems_shell_BSD_Command = {
	.name = "bsd",
	.usage = &rtems_bsd_usage[0],
	.topic = "bsp",
	.command = rtems_bsd_info,
	.alias = NULL,
	.next = NULL
};
