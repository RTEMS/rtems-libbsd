/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup NFSClient
 *
 * @brief RTEMS bindings to the NFS client
 *
 */

/*
 * Copyright (C) 2021 Chris Johns <chris@contemporary.software>
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <machine/rtems-bsd-kernel-space.h>
#include <machine/rtems-bsd-libio.h>
#include <machine/rtems-bsd-vfs.h>

#include <sys/buf.h>
#include <sys/errno.h>
#include <sys/fcntl.h>
#include <sys/file.h>
#include <sys/filedesc.h>
#include <sys/kernel.h>
#include <sys/mount.h>
#include <sys/namei.h>
#include <sys/proc.h>
#include <sys/resourcevar.h>
#include <sys/syscallsubr.h>
#include <sys/sysproto.h>
#include <sys/time.h>
#include <sys/vnode.h>

#include <rpc/rpc.h>

#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <rpc/types.h>
#include <rpc/auth.h>

#include <fs/nfsclient/nfs.h>
#include <nfs/nfsproto.h>
#include <nfsclient/nfs.h>
#include <rtems/bsd/rootfs.h>

#include <stdio.h>

SYSINIT_MODULE_REFERENCE(rootfs);
SYSINIT_MODULE_REFERENCE(nfs);

#ifndef RTEMS_DEBUG
#define RTEMS_DEBUG 1
#endif
#ifndef RTEMS_NFSCL_DEBUGLEVEL
#define RTEMS_NFSCL_DEBUGLEVEL 0
#endif

#if RTEMS_NFSCL_DEBUGLEVEL
extern int nfscl_debuglevel;
#endif

/*
 * Map user to kernel or just provide
 */
#define NFS_PROGRAM   NFS_PROG
#define MOUNTPROG     100005
#define MOUNTPROC_MNT 1
#define MNTPATHLEN    1024

#define NFS_FHSIZE  NFSX_V2FH
#define NFS3_FHSIZE NFSX_V3FHMAX

#undef malloc
#undef free

/* Table for af,sotype -> netid conversions. */
static const struct nc_protos {
	const char *netid;
	int af;
	int sotype;
} nc_protos[] = {
	{"udp",		AF_INET,	SOCK_DGRAM},
	{"tcp",		AF_INET,	SOCK_STREAM},
	{"udp6",	AF_INET6,	SOCK_DGRAM},
	{"tcp6",	AF_INET6,	SOCK_STREAM},
	{NULL,		0,		0}
};

struct nfhret {
	u_long		stat;
	long		vers;
	long		auth;
	long		fhsize;
	u_char		nfh[NFS3_FHSIZE];
};

enum mountmode {
	ANY,
	V2,
	V3,
	V4
};

/* Return codes for nfs_tryproto. */
enum tryret {
	TRYRET_SUCCESS,
	TRYRET_TIMEOUT,		/* No response received. */
	TRYRET_REMOTEERR,	/* Error received from remote server. */
	TRYRET_LOCALERR		/* Local failure. */
};

/*
 * Taken from freebsd/sbin/mount_nfs/mount_nfs.c
 */
struct nfs_args {
	struct mntarg *ma;
	char options[256 + 1];
	char hostname[MNAMELEN + 1];
	char dirpath[MNAMELEN + 1];
	char hname[MAXHOSTNAMELEN + 5];
	char pname[MAXHOSTNAMELEN + 5];
	enum mountmode mountmode;
	int opflags;
	int mnttcp_ok;
	int vers;
	int nfsproto;
	char portspec[16];
	int noconn;
	struct sockaddr *addr;
	int addrlen;
	u_char *fh;
	int fhsize;
	int secflavor;
	int got_principal;
	struct addrinfo *ai_nfs;
	char errstr[256];
};
#define OF_NOINET4      4
#define OF_NOINET6      8

/*
 * Options to exclude from the mount
 */
static const char *opts_exclude[] = {
	"nfsv2",
	"nfsv3",
	"nfsv4",
	"port",
	"noinet4",
	"noinet6",
	"vers"
};
#define NUM_OPTS_EXCLUDES (sizeof(opts_exclude) / sizeof(opts_exclude[0]))

