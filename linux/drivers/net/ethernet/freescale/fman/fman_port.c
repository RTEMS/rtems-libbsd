#include <machine/rtems-bsd-kernel-space.h>

#include <rtems/bsd/local/opt_dpaa.h>

/*
 * Copyright 2008 - 2015 Freescale Semiconductor Inc.
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

#include "fman_port.h"
#include "fman.h"
#include "fman_sp.h"

#include <asm/mpc85xx.h>
#include <linux/io.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/of_platform.h>
#include <linux/of_address.h>

/* Queue ID */
#define DFLT_FQ_ID		0x00FFFFFF

/* General defines */
#define PORT_BMI_FIFO_UNITS		0x100

#define MAX_PORT_FIFO_SIZE(bmi_max_fifo_size)	\
	min((u32)bmi_max_fifo_size, (u32)1024 * FMAN_BMI_FIFO_UNITS)

#define PORT_CG_MAP_NUM			8
#define PORT_PRS_RESULT_WORDS_NUM	8
#define PORT_IC_OFFSET_UNITS		0x10

#define MIN_EXT_BUF_SIZE		64

#define BMI_PORT_REGS_OFFSET				0
#define QMI_PORT_REGS_OFFSET				0x400

/* Default values */
#define DFLT_PORT_BUFFER_PREFIX_CONTEXT_DATA_ALIGN		\
	DFLT_FM_SP_BUFFER_PREFIX_CONTEXT_DATA_ALIGN

#define DFLT_PORT_CUT_BYTES_FROM_END		4

#define DFLT_PORT_ERRORS_TO_DISCARD		FM_PORT_FRM_ERR_CLS_DISCARD
#define DFLT_PORT_MAX_FRAME_LENGTH		9600

#define DFLT_PORT_RX_FIFO_PRI_ELEVATION_LEV(bmi_max_fifo_size)	\
	MAX_PORT_FIFO_SIZE(bmi_max_fifo_size)

#define DFLT_PORT_RX_FIFO_THRESHOLD(major, bmi_max_fifo_size)	\
	(major == 6 ?						\
	MAX_PORT_FIFO_SIZE(bmi_max_fifo_size) :		\
	(MAX_PORT_FIFO_SIZE(bmi_max_fifo_size) * 3 / 4))	\

#define DFLT_PORT_EXTRA_NUM_OF_FIFO_BUFS		0

/* QMI defines */
#define QMI_DEQ_CFG_SUBPORTAL_MASK		0x1f

#define QMI_PORT_CFG_EN				0x80000000
#define QMI_PORT_STATUS_DEQ_FD_BSY		0x20000000

#define QMI_DEQ_CFG_PRI				0x80000000
#define QMI_DEQ_CFG_TYPE1			0x10000000
#define QMI_DEQ_CFG_TYPE2			0x20000000
#define QMI_DEQ_CFG_TYPE3			0x30000000
#define QMI_DEQ_CFG_PREFETCH_PARTIAL		0x01000000
#define QMI_DEQ_CFG_PREFETCH_FULL		0x03000000
#define QMI_DEQ_CFG_SP_MASK			0xf
#define QMI_DEQ_CFG_SP_SHIFT			20

#define QMI_BYTE_COUNT_LEVEL_CONTROL(_type)	\
	(_type == FMAN_PORT_TYPE_TX ? 0x1400 : 0x400)

/* BMI defins */
#define BMI_EBD_EN				0x80000000

#define BMI_PORT_CFG_EN				0x80000000
#define BMI_PORT_CFG_FDOVR			0x02000000

#define BMI_PORT_STATUS_BSY			0x80000000

#define BMI_DMA_ATTR_SWP_SHIFT			FMAN_SP_DMA_ATTR_SWP_SHIFT
#define BMI_DMA_ATTR_IC_STASH_ON		0x10000000
#define BMI_DMA_ATTR_HDR_STASH_ON		0x04000000
#define BMI_DMA_ATTR_SG_STASH_ON		0x01000000
#define BMI_DMA_ATTR_WRITE_OPTIMIZE		FMAN_SP_DMA_ATTR_WRITE_OPTIMIZE

#define BMI_RX_FIFO_PRI_ELEVATION_SHIFT	16
#define BMI_RX_FIFO_THRESHOLD_ETHE		0x80000000

#define BMI_FRAME_END_CS_IGNORE_SHIFT		24
#define BMI_FRAME_END_CS_IGNORE_MASK		0x0000001f

#define BMI_RX_FRAME_END_CUT_SHIFT		16
#define BMI_RX_FRAME_END_CUT_MASK		0x0000001f

#define BMI_IC_TO_EXT_SHIFT			FMAN_SP_IC_TO_EXT_SHIFT
#define BMI_IC_TO_EXT_MASK			0x0000001f
#define BMI_IC_FROM_INT_SHIFT			FMAN_SP_IC_FROM_INT_SHIFT
#define BMI_IC_FROM_INT_MASK			0x0000000f
#define BMI_IC_SIZE_MASK			0x0000001f

#define BMI_INT_BUF_MARG_SHIFT			28
#define BMI_INT_BUF_MARG_MASK			0x0000000f
#define BMI_EXT_BUF_MARG_START_SHIFT		FMAN_SP_EXT_BUF_MARG_START_SHIFT
#define BMI_EXT_BUF_MARG_START_MASK		0x000001ff
#define BMI_EXT_BUF_MARG_END_MASK		0x000001ff

#define BMI_CMD_MR_LEAC				0x00200000
#define BMI_CMD_MR_SLEAC			0x00100000
#define BMI_CMD_MR_MA				0x00080000
#define BMI_CMD_MR_DEAS				0x00040000
#define BMI_CMD_RX_MR_DEF			(BMI_CMD_MR_LEAC | \
						BMI_CMD_MR_SLEAC | \
						BMI_CMD_MR_MA | \
						BMI_CMD_MR_DEAS)
#define BMI_CMD_TX_MR_DEF			0

#define BMI_CMD_ATTR_ORDER			0x80000000
#define BMI_CMD_ATTR_SYNC			0x02000000
#define BMI_CMD_ATTR_COLOR_SHIFT		26

#define BMI_FIFO_PIPELINE_DEPTH_SHIFT		12
#define BMI_FIFO_PIPELINE_DEPTH_MASK		0x0000000f
#define BMI_NEXT_ENG_FD_BITS_SHIFT		24

#define BMI_EXT_BUF_POOL_VALID			FMAN_SP_EXT_BUF_POOL_VALID
#define BMI_EXT_BUF_POOL_EN_COUNTER		FMAN_SP_EXT_BUF_POOL_EN_COUNTER
#define BMI_EXT_BUF_POOL_BACKUP		FMAN_SP_EXT_BUF_POOL_BACKUP
#define BMI_EXT_BUF_POOL_ID_SHIFT		16
#define BMI_EXT_BUF_POOL_ID_MASK		0x003F0000
#define BMI_POOL_DEP_NUM_OF_POOLS_SHIFT	16

#define BMI_TX_FIFO_MIN_FILL_SHIFT		16

#define BMI_SG_DISABLE				FMAN_SP_SG_DISABLE

#define BMI_PRIORITY_ELEVATION_LEVEL ((0x3FF + 1) * PORT_BMI_FIFO_UNITS)
#define BMI_FIFO_THRESHOLD	      ((0x3FF + 1) * PORT_BMI_FIFO_UNITS)

#define BMI_DEQUEUE_PIPELINE_DEPTH(_type, _speed)		\
	((_type == FMAN_PORT_TYPE_TX && _speed == 10000) ? 4 : 1)

#define BMI_PORT_RFNE_FRWD_RPD                  0x40000000

#define RX_ERRS_TO_ENQ				  \
	(FM_PORT_FRM_ERR_DMA			| \
	FM_PORT_FRM_ERR_PHYSICAL		| \
	FM_PORT_FRM_ERR_SIZE			| \
	FM_PORT_FRM_ERR_EXTRACTION		| \
	FM_PORT_FRM_ERR_NO_SCHEME		| \
	FM_PORT_FRM_ERR_PRS_TIMEOUT		| \
	FM_PORT_FRM_ERR_PRS_ILL_INSTRUCT	| \
	FM_PORT_FRM_ERR_BLOCK_LIMIT_EXCEEDED	| \
	FM_PORT_FRM_ERR_PRS_HDR_ERR		| \
	FM_PORT_FRM_ERR_KEYSIZE_OVERFLOW	| \
	FM_PORT_FRM_ERR_IPRE)

/* NIA defines */
#define NIA_ORDER_RESTOR				0x00800000
#define NIA_ENG_FM_CTL					0x00000000
#define NIA_ENG_BMI					0x00500000
#define NIA_ENG_QMI_ENQ					0x00540000
#define NIA_ENG_QMI_DEQ					0x00580000

