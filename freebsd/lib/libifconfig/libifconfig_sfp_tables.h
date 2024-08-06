/*-
 * Copyright (c) 2020, Ryan Moeller <freqlabs@FreeBSD.org>
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


#pragma once

#include <stdint.h>

/*
 * SFF-8024 Rev. 4.6 Table 4-1: Indentifier Values
 */

/** Transceiver identifier */
enum sfp_id {
	SFP_ID_UNKNOWN = 0, /**< Unknown or unspecified */
	SFP_ID_GBIC = 1, /**< GBIC */
	SFP_ID_SFF = 2, /**< Module soldered to motherboard (ex: SFF) */
	SFP_ID_SFP = 3, /**< SFP or SFP+ */
	SFP_ID_XBI = 4, /**< 300 pin XBI */
	SFP_ID_XENPAK = 5, /**< Xenpak */
	SFP_ID_XFP = 6, /**< XFP */
	SFP_ID_XFF = 7, /**< XFF */
	SFP_ID_XFPE = 8, /**< XFP-E */
	SFP_ID_XPAK = 9, /**< XPAK */
	SFP_ID_X2 = 10, /**< X2 */
	SFP_ID_DWDM_SFP = 11, /**< DWDM-SFP/SFP+ */
	SFP_ID_QSFP = 12, /**< QSFP */
	SFP_ID_QSFPPLUS = 13, /**< QSFP+ or later */
	SFP_ID_CXP = 14, /**< CXP */
	SFP_ID_HD4X = 15, /**< Shielded Mini Multilane HD 4X */
	SFP_ID_HD8X = 16, /**< Shielded Mini Multilane HD 8X */
	SFP_ID_QSFP28 = 17, /**< QSFP28 or later */
	SFP_ID_CXP2 = 18, /**< CXP2 (aka CXP28) */
	SFP_ID_CDFP = 19, /**< CDFP (Style 1/Style 2) */
	SFP_ID_SMM4 = 20, /**< Shielded Mini Multilane HD 4X fanout */
	SFP_ID_SMM8 = 21, /**< Shielded Mini Multilane HD 8X fanout */
	SFP_ID_CDFP3 = 22, /**< CDFP (Style 3) */
	SFP_ID_MICROQSFP = 23, /**< microQSFP */
	SFP_ID_QSFP_DD = 24, /**< QSFP-DD 8X pluggable transceiver */
	SFP_ID_QSFP8X = 25, /**< QSFP 8X pluggable transceiver */
	SFP_ID_SFP_DD = 26, /**< SFP-DD 2X pluggable transceiver */
	SFP_ID_DSFP = 27, /**< DSFP Dual SFP pluggable transceiver */
	SFP_ID_X4ML = 28, /**< x4 MiniLink/OcuLink */
	SFP_ID_X8ML = 29, /**< x8 MiniLink */
	SFP_ID_QSFP_CMIS = 30, /**< QSFP+ or later w/Common Management Interface Specification */
};

/** Get the symbolic name of a given sfp_id value */
const char *ifconfig_sfp_id_symbol(enum sfp_id);

/** Get a brief description of a given sfp_id value */
const char *ifconfig_sfp_id_description(enum sfp_id);

/** Get a shortened user-friendly display name for a given sfp_id value */
const char *ifconfig_sfp_id_display(enum sfp_id);

/*
 * SFF-8024 Rev. 4.6 Table 4-3: Connector Types
 */

