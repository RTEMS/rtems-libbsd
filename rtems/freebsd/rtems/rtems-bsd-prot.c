/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief TODO.
 */

/*
 * Copyright (c) 2009, 2010 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <rtems/freebsd/machine/rtems-bsd-config.h>

#include <rtems/freebsd/sys/types.h>
#include <rtems/freebsd/sys/systm.h>
#include <rtems/freebsd/vm/uma.h>
#include <rtems/freebsd/sys/malloc.h>
#include <rtems/freebsd/sys/ucred.h>
#include <rtems/freebsd/sys/refcount.h>

static MALLOC_DEFINE(M_CRED, "cred", "credentials");

static void crextend(struct ucred *cr, int n);
static void crsetgroups_locked(struct ucred *cr, int ngrp,
    gid_t *groups);



/*
 * Allocate a zeroed cred structure.
 */
struct ucred *
crget(void)
{
  register struct ucred *cr;

  cr = malloc(sizeof(*cr), M_CRED, M_WAITOK | M_ZERO);
  refcount_init(&cr->cr_ref, 1);
#ifdef AUDIT
  audit_cred_init(cr);
#endif
#ifdef MAC
  mac_cred_init(cr);
#endif
  crextend(cr, XU_NGROUPS);
  return (cr);
}

/*
 * Claim another reference to a ucred structure.
 */
struct ucred *
crhold(struct ucred *cr)
{

  refcount_acquire(&cr->cr_ref);
  return (cr);
}

/*
 * Free a cred structure.  Throws away space when ref count gets to 0.
 */
void
crfree(struct ucred *cr)
{

  KASSERT(cr->cr_ref > 0, ("bad ucred refcount: %d", cr->cr_ref));
  KASSERT(cr->cr_ref != 0xdeadc0de, ("dangling reference to ucred"));
  if (refcount_release(&cr->cr_ref)) {
    /*
     * Some callers of crget(), such as nfs_statfs(),
     * allocate a temporary credential, but don't
     * allocate a uidinfo structure.
     */
    if (cr->cr_uidinfo != NULL)
      uifree(cr->cr_uidinfo);
    if (cr->cr_ruidinfo != NULL)
      uifree(cr->cr_ruidinfo);
    /*
     * Free a prison, if any.
     */
    if (cr->cr_prison != NULL)
      prison_free(cr->cr_prison);
#ifdef AUDIT
    audit_cred_destroy(cr);
#endif
#ifdef MAC
    mac_cred_destroy(cr);
#endif
    free(cr->cr_groups, M_CRED);
    free(cr, M_CRED);
  }
}

/*
 * Check to see if this ucred is shared.
 */
int
crshared(struct ucred *cr)
{

  return (cr->cr_ref > 1);
}

/*
 * Copy a ucred's contents from a template.  Does not block.
 */
void
crcopy(struct ucred *dest, struct ucred *src)
{

  KASSERT(crshared(dest) == 0, ("crcopy of shared ucred"));
  bcopy(&src->cr_startcopy, &dest->cr_startcopy,
      (unsigned)((caddr_t)&src->cr_endcopy -
    (caddr_t)&src->cr_startcopy));
  crsetgroups(dest, src->cr_ngroups, src->cr_groups);
  uihold(dest->cr_uidinfo);
  uihold(dest->cr_ruidinfo);
  prison_hold(dest->cr_prison);
#ifdef AUDIT
  audit_cred_copy(src, dest);
#endif
#ifdef MAC
  mac_cred_copy(src, dest);
#endif
}

/*
 * Dup cred struct to a new held one.
 */
struct ucred *
crdup(struct ucred *cr)
{
  struct ucred *newcr;

  newcr = crget();
  crcopy(newcr, cr);
  return (newcr);
}

/*
 * Extend the passed in credential to hold n items.
 */
static void
crextend(struct ucred *cr, int n)
{
	int cnt;

	/* Truncate? */
	if (n <= cr->cr_agroups)
		return;

	/*
	 * We extend by 2 each time since we're using a power of two
	 * allocator until we need enough groups to fill a page.
	 * Once we're allocating multiple pages, only allocate as many
	 * as we actually need.  The case of processes needing a
	 * non-power of two number of pages seems more likely than
	 * a real world process that adds thousands of groups one at a
	 * time.
	 */
	if ( n < PAGE_SIZE / sizeof(gid_t) ) {
		if (cr->cr_agroups == 0)
			cnt = MINALLOCSIZE / sizeof(gid_t);
		else
			cnt = cr->cr_agroups * 2;

		while (cnt < n)
			cnt *= 2;
	} else
		cnt = roundup2(n, PAGE_SIZE / sizeof(gid_t));

	/* Free the old array. */
	if (cr->cr_groups)
		free(cr->cr_groups, M_CRED);

	cr->cr_groups = malloc(cnt * sizeof(gid_t), M_CRED, M_WAITOK | M_ZERO);
	cr->cr_agroups = cnt;
}

/*
 * Copy groups in to a credential, preserving any necessary invariants.
 * Currently this includes the sorting of all supplemental gids.
 * crextend() must have been called before hand to ensure sufficient
 * space is available.
 */
static void
crsetgroups_locked(struct ucred *cr, int ngrp, gid_t *groups)
{
	int i;
	int j;
	gid_t g;
	
	KASSERT(cr->cr_agroups >= ngrp, ("cr_ngroups is too small"));

	bcopy(groups, cr->cr_groups, ngrp * sizeof(gid_t));
	cr->cr_ngroups = ngrp;

	/*
	 * Sort all groups except cr_groups[0] to allow groupmember to
	 * perform a binary search.
	 *
	 * XXX: If large numbers of groups become common this should
	 * be replaced with shell sort like linux uses or possibly
	 * heap sort.
	 */
	for (i = 2; i < ngrp; i++) {
		g = cr->cr_groups[i];
		for (j = i-1; j >= 1 && g < cr->cr_groups[j]; j--)
			cr->cr_groups[j + 1] = cr->cr_groups[j];
		cr->cr_groups[j + 1] = g;
	}
}

/*
 * Copy groups in to a credential after expanding it if required.
 * Truncate the list to (ngroups_max + 1) if it is too large.
 */
void
crsetgroups(struct ucred *cr, int ngrp, gid_t *groups)
{

	if (ngrp > ngroups_max + 1)
		ngrp = ngroups_max + 1;

	crextend(cr, ngrp);
	crsetgroups_locked(cr, ngrp, groups);
}

