#include <machine/rtems-bsd-kernel-space.h>

#include <rtems/bsd/local/opt_dpaa.h>

/* Copyright 2012 - 2015 Freescale Semiconductor Inc.
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

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/init.h>
#include <linux/skbuff.h>
#include <linux/highmem.h>
#include <soc/fsl/bman.h>

#include "dpaa_eth.h"
#include "dpaa_eth_common.h"

/* Convenience macros for storing/retrieving the skb back-pointers.
 *
 * NB: @off is an offset from a (struct sk_buff **) pointer!
 */
#define DPA_WRITE_SKB_PTR(skb, skbh, addr, off) \
	{ \
		skbh = (struct sk_buff **)addr; \
		*(skbh + (off)) = skb; \
	}
#define DPA_READ_SKB_PTR(skb, skbh, addr, off) \
	{ \
		skbh = (struct sk_buff **)addr; \
		skb = *(skbh + (off)); \
	}

/* DMA map and add a page frag back into the bpool.
 * @vaddr fragment must have been allocated with netdev_alloc_frag(),
 * specifically for fitting into @dpa_bp.
 */
static void dpa_bp_recycle_frag(struct dpa_bp *dpa_bp, unsigned long vaddr,
				int *count_ptr)
{
	struct bm_buffer bmb;
	dma_addr_t addr;

	addr = dma_map_single(dpa_bp->dev, (void *)vaddr, dpa_bp->size,
			      DMA_BIDIRECTIONAL);
	if (unlikely(dma_mapping_error(dpa_bp->dev, addr))) {
		dev_err(dpa_bp->dev, "DMA mapping failed");
		return;
	}

	bm_buffer_set64(&bmb, addr);

	while (bman_release(dpa_bp->pool, &bmb, 1, 0))
		cpu_relax();

	(*count_ptr)++;
}

static int _dpa_bp_add_8_bufs(const struct dpa_bp *dpa_bp)
{
	struct bm_buffer bmb[8];
	void *new_buf;
	dma_addr_t addr;
	u8 i;
	struct device *dev = dpa_bp->dev;
	struct sk_buff *skb, **skbh;

	memset(bmb, 0, sizeof(bmb));

	for (i = 0; i < 8; i++) {
		/* We'll prepend the skb back-pointer; can't use the DPA
		 * priv space, because FMan will overwrite it (from offset 0)
		 * if it ends up being the second, third, etc. fragment
		 * in a S/G frame.
		 *
		 * We only need enough space to store a pointer, but allocate
		 * an entire cacheline for performance reasons.
		 */
		new_buf = netdev_alloc_frag(SMP_CACHE_BYTES + DPA_BP_RAW_SIZE);
		if (unlikely(!new_buf))
			goto netdev_alloc_failed;
		new_buf = PTR_ALIGN(new_buf + SMP_CACHE_BYTES, SMP_CACHE_BYTES);

		skb = build_skb(new_buf, DPA_SKB_SIZE(dpa_bp->size) +
			SKB_DATA_ALIGN(sizeof(struct skb_shared_info)));
		if (unlikely(!skb)) {
			put_page(virt_to_head_page(new_buf));
			goto build_skb_failed;
		}
		DPA_WRITE_SKB_PTR(skb, skbh, new_buf, -1);

		addr = dma_map_single(dev, new_buf,
				      dpa_bp->size, DMA_BIDIRECTIONAL);
		if (unlikely(dma_mapping_error(dev, addr)))
			goto dma_map_failed;

		bm_buffer_set64(&bmb[i], addr);
	}

release_bufs:
	/* Release the buffers. In case bman is busy, keep trying
	 * until successful. bman_release() is guaranteed to succeed
	 * in a reasonable amount of time
	 */
	while (unlikely(bman_release(dpa_bp->pool, bmb, i, 0)))
		cpu_relax();
	return i;

dma_map_failed:
	kfree_skb(skb);

build_skb_failed:
netdev_alloc_failed:
	net_err_ratelimited("dpa_bp_add_8_bufs() failed\n");
	WARN_ONCE(1, "Memory allocation failure on Rx\n");

	bm_buffer_set64(&bmb[i], 0);
	/* Avoid releasing a completely null buffer; bman_release() requires
	 * at least one buffer.
	 */
	if (likely(i))
		goto release_bufs;

	return 0;
}

