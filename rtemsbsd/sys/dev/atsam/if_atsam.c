/*
 * Copyright (c) 2016 - 2017 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <info@embedded-brains.de>
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

#include <bsp.h>

#ifdef LIBBSP_ARM_ATSAM_BSP_H

#include <bsp/irq.h>

#include <stdio.h>

#include <sys/types.h>
#include <sys/param.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <sys/sockio.h>
#include <sys/kernel.h>
#include <sys/module.h>
#include <sys/bus.h>
#include <sys/sysctl.h>

#include <net/bpf.h>
#include <net/if.h>
#include <net/if_dl.h>
#include <net/if_var.h>
#include <net/if_types.h>
#include <net/if_media.h>

#include <netinet/in.h>
#include <netinet/if_ether.h>

#include <dev/mii/mii.h>
#include <dev/mii/miivar.h>

#include <libchip/chip.h>
#include <libchip/include/gmacd.h>
#include <libchip/include/pio.h>

#include <rtems/bsd/local/miibus_if.h>
#include <rtems/bsd/if_atsam.h>
#include <rtems/bsd/bsd.h>

/*
 * Number of interfaces supported by the driver
 */
#define NIFACES			1

/** Enable/Disable CopyAllFrame */
#define GMAC_CAF_DISABLE	0
#define GMAC_CAF_ENABLE		1

/** Enable/Disable NoBroadCast */
#define GMAC_NBC_DISABLE	0
#define GMAC_NBC_ENABLE		1

/** The PIN list of PIO for GMAC */
#define BOARD_GMAC_PINS							   \
	{ (PIO_PD0A_GTXCK | PIO_PD1A_GTXEN | PIO_PD2A_GTX0 | PIO_PD3A_GTX1 \
	  | PIO_PD4A_GRXDV | PIO_PD5A_GRX0 | PIO_PD6A_GRX1		   \
	  | PIO_PD7A_GRXER						   \
	  | PIO_PD8A_GMDC | PIO_PD9A_GMDIO), PIOD, ID_PIOD, PIO_PERIPH_A,  \
	  PIO_DEFAULT }
/** The runtime pin configure list for GMAC */
#define BOARD_GMAC_RUN_PINS			BOARD_GMAC_PINS

/** RX Defines */
#define GMAC_RX_BUFFER_SIZE			1536
#define GMAC_RX_BUF_DESC_ADDR_MASK		0xFFFFFFFC

#define GMAC_DESCRIPTOR_ALIGNMENT		8

/** Events */
#define ATSAMV7_ETH_RX_EVENT_INTERRUPT		RTEMS_EVENT_1

/* FIXME: Make these configurable */
#define MDIO_RETRIES 10
#define MDIO_PHY MII_PHY_ANY
#define IGNORE_RX_ERR false

#define RX_DESC_LOG2 3
#define RX_DESC_COUNT (1U << RX_DESC_LOG2)
#define RX_DESC_WRAP(idx) \
  ((((idx) + 1) & RX_DESC_COUNT) >> (RX_DESC_LOG2 - 1))
RTEMS_STATIC_ASSERT(RX_DESC_WRAP(RX_DESC_COUNT - 1) ==
    GMAC_RX_WRAP_BIT, rx_desc_wrap);
RTEMS_STATIC_ASSERT(RX_DESC_WRAP(RX_DESC_COUNT - 2) ==
    0, rx_desc_no_wrap);

#define TX_DESC_LOG2 6
#define TX_DESC_COUNT (1U << TX_DESC_LOG2)
#define TX_DESC_WRAP(idx) \
  ((((idx) + 1) & TX_DESC_COUNT) << (30 - TX_DESC_LOG2))
RTEMS_STATIC_ASSERT(TX_DESC_WRAP(TX_DESC_COUNT - 1) ==
    GMAC_TX_WRAP_BIT, tx_desc_wrap);
RTEMS_STATIC_ASSERT(TX_DESC_WRAP(TX_DESC_COUNT - 2) ==
    0, tx_desc_no_wrap);

/** The PINs for GMAC */
static const Pin gmacPins[] = { BOARD_GMAC_RUN_PINS };

typedef struct if_atsam_gmac {
	/** The GMAC driver instance */
	sGmacd gGmacd;
	uint32_t retries;
} if_atsam_gmac;

struct if_atsam_tx_bds {
	volatile sGmacTxDescriptor bds[TX_DESC_COUNT];
};

struct if_atsam_rx_bds {
	volatile sGmacRxDescriptor bds[RX_DESC_COUNT];
};

/*
 * Per-device data
 */
typedef struct if_atsam_softc {
	/*
	 * Data
	 */
	device_t dev;
	struct ifnet *ifp;
	struct mtx mtx;
	if_atsam_gmac Gmac_inst;
	size_t tx_idx_head;
	size_t tx_idx_tail;
	struct if_atsam_tx_bds *tx;
	struct mbuf *tx_mbufs[TX_DESC_COUNT];
	struct if_atsam_rx_bds *rx;
	struct mbuf *rx_mbufs[RX_DESC_COUNT];
	uint8_t GMacAddress[6];
	rtems_id rx_daemon_tid;
	rtems_vector_number interrupt_number;
	struct callout tick_ch;

	/*
	 * Settings for a fixed speed.
	 */
	bool fixed_speed;
	uint32_t media;
	uint32_t duplex;
	struct ifmedia ifmedia;

	/*
	 * MII bus (only used if no fixed speed)
	 */
	device_t miibus;
	uint8_t link_speed;
	uint8_t link_duplex;

	/*
	 * Statistics
	 */
	struct if_atsam_stats {
		/* Software */
		uint32_t rx_overrun_errors;
		uint32_t rx_interrupts;
		uint32_t tx_tur_errors;
		uint32_t tx_rlex_errors;
		uint32_t tx_tfc_errors;
		uint32_t tx_hresp_errors;

		/* Hardware */
		uint64_t octets_transm;
		uint32_t frames_transm;
		uint32_t broadcast_frames_transm;
		uint32_t multicast_frames_transm;
		uint32_t pause_frames_transm;
		uint32_t frames_64_byte_transm;
		uint32_t frames_65_to_127_byte_transm;
		uint32_t frames_128_to_255_byte_transm;
		uint32_t frames_256_to_511_byte_transm;
		uint32_t frames_512_to_1023_byte_transm;
		uint32_t frames_1024_to_1518_byte_transm;
		uint32_t frames_greater_1518_byte_transm;
		uint32_t transmit_underruns;
		uint32_t single_collision_frames;
		uint32_t multiple_collision_frames;
		uint32_t excessive_collisions;
		uint32_t late_collisions;
		uint32_t deferred_transmission_frames;
		uint32_t carrier_sense_errors;
		uint64_t octets_rec;
		uint32_t frames_rec;
		uint32_t broadcast_frames_rec;
		uint32_t multicast_frames_rec;
		uint32_t pause_frames_rec;
		uint32_t frames_64_byte_rec;
		uint32_t frames_65_to_127_byte_rec;
		uint32_t frames_128_to_255_byte_rec;
		uint32_t frames_256_to_511_byte_rec;
		uint32_t frames_512_to_1023_byte_rec;
		uint32_t frames_1024_to_1518_byte_rec;
		uint32_t frames_1519_to_maximum_byte_rec;
		uint32_t undersize_frames_rec;
		uint32_t oversize_frames_rec;
		uint32_t jabbers_rec;
		uint32_t frame_check_sequence_errors;
		uint32_t length_field_frame_errors;
		uint32_t receive_symbol_errors;
		uint32_t alignment_errors;
		uint32_t receive_resource_errors;
		uint32_t receive_overrun;
		uint32_t ip_header_checksum_errors;
		uint32_t tcp_checksum_errors;
		uint32_t udp_checksum_errors;
	} stats;

	int if_flags;
} if_atsam_softc;

