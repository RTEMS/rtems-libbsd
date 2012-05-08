/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief TODO.
 */

/*
 * Copyright (c) 2012  On-Line Applications Research Corporation (OAR).
 * All rights reserved.
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

#include <freebsd/machine/rtems-bsd-config.h>

#include <freebsd/sys/param.h>
#include <freebsd/sys/systm.h>

#include <freebsd/sys/conf.h>
#include <freebsd/sys/domain.h>
#include <freebsd/sys/fcntl.h>
#include <freebsd/sys/file.h>
#include <freebsd/sys/filedesc.h>
#include <freebsd/sys/filio.h>
#include <freebsd/sys/jail.h>
#include <freebsd/sys/kernel.h>
#include <freebsd/sys/limits.h>
#include <freebsd/sys/lock.h>
#include <freebsd/sys/malloc.h>
#include <freebsd/sys/mount.h>
#include <freebsd/sys/mutex.h>
#include <freebsd/sys/namei.h>
#include <freebsd/sys/priv.h>
#include <freebsd/sys/proc.h>

/*
 * This routine is not supported in the RTEMS interface
 */
void
funsetown(struct sigio **sigiop)
{
  /* XXX sigiop isn't supported by process for rtems */
}

/*
 * This routine is not supported in the RTEMS interface
 */
int
fsetown(pid_t pgid, struct sigio **sigiop)
{
  /* XXX sigiop isn't supported by process for rtems */
  return 0;
}
