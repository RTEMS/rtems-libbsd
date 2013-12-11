/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
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

#include <rtems/bsd/bsd.h>

#include <machine/rtems-bsd-sysinit.h>

#include <bsp.h>

#if defined(LIBBSP_ARM_REALVIEW_PBX_A9_BSP_H)

#include <bsp/irq.h>

static const rtems_bsd_device_resource smc0_res[] = {
	{
		.type = RTEMS_BSD_RES_MEMORY,
		.start_request = 0,
		.start_actual = 0x4e000000
	}, {
		.type = RTEMS_BSD_RES_IRQ,
		.start_request = 0,
		.start_actual = RVPBXA9_IRQ_ETHERNET
	}
};

const rtems_bsd_device rtems_bsd_nexus_devices[] = {
	{
		.name = "smc",
		.unit = 0,
		.resource_count = RTEMS_ARRAY_SIZE(smc0_res),
		.resources = &smc0_res[0]
	}
};

SYSINIT_DRIVER_REFERENCE(smc, nexus);

#elif defined(__GENMCF548X_BSP_H)

const rtems_bsd_device rtems_bsd_nexus_devices[] = {
	{
		.name = "fec",
		.unit = 0
	}, {
		.name = "fec",
		.unit = 1
	}
};

SYSINIT_DRIVER_REFERENCE(fec, nexus);

#else

const rtems_bsd_device rtems_bsd_nexus_devices[0];

#endif

const size_t rtems_bsd_nexus_device_count =
    RTEMS_ARRAY_SIZE(rtems_bsd_nexus_devices);
