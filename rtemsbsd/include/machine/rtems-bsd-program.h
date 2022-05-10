/**
 * @file
 *
 * @ingroup rtems_bsd_machine
 *
 * @brief TODO.
 */

/*
 * Copyright (c) 2013, 2019 embedded brains GmbH.  All rights reserved.
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
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

__BEGIN_DECLS

int
rtems_bsd_program_call(const char *name, int (*prog)(void *), void *context);

int
rtems_bsd_program_call_main(const char *name, int (*main)(int, char **),
    int argc, char **argv);

int
rtems_bsd_program_call_main_with_data_restore(const char *name,
    int (*main)(int, char **), int argc, char **argv,
    void *data_buf, const size_t data_size);

void *
rtems_bsd_program_add_destructor(void (*destructor)(void *), void *arg);

void
rtems_bsd_program_remove_destructor(void *cookie, bool call);

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

int
rtems_bsd_program_open(const char *path, int oflag, ...);

int
rtems_bsd_program_socket(int domain, int type, int protocol);

int
rtems_bsd_program_close(int fd);

FILE *
rtems_bsd_program_fopen(const char *restrict filename,
    const char *restrict mode);

int
rtems_bsd_program_fclose(FILE *file);

void *
rtems_bsd_program_malloc(size_t size);

void *
rtems_bsd_program_calloc(size_t nelem, size_t elsize);

void *
rtems_bsd_program_realloc(void *ptr, size_t size);

void *
rtems_bsd_program_reallocf(void *ptr, size_t size);

char *
rtems_bsd_program_strdup(const char *s);

char *
rtems_bsd_program_strndup(const char *s, size_t size);

int
rtems_bsd_program_vasprintf(char **strp, const char *fmt, va_list ap);

int
rtems_bsd_program_asprintf(char **strp, const char *fmt, ...);

void
rtems_bsd_program_free(void *ptr);

#ifndef RTEMS_BSD_PROGRAM_NO_ABORT_WRAP
  #define abort() rtems_bsd_program_exit(1)
#endif

#ifndef RTEMS_BSD_PROGRAM_NO_EXIT_WRAP
  #define exit(code) rtems_bsd_program_exit(code)
#endif

#ifndef RTEMS_BSD_PROGRAM_NO_ERROR_WRAP
  #define error(fmt, ...) rtems_bsd_program_error(fmt, ## __VA_ARGS__)
#endif

#ifndef RTEMS_BSD_PROGRAM_NO_GETPROGNAME_WRAP
  #define getprogname() rtems_bsd_program_get_name()
#endif

#ifndef RTEMS_BSD_PROGRAM_NO_PRINTF_WRAP
  #define printf(...) fprintf(stdout, __VA_ARGS__)
#endif

#ifndef RTEMS_BSD_PROGRAM_NO_OPEN_WRAP
  #define open(path, oflag, ...) \
      rtems_bsd_program_open(path, oflag, ## __VA_ARGS__)
#endif

#ifndef RTEMS_BSD_PROGRAM_NO_SOCKET_WRAP
  #define socket(domain, type, protocol) \
      rtems_bsd_program_socket(domain, type, protocol)
#endif

#ifndef RTEMS_BSD_PROGRAM_NO_CLOSE_WRAP
  #define close(fildes) rtems_bsd_program_close(fildes)
#endif

#ifndef RTEMS_BSD_PROGRAM_NO_FOPEN_WRAP
  #define fopen(filename, mode) rtems_bsd_program_fopen(filename, mode)
#endif

#ifndef RTEMS_BSD_PROGRAM_NO_FCLOSE_WRAP
  #define fclose(file) rtems_bsd_program_fclose(file)
#endif

#ifndef RTEMS_BSD_PROGRAM_NO_MALLOC_WRAP
  #define malloc(size) rtems_bsd_program_malloc(size)
#endif

#ifndef RTEMS_BSD_PROGRAM_NO_CALLOC_WRAP
  #define calloc(nelem, elsize) rtems_bsd_program_calloc(nelem, elsize)
#endif

#ifndef RTEMS_BSD_PROGRAM_NO_REALLOC_WRAP
  #define realloc(ptr, size) rtems_bsd_program_realloc(ptr, size)
#endif

#ifndef RTEMS_BSD_PROGRAM_NO_REALLOC_WRAP
  #define reallocf(ptr, size) rtems_bsd_program_reallocf(ptr, size)
#endif

#ifndef RTEMS_BSD_PROGRAM_NO_STRDUP_WRAP
  #define strdup(s) rtems_bsd_program_strdup(s)
#endif

#ifndef RTEMS_BSD_PROGRAM_NO_STRNDUP_WRAP
  #define strndup(s, size) rtems_bsd_program_strndup(s, size)
#endif

#ifndef RTEMS_BSD_PROGRAM_NO_VASPRINTF_WRAP
  #define vasprintf(strp, fmt, ap) \
      rtems_bsd_program_vasprintf(strp, fmt, ap)
#endif

#ifndef RTEMS_BSD_PROGRAM_NO_ASPRINTF_WRAP
  #define asprintf(strp, fmt, ...) \
      rtems_bsd_program_asprintf(strp, fmt, ## __VA_ARGS__)
#endif

#ifndef RTEMS_BSD_PROGRAM_NO_FREE_WRAP
  #define free(ptr) rtems_bsd_program_free(ptr)
#endif

__END_DECLS

#endif /* _RTEMS_BSD_MACHINE_RTEMS_BSD_PROGRAM_H_ */
