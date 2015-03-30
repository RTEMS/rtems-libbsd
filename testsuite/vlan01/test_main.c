/*
 * Copyright (c) 2015 embedded brains GmbH.  All rights reserved.
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
#include <net/if.h>

#include <assert.h>
#include <stdlib.h>
#include <sysexits.h>

#include <machine/rtems-bsd-commands.h>

#include <rtems.h>

#define TEST_NAME "LIBBSD VLAN 1"

static void
ifconfig_vlan(char *ifname)
{
	int exit_code;
	char *vlan_create[] = {
		"ifconfig",
		"vlan122",
		"create",
		NULL
	};
	char *vlan_assign[] = {
		"ifconfig",
		"vlan122",
		"vlan",
		"122",
		"vlandev",
		ifname,
		NULL
	};
	char *vlan_ip[] = {
		"ifconfig",
		"vlan122",
		"10.1.122.1/24",
		NULL
	};
	char *vlan_up[] = {
		"ifconfig",
		"vlan122",
		"up",
		NULL
	};
	char *vlan_info[] = {
		"ifconfig",
		"vlan122",
		NULL
	};

	exit_code = rtems_bsd_command_ifconfig(RTEMS_BSD_ARGC(vlan_create),
	    vlan_create);
	assert(exit_code == EX_OK);

	exit_code = rtems_bsd_command_ifconfig(RTEMS_BSD_ARGC(vlan_assign),
	    vlan_assign);
	assert(exit_code == EX_OK);

	exit_code = rtems_bsd_command_ifconfig(RTEMS_BSD_ARGC(vlan_ip),
	    vlan_ip);
	assert(exit_code == EX_OK);

	exit_code = rtems_bsd_command_ifconfig(RTEMS_BSD_ARGC(vlan_up),
	    vlan_up);
	assert(exit_code == EX_OK);

	exit_code = rtems_bsd_command_ifconfig(RTEMS_BSD_ARGC(vlan_info),
	    vlan_info);
	assert(exit_code == EX_OK);
}

static void
test_main(void)
{
	char ifnamebuf[IF_NAMESIZE];
	char *ifname;

	ifname = if_indextoname(1, &ifnamebuf[0]);
	assert(ifname != NULL);

	ifconfig_vlan(ifname);

	exit(0);
}

#include <rtems/bsd/test/default-network-init.h>
