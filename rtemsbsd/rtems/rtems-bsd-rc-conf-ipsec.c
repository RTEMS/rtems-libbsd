/*
 * Copyright (c) 2018 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
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
 * Handle the IPSec and ike related directives found in rc.conf.
 * - ipsec_enable
 * - ipsec_file
 * - ike_enable
 * - ike_program
 * - ike_flags
 * - ike_priority
 *
 * Notes:
 * - `ike_...` seems not to be included in the rc.conf man page. But the
 *   parameters are there in the default rc.conf file. So handle them.
 * - For ike_program, "racoon" is the only supported one (without any path).
 */

#include <rtems.h>
#include <rtems/ipsec.h>

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include <machine/rtems-bsd-commands.h>
#include <machine/rtems-bsd-rc-conf-services.h>

static int
ipsec_service(rtems_bsd_rc_conf* rc_conf)
{
  rtems_bsd_rc_conf_argc_argv* aa;
  int r;
  int erroroccured = 0;

  aa = rtems_bsd_rc_conf_argc_argv_create();
  if (aa == NULL)
    return -1;

  r = rtems_bsd_rc_conf_find(rc_conf, "ipsec_enable", aa);
  if (r == 0) {
    if (aa->argc == 2 && strcasecmp("YES", aa->argv[1]) == 0) {
      char* ipsec_file = NULL;

      if (erroroccured == 0) {
        r = rtems_bsd_rc_conf_find(rc_conf, "ipsec_file", aa);
        if (r == 0) {
          if (aa->argc == 2) {
            ipsec_file = strdup(aa->argv[1]);
            if (ipsec_file == NULL) {
              fprintf(stderr,
                  "error: ipsec: Could not duplicate ipsec file string: %s\n",
                  strerror(errno));
              erroroccured = -1;
            }
          } else {
            fprintf(stderr,
                "error: ipsec: Syntax error in ipsec_file directive.\n");
              erroroccured = -1;
          }
        }
      }

      if (erroroccured == 0) {
        if (ipsec_file == NULL) {
          fprintf(stderr, "error: ipsec: No ipsec_file given.\n");
          erroroccured = -1;
        }
      }

      if (erroroccured == 0) {
        char *setkey[] = {"setkey", "-f", ipsec_file, NULL};

        rtems_bsd_rc_conf_print_cmd(
            rc_conf, "setkey", RTEMS_BSD_ARGC(setkey), (const char**)setkey);
        r = rtems_bsd_command_setkey(RTEMS_BSD_ARGC(setkey), setkey);
        if (r != EXIT_SUCCESS) {
          fprintf(stderr,
              "error: setkey: Call to setkey failed.\n");
          erroroccured = -1;
        }
      }

      if (ipsec_file != NULL) {
        free(ipsec_file);
      }
    }
  }

  rtems_bsd_rc_conf_argc_argv_destroy(aa);

  return erroroccured;
}

static int
ike_service(rtems_bsd_rc_conf* rc_conf)
{
  rtems_bsd_rc_conf_argc_argv* aa;
  int r;
  int erroroccured = 0;

  aa = rtems_bsd_rc_conf_argc_argv_create();
  if (aa == NULL)
    return -1;

  r = rtems_bsd_rc_conf_find(rc_conf, "ike_enable", aa);
  if (r == 0) {
    if (aa->argc == 2 && strcasecmp("YES", aa->argv[1]) == 0) {
      const char *default_argv[] = {"racoon", NULL};
      const char **argv = default_argv;
      rtems_task_priority prio = RTEMS_MAXIMUM_PRIORITY - 1;
      int argc = 1;

      r = rtems_bsd_rc_conf_find(rc_conf, "ike_program", aa);
      if (r == 0) {
        if (aa->argc != 2 || strcasecmp("racoon", aa->argv[1]) != 0) {
          fprintf(stderr,
              "error: ike: Only \"racoon\" is supported as ike_program\n");
          erroroccured = -1;
        }
      }

      if (erroroccured == 0) {
        r = rtems_bsd_rc_conf_find(rc_conf, "ike_priority", aa);
        if (r == 0) {
          if (aa->argc == 2) {
            char *end;
            prio = strtoul(aa->argv[1], &end, 10);
            if (*end != '\0') {
              fprintf(stderr,
                  "error: ike: syntax error in ike_priority\n");
              erroroccured = -1;
            }
          }
        }
      }

      if (erroroccured == 0) {
        r = rtems_bsd_rc_conf_find(rc_conf, "ike_flags", aa);
        if (r == 0) {
          argc = aa->argc;
          argv = aa->argv;
        }
      }

      if (erroroccured == 0) {
        rtems_status_code sc;
        sc = rtems_bsd_racoon_daemon(argc, argv, prio);
        if (sc != RTEMS_SUCCESSFUL) {
          fprintf(stderr, "error: ike: Could not start racoon: %s\n",
              rtems_status_text(sc));
          erroroccured = -1;
        }
      }
    }
  }

  rtems_bsd_rc_conf_argc_argv_destroy(aa);

  return erroroccured;
}

void
rc_conf_ipsec_init(void* arg)
{
  int r;
  r = rtems_bsd_rc_conf_service_add("ipsec",
                                    "after:network;before:telnetd;",
                                    ipsec_service);
  if (r < 0)
    fprintf(stderr,
            "error: ipsec service add failed: %s\n", strerror(errno));
  r = rtems_bsd_rc_conf_service_add("ike",
                                    "after:ipsec;before:telnetd;",
                                    ike_service);
  if (r < 0)
    fprintf(stderr,
            "error: ike service add failed: %s\n", strerror(errno));
}
