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

#include <err.h>
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <syslog.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <ck_epoch.h>

#define RTEMS_BSD_PROGRAM_NO_EXIT_WRAP
#define RTEMS_BSD_PROGRAM_NO_PRINTF_WRAP
#include <machine/rtems-bsd-program.h>
#include <machine/pcpu.h>

#include <rtems.h>
#include <rtems/libcsupport.h>

#define TEST_NAME "LIBBSD PROGRAM 1"

struct alloc_ctx {
	enum alloc_type {
		ALLOC_MALLOC = 0,
		ALLOC_CALLOC = 1,
		ALLOC_REALLOC = 2,
		ALLOC_STRDUP = 3,
		ALLOC_ASPRINTF = 4,
		ALLOC_LAST = 5,
	} type;
	unsigned free;
#define ALLOC_FREE_NONE		0x0
#define ALLOC_FREE_FIRST	0x1
#define ALLOC_FREE_SECOND	0x2
#define ALLOC_FREE_THIRD	0x4
#define ALLOC_FREE_ALL		(ALLOC_FREE_FIRST | \
				ALLOC_FREE_SECOND | \
				ALLOC_FREE_THIRD)
};

typedef void (*no_mem_test_body)(int fd);

typedef struct {
	no_mem_test_body body;
	int fd;
	rtems_id master_task;
} no_mem_test;

static void
no_mem_task(rtems_task_argument arg)
{
	const no_mem_test *self = (const no_mem_test *) arg;
	rtems_status_code sc;
	void *greedy;

	assert(rtems_configuration_get_unified_work_area());

	greedy = rtems_workspace_greedy_allocate(NULL, 0);
	(*self->body)(self->fd);
	rtems_workspace_greedy_free(greedy);

	sc = rtems_event_transient_send(self->master_task);
	assert(sc == RTEMS_SUCCESSFUL);

	sc = rtems_task_suspend(RTEMS_SELF);
	assert(sc == RTEMS_SUCCESSFUL);
}

void
do_no_mem_test(no_mem_test_body body, int fd)
{
	no_mem_test test = {
		.body = body,
		.fd = fd,
		.master_task = rtems_task_self()
	};
	rtems_status_code sc;
	rtems_id id;
	rtems_resource_snapshot snapshot;

	rtems_resource_snapshot_take(&snapshot);

	sc = rtems_task_create(
		rtems_build_name('N', 'M', 'E', 'M'),
		RTEMS_MINIMUM_PRIORITY,
		RTEMS_MINIMUM_STACK_SIZE,
		RTEMS_DEFAULT_MODES,
		RTEMS_FLOATING_POINT,
		&id
	);
	assert(sc == RTEMS_SUCCESSFUL);

	sc = rtems_task_start(id, no_mem_task, (rtems_task_argument) &test);
	assert(sc == RTEMS_SUCCESSFUL);

	sc = rtems_event_transient_receive(RTEMS_WAIT, RTEMS_NO_TIMEOUT);
	assert(sc == RTEMS_SUCCESSFUL);

	sc = rtems_task_delete(id);
	assert(sc == RTEMS_SUCCESSFUL);

	assert(rtems_resource_snapshot_check(&snapshot));
}

static const char prog_name[] = "prog";

static int
invalid_prog(void *ctx)
{
	(void) ctx;

	assert(0);
}

static int
invalid_main(int argc, char **argv)
{
	(void) argc;
	(void) argv;

	assert(0);
}

static void *const some_context = (void *) 0xcafe;

static int
some_prog(void *ctx)
{
	assert(ctx == some_context);
	assert(strcmp(rtems_bsd_program_get_name(), prog_name) == 0);
	assert(rtems_bsd_program_get_context() == some_context);
	errno = 0;
	rtems_bsd_program_exit(456);
}

static const int some_argc = 1;

static char *some_argv[] = { "a", NULL };

