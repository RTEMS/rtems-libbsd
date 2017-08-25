/*
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#define TTYDEFCHARS
#include <sys/ttydefaults.h>
#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

#define TEST_NAME "LIBBSD TERMIOS 1"

/* rtems_termios_baud_t is a typedefs to int32_t */
#define PRIdrtems_termios_baud_t PRId32

/*
 *  Baud Rate Constant Mapping Entry
 */
typedef struct {
  tcflag_t constant;
  rtems_termios_baud_t baud;
} termios_baud_test_r;

#define INVALID_CONSTANT ((tcflag_t) -2)

#define INVALID_BAUD ((rtems_termios_baud_t) -2)
/*
 *  Baud Rate Constant Mapping Table
 */
static const termios_baud_test_r baud_table[] = {
  { B0,           0 },
  { B50,         50 },
  { B75,         75 },
  { B110,       110 },
  { B134,       134 },
  { B150,       150 },
  { B200,       200 },
  { B300,       300 },
  { B600,       600 },
  { B1200,     1200 },
  { B1800,     1800 },
  { B2400,     2400 },
  { B4800,     4800 },
  { B9600,     9600 },
  { B19200,   19200 },
  { B38400,   38400 },
  { B7200,     7200 },
  { B14400,   14400 },
  { B28800,   28800 },
  { B57600,   57600 },
  { B76800,   76800 },
  { B115200, 115200 },
  { B230400, 230400 },
  { B460800, 460800 },
  { B921600, 921600 },
  { INVALID_CONSTANT, INVALID_BAUD }
};

/*
 *  Character Size Constant Mapping Entry
 */
typedef struct {
  tcflag_t constant;
  int bits;
} termios_character_size_test_r;

/*
 *  Character Size Constant Mapping Table
 */
static const termios_character_size_test_r char_size_table[] = {
  { CS5,      5 },
  { CS6,      6 },
  { CS7,      7 },
  { CS8,      8 },
  { INVALID_CONSTANT, -1 }
};

/*
 *  Parity Constant Mapping Entry
 */
typedef struct {
  tcflag_t constant;
  const char *parity;
} termios_parity_test_r;

/*
 *  Parity Constant Mapping Table
 */
static const termios_parity_test_r parity_table[] = {
  { 0,                "none" },
  { PARENB,           "even" },
  { PARENB | PARODD,  "odd" },
  { INVALID_CONSTANT, NULL }
};

/*
 *  Stop Bit Constant Mapping Entry
 */
typedef struct {
  tcflag_t constant;
  int stop;
} termios_stop_bits_test_r;

/*
 *  Stop Bit Constant Mapping Table
 */
static const termios_stop_bits_test_r stop_bits_table[] = {
  { 0,       1 },
  { CSTOPB,  2 },
  { INVALID_CONSTANT, -1 }
};

/*
 *  Test converting baud rate into an index
 */
static void test_termios_baud2index(void)
{
  int i;
  int index;

  puts( "Test termios_baud2index..." );
  puts( "termios_baud_to_index(-2) - NOT OK" );
  i = rtems_termios_baud_to_index( INVALID_CONSTANT );
  assert( i == -1 );

  for (i=0 ; baud_table[i].constant != INVALID_CONSTANT ; i++ ) {
    printf(
      "termios_baud_to_index(B%" PRIdrtems_termios_baud_t ") - OK\n",
      baud_table[i].baud
    );
    index = rtems_termios_baud_to_index( baud_table[i].constant );
    assert(index == i);
  }
}

/*
 *  Test converting termios baud constant to baud number
 */
static void test_termios_baud2number(void)
{
  int i;
  rtems_termios_baud_t number;

  puts(
    "\n"
    "Test termios_baud2number..."
  );
  puts( "termios_baud_to_number(-2) - NOT OK" );
  number = rtems_termios_baud_to_number( INVALID_CONSTANT );
  assert( number == 0 );

  for (i=0 ; baud_table[i].constant != INVALID_CONSTANT ; i++ ) {
    printf(
      "termios_baud_to_number(B%" PRIdrtems_termios_baud_t ") - OK\n",
      baud_table[i].baud
    );
    number = rtems_termios_baud_to_number( baud_table[i].constant );
    assert( number == baud_table[i].baud );
  }
}

