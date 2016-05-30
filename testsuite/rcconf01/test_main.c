/*
 * Copyright 2016 Chris Johns <chrisj@rtems.org>
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

#include <rtems/bsd/sys/param.h>

#include <assert.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sysexits.h>

#include <machine/rtems-bsd-rc-conf.h>

#define TEST_NAME "LIBBSD RC.CONF 1"

static const char* rc_conf_regex = \
  "\n"                              /* empty line */ \
  "#\n"                             /* comment then nothing */  \
  "# comment\n"                     /* comment */
  "   #   \n"                       /* whitespace (ws), commend, ws */ \
  "test_regex_1=\n"                 /* name, empty value */ \
  "test_regex_2=\"\"\n"             /* name, quoted empty value */ \
  "test_regex_3=\"1 2 3\"\n"        /* name, 3 args */ \
  "  test_regex_4=\"04\"\n"         /* ws, name, 1 arg */ \
  "\ttest_regex_5=\"05\" \n"        /* ws, name, 1 arg, ws */ \
  "test_regex_6=\" 1  2\t 3 \"\n";  /* name then ws and 3 args */

#define NUM_OF_TEST_REGEX_ 6
static bool test_regex_results[NUM_OF_TEST_REGEX_];
static int  test_regex_last_num;

static const char* rc_conf_not_found = \
  "# invalid directive.\n" \
  "abc_def_0=\"not found\"\n";

static int
test_regex_(rtems_bsd_rc_conf* rc_conf, int argc, const char** argv)
{
  int num;
  int arg;

  rtems_bsd_rc_conf_print_cmd(rc_conf, "test_regex_", argc, argv);

  assert(strncasecmp(argv[0], "test_regex_", strlen("test_regex_")) == 0);
  num = atoi(argv[0] + strlen("test_regex_"));
  assert(num == (test_regex_last_num + 1));
  assert((num - 1) < NUM_OF_TEST_REGEX_);
  for (arg = 0; arg < argc; ++arg) {
    const char* a = argv[arg];
    size_t      l = strlen(a);
    if (l > 0) {
      assert(!isspace(a[0]));
      assert(!isspace(a[l - 1]));
      assert(a[0] != '"');
      assert(a[l - 1] != '"');
    }
  }
  test_regex_results[num - 1] = true;
  ++test_regex_last_num;

  return 0;
}

static void
make_rc_conf(const char* rc_conf)
{
  FILE* f;
  unlink(rc_conf); /* Ignore any errors */
  assert((f = fopen(rc_conf, "w")) != NULL);
  assert(fwrite(rc_conf_regex, strlen(rc_conf_regex), 1, f) == 1);
  assert(fclose(f) == 0);
}

static void
test_regex_check(void)
{
  int i;
  assert(test_regex_last_num == NUM_OF_TEST_REGEX_); /* all items found? */
  for (i = 0; i < NUM_OF_TEST_REGEX_; ++i)
    assert(test_regex_results[i]);
}

static void
test_etc_rc_conf(void)
{
  memset(&test_regex_results[0], 0, sizeof(test_regex_results));
  test_regex_last_num = 0;
  make_rc_conf("/etc/rc.conf");
  assert(rtems_bsd_run_etc_rc_conf(true) == 0);
  test_regex_check();
}

static void
test_rc_conf(void)
{
  memset(&test_regex_results[0], 0, sizeof(test_regex_results));
  test_regex_last_num = 0;
  make_rc_conf("/my_rc.conf");
  assert(rtems_bsd_run_rc_conf("/my_rc.conf", true) == 0);
  test_regex_check();
}

static void
test_rc_conf_script(void)
{
  memset(&test_regex_results[0], 0, sizeof(test_regex_results));
  test_regex_last_num = 0;
  assert(rtems_bsd_run_rc_conf_script("internal", rc_conf_regex, true) == 0);
  test_regex_check();
}

static void
test_rc_conf_script_not_found(void)
{
  assert(rtems_bsd_run_rc_conf_script("internal", rc_conf_not_found, true) < 0);
}

static void
setup(void)
{
  struct stat sb;
  mkdir("/etc", S_IRWXU | S_IRWXG | S_IRWXO); /* ignore errors, check the dir after. */
  assert(stat("/etc", &sb) == 0);
  assert(S_ISDIR(sb.st_mode));
  assert(rtems_bsd_rc_conf_directive_add("test_regex_.*", test_regex_) == 0);
}

static void
test_main(void)
{
  setup();

  test_etc_rc_conf();
  test_rc_conf();
  test_rc_conf_script();
  test_rc_conf_script_not_found();

  exit(0);
}

#include <rtems/bsd/test/default-init.h>
