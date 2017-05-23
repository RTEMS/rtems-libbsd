/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief TODO.
 */

/*-
 * Copyright (c) 2012-2013 embedded brains GmbH.  All rights reserved.
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
 *    notice, this list of conditions, and the following disclaimer,
 *    without modification, immediately at the beginning of the file.
 * 2. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <machine/rtems-bsd-kernel-space.h>

#include <sys/param.h>
#include <sys/bus.h>

#include <rtems/bsd.h>

int
rtems_bsd_bus_root_attach(void)
{
	int err;
	device_t dev;

	dev = device_find_child(root_bus, "nexus", 0);
	if (dev != NULL) {
		err = device_probe_and_attach(dev);
	} else {
		err = ENOENT;
	}

	return (err);
}

int
rtems_bsd_bus_root_suspend(void)
{

	return (DEVICE_SUSPEND(root_bus));
}

int
rtems_bsd_bus_root_resume(void)
{

	return (DEVICE_RESUME(root_bus));
}

int
rtems_bsd_bus_root_detach(void)
{
	int err;
	device_t dev;

	dev = device_find_child(root_bus, "nexus", 0);
	if (dev != NULL) {
		err = device_detach(dev);
	} else {
		err = ENOENT;
	}

	return (err);
}