/*
 *  Test converting baud number to termios baud constant
 */
static void test_termios_number_to_baud(void)
{
  int i;
  tcflag_t termios_baud;

  puts(
    "\n"
    "Test termios_number_to_baud..."
  );
  puts( "termios_number_to_baud(-2) - NOT OK" );
  termios_baud = rtems_termios_number_to_baud( INVALID_BAUD );
  assert( termios_baud == 0 );

  for (i=0 ; baud_table[i].constant != INVALID_CONSTANT ; i++ ) {
    printf(
      "termios_number_to_baud(B%" PRIdrtems_termios_baud_t ") - OK\n",
      baud_table[i].baud
    );
    termios_baud = rtems_termios_number_to_baud( baud_table[i].baud );
    assert( termios_baud == baud_table[i].constant );
  }
}

/*
 *  Test all the baud rate options
 */
static void test_termios_set_baud(
  int test
)
{
  int             sc;
  int             i;
  struct termios  attr;

  puts( "Test termios setting device baud rate..." );
  for (i=0 ; baud_table[i].constant != INVALID_CONSTANT ; i++ ) {
    sc = tcgetattr( test, &attr );
    assert(sc == 0);

    attr.c_ispeed = baud_table[i].constant;
    attr.c_ospeed = baud_table[i].constant;

    printf(
      "tcsetattr(TCSANOW, B%" PRIdrtems_termios_baud_t ") - OK\n",
      baud_table[i].baud
    );
    sc = tcsetattr( test, TCSANOW, &attr );
    assert(sc == 0);

    printf(
      "tcsetattr(TCSADRAIN, B%" PRIdrtems_termios_baud_t ") - OK\n",
      baud_table[i].baud
    );
    sc = tcsetattr( test, TCSANOW, &attr );
    assert(sc == 0);

    printf(
      "tcsetattr(TCSAFLUSH, B%" PRIdrtems_termios_baud_t ") - OK\n",
      baud_table[i].baud
    );
    sc = tcsetattr( test, TCSAFLUSH, &attr );
    assert(sc == 0);
  }
}

/*
 *  Test all the character size options
 */
static void test_termios_set_charsize(
  int test
)
{
  int             sc;
  int             i;
  struct termios  attr;

  puts(
    "\n"
    "Test termios setting device character size ..."
  );
  for (i=0 ; char_size_table[i].constant != INVALID_CONSTANT ; i++ ) {
    tcflag_t csize = CSIZE;

    sc = tcgetattr( test, &attr );
    assert(sc == 0);

    attr.c_cflag &= ~csize;
    attr.c_cflag |= char_size_table[i].constant;

    printf( "tcsetattr(TCSANOW, CS%d) - OK\n", char_size_table[i].bits );
    sc = tcsetattr( test, TCSANOW, &attr );
    assert(sc == 0);

    printf( "tcsetattr(TCSADRAIN, CS%d) - OK\n", char_size_table[i].bits );
    sc = tcsetattr( test, TCSADRAIN, &attr );
    assert(sc == 0);

    printf( "tcsetattr(TCSAFLUSH, CS%d) - OK\n", char_size_table[i].bits );
    sc = tcsetattr( test, TCSAFLUSH, &attr );
    assert(sc == 0);

    printf( "tcsetattr(TCSASOFT, CS%d) - OK\n", char_size_table[i].bits );
    sc = tcsetattr( test, TCSASOFT, &attr );
    assert(sc == 0);
  }
}

/*
 *  Test all the parity options
 */
