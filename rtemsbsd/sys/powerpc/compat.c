#include <machine/rtems-bsd-kernel-space.h>
#include <rtems/bsd/local/opt_dpaa.h>

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
of_address_to_resource(struct device_node *dn, int index,
    struct resource *res)
{
	const void *fdt = bsp_fdt_get();
	int ac;
	int sc;
	int len;
	const fdt32_t *p;
	int i;

	memset(res, 0, sizeof(*res));

	ac = get_address_cells(fdt, dn->offset);
	if (ac < 0)
		return (-EINVAL);

	sc = get_size_cells(fdt, dn->offset);
	if (sc < 0)
		return (-EINVAL);

	p = fdt_getprop(fdt, dn->offset, "reg", &len);
	if (p == NULL)
		return (-EINVAL);

	len /= sizeof(*p);
	i = index * (ac + sc);
	if (i + ac + sc > len)
		return (-EINVAL);

	while (ac > 0) {
		res->start = (res->start << 32) | fdt32_to_cpu(p[i]);
		++i;
		--ac;
	}

	while (sc > 0) {
		res->end = (res->end << 32) | fdt32_to_cpu(p[i]);
		++i;
		--sc;
	}
	res->end += res->start;

	return (0);
}

int
of_irq_to_resource(struct device_node *dn, int index,
    struct resource *res)
{
	const void *fdt = bsp_fdt_get();
	int len;
	const fdt32_t *p;
	int i;
	int irq;

	if (res != NULL)
		memset(res, 0, sizeof(*res));

	p = fdt_getprop(fdt, dn->offset, "interrupts", &len);
	if (p == NULL)
		return (-EINVAL);

	i = index * 16;
	if (i + 16 > len)
		return (-EINVAL);

	irq = (int)fdt32_to_cpu(p[i / sizeof(*p)]);
#ifdef __PPC__
	/* FIXME */
	irq -= 16;
#endif
	return (irq);
}

#include <linux/of_net.h>
#include <linux/if_ether.h>

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
platform_get_resource_impl(struct platform_device *dev,
    unsigned int type, unsigned int num, struct resource *res)
{

	return (res);
}
