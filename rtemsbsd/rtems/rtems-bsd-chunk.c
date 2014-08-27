/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
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

#include <machine/rtems-bsd-kernel-space.h>
#include <machine/rtems-bsd-chunk.h>

#include <rtems/bsd/sys/param.h>
#include <sys/malloc.h>

#include <rtems/score/apimutex.h>

#define chunk_of_node(n) ((rtems_bsd_chunk_info *) n)

static rtems_rbtree_compare_result
chunk_compare(const rtems_rbtree_node *a, const rtems_rbtree_node *b)
{
	const rtems_bsd_chunk_info *left = chunk_of_node(a);
	const rtems_bsd_chunk_info *right = chunk_of_node(b);

	if (left->begin < right->begin) {
		return -1;
	} else if (left->begin < right->end) {
		return 0;
	} else {
		return 1;
	}
}

void
rtems_bsd_chunk_init(rtems_bsd_chunk_control *self, uintptr_t info_size,
    rtems_bsd_chunk_info_ctor info_ctor, rtems_bsd_chunk_info_dtor info_dtor)
{
	info_size = roundup(info_size, CPU_HEAP_ALIGNMENT);

	self->info_size = info_size;
	self->info_ctor = info_ctor;
	self->info_dtor = info_dtor;
	rtems_rbtree_initialize_empty(&self->chunks);
}

void *
rtems_bsd_chunk_alloc(rtems_bsd_chunk_control *self, uintptr_t chunk_size)
{
	char *p = malloc(chunk_size + self->info_size, M_TEMP, M_WAITOK);

	if (p != NULL) {
		rtems_bsd_chunk_info *info = (rtems_bsd_chunk_info *) p;

		p += self->info_size;

		info->begin = (uintptr_t) p;
		info->end = (uintptr_t) p + chunk_size;

		(*self->info_ctor)(self, info);

		_RTEMS_Lock_allocator();
		rtems_rbtree_insert(&self->chunks, &info->node, chunk_compare, true);
		_RTEMS_Unlock_allocator();
	}

	return p;
}

void
rtems_bsd_chunk_free(rtems_bsd_chunk_control *self,
    void *some_addr_in_chunk)
{
	rtems_bsd_chunk_info *info = rtems_bsd_chunk_get_info(self,
	    some_addr_in_chunk);

	_RTEMS_Lock_allocator();
	rtems_rbtree_extract(&self->chunks, &info->node);
	_RTEMS_Unlock_allocator();

	(*self->info_dtor)(self, info);

	free(info, M_TEMP);
}

rtems_bsd_chunk_info *
rtems_bsd_chunk_get_info(rtems_bsd_chunk_control *self,
    void *some_addr_in_chunk)
{
	rtems_bsd_chunk_info find_me = {
		.begin = (uintptr_t) some_addr_in_chunk
	};

	return chunk_of_node(rtems_rbtree_find(&self->chunks,
	    &find_me.node, chunk_compare, true));
}

void *
rtems_bsd_chunk_get_begin(rtems_bsd_chunk_control *self,
    void *some_addr_in_chunk)
{
	rtems_bsd_chunk_info *info = rtems_bsd_chunk_get_info(self,
	    some_addr_in_chunk);

	return (void *) info->begin;
}

void
rtems_bsd_chunk_info_dtor_default(rtems_bsd_chunk_control *self,
    rtems_bsd_chunk_info *info)
{
	(void) self;
	(void) info;
}
