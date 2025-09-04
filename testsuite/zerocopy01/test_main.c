/**
 * @file
 *
 * @brief Zerocopy test.
 */

/*
 * Copyright (c) 2015 embedded brains GmbH.  All rights reserved.
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
 * ARE DISCLAIMED.	IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/types.h>
#include <sys/param.h>
#include <sys/mbuf.h>
#include <sys/malloc.h>
#include <sys/queue.h>

#include <net/if.h>
#include <net/if_arp.h>
#include <net/ethernet.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <rtems.h>
#include <rtems/shell.h>
#include <rtems/bsd/zerocopy.h>
#include <rtems/bsd/test/network-config.h>

#define TEST_NAME "LIBBSD ZEROCOPY 1"
#define TEST_STATE_USER_INPUT 1

#define BUFFER_COUNT 128

#define NOTIFY_THRESHOLD 64

#define DATA_SIZE (ETHERMTU - sizeof(struct ip) - sizeof(struct udphdr))

struct buffer {
	SLIST_ENTRY(buffer) link;
	u_int ref_cnt;
	char *data;
};

struct buffer_control {
	SLIST_HEAD(buffer_list, buffer) free_list;
	rtems_interrupt_lock lock;
	size_t free_buffers;
	rtems_id waiting_task;
	struct buffer buffers[BUFFER_COUNT];
	char data[DATA_SIZE][BUFFER_COUNT];
};

static struct buffer_control buffer_control;

static void
buffer_free(struct mbuf *m)
{
	struct buffer_control *bc = m->m_ext.ext_arg1;
	struct buffer *buf = m->m_ext.ext_arg2;
	rtems_status_code sc;
	rtems_interrupt_lock_context lock_context;
	rtems_id waiting_task;

	buf->ref_cnt = 0;

	rtems_interrupt_lock_acquire(&bc->lock, &lock_context);
	SLIST_INSERT_HEAD(&bc->free_list, buf, link);
	waiting_task = bc->waiting_task;
	++bc->free_buffers;
	if (bc->free_buffers < NOTIFY_THRESHOLD) {
		waiting_task = 0;
	}
	rtems_interrupt_lock_release(&bc->lock, &lock_context);

	if (waiting_task != 0) {
		sc = rtems_event_transient_send(waiting_task);
		assert(sc == RTEMS_SUCCESSFUL);
	}
}

static struct mbuf *
buffer_get(struct buffer_control *bc)
{
	struct mbuf *m = rtems_bsd_m_gethdr(M_WAITOK, MT_DATA);
	struct buffer *buf;
	rtems_status_code sc;
	rtems_interrupt_lock_context lock_context;

	do {
		rtems_interrupt_lock_acquire(&bc->lock, &lock_context);
		if (SLIST_EMPTY(&bc->free_list)) {
			bc->waiting_task = rtems_task_self();
			rtems_interrupt_lock_release(&bc->lock, &lock_context);

			sc = rtems_event_transient_receive(RTEMS_WAIT,
			    RTEMS_NO_TIMEOUT);
			assert(sc == RTEMS_SUCCESSFUL);

			buf = NULL;
		} else {
			buf = SLIST_FIRST(&bc->free_list);
			SLIST_REMOVE_HEAD(&bc->free_list, link);
			--bc->free_buffers;
			rtems_interrupt_lock_release(&bc->lock, &lock_context);
		}
	} while (buf == NULL);

	m->m_len = DATA_SIZE;
	m->m_pkthdr.len = DATA_SIZE;
	rtems_bsd_m_extaddref(m, buf, DATA_SIZE, &buf->ref_cnt, buffer_free,
	    bc, buf);

	return (m);
}

static void
network_flood_task(rtems_task_argument arg)
{
	int fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	struct sockaddr_in addr = {
		.sin_len = sizeof(addr),
		.sin_family = AF_INET,
		.sin_port = htons(13161),
		.sin_addr = {
			.s_addr = INADDR_ANY
		}
	};
	int rv = inet_aton(NET_CFG_PEER_IP, &addr.sin_addr);
	struct buffer_control *bc = (struct buffer_control *)arg;

	assert(fd >= 0);
	assert(rv != 0);

	while (true) {
		struct mbuf *m = buffer_get(bc);
		int error = rtems_bsd_sendto(
			fd,
			m,
			0,
			(const struct sockaddr *) &addr
		);

		if (error != 0) {
			printf("zerocopy sendto error: %s\n", strerror(error));
			sleep(1);
		}
	}
}

static void
test_main(void)
{
	struct buffer_control *bc = &buffer_control;
	rtems_status_code sc;
	rtems_id id;
	size_t i;

	SLIST_INIT(&bc->free_list);

	for (i = 0; i < BUFFER_COUNT; ++i) {
		SLIST_INSERT_HEAD(&bc->free_list, &bc->buffers[i], link);
		bc->buffers[i].data = &bc->data[i][0];
	}

	sc = rtems_task_create(
		rtems_build_name('F', 'L', 'O', 'D'),
		3,
		RTEMS_MINIMUM_STACK_SIZE,
		RTEMS_DEFAULT_MODES,
		RTEMS_DEFAULT_ATTRIBUTES,
		&id
	);
	assert(sc == RTEMS_SUCCESSFUL);

	sc = rtems_task_start(id, network_flood_task, (rtems_task_argument) bc);
	assert(sc == RTEMS_SUCCESSFUL);

	rtems_task_exit();
}

#define DEFAULT_NETWORK_DHCPCD_ENABLE
#define DEFAULT_NETWORK_SHELL

#define CONFIGURE_MAXIMUM_DRIVERS 32

#include <rtems/bsd/test/default-network-init.h>