/** Connector type */
enum sfp_conn {
	SFP_CONN_UNKNOWN = 0, /**< Unknown */
	SFP_CONN_SC = 1, /**< SC */
	SFP_CONN_FC_1_COPPER = 2, /**< Fibre Channel Style 1 copper */
	SFP_CONN_FC_2_COPPER = 3, /**< Fibre Channel Style 2 copper */
	SFP_CONN_BNC_TNC = 4, /**< BNC/TNC */
	SFP_CONN_FC_COAX = 5, /**< Fibre Channel coaxial */
	SFP_CONN_FIBER_JACK = 6, /**< Fiber Jack */
	SFP_CONN_LC = 7, /**< LC */
	SFP_CONN_MT_RJ = 8, /**< MT-RJ */
	SFP_CONN_MU = 9, /**< MU */
	SFP_CONN_SG = 10, /**< SG */
	SFP_CONN_OPTICAL_PIGTAIL = 11, /**< Optical pigtail */
	SFP_CONN_MPO_1X12_POPTIC = 12, /**< MPO 1x12 Parallel Optic */
	SFP_CONN_MPO_2X16_POPTIC = 13, /**< MPO 2x16 Parallel Optic */
	SFP_CONN_HSSDC_II = 32, /**< HSSDC II */
	SFP_CONN_COPPER_PIGTAIL = 33, /**< Copper pigtail */
	SFP_CONN_RJ45 = 34, /**< RJ45 */
	SFP_CONN_NONE = 35, /**< No separable connector */
	SFP_CONN_MXC_2X16 = 36, /**< MXC 2x16 */
	SFP_CONN_CS_OPTICAL = 37, /**< CS optical connector */
	SFP_CONN_MINI_CS_OPTICAL = 38, /**< Mini CS optical connector */
	SFP_CONN_MPO_2X12_POPTIC = 39, /**< MPO 2x12 Parallel Optic */
	SFP_CONN_MPO_1X16_POPTIC = 40, /**< MPO 1x16 Parallel Optic */
};

/** Get the symbolic name of a given sfp_conn value */
const char *ifconfig_sfp_conn_symbol(enum sfp_conn);

/** Get a brief description of a given sfp_conn value */
const char *ifconfig_sfp_conn_description(enum sfp_conn);

/*
 * SFF-8472 Rev. 11.4 table 3.5: Transceiver codes
 */

/*
 * 10G Ethernet/IB compliance codes, byte 3
 */

/** 10G Ethernet/IB compliance */
enum sfp_eth_10g {
	SFP_ETH_10G_10G_BASE_ER = 128, /**< 10G Base-ER */
	SFP_ETH_10G_10G_BASE_LRM = 64, /**< 10G Base-LRM */
	SFP_ETH_10G_10G_BASE_LR = 32, /**< 10G Base-LR */
	SFP_ETH_10G_10G_BASE_SR = 16, /**< 10G Base-SR */
	SFP_ETH_10G_1X_SX = 8, /**< 1X SX */
	SFP_ETH_10G_1X_LX = 4, /**< 1X LX */
	SFP_ETH_10G_1X_COPPER_ACTIVE = 2, /**< 1X Copper Active */
	SFP_ETH_10G_1X_COPPER_PASSIVE = 1, /**< 1X Copper Passive */
};

/** Get the symbolic name of a given sfp_eth_10g value */
const char *ifconfig_sfp_eth_10g_symbol(enum sfp_eth_10g);

/** Get a brief description of a given sfp_eth_10g value */
const char *ifconfig_sfp_eth_10g_description(enum sfp_eth_10g);

/*
 * Ethernet compliance codes, byte 6
 */

/** Ethernet compliance */
enum sfp_eth {
	SFP_ETH_BASE_PX = 128, /**< BASE-PX */
	SFP_ETH_BASE_BX10 = 64, /**< BASE-BX10 */
	SFP_ETH_100BASE_FX = 32, /**< 100BASE-FX */
	SFP_ETH_100BASE_LX_LX10 = 16, /**< 100BASE-LX/LX10 */
	SFP_ETH_1000BASE_T = 8, /**< 1000BASE-T */
	SFP_ETH_1000BASE_CX = 4, /**< 1000BASE-CX */
	SFP_ETH_1000BASE_LX = 2, /**< 1000BASE-LX */
	SFP_ETH_1000BASE_SX = 1, /**< 1000BASE-SX */
};

/** Get the symbolic name of a given sfp_eth value */
const char *ifconfig_sfp_eth_symbol(enum sfp_eth);

/** Get a brief description of a given sfp_eth value */
const char *ifconfig_sfp_eth_description(enum sfp_eth);

/*
 * FC link length, byte 7
 */

