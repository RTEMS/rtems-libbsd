/* RTEMS driver for the mv643xx gigabit ethernet chip */

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
 * 	   Stanford Linear Accelerator Center, Stanford University.
 *
 * Acknowledgement of sponsorship
 * ------------------------------
 * The 'mv643xx ethernet driver for RTEMS' was produced by
 *     the Stanford Linear Accelerator Center, Stanford University,
 * 	   under Contract DE-AC03-76SFO0515 with the Department of Energy.
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

/* Nexus port by Till Straumann, <till.straumann@psi.ch>, 3/2021 */

#include <machine/rtems-bsd-kernel-space.h>
#include <bsp.h>

#ifdef LIBBSP_BEATNIK_BSP_H

#include <sys/types.h>
#include <sys/kernel.h>
#include <sys/bus.h>
#include <sys/module.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/sockio.h>
#include <net/if.h>
#include <net/if_types.h>
#include <net/if_var.h>
#include <net/if_dl.h>
#include <net/if_media.h>
#include <net/ethernet.h>
#include <dev/mii/mii.h>
#include <dev/mii/miivar.h>
#include <rtems/bsd/local/miibus_if.h>
#include <stdio.h>
#include <bsp/mv643xx_eth.h>

#define DRVNAME "mv63xx_nexus"

#undef  MVETH_DEBUG

/* Define default ring sizes */

#undef  MVETH_TESTING

#ifdef  MVETH_TESTING
/* hard and small defaults */
#define MV643XX_RX_RING_SIZE	2
#define MV643XX_TX_QUEUE_SIZE   4
#define MV643XX_BD_PER_PACKET   1
#define TX_LOWWATER             1

#else /* MVETH_TESTING */

#define MV643XX_RX_RING_SIZE	40	/* attached buffers are always 2k clusters, i.e., this
									 * driver - with a configured ring size of 40 - constantly
									 * locks 80k of cluster memory - your app config better
									 * provides enough space!
									 */
#define MV643XX_TX_QUEUE_SIZE   40
#define MV643XX_BD_PER_PACKET   10
#define TX_LOWWATER             (4*(MV643XX_BD_PER_PACKET))
#endif /* MVETH_TESTING */

/* NOTE: tx ring size MUST be > max. # of fragments / mbufs in a chain;
 *		 I observed chains of >17 entries regularly!
 */
#define MV643XX_TX_RING_SIZE	((MV643XX_TX_QUEUE_SIZE) * (MV643XX_BD_PER_PACKET))

/* The chip puts the ethernet header at offset 2 into the buffer so
 * that the payload is aligned
 */
#define ETH_RX_OFFSET								2
#define ETH_CRC_LEN									4	/* strip FCS at end of packet */

#ifndef __PPC__
#error	"Dont' know how to deal with cache on this CPU architecture"
#endif

/* Ring entries are 32 bytes; coherency-critical chunks are 16 -> software coherency
 * management works for cache line sizes of 16 and 32 bytes only. If the line size
 * is bigger, the descriptors could be padded...
 */
#if !defined(PPC_CACHE_ALIGNMENT)
#error "PPC_CACHE_ALIGNMENT not defined"
#elif PPC_CACHE_ALIGMENT != 16 && PPC_CACHE_ALIGNMENT != 32
#error "Cache line size must be 16 or 32"
#else
#define RX_BUF_ALIGNMENT			PPC_CACHE_ALIGNMENT
#endif

/* HELPER MACROS */

/* Align base to alignment 'a' */
#define MV643XX_ALIGN(b, a)	((((uint32_t)(b)) + (a)-1) & (~((a)-1)))


#define IRQ_EVENT RTEMS_EVENT_0
#define TX_EVENT  RTEMS_EVENT_1

/* Hacks -- FIXME */
rtems_id
rtems_bsdnet_newproc (char *name, int stacksize, void(*entry)(void *), void *arg);
#define SIO_RTEMS_SHOW_STATS _IO('i', 250)

#define	MVE643XX_DUMMY_PHY 0 /* phy is defined by low-level driver */

struct mve_enet_softc {
	device_t                  dev;
	struct ifnet             *ifp;
	device_t                  miibus;
	struct mii_data          *mii_softc;
	struct mveth_private     *mp;
    struct mtx                mtx;
	struct callout            wdCallout;
	rtems_id                  daemonTid;
	int                       oif_flags;
};

