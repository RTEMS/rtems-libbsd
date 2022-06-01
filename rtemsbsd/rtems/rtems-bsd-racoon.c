/*
 * Copyright (c) 2018 embedded brains GmbH.  All rights reserved.
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

#include <rtems.h>
#include <rtems/thread.h>
#include <rtems/netcmds-config.h>
#include <machine/rtems-bsd-commands.h>

#include <string.h>
#include <stdlib.h>
#include <syslog.h>

struct argc_argv {
	size_t argc;
	char **argv;
};

static bool racoon_initialized = false;
rtems_recursive_mutex racoon_mutex =
    RTEMS_RECURSIVE_MUTEX_INITIALIZER("racoon");

static void
clean_up_args(struct argc_argv *args)
{
	if (args != NULL) {
		if (args->argv != NULL) {
			for (int i = 0; i < args->argc; ++i) {
				if (args->argv[i] != NULL) {
					free(args->argv[i]);
				}
			}
			free(args->argv);
		}
		free(args);
	}
}

static void
racoon_task(rtems_task_argument arg)
{
	int exit_code;
	struct argc_argv *args = (struct argc_argv *) arg;

	exit_code = rtems_bsd_command_racoon(args->argc, args->argv);
	if (exit_code != EXIT_SUCCESS) {
		syslog(LOG_ERR, "racoon exited with %d\n", exit_code);
	}

	clean_up_args(args);
	rtems_task_exit();
}

rtems_status_code
rtems_bsd_racoon_daemon(int argc, const char **argv, rtems_task_priority prio)
{
	rtems_id id;
	rtems_status_code sc = RTEMS_SUCCESSFUL;
	struct argc_argv *args = NULL;

	rtems_recursive_mutex_lock(&racoon_mutex);

	if (racoon_initialized) {
		sc = RTEMS_INCORRECT_STATE;
	}

	if (sc == RTEMS_SUCCESSFUL) {
		args = malloc(sizeof(struct argc_argv));
		if (args == NULL) {
			sc = RTEMS_NO_MEMORY;
		}
	}

	if (sc == RTEMS_SUCCESSFUL) {
		args->argc = argc;
		args->argv = calloc(argc + 1, sizeof(char*));
		if (args->argv == NULL) {
			sc = RTEMS_NO_MEMORY;
		}
	}

	if (sc == RTEMS_SUCCESSFUL) {
		for (int i = 0; i < argc; ++i) {
			args->argv[i] = strdup(argv[i]);
			if (args->argv[i] == NULL) {
				sc = RTEMS_NO_MEMORY;
			}
		}
	}

	if (sc == RTEMS_SUCCESSFUL) {
		sc = rtems_task_create(
			rtems_build_name('R', 'A', 'C', 'N'),
			prio,
			32 * 1024,
			RTEMS_DEFAULT_MODES,
			RTEMS_FLOATING_POINT,
			&id
		);
	}

	if (sc == RTEMS_SUCCESSFUL) {
		sc = rtems_task_start(id, racoon_task,
		    (rtems_task_argument)args);
	}

	if (sc == RTEMS_SUCCESSFUL) {
		racoon_initialized = true;
	} else {
		clean_up_args(args);
	}

	rtems_recursive_mutex_unlock(&racoon_mutex);

	return (sc);
}