#define NIA_FM_CTL_AC_NO_IPACC_PRE_BMI_ENQ_FRAME	0x00000028
#define NIA_BMI_AC_ENQ_FRAME				0x00000002
#define NIA_BMI_AC_TX_RELEASE				0x000002C0
#define NIA_BMI_AC_RELEASE				0x000000C0
#define NIA_BMI_AC_TX					0x00000274
#define NIA_BMI_AC_FETCH_ALL_FRAME			0x0000020c

/* Port IDs */
#define TX_10G_PORT_BASE		0x30
#define RX_10G_PORT_BASE		0x10

/* BMI Rx port register map */
struct fman_port_rx_bmi_regs {
	u32 fmbm_rcfg;		/* Rx Configuration */
	u32 fmbm_rst;		/* Rx Status */
	u32 fmbm_rda;		/* Rx DMA attributes */
	u32 fmbm_rfp;		/* Rx FIFO Parameters */
	u32 fmbm_rfed;		/* Rx Frame End Data */
	u32 fmbm_ricp;		/* Rx Internal Context Parameters */
	u32 fmbm_rim;		/* Rx Internal Buffer Margins */
	u32 fmbm_rebm;		/* Rx External Buffer Margins */
	u32 fmbm_rfne;		/* Rx Frame Next Engine */
	u32 fmbm_rfca;		/* Rx Frame Command Attributes. */
	u32 fmbm_rfpne;		/* Rx Frame Parser Next Engine */
	u32 fmbm_rpso;		/* Rx Parse Start Offset */
	u32 fmbm_rpp;		/* Rx Policer Profile  */
	u32 fmbm_rccb;		/* Rx Coarse Classification Base */
	u32 fmbm_reth;		/* Rx Excessive Threshold */
	u32 reserved003c[1];	/* (0x03C 0x03F) */
	u32 fmbm_rprai[PORT_PRS_RESULT_WORDS_NUM];
	/* Rx Parse Results Array Init */
	u32 fmbm_rfqid;		/* Rx Frame Queue ID */
	u32 fmbm_refqid;	/* Rx Error Frame Queue ID */
	u32 fmbm_rfsdm;		/* Rx Frame Status Discard Mask */
	u32 fmbm_rfsem;		/* Rx Frame Status Error Mask */
	u32 fmbm_rfene;		/* Rx Frame Enqueue Next Engine */
	u32 reserved0074[0x2];	/* (0x074-0x07C)  */
	u32 fmbm_rcmne;		/* Rx Frame Continuous Mode Next Engine */
	u32 reserved0080[0x20];	/* (0x080 0x0FF)  */
	u32 fmbm_ebmpi[FMAN_PORT_MAX_EXT_POOLS_NUM];
	/* Buffer Manager pool Information- */
	u32 fmbm_acnt[FMAN_PORT_MAX_EXT_POOLS_NUM];	/* Allocate Counter- */
	u32 reserved0130[8];	/* 0x130/0x140 - 0x15F reserved - */
	u32 fmbm_rcgm[PORT_CG_MAP_NUM];	/* Congestion Group Map */
	u32 fmbm_mpd;		/* BM Pool Depletion  */
	u32 reserved0184[0x1F];	/* (0x184 0x1FF) */
	u32 fmbm_rstc;		/* Rx Statistics Counters */
	u32 fmbm_rfrc;		/* Rx Frame Counter */
	u32 fmbm_rfbc;		/* Rx Bad Frames Counter */
	u32 fmbm_rlfc;		/* Rx Large Frames Counter */
	u32 fmbm_rffc;		/* Rx Filter Frames Counter */
	u32 fmbm_rfdc;		/* Rx Frame Discard Counter */
	u32 fmbm_rfldec;		/* Rx Frames List DMA Error Counter */
	u32 fmbm_rodc;		/* Rx Out of Buffers Discard nntr */
	u32 fmbm_rbdc;		/* Rx Buffers Deallocate Counter */
	u32 fmbm_rpec;		/* RX Prepare to enqueue Counte */
	u32 reserved0224[0x16];	/* (0x224 0x27F) */
	u32 fmbm_rpc;		/* Rx Performance Counters */
	u32 fmbm_rpcp;		/* Rx Performance Count Parameters */
	u32 fmbm_rccn;		/* Rx Cycle Counter */
	u32 fmbm_rtuc;		/* Rx Tasks Utilization Counter */
	u32 fmbm_rrquc;		/* Rx Receive Queue Utilization cntr */
	u32 fmbm_rduc;		/* Rx DMA Utilization Counter */
	u32 fmbm_rfuc;		/* Rx FIFO Utilization Counter */
	u32 fmbm_rpac;		/* Rx Pause Activation Counter */
	u32 reserved02a0[0x18];	/* (0x2A0 0x2FF) */
	u32 fmbm_rdcfg[0x3];	/* Rx Debug Configuration */
	u32 fmbm_rgpr;		/* Rx General Purpose Register */
	u32 reserved0310[0x3a];
};

/* BMI Tx port register map */
struct fman_port_tx_bmi_regs {
	u32 fmbm_tcfg;		/* Tx Configuration */
	u32 fmbm_tst;		/* Tx Status */
	u32 fmbm_tda;		/* Tx DMA attributes */
	u32 fmbm_tfp;		/* Tx FIFO Parameters */
	u32 fmbm_tfed;		/* Tx Frame End Data */
	u32 fmbm_ticp;		/* Tx Internal Context Parameters */
	u32 fmbm_tfdne;		/* Tx Frame Dequeue Next Engine. */
	u32 fmbm_tfca;		/* Tx Frame Command attribute. */
	u32 fmbm_tcfqid;	/* Tx Confirmation Frame Queue ID. */
	u32 fmbm_tefqid;	/* Tx Frame Error Queue ID */
	u32 fmbm_tfene;		/* Tx Frame Enqueue Next Engine */
	u32 fmbm_trlmts;	/* Tx Rate Limiter Scale */
	u32 fmbm_trlmt;		/* Tx Rate Limiter */
	u32 reserved0034[0x0e];	/* (0x034-0x6c) */
	u32 fmbm_tccb;		/* Tx Coarse Classification base */
	u32 fmbm_tfne;		/* Tx Frame Next Engine */
	u32 fmbm_tpfcm[0x02];
	/* Tx Priority based Flow Control (PFC) Mapping */
	u32 fmbm_tcmne;		/* Tx Frame Continuous Mode Next Engine */
	u32 reserved0080[0x60];	/* (0x080-0x200) */
	u32 fmbm_tstc;		/* Tx Statistics Counters */
	u32 fmbm_tfrc;		/* Tx Frame Counter */
	u32 fmbm_tfdc;		/* Tx Frames Discard Counter */
	u32 fmbm_tfledc;	/* Tx Frame len error discard cntr */
	u32 fmbm_tfufdc;	/* Tx Frame unsprt frmt discard cntr */
	u32 fmbm_tbdc;		/* Tx Buffers Deallocate Counter */
	u32 reserved0218[0x1A];	/* (0x218-0x280) */
	u32 fmbm_tpc;		/* Tx Performance Counters */
	u32 fmbm_tpcp;		/* Tx Performance Count Parameters */
	u32 fmbm_tccn;		/* Tx Cycle Counter */
	u32 fmbm_ttuc;		/* Tx Tasks Utilization Counter */
	u32 fmbm_ttcquc;	/* Tx Transmit conf Q util Counter */
	u32 fmbm_tduc;		/* Tx DMA Utilization Counter */
	u32 fmbm_tfuc;		/* Tx FIFO Utilization Counter */
	u32 reserved029c[16];	/* (0x29C-0x2FF) */
	u32 fmbm_tdcfg[0x3];	/* Tx Debug Configuration */
	u32 fmbm_tgpr;		/* Tx General Purpose Register */
	u32 reserved0310[0x3a]; /* (0x310-0x3FF) */
};

/* BMI port register map */
union fman_port_bmi_regs {
	struct fman_port_rx_bmi_regs rx;
	struct fman_port_tx_bmi_regs tx;
};

/* QMI port register map */
struct fman_port_qmi_regs {
	u32 fmqm_pnc;		/* PortID n Configuration Register */
	u32 fmqm_pns;		/* PortID n Status Register */
	u32 fmqm_pnts;		/* PortID n Task Status Register */
	u32 reserved00c[4];	/* 0xn00C - 0xn01B */
	u32 fmqm_pnen;		/* PortID n Enqueue NIA Register */
	u32 fmqm_pnetfc;		/* PortID n Enq Total Frame Counter */
	u32 reserved024[2];	/* 0xn024 - 0x02B */
	u32 fmqm_pndn;		/* PortID n Dequeue NIA Register */
	u32 fmqm_pndc;		/* PortID n Dequeue Config Register */
	u32 fmqm_pndtfc;		/* PortID n Dequeue tot Frame cntr */
	u32 fmqm_pndfdc;		/* PortID n Dequeue FQID Dflt Cntr */
	u32 fmqm_pndcc;		/* PortID n Dequeue Confirm Counter */
};

