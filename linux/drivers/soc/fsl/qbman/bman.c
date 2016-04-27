#include <machine/rtems-bsd-kernel-space.h>

#include <rtems/bsd/local/opt_dpaa.h>

/* Copyright (c) 2009 - 2015 Freescale Semiconductor, Inc.
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

/* Last updated for v00.79 of the BG */

struct bman;

/* Register offsets */
#define REG_POOL_SWDET(n)	(0x0000 + ((n) * 0x04))
#define REG_POOL_HWDET(n)	(0x0100 + ((n) * 0x04))
#define REG_POOL_SWDXT(n)	(0x0200 + ((n) * 0x04))
#define REG_POOL_HWDXT(n)	(0x0300 + ((n) * 0x04))
#define REG_POOL_CONTENT(n)	(0x0600 + ((n) * 0x04))
#define REG_FBPR_FPC		0x0800
#define REG_ECSR		0x0a00
#define REG_ECIR		0x0a04
#define REG_EADR		0x0a08
#define REG_EDATA(n)		(0x0a10 + ((n) * 0x04))
#define REG_SBEC(n)		(0x0a80 + ((n) * 0x04))
#define REG_IP_REV_1		0x0bf8
#define REG_IP_REV_2		0x0bfc
#define REG_FBPR_BARE		0x0c00
#define REG_FBPR_BAR		0x0c04
#define REG_FBPR_AR		0x0c10
#define REG_SRCIDR		0x0d04
#define REG_LIODNR		0x0d08
#define REG_ERR_ISR		0x0e00	/* + "enum bm_isr_reg" */

/* Used by all error interrupt registers except 'inhibit' */
#define BM_EIRQ_IVCI	0x00000010	/* Invalid Command Verb */
#define BM_EIRQ_FLWI	0x00000008	/* FBPR Low Watermark */
#define BM_EIRQ_MBEI	0x00000004	/* Multi-bit ECC Error */
#define BM_EIRQ_SBEI	0x00000002	/* Single-bit ECC Error */
#define BM_EIRQ_BSCN	0x00000001	/* pool State Change Notification */

/* BMAN_ECIR valid error bit */
#define PORTAL_ECSR_ERR	(BM_EIRQ_IVCI)

union bman_ecir {
	u32 ecir_raw;
	struct {
		u32 __reserved1:4;
		u32 portal_num:4;
		u32 __reserved2:12;
		u32 numb:4;
		u32 __reserved3:2;
		u32 pid:6;
	} __packed info;
};

union bman_eadr {
	u32 eadr_raw;
	struct {
		u32 __reserved1:5;
		u32 memid:3;
		u32 __reserved2:14;
		u32 eadr:10;
	} __packed info;
};

struct bman_hwerr_txt {
	u32 mask;
	const char *txt;
};

#define BMAN_HWE_TXT(a, b) { .mask = BM_EIRQ_##a, .txt = b }

static const struct bman_hwerr_txt bman_hwerr_txts[] = {
	BMAN_HWE_TXT(IVCI, "Invalid Command Verb"),
	BMAN_HWE_TXT(FLWI, "FBPR Low Watermark"),
	BMAN_HWE_TXT(MBEI, "Multi-bit ECC Error"),
	BMAN_HWE_TXT(SBEI, "Single-bit ECC Error"),
	BMAN_HWE_TXT(BSCN, "Pool State Change Notification"),
};
#define BMAN_HWE_COUNT (sizeof(bman_hwerr_txts)/sizeof(struct bman_hwerr_txt))

struct bman_error_info_mdata {
	u16 addr_mask;
	u16 bits;
	const char *txt;
};

#define BMAN_ERR_MDATA(a, b, c) { .addr_mask = a, .bits = b, .txt = c}
static const struct bman_error_info_mdata error_mdata[] = {
	BMAN_ERR_MDATA(0x03FF, 192, "Stockpile memory"),
	BMAN_ERR_MDATA(0x00FF, 256, "SW portal ring memory port 1"),
	BMAN_ERR_MDATA(0x00FF, 256, "SW portal ring memory port 2"),
};
#define BMAN_ERR_MDATA_COUNT \
	(sizeof(error_mdata)/sizeof(struct bman_error_info_mdata))

