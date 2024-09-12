/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2021 embedded brains GmbH (http://www.embedded-brains.de)
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

#include <bsp.h>

#if defined(LIBBSP_ARM_STM32H7_BSP_H) || defined(LIBBSP_ARM_STM32U5_BSP_H)

#include <bsp/st-sdmmc-config.h>
#include <dev/mmc/mmcreg.h>

void
st_sdmmc_get_config(uintptr_t sdmmc_base, struct st_sdmmc_config *cfg)
{
	switch (sdmmc_base) {
	case SDMMC1_BASE:
		cfg->data_lines = 4;
		cfg->dirpol = true;
		/*
		 * FIXME: Also the evaluation board could switch to 1.8V, the
		 * control for the level converter isn't implemented in the
		 * driver yet. So only signal 2.9V.
		 */
		cfg->ocr_voltage = MMC_OCR_280_290 | MMC_OCR_290_300;
		break;
	}
}

#endif /* LIBBSP_ARM_STM32H7_BSP_H || LIBBSP_ARM_STM32U5_BSP_H */
