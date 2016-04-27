#include <machine/rtems-bsd-kernel-space.h>

#include <rtems/bsd/local/opt_dpaa.h>

/*
 * Copyright 2008-2015 Freescale Semiconductor Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Freescale Semiconductor nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 *
 * ALTERNATIVELY, this software may be distributed under the terms of the
 * GNU General Public License ("GPL") as published by the Free Software
 * Foundation, either version 2 of that License or (at your option) any
 * later version.
 *
 * THIS SOFTWARE IS PROVIDED BY Freescale Semiconductor ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Freescale Semiconductor BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include "crc_mac_addr_ext.h"

#include "fman_dtsec.h"
#include "fman.h"

#include <linux/slab.h>
#include <linux/bitrev.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/phy.h>

/* MII	Management Command Register */
#define MIIMCOM_READ_CYCLE		0x00000001

/* MII	Management Address Register */
#define MIIMADD_PHY_ADDR_SHIFT		8

/* MII Management Indicator Register */
#define MIIMIND_BUSY			0x00000001

/* PHY Control Register */
#define PHY_CR_PHY_RESET	0x8000
#define PHY_CR_SPEED0		0x2000
#define PHY_CR_ANE		0x1000
#define PHY_CR_RESET_AN		0x0200
#define PHY_CR_FULLDUPLEX	0x0100
#define PHY_CR_SPEED1		0x0040

#define PHY_TBICON_SRESET	0x8000
#define PHY_TBICON_CLK_SEL	0x0020
#define PHY_TBIANA_SGMII	0x4001
#define PHY_TBIANA_1000X	0x01a0

#define DTSEC_TO_MII_OFFSET	0x1000

/* Interrupt Mask Register (IMASK) */
#define DTSEC_IMASK_BREN	0x80000000
#define DTSEC_IMASK_RXCEN	0x40000000
#define DTSEC_IMASK_MSROEN	0x04000000
#define DTSEC_IMASK_GTSCEN	0x02000000
#define DTSEC_IMASK_BTEN	0x01000000
#define DTSEC_IMASK_TXCEN	0x00800000
#define DTSEC_IMASK_TXEEN	0x00400000
#define DTSEC_IMASK_LCEN	0x00040000
#define DTSEC_IMASK_CRLEN	0x00020000
#define DTSEC_IMASK_XFUNEN	0x00010000
#define DTSEC_IMASK_ABRTEN	0x00008000
#define DTSEC_IMASK_IFERREN	0x00004000
#define DTSEC_IMASK_MAGEN	0x00000800
#define DTSEC_IMASK_MMRDEN	0x00000400
#define DTSEC_IMASK_MMWREN	0x00000200
#define DTSEC_IMASK_GRSCEN	0x00000100
#define DTSEC_IMASK_TDPEEN	0x00000002
#define DTSEC_IMASK_RDPEEN	0x00000001

#define DTSEC_EVENTS_MASK		\
	 ((u32)(DTSEC_IMASK_BREN    |	\
		DTSEC_IMASK_RXCEN   |	\
		DTSEC_IMASK_BTEN    |	\
		DTSEC_IMASK_TXCEN   |	\
		DTSEC_IMASK_TXEEN   |	\
		DTSEC_IMASK_ABRTEN  |	\
		DTSEC_IMASK_LCEN    |	\
		DTSEC_IMASK_CRLEN   |	\
		DTSEC_IMASK_XFUNEN  |	\
		DTSEC_IMASK_IFERREN |	\
		DTSEC_IMASK_MAGEN   |	\
		DTSEC_IMASK_TDPEEN  |	\
		DTSEC_IMASK_RDPEEN))

/* dtsec timestamp event bits */
#define TMR_PEMASK_TSREEN	0x00010000
#define TMR_PEVENT_TSRE		0x00010000

/* Group address bit indication */
#define MAC_GROUP_ADDRESS	0x0000010000000000ULL

/* Defaults */
#define DEFAULT_HALFDUP_RETRANSMIT		0xf
#define DEFAULT_HALFDUP_COLL_WINDOW		0x37
#define DEFAULT_HALFDUP_ALT_BACKOFF_VAL	0x0A
#define DEFAULT_TX_PAUSE_TIME			0xf000
#define DEFAULT_TBIPA				5
#define DEFAULT_RX_PREPEND			0
#define DEFAULT_PREAMBLE_LEN			7
#define DEFAULT_TX_PAUSE_TIME_EXTD		0
#define DEFAULT_NON_BACK_TO_BACK_IPG1		0x40
#define DEFAULT_NON_BACK_TO_BACK_IPG2		0x60
#define DEFAULT_MIN_IFG_ENFORCEMENT		0x50
#define DEFAULT_BACK_TO_BACK_IPG		0x60
#define DEFAULT_MAXIMUM_FRAME			0x600
#define DEFAULT_TBI_PHY_ADDR			5

#define DTSEC_DEFAULT_EXCEPTIONS		 \
	((u32)((DTSEC_IMASK_BREN)		|\
			(DTSEC_IMASK_RXCEN)	|\
			(DTSEC_IMASK_BTEN)	|\
			(DTSEC_IMASK_TXCEN)	|\
			(DTSEC_IMASK_TXEEN)	|\
			(DTSEC_IMASK_ABRTEN)	|\
			(DTSEC_IMASK_LCEN)	|\
			(DTSEC_IMASK_CRLEN)	|\
			(DTSEC_IMASK_XFUNEN)	|\
			(DTSEC_IMASK_IFERREN)	|\
			(DTSEC_IMASK_MAGEN)	|\
			(DTSEC_IMASK_TDPEEN)	|\
			(DTSEC_IMASK_RDPEEN)))

/* register related defines (bits, field offsets..) */
#define DTSEC_ID2_INT_REDUCED_OFF	0x00010000

#define DTSEC_ECNTRL_GMIIM		0x00000040
#define DTSEC_ECNTRL_TBIM		0x00000020
#define DTSEC_ECNTRL_SGMIIM		0x00000002
#define DTSEC_ECNTRL_RPM		0x00000010
#define DTSEC_ECNTRL_R100M		0x00000008
#define DTSEC_ECNTRL_QSGMIIM		0x00000001

#define DTSEC_TCTRL_THDF		0x00000800
#define DTSEC_TCTRL_TTSE		0x00000040
#define DTSEC_TCTRL_GTS			0x00000020

#define RCTRL_PAL_MASK			0x001f0000
#define RCTRL_PAL_SHIFT			16
#define RCTRL_CFA			0x00008000
#define RCTRL_GHTX			0x00000400
#define RCTRL_RTSE			0x00000040
#define RCTRL_GRS			0x00000020
#define RCTRL_BC_REJ			0x00000010
#define RCTRL_MPROM			0x00000008
#define RCTRL_RSF			0x00000004
#define RCTRL_UPROM			0x00000001
#define RCTRL_PROM			(RCTRL_UPROM | RCTRL_MPROM)

#define MACCFG1_SOFT_RESET		0x80000000
#define MACCFG1_LOOPBACK		0x00000100
#define MACCFG1_RX_FLOW			0x00000020
#define MACCFG1_TX_FLOW			0x00000010
#define MACCFG1_TX_EN			0x00000001
#define MACCFG1_RX_EN			0x00000004

#define MACCFG2_NIBBLE_MODE		0x00000100
#define MACCFG2_BYTE_MODE		0x00000200
#define MACCFG2_PRE_AM_RX_EN		0x00000080
#define MACCFG2_PRE_AM_TX_EN		0x00000040
#define MACCFG2_LENGTH_CHECK		0x00000010
#define MACCFG2_PAD_CRC_EN		0x00000004
#define MACCFG2_CRC_EN			0x00000002
#define MACCFG2_FULL_DUPLEX		0x00000001
#define MACCFG2_PREAMBLE_LENGTH_MASK	0x0000f000
#define MACCFG2_PREAMBLE_LENGTH_SHIFT	12

#define IPGIFG_NON_BACK_TO_BACK_IPG_1_SHIFT	24
#define IPGIFG_NON_BACK_TO_BACK_IPG_2_SHIFT	16
#define IPGIFG_MIN_IFG_ENFORCEMENT_SHIFT	8

#define IPGIFG_NON_BACK_TO_BACK_IPG_1	0x7F000000
#define IPGIFG_NON_BACK_TO_BACK_IPG_2	0x007F0000
#define IPGIFG_MIN_IFG_ENFORCEMENT	0x0000FF00
#define IPGIFG_BACK_TO_BACK_IPG	0x0000007F

#define HAFDUP_ALT_BEB				0x00080000
#define HAFDUP_BP_NO_BACKOFF			0x00040000
#define HAFDUP_NO_BACKOFF			0x00020000
#define HAFDUP_EXCESS_DEFER			0x00010000
#define HAFDUP_COLLISION_WINDOW		0x000003ff
#define HAFDUP_ALTERNATE_BEB_TRUNCATION_MASK	0x00f00000
#define HAFDUP_ALTERNATE_BEB_TRUNCATION_SHIFT	20
#define HAFDUP_RETRANSMISSION_MAX_SHIFT	12
#define HAFDUP_RETRANSMISSION_MAX		0x0000f000

