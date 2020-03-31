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
#include <machine/rtems-bsd-rc-conf.h>

#include <rtems/bsd/bsd.h>
#include <rtems/bsd/test/network-config.h>
#include <rtems/console.h>
#include <rtems/shell.h>


#define IPSEC_LOC_INT "10.10.1.1"
#define IPSEC_LOC_NET "10.10.1.0/24"
#define IPSEC_LOC_EXT "192.168.10.1"
#define IPSEC_REM_INT "172.24.0.1"
#define IPSEC_REM_NET "172.24.0.0/24"
#define IPSEC_REM_EXT "192.168.10.10"
#define RACOON_PSK_FILE "/etc/racoon_psk.txt"
#define RACOON_CONFIG_FILE "/etc/racoon.conf"
#define SETKEY_CONFIG_FILE "/etc/setkey.conf"
#define RC_CONF "/etc/rc.conf"

static const char racoon_psk[] =
	IPSEC_REM_EXT "	mysecretkey\n";
static const char racoon_config[] =
	"path	pre_shared_key \"" RACOON_PSK_FILE "\";\n"
	"log	debug;\n"
	"\n"
	"padding	# options are not to be changed\n"
	"{\n"
	"	maximum_length			20;\n"
	"	randomize			off;\n"
	"	strict_check			off;\n"
	"	exclusive_tail			off;\n"
	"}\n"
	"\n"
	"listen	# address [port] that racoon will listen on\n"
	"{\n"
	"	isakmp				" IPSEC_LOC_EXT "[500];\n"
	"}\n"
	"\n"
	"remote " IPSEC_REM_EXT " [500]\n"
	"{\n"
	"	exchange_mode			main;\n"
	"	my_identifier			address " IPSEC_LOC_EXT ";\n"
	"	peers_identifier		address " IPSEC_REM_EXT ";\n"
	"	proposal_check			obey;\n"
	"\n"
	"	proposal {\n"
	"		encryption_algorithm	3des;\n"
	"		hash_algorithm		md5;\n"
	"		authentication_method	pre_shared_key;\n"
	"		lifetime time		3600 sec;\n"
	"		dh_group		2;\n"
	"	}\n"
	"}\n"
	"\n"
	"sainfo (address " IPSEC_LOC_NET " any address " IPSEC_REM_NET " any)\n"
	"{\n"
	"	pfs_group			2;\n"
	"	lifetime			time 28800 sec;\n"
	"	encryption_algorithm		3des;\n"
	"	authentication_algorithm	hmac_md5;\n"
	"	compression_algorithm		deflate;\n"
	"}\n";
static const char setkey_config[] =
	"flush;\n"
	"spdflush;\n"
	"spdadd " IPSEC_LOC_NET " " IPSEC_REM_NET " any -P out ipsec esp/tunnel/" IPSEC_LOC_EXT "-" IPSEC_REM_EXT"/use;\n"
	"spdadd " IPSEC_REM_NET " " IPSEC_LOC_NET " any -P in  ipsec esp/tunnel/" IPSEC_REM_EXT "-" IPSEC_LOC_EXT"/use;\n";
static const char rc_conf[] =
	"cloned_interfaces=\"gif0\"\n"
	"ifconfig_gif0=\"10.10.1.1 172.24.0.1 tunnel 192.168.10.1 192.168.10.10\"\n"
	"ike_enable=\"YES\"\n"
	"ike_program=\"racoon\"\n"
	"ike_flags=\"-F -f /etc/racoon.conf\"\n"
	"ike_priority=\"250\"\n"
	"\n"
	"ipsec_enable=\"YES\"\n"
	"ipsec_file=\"/etc/setkey.conf\"\n"
	"\n"
	"ifconfig_" NET_CFG_INTERFACE_0 "=\"inet " IPSEC_LOC_EXT " netmask 255.255.255.0\"\n";

static const struct {
	const char *name;
	const char *content;
} init_files[] = {
	{.name = RACOON_PSK_FILE, .content = racoon_psk},
	{.name = RACOON_CONFIG_FILE, .content = racoon_config},
	{.name = SETKEY_CONFIG_FILE, .content = setkey_config},
	{.name = RC_CONF, .content = rc_conf},
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

	rv = rtems_bsd_run_rc_conf(RC_CONF, 15, true);
	assert(rv == 0);

	/* Wait for initial racoon messages. */
	sleep(2);
	puts("--------------------------------------------------");
	puts("Everything should be prepared now.");
	puts("As soon as you communicate with someone in " IPSEC_REM_NET
	    " the IPSEC connection should be established.");
	puts("--------------------------------------------------");

	sc = rtems_shell_init("SHLL", 32 * 1024, 1, CONSOLE_DEVICE_NAME,
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

#define RTEMS_BSD_CONFIG_IPSEC
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
