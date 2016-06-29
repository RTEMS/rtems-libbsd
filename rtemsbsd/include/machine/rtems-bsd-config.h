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
 * Configure the LibBSD software stack.
 *
 * Configuration defines:
 *
 *  RTEMS_BSD_CONFIG_NET_PF_UNIX     : Packet Filter.
 *  RTEMS_BSD_CONFIG_NET_IF_LAGG     : Link Aggregetion and Failover.
 *  RTEMS_BSD_CONFIG_NET_IF_VLAN     : Virtual LAN.
 *  RTEMS_BSD_CONFIG_SERVICE_FTPD    : File Transfer Protocol (FTP).
 *  RTEMS_BSD_CONFIG_BSP_CONFIG      : Configure default BSP devices.
 *  RTEMS_BSD_CONFIG_INIT            : Configure the LibBSD support.
 *
 * Rules for adding to this file:
 *  1. All user visible defines start with 'RTEMS_BSD_CONFIG_'.
 *  2. Define an implementation define starting with 'RTEMS_BSD_CFGDECL_'.
 *  3. Only declare anything in the 'RTEMS_BSD_CONFIG_INIT' section.
 *  4. Do not use '#ifdef ..' or '#ifndef ..'.
 *  5. All '#endif' statements need a comment showing the '#if ..' logic.
 */

#ifndef _RTEMS_BSD_CONFIG_h
#define _RTEMS_BSD_CONFIG_h

#include <stdbool.h>

/*
 * Include the Nexus bus.
 */
#include <machine/rtems-bsd-nexus-bus.h>

/*
 * Include the SYSINIT support.
 */
#include <machine/rtems-bsd-sysinit.h>

/*
 * Include the services.
 */
#include <machine/rtems-bsd-rc-conf-services.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * BSD Kernel modules.
 */

/*
 * Unix packet filter.
 *  https://www.freebsd.org/doc/handbook/firewalls-pf.html
 */
#if defined(RTEMS_BSD_CONFIG_NET_PF_UNIX)
  #define RTEMS_BSD_CFGDECL_NET_PF_UNIX SYSINIT_NEED_NET_PF_UNIX
#endif /* RTEMS_BSD_CONFIG_NET_PF_UNIX */

/*
 * Link Aggregation and Failover.
 *  https://www.freebsd.org/doc/handbook/network-aggregation.html
 */
#if defined(RTEMS_BSD_CONFIG_NET_IF_LAGG)
  #define RTEMS_BSD_CFGDECL_IF_LAGG SYSINIT_NEED_NET_IF_LAGG
#endif /* RTEMS_BSD_CONFIG_NET_IF_LAGG */

/*
 * Virtual LAN
 *  https://www.freebsd.org/cgi/man.cgi?query=rc.conf then 'network_interfaces'.
 */
#if defined(RTEMS_BSD_CONFIG_NET_IF_VLAN)
  #define RTEMS_BSD_CFGDECL_NET_IF_VLAN SYSINIT_NEED_NET_IF_VLAN
#endif /* RTEMS_BSD_CONFIG_NET_IF_VLAN */

/*
 * FTPD
 */
#if defined(RTEMS_BSD_CONFIG_SERVICE_FTPD)
  #define RTEMS_BSD_CFGDECL_FTPD RTEMS_BSD_RC_CONF_SYSINT(rc_conf_ftpd)
#endif /* RTEMS_BSD_CONFIG_SERVICE_FTPD */

/*
 * Configure the system.
 */
#if defined(RTEMS_BSD_CONFIG_INIT)
 /*
  * If a BSP configuration is requested include the Nexus bus BSP configuration.
  */
 #if defined(RTEMS_BSD_CONFIG_BSP_CONFIG)
  #include <bsp/nexus-devices.h>
 #endif

 /*
  * Create the networking modules and interfaces.
  */
 RTEMS_BSD_CFGDECL_NET_PF_UNIX;
 RTEMS_BSD_CFGDECL_IF_LAGG;
 RTEMS_BSD_CFGDECL_NET_IF_VLAN;

 /*
  * Create the services.
  */
 RTEMS_BSD_CFGDECL_FTPD;
#endif /* RTEMS_BSD_CONFIG_INIT */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_BSD_CONFIG_h */
