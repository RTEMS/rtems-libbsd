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

#define TEST_NAME "LIBBSD TERMIOS 3"

const char ExpectedOutput_1[] = "This is test output.\n";
const char ExpectedInput_1[] = "Test input this is.\n";
const char ExpectedInput_2[] = "1235\b456.\n";
const char ExpectedInput_3[] = "tab\ttab.\n";
const char ExpectedInput_4[] = "cr\r.";
const char ExpectedInput_5[] = "aBcDeFgH.\n";
const char ExpectedInput_6[] = "Testing VERASE\177.\n";
const char ExpectedInput_7[] = "Testing VKILL\025.\n";
const char ExpectedInput_8[] = "\177Testing VERASE in column 1.\n";
const char ExpectedInput_9[] = "\t tab \tTesting VKILL after tab.\025\n";

static void
test_main(void)
{
  test_termios_make_dev();

  open_it();

  /* some basic cases */
  write_helper( Test_fd, ExpectedOutput_1 );
  read_helper( Test_fd, ExpectedInput_1 );
  read_helper( Test_fd, ExpectedInput_2 );
  read_helper( Test_fd, ExpectedInput_3 );
  read_helper( Test_fd, ExpectedInput_4 );

  /* test to lower case input mapping */
  read_helper( Test_fd, ExpectedInput_5 );
  //IUCLC is not supported within FREEBSD so this should not change the output
  change_iflag( "Enable to lower case mapping on input", IUCLC, IUCLC );
  read_helper( Test_fd, ExpectedInput_5 );
  change_iflag( "Disable to lower case mapping on input", IUCLC, 0 );

  read_helper( Test_fd, ExpectedInput_6 );
  read_helper( Test_fd, ExpectedInput_7 );
  read_helper( Test_fd, ExpectedInput_8 );
  read_helper( Test_fd, ExpectedInput_9 );

  puts( "" );
  close_it();

  exit(0);
}

#include <rtems/bsd/test/default-termios-init.h>