static void
nfs_args_defaults(struct nfs_args *args)
{
	memset(args, 0, sizeof(struct nfs_args));
	args->ma = NULL;
	args->mountmode = ANY;
	args->vers = 4;
	args->opflags = 0;
	args->nfsproto = IPPROTO_TCP;
	args->mnttcp_ok = 1;
	args->noconn = 0;
	args->addrlen = 0;
	args->fh = NULL;
	args->fhsize = 0;
	args->secflavor = -1;
	args->got_principal = 0;
}

/* The header for the mount arguments */
struct mntarg {
	struct iovec *v;
	int len;
	int error;
	SLIST_HEAD(, mntaarg)	list;
};

static void
print_mount_args(struct mntarg *ma)
{
	if (RTEMS_DEBUG) {
		int m;
		printf("nfs: mount args: %d\n", ma->len / 2);
		for (m = 0; m < ma->len; m += 2) {
			bool string = true;
			const char *p;
			printf(" %3d %s%c", ma->v[m + 1].iov_len,
			       (char*) ma->v[m].iov_base,
			       ma->v[m + 1].iov_len == 0 ? ' ' : '=');
			p = (const char*) ma->v[m + 1].iov_base;
			while (p != NULL && *p != '\0') {
				if (*p < ' ' || *p > '~') {
					string = false;
					break;
				}
				++p;
			}
			p = (const char*) ma->v[m + 1].iov_base;
			if (p != NULL && string) {
				printf(p);
			} else if (p != NULL) {
				int i;
				for (i = 0; i < ma->v[m + 1].iov_len; ++i) {
					printf("%02x ", (int) *p);
					++p;
				}
			}
			printf("\n");
		}
	}
}

/*
 * Look up a netid based on an address family and socket type.
 * `af' is the address family, and `sotype' is SOCK_DGRAM or SOCK_STREAM.
 *
 * XXX there should be a library function for this.
 */
static const char *
netidbytype(int af, int sotype)
{
	struct nc_protos *p;
	for (p = nc_protos; p->netid != NULL; p++) {
		if (af != p->af || sotype != p->sotype)
			continue;
		return (p->netid);
	}
	return (NULL);
}

static const char *
sec_num_to_name(int flavor)
{
	switch (flavor) {
	case RPCSEC_GSS_KRB5:
		return ("krb5");
	case RPCSEC_GSS_KRB5I:
		return ("krb5i");
	case RPCSEC_GSS_KRB5P:
		return ("krb5p");
	case AUTH_SYS:
		return ("sys");
	}
	return (NULL);
}

/*
 * Catagorise a RPC return status and error into an `enum tryret'
 * return code.
 */
static enum tryret
returncode(enum clnt_stat clntstat, struct rpc_err *rpcerr)
{

	switch (clntstat) {
	case RPC_TIMEDOUT:
		return (TRYRET_TIMEOUT);
	case RPC_PMAPFAILURE:
	case RPC_PROGNOTREGISTERED:
	case RPC_PROGVERSMISMATCH:
	/* XXX, these can be local or remote. */
	case RPC_CANTSEND:
	case RPC_CANTRECV:
		return (TRYRET_REMOTEERR);
	case RPC_SYSTEMERROR:
		switch (rpcerr->re_errno) {
		case ETIMEDOUT:
			return (TRYRET_TIMEOUT);
		case ENOMEM:
			break;
		default:
			return (TRYRET_REMOTEERR);
		}
		/* FALLTHROUGH */
	default:
		break;
	}
	return (TRYRET_LOCALERR);
}

/*
 * Look up a netconfig entry based on a netid, and cache the result so
 * that we don't need to remember to call freenetconfigent().
 *
 * Otherwise it behaves just like getnetconfigent(), so nc_*error()
 * work on failure.
 */
