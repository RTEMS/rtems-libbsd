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

#include <dev/usb/usb_otg_transceiver.h>

#define COUNT(array) (sizeof(array) / sizeof(array [0]))

#define USB_OTG_CONTROL_1_SPEED_REG (1 << 0)
#define USB_OTG_CONTROL_1_SUSPEND_REG (1 << 1)
#define USB_OTG_CONTROL_1_DAT_SE0 (1 << 2)
#define USB_OTG_CONTROL_1_TRANSP_EN (1 << 3)
#define USB_OTG_CONTROL_1_BDIS_ACON_EN (1 << 4)
#define USB_OTG_CONTROL_1_OE_INT_EN (1 << 5)
#define USB_OTG_CONTROL_1_UART_EN (1 << 6)

/*
 * USB_OTG_CONTROL_3
 *
 * On ISP130x  this is register MODE CONTROL 2
 * On STOTG04E this is register CONTROL 3
 *
 *  'rsrvd' = not available reserved bit.
 *  'diff'  = different functionality.
 *
 * Bit  Function              ISP1301      ISP1302       STOTG04E
 * --------------------------------------------------------------
 *  0   PWR_DN                  Y            Y             rsrvd
 *  1   SPD_SUSP_CTRL           Y           rsrvd          diff
 *  2   BI_DI                   Y           rsrvd           Y
 *  3   TRANSP_BDIR[0]          Y            Y              Y
 *  4   TRANSP_BDIR[1]          Y            Y              Y
 *  5   AUDIO_EN                Y            Y              Y
 *  6   PSW_OE                  Y            Y              Y
 *  7   EN2V7                   Y           rsrvd           Y
 */

/* ISP1301 & ISP1302 (Reserved on STOTG04E) */
#define ISP130x_CONTROL_3_GLOBAL_PWR_DN (1 << 0)
/* ISP1301   (ISP1302 reserved bit!) */
#define ISP1301_CONTROL_3_SPD_SUSP_CTRL (1 << 1)
/* STOTG04E (Different functionality!) */
#define STOTG04E_CONTROL_3_RX_BIAS_EN ISP1301_CONTROL_3_SPD_SUSP_CTRL
/* ISP1301  (ISP1302 reserved bit!) */
#define ISP1301_CONTROL_3_BI_DI (1 << 2)
/* STOTG04E */
#define STOTG04E_CONTROL_3_RC_BIAS_EN (1 << 1)
#define STOTG04E_CONTROL_3_BIDI_EN ISP1301_CONTROL_3_BI_DI
/* ISP1301, ISP1302 & STOTG04E common definitions. */
#define USB_OTG_CONTROL_3_TRANSP_BDIR0 (1 << 3)
#define USB_OTG_CONTROL_3_TRANSP_BDIR1 (1 << 4)
#define USB_OTG_CONTROL_3_AUDIO_EN (1 << 5)
#define USB_OTG_CONTROL_3_PSW_OE (1 << 6)
/* ISP1301   (ISP1302 reserved bit.) */
#define ISP1301_CONTROL_3_EN2V7 (1 << 7)
/* STOTG04E */
#define STOTG04E_CONTROL_3_2V7_EN ISP1301_CONTROL_3_EN2V7

/* Note: STOTG04E Control register 2 = ISP130x OTG Control register */
#define USB_OTG_CONTROL_2_DP_PULLUP (1 << 0)
#define USB_OTG_CONTROL_2_DM_PULLUP (1 << 1)
#define USB_OTG_CONTROL_2_DP_PULLDOWN (1 << 2)
#define USB_OTG_CONTROL_2_DM_PULLDOWN (1 << 3)
#define USB_OTG_CONTROL_2_ID_PULLDOWN (1 << 4)
#define USB_OTG_CONTROL_2_VBUS_DRV (1 << 5)
#define USB_OTG_CONTROL_2_VBUS_DISCHRG (1 << 6)
#define USB_OTG_CONTROL_2_VBUS_CHRG (1 << 7)

