#include <machine/rtems-bsd-kernel-space.h>

#include <rtems/bsd/local/opt_dpaa.h>

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

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/init.h>
#include <linux/module.h>
#include <linux/of_mdio.h>
#include <linux/of_net.h>
#include <linux/kthread.h>
#include <linux/io.h>
#ifndef __rtems__
#include <linux/if_arp.h>
#include <linux/if_vlan.h>
#include <linux/icmp.h>
#include <linux/ip.h>
#include <linux/ipv6.h>
#include <linux/udp.h>
#include <linux/tcp.h>
#include <linux/net.h>
#include <linux/if_ether.h>
#include <linux/highmem.h>
#include <linux/percpu.h>
#include <linux/dma-mapping.h>
#endif /* __rtems__ */
#include <soc/fsl/bman.h>

#include "fman.h"
#include "fman_port.h"

#include "mac.h"
#include "dpaa_eth.h"
#include "dpaa_eth_common.h"

/* CREATE_TRACE_POINTS only needs to be defined once. Other dpa files
 * using trace events only need to #include <trace/events/sched.h>
 */
#define CREATE_TRACE_POINTS
#include "dpaa_eth_trace.h"

#define DPA_NAPI_WEIGHT		64

/* Valid checksum indication */
#define DPA_CSUM_VALID		0xFFFF

#define DPA_DESCRIPTION "FSL DPAA Ethernet driver"

#define DPAA_INGRESS_CS_THRESHOLD 0x10000000
/* Ingress congestion threshold on FMan ports
 * The size in bytes of the ingress tail-drop threshold on FMan ports.
 * Traffic piling up above this value will be rejected by QMan and discarded
 * by FMan.
 */

#ifndef __rtems__
static u8 debug = -1;
module_param(debug, byte, S_IRUGO);
MODULE_PARM_DESC(debug, "Module/Driver verbosity level");

/* This has to work in tandem with the DPA_CS_THRESHOLD_xxx values. */
static u16 tx_timeout = 1000;
module_param(tx_timeout, ushort, S_IRUGO);
MODULE_PARM_DESC(tx_timeout, "The Tx timeout in ms");
#endif /* __rtems__ */

/* BM */

#define DPAA_ETH_MAX_PAD (L1_CACHE_BYTES * 8)

static u8 dpa_priv_common_bpid;

static void _dpa_rx_error(struct net_device *net_dev,
			  const struct dpa_priv_s *priv,
			  struct dpa_percpu_priv_s *percpu_priv,
			  const struct qm_fd *fd,
			  u32 fqid)
{
	/* limit common, possibly innocuous Rx FIFO Overflow errors'
	 * interference with zero-loss convergence benchmark results.
	 */
	if (likely(fd->status & FM_FD_ERR_PHYSICAL))
		pr_warn_once("non-zero error counters in fman statistics (sysfs)\n");
	else
#ifndef __rtems__
		if (net_ratelimit())
			netif_err(priv, hw, net_dev, "Err FD status = 0x%08x\n",
				  fd->status & FM_FD_STAT_RX_ERRORS);
#else /* __rtems__ */
		BSD_ASSERT(0);
#endif /* __rtems__ */

#ifndef __rtems__
	percpu_priv->stats.rx_errors++;
#endif /* __rtems__ */

	if (fd->status & FM_FD_ERR_DMA)
		percpu_priv->rx_errors.dme++;
	if (fd->status & FM_FD_ERR_PHYSICAL)
		percpu_priv->rx_errors.fpe++;
	if (fd->status & FM_FD_ERR_SIZE)
		percpu_priv->rx_errors.fse++;
	if (fd->status & FM_FD_ERR_PRS_HDR_ERR)
		percpu_priv->rx_errors.phe++;

	dpa_fd_release(net_dev, fd);
}

