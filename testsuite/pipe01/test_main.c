/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * The following system calls are tested on a pipe: pipe(), pipe2(), poll(),
 * select(), kevent(), read(), write() and close().
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
#include <sys/select.h>
#include <sys/time.h>

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <rtems.h>
#include <rtems/libio_.h>

#define TEST_NAME "LIBBSD PIPE 1"

#define PIPE_HOLD_COUNT 16

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

static long
ms_since(const struct timespec *t0)
{
	struct timespec t1;

	clock_gettime(CLOCK_MONOTONIC, &t1);
	return ((t1.tv_sec - t0->tv_sec) * 1000L +
	    (t1.tv_nsec - t0->tv_nsec) / 1000000L);
}

static short
poll_one(int fd, short events, int msec)
{
	struct pollfd pfd;
	int rv;

	pfd.fd = fd;
	pfd.events = events;
	pfd.revents = 0;
	rv = poll(&pfd, 1, msec);
	assert(rv >= 0);
	return (pfd.revents);
}

static void *
delayed_write(void *arg)
{
	int fd = *(int *)arg;
	struct timespec ts = { 0, 300000000L };
	ssize_t n;

	nanosleep(&ts, NULL);
	n = write(fd, "w", 1);
	assert(n == 1);
	return (NULL);
}

static void
test_pipe_read_write(void)
{
	int fd[2];
	ssize_t n;
	int rv;
	char buf[1];

	puts("test pipe() read and write");

	rv = pipe(fd);
	assert(rv == 0);
	assert(fd[0] >= 0);
	assert(fd[1] >= 0);

	n = write(fd[1], "x", 1);
	assert(n == 1);
	n = read(fd[0], &buf[0], 1);
	assert(n == 1);
	assert(buf[0] == 'x');

	rv = close(fd[0]);
	assert(rv == 0);
	rv = close(fd[1]);
	assert(rv == 0);
}

static void
test_pipe_poll(void)
{
	int fd[2];
	short revents;
	ssize_t n;
	int rv;
	char buf[1];
	struct timespec t0;
	long ms;

	puts("test poll() on a pipe");

	rv = pipe(fd);
	assert(rv == 0);

	/* An idle pipe is neither readable nor in error; the write end is
	 * writable. */
	revents = poll_one(fd[0], POLLIN, 0);
	assert(revents == 0);
	revents = poll_one(fd[1], POLLOUT, 0);
	assert(revents == POLLOUT);

	/* poll() follows the data. */
	n = write(fd[1], "y", 1);
	assert(n == 1);
	revents = poll_one(fd[0], POLLIN, 0);
	assert((revents & POLLIN) != 0);
	assert((revents & (POLLERR | POLLHUP)) == 0);
	n = read(fd[0], &buf[0], 1);
	assert(n == 1);
	revents = poll_one(fd[0], POLLIN, 0);
	assert(revents == 0);

	/* An idle poll() must wait out its timeout, not return at once. */
	clock_gettime(CLOCK_MONOTONIC, &t0);
	revents = poll_one(fd[0], POLLIN, 300);
	ms = ms_since(&t0);
	assert(revents == 0);
	assert(ms >= 250);

	rv = close(fd[0]);
	assert(rv == 0);
	rv = close(fd[1]);
	assert(rv == 0);
}

static void
test_pipe_poll_wakeup(void)
{
	int fd[2];
	pthread_t th;
	struct pollfd pfd;
	struct timespec t0;
	long ms;
	ssize_t n;
	int rv;
	char buf[1];

	puts("test a blocking poll() wakes on a pipe write");

	rv = pipe(fd);
	assert(rv == 0);

	rv = pthread_create(&th, NULL, delayed_write, &fd[1]);
	assert(rv == 0);

	clock_gettime(CLOCK_MONOTONIC, &t0);
	pfd.fd = fd[0];
	pfd.events = POLLIN;
	pfd.revents = 0;
	rv = poll(&pfd, 1, 10000);
	ms = ms_since(&t0);
	assert(rv == 1);
	assert((pfd.revents & POLLIN) != 0);
	assert(ms >= 200);
	assert(ms < 5000);

	rv = pthread_join(th, NULL);
	assert(rv == 0);
	n = read(fd[0], &buf[0], 1);
	assert(n == 1);

	rv = close(fd[0]);
	assert(rv == 0);
	rv = close(fd[1]);
	assert(rv == 0);
}

