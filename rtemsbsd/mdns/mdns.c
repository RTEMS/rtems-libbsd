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
#include <DNSCommon.h>

#include <sys/select.h>
#include <sys/socket.h>
#include <sys/stat.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <arpa/nameser.h>

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <nsswitch.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <rtems/bsd/util.h>
#include <rtems/mdns.h>

static rtems_id mdns_daemon_id;

static mDNS mDNSStorage;

static mDNS_PlatformSupport PlatformStorage;

typedef struct {
	mDNSu16 rrtype;
	struct hostent *he;
	rtems_id task_id;
} query_context;

static void
query_callback(mDNS *m, DNSQuestion *q, const ResourceRecord *answer,
    QC_result add_record)
{
	query_context *ctx = q->QuestionContext;
	struct hostent *he = ctx->he;
	bool stop = false;
	rtems_id task_id = ctx->task_id;

	(void)m;
	(void)add_record;

	if (ctx->rrtype == kDNSType_A && answer->rrtype == kDNSType_A) {
		const mDNSv4Addr *ipv4 = &answer->rdata->u.ipv4;

		memcpy(he->h_addr_list[0], ipv4, sizeof(*ipv4));
		stop = true;
	} else if (ctx->rrtype == kDNSType_AAAA
	    && answer->rrtype == kDNSType_AAAA) {
		const mDNSv6Addr *ipv6 = &answer->rdata->u.ipv6;

		memcpy(he->h_addr_list[0], ipv6, sizeof(*ipv6));
		stop = true;
	}

	if (stop && task_id != 0) {
		rtems_status_code sc;

		ctx->task_id = 0;

		sc = rtems_event_transient_send(task_id);
		assert(sc == RTEMS_SUCCESSFUL);
	}
}

static bool is_local_name(const char *name)
{
	size_t len = strlen(name);
	const char local[] = "local";
	size_t locallen = sizeof(local) - 1;

	if (len == 0) {
		return (false);
	}

	if (name[len - 1] == '.') {
		--len;
	}

	if (len < locallen) {
		return (false);
	}

	return strncasecmp(name + len - locallen, &local[0], locallen) == 0;
}

static int
mdns_gethostbyname(void *rval, void *cb_data, va_list ap)
{
	const char *name;
	int af;
	char *buffer;
	size_t buflen;
	size_t len;
	size_t namelen;
	int *errnop;
	int *h_errnop;
	struct hostent *hep;
	struct hostent **resultp;
	DNSQuestion q;
	query_context ctx;
	mDNSu8 *qname;
	rtems_status_code sc;

	memset(&q, 0, sizeof(q));
	memset(&ctx, 0, sizeof(ctx));

	name = va_arg(ap, const char *);
	af = va_arg(ap, int);
	hep = va_arg(ap, struct hostent *);
	buffer = va_arg(ap, char *);
	buflen = va_arg(ap, size_t);
	errnop = va_arg(ap, int *);
	h_errnop = va_arg(ap, int *);
	resultp = (struct hostent **)rval;

	*resultp = NULL;

	if (!is_local_name(name)) {
		*h_errnop = NETDB_INTERNAL;
		*errnop = EINVAL;
		return (NS_NOTFOUND);
	}

	hep->h_addrtype = af;
	switch (af) {
	case AF_INET:
		hep->h_length = NS_INADDRSZ;
		ctx.rrtype = kDNSType_A;
		break;
	case AF_INET6:
		hep->h_length = NS_IN6ADDRSZ;
		ctx.rrtype = kDNSType_AAAA;
		break;
	default:
		*h_errnop = NETDB_INTERNAL;
		*errnop = EAFNOSUPPORT;
		return (NS_UNAVAIL);
	}

	len = (char *)ALIGN(buffer) - buffer;
	len += 3 * sizeof(char *);
	len += ALIGN(hep->h_length);
	namelen = strlen(name) + 1;
	len += namelen;

	if (len > buflen) {
		*h_errnop = NETDB_INTERNAL;
		*errnop = ERANGE;
		return (NS_UNAVAIL);
	}

	buffer = (char *)ALIGN(buffer);

	hep->h_aliases = (char **)buffer;
	buffer += sizeof(char *);
	hep->h_aliases[0] = NULL;

	hep->h_addr_list = (char **)buffer;
	buffer += 2 * sizeof(char *);
	hep->h_addr_list[0] = buffer;
	buffer += ALIGN(hep->h_length);
	hep->h_addr_list[1] = NULL;

	hep->h_name = buffer;
	memcpy(buffer, name, namelen);

	qname = MakeDomainNameFromDNSNameString(&q.qname, name);
	if (qname == NULL) {
		*h_errnop = NETDB_INTERNAL;
		*errnop = ERANGE;
		return (NS_UNAVAIL);
	}

	q.TargetPort = MulticastDNSPort;
	q.qtype = kDNSQType_ANY;
	q.qclass = kDNSClass_IN;
	q.ForceMCast = mDNStrue;
	q.ReturnIntermed = mDNStrue;
	q.QuestionCallback = query_callback;
	q.QuestionContext = &ctx;

	ctx.rrtype = kDNSType_A;
	ctx.he = hep;
	ctx.task_id = rtems_task_self();

	mDNS_StartQuery(&mDNSStorage, &q);
	rtems_bsd_force_select_timeout(mdns_daemon_id);

	sc = rtems_event_transient_receive(RTEMS_WAIT,
	    10 * rtems_clock_get_ticks_per_second());

	mDNS_StopQuery(&mDNSStorage, &q);

	if (sc != RTEMS_SUCCESSFUL) {
		*h_errnop = NETDB_INTERNAL;
		*errnop = ETIMEDOUT;
		return (NS_NOTFOUND);
	}

	*resultp = hep;
	return (NS_SUCCESS);
}