static struct mve_enet_softc * ifaces[MV643XXETH_NUM_DRIVER_SLOTS] = { 0 };

typedef struct MveMbufIter {
	MveEthBufIter it;
	struct mbuf  *next;
	struct mbuf  *head;
} MveMbufIter;

/* Forward Declarations */
struct mve_enet_softc;

static void
mve_media_status(struct ifnet *ifp, struct ifmediareq *ifmr);

static int
mve_media_change(struct ifnet *ifp);

static void
mve_set_filters(struct ifnet *ifp);

static int
xlateMediaFlags(const struct mii_data *mid);

static void
mve_ack_link_change(struct mve_enet_softc *sc);

static __inline__ void
mve_send_event(struct mve_enet_softc *sc, rtems_event_set ev)
{
rtems_status_code st;
	if ( RTEMS_SUCCESSFUL != (st = rtems_event_send(sc->daemonTid, ev)) ) {
		printk(DRVNAME": rtems_event_send returned 0x%08x (TID: 0x%08x, sc: 0x%08x)\n", st, sc->daemonTid, sc);
		rtems_panic(DRVNAME": rtems_event_send() failed!\n");
	}
}

static __inline__ void
mve_lock(struct mve_enet_softc *sc, const char *from)
{
	mtx_lock( & sc->mtx );
/*printk("L V %s\n", from);*/
}

static __inline__ void
mve_unlock(struct mve_enet_softc *sc, const char *from)
{
/*printk("L ^ %s\n", from);*/
	mtx_unlock( & sc->mtx );
}

static int
mve_probe(device_t dev)
{
	int unit = device_get_unit(dev);
	int err;

#ifdef MVETH_DEBUG
	printk(DRVNAME": mve_probe (entering)\n");
#endif

	if ( unit >= 0 && unit < MV643XXETH_NUM_DRIVER_SLOTS ) {
		err = BUS_PROBE_DEFAULT;
	} else {
		err = ENXIO;
	}

	return err;
}

/*
 * starting at 'm' scan the buffer chain until we
 * find a non-empty buffer (which we return)
 */
static __inline__ struct mbuf *
skipEmpty(struct mbuf *m)
{
	while ( m && ( 0 == m->m_len ) ) {
		m = m->m_next;
	}
	return m;
}

/*
 * Record a buffer's info in the low-leve driver 'iterator' struct.
 * Also scan ahead to find the next non-empty buffer (store it in
 * the iterator's 'next' field). This info is needed because we
 * want to know if 'this' buffer is the last (non-empty!) one
 * in a chain.
 *
 * On entry 'it->next' identifies 'this' buffer and on return
 * 'it->next' points to the next non-empty buffer.
 */
static MveEthBufIter *
nextBuf(MveEthBufIter *arg)
{
MveMbufIter *it = (MveMbufIter*)arg;
struct mbuf *m;
	/* If 'this' buffer is non-null */
	if ( (m = it->next) ) {
		/* find next non-empty buffer */
		it->next    = skipEmpty( m->m_next );
		/* record 'this' buffer's info */
		it->it.data = mtod(m, void*);
		it->it.len  = m->m_len;
		/* if there is a non-empty buffer after 'this' uptr is NULL
		 * if this is tha last buffer in a chain then record the
		 * head of the chain in the uptr (for eventual cleanup
		 * by release_tx_mbuf()).
		 */
		it->it.uptr = it->next ? 0 : it->head;
		return (MveEthBufIter*)it;
	}
	return 0;
}

/*
 * Initialize the iterator struct
 */
static MveEthBufIter *
initIter(MveMbufIter *it, struct mbuf *m)
{
	/* record the head of the chain */
	it->head = m;
	/* initialize 'next' field to the first non-empty buffer.
     * This may be NULL if the chain is entirely empty but
	 * that is handled correctly.
	 */
	it->next = skipEmpty( m );
	/* Fill with first buf info */
	return nextBuf( &it->it );
}