static void _dpa_tx_error(struct net_device *net_dev,
			  const struct dpa_priv_s *priv,
			  struct dpa_percpu_priv_s *percpu_priv,
			  const struct qm_fd *fd,
			  u32 fqid)
{
#ifndef __rtems__
	struct sk_buff *skb;

	if (net_ratelimit())
		netif_warn(priv, hw, net_dev, "FD status = 0x%08x\n",
			   fd->status & FM_FD_STAT_TX_ERRORS);

	percpu_priv->stats.tx_errors++;
#else /* __rtems__ */
	struct ifnet *ifp = net_dev->ifp;

	if_inc_counter(ifp, IFCOUNTER_OERRORS, 1);
#endif /* __rtems__ */

	/* If we intended the buffers from this frame to go into the bpools
	 * when the FMan transmit was done, we need to put it in manually.
	 */
	if (fd->bpid != 0xff) {
		dpa_fd_release(net_dev, fd);
		return;
	}

#ifndef __rtems__
	skb = _dpa_cleanup_tx_fd(priv, fd);
	dev_kfree_skb(skb);
#else /* __rtems__ */
	_dpa_cleanup_tx_fd(ifp, fd);
#endif /* __rtems__ */
}

#ifndef __rtems__
static int dpaa_eth_poll(struct napi_struct *napi, int budget)
{
	struct dpa_napi_portal *np =
			container_of(napi, struct dpa_napi_portal, napi);

	int cleaned = qman_p_poll_dqrr(np->p, budget);

	if (cleaned < budget) {
		int tmp;

		napi_complete(napi);
		tmp = qman_p_irqsource_add(np->p, QM_PIRQ_DQRI);
		DPA_ERR_ON(tmp);
	}

	return cleaned;
}
#endif /* __rtems__ */

static void _dpa_tx_conf(struct net_device *net_dev,
			 const struct dpa_priv_s *priv,
			 struct dpa_percpu_priv_s *percpu_priv,
			 const struct qm_fd *fd,
			 u32 fqid)
{
#ifndef __rtems__
	struct sk_buff	*skb;

	if (unlikely(fd->status & FM_FD_STAT_TX_ERRORS) != 0) {
		if (net_ratelimit())
			netif_warn(priv, hw, net_dev, "FD status = 0x%08x\n",
				   fd->status & FM_FD_STAT_TX_ERRORS);

		percpu_priv->stats.tx_errors++;
	}

	percpu_priv->tx_confirm++;

	skb = _dpa_cleanup_tx_fd(priv, fd);

	dev_kfree_skb(skb);
#else /* __rtems__ */
	struct ifnet *ifp = net_dev->ifp;

	if (unlikely(fd->status & FM_FD_STAT_TX_ERRORS) != 0) {
		if_inc_counter(ifp, IFCOUNTER_OERRORS, 1);
	}

	_dpa_cleanup_tx_fd(ifp, fd);
#endif /* __rtems__ */
}

static enum qman_cb_dqrr_result
priv_rx_error_dqrr(struct qman_portal *portal,
		   struct qman_fq *fq,
		   const struct qm_dqrr_entry *dq)
{
	struct net_device *net_dev;
	struct dpa_priv_s *priv;
	struct dpa_percpu_priv_s *percpu_priv;
	int *count_ptr;

	net_dev = ((struct dpa_fq *)fq)->net_dev;
	priv = netdev_priv(net_dev);

	percpu_priv = raw_cpu_ptr(priv->percpu_priv);
	count_ptr = raw_cpu_ptr(priv->dpa_bp->percpu_count);

	if (dpaa_eth_napi_schedule(percpu_priv, portal))
		return qman_cb_dqrr_stop;

	if (unlikely(dpaa_eth_refill_bpools(priv->dpa_bp, count_ptr)))
		/* Unable to refill the buffer pool due to insufficient
		 * system memory. Just release the frame back into the pool,
		 * otherwise we'll soon end up with an empty buffer pool.
		 */
		dpa_fd_release(net_dev, &dq->fd);
	else
		_dpa_rx_error(net_dev, priv, percpu_priv, &dq->fd, fq->fqid);

	return qman_cb_dqrr_consume;
}

