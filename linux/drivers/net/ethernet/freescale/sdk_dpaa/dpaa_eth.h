/*
 * Copyright (c) 2018 embedded brains GmbH
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

#ifndef _SDK_DPAA_DPAA_ETH_H
#define	_SDK_DPAA_DPAA_ETH_H

#include "../dpaa/dpaa_eth.h"

#define	dpa_bp dpaa_bp
#define	dpa_fq_alloc dpaa_fq_alloc
#define	dpa_fq_cbs_t dpaa_fq_cbs
#define	dpa_fq_list dpaa_fq_list
#define	dpa_fq_setup dpaa_fq_setup
#define	dpa_fq_type dpaa_fq_type
#define	dpa_get_channel dpaa_get_channel
#define	dpa_percpu_priv_s dpaa_percpu_priv
#define	dpa_priv_s dpaa_priv

enum port_type { RX, TX };

struct fqid_cell {
	uint32_t start;
	uint32_t count;
};

struct ifnet;

struct if_ml_softc {
	struct ifnet *ifp;
	struct mtx mtx;
	struct net_device net_dev;
};

void if_ml_attach(struct if_ml_softc *sc, int unit,
    const uint8_t *mac_address);

#endif /* _SDK_DPAA_DPAA_ETH_H */
