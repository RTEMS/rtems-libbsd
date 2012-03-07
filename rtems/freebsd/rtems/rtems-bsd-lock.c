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

/* Necessary to obtain some internal functions */
#define __RTEMS_VIOLATE_KERNEL_VISIBILITY__

#include <rtems/freebsd/machine/rtems-bsd-config.h>

#include <rtems/freebsd/sys/param.h>
#include <rtems/freebsd/sys/types.h>
#include <rtems/freebsd/sys/systm.h>
#include <rtems/freebsd/sys/ktr.h>
#include <rtems/freebsd/sys/lock.h>
#include <rtems/freebsd/sys/mutex.h>
#include <rtems/freebsd/sys/sx.h>
#include <rtems/freebsd/sys/rwlock.h>
#include <rtems/freebsd/sys/proc.h>

struct lock_class *lock_classes[LOCK_CLASS_MAX + 1] = {
  &lock_class_mtx_spin,
  &lock_class_mtx_sleep,
  &lock_class_sx,
  &lock_class_rm,
  &lock_class_rw,
};