static enum qman_cb_dqrr_result
priv_rx_default_dqrr(struct qman_portal *portal,
		     struct qman_fq *fq,
		     const struct qm_dqrr_entry *dq)
{
	struct net_device *net_dev;
	struct dpa_priv_s *priv;
	struct dpa_percpu_priv_s *percpu_priv;
	int *count_ptr;
	struct dpa_bp *dpa_bp;

	net_dev = ((struct dpa_fq *)fq)->net_dev;
	priv = netdev_priv(net_dev);
	dpa_bp = priv->dpa_bp;

#ifndef __rtems__
	/* Trace the Rx fd */
	trace_dpa_rx_fd(net_dev, fq, &dq->fd);
#endif /* __rtems__ */

	/* IRQ handler, non-migratable; safe to use raw_cpu_ptr here */
	percpu_priv = raw_cpu_ptr(priv->percpu_priv);
	count_ptr = raw_cpu_ptr(dpa_bp->percpu_count);

	if (unlikely(dpaa_eth_napi_schedule(percpu_priv, portal)))
		return qman_cb_dqrr_stop;

	/* Vale of plenty: make sure we didn't run out of buffers */

	if (unlikely(dpaa_eth_refill_bpools(dpa_bp, count_ptr)))
#ifdef __rtems__
	{
		struct ifnet *ifp = net_dev->ifp;
		if_inc_counter(ifp, IFCOUNTER_IQDROPS, 1);
#endif /* __rtems__ */
		/* Unable to refill the buffer pool due to insufficient
		 * system memory. Just release the frame back into the pool,
		 * otherwise we'll soon end up with an empty buffer pool.
		 */
		dpa_fd_release(net_dev, &dq->fd);
#ifdef __rtems__
	}
#endif /* __rtems__ */
	else
		_dpa_rx(net_dev, portal, priv, percpu_priv, &dq->fd, fq->fqid,
			count_ptr);

	return qman_cb_dqrr_consume;
}

static enum qman_cb_dqrr_result
priv_tx_conf_error_dqrr(struct qman_portal *portal,
			struct qman_fq *fq,
			const struct qm_dqrr_entry *dq)
{
	struct net_device *net_dev;
	struct dpa_priv_s *priv;
	struct dpa_percpu_priv_s *percpu_priv;

	net_dev = ((struct dpa_fq *)fq)->net_dev;
	priv = netdev_priv(net_dev);

	percpu_priv = raw_cpu_ptr(priv->percpu_priv);

	if (dpaa_eth_napi_schedule(percpu_priv, portal))
		return qman_cb_dqrr_stop;

	_dpa_tx_error(net_dev, priv, percpu_priv, &dq->fd, fq->fqid);

	return qman_cb_dqrr_consume;
}

static enum qman_cb_dqrr_result
priv_tx_conf_default_dqrr(struct qman_portal *portal,
			  struct qman_fq *fq,
			  const struct qm_dqrr_entry *dq)
{
	struct net_device *net_dev;
	struct dpa_priv_s *priv;
	struct dpa_percpu_priv_s *percpu_priv;

	net_dev = ((struct dpa_fq *)fq)->net_dev;
	priv = netdev_priv(net_dev);

#ifndef __rtems__
	/* Trace the fd */
	trace_dpa_tx_conf_fd(net_dev, fq, &dq->fd);
#endif /* __rtems__ */

	/* Non-migratable context, safe to use raw_cpu_ptr */
	percpu_priv = raw_cpu_ptr(priv->percpu_priv);

	if (dpaa_eth_napi_schedule(percpu_priv, portal))
		return qman_cb_dqrr_stop;

	_dpa_tx_conf(net_dev, priv, percpu_priv, &dq->fd, fq->fqid);

	return qman_cb_dqrr_consume;
}

static void priv_ern(struct qman_portal *portal,
		     struct qman_fq *fq,
		     const struct qm_mr_entry *msg)
{
	struct net_device *net_dev;
	const struct dpa_priv_s *priv;
#ifndef __rtems__
	struct sk_buff *skb;
#else /* __rtems__ */
	struct ifnet *ifp;
#endif /* __rtems__ */
	struct dpa_percpu_priv_s *percpu_priv;
	const struct qm_fd *fd = &msg->ern.fd;

	net_dev = ((struct dpa_fq *)fq)->net_dev;
	priv = netdev_priv(net_dev);
	/* Non-migratable context, safe to use raw_cpu_ptr */
	percpu_priv = raw_cpu_ptr(priv->percpu_priv);

#ifndef __rtems__
	percpu_priv->stats.tx_dropped++;
	percpu_priv->stats.tx_fifo_errors++;
#else /* __rtems__ */
	ifp = net_dev->ifp;
	if_inc_counter(ifp, IFCOUNTER_OERRORS, 1);
#endif /* __rtems__ */
	count_ern(percpu_priv, msg);

	/* If we intended this buffer to go into the pool
	 * when the FM was done, we need to put it in
	 * manually.
	 */
	if (msg->ern.fd.bpid != 0xff) {
		dpa_fd_release(net_dev, fd);
		return;
	}

#ifndef __rtems__
	skb = _dpa_cleanup_tx_fd(priv, fd);
	dev_kfree_skb_any(skb);
#else /* __rtems__ */
	_dpa_cleanup_tx_fd(ifp, fd);
#endif /* __rtems__ */
}

