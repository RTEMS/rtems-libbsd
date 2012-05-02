/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief TODO.
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  XXX
 */

#include <freebsd/machine/rtems-bsd-config.h>

#include <freebsd/sys/param.h>
#include <freebsd/sys/systm.h>

#include <freebsd/sys/conf.h>
#include <freebsd/sys/domain.h>
#include <freebsd/sys/fcntl.h>
#include <freebsd/sys/file.h>
#include <freebsd/sys/filedesc.h>
#include <freebsd/sys/filio.h>
#include <freebsd/sys/jail.h>
#include <freebsd/sys/kernel.h>
#include <freebsd/sys/limits.h>
#include <freebsd/sys/lock.h>
#include <freebsd/sys/malloc.h>
#include <freebsd/sys/mount.h>
#include <freebsd/sys/mutex.h>
#include <freebsd/sys/namei.h>
#include <freebsd/sys/priv.h>
#include <freebsd/sys/proc.h>

/*
 * This routine is not supported in the RTEMS interface
 */
void
funsetown(struct sigio **sigiop)
{
}

/*
 * This routine is not supported in the RTEMS interface
 */
int
fsetown(pid_t pgid, struct sigio **sigiop)
{
  return 0;
}