/* Add this in Kconfig */
#define BMAN_ERRS_TO_UNENABLE (BM_EIRQ_FLWI)

/**
 * bm_err_isr_<reg>_<verb> - Manipulate global interrupt registers
 * @v: for accessors that write values, this is the 32-bit value
 *
 * Manipulates BMAN_ERR_ISR, BMAN_ERR_IER, BMAN_ERR_ISDR, BMAN_ERR_IIR. All
 * manipulations except bm_err_isr_[un]inhibit() use 32-bit masks composed of
 * the BM_EIRQ_*** definitions. Note that "bm_err_isr_enable_write" means
 * "write the enable register" rather than "enable the write register"!
 */
#define bm_err_isr_status_read(bm)	\
		__bm_err_isr_read(bm, bm_isr_status)
#define bm_err_isr_status_clear(bm, m)	\
		__bm_err_isr_write(bm, bm_isr_status, m)
#define bm_err_isr_enable_read(bm)	\
		__bm_err_isr_read(bm, bm_isr_enable)
#define bm_err_isr_enable_write(bm, v)	\
		__bm_err_isr_write(bm, bm_isr_enable, v)
#define bm_err_isr_disable_read(bm)	\
		__bm_err_isr_read(bm, bm_isr_disable)
#define bm_err_isr_disable_write(bm, v)	\
		__bm_err_isr_write(bm, bm_isr_disable, v)
#define bm_err_isr_inhibit(bm)		\
		__bm_err_isr_write(bm, bm_isr_inhibit, 1)
#define bm_err_isr_uninhibit(bm)	\
		__bm_err_isr_write(bm, bm_isr_inhibit, 0)

#ifndef __rtems__
static u16 bman_pool_max;
#else /* __rtems__ */
/* FIXME */
extern u16 bman_ip_rev;
extern u16 bman_pool_max;
#endif /* __rtems__ */

/*
 * TODO: unimplemented registers
 *
 * BMAN_POOLk_SDCNT, BMAN_POOLk_HDCNT, BMAN_FULT,
 * BMAN_VLDPL, BMAN_EECC, BMAN_SBET, BMAN_EINJ
 */

/* Encapsulate "struct bman *" as a cast of the register space address. */

static struct bman *bm_create(void *regs)
{
	return (struct bman *)regs;
}

static inline u32 __bm_in(struct bman *bm, u32 offset)
{
	return ioread32be((void *)bm + offset);
}
static inline void __bm_out(struct bman *bm, u32 offset, u32 val)
{
	iowrite32be(val, (void*) bm + offset);
}
#define bm_in(reg)		__bm_in(bm, REG_##reg)
#define bm_out(reg, val)	__bm_out(bm, REG_##reg, val)

static u32 __bm_err_isr_read(struct bman *bm, enum bm_isr_reg n)
{
	return __bm_in(bm, REG_ERR_ISR + (n << 2));
}

static void __bm_err_isr_write(struct bman *bm, enum bm_isr_reg n, u32 val)
{
	__bm_out(bm, REG_ERR_ISR + (n << 2), val);
}

static void bm_get_version(struct bman *bm, u16 *id, u8 *major, u8 *minor)
{
	u32 v = bm_in(IP_REV_1);
	*id = (v >> 16);
	*major = (v >> 8) & 0xff;
	*minor = v & 0xff;
}

static u32 __generate_thresh(u32 val, int roundup)
{
	u32 e = 0;	/* co-efficient, exponent */
	int oddbit = 0;

	while (val > 0xff) {
		oddbit = val & 1;
		val >>= 1;
		e++;
		if (roundup && oddbit)
			val++;
	}
	DPA_ASSERT(e < 0x10);
	return val | (e << 8);
}

static void bm_set_pool(struct bman *bm, u8 pool, u32 swdet, u32 swdxt,
			u32 hwdet, u32 hwdxt)
{
	DPA_ASSERT(pool < bman_pool_max);

	bm_out(POOL_SWDET(pool), __generate_thresh(swdet, 0));
	bm_out(POOL_SWDXT(pool), __generate_thresh(swdxt, 1));
	bm_out(POOL_HWDET(pool), __generate_thresh(hwdet, 0));
	bm_out(POOL_HWDXT(pool), __generate_thresh(hwdxt, 1));
}