static void
test_pipe_select(void)
{
	int fd[2];
	fd_set rd;
	struct timeval tv;
	ssize_t n;
	int rv;
	char buf[1];

	puts("test select() on a pipe");

	rv = pipe(fd);
	assert(rv == 0);

	FD_ZERO(&rd);
	FD_SET(fd[0], &rd);
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	rv = select(fd[0] + 1, &rd, NULL, NULL, &tv);
	assert(rv == 0);

	n = write(fd[1], "z", 1);
	assert(n == 1);

	FD_ZERO(&rd);
	FD_SET(fd[0], &rd);
	tv.tv_sec = 3;
	tv.tv_usec = 0;
	rv = select(fd[0] + 1, &rd, NULL, NULL, &tv);
	assert(rv == 1);
	assert(FD_ISSET(fd[0], &rd));
	n = read(fd[0], &buf[0], 1);
	assert(n == 1);

	rv = close(fd[0]);
	assert(rv == 0);
	rv = close(fd[1]);
	assert(rv == 0);
}

static void
test_pipe_kqueue(void)
{
	int fd[2];
	int kq;
	struct kevent ev;
	struct timespec ts = { 0, 0 };
	ssize_t n;
	int rv;
	char buf[1];

	puts("test kevent() on a pipe");

	rv = pipe(fd);
	assert(rv == 0);
	kq = kqueue();
	assert(kq >= 0);

	EV_SET(&ev, fd[0], EVFILT_READ, EV_ADD, 0, 0, NULL);
	rv = kevent(kq, &ev, 1, NULL, 0, NULL);
	assert(rv == 0);
	EV_SET(&ev, fd[1], EVFILT_WRITE, EV_ADD, 0, 0, NULL);
	rv = kevent(kq, &ev, 1, NULL, 0, NULL);
	assert(rv == 0);

	/* The write end is writable at once. */
	rv = kevent(kq, NULL, 0, &ev, 1, &ts);
	assert(rv == 1);
	assert(ev.ident == (uintptr_t)fd[1]);
	assert(ev.filter == EVFILT_WRITE);

	/* Deregister the level-triggered write filter so it cannot outrank
	 * the events below. */
	EV_SET(&ev, fd[1], EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
	rv = kevent(kq, &ev, 1, NULL, 0, NULL);
	assert(rv == 0);

	/* The read end becomes readable after a write. */
	n = write(fd[1], "k", 1);
	assert(n == 1);
	ts.tv_sec = 3;
	rv = kevent(kq, NULL, 0, &ev, 1, &ts);
	assert(rv == 1);
	assert(ev.ident == (uintptr_t)fd[0]);
	assert(ev.filter == EVFILT_READ);
	n = read(fd[0], &buf[0], 1);
	assert(n == 1);

	/* The reader sees EV_EOF when the write end closes. */
	rv = close(fd[1]);
	assert(rv == 0);
	rv = kevent(kq, NULL, 0, &ev, 1, &ts);
	assert(rv == 1);
	assert(ev.ident == (uintptr_t)fd[0]);
	assert((ev.flags & EV_EOF) != 0);

	rv = close(fd[0]);
	assert(rv == 0);
	rv = close(kq);
	assert(rv == 0);
}

static void
test_pipe2(void)
{
	int fd[2];
	ssize_t n;
	int rv;
	char buf[1];

	puts("test pipe2()");

	rv = pipe2(fd, O_NONBLOCK);
	assert(rv == 0);

	errno = 0;
	n = read(fd[0], &buf[0], sizeof(buf));
	assert(n == -1);
	assert(errno == EAGAIN);

	rv = close(fd[0]);
	assert(rv == 0);
	rv = close(fd[1]);
	assert(rv == 0);

	errno = 0;
	rv = pipe2(fd, ~(O_NONBLOCK | O_CLOEXEC));
	assert(rv == -1);
	assert(errno == EINVAL);
}

static void
test_no_descriptor_leak(void)
{
	int held[PIPE_HOLD_COUNT][2];
	int fd[2];
	uint32_t before;
	int made;
	int i;
	int rv;

	puts("test pipes do not leak descriptors");

	before = free_iops();

	/* Hold as many pipes as the descriptor pool allows. */
	for (made = 0; made < PIPE_HOLD_COUNT; ++made) {
		rv = pipe(held[made]);
		if (rv != 0)
			break;
	}
	assert(made > 0);
	for (i = 0; i < made; ++i) {
		rv = close(held[i][0]);
		assert(rv == 0);
		rv = close(held[i][1]);
		assert(rv == 0);
	}

	/* Descriptors closed by the earlier tests may still be released
	 * while this test runs, so the free count may rise; a fall is a
	 * leak. */
	assert(free_iops() >= before);

	/* The pool still serves a fresh pipe. */
	rv = pipe(fd);
	assert(rv == 0);
	rv = close(fd[0]);
	assert(rv == 0);
	rv = close(fd[1]);
	assert(rv == 0);
}

static void
test_main(void)
{
	test_pipe_read_write();
	test_pipe_poll();
	test_pipe_poll_wakeup();
	test_pipe_select();
	test_pipe_kqueue();
	test_pipe2();
	test_no_descriptor_leak();

	exit(0);
}

#include <rtems/bsd/test/default-init.h>
