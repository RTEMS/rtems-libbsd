/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief Example code to test multicasts.
 *
 * ping 224.0.0.1
 *
 * Linux:
 *
 * echo 0 > /proc/sys/net/ipv4/icmp_echo_ignore_broadcasts
 * echo 1 > /proc/sys/net/ipv4/ip_forward
 * ip route add 224.0.0.0/4 dev eth0
 * echo "Hello, world!" > /dev/udp/239.1.2.3/1234
 */

/*
 * Copyright (C) 2020 embedded brains GmbH (http://www.embedded-brains.de)
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/param.h>
#include <sys/socket.h>
#include <sys/sysctl.h>

#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>

#include <arpa/inet.h>
#include <netinet/in.h>

#include <machine/rtems-bsd-commands.h>

#include <rtems/console.h>
#include <rtems/shell.h>
#include <rtems/dhcpcd.h>

#define TEST_NAME "LIBBSD MCAST 1"
#define TEST_STATE_USER_INPUT 1

#define MCAST_PORT 1234

#define MAX_PACKET_SIZE UINT16_MAX

static const char mcast_addr[] = "239.1.2.3";

static const char hello[] = "Hello, world!";

typedef struct {
	in_addr_t addr;
	char name[];
} iface_binding;

static void
set_mcast_route(const char *iface_name)
{
	int exit_code;
	char *route[] = {
		"route",
		"add",
		"-net",
		"224.0.0.0/4",
		"-iface",
		iface_name,
		NULL
	};

	exit_code = rtems_bsd_command_route(RTEMS_BSD_ARGC(route), route);
	assert(exit_code == EXIT_SUCCESS);
}

static void
mcast_rx_task(rtems_task_argument arg)
{
	iface_binding *ifb;
	struct ip_mreq mreq;
	int sd;
	struct sockaddr_in addr;
	int rv;
	char addr_buf[INET_ADDRSTRLEN];
	const char *ip;
	ssize_t n;

	ifb = (iface_binding *)arg;
	set_mcast_route(ifb->name);

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = ifb->addr;

	ip = inet_ntop(AF_INET, &addr.sin_addr, addr_buf, sizeof(addr_buf));
	printf("mcast: join %s using interface IP %s\n", mcast_addr, ip);

	sd = socket(PF_INET,  SOCK_DGRAM, IPPROTO_UDP);
	assert(sd >= 0);

	addr.sin_port = htons(MCAST_PORT);
	addr.sin_addr.s_addr = INADDR_ANY;

	rv = bind(sd, (const struct sockaddr *)&addr, sizeof(addr));
	assert(rv == 0);

	memset(&mreq, 0, sizeof(mreq));
	mreq.imr_multiaddr.s_addr = inet_addr(mcast_addr);
	mreq.imr_interface.s_addr = ifb->addr;

	rv = setsockopt(sd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));
	assert(rv == 0);

	addr.sin_family = AF_INET;
	addr.sin_port = htons(MCAST_PORT);
	addr.sin_addr.s_addr = inet_addr(mcast_addr);
	n = sendto(sd, hello, sizeof(hello), 0,
	    (const struct sockaddr *)&addr, sizeof(addr));

	if (n != (ssize_t)sizeof(hello)) {
		perror("send");
	}

	while (true) {
		socklen_t addr_len;
		char buf[MAX_PACKET_SIZE];

		addr_len = sizeof(addr);
		n = recvfrom(sd, buf, sizeof(buf), 0,
		    (struct sockaddr *)&addr, &addr_len);
		if (n >= 0 && addr.sin_addr.s_addr != ifb->addr) {
			size_t m;
			ip = inet_ntop(AF_INET, &addr.sin_addr, addr_buf,
			    sizeof(addr_buf));
			printf("mcast: received %zi bytes from %s\n", n, ip);

			addr.sin_port = htons(MCAST_PORT);
			addr.sin_addr.s_addr = inet_addr(mcast_addr);
			m = (size_t)n;
			n = sendto(sd, buf, m, 0,
			    (const struct sockaddr *)&addr,
			    sizeof(addr));
			assert(n == (ssize_t)m);
		} else if (n < 0 ) {
			perror("recvfrom");
		}
	}
}

static void
start_mcast_rx(const char *iface_ip, const char *iface_name)
{
	rtems_status_code sc;
	rtems_id id;
	iface_binding *ifb;
	size_t len;

	len = strlen(iface_name) + 1;
	ifb = malloc(sizeof(*ifb) + len);
	assert(ifb != NULL);
	ifb->addr = inet_addr(iface_ip);
	memcpy(ifb->name, iface_name, len);

	sc = rtems_task_create(rtems_build_name('M', 'C', 'R', 'X'),
	    RTEMS_MINIMUM_PRIORITY, RTEMS_MINIMUM_STACK_SIZE + MAX_PACKET_SIZE,
	    RTEMS_DEFAULT_MODES, RTEMS_FLOATING_POINT, &id);
	assert(sc == RTEMS_SUCCESSFUL);

	sc = rtems_task_start(id, mcast_rx_task, (rtems_task_argument)ifb);
	assert(sc == RTEMS_SUCCESSFUL);
}

static const char *
get_value(char *const *env, const char *key)
{
	size_t len;

	len = strlen(key);

	while (true) {
		const char *s;

		s = *env;

		if (s == NULL) {
			return "";
		}

		if (strncmp(key, s, len) == 0 && s[len] == '=') {
			return &s[len + 1];
		}

		++env;
	}
}

static void
dhcpcd_hook_handler(rtems_dhcpcd_hook *hook, char *const *env)
{

	(void)hook;

	if (strcmp(get_value(env, "reason"), "BOUND") == 0) {
		start_mcast_rx(get_value(env, "new_ip_address"),
		    get_value(env, "interface"));
	}
}

static rtems_dhcpcd_hook dhcpcd_hook = {
	.name = "mcast",
	.handler = dhcpcd_hook_handler
};

static void
test_main(void)
{
	rtems_status_code sc;
	int enable;
	int rv;

	rtems_dhcpcd_add_hook(&dhcpcd_hook);

	enable = 1;
	rv = sysctlbyname("net.inet.icmp.bmcastecho", NULL, NULL, &enable,
	    sizeof(enable));
	assert(rv == 0);

	sc = rtems_shell_init("SHLL", 16 * 1024, 1, CONSOLE_DEVICE_NAME,
	    false, true, NULL);
	assert(sc == RTEMS_SUCCESSFUL);

	exit(0);
}

#define DEFAULT_NETWORK_DHCPCD_ENABLE

#include <rtems/bsd/test/default-network-init.h>

#define CONFIGURE_SHELL_COMMANDS_INIT

#include <bsp/irq-info.h>

#include <rtems/netcmds-config.h>

#define CONFIGURE_SHELL_USER_COMMANDS \
  &bsp_interrupt_shell_command, \
  &rtems_shell_ARP_Command, \
  &rtems_shell_HOSTNAME_Command, \
  &rtems_shell_PING_Command, \
  &rtems_shell_ROUTE_Command, \
  &rtems_shell_NETSTAT_Command, \
  &rtems_shell_SYSCTL_Command, \
  &rtems_shell_IFCONFIG_Command, \
  &rtems_shell_IFMCSTAT_Command, \
  &rtems_shell_VMSTAT_Command

#define CONFIGURE_SHELL_COMMAND_CPUINFO
#define CONFIGURE_SHELL_COMMAND_CPUUSE
#define CONFIGURE_SHELL_COMMAND_PERIODUSE
#define CONFIGURE_SHELL_COMMAND_STACKUSE
#define CONFIGURE_SHELL_COMMAND_MALLOC_INFO

#include <rtems/shellconfig.h>
