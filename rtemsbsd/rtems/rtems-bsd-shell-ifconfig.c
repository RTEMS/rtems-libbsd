/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems/netcmds-config.h>
#include <machine/rtems-bsd-commands.h>

rtems_shell_cmd_t rtems_shell_IFCONFIG_Command = {
  "ifconfig",                    /* name */
  "ifconfig [args]",             /* usage */
  "net",                         /* topic */
  rtems_bsd_command_ifconfig,    /* command */
  NULL,                          /* alias */
  NULL                           /* next */
};
