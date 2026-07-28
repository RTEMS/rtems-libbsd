/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * The following system calls are tested on libbsd descriptors: dup() and
 * fcntl(F_DUPFD).
 */

/*
 * Copyright (C) 2026 Sang Woo Kim <sngwkim915@gmail.com>
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
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <rtems.h>
#include <rtems/libio_.h>

#define TEST_NAME "LIBBSD DUP 1"

/* How many libio descriptors are on the free list. */
static uint32_t
free_iops(void)
{
	uint32_t i;
	uint32_t n = 0;

	for (i = 0; i < rtems_libio_number_iops; ++i) {
		if ((rtems_libio_iop_flags(&rtems_libio_iops[i]) &
		    LIBIO_FLAGS_FREE) != 0) {
			++n;
		}
	}

	return (n);
}

static void
mkpair(int sp[2])
{
	int rv;

	rv = socketpair(AF_UNIX, SOCK_STREAM, 0, sp);
	assert(rv == 0);
}

static void
test_dup_reaches_the_same_socket(void)
{
	int sp[2];
	int d;
	ssize_t n;
	int rv;
	char buf[4];

	puts("test dup() of a socket");

	mkpair(sp);

	d = dup(sp[0]);
	assert(d >= 0);
	assert(d != sp[0]);

	/* Both descriptors reach the same socket. */
	n = write(sp[1], "ab", 2);
	assert(n == 2);
	n = read(sp[0], &buf[0], 1);
	assert(n == 1);
	assert(buf[0] == 'a');
	n = read(d, &buf[0], 1);
	assert(n == 1);
	assert(buf[0] == 'b');
	n = write(d, "c", 1);
	assert(n == 1);
	n = read(sp[1], &buf[0], 1);
	assert(n == 1);
	assert(buf[0] == 'c');

	rv = close(sp[0]);
	assert(rv == 0);
	rv = close(sp[1]);
	assert(rv == 0);
	rv = close(d);
	assert(rv == 0);
}

static void
test_shared_file_status_flags(void)
{
	int sp[2];
	int d;
	int flags;
	ssize_t n;
	int rv;
	char buf[1];

	puts("test one open file description, shared status flags");

	mkpair(sp);

	d = dup(sp[0]);
	assert(d >= 0);

	/* Setting O_NONBLOCK through one descriptor affects the other. */
	flags = fcntl(sp[0], F_GETFL, 0);
	assert(flags != -1);
	rv = fcntl(sp[0], F_SETFL, flags | O_NONBLOCK);
	assert(rv != -1);

	errno = 0;
	n = read(d, &buf[0], sizeof(buf));
	assert(n == -1);
	assert(errno == EAGAIN);

	rv = fcntl(sp[0], F_SETFL, flags);
	assert(rv != -1);

	rv = close(sp[0]);
	assert(rv == 0);
	rv = close(sp[1]);
	assert(rv == 0);
	rv = close(d);
	assert(rv == 0);
}

static void
test_close_of_one_descriptor(void)
{
	int sp[2];
	int d;
	ssize_t n;
	int rv;
	struct pollfd pfd;
	char buf[1];

	puts("test closing one descriptor leaves the other usable");

	mkpair(sp);

	d = dup(sp[0]);
	assert(d >= 0);

	rv = close(sp[0]);
	assert(rv == 0);

	/* The duplicate still reads and writes. */
	n = write(sp[1], "e", 1);
	assert(n == 1);
	n = read(d, &buf[0], 1);
	assert(n == 1);
	assert(buf[0] == 'e');
	n = write(d, "f", 1);
	assert(n == 1);
	n = read(sp[1], &buf[0], 1);
	assert(n == 1);
	assert(buf[0] == 'f');

	/* The peer sees EOF only after the last close. */
	pfd.fd = sp[1];
	pfd.events = POLLIN;
	pfd.revents = 0;
	rv = poll(&pfd, 1, 100);
	assert(rv == 0);

	rv = close(d);
	assert(rv == 0);

	pfd.fd = sp[1];
	pfd.events = POLLIN;
	pfd.revents = 0;
	rv = poll(&pfd, 1, 3000);
	assert(rv == 1);
	n = read(sp[1], &buf[0], sizeof(buf));
	assert(n == 0);

	rv = close(sp[1]);
	assert(rv == 0);
}

static void
test_fcntl_dupfd(void)
{
	int sp[2];
	int d;
	int rv;
	ssize_t n;
	char buf[1];

	puts("test fcntl(F_DUPFD) of a socket");

	mkpair(sp);

	d = fcntl(sp[0], F_DUPFD, 0);
	assert(d >= 0);
	assert(d != sp[0]);

	n = write(sp[1], "g", 1);
	assert(n == 1);
	n = read(d, &buf[0], 1);
	assert(n == 1);
	assert(buf[0] == 'g');

	rv = close(d);
	assert(rv == 0);
	rv = close(sp[0]);
	assert(rv == 0);
	rv = close(sp[1]);
	assert(rv == 0);
}

static void
test_no_descriptor_leak(void)
{
	int sp[2];
	uint32_t before;
	uint32_t after;
	uint32_t i;
	int rv;

	puts("test dup()/close() does not leak descriptors");

	mkpair(sp);
	before = free_iops();

	/* More rounds than there are descriptors, so a leak of even one
	 * iop per round runs the pool out and fails the dup() below. */
	for (i = 0; i < rtems_libio_number_iops + 8; ++i) {
		int d = dup(sp[0]);
		assert(d >= 0);
		rv = close(d);
		assert(rv == 0);
	}

	/* Descriptors closed by the earlier tests may still be released
	 * while this test runs, so the free count may rise; a fall is a
	 * leak. */
	after = free_iops();
	assert(after >= before);

	rv = close(sp[0]);
	assert(rv == 0);
	rv = close(sp[1]);
	assert(rv == 0);
}

static void
test_main(void)
{
	test_dup_reaches_the_same_socket();
	test_shared_file_status_flags();
	test_close_of_one_descriptor();
	test_fcntl_dupfd();
	test_no_descriptor_leak();

	exit(0);
}

#include <rtems/bsd/test/default-init.h>
