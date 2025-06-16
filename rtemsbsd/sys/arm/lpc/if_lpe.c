/**
 * @file
 *
 * @ingroup lpc_eth
 *
 * @brief Ethernet driver.
 */

/*
 * Copyright (C) 2009, 2022 embedded brains GmbH
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <machine/rtems-bsd-kernel-space.h>

#include <bsp.h>

#if defined(LIBBSP_ARM_LPC24XX_BSP_H) || defined(LIBBSP_ARM_LPC32XX_BSP_H)

#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/malloc.h>
#include <sys/mbuf.h>
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

#include <dev/mii/mii.h>

#include <rtems/bsd/bsd.h>

#include <arm/lpc/probe.h>

#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/lpc-ethernet-config.h>
#include <bsp/utility.h>

#if MCLBYTES > (2 * 1024)
  #error "MCLBYTES to large"
#endif

#ifdef LPC_ETH_CONFIG_USE_TRANSMIT_DMA
  #define LPC_ETH_CONFIG_TX_BUF_SIZE sizeof(struct mbuf *)
#else
  #define LPC_ETH_CONFIG_TX_BUF_SIZE 1518U
#endif

#define DEFAULT_PHY 0
#define WATCHDOG_TIMEOUT 5

typedef struct {
  uint32_t start;
  uint32_t control;
} lpc_eth_transfer_descriptor;

typedef struct {
  uint32_t info;
  uint32_t hash_crc;
} lpc_eth_receive_status;

typedef struct {
  uint32_t mac1;
  uint32_t mac2;
  uint32_t ipgt;
  uint32_t ipgr;
  uint32_t clrt;
  uint32_t maxf;
  uint32_t supp;
  uint32_t test;
  uint32_t mcfg;
  uint32_t mcmd;
  uint32_t madr;
  uint32_t mwtd;
  uint32_t mrdd;
  uint32_t mind;
  uint32_t reserved_0 [2];
  uint32_t sa0;
  uint32_t sa1;
  uint32_t sa2;
  uint32_t reserved_1 [45];
  uint32_t command;
  uint32_t status;
  uint32_t rxdescriptor;
  uint32_t rxstatus;
  uint32_t rxdescriptornum;
  uint32_t rxproduceindex;
  uint32_t rxconsumeindex;
  uint32_t txdescriptor;
  uint32_t txstatus;
  uint32_t txdescriptornum;
  uint32_t txproduceindex;
  uint32_t txconsumeindex;
  uint32_t reserved_2 [10];
  uint32_t tsv0;
  uint32_t tsv1;
  uint32_t rsv;
  uint32_t reserved_3 [3];
  uint32_t flowcontrolcnt;
  uint32_t flowcontrolsts;
  uint32_t reserved_4 [34];
  uint32_t rxfilterctrl;
  uint32_t rxfilterwolsts;
  uint32_t rxfilterwolclr;
  uint32_t reserved_5 [1];
  uint32_t hashfilterl;
  uint32_t hashfilterh;
  uint32_t reserved_6 [882];
  uint32_t intstatus;
  uint32_t intenable;
  uint32_t intclear;
  uint32_t intset;
  uint32_t reserved_7 [1];
  uint32_t powerdown;
} lpc_eth_controller;

#define LPE_LOCK(e) mtx_lock(&(e)->mtx)

#define LPE_UNLOCK(e) mtx_unlock(&(e)->mtx)

static volatile lpc_eth_controller *const lpc_eth = 
  (volatile lpc_eth_controller *) LPC_ETH_CONFIG_REG_BASE;

/* ETH_RX_CTRL */

#define ETH_RX_CTRL_SIZE_MASK 0x000007ffU
#define ETH_RX_CTRL_INTERRUPT 0x80000000U

/* ETH_RX_STAT */

#define ETH_RX_STAT_RXSIZE_MASK 0x000007ffU
#define ETH_RX_STAT_BYTES 0x00000100U
#define ETH_RX_STAT_CONTROL_FRAME 0x00040000U
#define ETH_RX_STAT_VLAN 0x00080000U
#define ETH_RX_STAT_FAIL_FILTER 0x00100000U
#define ETH_RX_STAT_MULTICAST 0x00200000U
#define ETH_RX_STAT_BROADCAST 0x00400000U
#define ETH_RX_STAT_CRC_ERROR 0x00800000U
#define ETH_RX_STAT_SYMBOL_ERROR 0x01000000U
#define ETH_RX_STAT_LENGTH_ERROR 0x02000000U
#define ETH_RX_STAT_RANGE_ERROR 0x04000000U
#define ETH_RX_STAT_ALIGNMENT_ERROR 0x08000000U
#define ETH_RX_STAT_OVERRUN 0x10000000U
#define ETH_RX_STAT_NO_DESCRIPTOR 0x20000000U
#define ETH_RX_STAT_LAST_FLAG 0x40000000U
#define ETH_RX_STAT_ERROR 0x80000000U

/* ETH_TX_CTRL */

#define ETH_TX_CTRL_SIZE_MASK 0x7ffU
#define ETH_TX_CTRL_SIZE_SHIFT 0
#define ETH_TX_CTRL_OVERRIDE 0x04000000U
#define ETH_TX_CTRL_HUGE 0x08000000U
#define ETH_TX_CTRL_PAD 0x10000000U
#define ETH_TX_CTRL_CRC 0x20000000U
#define ETH_TX_CTRL_LAST 0x40000000U
#define ETH_TX_CTRL_INTERRUPT 0x80000000U

/* ETH_TX_STAT */

#define ETH_TX_STAT_COLLISION_COUNT_MASK 0x01e00000U
#define ETH_TX_STAT_DEFER 0x02000000U
#define ETH_TX_STAT_EXCESSIVE_DEFER 0x04000000U
#define ETH_TX_STAT_EXCESSIVE_COLLISION 0x08000000U
#define ETH_TX_STAT_LATE_COLLISION 0x10000000U
#define ETH_TX_STAT_UNDERRUN 0x20000000U
#define ETH_TX_STAT_NO_DESCRIPTOR 0x40000000U
#define ETH_TX_STAT_ERROR 0x80000000U

/* ETH_INT */

#define ETH_INT_RX_OVERRUN 0x00000001U
#define ETH_INT_RX_ERROR 0x00000002U
#define ETH_INT_RX_FINISHED 0x00000004U
#define ETH_INT_RX_DONE 0x00000008U
#define ETH_INT_TX_UNDERRUN 0x00000010U
#define ETH_INT_TX_ERROR 0x00000020U
#define ETH_INT_TX_FINISHED 0x00000040U
#define ETH_INT_TX_DONE 0x00000080U
#define ETH_INT_SOFT 0x00001000U
#define ETH_INT_WAKEUP 0x00002000U

/* ETH_RX_FIL_CTRL */

#define ETH_RX_FIL_CTRL_ACCEPT_UNICAST 0x00000001U
#define ETH_RX_FIL_CTRL_ACCEPT_BROADCAST 0x00000002U
#define ETH_RX_FIL_CTRL_ACCEPT_MULTICAST 0x00000004U
#define ETH_RX_FIL_CTRL_ACCEPT_UNICAST_HASH 0x00000008U
#define ETH_RX_FIL_CTRL_ACCEPT_MULTICAST_HASH 0x00000010U
#define ETH_RX_FIL_CTRL_ACCEPT_PERFECT 0x00000020U
#define ETH_RX_FIL_CTRL_MAGIC_PACKET_WOL 0x00001000U
#define ETH_RX_FIL_CTRL_RX_FILTER_WOL 0x00002000U

