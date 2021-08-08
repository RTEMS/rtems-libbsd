/**
 * @file
 *
 * @brief It mounts a network file system (nfs).
 */

/*
 * Copyright (c) 2016 embedded brains GmbH.  All rights reserved.
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
#include <unistd.h>

#include <rtems.h>
#include <rtems/libio.h>

#include <librtemsNfs.h>

#include <rtems/bsd/test/network-config.h>

#define TEST_NAME "LIBBSD NFS 1"
#define TEST_WAIT_FOR_LINK NET_CFG_INTERFACE_0
#define TEST_STATE_USER_INPUT 1

static void
test_main(void)
{
	static const char remote_target[] =
	    "1000.100@" NET_CFG_PEER_IP " :/srv/nfs";
	int rv;

	do {
		sleep(1);

		rv = mount_and_make_target_path(&remote_target[0], "/nfs",
		    RTEMS_FILESYSTEM_TYPE_NFS, RTEMS_FILESYSTEM_READ_WRITE,
		    NULL);
	} while (rv != 0);

	rtems_task_delete(RTEMS_SELF);
	assert(0);
}

#define DEFAULT_NETWORK_SHELL

#define CONFIGURE_FILESYSTEM_NFS

#define CONFIGURE_MAXIMUM_DRIVERS 32

#include <rtems/bsd/test/default-network-init.h>
