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

#include <mDNSEmbeddedAPI.h>
#include <mDNSPosix.h>

#include <sys/select.h>
#include <sys/socket.h>

#include <netinet/in.h>

#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define TEST_NAME "LIBBSD FOOBAR SERVER"
#define TEST_STATE_USER_INPUT 1

#define FOOBAR_PORT_BEGIN 10815

#define FOOBAR_PORT_END 20815

static mDNS mDNSStorage;

static mDNS_PlatformSupport PlatformStorage;

static void
foobar_callback(mDNS *m, ServiceRecordSet *srs, mStatus status)
{
	const mDNSu8 *name = srs->RR_SRV.resrec.name->c;

	switch (status) {
	case mStatus_NoError:
		printf("foobar server: name registered: %s\n", name);
		break;
	case mStatus_NameConflict:
		printf("foobar server: name conflict: %s\n", name);
		status = mDNS_RenameAndReregisterService(m, srs, mDNSNULL);
		assert(status == mStatus_NoError);
		break;
	case mStatus_MemFree:
		printf("foobar server: free: %s\n", name);
		free(srs);
		break;
	default:
		printf("foobar server: unexpected status: %s\n", name);
		break;
	}
}

static ServiceRecordSet *
foobar_register(mDNSu16 port)
{
	ServiceRecordSet *srs;
	mStatus status;
	domainlabel name;
	domainname type;
	domainname domain;

	srs = calloc(1, sizeof(*srs));
	assert(srs != NULL);

	MakeDomainLabelFromLiteralString(&name, "foobar");
	MakeDomainNameFromDNSNameString(&type, "_foobar._tcp");
	MakeDomainNameFromDNSNameString(&domain, "local.");

	status = mDNS_RegisterService(&mDNSStorage, srs, &name, &type, &domain,
	    NULL, mDNSOpaque16fromIntVal(port), NULL, 0, NULL, 0,
	    mDNSInterface_Any, foobar_callback, srs, 0);
	assert(status == mStatus_NoError);

	return srs;
}

static void
foobar_deregister(ServiceRecordSet *srs)
{
	mStatus status;

	status = mDNS_DeregisterService(&mDNSStorage, srs);
	assert(status == mStatus_NoError);
}

static void *
foobar_thread(void *arg)
{
	static const char foobar[] = "FooBar!";

	mDNSu16 port = FOOBAR_PORT_BEGIN;

	while (1) {
		ServiceRecordSet *srs;
		struct sockaddr_in addr;
		struct sockaddr_in addr2;
		socklen_t addr2_len;
		int sd;
		int sd2;
		int rv;
		ssize_t n;

		sd = socket(PF_INET, SOCK_STREAM, 0);
		assert(sd >= 0);

		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		addr.sin_addr.s_addr = htonl(INADDR_ANY);

		rv = bind(sd, (const struct sockaddr *) &addr, sizeof(addr));
		assert(rv == 0);

		rv = listen(sd, 0);
		assert(rv == 0);

		srs = foobar_register(port);

		addr2_len = sizeof(addr2);
		sd2 = accept(sd, (struct sockaddr *) &addr2, &addr2_len);
		assert(sd2 >= 0);

		n = write(sd2, &foobar[0], sizeof(foobar));
		assert(n == (ssize_t) sizeof(foobar));

		foobar_deregister(srs);

		rv = close(sd2);
		assert(rv == 0);

		rv = close(sd);
		assert(rv == 0);

		if (port < FOOBAR_PORT_END) {
			++port;
		} else {
			port = FOOBAR_PORT_BEGIN;
		}
	}

	return NULL;
}

static void
foobar_create_thread(void)
{
	int eno;
	pthread_t t;

	eno = pthread_create(&t, NULL, foobar_thread, NULL);
	assert(eno == 0);
}

static void
test_main(void)
{
	const char name[] = "foobarserver";
	int rv;
	mStatus status;

	rv = sethostname(&name[0], sizeof(name) - 1);
	assert(rv == 0);

	status = mDNS_Init(&mDNSStorage, &PlatformStorage, mDNS_Init_NoCache,
	    mDNS_Init_ZeroCacheSize, mDNS_Init_AdvertiseLocalAddresses,
	    mDNS_Init_NoInitCallback, mDNS_Init_NoInitCallbackContext);
	assert(status == mStatus_NoError);

	foobar_create_thread();

	while (1) {
		struct timeval timeout = { .tv_sec = 0x3fffffff, .tv_usec = 0 };
		sigset_t signals;
		mDNSBool got_something;

		mDNSPosixRunEventLoopOnce(&mDNSStorage, &timeout, &signals, &got_something);
	}
}

#define DEFAULT_NETWORK_DHCPCD_ENABLE
#define DEFAULT_NETWORK_DHCPCD_NO_DHCP_DISCOVERY
#define DEFAULT_NETWORK_SHELL

#include <rtems/bsd/test/default-network-init.h>
