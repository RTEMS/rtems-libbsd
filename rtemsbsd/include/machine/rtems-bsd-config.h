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
 *  RTEMS_BSD_CONFIG_DOMAIN_PAGE_MBUFS_SIZE : Memory in bytes for mbufs
 *  RTEMS_BSD_CONFIG_IPSEC                  : IPSec support.
 *  RTEMS_BSD_CONFIG_NET_PF_UNIX            : Packet Filter.
 *  RTEMS_BSD_CONFIG_NET_IF_LAGG            : Link Aggregetion and Failover.
 *  RTEMS_BSD_CONFIG_NET_IF_VLAN            : Virtual LAN.
 *  RTEMS_BSD_CONFIG_SERVICE_TELNETD        : Telnet Protocol (TELNET).
 *   RTEMS_BSD_CONFIG_TELNETD_STACK_SIZE    : Telnet shell task stack size.
 *  RTEMS_BSD_CONFIG_SERVICE_FTPD           : File Transfer Protocol (FTP).
 *  RTEMS_BSD_CONFIG_BSP_CONFIG             : Configure default BSP devices.
 *  RTEMS_BSD_CONFIG_INIT                   : Configure the LibBSD support.
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

/*
 * Include the RTEMS BSD support.
 */
#include <rtems/bsd/bsd.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * BSD Kernel configuration.
 */
#if defined(RTEMS_BSD_CONFIG_DOMAIN_PAGE_MBUFS_SIZE)
  #define RTEMS_BSD_CFGDECL_DOMAIN_PAGE_MBUFS_SIZE RTEMS_BSD_CONFIG_DOMAIN_PAGE_MBUFS_SIZE
#else
  #define RTEMS_BSD_CFGDECL_DOMAIN_PAGE_MBUFS_SIZE RTEMS_BSD_ALLOCATOR_DOMAIN_PAGE_MBUF_DEFAULT
#endif

#if defined(RTEMS_BSD_CONFIG_DOMAIN_BIO_SIZE)
  #define RTEMS_BSD_CFGDECL_DOMAIN_BIO_SIZE RTEMS_BSD_CONFIG_DOMAIN_BIO_SIZE
#else
  #define RTEMS_BSD_CFGDECL_DOMAIN_BIO_SIZE RTEMS_BSD_ALLOCATOR_DOMAIN_BIO_DEFAULT
#endif

/*
 * BSD Kernel modules.
 */

/*
 * Unix packet filter.
 *  https://www.freebsd.org/doc/handbook/firewalls-pf.html
 */
#if defined(RTEMS_BSD_CONFIG_NET_PF_UNIX)
  #define RTEMS_BSD_CFGDECL_NET_PF_UNIX SYSINIT_NEED_NET_PF_UNIX
#else
  #define RTEMS_BSD_CFGDECL_NET_PF_UNIX
#endif /* RTEMS_BSD_CONFIG_NET_PF_UNIX */

#if defined(RTEMS_BSD_CONFIG_NET_IP_MROUTE)
  #define RTEMS_BSD_CFGDECL_NET_IP_MROUTE SYSINIT_NEED_NET_IP_MROUTE
#else
  #define RTEMS_BSD_CFGDECL_NET_IP_MROUTE
#endif /* RTEMS_BSD_CONFIG_NET_IP_MROUTE */

#if defined(RTEMS_BSD_CONFIG_NET_IP6_MROUTE)
  #define RTEMS_BSD_CFGDECL_NET_IP6_MROUTE SYSINIT_NEED_NET_IP6_MROUTE
#else
  #define RTEMS_BSD_CFGDECL_NET_IP6_MROUTE
#endif /* RTEMS_BSD_CONFIG_NET_IP6_MROUTE */

/*
 * Bridging.
 *  https://www.freebsd.org/doc/handbook/network-bridging.html
 */
#if defined(RTEMS_BSD_CONFIG_NET_IF_BRIDGE)
  #define RTEMS_BSD_CFGDECL_NET_IF_BRIDGE SYSINIT_NEED_NET_IF_BRIDGE
