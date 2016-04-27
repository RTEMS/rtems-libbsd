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

#include "bman_priv.h"

/*
 * Global variables of the max portal/pool number this BMan version supported
 */
u16 bman_ip_rev;
EXPORT_SYMBOL(bman_ip_rev);

u16 bman_pool_max;
EXPORT_SYMBOL(bman_pool_max);

/* After initialising cpus that own shared portal configs, we cache the
 * resulting portals (ie. not just the configs) in this array. Then we
 * initialise slave cpus that don't have their own portals, redirecting them to
 * portals from this cache in a round-robin assignment. */
static struct bman_portal *shared_portals[NR_CPUS] __initdata;
static int num_shared_portals __initdata;
static int shared_portals_idx __initdata;

static LIST_HEAD(unused_pcfgs);
static void *affine_bportals[NR_CPUS];

#ifndef __rtems__
static const int flags[] = {0, _PAGE_GUARDED | _PAGE_NO_CACHE};
#else /* __rtems__ */
static const int flags[] = {0, 0};
#endif /* __rtems__ */

static struct bm_portal_config * __init get_pcfg(struct list_head *list)
{
	struct bm_portal_config *pcfg;

	if (list_empty(list))
		return NULL;
	pcfg = list_entry(list->prev, struct bm_portal_config, list);
	list_del(&pcfg->list);

	return pcfg;
}

static struct bman_portal * __init init_pcfg(struct bm_portal_config *pcfg)
{
	struct bman_portal *p = bman_create_affine_portal(pcfg);

	if (p) {
#ifdef CONFIG_FSL_DPA_PIRQ_SLOW
		bman_p_irqsource_add(p, BM_PIRQ_RCRI | BM_PIRQ_BSCN);
#endif
		pr_info("Portal %sinitialised, cpu %d\n",
			pcfg->public_cfg.is_shared ? "(shared) " : "",
			pcfg->public_cfg.cpu);
		affine_bportals[pcfg->public_cfg.cpu] = p;
	} else
		pr_crit("Portal failure on cpu %d\n", pcfg->public_cfg.cpu);

	return p;
}

static void __init init_slave(int cpu)
{
	struct bman_portal *p;

	p = bman_create_affine_slave(shared_portals[shared_portals_idx++], cpu);
	if (!p)
		pr_err("Slave portal failure on cpu %d\n", cpu);
	else
		pr_info("Portal %s initialised, cpu %d\n", "(slave) ", cpu);
	if (shared_portals_idx >= num_shared_portals)
		shared_portals_idx = 0;
	affine_bportals[cpu] = p;
}

/* Bootarg "bportals=[...]" has the same syntax as "qportals=", and so the
 * parsing is in dpaa_sys.h. The syntax is a comma-separated list of indexes
 * and/or ranges of indexes, with each being optionally prefixed by "s" to
 * explicitly mark it or them for sharing.
 *    Eg;
 *	  bportals=s0,1-3,s4
 * means that cpus 1,2,3 get "unshared" portals, cpus 0 and 4 get "shared"
 * portals, and any remaining cpus share the portals that are assigned to cpus 0
 * or 4, selected in a round-robin fashion. (In this example, cpu 5 would share
 * cpu 0's portal, cpu 6 would share cpu4's portal, and cpu 7 would share cpu
 * 0's portal.) */
static struct cpumask want_unshared __initdata; /* cpus requested without "s" */
static struct cpumask want_shared __initdata; /* cpus requested with "s" */

static int __init parse_bportals(char *str)
{
	return parse_portals_bootarg(str, &want_shared, &want_unshared,
				     "bportals");
}
__setup("bportals=", parse_bportals);

static void __cold bman_offline_cpu(unsigned int cpu)
{
	struct bman_portal *p = (struct bman_portal *)affine_bportals[cpu];
	const struct bm_portal_config *pcfg;

	if (p) {
		pcfg = bman_get_bm_portal_config(p);
		if (pcfg)
			irq_set_affinity(pcfg->public_cfg.irq, cpumask_of(0));
	}
}

#ifdef CONFIG_HOTPLUG_CPU
static void __cold bman_online_cpu(unsigned int cpu)
{
	struct bman_portal *p = (struct bman_portal *)affine_bportals[cpu];
	const struct bm_portal_config *pcfg;

	if (p) {
		pcfg = bman_get_bm_portal_config(p);
		if (pcfg)
			irq_set_affinity(pcfg->public_cfg.irq, cpumask_of(cpu));
	}
}

