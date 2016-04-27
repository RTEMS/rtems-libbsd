#include <machine/rtems-bsd-kernel-space.h>

#include <rtems/bsd/local/opt_dpaa.h>

/* Copyright 2009 - 2015 Freescale Semiconductor, Inc.
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

#if defined(CONFIG_FSL_BMAN_PORTAL) ||		\
    defined(CONFIG_FSL_BMAN_PORTAL_MODULE) ||	\
    defined(CONFIG_FSL_QMAN_PORTAL) ||		\
    defined(CONFIG_FSL_QMAN_PORTAL_MODULE)
#include "dpaa_sys.h"

/* The allocator is a (possibly-empty) list of these */
struct dpaa_resource_node {
	struct list_head list;
	u32 base;
	u32 num;
	/* refcount and is_alloced are only set
	   when the node is in the used list */
	unsigned int refcount;
	int is_alloced;
};

#ifdef DPAA_RESOURCE_DEBUG
#define DPRINT pr_info
static void DUMP(struct dpaa_resource *alloc)
{
	int off = 0;
	char buf[256];
	struct dpaa_resource_node *p;

	pr_info("Free Nodes\n");
	list_for_each_entry(p, &alloc->free, list) {
		if (off < 255)
			off += snprintf(buf + off, 255-off, "{%d,%d}",
				p->base, p->base + p->num - 1);
	}
	pr_info("%s\n", buf);

	off = 0;
	pr_info("Used Nodes\n");
	list_for_each_entry(p, &alloc->used, list) {
		if (off < 255)
			off += snprintf(buf + off, 255-off, "{%d,%d}",
				p->base, p->base + p->num - 1);
	}
	pr_info("%s\n", buf);
}
#else
#define DPRINT(x...)
#define DUMP(a)
#endif

int dpaa_resource_new(struct dpaa_resource *alloc, u32 *result,
		      u32 count, u32 align, int partial)
{
	struct dpaa_resource_node *i = NULL, *next_best = NULL,
		*used_node = NULL;
	u32 base, next_best_base = 0, num = 0, next_best_num = 0;
	struct dpaa_resource_node *margin_left, *margin_right;

	*result = (u32)-1;
	DPRINT("alloc_range(%d,%d,%d)\n", count, align, partial);
	DUMP(alloc);
	/* If 'align' is 0, it should behave as though it was 1 */
	if (!align)
		align = 1;
	margin_left = kmalloc(sizeof(*margin_left), GFP_KERNEL);
	if (!margin_left)
		goto err;
	margin_right = kmalloc(sizeof(*margin_right), GFP_KERNEL);
	if (!margin_right) {
		kfree(margin_left);
		goto err;
	}
	spin_lock_irq(&alloc->lock);
	list_for_each_entry(i, &alloc->free, list) {
		base = (i->base + align - 1) / align;
		base *= align;
		if ((base - i->base) >= i->num)
			/* alignment is impossible, regardless of count */
			continue;
		num = i->num - (base - i->base);
		if (num >= count) {
			/* this one will do nicely */
			num = count;
			goto done;
		}
		if (num > next_best_num) {
			next_best = i;
			next_best_base = base;
			next_best_num = num;
		}
	}
	if (partial && next_best) {
		i = next_best;
		base = next_best_base;
		num = next_best_num;
	} else
		i = NULL;
done:
	if (i) {
		if (base != i->base) {
			margin_left->base = i->base;
			margin_left->num = base - i->base;
			list_add_tail(&margin_left->list, &i->list);
		} else
			kfree(margin_left);
		if ((base + num) < (i->base + i->num)) {
			margin_right->base = base + num;
			margin_right->num = (i->base + i->num) -
						(base + num);
			list_add(&margin_right->list, &i->list);
		} else
			kfree(margin_right);
		list_del(&i->list);
		kfree(i);
		*result = base;
	}
	spin_unlock_irq(&alloc->lock);
err:
	DPRINT("returning %d\n", i ? num : -ENOMEM);
	DUMP(alloc);
	if (!i)
		return -ENOMEM;

	/* Add the allocation to the used list with a refcount of 1 */
	used_node = kmalloc(sizeof(*used_node), GFP_KERNEL);
	if (!used_node)
		return -ENOMEM;
	used_node->base = *result;
	used_node->num = num;
	used_node->refcount = 1;
	used_node->is_alloced = 1;
	list_add_tail(&used_node->list, &alloc->used);
	return (int)num;
}
EXPORT_SYMBOL(dpaa_resource_new);

/* Allocate the list node using GFP_ATOMIC, because we *really* want to avoid
 * forcing error-handling on to users in the deallocation path. */
static void _dpaa_resource_free(struct dpaa_resource *alloc, u32 base_id,
				u32 count)
{
	struct dpaa_resource_node *i,
		*node = kmalloc(sizeof(*node), GFP_ATOMIC);

	BUG_ON(!node);
	DPRINT("release_range(%d,%d)\n", base_id, count);
	DUMP(alloc);
	BUG_ON(!count);
	spin_lock_irq(&alloc->lock);

	node->base = base_id;
	node->num = count;
	list_for_each_entry(i, &alloc->free, list) {
		if (i->base >= node->base) {
			/* BUG_ON(any overlapping) */
			BUG_ON(i->base < (node->base + node->num));
			list_add_tail(&node->list, &i->list);
			goto done;
		}
	}
	list_add_tail(&node->list, &alloc->free);
done:
	/* Merge to the left */
	i = list_entry(node->list.prev, struct dpaa_resource_node, list);
	if (node->list.prev != &alloc->free) {
		BUG_ON((i->base + i->num) > node->base);
		if ((i->base + i->num) == node->base) {
			node->base = i->base;
			node->num += i->num;
			list_del(&i->list);
			kfree(i);
		}
	}
	/* Merge to the right */
	i = list_entry(node->list.next, struct dpaa_resource_node, list);
	if (node->list.next != &alloc->free) {
		BUG_ON((node->base + node->num) > i->base);
		if ((node->base + node->num) == i->base) {
			node->num += i->num;
			list_del(&i->list);
			kfree(i);
		}
	}
	spin_unlock_irq(&alloc->lock);
	DUMP(alloc);
}

