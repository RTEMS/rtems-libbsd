/*
 *  COPYRIGHT (c) 1989-2017.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>

#include <rtems/console.h>
#include <rtems/shell.h>
#include <rtems/bsd/bsd.h>

#include <termios.h>
#include <rtems/libcsupport.h>
#include <rtems/termiostypes.h>

#include "../termios/test_termios_driver.h"
#include "../termios/test_termios_utilities.h"

#define TEST_NAME "LIBBSD TERMIOS 4"

const char XON_String[] = "\021";
const char XOFF_String[] = "\023";

const char ExpectedOutput_1[] =
"0123456789012345678901234567890123456789"
"0123456789012345678901234567890123456789"
"0123456789012345678901234567890123456789"
"0123456789012345678901234567890123456789"
"0123456789012345678901234567890123456789\n";

static void
test_main(void)
{
  rtems_status_code sc;

  test_termios_make_dev();

  open_it();

  change_iflag( "Set XON/XOFF", IXON|IXOFF, IXON|IXOFF );

  printf( "XOFF\n" );
  termios_test_driver_set_rx( XOFF_String, 1 );

  write_helper( Test_fd, ExpectedOutput_1 );

  sc = rtems_task_wake_after( 2 * rtems_clock_get_ticks_per_second() );
  assert( sc == RTEMS_SUCCESSFUL );

  printf( "XON\n" );
  termios_test_driver_set_rx( XON_String, 1 );

  termios_test_driver_dump_tx("Transmitted");

  close_it();

  exit(0);
}

#include <rtems/bsd/test/default-termios-init.h>