/* QMI dequeue prefetch modes */
enum fman_port_deq_prefetch {
	FMAN_PORT_DEQ_NO_PREFETCH, /* No prefetch mode */
	FMAN_PORT_DEQ_PART_PREFETCH, /* Partial prefetch mode */
	FMAN_PORT_DEQ_FULL_PREFETCH /* Full prefetch mode */
};

/* A structure for defining FM port resources */
struct fman_port_rsrc {
	u32 num; /* Committed required resource */
	u32 extra; /* Extra (not committed) required resource */
};

enum fman_port_dma_swap {
	FMAN_PORT_DMA_NO_SWAP,	/* No swap, transfer data as is */
	FMAN_PORT_DMA_SWAP_LE,
	/* The transferred data should be swapped in PPC Little Endian mode */
	FMAN_PORT_DMA_SWAP_BE
	/* The transferred data should be swapped in Big Endian mode */
};

/* Default port color */
enum fman_port_color {
	FMAN_PORT_COLOR_GREEN,	/* Default port color is green */
	FMAN_PORT_COLOR_YELLOW,	/* Default port color is yellow */
	FMAN_PORT_COLOR_RED,		/* Default port color is red */
	FMAN_PORT_COLOR_OVERRIDE	/* Ignore color */
};

/* QMI dequeue from the SP channel - types */
enum fman_port_deq_type {
	FMAN_PORT_DEQ_BY_PRI,
	/* Priority precedence and Intra-Class scheduling */
	FMAN_PORT_DEQ_ACTIVE_FQ,
	/* Active FQ precedence and Intra-Class scheduling */
	FMAN_PORT_DEQ_ACTIVE_FQ_NO_ICS
	/* Active FQ precedence and override Intra-Class scheduling */
};

/* External buffer pools configuration */
struct fman_port_bpools {
	u8 count;			/* Num of pools to set up */
	bool counters_enable;		/* Enable allocate counters */
	u8 grp_bp_depleted_num;
	/* Number of depleted pools - if reached the BMI indicates
	 * the MAC to send a pause frame
	 */
	struct {
		u8 bpid;		/* BM pool ID */
		u16 size;
		/* Pool's size - must be in ascending order */
		bool is_backup;
		/* If this is a backup pool */
		bool grp_bp_depleted;
		/* Consider this buffer in multiple pools depletion criteria */
		bool single_bp_depleted;
		/* Consider this buffer in single pool depletion criteria */
	} bpool[FMAN_PORT_MAX_EXT_POOLS_NUM];
};

struct fman_port_cfg {
	u32 dflt_fqid;
	u32 err_fqid;
	u8 deq_sp;
	bool deq_high_priority;
	enum fman_port_deq_type deq_type;
	enum fman_port_deq_prefetch deq_prefetch_option;
	u16 deq_byte_cnt;
	u8 cheksum_last_bytes_ignore;
	u8 rx_cut_end_bytes;
	struct fman_buf_pool_depletion buf_pool_depletion;
	bool discard_override;
	bool en_buf_pool_depletion;
	struct fman_ext_pools ext_buf_pools;
	u32 tx_fifo_min_level;
	u32 tx_fifo_low_comf_level;
	u32 rx_pri_elevation;
	u32 rx_fifo_thr;
	struct fman_sp_buf_margins buf_margins;
	u32 int_buf_start_margin;
	struct fman_sp_int_context_data_copy int_context;
	u32 discard_mask;
	u32 err_mask;
	bool forward_reuse_int_context;
	struct fman_buffer_prefix_content buffer_prefix_content;
	bool dont_release_buf;
	bool set_num_of_tasks;
	bool set_num_of_open_dmas;
	bool set_size_of_fifo;
	bool bcb_workaround;

	u8 rx_fd_bits;
	u32 tx_fifo_deq_pipeline_depth;
	bool errata_A006675;
	bool errata_A006320;
	bool excessive_threshold_register;
	bool fmbm_rebm_has_sgd;
	bool fmbm_tfne_has_features;
	bool qmi_deq_options_support;

	enum fman_port_dma_swap dma_swap_data;
	bool dma_ic_stash_on;
	bool dma_header_stash_on;
	bool dma_sg_stash_on;
	bool dma_write_optimize;
	enum fman_port_color color;
	bool sync_req;

	bool no_scatter_gather;
};

struct fman_port_rx_pools_params {
	u8 num_of_pools;
	u16 second_largest_buf_size;
	u16 largest_buf_size;
};

struct fman_port_dts_params {
	void __iomem *base_addr;	/* FMan port virtual memory */
	enum fman_port_type type;	/* Port type */
	u16 speed;			/* Port speed */
	u8 id;				/* HW Port Id */
	u32 qman_channel_id;		/* QMan channel id (non RX only) */
	struct fman *fman;		/* FMan Handle */
};

struct fman_port {
	void *fm;
	struct fman_rev_info rev_info;
	u8 port_id;
	enum fman_port_type port_type;
	u16 port_speed;

	union fman_port_bmi_regs __iomem *bmi_regs;
	struct fman_port_qmi_regs __iomem *qmi_regs;

	struct fman_sp_buffer_offsets buffer_offsets;

	u8 internal_buf_offset;
	struct fman_ext_pools ext_buf_pools;

	u16 max_frame_length;
	struct fman_port_rsrc open_dmas;
	struct fman_port_rsrc tasks;
	struct fman_port_rsrc fifo_bufs;
	struct fman_port_rx_pools_params rx_pools_params;

	struct fman_port_cfg *cfg;
	struct fman_port_dts_params dts_params;

	u8 ext_pools_num;
	u32 max_port_fifo_size;
	u32 max_num_of_ext_pools;
	u32 max_num_of_sub_portals;
	u32 bm_max_num_of_pools;
};

static int init_bmi_rx(struct fman_port *port)
{
	struct fman_port_rx_bmi_regs __iomem *regs = &port->bmi_regs->rx;
	struct fman_port_cfg *cfg = port->cfg;
	u32 tmp;

	/* Rx Configuration register */
	tmp = 0;
	if (cfg->discard_override)
		tmp |= BMI_PORT_CFG_FDOVR;
	iowrite32be(tmp, &regs->fmbm_rcfg);

	/* DMA attributes */
	tmp = (u32)cfg->dma_swap_data << BMI_DMA_ATTR_SWP_SHIFT;
	if (cfg->dma_ic_stash_on)
		tmp |= BMI_DMA_ATTR_IC_STASH_ON;
	if (cfg->dma_header_stash_on)
		tmp |= BMI_DMA_ATTR_HDR_STASH_ON;
	if (cfg->dma_sg_stash_on)
		tmp |= BMI_DMA_ATTR_SG_STASH_ON;
	if (cfg->dma_write_optimize)
		tmp |= BMI_DMA_ATTR_WRITE_OPTIMIZE;
	iowrite32be(tmp, &regs->fmbm_rda);

	/* Rx FIFO parameters */
	tmp = (cfg->rx_pri_elevation / PORT_BMI_FIFO_UNITS - 1) <<
		BMI_RX_FIFO_PRI_ELEVATION_SHIFT;
	tmp |= cfg->rx_fifo_thr / PORT_BMI_FIFO_UNITS - 1;
	iowrite32be(tmp, &regs->fmbm_rfp);

	if (cfg->excessive_threshold_register)
		/* always allow access to the extra resources */
		iowrite32be(BMI_RX_FIFO_THRESHOLD_ETHE, &regs->fmbm_reth);

	/* Frame end data */
	tmp = (cfg->cheksum_last_bytes_ignore & BMI_FRAME_END_CS_IGNORE_MASK) <<
		BMI_FRAME_END_CS_IGNORE_SHIFT;
	tmp |= (cfg->rx_cut_end_bytes & BMI_RX_FRAME_END_CUT_MASK) <<
		BMI_RX_FRAME_END_CUT_SHIFT;
	if (cfg->errata_A006320)
		tmp &= 0xffe0ffff;
	iowrite32be(tmp, &regs->fmbm_rfed);

	/* Internal context parameters */
	tmp = ((cfg->int_context.ext_buf_offset / PORT_IC_OFFSET_UNITS) &
		BMI_IC_TO_EXT_MASK) << BMI_IC_TO_EXT_SHIFT;
	tmp |= ((cfg->int_context.int_context_offset / PORT_IC_OFFSET_UNITS) &
		BMI_IC_FROM_INT_MASK) << BMI_IC_FROM_INT_SHIFT;
	tmp |= (cfg->int_context.size / PORT_IC_OFFSET_UNITS) &
		BMI_IC_SIZE_MASK;
	iowrite32be(tmp, &regs->fmbm_ricp);

	/* Internal buffer offset */
	tmp = ((cfg->int_buf_start_margin / PORT_IC_OFFSET_UNITS) &
		BMI_INT_BUF_MARG_MASK) << BMI_INT_BUF_MARG_SHIFT;
	iowrite32be(tmp, &regs->fmbm_rim);

	/* External buffer margins */
	tmp = (cfg->buf_margins.start_margins & BMI_EXT_BUF_MARG_START_MASK) <<
		BMI_EXT_BUF_MARG_START_SHIFT;
	tmp |= cfg->buf_margins.end_margins & BMI_EXT_BUF_MARG_END_MASK;
	if (cfg->fmbm_rebm_has_sgd && cfg->no_scatter_gather)
		tmp |= BMI_SG_DISABLE;
	iowrite32be(tmp, &regs->fmbm_rebm);

	/* Frame attributes */
	tmp = BMI_CMD_RX_MR_DEF;
	tmp |= BMI_CMD_ATTR_ORDER;
	tmp |= (u32)cfg->color << BMI_CMD_ATTR_COLOR_SHIFT;
	if (cfg->sync_req)
		tmp |= BMI_CMD_ATTR_SYNC;

	iowrite32be(tmp, &regs->fmbm_rfca);

	/* NIA */
	tmp = (u32)cfg->rx_fd_bits << BMI_NEXT_ENG_FD_BITS_SHIFT;

	if (cfg->errata_A006675)
		tmp |= NIA_ENG_FM_CTL |
		       NIA_FM_CTL_AC_NO_IPACC_PRE_BMI_ENQ_FRAME;
	else
		tmp |= NIA_ENG_BMI | NIA_BMI_AC_ENQ_FRAME;
	iowrite32be(tmp, &regs->fmbm_rfne);

	/* Enqueue NIA */
	iowrite32be(NIA_ENG_QMI_ENQ | NIA_ORDER_RESTOR, &regs->fmbm_rfene);

	/* Default/error queues */
	iowrite32be((cfg->dflt_fqid & DFLT_FQ_ID), &regs->fmbm_rfqid);
	iowrite32be((cfg->err_fqid & DFLT_FQ_ID), &regs->fmbm_refqid);

	/* Discard/error masks */
	iowrite32be(cfg->discard_mask, &regs->fmbm_rfsdm);
	iowrite32be(cfg->err_mask, &regs->fmbm_rfsem);

	return 0;
}

