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

/*
 * Max line length.
 */
#define MAX_LINE_SIZE (512)

/*
 * Directive handler chain.
 */
struct rtems_bsd_rc_conf_ {
  const char*  name;       /**< Name of the file. */
  const char*  data;       /**< Pre-processed rc.conf data. */
  size_t       line_count; /**< Number of lines with text. */
  const char** lines;      /**< The lines in the file's text. */
  size_t       line;       /**< The line being processed. */
  bool         verbose;    /**< Verbose processing. */
};

/*
 * Directive handler chain.
 */
typedef struct {
  rtems_chain_node            node;
  const char*                 directive;
  rtems_bsd_rc_conf_directive handler;
} directive;

/*
 * Control of argc and argv.
 */
typedef struct {
  char*        command;
  int          argc;
  const char** argv;
} argc_argv;

/*
 * The chain of directives.
 */
static RTEMS_CHAIN_DEFINE_EMPTY(directives);

static int
argc_argv_create(const char* line, argc_argv* aa)
{
  char* c;
  int   arg;
  char* brk;

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

static void
argc_argv_destroy(argc_argv* aa)
{
  free(aa->argv);
  free(aa->command);
  aa->argv = NULL;
  aa->command = NULL;
}

static int
rc_conf_create(rtems_bsd_rc_conf* rc_conf,
               const char*        name,
               const char*        text,
               bool               verbose)
{
  size_t       length;
  char*        line;
  char*        end;
  char*        copy;
  char*        marker;
  const char** lines;
  size_t       line_count;

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

  rc_conf->name = name;
  rc_conf->data = copy;
  rc_conf->line_count = line_count;
  rc_conf->lines = lines;
  rc_conf->line = 0;
  rc_conf->verbose = verbose;

  return 0;
}

static void
rc_conf_destroy(rtems_bsd_rc_conf* rc_conf)
{
  free((void*) rc_conf->lines);
  free((void*) rc_conf->data);
  rc_conf->data = NULL;
  rc_conf->name = NULL;
}


static int
parse_line(rtems_bsd_rc_conf* rc_conf)
{
  const char*             line;
  rtems_chain_node*       node = rtems_chain_first(&directives);
  const rtems_chain_node* tail = rtems_chain_tail(&directives);
  argc_argv               aa;
  int                     r;

  line = rc_conf->lines[rc_conf->line];

  if (*line == '\0')
    return 0;

  r = argc_argv_create(line, &aa);
  if (r < 0) {
    fprintf(stderr, "error: %s:%lu: creating argc/argv: %s\n",
            rc_conf->name, rc_conf->line + 1, strerror(errno));
    return r;
  }

  while (node != tail) {
    directive* dir = (directive*) node;
    regex_t    rege;
    #define    MAX_MATCHES 1
    regmatch_t matches[MAX_MATCHES];

    r = regcomp(&rege, dir->directive, REG_EXTENDED);
    if (r != 0) {
      char rerror[128];
      regerror(r, &rege, rerror, sizeof(rerror));
      fprintf(stderr, "error: %s:%lu: %s\n",
              rc_conf->name, rc_conf->line + 1, rerror);
      argc_argv_destroy(&aa);
      return -1;
    }

    r = regexec(&rege, aa.argv[0], MAX_MATCHES, matches, 0);
    if (r != 0 && r != REG_NOMATCH) {
      char rerror[128];
      regerror(r, &rege, rerror, sizeof(rerror));
      fprintf(stderr, "error: %s:%lu: %s\n",
              rc_conf->name, rc_conf->line + 1, rerror);
      regfree(&rege);
      argc_argv_destroy(&aa);
      return -1;
    }

    if (r == 0) {
      r = dir->handler(rc_conf, aa.argc, aa.argv);
      if (r < 0)
        fprintf(stderr, "error: %s:%lu: runtime error: %s: %s\n",
                rc_conf->name, rc_conf->line + 1, aa.argv[0], strerror(errno));
      regfree(&rege);
      argc_argv_destroy(&aa);
      return r;
    }

    regfree(&rege);

    node = rtems_chain_next(node);
  }

  errno = ENOSYS;

  fprintf(stderr, "error: %s:%lu: %s: configuration name is not supported\n",
          rc_conf->name, rc_conf->line + 1, aa.argv[0]);

  argc_argv_destroy(&aa);

  return -1;
}

int
rtems_bsd_rc_conf_directive_add(const char*                 dir_regex,
                                rtems_bsd_rc_conf_directive handler)
{
  directive* dir;

  dir = malloc(sizeof(*dir));
  if (dir == NULL) {
    errno = ENOMEM;
    return -1;
  }

  memset(dir, 0, sizeof(*dir));

  dir->directive = strdup(dir_regex);
  if (dir->directive == NULL) {
    free(dir);
    errno = ENOMEM;
    return -1;
  }

  dir->handler = handler;

  rtems_chain_append(&directives, &dir->node);

  return 0;
}

int
rtems_bsd_run_rc_conf_script(const char* name, const char* text, bool verbose)
{
  rtems_bsd_rc_conf rc_conf;
  int               r;

  if (verbose)
    printf("rc.conf: processing: %s size:%lu\n", name, strlen(text));

  r = rc_conf_create(&rc_conf, name, text, verbose);
  if (r < 0) {
    fprintf(stderr, "error: %s: parse error: %s\n",
            name, strerror(errno));
    return -1;
  }

  while (rc_conf.line < rc_conf.line_count) {
    r = parse_line(&rc_conf);
    if (r < 0)
      break;
    ++rc_conf.line;
  }

  rc_conf_destroy(&rc_conf);

  return r;
}

int
rtems_bsd_run_rc_conf(const char* name, bool verbose)
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

  r = rtems_bsd_run_rc_conf_script(name, rc_conf, verbose);

  free(rc_conf);

  return r;
}

int
rtems_bsd_run_etc_rc_conf(bool verbose)
{
  return rtems_bsd_run_rc_conf("/etc/rc.conf", verbose);
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
    printf("rc.conf: %s:%lu: %s:", rc_conf->name, rc_conf->line + 1, name);
    for (arg = 0; arg < argc; ++arg)
      printf(" %s", argv[arg]);
    printf("\n");
  }
}
