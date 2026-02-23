/*
 * Copyright (c) 2026 Aaron Nyholm <aaron.nyholm@unfoldedeffective.com>.
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
 */

#ifndef	_DEV_XILINX_XLNX_NWL_PCIB_H_
#define	_DEV_XILINX_XLNX_NWL_PCIB_H_

#define XLNX_NWLP_MSI_MAX   64
#define XLNX_NWLP_MSI_ADDR  0xFE440000

/* Core config */
#define XLNX_NWLP_CCFG_PCIE_RX0         0x00000000
#define XLNX_NWLP_CCFG_PCIE_RX1         0x00000004
#define XLNX_NWLP_CCFG_AXI_MASTER       0x00000008
#define XLNX_NWLP_CCFG_PCIE_TX          0x0000000C
#define XLNX_NWLP_CCFG_INTR             0x00000010
#define XLNX_NWLP_CCFG_RAM_D0           0x00000014
#define XLNX_NWLP_CCFG_RAM_D1           0x00000018
#define XLNX_NWLP_CCFG_PCIE_RLX_ODR     0x0000001C
#define XLNX_NWLP_CCFG_PCIE_RX_MSG_FILT 0x00000020
#define XLNX_NWLP_CCFG_RQ_REQ_ODR       0x00000024
#define XLNX_NWLP_CCFG_PCIE_CREDIT      0x00000028
#define XLNX_NWLP_CCFG_AXI_M_W_TC       0x0000002C
#define XLNX_NWLP_CCFG_AXI_M_R_TC       0x00000030
#define XLNX_NWLP_CCFG_CRS_RPL_TC       0x00000034

#define XLNX_NWLP_CCFG_PCIE_RX0_DMA_BAR_MASK  0x00000003
#define XLNX_NWLP_CCFG_PCIE_RX0_DMA_BAR_SHIFT 0x00000000
#define XLNX_NWLP_CCFG_PCIE_RX0_DMA_BAR_NONE  0x00000007
#define XLNX_NWLP_CCFG_PCIE_RX0_PCIE_NO_DMA   0x00020000
#define XLNX_NWLP_CCFG_PCIE_RX0_PCIE_NO_BREG  0x00010000

/* Egress bridge register translation */
#define XLNX_NWLP_E_BREG_CAPS           0x00000200
#define XLNX_NWLP_E_BREG_STATUS         0x00000204
#define XLNX_NWLP_E_BREG_CONTROL        0x00000208
#define XLNX_NWLP_E_BREG_BASE_LO        0x00000210
#define XLNX_NWLP_E_BREG_BASE_HI        0x00000214

#define XLNX_NWLP_E_BREG_CTRL_ENA       0x00000001
#define XLNX_NWLP_E_BREG_CTRL_ENA_FORCE 0x00000002

/* Egress ECAM register translation */
#define XLNX_NWLP_E_ECAM_CAPS           0x00000220
#define XLNX_NWLP_E_ECAM_STATUS         0x00000224
#define XLNX_NWLP_E_ECAM_CONTROL        0x00000228
#define XLNX_NWLP_E_ECAM_BASE_LO        0x00000230
#define XLNX_NWLP_E_ECAM_BASE_HI        0x00000234

#define XLNX_NWLP_E_ECAM_CAPS_PRES            0x00000001
#define XLNX_NWLP_E_ECAM_CAPS_SIZE_OFF_MASK   0x001F0000
#define XLNX_NWLP_E_ECAM_CAPS_SIZE_OFF_SHIFT  0x00000010
#define XLNX_NWLP_E_ECAM_CAPS_SIZE_MAX_MASK   0xFF000000
#define XLNX_NWLP_E_ECAM_CAPS_SIZE_MAX_SHIFT  0x00000018

