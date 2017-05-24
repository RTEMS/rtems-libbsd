#include <machine/rtems-bsd-kernel-space.h>
#include <rtems/bsd/local/opt_dpaa.h>

/*
 * Copyright (c) 2016 embedded brains GmbH
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

#include <machine/rtems-bsd-kernel-space.h>

#include <sys/param.h>
#include <sys/lock.h>
#include <sys/time.h>
#include <sys/queue.h>
#include <sys/mutex.h>
#include <sys/kernel.h>
#include <sys/malloc.h>

#include <libfdt.h>

#include <rtems/bsd.h>

#include <bsp/fdt.h>

#include <linux/of_mdio.h>

#define	MDIO_LOCK()	mtx_lock(&mdio.mutex)
#define	MDIO_UNLOCK()	mtx_unlock(&mdio.mutex)

static struct {
	SLIST_HEAD(, mdio_bus) instances;
	struct mtx mutex;
} mdio = {
	.instances = SLIST_HEAD_INITIALIZER(mdio.instances)
};

MTX_SYSINIT(mdio_mutex, &mdio.mutex, "MDIO", MTX_DEF);

int
phy_read(struct phy_device *phy_dev, int reg)
{
	struct mdio_bus *mdio_dev;
	int val;

	mdio_dev = phy_dev->mdio.bus;
	MDIO_LOCK();
	val = (*mdio_dev->read)(mdio_dev, phy_dev->mdio.addr,
	    reg | phy_dev->mdio.is_c45);
	MDIO_UNLOCK();
	return (val);
}

int
phy_write(struct phy_device *phy_dev, int reg, int val)
{
	struct mdio_bus *mdio_dev;
	int err;

	mdio_dev = phy_dev->mdio.bus;
	MDIO_LOCK();
	err = (*mdio_dev->write)(mdio_dev, phy_dev->mdio.addr,
	    reg | phy_dev->mdio.is_c45, val);
	MDIO_UNLOCK();
	return (err);
}

static uint64_t
fdt_get_address(const void *fdt, int node)
{
	uint64_t addr;
	int nodes[16];
	size_t i;
	int ac;

	i = 0;
	do {
		nodes[i] = node;
		++i;
		node = fdt_parent_offset(fdt, node);
	} while (node >= 0 && i < nitems(nodes));

	if (node >= 0) {
		return (0);
	}

	ac = 1;
	addr = 0;
	while (i > 0) {
		const fdt32_t *p;
		int len;

		p = fdt_getprop(fdt, nodes[i - 1], "reg", &len);
		if (p != NULL) {
			if (ac == 1 && len >= 4) {
				addr += fdt32_to_cpu(p[0]);
			} else if (ac == 2 && len >= 8) {
				addr += fdt32_to_cpu(p[1]);
				addr += (uint64_t)fdt32_to_cpu(p[0]) << 32;
			} else {
				return (0);
			}
		}

		p = fdt_getprop(fdt, nodes[i - 1], "#address-cells", &len);
		if (p != NULL) {
			if (len != 4) {
				return (0);
			}
			ac = (int)fdt32_to_cpu(p[0]);
			if (ac != 1 && ac != 2) {
				return (0);
			}
		}

		--i;
	}

	return (addr);
}

struct fman_mdio_regs {
	uint32_t reserved[12];
	uint32_t mdio_cfg;
	uint32_t mdio_ctrl;
	uint32_t mdio_data;
	uint32_t mdio_addr;
};

#define	MDIO_CFG_BSY		(1U << 31)
#define	MDIO_CFG_ENC45		(1U << 6)
#define	MDIO_CFG_RD_ERR		(1U << 1)

#define	MDIO_CTRL_READ		(1U << 15)
#define	MDIO_CTRL_REG_ADDR(x)	((x) & 0x1fU)
#define	MDIO_CTRL_PHY_ADDR(x)	(((x) & 0x1fU) << 5)

struct fman_mdio_bus {
	struct mdio_bus base;
	volatile struct fman_mdio_regs *regs;
};

static int
fman_mdio_wait(volatile struct fman_mdio_regs *regs)
{
	struct bintime start;

	rtems_bsd_binuptime(&start);

	while ((regs->mdio_cfg & MDIO_CFG_BSY) != 0) {
		struct bintime now;

		rtems_bsd_binuptime(&now);
		if (bttosbt(now) - bttosbt(start) > 100 * SBT_1US) {
			break;
		}
	}

	/* Check again, to take thread pre-emption into account */
	if ((regs->mdio_cfg & MDIO_CFG_BSY) != 0) {
		return (EIO);
	}

	return (0);
}

