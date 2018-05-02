/**
 * @file
 *
 * @ingroup rtems_bsd
 *
 * @brief TODO.
 */

/*
 * Copyright (c) 2018 embedded brains GmbH.  All rights reserved.
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

#ifndef _RTEMS_DHCPCD_H_
#define _RTEMS_DHCPCD_H_

#include <rtems.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief The DHCP client configuration (dhcpcd).
 *
 * Zero initialize the structure to allow future extensions.
 */
typedef struct rtems_dhcpcd_config {
	rtems_task_priority priority;
	int argc;
	char **argv;
	void (*prepare)(const struct rtems_dhcpcd_config *config,
	    int argc, char **argv);
	void (*destroy)(const struct rtems_dhcpcd_config *config,
	    int exit_code);
} rtems_dhcpcd_config;

/**
 * @brief Starts the DHCP client (dhcpcd).
 *
 * @param config The DHCP client configuration.  Use NULL for a default
 * configuration.
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_INCORRECT_STATE The DHCP client runs already.
 * @retval RTEMS_TOO_MANY No task control block available.
 * @retval RTEMS_UNSATISFIED Not enough resources to create task.
 * @retval RTEMS_INVALID_PRIORITY Invalid task priority.
 */
rtems_status_code rtems_dhcpcd_start(const rtems_dhcpcd_config *config);

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_DHCPCD_H_ */
