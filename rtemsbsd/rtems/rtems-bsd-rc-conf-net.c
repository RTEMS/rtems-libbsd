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

#define _GNU_SOURCE

#include <sys/param.h>
#include <sys/types.h>
#include <sys/queue.h>
#include <sys/kernel.h>
#include <sysexits.h>

#include <ifaddrs.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <net/route.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>

#include <rtems.h>
#include <rtems/chain.h>

#include <machine/rtems-bsd-commands.h>
#include <machine/rtems-bsd-rc-conf-services.h>

#include <rtems/rtems-routes.h>

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
 * ifconfig_show

 */
static int
ifconfig_show(const char* ifname)
{
  const char const* ifconfig_show[] = { "ifconfig", ifname, NULL };
  return rtems_bsd_command_ifconfig(2, (char**) ifconfig_show);
}

/*
 * ifconfig_'interface'
 *
 * eg ifconfig_em0="inet 10.10.5.33 netmask 255.255.255.0"
 *    ifconfig_em0_alias0="ether 10:22:33:44:55:66"
 *    ifconfig_em0_alias1="inet 10.1.1.111 netmask 0xffffffff"
 *
 * See 'man rc.conf(5)' on FreeBSD.
 */
static int
ifconfig_(rtems_bsd_rc_conf* rc_conf,
          const char*        ifname,
          int                argc,
          const char**       argv,
          int                opt_argc,
          const char**       opt_argv,
          bool               add_up)
{
  const char**      args;
  int               arg;
  int               ifconfig_argc = 0;
  int               r;

  for (arg = 1; arg < argc; ++arg) {
    if (strcasecmp(argv[arg], "NOAUTO") == 0)
      return 0;
  }

  args = calloc(argc + opt_argc + 3, sizeof(char*));
  if (args == NULL) {
    errno = ENOMEM;
    return -1;
  }

  args[ifconfig_argc++] = "ifconfig";
  args[ifconfig_argc++] = ifname;

  for (arg = 1; arg < argc; ++arg) {
    if (strcasecmp("DHCP",     argv[arg]) == 0 ||
        strcasecmp("SYNCDHCP", argv[arg]) == 0 ||
        strcasecmp("UP",       argv[arg]) == 0) {
      add_up = false;
    }
    else {
      args[ifconfig_argc++] = argv[arg];
    }
  }

  if (opt_argv != NULL) {
    for (arg = 0; arg < opt_argc; ++arg) {
      args[ifconfig_argc++] = opt_argv[arg];
    }
  }

  if (add_up)
    args[ifconfig_argc++] = "up";

  rtems_bsd_rc_conf_print_cmd(rc_conf, "ifconfig", ifconfig_argc, args);

  r = rtems_bsd_command_ifconfig(ifconfig_argc, (char**) args);

  free(args);

  if (r != EX_OK) {
    errno = ECANCELED;
    return -1;
  }

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
defaultrouter(rtems_bsd_rc_conf* rc_conf, rtems_bsd_rc_conf_argc_argv* aa, bool dhcp)
{
  int r;

  if (dhcp) {
    char* end = NULL;
    int   delay = 30;

    /*
     * See if a delay is specified else use default to 30 seconds. Wait for a
     * valid default route.
     */
    r = rtems_bsd_rc_conf_find(rc_conf, "defaultroute_delay", aa);
    if (r == 0 && aa->argc == 2) {
      delay = (int) strtol(aa->argv[1], &end, 10);
      if (*end != '\0') {
        fprintf(stderr, "error: defaultroute_delay: invalid delay value\n");
        delay = 30;
      }
    }

    printf("Waiting %ds for default route interface: ", delay);
    fflush(stdout);

    while (delay > 0) {
      struct sockaddr_in sin;
      struct sockaddr*   rti_info[RTAX_MAX];

      --delay;

      memset(&sin, 0, sizeof(sin));
      memset(&rti_info[0], 0, sizeof(rti_info));
      sin.sin_family = AF_INET;
      inet_pton(AF_INET, "0.0.0.0", &sin.sin_addr);

      r = rtems_get_route(&sin, rti_info);
      if (r == 0 && rti_info[RTAX_GATEWAY] != NULL) {
        break;
      }
      else if (r < 0 && errno != ESRCH) {
        fprintf(stderr,
                "error: get routes %d: %d %s\n", r, errno, strerror(errno));
      }

      sleep(1);
    }

    /*
     * We should print the interface but I cannot see how to get the interface
     * with the default route without a lot of code.
     */
    if (delay > 0) {
      printf("found.\n");
      return 0;
    }

    printf("\nerror: no default route found, try defaultrouter\n");
  }

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

static int
list_interfaces(const char* msg, struct ifaddrs* ifap)
{
  struct ifaddrs* ifa;

  fprintf(stdout, msg);

  /*
   * Always have lo0 first.
   */

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
show_interfaces(struct ifaddrs* ifap)
{
  struct ifaddrs* ifa;

  for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
    ifconfig_show(ifa->ifa_name);
  }

  return 0;
}

static int
dhcp_check(rtems_bsd_rc_conf_argc_argv* aa)
{
  int arg;
  for (arg = 0; arg < aa->argc; ++arg) {
    if (strcasestr(aa->argv[1], "DHCP") != NULL ||
        strcasestr(aa->argv[1], "SYNCDHCP") != NULL)
      return true;
  }
  return false;
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
      show_result("lo0",
                  ifconfig_(rc_conf, "lo0",
                            5, lo0_argv,
                            0, NULL,
                            true));
      return 0;
    }
  }

  fprintf(stderr, "warning: no loopback interface found\n");

  return -1;
}