static ns_mtab mdns_mtab[] = {
	{
		.database = NSDB_HOSTS,
		.name = "gethostbyname2_r",
		.method = mdns_gethostbyname
	}
};

static void
mdns_daemon(rtems_task_argument arg)
{
	while (true) {
		struct timeval timeout = { .tv_sec = 0x1, .tv_usec = 0 };
		sigset_t signals;
		mDNSBool got_something;

		mDNSPosixRunEventLoopOnce(&mDNSStorage, &timeout, &signals,
		    &got_something);
	}
}

static void
truncate_at_first_dot(domainlabel *name)
{
	int c = name->c[0];
	int n = 0;

	while (n < c && name->c[n + 1] != '.') {
		++n;
	}

	name->c[0] = n;
}

static int
mdns_sethostname(const char *hostname)
{
	mDNS *m = &mDNSStorage;

	mDNS_Lock(m);

	MakeDomainLabelFromLiteralString(&m->hostlabel, hostname);
	truncate_at_first_dot(&m->hostlabel);

	mDNS_Unlock(m);

	mDNS_SetFQDN(m);

	rtems_bsd_force_select_timeout(mdns_daemon_id);

	return (0);
}

static int
mdns_gethostname(char *hostname, size_t size)
{
	mDNS *m = &mDNSStorage;

	if (size < MAX_ESCAPED_DOMAIN_LABEL) {
		errno = ERANGE;

		return (-1);
	}

	mDNS_Lock(m);

	ConvertDomainLabelToCString(&m->hostlabel, hostname);

	mDNS_Unlock(m);

	return (0);
}

rtems_status_code
rtems_mdns_initialize(rtems_task_priority daemon_priority,
    CacheEntity *rrcachestorage, mDNSu32 rrcachesize)
{
	mStatus status;
	int rv;
	int fd;
	rtems_status_code sc;

	status = mDNS_Init(&mDNSStorage, &PlatformStorage, rrcachestorage,
	    rrcachesize, mDNS_Init_AdvertiseLocalAddresses,
	    mDNS_Init_NoInitCallback, mDNS_Init_NoInitCallbackContext);
	if (status != mStatus_NoError) {
		return (RTEMS_UNSATISFIED);
	}

	sc = rtems_task_create(rtems_build_name('m', 'D', 'N', 'S'),
	    daemon_priority, 16 * 1024, RTEMS_DEFAULT_MODES,
	    RTEMS_DEFAULT_ATTRIBUTES, &mdns_daemon_id);
	if (sc != RTEMS_SUCCESSFUL) {
		return (RTEMS_UNSATISFIED);
	}

	sc = rtems_task_start(mdns_daemon_id, mdns_daemon, 0);
	if (sc != RTEMS_SUCCESSFUL) {
		return (RTEMS_UNSATISFIED);
	}

	rv = rtems_nss_register_module("mdns", &mdns_mtab[0],
	    RTEMS_ARRAY_SIZE(mdns_mtab));
	if (rv != 0) {
		return (RTEMS_UNSATISFIED);
	}

	fd = open(_PATH_NS_CONF, O_WRONLY | O_CREAT | O_EXCL,
	    S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if (fd >= 0) {
		static const char nsconf[] = "hosts: files mdns dns\n";
		ssize_t n;

		n = write(fd, &nsconf[0], sizeof(nsconf) - 1);

		rv = close(fd);
		assert(rv == 0);

		if (n != (ssize_t) (sizeof(nsconf) - 1)) {
			return (RTEMS_UNSATISFIED);
		}
	}

	rtems_mdns_sethostname_handler = mdns_sethostname;
	rtems_mdns_gethostname_handler = mdns_gethostname;

	return (RTEMS_SUCCESSFUL);
}

mDNS *
rtems_mdns_get_instance(void)
{
	return (&mDNSStorage);
}
