/*-
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 1982, 1986, 1993
 *	The Regents of the University of California.  All rights reserved.
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
 *	@(#)time.h	8.5 (Berkeley) 5/4/95
 */

/*
 * Taken from FreeBSD-14. This is the _KERNEL defined fragments of
 * sys/sys/time.h.
 */

#ifdef __rtems__
#include <machine/_timecounter.h>
#endif /* __rtems__ */

/*
 * Simple macros to convert ticks to milliseconds
 * or microseconds and vice-versa. The answer
 * will always be at least 1. Note the return
 * value is a uint32_t however we step up the
 * operations to 64 bit to avoid any overflow/underflow
 * problems.
 */
#define TICKS_2_MSEC(t) max(1, (uint32_t)(hz == 1000) ? \
	  (t) : (((uint64_t)(t) * (uint64_t)1000)/(uint64_t)hz))
#define TICKS_2_USEC(t) max(1, (uint32_t)(hz == 1000) ? \
	  ((t) * 1000) : (((uint64_t)(t) * (uint64_t)1000000)/(uint64_t)hz))
#define MSEC_2_TICKS(m) max(1, (uint32_t)((hz == 1000) ? \
	  (m) : ((uint64_t)(m) * (uint64_t)hz)/(uint64_t)1000))
#define USEC_2_TICKS(u) max(1, (uint32_t)((hz == 1000) ? \
	 ((u) / 1000) : ((uint64_t)(u) * (uint64_t)hz)/(uint64_t)1000000))

/* Operations on timevals. */

#define	timevalclear(tvp)		((tvp)->tv_sec = (tvp)->tv_usec = 0)
#define	timevalisset(tvp)		((tvp)->tv_sec || (tvp)->tv_usec)
#define	timevalcmp(tvp, uvp, cmp)					\
	(((tvp)->tv_sec == (uvp)->tv_sec) ?				\
	    ((tvp)->tv_usec cmp (uvp)->tv_usec) :			\
	    ((tvp)->tv_sec cmp (uvp)->tv_sec))

/* timevaladd and timevalsub are not inlined */

#if defined(_KERNEL) || defined(_STANDALONE)

/*
 * Kernel to clock driver interface.
 */
void	inittodr(time_t base);
void	resettodr(void);

#ifndef __rtems__
extern volatile time_t	time_second;
extern volatile time_t	time_uptime;
#else /* __rtems__ */
#define	time_second _Timecounter_Time_second
#define	time_uptime _Timecounter_Time_uptime
#endif /* __rtems__ */
extern struct bintime tc_tick_bt;
extern sbintime_t tc_tick_sbt;
extern time_t tick_seconds_max;
extern struct bintime tick_bt;
extern sbintime_t tick_sbt;
extern int tc_precexp;
extern int tc_timepercentage;
extern struct bintime bt_timethreshold;
extern struct bintime bt_tickthreshold;
extern sbintime_t sbt_timethreshold;
extern sbintime_t sbt_tickthreshold;

extern volatile int rtc_generation;

/*
 * Functions for looking at our clock: [get]{bin,nano,micro}[up]time()
 *
 * Functions without the "get" prefix returns the best timestamp
 * we can produce in the given format.
 *
 * "bin"   == struct bintime  == seconds + 64 bit fraction of seconds.
 * "nano"  == struct timespec == seconds + nanoseconds.
 * "micro" == struct timeval  == seconds + microseconds.
 *
 * Functions containing "up" returns time relative to boot and
 * should be used for calculating time intervals.
 *
 * Functions without "up" returns UTC time.
 *
 * Functions with the "get" prefix returns a less precise result
 * much faster than the functions without "get" prefix and should
 * be used where a precision of 1/hz seconds is acceptable or where
 * performance is priority. (NB: "precision", _not_ "resolution" !)
 */

