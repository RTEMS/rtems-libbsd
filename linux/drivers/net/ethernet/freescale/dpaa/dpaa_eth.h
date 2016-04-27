/* Copyright 2008 - 2015 Freescale Semiconductor Inc.
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

#ifndef __DPA_H
#define __DPA_H

#include <linux/netdevice.h>
#include <soc/fsl/qman.h>

#include "fman.h"
#include "mac.h"
#include "dpaa_eth_trace.h"

#ifndef __rtems__
extern int dpa_rx_extra_headroom;
extern int dpa_max_frm;

#define dpa_get_rx_extra_headroom() dpa_rx_extra_headroom
#define dpa_get_max_frm() dpa_max_frm
#else /* __rtems__ */
#define dpa_get_rx_extra_headroom fman_get_rx_extra_headroom
#define dpa_get_max_frm fman_get_max_frm
#endif /* __rtems__ */

#define dpa_get_max_mtu()	\
	(dpa_get_max_frm() - (VLAN_ETH_HLEN + ETH_FCS_LEN))

/* Simple enum of FQ types - used for array indexing */
enum port_type {RX, TX};

struct dpa_buffer_layout_s {
	u16 priv_data_size;
	bool parse_results;
	bool time_stamp;
	bool hash_results;
	u16 data_align;
};

#define DPA_ERR_ON(cond)

#define DPA_TX_PRIV_DATA_SIZE	16
#define DPA_PARSE_RESULTS_SIZE sizeof(struct fman_prs_result)
#define DPA_TIME_STAMP_SIZE 8
#define DPA_HASH_RESULTS_SIZE 8
#define DPA_RX_PRIV_DATA_SIZE	(DPA_TX_PRIV_DATA_SIZE + \
					dpa_get_rx_extra_headroom())

#define FM_FD_STAT_RX_ERRORS						\
	(FM_FD_ERR_DMA | FM_FD_ERR_PHYSICAL	| \
	 FM_FD_ERR_SIZE | FM_FD_ERR_CLS_DISCARD | \
	 FM_FD_ERR_EXTRACTION | FM_FD_ERR_NO_SCHEME	| \
	 FM_FD_ERR_PRS_TIMEOUT | FM_FD_ERR_PRS_ILL_INSTRUCT | \
	 FM_FD_ERR_PRS_HDR_ERR)

#define FM_FD_STAT_TX_ERRORS \
	(FM_FD_ERR_UNSUPPORTED_FORMAT | \
	 FM_FD_ERR_LENGTH | FM_FD_ERR_DMA)

/* The raw buffer size must be cacheline aligned.
 * Normally we use 2K buffers.
 */
#define DPA_BP_RAW_SIZE		2048

/* This is what FMan is ever allowed to use.
 * FMan-DMA requires 16-byte alignment for Rx buffers, but SKB_DATA_ALIGN is
 * even stronger (SMP_CACHE_BYTES-aligned), so we just get away with that,
 * via SKB_WITH_OVERHEAD(). We can't rely on netdev_alloc_frag() giving us
 * half-page-aligned buffers (can we?), so we reserve some more space
 * for start-of-buffer alignment.
 */
#ifndef __rtems__
#define dpa_bp_size(buffer_layout)	(SKB_WITH_OVERHEAD(DPA_BP_RAW_SIZE) - \
						SMP_CACHE_BYTES)
#else /* __rtems__ */
/*
 * FIXME: 4 bytes would be enough for the mbuf pointer.  However, jumbo receive
 * frames overwrite this area if < 64 bytes.
 */
#define DPA_OUT_OF_BAND_SIZE 64
#define DPA_MBUF_POINTER_OFFSET (DPA_BP_RAW_SIZE - DPA_OUT_OF_BAND_SIZE)
#define dpa_bp_size(buffer_layout) DPA_MBUF_POINTER_OFFSET
#endif /* __rtems__ */
/* We must ensure that skb_shinfo is always cacheline-aligned. */
#define DPA_SKB_SIZE(size)	((size) & ~(SMP_CACHE_BYTES - 1))

/* Largest value that the FQD's OAL field can hold.
 * This is DPAA-1.x specific.
 */
#define FSL_QMAN_MAX_OAL	127

/* Default alignment for start of data in an Rx FD */
#define DPA_FD_DATA_ALIGNMENT  16

/* Values for the L3R field of the FM Parse Results
 */
/* L3 Type field: First IP Present IPv4 */
#define FM_L3_PARSE_RESULT_IPV4	0x8000
/* L3 Type field: First IP Present IPv6 */
#define FM_L3_PARSE_RESULT_IPV6	0x4000

