/**
 * @file
 *
 * @ingroup rtems_bsd
 *
 * @brief TODO.
 */

/*
 * Copyright (c) 2015, 2017 embedded brains GmbH.  All rights reserved.
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

#ifndef _RTEMS_BSD_ZEROCOPY_H_
#define _RTEMS_BSD_ZEROCOPY_H_

#include <sys/param.h>
#include <sys/mbuf.h>
#include <sys/socket.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct mbuf *rtems_bsd_m_get(int how, short type);

struct mbuf *rtems_bsd_m_gethdr(int how, short type);

void rtems_bsd_m_extaddref(struct mbuf *m, void *buf, size_t size,
    u_int *ref_cnt, m_ext_free_t *freef, void *arg1, void *arg2);

void rtems_bsd_m_free(struct mbuf *m);

int rtems_bsd_sendto(int socket, struct mbuf *m, int flags,
    const struct sockaddr *dest_addr);

struct ifnet;

typedef void (*rtems_bsd_if_input_init)(struct ifnet *, void *);

typedef void (*rtems_bsd_if_input)(struct ifnet *, struct mbuf *);

/**
 * @brief Sets the interface input handler of the specified network interface.
 *
 * @param ifname The network interface name.
 * @param init Initialization routine called right before the new interface
 *   input handler is registered in the context of the executing thread.
 * @param if_input The new interface input handler.
 * @param arg The interface input handler argument available via struct
 * ifnet::if_input_arg.
 *
 * @retval NULL An error occurred.
 * @retval other The old interface input handler.
 */
rtems_bsd_if_input rtems_bsd_set_if_input(const char *ifname,
    rtems_bsd_if_input_init init, rtems_bsd_if_input if_input,
    void *arg);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_BSD_ZEROCOPY_H_ */