static int
some_main(int argc, char **argv)
{
	assert(argc == some_argc);
	assert(argv == some_argv);
	assert(strcmp(rtems_bsd_program_get_name(), prog_name) == 0);
	errno = 0;
	rtems_bsd_program_exit(789);
}

static void
no_mem_bsd_program(int fd)
{
	(void) fd;

	assert(rtems_bsd_program_call(prog_name, invalid_prog, NULL)
	    == EXIT_FAILURE);
	assert(rtems_bsd_program_call_main(prog_name, invalid_main, some_argc,
	    some_argv) == EXIT_FAILURE);
	assert(strcmp(rtems_bsd_program_get_name(), "?") == 0);
	assert(rtems_bsd_program_get_context() == NULL);
}

#define OVERWRITE_CONTENT	"Some test pattern"
#define OVERWRITE_AFTER_RESTORE	"xxxxxtestxxxxxxxx"
#define OVERWRITE_RESTORE_FIRST (5)
#define OVERWRITE_RESTORE_SIZE (4)
static const char overwrite_compare[] = OVERWRITE_AFTER_RESTORE;
static char overwrite_me[] = OVERWRITE_CONTENT;

static int
overwrite_main(int argc, char **argv)
{
	size_t len = strlen(overwrite_me);
	memset(overwrite_me, 'x', len);
	assert(strcmp(overwrite_me, overwrite_compare) != 0);
	errno = 0;
	rtems_bsd_program_exit(1012);
}

static void
test_bsd_program(void)
{
	rtems_resource_snapshot snapshot;
	int exit_code;
	void *greedy;
	char *invalid_argv[2] = { "a", "b" };
	struct thread *td;

	assert(rtems_configuration_get_unified_work_area());

	puts("test BSD program");

	/* Needs to be called once before the snapshot is taken so that the
	 * thread structure is already created. */
	td = rtems_bsd_get_curthread_or_null();
	assert(td != NULL);

	rtems_resource_snapshot_take(&snapshot);

	do_no_mem_test(no_mem_bsd_program, -1);

	greedy = rtems_workspace_greedy_allocate(NULL, 0);
	no_mem_bsd_program(-1);
	rtems_workspace_greedy_free(greedy);

	assert(rtems_resource_snapshot_check(&snapshot));

	errno = 0;
	exit_code = rtems_bsd_program_call_main(prog_name, NULL, 1, invalid_argv);
	assert(errno == EFAULT);
	assert(exit_code == EXIT_FAILURE);

	assert(rtems_resource_snapshot_check(&snapshot));

	errno = EINVAL;
	exit_code = rtems_bsd_program_call(prog_name, some_prog, some_context);
	assert(errno == 0);
	assert(exit_code == 456);
	assert(strcmp(rtems_bsd_program_get_name(), "?") == 0);
	assert(rtems_bsd_program_get_context() == NULL);

	assert(rtems_resource_snapshot_check(&snapshot));

	errno = EINVAL;
	exit_code = rtems_bsd_program_call_main(prog_name, some_main,
	    some_argc, some_argv);
	assert(errno == 0);
	assert(exit_code == 789);
	assert(strcmp(rtems_bsd_program_get_name(), "?") == 0);
	assert(rtems_bsd_program_get_context() == NULL);

	assert(rtems_resource_snapshot_check(&snapshot));

	exit_code = rtems_bsd_program_call_main_with_data_restore(prog_name,
	    overwrite_main, some_argc, some_argv,
	    overwrite_me + OVERWRITE_RESTORE_FIRST, OVERWRITE_RESTORE_SIZE);
	assert(errno == 0);
	assert(exit_code == 1012);
	assert(strcmp(rtems_bsd_program_get_name(), "?") == 0);
	assert(rtems_bsd_program_get_context() == NULL);
	assert(strcmp(overwrite_me, overwrite_compare) == 0);

	assert(rtems_resource_snapshot_check(&snapshot));
}

