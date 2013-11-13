/*
 *  This is the body of the test. It does not do much except ensure
 *  that the target is alive after initializing the TCP/IP stack.
 */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <rtems/shell.h>

#define TEST_NAME "LIBBSD NETSHELL 1"

/*
 * RTEMS Startup Task
 */
static void test_main(void)
{
  rtems_shell_env_t env = rtems_global_shell_env;

  rtems_shell_main_loop( &env );

  exit( 0 );
}

#include <rtems/bsd/test/default-init.h>