/* ISP1302 specific */
#define ISP1302_MISC_CONTROL_REG_BYPASS_DIS (1 << 0)
#define ISP1302_MISC_CONTROL_SRP_INIT (1 << 1)
#define ISP1302_MISC_CONTROL_DP_WKPU_EN (1 << 2)
#define ISP1302_MISC_CONTROL_IDPU_DIS (1 << 3)
#define ISP1302_MISC_CONTROL_UART_2V8_EN (1 << 4)
#define ISP1302_MISC_CONTROL_FORCE_DP_LOW (1 << 6)
#define ISP1302_MISC_CONTROL_FORCE_DP_HIGH (1 << 7)

int usb_otg_transceiver_read(
	const struct usb_otg_transceiver *self,
	const uint8_t *reg_addrs,
	uint8_t *values,
	size_t count
)
{
	int eno = 0;
	size_t i;

	for (i = 0; eno == 0 && i < count; ++i) {
		eno = (*self->read)(self, reg_addrs [i], &values [i]);
	}

	return eno;
}

int usb_otg_transceiver_write(
	const struct usb_otg_transceiver *self,
	const struct usb_otg_transceiver_write_request *requests,
	size_t count
)
{
	int eno = 0;
	size_t i;

	for (i = 0; eno == 0 && i < count; ++i) {
		const struct usb_otg_transceiver_write_request *request = &requests [i];

		eno = (*self->write)(self, request->reg_addr, request->value);
	}

	return eno;
}

int usb_otg_transceiver_init(struct usb_otg_transceiver *self)
{
	static const uint8_t reg_addrs [4] = {
		USB_OTG_VENDOR_ID_LOW,
		USB_OTG_VENDOR_ID_HIGH,
		USB_OTG_PRODUCT_ID_LOW,
		USB_OTG_PRODUCT_ID_HIGH
	};

	int eno;
	uint8_t values [4];

	eno = usb_otg_transceiver_read(self, reg_addrs, values, 4);
	if (eno == 0) {
		self->vendor_id = (uint16_t) ((values [1] << 8) | values [0]);
		self->product_id = (uint16_t) ((values [3] << 8) | values [2]);
	}

	return eno;
}

#define RESUME_SETUP_REQUESTS \
	{ USB_OTG_CONTROL_1_CLEAR, 0xff }, \
	{ USB_OTG_CONTROL_1_SET, USB_OTG_CONTROL_1_SPEED_REG }, \
	{ USB_OTG_CONTROL_3_CLEAR, 0xff }

#define RESUME_FINALIZE_REQUESTS \
	{ USB_OTG_CONTROL_2_CLEAR, 0xff }, \
	{ USB_OTG_CONTROL_1_SET, USB_OTG_CONTROL_1_DAT_SE0 }, \
	{ \
		USB_OTG_CONTROL_2_SET, \
		USB_OTG_CONTROL_2_DM_PULLDOWN | USB_OTG_CONTROL_2_DP_PULLDOWN \
	}, \
	{ USB_OTG_INT_LATCH_CLEAR, 0xff }, \
	{ USB_OTG_INT_ENABLE_LOW_CLEAR, 0xff }, \
	{ USB_OTG_INT_ENABLE_HIGH_CLEAR, 0xff }

