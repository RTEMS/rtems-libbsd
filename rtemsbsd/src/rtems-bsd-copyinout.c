/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief This file contains the RTEMS implementation of the bsd functions
 * from the copyinout.c file.
 */

/*
 *  COPYRIGHT (c) 2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <freebsd/machine/rtems-bsd-config.h>

#include <string.h>
#include <stdio.h>
#include <rtems.h>

int
copyinstr(const void *udaddr, void *kaddr, size_t len, size_t *done)
{
  memcpy(kaddr, udaddr, len);
  *done = len;
  return 0;
}
