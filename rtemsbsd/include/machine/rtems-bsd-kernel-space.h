/**
 * @file
 *
 * @ingroup rtems_bsd_machine
 *
 * @brief TODO.
 */

/*
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

/* FIXME: Update Newlib */
#define MSIZE 256

/* FIXME: Update Newlib */
#define	__GLOBL1(sym)	__asm__(".globl " #sym)
#define	__GLOBL(sym)	__GLOBL1(sym)

#define O_CLOEXEC 0

#define __FreeBSD__ 1

/* General define to activate BSD kernel parts */
#define _KERNEL 1

#include <machine/rtems-bsd-kernel-namespace.h>

/* Assert */

void rtems_bsd_assert_func(const char *file, int line, const char *func, const char *expr);

#ifdef RTEMS_BSD_NO_ASSERT
# define BSD_ASSERT(expr) ((void) 0)
#else
# define BSD_ASSERT(expr) (__predict_true(expr) ? (void) 0 : rtems_bsd_assert_func(__FILE__, __LINE__, __func__, #expr))
#endif

/* General definitions */

#define M_RTEMS_HEAP 0

#define BSD_DEFAULT_FIB 0

#define BSD_DEFAULT_PID 1 /* Must match getid() */

#define BSD_DEFAULT_UID 0

#define BSD_DEFAULT_GID 0

#define BSD_DEFAULT_PRISON (&prison0)

#define kdb_active 0

#endif /* _RTEMS_BSD_MACHINE_RTEMS_BSD_KERNEL_SPACE_H_ */
