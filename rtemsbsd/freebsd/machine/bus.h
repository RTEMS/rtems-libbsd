/**
 * @file
 *
 * @ingroup rtems_bsd_machine
 *
 * @brief TODO.
 *
 * File origin from FreeBSD 'sys/amd64/include/bus.h'.
 */

/*-
 * Copyright (c) 2009, 2010 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * Copyright (c) KATO Takenori, 1999.
 *
 * All rights reserved.  Unpublished rights reserved under the copyright
 * laws of Japan.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer as
 *    the first lines of this file unmodified.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*-
 * Copyright (c) 1996, 1997 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Jason R. Thorpe of the Numerical Aerospace Simulation Facility,
 * NASA Ames Research Center.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the NetBSD
 *	Foundation, Inc. and its contributors.
 * 4. Neither the name of The NetBSD Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*-
 * Copyright (c) 1996 Charles M. Hannum.  All rights reserved.
 * Copyright (c) 1996 Christopher G. Demetriou.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by Christopher G. Demetriou
 *	for the NetBSD Project.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _RTEMS_BSD_MACHINE_BUS_H_
#define _RTEMS_BSD_MACHINE_BUS_H_

#ifndef _RTEMS_BSD_MACHINE_RTEMS_BSD_CONFIG_H_
#error "the header file <freebsd/machine/rtems-bsd-config.h> must be included first"
#endif

/*
 * Bus address alignment.
 */
#define BUS_SPACE_ALIGNED_POINTER(p, t) ALIGNED_POINTER(p, t)

/*
 * Bus address maxima.
 */
#define BUS_SPACE_MAXADDR_24BIT	0xffffffU
#define BUS_SPACE_MAXADDR_32BIT 0xffffffffU
#define BUS_SPACE_MAXADDR 	0xffffffffU
#define BUS_SPACE_MAXSIZE_24BIT	0xffffffU
#define BUS_SPACE_MAXSIZE_32BIT	0xffffffffU
#define BUS_SPACE_MAXSIZE 	0xffffffffU

/*
 * Bus access.
 */
#define BUS_SPACE_UNRESTRICTED	(~0U)

/*
 * Bus read/write barrier method.
 */
#define	BUS_SPACE_BARRIER_READ	0x01		/* force read barrier */
#define	BUS_SPACE_BARRIER_WRITE	0x02		/* force write barrier */

/*
 * Bus address and size types
 */
typedef unsigned int bus_addr_t;
typedef unsigned int bus_size_t;

/*
 * Access methods for bus resources and address space.
 */
typedef	int bus_space_tag_t;
typedef	unsigned int bus_space_handle_t;

/*
 * Map a region of device bus space into CPU virtual address space.
 */

static __inline int
bus_space_map(bus_space_tag_t t __unused, bus_addr_t addr,
	      bus_size_t size __unused, int flags __unused,
	      bus_space_handle_t *bshp)
{
	*bshp = addr;
	return (0);
}

/*
 * Unmap a region of device bus space.
 */
static __inline void
bus_space_unmap(bus_space_tag_t bst __unused, bus_space_handle_t bsh __unused,
    bus_size_t size __unused)
{
	/* Do nothing */
}


/*
 * Get a new handle for a subregion of an already-mapped area of bus space.
 */
static __inline int
bus_space_subregion(bus_space_tag_t bst __unused, bus_space_handle_t bsh,
    bus_size_t ofs, bus_size_t size, bus_space_handle_t *nbshp)
{
	*nbshp = bsh + ofs;
	return (0);
}


/*
 * Allocate a region of memory that is accessible to devices in bus space.
 */
int
bus_space_alloc(bus_space_tag_t bst __unused, bus_addr_t rstart, bus_addr_t rend,
    bus_size_t size, bus_size_t align, bus_size_t boundary, int flags,
    bus_addr_t *addrp, bus_space_handle_t *bshp);


/*
 * Free a region of bus space accessible memory.
 */
void
bus_space_free(bus_space_tag_t bst __unused, bus_space_handle_t bsh, bus_size_t size);


static __inline void
bus_space_barrier(bus_space_tag_t bst __unused, bus_space_handle_t bsh, bus_size_t ofs,
    bus_size_t size, int flags)
{
	/* Do nothing */
}


/*
 * Read 1 unit of data from bus space described by the tag, handle and ofs
 * tuple. A unit of data can be 1 byte, 2 bytes, 4 bytes or 8 bytes. The
 * data is returned.
 */