/** Fibre Channel link length */
enum sfp_fc_len {
	SFP_FC_LEN_VERY_LONG = 128, /**< very long distance */
	SFP_FC_LEN_SHORT = 64, /**< short distance */
	SFP_FC_LEN_INTERMEDIATE = 32, /**< intermediate distance */
	SFP_FC_LEN_LONG = 16, /**< long distance */
	SFP_FC_LEN_MEDIUM = 8, /**< medium distance */
};

/** Get the symbolic name of a given sfp_fc_len value */
const char *ifconfig_sfp_fc_len_symbol(enum sfp_fc_len);

/** Get a brief description of a given sfp_fc_len value */
const char *ifconfig_sfp_fc_len_description(enum sfp_fc_len);

/*
 * Channel/Cable technology, byte 7-8
 */

/** Channel/cable technology */
enum sfp_cab_tech {
	SFP_CAB_TECH_SA = 1024, /**< Shortwave laser (SA) */
	SFP_CAB_TECH_LC = 512, /**< Longwave laser (LC) */
	SFP_CAB_TECH_EL_INTER = 256, /**< Electrical inter-enclosure (EL) */
	SFP_CAB_TECH_EL_INTRA = 128, /**< Electrical intra-enclosure (EL) */
	SFP_CAB_TECH_SN = 64, /**< Shortwave laser (SN) */
	SFP_CAB_TECH_SL = 32, /**< Shortwave laser (SL) */
	SFP_CAB_TECH_LL = 16, /**< Longwave laser (LL) */
	SFP_CAB_TECH_ACTIVE = 8, /**< Active Cable */
	SFP_CAB_TECH_PASSIVE = 4, /**< Passive Cable */
};

/** Get the symbolic name of a given sfp_cab_tech value */
const char *ifconfig_sfp_cab_tech_symbol(enum sfp_cab_tech);

/** Get a brief description of a given sfp_cab_tech value */
const char *ifconfig_sfp_cab_tech_description(enum sfp_cab_tech);

/*
 * FC Transmission media, byte 9
 */

/** Fibre Channel transmission media */
enum sfp_fc_media {
	SFP_FC_MEDIA_TW = 128, /**< Twin Axial Pair (TW) */
	SFP_FC_MEDIA_TP = 64, /**< Twisted Pair (TP) */
	SFP_FC_MEDIA_MI = 32, /**< Miniature Coax (MI) */
	SFP_FC_MEDIA_TV = 16, /**< Video Coax (TV) */
	SFP_FC_MEDIA_M6 = 8, /**< Miltimode 62.5um (M6) */
	SFP_FC_MEDIA_M5 = 4, /**< Multimode 50um (M5) */
	SFP_FC_MEDIA_RESERVED = 2, /**< Reserved */
	SFP_FC_MEDIA_SM = 1, /**< Single Mode (SM) */
};

/** Get the symbolic name of a given sfp_fc_media value */
const char *ifconfig_sfp_fc_media_symbol(enum sfp_fc_media);

/** Get a brief description of a given sfp_fc_media value */
const char *ifconfig_sfp_fc_media_description(enum sfp_fc_media);

/*
 * FC Speed, byte 10
 */

/** Fibre Channel speed */
enum sfp_fc_speed {
	SFP_FC_SPEED_1200 = 128, /**< 1200 MBytes/sec */
	SFP_FC_SPEED_800 = 64, /**< 800 MBytes/sec */
	SFP_FC_SPEED_1600 = 32, /**< 1600 MBytes/sec */
	SFP_FC_SPEED_400 = 16, /**< 400 MBytes/sec */
	SFP_FC_SPEED_3200 = 8, /**< 3200 MBytes/sec */
	SFP_FC_SPEED_200 = 4, /**< 200 MBytes/sec */
	SFP_FC_SPEED_100 = 1, /**< 100 MBytes/sec */
};

/** Get the symbolic name of a given sfp_fc_speed value */
const char *ifconfig_sfp_fc_speed_symbol(enum sfp_fc_speed);

/** Get a brief description of a given sfp_fc_speed value */
const char *ifconfig_sfp_fc_speed_description(enum sfp_fc_speed);

