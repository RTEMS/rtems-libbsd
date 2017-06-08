/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief This object is an minimal rtems implementation of kern_jail.c.
 */

/*
 * Copyright (c) 2009, 2010 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
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

#include <machine/rtems-bsd-kernel-space.h>

/*#include <sys/types.h>
#include <sys/systm.h>
#include <sys/malloc.h>
#include <sys/jail.h>
#include <sys/lock.h>
#include <sys/mutex.h>*/

#include <sys/param.h>
#include <sys/types.h>
#include <sys/kernel.h>
#include <sys/systm.h>
#include <sys/errno.h>
#include <sys/sysproto.h>
#include <sys/malloc.h>
#include <sys/osd.h>
#include <sys/priv.h>
#include <sys/proc.h>
#include <sys/taskqueue.h>
#include <sys/fcntl.h>
#include <sys/jail.h>
#include <sys/lock.h>
#include <sys/mutex.h>
#include <sys/sx.h>
#include <sys/sysent.h>
#include <sys/namei.h>
#include <sys/mount.h>
#include <sys/queue.h>
#include <sys/socket.h>
#include <sys/syscallsubr.h>
#include <sys/sysctl.h>

#define DEFAULT_HOSTUUID  "00000000-0000-0000-0000-000000000000"

/* Keep struct prison prison0 and some code in kern_jail_set() readable. */
#ifdef INET
#ifdef INET6
#define _PR_IP_SADDRSEL PR_IP4_SADDRSEL|PR_IP6_SADDRSEL
#else
#define _PR_IP_SADDRSEL PR_IP4_SADDRSEL
#endif
#else /* !INET */
#ifdef INET6
#define _PR_IP_SADDRSEL PR_IP6_SADDRSEL
#else
#define _PR_IP_SADDRSEL 0
#endif
#endif

/* prison0 describes what is "real" about the system. */
struct prison prison0 = {
  .pr_id    = 0,
  .pr_name  = "0",
  .pr_ref   = 1,
  .pr_uref  = 1,
  .pr_path  = "/",
  .pr_securelevel = -1,
  .pr_childmax  = JAIL_MAX,
  .pr_hostuuid  = DEFAULT_HOSTUUID,
  .pr_children  = LIST_HEAD_INITIALIZER(prison0.pr_children),
#ifdef VIMAGE
  .pr_flags = PR_HOST|PR_VNET|_PR_IP_SADDRSEL,
#else
  .pr_flags = PR_HOST|_PR_IP_SADDRSEL,
#endif
  .pr_allow = PR_ALLOW_ALL,
};
MTX_SYSINIT(prison0, &prison0.pr_mtx, "jail mutex", MTX_DEF);

/*
 * See if a prison has the specific flag set.
 */
int
prison_flag(struct ucred *cred, unsigned flag)
{
  /* This is an atomic read, so no locking is necessary. */
  return (prison0.pr_flags & flag);
}

void
prison_free(struct prison *pr)
{
}

void
prison_hold(struct prison *pr)
{
}

/*
 * Check if given address belongs to the jail referenced by cred (wrapper to
 * prison_check_ip[46]).
 *
 * Returns 0 if jail doesn't restrict the address family or if address belongs
 * to jail, EADDRNOTAVAIL if the address doesn't belong, or EAFNOSUPPORT if
 * the jail doesn't allow the address family.  IPv4 Address passed in in NBO.
 */
int
prison_if(struct ucred *cred, struct sockaddr *sa)
{
  return 0;
}

/*
 * Return 1 if we should do proper source address selection or are not jailed.
 * We will return 0 if we should bypass source address selection in favour
 * of the primary jail IPv6 address. Only in this case *ia will be updated and
 * returned in NBO.
 * Return EAFNOSUPPORT, in case this jail does not allow IPv6.
 */
int
prison_saddrsel_ip6(struct ucred *cred, struct in6_addr *ia6)
{
  return EAFNOSUPPORT;
}

/*
 * Check if given address belongs to the jail referenced by cred/prison.
 *
 * Returns 0 if jail doesn't restrict IPv4 or if address belongs to jail,
 * EADDRNOTAVAIL if the address doesn't belong, or EAFNOSUPPORT if the jail
 * doesn't allow IPv4.  Address passed in in NBO.
 */
int
prison_check_ip4(const struct ucred *cred, const struct in_addr *ia)
{
  return 0;
}

/*
 * Assuming 0 means no restrictions.
 *
 * NOTE: RTEMS does not restrict via a jail so return 0.
 */
