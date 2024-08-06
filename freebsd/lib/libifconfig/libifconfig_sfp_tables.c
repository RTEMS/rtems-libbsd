#include <machine/rtems-bsd-user-space.h>
#include <machine/rtems-bsd-program.h>

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


#include <libifconfig_sfp_tables.h>
#include <libifconfig_sfp_tables_internal.h>

struct sfp_enum_metadata {
	int		value;		/* numeric discriminant value */
	const char	*symbol;	/* symbolic name */
	const char	*description;	/* brief description */
	const char	*display;	/* shortened display name */
};

const struct sfp_enum_metadata *
find_metadata(const struct sfp_enum_metadata *table, int value)
{
	while (table->value != value && table->symbol != NULL)
		++table;
	return (table->symbol != NULL ? table : NULL);
}

/*
 * SFF-8024 Rev. 4.6 Table 4-1: Indentifier Values
 */

static const struct sfp_enum_metadata sfp_id_table_[] = {
	{
		.value = SFP_ID_UNKNOWN,
		.symbol = "SFP_ID_UNKNOWN",
		.description = "Unknown or unspecified",
		.display = "Unknown",
	},
	{
		.value = SFP_ID_GBIC,
		.symbol = "SFP_ID_GBIC",
		.description = "GBIC",
		.display = "GBIC",
	},
	{
		.value = SFP_ID_SFF,
		.symbol = "SFP_ID_SFF",
		.description = "Module soldered to motherboard (ex: SFF)",
		.display = "SFF",
	},
	{
		.value = SFP_ID_SFP,
		.symbol = "SFP_ID_SFP",
		.description = "SFP or SFP+",
		.display = "SFP/SFP+/SFP28",
	},
	{
		.value = SFP_ID_XBI,
		.symbol = "SFP_ID_XBI",
		.description = "300 pin XBI",
		.display = "XBI",
	},
	{
		.value = SFP_ID_XENPAK,
		.symbol = "SFP_ID_XENPAK",
		.description = "Xenpak",
		.display = "Xenpak",
	},
	{
		.value = SFP_ID_XFP,
		.symbol = "SFP_ID_XFP",
		.description = "XFP",
		.display = "XFP",
	},
	{
		.value = SFP_ID_XFF,
		.symbol = "SFP_ID_XFF",
		.description = "XFF",
		.display = "XFF",
	},
	{
		.value = SFP_ID_XFPE,
		.symbol = "SFP_ID_XFPE",
		.description = "XFP-E",
		.display = "XFP-E",
	},
	{
		.value = SFP_ID_XPAK,
		.symbol = "SFP_ID_XPAK",
		.description = "XPAK",
		.display = "XPAK",
	},
	{
		.value = SFP_ID_X2,
		.symbol = "SFP_ID_X2",
		.description = "X2",
		.display = "X2",
	},
	{
		.value = SFP_ID_DWDM_SFP,
		.symbol = "SFP_ID_DWDM_SFP",
		.description = "DWDM-SFP/SFP+",
		.display = "DWDM-SFP/SFP+",
	},
	{
		.value = SFP_ID_QSFP,
		.symbol = "SFP_ID_QSFP",
		.description = "QSFP",
		.display = "QSFP",
	},
	{
		.value = SFP_ID_QSFPPLUS,
		.symbol = "SFP_ID_QSFPPLUS",
		.description = "QSFP+ or later",
		.display = "QSFP+",
	},
	{
		.value = SFP_ID_CXP,
		.symbol = "SFP_ID_CXP",
		.description = "CXP",
		.display = "CXP",
	},
	{
		.value = SFP_ID_HD4X,
		.symbol = "SFP_ID_HD4X",
		.description = "Shielded Mini Multilane HD 4X",
		.display = "HD4X",
	},
	{
		.value = SFP_ID_HD8X,
		.symbol = "SFP_ID_HD8X",
		.description = "Shielded Mini Multilane HD 8X",
		.display = "HD8X",
	},
	{
		.value = SFP_ID_QSFP28,
		.symbol = "SFP_ID_QSFP28",
		.description = "QSFP28 or later",
		.display = "QSFP28",
	},
	{
		.value = SFP_ID_CXP2,
		.symbol = "SFP_ID_CXP2",
		.description = "CXP2 (aka CXP28)",
		.display = "CXP2",
	},
	{
		.value = SFP_ID_CDFP,
		.symbol = "SFP_ID_CDFP",
		.description = "CDFP (Style 1/Style 2)",
		.display = "CDFP",
	},
	{
		.value = SFP_ID_SMM4,
		.symbol = "SFP_ID_SMM4",
		.description = "Shielded Mini Multilane HD 4X fanout",
		.display = "SMM4",
	},
	{
		.value = SFP_ID_SMM8,
		.symbol = "SFP_ID_SMM8",
		.description = "Shielded Mini Multilane HD 8X fanout",
		.display = "SMM8",
	},
	{
		.value = SFP_ID_CDFP3,
		.symbol = "SFP_ID_CDFP3",
		.description = "CDFP (Style 3)",
		.display = "CDFP3",
	},
	{
		.value = SFP_ID_MICROQSFP,
		.symbol = "SFP_ID_MICROQSFP",
		.description = "microQSFP",
		.display = "microQSFP",
	},
	{
		.value = SFP_ID_QSFP_DD,
		.symbol = "SFP_ID_QSFP_DD",
		.description = "QSFP-DD 8X pluggable transceiver",
		.display = "QSFP-DD",
	},
	{
		.value = SFP_ID_QSFP8X,
		.symbol = "SFP_ID_QSFP8X",
		.description = "QSFP 8X pluggable transceiver",
		.display = "QSFP8X",
	},
	{
		.value = SFP_ID_SFP_DD,
		.symbol = "SFP_ID_SFP_DD",
		.description = "SFP-DD 2X pluggable transceiver",
		.display = "SFP-DD",
	},
	{
		.value = SFP_ID_DSFP,
		.symbol = "SFP_ID_DSFP",
		.description = "DSFP Dual SFP pluggable transceiver",
		.display = "DSFP",
	},
	{
		.value = SFP_ID_X4ML,
		.symbol = "SFP_ID_X4ML",
		.description = "x4 MiniLink/OcuLink",
		.display = "x4MiniLink/OcuLink",
	},
	{
		.value = SFP_ID_X8ML,
		.symbol = "SFP_ID_X8ML",
		.description = "x8 MiniLink",
		.display = "x8MiniLink",
	},
	{
		.value = SFP_ID_QSFP_CMIS,
		.symbol = "SFP_ID_QSFP_CMIS",
		.description = "QSFP+ or later w/Common Management Interface Specification",
		.display = "QSFP+(CMIS)",
	},
	{0}
};
const struct sfp_enum_metadata *sfp_id_table = sfp_id_table_;

