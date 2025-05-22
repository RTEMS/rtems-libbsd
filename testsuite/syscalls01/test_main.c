/**
 * @file
 *
 * @brief Different system calls are tested.
 */

/*
 * Copyright (c) 2013, 2018 embedded brains GmbH.  All rights reserved.
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

#include <sys/cdefs.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/poll.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/filio.h>
#include <vm/uma.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <err.h>

#include <assert.h>
#include <ck_epoch.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <syslog.h>

#include <rtems.h>
#include <rtems/bsd/bsd.h>
#include <rtems/bsd/modules.h>
#include <rtems/libcsupport.h>

#include <rtems/libio_.h>

#define TEST_NAME "LIBBSD SYSCALLS 1"

typedef void (*no_mem_test_body)(int fd);

typedef struct {
	no_mem_test_body body;
	int fd;
	rtems_id master_task;
} no_mem_test;

typedef struct {
	int domain;
	int type;
	int protocol;
	int expect_errno;
} socket_test;

static socket_test socket_tests[] = {
	{ -1, SOCK_RAW, 0, EAFNOSUPPORT },
	{ PF_UNSPEC, SOCK_DGRAM, 0, EAFNOSUPPORT },
	{ PF_UNIX, SOCK_STREAM, 0, 0 },
	{ PF_UNIX, SOCK_DGRAM, 0, 0 },
	{ PF_UNIX, SOCK_SEQPACKET, 0, 0 },
	{ PF_INET, -1, 0, EPROTOTYPE },
	{ PF_INET, SOCK_RAW, -1, EPROTONOSUPPORT },
	{ PF_INET, SOCK_STREAM, 0, 0 },
	{ PF_INET, SOCK_DGRAM, 0, 0 },
	{ PF_INET, SOCK_SEQPACKET, 0, EPROTOTYPE },
	{ PF_INET, SOCK_RAW, IPPROTO_3PC, 0 },

	{ PF_INET, SOCK_RAW, IPPROTO_ADFS, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_AH, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_AHIP, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_APES, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_ARGUS, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_AX25, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_BHA, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_BLT, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_BRSATMON, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_CARP, 0 },

	{ PF_INET, SOCK_RAW, IPPROTO_CFTP, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_CHAOS, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_CMTP, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_CPHB, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_CPNX, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_DDP, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_DGP, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_DSTOPTS, 0 },

	{ PF_INET, SOCK_RAW, IPPROTO_EGP, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_EMCON, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_ENCAP, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_EON, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_ESP, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_ETHERIP, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_FRAGMENT, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_GGP, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_GMTP, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_GRE, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_HELLO, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_HMP, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_ICMP, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_ICMPV6, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_IDP, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_IDPR, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_IDRP, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_IGMP, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_IGP, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_IGRP, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_IL, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_INLSP, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_INP, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_IP, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_IPCOMP, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_IPCV, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_IPEIP, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_IPIP, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_IPPC, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_IPV4, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_IPV6, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_IRTP, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_KRYPTOLAN, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_LARP, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_LEAF1, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_LEAF2, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_MEAS, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_MHRP, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_MICP, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_MOBILE, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_MTP, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_MUX, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_ND, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_NHRP, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_NONE, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_NSP, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_NVPII, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_OLD_DIVERT, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_OSPFIGP, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_PFSYNC, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_PGM, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_PIGP, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_PIM, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_PRM, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_PUP, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_PVP, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_RCCMON, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_RDP, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_ROUTING, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_RSVP, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_RVD, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_SATEXPAK, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_SATMON, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_SCCSP, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_SCTP, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_SDRP, 0 },
#ifdef IPPROTO_SEP
	{ PF_INET, SOCK_RAW, IPPROTO_SEP, 0 },
#endif
#ifdef	IPPROTO_DCCP
	{ PF_INET, SOCK_RAW, IPPROTO_DCCP, 0 },
#endif
	{ PF_INET, SOCK_RAW, IPPROTO_SKIP, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_SRPC, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_ST, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_SVMTP, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_SWIPE, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_TCF, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_TCP, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_TLSP, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_TP, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_TPXX, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_TRUNK1, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_TRUNK2, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_TTP, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_UDP, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_VINES, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_VISA, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_VMTP, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_WBEXPAK, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_WBMON, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_WSN, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_XNET, 0 },
	{ PF_INET, SOCK_RAW, IPPROTO_XTP, 0 },
	{ PF_IMPLINK, SOCK_DGRAM, 0, EAFNOSUPPORT },
	{ PF_PUP, SOCK_DGRAM, 0, EAFNOSUPPORT },
	{ PF_CHAOS, SOCK_DGRAM, 0, EAFNOSUPPORT },
	{ PF_NETBIOS, SOCK_DGRAM, 0, EAFNOSUPPORT },
	{ PF_ISO, SOCK_DGRAM, 0, EAFNOSUPPORT },
	{ PF_ECMA, SOCK_DGRAM, 0, EAFNOSUPPORT },
	{ PF_DATAKIT, SOCK_DGRAM, 0, EAFNOSUPPORT },
	{ PF_CCITT, SOCK_DGRAM, 0, EAFNOSUPPORT },
	{ PF_SNA, SOCK_DGRAM, 0, EAFNOSUPPORT },
	{ PF_DECnet, SOCK_DGRAM, 0, EAFNOSUPPORT },
	{ PF_DLI, SOCK_DGRAM, 0, EAFNOSUPPORT },
	{ PF_LAT, SOCK_DGRAM, 0, EAFNOSUPPORT },
	{ PF_HYLINK, SOCK_DGRAM, 0, EAFNOSUPPORT },
	{ PF_APPLETALK, SOCK_DGRAM, 0, EAFNOSUPPORT },
	{ PF_ROUTE, SOCK_DGRAM, 0, EPROTOTYPE },
	{ PF_LINK, SOCK_DGRAM, 0, EAFNOSUPPORT },
	{ PF_XTP, SOCK_DGRAM, 0, EAFNOSUPPORT },
	{ PF_COIP, SOCK_DGRAM, 0, EAFNOSUPPORT },
	{ PF_CNT, SOCK_DGRAM, 0, EAFNOSUPPORT },
	{ PF_RTIP, SOCK_DGRAM, 0, EAFNOSUPPORT },
	{ PF_IPX, SOCK_DGRAM, 0, EAFNOSUPPORT },
	{ PF_SIP, SOCK_DGRAM, 0, EAFNOSUPPORT },
	{ PF_PIP, SOCK_DGRAM, 0, EAFNOSUPPORT },
	{ PF_ISDN, SOCK_DGRAM, 0, EAFNOSUPPORT },
#ifdef RTEMS_BSD_MODULE_NETIPSEC
	{ PF_KEY, SOCK_DGRAM, 0, EPROTOTYPE },
#else
	{ PF_KEY, SOCK_DGRAM, 0, EAFNOSUPPORT },
#endif
	{ PF_INET6, SOCK_DGRAM, 0, 0 },
	{ PF_NATM, SOCK_DGRAM, 0, EAFNOSUPPORT },
	{ PF_ATM, SOCK_DGRAM, 0, EAFNOSUPPORT },
	{ pseudo_AF_HDRCMPLT, SOCK_DGRAM, 0, EAFNOSUPPORT },
	{ PF_NETGRAPH, SOCK_DGRAM, 0, EAFNOSUPPORT },
	{ PF_SLOW, SOCK_DGRAM, 0, EAFNOSUPPORT },
	{ PF_SCLUSTER, SOCK_DGRAM, 0, EAFNOSUPPORT },
	{ PF_ARP, SOCK_DGRAM, 0, EAFNOSUPPORT },
	{ PF_BLUETOOTH, SOCK_DGRAM, 0, EAFNOSUPPORT },
	{ AF_IEEE80211, SOCK_DGRAM, 0, EAFNOSUPPORT }
};

static void
epoch_cleanup(void)
{
	rtems_status_code sc;

	sc = rtems_task_wake_after(CK_EPOCH_LENGTH);
	assert(sc == RTEMS_SUCCESSFUL);
}

static void
init_addr(struct sockaddr_in *addr)
{
	int ok;

	memset(addr, 0, sizeof(*addr));
	addr->sin_family = AF_INET;
	addr->sin_port = htons(1234);
	ok = inet_aton("127.0.0.1", &addr->sin_addr);
	assert(ok != 0);
}

static void
init_msg(struct msghdr *msg, struct sockaddr_in *addr, struct iovec *iov,
    void *buf, size_t n)
{
	memset(msg, 0, sizeof(*msg));
	memset(iov, 0, sizeof(*iov));
	iov->iov_base = buf;
	iov->iov_len = n;
	msg->msg_name = addr;
	msg->msg_namelen = sizeof(*addr);
	msg->msg_iov = iov;
	msg->msg_iovlen = 1;
}

static void
no_mem_task(rtems_task_argument arg)
{
	const no_mem_test *self = (const no_mem_test *) arg;
	rtems_status_code sc;
	void *greedy;

	assert(rtems_configuration_get_unified_work_area());

	greedy = rtems_workspace_greedy_allocate(NULL, 0);
	(*self->body)(self->fd);
	rtems_workspace_greedy_free(greedy);

	sc = rtems_event_transient_send(self->master_task);
	assert(sc == RTEMS_SUCCESSFUL);

	sc = rtems_task_suspend(RTEMS_SELF);
	assert(sc == RTEMS_SUCCESSFUL);
}

static void
do_no_mem_test(no_mem_test_body body, int fd)
{
	no_mem_test test = {
		.body = body,
		.fd = fd,
		.master_task = rtems_task_self()
	};
	rtems_status_code sc;
	rtems_id id;
	rtems_resource_snapshot snapshot;

	rtems_resource_snapshot_take(&snapshot);

	sc = rtems_task_create(
		rtems_build_name('N', 'M', 'E', 'M'),
		RTEMS_MINIMUM_PRIORITY,
		RTEMS_MINIMUM_STACK_SIZE,
		RTEMS_DEFAULT_MODES,
		RTEMS_FLOATING_POINT,
		&id
	);
	assert(sc == RTEMS_SUCCESSFUL);

	sc = rtems_task_start(id, no_mem_task, (rtems_task_argument) &test);
	assert(sc == RTEMS_SUCCESSFUL);

	sc = rtems_event_transient_receive(RTEMS_WAIT, RTEMS_NO_TIMEOUT);
	assert(sc == RTEMS_SUCCESSFUL);

	sc = rtems_task_delete(id);
	assert(sc == RTEMS_SUCCESSFUL);

	assert(rtems_resource_snapshot_check(&snapshot));
}

static void
test_socket(const socket_test *st)
{
	int sd;
	int rv;

	errno = 0;
	sd = socket(st->domain, st->type, st->protocol);

	if (sd < 0) {
		epoch_cleanup();
		sd = socket(st->domain, st->type, st->protocol);
	}

	if (st->expect_errno == 0) {
		assert(sd >= 0);

		rv = close(sd);
		assert(rv == 0);
	} else {
		assert(st->expect_errno == errno);
	}
}

static void
test_sockets(void)
{
	size_t n = sizeof(socket_tests) / sizeof(socket_tests[0]);
	size_t i;

	puts("Test Sockets");

	for (i = 0; i < n; ++i) {
		const socket_test *st = &socket_tests[i];

		test_socket(st);
	}

	puts("test sockets and check resources");
	epoch_cleanup();

	for (i = 0; i < n; ++i) {
		const socket_test *st = &socket_tests[i];
		rtems_resource_snapshot snapshot;

		rtems_resource_snapshot_take(&snapshot);

		test_socket(st);

		epoch_cleanup();
		assert(rtems_resource_snapshot_check(&snapshot));
	}
}

static void
test_socket_unsupported_ops(void)
{
	rtems_resource_snapshot snapshot;
	int sd;
	int rv;
	off_t off;

	puts("test socket unsupported ops");

	rtems_resource_snapshot_take(&snapshot);

	sd = socket(PF_INET, SOCK_DGRAM, 0);
	assert(sd >= 0);

	errno = 0;
	off = lseek(sd, 0, SEEK_CUR);
	assert(off == -1);
	assert(errno == ESPIPE);

	errno = 0;
	rv = ftruncate(sd, 0);
	assert(rv == -1);
	assert(errno == EINVAL);

	errno = 0;
	rv = fsync(sd);
	assert(rv == -1);
	assert(errno == EINVAL);

	errno = 0;
	rv = fdatasync(sd);
	assert(rv == -1);
	assert(errno == EINVAL);

	rv = close(sd);
	assert(rv == 0);

	epoch_cleanup();
	assert(rtems_resource_snapshot_check(&snapshot));
}

static void
no_mem_socket_fstat(int fd)
{
	struct stat st;
	int rv;

	rv = fstat(fd, &st);
	assert(rv == -1);
	assert(errno == ENOMEM);
}

static void
no_mem_socket_shutdown(int fd)
{
	int rv;

	rv = shutdown(fd, SHUT_RDWR);
	assert(rv == -1);
	assert(errno == ENOMEM);
}

static void
test_socket_fstat_and_shutdown(void)
{
	mode_t canrecv = S_IRUSR | S_IRGRP | S_IROTH;
	mode_t cansend = S_IWUSR | S_IWGRP | S_IWOTH;
	rtems_resource_snapshot snapshot;
	struct sockaddr_in addr;
	struct stat st;
	int sd;
	int rv;

	puts("test socket fstat and shutdown");

	rtems_resource_snapshot_take(&snapshot);

	init_addr(&addr);

	sd = socket(PF_INET, SOCK_DGRAM, 0);
	assert(sd >= 0);

	do_no_mem_test(no_mem_socket_fstat, sd);

	rv = fstat(sd, &st);
	assert(rv == 0);
	assert(st.st_mode == (S_IFSOCK | canrecv | cansend));

	rv = connect(sd, (struct sockaddr *) &addr, sizeof(addr));
	assert(rv == 0);

	rv = shutdown(sd, SHUT_RD);
	assert(rv == 0);

	rv = fstat(sd, &st);
	assert(rv == 0);
	assert(st.st_mode == (S_IFSOCK | cansend));

	rv = shutdown(sd, SHUT_WR);
	assert(rv == 0);

	rv = fstat(sd, &st);
	assert(rv == 0);
	assert(st.st_mode == S_IFSOCK);

	errno = 0;
	rv = shutdown(sd, ~SHUT_RDWR);
	assert(rv == -1);
	assert(errno == EINVAL);

	rv = close(sd);
	assert(rv == 0);

	sd = socket(PF_INET, SOCK_DGRAM, 0);
	assert(sd >= 0);

	rv = connect(sd, (struct sockaddr *) &addr, sizeof(addr));
	assert(rv == 0);

	epoch_cleanup();
	do_no_mem_test(no_mem_socket_shutdown, sd);

	rv = close(sd);
	assert(rv == 0);

	errno = 0;
	rv = shutdown(sd, SHUT_RDWR);
	assert(rv == -1);
	assert(errno == EBADF);

	errno = 0;
	rv = shutdown(0, SHUT_RDWR);
	assert(rv == -1);
	assert(errno == ENOTSOCK);

	epoch_cleanup();
	assert(rtems_resource_snapshot_check(&snapshot));
}

static void
no_mem_socket_ioctl(int fd)
{
	int rv;
	int data;

	errno = 0;
	rv = ioctl(fd, FIONREAD, &data);
	assert(rv == -1);
	assert(errno == ENOMEM);
}

static void
test_socket_ioctl(void)
{
	rtems_resource_snapshot snapshot;
	int sd;
	int rv;
	int data;

	puts("test socket ioctl");

	rtems_resource_snapshot_take(&snapshot);

	sd = socket(PF_INET, SOCK_DGRAM, 0);
	assert(sd >= 0);

	do_no_mem_test(no_mem_socket_ioctl, sd);

	errno = 0;
	rv = ioctl(sd, 0xffffffff);
	assert(rv == -1);
	assert(errno == EADDRNOTAVAIL);

	data = -1;
	rv = ioctl(sd, FIONREAD, &data);
	assert(rv == 0);
	assert(data == 0);

	rv = close(sd);
	assert(rv == 0);

	errno = 0;
	rv = ioctl(sd, 0);
	assert(rv == -1);
	assert(errno == EBADF);

	epoch_cleanup();
	assert(rtems_resource_snapshot_check(&snapshot));
}

static void
test_socket_sockatmark(void)
{
	rtems_resource_snapshot snapshot;
	int sd;
	int rv;

	puts("test socket sockatmark");

	rtems_resource_snapshot_take(&snapshot);

	sd = socket(PF_INET, SOCK_DGRAM, 0);
	assert(sd >= 0);

	rv = sockatmark(sd);
	assert(rv == 0);

	rv = close(sd);
	assert(rv == 0);

	errno = 0;
	rv = sockatmark(sd);
	assert(rv == -1);
	assert(errno == EBADF);

	epoch_cleanup();
	assert(rtems_resource_snapshot_check(&snapshot));
}

static void
no_mem_socket_bind(int fd)
{
	struct sockaddr_in addr;
	int rv;

	errno = 0;
	rv = bind(fd, (const struct sockaddr *) &addr, sizeof(addr));
	assert(rv == -1);
	assert(errno == ENOMEM);
}

static void
test_socket_bind(void)
{
	rtems_resource_snapshot snapshot;
	struct sockaddr_in addr;
	int sd;
	int rv;

	puts("test socket bind");

	rtems_resource_snapshot_take(&snapshot);

	init_addr(&addr);

	sd = socket(PF_INET, SOCK_DGRAM, 0);
	assert(sd >= 0);

	do_no_mem_test(no_mem_socket_bind, sd);

	errno = 0;
	rv = bind(sd, (const struct sockaddr *) &addr, SOCK_MAXADDRLEN + 1);
	assert(rv == -1);
	assert(errno == ENAMETOOLONG);

	errno = 0;
	rv = bind(sd, (const struct sockaddr *) &addr, 0);
	assert(rv == -1);
	assert(errno == EINVAL);

	errno = 0;
	rv = bind(sd, (const struct sockaddr *) &addr, sizeof(addr));
	assert(rv == -1);
	assert(errno == EADDRNOTAVAIL);

	rv = close(sd);
	assert(rv == 0);

	errno = 0;
	rv = bind(sd, (struct sockaddr *) &addr, sizeof(addr));
	assert(rv == -1);
	assert(errno == EBADF);

	epoch_cleanup();
	assert(rtems_resource_snapshot_check(&snapshot));
}

static void
no_mem_socket_connect(int fd)
{
	struct sockaddr_in addr;
	int rv;

	errno = 0;
	rv = connect(fd, (const struct sockaddr *) &addr, sizeof(addr));
	assert(rv == -1);
	assert(errno == ENOMEM);
}

static void
test_socket_connect(void)
{
	rtems_resource_snapshot snapshot;
	struct sockaddr_in addr;
	int sd;
	int rv;

	puts("test socket connect");

	rtems_resource_snapshot_take(&snapshot);

	init_addr(&addr);

	sd = socket(PF_INET, SOCK_DGRAM, 0);
	assert(sd >= 0);

	do_no_mem_test(no_mem_socket_connect, sd);

	errno = 0;
	rv = connect(sd, (const struct sockaddr *) &addr, SOCK_MAXADDRLEN + 1);
	assert(rv == -1);
	assert(errno == ENAMETOOLONG);

	errno = 0;
	rv = connect(sd, (const struct sockaddr *) &addr, 0);
	assert(rv == -1);
	assert(errno == EINVAL);

	errno = 0;
	rv = connect(sd, (const struct sockaddr *) &addr, sizeof(addr));
	assert(rv == -1);
	assert(errno == ENETUNREACH);

	rv = close(sd);
	assert(rv == 0);

	errno = 0;
	rv = connect(sd, (struct sockaddr *) &addr, sizeof(addr));
	assert(rv == -1);
	assert(errno == EBADF);

	epoch_cleanup();
	assert(rtems_resource_snapshot_check(&snapshot));
}

static void
no_mem_socket_listen(int fd)
{
	int rv;

	errno = 0;
	rv = listen(fd, 0);
	assert(rv == -1);
	assert(errno == ENOMEM);
}

static void
test_socket_listen(void)
{
	rtems_resource_snapshot snapshot;
	int sd;
	int rv;

	puts("test socket listen");

	rtems_resource_snapshot_take(&snapshot);

	sd = socket(PF_INET, SOCK_DGRAM, 0);
	assert(sd >= 0);

	do_no_mem_test(no_mem_socket_listen, sd);

	errno = 0;
	rv = listen(sd, 0);
	assert(rv == -1);
	assert(errno == EOPNOTSUPP);

	rv = close(sd);
	assert(rv == 0);

	errno = 0;
	rv = listen(sd, 0);
	assert(rv == -1);
	assert(errno == EBADF);

	sd = socket(PF_INET, SOCK_STREAM, 0);
	assert(sd >= 0);

	rv = close(sd);
	assert(rv == 0);

	epoch_cleanup();
	assert(rtems_resource_snapshot_check(&snapshot));
}

static void
no_mem_socket_accept(int fd)
{
	struct sockaddr_in addr;
	socklen_t addr_len;
	int rv;

	errno = 0;
	addr_len = sizeof(addr);
	rv = accept(fd, (struct sockaddr *) &addr, &addr_len);
	assert(rv == -1);
	assert(errno == ENOMEM);
}

static void
test_socket_accept(void)
{
	rtems_resource_snapshot snapshot;
	struct sockaddr_in addr;
	socklen_t addr_len;
	int sd;
	int ad;
	int rv;

	puts("test socket accept");

	rtems_resource_snapshot_take(&snapshot);

	sd = socket(PF_INET, SOCK_STREAM, 0);
	assert(sd >= 0);

	do_no_mem_test(no_mem_socket_accept, sd);

	errno = 0;
	addr_len = sizeof(addr);
	ad = accept(sd, (struct sockaddr *) &addr, &addr_len);
	assert(ad == -1);
	assert(errno == EINVAL);

	rv = close(sd);
	assert(rv == 0);

	errno = 0;
	addr_len = sizeof(addr);
	ad = accept(sd, (struct sockaddr *) &addr, &addr_len);
	assert(ad == -1);
	assert(errno == EBADF);

	epoch_cleanup();
	assert(rtems_resource_snapshot_check(&snapshot));
}

static void
no_mem_socket_getsockopt_and_setsockopt(int fd)
{
	int rv;
	int optval;
	socklen_t optlen;

	errno = 0;
	optlen = sizeof(optval);
	rv = getsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &optval, &optlen);
	assert(rv == -1);
	assert(errno == ENOMEM);

	errno = 0;
	optlen = sizeof(optval);
	rv = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &optval, optlen);
	assert(rv == -1);
	assert(errno == ENOMEM);
}

static void
test_socket_getsockopt_and_setsockopt(void)
{
	rtems_resource_snapshot snapshot;
	int optval;
	socklen_t optlen;
	int sd;
	int rv;

	puts("test socket getsockopt and setsockopt");

	rtems_resource_snapshot_take(&snapshot);

	sd = socket(PF_INET, SOCK_STREAM, 0);
	assert(sd >= 0);

	do_no_mem_test(no_mem_socket_getsockopt_and_setsockopt, sd);

	errno = 0;
	optlen = sizeof(optval);
	rv = getsockopt(sd, SOL_SOCKET, -1, &optval, &optlen);
	assert(rv == -1);
	assert(errno == ENOPROTOOPT);

	errno = 0;
	optlen = sizeof(optval);
	rv = setsockopt(sd, SOL_SOCKET, -1, &optval, optlen);
	assert(rv == -1);
	assert(errno == ENOPROTOOPT);

	errno = 0;
	optlen = sizeof(optval);
	rv = getsockopt(sd, -1, SO_REUSEADDR, &optval, &optlen);
	assert(rv == -1);
	assert(errno == EINVAL);

	errno = 0;
	optlen = sizeof(optval);
	rv = setsockopt(sd, -1, SO_REUSEADDR, &optval, optlen);
	assert(rv == -1);
	assert(errno == EINVAL);

	optval = -1;
	optlen = sizeof(optval);
	rv = getsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &optval, &optlen);
	assert(rv == 0);
	assert(optval == 0);
	assert(optlen == sizeof(optval));

	optval = 1;
	optlen = sizeof(optval);
	rv = setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &optval, optlen);
	assert(rv == 0);

	optval = 0;
	optlen = sizeof(optval);
	rv = getsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &optval, &optlen);
	assert(rv == 0);
	assert(optval != 0);
	assert(optlen == sizeof(optval));

	rv = close(sd);
	assert(rv == 0);

	errno = 0;
	optlen = sizeof(optval);
	rv = getsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &optval, &optlen);
	assert(rv == -1);
	assert(errno == EBADF);

	errno = 0;
	optval = 0;
	optlen = sizeof(optval);
	rv = setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &optval, optlen);
	assert(rv == -1);
	assert(errno == EBADF);

	epoch_cleanup();
	assert(rtems_resource_snapshot_check(&snapshot));
}

static void
no_mem_socket_getpeername(int fd)
{
	struct sockaddr_in addr;
	socklen_t addr_len;
	int rv;

	errno = 0;
	addr_len = sizeof(addr);
	rv = getpeername(fd, (struct sockaddr *) &addr, &addr_len);
	assert(rv == -1);
	assert(errno == ENOMEM);
}

static void
test_socket_getpeername(void)
{
	rtems_resource_snapshot snapshot;
	struct sockaddr_in addr;
	socklen_t addr_len;
	int sd;
	int rv;

	puts("test socket getpeername");

	rtems_resource_snapshot_take(&snapshot);

	sd = socket(PF_INET, SOCK_STREAM, 0);
	assert(sd >= 0);

	do_no_mem_test(no_mem_socket_getpeername, sd);

	errno = 0;
	addr_len = sizeof(addr);
	rv = getpeername(sd, (struct sockaddr *) &addr, &addr_len);
	assert(rv == -1);
	assert(errno == ENOTCONN);

	rv = close(sd);
	assert(rv == 0);

	errno = 0;
	addr_len = sizeof(addr);
	rv = getpeername(sd, (struct sockaddr *) &addr, &addr_len);
	assert(rv == -1);
	assert(errno == EBADF);

	epoch_cleanup();
	assert(rtems_resource_snapshot_check(&snapshot));
}

static void
no_mem_socket_getsockname(int fd)
{
	struct sockaddr_in addr;
	socklen_t addr_len;
	int rv;

	errno = 0;
	addr_len = sizeof(addr);
	rv = getsockname(fd, (struct sockaddr *) &addr, &addr_len);
	assert(rv == -1);
	assert(errno == ENOMEM);
}

static void
test_socket_getsockname(void)
{
	rtems_resource_snapshot snapshot;
	struct sockaddr_in addr;
	struct sockaddr_in expected_addr;
	socklen_t addr_len;
	int sd;
	int rv;

	puts("test socket getsockname");

	rtems_resource_snapshot_take(&snapshot);

	sd = socket(PF_INET, SOCK_STREAM, 0);
	assert(sd >= 0);

	do_no_mem_test(no_mem_socket_getsockname, sd);

	memset(&addr, 0xff, sizeof(addr));
	addr_len = sizeof(addr);
	rv = getsockname(sd, (struct sockaddr *) &addr, &addr_len);
	assert(rv == 0);
	memset(&expected_addr, 0, sizeof(expected_addr));
	expected_addr.sin_len = sizeof(expected_addr);
	expected_addr.sin_family = AF_INET;
	assert(memcmp(&addr, &expected_addr, sizeof(addr)) == 0);

	rv = close(sd);
	assert(rv == 0);

	errno = 0;
	addr_len = sizeof(addr);
	rv = getsockname(sd, (struct sockaddr *) &addr, &addr_len);
	assert(rv == -1);
	assert(errno == EBADF);

	epoch_cleanup();
	assert(rtems_resource_snapshot_check(&snapshot));
}

static void
no_mem_socket_read_and_write(int fd)
{
	ssize_t n;
	char buf[1];

	errno = 0;
	n = read(fd, &buf[0], sizeof(buf));
	assert(n == -1);
	assert(errno == ENOMEM);

	errno = 0;
	n = write(fd, &buf[0], sizeof(buf));
	assert(n == -1);
	assert(errno == ENOMEM);
}

static void
test_socket_read_and_write(void)
{
	rtems_resource_snapshot snapshot;
	int sd;
	int rv;
	ssize_t n;
	char buf[1];
	int enable = 1;

	puts("test socket read and write");

	rtems_resource_snapshot_take(&snapshot);

	sd = socket(PF_INET, SOCK_DGRAM, 0);
	assert(sd >= 0);

	rv = ioctl(sd, FIONBIO, &enable);
	assert(rv == 0);

	do_no_mem_test(no_mem_socket_read_and_write, sd);

	errno = 0;
	n = read(sd, &buf[0], sizeof(buf));
	assert(n == -1);
	assert(errno == EAGAIN);

	errno = 0;
	n = write(sd, &buf[0], sizeof(buf));
	assert(n == -1);
	assert(errno == EDESTADDRREQ);

	rv = close(sd);
	assert(rv == 0);

	errno = 0;
	n = read(sd, &buf[0], sizeof(buf));
	assert(n == -1);
	assert(errno == EBADF);

	errno = 0;
	n = write(sd, &buf[0], sizeof(buf));
	assert(n == -1);
	assert(errno == EBADF);

	epoch_cleanup();
	assert(rtems_resource_snapshot_check(&snapshot));
}

static void
no_mem_socket_send_and_sendto_and_sendmsg(int fd)
{
	struct sockaddr_in addr;
	struct iovec iov;
	struct msghdr msg;
	ssize_t n;
	char buf[1];

	init_addr(&addr);
	init_msg(&msg, &addr, &iov, &buf[0], sizeof(buf));

	errno = 0;
	n = send(fd, &buf[0], sizeof(buf), 0);
	assert(n == -1);
	assert(errno == ENOMEM);

	errno = 0;
	n = sendto(fd, &buf[0], sizeof(buf), 0,
	    (const struct sockaddr *) &addr, sizeof(addr));
	assert(n == -1);
	assert(errno == ENOMEM);

	errno = 0;
	n = sendmsg(fd, &msg, 0);
	assert(n == -1);
	assert(errno == ENOMEM);
}

static void
test_socket_send_and_sendto_and_sendmsg(void)
{
	rtems_resource_snapshot snapshot;
	struct sockaddr_in addr;
	struct iovec iov;
	struct msghdr msg;
	int sd;
	int rv;
	ssize_t n;
	char buf[1];

	puts("test socket send, sendto and sendmsg");

	rtems_resource_snapshot_take(&snapshot);

	init_addr(&addr);
	init_msg(&msg, &addr, &iov, &buf[0], sizeof(buf));

	sd = socket(PF_INET, SOCK_DGRAM, 0);
	assert(sd >= 0);

	do_no_mem_test(no_mem_socket_send_and_sendto_and_sendmsg, sd);

	errno = 0;
	n = send(sd, &buf[0], sizeof(buf), 0);
	assert(n == -1);
	assert(errno == EDESTADDRREQ);

	errno = 0;
	n = sendto(sd, &buf[0], sizeof(buf), 0,
	    (const struct sockaddr *) &addr, sizeof(addr));
	assert(n == -1);
	assert(errno == ENETUNREACH);

	errno = 0;
	n = sendmsg(sd, &msg, 0);
	assert(n == -1);
	assert(errno == ENETUNREACH);

	rv = close(sd);
	assert(rv == 0);

	errno = 0;
	n = send(sd, &buf[0], sizeof(buf), 0);
	assert(n == -1);
	assert(errno == EBADF);

	errno = 0;
	n = sendto(sd, &buf[0], sizeof(buf), 0,
	    (const struct sockaddr *) &addr, sizeof(addr));
	assert(n == -1);
	assert(errno == EBADF);

	errno = 0;
	n = sendmsg(sd, &msg, 0);
	assert(n == -1);
	assert(errno == EBADF);

	epoch_cleanup();
	assert(rtems_resource_snapshot_check(&snapshot));
}

static void
no_mem_socket_recv_and_recvfrom_and_recvmsg(int fd)
{
	struct sockaddr_in addr;
	socklen_t addr_len;
	struct iovec iov;
	struct msghdr msg;
	ssize_t n;
	char buf[1];

	init_addr(&addr);
	init_msg(&msg, &addr, &iov, &buf[0], sizeof(buf));

	errno = 0;
	n = recv(fd, &buf[0], sizeof(buf), 0);
	assert(n == -1);
	assert(errno == ENOMEM);

	errno = 0;
	addr_len = sizeof(addr);
	n = recvfrom(fd, &buf[0], sizeof(buf), 0,
	    (struct sockaddr *) &addr, &addr_len);
	assert(n == -1);
	assert(errno == ENOMEM);

	errno = 0;
	n = recvmsg(fd, &msg, 0);
	assert(n == -1);
	assert(errno == ENOMEM);
}

static void
test_socket_recv_and_recvfrom_and_recvmsg(void)
{
	rtems_resource_snapshot snapshot;
	struct sockaddr_in addr;
	socklen_t addr_len;
	struct iovec iov;
	struct msghdr msg;
	int sd;
	int rv;
	ssize_t n;
	char buf[1];
	int enable = 1;

	puts("test socket recv, recvfrom and recvmsg");

	rtems_resource_snapshot_take(&snapshot);

	init_addr(&addr);
	init_msg(&msg, &addr, &iov, &buf[0], sizeof(buf));

	sd = socket(PF_INET, SOCK_DGRAM, 0);
	assert(sd >= 0);

	rv = ioctl(sd, FIONBIO, &enable);
	assert(rv == 0);

	do_no_mem_test(no_mem_socket_recv_and_recvfrom_and_recvmsg, sd);

	errno = 0;
	n = recv(sd, &buf[0], sizeof(buf), 0);
	assert(n == -1);
	assert(errno == EAGAIN);

	errno = 0;
	addr_len = sizeof(addr);
	n = recvfrom(sd, &buf[0], sizeof(buf), 0,
	    (struct sockaddr *) &addr, &addr_len);
	assert(n == -1);
	assert(errno == EAGAIN);

	errno = 0;
	n = recvmsg(sd, &msg, 0);
	assert(n == -1);
	assert(errno == EAGAIN);

	rv = close(sd);
	assert(rv == 0);

	errno = 0;
	n = recv(sd, &buf[0], sizeof(buf), 0);
	assert(n == -1);
	assert(errno == EBADF);

	errno = 0;
	addr_len = sizeof(addr);
	n = recvfrom(sd, &buf[0], sizeof(buf), 0,
	    (struct sockaddr *) &addr, &addr_len);
	assert(n == -1);
	assert(errno == EBADF);

	errno = 0;
	n = recvmsg(sd, &msg, 0);
	assert(n == -1);
	assert(errno == EBADF);

	epoch_cleanup();
	assert(rtems_resource_snapshot_check(&snapshot));
}

static void
no_mem_socket_select(int fd)
{
	struct fd_set set;
	int nfds;
	int rv;

	FD_ZERO(&set);
	FD_SET(fd, &set);
	nfds = fd + 1;

	errno = 0;
	rv = select(nfds, &set, NULL, NULL, NULL);
	assert(rv == -1);
	assert(errno == ENOMEM);
}

static void
test_socket_select(void)
{
	rtems_resource_snapshot snapshot;
	struct fd_set set;
	int nfds;
	int sd;
	int rv;

	puts("test socket select");

	sd = socket(PF_INET, SOCK_DGRAM, 0);
	assert(sd >= 0);

	rv = close(sd);
	assert(rv == 0);

	FD_ZERO(&set);
	FD_SET(sd, &set);
	nfds = sd + 1;

	errno = 0;
	rv = select(nfds, &set, NULL, NULL, NULL);
	assert(rv == -1);
	assert(errno == EBADF);

	epoch_cleanup();
	rtems_resource_snapshot_take(&snapshot);

	sd = socket(PF_INET, SOCK_DGRAM, 0);
	assert(sd >= 0);

	do_no_mem_test(no_mem_socket_select, sd);

	FD_ZERO(&set);
	nfds = -1;

	errno = 0;
	rv = select(nfds, &set, NULL, NULL, NULL);
	assert(rv == -1);
	assert(errno == EINVAL);

	rv = close(sd);
	assert(rv == 0);

	FD_ZERO(&set);
	FD_SET(sd, &set);
	nfds = sd + 1;

	errno = 0;
	rv = select(nfds, &set, NULL, NULL, NULL);
	assert(rv == -1);
	assert(errno == EBADF);

	epoch_cleanup();
	assert(rtems_resource_snapshot_check(&snapshot));
}

static void
no_mem_socket_pselect(int fd)
{
	struct fd_set set;
	int nfds;
	int rv;

	FD_ZERO(&set);
	FD_SET(fd, &set);
	nfds = fd + 1;

	errno = 0;
	rv = pselect(nfds, &set, NULL, NULL, NULL, NULL);
	assert(rv == -1);
	assert(errno == ENOMEM);
}

static void
test_socket_pselect(void)
{
	rtems_resource_snapshot snapshot;
	struct fd_set set;
	int nfds;
	int sd;
	int rv;

	puts("test socket pselect");

	sd = socket(PF_INET, SOCK_DGRAM, 0);
	assert(sd >= 0);

	rv = close(sd);
	assert(rv == 0);

	FD_ZERO(&set);
	FD_SET(sd, &set);
	nfds = sd + 1;

	errno = 0;
	rv = pselect(nfds, &set, NULL, NULL, NULL, NULL);
	assert(rv == -1);
	assert(errno == EBADF);

	epoch_cleanup();
	rtems_resource_snapshot_take(&snapshot);

	sd = socket(PF_INET, SOCK_DGRAM, 0);
	assert(sd >= 0);

	do_no_mem_test(no_mem_socket_pselect, sd);

	FD_ZERO(&set);
	nfds = -1;

	errno = 0;
	rv = pselect(nfds, &set, NULL, NULL, NULL, NULL);
	assert(rv == -1);
	assert(errno == EINVAL);

	rv = close(sd);
	assert(rv == 0);

	FD_ZERO(&set);
	FD_SET(sd, &set);
	nfds = sd + 1;

	errno = 0;
	rv = pselect(nfds, &set, NULL, NULL, NULL, NULL);
	assert(rv == -1);
	assert(errno == EBADF);

	epoch_cleanup();
	assert(rtems_resource_snapshot_check(&snapshot));
}

static void
no_mem_socket_poll(int fd)
{
	struct pollfd pfd;
	int timeout = -1;
	int rv;

	pfd.fd = fd;
	pfd.events = POLLIN;
	pfd.revents = 0;

	errno = 0;
	rv = poll(&pfd, 1, timeout);
	assert(rv == -1);
	assert(errno == ENOMEM);
}

static void
test_socket_poll(void)
{
	rtems_resource_snapshot snapshot;
	struct pollfd pfd;
	int timeout = -1;
	int sd;
	int rv;

	puts("test socket poll");

	sd = socket(PF_INET, SOCK_DGRAM, 0);
	assert(sd >= 0);

	rv = close(sd);
	assert(rv == 0);

	pfd.fd = sd;
	pfd.events = POLLIN;
	pfd.revents = 0;

	rv = poll(&pfd, 1, timeout);
	assert(rv == 1);
	assert(pfd.revents == POLLNVAL);

	epoch_cleanup();
	rtems_resource_snapshot_take(&snapshot);

	sd = socket(PF_INET, SOCK_DGRAM, 0);
	assert(sd >= 0);

	do_no_mem_test(no_mem_socket_poll, sd);

	pfd.fd = sd;
	pfd.events = POLLIN;
	pfd.revents = 0;

	errno = 0;
	rv = poll(NULL, UINT_MAX, timeout);
	assert(rv == -1);
	assert(errno == EINVAL);

	rv = close(sd);
	assert(rv == 0);

	pfd.fd = sd;
	pfd.events = POLLIN;
	pfd.revents = 0;

	rv = poll(&pfd, 1, timeout);
	assert(rv == 1);
	assert(pfd.revents == POLLNVAL);

	epoch_cleanup();
	assert(rtems_resource_snapshot_check(&snapshot));
}
static void
no_mem_socket_pair(int fd)
{
	int sd[2];
	int rv;

	errno = 0;
	rv = socketpair(PF_UNIX, SOCK_DGRAM, 0, &sd[0]);
	assert(rv == -1);
	assert(errno == ENOMEM);
}

static void
test_socket_pair(void)
{
	rtems_resource_snapshot snapshot;
	int sd[2];
	int rv;
	char in[] = { 'x' };
	char out[] = { 'o' };
	ssize_t n;

	puts("test socket pair");

	rtems_resource_snapshot_take(&snapshot);

	rv = socketpair(PF_UNIX, SOCK_DGRAM, 0, &sd[0]);
	assert(rv == 0);

	n = write(sd[0], &out[0], sizeof(out));
	assert(n == (ssize_t)sizeof(out));

	n = read(sd[1], &in[0], sizeof(in));
	assert(n == (ssize_t)sizeof(in));

	assert(memcmp(&in[0], &out[0], sizeof(in)) == 0);

	rv = close(sd[0]);
	assert(rv == 0);

	rv = close(sd[1]);
	assert(rv == 0);

	do_no_mem_test(no_mem_socket_pair, -1);

	epoch_cleanup();
	assert(rtems_resource_snapshot_check(&snapshot));
}

static void
test_kqueue_unsupported_ops(void)
{
	rtems_resource_snapshot snapshot;
	int kq;
	int rv;
	off_t off;
	ssize_t n;
	char buf[1];

	puts("test kqueue unsupported ops");

	rtems_resource_snapshot_take(&snapshot);

	kq = kqueue();
	assert(kq >= 0);

	errno = 0;
	n = read(kq, &buf[0], sizeof(buf));
	assert(n == -1);
	assert(errno == EOPNOTSUPP);

	errno = 0;
	n = write(kq, &buf[0], sizeof(buf));
	assert(n == -1);
	assert(errno == EOPNOTSUPP);

	errno = 0;
	rv = ioctl(kq, 0);
	assert(rv == -1);
	assert(errno == ENOTTY);

	errno = 0;
	off = lseek(kq, 0, SEEK_CUR);
	assert(off == -1);
	assert(errno == ESPIPE);

	errno = 0;
	rv = ftruncate(kq, 0);
	assert(rv == -1);
	assert(errno == EINVAL);

	errno = 0;
	rv = fsync(kq);
	assert(rv == -1);
	assert(errno == EINVAL);

	errno = 0;
	rv = fdatasync(kq);
	assert(rv == -1);
	assert(errno == EINVAL);

	rv = close(kq);
	assert(rv == 0);

	assert(rtems_resource_snapshot_check(&snapshot));
}

static void
no_mem_kqueue_fstat(int fd)
{
	struct stat st;
	int rv;

	rv = fstat(fd, &st);
	assert(rv == -1);
	assert(errno == ENOMEM);
}

static void
test_kqueue_fstat(void)
{
	static const struct stat expected_st = {
		.st_mode = S_IFIFO
	};

	rtems_resource_snapshot snapshot;
	struct stat st;
	int kq;
	int rv;

	puts("test kqueue fstat");

	rtems_resource_snapshot_take(&snapshot);

	kq = kqueue();
	assert(kq >= 0);

	do_no_mem_test(no_mem_kqueue_fstat, kq);

	rv = fstat(kq, &st);
	assert(rv == 0);
	assert(memcmp(&expected_st, &st, sizeof(st)) == 0);

	rv = close(kq);
	assert(rv == 0);

	assert(rtems_resource_snapshot_check(&snapshot));
}

static void
no_mem_kqueue_kevent(int fd)
{
	int rv;

	errno = 0;
	rv = kevent(fd, NULL, 0, NULL, 0, NULL);
	assert(rv == -1);
	assert(errno == ENOMEM);
}

static void
test_kqueue_kevent(void)
{
	rtems_resource_snapshot snapshot;
	int kq;
	int rv;

	puts("test kqueue kevent");

	rtems_resource_snapshot_take(&snapshot);

	kq = kqueue();
	assert(kq >= 0);

	do_no_mem_test(no_mem_kqueue_kevent, kq);

	rv = kevent(kq, NULL, 0, NULL, 0, NULL);
	assert(rv == 0);

	rv = close(kq);
	assert(rv == 0);

	errno = 0;
	rv = kevent(kq, NULL, 0, NULL, 0, NULL);
	assert(rv == -1);
	assert(errno == EBADF);

	errno = 0;
	rv = kevent(0, NULL, 0, NULL, 0, NULL);
	assert(rv == -1);
	assert(errno == EBADF);

	assert(rtems_resource_snapshot_check(&snapshot));
}

static void
test_syslog(void)
{
	puts("test syslog");

	errno = 0;
	syslog(LOG_ERR, "%m");
	syslog(LOG_ERR, "b%m");
	syslog(LOG_ERR, "%me");
	errno = ENXIO;
	syslog(LOG_ERR, "<%m><%m><%m>");
	syslog(LOG_INFO, "%m%m%m%m%m%m%m%m%m%m%m%m%m%m");
	syslog(LOG_EMERG, "emerg");
	syslog(LOG_ALERT, "alert");
	syslog(LOG_CRIT, "crit");
	syslog(LOG_ERR, "err");
	syslog(LOG_WARNING, "warning");
	syslog(LOG_NOTICE, "notice");
	syslog(LOG_INFO, "info");
	syslog(LOG_DEBUG, "debug");
}

static void
test_setgethostname(void)
{
	int rv;
	char out[] = { 'f', 'o', 'o', 'b', 'a', 'r' };
	char in[] = { '?', '?', '?', '?', '?', '?', '?' };

	puts("test set/get hostname");

	rv = sethostname(&out[0], sizeof(out));
	assert(rv == 0);

	rv = gethostname(&in[0], sizeof(in));
	assert(rv == 0);

	assert(memcmp(&in[0], &out[0], sizeof(in) - 1) == 0);
	assert(in[sizeof(in) - 1] == '\0');
}

static void
test_main(void)
{

	/*
	 * Stop interferences of uma_timeout() which may need some dynamic
	 * memory.  This could disturb the no memory tests.
	 */
	rtems_uma_drain_timeout();

	/* Must be first test to ensure resource checks work */
	test_sockets();

	test_socket_unsupported_ops();
	test_socket_ioctl();
	test_socket_sockatmark();
	test_socket_bind();
	test_socket_connect();
	test_socket_listen();
	test_socket_accept();
	test_socket_getsockopt_and_setsockopt();
	test_socket_getpeername();
	test_socket_getsockname();
	test_socket_read_and_write();
	test_socket_send_and_sendto_and_sendmsg();
	test_socket_recv_and_recvfrom_and_recvmsg();
	test_socket_select();
	test_socket_pselect();
	test_socket_poll();
	test_socket_pair();

	test_kqueue_unsupported_ops();
	test_kqueue_fstat();
	test_kqueue_kevent();

	test_syslog();
	test_setgethostname();

	rtems_bsd_ifconfig_lo0();
	epoch_cleanup();
	test_socket_fstat_and_shutdown();

	exit(0);
}

#include <rtems/bsd/test/default-init.h>
