/**
 * @file
 *
 * @ingroup rtems_bsd_machine
 *
 * @brief TODO.
 */

/*
 * Copyright (c) 2015 embedded brains GmbH.  All rights reserved.
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

#ifndef _RTEMS_BSD_MACHINE_RTEMS_BSD_PAGE_H_
#define _RTEMS_BSD_MACHINE_RTEMS_BSD_PAGE_H_

/*
 * A page is a fixed size memory area of size PAGE_SIZE with the ability to
 * associate an object with it.  The memory pool for pages has a fixed size and
 * is allocated during system initialization.  This API is intended to be used
 * by ZONE(9).
 */

#include <sys/cdefs.h>
#include <sys/param.h>

#include <stdint.h>

__BEGIN_DECLS

extern void **rtems_bsd_page_object_table;

extern uintptr_t rtems_bsd_page_area_begin;

void *rtems_bsd_page_alloc(uintptr_t size_in_bytes, int wait);

void rtems_bsd_page_free(void *addr);

void vm_pager_bufferinit(void);

static inline void **
rtems_bsd_page_get_object_entry(void *addr)
{
	uintptr_t a = (uintptr_t)addr;
	uintptr_t b = rtems_bsd_page_area_begin;
	uintptr_t s = PAGE_SHIFT;

	return (&rtems_bsd_page_object_table[(a - b) >> s]);
}

static inline void *
rtems_bsd_page_get_object(void *addr)
{
	void **obj_entry = rtems_bsd_page_get_object_entry(addr);

	return (*obj_entry);
}

static inline void
rtems_bsd_page_set_object(void *addr, void *obj)
{
	void **obj_entry = rtems_bsd_page_get_object_entry(addr);

	*obj_entry = obj;
}

__END_DECLS

#endif /* _RTEMS_BSD_MACHINE_RTEMS_BSD_PAGE_H_ */
