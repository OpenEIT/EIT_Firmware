/*********************************************************************************

  Copyright(c) 2013 Analog Devices, Inc. All Rights Reserved.

  This software is proprietary and confidential.  By using this software you agree
  to the terms of the associated Analog Devices License Agreement.

*********************************************************************************/

/*!
* @file      adi_usbh_musbmhdrc.c
*
* @brief     Host mode specific container for all USB driver files.
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

#include <drivers/usb/controller/host/adi_usbh_dev.h>
#include "adi_usb_dev_musbmhdrc_platform.h"
#include "adi_usb_dev_musbmhdrc_local.h"

/* driver source files are directly included */
#include "adi_usb_dev_musbmhdrc_platform.c"
#include "adi_usb_dev_musbmhdrc.c"
#include "adi_usb_dev_musbmhdrc_intrpt.c"
#include "adi_usb_dev_common.c"

#ifdef _MISRA_RULES
#pragma diag(pop)
#endif
