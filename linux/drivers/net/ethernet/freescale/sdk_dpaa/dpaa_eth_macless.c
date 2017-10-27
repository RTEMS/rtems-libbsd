#include <machine/rtems-bsd-kernel-space.h>

/* Copyright 2008-2013 Freescale Semiconductor Inc.
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

#ifdef CONFIG_FSL_DPAA_ETH_DEBUG
#define pr_fmt(fmt) \
	KBUILD_MODNAME ": %s:%hu:%s() " fmt, \
	KBUILD_BASENAME".c", __LINE__, __func__
#else
#define pr_fmt(fmt) \
	KBUILD_MODNAME ": " fmt
#endif

#include <linux/init.h>
#include <linux/module.h>
#include <linux/of_platform.h>
#include <linux/of_net.h>
#include <linux/etherdevice.h>
#include <linux/kthread.h>
#include <linux/percpu.h>
#include <linux/highmem.h>
#include <linux/fsl_qman.h>
#include "dpaa_eth.h"
#include "dpaa_eth_common.h"
#include "dpaa_eth_base.h"
#include "lnxwrp_fsl_fman.h" /* fm_get_rx_extra_headroom(), fm_get_max_frm() */
#include "mac.h"

/* For MAC-based interfaces, we compute the tx needed headroom from the
 * associated Tx port's buffer layout settings.
 * For MACless interfaces just use a default value.
 */
#define DPA_DEFAULT_TX_HEADROOM	64

#define DPA_DESCRIPTION "FSL DPAA MACless Ethernet driver"

MODULE_LICENSE("Dual BSD/GPL");

MODULE_DESCRIPTION(DPA_DESCRIPTION);

/* This has to work in tandem with the DPA_CS_THRESHOLD_xxx values. */
static uint16_t macless_tx_timeout = 1000;
module_param(macless_tx_timeout, ushort, S_IRUGO);
MODULE_PARM_DESC(macless_tx_timeout, "The MACless Tx timeout in ms");

/* forward declarations */
static int __cold dpa_macless_start(struct net_device *net_dev);
static int __cold dpa_macless_stop(struct net_device *net_dev);
static int __cold dpa_macless_set_address(struct net_device *net_dev,
					  void *addr);
static void __cold dpa_macless_set_rx_mode(struct net_device *net_dev);

static int dpaa_eth_macless_probe(struct platform_device *_of_dev);
static netdev_features_t
dpa_macless_fix_features(struct net_device *dev, netdev_features_t features);

static const struct net_device_ops dpa_macless_ops = {
	.ndo_open = dpa_macless_start,
	.ndo_start_xmit = dpa_shared_tx,
	.ndo_stop = dpa_macless_stop,
	.ndo_tx_timeout = dpa_timeout,
	.ndo_get_stats64 = dpa_get_stats64,
	.ndo_set_mac_address = dpa_macless_set_address,
	.ndo_set_rx_mode = dpa_macless_set_rx_mode,
	.ndo_validate_addr = eth_validate_addr,
#ifdef CONFIG_FSL_DPAA_ETH_USE_NDO_SELECT_QUEUE
	.ndo_select_queue = dpa_select_queue,
#endif
	.ndo_change_mtu = dpa_change_mtu,
	.ndo_init = dpa_ndo_init,
	.ndo_set_features = dpa_set_features,
	.ndo_fix_features = dpa_macless_fix_features,
};

static const struct of_device_id dpa_macless_match[] = {
	{
		.compatible	= "fsl,dpa-ethernet-macless"
	},
	{}
};
MODULE_DEVICE_TABLE(of, dpa_macless_match);

static struct platform_driver dpa_macless_driver = {
	.driver = {
		.name		= KBUILD_MODNAME "-macless",
		.of_match_table	= dpa_macless_match,
		.owner		= THIS_MODULE,
	},
	.probe		= dpaa_eth_macless_probe,
	.remove		= dpa_remove
};