/* Cold path wrapper over _dpa_bp_add_8_bufs(). */
static void dpa_bp_add_8_bufs(const struct dpa_bp *dpa_bp, int cpu)
{
	int *count_ptr = per_cpu_ptr(dpa_bp->percpu_count, cpu);
	*count_ptr += _dpa_bp_add_8_bufs(dpa_bp);
}

int dpa_bp_priv_seed(struct dpa_bp *dpa_bp)
{
	int i;

	/* Give each CPU an allotment of "config_count" buffers */
	for_each_possible_cpu(i) {
		int j;

		/* Although we access another CPU's counters here
		 * we do it at boot time so it is safe
		 */
		for (j = 0; j < dpa_bp->config_count; j += 8)
			dpa_bp_add_8_bufs(dpa_bp, i);
	}
	return 0;
}

/* Add buffers/(pages) for Rx processing whenever bpool count falls below
 * REFILL_THRESHOLD.
 */
int dpaa_eth_refill_bpools(struct dpa_bp *dpa_bp, int *countptr)
{
	int count = *countptr;
	int new_bufs;

	if (unlikely(count < FSL_DPAA_ETH_REFILL_THRESHOLD)) {
		do {
			new_bufs = _dpa_bp_add_8_bufs(dpa_bp);
			if (unlikely(!new_bufs)) {
				/* Avoid looping forever if we've temporarily
				 * run out of memory. We'll try again at the
				 * next NAPI cycle.
				 */
				break;
			}
			count += new_bufs;
		} while (count < FSL_DPAA_ETH_MAX_BUF_COUNT);

		*countptr = count;
		if (unlikely(count < FSL_DPAA_ETH_MAX_BUF_COUNT))
			return -ENOMEM;
	}

	return 0;
}

/* Cleanup function for outgoing frame descriptors that were built on Tx path,
 * either contiguous frames or scatter/gather ones.
 * Skb freeing is not handled here.
 *
 * This function may be called on error paths in the Tx function, so guard
 * against cases when not all fd relevant fields were filled in.
 *
 * Return the skb backpointer, since for S/G frames the buffer containing it
 * gets freed here.
 */
struct sk_buff *_dpa_cleanup_tx_fd(const struct dpa_priv_s *priv,
				   const struct qm_fd *fd)
{
	const struct qm_sg_entry *sgt;
	int i;
	struct dpa_bp *dpa_bp = priv->dpa_bp;
	dma_addr_t addr = qm_fd_addr(fd);
	struct sk_buff **skbh;
	struct sk_buff *skb = NULL;
	const enum dma_data_direction dma_dir = DMA_TO_DEVICE;
	int nr_frags;


	/* retrieve skb back pointer */
	DPA_READ_SKB_PTR(skb, skbh, phys_to_virt(addr), 0);

	if (unlikely(fd->format == qm_fd_sg)) {
		nr_frags = skb_shinfo(skb)->nr_frags;
		dma_unmap_single(dpa_bp->dev, addr, dpa_fd_offset(fd) +
				 sizeof(struct qm_sg_entry) * (1 + nr_frags),
				 dma_dir);

		/* The sgt buffer has been allocated with netdev_alloc_frag(),
		 * it's from lowmem.
		 */
		sgt = phys_to_virt(addr + dpa_fd_offset(fd));

		/* sgt[0] is from lowmem, was dma_map_single()-ed */
		dma_unmap_single(dpa_bp->dev, (dma_addr_t)sgt[0].addr,
				 sgt[0].length, dma_dir);

		/* remaining pages were mapped with dma_map_page() */
		for (i = 1; i < nr_frags; i++) {
			DPA_ERR_ON(sgt[i].extension);

			dma_unmap_page(dpa_bp->dev, (dma_addr_t)sgt[i].addr,
				       sgt[i].length, dma_dir);
		}

		/* Free the page frag that we allocated on Tx */
		put_page(virt_to_head_page(sgt));
	} else {
		dma_unmap_single(dpa_bp->dev, addr,
				 skb_tail_pointer(skb) - (u8 *)skbh, dma_dir);
	}

	return skb;
}

/* Build a linear skb around the received buffer.
 * We are guaranteed there is enough room at the end of the data buffer to
 * accommodate the shared info area of the skb.
 */