static void dpaa_resource_free(struct dpaa_resource *alloc, u32 base_id,
			       u32 count)
{
	struct dpaa_resource_node *i = NULL;

	spin_lock_irq(&alloc->lock);

	/* First find the node in the used list and decrement its ref count */
	list_for_each_entry(i, &alloc->used, list) {
		if (i->base == base_id && i->num == count) {
			--i->refcount;
			if (i->refcount == 0) {
				list_del(&i->list);
				spin_unlock_irq(&alloc->lock);
				if (i->is_alloced)
					_dpaa_resource_free(alloc, base_id,
							    count);
				kfree(i);
				return;
			}
			spin_unlock_irq(&alloc->lock);
			return;
		}
	}
	/* Couldn't find the allocation */
	pr_err("Attempt to free ID 0x%x COUNT %d that wasn't alloc'd or reserved\n",
	       base_id, count);
	spin_unlock_irq(&alloc->lock);
}

/* Same as free but no previous allocation checking is needed */
void dpaa_resource_seed(struct dpaa_resource *alloc, u32 base_id, u32 count)
{
	_dpaa_resource_free(alloc, base_id, count);
}
EXPORT_SYMBOL(dpaa_resource_seed);

/* Like 'new' but specifies the desired range, returns -ENOMEM if the entire
 * desired range is not available, or 0 for success
 */
int dpaa_resource_reserve(struct dpaa_resource *alloc, u32 base, u32 num)
{
	struct dpaa_resource_node *i = NULL, *used_node;

	DPRINT("alloc_reserve(%d,%d)\n", base, num);
	DUMP(alloc);

	spin_lock_irq(&alloc->lock);

	/* Check for the node in the used list.
	   If found, increase it's refcount */
	list_for_each_entry(i, &alloc->used, list) {
		if ((i->base == base) && (i->num == num)) {
			++i->refcount;
			spin_unlock_irq(&alloc->lock);
			return 0;
		}
		if ((base >= i->base) && (base < (i->base + i->num))) {
			/* This is an attempt to reserve a region that was
			   already reserved or alloced with a different
			   base or num */
			pr_err("Cannot reserve %d - %d, it overlaps with"
			       " existing reservation from %d - %d\n",
			       base, base + num - 1, i->base,
			       i->base + i->num - 1);
			spin_unlock_irq(&alloc->lock);
			return -1;
		}
	}
	/* Check to make sure this ID isn't in the free list */
	list_for_each_entry(i, &alloc->free, list) {
		if ((base >= i->base) && (base < (i->base + i->num))) {
			/* yep, the reservation is within this node */
			pr_err("Cannot reserve %d - %d, it overlaps with"
			       " free range %d - %d and must be alloced\n",
			       base, base + num - 1,
			       i->base, i->base + i->num - 1);
			spin_unlock_irq(&alloc->lock);
			return -1;
		}
	}
	/* Add the allocation to the used list with a refcount of 1 */
	used_node = kmalloc(sizeof(*used_node), GFP_KERNEL);
	if (!used_node) {
		spin_unlock_irq(&alloc->lock);
		return -ENOMEM;

	}
	used_node->base = base;
	used_node->num = num;
	used_node->refcount = 1;
	used_node->is_alloced = 0;
	list_add_tail(&used_node->list, &alloc->used);
	spin_unlock_irq(&alloc->lock);
	return 0;
}
EXPORT_SYMBOL(dpaa_resource_reserve);

/* This is a sort-of-conditional dpaa_resource_free() routine. Eg. when
 * releasing FQIDs (probably from user-space), it can filter out those
 * that aren't in the OOS state (better to leak a h/w resource than to
 * crash). This function returns the number of invalid IDs that were not
 * released.
*/
u32 dpaa_resource_release(struct dpaa_resource *alloc,
			  u32 id, u32 count, int (*is_valid)(u32 id))
{
	int valid_mode = 0;
	u32 loop = id, total_invalid = 0;

	while (loop < (id + count)) {
		int isvalid = is_valid ? is_valid(loop) : 1;

		if (!valid_mode) {
			/* We're looking for a valid ID to terminate an invalid
			 * range */
			if (isvalid) {
				/* We finished a range of invalid IDs, a valid
				 * range is now underway */
				valid_mode = 1;
				count -= (loop - id);
				id = loop;
			} else
				total_invalid++;
		} else {
			/* We're looking for an invalid ID to terminate a
			 * valid range */
			if (!isvalid) {
				/* Release the range of valid IDs, an unvalid
				 * range is now underway */
				if (loop > id)
					dpaa_resource_free(alloc, id,
							   loop - id);
				valid_mode = 0;
			}
		}
		loop++;
	}
	/* Release any unterminated range of valid IDs */
	if (valid_mode && count)
		dpaa_resource_free(alloc, id, count);
	return total_invalid;
}
EXPORT_SYMBOL(dpaa_resource_release);
#endif	/* CONFIG_FSL_*MAN_PORTAL* */
