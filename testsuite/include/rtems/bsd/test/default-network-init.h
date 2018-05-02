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

#include <sys/stat.h>
#include <sys/socket.h>

#include <net/if.h>

#include <assert.h>
#include <fcntl.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>

#include <machine/rtems-bsd-commands.h>

#include <rtems.h>
#include <rtems/printer.h>
#include <rtems/stackchk.h>
#include <rtems/bsd/bsd.h>
#include <rtems/bsd/modules.h>
#include <rtems/dhcpcd.h>

#if defined(DEFAULT_NETWORK_DHCPCD_ENABLE) && \
    !defined(DEFAULT_NETWORK_NO_STATIC_IFCONFIG)
#define DEFAULT_NETWORK_NO_STATIC_IFCONFIG
#endif

#ifndef DEFAULT_NETWORK_NO_STATIC_IFCONFIG
#include <rtems/bsd/test/network-config.h>
#endif

#ifdef DEFAULT_NETWORK_SHELL
#include <rtems/console.h>
#include <rtems/shell.h>
#endif

static void
default_network_set_self_prio(rtems_task_priority prio)
{
	rtems_status_code sc;

	sc = rtems_task_set_priority(RTEMS_SELF, prio, &prio);
	assert(sc == RTEMS_SUCCESSFUL);
}

#ifndef DEFAULT_NETWORK_NO_INTERFACE_0
static void
default_network_ifconfig_hwif0(char *ifname)
{
	int exit_code;
	char *ifcfg[] = {
		"ifconfig",
		ifname,
#ifdef DEFAULT_NETWORK_NO_STATIC_IFCONFIG
		"up",
#else
		"inet",
		NET_CFG_SELF_IP,
		"netmask",
		NET_CFG_NETMASK,
#endif
		NULL
	};

	exit_code = rtems_bsd_command_ifconfig(RTEMS_BSD_ARGC(ifcfg), ifcfg);
	assert(exit_code == EX_OK);
}

static void
default_network_route_hwif0(char *ifname)
{
#ifndef DEFAULT_NETWORK_NO_STATIC_IFCONFIG
	int exit_code;
	char *dflt_route[] = {
		"route",
		"add",
		"-host",
		NET_CFG_GATEWAY_IP,
		"-iface",
		ifname,
		NULL
	};
	char *dflt_route2[] = {
		"route",
		"add",
		"default",
		NET_CFG_GATEWAY_IP,
		NULL
	};

	exit_code = rtems_bsd_command_route(RTEMS_BSD_ARGC(dflt_route), dflt_route);
	assert(exit_code == EXIT_SUCCESS);

	exit_code = rtems_bsd_command_route(RTEMS_BSD_ARGC(dflt_route2), dflt_route2);
	assert(exit_code == EXIT_SUCCESS);
#endif
}
#endif

static void
default_network_dhcpcd(void)
{
#ifdef DEFAULT_NETWORK_DHCPCD_ENABLE
	static const char default_cfg[] = "clientid libbsd test client\n";
	rtems_status_code sc;
	int fd;
	int rv;
	ssize_t n;

	fd = open("/etc/dhcpcd.conf", O_CREAT | O_WRONLY,
	    S_IRWXU | S_IRWXG | S_IRWXO);
	assert(fd >= 0);

	n = write(fd, default_cfg, sizeof(default_cfg));
	assert(n == (ssize_t) sizeof(default_cfg));

#ifdef DEFAULT_NETWORK_DHCPCD_NO_DHCP_DISCOVERY
	static const char nodhcp_cfg[] = "nodhcp\nnodhcp6\n";

	n = write(fd, nodhcp_cfg, sizeof(nodhcp_cfg));
	assert(n == (ssize_t) sizeof(nodhcp_cfg));
#endif

	rv = close(fd);
	assert(rv == 0);

	sc = rtems_dhcpcd_start(NULL);
	assert(sc == RTEMS_SUCCESSFUL);
#endif
}

static void
default_network_on_exit(int exit_code, void *arg)
{
	rtems_printer printer;

	(void)arg;

	rtems_print_printer_printf(&printer);
	rtems_stack_checker_report_usage_with_plugin(&printer);

	if (exit_code == 0) {
		puts("*** END OF TEST " TEST_NAME " ***");
	}
}

static void
Init(rtems_task_argument arg)
{
	rtems_status_code sc;
#ifndef DEFAULT_NETWORK_NO_INTERFACE_0
#ifdef DEFAULT_NETWORK_NO_STATIC_IFCONFIG
	char ifnamebuf[IF_NAMESIZE];
#endif
	char *ifname;
#endif

	/*
	 * Default the syslog priority to 'debug' to aid developers.
	 */
	rtems_bsd_setlogpriority("debug");

	(void)arg;
	puts("*** " TEST_NAME " TEST ***");

	on_exit(default_network_on_exit, NULL);

#ifdef DEFAULT_EARLY_INITIALIZATION
	early_initialization();
#endif

	/* Let other tasks run to complete background work */
	default_network_set_self_prio(RTEMS_MAXIMUM_PRIORITY - 1U);

#ifdef DEFAULT_NETWORK_SHELL
	sc = rtems_shell_init(
		"SHLL",
		32 * 1024,
		1,
		CONSOLE_DEVICE_NAME,
		false,
		false,
		NULL
	);
	assert(sc == RTEMS_SUCCESSFUL);
#endif

	rtems_bsd_initialize();

#ifndef DEFAULT_NETWORK_NO_INTERFACE_0
#ifdef DEFAULT_NETWORK_NO_STATIC_IFCONFIG
	ifname = if_indextoname(1, &ifnamebuf[0]);
	assert(ifname != NULL);
#else
	ifname = NET_CFG_INTERFACE_0;
#endif
#endif

	/* Let the callout timer allocate its resources */
	sc = rtems_task_wake_after(2);
	assert(sc == RTEMS_SUCCESSFUL);

	rtems_bsd_ifconfig_lo0();
#ifndef DEFAULT_NETWORK_NO_INTERFACE_0
	default_network_ifconfig_hwif0(ifname);
	default_network_route_hwif0(ifname);
#endif
	default_network_dhcpcd();

	test_main();

	assert(0);
}

