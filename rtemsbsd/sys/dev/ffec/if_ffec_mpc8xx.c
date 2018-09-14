/*===============================================================*\
| Project: RTEMS TQM8xx BSP                                       |
+-----------------------------------------------------------------+
| This file has been adapted to MPC8xx by                         |
|    Thomas Doerfler <Thomas.Doerfler@embedded-brains.de>         |
|                    Copyright (c) 2008                           |
|                    Embedded Brains GmbH                         |
|                    Obere Lagerstr. 30                           |
|                    D-82178 Puchheim                             |
|                    Germany                                      |
|                    rtems@embedded-brains.de                     |
|                                                                 |
| See the other copyright notice below for the original parts.    |
+-----------------------------------------------------------------+
| The license and distribution terms for this file may be         |
| found in the file LICENSE in this distribution or at            |
|                                                                 |
| http://www.rtems.org/license/LICENSE.                           |
|                                                                 |
+-----------------------------------------------------------------+
| this file contains the console driver                           |
\*===============================================================*/
/* derived from: */
/*
 * RTEMS/TCPIP driver for MPC8xx Ethernet
 *
 * split into separate driver files for SCC and FEC by
 * Thomas Doerfler <Thomas.Doerfler@embedded-brains.de>
 *
 *  Modified for MPC860 by Jay Monkman (jmonkman@frasca.com)
 *
 *  This supports Ethernet on either SCC1 or the FEC of the MPC860T.
 *  Right now, we only do 10 Mbps, even with the FEC. The function
 *  rtems_enet_driver_attach determines which one to use. Currently,
 *  only one may be used at a time.
 *
 *  Based on the MC68360 network driver by
 *  W. Eric Norum
 *  Saskatchewan Accelerator Laboratory
 *  University of Saskatchewan
 *  Saskatoon, Saskatchewan, CANADA
 *  eric@skatter.usask.ca
 *
 *  This supports ethernet on SCC1. Right now, we only do 10 Mbps.
 *
 *  Modifications by Darlene Stewart <Darlene.Stewart@iit.nrc.ca>
 *  and Charles-Antoine Gauthier <charles.gauthier@iit.nrc.ca>
 *  Copyright (c) 1999, National Research Council of Canada
 */

#include <machine/rtems-bsd-kernel-space.h>

#include <bsp.h>

#ifdef LIBBSP_POWERPC_TQM8XX_BSP_H

#include <stdio.h>

#include <sys/param.h>
#include <sys/types.h>
#include <sys/mbuf.h>
#include <sys/malloc.h>
#include <sys/kernel.h>
#include <sys/module.h>
#include <sys/socket.h>
#include <sys/sockio.h>

#include <sys/bus.h>
#include <machine/bus.h>

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

#include <bsp/irq.h>
#include <rtems/bsd/bsd.h>

/* FIXME */
rtems_id
rtems_bsdnet_newproc (char *name, int stacksize, void(*entry)(void *), void *arg);
#define SIO_RTEMS_SHOW_STATS _IO('i', 250)

/*
 * Number of interfaces supported by this driver
 */
#define NIFACES 1

/*
 * Default number of buffer descriptors set aside for this driver.
 * The number of transmit buffer descriptors has to be quite large
 * since a single frame often uses four or more buffer descriptors.
 */
#define RX_BUF_COUNT     32
#define TX_BUF_COUNT     8
#define TX_BD_PER_BUF    4

#define INET_ADDR_MAX_BUF_SIZE (sizeof "255.255.255.255")

/*
 * RTEMS event used by interrupt handler to signal daemons.
 * This must *not* be the same event used by the TCP/IP task synchronization.
 */
#define INTERRUPT_EVENT RTEMS_EVENT_1

/*
 * RTEMS event used to start transmit daemon.
 * This must not be the same as INTERRUPT_EVENT.
 */
#define START_TRANSMIT_EVENT RTEMS_EVENT_2

/*
 * Receive buffer size -- Allow for a full ethernet packet plus CRC (1518).
 * Round off to nearest multiple of RBUF_ALIGN.
 */
#define MAX_MTU_SIZE	1518
#define RBUF_ALIGN		4
#define RBUF_SIZE       ((MAX_MTU_SIZE + RBUF_ALIGN) & ~RBUF_ALIGN)

#if (MCLBYTES < RBUF_SIZE)
# error "Driver must have MCLBYTES > RBUF_SIZE"
#endif

#define FEC_WATCHDOG_TIMEOUT 5 /* check media every 5 seconds */
/*
 * Per-device data
 */
