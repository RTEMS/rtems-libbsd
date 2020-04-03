/**
 * @file
 *
 * It creates a character device (cdev) and tests the basic i/o functions,
 * e.g. open(), read(), write(), ioctl(), etc.
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

#include <sys/types.h>
#include <sys/event.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/uio.h>
#include <sys/ioctl.h>

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "test_cdev01.h"

#define TEST_NAME "LIBBSD CDEV 1"

static void test_cdev(const char *path)
{
	const struct timespec *timeout = NULL;
	const char *name;

	/* Remove leading "/dev/" and use the rest as a name. */
	name = path + sizeof("/dev/") - 1;

	printf("Test creating a cdev named \"%s\" at \"%s\".\n", name, path);

	test_state state = TEST_NEW;
	int rv = 0;
	int fd = 0;
	char buf[1];
	ssize_t n = 0;
	off_t off = 0;
	struct iovec iov = {
	    .iov_base = &buf [0],
	    .iov_len = (int) sizeof(buf)
	};
	struct pollfd fds[1];
	int kq;
	struct kevent change;

	test_make_dev(&state, name);

	fd = open(path, O_RDWR);
	assert(fd >= 0);
	fds[0].fd = fd;
	fds[0].events = POLLOUT | POLLIN;

	n = read(fd, buf, sizeof(buf));
	assert(n == 0);

	n = write(fd, buf, sizeof(buf));
	assert(n == 0);

	rv = ioctl(fd, TEST_IOCTL_CMD);
	assert(rv == 0);

	off = lseek(fd, off, SEEK_SET);
	assert(off == 0);

	rv = ftruncate(fd, 0);
	assert(rv == -1);
	assert(errno == EINVAL);

	rv = fsync(fd);
	assert(rv == -1);
	assert(errno == EINVAL);

	rv = fdatasync(fd);
	assert(rv == -1);
	assert(errno == EINVAL);

	rv = fcntl(fd, F_GETFD);
	assert(rv >= 0);

	rv = readv(fd, &iov, 1);
	assert(rv == 0);

	rv = writev(fd, &iov, 1);
	assert(rv == 0);

	rv = poll(fds, sizeof(fds)/sizeof(fds[0]), 500);
	assert(rv > 0);

	kq = kqueue();
	assert(kq >= 0);

	EV_SET(&change, fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, TEST_UDATA);

	rv = kevent(kq, &change, 1, NULL, 0, timeout);
	assert(rv == -1);
	assert(errno == TEST_KQ_ERRNO);

	rv = close(fd);
	assert(rv == 0);

	rv = unlink(path);
	assert(rv == 0);

	assert(state == TEST_CLOSED);
}

static void
test_main(void)
{
	test_cdev("/dev/test");
	test_cdev("/dev/some/sub/dir/somedev");

	exit(0);
}

#include <rtems/bsd/test/default-init.h>
