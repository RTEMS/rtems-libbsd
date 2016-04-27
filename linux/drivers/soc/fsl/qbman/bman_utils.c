#include <machine/rtems-bsd-kernel-space.h>

#include <rtems/bsd/local/opt_dpaa.h>

/* Copyright 2009 - 2015 Freescale Semiconductor, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *	 notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *	 notice, this list of conditions and the following disclaimer in the
 *	 documentation and/or other materials provided with the distribution.
 *     * Neither the name of Freescale Semiconductor nor the
 *	 names of its contributors may be used to endorse or promote products
 *	 derived from this software without specific prior written permission.
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

#include "dpaa_sys.h"

#include <soc/fsl/bman.h>

/* BMan APIs are front-ends to the common code */

static DECLARE_DPAA_RESOURCE(bpalloc); /* BPID allocator */

/* BPID allocator front-end */

int bman_alloc_bpid_range(u32 *result, u32 count, u32 align, int partial)
{
	return dpaa_resource_new(&bpalloc, result, count, align, partial);
}
EXPORT_SYMBOL(bman_alloc_bpid_range);

static int bp_cleanup(u32 bpid)
{
	return bman_shutdown_pool(bpid) == 0;
}
void bman_release_bpid_range(u32 bpid, u32 count)
{
	u32 total_invalid = dpaa_resource_release(&bpalloc,
						  bpid, count, bp_cleanup);

	if (total_invalid)
		pr_err("BPID range [%d..%d] (%d) had %d leaks\n",
			bpid, bpid + count - 1, count, total_invalid);
}
EXPORT_SYMBOL(bman_release_bpid_range);

void bman_seed_bpid_range(u32 bpid, u32 count)
{
	dpaa_resource_seed(&bpalloc, bpid, count);
}
EXPORT_SYMBOL(bman_seed_bpid_range);

int bman_reserve_bpid_range(u32 bpid, u32 count)
{
	return dpaa_resource_reserve(&bpalloc, bpid, count);
}
EXPORT_SYMBOL(bman_reserve_bpid_range);
