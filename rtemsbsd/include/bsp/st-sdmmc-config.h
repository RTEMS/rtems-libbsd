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

#ifndef BSP_ST_SDMMC_CONFIG_H
#define BSP_ST_SDMMC_CONFIG_H

#include <stdbool.h>
#include <stdint.h>
#include <stm32h7/hal.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct st_sdmmc_config {
	/**
	 * Number of data lines. Can be 1, 4 or 8
	 */
	uint8_t data_lines;
	/**
	 * Polarity of the DIR pins. See "SDMMC_POWER" register in the
	 * STM32H7xx data sheet for that. If you don't have the lines, you can
	 * use any value.
	 */
	bool dirpol;
	/**
	 * Possible OCR voltages. Should be something like
	 * MMC_OCR_290_300 | MMC_OCR_300_310 depending on card supply.
	 */
	uint32_t ocr_voltage;
};

/**
 * Get hardware specific configuration for SDMMC.
 *
 * This function can be overwritten in the application to adapt to another board
 * configuration. The sdmmc_base points to the base address of the SDMMC
 * instance. The cfg structure is set to zero before calling this function so
 * that only the necessary fields have to be initialized.
 */
void st_sdmmc_get_config(uintptr_t sdmmc_base, struct st_sdmmc_config *cfg);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BSP_ST_SDMMC_CONFIG_H */