const char *
ifconfig_sfp_id_symbol(enum sfp_id v)
{
	const struct sfp_enum_metadata *metadata;

	if ((metadata = find_metadata(sfp_id_table, v)) == NULL)
		return (NULL);
	return (metadata->symbol);
}

const char *
ifconfig_sfp_id_description(enum sfp_id v)
{
	const struct sfp_enum_metadata *metadata;

	if ((metadata = find_metadata(sfp_id_table, v)) == NULL)
		return (NULL);
	return (metadata->description);
}

const char *
ifconfig_sfp_id_display(enum sfp_id v)
{
	const struct sfp_enum_metadata *metadata;

	if ((metadata = find_metadata(sfp_id_table, v)) == NULL)
		return (NULL);
	return (metadata->display);
}

/*
 * SFF-8024 Rev. 4.6 Table 4-3: Connector Types
 */

static const struct sfp_enum_metadata sfp_conn_table_[] = {
	{
		.value = SFP_CONN_UNKNOWN,
		.symbol = "SFP_CONN_UNKNOWN",
		.description = "Unknown",
	},
	{
		.value = SFP_CONN_SC,
		.symbol = "SFP_CONN_SC",
		.description = "SC",
	},
	{
		.value = SFP_CONN_FC_1_COPPER,
		.symbol = "SFP_CONN_FC_1_COPPER",
		.description = "Fibre Channel Style 1 copper",
	},
	{
		.value = SFP_CONN_FC_2_COPPER,
		.symbol = "SFP_CONN_FC_2_COPPER",
		.description = "Fibre Channel Style 2 copper",
	},
	{
		.value = SFP_CONN_BNC_TNC,
		.symbol = "SFP_CONN_BNC_TNC",
		.description = "BNC/TNC",
	},
	{
		.value = SFP_CONN_FC_COAX,
		.symbol = "SFP_CONN_FC_COAX",
		.description = "Fibre Channel coaxial",
	},
	{
		.value = SFP_CONN_FIBER_JACK,
		.symbol = "SFP_CONN_FIBER_JACK",
		.description = "Fiber Jack",
	},
	{
		.value = SFP_CONN_LC,
		.symbol = "SFP_CONN_LC",
		.description = "LC",
	},
	{
		.value = SFP_CONN_MT_RJ,
		.symbol = "SFP_CONN_MT_RJ",
		.description = "MT-RJ",
	},
	{
		.value = SFP_CONN_MU,
		.symbol = "SFP_CONN_MU",
		.description = "MU",
	},
	{
		.value = SFP_CONN_SG,
		.symbol = "SFP_CONN_SG",
		.description = "SG",
	},
	{
		.value = SFP_CONN_OPTICAL_PIGTAIL,
		.symbol = "SFP_CONN_OPTICAL_PIGTAIL",
		.description = "Optical pigtail",
	},
	{
		.value = SFP_CONN_MPO_1X12_POPTIC,
		.symbol = "SFP_CONN_MPO_1X12_POPTIC",
		.description = "MPO 1x12 Parallel Optic",
	},
	{
		.value = SFP_CONN_MPO_2X16_POPTIC,
		.symbol = "SFP_CONN_MPO_2X16_POPTIC",
		.description = "MPO 2x16 Parallel Optic",
	},
	{
		.value = SFP_CONN_HSSDC_II,
		.symbol = "SFP_CONN_HSSDC_II",
		.description = "HSSDC II",
	},
	{
		.value = SFP_CONN_COPPER_PIGTAIL,
		.symbol = "SFP_CONN_COPPER_PIGTAIL",
		.description = "Copper pigtail",
	},
	{
		.value = SFP_CONN_RJ45,
		.symbol = "SFP_CONN_RJ45",
		.description = "RJ45",
	},
	{
		.value = SFP_CONN_NONE,
		.symbol = "SFP_CONN_NONE",
		.description = "No separable connector",
	},
	{
		.value = SFP_CONN_MXC_2X16,
		.symbol = "SFP_CONN_MXC_2X16",
		.description = "MXC 2x16",
	},
	{
		.value = SFP_CONN_CS_OPTICAL,
		.symbol = "SFP_CONN_CS_OPTICAL",
		.description = "CS optical connector",
	},
	{
		.value = SFP_CONN_MINI_CS_OPTICAL,
		.symbol = "SFP_CONN_MINI_CS_OPTICAL",
		.description = "Mini CS optical connector",
	},
	{
		.value = SFP_CONN_MPO_2X12_POPTIC,
		.symbol = "SFP_CONN_MPO_2X12_POPTIC",
		.description = "MPO 2x12 Parallel Optic",
	},
	{
		.value = SFP_CONN_MPO_1X16_POPTIC,
		.symbol = "SFP_CONN_MPO_1X16_POPTIC",
		.description = "MPO 1x16 Parallel Optic",
	},
	{0}
};
const struct sfp_enum_metadata *sfp_conn_table = sfp_conn_table_;

const char *
ifconfig_sfp_conn_symbol(enum sfp_conn v)
{
	const struct sfp_enum_metadata *metadata;

	if ((metadata = find_metadata(sfp_conn_table, v)) == NULL)
		return (NULL);
	return (metadata->symbol);
}

const char *
ifconfig_sfp_conn_description(enum sfp_conn v)
{
	const struct sfp_enum_metadata *metadata;

	if ((metadata = find_metadata(sfp_conn_table, v)) == NULL)
		return (NULL);
	return (metadata->description);
}

