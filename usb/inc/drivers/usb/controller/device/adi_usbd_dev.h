/*********************************************************************************

  Copyright(c) 2013 Analog Devices, Inc. All Rights Reserved.

  This software is proprietary and confidential.  By using this software you agree
  to the terms of the associated Analog Devices License Agreement.

*********************************************************************************/

/*!
* @file      adi_usbd_dev.h
*
* @brief     Wrapper header file that includes the platform specific driver headers
*
*/
#ifndef _ADI_USBD_DEV_H_
#define _ADI_USBD_DEV_H_

#ifdef _MISRA_RULES
#pragma diag(push)
#pragma diag(suppress:misra_rules_all:"Suppress all misra rules")
#endif

#include <device.h>

#include <drivers/usb/controller/device/config/adi_usbd_dev_cfg.h>

#ifdef _MISRA_RULES
#pragma diag(pop)
#endif

#endif /* _ADI_USBD_DEV_H_ */