static int init_bmi_tx(struct fman_port *port)
{
	struct fman_port_tx_bmi_regs __iomem *regs = &port->bmi_regs->tx;
	struct fman_port_cfg *cfg = port->cfg;
	u32 tmp;

	/* Tx Configuration register */
	tmp = 0;
	iowrite32be(tmp, &regs->fmbm_tcfg);

	/* DMA attributes */
	tmp = (u32)cfg->dma_swap_data << BMI_DMA_ATTR_SWP_SHIFT;
	if (cfg->dma_ic_stash_on)
		tmp |= BMI_DMA_ATTR_IC_STASH_ON;
	if (cfg->dma_header_stash_on)
		tmp |= BMI_DMA_ATTR_HDR_STASH_ON;
	if (cfg->dma_sg_stash_on)
		tmp |= BMI_DMA_ATTR_SG_STASH_ON;
	iowrite32be(tmp, &regs->fmbm_tda);

	/* Tx FIFO parameters */
	tmp = (cfg->tx_fifo_min_level / PORT_BMI_FIFO_UNITS) <<
		BMI_TX_FIFO_MIN_FILL_SHIFT;
	tmp |= ((cfg->tx_fifo_deq_pipeline_depth - 1) &
		BMI_FIFO_PIPELINE_DEPTH_MASK) << BMI_FIFO_PIPELINE_DEPTH_SHIFT;
	tmp |= (cfg->tx_fifo_low_comf_level / PORT_BMI_FIFO_UNITS) - 1;
	iowrite32be(tmp, &regs->fmbm_tfp);

	/* Frame end data */
	tmp = (cfg->cheksum_last_bytes_ignore & BMI_FRAME_END_CS_IGNORE_MASK) <<
		BMI_FRAME_END_CS_IGNORE_SHIFT;
	iowrite32be(tmp, &regs->fmbm_tfed);

	/* Internal context parameters */
	tmp = ((cfg->int_context.ext_buf_offset / PORT_IC_OFFSET_UNITS) &
		BMI_IC_TO_EXT_MASK) << BMI_IC_TO_EXT_SHIFT;
	tmp |= ((cfg->int_context.int_context_offset / PORT_IC_OFFSET_UNITS) &
		BMI_IC_FROM_INT_MASK) << BMI_IC_FROM_INT_SHIFT;
	tmp |= (cfg->int_context.size / PORT_IC_OFFSET_UNITS) &
		BMI_IC_SIZE_MASK;
	iowrite32be(tmp, &regs->fmbm_ticp);

	/* Frame attributes */
	tmp = BMI_CMD_TX_MR_DEF;
	tmp |= BMI_CMD_ATTR_ORDER;
	tmp |= (u32)cfg->color << BMI_CMD_ATTR_COLOR_SHIFT;
	iowrite32be(tmp, &regs->fmbm_tfca);

	/* Dequeue NIA + enqueue NIA */
	iowrite32be(NIA_ENG_QMI_DEQ, &regs->fmbm_tfdne);
	iowrite32be(NIA_ENG_QMI_ENQ | NIA_ORDER_RESTOR, &regs->fmbm_tfene);
	if (cfg->fmbm_tfne_has_features)
		iowrite32be(!cfg->dflt_fqid ?
			    BMI_EBD_EN | NIA_BMI_AC_FETCH_ALL_FRAME :
			    NIA_BMI_AC_FETCH_ALL_FRAME, &regs->fmbm_tfne);
	if (!cfg->dflt_fqid && cfg->dont_release_buf) {
		iowrite32be(DFLT_FQ_ID, &regs->fmbm_tcfqid);
		iowrite32be(NIA_ENG_BMI | NIA_BMI_AC_TX_RELEASE,
			    &regs->fmbm_tfene);
		if (cfg->fmbm_tfne_has_features)
			iowrite32be(ioread32be(&regs->fmbm_tfne) & ~BMI_EBD_EN,
				    &regs->fmbm_tfne);
	}

	/* Confirmation/error queues */
	if (cfg->dflt_fqid || !cfg->dont_release_buf)
		iowrite32be(cfg->dflt_fqid & DFLT_FQ_ID, &regs->fmbm_tcfqid);
	iowrite32be((cfg->err_fqid & DFLT_FQ_ID), &regs->fmbm_tefqid);

	return 0;
}

static int init_qmi(struct fman_port *port)
{
	struct fman_port_qmi_regs __iomem *regs = port->qmi_regs;
	struct fman_port_cfg *cfg = port->cfg;
	u32 tmp;

	/* Rx port configuration */
	if (port->port_type == FMAN_PORT_TYPE_RX) {
		/* Enqueue NIA */
		iowrite32be(NIA_ENG_BMI | NIA_BMI_AC_RELEASE, &regs->fmqm_pnen);
		return 0;
	}

	/* Continue with Tx port configuration */
	if (port->port_type == FMAN_PORT_TYPE_TX) {
		/* Enqueue NIA */
		iowrite32be(NIA_ENG_BMI | NIA_BMI_AC_TX_RELEASE,
			    &regs->fmqm_pnen);
		/* Dequeue NIA */
		iowrite32be(NIA_ENG_BMI | NIA_BMI_AC_TX, &regs->fmqm_pndn);
	}

	/* Dequeue Configuration register */
	tmp = 0;
	if (cfg->deq_high_priority)
		tmp |= QMI_DEQ_CFG_PRI;

	switch (cfg->deq_type) {
	case FMAN_PORT_DEQ_BY_PRI:
		tmp |= QMI_DEQ_CFG_TYPE1;
		break;
	case FMAN_PORT_DEQ_ACTIVE_FQ:
		tmp |= QMI_DEQ_CFG_TYPE2;
		break;
	case FMAN_PORT_DEQ_ACTIVE_FQ_NO_ICS:
		tmp |= QMI_DEQ_CFG_TYPE3;
		break;
	default:
		return -EINVAL;
	}

	if (cfg->qmi_deq_options_support) {
		switch (cfg->deq_prefetch_option) {
		case FMAN_PORT_DEQ_NO_PREFETCH:
			break;
		case FMAN_PORT_DEQ_PART_PREFETCH:
			tmp |= QMI_DEQ_CFG_PREFETCH_PARTIAL;
			break;
		case FMAN_PORT_DEQ_FULL_PREFETCH:
			tmp |= QMI_DEQ_CFG_PREFETCH_FULL;
			break;
		default:
			return -EINVAL;
		}
	}
	tmp |= (cfg->deq_sp & QMI_DEQ_CFG_SP_MASK) << QMI_DEQ_CFG_SP_SHIFT;
	tmp |= cfg->deq_byte_cnt;
	iowrite32be(tmp, &regs->fmqm_pndc);

	return 0;
}

static int init(struct fman_port *port)
{
	int err;

	/* Init BMI registers */
	switch (port->port_type) {
	case FMAN_PORT_TYPE_RX:
		err = init_bmi_rx(port);
		break;
	case FMAN_PORT_TYPE_TX:
		err = init_bmi_tx(port);
		break;
	default:
		return -EINVAL;
	}

	if (err)
		return err;

	/* Init QMI registers */
	err = init_qmi(port);
	return err;

	return 0;
}

