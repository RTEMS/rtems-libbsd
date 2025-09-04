/**
 * @file
 *
 * @brief Packet filter rules are tested (see defines below).
 */

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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>

#include <machine/rtems-bsd-commands.h>

#include <rtems/libcsupport.h>
#include <rtems/shell.h>
#include <rtems/ftpd.h>

#define TEST_NAME "LIBBSD PF 1"

#define ARGC(x) RTEMS_BSD_ARGC(x)

#define LO1_IP "172.21.1.1"
#define LO2_IP "172.22.2.2"
#define TELNET_PORT 23
#define FTP_PORT 21

#define SERVER_TASK_PRIO 110

/*
 * WARNING: The following rules are not made to be an good example for using PF.
 * Check the online manuals for PF to find out how to write good rules.
 */

/* Config with invalid syntax */
#define TEST_CFG_INVALID "/etc/pf_invalid.conf"
#define TEST_CFG_INVALID_CONTENT \
	"some\n" \
	"garbage\n"

/* Allow only some services on output. */
#define TEST_CFG_BLOCK_MOST "/etc/pf_block_most.conf"
#define TEST_CFG_BLOCK_MOST_CONTENT \
	"tcp_services = \"{ ssh, telnet, www }\"\n" \
	"udp_services = \"{ telnet }\"\n" \
	"block all\n" \
	"pass out proto tcp to any port $tcp_services keep state\n" \
	"pass proto udp to any port $udp_services keep state\n"

/* Allow all on one and only some on another interface. */
#define TEST_CFG_IF_DEPEND "/etc/pf_if_depend.conf"
#define TEST_CFG_IF_DEPEND_CONTENT \
	"ext_if = \"cgem0\"\n" \
	"int_if = \"lo0\"\n" \
	"block in all\n" \
	"pass in on $int_if all\n" \
	"pass in on $ext_if inet proto { tcp, udp } from any port { telnet }\n"

/* Block all input except on lo1. */
#define TEST_CFG_ALLOW_LO1 "/etc/pf_allow_lo1.conf"
#define TEST_CFG_ALLOW_LO1_CONTENT \
	"block all\n" \
	"pass in on lo1 all keep state\n" \
	"pass out all\n"

/* Block all input except telnet on lo1. */
#define TEST_CFG_TELNET_LO1 "/etc/pf_allow_telnet_lo1.conf"
#define TEST_CFG_TELNET_LO1_CONTENT \
	"block all\n" \
	"pass in on lo1 inet proto { tcp } from any to any port { telnet } keep state\n" \
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
	{.name = ETC_PF_OS, .content = ETC_PF_OS_CONTENT},
	{.name = ETC_PROTOCOLS, .content = ETC_PROTOCOLS_CONTENT},
	{.name = ETC_SERVICES, .content = ETC_SERVICES_CONTENT},
	{.name = TEST_CFG_INVALID, .content = TEST_CFG_INVALID_CONTENT},
	{.name = TEST_CFG_BLOCK_MOST, .content = TEST_CFG_BLOCK_MOST_CONTENT},
	{.name = TEST_CFG_IF_DEPEND, .content = TEST_CFG_IF_DEPEND_CONTENT},
	{.name = TEST_CFG_ALLOW_LO1, .content = TEST_CFG_ALLOW_LO1_CONTENT},
	{.name = TEST_CFG_TELNET_LO1, .content = TEST_CFG_TELNET_LO1_CONTENT},
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

/* Generic server task */
static rtems_task
tcp_server_task(rtems_task_argument arg)
{
	int sd, client;
	socklen_t addrlen;
	struct sockaddr_in addr, client_addr;
	uint16_t port = arg;
	int rv;
	const char answer[] = "I am a dummy\n";

	sd = socket(AF_INET, SOCK_STREAM, 0);
	assert(sd > 0);

	memset(&addr, 0, sizeof addr);
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);

	rv = bind(sd, (struct sockaddr *)&addr, sizeof(addr));
	assert(rv == 0);

	rv = listen(sd, 10);
	assert(rv == 0);

	for(;;) {
		addrlen = sizeof(client_addr);
		client = accept(sd, (struct sockaddr *)&client_addr, &addrlen);
		if(client > 0) {
			write(client, answer, strlen(answer));
			close(client);
		}
	}
}

