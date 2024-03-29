/* Acknowledgement:
 *
 * Valuable information for developing this driver was obtained
 * from the linux open-source driver mv643xx_eth.c which was written
 * by the following people and organizations:
 *
 * Matthew Dharm <mdharm@momenco.com>
 * rabeeh@galileo.co.il
 * PMC-Sierra, Inc., Manish Lachwani
 * Ralf Baechle <ralf@linux-mips.org>
 * MontaVista Software, Inc., Dale Farnsworth <dale@farnsworth.org>
 * Steven J. Hill <sjhill1@rockwellcollins.com>/<sjhill@realitydiluted.com>
 *
 * Note however, that in spite of the identical name of this file
 * (and some of the symbols used herein) this file provides a
 * new implementation and is the original work by the author.
 */

/*
 * Authorship
 * ----------
 * This software (mv643xx ethernet driver for RTEMS) was
 *     created by Till Straumann <strauman@slac.stanford.edu>, 2005-2007,
 *         Stanford Linear Accelerator Center, Stanford University.
 *
 * Acknowledgement of sponsorship
 * ------------------------------
 * The 'mv643xx ethernet driver for RTEMS' was produced by
 *     the Stanford Linear Accelerator Center, Stanford University,
 *         under Contract DE-AC03-76SFO0515 with the Department of Energy.
 *
 * Government disclaimer of liability
 * ----------------------------------
 * Neither the United States nor the United States Department of Energy,
 * nor any of their employees, makes any warranty, express or implied, or
 * assumes any legal liability or responsibility for the accuracy,
 * completeness, or usefulness of any data, apparatus, product, or process
 * disclosed, or represents that its use would not infringe privately owned
 * rights.
 *
 * Stanford disclaimer of liability
 * --------------------------------
 * Stanford University makes no representations or warranties, express or
 * implied, nor assumes any liability for the use of this software.
 *
 * Stanford disclaimer of copyright
 * --------------------------------
 * Stanford University, owner of the copyright, hereby disclaims its
 * copyright and all other rights in this software.  Hence, anyone may
 * freely use it for any purpose without restriction.
 *
 * Maintenance of notices
 * ----------------------
 * In the interest of clarity regarding the origin and status of this
 * SLAC software, this and all the preceding Stanford University notices
 * are to remain affixed to any copy or derivative of this software made
 * or distributed by the recipient and are to be affixed to any copy of
 * software made or distributed by the recipient that contains a copy or
 * derivative of this software.
 *
 * ------------------ SLAC Software Notices, Set 4 OTT.002a, 2004 FEB 03
 */

#ifndef MV643XX_LOWLEVEL_DRIVER_H
#define MV643XX_LOWLEVEL_DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

#define MV643XXETH_NUM_DRIVER_SLOTS	2

struct mveth_private;

/* Create interface.
 * Allocates resources for descriptor rings and sets up the driver software structure.
 *
 * Arguments:
 *	unit:
 *		interface # (1..2). The interface must not be attached to BSD.
 *
 *  driver_tid:
 *		optional driver task-id (can be retrieved with BSP_mve_get_tid()).
 *		Not used by the low-level driver.
 *
 *  isr(isr_arg):
 *      user ISR.
 *
 *	void (*cleanup_txbuf)(void *user_buf, void *cleanup_txbuf_arg, int error_on_tx_occurred):
 *		Pointer to user-supplied callback to release a buffer that had been sent
 *		by BSP_mve_send_buf() earlier. The callback is passed 'cleanup_txbuf_arg'
 *		and a flag indicating whether the send had been successful.
 *		The driver no longer accesses 'user_buf' after invoking this callback.
 *		CONTEXT: This callback is executed either by BSP_mve_swipe_tx() or
 *		BSP_mve_send_buf(), BSP_mve_init_hw(), BSP_mve_stop_hw() (the latter
 *		ones calling BSP_mve_swipe_tx()).
 *	void *cleanup_txbuf_arg:
 *		Closure argument that is passed on to 'cleanup_txbuf()' callback;
 *
 *	void *(*alloc_rxbuf)(int *p_size, unsigned long *p_data_addr),
 *		Pointer to user-supplied callback to allocate a buffer for subsequent
 *		insertion into the RX ring by the driver.
 *		RETURNS: opaque handle to the buffer (which may be a more complex object
 *				 such as an 'mbuf'). The handle is not used by the driver directly
 *				 but passed back to the 'consume_rxbuf()' callback.
 *				 Size of the available data area and pointer to buffer's data area
 *				 in '*psize' and '*p_data_area', respectively.
 *				 If no buffer is available, this routine should return NULL in which
 *				 case the driver drops the last packet and re-uses the last buffer
 *				 instead of handing it out to 'consume_rxbuf()'.
 *		CONTEXT: Called when initializing the RX ring (BSP_mve_init_hw()) or when
 *				 swiping it (BSP_mve_swipe_rx()).
 *
 *
 *	void (*consume_rxbuf)(void *user_buf, void *consume_rxbuf_arg, int len);
 *		Pointer to user-supplied callback to pass a received buffer back to
 *		the user. The driver no longer accesses the buffer after invoking this
 *		callback (with 'len'>0, see below). 'user_buf' is the buffer handle
 *		previously generated by 'alloc_rxbuf()'.
 *		The callback is passed 'cleanup_rxbuf_arg' and a 'len'
 *		argument giving the number of bytes that were received.
 *		'len' may be <=0 in which case the 'user_buf' argument is NULL.
 *		'len' == 0 means that the last 'alloc_rxbuf()' had failed,
 *		'len' < 0 indicates a receiver error. In both cases, the last packet
 *		was dropped/missed and the last buffer will be re-used by the driver.
 *		NOTE: the data are 'prefixed' with two bytes, i.e., the ethernet packet header
 *		      is stored at offset 2 in the buffer's data area. Also, the FCS (4 bytes)
 *		      is appended. 'len' accounts for both.
 *		CONTEXT: Called from BSP_mve_swipe_rx().
 *	void *cleanup_rxbuf_arg:
 *		Closure argument that is passed on to 'consume_rxbuf()' callback;
 *
 *  rx_ring_size, tx_ring_size:
 *		How many big to make the RX and TX descriptor rings. Note that the sizes
 *		may be 0 in which case a reasonable default will be used.
 *		If either ring size is < 0 then the RX or TX will be disabled.
 *		Note that it is illegal in this case to use BSP_mve_swipe_rx() or
 *		BSP_mve_swipe_tx(), respectively.
 *
 *  irq_mask:
 *		Interrupts to enable. OR of flags from above.
 *
 */