static struct sk_buff *contig_fd_to_skb(const struct dpa_priv_s *priv,
					const struct qm_fd *fd)
{
	struct sk_buff *skb = NULL, **skbh;
	ssize_t fd_off = dpa_fd_offset(fd);
	dma_addr_t addr = qm_fd_addr(fd);
	void *vaddr;

	vaddr = phys_to_virt(addr);
	DPA_ERR_ON(!IS_ALIGNED((unsigned long)vaddr, SMP_CACHE_BYTES));

	/* Retrieve the skb and adjust data and tail pointers, to make sure
	 * forwarded skbs will have enough space on Tx if extra headers
	 * are added.
	 */
	DPA_READ_SKB_PTR(skb, skbh, vaddr, -1);

	DPA_ERR_ON(fd_off != priv->rx_headroom);
	skb_reserve(skb, fd_off);
	skb_put(skb, dpa_fd_length(fd));

	skb->ip_summed = CHECKSUM_NONE;

	return skb;
}

/* Build an skb with the data of the first S/G entry in the linear portion and
 * the rest of the frame as skb fragments.
 *
 * The page fragment holding the S/G Table is recycled here.
 */
static struct sk_buff *sg_fd_to_skb(const struct dpa_priv_s *priv,
				    const struct qm_fd *fd,
				    int *count_ptr)
{
	const struct qm_sg_entry *sgt;
	dma_addr_t addr = qm_fd_addr(fd);
	ssize_t fd_off = dpa_fd_offset(fd);
	dma_addr_t sg_addr;
	void *vaddr, *sg_vaddr;
	struct dpa_bp *dpa_bp;
	struct page *page, *head_page;
	int frag_offset, frag_len;
	int page_offset;
	int i;
	struct sk_buff *skb = NULL, *skb_tmp, **skbh;

	vaddr = phys_to_virt(addr);
	DPA_ERR_ON(!IS_ALIGNED((unsigned long)vaddr, SMP_CACHE_BYTES));

	dpa_bp = priv->dpa_bp;
	/* Iterate through the SGT entries and add data buffers to the skb */
	sgt = vaddr + fd_off;
	for (i = 0; i < DPA_SGT_MAX_ENTRIES; i++) {
		/* Extension bit is not supported */
		DPA_ERR_ON(sgt[i].extension);

		/* We use a single global Rx pool */
		DPA_ERR_ON(dpa_bp != dpa_bpid2pool(sgt[i].bpid));

		sg_addr = qm_sg_addr(&sgt[i]);
		sg_vaddr = phys_to_virt(sg_addr);
		DPA_ERR_ON(!IS_ALIGNED((unsigned long)sg_vaddr,
				       SMP_CACHE_BYTES));

		dma_unmap_single(dpa_bp->dev, sg_addr, dpa_bp->size,
				 DMA_BIDIRECTIONAL);
		if (i == 0) {
			DPA_READ_SKB_PTR(skb, skbh, sg_vaddr, -1);
			DPA_ERR_ON(skb->head != sg_vaddr);

			skb->ip_summed = CHECKSUM_NONE;

			/* Make sure forwarded skbs will have enough space
			 * on Tx, if extra headers are added.
			 */
			DPA_ERR_ON(fd_off != priv->rx_headroom);
			skb_reserve(skb, fd_off);
			skb_put(skb, sgt[i].length);
		} else {
			/* Not the first S/G entry; all data from buffer will
			 * be added in an skb fragment; fragment index is offset
			 * by one since first S/G entry was incorporated in the
			 * linear part of the skb.
			 *
			 * Caution: 'page' may be a tail page.
			 */
			DPA_READ_SKB_PTR(skb_tmp, skbh, sg_vaddr, -1);
			page = virt_to_page(sg_vaddr);
			head_page = virt_to_head_page(sg_vaddr);

			/* Free (only) the skbuff shell because its data buffer
			 * is already a frag in the main skb.
			 */
			get_page(head_page);
			dev_kfree_skb(skb_tmp);

			/* Compute offset in (possibly tail) page */
			page_offset = ((unsigned long)sg_vaddr &
					(PAGE_SIZE - 1)) +
				(page_address(page) - page_address(head_page));
			/* page_offset only refers to the beginning of sgt[i];
			 * but the buffer itself may have an internal offset.
			 */
			frag_offset = sgt[i].offset + page_offset;
			frag_len = sgt[i].length;
			/* skb_add_rx_frag() does no checking on the page; if
			 * we pass it a tail page, we'll end up with
			 * bad page accounting and eventually with segafults.
			 */
			skb_add_rx_frag(skb, i - 1, head_page, frag_offset,
					frag_len, dpa_bp->size);
		}
		/* Update the pool count for the current {cpu x bpool} */
		(*count_ptr)--;

		if (sgt[i].final)
			break;
	}
	WARN_ONCE(i == DPA_SGT_MAX_ENTRIES, "No final bit on SGT\n");

	/* recycle the SGT fragment */
	DPA_ERR_ON(dpa_bp != dpa_bpid2pool(fd->bpid));
	dpa_bp_recycle_frag(dpa_bp, (unsigned long)vaddr, count_ptr);
	return skb;
}

