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

#ifndef _RTEMS_BSD_UTIL_H_
#define _RTEMS_BSD_UTIL_H_

#include <sys/types.h>

#include <net/if.h>
#include <net/if_arp.h>
#include <net/ethernet.h>

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief Gets the Ethernet address of an interface identified by its name.
 *
 * This function uses getifaddrs().  The interface type is not checked, so it
 * is only safe to use this function for Ethernet type interfaces.
 *
 * @param[in] ifname The name of the interface.
 * @parma[out] eaddr The Ethernet address of this interface.
 *
 * @retval 0 Successful operation.
 * @retval -1 An error occurred.  The errno is set to indicate the error.
 */
int
rtems_bsd_get_ethernet_addr(const char *ifname, uint8_t eaddr[ETHER_ADDR_LEN]);

typedef void (*rtems_bsd_arp_processor)(void *arg, int fd,
    const uint8_t eaddr[ETHER_ADDR_LEN], const struct arphdr *ar,
    uint32_t spa, uint32_t tpa, const uint8_t *sha, const uint8_t *tha);

typedef struct rtems_bsd_arp_processor_context rtems_bsd_arp_processor_context;

rtems_bsd_arp_processor_context *
rtems_bsd_arp_processor_create(const char *ifname);

int
rtems_bsd_arp_processor_get_file_descriptor(
    const rtems_bsd_arp_processor_context *ctx);

int
rtems_bsd_arp_processor_process(
    rtems_bsd_arp_processor_context *ctx,
    rtems_bsd_arp_processor processor, void *arg);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_BSD_UTIL_H_ */