#else
  #define RTEMS_BSD_CFGDECL_NET_IF_BRIDGE
#endif /* RTEMS_BSD_CONFIG_NET_IF_BRIDGE */

/*
 * Link Aggregation and Failover.
 *  https://www.freebsd.org/doc/handbook/network-aggregation.html
 */
#if defined(RTEMS_BSD_CONFIG_NET_IF_LAGG)
  #define RTEMS_BSD_CFGDECL_NET_IF_LAGG SYSINIT_NEED_NET_IF_LAGG
#else
  #define RTEMS_BSD_CFGDECL_NET_IF_LAGG
#endif /* RTEMS_BSD_CONFIG_NET_IF_LAGG */

/*
 * Virtual LAN
 *  https://www.freebsd.org/cgi/man.cgi?query=rc.conf then 'network_interfaces'.
 */
#if defined(RTEMS_BSD_CONFIG_NET_IF_VLAN)
  #define RTEMS_BSD_CFGDECL_NET_IF_VLAN SYSINIT_NEED_NET_IF_VLAN
#else
  #define RTEMS_BSD_CFGDECL_NET_IF_VLAN
#endif /* RTEMS_BSD_CONFIG_NET_IF_VLAN */

/*
 * Firewall PF
 */
#if defined(RTEMS_BSD_CONFIG_FIREWALL_PF)
  #define RTEMS_BSD_CFGDECL_FIREWALL_PF SYSINIT_NEED_FIREWALL_PF
  #define RTEMS_BSD_CFGDECL_FIREWALL_PF_SERVICE \
      RTEMS_BSD_RC_CONF_SYSINT(rc_conf_firewall_pf)
#else
  #define RTEMS_BSD_CFGDECL_FIREWALL_PF
  #define RTEMS_BSD_CFGDECL_FIREWALL_PF_SERVICE
#endif /* RTEMS_BSD_CONFIG_FIREWALL_PF */

#if defined(RTEMS_BSD_CONFIG_FIREWALL_PFLOG)
  #define RTEMS_BSD_CFGDECL_FIREWALL_PFLOG SYSINIT_NEED_FIREWALL_PFLOG
#else
  #define RTEMS_BSD_CFGDECL_FIREWALL_PFLOG
#endif /* RTEMS_BSD_CONFIG_FIREWALL_PFLOG */

#if defined(RTEMS_BSD_CONFIG_FIREWALL_PFSYNC)
  #define RTEMS_BSD_CFGDECL_FIREWALL_PFSYNC SYSINIT_NEED_FIREWALL_PFSYNC
#else
  #define RTEMS_BSD_CFGDECL_FIREWALL_PFSYNC
#endif /* RTEMS_BSD_CONFIG_FIREWALL_PFSYNC */

/*
 * IPSec
 */
#if defined(RTEMS_BSD_CONFIG_IPSEC)
  #define RTEMS_BSD_CFGDECL_IPSEC \
      SYSINIT_NEED_NET_IF_GIF; \
      SYSINIT_NEED_CRYPTODEV
  #define RTEMS_BSD_CFGDECL_IPSEC_SERVICE \
      RTEMS_BSD_RC_CONF_SYSINT(rc_conf_ipsec)
#else
  #define RTEMS_BSD_CFGDECL_IPSEC
  #define RTEMS_BSD_CFGDECL_IPSEC_SERVICE
#endif /* RTEMS_BSD_CONFIG_FIREWALL_PF */

/*
 * FTPD
 */
#if defined(RTEMS_BSD_CONFIG_SERVICE_FTPD)
  #define RTEMS_BSD_CFGDECL_FTPD RTEMS_BSD_RC_CONF_SYSINT(rc_conf_ftpd)
#else
  #define RTEMS_BSD_CFGDECL_FTPD
#endif /* RTEMS_BSD_CONFIG_SERVICE_FTPD */

