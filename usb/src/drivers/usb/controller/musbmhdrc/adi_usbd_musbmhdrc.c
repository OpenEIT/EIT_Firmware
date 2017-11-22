/*********************************************************************************

  Copyright(c) 2013 Analog Devices, Inc. All Rights Reserved.

  This software is proprietary and confidential.  By using this software you agree
  to the terms of the associated Analog Devices License Agreement.

*********************************************************************************/

/*!
* @file      adi_usbd_musbmhdrc.c
*
* @brief     Device mode specific container for all USB driver files.
*
*
*/

/** @addtogroup USB MUSBMHDRC controller driver
 *  @{
 */

#ifdef _MISRA_RULES
#pragma diag(push)
#pragma diag(suppress:misra_rules_all:"Suppress all misra rules")
#endif

//#define ADI_DEBUG         (1)

#include <drivers/usb/controller/device/adi_usbd_dev.h>
#include "adi_usb_dev_musbmhdrc_platform.h"
#include "adi_usb_dev_musbmhdrc_local.h"

/* driver source files are directly included */
#include "adi_usb_dev_musbmhdrc_platform.c"
#include "adi_usb_dev_common.c"
#include "adi_usb_dev_musbmhdrc.c"
#include "adi_usb_dev_musbmhdrc_intrpt.c"


#ifdef _MISRA_RULES
#pragma diag(pop)
#endif
