/*
 * Copyright (c) 2013-2014 embedded brains GmbH.  All rights reserved.
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

RTEMS_BSD_DEFINE_NEXUS_DEVICE(smc, 0, RTEMS_ARRAY_SIZE(smc0_res),
   &smc0_res[0]);

#elif defined(__GENMCF548X_BSP_H)

RTEMS_BSD_DEFINE_NEXUS_DEVICE(fec, 0, 0, NULL);

#elif defined(LIBBSP_ARM_XILINX_ZYNQ_BSP_H)

#include <bsp/irq.h>

static const rtems_bsd_device_resource zy7_slcr0_res[] = {
	{
		.type = RTEMS_BSD_RES_MEMORY,
		.start_request = 0,
		.start_actual = 0xf8000000
	}
};

RTEMS_BSD_DEFINE_NEXUS_DEVICE(zy7_slcr, 0, RTEMS_ARRAY_SIZE(zy7_slcr0_res),
   &zy7_slcr0_res[0]);

static const rtems_bsd_device_resource cgem0_res[] = {
	{
		.type = RTEMS_BSD_RES_MEMORY,
		.start_request = 0,
		.start_actual = 0xe000b000
	}, {
		.type = RTEMS_BSD_RES_IRQ,
		.start_request = 0,
		.start_actual = ZYNQ_IRQ_ETHERNET_0
	}
};

RTEMS_BSD_DEFINE_NEXUS_DEVICE(cgem, 0, RTEMS_ARRAY_SIZE(cgem0_res),
   &cgem0_res[0]);

SYSINIT_DRIVER_REFERENCE(e1000phy, miibus);

#endif