static void if_atsam_poll_hw_stats(struct if_atsam_softc *sc);

#define IF_ATSAM_LOCK(sc) mtx_lock(&(sc)->mtx)

#define IF_ATSAM_UNLOCK(sc) mtx_unlock(&(sc)->mtx)

static struct mbuf *if_atsam_new_mbuf(struct ifnet *ifp)
{
	struct mbuf *m;

	MGETHDR(m, M_NOWAIT, MT_DATA);
	if (m != NULL) {
		MCLGET(m, M_NOWAIT);
		if ((m->m_flags & M_EXT) != 0) {
			m->m_pkthdr.rcvif = ifp;
			m->m_data = mtod(m, char *);
			rtems_cache_invalidate_multiple_data_lines(mtod(m, void *),
			    GMAC_RX_BUFFER_SIZE);
		} else {
			m_free(m);
			m = NULL;
		}
	}
	return (m);
}

static uint8_t if_atsam_wait_phy(Gmac *pHw, uint32_t retry)
{
	volatile uint32_t retry_count = 0;

	while (!GMAC_IsIdle(pHw)) {
		if (retry == 0) {
			continue;
		}
		retry_count++;

		if (retry_count >= retry) {
			return (1);
		}
		rtems_task_wake_after(1);
	}

	return (0);
}


static uint8_t
if_atsam_write_phy(Gmac *pHw, uint8_t PhyAddress, uint8_t Address,
    uint32_t Value, uint32_t retry)
{
	GMAC_PHYMaintain(pHw, PhyAddress, Address, 0, (uint16_t)Value);
	if (if_atsam_wait_phy(pHw, retry) == 1) {
		return (1);
	}
	return (0);
}


static uint8_t
if_atsam_read_phy(Gmac *pHw,
    uint8_t PhyAddress, uint8_t Address, uint32_t *pvalue, uint32_t retry)
{
	GMAC_PHYMaintain(pHw, PhyAddress, Address, 1, 0);
	if (if_atsam_wait_phy(pHw, retry) == 1) {
		return (1);
	}
	*pvalue = GMAC_PHYData(pHw);
	return (0);
}


static int
if_atsam_miibus_readreg(device_t dev, int phy, int reg)
{
	uint32_t val;
	uint8_t err;
	if_atsam_softc *sc = device_get_softc(dev);

	IF_ATSAM_LOCK(sc);
	err = if_atsam_read_phy(sc->Gmac_inst.gGmacd.pHw,
	    (uint8_t)phy, (uint8_t)reg, &val, sc->Gmac_inst.retries);
	IF_ATSAM_UNLOCK(sc);

	return (err == 0 ? val : 0);
}


static int
if_atsam_miibus_writereg(device_t dev, int phy, int reg, int data)
{
	if_atsam_softc *sc = device_get_softc(dev);

	IF_ATSAM_LOCK(sc);
	(void)if_atsam_write_phy(sc->Gmac_inst.gGmacd.pHw,
	    (uint8_t)phy, (uint8_t)reg, data, sc->Gmac_inst.retries);
	IF_ATSAM_UNLOCK(sc);

	return 0;
}


static uint8_t
if_atsam_init_phy(if_atsam_gmac *gmac_inst, uint32_t mck,
    const Pin *pResetPins, uint32_t nbResetPins, const Pin *pGmacPins,
    uint32_t nbGmacPins)
{
	uint8_t rc = 1;
	Gmac *pHw = gmac_inst->gGmacd.pHw;

	/* Perform RESET */
	if (pResetPins) {
		/* Configure PINS */
		PIO_Configure(pResetPins, nbResetPins);
		PIO_Clear(pResetPins);
		rtems_task_wake_after(1);
		PIO_Set(pResetPins);
	}
	/* Configure GMAC runtime pins */
	if (rc) {
		PIO_Configure(pGmacPins, nbGmacPins);
		rc = GMAC_SetMdcClock(pHw, mck);

		if (!rc) {
			return (0);
		}
	}
	return (rc);
}


/*
 * Interrupt Handler for the network driver
 */
static void if_atsam_interrupt_handler(void *arg)
{
	if_atsam_softc *sc = (if_atsam_softc *)arg;
	Gmac *pHw = sc->Gmac_inst.gGmacd.pHw;
	uint32_t is;

	/* Get interrupt status */
	is = pHw->GMAC_ISR;

	if (__predict_false((is & GMAC_TX_ERR_BIT) != 0)) {
		if ((is & GMAC_IER_TUR) != 0) {
			++sc->stats.tx_tur_errors;
		}
		if ((is & GMAC_IER_RLEX) != 0) {
			++sc->stats.tx_rlex_errors;
		}
		if ((is & GMAC_IER_TFC) != 0) {
			++sc->stats.tx_tfc_errors;
		}
		if ((is & GMAC_IER_HRESP) != 0) {
			++sc->stats.tx_hresp_errors;
		}
	}

	/* Check receive interrupts */
	if (__predict_true((is & (GMAC_IER_ROVR | GMAC_IER_RCOMP)) != 0)) {
		if (__predict_false((is & GMAC_IER_ROVR) != 0)) {
			++sc->stats.rx_overrun_errors;
		}

		++sc->stats.rx_interrupts;

		/* Erase the interrupts for RX completion and errors */
		pHw->GMAC_IDR = GMAC_IER_RCOMP | GMAC_IER_ROVR;

		(void)rtems_event_send(sc->rx_daemon_tid,
		    ATSAMV7_ETH_RX_EVENT_INTERRUPT);
	}
}

static void
if_atsam_rx_update_mbuf(struct mbuf *m, uint32_t status)
{
	int frame_len;

	frame_len = (int)(status & GMAC_LENGTH_FRAME);

	m->m_data = mtod(m, char*)+ETHER_ALIGN;
	m->m_len = frame_len;
	m->m_pkthdr.len = frame_len;

	/* check checksum offload result */
	m->m_pkthdr.csum_flags = 0;
	switch ((status >> 22) & 0x3) {
	case GMAC_RXDESC_ST_CKSUM_RESULT_IP_CHECKED:
		m->m_pkthdr.csum_flags = CSUM_IP_CHECKED | CSUM_IP_VALID;
		m->m_pkthdr.csum_data = 0xffff;
		break;
	case GMAC_RXDESC_ST_CKSUM_RESULT_IP_AND_TCP_CHECKED:
	case GMAC_RXDESC_ST_CKSUM_RESULT_IP_AND_UDP_CHECKED:
		m->m_pkthdr.csum_flags = CSUM_IP_CHECKED | CSUM_IP_VALID |
		    CSUM_L4_VALID | CSUM_L4_CALC;
		m->m_pkthdr.csum_data = 0xffff;
		break;
	}
}

