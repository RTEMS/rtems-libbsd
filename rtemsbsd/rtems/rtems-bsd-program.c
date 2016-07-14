/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
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

#include <machine/rtems-bsd-kernel-space.h>
#include <machine/rtems-bsd-thread.h>

#include <rtems/bsd/sys/param.h>
#include <rtems/bsd/sys/types.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/proc.h>
#include <sys/malloc.h>
#include <rtems/bsd/sys/lock.h>
#include <sys/mutex.h>

#include <fcntl.h>
#include <setjmp.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>

#undef printf
#define RTEMS_BSD_PROGRAM_NO_OPEN_WRAP
#define RTEMS_BSD_PROGRAM_NO_SOCKET_WRAP
#define RTEMS_BSD_PROGRAM_NO_CLOSE_WRAP
#define RTEMS_BSD_PROGRAM_NO_FOPEN_WRAP
#define RTEMS_BSD_PROGRAM_NO_FCLOSE_WRAP
#define RTEMS_BSD_PROGRAM_NO_MALLOC_WRAP
#define RTEMS_BSD_PROGRAM_NO_CALLOC_WRAP
#define RTEMS_BSD_PROGRAM_NO_REALLOC_WRAP
#define RTEMS_BSD_PROGRAM_NO_STRDUP_WRAP
#define RTEMS_BSD_PROGRAM_NO_VASPRINTF_WRAP
#define RTEMS_BSD_PROGRAM_NO_ASPRINTF_WRAP
#define RTEMS_BSD_PROGRAM_NO_FREE_WRAP
#include <machine/rtems-bsd-program.h>

struct rtems_bsd_program_control {
	void *context;
	int exit_code;
	const char *name;
	jmp_buf return_context;
	LIST_HEAD(, rtems_bsd_fd_list) open_fd;
	LIST_HEAD(, rtems_bsd_file_list) open_file;
	LIST_HEAD(, rtems_bsd_allocmem_list) allocated_mem;
};

struct rtems_bsd_fd_list {
	int	fd;
	LIST_ENTRY(rtems_bsd_fd_list) entries;
};

struct rtems_bsd_file_list {
	FILE	*file;
	LIST_ENTRY(rtems_bsd_file_list) entries;
};

struct rtems_bsd_allocmem_list {
	void	*ptr;
	LIST_ENTRY(rtems_bsd_allocmem_list) entries;
};

enum rtems_bsd_alloc_type {
	RTEMS_BSD_ALLOC_CALL_MALLOC,
	RTEMS_BSD_ALLOC_CALL_CALLOC,
	RTEMS_BSD_ALLOC_CALL_REALLOC,
	RTEMS_BSD_ALLOC_CALL_STRDUP,
};

static struct rtems_bsd_program_control *
rtems_bsd_program_get_cur_prog_ctrl_or_null(void)
{
	struct rtems_bsd_program_control *prog_ctrl = NULL;
	struct thread *td = rtems_bsd_get_curthread_or_null();

	if (td != NULL) {
		prog_ctrl = td->td_prog_ctrl;

		if (prog_ctrl == NULL) {
			panic("unexpected BSD program state");
		}
	} else {
		errno = ENOMEM;
	}

	return prog_ctrl;
}

static int
fd_remove(struct rtems_bsd_program_control *prog_ctrl, int fd)
{
	struct rtems_bsd_fd_list *current;
	int rv = -1;

	for(current = prog_ctrl->open_fd.lh_first;
	    current != NULL;
	    current = current->entries.le_next) {
		if(current->fd == fd) {
			LIST_REMOVE(current, entries);
			free(current, M_TEMP);
			rv = 0;
			break;
		}
	}

	return rv;
}

static int
fd_close_remove(struct rtems_bsd_program_control *prog_ctrl, int fd)
{
	int rv = -1;

	rv = fd_remove(prog_ctrl, fd);
	if(rv == 0) {
		rv = close(fd);
	}

	return rv;
}

static void
fd_close_all(struct rtems_bsd_program_control *prog_ctrl)
{
	while(prog_ctrl->open_fd.lh_first != NULL) {
		struct rtems_bsd_fd_list *current;
		int fd;
		int rv;

		current = prog_ctrl->open_fd.lh_first;
		fd = current->fd;

		rv = fd_close_remove(prog_ctrl, fd);
		if(rv != 0) {
			syslog(LOG_ERR,
			    "BSD Program: Could not close file %d or could not remove it from list of open files",
			    fd);
		}
	}
}