/* Values for the L4R field of the FM Parse Results
 * See $8.8.4.7.20 - L4 HXS - L4 Results from DPAA-Rev2 Reference Manual.
 */
/* L4 Type field: UDP */
#define FM_L4_PARSE_RESULT_UDP	0x40
/* L4 Type field: TCP */
#define FM_L4_PARSE_RESULT_TCP	0x20

/* number of Tx queues to FMan */
#define DPAA_ETH_TX_QUEUES	NR_CPUS

#define DPAA_ETH_RX_QUEUES	128

#define FSL_DPAA_ETH_MAX_BUF_COUNT	128
#define FSL_DPAA_ETH_REFILL_THRESHOLD	80

/* More detailed FQ types - used for fine-grained WQ assignments */
enum dpa_fq_type {
	FQ_TYPE_RX_DEFAULT = 1, /* Rx Default FQs */
	FQ_TYPE_RX_ERROR,	/* Rx Error FQs */
	FQ_TYPE_RX_PCD,		/* User-defined PCDs */
	FQ_TYPE_TX,		/* "Real" Tx FQs */
	FQ_TYPE_TX_CONFIRM,	/* Tx default Conf FQ (actually an Rx FQ) */
	FQ_TYPE_TX_CONF_MQ,	/* Tx conf FQs (one for each Tx FQ) */
	FQ_TYPE_TX_ERROR,	/* Tx Error FQs (these are actually Rx FQs) */
};

struct dpa_fq {
	struct qman_fq fq_base;
	struct list_head list;
	struct net_device *net_dev;
	bool init;
	u32 fqid;
	u32 flags;
	u16 channel;
	u8 wq;
	enum dpa_fq_type fq_type;
};

struct dpa_fq_cbs_t {
	struct qman_fq rx_defq;
	struct qman_fq tx_defq;
	struct qman_fq rx_errq;
	struct qman_fq tx_errq;
	struct qman_fq egress_ern;
};

struct fqid_cell {
	u32 start;
	u32 count;
};

struct dpa_bp {
	struct bman_pool *pool;
	u8 bpid;
#ifndef __rtems__
	struct device *dev;
#endif /* __rtems__ */
	/* the buffer pools used for the private ports are initialized
	 * with config_count buffers for each CPU; at runtime the
	 * number of buffers per CPU is constantly brought back to this
	 * level
	 */
	int config_count;
	size_t size;
	bool seed_pool;
	/* physical address of the contiguous memory used by the pool to store
	 * the buffers
	 */
	dma_addr_t paddr;
	/* virtual address of the contiguous memory used by the pool to store
	 * the buffers
	 */
	void __iomem *vaddr;
	/* current number of buffers in the bpool alloted to this CPU */
	int __percpu *percpu_count;
	atomic_t refs;
	/* some bpools need to be seeded before use by this cb */
	int (*seed_cb)(struct dpa_bp *);
	/* some bpools need to be emptied before freeing; this cb is used
	 * for freeing of individual buffers taken from the pool
	 */
	void (*free_buf_cb)(void *addr);
};

struct dpa_rx_errors {
	u64 dme;		/* DMA Error */
	u64 fpe;		/* Frame Physical Error */
	u64 fse;		/* Frame Size Error */
	u64 phe;		/* Header Error */
};

/* Counters for QMan ERN frames - one counter per rejection code */
struct dpa_ern_cnt {
	u64 cg_tdrop;		/* Congestion group taildrop */
	u64 wred;		/* WRED congestion */
	u64 err_cond;		/* Error condition */
	u64 early_window;	/* Order restoration, frame too early */
	u64 late_window;	/* Order restoration, frame too late */
	u64 fq_tdrop;		/* FQ taildrop */
	u64 fq_retired;		/* FQ is retired */
	u64 orp_zero;		/* ORP disabled */
};

struct dpa_napi_portal {
#ifndef __rtems__
	struct napi_struct napi;
#endif /* __rtems__ */
	struct qman_portal *p;
};

struct dpa_percpu_priv_s {
	struct net_device *net_dev;
	struct dpa_napi_portal *np;
	u64 in_interrupt;
	u64 tx_confirm;
	/* fragmented (non-linear) skbuffs received from the stack */
	u64 tx_frag_skbuffs;
#ifndef __rtems__
	struct rtnl_link_stats64 stats;
#endif /* __rtems__ */
	struct dpa_rx_errors rx_errors;
	struct dpa_ern_cnt ern_cnt;
};