#define NUM_OF_HASH_REGS	8	/* Number of hash table registers */

#define PTV_PTE_MASK		0xffff0000
#define PTV_PT_MASK		0x0000ffff
#define PTV_PTE_SHIFT		16

#define MAX_PACKET_ALIGNMENT		31
#define MAX_INTER_PACKET_GAP		0x7f
#define MAX_INTER_PALTERNATE_BEB	0x0f
#define MAX_RETRANSMISSION		0x0f
#define MAX_COLLISION_WINDOW		0x03ff

/* Hash table size (32 bits*8 regs) */
#define DTSEC_HASH_TABLE_SIZE		256
/* Extended Hash table size (32 bits*16 regs) */
#define EXTENDED_HASH_TABLE_SIZE	512

/* maximum number of phys */
#define MAX_PHYS			32

/* MII Configuration Control Memory Map Registers */
struct dtsec_mii_regs {
	u32 reserved1[72];
	u32 miimcfg;	/* MII Mgmt:configuration */
	u32 miimcom;	/* MII Mgmt:command	  */
	u32 miimadd;	/* MII Mgmt:address	  */
	u32 miimcon;	/* MII Mgmt:control 3	  */
	u32 miimstat;	/* MII Mgmt:status	  */
	u32 miimind;	/* MII Mgmt:indicators	  */
};

/* dTSEC Memory Map registers */
struct dtsec_regs {
	/* dTSEC General Control and Status Registers */
	u32 tsec_id;		/* 0x000 ETSEC_ID register */
	u32 tsec_id2;		/* 0x004 ETSEC_ID2 register */
	u32 ievent;		/* 0x008 Interrupt event register */
	u32 imask;		/* 0x00C Interrupt mask register */
	u32 reserved0010[1];
	u32 ecntrl;		/* 0x014 E control register */
	u32 ptv;		/* 0x018 Pause time value register */
	u32 tbipa;		/* 0x01C TBI PHY address register */
	u32 tmr_ctrl;		/* 0x020 Time-stamp Control register */
	u32 tmr_pevent;		/* 0x024 Time-stamp event register */
	u32 tmr_pemask;		/* 0x028 Timer event mask register */
	u32 reserved002c[5];
	u32 tctrl;		/* 0x040 Transmit control register */
	u32 reserved0044[3];
	u32 rctrl;		/* 0x050 Receive control register */
	u32 reserved0054[11];
	u32 igaddr[8];		/* 0x080-0x09C Individual/group address */
	u32 gaddr[8];		/* 0x0A0-0x0BC Group address registers 0-7 */
	u32 reserved00c0[16];
	u32 maccfg1;		/* 0x100 MAC configuration #1 */
	u32 maccfg2;		/* 0x104 MAC configuration #2 */
	u32 ipgifg;		/* 0x108 IPG/IFG */
	u32 hafdup;		/* 0x10C Half-duplex */
	u32 maxfrm;		/* 0x110 Maximum frame */
	u32 reserved0114[10];
	u32 ifstat;		/* 0x13C Interface status */
	u32 macstnaddr1;	/* 0x140 Station Address,part 1 */
	u32 macstnaddr2;	/* 0x144 Station Address,part 2 */
	struct {
		u32 exact_match1;	/* octets 1-4 */
		u32 exact_match2;	/* octets 5-6 */
	} macaddr[15];		/* 0x148-0x1BC mac exact match addresses 1-15 */
	u32 reserved01c0[16];
	u32 tr64;	/* 0x200 Tx and Rx 64 byte frame counter */
	u32 tr127;	/* 0x204 Tx and Rx 65 to 127 byte frame counter */
	u32 tr255;	/* 0x208 Tx and Rx 128 to 255 byte frame counter */
	u32 tr511;	/* 0x20C Tx and Rx 256 to 511 byte frame counter */
	u32 tr1k;	/* 0x210 Tx and Rx 512 to 1023 byte frame counter */
	u32 trmax;	/* 0x214 Tx and Rx 1024 to 1518 byte frame counter */
	u32 trmgv;
	/* 0x218 Tx and Rx 1519 to 1522 byte good VLAN frame count */
	u32 rbyt;	/* 0x21C receive byte counter */
	u32 rpkt;	/* 0x220 receive packet counter */
	u32 rfcs;	/* 0x224 receive FCS error counter */
	u32 rmca;	/* 0x228 RMCA Rx multicast packet counter */
	u32 rbca;	/* 0x22C Rx broadcast packet counter */
	u32 rxcf;	/* 0x230 Rx control frame packet counter */
	u32 rxpf;	/* 0x234 Rx pause frame packet counter */
	u32 rxuo;	/* 0x238 Rx unknown OP code counter */
	u32 raln;	/* 0x23C Rx alignment error counter */
	u32 rflr;	/* 0x240 Rx frame length error counter */
	u32 rcde;	/* 0x244 Rx code error counter */
	u32 rcse;	/* 0x248 Rx carrier sense error counter */
	u32 rund;	/* 0x24C Rx undersize packet counter */
	u32 rovr;	/* 0x250 Rx oversize packet counter */
	u32 rfrg;	/* 0x254 Rx fragments counter */
	u32 rjbr;	/* 0x258 Rx jabber counter */
	u32 rdrp;	/* 0x25C Rx drop */
	u32 tbyt;	/* 0x260 Tx byte counter */
	u32 tpkt;	/* 0x264 Tx packet counter */
	u32 tmca;	/* 0x268 Tx multicast packet counter */
	u32 tbca;	/* 0x26C Tx broadcast packet counter */
	u32 txpf;	/* 0x270 Tx pause control frame counter */
	u32 tdfr;	/* 0x274 Tx deferral packet counter */
	u32 tedf;	/* 0x278 Tx excessive deferral packet counter */
	u32 tscl;	/* 0x27C Tx single collision packet counter */
	u32 tmcl;	/* 0x280 Tx multiple collision packet counter */
	u32 tlcl;	/* 0x284 Tx late collision packet counter */
	u32 txcl;	/* 0x288 Tx excessive collision packet counter */
	u32 tncl;	/* 0x28C Tx total collision counter */
	u32 reserved0290[1];
	u32 tdrp;	/* 0x294 Tx drop frame counter */
	u32 tjbr;	/* 0x298 Tx jabber frame counter */
	u32 tfcs;	/* 0x29C Tx FCS error counter */
	u32 txcf;	/* 0x2A0 Tx control frame counter */
	u32 tovr;	/* 0x2A4 Tx oversize frame counter */
	u32 tund;	/* 0x2A8 Tx undersize frame counter */
	u32 tfrg;	/* 0x2AC Tx fragments frame counter */
	u32 car1;	/* 0x2B0 carry register one register* */
	u32 car2;	/* 0x2B4 carry register two register* */
	u32 cam1;	/* 0x2B8 carry register one mask register */
	u32 cam2;	/* 0x2BC carry register two mask register */
	u32 reserved02c0[848];
};

/* struct dtsec_cfg - dTSEC configuration
 * Transmit half-duplex flow control, under software control for 10/100-Mbps
 * half-duplex media. If set, back pressure is applied to media by raising
 * carrier.
 * halfdup_retransmit:
 * Number of retransmission attempts following a collision.
 * If this is exceeded dTSEC aborts transmission due to excessive collisions.
 * The standard specifies the attempt limit to be 15.
 * halfdup_coll_window:
 * The number of bytes of the frame during which collisions may occur.
 * The default value of 55 corresponds to the frame byte at the end of the
 * standard 512-bit slot time window. If collisions are detected after this
 * byte, the late collision event is asserted and transmission of current
 * frame is aborted.
 * rx_drop_bcast:
 * Discard broadcast frames. If set, all broadcast frames will be discarded
 * by dTSEC.
 * rx_short_frm:
 * Accept short frames. If set, dTSEC will accept frames of length 14-63 bytes.
 * rx_len_check:
 * Length check for received frames. If set, the MAC checks the frame's length
 * field on receive to ensure it matches the actual data field length.
 * This only works for received frames with length field less than 1500.
 * No check is performed for larger frames.
 * tx_pad_crc:
 * Pad and append CRC. If set, the MAC pads all ransmitted short frames and
 * appends a CRC to every frame regardless of padding requirement.
 * tx_crc:
 * Transmission CRC enable. If set, the MAC appends a CRC to all frames.
 * If frames presented to the MAC have a valid length and contain a valid CRC,
 * tx_crc should be reset. This field is ignored if tx_pad_crc is set.
 * rx_ctrl_acc:
 * Control frame accept. If set, this overrides 802.3 standard control frame
 * behavior, and all Ethernet frames that have an ethertype of 0x8808 are
 * treated as normal Ethernet frames and passed up to the packet interface on
 * a DA match. Received pause control frames are passed to the packet
 * interface only if Rx flow control is also disabled.
 * See dtsec_accept_rx_pause_frames() function.
 * tx_pause_time:
 * Transmit pause time value. This pause value is used as part of the pause
 * frame to be sent when a transmit pause frame is initiated.
 * If set to 0 this disables transmission of pause frames.
 * rx_preamble:
 * Receive preamble enable. If set, the MAC recovers the received Ethernet
 * 7-byte preamble and passes it to the packet interface at the start of each
 * received frame.
 * This field should be reset for internal MAC loop-back mode.
 * tx_preamble:
 * User defined preamble enable for transmitted frames.
 * If set, a user-defined preamble must passed to the MAC and it is
 * transmitted instead of the standard preamble.
 * preamble_len:
 * Length, in bytes, of the preamble field preceding each Ethernet
 * start-of-frame delimiter byte. The default value of 0x7 should be used in
 * order to guarantee reliable operation with IEEE 802.3 compliant hardware.
 * rx_prepend:
 * Packet alignment padding length. The specified number of bytes (1-31)
 * of zero padding are inserted before the start of each received frame.
 * For Ethernet, where optional preamble extraction is enabled, the padding
 * appears before the preamble, otherwise the padding precedes the
 * layer 2 header.
 *
 * This structure contains basic dTSEC configuration and must be passed to
 * init() function. A default set of configuration values can be
 * obtained by calling set_dflts().
 */
