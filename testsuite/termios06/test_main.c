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
#include <sys/tty.h>

#include <rtems/console.h>
#include <rtems/shell.h>
#include <rtems/bsd/bsd.h>

#include <termios.h>
#include <rtems/libcsupport.h>
#include <rtems/termiostypes.h>

#include "../termios/test_termios_driver.h"
#include "../termios/test_termios_utilities.h"

#define TEST_NAME "LIBBSD TERMIOS 6"

static struct termios term;
static rtems_id flush_task_id;

static void init_term(void)
{
  int rv;

  rv = tcgetattr(Test_fd, &term);
  assert(rv == 0);

  term.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP
    | INLCR | IGNCR | ICRNL | IXON);
  term.c_lflag &= ~(ECHO | ECHOE | ECHOK | ECHONL | ECHOPRT
    | ECHOCTL | ECHOKE | ICANON | ISIG | IEXTEN);
  term.c_cflag &= ~(CSIZE | PARENB);
  term.c_cflag |= CS8;
  term.c_oflag &= ~(OPOST | ONLRET | ONLCR | OCRNL
    | TABDLY | OLCUC);

  term.c_cc[VMIN] = 0;
  term.c_cc[VTIME] = 0;

  rv = tcsetattr(Test_fd, TCSANOW, &term);
  assert(rv == 0);
}

static void test_igncr(void)
{
  ssize_t n;
  char c;

  c = 'x';

  change_iflag("IGNCR", 0, IGNCR);

  n = read(Test_fd, &c, sizeof(c));
  assert(n == 0);
  assert(c == 'x');

  termios_test_driver_set_rx_char('\r');

  n = read(Test_fd, &c, sizeof(c));
  assert(n == 0);
  assert(c == 'x');

  change_iflag("~IGNCR", IGNCR, 0);
  termios_test_driver_set_rx_char('\r');

  n = read(Test_fd, &c, sizeof(c));
  assert(n == 1);
  assert(c == '\r');
}

static void test_istrip(void)
{
  ssize_t n;
  char c;

  c = 'x';

  change_iflag("ISTRIP", 0, ISTRIP);

  n = read(Test_fd, &c, sizeof(c));
  assert(n == 0);
  assert(c == 'x');

  termios_test_driver_set_rx_char('\376');

  n = read(Test_fd, &c, sizeof(c));
  assert(n == 1);
  assert(c == '~');

  change_iflag("~ISTRIP", ISTRIP, 0);
  termios_test_driver_set_rx_char('\376');

  n = read(Test_fd, &c, sizeof(c));
  assert(n == 1);
  assert(c == '\376');
}

static void test_iuclc(void)
{
  ssize_t n;
  char c;

  c = 'x';

  //IUCLC is not supported within FREEBSD so this should not change the output
  change_iflag("IUCLC", 0, IUCLC);

  n = read(Test_fd, &c, sizeof(c));
  assert(n == 0);
  assert(c == 'x');

  termios_test_driver_set_rx_char('A');

  n = read(Test_fd, &c, sizeof(c));
  assert(n == 1);
  assert(c == 'A');

  change_iflag("~IUCLC", IUCLC, 0);
  termios_test_driver_set_rx_char('A');

  n = read(Test_fd, &c, sizeof(c));
  assert(n == 1);
  assert(c == 'A');
}

static void test_icrnl(void)
{
  ssize_t n;
  char c;

  c = 'x';

  change_iflag("ICRNL", 0, ICRNL);

  n = read(Test_fd, &c, sizeof(c));
  assert(n == 0);
  assert(c == 'x');

  termios_test_driver_set_rx_char('\r');

  n = read(Test_fd, &c, sizeof(c));
  assert(n == 1);
  assert(c == '\n');

  change_iflag("~ICRNL", ICRNL, 0);
  termios_test_driver_set_rx_char('\r');

  n = read(Test_fd, &c, sizeof(c));
  assert(n == 1);
  assert(c == '\r');
}

static void test_inlcr(void)
{
  ssize_t n;
  char c;

  c = 'x';

  change_iflag("INLCR", 0, INLCR);

  n = read(Test_fd, &c, sizeof(c));
  assert(n == 0);
  assert(c == 'x');

  termios_test_driver_set_rx_char('\n');

  n = read(Test_fd, &c, sizeof(c));
  assert(n == 1);
  assert(c == '\r');

  change_iflag("~INLCR", INLCR, 0);
  termios_test_driver_set_rx_char('\n');

  n = read(Test_fd, &c, sizeof(c));
  assert(n == 1);
  assert(c == '\n');
}

