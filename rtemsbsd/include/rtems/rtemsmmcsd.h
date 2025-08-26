/*
 * Copyright (c) 2025 Aaron Nyholm <aaron.nyholm@unfoldedeffective.com>.
 * All rights reserved.
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
#ifndef __RTEMSBSD_RTEMS_RTEMSMMCSD_H__
#define __RTEMSBSD_RTEMS_RTEMSMMCSD_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct mmcsd_part;

/* define hook function pointer prototype */
typedef void (*rtems_mmcsd_attach_hookfunction)(struct mmcsd_part *part);

int rtems_mmcsd_attach_set_hook(rtems_mmcsd_attach_hookfunction hookfp);

void rtems_mmcsd_use_media_server();

void rtems_mmcsd_attach(struct mmcsd_part *part);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __RTEMSBSD_RTEMS_RTEMSMMCSD_H__ */
