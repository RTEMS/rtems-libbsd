/**
 * @file
 *
 * @brief Code used to test the Media Manager.
 *
 * Telnet daemon (telnetd) and FTP daemon (ftpd) are started. Events are recorded.
 */

/*
 * Copyright (c) 2010-2016 embedded brains GmbH.  All rights reserved.
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

#include <sys/param.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <rtems/bdbuf.h>
#include <rtems/console.h>
#include <rtems/ftpd.h>
#include <rtems/media.h>
#include <rtems/record.h>
#include <rtems/recordserver.h>
#include <rtems/shell.h>
#include <rtems/telnetd.h>

#define TEST_NAME "LIBBSD MEDIA 1"
#define TEST_STATE_USER_INPUT 1

struct rtems_ftpd_configuration rtems_ftpd_configuration = {
	/* FTPD task priority */
	.priority = 100,

	/* Maximum buffersize for hooks */
	.max_hook_filesize = 0,

	/* Well-known port */
	.port = 21,

	/* List of hooks */
	.hooks = NULL,

	/* Root for FTPD or NULL for "/" */
	.root = NULL,

	/* Max. connections depending on processor count */
	.tasks_count = 0,

	/* Idle timeout in seconds  or 0 for no (infinite) timeout */
	.idle = 5 * 60,

	/* Access: 0 - r/w, 1 - read-only, 2 - write-only, 3 - browse-only */
	.access = 0
};

static rtems_status_code
media_listener(rtems_media_event event, rtems_media_state state,
    const char *src, const char *dest, void *arg)
{
	if (dest == NULL) {
		dest = "NULL";
	}

	printf(
		"media listener: event = %s, state = %s, src = %s, dest = %s\n",
		rtems_media_event_description(event),
		rtems_media_state_description(state),
		src,
		dest
	);

	if (event == RTEMS_MEDIA_EVENT_MOUNT && state == RTEMS_MEDIA_STATE_SUCCESS) {
		char name[256];
		int n = snprintf(&name[0], sizeof(name), "%s/test.txt", dest);
		FILE *file;

		assert(n < (int) sizeof(name));

		printf("write file %s\n", &name[0]);
		file = fopen(&name[0], "w");
		if (file != NULL) {
			const char hello[] = "Hello, world!\n";

			fwrite(&hello[0], sizeof(hello) - 1, 1, file);
			fclose(file);
		}
	}

	return RTEMS_SUCCESSFUL;
}

static void
telnet_shell(char *name, void *arg)
{
	rtems_shell_env_t env;

	rtems_shell_dup_current_env(&env);

	env.devname = name;
	env.taskname = "TLNT";
	env.login_check = NULL;
	env.forever = false;

	rtems_shell_main_loop(&env);
}

rtems_telnetd_config_table rtems_telnetd_config = {
	.command = telnet_shell,
	.arg = NULL,
	.priority = 0,
	.stack_size = 0,
	.login_check = NULL,
	.keep_stdio = false
};

static void
test_main(void)
{
	int rv;
	rtems_status_code sc;

	rtems_ftpd_configuration.tasks_count = MAX(4,
	    rtems_scheduler_get_processor_maximum());
	rv = rtems_initialize_ftpd();
	assert(rv == 0);

	sc = rtems_telnetd_initialize();
	assert(sc == RTEMS_SUCCESSFUL);

	sc = rtems_record_start_server(1, 1234, 1);
	assert(sc == RTEMS_SUCCESSFUL);

	sc = rtems_shell_init("SHLL", 16 * 1024, 1, CONSOLE_DEVICE_NAME,
	    false, true, NULL);
	assert(sc == RTEMS_SUCCESSFUL);

	exit(0);
}

#define DEFAULT_EARLY_INITIALIZATION

static void
early_initialization(void)
{
	rtems_status_code sc;

	sc = rtems_bdbuf_init();
	assert(sc == RTEMS_SUCCESSFUL);

	sc = rtems_media_initialize();
	assert(sc == RTEMS_SUCCESSFUL);

	sc = rtems_media_listener_add(media_listener, NULL);
	assert(sc == RTEMS_SUCCESSFUL);

	sc = rtems_media_server_initialize(
		200,
		32 * 1024,
		RTEMS_DEFAULT_MODES,
		RTEMS_DEFAULT_ATTRIBUTES
	);
	assert(sc == RTEMS_SUCCESSFUL);
}