static __inline uint8_t
bus_space_read_1(bus_space_tag_t bst __unused, bus_space_handle_t bsh, bus_size_t ofs)
{
	uint8_t __volatile *bsp = (uint8_t __volatile *)(bsh + ofs);
	return (*bsp);
}

static __inline uint16_t
bus_space_read_2(bus_space_tag_t bst __unused, bus_space_handle_t bsh, bus_size_t ofs)
{
	uint16_t __volatile *bsp = (uint16_t __volatile *)(bsh + ofs);
	return (*bsp);
}

static __inline uint32_t
bus_space_read_4(bus_space_tag_t bst __unused, bus_space_handle_t bsh, bus_size_t ofs)
{
	uint32_t __volatile *bsp = (uint32_t __volatile *)(bsh + ofs);
	return (*bsp);
}

static __inline uint64_t
bus_space_read_8(bus_space_tag_t bst __unused, bus_space_handle_t bsh, bus_size_t ofs)
{
	uint64_t __volatile *bsp = (uint64_t __volatile *)(bsh + ofs);
	return (*bsp);
}


/*
 * Write 1 unit of data to bus space described by the tag, handle and ofs
 * tuple. A unit of data can be 1 byte, 2 bytes, 4 bytes or 8 bytes. The
 * data is passed by value.
 */
static __inline void
bus_space_write_1(bus_space_tag_t bst __unused, bus_space_handle_t bsh, bus_size_t ofs,
    uint8_t val)
{
	uint8_t __volatile *bsp = (uint8_t __volatile *)(bsh + ofs);
	*bsp = val;
}

static __inline void
bus_space_write_2(bus_space_tag_t bst __unused, bus_space_handle_t bsh, bus_size_t ofs,
    uint16_t val)
{
	uint16_t __volatile *bsp = (uint16_t __volatile *)(bsh + ofs);
	*bsp = val;
}

static __inline void
bus_space_write_4(bus_space_tag_t bst __unused, bus_space_handle_t bsh, bus_size_t ofs,
    uint32_t val)
{
	uint32_t __volatile *bsp = (uint32_t __volatile *)(bsh + ofs);
	*bsp = val;
}

static __inline void
bus_space_write_8(bus_space_tag_t bst __unused, bus_space_handle_t bsh, bus_size_t ofs,
    uint64_t val)
{
	uint64_t __volatile *bsp = (uint64_t __volatile *)(bsh + ofs);
	*bsp = val;
}


/*
 * Read count units of data from bus space described by the tag, handle and
 * ofs tuple. A unit of data can be 1 byte, 2 bytes, 4 bytes or 8 bytes. The
 * data is returned in the buffer passed by reference.
 */
static __inline void
bus_space_read_multi_1(bus_space_tag_t bst __unused, bus_space_handle_t bsh,
    bus_size_t ofs, uint8_t *bufp, bus_size_t count)
{
	uint8_t __volatile *bsp = (uint8_t __volatile *)(bsh + ofs);
	while (count-- > 0) {
		*bufp++ = *bsp;
	}
}

static __inline void
bus_space_read_multi_2(bus_space_tag_t bst __unused, bus_space_handle_t bsh,
    bus_size_t ofs, uint16_t *bufp, bus_size_t count)
{
	uint16_t __volatile *bsp = (uint16_t __volatile *)(bsh + ofs);
	while (count-- > 0) {
		*bufp++ = *bsp;
	}
}

static __inline void
bus_space_read_multi_4(bus_space_tag_t bst __unused, bus_space_handle_t bsh,
    bus_size_t ofs, uint32_t *bufp, bus_size_t count)
{
	uint32_t __volatile *bsp = (uint32_t __volatile *)(bsh + ofs);
	while (count-- > 0) {
		*bufp++ = *bsp;
	}
}

static __inline void
bus_space_read_multi_8(bus_space_tag_t bst __unused, bus_space_handle_t bsh,
    bus_size_t ofs, uint64_t *bufp, bus_size_t count)
{
	uint64_t __volatile *bsp = (uint64_t __volatile *)(bsh + ofs);
	while (count-- > 0) {
		*bufp++ = *bsp;
	}
}


