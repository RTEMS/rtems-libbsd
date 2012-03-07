/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief TODO.
 */

/*
 * Copyright (c) 2010 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <rtems/freebsd/machine/rtems-bsd-config.h>

#include <rtems/freebsd/sys/types.h>
#include <rtems/freebsd/sys/sysctl.h>

int sysctl(
  int *name,
	u_int namelen,
	void *oldp,
	size_t *oldlenp,
  void *newp,
	size_t newlen
)
{
	int eno = EINVAL;

	if (namelen <= CTL_MAXNAME) {
		int namedup [CTL_MAXNAME];

		memcpy(namedup, name, namelen * sizeof(*name));

		eno = kernel_sysctl(
			NULL,
			namedup,
			namelen,
			oldp,
			oldlenp,
			newp,
			newlen,
			oldlenp,
			0
		);
	}

	if (eno == 0) {
		return 0;
	} else {
		errno = eno;

		return -1;
	}
}