struct m8xx_fec_enet_struct {
  device_t                dev;
  struct ifnet            *ifp;
  struct mtx              mtx;
  struct mbuf             **rxMbuf;
  struct mbuf             **txMbuf;
  int                     rxBdCount;
  int                     txBdCount;
  int                     txBdHead;
  int                     txBdTail;
  int                     txBdActiveCount;
  struct callout          watchdogCallout;
  device_t                miibus;
  struct mii_data         *mii_softc;
  m8xxBufferDescriptor_t  *rxBdBase;
  m8xxBufferDescriptor_t  *txBdBase;
  rtems_id                rxDaemonTid;
  rtems_id                txDaemonTid;
  int                     if_flags;

  /*
   * MDIO/Phy info
   */
  int phy_default;
  /*
   * Statistics
   */
  unsigned long   rxInterrupts;
  unsigned long   rxNotFirst;
  unsigned long   rxNotLast;
  unsigned long   rxGiant;
  unsigned long   rxNonOctet;
  unsigned long   rxRunt;
  unsigned long   rxBadCRC;
  unsigned long   rxOverrun;
  unsigned long   rxCollision;

  unsigned long   txInterrupts;
  unsigned long   txDeferred;
  unsigned long   txHeartbeat;
  unsigned long   txLateCollision;
  unsigned long   txRetryLimit;
  unsigned long   txUnderrun;
  unsigned long   txLostCarrier;
  unsigned long   txRawWait;
};

#define FEC_LOCK(sc) mtx_lock(&(sc)->mtx)

#define FEC_UNLOCK(sc) mtx_unlock(&(sc)->mtx)

#define FEC_EVENT RTEMS_EVENT_0

static void fec_send_event(rtems_id task, rtems_event_set out)
{
  rtems_event_send(task, out);
}

static void fec_wait_for_event(struct m8xx_fec_enet_struct *sc,
			       rtems_event_set in)
{
  rtems_event_set out;

  FEC_UNLOCK(sc);
  rtems_event_receive(in, RTEMS_EVENT_ANY | RTEMS_WAIT, RTEMS_NO_TIMEOUT,
		      &out);
  FEC_LOCK(sc);
}

/***************************************************************************\
|  MII Management access functions                                          |
\***************************************************************************/

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static void fec_mdio_init
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   initialize the MII interface                                            |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 struct m8xx_fec_enet_struct *sc     /* control structure                */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    <none>                                                                 |
\*=========================================================================*/
{

  /* Set FEC registers for MDIO communication */
  /*
   * set clock divider
   */
  m8xx.fec.mii_speed = BSP_bus_frequency / 1250000 / 2 + 1;
}

static int
fec_miibus_read_reg(device_t dev, int phy, int reg)
{
  struct m8xx_fec_enet_struct *sc;

  sc = device_get_softc(dev);

  /*
   * make sure we work with a valid phy
   */
  if (phy == -1) {
    /*
     * set default phy number: 0
     */
    phy = sc->phy_default;
  }
  if ( (phy < 0) || (phy > 31)) {
    /*
     * invalid phy number
     */
    return 0;
  }
  /*
   * clear MII transfer event bit
   */
  m8xx.fec.ievent = M8xx_FEC_IEVENT_MII;
  /*
   * set access command, data, start transfer
   */
  m8xx.fec.mii_data = (M8xx_FEC_MII_DATA_ST    |
		       M8xx_FEC_MII_DATA_OP_RD |
		       M8xx_FEC_MII_DATA_PHYAD(phy) |
		       M8xx_FEC_MII_DATA_PHYRA(reg) |
		       M8xx_FEC_MII_DATA_TA |
		       M8xx_FEC_MII_DATA_WDATA(0));

  /*
   * wait for cycle to terminate
   */
  do {
    rtems_task_wake_after(RTEMS_YIELD_PROCESSOR);
  }  while (0 == (m8xx.fec.ievent & M8xx_FEC_IEVENT_MII));

  /*
   * fetch read data, if available
   */
  return M8xx_FEC_MII_DATA_RDATA(m8xx.fec.mii_data);
}

static int
fec_miibus_write_reg(device_t dev, int phy, int reg, int val)
{
  struct m8xx_fec_enet_struct *sc;

  sc = device_get_softc(dev);

  /*
   * make sure we work with a valid phy
   */
  if (phy == -1) {
    /*
     * set default phy number: 0
     */
    phy = sc->phy_default;
  }
  if ( (phy < 0) || (phy > 31)) {
    /*
     * invalid phy number
     */
    return EINVAL;
  }
  /*
   * clear MII transfer event bit
   */
  m8xx.fec.ievent = M8xx_FEC_IEVENT_MII;
  /*
   * set access command, data, start transfer
   */
  m8xx.fec.mii_data = (M8xx_FEC_MII_DATA_ST    |
		       M8xx_FEC_MII_DATA_OP_WR |
		       M8xx_FEC_MII_DATA_PHYAD(phy) |
		       M8xx_FEC_MII_DATA_PHYRA(reg) |
		       M8xx_FEC_MII_DATA_TA |
		       M8xx_FEC_MII_DATA_WDATA(val));

  /*
   * wait for cycle to terminate
   */
  do {
    rtems_task_wake_after(RTEMS_YIELD_PROCESSOR);
  }  while (0 == (m8xx.fec.ievent & M8xx_FEC_IEVENT_MII));

  return 0;
}

