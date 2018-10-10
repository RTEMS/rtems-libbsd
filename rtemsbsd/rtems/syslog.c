/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief TODO.
 */

/*
 * Copyright (c) 2014, 2018 embedded brains GmbH.  All rights reserved.
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

#include <errno.h>
#include <stddef.h>
#include <strings.h>

#define SYSLOG_NAMES
#include <syslog.h>

#include <rtems/bsd/bsd.h>

static int syslog_mask = LOG_UPTO(LOG_NOTICE);

static bool
syslog_do_log(int priority)
{

	return (((LOG_MASK(LOG_PRI(priority)) & syslog_mask)) != 0);
}

void
syslog(int priority, const char *format, ...)
{

	if (syslog_do_log(priority)) {
		va_list ap;

		va_start(ap, format);
		vsyslog(priority, format, ap);
		va_end(ap);
	}
}

void
vsyslog(int priority, const char *format, va_list ap)
{

	if (syslog_do_log(priority)) {
		rtems_bsd_vprintf(priority, format, ap);
	}
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

	/*
	 * Ignore settings via this function since it has a process-wide scope.
	 * System services (a DHCP client daemon for example) may set this and
	 * assume that they run in their own environment (process).  This is
	 * not the case in RTEMS.  The syslog mask can be set via
	 * rtems_bsd_setlogpriority().
	 */
	return (0);
}

int
rtems_bsd_setlogpriority(const char *priority)
{
	const CODE *c;

	c = &prioritynames[0];
	while (c->c_name != NULL) {
		if (strcasecmp(c->c_name, priority) == 0) {
			syslog_mask = LOG_UPTO(c->c_val);
			return (0);
		}
		++c;
	}

	errno = ENOENT;
	return (-1);
}
