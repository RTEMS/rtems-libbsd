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
#include <sys/limits.h>

#include <stdio.h>

/* REVIEW-AFTER-FREEBSD-BASELINE-UPDATE */
#define IFCAP_NOMAP 0x4000000

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

__END_DECLS

#endif /* _RTEMS_BSD_MACHINE_RTEMS_BSD_USER_SPACE_H_ */
