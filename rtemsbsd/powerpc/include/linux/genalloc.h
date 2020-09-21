/**************************************************************************

Copyright (c) 2007, 2008 Chelsio Inc.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.

 2. Neither the name of the Chelsio Corporation nor the names of its
    contributors may be used to endorse or promote products derived from
    this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
***************************************************************************/

#ifndef _LINUX_GENALLOC_H
#define	_LINUX_GENALLOC_H

#include <sys/blist.h>
#include <sys/malloc.h>

struct gen_pool {
	blist_t  	gen_list;
	daddr_t  	gen_base;
	int      	gen_chunk_shift;
	struct mtx 	gen_lock;
};

static inline struct gen_pool *
gen_pool_create(int min_alloc_order, int nid)
{
	struct gen_pool *gp;

	gp = malloc(sizeof(*gp), M_DEVBUF, M_NOWAIT | M_ZERO);
	if (gp == NULL)
		return (NULL);
	
	gp->gen_chunk_shift = min_alloc_order;
	mtx_init(&gp->gen_lock, "genpool", NULL, MTX_DEF);
	return (gp);
}

static inline int
gen_pool_add_virt(struct gen_pool *gp, daddr_t virt, daddr_t phys,
    size_t size, int nid)
{

	(void)phys;
	(void)nid;

	if (gp->gen_base != 0)
		return (-ENOMEM);

	gp->gen_list = blist_create(size >> gp->gen_chunk_shift, M_NOWAIT);
	if (gp->gen_list == NULL)
		return (-ENOMEM);

	gp->gen_base = virt;
	blist_free(gp->gen_list, 0, size >> gp->gen_chunk_shift);
	return (0);
}

static inline int
gen_pool_add(struct gen_pool *gp, daddr_t addr, size_t size, int nid)
{

	return (gen_pool_add_virt(gp, addr, -1, size, nid));
}

static inline daddr_t
gen_pool_alloc(struct gen_pool *gp, size_t size)
{
	int chunks;
	daddr_t blkno; 

	chunks = (size + (1 << gp->gen_chunk_shift) - 1) >> gp->gen_chunk_shift;
	mtx_lock(&gp->gen_lock);
	blkno = blist_alloc(gp->gen_list, &chunks, chunks);
	mtx_unlock(&gp->gen_lock);

	if (blkno == SWAPBLK_NONE)
		return (0);

	return (gp->gen_base + ((1 << gp->gen_chunk_shift) * blkno));
}

static inline void
gen_pool_free(struct gen_pool *gp, daddr_t address, size_t size)
{
	int chunks;
	daddr_t blkno;
	
	chunks = (size + (1<<gp->gen_chunk_shift) - 1) >> gp->gen_chunk_shift;
	blkno = (address - gp->gen_base) / (1 << gp->gen_chunk_shift);
	mtx_lock(&gp->gen_lock);
	blist_free(gp->gen_list, blkno, chunks);
	mtx_unlock(&gp->gen_lock);
}

static __inline void
gen_pool_destroy(struct gen_pool *gp)
{
	blist_destroy(gp->gen_list);
	free(gp, M_DEVBUF);
}

static inline struct gen_pool *
devm_gen_pool_create(struct device *dev, int min_alloc_order, int nid,
    const char *name)
{

	(void)dev;
	(void)name;
	return (gen_pool_create(min_alloc_order, nid));
}

#endif /* _LINUX_GENALLOC_H */