/*
 * Write count units of data to bus space described by the tag, handle and
 * ofs tuple. A unit of data can be 1 byte, 2 bytes, 4 bytes or 8 bytes. The
 * data is read from the buffer passed by reference.
 */
static __inline void
bus_space_write_multi_1(bus_space_tag_t bst __unused, bus_space_handle_t bsh,
    bus_size_t ofs, const uint8_t *bufp, bus_size_t count)
{
	uint8_t __volatile *bsp = (uint8_t __volatile *)(bsh + ofs);
	while (count-- > 0) {
		*bsp = *bufp++;
	}
}

static __inline void
bus_space_write_multi_2(bus_space_tag_t bst __unused, bus_space_handle_t bsh,
    bus_size_t ofs, const uint16_t *bufp, bus_size_t count)
{
	uint16_t __volatile *bsp = (uint16_t __volatile *)(bsh + ofs);
	while (count-- > 0) {
		*bsp = *bufp++;
	}
}

static __inline void
bus_space_write_multi_4(bus_space_tag_t bst __unused, bus_space_handle_t bsh,
    bus_size_t ofs, const uint32_t *bufp, bus_size_t count)
{
	uint32_t __volatile *bsp = (uint32_t __volatile *)(bsh + ofs);
	while (count-- > 0) {
		*bsp = *bufp++;
	}
}

static __inline void
bus_space_write_multi_8(bus_space_tag_t bst __unused, bus_space_handle_t bsh,
    bus_size_t ofs, const uint64_t *bufp, bus_size_t count)
{
	uint64_t __volatile *bsp = (uint64_t __volatile *)(bsh + ofs);
	while (count-- > 0) {
		*bsp = *bufp++;
	}
}


/*
 * Read count units of data from bus space described by the tag, handle and
 * ofs tuple. A unit of data can be 1 byte, 2 bytes, 4 bytes or 8 bytes. The
 * data is written to the buffer passed by reference and read from successive
 * bus space addresses. Access is unordered.
 */
static __inline void
bus_space_read_region_1(bus_space_tag_t bst __unused, bus_space_handle_t bsh,
    bus_size_t ofs, uint8_t *bufp, bus_size_t count)
{
	while (count-- > 0) {
		uint8_t __volatile *bsp = (uint8_t __volatile *)(bsh + ofs);
		*bufp++ = *bsp;
		ofs += 1;
	}
}

static __inline void
bus_space_read_region_2(bus_space_tag_t bst __unused, bus_space_handle_t bsh,
    bus_size_t ofs, uint16_t *bufp, bus_size_t count)
{
	while (count-- > 0) {
		uint16_t __volatile *bsp = (uint16_t __volatile *)(bsh + ofs);
		*bufp++ = *bsp;
		ofs += 2;
	}
}

static __inline void
bus_space_read_region_4(bus_space_tag_t bst __unused, bus_space_handle_t bsh,
    bus_size_t ofs, uint32_t *bufp, bus_size_t count)
{
	while (count-- > 0) {
		uint32_t __volatile *bsp = (uint32_t __volatile *)(bsh + ofs);
		*bufp++ = *bsp;
		ofs += 4;
	}
}

static __inline void
bus_space_read_region_8(bus_space_tag_t bst __unused, bus_space_handle_t bsh,
    bus_size_t ofs, uint64_t *bufp, bus_size_t count)
{
	while (count-- > 0) {
		uint64_t __volatile *bsp = (uint64_t __volatile *)(bsh + ofs);
		*bufp++ = *bsp;
		ofs += 8;
	}
}


/*
 * Write count units of data from bus space described by the tag, handle and
 * ofs tuple. A unit of data can be 1 byte, 2 bytes, 4 bytes or 8 bytes. The
 * data is read from the buffer passed by reference and written to successive
 * bus space addresses. Access is unordered.
 */
static __inline void
bus_space_write_region_1(bus_space_tag_t bst __unused, bus_space_handle_t bsh,
    bus_size_t ofs, const uint8_t *bufp, bus_size_t count)
{
	while (count-- > 0) {
		uint8_t __volatile *bsp = (uint8_t __volatile *)(bsh + ofs);
		*bsp = *bufp++;
		ofs += 1;
	}
}

static __inline void
bus_space_write_region_2(bus_space_tag_t bst __unused, bus_space_handle_t bsh,
    bus_size_t ofs, const uint16_t *bufp, bus_size_t count)
{
	while (count-- > 0) {
		uint16_t __volatile *bsp = (uint16_t __volatile *)(bsh + ofs);
		*bsp = *bufp++;
		ofs += 2;
	}
}