static int
mve_send_mbuf( struct mve_enet_softc *sc, struct mbuf *m_head )
{
MveMbufIter iter;
int         rval;

	if ( ! m_head ) {
		return 0;
	}

	if ( ! initIter( &iter, m_head ) ) {
		/* completely empty chain */
		m_freem( m_head );
		return 0;
	}

	rval = BSP_mve_send_buf_chain( sc->mp, nextBuf, &iter.it );

	return rval;
}

static void
mve_isr(void *closure)
{
struct mve_enet_softc *sc = (struct mve_enet_softc*)closure;
#ifdef MVETH_DEBUG
	printk(DRVNAME": mve_isr; posting event to %x\n", sc->daemonTid);
#endif
	BSP_mve_disable_irqs( sc->mp );
	mve_send_event( sc, IRQ_EVENT );
}

static void
mve_stop(struct mve_enet_softc *sc)
{
	BSP_mve_stop_hw( sc->mp );
	/* clear IF flags */
	if_setdrvflagbits(sc->ifp, 0, (IFF_DRV_OACTIVE | IFF_DRV_RUNNING));
}

static void
mve_set_filters(struct ifnet *ifp)
{
struct mve_enet_softc *sc = (struct mve_enet_softc*) if_getsoftc( ifp );
int                   iff = if_getflags(ifp);
struct ifmultiaddr   *ifma;
unsigned char        *lladdr;

	BSP_mve_promisc_set( sc->mp, !!(iff & IFF_PROMISC));

	if ( iff & (IFF_PROMISC | IFF_ALLMULTI) ) {
		BSP_mve_mcast_filter_accept_all(sc->mp);
	} else {
		BSP_mve_mcast_filter_clear( sc->mp );

		if_maddr_rlock( ifp );

		CK_STAILQ_FOREACH( ifma, &ifp->if_multiaddrs, ifma_link ) {

			if ( ifma->ifma_addr->sa_family != AF_LINK ) {
				continue;
			}

			lladdr = LLADDR((struct sockaddr_dl *) ifma->ifma_addr);

			BSP_mve_mcast_filter_accept_add( sc->mp, lladdr );

		}

		if_maddr_runlock( ifp );
	}
}

/* Daemon task does all the 'interrupt' work */
static void
mve_daemon(void *arg)
{
struct mve_enet_softc   *sc  = (struct mve_enet_softc*) arg;
struct ifnet		    *ifp = sc->ifp;
rtems_event_set		     evs;
struct mbuf             *m;
int                      avail;
int                      sndStat;
uint32_t                 irqstat;

#ifdef MVETH_DEBUG
	printk(DRVNAME": bsdnet mveth_daemon started\n");
#endif

	mve_lock( sc, "daemon" );

	for (;;) {

		mve_unlock( sc, "daemon" );
		if ( RTEMS_SUCCESSFUL != rtems_event_receive( (IRQ_EVENT | TX_EVENT), (RTEMS_WAIT | RTEMS_EVENT_ANY), RTEMS_NO_TIMEOUT, &evs ) ) {
			rtems_panic(DRVNAME": rtems_event_receive() failed!\n");
		}
		mve_lock( sc, "daemon" );

#ifdef MVETH_DEBUG
		printk(DRVNAME": bsdnet mveth_daemon event received 0x%x\n", evs);
#endif

		if ( !(if_getflags(ifp) & IFF_UP) ) {
			mve_stop(sc);
			/* clear flag */
			if_setdrvflagbits(sc->ifp, 0, IFF_DRV_RUNNING);
			continue;
		}

		if ( ! (if_getdrvflags(ifp) & IFF_DRV_RUNNING) ) {
			/* event could have been pending at the time hw was stopped;
			 * just ignore...
			 */
			continue;
		}

		if ( (evs & IRQ_EVENT) ) {
			irqstat = BSP_mve_ack_irqs(sc->mp);
		} else {
			irqstat = 0;
		}

		if ( (MV643XX_ETH_EXT_IRQ_LINK_CHG & irqstat) && sc->mii_softc ) {
			/* phy status changed */
			mii_pollstat( sc->mii_softc );
		}

		/* free tx chain and send */
		if ( (evs & TX_EVENT) || (MV643XX_ETH_EXT_IRQ_TX_DONE & irqstat)  ) {
			while ( (avail = BSP_mve_swipe_tx( sc->mp )) > TX_LOWWATER ) {
				IF_DEQUEUE( &ifp->if_snd, m );
				if ( ! m ) {
					/* clear active bit */
					if_setdrvflagbits(ifp, 0, IFF_DRV_OACTIVE);
					break;
				}
				sndStat = mve_send_mbuf( sc, m );
				if ( sndStat < 0 ) {
					/* maybe not enough space right now; requeue and wait for next IRQ */
					IF_PREPEND( &ifp->if_snd, m );
					break;
				}
			}
		}
		if ( (MV643XX_ETH_IRQ_RX_DONE & irqstat) ) {
			BSP_mve_swipe_rx(sc->mp);
		}

		BSP_mve_enable_irqs(sc->mp);
	}

	mve_unlock( sc, "daemon (xit)" );
}

