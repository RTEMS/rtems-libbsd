#include <machine/rtems-bsd-kernel-space.h>

#include <rtems/bsd/local/opt_dpaa.h>

/* Copyright 2008 - 2015 Freescale Semiconductor, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *	 notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *	 notice, this list of conditions and the following disclaimer in the
 *	 documentation and/or other materials provided with the distribution.
 *     * Neither the name of Freescale Semiconductor nor the
 *	 names of its contributors may be used to endorse or promote products
 *	 derived from this software without specific prior written permission.
 *
 * ALTERNATIVELY, this software may be distributed under the terms of the
 * GNU General Public License ("GPL") as published by the Free Software
 * Foundation, either version 2 of that License or (at your option) any
 * later version.
 *
 * THIS SOFTWARE IS PROVIDED BY Freescale Semiconductor ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Freescale Semiconductor BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "qman_priv.h"
#ifdef __rtems__
#include <bsp/qoriq.h>
#endif /* __rtems__ */

/* Enable portal interupts (as opposed to polling mode) */
#define CONFIG_FSL_DPA_PIRQ_SLOW  1
#define CONFIG_FSL_DPA_PIRQ_FAST  1

/* Global variable containing revision id (even on non-control plane systems
 * where CCSR isn't available) */
u16 qman_ip_rev;
EXPORT_SYMBOL(qman_ip_rev);
u16 qm_channel_pool1 = QMAN_CHANNEL_POOL1;
EXPORT_SYMBOL(qm_channel_pool1);
u16 qm_channel_caam = QMAN_CHANNEL_CAAM;
EXPORT_SYMBOL(qm_channel_caam);
u16 qm_channel_pme = QMAN_CHANNEL_PME;
EXPORT_SYMBOL(qm_channel_pme);
u16 qm_channel_dce = QMAN_CHANNEL_DCE;
EXPORT_SYMBOL(qm_channel_dce);
u16 qman_portal_max;
EXPORT_SYMBOL(qman_portal_max);

#ifndef __rtems__
/* For these variables, and the portal-initialisation logic, the
 * comments in bman_driver.c apply here so won't be repeated. */
static struct qman_portal *shared_portals[NR_CPUS];
static int num_shared_portals;
static int shared_portals_idx;
static LIST_HEAD(unused_pcfgs);
#endif /* __rtems__ */

/* A SDQCR mask comprising all the available/visible pool channels */
static u32 pools_sdqcr;

#define STR_ERR_NOPROP	    "No '%s' property in node %s\n"
#define STR_ERR_CELL	    "'%s' is not a %d-cell range in node %s\n"
#define STR_FQID_RANGE	    "fsl,fqid-range"
#define STR_POOL_CHAN_RANGE "fsl,pool-channel-range"
#define STR_CGRID_RANGE	     "fsl,cgrid-range"

/* A "fsl,fqid-range" node; release the given range to the allocator */
static __init int fsl_fqid_range_init(struct device_node *node)
{
	int ret;
	const u32 *range = of_get_property(node, STR_FQID_RANGE, &ret);

	if (!range) {
		pr_err(STR_ERR_NOPROP, STR_FQID_RANGE, node->full_name);
		return -EINVAL;
	}
	if (ret != 8) {
		pr_err(STR_ERR_CELL, STR_FQID_RANGE, 2, node->full_name);
		return -EINVAL;
	}
	qman_seed_fqid_range(range[0], range[1]);
	pr_info("FQID allocator includes range %d:%d\n",
		range[0], range[1]);
	return 0;
}

/* A "fsl,pool-channel-range" node; add to the SDQCR mask only */
static __init int fsl_pool_channel_range_sdqcr(struct device_node *node)
{
	int ret;
	const u32 *chanid = of_get_property(node, STR_POOL_CHAN_RANGE, &ret);

	if (!chanid) {
		pr_err(STR_ERR_NOPROP, STR_POOL_CHAN_RANGE, node->full_name);
		return -EINVAL;
	}
	if (ret != 8) {
		pr_err(STR_ERR_CELL, STR_POOL_CHAN_RANGE, 1, node->full_name);
		return -EINVAL;
	}
	for (ret = 0; ret < chanid[1]; ret++)
		pools_sdqcr |= QM_SDQCR_CHANNELS_POOL_CONV(chanid[0] + ret);
	return 0;
}

