/*
 * init_usb.c
 *
 *  Created on: 14/12/2015
 *      Author: juan
 */
#include "DHT.h"
#include "board.h"

#include <stdio.h>
#include <string.h>
#include "app_usbd_cfg.h"
#include "hid_generic.h"

static USBD_HANDLE_T g_hUsb;

extern const  USBD_HW_API_T hw_api;
extern const  USBD_CORE_API_T core_api;
extern const  USBD_HID_API_T hid_api;
/* Since this example only uses HID class link functions for that class only */
static const  USBD_API_T g_usbApi = {
	&hw_api,
	&core_api,
	0,
	0,
	&hid_api,
	0,
	0,
	0x02221101,
};

const  USBD_API_T *g_pUsbApi = &g_usbApi;

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/* Initialize pin and clocks for USB port */
static void usb_pin_clk_init(void)
{
	/* enable USB PLL and clocks */
	Chip_USB_Init();
	/* enable USB 1 port on the board */

	Board_USBD_Init(1);
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/**
 * @brief	Handle interrupt from USB
 * @return	Nothing
 */
void USB_IRQHandler(void)
{
	USBD_API->hw->ISR(g_hUsb);
}

/* Find the address of interface descriptor for given class type. */
USB_INTERFACE_DESCRIPTOR *find_IntfDesc(const uint8_t *pDesc, uint32_t intfClass)
{
	USB_COMMON_DESCRIPTOR *pD;
	USB_INTERFACE_DESCRIPTOR *pIntfDesc = 0;
	uint32_t next_desc_adr;

	pD = (USB_COMMON_DESCRIPTOR *) pDesc;
	next_desc_adr = (uint32_t) pDesc;

	while (pD->bLength) {
		/* is it interface descriptor */
		if (pD->bDescriptorType == USB_INTERFACE_DESCRIPTOR_TYPE) {

			pIntfDesc = (USB_INTERFACE_DESCRIPTOR *) pD;
			/* did we find the right interface descriptor */
			if (pIntfDesc->bInterfaceClass == intfClass) {
				break;
			}
		}
		pIntfDesc = 0;
		next_desc_adr = (uint32_t) pD + pD->bLength;
		pD = (USB_COMMON_DESCRIPTOR *) next_desc_adr;
	}

	return pIntfDesc;
}

void usb_init()
{
	USBD_API_INIT_PARAM_T usb_param;
	USB_CORE_DESCS_T desc;
	ErrorCode_t ret = LPC_OK;
	usb_pin_clk_init();

		/* initialize call back structures */
		memset((void *) &usb_param, 0, sizeof(USBD_API_INIT_PARAM_T));
		usb_param.usb_reg_base = LPC_USB_BASE + 0x200;
		usb_param.max_num_ep = 2;
		usb_param.mem_base = USB_STACK_MEM_BASE;
		usb_param.mem_size = USB_STACK_MEM_SIZE;

		/* Set the USB descriptors */
		desc.device_desc = (uint8_t *) USB_DeviceDescriptor;
		desc.string_desc = (uint8_t *) USB_StringDescriptor;

		/* Note, to pass USBCV test full-speed only devices should have both
		 * descriptor arrays point to same location and device_qualifier set
		 * to 0.
		 */
		desc.high_speed_desc = USB_FsConfigDescriptor;
		desc.full_speed_desc = USB_FsConfigDescriptor;
		desc.device_qualifier = 0;

		/* USB Initialization */
		ret = USBD_API->hw->Init(&g_hUsb, &desc, &usb_param);
		if (ret == LPC_OK) {

			ret = usb_hid_init(g_hUsb,
				(USB_INTERFACE_DESCRIPTOR *) &USB_FsConfigDescriptor[sizeof(USB_CONFIGURATION_DESCRIPTOR)],
				&usb_param.mem_base, &usb_param.mem_size);
			if (ret == LPC_OK) {
				/*  enable USB interrupts */
				NVIC_EnableIRQ(USB_IRQn);
				/* now connect */
				USBD_API->hw->Connect(g_hUsb, 1);
			}
		}
}