#define XLNX_NWLP_E_ECAM_CTRL_ENA         0x00000001
#define XLNX_NWLP_E_ECAM_CTRL_SEC_ENA     0x00000004
#define XLNX_NWLP_E_ECAM_CTRL_SIZE_MASK   0x001F0000
#define XLNX_NWLP_E_ECAM_CTRL_SIZE_SHIFT  0x00000014

/* Egress MSI-X table translation */
#define XLNX_NWLP_E_MSXT_CAPS           0x00000240
#define XLNX_NWLP_E_MSXT_STATUS         0x00000244
#define XLNX_NWLP_E_MSXT_CONTROL        0x00000248
#define XLNX_NWLP_E_MSXT_BASE_LO        0x00000250
#define XLNX_NWLP_E_MSXT_BASE_HI        0x00000254

/* Egress MSI-X PBA translation */
#define XLNX_NWLP_E_MSXP_CAPS           0x00000260
#define XLNX_NWLP_E_MSXP_STATUS         0x00000264
#define XLNX_NWLP_E_MSXP_CONTROL        0x00000268
#define XLNX_NWLP_E_MSXP_BASE_LO        0x00000270
#define XLNX_NWLP_E_MSXP_BASE_HI        0x00000274

/* Egress DMA register translation */
#define XLNX_NWLP_E_DREG_CAPS           0x00000280
#define XLNX_NWLP_E_DREG_STATUS         0x00000284
#define XLNX_NWLP_E_DREG_CONTROL        0x00000288
#define XLNX_NWLP_E_DREG_BASE_LO        0x00000290
#define XLNX_NWLP_E_DREG_BASE_HI        0x00000294

/* Egress subtractive decode translation */
#define XLNX_NWLP_E_ESUB_CAPS           0x000002E0
#define XLNX_NWLP_E_ESUB_STATUS         0x000002E4
#define XLNX_NWLP_E_ESUB_CONTROL        0x000002E8

/* Ingress MSI interrupt translation */
#define XLNX_NWLP_I_MSII_CAPS           0x00000300
#define XLNX_NWLP_I_MSII_CONTROL        0x00000308
#define XLNX_NWLP_I_MSII_BASE_LO        0x00000310
#define XLNX_NWLP_I_MSII_BASE_HI        0x00000314

#define XLNX_NWLP_I_MSII_CTRL_ENA       0x00008000
#define XLNX_NWLP_I_MSII_CTRL_STS_ENA   0x00000001

/* Ingress MSI-X interrupt translation */
#define XLNX_NWLP_I_MSIX_CAPS           0x00000320
#define XLNX_NWLP_I_MSIX_CONTROL        0x00000328
#define XLNX_NWLP_I_MSIX_BASE_LO        0x00000330
#define XLNX_NWLP_I_MSIX_BASE_HI        0x00000334

/* Ingress subtractive decode translation */
#define XLNX_NWLP_I_ISUB_CAPS           0x000003E0
#define XLNX_NWLP_I_ISUB_STATUS         0x000003E4
#define XLNX_NWLP_I_ISUB_CONTROL        0x000003E8

#define XLNX_NWLP_I_ISUB_CTRL_ENA       0x00000001

/* Message and interrupt controller */
#define XLNX_NWLP_MSGF_MISC_STATUS      0x00000400
#define XLNX_NWLP_MSGF_MISC_MASK        0x00000404
#define XLNX_NWLP_MSGF_MISC_SLAVE_ID    0x00000408
#define XLNX_NWLP_MSGF_MISC_MASTER_ID   0x0000040C
#define XLNX_NWLP_MSGF_MISC_INGRESS_ID  0x00000410
#define XLNX_NWLP_MSGF_MISC_EGRESS_ID   0x00000414

