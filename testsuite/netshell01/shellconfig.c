/*
 *  Shell Configuration
 */

#include <rtems/shell.h>

#include <rtems/netcmds-config.h>

#define CONFIGURE_SHELL_COMMANDS_INIT
#define CONFIGURE_SHELL_COMMANDS_ALL

#define CONFIGURE_SHELL_USER_COMMANDS \
    &rtems_shell_ARP_Command, \
    &rtems_shell_SYSCTL_Command, \
    &rtems_shell_HOSTNAME_Command, \
    &rtems_shell_PING_Command, \
    &rtems_shell_ROUTE_Command, \
    &rtems_shell_NETSTAT_Command, \
    &rtems_shell_IFCONFIG_Command, \
    &rtems_shell_VMSTAT_Command

#include <rtems/shellconfig.h>


