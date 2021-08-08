/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief This file provide a high level interface to simple interface
 * support calls.
 */

/*
 * Copyright (c) 2021. Chris Johns <chrisj@rtems.org>. All rights reserved.
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

#ifndef _RTEMS_BSD_IFACE_H_
#define _RTEMS_BSD_IFACE_H_

#include <stdbool.h>

#include <sys/socket.h>

#include <net/if.h>
#include <net/if_media.h>
#include <netinet/in.h>

struct rtems_bsd_iface {
	char name[IFNAMSIZ];
	int unit;
	struct in_addr address;
	size_t hw_len;
	uint8_t hw_address[16];
	struct ifreq ifr;
	int linkstate;
};

/*
 * These calls return 0 is successful and -1 and errno set on error.
 */
int rtems_bsd_iface_get(const char *name, struct rtems_bsd_iface *iface);
int rtems_bsd_iface_link_state(const char *name, bool *state);

#endif
