/***********************************************************/
/*
 *
 *  The telnet DAEMON
 *
 *  Author: 17,may 2001
 *
 *   WORK: fernando.ruiz@ctv.es
 *   HOME: correo@fernando-ruiz.com
 *
 * Copyright (c) 2009 embedded brains GmbH and others.
 *
 * embedded brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/telnetd.h>

rtems_status_code rtems_telnetd_initialize(void)
{
  return rtems_telnetd_start(&rtems_telnetd_config);
}