/*
 * Telnetd
 */
#if defined(RTEMS_BSD_CONFIG_SERVICE_TELNETD)
  #if !defined(RTEMS_BSD_CONFIG_TELNETD_STACK_SIZE)
    #define RTEMS_BSD_CONFIG_TELNETD_STACK_SIZE (8 * 1024)
  #endif /* RTEMS_BSD_CONFIG_TELNETD_STACK_SIZE */
  #define RTEMS_BSD_CFGDECL_TELNETD_STACK_SIZE \
    int rtems_telnetd_stack_size = RTEMS_BSD_CONFIG_TELNETD_STACK_SIZE
  #define RTEMS_BSD_CFGDECL_TELNETD RTEMS_BSD_RC_CONF_SYSINT(rc_conf_telnetd)
#else
  #define RTEMS_BSD_CFGDECL_TELNETD_STACK_SIZE
  #define RTEMS_BSD_CFGDECL_TELNETD
#endif /* RTEMS_BSD_CONFIG_SERVICE_TELNETD */

/*
 * Termios
 */
#if defined(RTEMS_BSD_CONFIG_TERMIOS_KQUEUE_AND_POLL)
  #define RTEMS_BSD_CFGDECL_TERMIOS_KQUEUE_AND_POLL SYSINIT_REFERENCE(termioskqueuepoll)
#else
  #define RTEMS_BSD_CFGDECL_TERMIOS_KQUEUE_AND_POLL
#endif /* RTEMS_BSD_CONFIG_TERMIOS_KQUEUE_AND_POLL */

/*
 * Configure the system.
 */
#if defined(RTEMS_BSD_CONFIG_INIT)
  /*
   * Configure the domain allocation memory size.
   */
  uintptr_t rtems_bsd_allocator_domain_page_mbuf_size = \
    RTEMS_BSD_CFGDECL_DOMAIN_PAGE_MBUFS_SIZE;

  /*
   * Configure the block IO buffer memory size.
   */
  uintptr_t rtems_bsd_allocator_domain_bio_size = \
    RTEMS_BSD_CFGDECL_DOMAIN_BIO_SIZE;

  /*
   * If a BSP configuration is requested include the Nexus bus BSP
   * configuration.
   */
  #if defined(RTEMS_BSD_CONFIG_BSP_CONFIG)
    #include <bsp/nexus-devices.h>
  #endif /* RTEMS_BSD_CONFIG_BSP_CONFIG */

  /*
   * Create the networking modules and interfaces.
   */
  RTEMS_BSD_CFGDECL_NET_PF_UNIX;
  RTEMS_BSD_CFGDECL_NET_IP_MROUTE;
  RTEMS_BSD_CFGDECL_NET_IP6_MROUTE;
  RTEMS_BSD_CFGDECL_NET_IF_BRIDGE;
  RTEMS_BSD_CFGDECL_NET_IF_LAGG;
  RTEMS_BSD_CFGDECL_NET_IF_VLAN;

  /*
   * Create the firewall
   */
  RTEMS_BSD_CFGDECL_FIREWALL_PF;
  RTEMS_BSD_CFGDECL_FIREWALL_PF_SERVICE;
  RTEMS_BSD_CFGDECL_FIREWALL_PFLOG;
  RTEMS_BSD_CFGDECL_FIREWALL_PFSYNC;

  /*
   * IPSec related stuff.
   */
  RTEMS_BSD_CFGDECL_IPSEC;
  RTEMS_BSD_CFGDECL_IPSEC_SERVICE;

  /*
   * Create the services.
   */
  RTEMS_BSD_CFGDECL_TELNETD;
  RTEMS_BSD_CFGDECL_TELNETD_STACK_SIZE;
  RTEMS_BSD_CFGDECL_FTPD;

  RTEMS_BSD_CFGDECL_TERMIOS_KQUEUE_AND_POLL;
#endif /* RTEMS_BSD_CONFIG_INIT */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_BSD_CONFIG_h */
