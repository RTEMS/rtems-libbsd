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

#ifndef __DPAA_SYS_H
#define __DPAA_SYS_H

#include <linux/slab.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/of_reserved_mem.h>
#include <linux/kthread.h>
#include <linux/uaccess.h>
#include <linux/debugfs.h>
#include <linux/vmalloc.h>
#include <linux/platform_device.h>
#include <linux/ctype.h>
#ifdef CONFIG_HOTPLUG_CPU
#include <linux/cpu.h>
#endif

#include <asm/pgtable.h>
#ifdef __rtems__
#include <asm/cache.h>
#include <asm/mpc85xx.h>
#include <linux/completion.h>
#include <linux/io.h>
#include <linux/rbtree.h>
#include <bsp/linker-symbols.h>
#define	DPAA_NOCACHENOLOAD_ALIGNED_REGION(designator, size) \
    BSP_NOCACHENOLOAD_SUBSECTION(designator) __aligned(size) \
    uint8_t designator[size]
#endif /* __rtems__ */

struct dpaa_resource {
	struct list_head free;
	spinlock_t lock;
	struct list_head used;
};

#define DECLARE_DPAA_RESOURCE(name)			\
struct dpaa_resource name = {				\
	.free = {					\
		.prev = &name.free,			\
		.next = &name.free			\
	},						\
	.lock = __SPIN_LOCK_UNLOCKED(name.lock),	\
	.used = {					\
		 .prev = &name.used,			\
		 .next = &name.used			\
	}						\
}

int dpaa_resource_new(struct dpaa_resource *alloc, u32 *result,
		      u32 count, u32 align, int partial);
u32 dpaa_resource_release(struct dpaa_resource *alloc,
			  u32 id, u32 count, int (*is_valid)(u32 id));
void dpaa_resource_seed(struct dpaa_resource *alloc, u32 base_id, u32 count);
int dpaa_resource_reserve(struct dpaa_resource *alloc, u32 base, u32 num);

/* When copying aligned words or shorts, try to avoid memcpy() */
#define CONFIG_TRY_BETTER_MEMCPY

/* For 2-element tables related to cache-inhibited and cache-enabled mappings */
#define DPA_PORTAL_CE 0
#define DPA_PORTAL_CI 1

/* Misc inline assists */

/* TODO: NB, we currently assume that hwsync() and lwsync() imply compiler
 * barriers and that dcb*() won't fall victim to compiler or execution
 * reordering with respect to other code/instructions that manipulate the same
 * cacheline. */
#define hwsync() __asm__ __volatile__ ("sync" : : : "memory")
#ifndef __rtems__
#define lwsync() __asm__ __volatile__ (stringify_in_c(LWSYNC) : : : "memory")
#else /* __rtems__ */
  #ifdef __PPC_CPU_E6500__
    #define lwsync() ppc_light_weight_synchronize()
  #else
    #define lwsync() ppc_synchronize_data()
  #endif
#endif /* __rtems__ */
#define dcbf(p) __asm__ __volatile__ ("dcbf 0,%0" : : "r" (p) : "memory")
#define dcbt_ro(p) __asm__ __volatile__ ("dcbt 0,%0" : : "r" (p))
#define dcbt_rw(p) __asm__ __volatile__ ("dcbtst 0,%0" : : "r" (p))
#define dcbi(p) dcbf(p)
#ifdef CONFIG_PPC_E500MC
#define dcbzl(p) __asm__ __volatile__ ("dcbzl 0,%0" : : "r" (p))
#define dcbz_64(p) dcbzl(p)
#define dcbf_64(p) dcbf(p)
/* Commonly used combo */
#define dcbit_ro(p) \
	do { \
		dcbi(p); \
		dcbt_ro(p); \
	} while (0)
#else
#define dcbz(p) __asm__ __volatile__ ("dcbz 0,%0" : : "r" (p))
#define dcbz_64(p) \
	do { \
		dcbz((u32)p + 32);	\
		dcbz(p);	\
	} while (0)
#define dcbf_64(p) \
	do { \
		dcbf((u32)p + 32); \
		dcbf(p); \
	} while (0)
/* Commonly used combo */
#define dcbit_ro(p) \
	do { \
		dcbi(p); \
		dcbi((u32)p + 32); \
		dcbt_ro(p); \
		dcbt_ro((u32)p + 32); \
	} while (0)
#endif /* CONFIG_PPC_E500MC */

