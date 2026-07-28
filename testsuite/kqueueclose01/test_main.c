/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * close() of kqueue-registered descriptors is tested: the close must
 * succeed, must drop the knotes of the closing descriptor and no other,
 * and repeated registration must not leak references.
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
#include <sys/event.h>
#include <sys/socket.h>
#include <sys/time.h>

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <rtems.h>
#include <rtems/libio_.h>

#define TEST_NAME "LIBBSD KQUEUE CLOSE 1"

#define PAIR_COUNT 8

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

static int
kchange(int kq, int fd, int filter, int flags)
{
	struct kevent ch;

	EV_SET(&ch, fd, filter, flags, 0, 0, NULL);
	return (kevent(kq, &ch, 1, NULL, 0, NULL));
}

static int
kwait(int kq, struct kevent *ev, int nev, int msec)
{
	struct timespec ts;

	ts.tv_sec = msec / 1000;
	ts.tv_nsec = (msec % 1000) * 1000000L;
	return (kevent(kq, NULL, 0, ev, nev, &ts));
}

static void
test_close_of_a_registered_descriptor(void)
{
	int sp[2];
	int kq;
	int watcher;
	struct kevent ev;
	int rv;
	int n;

	puts("test close() of a kqueue-registered socket");

	mkpair(sp);
	kq = kqueue();
	assert(kq >= 0);
	watcher = kqueue();
	assert(watcher >= 0);

	rv = kchange(kq, sp[0], EVFILT_READ, EV_ADD);
	assert(rv == 0);
	rv = kchange(watcher, sp[1], EVFILT_READ, EV_ADD);
	assert(rv == 0);

	/* The close must succeed even though the descriptor is registered,
	 * and the peer must see the resulting disconnect. */
	rv = close(sp[0]);
	assert(rv == 0);

	n = kwait(watcher, &ev, 1, 3000);
	assert(n == 1);
	assert((ev.flags & EV_EOF) != 0);

	rv = close(sp[1]);
	assert(rv == 0);
	rv = close(kq);
	assert(rv == 0);
	rv = close(watcher);
	assert(rv == 0);
}

static void
test_close_after_delete(void)
{
	int sp[2];
	int kq;
	int rv;

	puts("test close() after EV_DELETE");

	mkpair(sp);
	kq = kqueue();
	assert(kq >= 0);

	rv = kchange(kq, sp[0], EVFILT_READ, EV_ADD);
	assert(rv == 0);
	rv = kchange(kq, sp[0], EVFILT_READ, EV_DELETE);
	assert(rv == 0);

	/* A second delete has nothing to delete. */
	errno = 0;
	rv = kchange(kq, sp[0], EVFILT_READ, EV_DELETE);
	assert(rv == -1);
	assert(errno == ENOENT);

	rv = close(sp[0]);
	assert(rv == 0);
	rv = close(sp[1]);
	assert(rv == 0);
	rv = close(kq);
	assert(rv == 0);
}

static void
test_no_reference_leak(void)
{
	int sp[2];
	int kq;
	uint32_t before;
	int i;
	int rv;

	puts("test repeated registration does not leak references");

	mkpair(sp);
	kq = kqueue();
	assert(kq >= 0);

	before = free_iops();

	for (i = 0; i < 100; ++i) {
		rv = kchange(kq, sp[0], EVFILT_READ, EV_ADD);
		assert(rv == 0);
		rv = kchange(kq, sp[0], EVFILT_READ, EV_DELETE);
		assert(rv == 0);
	}

	assert(free_iops() == before);

	/* A leaked reference would make this close fail or hang. */
	rv = close(sp[0]);
	assert(rv == 0);
	rv = close(sp[1]);
	assert(rv == 0);
	rv = close(kq);
	assert(rv == 0);
}

static void
test_knotes_follow_the_descriptor(void)
{
	int sp[2];
	int d;
	int kq;
	struct kevent ev;
	ssize_t sn;
	int rv;
	int n;
	char buf[1];

	puts("test a knote belongs to the descriptor, not to the file");

	mkpair(sp);
	kq = kqueue();
	assert(kq >= 0);

	d = dup(sp[0]);
	assert(d >= 0);

	/* Watch through the duplicate only; closing the original must not
	 * drop the duplicate's knote. */
	rv = kchange(kq, d, EVFILT_READ, EV_ADD);
	assert(rv == 0);

	rv = close(sp[0]);
	assert(rv == 0);

	sn = write(sp[1], "a", 1);
	assert(sn == 1);
	n = kwait(kq, &ev, 1, 3000);
	assert(n == 1);
	assert(ev.ident == (uintptr_t)d);

	sn = read(d, &buf[0], 1);
	assert(sn == 1);

	rv = close(d);
	assert(rv == 0);
	rv = close(sp[1]);
	assert(rv == 0);
	rv = close(kq);
	assert(rv == 0);
}

static void
test_close_drops_only_its_own_knotes(void)
{
	int sp[PAIR_COUNT][2];
	int d[PAIR_COUNT];
	int kq;
	struct kevent ev;
	ssize_t sn;
	int rv;
	int n;
	int i;
	char buf[1];

	puts("test close() drops the knotes of the closing descriptor only");

	kq = kqueue();
	assert(kq >= 0);

	/* Register every original, then close every unregistered duplicate.
	 * The BSD descriptor number of a duplicate coincides with the libio
	 * number of some other descriptor, so a close that passes the wrong
	 * numbering space to knote_fdclose() hits a registered original. */
	for (i = 0; i < PAIR_COUNT; ++i) {
		mkpair(sp[i]);
		d[i] = dup(sp[i][0]);
		assert(d[i] >= 0);
		rv = kchange(kq, sp[i][0], EVFILT_READ, EV_ADD);
		assert(rv == 0);
	}

	for (i = 0; i < PAIR_COUNT; ++i) {
		rv = close(d[i]);
		assert(rv == 0);
	}

	/* Every original's knote must still fire. */
	for (i = 0; i < PAIR_COUNT; ++i) {
		sn = write(sp[i][1], "b", 1);
		assert(sn == 1);
		n = kwait(kq, &ev, 1, 3000);
		assert(n == 1);
		assert(ev.ident == (uintptr_t)sp[i][0]);
		sn = read(sp[i][0], &buf[0], 1);
		assert(sn == 1);
	}

	for (i = 0; i < PAIR_COUNT; ++i) {
		rv = close(sp[i][0]);
		assert(rv == 0);
		rv = close(sp[i][1]);
		assert(rv == 0);
	}
	rv = close(kq);
	assert(rv == 0);
}

static void
test_main(void)
{
	test_close_of_a_registered_descriptor();
	test_close_after_delete();
	test_no_reference_leak();
	test_knotes_follow_the_descriptor();
	test_close_drops_only_its_own_knotes();

	exit(0);
}

#include <rtems/bsd/test/default-init.h>
