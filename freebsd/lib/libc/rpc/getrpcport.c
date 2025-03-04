#include <machine/rtems-bsd-user-space.h>

/*	$NetBSD: getrpcport.c,v 1.16 2000/01/22 22:19:18 mycroft Exp $	*/

/*-
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2009, Sun Microsystems, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are met:
 * - Redistributions of source code must retain the above copyright notice, 
 *   this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice, 
 *   this list of conditions and the following disclaimer in the documentation 
 *   and/or other materials provided with the distribution.
 * - Neither the name of Sun Microsystems, Inc. nor the names of its 
 *   contributors may be used to endorse or promote products derived 
 *   from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE 
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Copyright (c) 1985 by Sun Microsystems, Inc.
 */

#include "namespace.h"
#include <sys/types.h>
#include <sys/socket.h>

#include <assert.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>

#include <rpc/rpc.h>
#include <rpc/pmap_clnt.h>
#include "un-namespace.h"

int
getrpcport(char *host, int prognum, int versnum, int proto)
{
	struct sockaddr_in addr;
	struct hostent *hp;

	assert(host != NULL);

	if ((hp = gethostbyname2(host, AF_INET)) == NULL)
		return (0);
	memset(&addr, 0, sizeof(addr));
	addr.sin_len = sizeof(struct sockaddr_in);
	addr.sin_family = AF_INET;
	addr.sin_port =  0;
	if (hp->h_length > sizeof(addr.sin_addr.s_addr))
		hp->h_length = sizeof(addr.sin_addr.s_addr);
	memcpy(&addr.sin_addr.s_addr, hp->h_addr, (size_t)hp->h_length);
	/* Inconsistent interfaces need casts! :-( */
	return (pmap_getport(&addr, (u_long)prognum, (u_long)versnum, 
	    (u_int)proto));
}