struct dtsec_cfg {
	bool halfdup_on;
	bool halfdup_alt_backoff_en;
	bool halfdup_excess_defer;
	bool halfdup_no_backoff;
	bool halfdup_bp_no_backoff;
	u32 halfdup_alt_backoff_val;
	u16 halfdup_retransmit;
	u16 halfdup_coll_window;
	bool rx_drop_bcast;
	bool rx_short_frm;
	bool rx_len_check;
	bool tx_pad_crc;
	bool tx_crc;
	bool rx_ctrl_acc;
	u16 tx_pause_time;
	u16 tbipa;
	bool ptp_tsu_en;
	bool ptp_exception_en;
	bool rx_preamble;
	bool tx_preamble;
	u32 preamble_len;
	u32 rx_prepend;
	bool loopback;
	bool rx_time_stamp_en;
	bool tx_time_stamp_en;
	bool rx_flow;
	bool tx_flow;
	bool rx_group_hash_exd;
	bool rx_promisc;
	u8 tbi_phy_addr;
	u16 tx_pause_time_extd;
	u16 maximum_frame;
	u32 non_back_to_back_ipg1;
	u32 non_back_to_back_ipg2;
	u32 min_ifg_enforcement;
	u32 back_to_back_ipg;
};

struct fman_mac {
	/* pointer to dTSEC memory mapped registers */
	struct dtsec_regs __iomem *regs;
	/* pointer to dTSEC MII memory mapped registers */
	struct dtsec_mii_regs __iomem *mii_regs;
	/* MII management clock */
	u16 mii_mgmt_clk;
	/* MAC address of device */
	u64 addr;
	/* Ethernet physical interface */
	phy_interface_t phy_if;
	u16 max_speed;
	void *dev_id; /* device cookie used by the exception cbs */
	fman_mac_exception_cb *exception_cb;
	fman_mac_exception_cb *event_cb;
	/* Number of individual addresses in registers for this station */
	u8 num_of_ind_addr_in_regs;
	/* pointer to driver's global address hash table */
	struct eth_hash_t *multicast_addr_hash;
	/* pointer to driver's individual address hash table */
	struct eth_hash_t *unicast_addr_hash;
	u8 mac_id;
	u8 tbi_phy_addr;
	u32 exceptions;
	bool ptp_tsu_enabled;
	bool en_tsu_err_exeption;
	struct dtsec_cfg *dtsec_drv_param;
	void *fm;
	struct fman_rev_info fm_rev_info;
	bool basex_if;
};

static u32 calc_mii_mgmt_clk(struct fman_mac *dtsec)
{
	u16 fm_clk_freq, dtsec_freq;
	u32 mgmt_clk;

	fm_clk_freq = fman_get_clock_freq(dtsec->fm);
	if (fm_clk_freq  == 0) {
		pr_err("Can't get clock for MAC!\n");
		return 0;
	}

	dtsec_freq = (u16)(fm_clk_freq >> 1);

	if (dtsec_freq < 80)
		mgmt_clk = 1;
	else if (dtsec_freq < 120)
		mgmt_clk = 2;
	else if (dtsec_freq < 160)
		mgmt_clk = 3;
	else if (dtsec_freq < 200)
		mgmt_clk = 4;
	else if (dtsec_freq < 280)
		mgmt_clk = 5;
	else if (dtsec_freq < 400)
		mgmt_clk = 6;
	else
		mgmt_clk = 7;

	return mgmt_clk;
}

static int mii_write_reg(struct fman_mac *dtsec, u8 addr, u8 reg, u16 data)
{
	struct dtsec_mii_regs __iomem *regs = dtsec->mii_regs;
	u32 tmp;
	int count;

	/* Setup the MII Mgmt clock speed */
	iowrite32be(dtsec->mii_mgmt_clk, &regs->miimcfg);

	/* Stop the MII management read cycle */
	iowrite32be(0, &regs->miimcom);
	/* Dummy read to make sure MIIMCOM is written */
	tmp = ioread32be(&regs->miimcom);

	/* Setting up MII Management Address Register */
	tmp = (u32)((addr << MIIMADD_PHY_ADDR_SHIFT) | reg);
	iowrite32be(tmp, &regs->miimadd);

	/* Setting up MII Management Control Register with data */
	iowrite32be((u32)data, &regs->miimcon);
	/* Dummy read to make sure MIIMCON is written */
	tmp = ioread32be(&regs->miimcon);

	/* Wait until MII management write is complete */
	count = 100;
	do {
		udelay(1);
	} while (((ioread32be(&regs->miimind)) & MIIMIND_BUSY) && count--);

	if (count == 0)
		return -EBUSY;

	return 0;
}

static int mii_read_reg(struct fman_mac *dtsec, u8 addr, u8 reg, u16 *data)
{
	struct dtsec_mii_regs __iomem *regs = dtsec->mii_regs;
	u32 tmp;
	int count;

	/* Setup the MII Mgmt clock speed */
	iowrite32be(dtsec->mii_mgmt_clk, &regs->miimcfg);

	/* Setting up the MII Management Address Register */
	tmp = (u32)((addr << MIIMADD_PHY_ADDR_SHIFT) | reg);
	iowrite32be(tmp, &regs->miimadd);

	/* Perform an MII management read cycle */
	iowrite32be(MIIMCOM_READ_CYCLE, &regs->miimcom);
	/* Dummy read to make sure MIIMCOM is written */
	tmp = ioread32be(&regs->miimcom);

	/* Wait until MII management write is complete */
	count = 100;
	do {
		udelay(1);
	} while (((ioread32be(&regs->miimind)) & MIIMIND_BUSY) && count--);

	if (count == 0)
		return -EBUSY;

	/* Read MII management status  */
	*data = (u16)ioread32be(&regs->miimstat);

	iowrite32be(0, &regs->miimcom);
	/* Dummy read to make sure MIIMCOM is written */
	tmp = ioread32be(&regs->miimcom);

	if (*data == 0xffff) {
		pr_warn("Read wrong data(0xffff):phy_addr 0x%x,reg 0x%x",
			addr, reg);
		return -ENXIO;
	}

	return 0;
}

static void set_dflts(struct dtsec_cfg *cfg)
{
	cfg->halfdup_on = false;
	cfg->halfdup_retransmit = DEFAULT_HALFDUP_RETRANSMIT;
	cfg->halfdup_coll_window = DEFAULT_HALFDUP_COLL_WINDOW;
	cfg->halfdup_excess_defer = true;
	cfg->halfdup_no_backoff = false;
	cfg->halfdup_bp_no_backoff = false;
	cfg->halfdup_alt_backoff_val = DEFAULT_HALFDUP_ALT_BACKOFF_VAL;
	cfg->halfdup_alt_backoff_en = false;
	cfg->rx_drop_bcast = false;
	cfg->rx_short_frm = true;
	cfg->rx_len_check = false;
	cfg->tx_pad_crc = true;
	cfg->tx_crc = false;
	cfg->rx_ctrl_acc = false;
	cfg->tx_pause_time = DEFAULT_TX_PAUSE_TIME;
	/* PHY address 0 is reserved (DPAA RM) */
	cfg->tbipa = DEFAULT_TBIPA;
	cfg->rx_prepend = DEFAULT_RX_PREPEND;
	cfg->ptp_tsu_en = true;
	cfg->ptp_exception_en = true;
	cfg->preamble_len = DEFAULT_PREAMBLE_LEN;
	cfg->rx_preamble = false;
	cfg->tx_preamble = false;
	cfg->loopback = false;
	cfg->rx_time_stamp_en = false;
	cfg->tx_time_stamp_en = false;
	cfg->rx_flow = true;
	cfg->tx_flow = true;
	cfg->rx_group_hash_exd = false;
	cfg->tx_pause_time_extd = DEFAULT_TX_PAUSE_TIME_EXTD;
	cfg->rx_promisc = false;
	cfg->non_back_to_back_ipg1 = DEFAULT_NON_BACK_TO_BACK_IPG1;
	cfg->non_back_to_back_ipg2 = DEFAULT_NON_BACK_TO_BACK_IPG2;
	cfg->min_ifg_enforcement = DEFAULT_MIN_IFG_ENFORCEMENT;
	cfg->back_to_back_ipg = DEFAULT_BACK_TO_BACK_IPG;
	cfg->maximum_frame = DEFAULT_MAXIMUM_FRAME;
	cfg->tbi_phy_addr = DEFAULT_TBI_PHY_ADDR;
}