static void bm_set_memory(struct bman *bm, u64 ba, int prio, u32 size)
{
	u32 exp = ilog2(size);
	/* choke if size isn't within range */
	DPA_ASSERT((size >= 4096) && (size <= 1073741824) &&
			is_power_of_2(size));
	/* choke if '[e]ba' has lower-alignment than 'size' */
	DPA_ASSERT(!(ba & (size - 1)));
	bm_out(FBPR_BARE, upper_32_bits(ba));
	bm_out(FBPR_BAR, lower_32_bits(ba));
	bm_out(FBPR_AR, (prio ? 0x40000000 : 0) | (exp - 1));
}

/*****************/
/* Config driver */
/*****************/

/* We support only one of these. */
static struct bman *bm;

/* And this state belongs to 'bm' */
#ifndef __rtems__
static dma_addr_t fbpr_a;
static size_t fbpr_sz;

static int bman_fbpr(struct reserved_mem *rmem)
{
	fbpr_a = rmem->base;
	fbpr_sz = rmem->size;

	WARN_ON(!(fbpr_a && fbpr_sz));

	return 0;
}
RESERVEDMEM_OF_DECLARE(bman_fbpr, "fsl,bman-fbpr", bman_fbpr);
#else /* __rtems__ */
static DPAA_NOCACHENOLOAD_ALIGNED_REGION(fbpr, 16777216);
#define fbpr_a ((uintptr_t)&fbpr[0])
#define fbpr_sz sizeof(fbpr)
#endif /* __rtems__ */

int bm_pool_set(u32 bpid, const u32 *thresholds)
{
	if (!bm)
		return -ENODEV;
	bm_set_pool(bm, bpid, thresholds[0], thresholds[1],
		thresholds[2], thresholds[3]);
	return 0;
}
EXPORT_SYMBOL(bm_pool_set);

static void log_edata_bits(u32 bit_count)
{
	u32 i, j, mask = 0xffffffff;

	pr_warn("ErrInt, EDATA:\n");
	i = bit_count/32;
	if (bit_count%32) {
		i++;
		mask = ~(mask << bit_count%32);
	}
	j = 16-i;
	pr_warn("  0x%08x\n", bm_in(EDATA(j)) & mask);
	j++;
	for (; j < 16; j++)
		pr_warn("  0x%08x\n", bm_in(EDATA(j)));
}

static void log_additional_error_info(u32 isr_val, u32 ecsr_val)
{
	union bman_ecir ecir_val;
	union bman_eadr eadr_val;

	ecir_val.ecir_raw = bm_in(ECIR);
	/* Is portal info valid */
	if (ecsr_val & PORTAL_ECSR_ERR) {
		pr_warn("ErrInt: SWP id %d, numb %d, pid %d\n",
			ecir_val.info.portal_num, ecir_val.info.numb,
			ecir_val.info.pid);
	}
	if (ecsr_val & (BM_EIRQ_SBEI|BM_EIRQ_MBEI)) {
		eadr_val.eadr_raw = bm_in(EADR);
		pr_warn("ErrInt: EADR Memory: %s, 0x%x\n",
			error_mdata[eadr_val.info.memid].txt,
			error_mdata[eadr_val.info.memid].addr_mask
				& eadr_val.info.eadr);
		log_edata_bits(error_mdata[eadr_val.info.memid].bits);
	}
}

/* BMan interrupt handler */
static irqreturn_t bman_isr(int irq, void *ptr)
{
	u32 isr_val, ier_val, ecsr_val, isr_mask, i;

	ier_val = bm_err_isr_enable_read(bm);
	isr_val = bm_err_isr_status_read(bm);
	ecsr_val = bm_in(ECSR);
	isr_mask = isr_val & ier_val;

	if (!isr_mask)
		return IRQ_NONE;

	for (i = 0; i < BMAN_HWE_COUNT; i++) {
		if (bman_hwerr_txts[i].mask & isr_mask) {
			pr_warn("ErrInt: %s\n", bman_hwerr_txts[i].txt);
			if (bman_hwerr_txts[i].mask & ecsr_val) {
				log_additional_error_info(isr_mask, ecsr_val);
				/* Re-arm error capture registers */
				bm_out(ECSR, ecsr_val);
			}
			if (bman_hwerr_txts[i].mask & BMAN_ERRS_TO_UNENABLE) {
				pr_devel("Un-enabling error 0x%x\n",
					bman_hwerr_txts[i].mask);
				ier_val &= ~bman_hwerr_txts[i].mask;
				bm_err_isr_enable_write(bm, ier_val);
			}
		}
	}
	bm_err_isr_status_clear(bm, isr_val);

	return IRQ_HANDLED;
}

