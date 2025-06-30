/*
 * Copyright (c) 2016 Chris Johns <chrisj@rtems.org>.  All rights reserved.
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

/*
 * Telnet service. To use add to rc.conf:
 *
 * telnetd_enable="YES"
 * telnet_options="-p 21 -s 32768 -P 8 -L"
 *
 * The options defaults are show. The options are:
 *
 *  -C conns      : Maximum connections (ptys).
 *  -P priority   : Telnet thread priority (RTEMS Classis API).
 *  -L            : Login using /etc/passwd
 */

#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define __need_getopt_newlib
#include <getopt.h>

#include <rtems/telnetd.h>
#include <machine/rtems-bsd-rc-conf-services.h>

/*
 * Stack size. Use RTEMS_BSD_CONFIG_TELNETD_STACK_SIZE.
 */
extern int rtems_telnetd_stack_size;
__weak_reference(_rtems_telnetd_stack_size, rtems_telnetd_stack_size);
int _rtems_telnetd_stack_size;

/*
 * By default no login.
 */
static bool telnet_login;

static void
hydra_telnetd_command(char* device, void* arg)
{
  rtems_shell_login_check_t login = NULL;
  rtems_shell_env_t         shell_env;

  if (telnet_login)
    login = rtems_shell_login_check;

  rtems_shell_dup_current_env(&shell_env);

  shell_env.devname       = device;
  shell_env.taskname      = "TELn";
  shell_env.exit_shell    = false;
  shell_env.forever       = 0;
  shell_env.echo          = 0;
  shell_env.input         = NULL;
  shell_env.output        = NULL;
  shell_env.output_append = 0;
  shell_env.wake_on_end   = 0;
  shell_env.login_check   = login;

  rtems_shell_main_loop (&shell_env);
}

static int
telnetd_service(rtems_bsd_rc_conf* rc_conf)
{
  rtems_telnetd_config_table config = {
    .priority = 100,                  /* Telnet task priority */
    .stack_size = 32 * 1024,          /* Stack size */
    .command = hydra_telnetd_command, /* The telnetd command, runs the shell. */
  };
  rtems_bsd_rc_conf_argc_argv* aa;
  int                          conns = 0;
  int                          r;

  if (rtems_telnetd_stack_size != 0)
    config.stack_size = rtems_telnetd_stack_size;

  aa = rtems_bsd_rc_conf_argc_argv_create();
  if (aa == NULL)
    return -1;

  r = rtems_bsd_rc_conf_find(rc_conf, "telnetd_enable", aa);
  if (r == 0) {
    rtems_status_code sc;
    bool              verbose = false;
    if (aa->argc == 2 && strcasecmp("YES", aa->argv[1]) == 0) {
      r = rtems_bsd_rc_conf_find(rc_conf, "telnetd_options", aa);
      if (r == 0) {
        struct getopt_data data;
        char*              end;

        memset(&data, 0, sizeof(data));

        while (true) {
          int c;

          c = getopt_r(aa->argc, (char * const *)aa->argv, "C:P:Lv", &data);
          if (c == -1)
            break;

          switch (c) {
          case 'C':
            conns = strtoul(data.optarg, &end, 10);
            if (conns == 0 || *end != '\0') {
              fprintf(stderr, "error: telnet: invalid connections countt\n");
            }
            else {
              config.client_maximum = conns;
            }
            break;
          case 'P':
            config.priority = strtoul(data.optarg, &end, 10);
            if (config.priority == 0 || *end != '\0') {
              fprintf(stderr, "error: telnetd: invalid priority\n");
              config.priority = 100;
            }
            break;
          case 'L':
            telnet_login = true;
            break;
          case 'v':
            verbose = true;
            break;
          case '?':
          default:
            fprintf(stderr, "error: telnetd: unknown option: %s\n", data.optarg);
            break;
          }
        }
      }
      if (verbose) {
        printf("telnetd: conns:%lu pri:%lu login:%s\n",
               config.client_maximum, config.priority,
               telnet_login == NULL ? "no" : "yes");
      }
      sc = rtems_telnetd_start(&config);
      if (sc != RTEMS_SUCCESSFUL)
        fprintf(stderr, "error: telnetd: %s\n", rtems_status_text(sc));
    }
  }

  return 0;
}

void
rc_conf_telnetd_init(void* arg)
{
  int r;
  r = rtems_bsd_rc_conf_service_add("telnetd",
                                    "after:network;before:ftpd;",
                                    telnetd_service);
  if (r < 0)
    fprintf(stderr, "error: telnetd service add failed: %s\n", strerror(errno));
}
