/*
 * Copyright (c) 2017 embedded brains GmbH
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

#ifndef _LINUX_CPUMASK_H
#define	_LINUX_CPUMASK_H

#include <rtems.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* FIXME */
#define	for_each_cpu(cpu, mask)						\
    for ((cpu) = 0; (cpu) < rtems_scheduler_get_processor_maximum(); ++(cpu))

/* FIXME */
#define	for_each_cpu_not(cpu, mask)					\
    for ((cpu) = 0; (cpu) < rtems_scheduler_get_processor_maximum(); ++(cpu))

/* FIXME */
#define	for_each_cpu_and(cpu, mask, and)				\
    for ((cpu) = 0; (cpu) < rtems_scheduler_get_processor_maximum(); ++(cpu))

/* FIXME */
#define	for_each_possible_cpu(cpu)					\
    for_each_cpu((cpu), 0)

/* FIXME */
#define	for_each_online_cpu(cpu)					\
    for_each_cpu((cpu), 0)

/* FIXME */
#define	for_each_present_cpu(cpu)					\
    for_each_cpu((cpu), 0)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LINUX_CPUMASK_H */