static struct netconfig *
getnetconf_cached(const char *netid)
{
	static struct nc_entry {
		struct netconfig *nconf;
		struct nc_entry *next;
	} *head;
	struct nc_entry *p;
	struct netconfig *nconf;

	for (p = head; p != NULL; p = p->next)
		if (strcmp(netid, p->nconf->nc_netid) == 0)
			return (p->nconf);

	if ((nconf = getnetconfigent(netid)) == NULL)
		return (NULL);
	if ((p = malloc(sizeof(*p))) == NULL)
		err(1, "malloc");
	p->nconf = nconf;
	p->next = head;
	head = p;

	return (p->nconf);
}

/*
 * xdr routines for mount rpc's
 */
static int
xdr_dir(XDR *xdrsp, char *dirp)
{
	return (xdr_string(xdrsp, &dirp, MNTPATHLEN));
}

static int
xdr_fh(XDR *xdrsp, struct nfhret *np)
{
	int i;
	long auth, authcnt, authfnd = 0;

	if (!xdr_u_long(xdrsp, &np->stat))
		return (0);
	if (np->stat)
		return (1);
	switch (np->vers) {
	case 1:
		np->fhsize = NFS_FHSIZE;
		return (xdr_opaque(xdrsp, (caddr_t)np->nfh, NFS_FHSIZE));
	case 3:
		if (!xdr_long(xdrsp, &np->fhsize))
			return (0);
		if (np->fhsize <= 0 || np->fhsize > NFS3_FHSIZE)
			return (0);
		if (!xdr_opaque(xdrsp, (caddr_t)np->nfh, np->fhsize))
			return (0);
		if (!xdr_long(xdrsp, &authcnt))
			return (0);
		for (i = 0; i < authcnt; i++) {
			if (!xdr_long(xdrsp, &auth))
				return (0);
			if (np->auth == -1) {
				np->auth = auth;
				authfnd++;
			} else if (auth == np->auth) {
				authfnd++;
			}
		}
		/*
		 * Some servers, such as DEC's OSF/1 return a nil authenticator
		 * list to indicate RPCAUTH_UNIX.
		 */
		if (authcnt == 0 && np->auth == -1)
			np->auth = AUTH_SYS;
		if (!authfnd && (authcnt > 0 || np->auth != AUTH_SYS))
			np->stat = EAUTH;
		return (1);
	}
	return (0);
}

static int
getnfsargs(char *spec, struct nfs_args *args)
{
	struct addrinfo hints;
	int ecode, speclen, remoteerr, offset, have_bracket = 0;
	char *hostp, *delimp, *errstr;
	size_t len;

	if (*spec == '[' && (delimp = strchr(spec + 1, ']')) != NULL &&
	    *(delimp + 1) == ':') {
		hostp = spec + 1;
		spec = delimp + 2;
		have_bracket = 1;
	} else if ((delimp = strrchr(spec, ':')) != NULL) {
		hostp = spec;
		spec = delimp + 1;
	} else {
	if (RTEMS_DEBUG) {
			printf("nfs: mount: no <host>:<dirpath> nfs-name\n");
		}
		return (-1);
	}
	*delimp = '\0';

	/*
	 * If there has been a trailing slash at mounttime it seems
	 * that some mountd implementations fail to remove the mount
	 * entries from their mountlist while unmounting.
	 */
	for (speclen = strlen(spec);
		speclen > 1 && spec[speclen - 1] == '/';
		speclen--)
		spec[speclen - 1] = '\0';
	if (strlen(hostp) + strlen(spec) + 1 > MNAMELEN) {
		if (RTEMS_DEBUG) {
			printf("nfs: mount: %s:%s: %s", hostp, spec, strerror(ENAMETOOLONG));
		}
		return (-1);
	}
	/* Make both '@' and ':' notations equal */
	if (*hostp != '\0') {
		len = strlen(hostp);
		offset = 0;
		if (have_bracket)
			args->hostname[offset++] = '[';
		memmove(args->hostname + offset, hostp, len);
		if (have_bracket)
			args->hostname[len + offset++] = ']';
		args->hostname[len + offset++] = ':';
		memmove(args->hostname + len + offset, spec, speclen);
		args->hostname[len + speclen + offset] = '\0';
	}

	/*
	 * Handle an internet host address.
	 */
	memset(&hints, 0, sizeof hints);
	hints.ai_flags = AI_NUMERICHOST;
	if (args->nfsproto == IPPROTO_TCP)
		hints.ai_socktype = SOCK_STREAM;
	else if (args->nfsproto == IPPROTO_UDP)
		hints.ai_socktype = SOCK_DGRAM;

	char* portspec = args->portspec[0] == '\0' ? NULL : args->portspec;
	if (getaddrinfo(hostp, portspec, &hints, &args->ai_nfs) != 0) {
		hints.ai_flags = AI_CANONNAME;
		if ((ecode = getaddrinfo(hostp, args->portspec, &hints, &args->ai_nfs))
		    != 0) {
			if (RTEMS_DEBUG) {
				printf("nfs: mount: getaddrinfo: ");
				if (args->portspec[0] == '\0')
					printf("%s: %s\n", hostp, gai_strerror(ecode));
				else
					printf("%s:%s: %s]n", hostp, args->portspec,
						    gai_strerror(ecode));
			}
			return (-1);
		}

		/*
		 * For a Kerberized nfs mount where the "principal"
		 * argument has not been set, add it here.
		 */
		if (args->got_principal == 0 && args->secflavor != AUTH_SYS &&
		    args->ai_nfs->ai_canonname != NULL) {
			snprintf(args->pname, sizeof(args->pname), "nfs@%s",
			    args->ai_nfs->ai_canonname);
		}
	}

	strlcpy(args->hname, hostp, sizeof(args->hname));
	strlcpy(args->dirpath, spec, sizeof(args->dirpath));

	return (0);
}

