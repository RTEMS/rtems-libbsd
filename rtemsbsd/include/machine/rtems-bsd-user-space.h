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
#include <sys/types.h>

#include <stdio.h>

#define _SC_CPUSET_SIZE 122

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

/* expat config */
#define XML_GE 1
#define XML_CONTEXT_BYTES 1024

#define secure_getenv getenv

#define CRYPTO_memcmp memcmp

/* Here until _termios in newlib is updated */
#define IUTF8   0x00004000
/* Here until netdb in newlib is updated */
#define EAI_ADDRFAMILY 1
/* Here until socket in newlib is updated */
#define AF_NETLINK 38
/* From sys/sys/ssp.h */
#define __ssp_real(fun)             fun

#define WITHOUT_NETLINK 1

/* From sys/sys/_types.h */
/* Types for sys/acl.h */
typedef __uint32_t	__acl_tag_t;
typedef __uint32_t	__acl_perm_t;
typedef __uint16_t	__acl_entry_type_t;
typedef __uint16_t	__acl_flag_t;
typedef __uint32_t	__acl_type_t;
typedef __uint32_t	*__acl_permset_t;
typedef __uint16_t	*__acl_flagset_t;

typedef   __int64_t       kssize_t;
typedef   __uint64_t      kpaddr_t;

typedef register_t syscallarg_t;
typedef max_align_t  __max_align_t;

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
