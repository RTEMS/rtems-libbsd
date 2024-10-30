#include <machine/rtems-bsd-user-space.h>

#ifdef __rtems__
#include "rtems-bsd-stty-namespace.h"
#endif /* __rtems__ */
/*-
 * Copyright (c) 1989, 1991, 1993, 1994
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifdef __rtems__
#define __need_getopt_newlib
#include <getopt.h>
#include <machine/rtems-bsd-program.h>
#include <machine/rtems-bsd-commands.h>
#endif /* __rtems__ */
#include <sys/types.h>

#include <ctype.h>
#include <err.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "stty.h"
#include "extern.h"
#ifdef __rtems__
#include "rtems-bsd-stty-stty-data.h"
#endif /* __rtems__ */

#ifdef __rtems__
static int main(int argc, char *argv[]);

RTEMS_LINKER_RWSET(bsd_prog_stty, char);

int
rtems_bsd_command_stty(int argc, char *argv[])
{
  int exit_code;
  void *data_begin;
  size_t data_size;

  data_begin = RTEMS_LINKER_SET_BEGIN(bsd_prog_stty);
  data_size = RTEMS_LINKER_SET_SIZE(bsd_prog_stty);

  rtems_bsd_program_lock();
  exit_code = rtems_bsd_program_call_main_with_data_restore("stty",
      main, argc, argv, data_begin, data_size);
  rtems_bsd_program_unlock();

  return exit_code;
}
#endif /* __rtems__ */
int
main(int argc, char *argv[])
{
	struct info i;
	enum FMT fmt;
	int ch;
	const char *file, *errstr = NULL;
#ifdef __rtems__
	struct getopt_data getopt_data;
	memset(&getopt_data, 0, sizeof(getopt_data));
#define optind getopt_data.optind
#define optarg getopt_data.optarg
#define opterr getopt_data.opterr
#define optopt getopt_data.optopt
#define getopt(argc, argv, opt) getopt_r(argc, argv, "+" opt, &getopt_data)
#endif /* __rtems__ */

	fmt = NOTSET;
	i.fd = STDIN_FILENO;
	file = "stdin";

	opterr = 0;
	while (optind < argc &&
#ifndef __rtems__
	    strspn(argv[optind], "-aefg") == strlen(argv[optind]) &&
#else /* __rtems__ */
	    strspn(argv[optind == 0 ? 1 : optind], "-aefg") == strlen(argv[optind == 0 ? 1 : optind]) &&
#endif /* __rtems__ */
	    (ch = getopt(argc, argv, "aef:g")) != -1)
		switch(ch) {
		case 'a':		/* undocumented: POSIX compatibility */
			fmt = POSIX;
			break;
		case 'e':
			fmt = BSD;
			break;
		case 'f':
			if ((i.fd = open(optarg, O_RDONLY | O_NONBLOCK)) < 0)
				err(1, "%s", optarg);
			file = optarg;
			break;
		case 'g':
			fmt = GFLAG;
			break;
		case '?':
		default:
			goto args;
		}

args:	argc -= optind;
	argv += optind;

	if (tcgetattr(i.fd, &i.t) < 0)
		errx(1, "%s isn't a terminal", file);
	if (ioctl(i.fd, TIOCGETD, &i.ldisc) < 0)
		err(1, "TIOCGETD");
	if (ioctl(i.fd, TIOCGWINSZ, &i.win) < 0)
		warn("TIOCGWINSZ");

	checkredirect();			/* conversion aid */

	switch(fmt) {
	case NOTSET:
		if (*argv)
			break;
		/* FALLTHROUGH */
	case BSD:
	case POSIX:
		print(&i.t, &i.win, i.ldisc, fmt);
		break;
	case GFLAG:
		gprint(&i.t, &i.win, i.ldisc);
		break;
	}

	for (i.set = i.wset = 0; *argv; ++argv) {
		if (ksearch(&argv, &i))
			continue;

		if (csearch(&argv, &i))
			continue;

		if (msearch(&argv, &i))
			continue;

		if (isdigit(**argv)) {
			speed_t speed;

			speed = strtonum(*argv, 0, UINT_MAX, &errstr);
			if (errstr)
				err(1, "speed");
			cfsetospeed(&i.t, speed);
			cfsetispeed(&i.t, speed);
			i.set = 1;
			continue;
		}

		if (!strncmp(*argv, "gfmt1", sizeof("gfmt1") - 1)) {
			gread(&i.t, *argv + sizeof("gfmt1") - 1);
			i.set = 1;
			continue;
		}

		warnx("illegal option -- %s", *argv);
		usage();
	}

	if (i.set && tcsetattr(i.fd, 0, &i.t) < 0)
		err(1, "tcsetattr");
	if (i.wset && ioctl(i.fd, TIOCSWINSZ, &i.win) < 0)
		warn("TIOCSWINSZ");
	exit(0);
}

void
usage(void)
{

	(void)fprintf(stderr,
	    "usage: stty [-a | -e | -g] [-f file] [arguments]\n");
	exit (1);
}
