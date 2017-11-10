/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief TODO.
 */

/*
 * Copyright (c) 2013, 2017 embedded brains GmbH.  All rights reserved.
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

#include <machine/rtems-bsd-commands.h>

#include <sysexits.h>

int
rtems_bsd_ifconfig_lo0(void)
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
	    "alias",
	    NULL
	};

	exit_code = rtems_bsd_command_ifconfig(RTEMS_BSD_ARGC(lo0), lo0);
	if (exit_code != EX_OK) {
		return (exit_code);
	}

	exit_code = rtems_bsd_command_ifconfig(RTEMS_BSD_ARGC(lo0_inet6), lo0_inet6);
	return (exit_code);
}
