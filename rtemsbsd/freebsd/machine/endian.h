/**
 * @file
 *
 * @ingroup rtems_bsd_machine
 *
 * @brief TODO.
 */

/*
 * Copyright (c) 2009-2012 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * Copyright (c) 1987, 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
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

static __inline uint64_t
__bswap64(uint64_t _x)
{

        return ((_x >> 56) | ((_x >> 40) & 0xff00) | ((_x >> 24) & 0xff0000) |
            ((_x >> 8) & 0xff000000) | ((_x << 8) & ((uint64_t)0xff << 32)) |
            ((_x << 24) & ((uint64_t)0xff << 40)) |
            ((_x << 40) & ((uint64_t)0xff << 48)) | ((_x << 56)));
}

#endif /* _RTEMS_BSD_MACHINE_ENDIAN_H */
