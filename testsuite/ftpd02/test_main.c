/*
 * Copyright (c) 2011, 2016 embedded brains GmbH.  All rights reserved.
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

#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <stdlib.h>

#include <rtems.h>
#include <rtems/ftpd.h>
#include <rtems/ftpfs.h>
#include <rtems/shell.h>
#include <rtems/console.h>

#include <machine/rtems-bsd-commands.h>

#define TEST_NAME "LIBBSD FTPD 2"

#define FTP_WORKER_TASK_COUNT 2

#define FTP_WORKER_TASK_EXTRA_STACK (FTP_WORKER_TASK_COUNT * FTPD_STACKSIZE)

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

static const char content[] =
"                      LICENSE INFORMATION\n"
"\n"
"RTEMS is free software; you can redistribute it and/or modify it under\n"
"terms of the GNU General Public License as published by the\n"
"Free Software Foundation; either version 2, or (at your option) any\n"
"later version.  RTEMS is distributed in the hope that it will be useful,\n"
"but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU\n"
"General Public License for more details. You should have received\n"
"a copy of the GNU General Public License along with RTEMS; see\n"
"file COPYING. If not, write to the Free Software Foundation, 675\n"
"Mass Ave, Cambridge, MA 02139, USA.\n"
"\n"
"As a special exception, including RTEMS header files in a file,\n"
"instantiating RTEMS generics or templates, or linking other files\n"
"with RTEMS objects to produce an executable application, does not\n"
"by itself cause the resulting executable application to be covered\n"
"by the GNU General Public License. This exception does not\n"
"however invalidate any other reasons why the executable file might be\n"
"covered by the GNU Public License.\n";

static void
initialize_ftpfs(void)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;
	int rv = 0;
	struct timeval to = {
		.tv_sec = 10,
		.tv_usec = 0
	};
	const char *target = RTEMS_FTPFS_MOUNT_POINT_DEFAULT;

	rv = mount_and_make_target_path(
		NULL,
		target,
		RTEMS_FILESYSTEM_TYPE_FTPFS,
		RTEMS_FILESYSTEM_READ_WRITE,
		NULL
	);
	assert(rv == 0);

	sc = rtems_ftpfs_set_verbose(target, true);
	assert(sc == RTEMS_SUCCESSFUL);

	sc = rtems_ftpfs_set_timeout(target, &to);
	assert(sc == RTEMS_SUCCESSFUL);
}

static void
change_self_priority(void)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;
	rtems_task_priority cur = 0;

	sc = rtems_task_set_priority(RTEMS_SUCCESSFUL, 110, &cur);
	assert(sc == RTEMS_SUCCESSFUL);
}

static void
create_file(const char *path, const void *begin, size_t size)
{
	int rv = 0;
	int fd = open(path, O_WRONLY);
	ssize_t n = 0;

	assert(fd >= 0);

	n = write(fd, begin, size);
	assert(n == (ssize_t) size);

	rv = close(fd);
	assert(rv == 0);
}

static void
copy_file(const char *src_path, const char *dest_path)
{
	int rv = 0;
	int in = open(src_path, O_RDONLY);
	int out = open(dest_path, O_WRONLY);
	ssize_t n_in = 0;
	char buf [64];
	struct stat st_in;
	struct stat st_out;

	memset(&st_in, 0xff, sizeof(st_in));
	memset(&st_out, 0xff, sizeof(st_out));

	assert(in >= 0);
	assert(out >= 0);

	rv = fstat(out, &st_out);
	assert(rv == 0);

	assert(st_out.st_size == 0);

	while ((n_in = read(in, buf, sizeof(buf))) > 0) {
		ssize_t n_out = write(out, buf, (size_t) n_in);
		assert(n_out == n_in);
	}

	rv = fstat(out, &st_out);
	assert(rv == 0);

	rv = fstat(in, &st_in);
	assert(rv == 0);

	assert(st_in.st_size == st_out.st_size);

	rv = close(out);
	assert(rv == 0);

	rv = close(in);
	assert(rv == 0);
}

static void
check_file_size(const char *path, size_t size)
{
	struct stat st;
	int rv = lstat(path, &st);

	assert(rv == 0);
	assert(st.st_size == (off_t) size);
}

static void
check_file(const char *path)
{
	int rv = 0;
	int fd = open(path, O_RDONLY);
	ssize_t n = 0;
	char buf [64];
	const char *current = &content [0];
	size_t done = 0;

	assert(fd >= 0);

	while ((n = read(fd, buf, sizeof(buf))) > 0) {
		done += (size_t) n;
		assert(done <= sizeof(content));
		assert(memcmp(current, buf, (size_t) n) == 0);
		current += (size_t) n;
	}

	assert(done == sizeof(content));

	rv = close(fd);
	assert(rv == 0);
}

static void
test_main(void)
{
	const char file_a[] = "/FTP/127.0.0.1/a.txt";
	const char file_b[] = "/FTP/127.0.0.1/b.txt";
	char *lo0[] = {
		"ifconfig",
		"lo0",
		"inet",
		"127.0.0.1",
		"netmask",
		"255.255.255.0",
		NULL
	};
	int rv;
	int exit_code;

	exit_code = rtems_bsd_command_ifconfig(RTEMS_BSD_ARGC(lo0), lo0);
	assert(exit_code == EXIT_SUCCESS);

	rv = rtems_initialize_ftpd();
	assert(rv == 0);

	initialize_ftpfs();
	change_self_priority();
	create_file(file_a, &content [0], sizeof(content));
	copy_file(file_a, file_b);
	check_file(file_b);
	check_file_size(file_a, sizeof(content));
	check_file_size(file_b, sizeof(content));

	exit(0);
}

#define CONFIGURE_FILESYSTEM_FTPFS

#include <rtems/bsd/test/default-init.h>
