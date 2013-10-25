/**
 * @file
 *
 * @ingroup rtems_bsd_machine
 *
 * @brief TODO.
 */

/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
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

#ifndef _RTEMS_BSD_MACHINE_RTEMS_BSD_CHUNK_H_
#define _RTEMS_BSD_MACHINE_RTEMS_BSD_CHUNK_H_

/*
 * A chunk is a fixed size memory area with some meta information attached to
 * it.  This API is used by ZONE(9).
 */

#include <sys/cdefs.h>

#include <rtems/rbtree.h>

__BEGIN_DECLS

typedef struct rtems_bsd_chunk_info rtems_bsd_chunk_info;

typedef struct rtems_bsd_chunk_control rtems_bsd_chunk_control;

typedef void (*rtems_bsd_chunk_info_ctor)(rtems_bsd_chunk_control *self,
    rtems_bsd_chunk_info *info);

typedef void (*rtems_bsd_chunk_info_dtor)(rtems_bsd_chunk_control *self,
    rtems_bsd_chunk_info *info);

struct rtems_bsd_chunk_info {
	rtems_rbtree_node node;
	uintptr_t begin;
	uintptr_t end;
};

struct rtems_bsd_chunk_control {
	rtems_rbtree_control chunks;
	uintptr_t info_size;
	rtems_bsd_chunk_info_ctor info_ctor;
	rtems_bsd_chunk_info_dtor info_dtor;
};

void rtems_bsd_chunk_init(rtems_bsd_chunk_control *self, uintptr_t info_size,
    rtems_bsd_chunk_info_ctor info_ctor, rtems_bsd_chunk_info_dtor info_dtor);

void *rtems_bsd_chunk_alloc(rtems_bsd_chunk_control *self,
    uintptr_t chunk_size);

void rtems_bsd_chunk_free(rtems_bsd_chunk_control *self,
    void *some_addr_in_chunk);

rtems_bsd_chunk_info *rtems_bsd_chunk_get_info(rtems_bsd_chunk_control *self,
    void *some_addr_in_chunk);

void *rtems_bsd_chunk_get_begin(rtems_bsd_chunk_control *self,
    void *some_addr_in_chunk);

void rtems_bsd_chunk_info_dtor_default(rtems_bsd_chunk_control *self,
    rtems_bsd_chunk_info *info);

__END_DECLS

#endif /* _RTEMS_BSD_MACHINE_RTEMS_BSD_CHUNK_H_ */
