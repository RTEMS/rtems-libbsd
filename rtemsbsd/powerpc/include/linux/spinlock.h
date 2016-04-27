/*-
 * Copyright (c) 2010 Isilon Systems, Inc.
 * Copyright (c) 2010 iX Systems, Inc.
 * Copyright (c) 2010 Panasas, Inc.
 * Copyright (c) 2013, 2014 Mellanox Technologies, Ltd.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice unmodified, this list of conditions, and the following
 *    disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $FreeBSD$
 */
#ifndef	_LINUX_SPINLOCK_H_
#define	_LINUX_SPINLOCK_H_

#include <sys/lock.h>

#include <linux/compiler.h>
#include <linux/kernel.h>
#include <linux/rwlock.h>

typedef struct _Mutex_Control spinlock_t;

#define	spin_lock_init(_l) _Mutex_Initialize(_l)
#define	spin_lock(_l) _Mutex_Acquire(_l)
#define	spin_unlock(_l) _Mutex_Release(_l)
#define	spin_lock_irq(_l) spin_lock(_l)
#define	spin_unlock_irq(_l) spin_unlock(_l)
#define	spin_lock_irqsave(_l, _f) \
    do { (void)_f; spin_lock(_l); } while (0)
#define	spin_unlock_irqrestore(_l, _f) \
    do { (void)_f; spin_unlock(_l); } while (0)

#define	__SPIN_LOCK_UNLOCKED(_l) _MUTEX_INITIALIZER

#define	DEFINE_SPINLOCK(_l) spinlock_t _l = __SPIN_LOCK_UNLOCKED(_l)

typedef spinlock_t raw_spinlock_t;

#define	raw_spin_lock_init(_l) spin_lock_init(_l)
#define	raw_spin_lock_irqsave(_l, _f) spin_lock_irqsave(_l, _f)
#define	raw_spin_unlock_irqrestore(_l, _f) spin_unlock_irqrestore(_l, _f)

#define	local_irq_save(_f) rtems_interrupt_local_disable(_f)
#define	local_irq_restore(_f) rtems_interrupt_local_enable(_f)

#endif	/* _LINUX_SPINLOCK_H_ */
