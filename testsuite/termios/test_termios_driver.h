/*
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef TEST_TERMIOS_DRIVER_H
#define TEST_TERMIOS_DRIVER_H

/**
 *  This macro defines the standard name for the USB serial device
 *  that is available to applications.
 */
#define TERMIOS_TEST_DRIVER_TTY_PREFIX "T"
#define TERMIOS_TEST_DRIVER_TTY_UNIT "0"
#define TERMIOS_TEST_DRIVER_DEVICE_NAME "/dev/tty" TERMIOS_TEST_DRIVER_TTY_PREFIX TERMIOS_TEST_DRIVER_TTY_UNIT

struct test_termios_softc {
  uint8_t sc_lsr;
  uint8_t sc_msr;
  uint8_t sc_mcr;
  struct tty *tp;
};

extern void test_termios_make_dev(void);

extern struct test_termios_softc tt_softc;

#endif /* TEST_TERMIOS_DRIVER_H */