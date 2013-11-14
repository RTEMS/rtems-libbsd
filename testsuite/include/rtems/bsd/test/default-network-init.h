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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>

#include <machine/rtems-bsd-commands.h>

#include <rtems.h>
#include <rtems/stackchk.h>
#include <rtems/bsd/test/network-config.h>
#include <rtems/bsd/bsd.h>

#define DEFAULT_NETWORK_ARGC(x) (RTEMS_ARRAY_SIZE(x) - 1)

static void
default_network_set_self_prio(rtems_task_priority prio)
{
	rtems_status_code sc;

	sc = rtems_task_set_priority(RTEMS_SELF, prio, &prio);
	assert(sc == RTEMS_SUCCESSFUL);
}

static void
default_network_ifconfig_lo0(void)
{
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
	char *lo0_inet6[] = {
		"ifconfig",
		"lo0",
		"inet6",
		"::1",
		"prefixlen",
		"128",
		NULL
	};

	exit_code = rtems_bsd_command_ifconfig(DEFAULT_NETWORK_ARGC(lo0), lo0);
	assert(exit_code == EX_OK);

	exit_code = rtems_bsd_command_ifconfig(DEFAULT_NETWORK_ARGC(lo0_inet6), lo0_inet6);
	assert(exit_code == EX_OK);
}

static void
default_network_ifconfig_interface_0(void)
{
	int exit_code;
	char *iface0[] = {
		"ifconfig",
		NET_CFG_INTERFACE_0,
		"inet",
		NET_CFG_SELF_IP,
		"netmask",
		NET_CFG_NETMASK,
		NULL
	};

	exit_code = rtems_bsd_command_ifconfig(DEFAULT_NETWORK_ARGC(iface0), iface0);
	assert(exit_code == EX_OK);
}

static void
default_network_route(void)
{
	int exit_code;
	char *dflt_route[] = {
		"route",
		"add",
		"-host",
		NET_CFG_GATEWAY_IP,
		"-iface",
		NET_CFG_INTERFACE_0,
		NULL
	};
	char *dflt_route2[] = {
		"route",
		"add",
		"default",
		NET_CFG_GATEWAY_IP,
		NULL
	};

	exit_code = rtems_bsd_command_route(DEFAULT_NETWORK_ARGC(dflt_route), dflt_route);
	assert(exit_code == EXIT_SUCCESS);

	exit_code = rtems_bsd_command_route(DEFAULT_NETWORK_ARGC(dflt_route2), dflt_route2);
	assert(exit_code == EXIT_SUCCESS);
}

static void
default_network_on_exit(int exit_code, void *arg)
{
	rtems_stack_checker_report_usage_with_plugin(NULL,
	    rtems_printf_plugin);

	if (exit_code == 0) {
		puts("*** END OF TEST " TEST_NAME " ***");
	}
}

static void
Init(rtems_task_argument arg)
{
	rtems_status_code sc;

	puts("*** " TEST_NAME " TEST ***");

	on_exit(default_network_on_exit, NULL);

	/* Let other tasks run to complete background work */
	default_network_set_self_prio(RTEMS_MAXIMUM_PRIORITY - 1);

	rtems_bsd_initialize_with_interrupt_server();

	/* Let the callout timer allocate its resources */
	sc = rtems_task_wake_after(2);
	assert(sc == RTEMS_SUCCESSFUL);

	default_network_ifconfig_lo0();
	default_network_ifconfig_interface_0();
	default_network_route();

	test_main();

	assert(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM

#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 32

#define CONFIGURE_MAXIMUM_USER_EXTENSIONS 1

#define CONFIGURE_UNLIMITED_ALLOCATION_SIZE 32
#define CONFIGURE_UNLIMITED_OBJECTS
#define CONFIGURE_UNIFIED_WORK_AREAS

#define CONFIGURE_STACK_CHECKER_ENABLED

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT_TASK_STACK_SIZE (32 * 1024)
#define CONFIGURE_INIT_TASK_INITIAL_MODES RTEMS_DEFAULT_MODES

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
