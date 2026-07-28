/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * select() and poll() are tested on descriptors whose libio and BSD numbers
 * differ: every reference a scan takes must be returned to the descriptor it
 * was taken from, and readiness must be reported on the descriptor the caller
 * passed.
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
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>

#include <assert.h>
#include <errno.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <rtems.h>
#include <rtems/libio_.h>

#define TEST_NAME "LIBBSD SELECT, POLL AND KQUEUE 2"

#define SCAN_ROUNDS 50

/* Reference count of a libio descriptor, in LIBIO_FLAGS_REFERENCE_INC
 * units. */
static unsigned
refs(int fd)
{
	rtems_libio_t *iop = rtems_libio_iop(fd);

	return (rtems_libio_iop_flags(iop) / LIBIO_FLAGS_REFERENCE_INC);
}

static void
mkpair(int sp[2])
{
	int rv;

	rv = socketpair(AF_UNIX, SOCK_STREAM, 0, sp);
	assert(rv == 0);
}

static void
test_poll_reference_balance(void)
{
	int sp[2];
	int pf[2];
	struct pollfd pfd[3];
	unsigned r0s;
	unsigned r0p;
	unsigned r0c;
	int i;
	int rv;

	puts("test poll() returns every reference it takes");

	mkpair(sp);
	rv = pipe(pf);
	assert(rv == 0);

	/* The three descriptor kinds an event loop mixes: a socket, a pipe
	 * and a non-libbsd descriptor. */
	r0s = refs(sp[0]);
	r0p = refs(pf[0]);
	r0c = refs(1);

	pfd[0].fd = sp[0];
	pfd[0].events = POLLIN;
	pfd[1].fd = pf[0];
	pfd[1].events = POLLIN;
	pfd[2].fd = 1;
	pfd[2].events = POLLIN;

	for (i = 0; i < SCAN_ROUNDS; ++i) {
		rv = poll(pfd, 3, 0);
		assert(rv >= 0);
	}

	assert(refs(sp[0]) == r0s);
	assert(refs(pf[0]) == r0p);
	assert(refs(1) == r0c);

	/* A reference left behind would make these closes fail. */
	rv = close(sp[0]);
	assert(rv == 0);
	rv = close(sp[1]);
	assert(rv == 0);
	rv = close(pf[0]);
	assert(rv == 0);
	rv = close(pf[1]);
	assert(rv == 0);
}

static void
test_select_reference_balance(void)
{
	int sp[2];
	int pf[2];
	fd_set rd;
	struct timeval tv;
	unsigned r0s;
	unsigned r0p;
	int nfds;
	int i;
	int rv;

	puts("test select() returns every reference it takes");

	mkpair(sp);
	rv = pipe(pf);
	assert(rv == 0);

	r0s = refs(sp[0]);
	r0p = refs(pf[0]);
	nfds = (sp[0] > pf[0] ? sp[0] : pf[0]) + 1;

	for (i = 0; i < SCAN_ROUNDS; ++i) {
		FD_ZERO(&rd);
		FD_SET(sp[0], &rd);
		FD_SET(pf[0], &rd);
		tv.tv_sec = 0;
		tv.tv_usec = 0;
		rv = select(nfds, &rd, NULL, NULL, &tv);
		assert(rv >= 0);
	}

	assert(refs(sp[0]) == r0s);
	assert(refs(pf[0]) == r0p);

	rv = close(sp[0]);
	assert(rv == 0);
	rv = close(sp[1]);
	assert(rv == 0);
	rv = close(pf[0]);
	assert(rv == 0);
	rv = close(pf[1]);
	assert(rv == 0);
}

