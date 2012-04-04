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
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <freebsd/machine/rtems-bsd-config.h>

#include <freebsd/sys/param.h>
#include <freebsd/sys/types.h>
#include <freebsd/sys/systm.h>
#include <freebsd/sys/malloc.h>
#include <freebsd/sys/kernel.h>

MALLOC_DEFINE(M_DEVBUF, "devbuf", "device driver memory");

MALLOC_DEFINE(M_TEMP, "temp", "misc temporary data buffers");

MALLOC_DEFINE(M_IP6OPT, "ip6opt", "IPv6 options");
MALLOC_DEFINE(M_IP6NDP, "ip6ndp", "IPv6 Neighbor Discovery");

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
