/**
 * @file
 *
 * @ingroup rtems_bsd_machine
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

#ifndef _RTEMS_BSD_MACHINE_PARAM_H_
#define _RTEMS_BSD_MACHINE_PARAM_H_

#ifndef _RTEMS_BSD_MACHINE_RTEMS_BSD_CONFIG_H_
#error "the header file <freebsd/machine/rtems-bsd-config.h> must be included first"
#endif

#include <machine/param.h>

#define MAXCPU 1

#define CACHE_LINE_SHIFT 7

#define CACHE_LINE_SIZE (1 << CACHE_LINE_SHIFT)

#define MAXPAGESIZES  1   /* maximum number of supported page sizes */

#define MACHINE_ARCH  "rtems"

#endif /* _RTEMS_BSD_MACHINE_PARAM_H_ */