static void test_onlret(void)
{
  char c;
  ssize_t n;

  tt_softc.tp->t_column = 0;
  tcflush(Test_fd, TCOFLUSH);

  change_oflag("OPOST|ONLRET", 0, OPOST|ONLRET);

  c = 'a';
  n = write(Test_fd, &c, sizeof(c));
  assert(n == 1);
  assert(tt_softc.tp->t_column == 1);
  n = termios_test_driver_read_tx();
  assert(n == 1);
  assert(Tx_Buffer[0] == 'a');

  c = '\n';
  n = write(Test_fd, &c, sizeof(c));
  assert(n == 1);
  assert(tt_softc.tp->t_column == 0);
  n = termios_test_driver_read_tx();
  assert(n == 1);
  assert(Tx_Buffer[0] == '\n');

  change_oflag("~(OPOST|ONLRET)", OPOST|ONLRET, 0);
}

static void test_onlcr(void)
{
  char c;
  ssize_t n;

  tt_softc.tp->t_column = 0;
  tcflush(Test_fd, TCOFLUSH);

  change_oflag("OPOST|ONLCR", 0, OPOST|ONLCR);

  c = 'a';
  n = write(Test_fd, &c, sizeof(c));
  assert(n == 1);
  assert(tt_softc.tp->t_column == 1);
  n = termios_test_driver_read_tx();
  assert(n == 1);
  assert(Tx_Buffer[0] == 'a');

  c = '\n';
  n = write(Test_fd, &c, sizeof(c));
  assert(n == 1);
  assert(tt_softc.tp->t_column == 0);
  n = termios_test_driver_read_tx();
  assert(n == 2);
  assert(Tx_Buffer[0] == '\r');
  assert(Tx_Buffer[1] == '\n');

  change_oflag("~(OPOST|ONLCR)", OPOST|ONLCR, 0);
}

static void test_onocr(void)
{
  char c;
  ssize_t n;

  tt_softc.tp->t_column = 0;
  tcflush(Test_fd, TCOFLUSH);

  change_oflag("OPOST|ONOCR", 0, OPOST|ONOCR);

  c = '\r';
  n = write(Test_fd, &c, sizeof(c));
  assert(n == 1);
  assert(tt_softc.tp->t_column == 0);
  n = termios_test_driver_read_tx();
  assert(n == 0);

  c = 'a';
  n = write(Test_fd, &c, sizeof(c));
  assert(n == 1);
  assert(tt_softc.tp->t_column == 1);
  n = termios_test_driver_read_tx();
  assert(n == 1);
  assert(Tx_Buffer[0] == 'a');

  c = '\r';
  n = write(Test_fd, &c, sizeof(c));
  assert(n == 1);
  assert(tt_softc.tp->t_column == 0);
  n = termios_test_driver_read_tx();
  assert(n == 1);
  assert(Tx_Buffer[0] == '\r');

  change_oflag("~(OPOST|ONOCR)", OPOST|ONOCR, 0);
}

static void test_ocrnl(void)
{
  char c;
  ssize_t n;

  tt_softc.tp->t_column = 0;
  tcflush(Test_fd, TCOFLUSH);

  change_oflag("OPOST|OCRNL", 0, OPOST|OCRNL);

  c = '\r';
  n = write(Test_fd, &c, sizeof(c));
  assert(n == 1);
  assert(tt_softc.tp->t_column == 0);
  n = termios_test_driver_read_tx();
  assert(n == 1);
  assert(Tx_Buffer[0] == '\n');

  change_oflag("~(OPOST|OCRNL)", OPOST|OCRNL, 0);
}

static void test_ocrnl_onlret(void)
{
  char c;
  ssize_t n;

  tt_softc.tp->t_column = 0;
  tcflush(Test_fd, TCOFLUSH);

  change_oflag("OPOST|OCRNL|ONLRET", 0, OPOST|OCRNL|ONLRET);

  c = 'a';
  n = write(Test_fd, &c, sizeof(c));
  assert(n == 1);
  assert(tt_softc.tp->t_column == 1);
  n = termios_test_driver_read_tx();
  assert(n == 1);
  assert(Tx_Buffer[0] == 'a');

  c = '\r';
  n = write(Test_fd, &c, sizeof(c));
  assert(n == 1);
  assert(tt_softc.tp->t_column == 0);
  n = termios_test_driver_read_tx();
  assert(n == 1);
  assert(Tx_Buffer[0] == '\n');

  change_oflag("~(OPOST|OCRNL|ONLRET)", OPOST|OCRNL|ONLRET, 0);
}