/* ETH_CMD */

#define ETH_CMD_RX_ENABLE 0x00000001U
#define ETH_CMD_TX_ENABLE 0x00000002U
#define ETH_CMD_REG_RESET 0x00000008U
#define ETH_CMD_TX_RESET 0x00000010U
#define ETH_CMD_RX_RESET 0x00000020U
#define ETH_CMD_PASS_RUNT_FRAME 0x00000040U
#define ETH_CMD_PASS_RX_FILTER 0X00000080U
#define ETH_CMD_TX_FLOW_CONTROL 0x00000100U
#define ETH_CMD_RMII 0x00000200U
#define ETH_CMD_FULL_DUPLEX 0x00000400U

/* ETH_STAT */

#define ETH_STAT_RX_ACTIVE 0x00000001U
#define ETH_STAT_TX_ACTIVE 0x00000002U

/* ETH_MAC2 */

#define ETH_MAC2_FULL_DUPLEX BSP_BIT32(8)

/* ETH_SUPP */

#define ETH_SUPP_SPEED BSP_BIT32(8)

/* ETH_MCFG */

#define ETH_MCFG_CLOCK_SELECT(val) BSP_FLD32(val, 2, 4)

#define ETH_MCFG_RESETMIIMGMT BSP_BIT32(15)

/* ETH_MCMD */

#define ETH_MCMD_READ BSP_BIT32(0)
#define ETH_MCMD_SCAN BSP_BIT32(1)

/* ETH_MADR */

#define ETH_MADR_REG(val) BSP_FLD32(val, 0, 4)
#define ETH_MADR_PHY(val) BSP_FLD32(val, 8, 12)

/* ETH_MIND */

#define ETH_MIND_BUSY BSP_BIT32(0)
#define ETH_MIND_SCANNING BSP_BIT32(1)
#define ETH_MIND_NOT_VALID BSP_BIT32(2)
#define ETH_MIND_MII_LINK_FAIL BSP_BIT32(3)

/* Events */

#define LPC_ETH_EVENT_INIT_RX RTEMS_EVENT_0

#define LPC_ETH_EVENT_INIT_TX RTEMS_EVENT_1

#define LPC_ETH_EVENT_INTERRUPT RTEMS_EVENT_3

#define LPC_ETH_EVENT_STOP RTEMS_EVENT_4

/* Status */

#define LPC_ETH_INTERRUPT_RECEIVE \
  (ETH_INT_RX_ERROR | ETH_INT_RX_FINISHED | ETH_INT_RX_DONE)

#define LPC_ETH_RX_STAT_ERRORS \
  (ETH_RX_STAT_CRC_ERROR \
    | ETH_RX_STAT_SYMBOL_ERROR \
    | ETH_RX_STAT_LENGTH_ERROR \
    | ETH_RX_STAT_ALIGNMENT_ERROR \
    | ETH_RX_STAT_OVERRUN \
    | ETH_RX_STAT_NO_DESCRIPTOR)

#define LPC_ETH_LAST_FRAGMENT_FLAGS \
  (ETH_TX_CTRL_OVERRIDE \
    | ETH_TX_CTRL_PAD \
    | ETH_TX_CTRL_CRC \
    | ETH_TX_CTRL_INTERRUPT \
    | ETH_TX_CTRL_LAST)

/* Debug */

#ifdef DEBUG
  #define LPC_ETH_PRINTF(...) printf(__VA_ARGS__)
  #define LPC_ETH_PRINTK(...) printk(__VA_ARGS__)
#else
  #define LPC_ETH_PRINTF(...)
  #define LPC_ETH_PRINTK(...)
#endif

typedef enum {
  LPC_ETH_STATE_NOT_INITIALIZED = 0,
  LPC_ETH_STATE_DOWN,
  LPC_ETH_STATE_UP
} lpc_eth_state;

typedef struct {
  device_t dev;
  struct ifnet *ifp;
  struct mtx mtx;
  lpc_eth_state state;
  uint32_t anlpar;
  struct callout watchdog_callout;
  rtems_id receive_task;
  unsigned rx_unit_count;
  unsigned tx_unit_count;
  volatile lpc_eth_transfer_descriptor *rx_desc_table;
  volatile lpc_eth_receive_status *rx_status_table;
  struct mbuf **rx_mbuf_table;
  volatile lpc_eth_transfer_descriptor *tx_desc_table;
  volatile uint32_t *tx_status_table;
  void *tx_buf_table;
  uint32_t tx_produce_index;
  uint32_t tx_consume_index;
  unsigned received_frames;
  unsigned receive_interrupts;
  unsigned transmitted_frames;
  unsigned receive_drop_errors;
  unsigned receive_overrun_errors;
  unsigned receive_fragment_errors;
  unsigned receive_crc_errors;
  unsigned receive_symbol_errors;
  unsigned receive_length_errors;
  unsigned receive_alignment_errors;
  unsigned receive_no_descriptor_errors;
  unsigned receive_fatal_errors;
  unsigned transmit_underrun_errors;
  unsigned transmit_late_collision_errors;
  unsigned transmit_excessive_collision_errors;
  unsigned transmit_excessive_defer_errors;
  unsigned transmit_no_descriptor_errors;
  unsigned transmit_overflow_errors;
  unsigned transmit_fatal_errors;
  uint32_t phy_id;
  int phy;
  rtems_vector_number interrupt_number;
  rtems_id control_task;
  int if_flags;
  struct ifmedia ifmedia;
} lpc_eth_driver_entry;

static void lpc_eth_interface_watchdog(void *arg);

static void lpc_eth_setup_rxfilter(lpc_eth_driver_entry *e);

static void lpc_eth_control_request_complete(const lpc_eth_driver_entry *e)
{
  rtems_status_code sc = rtems_event_transient_send(e->control_task);
  BSD_ASSERT(sc == RTEMS_SUCCESSFUL);
}

static void lpc_eth_control_request(
  lpc_eth_driver_entry *e,
  rtems_id task,
  rtems_event_set event
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  e->control_task = rtems_task_self();

  sc = rtems_event_send(task, event);
  BSD_ASSERT(sc == RTEMS_SUCCESSFUL);

  sc = rtems_event_transient_receive(RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  BSD_ASSERT(sc == RTEMS_SUCCESSFUL);

  e->control_task = 0;
}

static inline uint32_t lpc_eth_increment(
  uint32_t value,
  uint32_t cycle
)
{
  if (value < cycle) {
    return ++value;
  } else {
    return 0;
  }
}

static void lpc_eth_enable_promiscous_mode(bool enable)
{
  if (enable) {
    lpc_eth->rxfilterctrl = ETH_RX_FIL_CTRL_ACCEPT_UNICAST
      | ETH_RX_FIL_CTRL_ACCEPT_MULTICAST
      | ETH_RX_FIL_CTRL_ACCEPT_BROADCAST;
  } else {
    lpc_eth->rxfilterctrl = ETH_RX_FIL_CTRL_ACCEPT_PERFECT
      | ETH_RX_FIL_CTRL_ACCEPT_MULTICAST_HASH
      | ETH_RX_FIL_CTRL_ACCEPT_BROADCAST;
  }
}

static void lpc_eth_interrupt_handler(void *arg)
{
  lpc_eth_driver_entry *e = (lpc_eth_driver_entry *) arg;
  rtems_event_set re = 0;
  rtems_event_set te = 0;
  uint32_t ie = 0;

  /* Get interrupt status */
  uint32_t im = lpc_eth->intenable;
  uint32_t is = lpc_eth->intstatus & im;

  /* Check receive interrupts */
  if ((is & (ETH_INT_RX_OVERRUN | ETH_INT_TX_UNDERRUN)) != 0) {
    if ((is & ETH_INT_RX_OVERRUN) != 0) {
      re = LPC_ETH_EVENT_INIT_RX;
      ++e->receive_fatal_errors;
    }

    if ((is & ETH_INT_TX_UNDERRUN) != 0) {
      re = LPC_ETH_EVENT_INIT_TX;
      ++e->transmit_fatal_errors;
    }
  } else if ((is & LPC_ETH_INTERRUPT_RECEIVE) != 0) {
    re = LPC_ETH_EVENT_INTERRUPT;
    ie |= LPC_ETH_INTERRUPT_RECEIVE;
    ++e->receive_interrupts;
  }

  /* Send events to receive task */
  if (re != 0) {
    (void) rtems_event_send(e->receive_task, re);
  }

  LPC_ETH_PRINTK("interrupt: rx = 0x%08x, tx = 0x%08x\n", re, te);

  /* Update interrupt mask */
  lpc_eth->intenable = im & ~ie;

  /* Clear interrupts */
  lpc_eth->intclear = is;
}

static void lpc_eth_enable_receive_interrupts(void)
{
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);
  lpc_eth->intenable |= LPC_ETH_INTERRUPT_RECEIVE;
  rtems_interrupt_enable(level);
}