static void
if_atsam_rx_daemon(rtems_task_argument arg)
{
	if_atsam_softc *sc = (if_atsam_softc *)arg;
	struct ifnet *ifp = sc->ifp;
	volatile sGmacRxDescriptor *base;
	struct if_atsam_rx_bds *rx;
	Gmac *pHw = sc->Gmac_inst.gGmacd.pHw;
	size_t idx;
	struct mbuf **mbufs;
	struct mbuf *m;

	IF_ATSAM_LOCK(sc);

	if (IGNORE_RX_ERR) {
		pHw->GMAC_NCFGR |= GMAC_NCFGR_IRXER;
	} else {
		pHw->GMAC_NCFGR &= ~GMAC_NCFGR_IRXER;
	}

	/* Allocate memory space for priority queue descriptor list */
	base = rtems_cache_coherent_allocate(sizeof(*base),
		GMAC_DESCRIPTOR_ALIGNMENT, 0);
	assert(base != NULL);

	base->addr.val = GMAC_RX_OWNERSHIP_BIT | GMAC_RX_WRAP_BIT;
	base->status.val = 0;

	GMAC_SetRxQueue(pHw, (uint32_t)base, 1);
	GMAC_SetRxQueue(pHw, (uint32_t)base, 2);

	/* Allocate memory space for buffer descriptor list */
	rx = rtems_cache_coherent_allocate(sizeof(*rx),
		GMAC_DESCRIPTOR_ALIGNMENT, 0);
	assert(rx != NULL);
	sc->rx = rx;
	mbufs = &sc->rx_mbufs[0];

	/* Create descriptor list and mark as empty */
	for (idx = 0; idx < RX_DESC_COUNT; ++idx) {
		m = if_atsam_new_mbuf(ifp);
		assert(m != NULL);
		mbufs[idx] = m;
		rx->bds[idx].addr.val = mtod(m, uint32_t) | RX_DESC_WRAP(idx);
	}

	/* Set 2 Byte Receive Buffer Offset */
	pHw->GMAC_NCFGR |= GMAC_NCFGR_RXBUFO(2);

	/* Write Buffer Queue Base Address Register */
	GMAC_ReceiveEnable(pHw, 0);
	GMAC_SetRxQueue(pHw, (uint32_t)&rx->bds[0], 0);

	/* Set address for address matching */
	GMAC_SetAddress(pHw, 0, sc->GMacAddress);

	/* Enable Receiving of data */
	GMAC_ReceiveEnable(pHw, 1);

	IF_ATSAM_UNLOCK(sc);

	idx = 0;

	while (true) {
		rtems_event_set out;

		/* Enable RX interrupts */
		pHw->GMAC_IER = GMAC_IER_RCOMP | GMAC_IER_ROVR;

		(void) rtems_event_receive(ATSAMV7_ETH_RX_EVENT_INTERRUPT,
		    RTEMS_EVENT_ALL | RTEMS_WAIT, RTEMS_NO_TIMEOUT, &out);

		while (true) {
			uint32_t addr;
			uint32_t status;

			addr = rx->bds[idx].addr.val;
			if ((addr & GMAC_RX_OWNERSHIP_BIT) == 0) {
				break;
			}

			status = rx->bds[idx].status.val;
			m = mbufs[idx];

			if (__predict_true((status & GMAC_RX_EOF_BIT) != 0)) {
				struct mbuf *n;

				n = if_atsam_new_mbuf(ifp);
				if (n != NULL) {
					if_atsam_rx_update_mbuf(m, status);
					(*ifp->if_input)(ifp, m);
					m = n;
				}
			} else {
				if_inc_counter(ifp, IFCOUNTER_IERRORS, 1);
			}

			mbufs[idx] = m;
			rx->bds[idx].addr.val = mtod(m, uint32_t) |
			    RX_DESC_WRAP(idx);

			idx = (idx + 1) % RX_DESC_COUNT;
		}
	}
}

static void
if_atsam_tx_reclaim(struct if_atsam_softc *sc, struct ifnet *ifp)
{
	uint32_t head_idx;
	uint32_t tail_idx;
	volatile sGmacTxDescriptor *base;

	head_idx = sc->tx_idx_head;
	tail_idx = sc->tx_idx_tail;
	base = &sc->tx->bds[0];

	while (head_idx != tail_idx) {
		uint32_t status;
		ift_counter cnt;
		struct mbuf *m;

		status = base[tail_idx].status.val;

		if ((status & GMAC_TX_USED_BIT) == 0) {
			break;
		}

		if (__predict_true((status & GMAC_TX_ERR_BITS) == 0)) {
			cnt = IFCOUNTER_OPACKETS;
		} else {
			cnt = IFCOUNTER_OERRORS;
		}

		while ((m = sc->tx_mbufs[tail_idx]) == NULL ) {
			base[tail_idx].status.val = status | GMAC_TX_USED_BIT;
			tail_idx = (tail_idx + 1) % TX_DESC_COUNT;
			status = base[tail_idx].status.val;

			if (__predict_false((status & GMAC_TX_ERR_BITS) != 0)) {
				cnt = IFCOUNTER_OERRORS;
			}
		}

		base[tail_idx].status.val = status | GMAC_TX_USED_BIT;
		if_inc_counter(ifp, cnt, 1);
		sc->tx_mbufs[tail_idx] = NULL;
		m_freem(m);

		tail_idx = (tail_idx + 1) % TX_DESC_COUNT;
	}

	sc->tx_idx_tail = tail_idx;
}

static void
if_atsam_cache_flush(uintptr_t begin, uintptr_t size)
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
if_atsam_tx_enqueue(struct if_atsam_softc *sc, struct ifnet *ifp, struct mbuf *m)
{
	size_t head_idx;
	size_t tail_idx;
	size_t capacity;
	size_t idx;
	volatile sGmacTxDescriptor *base;
	volatile sGmacTxDescriptor *desc;
	size_t bufs;
	uint32_t status;
	struct mbuf *n;

	head_idx = sc->tx_idx_head;
	tail_idx = sc->tx_idx_tail;
	capacity = (tail_idx - head_idx - 1) % TX_DESC_COUNT;

	idx = head_idx;
	base = &sc->tx->bds[0];
	bufs = 0;
	n = m;

	do {
		uint32_t size;

		desc = &base[idx];

		size = (uint32_t)n->m_len;
		if (__predict_true(size > 0)) {
			uintptr_t begin;

			++bufs;
			if (__predict_false(bufs > capacity)) {
				return (ENOBUFS);
			}

			begin = mtod(n, uintptr_t);
			desc->addr = (uint32_t)begin;
			status = GMAC_TX_USED_BIT | TX_DESC_WRAP(idx) | size;
			desc->status.val = status;
			if_atsam_cache_flush(begin, size);
			idx = (idx + 1) % TX_DESC_COUNT;
		}

		n = n->m_next;
	} while (n != NULL);

	sc->tx_idx_head = idx;

	idx = (idx - 1) % TX_DESC_COUNT;
	desc = &base[idx];
	sc->tx_mbufs[idx] = m;
	status = GMAC_TX_LAST_BUFFER_BIT;

	while (idx != head_idx) {
		desc->status.val = (desc->status.val & ~GMAC_TX_USED_BIT) |
		    status;
		status = 0;

		idx = (idx - 1) % TX_DESC_COUNT;
		desc = &base[idx];
	}

	desc->status.val = (desc->status.val & ~GMAC_TX_USED_BIT) | status;
	_ARM_Data_synchronization_barrier();
	sc->Gmac_inst.gGmacd.pHw->GMAC_NCR |= GMAC_NCR_TSTART;
	ETHER_BPF_MTAP(ifp, m);
	return (0);
}

