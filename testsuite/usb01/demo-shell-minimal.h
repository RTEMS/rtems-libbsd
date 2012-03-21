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

#ifndef DEMO_SHELL_MINIMAL_H
#define DEMO_SHELL_MINIMAL_H

#define CONFIGURE_SHELL_COMMAND_CPUUSE
#define CONFIGURE_SHELL_COMMAND_PERIODUSE
#define CONFIGURE_SHELL_COMMAND_STACKUSE

#if defined(BSP_HAS_IRQ_INFO)
#include <bsp/irq-info.h>

#define CONFIGURE_SHELL_USER_COMMANDS \
  &bsp_interrupt_shell_command
#endif

#endif /* DEMO_SHELL_MINIMAL_H */
