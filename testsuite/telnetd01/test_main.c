/*
 * Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
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
#include <string.h>

#include <rtems.h>
#include <rtems/shell.h>
#include <rtems/telnetd.h>

#define TEST_NAME "LIBBSD TELNETD 1"
#define TEST_STATE_USER_INPUT 1

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

static void
test_main(void)
{
	rtems_status_code sc = rtems_telnetd_initialize();
	assert(sc == RTEMS_SUCCESSFUL);

	rtems_task_delete(RTEMS_SELF);
	assert(0);
}

#define DEFAULT_NETWORK_SHELL

#define CONFIGURE_MAXIMUM_DRIVERS 32

#include <rtems/bsd/test/default-network-init.h>
