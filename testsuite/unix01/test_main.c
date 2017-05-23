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

#include <sys/cdefs.h>
#include <sys/param.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <fcntl.h>

#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <rtems.h>
#include <rtems/libcsupport.h>

#define TEST_NAME "LIBBSD UNIX 1"

static const char name[] = "unix01.sock";

static void
test_unix_socket(bool do_resource_check)
{
	rtems_resource_snapshot snapshot;
	int sd;
	int rv;

	printf("test UNIX(4) socket, %s resource check\n",
	    do_resource_check ? "with" : "without");

	rtems_resource_snapshot_take(&snapshot);

	sd = socket(PF_UNIX, SOCK_STREAM, 0);
	assert(sd >= 0);

	rv = close(sd);
	assert(rv == 0);

	if (do_resource_check) {
		assert(rtems_resource_snapshot_check(&snapshot));
	}
}

static void
test_unix_read_write_not_connected(void)
{
	rtems_resource_snapshot snapshot;
	int sd;
	int rv;
	ssize_t n;
	char buf[1];

	puts("test UNIX(4) read/write not connected");

	rtems_resource_snapshot_take(&snapshot);

	sd = socket(PF_UNIX, SOCK_STREAM, 0);
	assert(sd >= 0);

	errno = 0;
	n = write(sd, &buf[0], sizeof(buf));
	assert(n == -1);
	assert(errno == ENOTCONN);

	errno = 0;
	n = read(sd, &buf[0], sizeof(buf));
	assert(n == -1);
	assert(errno == ENOTCONN);

	rv = close(sd);
	assert(rv == 0);

	assert(rtems_resource_snapshot_check(&snapshot));
}

static void
init_address(struct sockaddr_un *addr)
{
	addr->sun_family = AF_UNIX;
	strcpy(&addr->sun_path[0], &name[0]);
}

static void
test_unix_bind(bool do_close_before_unlink)
{
	rtems_resource_snapshot snapshot;
	int sd;
	int rv;
	struct sockaddr_un addr;

	printf("test UNIX(4) bind, %s close before unlink\n",
	    do_close_before_unlink ? "with" : "without");

	rtems_resource_snapshot_take(&snapshot);

	sd = socket(PF_UNIX, SOCK_STREAM, 0);
	assert(sd >= 0);

	init_address(&addr);

	errno = 0;
	rv = open(&name[0], O_RDWR);
	assert(rv == -1);
	assert(errno == ENOENT);

	rv = bind(sd, (const struct sockaddr *) &addr, SUN_LEN(&addr));
	assert(rv == 0);

	errno = 0;
	rv = open(&name[0], O_RDWR);
	assert(rv == -1);
	assert(errno == ENXIO);

	if (do_close_before_unlink) {
		rv = close(sd);
		assert(rv == 0);

		rv = unlink(&name[0]);
		assert(rv == 0);
	} else {
		rv = unlink(&name[0]);
		assert(rv == 0);

		rv = close(sd);
		assert(rv == 0);
	}

	assert(rtems_resource_snapshot_check(&snapshot));
}

static void
test_unix_listen_connect_accept_write_read(void)
{
	static const char sd2_to_sd3[] = "some data from sd2 for sd3";
	static const char sd3_to_sd2[] = "and other data from sd3 for sd2";

	rtems_resource_snapshot snapshot;
	int sd;
	int sd2;
	int sd3;
	int rv;
	struct sockaddr_un addr;
	struct sockaddr_un addr3;
	socklen_t addr3_len;
	ssize_t n;
	char buf[MAX(sizeof(sd2_to_sd3), sizeof(sd3_to_sd2))];

	puts("test UNIX(4) listen/connect/accept/write/read");

	rtems_resource_snapshot_take(&snapshot);

	sd = socket(PF_UNIX, SOCK_STREAM, 0);
	assert(sd >= 0);

	init_address(&addr);

	rv = bind(sd, (const struct sockaddr *) &addr, SUN_LEN(&addr));
	assert(rv == 0);

	rv = listen(sd, 0);
	assert(rv == 0);

	sd2 = socket(PF_UNIX, SOCK_STREAM, 0);
	assert(sd2 >= 0);

	rv = connect(sd2, (const struct sockaddr *) &addr, SUN_LEN(&addr));
	assert(rv == 0);

	addr3_len = sizeof(addr3);
	sd3 = accept(sd, (struct sockaddr *) &addr3, &addr3_len);
	assert(sd3 >= 0);

	n = write(sd2, &sd2_to_sd3[0], sizeof(sd2_to_sd3));
	assert(n == (ssize_t) sizeof(sd2_to_sd3));

	n = write(sd3, &sd3_to_sd2[0], sizeof(sd3_to_sd2));
	assert(n == (ssize_t) sizeof(sd3_to_sd2));

	memset(&buf[0], 'X', sizeof(buf));
	n = read(sd3, &buf[0], sizeof(buf));
	assert(n == (ssize_t) sizeof(sd2_to_sd3));
	assert(memcmp(&sd2_to_sd3[0], &buf[0], sizeof(sd2_to_sd3)) == 0);

	memset(&buf[0], 'X', sizeof(buf));
	n = read(sd2, &buf[0], sizeof(buf));
	assert(n == (ssize_t) sizeof(sd3_to_sd2));
	assert(memcmp(&sd3_to_sd2[0], &buf[0], sizeof(sd3_to_sd2)) == 0);

	rv = close(sd3);
	assert(rv == 0);

	rv = close(sd2);
	assert(rv == 0);

	rv = close(sd);
	assert(rv == 0);

	rv = unlink(&name[0]);
	assert(rv == 0);

	assert(rtems_resource_snapshot_check(&snapshot));
}

static void
test_main(void)
{
	test_unix_socket(false);
	test_unix_socket(true);
	test_unix_read_write_not_connected();
	test_unix_bind(false);
	test_unix_bind(true);
	test_unix_listen_connect_accept_write_read();

	exit(0);
}

#include <rtems/bsd/test/default-init.h>