void _dpa_rx(struct net_device *net_dev,
	     struct qman_portal *portal,
	     const struct dpa_priv_s *priv,
	     struct dpa_percpu_priv_s *percpu_priv,
	     const struct qm_fd *fd,
	     u32 fqid,
	     int *count_ptr)
{
	struct dpa_bp *dpa_bp;
	struct sk_buff *skb;
	dma_addr_t addr = qm_fd_addr(fd);
	u32 fd_status = fd->status;
	unsigned int skb_len;
	struct rtnl_link_stats64 *percpu_stats = &percpu_priv->stats;

	if (unlikely(fd_status & FM_FD_STAT_RX_ERRORS) != 0) {
		if (net_ratelimit())
			netif_warn(priv, hw, net_dev, "FD status = 0x%08x\n",
				   fd_status & FM_FD_STAT_RX_ERRORS);

		percpu_stats->rx_errors++;
		goto _release_frame;
	}

	dpa_bp = priv->dpa_bp;
	DPA_ERR_ON(dpa_bp != dpa_bpid2pool(fd->bpid));

	/* prefetch the first 64 bytes of the frame or the SGT start */
	dma_unmap_single(dpa_bp->dev, addr, dpa_bp->size, DMA_BIDIRECTIONAL);
	prefetch(phys_to_virt(addr) + dpa_fd_offset(fd));

	/* The only FD types that we may receive are contig and S/G */
	DPA_ERR_ON((fd->format != qm_fd_contig) && (fd->format != qm_fd_sg));

	if (likely(fd->format == qm_fd_contig))
		skb = contig_fd_to_skb(priv, fd);
	else
		skb = sg_fd_to_skb(priv, fd, count_ptr);

	/* Account for either the contig buffer or the SGT buffer (depending on
	 * which case we were in) having been removed from the pool.
	 */
	(*count_ptr)--;
	skb->protocol = eth_type_trans(skb, net_dev);

	/* IP Reassembled frames are allowed to be larger than MTU */
	if (unlikely(dpa_check_rx_mtu(skb, net_dev->mtu) &&
		     !(fd_status & FM_FD_IPR))) {
		percpu_stats->rx_dropped++;
		goto drop_bad_frame;
	}

	skb_len = skb->len;

	if (unlikely(netif_receive_skb(skb) == NET_RX_DROP))
		goto packet_dropped;

	percpu_stats->rx_packets++;
	percpu_stats->rx_bytes += skb_len;

packet_dropped:
	return;

drop_bad_frame:
	dev_kfree_skb(skb);
	return;

_release_frame:
	dpa_fd_release(net_dev, fd);
}

