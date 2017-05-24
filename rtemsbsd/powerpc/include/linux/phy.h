/*
 * Copyright (c) 2015, 2017 embedded brains GmbH
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

#ifndef _LINUX_PHY_H
#define	_LINUX_PHY_H

#include <sys/queue.h>
#include <linux/device.h>
#include <linux/list.h>
#include <linux/netdevice.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum {
	PHY_INTERFACE_MODE_NA,
	PHY_INTERFACE_MODE_MII,
	PHY_INTERFACE_MODE_GMII,
	PHY_INTERFACE_MODE_SGMII,
	PHY_INTERFACE_MODE_TBI,
	PHY_INTERFACE_MODE_REVMII,
	PHY_INTERFACE_MODE_RMII,
	PHY_INTERFACE_MODE_RGMII,
	PHY_INTERFACE_MODE_RGMII_ID,
	PHY_INTERFACE_MODE_RGMII_RXID,
	PHY_INTERFACE_MODE_RGMII_TXID,
	PHY_INTERFACE_MODE_RTBI,
	PHY_INTERFACE_MODE_SMII,
	PHY_INTERFACE_MODE_XGMII,
	PHY_INTERFACE_MODE_MOCA,
	PHY_INTERFACE_MODE_QSGMII,
	PHY_INTERFACE_MODE_MAX
} phy_interface_t;

#define	SPEED_10 10
#define	SPEED_100 100
#define	SPEED_1000 1000
#define	SPEED_2500 2500
#define	SPEED_5000 5000
#define	SPEED_10000 10000
#define	SPEED_20000 20000
#define	SPEED_25000 25000
#define	SPEED_40000 40000
#define	SPEED_50000 50000
#define	SPEED_56000 56000
#define	SPEED_100000 100000

#define	SUPPORTED_10000baseT_Full	(1U << 0)
#define	SUPPORTED_1000baseT_Full	(1U << 1)
#define	SUPPORTED_100baseT_Full		(1U << 2)
#define	SUPPORTED_100baseT_Half		(1U << 3)
#define	SUPPORTED_10baseT_Full		(1U << 4)
#define	SUPPORTED_10baseT_Half		(1U << 5)
#define	SUPPORTED_Asym_Pause		(1U << 6)
#define	SUPPORTED_Autoneg		(1U << 7)
#define	SUPPORTED_MII			(1U << 8)
#define	SUPPORTED_Pause			(1U << 9)

#define MII_ADDR_C45 (1 << 30)

struct mdio_bus {
	int (*read)(struct mdio_bus *bus, int addr, int reg);
	int (*write)(struct mdio_bus *bus, int addr, int reg, int val);
	SLIST_ENTRY(mdio_bus) next;
	int node;
};

struct phy_device {
	struct {
		struct device dev;
		int addr;
		int is_c45;
		struct mdio_bus *bus;
	} mdio;
};

int phy_read(struct phy_device *phy_dev, int reg);

int phy_write(struct phy_device *phy_dev, int reg, int val);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LINUX_PHY_H */
