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

#include <linux/init.h>
#include <linux/module.h>
#include <linux/of_platform.h>
#include <linux/of_net.h>
#include <linux/etherdevice.h>
#include <linux/kthread.h>
#include <linux/percpu.h>
#ifndef __rtems__
#include <linux/highmem.h>
#include <linux/sort.h>
#endif /* __rtems__ */
#include <soc/fsl/qman.h>
#ifndef __rtems__
#include <linux/ip.h>
#include <linux/ipv6.h>
#include <linux/if_vlan.h>
#endif /* __rtems__ */
#include "dpaa_eth.h"
#include "dpaa_eth_common.h"
#include "mac.h"

/* Size in bytes of the FQ taildrop threshold */
#define DPA_FQ_TD 0x200000

#define DPAA_CS_THRESHOLD_1G 0x06000000
/* Egress congestion threshold on 1G ports, range 0x1000 .. 0x10000000
 * The size in bytes of the egress Congestion State notification threshold on
 * 1G ports. The 1G dTSECs can quite easily be flooded by cores doing Tx in a
 * tight loop (e.g. by sending UDP datagrams at "while(1) speed"),
 * and the larger the frame size, the more acute the problem.
 * So we have to find a balance between these factors:
 * - avoiding the device staying congested for a prolonged time (risking
 *   the netdev watchdog to fire - see also the tx_timeout module param);
 * - affecting performance of protocols such as TCP, which otherwise
 *   behave well under the congestion notification mechanism;
 * - preventing the Tx cores from tightly-looping (as if the congestion
 *   threshold was too low to be effective);
 * - running out of memory if the CS threshold is set too high.
 */

#define DPAA_CS_THRESHOLD_10G 0x10000000
/* The size in bytes of the egress Congestion State notification threshold on
 * 10G ports, range 0x1000 .. 0x10000000
 */

static struct dpa_bp *dpa_bp_array[64];

#ifndef __rtems__
int dpa_max_frm;

int dpa_rx_extra_headroom;
#endif /* __rtems__ */

static const struct fqid_cell tx_confirm_fqids[] = {
	{0, DPAA_ETH_TX_QUEUES}
};

static const struct fqid_cell default_fqids[][3] = {
	[RX] = { {0, 1}, {0, 1}, {0, DPAA_ETH_RX_QUEUES} },
	[TX] = { {0, 1}, {0, 1}, {0, DPAA_ETH_TX_QUEUES} }
};

#ifndef __rtems__
int dpa_netdev_init(struct net_device *net_dev,
		    const u8 *mac_addr,
		    u16 tx_timeout)
{
	int err;
	struct dpa_priv_s *priv = netdev_priv(net_dev);
	struct device *dev = net_dev->dev.parent;

	net_dev->priv_flags |= IFF_LIVE_ADDR_CHANGE;
	/* we do not want shared skbs on TX */
	net_dev->priv_flags &= ~IFF_TX_SKB_SHARING;

	net_dev->features |= net_dev->hw_features;
	net_dev->vlan_features = net_dev->features;

	memcpy(net_dev->perm_addr, mac_addr, net_dev->addr_len);
	memcpy(net_dev->dev_addr, mac_addr, net_dev->addr_len);

	net_dev->ethtool_ops = &dpa_ethtool_ops;

	net_dev->needed_headroom = priv->tx_headroom;
	net_dev->watchdog_timeo = msecs_to_jiffies(tx_timeout);

	/* start without the RUNNING flag, phylib controls it later */
	netif_carrier_off(net_dev);

	err = register_netdev(net_dev);
	if (err < 0) {
		dev_err(dev, "register_netdev() = %d\n", err);
		return err;
	}

	return 0;
}
#endif /* __rtems__ */

int dpa_start(struct net_device *net_dev)
{
	int err, i;
	struct dpa_priv_s *priv;
	struct mac_device *mac_dev;

	priv = netdev_priv(net_dev);
	mac_dev = priv->mac_dev;

#ifndef __rtems__
	err = mac_dev->init_phy(net_dev, priv->mac_dev);
	if (err < 0) {
		netif_err(priv, ifup, net_dev, "init_phy() = %d\n", err);
		return err;
	}
#endif /* __rtems__ */

	for (i = 0; i < ARRAY_SIZE(mac_dev->port); i++) {
		err = fman_port_enable(mac_dev->port[i]);
		if (err)
			goto mac_start_failed;
	}

	err = priv->mac_dev->start(mac_dev);
	if (err < 0) {
		netif_err(priv, ifup, net_dev, "mac_dev->start() = %d\n", err);
		goto mac_start_failed;
	}

#ifndef __rtems__
	netif_tx_start_all_queues(net_dev);
#endif /* __rtems__ */

	return 0;

mac_start_failed:
	for (i = 0; i < ARRAY_SIZE(mac_dev->port); i++)
		fman_port_disable(mac_dev->port[i]);

	return err;
}

int dpa_stop(struct net_device *net_dev)
{
	int i, err, error;
	struct dpa_priv_s *priv;
	struct mac_device *mac_dev;

	priv = netdev_priv(net_dev);
	mac_dev = priv->mac_dev;

#ifndef __rtems__
	netif_tx_stop_all_queues(net_dev);
#endif /* __rtems__ */
	/* Allow the Fman (Tx) port to process in-flight frames before we
	 * try switching it off.
	 */
	usleep_range(5000, 10000);

	err = mac_dev->stop(mac_dev);
	if (err < 0)
		netif_err(priv, ifdown, net_dev, "mac_dev->stop() = %d\n",
			  err);

	for (i = 0; i < ARRAY_SIZE(mac_dev->port); i++) {
		error = fman_port_disable(mac_dev->port[i]);
		if (error)
			err = error;
	}

#ifndef __rtems__
	if (mac_dev->phy_dev)
		phy_disconnect(mac_dev->phy_dev);
	mac_dev->phy_dev = NULL;
#endif /* __rtems__ */

	return err;
}

#ifndef __rtems__
void dpa_timeout(struct net_device *net_dev)
{
	const struct dpa_priv_s	*priv;
	struct dpa_percpu_priv_s *percpu_priv;

	priv = netdev_priv(net_dev);
	percpu_priv = raw_cpu_ptr(priv->percpu_priv);

	netif_crit(priv, timer, net_dev, "Transmit timeout latency: %u ms\n",
		   jiffies_to_msecs(jiffies - net_dev->trans_start));

	percpu_priv->stats.tx_errors++;
}

