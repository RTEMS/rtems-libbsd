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

#include <sys/socket.h>

#include <errno.h>
#include <stdlib.h>

#define RTEMS_BSD_PROGRAM_NO_SOCKET_WRAP
#define RTEMS_BSD_PROGRAM_NO_CALLOC_WRAP
#define RTEMS_BSD_PROGRAM_NO_FREE_WRAP
#include <machine/rtems-bsd-program.h>

#include "program-internal.h"

int
rtems_bsd_program_socket(int domain, int type, int protocol)
{
	struct rtems_bsd_program_control *prog_ctrl;
	struct program_fd_item *item;
	int fd;

	prog_ctrl = rtems_bsd_program_get_control_or_null();
	if (prog_ctrl == NULL) {
		errno = ENOMEM;
		return (-1);
	}

	item = calloc(1, sizeof(*item));
	if (item == NULL) {
		return (-1);
	}

	fd = socket(domain, type, protocol);
	if (fd >= 0) {
		item->fd = fd;
		LIST_INSERT_HEAD(&prog_ctrl->open_fd, item, entries);
	} else {
		free(item);
	}

	return (fd);
}
