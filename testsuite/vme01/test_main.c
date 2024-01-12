/**
 * @file
 *
 * @brief A test for VME interrupts.
 */

/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2023 embedded brains GmbH & Co. KG
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#define TEST_NAME "LIBBSD VME 1"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <bsp.h>
#include <rtems/bsd/modules.h>
#if defined(RTEMS_BSD_MODULE_TSI148) && defined(LIBBSP_POWERPC_QORIQ_BSP_H)

#include <bsp/vmeTsi148.h>

static void
test_main(void)
{
	int error = 0;

	for (int vec = 42; vec < 43; ++vec) {
		for (int lvl = 7; lvl > 0; --lvl) {
			int x = vmeTsi148IntLoopbackTst(lvl, vec);
			printf("Tsi Interrupt test lvl %d, vec %d: %d\n",
				lvl, vec, x);
			if (x != 0) {
				error = x;
			}
		}
	}

	exit(error);
}

#else /* RTEMS_BSD_MODULE_TSI148 */

static void
test_main(void)
{
	puts("VME not enabled in the current build set or not available on this BSP.");
	exit(0);
}

#endif /* RTEMS_BSD_MODULE_TSI148 */

#define RTEMS_BSD_CONFIG_BSP_CONFIG

#include <rtems/bsd/test/default-init.h>

