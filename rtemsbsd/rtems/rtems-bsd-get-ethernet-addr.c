/*
 * Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
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

#include <rtems/bsd/util.h>

#include <sys/cdefs.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <net/if_dl.h>

#include <errno.h>
#include <ifaddrs.h>
#include <string.h>

int
rtems_bsd_get_ethernet_addr(const char *ifname, uint8_t eaddr[ETHER_ADDR_LEN])
{
	int rv;
	struct ifaddrs *ifa;

	rv = getifaddrs(&ifa);
	if (rv == 0) {
		struct ifaddrs *cur;

		for (cur = ifa; cur != NULL; cur = cur->ifa_next) {
			if (strcmp(cur->ifa_name, ifname) == 0 &&
			    cur->ifa_addr->sa_family == AF_LINK) {
				const struct sockaddr_dl *dl =
				    (const struct sockaddr_dl *) cur->ifa_addr;

				memcpy(&eaddr[0], LLADDR(dl), ETHER_ADDR_LEN);
				break;
			}
		}

		freeifaddrs(ifa);

		if (cur == NULL) {
			errno = ENXIO;
			rv = -1;
		}
	}

	return rv;
}
