/**
 * @file
 *
 * The following system calls are tested: select(), pselect(), poll(), kqueue() and pipe().
 */

/*
 * Copyright (c) 2013, 2019 embedded brains GmbH.  All rights reserved.
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

#include <sys/param.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/filio.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h>

#include <machine/rtems-bsd-commands.h>

#include <rtems/libcsupport.h>
#include <rtems.h>

#define TEST_NAME "LIBBSD SELECT AND POLL AND KQUEUE AND PIPE 1"

#define PRIO_MASTER 1

#define PRIO_WORKER 2

#define EVENT_READ RTEMS_EVENT_0

#define EVENT_WRITE RTEMS_EVENT_1

#define EVENT_CONNECT RTEMS_EVENT_2

#define EVENT_CLOSE RTEMS_EVENT_3

#define EVENT_SHUTDOWN RTEMS_EVENT_4

#define EVENT_CLOSE_PIPE RTEMS_EVENT_5

#define BUF_SIZE 4096

#define PORT 1234

#define TEST_UDATA ((void *) 0xcafe)

typedef struct {
	char buf[BUF_SIZE];
	const char *wbuf;
	char *rbuf;
	size_t rn;
	size_t wn;
	int lfd;
	int cfd;
	int afd;
	int rfd;
	int wfd;
	int pfd[2];
	struct sockaddr_in caddr;
	rtems_id worker_task;
} test_context;

static test_context test_instance = {
	.cfd = -1
};

static const char msg[] = "This is a message.  One two three.";

static void
setup_lo0(void)
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

	exit_code = rtems_bsd_command_ifconfig(RTEMS_ARRAY_SIZE(lo0) - 1, lo0);
	assert(exit_code == EX_OK);
}

static void
set_self_prio(rtems_task_priority prio)
{
	rtems_status_code sc;

	sc = rtems_task_set_priority(RTEMS_SELF, prio, &prio);
	assert(sc == RTEMS_SUCCESSFUL);
}

static void
create_server_socket(test_context *ctx)
{
	struct sockaddr_in saddr;
	int rv;
	int lfd;

	lfd = socket(PF_INET, SOCK_STREAM, 0);
	assert(lfd >= 0);

	memset(&saddr, 0, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(PORT);
	saddr.sin_addr.s_addr = htonl(INADDR_ANY);

	rv = bind(lfd, (const struct sockaddr *) &saddr, sizeof(saddr));
	assert(rv == 0);

	rv = listen(lfd, 1);
	assert(rv == 0);

	ctx->lfd = lfd;
}

static void
create_client_addr(test_context *ctx)
{
	struct sockaddr_in *caddr = &ctx->caddr;;
	int ok;

	memset(caddr, 0, sizeof(*caddr));
	caddr->sin_family = AF_INET;
	caddr->sin_port = htons(PORT);
	ok = inet_aton("127.0.0.1", &caddr->sin_addr);
	assert(ok != 0);
}

static void
worker_task(rtems_task_argument arg)
{
	test_context *ctx = (test_context *) arg;

	while (true) {
		rtems_status_code sc;
		rtems_event_set events;
		ssize_t n;
		int rv;
		int cfd = ctx->cfd;
		int rfd = ctx->pfd[0];
		int wfd = ctx->pfd[1];

		sc = rtems_event_receive(
			RTEMS_ALL_EVENTS,
			RTEMS_EVENT_ANY | RTEMS_WAIT,
			RTEMS_NO_TIMEOUT,
			&events
		);
		assert(sc == RTEMS_SUCCESSFUL);

		if ((events & EVENT_READ) != 0) {
			puts("worker: read");

			n = read(ctx->rfd, &ctx->rbuf[0], ctx->rn);
			assert(n == (ssize_t) ctx->rn);
		}

		if ((events & EVENT_WRITE) != 0) {
			puts("worker: write");

			n = write(ctx->wfd, &ctx->wbuf[0], ctx->wn);
			assert(n == (ssize_t) ctx->wn);
		}

		if ((events & EVENT_CONNECT) != 0) {
			if (cfd >= 0) {
				puts("worker: close connect socket");

				ctx->cfd = -1;

				rv = close(cfd);
				assert(rv == 0);

				cfd = -1;
			}

			if (cfd < 0) {
				puts("worker: create new connect socket");

				cfd = socket(PF_INET, SOCK_STREAM, 0);
				assert(cfd >= 0);

				ctx->cfd = cfd;
			}

			puts("worker: connect");

			rv = connect(
				cfd,
				(const struct sockaddr *) &ctx->caddr,
				sizeof(ctx->caddr)
			);
			assert(rv == 0);
		}

		if ((events & EVENT_CLOSE) != 0) {
			puts("worker: close");

			ctx->cfd = -1;

			rv = close(cfd);
			assert(rv == 0);
		}

		if ((events & EVENT_CLOSE_PIPE) != 0) {
			puts("worker: close pipe");

			ctx->pfd[0] = -1;
			ctx->pfd[1] = -1;

			rv = close(wfd);
			assert(rv == 0);

			rv = close(rfd);
			assert(rv == 0);
		}

		if ((events & EVENT_SHUTDOWN) != 0) {
			puts("worker: shutdown");

			rv = shutdown(cfd, SHUT_RDWR);
			assert(rv == 0);
		}
	}
}

static void
send_events(test_context *ctx, rtems_event_set events)
{
	rtems_status_code sc;

	sc = rtems_event_send(ctx->worker_task, events);
	assert(sc == RTEMS_SUCCESSFUL);
}

static void
start_worker(test_context *ctx)
{
	rtems_status_code sc;

	sc = rtems_task_create(
		rtems_build_name('W', 'O', 'R', 'K'),
		PRIO_WORKER,
		RTEMS_MINIMUM_STACK_SIZE,
		RTEMS_DEFAULT_MODES,
		RTEMS_FLOATING_POINT,
		&ctx->worker_task
	);
	assert(sc == RTEMS_SUCCESSFUL);

	sc = rtems_task_start(
		ctx->worker_task,
		worker_task,
		(rtems_task_argument) ctx
	);
	assert(sc == RTEMS_SUCCESSFUL);
}

static void
set_non_blocking(int fd, int enable)
{
	int rv;
	int flags = fcntl(fd, F_GETFL, 0);

	if (enable) {
		rv = fcntl(fd, F_SETFL, flags | O_NONBLOCK);
	} else {
		rv = fcntl(fd, F_SETFL, flags & ~O_NONBLOCK);
	}
	assert(rv == 0);
}

static void
test_select_timeout(test_context *ctx)
{
	struct timeval timeout = {
		.tv_sec = 0,
		.tv_usec = 100000
	};
	int fd = ctx->lfd;
	int nfds = fd + 1;
	struct fd_set set;
	int rv;
	int i;

	puts("test select timeout");

	set_non_blocking(ctx->lfd, 0);

	FD_ZERO(&set);
	FD_SET(fd, &set);

	rv = select(nfds, &set, NULL, NULL, &timeout);
	assert(rv == 0);

	for (i = 0; i < nfds; ++i) {
		assert(!FD_ISSET(i, &set));
	}

	rv = select(nfds, NULL, &set, NULL, &timeout);
	assert(rv == 0);

	for (i = 0; i < nfds; ++i) {
		assert(!FD_ISSET(i, &set));
	}

	rv = select(nfds, NULL, NULL, &set, &timeout);
	assert(rv == 0);

	for (i = 0; i < nfds; ++i) {
		assert(!FD_ISSET(i, &set));
	}
}

static void
test_select_connect(test_context *ctx)
{
	int lfd = ctx->lfd;
	int afd = ctx->afd;
	int nfds = lfd + 1;
	struct fd_set set;
	int rv;
	int i;

	puts("test select connect");

	if (afd >= 0) {
		ctx->afd = -1;

		rv = close(afd);
		assert(rv == 0);
	}

	send_events(ctx, EVENT_CONNECT);

	set_non_blocking(lfd, 1);

	errno = 0;
	afd = accept(lfd, NULL, NULL);
	assert(afd == -1);
	assert(errno == EAGAIN);

	set_non_blocking(lfd, 0);

	FD_ZERO(&set);
	FD_SET(lfd, &set);

	rv = select(nfds, &set, NULL, NULL, NULL);
	assert(rv == 1);

	for (i = 0; i < nfds; ++i) {
		bool is_set_expected = i == lfd;
		bool is_set = FD_ISSET(i, &set);

		assert(is_set_expected == is_set);
	}

	afd = accept(lfd, NULL, NULL);
	assert(afd >= 0);

	ctx->afd = afd;
}

static void
test_select_read(test_context *ctx)
{
	int afd = ctx->afd;
	int cfd = ctx->cfd;
	int nfds = afd + 1;
	struct fd_set set;
	int rv;
	int i;
	ssize_t n;

	puts("test select read");

	assert(afd >= 0);
	assert(cfd >= 0);

	ctx->wfd = cfd;
	ctx->wbuf = &msg[0];
	ctx->wn = sizeof(msg);
	send_events(ctx, EVENT_WRITE);

	set_non_blocking(afd, 1);

	errno = 0;
	n = read(afd, &ctx->buf[0], sizeof(ctx->buf));
	assert(n == -1);
	assert(errno == EAGAIN);

	FD_ZERO(&set);
	FD_SET(afd, &set);

	rv = select(nfds, &set, NULL, NULL, NULL);
	assert(rv == 1);

	for (i = 0; i < nfds; ++i) {
		bool is_set_expected = i == afd;
		bool is_set = FD_ISSET(i, &set);

		assert(is_set_expected == is_set);
	}

	n = read(afd, &ctx->buf[0], sizeof(ctx->buf));
	assert(n == (ssize_t) sizeof(msg));
	assert(memcmp(&msg[0], &ctx->buf[0], sizeof(msg)) == 0);
}

static void
test_select_write(test_context *ctx)
{
	int afd = ctx->afd;
	int cfd = ctx->cfd;
	int nfds = afd + 1;
	struct fd_set set;
	int rv;
	int i;
	ssize_t n;

	puts("test select write");

	assert(afd >= 0);
	assert(cfd >= 0);

	ctx->rfd = cfd;
	ctx->rbuf = &ctx->buf[0];
	ctx->rn = sizeof(ctx->buf);
	send_events(ctx, EVENT_READ);

	set_non_blocking(afd, 1);

	do {
		errno = 0;
		n = write(afd, &ctx->buf[0], sizeof(ctx->buf));
		if (n == -1) {
			assert(errno == EAGAIN);
		}
	} while (n > 0);

	FD_ZERO(&set);
	FD_SET(afd, &set);

	rv = select(nfds, NULL, &set, NULL, NULL);
	assert(rv == 1);

	for (i = 0; i < nfds; ++i) {
		bool is_set_expected = i == afd;
		bool is_set = FD_ISSET(i, &set);

		assert(is_set_expected == is_set);
	}

	n = write(afd, &ctx->buf[0], 1);
	assert(n == 1);
}

static void
test_select_close(test_context *ctx)
{
	int cfd = ctx->cfd;
	int nfds = cfd + 1;
	struct fd_set set;
	int rv;

	puts("test select close");

	assert(ctx->cfd >= 0);

	send_events(ctx, EVENT_CLOSE);

	assert(ctx->cfd >= 0);

	FD_ZERO(&set);
	FD_SET(cfd, &set);

	errno = 0;
	rv = select(nfds, NULL, NULL, &set, NULL);
	assert(rv == -1);
	assert(errno == EBADF);

	assert(ctx->cfd == -1);
}

static void
test_pselect_sigmask(void)
{
	int rv;
	sigset_t set;

	puts("test pselect sigmask");

	sigemptyset(&set);
	errno = 0;
	rv = pselect(0, NULL, NULL, NULL, NULL, &set);
	assert(rv == -1);
	assert(errno == ENOSYS);
}

static void
test_pselect_timeout(test_context *ctx)
{
	struct timespec timeout = {
		.tv_sec = 0,
		.tv_nsec = 100000000
	};
	int fd = ctx->lfd;
	int nfds = fd + 1;
	struct fd_set set;
	int rv;
	int i;

	puts("test pselect timeout");

	set_non_blocking(ctx->lfd, 0);

	FD_ZERO(&set);
	FD_SET(fd, &set);

	rv = pselect(nfds, &set, NULL, NULL, &timeout, NULL);
	assert(rv == 0);

	for (i = 0; i < nfds; ++i) {
		assert(!FD_ISSET(i, &set));
	}

	rv = pselect(nfds, NULL, &set, NULL, &timeout, NULL);
	assert(rv == 0);

	for (i = 0; i < nfds; ++i) {
		assert(!FD_ISSET(i, &set));
	}

	rv = pselect(nfds, NULL, NULL, &set, &timeout, NULL);
	assert(rv == 0);

	for (i = 0; i < nfds; ++i) {
		assert(!FD_ISSET(i, &set));
	}
}

static void
test_poll_timeout(test_context *ctx)
{
	static const short events[] = {
		POLLIN,
		POLLPRI,
		POLLOUT,
		POLLRDNORM,
		POLLWRNORM,
		POLLRDBAND,
		POLLWRBAND
	};

	int timeout = 100;
	struct pollfd pfd = {
		.fd = ctx->lfd
	};
	size_t i;

	puts("test poll timeout");

	for (i = 0; i < nitems(events); ++i) {
		int rv;

		pfd.events = events[i];
		pfd.revents = 0;

		rv = poll(&pfd, 1, timeout);
		assert(rv == 0);
	}
}

static void
test_poll_connect(test_context *ctx)
{
	int lfd = ctx->lfd;
	int afd = ctx->afd;
	struct pollfd pfd = {
		.fd = lfd,
		.events = POLLIN
	};
	int timeout = -1;
	int rv;

	puts("test poll connect");

	if (afd >= 0) {
		ctx->afd = -1;

		rv = close(afd);
		assert(rv == 0);
	}

	send_events(ctx, EVENT_CONNECT);

	set_non_blocking(lfd, 1);

	errno = 0;
	afd = accept(lfd, NULL, NULL);
	assert(afd == -1);
	assert(errno == EAGAIN);

	set_non_blocking(lfd, 0);

	rv = poll(&pfd, 1, timeout);
	assert(rv == 1);
	assert(pfd.revents == POLLIN);

	afd = accept(lfd, NULL, NULL);
	assert(afd >= 0);

	ctx->afd = afd;
}

static void
test_poll_read(test_context *ctx)
{
	int afd = ctx->afd;
	int cfd = ctx->cfd;
	struct pollfd pfd = {
		.fd = afd,
		.events = POLLIN
	};
	int timeout = -1;
	int rv;
	ssize_t n;

	puts("test poll read");

	assert(afd >= 0);
	assert(cfd >= 0);

	ctx->wfd = cfd;
	ctx->wbuf = &msg[0];
	ctx->wn = sizeof(msg);
	send_events(ctx, EVENT_WRITE);

	set_non_blocking(afd, 1);

	errno = 0;
	n = read(afd, &ctx->buf[0], sizeof(ctx->buf));
	assert(n == -1);
	assert(errno == EAGAIN);

	rv = poll(&pfd, 1, timeout);
	assert(rv == 1);
	assert(pfd.revents == POLLIN);

	n = read(afd, &ctx->buf[0], sizeof(ctx->buf));
	assert(n == (ssize_t) sizeof(msg));
	assert(memcmp(&msg[0], &ctx->buf[0], sizeof(msg)) == 0);
}

static void
test_poll_write(test_context *ctx)
{
	int afd = ctx->afd;
	int cfd = ctx->cfd;
	struct pollfd pfd = {
		.fd = afd,
		.events = POLLOUT
	};
	int timeout = -1;
	int rv;
	ssize_t n;

	puts("test poll write");

	assert(afd >= 0);
	assert(cfd >= 0);

	ctx->rfd = cfd;
	ctx->rbuf = &ctx->buf[0];
	ctx->rn = sizeof(ctx->buf);
	send_events(ctx, EVENT_READ);

	set_non_blocking(afd, 1);

	do {
		errno = 0;
		n = write(afd, &ctx->buf[0], sizeof(ctx->buf));
		if (n == -1) {
			assert(errno == EAGAIN);
		}
	} while (n > 0);

	rv = poll(&pfd, 1, timeout);
	assert(rv == 1);
	assert(pfd.revents == POLLOUT);

	n = write(afd, &ctx->buf[0], 1);
	assert(n == 1);
}

static void
test_poll_close(test_context *ctx)
{
	int cfd = ctx->cfd;
	struct pollfd pfd = {
		.fd = cfd,
		.events = POLLRDBAND
	};
	int timeout = -1;
	int rv;

	puts("test poll close");

	assert(ctx->cfd >= 0);

	send_events(ctx, EVENT_CLOSE);

	assert(ctx->cfd >= 0);

	rv = poll(&pfd, 1, timeout);
	assert(rv == 1);
	assert(pfd.revents == POLLNVAL);

	assert(ctx->cfd == -1);
}

static void
test_kqueue_timer(bool do_resource_check)
{
	rtems_resource_snapshot snapshot;
	int kq;
	int rv;
	struct kevent change;
	int i;

	puts("test kqueue timer");

	if (do_resource_check) {
		rtems_resource_snapshot_take(&snapshot);
	}

	kq = kqueue();
	assert(kq >= 0);

	EV_SET(&change, 0xbeef, EVFILT_TIMER, EV_ADD | EV_ENABLE, 0, 100,
	    TEST_UDATA);

	rv = kevent(kq, &change, 1, NULL, 0, NULL);
	assert(rv == 0);

	for (i = 0; i < 5; ++i) {
		struct kevent event;

		rv = kevent(kq, NULL, 0, &event, 1, NULL);
		assert(rv == 1);
		assert(event.ident == 0xbeef);
		assert(event.filter == EVFILT_TIMER);
		assert(event.flags == EV_CLEAR);
		assert(event.fflags == 0);
		assert(event.data == 1);
		assert(event.udata == TEST_UDATA);
	}

	rv = close(kq);
	assert(rv == 0);

	if (do_resource_check) {
		assert(rtems_resource_snapshot_check(&snapshot));
	}
}

static void
test_kqueue_connect(test_context *ctx)
{
	int lfd = ctx->lfd;
	int afd = ctx->afd;
	int kq;
	struct kevent change;
	struct kevent event;
	const struct timespec *timeout = NULL;
	int rv;

	puts("test kqueue connect");

	if (afd >= 0) {
		ctx->afd = -1;

		rv = close(afd);
		assert(rv == 0);
	}

	kq = kqueue();
	assert(kq >= 0);

	EV_SET(&change, lfd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0,
	    TEST_UDATA);

	rv = kevent(kq, &change, 1, NULL, 0, timeout);
	assert(rv == 0);

	send_events(ctx, EVENT_CONNECT);

	set_non_blocking(lfd, 1);

	errno = 0;
	afd = accept(lfd, NULL, NULL);
	assert(afd == -1);
	assert(errno == EAGAIN);

	set_non_blocking(lfd, 0);

	rv = kevent(kq, NULL, 0, &event, 1, timeout);
	assert(rv == 1);
	assert(event.ident == lfd);
	assert(event.filter == EVFILT_READ);
	assert(event.flags == 0);
	assert(event.fflags == 0);
	assert(event.data == 1);
	assert(event.udata == TEST_UDATA);

	afd = accept(lfd, NULL, NULL);
	assert(afd >= 0);

	ctx->afd = afd;

	rv = close(kq);
	assert(rv == 0);
}

static void
test_kqueue_read(test_context *ctx)
{
	int afd = ctx->afd;
	int cfd = ctx->cfd;
	int kq;
	struct kevent change;
	struct kevent event;
	const struct timespec *timeout = NULL;
	int rv;
	ssize_t n;

	puts("test kqueue read");

	assert(afd >= 0);
	assert(cfd >= 0);

	kq = kqueue();
	assert(kq >= 0);

	EV_SET(&change, afd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0,
	    TEST_UDATA);

	rv = kevent(kq, &change, 1, NULL, 0, timeout);
	assert(rv == 0);

	ctx->wfd = cfd;
	ctx->wbuf = &msg[0];
	ctx->wn = sizeof(msg);
	send_events(ctx, EVENT_WRITE);

	set_non_blocking(afd, 1);

	errno = 0;
	n = read(afd, &ctx->buf[0], sizeof(ctx->buf));
	assert(n == -1);
	assert(errno == EAGAIN);

	rv = kevent(kq, NULL, 0, &event, 1, timeout);
	assert(rv == 1);
	assert(event.ident == afd);
	assert(event.filter == EVFILT_READ);
	assert(event.flags == 0);
	assert(event.fflags == 0);
	assert(event.data == sizeof(msg));
	assert(event.udata == TEST_UDATA);

	n = read(afd, &ctx->buf[0], sizeof(ctx->buf));
	assert(n == (ssize_t) sizeof(msg));
	assert(memcmp(&msg[0], &ctx->buf[0], sizeof(msg)) == 0);

	rv = close(kq);
	assert(rv == 0);
}

static void
test_kqueue_write(test_context *ctx)
{
	int afd = ctx->afd;
	int cfd = ctx->cfd;
	int kq;
	struct kevent change;
	struct kevent event;
	const struct timespec *timeout = NULL;
	int rv;
	ssize_t n;

	puts("test kqueue write");

	assert(afd >= 0);
	assert(cfd >= 0);

	kq = kqueue();
	assert(kq >= 0);

	EV_SET(&change, afd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0,
	    TEST_UDATA);

	rv = kevent(kq, &change, 1, NULL, 0, timeout);
	assert(rv == 0);

	ctx->rfd = cfd;
	ctx->rbuf = &ctx->buf[0];
	ctx->rn = sizeof(ctx->buf);
	send_events(ctx, EVENT_READ);

	set_non_blocking(afd, 1);

	do {
		errno = 0;
		n = write(afd, &ctx->buf[0], sizeof(ctx->buf));
		if (n == -1) {
			assert(errno == EAGAIN);
		}
	} while (n > 0);

	rv = kevent(kq, NULL, 0, &event, 1, timeout);
	assert(rv == 1);
	assert(event.ident == afd);
	assert(event.filter == EVFILT_WRITE);
	assert(event.flags == 0);
	assert(event.fflags == 0);
	assert(event.data == 20428);
	assert(event.udata == TEST_UDATA);

	n = write(afd, &ctx->buf[0], 1);
	assert(n == 1);

	rv = close(kq);
	assert(rv == 0);
}

static void
test_kqueue_close(test_context *ctx)
{
	int cfd = ctx->cfd;
	int kq;
	struct kevent change;
	struct kevent event;
	const struct timespec *timeout = NULL;
	int rv;
	ssize_t n;
	mode_t canrecv = S_IRUSR | S_IRGRP | S_IROTH;
	mode_t cansend = S_IWUSR | S_IWGRP | S_IWOTH;
	struct stat st;

	puts("test kqueue close");

	assert(ctx->cfd >= 0);

	kq = kqueue();
	assert(kq >= 0);

	EV_SET(&change, cfd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0,
	    TEST_UDATA);

	rv = kevent(kq, &change, 1, NULL, 0, timeout);
	assert(rv == 0);

	set_non_blocking(cfd, 1);

	do {
		errno = 0;
		n = read(cfd, &ctx->buf[0], sizeof(ctx->buf));
		if (n == -1) {
			assert(errno == EAGAIN);
		}
	} while (n > 0);

	/*
	 * It is not allowed to close file descriptors still in use by
	 * kevent().  On FreeBSD the file descriptor reference counting would
	 * prevent this also.
	 */
	send_events(ctx, EVENT_SHUTDOWN);

	rv = fstat(cfd, &st);
	assert(rv == 0);
	assert(st.st_mode == (S_IFSOCK | canrecv | cansend));

	rv = kevent(kq, NULL, 0, &event, 1, timeout);
	assert(rv == 1);
	assert(event.ident == cfd);
	assert(event.filter == EVFILT_READ);
	assert(event.flags == EV_EOF);
	assert(event.fflags == 0);
	assert(event.data == 0);
	assert(event.udata == TEST_UDATA);

	/*
	 * The master task wakes up during the shutdown() operation.  So we do
	 * not see the full shutdown.
	 */
	rv = fstat(cfd, &st);
	assert(rv == 0);
	assert(st.st_mode == (S_IFSOCK | cansend));

	rv = close(cfd);
	assert(rv == 0);

	rv = close(kq);
	assert(rv == 0);
}

