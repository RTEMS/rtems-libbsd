/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief Kernel Dynamic Trace support
 */

/*
 * Copyright (c) 2026 Chris Johns <chris@contemporary.siftware>.
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

#ifndef RTEMS_KERNEL_DTRACE_H
#define RTEMS_KERNEL_DTRACE_H

#include <rtems.h>

int rtems_sdt_open(size_t records);
int rtems_sdt_close(void);
int rtems_sdt_reset(void);
int rtems_sdt_clear(void);
int rtems_sdt_enable(void);
int rtems_sdt_disable(void);
int rtems_sdt_status(void);
int rtems_sdt_set_trigger(const char* prov);
int rtems_sdt_clear_trigger(const char* prov);
int rtems_sdt_print(FILE* file);
int rtems_sdt_json(FILE* file);

#endif
