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

#include <sys/param.h>

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sysexits.h>
#include <unistd.h>

#include <machine/rtems-bsd-rc-conf.h>
#include <machine/rtems-bsd-rc-conf-services.h>

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
static int  test_service_last_num;

static const char* rc_conf_not_found = \
  "# invalid directive.\n" \
  "abc_def_0=\"not found\"\n";

static int
test_service(rtems_bsd_rc_conf* rc_conf)
{
  rtems_bsd_rc_conf_argc_argv* aa;
  int                          r;

  test_service_last_num = 1;

  /*
   * Fill match of anything.
   */
  puts("test_service regex: 'test_regex_.*'");
  test_regex_last_num = 0;

  assert((aa = rtems_bsd_rc_conf_argc_argv_create()) != NULL);
  r = rtems_bsd_rc_conf_find(rc_conf, "test_regex_.*", aa);
  assert(r == 0 || (r < 0 && errno == ENOENT));
  if (r < 0 && errno == ENOENT)
    return -1;

  while (r == 0) {
    int num;
    int arg;
    rtems_bsd_rc_conf_print_cmd(rc_conf, "test_service[1]", aa->argc, aa->argv);
    assert(strncasecmp(aa->argv[0], "test_regex_", strlen("test_regex_")) == 0);
    num = atoi(aa->argv[0] + strlen("test_regex_"));
    assert(num == (test_regex_last_num + 1));
    assert((num - 1) < NUM_OF_TEST_REGEX_);
    for (arg = 0; arg < aa->argc; ++arg) {
      const char* a = aa->argv[arg];
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
    r = rtems_bsd_rc_conf_find_next(rc_conf, aa);
    assert(r == 0 || (r < 0 && errno == ENOENT));
  }

  /*
   * Specific match of only numbers.
   */
  puts("test_service regex: 'test_regex_[0-9]+'");
  test_regex_last_num = 0;

  assert((aa = rtems_bsd_rc_conf_argc_argv_create()) != NULL);
  r = rtems_bsd_rc_conf_find(rc_conf, "test_regex_[0-9]+", aa);
  assert(r == 0 || (r < 0 && errno == ENOENT));
  if (r < 0 && errno == ENOENT)
    return -1;

  while (r == 0) {
    int num;
    int arg;
    rtems_bsd_rc_conf_print_cmd(rc_conf, "test_service[2]", aa->argc, aa->argv);
    assert(strncasecmp(aa->argv[0], "test_regex_", strlen("test_regex_")) == 0);
    num = atoi(aa->argv[0] + strlen("test_regex_"));
    assert(num == (test_regex_last_num + 1));
    assert((num - 1) < NUM_OF_TEST_REGEX_);
    for (arg = 0; arg < aa->argc; ++arg) {
      const char* a = aa->argv[arg];
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
    r = rtems_bsd_rc_conf_find_next(rc_conf, aa);
    assert(r == 0 || (r < 0 && errno == ENOENT));
  }

  rtems_bsd_rc_conf_argc_argv_destroy(aa);
  puts("test_service done");
  return 0;
}

static int
test_service_2(rtems_bsd_rc_conf* rc_conf)
{
  puts("test_service_2");
  assert(test_service_last_num == 1);
  test_service_last_num = 2;
  return 0;
}

static int
test_service_3(rtems_bsd_rc_conf* rc_conf)
{
  puts("test_service_3");
  assert(test_service_last_num == 2);
  test_service_last_num = 3;
  return 0;
}

static int
test_service_4(rtems_bsd_rc_conf* rc_conf)
{
  puts("test_service_4");
  assert(test_service_last_num == 3);
  test_service_last_num = 4;
  return 0;
}

static int
test_service_5(rtems_bsd_rc_conf* rc_conf)
{
  puts("test_service_5");
  assert(test_service_last_num == 4);
  test_service_last_num = 5;
  return 0;
}

static int
test_service_bad(rtems_bsd_rc_conf* rc_conf)
{
  puts("test_service_bad");
  return -1;
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
test_regex_reset(void)
{
  memset(&test_regex_results[0], 0, sizeof(test_regex_results));
  test_regex_last_num = 0;
  test_service_last_num = 0;
 }

static void
test_etc_rc_conf(void)
{
  puts("test_etc_rc_conf");
  make_rc_conf("/etc/rc.conf");
  test_regex_reset();
  assert(rtems_bsd_run_etc_rc_conf(0, true) == 0);
  test_regex_check();
}

static void
test_rc_conf(void)
{
  puts("test_rc_conf");
  make_rc_conf("/my_rc.conf");
  test_regex_reset();
  assert(rtems_bsd_run_rc_conf("/my_rc.conf", 0, true) == 0);
  test_regex_check();
}

static void
test_rc_conf_script(void)
{
  puts("test_rc_conf_conf");
  test_regex_reset();
  assert(rtems_bsd_run_rc_conf_script("internal", rc_conf_regex, 0, true) == 0);
  test_regex_check();
}

static void
test_rc_conf_script_not_found(void)
{
  puts("test_rc_conf_conf_not_found");
  test_regex_reset();
  assert(rtems_bsd_run_rc_conf_script("internal", rc_conf_not_found, 0, true) < 0);
  assert(test_regex_last_num == 0);
}

static void
setup(void)
{
  struct stat sb;
  mkdir("/etc", S_IRWXU | S_IRWXG | S_IRWXO); /* ignore errors, check the dir after. */
  assert(stat("/etc", &sb) == 0);
  assert(S_ISDIR(sb.st_mode));
  assert(rtems_bsd_rc_conf_service_add("test_service_2",
                                       "before:last;",
                                       test_service_2) == 0);
  assert(rtems_bsd_rc_conf_service_add("test_service_5",
                                       "after:test_service_2;",
                                       test_service_5) == 0);
  assert(rtems_bsd_rc_conf_service_add("test_service_4",
                                       "before:test_service_5;",
                                       test_service_4) == 0);
  assert(rtems_bsd_rc_conf_service_add("test_service_3",
                                       "before:test_service_4;after:test_service_2;",
                                       test_service_3) == 0);
  assert(rtems_bsd_rc_conf_service_add("test_service_bad",
                                       "before:first;",
                                       test_service_bad) < 0);
  assert(rtems_bsd_rc_conf_service_add("test_service_bad",
                                       "after:last;",
                                       test_service_bad) < 0);
  assert(rtems_bsd_rc_conf_service_add("test_service_bad",
                                       "after:xxxx,xxxx",
                                       test_service_bad) < 0);
  assert(rtems_bsd_rc_conf_service_add("test_service_bad",
                                       "yyyy:xxxx;",
                                       test_service_bad) < 0);
  assert(rtems_bsd_rc_conf_service_add("test_service",
                                       "after:first;",
                                       test_service) == 0);
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