/* Calculates the statistics for the given device by adding the statistics
 * collected by each CPU.
 */
struct rtnl_link_stats64 *dpa_get_stats64(struct net_device *net_dev,
					  struct rtnl_link_stats64 *stats)
{
	struct dpa_priv_s *priv = netdev_priv(net_dev);
	u64 *cpustats;
	u64 *netstats = (u64 *)stats;
	int i, j;
	struct dpa_percpu_priv_s *percpu_priv;
	int numstats = sizeof(struct rtnl_link_stats64) / sizeof(u64);

	for_each_possible_cpu(i) {
		percpu_priv = per_cpu_ptr(priv->percpu_priv, i);

		cpustats = (u64 *)&percpu_priv->stats;

		for (j = 0; j < numstats; j++)
			netstats[j] += cpustats[j];
	}

	return stats;
}
#endif /* __rtems__ */

int dpa_change_mtu(struct net_device *net_dev, int new_mtu)
{
	const int max_mtu = dpa_get_max_mtu();

	/* Make sure we don't exceed the Ethernet controller's MAXFRM */
	if (new_mtu < 68 || new_mtu > max_mtu) {
		netdev_err(net_dev, "Invalid L3 mtu %d (must be between %d and %d).\n",
			   new_mtu, 68, max_mtu);
		return -EINVAL;
	}
#ifndef __rtems__
	net_dev->mtu = new_mtu;
#endif /* __rtems__ */

	return 0;
}

#ifndef __rtems__
/* .ndo_init callback */
int dpa_ndo_init(struct net_device *net_dev)
{
	/* If fsl_fm_max_frm is set to a higher value than the all-common 1500,
	 * we choose conservatively and let the user explicitly set a higher
	 * MTU via ifconfig. Otherwise, the user may end up with different MTUs
	 * in the same LAN.
	 * If on the other hand fsl_fm_max_frm has been chosen below 1500,
	 * start with the maximum allowed.
	 */
	int init_mtu = min(dpa_get_max_mtu(), ETH_DATA_LEN);

	netdev_dbg(net_dev, "Setting initial MTU on net device: %d\n",
		   init_mtu);
	net_dev->mtu = init_mtu;

	return 0;
}

int dpa_set_features(struct net_device *dev, netdev_features_t features)
{
	/* Not much to do here for now */
	dev->features = features;
	return 0;
}

netdev_features_t dpa_fix_features(struct net_device *dev,
				   netdev_features_t features)
{
	netdev_features_t unsupported_features = 0;

	/* In theory we should never be requested to enable features that
	 * we didn't set in netdev->features and netdev->hw_features at probe
	 * time, but double check just to be on the safe side.
	 * We don't support enabling Rx csum through ethtool yet
	 */
	unsupported_features |= NETIF_F_RXCSUM;

	features &= ~unsupported_features;

	return features;
}

int dpa_remove(struct platform_device *pdev)
{
	int err;
	struct device *dev;
	struct net_device *net_dev;
	struct dpa_priv_s *priv;

	dev = &pdev->dev;
	net_dev = dev_get_drvdata(dev);

	priv = netdev_priv(net_dev);

	dpaa_eth_sysfs_remove(dev);

	dev_set_drvdata(dev, NULL);
	unregister_netdev(net_dev);

	err = dpa_fq_free(dev, &priv->dpa_fq_list);

	qman_delete_cgr_safe(&priv->ingress_cgr);
	qman_release_cgrid(priv->ingress_cgr.cgrid);
	qman_delete_cgr_safe(&priv->cgr_data.cgr);
	qman_release_cgrid(priv->cgr_data.cgr.cgrid);

	dpa_private_napi_del(net_dev);

	dpa_bp_free(priv);

	if (priv->buf_layout)
		devm_kfree(dev, priv->buf_layout);

	free_netdev(net_dev);

	return err;
}

struct mac_device *dpa_mac_dev_get(struct platform_device *pdev)
{
	struct device *dpa_dev, *dev;
	struct device_node *mac_node;
	struct platform_device *of_dev;
	struct mac_device *mac_dev;
	struct dpaa_eth_data *eth_data;

	dpa_dev = &pdev->dev;
	eth_data = dpa_dev->platform_data;
	if (!eth_data)
		return ERR_PTR(-ENODEV);

	mac_node = eth_data->mac_node;

	of_dev = of_find_device_by_node(mac_node);
	if (!of_dev) {
		dev_err(dpa_dev, "of_find_device_by_node(%s) failed\n",
			mac_node->full_name);
		of_node_put(mac_node);
		return ERR_PTR(-EINVAL);
	}
	of_node_put(mac_node);

	dev = &of_dev->dev;

	mac_dev = dev_get_drvdata(dev);
	if (!mac_dev) {
		dev_err(dpa_dev, "dev_get_drvdata(%s) failed\n",
			dev_name(dev));
		return ERR_PTR(-EINVAL);
	}

	return mac_dev;
}

int dpa_mac_hw_index_get(struct platform_device *pdev)
{
	struct device *dpa_dev;
	struct dpaa_eth_data *eth_data;

	dpa_dev = &pdev->dev;
	eth_data = dpa_dev->platform_data;

	return eth_data->mac_hw_id;
}

int dpa_mac_fman_index_get(struct platform_device *pdev)
{
	struct device *dpa_dev;
	struct dpaa_eth_data *eth_data;

	dpa_dev = &pdev->dev;
	eth_data = dpa_dev->platform_data;

	return eth_data->fman_hw_id;
}

int dpa_set_mac_address(struct net_device *net_dev, void *addr)
{
	const struct dpa_priv_s	*priv;
	int err;
	struct mac_device *mac_dev;

	priv = netdev_priv(net_dev);

	err = eth_mac_addr(net_dev, addr);
	if (err < 0) {
		netif_err(priv, drv, net_dev, "eth_mac_addr() = %d\n", err);
		return err;
	}

	mac_dev = priv->mac_dev;

	err = mac_dev->change_addr(mac_dev->fman_mac,
				   (enet_addr_t *)net_dev->dev_addr);
	if (err < 0) {
		netif_err(priv, drv, net_dev, "mac_dev->change_addr() = %d\n",
			  err);
		return err;
	}

	return 0;
}

