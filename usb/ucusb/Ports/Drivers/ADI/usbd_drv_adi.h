/*
*********************************************************************************************************
*                                    uC/USB-Device Controller Driver
*                                      ADI Driver Interface Layer
*
*                    Copyright(c) 2012-2014 Analog Devices, Inc. All Rights Reserved.
*
*               This software is proprietary and confidential. By using this software you agree
*               to the terms of the associated Analog Devices License Agreement.
*
*                          (c) Copyright 2004-2011; Micrium, Inc.; Weston, FL
*
*               All rights reserved. Protected by international copyright laws.
*
*               uC/USB is provided in source form to registered licensees ONLY.  It is
*               illegal to distribute this source code to any third party unless you receive
*               written permission by an authorized Micrium representative.  Knowledge of
*               the source code may NOT be used to develop a similar product.
*
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*
*               You can contact us at www.micrium.com.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                  DEVICE CONTROLLER DRIVER INTERFACE LAYER
*
*
* File           : usbd_drv_adi.h
* Version        : 0.0
* Programmer(s)  :
*********************************************************************************************************
*/


#ifndef  _USBD_DRV_ADI_H_
#define  _USBD_DRV_ADI_H_

#ifdef _MISRA_RULES
#pragma diag(push)
#pragma diag(suppress:misra_rules_all:"Suppress all misra rules")
#endif

/*
*********************************************************************************************************
*                                              INCLUDE FILES
*********************************************************************************************************
*/

#include "usbd_core.h"
#include <drivers/usb/controller/device/adi_usbd_dev.h>

#include  <usbd_bsp_dev.h>

/*
*********************************************************************************************************
*                                                 DEFINES
*********************************************************************************************************
*/
#define ADI_USB_GET_EP_NUM(EpAddr)                  ((EpAddr) & 0x0F)
#define ADI_USB_EP_IS_IN(EpAddr)                    ((EpAddr) & 0x80)

/* This Macro is used only in the case of the data Endpoints
 * Index is calculated as follows
 * IN EP1:  Index 1
 * OUT EP1: Index 2
 * IN EP2:  Index 3
 * OUT EP2: Index 4
 * IN EP3:  Index 5
 * OUT EP3: Index 6
 * ....
 */
#define ADI_USB_GET_URB_INDEX(EpAddr)               (ADI_USB_EP_IS_IN(EpAddr) ? (2*ADI_USB_GET_EP_NUM(EpAddr)-1):(2*ADI_USB_GET_EP_NUM(EpAddr)))

#define ADI_USB_GET_EP_TYPE(EpAttr)                 ((EpAttr) & 0x3)

/*
*********************************************************************************************************
*                                            GLOBAL VARIABLES
*********************************************************************************************************
*/

#if defined(__ADSPBF533_FAMILY__) || defined(__ADSPBF537_FAMILY__) || defined(__ADSPBF538_FAMILY__) || defined(__ADSPBF518_FAMILY__) || defined(__ADSPBF561_FAMILY__) || defined(__214xx__)
extern ADI_USB_DRV_API  ADI_NET2272_DeviceDrvAPI;
#else
extern ADI_USB_DRV_API  ADI_MUSBMHDRC_DeviceDrvAPI;
#endif
extern 	const 	USBD_DRV_API 	USBD_DrvAPI_ADI;

#ifdef _MISRA_RULES
#pragma diag(pop)
#endif

#endif /* _USBD_DRV_ADI_H_ */
