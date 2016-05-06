#!/bin/sh -x

#
# Copyright (c) 2016 embedded brains GmbH.  All rights reserved.
#
#  embedded brains GmbH
#  Dornierstr. 4
#  82178 Puchheim
#  Germany
#  <rtems@embedded-brains.de>
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.
#

#
# This script generates the kernel namespace header file
# <machine/rtems-bsd-kernel-namespace.h>.
#

objdump --syms `for i in build/*rtems* ; do find $i/freebsd/sys/ -name '*.o' ; echo \
	$i/rtemsbsd/rtems/rtems-bsd-assert*.o \
	$i/rtemsbsd/rtems/rtems-bsd-autoconf*.o \
	$i/rtemsbsd/rtems/rtems-bsd-bus-dma-mbuf*.o \
	$i/rtemsbsd/rtems/rtems-bsd-bus-dma*.o \
	$i/rtemsbsd/rtems/rtems-bsd-bus-root*.o \
	$i/rtemsbsd/rtems/rtems-bsd-cam*.o \
	$i/rtemsbsd/rtems/rtems-bsd-chunk*.o \
	$i/rtemsbsd/rtems/rtems-bsd-configintrhook*.o \
	$i/rtemsbsd/rtems/rtems-bsd-conf*.o \
	$i/rtemsbsd/rtems/rtems-bsd-delay*.o \
	$i/rtemsbsd/rtems/rtems-bsd-get-file*.o \
	$i/rtemsbsd/rtems/rtems-bsd-init*.o \
	$i/rtemsbsd/rtems/rtems-bsd-irqs*.o \
	$i/rtemsbsd/rtems/rtems-bsd-jail*.o \
	$i/rtemsbsd/rtems/rtems-bsd-log*.o \
	$i/rtemsbsd/rtems/rtems-bsd-malloc*.o \
	$i/rtemsbsd/rtems/rtems-bsd-mbuf*.o \
	$i/rtemsbsd/rtems/rtems-bsd-muteximpl*.o \
	$i/rtemsbsd/rtems/rtems-bsd-mutex*.o \
	$i/rtemsbsd/rtems/rtems-bsdnet-rtrequest*.o \
	$i/rtemsbsd/rtems/rtems-bsd-nexus*.o \
	$i/rtemsbsd/rtems/rtems-bsd-page*.o \
	$i/rtemsbsd/rtems/rtems-bsd-panic*.o \
	$i/rtemsbsd/rtems/rtems-bsd-pci_bus*.o \
	$i/rtemsbsd/rtems/rtems-bsd-pci_cfgreg*.o \
	$i/rtemsbsd/rtems/rtems-bsd-program*.o \
	$i/rtemsbsd/rtems/rtems-bsd-rwlock*.o \
	$i/rtemsbsd/rtems/rtems-bsd-shell*.o \
	$i/rtemsbsd/rtems/rtems-bsd-signal*.o \
	$i/rtemsbsd/rtems/rtems-bsd-sx*.o \
	$i/rtemsbsd/rtems/rtems-bsd-sysctlbyname*.o \
	$i/rtemsbsd/rtems/rtems-bsd-sysctlnametomib*.o \
	$i/rtemsbsd/rtems/rtems-bsd-sysctl*.o \
	$i/rtemsbsd/rtems/rtems-bsd-thread*.o \
	$i/rtemsbsd/rtems/rtems-bsd-timesupport*.o \
	$i/rtemsbsd/rtems/rtems_mii_ioctl_kern*.o ; done` \
	| awk '/^[0-9a-f]+[[:blank:]]+g/ {print $6}' \
	| sed 's/^_bsd_//' \
	| sed '/^accept$/d' \
	| sed '/^arc4random$/d' \
	| sed '/^bind$/d' \
	| sed '/^blackhole$/d' \
	| sed '/^bpf_filter$/d' \
	| sed '/^bpf_jitter$/d' \
	| sed '/^bpf_jitter_enable$/d' \
	| sed '/^bpf_validate$/d' \
	| sed '/^connect$/d' \
	| sed '/^drop_redirect$/d' \
	| sed '/^drop_synfin$/d' \
	| sed '/^free$/d' \
	| sed '/^getentropy$/d' \
	| sed '/^getpeername$/d' \
	| sed '/^getsockname$/d' \
	| sed '/^getsockopt$/d' \
	| sed '/^ifqmaxlen$/d' \
	| sed '/^in6addr_any$/d' \
	| sed '/^kevent$/d' \
	| sed '/^kqueue$/d' \
	| sed '/^listen$/d' \
	| sed '/^malloc$/d' \
	| sed '/^max_datalen$/d' \
	| sed '/^max_hdr$/d' \
	| sed '/^max_linkhdr$/d' \
	| sed '/^max_protohdr$/d' \
	| sed '/^maxsockets$/d' \
	| sed '/^nd6_debug$/d' \
	| sed '/^nd6_delay$/d' \
	| sed '/^nd6_gctimer$/d' \
	| sed '/^nd6_maxnudhint$/d' \
	| sed '/^nd6_mmaxtries$/d' \
	| sed '/^nd6_onlink_ns_rfc4861$/d' \
	| sed '/^nd6_prune$/d' \
	| sed '/^nd6_umaxtries$/d' \
	| sed '/^nd6_useloopback$/d' \
	| sed '/^nmbclusters$/d' \
	| sed '/^nmbjumbo16$/d' \
	| sed '/^nmbjumbo9$/d' \
	| sed '/^nmbjumbop$/d' \
	| sed '/^nmbufs$/d' \
	| sed '/^nolocaltimewait$/d' \
	| sed '/^path_mtu_discovery$/d' \
	| sed '/^pause$/d' \
	| sed '/^pf_osfp_entry_pl$/d' \
	| sed '/^pf_osfp_pl$/d' \
	| sed '/^poll$/d' \
	| sed '/^random$/d' \
	| sed '/^realloc$/d' \
	| sed '/^reallocf$/d' \
	| sed '/^recvfrom$/d' \
	| sed '/^recvmsg$/d' \
	| sed '/^rtems/d' \
	| sed '/^select$/d' \
	| sed '/^sendmsg$/d' \
	| sed '/^sendto$/d' \
	| sed '/^setfib$/d' \
	| sed '/^setsockopt$/d' \
	| sed '/^shutdown$/d' \
	| sed '/^socket$/d' \
	| sed '/^socketpair$/d' \
	| sed '/^soreceive_stream$/d' \
	| sed '/^srandom$/d' \
	| sed '/^strdup$/d' \
	| sed '/^sysctlbyname$/d' \
	| sed '/^sysctl$/d' \
	| sed '/^sysctlnametomib$/d' \
	| sed '/sys_init/d' \
	| sed '/^taskqueue_/d' \
	| sed '/^tcp_offload_listen_start$/d' \
	| sed '/^tcp_offload_listen_stop$/d' \
	| sed '/^ticks$/d' \
	| sed '/^useloopback$/d' \
	| sed '/^_Watchdog_Ticks_since_boot$/d' \
	| sort -u > symbols.txt

cat << eof > rtems-bsd-kernel-namespace.h
#ifndef _RTEMS_BSD_MACHINE_RTEMS_BSD_KERNEL_SPACE_H_
#error "the header file <machine/rtems-bsd-kernel-space.h> must be included first"
#endif

eof
sed 's/^\(.*\)/#define	\1 _bsd_\1/' < symbols.txt >> rtems-bsd-kernel-namespace.h
rm symbols.txt
mv rtems-bsd-kernel-namespace.h rtemsbsd/include/machine/rtems-bsd-kernel-namespace.h