static void
spawn_server(uint16_t port)
{
	rtems_status_code sc;
	rtems_id tid;

	sc = rtems_task_create(rtems_build_name('s','r','v',' '),
	    SERVER_TASK_PRIO,
	    RTEMS_MINIMUM_STACK_SIZE,
	    RTEMS_DEFAULT_MODES,
	    RTEMS_DEFAULT_ATTRIBUTES,
	    &tid);
	assert(sc == RTEMS_SUCCESSFUL);
	sc = rtems_task_start(tid, tcp_server_task, port);
	assert(sc == RTEMS_SUCCESSFUL);
}

/* Start a few services to test */
static void
prepare_services()
{
	spawn_server(TELNET_PORT);
	spawn_server(FTP_PORT);
}

/* Create some additional loopback interfaces for the test. */
static void
prepare_interfaces()
{
	int exit_code;
	char *lo1[] = {
		"ifconfig", "lo1", "create", "inet", LO1_IP,
		"netmask", "255.255.0.0", NULL
	};
	char *lo2[] = {
		"ifconfig", "lo2", "create", "inet", LO2_IP,
		"netmask", "255.255.0.0", NULL
	};

	puts("--- create lo1 and lo2");
	exit_code = rtems_bsd_command_ifconfig(ARGC(lo1), lo1);
	assert(exit_code == EXIT_SUCCESS);
	exit_code = rtems_bsd_command_ifconfig(ARGC(lo2), lo2);
	assert(exit_code == EXIT_SUCCESS);
}

static void
check_tcp_port(char *host, bool expect_open, uint16_t port)
{
	int sd;
	struct sockaddr_in addr;
	struct hostent *server;
	int rv;
	struct timeval tv;
	fd_set select_set;

	sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	assert(sd > 0);

	/* make socket nonblocking */
	rv = fcntl(sd, F_GETFL, NULL);
	assert(rv >= 0);
	rv |= O_NONBLOCK;
	rv = fcntl(sd, F_SETFL, rv);
	assert(rv == 0);

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);

	server = gethostbyname(host);
	assert(server != NULL);
	memcpy(&addr.sin_addr.s_addr, server->h_addr, server->h_length);

	rv = connect(sd, (struct sockaddr *)&addr, sizeof(addr));
	if(rv == 0) {
		/* successfully connected */
		assert(expect_open == true);
	} else {
		if(errno != EINPROGRESS) {
			assert(expect_open == false);
		} else {
			/* wait for connection or timeout */
			memset(&tv, 0, sizeof(tv));
			tv.tv_usec = 100000;

			FD_ZERO(&select_set);
			FD_SET(sd, &select_set);
			rv = select(sd+1, NULL, &select_set, NULL, &tv);

			/* rv < 0 would be an error
			 * rv == 0 would be an timeout
			 * rv > 0 would be success */
			if(expect_open == false) {
				assert(rv == 0);
			} else {
				assert(rv > 0);
			}
		}
	}

	rv = close(sd);
	assert(rv == 0);
}

static void
check_telnet(char *host, bool expect_open)
{
	check_tcp_port(host, expect_open, TELNET_PORT);
}

static void
check_ftp(char *host, bool expect_open)
{
	check_tcp_port(host, expect_open, FTP_PORT);
}

/* Check if services work like expected */
static void
check_services(char *host, bool exp_telnet, bool exp_ftp)
{
	check_telnet(host, exp_telnet);
	check_ftp(host, exp_ftp);
}

static void
disable_pf(bool ignore_not_enabled)
{
	int exit_code;
	char *pfctl[] = {"pfctl", "-d", "-q", NULL};

	exit_code = rtems_bsd_command_pfctl(ARGC(pfctl), pfctl);
	assert(ignore_not_enabled || (exit_code == EXIT_SUCCESS));
}