static __inline void
bus_space_write_region_4(bus_space_tag_t bst __unused, bus_space_handle_t bsh,
    bus_size_t ofs, const uint32_t *bufp, bus_size_t count)
{
	while (count-- > 0) {
		uint32_t __volatile *bsp = (uint32_t __volatile *)(bsh + ofs);
		*bsp = *bufp++;
		ofs += 4;
	}
}

static __inline void
bus_space_write_region_8(bus_space_tag_t bst __unused, bus_space_handle_t bsh,
    bus_size_t ofs, const uint64_t *bufp, bus_size_t count)
{
	while (count-- > 0) {
		uint64_t __volatile *bsp = (uint64_t __volatile *)(bsh + ofs);
		*bsp = *bufp++;
		ofs += 8;
	}
}


/*
 * Write count units of data from bus space described by the tag, handle and
 * ofs tuple. A unit of data can be 1 byte, 2 bytes, 4 bytes or 8 bytes. The
 * data is passed by value. Writes are unordered.
 */
static __inline void
bus_space_set_multi_1(bus_space_tag_t bst __unused, bus_space_handle_t bsh,
    bus_size_t ofs, uint8_t val, bus_size_t count)
{
	uint8_t __volatile *bsp = (uint8_t __volatile *)(bsh + ofs);
	while (count-- > 0) {
		*bsp = val;
	}
}

static __inline void
bus_space_set_multi_2(bus_space_tag_t bst __unused, bus_space_handle_t bsh,
    bus_size_t ofs, uint16_t val, bus_size_t count)
{
	uint16_t __volatile *bsp = (uint16_t __volatile *)(bsh + ofs);
	while (count-- > 0) {
		*bsp = val;
	}
}

static __inline void
bus_space_set_multi_4(bus_space_tag_t bst __unused, bus_space_handle_t bsh,
    bus_size_t ofs, uint32_t val, bus_size_t count)
{
	uint32_t __volatile *bsp = (uint32_t __volatile *)(bsh + ofs);
	while (count-- > 0) {
		*bsp = val;
	}
}

static __inline void
bus_space_set_multi_8(bus_space_tag_t bst __unused, bus_space_handle_t bsh,
    bus_size_t ofs, uint64_t val, bus_size_t count)
{
	uint64_t __volatile *bsp = (uint64_t __volatile *)(bsh + ofs);
	while (count-- > 0) {
		*bsp = val;
	}
}


/*
 * Write count units of data from bus space described by the tag, handle and
 * ofs tuple. A unit of data can be 1 byte, 2 bytes, 4 bytes or 8 bytes. The
 * data is passed by value and written to successive bus space addresses.
 * Writes are unordered.
 */
static __inline void
bus_space_set_region_1(bus_space_tag_t bst __unused, bus_space_handle_t bsh,
    bus_size_t ofs, uint8_t val, bus_size_t count)
{
	while (count-- > 0) {
		uint8_t __volatile *bsp = (uint8_t __volatile *)(bsh + ofs);
		*bsp = val;
		ofs += 1;
	}
}

static __inline void
bus_space_set_region_2(bus_space_tag_t bst __unused, bus_space_handle_t bsh,
    bus_size_t ofs, uint16_t val, bus_size_t count)
{
	while (count-- > 0) {
		uint16_t __volatile *bsp = (uint16_t __volatile *)(bsh + ofs);
		*bsp = val;
		ofs += 2;
	}
}

static __inline void
bus_space_set_region_4(bus_space_tag_t bst __unused, bus_space_handle_t bsh,
    bus_size_t ofs, uint32_t val, bus_size_t count)
{
	while (count-- > 0) {
		uint32_t __volatile *bsp = (uint32_t __volatile *)(bsh + ofs);
		*bsp = val;
		ofs += 4;
	}
}

static __inline void
bus_space_set_region_8(bus_space_tag_t bst __unused, bus_space_handle_t bsh,
    bus_size_t ofs, uint64_t val, bus_size_t count)
{
	while (count-- > 0) {
		uint64_t __volatile *bsp = (uint64_t __volatile *)(bsh + ofs);
		*bsp = val;
		ofs += 8;
	}
}