/*
 * Configure LibBSD.
 */

#if defined(LIBBSP_I386_PC386_BSP_H)
#define RTEMS_BSD_CONFIG_DOMAIN_PAGE_MBUFS_SIZE (64 * 1024 * 1024)
#elif defined(LIBBSP_POWERPC_QORIQ_BSP_H)
#define RTEMS_BSD_CONFIG_DOMAIN_PAGE_MBUFS_SIZE (32 * 1024 * 1024)
#endif

#define RTEMS_BSD_CONFIG_NET_PF_UNIX
#define RTEMS_BSD_CONFIG_NET_IP_MROUTE
#define RTEMS_BSD_CONFIG_NET_IP6_MROUTE
#define RTEMS_BSD_CONFIG_NET_IF_BRIDGE
#define RTEMS_BSD_CONFIG_NET_IF_LAGG
#define RTEMS_BSD_CONFIG_NET_IF_VLAN
#define RTEMS_BSD_CONFIG_BSP_CONFIG
#define RTEMS_BSD_CONFIG_INIT

#include <machine/rtems-bsd-config.h>

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_STUB_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_ZERO_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_LIBBLOCK

#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 32

#define CONFIGURE_MAXIMUM_USER_EXTENSIONS 1

#define CONFIGURE_UNLIMITED_ALLOCATION_SIZE 32
#define CONFIGURE_UNLIMITED_OBJECTS
#define CONFIGURE_UNIFIED_WORK_AREAS

#define CONFIGURE_STACK_CHECKER_ENABLED

#define CONFIGURE_BDBUF_BUFFER_MAX_SIZE (64 * 1024)
#define CONFIGURE_BDBUF_MAX_READ_AHEAD_BLOCKS 4
#define CONFIGURE_BDBUF_CACHE_MEMORY_SIZE (1 * 1024 * 1024)

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT_TASK_STACK_SIZE (32 * 1024)
#define CONFIGURE_INIT_TASK_INITIAL_MODES RTEMS_DEFAULT_MODES
#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_FLOATING_POINT

#define CONFIGURE_INIT

#include <rtems/confdefs.h>

#ifdef DEFAULT_NETWORK_SHELL

#define CONFIGURE_SHELL_COMMANDS_INIT

#include <bsp/irq-info.h>

#include <rtems/netcmds-config.h>

#ifdef RTEMS_BSD_MODULE_USER_SPACE_WLANSTATS
  #define SHELL_WLANSTATS_COMMAND &rtems_shell_WLANSTATS_Command,
#else
  #define SHELL_WLANSTATS_COMMAND
#endif

#ifdef RTEMS_BSD_MODULE_USR_SBIN_WPA_SUPPLICANT
  #define SHELL_WPA_SUPPLICANT_COMMAND &rtems_shell_WPA_SUPPLICANT_Command,
#else
  #define SHELL_WPA_SUPPLICANT_COMMAND
#endif

#define CONFIGURE_SHELL_USER_COMMANDS \
  SHELL_WLANSTATS_COMMAND \
  SHELL_WPA_SUPPLICANT_COMMAND \
  &bsp_interrupt_shell_command, \
  &rtems_shell_ARP_Command, \
  &rtems_shell_HOSTNAME_Command, \
  &rtems_shell_PING_Command, \
  &rtems_shell_ROUTE_Command, \
  &rtems_shell_NETSTAT_Command, \
  &rtems_shell_IFCONFIG_Command, \
  &rtems_shell_TCPDUMP_Command, \
  &rtems_shell_SYSCTL_Command, \
  &rtems_shell_VMSTAT_Command

#define CONFIGURE_SHELL_COMMAND_CPUINFO
#define CONFIGURE_SHELL_COMMAND_CPUUSE
#define CONFIGURE_SHELL_COMMAND_PERIODUSE
#define CONFIGURE_SHELL_COMMAND_STACKUSE
#define CONFIGURE_SHELL_COMMAND_PROFREPORT

#define CONFIGURE_SHELL_COMMAND_CP
#define CONFIGURE_SHELL_COMMAND_PWD
#define CONFIGURE_SHELL_COMMAND_LS
#define CONFIGURE_SHELL_COMMAND_LN
#define CONFIGURE_SHELL_COMMAND_LSOF
#define CONFIGURE_SHELL_COMMAND_CHDIR
#define CONFIGURE_SHELL_COMMAND_CD
#define CONFIGURE_SHELL_COMMAND_MKDIR
#define CONFIGURE_SHELL_COMMAND_RMDIR
#define CONFIGURE_SHELL_COMMAND_CAT
#define CONFIGURE_SHELL_COMMAND_MV
#define CONFIGURE_SHELL_COMMAND_RM
#define CONFIGURE_SHELL_COMMAND_MALLOC_INFO
#define CONFIGURE_SHELL_COMMAND_SHUTDOWN

#include <rtems/shellconfig.h>

#endif /* DEFAULT_NETWORK_SHELL */
