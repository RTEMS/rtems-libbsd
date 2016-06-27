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
 * Parse the /etc/rc.conf format file and execute the configuration options
 * we want to support.
 */


#include <ctype.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <regex.h>

#include <rtems.h>
#include <rtems/chain.h>

#include <machine/rtems-bsd-rc-conf.h>
#include <machine/rtems-bsd-rc-conf-services.h>

/*
 * By default the networking service is always available.
 */
RTEMS_BSD_RC_CONF_SYSINT(rc_conf_net);

/*
 * Max line length.
 */
#define MAX_LINE_SIZE (512)

/*
 * Intrenal rc.conf data. This is not visible out side of here.
 */
struct rtems_bsd_rc_conf_ {
  const char*  name;          /**< Name of the file. */
  const char*  data;          /**< Pre-processed rc.conf data. */
  size_t       line_count;    /**< Number of lines with text. */
  const char** lines;         /**< The lines in the file's text. */
  size_t       line;          /**< The line being processed. */
  char*        find_regex;    /**< Find's regular expression. */
  int          timeout;       /**< The timeout for processing rc.conf. */
  bool         verbose;       /**< Verbose processing. */
  int          error_code;    /**< The error code returned to the caller. */
  rtems_id     lock;          /**< Threading lock for this data. */
  rtems_id     waiter;        /**< The waiting thread, 0 if no one waiting */
};

/*
 * Services handler chain.
 */
typedef struct {
  rtems_chain_node          node;
  const char*               name;
  const char*               control;
  const char*               before;
  const char*               after;
  const char*               require;
  rtems_bsd_rc_conf_service entry;
} service;

/*
 * The chain of services.
 */
static RTEMS_CHAIN_DEFINE_EMPTY(services);

#define ARGC_ARGV_MARKER (0x20010928)

static int
argc_argv_valid(rtems_bsd_rc_conf_argc_argv* aa)
{
  if (aa->marker != ARGC_ARGV_MARKER) {
    errno = EACCES;
    return -1;
  }
  return 0;
}

static void
argc_argv_clean(rtems_bsd_rc_conf_argc_argv* aa)
{
  if (argc_argv_valid(aa) >= 0) {
    free(aa->argv);
    free(aa->command);
    aa->argv = NULL;
    aa->command = NULL;
  }
}

static int
argc_argv_update(const char* line, rtems_bsd_rc_conf_argc_argv* aa)
{
  char* c;
  int   arg;
  char* brk;

  if (argc_argv_valid(aa) < 0)
    return -1;

  argc_argv_clean(aa);

  if (strnlen(line, MAX_LINE_SIZE) >= MAX_LINE_SIZE) {
    errno = EFBIG;
    return -1;
  }

  aa->command = strdup(line);
  if (aa->command == NULL) {
    errno = ENOMEM;
    return -1;
  }

  aa->argc = 0;
  aa->argv = NULL;

  /*
   * Yes, the man page says do not use strtok. This is a local oopy, and the
   * re-entrant version is being used.
   */
  c = aa->command;
  while (true) {
    c = strtok_r(c, " \t=\"", &brk);
    if (c == NULL)
      break;
    ++aa->argc;
    c = NULL;
  }

  /*
   * Plus 1 for the trailing NULL present in argv lists.
   */
  aa->argv = calloc(aa->argc + 1, sizeof(char*));
  if (aa->argv == NULL) {
    free(aa->command);
    errno = ENOMEM;
    return -1;
  }

  aa->argv[0] = aa->command;
  c = aa->command;

  for (arg = 1; arg < aa->argc; ++arg) {
    c += strlen(c) + 1;
    while (*c != '\0' && (*c == '"' || isspace(*c)))
      ++c;
    aa->argv[arg] = c;
  }

  return 0;
}

