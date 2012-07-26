/*
 *  This is the body of the test. It does not do much except ensure
 *  that the target is alive after initializing the TCP/IP stack.
 */

#include <bsp.h>
#include <stdlib.h>
#include <stdio.h>
#include <freebsd/bsd.h>

void print_test_name(void)
{
  printf( "\n\n*** LIBFREEBSD INITIALIZATION TEST ***\n" );
}

void test_main(void)
{
  puts( "Sleeping to see what happens" );
  sleep( 5 );

  printf( "*** END OF LIBFREEBSD INITIALIZATION TEST ***\n" );
  exit( 0 );
}
