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

void print_test_name(void)
{
  printf( "\n\n*** LIBFREEBSD NETWORK SHELL TEST ***\n" );
}

/*
 * RTEMS Startup Task
 */
void test_main(void)
{
  rtems_shell_env_t env = rtems_global_shell_env;

  rtems_shell_main_loop( &env );

  puts( "*** END OF NETWORK SHELL TEST ***" );
  exit( 0 );
}