/*
 * SFF-8436 Rev. 4.8 table 33: Specification compliance
 */

/*
 * 10/40G Ethernet compliance codes, byte 128 + 3
 */

/** 10/40G Ethernet compliance */
enum sfp_eth_1040g {
	SFP_ETH_1040G_EXTENDED = 128, /**< Extended */
	SFP_ETH_1040G_10GBASE_LRM = 64, /**< 10GBASE-LRM */
	SFP_ETH_1040G_10GBASE_LR = 32, /**< 10GBASE-LR */
	SFP_ETH_1040G_10GBASE_SR = 16, /**< 10GBASE-SR */
	SFP_ETH_1040G_40GBASE_CR4 = 8, /**< 40GBASE-CR4 */
	SFP_ETH_1040G_40GBASE_SR4 = 4, /**< 40GBASE-SR4 */
	SFP_ETH_1040G_40GBASE_LR4 = 2, /**< 40GBASE-LR4 */
	SFP_ETH_1040G_40G_ACTIVE = 1, /**< 40G Active Cable */
};

/** Get the symbolic name of a given sfp_eth_1040g value */
const char *ifconfig_sfp_eth_1040g_symbol(enum sfp_eth_1040g);

/** Get a brief description of a given sfp_eth_1040g value */
const char *ifconfig_sfp_eth_1040g_description(enum sfp_eth_1040g);

/*
 * SFF-8024 Rev. 4.6 table 4-4: Extended Specification Compliance
 */

