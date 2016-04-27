#include <machine/rtems-bsd-kernel-space.h>

#include <rtems/bsd/local/opt_dpaa.h>

/* Copyright 2008 - 2015 Freescale Semiconductor, Inc.
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

#include "qman_priv.h"

/* --- FQID Pool --- */

struct qman_fqid_pool {
	/* Base and size of the FQID range */
	u32 fqid_base;
	u32 total;
	/* Number of FQIDs currently "allocated" */
	u32 used;
	/* Allocation optimisation. When 'used<total', it is the index of an
	 * available FQID. Otherwise there are no available FQIDs, and this
	 * will be set when the next deallocation occurs. */
	u32 next;
	/* A bit-field representation of the FQID range. */
	unsigned long *bits;
};

#define QLONG_BYTES	sizeof(unsigned long)
#define QLONG_BITS	(QLONG_BYTES * 8)
/* Number of 'longs' required for the given number of bits */
#define QNUM_LONGS(b)	(((b) + QLONG_BITS - 1) / QLONG_BITS)
/* Shorthand for the number of bytes of same (kmalloc, memset, etc) */
#define QNUM_BYTES(b)	(QNUM_LONGS(b) * QLONG_BYTES)
/* And in bits */
#define QNUM_BITS(b)	(QNUM_LONGS(b) * QLONG_BITS)

struct qman_fqid_pool *qman_fqid_pool_create(u32 fqid_start, u32 num)
{
	struct qman_fqid_pool *pool = kmalloc(sizeof(*pool), GFP_KERNEL);
	unsigned int i;

	BUG_ON(!num);
	if (!pool)
		return NULL;
	pool->fqid_base = fqid_start;
	pool->total = num;
	pool->used = 0;
	pool->next = 0;
	pool->bits = kzalloc(QNUM_BYTES(num), GFP_KERNEL);
	if (!pool->bits) {
		kfree(pool);
		return NULL;
	}
	/* If num is not an even multiple of QLONG_BITS (or even 8, for
	 * byte-oriented searching) then we fill the trailing bits with 1, to
	 * make them look allocated (permanently). */
	for (i = num + 1; i < QNUM_BITS(num); i++)
		set_bit(i, pool->bits);
	return pool;
}
EXPORT_SYMBOL(qman_fqid_pool_create);

int qman_fqid_pool_destroy(struct qman_fqid_pool *pool)
{
	int ret = pool->used;

	kfree(pool->bits);
	kfree(pool);
	return ret;
}
EXPORT_SYMBOL(qman_fqid_pool_destroy);

int qman_fqid_pool_alloc(struct qman_fqid_pool *pool, u32 *fqid)
{
	int ret;

	if (pool->used == pool->total)
		return -ENOMEM;
	*fqid = pool->fqid_base + pool->next;
	ret = test_and_set_bit(pool->next, pool->bits);
	BUG_ON(ret);
	if (++pool->used == pool->total)
		return 0;
	pool->next = find_next_zero_bit(pool->bits, pool->total, pool->next);
	if (pool->next >= pool->total)
		pool->next = find_first_zero_bit(pool->bits, pool->total);
	BUG_ON(pool->next >= pool->total);
	return 0;
}
EXPORT_SYMBOL(qman_fqid_pool_alloc);

void qman_fqid_pool_free(struct qman_fqid_pool *pool, u32 fqid)
{
	int ret;

	fqid -= pool->fqid_base;
	ret = test_and_clear_bit(fqid, pool->bits);
	BUG_ON(!ret);
	if (pool->used-- == pool->total)
		pool->next = fqid;
}
EXPORT_SYMBOL(qman_fqid_pool_free);

u32 qman_fqid_pool_used(struct qman_fqid_pool *pool)
{
	return pool->used;
}
EXPORT_SYMBOL(qman_fqid_pool_used);

static DECLARE_DPAA_RESOURCE(fqalloc); /* FQID allocator */
static DECLARE_DPAA_RESOURCE(qpalloc); /* pool-channel allocator */
static DECLARE_DPAA_RESOURCE(cgralloc); /* CGR ID allocator */

/* FQID allocator front-end */

int qman_alloc_fqid_range(u32 *result, u32 count, u32 align, int partial)
{
	return dpaa_resource_new(&fqalloc, result, count, align, partial);
}
EXPORT_SYMBOL(qman_alloc_fqid_range);

static int fq_cleanup(u32 fqid)
{
	return qman_shutdown_fq(fqid) == 0;
}

void qman_release_fqid_range(u32 fqid, u32 count)
{
	u32 total_invalid = dpaa_resource_release(&fqalloc,
						  fqid, count, fq_cleanup);

	if (total_invalid)
		pr_err("FQID range [%d..%d] (%d) had %d leaks\n",
			fqid, fqid + count - 1, count, total_invalid);
}
EXPORT_SYMBOL(qman_release_fqid_range);