static const struct dpa_fq_cbs_t private_fq_cbs = {
	.rx_defq = { .cb = { .dqrr = priv_rx_default_dqrr } },
	.tx_defq = { .cb = { .dqrr = priv_tx_conf_default_dqrr } },
	.rx_errq = { .cb = { .dqrr = priv_rx_error_dqrr } },
	.tx_errq = { .cb = { .dqrr = priv_tx_conf_error_dqrr } },
	.egress_ern = { .cb = { .ern = priv_ern } }
};

static void dpaa_eth_napi_enable(struct dpa_priv_s *priv)
{
#ifndef __rtems__
	struct dpa_percpu_priv_s *percpu_priv;
	int i, j;

	for_each_possible_cpu(i) {
		percpu_priv = per_cpu_ptr(priv->percpu_priv, i);

		for (j = 0; j < qman_portal_max; j++)
			napi_enable(&percpu_priv->np[j].napi);
	}
#endif /* __rtems__ */
}

static void dpaa_eth_napi_disable(struct dpa_priv_s *priv)
{
#ifndef __rtems__
	struct dpa_percpu_priv_s *percpu_priv;
	int i, j;

	for_each_possible_cpu(i) {
		percpu_priv = per_cpu_ptr(priv->percpu_priv, i);

		for (j = 0; j < qman_portal_max; j++)
			napi_disable(&percpu_priv->np[j].napi);
	}
#endif /* __rtems__ */
}

#ifndef __rtems__
static int dpa_eth_priv_start(struct net_device *net_dev)
#else /* __rtems__ */
int dpa_eth_priv_start(struct net_device *net_dev)
#endif /* __rtems__ */
{
	int err;
	struct dpa_priv_s *priv;

	priv = netdev_priv(net_dev);

	dpaa_eth_napi_enable(priv);

	err = dpa_start(net_dev);
	if (err < 0)
		dpaa_eth_napi_disable(priv);

	return err;
}

#ifndef __rtems__
static int dpa_eth_priv_stop(struct net_device *net_dev)
#else /* __rtems__ */
int dpa_eth_priv_stop(struct net_device *net_dev)
#endif /* __rtems__ */
{
	int err;
	struct dpa_priv_s *priv;

	err = dpa_stop(net_dev);
	/* Allow NAPI to consume any frame still in the Rx/TxConfirm
	 * ingress queues. This is to avoid a race between the current
	 * context and ksoftirqd which could leave NAPI disabled while
	 * in fact there's still Rx traffic to be processed.
	 */
	usleep_range(5000, 10000);

	priv = netdev_priv(net_dev);
	dpaa_eth_napi_disable(priv);

	return err;
}

#ifndef __rtems__
static const struct net_device_ops dpa_private_ops = {
	.ndo_open = dpa_eth_priv_start,
	.ndo_start_xmit = dpa_tx,
	.ndo_stop = dpa_eth_priv_stop,
	.ndo_tx_timeout = dpa_timeout,
	.ndo_get_stats64 = dpa_get_stats64,
	.ndo_set_mac_address = dpa_set_mac_address,
	.ndo_validate_addr = eth_validate_addr,
#ifdef CONFIG_FSL_DPAA_ETH_USE_NDO_SELECT_QUEUE
	.ndo_select_queue = dpa_select_queue,
#endif
	.ndo_change_mtu = dpa_change_mtu,
	.ndo_set_rx_mode = dpa_set_rx_mode,
	.ndo_init = dpa_ndo_init,
	.ndo_set_features = dpa_set_features,
	.ndo_fix_features = dpa_fix_features,
};
#endif /* __rtems__ */

