/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief This file is an rtems representation of needed methods from
 * the FreeBSD file kern_conf.c
 */

/*
 * COPYRIGHT (c) 2012. On-Line Applications Research Corporation (OAR).
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
#include <stdio.h>
#include <rtems.h>

__FBSDID("$FreeBSD$");

#include <freebsd/sys/param.h>
#include <freebsd/sys/conf.h>

struct cdev *
make_dev(struct cdevsw *devsw, int unit, uid_t uid, gid_t gid, int mode,
    const char *fmt, ...)
{
        struct cdev *dev;
	dev = malloc( sizeof(struct cdev) );

	/* Initialize the elements that rtems uses. */
	dev->si_flags = 0;
	dev->si_drv0 = unit;
	dev->si_drv1 = NULL;
	dev->si_drv2 = NULL;

        return (dev);
}

void
destroy_dev(struct cdev *dev)
{
	free (dev);
}
