/*
 * Copyright (c) 2015 embedded brains GmbH
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _LINUX_PERCPU_H
#define	_LINUX_PERCPU_H

#include <linux/threads.h>

#include <rtems/score/threaddispatch.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define	DEFINE_PER_CPU(_type, _designator) \
    _type _designator[NR_CPUS]

#define per_cpu(_designator, _cpu) \
    (_designator[_cpu])

#define this_cpu_ptr(_ptr_designator) \
    (&(*_ptr_designator)[_CPU_SMP_Get_current_processor()])

#define get_cpu_var(_designator) \
    (*({ Per_CPU_Control *_cpu_self = _Thread_Dispatch_disable(); \
    &_designator[_Per_CPU_Get_index(_cpu_self)]; }))

#define put_cpu_var(_designator) \
    _Thread_Dispatch_enable(_Per_CPU_Get())

#define	per_cpu_ptr(_ptr, _index) \
    ((_ptr) + (_index))

#define	raw_cpu_ptr(_ptr) \
    per_cpu_ptr(_ptr, _CPU_SMP_Get_current_processor())

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LINUX_PERCPU_H */
