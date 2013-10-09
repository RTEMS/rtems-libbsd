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

#include <machine/rtems-bsd-config.h>

#include <sys/types.h>
#include <sys/systm.h>

#include <sys/param.h>
#include <sys/bus.h>
#include <sys/interrupt.h>

#define SWI_TEST_THREAD_PRIO (0)

// Time to wait for swi-test-handler
#define SWI_SLEEP_TIME (1)

enum arg {
	HANDLER_NOT_VISITED,
	HANDLER_VISITED,
};

// The swi handler function, that will be called by all tests.
void swi_test_handler(void *arg)
{
	enum arg* argument = arg;

	printf("This is swi_test_handler.\n");

	*argument = HANDLER_VISITED;
}

void swi_test_normal_handler()
{
	struct intr_event *test_intr_event = NULL;
	enum arg argument = HANDLER_NOT_VISITED;
	void *test_ih = NULL;
	int retval = 0;

	printf("== Create thread and install a functional handler.\n");

	retval = swi_add(&test_intr_event, "swi_test", swi_test_handler, &argument,
		SWI_TEST_THREAD_PRIO, 0, &test_ih);
	assert(retval == 0);

	swi_sched(test_ih, 0);

	sleep(SWI_SLEEP_TIME);
	
	assert(argument == HANDLER_VISITED);
}

void swi_test_exclusive_handler()
{
	struct intr_event *test_intr_event = NULL;
	enum arg argument = HANDLER_NOT_VISITED;
	void *test_ih = NULL;
	int retval = 0;

	printf("== Create a thread with a exclusive handler.\n");

	retval = swi_add(&test_intr_event, "swi_test", swi_test_handler, &argument,
		SWI_TEST_THREAD_PRIO, INTR_EXCL, &test_ih);
	assert(retval == 0);

	swi_sched(test_ih, 0);
	
	sleep(SWI_SLEEP_TIME);
	
	assert(argument == HANDLER_VISITED);
}

void swi_test_error_number_of_processes_exceeded()
{
	// [EAGAIN] The system-imposed limit on the total number of processes
	// under execution would be exceeded.  The limit is given by the
	// sysctl(3) MIB variable KERN_MAXPROC.
#warning TODO: write test case
}

void swi_test_error_intr_entropy_set()
{
	struct intr_event *test_intr_event = NULL;
	enum arg argument = HANDLER_NOT_VISITED;
	void *test_ih = NULL;
	int retval = 0;

	printf("== Set the INTR_ENTROPY flag.\n");

	retval = swi_add(&test_intr_event, "swi_test", swi_test_handler, &argument,
		SWI_TEST_THREAD_PRIO, INTR_ENTROPY, &test_ih);
	assert(retval == EINVAL);
	
	sleep(SWI_SLEEP_TIME);
	
	assert(argument == HANDLER_NOT_VISITED);
}

void swi_test_error_point_to_hardware_interrupt_thread()
{
	//[EINVAL] The ithdp argument points to a hardware interrupt thread.
#warning TODO: write test case
}
	
void swi_test_error_name_null()
{
	struct intr_event *test_intr_event = NULL;
	enum arg argument = HANDLER_NOT_VISITED;
	void *test_ih = NULL;
	int retval = 0;

	printf("== Set name to NULL.\n");

	retval = swi_add(&test_intr_event, NULL, swi_test_handler, &argument,
		SWI_TEST_THREAD_PRIO, 0, &test_ih);
	assert(retval == EINVAL);
	
	sleep(SWI_SLEEP_TIME);
	
	assert(argument == HANDLER_NOT_VISITED);
}

void swi_test_error_handler_null()
{
	struct intr_event *test_intr_event = NULL;
	enum arg argument = HANDLER_NOT_VISITED;
	void *test_ih = NULL;
	int retval = 0;

	printf("== Set handler to NULL.\n");

	retval = swi_add(&test_intr_event, "swi_test", NULL, &argument,
		SWI_TEST_THREAD_PRIO, 0, &test_ih);
	assert(retval == EINVAL);
	
	sleep(SWI_SLEEP_TIME);
	
	assert(argument == HANDLER_NOT_VISITED);
}

void swi_test_error_has_allready_exclusive()
{
	struct intr_event *test_intr_event = NULL;
	enum arg argument = HANDLER_NOT_VISITED;
	void *test_ih1 = NULL;
	void *test_ih2 = NULL;
	int retval = 0;

	printf("== Create a thread with a exclusive handler and try to add another handler.\n");

	retval = swi_add(&test_intr_event, "swi_test1", swi_test_handler, &argument,
		SWI_TEST_THREAD_PRIO, INTR_EXCL, &test_ih1);
	assert(retval == 0);

	retval = swi_add(&test_intr_event, "swi_test2", swi_test_handler, &argument,
		SWI_TEST_THREAD_PRIO, 0, &test_ih2);
	assert(retval == EINVAL);
	
	sleep(SWI_SLEEP_TIME);
	
	assert(argument == HANDLER_NOT_VISITED);
}

void swi_test(void)
{
	swi_test_normal_handler();
	swi_test_exclusive_handler();
	swi_test_error_number_of_processes_exceeded();
	swi_test_error_intr_entropy_set();
	swi_test_error_point_to_hardware_interrupt_thread();
	swi_test_error_name_null();
	swi_test_error_handler_null();
	swi_test_error_has_allready_exclusive();
}

