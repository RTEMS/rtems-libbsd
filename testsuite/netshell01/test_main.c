/*
 * COPYRIGHT (c) 2012. On-Line Applications Research Corporation (OAR).
 * All rights reserved.
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

/*
 *  This is the body of the test. It does not do much except ensure
 *  that the target is alive after initializing the TCP/IP stack.
 */

#include <assert.h>
#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <rtems/shell.h>
#include <rtems/termiostypes.h>

#define TEST_NAME "LIBBSD NETSHELL 1"

static void
change_serial_settings(int fd, const struct termios *current, bool icanon)
{
	struct termios term = *current;

	term.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR |
	    ICRNL | IXON);
	term.c_lflag &= ~(ECHO | ECHOE | ECHOK | ECHONL | ECHOPRT | ECHOCTL |
	    ECHOKE | ICANON | ISIG | IEXTEN);
	term.c_cflag &= ~(CSIZE | PARENB);
	term.c_cflag |= CS8;
	term.c_oflag &= ~(OPOST | ONLRET | ONLCR | OCRNL | ONLRET | TABDLY |
	    OLCUC);

	term.c_cc[VMIN] = 0;
	term.c_cc[VTIME] = 10;

	if (icanon) {
		term.c_iflag |= ICRNL;
		term.c_lflag |= ICANON;
	}

	tcsetattr(fd, TCSANOW, &term);
}

static void
do_read_select(int fd)
{
	int nfds = fd + 1;
	struct fd_set read_set;
	struct timeval timeout = {
		.tv_sec = 10,
		.tv_usec = 0
	};
	int rv;

	FD_ZERO(&read_set);
	FD_SET(fd, &read_set);

	rv = select(nfds, &read_set, NULL, NULL, &timeout);
	if (rv == 0) {
		printf("timeout\n");
	} else if (rv > 0) {
		if (FD_ISSET(fd, &read_set)) {
			char buf[512];
			ssize_t n = read(fd, buf, sizeof(buf));
			printf("read returned %zi\n", n);
		}
	} else {
		perror("select failed");
	}
}

static void
do_write_select(int fd)
{
	int nfds = fd + 1;
	struct fd_set write_set;
	struct timeval to = {
		.tv_sec = 0,
		.tv_usec = 1
	};
	struct timeval *timeout = &to;
	char buf[512];
	int rv;
	size_t i;

	memset(buf, 'a', sizeof(buf));

	for (i = 0; i < sizeof(buf); i += 24) {
		buf[i] = '\r';
		buf[i + 1] = '\n';
	}

	for (i = 0; i < 10; ++i) {
		write(fd, buf, sizeof(buf));

		FD_ZERO(&write_set);
		FD_SET(fd, &write_set);
		rv = select(nfds, NULL, &write_set, NULL, timeout);
		if (rv == 0) {
			printf("timeout\n");
		} else {
			printf("write set: %i\n", FD_ISSET(fd, &write_set));
		}

		timeout = NULL;
	}
}

static int
termiosselect_command(int argc, char *argv[])
{
	bool icanon = argc > 1 && strcmp(argv[1], "icanon") == 0;
	int fd = STDIN_FILENO;
	struct termios term;
	int rv = tcgetattr(fd, &term);
	assert(rv == 0);

	change_serial_settings(fd, &term, icanon);
	do_read_select(fd);
	do_write_select(STDOUT_FILENO);
	tcsetattr(fd, TCSANOW, &term);
	return (0);
}

rtems_shell_cmd_t rtems_shell_ARP_Command = {
	.name = "termiosselect",
	.usage = "termiosselect [icanon]",
	.topic = "net",
	.command = termiosselect_command
};

static void
test_main(void)
{
	rtems_shell_env_t env;

	memset(&env, 0, sizeof(env));
	rtems_shell_main_loop(&env);

	exit(0);
}

#define RTEMS_BSD_CONFIG_TERMIOS_KQUEUE_AND_POLL

#include <rtems/bsd/test/default-init.h>