/*
 * FEC interrupt handler
 */
static void m8xx_fec_interrupt_handler (void *arg)
{
  struct m8xx_fec_enet_struct *sc = arg;

  /*
   * Frame received?
   */
  if (m8xx.fec.ievent & M8xx_FEC_IEVENT_RFINT) {
    m8xx.fec.ievent = M8xx_FEC_IEVENT_RFINT;
    sc->rxInterrupts++;
    fec_send_event (sc->rxDaemonTid, INTERRUPT_EVENT);
  }

  /*
   * Buffer transmitted or transmitter error?
   */
  if (m8xx.fec.ievent & M8xx_FEC_IEVENT_TFINT) {
    m8xx.fec.ievent = M8xx_FEC_IEVENT_TFINT;
    sc->txInterrupts++;
    fec_send_event (sc->txDaemonTid, INTERRUPT_EVENT);
  }
}

static void
m8xx_fec_initialize_hardware (struct m8xx_fec_enet_struct *sc)
{
  int i;
  unsigned char *hwaddr;
  rtems_status_code status;

  /*
   * Issue reset to FEC
   */
  m8xx.fec.ecntrl = M8xx_FEC_ECNTRL_RESET;

  /*
   * Put ethernet transciever in reset
   */
  m8xx.pgcra |= 0x80;

  /*
   * Configure I/O ports
   */
  m8xx.pdpar = 0x1fff;
  m8xx.pddir = 0x1fff;

  /*
   * Take ethernet transciever out of reset
   */
  m8xx.pgcra &= ~0x80;

  /*
   * Set SIU interrupt level to LVL2
   *
   */
  m8xx.fec.ivec = ((((unsigned) BSP_FAST_ETHERNET_CTRL)/2) << 29);

  /*
   * Set the TX and RX fifo sizes. For now, we'll split it evenly
   */
  /* If you uncomment these, the FEC will not work right.
     m8xx.fec.r_fstart = ((m8xx.fec.r_bound & 0x3ff) >> 2) & 0x3ff;
     m8xx.fec.x_fstart = 0;
  */

  /*
   * Set our physical address
   */
  hwaddr = IF_LLADDR(sc->ifp);

  m8xx.fec.addr_low = (hwaddr[0] << 24) | (hwaddr[1] << 16) |
    (hwaddr[2] << 8)  | (hwaddr[3] << 0);
  m8xx.fec.addr_high = (hwaddr[4] << 24) | (hwaddr[5] << 16);

  /*
   * Clear the hash table
   */
  m8xx.fec.hash_table_high = 0;
  m8xx.fec.hash_table_low  = 0;

  /*
   * Set up receive buffer size
   */
  m8xx.fec.r_buf_size = 0x5f0; /* set to 1520 */

  /*
   * Allocate mbuf pointers
   */
  sc->rxMbuf = malloc (sc->rxBdCount * sizeof *sc->rxMbuf,
                       M_TEMP, M_NOWAIT);
  sc->txMbuf = malloc (sc->txBdCount * sizeof *sc->txMbuf,
                       M_TEMP, M_NOWAIT);
  if (!sc->rxMbuf || !sc->txMbuf)
    rtems_panic ("No memory for mbuf pointers");

  /*
   * Set receiver and transmitter buffer descriptor bases
   */
  sc->rxBdBase = m8xx_bd_allocate(sc->rxBdCount);
  sc->txBdBase = m8xx_bd_allocate(sc->txBdCount);
  m8xx.fec.r_des_start = (int)sc->rxBdBase;
  m8xx.fec.x_des_start = (int)sc->txBdBase;

  /*
   * Set up Receive Control Register:
   *   Not promiscuous mode
   *   MII mode
   *   Half duplex
   *   No loopback
   */
  m8xx.fec.r_cntrl = M8xx_FEC_R_CNTRL_MII_MODE | M8xx_FEC_R_CNTRL_DRT;

  /*
   * Set up Transmit Control Register:
   *   Full duplex
   *   No heartbeat
   */
  m8xx.fec.x_cntrl = M8xx_FEC_X_CNTRL_FDEN;

  /*
   * Set up DMA function code:
   *   Big-endian
   *   DMA functino code = 0
   */
  m8xx.fec.fun_code = 0x78000000;

  /*
   * Initialize SDMA configuration register
   *   SDMA ignores FRZ
   *   FEC not aggressive
   *   FEC arbitration ID = 0 => U-bus arbitration = 6
   *   RISC arbitration ID = 1 => U-bus arbitration = 5
   */
  m8xx.sdcr = M8xx_SDCR_RAID_5;

  /*
   * Set up receive buffer descriptors
   */
  for (i = 0 ; i < sc->rxBdCount ; i++)
    (sc->rxBdBase + i)->status = 0;

  /*
   * Set up transmit buffer descriptors
   */
  for (i = 0 ; i < sc->txBdCount ; i++) {
    (sc->txBdBase + i)->status = 0;
    sc->txMbuf[i] = NULL;
  }
  sc->txBdHead = sc->txBdTail = 0;
  sc->txBdActiveCount = 0;

  /* Set pin multiplexing */
  m8xx.fec.ecntrl = M8xx_FEC_ECNTRL_FEC_PINMUX;

  /*
   * Mask all FEC interrupts and clear events
   */
  m8xx.fec.imask = M8xx_FEC_IEVENT_TFINT |
    M8xx_FEC_IEVENT_RFINT;
  m8xx.fec.ievent = ~0;

  /*
   * Set up interrupts
   */
  status = rtems_interrupt_handler_install(BSP_FAST_ETHERNET_CTRL, "FEC",
					   RTEMS_INTERRUPT_UNIQUE,
					   m8xx_fec_interrupt_handler, sc);
  if (status != RTEMS_SUCCESSFUL)
    rtems_panic ("Can't attach M860 FEC interrupt handler\n");

}
static void fec_rxDaemon (void *arg)
{
  struct m8xx_fec_enet_struct *sc = (struct m8xx_fec_enet_struct *)arg;
  struct ifnet *ifp = sc->ifp;
  struct mbuf *m;
  uint16_t   status;
  m8xxBufferDescriptor_t *rxBd;
  int rxBdIndex;

  FEC_LOCK(sc);

  /*
   * Allocate space for incoming packets and start reception
   */
  for (rxBdIndex = 0 ; ;) {
    rxBd = sc->rxBdBase + rxBdIndex;
    m = m_getcl(M_WAITOK, MT_DATA, M_PKTHDR);
    m->m_pkthdr.rcvif = ifp;
    sc->rxMbuf[rxBdIndex] = m;
    rxBd->buffer = mtod (m, void *);
    rxBd->status = M8xx_BD_EMPTY;
    m8xx.fec.r_des_active = 0x1000000;
    if (++rxBdIndex == sc->rxBdCount) {
      rxBd->status |= M8xx_BD_WRAP;
      break;
    }
  }

  /*
   * Input packet handling loop
   */
  rxBdIndex = 0;
  for (;;) {
    rxBd = sc->rxBdBase + rxBdIndex;

    /*
     * Wait for packet if there's not one ready
     */
    if ((status = rxBd->status) & M8xx_BD_EMPTY) {
      /*
       * Clear old events
       */
      m8xx.fec.ievent = M8xx_FEC_IEVENT_RFINT;

      /*
       * Wait for packet
       * Note that the buffer descriptor is checked
       * *before* the event wait -- this catches the
       * possibility that a packet arrived between the
       * `if' above, and the clearing of the event register.
       */
      while ((status = rxBd->status) & M8xx_BD_EMPTY) {
        /*
         * Unmask RXF (Full frame received) event
         */
        m8xx.fec.ievent |= M8xx_FEC_IEVENT_RFINT;

        fec_wait_for_event (sc, INTERRUPT_EVENT);
      }
    }

    /*
     * Check that packet is valid
     */
    if (status & M8xx_BD_LAST) {
      /*
       * Pass the packet up the chain.
       * FIXME: Packet filtering hook could be done here.
       */

      /*
       * Invalidate the buffer for this descriptor
       */
      rtems_cache_invalidate_multiple_data_lines((const void *)rxBd->buffer, rxBd->length);

      m = sc->rxMbuf[rxBdIndex];
      m->m_len = m->m_pkthdr.len = rxBd->length - sizeof(uint32_t);
      FEC_UNLOCK(sc);
      (*sc->ifp->if_input)(sc->ifp, m);
      FEC_LOCK(sc);

      /*
       * Allocate a new mbuf
       */
      m = m_getcl(M_WAITOK, MT_DATA, M_PKTHDR);
      m->m_pkthdr.rcvif = ifp;
      sc->rxMbuf[rxBdIndex] = m;
      rxBd->buffer = mtod (m, void *);
    }
    else {
      /*
       * Something went wrong with the reception
       */
      if (!(status & M8xx_BD_LAST))
        sc->rxNotLast++;
      if (status & M8xx_BD_LONG)
        sc->rxGiant++;
      if (status & M8xx_BD_NONALIGNED)
        sc->rxNonOctet++;
      if (status & M8xx_BD_SHORT)
        sc->rxRunt++;
      if (status & M8xx_BD_CRC_ERROR)
        sc->rxBadCRC++;
      if (status & M8xx_BD_OVERRUN)
        sc->rxOverrun++;
      if (status & M8xx_BD_COLLISION)
        sc->rxCollision++;
    }
    /*
     * Reenable the buffer descriptor
     */
    rxBd->status = (status & M8xx_BD_WRAP) |
      M8xx_BD_EMPTY;
    m8xx.fec.r_des_active = 0x1000000;
    /*
     * Move to next buffer descriptor
     */
    if (++rxBdIndex == sc->rxBdCount)
      rxBdIndex = 0;
  }
}

