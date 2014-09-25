/**
 * @file rtems/netcmds-config.h
 *
 * RTEMS Shell Network Command Set Configuration
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_NETCMDS_CONFIG_h
#define _RTEMS_NETCMDS_CONFIG_h

#include <rtems/shell.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern rtems_shell_cmd_t rtems_shell_BSD_Command;

extern rtems_shell_cmd_t rtems_shell_PING_Command;
extern rtems_shell_cmd_t rtems_shell_PING6_Command;

extern rtems_shell_cmd_t rtems_shell_IFCONFIG_Command;
extern rtems_shell_cmd_t rtems_shell_ROUTE_Command;
extern rtems_shell_cmd_t rtems_shell_NETSTAT_Command;
extern rtems_shell_cmd_t rtems_shell_DHCPCD_Command;

extern rtems_shell_cmd_t rtems_shell_DHCPCD_Command;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
