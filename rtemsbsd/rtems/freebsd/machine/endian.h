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

#ifndef _RTEMS_BSD_MACHINE_ENDIAN_H
#define _RTEMS_BSD_MACHINE_ENDIAN_H

#ifndef _RTEMS_BSD_MACHINE_RTEMS_BSD_CONFIG_H_
#error "the header file <freebsd/machine/rtems-bsd-config.h> must be included first"
#endif

#include <rtems/endian.h>

#if CPU_BIG_ENDIAN
# define _BYTE_ORDER _BIG_ENDIAN
#elif CPU_LITTLE_ENDIAN
# define _BYTE_ORDER _LITTLE_ENDIAN
#else
# error "undefined endian"
#endif

#define __bswap16(x) CPU_swap_u16(x)
#define __bswap32(x) CPU_swap_u32(x)

#define __htonl(x)  __bswap32(x)
#define __htons(x)  __bswap16(x)
#define __ntohl(x)  __bswap32(x)
#define __ntohs(x)  __bswap16(x)

#endif /* _RTEMS_BSD_MACHINE_ENDIAN_H */