/*
 * SFF-8472 Rev. 11.4 table 3.5: Transceiver codes
 */

/*
 * 10G Ethernet/IB compliance codes, byte 3
 */

static const struct sfp_enum_metadata sfp_eth_10g_table_[] = {
	{
		.value = SFP_ETH_10G_10G_BASE_ER,
		.symbol = "SFP_ETH_10G_10G_BASE_ER",
		.description = "10G Base-ER",
	},
	{
		.value = SFP_ETH_10G_10G_BASE_LRM,
		.symbol = "SFP_ETH_10G_10G_BASE_LRM",
		.description = "10G Base-LRM",
	},
	{
		.value = SFP_ETH_10G_10G_BASE_LR,
		.symbol = "SFP_ETH_10G_10G_BASE_LR",
		.description = "10G Base-LR",
	},
	{
		.value = SFP_ETH_10G_10G_BASE_SR,
		.symbol = "SFP_ETH_10G_10G_BASE_SR",
		.description = "10G Base-SR",
	},
	{
		.value = SFP_ETH_10G_1X_SX,
		.symbol = "SFP_ETH_10G_1X_SX",
		.description = "1X SX",
	},
	{
		.value = SFP_ETH_10G_1X_LX,
		.symbol = "SFP_ETH_10G_1X_LX",
		.description = "1X LX",
	},
	{
		.value = SFP_ETH_10G_1X_COPPER_ACTIVE,
		.symbol = "SFP_ETH_10G_1X_COPPER_ACTIVE",
		.description = "1X Copper Active",
	},
	{
		.value = SFP_ETH_10G_1X_COPPER_PASSIVE,
		.symbol = "SFP_ETH_10G_1X_COPPER_PASSIVE",
		.description = "1X Copper Passive",
	},
	{0}
};
const struct sfp_enum_metadata *sfp_eth_10g_table = sfp_eth_10g_table_;

const char *
ifconfig_sfp_eth_10g_symbol(enum sfp_eth_10g v)
{
	const struct sfp_enum_metadata *metadata;

	if ((metadata = find_metadata(sfp_eth_10g_table, v)) == NULL)
		return (NULL);
	return (metadata->symbol);
}

const char *
ifconfig_sfp_eth_10g_description(enum sfp_eth_10g v)
{
	const struct sfp_enum_metadata *metadata;

	if ((metadata = find_metadata(sfp_eth_10g_table, v)) == NULL)
		return (NULL);
	return (metadata->description);
}

/*
 * Ethernet compliance codes, byte 6
 */

static const struct sfp_enum_metadata sfp_eth_table_[] = {
	{
		.value = SFP_ETH_BASE_PX,
		.symbol = "SFP_ETH_BASE_PX",
		.description = "BASE-PX",
	},
	{
		.value = SFP_ETH_BASE_BX10,
		.symbol = "SFP_ETH_BASE_BX10",
		.description = "BASE-BX10",
	},
	{
		.value = SFP_ETH_100BASE_FX,
		.symbol = "SFP_ETH_100BASE_FX",
		.description = "100BASE-FX",
	},
	{
		.value = SFP_ETH_100BASE_LX_LX10,
		.symbol = "SFP_ETH_100BASE_LX_LX10",
		.description = "100BASE-LX/LX10",
	},
	{
		.value = SFP_ETH_1000BASE_T,
		.symbol = "SFP_ETH_1000BASE_T",
		.description = "1000BASE-T",
	},
	{
		.value = SFP_ETH_1000BASE_CX,
		.symbol = "SFP_ETH_1000BASE_CX",
		.description = "1000BASE-CX",
	},
	{
		.value = SFP_ETH_1000BASE_LX,
		.symbol = "SFP_ETH_1000BASE_LX",
		.description = "1000BASE-LX",
	},
	{
		.value = SFP_ETH_1000BASE_SX,
		.symbol = "SFP_ETH_1000BASE_SX",
		.description = "1000BASE-SX",
	},
	{0}
};
const struct sfp_enum_metadata *sfp_eth_table = sfp_eth_table_;

const char *
ifconfig_sfp_eth_symbol(enum sfp_eth v)
{
	const struct sfp_enum_metadata *metadata;

	if ((metadata = find_metadata(sfp_eth_table, v)) == NULL)
		return (NULL);
	return (metadata->symbol);
}

const char *
ifconfig_sfp_eth_description(enum sfp_eth v)
{
	const struct sfp_enum_metadata *metadata;

	if ((metadata = find_metadata(sfp_eth_table, v)) == NULL)
		return (NULL);
	return (metadata->description);
}

/*
 * FC link length, byte 7
 */

static const struct sfp_enum_metadata sfp_fc_len_table_[] = {
	{
		.value = SFP_FC_LEN_VERY_LONG,
		.symbol = "SFP_FC_LEN_VERY_LONG",
		.description = "very long distance",
	},
	{
		.value = SFP_FC_LEN_SHORT,
		.symbol = "SFP_FC_LEN_SHORT",
		.description = "short distance",
	},
	{
		.value = SFP_FC_LEN_INTERMEDIATE,
		.symbol = "SFP_FC_LEN_INTERMEDIATE",
		.description = "intermediate distance",
	},
	{
		.value = SFP_FC_LEN_LONG,
		.symbol = "SFP_FC_LEN_LONG",
		.description = "long distance",
	},
	{
		.value = SFP_FC_LEN_MEDIUM,
		.symbol = "SFP_FC_LEN_MEDIUM",
		.description = "medium distance",
	},
	{0}
};
const struct sfp_enum_metadata *sfp_fc_len_table = sfp_fc_len_table_;

const char *
ifconfig_sfp_fc_len_symbol(enum sfp_fc_len v)
{
	const struct sfp_enum_metadata *metadata;

	if ((metadata = find_metadata(sfp_fc_len_table, v)) == NULL)
		return (NULL);
	return (metadata->symbol);
}

const char *
ifconfig_sfp_fc_len_description(enum sfp_fc_len v)
{
	const struct sfp_enum_metadata *metadata;

	if ((metadata = find_metadata(sfp_fc_len_table, v)) == NULL)
		return (NULL);
	return (metadata->description);
}

