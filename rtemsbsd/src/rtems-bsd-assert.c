/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief TODO.
 */

/*
 * Copyright (c) 2009, 2010 embedded brains GmbH.  All rights reserved.
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

#include <freebsd/machine/rtems-bsd-config.h>

#include <freebsd/sys/types.h>
#include <freebsd/sys/systm.h>

void
rtems_bsd_assert_func(const char *file, int line, const char *func, const char *expr)
{
	panic(
		"assertion \"%s\" failed: file \"%s\", line %d%s%s\n",
		expr,
		file,
		line,
		(func != NULL) ? ", function: " : "",
		(func != NULL) ? func : ""
	);
}
