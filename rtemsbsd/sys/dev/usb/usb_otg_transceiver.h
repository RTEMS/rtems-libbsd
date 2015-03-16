/*
 * Copyright (c) 2012 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <info@embedded-brains.de>
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

#ifndef _USB_OTG_TRANSCEIVER_HH_
#define	_USB_OTG_TRANSCEIVER_HH_

#include <sys/stdint.h>

/* USB OTG transceiver standard registers */
#define USB_OTG_VENDOR_ID_LOW 0x0
#define USB_OTG_VENDOR_ID_HIGH 0x1
#define USB_OTG_PRODUCT_ID_LOW 0x2
#define USB_OTG_PRODUCT_ID_HIGH 0x3
#define USB_OTG_CONTROL_1 0x4
#define USB_OTG_CONTROL_1_SET 0x4
#define USB_OTG_CONTROL_1_CLEAR 0x5
#define USB_OTG_CONTROL_2 0x6
#define USB_OTG_CONTROL_2_SET 0x6
#define USB_OTG_CONTROL_2_CLEAR 0x7
#define USB_OTG_CONTROL_3 0x12
#define USB_OTG_CONTROL_3_SET 0x12
#define USB_OTG_CONTROL_3_CLEAR 0x13
#define USB_OTG_INT_SOURCE 0x8
#define USB_OTG_INT_LATCH 0xa
#define USB_OTG_INT_LATCH_SET 0xa
#define USB_OTG_INT_LATCH_CLEAR 0xb
#define USB_OTG_INT_ENABLE_LOW 0xc
#define USB_OTG_INT_ENABLE_LOW_SET 0xc
#define USB_OTG_INT_ENABLE_LOW_CLEAR 0xd
#define USB_OTG_INT_ENABLE_HIGH 0xe
#define USB_OTG_INT_ENABLE_HIGH_SET 0xe
#define USB_OTG_INT_ENABLE_HIGH_CLEAR 0xf

/* ISP130X specific registers */
#define ISP130X_OTG_STATUS 0x10
#define ISP130X_VERSION_ID_LOW 0x14
#define ISP130X_VERSION_ID_HIGH 0x15

/* ISP1302 specific registers */
#define ISP1302_MISC_CONTROL_SET 0x18
#define ISP1302_MISC_CONTROL_CLEAR 0x19

/* Product identifiers */
#define USB_OTG_PRODUCT_ID_ISP1301 0x1301
#define USB_OTG_PRODUCT_ID_ISP1302 0x1302
#define USB_OTG_PRODUCT_ID_STOTG04E 0xa0c4
#define USB_OTG_PRODUCT_ID_MIC2555 0x55b0

struct usb_otg_transceiver {
	int (*read)(
		const struct usb_otg_transceiver *self,
		uint8_t reg_addr,
		uint8_t *value
	);
	int (*write)(
		const struct usb_otg_transceiver *self,
		uint8_t reg_addr,
		uint8_t value
	);
	uint16_t vendor_id;
	uint16_t product_id;
	uint8_t i2c_addr;
};

int usb_otg_transceiver_read(
	const struct usb_otg_transceiver *self,
	const uint8_t *reg_addrs,
	uint8_t *values,
	size_t count
);

struct usb_otg_transceiver_write_request {
	uint8_t reg_addr;
	uint8_t value;
};

int usb_otg_transceiver_write(
	const struct usb_otg_transceiver *self,
	const struct usb_otg_transceiver_write_request *requests,
	size_t count
);

int usb_otg_transceiver_init(struct usb_otg_transceiver *self);

int usb_otg_transceiver_dump(const struct usb_otg_transceiver *self);

int usb_otg_transceiver_resume(const struct usb_otg_transceiver *self);

int usb_otg_transceiver_suspend(const struct usb_otg_transceiver *self);

enum usb_otg_transceiver_vbus {
	USB_OTG_VBUS_NOTHING,
	USB_OTG_VBUS_POWER_WITH_CHARGE_PUMP = 1 << 5,
	USB_OTG_VBUS_DISCHARGE_VIA_RESISTOR = 1 << 6,
	USB_OTG_VBUS_CHARGE_VIA_RESISTOR = 1 << 7
};

int usb_otg_transceiver_set_vbus(
	const struct usb_otg_transceiver *self,
	enum usb_otg_transceiver_vbus vbus
);

#endif/* _USB_OTG_TRANSCEIVER_HH_ */