static int
setup_interfaces(rtems_bsd_rc_conf*           rc_conf,
                 rtems_bsd_rc_conf_argc_argv* aa,
                 struct ifaddrs*              ifap,
                 bool*                        dhcp)
{
  struct ifaddrs* ifa;
  int             r;

  for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
    if (strcasecmp("lo0", ifa->ifa_name) != 0) {
      char iface[64];
      snprintf(iface, sizeof(iface), "ifconfig_%s", ifa->ifa_name);
      r = rtems_bsd_rc_conf_find(rc_conf, iface, aa);
      if (r == 0) {
        if (dhcp_check(aa)) {
          *dhcp = true;
        }
        /*
         * A DHCP ifconfig can have other options we need to set on the
         * interface.
         */
        show_result(iface, ifconfig_(rc_conf, ifa->ifa_name,
                                     aa->argc, aa->argv,
                                     0, NULL,
                                     true));
      }
      snprintf(iface, sizeof(iface), "ifconfig_%s_alias[0-9]+", ifa->ifa_name);
      if (r == 0) {
        r = rtems_bsd_rc_conf_find(rc_conf, iface, aa);
        while (r == 0) {
          const char* alias_argv[] = { "alias", NULL };
          show_result(iface,
                      ifconfig_(rc_conf, ifa->ifa_name,
                                aa->argc, aa->argv,
                                1, alias_argv,
                                false));
          r = rtems_bsd_rc_conf_find_next(rc_conf, aa);
        }
      }
    }
  }

  return 0;
}

static int
setup_vlans(rtems_bsd_rc_conf*           rc_conf,
            rtems_bsd_rc_conf_argc_argv* aa,
            struct ifaddrs*              ifap,
            bool*                        dhcp)
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
              if (dhcp_check(vaa)) {
                *dhcp = true;
              }
              else {
                show_result(vlan_name,
                            ifconfig_(rc_conf, vlan_name,
                                      vaa->argc, vaa->argv,
                                      0, NULL,
                                      true));
              }
            }
          }
        }
      }
    }
  }

  rtems_bsd_rc_conf_argc_argv_destroy(vaa);

  return 0;
}

/*
 * The rc_conf struct cannot be passed to a thread as a pointer. It can only be
 * used in the rc.conf worker thread. As a result the values needed to print a
 * verbose message to aid debugging need to have local copies made and passed
 * to the dhcpcd worker. The dhcpcd worker should run for ever. Clean up the
 * memory if it exits.
 */
typedef struct dhcpcd_data {
  rtems_bsd_rc_conf_argc_argv* argc_argv;
  bool                         verbose;
  const char*                  name;
} dhcpcd_data;

static void
dhcpcd_worker(rtems_task_argument arg)
{
  dhcpcd_data*  dd = (dhcpcd_data*) arg;
  int           argc;
  const char**  argv;
  const char*   dhcpcd_argv[] = { "dhcpcd", NULL };
  struct stat   sb;
  int           r;

  r = stat("/var", &sb);
  if (r < 0) {
    mkdir("/var", S_IRWXU | S_IRWXG | S_IRWXO);
  }

  r = stat("/var/db", &sb);
  if (r < 0) {
    mkdir("/var/db", S_IRWXU | S_IRWXG | S_IRWXO);
  }

  if (dd->argc_argv->argc > 0) {
    argc = dd->argc_argv->argc;
    argv = dd->argc_argv->argv;
  }
  else {
    argc = 1;
    argv = dhcpcd_argv;
  }

  if (dd->verbose) {
    fprintf(stdout, "rc.conf: %s: dhcpcd ", dd->name);
    for (r = 1; r < argc; ++r)
      fprintf(stdout, "%s ", argv[r]);
    fprintf(stdout, "\n");
  }

  r = rtems_bsd_command_dhcpcd(argc, argv);
  if (r != EX_OK)
    fprintf(stderr, "error: dhcpcd: stopped\n");

  free(dd->name);
  rtems_bsd_rc_conf_argc_argv_destroy(dd->argc_argv);
  free(dd);

  rtems_task_delete(RTEMS_SELF);
}