static void
release_tx_mbuf(void *user_buf, void *closure, int error_on_tx_occurred)
{
struct mve_enet_softc  *sc = (struct mve_enet_softc*)closure;
struct ifnet          *ifp = sc->ifp;
struct mbuf            *mb = (struct mbuf*)user_buf;

	if ( error_on_tx_occurred ) {
		if_inc_counter( ifp, IFCOUNTER_OERRORS,               1 );
	} else {
		if_inc_counter( ifp, IFCOUNTER_OPACKETS,              1 );
		if_inc_counter( ifp, IFCOUNTER_OBYTES, mb->m_pkthdr.len );
	}
	m_freem(mb);
}

static void *
alloc_rx_mbuf(int *p_size, uintptr_t *p_data)
{
struct mbuf		*m;
unsigned long	l,o;

	m = m_getcl(M_NOWAIT, MT_DATA, M_PKTHDR);

	if ( m ) {

		o = mtod(m, unsigned long);
		l = MV643XX_ALIGN(o, RX_BUF_ALIGNMENT) - o;

		/* align start of buffer */
		m->m_data += l;

		/* reduced length */
		l = MCLBYTES - l;

		m->m_len   = m->m_pkthdr.len = l;
		*p_size    = m->m_len;
		*p_data    = mtod(m, uintptr_t);
	}

	return (void*) m;
}


static void
consume_rx_mbuf(void *user_buf, void *closure, int len)
{
struct mve_enet_softc *sc  = (struct mve_enet_softc*)closure;
struct ifnet          *ifp = sc->ifp;
struct mbuf           *m   = (struct mbuf*)user_buf;

	if ( len <= 0 ) {
		if_inc_counter( ifp, IFCOUNTER_IQDROPS, 1 );
		if ( len < 0 ) {
			if_inc_counter( ifp, IFCOUNTER_IERRORS, 1 );
		}
		m_freem(m);
	} else {
			m->m_len	      = m->m_pkthdr.len = len - ETH_RX_OFFSET - ETH_CRC_LEN;
			m->m_data        += ETH_RX_OFFSET;
			m->m_pkthdr.rcvif = ifp;

			if_inc_counter( ifp, IFCOUNTER_IPACKETS,               1 );
			if_inc_counter( ifp, IFCOUNTER_IBYTES,   m->m_pkthdr.len );

			if (0) {
				/* Low-level debugging */
				int i;
				for (i=0; i<m->m_len; i++) {
					if ( !(i&15) )
						printk("\n");
					printk("0x%02x ",mtod(m,char*)[i]);
				}
				printk("\n");
			}

			mve_unlock( sc, "rx_cleanup" );
			(*ifp->if_input)(ifp, m);
			mve_lock( sc, "rx_cleanup" );
	}
}

