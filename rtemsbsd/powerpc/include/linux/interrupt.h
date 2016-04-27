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
#ifndef	_LINUX_INTERRUPT_H_
#define	_LINUX_INTERRUPT_H_

#include <linux/compiler.h>
#include <linux/device.h>
#include <rtems/irq-extension.h>

typedef	irqreturn_t	(*irq_handler_t)(int, void *);

#define	IRQ_RETVAL(x)	((x) != IRQ_NONE)

#define	IRQF_SHARED	RTEMS_INTERRUPT_SHARED
#define	IRQF_NO_SUSPEND	0

#define	NO_IRQ (-1U)

int __must_check request_irq(unsigned int irq, irq_handler_t handler,
    unsigned long flags, const char *name, void *arg);

static inline void
free_irq(unsigned int irq, void *device)
{

	panic("free_irq()");
}

static inline int
__must_check devm_request_irq(struct device *dev, unsigned int irq,
    irq_handler_t handler, unsigned long flags, const char *name,
    void *arg)
{

	(void)dev;
	return (request_irq(irq, handler, flags, name, arg));
}

#endif	/* _LINUX_INTERRUPT_H_ */
