/*********************************************************************************

  Copyright(c) 2013 Analog Devices, Inc. All Rights Reserved.

  This software is proprietary and confidential.  By using this software you agree
  to the terms of the associated Analog Devices License Agreement.

*********************************************************************************/

/*!
* @file      adi_usbd_dev_cfg.h
*
* @brief     Wrapper header file that includes the platform specific configuration headers
*
*/
#ifndef _ADI_USBD_DEV_CFG_H_
#define _ADI_USBD_DEV_CFG_H_

#ifdef _MISRA_RULES
#pragma diag(push)
#pragma diag(suppress:misra_rules_all:"Suppress all misra rules")
#endif

#if defined(__ADSPBF526_FAMILY__) || defined(__ADSPBF527_FAMILY__) || defined(__ADSPBF548_FAMILY__)
#define ADI_CFG_USB_BF_MUSBHDRC
#elif defined(__ADSPBF609_FAMILY__) || defined(__ADSPBF707_FAMILY__)
#define ADI_CFG_USB_BF_MUSBMHDRC
#elif defined(__ADSPSC589_FAMILY__)       /* Griffin  */
#define ADI_CFG_USB_BF_MUSBMHDRC
#elif defined(ADI_ADUCM350)               /* aduCM350 */
#define ADI_CFG_USB_ARM_MUSBMHDRC
#elif defined(__ADSPCM40x_FAMILY__)
#define ADI_CFG_USB_MUSBMHDRC
#endif

#if defined(ADI_CFG_USB_BF_MUSBMHDRC) || defined(ADI_CFG_USB_ARM_MUSBMHDRC)
#include <drivers/usb/controller/device/config/musbmhdrc/adi_usbd_dev_musbmhdrc_cfg.h>
#elif defined(ADI_CFG_USB_BF_MUSBHDRC)
#include <drivers/usb/controller/device/config/musbhdrc/adi_usbd_dev_musbhdrc_cfg.h>
#else
#error "processor not supported"
#endif

#include <drivers/usb/controller/musbmhdrc/adi_usb_dev_musbmhdrc.h>

#ifdef _MISRA_RULES
#pragma diag(pop)
#endif

#endif /* _ADI_USBD_DEV_CFG_H_ */