static int
file_remove(struct rtems_bsd_program_control *prog_ctrl, FILE *file)
{
	struct rtems_bsd_file_list *current;
	int rv = EOF;

	for(current = prog_ctrl->open_file.lh_first;
	    current != NULL;
	    current = current->entries.le_next) {
		if(current->file == file) {
			LIST_REMOVE(current, entries);
			free(current, M_TEMP);
			rv = 0;
			break;
		}
	}

	return rv;
}

static int
file_close_remove(struct rtems_bsd_program_control *prog_ctrl, FILE *file)
{
	int rv = EOF;

	rv = file_remove(prog_ctrl, file);
	if(rv == 0) {
		rv = fclose(file);
	}

	return rv;
}

static void
file_close_all(struct rtems_bsd_program_control *prog_ctrl)
{
	while(prog_ctrl->open_file.lh_first != NULL) {
		struct rtems_bsd_file_list *current;
		FILE *file;
		int rv;

		current = prog_ctrl->open_file.lh_first;
		file = current->file;

		rv = file_close_remove(prog_ctrl, file);
		if(rv != 0) {
			syslog(LOG_ERR,
			    "BSD Program: Could not close stream %p or could not remove it from list of open streams",
			    file);
		}
	}
}

static int
allocmem_remove(struct rtems_bsd_program_control *prog_ctrl, void *ptr)
{
	struct rtems_bsd_allocmem_list *current;
	int rv = -1;

	for(current = prog_ctrl->allocated_mem.lh_first;
	    current != NULL;
	    current = current->entries.le_next) {
		if(current->ptr == ptr) {
			LIST_REMOVE(current, entries);
			rv = 0;
			break;
		}
	}

	return rv;
}

static int
allocmem_free_remove(struct rtems_bsd_program_control *prog_ctrl, void *ptr)
{
	int rv = -1;

	rv = allocmem_remove(prog_ctrl, ptr);
	if(rv == 0) {
		free(ptr, M_TEMP);
	}

	return rv;
}

static void
allocmem_free_all(struct rtems_bsd_program_control *prog_ctrl)
{
	while(prog_ctrl->allocated_mem.lh_first != NULL) {
		struct rtems_bsd_allocmem_list *current;
		void *ptr;
		int rv;

		current = prog_ctrl->allocated_mem.lh_first;
		ptr = current->ptr;

		rv = allocmem_free_remove(prog_ctrl, ptr);
		if(rv != 0) {
			/* This should never happen. It would mean that someone
			 * changed the allocmem list while we are removing the
			 * element. */
			syslog(LOG_ERR,
			    "BSD Program: Could not remove %p from list of allocated memory",
			    ptr);
		}
	}
}

int
rtems_bsd_program_call(const char *name, int (*prog)(void *), void *context)
{
	struct thread *td = rtems_bsd_get_curthread_or_null();
	int exit_code = EXIT_FAILURE;

	if (td != NULL) {
		struct rtems_bsd_program_control *prog_ctrl = td->td_prog_ctrl;

		if (prog_ctrl == NULL) {
			prog_ctrl = malloc(sizeof(*prog_ctrl), M_TEMP, 0);

			if (prog_ctrl != NULL) {
				td->td_prog_ctrl = prog_ctrl;

				prog_ctrl->context = context;
				prog_ctrl->name = name;
				prog_ctrl->exit_code = exit_code;

				LIST_INIT(&(prog_ctrl->open_fd));
				LIST_INIT(&(prog_ctrl->open_file));
				LIST_INIT(&(prog_ctrl->allocated_mem));

				if (setjmp(prog_ctrl->return_context) == 0) {
					exit_code = (*prog)(context);
				} else {
					exit_code = prog_ctrl->exit_code;
				}

				fd_close_all(prog_ctrl);
				file_close_all(prog_ctrl);
				allocmem_free_all(prog_ctrl);

				td->td_prog_ctrl = NULL;
				free(prog_ctrl, M_TEMP);
			} else {
				errno = ENOMEM;
			}
		} else {
			panic("unexpected BSD program state");
		}
	} else {
		errno = ENOMEM;
	}

	return exit_code;
}

void
rtems_bsd_program_exit(int exit_code)
{
	struct thread *td = rtems_bsd_get_curthread_or_null();

	if (td != NULL) {
		struct rtems_bsd_program_control *prog_ctrl = td->td_prog_ctrl;

		if (prog_ctrl != NULL) {
			prog_ctrl->exit_code = exit_code;
			longjmp(prog_ctrl->return_context, 1);
		}
	}

	panic("unexpected BSD program exit");
}

void
rtems_bsd_program_error(const char *fmt, ...)
{
	va_list list;
	va_start(list, fmt);
	vfprintf(stderr, fmt, list);
	fprintf(stderr, "\n");
	va_end(list);
	rtems_bsd_program_exit(1);
}

