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

#include <assert.h>
#include <fcntl.h>
#include <stdlib.h>

#include <rtems/telnetd.h>
#include <rtems/ftpd.h>
#include <rtems/shell.h>

#define TEST_NAME "LIBBSD PF 2"

/* Block all input except telnet. */
#define ETC_PF_CONF "/etc/pf.conf"
#define ETC_PF_CONF_CONTENT \
	"block all\n" \
	"pass in inet proto { tcp } from any to any port { telnet } keep state\n" \
	"pass out all\n"

/* pf.os */
#define ETC_PF_OS "/etc/pf.os"
#define ETC_PF_OS_CONTENT "# empty"

/* protocols */
#define ETC_PROTOCOLS "/etc/protocols"
#define ETC_PROTOCOLS_CONTENT \
	"ip	0	IP		# internet protocol, pseudo protocol number\n" \
	"tcp	6	TCP		# transmission control protocol\n" \
	"udp	17	UDP		# user datagram protocol\n"

/* services */
#define ETC_SERVICES "/etc/services"
#define ETC_SERVICES_CONTENT \
	"ftp-data	 20/sctp   #File Transfer [Default Data]\n" \
	"ftp-data	 20/tcp	   #File Transfer [Default Data]\n" \
	"ftp-data	 20/udp	   #File Transfer [Default Data]\n" \
	"ftp		 21/sctp   #File Transfer [Control]\n" \
	"ftp		 21/tcp	   #File Transfer [Control]\n" \
	"ftp		 21/udp	   #File Transfer [Control]\n" \
	"ssh		 22/tcp	   #Secure Shell Login\n" \
	"telnet		 23/tcp\n" \
	"telnet		 23/udp\n" \
	"http		 80/tcp	   www www-http	#World Wide Web HTTP\n"

static const struct {
	const char *name;
	const char *content;
} init_files[] = {
	{.name = ETC_PF_CONF, .content = ETC_PF_CONF_CONTENT},
	{.name = ETC_PF_OS, .content = ETC_PF_OS_CONTENT},
	{.name = ETC_PROTOCOLS, .content = ETC_PROTOCOLS_CONTENT},
	{.name = ETC_SERVICES, .content = ETC_SERVICES_CONTENT},
};

/* Create all necessary files */
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
telnet_shell(char *name, void *arg)
{
	rtems_shell_env_t env;

	memset(&env, 0, sizeof(env));

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

struct rtems_ftpd_configuration rtems_ftpd_configuration = {
	.priority = 100,
	.max_hook_filesize = 0,
	.port = 21,
	.hooks = NULL,
	.root = NULL,
	.tasks_count = 4,
	.idle = 5 * 60,
	.access = 0
};

static void
test_main(void)
{
	rtems_status_code sc;
	int rv;

	prepare_files();

	sc = rtems_telnetd_initialize();
	assert(sc == RTEMS_SUCCESSFUL);

	rv = rtems_initialize_ftpd();
	assert(rv == 0);

	rtems_shell_env_t env;

	memset(&env, 0, sizeof(env));
	rtems_shell_main_loop( &env );

	exit(0);
}

#include <machine/rtems-bsd-sysinit.h>

SYSINIT_NEED_FIREWALL_PF;
SYSINIT_NEED_FIREWALL_PFLOG;
#define CONFIGURE_MAXIMUM_DRIVERS 32

#include <rtems/bsd/test/default-network-init.h>

/* Shell config */
#include <rtems/netcmds-config.h>

#define CONFIGURE_SHELL_COMMANDS_INIT
#define CONFIGURE_SHELL_COMMANDS_ALL

#define CONFIGURE_SHELL_USER_COMMANDS \
    &rtems_shell_PING_Command, \
    &rtems_shell_IFCONFIG_Command, \
    &rtems_shell_PFCTL_Command

#include <rtems/shellconfig.h>
