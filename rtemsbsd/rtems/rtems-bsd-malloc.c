/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
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

#include <machine/rtems-bsd-config.h>

#include <rtems/bsd/sys/param.h>
#include <rtems/bsd/sys/types.h>
#include <sys/systm.h>
#include <sys/malloc.h>
#include <sys/kernel.h>

MALLOC_DEFINE(M_DEVBUF, "devbuf", "device driver memory");

MALLOC_DEFINE(M_TEMP, "temp", "misc temporary data buffers");

MALLOC_DEFINE(M_IP6OPT, "ip6opt", "IPv6 options");
MALLOC_DEFINE(M_IP6NDP, "ip6ndp", "IPv6 Neighbor Discovery");

MALLOC_DEFINE(M_IOV, "iov", "large iov's");

void
malloc_init(void *data)
{
	struct malloc_type *mtp = data;
}

void
malloc_uninit(void *data)
{
	struct malloc_type *mtp = data;

	BSD_PRINTF( "desc = %s\n", mtp->ks_shortdesc);
}

#undef malloc

void *
_bsd_malloc(unsigned long size, struct malloc_type *mtp, int flags)
{
	void *p = malloc(size);

	if ((flags & M_ZERO) != 0 && p != NULL) {
		memset(p, 0, size);
	}

	return p;
}

#undef realloc
void *
_bsd_realloc( void *addr, unsigned long size, 
  struct malloc_type *type, int flags)
{
	void *p = realloc(addr, size);

	if ((flags & M_ZERO) != 0 && p != NULL) {
		memset(p, 0, size);
	}

	return p;
}

#undef reallocf
void *
_bsd_reallocf( void *addr, unsigned long size, 
  struct malloc_type *type, int flags)
{
	void *p = realloc(addr, size);

	if (p == NULL) {
		free(addr,NULL);
	}

	return p;
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
	return strdup(s);
}
