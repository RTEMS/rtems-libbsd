/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
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

#include <machine/rtems-bsd-kernel-space.h>
#include <machine/rtems-bsd-page.h>

#include <sys/param.h>
#include <sys/types.h>
#include <sys/lock.h>
#include <sys/mutex.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <vm/uma.h>

#include <stdlib.h>

#include <rtems/bsd/bsd.h>
#include <rtems/malloc.h>
#include <rtems/rbheap.h>

void **rtems_bsd_page_object_table;

uintptr_t rtems_bsd_page_area_begin;

static struct {
	struct mtx mtx;
	rtems_rbheap_control heap;
	size_t used;
} page_alloc;

void *
rtems_bsd_page_alloc(uintptr_t size_in_bytes, int wait)
{
	void *addr;

	mtx_lock(&page_alloc.mtx);

	addr = rtems_rbheap_allocate(&page_alloc.heap, size_in_bytes);
	if (addr == NULL && wait) {
		int i;

		for (i = 0; i < 8; i++) {
			mtx_unlock(&page_alloc.mtx);
			uma_reclaim();
			mtx_lock(&page_alloc.mtx);

			addr = rtems_rbheap_allocate(&page_alloc.heap,
			    size_in_bytes);
			if (addr != NULL)
				break;

			msleep(&page_alloc.heap, &page_alloc.mtx, 0,
			    "page alloc", (hz / 4) * (i + 1));
		}

		if (i == 8) {
			panic("rtems_bsd_page_alloc: page starvation");
		}
	}

	page_alloc.used += (addr != NULL) ? 1 : 0;
	mtx_unlock(&page_alloc.mtx);

#ifdef INVARIANTS
	wait |= M_ZERO;
#endif

	if (addr != NULL && (wait & M_ZERO) != 0) {
		memset(addr, 0, size_in_bytes);
	}

	return (addr);
}

void
rtems_bsd_page_free(void *addr)
{

	mtx_lock(&page_alloc.mtx);
	--page_alloc.used;
	rtems_rbheap_free(&page_alloc.heap, addr);
	wakeup(&page_alloc.heap);
	mtx_unlock(&page_alloc.mtx);
}

static void
rtems_bsd_page_init(void *arg)
{
	rtems_status_code sc;
	void *area;
	void **obj_table;
	rtems_rbheap_chunk *chunks;
	size_t i;
	size_t n;
	uintptr_t heap_size;

	mtx_init(&page_alloc.mtx, "page heap", NULL, MTX_DEF);

	heap_size = rtems_bsd_get_allocator_domain_size(
	    RTEMS_BSD_ALLOCATOR_DOMAIN_PAGE);

	area = rtems_heap_allocate_aligned_with_boundary(heap_size, PAGE_SIZE,
	    0);
	BSD_ASSERT(area != NULL);

	sc = rtems_rbheap_initialize(&page_alloc.heap, area, heap_size,
	    PAGE_SIZE, rtems_rbheap_extend_descriptors_with_malloc, NULL);
	BSD_ASSERT(sc == RTEMS_SUCCESSFUL);

	rtems_rbheap_set_extend_descriptors(&page_alloc.heap,
	    rtems_rbheap_extend_descriptors_never);

	n = heap_size / PAGE_SIZE;

	chunks = malloc(n * sizeof(*chunks), M_RTEMS_HEAP, M_NOWAIT);
	BSD_ASSERT(chunks != NULL);

	for (i = 0; i < n; ++i) {
		rtems_rbheap_add_to_spare_descriptor_chain(&page_alloc.heap,
		    &chunks[i]);
	}

	obj_table = calloc(n, sizeof(*obj_table));

	rtems_bsd_page_area_begin = (uintptr_t)area;
	rtems_bsd_page_object_table = obj_table;
}

SYSINIT(rtems_bsd_page, SI_SUB_VM, SI_ORDER_FIRST, rtems_bsd_page_init, NULL);
