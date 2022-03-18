/*
 * @brief Vitual communication port example
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

#include <stdio.h>
#include <string.h>
#include "app_usbd_cfg.h"
#include "cdc_vcom.h"
#include "delay.h"

#define __VTOR_PRESENT 1

const uint32_t OscRateIn = 12000000;
const uint32_t RTCOscRateIn = 32768;

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

static USBD_HANDLE_T g_hUsb;
//static uint8_t g_rxBuff[256];

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

const USBD_API_T *g_pUsbApi;

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/**
 * @brief	Handle interrupt from USB0
 * @return	Nothing
 */

//struct Settings Settings={.SWV="v0.1", .PASS="AAAA", .ISP_Mode=0};
//struct Settings Settings;


/* Find the address of interface descriptor for given class type. */
USB_INTERFACE_DESCRIPTOR *find_IntfDesc(const uint8_t *pDesc,
		uint32_t intfClass)
{
	USB_COMMON_DESCRIPTOR *pD;
	USB_INTERFACE_DESCRIPTOR *pIntfDesc = 0;
	uint32_t next_desc_adr;

	pD = (USB_COMMON_DESCRIPTOR *) pDesc;
	next_desc_adr = (uint32_t) pDesc;

	while (pD->bLength)
	{
		/* is it interface descriptor */
		if (pD->bDescriptorType == USB_INTERFACE_DESCRIPTOR_TYPE)
		{

			pIntfDesc = (USB_INTERFACE_DESCRIPTOR *) pD;
			/* did we find the right interface descriptor */
			if (pIntfDesc->bInterfaceClass == intfClass)
			{
				break;
			}
		}
		pIntfDesc = 0;
		next_desc_adr = (uint32_t) pD + pD->bLength;
		pD = (USB_COMMON_DESCRIPTOR *) next_desc_adr;
	}

	return pIntfDesc;
}

/**
 * @brief	main routine for blinky example
 * @return	Function should not exit.
 */
int USB_CLI_Start(void)
{

	USBD_API_INIT_PARAM_T usb_param;
	USB_CORE_DESCS_T desc;
	ErrorCode_t ret = LPC_OK;
	uint32_t prompt = 0;//, rdCnt = 0;

	SystemCoreClockUpdate();
	/* Initialize board and chip */
	//Board_Init();

	//Settings_Save();


	/* enable clocks and pinmux */
	Chip_USB_Init();

	/* initialize USBD ROM API pointer. */
	//g_pUsbApi = (const USBD_API_T *) LPC_ROM_API->usbdApiBase;

	/* initialize call back structures */
	memset((void *) &usb_param, 0, sizeof(USBD_API_INIT_PARAM_T));
	//usb_param.usb_reg_base = LPC_USB0_BASE;
	/*	WORKAROUND for artf44835 ROM driver BUG:
	 Code clearing STALL bits in endpoint reset routine corrupts memory area
	 next to the endpoint control data. For example When EP0, EP1_IN, EP1_OUT,
	 EP2_IN are used we need to specify 3 here. But as a workaround for this
	 issue specify 4. So that extra EPs control structure acts as padding buffer
	 to avoid data corruption. Corruption of padding memory doesn’t affect the
	 stack/program behaviour.
	 */
	usb_param.max_num_ep = 3 + 1;
	usb_param.mem_base = USB_STACK_MEM_BASE;
	usb_param.mem_size = USB_STACK_MEM_SIZE;

	/* Set the USB descriptors */
	desc.device_desc = (uint8_t *) &USB_DeviceDescriptor[0];
	desc.string_desc = (uint8_t *) &USB_StringDescriptor[0];
	/* Note, to pass USBCV test full-speed only devices should have both
	 descriptor arrays point to same location and device_qualifier set to 0.
	 */
	desc.high_speed_desc = (uint8_t *) &USB_FsConfigDescriptor[0];
	desc.full_speed_desc = (uint8_t *) &USB_FsConfigDescriptor[0];
	desc.device_qualifier = 0;

	/* USB Initialization */
	ret = USBD_API->hw->Init(&g_hUsb, &desc, &usb_param);
	if (ret == LPC_OK)
	{
		usb_param.mem_base = USB_STACK_MEM_BASE
				+ (USB_STACK_MEM_SIZE - usb_param.mem_size);

		/* Init VCOM interface */
		ret = vcom_init(g_hUsb, &desc, &usb_param);
		if (ret == LPC_OK)
		{
			/*  enable USB interrupts */
			//NVIC_EnableIRQ(USB0_IRQn);
			/* now connect */
			USBD_API->hw->Connect(g_hUsb, 1);
		}

	}


	while (prompt == 0)
	{
		if ((vcom_connected() != 0) && (prompt == 0))
		{
			//Settings_Restore();

			short_delay(20);

			short_delay(20);
			//CLI_clear_screen();
			USART0_IRQHandler();
			short_delay(10);

			prompt = 1;

		}
		//Chip_WWDT_Feed(LPC_WWDT);
	}
	return 0;
}
