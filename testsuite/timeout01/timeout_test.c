/*
 * Copyright (c) 2012 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
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

#include <assert.h>

#include <stdio.h>
#include <unistd.h>

#include <machine/rtems-bsd-kernel-space.h>

#include <rtems/bsd/sys/types.h>
#include <sys/systm.h>

#include <rtems/bsd/sys/param.h>
#include <rtems/bsd/sys/lock.h>
#include <sys/mutex.h>
#include <sys/rwlock.h>

#define TIMEOUT_MILLISECONDS	(100)

// test after TEST_NOT_FIRED_MS, if handlar has not been executed
#define TEST_NOT_FIRED_MS	(TIMEOUT_MILLISECONDS * 80 / 100)
// test TEST_FIRED_MS after TEST_NOT_FIRED_MS, if handlar has been executed
#define TEST_FIRED_MS		(TIMEOUT_MILLISECONDS * 40 / 100)
// delay handler by this time with a mutex
#define TEST_DELAY_MS		(TIMEOUT_MILLISECONDS)

enum arg {
	HANDLER_NOT_VISITED,
	HANDLER_VISITED,
};

static void timeout_handler(void *arg)
{
	enum arg* argument = arg;

	printf("This is the timout_handler.\n");

	*argument = HANDLER_VISITED;
}

void timeout_test_timeout()
{
	enum arg argument = HANDLER_NOT_VISITED;
	struct callout_handle handle;
	
	printf("== Start a timeout and test if handler has been called.\n");

	callout_handle_init(&handle);

	handle = timeout(timeout_handler, &argument, RTEMS_MILLISECONDS_TO_TICKS(TIMEOUT_MILLISECONDS));

	usleep(TEST_NOT_FIRED_MS * 1000);
	assert(argument == HANDLER_NOT_VISITED);
	
	usleep(TEST_FIRED_MS * 1000);
	assert(argument == HANDLER_VISITED);
}

void timeout_test_cancel_timeout()
{
	enum arg argument = HANDLER_NOT_VISITED;
	struct callout_handle handle;
	
	printf("== Start a timeout and cancel it.\n");

	callout_handle_init(&handle);

	handle = timeout(timeout_handler, &argument, RTEMS_MILLISECONDS_TO_TICKS(TIMEOUT_MILLISECONDS));

	usleep(TEST_NOT_FIRED_MS * 1000);
	
	untimeout(timeout_handler, &argument, handle);

	usleep(TEST_FIRED_MS * 1000);
	assert(argument == HANDLER_NOT_VISITED);
}

void timeout_test_callout(int mpsave)
{
	enum arg argument = HANDLER_NOT_VISITED;
	struct callout callout;
	int retval = 0;
	printf("== Start a callout and test if handler has been called. mpsave=%d\n", mpsave);

	callout_init(&callout, mpsave);

	retval = callout_reset(&callout, RTEMS_MILLISECONDS_TO_TICKS(TIMEOUT_MILLISECONDS), timeout_handler, &argument);
	assert(retval == 0);

	usleep(TEST_NOT_FIRED_MS * 1000);
	assert(argument == HANDLER_NOT_VISITED);
	
	usleep(TEST_FIRED_MS * 1000);
	assert(argument == HANDLER_VISITED);

	callout_deactivate(&callout);
}

void timeout_test_cancel_callout(int mpsave, bool use_drain)
{
	enum arg argument = HANDLER_NOT_VISITED;
	struct callout callout;
	int retval = 0;
	printf("== Start a callout and cancel it with %s. mpsave=%d\n", use_drain ? "drain" : "stop", mpsave);

	callout_init(&callout, mpsave);

	retval = callout_reset(&callout, RTEMS_MILLISECONDS_TO_TICKS(TIMEOUT_MILLISECONDS), timeout_handler, &argument);
	assert(retval == 0);

	usleep(TEST_NOT_FIRED_MS * 1000);

	if(!use_drain)
	{
		retval = callout_stop(&callout);
	}
	else
	{
		retval = callout_drain(&callout);
	}
	assert(retval != 0);
	
	usleep(TEST_FIRED_MS * 1000);
	assert(argument == HANDLER_NOT_VISITED);

	callout_deactivate(&callout);
}

void timeout_test_callout_reschedule(int mpsave, bool use_reset)
{
	enum arg argument = HANDLER_NOT_VISITED;
	struct callout callout;
	int retval = 0;
	printf("== Start a callout and reschedule it after some time with %s. mpsave=%d\n", use_reset ? "reset" : "schedule", mpsave);

	callout_init(&callout, mpsave);

	retval = callout_reset(&callout, RTEMS_MILLISECONDS_TO_TICKS(TIMEOUT_MILLISECONDS), timeout_handler, &argument);
	assert(retval == 0);

	usleep(TEST_NOT_FIRED_MS * 1000);
	assert(argument == HANDLER_NOT_VISITED);

	if(!use_reset)
	{
		retval = callout_schedule(&callout, RTEMS_MILLISECONDS_TO_TICKS(TIMEOUT_MILLISECONDS));
	}
	else
	{
		retval = callout_reset(&callout, RTEMS_MILLISECONDS_TO_TICKS(TIMEOUT_MILLISECONDS), timeout_handler, &argument);
	}
	assert(retval != 0);

	usleep(TEST_NOT_FIRED_MS * 1000);
	assert(argument == HANDLER_NOT_VISITED);

	usleep(TEST_FIRED_MS * 1000);
	assert(argument == HANDLER_VISITED);

	callout_deactivate(&callout);
}

void timeout_test_callout_mutex(bool delay_with_lock)
{
	enum arg argument = HANDLER_NOT_VISITED;
	struct callout callout;
	struct mtx mtx;
	int retval = 0;
	printf("== Start a callout with a mutex%s\n", delay_with_lock ? " and delay execution by locking it." : ".");

	mtx_init(&mtx, "callouttest", NULL, MTX_DEF);
	callout_init_mtx(&callout, &mtx, 0);

	retval = callout_reset(&callout, RTEMS_MILLISECONDS_TO_TICKS(TIMEOUT_MILLISECONDS), timeout_handler, &argument);
	assert(retval == 0);

	usleep(TEST_NOT_FIRED_MS * 1000);
	assert(argument == HANDLER_NOT_VISITED);

	if(delay_with_lock)
	{
		retval = mtx_trylock(&mtx);
		assert(retval != 0);

		usleep(TEST_DELAY_MS * 1000);
		assert(argument == HANDLER_NOT_VISITED);

		mtx_unlock(&mtx);
	}
	
	usleep(TEST_FIRED_MS * 1000);
	assert(argument == HANDLER_VISITED);
	
	callout_deactivate(&callout);

	mtx_destroy(&mtx);
}

void timeout_test_callout_rwlock(bool delay_with_lock)
{
	enum arg argument = HANDLER_NOT_VISITED;
	struct callout callout;
	struct rwlock rw;
	int retval = 0;
	printf("== Start a callout with a rwlock%s\n", delay_with_lock ? " and delay execution by locking it." : ".");

	rw_init(&rw, "callouttest");
	callout_init_rw(&callout, &rw, 0);

	retval = callout_reset(&callout, RTEMS_MILLISECONDS_TO_TICKS(TIMEOUT_MILLISECONDS), timeout_handler, &argument);
	assert(retval == 0);

	usleep(TEST_NOT_FIRED_MS * 1000);
	assert(argument == HANDLER_NOT_VISITED);
	
	if(delay_with_lock)
	{
		retval = rw_try_wlock(&rw);
		assert(retval != 0);

		usleep(TEST_DELAY_MS * 1000);
		assert(argument == HANDLER_NOT_VISITED);

		rw_wunlock(&rw);
	}

	usleep(TEST_FIRED_MS * 1000);
	assert(argument == HANDLER_VISITED);
	
	callout_deactivate(&callout);
}

void timeout_test(void)
{
	int mpsave = 0;

	timeout_test_timeout();
	timeout_test_cancel_timeout();

	for(mpsave = 0; mpsave<=1; mpsave++)
	{
		timeout_test_callout(mpsave);
		timeout_test_cancel_callout(mpsave, false);
		timeout_test_cancel_callout(mpsave, true);
		timeout_test_callout_reschedule(mpsave, false);
		timeout_test_callout_reschedule(mpsave, true);
	}
	
	timeout_test_callout_mutex(false);
	timeout_test_callout_mutex(true);
	timeout_test_callout_rwlock(false);
	timeout_test_callout_rwlock(true);
}
