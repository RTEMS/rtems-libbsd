#include <machine/rtems-bsd-user-space.h>

/*-
 * Copyright (c) 1988, 1993
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

#if 0
#ifndef lint
static char const copyright[] =
"@(#) Copyright (c) 1988, 1993\n\
	The Regents of the University of California.  All rights reserved.\n";
#endif /* not lint */

#ifndef lint
static char sccsid[] = "@(#)hostname.c	8.1 (Berkeley) 5/31/93";
#endif /* not lint */
#endif
#ifdef __rtems__
#define __need_getopt_newlib
#include <getopt.h>
#include <rtems/netcmds-config.h>
#define RTEMS_BSD_PROGRAM_NO_OPEN_WRAP
#define RTEMS_BSD_PROGRAM_NO_SOCKET_WRAP
#define RTEMS_BSD_PROGRAM_NO_CLOSE_WRAP
#define RTEMS_BSD_PROGRAM_NO_FOPEN_WRAP
#define RTEMS_BSD_PROGRAM_NO_FCLOSE_WRAP
#define RTEMS_BSD_PROGRAM_NO_MALLOC_WRAP
#define RTEMS_BSD_PROGRAM_NO_CALLOC_WRAP
#define RTEMS_BSD_PROGRAM_NO_REALLOC_WRAP
#define RTEMS_BSD_PROGRAM_NO_FREE_WRAP
#include <machine/rtems-bsd-program.h>
#include <machine/rtems-bsd-commands.h>
#include <rtems/mdns.h>
#endif /* __rtems__ */
#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <rtems/bsd/sys/param.h>

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void usage(void);

#ifdef __rtems__
static int main(int argc, char *argv[]);

static int hostname_command(int argc, char *argv[])
{
	int exit_code;

	rtems_bsd_program_lock();

	exit_code = rtems_bsd_program_call_main("hostname", main, argc, argv);

	rtems_bsd_program_unlock();

	return exit_code;
}

rtems_shell_cmd_t rtems_shell_HOSTNAME_Command = {
  .name = "hostname",
  .usage = "hostname [-fms] [name-of-host]",
  .topic = "net",
  .command = hostname_command
};
#endif /* __rtems__ */
int
main(int argc, char *argv[])
{
	int ch, sflag, dflag;
	char *p, hostname[MAXHOSTNAMELEN];
#ifdef __rtems__
	struct getopt_data getopt_data;
	memset(&getopt_data, 0, sizeof(getopt_data));
#define optind getopt_data.optind
#define getopt(argc, argv, opt) getopt_r(argc, argv, "+" opt, &getopt_data)
	int mflag = 0;
#endif /* __rtems__ */

	sflag = 0;
	dflag = 0;
#ifndef __rtems__
	while ((ch = getopt(argc, argv, "fsd")) != -1)
#else /* __rtems__ */
	while ((ch = getopt(argc, argv, "fdms")) != -1)
#endif /* __rtems__ */
		switch (ch) {
		case 'f':
			/*
			 * On Linux, "hostname -f" prints FQDN.
			 * BSD "hostname" always prints FQDN by
			 * default, so we accept but ignore -f.
			 */
			break;
		case 's':
			sflag = 1;
			break;
		case 'd':
			dflag = 1;
			break;
#ifdef __rtems__
		case 'm':
			mflag = 1;
			break;
#endif /* __rtems__ */
		case '?':
		default:
			usage();
		}
	argc -= optind;
	argv += optind;

	if (argc > 1 || (sflag && dflag))
		usage();

	if (*argv) {
#ifdef __rtems__
		if (mflag) {
			if (rtems_mdns_sethostname(*argv)) {
				err(1, "rtems_mdns_sethostname");
			}
		} else {
#endif /* __rtems__ */
		if (sethostname(*argv, (int)strlen(*argv)))
			err(1, "sethostname");
#ifdef __rtems__
		}
	} else if (mflag) {
		if (rtems_mdns_gethostname(hostname, sizeof(hostname))) {
			err(1, "rtems_mdns_gethostname");
		}

		(void)printf("%s\n", hostname);
#endif /* __rtems__ */
	} else {
		if (gethostname(hostname, (int)sizeof(hostname)))
			err(1, "gethostname");
		if (sflag) {
			p = strchr(hostname, '.');
			if (p != NULL)
				*p = '\0';
		} else if (dflag) {
			p = strchr(hostname, '.');
			if (p != NULL)
				strcpy(hostname, ++p);
		}
		(void)printf("%s\n", hostname);
	}
	exit(0);
}

static void
usage(void)
{

#ifndef __rtems__
	(void)fprintf(stderr, "usage: hostname [-f] [-s | -d] [name-of-host]\n");
#else /* __rtems__ */
	(void)fprintf(stderr, "usage: hostname [-f] [-m | -s | -d] [name-of-host]\n");
#endif /* __rtems__ */
	exit(1);
}
