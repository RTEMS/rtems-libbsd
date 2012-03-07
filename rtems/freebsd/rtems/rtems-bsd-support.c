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
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>

#include <rtems/freebsd/sys/types.h>
#include <rtems/freebsd/sys/systm.h>
#include <rtems/freebsd/sys/malloc.h>
#include <rtems/freebsd/sys/uio.h>

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