static void lpc_eth_disable_receive_interrupts(void)
{
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);
  lpc_eth->intenable &= ~LPC_ETH_INTERRUPT_RECEIVE;
  rtems_interrupt_enable(level);
}

static void lpc_eth_initialize_transmit(lpc_eth_driver_entry *e)
{
  volatile uint32_t *const status = e->tx_status_table;
  uint32_t const index_max = e->tx_unit_count - 1;
  volatile lpc_eth_transfer_descriptor *const desc = e->tx_desc_table;
  #ifdef LPC_ETH_CONFIG_USE_TRANSMIT_DMA
    struct mbuf **const mbufs = e->tx_buf_table;
  #else
    char *const buf = e->tx_buf_table;
  #endif
  uint32_t produce_index;

  /* Disable transmitter */
  lpc_eth->command &= ~ETH_CMD_TX_ENABLE;

  /* Wait for inactive status */
  while ((lpc_eth->status & ETH_STAT_TX_ACTIVE) != 0) {
    /* Wait */
  }

  /* Reset */
  lpc_eth->command |= ETH_CMD_TX_RESET;

  /* Transmit descriptors */
  lpc_eth->txdescriptornum = index_max;
  lpc_eth->txdescriptor = (uint32_t) desc;
  lpc_eth->txstatus = (uint32_t) status;

  #ifdef LPC_ETH_CONFIG_USE_TRANSMIT_DMA
    /* Discard outstanding fragments (= data loss) */
    for (produce_index = 0; produce_index <= index_max; ++produce_index) {
      m_freem(mbufs [produce_index]);
      mbufs [produce_index] = NULL;
    }
  #else
    /* Initialize descriptor table */
    for (produce_index = 0; produce_index <= index_max; ++produce_index) {
      desc [produce_index].start =
        (uint32_t) (buf + produce_index * LPC_ETH_CONFIG_TX_BUF_SIZE);
    }
  #endif

  /* Initialize indices */
  e->tx_produce_index = lpc_eth->txproduceindex;
  e->tx_consume_index = lpc_eth->txconsumeindex;

  /* Enable transmitter */
  lpc_eth->command |= ETH_CMD_TX_ENABLE;
}

#define LPC_ETH_RX_DATA_OFFSET 2

static struct mbuf *lpc_eth_new_mbuf(struct ifnet *ifp, bool wait)
{
  struct mbuf *m = NULL;
  int mw = wait ? M_WAITOK : M_NOWAIT;

  MGETHDR(m, mw, MT_DATA);
  if (m != NULL) {
    MCLGET(m, mw);
    if ((m->m_flags & M_EXT) != 0) {
      /* Set receive interface */
      m->m_pkthdr.rcvif = ifp;

      /* Adjust by two bytes for proper IP header alignment */
      m->m_data = mtod(m, char *) + LPC_ETH_RX_DATA_OFFSET;

      return m;
    } else {
      m_free(m);
    }
  }

  return NULL;
}

static bool lpc_eth_add_new_mbuf(
  struct ifnet *ifp,
  volatile lpc_eth_transfer_descriptor *desc,
  struct mbuf **mbufs,
  uint32_t i,
  bool wait
)
{
  /* New mbuf */
  struct mbuf *m = lpc_eth_new_mbuf(ifp, wait);

  /* Check mbuf */
  if (m != NULL) {
    /* Cache invalidate */
    rtems_cache_invalidate_multiple_data_lines(
      mtod(m, void *),
      MCLBYTES - LPC_ETH_RX_DATA_OFFSET
    );

    /* Add mbuf to queue */
    desc [i].start = mtod(m, uint32_t);
    desc [i].control = (MCLBYTES - LPC_ETH_RX_DATA_OFFSET - 1)
      | ETH_RX_CTRL_INTERRUPT;

    /* Cache flush of descriptor  */
    rtems_cache_flush_multiple_data_lines(
      (void *) &desc [i],
      sizeof(desc [0])
    );

    /* Add mbuf to table */
    mbufs [i] = m;

    return true;
  } else {
    return false;
  }
}