/* Translate IFFLAGS to low-level driver representation */
static int
xlateMediaFlags(const struct mii_data *mid)
{
int lowLevelFlags = 0;
int msk           = IFM_AVALID | IFM_ACTIVE;

	if ( (mid->mii_media_status & msk) == msk ) {
		lowLevelFlags |= MV643XX_MEDIA_LINK;

		if ( IFM_OPTIONS( mid->mii_media_active ) & IFM_FDX ) {
			lowLevelFlags |= MV643XX_MEDIA_FD;
		}

		switch ( IFM_ETHER_SUBTYPE_GET( mid->mii_media_active ) ) {
			default:
#ifdef MVETH_DEBUG
				printk(DRVNAME"xlateMediaFlags: UNKNOWN SPEED\n");
#endif
				break; /* UNKNOWN -- FIXME */
			case IFM_10_T:
#ifdef MVETH_DEBUG
				printk(DRVNAME"xlateMediaFlags: 10baseT\n");
#endif
				lowLevelFlags |= MV643XX_MEDIA_10;
				break;
			case IFM_100_TX:
#ifdef MVETH_DEBUG
				printk(DRVNAME"xlateMediaFlags: 100baseT\n");
#endif
				lowLevelFlags |= MV643XX_MEDIA_100;
				break;
			case IFM_1000_T:
#ifdef MVETH_DEBUG
				printk(DRVNAME"xlateMediaFlags: 1000baseT\n");
#endif
				lowLevelFlags |= MV643XX_MEDIA_1000;
				break;
		}
	} else {
#ifdef MVETH_DEBUG
				printk(DRVNAME"xlateMediaFlags: NO LINK\n");
#endif
	}
	return lowLevelFlags;
}

static void
mve_init(void *arg)
{
struct mve_enet_softc  *sc                 = (struct mve_enet_softc*)arg;
struct ifnet           *ifp                = sc->ifp;
int                    lowLevelMediaStatus = 0;
int                    promisc;

#ifdef MVETH_DEBUG
	printk(DRVNAME": mve_init (entering)\n");
#endif

	if ( sc->mii_softc ) {
		mii_pollstat( sc->mii_softc );
		lowLevelMediaStatus = xlateMediaFlags( sc->mii_softc );
		if ( (lowLevelMediaStatus & MV643XX_MEDIA_LINK) ) {
			if_setdrvflagbits(ifp, 0, IFF_DRV_OACTIVE);
		} else {
			if_setdrvflagbits(ifp, IFF_DRV_OACTIVE, 0);
		}
	}

	promisc = !! (if_getdrvflags(ifp) & IFF_PROMISC);

	BSP_mve_init_hw(sc->mp, promisc, if_getlladdr(ifp), lowLevelMediaStatus);

	/* if promiscuous then there is no need to change */
	if ( ! promisc ) {
		mve_set_filters(ifp);
	}


	if_setdrvflagbits(ifp, IFF_DRV_RUNNING, 0);
}

static void
mve_start(struct ifnet *ifp)
{
struct mve_enet_softc *sc = (struct mve_enet_softc*) if_getsoftc( ifp );
	mve_lock( sc, "mve_start" );
		if_setdrvflagbits(ifp, IFF_DRV_OACTIVE, 0);
	mve_unlock( sc, "mve_start" );
	mve_send_event( sc, TX_EVENT );
}

static int
mve_ioctl(struct ifnet *ifp, ioctl_command_t cmd, caddr_t data)
{
struct mve_enet_softc *sc = (struct mve_enet_softc*) if_getsoftc( ifp );
struct ifreq          *ifr = (struct ifreq *)data;
int                    err = 0;
int                      f, df;

#ifdef MVETH_DEBUG
	printk(DRVNAME": mve_ioctl (entering)\n");
#endif

	mve_lock( sc, "mve_ioctl" );

	switch ( cmd ) {
  		case SIOCSIFFLAGS:
			f  = if_getflags( ifp );
			df = if_getdrvflags( ifp );
			if ( (f & IFF_UP) ) {
				if ( ! ( df & IFF_DRV_RUNNING ) ) {
					mve_init( (void*)sc );
				} else {
					if ( (f & IFF_PROMISC) != (sc->oif_flags & IFF_PROMISC) ) {
						mve_set_filters(ifp);
					}
					/* FIXME: other flag changes are ignored/unimplemented */
				}
			} else {
				if ( df & IFF_DRV_RUNNING ) {
					mve_stop(sc);
				}
			}
			sc->oif_flags = f;
		break;

  		case SIOCGIFMEDIA:
  		case SIOCSIFMEDIA:
			if ( sc->mii_softc ) {
				err = ifmedia_ioctl( ifp, ifr, &sc->mii_softc->mii_media, cmd );
			} else {
				err = EINVAL;
			}
		break;

		case SIOCADDMULTI:
		case SIOCDELMULTI:
			if ( if_getdrvflags( ifp ) & IFF_DRV_RUNNING ) {
				mve_set_filters(ifp);
			}
		break;

 		case SIO_RTEMS_SHOW_STATS:
			BSP_mve_dump_stats(sc->mp, stdout);
		break;

		default:
			err = ether_ioctl(ifp, cmd, data);
		break;
	}

	mve_unlock( sc, "mve_ioctl" );

	return err;
}

