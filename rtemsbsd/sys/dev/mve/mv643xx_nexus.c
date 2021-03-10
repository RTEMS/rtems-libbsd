#include <machine/rtems-bsd-kernel-space.h>
#include <bsp.h>

#ifdef LIBBSP_BEATNIK_BSP_H

#include <sys/kernel.h>
#include <sys/bus.h>
#include <sys/module.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/sockio.h>
#include <net/if.h>
#include <net/if_types.h>
#include <net/if_var.h>
#include <net/ethernet.h>
#include <stdio.h>
#include <bsp/mv643xx_eth.h>

/* Define default ring sizes */

#ifdef MVETH_TESTING

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
 *       in 'TESTING' mode, special code is compiled in to repackage
 *		 chains that are longer than the ring size. Normally, this is
 *		 disabled for sake of speed.
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

/* Hack -- FIXME */
#define SIO_RTEMS_SHOW_STATS _IO('i', 250)


struct mve_enet_softc {
	device_t                  dev;
	struct ifnet             *ifp;
	struct mveth_private     *mp;
    struct mtx                mtx;
	struct callout            wdCallout;
	rtems_id                  daemonTid;
	int                       oif_flags;
};

typedef struct MveMbufIter {
	MveEthBufIter it;
	struct mbuf  *next;
	struct mbuf  *head;
} MveMbufIter;

static __inline__ void
mve_send_event(struct mve_enet_softc *sc, rtems_event_set ev)
{
	rtems_event_send(sc->daemonTid, ev);
}

static __inline__ void
mve_lock(struct mve_enet_softc *sc)
{
	mtx_lock( & sc->mtx );
}

static __inline__ void
mve_unlock(struct mve_enet_softc *sc)
{
	mtx_unlock( & sc->mtx );
}

static int
mve_probe(device_t dev)
{
	int unit = device_get_unit(dev);
	int err;

	if ( unit >= 0 && unit < MV643XXETH_NUM_DRIVER_SLOTS ) {
		err = BUS_PROBE_DEFAULT;
	} else { 
		err = ENXIO;
	}

	return err;
}

/* allocate a new cluster and copy an existing chain there;
 * old chain is released...
 */
static struct mbuf *
repackage_chain(struct mbuf *m_head)
{
struct mbuf *m;

	m = m_getcl(M_NOWAIT, MT_DATA, M_PKTHDR);

	if ( m ) {
		m_copydata(m_head, 0, MCLBYTES, mtod(m, caddr_t));
		m->m_pkthdr.len = m->m_len = m_head->m_pkthdr.len;
	}

	m_freem(m_head);
	return m;
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

startover:

	if ( ! m_head ) {
		return 0;
	}

	if ( ! initIter( &iter, m_head ) ) {
		/* completely empty chain */
		m_freem( m_head );
		return 0;
	}

	rval = BSP_mve_send_buf_chain( sc->mp, nextBuf, &iter.it );

	if ( -2 == rval ) {
		/* would never fit (too many fragments) */
		m_head = repackage_chain( m_head );
		goto startover;
	}

	return rval;
}

static void
mve_isr(void *closure)
{
struct mve_enet_softc *sc = (struct mve_enet_softc*)closure;
	BSP_mve_disable_irqs( sc->mp );
	mve_send_event( sc->daemonTid, IRQ_EVENT );
}

static void
mveth_stop(struct mve_enet_softc *sc)
{
	BSP_mve_stop_hw( sc->mp );
	/* clear IF flags */
	if_setdrvflagbits(sc->ifp, 0, (IFF_DRV_OACTIVE | IFF_DRV_RUNNING));
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
	sleep(1);
	printk(DRVNAME": bsdnet mveth_daemon started\n");
#endif

	mve_lock( sc );

	for (;;) {

		mve_unlock( sc );
		rtems_event_receive( IRQ_EVENT | TX_EVENT, RTEMS_WAIT | RTEMS_EVENT_ANY, RTEMS_NO_TIMEOUT, &evs );
		mve_lock( sc );

#ifdef MVETH_DEBUG
		printk(DRVNAME": bsdnet mveth_daemon event received 0x%x\n", evs);
#endif

		if ( !(if_getflags(ifp) & IFF_UP) ) {
			mveth_stop(sc);
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

#warning TODO_
#if 0
		if ( (MV643XX_ETH_EXT_IRQ_LINK_CHG & irqstat) ) {
			/* phy status changed */
			int media;

			if ( 0 == BSP_mve_ack_link_chg(sc->pvt, &media) ) {
				if ( IFM_LINK_OK & media ) {
					/* stop sending */
					if_setdrvflagbits(ifp, 0, IFF_DRV_OACTIVE);
					mveth_start(ifp);
				} else {
					if_setdrvflagbits(ifp, IFF_DRV_OACTIVE, 0);
				}
			}
		}
#endif
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

	mve_unlock( sc );
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
						printf("\n");
					printf("0x%02x ",mtod(m,char*)[i]);
				}
				printf("\n");
			}

			mve_unlock( sc );
			(*ifp->if_input)(ifp, m);
			mve_lock( sc );
	}
}

