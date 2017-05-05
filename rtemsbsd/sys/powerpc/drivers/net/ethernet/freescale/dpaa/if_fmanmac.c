#include <machine/rtems-bsd-kernel-space.h>
#include <rtems/bsd/local/opt_dpaa.h>

/*
 * Copyright 2012 - 2015 Freescale Semiconductor Inc.
 * Copyright (c) 2016 embedded brains GmbH
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *     * Neither the name of Freescale Semiconductor nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
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

#include <machine/rtems-bsd-kernel-space.h>

#include "if_fmanmac.h"

#include <sys/sockio.h>

#include <net/if_vlan_var.h>
#include <netinet/ip.h>

#include <linux/phy.h>

#include "../../../../../../../../linux/drivers/net/ethernet/freescale/dpaa/dpaa_eth.h"

#define	FMAN_MAC_LOCK(sc)		mtx_lock(&(sc)->mtx)
#define	FMAN_MAC_UNLOCK(sc)		mtx_unlock(&(sc)->mtx)
#define	FMAN_MAC_ASSERT_LOCKED(sc)	mtx_assert(&(sc)->mtx, MA_OWNED)

#define	FMAN_MAC_CSUM (CSUM_IP | CSUM_TCP | CSUM_UDP | CSUM_TCP_IPV6 | \
    CSUM_UDP_IPV6)

struct fman_mac_sgt {
	char priv[DPAA_TX_PRIV_DATA_SIZE];
	struct fman_prs_result prs;
	struct qm_sg_entry sg[DPAA_SGT_MAX_ENTRIES];
	struct mbuf *m;
};

static void
fman_mac_enable_tx_csum(struct mbuf *m, struct qm_fd *fd,
    struct fman_prs_result *prs)
{
	int csum_flags = m->m_pkthdr.csum_flags;

	if ((csum_flags & FMAN_MAC_CSUM) == 0) {
		return;
	}

	memset(prs, 0, sizeof(*prs));

	if ((csum_flags & FMAN_MAC_CSUM) == CSUM_IP) {
		prs->l3r = FM_L3_PARSE_RESULT_IPV4;
	} else if ((csum_flags & CSUM_TCP) != 0) {
		prs->l3r = FM_L3_PARSE_RESULT_IPV4;
		prs->l4r = FM_L4_PARSE_RESULT_TCP;
	} else if ((csum_flags & CSUM_UDP) != 0) {
		prs->l3r = FM_L3_PARSE_RESULT_IPV4;
		prs->l4r = FM_L4_PARSE_RESULT_UDP;
	} else if ((csum_flags & CSUM_TCP_IPV6) != 0) {
		prs->l3r = FM_L3_PARSE_RESULT_IPV6;
		prs->l4r = FM_L4_PARSE_RESULT_TCP;
	} else if ((csum_flags & CSUM_UDP_IPV6) != 0) {
		prs->l3r = FM_L3_PARSE_RESULT_IPV6;
		prs->l4r = FM_L4_PARSE_RESULT_UDP;
	} else {
		BSD_ASSERT(0);
	}

	/* FIXME: VLAN */
	prs->ip_off[0] = (u8)sizeof(struct ether_header);
	prs->l4_off = (u8)(sizeof(struct ether_header) + sizeof(struct ip));

	fd->cmd |= FM_FD_CMD_RPD | FM_FD_CMD_DTC;
}