static void
test_duplicate_is_scanned_as_itself(void)
{
	int sp[2];
	int d;
	struct pollfd pfd;
	fd_set rd;
	struct timeval tv;
	unsigned rd0;
	unsigned ro0;
	int rv;

	puts("test a duplicate is scanned as itself");

	mkpair(sp);
	d = dup(sp[0]);
	assert(d >= 0);

	/* The scan of the duplicate must charge and credit the duplicate,
	 * not the original the shared file was first installed for. */
	rd0 = refs(d);
	ro0 = refs(sp[0]);

	pfd.fd = d;
	pfd.events = POLLIN;
	pfd.revents = 0;
	rv = poll(&pfd, 1, 0);
	assert(rv == 0);
	assert(refs(d) == rd0);
	assert(refs(sp[0]) == ro0);

	FD_ZERO(&rd);
	FD_SET(d, &rd);
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	rv = select(d + 1, &rd, NULL, NULL, &tv);
	assert(rv == 0);
	assert(refs(d) == rd0);
	assert(refs(sp[0]) == ro0);

	rv = close(sp[0]);
	assert(rv == 0);
	rv = close(sp[1]);
	assert(rv == 0);
	rv = close(d);
	assert(rv == 0);
}

static void
test_select_reports_the_polled_descriptor(void)
{
	int sp[2];
	int d;
	fd_set rd;
	struct timeval tv;
	ssize_t n;
	int rv;
	char buf[1];

	puts("test select() reports readiness on the descriptor passed");

	mkpair(sp);
	d = dup(sp[0]);
	assert(d >= 0);

	n = write(sp[1], "a", 1);
	assert(n == 1);

	/* The libio and BSD numbers of the duplicate differ, so a bit index
	 * recomputed from the wrong numbering space lands elsewhere. */
	FD_ZERO(&rd);
	FD_SET(d, &rd);
	tv.tv_sec = 3;
	tv.tv_usec = 0;
	rv = select(d + 1, &rd, NULL, NULL, &tv);
	assert(rv == 1);
	assert(FD_ISSET(d, &rd));

	n = read(d, &buf[0], 1);
	assert(n == 1);

	rv = close(sp[0]);
	assert(rv == 0);
	rv = close(sp[1]);
	assert(rv == 0);
	rv = close(d);
	assert(rv == 0);
}

static void
test_scan_of_an_invalid_descriptor(void)
{
	int sp[2];
	int dead;
	struct pollfd pfd[2];
	fd_set rd;
	struct timeval tv;
	unsigned r0;
	int i;
	int rv;

	puts("test a scan mixing in an invalid descriptor");

	mkpair(sp);

	/* A descriptor number that is currently closed. */
	dead = dup(sp[0]);
	assert(dead >= 0);
	rv = close(dead);
	assert(rv == 0);

	r0 = refs(sp[0]);

	/* poll() answers POLLNVAL for the dead entry and must still scan --
	 * and credit -- the live one. */
	for (i = 0; i < SCAN_ROUNDS; ++i) {
		pfd[0].fd = dead;
		pfd[0].events = POLLIN;
		pfd[0].revents = 0;
		pfd[1].fd = sp[0];
		pfd[1].events = POLLIN;
		pfd[1].revents = 0;
		rv = poll(pfd, 2, 0);
		assert(rv == 1);
		assert(pfd[0].revents == POLLNVAL);
		assert(pfd[1].revents == 0);
	}
	assert(refs(sp[0]) == r0);

	/* select() answers EBADF and must leave no reference behind. */
	for (i = 0; i < SCAN_ROUNDS; ++i) {
		FD_ZERO(&rd);
		FD_SET(dead, &rd);
		FD_SET(sp[0], &rd);
		tv.tv_sec = 0;
		tv.tv_usec = 0;
		errno = 0;
		rv = select((dead > sp[0] ? dead : sp[0]) + 1, &rd, NULL,
		    NULL, &tv);
		assert(rv == -1);
		assert(errno == EBADF);
	}
	assert(refs(sp[0]) == r0);

	rv = close(sp[0]);
	assert(rv == 0);
	rv = close(sp[1]);
	assert(rv == 0);
}

static void
test_main(void)
{
	test_poll_reference_balance();
	test_select_reference_balance();
	test_duplicate_is_scanned_as_itself();
	test_select_reports_the_polled_descriptor();
	test_scan_of_an_invalid_descriptor();

	exit(0);
}

#include <rtems/bsd/test/default-init.h>