static int init(struct dtsec_regs __iomem *regs, struct dtsec_cfg *cfg,
		phy_interface_t iface, u16 iface_speed, u8 *macaddr,
		u32 exception_mask)
{
	bool is_rgmii, is_sgmii, is_qsgmii;
	int i;
	u32 tmp;

	/* Soft reset */
	iowrite32be(MACCFG1_SOFT_RESET, &regs->maccfg1);
	iowrite32be(0, &regs->maccfg1);

	/* dtsec_id2 */
	tmp = ioread32be(&regs->tsec_id2);

	/* check RGMII support */
	if (iface == PHY_INTERFACE_MODE_RGMII ||
	    iface == PHY_INTERFACE_MODE_RMII)
		if (tmp & DTSEC_ID2_INT_REDUCED_OFF)
			return -EINVAL;

	if (iface == PHY_INTERFACE_MODE_SGMII ||
	    iface == PHY_INTERFACE_MODE_MII)
		if (tmp & DTSEC_ID2_INT_REDUCED_OFF)
			return -EINVAL;

	is_rgmii = iface == PHY_INTERFACE_MODE_RGMII;
	is_sgmii = iface == PHY_INTERFACE_MODE_SGMII;
	is_qsgmii = iface == PHY_INTERFACE_MODE_QSGMII;

	tmp = 0;
	if (is_rgmii || iface == PHY_INTERFACE_MODE_GMII)
		tmp |= DTSEC_ECNTRL_GMIIM;
	if (is_sgmii)
		tmp |= (DTSEC_ECNTRL_SGMIIM | DTSEC_ECNTRL_TBIM);
	if (is_qsgmii)
		tmp |= (DTSEC_ECNTRL_SGMIIM | DTSEC_ECNTRL_TBIM |
			DTSEC_ECNTRL_QSGMIIM);
	if (is_rgmii)
		tmp |= DTSEC_ECNTRL_RPM;
	if (iface_speed == SPEED_100)
		tmp |= DTSEC_ECNTRL_R100M;

	iowrite32be(tmp, &regs->ecntrl);

	tmp = 0;
	if (cfg->halfdup_on)
		tmp |= DTSEC_TCTRL_THDF;
	if (cfg->tx_time_stamp_en)
		tmp |= DTSEC_TCTRL_TTSE;

	iowrite32be(tmp, &regs->tctrl);

	tmp = 0;

	if (cfg->tx_pause_time)
		tmp |= cfg->tx_pause_time;
	if (cfg->tx_pause_time_extd)
		tmp |= cfg->tx_pause_time_extd << PTV_PTE_SHIFT;
	iowrite32be(tmp, &regs->ptv);

	tmp = 0;
	tmp |= (cfg->rx_prepend << RCTRL_PAL_SHIFT) & RCTRL_PAL_MASK;
	if (cfg->rx_ctrl_acc)
		tmp |= RCTRL_CFA;
	if (cfg->rx_group_hash_exd)
		tmp |= RCTRL_GHTX;
	if (cfg->rx_time_stamp_en)
		tmp |= RCTRL_RTSE;
	if (cfg->rx_drop_bcast)
		tmp |= RCTRL_BC_REJ;
	if (cfg->rx_short_frm)
		tmp |= RCTRL_RSF;
	if (cfg->rx_promisc)
		tmp |= RCTRL_PROM;

	iowrite32be(tmp, &regs->rctrl);

	/* Assign a Phy Address to the TBI (TBIPA).
	 * Done also in cases where TBI is not selected to avoid conflict with
	 * the external PHY's Physical address
	 */
	iowrite32be(cfg->tbipa, &regs->tbipa);

	iowrite32be(0, &regs->tmr_ctrl);

	if (cfg->ptp_tsu_en) {
		tmp = 0;
		tmp |= TMR_PEVENT_TSRE;
		iowrite32be(tmp, &regs->tmr_pevent);

		if (cfg->ptp_exception_en) {
			tmp = 0;
			tmp |= TMR_PEMASK_TSREEN;
			iowrite32be(tmp, &regs->tmr_pemask);
		}
	}

	tmp = 0;
	if (cfg->loopback)
		tmp |= MACCFG1_LOOPBACK;
	if (cfg->rx_flow)
		tmp |= MACCFG1_RX_FLOW;
	if (cfg->tx_flow)
		tmp |= MACCFG1_TX_FLOW;
	iowrite32be(tmp, &regs->maccfg1);

	tmp = 0;

	if (iface_speed < SPEED_1000)
		tmp |= MACCFG2_NIBBLE_MODE;
	else if (iface_speed == SPEED_1000)
		tmp |= MACCFG2_BYTE_MODE;

	tmp |= (cfg->preamble_len << MACCFG2_PREAMBLE_LENGTH_SHIFT) &
		MACCFG2_PREAMBLE_LENGTH_MASK;
	if (cfg->rx_preamble)
		tmp |= MACCFG2_PRE_AM_RX_EN;
	if (cfg->tx_preamble)
		tmp |= MACCFG2_PRE_AM_TX_EN;
	if (cfg->rx_len_check)
		tmp |= MACCFG2_LENGTH_CHECK;
	if (cfg->tx_pad_crc)
		tmp |= MACCFG2_PAD_CRC_EN;
	if (cfg->tx_crc)
		tmp |= MACCFG2_CRC_EN;
	if (!cfg->halfdup_on)
		tmp |= MACCFG2_FULL_DUPLEX;
	iowrite32be(tmp, &regs->maccfg2);

	tmp = (((cfg->non_back_to_back_ipg1 <<
		 IPGIFG_NON_BACK_TO_BACK_IPG_1_SHIFT)
		& IPGIFG_NON_BACK_TO_BACK_IPG_1)
	       | ((cfg->non_back_to_back_ipg2 <<
		   IPGIFG_NON_BACK_TO_BACK_IPG_2_SHIFT)
		 & IPGIFG_NON_BACK_TO_BACK_IPG_2)
	       | ((cfg->min_ifg_enforcement << IPGIFG_MIN_IFG_ENFORCEMENT_SHIFT)
		 & IPGIFG_MIN_IFG_ENFORCEMENT)
	       | (cfg->back_to_back_ipg & IPGIFG_BACK_TO_BACK_IPG));
	iowrite32be(tmp, &regs->ipgifg);

	tmp = 0;

	if (cfg->halfdup_alt_backoff_en) {
		tmp = HAFDUP_ALT_BEB;
		tmp |= (cfg->halfdup_alt_backoff_val <<
			HAFDUP_ALTERNATE_BEB_TRUNCATION_SHIFT) &
			HAFDUP_ALTERNATE_BEB_TRUNCATION_MASK;
	}
	if (cfg->halfdup_bp_no_backoff)
		tmp |= HAFDUP_BP_NO_BACKOFF;
	if (cfg->halfdup_no_backoff)
		tmp |= HAFDUP_NO_BACKOFF;
	if (cfg->halfdup_excess_defer)
		tmp |= HAFDUP_EXCESS_DEFER;
	tmp |= ((cfg->halfdup_retransmit << HAFDUP_RETRANSMISSION_MAX_SHIFT)
		& HAFDUP_RETRANSMISSION_MAX);
	tmp |= (cfg->halfdup_coll_window & HAFDUP_COLLISION_WINDOW);

	iowrite32be(tmp, &regs->hafdup);

	/* Initialize Maximum frame length */
	iowrite32be(cfg->maximum_frame, &regs->maxfrm);

	iowrite32be(0xffffffff, &regs->cam1);
	iowrite32be(0xffffffff, &regs->cam2);

	iowrite32be(exception_mask, &regs->imask);

	iowrite32be(0xffffffff, &regs->ievent);

	tmp = (u32)((macaddr[5] << 24) |
		    (macaddr[4] << 16) | (macaddr[3] << 8) | macaddr[2]);
	iowrite32be(tmp, &regs->macstnaddr1);

	tmp = (u32)((macaddr[1] << 24) | (macaddr[0] << 16));
	iowrite32be(tmp, &regs->macstnaddr2);

	/* HASH */
	for (i = 0; i < NUM_OF_HASH_REGS; i++) {
		/* Initialize IADDRx */
		iowrite32be(0, &regs->igaddr[i]);
		/* Initialize GADDRx */
		iowrite32be(0, &regs->gaddr[i]);
	}

	return 0;
}

static void set_mac_address(struct dtsec_regs __iomem *regs, u8 *adr)
{
	u32 tmp;

	tmp = (u32)((adr[5] << 24) |
		    (adr[4] << 16) | (adr[3] << 8) | adr[2]);
	iowrite32be(tmp, &regs->macstnaddr1);

	tmp = (u32)((adr[1] << 24) | (adr[0] << 16));
	iowrite32be(tmp, &regs->macstnaddr2);
}

