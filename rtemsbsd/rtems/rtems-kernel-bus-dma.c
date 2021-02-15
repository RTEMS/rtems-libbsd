/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief TODO.
 *
 * File origin from FreeBSD "sys/powerpc/powerpc/busdma_machdep.c".
 */

/*
 * Copyright (c) 2009-2012 embedded brains GmbH.  
 * All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * Copyright (c) 2004 Olivier Houchard
 * Copyright (c) 2002 Peter Grehan
 * Copyright (c) 1997, 1998 Justin T. Gibbs.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions, and the following disclaimer,
 *    without modification, immediately at the beginning of the file.
 * 2. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <machine/rtems-bsd-kernel-space.h>
#include <machine/rtems-bsd-cache.h>
#include <machine/rtems-bsd-bus-dma.h>

#include <rtems/malloc.h>

#include <sys/malloc.h>
#include <machine/atomic.h>

#ifdef CPU_DATA_CACHE_ALIGNMENT
  #define CLSZ ((uintptr_t) CPU_DATA_CACHE_ALIGNMENT)
  #define CLMASK (CLSZ - (uintptr_t) 1)
#endif

#ifdef __arm__
#include <bsp/linker-symbols.h>
#endif

/*
 * Convenience function for manipulating driver locks from busdma (during
 * busdma_swi, for example).  Drivers that don't provide their own locks
 * should specify &Giant to dmat->lockfuncarg.  Drivers that use their own
 * non-mutex locking scheme don't have to use this at all.
 */
void
busdma_lock_mutex(void *arg, bus_dma_lock_op_t op)
{
	struct mtx *dmtx;

	dmtx = (struct mtx *)arg;
	switch (op) {
	case BUS_DMA_LOCK:
		mtx_lock(dmtx);
		break;
	case BUS_DMA_UNLOCK:
		mtx_unlock(dmtx);
		break;
	default:
		panic("Unknown operation 0x%x for busdma_lock_mutex!", op);
	}
}

/*
 * dflt_lock should never get called.  It gets put into the dma tag when
 * lockfunc == NULL, which is only valid if the maps that are associated
 * with the tag are meant to never be defered.
 * XXX Should have a way to identify which driver is responsible here.
 */
static void
dflt_lock(void *arg, bus_dma_lock_op_t op)
{
	panic("driver error: busdma dflt_lock called");
}

/*
 * Allocate a device specific dma_tag.
 */
int
bus_dma_tag_create(bus_dma_tag_t parent, bus_size_t alignment,
    bus_size_t boundary, bus_addr_t lowaddr, bus_addr_t highaddr,
    bus_dma_filter_t *filter, void *filterarg, bus_size_t maxsize,
    int nsegments, bus_size_t maxsegsz, int flags, bus_dma_lock_t *lockfunc,
    void *lockfuncarg, bus_dma_tag_t *dmat)
{
	bus_dma_tag_t newtag;
	int error = 0;

	/* Return a NULL tag on failure */
	*dmat = NULL;

	newtag = malloc(sizeof(*newtag), M_DEVBUF, M_NOWAIT | M_ZERO);
	if (newtag == NULL)
		return (ENOMEM);

	newtag->parent = parent;
	newtag->alignment = alignment;
	newtag->boundary = boundary;
	newtag->lowaddr = lowaddr;
	newtag->highaddr = highaddr;
	newtag->filter = filter;
	newtag->filterarg = filterarg;
	newtag->maxsize = maxsize;
	newtag->nsegments = nsegments;
	newtag->maxsegsz = maxsegsz;
	newtag->flags = flags;
	newtag->ref_count = 1; /* Count ourself */
	newtag->map_count = 0;
	if (lockfunc != NULL) {
		newtag->lockfunc = lockfunc;
		newtag->lockfuncarg = lockfuncarg;
	} else {
		newtag->lockfunc = dflt_lock;
		newtag->lockfuncarg = NULL;
	}

	/*
	 * Take into account any restrictions imposed by our parent tag
	 */
	if (parent != NULL) {
		newtag->lowaddr = min(parent->lowaddr, newtag->lowaddr);
		newtag->highaddr = max(parent->highaddr, newtag->highaddr);
		if (newtag->boundary == 0)
			newtag->boundary = parent->boundary;
		else if (parent->boundary != 0)
			newtag->boundary = MIN(parent->boundary,
					       newtag->boundary);
		if (newtag->filter == NULL) {
			/*
			 * Short circuit looking at our parent directly
			 * since we have encapsulated all of its information
			 */
			newtag->filter = parent->filter;
			newtag->filterarg = parent->filterarg;
			newtag->parent = parent->parent;
		}
		if (newtag->parent != NULL)
			atomic_add_int(&parent->ref_count, 1);
	}

	*dmat = newtag;
	return (error);
}

