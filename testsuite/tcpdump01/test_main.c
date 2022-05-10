/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief Tests the tcpdump command.
 */

/*
 * Copyright (C) 2022 embedded brains GmbH
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
#include <sys/types.h>
#include <sys/socket.h>
#include <vm/uma.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <err.h>

#include <assert.h>
#include <ck_epoch.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <rtems.h>
#include <rtems/bsd/bsd.h>
#include <rtems/console.h>
#include <rtems/libcsupport.h>

#include <machine/rtems-bsd-commands.h>

#define TEST_NAME "LIBBSD TCPDUMP 1"

typedef struct {
	rtems_id main_id;
	int sp[2];
} test_context;

static test_context test_instance;

static void
epoch_cleanup(void)
{
	rtems_status_code sc;

	sc = rtems_task_wake_after(CK_EPOCH_LENGTH);
	assert(sc == RTEMS_SUCCESSFUL);
}

static void
init_addr(struct sockaddr_in *addr)
{
	int ok;

	memset(addr, 0, sizeof(*addr));
	addr->sin_family = AF_INET;
	addr->sin_port = htons(1234);
	ok = inet_aton("127.0.0.1", &addr->sin_addr);
	assert(ok != 0);
}

static rtems_id
start_task(rtems_task_entry entry, void *arg)
{
	rtems_task_priority prio;
	rtems_id id;
	rtems_status_code sc;

	sc = rtems_task_set_priority(RTEMS_SELF, RTEMS_CURRENT_PRIORITY, &prio);
	assert(sc == RTEMS_SUCCESSFUL);

	sc = rtems_task_create(rtems_build_name('T', 'E', 'S', 'T' ), prio - 1,
	    32 * 1024, RTEMS_DEFAULT_MODES, RTEMS_DEFAULT_ATTRIBUTES, &id);
	assert(sc == RTEMS_SUCCESSFUL);

	sc = rtems_task_start(id, entry, (rtems_task_argument)arg);
	assert(sc == RTEMS_SUCCESSFUL);

	return id;
}

static void
test_tcpdump_help(void)
{
	char *argv[] = {
		"tcpdump",
		"-h",
		NULL
	};
	rtems_resource_snapshot snapshot;
	int exit_code;

	rtems_resource_snapshot_take(&snapshot);

	exit_code = rtems_bsd_command_tcpdump(RTEMS_BSD_ARGC(argv), argv);
	assert(exit_code == 0);

	assert(rtems_resource_snapshot_check(&snapshot));
}

static const char raw_packets_file[] = "packets.bin";

static void
run_tcpdump_write_raw_packets(rtems_task_argument arg)
{
	test_context *ctx;
	FILE *file;
	FILE *saved_stdin;
	FILE *saved_stdout;
	FILE *saved_stderr;
	int rv;
	char *argv[] = {
		"tcpdump",
		"-n",
		"-i",
		"lo0",
		"-w",
		RTEMS_DECONST(char *, raw_packets_file),
		NULL
	};
	int exit_code;
	rtems_status_code sc;

	ctx = (test_context *)arg;

	saved_stdin = stdin;
	saved_stdout = stdout;
	saved_stderr = stderr;

	file = fdopen(ctx->sp[1], "r+");
	assert(file != NULL);

	stdin = file;
	stdout = file;
	stderr = file;

	exit_code = rtems_bsd_command_tcpdump(RTEMS_BSD_ARGC(argv), argv);
	assert(exit_code == 0);

	stdin = saved_stdin;
	stdout = saved_stdout;
	stderr = saved_stderr;

	rv = fclose(file);
	assert(rv == 0);

	sc = rtems_event_transient_send(ctx->main_id);
	assert(sc == RTEMS_SUCCESSFUL);

	rtems_task_exit();
}

static void
test_tcpdump_write_raw_packets(test_context *ctx)
{
	char *argv[] = {
		"tcpdump",
		"-n",
		"-r",
		RTEMS_DECONST(char *, raw_packets_file),
		NULL
	};
	rtems_resource_snapshot snapshot;
	int in;
	int out;
	int rv;
	char c;
	ssize_t n;
	struct sockaddr_in addr;
	socklen_t addr_len;
	rtems_status_code sc;
	int exit_code;

	rtems_resource_snapshot_take(&snapshot);

	rv = socketpair(PF_UNIX, SOCK_STREAM, 0, ctx->sp);
	assert(rv == 0);

	start_task(run_tcpdump_write_raw_packets, ctx);

	init_addr(&addr);

	in = socket(PF_INET, SOCK_DGRAM, 0);
	assert(out >= 0);

	rv = bind(in, (const struct sockaddr *) &addr, sizeof(addr));
	assert(rv == 0);

	out = socket(PF_INET, SOCK_DGRAM, 0);
	assert(out >= 0);

	c = 'x';
	n = sendto(out, &c, sizeof(c), 0,
	    (const struct sockaddr *) &addr, sizeof(addr));
	assert(n == 1);

	c = 'y';
	addr_len = sizeof(addr);
	n = recvfrom(in, &c, sizeof(c), 0,
	    (struct sockaddr *) &addr, &addr_len);
	assert(n == 1);
	assert(c == 'x');

	/* The tcpdump pcap read timeout is 1000ms */
	sc = rtems_task_wake_after(rtems_clock_get_ticks_per_second());
	assert(sc == RTEMS_SUCCESSFUL);

	c = 'q';
	n = write(ctx->sp[0], &c, sizeof(c));
	assert(n == 1);

	rv = close(out);
	assert(rv == 0);

	rv = close(in);
	assert(rv == 0);

	sc = rtems_event_transient_receive(RTEMS_WAIT, RTEMS_NO_TIMEOUT);
	assert(sc == RTEMS_SUCCESSFUL);

	rv = close(ctx->sp[0]);
	assert(rv == 0);

	exit_code = rtems_bsd_command_tcpdump(RTEMS_BSD_ARGC(argv), argv);
	assert(exit_code == 0);

	rv = unlink(raw_packets_file);
	assert(rv == 0);

	epoch_cleanup();
	assert(rtems_resource_snapshot_check(&snapshot));
}

static void
test_main(void)
{
	test_context *ctx;
	FILE *file;
	int rv;

	ctx = &test_instance;
	ctx->main_id = rtems_task_self();

	/* Fill dynamic file pool in Newlib */
	file = fopen(CONSOLE_DEVICE_NAME, "r+");
	assert(file != NULL);
	rv = fclose(file);
	assert(rv == 0);

	/*
	 * Stop interferences of uma_timeout() which may need some dynamic
	 * memory.
	 */
	rtems_uma_drain_timeout();

	rtems_bsd_ifconfig_lo0();
	epoch_cleanup();

	test_tcpdump_help();
	test_tcpdump_write_raw_packets(ctx);

	exit(0);
}

#include <rtems/bsd/test/default-init.h>