u32 bm_pool_free_buffers(u32 bpid)
{
	return bm_in(POOL_CONTENT(bpid));
}
EXPORT_SYMBOL(bm_pool_free_buffers);

#ifndef __rtems__
static ssize_t show_fbpr_fpc(struct device *dev,
	struct device_attribute *dev_attr, char *buf)
{
	return snprintf(buf, PAGE_SIZE, "%u\n", bm_in(FBPR_FPC));
};

static ssize_t show_pool_count(struct device *dev,
	struct device_attribute *dev_attr, char *buf)
{
	u32 data;
	int i;

	if (kstrtoint(dev_attr->attr.name, 10, &i))
		return -EINVAL;
	data = bm_in(POOL_CONTENT(i));
	return snprintf(buf, PAGE_SIZE, "%d\n", data);
};

static ssize_t show_err_isr(struct device *dev,
	struct device_attribute *dev_attr, char *buf)
{
	return snprintf(buf, PAGE_SIZE, "0x%08x\n", bm_in(ERR_ISR));
};

static ssize_t show_sbec(struct device *dev,
	struct device_attribute *dev_attr, char *buf)
{
	int i;

	if (sscanf(dev_attr->attr.name, "sbec_%d", &i) != 1)
		return -EINVAL;
	return snprintf(buf, PAGE_SIZE, "%u\n", bm_in(SBEC(i)));
};

static DEVICE_ATTR(err_isr, S_IRUSR, show_err_isr, NULL);
static DEVICE_ATTR(fbpr_fpc, S_IRUSR, show_fbpr_fpc, NULL);

/* Didn't use DEVICE_ATTR as 64 of this would be required.
 * Initialize them when needed. */
static char *name_attrs_pool_count; /* "xx" + null-terminator */
static struct device_attribute *dev_attr_buffer_pool_count;

static DEVICE_ATTR(sbec_0, S_IRUSR, show_sbec, NULL);
static DEVICE_ATTR(sbec_1, S_IRUSR, show_sbec, NULL);

static struct attribute *bman_dev_attributes[] = {
	&dev_attr_fbpr_fpc.attr,
	&dev_attr_err_isr.attr,
	NULL
};

static struct attribute *bman_dev_ecr_attributes[] = {
	&dev_attr_sbec_0.attr,
	&dev_attr_sbec_1.attr,
	NULL
};

static struct attribute **bman_dev_pool_count_attributes;

/* root level */
static const struct attribute_group bman_dev_attr_grp = {
	.name = NULL,
	.attrs = bman_dev_attributes
};
static const struct attribute_group bman_dev_ecr_grp = {
	.name = "error_capture",
	.attrs = bman_dev_ecr_attributes
};
static struct attribute_group bman_dev_pool_countent_grp = {
	.name = "pool_count",
};

static int of_fsl_bman_remove(struct platform_device *ofdev)
{
	sysfs_remove_group(&ofdev->dev.kobj, &bman_dev_attr_grp);
	return 0;
};
#endif /* __rtems__ */