/*
 * Soak up buffer descriptors that have been sent.
 * Note that a buffer descriptor can't be retired as soon as it becomes
 * ready. The MPC860 manual (MPC860UM/AD 07/98 Rev.1) and the MPC821
 * manual state that, "If an Ethernet frame is made up of multiple
 * buffers, the user should not reuse the first buffer descriptor until
 * the last buffer descriptor of the frame has had its ready bit cleared
 * by the CPM".
 */
static void
m8xx_fec_Enet_retire_tx_bd (struct m8xx_fec_enet_struct *sc)
{
  uint16_t   status;
  int i;
  int nRetired;
  struct mbuf *m;

  i = sc->txBdTail;
  nRetired = 0;
  while ((sc->txBdActiveCount != 0)
	 &&  (((status = (sc->txBdBase + i)->status) & M8xx_BD_READY) == 0)) {
    /*
     * See if anything went wrong
     */
    if (status & (M8xx_BD_DEFER |
                  M8xx_BD_HEARTBEAT |
                  M8xx_BD_LATE_COLLISION |
                  M8xx_BD_RETRY_LIMIT |
                  M8xx_BD_UNDERRUN |
                  M8xx_BD_CARRIER_LOST)) {
      /*
       * Check for errors which stop the transmitter.
       */
      if (status & (M8xx_BD_LATE_COLLISION |
                    M8xx_BD_RETRY_LIMIT |
                    M8xx_BD_UNDERRUN)) {
        if (status & M8xx_BD_LATE_COLLISION)
          sc->txLateCollision++;
        if (status & M8xx_BD_RETRY_LIMIT)
          sc->txRetryLimit++;
        if (status & M8xx_BD_UNDERRUN)
          sc->txUnderrun++;

      }
      if (status & M8xx_BD_DEFER)
        sc->txDeferred++;
      if (status & M8xx_BD_HEARTBEAT)
        sc->txHeartbeat++;
      if (status & M8xx_BD_CARRIER_LOST)
        sc->txLostCarrier++;
    }
    nRetired++;
    if (status & M8xx_BD_LAST) {
      /*
       * A full frame has been transmitted.
       * Free all the associated buffer descriptors.
       */
      sc->txBdActiveCount -= nRetired;
      while (nRetired) {
        nRetired--;
        m = sc->txMbuf[sc->txBdTail];
        m_free(m);
        if (++sc->txBdTail == sc->txBdCount)
          sc->txBdTail = 0;
      }
    }
    if (++i == sc->txBdCount)
      i = 0;
  }
}

