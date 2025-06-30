/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  Copyright (C) 2012 On-Line Applications Research Corporation (OAR)
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "port_before.h"
#include <sys/cdefs.h>
#include <sys/types.h>
#include <kvm.h>
#include <nlist.h>
#include <assert.h>
#include <string.h>

#include "kvm_private.h"

char *
kvm_geterr(
	kvm_t *kd
)
{
	return "KVM error";
	// return (kd->errbuf);
}

kvm_t *
kvm_openfiles(
	const char *uf,
	const char *mf,
	const char *sf __unused,
	int flag,
	char *errout
)
{
  return (kvm_t *)kvm_openfiles;
}

int
kvm_nlist(kvm_t *kd, struct nlist *nl)
{
	struct nlist *p;
	int nvalid;
	int error;
	const kvm_symval_t *kvm;

	assert( kd != NULL );
	assert( nl != NULL );

	nvalid = 0;
again:
	for (p = nl; p->n_name && p->n_name[0]; ++p) {
		if (p->n_type != N_UNDF)
			continue;
		if (p->n_name[0] != '_')
			continue;
             
		for (kvm=rtems_kvm_symbols; kvm->symbol[0]; ++kvm) {
			if (!strcmp(p->n_name, kvm->symbol)) {
				p->n_type = N_TEXT;
				p->n_other = 0;
				p->n_desc = 0;
				p->n_value = kvm->value;
				if (kvm->value) {
					++nvalid;
		// printf("kvm: %s = %p\n", p->n_name, (void*)kvm->value);
				}
				break;
			}
		}
		if (! kvm->symbol[0]) {
			printf("kvm: did not know about %s\n", p->n_name);
		}
	}

	error = ((p - nl) - nvalid);
	return (error);

}


ssize_t
kvm_read(kd, kva, buf, len)
	kvm_t *kd;
	u_long kva;
	void *buf;
	size_t len;
{
	assert( kd != NULL );
	assert( buf != NULL );

	if ( kva == 0 )
		return (-1);

#ifdef __rtems__
	// printf( "%d from %p to %p\n", len, (void*)kva, buf);
#endif
#if 0
	/*
	 * Use a byte-wide copy to avoid alignment issues.
	 */
	{
		unsigned char *s;
		unsigned char *d;
		size_t c;

		s = (void *)kva;
		d = buf;
		for (c=0; c<len ; c++)
			*d++ = *s++;
	}
#else
	memcpy(buf, (void *)kva, len);
#endif
	return len;
}