struct dpa_priv_s {
	struct dpa_percpu_priv_s __percpu *percpu_priv;
	struct dpa_bp *dpa_bp;
	/* Store here the needed Tx headroom for convenience and speed
	 * (even though it can be computed based on the fields of buf_layout)
	 */
	u16 tx_headroom;
	struct net_device *net_dev;
	struct mac_device *mac_dev;
	struct qman_fq *egress_fqs[DPAA_ETH_TX_QUEUES];
	struct qman_fq *conf_fqs[DPAA_ETH_TX_QUEUES];

	size_t bp_count;

	u16 channel;	/* "fsl,qman-channel-id" */
	struct list_head dpa_fq_list;

#ifndef __rtems__
	u32 msg_enable;	/* net_device message level */
#endif /* __rtems__ */

	struct {
		/* All egress queues to a given net device belong to one
		 * (and the same) congestion group.
		 */
		struct qman_cgr cgr;
		/* If congested, when it began. Used for performance stats. */
		u32 congestion_start_jiffies;
		/* Number of jiffies the Tx port was congested. */
		u32 congested_jiffies;
		/* Counter for the number of times the CGR
		 * entered congestion state
		 */
		u32 cgr_congested_count;
	} cgr_data;
	/* Use a per-port CGR for ingress traffic. */
	bool use_ingress_cgr;
	struct qman_cgr ingress_cgr;

	struct dpa_buffer_layout_s *buf_layout;
	u16 rx_headroom;
};

struct fm_port_fqs {
	struct dpa_fq *tx_defq;
	struct dpa_fq *tx_errq;
	struct dpa_fq *rx_defq;
	struct dpa_fq *rx_errq;
};

int dpa_bp_priv_seed(struct dpa_bp *dpa_bp);
int dpaa_eth_refill_bpools(struct dpa_bp *dpa_bp, int *count_ptr);
void _dpa_rx(struct net_device *net_dev,
	     struct qman_portal *portal,
	     const struct dpa_priv_s *priv,
	     struct dpa_percpu_priv_s *percpu_priv,
	     const struct qm_fd *fd,
	     u32 fqid,
	     int *count_ptr);
#ifndef __rtems__
int dpa_tx(struct sk_buff *skb, struct net_device *net_dev);
struct sk_buff *_dpa_cleanup_tx_fd(const struct dpa_priv_s *priv,
				   const struct qm_fd *fd);

/* Turn on HW checksum computation for this outgoing frame.
 * If the current protocol is not something we support in this regard
 * (or if the stack has already computed the SW checksum), we do nothing.
 *
 * Returns 0 if all goes well (or HW csum doesn't apply), and a negative value
 * otherwise.
 *
 * Note that this function may modify the fd->cmd field and the skb data buffer
 * (the Parse Results area).
 */
int dpa_enable_tx_csum(struct dpa_priv_s *priv, struct sk_buff *skb,
		       struct qm_fd *fd, char *parse_results);
#else /* __rtems__ */
void _dpa_cleanup_tx_fd(struct ifnet *ifp, const struct qm_fd *fd);
#endif /* __rtems__ */

static inline int dpaa_eth_napi_schedule(struct dpa_percpu_priv_s *percpu_priv,
					 struct qman_portal *portal)
{
#ifndef __rtems__
	/* In case of threaded ISR for RT enable kernel,
	 * in_irq() does not return appropriate value, so use
	 * in_serving_softirq to distinguish softirq or irq context.
	 */
	if (unlikely(in_irq() || !in_serving_softirq())) {
		/* Disable QMan IRQ and invoke NAPI */
		int ret = qman_p_irqsource_remove(portal, QM_PIRQ_DQRI);

		if (likely(!ret)) {
			const struct qman_portal_config *pc =
					qman_p_get_portal_config(portal);
			struct dpa_napi_portal *np =
					&percpu_priv->np[pc->channel];

			np->p = portal;
			napi_schedule(&np->napi);
			percpu_priv->in_interrupt++;
			return 1;
		}
	}
#else /* __rtems__ */
	/* FIXME */
#endif /* __rtems__ */
	return 0;
}

static inline ssize_t __const dpa_fd_length(const struct qm_fd *fd)
{
	return fd->length20;
}

static inline ssize_t __const dpa_fd_offset(const struct qm_fd *fd)
{
	return fd->offset;
}

#ifndef __rtems__
/* Verifies if the skb length is below the interface MTU */
static inline int dpa_check_rx_mtu(struct sk_buff *skb, int mtu)
{
	if (unlikely(skb->len > mtu))
		if ((skb->protocol != htons(ETH_P_8021Q)) ||
		    (skb->len > mtu + 4))
			return -1;

	return 0;
}
#endif /* __rtems__ */