static void lpc_eth_receive_task(rtems_task_argument arg)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_event_set events = 0;
  lpc_eth_driver_entry *const e = (lpc_eth_driver_entry *) arg;
  struct ifnet *const ifp = e->ifp;
  volatile lpc_eth_transfer_descriptor *const desc = e->rx_desc_table;
  volatile lpc_eth_receive_status *const status = e->rx_status_table;
  struct mbuf **const mbufs = e->rx_mbuf_table;
  uint32_t const index_max = e->rx_unit_count - 1;
  uint32_t produce_index = 0;
  uint32_t consume_index = 0;

  LPC_ETH_PRINTF("%s\n", __func__);

  /* Main event loop */
  while (true) {
    /* Wait for events */
    sc = rtems_event_receive(
      LPC_ETH_EVENT_INIT_RX
        | LPC_ETH_EVENT_INIT_TX
        | LPC_ETH_EVENT_STOP
        | LPC_ETH_EVENT_INTERRUPT,
      RTEMS_EVENT_ANY | RTEMS_WAIT,
      RTEMS_NO_TIMEOUT,
      &events
    );
    BSD_ASSERT(sc == RTEMS_SUCCESSFUL);

    LPC_ETH_PRINTF("rx: wake up: 0x%08" PRIx32 "\n", events);

    /* Stop receiver? */
    if ((events & LPC_ETH_EVENT_STOP) != 0) {
      lpc_eth_control_request_complete(e);

      /* Wait for events */
      continue;
    }

    /* Initialize receiver or transmitter? */
    if ((events & (LPC_ETH_EVENT_INIT_RX | LPC_ETH_EVENT_INIT_TX)) != 0) {
      if ((events & LPC_ETH_EVENT_INIT_RX) != 0) {
        /* Disable receive interrupts */
        lpc_eth_disable_receive_interrupts();

        /* Disable receiver */
        lpc_eth->command &= ~ETH_CMD_RX_ENABLE;

        /* Wait for inactive status */
        while ((lpc_eth->status & ETH_STAT_RX_ACTIVE) != 0) {
          /* Wait */
        }

        /* Reset */
        lpc_eth->command |= ETH_CMD_RX_RESET;

        /* Clear receive interrupts */
        lpc_eth->intclear = LPC_ETH_INTERRUPT_RECEIVE;

        /* Move existing mbufs to the front */
        consume_index = 0;
        for (produce_index = 0; produce_index <= index_max; ++produce_index) {
          if (mbufs [produce_index] != NULL) {
            mbufs [consume_index] = mbufs [produce_index];
            ++consume_index;
          }
        }

        /* Fill receive queue */
        for (
          produce_index = consume_index;
          produce_index <= index_max;
          ++produce_index
        ) {
          lpc_eth_add_new_mbuf(ifp, desc, mbufs, produce_index, true);
        }

        /* Receive descriptor table */
        lpc_eth->rxdescriptornum = index_max;
        lpc_eth->rxdescriptor = (uint32_t) desc;
        lpc_eth->rxstatus = (uint32_t) status;

        /* Initialize indices */
        produce_index = lpc_eth->rxproduceindex;
        consume_index = lpc_eth->rxconsumeindex;

        /* Enable receiver */
        lpc_eth->command |= ETH_CMD_RX_ENABLE;

        /* Enable receive interrupts */
        lpc_eth_enable_receive_interrupts();

        lpc_eth_control_request_complete(e);
      }

      if ((events & LPC_ETH_EVENT_INIT_TX) != 0) {
        LPE_LOCK(e);
        lpc_eth_initialize_transmit(e);
        LPE_UNLOCK(e);
      }

      /* Wait for events */
      continue;
    }

    while (true) {
      /* Clear receive interrupt status */
      lpc_eth->intclear = LPC_ETH_INTERRUPT_RECEIVE;

      /* Get current produce index */
      produce_index = lpc_eth->rxproduceindex;

      if (consume_index != produce_index) {
        uint32_t stat = 0;

        /* Fragment status */
        rtems_cache_invalidate_multiple_data_lines(
          (void *) &status [consume_index],
          sizeof(status [0])
        );
        stat = status [consume_index].info;

        if (
          (stat & ETH_RX_STAT_LAST_FLAG) != 0
            && (stat & LPC_ETH_RX_STAT_ERRORS) == 0
        ) {
          /* Received mbuf */
          struct mbuf *m = mbufs [consume_index];

          if (lpc_eth_add_new_mbuf(ifp, desc, mbufs, consume_index, false)) {
            /* Discard Ethernet CRC */
            int sz = (int) (stat & ETH_RX_STAT_RXSIZE_MASK) + 1 - ETHER_CRC_LEN;

            /* Update mbuf */
            m->m_len = sz;
            m->m_pkthdr.len = sz;

            LPC_ETH_PRINTF("rx: %02" PRIu32 ": %u\n", consume_index, sz);

            /* Hand over */
            (*ifp->if_input)(ifp, m);

            /* Increment received frames counter */
            ++e->received_frames;
          } else {
            ++e->receive_drop_errors;
          }
        } else {
          /* Update error counters */
          if ((stat & ETH_RX_STAT_OVERRUN) != 0) {
            ++e->receive_overrun_errors;
          }
          if ((stat & ETH_RX_STAT_LAST_FLAG) == 0) {
            ++e->receive_fragment_errors;
          }
          if ((stat & ETH_RX_STAT_CRC_ERROR) != 0) {
            ++e->receive_crc_errors;
          }
          if ((stat & ETH_RX_STAT_SYMBOL_ERROR) != 0) {
            ++e->receive_symbol_errors;
          }
          if ((stat & ETH_RX_STAT_LENGTH_ERROR) != 0) {
            ++e->receive_length_errors;
          }
          if ((stat & ETH_RX_STAT_ALIGNMENT_ERROR) != 0) {
            ++e->receive_alignment_errors;
          }
          if ((stat & ETH_RX_STAT_NO_DESCRIPTOR) != 0) {
            ++e->receive_no_descriptor_errors;
          }
        }

        /* Increment and update consume index */
        consume_index = lpc_eth_increment(consume_index, index_max);
        lpc_eth->rxconsumeindex = consume_index;
      } else {
        /* Nothing to do, enable receive interrupts */
        lpc_eth_enable_receive_interrupts();
        break;
      }
    }
  }
}

static struct mbuf *lpc_eth_next_fragment(
  struct ifnet *ifp,
  struct mbuf *m,
  uint32_t *ctrl
)
{
  struct mbuf *n;
  int size;

  while (true) {
    /* Get fragment size */
    size = m->m_len;

    if (size > 0) {
      /* Now we have a not empty fragment */
      break;
    } else {
      /* Skip empty fragments */
      m = m->m_next;

      if (m == NULL) {
        return NULL;
      }
    }
  }

  /* Set fragment size */
  *ctrl = (uint32_t) (size - 1);

  /* Discard empty successive fragments */
  n = m->m_next;
  while (n != NULL && n->m_len <= 0) {
    n = m_free(n);
  }
  m->m_next = n;

  /* Is our fragment the last in the frame? */
  if (n == NULL) {
    *ctrl |= LPC_ETH_LAST_FRAGMENT_FLAGS;
  }

  return m;
}

static void lpc_eth_tx_reclaim(lpc_eth_driver_entry *e, struct ifnet *ifp)
{
  volatile uint32_t *const status = e->tx_status_table;
  volatile lpc_eth_transfer_descriptor *const desc = e->tx_desc_table;
  #ifdef LPC_ETH_CONFIG_USE_TRANSMIT_DMA
    struct mbuf **const mbufs = e->tx_buf_table;
  #else
    char *const buf = e->tx_buf_table;
  #endif
  uint32_t const index_max = e->tx_unit_count - 1;
  uint32_t consume_index = e->tx_consume_index;

  /* Free consumed fragments */
  while (true) {
    /* Save last known consume index */
    uint32_t c = consume_index;

    /* Get new consume index */
    consume_index = lpc_eth->txconsumeindex;

    /* Nothing consumed in the meantime? */
    if (c == consume_index) {
      break;
    }

    while (c != consume_index) {
      uint32_t s = status [c];

      /* Update error counters */
      if ((s & (ETH_TX_STAT_ERROR | ETH_TX_STAT_NO_DESCRIPTOR)) != 0) {
        if ((s & ETH_TX_STAT_UNDERRUN) != 0) {
          ++e->transmit_underrun_errors;
        }
        if ((s & ETH_TX_STAT_LATE_COLLISION) != 0) {
          ++e->transmit_late_collision_errors;
        }
        if ((s & ETH_TX_STAT_EXCESSIVE_COLLISION) != 0) {
          ++e->transmit_excessive_collision_errors;
        }
        if ((s & ETH_TX_STAT_EXCESSIVE_DEFER) != 0) {
          ++e->transmit_excessive_defer_errors;
        }
        if ((s & ETH_TX_STAT_NO_DESCRIPTOR) != 0) {
          ++e->transmit_no_descriptor_errors;
        }
      }

      #ifdef LPC_ETH_CONFIG_USE_TRANSMIT_DMA
        /* Release mbuf */
        m_freem(mbufs [c]);
        mbufs [c] = NULL;
      #endif

      /* Next consume index */
      c = lpc_eth_increment(c, index_max);
    }
  }

  e->tx_consume_index = consume_index;
}

