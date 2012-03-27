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
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <freebsd/machine/rtems-bsd-config.h>

/*#include <freebsd/sys/types.h>
#include <freebsd/sys/systm.h>
#include <freebsd/sys/malloc.h>
#include <freebsd/sys/jail.h>
#include <freebsd/sys/lock.h>
#include <freebsd/sys/mutex.h>*/

#include <freebsd/sys/param.h>
#include <freebsd/sys/types.h>
#include <freebsd/sys/kernel.h>
#include <freebsd/sys/systm.h>
#include <freebsd/sys/errno.h>
#include <freebsd/sys/sysproto.h>
#include <freebsd/sys/malloc.h>
#include <freebsd/sys/osd.h>
#include <freebsd/sys/priv.h>
#include <freebsd/sys/proc.h>
#include <freebsd/sys/taskqueue.h>
#include <freebsd/sys/fcntl.h>
#include <freebsd/sys/jail.h>
#include <freebsd/sys/lock.h>
#include <freebsd/sys/mutex.h>
#include <freebsd/sys/sx.h>
#include <freebsd/sys/sysent.h>
#include <freebsd/sys/namei.h>
#include <freebsd/sys/mount.h>
#include <freebsd/sys/queue.h>
#include <freebsd/sys/socket.h>
#include <freebsd/sys/syscallsubr.h>
#include <freebsd/sys/sysctl.h>

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
        return (cred->cr_prison->pr_flags & flag);
}


void prison_free(struct prison *pr) {}
void prison_hold(struct prison *pr) {}

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
 * Return true if pr1 and pr2 have the same IPv4 address restrictions.
 */
int
prison_equal_ip4(struct prison *pr1, struct prison *pr2)
{
  return 1;
}

int
prison_check_ip6(struct ucred *cred, struct in6_addr *ia6)
{
  return EAFNOSUPPORT;
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
  return EAFNOSUPPORT;
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
  return EAFNOSUPPORT;
}
/*
 * Return true if pr1 and pr2 have the same IPv6 address restrictions.
 */
int
prison_equal_ip6(struct prison *pr1, struct prison *pr2)
{
  return 1;
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
 */
int
prison_local_ip6(struct ucred *cred, struct in6_addr *ia6, int v6only)
{
  return EAFNOSUPPORT;
}

/*
 * Rewrite destination address in case we will connect to loopback address.
 *
 * Returns 0 on success, EAFNOSUPPORT if the jail doesn't allow IPv6.
 */
int
prison_remote_ip6(struct ucred *cred, struct in6_addr *ia6)
{
  return EAFNOSUPPORT;
}