static int of_fsl_bman_probe(struct platform_device *ofdev)
{
	int ret, err_irq, i;
	struct device *dev = &ofdev->dev;
	struct device_node *node = dev->of_node;
	struct resource res;
	u32 __iomem *regs;
	u16 id;
	u8 major, minor;

	if (!of_device_is_available(node))
		return -ENODEV;

	ret = of_address_to_resource(node, 0, &res);
	if (ret) {
		dev_err(dev, "Can't get %s property 'reg'\n", node->full_name);
		return ret;
	}
	regs = devm_ioremap(dev, res.start, res.end - res.start + 1);
	if (!regs)
		return -ENXIO;

	bm = bm_create(regs);

	bm_get_version(bm, &id, &major, &minor);
	dev_info(dev, "Bman ver:%04x,%02x,%02x\n", id, major, minor);
	if ((major == 1) && (minor == 0))
		bman_pool_max = 64;
	else if ((major == 2) && (minor == 0))
		bman_pool_max = 8;
	else if ((major == 2) && (minor == 1))
		bman_pool_max = 64;
	else
		dev_warn(dev, "unknown Bman version, default to rev1.0\n");
#ifdef __rtems__
	bman_ip_rev = (u16)((major << 8) | minor);
#endif /* __rtems__ */


	bm_set_memory(bm, fbpr_a, 0, fbpr_sz);

	err_irq = of_irq_to_resource(node, 0, NULL);
	if (err_irq == NO_IRQ) {
		dev_info(dev, "Can't get %s property 'interrupts'\n",
			 node->full_name);
		return -ENODEV;
	}
	ret = devm_request_irq(dev, err_irq, bman_isr, IRQF_SHARED, "bman-err",
			       node);
	if (ret)  {
		dev_err(dev, "devm_request_irq() failed %d for '%s'\n",
			ret, node->full_name);
		return ret;
	}
	/* Disable Buffer Pool State Change */
	bm_err_isr_disable_write(bm, BM_EIRQ_BSCN);
	/* Write-to-clear any stale bits, (eg. starvation being asserted prior
	 * to resource allocation during driver init). */
	bm_err_isr_status_clear(bm, 0xffffffff);
	/* Enable Error Interrupts */
	bm_err_isr_enable_write(bm, 0xffffffff);

#ifndef __rtems__
	ret = sysfs_create_group(&dev->kobj, &bman_dev_attr_grp);
	if (ret)
		goto done;
	ret = sysfs_create_group(&dev->kobj, &bman_dev_ecr_grp);
	if (ret)
		goto del_group_0;

	name_attrs_pool_count = devm_kmalloc(dev,
		sizeof(char) * bman_pool_max * 3, GFP_KERNEL);
	if (!name_attrs_pool_count)
		goto del_group_1;

	dev_attr_buffer_pool_count = devm_kmalloc(dev,
		sizeof(struct device_attribute) * bman_pool_max, GFP_KERNEL);
	if (!dev_attr_buffer_pool_count)
		goto del_group_1;

	bman_dev_pool_count_attributes = devm_kmalloc(dev,
		sizeof(struct attribute *) * (bman_pool_max + 1), GFP_KERNEL);
	if (!bman_dev_pool_count_attributes)
		goto del_group_1;

	for (i = 0; i < bman_pool_max; i++) {
		ret = scnprintf((name_attrs_pool_count + i * 3), 3, "%d", i);
		if (!ret)
			goto del_group_1;
		dev_attr_buffer_pool_count[i].attr.name =
			(name_attrs_pool_count + i * 3);
		dev_attr_buffer_pool_count[i].attr.mode = S_IRUSR;
		dev_attr_buffer_pool_count[i].show = show_pool_count;
		bman_dev_pool_count_attributes[i] =
			&dev_attr_buffer_pool_count[i].attr;
	}
	bman_dev_pool_count_attributes[bman_pool_max] = NULL;

	bman_dev_pool_countent_grp.attrs = bman_dev_pool_count_attributes;

	ret = sysfs_create_group(&dev->kobj, &bman_dev_pool_countent_grp);
	if (ret)
		goto del_group_1;

	goto done;

del_group_1:
	sysfs_remove_group(&dev->kobj, &bman_dev_ecr_grp);
del_group_0:
	sysfs_remove_group(&dev->kobj, &bman_dev_attr_grp);
done:
	if (ret)
		dev_err(dev, "Cannot create dev attributes ret=%d\n", ret);
#else /* __rtems__ */
	(void)i;
#endif /* __rtems__ */

	return ret;
};

#ifndef __rtems__
static const struct of_device_id of_fsl_bman_ids[] = {
	{
		.compatible = "fsl,bman",
	},
	{}
};

static struct platform_driver of_fsl_bman_driver = {
	.driver = {
		.name = KBUILD_MODNAME,
		.of_match_table = of_fsl_bman_ids,
	},
	.probe = of_fsl_bman_probe,
	.remove = of_fsl_bman_remove,
};