/* A "fsl,pool-channel-range" node; release the given range to the allocator */
static __init int fsl_pool_channel_range_init(struct device_node *node)
{
	int ret;
	const u32 *chanid = of_get_property(node, STR_POOL_CHAN_RANGE, &ret);

	if (!chanid) {
		pr_err(STR_ERR_NOPROP, STR_POOL_CHAN_RANGE, node->full_name);
		return -EINVAL;
	}
	if (ret != 8) {
		pr_err(STR_ERR_CELL, STR_POOL_CHAN_RANGE, 1, node->full_name);
		return -EINVAL;
	}
	qman_seed_pool_range(chanid[0], chanid[1]);
	pr_info("Pool channel allocator includes range %d:%d\n",
		chanid[0], chanid[1]);
	return 0;
}

/* A "fsl,cgrid-range" node; release the given range to the allocator */
static __init int fsl_cgrid_range_init(struct device_node *node)
{
	struct qman_cgr cgr;
	int ret, errors = 0;
	const u32 *range = of_get_property(node, STR_CGRID_RANGE, &ret);

	if (!range) {
		pr_err(STR_ERR_NOPROP, STR_CGRID_RANGE, node->full_name);
		return -EINVAL;
	}
	if (ret != 8) {
		pr_err(STR_ERR_CELL, STR_CGRID_RANGE, 2, node->full_name);
		return -EINVAL;
	}
	qman_seed_cgrid_range(range[0], range[1]);
	pr_info("CGRID allocator includes range %d:%d\n",
		range[0], range[1]);
	for (cgr.cgrid = 0; cgr.cgrid < __CGR_NUM; cgr.cgrid++) {
		ret = qman_modify_cgr(&cgr, QMAN_CGR_FLAG_USE_INIT, NULL);
		if (ret)
			errors++;
	}
	if (errors)
		pr_err("Warning: %d error%s while initialising CGRs %d:%d\n",
			errors, (errors > 1) ? "s" : "", range[0], range[1]);
	return 0;
}

static void qman_get_ip_revision(struct device_node *dn)
{
#ifdef __rtems__
	struct device_node of_dns;
#endif /* __rtems__ */
	u16 ip_rev = 0;

	for_each_compatible_node(dn, NULL, "fsl,qman-portal") {
		if (!of_device_is_available(dn))
			continue;
		if (of_device_is_compatible(dn, "fsl,qman-portal-1.0") ||
			of_device_is_compatible(dn, "fsl,qman-portal-1.0.0")) {
			pr_err("Rev1.0 on P4080 rev1 is not supported!\n");
			BUG_ON(1);
		} else if (of_device_is_compatible(dn, "fsl,qman-portal-1.1") ||
			of_device_is_compatible(dn, "fsl,qman-portal-1.1.0")) {
			ip_rev = QMAN_REV11;
			qman_portal_max = 10;
		} else if (of_device_is_compatible(dn, "fsl,qman-portal-1.2") ||
			of_device_is_compatible(dn, "fsl,qman-portal-1.2.0")) {
			ip_rev = QMAN_REV12;
			qman_portal_max = 10;
		} else if (of_device_is_compatible(dn, "fsl,qman-portal-2.0") ||
			of_device_is_compatible(dn, "fsl,qman-portal-2.0.0")) {
			ip_rev = QMAN_REV20;
			qman_portal_max = 3;
		} else if (of_device_is_compatible(dn,
						"fsl,qman-portal-3.0.0")) {
			ip_rev = QMAN_REV30;
			qman_portal_max = 50;
		} else if (of_device_is_compatible(dn,
						"fsl,qman-portal-3.0.1")) {
			ip_rev = QMAN_REV30;
			qman_portal_max = 25;
		} else if (of_device_is_compatible(dn,
						"fsl,qman-portal-3.1.0")) {
			ip_rev = QMAN_REV31;
			qman_portal_max = 50;
		} else if (of_device_is_compatible(dn,
						"fsl,qman-portal-3.1.1")) {
			ip_rev = QMAN_REV31;
			qman_portal_max = 25;
		} else if (of_device_is_compatible(dn,
						"fsl,qman-portal-3.1.2")) {
			ip_rev = QMAN_REV31;
			qman_portal_max = 18;
		} else if (of_device_is_compatible(dn,
						"fsl,qman-portal-3.1.3")) {
			ip_rev = QMAN_REV31;
			qman_portal_max = 10;
		} else {
			pr_warn("Unknown version in portal node, default to rev1.1\n");
			ip_rev = QMAN_REV11;
			qman_portal_max = 10;
		}

		if (!qman_ip_rev) {
			if (ip_rev) {
				qman_ip_rev = ip_rev;
			} else {
				pr_warn("Unknown version, default to rev1.1\n");
				qman_ip_rev = QMAN_REV11;
			}
		} else if (ip_rev && (qman_ip_rev != ip_rev))
			pr_warn("Revision = 0x%04x, but portal '%s' has 0x%04x\n",
				qman_ip_rev, dn->full_name, ip_rev);
		if (qman_ip_rev == ip_rev)
			break;
	}
}

