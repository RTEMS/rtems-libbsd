/*
 * Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
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

#ifndef DW_MMC_DW_MMCREG_H
#define DW_MMC_DW_MMCREG_H

#define DW_MMC_CTRL		0x000
#define DW_MMC_PWREN		0x004
#define DW_MMC_CLKDIV		0x008
#define DW_MMC_CLKSRC		0x00c
#define DW_MMC_CLKENA		0x010
#define DW_MMC_TMOUT		0x014
#define DW_MMC_CTYPE		0x018
#define DW_MMC_BLKSIZ		0x01c
#define DW_MMC_BYTCNT		0x020
#define DW_MMC_INTMASK		0x024
#define DW_MMC_CMDARG		0x028
#define DW_MMC_CMD		0x02c
#define DW_MMC_RESP0		0x030
#define DW_MMC_RESP1		0x034
#define DW_MMC_RESP2		0x038
#define DW_MMC_RESP3		0x03c
#define DW_MMC_MINTSTS		0x040
#define DW_MMC_RINTSTS		0x044
#define DW_MMC_STATUS		0x048
#define DW_MMC_FIFOTH		0x04c
#define DW_MMC_CDETECT		0x050
#define DW_MMC_WRTPRT		0x054
#define DW_MMC_GPIO		0x058
#define DW_MMC_TCBCNT		0x05c
#define DW_MMC_TBBCNT		0x060
#define DW_MMC_DEBNCE		0x064
#define DW_MMC_USRID		0x068
#define DW_MMC_VERID		0x06c
#define DW_MMC_HCON		0x070
#define DW_MMC_UHS_REG		0x074
#define DW_MMC_RST_N		0x078
#define DW_MMC_BMOD		0x080
#define DW_MMC_PLDMND		0x084
#define DW_MMC_DBADDR		0x088
#define DW_MMC_IDSTS		0x08c
#define DW_MMC_IDINTEN		0x090
#define DW_MMC_DSCADDR		0x094
#define DW_MMC_BUFADDR		0x098
#define DW_MMC_DATA		0x200

/* Control Register */
#define DW_MMC_CTRL_DMA_ENABLE		(1u << 25)
#define DW_MMC_CTRL_CEATA_INT_EN	(1u << 11)
#define DW_MMC_CTRL_SEND_AS_CCSD	(1u << 10)
#define DW_MMC_CTRL_SEND_CCSD		(1u << 9)
#define DW_MMC_CTRL_ABRT_READ_DATA	(1u << 8)
#define DW_MMC_CTRL_SEND_IRQ_RESP	(1u << 7)
#define DW_MMC_CTRL_READ_WAIT		(1u << 6)
#define DW_MMC_CTRL_INT_ENABLE		(1u << 4)
#define DW_MMC_CTRL_DMA_RESET		(1u << 2)
#define DW_MMC_CTRL_FIFO_RESET		(1u << 1)
#define DW_MMC_CTRL_RESET		(1u << 0)

/* Power Enable Register */
#define DW_MMC_PWREN_ENABLE		(1u << 0)

/* Clock Enable Register */
#define DW_MMC_CLKEN_LOW_PWR		(1u << 16)
#define DW_MMC_CLKEN_ENABLE		(1u << 0)

/* Timeout Register */
#define DW_MMC_TMOUT_DATA(x)		((x) << 8)
#define DW_MMC_TMOUT_DATA_MSK		0xffffff00
#define DW_MMC_TMOUT_RESP(x)		((x) & 0xFF)
#define DW_MMC_TMOUT_RESP_MSK		0xff

/* Card Type Register */
#define DW_MMC_CTYPE_8BIT		(1u << 16)
#define DW_MMC_CTYPE_4BIT		(1u << 0)
#define DW_MMC_CTYPE_1BIT		0

/* Interrupt Status and Interrupt Mask Register */
#define DW_MMC_INT_SDIO(x)		(1u << (16 + (x)))
#define DW_MMC_INT_EBE			(1u << 15)
#define DW_MMC_INT_ACD			(1u << 14)
#define DW_MMC_INT_SBE			(1u << 13)
#define DW_MMC_INT_HLE			(1u << 12)
#define DW_MMC_INT_FRUN			(1u << 11)
#define DW_MMC_INT_HTO			(1u << 10)
#define DW_MMC_INT_DRTO			(1u << 9)
#define DW_MMC_INT_RTO			(1u << 8)
#define DW_MMC_INT_DCRC			(1u << 7)
#define DW_MMC_INT_RCRC			(1u << 6)
#define DW_MMC_INT_RXDR			(1u << 5)
#define DW_MMC_INT_TXDR			(1u << 4)
#define DW_MMC_INT_DTO			(1u << 3)
#define DW_MMC_INT_CMD_DONE		(1u << 2)
#define DW_MMC_INT_RE			(1u << 1)
#define DW_MMC_INT_CD			(1u << 0)
#define DW_MMC_INT_ERROR  (DW_MMC_INT_RE | DW_MMC_INT_RCRC | DW_MMC_INT_DCRC \
    | DW_MMC_INT_DRTO | DW_MMC_INT_RTO | DW_MMC_INT_EBE)

