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
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
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

#endif /* _RTEMS_BSD_MACHINE_RTEMS_BSD_SYSINIT_H_ */