static int
if_atsam_transmit(struct ifnet *ifp, struct mbuf *m)
{
	struct if_atsam_softc *sc;
	int error;

	if (__predict_false((m->m_flags & M_VLANTAG) != 0)) {
		struct mbuf *n;

		n = ether_vlanencap(m, m->m_pkthdr.ether_vtag);
		if (n == NULL) {
			m_freem(m);
			return (ENOBUFS);
		}

		m = n;
	}

	sc = ifp->if_softc;
	IF_ATSAM_LOCK(sc);

	error = if_atsam_tx_enqueue(sc, ifp, m);
	if_atsam_tx_reclaim(sc, ifp);

	if (__predict_false(error != 0)) {
		struct mbuf *n;

		n = m_defrag(m, M_NOWAIT);
		if (n != NULL) {
			m = n;
		}

		error = if_atsam_tx_enqueue(sc, ifp, m);
		if (error != 0) {
			m_freem(m);
			if_inc_counter(ifp, IFCOUNTER_OQDROPS, 1);
		}
	}

	IF_ATSAM_UNLOCK(sc);
	return (error);
}

static uint8_t if_atsam_get_gmac_linkspeed_from_media(uint32_t media_subtype)
{
	switch (media_subtype) {
	case IFM_10_T:
		return GMAC_SPEED_10M;
		break;
	case IFM_100_TX:
		return GMAC_SPEED_100M;
		break;
	case IFM_1000_T:
		return GMAC_SPEED_1000M;
		break;
	default:
		return 0xFF;
		break;
	}
}


static uint8_t if_atsam_get_gmac_duplex_from_media(uint32_t media_options)
{
	if (media_options & IFM_FDX) {
		return GMAC_DUPLEX_FULL;
	} else {
		return GMAC_DUPLEX_HALF;
	}
}


static void if_atsam_miibus_statchg(device_t dev)
{
	uint8_t link_speed = GMAC_SPEED_100M;
	uint8_t link_duplex = GMAC_DUPLEX_FULL;
	if_atsam_softc *sc = device_get_softc(dev);
	struct mii_data *mii = device_get_softc(sc->miibus);

	if(sc->fixed_speed)
		return;

	Gmac *pHw = sc->Gmac_inst.gGmacd.pHw;

	link_duplex = if_atsam_get_gmac_duplex_from_media(
	    IFM_OPTIONS(mii->mii_media_active));

	link_speed = if_atsam_get_gmac_linkspeed_from_media(
	    IFM_SUBTYPE(mii->mii_media_active));

	if (sc->link_speed != link_speed || sc->link_duplex != link_duplex) {
		GMAC_SetLinkSpeed(pHw, link_speed, link_duplex);
		sc->link_speed = link_speed;
		sc->link_duplex = link_duplex;
	}
}


static int
if_atsam_mii_ifmedia_upd(struct ifnet *ifp)
{
	if_atsam_softc *sc;
	struct mii_data *mii;

	sc = ifp->if_softc;
	if (sc->fixed_speed || sc->miibus == NULL)
		return (ENXIO);

	mii = device_get_softc(sc->miibus);
	return (mii_mediachg(mii));
}


static void
if_atsam_mii_ifmedia_sts(struct ifnet *ifp, struct ifmediareq *ifmr)
{
	if_atsam_softc *sc;
	struct mii_data *mii;

	sc = ifp->if_softc;
	if (sc->fixed_speed || sc->miibus == NULL)
		return;

	mii = device_get_softc(sc->miibus);
	mii_pollstat(mii);
	ifmr->ifm_active = mii->mii_media_active;
	ifmr->ifm_status = mii->mii_media_status;
}


static int
if_atsam_media_change(struct ifnet *ifp __unused)
{
	/* Do nothing. */
	return (0);
}


static void
if_atsam_media_status(struct ifnet *ifp, struct ifmediareq *imr)
{
	if_atsam_softc *sc = (if_atsam_softc *)ifp->if_softc;

	imr->ifm_status = IFM_AVALID | IFM_ACTIVE;
	imr->ifm_active = IFM_ETHER | sc->media | sc->duplex;
}


static void
if_atsam_tick(void *context)
{
	if_atsam_softc *sc = context;

	if_atsam_poll_hw_stats(sc);

	IF_ATSAM_UNLOCK(sc);

	if (!sc->fixed_speed) {
		mii_tick(device_get_softc(sc->miibus));
	}
	callout_reset(&sc->tick_ch, hz, if_atsam_tick, sc);
}

static void
if_atsam_setup_tx(struct if_atsam_softc *sc)
{
	sGmacTxDescriptor *base;
	struct if_atsam_tx_bds *tx;
	size_t i;
	Gmac *pHw;

	pHw = sc->Gmac_inst.gGmacd.pHw;
	GMAC_TransmitEnable(pHw, 0);

	/* Allocate memory space for priority queue descriptor list */
	base = rtems_cache_coherent_allocate(sizeof(base),
	    GMAC_DESCRIPTOR_ALIGNMENT, 0);
	assert(base != NULL);

	base->addr = 0;
	base->status.val = GMAC_TX_USED_BIT | GMAC_TX_WRAP_BIT;

	GMAC_SetTxQueue(pHw, (uint32_t)base, 1);
	GMAC_SetTxQueue(pHw, (uint32_t)base, 2);

	/* Allocate memory space for buffer descriptor list */
	tx = rtems_cache_coherent_allocate(sizeof(*sc->tx),
	    GMAC_DESCRIPTOR_ALIGNMENT, 0);
	assert(tx != NULL);

	/* Set variables in context */
	sc->tx = tx;

	/* Create descriptor list and mark as empty */
	for (i = 0; i < TX_DESC_COUNT; ++i) {
		tx->bds[i].addr = 0;
		tx->bds[i].status.val = GMAC_TX_USED_BIT | TX_DESC_WRAP(i);
	}

	/* Write Buffer Queue Base Address Register */
	GMAC_SetTxQueue(pHw, (uint32_t)&tx->bds[0], 0);

	/* Enable Transmission of data */
	GMAC_TransmitEnable(pHw, 1);
}

static void
if_atsam_init(if_atsam_softc *sc)
{
	rtems_status_code status;
	uint32_t dmac_cfg = 0;

	sc->interrupt_number = GMAC_IRQn;

	/* Enable Peripheral Clock */
	if ((PMC->PMC_PCSR1 & (1u << 7)) != (1u << 7)) {
		PMC->PMC_PCER1 = 1 << 7;
	}
	/* Setup interrupts */
	NVIC_ClearPendingIRQ(GMAC_IRQn);

	/* Configuration of DMAC */
	dmac_cfg = (GMAC_DCFGR_DRBS(GMAC_RX_BUFFER_SIZE >> 6)) |
	    GMAC_DCFGR_RXBMS(3) | GMAC_DCFGR_TXPBMS | GMAC_DCFGR_FBLDO_INCR16 |
	    GMAC_DCFGR_TXCOEN;
	GMAC_SetDMAConfig(sc->Gmac_inst.gGmacd.pHw, dmac_cfg, 0);

	/* Enable hardware checksum offload for receive */
	sc->Gmac_inst.gGmacd.pHw->GMAC_NCFGR |= GMAC_NCFGR_RXCOEN;

	/* Use Multicast Hash Filter */
	sc->Gmac_inst.gGmacd.pHw->GMAC_NCFGR |= GMAC_NCFGR_MTIHEN;
	sc->Gmac_inst.gGmacd.pHw->GMAC_HRB = 0;
	sc->Gmac_inst.gGmacd.pHw->GMAC_HRT = 0;

	/* Shut down Transmit and Receive */
	GMAC_ReceiveEnable(sc->Gmac_inst.gGmacd.pHw, 0);
	GMAC_TransmitEnable(sc->Gmac_inst.gGmacd.pHw, 0);

	GMAC_StatisticsWriteEnable(sc->Gmac_inst.gGmacd.pHw, 1);

	/* Install interrupt handler */
	status = rtems_interrupt_handler_install(sc->interrupt_number,
		"Ethernet",
		RTEMS_INTERRUPT_UNIQUE,
		if_atsam_interrupt_handler,
		sc);
	assert(status == RTEMS_SUCCESSFUL);

	/*
	 * Start driver tasks
	 */

	status = rtems_task_create(rtems_build_name('S', 'C', 'r', 'x'),
	    rtems_bsd_get_task_priority(device_get_name(sc->dev)), 4096,
	    RTEMS_DEFAULT_MODES, RTEMS_DEFAULT_MODES, &sc->rx_daemon_tid);
	assert(status == RTEMS_SUCCESSFUL);

	status = rtems_task_start(sc->rx_daemon_tid, if_atsam_rx_daemon,
	    (rtems_task_argument)sc);
	assert(status == RTEMS_SUCCESSFUL);

	callout_reset(&sc->tick_ch, hz, if_atsam_tick, sc);

	if_atsam_setup_tx(sc);
}