static void fec_sendpacket (struct m8xx_fec_enet_struct *sc, struct mbuf *m)
{
  volatile m8xxBufferDescriptor_t *firstTxBd, *txBd;
  /*  struct mbuf *l = NULL; */
  uint16_t   status;
  int nAdded;

  /*
   * Free up buffer descriptors
   */
  m8xx_fec_Enet_retire_tx_bd (sc);

  /*
   * Set up the transmit buffer descriptors.
   * No need to pad out short packets since the
   * hardware takes care of that automatically.
   * No need to copy the packet to a contiguous buffer
   * since the hardware is capable of scatter/gather DMA.
   */
  nAdded = 0;
  txBd = firstTxBd = sc->txBdBase + sc->txBdHead;
  for (;;) {
    /*
     * Wait for buffer descriptor to become available.
     */
    if ((sc->txBdActiveCount + nAdded) == sc->txBdCount) {
      /*
       * Clear old events
       */
      m8xx.fec.ievent = M8xx_FEC_IEVENT_TFINT;

      /*
       * Wait for buffer descriptor to become available.
       * Note that the buffer descriptors are checked
       * *before* * entering the wait loop -- this catches
       * the possibility that a buffer descriptor became
       * available between the `if' above, and the clearing
       * of the event register.
       * This is to catch the case where the transmitter
       * stops in the middle of a frame -- and only the
       * last buffer descriptor in a frame can generate
       * an interrupt.
       */
      m8xx_fec_Enet_retire_tx_bd (sc);
      while ((sc->txBdActiveCount + nAdded) == sc->txBdCount) {
        /*
         * Unmask TXB (buffer transmitted) and
         * TXE (transmitter error) events.
         */
        m8xx.fec.ievent |= M8xx_FEC_IEVENT_TFINT;
        fec_wait_for_event (sc, INTERRUPT_EVENT);
        m8xx_fec_Enet_retire_tx_bd (sc);
      }
    }

    /*
     * Don't set the READY flag till the
     * whole packet has been readied.
     */
    status = nAdded ? M8xx_BD_READY : 0;

    /*
     *  FIXME: Why not deal with empty mbufs at at higher level?
     * The IP fragmentation routine in ip_output
     * can produce packet fragments with zero length.
     * I think that ip_output should be changed to get
     * rid of these zero-length mbufs, but for now,
     * I'll deal with them here.
     */
    if (m->m_len) {
      /*
       * Fill in the buffer descriptor
       */
      txBd->buffer = mtod (m, void *);
      txBd->length = m->m_len;

      /*
       * Flush the buffer for this descriptor
       */
      rtems_cache_flush_multiple_data_lines((void *)txBd->buffer, txBd->length);

      sc->txMbuf[sc->txBdHead] = m;
      nAdded++;
      if (++sc->txBdHead == sc->txBdCount) {
        status |= M8xx_BD_WRAP;
        sc->txBdHead = 0;
      }
      /*      l = m;*/
      m = m->m_next;
    }
    else {
      /*
       * Just toss empty mbufs
       */
      m = m_free (m);
      /*
	if (l != NULL)
        l->m_next = m;
      */
    }

    /*
     * Set the transmit buffer status.
     * Break out of the loop if this mbuf is the last in the frame.
     */
    if (m == NULL) {
      if (nAdded) {
        status |= M8xx_BD_LAST | M8xx_BD_TX_CRC;
        txBd->status = status;
        firstTxBd->status |= M8xx_BD_READY;
        m8xx.fec.x_des_active = 0x1000000;
        sc->txBdActiveCount += nAdded;
      }
      break;
    }
    txBd->status = status;
    txBd = sc->txBdBase + sc->txBdHead;
  }
}
void fec_txDaemon (void *arg)
{
  struct m8xx_fec_enet_struct *sc = (struct m8xx_fec_enet_struct *)arg;
  struct ifnet *ifp = sc->ifp;
  struct mbuf *m;

  FEC_LOCK(sc);

  for (;;) {
    /*
     * Wait for packet
     */
    fec_wait_for_event (sc, START_TRANSMIT_EVENT);

    /*
     * Send packets till queue is empty
     */
    for (;;) {
      /*
       * Get the next mbuf chain to transmit.
       */
      IF_DEQUEUE(&ifp->if_snd, m);
      if (!m)
        break;
      fec_sendpacket (sc, m);
    }
    ifp->if_drv_flags &= ~IFF_DRV_OACTIVE;
  }
}

