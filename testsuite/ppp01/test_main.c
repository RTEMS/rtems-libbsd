/*
 * Copyright (c) 2014, 2018 embedded brains GmbH.  All rights reserved.
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

/*
 * To test PPP connect the RTEMS target with your host.  Start PPP on the host
 * with something like this:
 *
 * pppd nodetach noauth 192.168.100.11:192.168.100.70 proxyarp 115200 dump \
 *   local nocrtscts debug mtu 296 mru 296 nolock ms-dns 192.168.96.1 novj \
 *   /dev/ttyS0
 *
 * Make sure IP forwarding is enabled and check the firewall settings if you
 * want to access the internet.
 */

#include <assert.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <stdio.h>
#include <sysexits.h>

#include <machine/rtems-bsd-commands.h>

#include <rtems.h>
#include <rtems/bsd/bsd.h>
#include <rtems/telnetd.h>
#include <rtems/ftpd.h>
#include <rtems/rtemspppd.h>

#define TEST_NAME "LIBBSD PPP 1"
#define TEST_STATE_USER_INPUT 1

#ifndef RTEMS_SMP

static void
set_pppd_options(void)
{
	int rv;

	rv = rtems_pppd_set_option("/dev/ttyS1", NULL);
	assert(rv == 1);

	rv = rtems_pppd_set_option("115200", NULL);
	assert(rv == 1);

	rv = rtems_pppd_set_option("crtscts", NULL);
	assert(rv == 1);

	rv = rtems_pppd_set_option("debug", NULL);
	assert(rv == 1);

	rv = rtems_pppd_set_option("defaultroute", NULL);
	assert(rv == 1);

	rv = rtems_pppd_set_option("local", NULL);
	assert(rv == 1);

	rv = rtems_pppd_set_option("noauth", NULL);
	assert(rv == 1);

	rv = rtems_pppd_set_option("noipdefault", NULL);
	assert(rv == 1);

	rv = rtems_pppd_set_option("usepeerdns", NULL);
	assert(rv == 1);

	rv = rtems_pppd_set_option("password", "wurst");
	assert(rv == 1);

	rv = rtems_pppd_set_option("user", "hans");
	assert(rv == 1);

	rv = rtems_pppd_set_option("mru", "296");
	assert(rv == 1);

	rv = rtems_pppd_set_option("mtu", "296");
	assert(rv == 1);

	rv = rtems_pppd_set_option("lcp-echo-failure", "5");
	assert(rv == 1);

	rv = rtems_pppd_set_option("lcp-echo-interval", "5");
	assert(rv == 1);
}

static void
linkup_hook(void)
{
	printf("linkup hook\n");
}

static void
linkdown_hook(void)
{
	printf("linkdown hook\n");
}

static void
ipup_hook(void)
{
	static bool first = true;

	printf("ipup hook\n");

	if (first) {
		int rv;
		rtems_status_code sc;

		first = false;

		sc = rtems_telnetd_initialize();
		assert(sc == RTEMS_SUCCESSFUL);

		rv = rtems_initialize_ftpd();
		assert(rv == 0);
	}
}

static void
ipdown_hook(void)
{
	printf("ipdown hook\n");
}

static void
exit_hook(void)
{
	printf("exit hook\n");
}

static void
error_hook(void)
{
	printf("error hook\n");
}

static void
set_pppd_hooks(void)
{
	int rv;

	rv = rtems_pppd_set_hook(RTEMS_PPPD_LINKUP_HOOK, linkup_hook);
	assert(rv == 0);

	rv = rtems_pppd_set_hook(RTEMS_PPPD_LINKDOWN_HOOK, linkdown_hook);
	assert(rv == 0);

	rv = rtems_pppd_set_hook(RTEMS_PPPD_IPUP_HOOK, ipup_hook);
	assert(rv == 0);

	rv = rtems_pppd_set_hook(RTEMS_PPPD_IPDOWN_HOOK, ipdown_hook);
	assert(rv == 0);

	rv = rtems_pppd_set_hook(RTEMS_PPPD_EXIT_HOOK, exit_hook);
	assert(rv == 0);

	rv = rtems_pppd_set_hook(RTEMS_PPPD_ERROR_HOOK, error_hook);
	assert(rv == 0);
}

static void
ifconfig_ppp0(void)
{
	int exit_code;
	char *ifcfg[] = {
		"ifconfig",
		"ppp0",
		"up",
		NULL
	};

	exit_code = rtems_bsd_command_ifconfig(RTEMS_BSD_ARGC(ifcfg), ifcfg);
	assert(exit_code == EX_OK);
}

static void
telnet_shell(char *name, void *arg)
{
	rtems_shell_env_t env;

	memset(&env, 0, sizeof(env));

	env.devname = name;
	env.taskname = "TLNT";
	env.login_check = NULL;
	env.forever = false;

	rtems_shell_main_loop(&env);
}

rtems_telnetd_config_table rtems_telnetd_config = {
	.command = telnet_shell,
	.arg = NULL,
	.priority = 0,
	.stack_size = 0,
	.login_check = NULL,
	.keep_stdio = false
};

struct rtems_ftpd_configuration rtems_ftpd_configuration = {
	/* FTPD task priority */
	.priority = 100,

	/* Maximum buffersize for hooks */
	.max_hook_filesize = 0,

	/* Well-known port */
	.port = 21,

	/* List of hooks */
	.hooks = NULL,

	/* Root for FTPD or NULL for "/" */
	.root = NULL,

	/* Max. connections */
	.tasks_count = 4,

	/* Idle timeout in seconds  or 0 for no (infinite) timeout */
	.idle = 5 * 60,

	/* Access: 0 - r/w, 1 - read-only, 2 - write-only, 3 - browse-only */
	.access = 0
};

static void
test_main(void)
{
	int rv;

	ifconfig_ppp0();

	rv = rtems_pppd_initialize();
	assert(rv == 0);

	set_pppd_options();
	set_pppd_hooks();

	rv = rtems_pppd_connect();
	assert(rv == 0);

	rtems_task_delete(RTEMS_SELF);
	assert(0);
}

RTEMS_BSD_DEFINE_NEXUS_DEVICE(ppp, 0, 0, NULL);

#else /* RTEMS_SMP */

static void
test_main(void)
{
	printf("PPP is not support on SMP configurations");
	exit(0);
}

#endif /* RTEMS_SMP */

#define CONFIGURE_MAXIMUM_DRIVERS 32

#define DEFAULT_NETWORK_SHELL
#define DEFAULT_NETWORK_DHCPCD_ENABLE
#define DEFAULT_NETWORK_NO_INTERFACE_0

#include <rtems/bsd/test/default-network-init.h>
