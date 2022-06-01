/**
 * @file
 *
 * @brief The multicast DNS (mDNS) protocol is tested.
 *
 * To run this test is also required to run foobarserver.
 * The following functions are called: mDNS_StartBrowse(),
 * mDNS_StartResolveService() and mDNS_StopResolveService().
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

#include <sys/select.h>
#include <sys/socket.h>

#include <netinet/in.h>

#include <assert.h>
#include <errno.h>
#include <netdb.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <rtems.h>
#include <rtems/mdns.h>

#define TEST_NAME "LIBBSD FOOBAR CLIENT"
#define TEST_STATE_USER_INPUT 1

static CacheEntity rr_cache[64];

static const char * const qc_results[] = {
	"rmv",
	"add",
	"addnocache",
	"forceresponse",
	"dnssec",
	"nodnssec",
	"suppressed"
};

typedef struct service_info_query {
	struct service_info_query *next;
	ServiceInfoQuery query;
	ServiceInfo info;
} service_info_query;

static service_info_query *service_info_query_list;

static mDNSBool
eval_service_name(const domainname *fqdn, domainlabel *name)
{
	domainname type;
	domainname domain;
	mDNSBool ok = DeconstructServiceName(fqdn, name, &type, &domain);

	if (ok) {
		char name_str[MAX_DOMAIN_LABEL + 1];
		char type_str[MAX_ESCAPED_DOMAIN_NAME];
		char domain_str[MAX_ESCAPED_DOMAIN_NAME];

		ConvertDomainLabelToCString_unescaped(name, name_str);
		ConvertDomainNameToCString(&type, type_str);
		ConvertDomainNameToCString(&domain, domain_str);

		printf("name = '%s', type = '%s', domain = '%s'",
		    name_str, type_str, domain_str);
	} else {
		printf("?");
	}

	return ok;
}

static void
foobar_action(const struct sockaddr_storage *addr)
{
	static const char foobar[] = "FooBar!";

	int sd;
	int rv;

	sd = socket(addr->ss_family, SOCK_STREAM, 0);
	assert(sd >= 0);

	rv = connect(sd, (struct sockaddr *) addr, addr->ss_len);
	if (rv == 0) {
		char buf[sizeof(foobar)];
		ssize_t n;

		n = read(sd, &buf[0], sizeof(buf));
		if (n == (ssize_t) sizeof(buf)) {
			assert(memcmp(&buf[0], &foobar[0], sizeof(buf)) == 0);
			printf("foobar client: read: %s\n", &buf[0]);
		} else if (n >= 0) {
			printf("foobar client: partial read: %zi\n", n);
		} else {
			printf("foobar client: read error: %s\n",
			    strerror(errno));
		}
	} else {
		printf("foobar client: connect error: %s\n", strerror(errno));
	}

	rv = close(sd);
	assert(rv == 0);
}

static void
foobar_service_info(mDNS *const m, ServiceInfoQuery *query)
{
	const ServiceInfo *i = query->info;
	struct sockaddr_storage addr;
	char host[NI_MAXHOST];
	domainlabel name;
	int rv;

	memset(&addr, 0, sizeof(addr));

	if (i->ip.type == mDNSAddrType_IPv4) {
		struct sockaddr_in *s = (struct sockaddr_in *) &addr;

		s->sin_len = sizeof(*s);
		s->sin_family = AF_INET;
		s->sin_port = i->port.NotAnInteger;
		s->sin_addr.s_addr = i->ip.ip.v4.NotAnInteger;
	} else {
		struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *) &addr;

		sin6->sin6_len = sizeof(*sin6);
		sin6->sin6_family = AF_INET6;
		sin6->sin6_port = i->port.NotAnInteger;
		sin6->sin6_addr = *(struct in6_addr *) &i->ip.ip.v6;
	}

	rv = getnameinfo((struct sockaddr *) &addr, addr.ss_len, host,
	    sizeof(host), NULL, 0, NI_NUMERICHOST);
	assert(rv == 0);

	printf("foobar client: service info: ");
	eval_service_name(&i->name, &name);
	printf(", addr = %s, port = %i\n", &host[0], mDNSVal16(i->port));

	foobar_action(&addr);
}

static void
foobar_manage_service(mDNS *m, const ResourceRecord *answer,
    const domainlabel *name, bool add)
{
	if (add) {
		mStatus status;
		service_info_query *siq =
		    calloc(1, sizeof(*siq));

		assert(siq != NULL);

		siq->next = service_info_query_list;
		service_info_query_list = siq;

		siq->info.name = answer->rdata->u.name;
		siq->info.InterfaceID = answer->InterfaceID;

		status = mDNS_StartResolveService(m, &siq->query, &siq->info,
		    foobar_service_info, NULL);
		assert(status == mStatus_NoError);
	} else {
		service_info_query *cur = service_info_query_list;
		service_info_query *prev = cur;

		while (cur != NULL && !SameDomainLabel(&name->c[0],
		    &cur->info.name.c[0])) {
			prev = cur;
			cur = cur->next;
		}

		if (cur != NULL) {
			if (prev != cur) {
				prev->next = cur->next;
			} else {
				service_info_query_list = cur->next;
			}

			mDNS_StopResolveService(m, &cur->query);
			free(cur);
		}
	}
}

static void
foobar_browse(mDNS *m, DNSQuestion *question,
    const ResourceRecord *answer, QC_result res)
{
	(void) question;

	printf("foobar client: %s: ", qc_results[res]);

	if (answer->rrtype == kDNSType_PTR &&
	    (res == QC_rmv || res == QC_add)) {
		domainlabel name;
		mDNSBool ok = eval_service_name(&answer->rdata->u.name, &name);

		if (ok) {
			foobar_manage_service(m, answer, &name, res == QC_add);
		}

		printf("\n");
	}
}

static void
foobar_register(DNSQuestion *question)
{
	mStatus status;
	domainname type;
	domainname domain;

	MakeDomainNameFromDNSNameString(&type, "_foobar._tcp");
	MakeDomainNameFromDNSNameString(&domain, "local.");

	status = mDNS_StartBrowse(rtems_mdns_get_instance(), question, &type,
	    &domain, mDNSNULL, mDNSInterface_Any, 0, mDNSfalse, mDNSfalse,
	    foobar_browse, NULL);
	assert(status == mStatus_NoError);
}

static void
test_main(void)
{
	const char name[] = "foobarclient";
	int rv;
	DNSQuestion question;
	rtems_status_code sc;

	rv = sethostname(&name[0], sizeof(name) - 1);
	assert(rv == 0);

	sc = rtems_mdns_initialize(254, &rr_cache[0], RTEMS_ARRAY_SIZE(rr_cache));
	assert(sc == RTEMS_SUCCESSFUL);

	foobar_register(&question);

	rtems_task_exit();
}

#define DEFAULT_NETWORK_DHCPCD_ENABLE
#define DEFAULT_NETWORK_DHCPCD_NO_DHCP_DISCOVERY
#define DEFAULT_NETWORK_SHELL

#include <rtems/bsd/test/default-network-init.h>
