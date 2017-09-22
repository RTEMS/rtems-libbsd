/*
 * Copyright (c) 2016 embedded brains GmbH.  All rights reserved.
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

#include <rtems/rtems_mii_ioctl.h>
#include <rtems/bsd/local/miibus_if.h>

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

#define SIO_RTEMS_SHOW_STATS _IO('i', 250)

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

	/*
	 * mii bus
	 */
	device_t miibus;
	uint8_t link_speed;
	uint8_t link_duplex;
	struct callout mii_tick_ch;

	/*
	 * Statistics
	 */
	unsigned rx_overrun_errors;
	unsigned rx_interrupts;
	unsigned tx_complete_int;
	unsigned tx_tur_errors;
	unsigned tx_rlex_errors;
	unsigned tx_tfc_errors;
	unsigned tx_hresp_errors;
	unsigned tx_interrupts;
} if_atsam_softc;

static void if_atsam_watchdog(void *arg);

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
		++sc->rx_overrun_errors;
		rx_event = ATSAMV7_ETH_RX_EVENT_INTERRUPT;
	}
	if ((irq_status_val & GMAC_IER_RCOMP) != 0) {
		rx_event = ATSAMV7_ETH_RX_EVENT_INTERRUPT;
	}
	/* Send events to receive task and switch off rx interrupts */
	if (rx_event != 0) {
		++sc->rx_interrupts;
		/* Erase the interrupts for RX completion and errors */
		GMAC_DisableIt(pHw, GMAC_IER_RCOMP | GMAC_IER_ROVR, 0);
		(void)if_atsam_event_send(sc->rx_daemon_tid, rx_event);
	}
	if ((irq_status_val & GMAC_IER_TUR) != 0) {
		++sc->tx_tur_errors;
		tx_event = ATSAMV7_ETH_TX_EVENT_INTERRUPT;
	}
	if ((irq_status_val & GMAC_IER_RLEX) != 0) {
		++sc->tx_rlex_errors;
		tx_event = ATSAMV7_ETH_TX_EVENT_INTERRUPT;
	}
	if ((irq_status_val & GMAC_IER_TFC) != 0) {
		++sc->tx_tfc_errors;
		tx_event = ATSAMV7_ETH_TX_EVENT_INTERRUPT;
	}
	if ((irq_status_val & GMAC_IER_HRESP) != 0) {
		++sc->tx_hresp_errors;
		tx_event = ATSAMV7_ETH_TX_EVENT_INTERRUPT;
	}
	if ((irq_status_val & GMAC_IER_TCOMP) != 0) {
		++sc->tx_complete_int;
		tx_event = ATSAMV7_ETH_TX_EVENT_INTERRUPT;
	}
	/* Send events to transmit task and switch off tx interrupts */
	if (tx_event != 0) {
		++sc->tx_interrupts;
		/* Erase the interrupts for TX completion and errors */
		GMAC_DisableIt(pHw, GMAC_INT_TX_BITS, 0);
		(void)if_atsam_event_send(sc->tx_daemon_tid, tx_event);
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
	int frame_len;
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
					frame_len = (int)
					    (buffer_desc->status.bm.len);

					/* Update mbuf */
					m->m_data = mtod(m, char*)+ETHER_ALIGN;
					m->m_len = frame_len;
					m->m_pkthdr.len = frame_len;
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


static void if_atsam_miibus_statchg(device_t dev)
{
	uint8_t link_speed = GMAC_SPEED_100M;
	uint8_t link_duplex = GMAC_DUPLEX_FULL;
	if_atsam_softc *sc = device_get_softc(dev);
	struct mii_data *mii = device_get_softc(sc->miibus);

	Gmac *pHw = sc->Gmac_inst.gGmacd.pHw;

	if (IFM_OPTIONS(mii->mii_media_active) & IFM_FDX) {
		link_duplex = GMAC_DUPLEX_FULL;
	} else {
		link_duplex = GMAC_DUPLEX_HALF;
	}

	switch (IFM_SUBTYPE(mii->mii_media_active)) {
	case IFM_10_T:
		link_speed = GMAC_SPEED_10M;
		break;
	case IFM_100_TX:
		link_speed = GMAC_SPEED_100M;
		break;
	case IFM_1000_T:
		link_speed = GMAC_SPEED_1000M;
		break;
	default:
		/* FIXME: What to do in that case? */
		break;
	}

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
	if (sc->miibus == NULL)
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
	if (sc->miibus == NULL)
		return;

	mii = device_get_softc(sc->miibus);
	mii_pollstat(mii);
	ifmr->ifm_active = mii->mii_media_active;
	ifmr->ifm_status = mii->mii_media_status;
}


static void
if_atsam_mii_tick(void *context)
{
	if_atsam_softc *sc = context;

	if (sc->miibus == NULL)
		return;

	IF_ATSAM_UNLOCK(sc);

	mii_tick(device_get_softc(sc->miibus));
	callout_reset(&sc->mii_tick_ch, hz, if_atsam_mii_tick, sc);
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

	/* Disable Watchdog */
	WDT_Disable(WDT);

	/* Enable Peripheral Clock */
	if ((PMC->PMC_PCSR1 & (1u << 7)) != (1u << 7)) {
		PMC->PMC_PCER1 = 1 << 7;
	}
	/* Setup interrupts */
	NVIC_ClearPendingIRQ(GMAC_IRQn);
	NVIC_EnableIRQ(GMAC_IRQn);

	/* Configuration of DMAC */
	dmac_cfg = (GMAC_DCFGR_DRBS(GMAC_RX_BUFFER_SIZE >> 6)) |
	    GMAC_DCFGR_RXBMS(3) | GMAC_DCFGR_TXPBMS | GMAC_DCFGR_FBLDO_INCR16;
	GMAC_SetDMAConfig(sc->Gmac_inst.gGmacd.pHw, dmac_cfg, 0);

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

	callout_reset(&sc->mii_tick_ch, hz, if_atsam_mii_tick, sc);

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


/*
 * Show interface statistics
 */
static void if_atsam_stats(struct if_atsam_softc *sc)
{
	int eno = EIO;
	Gmac *pHw;

	pHw = sc->Gmac_inst.gGmacd.pHw;

	printf("\n** Context Statistics **\n");
	printf("Rx interrupts: %u\n", sc->rx_interrupts);
	printf("Tx interrupts: %u\n", sc->tx_interrupts);
	printf("Error Tur Tx interrupts: %u\n\n", sc->tx_tur_errors);
	printf("Error Rlex Tx interrupts: %u\n\n", sc->tx_rlex_errors);
	printf("Error Tfc Tx interrupts: %u\n\n", sc->tx_tfc_errors);
	printf("Error Hresp Tx interrupts: %u\n\n", sc->tx_hresp_errors);
	printf("Tx complete interrupts: %u\n\n", sc->tx_complete_int);
	printf("\n** Statistics **\n");
	printf("Octets Transmitted Low: %lu\n", pHw->GMAC_OTLO);
	printf("Octets Transmitted High: %lu\n", pHw->GMAC_OTHI);
	printf("Frames Transmitted: %lu\n", pHw->GMAC_FT);
	printf("Broadcast Frames Transmitted: %lu\n", pHw->GMAC_BCFT);
	printf("Multicast Frames Transmitted: %lu\n", pHw->GMAC_MFT);
	printf("Pause Frames Transmitted: %lu\n", pHw->GMAC_PFT);
	printf("64 Byte Frames Transmitted: %lu\n", pHw->GMAC_BFT64);
	printf("65 to 127 Byte Frames Transmitted: %lu\n", pHw->GMAC_TBFT127);
	printf("128 to 255 Byte Frames Transmitted: %lu\n", pHw->GMAC_TBFR255);
	printf("256 to 511 Byte Frames Transmitted: %lu\n", pHw->GMAC_TBFT511);
	printf("512 to 1023 Byte Frames Transmitted: %lu\n",
	    pHw->GMAC_TBFT1023);
	printf("1024 to 1518 Byte Frames Transmitted: %lu\n",
	    pHw->GMAC_TBFT1518);
	printf("Greater Than 1518 Byte Frames Transmitted: %lu\n",
	    pHw->GMAC_GTBFT1518);
	printf("Transmit Underruns: %lu\n", pHw->GMAC_TUR);
	printf("Single Collision Frames: %lu\n", pHw->GMAC_SCF);
	printf("Multiple Collision Frames: %lu\n", pHw->GMAC_MCF);
	printf("Excessive Collisions: %lu\n", pHw->GMAC_EC);
	printf("Late Collisions: %lu\n", pHw->GMAC_LC);
	printf("Deferred Transmission Frames: %lu\n", pHw->GMAC_DTF);
	printf("Carrier Sense Errors: %lu\n", pHw->GMAC_CSE);
	printf("Octets Received Low: %lu\n", pHw->GMAC_ORLO);
	printf("Octets Received High: %lu\n", pHw->GMAC_ORHI);
	printf("Frames Received: %lu\n", pHw->GMAC_FR);
	printf("Broadcast Frames Received: %lu\n", pHw->GMAC_BCFR);
	printf("Multicast Frames Received: %lu\n", pHw->GMAC_MFR);
	printf("Pause Frames Received: %lu\n", pHw->GMAC_PFR);
	printf("64 Byte Frames Received: %lu\n", pHw->GMAC_BFR64);
	printf("65 to 127 Byte Frames Received: %lu\n", pHw->GMAC_TBFR127);
	printf("128 to 255 Byte Frames Received: %lu\n", pHw->GMAC_TBFR255);
	printf("256 to 511 Byte Frames Received: %lu\n", pHw->GMAC_TBFR511);
	printf("512 to 1023 Byte Frames Received: %lu\n", pHw->GMAC_TBFR1023);
	printf("1024 to 1518 Byte Frames Received: %lu\n", pHw->GMAC_TBFR1518);
	printf("1519 to Maximum Byte Frames Received: %lu\n",
	    pHw->GMAC_TBFR1518);
	printf("Undersize Frames Received: %lu\n", pHw->GMAC_UFR);
	printf("Oversize Frames Received: %lu\n", pHw->GMAC_OFR);
	printf("Jabbers Received: %lu\n", pHw->GMAC_JR);
	printf("Frame Check Sequence Errors: %lu\n", pHw->GMAC_FCSE);
	printf("Length Field Frame Errors: %lu\n", pHw->GMAC_LFFE);
	printf("Receive Symbol Errors: %lu\n", pHw->GMAC_RSE);
	printf("Alignment Errors: %lu\n", pHw->GMAC_AE);
	printf("Receive Resource Errors: %lu\n", pHw->GMAC_RRE);
	printf("Receive Overrun: %lu\n", pHw->GMAC_ROE);
	printf("IP Header Checksum Errors: %lu\n", pHw->GMAC_IHCE);
	printf("TCP Checksum Errors: %lu\n", pHw->GMAC_TCE);
	printf("UDP Checksum Errors: %lu\n", pHw->GMAC_UCE);
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
	struct mii_data *mii;

	if (sc->miibus == NULL)
		return (EINVAL);

	mii = device_get_softc(sc->miibus);
	return (ifmedia_ioctl(sc->ifp, ifr, &mii->mii_media, command));
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
	case SIO_RTEMS_SHOW_STATS:
		if_atsam_stats(sc);
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
	callout_init_mtx(&sc->mii_tick_ch, &sc->mtx, CALLOUT_RETURNUNLOCKED);
	mii_attach(dev, &sc->miibus, ifp,
	    if_atsam_mii_ifmedia_upd, if_atsam_mii_ifmedia_sts, BMSR_DEFCAPMASK,
	    MDIO_PHY, MII_OFFSET_ANY, 0);

	/*
	 * Set up network interface values
	 */
	ifp->if_softc = sc;
	if_initname(ifp, device_get_name(dev), device_get_unit(dev));
	ifp->if_init = if_atsam_init;
	ifp->if_ioctl = if_atsam_ioctl;
	ifp->if_start = if_atsam_enet_start;
	ifp->if_flags = IFF_BROADCAST | IFF_SIMPLEX;
	IFQ_SET_MAXLEN(&ifp->if_snd, TXBUF_COUNT - 1);
	ifp->if_snd.ifq_drv_maxlen = TXBUF_COUNT - 1;
	IFQ_SET_READY(&ifp->if_snd);

	/*
	 * Attach the interface
	 */
	ether_ifattach(ifp, eaddr);

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
MODULE_DEPEND(if_atsam, miibus, 1, 1, 1);
MODULE_DEPEND(if_atsam, nexus, 1, 1, 1);
MODULE_DEPEND(if_atsam, ether, 1, 1, 1);
DRIVER_MODULE(miibus, if_atsam, miibus_driver, miibus_devclass, NULL, NULL);

#endif /* LIBBSP_ARM_ATSAM_BSP_H */