/* Direct assignment of MVE flags to user API relies on irqs and x-irqs not overlapping */
#define MV643XX_ETH_IRQ_RX_DONE						(1<<2)
#define MV643XX_ETH_EXT_IRQ_TX_DONE					(1<<0)
#define MV643XX_ETH_EXT_IRQ_LINK_CHG				(1<<16)

struct mveth_private *
BSP_mve_create(
	int		 unit,
	rtems_id tid,
	void     (*isr)(void*isr_arg),
	void     *isr_arg,
	void (*cleanup_txbuf)(void *user_buf, void *closure, int error_on_tx_occurred),
	void *cleanup_txbuf_arg,
	void *(*alloc_rxbuf)(int *p_size, uintptr_t *p_data_addr),
	void (*consume_rxbuf)(void *user_buf, void *closure, int len),
	void *consume_rxbuf_arg,
	int		rx_ring_size,
	int		tx_ring_size,
	int		irq_mask
);

/*
 * Clear multicast hash filter. No multicast frames are accepted
 * after executing this routine (unless the hardware was initialized
 * in 'promiscuous' mode).
 */
void
BSP_mve_mcast_filter_clear(struct mveth_private *mp);

/*
 * Program multicast filter to accept all multicast frames
 */
void
BSP_mve_mcast_filter_accept_all(struct mveth_private *mp);

/*
 * Add a MAC address to the multicast filter.
 * Existing entries are not changed but note that
 * the filter is imperfect, i.e., multiple MAC addresses
 * may alias to a single filter entry. Hence software
 * filtering must still be performed.
 *
 * If a higher-level driver implements IP multicasting
 * then multiple IP addresses may alias to the same MAC
 * address. This driver maintains a 'reference-count'
 * which is incremented every time the same MAC-address
 * is passed to this routine; the address is only removed
 * from the filter if BSP_mve_mcast_filter_accept_del()
 * is called the same number of times (or by BSP_mve_mcast_filter_clear).
 */
void
BSP_mve_mcast_filter_accept_add(struct mveth_private *mp, unsigned char *enaddr);

/*
 * Remove a MAC address from the multicast filter.
 * This routine decrements the reference count of the given
 * MAC-address and removes it from the filter once the
 * count reaches zero.
 */
void
BSP_mve_mcast_filter_accept_del(struct mveth_private *mp, unsigned char *enaddr);


/* Enable/disable promiscuous mode */
void
BSP_mve_promisc_set(struct mveth_private *mp, int promisc);

/* calls BSP_mve_stop_hw(), releases all resources and marks the interface
 * as unused.
 * RETURNS 0 on success, nonzero on failure.
 * NOTE:   the handle MUST NOT be used after successful execution of this
 *         routine.
 */
int
BSP_mve_detach(struct mveth_private *mp);

/* Enqueue a buffer chain for transmission.
 *
 * RETURN: #bytes sent or -1 if there are not enough descriptors
 *         -2 is returned if the caller should attempt to
 *         repackage the chain into a smaller one.
 *
 * Comments: software cache-flushing incurs a penalty if the
 *           packet cannot be queued since it is flushed anyways.
 *           The algorithm is slightly more efficient in the normal
 *			 case, though.
 */

typedef struct MveEthBufIter {
	void  *hdl;    /* opaque handle for the iterator implementor */
	void  *data;   /* data to be sent                            */
	size_t len;    /* data size (in octets)                      */
	void  *uptr;   /* user-pointer to go into the descriptor;
                      note: cleanup_txbuf is only called for desriptors
	                        where this field is non-NULL (for historical
	                        reasons)                             */
} MveEthBufIter;