static int lpc_eth_tx_enqueue(
  lpc_eth_driver_entry *e,
  struct ifnet *ifp,
  struct mbuf *m0
)
{
  volatile lpc_eth_transfer_descriptor *const desc = e->tx_desc_table;
  #ifdef LPC_ETH_CONFIG_USE_TRANSMIT_DMA
    struct mbuf **const mbufs = e->tx_buf_table;
  #else
    char *const buf = e->tx_buf_table;
    uint32_t frame_length;
    char *frame_buffer;
  #endif
  uint32_t const index_max = e->tx_unit_count - 1;
  uint32_t produce_index = e->tx_produce_index;
  uint32_t consume_index = e->tx_consume_index;
  struct mbuf *m = m0;

  while (true) {
    uint32_t ctrl;

    /* Compute next produce index */
    uint32_t p = lpc_eth_increment(produce_index, index_max);

    /* Queue full? */
    if (p == consume_index) {
      LPC_ETH_PRINTF("tx: full queue: 0x%08x\n", m);

      /* The queue is full */
      return ENOBUFS;
    }

    /* Get next fragment and control value */
    m = lpc_eth_next_fragment(ifp, m, &ctrl);

    /* New fragment? */
    if (m != NULL) {
      #ifdef LPC_ETH_CONFIG_USE_TRANSMIT_DMA
        /* Set the transfer data */
        rtems_cache_flush_multiple_data_lines(
          mtod(m, const void *),
          (size_t) m->m_len
        );
        desc [produce_index].start = mtod(m, uint32_t);
        desc [produce_index].control = ctrl;
        rtems_cache_flush_multiple_data_lines(
          (void *) &desc [produce_index],
          sizeof(desc [0])
         );

        LPC_ETH_PRINTF(
          "tx: %02" PRIu32 ": %u %s\n",
          produce_index, m->m_len,
          (ctrl & ETH_TX_CTRL_LAST) != 0 ? "L" : ""
        );

        /* Next produce index */
        produce_index = p;

        /* Last fragment of a frame? */
        if ((ctrl & ETH_TX_CTRL_LAST) != 0) {
          /* Update the produce index */
          lpc_eth->txproduceindex = produce_index;
          e->tx_produce_index = produce_index;

          mbufs [produce_index] = m0;

          /* Increment transmitted frames counter */
          ++e->transmitted_frames;

          return 0;
        }

        /* Next fragment of the frame */
        m = m->m_next;
      #else
        size_t fragment_length = (size_t) m->m_len;
        void *fragment_start = mtod(m, void *);
        uint32_t new_frame_length = frame_length + fragment_length;

        /* Check buffer size */
        if (new_frame_length > LPC_ETH_CONFIG_TX_BUF_SIZE) {
          LPC_ETH_PRINTF("tx: overflow\n");

          /* Discard overflow data */
          new_frame_length = LPC_ETH_CONFIG_TX_BUF_SIZE;
          fragment_length = new_frame_length - frame_length;

          /* Finalize frame */
          ctrl |= LPC_ETH_LAST_FRAGMENT_FLAGS;

          /* Update error counter */
          ++e->transmit_overflow_errors;
        }

        LPC_ETH_PRINTF(
          "tx: copy: %" PRIu32 "%s%s\n",
          fragment_length,
          (m->m_flags & M_EXT) != 0 ? ", E" : "",
          (m->m_flags & M_PKTHDR) != 0 ? ", H" : ""
        );

        /* Copy fragment to buffer in Ethernet RAM */
        memcpy(frame_buffer, fragment_start, fragment_length);

        if ((ctrl & ETH_TX_CTRL_LAST) != 0) {
          /* Finalize descriptor */
          desc [produce_index].control = (ctrl & ~ETH_TX_CTRL_SIZE_MASK)
            | (new_frame_length - 1);

          LPC_ETH_PRINTF(
            "tx: %02" PRIu32 ": %" PRIu32 "\n",
            produce_index,
            new_frame_length
          );

          /* Cache flush of data */
          rtems_cache_flush_multiple_data_lines(
            (const void *) desc [produce_index].start,
            new_frame_length
          );

          /* Cache flush of descriptor  */
          rtems_cache_flush_multiple_data_lines(
            (void *) &desc [produce_index],
            sizeof(desc [0])
          );

          /* Next produce index */
          produce_index = p;

          /* Update the produce index */
          lpc_eth->txproduceindex = produce_index;

          /* Fresh frame length and buffer start */
          frame_length = 0;
          frame_buffer = (char *) desc [produce_index].start;

          /* Increment transmitted frames counter */
          ++e->transmitted_frames;
        } else {
          /* New frame length */
          frame_length = new_frame_length;

          /* Update current frame buffer start */
          frame_buffer += fragment_length;
        }

        /* Free mbuf and get next */
        m = m_free(m);
      #endif
    } else {
      /* Nothing to transmit */
      m_freem(m0);
      return 0;
    }
  }
}

static int lpc_eth_mdio_wait_for_not_busy(void)
{
  rtems_interval one_second = rtems_clock_get_ticks_per_second();
  rtems_interval i = 0;

  while ((lpc_eth->mind & ETH_MIND_BUSY) != 0 && i < one_second) {
    rtems_task_wake_after(1);
    ++i;
  }

  LPC_ETH_PRINTK("tx: lpc_eth_mdio_wait %s after %d\n",
                 i != one_second? "succeed": "timeout", i);

  return i != one_second ? 0 : ETIMEDOUT;
}

static uint32_t lpc_eth_mdio_read_anlpar(int phy)
{
  uint32_t madr = ETH_MADR_REG(MII_ANLPAR) | ETH_MADR_PHY(phy);
  uint32_t anlpar = 0;
  int eno = 0;

  if (lpc_eth->madr != madr) {
    lpc_eth->madr = madr;
  }

  if (lpc_eth->mcmd != ETH_MCMD_READ) {
    lpc_eth->mcmd = 0;
    lpc_eth->mcmd = ETH_MCMD_READ;
  }

  eno = lpc_eth_mdio_wait_for_not_busy();
  if (eno == 0) {
    anlpar = lpc_eth->mrdd;
  }

  /* Start next read */
  lpc_eth->mcmd = 0;
  lpc_eth->mcmd = ETH_MCMD_READ;

  return anlpar;
}

static int lpc_eth_mdio_read(
  int phy,
  void *arg RTEMS_UNUSED,
  unsigned reg,
  uint32_t *val
)
{
  int eno = 0;

  if (0 <= phy && phy <= 31) {
    lpc_eth->madr = ETH_MADR_REG(reg) | ETH_MADR_PHY(phy);
    lpc_eth->mcmd = 0;
    lpc_eth->mcmd = ETH_MCMD_READ;
    eno = lpc_eth_mdio_wait_for_not_busy();

    if (eno == 0) {
      *val = lpc_eth->mrdd;
    }
  } else {
    eno = EINVAL;
  }

  return eno;
}

static int lpc_eth_mdio_write(
  int phy,
  void *arg RTEMS_UNUSED,
  unsigned reg,
  uint32_t val
)
{
  int eno = 0;

  if (0 <= phy && phy <= 31) {
    lpc_eth->madr = ETH_MADR_REG(reg) | ETH_MADR_PHY(phy);
    lpc_eth->mwtd = val;
    eno = lpc_eth_mdio_wait_for_not_busy();
  } else {
    eno = EINVAL;
  }

  return eno;
}