static int
if_atsam_get_hash_index(const uint8_t *eaddr)
{
	uint64_t eaddr64;
	int index;
	int i;

	eaddr64 = eaddr[5];

	for (i = 4; i >= 0; --i) {
		eaddr64 <<= 8;
		eaddr64 |= eaddr[i];
	}

	index = 0;

	for (i = 0; i < 6; ++i) {
		uint64_t bits;
		int j;
		int hash;

		bits = eaddr64 >> i;
		hash = bits & 1;

		for (j = 1; j < 8; ++j) {
			bits >>= 6;
			hash ^= bits & 1;
		}

		index |= hash << i;
	}

	return index;
}

static void
if_atsam_setup_rxfilter(struct if_atsam_softc *sc)
{
	struct ifnet *ifp;
	struct ifmultiaddr *ifma;
	uint64_t mhash;
	Gmac *pHw;

	pHw = sc->Gmac_inst.gGmacd.pHw;

	if ((sc->ifp->if_flags & IFF_PROMISC) != 0) {
		pHw->GMAC_NCFGR |= GMAC_NCFGR_CAF;
	} else {
		pHw->GMAC_NCFGR &= ~GMAC_NCFGR_CAF;
	}

	ifp = sc->ifp;

	if ((ifp->if_flags & IFF_ALLMULTI))
		mhash = 0xffffffffffffffffLLU;
	else {
		mhash = 0;
		if_maddr_rlock(ifp);
		CK_STAILQ_FOREACH(ifma, &sc->ifp->if_multiaddrs, ifma_link) {
			if (ifma->ifma_addr->sa_family != AF_LINK)
				continue;
			mhash |= 1LLU << if_atsam_get_hash_index(
			    LLADDR((struct sockaddr_dl *) ifma->ifma_addr));
		}
		if_maddr_runlock(ifp);
	}

	pHw->GMAC_HRB = (uint32_t)mhash;
	pHw->GMAC_HRT = (uint32_t)(mhash >> 32);
}

static void
if_atsam_start_locked(struct if_atsam_softc *sc)
{
	struct ifnet *ifp = sc->ifp;
	Gmac *pHw = sc->Gmac_inst.gGmacd.pHw;

	if (ifp->if_drv_flags & IFF_DRV_RUNNING) {
		return;
	}

	ifp->if_drv_flags |= IFF_DRV_RUNNING;

	if_atsam_setup_rxfilter(sc);

	/* Enable TX/RX */
	pHw->GMAC_NCR |= GMAC_NCR_RXEN | GMAC_NCR_TXEN;
}

static void
if_atsam_start(void *arg)
{
	struct if_atsam_softc *sc = arg;

	IF_ATSAM_LOCK(sc);
	if_atsam_start_locked(sc);
	IF_ATSAM_UNLOCK(sc);
}

static void
if_atsam_stop_locked(struct if_atsam_softc *sc)
{
	struct ifnet *ifp = sc->ifp;
	Gmac *pHw = sc->Gmac_inst.gGmacd.pHw;
	size_t i;

	ifp->if_drv_flags &= ~IFF_DRV_RUNNING;

	/* Disable TX/RX */
	pHw->GMAC_NCR &= ~(GMAC_NCR_RXEN | GMAC_NCR_TXEN);

	/* Reinitialize the TX descriptors */

	sc->tx_idx_head = 0;
	sc->tx_idx_tail = 0;

	for (i = 0; i < TX_DESC_COUNT; ++i) {
		sc->tx->bds[i].addr = 0;
		sc->tx->bds[i].status.val = GMAC_TX_USED_BIT | TX_DESC_WRAP(i);
		m_freem(sc->tx_mbufs[i]);
		sc->tx_mbufs[i] = NULL;
	}
}


static void
if_atsam_poll_hw_stats(struct if_atsam_softc *sc)
{
	uint64_t octets;
	Gmac *pHw = sc->Gmac_inst.gGmacd.pHw;

	octets = pHw->GMAC_OTLO;
	octets |= (uint64_t)pHw->GMAC_OTHI << 32;
	sc->stats.octets_transm += octets;
	sc->stats.frames_transm += pHw->GMAC_FT;
	sc->stats.broadcast_frames_transm += pHw->GMAC_BCFT;
	sc->stats.multicast_frames_transm += pHw->GMAC_MFT;
	sc->stats.pause_frames_transm += pHw->GMAC_PFT;
	sc->stats.frames_64_byte_transm += pHw->GMAC_BFT64;
	sc->stats.frames_65_to_127_byte_transm += pHw->GMAC_TBFT127;
	sc->stats.frames_128_to_255_byte_transm += pHw->GMAC_TBFT255;
	sc->stats.frames_256_to_511_byte_transm += pHw->GMAC_TBFT511;
	sc->stats.frames_512_to_1023_byte_transm += pHw->GMAC_TBFT1023;
	sc->stats.frames_1024_to_1518_byte_transm += pHw->GMAC_TBFT1518;
	sc->stats.frames_greater_1518_byte_transm += pHw->GMAC_GTBFT1518;
	sc->stats.transmit_underruns += pHw->GMAC_TUR;
	sc->stats.single_collision_frames += pHw->GMAC_SCF;
	sc->stats.multiple_collision_frames += pHw->GMAC_MCF;
	sc->stats.excessive_collisions += pHw->GMAC_EC;
	sc->stats.late_collisions += pHw->GMAC_LC;
	sc->stats.deferred_transmission_frames += pHw->GMAC_DTF;
	sc->stats.carrier_sense_errors += pHw->GMAC_CSE;

	octets = pHw->GMAC_ORLO;
	octets |= (uint64_t)pHw->GMAC_ORHI << 32;
	sc->stats.octets_rec += octets;
	sc->stats.frames_rec += pHw->GMAC_FR;
	sc->stats.broadcast_frames_rec += pHw->GMAC_BCFR;
	sc->stats.multicast_frames_rec += pHw->GMAC_MFR;
	sc->stats.pause_frames_rec += pHw->GMAC_PFR;
	sc->stats.frames_64_byte_rec += pHw->GMAC_BFR64;
	sc->stats.frames_65_to_127_byte_rec += pHw->GMAC_TBFR127;
	sc->stats.frames_128_to_255_byte_rec += pHw->GMAC_TBFR255;
	sc->stats.frames_256_to_511_byte_rec += pHw->GMAC_TBFR511;
	sc->stats.frames_512_to_1023_byte_rec += pHw->GMAC_TBFR1023;
	sc->stats.frames_1024_to_1518_byte_rec += pHw->GMAC_TBFR1518;
	sc->stats.frames_1519_to_maximum_byte_rec += pHw->GMAC_TMXBFR;
	sc->stats.undersize_frames_rec += pHw->GMAC_UFR;
	sc->stats.oversize_frames_rec += pHw->GMAC_OFR;
	sc->stats.jabbers_rec += pHw->GMAC_JR;
	sc->stats.frame_check_sequence_errors += pHw->GMAC_FCSE;
	sc->stats.length_field_frame_errors += pHw->GMAC_LFFE;
	sc->stats.receive_symbol_errors += pHw->GMAC_RSE;
	sc->stats.alignment_errors += pHw->GMAC_AE;
	sc->stats.receive_resource_errors += pHw->GMAC_RRE;
	sc->stats.receive_overrun += pHw->GMAC_ROE;

	sc->stats.ip_header_checksum_errors += pHw->GMAC_IHCE;
	sc->stats.tcp_checksum_errors += pHw->GMAC_TCE;
	sc->stats.udp_checksum_errors += pHw->GMAC_UCE;
}