static int
fman_mdio_setup(volatile struct fman_mdio_regs *regs, int addr, int reg,
    uint32_t *mdio_ctrl_p)
{
	uint32_t mdio_cfg;
	uint32_t mdio_ctrl;
	uint32_t reg_addr;
	int err;

	err = fman_mdio_wait(regs);
	if (err != 0) {
		return (err);
	}

	mdio_cfg = regs->mdio_cfg;
	if ((reg & MII_ADDR_C45) != 0) {
		reg_addr = (uint32_t)(reg >> 16);
		mdio_cfg |= MDIO_CFG_ENC45;
	} else {
		reg_addr = (uint32_t)reg;
		mdio_cfg &= ~MDIO_CFG_ENC45;
	}
	regs->mdio_cfg = mdio_cfg;

	mdio_ctrl = MDIO_CTRL_PHY_ADDR(addr) | MDIO_CTRL_REG_ADDR(reg_addr);
	regs->mdio_ctrl = mdio_ctrl;

	if ((reg & MII_ADDR_C45) != 0) {
		regs->mdio_addr = (uint32_t)(reg & 0xffff);
		err = fman_mdio_wait(regs);
		if (err != 0) {
			return (err);
		}
	}

	*mdio_ctrl_p = mdio_ctrl;
	return (0);
}

static int
fman_mdio_read(struct mdio_bus *base, int addr, int reg)
{
	struct fman_mdio_bus *fm;
	volatile struct fman_mdio_regs *regs;
	uint32_t mdio_ctrl;
	int val;
	int err;

	fm = (struct fman_mdio_bus *)base;
	regs = fm->regs;

	err = fman_mdio_setup(regs, addr, reg, &mdio_ctrl);
	if (err != 0) {
		return (-1);
	}

	mdio_ctrl |= MDIO_CTRL_READ;
	regs->mdio_ctrl = mdio_ctrl;

	err = fman_mdio_wait(regs);
	if (err == 0 && (regs->mdio_cfg & MDIO_CFG_RD_ERR) == 0) {
		val = (int)(regs->mdio_data & 0xffff);
	} else {
		val = -1;
	}

	return (val);
}

static int
fman_mdio_write(struct mdio_bus *base, int addr, int reg, int val)
{
	struct fman_mdio_bus *fm;
	volatile struct fman_mdio_regs *regs;
	uint32_t mdio_ctrl;
	int err;

	fm = (struct fman_mdio_bus *)base;
	regs = fm->regs;

	err = fman_mdio_setup(regs, addr, reg, &mdio_ctrl);
	if (err != 0) {
		return (0);
	}

	regs->mdio_data = (uint32_t)(val & 0xffff);
	fman_mdio_wait(regs);
	return (0);
}

static struct mdio_bus *
create_fman_mdio(const void *fdt, int mdio_node)
{
	struct fman_mdio_bus *fm = NULL;

	fm = malloc(sizeof(*fm), M_TEMP, M_WAITOK | M_ZERO);
	if (fm == NULL) {
		return (NULL);
	}

	fm->base.read = fman_mdio_read;
	fm->base.write = fman_mdio_write;
	fm->base.node = mdio_node;
	fm->regs = (volatile struct fman_mdio_regs *)(uintptr_t)
	    fdt_get_address(fdt, mdio_node);

	return (&fm->base);
}

static struct mdio_bus *
create_mdio_bus(const void *fdt, int mdio_node)
{

	if (fdt_node_check_compatible(fdt, mdio_node,
	    "fsl,fman-memac-mdio") == 0 ||
	    fdt_node_check_compatible(fdt, mdio_node,
	    "fsl,fman-xmdio") == 0) {
		return (create_fman_mdio(fdt, mdio_node));
	} else {
		return (NULL);
	}
}

static int
find_mdio_bus(const void *fdt, int mdio_node,
    struct phy_device *phy_dev)
{
	struct mdio_bus *mdio_bus = NULL;

	SLIST_FOREACH(mdio_bus, &mdio.instances, next) {
		if (mdio_bus->node == mdio_node) {
			break;
		}
	}

	if (mdio_bus == NULL) {
		mdio_bus = create_mdio_bus(fdt, mdio_node);
	}

	if (mdio_bus == NULL) {
		return (ENXIO);
	}

	phy_dev->mdio.bus = mdio_bus;
	return (0);
}

