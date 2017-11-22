/*
*********************************************************************************************************
*                                            uC/USB-Device
*                                       Device Configuration Module
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
*                                                   Header File
*
* File           : usbd_bsp_dev.h
* Version        : V4.02.00
* Programmer(s)  : MJK
*********************************************************************************************************
*/

#ifdef USBD_DRV_DEBUG
#include <adi_usbd_dev.h>
#else
#include <drivers/usb/controller/device/adi_usbd_dev.h>
#endif


#if defined(__ADSPBF527_FAMILY__) || defined(__ADSPBF526_FAMILY__)  || defined(__ADSPBF548_FAMILY__) || defined(__ADSPBF548M_FAMILY__)

#include  "musbhdrc/usbd_bsp_musbhdrc.h"

#elif defined(__ADSPBF609_FAMILY__) || defined(__ADSPBF707_FAMILY__)

#include  "musbmhdrc/usbd_bsp_musbmhdrc.h"

#elif defined(ADI_ADUCM350)

#include  "musbmhdrc/usbd_bsp_musbmhdrc.h"

#elif defined(__ADSPBF533_FAMILY__) || defined(__ADSPBF537_FAMILY__) || defined(__ADSPBF538_FAMILY__) || defined(__ADSPBF518_FAMILY__) || defined(__ADSPBF561_FAMILY__)

#include  "net2272/usbd_bsp_net2272.h"

#endif