static void test_termios_set_parity(
  int test
)
{
  int             sc;
  int             i;
  struct termios  attr;

  puts(
    "\n"
    "Test termios setting device parity ..."
  );
  for (i=0 ; parity_table[i].constant != INVALID_CONSTANT ; i++ ) {
    tcflag_t par = PARENB | PARODD;

    sc = tcgetattr( test, &attr );
    assert(sc == 0);

    attr.c_cflag &= ~par;
    attr.c_cflag |= parity_table[i].constant;

    printf( "tcsetattr(TCSANOW, %s) - OK\n", parity_table[i].parity );
    sc = tcsetattr( test, TCSANOW, &attr );
    assert(sc == 0);

    printf( "tcsetattr(TCSADRAIN, %s) - OK\n", parity_table[i].parity );
    sc = tcsetattr( test, TCSADRAIN, &attr );
    assert(sc == 0);

    printf( "tcsetattr(TCSAFLUSH, %s) - OK\n", parity_table[i].parity );
    sc = tcsetattr( test, TCSAFLUSH, &attr );
    assert(sc == 0);

    printf( "tcsetattr(TCSASOFT, %s) - OK\n", parity_table[i].parity );
    sc = tcsetattr( test, TCSASOFT, &attr );
    assert(sc == 0);
  }
}

/*
 *  Test all the stop bit options
 */
static void test_termios_set_stop_bits(
  int test
)
{
  int             sc;
  int             i;
  struct termios  attr;

  puts(
    "\n"
    "Test termios setting device character size ..."
  );
  for (i=0 ; stop_bits_table[i].constant != INVALID_CONSTANT ; i++ ) {
    tcflag_t cstopb = CSTOPB;

    sc = tcgetattr( test, &attr );
    assert(sc == 0);

    attr.c_cflag &= ~cstopb;
    attr.c_cflag |= stop_bits_table[i].constant;

    printf( "tcsetattr(TCSANOW, %d bit%s) - OK\n",
      stop_bits_table[i].stop,
      ((stop_bits_table[i].stop == 1) ? "" : "s")
    );
    sc = tcsetattr( test, TCSANOW, &attr );
    assert(sc == 0);

    printf( "tcsetattr(TCSADRAIN, %d bits) - OK\n", stop_bits_table[i].stop );
    sc = tcsetattr( test, TCSADRAIN, &attr );
    assert(sc == 0);

    printf( "tcsetattr(TCSAFLUSH, %d bits) - OK\n", stop_bits_table[i].stop );
    sc = tcsetattr( test, TCSAFLUSH, &attr );
    assert(sc == 0);

    printf( "tcsetattr(TCSASOFT, %d bits) - OK\n", stop_bits_table[i].stop );
    sc = tcsetattr( test, TCSASOFT, &attr );
    assert(sc == 0);
  }
}

static void test_termios_cfoutspeed(void)
{
  int i;
  int sc;
  speed_t speed;
  struct termios term;
  speed_t bad;

  bad = B921600 << 1;
  memset( &term, '\0', sizeof(term) );
  puts( "cfsetospeed(BAD BAUD) - EINVAL" );
  sc = cfsetospeed( &term, bad );
  assert( sc == -1 );
  assert( errno == EINVAL );

  for (i=0 ; baud_table[i].constant != INVALID_CONSTANT ; i++ ) {
    memset( &term, '\0', sizeof(term) );
    printf(
      "cfsetospeed(B%" PRIdrtems_termios_baud_t ") - OK\n",
      baud_table[i].baud
    );
    sc = cfsetospeed( &term, baud_table[i].constant );
    assert( !sc );
    printf(
      "cfgetospeed(B%" PRIdrtems_termios_baud_t ") - OK\n",
      baud_table[i].baud
    );
    speed = cfgetospeed( &term );
    assert( speed == baud_table[i].constant );
  }
}

