/*
 * Copyright 2008-2015 Freescale Semiconductor Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Freescale Semiconductor nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 *
 * ALTERNATIVELY, this software may be distributed under the terms of the
 * GNU General Public License ("GPL") as published by the Free Software
 * Foundation, either version 2 of that License or (at your option) any
 * later version.
 *
 * THIS SOFTWARE IS PROVIDED BY Freescale Semiconductor ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Freescale Semiconductor BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef __FM_MURAM_EXT
#define __FM_MURAM_EXT

#include <linux/types.h>

#define FM_MURAM_INVALID_ALLOCATION	-1

/* Structure for FM MURAM information */
struct muram_info;

/**
 * fman_muram_init
 * @base:	Pointer to base of memory mapped FM-MURAM.
 * @size:	Size of the FM-MURAM partition.
 *
 * Creates partition in the MURAM.
 * The routine returns a pointer to the MURAM partition.
 * This pointer must be passed as to all other FM-MURAM function calls.
 * No actual initialization or configuration of FM_MURAM hardware is done by
 * this routine.
 *
 * Return: pointer to FM-MURAM object, or NULL for Failure.
 */
struct muram_info *fman_muram_init(phys_addr_t base, size_t size);

/**
 * fman_muram_offset_to_vbase
 * @muram:	FM-MURAM module pointer.
 * @offset:	the offset of the memory block
 *
 * Gives the address of the memory region from specific offset
 *
 * Return: The address of the memory block
 */
unsigned long fman_muram_offset_to_vbase(struct muram_info *muram,
					 unsigned long offset);

/**
 * fman_muram_alloc
 * @muram:	FM-MURAM module pointer.
 * @size:	Size of the memory to be allocated.
 *
 * Allocate some memory from FM-MURAM partition.
 *
 * Return: address of the allocated memory; NULL otherwise.
 */
int fman_muram_alloc(struct muram_info *muram, size_t size);

/**
 * fman_muram_free_mem
 * muram:	FM-MURAM module pointer.
 * offset:	offset of the memory region to be freed.
 * size:	size of the memory to be freed.
 *
 * Free an allocated memory from FM-MURAM partition.
 */
void fman_muram_free_mem(struct muram_info *muram, u32 offset, size_t size);

#endif /* __FM_MURAM_EXT */
