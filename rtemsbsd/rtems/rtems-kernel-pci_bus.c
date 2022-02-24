#include <machine/rtems-bsd-kernel-space.h>

/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief This file contains methods that are rtems specific implementation
 * of methods in pci_bus.c.
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

#include <rtems/bsd/local/opt_cpu.h>

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/bus.h>
#include <sys/kernel.h>
#include <sys/rman.h>

#include <dev/pci/pcivar.h>
#include <dev/pci/pcireg.h>
#include <dev/pci/pcib_private.h>
#include <machine/resource.h>

#include <rtems/bsd/local/pcib_if.h>
#define pci_find_device rtems_pci_find_device
#if HAVE_RTEMS_PCI_H
#include <rtems/pci.h>
#endif
#include <machine/bus.h>

int
pcibios_pcib_route_interrupt(device_t pcib, device_t dev, int pin)
{
#if HAVE_RTEMS_PCI_H
  int     bus;
  int     slot;
  int     func;
  uint8_t irq;

  bus  = pci_get_bus(dev);
  slot = pci_get_slot(dev);
  func = pci_get_function(dev);

  pci_read_config_byte(bus, slot, func, PCI_INTERRUPT_LINE, &irq);
  return irq;
#else
  return 0;
#endif
}