static void fec_watchdog (void *arg)
{
  struct m8xx_fec_enet_struct *sc = arg;

  mii_tick(sc->mii_softc);
  callout_reset(&sc->watchdogCallout, FEC_WATCHDOG_TIMEOUT * hz,
		fec_watchdog, sc);
}

static int
fec_media_change(struct ifnet * ifp)
{
	struct m8xx_fec_enet_struct *sc;
	struct mii_data *mii;
	int error;

	sc = ifp->if_softc;
	mii = sc->mii_softc;

	if (mii != NULL) {
		FEC_LOCK(sc);
		error = mii_mediachg(sc->mii_softc);
		FEC_UNLOCK(sc);
	} else {
		error = ENXIO;
	}

	return (error);
}

static void
fec_media_status(struct ifnet * ifp, struct ifmediareq *ifmr)
{
	struct m8xx_fec_enet_struct *sc;
	struct mii_data *mii;

	sc = ifp->if_softc;
	mii = sc->mii_softc;

	if (mii != NULL) {
		FEC_LOCK(sc);
		mii_pollstat(mii);
		ifmr->ifm_active = mii->mii_media_active;
		ifmr->ifm_status = mii->mii_media_status;
		FEC_UNLOCK(sc);
	}
}

static void fec_init (void *arg)
{
  struct m8xx_fec_enet_struct *sc = arg;
  struct ifnet *ifp = sc->ifp;

  if (sc->txDaemonTid == 0) {
    int error;

    /*
     * Set up FEC hardware
     */
    m8xx_fec_initialize_hardware (sc);

    /*
     * init access to phy
     */
    fec_mdio_init(sc);

    /*
     * Start driver tasks
     */
    sc->txDaemonTid = rtems_bsdnet_newproc ("SCtx", 4096, fec_txDaemon, sc);
    sc->rxDaemonTid = rtems_bsdnet_newproc ("SCrx", 4096, fec_rxDaemon, sc);

    /* Attach the mii driver. */
    error = mii_attach(sc->dev, &sc->miibus, ifp, fec_media_change,
                       fec_media_status, BMSR_DEFCAPMASK, MII_PHY_ANY,
                       MII_OFFSET_ANY, 0);
    if (error == 0) {
      sc->mii_softc = device_get_softc(sc->miibus);
    }
  }

  /*
   * Set flags appropriately
   */
  if (ifp->if_flags & IFF_PROMISC)
    m8xx.fec.r_cntrl |= M8xx_FEC_R_CNTRL_PROM;
  else
    m8xx.fec.r_cntrl &= ~M8xx_FEC_R_CNTRL_PROM;

  if (sc->mii_softc != NULL ) {
    /*
     * init timer so the "watchdog function gets called periodically
     */
    mii_mediachg(sc->mii_softc);
    callout_reset(&sc->watchdogCallout, hz, fec_watchdog, sc);
  }

  /*
   * Tell the world that we're running.
   */
  ifp->if_drv_flags |= IFF_DRV_RUNNING;

  /*
   * Enable receiver and transmitter
   */
  m8xx.fec.ecntrl |= M8xx_FEC_ECNTRL_ETHER_EN;
}

