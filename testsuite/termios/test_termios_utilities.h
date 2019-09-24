/*
 *  COPYRIGHT (c) 1989-2017.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef TEST_TERMIOS_UTILITIES_H
#define TEST_TERMIOS_UTILITIES_H

#include <sys/types.h>

#define TX_MAX 1024

extern void open_it(void);
extern void close_it(void);
extern void read_it(ssize_t expected);
extern void change_iflag(const char *desc, int mask, int new);
extern void change_lflag(const char *desc, int mask, int new);
extern void change_oflag(const char *desc, int mask, int new);
extern void change_vmin_vtime( const char *desc, int min, int time );
extern void enable_non_blocking(bool enable);
extern void write_helper(int fd, const char *c);
extern void read_helper(int fd, const char *expected);
extern void termios_test_driver_dump_tx(const char *c);
extern int termios_test_driver_read_tx(void);
extern void termios_test_driver_set_rx( const void *p, size_t len );
extern void termios_test_driver_set_rx_char( char ch );

extern int Test_fd;
extern uint8_t read_helper_buffer[256];
extern uint8_t Tx_Buffer[TX_MAX];

#endif /* TEST_TERMIOS_UTILITIES_H */
