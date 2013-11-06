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
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/proc.h>
#include <sys/malloc.h>
#include <rtems/bsd/sys/lock.h>
#include <sys/mutex.h>

#include <setjmp.h>
#include <stdlib.h>

struct rtems_bsd_program_control {
	void *context;
	int exit_code;
	const char *name;
	jmp_buf return_context;
};

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

				if (setjmp(prog_ctrl->return_context) == 0) {
					exit_code = (*prog)(context);
				} else {
					exit_code = prog_ctrl->exit_code;
				}

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
