/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief Interface support routines
 */

/*
 * Copyright (c) 2021. Chris Johns <chrisj@rtems.org>  All rights reserved.
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

#include <errno.h>
#include <stdio.h>
#include <sys/ioctl.h>

#include <ifaddrs.h>
#include <net/if_dl.h>

#include <rtems/bsd/iface.h>

#if !defined(RTEMS_BSD_IFACE_TRACE)
#define RTEMS_BSD_IFACE_TRACE 0
#endif

int
rtems_bsd_iface_get(const char *name, struct rtems_bsd_iface *iface)
{
	struct ifaddrs *ifap, *ifa;
	bool found = false;

	if (iface == NULL) {
		errno = EINVAL;
		return -1;
	}

	memset(iface, 0, sizeof(*iface));

	if (getifaddrs(&ifap) != 0) {
		if (RTEMS_BSD_IFACE_TRACE) {
			printf("bsd: iface: getifaddrs failed: %s\n", strerror(errno));
		}
		return -1;
	}

	for (ifa = ifap; ifa != NULL; ifa = ifa->ifa_next) {
		if ((ifa->ifa_flags & IFF_LOOPBACK) ||
		    (ifa->ifa_flags & IFF_POINTOPOINT))
			continue;

		if (strcmp(name, ifa->ifa_name) != 0)
			continue;

		found = true;

		if (ifa->ifa_addr->sa_family == AF_LINK) {
			struct sockaddr_dl *sa;
			sa = (struct sockaddr_dl *)ifa->ifa_addr;
			iface->unit = sa->sdl_index;
			iface->hw_len = sa->sdl_alen;
			memcpy(iface->hw_address, LLADDR(sa), sa->sdl_alen);
		} else if (ifa->ifa_addr->sa_family == AF_INET) {
			struct sockaddr_in sa;
			memcpy(&sa, ifa->ifa_addr, sizeof(sa));
			if (sa.sin_addr.s_addr == htonl(INADDR_LOOPBACK))
				continue;
			strlcpy(iface->ifr.ifr_name, ifa->ifa_name, IFNAMSIZ);
			memcpy(&iface->ifr.ifr_addr, ifa->ifa_addr,
			    ifa->ifa_addr->sa_len);
			iface->address = sa.sin_addr;
		}

	}

	freeifaddrs(ifap);

	if (!found) {
		if (RTEMS_BSD_IFACE_TRACE) {
			printf("bsd: iface: not-found: %s\n", name);
		}
		errno = ENOENT;
		return -1;
	}

	strlcpy(iface->name, name, sizeof(iface->name));

	return 0;
}

int
rtems_bsd_iface_link_state(const char *name, bool *state)
{
	struct rtems_bsd_iface iface;
	struct ifmediareq ifmr;
	int s;
	int rc;

	*state = false;

	rc = rtems_bsd_iface_get(name, &iface);
	if (rc < 0) {
		return -1;
	}

	s = socket(iface.ifr.ifr_addr.sa_family, SOCK_DGRAM, 0);
	if (s < 0) {
		if (RTEMS_BSD_IFACE_TRACE) {
			printf("bsd: iface: link state: socket open failed: %s\n", strerror(errno));
		}
		return -1;
	}

	memset(&ifmr, 0, sizeof(ifmr));
        strlcpy(ifmr.ifm_name, name, sizeof(ifmr.ifm_name));

        rc = ioctl(s, SIOCGIFMEDIA, (caddr_t)&ifmr);
	close(s);

	if (rc < 0) {
		if (RTEMS_BSD_IFACE_TRACE) {
			printf("bsd: iface: link state: socket ioctl failed: %s\n", strerror(errno));
		}
		return -1;
	}

	if ((ifmr.ifm_status & IFM_AVALID) != 0) {
		*state = (ifmr.ifm_status & IFM_ACTIVE) != 0;
	}

	return 0;
}