/* Command Register */
#define DW_MMC_CMD_START		(1u << 31)
#define DW_MMC_CMD_USE_HOLD_REG		(1u << 29)
#define DW_MMC_CMD_VOLT_SWITCH		(1u << 28)
#define DW_MMC_CMD_BOOT_MODE		(1u << 27)
#define DW_MMC_CMD_DISABLE_BOOT		(1u << 26)
#define DW_MMC_CMD_EXP_BOOT_ACK		(1u << 25)
#define DW_MMC_CMD_ENABLE_BOOT		(1u << 24)
#define DW_MMC_CMD_CCS_EXP		(1u << 23)
#define DW_MMC_CMD_CEATA_RD		(1u << 22)
#define DW_MMC_CMD_UPDATE_CLK		(1u << 21)
#define DW_MMC_CMD_SEND_INIT		(1u << 15)
#define DW_MMC_CMD_STOP_ABRT		(1u << 14)
#define DW_MMC_CMD_PRV_DATA_WAIT	(1u << 13)
#define DW_MMC_CMD_SEND_STOP		(1u << 12)
#define DW_MMC_CMD_STREAM_MODE		(1u << 11)
#define DW_MMC_CMD_DATA_WR		(1u << 10)
#define DW_MMC_CMD_DATA_EXP		(1u << 9)
#define DW_MMC_CMD_RESP_CRC		(1u << 8)
#define DW_MMC_CMD_RESP_LONG		(1u << 7)
#define DW_MMC_CMD_RESP_EXP		(1u << 6)
#define DW_MMC_CMD_INDEX(x)		((x) & 0x1f)

/* Status Register */
#define DW_MMC_STATUS_GET_FIFO_CNT(x)	(((x) >> 17) & 0x1fff)
#define DW_MMC_STATUS_GET_RESP_IDX(x)	(((x) >> 11) & 0x3f)
#define DW_MMC_STATUS_DS_MC_BUSY	(1u << 10)
#define DW_MMC_STATUS_CARD_DATA_BUSY	(1u << 9)
#define DW_MMC_STATUS_CARD_PRESENT	(1u << 8)
#define DW_MMC_STATUS_GET_FSM_STATE(x)	(((x) >> 4) & 0xf)
#define DW_MMC_STATUS_FIFO_FULL		(1u << 3)
#define DW_MMC_STATUS_FIFO_EMPTY	(1u << 2)
#define DW_MMC_STATUS_FIFO_TX_WM	(1u << 1)
#define DW_MMC_STATUS_FIFO_RX_WM	(1u << 0)

/* DMA and FIFO Control Register */
#define DW_MMC_FIFOTH_BSZ(x)		((x) << 28)
#define DW_MMC_FIFOTH_BSZ_MSK		DW_MMC_FIFOTH_BSZ(0x7)
#define DW_MMC_FIFOTH_RX_WMARK(x)	((x) << 16)
#define DW_MMC_FIFOTH_RX_WMARK_MSK	DW_MMC_FIFOTH_RX_WMARK(0xfff)
#define DW_MMC_FIFOTH_TX_WMARK(x)	((x) << 0)
#define DW_MMC_FIFOTH_TX_WMARK_MSK	DW_MMC_FIFOTH_TX_WMARK(0xfff)

/* Card Detect Register */
#define DW_MMC_CDETECT_NOT_DETECTED	(1u << 0)

/* Write Protect Register */
#define DW_MMC_WRTPRT_ENABLED		(1u << 0)

/* Hardware Reset Register */
#define DW_MMC_RST_N_ACTIVE_MODE	(1u << 0)

/* Bus Mode Register */
#define DW_MMC_BMOD_DE			(1u << 7)
#define DW_MMC_BMOD_FB			(1u << 1)
#define DW_MMC_BMOD_SWR			(1u << 0)

/* Internal DMAC Status and Interrupt DMAC Interrupt Enable Register */
#define DW_MMC_IDMAC_INT_GET_FSM(x)	(((x) >> 13) & 0xf)
#define DW_MMC_IDMAC_INT_GET_EB(x)	(((x) >> 10) & 0x7)
#define DW_MMC_IDMAC_INT_AIS		(1u << 9)
#define DW_MMC_IDMAC_INT_NIS		(1u << 8)
#define DW_MMC_IDMAC_INT_CES		(1u << 5)
#define DW_MMC_IDMAC_INT_DU		(1u << 4)
#define DW_MMC_IDMAC_INT_FBE		(1u << 2)
#define DW_MMC_IDMAC_INT_RI		(1u << 1)
#define DW_MMC_IDMAC_INT_TI		(1u << 0)

/* Internal DMA descriptor */

struct dw_mmc_des {
	uint32_t des0;
	uint32_t des1;
	uint32_t des2;
	uint32_t des3;
};

#define DW_MMC_DES0_OWN			(1u << 31)
#define DW_MMC_DES0_CES			(1u << 30)
#define DW_MMC_DES0_ER			(1u << 5)
#define DW_MMC_DES0_CH			(1u << 4)
#define DW_MMC_DES0_FS			(1u << 3)
#define DW_MMC_DES0_LD			(1u << 2)
#define DW_MMC_DES0_DIC			(1u << 1)

#define DW_MMC_DES1_BS2(x)		((x) << 13)
#define DW_MMC_DES1_BS1(x)		((x) << 0)
#define DW_MMC_DES1_MAX_BS		4096

/* FIFO dimensions */
#define DW_MMC_FIFO_DEPTH 1024
#define DW_MMC_FIFO_WIDTH 4

#endif /* DW_MMC_DW_MMCREG_H */