#define	MDIO_C45_DEVID1 2
#define	MDIO_C45_DEVID2 3
#define	MDIO_C45_DEVINPKG1 5
#define	MDIO_C45_DEVINPKG2 6

struct phy_c45_device_ids {
	uint32_t devices_in_package;
	uint32_t device_ids[8];
};

static int
c45_get_devices_in_package(struct phy_device *phy_dev, int dev, uint32_t *dip)
{
	int val;
	int reg;

	reg = (dev << 16) | MDIO_C45_DEVINPKG2;
	val = phy_read(phy_dev, reg);
	if (val < 0) {
		return (-EIO);
	}
	*dip = (uint32_t)((val & 0xffff) << 16);

	reg = (dev << 16) | MDIO_C45_DEVINPKG1;
	val = phy_read(phy_dev, reg);
	if (val < 0) {
		return (-EIO);
	}
	*dip |= (uint32_t)(val & 0xffff);
	return (0);
}

static int
c45_get_id(struct phy_device *phy_dev, int dev, uint32_t *id)
{
	int val;
	int reg;

	reg = (dev << 16) | MDIO_C45_DEVID1;
	val = phy_read(phy_dev, reg);
	if (val < 0) {
		return (-EIO);
	}
	*id = (uint32_t)((val & 0xffff) << 16);

	reg = (dev << 16) | MDIO_C45_DEVID2;
	val = phy_read(phy_dev, reg);
	if (val < 0) {
		return (-EIO);
	}
	*id |= (uint32_t)(val & 0xffff);
	return (0);
}

static bool
c45_has_no_dip(const uint32_t *dip)
{

	return ((*dip & 0x1fffffff) == 0x1fffffff);
}

static int
c45_get_ids(struct phy_device *phy_dev, struct phy_c45_device_ids *ids)
{
	int i;
	int err;

	for (i = 1; i < ARRAY_SIZE(ids->device_ids) &&
	    ids->devices_in_package == 0; ++i) {
		err = c45_get_devices_in_package(phy_dev, i,
		    &ids->devices_in_package);
		if (err != 0) {
			return (err);
		}

		if (c45_has_no_dip(&ids->devices_in_package)) {
			err = c45_get_devices_in_package(phy_dev, 0,
			    &ids->devices_in_package);
			if (err != 0) {
				return (err);
			}

			if (c45_has_no_dip(&ids->devices_in_package)) {
				return (-EIO);
			}

			break;
		}
	}

	for (i = 1; i < ARRAY_SIZE(ids->device_ids); ++i) {
		if ((ids->devices_in_package & (1U << i)) != 0) {
			err = c45_get_id(phy_dev, i, &ids->device_ids[i]);
			if (err != 0) {
				return (err);
			}
		}
	}

	return (0);
}

static struct phy_device *
phy_obtain(const void *fdt, int is_c45, int mdio_node, int addr)
{
	struct phy_device *phy_dev;
	int err;

	phy_dev = malloc(sizeof(*phy_dev), M_TEMP, M_WAITOK | M_ZERO);
	if (phy_dev == NULL) {
		return (NULL);
	}

	phy_dev->mdio.addr = addr;
	phy_dev->mdio.is_c45 = is_c45;
	MDIO_LOCK();
	err = find_mdio_bus(fdt, mdio_node, phy_dev);
	MDIO_UNLOCK();

	if (err != 0) {
		free(phy_dev, M_TEMP);
		return (NULL);
	}

	if (is_c45) {
		struct phy_c45_device_ids ids = { 0 };

		c45_get_ids(phy_dev, &ids);
	}

	return (phy_dev);
}

struct phy_device *
of_phy_find_device(struct device_node *dn)
{
	const void *fdt;
	const fdt32_t *addr;
	int len;
	int is_c45;
	int mdio_node;

	fdt = bsp_fdt_get();

	addr = fdt_getprop(fdt, dn->offset, "reg", &len);
	if (addr == NULL || len != sizeof(*addr)) {
		return (NULL);
	}

	if (of_device_is_compatible(dn, "ethernet-phy-ieee802.3-c45")) {
		is_c45 = MII_ADDR_C45;
	} else {
		is_c45 = 0;
	}

	mdio_node = fdt_parent_offset(fdt, dn->offset);
	if (mdio_node < 0) {
		return (NULL);
	}

	return (phy_obtain(fdt, is_c45, mdio_node, (int)fdt32_to_cpu(*addr)));
}