static void
enable_pf()
{
	int exit_code;
	char *pfctl[] = {"pfctl", "-e", "-q", NULL};

	exit_code = rtems_bsd_command_pfctl(ARGC(pfctl), pfctl);
	assert(exit_code == EXIT_SUCCESS);
}

/* Execute pfctl two times with the given arguments. Check the resources. */
static void
run_pfctl(int argc, char *argv[], int expected_result)
{
	int exit_code;
	rtems_resource_snapshot snapshot, snapshot2;

	exit_code = rtems_bsd_command_pfctl(argc, argv);
	assert(exit_code == expected_result);

	rtems_resource_snapshot_take(&snapshot);
	exit_code = rtems_bsd_command_pfctl(argc, argv);
	assert(exit_code == expected_result);
	rtems_resource_snapshot_take(&snapshot2);

	/* heap fragmentation might change so remove it from compare */
	snapshot.workspace_info.Free.largest = 0;
	snapshot2.workspace_info.Free.largest = 0;
	snapshot.heap_info.Free.largest = 0;
	snapshot2.heap_info.Free.largest = 0;
	assert(exit_code == EXIT_SUCCESS ||
	    rtems_resource_snapshot_equal(&snapshot, &snapshot2));
}

static void
test_pfctl(void)
{
	char *no_params[] = {
		"pfctl",
		NULL
	};
	char *invalid[] = {
		"pfctl",
		"-f",
		TEST_CFG_INVALID,
		"-q",
		NULL
	};
	char *block_most[] = {
		"pfctl",
		"-f",
		TEST_CFG_BLOCK_MOST,
		"-q",
		NULL
	};
	char *if_depend[] = {
		"pfctl",
		"-f",
		TEST_CFG_IF_DEPEND,
		"-q",
		NULL
	};

	puts("--- run pfctl with no paramters");
	run_pfctl(ARGC(no_params), no_params, EXIT_FAILURE);

	puts("--- load invalid rule");
	run_pfctl(ARGC(invalid), invalid, EXIT_FAILURE);

	puts("--- load rule to block input and most output");
	run_pfctl(ARGC(block_most), block_most, EXIT_SUCCESS);

	puts("--- load rule to block if dependent");
	run_pfctl(ARGC(if_depend), if_depend, EXIT_SUCCESS);
}

static void
test_allow_lo1()
{
	char *pfctl[] = {
		"pfctl",
		"-f",
		TEST_CFG_ALLOW_LO1,
		"-q",
		NULL
	};

	disable_pf(true);

	puts("--- check services");
	check_services(LO1_IP, true, true);
	check_services(LO2_IP, true, true);

	puts("--- load and enable rule to allow only lo1");
	run_pfctl(ARGC(pfctl), pfctl, EXIT_SUCCESS);
	enable_pf();

	puts("--- check services");
	check_services(LO1_IP, true, true);
	check_services(LO2_IP, false, false);
}

static void
test_telnet_lo1()
{
	char *pfctl[] = {
		"pfctl",
		"-f",
		TEST_CFG_TELNET_LO1,
		"-q",
		NULL
	};

	disable_pf(true);

	puts("--- check services");
	check_services(LO1_IP, true, true);
	check_services(LO2_IP, true, true);

	puts("--- load and enable rule to allow telnet on lo1");
	run_pfctl(ARGC(pfctl), pfctl, EXIT_SUCCESS);
	enable_pf();

	puts("--- check services");
	check_services(LO1_IP, true, false);
	check_services(LO2_IP, false, false);
}

static void
test_main(void)
{
	prepare_files();
	prepare_interfaces();
	prepare_services();

	test_pfctl();

	test_allow_lo1();
	test_telnet_lo1();

	exit(0);
}

#include <machine/rtems-bsd-sysinit.h>

#define RTEMS_BSD_CONFIG_FIREWALL_PF

#include <rtems/bsd/test/default-network-init.h>