#ifndef __rtems__
/* Parse a portal node, perform generic mapping duties and return the config. It
 * is not known at this stage for what purpose (or even if) the portal will be
 * used. */
static struct qm_portal_config * __init parse_pcfg(struct device_node *node)
{
	struct qm_portal_config *pcfg;
	const u32 *channel;
	int irq, ret;
	struct resource res;

	pcfg = kzalloc(sizeof(*pcfg), GFP_KERNEL);
	if (!pcfg)
		return NULL;

	/*
	 * This is a *horrible hack*, but the IOMMU/PAMU driver needs a
	 * 'struct device' in order to get the PAMU stashing setup and the QMan
	 * portal [driver] won't function at all without ring stashing
	 *
	 * Making the QMan portal driver nice and proper is part of the
	 * upstreaming effort
	 */
	pcfg->dev.bus = &platform_bus_type;
	pcfg->dev.of_node = node;
#ifdef CONFIG_IOMMU_API
	pcfg->dev.archdata.iommu_domain = NULL;
#endif

	ret = of_address_to_resource(node, DPA_PORTAL_CE,
				&pcfg->addr_phys[DPA_PORTAL_CE]);
	if (ret) {
		pr_err("Can't get %s property 'reg::CE'\n", node->full_name);
		goto err;
	}
	ret = of_address_to_resource(node, DPA_PORTAL_CI,
				&pcfg->addr_phys[DPA_PORTAL_CI]);
	if (ret) {
		pr_err("Can't get %s property 'reg::CI'\n", node->full_name);
		goto err;
	}

	channel = of_get_property(node, "fsl,qman-channel-id", &ret);
	if (!channel || (ret != 4)) {
		pr_err("Can't get %s property 'fsl,qman-channel-id'\n",
		       node->full_name);
		goto err;
	}
	pcfg->public_cfg.channel = *channel;
	pcfg->public_cfg.cpu = -1;
	irq = irq_of_parse_and_map(node, 0);
	if (irq == NO_IRQ) {
		pr_err("Can't get %s property 'interrupts'\n", node->full_name);
		goto err;
	}
	pcfg->public_cfg.irq = irq;
#ifdef CONFIG_FSL_QMAN_CONFIG
	/* We need the same LIODN offset for all portals */
	qman_liodn_fixup(pcfg->public_cfg.channel);
#endif

	pcfg->addr_virt[DPA_PORTAL_CE] = ioremap_prot(
				pcfg->addr_phys[DPA_PORTAL_CE].start,
				resource_size(&pcfg->addr_phys[DPA_PORTAL_CE]),
				0);
	pcfg->addr_virt[DPA_PORTAL_CI] = ioremap_prot(
				pcfg->addr_phys[DPA_PORTAL_CI].start,
				resource_size(&pcfg->addr_phys[DPA_PORTAL_CI]),
				_PAGE_GUARDED | _PAGE_NO_CACHE);

	return pcfg;
err:
	kfree(pcfg);
	return NULL;
}

static struct qm_portal_config *get_pcfg(struct list_head *list)
{
	struct qm_portal_config *pcfg;

	if (list_empty(list))
		return NULL;
	pcfg = list_entry(list->prev, struct qm_portal_config, list);
	list_del(&pcfg->list);
	return pcfg;
}
#endif /* __rtems__ */

