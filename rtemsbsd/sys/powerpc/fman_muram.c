#include <machine/rtems-bsd-kernel-space.h>
#include <rtems/bsd/local/opt_dpaa.h>

/*
 * Copyright 2008-2015 Freescale Semiconductor Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Freescale Semiconductor nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 *
 * ALTERNATIVELY, this software may be distributed under the terms of the
 * GNU General Public License ("GPL") as published by the Free Software
 * Foundation, either version 2 of that License or (at your option) any
 * later version.
 *
 * THIS SOFTWARE IS PROVIDED BY Freescale Semiconductor ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Freescale Semiconductor BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "../../../linux/drivers/net/ethernet/freescale/fman/fman_muram.h"

#include <sys/lock.h>
#include <sys/mutex.h>

#include <stdlib.h>
#include <rtems/score/heapimpl.h>

#define	MURAM_LOCK(x) (mtx_lock(&(x)->mtx))

#define	MURAM_UNLOCK(x) (mtx_unlock(&(x)->mtx))

struct muram_info {
	struct mtx mtx;
	Heap_Control heap;
	unsigned long base;
};

static unsigned long
fman_muram_vbase_to_offset(struct muram_info *muram, unsigned long addr)
{

	return (addr - muram->base);
}

struct muram_info *fman_muram_init(phys_addr_t base, size_t size)
{
	struct muram_info *muram;
	uintptr_t s;

	muram = malloc(sizeof(*muram));
	if (muram == NULL)
		return (NULL);

	muram->base = (unsigned long)base;
	memset((void *)muram->base, 0xab, size);

	s = _Heap_Initialize(&muram->heap, (void *)(uintptr_t)base, size, 64);
	if (s == 0) {
		free(muram);
		return (NULL);
	}

	mtx_init(&muram->mtx, "FMan MURAM", NULL, MTX_DEF);

	return (muram);
}

unsigned long
fman_muram_offset_to_vbase(struct muram_info *muram, unsigned long offset)
{

	return (offset + muram->base);
}

unsigned long
fman_muram_alloc(struct muram_info *muram, size_t size)
{
	void *p;

	MURAM_LOCK(muram);
	p = _Heap_Allocate(&muram->heap, size);
	MURAM_UNLOCK(muram);

	if (p == NULL)
		return -ENOMEM;

	memset(p, 0, size);

	return (fman_muram_vbase_to_offset(muram, (unsigned long)p));
}

void
fman_muram_free_mem(struct muram_info *muram, unsigned long offset, size_t size)
{
	void *p = (void *)fman_muram_offset_to_vbase(muram, offset);

	MURAM_LOCK(muram);
	_Heap_Free(&muram->heap, p);
	MURAM_UNLOCK(muram);
}