#ifndef __rtems__
void	binuptime(struct bintime *bt);
void	nanouptime(struct timespec *tsp);
void	microuptime(struct timeval *tvp);
#else /* __rtems__ */
static inline void binuptime(struct bintime *bt) {
  _Timecounter_Binuptime(bt);
}
static inline void nanouptime(struct timespec *tsp) {
  _Timecounter_Nanouptime(tsp);
}
static inline void microuptime(struct timeval *tvp) {
  _Timecounter_Microuptime(tvp);
}
#endif /* __rtems__ */

static __inline sbintime_t
sbinuptime(void)
{
	struct bintime _bt;

	binuptime(&_bt);
	return (bttosbt(_bt));
}

#ifndef __rtems__
void	bintime(struct bintime *bt);
void	nanotime(struct timespec *tsp);
void	microtime(struct timeval *tvp);

void	getbinuptime(struct bintime *bt);
void	getnanouptime(struct timespec *tsp);
void	getmicrouptime(struct timeval *tvp);
#else /* __rtems__ */
#define bintime(_bt) _Timecounter_Bintime(_bt)
#define nanotime(_tsp) _Timecounter_Nanotime(_tsp)
#define microtime(_tvp) _Timecounter_Microtime(_tvp)

#define getbinuptime(_bt) _Timecounter_Getbinuptime(_bt)
#define getnanouptime(_tsp) _Timecounter_Getnanouptime(_tsp)
#define getmicrouptime(_tvp) _Timecounter_Getmicrouptime(_tvp)
#endif /* __rtems__ */

static __inline sbintime_t
getsbinuptime(void)
{
	struct bintime _bt;

	getbinuptime(&_bt);
	return (bttosbt(_bt));
}

#ifndef __rtems__
void	getbintime(struct bintime *bt);
void	getnanotime(struct timespec *tsp);
void	getmicrotime(struct timeval *tvp);

void	getboottime(struct timeval *boottime);
void	getboottimebin(struct bintime *boottimebin);
#else /* __rtems__ */
#define getbintime(_bt) _Timecounter_Getbintime(_bt)
#define getnanotime(_tsp) _Timecounter_Getnanotime(_tsp)
#define getmicrotime(_tvp) _Timecounter_Getmicrotime(_tvp)

#define getboottime(_tvp) _Timecounter_Getboottime(_tvp)
#define getboottimebin(_bt) _Timecounter_Getboottimebin(_bt)
#endif /* __rtems__ */

/* Other functions */
int	itimerdecr(struct itimerval *itp, int usec);
int	itimerfix(struct timeval *tv);
int	eventratecheck(struct timeval *, int *, int);
#define	ppsratecheck(t, c, m) eventratecheck(t, c, m)
int	ratecheck(struct timeval *, const struct timeval *);
void	timevaladd(struct timeval *t1, const struct timeval *t2);
void	timevalsub(struct timeval *t1, const struct timeval *t2);
int	tvtohz(struct timeval *tv);

/*
 * The following HZ limits allow the tvtohz() function
 * to only use integer computations.
 */
#define	HZ_MAXIMUM (INT_MAX / (1000000 >> 6)) /* 137kHz */
#define	HZ_MINIMUM 8 /* hz */

#define	TC_DEFAULTPERC		5

#define	BT2FREQ(bt)                                                     \
	(((uint64_t)0x8000000000000000 + ((bt)->frac >> 2)) /           \
	    ((bt)->frac >> 1))

#define	SBT2FREQ(sbt)	((SBT_1S + ((sbt) >> 1)) / (sbt))

#define	FREQ2BT(freq, bt)                                               \
{									\
	(bt)->sec = 0;                                                  \
	(bt)->frac = ((uint64_t)0x8000000000000000  / (freq)) << 1;     \
}

#define	TIMESEL(sbt, sbt2)						\
	(((sbt2) >= sbt_timethreshold) ?				\
	    ((*(sbt) = getsbinuptime()), 1) : ((*(sbt) = sbinuptime()), 0))
#endif