#define XLNX_NWLP_MISC_RX_MSG_AVAIL     0x00000001
#define XLNX_NWLP_MISC_RX_MSG_OVER      0x00000002
#define XLNX_NWLP_MISC_UC_WRITE         0x00000008
#define XLNX_NWLP_MISC_SLAVE            0x00000010
#define XLNX_NWLP_MISC_MASTER           0x00000020
#define XLNX_NWLP_MISC_I_ADDR_TRANS     0x00000040
#define XLNX_NWLP_MISC_E_ADDR_TRANS     0x00000040
#define XLNX_NWLP_MISC_R_FATAL_AER      0x00010000
#define XLNX_NWLP_MISC_R_NON_FATAL_AER  0x00020000
#define XLNX_NWLP_MISC_R_CORRECT_AER    0x00040000
#define XLNX_NWLP_MISC_TS12_HR          0x00080000
#define XLNX_NWLP_MISC_UR_DETECT        0x00100000
#define XLNX_NWLP_MISC_D_CORRECT        0x00200000
#define XLNX_NWLP_MISC_D_NON_FATAL      0x00400000
#define XLNX_NWLP_MISC_D_FATAL          0x00800000
#define XLNX_NWLP_MISC_LINK_DOWN        0x01000000
#define XLNX_NWLP_MISC_LINK_AUTO_BAND   0x02000000
#define XLNX_NWLP_MISC_LINK_BAND        0x04000000

#define XLNX_NWLP_MSGF_LEG_STATUS       0x00000420
#define XLNX_NWLP_MSGF_LEG_MASK         0x00000424

#define XLNX_NWLP_MSGF_MSI_STATUS_LO    0x00000440
#define XLNX_NWLP_MSGF_MSI_STATUS_HI    0x00000444
#define XLNX_NWLP_MSGF_MSI_MASK_LO      0x00000448
#define XLNX_NWLP_MSGF_MSI_MASK_HI      0x0000044C

#define XLNX_NWLP_MSGF_DMA_STATUS       0x00000460
#define XLNX_NWLP_MSGF_DMA_MASK         0x00000464

#define XLNX_NWLP_MSGF_RX_FIFO_LEVEL    0x00000480
#define XLNX_NWLP_MSGF_RX_FIFO_POP      0x00000484
#define XLNX_NWLP_MSGF_RX_FIFO_TYPE     0x00000488
#define XLNX_NWLP_MSGF_RX_FIFO_MSG      0x0000048C
#define XLNX_NWLP_MSGF_RX_FIFO_ADDR_LO  0x00000490
#define XLNX_NWLP_MSGF_RX_FIFO_ADDR_HI  0x00000494
#define XLNX_NWLP_MSGF_RX_FIFO_DATA     0x00000498

#define XLNX_NWLP_MSGF_RX_FIFO_LEVEL_MASK 0x000000FF
#define XLNX_NWLP_MSGF_RX_FIFO_POP_MASK   0x00000001

#define XLNX_NWLP_TX_PCIE_MSG_EXEC      0x00000620
#define XLNX_NWLP_TX_PCIE_MSG_CONTROL   0x00000624
#define XLNX_NWLP_TX_PCIE_MSG_SPEC_LO   0x00000628
#define XLNX_NWLP_TX_PCIE_MSG_SPEC_HI   0x0000062C
#define XLNX_NWLP_TX_PCIE_MSG_DATA      0x00000630