int qman_reserve_fqid_range(u32 fqid, u32 count)
{
	return dpaa_resource_reserve(&fqalloc, fqid, count);
}
EXPORT_SYMBOL(qman_reserve_fqid_range);

void qman_seed_fqid_range(u32 fqid, u32 count)
{
	dpaa_resource_seed(&fqalloc, fqid, count);
}
EXPORT_SYMBOL(qman_seed_fqid_range);

/* Pool-channel allocator front-end */

int qman_alloc_pool_range(u32 *result, u32 count, u32 align, int partial)
{
	return dpaa_resource_new(&qpalloc, result, count, align, partial);
}
EXPORT_SYMBOL(qman_alloc_pool_range);

static int qpool_cleanup(u32 qp)
{
	/* We query all FQDs starting from
	 * FQID 1 until we get an "invalid FQID" error, looking for non-OOS FQDs
	 * whose destination channel is the pool-channel being released.
	 * When a non-OOS FQD is found we attempt to clean it up */
	struct qman_fq fq = {
		.fqid = 1
	};
	int err;

	do {
		struct qm_mcr_queryfq_np np;

		err = qman_query_fq_np(&fq, &np);
		if (err)
			/* FQID range exceeded, found no problems */
			return 1;
		if ((np.state & QM_MCR_NP_STATE_MASK) != QM_MCR_NP_STATE_OOS) {
			struct qm_fqd fqd;

			err = qman_query_fq(&fq, &fqd);
			BUG_ON(err);
			if (fqd.dest.channel == qp) {
				/* The channel is the FQ's target, clean it */
				if (qman_shutdown_fq(fq.fqid) != 0)
					/* Couldn't shut down the FQ
					   so the pool must be leaked */
					return 0;
			}
		}
		/* Move to the next FQID */
		fq.fqid++;
	} while (1);
}

void qman_release_pool_range(u32 qp, u32 count)
{
	u32 total_invalid = dpaa_resource_release(&qpalloc,
						  qp, count, qpool_cleanup);

	if (total_invalid) {
		/* Pool channels are almost always used individually */
		if (count == 1)
			pr_err("Pool channel 0x%x had %d leaks\n",
				qp, total_invalid);
		else
			pr_err("Pool channels [%d..%d] (%d) had %d leaks\n",
				qp, qp + count - 1, count, total_invalid);
	}
}
EXPORT_SYMBOL(qman_release_pool_range);

void qman_seed_pool_range(u32 poolid, u32 count)
{
	dpaa_resource_seed(&qpalloc, poolid, count);

}
EXPORT_SYMBOL(qman_seed_pool_range);

int qman_reserve_pool_range(u32 poolid, u32 count)
{
	return dpaa_resource_reserve(&qpalloc, poolid, count);
}
EXPORT_SYMBOL(qman_reserve_pool_range);


/* CGR ID allocator front-end */

int qman_alloc_cgrid_range(u32 *result, u32 count, u32 align, int partial)
{
	return dpaa_resource_new(&cgralloc, result, count, align, partial);
}
EXPORT_SYMBOL(qman_alloc_cgrid_range);

static int cqr_cleanup(u32 cgrid)
{
	/* We query all FQDs starting from
	 * FQID 1 until we get an "invalid FQID" error, looking for non-OOS FQDs
	 * whose CGR is the CGR being released.
	 */
	struct qman_fq fq = {
		.fqid = 1
	};
	int err;

	do {
		struct qm_mcr_queryfq_np np;

		err = qman_query_fq_np(&fq, &np);
		if (err)
			/* FQID range exceeded, found no problems */
			return 1;
		if ((np.state & QM_MCR_NP_STATE_MASK) != QM_MCR_NP_STATE_OOS) {
			struct qm_fqd fqd;

			err = qman_query_fq(&fq, &fqd);
			BUG_ON(err);
			if ((fqd.fq_ctrl & QM_FQCTRL_CGE) &&
			    (fqd.cgid == cgrid)) {
				pr_err("CRGID 0x%x is being used by FQID 0x%x,"
				       " CGR will be leaked\n",
				       cgrid, fq.fqid);
				return 1;
			}
		}
		/* Move to the next FQID */
		fq.fqid++;
	} while (1);
}

void qman_release_cgrid_range(u32 cgrid, u32 count)
{
	u32 total_invalid = dpaa_resource_release(&cgralloc,
						  cgrid, count, cqr_cleanup);
	if (total_invalid)
		pr_err("CGRID range [%d..%d] (%d) had %d leaks\n",
			cgrid, cgrid + count - 1, count, total_invalid);
}
EXPORT_SYMBOL(qman_release_cgrid_range);

void qman_seed_cgrid_range(u32 cgrid, u32 count)
{
	dpaa_resource_seed(&cgralloc, cgrid, count);

}
EXPORT_SYMBOL(qman_seed_cgrid_range);