/* SIO RTEMS_SHOW_STATS is too cumbersome to use -- for debugging, provide direct hack */
int
mv643xx_nexus_dump_stats(int unit, FILE *f)
{
	if ( unit < 0 || unit >= MV643XXETH_NUM_DRIVER_SLOTS || ! ifaces[unit] )
		return -EINVAL;
	if ( ! f )
		f = stdout;
	BSP_mve_dump_stats(ifaces[unit]->mp, f);
	return 0;
}

/*
 * Used to update speed settings in the hardware
 * when the phy setup changes.
 *
 * ASSUME: caller holds lock
 */
static void
mve_ack_link_change(struct mve_enet_softc *sc)
{
struct mii_data *mii                 = sc->mii_softc;
int              lowLevelMediaStatus;

	if ( !mii )
		return;

	lowLevelMediaStatus = xlateMediaFlags( mii );

	if ( (lowLevelMediaStatus & MV643XX_MEDIA_LINK) ) {
		BSP_mve_update_serial_port( sc->mp, lowLevelMediaStatus );
		if_setdrvflagbits( sc->ifp, 0, IFF_DRV_OACTIVE );
        mve_start( sc->ifp );
	} else {
		if_setdrvflagbits( sc->ifp, IFF_DRV_OACTIVE, 0 );
	}
}

/* Callback from ifmedia_ioctl()
 *
 * Caller probably holds the lock already but
 * since it is recursive we may as well make sure
 * in case there are other possible execution paths.
 */
static int
mve_media_change(struct ifnet *ifp)
{
struct mve_enet_softc *sc   = (struct mve_enet_softc*) if_getsoftc( ifp );
struct mii_data        *mii = sc->mii_softc;
int                     err;

#ifdef MVETH_DEBUG
	printk(DRVNAME": mve_media_change\n");
#endif

	if ( ! mii ) {
		return ENXIO;
	}

	err = mii_mediachg( mii );

	return err;
}

static void
mve_media_status(struct ifnet *ifp, struct ifmediareq *ifmr)
{
struct mve_enet_softc *sc   = (struct mve_enet_softc*) if_getsoftc( ifp );
struct mii_data        *mii = sc->mii_softc;

#ifdef MVETH_DEBUG
	printk(DRVNAME": mve_media_status\n");
#endif

	if ( mii ) {
		mii_pollstat( mii );
		ifmr->ifm_active = mii->mii_media_active;
		ifmr->ifm_status = mii->mii_media_status;
	}
}