static void portal_set_cpu(struct qm_portal_config *pcfg, int cpu)
{
#ifdef CONFIG_FSL_PAMU
	int ret;
	int window_count = 1;
	struct iommu_domain_geometry geom_attr;
	struct pamu_stash_attribute stash_attr;

	pcfg->iommu_domain = iommu_domain_alloc(&platform_bus_type);
	if (!pcfg->iommu_domain) {
		pr_err("%s(): iommu_domain_alloc() failed", __func__);
		goto _no_iommu;
	}
	geom_attr.aperture_start = 0;
	geom_attr.aperture_end =
		((dma_addr_t)1 << min(8 * sizeof(dma_addr_t), (size_t)36)) - 1;
	geom_attr.force_aperture = true;
	ret = iommu_domain_set_attr(pcfg->iommu_domain, DOMAIN_ATTR_GEOMETRY,
				    &geom_attr);
	if (ret < 0) {
		pr_err("%s(): iommu_domain_set_attr() = %d", __func__, ret);
		goto _iommu_domain_free;
	}
	ret = iommu_domain_set_attr(pcfg->iommu_domain, DOMAIN_ATTR_WINDOWS,
				    &window_count);
	if (ret < 0) {
		pr_err("%s(): iommu_domain_set_attr() = %d", __func__, ret);
		goto _iommu_domain_free;
	}
	stash_attr.cpu = cpu;
	stash_attr.cache = PAMU_ATTR_CACHE_L1;
	ret = iommu_domain_set_attr(pcfg->iommu_domain,
				    DOMAIN_ATTR_FSL_PAMU_STASH,
				    &stash_attr);
	if (ret < 0) {
		pr_err("%s(): iommu_domain_set_attr() = %d",
			   __func__, ret);
		goto _iommu_domain_free;
	}
	ret = iommu_domain_window_enable(pcfg->iommu_domain, 0, 0, 1ULL << 36,
					 IOMMU_READ | IOMMU_WRITE);
	if (ret < 0) {
		pr_err("%s(): iommu_domain_window_enable() = %d",
			   __func__, ret);
		goto _iommu_domain_free;
	}
	ret = iommu_attach_device(pcfg->iommu_domain, &pcfg->dev);
	if (ret < 0) {
		pr_err("%s(): iommu_device_attach() = %d",
			   __func__, ret);
		goto _iommu_domain_free;
	}
	ret = iommu_domain_set_attr(pcfg->iommu_domain,
				    DOMAIN_ATTR_FSL_PAMU_ENABLE,
				    &window_count);
	if (ret < 0) {
		pr_err("%s(): iommu_domain_set_attr() = %d",
			   __func__, ret);
		goto _iommu_detach_device;
	}

_no_iommu:
#endif
#ifdef CONFIG_FSL_QMAN_CONFIG
	if (qman_set_sdest(pcfg->public_cfg.channel, cpu))
#endif
		pr_warn("Failed to set the stash request queue\n");

	return;

#ifdef CONFIG_FSL_PAMU
_iommu_detach_device:
	iommu_detach_device(pcfg->iommu_domain, NULL);
_iommu_domain_free:
	iommu_domain_free(pcfg->iommu_domain);
	pcfg->iommu_domain = NULL;
#endif
}

static struct qman_portal *init_pcfg(struct qm_portal_config *pcfg)
{
	struct qman_portal *p;

#ifndef __rtems__
	pcfg->iommu_domain = NULL;
#endif /* __rtems__ */
	portal_set_cpu(pcfg, pcfg->public_cfg.cpu);
	p = qman_create_affine_portal(pcfg, NULL);
	if (p) {
		u32 irq_sources = 0;
		/* Determine what should be interrupt-vs-poll driven */
#ifdef CONFIG_FSL_DPA_PIRQ_SLOW
		irq_sources |= QM_PIRQ_EQCI | QM_PIRQ_EQRI | QM_PIRQ_MRI |
			       QM_PIRQ_CSCI;
#endif
#ifdef CONFIG_FSL_DPA_PIRQ_FAST
		irq_sources |= QM_PIRQ_DQRI;
#endif
		qman_p_irqsource_add(p, irq_sources);
		pr_info("Portal %sinitialised, cpu %d\n",
#ifndef __rtems__
			pcfg->public_cfg.is_shared ? "(shared) " : "",
#else /* __rtems__ */
			"",
#endif /* __rtems__ */
			pcfg->public_cfg.cpu);
	} else
		pr_crit("Portal failure on cpu %d\n", pcfg->public_cfg.cpu);
	return p;
}

#ifndef __rtems__
static void init_slave(int cpu)
{
	struct qman_portal *p;
	struct cpumask oldmask = *tsk_cpus_allowed(current);

	set_cpus_allowed_ptr(current, get_cpu_mask(cpu));
	p = qman_create_affine_slave(shared_portals[shared_portals_idx++], cpu);
	if (!p)
		pr_err("Slave portal failure on cpu %d\n", cpu);
	else
		pr_info("Portal (slave) initialised, cpu %d\n", cpu);
	set_cpus_allowed_ptr(current, &oldmask);
	if (shared_portals_idx >= num_shared_portals)
		shared_portals_idx = 0;
}

