/**
 * @file
 *
 * @ingroup rtems_bsd_machine
 *
 * @brief TODO.
 */

/*
 * Copyright (c) 2009-2013 embedded brains GmbH.  All rights reserved.
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

#ifndef _RTEMS_BSD_MACHINE_RTEMS_BSD_SUPPORT_H_
#define _RTEMS_BSD_MACHINE_RTEMS_BSD_SUPPORT_H_

#include <stdio.h>

#include <rtems/chain.h>

/* Debug */

#define BSD_PRINTF(fmt, ...) printf("%s: " fmt, __func__, ##__VA_ARGS__)

#define BSD_PANIC(fmt, ...) panic("%s: " fmt "\n",  __func__, ##__VA_ARGS__)

#define BSD_ASSERT_SC(sc) BSD_ASSERT((sc) == RTEMS_SUCCESSFUL)

#define BSD_ASSERT_RV(rv) BSD_ASSERT((rv) == 0)

extern rtems_chain_control rtems_bsd_lock_chain;

extern rtems_chain_control rtems_bsd_mtx_chain;

extern rtems_chain_control rtems_bsd_sx_chain;

extern rtems_chain_control rtems_bsd_condvar_chain;

extern rtems_chain_control rtems_bsd_callout_chain;

extern rtems_chain_control rtems_bsd_malloc_chain;

#endif /* _RTEMS_BSD_MACHINE_RTEMS_BSD_SUPPORT_H_ */
