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

#include <net/if.h>
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

/** Multicast Enable */
#define GMAC_MC_ENABLE				(1u << 6)
#define HASH_INDEX_AMOUNT			6
#define HASH_ELEMENTS_PER_INDEX			8
#define MAC_ADDR_MASK				0x0000FFFFFFFFFFFF
#define MAC_IDX_MASK				(1u << 0)

/** Promiscuous Mode Enable */
#define GMAC_PROM_ENABLE			(1u << 4)

/** RX Defines */
#define GMAC_RX_BUFFER_SIZE			1536
#define GMAC_RX_BUF_DESC_ADDR_MASK		0xFFFFFFFC
#define GMAC_RX_SET_OFFSET			(1u << 15)
#define GMAC_RX_SET_USED_WRAP			((1u << 1) | (1u << 0))
#define GMAC_RX_SET_WRAP			(1u << 1)
#define GMAC_RX_SET_USED			(1u << 0)
/** TX Defines */
#define GMAC_TX_SET_EOF				(1u << 15)
#define GMAC_TX_SET_WRAP			(1u << 30)
#define GMAC_TX_SET_USED			(1u << 31)

#define GMAC_DESCRIPTOR_ALIGNMENT		8

/** Events */
#define ATSAMV7_ETH_RX_EVENT_INTERRUPT		RTEMS_EVENT_1
#define ATSAMV7_ETH_TX_EVENT_INTERRUPT		RTEMS_EVENT_2
#define ATSAMV7_ETH_START_TRANSMIT_EVENT	RTEMS_EVENT_3

#define ATSAMV7_ETH_RX_DATA_OFFSET		2

#define WATCHDOG_TIMEOUT			5

/* FIXME: Make these configurable */
#define MDIO_RETRIES 10
#define MDIO_PHY MII_PHY_ANY
#define RXBUF_COUNT 8
#define TXBUF_COUNT 64
#define IGNORE_RX_ERR false

/** The PINs for GMAC */
static const Pin gmacPins[] = { BOARD_GMAC_RUN_PINS };

typedef struct if_atsam_gmac {
	/** The GMAC driver instance */
	sGmacd gGmacd;
	uint32_t retries;
} if_atsam_gmac;

typedef struct ring_buffer {
	unsigned tx_bd_used;
	unsigned tx_bd_free;
	size_t length;
} ring_buffer;

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
	uint8_t GMacAddress[6];
	rtems_id rx_daemon_tid;
	rtems_id tx_daemon_tid;
	rtems_vector_number interrupt_number;
	struct mbuf **rx_mbuf;
	struct mbuf **tx_mbuf;
	volatile sGmacTxDescriptor *tx_bd_base;
	size_t rx_bd_fill_idx;
	size_t amount_rx_buf;
	size_t amount_tx_buf;
	ring_buffer tx_ring;
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
		uint32_t tx_complete_int;
		uint32_t tx_tur_errors;
		uint32_t tx_rlex_errors;
		uint32_t tx_tfc_errors;
		uint32_t tx_hresp_errors;
		uint32_t tx_interrupts;

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
} if_atsam_softc;

static void if_atsam_poll_hw_stats(struct if_atsam_softc *sc);

#define IF_ATSAM_LOCK(sc) mtx_lock(&(sc)->mtx)

#define IF_ATSAM_UNLOCK(sc) mtx_unlock(&(sc)->mtx)

static void if_atsam_event_send(rtems_id task, rtems_event_set event)
{
	rtems_event_send(task, event);
}