/*
 * Send packet (caller provides header).
 */
static void
m8xx_fec_enet_start (struct ifnet *ifp)
{
  struct m8xx_fec_enet_struct *sc = ifp->if_softc;

  FEC_LOCK(sc);
  ifp->if_drv_flags |= IFF_DRV_OACTIVE;
  FEC_UNLOCK(sc);
  fec_send_event (sc->txDaemonTid, START_TRANSMIT_EVENT);
}

static void fec_stop (struct ifnet *ifp)
{
  ifp->if_drv_flags &= ~IFF_DRV_RUNNING;

  /*
   * Shut down receiver and transmitter
   */
  m8xx.fec.ecntrl &= ~M8xx_FEC_ECNTRL_ETHER_EN;
}

/*
 * Show interface statistics
 */
static void fec_enet_stats (struct m8xx_fec_enet_struct *sc)
{
  printf ("      Rx Interrupts:%-8lu", sc->rxInterrupts);
  printf ("       Not First:%-8lu", sc->rxNotFirst);
  printf ("        Not Last:%-8lu\n", sc->rxNotLast);
  printf ("              Giant:%-8lu", sc->rxGiant);
  printf ("            Runt:%-8lu", sc->rxRunt);
  printf ("       Non-octet:%-8lu\n", sc->rxNonOctet);
  printf ("            Bad CRC:%-8lu", sc->rxBadCRC);
  printf ("         Overrun:%-8lu", sc->rxOverrun);
  printf ("       Collision:%-8lu\n", sc->rxCollision);

  printf ("      Tx Interrupts:%-8lu", sc->txInterrupts);
  printf ("        Deferred:%-8lu", sc->txDeferred);
  printf (" Missed Hearbeat:%-8lu\n", sc->txHeartbeat);
  printf ("         No Carrier:%-8lu", sc->txLostCarrier);
  printf ("Retransmit Limit:%-8lu", sc->txRetryLimit);
  printf ("  Late Collision:%-8lu\n", sc->txLateCollision);
  printf ("           Underrun:%-8lu", sc->txUnderrun);
  printf (" Raw output wait:%-8lu\n", sc->txRawWait);
}

