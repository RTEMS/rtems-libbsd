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
 * FTPD service. To use add to rc.conf:
 *
 * ftpd_enable="YES"
 * ftp_options="-p 21 -C 4 -P 100 -I 300 -R / -L -A rw"
 *
 * The options defaults are show. The options are:
 *
 *  -p port       : Port.
 *  -C conns      : Maximum connections.
 *  -P priority   : FTP thread priority (RTEMS Classis API).
 *  -I secs       : Idle timeout in seconds.
 *  -R path       : Root path.
 *  -L            : Logins using /etc/passwd
 *  -A <rw/rd/wr> : Access, rw=read/write, rd=read, wr=write.
 */

#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define __need_getopt_newlib
#include <getopt.h>

#include <rtems/ftpd.h>
#include <machine/rtems-bsd-rc-conf-services.h>

static int
ftpd_service(rtems_bsd_rc_conf* rc_conf)
{
  struct rtems_ftpd_configuration config = {
    .priority = 100,        /* FTPD task priority */
    .max_hook_filesize = 0, /* Maximum buffersize for hooks */
    .port = 21,             /* Well-known port */
    .hooks = NULL,          /* List of hooks */
    .root = NULL,           /* Root for FTPD or NULL for "/" */
    .tasks_count = 4,       /* Max. connections */
    .idle = 5 * 60,         /* Idle timeout in seconds  or 0 for no (infinite) timeout */
    .access = 0,            /* Access: 0 - r/w, 1 - read-only, 2 - write-only,
                             * 3 - browse-only */
    .login = NULL           /* Login */
  };
  rtems_bsd_rc_conf_argc_argv* aa;
  int                          r;

  aa = rtems_bsd_rc_conf_argc_argv_create();
  if (aa == NULL)
    return -1;

  r = rtems_bsd_rc_conf_find(rc_conf, "ftpd_enable", aa);
  if (r == 0) {
    rtems_status_code sc;
    bool              verbose = false;
    if (aa->argc == 2 && strcasecmp("YES", aa->argv[1]) == 0) {
      r = rtems_bsd_rc_conf_find(rc_conf, "ftpd_options", aa);
      if (r == 0) {
        struct getopt_data data;
        char*              end;

        memset(&data, 0, sizeof(data));

        while (true) {
          int c;

          c = getopt_r(aa->argc, (char * const *)aa->argv, "p:C:P:I:R:LA:v", &data);
          if (c == -1)
            break;

          switch (c) {
          case 'p':
            config.port = strtoul(data.optarg, &end, 10);
            if (config.port == 0 || *end != '\0') {
              fprintf(stderr, "error: ftpd: invalid port\n");
              config.port = 21;
            }
            break;
          case 'C':
            config.tasks_count = strtoul(data.optarg, &end, 10);
            if (config.tasks_count == 0 || *end != '\0') {
              fprintf(stderr, "error: ftpd: invalid port\n");
              config.tasks_count = 4;
            }
            break;
          case 'P':
            config.priority = strtoul(data.optarg, &end, 10);
            if (config.priority == 0 || *end != '\0') {
              fprintf(stderr, "error: ftpd: invalid priority\n");
              config.priority = 100;
            }
            break;
          case 'I':
            config.idle = strtoul(data.optarg, &end, 10);
            if (config.tasks_count == 0 || *end != '\0') {
              fprintf(stderr, "error: ftpd: invalid idle seconds\n");
              config.idle = 5 * 60;
            }
            break;
          case 'R':
            config.root = strdup(data.optarg);
            if (config.root == NULL) {
              fprintf(stderr, "error: ftpd: no memory for root path\n");
            }
            break;
          case 'L':
            config.login = rtems_shell_login_check;
            break;
          case 'A':
            if (strcasecmp("rw", data.optarg) == 0)
              config.access = 0;
            else if (strcasecmp("rd", data.optarg) == 0)
              config.access = 1;
            else if (strcasecmp("wr", data.optarg) == 0)
              config.access = 2;
            else
              fprintf(stderr, "error: ftp: invalid access option\n");
            break;
          case 'v':
            verbose = true;
            break;
          case '?':
          default:
            fprintf(stderr, "error: ftp: unknown option: %s\n", data.optarg);
            break;
          }
        }
      }
      if (verbose) {
        printf("ftpd: port:%lu conns:%lu pri:%lu idle:%lu root:%s login:%s access:%s\n",
               config.port, config.tasks_count, config.priority, config.idle,
               config.root == NULL ? "/" : config.root,
               config.login == NULL ? "no" : "yes",
               config.access == 0 ? "read/write" : config.access == 1 ? "read" : "write");
      }
      sc = rtems_ftpd_start(&config);
      if (sc != RTEMS_SUCCESSFUL)
        fprintf(stderr, "error: ftpd: %s\n", rtems_status_text(sc));
    }
  }

  return 0;
}

void
rc_conf_ftpd_init(void* arg)
{
  int r;
  r = rtems_bsd_rc_conf_service_add("ftpd",
                                    "after:network;",
                                    ftpd_service);
  if (r < 0)
    fprintf(stderr, "error: ftpd service add failed: %s\n", strerror(errno));
}