/*
 * Channel/Cable technology, byte 7-8
 */

static const struct sfp_enum_metadata sfp_cab_tech_table_[] = {
	{
		.value = SFP_CAB_TECH_SA,
		.symbol = "SFP_CAB_TECH_SA",
		.description = "Shortwave laser (SA)",
	},
	{
		.value = SFP_CAB_TECH_LC,
		.symbol = "SFP_CAB_TECH_LC",
		.description = "Longwave laser (LC)",
	},
	{
		.value = SFP_CAB_TECH_EL_INTER,
		.symbol = "SFP_CAB_TECH_EL_INTER",
		.description = "Electrical inter-enclosure (EL)",
	},
	{
		.value = SFP_CAB_TECH_EL_INTRA,
		.symbol = "SFP_CAB_TECH_EL_INTRA",
		.description = "Electrical intra-enclosure (EL)",
	},
	{
		.value = SFP_CAB_TECH_SN,
		.symbol = "SFP_CAB_TECH_SN",
		.description = "Shortwave laser (SN)",
	},
	{
		.value = SFP_CAB_TECH_SL,
		.symbol = "SFP_CAB_TECH_SL",
		.description = "Shortwave laser (SL)",
	},
	{
		.value = SFP_CAB_TECH_LL,
		.symbol = "SFP_CAB_TECH_LL",
		.description = "Longwave laser (LL)",
	},
	{
		.value = SFP_CAB_TECH_ACTIVE,
		.symbol = "SFP_CAB_TECH_ACTIVE",
		.description = "Active Cable",
	},
	{
		.value = SFP_CAB_TECH_PASSIVE,
		.symbol = "SFP_CAB_TECH_PASSIVE",
		.description = "Passive Cable",
	},
	{0}
};
const struct sfp_enum_metadata *sfp_cab_tech_table = sfp_cab_tech_table_;

const char *
ifconfig_sfp_cab_tech_symbol(enum sfp_cab_tech v)
{
	const struct sfp_enum_metadata *metadata;

	if ((metadata = find_metadata(sfp_cab_tech_table, v)) == NULL)
		return (NULL);
	return (metadata->symbol);
}

const char *
ifconfig_sfp_cab_tech_description(enum sfp_cab_tech v)
{
	const struct sfp_enum_metadata *metadata;

	if ((metadata = find_metadata(sfp_cab_tech_table, v)) == NULL)
		return (NULL);
	return (metadata->description);
}

/*
 * FC Transmission media, byte 9
 */

static const struct sfp_enum_metadata sfp_fc_media_table_[] = {
	{
		.value = SFP_FC_MEDIA_TW,
		.symbol = "SFP_FC_MEDIA_TW",
		.description = "Twin Axial Pair (TW)",
	},
	{
		.value = SFP_FC_MEDIA_TP,
		.symbol = "SFP_FC_MEDIA_TP",
		.description = "Twisted Pair (TP)",
	},
	{
		.value = SFP_FC_MEDIA_MI,
		.symbol = "SFP_FC_MEDIA_MI",
		.description = "Miniature Coax (MI)",
	},
	{
		.value = SFP_FC_MEDIA_TV,
		.symbol = "SFP_FC_MEDIA_TV",
		.description = "Video Coax (TV)",
	},
	{
		.value = SFP_FC_MEDIA_M6,
		.symbol = "SFP_FC_MEDIA_M6",
		.description = "Miltimode 62.5um (M6)",
	},
	{
		.value = SFP_FC_MEDIA_M5,
		.symbol = "SFP_FC_MEDIA_M5",
		.description = "Multimode 50um (M5)",
	},
	{
		.value = SFP_FC_MEDIA_RESERVED,
		.symbol = "SFP_FC_MEDIA_RESERVED",
		.description = "Reserved",
	},
	{
		.value = SFP_FC_MEDIA_SM,
		.symbol = "SFP_FC_MEDIA_SM",
		.description = "Single Mode (SM)",
	},
	{0}
};
const struct sfp_enum_metadata *sfp_fc_media_table = sfp_fc_media_table_;

const char *
ifconfig_sfp_fc_media_symbol(enum sfp_fc_media v)
{
	const struct sfp_enum_metadata *metadata;

	if ((metadata = find_metadata(sfp_fc_media_table, v)) == NULL)
		return (NULL);
	return (metadata->symbol);
}

const char *
ifconfig_sfp_fc_media_description(enum sfp_fc_media v)
{
	const struct sfp_enum_metadata *metadata;

	if ((metadata = find_metadata(sfp_fc_media_table, v)) == NULL)
		return (NULL);
	return (metadata->description);
}

/*
 * FC Speed, byte 10
 */

static const struct sfp_enum_metadata sfp_fc_speed_table_[] = {
	{
		.value = SFP_FC_SPEED_1200,
		.symbol = "SFP_FC_SPEED_1200",
		.description = "1200 MBytes/sec",
	},
	{
		.value = SFP_FC_SPEED_800,
		.symbol = "SFP_FC_SPEED_800",
		.description = "800 MBytes/sec",
	},
	{
		.value = SFP_FC_SPEED_1600,
		.symbol = "SFP_FC_SPEED_1600",
		.description = "1600 MBytes/sec",
	},
	{
		.value = SFP_FC_SPEED_400,
		.symbol = "SFP_FC_SPEED_400",
		.description = "400 MBytes/sec",
	},
	{
		.value = SFP_FC_SPEED_3200,
		.symbol = "SFP_FC_SPEED_3200",
		.description = "3200 MBytes/sec",
	},
	{
		.value = SFP_FC_SPEED_200,
		.symbol = "SFP_FC_SPEED_200",
		.description = "200 MBytes/sec",
	},
	{
		.value = SFP_FC_SPEED_100,
		.symbol = "SFP_FC_SPEED_100",
		.description = "100 MBytes/sec",
	},
	{0}
};
const struct sfp_enum_metadata *sfp_fc_speed_table = sfp_fc_speed_table_;

