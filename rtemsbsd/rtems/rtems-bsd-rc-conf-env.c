/*
 * Copyright (c) 2026 Chris Johns <chrisj@rtems.org>.  All rights reserved.
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
 * Convert a standard set of enviroment variables to an rc.conf
 * file.
 */


#include <ctype.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <rtems.h>
#include <rtems/bsd/bsd.h>

#include <machine/rtems-bsd-rc-conf-env.h>

static bool append_line(char* buf, const size_t buf_len, const char* line) {
  const size_t level = strlen(buf);
  const size_t line_len = strlen(line) + 1;
  if (line_len >= (buf_len - level)) {
    printf("error: rc.conf: from env: too big\n");
    return false;
  }
  strncat(buf, line, buf_len);
  strncat(buf, "\n", buf_len);
  return true;
}

int rtems_bsd_rc_conf_from_env(
  bool auto_dhcp, int dhcp_prio, int dhcp_timeout, bool verbose) {
  char buf[1024];
  int r = rtems_bsd_rc_conf_from_env_buf(
    buf, sizeof(buf), auto_dhcp, dhcp_prio, dhcp_timeout, verbose);
  if (r > 0) {
    FILE* rc_conf = fopen("/etc/rc.conf", "w");
    if (rc_conf == NULL) {
      printf(
        "error rc.conf: from env: cannot open /etc/rc.conf: %d: %s\n",
        errno, strerror(errno));
      r = -1;
    } else {
      size_t written = fwrite(buf, 1, r, rc_conf);
      if (written != (size_t) r) {
        if (ferror(rc_conf)) {
          printf(
            "error rc.conf: from env: write to /etc/rc.conf failed: %d: %s\n",
            errno, strerror(errno));
        } else {
          printf("error rc.conf: from env: write to /etc/rc.conf failed\n");
        }
        r = -1;
      }
      fclose(rc_conf);
    }
  }
  return r;
}

int rtems_bsd_rc_conf_from_env_buf(char* out, size_t out_len, bool auto_dhcp, int dhcp_prio,
                                   int dhcp_timeout, bool verbose) {
  const char* env;
  char buf[128];
  const size_t buf_len = sizeof(buf);
  bool dhcp = false;
  int unit;
  memset(&out[0], 0, out_len);
  env = getenv("RTEMS_NET_HOSTNAME");
  if (env != NULL) {
    snprintf(buf, buf_len, "hostname=\"%s\"", env);
    append_line(out, out_len, buf);
  }
  for (unit = 1; unit <= RTEMS_BSD_RC_CONF_ENV_MAX_IFACES; ++unit) {
    /*
     * We need the interface name to be able to process an variables.
     */
    snprintf(buf, buf_len, "RTEMS_NET_IFACE_%i", unit);
    env = getenv(buf);
    if (env != NULL && strlen(env) > 0) {
      const char* name = env;
      const char* if_opts;
      snprintf(buf, buf_len, "RTEMS_NET_IF_%i_OPTS", unit);
      if_opts = getenv(buf);
      snprintf(buf, buf_len, "RTEMS_NET_IF_%i_IP_ADDR", unit);
      env = getenv(buf);
      if (env != NULL) {
        /* ifconfig_* as inet and netmask */
        const char* ip = env;
        snprintf(buf, buf_len, "RTEMS_NET_IF_%i_NETMASK", unit);
        env = getenv(buf);
        if (env == NULL) {
          printf("error: rc.conf: from env: %i: %s: no netmask for static ip\n",
                 unit, name);
          continue;
        }
        if (if_opts != NULL) {
          snprintf(buf, buf_len,
                   "ifconfig_%s=\"inet %s netmask %s %s\"",
                   name, ip, env, if_opts);
        } else {
          snprintf(buf, buf_len,
                   "ifconfig_%s=\"inet %s netmask %s\"", name, ip, env);
        }
      } else if (auto_dhcp) {
        /* ifconfig_* as DHCP */
        dhcp = true;
        if (if_opts != NULL) {
          snprintf(buf, buf_len, "ifconfig_%s=\"DHCP %s\"", name, if_opts);
        } else {
          snprintf(buf, buf_len, "ifconfig_%s=\"DHCP\"", name);
        }
      }
      append_line(out, out_len, buf);
      /* ifconfig_*_alias0 to set a MAC address */
      snprintf(buf, buf_len, "RTEMS_NET_IF_%i_MAC", unit);
      env = getenv(buf);
      if (env != NULL) {
        snprintf(buf, buf_len, "ifconfig_%s_alias0=\"ether %s\"", name, env);
        append_line(out, out_len, buf);
      }
    }
  }
  if (dhcp) {
    snprintf(buf, buf_len, "dhcpcd_priority=\"%i\"", dhcp_prio);
    append_line(out, out_len, buf);
    snprintf(buf, buf_len,
             "dhcpcd_options=\"--debug --nobackground --timeout %i\"",
             dhcp_timeout);
    append_line(out, out_len, buf);
  }
  env = getenv("RTEMS_NET_GATEWAY_IP");
  if (env != NULL) {
    snprintf(buf, buf_len, "defaultrouter=\"%s\"", env);
    append_line(out, out_len, buf);
  }
  int len = strlen(out);
  if (verbose) {
    printf("rc-conf: env: len = %i\n---\n", len);
    printf(out);
    printf("---\n");
  }
  return len;
}

int rtems_bsd_resolv_conf_from_env(bool verbose) {
  char buf[1024];
  int r = rtems_bsd_resolv_conf_from_env_buf(buf, sizeof(buf), verbose);
  if (r > 0) {
    FILE* resolv_conf = fopen("/etc/resolv.conf", "w");
    if (resolv_conf == NULL) {
      printf(
        "error rc.conf: from env: cannot open /etc/resolv.conf: %d: %s\n",
        errno, strerror(errno));
      r = -1;
    } else {
      size_t written = fwrite(buf, 1, r, resolv_conf);
      if (written != (size_t) r) {
        if (ferror(resolv_conf)) {
          printf(
            "error rc.conf: from env: write to /etc/resolv.conf failed: %d: %s\n",
            errno, strerror(errno));
        } else {
          printf("error rc.conf: from env: write to /etc/resolv.conf failed\n");
        }
        r = -1;
      }
      fclose(resolv_conf);
    }
  }
  return r;
}

int rtems_bsd_resolv_conf_from_env_buf(char* out, size_t out_len, bool verbose) {
  const char* env;
  char buf[128];
  const size_t buf_len = sizeof(buf);
  memset(&out[0], 0, out_len);
  env = getenv("RTEMS_NET_DOMAIN_NAME");
  if (env != NULL) {
    snprintf(buf, buf_len, "domain %s", env);
    append_line(out, out_len, buf);
  }
  env = getenv("RTEMS_NET_DNS_IP");
  if (env != NULL) {
    snprintf(buf, buf_len, "nameserver %s", env);
    append_line(out, out_len, buf);
  }
  int len = strlen(out);
  if (verbose) {
    printf("resolv-conf: env: len = %i\n---\n", len);
    printf(out);
    printf("---\n");
  }
  return len;
}