static void test_termios_cfinspeed(void)
{
  int             i;
  int             sc;
  speed_t         speed;
  struct termios  term;
  speed_t         bad;

  bad = B921600 << 1;
  memset( &term, '\0', sizeof(term) );
  puts( "cfsetispeed(BAD BAUD) - EINVAL" );
  sc = cfsetispeed( &term, bad );
  assert( sc == -1 );
  assert( errno == EINVAL );

  for (i=0 ; baud_table[i].constant != INVALID_CONSTANT ; i++ ) {
    memset( &term, '\0', sizeof(term) );
    printf(
      "cfsetispeed(B%" PRIdrtems_termios_baud_t ") - OK\n",
      baud_table[i].baud
    );
    sc = cfsetispeed( &term, baud_table[i].constant );
    assert( !sc );

    printf(
      "cfgetispeed(B%" PRIdrtems_termios_baud_t ") - OK\n",
      baud_table[i].baud
    );
    speed = cfgetispeed( &term );
    assert( speed == baud_table[i].constant );
  }
}

static void test_termios_cfsetspeed(void)
{
  int             i;
  int             status;
  speed_t         speed;
  struct termios  term;
  speed_t         bad;

  bad = B921600 << 1;
  memset( &term, '\0', sizeof(term) );
  puts( "cfsetspeed(BAD BAUD) - EINVAL" );
  status = cfsetspeed( &term, bad );
  assert( status == -1 );
  assert( errno == EINVAL );

  for (i=0 ; baud_table[i].constant != INVALID_CONSTANT ; i++ ) {
    memset( &term, '\0', sizeof(term) );
    printf(
      "cfsetspeed(B%" PRIdrtems_termios_baud_t ") - OK\n",
      baud_table[i].baud
    );
    status = cfsetspeed( &term, baud_table[i].constant );
    assert( !status );

    printf(
      "cfgetspeed(B%" PRIdrtems_termios_baud_t ") - checking both inspeed and outspeed - OK\n",
      baud_table[i].baud
    );
    speed = cfgetispeed( &term );
    assert( speed == baud_table[i].constant );

    speed = cfgetospeed( &term );
    assert( speed == baud_table[i].constant );
  }
}

static void test_termios_cfmakeraw(void)
{
  struct termios  term;

  memset( &term, '\0', sizeof(term) );
  cfmakeraw( &term );
  puts( "cfmakeraw - OK" );

  /* Check that all of the flags were set correctly */
  assert( ~(term.c_iflag & (IMAXBEL|IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL|IXON)) );

  assert( term.c_iflag & (IGNBRK) );

  assert( ~(term.c_oflag & OPOST) );

  assert( ~(term.c_lflag & (ECHO|ECHONL|ICANON|ISIG|IEXTEN)) );

  assert( ~(term.c_cflag & (CSIZE|PARENB)) );

  assert( term.c_cflag & (CS8|CREAD) );

  assert( term.c_cc[VMIN] == 1 );

  assert( term.c_cc[VTIME] == 0 );
}

static void test_termios_cfmakesane(void)
{
  struct termios  term;

  memset( &term, '\0', sizeof(term) );
  cfmakesane( &term );
  puts( "cfmakesane - OK" );

  /* Check that all of the flags were set correctly */
  assert( term.c_iflag == TTYDEF_IFLAG );

  assert( term.c_oflag == TTYDEF_OFLAG );

  assert( term.c_lflag == TTYDEF_LFLAG );

  assert( term.c_cflag == TTYDEF_CFLAG );

  assert( term.c_ispeed == TTYDEF_SPEED );

  assert( term.c_ospeed == TTYDEF_SPEED );

  assert( memcmp(&term.c_cc, ttydefchars, sizeof(term.c_cc)) == 0 );
}

