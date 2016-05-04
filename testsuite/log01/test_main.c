/*
 * Copyright (c) 2016 embedded brains GmbH.  All rights reserved.
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

#include <rtems/bsd/sys/types.h>
#include <sys/systm.h>
#include <syslog.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEST_NAME "LIBBSD LOG 1"

typedef struct {
	char a[99];
	char b[99];
	size_t i;
	int expected_level;
} test_context;

static test_context test_instance;

static void
vprintf_putchar(int c, void *arg)
{
	test_context *ctx;

	ctx = arg;
	ctx->a[ctx->i] = (char) c;
	++ctx->i;
}

static int
vprintf_handler(int level, const char *fmt, va_list ap)
{
	test_context *ctx;
	int n;

	ctx = &test_instance;

	assert(level == ctx->expected_level);
	n = (kvprintf(fmt, vprintf_putchar, &test_instance, 10, ap));
	ctx->a[n] = '\0';
	return (n);
}

static void
reset(test_context *ctx, int expected_level)
{

	ctx->i = 0;
	ctx->expected_level = expected_level;
	memset(&ctx->a[0], '?', sizeof(ctx->a));
	memset(&ctx->b[0], '?', sizeof(ctx->b));
}

static void
check(test_context *ctx, const char *b)
{

	memcpy(&ctx->b[0], b, strlen(b) + 1);
	assert(memcmp(&ctx->a[0], &ctx->b[0], sizeof(ctx->a)) == 0);
}

static void
test_main(void)
{
	test_context *ctx;
	int n;

	ctx = &test_instance;

	rtems_bsd_set_vprintf_handler(vprintf_handler);

	reset(ctx, 0);
	n = sprintf(&ctx->a[0], "%04i", 123);
	assert(n == 4);
	check(ctx, "0123");

	reset(ctx, 0);
	n = snprintf(&ctx->a[0], 3, "%04i", 123);
	assert(n == 4);
	check(ctx, "01");

	reset(ctx, 0);
	n = snprintf(&ctx->a[0], sizeof(ctx->a), "%i%f%i", 123, 0.0, 456);
	assert(n == 7);
	check(ctx, "123%f%i");

	reset(ctx, LOG_INFO);
	log(LOG_INFO, "log %i", 456);
	check(ctx, "log 456");

	reset(ctx, LOG_WARNING);
	syslog(LOG_WARNING, "syslog %i", 789);
	check(ctx, "syslog 789");

	reset(ctx, LOG_PRINTF);
	printf("printf %i", 0);
	check(ctx, "printf 0");

	reset(ctx, LOG_PRINTF);
	printf("reg=%b", 3, "\10\2BITTWO\1BITONE");
	check(ctx, "reg=3<BITTWO,BITONE>");

	reset(ctx, LOG_PRINTF);
	printf("out:	%4D", "AAAA", ":");
	check(ctx, "out:	41:41:41:41");

	exit(0);
}

#include <rtems/bsd/test/default-init.h>
