/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief TODO.
 */

/*
 * Copyright (c) 2020 Chris Johns <chris@contemporary.software>
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

#include <machine/rtems-bsd-kernel-space.h>

#include <sys/param.h>
#include <sys/buf.h>
#include <sys/kernel.h>
#include <sys/vmem.h>

#include <vm/vm.h>
#include <vm/uma.h>
#include <vm/vm_extern.h>

#define B_MAXPHYS 0x00200000 /* nitems(b_pages[]) = atop(MAXPHYS). */
#define PBUF_PAGES (atop(maxphys) + 1)
#define MAXPHYS (128 * 1024)
#define NSWBUF_MIN 16

static u_long maxphys; /* max raw I/O transfer size */
static int nswbuf_max;

vmem_t *kernel_arena;
uma_zone_t pbuf_zone;
uma_zone_t ncl_pbuf_zone;

vm_offset_t
kva_alloc(vm_size_t size)
{
	vm_offset_t addr;

	size = round_page(size);
	if (vmem_alloc(kernel_arena, size, M_BESTFIT | M_NOWAIT, &addr))
		return (0);

	return (addr);
}

static int
pbuf_ctor(void *mem, int size, void *arg, int flags)
{
	struct buf *bp = mem;

	bp->b_vp = NULL;
	bp->b_bufobj = NULL;

	/* copied from initpbuf() */
	bp->b_rcred = NOCRED;
	bp->b_wcred = NOCRED;
	bp->b_qindex = 0; /* On no queue (QUEUE_NONE) */
	bp->b_data = bp->b_kvabase;
	bp->b_xflags = 0;
	bp->b_flags = B_MAXPHYS;
	bp->b_ioflags = 0;
	bp->b_iodone = NULL;
	bp->b_error = 0;
	BUF_LOCK(bp, LK_EXCLUSIVE, NULL);

	return (0);
}

static void
pbuf_dtor(void *mem, int size, void *arg)
{
	struct buf *bp = mem;

	if (bp->b_rcred != NOCRED) {
		crfree(bp->b_rcred);
		bp->b_rcred = NOCRED;
	}
	if (bp->b_wcred != NOCRED) {
		crfree(bp->b_wcred);
		bp->b_wcred = NOCRED;
	}

	BUF_UNLOCK(bp);
}

static int
pbuf_init(void *mem, int size, int flags)
{
	struct buf *bp = mem;

	bp->b_kvabase = (void *)kva_alloc(ptoa(PBUF_PAGES));
	if (bp->b_kvabase == NULL)
		return (ENOMEM);
	bp->b_kvasize = ptoa(PBUF_PAGES);
	BUF_LOCKINIT(bp);
	LIST_INIT(&bp->b_dep);
	bp->b_rcred = bp->b_wcred = NOCRED;
	bp->b_xflags = 0;

	return (0);
}

void
vm_pager_bufferinit(void)
{
	/* Main zone for paging bufs. */
	pbuf_zone = uma_zcreate("pbuf",
	    sizeof(struct buf) + PBUF_PAGES * sizeof(vm_page_t), pbuf_ctor,
	    pbuf_dtor, pbuf_init, NULL, UMA_ALIGN_CACHE, UMA_ZONE_NOFREE);
	/* Few systems may still use this zone directly, so it needs a limit. */
	nswbuf_max += uma_zone_set_max(pbuf_zone, NSWBUF_MIN);
}

uma_zone_t
pbuf_zsecond_create(char *name, int max)
{
	uma_zone_t zone;

	zone = uma_zsecond_create(
	    name, pbuf_ctor, pbuf_dtor, NULL, NULL, pbuf_zone);
	/*
	 * uma_prealloc() rounds up to items per slab. If we would prealloc
	 * immediately on every pbuf_zsecond_create(), we may accumulate too
	 * much of difference between hard limit and prealloced items, which
	 * means wasted memory.
	 */
	if (nswbuf_max > 0)
		nswbuf_max += uma_zone_set_max(zone, max);
	else
		uma_prealloc(pbuf_zone, uma_zone_set_max(zone, max));

	return (zone);
}

struct buf *
getpbuf(int *pfreecnt)
{
	(void)pfreecnt;
	return uma_zalloc(ncl_pbuf_zone, M_WAITOK);
}

void
relpbuf(struct buf *bp, int *pfreecnt)
{
	(void)pfreecnt;
	uma_zfree(ncl_pbuf_zone, bp);
}
