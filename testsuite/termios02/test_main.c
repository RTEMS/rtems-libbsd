/**
 * @file
 *
 * The following system calls are tested: tcflow(), tcflush(), tcgetpgrp(),
 * tcsendbreak() and ctermid().
 */

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

#define TEST_NAME "LIBBSD TERMIOS 2"

static void
test_main(void)
{
  int sc;
  pid_t pid;
  char *term_name_p;
  char term_name[32];

  test_termios_make_dev();

  open_it();

  puts( "tcdrain() - OK" );
  sc = tcdrain(Test_fd);
  assert( !sc );

  puts( "" );

  /***** TEST TCFLOW *****/
  puts( "tcflow(TCOOFF) - ENOTSUP" );
  errno = 0;
  sc = tcflow( Test_fd, TCOOFF );
  assert( sc == -1 );
  assert( errno == ENOTSUP );

  puts( "tcflow(TCOON) - ENOTSUP" );
  errno = 0;
  sc = tcflow( Test_fd, TCOON );
  assert( sc == -1 );
  assert( errno == ENOTSUP );

  puts( "tcflow(TCIOFF) - ENOTSUP" );
  errno = 0;
  sc = tcflow( Test_fd, TCIOFF );
  assert( sc == -1 );
  assert( errno == ENOTSUP );

  puts( "tcflow(TCION) - ENOTSUP" );
  errno = 0;
  sc = tcflow( Test_fd, TCION );
  assert( sc == -1 );
  assert( errno == ENOTSUP );

  puts( "tcflow(22) - EINVAL" );
  errno = 0;
  sc = tcflow( Test_fd, 22 );
  assert( sc == -1 );
  assert( errno == EINVAL );

  puts( "" );

  /***** TEST TCFLUSH *****/
  puts( "tcflush(TCIFLUSH) - OK" );
  errno = 0;
  sc = tcflush( Test_fd, TCIFLUSH );
  assert( sc == 0 );
  assert( errno == 0 );

  puts( "tcflush(TCOFLUSH) - OK" );
  sc = tcflush( Test_fd, TCOFLUSH );
  assert( sc == 0 );
  assert( errno == 0 );

  puts( "tcflush(TCIOFLUSH) - OK" );
  sc = tcflush( Test_fd, TCIOFLUSH );
  assert( sc == 0 );
  assert( errno == 0 );

  puts( "tcflush(22) - EINVAL" );
  errno = 0;
  sc = tcflush( Test_fd, 22 );
  assert( sc == -1 );
  assert( errno == EINVAL );

  puts( "" );

  /***** TEST TCGETPGRP *****/
  puts( "tcgetpgrp() - OK" );
  pid = tcgetpgrp(Test_fd);
  assert( pid == getpid() );

  puts( "tcsetpgrp(3) - OK" );
  sc = tcsetpgrp( Test_fd, 3 );
  assert( !sc );

  puts( "" );

  /***** TEST TCSENDBREAK *****/
  puts( "tcsendbreak(0) - OK" );
  sc = tcsendbreak( Test_fd, 0 );
  assert( !sc );

  puts( "" );

  /***** TEST CTERMID *****/
  puts( "ctermid( NULL ) - OK" );
  term_name_p = ctermid( NULL );
  assert( term_name_p );
  printf( "ctermid ==> %s\n", term_name_p );

  puts( "ctermid( term_name ) - OK" );
  term_name_p = ctermid( term_name );
  assert( term_name_p == term_name );
  printf( "ctermid ==> %s\n", term_name_p );

  close_it();

  exit(0);
}

#include <rtems/bsd/test/default-termios-init.h>
