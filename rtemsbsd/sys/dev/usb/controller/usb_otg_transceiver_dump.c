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

#include <machine/rtems-bsd-kernel-space.h>

#include <sys/param.h>
#include <sys/systm.h>
#include <dev/usb/usb_otg_transceiver.h>

int usb_otg_transceiver_dump(const struct usb_otg_transceiver *self)
{
	static const uint8_t reg_addrs [] = {
		USB_OTG_CONTROL_1,
		USB_OTG_CONTROL_2,
		USB_OTG_CONTROL_3,
		USB_OTG_INT_SOURCE,
		USB_OTG_INT_LATCH,
		USB_OTG_INT_ENABLE_LOW,
		USB_OTG_INT_ENABLE_HIGH,
		ISP130X_OTG_STATUS,
		ISP130X_VERSION_ID_LOW,
		ISP130X_VERSION_ID_HIGH
	};

	int eno;
	uint8_t values [sizeof(reg_addrs) / sizeof(reg_addrs [0])];
	size_t reg_count = 7;
	const char *product = "unknown";

	switch (self->product_id) {
		case USB_OTG_PRODUCT_ID_ISP1301:
			product = "ISP1301";
			reg_count = 10;
			break;
		case USB_OTG_PRODUCT_ID_ISP1302:
			product = "ISP1302";
			reg_count = 10;
			break;
		case USB_OTG_PRODUCT_ID_STOTG04E:
			product = "STOTG04E";
			break;
		case USB_OTG_PRODUCT_ID_MIC2555:
			product = "MIC2555";
			break;
		default:
			break;
	}

	printf("USB OTG transceiver <%s>", product);

	eno = usb_otg_transceiver_read(self, reg_addrs, values, reg_count);
	if (eno == 0) {
		switch (self->product_id) {
			case USB_OTG_PRODUCT_ID_ISP1301:
			case USB_OTG_PRODUCT_ID_ISP1302:
				printf(
					": mctl1 %02x, mctl2 %02x, otgctl %02x"
						", isrc %02x, iltch %02x"
						", ienl %02x, ienh %02x"
						", otgsts %02x"
						", verl %02x, verh %02x\n",
					values [0],
					values [2],
					values [1],
					values [3],
					values [4],
					values [5],
					values [6],
					values [7],
					values [8],
					values [9]
				);
				break;
			default:
				printf(
					": ctl1 %02x, ctl2 %02x, ctl3 %02x"
						", isrc %02x, iltch %02x"
						", ienl %02x, ienh %02x\n",
					values [0],
					values [1],
					values [2],
					values [3],
					values [4],
					values [5],
					values [6]
				);
				break;
		}
	} else {
		printf("\n");
	}

	return eno;
}
