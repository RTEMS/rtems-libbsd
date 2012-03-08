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
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <freebsd/machine/rtems-bsd-config.h>

#include <freebsd/sys/param.h>
#include <freebsd/sys/types.h>
#include <freebsd/sys/systm.h>
#include <freebsd/sys/lock.h>
#include <freebsd/sys/mutex.h>
#include <freebsd/sys/callout.h>
#include <freebsd/sys/condvar.h>
#include <freebsd/sys/proc.h>

#include <freebsd/bsd.h>
#include <rtems/shell.h>

static void
rtems_bsd_dump_callout(void)
{
	rtems_chain_control *chain = &rtems_bsd_callout_chain;
	rtems_chain_node *node = rtems_chain_first(chain);

	printf("callout dump:\n");

	while (!rtems_chain_is_tail(chain, node)) {
		struct callout *c = (struct callout *) node;

		printf("\t%08x\n", c->c_id);

		node = rtems_chain_next(node);
	}
}

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
	rtems_chain_control *chain = &rtems_bsd_thread_chain;
	rtems_chain_node *node = rtems_chain_first(chain);

	printf("thread dump:\n");

	while (!rtems_chain_is_tail(chain, node)) {
		struct thread *td = (struct thread *) node;

		printf("\t%s: 0x%08x\n", td->td_name, td->td_id);

		node = rtems_chain_next(node);
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
		if (CMP("callout")) {
			rtems_bsd_dump_callout();
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