/** Extended specification compliance */
enum sfp_eth_ext {
	SFP_ETH_EXT_RESERVED_FF = 255, /**< Reserved */
	SFP_ETH_EXT_128GFC_LW = 85, /**< 128GFC LW */
	SFP_ETH_EXT_128GFC_SW = 84, /**< 128GFC SW */
	SFP_ETH_EXT_128GFC_EA = 83, /**< 128GFC EA */
	SFP_ETH_EXT_64GFC_LW = 82, /**< 64GFC LW */
	SFP_ETH_EXT_64GFC_SW = 81, /**< 64GFC SW */
	SFP_ETH_EXT_64GFC_EA = 80, /**< 64GFC EA */
	SFP_ETH_EXT_RESERVED_4F = 79, /**< Reserved */
	SFP_ETH_EXT_RESERVED_4E = 78, /**< Reserved */
	SFP_ETH_EXT_RESERVED_4D = 77, /**< Reserved */
	SFP_ETH_EXT_RESERVED_4C = 76, /**< Reserved */
	SFP_ETH_EXT_RESERVED_4B = 75, /**< Reserved */
	SFP_ETH_EXT_RESERVED_4A = 74, /**< Reserved */
	SFP_ETH_EXT_RESERVED_49 = 73, /**< Reserved */
	SFP_ETH_EXT_RESERVED_48 = 72, /**< Reserved */
	SFP_ETH_EXT_RESERVED_47 = 71, /**< Reserved */
	SFP_ETH_EXT_200GBASE_LR4 = 70, /**< 200GBASE-LR4 */
	SFP_ETH_EXT_50GBASE_LR = 69, /**< 50GBASE-LR */
	SFP_ETH_EXT_200G_1550NM_PSM4 = 68, /**< 200G 1550nm PSM4 */
	SFP_ETH_EXT_200GBASE_FR4 = 67, /**< 200GBASE-FR4 */
	SFP_ETH_EXT_50GBASE_FR_200GBASE_DR4 = 66, /**< 50GBASE-FR or 200GBASE-DR4 */
	SFP_ETH_EXT_50GBASE_SR_100GBASE_SR2_200GBASE_SR4 = 65, /**< 50GBASE-SR/100GBASE-SR2/200GBASE-SR4 */
	SFP_ETH_EXT_50GBASE_CR_100GBASE_CR2_200GBASE_CR4 = 64, /**< 50GBASE-CR/100GBASE-CR2/200GBASE-CR4 */
	SFP_ETH_EXT_RESERVED_3F = 63, /**< Reserved */
	SFP_ETH_EXT_RESERVED_3E = 62, /**< Reserved */
	SFP_ETH_EXT_RESERVED_3D = 61, /**< Reserved */
	SFP_ETH_EXT_RESERVED_3C = 60, /**< Reserved */
	SFP_ETH_EXT_RESERVED_3B = 59, /**< Reserved */
	SFP_ETH_EXT_RESERVED_3A = 58, /**< Reserved */
	SFP_ETH_EXT_RESERVED_39 = 57, /**< Reserved */
	SFP_ETH_EXT_RESERVED_38 = 56, /**< Reserved */
	SFP_ETH_EXT_RESERVED_37 = 55, /**< Reserved */
	SFP_ETH_EXT_RESERVED_36 = 54, /**< Reserved */
	SFP_ETH_EXT_RESERVED_35 = 53, /**< Reserved */
	SFP_ETH_EXT_RESERVED_34 = 52, /**< Reserved */
	SFP_ETH_EXT_50_100_200GAUI_AOC_HI_BER = 51, /**< 50GAUI/100GAUI-2/200GAUI-4 AOC (BER <2.6e-4) */
	SFP_ETH_EXT_50_100_200GAUI_ACC_HI_BER = 50, /**< 50GAUI/100GAUI-2/200GAUI-4 ACC (BER <2.6e-4) */
	SFP_ETH_EXT_50_100_200GAUI_AOC_LO_BER = 49, /**< 50GAUI/100GAUI-2/200GAUI-4 AOC (BER <1e-6) */
	SFP_ETH_EXT_50_100_200GAUI_ACC_LO_BER = 48, /**< 50GAUI/100GAUI-2/200GAUI-4 ACC (BER <1e-6) */
	SFP_ETH_EXT_RESERVED_2F = 47, /**< Reserved */
	SFP_ETH_EXT_RESERVED_2E = 46, /**< Reserved */
	SFP_ETH_EXT_RESERVED_2D = 45, /**< Reserved */
	SFP_ETH_EXT_RESERVED_2C = 44, /**< Reserved */
	SFP_ETH_EXT_RESERVED_2B = 43, /**< Reserved */
	SFP_ETH_EXT_RESERVED_2A = 42, /**< Reserved */
	SFP_ETH_EXT_RESERVED_29 = 41, /**< Reserved */
	SFP_ETH_EXT_RESERVED_28 = 40, /**< Reserved */
	SFP_ETH_EXT_100G_LR = 39, /**< 100G-LR */
	SFP_ETH_EXT_100G_FR = 38, /**< 100G-FR */
	SFP_ETH_EXT_100GBASE_DR = 37, /**< 100GBASE-DR */
	SFP_ETH_EXT_4WDM_40_MSA = 36, /**< 4WDM-40 MSA */
	SFP_ETH_EXT_4WDM_20_MSA = 35, /**< 4WDM-20 MSA */
	SFP_ETH_EXT_4WDM_10_MSA = 34, /**< 4WDM-10 MSA */
	SFP_ETH_EXT_100G_PAM4_BIDI = 33, /**< 100G PAM4 BiDi */
	SFP_ETH_EXT_100G_SWDM4 = 32, /**< 100G SWDM4 */
	SFP_ETH_EXT_40G_SWDM4 = 31, /**< 40G SWDM4 */
	SFP_ETH_EXT_2_5GBASE_T = 30, /**< 2.5GBASE-T */
	SFP_ETH_EXT_5GBASE_T = 29, /**< 5GBASE-T */
	SFP_ETH_EXT_10GBASE_T_SR = 28, /**< 10GBASE-T Short Reach */
	SFP_ETH_EXT_100G_1550NM_WDM = 27, /**< 100G 1550nm WDM */
	SFP_ETH_EXT_100GE_DWDM2 = 26, /**< 100GE-DWDM2 */
	SFP_ETH_EXT_100G_25GAUI_C2M_ACC = 25, /**< 100G ACC or 25GAUI C2M ACC */
	SFP_ETH_EXT_100G_25GAUI_C2M_AOC = 24, /**< 100G AOC or 25GAUI C2M AOC */
	SFP_ETH_EXT_100G_CLR4 = 23, /**< 100G CLR4 */
	SFP_ETH_EXT_10GBASE_T_SFI = 22, /**< 10GBASE-T with SFI electrical interface */
	SFP_ETH_EXT_G959_1_P1L1_2D2 = 21, /**< G959.1 profile P1L1-2D2 */
	SFP_ETH_EXT_G959_1_P1S1_2D2 = 20, /**< G959.1 profile P1S1-2D2 */
	SFP_ETH_EXT_G959_1_P1I1_2D1 = 19, /**< G959.1 profile P1I1-2D1 */
	SFP_ETH_EXT_40G_PSM4 = 18, /**< 40G PSM4 Parallel SMF */
	SFP_ETH_EXT_4X_10GBASE_SR = 17, /**< 4 x 10GBASE-SR */
	SFP_ETH_EXT_40GBASE_ER4 = 16, /**< 40GBASE-ER4 */
	SFP_ETH_EXT_RESERVED_0F = 15, /**< Reserved */
	SFP_ETH_EXT_RESERVED_0E = 14, /**< Reserved */
	SFP_ETH_EXT_CA_25G_N = 13, /**< 25GBASE-CR CA-25G-N */
	SFP_ETH_EXT_CA_25G_S = 12, /**< 25GBASE-CR CA-25G-S */
	SFP_ETH_EXT_CA_L = 11, /**< 100GBASE-CR4 or 25GBASE-CR CA-L */
	SFP_ETH_EXT_RESERVED_0A = 10, /**< Reserved */
	SFP_ETH_EXT_OBSOLETE = 9, /**< Obsolete */
	SFP_ETH_EXT_100G_25GAUI_C2M_ACC_1 = 8, /**< 100G ACC (Active Copper Cable */
	SFP_ETH_EXT_100G_PSM4_P_SMF = 7, /**< 100G PSM4 Parallel SMF */
	SFP_ETH_EXT_100G_CWDM4 = 6, /**< 100G CWDM4 */
	SFP_ETH_EXT_100GBASE_SR10 = 5, /**< 100GBASE-SR10 */
	SFP_ETH_EXT_100GBASE_ER4_25GBASE_ER = 4, /**< 100GBASE-ER4 or 25GBASE-ER */
	SFP_ETH_EXT_100GBASE_LR4_25GBASE_LR = 3, /**< 100GBASE-LR4 or 25GBASE-LR */
	SFP_ETH_EXT_100GBASE_SR4_25GBASE_SR = 2, /**< 100GBASE-SR4 or 25GBASE-SR */
	SFP_ETH_EXT_100G_25GAUI_C2M_AOC_1 = 1, /**< 100G AOC (Active Optical Cable */
	SFP_ETH_EXT_UNSPECIFIED = 0, /**< Unspecified */
};

