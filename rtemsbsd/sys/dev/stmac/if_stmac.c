/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2020 embedded brains Gmb_h (http://www.embedded-brains.de)
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <machine/rtems-bsd-kernel-space.h>

#include <bsp.h>

#ifdef LIBBSP_ARM_STM32H7_BSP_H

#include <sys/param.h>
#include <sys/types.h>
#include <sys/bus.h>
#include <sys/mbuf.h>
#include <sys/malloc.h>
#include <sys/kernel.h>
#include <sys/module.h>
#include <sys/socket.h>
#include <sys/sockio.h>

#include <net/if.h>
#include <net/ethernet.h>
#include <net/if_arp.h>
#include <net/if_dl.h>
#include <net/if_media.h>
#include <net/if_types.h>
#include <net/if_var.h>

#include <machine/bus.h>

#include <dev/mii/mii.h>
#include <dev/mii/miivar.h>

#include <rtems/bsd/local/miibus_if.h>

#include <stm32h7xx_hal.h>

#include <rtems/bsd/bsd.h>
#include <rtems/irq-extension.h>
#include <rtems/score/armv7m.h>

#define	RX_DESC_COUNT		64
#define	TX_DESC_COUNT		256

#define	STMAC_LOCK(sc) mtx_lock(&(sc)->mtx)
#define	STMAC_UNLOCK(sc) mtx_unlock(&(sc)->mtx)

#define	STMAC_TXLOCK(sc) mtx_lock(&(sc)->tx_mtx)
#define	STMAC_TXUNLOCK(sc) mtx_unlock(&(sc)->tx_mtx)

struct stmac_desc {
	__IO uint32_t	DESC0;
	__IO uint32_t	DESC1;
	__IO uint32_t	DESC2;
	__IO uint32_t	DESC3;
	struct mbuf	*m;
};

struct stmac_softc {
	ETH_HandleTypeDef	heth;
	uint8_t			mac_addr[6];
	struct ifnet		*ifp;
	struct mtx		mtx;
	device_t		miibus;
	struct mtx		mii_mtx;
	struct mii_data		*mii_softc;
	u_int			mii_media_active;
	u_int			mii_media_status;
	struct callout		tick_callout;
	int			if_flags;

	/* RX */
	struct stmac_desc	*rx_desc_ring;
	uint32_t		rx_idx;
	bool			rx_do_alloc;

	/* TX */
	struct mtx		tx_mtx;
	struct stmac_desc	*tx_desc_ring;
	uint32_t		tx_idx_head;
	uint32_t		tx_idx_tail;
};

static void stmac_init_locked(struct stmac_softc *);
static void stmac_stop_locked(struct stmac_softc *);

struct mbuf *
stmac_new_mbuf(struct ifnet *ifp)
{
	struct mbuf *m;

	m = m_getcl(M_NOWAIT, MT_DATA, M_PKTHDR);
	if (m != NULL) {
		m->m_data = mtod(m, char *) + ETHER_ALIGN;
		m->m_pkthdr.rcvif = ifp;
		rtems_cache_invalidate_multiple_data_lines(m->m_data, m->m_len);
	}

	return m;
}

static void
stmac_tick(void *arg)
{
	struct stmac_softc *sc;
	struct ifnet *ifp;

	sc = arg;
	ifp = sc->ifp;

	if ((ifp->if_drv_flags & IFF_DRV_RUNNING) == 0) {
		return;
	}

	mii_tick(sc->mii_softc);
	callout_reset(&sc->tick_callout, hz, stmac_tick, sc);
}

static uint32_t
stmac_rx_desc3(uint32_t idx)
{

	if (idx % 16 != 0) {
		return (ETH_DMARXNDESCRF_OWN | ETH_DMARXNDESCRF_BUF1V);
	}

	return (ETH_DMARXNDESCRF_OWN | ETH_DMARXNDESCRF_BUF1V |
	    ETH_DMARXNDESCRF_IOC);
}

static void
stmac_rx_setup_desc(struct stmac_softc *sc)
{
	uint32_t idx;
	bool do_alloc;
	ETH_TypeDef *regs;

	do_alloc = sc->rx_do_alloc;
	sc->rx_do_alloc = false;
	sc->rx_idx = 0;

	for (idx = 0; idx < RX_DESC_COUNT; ++idx) {
		struct stmac_desc *desc;
		struct mbuf *m;

		desc = &sc->rx_desc_ring[idx];

		if (do_alloc) {
			m = stmac_new_mbuf(sc->ifp);
			BSD_ASSERT(m != NULL);
			desc->m = m;
		} else {
			m = desc->m;
		}

		WRITE_REG(desc->DESC0, mtod(m, uint32_t));
		WRITE_REG(desc->DESC1, 0x0);
		WRITE_REG(desc->DESC2, 0x0);
		WRITE_REG(desc->DESC3, stmac_rx_desc3(idx));
	}

	regs = sc->heth.Instance;
	WRITE_REG(regs->DMACRDRLR, RX_DESC_COUNT -1);
	WRITE_REG(regs->DMACRDLAR, (uint32_t)&sc->rx_desc_ring[0]);
	WRITE_REG(regs->DMACRDTPR,
	    (uint32_t)&sc->rx_desc_ring[RX_DESC_COUNT - 1]);
}

static void
stmac_tx_setup_desc(struct stmac_softc *sc)
{
	uint32_t idx;
	ETH_TypeDef *regs;

	sc->tx_idx_head = 0;
	sc->tx_idx_tail = 0;

	for (idx = 0; idx < TX_DESC_COUNT; ++idx) {
		struct stmac_desc *desc;

		desc = &sc->tx_desc_ring[idx];
		WRITE_REG(desc->DESC0, 0x0);
		WRITE_REG(desc->DESC1, 0x0);
		WRITE_REG(desc->DESC2, 0x0);
		WRITE_REG(desc->DESC3, 0x0);
		desc->m = NULL;
	}

	regs = sc->heth.Instance;
	WRITE_REG(regs->DMACTDRLR, TX_DESC_COUNT -1);
	WRITE_REG(regs->DMACTDLAR, (uint32_t)&sc->tx_desc_ring[0]);
	WRITE_REG(regs->DMACTDTPR, (uint32_t)&sc->tx_desc_ring[0]);
}

static void
stmac_init_locked(struct stmac_softc *sc)
{
	struct ifnet *ifp;
	ETH_TypeDef *regs;

	ifp = sc->ifp;

	if ((ifp->if_drv_flags & IFF_DRV_RUNNING) != 0) {
		return;
	}

	ifp->if_drv_flags |= IFF_DRV_RUNNING;

	stmac_rx_setup_desc(sc);
	stmac_tx_setup_desc(sc);

	HAL_ETH_Start(&sc->heth);
	regs = sc->heth.Instance;

	/* Enable interrupts */
	SET_BIT(regs->DMACIER, ETH_DMACIER_NIE | ETH_DMACIER_RIE |
	    ETH_DMACIER_FBEE | ETH_DMACIER_AIE);

	mii_mediachg(sc->mii_softc);
	callout_reset(&sc->tick_callout, hz, stmac_tick, sc);
}

static void
stmac_tx_reclaim_all(struct stmac_softc *sc)
{
	uint32_t idx;

	for (idx = 0; idx < TX_DESC_COUNT; ++idx) {
		struct stmac_desc *desc;
		struct mbuf *m;

		desc = &sc->tx_desc_ring[idx];
		m = desc->m;
		desc->m = NULL;
		m_freem(m);
	}
}

static void
stmac_stop_locked(struct stmac_softc *sc)
{
	struct ifnet *ifp;
	ETH_TypeDef *regs;
	rtems_interrupt_server_entry server_entry;
	rtems_status_code status;
	rtems_name name;
	uint64_t t0;

	ifp = sc->ifp;

	if ((ifp->if_drv_flags & IFF_DRV_RUNNING) == 0) {
		return;
	}

	ifp->if_drv_flags &= ~IFF_DRV_RUNNING;
	regs = sc->heth.Instance;

	/* Disable interrupts */
	WRITE_REG(regs->DMACIER, 0);

	/* Disable the DMA transmission and reception */
	CLEAR_BIT(regs->DMACTCR, ETH_DMACTCR_ST);
	CLEAR_BIT(regs->DMACRCR, ETH_DMACRCR_SR);

	/* Wait for DMA done */
	t0 = rtems_clock_get_uptime_nanoseconds();
	while ((regs->MTLTQDR & (ETH_MTLTQDR_TXQSTS |
	    ETH_MTLTQDR_TRCSTS)) != 0 ||
	    (regs->MTLRQDR & (ETH_MTLRQDR_RXQSTS |
	    ETH_MTLRQDR_PRXQ)) != 0) {
		if (rtems_clock_get_uptime_nanoseconds() - t0 > 500000) {
			break;
		}
	}

	/* Disable the MAC reception and transmission */
	CLEAR_BIT(regs->MACCR, ETH_MACCR_RE | ETH_MACCR_TE);

	/* Make sure no receive interrupt is in progress */
	status = rtems_object_get_classic_name(rtems_task_self(), &name);
	BSD_ASSERT(status == RTEMS_SUCCESSFUL);
	if (name != rtems_build_name('I', 'R', 'Q', 'S')) {
		status = rtems_interrupt_server_entry_initialize(
		    RTEMS_INTERRUPT_SERVER_DEFAULT, &server_entry);
		BSD_ASSERT(status == RTEMS_SUCCESSFUL);
		rtems_interrupt_server_entry_destroy(&server_entry);
	}

	stmac_tx_reclaim_all(sc);
}

static uint8_t
stmac_bitreverse(uint8_t x)
{
	static const uint8_t nibbletab[] = {
	    0, 8, 4, 12, 2, 10, 6, 14, 1, 9, 5, 13, 3, 11, 7, 15
	};

	return ((nibbletab[x & 15] << 4) | nibbletab[x >> 4]);
}

static uint64_t
stmac_hash_bit(const uint8_t *eaddr)
{
	uint32_t crc;

	/*
	 * Thanks to the excellent documentation from STM it was easy to figure
	 * this out.
	 */
	crc = 0;
	crc = crc32_raw(eaddr, ETHER_ADDR_LEN, ~crc);
	crc = stmac_bitreverse((uint8_t)~crc);
	return ((uint64_t)1 << (crc >> 2));
}

static void
stmac_rx_setup_filter(struct stmac_softc *sc)
{
	struct ifmultiaddr *ifma;
	struct ifnet *ifp;
	ETH_TypeDef *regs;
	uint32_t macpfr;
	uint64_t machtr;
	const uint8_t *eaddr;
	struct epoch_tracker et;

	ifp = sc->ifp;

	macpfr = ETH_MACPFR_HMC;
	if ((ifp->if_flags & IFF_PROMISC) != 0) {
		macpfr |= ETH_MACPFR_PR;
	}

	machtr = 0;
	if ((ifp->if_flags & IFF_ALLMULTI) != 0) {
		machtr = ~machtr;
	} else {
		NET_EPOCH_ENTER(et);
		CK_STAILQ_FOREACH(ifma, &sc->ifp->if_multiaddrs, ifma_link) {
			if (ifma->ifma_addr->sa_family != AF_LINK) {
				continue;
			}

			eaddr = LLADDR((struct sockaddr_dl *)ifma->ifma_addr);
			machtr |= stmac_hash_bit(eaddr);
		}
		NET_EPOCH_EXIT(et);
	}

	regs = sc->heth.Instance;
	eaddr = IF_LLADDR(ifp);
	regs->MACA0LR = eaddr[0] | (eaddr[1] << 8) | (eaddr[2] << 16) |
	    (eaddr[3] << 24);
	regs->MACA0HR = eaddr[4] | (eaddr[5] << 8);
	regs->MACHT0R = (uint32_t)machtr;
	regs->MACHT1R = (uint32_t)(machtr >> 32);
	regs->MACPFR = macpfr;
}

static int
stmac_ioctl(struct ifnet *ifp, ioctl_command_t cmd, caddr_t data)
{
	struct stmac_softc *sc;
	int error;
	struct mii_data *mii;

	sc = ifp->if_softc;

	error = 0;
	switch (cmd) {
	case SIOCSIFFLAGS:
		STMAC_LOCK(sc);
		if ((ifp->if_flags & IFF_UP) != 0) {
			if ((ifp->if_drv_flags & IFF_DRV_RUNNING) != 0) {
				if ((ifp->if_flags ^ sc->if_flags) &
				    (IFF_PROMISC | IFF_ALLMULTI))
					stmac_rx_setup_filter(sc);
			} else {
				stmac_init_locked(sc);
			}
		} else {
			if ((ifp->if_drv_flags & IFF_DRV_RUNNING) != 0) {
				STMAC_TXLOCK(sc);
				stmac_stop_locked(sc);
				STMAC_TXUNLOCK(sc);
			}
		}
		sc->if_flags = ifp->if_flags;
		STMAC_UNLOCK(sc);
		break;
	case SIOCADDMULTI:
	case SIOCDELMULTI:
		STMAC_LOCK(sc);
		if ((ifp->if_drv_flags & IFF_DRV_RUNNING) != 0) {
			stmac_rx_setup_filter(sc);
		}
		STMAC_UNLOCK(sc);
		break;
	case SIOCSIFMEDIA:
	case SIOCGIFMEDIA:
		mii = sc->mii_softc;
		if (mii != NULL) {
			error = ifmedia_ioctl(ifp, (struct ifreq *)data, &mii->mii_media, cmd);
		} else {
			error = ether_ioctl(ifp, cmd, data);
		}
		break;
	default:
		error = ether_ioctl(ifp, cmd, data);
		break;
	}

	return (error);
}

static void
stmac_init(void *arg)
{
	struct stmac_softc *sc;

	sc = arg;
	STMAC_LOCK(sc);
	STMAC_TXLOCK(sc);
	stmac_init_locked(sc);
	STMAC_TXUNLOCK(sc);
	STMAC_UNLOCK(sc);
}

static void
stmac_media_status(struct ifnet *ifp, struct ifmediareq *ifmr_p)
{
	struct stmac_softc *sc;
	struct mii_data *mii;

	sc = ifp->if_softc;

	STMAC_LOCK(sc);
	mii = sc->mii_softc;
	if (mii != NULL) {
		mii_pollstat(mii);
		ifmr_p->ifm_active = mii->mii_media_active;
		ifmr_p->ifm_status = mii->mii_media_status;
	}
	STMAC_UNLOCK(sc);
}

static int
stmac_media_change(struct ifnet *ifp)
{
	struct stmac_softc *sc;
	int error;

	sc = ifp->if_softc;

	STMAC_LOCK(sc);
	if (sc->mii_softc != NULL) {
		error = mii_mediachg(sc->mii_softc);
	} else {
		error = ENXIO;
	}
	STMAC_UNLOCK(sc);
	return (error);
}

static int
stmac_probe(device_t dev)
{

	device_set_desc(dev, "STM MAC");
	return (BUS_PROBE_DEFAULT);
}

static void
stmac_rx_checksum(struct mbuf *m, uint32_t desc1)
{

	if ((desc1 & (ETH_DMARXNDESCWBF_IPCE | ETH_DMARXNDESCWBF_IPCB |
	    ETH_DMARXNDESCWBF_IPHE)) == 0) {
		uint32_t pt;

		pt = desc1 & ETH_DMARXNDESCWBF_PT;
		if (pt == ETH_DMARXNDESCWBF_PT_UDP ||
		    pt == ETH_DMARXNDESCWBF_PT_TCP) {
			m->m_pkthdr.csum_flags |=
			    CSUM_IP_CHECKED |
			    CSUM_IP_VALID |
			    CSUM_DATA_VALID |
			    CSUM_PSEUDO_HDR;
			m->m_pkthdr.csum_data = 0xffff;
		} else if ((desc1 & ETH_DMARXNDESCWBF_IPV4) != 0) {
			m->m_pkthdr.csum_flags |=
			    CSUM_IP_CHECKED |
			    CSUM_IP_VALID;
			m->m_pkthdr.csum_data = 0xffff;
		}
	}
}

static void
stmac_rx_interrupt(struct stmac_softc *sc, ETH_TypeDef *regs)
{
	struct ifnet *ifp;
	uint32_t idx;
	struct stmac_desc *desc_ring;
	struct stmac_desc *desc;

	ifp = sc->ifp;
	desc_ring = sc->rx_desc_ring;
	idx = sc->rx_idx;
	desc = &desc_ring[idx];

	while (true) {
		uint32_t desc3;
		struct mbuf *m;

		/*
		 * Make sure the interrupt is cleared and no longer pending
		 * before we read the descriptor status.
		 */
		regs->DMACSR = ETH_DMACSR_RI | ETH_DMACSR_NIS;
		regs->DMACSR;
		_ARMV7M_NVIC_Clear_pending(ETH_IRQn);

		desc3 = desc->DESC3;
		if ((desc3 & ETH_DMARXNDESCRF_OWN) != 0) {
			break;
		}

		m = stmac_new_mbuf(ifp);
		if (m != NULL) {
			uint32_t mask;
			uint32_t set;
			uint32_t len;

			mask = ETH_DMARXNDESCWBF_CTXT | ETH_DMARXNDESCWBF_FD |
			    ETH_DMARXNDESCWBF_LD | ETH_DMARXNDESCWBF_ES;
			set = ETH_DMARXNDESCWBF_FD | ETH_DMARXNDESCWBF_LD;
			len = desc3 & ETH_DMARXNDESCWBF_PL;
			if ((desc3 & mask) == set && len > ETHER_CRC_LEN) {
				struct mbuf *rx;

				rx = desc->m;
				stmac_rx_checksum(rx, desc->DESC1);
				rx->m_len = len;
				rx->m_pkthdr.len = len;
				(*ifp->if_input)(ifp, rx);
			} else {
				if_inc_counter(ifp, IFCOUNTER_IERRORS, 1);
				m_freem(m);
				m = desc->m;
			}
		} else {
			if_inc_counter(ifp, IFCOUNTER_IQDROPS, 1);
			m = desc->m;
		}

		WRITE_REG(desc->DESC0, mtod(m, uint32_t));
		WRITE_REG(desc->DESC1, 0x0);
		WRITE_REG(desc->DESC2, 0x0);
		WRITE_REG(desc->DESC3, stmac_rx_desc3(idx));
		desc->m = m;

		_ARM_Data_synchronization_barrier();
		WRITE_REG(regs->DMACRDTPR, (uint32_t)desc);

		idx = (idx + 1) % RX_DESC_COUNT;
		desc = &desc_ring[idx];
	}

	sc->rx_idx = idx;
}

static void
stmac_reset(struct stmac_softc *sc)
{
	HAL_StatusTypeDef hal_status;
	ETH_TypeDef *regs;

	hal_status = HAL_ETH_Init(&sc->heth);
	BSD_ASSERT(hal_status == HAL_OK);

	regs = sc->heth.Instance;

	/* Set descriptor skip length according to struct stmac_desc */
	MODIFY_REG(regs->DMACCR, ETH_DMACCR_DSL, ETH_DMACCR_DSL_32BIT);

	/*
	 * FIXME: Just use a random value.  It is not clear which clock is used
	 * here.
	 */
	regs->DMACRIWTR = 0x80;
}

static void
stmac_interrupt(void *arg)
{
	struct stmac_softc *sc;
	ETH_TypeDef *regs;
	uint32_t dmacsr;

	sc = arg;
	regs = sc->heth.Instance;
	dmacsr = regs->DMACSR;

	/* This is almost always a receive interrupt */
	stmac_rx_interrupt(sc, regs);

	if (__predict_false((dmacsr & ETH_DMACSR_FBE) != 0)) {
		regs->DMACSR = ETH_DMACSR_FBE | ETH_DMACSR_AIS;

		STMAC_LOCK(sc);
		STMAC_TXLOCK(sc);
		stmac_stop_locked(sc);
		stmac_reset(sc);
		stmac_init_locked(sc);
		STMAC_TXUNLOCK(sc);
		STMAC_UNLOCK(sc);
	}
}

static void
stmac_tx_reclaim(struct stmac_softc *sc, struct ifnet *ifp)
{
	uint32_t head_idx;
	uint32_t tail_idx;
	struct stmac_desc *desc_ring;

	head_idx = sc->tx_idx_head;
	tail_idx = sc->tx_idx_tail;
	desc_ring = sc->tx_desc_ring;

	while (head_idx != tail_idx) {
		struct stmac_desc *tail_desc;
		uint32_t desc3;

		tail_desc = &desc_ring[tail_idx];

		desc3 = tail_desc->DESC3;
		if ((desc3 & ETH_DMATXNDESCWBF_OWN) != 0) {
			break;
		}

		if ((desc3 & ETH_DMATXNDESCWBF_LD) != 0) {
			struct mbuf *m;
			ift_counter cnt;

			if ((desc3 & ETH_DMATXNDESCWBF_ES) == 0) {
				cnt = IFCOUNTER_OPACKETS;
			} else {
				cnt = IFCOUNTER_OERRORS;
			}

			if_inc_counter(ifp, cnt, 1);

			m = tail_desc->m;
			tail_desc->m = NULL;
			m_freem(m);
		}

		tail_idx = (tail_idx + 1) % TX_DESC_COUNT;
	}

	sc->tx_idx_tail = tail_idx;
}

static void
stmac_cache_flush(uintptr_t begin, uintptr_t size)
{
	uintptr_t end;
	uintptr_t mask;

	/* Align begin and end of the data to a cache line */
	end = begin + size;
	mask = CPU_CACHE_LINE_BYTES - 1;
	begin &= ~mask;
	end = (end + mask) & ~mask;
	rtems_cache_flush_multiple_data_lines((void *)begin, end - begin);
}

static int
stmac_tx_enqueue(struct stmac_softc *sc, struct ifnet *ifp, struct mbuf *m)
{
	uint32_t head_idx;
	uint32_t tail_idx;
	uint32_t capacity;
	uint32_t new_head_idx;
	uint32_t idx;
	struct stmac_desc *desc_ring;
	struct stmac_desc *desc;
	ETH_TypeDef *regs;
	uint32_t bufs;
	uint32_t desc2;
	uint32_t desc3;
	struct mbuf *n;
	int csum_flags;

	head_idx = sc->tx_idx_head;
	tail_idx = sc->tx_idx_tail;
	capacity = 2 * ((tail_idx - head_idx - 1) % TX_DESC_COUNT);

	idx = head_idx;
	desc_ring = sc->tx_desc_ring;
	desc2 = 0;
	bufs = 0;
	n = m;

	do {
		uintptr_t size;

		desc = &desc_ring[idx];

		size = (uintptr_t)n->m_len;
		if (__predict_true(size > 0)) {
			uintptr_t begin;

			++bufs;
			if (__predict_false(bufs > capacity)) {
				return (ENOBUFS);
			}

			begin = mtod(n, uintptr_t);

			if (bufs % 2 == 1) {
				desc->DESC0 = begin;
				desc2 = size;
			} else {
				desc->DESC1 = begin;
				desc->DESC2 = (size << 16) | desc2;
				idx = (idx + 1) % TX_DESC_COUNT;
			}

			stmac_cache_flush(begin, size);
		}

		n = n->m_next;
	} while (n != NULL);

	if (bufs % 2 == 1) {
		desc->DESC1 = 0;
		desc->DESC2 = desc2;
		idx = (idx + 1) % TX_DESC_COUNT;
	}

	new_head_idx = idx;
	sc->tx_idx_head = new_head_idx;

	idx = (idx - 1) % TX_DESC_COUNT;
	desc = &desc_ring[idx];
	desc->m = m;

	desc3 = ETH_DMATXNDESCRF_OWN | ETH_DMATXNDESCRF_LD | m->m_pkthdr.len;
	csum_flags = m->m_pkthdr.csum_flags;
	if ((csum_flags & (CSUM_TCP | CSUM_UDP | CSUM_TCP_IPV6 |
	    CSUM_UDP_IPV6)) != 0) {
		desc3 |= ETH_DMATXNDESCRF_CIC_IPHDR_PAYLOAD_INSERT_PHDR_CALC;
	} else if ((csum_flags & CSUM_IP) != 0) {
		desc3 |= ETH_DMATXNDESCRF_CIC_IPHDR_INSERT;
	}

	while (idx != head_idx) {
		desc->DESC3 = desc3;
		desc3 &= ~ETH_DMATXNDESCRF_LD;

		idx = (idx - 1) % TX_DESC_COUNT;
		desc = &desc_ring[idx];
	}

	desc->DESC3 = ETH_DMATXNDESCRF_FD | desc3;
	_ARM_Data_synchronization_barrier();
	regs = sc->heth.Instance;
	WRITE_REG(regs->DMACTDTPR, (uint32_t)&desc_ring[new_head_idx]);
	return (0);
}

static int
stmac_transmit(struct ifnet *ifp, struct mbuf *m)
{
	struct stmac_softc *sc;
	int error;

	sc = ifp->if_softc;
	STMAC_TXLOCK(sc);

	error = stmac_tx_enqueue(sc, ifp, m);
	stmac_tx_reclaim(sc, ifp);

	if (__predict_false(error != 0)) {
		error = stmac_tx_enqueue(sc, ifp, m);
		if (error != 0) {
			m_freem(m);
			if_inc_counter(ifp, IFCOUNTER_OQDROPS, 1);
		}
	}

	STMAC_TXUNLOCK(sc);
	return (error);
}

static int
stmac_transmit_no_link(struct ifnet *ifp, struct mbuf *m)
{

	(void)ifp;
	(void)m;
	return (ENETDOWN);
}

static void
stmac_qflush(struct ifnet *ifp)
{

	(void)ifp;
}

static int
stmac_attach(device_t dev)
{
	struct stmac_softc *sc;
	struct ifnet *ifp;
	int error;
	struct stmac_desc *descs;
	ETH_TypeDef *regs;
	rtems_status_code status;

	sc = device_get_softc(dev);

	sc->rx_do_alloc = true;
	mtx_init(&sc->mtx, device_get_nameunit(dev), MTX_NETWORK_LOCK, MTX_DEF);
	mtx_init(&sc->tx_mtx, "stmac tx", MTX_NETWORK_LOCK, MTX_DEF);
	callout_init_mtx(&sc->tick_callout, &sc->mtx, 0);

	sc->ifp = ifp = if_alloc(IFT_ETHER);
	ifp->if_softc = sc;

	regs = ETH;
	sc->heth.Instance = regs;

	descs = rtems_cache_coherent_allocate(sizeof(*descs) *
	    (RX_DESC_COUNT + TX_DESC_COUNT), 4, 0);
	BSD_ASSERT(descs != NULL);

	sc->tx_desc_ring = &descs[0];
	sc->rx_desc_ring = &descs[TX_DESC_COUNT];

	rtems_bsd_get_mac_address(device_get_name(dev), device_get_unit(dev),
	    &sc->mac_addr[0]);

	sc->heth.Init.MACAddr = &sc->mac_addr[0];
	sc->heth.Init.MediaInterface = HAL_ETH_RMII_MODE;
	sc->heth.Init.RxBuffLen = 1524;

	stmac_reset(sc);

	if_initname(ifp, "stm", device_get_unit(dev));
	ifp->if_flags = IFF_SIMPLEX | IFF_MULTICAST | IFF_BROADCAST;
	ifp->if_capabilities |= IFCAP_HWCSUM | IFCAP_HWCSUM_IPV6;
	ifp->if_capenable = ifp->if_capabilities;
	ifp->if_hwassist = CSUM_IP | CSUM_TCP | CSUM_UDP | CSUM_TCP_IPV6 | CSUM_UDP_IPV6;
	ifp->if_transmit = stmac_transmit_no_link;
	ifp->if_qflush = stmac_qflush;
	ifp->if_ioctl = stmac_ioctl;
	ifp->if_init = stmac_init;
	IFQ_SET_MAXLEN(&ifp->if_snd, TX_DESC_COUNT - 1);
	ifp->if_snd.ifq_drv_maxlen = TX_DESC_COUNT - 1;
	IFQ_SET_READY(&ifp->if_snd);

	error = mii_attach(dev, &sc->miibus, ifp, stmac_media_change,
	    stmac_media_status, BMSR_DEFCAPMASK, MII_PHY_ANY,
	    MII_OFFSET_ANY, 0);
	if (error == 0) {
		sc->mii_softc = device_get_softc(sc->miibus);
	}

	ether_ifattach(ifp, &sc->heth.Init.MACAddr[0]);

	status = rtems_interrupt_server_handler_install(
	    RTEMS_INTERRUPT_SERVER_DEFAULT, ETH_IRQn, "stmac",
	    RTEMS_INTERRUPT_SHARED, stmac_interrupt, sc);
	BSD_ASSERT(status == RTEMS_SUCCESSFUL);
	return (0);
}

static int
stmac_miibus_read(device_t dev, int phy, int reg)
{
	struct stmac_softc *sc;
	uint32_t val;
	HAL_StatusTypeDef hal_status;

	sc = device_get_softc(dev);
	hal_status = HAL_ETH_ReadPHYRegister(&sc->heth, (uint32_t)phy,
	    (uint32_t)reg,  &val);
	if (hal_status != HAL_OK) {
		return (-1);
	}

	return ((int)val);
}

static int
stmac_miibus_write(device_t dev, int phy, int reg, int val)
{
	struct stmac_softc *sc;
	HAL_StatusTypeDef hal_status;

	sc = device_get_softc(dev);
	hal_status = HAL_ETH_WritePHYRegister(&sc->heth, (uint32_t)phy,
	    (uint32_t)reg, (uint32_t)val);
	if (hal_status != HAL_OK) {
		return (-1);
	}

	return (0);
}

static void
stmac_miibus_statchg(device_t dev)
{
	struct stmac_softc *sc;
	struct mii_data *mii;
	u_int active;
	u_int status;
	ETH_TypeDef *regs;
	uint32_t maccr;

	sc = device_get_softc(dev);
	mii = device_get_softc(sc->miibus);
	active = mii->mii_media_active;
	status = mii->mii_media_status;

	if (sc->mii_media_active == active && sc->mii_media_status == status) {
		return;
	}

	sc->mii_media_active = active;
	sc->mii_media_status = status;

	regs = sc->heth.Instance;
	maccr = regs->MACCR;

	if ((status & IFM_ACTIVE) != 0) {
		if_settransmitfn(sc->ifp, stmac_transmit);

		if ((IFM_OPTIONS(active) & IFM_FDX) != 0) {
			maccr |= ETH_MACCR_DM;
		} else {
			maccr &= ~ETH_MACCR_DM;
		}

		if (IFM_SUBTYPE(active) != IFM_10_T) {
			maccr |= ETH_MACCR_FES;
		} else {
			maccr &= ~ETH_MACCR_FES;
		}
	} else {
		maccr |= ETH_MACCR_DM | ETH_MACCR_FES;
	}

	regs->MACCR = maccr;
}

static device_method_t stmac_methods[] = {
	DEVMETHOD(device_probe, stmac_probe),
	DEVMETHOD(device_attach, stmac_attach),
	DEVMETHOD(miibus_readreg, stmac_miibus_read),
	DEVMETHOD(miibus_writereg, stmac_miibus_write),
	DEVMETHOD(miibus_statchg, stmac_miibus_statchg),
	DEVMETHOD_END
};

driver_t stmac_driver = {
	"stmac",
	stmac_methods,
	sizeof(struct stmac_softc)
};

DRIVER_MODULE(stmac, nexus, stmac_driver, 0, 0);
DRIVER_MODULE(miibus, stmac, miibus_driver, 0, 0);

MODULE_DEPEND(stmac, ether, 1, 1, 1);
MODULE_DEPEND(stmac, miibus, 1, 1, 1);

#endif /* LIBBSP_ARM_STM32H7_BSP_H */
