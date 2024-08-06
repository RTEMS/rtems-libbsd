
/*-
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 1982, 1986, 1989, 1993
 *	The Regents of the University of California.  All rights reserved.
 * (c) UNIX System Laboratories, Inc.
 * All or some portions of this file are derived from material licensed
 * to the University of California by American Telephone and Telegraph
 * Co. or Unix System Laboratories, Inc. and are reproduced herein with
 * the permission of UNIX System Laboratories, Inc.
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
 *	@(#)param.h	8.3 (Berkeley) 4/4/95
 */

/*
 * Taken from FreeBSD-14. This is the _KERNEL defined fragments of
 * sys/netinet/in.h.
 */


#if defined(_KERNEL) || defined(_WANT_P_OSREL)
#define	P_OSREL_SIGWAIT			700000
#define	P_OSREL_SIGSEGV			700004
#define	P_OSREL_MAP_ANON		800104
#define	P_OSREL_MAP_FSTRICT		1100036
#define	P_OSREL_SHUTDOWN_ENOTCONN	1100077
#define	P_OSREL_MAP_GUARD		1200035
#define	P_OSREL_WRFSBASE		1200041
#define	P_OSREL_CK_CYLGRP		1200046
#define	P_OSREL_VMTOTAL64		1200054
#define	P_OSREL_CK_SUPERBLOCK		1300000
#define	P_OSREL_CK_INODE		1300005
#define	P_OSREL_POWERPC_NEW_AUX_ARGS	1300070
#define	P_OSREL_TIDPID			1400079
#define	P_OSREL_ARM64_SPSR		1400084

#define	P_OSREL_MAJOR(x)		((x) / 100000)
#endif

#include <sys/cdefs.h>
#include <sys/errno.h>
#ifndef LOCORE
#include <sys/time.h>
#include <sys/priority.h>
#endif

#ifndef FALSE
#define	FALSE	0
#endif
#ifndef TRUE
#define	TRUE	1
#endif

/*
 * Basic byte order function prototypes for non-inline functions.
 */
#ifndef LOCORE
#ifndef _BYTEORDER_PROTOTYPED
#define	_BYTEORDER_PROTOTYPED
__BEGIN_DECLS
__uint32_t	 htonl(__uint32_t);
__uint16_t	 htons(__uint16_t);
__uint32_t	 ntohl(__uint32_t);
__uint16_t	 ntohs(__uint16_t);
__END_DECLS
#endif
#endif

#ifndef _BYTEORDER_FUNC_DEFINED
#define	_BYTEORDER_FUNC_DEFINED
#define	htonl(x)	__htonl(x)
#define	htons(x)	__htons(x)
#define	ntohl(x)	__ntohl(x)
#define	ntohs(x)	__ntohs(x)
#endif /* !_BYTEORDER_FUNC_DEFINED */
