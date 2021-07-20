/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief This object is an minimal RTEMS implementation of kern_jail.c.
 */

/*
 * Copyright (c) 2009, 2019 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4.
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

#include <rtems/score/threadqimpl.h>

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

  /*
   * Statically initialize this mutex to allow a garbage collection of this
   * structure.
   */
  .pr_mtx = {
    .lock_object = {
      .lo_flags = LO_INITIALIZED,
      .lo_mtx = {
	.queue = THREAD_QUEUE_INITIALIZER("jail mutex"),
	.nest_level = 0
      }
    }
  },

  .pr_hostuuid  = DEFAULT_HOSTUUID,
  .pr_children  = LIST_HEAD_INITIALIZER(prison0.pr_children),
#ifdef VIMAGE
  .pr_flags = PR_HOST|PR_VNET|_PR_IP_SADDRSEL,
#else
  .pr_flags = PR_HOST|_PR_IP_SADDRSEL,
#endif
  .pr_allow = PR_ALLOW_ALL_STATIC
};

void
getcredhostuuid(struct ucred *cred, char *buf, size_t size)
{

	(void)cred;
	mtx_lock(&prison0.pr_mtx);
	strlcpy(buf, prison0.pr_hostuuid, size);
	mtx_unlock(&prison0.pr_mtx);
}