const char *
ifconfig_sfp_fc_speed_symbol(enum sfp_fc_speed v)
{
	const struct sfp_enum_metadata *metadata;

	if ((metadata = find_metadata(sfp_fc_speed_table, v)) == NULL)
		return (NULL);
	return (metadata->symbol);
}

const char *
ifconfig_sfp_fc_speed_description(enum sfp_fc_speed v)
{
	const struct sfp_enum_metadata *metadata;

	if ((metadata = find_metadata(sfp_fc_speed_table, v)) == NULL)
		return (NULL);
	return (metadata->description);
}

/*
 * SFF-8436 Rev. 4.8 table 33: Specification compliance
 */

/*
 * 10/40G Ethernet compliance codes, byte 128 + 3
 */

static const struct sfp_enum_metadata sfp_eth_1040g_table_[] = {
	{
		.value = SFP_ETH_1040G_EXTENDED,
		.symbol = "SFP_ETH_1040G_EXTENDED",
		.description = "Extended",
	},
	{
		.value = SFP_ETH_1040G_10GBASE_LRM,
		.symbol = "SFP_ETH_1040G_10GBASE_LRM",
		.description = "10GBASE-LRM",
	},
	{
		.value = SFP_ETH_1040G_10GBASE_LR,
		.symbol = "SFP_ETH_1040G_10GBASE_LR",
		.description = "10GBASE-LR",
	},
	{
		.value = SFP_ETH_1040G_10GBASE_SR,
		.symbol = "SFP_ETH_1040G_10GBASE_SR",
		.description = "10GBASE-SR",
	},
	{
		.value = SFP_ETH_1040G_40GBASE_CR4,
		.symbol = "SFP_ETH_1040G_40GBASE_CR4",
		.description = "40GBASE-CR4",
	},
	{
		.value = SFP_ETH_1040G_40GBASE_SR4,
		.symbol = "SFP_ETH_1040G_40GBASE_SR4",
		.description = "40GBASE-SR4",
	},
	{
		.value = SFP_ETH_1040G_40GBASE_LR4,
		.symbol = "SFP_ETH_1040G_40GBASE_LR4",
		.description = "40GBASE-LR4",
	},
	{
		.value = SFP_ETH_1040G_40G_ACTIVE,
		.symbol = "SFP_ETH_1040G_40G_ACTIVE",
		.description = "40G Active Cable",
	},
	{0}
};
const struct sfp_enum_metadata *sfp_eth_1040g_table = sfp_eth_1040g_table_;

const char *
ifconfig_sfp_eth_1040g_symbol(enum sfp_eth_1040g v)
{
	const struct sfp_enum_metadata *metadata;

	if ((metadata = find_metadata(sfp_eth_1040g_table, v)) == NULL)
		return (NULL);
	return (metadata->symbol);
}

const char *
ifconfig_sfp_eth_1040g_description(enum sfp_eth_1040g v)
{
	const struct sfp_enum_metadata *metadata;

	if ((metadata = find_metadata(sfp_eth_1040g_table, v)) == NULL)
		return (NULL);
	return (metadata->description);
}

/*
 * SFF-8024 Rev. 4.6 table 4-4: Extended Specification Compliance
 */