static void set_bucket(struct dtsec_regs __iomem *regs, int bucket,
		       bool enable)
{
	int reg_idx = (bucket >> 5) & 0xf;
	int bit_idx = bucket & 0x1f;
	u32 bit_mask = 0x80000000 >> bit_idx;
	u32 __iomem *reg;

	if (reg_idx > 7)
		reg = &regs->gaddr[reg_idx - 8];
	else
		reg = &regs->igaddr[reg_idx];

	if (enable)
		iowrite32be(ioread32be(reg) | bit_mask, reg);
	else
		iowrite32be(ioread32be(reg) & (~bit_mask), reg);
}

static int check_init_parameters(struct fman_mac *dtsec)
{
	if (dtsec->max_speed >= SPEED_10000) {
		pr_err("1G MAC driver supports 1G or lower speeds\n");
		return -EINVAL;
	}
	if (dtsec->addr == 0) {
		pr_err("Ethernet MAC Must have a valid MAC Address\n");
		return -EINVAL;
	}
	if (dtsec->max_speed >= SPEED_1000 &&
	    dtsec->dtsec_drv_param->halfdup_on) {
		pr_err("Ethernet MAC 1G can't work in half duplex\n");
		return -EINVAL;
	}

	/* FM_RX_PREAM_4_ERRATA_DTSEC_A001 Errata workaround */
	if (dtsec->dtsec_drv_param->rx_preamble) {
		pr_err("preamble_rx_en\n");
		return -EINVAL;
	}

	if (((dtsec->dtsec_drv_param)->tx_preamble ||
	     (dtsec->dtsec_drv_param)->rx_preamble) &&
	    ((dtsec->dtsec_drv_param)->preamble_len != 0x7)) {
		pr_err("Preamble length should be 0x7 bytes\n");
		return -EINVAL;
	}
	if ((dtsec->dtsec_drv_param)->halfdup_on &&
	    (dtsec->dtsec_drv_param->tx_time_stamp_en ||
	     dtsec->dtsec_drv_param->rx_time_stamp_en)) {
		pr_err("1588 timeStamp disabled in half duplex mode\n");
		return -EINVAL;
	}
	if ((dtsec->dtsec_drv_param)->rx_flow &&
	    (dtsec->dtsec_drv_param)->rx_ctrl_acc) {
		pr_err("Receive control frame can not be accepted\n");
		return -EINVAL;
	}
	if ((dtsec->dtsec_drv_param)->rx_prepend >
	    MAX_PACKET_ALIGNMENT) {
		pr_err("packetAlignmentPadding can't be > than %d\n",
		       MAX_PACKET_ALIGNMENT);
		return -EINVAL;
	}
	if (((dtsec->dtsec_drv_param)->non_back_to_back_ipg1 >
	     MAX_INTER_PACKET_GAP) ||
	    ((dtsec->dtsec_drv_param)->non_back_to_back_ipg2 >
	     MAX_INTER_PACKET_GAP) ||
	     ((dtsec->dtsec_drv_param)->back_to_back_ipg >
	      MAX_INTER_PACKET_GAP)) {
		pr_err("Inter packet gap can't be greater than %d\n",
		       MAX_INTER_PACKET_GAP);
		return -EINVAL;
	}
	if ((dtsec->dtsec_drv_param)->halfdup_alt_backoff_val >
	    MAX_INTER_PALTERNATE_BEB) {
		pr_err("alternateBackoffVal can't be greater than %d\n",
		       MAX_INTER_PALTERNATE_BEB);
		return -EINVAL;
	}
	if ((dtsec->dtsec_drv_param)->halfdup_retransmit >
	    MAX_RETRANSMISSION) {
		pr_err("maxRetransmission can't be greater than %d\n",
		       MAX_RETRANSMISSION);
		return -EINVAL;
	}
	if ((dtsec->dtsec_drv_param)->halfdup_coll_window >
	    MAX_COLLISION_WINDOW) {
		pr_err("collisionWindow can't be greater than %d\n",
		       MAX_COLLISION_WINDOW);
		return -EINVAL;
	/* If Auto negotiation process is disabled, need to set up the PHY
	 * using the MII Management Interface
	 */
	}
	if (dtsec->dtsec_drv_param->tbipa > MAX_PHYS) {
		pr_err("PHY address (should be 0-%d)\n", MAX_PHYS);
		return -ERANGE;
	}
	if (!dtsec->exception_cb) {
		pr_err("uninitialized exception_cb\n");
		return -EINVAL;
	}
	if (!dtsec->event_cb) {
		pr_err("uninitialized event_cb\n");
		return -EINVAL;
	}

	/* FM_LEN_CHECK_ERRATA_FMAN_SW002 Errata workaround */
	if (dtsec->dtsec_drv_param->rx_len_check) {
		pr_warn("Length Check!\n");
		return -EINVAL;
	}

	return 0;
}

static int get_exception_flag(enum fman_mac_exceptions exception)
{
	u32 bit_mask;

	switch (exception) {
	case FM_MAC_EX_1G_BAB_RX:
		bit_mask = DTSEC_IMASK_BREN;
		break;
	case FM_MAC_EX_1G_RX_CTL:
		bit_mask = DTSEC_IMASK_RXCEN;
		break;
	case FM_MAC_EX_1G_GRATEFUL_TX_STP_COMPLET:
		bit_mask = DTSEC_IMASK_GTSCEN;
		break;
	case FM_MAC_EX_1G_BAB_TX:
		bit_mask = DTSEC_IMASK_BTEN;
		break;
	case FM_MAC_EX_1G_TX_CTL:
		bit_mask = DTSEC_IMASK_TXCEN;
		break;
	case FM_MAC_EX_1G_TX_ERR:
		bit_mask = DTSEC_IMASK_TXEEN;
		break;
	case FM_MAC_EX_1G_LATE_COL:
		bit_mask = DTSEC_IMASK_LCEN;
		break;
	case FM_MAC_EX_1G_COL_RET_LMT:
		bit_mask = DTSEC_IMASK_CRLEN;
		break;
	case FM_MAC_EX_1G_TX_FIFO_UNDRN:
		bit_mask = DTSEC_IMASK_XFUNEN;
		break;
	case FM_MAC_EX_1G_MAG_PCKT:
		bit_mask = DTSEC_IMASK_MAGEN;
		break;
	case FM_MAC_EX_1G_MII_MNG_RD_COMPLET:
		bit_mask = DTSEC_IMASK_MMRDEN;
		break;
	case FM_MAC_EX_1G_MII_MNG_WR_COMPLET:
		bit_mask = DTSEC_IMASK_MMWREN;
		break;
	case FM_MAC_EX_1G_GRATEFUL_RX_STP_COMPLET:
		bit_mask = DTSEC_IMASK_GRSCEN;
		break;
	case FM_MAC_EX_1G_DATA_ERR:
		bit_mask = DTSEC_IMASK_TDPEEN;
		break;
	case FM_MAC_EX_1G_RX_MIB_CNT_OVFL:
		bit_mask = DTSEC_IMASK_MSROEN;
		break;
	default:
		bit_mask = 0;
		break;
	}

	return bit_mask;
}

static bool is_init_done(struct dtsec_cfg *dtsec_drv_params)
{
	/* Checks if dTSEC driver parameters were initialized */
	if (!dtsec_drv_params)
		return true;

	return false;
}

static u32 get_mac_addr_hash_code(u64 eth_addr)
{
	u32 crc;

	/* CRC calculation */
	GET_MAC_ADDR_CRC(eth_addr, crc);

	crc = bitrev32(crc);

	return crc;
}

static u16 dtsec_get_max_frame_length(struct fman_mac *dtsec)
{
	struct dtsec_regs __iomem *regs = dtsec->regs;

	if (is_init_done(dtsec->dtsec_drv_param))
		return 0;

	return (u16)ioread32be(&regs->maxfrm);
}