static const char macless_frame_queues[][25] = {
	[RX] = "fsl,qman-frame-queues-rx",
	[TX] = "fsl,qman-frame-queues-tx"
};

static int __cold dpa_macless_start(struct net_device *net_dev)
{
	const struct dpa_priv_s *priv = netdev_priv(net_dev);
	struct proxy_device *proxy_dev = (struct proxy_device *)priv->peer;

	netif_tx_start_all_queues(net_dev);

	if (proxy_dev)
		dpa_proxy_start(net_dev);


	return 0;
}

static int __cold dpa_macless_stop(struct net_device *net_dev)
{
	const struct dpa_priv_s *priv = netdev_priv(net_dev);
	struct proxy_device *proxy_dev = (struct proxy_device *)priv->peer;

	netif_tx_stop_all_queues(net_dev);

	if (proxy_dev)
		dpa_proxy_stop(proxy_dev, net_dev);

	return 0;
}

static int dpa_macless_set_address(struct net_device *net_dev, void *addr)
{
	const struct dpa_priv_s *priv = netdev_priv(net_dev);
	struct proxy_device *proxy_dev = (struct proxy_device *)priv->peer;
	int			 _errno;

	_errno = eth_mac_addr(net_dev, addr);
	if (_errno < 0) {
		if (netif_msg_drv(priv))
			netdev_err(net_dev, "eth_mac_addr() = %d\n", _errno);
		return _errno;
	}

	if (proxy_dev) {
		_errno = dpa_proxy_set_mac_address(proxy_dev, net_dev);
		if (_errno < 0) {
			if (netif_msg_drv(priv))
				netdev_err(net_dev, "proxy_set_mac_address() = %d\n",
						_errno);
			return _errno;
		}
	}

	return 0;
}

static void __cold dpa_macless_set_rx_mode(struct net_device *net_dev)
{
	const struct dpa_priv_s	*priv = netdev_priv(net_dev);
	struct proxy_device *proxy_dev = (struct proxy_device *)priv->peer;

	if (proxy_dev)
		dpa_proxy_set_rx_mode(proxy_dev, net_dev);
}

static netdev_features_t
dpa_macless_fix_features(struct net_device *dev, netdev_features_t features)
{
	netdev_features_t unsupported_features = 0;

	/* In theory we should never be requested to enable features that
	 * we didn't set in netdev->features and netdev->hw_features at probe
	 * time, but double check just to be on the safe side.
	 */
	unsupported_features |= NETIF_F_IP_CSUM | NETIF_F_IPV6_CSUM;
	/* We don't support enabling Rx csum through ethtool yet */
	unsupported_features |= NETIF_F_RXCSUM;

	features &= ~unsupported_features;

	return features;
}

static int dpa_macless_netdev_init(struct device_node *dpa_node,
				struct net_device *net_dev)
{
	struct dpa_priv_s *priv = netdev_priv(net_dev);
	struct proxy_device *proxy_dev = (struct proxy_device *)priv->peer;
	struct device *dev = net_dev->dev.parent;
	const uint8_t *mac_addr;

	net_dev->netdev_ops = &dpa_macless_ops;

	if (proxy_dev) {
		struct mac_device *mac_dev = proxy_dev->mac_dev;
		net_dev->mem_start = mac_dev->res->start;
		net_dev->mem_end = mac_dev->res->end;

		return dpa_netdev_init(net_dev, mac_dev->addr,
				macless_tx_timeout);
	} else {
		/* Get the MAC address from device tree */
		mac_addr = of_get_mac_address(dpa_node);

		if (mac_addr == NULL) {
			if (netif_msg_probe(priv))
				dev_err(dev, "No MAC address found!\n");
			return -EINVAL;
		}

		return dpa_netdev_init(net_dev, mac_addr,
				macless_tx_timeout);
	}
}

/* Probing of FQs for MACless ports */
static int dpa_fq_probe_macless(struct device *dev, struct list_head *list,
				enum port_type ptype)
{
	struct device_node *np = dev->of_node;
	const struct fqid_cell *fqids;
	int num_ranges;
	int i, lenp;

