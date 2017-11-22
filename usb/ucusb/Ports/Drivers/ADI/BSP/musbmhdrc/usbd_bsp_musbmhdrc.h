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
*                                  USB DEVICE DRIVER BOARD-SPECIFIC FUNCTIONS
*                                      ADI device driver implementation
*
*********************************************************************************************************
*/

/*
 * MUSBMHDRC Endpoint Definitions
 */
extern const  USBD_DRV_EP_INFO  USBD_DrvEP_InfoTbl_MUSBMHDRC[];

/*
 * MUSBMHDRC USB Board Support API
 */
extern  const  USBD_DRV_BSP_API    USBD_DrvBSP_ADI;
/*
 * For backward compatibility:
 */
#define USBD_DrvBSP_MUSBMHDRC USBD_DrvBSP_ADI