const char *
rtems_bsd_program_get_name(void)
{
	struct thread *td = rtems_bsd_get_curthread_or_null();
	const char *name = "?";

	if (td != NULL) {
		struct rtems_bsd_program_control *prog_ctrl = td->td_prog_ctrl;

		if (prog_ctrl != NULL) {
			name = prog_ctrl->name;
		}
	}

	return name;
}

void *
rtems_bsd_program_get_context(void)
{
	struct thread *td = rtems_bsd_get_curthread_or_null();
	void *context = NULL;

	if (td != NULL) {
		struct rtems_bsd_program_control *prog_ctrl = td->td_prog_ctrl;

		if (prog_ctrl != NULL) {
			context = prog_ctrl->context;
		}
	}

	return context;
}

struct main_context {
	int argc;
	char **argv;
	int (*main)(int, char **);
};

static int
call_main(void *ctx)
{
	const struct main_context *mc = ctx;

	return (*mc->main)(mc->argc, mc->argv);
}

int
rtems_bsd_program_call_main(const char *name, int (*main)(int, char **),
    int argc, char **argv)
{
	struct main_context mc = {
		.argc = argc,
		.argv = argv,
		.main = main
	};
	int exit_code;

	if (argv[argc] == NULL) {
		exit_code = rtems_bsd_program_call(name, call_main, &mc);
	} else {
		errno = EFAULT;
		exit_code = EXIT_FAILURE;
	}

	return exit_code;
}

int
rtems_bsd_program_call_main_with_data_restore(const char *name,
    int (*main)(int, char **), int argc, char **argv,
    void *data_buf, const size_t data_size)
{
	int exit_code = EXIT_FAILURE;
	void *savebuf;

	savebuf = malloc(data_size, M_TEMP, 0);
	if(savebuf == NULL) {
		errno = ENOMEM;
		exit_code = EXIT_FAILURE;
	} else {
		memcpy(savebuf, data_buf, data_size);
		exit_code = rtems_bsd_program_call_main(name, main, argc,
		    argv);
		memcpy(data_buf, savebuf, data_size);
		free(savebuf, M_TEMP);
	}

	return exit_code;
}

static struct mtx program_mtx;

MTX_SYSINIT(rtems_bsd_program, &program_mtx, "BSD program", MTX_DEF);

void
rtems_bsd_program_lock(void)
{
	mtx_lock(&program_mtx);
}

void
rtems_bsd_program_unlock(void)
{
	mtx_unlock(&program_mtx);
}

int
rtems_bsd_program_open(const char *path, int oflag, ...)
{
	struct rtems_bsd_program_control *prog_ctrl =
	    rtems_bsd_program_get_cur_prog_ctrl_or_null();
	va_list list;
	mode_t mode = 0;
	int fd = -1;

	if (prog_ctrl != NULL) {
		struct rtems_bsd_fd_list *new =
		    malloc(sizeof(struct rtems_bsd_fd_list), M_TEMP, 0);

		if(new != NULL) {
			va_start(list, oflag);
			mode = va_arg(list, mode_t);

			fd = open(path, oflag, mode);

			va_end(list);

			if(fd != -1) {
				new->fd = fd;
				LIST_INSERT_HEAD(&(prog_ctrl->open_fd),
				    new, entries);
			} else {
				free(new, M_TEMP);
			}
		} else {
			errno = ENOMEM;
		}
	}

	return fd;
}

int
rtems_bsd_program_socket(int domain, int type, int protocol)
{
	struct rtems_bsd_program_control *prog_ctrl =
	    rtems_bsd_program_get_cur_prog_ctrl_or_null();
	int fd = -1;

	if (prog_ctrl != NULL) {
		struct rtems_bsd_fd_list *new =
		    malloc(sizeof(struct rtems_bsd_fd_list), M_TEMP, 0);

		if(new != NULL) {
			/* FIXME: Why is there an implicit declaration warning?
			 */
			fd = socket(domain, type, protocol);

			if(fd != -1) {
				new->fd = fd;
				LIST_INSERT_HEAD(&(prog_ctrl->open_fd),
				    new, entries);
			} else {
				free(new, M_TEMP);
			}
		} else {
			errno = ENOMEM;
		}
	}

	return fd;
}

int
rtems_bsd_program_close(int fd)
{
	struct rtems_bsd_program_control *prog_ctrl =
	    rtems_bsd_program_get_cur_prog_ctrl_or_null();
	int rv = -1;

	if (prog_ctrl != NULL) {
		rv = fd_close_remove(prog_ctrl, fd);
	}

	return rv;
}

