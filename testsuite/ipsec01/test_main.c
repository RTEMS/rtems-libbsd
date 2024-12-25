/**
 * @file
 *
 * @brief It tests Internet Protocol Security protocol (ipsec) using
 *        setkey tool.
 */

/*
 * Copyright (c) 2024 Ning Yang
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

#define TEST_NAME "LIBBSD IPSEC 1"
#define TEST_STATE_USER_INPUT 1

#include <stdio.h>
#include <stdlib.h>

#include <rtems/bsd/modules.h>
#ifdef RTEMS_BSD_MODULE_NETIPSEC

#include <assert.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include <machine/rtems-bsd-commands.h>

#include <rtems/bsd/bsd.h>
#include <rtems/bsd/test/network-config.h>
#include <rtems/console.h>
#include <rtems/shell.h>

#define LOCAL_IP "169.254.1.2"
#define REMOTE_IP "169.254.1.1"
#define SETKEY_CONFIG_FILE "/etc/setkey.conf"

static const char setkey_config[] =
	"flush;\n"
	"spdflush;\n"
	"add "LOCAL_IP" "REMOTE_IP" esp 29579 -m tunnel -E aes-cbc 0x9086d234780b898efab61237874345789885b87c076cccf9;\n"
	"add "REMOTE_IP" "LOCAL_IP" esp 29578 -m tunnel -E aes-cbc 0x9086d234780b898efab61237874345789885b87c076cccf8;\n"
	"spdadd "REMOTE_IP" "LOCAL_IP" any -P in ipsec esp/tunnel/"REMOTE_IP"-"LOCAL_IP"/use;\n"
	"spdadd "LOCAL_IP" "REMOTE_IP" any -P out ipsec esp/tunnel/"LOCAL_IP"-"REMOTE_IP"/use;\n";

static const struct {
	const char *name;
	const char *content;
} init_files[] = {
	{.name = SETKEY_CONFIG_FILE, .content = setkey_config}
};

static void
prepare_files()
{
	size_t i;
	struct stat sb;
	int rv;
	int fd;
	size_t written;

	/* Create /etc if necessary */
	rv = mkdir("/etc", S_IRWXU | S_IRWXG | S_IRWXO);
	/* ignore errors, check the dir after. */
	assert(stat("/etc", &sb) == 0);
	assert(S_ISDIR(sb.st_mode));

	/* Create files */
	for(i = 0; i < (sizeof(init_files)/sizeof(init_files[0])); ++i) {
		const char *content;
		size_t len;

		content = init_files[i].content;
		len = strlen(content);

		fd = open(init_files[i].name, O_WRONLY | O_CREAT,
		    S_IRWXU | S_IRWXG | S_IRWXO);
		assert(fd != -1);

		written = write(fd, content, len);
		assert(written == len);

		rv = close(fd);
		assert(rv == 0);
	}
}

static void
test_main(void)
{
	int rv;
	rtems_status_code sc;

	sc = rtems_shell_init("SHLL", 16 * 1024, 1, CONSOLE_DEVICE_NAME,
	    false, true, NULL);
	assert(sc == RTEMS_SUCCESSFUL);

	exit(0);
}

#define DEFAULT_EARLY_INITIALIZATION

static void
early_initialization(void)
{
	prepare_files();
}

#include <machine/rtems-bsd-sysinit.h>

/* Software crypto should work on all devices. */
RTEMS_BSD_DEFINE_NEXUS_DEVICE(cryptosoft, 0, 0, NULL);
#define RTEMS_BSD_CONFIG_BSP_CONFIG

#include <rtems/bsd/test/default-init.h>

#define CONFIGURE_SHELL_COMMANDS_INIT
#include <rtems/netcmds-config.h>

#define CONFIGURE_SHELL_USER_COMMANDS \
  &rtems_shell_ARP_Command, \
  &rtems_shell_HOSTNAME_Command, \
  &rtems_shell_PING_Command, \
  &rtems_shell_ROUTE_Command, \
  &rtems_shell_NETSTAT_Command, \
  &rtems_shell_SYSCTL_Command, \
  &rtems_shell_IFCONFIG_Command, \
  &rtems_shell_VMSTAT_Command, \
  &rtems_shell_SETKEY_Command

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

#include <rtems/shellconfig.h>

#else /* RTEMS_BSD_MODULE_NETIPSEC */

static void
test_main(void)
{
	puts("IPSec not enabled in the current build set.");
	exit(0);
}

#include <rtems/bsd/test/default-init.h>

#endif /* RTEMS_BSD_MODULE_NETIPSEC */
