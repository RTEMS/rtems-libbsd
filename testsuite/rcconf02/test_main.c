/*
 * Copyright 2016 Chris Johns <chrisj@rtems.org>
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

/*
 * Tests:
 *
 * 1. rc.conf processing
 *  1.1  syslog_priority
 *  1.2  create_args_*
 *  1.3  hostname
 *  1.4  ifconfig_<iface>
 *  1.5  vlans_<iface>
 *  1.6  ifconfig_<iface>.<vlan>
 *  1.7  defaultrouter
 *  1.8  defaultroute_delay
 *  1.9  ftp_enable
 *  1.10 ftp_options
 *  1.11 dhcpcd_priority
 *  1.12 dhcpcd_options
 *
 * 2. dhcpcd (via vlan, should timeout unless VLAN is present)
 *
 * 3. get route, the defaultrouter sets a default route and the vlan DHCP
 *    interface requires the default route be probed and found.
 *
 * 4. ftpd
 */

#include <rtems/bsd/sys/param.h>

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sysexits.h>
#include <unistd.h>

#include <machine/rtems-bsd-commands.h>
#include <machine/rtems-bsd-rc-conf.h>
#include <machine/rtems-bsd-rc-conf-services.h>

#include <rtems/bsd/test/network-config.h>

#include <rtems/console.h>
#include <rtems/shell.h>

#define TEST_NAME "LIBBSD RC.CONF 2"

#define IFACE_IPV4(iface) \
  "ifconfig_" # iface "=\"inet " NET_CFG_SELF_IP " netmask " NET_CFG_NETMASK "\"\n"


#define RC_CONF_IFACES \
  IFACE_IPV4(dmc0)  \
  IFACE_IPV4(sm0)   \
  IFACE_IPV4(cgem0) \
  IFACE_IPV4(fec0)  \
  IFACE_IPV4(em0)   \
  IFACE_IPV4(re0)

#define IFACE_VLAN(iface) \
  "vlans_" # iface "=\"101 102\"\n" \
  "ifconfig_" # iface "_101=\"inet 192.0.101.1/24\"\n" \
  "ifconfig_" # iface "_102=\"DHCP\"\n"

#define RC_CONF_VLANS \
  IFACE_VLAN(dmc0)  \
  IFACE_VLAN(sm0)   \
  IFACE_VLAN(cgem0) \
  IFACE_VLAN(fec0)  \
  IFACE_VLAN(em0)   \
  IFACE_VLAN(re0)

static const char* rc_conf_text =                          \
  "#\n"                                                    \
  "# Tests rc.conf. Add every NIC\n"                       \
  "#\n"                                                    \
  "\n"                                                     \
  "syslog_priority=\"debug\"\n"                            \
  "\n"                                                     \
  "hostname=\"rctest\"\n"                                  \
  "\n"                                                     \
  "create_args_myvlan=\"vlan 102\"\n"                      \
  "create_args_yourvlan=\"vlan 202\"\n"                    \
  "\n"                                                     \
  RC_CONF_IFACES                                           \
  "\n"                                                     \
  RC_CONF_VLANS                                            \
  "\n"                                                     \
  "defaultrouter=\"" NET_CFG_GATEWAY_IP "\"\n"             \
  "defaultroute_delay=\"5\"\n"                             \
  "\n"                                                     \
  "dhcpcd_options=\"-h foobar\"\n"                         \
  "\n"                                                     \
  "telnetd_enable=\"YES\"\n"                               \
  "telnetd_options=\"-v -C 10 -P 50 -L\"\n"                \
  "\n"                                                     \
  "ftpd_enable=\"YES\"\n"                                  \
  "ftpd_options=\"-v -p 21 -C 10 -P 150 -L -I 10 -R /\"\n" \
  "n";

static void
test_rc_conf_script(void)
{
  const char* ifconfg_args[] = {
    "ifconfig", NULL
  };
  const char* netstat_args[] = {
    "netstat", "-rn", NULL
  };

  printf("--------------- rc.conf -----------------\n");
  printf(rc_conf_text);
  printf("-----------------------------------------\n");

  assert(rtems_bsd_run_rc_conf_script("internal", rc_conf_text, 15, true) == 0);

  printf("-------------- IFCONFIG -----------------\n");
  rtems_bsd_command_ifconfig(1, (char**) ifconfg_args);
  printf("-------------- NETSTAT ------------------\n");
  rtems_bsd_command_netstat(2, (char**) netstat_args);
  printf("-----------------------------------------\n");
}

static void
test_main(void)
{
  test_rc_conf_script();

  rtems_shell_init(
    "SHLL",
    32 * 1024,
    1,
    CONSOLE_DEVICE_NAME,
    false,
    true,
    NULL
    );

  exit(0);
}

#define CONFIGURE_SHELL_COMMANDS_INIT

#include <bsp/irq-info.h>

#include <rtems/netcmds-config.h>

#define CONFIGURE_SHELL_USER_COMMANDS \
  &bsp_interrupt_shell_command, \
  &rtems_shell_BSD_Command, \
  &rtems_shell_HOSTNAME_Command, \
  &rtems_shell_PING_Command, \
  &rtems_shell_ROUTE_Command, \
  &rtems_shell_NETSTAT_Command, \
  &rtems_shell_IFCONFIG_Command, \
  &rtems_shell_TCPDUMP_Command, \
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

#include <rtems/shellconfig.h>

#define RTEMS_BSD_CONFIG_BSP_CONFIG
#define RTEMS_BSD_CONFIG_SERVICE_TELNETD
#define RTEMS_BSD_CONFIG_TELNETD_STACK_SIZE (16 * 1024)
#define RTEMS_BSD_CONFIG_SERVICE_FTPD

#define CONFIGURE_MAXIMUM_DRIVERS 32

#include <rtems/bsd/test/default-init.h>