FILE *
rtems_bsd_program_fopen(const char *restrict filename,
    const char *restrict mode)
{
	FILE *file = NULL;
	struct rtems_bsd_program_control *prog_ctrl =
	    rtems_bsd_program_get_cur_prog_ctrl_or_null();

	if (prog_ctrl != NULL) {
		struct rtems_bsd_file_list *new =
		    malloc(sizeof(struct rtems_bsd_file_list), M_TEMP, 0);

		if(new != NULL) {
			file = fopen(filename, mode);

			if(file != NULL) {
				new->file = file;
				LIST_INSERT_HEAD(
				    &(prog_ctrl->open_file), new,
				    entries);
			} else {
				free(new, M_TEMP);
			}
		} else {
			errno = ENOMEM;
		}
	}

	return file;
}

int
rtems_bsd_program_fclose(FILE *file)
{
	struct rtems_bsd_program_control *prog_ctrl =
	    rtems_bsd_program_get_cur_prog_ctrl_or_null();
	int rv = EOF;

	if (prog_ctrl != NULL) {
		rv = file_close_remove(prog_ctrl, file);
	}

	return rv;
}

static void *
rtems_bsd_program_alloc(size_t size, void *org_ptr)
{
	struct rtems_bsd_program_control *prog_ctrl =
	    rtems_bsd_program_get_cur_prog_ctrl_or_null();
	void *ptr = NULL;
	size_t size_with_list;
	size_t size_alligned;

	if (prog_ctrl != NULL) {
		/* align the end to the next word address */
		size_alligned = size;
		if((size_alligned & 0x3) != 0) {
			size_alligned = (size_alligned | 0x03) + 1;
		}
		size_with_list = size_alligned +
		    sizeof(struct rtems_bsd_allocmem_list);

		if(org_ptr != NULL) {
			/* It's a reallocation. So first remove the old pointer
			 * from the list */
			allocmem_remove(prog_ctrl, org_ptr);
		}

		ptr = realloc(org_ptr, size_with_list, M_TEMP, 0);

		if(ptr != NULL) {
			struct rtems_bsd_allocmem_list *listele;
			listele = ptr + size_alligned;
			listele->ptr = ptr;
			LIST_INSERT_HEAD(&(prog_ctrl->allocated_mem),
			    listele, entries);
		}
	}

	return ptr;
}

void *
rtems_bsd_program_malloc(size_t size)
{
	return rtems_bsd_program_alloc(size, NULL);
}

void *
rtems_bsd_program_calloc(size_t nelem, size_t elsize)
{
	void *ptr;
	size_t size = elsize * nelem;

	ptr = rtems_bsd_program_alloc(size, NULL);
	if(ptr != NULL) {
		memset(ptr, 0, size);
	}

	return ptr;
}

void *
rtems_bsd_program_realloc(void *ptr, size_t size)
{
	return rtems_bsd_program_alloc(size, ptr);
}

char *
rtems_bsd_program_strdup(const char *s1)
{
	size_t size = strlen(s1) + 1; /* add one for null termination */
	char *new;

	new = rtems_bsd_program_alloc(size, NULL);

	if(new != NULL) {
		memcpy(new, s1, size);
	}

	return new;
}

int
rtems_bsd_program_vasprintf(char **strp, const char *fmt, va_list ap)
{
	va_list aq;
	int size;
	int rv = -1;

	va_copy(aq, ap);
	size = vsnprintf(NULL, 0, fmt, aq);
	va_end(aq);

	size += 1; /* Add space for terminating null byte */
	*strp = rtems_bsd_program_alloc(size, NULL);

	if(*strp != NULL) {
		rv = vsnprintf(*strp, size, fmt, ap);
	}

	return rv;
}

int
rtems_bsd_program_asprintf(char **strp, const char *fmt, ...)
{
	va_list ap;
	int rv;

	va_start(ap, fmt);

	rv = rtems_bsd_program_vasprintf(strp, fmt, ap);

	va_end(ap);

	return rv;
}

void
rtems_bsd_program_free(void *ptr)
{
	if(ptr != NULL) {
		struct rtems_bsd_program_control *prog_ctrl =
		    rtems_bsd_program_get_cur_prog_ctrl_or_null();

		if (prog_ctrl != NULL) {
			int rv = allocmem_free_remove(prog_ctrl, ptr);
			if(rv != 0) {
				panic("unexpected BSD program state: could not find pointer to free");
			}
		} else {
			/* Outside of program context. Just free it. */
			free(ptr, M_TEMP);
		}
	}
}
