/*******************************************************************************
 * (c) Copyright 2012 Microsemi Corporation.  All rights reserved.
 *
 * USB HID Class Mouse Device example aplication to demonstrate the SmartFusion2
 * MSS USB operations in device mode.
 *
 * This file uses MSS USB Driver stack (incusive of USBD-HID class driver) to
 * connect to USB Host as USB HID Mouse device.
 *
 * SVN $Revision: 5468 $
 * SVN $Date: 2013-03-29 15:38:01 +0530 (Fri, 29 Mar 2013) $
 */

#include "mouse_app.h"
#include "drivers/mss_gpio/mss_gpio.h"
#include "drivers/mss_usb/mss_usb_device.h"
#include "drivers/mss_usb/mss_usb_device_hid.h"

#ifdef __cplusplus
extern "C" {
#endif

//mss_usbd_hid_report_t report = {0};

/* USB descriptors for HID class mouse enumeration. */
extern mss_usbd_user_descr_cb_t hid_mouse_descriptors_cb;
volatile uint32_t gpio_in;

void MOUSE_task(void) {

}

void MOUSE_init(void) {

	MSS_GPIO_config(MSS_GPIO_6, MSS_GPIO_OUTPUT_MODE);
	/* Bring USB controller out of reset */
	MSS_GPIO_set_output(MSS_GPIO_6, 0u);

	/*Assign call-back function handler structure needed by USB Device Core driver*/
	MSS_USBD_set_descr_cb_handler(&hid_mouse_descriptors_cb);

	/*Initialize HID Class driver.*/
	MSS_USBD_HID_init(MSS_USB_DEVICE_HS);

	/*Initialize USB Device Core driver.*/
	MSS_USBD_init(MSS_USB_DEVICE_HS);

	/*MSS_USBD_rx_ep_configure(MSS_USB_RX_EP_1,
	 0x100u,
	 16,
	 10,
	 1u,
	 DMA_ENABLE,
	 MSS_USB_DMA_CHANNEL1,
	 MSS_USB_XFR_BULK,
	 NO_ZLP_TO_XFR);*///Modified by Sudeep


	MSS_USBD_tx_ep_configure(HID_INTR_TX_EP, HID_INTR_TX_EP_FIFO_ADDR, 1024u,
//	MSS_USBD_tx_ep_configure(HID_INTR_IN_EP, HID_INTR_IN_EP_FIFO_ADDR, 1024u,
			1024u, 1u, DMA_DISABLE, MSS_USB_DMA_CHANNEL1,
			MSS_USB_XFR_INTERRUPT, NO_ZLP_TO_XFR);

	MSS_USBD_rx_ep_configure(MSS_USB_RX_EP_1, 0x100u, 1024u, 1024u, 1u,
			DMA_DISABLE, MSS_USB_DMA_CHANNEL1, MSS_USB_XFR_INTERRUPT,
			NO_ZLP_TO_XFR); //Modified by Sudeep

	MSS_USB_CIF_rx_ep_enable_irq(MSS_USB_RX_EP_1);
	MSS_USBD_rx_ep_flush_fifo(MSS_USB_RX_EP_1);
	MSS_USBD_rx_ep_read_prepare(MSS_USB_RX_EP_1, (uint8_t*) &g_rx_data, 256);

}

#ifdef __cplusplus
}
#endif