static int lpc_eth_phy_get_id(int phy, uint32_t *id)
{
  uint32_t id1 = 0;
  int eno = lpc_eth_mdio_read(phy, NULL, MII_PHYIDR1, &id1);

  if (eno == 0) {
    uint32_t id2 = 0;

    eno = lpc_eth_mdio_read(phy, NULL, MII_PHYIDR2, &id2);
    if (eno == 0) {
      *id = (id1 << 16) | (id2 & 0xfff0);
    }
  }

  return eno;
}

#define PHY_KSZ80X1RNL 0x221550
#define PHY_DP83848    0x20005c90

typedef struct {
  unsigned reg;
  uint32_t set;
  uint32_t clear;
} lpc_eth_phy_action;

static int lpc_eth_phy_set_and_clear(
  lpc_eth_driver_entry *e,
  const lpc_eth_phy_action *actions,
  size_t n
)
{
  int eno = 0;
  size_t i;

  for (i = 0; eno == 0 && i < n; ++i) {
    const lpc_eth_phy_action *action = &actions [i];
    uint32_t val;

    eno = lpc_eth_mdio_read(e->phy, NULL, action->reg, &val);
    if (eno == 0) {
      val |= action->set;
      val &= ~action->clear;
      eno = lpc_eth_mdio_write(e->phy, NULL, action->reg, val);
    }
  }

  return eno;
}

static const lpc_eth_phy_action lpc_eth_phy_up_action_default [] = {
  { MII_BMCR, 0, BMCR_PDOWN },
  { MII_BMCR, BMCR_RESET, 0 },
  { MII_BMCR, BMCR_AUTOEN, 0 }
};

static const lpc_eth_phy_action lpc_eth_phy_up_pre_action_KSZ80X1RNL [] = {
  /* Disable slow oscillator mode */
  { 0x11, 0, 0x10 }
};

static const lpc_eth_phy_action lpc_eth_phy_up_post_action_KSZ80X1RNL [] = {
  /* Enable energy detect power down (EDPD) mode */
  { 0x18, 0x0800, 0 },
  /* Turn PLL of automatically in EDPD mode */
  { 0x10, 0x10, 0 }
};

static int lpc_eth_phy_up(lpc_eth_driver_entry *e)
{
  int eno;
  int retries = 64;
  uint32_t val;

  e->phy = DEFAULT_PHY - 1;
  while (true) {
    e->phy = (e->phy + 1) % 32;

    --retries;
    eno = lpc_eth_phy_get_id(e->phy, &e->phy_id);
    if (
      (eno == 0 && e->phy_id != 0xfffffff0 && e->phy_id != 0)
        || retries <= 0
    ) {
      break;
    }

    rtems_task_wake_after(1);
  }

  LPC_ETH_PRINTF("lpc_eth_phy_get_id: 0x%08" PRIx32 " from phy %d retries %d\n",
                 e->phy_id, e->phy, retries);

  if (eno == 0) {
    switch (e->phy_id) {
      case PHY_KSZ80X1RNL:
        eno = lpc_eth_phy_set_and_clear(
          e,
          &lpc_eth_phy_up_pre_action_KSZ80X1RNL [0],
          RTEMS_ARRAY_SIZE(lpc_eth_phy_up_pre_action_KSZ80X1RNL)
        );
        break;
      case PHY_DP83848:
        eno = lpc_eth_mdio_read(e->phy, NULL, 0x17, &val);
        LPC_ETH_PRINTF("phy PHY_DP83848 RBR 0x%08" PRIx32 "\n", val);
        /* val = 0x21; */
        val = 0x32 ;
        eno = lpc_eth_mdio_write(e->phy, NULL, 0x17, val);
        break;
      case 0:
      case 0xfffffff0:
        eno = EIO;
        e->phy = DEFAULT_PHY;
        break;
      default:
        break;
    }

    if (eno == 0) {
      eno = lpc_eth_phy_set_and_clear(
        e,
        &lpc_eth_phy_up_action_default [0],
        RTEMS_ARRAY_SIZE(lpc_eth_phy_up_action_default)
      );
    }

    if (eno == 0) {
      switch (e->phy_id) {
        case PHY_KSZ80X1RNL:
          eno = lpc_eth_phy_set_and_clear(
            e,
            &lpc_eth_phy_up_post_action_KSZ80X1RNL [0],
            RTEMS_ARRAY_SIZE(lpc_eth_phy_up_post_action_KSZ80X1RNL)
          );
          break;
        default:
          break;
      }
    }
  } else {
    e->phy_id = 0;
  }

  return eno;
}

static const lpc_eth_phy_action lpc_eth_phy_down_action_default [] = {
  { MII_BMCR, BMCR_PDOWN, 0 }
};

static const lpc_eth_phy_action lpc_eth_phy_down_post_action_KSZ80X1RNL [] = {
  /* Enable slow oscillator mode */
  { 0x11, 0x10, 0 }
};

static void lpc_eth_phy_down(lpc_eth_driver_entry *e)
{
  int eno = lpc_eth_phy_set_and_clear(
    e,
    &lpc_eth_phy_down_action_default [0],
    RTEMS_ARRAY_SIZE(lpc_eth_phy_down_action_default)
  );

  if (eno == 0) {
    switch (e->phy_id) {
      case PHY_KSZ80X1RNL:
        eno = lpc_eth_phy_set_and_clear(
          e,
          &lpc_eth_phy_down_post_action_KSZ80X1RNL [0],
          RTEMS_ARRAY_SIZE(lpc_eth_phy_down_post_action_KSZ80X1RNL)
        );
        break;
      default:
        break;
    }
  }
}

static void lpc_eth_soft_reset(void)
{
  lpc_eth->command = 0x38;
  lpc_eth->mac1 = 0xcf00;
  lpc_eth->mac1 = 0x0;
}