/*
 * Copy count units of data from bus space described by the tag and the first
 * handle and ofs pair to bus space described by the tag and the second handle
 * and ofs pair. A unit of data can be 1 byte, 2 bytes, 4 bytes or 8 bytes.
 * The data is read from successive bus space addresses and also written to
 * successive bus space addresses. Both reads and writes are unordered.
 */
static __inline void
bus_space_copy_region_1(bus_space_tag_t bst __unused, bus_space_handle_t bsh1,
    bus_size_t ofs1, bus_space_handle_t bsh2, bus_size_t ofs2, bus_size_t count)
{
	bus_addr_t dst = bsh1 + ofs1;
	bus_addr_t src = bsh2 + ofs2;
	uint8_t __volatile *dstp = (uint8_t __volatile *) dst;
	uint8_t __volatile *srcp = (uint8_t __volatile *) src;
	if (dst > src) {
		src += count - 1;
		dst += count - 1;
		while (count-- > 0) {
			*dstp = *srcp;
			src -= 1;
			dst -= 1;
		}
	} else {
		while (count-- > 0) {
			*dstp = *srcp;
			src += 1;
			dst += 1;
		}
	}
}

static __inline void
bus_space_copy_region_2(bus_space_tag_t bst __unused, bus_space_handle_t bsh1,
    bus_size_t ofs1, bus_space_handle_t bsh2, bus_size_t ofs2, bus_size_t count)
{
	bus_addr_t dst = bsh1 + ofs1;
	bus_addr_t src = bsh2 + ofs2;
	uint16_t __volatile *dstp = (uint16_t __volatile *) dst;
	uint16_t __volatile *srcp = (uint16_t __volatile *) src;
	if (dst > src) {
		src += (count - 1) << 1;
		dst += (count - 1) << 1;
		while (count-- > 0) {
			*dstp = *srcp;
			src -= 2;
			dst -= 2;
		}
	} else {
		while (count-- > 0) {
			*dstp = *srcp;
			src += 2;
			dst += 2;
		}
	}
}

static __inline void
bus_space_copy_region_4(bus_space_tag_t bst __unused, bus_space_handle_t bsh1,
    bus_size_t ofs1, bus_space_handle_t bsh2, bus_size_t ofs2, bus_size_t count)
{
	bus_addr_t dst = bsh1 + ofs1;
	bus_addr_t src = bsh2 + ofs2;
	uint32_t __volatile *dstp = (uint32_t __volatile *) dst;
	uint32_t __volatile *srcp = (uint32_t __volatile *) src;
	if (dst > src) {
		src += (count - 1) << 2;
		dst += (count - 1) << 2;
		while (count-- > 0) {
			*dstp = *srcp;
			src -= 4;
			dst -= 4;
		}
	} else {
		while (count-- > 0) {
			*dstp = *srcp;
			src += 4;
			dst += 4;
		}
	}
}

static __inline void
bus_space_copy_region_8(bus_space_tag_t bst __unused, bus_space_handle_t bsh1,
    bus_size_t ofs1, bus_space_handle_t bsh2, bus_size_t ofs2, bus_size_t count)
{
	bus_addr_t dst = bsh1 + ofs1;
	bus_addr_t src = bsh2 + ofs2;
	uint64_t __volatile *dstp = (uint64_t __volatile *) dst;
	uint64_t __volatile *srcp = (uint64_t __volatile *) src;
	if (dst > src) {
		src += (count - 1) << 3;
		dst += (count - 1) << 3;
		while (count-- > 0) {
			*dstp = *srcp;
			src -= 8;
			dst -= 8;
		}
	} else {
		while (count-- > 0) {
			*dstp = *srcp;
			src += 8;
			dst += 8;
		}
	}
}


/*
 * Stream accesses are the same as normal accesses on RTEMS; there are no
 * supported bus systems with an endianess different from the host one.
 */
#define	bus_space_read_stream_1(t, h, o)	\
	bus_space_read_1(t, h, o)
#define	bus_space_read_stream_2(t, h, o)	\
	bus_space_read_2(t, h, o)
#define	bus_space_read_stream_4(t, h, o)	\
	bus_space_read_4(t, h, o)
#define	bus_space_read_stream_8(t, h, o)	\
	bus_space_read_8(t, h, o)

#define	bus_space_read_multi_stream_1(t, h, o, a, c)	\
	bus_space_read_multi_1(t, h, o, a, c)