static void if_atsam_event_receive(if_atsam_softc *sc, rtems_event_set in)
{
	rtems_event_set out;

	IF_ATSAM_UNLOCK(sc);
	rtems_event_receive(
	    in,
	    RTEMS_EVENT_ANY | RTEMS_WAIT,
	    RTEMS_NO_TIMEOUT,
	    &out
	);
	IF_ATSAM_LOCK(sc);
}


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
	uint8_t err;
	if_atsam_softc *sc = device_get_softc(dev);

	IF_ATSAM_LOCK(sc);
	err = if_atsam_write_phy(sc->Gmac_inst.gGmacd.pHw,
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
	uint32_t irq_status_val;
	rtems_event_set rx_event = 0;
	rtems_event_set tx_event = 0;
	Gmac *pHw = sc->Gmac_inst.gGmacd.pHw;

	/* Get interrupt status */
	irq_status_val = GMAC_GetItStatus(pHw, 0);

	/* Check receive interrupts */
	if ((irq_status_val & GMAC_IER_ROVR) != 0) {
		++sc->stats.rx_overrun_errors;
		rx_event = ATSAMV7_ETH_RX_EVENT_INTERRUPT;
	}
	if ((irq_status_val & GMAC_IER_RCOMP) != 0) {
		rx_event = ATSAMV7_ETH_RX_EVENT_INTERRUPT;
	}
	/* Send events to receive task and switch off rx interrupts */
	if (rx_event != 0) {
		++sc->stats.rx_interrupts;
		/* Erase the interrupts for RX completion and errors */
		GMAC_DisableIt(pHw, GMAC_IER_RCOMP | GMAC_IER_ROVR, 0);
		(void)if_atsam_event_send(sc->rx_daemon_tid, rx_event);
	}
	if ((irq_status_val & GMAC_IER_TUR) != 0) {
		++sc->stats.tx_tur_errors;
		tx_event = ATSAMV7_ETH_TX_EVENT_INTERRUPT;
	}
	if ((irq_status_val & GMAC_IER_RLEX) != 0) {
		++sc->stats.tx_rlex_errors;
		tx_event = ATSAMV7_ETH_TX_EVENT_INTERRUPT;
	}
	if ((irq_status_val & GMAC_IER_TFC) != 0) {
		++sc->stats.tx_tfc_errors;
		tx_event = ATSAMV7_ETH_TX_EVENT_INTERRUPT;
	}
	if ((irq_status_val & GMAC_IER_HRESP) != 0) {
		++sc->stats.tx_hresp_errors;
		tx_event = ATSAMV7_ETH_TX_EVENT_INTERRUPT;
	}
	if ((irq_status_val & GMAC_IER_TCOMP) != 0) {
		++sc->stats.tx_complete_int;
		tx_event = ATSAMV7_ETH_TX_EVENT_INTERRUPT;
	}
	/* Send events to transmit task and switch off tx interrupts */
	if (tx_event != 0) {
		++sc->stats.tx_interrupts;
		/* Erase the interrupts for TX completion and errors */
		GMAC_DisableIt(pHw, GMAC_INT_TX_BITS, 0);
		(void)if_atsam_event_send(sc->tx_daemon_tid, tx_event);
	}
}

