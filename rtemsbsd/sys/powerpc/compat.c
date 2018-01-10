#include <machine/rtems-bsd-kernel-space.h>
#include <rtems/bsd/local/opt_dpaa.h>

/*
 * Copyright (c) 2015, 2018 embedded brains GmbH
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

#include <linux/slab.h>

MALLOC_DEFINE(M_KMALLOC, "kmalloc", "Linux kmalloc compatibility");

#include <bsp/fdt.h>

#include <linux/of.h>

const void *
of_get_property(const struct device_node *dn, const char *name, int *len)
{
	const void *fdt = bsp_fdt_get();

	return (fdt_getprop(fdt, dn->offset, name, len));
}

int
of_property_read_u32_array(const struct device_node *dn, const char *name,
    u32 *vals, size_t n)
{
	const u32 *prop_vals;
	int len;

	prop_vals = of_get_property(dn, name, &len);
	if (prop_vals == NULL) {
		return (-EINVAL);
	}

	if (len != n * sizeof(*vals)) {
		return (-EOVERFLOW);
	}

	while (n > 0) {
		*vals = fdt32_to_cpu(*prop_vals);
		++vals;
		++prop_vals;
		--n;
	}

	return (0);
}

bool
of_device_is_available(const struct device_node *dn)
{
	const char *status;
	int len;

	status = of_get_property(dn, "status", &len);
	return (status == NULL ||
	    (len > 0 && (strcmp(status, "okay") == 0 ||
	    strcmp(status, "ok") == 0)));
}

int
of_device_is_compatible(const struct device_node *dn, const char *name)
{
	const void *fdt = bsp_fdt_get();

	return (fdt_node_check_compatible(fdt, dn->offset, name) == 0);
}

struct device_node *
of_find_node_by_path(struct device_node *dns, const char *path)
{
	const void *fdt = bsp_fdt_get();
	int node;

	memset(dns, 0, sizeof(*dns));

	node = fdt_path_offset(fdt, path);
	if (node < 0)
		return (NULL);

	dns->offset = node;
	return (dns);
}

struct device_node *
of_find_compatible_node(struct device_node *dns, const struct device_node *dn,
    const char *type, const char *compatible)
{
	const void *fdt = bsp_fdt_get();
	int node;

	(void)type;

	if (dn != NULL) {
		node = dn->offset;
	} else {
		node = 0;
	}

	memset(dns, 0, sizeof(*dns));

	while (1) {
		int err;

		node = fdt_next_node(fdt, node, NULL);
		if (node < 0)
			return (NULL);

		err = fdt_node_check_compatible(fdt, node, compatible);
		if (err == 0) {
			dns->offset = node;
			return (dns);
		}
	}
}

uint64_t
of_read_number(const uint32_t *cell, int size)
{
	uint64_t number;

	number = 0;

	while (size > 0) {
		number = (number << 32) | fdt32_to_cpu(*cell);
		++cell;
		--size;
	}

	return (number);
}

struct device_node *
of_parse_phandle(struct device_node *dns, struct device_node *dn,
    const char *phandle_name, int index)
{
	const void *fdt = bsp_fdt_get();
	const fdt32_t *phandle;
	int node;
	int len;

	phandle = fdt_getprop(fdt, dn->offset, phandle_name, &len);
	if (phandle == NULL || (len / (int) sizeof(*phandle)) <= index) {
		return (NULL);
	}

	node = fdt_node_offset_by_phandle(fdt, fdt32_to_cpu(phandle[index]));
	if (node < 0) {
		return (NULL);
	}

	dns->offset = node;
	dns->full_name = NULL;
	return (dns);
}

int
of_count_phandle_with_args(struct device_node *dn, const char *list_name,
    const char *cells_name)
{
	const void *fdt = bsp_fdt_get();
	const fdt32_t *phandle;
	int len;

	BSD_ASSERT(cells_name == NULL);

	phandle = fdt_getprop(fdt, dn->offset, list_name, &len);
	if (phandle == NULL) {
		return (-ENOENT);
	}

	return (len / (int)sizeof(*phandle));
}

#include <linux/of_address.h>
#include <linux/of_irq.h>

static int
get_cells(const void *fdt, int node, const char *name)
{
	const fdt32_t *c;
	int len;
	int val;

	do {
		c = fdt_getprop(fdt, node, name, &len);
		if (c != NULL) {
			if (len != sizeof(*c))
				return (-EINVAL);
			val = fdt32_to_cpu(*c);
			if (val <= 0 ||
			    val > sizeof(resource_size_t) / sizeof(*c))
				return (-EINVAL);
			return (val);
		}
		node = fdt_parent_offset(fdt, node);
	} while (node >= 0);

	return (-EINVAL);
}

static int
get_address_cells(const void *fdt, int node)
{

	return (get_cells(fdt, node, "#address-cells"));
}

static int
get_size_cells(const void *fdt, int node)
{

	return (get_cells(fdt, node, "#size-cells"));
}

int
of_n_addr_cells(struct device_node *dn)
{

	return (get_address_cells(bsp_fdt_get(), dn->offset));
}

int
of_n_size_cells(struct device_node *dn)
{

	return (get_size_cells(bsp_fdt_get(), dn->offset));
}

static uint64_t
translate_address(const char *fdt, int node, int ac, int sc,
    const uint32_t *addr)
{
	int pac;
	int psc;
	uint64_t taddr;

	taddr = of_read_number(addr, ac);

	node = fdt_parent_offset(fdt, node);
	if (node < 0)
		return (OF_BAD_ADDR);

	for (;;) {
		int len;
		int parent;
		const uint32_t *ranges;
		uint64_t offset;

		parent = fdt_parent_offset(fdt, node);
		if (parent < 0)
			break;

		pac = get_address_cells(fdt, parent);
		if (pac < 0)
			return (OF_BAD_ADDR);

		psc = get_size_cells(fdt, parent);
		if (psc < 0)
			return (OF_BAD_ADDR);

		ranges = fdt_getprop(fdt, node, "ranges", &len);
		if (ranges == NULL || len == 0)
			break;

		if (len != (ac + pac + sc) * 4)
			return (OF_BAD_ADDR);

		if (of_read_number(&ranges[0], ac) != 0)
			return (OF_BAD_ADDR);

		offset = of_read_number(&ranges[ac], pac);
		taddr += offset;

		node = parent;
		ac = pac;
		sc = psc;
	}

	return (taddr);
}

uint64_t
of_translate_address(struct device_node *dn, const uint32_t *addr)
{
	const void *fdt = bsp_fdt_get();
	int node;
	int ac;
	int sc;

	node = dn->offset;

	ac = get_address_cells(fdt, node);
	if (ac < 0)
		return (OF_BAD_ADDR);

	sc = get_size_cells(fdt, node);
	if (sc < 0)
		return (OF_BAD_ADDR);

	return (translate_address(fdt, node, ac, sc, addr));
}

int
of_address_to_resource(struct device_node *dn, int index,
    struct resource *res)
{
	const void *fdt = bsp_fdt_get();
	int node;
	int ac;
	int sc;
	int len;
	const fdt32_t *p;
	int i;

	memset(res, 0, sizeof(*res));
	node = dn->offset;

	ac = get_address_cells(fdt, node);
	if (ac < 0)
		return (-EINVAL);

	sc = get_size_cells(fdt, node);
	if (sc < 0)
		return (-EINVAL);

	p = fdt_getprop(fdt, node, "reg", &len);
	if (p == NULL)
		return (-EINVAL);

	len /= sizeof(*p);
	i = index * (ac + sc);
	if (i + ac + sc > len)
		return (-EINVAL);

	res->start = translate_address(fdt, node, ac, sc, &p[i]);
	res->end = of_read_number(&p[i + ac], sc);
	res->end += res->start;
	return (0);
}

static int
get_interrupt_cells(const void *fdt, int node)
{
	do {
		const fdt32_t *p;
		int len;

		p = fdt_getprop(fdt, node, "interrupt-parent", &len);
		if (p != NULL) {
			if (len != sizeof(*p))
				return (-EINVAL);

			node = fdt_node_offset_by_phandle(fdt,
			    fdt32_to_cpu(*p));
		} else {
			node = fdt_parent_offset(fdt, node);
		}

		p = fdt_getprop(fdt, node, "#interrupt-cells", &len);
		if (p != NULL) {
			if (len != sizeof(*p))
				return (-EINVAL);

			return ((int)fdt32_to_cpu(*p));
		}
	} while (node >= 0);

	return (-EINVAL);
}

int
of_irq_to_resource(struct device_node *dn, int index,
    struct resource *res)
{
	const void *fdt = bsp_fdt_get();
	int len;
	uint32_t spec_buf[8];
	const fdt32_t *spec;
	int node;
	int ic;
	int i;
	int j;
	int item_len;
	int irq;

	if (res != NULL)
		memset(res, 0, sizeof(*res));

	node = dn->offset;

	spec = fdt_getprop(fdt, node, "interrupts", &len);
	if (spec == NULL)
		return (-EINVAL);

	ic = get_interrupt_cells(fdt, node);
	if (ic < 0 || ic >= RTEMS_ARRAY_SIZE(spec_buf))
		return (EINVAL);

	item_len = ic * 4;
	i = index * item_len;
	if (i + item_len > len)
		return (-EINVAL);

	for (j = 0; j < ic; ++j)
		spec_buf[j] = fdt32_to_cpu(spec[i / sizeof(*spec)]);

	irq = bsp_fdt_map_intr(spec_buf, (size_t)ic);

	if (res != NULL) {
		res->start = irq;
		res->end = irq;
	}

	return (irq);
}

#include <linux/of_net.h>
#include <linux/if_ether.h>
#include <linux/phy.h>

static const char * const phy_modes[] = {
	[PHY_INTERFACE_MODE_MII]	= "mii",
	[PHY_INTERFACE_MODE_GMII]	= "gmii",
	[PHY_INTERFACE_MODE_SGMII]	= "sgmii",
	[PHY_INTERFACE_MODE_TBI]	= "tbi",
	[PHY_INTERFACE_MODE_REVMII]	= "rev-mii",
	[PHY_INTERFACE_MODE_RMII]	= "rmii",
	[PHY_INTERFACE_MODE_RGMII]	= "rgmii",
	[PHY_INTERFACE_MODE_RGMII_ID]	= "rgmii-id",
	[PHY_INTERFACE_MODE_RGMII_RXID]	= "rgmii-rxid",
	[PHY_INTERFACE_MODE_RGMII_TXID]	= "rgmii-txid",
	[PHY_INTERFACE_MODE_RTBI]	= "rtbi",
	[PHY_INTERFACE_MODE_SMII]	= "smii",
	[PHY_INTERFACE_MODE_XGMII]	= "xgmii",
	[PHY_INTERFACE_MODE_MOCA]	= "moca",
	[PHY_INTERFACE_MODE_QSGMII]	= "qsgmii"
};

int
of_get_phy_mode(struct device_node *dn)
{
	const void *fdt = bsp_fdt_get();
	int len;
	const char *p;
	int i;

	p = fdt_getprop(fdt, dn->offset, "phy-mode", &len);

	if (p == NULL) {
		p = fdt_getprop(fdt, dn->offset, "phy-connection-type", &len);
	}

	if (p == NULL) {
		return (-ENODEV);
	}

	for (i = 0; i < ARRAY_SIZE(phy_modes); i++) {
		if (phy_modes[i] != NULL && strcmp(p, phy_modes[i]) == 0) {
			return (i);
		}
	}

	return (-ENODEV);
}

static const void *
get_mac_address(struct device_node *dn, const char *name)
{
	const void *fdt = bsp_fdt_get();
	int len;
	const fdt32_t *p;

	p = fdt_getprop(fdt, dn->offset, name, &len);
	if (p == NULL || len != ETH_ALEN) {
		return (NULL);
	}

	return (p);
}

const void *
of_get_mac_address(struct device_node *dn)
{
	const void *addr;

	addr = get_mac_address(dn, "mac-address");
	if (addr != NULL) {
		return addr;
	}

	return get_mac_address(dn, "local-mac-address");
}

#include <linux/interrupt.h>

struct arg_wrapper {
	irq_handler_t handler;
	unsigned int irq;
	void *arg;
};

static void
handler_wrapper(void *arg)
{
	struct arg_wrapper *aw = arg;

	(*aw->handler)(aw->irq, aw->arg);
}

int __must_check
request_irq(unsigned int irq, irq_handler_t handler, unsigned long flags,
    const char *name, void *arg)
{
	struct arg_wrapper *aw;
	rtems_status_code sc;

	aw = kmalloc(sizeof(*aw), GFP_KERNEL);
	if (aw == NULL)
		return (-ENOMEM);

	aw->handler = handler;
	aw->irq = irq;
	aw->arg = arg;
	sc = rtems_interrupt_server_handler_install(RTEMS_ID_NONE, irq, name,
	    RTEMS_INTERRUPT_SHARED, handler_wrapper, aw);
	if (sc != RTEMS_SUCCESSFUL)
		return (-EINVAL);

	return (0);
}

#include <linux/bitrev.h>

const uint8_t bitrev_nibbles[16] = {
	0, 8, 4, 12, 2, 10, 6, 14, 1, 9, 5, 13, 3, 11, 7, 15
};

#include <linux/platform_device.h>

struct resource *
platform_get_resource(struct resource *res, struct platform_device *pdev,
    unsigned int type, unsigned int num)
{
	struct device_node *dn;
	int ret;

	dn = pdev->dev.of_node;

	switch (type) {
	case IORESOURCE_MEM:
		ret = of_address_to_resource(dn, num, res);
		if (ret == 0)
			return res;
	case IORESOURCE_IRQ:
		ret = of_irq_to_resource(dn, num, res);
		if (ret >= 0)
			return res;
	default:
		break;
	}

	return (NULL);
}

int platform_get_irq(struct platform_device *pdev, unsigned int num)
{
	struct resource res_storage;
	struct resource *res;

	res = platform_get_resource(&res_storage, pdev, IORESOURCE_IRQ, num);
	return (res != NULL ? res->start : -ENXIO);
}