static void
fman_mac_txstart_locked(struct ifnet *ifp, struct fman_mac_softc *sc)
{

	FMAN_MAC_ASSERT_LOCKED(sc);

	for (;;) {
		struct fman_mac_sgt *sgt;
		struct mbuf *m;
		struct mbuf *n;
		struct qm_fd fd;
		struct dpaa_priv *priv;
		struct qman_fq *egress_fq;
		int queue = 0;
		size_t i;
		int err;

		IFQ_DRV_DEQUEUE(&ifp->if_snd, m);
		if (m == NULL) {
			break;
		}

		sgt = uma_zalloc(sc->sgt_zone, M_NOWAIT);
		if (sgt == NULL) {
			if_inc_counter(ifp, IFCOUNTER_OQDROPS, 1);
			m_freem(m);
			continue;
		}

		qm_fd_clear_fd(&fd);
		qm_fd_set_sg(&fd, offsetof(struct fman_mac_sgt, sg), m->m_pkthdr.len);
		fd.bpid = FSL_DPAA_BPID_INV;
		fd.cmd |= cpu_to_be32(FM_FD_CMD_FCO);
		qm_fd_addr_set64(&fd, (uintptr_t)sgt);
		fman_mac_enable_tx_csum(m, &fd, &sgt->prs);

repeat_with_collapsed_mbuf_chain:

		i = 0;
		n = m;

		while (n != NULL && i < DPAA_SGT_MAX_ENTRIES) {
			int len = n->m_len;

			if (len > 0) {
				qm_sg_entry_set_len(&sgt->sg[i], len);
				sgt->sg[i].bpid = FSL_DPAA_BPID_INV;
				sgt->sg[i].offset = 0;
				qm_sg_entry_set64(&sgt->sg[i],
				    mtod(n, uintptr_t));
				++i;
			}

			n = n->m_next;
		}

		if (n != NULL && i == DPAA_SGT_MAX_ENTRIES) {
			struct mbuf *c;

			c = m_collapse(m, M_NOWAIT, DPAA_SGT_MAX_ENTRIES);
			if (c == NULL) {
				if_inc_counter(ifp, IFCOUNTER_OQDROPS, 1);
				m_freem(m);
				uma_zfree(sc->sgt_zone, sgt);
				continue;
			}

			m = c;
			goto repeat_with_collapsed_mbuf_chain;
		}

		sgt->sg[i - 1].cfg |= cpu_to_be32(QM_SG_FIN);
		sgt->m = m;
		priv = netdev_priv(&sc->mac_dev.net_dev);
		egress_fq = priv->egress_fqs[queue];
		fd.cmd |= cpu_to_be32(qman_fq_fqid(priv->conf_fqs[queue]));

		for (i = 0; i < DPAA_ENQUEUE_RETRIES; ++i) {
			err = qman_enqueue(egress_fq, &fd);
			if (err != -EBUSY) {
				break;
			}
		}

		if (unlikely(err < 0)) {
			if_inc_counter(ifp, IFCOUNTER_OQDROPS, 1);
			m_freem(m);
			continue;
		}
	}
}

static void
fman_mac_txstart(struct ifnet *ifp)
{
	struct fman_mac_softc *sc;

	sc = ifp->if_softc;

	FMAN_MAC_LOCK(sc);
	fman_mac_txstart_locked(ifp, sc);
	FMAN_MAC_UNLOCK(sc);
}

static void
fman_mac_tick(void *arg)
{
	struct fman_mac_softc *sc;
	struct ifnet *ifp;

	sc = arg;
	ifp = sc->ifp;

	FMAN_MAC_ASSERT_LOCKED(sc);

	if ((ifp->if_drv_flags & IFF_DRV_RUNNING) == 0) {
		return;
	}

	mii_tick(sc->mii_softc);
	callout_reset(&sc->fman_mac_callout, hz, fman_mac_tick, sc);
}

static void
fman_mac_set_multi(struct fman_mac_softc *sc)
{
	struct mac_device *mac_dev;

	FMAN_MAC_ASSERT_LOCKED(sc);
	mac_dev = &sc->mac_dev;
	(*mac_dev->set_multi)(&mac_dev->net_dev, mac_dev);
}

static void
fman_mac_set_promisc(struct fman_mac_softc *sc, int if_flags)
{
	struct mac_device *mac_dev;

	FMAN_MAC_ASSERT_LOCKED(sc);
	mac_dev = &sc->mac_dev;
	(*mac_dev->set_promisc)(mac_dev->fman_mac,
	    (if_flags & IFF_PROMISC) != 0);
}

static int
fman_mac_set_mtu(struct fman_mac_softc *sc, int mtu)
{
	struct ifnet *ifp;
	int real_mtu;

	ifp = sc->ifp;
	real_mtu = mtu + ETHER_HDR_LEN + ETHER_VLAN_ENCAP_LEN;
	if (real_mtu > fman_get_max_frm() ||
	    real_mtu < ETHER_MIN_LEN) {
		return (EINVAL);
	}

	ifp->if_mtu = mtu;
	return (0);
}

static void
fman_mac_init_locked(struct fman_mac_softc *sc)
{
	struct ifnet *ifp;
	int error;

	FMAN_MAC_ASSERT_LOCKED(sc);

	ifp = sc->ifp;
	if ((ifp->if_drv_flags & IFF_DRV_RUNNING) != 0) {
		return;
	}

	ifp->if_drv_flags |= IFF_DRV_RUNNING;

	error = dpa_eth_priv_start(&sc->mac_dev.net_dev);
	BSD_ASSERT(error == 0);

	if (sc->mii_softc != NULL) {
		mii_mediachg(sc->mii_softc);
		callout_reset(&sc->fman_mac_callout, hz, fman_mac_tick, sc);
	}

	fman_mac_set_multi(sc);
}

