/**
 * @file
 *
 * @brief A rc.conf script is generated.
 */

/*
 * Copyright 2026 Chris Johns <chrisj@rtems.org>
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
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sysexits.h>
#include <unistd.h>

#include <machine/rtems-bsd-commands.h>
#include <machine/rtems-bsd-rc-conf-env.h>

#include <rtems/bsd/test/network-config.h>

#include <rtems/console.h>
#include <rtems/shell.h>

#define TEST_END_IN_SHELL 0

#define TEST_NAME "LIBBSD RCCONF03"

struct env_var {
  const char* key;
  const char* val;
};

static struct env_var test1_no_dhcp[] = {
  { "RTEMS_NET_HOSTNAME", "h1" },
  { "RTEMS_NET_IFACE_1", "if1" },
  { "RTEMS_NET_IFACE_2", "if2" },
  { "RTEMS_NET_IFACE_3", "if3" },
  { "RTEMS_NET_IFACE_4", "if4" },
  { "RTEMS_NET_IF_1_MAC", "00:01:02:03:04:05" },
  { "RTEMS_NET_IF_2_MAC", "05:04:03:02:01:00" },
  { "RTEMS_NET_IF_1_OPTS", "opt1" },
  { "RTEMS_NET_IF_2_OPTS", "opt2" },
  { "RTEMS_NET_IF_3_OPTS", "opt3" },
  { "RTEMS_NET_IF_4_OPTS", "opt4" },
  { "RTEMS_NET_IF_1_IP_ADDR", "1.1.1.1" },
  { "RTEMS_NET_IF_2_IP_ADDR", "2.2.2.2" },
  { "RTEMS_NET_IF_3_IP_ADDR", "3.3.3.3" },
  { "RTEMS_NET_IF_4_IP_ADDR", "4.4.4.4" },
  { "RTEMS_NET_IF_1_NETMASK", "255.255.255.0" },
  { "RTEMS_NET_IF_2_NETMASK", "255.255.250.0" },
  { "RTEMS_NET_IF_3_NETMASK", "255.255.192.0" },
  { "RTEMS_NET_IF_4_NETMASK", "255.255.128.0" },
  { "RTEMS_NET_GATEWAY_IP", "9.9.9.9" },
};

static const char* test1_rc_conf =
  "hostname=\"h1\"\n"
  "ifconfig_if1=\"inet 1.1.1.1 netmask 255.255.255.0 opt1\"\n"
  "ifconfig_if1_alias0=\"ether 00:01:02:03:04:05\"\n"
  "ifconfig_if2=\"inet 2.2.2.2 netmask 255.255.250.0 opt2\"\n"
  "ifconfig_if2_alias0=\"ether 05:04:03:02:01:00\"\n"
  "ifconfig_if3=\"inet 3.3.3.3 netmask 255.255.192.0 opt3\"\n"
  "ifconfig_if4=\"inet 4.4.4.4 netmask 255.255.128.0 opt4\"\n"
  "defaultrouter=\"9.9.9.9\"\n";

static struct env_var test2_dhcp[] = {
  { "RTEMS_NET_HOSTNAME", "h1" },
  { "RTEMS_NET_IFACE_1", "if1" },
  { "RTEMS_NET_IFACE_2", "if2" },
  { "RTEMS_NET_IFACE_3", "if3" },
  { "RTEMS_NET_IFACE_4", "if4" },
  { "RTEMS_NET_IF_1_MAC", "00:01:02:03:04:05" },
  { "RTEMS_NET_IF_2_MAC", "05:04:03:02:01:00" },
  { "RTEMS_NET_IF_1_OPTS", "opt1" },
  { "RTEMS_NET_IF_2_OPTS", "opt2" },
  { "RTEMS_NET_IF_3_OPTS", "opt3" },
  { "RTEMS_NET_IF_4_OPTS", "opt4" },
  { "RTEMS_NET_IF_1_IP_ADDR", "1.1.1.1" },
  { "RTEMS_NET_IF_4_IP_ADDR", "4.4.4.4" },
  { "RTEMS_NET_IF_1_NETMASK", "255.255.255.0" },
  { "RTEMS_NET_IF_4_NETMASK", "255.255.128.0" },
  { "RTEMS_NET_GATEWAY_IP", "9.9.9.9" },
};

static const char* test2_rc_conf =
  "hostname=\"h1\"\n"
  "ifconfig_if1=\"inet 1.1.1.1 netmask 255.255.255.0 opt1\"\n"
  "ifconfig_if1_alias0=\"ether 00:01:02:03:04:05\"\n"
  "ifconfig_if2=\"DHCP opt2\"\n"
  "ifconfig_if2_alias0=\"ether 05:04:03:02:01:00\"\n"
  "ifconfig_if3=\"DHCP opt3\"\n"
  "ifconfig_if4=\"inet 4.4.4.4 netmask 255.255.128.0 opt4\"\n"
  "dhcpcd_priority=\"123\"\n"
  "dhcpcd_options=\"--debug --nobackground --timeout 60\"\n"\
  "defaultrouter=\"9.9.9.9\"\n";

static struct env_var test3_resolv[] = {
  { "RTEMS_NET_DOMAIN_NAME", "my.domain.name" },
  { "RTEMS_NET_DNS_IP", "8.8.8.8" },
};

static const char* test3_dns =
  "domain my.domain.name\n"
  "nameserver 8.8.8.8\n";

static struct env_var test4_no_iface[] = {
  { "RTEMS_NET_IFACE_1", "" },
  { "RTEMS_NET_IF_1_MAC", "00:01:02:03:04:05" },
  { "RTEMS_NET_IF_1_OPTS", "opt1" },
  { "RTEMS_NET_IF_1_IP_ADDR", "1.1.1.1" },
  { "RTEMS_NET_IF_1_NETMASK", "255.255.255.0" },
};

static const char* all_vars[] = {
  "RTEMS_NET_HOSTNAME",
  "RTEMS_NET_IFACE_1",
  "RTEMS_NET_IFACE_2",
  "RTEMS_NET_IFACE_3",
  "RTEMS_NET_IFACE_4",
  "RTEMS_NET_IF_1_MAC",
  "RTEMS_NET_IF_2_MAC",
  "RTEMS_NET_IF_3_MAC",
  "RTEMS_NET_IF_4_MAC",
  "RTEMS_NET_IF_1_OPTS",
  "RTEMS_NET_IF_2_OPTS",
  "RTEMS_NET_IF_3_OPTS",
  "RTEMS_NET_IF_4_OPTS",
  "RTEMS_NET_IF_1_IP_ADDR",
  "RTEMS_NET_IF_2_IP_ADDR",
  "RTEMS_NET_IF_3_IP_ADDR",
  "RTEMS_NET_IF_4_IP_ADDR",
  "RTEMS_NET_IF_1_NETMASK",
  "RTEMS_NET_IF_2_NETMASK",
  "RTEMS_NET_IF_3_NETMASK",
  "RTEMS_NET_IF_4_NETMASK",
  "RTEMS_NET_SERVER_IP",
  "RTEMS_NET_GATEWAY_IP",
  "RTEMS_NET_NTP_IP",
  "RTEMS_NET_DNS_IP",
  "RTEMS_NET_DOMAIN_NAME",
  "RTEMS_NET_BOOT_FILE",
  "RTEMS_BOOT_CMD_LINE",
  "RTEMS_BOOT_SCRIPT",
  "RTEMS_NFS_MOUNT_PATH",
};

#define NUMOF(_s) (sizeof(_s) / (sizeof(_s[0])))

static void load_env(struct env_var* vars, size_t num_vars) {
  size_t v;
  for (v = 0; v < num_vars; ++v) {
    assert(setenv(vars[v].key, vars[v].val, 1) == 0);
  }
}

static void unload_env(struct env_var* vars, size_t num_vars) {
  size_t v;
  for (v = 0; v < num_vars; ++v) {
    unsetenv(vars[v].key);
  }
}

static void clear_all(void) {
  size_t v;
  for (v = 0; v < NUMOF(all_vars); ++v) {
    unsetenv(all_vars[v]);
  }
}
static void test_rc_conf_env_test1(void) {
  char out[1024];
  load_env(test1_no_dhcp, NUMOF(test1_no_dhcp));
  int r = rtems_bsd_rc_conf_from_env_buf(
    out, sizeof(out), true, 123, 60, true);
  assert(r > 0);
  assert(strcmp(out, test1_rc_conf) == 0);
  unload_env(test1_no_dhcp, NUMOF(test1_no_dhcp));
}

static void test_rc_conf_env_test2(void) {
  char out[1024];
  load_env(test2_dhcp, NUMOF(test2_dhcp));
  int r = rtems_bsd_rc_conf_from_env_buf(
    out, sizeof(out), true, 123, 60, true);
  assert(r > 0);
  assert(strcmp(out, test2_rc_conf) == 0);
  unload_env(test2_dhcp, NUMOF(test2_dhcp));
}

static void test_resolv_conf_env_test3(void) {
  char out[1024];
  load_env(test3_resolv, NUMOF(test3_resolv));
  int r = rtems_bsd_resolv_conf_from_env_buf(out, sizeof(out), true);
  assert(r > 0);
  assert(strcmp(out, test3_dns) == 0);
  unload_env(test3_resolv, NUMOF(test3_resolv));
}

static void test_no_iface_env_test4(void) {
  char out[1024];
  load_env(test4_no_iface, NUMOF(test4_no_iface));
  int r = rtems_bsd_rc_conf_from_env_buf(
    out, sizeof(out), true, 123, 60, true);
  assert(r == 0);
  unload_env(test4_no_iface, NUMOF(test4_no_iface));
}

static void test_rc_conf_env(void) {
  test_rc_conf_env_test1();
  test_rc_conf_env_test2();
  test_resolv_conf_env_test3();
  test_no_iface_env_test4();
}

static void shell(void) {
#if TEST_END_IN_SHELL
  rtems_shell_init(
    "SHLL", 32 * 1024, 1, CONSOLE_DEVICE_NAME, false, true, NULL);
#endif
}

static void
test_main(void)
{
  clear_all();
  test_rc_conf_env();
  shell();
  exit(0);
}

#define CONFIGURE_SHELL_COMMANDS_INIT

#include <bsp/irq-info.h>

#include <rtems/netcmds-config.h>

#define CONFIGURE_SHELL_USER_COMMANDS \
  &bsp_interrupt_shell_command, \
  &rtems_shell_HOSTNAME_Command, \
  &rtems_shell_PING_Command, \
  &rtems_shell_ROUTE_Command, \
  &rtems_shell_NETSTAT_Command, \
  &rtems_shell_IFCONFIG_Command, \
  &rtems_shell_TCPDUMP_Command, \
  &rtems_shell_PFCTL_Command, \
  &rtems_shell_SYSCTL_Command

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
#define CONFIGURE_SHELL_COMMAND_SHUTDOWN
#define CONFIGURE_SHELL_COMMAND_GETENV

#include <rtems/shellconfig.h>

#define RTEMS_BSD_CONFIG_BSP_CONFIG
#define RTEMS_BSD_CONFIG_SERVICE_TELNETD
#define RTEMS_BSD_CONFIG_TELNETD_STACK_SIZE (16 * 1024)
#define RTEMS_BSD_CONFIG_SERVICE_FTPD
#define RTEMS_BSD_CONFIG_FIREWALL_PF

#define CONFIGURE_MAXIMUM_DRIVERS 32

#include <rtems/bsd/test/default-init.h>