builtin_platform_driver(of_fsl_bman_driver);
#else /* __rtems__ */
#include <sys/types.h>
#include <sys/kernel.h>
#include <rtems.h>
#include <bsp/fdt.h>
#include <bsp/qoriq.h>

static struct bm_portal_config bman_configs[NR_CPUS];

u16 bman_ip_rev;

u16 bman_pool_max;

SYSINIT_REFERENCE(irqs);

static void
bman_sysinit(void)
{
	const char *fdt = bsp_fdt_get();
	struct device_node dn;
	struct platform_device ofdev = {
		.dev = {
			.of_node = &dn,
			.base = (uintptr_t)&qoriq
		}
	};
	const char *name;
	int cpu_count = (int)rtems_get_processor_count();
	int cpu;
	int ret;
	int node;
	int parent;

	qoriq_reset_qman_and_bman();
	qoriq_clear_ce_portal(&qoriq_bman_portal[0][0],
	    sizeof(qoriq_bman_portal[0]));
	qoriq_clear_ci_portal(&qoriq_bman_portal[1][0],
	    sizeof(qoriq_bman_portal[1]));

	memset(&dn, 0, sizeof(dn));

	name = "fsl,bman";
	node = fdt_node_offset_by_compatible(fdt, 0, name);
	if (node < 0)
		panic("bman: no bman in FDT");

	dn.full_name = name;
	dn.offset = node;
	ret = of_fsl_bman_probe(&ofdev);
	if (ret != 0)
		panic("bman: probe failed");

	name = "fsl,bman-portal";
	node = fdt_node_offset_by_compatible(fdt, 0, name);
	if (node < 0)
		panic("bman: no portals in FDT");
	parent = fdt_parent_offset(fdt, node);
	if (parent < 0)
		panic("bman: no parent of portals in FDT");
	node = fdt_first_subnode(fdt, parent);

	dn.full_name = name;
	dn.offset = node;

	for (cpu = 0; cpu < cpu_count; ++cpu) {
		struct bm_portal_config *pcfg = &bman_configs[cpu];
		struct bman_portal *portal;
		struct resource res;

		if (node < 0)
			panic("bman: missing portal in FDT");

		ret = of_address_to_resource(&dn, 0, &res);
		if (ret != 0)
			panic("bman: no portal CE address");
		pcfg->addr_virt[0] = (__iomem void *)
		    ((uintptr_t)&qoriq_bman_portal[0][0] + (uintptr_t)res.start);
		BSD_ASSERT((uintptr_t)pcfg->addr_virt[0] >=
		    (uintptr_t)&qoriq_bman_portal[0][0]);
		BSD_ASSERT((uintptr_t)pcfg->addr_virt[0] <
		    (uintptr_t)&qoriq_bman_portal[1][0]);

		ret = of_address_to_resource(&dn, 1, &res);
		if (ret != 0)
			panic("bman: no portal CI address");
		pcfg->addr_virt[1] = (__iomem void *)
		    ((uintptr_t)&qoriq_bman_portal[0][0] + (uintptr_t)res.start);
		BSD_ASSERT((uintptr_t)pcfg->addr_virt[1] >=
		    (uintptr_t)&qoriq_bman_portal[1][0]);
		BSD_ASSERT((uintptr_t)pcfg->addr_virt[1] <
		    (uintptr_t)&qoriq_bman_portal[2][0]);

		pcfg->public_cfg.irq = of_irq_to_resource(&dn, 0, NULL);
		if (pcfg->public_cfg.irq == NO_IRQ)
			panic("bman: no portal interrupt");

		pcfg->public_cfg.cpu = cpu;
		bman_depletion_fill(&pcfg->public_cfg.mask);

		portal = bman_create_affine_portal(pcfg);
		if (portal == NULL)
			panic("bman: cannot create portal");

		bman_p_irqsource_add(portal, BM_PIRQ_RCRI | BM_PIRQ_BSCN);

		node = fdt_next_subnode(fdt, node);
		dn.offset = node;
	}

	bman_seed_bpid_range(0, bman_pool_max);
}
SYSINIT(bman_sysinit, SI_SUB_CPU, SI_ORDER_FIRST, bman_sysinit, NULL);
#endif /* __rtems__ */