typedef MveEthBufIter *(*MveEthBufIterNext)(MveEthBufIter*);

int
BSP_mve_send_buf_chain(struct mveth_private *mp, MveEthBufIterNext next, MveEthBufIter *it);


/* Legacy entry point to send a header + a buffer */
int
BSP_mve_send_buf_raw(struct mveth_private *mp, void *head_p, int h_len, void *data_p, int d_len);

/* Descriptor scavenger; cleanup the TX ring, passing all buffers
 * that have been sent to the cleanup_tx() callback.
 * This routine is called from BSP_mve_send_buf(), BSP_mve_init_hw(),
 * BSP_mve_stop_hw().
 *
 * RETURNS: number of buffers processed.
 */
int
BSP_mve_swipe_tx(struct mveth_private *mp);

/* Retrieve all received buffers from the RX ring, replacing them
 * by fresh ones (obtained from the alloc_rxbuf() callback). The
 * received buffers are passed to consume_rxbuf().
 *
 * RETURNS: number of buffers processed.
 */
int
BSP_mve_swipe_rx(struct mveth_private *mp);

/* read ethernet address from hw to buffer */
void
BSP_mve_read_eaddr(struct mveth_private *mp, unsigned char *oeaddr);

/* Interrupt related routines */

/* Note: the BSP_mve_enable/disable/ack_irqs() entry points
 *       are deprecated.
 *       The newer API where the user passes a mask allows
 *       for more selective control.
 */

/* Enable all supported interrupts at device */
void
BSP_mve_enable_irqs(struct mveth_private *mp);

/* Disable all supported interrupts at device */
void
BSP_mve_disable_irqs(struct mveth_private *mp);

/* Acknowledge (and clear) all supported interrupts.
 * RETURNS: interrupts that were raised.
 */
uint32_t
BSP_mve_ack_irqs(struct mveth_private *mp);

/* Enable interrupts included in 'mask' (leaving
 * already enabled interrupts on). If the mask
 * includes bits that were not passed to the 'setup'
 * routine then the behavior is undefined.
 */
void
BSP_mve_enable_irq_mask(struct mveth_private *mp, uint32_t irq_mask);

/* Disable interrupts included in 'mask' (leaving
 * other ones that are currently enabled on). If the
 * mask includes bits that were not passed to the 'setup'
 * routine then the behavior is undefined.
 *
 * RETURNS: Bitmask of interrupts that were enabled upon entry
 *          into this routine. This can be used to restore the
 *          previous state.
 */
uint32_t
BSP_mve_disable_irq_mask(struct mveth_private *mp, uint32_t irq_mask);

/* Acknowledge and clear selected interrupts.
 *
 * RETURNS: All pending interrupts.
 *
 * NOTE:    Only pending interrupts contained in 'mask'
 *          are cleared. Others are left pending.
 *
 *          This routine can be used to check for pending
 *          interrupts (pass mask ==  0) or to clear all
 *          interrupts (pass mask == -1).
 */
uint32_t
BSP_mve_ack_irq_mask(struct mveth_private *mp, uint32_t mask);

/* Retrieve the driver daemon TID that was passed to
 * BSP_mve_setup().
 */

rtems_id
BSP_mve_get_tid(struct mveth_private *mp);

/* Dump statistics to file (stdout if NULL)
 *
 * NOTE: this routine is not thread safe
 */
void
BSP_mve_dump_stats(struct mveth_private *mp, FILE *f);

#define MV643XX_MEDIA_LINK (1<<0)
#define MV643XX_MEDIA_FD   (1<<1)
#define MV643XX_MEDIA_10   (1<<8)
#define MV643XX_MEDIA_100  (2<<8)
#define MV643XX_MEDIA_1000 (3<<8)
#define MV643XX_MEDIA_SPEED_MSK (0xff00)

/*
 * Initialize interface hardware
 *
 * 'mp'			handle obtained by from BSP_mve_setup().
 * 'promisc'	whether to set promiscuous flag.
 * 'enaddr'		pointer to six bytes with MAC address. Read
 *				from the device if NULL.
 * 'speed'      current speed and link status as read from the PHY.
 *
 * Note:        Multicast filters are cleared by this routine.
 *              However, in promiscuous mode the mcast filters
 *              are programmed to accept all multicast frames.
 */
void
BSP_mve_init_hw(struct mveth_private *mp, int promisc, unsigned char *enaddr, int speed);

/*
 * Update the serial port to a new speed (e.g., result of a PHY IRQ)
 */
void
BSP_mve_update_serial_port(struct mveth_private *mp, int speed);

/*
 * Shutdown hardware and clean out the rings
 */
void
BSP_mve_stop_hw(struct mveth_private *mp);

unsigned
BSP_mve_mii_read(struct mveth_private *mp, unsigned addr);

unsigned
BSP_mve_mii_write(struct mveth_private *mp, unsigned addr, unsigned v);

unsigned
BSP_mve_mii_read_early(int port, unsigned addr);

unsigned
BSP_mve_mii_write_early(int port, unsigned addr, unsigned v);

#ifdef __cplusplus
};
#endif

#endif