static void dtsec_isr(void *handle)
{
	struct fman_mac *dtsec = (struct fman_mac *)handle;
	struct dtsec_regs __iomem *regs = dtsec->regs;
	u32 event;

	/* do not handle MDIO events */
	event = ioread32be(&regs->ievent) &
		(u32)(~(DTSEC_IMASK_MMRDEN | DTSEC_IMASK_MMWREN));

	event &= ioread32be(&regs->imask);

	iowrite32be(event, &regs->ievent);

	if (event & DTSEC_IMASK_BREN)
		dtsec->exception_cb(dtsec->dev_id, FM_MAC_EX_1G_BAB_RX);
	if (event & DTSEC_IMASK_RXCEN)
		dtsec->exception_cb(dtsec->dev_id, FM_MAC_EX_1G_RX_CTL);
	if (event & DTSEC_IMASK_GTSCEN)
		dtsec->exception_cb(dtsec->dev_id,
				    FM_MAC_EX_1G_GRATEFUL_TX_STP_COMPLET);
	if (event & DTSEC_IMASK_BTEN)
		dtsec->exception_cb(dtsec->dev_id, FM_MAC_EX_1G_BAB_TX);
	if (event & DTSEC_IMASK_TXCEN)
		dtsec->exception_cb(dtsec->dev_id, FM_MAC_EX_1G_TX_CTL);
	if (event & DTSEC_IMASK_TXEEN)
		dtsec->exception_cb(dtsec->dev_id, FM_MAC_EX_1G_TX_ERR);
	if (event & DTSEC_IMASK_LCEN)
		dtsec->exception_cb(dtsec->dev_id, FM_MAC_EX_1G_LATE_COL);
	if (event & DTSEC_IMASK_CRLEN)
		dtsec->exception_cb(dtsec->dev_id, FM_MAC_EX_1G_COL_RET_LMT);
	if (event & DTSEC_IMASK_XFUNEN) {
		/* FM_TX_LOCKUP_ERRATA_DTSEC6 Errata workaround */
		if (dtsec->fm_rev_info.major == 2) {
			u32 tpkt1, tmp_reg1, tpkt2, tmp_reg2, i;
			/* a. Write 0x00E0_0C00 to DTSEC_ID
			 *	This is a read only register
			 * b. Read and save the value of TPKT
			 */
			tpkt1 = in_be32(&regs->tpkt);

			/* c. Read the register at dTSEC address offset 0x32C */
			tmp_reg1 = in_be32(&regs->reserved02c0[27]);

			/* d. Compare bits [9:15] to bits [25:31] of the
			 * register at address offset 0x32C.
			 */
			if ((tmp_reg1 & 0x007F0000) !=
				(tmp_reg1 & 0x0000007F)) {
				/* If they are not equal, save the value of
				 * this register and wait for at least
				 * MAXFRM*16 ns
				 */
				usleep_range((u32)(min
					(dtsec_get_max_frame_length(dtsec) *
					16 / 1000, 1)), (u32)
					(min(dtsec_get_max_frame_length
					(dtsec) * 16 / 1000, 1) + 1));
			}

			/* e. Read and save TPKT again and read the register
			 * at dTSEC address offset 0x32C again
			 */
			tpkt2 = in_be32(&regs->tpkt);
			tmp_reg2 = in_be32(&regs->reserved02c0[27]);

			/* f. Compare the value of TPKT saved in step b to
			 * value read in step e. Also compare bits [9:15] of
			 * the register at offset 0x32C saved in step d to the
			 * value of bits [9:15] saved in step e. If the two
			 * registers values are unchanged, then the transmit
			 * portion of the dTSEC controller is locked up and
			 * the user should proceed to the recover sequence.
			 */
			if ((tpkt1 == tpkt2) && ((tmp_reg1 & 0x007F0000) ==
				(tmp_reg2 & 0x007F0000))) {
				/* recover sequence */

				/* a.Write a 1 to RCTRL[GRS] */

				out_be32(&regs->rctrl,
					 in_be32(&regs->rctrl) | RCTRL_GRS);

				/* b.Wait until IEVENT[GRSC]=1, or at least
				 * 100 us has elapsed.
				 */
				for (i = 0; i < 100; i++) {
					if (in_be32(&regs->ievent) &
					    DTSEC_IMASK_GRSCEN)
						break;
					udelay(1);
				}
				if (in_be32(&regs->ievent) & DTSEC_IMASK_GRSCEN)
					out_be32(&regs->ievent,
						 DTSEC_IMASK_GRSCEN);
				else
					pr_debug("Rx lockup due to Tx lockup\n");

				/* c.Write a 1 to bit n of FM_RSTC
				 * (offset 0x0CC of FPM)
				 */
				fman_reset_mac(dtsec->fm, dtsec->mac_id);

				/* d.Wait 4 Tx clocks (32 ns) */
				udelay(1);

				/* e.Write a 0 to bit n of FM_RSTC. */
				/* cleared by FMAN
				 */
			}
		}

		dtsec->exception_cb(dtsec->dev_id, FM_MAC_EX_1G_TX_FIFO_UNDRN);
	}
	if (event & DTSEC_IMASK_MAGEN)
		dtsec->exception_cb(dtsec->dev_id, FM_MAC_EX_1G_MAG_PCKT);
	if (event & DTSEC_IMASK_GRSCEN)
		dtsec->exception_cb(dtsec->dev_id,
				    FM_MAC_EX_1G_GRATEFUL_RX_STP_COMPLET);
	if (event & DTSEC_IMASK_TDPEEN)
		dtsec->exception_cb(dtsec->dev_id, FM_MAC_EX_1G_DATA_ERR);
	if (event & DTSEC_IMASK_RDPEEN)
		dtsec->exception_cb(dtsec->dev_id, FM_MAC_1G_RX_DATA_ERR);

	/* masked interrupts */
	WARN_ON(event & DTSEC_IMASK_ABRTEN);
	WARN_ON(event & DTSEC_IMASK_IFERREN);
}

static void dtsec_1588_isr(void *handle)
{
	struct fman_mac *dtsec = (struct fman_mac *)handle;
	struct dtsec_regs __iomem *regs = dtsec->regs;
	u32 event;

	if (dtsec->ptp_tsu_enabled) {
		event = ioread32be(&regs->tmr_pevent);
		event &= ioread32be(&regs->tmr_pemask);

		if (event) {
			iowrite32be(event, &regs->tmr_pevent);
			WARN_ON(event & TMR_PEVENT_TSRE);
			dtsec->exception_cb(dtsec->dev_id,
					    FM_MAC_EX_1G_1588_TS_RX_ERR);
		}
	}
}

static void free_init_resources(struct fman_mac *dtsec)
{
	fman_unregister_intr(dtsec->fm, FMAN_MOD_MAC, dtsec->mac_id,
			     FMAN_INTR_TYPE_ERR);
	fman_unregister_intr(dtsec->fm, FMAN_MOD_MAC, dtsec->mac_id,
			     FMAN_INTR_TYPE_NORMAL);

	/* release the driver's group hash table */
	free_hash_table(dtsec->multicast_addr_hash);
	dtsec->multicast_addr_hash = NULL;

	/* release the driver's individual hash table */
	free_hash_table(dtsec->unicast_addr_hash);
	dtsec->unicast_addr_hash = NULL;
}

int dtsec_cfg_max_frame_len(struct fman_mac *dtsec, u16 new_val)
{
	if (is_init_done(dtsec->dtsec_drv_param))
		return -EINVAL;

	dtsec->dtsec_drv_param->maximum_frame = new_val;

	return 0;
}

int dtsec_cfg_pad_and_crc(struct fman_mac *dtsec, bool new_val)
{
	if (is_init_done(dtsec->dtsec_drv_param))
		return -EINVAL;

	dtsec->dtsec_drv_param->tx_pad_crc = new_val;

	return 0;
}

int dtsec_enable(struct fman_mac *dtsec, enum comm_mode mode)
{
	struct dtsec_regs __iomem *regs = dtsec->regs;
	u32 tmp;

	if (!is_init_done(dtsec->dtsec_drv_param))
		return -EINVAL;

	/* Enable */
	tmp = ioread32be(&regs->maccfg1);
	if (mode & COMM_MODE_RX)
		tmp |= MACCFG1_RX_EN;
	if (mode & COMM_MODE_TX)
		tmp |= MACCFG1_TX_EN;

	iowrite32be(tmp, &regs->maccfg1);

	/* Graceful start - clear the graceful receive stop bit */
	if (mode & COMM_MODE_TX)
		iowrite32be(ioread32be(&regs->tctrl) & ~DTSEC_TCTRL_GTS,
			    &regs->tctrl);
	if (mode & COMM_MODE_RX)
		iowrite32be(ioread32be(&regs->rctrl) & ~RCTRL_GRS,
			    &regs->rctrl);

	return 0;
}

int dtsec_disable(struct fman_mac *dtsec, enum comm_mode mode)
{
	struct dtsec_regs __iomem *regs = dtsec->regs;
	u32 tmp;

	if (!is_init_done(dtsec->dtsec_drv_param))
		return -EINVAL;

	/* Gracefull stop - Assert the graceful transmit stop bit */
	if (mode & COMM_MODE_RX) {
		tmp = ioread32be(&regs->rctrl) | RCTRL_GRS;
		iowrite32be(tmp, &regs->rctrl);

		if (dtsec->fm_rev_info.major == 2)
			usleep_range(100, 200);
		else
			udelay(10);
	}

	if (mode & COMM_MODE_TX) {
		if (dtsec->fm_rev_info.major == 2)
			pr_debug("GTS not supported due to DTSEC_A004 errata.\n");
		else
			pr_debug("GTS not supported due to DTSEC_A0014 errata.\n");
	}

	tmp = ioread32be(&regs->maccfg1);
	if (mode & COMM_MODE_RX)
		tmp &= ~MACCFG1_RX_EN;
	if (mode & COMM_MODE_TX)
		tmp &= ~MACCFG1_TX_EN;

	iowrite32be(tmp, &regs->maccfg1);

	return 0;
}