static inline u16 dpa_get_headroom(struct dpa_buffer_layout_s *bl)
{
	u16 headroom;
	/* The frame headroom must accommodate:
	 * - the driver private data area
	 * - parse results, hash results, timestamp if selected
	 * If either hash results or time stamp are selected, both will
	 * be copied to/from the frame headroom, as TS is located between PR and
	 * HR in the IC and IC copy size has a granularity of 16bytes
	 * (see description of FMBM_RICP and FMBM_TICP registers in DPAARM)
	 *
	 * Also make sure the headroom is a multiple of data_align bytes
	 */
	headroom = (u16)(bl->priv_data_size +
		   (bl->parse_results ? DPA_PARSE_RESULTS_SIZE : 0) +
		   (bl->hash_results || bl->time_stamp ?
		    DPA_TIME_STAMP_SIZE + DPA_HASH_RESULTS_SIZE : 0));

	return bl->data_align ? ALIGN(headroom, bl->data_align) : headroom;
}

#ifndef __rtems__
void dpaa_eth_sysfs_remove(struct device *dev);
void dpaa_eth_sysfs_init(struct device *dev);

void dpa_private_napi_del(struct net_device *net_dev);
#endif /* __rtems__ */

static inline void clear_fd(struct qm_fd *fd)
{
	fd->opaque_addr = 0;
	fd->opaque = 0;
	fd->cmd = 0;
}

static inline int _dpa_tx_fq_to_id(const struct dpa_priv_s *priv,
		struct qman_fq *tx_fq)
{
	int i;

	for (i = 0; i < DPAA_ETH_TX_QUEUES; i++)
		if (priv->egress_fqs[i] == tx_fq)
			return i;

	return -EINVAL;
}

#ifndef __rtems__
static inline int dpa_xmit(struct dpa_priv_s *priv,
			   struct rtnl_link_stats64 *percpu_stats,
			   int queue,
			   struct qm_fd *fd)
{
	int err, i;
	struct qman_fq *egress_fq;

	egress_fq = priv->egress_fqs[queue];
	if (fd->bpid == 0xff)
		fd->cmd |= qman_fq_fqid(priv->conf_fqs[queue]);

	/* Trace this Tx fd */
	trace_dpa_tx_fd(priv->net_dev, egress_fq, fd);

	for (i = 0; i < 100000; i++) {
		err = qman_enqueue(egress_fq, fd, 0);
		if (err != -EBUSY)
			break;
	}

	if (unlikely(err < 0)) {
		percpu_stats->tx_errors++;
		percpu_stats->tx_fifo_errors++;
		return err;
	}

	percpu_stats->tx_packets++;
	percpu_stats->tx_bytes += dpa_fd_length(fd);

	return 0;
}
#endif /* __rtems__ */

/* Use multiple WQs for FQ assignment:
 *	- Tx Confirmation queues go to WQ1.
 *	- Rx Default and Tx queues go to WQ3 (no differentiation between
 *	  Rx and Tx traffic).
 *	- Rx Error and Tx Error queues go to WQ2 (giving them a better chance
 *	  to be scheduled, in case there are many more FQs in WQ3).
 * This ensures that Tx-confirmed buffers are timely released. In particular,
 * it avoids congestion on the Tx Confirm FQs, which can pile up PFDRs if they
 * are greatly outnumbered by other FQs in the system, while
 * dequeue scheduling is round-robin.
 */
static inline void _dpa_assign_wq(struct dpa_fq *fq)
{
	switch (fq->fq_type) {
	case FQ_TYPE_TX_CONFIRM:
	case FQ_TYPE_TX_CONF_MQ:
		fq->wq = 1;
		break;
	case FQ_TYPE_RX_DEFAULT:
	case FQ_TYPE_TX:
		fq->wq = 3;
		break;
	case FQ_TYPE_RX_ERROR:
	case FQ_TYPE_TX_ERROR:
		fq->wq = 2;
		break;
	default:
		WARN(1, "Invalid FQ type %d for FQID %d!\n",
		     fq->fq_type, fq->fqid);
	}
}

#ifdef CONFIG_FSL_DPAA_ETH_USE_NDO_SELECT_QUEUE
/* Use in lieu of skb_get_queue_mapping() */
#define dpa_get_queue_mapping(skb) \
	raw_smp_processor_id()
#else
/* Use the queue selected by XPS */
#define dpa_get_queue_mapping(skb) \
	skb_get_queue_mapping(skb)
#endif

static inline void _dpa_bp_free_pf(void *addr)
{
#ifndef __rtems__
	put_page(virt_to_head_page(addr));
#else /* __rtems__ */
	BSD_ASSERT(0);
#endif /* __rtems__ */
}

#endif	/* __DPA_H */