static int set_bpools(const struct fman_port *port,
		      const struct fman_port_bpools *bp)
{
	u32 __iomem *bp_reg, *bp_depl_reg;
	u32 tmp;
	u8 i, max_bp_num;
	bool grp_depl_used = false, rx_port;

	switch (port->port_type) {
	case FMAN_PORT_TYPE_RX:
		max_bp_num = port->ext_pools_num;
		rx_port = true;
		bp_reg = port->bmi_regs->rx.fmbm_ebmpi;
		bp_depl_reg = &port->bmi_regs->rx.fmbm_mpd;
		break;
	default:
		return -EINVAL;
	}

	if (rx_port) {
		/* Check buffers are provided in ascending order */
		for (i = 0; (i < (bp->count - 1) &&
			     (i < FMAN_PORT_MAX_EXT_POOLS_NUM - 1)); i++) {
			if (bp->bpool[i].size > bp->bpool[i + 1].size)
				return -EINVAL;
		}
	}

	/* Set up external buffers pools */
	for (i = 0; i < bp->count; i++) {
		tmp = BMI_EXT_BUF_POOL_VALID;
		tmp |= ((u32)bp->bpool[i].bpid <<
			BMI_EXT_BUF_POOL_ID_SHIFT) & BMI_EXT_BUF_POOL_ID_MASK;

		if (rx_port) {
			if (bp->counters_enable)
				tmp |= BMI_EXT_BUF_POOL_EN_COUNTER;

			if (bp->bpool[i].is_backup)
				tmp |= BMI_EXT_BUF_POOL_BACKUP;

			tmp |= (u32)bp->bpool[i].size;
		}

		iowrite32be(tmp, &bp_reg[i]);
	}

	/* Clear unused pools */
	for (i = bp->count; i < max_bp_num; i++)
		iowrite32be(0, &bp_reg[i]);

	/* Pools depletion */
	tmp = 0;
	for (i = 0; i < FMAN_PORT_MAX_EXT_POOLS_NUM; i++) {
		if (bp->bpool[i].grp_bp_depleted) {
			grp_depl_used = true;
			tmp |= 0x80000000 >> i;
		}

		if (bp->bpool[i].single_bp_depleted)
			tmp |= 0x80 >> i;
	}

	if (grp_depl_used)
		tmp |= ((u32)bp->grp_bp_depleted_num - 1) <<
		    BMI_POOL_DEP_NUM_OF_POOLS_SHIFT;

	iowrite32be(tmp, bp_depl_reg);
	return 0;
}

static bool is_init_done(struct fman_port_cfg *cfg)
{
	/* Checks if FMan port driver parameters were initialized */
	if (!cfg)
		return true;

	return false;
}

static int verify_size_of_fifo(struct fman_port *port)
{
	u32 min_fifo_size_required = 0, opt_fifo_size_for_b2b = 0;

	/* TX Ports */
	if (port->port_type == FMAN_PORT_TYPE_TX) {
		min_fifo_size_required = (u32)
		    (roundup(port->max_frame_length,
			     FMAN_BMI_FIFO_UNITS) + (3 * FMAN_BMI_FIFO_UNITS));

		min_fifo_size_required +=
		    port->cfg->tx_fifo_deq_pipeline_depth *
		    FMAN_BMI_FIFO_UNITS;

		opt_fifo_size_for_b2b = min_fifo_size_required;

		/* Add some margin for back-to-back capability to improve
		 * performance, allows the hardware to pipeline new frame dma
		 * while the previous frame not yet transmitted.
		 */
		if (port->port_speed == 10000)
			opt_fifo_size_for_b2b += 3 * FMAN_BMI_FIFO_UNITS;
		else
			opt_fifo_size_for_b2b += 2 * FMAN_BMI_FIFO_UNITS;
	}

	/* RX Ports */
	else if (port->port_type == FMAN_PORT_TYPE_RX) {
		if (port->rev_info.major >= 6)
			min_fifo_size_required = (u32)
			(roundup(port->max_frame_length,
				 FMAN_BMI_FIFO_UNITS) +
				 (5 * FMAN_BMI_FIFO_UNITS));
			/* 4 according to spec + 1 for FOF>0 */
		else
			min_fifo_size_required = (u32)
			(roundup(min(port->max_frame_length,
				     port->rx_pools_params.largest_buf_size),
				     FMAN_BMI_FIFO_UNITS) +
				     (7 * FMAN_BMI_FIFO_UNITS));

		opt_fifo_size_for_b2b = min_fifo_size_required;

		/* Add some margin for back-to-back capability to improve
		 * performance,allows the hardware to pipeline new frame dma
		 * while the previous frame not yet transmitted.
		 */
		if (port->port_speed == 10000)
			opt_fifo_size_for_b2b += 8 * FMAN_BMI_FIFO_UNITS;
		else
			opt_fifo_size_for_b2b += 3 * FMAN_BMI_FIFO_UNITS;
	}

	WARN_ON(min_fifo_size_required <= 0);
	WARN_ON(opt_fifo_size_for_b2b < min_fifo_size_required);

	/* Verify the size  */
	if (port->fifo_bufs.num < min_fifo_size_required)
		pr_debug("FIFO size should be enlarged to %d bytes\n",
			 min_fifo_size_required);
	else if (port->fifo_bufs.num < opt_fifo_size_for_b2b)
		pr_debug("For b2b processing,FIFO may be enlarged to %d bytes\n",
			 opt_fifo_size_for_b2b);

	return 0;
}

static int set_ext_buffer_pools(struct fman_port *port)
{
	struct fman_ext_pools *ext_buf_pools = &port->cfg->ext_buf_pools;
	struct fman_buf_pool_depletion *buf_pool_depletion =
	&port->cfg->buf_pool_depletion;
	u8 ordered_array[FMAN_PORT_MAX_EXT_POOLS_NUM];
	u16 sizes_array[BM_MAX_NUM_OF_POOLS];
	int i = 0, j = 0, err;
	struct fman_port_bpools bpools;

	memset(&ordered_array, 0, sizeof(u8) * FMAN_PORT_MAX_EXT_POOLS_NUM);
	memset(&sizes_array, 0, sizeof(u16) * BM_MAX_NUM_OF_POOLS);
	memcpy(&port->ext_buf_pools, ext_buf_pools,
	       sizeof(struct fman_ext_pools));

	fman_sp_set_buf_pools_in_asc_order_of_buf_sizes(ext_buf_pools,
							ordered_array,
							sizes_array);

	memset(&bpools, 0, sizeof(struct fman_port_bpools));
	bpools.count = ext_buf_pools->num_of_pools_used;
	bpools.counters_enable = true;
	for (i = 0; i < ext_buf_pools->num_of_pools_used; i++) {
		bpools.bpool[i].bpid = ordered_array[i];
		bpools.bpool[i].size = sizes_array[ordered_array[i]];
	}

	/* save pools parameters for later use */
	port->rx_pools_params.num_of_pools = ext_buf_pools->num_of_pools_used;
	port->rx_pools_params.largest_buf_size =
	    sizes_array[ordered_array[ext_buf_pools->num_of_pools_used - 1]];
	port->rx_pools_params.second_largest_buf_size =
	    sizes_array[ordered_array[ext_buf_pools->num_of_pools_used - 2]];

	/* FMBM_RMPD reg. - pool depletion */
	if (buf_pool_depletion->pools_grp_mode_enable) {
		bpools.grp_bp_depleted_num = buf_pool_depletion->num_of_pools;
		for (i = 0; i < port->bm_max_num_of_pools; i++) {
			if (buf_pool_depletion->pools_to_consider[i]) {
				for (j = 0; j < ext_buf_pools->
				     num_of_pools_used; j++) {
					if (i == ordered_array[j]) {
						bpools.bpool[j].
						    grp_bp_depleted = true;
						break;
					}
				}
			}
		}
	}

	if (buf_pool_depletion->single_pool_mode_enable) {
		for (i = 0; i < port->bm_max_num_of_pools; i++) {
			if (buf_pool_depletion->
			    pools_to_consider_for_single_mode[i]) {
				for (j = 0; j < ext_buf_pools->
				     num_of_pools_used; j++) {
					if (i == ordered_array[j]) {
						bpools.bpool[j].
						    single_bp_depleted = true;
						break;
					}
				}
			}
		}
	}

	err = set_bpools(port, &bpools);
	if (err != 0) {
		pr_err("FMan port: set_bpools\n");
		return -EINVAL;
	}

	return 0;
}