static int skb_to_contig_fd(struct dpa_priv_s *priv,
			    struct sk_buff *skb, struct qm_fd *fd,
			    int *count_ptr, int *offset)
{
	struct sk_buff **skbh;
	dma_addr_t addr;
	struct dpa_bp *dpa_bp = priv->dpa_bp;
	struct net_device *net_dev = priv->net_dev;
	int err;
	enum dma_data_direction dma_dir;
	unsigned char *buffer_start;

	{
		/* We are guaranteed to have at least tx_headroom bytes
		 * available, so just use that for offset.
		 */
		fd->bpid = 0xff;
		buffer_start = skb->data - priv->tx_headroom;
		fd->offset = priv->tx_headroom;
		dma_dir = DMA_TO_DEVICE;

		DPA_WRITE_SKB_PTR(skb, skbh, buffer_start, 0);
	}

	/* Enable L3/L4 hardware checksum computation.
	 *
	 * We must do this before dma_map_single(DMA_TO_DEVICE), because we may
	 * need to write into the skb.
	 */
	err = dpa_enable_tx_csum(priv, skb, fd,
				 ((char *)skbh) + DPA_TX_PRIV_DATA_SIZE);
	if (unlikely(err < 0)) {
		if (net_ratelimit())
			netif_err(priv, tx_err, net_dev, "HW csum error: %d\n",
				  err);
		return err;
	}

	/* Fill in the rest of the FD fields */
	fd->format = qm_fd_contig;
	fd->length20 = skb->len;
	fd->cmd |= FM_FD_CMD_FCO;

	/* Map the entire buffer size that may be seen by FMan, but no more */
	addr = dma_map_single(dpa_bp->dev, skbh,
			      skb_tail_pointer(skb) - buffer_start, dma_dir);
	if (unlikely(dma_mapping_error(dpa_bp->dev, addr))) {
		if (net_ratelimit())
			netif_err(priv, tx_err, net_dev, "dma_map_single() failed\n");
		return -EINVAL;
	}
	fd->addr_hi = (u8)upper_32_bits(addr);
	fd->addr_lo = lower_32_bits(addr);

	return 0;
}

static int skb_to_sg_fd(struct dpa_priv_s *priv,
			struct sk_buff *skb, struct qm_fd *fd)
{
	struct dpa_bp *dpa_bp = priv->dpa_bp;
	dma_addr_t addr;
	struct sk_buff **skbh;
	struct net_device *net_dev = priv->net_dev;
	int err;

	struct qm_sg_entry *sgt;
	void *sgt_buf;
	void *buffer_start;
	skb_frag_t *frag;
	int i, j;
	const enum dma_data_direction dma_dir = DMA_TO_DEVICE;
	const int nr_frags = skb_shinfo(skb)->nr_frags;

	fd->format = qm_fd_sg;

	/* get a page frag to store the SGTable */
	sgt_buf = netdev_alloc_frag(priv->tx_headroom +
		sizeof(struct qm_sg_entry) * (1 + nr_frags));
	if (unlikely(!sgt_buf)) {
		netdev_err(net_dev, "netdev_alloc_frag() failed\n");
		return -ENOMEM;
	}

	/* Enable L3/L4 hardware checksum computation.
	 *
	 * We must do this before dma_map_single(DMA_TO_DEVICE), because we may
	 * need to write into the skb.
	 */
	err = dpa_enable_tx_csum(priv, skb, fd,
				 sgt_buf + DPA_TX_PRIV_DATA_SIZE);
	if (unlikely(err < 0)) {
		if (net_ratelimit())
			netif_err(priv, tx_err, net_dev, "HW csum error: %d\n",
				  err);
		goto csum_failed;
	}

	sgt = (struct qm_sg_entry *)(sgt_buf + priv->tx_headroom);
	sgt[0].bpid = 0xff;
	sgt[0].offset = 0;
	sgt[0].length = cpu_to_be32(skb_headlen(skb));
	sgt[0].extension = 0;
	sgt[0].final = 0;
	addr = dma_map_single(dpa_bp->dev, skb->data, sgt[0].length, dma_dir);
	if (unlikely(dma_mapping_error(dpa_bp->dev, addr))) {
		dev_err(dpa_bp->dev, "DMA mapping failed");
		err = -EINVAL;
		goto sg0_map_failed;
	}
	sgt[0].addr_hi = (u8)upper_32_bits(addr);
	sgt[0].addr_lo = cpu_to_be32(lower_32_bits(addr));

	/* populate the rest of SGT entries */
	for (i = 1; i <= nr_frags; i++) {
		frag = &skb_shinfo(skb)->frags[i - 1];
		sgt[i].bpid = 0xff;
		sgt[i].offset = 0;
		sgt[i].length = cpu_to_be32(frag->size);
		sgt[i].extension = 0;
		sgt[i].final = 0;

		DPA_ERR_ON(!skb_frag_page(frag));
		addr = skb_frag_dma_map(dpa_bp->dev, frag, 0, sgt[i].length,
					dma_dir);
		if (unlikely(dma_mapping_error(dpa_bp->dev, addr))) {
			dev_err(dpa_bp->dev, "DMA mapping failed");
			err = -EINVAL;
			goto sg_map_failed;
		}

		/* keep the offset in the address */
		sgt[i].addr_hi = (u8)upper_32_bits(addr);
		sgt[i].addr_lo = cpu_to_be32(lower_32_bits(addr));
	}
	sgt[i - 1].final = 1;