int
bus_dma_tag_destroy(bus_dma_tag_t dmat)
{
	if (dmat != NULL) {

		if (dmat->map_count != 0)
			return (EBUSY);

		while (dmat != NULL) {
			bus_dma_tag_t parent;

			parent = dmat->parent;
			atomic_subtract_int(&dmat->ref_count, 1);
			if (dmat->ref_count == 0) {
				free(dmat, M_DEVBUF);
				/*
				 * Last reference count, so
				 * release our reference
				 * count on our parent.
				 */
				dmat = parent;
			} else
				dmat = NULL;
		}
	}
	return (0);
}

/*
 * Allocate a handle for mapping from kva/uva/physical
 * address space into bus device space.
 */
int
bus_dmamap_create(bus_dma_tag_t dmat, int flags, bus_dmamap_t *mapp)
{
	*mapp = malloc(sizeof(**mapp), M_DEVBUF, M_NOWAIT | M_ZERO);
	if (*mapp == NULL) {
		return ENOMEM;
	}

	dmat->map_count++;

	return (0);
}

/*
 * Destroy a handle for mapping from kva/uva/physical
 * address space into bus device space.
 */
int
bus_dmamap_destroy(bus_dma_tag_t dmat, bus_dmamap_t map)
{
	free(map, M_DEVBUF);

	dmat->map_count--;

	return (0);
}

/*
 * Allocate a piece of memory that can be efficiently mapped into
 * bus device space based on the constraints lited in the dma tag.
 * A dmamap to for use with dmamap_load is also allocated.
 */
int
bus_dmamem_alloc(bus_dma_tag_t dmat, void** vaddr, int flags,
    bus_dmamap_t *mapp)
{
	*mapp = malloc(sizeof(**mapp), M_DEVBUF, M_NOWAIT | M_ZERO);
	if (*mapp == NULL) {
		return ENOMEM;
	}

	if ((flags & BUS_DMA_COHERENT) != 0) {
		*vaddr = rtems_cache_coherent_allocate(
		    dmat->maxsize, dmat->alignment, dmat->boundary);
	} else {
		*vaddr = rtems_heap_allocate_aligned_with_boundary(
		    dmat->maxsize, dmat->alignment, dmat->boundary);
	}

	if (*vaddr == NULL) {
		free(*mapp, M_DEVBUF);
		return ENOMEM;
	}

	(*mapp)->buffer_begin = *vaddr;
	(*mapp)->buffer_size = dmat->maxsize;

	if ((flags & BUS_DMA_ZERO) != 0) {
		memset(*vaddr, 0, dmat->maxsize);
	}

	return (0);
}

/*
 * Free a piece of memory and it's allocated dmamap, that was allocated
 * via bus_dmamem_alloc.  Make the same choice for free/contigfree.
 */
void
bus_dmamem_free(bus_dma_tag_t dmat, void *vaddr, bus_dmamap_t map)
{
	rtems_cache_coherent_free(vaddr);
	free(map, M_DEVBUF);
}

/*
 * Utility function to load a linear buffer.  lastaddrp holds state
 * between invocations (for multiple-buffer loads).  segp contains
 * the starting segment on entrance, and the ending segment on exit.
 * first indicates if this is the first invocation of this function.
 */
int
bus_dmamap_load_buffer(bus_dma_tag_t dmat, bus_dma_segment_t segs[],
    void *buf, bus_size_t buflen, struct thread *td, int flags,
    vm_offset_t *lastaddrp, int *segp, int first)
{
	bus_size_t sgsize;
	bus_addr_t curaddr, lastaddr, baddr, bmask;
	vm_offset_t vaddr = (vm_offset_t)buf;
	int seg;

#ifdef RTEMS_BSP_PCI_MEM_REGION_BASE
	vaddr += RTEMS_BSP_PCI_MEM_REGION_BASE;
#endif

	lastaddr = *lastaddrp;
	bmask = ~(dmat->boundary - 1);

	for (seg = *segp; buflen > 0 ; ) {
		/*
		 * Get the physical address for this segment.
		 */
		curaddr = vaddr;

		/*
		 * Compute the segment size, and adjust counts.
		 */
		sgsize = PAGE_SIZE - ((u_long)curaddr & PAGE_MASK);
		if (sgsize > dmat->maxsegsz)
			sgsize = dmat->maxsegsz;
		if (buflen < sgsize)
			sgsize = buflen;

		/*
		 * Make sure we don't cross any boundaries.
		 */
		if (dmat->boundary > 0) {
			baddr = (curaddr + dmat->boundary) & bmask;
			if (sgsize > (baddr - curaddr))
				sgsize = (baddr - curaddr);
		}

		/*
		 * Insert chunk into a segment, coalescing with
		 * the previous segment if possible.
		 */
		if (first) {
			segs[seg].ds_addr = curaddr;
			segs[seg].ds_len = sgsize;
			first = 0;
		} else {
			if (curaddr == lastaddr &&
			    (segs[seg].ds_len + sgsize) <= dmat->maxsegsz &&
			    (dmat->boundary == 0 ||
			     (segs[seg].ds_addr & bmask) == (curaddr & bmask)))
				segs[seg].ds_len += sgsize;
			else {
				if (++seg >= dmat->nsegments)
					break;
				segs[seg].ds_addr = curaddr;
				segs[seg].ds_len = sgsize;
			}
		}

		lastaddr = curaddr + sgsize;
		vaddr += sgsize;
		buflen -= sgsize;
	}

	*segp = seg;
	*lastaddrp = lastaddr;

	/*
	 * Did we fit?
	 */
	return (buflen != 0 ? EFBIG : 0); /* XXX better return value here? */
}

