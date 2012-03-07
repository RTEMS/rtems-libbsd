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

#include <rtems/freebsd/sys/cdefs.h>
#include <rtems/freebsd/sys/queue.h>
#include <rtems/freebsd/sys/kernel.h>

#define SYSINIT_NEED_FREEBSD_CORE \
	SYSINIT_REFERENCE(configure1); \
	SYSINIT_REFERENCE(module); \
	SYSINIT_REFERENCE(kobj); \
	SYSINIT_REFERENCE(linker_kernel); \
	SYSINIT_MODULE_REFERENCE(rootbus); \
	SYSINIT_DRIVER_REFERENCE(nexus, root)

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

#define SYSINIT_NEED_NET_MII \
  SYSINIT_DRIVER_REFERENCE(icsphy, miibus);

/* FIXME */
extern const char *const _bsd_nexus_devices [];

#endif /* _RTEMS_BSD_MACHINE_RTEMS_BSD_SYSINIT_H_ */
