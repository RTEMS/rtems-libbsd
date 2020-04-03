/**
 * @file
 *
 * @brief This test program processes all ARP requests and claims every IP address it
 * notices.
 *
 * Use with care in production networks.
 */

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

#include <sys/types.h>

#include <net/if.h>
#include <net/if_arp.h>
#include <net/ethernet.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <assert.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <rtems/bsd/util.h>

#define TEST_NAME "LIBBSD ARP HOLE"
#define TEST_STATE_USER_INPUT 1

typedef struct {
	struct ether_header eh;
	struct arphdr arh;
	uint8_t sha[ETHER_ADDR_LEN];
	uint32_t spa;
	uint8_t tha[ETHER_ADDR_LEN];
	uint32_t tpa;
} __packed arp_request;

static void
arp_processor(void *arg, int fd, const uint8_t eaddr[ETHER_ADDR_LEN],
    const struct arphdr *ar, uint32_t spa, uint32_t tpa,
    const uint8_t *sha, const uint8_t *tha)
{
	if (ar->ar_op == htons(ARPOP_REQUEST) && spa == htonl(INADDR_ANY) &&
	    tpa != htonl(INADDR_ANY)) {
		arp_request arr;

		memcpy(&arr.eh.ether_shost[0], &eaddr[0], ETHER_HDR_LEN);
		memcpy(&arr.eh.ether_dhost[0], sha, ETHER_HDR_LEN);
		arr.eh.ether_type = htons(ETHERTYPE_ARP);

		arr.arh.ar_hrd = htons(ARPHRD_ETHER);
		arr.arh.ar_pro = htons(ETHERTYPE_IP);
		arr.arh.ar_hln = ETHER_ADDR_LEN;
		arr.arh.ar_pln = sizeof(spa);
		arr.arh.ar_op = htons(ARPOP_REPLY);

		memcpy(&arr.sha[0], &eaddr[0], ETHER_ADDR_LEN);
		arr.spa = tpa;
		memcpy(&arr.tha[0], sha, ETHER_ADDR_LEN);
		arr.tpa = htonl(INADDR_ANY);

		write(fd, &arr, sizeof(arr));
	}
}

static void
test_main(void)
{
	char ifnamebuf[IF_NAMESIZE];
	char *ifname;
	rtems_bsd_arp_processor_context *ctx;

	ifname = if_indextoname(1, &ifnamebuf[0]);
	assert(ifname != NULL);

	ctx = rtems_bsd_arp_processor_create(ifname);
	assert(ctx != NULL);

	while (1) {
		rtems_bsd_arp_processor_process(ctx, arp_processor, NULL);
	}
}

#define DEFAULT_NETWORK_NO_STATIC_IFCONFIG

#include <rtems/bsd/test/default-network-init.h>