static int lpc_eth_up_or_down(lpc_eth_driver_entry *e, bool up)
{
  int eno = 0;
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  struct ifnet *ifp = e->ifp;

  if (up && e->state == LPC_ETH_STATE_DOWN) {
    lpc_eth_config_module_enable();

    /* Enable RX/TX reset and disable soft reset */
    lpc_eth->mac1 = 0xf00;

    /* Initialize PHY */
    /* Clock value 10 (divide by 44 ) is safe on LPC178x up to 100 MHz AHB clock */
    lpc_eth->mcfg = ETH_MCFG_CLOCK_SELECT(10) | ETH_MCFG_RESETMIIMGMT;
    rtems_task_wake_after(1);
    lpc_eth->mcfg = ETH_MCFG_CLOCK_SELECT(10);
    rtems_task_wake_after(1);
    eno = lpc_eth_phy_up(e);

    if (eno == 0) {
      const uint8_t *eaddr;

      /*
       * We must have a valid external clock from the PHY at this point,
       * otherwise the system bus hangs and only a watchdog reset helps.
       */
      lpc_eth_soft_reset();

      /* Reinitialize registers */
      lpc_eth->mac2 = 0x31;
      lpc_eth->ipgt = 0x15;
      lpc_eth->ipgr = 0x12;
      lpc_eth->clrt = 0x370f;
      lpc_eth->maxf = 0x0600;
      lpc_eth->supp = ETH_SUPP_SPEED;
      lpc_eth->test = 0;
      #ifdef LPC_ETH_CONFIG_RMII
        lpc_eth->command = 0x0600;
      #else
        lpc_eth->command = 0x0400;
      #endif
      lpc_eth->intenable = ETH_INT_RX_OVERRUN | ETH_INT_TX_UNDERRUN;
      lpc_eth->intclear = 0x30ff;
      lpc_eth->powerdown = 0;

      /* MAC address */
      eaddr = IF_LLADDR(e->ifp);
      lpc_eth->sa0 = ((uint32_t) eaddr [5] << 8) | (uint32_t) eaddr [4];
      lpc_eth->sa1 = ((uint32_t) eaddr [3] << 8) | (uint32_t) eaddr [2];
      lpc_eth->sa2 = ((uint32_t) eaddr [1] << 8) | (uint32_t) eaddr [0];

      lpc_eth_setup_rxfilter(e);

      /* Enable receiver */
      lpc_eth->mac1 = 0x03;

      /* Initialize tasks */
      lpc_eth_control_request(e, e->receive_task, LPC_ETH_EVENT_INIT_RX);
      lpc_eth_initialize_transmit(e);

      /* Install interrupt handler */
      sc = rtems_interrupt_handler_install(
        e->interrupt_number,
        "Ethernet",
        RTEMS_INTERRUPT_UNIQUE,
        lpc_eth_interrupt_handler,
        e
      );
      BSD_ASSERT(sc == RTEMS_SUCCESSFUL);

      /* Start watchdog timer */
      callout_reset(&e->watchdog_callout, hz, lpc_eth_interface_watchdog, e);

      /* Change state */
      ifp->if_drv_flags |= IFF_DRV_RUNNING;
      e->state = LPC_ETH_STATE_UP;
    }
  } else if (!up && e->state == LPC_ETH_STATE_UP) {
    ifp->if_drv_flags &= ~IFF_DRV_RUNNING;

    /* Remove interrupt handler */
    sc = rtems_interrupt_handler_remove(
      e->interrupt_number,
      lpc_eth_interrupt_handler,
      e
    );
    BSD_ASSERT(sc == RTEMS_SUCCESSFUL);

    /* Stop task */
    lpc_eth_control_request(e, e->receive_task, LPC_ETH_EVENT_STOP);

    lpc_eth_soft_reset();
    lpc_eth_phy_down(e);
    lpc_eth_config_module_disable();

    /* Stop watchdog timer */
    callout_stop(&e->watchdog_callout);

    /* Change state */
    e->state = LPC_ETH_STATE_DOWN;
  }

  return eno;
}

static void lpc_eth_interface_init(void *arg)
{
  lpc_eth_driver_entry *e = (lpc_eth_driver_entry *) arg;

  (void) lpc_eth_up_or_down(e, true);
}

static void lpc_eth_setup_rxfilter(lpc_eth_driver_entry *e)
{
  struct ifnet *ifp = e->ifp;
  struct epoch_tracker et;

  lpc_eth_enable_promiscous_mode((ifp->if_flags & IFF_PROMISC) != 0);

  if ((ifp->if_flags & IFF_ALLMULTI)) {
    lpc_eth->hashfilterl = 0xffffffff;
    lpc_eth->hashfilterh = 0xffffffff;
  } else {
    struct ifmultiaddr *ifma;

    lpc_eth->hashfilterl = 0x0;
    lpc_eth->hashfilterh = 0x0;

    NET_EPOCH_ENTER(et);
    CK_STAILQ_FOREACH(ifma, &ifp->if_multiaddrs, ifma_link) {
      uint32_t crc;
      uint32_t index;

      if (ifma->ifma_addr->sa_family != AF_LINK)
        continue;

      /* XXX: ether_crc32_le() does not work, why? */
      crc = ether_crc32_be(
        LLADDR((struct sockaddr_dl *) ifma->ifma_addr),
        ETHER_ADDR_LEN
      );
      index = (crc >> 23) & 0x3f;

      if (index < 32) {
        lpc_eth->hashfilterl |= 1U << index;
      } else {
        lpc_eth->hashfilterh |= 1U << (index - 32);
      }
    }
    NET_EPOCH_EXIT(et);
  }
}

static int lpc_eth_interface_ioctl(
  struct ifnet *ifp,
  ioctl_command_t cmd,
  caddr_t data
)
{
  lpc_eth_driver_entry *e = (lpc_eth_driver_entry *) ifp->if_softc;
  struct ifreq *ifr = (struct ifreq *) data;
  int eno = 0;

  LPC_ETH_PRINTF("%s\n", __func__);

  switch (cmd)  {
    case SIOCGIFMEDIA:
    case SIOCSIFMEDIA:
      eno = ifmedia_ioctl(ifp, ifr, &e->ifmedia, cmd);
      break;
    case SIOCGIFADDR:
    case SIOCSIFADDR:
      ether_ioctl(ifp, cmd, data);
      break;
    case SIOCSIFFLAGS:
      LPE_LOCK(e);
      if (ifp->if_flags & IFF_UP) {
        if (ifp->if_drv_flags & IFF_DRV_RUNNING) {
          if ((ifp->if_flags ^ e->if_flags) & (IFF_PROMISC | IFF_ALLMULTI)) {
            lpc_eth_setup_rxfilter(e);
          }
        } else {
          eno = lpc_eth_up_or_down(e, true);
        }
      } else {
        if (ifp->if_drv_flags & IFF_DRV_RUNNING) {
          eno = lpc_eth_up_or_down(e, false);
        }
      }
      e->if_flags = ifp->if_flags;
      LPE_UNLOCK(e);
      break;
    case SIOCADDMULTI:
    case SIOCDELMULTI:
      if (ifp->if_drv_flags & IFF_DRV_RUNNING) {
        LPE_LOCK(e);
        lpc_eth_setup_rxfilter(e);
        LPE_UNLOCK(e);
      }
      break;
    default:
      eno = ether_ioctl(ifp, cmd, data);
      break;
  }

  return eno;
}

static int lpc_eth_interface_transmit(struct ifnet *ifp, struct mbuf *m)
{
  lpc_eth_driver_entry *e = (lpc_eth_driver_entry *) ifp->if_softc;
  int eno;

  LPE_LOCK(e);

  if (e->state == LPC_ETH_STATE_UP) {
    eno = lpc_eth_tx_enqueue(e, ifp, m);
    lpc_eth_tx_reclaim(e, ifp);

    if (__predict_false(eno != 0)) {
      struct mbuf *n;

      n = m_defrag(m, M_NOWAIT);
      if (n != NULL) {
        m = n;
      }

      eno = lpc_eth_tx_enqueue(e, ifp, m);
    }
  } else {
    eno = ENETDOWN;
  }

  if (eno != 0) {
    m_freem(m);
    if_inc_counter(ifp, IFCOUNTER_OQDROPS, 1);
  }

  LPE_UNLOCK(e);
  return eno;
}

static void lpc_eth_interface_watchdog(void *arg)
{
  lpc_eth_driver_entry *e = (lpc_eth_driver_entry *) arg;

  if (e->state == LPC_ETH_STATE_UP) {
    uint32_t anlpar = lpc_eth_mdio_read_anlpar(e->phy);

    if (e->anlpar != anlpar) {
      bool full_duplex = false;
      bool speed = false;

      e->anlpar = anlpar;

      if ((anlpar & ANLPAR_TX_FD) != 0) {
        full_duplex = true;
        speed = true;
      } else if ((anlpar & ANLPAR_T4) != 0) {
        speed = true;
      } else if ((anlpar & ANLPAR_TX) != 0) {
        speed = true;
      } else if ((anlpar & ANLPAR_10_FD) != 0) {
        full_duplex = true;
      }

      if (full_duplex) {
        lpc_eth->mac2 |= ETH_MAC2_FULL_DUPLEX;
      } else {
        lpc_eth->mac2 &= ~ETH_MAC2_FULL_DUPLEX;
      }

      if (speed) {
        lpc_eth->supp |= ETH_SUPP_SPEED;
      } else {
        lpc_eth->supp &= ~ETH_SUPP_SPEED;
      }
    }

    callout_reset(&e->watchdog_callout, WATCHDOG_TIMEOUT * hz, lpc_eth_interface_watchdog, e);
  }
}

