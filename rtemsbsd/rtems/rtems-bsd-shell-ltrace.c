/*
 * Copyright (c) 2026 Chris Johns <chris@contemporary.siftware>.
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

#include <machine/rtems-bsd-kernel-space.h>
#include <machine/rtems-bsd-support.h>

#include <sys/sdt.h>

#include <rtems/netcmds-config.h>
#include <machine/rtems-bsd-commands.h>

#include "rtems-kernel-kdtrace.h"

static int rtems_bsd_command_ltrace(int argc, char **argv) {
#if defined(KDTRACE_HOOKS)
  if (argc == 1) {
    int count = rtems_sdt_status();
    if (count < 0) {
      printf("ltrace not active\n");
    } else {
      printf("ltrace: %i records\n", count);
    }
    return 0;
  }
  if (argc > 2) {
    printf("error: too many arguments\n");
    return 1;
  }
  if (strcmp("open", argv[1]) == 0) {
    rtems_sdt_open(5000);
  } else if (strcmp("start", argv[1]) == 0) {
    rtems_sdt_enable();
  } else if (strcmp("stop", argv[1]) == 0) {
    rtems_sdt_disable();
  } else if (strcmp("clear", argv[1]) == 0) {
    rtems_sdt_clear();
  } else if (strcmp("reset", argv[1]) == 0) {
    rtems_sdt_reset();
  } else if (strcmp("print", argv[1]) == 0) {
    rtems_sdt_print(stdout);
  } else if (strcmp("json", argv[1]) == 0) {
    rtems_sdt_json(stdout);
  } else {
    printf("error: invalid command: %s\n", argv[1]);
    return 1;
  }
#else /* KDTRACE_HOOKS */
  printf("errro: o KDTRACE support build\n");
#endif /* KDTRACE_HOOKS */
  return 0;
}

rtems_shell_cmd_t rtems_shell_LTRACE_Command = {
  .name = "ltrace",
  .usage = "ltrace [-h] [args]",
  .topic = "misc",
  .command = rtems_bsd_command_ltrace
};