int dtsec_set_tx_pause_frames(struct fman_mac *dtsec,
			      u8 __maybe_unused priority,
			      u16 pause_time, u16 __maybe_unused thresh_time)
{
	struct dtsec_regs __iomem *regs = dtsec->regs;
	u32 ptv = 0;

	if (!is_init_done(dtsec->dtsec_drv_param))
		return -EINVAL;

	/* FM_BAD_TX_TS_IN_B_2_B_ERRATA_DTSEC_A003 Errata workaround */
	if (dtsec->fm_rev_info.major == 2)
		if (0 < pause_time && pause_time <= 320) {
			pr_warn("pause-time: %d illegal.Should be > 320\n",
				pause_time);
			return -EINVAL;
		}

	if (pause_time) {
		ptv = ioread32be(&regs->ptv);
		ptv &= PTV_PTE_MASK;
		ptv |= pause_time & PTV_PT_MASK;
		iowrite32be(ptv, &regs->ptv);

		/* trigger the transmission of a flow-control pause frame */
		iowrite32be(ioread32be(&regs->maccfg1) | MACCFG1_TX_FLOW,
			    &regs->maccfg1);
	} else
		iowrite32be(ioread32be(&regs->maccfg1) & ~MACCFG1_TX_FLOW,
			    &regs->maccfg1);

	return 0;
}

int dtsec_accept_rx_pause_frames(struct fman_mac *dtsec, bool en)
{
	struct dtsec_regs __iomem *regs = dtsec->regs;
	u32 tmp;

	if (!is_init_done(dtsec->dtsec_drv_param))
		return -EINVAL;

	tmp = ioread32be(&regs->maccfg1);
	if (en)
		tmp |= MACCFG1_RX_FLOW;
	else
		tmp &= ~MACCFG1_RX_FLOW;
	iowrite32be(tmp, &regs->maccfg1);

	return 0;
}

int dtsec_modify_mac_address(struct fman_mac *dtsec, enet_addr_t *enet_addr)
{
	if (!is_init_done(dtsec->dtsec_drv_param))
		return -EINVAL;

	/* Initialize MAC Station Address registers (1 & 2)
	 * Station address have to be swapped (big endian to little endian
	 */
	dtsec->addr = ENET_ADDR_TO_UINT64(*enet_addr);
	set_mac_address(dtsec->regs, (u8 *)(*enet_addr));

	return 0;
}

int dtsec_add_hash_mac_address(struct fman_mac *dtsec, enet_addr_t *eth_addr)
{
	struct dtsec_regs __iomem *regs = dtsec->regs;
	struct eth_hash_entry *hash_entry;
	u64 addr;
	s32 bucket;
	u32 crc;
	bool mcast, ghtx;

	if (!is_init_done(dtsec->dtsec_drv_param))
		return -EINVAL;

	addr = ENET_ADDR_TO_UINT64(*eth_addr);

	ghtx = (bool)((ioread32be(&regs->rctrl) & RCTRL_GHTX) ? true : false);
	mcast = (bool)((addr & MAC_GROUP_ADDRESS) ? true : false);

	/* Cannot handle unicast mac addr when GHTX is on */
	if (ghtx && !mcast) {
		pr_err("Could not compute hash bucket\n");
		return -EINVAL;
	}
	crc = get_mac_addr_hash_code(addr);

	/* considering the 9 highest order bits in crc H[8:0]:
	 *if ghtx = 0 H[8:6] (highest order 3 bits) identify the hash register
	 *and H[5:1] (next 5 bits) identify the hash bit
	 *if ghts = 1 H[8:5] (highest order 4 bits) identify the hash register
	 *and H[4:0] (next 5 bits) identify the hash bit.
	 *
	 *In bucket index output the low 5 bits identify the hash register
	 *bit, while the higher 4 bits identify the hash register
	 */

	if (ghtx) {
		bucket = (s32)((crc >> 23) & 0x1ff);
	} else {
		bucket = (s32)((crc >> 24) & 0xff);
		/* if !ghtx and mcast the bit must be set in gaddr instead of
		 *igaddr.
		 */
		if (mcast)
			bucket += 0x100;
	}

	set_bucket(dtsec->regs, bucket, true);

	/* Create element to be added to the driver hash table */
	hash_entry = kmalloc(sizeof(*hash_entry), GFP_KERNEL);
	if (!hash_entry)
		return -ENOMEM;
	hash_entry->addr = addr;
	INIT_LIST_HEAD(&hash_entry->node);

	if (addr & MAC_GROUP_ADDRESS)
		/* Group Address */
		list_add_tail(&hash_entry->node,
			      &dtsec->multicast_addr_hash->lsts[bucket]);
	else
		list_add_tail(&hash_entry->node,
			      &dtsec->unicast_addr_hash->lsts[bucket]);

	return 0;
}

int dtsec_del_hash_mac_address(struct fman_mac *dtsec, enet_addr_t *eth_addr)
{
	struct dtsec_regs __iomem *regs = dtsec->regs;
	struct list_head *pos;
	struct eth_hash_entry *hash_entry = NULL;
	u64 addr;
	s32 bucket;
	u32 crc;
	bool mcast, ghtx;

	if (!is_init_done(dtsec->dtsec_drv_param))
		return -EINVAL;

	addr = ENET_ADDR_TO_UINT64(*eth_addr);

	ghtx = (bool)((ioread32be(&regs->rctrl) & RCTRL_GHTX) ? true : false);
	mcast = (bool)((addr & MAC_GROUP_ADDRESS) ? true : false);

	/* Cannot handle unicast mac addr when GHTX is on */
	if (ghtx && !mcast) {
		pr_err("Could not compute hash bucket\n");
		return -EINVAL;
	}
	crc = get_mac_addr_hash_code(addr);

	if (ghtx) {
		bucket = (s32)((crc >> 23) & 0x1ff);
	} else {
		bucket = (s32)((crc >> 24) & 0xff);
		/* if !ghtx and mcast the bit must be set
		 * in gaddr instead of igaddr.
		 */
		if (mcast)
			bucket += 0x100;
	}

	if (addr & MAC_GROUP_ADDRESS) {
		/* Group Address */
		list_for_each(pos,
			      &dtsec->multicast_addr_hash->lsts[bucket]) {
			hash_entry = ETH_HASH_ENTRY_OBJ(pos);
			if (hash_entry->addr == addr) {
				list_del_init(&hash_entry->node);
				kfree(hash_entry);
				break;
			}
		}
		if (list_empty(&dtsec->multicast_addr_hash->lsts[bucket]))
			set_bucket(dtsec->regs, bucket, false);
	} else {
		/* Individual Address */
		list_for_each(pos,
			      &dtsec->unicast_addr_hash->lsts[bucket]) {
			hash_entry = ETH_HASH_ENTRY_OBJ(pos);
			if (hash_entry->addr == addr) {
				list_del_init(&hash_entry->node);
				kfree(hash_entry);
				break;
			}
		}
		if (list_empty(&dtsec->unicast_addr_hash->lsts[bucket]))
			set_bucket(dtsec->regs, bucket, false);
	}

	/* address does not exist */
	WARN_ON(!hash_entry);

	return 0;
}

int dtsec_set_promiscuous(struct fman_mac *dtsec, bool new_val)
{
	struct dtsec_regs __iomem *regs = dtsec->regs;
	u32 tmp;

	if (!is_init_done(dtsec->dtsec_drv_param))
		return -EINVAL;

	/* Set unicast promiscuous */
	tmp = ioread32be(&regs->rctrl);
	if (new_val)
		tmp |= RCTRL_UPROM;
	else
		tmp &= ~RCTRL_UPROM;

	iowrite32be(tmp, &regs->rctrl);

	/* Set multicast promiscuous */
	tmp = ioread32be(&regs->rctrl);
	if (new_val)
		tmp |= RCTRL_MPROM;
	else
		tmp &= ~RCTRL_MPROM;

	iowrite32be(tmp, &regs->rctrl);

	return 0;
}

int dtsec_adjust_link(struct fman_mac *dtsec, u16 speed)
{
	struct dtsec_regs __iomem *regs = dtsec->regs;
	u32 tmp;

	if (!is_init_done(dtsec->dtsec_drv_param))
		return -EINVAL;

	tmp = ioread32be(&regs->maccfg2);

	/* Full Duplex */
	tmp |= MACCFG2_FULL_DUPLEX;

	tmp &= ~(MACCFG2_NIBBLE_MODE | MACCFG2_BYTE_MODE);
	if (speed < SPEED_1000)
		tmp |= MACCFG2_NIBBLE_MODE;
	else if (speed == SPEED_1000)
		tmp |= MACCFG2_BYTE_MODE;
	iowrite32be(tmp, &regs->maccfg2);

	tmp = ioread32be(&regs->ecntrl);
	if (speed == SPEED_100)
		tmp |= DTSEC_ECNTRL_R100M;
	else
		tmp &= ~DTSEC_ECNTRL_R100M;
	iowrite32be(tmp, &regs->ecntrl);

	return 0;
}

