/*********************************************************************************

  Copyright(c) 2013 Analog Devices, Inc. All Rights Reserved.

  This software is proprietary and confidential.  By using this software you agree
  to the terms of the associated Analog Devices License Agreement.

*********************************************************************************/

/*!
* @file      adi_usbd_dev_musbmhdrc_cfg.h
*
* @brief     MUSBMHDRC configuration file
*/
#ifndef _ADI_USBD_DEV_MUSBMHDRC_CFG_H_
#define _ADI_USBD_DEV_MUSBMHDRC_CFG_H_

#ifdef _MISRA_RULES
#pragma diag(push)
#pragma diag(suppress:misra_rules_all:"Suppress all misra rules")
#endif

#define ADI_CFG_USB_MAX_NBR_HC              (1u)                             /* Number of USB host controllers possible with this processor    */

#if defined(ADI_ADUCM350)
#define ADI_CFG_USB_MAX_NUM_EP              (4u)                             /* Maximum possible number of physical Endpoints available        */
#define ADI_CFG_USB_MAX_DMA_CHANNELS        (2u)                             /* Maximum possible number of DMA channels available              */
#else
#define ADI_CFG_USB_MAX_NUM_EP              (12u)                            /* Maximum possible number of physical Endpoints available        */
#define ADI_CFG_USB_MAX_DMA_CHANNELS        (8u)                             /* Maximum possible number of DMA channels available              */
#endif

#if defined(ADI_DEBUG)
#define ADI_CFG_USB_STATS                   (1u)                             /* USB stats is ON by default                                     */
#endif

#define ADI_CFG_USB_DEVICE                  (1u)                             /* USB controller is configured as DEVICE                         */
#define ADI_CFG_USB_MAX_NUM_URB             (2u*ADI_CFG_USB_MAX_NUM_EP - 1u) /* 1 Control EP, 11 Tx Data EP, 11 Rx Data EP                     */

#if !defined(ADI_ADUCM350) /* DMA mode 1 not valid for Full speed devices */
#define ADI_CFG_USB_ENABLE_DMA_MODE1        (1u)                             /* DMA mode 1 enabled                                             */
#endif

#ifdef _MISRA_RULES
#pragma diag(pop)
#endif

#endif /* _ADI_USBD_DEV_MUSBMHDRC_CFG_H_ */