/* PCIe Attributes */
#define XLNX_NWLP_ATTR_1                0x00000004
#define XLNX_NWLP_ATTR_2                0x00000008
#define XLNX_NWLP_ATTR_3                0x0000000c
#define XLNX_NWLP_ATTR_4                0x00000010
#define XLNX_NWLP_ATTR_5                0x00000014
#define XLNX_NWLP_ATTR_6                0x00000018
#define XLNX_NWLP_ATTR_7                0x0000001c
#define XLNX_NWLP_ATTR_8                0x00000020
#define XLNX_NWLP_ATTR_9                0x00000024
#define XLNX_NWLP_ATTR_10               0x00000028
#define XLNX_NWLP_ATTR_11               0x0000002c
#define XLNX_NWLP_ATTR_12               0x00000030
#define XLNX_NWLP_ATTR_13               0x00000034
#define XLNX_NWLP_ATTR_14               0x00000038
#define XLNX_NWLP_ATTR_15               0x0000003c
#define XLNX_NWLP_ATTR_16               0x00000040
#define XLNX_NWLP_ATTR_17               0x00000044
#define XLNX_NWLP_ATTR_18               0x00000048
#define XLNX_NWLP_ATTR_19               0x0000004c
#define XLNX_NWLP_ATTR_20               0x00000050
#define XLNX_NWLP_ATTR_21               0x00000054
#define XLNX_NWLP_ATTR_22               0x00000058
#define XLNX_NWLP_ATTR_23               0x0000005c
#define XLNX_NWLP_ATTR_24               0x00000060
#define XLNX_NWLP_ATTR_25               0x00000064
#define XLNX_NWLP_ATTR_26               0x00000068
#define XLNX_NWLP_ATTR_27               0x0000006c
#define XLNX_NWLP_ATTR_28               0x00000070
#define XLNX_NWLP_ATTR_29               0x00000074
#define XLNX_NWLP_ATTR_30               0x00000078
#define XLNX_NWLP_ATTR_31               0x0000007c
#define XLNX_NWLP_ATTR_32               0x00000080
#define XLNX_NWLP_ATTR_33               0x00000084
#define XLNX_NWLP_ATTR_34               0x00000088
#define XLNX_NWLP_ATTR_35               0x0000008c
#define XLNX_NWLP_ATTR_36               0x00000090
#define XLNX_NWLP_ATTR_37               0x00000094
#define XLNX_NWLP_ATTR_38               0x00000098
#define XLNX_NWLP_ATTR_39               0x0000009c
#define XLNX_NWLP_ATTR_40               0x000000a0
#define XLNX_NWLP_ATTR_41               0x000000a4
#define XLNX_NWLP_ATTR_42               0x000000a8
#define XLNX_NWLP_ATTR_43               0x000000ac
#define XLNX_NWLP_ATTR_44               0x000000b0
#define XLNX_NWLP_ATTR_45               0x000000b4
#define XLNX_NWLP_ATTR_46               0x000000b8
#define XLNX_NWLP_ATTR_47               0x000000bc
#define XLNX_NWLP_ATTR_48               0x000000c0
#define XLNX_NWLP_ATTR_49               0x000000c4
#define XLNX_NWLP_ATTR_50               0x000000c8
#define XLNX_NWLP_ATTR_51               0x000000cc
#define XLNX_NWLP_ATTR_52               0x000000d0
#define XLNX_NWLP_ATTR_53               0x000000d4
#define XLNX_NWLP_ATTR_54               0x000000d8
#define XLNX_NWLP_ATTR_55               0x000000dc
#define XLNX_NWLP_ATTR_56               0x000000e0
#define XLNX_NWLP_ATTR_57               0x000000e4
#define XLNX_NWLP_ATTR_58               0x000000e8
#define XLNX_NWLP_ATTR_59               0x000000ec
#define XLNX_NWLP_ATTR_60               0x000000f0
#define XLNX_NWLP_ATTR_61               0x000000f4
#define XLNX_NWLP_ATTR_62               0x000000f8
#define XLNX_NWLP_ATTR_63               0x000000fc
#define XLNX_NWLP_ATTR_64               0x00000100
#define XLNX_NWLP_ATTR_65               0x00000104
#define XLNX_NWLP_ATTR_66               0x00000108
#define XLNX_NWLP_ATTR_67               0x0000010c
#define XLNX_NWLP_ATTR_68               0x00000110
#define XLNX_NWLP_ATTR_69               0x00000114
#define XLNX_NWLP_ATTR_70               0x00000118
#define XLNX_NWLP_ATTR_71               0x0000011c
#define XLNX_NWLP_ATTR_72               0x00000120
#define XLNX_NWLP_ATTR_73               0x00000124
#define XLNX_NWLP_ATTR_74               0x00000128
#define XLNX_NWLP_ATTR_75               0x0000012c
#define XLNX_NWLP_ATTR_76               0x00000130
#define XLNX_NWLP_ATTR_77               0x00000134
#define XLNX_NWLP_ATTR_78               0x00000138
#define XLNX_NWLP_ATTR_79               0x0000013c
#define XLNX_NWLP_ATTR_80               0x00000140
#define XLNX_NWLP_ATTR_81               0x00000144
#define XLNX_NWLP_ATTR_82               0x00000148
#define XLNX_NWLP_ATTR_83               0x0000014c
#define XLNX_NWLP_ATTR_84               0x00000150
#define XLNX_NWLP_ATTR_85               0x00000154
#define XLNX_NWLP_ATTR_86               0x00000158
#define XLNX_NWLP_ATTR_87               0x0000015c
#define XLNX_NWLP_ATTR_88               0x00000160
#define XLNX_NWLP_ATTR_89               0x00000164
#define XLNX_NWLP_ATTR_90               0x00000168
#define XLNX_NWLP_ATTR_91               0x0000016c
#define XLNX_NWLP_ATTR_92               0x00000170
#define XLNX_NWLP_ATTR_93               0x00000174
#define XLNX_NWLP_ATTR_94               0x00000178
#define XLNX_NWLP_ATTR_95               0x0000017c
#define XLNX_NWLP_ATTR_96               0x00000180
#define XLNX_NWLP_ATTR_97               0x00000184
#define XLNX_NWLP_ATTR_98               0x00000188
#define XLNX_NWLP_ATTR_99               0x0000018c
#define XLNX_NWLP_ATTR_100              0x00000190
#define XLNX_NWLP_ATTR_101              0x00000194
#define XLNX_NWLP_ATTR_102              0x00000198
#define XLNX_NWLP_ATTR_103              0x0000019c
#define XLNX_NWLP_ATTR_104              0x000001a0
#define XLNX_NWLP_ATTR_105              0x000001a4
#define XLNX_NWLP_ATTR_106              0x000001a8
#define XLNX_NWLP_ATTR_107              0x000001ac
#define XLNX_NWLP_ATTR_108              0x000001b0
#define XLNX_NWLP_ATTR_109              0x000001b4
#define XLNX_NWLP_ATTR_110              0x000001b8