	fqids = of_get_property(np, macless_frame_queues[ptype], &lenp);
	if (fqids == NULL) {
		dev_err(dev, "Need FQ definition in dts for MACless devices\n");
		return -EINVAL;
	}

	num_ranges = lenp / sizeof(*fqids);

	/* All ranges defined in the device tree are used as Rx/Tx queues */
	for (i = 0; i < num_ranges; i++) {
		if (!dpa_fq_alloc(dev, be32_to_cpu(fqids[i].start),
				  be32_to_cpu(fqids[i].count), list,
				  ptype == RX ? FQ_TYPE_RX_PCD : FQ_TYPE_TX)) {
			dev_err(dev, "_dpa_fq_alloc() failed\n");
			return -ENOMEM;
		}
	}

	return 0;
}

	static struct proxy_device *
dpa_macless_proxy_probe(struct platform_device *_of_dev)
{
	struct device		*dev;
	const phandle		*proxy_prop;
	struct proxy_device	*proxy_dev;
	struct device_node	*proxy_node;
	struct platform_device  *proxy_pdev;
	int lenp;

	dev = &_of_dev->dev;

	proxy_prop = of_get_property(dev->of_node, "proxy", &lenp);
	if (!proxy_prop)
		return NULL;

	proxy_node = of_find_node_by_phandle(*proxy_prop);
	if (!proxy_node) {
		dev_err(dev, "Cannot find proxy node\n");
		return NULL;
	}

	proxy_pdev = of_find_device_by_node(proxy_node);
	if (!proxy_pdev) {
		of_node_put(proxy_node);
		dev_err(dev, "Cannot find device represented by proxy node\n");
		return NULL;
	}

	proxy_dev = dev_get_drvdata(&proxy_pdev->dev);

	of_node_put(proxy_node);

	return proxy_dev;
}

