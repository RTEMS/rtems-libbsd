/*
 * Copyright (c) XXX
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
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
 */

#ifndef _NAMESPACE_H_
#define _NAMESPACE_H_

/*
 * Reverse the use of "hidden" names in the source code.
 *
 * README: When modifying this file don't forget to make the appropriate
 *         changes in un-namespace.h!!!
 */


#define _pthread_getspecific		pthread_getspecific
#define _pthread_key_create		pthread_key_create
#define _pthread_main_np		pthread_main_np
#define _pthread_once			pthread_once
#define _pthread_setspecific		pthread_setspecific
#define _read				read
#define _recvfrom			recvfrom
#define _sendto				sendto
#define _setsockopt			setsockopt
#define _socket				socket

#define _close				close
#define _connect			connect
#define _writev				writev
#define _getpeername			getpeername
#define _getprogname			getprogname
#define _getsockname			getsockname

#endif /* _NAMESPACE_H_ */
