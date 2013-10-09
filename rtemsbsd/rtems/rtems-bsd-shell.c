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

#include <machine/rtems-bsd-config.h>
#include <machine/rtems-bsd-thread.h>

#include <rtems/bsd/sys/param.h>
#include <rtems/bsd/sys/types.h>
#include <sys/systm.h>
#include <rtems/bsd/sys/lock.h>
#include <sys/mutex.h>
#include <sys/callout.h>
#include <sys/condvar.h>
#include <sys/proc.h>

#include <rtems/bsd/bsd.h>
#include <rtems/shell.h>

static void
rtems_bsd_dump_mtx(void)
{
	rtems_chain_control *chain = &rtems_bsd_mtx_chain;
	rtems_chain_node *node = rtems_chain_first(chain);

	printf("mtx dump:\n");

	while (!rtems_chain_is_tail(chain, node)) {
		struct lock_object *lo = (struct lock_object *) node;

		printf("\t%s: 0x%08x\n", lo->lo_name, lo->lo_id);

		node = rtems_chain_next(node);
	}
}

static void
rtems_bsd_dump_sx(void)
{
	rtems_chain_control *chain = &rtems_bsd_sx_chain;
	rtems_chain_node *node = rtems_chain_first(chain);

	printf("sx dump:\n");

	while (!rtems_chain_is_tail(chain, node)) {
		struct lock_object *lo = (struct lock_object *) node;

		printf("\t%s: 0x%08x\n", lo->lo_name, lo->lo_id);

		node = rtems_chain_next(node);
	}
}

static void
rtems_bsd_dump_condvar(void)
{
	rtems_chain_control *chain = &rtems_bsd_condvar_chain;
	rtems_chain_node *node = rtems_chain_first(chain);

	printf("condvar dump:\n");

	while (!rtems_chain_is_tail(chain, node)) {
		struct cv *cv = (struct cv *) node;

		printf("\t%s: 0x%08x\n", cv->cv_description, cv->cv_id);

		node = rtems_chain_next(node);
	}
}

static void
rtems_bsd_dump_thread(void)
{
	const rtems_chain_control *chain = &rtems_bsd_thread_chain;
	const rtems_chain_node *node = rtems_chain_immutable_first(chain);

	printf("thread dump:\n");

	while (!rtems_chain_is_tail(chain, node)) {
		const struct thread *td = (const struct thread *) node;

		printf("\t%s: 0x%08x\n", td->td_name, rtems_bsd_get_task_id(td));

		node = rtems_chain_immutable_next(node);
	}
}

static const char rtems_bsd_usage [] =
	"bsd {all|mtx|sx|condvar|thread|callout}";

#define CMP(s) all || strcasecmp(argv [1], s) == 0

static int
rtems_bsd_info(int argc, char **argv)
{
	bool usage = true;

	if (argc == 2) {
		bool all = false;

		if (CMP("all")) {
			all = true;
		}

		if (CMP("mtx")) {
			rtems_bsd_dump_mtx();
			usage = false;
		}
		if (CMP("sx")) {
			rtems_bsd_dump_sx();
			usage = false;
		}
		if (CMP("condvar")) {
			rtems_bsd_dump_condvar();
			usage = false;
		}
		if (CMP("thread")) {
			rtems_bsd_dump_thread();
			usage = false;
		}
	}

	if (usage) {
		puts(rtems_bsd_usage);
	}

	return 0;
}

static rtems_shell_cmd_t rtems_bsd_info_command = {
	.name = "bsd",
	.usage = rtems_bsd_usage,
	.topic = "bsp",
	.command = rtems_bsd_info,
	.alias = NULL,
	.next = NULL
};

void
rtems_bsd_shell_initialize(void)
{
	rtems_shell_add_cmd_struct(&rtems_bsd_info_command);
}
