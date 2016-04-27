/*
 * Copyright (c) 2015 embedded brains GmbH
 * All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _LINUX_BITREV_H
#define	_LINUX_BITREV_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern const uint8_t bitrev_nibbles[16];

static inline uint8_t
bitrev8(uint8_t x)
{

	return ((bitrev_nibbles[x & 15] << 4) | bitrev_nibbles[x >> 4]);
}

static inline uint16_t
bitrev16(uint16_t x)
{

	return (uint16_t)((bitrev8((uint8_t)x) << 8) |
	    bitrev8((uint8_t)(x >> 8)));
}

static inline uint32_t
bitrev32(uint32_t x)
{

	return ((((uint32_t)bitrev16((uint16_t)x) << 16)) |
	    ((uint32_t)bitrev8((uint16_t)(x >> 16))));
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LINUX_BITREV_H */