/** Get the symbolic name of a given sfp_eth_ext value */
const char *ifconfig_sfp_eth_ext_symbol(enum sfp_eth_ext);

/** Get a brief description of a given sfp_eth_ext value */
const char *ifconfig_sfp_eth_ext_description(enum sfp_eth_ext);

/*
 * SFF-8636 Rev. 2.9 table 6.3: Revision compliance
 */

/** Revision compliance */
enum sfp_rev {
	SFP_REV_SFF_8436_REV_LE_4_8 = 1, /**< SFF-8436 rev <=4.8 */
	SFP_REV_SFF_8436_REV_LE_4_8_ALT = 2, /**< SFF-8436 rev <=4.8 */
	SFP_REV_SFF_8636_REV_LE_1_3 = 3, /**< SFF-8636 rev <=1.3 */
	SFP_REV_SFF_8636_REV_LE_1_4 = 4, /**< SFF-8636 rev <=1.4 */
	SFP_REV_SFF_8636_REV_LE_1_5 = 5, /**< SFF-8636 rev <=1.5 */
	SFP_REV_SFF_8636_REV_LE_2_0 = 6, /**< SFF-8636 rev <=2.0 */
	SFP_REV_SFF_8636_REV_LE_2_7 = 7, /**< SFF-8636 rev <=2.7 */
	SFP_REV_SFF_8363_REV_GE_2_8 = 8, /**< SFF-8636 rev >=2.8 */
	SFP_REV_UNSPECIFIED = 0, /**< Unspecified */
};

