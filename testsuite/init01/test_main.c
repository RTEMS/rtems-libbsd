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

/* for old configuration structure */
#include <rtems/rtems_bsdnet.h>

/*
 * Network configuration
 */
struct rtems_bsdnet_config rtems_bsdnet_config = {
    NULL,                   /* Network interface */
    NULL,                   /* Use fixed network configuration */
    0,                      /* Default network task priority */
    0,                      /* Default mbuf capacity */
    0,                      /* Default mbuf cluster capacity */
    "testSystem",           /* Host name */
    "nowhere.com",          /* Domain name */
    "127.0.0.1",            /* Gateway */
    "127.0.0.1",            /* Log host */
    {"127.0.0.1" },         /* Name server(s) */
    {"127.0.0.1" },         /* NTP server(s) */
    0,
    0,
    0,
    0,
    0
};