static void
if_atsam_add_sysctls(device_t dev)
{
	struct if_atsam_softc *sc = device_get_softc(dev);
	struct sysctl_ctx_list *ctx;
	struct sysctl_oid_list *statsnode;
	struct sysctl_oid_list *hwstatsnode;
	struct sysctl_oid_list *child;
	struct sysctl_oid *tree;

	ctx = device_get_sysctl_ctx(dev);
	child = SYSCTL_CHILDREN(device_get_sysctl_tree(dev));

	tree = SYSCTL_ADD_NODE(ctx, child, OID_AUTO, "stats", CTLFLAG_RD,
			       NULL, "if_atsam statistics");
	statsnode = SYSCTL_CHILDREN(tree);

	tree = SYSCTL_ADD_NODE(ctx, statsnode, OID_AUTO, "sw", CTLFLAG_RD,
			       NULL, "if_atsam software statistics");
	child = SYSCTL_CHILDREN(tree);

	SYSCTL_ADD_UINT(ctx, child, OID_AUTO, "rx_overrun_errors",
	    CTLFLAG_RD, &sc->stats.rx_overrun_errors, 0,
	    "RX overrun errors");
	SYSCTL_ADD_UINT(ctx, child, OID_AUTO, "rx_interrupts",
	    CTLFLAG_RD, &sc->stats.rx_interrupts, 0,
	    "Rx interrupts");
	SYSCTL_ADD_UINT(ctx, child, OID_AUTO, "tx_tur_errors",
	    CTLFLAG_RD, &sc->stats.tx_tur_errors, 0,
	    "Error Tur Tx interrupts");
	SYSCTL_ADD_UINT(ctx, child, OID_AUTO, "tx_rlex_errors",
	    CTLFLAG_RD, &sc->stats.tx_rlex_errors, 0,
	    "Error Rlex Tx interrupts");
	SYSCTL_ADD_UINT(ctx, child, OID_AUTO, "tx_tfc_errors",
	    CTLFLAG_RD, &sc->stats.tx_tfc_errors, 0,
	    "Error Tfc Tx interrupts");
	SYSCTL_ADD_UINT(ctx, child, OID_AUTO, "tx_hresp_errors",
	    CTLFLAG_RD, &sc->stats.tx_hresp_errors, 0,
	    "Error Hresp Tx interrupts");

	tree = SYSCTL_ADD_NODE(ctx, statsnode, OID_AUTO, "hw", CTLFLAG_RD,
			       NULL, "if_atsam hardware statistics");
	hwstatsnode = SYSCTL_CHILDREN(tree);

	tree = SYSCTL_ADD_NODE(ctx, hwstatsnode, OID_AUTO, "tx", CTLFLAG_RD,
			       NULL, "if_atsam hardware transmit statistics");
	child = SYSCTL_CHILDREN(tree);

	SYSCTL_ADD_UQUAD(ctx, child, OID_AUTO, "octets_transm",
	    CTLFLAG_RD, &sc->stats.octets_transm,
	    "Octets Transmitted");
	SYSCTL_ADD_UINT(ctx, child, OID_AUTO, "frames_transm",
	    CTLFLAG_RD, &sc->stats.frames_transm, 0,
	    "Frames Transmitted");
	SYSCTL_ADD_UINT(ctx, child, OID_AUTO, "broadcast_frames_transm",
	    CTLFLAG_RD, &sc->stats.broadcast_frames_transm, 0,
	    "Broadcast Frames Transmitted");
	SYSCTL_ADD_UINT(ctx, child, OID_AUTO, "multicast_frames_transm",
	    CTLFLAG_RD, &sc->stats.multicast_frames_transm, 0,
	    "Multicast Frames Transmitted");
	SYSCTL_ADD_UINT(ctx, child, OID_AUTO, "pause_frames_transm",
	    CTLFLAG_RD, &sc->stats.pause_frames_transm, 0,
	    "Pause Frames Transmitted");
	SYSCTL_ADD_UINT(ctx, child, OID_AUTO, "frames_64_byte_transm",
	    CTLFLAG_RD, &sc->stats.frames_64_byte_transm, 0,
	    "64 Byte Frames Transmitted");
	SYSCTL_ADD_UINT(ctx, child, OID_AUTO, "frames_65_to_127_byte_transm",
	    CTLFLAG_RD, &sc->stats.frames_65_to_127_byte_transm, 0,
	    "65 to 127 Byte Frames Transmitted");
	SYSCTL_ADD_UINT(ctx, child, OID_AUTO, "frames_128_to_255_byte_transm",
	    CTLFLAG_RD, &sc->stats.frames_128_to_255_byte_transm, 0,
	    "128 to 255 Byte Frames Transmitted");
	SYSCTL_ADD_UINT(ctx, child, OID_AUTO, "frames_256_to_511_byte_transm",
	    CTLFLAG_RD, &sc->stats.frames_256_to_511_byte_transm, 0,
	    "256 to 511 Byte Frames Transmitted");
	SYSCTL_ADD_UINT(ctx, child, OID_AUTO, "frames_512_to_1023_byte_transm",
	    CTLFLAG_RD, &sc->stats.frames_512_to_1023_byte_transm, 0,
	    "512 to 1023 Byte Frames Transmitted");
	SYSCTL_ADD_UINT(ctx, child, OID_AUTO, "frames_1024_to_1518_byte_transm",
	    CTLFLAG_RD, &sc->stats.frames_1024_to_1518_byte_transm, 0,
	    "1024 to 1518 Byte Frames Transmitted");
	SYSCTL_ADD_UINT(ctx, child, OID_AUTO, "frames_greater_1518_byte_transm",
	    CTLFLAG_RD, &sc->stats.frames_greater_1518_byte_transm, 0,
	    "Greater Than 1518 Byte Frames Transmitted");
	SYSCTL_ADD_UINT(ctx, child, OID_AUTO, "transmit_underruns",
	    CTLFLAG_RD, &sc->stats.transmit_underruns, 0,
	    "Transmit Underruns");
	SYSCTL_ADD_UINT(ctx, child, OID_AUTO, "single_collision_frames",
	    CTLFLAG_RD, &sc->stats.single_collision_frames, 0,
	    "Single Collision Frames");
	SYSCTL_ADD_UINT(ctx, child, OID_AUTO, "multiple_collision_frames",
	    CTLFLAG_RD, &sc->stats.multiple_collision_frames, 0,
	    "Multiple Collision Frames");
	SYSCTL_ADD_UINT(ctx, child, OID_AUTO, "excessive_collisions",
	    CTLFLAG_RD, &sc->stats.excessive_collisions, 0,
	    "Excessive Collisions");
	SYSCTL_ADD_UINT(ctx, child, OID_AUTO, "late_collisions",
	    CTLFLAG_RD, &sc->stats.late_collisions, 0,
	    "Late Collisions");
	SYSCTL_ADD_UINT(ctx, child, OID_AUTO, "deferred_transmission_frames",
	    CTLFLAG_RD, &sc->stats.deferred_transmission_frames, 0,
	    "Deferred Transmission Frames");
	SYSCTL_ADD_UINT(ctx, child, OID_AUTO, "carrier_sense_errors",
	    CTLFLAG_RD, &sc->stats.carrier_sense_errors, 0,
	    "Carrier Sense Errors");

	tree = SYSCTL_ADD_NODE(ctx, hwstatsnode, OID_AUTO, "rx", CTLFLAG_RD,
			       NULL, "if_atsam hardware receive statistics");
	child = SYSCTL_CHILDREN(tree);

	SYSCTL_ADD_UQUAD(ctx, child, OID_AUTO, "octets_rec",
	    CTLFLAG_RD, &sc->stats.octets_rec,
	    "Octets Received");
	SYSCTL_ADD_UINT(ctx, child, OID_AUTO, "frames_rec",
	    CTLFLAG_RD, &sc->stats.frames_rec, 0,
	    "Frames Received");
	SYSCTL_ADD_UINT(ctx, child, OID_AUTO, "broadcast_frames_rec",
	    CTLFLAG_RD, &sc->stats.broadcast_frames_rec, 0,
	    "Broadcast Frames Received");
	SYSCTL_ADD_UINT(ctx, child, OID_AUTO, "multicast_frames_rec",
	    CTLFLAG_RD, &sc->stats.multicast_frames_rec, 0,
	    "Multicast Frames Received");
	SYSCTL_ADD_UINT(ctx, child, OID_AUTO, "pause_frames_rec",
	    CTLFLAG_RD, &sc->stats.pause_frames_rec, 0,
	    "Pause Frames Received");
	SYSCTL_ADD_UINT(ctx, child, OID_AUTO, "frames_64_byte_rec",
	    CTLFLAG_RD, &sc->stats.frames_64_byte_rec, 0,
	    "64 Byte Frames Received");
	SYSCTL_ADD_UINT(ctx, child, OID_AUTO, "frames_65_to_127_byte_rec",
	    CTLFLAG_RD, &sc->stats.frames_65_to_127_byte_rec, 0,
	    "65 to 127 Byte Frames Received");
	SYSCTL_ADD_UINT(ctx, child, OID_AUTO, "frames_128_to_255_byte_rec",
	    CTLFLAG_RD, &sc->stats.frames_128_to_255_byte_rec, 0,
	    "128 to 255 Byte Frames Received");
	SYSCTL_ADD_UINT(ctx, child, OID_AUTO, "frames_256_to_511_byte_rec",
	    CTLFLAG_RD, &sc->stats.frames_256_to_511_byte_rec, 0,
	    "256 to 511 Byte Frames Received");
	SYSCTL_ADD_UINT(ctx, child, OID_AUTO, "frames_512_to_1023_byte_rec",
	    CTLFLAG_RD, &sc->stats.frames_512_to_1023_byte_rec, 0,
	    "512 to 1023 Byte Frames Received");
	SYSCTL_ADD_UINT(ctx, child, OID_AUTO, "frames_1024_to_1518_byte_rec",
	    CTLFLAG_RD, &sc->stats.frames_1024_to_1518_byte_rec, 0,
	    "1024 to 1518 Byte Frames Received");
	SYSCTL_ADD_UINT(ctx, child, OID_AUTO, "frames_1519_to_maximum_byte_rec",
	    CTLFLAG_RD, &sc->stats.frames_1519_to_maximum_byte_rec, 0,
	    "1519 to Maximum Byte Frames Received");
	SYSCTL_ADD_UINT(ctx, child, OID_AUTO, "undersize_frames_rec",
	    CTLFLAG_RD, &sc->stats.undersize_frames_rec, 0,
	    "Undersize Frames Received");
	SYSCTL_ADD_UINT(ctx, child, OID_AUTO, "oversize_frames_rec",
	    CTLFLAG_RD, &sc->stats.oversize_frames_rec, 0,
	    "Oversize Frames Received");
	SYSCTL_ADD_UINT(ctx, child, OID_AUTO, "jabbers_rec",
	    CTLFLAG_RD, &sc->stats.jabbers_rec, 0,
	    "Jabbers Received");
	SYSCTL_ADD_UINT(ctx, child, OID_AUTO, "frame_check_sequence_errors",
	    CTLFLAG_RD, &sc->stats.frame_check_sequence_errors, 0,
	    "Frame Check Sequence Errors");
	SYSCTL_ADD_UINT(ctx, child, OID_AUTO, "length_field_frame_errors",
	    CTLFLAG_RD, &sc->stats.length_field_frame_errors, 0,
	    "Length Field Frame Errors");
	SYSCTL_ADD_UINT(ctx, child, OID_AUTO, "receive_symbol_errors",
	    CTLFLAG_RD, &sc->stats.receive_symbol_errors, 0,
	    "Receive Symbol Errors");
	SYSCTL_ADD_UINT(ctx, child, OID_AUTO, "alignment_errors",
	    CTLFLAG_RD, &sc->stats.alignment_errors, 0,
	    "Alignment Errors");
	SYSCTL_ADD_UINT(ctx, child, OID_AUTO, "receive_resource_errors",
	    CTLFLAG_RD, &sc->stats.receive_resource_errors, 0,
	    "Receive Resource Errors");
	SYSCTL_ADD_UINT(ctx, child, OID_AUTO, "receive_overrun",
	    CTLFLAG_RD, &sc->stats.receive_overrun, 0,
	    "Receive Overrun");
	SYSCTL_ADD_UINT(ctx, child, OID_AUTO, "ip_header_checksum_errors",
	    CTLFLAG_RD, &sc->stats.ip_header_checksum_errors, 0,
	    "IP Header Checksum Errors");
	SYSCTL_ADD_UINT(ctx, child, OID_AUTO, "tcp_checksum_errors",
	    CTLFLAG_RD, &sc->stats.tcp_checksum_errors, 0,
	    "TCP Checksum Errors");
	SYSCTL_ADD_UINT(ctx, child, OID_AUTO, "udp_checksum_errors",
	    CTLFLAG_RD, &sc->stats.udp_checksum_errors, 0,
	    "UDP Checksum Errors");
}