static void test_opost(void)
{
  char c;
  ssize_t n;

  tt_softc.tp->t_column = 0;
  tcflush(Test_fd, TCOFLUSH);

  change_oflag("OPOST", 0, OPOST);

  c = 'a';
  n = write(Test_fd, &c, sizeof(c));
  assert(n == 1);
  assert(tt_softc.tp->t_column == 1);
  n = termios_test_driver_read_tx();
  assert(n == 1);
  assert(Tx_Buffer[0] == 'a');

  c = '\33';
  n = write(Test_fd, &c, sizeof(c));
  assert(n == 1);
  assert(tt_softc.tp->t_column == 1);
  n = termios_test_driver_read_tx();
  assert(n == 1);
  assert(Tx_Buffer[0] == '\33');

  c = '\t';
  n = write(Test_fd, &c, sizeof(c));
  assert(n == 1);
  assert(tt_softc.tp->t_column == 8);
  n = termios_test_driver_read_tx();
  assert(n == 1);
  assert(Tx_Buffer[0] == '\t');

  c = '\b';
  n = write(Test_fd, &c, sizeof(c));
  assert(n == 1);
  assert(tt_softc.tp->t_column == 7);
  n = termios_test_driver_read_tx();
  assert(n == 1);
  assert(Tx_Buffer[0] == '\b');

  c = '\r';
  n = write(Test_fd, &c, sizeof(c));
  assert(n == 1);
  assert(tt_softc.tp->t_column == 0);
  n = termios_test_driver_read_tx();
  assert(n == 1);
  assert(Tx_Buffer[0] == '\r');

  change_oflag("~OPOST", OPOST, 0);
}

static void test_xtabs(void)
{
  char c;
  ssize_t n;

  tt_softc.tp->t_column = 0;
  tcflush(Test_fd, TCOFLUSH);

  change_oflag("OPOST|OXTABS", 0, OPOST|OXTABS);

  c = 'a';
  n = write(Test_fd, &c, sizeof(c));
  assert(n == 1);
  assert(tt_softc.tp->t_column == 1);
  n = termios_test_driver_read_tx();
  assert(n == 1);
  assert(Tx_Buffer[0] == 'a');

  c = '\t';
  n = write(Test_fd, &c, sizeof(c));
  assert(n == 1);
  assert(tt_softc.tp->t_column == 8);
  n = termios_test_driver_read_tx();
  assert(n == 7);
  assert(Tx_Buffer[0] == ' ');
  assert(Tx_Buffer[1] == ' ');
  assert(Tx_Buffer[2] == ' ');
  assert(Tx_Buffer[3] == ' ');
  assert(Tx_Buffer[4] == ' ');
  assert(Tx_Buffer[5] == ' ');
  assert(Tx_Buffer[6] == ' ');

  change_oflag("~(OPOST|OXTABS)", OPOST|OXTABS, 0);
}

static void test_olcuc(void)
{
  char c;
  ssize_t n;

  tt_softc.tp->t_column = 0;
  tcflush(Test_fd, TCOFLUSH);

  //OLCUC is not supported within FREEBSD so this should not change the output
  change_oflag("OPOST|OLCUC", 0, OPOST|OLCUC);

  c = 'a';
  n = write(Test_fd, &c, sizeof(c));
  assert(n == 1);
  assert(tt_softc.tp->t_column == 1);
  n = termios_test_driver_read_tx();
  assert(n == 1);
  assert(Tx_Buffer[0] == 'a');

  c = 'B';
  n = write(Test_fd, &c, sizeof(c));
  assert(n == 1);
  assert(tt_softc.tp->t_column == 2);
  n = termios_test_driver_read_tx();
  assert(n == 1);
  assert(Tx_Buffer[0] == 'B');

  c = '9';
  n = write(Test_fd, &c, sizeof(c));
  assert(n == 1);
  assert(tt_softc.tp->t_column == 3);
  n = termios_test_driver_read_tx();
  assert(n == 1);
  assert(Tx_Buffer[0] == '9');

  change_oflag("~(OPOST|OLCUC)", OPOST|OLCUC, 0);
}

static void
test_main(void)
{
  test_termios_make_dev();

  open_it();

  init_term();

  test_igncr();
  test_istrip();
  test_iuclc();
  test_icrnl();
  test_inlcr();
  test_onlret();
  test_onlcr();
  test_onocr();
  test_ocrnl();
  test_ocrnl_onlret();
  test_opost();
  test_xtabs();
  test_olcuc();

  close_it();

  exit(0);
}

#include <rtems/bsd/test/default-termios-init.h>