void dpa_set_rx_mode(struct net_device *net_dev)
{
	int err;
	const struct dpa_priv_s	*priv;

	priv = netdev_priv(net_dev);

	if (!!(net_dev->flags & IFF_PROMISC) != priv->mac_dev->promisc) {
		priv->mac_dev->promisc = !priv->mac_dev->promisc;
		err = priv->mac_dev->set_promisc(priv->mac_dev->fman_mac,
						 priv->mac_dev->promisc);
		if (err < 0)
			netif_err(priv, drv, net_dev,
				  "mac_dev->set_promisc() = %d\n",
				  err);
	}

	err = priv->mac_dev->set_multi(net_dev, priv->mac_dev);
	if (err < 0)
		netif_err(priv, drv, net_dev, "mac_dev->set_multi() = %d\n",
			  err);
}
#endif /* __rtems__ */

void dpa_set_buffers_layout(struct mac_device *mac_dev,
			    struct dpa_buffer_layout_s *layout)
{
	/* Rx */
	layout[RX].priv_data_size = (u16)DPA_RX_PRIV_DATA_SIZE;
	layout[RX].parse_results = true;
	layout[RX].hash_results = true;
	layout[RX].data_align = DPA_FD_DATA_ALIGNMENT;

	/* Tx */
	layout[TX].priv_data_size = DPA_TX_PRIV_DATA_SIZE;
	layout[TX].parse_results = true;
	layout[TX].hash_results = true;
	layout[TX].data_align = DPA_FD_DATA_ALIGNMENT;
}

int dpa_bp_alloc(struct dpa_bp *dpa_bp)
{
	int err;
	struct bman_pool_params bp_params;
#ifndef __rtems__
	struct platform_device *pdev;
#endif /* __rtems__ */

	if (dpa_bp->size == 0 || dpa_bp->config_count == 0) {
		pr_err("Buffer pool is not properly initialized! Missing size or initial number of buffers");
		return -EINVAL;
	}

	memset(&bp_params, 0, sizeof(struct bman_pool_params));

	/* If the pool is already specified, we only create one per bpid */
	if (dpa_bpid2pool_use(dpa_bp->bpid))
		return 0;

	if (dpa_bp->bpid == 0)
		bp_params.flags |= BMAN_POOL_FLAG_DYNAMIC_BPID;
	else
		bp_params.bpid = dpa_bp->bpid;

	dpa_bp->pool = bman_new_pool(&bp_params);
	if (!dpa_bp->pool) {
		pr_err("bman_new_pool() failed\n");
		return -ENODEV;
	}

	dpa_bp->bpid = (u8)bman_get_params(dpa_bp->pool)->bpid;

#ifndef __rtems__
	pdev = platform_device_register_simple("DPAA_bpool",
					       dpa_bp->bpid, NULL, 0);
	if (IS_ERR(pdev)) {
		err = PTR_ERR(pdev);
		goto pdev_register_failed;
	}

	err = dma_set_mask(&pdev->dev, DMA_BIT_MASK(40));
	if (err)
		goto pdev_mask_failed;

	dpa_bp->dev = &pdev->dev;
#endif /* __rtems__ */

	if (dpa_bp->seed_cb) {
		err = dpa_bp->seed_cb(dpa_bp);
		if (err)
			goto pool_seed_failed;
	}

	dpa_bpid2pool_map(dpa_bp->bpid, dpa_bp);

	return 0;

pool_seed_failed:
#ifndef __rtems__
pdev_mask_failed:
	platform_device_unregister(pdev);
pdev_register_failed:
#endif /* __rtems__ */
	bman_free_pool(dpa_bp->pool);

	return err;
}

void dpa_bp_drain(struct dpa_bp *bp)
{
	int ret;
	u8 num = 8;

	do {
		struct bm_buffer bmb[8];
		int i;

		ret = bman_acquire(bp->pool, bmb, num, 0);
		if (ret < 0) {
			if (num == 8) {
				/* we have less than 8 buffers left;
				 * drain them one by one
				 */
				num = 1;
				ret = 1;
				continue;
			} else {
				/* Pool is fully drained */
				break;
			}
		}

		for (i = 0; i < num; i++) {
			dma_addr_t addr = bm_buf_addr(&bmb[i]);

#ifndef __rtems__
			dma_unmap_single(bp->dev, addr, bp->size,
					 DMA_BIDIRECTIONAL);
#endif /* __rtems__ */

			bp->free_buf_cb(phys_to_virt(addr));
		}
	} while (ret > 0);
}

static void _dpa_bp_free(struct dpa_bp *dpa_bp)
{
	struct dpa_bp *bp = dpa_bpid2pool(dpa_bp->bpid);

	/* the mapping between bpid and dpa_bp is done very late in the
	 * allocation procedure; if something failed before the mapping, the bp
	 * was not configured, therefore we don't need the below instructions
	 */
	if (!bp)
		return;

	if (!atomic_dec_and_test(&bp->refs))
		return;

	if (bp->free_buf_cb)
		dpa_bp_drain(bp);

	dpa_bp_array[bp->bpid] = NULL;
	bman_free_pool(bp->pool);

#ifndef __rtems__
	if (bp->dev)
		platform_device_unregister(to_platform_device(bp->dev));
#endif /* __rtems__ */
}

void dpa_bp_free(struct dpa_priv_s *priv)
{
	int i;

	for (i = 0; i < priv->bp_count; i++)
		_dpa_bp_free(&priv->dpa_bp[i]);
}

struct dpa_bp *dpa_bpid2pool(int bpid)
{
	return dpa_bp_array[bpid];
}

void dpa_bpid2pool_map(int bpid, struct dpa_bp *dpa_bp)
{
	dpa_bp_array[bpid] = dpa_bp;
	atomic_set(&dpa_bp->refs, 1);
}

bool dpa_bpid2pool_use(int bpid)
{
	if (dpa_bpid2pool(bpid)) {
		atomic_inc(&dpa_bp_array[bpid]->refs);
		return true;
	}

	return false;
}

#ifdef CONFIG_FSL_DPAA_ETH_USE_NDO_SELECT_QUEUE
u16 dpa_select_queue(struct net_device *net_dev, struct sk_buff *skb,
		     void *accel_priv, select_queue_fallback_t fallback)
{
	return dpa_get_queue_mapping(skb);
}
#endif

