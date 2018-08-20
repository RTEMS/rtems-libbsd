/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief TODO.
 */

/*-
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 1987, 1991, 1993
 *	The Regents of the University of California.
 * Copyright (c) 2005-2009 Robert N. M. Watson
 * Copyright (c) 2008 Otto Moerbeek <otto@drijf.net> (mallocarray)
 * Copyright (c) 2009, 2018 embedded brains GmbH
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
 * 3. Neither the name of the University nor the names of its contributors
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
 *
 *	@(#)kern_malloc.c	8.3 (Berkeley) 1/4/94
 */

#include <machine/rtems-bsd-kernel-space.h>
#include <machine/rtems-bsd-support.h>

#include <sys/param.h>
#include <sys/types.h>
#include <sys/systm.h>
#include <sys/malloc.h>
#include <sys/kernel.h>

MALLOC_DEFINE(M_DEVBUF, "devbuf", "device driver memory");

MALLOC_DEFINE(M_TEMP, "temp", "misc temporary data buffers");

MALLOC_DEFINE(M_IOV, "iov", "large iov's");

void
malloc_init(void *data)
{

}

void
malloc_uninit(void *data)
{
	struct malloc_type *mtp = data;

	BSD_PRINTF( "desc = %s\n", mtp->ks_shortdesc);
}

#undef malloc

void *
_bsd_malloc(size_t size, struct malloc_type *mtp, int flags)
{
	void *p = malloc(size > 0 ? size : 1);

	if ((flags & M_ZERO) != 0 && p != NULL) {
		memset(p, 0, size);
	}

	return (p);
}

void *
mallocarray(size_t nmemb, size_t size, struct malloc_type *type, int flags)
{

	if (WOULD_OVERFLOW(nmemb, size))
		panic("mallocarray: %zu * %zu overflowed", nmemb, size);

	return (_bsd_malloc(size * nmemb, type, flags));
}

#undef realloc

void *
_bsd_realloc( void *addr, size_t size, struct malloc_type *type, int flags)
{
	void *p = realloc(addr, size > 0 ? size : 1);

	if ((flags & M_ZERO) != 0 && p != NULL) {
		memset(p, 0, size);
	}

	return p;
}

#undef reallocf

void *
_bsd_reallocf( void *addr, size_t size, struct malloc_type *type, int flags)
{
	void *p = realloc(addr, size > 0 ? size : 1);

	if (p == NULL) {
		free(addr, NULL);
	}

	return (p);
}

#undef free

void
_bsd_free(void *addr, struct malloc_type *mtp)
{

	free(addr);
}

#undef strdup

char *
_bsd_strdup(const char *__restrict s, struct malloc_type *type)
{

	return (strdup(s));
}
