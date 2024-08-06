/**
 * @file
 *
 * The following commands are tested: route, ifconfig, ping, ping6 (if defined),
 * netstat and wlanstats (if defined).
 */

/*
 * Copyright (c) 2013-2014 embedded brains GmbH.  All rights reserved.
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

#include <sys/param.h>

#include <assert.h>
#include <ck_epoch.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>

#include <machine/rtems-bsd-commands.h>

#include <rtems/libcsupport.h>
#include <rtems/bsd/modules.h>

#define TEST_NAME "LIBBSD COMMANDS 1"

#define ARGC(x) RTEMS_BSD_ARGC(x)

static void
epoch_cleanup(void)
{
	rtems_status_code sc;

	sc = rtems_task_wake_after(CK_EPOCH_LENGTH);
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

	epoch_cleanup();
	rtems_resource_snapshot_take(&snapshot);

	exit_code = rtems_bsd_command_route(ARGC(flush), flush);
	assert(exit_code == EX_OK);

	epoch_cleanup();
	assert(rtems_resource_snapshot_check(&snapshot));

	exit_code = rtems_bsd_command_route(ARGC(dflt_route), dflt_route);
	assert(exit_code == EXIT_FAILURE);

	epoch_cleanup();
	rtems_resource_snapshot_take(&snapshot);

	exit_code = rtems_bsd_command_route(ARGC(dflt_route), dflt_route);
	assert(exit_code == EXIT_FAILURE);

	epoch_cleanup();
	assert(rtems_resource_snapshot_check(&snapshot));

	exit_code = rtems_bsd_command_route(ARGC(invalid), invalid);
	assert(exit_code == EX_USAGE);

	epoch_cleanup();
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
#ifdef RTEMS_BSD_MODULE_NETINET6
	char *lo0_inet6[] = {
		"ifconfig",
		"lo0",
		"inet6",
		"::1",
		"prefixlen",
		"128",
		NULL
	};
#endif /* RTEMS_BSD_MODULE_NETINET6 */
	char *status[] = {
		"ifconfig",
		"lo0",
		"inet",
		NULL
	};
#ifdef RTEMS_BSD_MODULE_NETINET6
	char *status_inet6[] = {
		"ifconfig",
		"lo0",
		"inet6",
		NULL
	};
#endif /* RTEMS_BSD_MODULE_NETINET6 */

	exit_code = rtems_bsd_command_ifconfig(ARGC(lo0), lo0);
	assert(exit_code == EX_OK);

#ifdef RTEMS_BSD_MODULE_NETINET6
	exit_code = rtems_bsd_command_ifconfig(ARGC(lo0_inet6), lo0_inet6);
	assert(exit_code == EX_OK);
#endif /* RTEMS_BSD_MODULE_NETINET6 */

	epoch_cleanup();
	rtems_resource_snapshot_take(&snapshot);

	exit_code = rtems_bsd_command_ifconfig(ARGC(status), status);
	assert(exit_code == EX_OK);

#ifdef RTEMS_BSD_MODULE_NETINET6
	exit_code = rtems_bsd_command_ifconfig(ARGC(status_inet6), status_inet6);
	assert(exit_code == EX_OK);
#endif /* RTEMS_BSD_MODULE_NETINET6 */

	epoch_cleanup();
	assert(rtems_resource_snapshot_check(&snapshot));

#ifdef RTEMS_BSD_MODULE_NETINET6
	exit_code = rtems_bsd_command_ifconfig(ARGC(status_inet6), status_inet6);
	assert(exit_code == EX_OK);
#endif /* RTEMS_BSD_MODULE_NETINET6 */

	epoch_cleanup();
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

	epoch_cleanup();
	rtems_resource_snapshot_take(&snapshot);

	exit_code = rtems_bsd_command_ping(ARGC(ping), ping);
	assert(exit_code == EXIT_SUCCESS);

	epoch_cleanup();
	assert(rtems_resource_snapshot_check(&snapshot));
}

static void
test_ping6(void)
{
#ifdef RTEMS_BSD_MODULE_NETINET6
	rtems_resource_snapshot snapshot;
	int exit_code;
	char *ping6[] = {
		"ping",
		"-c6",
		"1",
		"::1",
		NULL
	};

	exit_code = rtems_bsd_command_ping(ARGC(ping6), ping6);
	assert(exit_code == EXIT_SUCCESS);

	epoch_cleanup();
	rtems_resource_snapshot_take(&snapshot);

	exit_code = rtems_bsd_command_ping(ARGC(ping6), ping6);
	assert(exit_code == EXIT_SUCCESS);

	epoch_cleanup();
	assert(rtems_resource_snapshot_check(&snapshot));
#endif
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

	epoch_cleanup();
	rtems_resource_snapshot_take(&snapshot);

	exit_code = rtems_bsd_command_netstat(ARGC(netstat_s), netstat_s);
	assert(exit_code == EXIT_SUCCESS);

	epoch_cleanup();
	assert(rtems_resource_snapshot_check(&snapshot));

	exit_code = rtems_bsd_command_netstat(ARGC(netstat_r), netstat_r);
	assert(exit_code == EXIT_SUCCESS);

	epoch_cleanup();
	rtems_resource_snapshot_take(&snapshot);

	exit_code = rtems_bsd_command_netstat(ARGC(netstat_r), netstat_r);
	assert(exit_code == EXIT_SUCCESS);

	epoch_cleanup();
	assert(rtems_resource_snapshot_check(&snapshot));
}

static void
test_wlanstats(void)
{
#ifdef RTEMS_BSD_MODULE_USER_SPACE_WLANSTATS
	rtems_resource_snapshot snapshot;
	char *wlanstats[] = {
		"wlanstats",
		NULL
	};

	/* Without a WLAN device, only the basic call can be tested. */
	rtems_bsd_command_wlanstats(ARGC(wlanstats), wlanstats);
	epoch_cleanup();
	rtems_resource_snapshot_take(&snapshot);
	rtems_bsd_command_wlanstats(ARGC(wlanstats), wlanstats);
	epoch_cleanup();
	assert(rtems_resource_snapshot_check(&snapshot));
#endif /* RTEMS_BSD_MODULE_USER_SPACE_WLANSTATS */
}

static void
test_main(void)
{
	test_route_without_if();
	test_ifconfig_lo0();
	test_route_with_lo0();
	test_ping();
	test_ping6();
	test_netstat();
	test_wlanstats();

	exit(0);
}

#include <rtems/bsd/test/default-init.h>
