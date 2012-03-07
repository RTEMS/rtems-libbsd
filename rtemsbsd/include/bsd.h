/**
 * @file
 *
 * @ingroup rtems_bsd
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

#ifndef _RTEMS_BSD_BSD_HH_
#define _RTEMS_BSD_BSD_HH_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <rtems/freebsd/machine/rtems-bsd-config.h>
#include <rtems/freebsd/machine/rtems-bsd-select.h>

rtems_status_code rtems_bsd_initialize(void);

rtems_status_code rtems_bsd_initialize_with_interrupt_server(void);

void rtems_bsd_shell_initialize(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_BSD_BSD_HH_ */