static int init_low_level_driver(struct fman_port *port)
{
	struct fman_port_cfg *cfg = port->cfg;
	u32 tmp_val;

	switch (port->port_type) {
	case FMAN_PORT_TYPE_RX:
		cfg->err_mask = (RX_ERRS_TO_ENQ & ~cfg->discard_mask);
		if (cfg->forward_reuse_int_context)
			cfg->rx_fd_bits = (u8)(BMI_PORT_RFNE_FRWD_RPD >> 24);
		break;
	default:
		break;
	}

	tmp_val = (u32)((port->internal_buf_offset % OFFSET_UNITS) ?
		(port->internal_buf_offset / OFFSET_UNITS + 1) :
		(port->internal_buf_offset / OFFSET_UNITS));
	port->internal_buf_offset = (u8)(tmp_val * OFFSET_UNITS);
	port->cfg->int_buf_start_margin = port->internal_buf_offset;

	if (init(port) != 0) {
		pr_err("fman_port_init\n");
		return -ENODEV;
	}

	/* The code bellow is a trick so the FM will not release the buffer
	 * to BM nor will try to enqueue the frame to QM
	 */
	if (port->port_type == FMAN_PORT_TYPE_TX) {
		if (!cfg->dflt_fqid && cfg->dont_release_buf) {
			/* override fmbm_tcfqid 0 with a false non-0 value.
			 * This will force FM to act according to tfene.
			 * Otherwise, if fmbm_tcfqid is 0 the FM will release
			 * buffers to BM regardless of fmbm_tfene
			 */
			out_be32(&port->bmi_regs->tx.fmbm_tcfqid, 0xFFFFFF);
			out_be32(&port->bmi_regs->tx.fmbm_tfene,
				 NIA_ENG_BMI | NIA_BMI_AC_TX_RELEASE);
		}
	}

	return 0;
}

static int fill_soc_specific_params(struct fman_port *port)
{
	u32 bmi_max_fifo_size;

	bmi_max_fifo_size = fman_get_bmi_max_fifo_size(port->fm);
	port->max_port_fifo_size = MAX_PORT_FIFO_SIZE(bmi_max_fifo_size);
	port->bm_max_num_of_pools = 64;

	/* P4080 - Major 2
	 * P2041/P3041/P5020/P5040 - Major 3
	 * Tx/Bx - Major 6
	 */
	switch (port->rev_info.major) {
	case 2:
	case 3:
		port->max_num_of_ext_pools		= 4;
		port->max_num_of_sub_portals		= 12;
		break;

	case 6:
		port->max_num_of_ext_pools		= 8;
		port->max_num_of_sub_portals		= 16;
		break;

	default:
		pr_err("Unsupported FMan version\n");
		return -EINVAL;
	}

	return 0;
}

static int get_dflt_fifo_deq_pipeline_depth(u8 major, enum fman_port_type type,
					    u16 speed)
{
	switch (type) {
	case FMAN_PORT_TYPE_RX:
	case FMAN_PORT_TYPE_TX:
		switch (speed) {
		case 10000:
			return 4;
		case 1000:
			if (major >= 6)
				return 2;
			else
				return 1;
		default:
			return 0;
		}
	default:
		return 0;
	}
}

static int get_dflt_num_of_tasks(u8 major, enum fman_port_type type,
				 u16 speed)
{
	switch (type) {
	case FMAN_PORT_TYPE_RX:
	case FMAN_PORT_TYPE_TX:
		switch (speed) {
		case 10000:
			return 16;
		case 1000:
			if (major >= 6)
				return 4;
			else
				return 3;
		default:
			return 0;
		}
	default:
		return 0;
	}
}

static int get_dflt_extra_num_of_tasks(u8 major, enum fman_port_type type,
				       u16 speed)
{
	switch (type) {
	case FMAN_PORT_TYPE_RX:
		/* FMan V3 */
		if (major >= 6)
			return 0;

		/* FMan V2 */
		if (speed == 10000)
			return 8;
		else
			return 2;
	case FMAN_PORT_TYPE_TX:
	default:
		return 0;
	}
}

static int get_dflt_num_of_open_dmas(u8 major, enum fman_port_type type,
				     u16 speed)
{
	int val;

	if (major >= 6) {
		switch (type) {
		case FMAN_PORT_TYPE_TX:
			if (speed == 10000)
				val = 12;
			else
				val = 3;
			break;
		case FMAN_PORT_TYPE_RX:
			if (speed == 10000)
				val = 8;
			else
				val = 2;
			break;
		default:
			return 0;
		}
	} else {
		switch (type) {
		case FMAN_PORT_TYPE_TX:
		case FMAN_PORT_TYPE_RX:
			if (speed == 10000)
				val = 8;
			else
				val = 1;
			break;
		default:
			val = 0;
		}
	}

	return val;
}

static int get_dflt_extra_num_of_open_dmas(u8 major, enum fman_port_type type,
					   u16 speed)
{
	/* FMan V3 */
	if (major >= 6)
		return 0;

	/* FMan V2 */
	switch (type) {
	case FMAN_PORT_TYPE_RX:
	case FMAN_PORT_TYPE_TX:
		if (speed == 10000)
			return 8;
		else
			return 1;
	default:
		return 0;
	}
}

static int get_dflt_num_of_fifo_bufs(u8 major, enum fman_port_type type,
				     u16 speed)
{
	int val;

	if (major >= 6) {
		switch (type) {
		case FMAN_PORT_TYPE_TX:
			if (speed == 10000)
				val = 64;
			else
				val = 50;
			break;
		case FMAN_PORT_TYPE_RX:
			if (speed == 10000)
				val = 96;
			else
				val = 50;
			break;
		default:
			val = 0;
		}
	} else {
		switch (type) {
		case FMAN_PORT_TYPE_TX:
			if (speed == 10000)
				val = 48;
			else
				val = 44;
			break;
		case FMAN_PORT_TYPE_RX:
			if (speed == 10000)
				val = 48;
			else
				val = 45;
			break;
		default:
			val = 0;
		}
	}

	return val;
}

static void set_dflt_cfg(struct fman_port *port,
			 struct fman_port_params *port_params)
{
	struct fman_port_cfg *cfg = port->cfg;

	cfg->dma_swap_data = FMAN_PORT_DMA_NO_SWAP;
	cfg->dma_write_optimize = true;
	cfg->color = FMAN_PORT_COLOR_GREEN;
	cfg->rx_cut_end_bytes = DFLT_PORT_CUT_BYTES_FROM_END;
	cfg->rx_pri_elevation = BMI_PRIORITY_ELEVATION_LEVEL;
	cfg->rx_fifo_thr = BMI_FIFO_THRESHOLD;
	cfg->tx_fifo_low_comf_level = (5 * 1024);
	cfg->deq_type = FMAN_PORT_DEQ_BY_PRI;
	cfg->sync_req = true;
	cfg->deq_prefetch_option = FMAN_PORT_DEQ_FULL_PREFETCH;
	cfg->tx_fifo_deq_pipeline_depth =
		BMI_DEQUEUE_PIPELINE_DEPTH(port->port_type, port->port_speed);
	cfg->deq_byte_cnt = QMI_BYTE_COUNT_LEVEL_CONTROL(port->port_type);

	cfg->rx_pri_elevation =
		DFLT_PORT_RX_FIFO_PRI_ELEVATION_LEV(port->max_port_fifo_size);
	port->cfg->rx_fifo_thr =
		DFLT_PORT_RX_FIFO_THRESHOLD(port->rev_info.major,
					    port->max_port_fifo_size);

	if ((port->rev_info.major == 6) &&
	    ((port->rev_info.minor == 0) || (port->rev_info.minor == 3)))
		cfg->errata_A006320 = true;

	/* Excessive Threshold register - exists for pre-FMv3 chips only */
	if (port->rev_info.major < 6) {
		cfg->excessive_threshold_register = true;
	} else {
		cfg->fmbm_rebm_has_sgd = true;
		cfg->fmbm_tfne_has_features = true;
	}

	cfg->qmi_deq_options_support = true;

	cfg->buffer_prefix_content.data_align =
		DFLT_PORT_BUFFER_PREFIX_CONTEXT_DATA_ALIGN;
}

static void set_rx_dflt_cfg(struct fman_port *port,
			    struct fman_port_params *port_params)
{
	port->cfg->discard_mask = DFLT_PORT_ERRORS_TO_DISCARD;

	memcpy(&port->cfg->ext_buf_pools,
	       &port_params->specific_params.rx_params.ext_buf_pools,
	       sizeof(struct fman_ext_pools));
	port->cfg->err_fqid =
		port_params->specific_params.rx_params.err_fqid;
	port->cfg->dflt_fqid =
		port_params->specific_params.rx_params.dflt_fqid;

	/* Set BCB workaround on Rx ports, only for B4860 rev1 */
	if (port->rev_info.major >= 6) {
		unsigned int svr;

		svr = mfspr(SPRN_SVR);
		if ((SVR_SOC_VER(svr) == SVR_B4860) && (SVR_MAJ(svr) == 1))
			port->cfg->bcb_workaround = true;
	}
}