static int __cold bman_hotplug_cpu_callback(struct notifier_block *nfb,
					    unsigned long action, void *hcpu)
{
	unsigned int cpu = (unsigned long)hcpu;

	switch (action) {
	case CPU_ONLINE:
	case CPU_ONLINE_FROZEN:
		bman_online_cpu(cpu);
		break;
	case CPU_DOWN_PREPARE:
	case CPU_DOWN_PREPARE_FROZEN:
		bman_offline_cpu(cpu);
	}

	return NOTIFY_OK;
}

static struct notifier_block bman_hotplug_cpu_notifier = {
	.notifier_call = bman_hotplug_cpu_callback,
};
#endif /* CONFIG_HOTPLUG_CPU */

static int __cold bman_portal_probe(struct platform_device *of_dev)
{
	struct device *dev = &of_dev->dev;
	struct device_node *node = dev->of_node;
	struct bm_portal_config *pcfg;
	int i, irq, ret;

	if (!of_device_is_available(node))
		return -ENODEV;

	if (of_device_is_compatible(node, "fsl,bman-portal-1.0") ||
		of_device_is_compatible(node, "fsl,bman-portal-1.0.0")) {
		bman_ip_rev = BMAN_REV10;
		bman_pool_max = 64;
	} else if (of_device_is_compatible(node, "fsl,bman-portal-2.0") ||
		of_device_is_compatible(node, "fsl,bman-portal-2.0.8")) {
		bman_ip_rev = BMAN_REV20;
		bman_pool_max = 8;
	} else if (of_device_is_compatible(node, "fsl,bman-portal-2.1.0") ||
		   of_device_is_compatible(node, "fsl,bman-portal-2.1.1") ||
		   of_device_is_compatible(node, "fsl,bman-portal-2.1.2") ||
		   of_device_is_compatible(node, "fsl,bman-portal-2.1.3")) {
		bman_ip_rev = BMAN_REV21;
		bman_pool_max = 64;
	}

	pcfg = devm_kmalloc(dev, sizeof(*pcfg), GFP_KERNEL);
	if (!pcfg) {
		dev_err(dev, "Can't allocate portal config\n");
		return -ENOMEM;
	}

	for (i = DPA_PORTAL_CE; i <= DPA_PORTAL_CI; i++) {
		ret = of_address_to_resource(node, i, pcfg->addr_phys + i);
		if (ret < 0) {
			dev_err(dev, "Can't get %s property 'reg::%d'\n",
				node->full_name, i);
			return ret;
		}
		ret = devm_request_resource(dev, &iomem_resource,
					    pcfg->addr_phys + i);
		if (ret < 0)
			return ret;
		pcfg->addr_virt[i] = devm_ioremap_prot(dev,
					pcfg->addr_phys[i].start,
					resource_size(pcfg->addr_phys + i),
					flags[i]);
		if (!pcfg->addr_virt[i])
			return -ENXIO;
	}

	pcfg->public_cfg.cpu = -1;

	irq = irq_of_parse_and_map(node, 0);
	if (irq == NO_IRQ) {
		dev_err(dev, "Can't get %s property 'interrupts'\n",
			node->full_name);
		return -ENXIO;
	}
	pcfg->public_cfg.irq = irq;

	bman_depletion_fill(&pcfg->public_cfg.mask);

	list_add_tail(&pcfg->list, &unused_pcfgs);

	return 0;
};

static int __cold bman_portal_remove(struct platform_device *of_dev)
{
	return 0;
};

static const struct of_device_id bman_portal_ids[] = {
	{
		.compatible = "fsl,bman-portal",
	},
	{}
};
MODULE_DEVICE_TABLE(of, bman_portal_ids);

static struct platform_driver bman_portal_driver = {
	.driver = {
		.name = KBUILD_MODNAME,
		.of_match_table = bman_portal_ids,
	},
	.probe = bman_portal_probe,
	.remove = bman_portal_remove,
};