static void
fman_mac_stop_locked(struct fman_mac_softc *sc)
{
	struct ifnet *ifp;
	int error;

	FMAN_MAC_ASSERT_LOCKED(sc);

	ifp = sc->ifp;
	ifp->if_drv_flags &= ~(IFF_DRV_RUNNING | IFF_DRV_OACTIVE);

	error = dpa_eth_priv_stop(&sc->mac_dev.net_dev);
	BSD_ASSERT(error == 0);
}

static void
fman_mac_init(void *if_softc)
{
	struct fman_mac_softc *sc;

	sc = if_softc;
	FMAN_MAC_LOCK(sc);
	fman_mac_init_locked(sc);
	FMAN_MAC_UNLOCK(sc);
}

static int
fman_mac_ioctl(struct ifnet *ifp, u_long cmd, caddr_t data)
{
	struct fman_mac_softc *sc;
	struct mii_data *mii;
	struct ifreq *ifr;
	int error;

	sc = ifp->if_softc;
	ifr = (struct ifreq *)data;

	error = 0;
	switch (cmd) {
	case SIOCSIFFLAGS:
		FMAN_MAC_LOCK(sc);
		if (ifp->if_flags & IFF_UP) {
			if (ifp->if_drv_flags & IFF_DRV_RUNNING) {
				if ((ifp->if_flags ^ sc->if_flags) &
				    IFF_PROMISC)
					fman_mac_set_promisc(sc,
					    ifp->if_flags);
			} else {
				fman_mac_init_locked(sc);
			}
		} else {
			if (ifp->if_drv_flags & IFF_DRV_RUNNING) {
				fman_mac_stop_locked(sc);
			}
		}
		sc->if_flags = ifp->if_flags;
		FMAN_MAC_UNLOCK(sc);
		break;
	case SIOCSIFMTU:
		error = fman_mac_set_mtu(sc, ifr->ifr_mtu);
		break;
	case SIOCADDMULTI:
	case SIOCDELMULTI:
		if (ifp->if_drv_flags & IFF_DRV_RUNNING) {
			FMAN_MAC_LOCK(sc);
			fman_mac_set_multi(sc);
			FMAN_MAC_UNLOCK(sc);
		}
		break;
	case SIOCSIFMEDIA:
	case SIOCGIFMEDIA:
		mii = sc->mii_softc;

		if (mii != NULL) {
			error = ifmedia_ioctl(ifp, ifr, &mii->mii_media, cmd);
		} else {
			error = EINVAL;
		}

		break;
	default:
		error = ether_ioctl(ifp, cmd, data);
		break;
	}

	return (error);
}

static int
fman_mac_media_change(struct ifnet *ifp)
{
	struct fman_mac_softc *sc;
	int error;

	sc = ifp->if_softc;
	FMAN_MAC_LOCK(sc);
	error = mii_mediachg(sc->mii_softc);
	FMAN_MAC_UNLOCK(sc);
	return (error);
}

static void
fman_mac_media_status(struct ifnet *ifp, struct ifmediareq *ifmr)
{
	struct fman_mac_softc *sc;
	struct mii_data *mii;

	sc = ifp->if_softc;
	mii = sc->mii_softc;
	FMAN_MAC_LOCK(sc);
	mii_pollstat(mii);
	ifmr->ifm_active = mii->mii_media_active;
	ifmr->ifm_status = mii->mii_media_status;
	FMAN_MAC_UNLOCK(sc);
}

