/*
 * Copyright (c) 1995-1998 John Birrell <jb@cimlogic.com.au>.
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
 * 3. Neither the name of the author nor the names of any co-contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY JOHN BIRRELL AND CONTRIBUTORS ``AS IS'' AND
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
 *
 * Private thread definitions for the uthread kernel.
 *
 * $FreeBSD$
 */

#ifndef _PTHREAD_PRIVATE_H
#define _PTHREAD_PRIVATE_H

/*
 * Evaluate the storage class specifier.
 */
#ifdef GLOBAL_PTHREAD_PRIVATE
#define SCLASS
#else
#define SCLASS extern
#endif

/*
 * Include files.
 */
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <freebsd/sys/param.h>
#include <freebsd/sys/queue.h>
#include <freebsd/sys/types.h>
#include <freebsd/sys/time.h>
#include <freebsd/sys/cdefs.h>
#include <sched.h>
#include <spinlock.h>
#include <pthread_np.h>
#include <freebsd/sys/malloc.h>

/*
 * Define a thread-safe macro to get the current time of day
 * which is updated at regular intervals by the scheduling signal
 * handler.
 */
#define	GET_CURRENT_TOD(tv)				\
	do {						\
		tv.tv_sec = _sched_tod.tv_sec;		\
		tv.tv_usec = _sched_tod.tv_usec;	\
	} while (tv.tv_sec != _sched_tod.tv_sec)


/* 
 * rtems uses the following structure to allow the method
 * _thread_kern_sched_state to be called.  This function
 * is stubbed out to cause a processor yeild.
 */

/*
 * Thread states.
 */
enum pthread_state {
#if 0
	PS_RUNNING,
	PS_SIGTHREAD,
	PS_MUTEX_WAIT,
	PS_COND_WAIT,
	PS_FDLR_WAIT,
	PS_FDLW_WAIT,
	PS_FDR_WAIT,
	PS_FDW_WAIT,
	PS_FILE_WAIT,
	PS_POLL_WAIT,
#endif
	PS_SELECT_WAIT,
#if 0
	PS_SLEEP_WAIT,
	PS_WAIT_WAIT,
	PS_SIGSUSPEND,
	PS_SIGWAIT,
	PS_SPINBLOCK,
	PS_JOIN,
	PS_SUSPENDED,
	PS_DEAD,
	PS_DEADLOCK,
#endif
	PS_STATE_MAX
};

struct pthread_poll_data {
	int	nfds;
	struct pollfd *fds;
};

struct pthread_wait_data {
	struct pthread_poll_data *poll_data;
};

/*
 * Thread structure.
 */
struct pthread {

	/*
	 * Time to wake up thread. This is used for sleeping threads and
	 * for any operation which may time out (such as select).
	 */
	struct timespec	wakeup_time;

	/* TRUE if operation has timed out. */
	int	timeout;

	/* Wait data. */
	struct pthread_wait_data data;

	/*
	 * Allocated for converting select into poll.
	 */
	struct pthread_poll_data poll_data;

	/*
	 * Set to TRUE if a blocking operation was
	 * interrupted by a signal:
	 */
	int		interrupted;
};

/* Time of day at last scheduling timer signal: */
SCLASS struct timeval volatile	_sched_tod
#ifdef GLOBAL_PTHREAD_PRIVATE
= { 0, 0 };
#else
;
#endif
struct pthread *_get_curthread(void);

#endif  /* !_PTHREAD_PRIVATE_H */
