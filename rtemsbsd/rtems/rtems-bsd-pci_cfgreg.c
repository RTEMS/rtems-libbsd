#include <machine/rtems-bsd-config.h>

/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief This is the rtems version for the FreeBSD cpu specific
 * file pci_cfgreg.c.  Please note that the Ether Express is not
 * supported in this version.
 */

/*
 * COPYRIGHT (c) 2012. On-Line Applications Research Corporation (OAR).
 * All rights reserved.
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

#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <rtems/bsd/sys/param.h>
#include <sys/systm.h>
#include <sys/bus.h>
#include <rtems/bsd/sys/lock.h>
#include <sys/kernel.h>
#include <sys/malloc.h>
#include <sys/sysctl.h>
#include <dev/pci/pcivar.h>
#include <dev/pci/pcireg.h>
#define pci_find_device rtems_pci_find_device
#include <rtems/pci.h>


/* 
 * Initialise access to PCI configuration space 
 */
int
pci_cfgregopen(void)
{
  return(1);
}

/* 
 * Read configuration space register
 */
u_int32_t
pci_cfgregread(int bus, int slot, int func, int reg, int bytes)
{
  u_int32_t value;
  uint8_t   v8;
  uint16_t  v16;
  uint32_t  v32;
  int       data;

  switch (bytes) {
    case 1:
      data = pci_read_config_byte( bus, slot, func, reg, &v8 );
      value = v8;
      break;
    case 2:
      data = pci_read_config_word( bus, slot, func, reg, &v16 );
      value = v16;
      break;
    case 4:
      data = pci_read_config_dword( bus, slot, func, reg, &v32 );
      value = v32;
      break;
  }

  return value;
}

/* 
 * Write configuration space register 
 */
void
pci_cfgregwrite(int bus, int slot, int func, int reg, u_int32_t data, int bytes)
{
  uint8_t   v8  = data & 0xff;
  uint16_t  v16 = data & 0xffff;
  uint32_t  v32 = data;

  switch (bytes) {
    case 1:
      pci_write_config_byte( bus, slot, func, reg, v8 );
      break;
    case 2:
      pci_write_config_word( bus, slot, func, reg, v16 );
      break;
    case 4:
      pci_write_config_dword( bus, slot, func, reg, v32 );
      break;
  }
}
