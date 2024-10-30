/*-
 * SPDX-License-Identifier: BSD-2-Clause
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
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _NET_IF_STRINGS_H_
#define _NET_IF_STRINGS_H_

#define	IFCAP_RXCSUM_NAME		"RXCSUM"
#define	IFCAP_TXCSUM_NAME		"TXCSUM"
#define	IFCAP_NETCONS_NAME		"NETCONS"
#define	IFCAP_VLAN_MTU_NAME		"VLAN_MTU"
#define	IFCAP_VLAN_HWTAGGING_NAME 	"VLAN_HWTAGGING"
#define	IFCAP_JUMBO_MTU_NAME		"JUMBO_MTU"
#define	IFCAP_POLLING_NAME		"POLLING"
#define	IFCAP_VLAN_HWCSUM_NAME		"VLAN_HWCSUM"
#define	IFCAP_TSO4_NAME			"TSO4"
#define	IFCAP_TSO6_NAME			"TSO6"
#define	IFCAP_LRO_NAME			"LRO"
#define	IFCAP_WOL_UCAST_NAME		"WOL_UCAST"
#define	IFCAP_WOL_MCAST_NAME		"WOL_MCAST"
#define	IFCAP_WOL_MAGIC_NAME		"WOL_MAGIC"
#define	IFCAP_TOE4_NAME			"TOE4"
#define	IFCAP_TOE6_NAME			"TOE6"
#define	IFCAP_VLAN_HWFILTER_NAME 	"VLAN_HWFILTER"
#define	IFCAP_NV_NAME			"NV"
#define	IFCAP_VLAN_HWTSO_NAME		"VLAN_HWTSO"
#define	IFCAP_LINKSTATE_NAME		"LINKSTATE"
#define	IFCAP_NETMAP_NAME		"NETMAP"
#define	IFCAP_RXCSUM_IPV6_NAME		"RXCSUM_IPV6"
#define	IFCAP_TXCSUM_IPV6_NAME		"TXCSUM_IPV6"
#define	IFCAP_HWSTATS_NAME		"HWSTATS"
#define	IFCAP_TXRTLMT_NAME		"TXRTLMT"
#define	IFCAP_HWRXTSTMP_NAME		"HWRXTSTMP"
#define	IFCAP_MEXTPG_NAME		"MEXTPG"
#define	IFCAP_TXTLS4_NAME		"TXTLS4"
#define	IFCAP_TXTLS6_NAME		"TXTLS6"
#define	IFCAP_VXLAN_HWCSUM_NAME		"VXLAN_HWCSUM"
#define	IFCAP_VXLAN_HWTSO_NAME		"VXLAN_HWTSO"
#define	IFCAP_TXTLS_RTLMT_NAME		"TXTLS_RTLMT"
#define	IFCAP_RXTLS4_NAME		"RXTLS4"
#define	IFCAP_RXTLS6_NAME		"RXTLS6"
#define	IFCAP_IPSEC_OFFLOAD_NAME	"IPSEC"

#define	IFCAP2_RXTLS4_NAME	IFCAP_RXTLS4_NAME
#define	IFCAP2_RXTLS6_NAME	IFCAP_RXTLS6_NAME
#define	IFCAP2_IPSEC_OFFLOAD_NAME IFCAP_IPSEC_OFFLOAD_NAME

static const char *ifcap_bit_names[] = {
	IFCAP_RXCSUM_NAME,
	IFCAP_TXCSUM_NAME,
	IFCAP_NETCONS_NAME,
	IFCAP_VLAN_MTU_NAME,
	IFCAP_VLAN_HWTAGGING_NAME,
	IFCAP_JUMBO_MTU_NAME,
	IFCAP_POLLING_NAME,
	IFCAP_VLAN_HWCSUM_NAME,
	IFCAP_TSO4_NAME,
	IFCAP_TSO6_NAME,
	IFCAP_LRO_NAME,
	IFCAP_WOL_UCAST_NAME,
	IFCAP_WOL_MCAST_NAME,
	IFCAP_WOL_MAGIC_NAME,
	IFCAP_TOE4_NAME,
	IFCAP_TOE6_NAME,
	IFCAP_VLAN_HWFILTER_NAME,
	IFCAP_NV_NAME,
	IFCAP_VLAN_HWTSO_NAME,
	IFCAP_LINKSTATE_NAME,
	IFCAP_NETMAP_NAME,
	IFCAP_RXCSUM_IPV6_NAME,
	IFCAP_TXCSUM_IPV6_NAME,
	IFCAP_HWSTATS_NAME,
	IFCAP_TXRTLMT_NAME,
	IFCAP_HWRXTSTMP_NAME,
	IFCAP_MEXTPG_NAME,
	IFCAP_TXTLS4_NAME,
	IFCAP_TXTLS6_NAME,
	IFCAP_VXLAN_HWCSUM_NAME,
	IFCAP_VXLAN_HWTSO_NAME,
	IFCAP_TXTLS_RTLMT_NAME,
	IFCAP_RXTLS4_NAME,
	IFCAP_RXTLS6_NAME,
	IFCAP_IPSEC_OFFLOAD_NAME,
};

#ifdef IFCAP_B_SIZE
_Static_assert(sizeof(ifcap_bit_names) >= IFCAP_B_SIZE * sizeof(char *),
    "ifcap bit names missing from ifcap_bit_names");
#endif

#endif
