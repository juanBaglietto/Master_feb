/*
 * @brief HID generic example's callabck routines
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2013
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#include "board.h"
#include <stdint.h>
#include <string.h>
#include "usbd_rom_api.h"
#include "Placa1_def.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

static uint8_t *loopback_report;

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

extern const uint8_t HID_ReportDescriptor[];
extern const uint16_t HID_ReportDescSize;

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/*  HID get report callback function. */
static ErrorCode_t HID_GetReport(USBD_HANDLE_T hHid, USB_SETUP_PACKET *pSetup, uint8_t * *pBuffer, uint16_t *plength)
{
	/* ReportID = SetupPacket.wValue.WB.L; */
	switch (pSetup->wValue.WB.H) {
	case HID_REPORT_INPUT:
		*pBuffer[0] = *loopback_report;
		*plength = 1;
		break;

	case HID_REPORT_OUTPUT:
		return ERR_USBD_STALL;			/* Not Supported */

	case HID_REPORT_FEATURE:
		return ERR_USBD_STALL;			/* Not Supported */
	}
	return LPC_OK;
}

/* HID set report callback function. */
static ErrorCode_t HID_SetReport(USBD_HANDLE_T hHid, USB_SETUP_PACKET *pSetup, uint8_t * *pBuffer, uint16_t length)
{
	/* we will reuse standard EP0Buf */
	if (length == 0) {
		return LPC_OK;
	}

	/* ReportID = SetupPacket.wValue.WB.L; */
	switch (pSetup->wValue.WB.H) {
	case HID_REPORT_INPUT:
		return ERR_USBD_STALL;			/* Not Supported */

	case HID_REPORT_OUTPUT:
		*loopback_report = **pBuffer;
		break;

	case HID_REPORT_FEATURE:
		return ERR_USBD_STALL;			/* Not Supported */
	}
	return LPC_OK;
}