static int
lock_create(rtems_bsd_rc_conf* rc_conf)
{
  rtems_status_code sc;
  sc = rtems_semaphore_create(rtems_build_name('B', 'S', 'D', 'r'),
                              1,
                              RTEMS_FIFO | RTEMS_BINARY_SEMAPHORE |
                              RTEMS_NO_INHERIT_PRIORITY | RTEMS_NO_PRIORITY_CEILING |
                              RTEMS_LOCAL,
                              0,
                              &rc_conf->lock);
  if (sc != RTEMS_SUCCESSFUL) {
    fprintf(stderr, "error: rc_conf: cannot create lock: %s", rtems_status_text(sc));
    errno = EIO;
    return -1;
  }
  return 0;
}

static int
lock_delete(rtems_bsd_rc_conf* rc_conf)
{
  rtems_semaphore_delete(rc_conf->lock);
  return 0;
}

static int
lock(rtems_bsd_rc_conf* rc_conf)
{
  rtems_status_code sc;
  sc = rtems_semaphore_obtain(rc_conf->lock, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  if (sc != RTEMS_SUCCESSFUL) {
    fprintf(stderr, "error: rc_conf: locking: %s", rtems_status_text(sc));
    errno = EIO;
    return -1;
  }
  return 0;
}

static int
unlock(rtems_bsd_rc_conf* rc_conf)
{
  rtems_status_code sc;
  sc = rtems_semaphore_release(rc_conf->lock);
  if (sc != RTEMS_SUCCESSFUL) {
    fprintf(stderr, "error: rc_conf: locking: %s", rtems_status_text(sc));
    errno = EIO;
    return -1;
  }
  return 0;
}

static int
rc_conf_create(rtems_bsd_rc_conf* rc_conf,
               const char*        name,
               const char*        text,
               int                timeout,
               bool               verbose)
{
  size_t       length;
  char*        line;
  char*        end;
  char*        copy;
  char*        marker;
  const char** lines;
  size_t       line_count;
  int          r;

  memset(rc_conf, 0, sizeof(*rc_conf));

  /*
   * Range check, this makes the rest safer in respect to buffer overflows.
   */
  length = strnlen(text, RTEMS_BSD_RC_CONF_MAX_SIZE);
  if (length == RTEMS_BSD_RC_CONF_MAX_SIZE) {
    errno = E2BIG;
    return -1;
  }

  copy = strdup(text);
  if (copy == NULL) {
    errno = ENOMEM;
    return -1;
  }

  end = copy + length;

  /*
   * Break the text up into lines.
   */
  line_count = 0;
  for (line = copy; line != end; ++line) {
    if (*line == '\n') {
      line_count++;
      *line = '\0';
    }
  }

  lines = malloc(sizeof(char*) * line_count);
  if (lines == NULL) {
    free(copy);
    errno = ENOMEM;
    return -1;
  }

  memset(lines, 0, sizeof(char*) * line_count);

  /*
   * Extract all the lines.
   */
  line_count = 0;
  for (marker = line = copy; line != end; ++line) {
    if (*line == '\0' && line != end) {
      /*
       * Look for a comment.
       */
      char* comment = strchr(marker, '#');
      if (comment != NULL)
        *comment = '\0';
      /*
       * Remove leading whitespace.
       */
      length = strlen(marker);
      while (*marker != '\0' && isspace(*marker))
        memmove(marker, marker + 1, length--);
      /*
       * Remove trailing whitespace.
       */
      length = strlen(marker) - 1;
      while (length > 0 && isspace(marker[length]))
        marker[length--] = '\0';
      /*
       * Set the line.
       */
      lines[line_count] = marker;
      ++line_count;
      marker = line + 1;
    }
  }

  rc_conf->name = strdup(name);
  rc_conf->data = copy;
  rc_conf->line_count = line_count;
  rc_conf->lines = lines;
  rc_conf->line = 0;
  rc_conf->timeout = timeout;
  rc_conf->verbose = verbose;
  if (timeout >= 0)
    rc_conf->waiter = rtems_task_self();

  /*
   * Create the lock.
   */
  r = lock_create(rc_conf);
  if (r < 0) {
    free((void*) rc_conf->name);
    free((void*) rc_conf->lines);
    free((void*) rc_conf->data);
    return -1;
  }

  return 0;
}

static void
rc_conf_destroy(rtems_bsd_rc_conf* rc_conf)
{
  if (rc_conf->name != NULL) {
    free((void*) rc_conf->name);
    free((void*) rc_conf->lines);
    free((void*) rc_conf->data);
    rc_conf->name = NULL;
    rc_conf->lines = NULL;
    rc_conf->data = NULL;
    lock_delete(rc_conf);
  }
}

rtems_bsd_rc_conf_argc_argv*
rtems_bsd_rc_conf_argc_argv_create(void)
{
  rtems_bsd_rc_conf_argc_argv* aa = calloc(1, sizeof(rtems_bsd_rc_conf_argc_argv));
  if (aa != NULL)
    aa->marker = ARGC_ARGV_MARKER;
  return aa;
}

void
rtems_bsd_rc_conf_argc_argv_destroy(rtems_bsd_rc_conf_argc_argv* aa)
{
  if (aa->marker == ARGC_ARGV_MARKER) {
    argc_argv_clean(aa);
    free(aa);
  }
}

int rtems_bsd_rc_conf_find_next(rtems_bsd_rc_conf*           rc_conf,
                                rtems_bsd_rc_conf_argc_argv* argc_argv)
{

  if (argc_argv_valid(argc_argv) < 0)
    return -1;

  while (rc_conf->line < rc_conf->line_count) {
    const char* line;
    regex_t     rege;
    #define     MAX_MATCHES 1
    regmatch_t  matches[MAX_MATCHES];
    int         r;

    line = rc_conf->lines[rc_conf->line];
    ++rc_conf->line;

    if (*line == '\0')
      continue;

    if (argc_argv_update(line, argc_argv) < 0)
      return -1;

    r = regcomp(&rege, rc_conf->find_regex, REG_EXTENDED);
    if (r != 0) {
      char rerror[128];
      regerror(r, &rege, rerror, sizeof(rerror));
      fprintf(stderr, "error: %s:%lu: %s\n",
              rc_conf->name, rc_conf->line + 1, rerror);
      return -1;
    }

    r = regexec(&rege, argc_argv->argv[0], MAX_MATCHES, matches, 0);
    if (r != 0 && r != REG_NOMATCH) {
      char rerror[128];
      regerror(r, &rege, rerror, sizeof(rerror));
      fprintf(stderr, "error: %s:%lu: %s\n",
              rc_conf->name, rc_conf->line + 1, rerror);
      regfree(&rege);
      return -1;
    }

    regfree(&rege);

    /*
     * Match found.
     */
    if (r == 0)
      return 0;

    regfree(&rege);
  }

  errno = ENOENT;

  return -1;
}

int rtems_bsd_rc_conf_find(rtems_bsd_rc_conf*           rc_conf,
                           const char*                  expression,
                           rtems_bsd_rc_conf_argc_argv* argc_argv)
{
  if (argc_argv_valid(argc_argv) < 0)
    return -1;
  free(rc_conf->find_regex);
  rc_conf->find_regex = strdup(expression);
  rc_conf->line = 0;
  if (rc_conf->find_regex == NULL) {
    errno = ENOMEM;
    return -1;
  }
  return rtems_bsd_rc_conf_find_next(rc_conf, argc_argv);
}

int
rtems_bsd_rc_conf_service_add(const char*               name,
                              const char*               control,
                              rtems_bsd_rc_conf_service entry)
{
  service* srv;
  char*    ctl = NULL;
  char*    s;
  char*    c;

  srv = malloc(sizeof(*srv));
  if (srv == NULL) {
    errno = ENOMEM;
    return -1;
  }

  memset(srv, 0, sizeof(*srv));

  srv->name = strdup(name);
  if (control != NULL) {
    ctl = strdup(control);
    srv->control = ctl;
  }
  srv->entry = entry;

  if (srv->name == NULL || (control != NULL && ctl == NULL)) {
    fprintf(stderr, "error: rc.conf: add service: no memory\n");
    free((void*) srv->control);
    free((void*) srv->name);
    free(srv);
    errno = ENOMEM;
    return -1;
  }

  if (control != NULL) {
    s = c = ctl;

    while (*c != '\0') {
      if (*c == ';') {
        *c = '\0';

        if (strncasecmp("before:", s, sizeof("before:") - 1) == 0) {
          if (srv->before == NULL) {
            srv->before = s + sizeof("before:") - 1;
            s = NULL;
          }
          else {
            fprintf(stderr, "error: rc.conf: add service: repeated 'before'\n");
            c = NULL;
          }
        }
        else if (strncasecmp("after:", s, sizeof("after:") - 1) == 0) {
          if (srv->after == NULL) {
            srv->after = s + sizeof("after:") - 1;
            s = NULL;
          }
          else {
            fprintf(stderr, "error: rc.conf: add service: repeated 'after'\n");
            c = NULL;
          }
        }
        else if (strncasecmp("require:", s, sizeof("require:") - 1) == 0) {
          if (srv->require == NULL) {
            srv->require = s + sizeof("require:") - 1;
            s = NULL;
          }
          else {
            fprintf(stderr, "error: rc.conf: add service: repeated 'require'\n");
            c = NULL;
          }
        }
        else {
          fprintf(stderr, "error: rc.conf: add service: unknown keyword: %s\n", s);
          c = NULL;
        }

        if (c == NULL) {
          free((void*) srv->control);
          free((void*) srv->name);
          free(srv);
          errno = EINVAL;
          return -1;
        }
      }
      else if (s == NULL) {
        s = c;
      }

      ++c;
    }

    if (s != NULL) {
      fprintf(stderr, "error: rc.conf: add service: no ';' found\n");
      free((void*) srv->control);
      free((void*) srv->name);
      free(srv);
      errno = EINVAL;
      return -1;
    }

    /*
     * Place on the services list. The node is removed before being inserted. If
     * there are competing positions the last position is used. As a result
     * handle 'after' before 'before'.
     */
    rtems_chain_prepend(&services, &srv->node);
  }
  else {
    /*
     * No control string, add the end.
     */
    rtems_chain_append(&services, &srv->node);
  }

  /*
   * After.
   */
  if (srv->after != NULL) {
    const char* cc = srv->after;
    while (*cc != '\0') {
      const char* cs = cc;
      size_t      l;
      while (*cc != ',' && *cc != '\0')
        ++cc;
      l = cc - cs;
      if (strncasecmp(cs, "last", l) == 0) {
        fprintf(stderr,
                "error: rc.conf: add service: 'last' in 'after': %s\n",
                control);
        rtems_chain_extract(&srv->node);
        free((void*) srv->control);
        free((void*) srv->name);
        free(srv);
        errno = EINVAL;
        return -1;
      }
      else if (strncasecmp(cs, "first", l) == 0) {
        /* already prepended */
      }
      else {
        rtems_chain_node* node = rtems_chain_first(&services);
        while (!rtems_chain_is_tail(&services, node)) {
          service* ss = (service*) node;
          if (ss != srv &&
              strlen(ss->name) == l && strncasecmp(ss->name, cs, l) == 0) {
            rtems_chain_extract(&srv->node);
            rtems_chain_insert(&ss->node, &srv->node);
            break;
          }
          node = rtems_chain_next(node);
        }
      }
    }
  }

  /*
   * Before.
   */
  if (srv->before != NULL) {
    const char* cc = srv->before;
    while (*cc != '\0') {
      const char* cs = cc;
      size_t      l;
      while (*cc != ',' && *cc != '\0')
        ++cc;
      l = cc - cs;
      if (strncasecmp(cs, "first", l) == 0) {
        fprintf(stderr, "error: rc.conf: add service: 'first' in 'before'\n");
        rtems_chain_extract(&srv->node);
        free((void*) srv->control);
        free((void*) srv->name);
        free(srv);
        errno = EINVAL;
        return -1;
      }
      else if (strncasecmp(cs, "last", l) == 0) {
        rtems_chain_extract(&srv->node);
        rtems_chain_append(&services, &srv->node);
      }
      else {
        rtems_chain_node* node = rtems_chain_first(&services);
        while (!rtems_chain_is_tail(&services, node)) {
          service* ss = (service*) node;
          if (strlen(ss->name) == l && strncasecmp(ss->name, cs, l) == 0) {
            rtems_chain_extract(&srv->node);
            if (rtems_chain_is_first(node))
              rtems_chain_prepend(&services, &srv->node);
            else {
              service* sp = (service*) rtems_chain_previous(node);
              rtems_chain_insert(&sp->node, &srv->node);
            }
            break;
          }
          node = rtems_chain_next(node);
        }
      }
    }
  }

  return 0;
}

int
rtems_bsd_rc_conf_service_remove(const char* name)
{
  rtems_chain_node*       node = rtems_chain_first(&services);
  const rtems_chain_node* tail = rtems_chain_tail(&services);

  while (node != tail) {
    service* srv = (service*) node;

    if (strcasecmp(name, srv->name) == 0) {
      rtems_chain_extract(&srv->node);
      free((void*) srv->control);
      free((void*) srv->name);
      free(srv);
      return 0;
    }

    node = rtems_chain_next(node);
  }

  errno = ENOENT;
  return -1;
}

static rtems_task rc_conf_worker(rtems_task_argument task_argument)
{
  rtems_bsd_rc_conf* rc_conf = (rtems_bsd_rc_conf*) task_argument;
  rtems_chain_node* node = rtems_chain_first(&services);
  int               r = 0;
  int               error;

  if (rc_conf->verbose)
    printf("rc.conf: running\n");

  while (!rtems_chain_is_tail(&services, node)) {
    service* srv = (service*) node;
    int      rr;
    if (rc_conf->verbose)
      printf("BSD service: %s\n", srv->name);
    rr = srv->entry(rc_conf);
    if (rr < 0) {
      fprintf(stderr, "error: bsd service: %s: %s\n", srv->name, strerror(errno));
      if (r == 0) {
        r = rr;
        error = errno;
      }
    }
    node = rtems_chain_next(node);
  }

  if (rc_conf->verbose)
    printf("rc.conf: services done\n");

  lock(rc_conf);

  if (r < 0)
    rc_conf->error_code = error;

  /*
   * If there is a waiter signal else clean up because the waiter has gone.
   */
  if (rc_conf->waiter != 0) {
    rtems_event_send(rc_conf->waiter, RTEMS_EVENT_1);
    unlock(rc_conf);
  }
  else {
    unlock(rc_conf);
    rc_conf_destroy(rc_conf);
  }

  if (rc_conf->verbose)
    printf("rc.conf: finished\n");

  rtems_task_delete(RTEMS_SELF);
}

int
rtems_bsd_run_rc_conf_script(const char* name,
                             const char* text,
                             int         timeout,
                             bool        verbose)
{
  rtems_bsd_rc_conf   rc_conf;
  rtems_task_priority priority;
  rtems_id            worker;
  rtems_status_code   sc;
  int                 r = 0;

  if (verbose)
    printf("rc.conf: start: %s size:%lu, timeout: %i\n",
           name, strlen(text), timeout);

  r = rc_conf_create(&rc_conf, name, text, timeout, verbose);
  if (r < 0) {
    fprintf(stderr, "error: %s: parse error: %s\n",
            name, strerror(errno));
    return -1;
  }

  sc = rtems_task_set_priority(RTEMS_SELF, RTEMS_CURRENT_PRIORITY, &priority);
  if (sc != RTEMS_SUCCESSFUL) {
    fprintf(stderr, "error: %s: get priority: %s\n",
            name, rtems_status_text(sc));
    errno = EIO;
    return -1;
  }

  sc = rtems_task_create(rtems_build_name('B', 'S', 'D', 'r' ),
                         priority,
                         8 * 1024,
                         RTEMS_PREEMPT | RTEMS_NO_TIMESLICE | RTEMS_NO_ASR,
                         RTEMS_LOCAL | RTEMS_FLOATING_POINT,
                         &worker);
  if (sc != RTEMS_SUCCESSFUL) {
    fprintf (stderr, "error: worker create: %s", rtems_status_text(sc));
    errno = EIO;
    return -1;
  }

  sc = rtems_task_start(worker,
                        rc_conf_worker,
                        (rtems_task_argument) &rc_conf);
  if (sc != RTEMS_SUCCESSFUL) {
    fprintf (stderr, "error: worker start: %s", rtems_status_text(sc));
    errno = EIO;
    return - 1;
  }

  if (timeout >= 0) {
    rtems_event_set out = 0;
    rtems_interval  ticks;

    if (timeout == 0)
      ticks = RTEMS_NO_TIMEOUT;
    else
      ticks = RTEMS_MICROSECONDS_TO_TICKS(timeout * 1000000UL);

    sc = rtems_event_receive(RTEMS_EVENT_1,
                             RTEMS_WAIT | RTEMS_EVENT_ANY,
                             ticks,
                             &out);
    if (sc != RTEMS_SUCCESSFUL) {
      if (sc != RTEMS_TIMEOUT) {
        fprintf (stderr, "error: worker event in: %s", rtems_status_text(sc));
        errno = EIO;
      }
      else {
        lock(&rc_conf);
        rc_conf.waiter = 0;
        unlock(&rc_conf);
        errno = ETIMEDOUT;
      }
      r = -1;
    }
    else {
      lock(&rc_conf);
      errno = rc_conf.error_code;
      if (errno != 0)
        r = -1;
      unlock(&rc_conf);
      rc_conf_destroy(&rc_conf);
    }
  }

  return r;
}

int
rtems_bsd_run_rc_conf(const char* name, int timeout, bool verbose)
{
  struct stat sb;
  int         r;
  char*       rc_conf;
  FILE*       file;

  r = stat(name, &sb);
  if (r < 0)
    return r;

  rc_conf = malloc(sb.st_size);
  if (rc_conf == NULL) {
    errno = ENOMEM;
    return -1;
  }

  if (verbose)
    printf("rc.conf: loading: %s\n", name);

  file = fopen(name, "r");
  if (file == NULL) {
    free(rc_conf);
    return -1;
  }

  if (fread(rc_conf, 1, sb.st_size, file) != sb.st_size) {
    fclose(file);
    free(rc_conf);
    return -1;
  }

  fclose(file);

  r = rtems_bsd_run_rc_conf_script(name, rc_conf, timeout, verbose);

  free(rc_conf);

  return r;
}

int
rtems_bsd_run_etc_rc_conf(int timeout, bool verbose)
{
  return rtems_bsd_run_rc_conf("/etc/rc.conf", timeout, verbose);
}

const char*
rtems_bsd_rc_conf_name(rtems_bsd_rc_conf* rc_conf)
{
  return rc_conf->name;
}

int
rtems_bsd_rc_conf_line(rtems_bsd_rc_conf* rc_conf)
{
  return rc_conf->line + 1;
}

bool
rtems_bsd_rc_conf_verbose(rtems_bsd_rc_conf* rc_conf)
{
  return rc_conf->verbose;
}

void rtems_bsd_rc_conf_print_cmd(rtems_bsd_rc_conf* rc_conf,
                                 const char*        name,
                                 int                argc,
                                 const char**       argv)
{
  if (rc_conf->verbose) {
    int arg;
    printf("rc.conf: %s: %s:", rc_conf->name, name);
    for (arg = 0; arg < argc; ++arg)
      printf(" %s", argv[arg]);
    printf("\n");
  }
}