static void
test_warn(void)
{
	puts("test warn");

	errno = EAGAIN;
	warn("%s", "warn");

	errno = ENAMETOOLONG;
	warn(NULL);

	errno = 0;
	warnc(EDOM, "%s", "warnc");

	errno = 0;
	warnc(ERANGE, NULL);

	warnx("%s", "warnx");

	warnx(NULL);
}

static int
call_err(void *ctx)
{
	errno = EAGAIN;
	err(10, "%s", "call_err");
}

static int
call_err_null(void *ctx)
{
	errno = ENAMETOOLONG;
	err(11, NULL);
}

static int
call_errc(void *ctx)
{
	errc(12, EDOM, "%s", "call_errc");
}

static int
call_errc_null(void *ctx)
{
	errc(13, ERANGE, NULL);
}

static int
call_errx(void *ctx)
{
	errx(14, "%s", "call_errx");
}

static int
call_errx_null(void *ctx)
{
	errx(15, NULL);
}

static void
test_err(void)
{
	int exit_code;

	puts("test err");

	exit_code = rtems_bsd_program_call("err", call_err, NULL);
	assert(exit_code == 10);

	exit_code = rtems_bsd_program_call("err", call_err_null, NULL);
	assert(exit_code == 11);

	exit_code = rtems_bsd_program_call("errc", call_errc, NULL);
	assert(exit_code == 12);

	exit_code = rtems_bsd_program_call("errc", call_errc_null, NULL);
	assert(exit_code == 13);

	exit_code = rtems_bsd_program_call("errx", call_errx, NULL);
	assert(exit_code == 14);

	exit_code = rtems_bsd_program_call("errx", call_errx_null, NULL);
	assert(exit_code == 15);
}

static int
call_socket(void *ctx)
{
	int fd = socket(PF_INET, SOCK_DGRAM, 0);
	assert (fd != -1);
	return 0;
}

static int
call_socket_close(void *ctx)
{
	int rv;
	int fd = socket(PF_INET, SOCK_DGRAM, 0);
	assert (fd != -1);

	rv = close(fd);
	assert(rv == 0);

	return 0;
}

static int
call_open(void *ctx)
{
	int fd = open("/testfile", O_RDWR | O_CREAT, S_IRWXU);
	assert (fd != -1);
	return 0;
}

static int
call_open_close(void *ctx)
{
	int rv;
	int fd = open("/testfile", O_RDWR);
	assert (fd != -1);

	rv = close(fd);
	assert(rv == 0);

	return 0;
}

static int
call_fopen(void *ctx)
{
	FILE *file = fopen("/testfile", "rw");
	assert (file != NULL);
	return 0;
}

static int
call_fopen_fclose(void *ctx)
{
	int rv;

	FILE *file = fopen("/testfile", "rw");
	assert (file != NULL);

	rv = fclose(file);
	assert(rv == 0);

	return 0;
}

static void
test_open_close(void)
{
	int exit_code;
	rtems_status_code sc;
	rtems_resource_snapshot snapshot;

	puts("test open, socket and close");

	/* Call a first time to create all resources before taking a memory
	 * snapshot. */
	exit_code = rtems_bsd_program_call("socket", call_socket, NULL);
	assert(exit_code == 0);
	exit_code = rtems_bsd_program_call("open", call_open, NULL);
	assert(exit_code == 0);
	exit_code = rtems_bsd_program_call("fopen", call_fopen, NULL);
	assert(exit_code == 0);

	sc = rtems_task_wake_after(CK_EPOCH_LENGTH);
	assert(sc == RTEMS_SUCCESSFUL);
	rtems_resource_snapshot_take(&snapshot);

	exit_code = rtems_bsd_program_call("open", call_open, NULL);
	assert(exit_code == 0);
	assert(rtems_resource_snapshot_check(&snapshot));

	exit_code = rtems_bsd_program_call("open_close", call_open_close, NULL);
	assert(exit_code == 0);
	assert(rtems_resource_snapshot_check(&snapshot));

	rtems_resource_snapshot_take(&snapshot);

	exit_code = rtems_bsd_program_call("socket", call_socket, NULL);
	assert(exit_code == 0);
	sc = rtems_task_wake_after(CK_EPOCH_LENGTH);
	assert(sc == RTEMS_SUCCESSFUL);
	assert(rtems_resource_snapshot_check(&snapshot));

	exit_code = rtems_bsd_program_call("socket_close", call_socket_close,
	    NULL);
	assert(exit_code == 0);
	sc = rtems_task_wake_after(CK_EPOCH_LENGTH);
	assert(sc == RTEMS_SUCCESSFUL);
	assert(rtems_resource_snapshot_check(&snapshot));

	exit_code = rtems_bsd_program_call("fopen", call_fopen, NULL);
	assert(exit_code == 0);
	assert(rtems_resource_snapshot_check(&snapshot));

	exit_code = rtems_bsd_program_call("fopen_fclose", call_fopen_fclose,
	    NULL);
	assert(exit_code == 0);
	assert(rtems_resource_snapshot_check(&snapshot));
}