int
prison_check_ip6(const struct ucred *cred, const struct in6_addr *ia6)
{
  return 0;
}

/*
 * Make sure our (source) address is set to something meaningful to this
 * jail.
 *
 * Returns 0 if jail doesn't restrict IPv4 or if address belongs to jail,
 * EADDRNOTAVAIL if the address doesn't belong, or EAFNOSUPPORT if the jail
 * doesn't allow IPv4.  Address passed in in NBO and returned in NBO.
 */
int
prison_local_ip4(struct ucred *cred, struct in_addr *ia)
{
  return 0;
}

/*
 * Rewrite destination address in case we will connect to loopback address.
 *
 * Returns 0 on success, EAFNOSUPPORT if the jail doesn't allow IPv4.
 * Address passed in in NBO and returned in NBO.
 */
int
prison_remote_ip4(struct ucred *cred, struct in_addr *ia)
{
  return 0;
}

/*
 * Make sure our (source) address is set to something meaningful to this jail.
 *
 * v6only should be set based on (inp->inp_flags & IN6P_IPV6_V6ONLY != 0)
 * when needed while binding.
 *
 * Returns 0 if jail doesn't restrict IPv6 or if address belongs to jail,
 * EADDRNOTAVAIL if the address doesn't belong, or EAFNOSUPPORT if the jail
 * doesn't allow IPv6.
 *
 * NOTE: RTEMS does not restrict via a jail so return 0.
 */
int
prison_local_ip6(struct ucred *cred, struct in6_addr *ia6, int v6only)
{
  return 0;
}

/*
 * Rewrite destination address in case we will connect to loopback address.
 *
 * Returns 0 on success, EAFNOSUPPORT if the jail doesn't allow IPv6.
 *
 * NOTE: RTEMS does not restrict via a jail so return 0.
 */
int
prison_remote_ip6(struct ucred *cred, struct in6_addr *ia6)
{
  return 0;
}

/*
 * Return 1 if we should do proper source address selection or are not jailed.
 * We will return 0 if we should bypass source address selection in favour
 * of the primary jail IPv4 address. Only in this case *ia will be updated and
 * returned in NBO.
 * Return EAFNOSUPPORT, in case this jail does not allow IPv4.
 */
int
prison_saddrsel_ip4(struct ucred *cred, struct in_addr *ia)
{
  return 1;
}

/*
 * Pass back primary IPv4 address of this jail.
 *
 * If not restricted return success but do not alter the address.  Caller has
 * to make sure to initialize it correctly (e.g. INADDR_ANY).
 *
 * Returns 0 on success, EAFNOSUPPORT if the jail doesn't allow IPv4.
 * Address returned in NBO.
 */
int
prison_get_ip4(struct ucred *cred, struct in_addr *ia)
{
  return 0;
}

/*
 * Return 1 if the passed credential is in a jail and that jail does not
 * have its own virtual network stack, otherwise 0.
 */
int
jailed_without_vnet(struct ucred *cred)
{
  return 0;
}

/*
 * Pass back primary IPv6 address for this jail.
 *
 * If not restricted return success but do not alter the address.  Caller has
 * to make sure to initialize it correctly (e.g. IN6ADDR_ANY_INIT).
 *
 * Returns 0 on success, EAFNOSUPPORT if the jail doesn't allow IPv6.
 */
int
prison_get_ip6(struct ucred *cred, struct in6_addr *ia6)
{
  return 0;
}
/*
 * Return 0 if jails permit p1 to frob p2, otherwise ESRCH.
 */
int
prison_check(struct ucred *cred1, struct ucred *cred2)
{
  return 0;
}

/*
 * Check if a jail supports the given address family.
 *
 * Returns 0 if not jailed or the address family is supported, EAFNOSUPPORT
 * if not.
 */
int
prison_check_af(struct ucred *cred, int af)
{
  return 0;
}

/*
 * Return the correct hostname (domainname, et al) for the passed credential.
 */
void
getcredhostname(struct ucred *cred, char *buf, size_t size)
{
  gethostname(buf, size);
}

void
getcreddomainname(struct ucred *cred, char *buf, size_t size)
{
  getdomainname(buf, size);
}

void
getcredhostid(struct ucred *cred, unsigned long *hostid)
{
  *hostid = 0;
}

/*
 * Return 1 if the passed credential is in a jail, otherwise 0.
 */
int
jailed(struct ucred *cred)
{
  return 0;
}
