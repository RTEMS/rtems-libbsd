/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief TODO.
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

#include <rtems/freebsd/machine/rtems-bsd-config.h>

/*#include <rtems/freebsd/sys/types.h>
#include <rtems/freebsd/sys/systm.h>
#include <rtems/freebsd/sys/malloc.h>
#include <rtems/freebsd/sys/jail.h>
#include <rtems/freebsd/sys/lock.h>
#include <rtems/freebsd/sys/mutex.h>*/

#include <rtems/freebsd/sys/param.h>
#include <rtems/freebsd/sys/types.h>
#include <rtems/freebsd/sys/kernel.h>
#include <rtems/freebsd/sys/systm.h>
#include <rtems/freebsd/sys/errno.h>
#include <rtems/freebsd/sys/sysproto.h>
#include <rtems/freebsd/sys/malloc.h>
#include <rtems/freebsd/sys/osd.h>
#include <rtems/freebsd/sys/priv.h>
#include <rtems/freebsd/sys/proc.h>
#include <rtems/freebsd/sys/taskqueue.h>
#include <rtems/freebsd/sys/fcntl.h>
#include <rtems/freebsd/sys/jail.h>
#include <rtems/freebsd/sys/lock.h>
#include <rtems/freebsd/sys/mutex.h>
#include <rtems/freebsd/sys/sx.h>
#include <rtems/freebsd/sys/sysent.h>
#include <rtems/freebsd/sys/namei.h>
#include <rtems/freebsd/sys/mount.h>
#include <rtems/freebsd/sys/queue.h>
#include <rtems/freebsd/sys/socket.h>
#include <rtems/freebsd/sys/syscallsubr.h>
#include <rtems/freebsd/sys/sysctl.h>

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