static int lpc_eth_media_change(struct ifnet *ifp)
{
  (void) ifp;
  return EINVAL;
}

static void lpc_eth_media_status(struct ifnet *ifp, struct ifmediareq *imr)
{
  (void) ifp;

  imr->ifm_status = IFM_AVALID | IFM_ACTIVE;
  imr->ifm_active = IFM_ETHER;

  if ((lpc_eth->supp & ETH_SUPP_SPEED) != 0) {
    imr->ifm_active |= IFM_100_TX;
  } else {
    imr->ifm_active |= IFM_10_T;
  }

  if ((lpc_eth->mac2 & ETH_MAC2_FULL_DUPLEX) != 0) {
    imr->ifm_active |= IFM_FDX;
  } else {
    imr->ifm_active |= IFM_HDX;
  }
}

__weak_symbol int lpc_eth_probe(int unit)
{
  if (unit != 0) {
    return ENXIO;
  }

  return BUS_PROBE_DEFAULT;
}

static int lpc_eth_do_probe(device_t dev)
{
  device_set_desc(dev, "LPC32x0 Ethernet controller");
  return lpc_eth_probe(device_get_unit(dev));
}

static int lpc_eth_attach(device_t dev)
{
  lpc_eth_driver_entry *e = device_get_softc(dev);
  struct ifnet *ifp = NULL;
  char *unit_name = NULL;
  int unit_index = device_get_unit(dev);
  size_t table_area_size = 0;
  char *table_area = NULL;
  char *table_location = NULL;
  rtems_status_code status;
  uint8_t eaddr[ETHER_ADDR_LEN];

  BSD_ASSERT(e->state == LPC_ETH_STATE_NOT_INITIALIZED);

  mtx_init(&e->mtx, device_get_nameunit(e->dev), MTX_NETWORK_LOCK, MTX_DEF);

  ifmedia_init(&e->ifmedia, 0, lpc_eth_media_change, lpc_eth_media_status);
  ifmedia_add(&e->ifmedia, IFM_ETHER | IFM_AUTO, 0, NULL);
  ifmedia_set(&e->ifmedia, IFM_ETHER | IFM_AUTO);

  callout_init_mtx(&e->watchdog_callout, &e->mtx, 0);

  /* Receive unit count */
  e->rx_unit_count = LPC_ETH_CONFIG_RX_UNIT_COUNT_DEFAULT;

  /* Transmit unit count */
  e->tx_unit_count = LPC_ETH_CONFIG_TX_UNIT_COUNT_DEFAULT;

  /* Remember interrupt number */
  e->interrupt_number = LPC_ETH_CONFIG_INTERRUPT;

  /* Allocate and clear table area */
  table_area_size =
    e->rx_unit_count
      * (sizeof(lpc_eth_transfer_descriptor)
        + sizeof(lpc_eth_receive_status)
        + sizeof(struct mbuf *))
    + e->tx_unit_count
      * (sizeof(lpc_eth_transfer_descriptor)
        + sizeof(uint32_t)
        + LPC_ETH_CONFIG_TX_BUF_SIZE);
  table_area = lpc_eth_config_alloc_table_area(table_area_size);
  if (table_area == NULL) {
    return ENOMEM;
  }
  memset(table_area, 0, table_area_size);

  table_location = table_area;

  /*
   * The receive status table must be the first one since it has the strictest
   * alignment requirements.
   */
  e->rx_status_table = (volatile lpc_eth_receive_status *) table_location;
  table_location += e->rx_unit_count * sizeof(e->rx_status_table [0]);

  e->rx_desc_table = (volatile lpc_eth_transfer_descriptor *) table_location;
  table_location += e->rx_unit_count * sizeof(e->rx_desc_table [0]);

  e->rx_mbuf_table = (struct mbuf **) table_location;
  table_location += e->rx_unit_count * sizeof(e->rx_mbuf_table [0]);

  e->tx_desc_table = (volatile lpc_eth_transfer_descriptor *) table_location;
  table_location += e->tx_unit_count * sizeof(e->tx_desc_table [0]);

  e->tx_status_table = (volatile uint32_t *) table_location;
  table_location += e->tx_unit_count * sizeof(e->tx_status_table [0]);

  e->tx_buf_table = table_location;

  /* Set interface data */
  e->dev = dev;
  e->ifp = ifp = if_alloc(IFT_ETHER);
  ifp->if_softc = e;
  if_initname(ifp, device_get_name(dev), device_get_unit(dev));
  ifp->if_init = lpc_eth_interface_init;
  ifp->if_ioctl = lpc_eth_interface_ioctl;
  ifp->if_transmit = lpc_eth_interface_transmit;
  ifp->if_qflush = if_qflush;
  ifp->if_flags = IFF_BROADCAST | IFF_MULTICAST | IFF_SIMPLEX;
  IFQ_SET_MAXLEN(&ifp->if_snd, LPC_ETH_CONFIG_TX_UNIT_COUNT_MAX - 1);
  ifp->if_snd.ifq_drv_maxlen = LPC_ETH_CONFIG_TX_UNIT_COUNT_MAX - 1;
  IFQ_SET_READY(&ifp->if_snd);
  ifp->if_hdrlen = sizeof(struct ether_header);

  rtems_bsd_get_mac_address(device_get_name(e->dev), unit_index, eaddr);

  /* Create tasks */
  status = rtems_task_create(
    rtems_build_name('n', 't', 'r', 'x'),
    rtems_bsd_get_task_priority(device_get_name(e->dev)),
    4096,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &e->receive_task
  );
  BSD_ASSERT(status == RTEMS_SUCCESSFUL);
  status = rtems_task_start(
    e->receive_task,
    lpc_eth_receive_task,
    (rtems_task_argument)e
  );
  BSD_ASSERT(status == RTEMS_SUCCESSFUL);

  if_link_state_change(e->ifp, LINK_STATE_UP);

  /* Change status */
  e->state = LPC_ETH_STATE_DOWN;

  /* Attach the interface */
  ether_ifattach(ifp, eaddr);

  return 0;
}

static int lpc_eth_detach(device_t dev)
{
  /* FIXME: Detach the interface from the upper layers? */

  /* Module soft reset */
  lpc_eth->command = 0x38;
  lpc_eth->mac1 = 0xcf00;

  /* FIXME: More cleanup */

  return ENXIO;
}

static device_method_t lpe_methods[] = {
  DEVMETHOD(device_probe, lpc_eth_do_probe),
  DEVMETHOD(device_attach, lpc_eth_attach),
  DEVMETHOD(device_detach, lpc_eth_detach),
  DEVMETHOD_END
};

static driver_t lpe_nexus_driver = {
  "lpe",
  lpe_methods,
  sizeof(lpc_eth_driver_entry)
};

DRIVER_MODULE(lpe, nexus, lpe_nexus_driver, 0, 0);
MODULE_DEPEND(lpe, nexus, 1, 1, 1);
MODULE_DEPEND(lpe, ether, 1, 1, 1);

#endif /* LIBBSP_ARM_LPC24XX_BSP_H || LIBBSP_ARM_LPC32XX_BSP_H */