static struct cpumask want_unshared __initdata;
static struct cpumask want_shared __initdata;

static int __init parse_qportals(char *str)
{
	return parse_portals_bootarg(str, &want_shared, &want_unshared,
				     "qportals");
}
__setup("qportals=", parse_qportals);

static void qman_portal_update_sdest(const struct qm_portal_config *pcfg,
							unsigned int cpu)
{
	struct pamu_stash_attribute stash_attr;
	int ret;

	if (pcfg->iommu_domain) {
		stash_attr.cpu = cpu;
		stash_attr.cache = PAMU_ATTR_CACHE_L1;
		ret = iommu_domain_set_attr(pcfg->iommu_domain,
				DOMAIN_ATTR_FSL_PAMU_STASH, &stash_attr);
		if (ret < 0) {
			pr_err("Failed to update pamu stash setting\n");
			return;
		}
	}
#ifdef CONFIG_FSL_QMAN_CONFIG
	if (qman_set_sdest(pcfg->public_cfg.channel, cpu))
#endif
		pr_warn("Failed to update portal's stash request queue\n");
}

static void qman_offline_cpu(unsigned int cpu)
{
	struct qman_portal *p;
	const struct qm_portal_config *pcfg;

	p = (struct qman_portal *)affine_portals[cpu];
	if (p) {
		pcfg = qman_get_qm_portal_config(p);
		if (pcfg) {
			irq_set_affinity(pcfg->public_cfg.irq, cpumask_of(0));
			qman_portal_update_sdest(pcfg, 0);
		}
	}
}
#endif /* __rtems__ */

#ifdef CONFIG_HOTPLUG_CPU
static void qman_online_cpu(unsigned int cpu)
{
	struct qman_portal *p;
	const struct qm_portal_config *pcfg;

	p = (struct qman_portal *)affine_portals[cpu];
	if (p) {
		pcfg = qman_get_qm_portal_config(p);
		if (pcfg) {
			irq_set_affinity(pcfg->public_cfg.irq, cpumask_of(cpu));
			qman_portal_update_sdest(pcfg, cpu);
		}
	}
}

static int qman_hotplug_cpu_callback(struct notifier_block *nfb,
				     unsigned long action, void *hcpu)
{
	unsigned int cpu = (unsigned long)hcpu;

	switch (action) {
	case CPU_ONLINE:
	case CPU_ONLINE_FROZEN:
		qman_online_cpu(cpu);
		break;
	case CPU_DOWN_PREPARE:
	case CPU_DOWN_PREPARE_FROZEN:
		qman_offline_cpu(cpu);
	default:
		break;
	}
	return NOTIFY_OK;
}

static struct notifier_block qman_hotplug_cpu_notifier = {
	.notifier_call = qman_hotplug_cpu_callback,
};
#endif /* CONFIG_HOTPLUG_CPU */

