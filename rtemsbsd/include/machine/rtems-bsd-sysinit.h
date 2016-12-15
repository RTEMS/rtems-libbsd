/**
 * @file
 *
 * @ingroup rtems_bsd_machine
 *
 * @brief TODO.
 */

/*
 * Copyright (c) 2009, 2010 embedded brains GmbH.  All rights reserved.
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

#ifndef _RTEMS_BSD_MACHINE_RTEMS_BSD_SYSINIT_H_
#define _RTEMS_BSD_MACHINE_RTEMS_BSD_SYSINIT_H_

#include <sys/cdefs.h>
#include <sys/queue.h>
#include <sys/kernel.h>

#define SYSINIT_NEED_USB_CORE \
	SYSINIT_REFERENCE(usb_quirk_init); \
	SYSINIT_DRIVER_REFERENCE(uhub, usbus)

#define SYSINIT_NEED_USB_OHCI \
	SYSINIT_DRIVER_REFERENCE(ohci, nexus); \
	SYSINIT_DRIVER_REFERENCE(usbus, ohci)

#define SYSINIT_NEED_USB_EHCI \
	SYSINIT_DRIVER_REFERENCE(ehci, nexus); \
	SYSINIT_DRIVER_REFERENCE(usbus, ehci)

#define SYSINIT_NEED_USB_MASS_STORAGE \
	SYSINIT_DRIVER_REFERENCE(umass, uhub)

#define SYSINIT_NEED_USB_MOUSE \
	SYSINIT_DRIVER_REFERENCE(umass, uhub)

#define SYSINIT_NEED_SDHC \
	SYSINIT_DRIVER_REFERENCE(sdhci, nexus); \
	SYSINIT_DRIVER_REFERENCE(mmc, sdhci); \
	SYSINIT_DRIVER_REFERENCE(mmcsd, mmc)

#define SYSINIT_NEED_PCIB \
	SYSINIT_DRIVER_REFERENCE(legacy, nexus); \
	SYSINIT_DRIVER_REFERENCE(pcib, legacy);  \
	SYSINIT_DRIVER_REFERENCE(pci, pcib);

#define SYSINIT_NEED_FIREWALL_PF \
	SYSINIT_MODULE_REFERENCE(pf)

#define SYSINIT_NEED_FIREWALL_PFLOG \
	SYSINIT_MODULE_REFERENCE(pflog)

#define SYSINIT_NEED_FIREWALL_PFSYNC \
	SYSINIT_MODULE_REFERENCE(pfsync)

#define SYSINIT_NEED_NET_PF_UNIX \
	SYSINIT_DOMAIN_REFERENCE(local)

#define SYSINIT_NEED_NET_IF_BFE \
	SYSINIT_DRIVER_REFERENCE(bfe, pci)

#define SYSINIT_NEED_NET_IF_RE \
	SYSINIT_DRIVER_REFERENCE(re, pci)

#define SYSINIT_NEED_NET_IF_EM \
	SYSINIT_DRIVER_REFERENCE(em, pci)

#define SYSINIT_NEED_NET_IF_IGB \
	SYSINIT_DRIVER_REFERENCE(igb, pci)

#define SYSINIT_NEED_NET_IF_LEM \
	SYSINIT_DRIVER_REFERENCE(lem, pci)

#define SYSINIT_NEED_NET_IF_BCE \
	SYSINIT_DRIVER_REFERENCE(bce, pci)

#define SYSINIT_NEED_NET_IF_BGE \
	SYSINIT_DRIVER_REFERENCE(bge, pci)

#define SYSINIT_NEED_NET_IF_FXP \
	SYSINIT_DRIVER_REFERENCE(fxp, pci)

#define SYSINIT_NEED_NET_IF_DC \
	SYSINIT_DRIVER_REFERENCE(dc, pci)

#define SYSINIT_NEED_NET_IF_BRIDGE \
	SYSINIT_MODULE_REFERENCE(if_bridge)

#define SYSINIT_NEED_NET_IF_LAGG \
	SYSINIT_MODULE_REFERENCE(if_lagg)

#define SYSINIT_NEED_NET_IF_VLAN \
	SYSINIT_MODULE_REFERENCE(if_vlan)

#endif /* _RTEMS_BSD_MACHINE_RTEMS_BSD_SYSINIT_H_ */
