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

#include <freebsd/machine/rtems-bsd-config.h>

#include <freebsd/sys/types.h>
#include <freebsd/sys/systm.h>
#include <freebsd/sys/lock.h>
#include <freebsd/sys/malloc.h>
#include <freebsd/sys/mutex.h>
#include <freebsd/sys/proc.h>
#include <freebsd/sys/resourcevar.h>
#include <freebsd/sys/rwlock.h>
#include <freebsd/sys/refcount.h>

static MALLOC_DEFINE(M_UIDINFO, "uidinfo", "uidinfo structures");

#define UIHASH(uid) (&uihashtbl[(uid) & uihash])
static struct rwlock uihashtbl_lock;
static LIST_HEAD(uihashhead, uidinfo) *uihashtbl;
static u_long uihash;   /* size of hash table - 1 */

/*
 * Find the uidinfo structure for a uid.  This structure is used to
 * track the total resource consumption (process count, socket buffer
 * size, etc.) for the uid and impose limits.
 */
void
uihashinit()
{

  uihashtbl = hashinit(maxproc / 16, M_UIDINFO, &uihash);
  rw_init(&uihashtbl_lock, "uidinfo hash");
}

/*
 * Look up a uidinfo struct for the parameter uid.
 * uihashtbl_lock must be locked.
 */
static struct uidinfo *
uilookup(uid)
  uid_t uid;
{
  struct uihashhead *uipp;
  struct uidinfo *uip;

  rw_assert(&uihashtbl_lock, RA_LOCKED);
  uipp = UIHASH(uid);
  LIST_FOREACH(uip, uipp, ui_hash)
    if (uip->ui_uid == uid)
      break;

  return (uip);
}

/*
 * Find or allocate a struct uidinfo for a particular uid.
 * Increase refcount on uidinfo struct returned.
 * uifree() should be called on a struct uidinfo when released.
 */
struct uidinfo *
uifind(uid)
  uid_t uid;
{
  struct uidinfo *old_uip, *uip;

  rw_rlock(&uihashtbl_lock);
  uip = uilookup(uid);
  if (uip == NULL) {
    rw_runlock(&uihashtbl_lock);
    uip = malloc(sizeof(*uip), M_UIDINFO, M_WAITOK | M_ZERO);
    rw_wlock(&uihashtbl_lock);
    /*
     * There's a chance someone created our uidinfo while we
     * were in malloc and not holding the lock, so we have to
     * make sure we don't insert a duplicate uidinfo.
     */
    if ((old_uip = uilookup(uid)) != NULL) {
      /* Someone else beat us to it. */
      free(uip, M_UIDINFO);
      uip = old_uip;
    } else {
      refcount_init(&uip->ui_ref, 0);
      uip->ui_uid = uid;
      mtx_init(&uip->ui_vmsize_mtx, "ui_vmsize", NULL,
          MTX_DEF);
      LIST_INSERT_HEAD(UIHASH(uid), uip, ui_hash);
    }
  }
  uihold(uip);
  rw_unlock(&uihashtbl_lock);
  return (uip);
}

/*
 * Place another refcount on a uidinfo struct.
 */
void
uihold(uip)
  struct uidinfo *uip;
{

  refcount_acquire(&uip->ui_ref);
}

/*-
 * Since uidinfo structs have a long lifetime, we use an
 * opportunistic refcounting scheme to avoid locking the lookup hash
 * for each release.
 *
 * If the refcount hits 0, we need to free the structure,
 * which means we need to lock the hash.
 * Optimal case:
 *   After locking the struct and lowering the refcount, if we find
 *   that we don't need to free, simply unlock and return.
 * Suboptimal case:
 *   If refcount lowering results in need to free, bump the count
 *   back up, lose the lock and acquire the locks in the proper
 *   order to try again.
 */
void
uifree(uip)
  struct uidinfo *uip;
{
  int old;

  /* Prepare for optimal case. */
  old = uip->ui_ref;
  if (old > 1 && atomic_cmpset_int(&uip->ui_ref, old, old - 1))
    return;

  /* Prepare for suboptimal case. */
  rw_wlock(&uihashtbl_lock);
  if (refcount_release(&uip->ui_ref)) {
    LIST_REMOVE(uip, ui_hash);
    rw_wunlock(&uihashtbl_lock);
    if (uip->ui_sbsize != 0)
      printf("freeing uidinfo: uid = %d, sbsize = %ld\n",
          uip->ui_uid, uip->ui_sbsize);
    if (uip->ui_proccnt != 0)
      printf("freeing uidinfo: uid = %d, proccnt = %ld\n",
          uip->ui_uid, uip->ui_proccnt);
    if (uip->ui_vmsize != 0)
      printf("freeing uidinfo: uid = %d, swapuse = %lld\n",
          uip->ui_uid, (unsigned long long)uip->ui_vmsize);
    mtx_destroy(&uip->ui_vmsize_mtx);
    free(uip, M_UIDINFO);
    return;
  }
  /*
   * Someone added a reference between atomic_cmpset_int() and
   * rw_wlock(&uihashtbl_lock).
   */
  rw_wunlock(&uihashtbl_lock);
}
