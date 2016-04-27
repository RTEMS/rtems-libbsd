/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief TODO.
 */

/*
 * Copyright (c) 2014, 2016 embedded brains GmbH.  All rights reserved.
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

#include <sys/types.h>
#include <sys/lock.h>
#include <sys/systm.h>
#include <sys/syslog.h>

#include <stdio.h>

#include <rtems/bsd/bsd.h>

#undef printf

#define	VPRINTF_LOCK() _Mutex_Acquire(&vprintf_mtx)
#define	VPRINTF_UNLOCK() _Mutex_Release(&vprintf_mtx)

static const char * const log_priorities[] = {
	[LOG_EMERG] = "emerg",
	[LOG_ALERT] = "alert",
	[LOG_CRIT] = "crit",
	[LOG_ERR] = "err",
	[LOG_WARNING] = "warning",
	[LOG_NOTICE] = "notice",
	[LOG_INFO] = "info",
	[LOG_DEBUG] = "debug"
};

/* Use a <sys/lock.h> mutex due to the static initialization capability */
static struct _Mutex_Control vprintf_mtx = _MUTEX_INITIALIZER;

static void
vprintf_putchar(int c, void *arg)
{
	int *last;

	last = arg;
	*last = c;
	putchar(c);
}

static int
default_vprintf_handler(int level, const char *fmt, va_list ap)
{
	int n;
	int last;

	VPRINTF_LOCK();

	if (level != LOG_PRINTF) {
		printf("%s: ", log_priorities[LOG_PRI(level)]);
	}

	last = -1;
	n = kvprintf(fmt, vprintf_putchar, &last, 10, ap);

	if (level != LOG_PRINTF && last != '\n') {
		putchar('\n');
	}

	VPRINTF_UNLOCK();
	return (n);
}

static int (*vprintf_handler)(int, const char *, va_list) =
    default_vprintf_handler;

void
rtems_bsd_set_vprintf_handler(int (*new_vprintf_handler)
    (int, const char *, va_list))
{

	vprintf_handler = new_vprintf_handler;
}

int
rtems_bsd_vprintf(int level, const char *fmt, va_list ap)
{

	return ((*vprintf_handler)(level, fmt, ap));
}