int dtsec_restart_autoneg(struct fman_mac *dtsec)
{
	u16 tmp_reg16;
	int err;

	if (!is_init_done(dtsec->dtsec_drv_param))
		return -EINVAL;

	err = mii_read_reg(dtsec, dtsec->tbi_phy_addr, 0, &tmp_reg16);
	if (err) {
		pr_err("Autonegotiation restart failed\n");
		return err;
	}

	tmp_reg16 &= ~(PHY_CR_SPEED0 | PHY_CR_SPEED1);
	tmp_reg16 |=
	    (PHY_CR_ANE | PHY_CR_RESET_AN | PHY_CR_FULLDUPLEX | PHY_CR_SPEED1);

	mii_write_reg(dtsec, dtsec->tbi_phy_addr, 0, tmp_reg16);

	return 0;
}

int dtsec_get_version(struct fman_mac *dtsec, u32 *mac_version)
{
	struct dtsec_regs __iomem *regs = dtsec->regs;

	if (!is_init_done(dtsec->dtsec_drv_param))
		return -EINVAL;

	*mac_version = ioread32be(&regs->tsec_id);

	return 0;
}

int dtsec_set_exception(struct fman_mac *dtsec,
			enum fman_mac_exceptions exception, bool enable)
{
	struct dtsec_regs __iomem *regs = dtsec->regs;
	u32 bit_mask = 0;

	if (!is_init_done(dtsec->dtsec_drv_param))
		return -EINVAL;

	if (exception != FM_MAC_EX_1G_1588_TS_RX_ERR) {
		bit_mask = get_exception_flag(exception);
		if (bit_mask) {
			if (enable)
				dtsec->exceptions |= bit_mask;
			else
				dtsec->exceptions &= ~bit_mask;
		} else {
			pr_err("Undefined exception\n");
			return -EINVAL;
		}
		if (enable)
			iowrite32be(ioread32be(&regs->imask) | bit_mask,
				    &regs->imask);
		else
			iowrite32be(ioread32be(&regs->imask) & ~bit_mask,
				    &regs->imask);
	} else {
		if (!dtsec->ptp_tsu_enabled) {
			pr_err("Exception valid for 1588 only\n");
			return -EINVAL;
		}
		switch (exception) {
		case FM_MAC_EX_1G_1588_TS_RX_ERR:
			if (enable) {
				dtsec->en_tsu_err_exeption = true;
				iowrite32be(ioread32be(&regs->tmr_pemask) |
					    TMR_PEMASK_TSREEN,
					    &regs->tmr_pemask);
			} else {
				dtsec->en_tsu_err_exeption = false;
				iowrite32be(ioread32be(&regs->tmr_pemask) &
					    ~TMR_PEMASK_TSREEN,
					    &regs->tmr_pemask);
			}
			break;
		default:
			pr_err("Undefined exception\n");
			return -EINVAL;
		}
	}

	return 0;
}

int dtsec_init(struct fman_mac *dtsec)
{
	struct dtsec_regs __iomem *regs = dtsec->regs;
	struct dtsec_cfg *dtsec_drv_param;
	int err;
	u16 max_frm_ln;
	enet_addr_t eth_addr;

	if (is_init_done(dtsec->dtsec_drv_param))
		return -EINVAL;

	if (DEFAULT_RESET_ON_INIT &&
	    (fman_reset_mac(dtsec->fm, dtsec->mac_id) != 0)) {
		pr_err("Can't reset MAC!\n");
		return -EINVAL;
	}

	err = check_init_parameters(dtsec);
	if (err)
		return err;

	dtsec_drv_param = dtsec->dtsec_drv_param;

	MAKE_ENET_ADDR_FROM_UINT64(dtsec->addr, eth_addr);

	err = init(dtsec->regs, dtsec_drv_param, dtsec->phy_if,
		   dtsec->max_speed, (u8 *)eth_addr, dtsec->exceptions);
	if (err) {
		free_init_resources(dtsec);
		pr_err("DTSEC version doesn't support this i/f mode\n");
		return err;
	}

	if (dtsec->phy_if == PHY_INTERFACE_MODE_SGMII) {
		u16 tmp_reg16;

		/* Configure the TBI PHY Control Register */
		tmp_reg16 = PHY_TBICON_CLK_SEL | PHY_TBICON_SRESET;
		mii_write_reg(dtsec, (u8)dtsec_drv_param->tbipa, 17,
			      tmp_reg16);

		tmp_reg16 = PHY_TBICON_CLK_SEL;
		mii_write_reg(dtsec, (u8)dtsec_drv_param->tbipa, 17,
			      tmp_reg16);

		tmp_reg16 =
		    (PHY_CR_PHY_RESET | PHY_CR_ANE | PHY_CR_FULLDUPLEX |
		     PHY_CR_SPEED1);
		mii_write_reg(dtsec, (u8)dtsec_drv_param->tbipa, 0, tmp_reg16);

		if (dtsec->basex_if)
			tmp_reg16 = PHY_TBIANA_1000X;
		else
			tmp_reg16 = PHY_TBIANA_SGMII;
		mii_write_reg(dtsec, (u8)dtsec_drv_param->tbipa, 4, tmp_reg16);

		tmp_reg16 =
		    (PHY_CR_ANE | PHY_CR_RESET_AN | PHY_CR_FULLDUPLEX |
		     PHY_CR_SPEED1);

		mii_write_reg(dtsec, (u8)dtsec_drv_param->tbipa, 0, tmp_reg16);
	}

	/* Max Frame Length */
	max_frm_ln = (u16)ioread32be(&regs->maxfrm);
	err = fman_set_mac_max_frame(dtsec->fm, dtsec->mac_id, max_frm_ln);
	if (err) {
		pr_err("Setting max frame length failed\n");
		free_init_resources(dtsec);
		return -EINVAL;
	}

	dtsec->multicast_addr_hash =
	alloc_hash_table(EXTENDED_HASH_TABLE_SIZE);
	if (!dtsec->multicast_addr_hash) {
		free_init_resources(dtsec);
		pr_err("MC hash table is failed\n");
		return -ENOMEM;
	}

	dtsec->unicast_addr_hash = alloc_hash_table(DTSEC_HASH_TABLE_SIZE);
	if (!dtsec->unicast_addr_hash) {
		free_init_resources(dtsec);
		pr_err("UC hash table is failed\n");
		return -ENOMEM;
	}

	/* register err intr handler for dtsec to FPM (err) */
	fman_register_intr(dtsec->fm, FMAN_MOD_MAC, dtsec->mac_id,
			   FMAN_INTR_TYPE_ERR, dtsec_isr, dtsec);
	/* register 1588 intr handler for TMR to FPM (normal) */
	fman_register_intr(dtsec->fm, FMAN_MOD_MAC, dtsec->mac_id,
			   FMAN_INTR_TYPE_NORMAL, dtsec_1588_isr, dtsec);

	kfree(dtsec_drv_param);
	dtsec->dtsec_drv_param = NULL;

	return 0;
}

int dtsec_free(struct fman_mac *dtsec)
{
	free_init_resources(dtsec);

	kfree(dtsec->dtsec_drv_param);
	dtsec->dtsec_drv_param = NULL;
	kfree(dtsec);

	return 0;
}

struct fman_mac *dtsec_config(struct fman_mac_params *params)
{
	struct fman_mac *dtsec;
	struct dtsec_cfg *dtsec_drv_param;
	void __iomem *base_addr;

	base_addr = params->base_addr;

	/* allocate memory for the UCC GETH data structure. */
	dtsec = kzalloc(sizeof(*dtsec), GFP_KERNEL);
	if (!dtsec)
		return NULL;

	/* allocate memory for the d_tsec driver parameters data structure. */
	dtsec_drv_param = kzalloc(sizeof(*dtsec_drv_param), GFP_KERNEL);
	if (!dtsec_drv_param)
		goto err_dtsec;

	/* Plant parameter structure pointer */
	dtsec->dtsec_drv_param = dtsec_drv_param;

	set_dflts(dtsec_drv_param);

	dtsec->regs = (struct dtsec_regs __iomem *)(base_addr);
	dtsec->mii_regs = (struct dtsec_mii_regs __iomem *)
		(base_addr + DTSEC_TO_MII_OFFSET);
	dtsec->addr = ENET_ADDR_TO_UINT64(params->addr);
	dtsec->max_speed = params->max_speed;
	dtsec->phy_if = params->phy_if;
	dtsec->mac_id = params->mac_id;
	dtsec->exceptions = DTSEC_DEFAULT_EXCEPTIONS;
	dtsec->exception_cb = params->exception_cb;
	dtsec->event_cb = params->event_cb;
	dtsec->dev_id = params->dev_id;
	dtsec->ptp_tsu_enabled = dtsec->dtsec_drv_param->ptp_tsu_en;
	dtsec->en_tsu_err_exeption = dtsec->dtsec_drv_param->ptp_exception_en;
	dtsec->tbi_phy_addr = dtsec->dtsec_drv_param->tbi_phy_addr;

	dtsec->fm = params->fm;
	dtsec->basex_if = params->basex_if;
	dtsec->mii_mgmt_clk = calc_mii_mgmt_clk(dtsec);
	if (dtsec->mii_mgmt_clk == 0) {
		pr_err("Can't calculate MII management clock\n");
		goto err_dtsec;
	}

	/* Save FMan revision */
	fman_get_revision(dtsec->fm, &dtsec->fm_rev_info);

	return dtsec;

err_dtsec:
	kfree(dtsec);
	return NULL;
}