static int
run_dhcp(rtems_bsd_rc_conf* rc_conf, rtems_bsd_rc_conf_argc_argv* aa)
{
  dhcpcd_data*        dd;
  rtems_status_code   sc;
  rtems_id            id;
  rtems_task_priority priority = RTEMS_MAXIMUM_PRIORITY - 1;
  char*               end = NULL;
  int                 r;

  /*
   * These are passed to the worker and cleaned up there if it ever exits. Do
   * not destroy here unless an error before the thread runs.
   */
  dd = calloc(1, sizeof(*dd));
  if (dd == NULL) {
    fprintf(stderr, "error: dhcpcd data: no memory\n");
    errno = ENOMEM;
    return -1;
  }

  dd->name = strdup(rtems_bsd_rc_conf_name(rc_conf));
  if (dd == NULL) {
    free(dd);
    fprintf(stderr, "error: dhcpcd data: no memory\n");
    errno = ENOMEM;
    return -1;
  }

  dd->argc_argv = rtems_bsd_rc_conf_argc_argv_create();
  if (dd->argc_argv == NULL) {
    free(dd->name);
    free(dd);
    errno = ENOMEM;
    return -1;
  }

  dd->verbose = rtems_bsd_rc_conf_verbose(rc_conf);

  r = rtems_bsd_rc_conf_find(rc_conf, "dhcpcd_priority", dd->argc_argv);
  if (r == 0) {
    if (dd->argc_argv->argc == 2) {
      priority = strtoul(dd->argc_argv->argv[1], &end, 10);
      if (priority == 0 || *end != '\0')
        priority = RTEMS_MAXIMUM_PRIORITY - 1;
    }
  }

  rtems_bsd_rc_conf_find(rc_conf, "dhcpcd_options", dd->argc_argv);

  sc = rtems_task_create(rtems_build_name('D', 'H', 'C', 'P'),
                         priority,
                         2 * RTEMS_MINIMUM_STACK_SIZE,
                         RTEMS_DEFAULT_MODES,
                         RTEMS_FLOATING_POINT,
                         &id);
  if (sc == RTEMS_SUCCESSFUL)
    sc = rtems_task_start(id, dhcpcd_worker, (rtems_task_argument) dd);
  if (sc != RTEMS_SUCCESSFUL) {
    fprintf(stderr,
            "error: dhcpcd: thread create/start: %s\n", rtems_status_text(sc));
    rtems_bsd_rc_conf_argc_argv_destroy(dd->argc_argv);
    free(dd->name);
    free(dd);
    errno = EIO;
    return -1;
  }

  /*
   * Let it run before moving on.
   */
  sleep(1);

  return 0;
}

static int
interfaces(rtems_bsd_rc_conf* rc_conf, rtems_bsd_rc_conf_argc_argv* aa)
{
  struct ifaddrs* ifap;
  bool            dhcp = false;

  if (getifaddrs(&ifap) != 0) {
    fprintf(stderr, "error: interfaces: getifaddrs: %s\n", strerror(errno));
    return -1;
  }

  list_interfaces("Starting network: ", ifap);
  show_result("cloned_interfaces", cloned_interfaces(rc_conf, aa));
  show_result("lo0", setup_lo0(rc_conf, ifap));
  show_result("ifaces", setup_interfaces(rc_conf, aa, ifap, &dhcp));
  show_result("vlans", setup_vlans(rc_conf, aa, ifap, &dhcp));
  show_interfaces(ifap);

  if (dhcp)
    show_result("dhcp", run_dhcp(rc_conf, aa));
  show_result("defaultrouter", defaultrouter(rc_conf, aa, dhcp));

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

  show_result("hostname", hostname(rc_conf, aa));

  r = interfaces(rc_conf, aa);
  if (r < 0) {
    rtems_bsd_rc_conf_argc_argv_destroy(aa);
    return -1;
  }

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
    fprintf(stderr,
            "error: network service add failed: %s\n", strerror(errno));
}
