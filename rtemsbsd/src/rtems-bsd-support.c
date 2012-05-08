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

/*
 * This violation is specifically for _Thread_Disable_dispatch
 * and _Thread_Enable_dispatch. Use of the critical_enter()
 * and critical_exit() routines should be reviewed.
 */
#define __RTEMS_VIOLATE_KERNEL_VISIBILITY__ 
#include <freebsd/machine/rtems-bsd-config.h>

#include <freebsd/sys/types.h>
#include <freebsd/sys/systm.h>
#include <freebsd/sys/malloc.h>
#include <freebsd/sys/uio.h>

int     maxproc = 6;               /* XXX Used value of rtems KERN_MAXPROC */
int     ngroups_max = NGROUPS_MAX; /* XXX */

int     maxfiles = 7;                  /* XXX sys. wide open files limit */
int     maxfilesperproc = 27;          /* XXX per-proc open files limit */
int     cold = 1;                      /* XXX Wasn't sure where to put this */
uintptr_t dpcpu_off[MAXCPU];
int	hogticks = 2;                  /* hogticks = 2 * sched_quantum */

int
copyout(const void *kaddr, void *udaddr, size_t len)
{
  bcopy(kaddr, udaddr, len);
  return (0);
}

int
copyin(const void *udaddr, void *kaddr, size_t len)
{
  bcopy(udaddr, kaddr, len);
  return (0);
}

#if 0
/*
 * As of 27 March 2012, use version in kern_subr.c
 */
int
copyiniov(struct iovec *iovp, u_int iovcnt, struct iovec **iov, int error)
{
  u_int iovlen;

  *iov = NULL;
  if (iovcnt > UIO_MAXIOV)
    return (error);
  iovlen = iovcnt * sizeof (struct iovec);
  *iov = malloc(iovlen, M_IOV, M_WAITOK);
  error = copyin(iovp, *iov, iovlen);
  if (error) {
    free(*iov, M_IOV);
    *iov = NULL;
  }
  return (error);
}
#endif

void
critical_enter(void)
{
  _Thread_Disable_dispatch();
}

void
critical_exit(void)
{
  _Thread_Enable_dispatch();
}