static int
mve_attach(device_t dev)
{
struct mve_enet_softc  *sc;
struct ifnet           *ifp;
uint8_t                 hwaddr[ETHER_ADDR_LEN];
struct mveth_private   *mp;
int                     unit         = device_get_unit(dev);
int                     tx_ring_size = MV643XX_TX_RING_SIZE;
int                     rx_ring_size = MV643XX_RX_RING_SIZE;
int                     tx_q_size    = MV643XX_TX_QUEUE_SIZE;

	sc            = device_get_softc( dev );
	sc->dev       = dev;
	sc->ifp       = ifp = if_alloc(IFT_ETHER);
	sc->daemonTid = 0;
	sc->mii_softc = 0;

#ifdef MVETH_DEBUG
	printk(DRVNAME": mve_attach (entering)\n");
#endif

	mtx_init( &sc->mtx, device_get_nameunit( sc->dev ), MTX_NETWORK_LOCK, MTX_RECURSE );
	callout_init_mtx( &sc->wdCallout, &sc->mtx, 0 );

	if_setsoftc     ( ifp, sc        );
	if_initname     ( ifp, device_get_name(dev), unit);
	if_setinitfn    ( ifp, mve_init  );
	if_setioctlfn   ( ifp, mve_ioctl );
	if_setstartfn   ( ifp, mve_start );
	if_setflags     ( ifp, (IFF_BROADCAST | IFF_MULTICAST | IFF_SIMPLEX) );
	sc->oif_flags = if_getflags( ifp );

	if_setsendqlen  ( ifp, tx_q_size );
	if_setsendqready( ifp );

	mp = BSP_mve_create(
	        unit + 1, /* low-level driver' unit numbers are 1-based */
	        0,
	        mve_isr, (void*)sc,
	        release_tx_mbuf, (void*)sc,
	        alloc_rx_mbuf,
	        consume_rx_mbuf, (void*)sc,
	        rx_ring_size,
	        tx_ring_size,
	        (   MV643XX_ETH_IRQ_RX_DONE
	          | MV643XX_ETH_EXT_IRQ_TX_DONE
	          | MV643XX_ETH_EXT_IRQ_LINK_CHG));

	if ( ! mp ) {
		rtems_panic("Unable to create mv643xx low-level driver");
	}

	sc->mp = mp;

	BSP_mve_read_eaddr( mp, hwaddr );

	if ( 0 == mii_attach( sc->dev,
	                     &sc->miibus,
	                      ifp,
	                      mve_media_change,
	                      mve_media_status,
	                      BMSR_DEFCAPMASK,
 	                      MVE643XX_DUMMY_PHY,
 	                      MII_OFFSET_ANY,
	                      0 ) ) {
		sc->mii_softc = device_get_softc( sc->miibus );
	}

	sc->daemonTid = rtems_bsdnet_newproc("MVE", 4096, mve_daemon, (void*)sc);

	ether_ifattach( ifp, hwaddr );

#ifdef MVETH_DEBUG
	printk(DRVNAME": mve_attach (leaving)\n");
#endif

	ifaces[unit] = sc;

	return 0;
}

static int
mve_miibus_read_reg(device_t dev, int phy, int reg)
{
struct mve_enet_softc  *sc = (struct mve_enet_softc*) device_get_softc(dev);

	/* low-level driver knows what phy to use; ignore arg */
	return (int) BSP_mve_mii_read( sc->mp, reg );
}

static int
mve_miibus_write_reg(device_t dev, int phy, int reg, int val)
{
struct mve_enet_softc  *sc = (struct mve_enet_softc*) device_get_softc(dev);

	/* low-level driver knows what phy to use; ignore arg */
	BSP_mve_mii_write( sc->mp, reg, val );
	return 0;
}

static void
mve_miibus_statchg(device_t dev)
{
struct mve_enet_softc  *sc = (struct mve_enet_softc*) device_get_softc(dev);
#ifdef MVETH_DEBUG
	printk(DRVNAME": mve_miibus_statchg\n");
#endif
	/* assume this ends up being called either from the ioctl or the driver
	 * task -- either of which holds the lock.
	 */
	mve_ack_link_change( sc );
}

static void
mve_miibus_linkchg(device_t dev)
{
struct mve_enet_softc  *sc = (struct mve_enet_softc*) device_get_softc(dev);
#ifdef MVETH_DEBUG
	printk(DRVNAME": mve_miibus_linkchg\n");
#endif
	/* assume this ends up being called either from the ioctl or the driver
	 * task -- either of which holds the lock.
	 */
	mve_ack_link_change( sc );
}


static device_method_t mve_methods[] = {
	DEVMETHOD(device_probe,    mve_probe ),
	DEVMETHOD(device_attach,   mve_attach),

	DEVMETHOD(miibus_readreg,  mve_miibus_read_reg ),
	DEVMETHOD(miibus_writereg, mve_miibus_write_reg),
	DEVMETHOD(miibus_statchg , mve_miibus_statchg  ),
	DEVMETHOD(miibus_linkchg , mve_miibus_linkchg  ),

	DEVMETHOD_END
};

static driver_t mve_nexus_driver = {
	"mve",
	mve_methods,
	sizeof( struct mve_enet_softc )
};

static devclass_t mve_devclass;

DRIVER_MODULE(mve, nexus, mve_nexus_driver, mve_devclass, 0, 0);
DRIVER_MODULE(miibus, mve, miibus_driver,   miibus_devclass, 0, 0);

MODULE_DEPEND(mve, nexus, 1, 1, 1);
MODULE_DEPEND(mve, ether, 1, 1, 1);
#endif
