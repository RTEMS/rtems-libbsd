/**
 * @file
 *
 * @ingroup rtems_bsd_machine
 *
 * @brief TODO.
 */

/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
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

#ifndef _RTEMS_BSD_MACHINE_RTEMS_BSD_USER_SPACE_H_
#define _RTEMS_BSD_MACHINE_RTEMS_BSD_USER_SPACE_H_

#define __FreeBSD__ 1
#define _WANT_FREEBSD_BITSET

#include <rtems/bsd/local/opt_inet6.h>
#include <machine/rtems-bsd-version.h>
#include <sys/cdefs.h>

#include <stdio.h>

#define O_CLOEXEC 0

#define O_DIRECTORY 0

/* FIXME: Add lock capabilities to RTEMS file system */
#define O_EXLOCK 0
#define O_SHLOCK 0

/* FIXME: Add no symlink follow capabilities to RTEMS file system */
#define O_NOFOLLOW 0

#define _PATH_VARRUN "/var/run"

#define _POSIX2_LINE_MAX 128

#ifndef CLOCK_MONOTONIC_FAST
#define	CLOCK_MONOTONIC_FAST CLOCK_MONOTONIC
#endif

#ifndef CLOCK_REALTIME_FAST
#define	CLOCK_REALTIME_FAST CLOCK_REALTIME
#endif

#ifndef CLOCK_UPTIME
#define	CLOCK_UPTIME CLOCK_MONOTONIC
#endif

#define	SIZE_T_MAX SIZE_MAX

#define	__libc_sigprocmask(a, b, c) sigprocmask(a, b, c)

__BEGIN_DECLS

/* FIXME: Add to Newlib */
long long strtonum(const char *numstr, long long minval, long long maxval,
    const char **errstrp);

/* FIXME: Add to Newlib */
char *fgetln(FILE *fp, size_t *lenp);

/* FIXME: Add to Newlib */
int	feature_present(const char *);

/* FIXME: Remove after Newlib update */
#ifndef timespecclear
#define	timespecclear(tvp)	((tvp)->tv_sec = (tvp)->tv_nsec = 0)
#endif
#ifndef timespecisset
#define	timespecisset(tvp)	((tvp)->tv_sec || (tvp)->tv_nsec)
#endif
#ifndef timespeccmp
#define	timespeccmp(tvp, uvp, cmp)					\
	(((tvp)->tv_sec == (uvp)->tv_sec) ?				\
	    ((tvp)->tv_nsec cmp (uvp)->tv_nsec) :			\
	    ((tvp)->tv_sec cmp (uvp)->tv_sec))
#endif
#ifndef timespecadd
#define	timespecadd(tsp, usp, vsp)					\
	do {								\
		(vsp)->tv_sec = (tsp)->tv_sec + (usp)->tv_sec;		\
		(vsp)->tv_nsec = (tsp)->tv_nsec + (usp)->tv_nsec;	\
		if ((vsp)->tv_nsec >= 1000000000L) {			\
			(vsp)->tv_sec++;				\
			(vsp)->tv_nsec -= 1000000000L;			\
		}							\
	} while (0)
#endif
#ifndef timespecsub
#define	timespecsub(tsp, usp, vsp)					\
	do {								\
		(vsp)->tv_sec = (tsp)->tv_sec - (usp)->tv_sec;		\
		(vsp)->tv_nsec = (tsp)->tv_nsec - (usp)->tv_nsec;	\
		if ((vsp)->tv_nsec < 0) {				\
			(vsp)->tv_sec--;				\
			(vsp)->tv_nsec += 1000000000L;			\
		}							\
	} while (0)
#endif

__END_DECLS

#endif /* _RTEMS_BSD_MACHINE_RTEMS_BSD_USER_SPACE_H_ */