static int dpaa_eth_macless_probe(struct platform_device *_of_dev)
{
	int err = 0, i, channel;
	struct device *dev;
	struct device_node *dpa_node;
	struct dpa_bp *dpa_bp;
	size_t count;
	struct net_device *net_dev = NULL;
	struct dpa_priv_s *priv = NULL;
	struct dpa_percpu_priv_s *percpu_priv;
	static struct proxy_device *proxy_dev;
	struct task_struct *kth;
	static u8 macless_idx;

	dev = &_of_dev->dev;

	dpa_node = dev->of_node;

	if (!of_device_is_available(dpa_node))
		return -ENODEV;

	/* Get the buffer pools assigned to this interface */
	dpa_bp = dpa_bp_probe(_of_dev, &count);
	if (IS_ERR(dpa_bp))
		return PTR_ERR(dpa_bp);

	for (i = 0; i < count; i++)
		dpa_bp[i].seed_cb = dpa_bp_shared_port_seed;

	proxy_dev = dpa_macless_proxy_probe(_of_dev);


	/* Allocate this early, so we can store relevant information in
	 * the private area (needed by 1588 code in dpa_mac_probe)
	 */
	net_dev = alloc_etherdev_mq(sizeof(*priv), DPAA_ETH_TX_QUEUES);
	if (!net_dev) {
		dev_err(dev, "alloc_etherdev_mq() failed\n");
		return -ENOMEM;
	}

	/* Do this here, so we can be verbose early */
	SET_NETDEV_DEV(net_dev, dev);
	dev_set_drvdata(dev, net_dev);

	priv = netdev_priv(net_dev);
	priv->net_dev = net_dev;
	sprintf(priv->if_type, "macless%d", macless_idx++);

	priv->msg_enable = netif_msg_init(advanced_debug, -1);

	priv->peer = NULL;
	priv->mac_dev = NULL;
	if (proxy_dev) {
		/* This is a temporary solution for the need of
		 * having main driver upstreamability: adjust_link
		 * is a general function that should work for both
		 * private driver and macless driver with MAC device
		 * control capabilities even if the last will not be
		 * upstreamable.
		 * TODO: find a convenient solution (wrapper over
		 * main priv structure, etc.)
		 */
		priv->mac_dev = proxy_dev->mac_dev;

		/* control over proxy's mac device */
		priv->peer = (void *)proxy_dev;
	}

	INIT_LIST_HEAD(&priv->dpa_fq_list);

	err = dpa_fq_probe_macless(dev, &priv->dpa_fq_list, RX);
	if (!err)
		err = dpa_fq_probe_macless(dev, &priv->dpa_fq_list,
					   TX);
	if (err < 0)
		goto fq_probe_failed;

	/* bp init */
	priv->bp_count = count;
	err = dpa_bp_create(net_dev, dpa_bp, count);
	if (err < 0)
		goto bp_create_failed;

	channel = dpa_get_channel();

	if (channel < 0) {
		err = channel;
		goto get_channel_failed;
	}

	priv->channel = (uint16_t)channel;

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

	dpa_fq_setup(priv, &shared_fq_cbs, NULL);

	/* Add the FQs to the interface, and make them active */
	/* For MAC-less devices we only get here for RX frame queues
	 * initialization, which are the TX queues of the other
	 * partition.
	 * It is safe to rely on one partition to set the FQ taildrop
	 * threshold for the TX queues of the other partition
	 * because the ERN notifications will be received by the
	 * partition doing qman_enqueue.
	 */
	err = dpa_fqs_init(dev,  &priv->dpa_fq_list, true);
	if (err < 0)
		goto fq_alloc_failed;

	priv->tx_headroom = DPA_DEFAULT_TX_HEADROOM;

	priv->percpu_priv = devm_alloc_percpu(dev, *priv->percpu_priv);

	if (priv->percpu_priv == NULL) {
		dev_err(dev, "devm_alloc_percpu() failed\n");
		err = -ENOMEM;
		goto alloc_percpu_failed;
	}
	for_each_possible_cpu(i) {
		percpu_priv = per_cpu_ptr(priv->percpu_priv, i);
		memset(percpu_priv, 0, sizeof(*percpu_priv));
	}

	err = dpa_macless_netdev_init(dpa_node, net_dev);
	if (err < 0)
		goto netdev_init_failed;

	dpaa_eth_sysfs_init(&net_dev->dev);

	pr_info("fsl_dpa_macless: Probed %s interface as %s\n",
			priv->if_type, net_dev->name);

	return 0;

netdev_init_failed:
alloc_percpu_failed:
fq_alloc_failed:
	if (net_dev)
		dpa_fq_free(dev, &priv->dpa_fq_list);
add_channel_failed:
get_channel_failed:
	if (net_dev)
		dpa_bp_free(priv);
bp_create_failed:
fq_probe_failed:
	dev_set_drvdata(dev, NULL);
	if (net_dev)
		free_netdev(net_dev);

	return err;
}

static int __init __cold dpa_macless_load(void)
{
	int	 _errno;

	pr_info(DPA_DESCRIPTION "\n");

	/* Initialize dpaa_eth mirror values */
	dpa_rx_extra_headroom = fm_get_rx_extra_headroom();
	dpa_max_frm = fm_get_max_frm();

	_errno = platform_driver_register(&dpa_macless_driver);
	if (unlikely(_errno < 0)) {
		pr_err(KBUILD_MODNAME
			": %s:%hu:%s(): platform_driver_register() = %d\n",
			KBUILD_BASENAME".c", __LINE__, __func__, _errno);
	}

	pr_debug(KBUILD_MODNAME ": %s:%s() ->\n",
		KBUILD_BASENAME".c", __func__);

	return _errno;
}
module_init(dpa_macless_load);

static void __exit __cold dpa_macless_unload(void)
{
	platform_driver_unregister(&dpa_macless_driver);

	pr_debug(KBUILD_MODNAME ": %s:%s() ->\n",
		KBUILD_BASENAME".c", __func__);
}
module_exit(dpa_macless_unload);