struct dpa_fq *dpa_fq_alloc(struct device *dev,
			    const struct fqid_cell *fqids,
			    struct list_head *list,
			    enum dpa_fq_type fq_type)
{
	int i;
	struct dpa_fq *dpa_fq;

	dpa_fq = devm_kzalloc(dev, sizeof(*dpa_fq) * fqids->count, GFP_KERNEL);
	if (!dpa_fq)
		return NULL;

	for (i = 0; i < fqids->count; i++) {
		dpa_fq[i].fq_type = fq_type;
		dpa_fq[i].fqid = fqids->start ? fqids->start + i : 0;
		list_add_tail(&dpa_fq[i].list, list);
	}

	for (i = 0; i < fqids->count; i++)
		_dpa_assign_wq(dpa_fq + i);

	return dpa_fq;
}

int dpa_fq_probe_mac(struct device *dev, struct list_head *list,
		     struct fm_port_fqs *port_fqs,
		     bool alloc_tx_conf_fqs,
		     enum port_type ptype)
{
	const struct fqid_cell *fqids;
	struct dpa_fq *dpa_fq;
	int num_ranges;
	int i;

	if (ptype == TX && alloc_tx_conf_fqs) {
		if (!dpa_fq_alloc(dev, tx_confirm_fqids, list,
				  FQ_TYPE_TX_CONF_MQ))
			goto fq_alloc_failed;
	}

	fqids = default_fqids[ptype];
	num_ranges = 3;

	for (i = 0; i < num_ranges; i++) {
		switch (i) {
		case 0:
			/* The first queue is the error queue */
			if (fqids[i].count != 1)
				goto invalid_error_queue;

			dpa_fq = dpa_fq_alloc(dev, &fqids[i], list,
					      ptype == RX ?
						FQ_TYPE_RX_ERROR :
						FQ_TYPE_TX_ERROR);
			if (!dpa_fq)
				goto fq_alloc_failed;

			if (ptype == RX)
				port_fqs->rx_errq = &dpa_fq[0];
			else
				port_fqs->tx_errq = &dpa_fq[0];
			break;
		case 1:
			/* the second queue is the default queue */
			if (fqids[i].count != 1)
				goto invalid_default_queue;

			dpa_fq = dpa_fq_alloc(dev, &fqids[i], list,
					      ptype == RX ?
						FQ_TYPE_RX_DEFAULT :
						FQ_TYPE_TX_CONFIRM);
			if (!dpa_fq)
				goto fq_alloc_failed;

			if (ptype == RX)
				port_fqs->rx_defq = &dpa_fq[0];
			else
				port_fqs->tx_defq = &dpa_fq[0];
			break;
		default:
			/* all subsequent queues are Tx */
			if (!dpa_fq_alloc(dev, &fqids[i], list, FQ_TYPE_TX))
				goto fq_alloc_failed;
			break;
		}
	}

	return 0;

fq_alloc_failed:
	dev_err(dev, "dpa_fq_alloc() failed\n");
	return -ENOMEM;

invalid_default_queue:
invalid_error_queue:
	dev_err(dev, "Too many default or error queues\n");
	return -EINVAL;
}

static u32 rx_pool_channel;
static DEFINE_SPINLOCK(rx_pool_channel_init);

int dpa_get_channel(void)
{
	spin_lock(&rx_pool_channel_init);
	if (!rx_pool_channel) {
		u32 pool;
		int ret = qman_alloc_pool(&pool);

		if (!ret)
			rx_pool_channel = pool;
	}
	spin_unlock(&rx_pool_channel_init);
	if (!rx_pool_channel)
		return -ENOMEM;
	return rx_pool_channel;
}

void dpa_release_channel(void)
{
	qman_release_pool(rx_pool_channel);
}

