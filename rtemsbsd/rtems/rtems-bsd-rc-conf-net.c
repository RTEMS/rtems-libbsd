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
#include <sys/types.h>
#include <sys/queue.h>
#include <sys/kernel.h>
#include <sysexits.h>

#include <ifaddrs.h>

#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <rtems.h>
#include <rtems/chain.h>

#include <machine/rtems-bsd-commands.h>
#include <machine/rtems-bsd-rc-conf-services.h>

/*
 * Default defaultroute_delay is 30seconds.
 */
static int defaultroute_delay_secs = 30;

/*
 * Show a result.
 */
static void
show_result(const char* label, int r)
{
  if (r < 0)
    fprintf(stderr, "error: %s: %s\n", label, strerror(errno));
}

/*
 * cloned_interfaces
 *
 * eg cloned_interfaces="vlan0 bridge0 tap1 tap2"
 *
 * See 'man rc.conf(5)' on FreeBSD.
 */
static int
cloned_interfaces(rtems_bsd_rc_conf* rc_conf, rtems_bsd_rc_conf_argc_argv* aa)
{
  int arg;
  int r = 0;

  r = rtems_bsd_rc_conf_find(rc_conf, "cloned_interfaces", aa);
  if (r < 0) {
    if (errno == ENOENT)
      r = 0;
    return r;
  }

  for (arg = 1; arg < aa->argc; ++arg) {
    const char* ifconfg_args[] = {
      "ifconfig", aa->argv[arg], "create", NULL
    };
    rtems_bsd_rc_conf_print_cmd(rc_conf, "cloning_interfaces", 3, ifconfg_args);
    rtems_bsd_command_ifconfig(3, (char**) ifconfg_args);
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
load_create_args(rtems_bsd_rc_conf* rc_conf, rtems_bsd_rc_conf_argc_argv* aa)
{
  int r = 0;

  r = rtems_bsd_rc_conf_find(rc_conf, "create_args_.*", aa);

  while (r == 0) {
    rtems_chain_node* node;
    const char*       label;
    create_args_item* item;
    int               arg;

    rtems_bsd_rc_conf_print_cmd(rc_conf, "create_args_", aa->argc, aa->argv);

    label = aa->argv[0] + strlen("create_args_");

    node = rtems_chain_first(&create_args_items);

    while (!rtems_chain_is_tail(&create_args_items, node)) {
      item = (create_args_item*) node;
      if (strcasecmp(item->label, label) == 0) {
        fprintf(stderr, "error: %s:%d: duplicate create args entry: %s\n",
                rtems_bsd_rc_conf_name(rc_conf),
                rtems_bsd_rc_conf_line(rc_conf),
                aa->argv[0]);
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

    item->argc = aa->argc;

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

    item->argv = calloc(aa->argc + 1, sizeof(char*));
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

    for (arg = 0; arg < aa->argc; ++arg) {
      item->argv[arg] = strdup(aa->argv[0]);
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

    r = rtems_bsd_rc_conf_find_next(rc_conf, aa);
  }

  /*
   * ignore not found.
   */
  if (r < 0 && errno == ENOENT)
    r = 0;

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
          const char*        ifname,
          int                argc,
          const char**       argv)
{
  const char**      args;
  int               arg;
  int               r;
  const char const* ifconfig_show[] = { "ifconfig", ifname, NULL };

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
  args[1] = ifname;

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

  r = rtems_bsd_command_ifconfig(2, (char**) ifconfig_show);

  return r;
}

/*
 * hostname
 *
 * eg hostname="myhost"
 *
 * See 'man rc.conf(5)' on FreeBSD.
 */
static int
hostname(rtems_bsd_rc_conf* rc_conf, rtems_bsd_rc_conf_argc_argv* aa)
{
  const char**      argv;
  int               r;
  const char const* default_argv[] = { "hostname", "Amnesiac", NULL };

  r = rtems_bsd_rc_conf_find(rc_conf, "hostname", aa);
  if (r < 0 && errno != ENOENT)
    return -1;

  if (r < 0 || (r == 0 && aa->argc != 2)) {
    argv = default_argv;
  }
  else {
    argv = aa->argv;
  }

  fprintf(stdout, "Setting hostname: %s.\n", argv[1]);

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
defaultrouter(rtems_bsd_rc_conf* rc_conf, rtems_bsd_rc_conf_argc_argv* aa)
{
  int r;

  r = rtems_bsd_rc_conf_find(rc_conf, "defaultrouter", aa);
  if (r < 0 && errno != ENOENT)
    return -1;

  if (r == 0) {
    if (aa->argc > 2) {
      errno = EINVAL;
      return -1;
    }

    if (strcasecmp(aa->argv[1], "NO") != 0) {
      const char* args[] = { "route", "add", "default", aa->argv[1], NULL };
      int         r;

      rtems_bsd_rc_conf_print_cmd(rc_conf, "defaultrouter", 4, args);

      r = rtems_bsd_command_route(4, (char**) args);
      if (r != EX_OK) {
        errno = ECANCELED;
        return -1;
      }
    }
  }

  return 0;
}

/*
 * defaultroute_delay
 *
 * eg defaultroute=120
 *
 * See 'man rc.conf(5)' on FreeBSD.
 */
static int
defaultroute_delay(rtems_bsd_rc_conf* rc_conf,
                   int                argc,
                   const char**       argv)
{
  int   value;
  char* end = NULL;

  if (argc != 2) {
    errno = EINVAL;
    return -1;
  }

  value = strtol(argv[1], &end, 10);

  if (end == NULL) {
      const char* args[] = {
        "defaultrouter_delay", argv[1], NULL
    };

    rtems_bsd_rc_conf_print_cmd(rc_conf, "defaultrouter", 2, args);

    defaultroute_delay_secs = value;
  }
  else {
    errno = EINVAL;
    return -1;
  }

  return 0;
}

static int
show_interfaces(const char* msg, struct ifaddrs* ifap)
{
  struct ifaddrs* ifa;

  fprintf(stdout, msg);

  for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
    if (strcasecmp("lo0", ifa->ifa_name) == 0) {
      fprintf(stdout, "%s ", ifa->ifa_name);
      break;
    }
  }

  for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
    if (strcasecmp("lo0", ifa->ifa_name) != 0) {
      fprintf(stdout, "%s ", ifa->ifa_name);
    }
  }

  fprintf(stdout, "\b.\n");

  return 0;
}

static int
setup_lo0(rtems_bsd_rc_conf* rc_conf, struct ifaddrs* ifap)
{
  struct ifaddrs* ifa;
  for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
    if (strcasecmp("lo0", ifa->ifa_name) == 0) {
      const char* lo0_argv[] = {
        "ifconfig_lo0", "inet", "127.0.0.1", "netmask", "255.0.0.0", NULL
      };
      show_result("lo0", ifconfig_(rc_conf, "lo0", 5, lo0_argv));
      return 0;
    }
  }
  fprintf(stderr, "warning: no loopback interface found\n");
  return -1;
}

static int
setup_interfaces(rtems_bsd_rc_conf*           rc_conf,
                 rtems_bsd_rc_conf_argc_argv* aa,
                 struct ifaddrs*              ifap)
{
  struct ifaddrs* ifa;
  for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
    if (strcasecmp("lo0", ifa->ifa_name) != 0) {
      char iface[64];
      int  r;
      snprintf(iface, sizeof(iface), "ifconfig_%s", ifa->ifa_name);
      r = rtems_bsd_rc_conf_find(rc_conf, iface, aa);
      if (r == 0) {
        show_result(iface, ifconfig_(rc_conf, ifa->ifa_name, aa->argc, aa->argv));
      }
    }
  }
  return 0;
}

static int
setup_vlans(rtems_bsd_rc_conf*           rc_conf,
            rtems_bsd_rc_conf_argc_argv* aa,
            struct ifaddrs*              ifap)
{
  rtems_bsd_rc_conf_argc_argv* vaa;
  struct ifaddrs*              ifa;

  vaa = rtems_bsd_rc_conf_argc_argv_create();
  if (vaa == NULL)
    return -1;

  show_result("create_args", load_create_args(rc_conf, aa));

  for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
    if (strcasecmp("lo0", ifa->ifa_name) != 0) {
      char expr[128];
      int  r;
      /*
       * Look for vlans_'iface'="101 102 103"
       */
      snprintf(expr, sizeof(expr), "vlans_%s", ifa->ifa_name);
      r = rtems_bsd_rc_conf_find(rc_conf, expr, aa);
      if (r == 0) {
        int arg;
        for (arg = 1; arg < aa->argc; ++arg) {
          char vlan_name[64];
          const char* vlan_create[] = {
            "ifconfig", vlan_name, "create", NULL
          };
          /*
           * Create the VLAN name as 'iface'.'vlan'.
           */
          snprintf(vlan_name, sizeof(vlan_name),
                   "%s.%s", ifa->ifa_name, aa->argv[arg]);
          rtems_bsd_rc_conf_print_cmd(rc_conf, "vlan", 3, vlan_create);
          r = rtems_bsd_command_ifconfig(3, (char**) vlan_create);
          if (r == 0) {
            /*
             * Look for ifconfig_'iface'_'vlan'="..."
             */
            snprintf(expr, sizeof(expr),
                     "ifconfig_%s_%s", ifa->ifa_name, aa->argv[arg]);
            r = rtems_bsd_rc_conf_find(rc_conf, expr, vaa);
            if (r == 0) {
              show_result(vlan_name, ifconfig_(rc_conf, vlan_name,
                                               vaa->argc, vaa->argv));
            }
          }
        }
      }
    }
  }

  rtems_bsd_rc_conf_argc_argv_destroy(vaa);

  return 0;
}

static int
interfaces(rtems_bsd_rc_conf* rc_conf, rtems_bsd_rc_conf_argc_argv* aa)
{
  struct ifaddrs* ifap;

  if (getifaddrs(&ifap) != 0) {
    fprintf(stderr, "error: interfaces: getifaddrs: %s\n", strerror(errno));
    return -1;
  }

  show_interfaces("Starting network: ", ifap);
  show_result("cloned_interfaces", cloned_interfaces(rc_conf, aa));
  show_result("lo0", setup_lo0(rc_conf, ifap));
  show_result("ifaces", setup_interfaces(rc_conf, aa, ifap));
  show_result("vlans", setup_vlans(rc_conf, aa, ifap));

  free(ifap);

  return 0;
}

static int
network_service(rtems_bsd_rc_conf* rc_conf)
{
  rtems_bsd_rc_conf_argc_argv* aa;
  int                          r;

  aa = rtems_bsd_rc_conf_argc_argv_create();
  if (aa == NULL)
    return -1;

  show_result("hostname",    hostname(rc_conf, aa));

  r = interfaces(rc_conf, aa);
  if (r < 0) {
    rtems_bsd_rc_conf_argc_argv_destroy(aa);
    return -1;
  }

  show_result("defaultrouter", defaultrouter(rc_conf, aa));

  rtems_bsd_rc_conf_argc_argv_destroy(aa);

  return 0;
}

void
rc_conf_net_init(void* arg)
{
  int r;
  r = rtems_bsd_rc_conf_service_add("network",
                                    "after:first;",
                                    network_service);
  if (r < 0)
    fprintf(stderr, "error: network service add failed: %s\n", strerror(errno));
}