static void set_tx_dflt_cfg(struct fman_port *port,
			    struct fman_port_params *port_params,
			    struct fman_port_dts_params *dts_params)
{
	port->cfg->tx_fifo_deq_pipeline_depth =
		get_dflt_fifo_deq_pipeline_depth(port->rev_info.major,
						 port->port_type,
						 port->port_speed);
	port->cfg->err_fqid =
		port_params->specific_params.non_rx_params.err_fqid;
	port->cfg->deq_sp =
		(u8)(dts_params->qman_channel_id & QMI_DEQ_CFG_SUBPORTAL_MASK);
	port->cfg->dflt_fqid =
		port_params->specific_params.non_rx_params.dflt_fqid;
	port->cfg->deq_high_priority = true;
}

int fman_port_config(struct fman_port *port, struct fman_port_params *params)
{
	void __iomem *base_addr = port->dts_params.base_addr;
	int err;

	/* Allocate the FM driver's parameters structure */
	port->cfg = kzalloc(sizeof(*port->cfg), GFP_KERNEL);
	if (!port->cfg)
		goto err_params;

	/* Initialize FM port parameters which will be kept by the driver */
	port->port_type = port->dts_params.type;
	port->port_speed = port->dts_params.speed;
	port->port_id = port->dts_params.id;
	port->fm = port->dts_params.fman;
	port->ext_pools_num = (u8)8;

	/* get FM revision */
	fman_get_revision(port->fm, &port->rev_info);

	err = fill_soc_specific_params(port);
	if (err)
		goto err_port_cfg;

	switch (port->port_type) {
	case FMAN_PORT_TYPE_RX:
		set_rx_dflt_cfg(port, params);
	case FMAN_PORT_TYPE_TX:
		set_tx_dflt_cfg(port, params, &port->dts_params);
	default:
		set_dflt_cfg(port, params);
	}

	/* Continue with other parameters */
	/* set memory map pointers */
	port->bmi_regs = (union fman_port_bmi_regs __iomem *)
			 (base_addr + BMI_PORT_REGS_OFFSET);
	port->qmi_regs = (struct fman_port_qmi_regs __iomem *)
			 (base_addr + QMI_PORT_REGS_OFFSET);

	port->max_frame_length = DFLT_PORT_MAX_FRAME_LENGTH;
	/* resource distribution. */

	port->fifo_bufs.num =
	get_dflt_num_of_fifo_bufs(port->rev_info.major, port->port_type,
				  port->port_speed) * FMAN_BMI_FIFO_UNITS;
	port->fifo_bufs.extra =
	DFLT_PORT_EXTRA_NUM_OF_FIFO_BUFS * FMAN_BMI_FIFO_UNITS;

	port->open_dmas.num =
	get_dflt_num_of_open_dmas(port->rev_info.major,
				  port->port_type, port->port_speed);
	port->open_dmas.extra =
	get_dflt_extra_num_of_open_dmas(port->rev_info.major,
					port->port_type, port->port_speed);
	port->tasks.num =
	get_dflt_num_of_tasks(port->rev_info.major,
			      port->port_type, port->port_speed);
	port->tasks.extra =
	get_dflt_extra_num_of_tasks(port->rev_info.major,
				    port->port_type, port->port_speed);

	/* FM_HEAVY_TRAFFIC_SEQUENCER_HANG_ERRATA_FMAN_A006981 errata
	 * workaround
	 */
	if ((port->rev_info.major == 6) && (port->rev_info.minor == 0) &&
	    (((port->port_type == FMAN_PORT_TYPE_TX) &&
	    (port->port_speed == 1000)))) {
		port->open_dmas.num = 16;
		port->open_dmas.extra = 0;
	}

	if (port->rev_info.major >= 6 &&
	    port->port_type == FMAN_PORT_TYPE_TX &&
	    port->port_speed == 1000) {
		/* FM_WRONG_RESET_VALUES_ERRATA_FMAN_A005127 Errata
		 * workaround
		 */
		if (port->rev_info.major >= 6) {
			u32 reg;

			reg = 0x00001013;
			out_be32(&port->bmi_regs->tx.fmbm_tfp, reg);
		}
	}

	return 0;

err_port_cfg:
	kfree(port->cfg);
err_params:
	kfree(port);
	return -EINVAL;
}
EXPORT_SYMBOL(fman_port_config);

int fman_port_init(struct fman_port *port)
{
	struct fman_port_cfg *cfg;
	int err;
	struct fman_port_init_params params;

	if (is_init_done(port->cfg))
		return -EINVAL;

	err = fman_sp_build_buffer_struct(&port->cfg->int_context,
					  &port->cfg->buffer_prefix_content,
					  &port->cfg->buf_margins,
					  &port->buffer_offsets,
					  &port->internal_buf_offset);
	if (err)
		return err;

	/* FM_HEAVY_TRAFFIC_HANG_ERRATA_FMAN_A005669 Errata workaround */
	if (port->rev_info.major >= 6 && (port->cfg->bcb_workaround) &&
	    ((port->port_type == FMAN_PORT_TYPE_RX) &&
	    (port->port_speed == 1000))) {
		port->cfg->discard_mask |= FM_PORT_FRM_ERR_PHYSICAL;
		port->fifo_bufs.num += 4 * 1024;
	}

	cfg = port->cfg;

	if (port->port_type == FMAN_PORT_TYPE_RX) {
		/* Call the external Buffer routine which also checks fifo
		 * size and updates it if necessary
		 */
		/* define external buffer pools and pool depletion */
		err = set_ext_buffer_pools(port);
		if (err)
			return err;
		/* check if the largest external buffer pool is large enough */
		if (cfg->buf_margins.start_margins + MIN_EXT_BUF_SIZE +
		    cfg->buf_margins.end_margins >
		    port->rx_pools_params.largest_buf_size) {
			pr_err("buf_margins.start_margins (%d) + minimum buf size (64) + buf_margins.end_margins (%d) is larger than maximum external buffer size (%d)\n",
			       cfg->buf_margins.start_margins,
			       cfg->buf_margins.end_margins,
			       port->rx_pools_params.largest_buf_size);
			return -EINVAL;
		}
	}

	/* Call FM module routine for communicating parameters */
	memset(&params, 0, sizeof(params));
	params.port_id = port->port_id;
	params.port_type = port->port_type;
	params.port_speed = port->port_speed;
	params.num_of_tasks = (u8)port->tasks.num;
	params.num_of_extra_tasks = (u8)port->tasks.extra;
	params.num_of_open_dmas = (u8)port->open_dmas.num;
	params.num_of_extra_open_dmas = (u8)port->open_dmas.extra;

	if (port->fifo_bufs.num) {
		err = verify_size_of_fifo(port);
		if (err)
			return err;
	}
	params.size_of_fifo = port->fifo_bufs.num;
	params.extra_size_of_fifo = port->fifo_bufs.extra;
	params.deq_pipeline_depth = port->cfg->tx_fifo_deq_pipeline_depth;
	params.max_frame_length = port->max_frame_length;

	err = fman_set_port_params(port->fm, &params);
	if (err)
		return err;

	err = init_low_level_driver(port);
	if (err)
		return err;

	kfree(port->cfg);
	port->cfg = NULL;

	return 0;
}
EXPORT_SYMBOL(fman_port_init);

int fman_port_cfg_buf_prefix_content(struct fman_port *port,
				     struct fman_buffer_prefix_content *
				     buffer_prefix_content)
{
	if (is_init_done(port->cfg))
		return -EINVAL;

	memcpy(&port->cfg->buffer_prefix_content,
	       buffer_prefix_content,
	       sizeof(struct fman_buffer_prefix_content));
	/* if data_align was not initialized by user,
	 * we return to driver's default
	 */
	if (!port->cfg->buffer_prefix_content.data_align)
		port->cfg->buffer_prefix_content.data_align =
		DFLT_PORT_BUFFER_PREFIX_CONTEXT_DATA_ALIGN;

	return 0;
}
EXPORT_SYMBOL(fman_port_cfg_buf_prefix_content);

int fman_port_disable(struct fman_port *port)
{
	u32 __iomem *bmi_cfg_reg, *bmi_status_reg, tmp;
	bool rx_port, failure = false;
	int count;

	if (!is_init_done(port->cfg))
		return -EINVAL;

	switch (port->port_type) {
	case FMAN_PORT_TYPE_RX:
		bmi_cfg_reg = &port->bmi_regs->rx.fmbm_rcfg;
		bmi_status_reg = &port->bmi_regs->rx.fmbm_rst;
		rx_port = true;
		break;
	case FMAN_PORT_TYPE_TX:
		bmi_cfg_reg = &port->bmi_regs->tx.fmbm_tcfg;
		bmi_status_reg = &port->bmi_regs->tx.fmbm_tst;
		rx_port = false;
		break;
	default:
		return -EINVAL;
	}

	/* Disable QMI */
	if (!rx_port) {
		tmp = ioread32be(&port->qmi_regs->fmqm_pnc) & ~QMI_PORT_CFG_EN;
		iowrite32be(tmp, &port->qmi_regs->fmqm_pnc);

		/* Wait for QMI to finish FD handling */
		count = 100;
		do {
			udelay(10);
			tmp = ioread32be(&port->qmi_regs->fmqm_pns);
		} while ((tmp & QMI_PORT_STATUS_DEQ_FD_BSY) && --count);

		if (count == 0) {
			/* Timeout */
			failure = true;
		}
	}

	/* Disable BMI */
	tmp = ioread32be(bmi_cfg_reg) & ~BMI_PORT_CFG_EN;
	iowrite32be(tmp, bmi_cfg_reg);

	/* Wait for graceful stop end */
	count = 500;
	do {
		udelay(10);
		tmp = ioread32be(bmi_status_reg);
	} while ((tmp & BMI_PORT_STATUS_BSY) && --count);

	if (count == 0) {
		/* Timeout */
		failure = true;
	}

	if (failure)
		pr_debug("FMan Port[%d]: BMI or QMI is Busy. Port forced down\n",
			 port->port_id);

	return 0;
}
EXPORT_SYMBOL(fman_port_disable);