int
fman_mac_dev_attach(device_t dev)
{
	struct fman_mac_softc *sc;
	struct ifnet *ifp;
	struct phy_device *phy_dev;
	int error;

	sc = device_get_softc(dev);

	mtx_init(&sc->mtx, device_get_nameunit(dev), MTX_NETWORK_LOCK,
	    MTX_DEF);

	callout_init_mtx(&sc->fman_mac_callout, &sc->mtx, 0);

	sc->sgt_zone = uma_zcreate("FMan MAC SGT", sizeof(struct fman_mac_sgt),
	    NULL, NULL, NULL, NULL, 16, 0);
	if (sc->sgt_zone == NULL) {
		goto error_0;
	}

	/* Set up the Ethernet interface */
	sc->ifp = ifp = if_alloc(IFT_ETHER);
	if (sc->ifp == NULL) {
		goto error_1;
	}

	snprintf(&sc->name[0], sizeof(sc->name), "fm%im",
	    device_get_unit(device_get_parent(dev)));

	ifp->if_softc = sc;
	if_initname(ifp, &sc->name[0], sc->mac_dev.data.mac_hw_id);
	ifp->if_flags = IFF_BROADCAST | IFF_SIMPLEX | IFF_MULTICAST;
	ifp->if_capabilities = IFCAP_TXCSUM | IFCAP_TXCSUM_IPV6 |
	    IFCAP_VLAN_MTU | IFCAP_JUMBO_MTU;
	ifp->if_capenable = ifp->if_capabilities;
	ifp->if_hwassist = FMAN_MAC_CSUM;
	ifp->if_start = fman_mac_txstart;
	ifp->if_ioctl = fman_mac_ioctl;
	ifp->if_init = fman_mac_init;
	IFQ_SET_MAXLEN(&ifp->if_snd, 128);
	ifp->if_snd.ifq_drv_maxlen = 128;
	IFQ_SET_READY(&ifp->if_snd);
	ifp->if_hdrlen = sizeof(struct ether_vlan_header);

	/* Attach the MII driver if necessary */
	phy_dev = sc->mac_dev.phy_dev;
	if (phy_dev != NULL) {
		error = mii_attach(dev, &sc->miibus, ifp,
		    fman_mac_media_change, fman_mac_media_status,
		    BMSR_DEFCAPMASK, phy_dev->mdio.addr, MII_OFFSET_ANY, 0);
		if (error != 0) {
			goto error_2;
		}
		sc->mii_softc = device_get_softc(sc->miibus);
	}

	sc->mac_dev.net_dev.ifp = ifp;

	ether_ifattach(ifp, &sc->mac_dev.addr[0]);
#if 0
	fman_mac_set_mtu(sc, ETHERMTU_JUMBO);
#endif

	return (0);

error_2:
	if_free(ifp);
error_1:
	uma_zdestroy(sc->sgt_zone);
error_0:
	mtx_destroy(&sc->mtx);
	return (ENXIO);
}

int
fman_mac_dev_detach(device_t _dev)
{
	struct fman_mac_softc *sc = device_get_softc(_dev);

	ether_ifdetach(sc->ifp);

	FMAN_MAC_LOCK(sc);
	fman_mac_stop_locked(sc);
	FMAN_MAC_UNLOCK(sc);

	if_free(sc->ifp);
	uma_zdestroy(sc->sgt_zone);
	mtx_destroy(&sc->mtx);

	return (bus_generic_detach(_dev));
}

int
fman_mac_miibus_read_reg(device_t dev, int phy, int reg)
{
	struct fman_mac_softc *sc;
	struct phy_device *phy_dev;

	sc = device_get_softc(dev);
	phy_dev = sc->mac_dev.phy_dev;
	BSD_ASSERT(phy == phy_dev->mdio.addr);
	return (phy_read(phy_dev, reg));
}

int
fman_mac_miibus_write_reg(device_t dev, int phy, int reg, int val)
{
	struct fman_mac_softc *sc;
	struct phy_device *phy_dev;

	sc = device_get_softc(dev);
	phy_dev = sc->mac_dev.phy_dev;
	BSD_ASSERT(phy == phy_dev->mdio.addr);
	return (phy_write(phy_dev, reg, val));
}

void
fman_mac_miibus_statchg(device_t dev)
{
	struct fman_mac_softc *sc;
	struct mac_device *mac_dev;
	struct mii_data *mii;
	u16 speed;

	sc = device_get_softc(dev);
	mac_dev = &sc->mac_dev;
	mii = sc->mii_softc;

	FMAN_MAC_ASSERT_LOCKED(sc);

	switch (IFM_SUBTYPE(mii->mii_media_active)) {
	case IFM_10_T:
	case IFM_10_2:
	case IFM_10_5:
	case IFM_10_STP:
	case IFM_10_FL:
		speed = SPEED_10;
		break;
	case IFM_100_TX:
	case IFM_100_FX:
	case IFM_100_T4:
	case IFM_100_VG:
	case IFM_100_T2:
		speed = SPEED_100;
		break;
	case IFM_1000_SX:
	case IFM_1000_LX:
	case IFM_1000_CX:
	case IFM_1000_T:
		speed = SPEED_1000;
		break;
	case IFM_10G_LR:
	case IFM_10G_SR:
	case IFM_10G_CX4:
	case IFM_10G_TWINAX:
	case IFM_10G_TWINAX_LONG:
	case IFM_10G_LRM:
		speed = SPEED_10000;
		break;
	default:
		speed = 0;
		break;
	}

	(*mac_dev->adjust_link)(mac_dev, speed);
}

void dpaa_cleanup_tx_fd(struct ifnet *ifp, const struct qm_fd *fd)
{
	struct fman_mac_softc *sc;
	struct fman_mac_sgt *sgt;

	BSD_ASSERT(qm_fd_get_format(fd) == qm_fd_sg);

	sc = ifp->if_softc;
	sgt = (struct fman_mac_sgt *)qm_fd_addr(fd);

	m_freem(sgt->m);
	uma_zfree(sc->sgt_zone, sgt);
}
