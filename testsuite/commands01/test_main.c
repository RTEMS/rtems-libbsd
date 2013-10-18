/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
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

#include <rtems/bsd/sys/param.h>

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>

#include <machine/rtems-bsd-commands.h>

#include <rtems/libcsupport.h>
#include <rtems/stackchk.h>

#define TEST_NAME "LIBBSD COMMANDS 1"

#define ARGC(x) (nitems(x) - 1)

static void
set_self_prio(rtems_task_priority prio)
{
	rtems_status_code sc;

	sc = rtems_task_set_priority(RTEMS_SELF, prio, &prio);
	assert(sc == RTEMS_SUCCESSFUL);
}

static void
test_route_without_if(void)
{
	rtems_resource_snapshot snapshot;
	int exit_code;
	char *flush[] = {
		"route",
		"flush",
		NULL
	};
	char *dflt_route[] = {
		"route",
		"add",
		"default",
		"192.168.96.1",
		NULL
	};
	char *invalid[] = {
		"route",
		"blub",
		NULL
	};

	exit_code = rtems_bsd_command_route(ARGC(flush), flush);
	assert(exit_code == EX_OK);

	rtems_resource_snapshot_take(&snapshot);

	exit_code = rtems_bsd_command_route(ARGC(flush), flush);
	assert(exit_code == EX_OK);

	assert(rtems_resource_snapshot_check(&snapshot));

	exit_code = rtems_bsd_command_route(ARGC(dflt_route), dflt_route);
	assert(exit_code == EXIT_FAILURE);

	rtems_resource_snapshot_take(&snapshot);

	exit_code = rtems_bsd_command_route(ARGC(dflt_route), dflt_route);
	assert(exit_code == EXIT_FAILURE);

	assert(rtems_resource_snapshot_check(&snapshot));

	exit_code = rtems_bsd_command_route(ARGC(invalid), invalid);
	assert(exit_code == EX_USAGE);

	assert(rtems_resource_snapshot_check(&snapshot));
}

static void
test_ifconfig_lo0(void)
{
	rtems_resource_snapshot snapshot;
	int exit_code;
	char *lo0[] = {
		"ifconfig",
		"lo0",
		"inet",
		"127.0.0.1",
		"netmask",
		"255.255.255.0",
		NULL
	};
	char *status[] = {
		"ifconfig",
		"lo0",
		"inet",
		NULL
	};

	exit_code = rtems_bsd_command_ifconfig(ARGC(lo0), lo0);
	assert(exit_code == EX_OK);

	rtems_resource_snapshot_take(&snapshot);

	exit_code = rtems_bsd_command_ifconfig(ARGC(status), status);
	assert(exit_code == EX_OK);

	assert(rtems_resource_snapshot_check(&snapshot));
}

static void
test_route_with_lo0(void)
{
	int exit_code;
	char *dflt_route[] = {
		"route",
		"add",
		"default",
		"127.0.0.1",
		NULL
	};

	exit_code = rtems_bsd_command_route(ARGC(dflt_route), dflt_route);
	assert(exit_code == EXIT_SUCCESS);
}

static void
test_ping(void)
{
	rtems_resource_snapshot snapshot;
	int exit_code;
	char *ping[] = {
		"ping",
		"-c",
		"3",
		"127.0.0.1",
		NULL
	};

	exit_code = rtems_bsd_command_ping(ARGC(ping), ping);
	assert(exit_code == EXIT_SUCCESS);

	rtems_resource_snapshot_take(&snapshot);

	exit_code = rtems_bsd_command_ping(ARGC(ping), ping);
	assert(exit_code == EXIT_SUCCESS);

	assert(rtems_resource_snapshot_check(&snapshot));
}

static void
test_netstat(void)
{
	rtems_resource_snapshot snapshot;
	int exit_code;
	char *netstat[] = {
		"netstat",
		NULL
	};
	char *netstat_s[] = {
		"netstat",
		"-s",
		NULL
	};
	char *netstat_r[] = {
		"netstat",
		"-r",
		NULL
	};

	exit_code = rtems_bsd_command_netstat(ARGC(netstat), netstat);
	assert(exit_code == EXIT_SUCCESS);

	rtems_resource_snapshot_take(&snapshot);

	exit_code = rtems_bsd_command_netstat(ARGC(netstat_s), netstat_s);
	assert(exit_code == EXIT_SUCCESS);

	assert(rtems_resource_snapshot_check(&snapshot));

	exit_code = rtems_bsd_command_netstat(ARGC(netstat_r), netstat_r);
	assert(exit_code == EXIT_SUCCESS);

	rtems_resource_snapshot_take(&snapshot);

	exit_code = rtems_bsd_command_netstat(ARGC(netstat_r), netstat_r);
	assert(exit_code == EXIT_SUCCESS);

	assert(rtems_resource_snapshot_check(&snapshot));
}

static void
test_main(void)
{
	/*
	 * Let other tasks run to complete background work that frees allocated
	 * resources.
	 */
	set_self_prio(RTEMS_MAXIMUM_PRIORITY - 1);

	test_route_without_if();
	test_ifconfig_lo0();
	test_route_with_lo0();
	test_ping();
	test_netstat();

	rtems_stack_checker_report_usage_with_plugin(NULL,
	    rtems_printf_plugin);

	puts("*** END OF " TEST_NAME " TEST ***");
	exit(0);
}

#include <rtems/bsd/test/default-init.h>
