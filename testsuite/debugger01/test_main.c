/*
 * Copyright (c) 2016 Chris Johns <chrisj@rtems.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <assert.h>
#include <errno.h>
#include <string.h>

#include <rtems.h>
#include <rtems/shell.h>

#include <rtems/rtems-debugger.h>
#include <rtems/rtems-debugger-remote-tcp.h>

#define TEST_NAME "LIBBSD DEBUGGER 1"

extern rtems_shell_cmd_t rtems_shell_DEBUGGER_Command;

static void
test_main(void)
{
  rtems_printer printer;
  int           r;

  rtems_print_printer_fprintf(&printer, stdout);

  rtems_shell_add_cmd_struct(&rtems_shell_DEBUGGER_Command);

  r = rtems_debugger_register_tcp_remote();
  if (r < 0) {
    printf("error: TCP remote register: %s\n", strerror(errno));
    return;
  }

  r = rtems_debugger_start("tcp", "1122", 3, 1, &printer);
  if (r < 0) {
    printf("error: debugger start: %s\n", strerror(errno));
    return;
  }

  while (true)
    sleep(60);
}

#define DEFAULT_NETWORK_SHELL

#define CONFIGURE_MAXIMUM_DRIVERS 32

#include <rtems/bsd/test/default-network-init.h>
