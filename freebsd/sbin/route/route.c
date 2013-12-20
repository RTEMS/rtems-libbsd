#include <machine/rtems-bsd-user-space.h>

/*
 * Copyright (c) 1983, 1989, 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef lint
static const char copyright[] =
"@(#) Copyright (c) 1983, 1989, 1991, 1993\n\
	The Regents of the University of California.  All rights reserved.\n";
#endif /* not lint */

#ifndef lint
#if 0
static char sccsid[] = "@(#)route.c	8.6 (Berkeley) 4/28/95";
#endif
#endif /* not lint */

#ifdef __rtems__
#define __need_getopt_newlib
#include <getopt.h>
#include <machine/rtems-bsd-program.h>
#include <machine/rtems-bsd-commands.h>
#endif
#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <rtems/bsd/sys/param.h>
#include <sys/file.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/sysctl.h>
#include <rtems/bsd/sys/types.h>
#include <sys/queue.h>

#include <net/if.h>
#include <net/route.h>
#include <net/if_dl.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>
#include <netatalk/at.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <ctype.h>
#include <err.h>
#include <errno.h>
#include <paths.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h>
#include <ifaddrs.h>

static const struct keytab {
	const char	*kt_cp;
	int	kt_i;
} keywords[] = {
#include "keywords.h"
	{0, 0}
};

struct fibl {
	TAILQ_ENTRY(fibl)	fl_next;

	int	fl_num;
	int	fl_error;
	int	fl_errno;
};

struct rt_ctx {
	union	sockunion {
		struct	sockaddr sa;
		struct	sockaddr_in sin;
#ifdef INET6
		struct	sockaddr_in6 sin6;
#endif
		struct	sockaddr_at sat;
		struct	sockaddr_dl sdl;
		struct	sockaddr_inarp sinarp;
		struct	sockaddr_storage ss; /* added to avoid memory overrun */
	} so_dst, so_gate, so_mask, so_genmask, so_ifa, so_ifp;

	int	pid, rtm_addrs;
	int	s;
	int	forcehost, forcenet, nflag, af, qflag, tflag;
	int	verbose, aflen;
	int	locking, lockrest, debugonly;
	struct	rt_metrics rt_metrics;
	u_long  rtm_inits;
	uid_t	uid;
	int	defaultfib;
	int	numfibs;
	char	domain[MAXHOSTNAMELEN + 1];
	int	domain_initialized;
	int	rtm_seq;
	char	rt_line[MAXHOSTNAMELEN + 1];
	char	net_line[MAXHOSTNAMELEN + 1];
	struct {
		struct	rt_msghdr m_rtm;
		char	m_space[512];
	} m_rtmsg;
	TAILQ_HEAD(fibl_head_t, fibl) fibl_head;
};

#ifndef __rtems__
struct rt_ctx rt_ctx;
#endif /* __rtems__ */

typedef union sockunion *sup;

static int	atalk_aton(const char *, struct at_addr *);
static char	*atalk_ntoa(struct at_addr, char [20]);
static void	bprintf(FILE *, int, const char *);
static void	flushroutes(struct rt_ctx *, int argc, char *argv[]);
static int	flushroutes_fib(struct rt_ctx *, int);
static int	getaddr(struct rt_ctx *, int, char *, struct hostent **, int);
static int	keyword(const char *);
static void	inet_makenetandmask(struct rt_ctx *, u_long, struct sockaddr_in *, u_long);
#ifdef INET6
static int inet6_makenetandmask(struct rt_ctx *, struct sockaddr_in6 *, const char *);
#endif
static void	interfaces(struct rt_ctx *);
static void	mask_addr(struct rt_ctx *);
static void	monitor(struct rt_ctx *, int, char *[]);
static const char	*netname(struct rt_ctx *, struct sockaddr *);
static void	newroute(struct rt_ctx *, int, char **);
static int	newroute_fib(struct rt_ctx *, int, char *, int);
static void	pmsg_addrs(struct rt_ctx *, char *, int, size_t);
static void	pmsg_common(struct rt_ctx *, struct rt_msghdr *, size_t);
static int	prefixlen(struct rt_ctx *, const char *);
static void	print_getmsg(struct rt_ctx *, struct rt_msghdr *, int, int);
static void	print_rtmsg(struct rt_ctx *, struct rt_msghdr *, size_t);
static const char	*routename(struct rt_ctx *, struct sockaddr *);
static int	rtmsg(struct rt_ctx *, int, int, int);
static void	set_metric(struct rt_ctx *, char *, int);
static int	set_sofib(struct rt_ctx *, int);
static int	set_procfib(int);
static void	sockaddr(char *, struct sockaddr *);
static void	sodump(sup, const char *);
extern	char *iso_ntoa(void);

static int	fiboptlist_csv(struct rt_ctx *, const char *, struct fibl_head_t *);
static int	fiboptlist_range(struct rt_ctx *, const char *, struct fibl_head_t *);

static void usage(const char *) __dead2;

void
usage(const char *cp)
{
	if (cp != NULL)
		warnx("bad keyword: %s", cp);
	(void) fprintf(stderr,
	    "usage: route [-dnqtv] command [[modifiers] args]\n");
	exit(EX_USAGE);
	/* NOTREACHED */
}

#ifdef __rtems__
static int main(int argc, char **argv, struct rt_ctx *c);

struct main_ctx {
	int argc;
	char **argv;
	struct rt_ctx *c;
};

static int
call_main(void *ctx)
{
	const struct main_ctx *mc = ctx;

	return main(mc->argc, mc->argv, mc->c);
}

int rtems_bsd_command_route(int argc, char *argv[])
{
	struct rt_ctx *c;
	int exit_code;

	c = calloc(1, sizeof(*c));
	if (c != NULL) {
		struct main_ctx mc;
		struct fibl *fl;
		struct fibl *tfl;

		mc.argc = argc;
		mc.argv = argv;
		mc.c = c;

		c->s = -1;
		c->aflen = sizeof(struct sockaddr_in);
		TAILQ_INIT(&c->fibl_head);

		exit_code = rtems_bsd_program_call("route", call_main, &mc);

		close(c->s);

		TAILQ_FOREACH_SAFE(fl, &c->fibl_head, fl_next, tfl) {
			free(fl);
		}

		free(c);
	} else {
		exit_code = EXIT_FAILURE;
	}

	return exit_code;
}

