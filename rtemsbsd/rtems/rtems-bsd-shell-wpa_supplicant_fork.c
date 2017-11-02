/*
 * Copyright (c) 2017 Sichen Zhao.  All rights reserved.
 *
 *  <zsc19940506@gmail.com>
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

#include <rtems/netcmds-config.h>
#include <machine/rtems-bsd-commands.h>
#include <assert.h>

struct myparams {
	int argc;
	char ** argv;
};

static void
new_wpa_supplicant_task(rtems_task_argument arg)
{
	int argc;
	char ** argv;
	int i;

	struct myparams *params = (struct myparams *)arg;
	argc = params->argc;
	argv = params->argv;

	rtems_bsd_command_wpa_supplicant(argc, argv);

	for (i = 0; i < params->argc; i++) {
		free(params->argv[i]);
	}
	free(params->argv);
	free(params);

	rtems_task_delete( RTEMS_SELF );
}

int rtems_bsd_command_wpa_supplicant_fork(int argc, char **argv)
{
	rtems_status_code sc;
	rtems_id id;
	int i;

	struct myparams *params = malloc(sizeof(struct myparams));
	if (params == NULL)
		return NULL;

	params->argc = argc;
	params->argv = malloc((argc + 1) * sizeof(argv[0]));
	if (params->argv == NULL)
		return NULL;

	for (i = 0; i < argc; i++) {
		params->argv[i] = strdup(argv[i]);
		if (params->argv[i] == NULL)
			return NULL;
	}
	params->argv[argc] = NULL;

	sc = rtems_task_create(
		rtems_build_name('W', 'P', 'A', 'S'),
		RTEMS_MAXIMUM_PRIORITY - 1,
		8 * RTEMS_MINIMUM_STACK_SIZE,
		RTEMS_DEFAULT_MODES,
		RTEMS_FLOATING_POINT,
		&id
	);
	assert(sc == RTEMS_SUCCESSFUL);

	sc = rtems_task_start(id, new_wpa_supplicant_task, params);
	assert(sc == RTEMS_SUCCESSFUL);
}

rtems_shell_cmd_t rtems_shell_WPA_SUPPLICANT_FORK_Command = {
  .name = "wpa_supplicant_fork",
  .usage = "wpa_supplicant_fork [args]",
  .topic = "net",
  .command = rtems_bsd_command_wpa_supplicant_fork
};
