/*
 * Copyright (c) 2009-2015 embedded brains GmbH.  All rights reserved.
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
#ifndef _RTEMS_BSD_SYS_TIME_H_
#define _RTEMS_BSD_SYS_TIME_H_

#include <sys/time.h>
#include <rtems/timecounter.h>

#define bintime(_x) _Timecounter_Bintime(_x)
#define nanotime(_x) _Timecounter_Nanotime(_x)
#define microtime(_x) _Timecounter_Microtime(_x)
#define binuptime(_x) _Timecounter_Binuptime(_x)
#define nanouptime(_x) _Timecounter_Nanouptime(_x)
#define microuptime(_x) _Timecounter_Microuptime(_x)
#define getbintime(_x) _Timecounter_Getbintime(_x)
#define getnanotime(_x) _Timecounter_Getnanotime(_x)
#define getmicrotime(_x) _Timecounter_Getmicrotime(_x)
#define getbinuptime(_x) _Timecounter_Getbinuptime(_x)
#define getnanouptime(_x) _Timecounter_Getnanouptime(_x)
#define getmicrouptime(_x) _Timecounter_Getmicrouptime(_x)
#define boottimebin _Timecounter_Boottimebin
#define time_second _Timecounter_Time_second
#define time_uptime _Timecounter_Time_uptime

extern struct bintime _Timecounter_Boottimebin;

#endif /* _RTEMS_BSD_SYS_TIME_H_ */
