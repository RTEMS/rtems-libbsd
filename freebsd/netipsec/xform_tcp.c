#include <freebsd/machine/rtems-bsd-config.h>

/*	$FreeBSD$ */

/*-
 * Copyright (c) 2003 Bruce M. Simpson <bms@spc.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *   derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* TCP MD5 Signature Option (RFC2385) */
#include <freebsd/local/opt_inet.h>
#include <freebsd/local/opt_inet6.h>

#include <freebsd/sys/param.h>
#include <freebsd/sys/systm.h>
#include <freebsd/sys/mbuf.h>
#include <freebsd/sys/lock.h>
#include <freebsd/sys/socket.h>
#include <freebsd/sys/kernel.h>
#include <freebsd/sys/protosw.h>
#include <freebsd/sys/sysctl.h>

#include <freebsd/netinet/in.h>
#include <freebsd/netinet/in_systm.h>
#include <freebsd/netinet/ip.h>
#include <freebsd/netinet/ip_var.h>
#include <freebsd/netinet/tcp.h>
#include <freebsd/netinet/tcp_var.h>

#include <freebsd/net/route.h>
#include <freebsd/net/vnet.h>

#include <freebsd/netipsec/ipsec.h>
#include <freebsd/netipsec/xform.h>

#ifdef INET6
#include <freebsd/netinet/ip6.h>
#include <freebsd/netipsec/ipsec6.h>
#endif

#include <freebsd/netipsec/key.h>
#include <freebsd/netipsec/key_debug.h>

/*
 * Initialize a TCP-MD5 SA. Called when the SA is being set up.
 *
 * We don't need to set up the tdb prefixed fields, as we don't use the
 * opencrypto code; we just perform a key length check.
 *
 * XXX: Currently we only allow a single 'magic' SPI to be used.
 *
 * This allows per-host granularity without affecting the userland
 * interface, which is a simple socket option toggle switch,
 * TCP_SIGNATURE_ENABLE.
 *
 * To allow per-service granularity requires that we have a means
 * of mapping port to SPI. The mandated way of doing this is to
 * use SPD entries to specify packet flows which get the TCP-MD5
 * treatment, however the code to do this is currently unstable
 * and unsuitable for production use.
 *
 * Therefore we use this compromise in the meantime.
 */
static int
tcpsignature_init(struct secasvar *sav, struct xformsw *xsp)
{
	int keylen;

	if (sav->spi != htonl(TCP_SIG_SPI)) {
		DPRINTF(("%s: SPI must be TCP_SIG_SPI (0x1000)\n",
		    __func__));
		return (EINVAL);
	}
	if (sav->alg_auth != SADB_X_AALG_TCP_MD5) {
		DPRINTF(("%s: unsupported authentication algorithm %u\n",
		    __func__, sav->alg_auth));
		return (EINVAL);
	}
	if (sav->key_auth == NULL) {
		DPRINTF(("%s: no authentication key present\n", __func__));
		return (EINVAL);
	}
	keylen = _KEYLEN(sav->key_auth);
	if ((keylen < TCP_KEYLEN_MIN) || (keylen > TCP_KEYLEN_MAX)) {
		DPRINTF(("%s: invalid key length %u\n", __func__, keylen));
		return (EINVAL);
	}

	return (0);
}

/*
 * Paranoia.
 *
 * Called when the SA is deleted.
 */
static int
tcpsignature_zeroize(struct secasvar *sav)
{

	if (sav->key_auth)
		bzero(sav->key_auth->key_data, _KEYLEN(sav->key_auth));

	sav->tdb_cryptoid = 0;
	sav->tdb_authalgxform = NULL;
	sav->tdb_xform = NULL;

	return (0);
}

/*
 * Verify that an input packet passes authentication.
 * Called from the ipsec layer.
 * We do this from within tcp itself, so this routine is just a stub.
 */
static int
tcpsignature_input(struct mbuf *m, struct secasvar *sav, int skip,
    int protoff)
{

	return (0);
}

/*
 * Prepend the authentication header.
 * Called from the ipsec layer.
 * We do this from within tcp itself, so this routine is just a stub.
 */
static int
tcpsignature_output(struct mbuf *m, struct ipsecrequest *isr,
    struct mbuf **mp, int skip, int protoff)
{

	return (EINVAL);
}

static struct xformsw tcpsignature_xformsw = {
	XF_TCPSIGNATURE,	XFT_AUTH,		"TCPMD5",
	tcpsignature_init,	tcpsignature_zeroize,
	tcpsignature_input,	tcpsignature_output
};

static void
tcpsignature_attach(void)
{

	xform_register(&tcpsignature_xformsw);
}

SYSINIT(tcpsignature_xform_init, SI_SUB_DRIVERS, SI_ORDER_FIRST,
    tcpsignature_attach, NULL);