#define	bus_space_read_multi_stream_2(t, h, o, a, c)	\
	bus_space_read_multi_2(t, h, o, a, c)
#define	bus_space_read_multi_stream_4(t, h, o, a, c)	\
	bus_space_read_multi_4(t, h, o, a, c)
#define	bus_space_read_multi_stream_8(t, h, o, a, c)	\
	bus_space_read_multi_8(t, h, o, a, c)

#define	bus_space_write_stream_1(t, h, o, v)	\
	bus_space_write_1(t, h, o, v)
#define	bus_space_write_stream_2(t, h, o, v)	\
	bus_space_write_2(t, h, o, v)
#define	bus_space_write_stream_4(t, h, o, v)	\
	bus_space_write_4(t, h, o, v)
#define	bus_space_write_stream_8(t, h, o, v)	\
	bus_space_write_8(t, h, o, v)

#define	bus_space_write_multi_stream_1(t, h, o, a, c)	\
	bus_space_write_multi_1(t, h, o, a, c)
#define	bus_space_write_multi_stream_2(t, h, o, a, c)	\
	bus_space_write_multi_2(t, h, o, a, c)
#define	bus_space_write_multi_stream_4(t, h, o, a, c)	\
	bus_space_write_multi_4(t, h, o, a, c)
#define	bus_space_write_multi_stream_8(t, h, o, a, c)	\
	bus_space_write_multi_8(t, h, o, a, c)

#define	bus_space_set_multi_stream_1(t, h, o, v, c)	\
	bus_space_set_multi_1(t, h, o, v, c)
#define	bus_space_set_multi_stream_2(t, h, o, v, c)	\
	bus_space_set_multi_2(t, h, o, v, c)
#define	bus_space_set_multi_stream_4(t, h, o, v, c)	\
	bus_space_set_multi_4(t, h, o, v, c)
#define	bus_space_set_multi_stream_8(t, h, o, v, c)	\
	bus_space_set_multi_8(t, h, o, v, c)

#define	bus_space_read_region_stream_1(t, h, o, a, c)	\
	bus_space_read_region_1(t, h, o, a, c)
#define	bus_space_read_region_stream_2(t, h, o, a, c)	\
	bus_space_read_region_2(t, h, o, a, c)
#define	bus_space_read_region_stream_4(t, h, o, a, c)	\
	bus_space_read_region_4(t, h, o, a, c)
#define	bus_space_read_region_stream_8(t, h, o, a, c)	\
	bus_space_read_region_8(t, h, o, a, c)

#define	bus_space_write_region_stream_1(t, h, o, a, c)	\
	bus_space_write_region_1(t, h, o, a, c)
#define	bus_space_write_region_stream_2(t, h, o, a, c)	\
	bus_space_write_region_2(t, h, o, a, c)
#define	bus_space_write_region_stream_4(t, h, o, a, c)	\
	bus_space_write_region_4(t, h, o, a, c)
#define	bus_space_write_region_stream_8(t, h, o, a, c)	\
	bus_space_write_region_8(t, h, o, a, c)

#define	bus_space_set_region_stream_1(t, h, o, v, c)	\
	bus_space_set_region_1(t, h, o, v, c)
#define	bus_space_set_region_stream_2(t, h, o, v, c)	\
	bus_space_set_region_2(t, h, o, v, c)
#define	bus_space_set_region_stream_4(t, h, o, v, c)	\
	bus_space_set_region_4(t, h, o, v, c)
#define	bus_space_set_region_stream_8(t, h, o, v, c)	\
	bus_space_set_region_8(t, h, o, v, c)

#define	bus_space_copy_region_stream_1(t, h1, o1, h2, o2, c)	\
	bus_space_copy_region_1(t, h1, o1, h2, o2, c)
#define	bus_space_copy_region_stream_2(t, h1, o1, h2, o2, c)	\
	bus_space_copy_region_2(t, h1, o1, h2, o2, c)
#define	bus_space_copy_region_stream_4(t, h1, o1, h2, o2, c)	\
	bus_space_copy_region_4(t, h1, o1, h2, o2, c)
#define	bus_space_copy_region_stream_8(t, h1, o1, h2, o2, c)	\
	bus_space_copy_region_8(t, h1, o1, h2, o2, c)

#include <freebsd/machine/bus_dma.h>

#endif /* _RTEMS_BSD_MACHINE_BUS_H_ */
