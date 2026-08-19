/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2026 Shaunak Datar
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include <machine/rtems-bsd-kernel-space.h>

#include <bsp.h>

#ifdef LIBBSP_AARCH64_RASPBERRYPI_4_BSP_H

#include <sys/bus.h>

#include <bsp/mailbox.h>

#include <arm/broadcom/bcm2835/bcm2835_mbox_prop.h>
#include <arm/broadcom/bcm2835/bcm2835_dma.h>

#define MBOX_BUFFER_SIZE 128
#define MBOX_BUFFER_ALIGN 16
#define SET_POWER_STATE_ON 0x3 /* bit 0 = 1 for ON, bit 1 = 1 for wait*/
#define SET_POWER_STATE_OFF 0x0
#define SET_POWER_STATE_VALUE_FIELDS 2
#define GET_CLOCK_RATE_VALUE_FIELDS 2
#define DEVICE_ID 0
#define CLOCK_ID 0
#define STATE 1
#define RATE 1
#define TAG_COUNT 1

static int
rpi_prop_txn(bcm2711_mbox_property_tag tag_id, uint32_t *vals,
    uint32_t value_size)
{
	uint8_t message_buffer[MBOX_BUFFER_SIZE]
	    __attribute__((aligned(MBOX_BUFFER_ALIGN)));
	mbox_property_message *msg = (mbox_property_message *)message_buffer;
	mbox_property_tag_metadata md;
	mbox_property_tag *tag;
	unsigned int n = value_size / sizeof(uint32_t);
	unsigned int i;

	md.tag_id = tag_id;
	md.value_size = value_size;

	if (rpi_mbox_property_message_init(msg, sizeof(message_buffer), &md,
	    TAG_COUNT) != RTEMS_SUCCESSFUL)
		return (EIO);

	tag = (mbox_property_tag *)msg->buffer;
	for (i = 0; i < n; i++)
		tag->value_buffer[i] = vals[i];

	if (rpi_mbox_process(msg) != RTEMS_SUCCESSFUL)
		return (EIO);

	for (i = 0; i < n; i++)
		vals[i] = tag->value_buffer[i];

	return (0);
}

int
bcm2835_mbox_set_power_state(uint32_t device_id, boolean_t on)
{
	uint32_t vals[SET_POWER_STATE_VALUE_FIELDS];

	vals[DEVICE_ID] = device_id;
	vals[STATE] = on ? SET_POWER_STATE_ON : SET_POWER_STATE_OFF;

	return (rpi_prop_txn(BCM2711_TAG_SET_POWER_STATE, vals,
	    BCM2711_TAG_SIZE_POWER_STATE));
}

int
bcm2835_mbox_get_clock_rate(uint32_t clock_id, uint32_t *rate)
{
	uint32_t vals[GET_CLOCK_RATE_VALUE_FIELDS];
	int err;

	vals[CLOCK_ID] = clock_id;
	vals[RATE] = 0;

	err = rpi_prop_txn(BCM2711_TAG_GET_CLOCK_RATE, vals,
	    BCM2711_TAG_SIZE_CLOCK_RATE);
	if (err == 0)
		*rate = vals[RATE];

	return (err);
}

int
bcm_dma_allocate(int req_ch)
{
	(void)req_ch;
	return (0);
}

int
bcm_dma_setup_intr(int ch, void (*func)(int, void *), void *arg)
{
	(void)ch;
	(void)func;
	(void)arg;
	return (0);
}

int
bcm_dma_setup_src(int ch, int dreq, int inc_addr, int width)
{
	(void)ch;
	(void)dreq;
	(void)inc_addr;
	(void)width;
	return (0);
}

int
bcm_dma_setup_dst(int ch, int dreq, int inc_addr, int width)
{
	(void)ch;
	(void)dreq;
	(void)inc_addr;
	(void)width;
	return (0);
}

int
bcm_dma_start(int ch, vm_paddr_t src, vm_paddr_t dst, int len)
{
	(void)ch;
	(void)src;
	(void)dst;
	(void)len;
	return (-1);
}

bus_addr_t
bcm283x_dmabus_peripheral_lowaddr(void)
{
	return (BUS_SPACE_MAXADDR);
}

#endif /* LIBBSP_AARCH64_RASPBERRYPI_4_BSP_H */
