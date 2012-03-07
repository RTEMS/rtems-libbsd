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
#include <rtems/freebsd/sys/malloc.h>
#include <rtems/freebsd/sys/ucred.h>

static MALLOC_DEFINE(M_CRED, "cred", "credentials");

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