/*
 * Try to set up the NFS arguments according to the address
 * family, protocol (and possibly port) specified in `ai'.
 *
 * Returns TRYRET_SUCCESS if successful, or:
 *   TRYRET_TIMEOUT		The server did not respond.
 *   TRYRET_REMOTEERR		The server reported an error.
 *   TRYRET_LOCALERR		Local failure.
 *
 * In all error cases, *errstr will be set to a statically-allocated string
 * describing the error.
 */
static enum tryret
nfs_tryproto(struct addrinfo *ai, struct nfs_args *args)
{
	#define errbuf args->errstr
	struct sockaddr_storage nfs_ss;
	struct netbuf nfs_nb;
	struct nfhret nfhret;
	struct timeval try;
	struct rpc_err rpcerr;
	CLIENT *clp;
	struct netconfig *nconf, *nconf_mnt;
	const char *netid, *netid_mnt, *secname;
	int doconnect, nfsvers, mntvers, sotype;
	enum clnt_stat clntstat;
	enum mountmode trymntmode;

	const char *portspec = *args->portspec == '\0' ? NULL : args->portspec;
	const char *hostp = args->hname;
	const char *spec = args->dirpath;

	sotype = 0;
	trymntmode = args->mountmode;
	errbuf[0] = '\0';

	if (args->nfsproto == IPPROTO_TCP)
		sotype = SOCK_STREAM;
	else if (args->nfsproto == IPPROTO_UDP)
		sotype = SOCK_DGRAM;

	if ((netid = netidbytype(ai->ai_family, sotype)) == NULL) {
		snprintf(errbuf, sizeof errbuf,
		    "af %d sotype %d not supported", ai->ai_family, sotype);
		return (TRYRET_LOCALERR);
	}
	if ((nconf = getnetconf_cached(netid)) == NULL) {
		snprintf(errbuf, sizeof errbuf, "%s: %s", netid, nc_sperror());
		return (TRYRET_LOCALERR);
	}
	/* The RPCPROG_MNT netid may be different. */
	if (args->mnttcp_ok) {
		netid_mnt = netid;
		nconf_mnt = nconf;
	} else {
		if ((netid_mnt = netidbytype(ai->ai_family, SOCK_DGRAM))
		     == NULL) {
			snprintf(errbuf, sizeof errbuf,
			    "af %d sotype SOCK_DGRAM not supported",
			     ai->ai_family);
			return (TRYRET_LOCALERR);
		}
		if ((nconf_mnt = getnetconf_cached(netid_mnt)) == NULL) {
			snprintf(errbuf, sizeof errbuf, "%s: %s", netid_mnt,
			    nc_sperror());
			return (TRYRET_LOCALERR);
	}
	}

tryagain:
	if (trymntmode == V4) {
		nfsvers = 4;
		mntvers = 3; /* Workaround for GCC. */
	} else if (trymntmode == V2) {
		nfsvers = 2;
		mntvers = 1;
	} else {
		nfsvers = 3;
		mntvers = 3;
	}

	if (portspec != NULL) {
		/* `ai' contains the complete nfsd sockaddr. */
		nfs_nb.buf = ai->ai_addr;
		nfs_nb.len = nfs_nb.maxlen = ai->ai_addrlen;
	} else {
		/* Ask the remote rpcbind. */
		nfs_nb.buf = &nfs_ss;
		nfs_nb.len = nfs_nb.maxlen = sizeof nfs_ss;

		if (!rpcb_getaddr(NFS_PROGRAM, nfsvers, nconf, &nfs_nb,
		    hostp)) {
			if (rpc_createerr.cf_stat == RPC_PROGVERSMISMATCH &&
			    trymntmode == ANY) {
				trymntmode = V2;
				goto tryagain;
			}
			snprintf(errbuf, sizeof errbuf, "[%s] %s:%s: %s",
			    netid, hostp, spec,
			    clnt_spcreateerror("RPCPROG_NFS"));
			return (returncode(rpc_createerr.cf_stat,
			    &rpc_createerr.cf_error));
		}
	}

	/* Check that the server (nfsd) responds on the port we have chosen. */
	clp = clnt_tli_create(RPC_ANYFD, nconf, &nfs_nb, NFS_PROGRAM, nfsvers,
	    0, 0);
	if (clp == NULL) {
		snprintf(errbuf, sizeof errbuf, "[%s] %s:%s: %s", netid,
		    hostp, spec, clnt_spcreateerror("nfsd: RPCPROG_NFS"));
		return (returncode(rpc_createerr.cf_stat,
		    &rpc_createerr.cf_error));
	}
	if (sotype == SOCK_DGRAM && args->noconn == 0) {
		/*
		 * Use connect(), to match what the kernel does. This
		 * catches cases where the server responds from the
		 * wrong source address.
		 */
		doconnect = 1;
		if (!clnt_control(clp, CLSET_CONNECT, (char *)&doconnect)) {
			clnt_destroy(clp);
			snprintf(errbuf, sizeof errbuf,
			    "[%s] %s:%s: CLSET_CONNECT failed", netid, hostp,
			    spec);
			return (TRYRET_LOCALERR);
		}
	}

	try.tv_sec = 10;
	try.tv_usec = 0;
	clntstat = clnt_call(clp, NFSPROC_NULL, (xdrproc_t)xdr_void, NULL,
			 (xdrproc_t)xdr_void, NULL, try);
	if (clntstat != RPC_SUCCESS) {
		if (clntstat == RPC_PROGVERSMISMATCH && trymntmode == ANY) {
			clnt_destroy(clp);
			trymntmode = V2;
			goto tryagain;
		}
		clnt_geterr(clp, &rpcerr);
		snprintf(errbuf, sizeof errbuf, "[%s] %s:%s: %s", netid,
		    hostp, spec, clnt_sperror(clp, "NFSPROC_NULL"));
		clnt_destroy(clp);
		return (returncode(clntstat, &rpcerr));
	}
	clnt_destroy(clp);

	/*
	 * For NFSv4, there is no mount protocol.
	 */
	if (trymntmode == V4) {
		/*
		 * Store the server address in nfsargsp, making
		 * sure to copy any locally allocated structures.
		 */
		args->addrlen = nfs_nb.len;
		args->addr = malloc(args->addrlen);
		if (args->addr == NULL) {
			snprintf(errbuf, sizeof errbuf, "no memory");
			return (TRYRET_LOCALERR);
		}
		bcopy(nfs_nb.buf, args->addr, args->addrlen);
		args->ma = mount_arg(args->ma, "addr", args->addr, args->addrlen);
		secname = sec_num_to_name(args->secflavor);
		if (secname != NULL) {
			args->ma = mount_arg(args->ma, "sec",
			    __DECONST(void *, secname), (size_t)-1);
		}
		args->ma = mount_arg(args->ma, "nfsv4", NULL, 0);
		args->ma = mount_arg(args->ma, "dirpath", spec, -1);

		return (TRYRET_SUCCESS);
	}

	/* Send the MOUNTPROC_MNT RPC to get the root filehandle. */
	try.tv_sec = 10;
	try.tv_usec = 0;
	clp = clnt_tp_create(hostp, MOUNTPROG, mntvers, nconf_mnt);
	if (clp == NULL) {
		snprintf(errbuf, sizeof errbuf, "[%s] %s:%s: %s", netid_mnt,
		    hostp, spec, clnt_spcreateerror("RPCMNT: clnt_create"));
		return (returncode(rpc_createerr.cf_stat,
		    &rpc_createerr.cf_error));
	}
	clp->cl_auth = authsys_create_default();
	nfhret.auth = args->secflavor;
	nfhret.vers = mntvers;
	clntstat = clnt_call(clp, MOUNTPROC_MNT, (xdrproc_t)xdr_dir, spec,
			 (xdrproc_t)xdr_fh, &nfhret, try);
	auth_destroy(clp->cl_auth);
	if (clntstat != RPC_SUCCESS) {
		if (clntstat == RPC_PROGVERSMISMATCH && trymntmode == ANY) {
			clnt_destroy(clp);
			trymntmode = V2;
			goto tryagain;
		}
		clnt_geterr(clp, &rpcerr);
		snprintf(errbuf, sizeof errbuf, "[%s] %s:%s: %s", netid_mnt,
		    hostp, spec, clnt_sperror(clp, "RPCPROG_MNT"));
		clnt_destroy(clp);
		return (returncode(clntstat, &rpcerr));
	}
	clnt_destroy(clp);

	if (nfhret.stat != 0) {
		snprintf(errbuf, sizeof errbuf, "[%s] %s:%s: %s", netid_mnt,
		    hostp, spec, strerror(nfhret.stat));
		return (TRYRET_REMOTEERR);
	}

	/*
	 * Store the filehandle and server address in nfsargsp, making
	 * sure to copy any locally allocated structures.
	 */
	args->addrlen = nfs_nb.len;
	args->addr = malloc(args->addrlen);
	args->fhsize = nfhret.fhsize;
	args->fh = malloc(args->fhsize);
	if (args->addr == NULL || args->fh == NULL) {
		free(args->addr);
		free(args->fh);
		snprintf(errbuf, sizeof errbuf, "no memory");
		return (TRYRET_LOCALERR);
	}
	bcopy(nfs_nb.buf, args->addr, args->addrlen);
	bcopy(nfhret.nfh, args->fh, args->fhsize);

	args->ma = mount_arg(args->ma, "addr", args->addr, args->addrlen);
	args->ma = mount_arg(args->ma, "fh", args->fh, args->fhsize);
	secname = sec_num_to_name(nfhret.auth);
	if (secname) {
		args->ma = mount_arg(args->ma, "sec",
		    __DECONST(void *, secname), (size_t)-1);
	}
	if (nfsvers == 3)
		args->ma = mount_arg(args->ma, "nfsv3", NULL, 0);

	return (TRYRET_SUCCESS);
}