static inline u64 mfatb(void)
{
	u32 hi, lo, chk;

	do {
		hi = mfspr(SPRN_ATBU);
		lo = mfspr(SPRN_ATBL);
		chk = mfspr(SPRN_ATBU);
	} while (unlikely(hi != chk));
	return ((u64)hi << 32) | (u64)lo;
}

#ifdef CONFIG_FSL_DPA_CHECKING
#define DPA_ASSERT(x) WARN_ON(!(x))
#else
#define DPA_ASSERT(x)
#endif

#ifdef CONFIG_TRY_BETTER_MEMCPY
static inline void copy_words(void *dest, const void *src, size_t sz)
{
	u32 *__dest = dest;
	const u32 *__src = src;
	size_t __sz = sz >> 2;

	BUG_ON((unsigned long)dest & 0x3);
	BUG_ON((unsigned long)src & 0x3);
	BUG_ON(sz & 0x3);
	while (__sz--)
		*(__dest++) = *(__src++);
}
#else
#define copy_words memcpy
#endif

/* RB-trees */

/* We encapsulate RB-trees so that its easier to use non-linux forms in
 * non-linux systems. This also encapsulates the extra plumbing that linux code
 * usually provides when using RB-trees. This encapsulation assumes that the
 * data type held by the tree is u32. */

struct dpa_rbtree {
	struct rb_root root;
};
#define DPA_RBTREE { .root = RB_ROOT }

static inline void dpa_rbtree_init(struct dpa_rbtree *tree)
{
	tree->root = RB_ROOT;
}

#define IMPLEMENT_DPA_RBTREE(name, type, node_field, val_field) \
static inline int name##_push(struct dpa_rbtree *tree, type *obj) \
{ \
	struct rb_node *parent = NULL, **p = &tree->root.rb_node; \
	while (*p) { \
		u32 item; \
		parent = *p; \
		item = rb_entry(parent, type, node_field)->val_field; \
		if (obj->val_field < item) \
			p = &parent->rb_left; \
		else if (obj->val_field > item) \
			p = &parent->rb_right; \
		else \
			return -EBUSY; \
	} \
	rb_link_node(&obj->node_field, parent, p); \
	rb_insert_color(&obj->node_field, &tree->root); \
	return 0; \
} \
static inline void name##_del(struct dpa_rbtree *tree, type *obj) \
{ \
	rb_erase(&obj->node_field, &tree->root); \
} \
static inline type *name##_find(struct dpa_rbtree *tree, u32 val) \
{ \
	type *ret; \
	struct rb_node *p = tree->root.rb_node; \
	while (p) { \
		ret = rb_entry(p, type, node_field); \
		if (val < ret->val_field) \
			p = p->rb_left; \
		else if (val > ret->val_field) \
			p = p->rb_right; \
		else \
			return ret; \
	} \
	return NULL; \
}

#ifndef __rtems__
/* Bootargs */

/* QMan has "qportals=" and BMan has "bportals=", they use the same syntax
 * though; a comma-separated list of items, each item being a cpu index and/or a
 * range of cpu indices, and each item optionally be prefixed by "s" to indicate
 * that the portal associated with that cpu should be shared. See bman_driver.c
 * for more specifics. */
static int __parse_portals_cpu(const char **s, unsigned int *cpu)
{
	*cpu = 0;
	if (!isdigit(**s))
		return -EINVAL;
	while (isdigit(**s))
		*cpu = *cpu * 10 + (*((*s)++) - '0');
	return 0;
}
static inline int parse_portals_bootarg(char *str, struct cpumask *want_shared,
					struct cpumask *want_unshared,
					const char *argname)
{
	const char *s = str;
	unsigned int shared, cpu1, cpu2, loop;

keep_going:
	if (*s == 's') {
		shared = 1;
		s++;
	} else
		shared = 0;
	if (__parse_portals_cpu(&s, &cpu1))
		goto err;
	if (*s == '-') {
		s++;
		if (__parse_portals_cpu(&s, &cpu2))
			goto err;
		if (cpu2 < cpu1)
			goto err;
	} else
		cpu2 = cpu1;
	for (loop = cpu1; loop <= cpu2; loop++)
		cpumask_set_cpu(loop, shared ? want_shared : want_unshared);
	if (*s == ',') {
		s++;
		goto keep_going;
	} else if ((*s == '\0') || isspace(*s))
		return 0;
err:
	pr_crit("Malformed %s argument: %s, offset: %lu\n", argname, str,
		(unsigned long)s - (unsigned long)str);
	return -EINVAL;
}
#endif /* __rtems__ */
#endif	/* __DPAA_SYS_H */