/** Get the symbolic name of a given sfp_rev value */
const char *ifconfig_sfp_rev_symbol(enum sfp_rev);

/** Get a brief description of a given sfp_rev value */
const char *ifconfig_sfp_rev_description(enum sfp_rev);

/*
 * Descriptions of each enum
 */

/** Get a brief description of the sfp_id enum */
static inline const char *
ifconfig_enum_sfp_id_description(void)
{
	return ("Transceiver identifier");
}

/** Get a brief description of the sfp_conn enum */
static inline const char *
ifconfig_enum_sfp_conn_description(void)
{
	return ("Connector type");
}

/** Get a brief description of the sfp_eth_10g enum */
static inline const char *
ifconfig_enum_sfp_eth_10g_description(void)
{
	return ("10G Ethernet/IB compliance");
}

/** Get a brief description of the sfp_eth enum */
static inline const char *
ifconfig_enum_sfp_eth_description(void)
{
	return ("Ethernet compliance");
}

/** Get a brief description of the sfp_fc_len enum */
static inline const char *
ifconfig_enum_sfp_fc_len_description(void)
{
	return ("Fibre Channel link length");
}

/** Get a brief description of the sfp_cab_tech enum */
static inline const char *
ifconfig_enum_sfp_cab_tech_description(void)
{
	return ("Channel/cable technology");
}

/** Get a brief description of the sfp_fc_media enum */
static inline const char *
ifconfig_enum_sfp_fc_media_description(void)
{
	return ("Fibre Channel transmission media");
}

/** Get a brief description of the sfp_fc_speed enum */
static inline const char *
ifconfig_enum_sfp_fc_speed_description(void)
{
	return ("Fibre Channel speed");
}

/** Get a brief description of the sfp_eth_1040g enum */
static inline const char *
ifconfig_enum_sfp_eth_1040g_description(void)
{
	return ("10/40G Ethernet compliance");
}

/** Get a brief description of the sfp_eth_ext enum */
static inline const char *
ifconfig_enum_sfp_eth_ext_description(void)
{
	return ("Extended specification compliance");
}

/** Get a brief description of the sfp_rev enum */
static inline const char *
ifconfig_enum_sfp_rev_description(void)
{
	return ("Revision compliance");
}

/*
 * Info struct definitions
 */

struct ifconfig_sfp_info {
	uint8_t sfp_id; /**< Transceiver identifier */
	uint8_t sfp_conn; /**< Connector type */
	uint8_t sfp_eth_10g; /**< 10G Ethernet/IB compliance */
	uint8_t sfp_eth; /**< Ethernet compliance */
	uint8_t sfp_fc_len; /**< Fibre Channel link length */
	uint16_t sfp_cab_tech; /**< Channel/cable technology */
	uint8_t sfp_fc_media; /**< Fibre Channel transmission media */
	uint8_t sfp_fc_speed; /**< Fibre Channel speed */
	uint8_t sfp_eth_1040g; /**< 10/40G Ethernet compliance */
	uint8_t sfp_eth_ext; /**< Extended specification compliance */
	uint8_t sfp_rev; /**< Revision compliance */
};

struct ifconfig_sfp_info_strings {
	const char *sfp_id; /**< Transceiver identifier */
	const char *sfp_conn; /**< Connector type */
	const char *sfp_eth_10g; /**< 10G Ethernet/IB compliance */
	const char *sfp_eth; /**< Ethernet compliance */
	const char *sfp_fc_len; /**< Fibre Channel link length */
	const char *sfp_cab_tech; /**< Channel/cable technology */
	const char *sfp_fc_media; /**< Fibre Channel transmission media */
	const char *sfp_fc_speed; /**< Fibre Channel speed */
	const char *sfp_eth_1040g; /**< 10/40G Ethernet compliance */
	const char *sfp_eth_ext; /**< Extended specification compliance */
	const char *sfp_rev; /**< Revision compliance */
};
