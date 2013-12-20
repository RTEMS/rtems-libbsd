/**
 * @file
 *
 * @ingroup rtems_bsd
 *
 * @brief TODO.
 */

/*
 * Copyright (c) 2009-2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
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

#ifndef _RTEMS_BSD_BSD_H_
#define _RTEMS_BSD_BSD_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <rtems.h>

typedef enum {
	RTEMS_BSD_RES_IRQ = 1,
	RTEMS_BSD_RES_MEMORY = 3
} rtems_bsd_device_resource_type;

typedef struct {
	rtems_bsd_device_resource_type type;
	unsigned long start_request;
	unsigned long start_actual;
} rtems_bsd_device_resource;

typedef struct {
	const char *name;
	int unit;
	size_t resource_count;
	const rtems_bsd_device_resource *resources;
} rtems_bsd_device;

extern const rtems_bsd_device rtems_bsd_nexus_devices[];

extern const size_t rtems_bsd_nexus_device_count;

rtems_status_code rtems_bsd_initialize(void);

void rtems_bsd_shell_initialize(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_BSD_BSD_H_ */