static int
call_alloc(void *ctx)
{
	struct alloc_ctx *context = ctx;
	char *first, *second, *third;
	const int random_size = 64;
	const char teststring[] = "test";

	switch(context->type) {
	case ALLOC_MALLOC:
		first = malloc(random_size * sizeof(int));
		second = malloc(random_size * sizeof(int));
		third = malloc(random_size * sizeof(int));
		break;
	case ALLOC_CALLOC:
		first = calloc(random_size, sizeof(int));
		second = calloc(random_size, sizeof(int));
		third = calloc(random_size, sizeof(int));
		break;
	case ALLOC_REALLOC:
		first = malloc(sizeof(int));
		second = malloc(sizeof(int));
		third = malloc(sizeof(int));
		assert(first != NULL);
		assert(second != NULL);
		assert(third != NULL);
		first = realloc(first, sizeof(int) * random_size);
		second = realloc(second, sizeof(int) * random_size);
		third = realloc(third, sizeof(int) * random_size);
		break;
	case ALLOC_STRDUP:
		first = strdup(teststring);
		second = strdup(teststring);
		third = strdup(teststring);
		break;
	case ALLOC_ASPRINTF:
		asprintf(&first, "a number %d", 0x123456);
		asprintf(&second, "some string: %s", "abcdefghijklm");
		asprintf(&third, "just something");
		break;
	default:
		assert(false);
		break;
	}

	assert(first != NULL);
	assert(second != NULL);
	assert(third != NULL);

	if((context->free & ALLOC_FREE_FIRST) != 0) {
		free(first);
	}
	if((context->free & ALLOC_FREE_SECOND) != 0) {
		free(second);
	}
	if((context->free & ALLOC_FREE_THIRD) != 0) {
		free(third);
	}

	return 0;
}

static int
call_free_on_null(void *ctx)
{
	void *new = NULL;
	free(new);
	return 0;
}

static void
test_alloc_free(void)
{
	int exit_code;
	rtems_resource_snapshot snapshot;
	struct alloc_ctx context;
	enum alloc_type type;

	puts("test alloc and free");

	rtems_resource_snapshot_take(&snapshot);

	for(type = ALLOC_MALLOC; type < ALLOC_LAST; ++type) {
		unsigned free;

		for(free = ALLOC_FREE_NONE; free < ALLOC_FREE_ALL; ++free) {
			context.type = type;
			context.free = free;

			exit_code = rtems_bsd_program_call("alloc", call_alloc,
			    &context);
			assert(exit_code == 0);
			assert(rtems_resource_snapshot_check(&snapshot));
		}
	}

	exit_code = rtems_bsd_program_call("free_on_null", call_free_on_null,
	    NULL);
	assert(exit_code == 0);
	assert(rtems_resource_snapshot_check(&snapshot));
}

static void
test_main(void)
{
	test_bsd_program();
	test_warn();
	test_err();
	test_open_close();
	test_alloc_free();

	exit(0);
}

#include <rtems/bsd/test/default-init.h>