static int dpa_private_napi_add(struct net_device *net_dev)
{
#ifndef __rtems__
	struct dpa_priv_s *priv = netdev_priv(net_dev);
	struct dpa_percpu_priv_s *percpu_priv;
	int i, cpu;

	for_each_possible_cpu(cpu) {
		percpu_priv = per_cpu_ptr(priv->percpu_priv, cpu);

		percpu_priv->np = devm_kzalloc(net_dev->dev.parent,
			qman_portal_max * sizeof(struct dpa_napi_portal),
			GFP_KERNEL);

		if (!percpu_priv->np)
			return -ENOMEM;

		for (i = 0; i < qman_portal_max; i++)
			netif_napi_add(net_dev, &percpu_priv->np[i].napi,
				       dpaa_eth_poll, DPA_NAPI_WEIGHT);
	}
#endif /* __rtems__ */

	return 0;
}

void dpa_private_napi_del(struct net_device *net_dev)
{
#ifndef __rtems__
	struct dpa_priv_s *priv = netdev_priv(net_dev);
	struct dpa_percpu_priv_s *percpu_priv;
	int i, cpu;

	for_each_possible_cpu(cpu) {
		percpu_priv = per_cpu_ptr(priv->percpu_priv, cpu);

		if (percpu_priv->np) {
			for (i = 0; i < qman_portal_max; i++)
				netif_napi_del(&percpu_priv->np[i].napi);

			devm_kfree(net_dev->dev.parent, percpu_priv->np);
		}
	}
#endif /* __rtems__ */
}