static void test_set_best_baud(void)
{
  static const struct {
    uint32_t baud;
    speed_t speed;
  } baud_to_speed_table[] = {
    { 0,          B0 },
    { 25,         B0 },
    { 26,         B50 },
    { 50,         B50 },
    { 62,         B50 },
    { 63,         B75 },
    { 75,         B75 },
    { 110,        B110 },
    { 134,        B134 },
    { 150,        B150 },
    { 200,        B200 },
    { 300,        B300 },
    { 600,        B600 },
    { 1200,       B1200 },
    { 1800,       B1800 },
    { 2400,       B2400 },
    { 4800,       B4800 },
    { 9600,       B9600 },
    { 19200,      B19200 },
    { 38400,      B38400 },
    { 57600,      B57600 },
    { 115200,     B115200 },
    { 230400,     B230400 },
    { 460800,     B460800 },
    { 0xffffffff, B460800 }
  };

  size_t n = RTEMS_ARRAY_SIZE(baud_to_speed_table);
  size_t i;

  for ( i = 0; i < n; ++i ) {
    struct termios term;

    memset( &term, 0xff, sizeof( term ) );
    rtems_termios_set_best_baud( &term, baud_to_speed_table[ i ].baud );

    assert( term.c_ispeed == baud_to_speed_table[ i ].speed );
    assert( term.c_ospeed == baud_to_speed_table[ i ].speed );
  }
}

static void
test_main(void)
{
  int                       rc;
  int                       test;
  struct termios            t;
  int index = 0;

  /*
   * Test baud rate
   */
  test_termios_baud2index();
  test_termios_baud2number();
  test_termios_number_to_baud();

  test_termios_make_dev();

  puts( "Init - open - " TERMIOS_TEST_DRIVER_DEVICE_NAME " - OK" );
  test = open( TERMIOS_TEST_DRIVER_DEVICE_NAME, O_RDWR );
  assert(test != -1);

  /*
   * tcsetattr - ERROR invalid operation
   */
  puts( "tcsetattr - invalid operation - EINVAL" );
  rc = tcsetattr( test, INT_MAX, &t );
  assert(rc == -1);
  assert(errno == EINVAL);

  test_termios_cfmakeraw();
  test_termios_cfmakesane();

  /*
   * tcsetattr - TCSADRAIN
   */
  puts( "\ntcsetattr - drain - OK" );
  rc = tcsetattr( test,  TCSADRAIN, &t );
  assert(rc == 0);

  test_termios_set_baud(test);

  puts( "Init - close - " TERMIOS_TEST_DRIVER_DEVICE_NAME " - OK" );
  rc = close( test );
  assert(rc == 0);

  /*
   * Test character size
   */
  puts(
    "\n"
    "Init - open - " TERMIOS_TEST_DRIVER_DEVICE_NAME " - OK"
  );
  test = open( TERMIOS_TEST_DRIVER_DEVICE_NAME, O_RDWR );
  assert(test != -1);

  test_termios_set_charsize(test);

  puts( "Init - close - " TERMIOS_TEST_DRIVER_DEVICE_NAME " - OK" );
  rc = close( test );
  assert(rc == 0);

  /*
   * Test parity
   */
  puts(
    "\n"
    "Init - open - " TERMIOS_TEST_DRIVER_DEVICE_NAME " - OK"
  );
  test = open( TERMIOS_TEST_DRIVER_DEVICE_NAME, O_RDWR );
  assert(test != -1);

  test_termios_set_parity(test);

  puts( "Init - close - " TERMIOS_TEST_DRIVER_DEVICE_NAME " - OK" );
  rc = close( test );
  assert(rc == 0);

  /*
   * Test stop bits
   */
  puts(
    "\n"
    "Init - open - " TERMIOS_TEST_DRIVER_DEVICE_NAME " - OK"
  );
  test = open( TERMIOS_TEST_DRIVER_DEVICE_NAME, O_RDWR );
  assert(test != -1);

  test_termios_set_stop_bits(test);

  test_termios_cfoutspeed();

  test_termios_cfinspeed();

  test_termios_cfsetspeed();

  puts( "Init - close - " TERMIOS_TEST_DRIVER_DEVICE_NAME " - OK" );
  rc = close( test );
  assert(rc == 0);

  puts( "Multiple open of the device" );
  for( ; index < 26; ++index ) {
    test = open( TERMIOS_TEST_DRIVER_DEVICE_NAME, O_RDWR );
    assert(test != -1);
    rc = close( test );
    assert(rc == 0);
  }
  puts( "" );

  test_set_best_baud();

  exit(0);
}

#include <rtems/bsd/test/default-termios-init.h>