static int
nfs_trymount(
	rtems_filesystem_mount_table_entry_t *mt_entry,
	struct addrinfo *ai, struct nfs_args *args,
	const  char* fspath, void *data
) {
	struct thread *td = curthread;
		char errmsg[255];
	int error;
	if (td == NULL) {
		if (RTEMS_DEBUG)
			printf("nfs: mount: no current thread\n");
		return ENOMEM;
	}
	args->ma = mount_arg(
		args->ma, "fstype", RTEMS_DECONST(char *, mt_entry->type), -1);
	args->ma = mount_arg(args->ma, "fspath", RTEMS_DECONST(char *, fspath), -1);
	args->ma = mount_arg(
		args->ma, "hostname", RTEMS_DECONST(char *, args->hostname), -1);
	if (mt_entry->writeable) {
		args->ma = mount_arg(args->ma, "rw", NULL, 0);
	} else {
		args->ma = mount_arg(args->ma, "ro", NULL, 0);
	}
	if (data != NULL) {
		char *options = args->options;
		char *opts;
		/*
		 * See `man mount_nfs` and the list of options.
		 */
		strlcpy(options, (const char *)data, sizeof(args->options));
		opts = &options[0];
		while (opts != NULL) {
			char *delimiter = strchr(opts, ',');
			char *opt = opts;
			int s;
			if (delimiter != NULL) {
				*delimiter = '\0';
				opts = delimiter + 1;
			} else {
				opts = NULL;
			}
			delimiter = strchr(opt, '=');
			if (delimiter != NULL) {
				*delimiter = '\0';
			}
			for (s = 0; s < NUM_OPTS_EXCLUDES; ++s) {
				if (strcasecmp(opt, opts_exclude[s]) == 0) {
					break;
				}
			}
			if (s < NUM_OPTS_EXCLUDES) {
				continue;
			}
			if (delimiter != NULL) {
				args->ma = mount_arg(
					args->ma, opt, delimiter + 1, -1);
			} else {
				args->ma = mount_arg(args->ma, opt, NULL, 0);
			}
		}
	}
	memset(errmsg, 0, sizeof(errmsg));
	print_mount_args(args->ma);
	args->ma = mount_arg(args->ma, "errmsg", errmsg, sizeof(errmsg) - 1);
	error = kernel_mount(args->ma, MNT_VERIFIED);
	if (error == 0) {
		struct nameidata nd;
		vhold(rootvnode);
		NDINIT_ATVP(&nd, LOOKUP, NOFOLLOW, UIO_USERSPACE,
		    fspath, rootvnode, td);
		error = namei(&nd);
		if (error == 0) {
			rtems_bsd_libio_loc_set_vnode(
			    &mt_entry->mt_fs_root->location, nd.ni_vp);
			rtems_bsd_vfs_clonenode(
			    &mt_entry->mt_fs_root->location);
			NDFREE(&nd, NDF_NO_VP_RELE);
		} else {
			NDFREE(&nd, 0);
			rtems_bsd_libio_loc_set_vnode(
			    &mt_entry->mt_fs_root->location, NULL);
			rtems_bsd_vfs_freenode(
			    &mt_entry->mt_fs_root->location);
			rtems_bsd_rootfs_rmdir(fspath);
		}
	} else {
		if (RTEMS_DEBUG) {
			if (strlen(errmsg) > 0) {
				printf("nfs: mount: error: %s\n", errmsg);
			}
		}
	}
	return error;
}