static int dpa_private_netdev_init(struct net_device *net_dev)
{
	int i;
	struct dpa_priv_s *priv = netdev_priv(net_dev);
	struct dpa_percpu_priv_s *percpu_priv;
#ifndef __rtems__
	const u8 *mac_addr;
#endif /* __rtems__ */

	/* Although we access another CPU's private data here
	 * we do it at initialization so it is safe
	 */
#ifndef __rtems__
	for_each_possible_cpu(i) {
#else /* __rtems__ */
	for (i = 0; i < (int)rtems_get_processor_count(); ++i) {
#endif /* __rtems__ */
		percpu_priv = per_cpu_ptr(priv->percpu_priv, i);
		percpu_priv->net_dev = net_dev;
	}

#ifndef __rtems__
	net_dev->netdev_ops = &dpa_private_ops;
	mac_addr = priv->mac_dev->addr;

	net_dev->mem_start = priv->mac_dev->res->start;
	net_dev->mem_end = priv->mac_dev->res->end;

	net_dev->hw_features |= (NETIF_F_IP_CSUM | NETIF_F_IPV6_CSUM |
		NETIF_F_LLTX);

	/* Advertise S/G and HIGHDMA support for private interfaces */
	net_dev->hw_features |= NETIF_F_SG | NETIF_F_HIGHDMA;
	/* Recent kernels enable GSO automatically, if
	 * we declare NETIF_F_SG. For conformity, we'll
	 * still declare GSO explicitly.
	 */
	net_dev->features |= NETIF_F_GSO;

	return dpa_netdev_init(net_dev, mac_addr, tx_timeout);
#else /* __rtems__ */
	return 0;
#endif /* __rtems__ */
}

static struct dpa_bp *dpa_priv_bp_probe(struct device *dev)
{
	struct dpa_bp *dpa_bp;

	dpa_bp = devm_kzalloc(dev, sizeof(*dpa_bp), GFP_KERNEL);
	if (!dpa_bp)
		return ERR_PTR(-ENOMEM);

	dpa_bp->percpu_count = devm_alloc_percpu(dev, *dpa_bp->percpu_count);
	dpa_bp->config_count = FSL_DPAA_ETH_MAX_BUF_COUNT;

	dpa_bp->seed_cb = dpa_bp_priv_seed;
	dpa_bp->free_buf_cb = _dpa_bp_free_pf;

	return dpa_bp;
}

/* Place all ingress FQs (Rx Default, Rx Error) in a dedicated CGR.
 * We won't be sending congestion notifications to FMan; for now, we just use
 * this CGR to generate enqueue rejections to FMan in order to drop the frames
 * before they reach our ingress queues and eat up memory.
 */
static int dpaa_eth_priv_ingress_cgr_init(struct dpa_priv_s *priv)
{
	struct qm_mcc_initcgr initcgr;
	u32 cs_th;
	int err;

	err = qman_alloc_cgrid(&priv->ingress_cgr.cgrid);
	if (err < 0) {
		pr_err("Error %d allocating CGR ID\n", err);
		goto out_error;
	}

	/* Enable CS TD, but disable Congestion State Change Notifications. */
	initcgr.we_mask = QM_CGR_WE_CS_THRES;
	initcgr.cgr.cscn_en = QM_CGR_EN;
	cs_th = DPAA_INGRESS_CS_THRESHOLD;
	qm_cgr_cs_thres_set64(&initcgr.cgr.cs_thres, cs_th, 1);

	initcgr.we_mask |= QM_CGR_WE_CSTD_EN;
	initcgr.cgr.cstd_en = QM_CGR_EN;

	/* This is actually a hack, because this CGR will be associated with
	 * our affine SWP. However, we'll place our ingress FQs in it.
	 */
	err = qman_create_cgr(&priv->ingress_cgr, QMAN_CGR_FLAG_USE_INIT,
			      &initcgr);
	if (err < 0) {
		pr_err("Error %d creating ingress CGR with ID %d\n", err,
		       priv->ingress_cgr.cgrid);
		qman_release_cgrid(priv->ingress_cgr.cgrid);
		goto out_error;
	}
	pr_debug("Created ingress CGR %d for netdev with hwaddr %pM\n",
		 priv->ingress_cgr.cgrid, priv->mac_dev->addr);

	/* struct qman_cgr allows special cgrid values (i.e. outside the 0..255
	 * range), but we have no common initialization path between the
	 * different variants of the DPAA Eth driver, so we do it here rather
	 * than modifying every other variant than "private Eth".
	 */
	priv->use_ingress_cgr = true;

out_error:
	return err;
}

static int dpa_priv_bp_create(struct net_device *net_dev, struct dpa_bp *dpa_bp,
			      size_t count)
{
	struct dpa_priv_s *priv = netdev_priv(net_dev);
	int i;

	netif_dbg(priv, probe, net_dev,
		  "Using private BM buffer pools\n");

	priv->bp_count = count;

	for (i = 0; i < count; i++) {
		int err;

		err = dpa_bp_alloc(&dpa_bp[i]);
		if (err < 0) {
			dpa_bp_free(priv);
			priv->dpa_bp = NULL;
			return err;
		}

		priv->dpa_bp = &dpa_bp[i];
	}

	dpa_priv_common_bpid = priv->dpa_bp->bpid;
	return 0;
}

#ifndef __rtems__
static const struct of_device_id dpa_match[];

static int
dpaa_eth_priv_probe(struct platform_device *pdev)
#else /* __rtems__ */
int
dpaa_eth_priv_probe(struct platform_device *pdev, struct mac_device *mac_dev)
#endif /* __rtems__ */
{
	int err = 0, i, channel;
	struct device *dev;
	struct dpa_bp *dpa_bp;
	struct dpa_fq *dpa_fq, *tmp;
	size_t count = 1;
	struct net_device *net_dev = NULL;
	struct dpa_priv_s *priv = NULL;
	struct dpa_percpu_priv_s *percpu_priv;
	struct fm_port_fqs port_fqs;
	struct dpa_buffer_layout_s *buf_layout = NULL;
#ifndef __rtems__
	struct mac_device *mac_dev;
	struct task_struct *kth;
#endif /* __rtems__ */

	dev = &pdev->dev;

	/* Get the buffer pool assigned to this interface;
	 * run only once the default pool probing code
	 */
	dpa_bp = (dpa_bpid2pool(dpa_priv_common_bpid)) ? :
			dpa_priv_bp_probe(dev);
	if (IS_ERR(dpa_bp))
		return PTR_ERR(dpa_bp);

#ifndef __rtems__
	/* Allocate this early, so we can store relevant information in
	 * the private area
	 */
	net_dev = alloc_etherdev_mq(sizeof(*priv), DPAA_ETH_TX_QUEUES);
	if (!net_dev) {
		dev_err(dev, "alloc_etherdev_mq() failed\n");
		goto alloc_etherdev_mq_failed;
	}
#else /* __rtems__ */
	net_dev = &mac_dev->net_dev;
	net_dev->priv = malloc(sizeof(*priv), M_KMALLOC, M_WAITOK | M_ZERO);
#endif /* __rtems__ */

#ifdef CONFIG_FSL_DPAA_ETH_FRIENDLY_IF_NAME
	snprintf(net_dev->name, IFNAMSIZ, "fm%d-mac%d",
		 dpa_mac_fman_index_get(pdev),
		 dpa_mac_hw_index_get(pdev));
#endif

	/* Do this here, so we can be verbose early */
#ifndef __rtems__
	SET_NETDEV_DEV(net_dev, dev);
#endif /* __rtems__ */
	dev_set_drvdata(dev, net_dev);

	priv = netdev_priv(net_dev);
	priv->net_dev = net_dev;

#ifndef __rtems__
	priv->msg_enable = netif_msg_init(debug, -1);

	mac_dev = dpa_mac_dev_get(pdev);
	if (IS_ERR(mac_dev) || !mac_dev) {
		err = PTR_ERR(mac_dev);
		goto mac_probe_failed;
	}
#endif /* __rtems__ */

	/* We have physical ports, so we need to establish
	 * the buffer layout.
	 */
	buf_layout = devm_kzalloc(dev, 2 * sizeof(*buf_layout),
				  GFP_KERNEL);
	if (!buf_layout)
		goto alloc_failed;

	dpa_set_buffers_layout(mac_dev, buf_layout);

	/* For private ports, need to compute the size of the default
	 * buffer pool, based on FMan port buffer layout;also update
	 * the maximum buffer size for private ports if necessary
	 */
	dpa_bp->size = dpa_bp_size(&buf_layout[RX]);

	INIT_LIST_HEAD(&priv->dpa_fq_list);

	memset(&port_fqs, 0, sizeof(port_fqs));

	err = dpa_fq_probe_mac(dev, &priv->dpa_fq_list, &port_fqs, true, RX);
	if (!err)
		err = dpa_fq_probe_mac(dev, &priv->dpa_fq_list,
				       &port_fqs, true, TX);

	if (err < 0)
		goto fq_probe_failed;

	/* bp init */

	err = dpa_priv_bp_create(net_dev, dpa_bp, count);

	if (err < 0)
		goto bp_create_failed;

	priv->mac_dev = mac_dev;

	channel = dpa_get_channel();

	if (channel < 0) {
		err = channel;
		goto get_channel_failed;
	}

	priv->channel = (u16)channel;

#ifndef __rtems__
	/* Start a thread that will walk the cpus with affine portals
	 * and add this pool channel to each's dequeue mask.
	 */
	kth = kthread_run(dpaa_eth_add_channel,
			  (void *)(unsigned long)priv->channel,
			  "dpaa_%p:%d", net_dev, priv->channel);
	if (!kth) {
		err = -ENOMEM;
		goto add_channel_failed;
	}
#else /* __rtems__ */
	dpaa_eth_add_channel((void *)(unsigned long)priv->channel);
#endif /* __rtems__ */

	dpa_fq_setup(priv, &private_fq_cbs, priv->mac_dev->port[TX]);

	/* Create a congestion group for this netdev, with
	 * dynamically-allocated CGR ID.
	 * Must be executed after probing the MAC, but before
	 * assigning the egress FQs to the CGRs.
	 */
	err = dpaa_eth_cgr_init(priv);
	if (err < 0) {
		dev_err(dev, "Error initializing CGR\n");
		goto tx_cgr_init_failed;
	}
	err = dpaa_eth_priv_ingress_cgr_init(priv);
	if (err < 0) {
		dev_err(dev, "Error initializing ingress CGR\n");
		goto rx_cgr_init_failed;
	}

	/* Add the FQs to the interface, and make them active */
	list_for_each_entry_safe(dpa_fq, tmp, &priv->dpa_fq_list, list) {
		err = dpa_fq_init(dpa_fq, false);
		if (err < 0)
			goto fq_alloc_failed;
	}

	priv->buf_layout = buf_layout;
	priv->tx_headroom = dpa_get_headroom(&priv->buf_layout[TX]);
	priv->rx_headroom = dpa_get_headroom(&priv->buf_layout[RX]);

	/* All real interfaces need their ports initialized */
	dpaa_eth_init_ports(mac_dev, dpa_bp, count, &port_fqs,
			    buf_layout, dev);

	priv->percpu_priv = devm_alloc_percpu(dev, *priv->percpu_priv);

	if (!priv->percpu_priv) {
		dev_err(dev, "devm_alloc_percpu() failed\n");
		err = -ENOMEM;
		goto alloc_percpu_failed;
	}
#ifndef __rtems__
	for_each_possible_cpu(i) {
#else /* __rtems__ */
	for (i = 0; i < (int)rtems_get_processor_count(); ++i) {
#endif /* __rtems__ */
		percpu_priv = per_cpu_ptr(priv->percpu_priv, i);
		memset(percpu_priv, 0, sizeof(*percpu_priv));
	}

	/* Initialize NAPI */
	err = dpa_private_napi_add(net_dev);

	if (err < 0)
		goto napi_add_failed;

	err = dpa_private_netdev_init(net_dev);

	if (err < 0)
		goto netdev_init_failed;

#ifndef __rtems__
	dpaa_eth_sysfs_init(&net_dev->dev);

	pr_info("Probed interface %s\n", net_dev->name);
#endif /* __rtems__ */

	return 0;

netdev_init_failed:
napi_add_failed:
	dpa_private_napi_del(net_dev);
alloc_percpu_failed:
#ifndef __rtems__
	dpa_fq_free(dev, &priv->dpa_fq_list);
#endif /* __rtems__ */
fq_alloc_failed:
#ifndef __rtems__
	qman_delete_cgr_safe(&priv->ingress_cgr);
	qman_release_cgrid(priv->ingress_cgr.cgrid);
#endif /* __rtems__ */
rx_cgr_init_failed:
#ifndef __rtems__
	qman_delete_cgr_safe(&priv->cgr_data.cgr);
	qman_release_cgrid(priv->cgr_data.cgr.cgrid);
#endif /* __rtems__ */
tx_cgr_init_failed:
#ifndef __rtems__
add_channel_failed:
#endif /* __rtems__ */
get_channel_failed:
	dpa_bp_free(priv);
bp_create_failed:
fq_probe_failed:
alloc_failed:
#ifndef __rtems__
mac_probe_failed:
#endif /* __rtems__ */
	dev_set_drvdata(dev, NULL);
#ifndef __rtems__
	free_netdev(net_dev);
alloc_etherdev_mq_failed:
	if (atomic_read(&dpa_bp->refs) == 0)
		devm_kfree(dev, dpa_bp);
#else /* __rtems__ */
	BSD_ASSERT(0);
#endif /* __rtems__ */

	return err;
}

#ifndef __rtems__
static struct platform_device_id dpa_devtype[] = {
	{
		.name = "dpaa-ethernet",
		.driver_data = 0,
	}, {
	}
};
MODULE_DEVICE_TABLE(platform, dpa_devtype);

static struct platform_driver dpa_driver = {
	.driver = {
		.name = KBUILD_MODNAME,
	},
	.id_table = dpa_devtype,
	.probe = dpaa_eth_priv_probe,
	.remove = dpa_remove
};

static int __init dpa_load(void)
{
	int err;

	pr_info(DPA_DESCRIPTION "\n");

	/* initialise dpaa_eth mirror values */
	dpa_rx_extra_headroom = fman_get_rx_extra_headroom();
	dpa_max_frm = fman_get_max_frm();

	err = platform_driver_register(&dpa_driver);
	if (err < 0)
		pr_err("Error, platform_driver_register() = %d\n", err);

	return err;
}
module_init(dpa_load);

static void __exit dpa_unload(void)
{
	platform_driver_unregister(&dpa_driver);

	/* Only one channel is used and needs to be relased after all
	 * interfaces are removed
	 */
	dpa_release_channel();
}
module_exit(dpa_unload);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Andy Fleming <afleming@freescale.com>");
MODULE_DESCRIPTION(DPA_DESCRIPTION);
#endif /* __rtems__ */