static const struct sfp_enum_metadata sfp_eth_ext_table_[] = {
	{
		.value = SFP_ETH_EXT_RESERVED_FF,
		.symbol = "SFP_ETH_EXT_RESERVED_FF",
		.description = "Reserved",
	},
	{
		.value = SFP_ETH_EXT_128GFC_LW,
		.symbol = "SFP_ETH_EXT_128GFC_LW",
		.description = "128GFC LW",
	},
	{
		.value = SFP_ETH_EXT_128GFC_SW,
		.symbol = "SFP_ETH_EXT_128GFC_SW",
		.description = "128GFC SW",
	},
	{
		.value = SFP_ETH_EXT_128GFC_EA,
		.symbol = "SFP_ETH_EXT_128GFC_EA",
		.description = "128GFC EA",
	},
	{
		.value = SFP_ETH_EXT_64GFC_LW,
		.symbol = "SFP_ETH_EXT_64GFC_LW",
		.description = "64GFC LW",
	},
	{
		.value = SFP_ETH_EXT_64GFC_SW,
		.symbol = "SFP_ETH_EXT_64GFC_SW",
		.description = "64GFC SW",
	},
	{
		.value = SFP_ETH_EXT_64GFC_EA,
		.symbol = "SFP_ETH_EXT_64GFC_EA",
		.description = "64GFC EA",
	},
	{
		.value = SFP_ETH_EXT_RESERVED_4F,
		.symbol = "SFP_ETH_EXT_RESERVED_4F",
		.description = "Reserved",
	},
	{
		.value = SFP_ETH_EXT_RESERVED_4E,
		.symbol = "SFP_ETH_EXT_RESERVED_4E",
		.description = "Reserved",
	},
	{
		.value = SFP_ETH_EXT_RESERVED_4D,
		.symbol = "SFP_ETH_EXT_RESERVED_4D",
		.description = "Reserved",
	},
	{
		.value = SFP_ETH_EXT_RESERVED_4C,
		.symbol = "SFP_ETH_EXT_RESERVED_4C",
		.description = "Reserved",
	},
	{
		.value = SFP_ETH_EXT_RESERVED_4B,
		.symbol = "SFP_ETH_EXT_RESERVED_4B",
		.description = "Reserved",
	},
	{
		.value = SFP_ETH_EXT_RESERVED_4A,
		.symbol = "SFP_ETH_EXT_RESERVED_4A",
		.description = "Reserved",
	},
	{
		.value = SFP_ETH_EXT_RESERVED_49,
		.symbol = "SFP_ETH_EXT_RESERVED_49",
		.description = "Reserved",
	},
	{
		.value = SFP_ETH_EXT_RESERVED_48,
		.symbol = "SFP_ETH_EXT_RESERVED_48",
		.description = "Reserved",
	},
	{
		.value = SFP_ETH_EXT_RESERVED_47,
		.symbol = "SFP_ETH_EXT_RESERVED_47",
		.description = "Reserved",
	},
	{
		.value = SFP_ETH_EXT_200GBASE_LR4,
		.symbol = "SFP_ETH_EXT_200GBASE_LR4",
		.description = "200GBASE-LR4",
	},
	{
		.value = SFP_ETH_EXT_50GBASE_LR,
		.symbol = "SFP_ETH_EXT_50GBASE_LR",
		.description = "50GBASE-LR",
	},
	{
		.value = SFP_ETH_EXT_200G_1550NM_PSM4,
		.symbol = "SFP_ETH_EXT_200G_1550NM_PSM4",
		.description = "200G 1550nm PSM4",
	},
	{
		.value = SFP_ETH_EXT_200GBASE_FR4,
		.symbol = "SFP_ETH_EXT_200GBASE_FR4",
		.description = "200GBASE-FR4",
	},
	{
		.value = SFP_ETH_EXT_50GBASE_FR_200GBASE_DR4,
		.symbol = "SFP_ETH_EXT_50GBASE_FR_200GBASE_DR4",
		.description = "50GBASE-FR or 200GBASE-DR4",
	},
	{
		.value = SFP_ETH_EXT_50GBASE_SR_100GBASE_SR2_200GBASE_SR4,
		.symbol = "SFP_ETH_EXT_50GBASE_SR_100GBASE_SR2_200GBASE_SR4",
		.description = "50GBASE-SR/100GBASE-SR2/200GBASE-SR4",
	},
	{
		.value = SFP_ETH_EXT_50GBASE_CR_100GBASE_CR2_200GBASE_CR4,
		.symbol = "SFP_ETH_EXT_50GBASE_CR_100GBASE_CR2_200GBASE_CR4",
		.description = "50GBASE-CR/100GBASE-CR2/200GBASE-CR4",
	},
	{
		.value = SFP_ETH_EXT_RESERVED_3F,
		.symbol = "SFP_ETH_EXT_RESERVED_3F",
		.description = "Reserved",
	},
	{
		.value = SFP_ETH_EXT_RESERVED_3E,
		.symbol = "SFP_ETH_EXT_RESERVED_3E",
		.description = "Reserved",
	},
	{
		.value = SFP_ETH_EXT_RESERVED_3D,
		.symbol = "SFP_ETH_EXT_RESERVED_3D",
		.description = "Reserved",
	},
	{
		.value = SFP_ETH_EXT_RESERVED_3C,
		.symbol = "SFP_ETH_EXT_RESERVED_3C",
		.description = "Reserved",
	},
	{
		.value = SFP_ETH_EXT_RESERVED_3B,
		.symbol = "SFP_ETH_EXT_RESERVED_3B",
		.description = "Reserved",
	},
	{
		.value = SFP_ETH_EXT_RESERVED_3A,
		.symbol = "SFP_ETH_EXT_RESERVED_3A",
		.description = "Reserved",
	},
	{
		.value = SFP_ETH_EXT_RESERVED_39,
		.symbol = "SFP_ETH_EXT_RESERVED_39",
		.description = "Reserved",
	},
	{
		.value = SFP_ETH_EXT_RESERVED_38,
		.symbol = "SFP_ETH_EXT_RESERVED_38",
		.description = "Reserved",
	},
	{
		.value = SFP_ETH_EXT_RESERVED_37,
		.symbol = "SFP_ETH_EXT_RESERVED_37",
		.description = "Reserved",
	},
	{
		.value = SFP_ETH_EXT_RESERVED_36,
		.symbol = "SFP_ETH_EXT_RESERVED_36",
		.description = "Reserved",
	},
	{
		.value = SFP_ETH_EXT_RESERVED_35,
		.symbol = "SFP_ETH_EXT_RESERVED_35",
		.description = "Reserved",
	},
	{
		.value = SFP_ETH_EXT_RESERVED_34,
		.symbol = "SFP_ETH_EXT_RESERVED_34",
		.description = "Reserved",
	},
	{
		.value = SFP_ETH_EXT_50_100_200GAUI_AOC_HI_BER,
		.symbol = "SFP_ETH_EXT_50_100_200GAUI_AOC_HI_BER",
		.description = "50GAUI/100GAUI-2/200GAUI-4 AOC (BER <2.6e-4)",
	},
	{
		.value = SFP_ETH_EXT_50_100_200GAUI_ACC_HI_BER,
		.symbol = "SFP_ETH_EXT_50_100_200GAUI_ACC_HI_BER",
		.description = "50GAUI/100GAUI-2/200GAUI-4 ACC (BER <2.6e-4)",
	},
	{
		.value = SFP_ETH_EXT_50_100_200GAUI_AOC_LO_BER,
		.symbol = "SFP_ETH_EXT_50_100_200GAUI_AOC_LO_BER",
		.description = "50GAUI/100GAUI-2/200GAUI-4 AOC (BER <1e-6)",
	},
	{
		.value = SFP_ETH_EXT_50_100_200GAUI_ACC_LO_BER,
		.symbol = "SFP_ETH_EXT_50_100_200GAUI_ACC_LO_BER",
		.description = "50GAUI/100GAUI-2/200GAUI-4 ACC (BER <1e-6)",
	},
	{
		.value = SFP_ETH_EXT_RESERVED_2F,
		.symbol = "SFP_ETH_EXT_RESERVED_2F",
		.description = "Reserved",
	},
	{
		.value = SFP_ETH_EXT_RESERVED_2E,
		.symbol = "SFP_ETH_EXT_RESERVED_2E",
		.description = "Reserved",
	},
	{
		.value = SFP_ETH_EXT_RESERVED_2D,
		.symbol = "SFP_ETH_EXT_RESERVED_2D",
		.description = "Reserved",
	},
	{
		.value = SFP_ETH_EXT_RESERVED_2C,
		.symbol = "SFP_ETH_EXT_RESERVED_2C",
		.description = "Reserved",
	},
	{
		.value = SFP_ETH_EXT_RESERVED_2B,
		.symbol = "SFP_ETH_EXT_RESERVED_2B",
		.description = "Reserved",
	},
	{
		.value = SFP_ETH_EXT_RESERVED_2A,
		.symbol = "SFP_ETH_EXT_RESERVED_2A",
		.description = "Reserved",
	},
	{
		.value = SFP_ETH_EXT_RESERVED_29,
		.symbol = "SFP_ETH_EXT_RESERVED_29",
		.description = "Reserved",
	},
	{
		.value = SFP_ETH_EXT_RESERVED_28,
		.symbol = "SFP_ETH_EXT_RESERVED_28",
		.description = "Reserved",
	},
	{
		.value = SFP_ETH_EXT_100G_LR,
		.symbol = "SFP_ETH_EXT_100G_LR",
		.description = "100G-LR",
	},
	{
		.value = SFP_ETH_EXT_100G_FR,
		.symbol = "SFP_ETH_EXT_100G_FR",
		.description = "100G-FR",
	},
	{
		.value = SFP_ETH_EXT_100GBASE_DR,
		.symbol = "SFP_ETH_EXT_100GBASE_DR",
		.description = "100GBASE-DR",
	},
	{
		.value = SFP_ETH_EXT_4WDM_40_MSA,
		.symbol = "SFP_ETH_EXT_4WDM_40_MSA",
		.description = "4WDM-40 MSA",
	},
	{
		.value = SFP_ETH_EXT_4WDM_20_MSA,
		.symbol = "SFP_ETH_EXT_4WDM_20_MSA",
		.description = "4WDM-20 MSA",
	},
	{
		.value = SFP_ETH_EXT_4WDM_10_MSA,
		.symbol = "SFP_ETH_EXT_4WDM_10_MSA",
		.description = "4WDM-10 MSA",
	},
	{
		.value = SFP_ETH_EXT_100G_PAM4_BIDI,
		.symbol = "SFP_ETH_EXT_100G_PAM4_BIDI",
		.description = "100G PAM4 BiDi",
	},
	{
		.value = SFP_ETH_EXT_100G_SWDM4,
		.symbol = "SFP_ETH_EXT_100G_SWDM4",
		.description = "100G SWDM4",
	},
	{
		.value = SFP_ETH_EXT_40G_SWDM4,
		.symbol = "SFP_ETH_EXT_40G_SWDM4",
		.description = "40G SWDM4",
	},
	{
		.value = SFP_ETH_EXT_2_5GBASE_T,
		.symbol = "SFP_ETH_EXT_2_5GBASE_T",
		.description = "2.5GBASE-T",
	},
	{
		.value = SFP_ETH_EXT_5GBASE_T,
		.symbol = "SFP_ETH_EXT_5GBASE_T",
		.description = "5GBASE-T",
	},
	{
		.value = SFP_ETH_EXT_10GBASE_T_SR,
		.symbol = "SFP_ETH_EXT_10GBASE_T_SR",
		.description = "10GBASE-T Short Reach",
	},
	{
		.value = SFP_ETH_EXT_100G_1550NM_WDM,
		.symbol = "SFP_ETH_EXT_100G_1550NM_WDM",
		.description = "100G 1550nm WDM",
	},
	{
		.value = SFP_ETH_EXT_100GE_DWDM2,
		.symbol = "SFP_ETH_EXT_100GE_DWDM2",
		.description = "100GE-DWDM2",
	},
	{
		.value = SFP_ETH_EXT_100G_25GAUI_C2M_ACC,
		.symbol = "SFP_ETH_EXT_100G_25GAUI_C2M_ACC",
		.description = "100G ACC or 25GAUI C2M ACC",
	},
	{
		.value = SFP_ETH_EXT_100G_25GAUI_C2M_AOC,
		.symbol = "SFP_ETH_EXT_100G_25GAUI_C2M_AOC",
		.description = "100G AOC or 25GAUI C2M AOC",
	},
	{
		.value = SFP_ETH_EXT_100G_CLR4,
		.symbol = "SFP_ETH_EXT_100G_CLR4",
		.description = "100G CLR4",
	},
	{
		.value = SFP_ETH_EXT_10GBASE_T_SFI,
		.symbol = "SFP_ETH_EXT_10GBASE_T_SFI",
		.description = "10GBASE-T with SFI electrical interface",
	},
	{
		.value = SFP_ETH_EXT_G959_1_P1L1_2D2,
		.symbol = "SFP_ETH_EXT_G959_1_P1L1_2D2",
		.description = "G959.1 profile P1L1-2D2",
	},
	{
		.value = SFP_ETH_EXT_G959_1_P1S1_2D2,
		.symbol = "SFP_ETH_EXT_G959_1_P1S1_2D2",
		.description = "G959.1 profile P1S1-2D2",
	},
	{
		.value = SFP_ETH_EXT_G959_1_P1I1_2D1,
		.symbol = "SFP_ETH_EXT_G959_1_P1I1_2D1",
		.description = "G959.1 profile P1I1-2D1",
	},
	{
		.value = SFP_ETH_EXT_40G_PSM4,
		.symbol = "SFP_ETH_EXT_40G_PSM4",
		.description = "40G PSM4 Parallel SMF",
	},
	{
		.value = SFP_ETH_EXT_4X_10GBASE_SR,
		.symbol = "SFP_ETH_EXT_4X_10GBASE_SR",
		.description = "4 x 10GBASE-SR",
	},
	{
		.value = SFP_ETH_EXT_40GBASE_ER4,
		.symbol = "SFP_ETH_EXT_40GBASE_ER4",
		.description = "40GBASE-ER4",
	},
	{
		.value = SFP_ETH_EXT_RESERVED_0F,
		.symbol = "SFP_ETH_EXT_RESERVED_0F",
		.description = "Reserved",
	},
	{
		.value = SFP_ETH_EXT_RESERVED_0E,
		.symbol = "SFP_ETH_EXT_RESERVED_0E",
		.description = "Reserved",
	},
	{
		.value = SFP_ETH_EXT_CA_25G_N,
		.symbol = "SFP_ETH_EXT_CA_25G_N",
		.description = "25GBASE-CR CA-25G-N",
	},
	{
		.value = SFP_ETH_EXT_CA_25G_S,
		.symbol = "SFP_ETH_EXT_CA_25G_S",
		.description = "25GBASE-CR CA-25G-S",
	},
	{
		.value = SFP_ETH_EXT_CA_L,
		.symbol = "SFP_ETH_EXT_CA_L",
		.description = "100GBASE-CR4 or 25GBASE-CR CA-L",
	},
	{
		.value = SFP_ETH_EXT_RESERVED_0A,
		.symbol = "SFP_ETH_EXT_RESERVED_0A",
		.description = "Reserved",
	},
	{
		.value = SFP_ETH_EXT_OBSOLETE,
		.symbol = "SFP_ETH_EXT_OBSOLETE",
		.description = "Obsolete",
	},
	{
		.value = SFP_ETH_EXT_100G_25GAUI_C2M_ACC_1,
		.symbol = "SFP_ETH_EXT_100G_25GAUI_C2M_ACC_1",
		.description = "100G ACC (Active Copper Cable",
	},
	{
		.value = SFP_ETH_EXT_100G_PSM4_P_SMF,
		.symbol = "SFP_ETH_EXT_100G_PSM4_P_SMF",
		.description = "100G PSM4 Parallel SMF",
	},
	{
		.value = SFP_ETH_EXT_100G_CWDM4,
		.symbol = "SFP_ETH_EXT_100G_CWDM4",
		.description = "100G CWDM4",
	},
	{
		.value = SFP_ETH_EXT_100GBASE_SR10,
		.symbol = "SFP_ETH_EXT_100GBASE_SR10",
		.description = "100GBASE-SR10",
	},
	{
		.value = SFP_ETH_EXT_100GBASE_ER4_25GBASE_ER,
		.symbol = "SFP_ETH_EXT_100GBASE_ER4_25GBASE_ER",
		.description = "100GBASE-ER4 or 25GBASE-ER",
	},
	{
		.value = SFP_ETH_EXT_100GBASE_LR4_25GBASE_LR,
		.symbol = "SFP_ETH_EXT_100GBASE_LR4_25GBASE_LR",
		.description = "100GBASE-LR4 or 25GBASE-LR",
	},
	{
		.value = SFP_ETH_EXT_100GBASE_SR4_25GBASE_SR,
		.symbol = "SFP_ETH_EXT_100GBASE_SR4_25GBASE_SR",
		.description = "100GBASE-SR4 or 25GBASE-SR",
	},
	{
		.value = SFP_ETH_EXT_100G_25GAUI_C2M_AOC_1,
		.symbol = "SFP_ETH_EXT_100G_25GAUI_C2M_AOC_1",
		.description = "100G AOC (Active Optical Cable",
	},
	{
		.value = SFP_ETH_EXT_UNSPECIFIED,
		.symbol = "SFP_ETH_EXT_UNSPECIFIED",
		.description = "Unspecified",
	},
	{0}
};
const struct sfp_enum_metadata *sfp_eth_ext_table = sfp_eth_ext_table_;