int
rtems_nfs_initialize(
    rtems_filesystem_mount_table_entry_t *mt_entry, const void *data)
{
	struct nfs_args args;
	const char *fspath = NULL;
	char *at;
	int error;

	if (RTEMS_DEBUG) {
		printf("nfs: mount: %s -> %s", mt_entry->type, mt_entry->dev,
		    mt_entry->target);
		if (data != NULL) {
			printf(" (%s)", (const char *)data);
		}
		printf("\n");
#if RTEMS_NFSCL_DEBUGLEVEL
		nfscl_debuglevel = RTEMS_NFSCL_DEBUGLEVEL;
#endif
	}

	at = strchr(mt_entry->dev, '@');
	if (at != NULL) {
		if (RTEMS_DEBUG)
			printf(
			    "nfs: mount: user/group name in path not supported\n");
		error = EINVAL;
		goto out;
	}

	nfs_args_defaults(&args);

	if (data != NULL) {
		char options[64];
		size_t opts_len = strnlen((const char *)data, sizeof(options));
		char *opts;
		if (opts_len >= sizeof(options)) {
			if (RTEMS_DEBUG)
				printf(
				    "nfs: mount: options string too long\n");
			error = EINVAL;
			goto out;
		}

		/*
		 * See `man mount_nfs` and the list of options.
		 */
		strlcpy(options, (const char *)data, sizeof(options));
		opts = &options[0];
		while (opts != NULL) {
			char *delimiter = strchr(opts, ',');
			char *opt = opts;
			if (delimiter != NULL) {
				*delimiter = '\0';
				opts = delimiter + 1;
			} else {
				opts = NULL;
			}
			delimiter = strchr(opt, '=');
			if (strcasecmp(opt, "nfsv2") == 0) {
				args.vers = 2;
				args.mountmode = V2;
			} else if (strcasecmp(opt, "nfsv3") == 0) {
				args.vers = 3;
				args.mountmode = V3;
			} else if (strcasecmp(opt, "nfsv4") == 0) {
				args.vers = 4;
				args.mountmode = V4;
				if (args.portspec[0] == '\0')
					strlcpy(args.portspec, "2049", sizeof(args.portspec));
			} else if (strcasecmp(opt, "tcp") == 0) {
				args.nfsproto = IPPROTO_TCP;
			} else if (strcasecmp(opt, "udp") == 0) {
				args.nfsproto = IPPROTO_UDP;
			} else if (strcasecmp(opt, "port") == 0) {
				if (delimiter != NULL) {
					strlcpy(args.portspec, delimiter + 1, sizeof(args.portspec));
				} else {
					error = EINVAL;
					goto out;
				}
			} else if (strcasecmp(opt, "noinet4") == 0) {
				args.opflags |= OF_NOINET4;
			} else if (strcasecmp(opt, "noinet6") == 0) {
				args.opflags |= OF_NOINET6;
			} else if (strcasecmp(opt, "vers") == 0) {
				if (delimiter != NULL) {
					args.vers = 2;
				} else {
					error = EINVAL;
					goto out;
				}
			}
		}
	}

	rtems_bsd_vfs_mount_init(mt_entry);

	fspath = mt_entry->target;
	if (*fspath == '/') {
		++fspath;
	}
	if (strchr(fspath, '/') != 0) {
		error = EINVAL;
		goto out;
	}

	if (getnfsargs(mt_entry->dev, &args) < 0) {
		if (RTEMS_DEBUG)
			printf(
			    "nfs: mount: invalid device path: %s\n", mt_entry->dev);
		error = EINVAL;
		goto out;
	}

	rtems_bsd_libio_loc_set_vnode(&mt_entry->mt_fs_root->location, NULL);
	rtems_bsd_libio_loc_set_vnode_dir(
	    &mt_entry->mt_fs_root->location, NULL);

	/*
	 * Make the mount point in the BSD root file system, mount the NFS
	 * export then find the vnode and hold it. Make sure we find the root
	 * node of the NFS share and the not the root file system's mount node.
	 */
	error = rtems_bsd_rootfs_mkdir(fspath);
	if (error == 0) {
		struct addrinfo *ai;
		enum tryret tryret;
		for (ai = args.ai_nfs; ai != NULL; ai = ai->ai_next) {
			if ((ai->ai_family == AF_INET6) &&
			    (args.opflags & OF_NOINET6))
				continue;
			if ((ai->ai_family == AF_INET) &&
			    (args.opflags & OF_NOINET4))
				continue;
			tryret = nfs_tryproto(ai, &args);
			if (tryret == TRYRET_SUCCESS) {
				error = nfs_trymount(mt_entry, ai, &args, fspath, data);
			if (RTEMS_DEBUG)
					printf("nfs: mount: (%d) %s\n", error, strerror(error));
				break;
			} else {
				error = EIO;
				if (RTEMS_DEBUG)
					printf("nfs: mount: %s\n", args.errstr);
		}
	}
	}

	freeaddrinfo(args.ai_nfs);

	rtems_bsd_libio_loc_set_vnode_dir(
	    &mt_entry->mt_fs_root->location, NULL);

out:
	if (error != 0) {
		if (fspath != NULL) {
			rtems_bsd_rootfs_rmdir(fspath);
		}
	}
	return error;
}
