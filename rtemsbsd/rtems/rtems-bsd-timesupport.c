/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief TODO.
 */

/*
 * COPYRIGHT (c) 2012.
 * On-Line Applications Research Corporation (OAR).
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
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <machine/rtems-bsd-kernel-space.h>

#include <sys/time.h>

#include <rtems/score/timespec.h>

/*
 * Compute number of ticks in the specified amount of time.
 */
int
tvtohz(struct timeval *tv)
{
  struct timespec ts;

  ts.tv_sec = tv->tv_sec;
  ts.tv_nsec = tv->tv_usec * 1000;

  return (int) _Timespec_To_ticks( &ts );
}

void
binuptime(struct bintime *bt)
{
	struct timeval tv;

	rtems_clock_get_uptime_timeval(&tv);
	timeval2bintime(&tv, bt);
}

void
bintime(struct bintime *bt)
{
	struct timeval tv;

	gettimeofday(&tv, NULL);
	timeval2bintime(&tv, bt);
}

void
microtime(struct timeval *tvp)
{
	gettimeofday(tvp, NULL);
}

void
getbinuptime(struct bintime *bt)
{
	binuptime(bt);
}

void
getmicrouptime(struct timeval *tvp)
{
	rtems_clock_get_uptime_timeval(tvp);
}

void
getmicrotime(struct timeval *tvp)
{
	microtime(tvp);
}
