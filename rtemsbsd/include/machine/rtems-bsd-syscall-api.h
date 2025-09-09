/**
 * @file
 *
 * @ingroup rtems_bsd_machine
 *
 * @brief TODO.
 *
 * This file helps to ensure that the system calls match the user space
 * declaration.  The user space declaration is normally not visible to the
 * kernel space due to the _KERNEL guard.
 */

/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
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

#ifndef _RTEMS_BSD_MACHINE_RTEMS_BSD_SYSCALL_API_H_
#define _RTEMS_BSD_MACHINE_RTEMS_BSD_SYSCALL_API_H_

#include <sys/cdefs.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/poll.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/sysctl.h>
#include <sys/time.h>

__BEGIN_DECLS

int pselect(int, fd_set *__restrict, fd_set *__restrict, fd_set *__restrict,
	const struct timespec *__restrict, const sigset_t *__restrict);

int	select(int, fd_set *, fd_set *, fd_set *, struct timeval *);

int	accept(int, struct sockaddr * __restrict, socklen_t * __restrict);

int	bind(int, const struct sockaddr *, socklen_t);

int	connect(int, const struct sockaddr *, socklen_t);

int	getpeername(int, struct sockaddr * __restrict, socklen_t * __restrict);

int	getsockname(int, struct sockaddr * __restrict, socklen_t * __restrict);

int	getsockopt(int, int, int, void * __restrict, socklen_t * __restrict);

int     kqueue(void);

int     kevent(int kq, const struct kevent *changelist, int nchanges,
	    struct kevent *eventlist, int nevents,
	    const struct timespec *timeout);

int	listen(int, int);

int	poll(struct pollfd _pfd[], nfds_t _nfds, int _timeout);

int	ppoll(struct pollfd _pfd[], nfds_t _nfds, const struct timespec *_timeout,
	const sigset_t *_sigmask);

ssize_t	recvfrom(int, void *, size_t, int, struct sockaddr * __restrict, socklen_t * __restrict);

ssize_t	recvmsg(int, struct msghdr *, int);

ssize_t	sendto(int, const void *, size_t, int, const struct sockaddr *, socklen_t);

ssize_t	sendmsg(int, const struct msghdr *, int);

int	setfib(int);

int	setsockopt(int, int, int, const void *, socklen_t);

int	shutdown(int, int);

int	socket(int, int, int);

int	socketpair(int, int, int, int *);

int	sysctl(const int *, u_int, void *, size_t *, const void *, size_t);

int	sysctlbyname(const char *, void *, size_t *, const void *, size_t);

int	sysctlnametomib(const char *, int *, size_t *);

__END_DECLS

#endif /* _RTEMS_BSD_MACHINE_RTEMS_BSD_SYSCALL_API_H_ */