int usb_otg_transceiver_resume(const struct usb_otg_transceiver *self)
{
	static const struct usb_otg_transceiver_write_request
	default_requests [] = {
		RESUME_SETUP_REQUESTS,
		RESUME_FINALIZE_REQUESTS
	};

	static const struct usb_otg_transceiver_write_request
	isp1301_requests [] = {
		RESUME_SETUP_REQUESTS,
		{
			USB_OTG_CONTROL_3_SET,
			ISP1301_CONTROL_3_BI_DI
				| USB_OTG_CONTROL_3_PSW_OE
				| ISP1301_CONTROL_3_SPD_SUSP_CTRL
		},
		RESUME_FINALIZE_REQUESTS
	};

	static const struct usb_otg_transceiver_write_request
	isp1302_requests [] = {
		RESUME_SETUP_REQUESTS,
		{
			USB_OTG_CONTROL_3_SET,
			ISP1301_CONTROL_3_BI_DI | USB_OTG_CONTROL_3_PSW_OE
		}, {
			ISP1302_MISC_CONTROL_CLEAR,
			0xff
		}, {
			ISP1302_MISC_CONTROL_SET,
			ISP1302_MISC_CONTROL_UART_2V8_EN
		},
		RESUME_FINALIZE_REQUESTS
	};

	static const struct usb_otg_transceiver_write_request
	stotg04e_requests [] = {
		RESUME_SETUP_REQUESTS,
		{
			USB_OTG_CONTROL_3_SET,
			STOTG04E_CONTROL_3_BIDI_EN | USB_OTG_CONTROL_3_PSW_OE,
		},
		RESUME_FINALIZE_REQUESTS
	};

	int eno;
	const struct usb_otg_transceiver_write_request *requests;
	size_t count;

	switch (self->product_id) {
		case USB_OTG_PRODUCT_ID_ISP1301:
			requests = isp1301_requests;
			count = COUNT(isp1301_requests);
			break;
		case USB_OTG_PRODUCT_ID_ISP1302:
			requests = isp1302_requests;
			count = COUNT(isp1302_requests);
			break;
		case USB_OTG_PRODUCT_ID_STOTG04E:
			requests = stotg04e_requests;
			count = COUNT(stotg04e_requests);
			break;
		default:
			requests = default_requests;
			count = COUNT(default_requests);
			break;
	}

	eno = usb_otg_transceiver_write(self, requests, count);

	return eno;
}

int usb_otg_transceiver_suspend(const struct usb_otg_transceiver *self)
{
	static const struct usb_otg_transceiver_write_request
	default_requests [] = {
		{
			USB_OTG_CONTROL_1_SET,
			USB_OTG_CONTROL_1_SUSPEND_REG
		}, {
			USB_OTG_CONTROL_1_CLEAR,
			USB_OTG_CONTROL_1_TRANSP_EN | USB_OTG_CONTROL_1_UART_EN
		}, {
			USB_OTG_CONTROL_2_CLEAR,
			USB_OTG_CONTROL_2_DP_PULLUP
				| USB_OTG_CONTROL_2_DM_PULLUP
				| USB_OTG_CONTROL_2_ID_PULLDOWN
				| USB_OTG_CONTROL_2_VBUS_DRV
				| USB_OTG_CONTROL_2_VBUS_CHRG
		}, {
			USB_OTG_CONTROL_3_SET,
			USB_OTG_CONTROL_3_PSW_OE
		}, {
			USB_OTG_CONTROL_3_CLEAR,
			USB_OTG_CONTROL_3_AUDIO_EN
		}
	};

	int eno;
	const struct usb_otg_transceiver_write_request *requests;
	size_t count;

	requests = default_requests;
	count = COUNT(default_requests);

	eno = usb_otg_transceiver_write(self, requests, count);

	return eno;
}

int usb_otg_transceiver_set_vbus(
	const struct usb_otg_transceiver *self,
	enum usb_otg_transceiver_vbus vbus
)
{
	int eno;
	int all = USB_OTG_VBUS_POWER_WITH_CHARGE_PUMP
		| USB_OTG_VBUS_DISCHARGE_VIA_RESISTOR
		| USB_OTG_VBUS_CHARGE_VIA_RESISTOR;
	struct usb_otg_transceiver_write_request requests [] = {
		{ USB_OTG_CONTROL_2_CLEAR, (uint8_t) (all & ~vbus) },
		{ USB_OTG_CONTROL_2_SET, (uint8_t) (all & vbus) }
	};

	eno = usb_otg_transceiver_write(self, &requests [0], COUNT(requests));

	return eno;
}