static void
test_kqueue_user(test_context *ctx)
{
	int kq;
	uintptr_t ident;
	u_int flag;
	struct kevent change;
	struct kevent trigger;
	struct kevent event;
	const struct timespec *timeout = NULL;
	int rv;

	puts("test kqueue user");

	ident = 0xabc;
	flag = 0x1;

	kq = kqueue();
	assert(kq >= 0);

	EV_SET(&change, ident, EVFILT_USER, EV_ADD | EV_ENABLE, NOTE_FFNOP, 0,
	    TEST_UDATA);

	rv = kevent(kq, &change, 1, NULL, 0, timeout);
	assert(rv == 0);

	ident = 0xabc;
	EV_SET(&trigger, ident, EVFILT_USER, 0, NOTE_TRIGGER | NOTE_FFCOPY |
	    flag, 0, TEST_UDATA);

	rv = kevent(kq, &trigger, 1, NULL, 0, timeout);
	assert(rv == 0);

	memset(&event, 0, sizeof(event));
	rv = kevent(kq, NULL, 0, &event, 1, timeout);
	assert(rv == 1);
	assert(event.ident == ident);
	assert(event.filter == EVFILT_USER);
	assert(event.flags == 0);
	assert(event.fflags == flag);
	assert(event.data == 0);
	assert(event.udata == TEST_UDATA);

	rv = close(kq);
	assert(rv == 0);
}

static void
test_main(void)
{
	test_context *ctx = &test_instance;

	setup_lo0();
	set_self_prio(PRIO_MASTER);
	start_worker(ctx);
	create_server_socket(ctx);
	create_client_addr(ctx);

	test_select_timeout(ctx);
	test_select_connect(ctx);
	test_select_read(ctx);
	test_select_write(ctx);
	test_select_close(ctx);

	test_pselect_sigmask();
	test_pselect_timeout(ctx);

	test_poll_timeout(ctx);
	test_poll_connect(ctx);
	test_poll_read(ctx);
	test_poll_write(ctx);
	test_poll_close(ctx);

	test_kqueue_timer(false);
	test_kqueue_timer(true);
	test_kqueue_connect(ctx);
	test_kqueue_read(ctx);
	test_kqueue_write(ctx);
	test_kqueue_close(ctx);
	test_kqueue_user(ctx);

	exit(0);
}

#include <rtems/bsd/test/default-init.h>
