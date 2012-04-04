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