static int
if_atsam_mediaioctl(if_atsam_softc *sc, struct ifreq *ifr, u_long command)
{
	if (sc->fixed_speed) {
		return ifmedia_ioctl(sc->ifp, ifr, &sc->ifmedia, command);
	} else {
		struct mii_data *mii;

		if (sc->miibus == NULL)
			return (EINVAL);

		mii = device_get_softc(sc->miibus);
		return (ifmedia_ioctl(sc->ifp, ifr, &mii->mii_media, command));
	}
}


/*
 * Driver ioctl handler
 */
static int
if_atsam_ioctl(struct ifnet *ifp, ioctl_command_t command, caddr_t data)
{
	if_atsam_softc *sc = (if_atsam_softc *)ifp->if_softc;
	struct ifreq *ifr = (struct ifreq *)data;
	int rv = 0;

	switch (command) {
	case SIOCGIFMEDIA:
	case SIOCSIFMEDIA:
		rv = if_atsam_mediaioctl(sc, ifr, command);
		break;
	case SIOCSIFFLAGS:
		IF_ATSAM_LOCK(sc);
		if (ifp->if_flags & IFF_UP) {
			if (ifp->if_drv_flags & IFF_DRV_RUNNING) {
				if ((ifp->if_flags ^ sc->if_flags) &
				    (IFF_PROMISC | IFF_ALLMULTI)) {
					if_atsam_setup_rxfilter(sc);
				}
			} else {
				if_atsam_start_locked(sc);
			}
		} else {
			if (ifp->if_drv_flags & IFF_DRV_RUNNING) {
				if_atsam_stop_locked(sc);
			}
		}
		sc->if_flags = ifp->if_flags;
		IF_ATSAM_UNLOCK(sc);
		break;
	case SIOCADDMULTI:
	case SIOCDELMULTI:
		if (ifp->if_drv_flags & IFF_DRV_RUNNING) {
			IF_ATSAM_LOCK(sc);
			if_atsam_setup_rxfilter(sc);
			IF_ATSAM_UNLOCK(sc);
		}
		break;
	default:
		rv = ether_ioctl(ifp, command, data);
		break;
	}
	return (rv);
}

