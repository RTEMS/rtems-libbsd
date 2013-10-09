/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief This file describes the divice structure.  It was derived
 * from the libbsdport source.
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

#ifndef _RTEMS_BSD_MACHINE_RTEMS_BSD_DEVICET_H_
#define _RTEMS_BSD_MACHINE_RTEMS_BSD_DEVICET_H_

#include <rtems.h>
#include <rtems/bspIo.h>
#include <stdarg.h>
#include <stdio.h>

#include <freebsd/sys/queue.h>

typedef struct device *device_t;

typedef struct _pcidev_t {
	unsigned short bus;
	unsigned char  dev;
	unsigned char  fun;
} pcidev_t;

#define DEVICE_SOFTC_ALIGNMENT 16

struct device {
	union {
		pcidev_t	pci;
	}	     bushdr;
	int      type;
	STAILQ_ENTRY(device) list;
	const char     *name;
	char     nameunit[16];	/* NEVER use knowledge about the size of this -- we may change it */
	int      unit;
	char     *desc;
	driver_t *drv;
	int      attached;
	void     *rawmem;       /* back pointer */
	struct rtems_bsdnet_ifconfig *ifconfig;
	char softc[] __attribute__ ((aligned(DEVICE_SOFTC_ALIGNMENT), may_alias));
	/* a pointer to back to the device is installed past the 'softc' */
};
#endif