static int __init bman_portal_driver_register(struct platform_driver *drv)
{
	int _errno;
	struct cpumask slave_cpus;
	struct cpumask unshared_cpus = *cpu_none_mask;
	struct cpumask shared_cpus = *cpu_none_mask;
	LIST_HEAD(unshared_pcfgs);
	LIST_HEAD(shared_pcfgs);
	struct bm_portal_config *pcfg;
	struct bman_portal *p;
	int cpu;
	struct cpumask offline_cpus;

	_errno = platform_driver_register(drv);
	if (_errno < 0)
		return _errno;

/* Initialise the BMan driver. The meat of this function deals with portals. The
 * following describes the flow of portal-handling, the code "steps" refer to
 * this description;
 * 1. Portal configs are parsed from the device-tree into 'unused_pcfgs', with
 *    ::cpu==-1. Regions and interrupts are mapped (but interrupts are not
 *    bound).
 * 2. The "want_shared" and "want_unshared" lists (as filled by the
 *    "bportals=[...]" bootarg) are processed, allocating portals and assigning
 *    them to cpus, placing them in the relevant list and setting ::cpu as
 *    appropriate. If no "bportals" bootarg was present, the defaut is to try to
 *    assign portals to all online cpus at the time of driver initialisation.
 *    Any failure to allocate portals (when parsing the "want" lists or when
 *    using default behaviour) will be silently tolerated (the "fixup" logic in
 *    step 3 will determine what happens in this case).
 * 3. Do fixups relative to cpu_online_mask(). If no portals are marked for
 *    sharing and sharing is required (because not all cpus have been assigned
 *    portals), then one portal will marked for sharing. Conversely if no
 *    sharing is required, any portals marked for sharing will not be shared. It
 *    may be that sharing occurs when it wasn't expected, if portal allocation
 *    failed to honour all the requested assignments (including the default
 *    assignments if no bootarg is present).
 * 4. Unshared portals are initialised on their respective cpus.
 * 5. Shared portals are initialised on their respective cpus.
 * 6. Each remaining cpu is initialised to slave to one of the shared portals,
 *    which are selected in a round-robin fashion.
 */
	/* Step 2. */
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
		/* Default, give an unshared portal to each online cpu */
		for_each_possible_cpu(cpu) {
			pcfg = get_pcfg(&unused_pcfgs);
			if (!pcfg)
				break;
			pcfg->public_cfg.cpu = cpu;
			list_add_tail(&pcfg->list, &unshared_pcfgs);
			cpumask_set_cpu(cpu, &unshared_cpus);
		}
	}
	/* Step 3. */
	cpumask_andnot(&slave_cpus, cpu_possible_mask, &shared_cpus);
	cpumask_andnot(&slave_cpus, &slave_cpus, &unshared_cpus);
	if (cpumask_empty(&slave_cpus)) {
		/* No sharing required */
		if (!list_empty(&shared_pcfgs)) {
			/* Migrate "shared" to "unshared" */
			cpumask_or(&unshared_cpus, &unshared_cpus,
				   &shared_cpus);
			cpumask_clear(&shared_cpus);
			list_splice_tail(&shared_pcfgs, &unshared_pcfgs);
			INIT_LIST_HEAD(&shared_pcfgs);
		}
	} else {
		/* Sharing required */
		if (list_empty(&shared_pcfgs)) {
			/* Migrate one "unshared" to "shared" */
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
	/* Step 4. */
	list_for_each_entry(pcfg, &unshared_pcfgs, list) {
		pcfg->public_cfg.is_shared = 0;
		p = init_pcfg(pcfg);
	}
	/* Step 5. */
	list_for_each_entry(pcfg, &shared_pcfgs, list) {
		pcfg->public_cfg.is_shared = 1;
		p = init_pcfg(pcfg);
		if (p)
			shared_portals[num_shared_portals++] = p;
	}
	/* Step 6. */
	if (!cpumask_empty(&slave_cpus))
		for_each_cpu(cpu, &slave_cpus)
			init_slave(cpu);
	pr_info("Portals initialised\n");
	cpumask_andnot(&offline_cpus, cpu_possible_mask, cpu_online_mask);
	for_each_cpu(cpu, &offline_cpus)
		bman_offline_cpu(cpu);

#ifdef CONFIG_HOTPLUG_CPU
	register_hotcpu_notifier(&bman_hotplug_cpu_notifier);
#endif

	bman_seed_bpid_range(0, bman_pool_max);

	return 0;
}

module_driver(bman_portal_driver,
	      bman_portal_driver_register, platform_driver_unregister);
