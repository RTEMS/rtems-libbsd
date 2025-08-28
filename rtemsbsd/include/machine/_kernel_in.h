/*-
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 1982, 1986, 1990, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * Taken from FreeBSD-14. This is the _KERNEL defined fragments of
 * sys/netinet/in.h.
 */

#define IN_LOOPBACK(i) \
    (((in_addr_t)(i) & V_in_loopback_mask) == 0x7f000000)
#define IN_LOOPBACK_MASK_DFLT	0xff000000

#include <net/vnet.h>

VNET_DECLARE(bool, ip_allow_net0);
VNET_DECLARE(bool, ip_allow_net240);
/* Address space reserved for loopback */
VNET_DECLARE(uint32_t, in_loopback_mask);
#define	V_ip_allow_net0		VNET(ip_allow_net0)
#define	V_ip_allow_net240	VNET(ip_allow_net240)
#define	V_in_loopback_mask	VNET(in_loopback_mask)

struct ifnet; struct mbuf;	/* forward declarations for Standard C */
struct in_ifaddr;

int	 in_broadcast(struct in_addr, struct ifnet *);
int	 in_ifaddr_broadcast(struct in_addr, struct in_ifaddr *);
bool	 in_canforward(struct in_addr);
bool	 in_localaddr(struct in_addr);
bool	 in_localip(struct in_addr);
bool	 in_localip_fib(struct in_addr, uint16_t);
bool	 in_ifhasaddr(struct ifnet *, struct in_addr);
struct in_ifaddr *in_findlocal(uint32_t, bool);
int	 inet_aton(const char *, struct in_addr *); /* in libkern */
char	*inet_ntoa_r(struct in_addr ina, char *buf); /* in libkern */
char	*inet_ntop(int, const void *, char *, socklen_t); /* in libkern */
int	 inet_pton(int af, const char *, void *); /* in libkern */
void	 in_ifdetach(struct ifnet *);

#define	in_hosteq(s, t)	((s).s_addr == (t).s_addr)
#define	in_nullhost(x)	((x).s_addr == INADDR_ANY)
#define	in_allhosts(x)	((x).s_addr == htonl(INADDR_ALLHOSTS_GROUP))

#define	satosin(sa)	((struct sockaddr_in *)(sa))
#define	sintosa(sin)	((struct sockaddr *)(sin))
#define	ifatoia(ifa)	((struct in_ifaddr *)(ifa))
