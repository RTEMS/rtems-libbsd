/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief TODO.
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
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
