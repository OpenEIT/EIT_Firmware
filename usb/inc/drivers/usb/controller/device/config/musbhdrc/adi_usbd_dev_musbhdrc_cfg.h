/*********************************************************************************

  Copyright(c) 2013 Analog Devices, Inc. All Rights Reserved.

  This software is proprietary and confidential.  By using this software you agree
  to the terms of the associated Analog Devices License Agreement.

*********************************************************************************/

/*!
* @file      adi_usbd_dev_musbhdrc_cfg.h
*
* @brief     MUSBHDRC configuration file
*/
#ifndef _ADI_USBD_DEV_MUSBHDRC_CFG_H_
#define _ADI_USBD_DEV_MUSBHDRC_CFG_H_

#ifdef _MISRA_RULES
#pragma diag(push)
#pragma diag(suppress:misra_rules_all:"Suppress all misra rules")
#endif

#include <services/gpio/adi_gpio.h>

#define ADI_CFG_USB_MAX_NBR_HC              (1u)                             /* Number of USB host controllers possible with this processor */

#define ADI_CFG_USB_MAX_NUM_EP              (8u)                             /* Maximum possible number of physical Endpoints available     */
#define ADI_CFG_USB_MAX_DMA_CHANNELS        (8u)                             /* Maximum possible number of DMA channels available           */

#if defined(ADI_DEBUG)
#define ADI_CFG_USB_STATS                   (1u)                             /* USB stats is ON by default                                  */
#endif

#define ADI_CFG_USB_DEVICE                  (1u)                             /* USB controller is configured as DEVICE                         */
#define ADI_CFG_USB_MAX_NUM_URB             (2u*ADI_CFG_USB_MAX_NUM_EP - 1u) /* 1 Control EP, 7 Tx Data EP, 7 Rx Data EP                    */


/*
 * ADSP-BF526 - DMA mode1 operation
 * ADSP-BF527 - DMA mode0 operation
 * ADSP-BF548 - DMA mode0 operation
 */
#if defined(__ADSPBF526_FAMILY__)
#define ADI_CFG_USB_ENABLE_DMA_MODE1        (1u)                             /* DMA mode 1 enabled                                          */
#define ADI_CFG_USB_VBUS_PORT               (ADI_GPIO_PORT_G)
#define ADI_CFG_USB_VBUS_PIN                (ADI_GPIO_PIN_13)
#define ADI_CFG_USB_VBUS_ASSRT_POLARITY     1u
#define ADI_CFG_USB_PLL_OSCVAL              (0x30a8)                         /* PLL OSC value */
#endif

#if defined(__ADSPBF527_FAMILY__)
#define ADI_CFG_USB_VBUS_PORT               (ADI_GPIO_PORT_G)
#define ADI_CFG_USB_VBUS_PIN                (ADI_GPIO_PIN_13)
#define ADI_CFG_USB_VBUS_ASSRT_POLARITY     0u
#define ADI_CFG_USB_PLL_OSCVAL              (0x30a8)                         /* PLL OSC value */
#endif

#if defined(__ADSPBF548_FAMILY__)
#define ADI_CFG_USB_VBUS_PORT               (ADI_GPIO_PORT_E)
#define ADI_CFG_USB_VBUS_PIN                (ADI_GPIO_PIN_7)
#define ADI_CFG_USB_VBUS_ASSRT_POLARITY     0u
#define ADI_CFG_USB_PLL_OSCVAL              (0x30a8)                         /* PLL OSC value */
#endif

#ifdef _MISRA_RULES
#pragma diag(pop)
#endif

#endif /* _ADI_USBD_DEV_MUSBHDRC_CFG_H_ */