	fd->length20 = skb->len;
	fd->offset = priv->tx_headroom;

	/* DMA map the SGT page */
	buffer_start = (void *)sgt - priv->tx_headroom;
	DPA_WRITE_SKB_PTR(skb, skbh, buffer_start, 0);

	addr = dma_map_single(dpa_bp->dev, buffer_start, priv->tx_headroom +
			      sizeof(struct qm_sg_entry) * (1 + nr_frags),
			      dma_dir);
	if (unlikely(dma_mapping_error(dpa_bp->dev, addr))) {
		dev_err(dpa_bp->dev, "DMA mapping failed");
		err = -EINVAL;
		goto sgt_map_failed;
	}

	fd->bpid = 0xff;
	fd->cmd |= FM_FD_CMD_FCO;
	fd->addr_hi = (u8)upper_32_bits(addr);
	fd->addr_lo = lower_32_bits(addr);

	return 0;

sgt_map_failed:
sg_map_failed:
	for (j = 0; j < i; j++)
		dma_unmap_page(dpa_bp->dev, qm_sg_addr(&sgt[j]),
			       cpu_to_be32(sgt[j].length), dma_dir);
sg0_map_failed:
csum_failed:
	put_page(virt_to_head_page(sgt_buf));

	return err;
}

int dpa_tx(struct sk_buff *skb, struct net_device *net_dev)
{
	struct dpa_priv_s *priv;
	struct qm_fd fd;
	struct dpa_percpu_priv_s *percpu_priv;
	struct rtnl_link_stats64 *percpu_stats;
	int err = 0;
	const int queue_mapping = dpa_get_queue_mapping(skb);
	bool nonlinear = skb_is_nonlinear(skb);
	int *countptr, offset = 0;

	priv = netdev_priv(net_dev);
	/* Non-migratable context, safe to use raw_cpu_ptr */
	percpu_priv = raw_cpu_ptr(priv->percpu_priv);
	percpu_stats = &percpu_priv->stats;
	countptr = raw_cpu_ptr(priv->dpa_bp->percpu_count);

	clear_fd(&fd);

	if (!nonlinear) {
		/* We're going to store the skb backpointer at the beginning
		 * of the data buffer, so we need a privately owned skb
		 *
		 * We've made sure skb is not shared in dev->priv_flags,
		 * we need to verify the skb head is not cloned
		 */
		if (skb_cow_head(skb, priv->tx_headroom))
			goto enomem;

		BUG_ON(skb_is_nonlinear(skb));
	}

	/* MAX_SKB_FRAGS is equal or larger than our DPA_SGT_MAX_ENTRIES;
	 * make sure we don't feed FMan with more fragments than it supports.
	 * Btw, we're using the first sgt entry to store the linear part of
	 * the skb, so we're one extra frag short.
	 */
	if (nonlinear &&
	    likely(skb_shinfo(skb)->nr_frags < DPA_SGT_MAX_ENTRIES)) {
		/* Just create a S/G fd based on the skb */
		err = skb_to_sg_fd(priv, skb, &fd);
		percpu_priv->tx_frag_skbuffs++;
	} else {
		/* If the egress skb contains more fragments than we support
		 * we have no choice but to linearize it ourselves.
		 */
		if (unlikely(nonlinear) && __skb_linearize(skb))
			goto enomem;

		/* Finally, create a contig FD from this skb */
		err = skb_to_contig_fd(priv, skb, &fd, countptr, &offset);
	}
	if (unlikely(err < 0))
		goto skb_to_fd_failed;

	if (likely(dpa_xmit(priv, percpu_stats, queue_mapping, &fd) == 0))
		return NETDEV_TX_OK;

	/* dpa_xmit failed */
	if (fd.bpid != 0xff) {
		(*countptr)--;
		dpa_fd_release(net_dev, &fd);
		percpu_stats->tx_errors++;
		return NETDEV_TX_OK;
	}
	_dpa_cleanup_tx_fd(priv, &fd);
skb_to_fd_failed:
enomem:
	percpu_stats->tx_errors++;
	dev_kfree_skb(skb);
	return NETDEV_TX_OK;
}
