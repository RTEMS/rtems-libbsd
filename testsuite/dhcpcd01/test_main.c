/**
 * @file
 *
 * @brief It tests to add a hook to DHCP client daemon (dhcpcd).
 *
 * Every time the hook is called, the environment is printed out.
 */

/*
 * Copyright (c) 2013, 2018 embedded brains GmbH.  All rights reserved.
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

#include <rtems.h>
#include <rtems/dhcpcd.h>

#define TEST_NAME "LIBBSD DHCPCD 1"

static void
dhcpcd_hook_handler(rtems_dhcpcd_hook *hook, char *const *env)
{

	(void)hook;

	while (*env != NULL) {
		printf("%s\n", *env);
		++env;
	}
}

static rtems_dhcpcd_hook dhcpcd_hook = {
	.name = "test",
	.handler = dhcpcd_hook_handler
};

static void
test_main(void)
{

	rtems_dhcpcd_add_hook(&dhcpcd_hook);

	rtems_task_delete(RTEMS_SELF);
	assert(0);
}

#define DEFAULT_NETWORK_DHCPCD_ENABLE
#define DEFAULT_NETWORK_SHELL

#include <rtems/bsd/test/default-network-init.h>