/*
 * Attach an SAMV71 driver to the system
 */
static int if_atsam_driver_attach(device_t dev)
{
	if_atsam_softc *sc;
	struct ifnet *ifp;
	int unit;
	uint8_t eaddr[ETHER_ADDR_LEN];

	sc = device_get_softc(dev);
	unit = device_get_unit(dev);
	assert(unit == 0);

	sc->dev = dev;
	sc->ifp = ifp = if_alloc(IFT_ETHER);

	mtx_init(&sc->mtx, device_get_nameunit(sc->dev), MTX_NETWORK_LOCK,
	    MTX_DEF);

	rtems_bsd_if_atsam_get_if_media_props(device_get_name(sc->dev), unit,
	    &sc->fixed_speed, &sc->media, &sc->duplex);
	rtems_bsd_get_mac_address(device_get_name(sc->dev), unit, eaddr);

	sc->Gmac_inst.retries = MDIO_RETRIES;

	memcpy(sc->GMacAddress, eaddr, ETHER_ADDR_LEN);

	/* Set Initial Link Speed */
	sc->link_speed = GMAC_SPEED_100M;
	sc->link_duplex = GMAC_DUPLEX_FULL;

	GMACD_Init(&sc->Gmac_inst.gGmacd, GMAC, ID_GMAC, GMAC_CAF_ENABLE,
	    GMAC_NBC_DISABLE);

	/* Enable MDIO interface */
	GMAC_EnableMdio(sc->Gmac_inst.gGmacd.pHw);

	/* PHY initialize */
	if_atsam_init_phy(&sc->Gmac_inst, BOARD_MCK, NULL, 0,
	    gmacPins, PIO_LISTSIZE(gmacPins));

	/*
	 * MII Bus
	 */
	callout_init_mtx(&sc->tick_ch, &sc->mtx, CALLOUT_RETURNUNLOCKED);
	if (!sc->fixed_speed) {
		mii_attach(dev, &sc->miibus, ifp, if_atsam_mii_ifmedia_upd,
		    if_atsam_mii_ifmedia_sts, BMSR_DEFCAPMASK,
		    MDIO_PHY, MII_OFFSET_ANY, 0);
	} else {
		ifmedia_init(&sc->ifmedia, 0, if_atsam_media_change,
		    if_atsam_media_status);
		ifmedia_add(&sc->ifmedia, IFM_ETHER | sc->media
		    | sc->duplex, 0, NULL);
		ifmedia_set(&sc->ifmedia, IFM_ETHER | sc->media
		    | sc->duplex);

		GMAC_SetLinkSpeed(sc->Gmac_inst.gGmacd.pHw,
		    if_atsam_get_gmac_linkspeed_from_media(sc->media),
		    if_atsam_get_gmac_duplex_from_media(sc->duplex));

		if_link_state_change(sc->ifp, LINK_STATE_UP);
	}

	/*
	 * Set up network interface values
	 */
	ifp->if_softc = sc;
	if_initname(ifp, device_get_name(dev), device_get_unit(dev));
	ifp->if_init = if_atsam_start;
	ifp->if_ioctl = if_atsam_ioctl;
	ifp->if_transmit = if_atsam_transmit;
	ifp->if_qflush = if_qflush;
	ifp->if_flags = IFF_BROADCAST | IFF_MULTICAST | IFF_SIMPLEX;
	ifp->if_capabilities |= IFCAP_HWCSUM | IFCAP_HWCSUM_IPV6 |
	    IFCAP_VLAN_HWCSUM | IFCAP_VLAN_HWTAGGING;
	ifp->if_capenable = ifp->if_capabilities;
	ifp->if_hwassist = CSUM_IP | CSUM_IP_UDP | CSUM_IP_TCP |
	    CSUM_IP6_UDP | CSUM_IP6_TCP;
	IFQ_SET_MAXLEN(&ifp->if_snd, TX_DESC_COUNT - 1);
	ifp->if_snd.ifq_drv_maxlen = TX_DESC_COUNT - 1;
	IFQ_SET_READY(&ifp->if_snd);
	ifp->if_hdrlen = sizeof(struct ether_vlan_header);

	/*
	 * Attach the interface
	 */
	ether_ifattach(ifp, eaddr);

	if_atsam_add_sysctls(dev);
	if_atsam_init(sc);

	return (0);
}

static int
if_atsam_probe(device_t dev)
{
	int unit = device_get_unit(dev);
	int error;

	if (unit >= 0 && unit < NIFACES) {
		error = BUS_PROBE_DEFAULT;
	} else {
		error = ENXIO;
	}

	return (error);
}

static device_method_t if_atsam_methods[] = {
	DEVMETHOD(device_probe,		if_atsam_probe),
	DEVMETHOD(device_attach,	if_atsam_driver_attach),
	DEVMETHOD(miibus_readreg,	if_atsam_miibus_readreg),
	DEVMETHOD(miibus_writereg,	if_atsam_miibus_writereg),
	DEVMETHOD(miibus_statchg,	if_atsam_miibus_statchg),
	DEVMETHOD_END
};

static driver_t if_atsam_nexus_driver = {
	"if_atsam",
	if_atsam_methods,
	sizeof(struct if_atsam_softc)
};

static devclass_t if_atsam_devclass;
DRIVER_MODULE(if_atsam, nexus, if_atsam_nexus_driver, if_atsam_devclass, 0, 0);
MODULE_DEPEND(if_atsam, nexus, 1, 1, 1);
MODULE_DEPEND(if_atsam, ether, 1, 1, 1);
MODULE_DEPEND(if_atsam, miibus, 1, 1, 1);
DRIVER_MODULE(miibus, if_atsam, miibus_driver, miibus_devclass, NULL, NULL);

#endif /* LIBBSP_ARM_ATSAM_BSP_H */
