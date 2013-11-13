/*
 *  This is the body of the test. It does not do much except ensure
 *  that the target is alive after initializing the TCP/IP stack.
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define TEST_NAME "LIBBSD INIT 1"

static void test_main(void)
{
  puts( "Sleeping to see what happens" );
  sleep( 5 );

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

#include <rtems/bsd/test/default-init.h>
