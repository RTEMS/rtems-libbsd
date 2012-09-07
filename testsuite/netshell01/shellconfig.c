/*
 *  Shell Configuration
 */

#include <rtems/shell.h>

#include <rtems/netcmds-config.h>

#define CONFIGURE_SHELL_COMMANDS_INIT
#define CONFIGURE_SHELL_COMMANDS_ALL

#define CONFIGURE_SHELL_USER_COMMANDS \
    &rtems_shell_PING_Command

#include <rtems/shellconfig.h>


