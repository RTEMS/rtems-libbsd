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

MTX_SYSINIT(mdio_mutex, &mdio.mutex, "FDT MDIO", MTX_DEF);

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
fman_mdio_read(struct mdio_bus *base, int phy, int reg)
{
	struct fman_mdio_bus *fm;
	volatile struct fman_mdio_regs *regs;
	int val;
	int err;

	fm = (struct fman_mdio_bus *)base;
	regs = fm->regs;

	MDIO_LOCK();

	err = fman_mdio_wait(regs);
	if (err == 0) {
		uint32_t mdio_cfg;
		uint32_t mdio_ctrl;

		mdio_cfg = regs->mdio_cfg;
		mdio_cfg &= ~MDIO_CFG_ENC45;
		regs->mdio_cfg = mdio_cfg;

		mdio_ctrl = MDIO_CTRL_PHY_ADDR(phy) | MDIO_CTRL_REG_ADDR(reg);
		regs->mdio_ctrl = mdio_ctrl;
		mdio_ctrl |= MDIO_CTRL_READ;
		regs->mdio_ctrl = mdio_ctrl;

		err = fman_mdio_wait(regs);
		if (err == 0 && (regs->mdio_cfg & MDIO_CFG_RD_ERR) == 0) {
			val = (int)(regs->mdio_data & 0xffff);
		} else {
			val = 0xffff;
		}
	} else {
		val = 0xffff;
	}

	MDIO_UNLOCK();

	return (val);
}

static int
fman_mdio_write(struct mdio_bus *base, int phy, int reg, int val)
{
	struct fman_mdio_bus *fm;
	volatile struct fman_mdio_regs *regs;
	int err;

	fm = (struct fman_mdio_bus *)base;
	regs = fm->regs;

	MDIO_LOCK();

	err = fman_mdio_wait(regs);
	if (err == 0) {
		uint32_t mdio_cfg;
		uint32_t mdio_ctrl;

		mdio_cfg = regs->mdio_cfg;
		mdio_cfg &= ~MDIO_CFG_ENC45;
		regs->mdio_cfg = mdio_cfg;

		mdio_ctrl = MDIO_CTRL_PHY_ADDR(phy) | MDIO_CTRL_REG_ADDR(reg);
		regs->mdio_ctrl = mdio_ctrl;

		regs->mdio_data = (uint32_t)(val & 0xffff);

		fman_mdio_wait(regs);
	}

	MDIO_UNLOCK();

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

static struct phy_device *
phy_obtain(const void *fdt, int mdio_node, int phy)
{
	struct phy_device *phy_dev;
	int err;

	phy_dev = malloc(sizeof(*phy_dev), M_TEMP, M_WAITOK | M_ZERO);
	if (phy_dev == NULL) {
		return (NULL);
	}

	phy_dev->mdio.addr = phy;
	MDIO_LOCK();
	err = find_mdio_bus(fdt, mdio_node, phy_dev);
	MDIO_UNLOCK();

	if (err != 0) {
		free(phy_dev, M_TEMP);
		return (NULL);
	}

	return (phy_dev);
}

struct phy_device *
of_phy_find_device(struct device_node *dn)
{
	const void *fdt;
	const fdt32_t *phy;
	int len;
	int mdio_node;

	fdt = bsp_fdt_get();

	phy = fdt_getprop(fdt, dn->offset, "reg", &len);
	if (phy == NULL || len != sizeof(*phy)) {
		return (NULL);
	}

	mdio_node = fdt_parent_offset(fdt, dn->offset);
	if (mdio_node < 0) {
		return (NULL);
	}

	return (phy_obtain(fdt, mdio_node, (int)fdt32_to_cpu(*phy)));
}
