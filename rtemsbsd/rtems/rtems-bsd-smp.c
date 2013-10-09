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

#include <freebsd/machine/rtems-bsd-config.h>

#include <freebsd/sys/types.h>

#include <freebsd/sys/smp.h>
#include <freebsd/machine/smp.h>
#include <freebsd/sys/sysctl.h>

int       mp_ncpus;
int       mp_maxcpus;
cpumask_t all_cpus;
u_int     mp_maxid;

static void smp_configure(void *dummy)
{
  int i;

  all_cpus = 0x0;

#if defined(RTEMS_SMP)
  mp_ncpus = rtems_smp_get_number_of_processors();
  mp_maxcpus = rtems_configuration_smp_maximum_processors;
#else
  mp_ncpus = 1;
  mp_maxcpus = 1;
#endif

  mp_maxid = mp_maxcpus;
  for(i=0; i< mp_ncpus; i++)
    all_cpus |= 0x1 << i;
}

SYSCTL_NODE(_kern, OID_AUTO, smp, CTLFLAG_RD, smp_configure, "Kernel SMP");