#ifdef __rtems__
#include <bsp/fdt.h>
static struct qm_portal_config qman_configs[NR_CPUS];
static void
qman_init_portals(void)
{
	const char *fdt = bsp_fdt_get();
	struct device_node dn;
	const char *name;
	int cpu_count = (int)rtems_get_processor_count();
	int cpu;
	int ret;
	int node;
	int parent;

	memset(&dn, 0, sizeof(dn));

	name = "fsl,qman-portal";
	node = fdt_node_offset_by_compatible(fdt, 0, name);
	if (node < 0)
		panic("qman: no portals in FDT");
	parent = fdt_parent_offset(fdt, node);
	if (parent < 0)
		panic("qman: no parent of portals in FDT");
	node = fdt_first_subnode(fdt, parent);

	dn.full_name = name;
	dn.offset = node;

	qoriq_clear_ce_portal(&qoriq_qman_portal[0][0],
	    sizeof(qoriq_qman_portal[0]));
	qoriq_clear_ci_portal(&qoriq_qman_portal[1][0],
	    sizeof(qoriq_qman_portal[1]));

	for (cpu = 0; cpu < cpu_count; ++cpu) {
		struct qm_portal_config *pcfg = &qman_configs[cpu];
		struct qman_portal *portal;
		struct resource res;
		const u32 *channel;

		if (node < 0)
			panic("qman: missing portal in FDT");

		ret = of_address_to_resource(&dn, 0, &res);
		if (ret != 0)
			panic("qman: no portal CE address");
		pcfg->addr_virt[0] = (__iomem void *)
		    ((uintptr_t)&qoriq_qman_portal[0][0] + (uintptr_t)res.start);
		BSD_ASSERT((uintptr_t)pcfg->addr_virt[0] >=
		    (uintptr_t)&qoriq_qman_portal[0][0]);
		BSD_ASSERT((uintptr_t)pcfg->addr_virt[0] <
		    (uintptr_t)&qoriq_qman_portal[1][0]);

		ret = of_address_to_resource(&dn, 1, &res);
		if (ret != 0)
			panic("qman: no portal CI address");
		pcfg->addr_virt[1] = (__iomem void *)
		    ((uintptr_t)&qoriq_qman_portal[0][0] + (uintptr_t)res.start);
		BSD_ASSERT((uintptr_t)pcfg->addr_virt[1] >=
		    (uintptr_t)&qoriq_qman_portal[1][0]);
		BSD_ASSERT((uintptr_t)pcfg->addr_virt[1] <
		    (uintptr_t)&qoriq_qman_portal[2][0]);

		pcfg->public_cfg.irq = of_irq_to_resource(&dn, 0, NULL);
		if (pcfg->public_cfg.irq == NO_IRQ)
			panic("qman: no portal interrupt");

		channel = of_get_property(&dn, "fsl,qman-channel-id", &ret);
		if (channel == NULL || ret != 4)
			panic("qman: no portal channel ID");
		pcfg->public_cfg.channel = *channel;

		pcfg->public_cfg.cpu = cpu;
		pcfg->public_cfg.pools = pools_sdqcr;

		portal = init_pcfg(pcfg);
		if (portal == NULL)
			panic("qman: cannot create portal");

		node = fdt_next_subnode(fdt, node);
		dn.offset = node;
	}
}
#endif /* __rtems__ */
#ifndef __rtems__
__init int qman_init(void)
{
	struct cpumask slave_cpus;
	struct cpumask unshared_cpus = *cpu_none_mask;
	struct cpumask shared_cpus = *cpu_none_mask;
	LIST_HEAD(unshared_pcfgs);
	LIST_HEAD(shared_pcfgs);
	struct device_node *dn;
	struct qm_portal_config *pcfg;
	struct qman_portal *p;
	int cpu, ret;
	struct cpumask offline_cpus;

	/* Initialise the QMan (CCSR) device */
	for_each_compatible_node(dn, NULL, "fsl,qman") {
		if (!qman_init_ccsr(dn))
			pr_info("Err interrupt handler present\n");
		else
			pr_err("CCSR setup failed\n");
	}
#else /* __rtems__ */
int
qman_init(struct device_node *dn)
{
	struct device_node of_dns;
	int ret;
#endif /* __rtems__ */
#ifdef CONFIG_FSL_QMAN_FQ_LOOKUP
	/* Setup lookup table for FQ demux */
	ret = qman_setup_fq_lookup_table(qman_fqd_size()/64);
	if (ret)
		return ret;
#endif

	/* Get qman ip revision */
	qman_get_ip_revision(dn);
	if ((qman_ip_rev & 0xff00) >= QMAN_REV30) {
		qm_channel_pool1 = QMAN_CHANNEL_POOL1_REV3;
		qm_channel_caam = QMAN_CHANNEL_CAAM_REV3;
		qm_channel_pme = QMAN_CHANNEL_PME_REV3;
	}

	/* Parse pool channels into the SDQCR mask. (Must happen before portals
	 * are initialised.) */
	for_each_compatible_node(dn, NULL, "fsl,pool-channel-range") {
		ret = fsl_pool_channel_range_sdqcr(dn);
		if (ret)
			return ret;
	}

#ifndef __rtems__
	memset(affine_portals, 0, sizeof(void *) * num_possible_cpus());
	/* Initialise portals. See bman_driver.c for comments */
	for_each_compatible_node(dn, NULL, "fsl,qman-portal") {
		if (!of_device_is_available(dn))
			continue;
		pcfg = parse_pcfg(dn);
		if (pcfg) {
			pcfg->public_cfg.pools = pools_sdqcr;
			list_add_tail(&pcfg->list, &unused_pcfgs);
		}
	}
	for_each_possible_cpu(cpu) {
		if (cpumask_test_cpu(cpu, &want_shared)) {
			pcfg = get_pcfg(&unused_pcfgs);
			if (!pcfg)
				break;
			pcfg->public_cfg.cpu = cpu;
			list_add_tail(&pcfg->list, &shared_pcfgs);
			cpumask_set_cpu(cpu, &shared_cpus);
		}
		if (cpumask_test_cpu(cpu, &want_unshared)) {
			if (cpumask_test_cpu(cpu, &shared_cpus))
				continue;
			pcfg = get_pcfg(&unused_pcfgs);
			if (!pcfg)
				break;
			pcfg->public_cfg.cpu = cpu;
			list_add_tail(&pcfg->list, &unshared_pcfgs);
			cpumask_set_cpu(cpu, &unshared_cpus);
		}
	}
	if (list_empty(&shared_pcfgs) && list_empty(&unshared_pcfgs)) {
		for_each_possible_cpu(cpu) {
			pcfg = get_pcfg(&unused_pcfgs);
			if (!pcfg)
				break;
			pcfg->public_cfg.cpu = cpu;
			list_add_tail(&pcfg->list, &unshared_pcfgs);
			cpumask_set_cpu(cpu, &unshared_cpus);
		}
	}
	cpumask_andnot(&slave_cpus, cpu_possible_mask, &shared_cpus);
	cpumask_andnot(&slave_cpus, &slave_cpus, &unshared_cpus);
	if (cpumask_empty(&slave_cpus)) {
		if (!list_empty(&shared_pcfgs)) {
			cpumask_or(&unshared_cpus, &unshared_cpus,
				   &shared_cpus);
			cpumask_clear(&shared_cpus);
			list_splice_tail(&shared_pcfgs, &unshared_pcfgs);
			INIT_LIST_HEAD(&shared_pcfgs);
		}
	} else {
		if (list_empty(&shared_pcfgs)) {
			pcfg = get_pcfg(&unshared_pcfgs);
			if (!pcfg) {
				pr_crit("No portals available!\n");
				return 0;
			}
			cpumask_clear_cpu(pcfg->public_cfg.cpu, &unshared_cpus);
			cpumask_set_cpu(pcfg->public_cfg.cpu, &shared_cpus);
			list_add_tail(&pcfg->list, &shared_pcfgs);
		}
	}
	list_for_each_entry(pcfg, &unshared_pcfgs, list) {
		pcfg->public_cfg.is_shared = 0;
		p = init_pcfg(pcfg);
	}
	list_for_each_entry(pcfg, &shared_pcfgs, list) {
		pcfg->public_cfg.is_shared = 1;
		p = init_pcfg(pcfg);
		if (p)
			shared_portals[num_shared_portals++] = p;
	}
	if (!cpumask_empty(&slave_cpus))
		for_each_cpu(cpu, &slave_cpus)
			init_slave(cpu);
#else /* __rtems__ */
	qman_init_portals();
#endif /* __rtems__ */
	pr_info("Portals initialised\n");
#ifndef __rtems__
	cpumask_andnot(&offline_cpus, cpu_possible_mask, cpu_online_mask);
	for_each_cpu(cpu, &offline_cpus)
		qman_offline_cpu(cpu);
#endif /* __rtems__ */
#ifdef CONFIG_HOTPLUG_CPU
	register_hotcpu_notifier(&qman_hotplug_cpu_notifier);
#endif
	return 0;
}

__init int qman_resource_init(void)
{
#ifdef __rtems__
	struct device_node of_dns;
#endif /* __rtems__ */
	struct device_node *dn;
	int ret;

	/* Initialise FQID allocation ranges */
	for_each_compatible_node(dn, NULL, "fsl,fqid-range") {
		ret = fsl_fqid_range_init(dn);
		if (ret)
			return ret;
	}
	/* Initialise CGRID allocation ranges */
	for_each_compatible_node(dn, NULL, "fsl,cgrid-range") {
		ret = fsl_cgrid_range_init(dn);
		if (ret)
			return ret;
	}
	/* Parse pool channels into the allocator. (Must happen after portals
	 * are initialised.) */
	for_each_compatible_node(dn, NULL, "fsl,pool-channel-range") {
		ret = fsl_pool_channel_range_init(dn);
		if (ret)
			return ret;
	}

	return 0;
}
