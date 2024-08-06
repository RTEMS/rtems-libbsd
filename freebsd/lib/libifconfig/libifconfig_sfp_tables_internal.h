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

#include <libifconfig_sfp.h>
#include <libifconfig_sfp_tables.h>

struct sfp_enum_metadata;
const struct sfp_enum_metadata *find_metadata(const struct sfp_enum_metadata *,
    int);

extern const struct sfp_enum_metadata *sfp_id_table;
extern const struct sfp_enum_metadata *sfp_conn_table;
extern const struct sfp_enum_metadata *sfp_eth_10g_table;
extern const struct sfp_enum_metadata *sfp_eth_table;
extern const struct sfp_enum_metadata *sfp_fc_len_table;
extern const struct sfp_enum_metadata *sfp_cab_tech_table;
extern const struct sfp_enum_metadata *sfp_fc_media_table;
extern const struct sfp_enum_metadata *sfp_fc_speed_table;
extern const struct sfp_enum_metadata *sfp_eth_1040g_table;
extern const struct sfp_enum_metadata *sfp_eth_ext_table;
extern const struct sfp_enum_metadata *sfp_rev_table;

static inline void
get_sfp_info_strings(const struct ifconfig_sfp_info *sfp,
    struct ifconfig_sfp_info_strings *strings)
{
	strings->sfp_id = ifconfig_sfp_id_description(sfp->sfp_id);
	strings->sfp_conn = ifconfig_sfp_conn_description(sfp->sfp_conn);
	strings->sfp_eth_10g = ifconfig_sfp_eth_10g_description(sfp->sfp_eth_10g);
	strings->sfp_eth = ifconfig_sfp_eth_description(sfp->sfp_eth);
	strings->sfp_fc_len = ifconfig_sfp_fc_len_description(sfp->sfp_fc_len);
	strings->sfp_cab_tech = ifconfig_sfp_cab_tech_description(sfp->sfp_cab_tech);
	strings->sfp_fc_media = ifconfig_sfp_fc_media_description(sfp->sfp_fc_media);
	strings->sfp_fc_speed = ifconfig_sfp_fc_speed_description(sfp->sfp_fc_speed);
	strings->sfp_eth_1040g = ifconfig_sfp_eth_1040g_description(sfp->sfp_eth_1040g);
	strings->sfp_eth_ext = ifconfig_sfp_eth_ext_description(sfp->sfp_eth_ext);
	strings->sfp_rev = ifconfig_sfp_rev_description(sfp->sfp_rev);
}
