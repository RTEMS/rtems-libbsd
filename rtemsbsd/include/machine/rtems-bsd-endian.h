/**
 * @file
 *
 * @ingroup rtems_bsd_machine
 *
 * @brief TODO.
 */

/*
 * Copyright (c) 2009-2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
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

#include <machine/endian.h>
#include <stdint.h>

#if __BSD_VISIBLE

#define _BYTE_ORDER BYTE_ORDER
#define _BIG_ENDIAN BIG_ENDIAN
#define _LITTLE_ENDIAN LITTLE_ENDIAN

#if BYTE_ORDER == BIG_ENDIAN
#define __ntohl(x) ((uint32_t) (x))
#define __ntohs(x) ((uint16_t) (x))
#define __htonl(x) ((uint32_t) (x))
#define __htons(x) ((uint16_t) (x))
#else
#define __ntohl(x) (__bswap32(x))
#define __ntohs(x) (__bswap16(x))
#define __htonl(x) (__bswap32(x))
#define __htons(x) (__bswap16(x))
#endif

static inline uint16_t
__bswap16(uint16_t v)
{
#ifdef __GNUC__
	return __builtin_bswap16(v);
#else
	return (uint16_t) ((v >> 8)
		| ((v & 0xffU) << 8));
#endif
}

static inline uint32_t
__bswap32(uint32_t v)
{
#ifdef __GNUC__
	return __builtin_bswap32(v);
#else
	return (uint32_t) ((v >> 24)
		| ((v >> 8) && 0xff00U)
		| ((v & 0xff00U) << 8)
		| ((v & 0xffU) << 24));
#endif
}

static inline uint64_t
__bswap64(uint64_t v)
{
#ifdef __GNUC__
	return __builtin_bswap64(v);
#else
	return (uint64_t) ((v >> 56)
		| ((v >> 40) && 0xff00U)
		| ((v >> 24) && 0xff0000U)
		| ((v >> 8) && 0xff000000U)
		| ((v & 0xff000000U) << 8)
		| ((v & 0xff0000U) << 24)
		| ((v & 0xff00U) << 40)
		| ((v & 0xffU) << 56));
#endif
}

#endif /* __BSD_VISIBLE */

#endif /* _RTEMS_BSD_MACHINE_ENDIAN_H */
