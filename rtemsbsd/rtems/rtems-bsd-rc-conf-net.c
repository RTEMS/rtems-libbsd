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
 * Handle the networking directives found in rc.conf.
 *  - ifconfig_*
 *  - cloned_interfaces
 *  - autobridge_interfaces
 *  - autobridge_bridge*
 *  - defaultrouter
 */

#include <rtems/bsd/sys/param.h>
#include <rtems/bsd/sys/types.h>

#include <sys/queue.h>
#include <sys/kernel.h>
#include <sysexits.h>

#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <rtems.h>
#include <rtems/chain.h>

#include <machine/rtems-bsd-commands.h>
#include <machine/rtems-bsd-rc-conf.h>

/*
 * cloned_interfaces
 *
 * eg cloned_interfaces="vlan0 bridge0 tap1 tap2"
 *
 * See 'man rc.conf(5)' on FreeBSD.
 */
static int
cloned_interfaces(rtems_bsd_rc_conf* rc_conf,
                  int                argc,
                  const char**       argv)
{
  int arg;
  for (arg = 1; arg < argc; ++arg) {
    const char* ifconfg_args[] = {
      "ifconfig", argv[arg], "create", NULL
    };
    int r;
    rtems_bsd_rc_conf_print_cmd(rc_conf, "cloning_interfaces", 3, ifconfg_args);
    r = rtems_bsd_command_ifconfig(3, (char**) ifconfg_args);
    if (r != EX_OK) {
      errno = ECANCELED;
      return -1;
    }
  }
  return 0;
}

/*
 * create_args_'interface'
 *
 * eg create_args_myvlan="vlan 102"
 *
 * See 'man rc.conf(5)' on FreeBSD.
 */
typedef struct {
  rtems_chain_node node;
  const char*      label;
  int              argc;
  const char**     argv;
} create_args_item;

static RTEMS_CHAIN_DEFINE_EMPTY(create_args_items);

static int
create_args_(rtems_bsd_rc_conf* rc_conf,
             int                argc,
             const char**       argv)
{
  rtems_chain_node*       node = rtems_chain_first(&create_args_items);
  const rtems_chain_node* tail = rtems_chain_tail(&create_args_items);
  const char*             label = argv[0] + strlen("create_args_");
  create_args_item*       item;
  int                     arg;

  while (node != tail) {
    item = (create_args_item*) node;
    if (strcasecmp(item->label, label) == 0) {
      fprintf(stderr, "error: %s:%d: duplicate create args entry: %s\n",
              rtems_bsd_rc_conf_name(rc_conf),
              rtems_bsd_rc_conf_line(rc_conf),
              argv[0]);
      errno = EEXIST;
      return -1;
    }
    node = rtems_chain_next(node);
  }

  item = calloc(1, sizeof(*item));
  if (item == NULL) {
    errno = ENOMEM;
    fprintf(stderr, "error: %s:%d: %s\n",
            rtems_bsd_rc_conf_name(rc_conf),
            rtems_bsd_rc_conf_line(rc_conf),
            strerror(errno));
    return -1;
  }

  item->argc = argc;

  item->label = strdup(label);
  if (item->label == NULL) {
    free(item);
    errno = ENOMEM;
    fprintf(stderr, "error: %s:%d: %s\n",
            rtems_bsd_rc_conf_name(rc_conf),
            rtems_bsd_rc_conf_line(rc_conf),
            strerror(errno));
    return -1;
  }

  item->argv = calloc(argc + 1, sizeof(char*));
  if (item->argv == NULL) {
    free((void*) item->label);
    free(item);
    errno = ENOMEM;
    fprintf(stderr, "error: %s:%d: %s\n",
            rtems_bsd_rc_conf_name(rc_conf),
            rtems_bsd_rc_conf_line(rc_conf),
            strerror(errno));
    return -1;
  }

  for (arg = 0; arg < argc; ++arg) {
    item->argv[arg] = strdup(argv[0]);
    if (item->argv[arg] == NULL) {
      int a;
      for (a = 0; a < arg; ++a)
        free((void*) item->argv[a]);
      free(item->argv);
      free((void*) item->label);
      free(item);
      errno = ENOMEM;
      fprintf(stderr, "error: %s:%d: %s\n",
              rtems_bsd_rc_conf_name(rc_conf),
              rtems_bsd_rc_conf_line(rc_conf),
              strerror(errno));
      return -1;
    }
  }

  rtems_chain_append(&create_args_items, &item->node);

  return 0;
}

/*
 * ifconfig_'interface'
 *
 * eg ifconfig_em0="inet 10.10.5.33 netmask 255.255.255.0"
 *
 * See 'man rc.conf(5)' on FreeBSD.
 */
static int
ifconfig_(rtems_bsd_rc_conf* rc_conf,
          int                argc,
          const char**       argv)
{
  const char** args;
  int          arg;
  int          r;

  for (arg = 1; arg < argc; ++arg) {
    if (strcasecmp(argv[arg], "NOAUTO") == 0)
      return 0;
  }

  args = calloc(argc + 3, sizeof(char*));
  if (args == NULL) {
    errno = ENOMEM;
    return -1;
  }

  args[0] = "ifconfig";
  args[1] = argv[0] + strlen("ifconfig_");

  for (arg = 1; arg < argc; ++arg)
    args[arg + 1] = argv[arg];

  args[argc + 1] = "up";

  rtems_bsd_rc_conf_print_cmd(rc_conf, "ifconfig", argc + 2, args);

  r = rtems_bsd_command_ifconfig(argc + 2, (char**) args);

  free(args);

  if (r != EX_OK) {
    errno = ECANCELED;
    return -1;
  }

  return 0;
}

/*
 * hostname
 *
 * eg hostname="myhost"
 *
 * See 'man rc.conf(5)' on FreeBSD.
 */
static int
hostname(rtems_bsd_rc_conf* rc_conf,
         int                argc,
         const char**       argv)
{
  if (argc > 2) {
    errno = EINVAL;
    return -1;
  }

  rtems_bsd_rc_conf_print_cmd(rc_conf, "hostname", argc, argv);

  return sethostname(argv[1], strlen(argv[1]));
}

/*
 * defaultrouter
 *
 * eg defaultrouter="1.2.3.4"
 *
 * See 'man rc.conf(5)' on FreeBSD.
 */
static int
defaultrouter(rtems_bsd_rc_conf* rc_conf,
              int                argc,
              const char**       argv)
{
  if (argc > 2) {
    errno = EINVAL;
    return -1;
  }

  if (strcasecmp(argv[1], "NO") != 0) {
    const char* args[] = {
      "route", "add", "default", argv[1], NULL
    };
    int r;

    rtems_bsd_rc_conf_print_cmd(rc_conf, "defaultrouter", 4, args);

    r = rtems_bsd_command_route(4, (char**) args);
    if (r != EX_OK) {
      errno = ECANCELED;
      return -1;
    }
  }

  return 0;
}

static void
add_directive(const char* name, rtems_bsd_rc_conf_directive handler)
{
  int r;
  r = rtems_bsd_rc_conf_directive_add(name, handler);
  if (r < 0)
    fprintf(stderr, "error: cannot register rc.conf handler: %s\n", name);
}

void
rc_conf_net_init(void* arg)
{
  add_directive("cloned_interfaces", cloned_interfaces);
  add_directive("create_args_.*", create_args_);
  add_directive("ifconfig_*", ifconfig_);
  add_directive("hostname", hostname);
  add_directive("defaultrouter", defaultrouter);
}
