/*
 *  COPYRIGHT (c) 1989-2017.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <machine/rtems-bsd-kernel-space.h>

#include <sys/param.h>
#include <sys/tty.h>
#include <sys/ttydisc.h>

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <rtems/console.h>
#include <rtems/dumpbuf.h>
#include <rtems/shell.h>
#include <rtems/bsd/bsd.h>

#include <termios.h>
#include <rtems/libcsupport.h>
#include <rtems/termiostypes.h>

#include "../termios/test_termios_driver.h"
#include "../termios/test_termios_utilities.h"

int Test_fd;
uint8_t read_helper_buffer[256];
uint8_t Tx_Buffer[TX_MAX];

void open_it(void)
{
  puts( "open(" TERMIOS_TEST_DRIVER_DEVICE_NAME ") - OK " );
  Test_fd = open( TERMIOS_TEST_DRIVER_DEVICE_NAME, O_RDWR );
  assert( Test_fd != -1 );

  change_iflag("BRKINT|ICRNL|IXON|IMAXBEL", 0xFFFFFFFF, BRKINT|ICRNL|IXON|IMAXBEL);
  change_lflag("ISIG|ICANON|IEXTEN|ECHO|ECHOK|ECHOE|ECHOCTL", 0xFFFFFFFF, ISIG|ICANON|IEXTEN|ECHO|ECHOK|ECHOE|ECHOCTL);
  change_oflag("OPOST|ONLCR|OXTABS", 0xFFFFFFFF, OPOST|ONLCR|OXTABS);
}

void close_it(void)
{
  int rc;

  puts( "close(" TERMIOS_TEST_DRIVER_DEVICE_NAME ") - OK " );
  rc = close( Test_fd );
  assert( rc == 0 );
}

void read_it( ssize_t expected )
{
  ssize_t rc;
  char    buf[32];

  assert( expected <= sizeof(buf) );

  printf( "read - %zd expected\n", expected );
  rc = read( Test_fd, buf, expected );
  if ( expected != rc )
    printf( "ERROR - expected=%zd rc=%zd\n", expected, rc );
  assert( expected == rc );
}

void write_it(void)
{
  ssize_t sc;
  char    ch[10] = "PPPD TEST";

  puts( "write(PPPD TEST) - OK " );
  sc = write(Test_fd, ch, sizeof(ch));
  assert( sc == sizeof(ch) );
}

void change_iflag( const char *desc, int mask, int new )
{
  int            rc;
  struct termios attr;

  printf( "Changing c_iflag to: %s\n", desc );
  rc = tcgetattr( Test_fd, &attr );
  assert( rc == 0 );

  attr.c_iflag &= ~mask;
  attr.c_iflag |= new;

  rc = tcsetattr( Test_fd, TCSANOW, &attr );
  assert( rc == 0 );
}

void change_lflag( const char *desc, int mask, int new )
{
  int            rc;
  struct termios attr;

  printf( "Changing c_lflag to: %s\n", desc );
  rc = tcgetattr( Test_fd, &attr );
  assert( rc == 0 );

  attr.c_lflag &= ~mask;
  attr.c_lflag |= new;

  rc = tcsetattr( Test_fd, TCSANOW, &attr );
  assert( rc == 0 );
}

void change_oflag( const char *desc, int mask, int new )
{
  int            rc;
  struct termios attr;

  printf( "Changing c_oflag to: %s\n", desc );
  rc = tcgetattr( Test_fd, &attr );
  assert( rc == 0 );

  attr.c_oflag &= ~mask;
  attr.c_oflag |= new;

  rc = tcsetattr( Test_fd, TCSANOW, &attr );
  assert( rc == 0 );
}

void change_vmin_vtime( const char *desc, int min, int time )
{
  int            rc;
  struct termios attr;

  printf( "Changing %s - VMIN=%d VTIME=%d\n", desc, min, time );
  rc = tcgetattr( Test_fd, &attr );
  assert( rc == 0 );

  attr.c_cc[VMIN] = min;
  attr.c_cc[VTIME] = time;

  rc = tcsetattr( Test_fd, TCSANOW, &attr );
  assert( rc == 0 );
}

void enable_non_blocking(bool enable)
{
  int flags;
  int rv;

  flags = fcntl(Test_fd, F_GETFL, 0);
  assert(flags >= 0);

  if (enable) {
    flags |= O_NONBLOCK;
  } else {
    flags &= ~O_NONBLOCK;
  }

  rv = fcntl(Test_fd, F_SETFL, flags);
  assert(rv == 0);
}

void write_helper( int fd, const char *c )
{
  size_t   len;
  int      rc;

  len = strlen( c );
  printf( "Writing: %s", c );

  rc = write( fd, c, len );
  assert( rc == len );

  termios_test_driver_dump_tx("Transmitted");
}

void read_helper( int fd, const char *expected )
{
  int    rc;
  size_t len;

  len = strlen( expected );

  termios_test_driver_set_rx( expected, len );
  printf( "\nReading (expected):\n" );
  rtems_print_buffer( (unsigned char *)expected, len-1 );

  rc = read( fd, read_helper_buffer, sizeof(read_helper_buffer) );
  assert( rc != -1 );

  printf( "Read %d bytes from read(2)\n", rc );
  rtems_print_buffer( read_helper_buffer, rc );

  termios_test_driver_dump_tx("Echoed");
}

int termios_test_driver_read_tx(void)
{
  int Tx_Index = 0;
  int Tx_Size = TX_MAX;
  int Tx_Length;

  for (;;) {
    Tx_Length = ttydisc_getc(tt_softc.tp, &Tx_Buffer[Tx_Index], Tx_Size);
    if (Tx_Length == 0)
        break;
    Tx_Index += Tx_Length;
    Tx_Size -= Tx_Length;
  }

  return Tx_Index;
}

void termios_test_driver_dump_tx(const char *c)
{
  int Tx_Index;

  Tx_Index = termios_test_driver_read_tx();
  printf( "%s %d characters\n", c, Tx_Index );
  if( Tx_Index > 0 ) {
    rtems_print_buffer( &Tx_Buffer[0], Tx_Index );
  }
}

void termios_test_driver_set_rx( const void *p, size_t len )
{
  tcflush(Test_fd, TCIFLUSH );
  ttydisc_rint_simple(tt_softc.tp, p, len);
  ttydisc_rint_done(tt_softc.tp);
}

void termios_test_driver_set_rx_char( char ch )
{
  char chs[1];

  chs[0] = ch;
  termios_test_driver_set_rx( &chs[0], 1 );
}