const char *
ifconfig_sfp_eth_ext_symbol(enum sfp_eth_ext v)
{
	const struct sfp_enum_metadata *metadata;

	if ((metadata = find_metadata(sfp_eth_ext_table, v)) == NULL)
		return (NULL);
	return (metadata->symbol);
}

const char *
ifconfig_sfp_eth_ext_description(enum sfp_eth_ext v)
{
	const struct sfp_enum_metadata *metadata;

	if ((metadata = find_metadata(sfp_eth_ext_table, v)) == NULL)
		return (NULL);
	return (metadata->description);
}

/*
 * SFF-8636 Rev. 2.9 table 6.3: Revision compliance
 */

static const struct sfp_enum_metadata sfp_rev_table_[] = {
	{
		.value = SFP_REV_SFF_8436_REV_LE_4_8,
		.symbol = "SFP_REV_SFF_8436_REV_LE_4_8",
		.description = "SFF-8436 rev <=4.8",
	},
	{
		.value = SFP_REV_SFF_8436_REV_LE_4_8_ALT,
		.symbol = "SFP_REV_SFF_8436_REV_LE_4_8_ALT",
		.description = "SFF-8436 rev <=4.8",
	},
	{
		.value = SFP_REV_SFF_8636_REV_LE_1_3,
		.symbol = "SFP_REV_SFF_8636_REV_LE_1_3",
		.description = "SFF-8636 rev <=1.3",
	},
	{
		.value = SFP_REV_SFF_8636_REV_LE_1_4,
		.symbol = "SFP_REV_SFF_8636_REV_LE_1_4",
		.description = "SFF-8636 rev <=1.4",
	},
	{
		.value = SFP_REV_SFF_8636_REV_LE_1_5,
		.symbol = "SFP_REV_SFF_8636_REV_LE_1_5",
		.description = "SFF-8636 rev <=1.5",
	},
	{
		.value = SFP_REV_SFF_8636_REV_LE_2_0,
		.symbol = "SFP_REV_SFF_8636_REV_LE_2_0",
		.description = "SFF-8636 rev <=2.0",
	},
	{
		.value = SFP_REV_SFF_8636_REV_LE_2_7,
		.symbol = "SFP_REV_SFF_8636_REV_LE_2_7",
		.description = "SFF-8636 rev <=2.7",
	},
	{
		.value = SFP_REV_SFF_8363_REV_GE_2_8,
		.symbol = "SFP_REV_SFF_8363_REV_GE_2_8",
		.description = "SFF-8636 rev >=2.8",
	},
	{
		.value = SFP_REV_UNSPECIFIED,
		.symbol = "SFP_REV_UNSPECIFIED",
		.description = "Unspecified",
	},
	{0}
};
const struct sfp_enum_metadata *sfp_rev_table = sfp_rev_table_;

const char *
ifconfig_sfp_rev_symbol(enum sfp_rev v)
{
	const struct sfp_enum_metadata *metadata;

	if ((metadata = find_metadata(sfp_rev_table, v)) == NULL)
		return (NULL);
	return (metadata->symbol);
}

const char *
ifconfig_sfp_rev_description(enum sfp_rev v)
{
	const struct sfp_enum_metadata *metadata;

	if ((metadata = find_metadata(sfp_rev_table, v)) == NULL)
		return (NULL);
	return (metadata->description);
}

