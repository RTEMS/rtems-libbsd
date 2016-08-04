/*
 * Copyright (c) 2016 embedded brains GmbH.  All rights reserved.
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
 * Handle the PF firewall directives found in rc.conf.
 * - pf_enable
 * - pf_rules
 * - pf_flags
 *
 * Note: RTEMS ignores the
 * - pf_program
 * directive.
 */

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include <machine/rtems-bsd-commands.h>
#include <machine/rtems-bsd-rc-conf-services.h>

static char firewall_pf_rules_default[] = "/etc/pf.conf";

static int
firewall_pf_enable(rtems_bsd_rc_conf* rc_conf)
{
  char *args[] = {
    "pfctl",
    "-q",
    "-e",
    NULL
  };

  rtems_bsd_rc_conf_print_cmd(rc_conf, "pfctl", RTEMS_BSD_ARGC(args),
      (const char **) args);
  return rtems_bsd_command_pfctl(RTEMS_BSD_ARGC(args), args);
}

static int
firewall_pf_flush(
    rtems_bsd_rc_conf* rc_conf,
    char what[])
{
  char *flush[] = {
    "pfctl",
    "-q",
    "-F",
    what,
    NULL
  };

  rtems_bsd_rc_conf_print_cmd(rc_conf, "pfctl", RTEMS_BSD_ARGC(flush),
      (const char **) flush);
  return rtems_bsd_command_pfctl(RTEMS_BSD_ARGC(flush), flush);
}

/*
 * Executes something like the following command line:
 *   pfctl -f <rules> [<argv[1]> [<argv[2]> [..]]]
 */
static int
firewall_pf_load_rules_with_extra_args(
    rtems_bsd_rc_conf* rc_conf,
    char *rules,
    int argc,
    const char **argv)
{
  char** args;
  const int fixed_args = 2; /* "-f" and filename */
  int arg;
  int r = -1;
  bool prepare_failed = false;

  /*
   * argc/argv contains something like the following:
   *   argc = 3
   *   argv = {"pf_flags", "-i", "lo0"}
   *
   * Alternatively it might be
   *   argc = 0
   *   argv = NULL
   */

  /* The code below assumes an argc >= 1. Fake it if there are no additional
   * arguments. */
  if(argc == 0) {
    argc = 1;
  }

  args = calloc(argc + fixed_args + 1, sizeof(char*));
  if (args == NULL) {
    return -1;
  }

  args[0] = "pfctl";
  args[1] = "-f";
  args[2] = rules;
  args[argc + fixed_args] = NULL;

  for (arg = 1; arg < argc; ++arg) {
    args[arg + fixed_args] = strdup(argv[arg]);
    if (args[arg + fixed_args] == NULL) {
      prepare_failed = true;
      break;
    }
  }

  if (prepare_failed == false) {
    rtems_bsd_rc_conf_print_cmd(rc_conf, "pfctl", argc + fixed_args,
        (const char **) args);
    r = rtems_bsd_command_pfctl(argc + fixed_args, args);
  } else {
    r = EXIT_FAILURE;
  }

  for (arg = 1; arg < argc; ++arg) {
    free(args[arg + fixed_args]);
  }
  free(args);

  return r;
}

static int
firewall_pf_service(rtems_bsd_rc_conf* rc_conf)
{
  rtems_bsd_rc_conf_argc_argv* aa;
  int r;
  int erroroccured = 0;

  aa = rtems_bsd_rc_conf_argc_argv_create();
  if (aa == NULL)
    return -1;

  r = rtems_bsd_rc_conf_find(rc_conf, "pf_enable", aa);
  if (r == 0) {
    if (aa->argc == 2 && strcasecmp("YES", aa->argv[1]) == 0) {
      char* rules = firewall_pf_rules_default;
      int argc = 0;
      const char** argv = NULL;

      if (erroroccured == 0) {
        r = rtems_bsd_rc_conf_find(rc_conf, "pf_rules", aa);
        if (r == 0) {
          if (aa->argc == 2) {
            rules = strdup(aa->argv[1]);
            if (rules == NULL) {
              fprintf(stderr,
                  "error: pf: Could not create rule file string: %s\n",
                  strerror(errno));
              erroroccured = -1;
            }
          } else {
            fprintf(stderr,
                "error: pf: Syntax error in pf_rules directive.\n");
              erroroccured = -1;
          }
        }
      }

      if (erroroccured == 0) {
        r = rtems_bsd_rc_conf_find(rc_conf, "pf_flags", aa);
        if (r == 0) {
          argc = aa->argc;
          argv = aa->argv;
        }
      }

      /*
       * FreeBSD does the following on pf_start() in /etc/rc.d/pf:
       * ----
       *    check_startmsgs && echo -n 'Enabling pf'
       *    $pf_program -F all > /dev/null 2>&1
       *    $pf_program -f "$pf_rules" $pf_flags
       *    if ! $pf_program -s info | grep -q "Enabled" ; then
       *        $pf_program -eq
       *    fi
       *    check_startmsgs && echo '.'
       * ----
       * We should do roughly the same
       */
      if (erroroccured == 0) {
        r = firewall_pf_flush(rc_conf, "all");
        if(r != EXIT_SUCCESS) {
          fprintf(stderr,
              "error: pf: Could not flush.\n");
          erroroccured = -1;
        }
      }

      if (erroroccured == 0) {
        r = firewall_pf_load_rules_with_extra_args(rc_conf, rules, argc, argv);
        if (r != EXIT_SUCCESS) {
          fprintf(stderr,
              "error: pf: Could not load rules.\n");
          erroroccured = -1;
        }
      }

      if (erroroccured == 0) {
        r = firewall_pf_enable(rc_conf);
        if (r != EXIT_SUCCESS) {
          fprintf(stderr,
              "error: pf: Could not enable firewall.\n");
          erroroccured = -1;
        }
      }

      if (rules != firewall_pf_rules_default && rules != NULL) {
        free(rules);
      }
    }
  }

  rtems_bsd_rc_conf_argc_argv_destroy(aa);

  return erroroccured;
}

void
rc_conf_firewall_pf_init(void* arg)
{
  int r;
  r = rtems_bsd_rc_conf_service_add("pf",
                                    "after:network;before:telnetd;",
                                    firewall_pf_service);
  if (r < 0)
    fprintf(stderr,
            "error: pf service add failed: %s\n", strerror(errno));
}