static void rx_update_mbuf(struct mbuf *m, sGmacRxDescriptor *buffer_desc)
{
	int frame_len;

	frame_len = (int) (buffer_desc->status.bm.len);

	m->m_data = mtod(m, char*)+ETHER_ALIGN;
	m->m_len = frame_len;
	m->m_pkthdr.len = frame_len;

	/* check checksum offload result */
	m->m_pkthdr.csum_flags = 0;
	switch (buffer_desc->status.bm.typeIDMatchOrCksumResult) {
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

/*
 * Receive daemon
 */
static void if_atsam_rx_daemon(void *arg)
{
	if_atsam_softc *sc = (if_atsam_softc *)arg;
	struct ifnet *ifp = sc->ifp;
	rtems_event_set events = 0;
	void *rx_bd_base;
	struct mbuf *m;
	struct mbuf *n;
	volatile sGmacRxDescriptor *buffer_desc;
	uint32_t tmp_rx_bd_address;
	size_t i;
	Gmac *pHw = sc->Gmac_inst.gGmacd.pHw;

	IF_ATSAM_LOCK(sc);

	if (IGNORE_RX_ERR) {
		pHw->GMAC_NCFGR |= GMAC_NCFGR_IRXER;
	} else {
		pHw->GMAC_NCFGR &= ~GMAC_NCFGR_IRXER;
	}

	/* Allocate memory space for priority queue descriptor list */
	rx_bd_base = rtems_cache_coherent_allocate(sizeof(sGmacRxDescriptor),
		GMAC_DESCRIPTOR_ALIGNMENT, 0);
	assert(rx_bd_base != NULL);

	buffer_desc = (sGmacRxDescriptor *)rx_bd_base;
	buffer_desc->addr.val = GMAC_RX_SET_USED_WRAP;
	buffer_desc->status.val = 0;

	GMAC_SetRxQueue(pHw, (uint32_t)buffer_desc, 1);
	GMAC_SetRxQueue(pHw, (uint32_t)buffer_desc, 2);

	/* Allocate memory space for buffer descriptor list */
	rx_bd_base = rtems_cache_coherent_allocate(
		sc->amount_rx_buf * sizeof(sGmacRxDescriptor),
		GMAC_DESCRIPTOR_ALIGNMENT, 0);
	assert(rx_bd_base != NULL);
	buffer_desc = (sGmacRxDescriptor *)rx_bd_base;

	/* Create descriptor list and mark as empty */
	for (sc->rx_bd_fill_idx = 0; sc->rx_bd_fill_idx < sc->amount_rx_buf;
	    ++sc->rx_bd_fill_idx) {
		m = if_atsam_new_mbuf(ifp);
		assert(m != NULL);
		sc->rx_mbuf[sc->rx_bd_fill_idx] = m;
		buffer_desc->addr.val = ((uint32_t)m->m_data) &
		    GMAC_RX_BUF_DESC_ADDR_MASK;
		buffer_desc->status.val = 0;
		if (sc->rx_bd_fill_idx == (sc->amount_rx_buf - 1)) {
			buffer_desc->addr.bm.bWrap = 1;
		} else {
			buffer_desc++;
		}
	}
	buffer_desc = (sGmacRxDescriptor *)rx_bd_base;

	/* Set 2 Byte Receive Buffer Offset */
	pHw->GMAC_NCFGR |= GMAC_RX_SET_OFFSET;

	/* Write Buffer Queue Base Address Register */
	GMAC_ReceiveEnable(pHw, 0);
	GMAC_SetRxQueue(pHw, (uint32_t)buffer_desc, 0);

	/* Set address for address matching */
	GMAC_SetAddress(pHw, 0, sc->GMacAddress);

	/* Enable Receiving of data */
	GMAC_ReceiveEnable(pHw, 1);

	/* Setup the interrupts for RX completion and errors */
	GMAC_EnableIt(pHw, GMAC_IER_RCOMP | GMAC_IER_ROVR, 0);

	sc->rx_bd_fill_idx = 0;

	while (true) {
		/* Wait for events */
		if_atsam_event_receive(sc, ATSAMV7_ETH_RX_EVENT_INTERRUPT);

		/*
		 * Check for all packets with a set ownership bit
		 */
		while (buffer_desc->addr.bm.bOwnership == 1) {
			if (buffer_desc->status.bm.bEof == 1) {
				m = sc->rx_mbuf[sc->rx_bd_fill_idx];

				/* New mbuf for desc */
				n = if_atsam_new_mbuf(ifp);
				if (n != NULL) {
					rx_update_mbuf(m, buffer_desc);

					IF_ATSAM_UNLOCK(sc);
					sc->ifp->if_input(ifp, m);
					IF_ATSAM_LOCK(sc);
					m = n;
				} else {
					(void)if_atsam_event_send(
					    sc->tx_daemon_tid, ATSAMV7_ETH_START_TRANSMIT_EVENT);
				}
				sc->rx_mbuf[sc->rx_bd_fill_idx] = m;
				tmp_rx_bd_address = (uint32_t)m->m_data &
				    GMAC_RX_BUF_DESC_ADDR_MASK;

				/* Switch pointer to next buffer descriptor */
				if (sc->rx_bd_fill_idx ==
				    (sc->amount_rx_buf - 1)) {
					tmp_rx_bd_address |= GMAC_RX_SET_WRAP;
					sc->rx_bd_fill_idx = 0;
				} else {
					++sc->rx_bd_fill_idx;
				}

				/*
				 * Give ownership to GMAC for further processing
				 */
				tmp_rx_bd_address &= ~GMAC_RX_SET_USED;
				_ARM_Data_synchronization_barrier();
				buffer_desc->addr.val = tmp_rx_bd_address;

				buffer_desc = (sGmacRxDescriptor *)rx_bd_base
				    + sc->rx_bd_fill_idx;
			}
		}
		/* Setup the interrupts for RX completion and errors */
		GMAC_EnableIt(pHw, GMAC_IER_RCOMP | GMAC_IER_ROVR, 0);
	}
}

/*
 * Update of current transmit buffer position.
 */
static void if_atsam_tx_bd_pos_update(size_t *pos, size_t amount_tx_buf)
{
	*pos = (*pos + 1) % amount_tx_buf;
}

/*
 * Is RingBuffer empty
 */
static bool if_atsam_ring_buffer_empty(ring_buffer *ring_buffer)
{
	return (ring_buffer->tx_bd_used == ring_buffer->tx_bd_free);
}

/*
 * Is RingBuffer full
 */
static bool if_atsam_ring_buffer_full(ring_buffer *ring_buffer)
{
	size_t tx_bd_used_next = ring_buffer->tx_bd_used;

	if_atsam_tx_bd_pos_update(&tx_bd_used_next, ring_buffer->length);
	return (tx_bd_used_next == ring_buffer->tx_bd_free);
}

/*
 * Cleanup transmit file descriptors by freeing mbufs which are not needed any
 * longer due to correct transmission.
 */
static void if_atsam_tx_bd_cleanup(if_atsam_softc *sc)
{
	struct mbuf *m;
	volatile sGmacTxDescriptor *cur;
	bool eof_needed = false;

	while (!if_atsam_ring_buffer_empty(&sc->tx_ring)){
		cur = sc->tx_bd_base + sc->tx_ring.tx_bd_free;
		if (((cur->status.bm.bUsed == 1) && !eof_needed) || eof_needed) {
			eof_needed = true;
			cur->status.val |= GMAC_TX_SET_USED;
			m = sc->tx_mbuf[sc->tx_ring.tx_bd_free];
			m_free(m);
			sc->tx_mbuf[sc->tx_ring.tx_bd_free] = 0;
			if_atsam_tx_bd_pos_update(&sc->tx_ring.tx_bd_free,
			    sc->tx_ring.length);
			if (cur->status.bm.bLastBuffer) {
				eof_needed = false;
			}
		} else {
			break;
		}
	}
}

/*
 * Prepare Ethernet frame to start transmission.
 */
static bool if_atsam_send_packet(if_atsam_softc *sc, struct mbuf *m)
{
	volatile sGmacTxDescriptor *cur;
	volatile sGmacTxDescriptor *start_packet_tx_bd = 0;
	int pos = 0;
	uint32_t tmp_val = 0;
	Gmac *pHw = sc->Gmac_inst.gGmacd.pHw;
	bool success;
	int csum_flags = m->m_pkthdr.csum_flags;

	if_atsam_tx_bd_cleanup(sc);
	/* Wait for interrupt in case no buffer descriptors are available */
	/* Wait for events */
	while (true) {
		if (if_atsam_ring_buffer_full(&sc->tx_ring)) {
			/* Setup the interrupts for TX completion and errors */
			GMAC_EnableIt(pHw, GMAC_INT_TX_BITS, 0);
			success = false;
			break;
		}

		/*
		 * Get current mbuf for data fill
		 */
		cur = &sc->tx_bd_base[sc->tx_ring.tx_bd_used];
		/* Set the transfer data */
		if (m->m_len) {
			uintptr_t cache_adjustment = mtod(m, uintptr_t) % 32;

			rtems_cache_flush_multiple_data_lines(
			  mtod(m, const char *) - cache_adjustment,
			  (size_t)(m->m_len + cache_adjustment));

			cur->addr = mtod(m, uint32_t);
			tmp_val = (uint32_t)m->m_len | GMAC_TX_SET_USED;
			if (sc->tx_ring.tx_bd_used == (sc->tx_ring.length - 1)) {
				tmp_val |= GMAC_TX_SET_WRAP;
			}
			if (pos == 0) {
				start_packet_tx_bd = cur;
			}
			sc->tx_mbuf[sc->tx_ring.tx_bd_used] = m;
			m = m->m_next;
			if_atsam_tx_bd_pos_update(&sc->tx_ring.tx_bd_used,
			    sc->tx_ring.length);
		} else {
			/* Discard empty mbufs */
			m = m_free(m);
		}

		/*
		 * Send out the buffer once the complete mbuf_chain has been
		 * processed
		 */
		if (m == NULL) {
			tmp_val |= GMAC_TX_SET_EOF;
			tmp_val &= ~GMAC_TX_SET_USED;
			if ((csum_flags & (CSUM_IP | CSUM_TCP | CSUM_UDP |
			    CSUM_TCP_IPV6 | CSUM_UDP_IPV6)) != 0) {
				start_packet_tx_bd->status.bm.bNoCRC = 0;
			} else {
				start_packet_tx_bd->status.bm.bNoCRC = 1;
			}
			_ARM_Data_synchronization_barrier();
			cur->status.val = tmp_val;
			start_packet_tx_bd->status.val &= ~GMAC_TX_SET_USED;
			_ARM_Data_synchronization_barrier();
			GMAC_TransmissionStart(pHw);
			success = true;
			break;
		} else {
			if (pos > 0) {
				tmp_val &= ~GMAC_TX_SET_USED;
			}
			pos++;
			cur->status.val = tmp_val;
		}
	}
	return success;
}


/*
 * Transmit daemon
 */
static void if_atsam_tx_daemon(void *arg)
{
	if_atsam_softc *sc = (if_atsam_softc *)arg;
	rtems_event_set events = 0;
	sGmacTxDescriptor *buffer_desc;
	int bd_number;
	void *tx_bd_base;
	struct mbuf *m;
	bool success;

	IF_ATSAM_LOCK(sc);

	Gmac *pHw = sc->Gmac_inst.gGmacd.pHw;
	struct ifnet *ifp = sc->ifp;

	GMAC_TransmitEnable(pHw, 0);

	/* Allocate memory space for priority queue descriptor list */
	tx_bd_base = rtems_cache_coherent_allocate(sizeof(sGmacTxDescriptor),
		GMAC_DESCRIPTOR_ALIGNMENT, 0);
	assert(tx_bd_base != NULL);

	buffer_desc = (sGmacTxDescriptor *)tx_bd_base;
	buffer_desc->addr = 0;
	buffer_desc->status.val = GMAC_TX_SET_USED | GMAC_TX_SET_WRAP;

	GMAC_SetTxQueue(pHw, (uint32_t)buffer_desc, 1);
	GMAC_SetTxQueue(pHw, (uint32_t)buffer_desc, 2);

	/* Allocate memory space for buffer descriptor list */
	tx_bd_base = rtems_cache_coherent_allocate(
		sc->amount_tx_buf * sizeof(sGmacTxDescriptor),
		GMAC_DESCRIPTOR_ALIGNMENT, 0);
	assert(tx_bd_base != NULL);
	buffer_desc = (sGmacTxDescriptor *)tx_bd_base;

	/* Create descriptor list and mark as empty */
	for (bd_number = 0; bd_number < sc->amount_tx_buf; bd_number++) {
		buffer_desc->addr = 0;
		buffer_desc->status.val = GMAC_TX_SET_USED;
		if (bd_number == (sc->amount_tx_buf - 1)) {
			buffer_desc->status.bm.bWrap = 1;
		} else {
			buffer_desc++;
		}
	}
	buffer_desc = (sGmacTxDescriptor *)tx_bd_base;

	/* Write Buffer Queue Base Address Register */
	GMAC_SetTxQueue(pHw, (uint32_t)buffer_desc, 0);

	/* Enable Transmission of data */
	GMAC_TransmitEnable(pHw, 1);

	/* Set variables in context */
	sc->tx_bd_base = tx_bd_base;

	while (true) {
		/* Wait for events */
		if_atsam_event_receive(sc,
		    ATSAMV7_ETH_START_TRANSMIT_EVENT |
		    ATSAMV7_ETH_TX_EVENT_INTERRUPT);
		//printf("TX Transmit Event received\n");

		/*
		 * Send packets till queue is empty
		 */
		while (true) {
			/*
			 * Get the mbuf chain to transmit
			 */
			if_atsam_tx_bd_cleanup(sc);
			IF_DEQUEUE(&ifp->if_snd, m);
			if (!m) {
				ifp->if_drv_flags &= ~IFF_DRV_OACTIVE;
				break;
			}
			success = if_atsam_send_packet(sc, m);
			if (!success){
				break;
			}
		}
	}
}


/*
 * Send packet (caller provides header).
 */
static void if_atsam_enet_start(struct ifnet *ifp)
{
	if_atsam_softc *sc = (if_atsam_softc *)ifp->if_softc;

	ifp->if_drv_flags |= IFF_DRV_OACTIVE;
	if_atsam_event_send(sc->tx_daemon_tid,
	    ATSAMV7_ETH_START_TRANSMIT_EVENT);
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


/*
 * Sets up the hardware and chooses the interface to be used
 */
static void if_atsam_init(void *arg)
{
	rtems_status_code status;

	if_atsam_softc *sc = (if_atsam_softc *)arg;
	struct ifnet *ifp = sc->ifp;
	uint32_t dmac_cfg = 0;
	uint32_t gmii_val = 0;

	if (ifp->if_flags & IFF_DRV_RUNNING) {
		return;
	}
	ifp->if_flags |= IFF_DRV_RUNNING;
	sc->interrupt_number = GMAC_IRQn;

	/* Enable Peripheral Clock */
	if ((PMC->PMC_PCSR1 & (1u << 7)) != (1u << 7)) {
		PMC->PMC_PCER1 = 1 << 7;
	}
	/* Setup interrupts */
	NVIC_ClearPendingIRQ(GMAC_IRQn);
	NVIC_EnableIRQ(GMAC_IRQn);

	/* Configuration of DMAC */
	dmac_cfg = (GMAC_DCFGR_DRBS(GMAC_RX_BUFFER_SIZE >> 6)) |
	    GMAC_DCFGR_RXBMS(3) | GMAC_DCFGR_TXPBMS | GMAC_DCFGR_FBLDO_INCR16 |
	    GMAC_DCFGR_TXCOEN;
	GMAC_SetDMAConfig(sc->Gmac_inst.gGmacd.pHw, dmac_cfg, 0);

	/* Enable hardware checksum offload for receive */
	sc->Gmac_inst.gGmacd.pHw->GMAC_NCFGR |= GMAC_NCFGR_RXCOEN;

	/* Shut down Transmit and Receive */
	GMAC_ReceiveEnable(sc->Gmac_inst.gGmacd.pHw, 0);
	GMAC_TransmitEnable(sc->Gmac_inst.gGmacd.pHw, 0);

	GMAC_StatisticsWriteEnable(sc->Gmac_inst.gGmacd.pHw, 1);

	/*
	 * Allocate mbuf pointers
	 */
	sc->rx_mbuf = malloc(sc->amount_rx_buf * sizeof *sc->rx_mbuf,
		M_TEMP, M_NOWAIT);
	sc->tx_mbuf = malloc(sc->amount_tx_buf * sizeof *sc->tx_mbuf,
		M_TEMP, M_NOWAIT);

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
	sc->rx_daemon_tid = rtems_bsdnet_newproc("SCrx", 4096,
		if_atsam_rx_daemon, sc);
	sc->tx_daemon_tid = rtems_bsdnet_newproc("SCtx", 4096,
		if_atsam_tx_daemon, sc);

	callout_reset(&sc->tick_ch, hz, if_atsam_tick, sc);

	ifp->if_drv_flags |= IFF_DRV_RUNNING;
}


/*
 * Stop the device
 */
static void if_atsam_stop(struct if_atsam_softc *sc)
{
	struct ifnet *ifp = sc->ifp;
	Gmac *pHw = sc->Gmac_inst.gGmacd.pHw;

	ifp->if_flags &= ~IFF_DRV_RUNNING;

	/* Disable MDIO interface and TX/RX */
	pHw->GMAC_NCR &= ~(GMAC_NCR_RXEN | GMAC_NCR_TXEN);
	pHw->GMAC_NCR &= ~GMAC_NCR_MPE;
}


static void
if_atsam_poll_hw_stats(struct if_atsam_softc *sc)
{
	uint64_t octets;
	Gmac *pHw = sc->Gmac_inst.gGmacd.pHw;

	octets = pHw->GMAC_OTLO;
	octets |= pHw->GMAC_OTHI << 32;
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
	octets |= pHw->GMAC_ORHI << 32;
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
	SYSCTL_ADD_UINT(ctx, child, OID_AUTO, "tx_complete_int",
	    CTLFLAG_RD, &sc->stats.tx_complete_int, 0,
	    "Tx complete interrupts");
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
	SYSCTL_ADD_UINT(ctx, child, OID_AUTO, "tx_interrupts",
	    CTLFLAG_RD, &sc->stats.tx_interrupts, 0,
	    "Tx interrupts");

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


/*
 * Calculates the index that is to be sent into the hash registers
 */
static void if_atsam_get_hash_index(uint64_t addr, uint32_t *val)
{
	uint64_t tmp_val;
	uint8_t i, j;
	uint64_t idx;
	int offset = 0;

	addr &= MAC_ADDR_MASK;

	for (i = 0; i < HASH_INDEX_AMOUNT; ++i) {
		tmp_val = 0;
		offset = 0;
		for (j = 0; j < HASH_ELEMENTS_PER_INDEX; j++) {
			idx = (addr >> (offset + i)) & MAC_IDX_MASK;
			tmp_val ^= idx;
			offset += HASH_INDEX_AMOUNT;
		}
		if (tmp_val > 0) {
			*val |= (1u << i);
		}
	}
}


/*
 * Dis/Enable promiscuous Mode
 */
static void if_atsam_promiscuous_mode(if_atsam_softc *sc, bool enable)
{
	Gmac *pHw = sc->Gmac_inst.gGmacd.pHw;

	if (enable) {
		pHw->GMAC_NCFGR |= GMAC_PROM_ENABLE;
	} else {
		pHw->GMAC_NCFGR &= ~GMAC_PROM_ENABLE;
	}
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
	bool prom_enable;
	struct mii_data *mii;

	switch (command) {
	case SIOCGIFMEDIA:
	case SIOCSIFMEDIA:
		rv = if_atsam_mediaioctl(sc, ifr, command);
		break;
	case SIOCSIFFLAGS:
		if (ifp->if_flags & IFF_UP) {
			if (!(ifp->if_drv_flags & IFF_DRV_RUNNING)) {
				if_atsam_init(sc);
			}
			prom_enable = ((ifp->if_flags & IFF_PROMISC) != 0);
			if_atsam_promiscuous_mode(sc, prom_enable);
		} else {
			if (ifp->if_drv_flags & IFF_DRV_RUNNING) {
				if_atsam_stop(sc);
			}
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
	char *unitName;
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

	sc->amount_rx_buf = RXBUF_COUNT;
	sc->amount_tx_buf = TXBUF_COUNT;

	sc->tx_ring.tx_bd_used = 0;
	sc->tx_ring.tx_bd_free = 0;
	sc->tx_ring.length = sc->amount_tx_buf;

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
	ifp->if_init = if_atsam_init;
	ifp->if_ioctl = if_atsam_ioctl;
	ifp->if_start = if_atsam_enet_start;
	ifp->if_flags = IFF_BROADCAST | IFF_SIMPLEX;
	ifp->if_capabilities |= IFCAP_HWCSUM | IFCAP_HWCSUM_IPV6 |
	    IFCAP_VLAN_HWCSUM;
	ifp->if_hwassist = CSUM_IP | CSUM_IP_UDP | CSUM_IP_TCP |
	    CSUM_IP6_UDP | CSUM_IP6_TCP;
	IFQ_SET_MAXLEN(&ifp->if_snd, TXBUF_COUNT - 1);
	ifp->if_snd.ifq_drv_maxlen = TXBUF_COUNT - 1;
	IFQ_SET_READY(&ifp->if_snd);

	/*
	 * Attach the interface
	 */
	ether_ifattach(ifp, eaddr);

	if_atsam_add_sysctls(dev);

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
