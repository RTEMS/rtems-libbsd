/*-
 * SPDX-License-Identifier: BSD-2-Clause-FreeBSD
 *
 * Copyright (c) 2021 Kinsey Moore
 * All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $FreeBSD$
 */

/*
 * Defines for Zynq Ultrascale+ MPSoC SLCR registers.
 *
 * Reference: Zynq Ultrascale+ MPSoC Technical Reference Manual.
 * (v2.2) December 4, 2020.  Xilinx doc UG1085.
 *
 * Additional Reference: Zynq Ultrascale+ MPSoC Register Reference.
 * (v1.7) February 8, 2019.  Xilinx doc UG1087.
 */

#ifndef _ZYNQMP_SLCR_H_
#define _ZYNQMP_SLCR_H_

#define ZYNQMP_SLCR_CRF_OFFSET		0x0d1a0000
#define ZYNQMP_SLCR_CRL_OFFSET		0x0f5e0000

/* PLL controls. */
#define ZYNQMP_SLCR_IO_PLL_CTRL		(ZYNQMP_SLCR_CRL_OFFSET + 0x20)
#define ZYNQMP_SLCR_R_PLL_CTRL		(ZYNQMP_SLCR_CRL_OFFSET + 0x30)
#define ZYNQMP_SLCR_D_PLL_CTRL		(ZYNQMP_SLCR_CRF_OFFSET + 0x2c)
#define   ZYNQMP_SLCR_PLL_CTRL_RESET		(1<<0)
#define   ZYNQMP_SLCR_PLL_CTRL_BYPASS		(1<<3)
#define   ZYNQMP_SLCR_PLL_CTRL_FBDIV_SHIFT	8
#define   ZYNQMP_SLCR_PLL_CTRL_FBDIV_MASK	(0x7f<<8)
#define   ZYNQMP_SLCR_PLL_CTRL_FBDIV_MAX	0x7f
#define   ZYNQMP_SLCR_PLL_CTRL_DIV2		(1<<16)
#define   ZYNQMP_SLCR_PLL_CTRL_PRE_SRC_SHIFT	20
#define   ZYNQMP_SLCR_PLL_CTRL_PRE_SRC_MASK	(0x7<<20)
#define   ZYNQMP_SLCR_PLL_CTRL_POST_SRC_SHIFT	24
#define   ZYNQMP_SLCR_PLL_CTRL_POST_SRC_MASK	(0x7<<24)
#define     ZYNQMP_SLCR_PLL_CTRL_SRC_PS		0x0
#define     ZYNQMP_SLCR_PLL_CTRL_SRC_VIDEO	0x4
#define     ZYNQMP_SLCR_PLL_CTRL_SRC_ALT	0x5
#define     ZYNQMP_SLCR_PLL_CTRL_SRC_AUX	0x6
#define     ZYNQMP_SLCR_PLL_CTRL_SRC_GT		0x7

/* Clock controls. */
#define ZYNQMP_SLCR_GEM0_CLK_CTRL		(ZYNQMP_SLCR_CRL_OFFSET + 0x50)
#define ZYNQMP_SLCR_GEM1_CLK_CTRL		(ZYNQMP_SLCR_CRL_OFFSET + 0x54)
#define ZYNQMP_SLCR_GEM2_CLK_CTRL		(ZYNQMP_SLCR_CRL_OFFSET + 0x58)
#define ZYNQMP_SLCR_GEM3_CLK_CTRL		(ZYNQMP_SLCR_CRL_OFFSET + 0x5c)
#define   ZYNQMP_SLCR_GEM_CLK_CTRL_RX_CLKACT		(1<<26)
#define   ZYNQMP_SLCR_GEM_CLK_CTRL_CLKACT		(1<<25)
#define   ZYNQMP_SLCR_GEM_CLK_CTRL_DIVISOR1_MASK	(0x3f<<16)
#define   ZYNQMP_SLCR_GEM_CLK_CTRL_DIVISOR1_SHIFT	16
#define   ZYNQMP_SLCR_GEM_CLK_CTRL_DIVISOR1_MAX		0x3f
#define   ZYNQMP_SLCR_GEM_CLK_CTRL_DIVISOR0_MASK	(0x3f<<8)
#define   ZYNQMP_SLCR_GEM_CLK_CTRL_DIVISOR0_SHIFT	8
#define   ZYNQMP_SLCR_GEM_CLK_CTRL_DIVISOR0_MAX		0x3f
#define   ZYNQMP_SLCR_GEM_CLK_CTRL_SRCSEL_MASK		(7<<0)
#define   ZYNQMP_SLCR_GEM_CLK_CTRL_SRCSEL_IO_PLL	(0<<0)
#define   ZYNQMP_SLCR_GEM_CLK_CTRL_SRCSEL_R_PLL		(2<<0)
#define   ZYNQMP_SLCR_GEM_CLK_CTRL_SRCSEL_D_PLL		(3<<0)

#define ZYNQMP_DEFAULT_PS_CLK_FREQUENCY 33333333

#ifdef _KERNEL
extern int cgem_set_ref_clk(int unit, int frequency);
#endif
#endif /* _ZYNQMP_SLCR_H_ */