static void
mve_init(void *arg)
{
struct mve_enet_softc *sc                  = (struct mve_enet_softc*)arg;
struct ifnet		  *ifp                 = sc->ifp;
int                    lowLevelMediaStatus = 0;
int                    promisc; 

	mve_lock( sc );

#ifdef TODO__
int                    media;

	media = IFM_MAKEWORD(0, 0, 0, 0);
	if ( 0 == BSP_mve_media_ioctl(sc->pvt, SIOCGIFMEDIA, &media) ) {
	    if ( (IFM_LINK_OK & media) ) {
			if_setdrvflagbits(ifp, 0, IFF_DRV_OACTIVE);
		} else {
			if_setdrvflagbits(ifp, IFF_DRV_OACTIVE, 0);
		}
	}

	lowLevelMediaStatus = xlateMediaFlags( media );
#endif

	promisc = !! (if_getdrvflags(ifp) & IFF_PROMISC);

	BSP_mve_init_hw(sc->mp, promisc, if_getlladdr(ifp), lowLevelMediaStatus);

	/* if promiscuous then there is no need to change */
	if ( ! promisc ) {
		mveth_set_filters(ifp);
	}

	if ( ! sc->daemonTid ) {
		sc->daemonTid = rtems_bsd_newproc("MVE", 4096, mve_daemon, (void*)sc);
	}

	if_setdrvflagbits(ifp, IFF_DRV_RUNNING, 0);

	mve_unlock( sc );
}

static void
mve_start(struct ifnet *ifp)
{
struct mve_enet_softc *sc  = (struct mve_enet_softc*)ifp->if_softc;
	mve_lock( sc );
		if_setdrvflagbits(ifp, IFF_DRV_OACTIVE, 0);
	mve_unlock( sc );
	mve_send_event( sc, TX_EVENT );
}

static int
mve_ioctl(struct ifnet *ifp, ioctl_command_t cmd, caddr_t data)
{
struct mve_enet_softc  *sc = (struct mve_enet_softc*)ifp->if_softc;
struct ifreq          *ifr = (struct ifreq *)data;
int                    err = 0;
int                      f, df;

	mve_lock( sc );

	switch ( cmd ) {
  		case SIOCSIFFLAGS:
			f  = if_getflags( ifp );
			df = if_getdrvflags( ifp );
			if ( (f & IFF_UP) ) {
				if ( ! ( df & IFF_DRV_RUNNING ) ) {
					mveth_init(sc);
				} else {
					if ( (f & IFF_PROMISC) != (sc->oif_flags & IFF_PROMISC) ) {
						mveth_set_filters(ifp);
					}
					/* FIXME: other flag changes are ignored/unimplemented */
				}
			} else {
				if ( df & IFF_DRV_RUNNING ) {
					mveth_stop(sc);
				}
			}
			sc->oif_flags = f;
		break;

#ifdef TODO_
/* what to do with lock ? */
  		case SIOCGIFMEDIA:
  		case SIOCSIFMEDIA:
			err = BSP_mve_media_ioctl(sc->pvt, cmd, &ifr->ifr_media);
		break;
#endif
 
		case SIOCADDMULTI:
		case SIOCDELMULTI:
			if ( if_getdrvflags( ifp ) & IFF_DRV_RUNNING ) {
				mveth_set_filters(ifp);
			}
		break;

 		case SIO_RTEMS_SHOW_STATS:
			BSP_mve_dump_stats(sc->mp, stdout);
		break;

		default:
			err = ether_ioctl(ifp, cmd, data);
		break;
	}

	mve_unlock( sc );

	return err;
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

	mtx_init( &sc->mtx, device_get_nameunit( sc->dev ), MTX_NETWORK_LOCK, MTX_DEF );
	callout_init_mtx( &sc->wdCallout, &sc->mtx, 0 );

	ifp->if_softc = sc;
	if_initname(ifp, device_get_name(dev), unit);
	ifp->if_init  = mve_init;
	ifp->if_ioctl = mve_ioctl;
	ifp->if_start = mve_start;
	if_setflags(ifp, (IFF_BROADCAST | IFF_MULTICAST | IFF_SIMPLEX) );
	sc->oif_flags = if_getflags(ifp);
	if_setsendqlen( ifp, tx_q_size );
	if_setsendqready( ifp );

	mp = BSP_mve_create(
	        unit,
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

	ether_ifattach( ifp, hwaddr );
}

static device_method_t mve_methods[] = {
	DEVMETHOD(device_probe,    mve_probe),
	DEVMETHOD_END
};

static driver_t mve_nexus_driver = {
	"mve",
	mve_methods,
	sizeof(struct mve_enet_softc )
};

static devclass_t mve_devclass;

DRIVER_MODULE(mve, nexus, mve_nexus_driver, mve_devclass, 0, 0);
#endif
