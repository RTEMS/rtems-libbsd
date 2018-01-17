#include <machine/rtems-bsd-kernel-space.h>

#include <rtems/bsd/local/opt_dpaa.h>

/*
 * Copyright (c) 2018 embedded brains GmbH
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <machine/rtems-bsd-kernel-space.h>

#include <linux/kernel.h>

#include "../../../../../../../../linux/drivers/net/ethernet/freescale/sdk_dpaa/dpaa_eth.h"

#include <soc/fsl/bman.h>

#include <sys/bus.h>
#include <sys/callout.h>
#include <sys/kernel.h>
#include <sys/malloc.h>
#include <sys/mbuf.h>
#include <sys/module.h>
#include <sys/queue.h>
#include <sys/socket.h>
#include <sys/sockio.h>

#include <net/if.h>
#include <net/ethernet.h>
#include <net/if_arp.h>
#include <net/if_dl.h>
#include <net/if_media.h>
#include <net/if_types.h>
#include <net/if_var.h>

#define	IF_ML_LOCK(sc) mtx_lock(&(sc)->mtx)
#define	IF_ML_UNLOCK(sc) mtx_unlock(&(sc)->mtx)

static void
if_ml_start(struct ifnet *ifp)
{
	struct if_ml_softc *sc;
	struct dpaa_priv *priv;
	int queue;
	struct dpaa_bp *bp;
	struct qman_fq *egress_fq;

	sc = ifp->if_softc;
	priv = netdev_priv(&sc->net_dev);
	queue = 0;
	bp = priv->dpaa_bps[queue];
	egress_fq = priv->egress_fqs[queue];

	for (;;) {
		struct mbuf *m;
		struct bm_buffer bmb;
		struct qm_fd fd;
		uintptr_t buf_addr;
		char *dst;
		int len;
		int err;
		int i;

		IF_DEQUEUE(&ifp->if_snd, m);
		if (m == NULL)
			break;

		err = bman_acquire(bp->pool, &bmb, 1);
		if (unlikely(err <= 0)) {
			if_inc_counter(ifp, IFCOUNTER_OQDROPS, 1);
			m_freem(m);
			continue;
		}

		qm_fd_clear_fd(&fd);
		len = m->m_pkthdr.len;
		qm_fd_set_contig(&fd, priv->tx_headroom, len);
		fd.bpid = bp->bpid;
		buf_addr = bm_buf_addr(&bmb);
		qm_fd_addr_set64(&fd, buf_addr);
		dst = (char *)(buf_addr + priv->tx_headroom);

		do {
			len = m->m_len;
			dst = memcpy(dst, mtod(m, const void *), len);
			dst += len;
			m = m_free(m);
		} while (m != NULL);

		for (i = 0; i < DPAA_ENQUEUE_RETRIES; ++i) {
			err = qman_enqueue(egress_fq, &fd);
			if (err != -EBUSY) {
				break;
			}
		}

		if (unlikely(err < 0)) {
			if_inc_counter(ifp, IFCOUNTER_OQDROPS, 1);
			continue;
		}
	}
}

static void
if_ml_init_locked(struct if_ml_softc* sc_p)
{
	struct ifnet *ifp;

	ifp = sc_p->ifp;
	if (ifp->if_drv_flags & IFF_DRV_RUNNING)
		return;

	ifp->if_drv_flags |= IFF_DRV_RUNNING;
}

static void
if_ml_stop_locked(struct if_ml_softc* sc_p)
{
}

static int
if_ml_ioctl(struct ifnet* ifp, ioctl_command_t cmd, caddr_t data)
{
	struct if_ml_softc *sc;
	int error;

	sc = ifp->if_softc;

	switch (cmd) {
	case SIOCSIFFLAGS:
		IF_ML_LOCK(sc);
		if (ifp->if_flags & IFF_UP) {
			if ((ifp->if_drv_flags & IFF_DRV_RUNNING) == 0)
				if_ml_init_locked(sc);
		} else {
			if ((ifp->if_drv_flags & IFF_DRV_RUNNING) != 0)
				if_ml_stop_locked(sc);
		}
		IF_ML_UNLOCK(sc);
		error = 0;
		break;

	default:
		error = ether_ioctl(ifp, cmd, data);
		break;
	}

	return error;
}

static void
if_ml_init(void* arg)
{
	struct if_ml_softc *sc;

	sc = arg;
	IF_ML_LOCK(sc);
	if_ml_init_locked(sc);
	IF_ML_UNLOCK(sc);
}

void
if_ml_attach(struct if_ml_softc *sc, int unit, const uint8_t *mac_address)
{
	struct ifnet *ifp;

	BSD_ASSERT(mac_address != NULL);

	sc->ifp = ifp = if_alloc(IFT_ETHER);
	BSD_ASSERT(ifp != NULL);

	sc->net_dev.ifp = ifp;

	mtx_init(&sc->mtx, "if_ml", MTX_NETWORK_LOCK, MTX_DEF);

	ifp->if_softc = sc;
	if_initname(ifp, "ml", unit);
	ifp->if_flags = IFF_SIMPLEX | IFF_MULTICAST | IFF_BROADCAST;
	ifp->if_start = if_ml_start;
	ifp->if_ioctl = if_ml_ioctl;
	ifp->if_init = if_ml_init;
	IFQ_SET_MAXLEN(&ifp->if_snd, ifqmaxlen);
	ifp->if_snd.ifq_drv_maxlen = ifqmaxlen;
	IFQ_SET_READY(&ifp->if_snd);

	ether_ifattach(ifp, mac_address);
}