/*
 * Map the buffer buf into bus space using the dmamap map.
 */
int
bus_dmamap_load(bus_dma_tag_t dmat, bus_dmamap_t map, void *buf,
    bus_size_t buflen, bus_dmamap_callback_t *callback,
    void *callback_arg, int flags)
{
	bus_dma_segment_t	dm_segments[dmat->nsegments];
	vm_offset_t		lastaddr;
	int			error, nsegs;

	map->buffer_begin = buf;
	map->buffer_size = buflen;
	if ((flags & BUS_DMA_DO_CACHE_LINE_BLOW_UP) != 0) {
		map->flags |= DMAMAP_CACHE_ALIGNED;
	}

	lastaddr = (vm_offset_t)0;
	nsegs = 0;

	error = bus_dmamap_load_buffer(dmat, dm_segments, buf, buflen,
	    NULL, flags, &lastaddr, &nsegs, 1);

	if (error == 0)
		(*callback)(callback_arg, dm_segments, nsegs + 1, 0);
	else
		(*callback)(callback_arg, NULL, 0, error);

	return (0);
}

/*
 * Release the mapping held by map. A no-op on PowerPC.
 */
void
bus_dmamap_unload(bus_dma_tag_t dmat, bus_dmamap_t map)
{

	return;
}

void
bus_dmamap_sync(bus_dma_tag_t dmat, bus_dmamap_t map, bus_dmasync_op_t op)
{
#ifdef CPU_DATA_CACHE_ALIGNMENT
	uintptr_t size = map->buffer_size;
	uintptr_t begin = (uintptr_t) map->buffer_begin;
	uintptr_t end = begin + size;
#ifdef __arm__
	if (begin >= (uintptr_t)bsp_section_nocache_begin &&
	    end <= (uintptr_t)bsp_section_nocachenoload_end) {
		return;
	}
#endif

	if ((map->flags & DMAMAP_CACHE_ALIGNED) != 0) {
		begin &= ~CLMASK;
		end = (end + CLMASK) & ~CLMASK;
		size = end - begin;
	}
	if ((op & BUS_DMASYNC_PREWRITE) != 0 && (op & BUS_DMASYNC_PREREAD) == 0) {
		rtems_cache_flush_multiple_data_lines((void *) begin, size);
	}
	if ((op & BUS_DMASYNC_PREREAD) != 0) {
		if ((op & BUS_DMASYNC_PREWRITE) != 0 || ((begin | size) & CLMASK) != 0) {
			rtems_cache_flush_multiple_data_lines((void *) begin, size);
		}
		rtems_cache_invalidate_multiple_data_lines((void *) begin, size);
	}
	if ((op & BUS_DMASYNC_POSTREAD) != 0) {
		char first_buf [CLSZ];
		char last_buf [CLSZ];
		bool first_is_aligned = (begin & CLMASK) == 0;
		bool last_is_aligned = (end & CLMASK) == 0;
		void *first_begin = (void *) (begin & ~CLMASK);
		size_t first_size = begin & CLMASK;
		void *last_begin = (void *) end;
		size_t last_size = CLSZ - (end & CLMASK);

		if (!first_is_aligned) {
			memcpy(first_buf, first_begin, first_size);
		}
		if (!last_is_aligned) {
			memcpy(last_buf, last_begin, last_size);
		}

		rtems_cache_invalidate_multiple_data_lines((void *) begin, size);

		if (!first_is_aligned) {
			memcpy(first_begin, first_buf, first_size);
		}
		if (!last_is_aligned) {
			memcpy(last_begin, last_buf, last_size);
		}
	}
#endif /* CPU_DATA_CACHE_ALIGNMENT */
}
