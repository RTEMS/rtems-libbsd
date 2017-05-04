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

#define TEST_NAME "LIBBSD TERMIOS 5"

static void
test_main(void)
{
  test_termios_make_dev();

  open_it();

  change_lflag( "non-canonical", ICANON, 0 );

  change_vmin_vtime( "to polling", 0, 0 );
  read_it( 0 );

  change_vmin_vtime( "to half-second timeout", 0, 5 );
  read_it( 0 );

  change_vmin_vtime( "to half-second timeout", 5, 3 );
  puts( "Enqueue 2 characters" );
  termios_test_driver_set_rx( "ab", 2 );
  read_it( 2 );

  change_vmin_vtime( "to half-second timeout", 5, 3 );
  puts( "Enqueue 1 character" );
  termios_test_driver_set_rx( "b", 1 );
  read_it( 1 );

  puts( "Enqueue 7 characters" );
  termios_test_driver_set_rx( "1234567", 7 );
  read_it( 5 );
  read_it( 2 );

  close_it();

  exit(0);
}

#include <rtems/bsd/test/default-termios-init.h>