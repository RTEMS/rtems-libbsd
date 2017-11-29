/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief TODO.
 */

/*
 * Copyright (c) 2009-2015 embedded brains GmbH.  All rights reserved.
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

#include <rtems/bsd/bsd.h>

#include <assert.h>
#include <sysexits.h>

#include <machine/rtems-bsd-commands.h>

#include <bsp.h>

static void
dhcpcd_task(rtems_task_argument arg)
{
	int exit_code;
	char *dhcpcd[] = {
	    "dhcpcd",
	    NULL
	};

	(void)arg;

	exit_code = rtems_bsd_command_dhcpcd(RTEMS_BSD_ARGC(dhcpcd), dhcpcd);
	assert(exit_code == EX_OK);
	(void)exit_code;
}

rtems_status_code
rtems_bsd_initialize_dhcp(void)
{
	rtems_status_code sc;
	int exit_code;
	rtems_id id;

	sc = rtems_bsd_initialize();
	if (sc != RTEMS_SUCCESSFUL) {
		return (sc);
	}

	exit_code = rtems_bsd_ifconfig_lo0();
	if (exit_code != EX_OK) {
		return (RTEMS_UNSATISFIED);
	}

	sc = rtems_task_create(
		rtems_build_name('D', 'H', 'C', 'P'),
		RTEMS_MAXIMUM_PRIORITY - 1,
		2 * RTEMS_MINIMUM_STACK_SIZE,
		RTEMS_DEFAULT_MODES,
		RTEMS_FLOATING_POINT,
		&id
	);
	if (sc != RTEMS_SUCCESSFUL) {
		return (RTEMS_UNSATISFIED);
	}

	sc = rtems_task_start(id, dhcpcd_task, 0);
	assert(sc == RTEMS_SUCCESSFUL);

	return (RTEMS_SUCCESSFUL);
}

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
