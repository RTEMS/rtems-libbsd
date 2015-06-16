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

#ifndef _RTEMS_BSD_MACHINE_RTEMS_BSD_PROGRAM_H_
#define _RTEMS_BSD_MACHINE_RTEMS_BSD_PROGRAM_H_

#include <sys/cdefs.h>
#include <stdbool.h>
#include <stdio.h>

__BEGIN_DECLS

int
rtems_bsd_program_call(const char *name, int (*prog)(void *), void *context);

int
rtems_bsd_program_call_main(const char *name, int (*main)(int, char **),
    int argc, char **argv);

void
rtems_bsd_program_exit(int exit_code) __dead2;

void
rtems_bsd_program_error(const char *, ...) __attribute__ ((__format__ (__printf__, 1, 2)));

const char *
rtems_bsd_program_get_name(void);

void *
rtems_bsd_program_get_context(void) __pure2;

void
rtems_bsd_program_lock(void);

void
rtems_bsd_program_unlock(void);

#ifndef RTEMS_BSD_PROGRAM_NO_EXIT_WRAP
  #undef exit
  #define exit(code) rtems_bsd_program_exit(code)
#endif

#ifndef RTEMS_BSD_PROGRAM_NO_ERROR_WRAP
  #undef error
  #define error(fmt, ...) rtems_bsd_program_error(fmt, ## __VA_ARGS__)
#endif

#ifndef RTEMS_BSD_PROGRAM_NO_GETPROGNAME_WRAP
  #undef getprogname
  #define getprogname() rtems_bsd_program_get_name()
#endif

#ifndef RTEMS_BSD_PROGRAM_NO_PRINTF_WRAP
  #undef printf
  #define printf(...) fprintf(stdout, __VA_ARGS__)
#endif

__END_DECLS

#endif /* _RTEMS_BSD_MACHINE_RTEMS_BSD_PROGRAM_H_ */
