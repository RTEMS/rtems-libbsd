/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief TODO.
 */

/*
 * Copyright (c) 2013, 2016 embedded brains GmbH.  All rights reserved.
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

#include <sys/cdefs.h>
#include <sys/types.h>
#include <sys/queue.h>

#include <setjmp.h>

__BEGIN_DECLS

struct program_fd_item {
	int	fd;
	LIST_ENTRY(program_fd_item) entries;
};

struct program_file_item {
	FILE	*file;
	LIST_ENTRY(program_file_item) entries;
};

struct program_allocmem_item {
	void	*ptr;
	LIST_ENTRY(program_allocmem_item) entries;
};

struct program_destructor {
	void	(*destructor)(void *);
	void	*arg;
	LIST_ENTRY(program_destructor) link;
};

struct rtems_bsd_program_control {
	void *context;
	int exit_code;
	const char *name;
	jmp_buf return_context;
	LIST_HEAD(, program_fd_item) open_fd;
	LIST_HEAD(, program_file_item) open_file;
	LIST_HEAD(, program_allocmem_item) allocated_mem;
	LIST_HEAD(, program_destructor) destructors;
};

struct rtems_bsd_program_control *rtems_bsd_program_get_control_or_null(void);

int rtems_bsd_program_set_control(struct rtems_bsd_program_control *);

__END_DECLS