int
main(int argc, char **argv, struct rt_ctx *c)
{
#else /* __rtems__ */
int
main(int argc, char **argv)
{
	struct rt_ctx *c;
#endif /* __rtems__ */
	int ch;
	size_t len;
#ifdef __rtems__
	struct getopt_data getopt_data;
	memset(&getopt_data, 0, sizeof(getopt_data));
#define optind getopt_data.optind
#define optarg getopt_data.optarg
#define opterr getopt_data.opterr
#define optopt getopt_data.optopt
#define getopt(argc, argv, opt) getopt_r(argc, argv, "+" opt, &getopt_data)
#endif /* __rtems__ */

#ifndef __rtems__
	c = &rt_ctx;
	c->aflen = sizeof (struct sockaddr_in);
#endif /* __rtems__ */

	if (argc < 2)
		usage(NULL);

	while ((ch = getopt(argc, argv, "nqdtv")) != -1)
		switch(ch) {
		case 'n':
			c->nflag = 1;
			break;
		case 'q':
			c->qflag = 1;
			break;
		case 'v':
			c->verbose = 1;
			break;
		case 't':
			c->tflag = 1;
			break;
		case 'd':
			c->debugonly = 1;
			break;
		case '?':
		default:
			usage(NULL);
		}
	argc -= optind;
	argv += optind;

	c->pid = getpid();
	c->uid = geteuid();
	if (c->tflag)
		c->s = open(_PATH_DEVNULL, O_WRONLY, 0);
	else
		c->s = socket(PF_ROUTE, SOCK_RAW, 0);
	if (c->s < 0)
		err(EX_OSERR, "socket");

	len = sizeof(c->numfibs);
	if (sysctlbyname("net.fibs", (void *)&c->numfibs, &len, NULL, 0) == -1)
		c->numfibs = -1;

	len = sizeof(c->defaultfib);
	if (c->numfibs != -1 &&
	    sysctlbyname("net.my_fibnum", (void *)&c->defaultfib, &len, NULL,
		0) == -1)
		c->defaultfib = -1;

	if (*argv != NULL)
		switch (keyword(*argv)) {
		case K_GET:
		case K_SHOW:
			c->uid = 0;
			/* FALLTHROUGH */

		case K_CHANGE:
		case K_ADD:
		case K_DEL:
		case K_DELETE:
			newroute(c, argc, argv);
			/* NOTREACHED */

		case K_MONITOR:
			monitor(c, argc, argv);
			/* NOTREACHED */

		case K_FLUSH:
			flushroutes(c, argc, argv);
			exit(0);
			/* NOTREACHED */
		}
	usage(*argv);
	/* NOTREACHED */
}

static int
set_sofib(struct rt_ctx *c, int fib)
{

	if (fib < 0)
		return (0);
	return (setsockopt(c->s, SOL_SOCKET, SO_SETFIB, (void *)&fib,
	    sizeof(fib)));
}

static int
set_procfib(int fib)
{
	
	if (fib < 0)
		return (0);
	return (setfib(fib));
}

static int
fiboptlist_range(struct rt_ctx *c, const char *arg, struct fibl_head_t *flh)
{
	struct fibl *fl;
	char *str0, *str, *token, *endptr;
	int fib[2], i, error;

	str0 = str = strdup(arg);
	error = 0;
	i = 0;
	while ((token = strsep(&str, "-")) != NULL) {
		switch (i) {
		case 0:
		case 1:
			errno = 0;
			fib[i] = strtol(token, &endptr, 0);
			if (errno == 0) {
				if (*endptr != '\0' ||
				    fib[i] < 0 ||
				    (c->numfibs != -1 && fib[i] > c->numfibs - 1)) 
					errno = EINVAL;
			}
			if (errno)
				error = 1;
			break;
		default:
			error = 1;
		}
		if (error)
			goto fiboptlist_range_ret;
		i++;
	}
	if (fib[0] >= fib[1]) {
		error = 1;
		goto fiboptlist_range_ret;
	}
	for (i = fib[0]; i <= fib[1]; i++) {
		fl = calloc(1, sizeof(*fl));
		if (fl == NULL) {
			error = 1;
			goto fiboptlist_range_ret;
		}
		fl->fl_num = i;
		TAILQ_INSERT_TAIL(flh, fl, fl_next);
	}
fiboptlist_range_ret:
	free(str0);
	return (error);
}

#define	ALLSTRLEN	64
static int
fiboptlist_csv(struct rt_ctx *c, const char *arg, struct fibl_head_t *flh)
{
	struct fibl *fl;
	char *str0, *str, *token, *endptr;
	int fib, error;

	if (strcmp("all", arg) == 0) {
		str = calloc(1, ALLSTRLEN);
		if (str == NULL) {
			error = 1;
			goto fiboptlist_csv_ret;
		}
		if (c->numfibs > 1)
			snprintf(str, ALLSTRLEN - 1, "%d-%d", 0, c->numfibs - 1);
		else
			snprintf(str, ALLSTRLEN - 1, "%d", 0);
	} else if (strcmp("default", arg) == 0) {
		str0 = str = calloc(1, ALLSTRLEN);
		if (str == NULL) {
			error = 1;
			goto fiboptlist_csv_ret;
		}
		snprintf(str, ALLSTRLEN - 1, "%d", c->defaultfib);
	} else
		str0 = str = strdup(arg);

	error = 0;
	while ((token = strsep(&str, ",")) != NULL) {
		if (*token != '-' && strchr(token, '-') != NULL) {
			error = fiboptlist_range(c, token, flh);
			if (error)
				goto fiboptlist_csv_ret;
		} else {
			errno = 0;
			fib = strtol(token, &endptr, 0);
			if (errno == 0) {
				if (*endptr != '\0' ||
				    fib < 0 ||
				    (c->numfibs != -1 && fib > c->numfibs - 1))
					errno = EINVAL;
			}
			if (errno) {
				error = 1;
				goto fiboptlist_csv_ret;
			}
			fl = calloc(1, sizeof(*fl));
			if (fl == NULL) {
				error = 1;
				goto fiboptlist_csv_ret;
			}
			fl->fl_num = fib;
			TAILQ_INSERT_TAIL(flh, fl, fl_next);
		}
	}
fiboptlist_csv_ret:
	free(str0);
	return (error);
}

/*
 * Purge all entries in the routing tables not
 * associated with network interfaces.
 */
static void
flushroutes(struct rt_ctx *c, int argc, char *argv[])
{
	struct fibl *fl;
	int error;

	if (c->uid != 0 && !c->debugonly) {
		errx(EX_NOPERM, "must be root to alter routing table");
	}
	shutdown(c->s, SHUT_RD); /* Don't want to read back our messages */

	TAILQ_INIT(&c->fibl_head);
	while (argc > 1) {
		argc--;
		argv++;
		if (**argv != '-')
			usage(*argv);
		switch (keyword(*argv + 1)) {
		case K_INET:
			c->af = AF_INET;
			break;
#ifdef INET6
		case K_INET6:
			c->af = AF_INET6;
			break;
#endif
		case K_ATALK:
			c->af = AF_APPLETALK;
			break;
		case K_LINK:
			c->af = AF_LINK;
			break;
		case K_FIB:
			if (!--argc)
				usage(*argv);
			error = fiboptlist_csv(c, *++argv, &c->fibl_head);
			if (error)
				errx(EX_USAGE, "invalid fib number: %s", *argv);
			break;
		default:
			usage(*argv);
		}
	}
	if (TAILQ_EMPTY(&c->fibl_head)) {
		error = fiboptlist_csv(c, "default", &c->fibl_head);
		if (error)
			errx(EX_OSERR, "fiboptlist_csv failed.");
	}
	TAILQ_FOREACH(fl, &c->fibl_head, fl_next)
		flushroutes_fib(c, fl->fl_num);
}

static int
flushroutes_fib(struct rt_ctx *c, int fib)
{
	struct rt_msghdr *rtm;
	size_t needed;
	char *buf, *next, *lim;
	int mib[6], rlen, seqno, count = 0;
	int error;

	error = set_sofib(c, fib);
	error += set_procfib(fib);
	if (error) {
		warn("fib number %d is ignored", fib);
		return (error);
	}

retry:
	mib[0] = CTL_NET;
	mib[1] = PF_ROUTE;
	mib[2] = 0;		/* protocol */
	mib[3] = 0;		/* wildcard address family */
	mib[4] = NET_RT_DUMP;
	mib[5] = 0;		/* no flags */
	if (sysctl(mib, 6, NULL, &needed, NULL, 0) < 0)
		err(EX_OSERR, "route-sysctl-estimate");
	if ((buf = malloc(needed)) == NULL && needed != 0)
		errx(EX_OSERR, "malloc failed");
	if (sysctl(mib, 6, buf, &needed, NULL, 0) < 0) {
		if (errno == ENOMEM && count++ < 10) {
			warnx("Routing table grew, retrying");
			sleep(1);
			free(buf);
			goto retry;
		}
		err(EX_OSERR, "route-sysctl-get");
	}
	lim = buf + needed;
	if (c->verbose)
		(void) printf("Examining routing table from sysctl\n");
	seqno = 0;		/* ??? */
	for (next = buf; next < lim; next += rtm->rtm_msglen) {
		rtm = (struct rt_msghdr *)next;
		if (c->verbose)
			print_rtmsg(c, rtm, rtm->rtm_msglen);
		if ((rtm->rtm_flags & RTF_GATEWAY) == 0)
			continue;
		if (c->af != 0) {
			struct sockaddr *sa = (struct sockaddr *)(rtm + 1);

			if (sa->sa_family != c->af)
				continue;
		}
		if (c->debugonly)
			continue;
		rtm->rtm_type = RTM_DELETE;
		rtm->rtm_seq = seqno;
		rlen = write(c->s, next, rtm->rtm_msglen);
		if (rlen < 0 && errno == EPERM)
			err(1, "write to routing socket");
		if (rlen < (int)rtm->rtm_msglen) {
			warn("write to routing socket");
			(void) printf("got only %d for rlen\n", rlen);
			free(buf);
			goto retry;
			break;
		}
		seqno++;
		if (c->qflag)
			continue;
		if (c->verbose)
			print_rtmsg(c, rtm, rlen);
		else {
			struct sockaddr *sa = (struct sockaddr *)(rtm + 1);

			printf("%-20.20s ", rtm->rtm_flags & RTF_HOST ?
			    routename(c, sa) : netname(c, sa));
			sa = (struct sockaddr *)(SA_SIZE(sa) + (char *)sa);
			printf("%-20.20s ", routename(c, sa));
			if (fib >= 0)
				printf("-fib %-3d ", fib);
			printf("done\n");
		}
	}
	free(buf);
	return (error);
}

static const char *
routename(struct rt_ctx *c, struct sockaddr *sa)
{
	const char *cp;
	char atalk_buf[20];
	struct hostent *hp;
	int n;

	if (c->domain_initialized) {
		c->domain_initialized = 1;
		if (gethostname(c->domain, MAXHOSTNAMELEN) == 0 &&
		    (cp = strchr(c->domain, '.'))) {
			c->domain[MAXHOSTNAMELEN] = '\0';
			(void) strcpy(c->domain, cp + 1);
		} else
			c->domain[0] = 0;
	}

	if (sa->sa_len == 0)
		strcpy(c->rt_line, "default");
	else switch (sa->sa_family) {

	case AF_INET:
	    {	struct in_addr in;
		in = ((struct sockaddr_in *)sa)->sin_addr;

		cp = NULL;
		if (in.s_addr == INADDR_ANY || sa->sa_len < 4)
			cp = "default";
		if (cp == NULL && !c->nflag) {
			hp = gethostbyaddr((char *)&in, sizeof (struct in_addr),
				AF_INET);
			if (hp != NULL) {
				char *cptr;
				cptr = strchr(hp->h_name, '.');
				if (cptr != NULL &&
				    strcmp(cptr + 1, c->domain) == 0)
					*cptr = '\0';
				cp = hp->h_name;
			}
		}
		if (cp != NULL) {
			strncpy(c->rt_line, cp, sizeof(c->rt_line) - 1);
			c->rt_line[sizeof(c->rt_line) - 1] = '\0';
		} else
			(void) sprintf(c->rt_line, "%s", inet_ntoa(in));
		break;
	    }

#ifdef INET6
	case AF_INET6:
	{
		struct sockaddr_in6 sin6; /* use static var for safety */
		int niflags = 0;

		memset(&sin6, 0, sizeof(sin6));
		memcpy(&sin6, sa, sa->sa_len);
		sin6.sin6_len = sizeof(struct sockaddr_in6);
		sin6.sin6_family = AF_INET6;
#ifdef __KAME__
		if (sa->sa_len == sizeof(struct sockaddr_in6) &&
		    (IN6_IS_ADDR_LINKLOCAL(&sin6.sin6_addr) ||
		     IN6_IS_ADDR_MC_LINKLOCAL(&sin6.sin6_addr) ||
		     IN6_IS_ADDR_MC_NODELOCAL(&sin6.sin6_addr)) &&
		    sin6.sin6_scope_id == 0) {
			sin6.sin6_scope_id =
			    ntohs(*(u_int16_t *)&sin6.sin6_addr.s6_addr[2]);
			sin6.sin6_addr.s6_addr[2] = 0;
			sin6.sin6_addr.s6_addr[3] = 0;
		}
#endif
		if (c->nflag)
			niflags |= NI_NUMERICHOST;
		if (getnameinfo((struct sockaddr *)&sin6, sin6.sin6_len,
		    c->rt_line, sizeof(c->rt_line), NULL, 0, niflags) != 0)
			strncpy(c->rt_line, "invalid", sizeof(c->rt_line));

		return(c->rt_line);
	}
#endif

	case AF_APPLETALK:
		(void) snprintf(c->rt_line, sizeof(c->rt_line), "atalk %s",
			atalk_ntoa(((struct sockaddr_at *)sa)->sat_addr, atalk_buf));
		break;

	case AF_LINK:
		return (link_ntoa((struct sockaddr_dl *)sa));

	default:
	    {
		u_short *sp = (u_short *)sa;
		u_short *splim = sp + ((sa->sa_len + 1) >> 1);
		char *cps = c->rt_line + sprintf(c->rt_line, "(%d)", sa->sa_family);
		char *cpe = c->rt_line + sizeof(c->rt_line);

		while (++sp < splim && cps < cpe) /* start with sa->sa_data */
			if ((n = snprintf(cps, cpe - cps, " %x", *sp)) > 0)
				cps += n;
			else
				*cps = '\0';
		break;
	    }
	}
	return (c->rt_line);
}

/*
 * Return the name of the network whose address is given.
 * The address is assumed to be that of a net or subnet, not a host.
 */
const char *
netname(struct rt_ctx *c, struct sockaddr *sa)
{
	const char *cp = NULL;
	char atalk_buf[20];
	struct netent *np = NULL;
	u_long net, mask;
	u_long i;
	int n, subnetshift;

	switch (sa->sa_family) {

	case AF_INET:
	    {	struct in_addr in;
		in = ((struct sockaddr_in *)sa)->sin_addr;

		i = in.s_addr = ntohl(in.s_addr);
		if (in.s_addr == 0)
			cp = "default";
		else if (!c->nflag) {
			if (IN_CLASSA(i)) {
				mask = IN_CLASSA_NET;
				subnetshift = 8;
			} else if (IN_CLASSB(i)) {
				mask = IN_CLASSB_NET;
				subnetshift = 8;
			} else {
				mask = IN_CLASSC_NET;
				subnetshift = 4;
			}
			/*
			 * If there are more bits than the standard mask
			 * would suggest, subnets must be in use.
			 * Guess at the subnet mask, assuming reasonable
			 * width subnet fields.
			 */
			while (in.s_addr &~ mask)
				mask = (long)mask >> subnetshift;
			net = in.s_addr & mask;
			while ((mask & 1) == 0)
				mask >>= 1, net >>= 1;
			np = getnetbyaddr(net, AF_INET);
			if (np != NULL)
				cp = np->n_name;
		}
#define C(x)	(unsigned)((x) & 0xff)
		if (cp != NULL)
			strncpy(c->net_line, cp, sizeof(c->net_line));
		else if ((in.s_addr & 0xffffff) == 0)
			(void) sprintf(c->net_line, "%u", C(in.s_addr >> 24));
		else if ((in.s_addr & 0xffff) == 0)
			(void) sprintf(c->net_line, "%u.%u", C(in.s_addr >> 24),
			    C(in.s_addr >> 16));
		else if ((in.s_addr & 0xff) == 0)
			(void) sprintf(c->net_line, "%u.%u.%u", C(in.s_addr >> 24),
			    C(in.s_addr >> 16), C(in.s_addr >> 8));
		else
			(void) sprintf(c->net_line, "%u.%u.%u.%u", C(in.s_addr >> 24),
			    C(in.s_addr >> 16), C(in.s_addr >> 8),
			    C(in.s_addr));
#undef C
		break;
	    }

#ifdef INET6
	case AF_INET6:
	{
		struct sockaddr_in6 sin6; /* use static var for safety */
		int niflags = 0;

		memset(&sin6, 0, sizeof(sin6));
		memcpy(&sin6, sa, sa->sa_len);
		sin6.sin6_len = sizeof(struct sockaddr_in6);
		sin6.sin6_family = AF_INET6;
#ifdef __KAME__
		if (sa->sa_len == sizeof(struct sockaddr_in6) &&
		    (IN6_IS_ADDR_LINKLOCAL(&sin6.sin6_addr) ||
		     IN6_IS_ADDR_MC_LINKLOCAL(&sin6.sin6_addr) ||
		     IN6_IS_ADDR_MC_NODELOCAL(&sin6.sin6_addr)) &&
		    sin6.sin6_scope_id == 0) {
			sin6.sin6_scope_id =
			    ntohs(*(u_int16_t *)&sin6.sin6_addr.s6_addr[2]);
			sin6.sin6_addr.s6_addr[2] = 0;
			sin6.sin6_addr.s6_addr[3] = 0;
		}
#endif
		if (c->nflag)
			niflags |= NI_NUMERICHOST;
		if (getnameinfo((struct sockaddr *)&sin6, sin6.sin6_len,
		    c->net_line, sizeof(c->net_line), NULL, 0, niflags) != 0)
			strncpy(c->net_line, "invalid", sizeof(c->net_line));

		return(c->net_line);
	}
#endif

	case AF_APPLETALK:
		(void) snprintf(c->net_line, sizeof(c->net_line), "atalk %s",
			atalk_ntoa(((struct sockaddr_at *)sa)->sat_addr, atalk_buf));
		break;

	case AF_LINK:
		return (link_ntoa((struct sockaddr_dl *)sa));


	default:
	    {
		u_short *sp = (u_short *)sa->sa_data;
		u_short *splim = sp + ((sa->sa_len + 1)>>1);
		char *cps = c->net_line + sprintf(c->net_line, "af %d:", sa->sa_family);
		char *cpe = c->net_line + sizeof(c->net_line);

		while (sp < splim && cps < cpe)
			if ((n = snprintf(cps, cpe - cps, " %x", *sp++)) > 0)
				cps += n;
			else
				*cps = '\0';
		break;
	    }
	}
	return (c->net_line);
}

static void
set_metric(struct rt_ctx *c, char *value, int key)
{
	int flag = 0;
	u_long noval, *valp = &noval;

	switch (key) {
#define caseof(x, y, z)	case x: valp = &c->rt_metrics.z; flag = y; break
	caseof(K_MTU, RTV_MTU, rmx_mtu);
	caseof(K_HOPCOUNT, RTV_HOPCOUNT, rmx_hopcount);
	caseof(K_EXPIRE, RTV_EXPIRE, rmx_expire);
	caseof(K_RECVPIPE, RTV_RPIPE, rmx_recvpipe);
	caseof(K_SENDPIPE, RTV_SPIPE, rmx_sendpipe);
	caseof(K_SSTHRESH, RTV_SSTHRESH, rmx_ssthresh);
	caseof(K_RTT, RTV_RTT, rmx_rtt);
	caseof(K_RTTVAR, RTV_RTTVAR, rmx_rttvar);
	caseof(K_WEIGHT, RTV_WEIGHT, rmx_weight);
	}
	c->rtm_inits |= flag;
	if (c->lockrest || c->locking)
		c->rt_metrics.rmx_locks |= flag;
	if (c->locking)
		c->locking = 0;
	*valp = atoi(value);
}

#define	F_ISHOST	0x01
#define	F_FORCENET	0x02
#define	F_FORCEHOST	0x04
#define	F_PROXY		0x08
#define	F_INTERFACE	0x10

static void
newroute(struct rt_ctx *c, int argc, char **argv)
{
	struct hostent *hp;
	struct fibl *fl;
	char *cmd;
	const char *dest, *gateway, *errmsg;
	int key, error, flags, nrflags, fibnum;

	if (c->uid != 0) {
		errx(EX_NOPERM, "must be root to alter routing table");
	}

	dest = NULL;
	gateway = NULL;
	flags = RTF_STATIC;
	nrflags = 0;
	hp = NULL;
	TAILQ_INIT(&c->fibl_head);

	cmd = argv[0];
	if (*cmd != 'g' && *cmd != 's')
		shutdown(c->s, SHUT_RD); /* Don't want to read back our messages */

	while (--argc > 0) {
		if (**(++argv)== '-') {
			switch (key = keyword(1 + *argv)) {
			case K_LINK:
				c->af = AF_LINK;
				c->aflen = sizeof(struct sockaddr_dl);
				break;
			case K_INET:
				c->af = AF_INET;
				c->aflen = sizeof(struct sockaddr_in);
				break;
#ifdef INET6
			case K_INET6:
				c->af = AF_INET6;
				c->aflen = sizeof(struct sockaddr_in6);
				break;
#endif
			case K_ATALK:
				c->af = AF_APPLETALK;
				c->aflen = sizeof(struct sockaddr_at);
				break;
			case K_SA:
				c->af = PF_ROUTE;
				c->aflen = sizeof(union sockunion);
				break;
			case K_IFACE:
			case K_INTERFACE:
				nrflags |= F_INTERFACE;
				break;
			case K_NOSTATIC:
				flags &= ~RTF_STATIC;
				break;
			case K_LOCK:
				c->locking = 1;
				break;
			case K_LOCKREST:
				c->lockrest = 1;
				break;
			case K_HOST:
				nrflags |= F_FORCEHOST;
				break;
			case K_REJECT:
				flags |= RTF_REJECT;
				break;
			case K_BLACKHOLE:
				flags |= RTF_BLACKHOLE;
				break;
			case K_PROTO1:
				flags |= RTF_PROTO1;
				break;
			case K_PROTO2:
				flags |= RTF_PROTO2;
				break;
			case K_PROXY:
				nrflags |= F_PROXY;
				break;
			case K_XRESOLVE:
				flags |= RTF_XRESOLVE;
				break;
			case K_STATIC:
				flags |= RTF_STATIC;
				break;
			case K_STICKY:
				flags |= RTF_STICKY;
				break;
			case K_NOSTICK:
				flags &= ~RTF_STICKY;
				break;
			case K_FIB:
				if (!--argc)
					usage(NULL);
				error = fiboptlist_csv(c, *++argv, &c->fibl_head);
				if (error)
					errx(EX_USAGE,
					    "invalid fib number: %s", *argv);
				break;
			case K_IFA:
				if (!--argc)
					usage(NULL);
				getaddr(c, RTA_IFA, *++argv, 0, nrflags);
				break;
			case K_IFP:
				if (!--argc)
					usage(NULL);
				getaddr(c, RTA_IFP, *++argv, 0, nrflags);
				break;
			case K_GENMASK:
				if (!--argc)
					usage(NULL);
				getaddr(c, RTA_GENMASK, *++argv, 0, nrflags);
				break;
			case K_GATEWAY:
				if (!--argc)
					usage(NULL);
				getaddr(c, RTA_GATEWAY, *++argv, 0, nrflags);
				gateway = *argv;
				break;
			case K_DST:
				if (!--argc)
					usage(NULL);
				if (getaddr(c, RTA_DST, *++argv, &hp, nrflags))
					nrflags |= F_ISHOST;
				dest = *argv;
				break;
			case K_NETMASK:
				if (!--argc)
					usage(NULL);
				getaddr(c, RTA_NETMASK, *++argv, 0, nrflags);
				/* FALLTHROUGH */
			case K_NET:
				nrflags |= F_FORCENET;
				break;
			case K_PREFIXLEN:
				if (!--argc)
					usage(NULL);
				if (prefixlen(c, *++argv) == -1) {
					nrflags &= ~F_FORCENET;
					nrflags |= F_ISHOST;
				} else {
					nrflags |= F_FORCENET;
					nrflags &= ~F_ISHOST;
				}
				break;
			case K_MTU:
			case K_HOPCOUNT:
			case K_EXPIRE:
			case K_RECVPIPE:
			case K_SENDPIPE:
			case K_SSTHRESH:
			case K_RTT:
			case K_RTTVAR:
			case K_WEIGHT:
				if (!--argc)
					usage(NULL);
				set_metric(c, *++argv, key);
				break;
			default:
				usage(1+*argv);
			}
		} else {
			if ((c->rtm_addrs & RTA_DST) == 0) {
				dest = *argv;
				if (getaddr(c, RTA_DST, *argv, &hp, nrflags))
					nrflags |= F_ISHOST;
			} else if ((c->rtm_addrs & RTA_GATEWAY) == 0) {
				gateway = *argv;
				getaddr(c, RTA_GATEWAY, *argv, &hp, nrflags);
			} else {
				getaddr(c, RTA_NETMASK, *argv, 0, nrflags);
				nrflags |= F_FORCENET;
			}
		}
	}

	if (nrflags & F_FORCEHOST) {
		nrflags |= F_ISHOST;
#ifdef INET6
		if (c->af == AF_INET6) {
			c->rtm_addrs &= ~RTA_NETMASK;
			memset((void *)&c->so_mask, 0, sizeof(c->so_mask));
		}
#endif
	}
	if (nrflags & F_FORCENET)
		nrflags &= ~F_ISHOST;
	flags |= RTF_UP;
	if (nrflags & F_ISHOST)
		flags |= RTF_HOST;
	if ((nrflags & F_INTERFACE) == 0)
		flags |= RTF_GATEWAY;
	if (nrflags & F_PROXY) {
		c->so_dst.sinarp.sin_other = SIN_PROXY;
		flags |= RTF_ANNOUNCE;
	}
	if (dest == NULL)
		dest = "";
	if (gateway == NULL)
		gateway = "";

	if (TAILQ_EMPTY(&c->fibl_head)) {
		error = fiboptlist_csv(c, "default", &c->fibl_head);
		if (error)
			errx(EX_OSERR, "fiboptlist_csv failed.");
	}
	error = 0;
	TAILQ_FOREACH(fl, &c->fibl_head, fl_next) {
		fl->fl_error = newroute_fib(c, fl->fl_num, cmd, flags);
		if (fl->fl_error)
			fl->fl_errno = errno;
		error += fl->fl_error;
	}
	if (*cmd == 'g' || *cmd == 's')
		exit(error);

	error = 0;
	if (!c->qflag) {
		fibnum = 0;
		TAILQ_FOREACH(fl, &c->fibl_head, fl_next) {
			if (fl->fl_error == 0)
				fibnum++;
		}
		if (fibnum > 0) {
			int firstfib = 1;

			printf("%s %s %s", cmd,
			    (nrflags & F_ISHOST) ? "host" : "net", dest);
			if (*gateway)
				printf(": gateway %s", gateway);

			if (c->numfibs > 1) {
				TAILQ_FOREACH(fl, &c->fibl_head, fl_next) {
					if (fl->fl_error == 0
					    && fl->fl_num >= 0) {
						if (firstfib) {
							printf(" fib ");
							firstfib = 0;
						}
						printf("%d", fl->fl_num);
						if (fibnum-- > 1)
							printf(",");
					}
				}
			}
			printf("\n");
		}

		fibnum = 0;
		TAILQ_FOREACH(fl, &c->fibl_head, fl_next) {
			if (fl->fl_error != 0) {
				printf("%s %s %s", cmd, (nrflags & F_ISHOST)
				    ? "host" : "net", dest);
				if (*gateway)
					printf(": gateway %s", gateway);

				if (fl->fl_num >= 0)
					printf(" fib %d", fl->fl_num);

				switch (fl->fl_errno) {
				case ESRCH:
					errmsg = "not in table";
					break;
				case EBUSY:
					errmsg = "entry in use";
					break;
				case ENOBUFS:
					errmsg = "not enough memory";
					break;
				case EADDRINUSE:
					/*
					 * handle recursion avoidance
					 * in rt_setgate()
					 */
					errmsg = "gateway uses the same route";
					break;
				case EEXIST:
					errmsg = "route already in table";
					break;
				default:
					errmsg = strerror(fl->fl_errno);
					break;
				}
				printf(": %s\n", errmsg);
				error = 1;
			}
		}
	}
	exit(error);
}

static int
newroute_fib(struct rt_ctx *c, int fib, char *cmd, int flags)
{
	int error;

	error = set_sofib(c, fib);
	if (error) {
		warn("fib number %d is ignored", fib);
		return (error);
	}

	error = rtmsg(c, *cmd, flags, fib);
	return (error);
}

static void
inet_makenetandmask(struct rt_ctx *c, u_long net, struct sockaddr_in *sin, u_long bits)
{
	u_long addr, mask = 0;
	char *cp;

	c->rtm_addrs |= RTA_NETMASK;
	/*
	 * XXX: This approach unable to handle 0.0.0.1/32 correctly
	 * as inet_network() converts 0.0.0.1 and 1 equally.
	 */
	if (net <= 0xff)
		addr = net << IN_CLASSA_NSHIFT;
	else if (net <= 0xffff)
		addr = net << IN_CLASSB_NSHIFT;
	else if (net <= 0xffffff)
		addr = net << IN_CLASSC_NSHIFT;
	else
		addr = net;
	/*
	 * If no /xx was specified we must calculate the
	 * CIDR address.
	 */
	if ((bits == 0)  && (addr != 0)) {
		u_long i, j;
		for(i=0,j=0xff; i<4; i++)  {
			if (addr & j) {
				break;
			}
			j <<= 8;
		}
		/* i holds the first non zero bit */
		bits = 32 - (i*8);	
	}
	if (bits != 0)
		mask = 0xffffffff << (32 - bits);

	sin->sin_addr.s_addr = htonl(addr);
	sin = &c->so_mask.sin;
	sin->sin_addr.s_addr = htonl(mask);
	sin->sin_len = 0;
	sin->sin_family = 0;
	cp = (char *)(&sin->sin_addr + 1);
	while (*--cp == 0 && cp > (char *)sin)
		;
	sin->sin_len = 1 + cp - (char *)sin;
}

#ifdef INET6
/*
 * XXX the function may need more improvement...
 */
static int
inet6_makenetandmask(struct rt_ctx *c, struct sockaddr_in6 *sin6, const char *plen)
{
	struct in6_addr in6;

	if (plen == NULL) {
		if (IN6_IS_ADDR_UNSPECIFIED(&sin6->sin6_addr) &&
		    sin6->sin6_scope_id == 0) {
			plen = "0";
		} else if ((sin6->sin6_addr.s6_addr[0] & 0xe0) == 0x20) {
			/* aggregatable global unicast - RFC2374 */
			memset(&in6, 0, sizeof(in6));
			if (!memcmp(&sin6->sin6_addr.s6_addr[8],
				    &in6.s6_addr[8], 8))
				plen = "64";
		}
	}

	if (plen == NULL || strcmp(plen, "128") == 0)
		return (1);
	c->rtm_addrs |= RTA_NETMASK;
	prefixlen(c, plen);
	return (0);
}
#endif

/*
 * Interpret an argument as a network address of some kind,
 * returning 1 if a host address, 0 if a network address.
 */
static int
getaddr(struct rt_ctx *c, int which, char *str, struct hostent **hpp, int nrflags)
{
	sup su;
	struct hostent *hp;
	struct netent *np;
	u_long val;
	char *q;
	int afamily;  /* local copy of af so we can change it */

	if (c->af == 0) {
		c->af = AF_INET;
		c->aflen = sizeof(struct sockaddr_in);
	}
	afamily = c->af;
	c->rtm_addrs |= which;
	switch (which) {
	case RTA_DST:
		su = &c->so_dst;
		break;
	case RTA_GATEWAY:
		su = &c->so_gate;
		if (nrflags & F_INTERFACE) {
			struct ifaddrs *ifap, *ifa;
			struct sockaddr_dl *sdl = NULL;

			if (getifaddrs(&ifap))
				err(1, "getifaddrs");

			for (ifa = ifap; ifa != NULL; ifa = ifa->ifa_next) {
				if (ifa->ifa_addr->sa_family != AF_LINK)
					continue;

				if (strcmp(str, ifa->ifa_name) != 0)
					continue;

				sdl = (struct sockaddr_dl *)ifa->ifa_addr;
			}
			/* If we found it, then use it */
			if (sdl != NULL) {
				/*
				 * Copy is safe since we have a
				 * sockaddr_storage member in sockunion{}.
				 * Note that we need to copy before calling
				 * freeifaddrs().
				 */
				memcpy(&su->sdl, sdl, sdl->sdl_len);
			}
			freeifaddrs(ifap);
			if (sdl != NULL)
				return(1);
		}
		break;
	case RTA_NETMASK:
		su = &c->so_mask;
		break;
	case RTA_GENMASK:
		su = &c->so_genmask;
		break;
	case RTA_IFP:
		su = &c->so_ifp;
		afamily = AF_LINK;
		break;
	case RTA_IFA:
		su = &c->so_ifa;
		break;
	default:
		usage("internal error");
		/*NOTREACHED*/
	}
	su->sa.sa_len = c->aflen;
	su->sa.sa_family = afamily; /* cases that don't want it have left already */
	if (strcmp(str, "default") == 0) {
		/*
		 * Default is net 0.0.0.0/0
		 */
		switch (which) {
		case RTA_DST:
			c->forcenet++;
#if 0
			bzero(su, sizeof(*su));	/* for readability */
#endif
			getaddr(c, RTA_NETMASK, str, 0, nrflags);
			break;
#if 0
		case RTA_NETMASK:
		case RTA_GENMASK:
			bzero(su, sizeof(*su));	/* for readability */
#endif
		}
		return (0);
	}
	switch (afamily) {
#ifdef INET6
	case AF_INET6:
	{
		struct addrinfo hints, *res;
		int ecode;

		q = NULL;
		if (which == RTA_DST && (q = strchr(str, '/')) != NULL)
			*q = '\0';
		memset(&hints, 0, sizeof(hints));
		hints.ai_family = afamily;	/*AF_INET6*/
		hints.ai_socktype = SOCK_DGRAM;		/*dummy*/
		ecode = getaddrinfo(str, NULL, &hints, &res);
		if (ecode != 0 || res->ai_family != AF_INET6 ||
		    res->ai_addrlen != sizeof(su->sin6)) {
			(void) fprintf(stderr, "%s: %s\n", str,
			    gai_strerror(ecode));
			exit(1);
		}
		memcpy(&su->sin6, res->ai_addr, sizeof(su->sin6));
#ifdef __KAME__
		if ((IN6_IS_ADDR_LINKLOCAL(&su->sin6.sin6_addr) ||
		     IN6_IS_ADDR_MC_LINKLOCAL(&su->sin6.sin6_addr) ||
		     IN6_IS_ADDR_MC_NODELOCAL(&su->sin6.sin6_addr)) &&
		    su->sin6.sin6_scope_id) {
			*(u_int16_t *)&su->sin6.sin6_addr.s6_addr[2] =
				htons(su->sin6.sin6_scope_id);
			su->sin6.sin6_scope_id = 0;
		}
#endif
		freeaddrinfo(res);
		if (q != NULL)
			*q++ = '/';
		if (which == RTA_DST)
			return (inet6_makenetandmask(c, &su->sin6, q));
		return (0);
	}
#endif /* INET6 */

	case AF_APPLETALK:
		if (!atalk_aton(str, &su->sat.sat_addr))
			errx(EX_NOHOST, "bad address: %s", str);
		c->rtm_addrs |= RTA_NETMASK;
		return(c->forcehost || su->sat.sat_addr.s_node != 0);

	case AF_LINK:
		link_addr(str, &su->sdl);
		return (1);


	case PF_ROUTE:
		su->sa.sa_len = sizeof(*su);
		sockaddr(str, &su->sa);
		return (1);

	case AF_INET:
	default:
		break;
	}

	if (hpp == NULL)
		hpp = &hp;
	*hpp = NULL;

	q = strchr(str,'/');
	if (q != NULL && which == RTA_DST) {
		*q = '\0';
		if ((val = inet_network(str)) != INADDR_NONE) {
			inet_makenetandmask(
				c, val, &su->sin, strtoul(q+1, 0, 0));
			return (0);
		}
		*q = '/';
	}
	if ((which != RTA_DST || c->forcenet == 0) &&
	    inet_aton(str, &su->sin.sin_addr)) {
		val = su->sin.sin_addr.s_addr;
		if (which != RTA_DST || c->forcehost ||
		    inet_lnaof(su->sin.sin_addr) != INADDR_ANY)
			return (1);
		else {
			val = ntohl(val);
			goto netdone;
		}
	}
	if (which == RTA_DST && c->forcehost == 0 &&
	    ((val = inet_network(str)) != INADDR_NONE ||
	    ((np = getnetbyname(str)) != NULL && (val = np->n_net) != 0))) {
netdone:
		inet_makenetandmask(c, val, &su->sin, 0);
		return (0);
	}
	hp = gethostbyname(str);
	if (hp != NULL) {
		*hpp = hp;
		su->sin.sin_family = hp->h_addrtype;
		memmove((char *)&su->sin.sin_addr, hp->h_addr,
		    MIN((size_t)hp->h_length, sizeof(su->sin.sin_addr)));
		return (1);
	}
	errx(EX_NOHOST, "bad address: %s", str);
}

static int
prefixlen(struct rt_ctx *c, const char *str)
{
	int len = atoi(str), q, r;
	int max;
	char *p;

	c->rtm_addrs |= RTA_NETMASK;	
	switch (c->af) {
#ifdef INET6
	case AF_INET6:
		max = 128;
		p = (char *)&c->so_mask.sin6.sin6_addr;
		break;
#endif
	case AF_INET:
		max = 32;
		p = (char *)&c->so_mask.sin.sin_addr;
		break;
	default:
		fprintf(stderr, "prefixlen not supported in this af\n");
		exit(1);
	}

	if (len < 0 || max < len) {
		fprintf(stderr, "%s: bad value\n", str);
		exit(1);
	}
	
	q = len >> 3;
	r = len & 7;
	c->so_mask.sa.sa_family = c->af;
	c->so_mask.sa.sa_len = c->aflen;
	memset((void *)p, 0, max / 8);
	if (q > 0)
		memset((void *)p, 0xff, q);
	if (r > 0)
		*((u_char *)p + q) = (0xff00 >> r) & 0xff;
	if (len == max)
		return (-1);
	else
		return (len);
}

static void
interfaces(struct rt_ctx *c)
{
	size_t needed;
	int mib[6];
	char *buf, *lim, *next, count = 0;
	struct rt_msghdr *rtm;

retry2:
	mib[0] = CTL_NET;
	mib[1] = PF_ROUTE;
	mib[2] = 0;		/* protocol */
	mib[3] = 0;		/* wildcard address family */
	mib[4] = NET_RT_IFLIST;
	mib[5] = 0;		/* no flags */
	if (sysctl(mib, 6, NULL, &needed, NULL, 0) < 0)
		err(EX_OSERR, "route-sysctl-estimate");
	if ((buf = malloc(needed)) == NULL && needed != 0)
		errx(EX_OSERR, "malloc failed");
	if (sysctl(mib, 6, buf, &needed, NULL, 0) < 0) {
		if (errno == ENOMEM && count++ < 10) {
			warnx("Routing table grew, retrying");
			sleep(1);
			free(buf);
			goto retry2;
		}
		err(EX_OSERR, "actual retrieval of interface table");
	}
	lim = buf + needed;
	for (next = buf; next < lim; next += rtm->rtm_msglen) {
		rtm = (struct rt_msghdr *)next;
		print_rtmsg(c, rtm, rtm->rtm_msglen);
	}
	free(buf);
}

static void
monitor(struct rt_ctx *c, int argc, char *argv[])
{
	int n, fib, error;
	char msg[2048], *endptr;

	fib = c->defaultfib;
	while (argc > 1) {
		argc--;
		argv++;
		if (**argv != '-')
			usage(*argv);
		switch (keyword(*argv + 1)) {
		case K_FIB:
			if (!--argc)
				usage(*argv);
			errno = 0;
			fib = strtol(*++argv, &endptr, 0);
			if (errno == 0) {
				if (*endptr != '\0' ||
				    fib < 0 ||
				    (c->numfibs != -1 && fib > c->numfibs - 1))
					errno = EINVAL;
			}
			if (errno)
				errx(EX_USAGE, "invalid fib number: %s", *argv);
			break;
		default:
			usage(*argv);
		}
	}
	error = set_sofib(c, fib);
	if (error)
		errx(EX_USAGE, "invalid fib number: %d", fib);

	c->verbose = 1;
	if (c->debugonly) {
		interfaces(c);
		exit(0);
	}
	for (;;) {
		time_t now;
		n = read(c->s, msg, 2048);
		now = time(NULL);
		(void) printf("\ngot message of size %d on %s", n, ctime(&now));
		print_rtmsg(c, (struct rt_msghdr *)msg, n);
	}
}

static int
rtmsg(struct rt_ctx *c, int cmd, int flags, int fib)
{
	int rlen;
	char *cp = c->m_rtmsg.m_space;
	int l;

#define NEXTADDR(w, u) \
	if (c->rtm_addrs & (w)) {\
	    l = SA_SIZE(&(u.sa)); memmove(cp, &(u), l); cp += l;\
	    if (c->verbose) sodump(&(u),#u);\
	}

	errno = 0;
	memset(&c->m_rtmsg, 0, sizeof(c->m_rtmsg));
	if (cmd == 'a')
		cmd = RTM_ADD;
	else if (cmd == 'c')
		cmd = RTM_CHANGE;
	else if (cmd == 'g' || cmd == 's') {
		cmd = RTM_GET;
		if (c->so_ifp.sa.sa_family == 0) {
			c->so_ifp.sa.sa_family = AF_LINK;
			c->so_ifp.sa.sa_len = sizeof(struct sockaddr_dl);
			c->rtm_addrs |= RTA_IFP;
		}
	} else
		cmd = RTM_DELETE;
#define rtm c->m_rtmsg.m_rtm
	rtm.rtm_type = cmd;
	rtm.rtm_flags = flags;
	rtm.rtm_version = RTM_VERSION;
	rtm.rtm_seq = ++c->rtm_seq;
	rtm.rtm_addrs = c->rtm_addrs;
	rtm.rtm_rmx = c->rt_metrics;
	rtm.rtm_inits = c->rtm_inits;

	if (c->rtm_addrs & RTA_NETMASK)
		mask_addr(c);
	NEXTADDR(RTA_DST, c->so_dst);
	NEXTADDR(RTA_GATEWAY, c->so_gate);
	NEXTADDR(RTA_NETMASK, c->so_mask);
	NEXTADDR(RTA_GENMASK, c->so_genmask);
	NEXTADDR(RTA_IFP, c->so_ifp);
	NEXTADDR(RTA_IFA, c->so_ifa);
	rtm.rtm_msglen = l = cp - (char *)&c->m_rtmsg;
	if (c->verbose)
		print_rtmsg(c, &rtm, l);
	if (c->debugonly)
		return (0);
	if ((rlen = write(c->s, (char *)&c->m_rtmsg, l)) < 0) {
		if (errno == EPERM)
			err(1, "writing to routing socket");
		warn("writing to routing socket");
		return (-1);
	}
	if (cmd == RTM_GET) {
		do {
			l = read(c->s, (char *)&c->m_rtmsg, sizeof(c->m_rtmsg));
		} while (l > 0 && (rtm.rtm_seq != c->rtm_seq || rtm.rtm_pid != c->pid));
		if (l < 0)
			warn("read from routing socket");
		else
			print_getmsg(c, &rtm, l, fib);
	}
#undef rtm
	return (0);
}

static void
mask_addr(struct rt_ctx *c)
{
	int olen = c->so_mask.sa.sa_len;
	char *cp1 = olen + (char *)&c->so_mask, *cp2;

	for (c->so_mask.sa.sa_len = 0; cp1 > (char *)&c->so_mask; )
		if (*--cp1 != 0) {
			c->so_mask.sa.sa_len = 1 + cp1 - (char *)&c->so_mask;
			break;
		}
	if ((c->rtm_addrs & RTA_DST) == 0)
		return;
	switch (c->so_dst.sa.sa_family) {
	case AF_INET:
#ifdef INET6
	case AF_INET6:
#endif
	case AF_APPLETALK:
	case 0:
		return;
	}
	cp1 = c->so_mask.sa.sa_len + 1 + (char *)&c->so_dst;
	cp2 = c->so_dst.sa.sa_len + 1 + (char *)&c->so_dst;
	while (cp2 > cp1)
		*--cp2 = 0;
	cp2 = c->so_mask.sa.sa_len + 1 + (char *)&c->so_mask;
	while (cp1 > c->so_dst.sa.sa_data)
		*--cp1 &= *--cp2;
}

static const char *const msgtypes[] = {
	"",
	"RTM_ADD: Add Route",
	"RTM_DELETE: Delete Route",
	"RTM_CHANGE: Change Metrics or flags",
	"RTM_GET: Report Metrics",
	"RTM_LOSING: Kernel Suspects Partitioning",
	"RTM_REDIRECT: Told to use different route",
	"RTM_MISS: Lookup failed on this address",
	"RTM_LOCK: fix specified metrics",
	"RTM_OLDADD: caused by SIOCADDRT",
	"RTM_OLDDEL: caused by SIOCDELRT",
	"RTM_RESOLVE: Route created by cloning",
	"RTM_NEWADDR: address being added to iface",
	"RTM_DELADDR: address being removed from iface",
	"RTM_IFINFO: iface status change",
	"RTM_NEWMADDR: new multicast group membership on iface",
	"RTM_DELMADDR: multicast group membership removed from iface",
	"RTM_IFANNOUNCE: interface arrival/departure",
	"RTM_IEEE80211: IEEE 802.11 wireless event",
};

static const char metricnames[] =
"\011weight\010rttvar\7rtt\6ssthresh\5sendpipe\4recvpipe\3expire"
"\1mtu";
static const char routeflags[] =
"\1UP\2GATEWAY\3HOST\4REJECT\5DYNAMIC\6MODIFIED\7DONE"
"\012XRESOLVE\013LLINFO\014STATIC\015BLACKHOLE"
"\017PROTO2\020PROTO1\021PRCLONING\022WASCLONED\023PROTO3"
"\025PINNED\026LOCAL\027BROADCAST\030MULTICAST\035STICKY";
static const char ifnetflags[] =
"\1UP\2BROADCAST\3DEBUG\4LOOPBACK\5PTP\6b6\7RUNNING\010NOARP"
"\011PPROMISC\012ALLMULTI\013OACTIVE\014SIMPLEX\015LINK0\016LINK1"
"\017LINK2\020MULTICAST";
static const char addrnames[] =
"\1DST\2GATEWAY\3NETMASK\4GENMASK\5IFP\6IFA\7AUTHOR\010BRD";

static const char errfmt[] =
"\n%s: truncated route message, only %zu bytes left\n";

static void
print_rtmsg(struct rt_ctx *c, struct rt_msghdr *rtm, size_t msglen)
{
	struct if_msghdr *ifm;
	struct ifa_msghdr *ifam;
#ifdef RTM_NEWMADDR
	struct ifma_msghdr *ifmam;
#endif
	struct if_announcemsghdr *ifan;
	const char *state;

	if (c->verbose == 0)
		return;
	if (rtm->rtm_version != RTM_VERSION) {
		(void) printf("routing message version %d not understood\n",
		    rtm->rtm_version);
		return;
	}
	if (rtm->rtm_type < sizeof(msgtypes) / sizeof(msgtypes[0]))
		(void)printf("%s: ", msgtypes[rtm->rtm_type]);
	else
		(void)printf("unknown type %d: ", rtm->rtm_type);
	(void)printf("len %d, ", rtm->rtm_msglen);

#define	REQUIRE(x)	do {		\
	if (msglen < sizeof(x))		\
		goto badlen;		\
	else				\
		msglen -= sizeof(x);	\
	} while (0)

	switch (rtm->rtm_type) {
	case RTM_IFINFO:
		REQUIRE(struct if_msghdr);
		ifm = (struct if_msghdr *)rtm;
		(void) printf("if# %d, ", ifm->ifm_index);
		switch (ifm->ifm_data.ifi_link_state) {
		case LINK_STATE_DOWN:
			state = "down";
			break;
		case LINK_STATE_UP:
			state = "up";
			break;
		default:
			state = "unknown";
			break;
		}
		(void) printf("link: %s, flags:", state);
		bprintf(stdout, ifm->ifm_flags, ifnetflags);
		pmsg_addrs(c, (char *)(ifm + 1), ifm->ifm_addrs, msglen);
		break;
	case RTM_NEWADDR:
	case RTM_DELADDR:
		REQUIRE(struct ifa_msghdr);
		ifam = (struct ifa_msghdr *)rtm;
		(void) printf("metric %d, flags:", ifam->ifam_metric);
		bprintf(stdout, ifam->ifam_flags, routeflags);
		pmsg_addrs(c, (char *)(ifam + 1), ifam->ifam_addrs, msglen);
		break;
#ifdef RTM_NEWMADDR
	case RTM_NEWMADDR:
	case RTM_DELMADDR:
		REQUIRE(struct ifma_msghdr);
		ifmam = (struct ifma_msghdr *)rtm;
		pmsg_addrs(c, (char *)(ifmam + 1), ifmam->ifmam_addrs, msglen);
		break;
#endif
	case RTM_IFANNOUNCE:
		REQUIRE(struct if_announcemsghdr);
		ifan = (struct if_announcemsghdr *)rtm;
		(void) printf("if# %d, what: ", ifan->ifan_index);
		switch (ifan->ifan_what) {
		case IFAN_ARRIVAL:
			printf("arrival");
			break;
		case IFAN_DEPARTURE:
			printf("departure");
			break;
		default:
			printf("#%d", ifan->ifan_what);
			break;
		}
		printf("\n");
		fflush(stdout);
		break;

	default:
		(void) printf("pid: %ld, seq %d, errno %d, flags:",
			(long)rtm->rtm_pid, rtm->rtm_seq, rtm->rtm_errno);
		bprintf(stdout, rtm->rtm_flags, routeflags);
		pmsg_common(c, rtm, msglen);
	}

	return;

badlen:
	(void)printf(errfmt, __func__, msglen);
#undef	REQUIRE
}

static void
print_getmsg(struct rt_ctx *c, struct rt_msghdr *rtm, int msglen, int fib)
{
	struct sockaddr *dst = NULL, *gate = NULL, *mask = NULL;
	struct sockaddr_dl *ifp = NULL;
	struct sockaddr *sa;
	char *cp;
	int i;

	(void) printf("   route to: %s\n",
	    routename(c, (struct sockaddr *)&c->so_dst));
	if (rtm->rtm_version != RTM_VERSION) {
		warnx("routing message version %d not understood",
		     rtm->rtm_version);
		return;
	}
	if (rtm->rtm_msglen > msglen) {
		warnx("message length mismatch, in packet %d, returned %d",
		      rtm->rtm_msglen, msglen);
	}
	if (rtm->rtm_errno)  {
		errno = rtm->rtm_errno;
		warn("message indicates error %d", errno);
		return;
	}
	cp = ((char *)(rtm + 1));
	if (rtm->rtm_addrs)
		for (i = 1; i; i <<= 1)
			if (i & rtm->rtm_addrs) {
				sa = (struct sockaddr *)cp;
				switch (i) {
				case RTA_DST:
					dst = sa;
					break;
				case RTA_GATEWAY:
					gate = sa;
					break;
				case RTA_NETMASK:
					mask = sa;
					break;
				case RTA_IFP:
					if (sa->sa_family == AF_LINK &&
					   ((struct sockaddr_dl *)sa)->sdl_nlen)
						ifp = (struct sockaddr_dl *)sa;
					break;
				}
				cp += SA_SIZE(sa);
			}
	if (dst && mask)
		mask->sa_family = dst->sa_family;	/* XXX */
	if (dst)
		(void)printf("destination: %s\n", routename(c, dst));
	if (mask) {
		int savenflag = c->nflag;

		c->nflag = 1;
		(void)printf("       mask: %s\n", routename(c, mask));
		c->nflag = savenflag;
	}
	if (gate && rtm->rtm_flags & RTF_GATEWAY)
		(void)printf("    gateway: %s\n", routename(c, gate));
	if (fib >= 0)
		(void)printf("        fib: %u\n", (unsigned int)fib);
	if (ifp)
		(void)printf("  interface: %.*s\n",
		    ifp->sdl_nlen, ifp->sdl_data);
	(void)printf("      flags: ");
	bprintf(stdout, rtm->rtm_flags, routeflags);

#define lock(f)	((rtm->rtm_rmx.rmx_locks & __CONCAT(RTV_,f)) ? 'L' : ' ')
#define msec(u)	(((u) + 500) / 1000)		/* usec to msec */

	(void) printf("\n%s\n", "\
 recvpipe  sendpipe  ssthresh  rtt,msec    mtu        weight    expire");
	printf("%8ld%c ", rtm->rtm_rmx.rmx_recvpipe, lock(RPIPE));
	printf("%8ld%c ", rtm->rtm_rmx.rmx_sendpipe, lock(SPIPE));
	printf("%8ld%c ", rtm->rtm_rmx.rmx_ssthresh, lock(SSTHRESH));
	printf("%8ld%c ", msec(rtm->rtm_rmx.rmx_rtt), lock(RTT));
	printf("%8ld%c ", rtm->rtm_rmx.rmx_mtu, lock(MTU));
	printf("%8ld%c ", rtm->rtm_rmx.rmx_weight, lock(WEIGHT));
	if (rtm->rtm_rmx.rmx_expire)
		rtm->rtm_rmx.rmx_expire -= time(0);
	printf("%8ld%c\n", rtm->rtm_rmx.rmx_expire, lock(EXPIRE));
#undef lock
#undef msec
#define	RTA_IGN	(RTA_DST|RTA_GATEWAY|RTA_NETMASK|RTA_IFP|RTA_IFA|RTA_BRD)
	if (c->verbose)
		pmsg_common(c, rtm, msglen);
	else if (rtm->rtm_addrs &~ RTA_IGN) {
		(void) printf("sockaddrs: ");
		bprintf(stdout, rtm->rtm_addrs, addrnames);
		putchar('\n');
	}
#undef	RTA_IGN
}

static void
pmsg_common(struct rt_ctx *c, struct rt_msghdr *rtm, size_t msglen)
{
	(void) printf("\nlocks: ");
	bprintf(stdout, rtm->rtm_rmx.rmx_locks, metricnames);
	(void) printf(" inits: ");
	bprintf(stdout, rtm->rtm_inits, metricnames);
	if (msglen > sizeof(struct rt_msghdr))
		pmsg_addrs(c, ((char *)(rtm + 1)), rtm->rtm_addrs,
		    msglen - sizeof(struct rt_msghdr));
	else
		(void) fflush(stdout);
}

static void
pmsg_addrs(struct rt_ctx *c, char *cp, int addrs, size_t len)
{
	struct sockaddr *sa;
	int i;

	if (addrs == 0) {
		(void) putchar('\n');
		return;
	}
	(void) printf("\nsockaddrs: ");
	bprintf(stdout, addrs, addrnames);
	(void) putchar('\n');
	for (i = 1; i != 0; i <<= 1)
		if (i & addrs) {
			sa = (struct sockaddr *)cp;
			if (len == 0 || len < SA_SIZE(sa)) {
				(void) printf(errfmt, __func__, len);
				break;
			}
			(void) printf(" %s", routename(c, sa));
			len -= SA_SIZE(sa);
			cp += SA_SIZE(sa);
		}
	(void) putchar('\n');
	(void) fflush(stdout);
}

static void
bprintf(FILE *fp, int b, const char *sstr)
{
  const u_char *str = (const u_char *) sstr;
	int i;
	int gotsome = 0;

	if (b == 0)
		return;
	while ((i = *str++) != 0) {
		if (b & (1 << (i-1))) {
			if (gotsome == 0)
				i = '<';
			else
				i = ',';
			(void) putc(i, fp);
			gotsome = 1;
			for (; (i = *str) > 32; str++)
				(void) putc(i, fp);
		} else
			while (*str > 32)
				str++;
	}
	if (gotsome)
		(void) putc('>', fp);
}

int
keyword(const char *cp)
{
	const struct keytab *kt = keywords;

	while (kt->kt_cp != NULL && strcmp(kt->kt_cp, cp) != 0)
		kt++;
	return (kt->kt_i);
}

static void
sodump(sup su, const char *which)
{
	char atalk_buf[20];

	switch (su->sa.sa_family) {
	case AF_LINK:
		(void) printf("%s: link %s; ",
		    which, link_ntoa(&su->sdl));
		break;
	case AF_INET:
		(void) printf("%s: inet %s; ",
		    which, inet_ntoa(su->sin.sin_addr));
		break;
	case AF_APPLETALK:
		(void) printf("%s: atalk %s; ",
		    which, atalk_ntoa(su->sat.sat_addr, atalk_buf));
		break;
	}
	(void) fflush(stdout);
}

/* States*/
#define VIRGIN	0
#define GOTONE	1
#define GOTTWO	2
/* Inputs */
#define	DIGIT	(4*0)
#define	END	(4*1)
#define DELIM	(4*2)

static void
sockaddr(char *addr, struct sockaddr *sa)
{
	char *cp = (char *)sa;
	int size = sa->sa_len;
	char *cplim = cp + size;
	int byte = 0, state = VIRGIN, new = 0 /* foil gcc */;

	memset(cp, 0, size);
	cp++;
	do {
		if ((*addr >= '0') && (*addr <= '9')) {
			new = *addr - '0';
		} else if ((*addr >= 'a') && (*addr <= 'f')) {
			new = *addr - 'a' + 10;
		} else if ((*addr >= 'A') && (*addr <= 'F')) {
			new = *addr - 'A' + 10;
		} else if (*addr == '\0')
			state |= END;
		else
			state |= DELIM;
		addr++;
		switch (state /* | INPUT */) {
		case GOTTWO | DIGIT:
			*cp++ = byte; /*FALLTHROUGH*/
		case VIRGIN | DIGIT:
			state = GOTONE; byte = new; continue;
		case GOTONE | DIGIT:
			state = GOTTWO; byte = new + (byte << 4); continue;
		default: /* | DELIM */
			state = VIRGIN; *cp++ = byte; byte = 0; continue;
		case GOTONE | END:
		case GOTTWO | END:
			*cp++ = byte; /* FALLTHROUGH */
		case VIRGIN | END:
			break;
		}
		break;
	} while (cp < cplim);
	sa->sa_len = cp - (char *)sa;
}

static int
atalk_aton(const char *text, struct at_addr *addr)
{
	u_int net, node;

	if (sscanf(text, "%u.%u", &net, &node) != 2
	    || net > 0xffff || node > 0xff)
		return(0);
	addr->s_net = htons(net);
	addr->s_node = node;
	return(1);
}

static char *
atalk_ntoa(struct at_addr at, char buf[20])
{
	(void) snprintf(buf, sizeof(buf), "%u.%u", ntohs(at.s_net), at.s_node);
	return(buf);
}