int dpaa_eth_add_channel(void *__arg)
{
#ifndef __rtems__
	const cpumask_t *cpus = qman_affine_cpus();
#endif /* __rtems__ */
	u32 pool = QM_SDQCR_CHANNELS_POOL_CONV((u16)(unsigned long)__arg);
	int cpu;
	struct qman_portal *portal;

#ifndef __rtems__
	for_each_cpu(cpu, cpus) {
#else /* __rtems__ */
	for (cpu = 0; cpu < (int)rtems_get_processor_count(); ++cpu) {
#endif /* __rtems__ */

		portal = (struct qman_portal *)qman_get_affine_portal(cpu);
		qman_p_static_dequeue_add(portal, pool);
	}
	return 0;
}

/* Congestion group state change notification callback.
 * Stops the device's egress queues while they are congested and
 * wakes them upon exiting congested state.
 * Also updates some CGR-related stats.
 */
static void dpaa_eth_cgscn(struct qman_portal *qm, struct qman_cgr *cgr,
			   int congested)
{
	struct dpa_priv_s *priv = (struct dpa_priv_s *)container_of(cgr,
		struct dpa_priv_s, cgr_data.cgr);

	if (congested) {
		priv->cgr_data.congestion_start_jiffies = jiffies;
#ifndef __rtems__
		netif_tx_stop_all_queues(priv->net_dev);
#else /* __rtems__ */
		BSD_ASSERT(0);
#endif /* __rtems__ */
		priv->cgr_data.cgr_congested_count++;
	} else {
		priv->cgr_data.congested_jiffies +=
			(jiffies - priv->cgr_data.congestion_start_jiffies);
#ifndef __rtems__
		netif_tx_wake_all_queues(priv->net_dev);
#else /* __rtems__ */
		BSD_ASSERT(0);
#endif /* __rtems__ */
	}
}

int dpaa_eth_cgr_init(struct dpa_priv_s *priv)
{
	struct qm_mcc_initcgr initcgr;
	u32 cs_th;
	int err;

	err = qman_alloc_cgrid(&priv->cgr_data.cgr.cgrid);
	if (err < 0) {
		pr_err("Error %d allocating CGR ID\n", err);
		goto out_error;
	}
	priv->cgr_data.cgr.cb = dpaa_eth_cgscn;

	/* Enable Congestion State Change Notifications and CS taildrop */
	initcgr.we_mask = QM_CGR_WE_CSCN_EN | QM_CGR_WE_CS_THRES;
	initcgr.cgr.cscn_en = QM_CGR_EN;

	/* Set different thresholds based on the MAC speed.
	 * This may turn suboptimal if the MAC is reconfigured at a speed
	 * lower than its max, e.g. if a dTSEC later negotiates a 100Mbps link.
	 * In such cases, we ought to reconfigure the threshold, too.
	 */
#ifndef __rtems__
	if (priv->mac_dev->if_support & SUPPORTED_10000baseT_Full)
		cs_th = DPAA_CS_THRESHOLD_10G;
	else
		cs_th = DPAA_CS_THRESHOLD_1G;
#else /* __rtems__ */
	/* FIXME */
	cs_th = DPAA_CS_THRESHOLD_1G;
#endif /* __rtems__ */
	qm_cgr_cs_thres_set64(&initcgr.cgr.cs_thres, cs_th, 1);

	initcgr.we_mask |= QM_CGR_WE_CSTD_EN;
	initcgr.cgr.cstd_en = QM_CGR_EN;

	err = qman_create_cgr(&priv->cgr_data.cgr, QMAN_CGR_FLAG_USE_INIT,
			      &initcgr);
	if (err < 0) {
		pr_err("Error %d creating CGR with ID %d\n", err,
		       priv->cgr_data.cgr.cgrid);
		qman_release_cgrid(priv->cgr_data.cgr.cgrid);
		goto out_error;
	}
	pr_debug("Created CGR %d for netdev with hwaddr %pM on QMan channel %d\n",
		 priv->cgr_data.cgr.cgrid, priv->mac_dev->addr,
		 priv->cgr_data.cgr.chan);

out_error:
	return err;
}

static inline void dpa_setup_ingress(const struct dpa_priv_s *priv,
				     struct dpa_fq *fq,
				     const struct qman_fq *template)
{
	fq->fq_base = *template;
	fq->net_dev = priv->net_dev;

	fq->flags = QMAN_FQ_FLAG_NO_ENQUEUE;
	fq->channel = priv->channel;
}

static inline void dpa_setup_egress(const struct dpa_priv_s *priv,
				    struct dpa_fq *fq,
				    struct fman_port *port,
				    const struct qman_fq *template)
{
	fq->fq_base = *template;
	fq->net_dev = priv->net_dev;

	if (port) {
		fq->flags = QMAN_FQ_FLAG_TO_DCPORTAL;
		fq->channel = (u16)fman_port_get_qman_channel_id(port);
	} else {
		fq->flags = QMAN_FQ_FLAG_NO_MODIFY;
	}
}

void dpa_fq_setup(struct dpa_priv_s *priv, const struct dpa_fq_cbs_t *fq_cbs,
		  struct fman_port *tx_port)
{
	struct dpa_fq *fq;
#ifndef __rtems__
	u16 portals[NR_CPUS];
	int cpu, num_portals = 0;
	const cpumask_t *affine_cpus = qman_affine_cpus();
#endif /* __rtems__ */
	int egress_cnt = 0, conf_cnt = 0;

#ifndef __rtems__
	for_each_cpu(cpu, affine_cpus)
		portals[num_portals++] = qman_affine_channel(cpu);
	if (num_portals == 0)
		dev_err(priv->net_dev->dev.parent,
			"No Qman software (affine) channels found");
#else /* __rtems__ */
	/* FIXME */
#endif /* __rtems__ */

	/* Initialize each FQ in the list */
	list_for_each_entry(fq, &priv->dpa_fq_list, list) {
		switch (fq->fq_type) {
		case FQ_TYPE_RX_DEFAULT:
			DPA_ERR_ON(!priv->mac_dev);
			dpa_setup_ingress(priv, fq, &fq_cbs->rx_defq);
			break;
		case FQ_TYPE_RX_ERROR:
			DPA_ERR_ON(!priv->mac_dev);
			dpa_setup_ingress(priv, fq, &fq_cbs->rx_errq);
			break;
		case FQ_TYPE_TX:
			dpa_setup_egress(priv, fq, tx_port,
					 &fq_cbs->egress_ern);
			/* If we have more Tx queues than the number of cores,
			 * just ignore the extra ones.
			 */
			if (egress_cnt < DPAA_ETH_TX_QUEUES)
				priv->egress_fqs[egress_cnt++] = &fq->fq_base;
			break;
		case FQ_TYPE_TX_CONFIRM:
			DPA_ERR_ON(!priv->mac_dev);
			dpa_setup_ingress(priv, fq, &fq_cbs->tx_defq);
			break;
		case FQ_TYPE_TX_CONF_MQ:
			DPA_ERR_ON(!priv->mac_dev);
			dpa_setup_ingress(priv, fq, &fq_cbs->tx_defq);
			priv->conf_fqs[conf_cnt++] = &fq->fq_base;
			break;
		case FQ_TYPE_TX_ERROR:
			DPA_ERR_ON(!priv->mac_dev);
			dpa_setup_ingress(priv, fq, &fq_cbs->tx_errq);
			break;
		default:
#ifndef __rtems__
			dev_warn(priv->net_dev->dev.parent,
				 "Unknown FQ type detected!\n");
#else /* __rtems__ */
			BSD_ASSERT(0);
#endif /* __rtems__ */
			break;
		}
	}

	/* The number of Tx queues may be smaller than the number of cores, if
	 * the Tx queue range is specified in the device tree instead of being
	 * dynamically allocated.
	 * Make sure all CPUs receive a corresponding Tx queue.
	 */
	while (egress_cnt < DPAA_ETH_TX_QUEUES) {
		list_for_each_entry(fq, &priv->dpa_fq_list, list) {
			if (fq->fq_type != FQ_TYPE_TX)
				continue;
			priv->egress_fqs[egress_cnt++] = &fq->fq_base;
			if (egress_cnt == DPAA_ETH_TX_QUEUES)
				break;
		}
	}
}

int dpa_fq_init(struct dpa_fq *dpa_fq, bool td_enable)
{
	int err;
	const struct dpa_priv_s	*priv;
#ifndef __rtems__
	struct device *dev;
#endif /* __rtems__ */
	struct qman_fq *fq;
	struct qm_mcc_initfq initfq;
	struct qman_fq *confq = NULL;
	int queue_id;

	priv = netdev_priv(dpa_fq->net_dev);
#ifndef __rtems__
	dev = dpa_fq->net_dev->dev.parent;
#endif /* __rtems__ */

	if (dpa_fq->fqid == 0)
		dpa_fq->flags |= QMAN_FQ_FLAG_DYNAMIC_FQID;

	dpa_fq->init = !(dpa_fq->flags & QMAN_FQ_FLAG_NO_MODIFY);

	err = qman_create_fq(dpa_fq->fqid, dpa_fq->flags, &dpa_fq->fq_base);
	if (err) {
#ifndef __rtems__
		dev_err(dev, "qman_create_fq() failed\n");
#else /* __rtems__ */
		BSD_ASSERT(0);
#endif /* __rtems__ */
		return err;
	}
	fq = &dpa_fq->fq_base;

	if (dpa_fq->init) {
		memset(&initfq, 0, sizeof(initfq));

		initfq.we_mask = QM_INITFQ_WE_FQCTRL;
		/* Note: we may get to keep an empty FQ in cache */
		initfq.fqd.fq_ctrl = QM_FQCTRL_PREFERINCACHE;

		/* Try to reduce the number of portal interrupts for
		 * Tx Confirmation FQs.
		 */
		if (dpa_fq->fq_type == FQ_TYPE_TX_CONFIRM)
			initfq.fqd.fq_ctrl |= QM_FQCTRL_HOLDACTIVE;

		/* FQ placement */
		initfq.we_mask |= QM_INITFQ_WE_DESTWQ;

		initfq.fqd.dest.channel	= dpa_fq->channel;
		initfq.fqd.dest.wq = dpa_fq->wq;

		/* Put all egress queues in a congestion group of their own.
		 * Sensu stricto, the Tx confirmation queues are Rx FQs,
		 * rather than Tx - but they nonetheless account for the
		 * memory footprint on behalf of egress traffic. We therefore
		 * place them in the netdev's CGR, along with the Tx FQs.
		 */
		if (dpa_fq->fq_type == FQ_TYPE_TX ||
		    dpa_fq->fq_type == FQ_TYPE_TX_CONFIRM ||
		    dpa_fq->fq_type == FQ_TYPE_TX_CONF_MQ) {
			initfq.we_mask |= QM_INITFQ_WE_CGID;
			initfq.fqd.fq_ctrl |= QM_FQCTRL_CGE;
			initfq.fqd.cgid = (u8)priv->cgr_data.cgr.cgrid;
			/* Set a fixed overhead accounting, in an attempt to
			 * reduce the impact of fixed-size skb shells and the
			 * driver's needed headroom on system memory. This is
			 * especially the case when the egress traffic is
			 * composed of small datagrams.
			 * Unfortunately, QMan's OAL value is capped to an
			 * insufficient value, but even that is better than
			 * no overhead accounting at all.
			 */
			initfq.we_mask |= QM_INITFQ_WE_OAC;
			initfq.fqd.oac_init.oac = QM_OAC_CG;
#ifndef __rtems__
			initfq.fqd.oac_init.oal =
				(signed char)(min(sizeof(struct sk_buff) +
						  priv->tx_headroom,
						  (size_t)FSL_QMAN_MAX_OAL));
#else /* __rtems__ */
			/* FIXME */
			initfq.fqd.oac_init.oal = FSL_QMAN_MAX_OAL;
#endif /* __rtems__ */
		}

		if (td_enable) {
			initfq.we_mask |= QM_INITFQ_WE_TDTHRESH;
			qm_fqd_taildrop_set(&initfq.fqd.td,
					    DPA_FQ_TD, 1);
			initfq.fqd.fq_ctrl = QM_FQCTRL_TDE;
		}

		/* Configure the Tx confirmation queue, now that we know
		 * which Tx queue it pairs with.
		 */
		if (dpa_fq->fq_type == FQ_TYPE_TX) {
			queue_id = _dpa_tx_fq_to_id(priv, &dpa_fq->fq_base);
			if (queue_id >= 0)
				confq = priv->conf_fqs[queue_id];
			if (confq) {
				initfq.we_mask |= QM_INITFQ_WE_CONTEXTA;
			/* ContextA: OVOM=1(use contextA2 bits instead of ICAD)
			 *	     A2V=1 (contextA A2 field is valid)
			 *	     A0V=1 (contextA A0 field is valid)
			 *	     B0V=1 (contextB field is valid)
			 * ContextA A2: EBD=1 (deallocate buffers inside FMan)
			 * ContextB B0(ASPID): 0 (absolute Virtual Storage ID)
			 */
				initfq.fqd.context_a.hi = 0x1e000000;
				initfq.fqd.context_a.lo = 0x80000000;
			}
		}

		/* Put all *private* ingress queues in our "ingress CGR". */
		if (priv->use_ingress_cgr &&
		    (dpa_fq->fq_type == FQ_TYPE_RX_DEFAULT ||
		     dpa_fq->fq_type == FQ_TYPE_RX_ERROR)) {
			initfq.we_mask |= QM_INITFQ_WE_CGID;
			initfq.fqd.fq_ctrl |= QM_FQCTRL_CGE;
			initfq.fqd.cgid = (u8)priv->ingress_cgr.cgrid;
			/* Set a fixed overhead accounting, just like for the
			 * egress CGR.
			 */
			initfq.we_mask |= QM_INITFQ_WE_OAC;
			initfq.fqd.oac_init.oac = QM_OAC_CG;
#ifndef __rtems__
			initfq.fqd.oac_init.oal =
				(signed char)(min(sizeof(struct sk_buff) +
				priv->tx_headroom, (size_t)FSL_QMAN_MAX_OAL));
#else /* __rtems__ */
			/* FIXME */
			initfq.fqd.oac_init.oal = FSL_QMAN_MAX_OAL;
#endif /* __rtems__ */
		}

		/* Initialization common to all ingress queues */
		if (dpa_fq->flags & QMAN_FQ_FLAG_NO_ENQUEUE) {
			initfq.we_mask |= QM_INITFQ_WE_CONTEXTA;
			initfq.fqd.fq_ctrl |=
				QM_FQCTRL_CTXASTASHING | QM_FQCTRL_AVOIDBLOCK;
			initfq.fqd.context_a.stashing.exclusive =
				QM_STASHING_EXCL_DATA | QM_STASHING_EXCL_CTX |
				QM_STASHING_EXCL_ANNOTATION;
			initfq.fqd.context_a.stashing.data_cl = 2;
			initfq.fqd.context_a.stashing.annotation_cl = 1;
			initfq.fqd.context_a.stashing.context_cl =
				DIV_ROUND_UP(sizeof(struct qman_fq), 64);
		}

		err = qman_init_fq(fq, QMAN_INITFQ_FLAG_SCHED, &initfq);
		if (err < 0) {
#ifndef __rtems__
			dev_err(dev, "qman_init_fq(%u) = %d\n",
				qman_fq_fqid(fq), err);
#endif /* __rtems__ */
			qman_destroy_fq(fq, 0);
			return err;
		}
	}

	dpa_fq->fqid = qman_fq_fqid(fq);

	return 0;
}

#ifndef __rtems__
static int _dpa_fq_free(struct device *dev, struct qman_fq *fq)
{
	int err, error;
	struct dpa_fq *dpa_fq;
	const struct dpa_priv_s	*priv;

	err = 0;

	dpa_fq = container_of(fq, struct dpa_fq, fq_base);
	priv = netdev_priv(dpa_fq->net_dev);

	if (dpa_fq->init) {
		err = qman_retire_fq(fq, NULL);
		if (err < 0 && netif_msg_drv(priv))
			dev_err(dev, "qman_retire_fq(%u) = %d\n",
				qman_fq_fqid(fq), err);

		error = qman_oos_fq(fq);
		if (error < 0 && netif_msg_drv(priv)) {
			dev_err(dev, "qman_oos_fq(%u) = %d\n",
				qman_fq_fqid(fq), error);
			if (err >= 0)
				err = error;
		}
	}

	qman_destroy_fq(fq, 0);
	list_del(&dpa_fq->list);

	return err;
}

int dpa_fq_free(struct device *dev, struct list_head *list)
{
	int err, error;
	struct dpa_fq *dpa_fq, *tmp;

	err = 0;
	list_for_each_entry_safe(dpa_fq, tmp, list, list) {
		error = _dpa_fq_free(dev, (struct qman_fq *)dpa_fq);
		if (error < 0 && err >= 0)
			err = error;
	}

	return err;
}
#endif /* __rtems__ */

static void
dpaa_eth_init_tx_port(struct fman_port *port, struct dpa_fq *errq,
		      struct dpa_fq *defq,
		      struct dpa_buffer_layout_s *buf_layout)
{
	struct fman_port_params params;
	struct fman_buffer_prefix_content buf_prefix_content;
	int err;

	memset(&params, 0, sizeof(params));
	memset(&buf_prefix_content, 0, sizeof(buf_prefix_content));

	buf_prefix_content.priv_data_size = buf_layout->priv_data_size;
	buf_prefix_content.pass_prs_result = buf_layout->parse_results;
	buf_prefix_content.pass_hash_result = buf_layout->hash_results;
	buf_prefix_content.pass_time_stamp = buf_layout->time_stamp;
	buf_prefix_content.data_align = buf_layout->data_align;

	params.specific_params.non_rx_params.err_fqid = errq->fqid;
	params.specific_params.non_rx_params.dflt_fqid = defq->fqid;

	err = fman_port_config(port, &params);
	if (err)
		pr_info("fman_port_config failed\n");

	err = fman_port_cfg_buf_prefix_content(port, &buf_prefix_content);
	if (err)
		pr_info("fman_port_cfg_buf_prefix_content failed\n");

	err = fman_port_init(port);
	if (err)
		pr_err("fm_port_init failed\n");
}

static void
dpaa_eth_init_rx_port(struct fman_port *port, struct dpa_bp *bp,
		      size_t count, struct dpa_fq *errq, struct dpa_fq *defq,
		      struct dpa_buffer_layout_s *buf_layout)
{
	struct fman_port_params params;
	struct fman_buffer_prefix_content buf_prefix_content;
	struct fman_port_rx_params *rx_p;
	int i, err;

	memset(&params, 0, sizeof(params));
	memset(&buf_prefix_content, 0, sizeof(buf_prefix_content));

	buf_prefix_content.priv_data_size = buf_layout->priv_data_size;
	buf_prefix_content.pass_prs_result = buf_layout->parse_results;
	buf_prefix_content.pass_hash_result = buf_layout->hash_results;
	buf_prefix_content.pass_time_stamp = buf_layout->time_stamp;
	buf_prefix_content.data_align = buf_layout->data_align;

	rx_p = &params.specific_params.rx_params;
	rx_p->err_fqid = errq->fqid;
	rx_p->dflt_fqid = defq->fqid;

	count = min(ARRAY_SIZE(rx_p->ext_buf_pools.ext_buf_pool), count);
	rx_p->ext_buf_pools.num_of_pools_used = (u8)count;
	for (i = 0; i < count; i++) {
		rx_p->ext_buf_pools.ext_buf_pool[i].id =  bp[i].bpid;
		rx_p->ext_buf_pools.ext_buf_pool[i].size = (u16)bp[i].size;
	}

	err = fman_port_config(port, &params);
	if (err)
		pr_info("fman_port_config failed\n");

	err = fman_port_cfg_buf_prefix_content(port, &buf_prefix_content);
	if (err)
		pr_info("fman_port_cfg_buf_prefix_content failed\n");

	err = fman_port_init(port);
	if (err)
		pr_err("fm_port_init failed\n");
}

void dpaa_eth_init_ports(struct mac_device *mac_dev,
			 struct dpa_bp *bp, size_t count,
			 struct fm_port_fqs *port_fqs,
			 struct dpa_buffer_layout_s *buf_layout,
			 struct device *dev)
{
	struct fman_port *rxport = mac_dev->port[RX];
	struct fman_port *txport = mac_dev->port[TX];

	dpaa_eth_init_tx_port(txport, port_fqs->tx_errq,
			      port_fqs->tx_defq, &buf_layout[TX]);
	dpaa_eth_init_rx_port(rxport, bp, count, port_fqs->rx_errq,
			      port_fqs->rx_defq, &buf_layout[RX]);
}

void dpa_release_sgt(struct qm_sg_entry *sgt)
{
	struct dpa_bp *dpa_bp;
	struct bm_buffer bmb[DPA_BUFF_RELEASE_MAX];
	u8 i = 0, j;

	memset(bmb, 0, sizeof(bmb));

	do {
		dpa_bp = dpa_bpid2pool(sgt[i].bpid);
		DPA_ERR_ON(!dpa_bp);

		j = 0;
		do {
			DPA_ERR_ON(sgt[i].extension);

			bmb[j].hi = sgt[i].addr_hi;
			bmb[j].lo = be32_to_cpu(sgt[i].addr_lo);

			j++; i++;
		} while (j < ARRAY_SIZE(bmb) &&
				!sgt[i - 1].final &&
				sgt[i - 1].bpid == sgt[i].bpid);

		while (bman_release(dpa_bp->pool, bmb, j, 0))
			cpu_relax();
	} while (!sgt[i - 1].final);
}

void dpa_fd_release(const struct net_device *net_dev, const struct qm_fd *fd)
{
	struct qm_sg_entry *sgt;
	struct dpa_bp *dpa_bp;
	struct bm_buffer bmb;
	dma_addr_t addr;
	void *vaddr;

	memset(&bmb, 0, sizeof(bmb));
	bm_buffer_set64(&bmb, fd->addr);

	dpa_bp = dpa_bpid2pool(fd->bpid);
	DPA_ERR_ON(!dpa_bp);

	if (fd->format == qm_fd_sg) {
		vaddr = phys_to_virt(fd->addr);
		sgt = vaddr + dpa_fd_offset(fd);

#ifndef __rtems__
		dma_unmap_single(dpa_bp->dev, qm_fd_addr(fd), dpa_bp->size,
				 DMA_BIDIRECTIONAL);
#endif /* __rtems__ */

		dpa_release_sgt(sgt);

#ifndef __rtems__
		addr = dma_map_single(dpa_bp->dev, vaddr, dpa_bp->size,
				      DMA_BIDIRECTIONAL);
		if (dma_mapping_error(dpa_bp->dev, addr)) {
			dev_err(dpa_bp->dev, "DMA mapping failed");
			return;
		}
#else /* __rtems__ */
		addr = (dma_addr_t)vaddr;
#endif /* __rtems__ */
		bm_buffer_set64(&bmb, addr);
	}

	while (bman_release(dpa_bp->pool, &bmb, 1, 0))
		cpu_relax();
}

void count_ern(struct dpa_percpu_priv_s *percpu_priv,
	       const struct qm_mr_entry *msg)
{
	switch (msg->ern.rc & QM_MR_RC_MASK) {
	case QM_MR_RC_CGR_TAILDROP:
		percpu_priv->ern_cnt.cg_tdrop++;
		break;
	case QM_MR_RC_WRED:
		percpu_priv->ern_cnt.wred++;
		break;
	case QM_MR_RC_ERROR:
		percpu_priv->ern_cnt.err_cond++;
		break;
	case QM_MR_RC_ORPWINDOW_EARLY:
		percpu_priv->ern_cnt.early_window++;
		break;
	case QM_MR_RC_ORPWINDOW_LATE:
		percpu_priv->ern_cnt.late_window++;
		break;
	case QM_MR_RC_FQ_TAILDROP:
		percpu_priv->ern_cnt.fq_tdrop++;
		break;
	case QM_MR_RC_ORPWINDOW_RETIRED:
		percpu_priv->ern_cnt.fq_retired++;
		break;
	case QM_MR_RC_ORP_ZERO:
		percpu_priv->ern_cnt.orp_zero++;
		break;
	}
}

#ifndef __rtems__
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
int dpa_enable_tx_csum(struct dpa_priv_s *priv,
		       struct sk_buff *skb,
		       struct qm_fd *fd,
		       char *parse_results)
{
	struct fman_prs_result *parse_result;
	struct iphdr *iph;
	struct ipv6hdr *ipv6h = NULL;
	u8 l4_proto;
	u16 ethertype = ntohs(skb->protocol);
	int retval = 0;

	if (skb->ip_summed != CHECKSUM_PARTIAL)
		return 0;

	/* Note: L3 csum seems to be already computed in sw, but we can't choose
	 * L4 alone from the FM configuration anyway.
	 */

	/* Fill in some fields of the Parse Results array, so the FMan
	 * can find them as if they came from the FMan Parser.
	 */
	parse_result = (struct fman_prs_result *)parse_results;

	/* If we're dealing with VLAN, get the real Ethernet type */
	if (ethertype == ETH_P_8021Q) {
		/* We can't always assume the MAC header is set correctly
		 * by the stack, so reset to beginning of skb->data
		 */
		skb_reset_mac_header(skb);
		ethertype = ntohs(vlan_eth_hdr(skb)->h_vlan_encapsulated_proto);
	}

	/* Fill in the relevant L3 parse result fields
	 * and read the L4 protocol type
	 */
	switch (ethertype) {
	case ETH_P_IP:
		parse_result->l3r = cpu_to_be16(FM_L3_PARSE_RESULT_IPV4);
		iph = ip_hdr(skb);
		DPA_ERR_ON(!iph);
		l4_proto = iph->protocol;
		break;
	case ETH_P_IPV6:
		parse_result->l3r = cpu_to_be16(FM_L3_PARSE_RESULT_IPV6);
		ipv6h = ipv6_hdr(skb);
		DPA_ERR_ON(!ipv6h);
		l4_proto = ipv6h->nexthdr;
		break;
	default:
		/* We shouldn't even be here */
		if (net_ratelimit())
			netif_alert(priv, tx_err, priv->net_dev,
				    "Can't compute HW csum for L3 proto 0x%x\n",
				    ntohs(skb->protocol));
		retval = -EIO;
		goto return_error;
	}

	/* Fill in the relevant L4 parse result fields */
	switch (l4_proto) {
	case IPPROTO_UDP:
		parse_result->l4r = FM_L4_PARSE_RESULT_UDP;
		break;
	case IPPROTO_TCP:
		parse_result->l4r = FM_L4_PARSE_RESULT_TCP;
		break;
	default:
		/* This can as well be a BUG() */
		if (net_ratelimit())
			netif_alert(priv, tx_err, priv->net_dev,
				    "Can't compute HW csum for L4 proto 0x%x\n",
				    l4_proto);
		retval = -EIO;
		goto return_error;
	}

	/* At index 0 is IPOffset_1 as defined in the Parse Results */
	parse_result->ip_off[0] = (u8)skb_network_offset(skb);
	parse_result->l4_off = (u8)skb_transport_offset(skb);

	/* Enable L3 (and L4, if TCP or UDP) HW checksum. */
	fd->cmd |= FM_FD_CMD_RPD | FM_FD_CMD_DTC;

	/* On P1023 and similar platforms fd->cmd interpretation could
	 * be disabled by setting CONTEXT_A bit ICMD; currently this bit
	 * is not set so we do not need to check; in the future, if/when
	 * using context_a we need to check this bit
	 */

return_error:
	return retval;
}
#endif /* __rtems__ */
