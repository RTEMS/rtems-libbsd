/*
 * Copyright (c) 2013, 2018 embedded brains GmbH.  All rights reserved.
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

#include <rtems/netcmds-config.h>

#include <rtems.h>
#include <rtems/dhcpcd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	rtems_dhcpcd_config config;
	char *argv[RTEMS_ZERO_LENGTH_ARRAY];
} dhcpcd_command_config;

static void
dhcpcd_command_destroy_config(const rtems_dhcpcd_config *config, int exit_code)
{
	char **argv;

	(void)exit_code;

	argv = config->argv;
	while (*argv != NULL) {
		free(*argv);
		++argv;
	}

	free(RTEMS_DECONST(rtems_dhcpcd_config *, config));
}

static int
dhcpcd_command(int argc, char **argv)
{
	dhcpcd_command_config *config;
	rtems_status_code sc;
	int i;

	config = calloc(1, sizeof(*config) + (argc + 1) * sizeof(char *));
	if (config == NULL) {
		fprintf(stdout, "dhcpcd error: not enough memory\n");
		return 1;
	}

	for (i = 0; i < argc; ++i) {
		config->argv[i] = strdup(argv[i]);
		if (config->argv[i] == NULL) {
			dhcpcd_command_destroy_config(&config->config, 0);
			fprintf(stdout, "dhcpcd error: not enough memory\n");
			return 1;
		}
	}

	config->config.argc = argc;
	config->config.argv = &config->argv[0];
	config->config.destroy = dhcpcd_command_destroy_config;

	sc = rtems_dhcpcd_start(&config->config);
	if (sc != RTEMS_SUCCESSFUL) {
		dhcpcd_command_destroy_config(&config->config, 0);
		fprintf(stdout, "dhcpcd start failed: %s\n", rtems_status_text(sc));
	}

	return 0;
}

rtems_shell_cmd_t rtems_shell_DHCPCD_Command = {
  .name = "dhcpcd",
  .usage = "dhcpcd [args]",
  .topic = "net",
  .command = dhcpcd_command
};
