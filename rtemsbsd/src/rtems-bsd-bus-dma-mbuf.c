/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief TODO.
 *
 * File origin from FreeBSD "sys/powerpc/powerpc/busdma_machdep.c".
 */

/*-
 * Copyright (c) 2012 embedded brains GmbH.  
 * All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
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

#include <freebsd/machine/rtems-bsd-config.h>
#include <freebsd/machine/rtems-bsd-bus-dma.h>

#include <freebsd/sys/mbuf.h>

/*
 * Like bus_dmamap_load(), but for mbufs.
 */
int
bus_dmamap_load_mbuf(bus_dma_tag_t dmat, bus_dmamap_t map,
		     struct mbuf *m0,
		     bus_dmamap_callback2_t *callback, void *callback_arg,
		     int flags)
{
	bus_dma_segment_t dm_segments[dmat->nsegments];
	int nsegs, error;

	M_ASSERTPKTHDR(m0);

	flags |= BUS_DMA_NOWAIT;
	nsegs = 0;
	error = 0;
	if (m0->m_pkthdr.len <= dmat->maxsize) {
		int first = 1;
		bus_addr_t lastaddr = 0;
		struct mbuf *m;

		for (m = m0; m != NULL && error == 0; m = m->m_next) {
			if (m->m_len > 0) {
				error = bus_dmamap_load_buffer(dmat, dm_segments,
						m->m_data, m->m_len,
						NULL, flags, &lastaddr,
						&nsegs, first);
				first = 0;
			}
		}
	} else {
		error = EINVAL;
	}

	if (error) {
		/* force "no valid mappings" in callback */
		(*callback)(callback_arg, dm_segments, 0, 0, error);
	} else {
		(*callback)(callback_arg, dm_segments,
			    nsegs+1, m0->m_pkthdr.len, error);
	}
	return (error);
}

int
bus_dmamap_load_mbuf_sg(bus_dma_tag_t dmat, bus_dmamap_t map,
			struct mbuf *m0, bus_dma_segment_t *segs, int *nsegs,
			int flags)
{
	int error;

	M_ASSERTPKTHDR(m0);

	flags |= BUS_DMA_NOWAIT;
	*nsegs = 0;
	error = 0;
	if (m0->m_pkthdr.len <= dmat->maxsize) {
		int first = 1;
		bus_addr_t lastaddr = 0;
		struct mbuf *m;

		for (m = m0; m != NULL && error == 0; m = m->m_next) {
			if (m->m_len > 0) {
				error = bus_dmamap_load_buffer(dmat, segs,
						m->m_data, m->m_len,
						NULL, flags, &lastaddr,
						nsegs, first);
				first = 0;
			}
		}
	} else {
		error = EINVAL;
	}

	/* XXX FIXME: Having to increment nsegs is really annoying */
	++*nsegs;
	return (error);
}
