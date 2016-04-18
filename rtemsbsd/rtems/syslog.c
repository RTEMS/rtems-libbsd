/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief TODO.
 */

/*
 * Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
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

#include <machine/rtems-bsd-user-space.h>
#include <machine/rtems-bsd-program.h>

#include <syslog.h>
#include <stdarg.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

typedef struct {
  int mask;
} rtems_bsd_syslog_context;

/* FIXME: This should be thread specific */
static rtems_bsd_syslog_context rtems_bsd_syslog_instance = {
	.mask = LOG_UPTO(LOG_DEBUG)
};

static const char * const rtems_bsd_syslog_priorities[] = {
	[LOG_EMERG] = "emerg",
	[LOG_ALERT] = "alert",
	[LOG_CRIT] = "crit",
	[LOG_ERR] = "err",
	[LOG_WARNING] = "warning",
	[LOG_NOTICE] = "notice",
	[LOG_INFO] = "info",
	[LOG_DEBUG] = "debug"
};

static rtems_bsd_syslog_context *
rtems_bsd_syslog_get_context(void)
{
	return &rtems_bsd_syslog_instance;
}

static void
rtems_bsd_syslog_format_buffer_overflow(void)
{
	fputs("err: syslog: format buffer overflow\n", stderr);
}

void
syslog(int priority, const char *format, ...)
{
	va_list ap;

	va_start(ap, format);
	vsyslog(priority, format, ap);
	va_end(ap);
}

void
vsyslog(int priority, const char *format, va_list ap)
{
	rtems_bsd_syslog_context *ctx = rtems_bsd_syslog_get_context();
	int pri = LOG_PRI(priority);
	char fmt[128];
	char buf[128];
	const char *src;
	char *dst;
	size_t rem;
	char *m;
	int n;
	size_t len;

	if ((LOG_MASK(pri) & ctx->mask) == 0) {
		return;
	}

	/* Expand the %m in the format string and add a newline character */

	src = format;
	dst = &fmt[0];
	rem = sizeof(fmt) - 2;

	while ((m = strstr(src, "%m")) != NULL) {
		size_t c = m - src;

		if (c > rem) {
			rtems_bsd_syslog_format_buffer_overflow();
			return;
		}

		memcpy(dst, src, c);
		dst += c;
		src += c + 2;
		rem -= c;

		n = sniprintf(dst, rem, "%s", strerror(errno));
		if (n > rem || n < 0) {
			rtems_bsd_syslog_format_buffer_overflow();
			return;
		}

		dst += (size_t) n;
		rem -= (size_t) n;
	}

	len = strlen(src);
	if (len > rem) {
		rtems_bsd_syslog_format_buffer_overflow();
		return;
	}

	memcpy(dst, src, len);
	dst += len;
	dst[0] = '\n';
	dst[1] = '\0';

	/* Print into buffer */

	dst = &buf[0];
	rem = sizeof(buf) - 1;

	n = sniprintf(dst, rem, "%s: ", rtems_bsd_syslog_priorities[pri]);
	if (n <= rem) {
		dst += (size_t) n;
		rem -= (size_t) n;
	}

	n = sniprintf(dst, rem, "%s: ", rtems_bsd_program_get_name());
	if (n <= rem) {
		dst += (size_t) n;
		rem -= (size_t) n;
	}

	vsniprintf(dst, rem, &fmt[0], ap);

	/* Write in one rush */

	fputs(&buf[0], stderr);
}

void
openlog(const char *ident, int option, int facility)
{
	/* TODO */
}

void
closelog(void)
{
	/* TODO */
}

int
setlogmask(int mask)
{
	rtems_bsd_syslog_context *ctx = rtems_bsd_syslog_get_context();
	int cur = ctx->mask;

	ctx->mask = mask & LOG_UPTO(LOG_DEBUG);

	return cur;
}