int fman_port_enable(struct fman_port *port)
{
	u32 __iomem *bmi_cfg_reg, tmp;
	bool rx_port;

	if (!is_init_done(port->cfg))
		return -EINVAL;

	switch (port->port_type) {
	case FMAN_PORT_TYPE_RX:
		bmi_cfg_reg = &port->bmi_regs->rx.fmbm_rcfg;
		rx_port = true;
		break;
	case FMAN_PORT_TYPE_TX:
		bmi_cfg_reg = &port->bmi_regs->tx.fmbm_tcfg;
		rx_port = false;
		break;
	default:
		return -EINVAL;
	}

	/* Enable QMI */
	if (!rx_port) {
		tmp = ioread32be(&port->qmi_regs->fmqm_pnc) | QMI_PORT_CFG_EN;
		iowrite32be(tmp, &port->qmi_regs->fmqm_pnc);
	}

	/* Enable BMI */
	tmp = ioread32be(bmi_cfg_reg) | BMI_PORT_CFG_EN;
	iowrite32be(tmp, bmi_cfg_reg);

	return 0;
}
EXPORT_SYMBOL(fman_port_enable);

struct fman_port *fman_port_bind(struct device *dev)
{
	return (struct fman_port *)(dev_get_drvdata(get_device(dev)));
}
EXPORT_SYMBOL(fman_port_bind);

u32 fman_port_get_qman_channel_id(struct fman_port *port)
{
	return port->dts_params.qman_channel_id;
}
EXPORT_SYMBOL(fman_port_get_qman_channel_id);

#ifndef __rtems__
static int fman_port_probe(struct platform_device *of_dev)
#else /* __rtems__ */
static int fman_port_probe(struct platform_device *of_dev, struct fman *fman)
#endif /* __rtems__ */
{
	struct fman_port *port;
#ifndef __rtems__
	struct fman *fman;
	struct device_node *fm_node, *port_node;
#else /* __rtems__ */
	struct device_node *port_node;
#endif /* __rtems__ */
	struct resource res;
#ifndef __rtems__
	struct resource *dev_res;
#endif /* __rtems__ */
	const u32 *u32_prop;
	int err = 0, lenp;
	enum fman_port_type port_type;
	u16 port_speed;
	u8 port_id;

	port = kzalloc(sizeof(*port), GFP_KERNEL);
	if (!port)
		return -ENOMEM;

	port_node = of_node_get(of_dev->dev.of_node);

	/* Get the FM node */
#ifndef __rtems__
	fm_node = of_get_parent(port_node);
	if (!fm_node) {
		pr_err("of_get_parent() failed\n");
		err = -ENODEV;
		goto return_err;
	}

	fman = dev_get_drvdata(&of_find_device_by_node(fm_node)->dev);
	of_node_put(fm_node);
	if (!fman) {
		err = -EINVAL;
		goto return_err;
	}
#endif /* __rtems__ */

	u32_prop = (const u32 *)of_get_property(port_node, "cell-index", &lenp);
	if (!u32_prop) {
		pr_err("of_get_property(%s, cell-index) failed\n",
		       port_node->full_name);
		err = -EINVAL;
		goto return_err;
	}
	if (WARN_ON(lenp != sizeof(u32))) {
		err = -EINVAL;
		goto return_err;
	}
	port_id = (u8)*u32_prop;

	port->dts_params.id = port_id;

	if (of_device_is_compatible(port_node, "fsl,fman-v3-port-tx")) {
		port_type = FMAN_PORT_TYPE_TX;
		port_speed = 1000;
		u32_prop = (const u32 *)of_get_property(port_node,
							"fsl,fman-10g-port",
							&lenp);
		if (u32_prop)
			port_speed = 10000;

	} else if (of_device_is_compatible(port_node, "fsl,fman-v2-port-tx")) {
		if (port_id >= TX_10G_PORT_BASE)
			port_speed = 10000;
		else
			port_speed = 1000;
		port_type = FMAN_PORT_TYPE_TX;

	} else if (of_device_is_compatible(port_node, "fsl,fman-v3-port-rx")) {
		port_type = FMAN_PORT_TYPE_RX;
		port_speed = 1000;
		u32_prop = (const u32 *)of_get_property(port_node,
						  "fsl,fman-10g-port", &lenp);
		if (u32_prop)
			port_speed = 10000;

	} else if (of_device_is_compatible(port_node, "fsl,fman-v2-port-rx")) {
		if (port_id >= RX_10G_PORT_BASE)
			port_speed = 10000;
		else
			port_speed = 1000;
		port_type = FMAN_PORT_TYPE_RX;

	}  else {
		pr_err("Illegal port type\n");
		err = -EINVAL;
		goto return_err;
	}

	port->dts_params.type = port_type;
	port->dts_params.speed = port_speed;

	if (port_type == FMAN_PORT_TYPE_TX) {
		u32 qman_channel_id;

		qman_channel_id = fman_get_qman_channel_id(fman, port_id);
		if (qman_channel_id == 0) {
			pr_err("incorrect qman-channel-id\n");
			err = -EINVAL;
			goto return_err;
		}
		port->dts_params.qman_channel_id = qman_channel_id;
	}

	err = of_address_to_resource(port_node, 0, &res);
	if (err < 0) {
		pr_err("of_address_to_resource() failed\n");
		err = -ENOMEM;
		goto return_err;
	}

	port->dts_params.fman = fman;

	of_node_put(port_node);

#ifndef __rtems__
	dev_res = __devm_request_region(fman_get_device(fman), &res,
					res.start, (res.end + 1 - res.start),
					"fman-port");
	if (!dev_res) {
		pr_err("__devm_request_region() failed\n");
		err = -EINVAL;
		goto free_port;
	}
#endif /* __rtems__ */

	port->dts_params.base_addr = devm_ioremap(fman_get_device(fman),
						  res.start,
						  (res.end + 1 - res.start));
	if (port->dts_params.base_addr == 0)
		pr_err("devm_ioremap() failed\n");

	dev_set_drvdata(&of_dev->dev, port);

	return 0;

return_err:
	of_node_put(port_node);
#ifndef __rtems__
free_port:
#endif /* __rtems__ */
	kfree(port);
	return err;
}

#ifndef __rtems__
static const struct of_device_id fman_port_match[] = {
	{.compatible = "fsl,fman-v3-port-rx"},
	{.compatible = "fsl,fman-v2-port-rx"},
	{.compatible = "fsl,fman-v3-port-tx"},
	{.compatible = "fsl,fman-v2-port-tx"},
	{}
};

MODULE_DEVICE_TABLE(of, fman_port_match);

static struct platform_driver fman_port_driver = {
	.driver = {
		   .name = "fsl-fman-port",
		   .of_match_table = fman_port_match,
		   },
	.probe = fman_port_probe,
};

builtin_platform_driver(fman_port_driver);

#else /* __rtems__ */
#include <sys/cdefs.h>
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/bus.h>
#include <sys/kernel.h>

static int
fman_port_dev_probe(device_t dev)
{
	struct fman_ivars *ivars = device_get_ivars(dev);
	int err;

	err = fman_port_probe(&ivars->of_dev, ivars->fman);
	if (err == 0) {
		device_set_desc(dev, "FMan Port");
		return (BUS_PROBE_DEFAULT);
	} else {
		return (ENXIO);
	}
}

static device_method_t fman_port_methods[] = {
	/* Device interface */
	DEVMETHOD(device_probe, fman_port_dev_probe),
	DEVMETHOD(device_attach, bus_generic_attach),
	DEVMETHOD(device_detach, bus_generic_detach),
	DEVMETHOD(device_suspend, bus_generic_suspend),
	DEVMETHOD(device_resume, bus_generic_resume),
	DEVMETHOD(device_shutdown, bus_generic_shutdown),

	DEVMETHOD_END
};

driver_t fman_port_driver = {
	.name = "fman_port",
	.methods = fman_port_methods
};

static devclass_t fman_port_devclass;

DRIVER_MODULE(fman_port, fman_mac, fman_port_driver, fman_port_devclass, 0, 0);
#endif /* __rtems__ */
