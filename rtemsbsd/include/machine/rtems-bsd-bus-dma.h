/**
 * @file
 *
 * @ingroup rtems_bsd_machine
 *
 * @brief TODO.
 *
 * File origin from FreeBSD "sys/powerpc/powerpc/busdma_machdep.c".
 */

/*-
 * Copyright (c) 2009-2012 embedded brains GmbH.  All rights reserved.
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

#ifndef _RTEMS_BSD_MACHINE_RTEMS_BSD_BUS_DMA_H_
#define _RTEMS_BSD_MACHINE_RTEMS_BSD_BUS_DMA_H_

#include <sys/param.h>
#include <sys/types.h>
#include <sys/lock.h>
#include <sys/mutex.h>
#include <sys/systm.h>
#include <machine/bus.h>

struct bus_dma_tag {
	bus_dma_tag_t     parent;
	bus_size_t	alignment;
	bus_size_t	boundary;
	bus_addr_t	lowaddr;
	bus_addr_t	highaddr;
	bus_dma_filter_t *filter;
	void	     *filterarg;
	bus_size_t	maxsize;
	int	       nsegments;
	bus_size_t	maxsegsz;
	int	       flags;
	int	       ref_count;
	int	       map_count;
	bus_dma_lock_t	 *lockfunc;
	void		 *lockfuncarg;
};

struct bus_dmamap {
	void *buffer_begin;
	bus_size_t buffer_size;
};

int
bus_dmamap_load_buffer(bus_dma_tag_t dmat, bus_dma_segment_t segs[],
    void *buf, bus_size_t buflen, struct thread *td, int flags,
    vm_offset_t *lastaddrp, int *segp, int first);

#endif /* _RTEMS_BSD_MACHINE_RTEMS_BSD_BUS_DMA_H_ */
