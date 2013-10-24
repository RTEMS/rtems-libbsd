/*-
 * Copyright (c) 2002 Mike Barcroft <mike@FreeBSD.org>
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
 *
 * $FreeBSD$
 */

#ifndef _RTEMS_BSD_SYS__TYPES_H_
#define _RTEMS_BSD_SYS__TYPES_H_

#include <sys/cdefs.h>
#ifndef __rtems__
#include <machine/_types.h>

/*
 * Standard type definitions.
 */
typedef	__uint32_t	__blksize_t;	/* file block size */
typedef	__int64_t	__blkcnt_t;	/* file block count */
typedef	__int32_t	__clockid_t;	/* clock_gettime()... */
typedef	__uint32_t	__fflags_t;	/* file flags */
typedef	__uint64_t	__fsblkcnt_t;
typedef	__uint64_t	__fsfilcnt_t;
typedef	__uint32_t	__gid_t;
typedef	__int64_t	__id_t;		/* can hold a gid_t, pid_t, or uid_t */
typedef	__uint32_t	__ino_t;	/* inode number */
typedef	long		__key_t;	/* IPC key (for Sys V IPC) */
typedef	__int32_t	__lwpid_t;	/* Thread ID (a.k.a. LWP) */
typedef	__uint16_t	__mode_t;	/* permissions */
typedef	int		__accmode_t;	/* access permissions */
typedef	int		__nl_item;
typedef	__uint16_t	__nlink_t;	/* link count */
typedef	__int64_t	__off_t;	/* file offset */
typedef	__int32_t	__pid_t;	/* process [group] */
typedef	__int64_t	__rlim_t;	/* resource limit - intentionally */
					/* signed, because of legacy code */
					/* that uses -1 for RLIM_INFINITY */
typedef	__uint8_t	__sa_family_t;
typedef	__uint32_t	__socklen_t;
typedef	long		__suseconds_t;	/* microseconds (signed) */
typedef	struct __timer	*__timer_t;	/* timer_gettime()... */
typedef	struct __mq	*__mqd_t;	/* mq_open()... */
typedef	__uint32_t	__uid_t;
typedef	unsigned int	__useconds_t;	/* microseconds (unsigned) */
typedef	int		__cpuwhich_t;	/* which parameter for cpuset. */
typedef	int		__cpulevel_t;	/* level parameter for cpuset. */
typedef int		__cpusetid_t;	/* cpuset identifier. */

/*
 * Unusual type definitions.
 */
/*
 * rune_t is declared to be an ``int'' instead of the more natural
 * ``unsigned long'' or ``long''.  Two things are happening here.  It is not
 * unsigned so that EOF (-1) can be naturally assigned to it and used.  Also,
 * it looks like 10646 will be a 31 bit standard.  This means that if your
 * ints cannot hold 32 bits, you will be in trouble.  The reason an int was
 * chosen over a long is that the is*() and to*() routines take ints (says
 * ANSI C), but they use __ct_rune_t instead of int.
 *
 * NOTE: rune_t is not covered by ANSI nor other standards, and should not
 * be instantiated outside of lib/libc/locale.  Use wchar_t.  wchar_t and
 * rune_t must be the same type.  Also, wint_t must be no narrower than
 * wchar_t, and should be able to hold all members of the largest
 * character set plus one extra value (WEOF), and must be at least 16 bits.
 */
typedef	int		__ct_rune_t;	/* arg type for ctype funcs */
typedef	__ct_rune_t	__rune_t;	/* rune_t (see above) */
typedef	__ct_rune_t	__wchar_t;	/* wchar_t (see above) */
typedef	__ct_rune_t	__wint_t;	/* wint_t (see above) */

typedef	__uint32_t	__dev_t;	/* device number */

typedef	__uint32_t	__fixpt_t;	/* fixed point number */

/*
 * mbstate_t is an opaque object to keep conversion state during multibyte
 * stream conversions.
 */
typedef union {
	char		__mbstate8[128];
	__int64_t	_mbstateL;	/* for alignment */
} __mbstate_t;
#else /* __rtems__ */
#include <sys/types.h>

#ifdef __BSD_VISIBLE

#include <machine/rtems-bsd-endian.h>
#include <stdarg.h>

#define _INT8_T_DECLARED
#define _UINT8_T_DECLARED

#define _INT16_T_DECLARED
#define _UINT16_T_DECLARED

#define _INT32_T_DECLARED
#define _UINT32_T_DECLARED

#define _INT64_T_DECLARED
#define _UINT64_T_DECLARED

#define _INTPTR_T_DECLARED

#ifdef _KERNEL

typedef int boolean_t;

typedef clock_t __clock_t;

typedef unsigned int __cpumask_t;

typedef intptr_t __critical_t;

typedef struct device *device_t;

typedef double __double_t;

typedef float __float_t;

typedef intptr_t intfptr_t;
typedef uintptr_t uintfptr_t;

typedef uint32_t intrmask_t;

typedef time_t __time_t;

typedef ptrdiff_t __ptrdiff_t;

typedef char vm_memattr_t;

#endif /* _KERNEL */

typedef int accmode_t;
#define _ACCMODE_T_DECLARED

typedef const char *c_caddr_t;

typedef uint32_t cpumask_t;

typedef int cpusetid_t;

#define _DEV_T_DECLARED

typedef uint32_t __fixpt_t;
typedef __fixpt_t fixpt_t;

#define _GID_T_DECLARED

typedef uint32_t in_addr_t;
#define _IN_ADDR_T_DECLARED

typedef uint16_t in_port_t;
#define _IN_PORT_T_DECLARED

#define _MODE_T_DECLARED

typedef _off_t __off_t;
#define _OFF_T_DECLARED

#define _PID_T_DECLARED

typedef int register_t;
typedef unsigned int u_register_t;

typedef intptr_t segsz_t;

#undef __size_t
typedef size_t __size_t;
#define _SIZE_T_DECLARED

#define __ssize_t ssize_t
#define _SSIZE_T_DECLARED

#define _UID_T_DECLARED

#define __va_list va_list

typedef uintptr_t vm_offset_t;
typedef uint64_t vm_ooffset_t;
typedef uintptr_t vm_paddr_t;
typedef uint64_t vm_pindex_t;
typedef uintptr_t vm_size_t;

typedef int lwpid_t;
#define _LWPID_T_DECLARED

typedef int64_t rlim_t;
#define _RLIM_T_DECLARED

typedef uint8_t sa_family_t;
#define _SA_FAMILY_T_DECLARED

typedef size_t __socklen_t;
typedef __socklen_t socklen_t;
#define _SOCKLEN_T_DECLARED

#define SIZE_T_MAX SIZE_MAX

/* FIXME */
#define FNONBLOCK _FNONBLOCK

#endif /* __BSD_VISIBLE */

#endif /* __rtems__ */

#endif /* !_RTEMS_BSD_SYS__TYPES_H_ */