/* PCIe control */
#define XLNX_NWLP_ID                    0x00000200
#define XLNX_NWLP_SUBSYS_ID             0x00000204
#define XLNX_NWLP_REV_ID                0x00000208
#define XLNX_NWLP_DSN_0                 0x0000020C
#define XLNX_NWLP_DSN_1                 0x00000210
#define XLNX_NWLP_PM_CTRL               0x00000218
#define XLNX_NWLP_PL_LINK_CTRL_STATUS   0x00000228
#define XLNX_NWLP_EP_CTRL               0x00000230
#define XLNX_NWLP_RP_CTRL               0x00000234
#define XLNX_NWLP_PCIE_STATUS           0x00000238
#define XLNX_NWLP_MISC_CTRL             0x00000300
#define XLNX_NWLP_ISR                   0x00000304
#define XLNX_NWLP_IMR                   0x00000308
#define XLNX_NWLP_IER                   0x0000030C
#define XLNX_NWLP_IDR                   0x00000310
#define XLNX_NWLP_CB                    0x0000031C

#define XLNX_NWLP_INTX_ADDR_DECODE      0x00000001
#define XLNX_NWLP_INTX_PCIE_RESET       0x00000002

#define XLNX_NWLP_PCIE_STS_LINK_UP      0x00000001
#define XLNX_NWLP_PCIE_STS_PHY_RDY      0x00000002

#endif /* _DEV_XILINX_XLNX_NWL_PCIB_H_ */
