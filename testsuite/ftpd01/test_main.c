/**
 * @file
 *
 * @brief It starts an FTP daemon (ftpd).
 */

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

#include <rtems.h>
#include <rtems/ftpd.h>
#include <rtems/shell.h>
#include <rtems/console.h>

#define TEST_NAME "LIBBSD FTPD 1"
#define TEST_STATE_USER_INPUT 1

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

	rv = rtems_initialize_ftpd();
	assert(rv == 0);

	rtems_task_exit();
}

#define DEFAULT_NETWORK_DHCPCD_ENABLE
#define DEFAULT_NETWORK_SHELL

#include <rtems/bsd/test/default-network-init.h>