/* HID Interrupt endpoint event handler. */
static ErrorCode_t HID_Ep_Hdlr(USBD_HANDLE_T hUsb, void *data, uint32_t event)
{
	static uint8_t valor_rec_usb=0;
	static uint8_t estado=0;
	static uint16_t rpm_actual=0;
	static uint16_t nivel_actual=0;
	static uint16_t temp_actual=27;
	static uint16_t hum_actual=56;



	USB_HID_CTRL_T *pHidCtrl = (USB_HID_CTRL_T *) data;


	switch (event) {
	case USB_EVT_IN:
		/* last report is successfully sent. Do something... */
		break;

	case USB_EVT_OUT:
		/* Read the new report received. */
		USBD_API->hw->ReadEP(hUsb, pHidCtrl->epout_adr, loopback_report);

		valor_rec_usb=*loopback_report;

		switch(estado)
		{
		case 0:
			if(valor_rec_usb==rps_id)
			{
				estado=2;

			}
			if(valor_rec_usb==nievel_id )
			{
				estado=1;
			}
			if(valor_rec_usb==ID_ACTUAL_RPM)//actulizacion de las rpm
			{
				xQueueReceive(actual_rpm,&rpm_actual,0);
				//rpm_actual=30;
				uint8_t val1=0;
				val1=(uint8_t)rpm_actual;

				USBD_API->hw->WriteEP(hUsb, pHidCtrl->epin_adr, &val1, sizeof(val1));
				estado=0;
			}
			if(valor_rec_usb==ID_ACTUAL_NIVEL)//actulizacion de las nivel del tanque
			{
				xQueueReceive(actual_nivel,&nivel_actual,0);
				//nivel_actual=59;
				uint8_t val2=0;
				val2=(uint8_t)nivel_actual;
				USBD_API->hw->WriteEP(hUsb, pHidCtrl->epin_adr,&val2 , sizeof(val2));
				estado=0;
			}
			if(valor_rec_usb==ID_ACTUAL_TEMP)//actulizacion de la temperatura
			{
				xQueueReceive(actual_temp,&temp_actual,0);
				//temp_actual=25;
				uint8_t val3=0;
				val3=(uint8_t)temp_actual;
				USBD_API->hw->WriteEP(hUsb, pHidCtrl->epin_adr,&val3 , sizeof(val3));
				estado=0;
			}
			if(valor_rec_usb==ID_ACTUAL_HUM)//actulizacion de la humedad
			{
				xQueueReceive(actual_hum,&hum_actual,0);
				//hum_actual=60;
				uint8_t val4=4;
				val4=(uint8_t)hum_actual;
				USBD_API->hw->WriteEP(hUsb, pHidCtrl->epin_adr,&val4 , sizeof(val4));
				estado=0;
			}

			break;
		case 1://valor nuevo para setear el nivel del tanque

			xQueueSendToBack(bomba,&valor_rec_usb,0);

			USBD_API->hw->WriteEP(hUsb, pHidCtrl->epin_adr, &valor_rec_usb, sizeof(valor_rec_usb));

			estado=0;
//			xQueueReceive(actual_nivel,&nivel_actual,0);
//			//nivel_actual=59;
//			uint8_t val2=2;
//			val2=(uint8_t)nivel_actual;
//			USBD_API->hw->WriteEP(hUsb, pHidCtrl->epin_adr,&val2 , sizeof(val2));
			break;
		case 2: //valor nuevo para setear las rpm

			xQueueSendToBack(motor,&valor_rec_usb,0);
			USBD_API->hw->WriteEP(hUsb, pHidCtrl->epin_adr, &valor_rec_usb, sizeof(valor_rec_usb));

			estado=0;
//			xQueueReceive(actual_rpm,&rpm_actual,0);
//			//rpm_actual=30;
//			uint8_t val1=76;
//			val1=(uint8_t)rpm_actual;
//
//			USBD_API->hw->WriteEP(hUsb, pHidCtrl->epin_adr, &val1, sizeof(val1));

			break;


		}


	}
	return LPC_OK;
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* HID init routine */
ErrorCode_t usb_hid_init(USBD_HANDLE_T hUsb,
						 USB_INTERFACE_DESCRIPTOR *pIntfDesc,
						 uint32_t *mem_base,
						 uint32_t *mem_size)
{
	USBD_HID_INIT_PARAM_T hid_param;
	USB_HID_REPORT_T reports_data[1];
	ErrorCode_t ret = LPC_OK;

	memset((void *) &hid_param, 0, sizeof(USBD_HID_INIT_PARAM_T));
	/* HID paramas */
	hid_param.max_reports = 1;
	/* Init reports_data */
	reports_data[0].len = HID_ReportDescSize;
	reports_data[0].idle_time = 0;
	reports_data[0].desc = (uint8_t *) &HID_ReportDescriptor[0];

	if ((pIntfDesc == 0) || (pIntfDesc->bInterfaceClass != USB_DEVICE_CLASS_HUMAN_INTERFACE)) {
		return ERR_FAILED;
	}

	hid_param.mem_base = *mem_base;
	hid_param.mem_size = *mem_size;
	hid_param.intf_desc = (uint8_t *) pIntfDesc;
	/* user defined functions */
	hid_param.HID_GetReport = HID_GetReport;
	hid_param.HID_SetReport = HID_SetReport;
	hid_param.HID_EpIn_Hdlr  = HID_Ep_Hdlr;
	hid_param.HID_EpOut_Hdlr = HID_Ep_Hdlr;
	hid_param.report_data  = reports_data;

	ret = USBD_API->hid->init(hUsb, &hid_param);
	/* allocate USB accessable memory space for report data */
	loopback_report =  (uint8_t *) hid_param.mem_base;
	hid_param.mem_base += 4;
	hid_param.mem_size += 4;
	/* update memory variables */
	*mem_base = hid_param.mem_base;
	*mem_size = hid_param.mem_size;
	return ret;
}
