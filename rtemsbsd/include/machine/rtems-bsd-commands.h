/**
 * @file
 *
 * @ingroup rtems_bsd_machine
 *
 * @brief TODO.
 */

/*
 * Copyright (c) 2013, 2016 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
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

#ifndef _RTEMS_BSD_MACHINE_RTEMS_BSD_COMMANDS_H_
#define _RTEMS_BSD_MACHINE_RTEMS_BSD_COMMANDS_H_

#include <sys/cdefs.h>

#define RTEMS_BSD_ARGC(array) (sizeof(array) / sizeof((array)[0]) - 1)

__BEGIN_DECLS

int rtems_bsd_command_arp(int argc, char **argv);

int rtems_bsd_command_ifconfig(int argc, char **argv);

int rtems_bsd_command_ifmcstat(int argc, char **argv);

int rtems_bsd_command_netstat(int argc, char **argv);

int rtems_bsd_command_nvmecontrol(int argc, char **argv);

int rtems_bsd_command_pfctl(int argc, char **argv);

int rtems_bsd_command_ping(int argc, char **argv);

int rtems_bsd_command_ping6(int argc, char **argv);

int rtems_bsd_command_route(int argc, char **argv);

int rtems_bsd_command_wpa_supplicant(int argc, char **argv);

int rtems_bsd_command_wpa_supplicant_fork(int argc, char **argv);

int rtems_bsd_command_tcpdump(int argc, char **argv);

int rtems_bsd_command_sysctl(int argc, char **argv);

int rtems_bsd_command_vmstat(int argc, char **argv);

int rtems_bsd_command_wlanstats(int argc, char **argv);

int rtems_bsd_command_stty(int argc, char **argv);

int rtems_bsd_command_racoon(int argc, char **argv);

int rtems_bsd_command_setkey(int argc, char **argv);

int rtems_bsd_command_openssl(int argc, char **argv);

__END_DECLS

#endif /* _RTEMS_BSD_MACHINE_RTEMS_BSD_COMMANDS_H_ */
