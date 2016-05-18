/*
 * Copyright (c) 2016 Chris Johns <chrisj@rtems.org>.  All rights reserved.
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

/*
 * The directives available with rc.conf. You define the list you wish
 * to have available in your application and they will be available.
 */

#ifndef _RTEMS_BSP_RC_CONF_DIRECTIVES_h
#define _RTEMS_BSP_RC_CONF_DIRECTIVES_h

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <sys/queue.h>
#include <sys/kernel.h>

/*
 * Use the same SYSINT for all directives.
 */
#define RTEMS_BSD_RC_CONF_SYSINT(_n) \
  SYSINIT(_n, SI_SUB_CREATE_INIT, SI_ORDER_ANY, _n ## _init, NULL)

/*
 * Decls for the handlers.
 */
void rc_conf_net_init(void* arg);   /* Installed by default. */

/*
 * The user available directives.
 */
/* add here */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
