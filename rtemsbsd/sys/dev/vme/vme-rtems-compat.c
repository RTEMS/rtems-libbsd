/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2023 embedded brains GmbH
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

/*
 * This file is a glue layer that allows the TSI148 RTEMS VME driver to use the
 * libbsd PCI support.
 */

typedef void FILE;

#define __INSIDE_RTEMS_BSP__

#include <rtems/pci.h>

#include <machine/rtems-bsd-kernel-space.h>

#include <sys/types.h>
#include <sys/bus.h>
#include <sys/pciio.h>
#include <dev/pci/pcivar.h>

#undef pci_find_device

static device_t dev;

static int
read_config_byte(unsigned char bus, unsigned char slot, unsigned char func,
    unsigned char reg, uint8_t *val)
{
	*val = (uint8_t)pci_read_config(dev, reg, 1);
	return 0;
}

static int
read_config_word(unsigned char bus, unsigned char slot, unsigned char func,
    unsigned char reg, uint16_t *val)
{
	*val = (uint16_t)pci_read_config(dev, reg, 2);
	return 0;
}

static int
read_config_dword(unsigned char bus, unsigned char slot, unsigned char func,
    unsigned char reg, uint32_t *val)
{
	*val = pci_read_config(dev, reg, 4);
	return 0;
}

static int
write_config_byte(unsigned char bus, unsigned char slot, unsigned char func,
    unsigned char reg, uint8_t val)
{
	pci_write_config(dev, reg, val, 1);
	return 0;
}

static int
write_config_word(unsigned char bus, unsigned char slot, unsigned char func,
    unsigned char reg, uint16_t val)
{
	pci_write_config(dev, reg, val, 2);
	return 0;
}

static int
write_config_dword(unsigned char bus, unsigned char slot, unsigned char func,
    unsigned char reg, uint32_t val)
{
	pci_write_config(dev, reg, val, 4);
	return 0;
}

static const pci_config_access_functions pci_functions = {
	.read_config_byte = read_config_byte,
	.read_config_word = read_config_word,
	.read_config_dword = read_config_dword,
	.write_config_byte = write_config_byte,
	.write_config_word = write_config_word,
	.write_config_dword = write_config_dword
};

rtems_pci_config_t BSP_pci_configuration = {
	.pci_functions = &pci_functions
};

int
pci_find_device(unsigned short vendorid, unsigned short deviceid,
    int instance, int *pbus, int *pdev, int *pfun)
{
	struct pci_devinfo *dinfo;

	if (instance != 0) {
		return -1;
	}

	if (dev != NULL) {
		return -1;
	}

	dev = _bsd_pci_find_device(vendorid, deviceid);
	if (dev == NULL) {
		return -1;
	}

	dinfo = device_get_ivars(dev);
	*pbus = dinfo->conf.pc_sel.pc_bus;
	*pdev = dinfo->conf.pc_sel.pc_dev;
	*pfun = dinfo->conf.pc_sel.pc_func;
	return 0;
}

#include <bsp.h>
#ifdef LIBBSP_POWERPC_QORIQ_BSP_H
#include <bsp/VMEConfig.h>

uintptr_t bsp_vme_pcie_base_address = 0;
unsigned short (*_BSP_clear_vmebridge_errors)(int) = NULL;
#endif
