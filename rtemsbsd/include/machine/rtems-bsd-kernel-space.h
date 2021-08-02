/**
 * @file
 *
 * @ingroup rtems_bsd_machine
 *
 * @brief RTEMS FreeBSD Kernel related machine settings and defines
 */

/*
 * Copyright (c) 2020-2021 Chris Johns <chrisj@rtems.org>.  All rights reserved.
 *
 * Copyright (c) 2009-2013 embedded brains GmbH.  All rights reserved.
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

#ifndef _RTEMS_BSD_MACHINE_RTEMS_BSD_KERNEL_SPACE_H_
#define _RTEMS_BSD_MACHINE_RTEMS_BSD_KERNEL_SPACE_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * FreeBSD warning control
 */
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-value"
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-const-variable"
#pragma GCC diagnostic ignored "-Waddress-of-packed-member"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"

/* FIXME: start: Update Newlib */
#define MSIZE 256

#define	__GLOBL1(sym)	__asm__(".globl " #sym)
#define	__GLOBL(sym)	__GLOBL1(sym)

#define O_CLOEXEC 0

/* FIXME: end: Update Newlib */

#define FSEARCH O_SEARCH
#define O_FSYNC FSYNC

#define FMASK (FREAD|FWRITE|FAPPEND|FASYNC|FNONBLOCK|O_DIRECT)
#define FCNTLFLAGS (FAPPEND|FASYNC|FNONBLOCK|O_DIRECT)
#define FHASLOCK 0

#define PIPE_NODIRECT

#define SEEK_DATA               3       /* set file offset to next data past offset */
#define SEEK_HOLE               4       /* set file offset to next hole past offset */
#define L_SET                   SEEK_SET
#define L_INCR                  SEEK_CUR
#define L_XTND                  SEEK_END
#define O_FSYNC                 FSYNC
#define O_SHLOCK                FSHLOCK
#define O_EXLOCK                FEXLOCK
#define POSIX_FADV_NORMAL       0       /* no special treatment */
#define POSIX_FADV_RANDOM       1       /* expect random page references */
#define POSIX_FADV_SEQUENTIAL   2       /* expect sequential page references */
#define POSIX_FADV_WILLNEED     3       /* will need these pages */
#define POSIX_FADV_DONTNEED     4       /* dont need these pages */
#define POSIX_FADV_NOREUSE      5       /* access data only once */

/* Used in NFS common code */
#define __LINUX_ERRNO_EXTENSIONS__ 1

#define __FreeBSD__ 1

/* General define to activate BSD kernel parts */
#define _KERNEL 1

/*
 * Various developer tracing options. See waf --help and --freebsd-options.
 */
#ifndef RTEMS_BSD_DESCRIP_TRACE
  #define RTEMS_BSD_DESCRIP_TRACE (0)
#endif
#ifndef RTEMS_BSD_SYSCALL_TRACE
  #define RTEMS_BSD_SYSCALL_TRACE (0)
#endif
#ifndef RTEMS_BSD_VFS_TRACE
  #define RTEMS_BSD_VFS_TRACE (0)
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#include <machine/rtems-bsd-version.h>
#include <machine/rtems-bsd-kernel-namespace.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Assert */

void rtems_bsd_assert_func(const char *file, int line, const char *func, const char *expr);

#ifdef RTEMS_BSD_NO_ASSERT
# define BSD_ASSERT(expr) ((void) 0)
#else
# define BSD_ASSERT(expr) (__predict_true(expr) ? (void) 0 : rtems_bsd_assert_func(__FILE__, __LINE__, __func__, #expr))
#endif

/* General definitions */

#define MACHINE "rtems"

#define MACHINE_ARCH "rtems"

#define M_RTEMS_HEAP 0

#define BSD_DEFAULT_FIB 0

#define BSD_DEFAULT_PID 1 /* Must match getid() */

#define BSD_DEFAULT_UID 0

#define BSD_DEFAULT_GID 0

#define BSD_DEFAULT_PRISON (&prison0)

#define kdb_active 0

/* pseudo-errors returned inside kernel to modify return to process */
#define	ERESTART	(-1)		/* restart syscall */
#define	EJUSTRETURN	(-2)		/* don't modify regs, just return */
#define	ENOIOCTL	(-3)		/* ioctl not handled by this layer */
#define	EDIRIOCTL	(-4)		/* do direct ioctl in GEOM */
#define ERELOOKUP       (-5)            /* retry the directory lookup */

/* error codes used in the NFS code */
#define EBADRPC         72
#define EPROGMISMATCH   75
#define EAUTH           80
#define ENOTCAPABLE     93

#define	EDOOFUS __ELASTERROR

typedef unsigned char   u_char;
typedef unsigned short  u_short;
typedef unsigned int    u_int;
typedef unsigned long   u_long;
typedef unsigned short  ushort;         /* Sys V compatibility */
typedef unsigned int    uint;           /* Sys V compatibility */

typedef __UINT64_TYPE__ uoff_t;

#define __LLONG_MAX __LONG_LONG_MAX__
#define __OFF_MAX   __LLONG_MAX
#define OFF_MAX     __OFF_MAX

#define F_WAIT          0x010           /* Wait until lock is granted */
#define F_FLOCK         0x020           /* Use flock(2) semantics for lock */
#define F_POSIX         0x040           /* Use POSIX semantics for lock */
#define F_REMOTE        0x080           /* Lock owner is remote NFS client */
#define F_NOINTR        0x100           /* Ignore signals when waiting */

#define _PC_ACL_EXTENDED        59
#define _PC_ACL_PATH_MAX        60
#define _PC_CAP_PRESENT         61
#define _PC_INF_PRESENT         62
#define _PC_MAC_PRESENT         63
#define _PC_ACL_NFS4            64

#define FFLAGS(oflags)  ((oflags) + 1)
#define OFLAGS(fflags)  ((fflags) - 1)

#define buffer_arena NULL
extern struct vmem *rtems_bsd_transient_arena;
#define transient_arena rtems_bsd_transient_arena

struct uma_zone;
extern struct uma_zone* ncl_pbuf_zone;
struct uma_zone* pbuf_zsecond_create(char *name, int max);

/*
 * Device dev_t handling
 */
int rtems_bsd__major(dev_t _d);
int rtems_bsd__minor(dev_t _d);
dev_t rtems_bsd__makedev(int _M, int _m);
#define major(d)        rtems_bsd__major(d)
#define minor(d)        rtems_bsd__minor(d)
#define makedev(M, m)   rtems_bsd__makedev((M), (m))

/*
 * VM, avoid bringing the headers across
 */
#define VM_OBJECT_WLOCK(object)
#define VM_OBJECT_WUNLOCK(object)

/*
 * From freebsd-oirg/sys/sys/dirent.h
 */
#define _GENERIC_DIRLEN(namlen)                                 \
        ((__offsetof(struct dirent, d_name) + (namlen) + 1 + 7) & ~7)
#define _GENERIC_DIRSIZ(dp)     _GENERIC_DIRLEN((dp)->d_namlen)

/*
 * From signalvar.h
 */
#define SIGISMEMBER(set, signo) (0)

/*
 * Special knote status bit to indicate the kn_fp is an iop.
 */
#define KN_FP_IS_IOP       0x10000000

/*
 * Ensure that padding bytes are zeroed and that the name is NUL-terminated.
 */
struct dirent;
void dirent_terminate(struct dirent *dp);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_BSD_MACHINE_RTEMS_BSD_KERNEL_SPACE_H_ */