static int fec_ioctl (struct ifnet *ifp, ioctl_command_t cmd, caddr_t data)
{
  struct m8xx_fec_enet_struct *sc;
  struct ifreq *ifr;
  int error;
  struct mii_data *mii;

  sc = ifp->if_softc;
  ifr = (struct ifreq *)data;

  error = 0;
  switch (cmd) {
  case SIOCSIFFLAGS:
    FEC_LOCK(sc);
    if (ifp->if_flags & IFF_UP) {
      if (!(ifp->if_drv_flags & IFF_DRV_RUNNING)) {
        fec_init (sc);
      }
    } else {
      if (ifp->if_drv_flags & IFF_DRV_RUNNING)
        fec_stop (ifp);
    }
    sc->if_flags = ifp->if_flags;
    FEC_UNLOCK(sc);
    break;

  case SIOCSIFMEDIA:
  case SIOCGIFMEDIA:
    mii = sc->mii_softc;

    if (mii != NULL) {
      error = ifmedia_ioctl(ifp, ifr, &mii->mii_media, cmd);
    } else {
      error = ether_ioctl(ifp, cmd, data);
    }

    break;

  case SIO_RTEMS_SHOW_STATS:
    fec_enet_stats (sc);
    break;

    /*
     * FIXME: All sorts of multicast commands need to be added here!
     */
  default:
    error = ether_ioctl(ifp, cmd, data);
    break;
  }
  return error;
}

static void
fec_miibus_statchg(device_t dev)
{
	struct m8xx_fec_enet_struct *sc;
	struct mii_data *mii;

	sc = device_get_softc(dev);
	mii = sc->mii_softc;

	if (mii == NULL ||
	    (IFM_OPTIONS(mii->mii_media_active) & IFM_FDX) != 0) {
		m8xx.fec.x_cntrl |=  M8xx_FEC_X_CNTRL_FDEN;
		m8xx.fec.r_cntrl &= ~M8xx_FEC_R_CNTRL_DRT;
	} else {
		m8xx.fec.x_cntrl &= ~M8xx_FEC_X_CNTRL_FDEN;
		m8xx.fec.r_cntrl |=  M8xx_FEC_R_CNTRL_DRT;
	}
}

static int fec_attach (device_t dev)
{
  struct m8xx_fec_enet_struct *sc;
  struct ifnet *ifp;
  int unitNumber = device_get_unit(dev);
  uint8_t hwaddr[ETHER_ADDR_LEN];

  rtems_bsd_get_mac_address(device_get_name(dev), device_get_unit(dev),
			    hwaddr);

  sc = device_get_softc(dev);
  sc->dev = dev;
  sc->ifp = ifp = if_alloc(IFT_ETHER);

  mtx_init(&sc->mtx, device_get_nameunit(sc->dev), MTX_NETWORK_LOCK, MTX_DEF);
  callout_init_mtx(&sc->watchdogCallout, &sc->mtx, 0);

  sc->rxBdCount = RX_BUF_COUNT;
  sc->txBdCount = TX_BUF_COUNT * TX_BD_PER_BUF;

  /*
   * assume: IF 1 -> PHY 0
   */
  sc->phy_default = unitNumber-1;

  /*
   * Set up network interface values
   */
  ifp->if_softc = sc;
  if_initname(ifp, device_get_name(dev), device_get_unit(dev));
  ifp->if_init = fec_init;
  ifp->if_ioctl = fec_ioctl;
  ifp->if_start = m8xx_fec_enet_start;
  ifp->if_flags = IFF_BROADCAST | IFF_MULTICAST | IFF_SIMPLEX;
  IFQ_SET_MAXLEN(&ifp->if_snd, sc->txBdCount - 1);
  ifp->if_snd.ifq_drv_maxlen = sc->txBdCount - 1;
  IFQ_SET_READY(&ifp->if_snd);

  /*
   * Attach the interface
   */
  ether_ifattach (ifp, hwaddr);
  return 0;
};

static int
fec_probe(device_t dev)
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

static device_method_t fec_methods[] = {
	/* Device interface */
	DEVMETHOD(device_probe,		fec_probe),
	DEVMETHOD(device_attach,	fec_attach),

	/* MII Interface */
	DEVMETHOD(miibus_readreg,	fec_miibus_read_reg),
	DEVMETHOD(miibus_writereg,	fec_miibus_write_reg),
	DEVMETHOD(miibus_statchg,	fec_miibus_statchg),

	DEVMETHOD_END
};

static driver_t fec_nexus_driver = {
	"fec",
	fec_methods,
	sizeof(struct m8xx_fec_enet_struct)
};

static devclass_t fec_devclass;

DRIVER_MODULE(fec, nexus, fec_nexus_driver, fec_devclass, 0, 0);
DRIVER_MODULE(miibus, fec, miibus_driver, miibus_devclass, 0, 0);

MODULE_DEPEND(fec, nexus, 1, 1, 1);
MODULE_DEPEND(fec, ether, 1, 1, 1);

#endif /* LIBBSP_POWERPC_TQM8XX_BSP_H */
