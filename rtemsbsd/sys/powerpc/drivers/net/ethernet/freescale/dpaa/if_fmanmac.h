/*
 * Copyright (c) 2015 embedded brains GmbH
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

#ifndef _IF_FMANMAC_H
#define	_IF_FMANMAC_H

#include <sys/param.h>
#include <sys/lock.h>
#include <sys/bus.h>
#include <sys/callout.h>
#include <sys/mutex.h>
#include <sys/queue.h>
#include <sys/socket.h>

#include <net/if.h>
#include <net/if_media.h>
#include <net/if_var.h>

#include <dev/mii/mii.h>
#include <dev/mii/miivar.h>

#include <linux/netdevice.h>

#include <fdt_phy.h>

#include "mac.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct fman_mac_softc {
	struct mac_device	mac_dev;
	struct fdt_phy_device	*phy_dev;
	device_t		miibus;
	struct mii_data		*mii_softc;
	struct ifnet		*ifp;
	int			if_flags;
	struct mtx		mtx;
	uma_zone_t		sgt_zone;
	struct callout		fman_mac_callout;
	char			name[8];
};

int fman_mac_dev_attach(device_t dev);

int fman_mac_dev_detach(device_t dev);

int fman_mac_miibus_read_reg(device_t dev, int phy, int reg);

int fman_mac_miibus_write_reg(device_t dev, int phy, int reg, int val);

void fman_mac_miibus_statchg(device_t dev);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _IF_FMANMAC_H */
