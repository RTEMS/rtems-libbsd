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

#ifndef __DPAA_ETH_COMMON_H
#define __DPAA_ETH_COMMON_H

#include <linux/etherdevice.h>
#include <soc/fsl/bman.h>
#include <linux/of_platform.h>

#include "dpaa_eth.h"

#define DPA_SGT_MAX_ENTRIES 16 /* maximum number of entries in SG Table */
#define DPA_BUFF_RELEASE_MAX 8 /* maximum number of buffers released at once */

/* used in napi related functions */
extern u16 qman_portal_max;

/* from dpa_ethtool.c */
extern const struct ethtool_ops dpa_ethtool_ops;

int dpa_netdev_init(struct net_device *net_dev,
		    const u8 *mac_addr,
		    u16 tx_timeout);
int dpa_start(struct net_device *net_dev);
int dpa_stop(struct net_device *net_dev);
void dpa_timeout(struct net_device *net_dev);
struct rtnl_link_stats64 *dpa_get_stats64(struct net_device *net_dev,
					  struct rtnl_link_stats64 *stats);
int dpa_change_mtu(struct net_device *net_dev, int new_mtu);
int dpa_ndo_init(struct net_device *net_dev);
#ifndef __rtems__
int dpa_set_features(struct net_device *dev, netdev_features_t features);
netdev_features_t dpa_fix_features(struct net_device *dev,
				   netdev_features_t features);
#endif /* __rtems__ */
int dpa_remove(struct platform_device *pdev);
struct mac_device *dpa_mac_dev_get(struct platform_device *pdev);
int dpa_mac_hw_index_get(struct platform_device *pdev);
int dpa_mac_fman_index_get(struct platform_device *pdev);
int dpa_set_mac_address(struct net_device *net_dev, void *addr);
void dpa_set_rx_mode(struct net_device *net_dev);
void dpa_set_buffers_layout(struct mac_device *mac_dev,
			    struct dpa_buffer_layout_s *layout);
int dpa_bp_alloc(struct dpa_bp *dpa_bp);
void dpa_bp_free(struct dpa_priv_s *priv);
struct dpa_bp *dpa_bpid2pool(int bpid);
void dpa_bpid2pool_map(int bpid, struct dpa_bp *dpa_bp);
bool dpa_bpid2pool_use(int bpid);
void dpa_bp_drain(struct dpa_bp *bp);
#ifdef CONFIG_FSL_DPAA_ETH_USE_NDO_SELECT_QUEUE
u16 dpa_select_queue(struct net_device *net_dev, struct sk_buff *skb,
		     void *accel_priv, select_queue_fallback_t fallback);
#endif
struct dpa_fq *dpa_fq_alloc(struct device *dev,
			    const struct fqid_cell *fqids,
			    struct list_head *list,
			    enum dpa_fq_type fq_type);
int dpa_fq_probe_mac(struct device *dev, struct list_head *list,
		     struct fm_port_fqs *port_fqs,
		     bool tx_conf_fqs_per_core,
		     enum port_type ptype);
int dpa_get_channel(void);
void dpa_release_channel(void);
int dpaa_eth_add_channel(void *__arg);
int dpaa_eth_cgr_init(struct dpa_priv_s *priv);
void dpa_fq_setup(struct dpa_priv_s *priv, const struct dpa_fq_cbs_t *fq_cbs,
		  struct fman_port *tx_port);
int dpa_fq_init(struct dpa_fq *dpa_fq, bool td_enable);
int dpa_fq_free(struct device *dev, struct list_head *list);
void dpaa_eth_init_ports(struct mac_device *mac_dev,
			 struct dpa_bp *bp, size_t count,
			 struct fm_port_fqs *port_fqs,
			 struct dpa_buffer_layout_s *buf_layout,
			 struct device *dev);
void dpa_release_sgt(struct qm_sg_entry *sgt);
void dpa_fd_release(const struct net_device *net_dev, const struct qm_fd *fd);
void count_ern(struct dpa_percpu_priv_s *percpu_priv,
	       const struct qm_mr_entry *msg);
#ifndef __rtems__
int dpa_enable_tx_csum(struct dpa_priv_s *priv,
		       struct sk_buff *skb,
		       struct qm_fd *fd,
		       char *parse_results);
#endif /* __rtems__ */
#endif	/* __DPAA_ETH_COMMON_H */
