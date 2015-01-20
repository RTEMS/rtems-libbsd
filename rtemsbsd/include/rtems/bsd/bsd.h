/**
 * @file
 *
 * @ingroup rtems_bsd
 *
 * @brief TODO.
 */

/*
 * Copyright (c) 2009-2015 embedded brains GmbH.  All rights reserved.
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

#include <sys/cdefs.h>
#include <sys/queue.h>
#include <sys/kernel.h>

#include <rtems.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

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
	const struct sysinit *driver_reference;
} rtems_bsd_device;

#define RTEMS_BSD_DEFINE_NEXUS_DEVICE(name, unit, resource_count, resources) \
    extern struct sysinit SYSINIT_ENTRY_NAME(name##_nexusmodule); \
    RTEMS_BSD_DEFINE_SET_ITEM(nexus, name##unit, rtems_bsd_device) = \
        { #name, unit, (resource_count), (resources), \
            &SYSINIT_ENTRY_NAME(name##_nexusmodule) }

rtems_status_code rtems_bsd_initialize(void);

/**
 * @brief Returns the initial priority for a task specified by its name.
 *
 * Applications may provide their own implementation of this function.  For
 * example they can define their implementation in the same module which calls
 * rtems_bsd_initialize().
 *
 * @param[in] name The task name.
 *
 * @return The desired initial task priority.
 */
rtems_task_priority rtems_bsd_get_task_priority(const char *name);

/**
 * @brief Returns the stack size for a task specified by its name.
 *
 * Applications may provide their own implementation of this function.  For
 * example they can define their implementation in the same module which calls
 * rtems_bsd_initialize().
 *
 * @param[in] name The task name.
 *
 * @return The desired task stack size.
 */
size_t rtems_bsd_get_task_stack_size(const char *name);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_BSD_BSD_H_ */
