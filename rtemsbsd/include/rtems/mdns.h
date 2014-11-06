/*
 * Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
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

#ifndef _RTEMS_MDNS_H_
#define _RTEMS_MDNS_H_

#include <mDNSEmbeddedAPI.h>

#include <rtems.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Private variable, do not touch.  Use rtems_mdns_sethostname() instead. */
extern void (*rtems_mdns_sethostname_handler)(const char *hostname);

/**
 * @brief Initializes an mDNS resolver instance.
 *
 * Calling this function multiple times leads to unpredictable results.
 *
 * @param[in] daemon_priority The task priority of the mDNS daemon.
 * @param[in] rrcachestorage The resource record cache storage.  Use
 *   mDNS_Init_NoCache in case no cache is desired.
 * @param[in] rrcachesize The resource record cache entity count of the
 *   provided storage.  Use mDNS_Init_ZeroCacheSize in case no cache is desired.
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_UNSATISFIED Initialization is incomplete.
 */
rtems_status_code rtems_mdns_initialize(rtems_task_priority daemon_priority,
    CacheEntity *rrcachestorage, mDNSu32 rrcachesize);

/**
 * @brief Returns the mDNS resolver instance.
 *
 * Using of this instance is only allowed after a successful call to
 * rtems_mdns_initialize().
 *
 * @retval instance The mDNS resolver instance.
 */
mDNS *rtems_mdns_get_instance(void);

/**
 * @brief Sets the multi-cast hostname of the mDNS resolver instance.
 *
 * In case the mDNS resolver instance is not initialized, then this function
 * has no effect.
 *
 * @param[in] hostname The new multi-cast hostname.
 */
static inline void
rtems_mdns_sethostname(const char *hostname)
{
  (*rtems_mdns_sethostname_handler)(hostname);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_MDNS_H_ */
