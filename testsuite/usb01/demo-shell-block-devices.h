/**
 * @file
 *
 * @ingroup demo
 *
 * @brief Shell configuration.
 */

/*
 * Copyright (c) 2010 embedded brains GmbH.  All rights reserved.
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

#ifndef DEMO_SHELL_BLOCK_DEVICES_H
#define DEMO_SHELL_BLOCK_DEVICES_H

#include "demo-shell-minimal.h"

#define CONFIGURE_SHELL_COMMAND_FDISK
#define CONFIGURE_SHELL_COMMAND_BLKSYNC
#define CONFIGURE_SHELL_COMMAND_MSDOSFMT

#endif /* DEMO_SHELL_BLOCK_DEVICES_H */
