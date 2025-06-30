/*
 * Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * Copyright (c) 2006-2013 Roy Marples <roy@marples.name>
 *
 * Some parts are derived from the dhcpcd sources.
 *
 * http://roy.marples.name/projects/dhcpcd
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
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <net/bpf.h>
#include <net/if_dl.h>
#include <arpa/inet.h>

#include <errno.h>
#include <fcntl.h>
#include <ifaddrs.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct rtems_bsd_arp_processor_context {
	int fd;
	uint8_t eaddr[ETHER_ADDR_LEN];
	size_t buf_size;
	uint8_t buf[];
};

#ifndef BPF_ETHCOOK
# define BPF_ETHCOOK 0
#endif
#ifndef BPF_WHOLEPACKET
# define BPF_WHOLEPACKET ~0U
#endif
static const struct bpf_insn arp_bpf_filter[] = {
	/* Make sure this is an ARP packet... */
	BPF_STMT(BPF_LD + BPF_H + BPF_ABS, 12),
	BPF_JUMP(BPF_JMP + BPF_JEQ + BPF_K, ETHERTYPE_ARP, 0, 3),
	/* Make sure this is an ARP REQUEST... */
	BPF_STMT(BPF_LD + BPF_H + BPF_ABS, 20 + BPF_ETHCOOK),
	BPF_JUMP(BPF_JMP + BPF_JEQ + BPF_K, ARPOP_REQUEST, 2, 0),
	/* or ARP REPLY... */
	BPF_STMT(BPF_LD + BPF_H + BPF_ABS, 20 + BPF_ETHCOOK),
	BPF_JUMP(BPF_JMP + BPF_JEQ + BPF_K, ARPOP_REPLY, 0, 1),
	/* If we passed all the tests, ask for the whole packet. */
	BPF_STMT(BPF_RET + BPF_K, BPF_WHOLEPACKET),
	/* Otherwise, drop it. */
	BPF_STMT(BPF_RET + BPF_K, 0)
};

static const size_t arp_bpf_filter_len =
    sizeof(arp_bpf_filter) / sizeof(arp_bpf_filter[0]);

static void
process_arp_packet(const uint8_t *pkt, size_t pkt_size, int fd,
    const uint8_t eaddr[ETHER_ADDR_LEN], rtems_bsd_arp_processor processor,
    void *arg)
{
	struct arphdr ar;
	uint32_t spa;
	uint32_t tpa;
	const uint8_t *sha;
	const uint8_t *tha;

	/* We must have a full ARP header */
	if (pkt_size < sizeof(ar))
		return;

	memcpy(&ar, pkt, sizeof(ar));

	/* Protocol must be IP. */
	if (ar.ar_pro != htons(ETHERTYPE_IP))
		return;

	if (ar.ar_pln != sizeof(spa))
		return;

	/* Only these types are recognised */
	if (ar.ar_op != htons(ARPOP_REPLY) &&
	    ar.ar_op != htons(ARPOP_REQUEST))
		return;

	/* Get pointers to the hardware addresses */
	sha = pkt + sizeof(ar);
	tha = sha + ar.ar_hln + ar.ar_pln;

	/* Ensure we got all the data */
	if ((tha + ar.ar_hln + ar.ar_pln) - pkt > pkt_size)
		return;

	/* Ignore messages from ourself */
	if (ar.ar_hln == ETHER_ADDR_LEN &&
	    memcmp(sha, &eaddr[0], ETHER_ADDR_LEN) == 0)
		return;

	/* Copy out the IP addresses */
	memcpy(&spa, sha + ar.ar_hln, ar.ar_pln);
	memcpy(&tpa, tha + ar.ar_hln, ar.ar_pln);

	(*processor)(arg, fd, eaddr, &ar, spa, tpa, sha, tha);
}

rtems_bsd_arp_processor_context *
rtems_bsd_arp_processor_create(const char *ifname)
{
	rtems_bsd_arp_processor_context *ctx = NULL;
	int fd = -1;
	struct ifreq ifr;
	int buf_len;
	size_t buf_size;
	struct bpf_version pv;
	struct bpf_program pf;
	int flags;
	int rv;

	fd = open("/dev/bpf", O_RDWR);
	if (fd < 0) {
		goto error;
	}

	rv = ioctl(fd, BIOCVERSION, &pv);
	if (rv != 0 || pv.bv_major != BPF_MAJOR_VERSION ||
	    pv.bv_minor < BPF_MINOR_VERSION) {
		goto error;
	}

	memset(&ifr, 0, sizeof(ifr));
	strlcpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name));
	rv = ioctl(fd, BIOCSETIF, &ifr);
	if (rv != 0) {
		goto error;
	}

	rv = ioctl(fd, BIOCGBLEN, &buf_len);
	if (rv != 0 || buf_len <= 0) {
		goto error;
	}
	buf_size = (size_t) buf_len;

	flags = 1;
	rv = ioctl(fd, BIOCIMMEDIATE, &flags);
	if (rv != 0) {
		goto error;
	}

	pf.bf_insns = __DECONST(struct bpf_insn *, arp_bpf_filter);
	pf.bf_len = arp_bpf_filter_len;
	rv = ioctl(fd, BIOCSETF, &pf);
	if (rv != 0) {
		goto error;
	}

	ctx = malloc(sizeof(*ctx) + buf_size);
	if (ctx == NULL) {
		goto error;
	}

	rv = rtems_bsd_get_ethernet_addr(ifname, &ctx->eaddr[0]);
	if (rv != 0) {
		goto error;
	}

	ctx->fd = fd;
	ctx->buf_size = buf_size;

	return ctx;

error:
	close(fd);
	free(ctx);

	return NULL;
}

int
rtems_bsd_arp_processor_get_file_descriptor(
    const rtems_bsd_arp_processor_context *ctx)
{
	return ctx->fd;
}

int
rtems_bsd_arp_processor_process(
    rtems_bsd_arp_processor_context *ctx,
    rtems_bsd_arp_processor processor, void *arg)
{
	ssize_t n;
	size_t buf_avail;
	size_t buf_pos;

	n = read(ctx->fd, &ctx->buf[0], ctx->buf_size);
	if (n < 0) {
		return -1;
	}

	buf_avail = (size_t) n;
	buf_pos = 0;

	while (buf_pos < buf_avail) {
		const uint8_t *pkt = &ctx->buf[0] + buf_pos;
		struct bpf_hdr bp;

		memcpy(&bp, pkt, sizeof(bp));

		if (bp.bh_caplen == bp.bh_datalen &&
		    buf_pos + bp.bh_caplen + bp.bh_hdrlen <= buf_avail) {
			const uint8_t *payload = pkt + bp.bh_hdrlen +
			    ETHER_HDR_LEN;
			size_t payload_size = bp.bh_caplen - ETHER_HDR_LEN;

			process_arp_packet(payload, payload_size, ctx->fd, ctx->eaddr,
			    processor, NULL);
		}

		buf_pos += BPF_WORDALIGN(bp.bh_hdrlen + bp.bh_caplen);
	}

	return 0;
}