#define DEFAULT_NETWORK_DHCPCD_ENABLE

#define CONFIGURE_MICROSECONDS_PER_TICK 1000

#define CONFIGURE_MAXIMUM_DRIVERS 32

#define CONFIGURE_FILESYSTEM_DOSFS

#define CONFIGURE_MAXIMUM_PROCESSORS 32

#define CONFIGURE_RECORD_PER_PROCESSOR_ITEMS 4096

#define CONFIGURE_RECORD_EXTENSIONS_ENABLED

#include <rtems/bsd/test/default-network-init.h>

#define CONFIGURE_SHELL_COMMANDS_INIT

#include <bsp/irq-info.h>

#include <rtems/netcmds-config.h>

#ifdef RTEMS_BSD_MODULE_USR_SBIN_WPA_SUPPLICANT
  #define SHELL_WPA_SUPPLICANT_COMMANDS \
    &rtems_shell_WPA_SUPPLICANT_Command, \
    &rtems_shell_WPA_SUPPLICANT_FORK_Command,
#else
  #define SHELL_WPA_SUPPLICANT_COMMANDS
#endif

#ifdef RTEMS_BSD_MODULE_NETIPSEC
  #define SHELL_NETIPSEC_COMMANDS \
    &rtems_shell_RACOON_Command, \
    &rtems_shell_SETKEY_Command,
#else
  #define SHELL_NETIPSEC_COMMANDS
#endif

#define CONFIGURE_SHELL_USER_COMMANDS \
  SHELL_WPA_SUPPLICANT_COMMANDS \
  SHELL_NETIPSEC_COMMANDS \
  &bsp_interrupt_shell_command, \
  &rtems_shell_ARP_Command, \
  &rtems_shell_HOSTNAME_Command, \
  &rtems_shell_PING_Command, \
  &rtems_shell_ROUTE_Command, \
  &rtems_shell_NETSTAT_Command, \
  &rtems_shell_SYSCTL_Command, \
  &rtems_shell_IFCONFIG_Command, \
  &rtems_shell_IFMCSTAT_Command, \
  &rtems_shell_VMSTAT_Command

#define CONFIGURE_SHELL_COMMAND_CPUINFO
#define CONFIGURE_SHELL_COMMAND_CPUUSE
#define CONFIGURE_SHELL_COMMAND_PERIODUSE
#define CONFIGURE_SHELL_COMMAND_STACKUSE
#define CONFIGURE_SHELL_COMMAND_PROFREPORT

#define CONFIGURE_SHELL_COMMAND_CP
#define CONFIGURE_SHELL_COMMAND_PWD
#define CONFIGURE_SHELL_COMMAND_LS
#define CONFIGURE_SHELL_COMMAND_LN
#define CONFIGURE_SHELL_COMMAND_LSOF
#define CONFIGURE_SHELL_COMMAND_CHDIR
#define CONFIGURE_SHELL_COMMAND_CD
#define CONFIGURE_SHELL_COMMAND_MKDIR
#define CONFIGURE_SHELL_COMMAND_RMDIR
#define CONFIGURE_SHELL_COMMAND_CAT
#define CONFIGURE_SHELL_COMMAND_MV
#define CONFIGURE_SHELL_COMMAND_RM
#define CONFIGURE_SHELL_COMMAND_MALLOC_INFO

#define CONFIGURE_SHELL_COMMAND_FDISK
#define CONFIGURE_SHELL_COMMAND_BLKSTATS
#define CONFIGURE_SHELL_COMMAND_BLKSYNC
#define CONFIGURE_SHELL_COMMAND_MSDOSFMT
#define CONFIGURE_SHELL_COMMAND_DF
#define CONFIGURE_SHELL_COMMAND_MOUNT
#define CONFIGURE_SHELL_COMMAND_UNMOUNT
#define CONFIGURE_SHELL_COMMAND_MSDOSFMT

#include <rtems/shellconfig.h>
