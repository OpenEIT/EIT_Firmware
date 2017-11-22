/*********************************************************************************

Copyright (c) 2012-2014 Analog Devices, Inc. All Rights Reserved.

This software is proprietary to Analog Devices, Inc. and its licensors.  By using 
this software you agree to the terms of the associated Analog Devices Software 
License Agreement.

*********************************************************************************/

/*!
 *****************************************************************************
   @file:    adi_afe_config.h
   @brief:   Configuration options for AFE driver.
             This is specific to the AFE driver and will be included by the driver.
             It is not required for the application to include this header file.
   @version: $Revision: 28525 $
   @date:    $Date: 2014-11-12 14:51:26 -0500 (Wed, 12 Nov 2014) $
*****************************************************************************/

#ifndef __ADI_AFE_CONFIG_H__
#define __ADI_AFE_CONFIG_H__

/************* AFE Driver configurations ***************/

/*!
   Enable Rx DMA dual buffer support in the driver code.
   1 -  To have the Rx DMA dual buffer support code in the driver.
   0 -  To eliminate the Rx DMA dual buffer support.
*/
#define ADI_AFE_CFG_ENABLE_RX_DMA_DUAL_BUFFER_SUPPORT           1

#if ( ADI_AFE_CFG_ENABLE_RX_DMA_DUAL_BUFFER_SUPPORT > 1 )
#error "Invalid configuration"
#endif

/*!
   Enable deprecated AFE functions.
   Over time, some AFE functions may be deprecated.
   1 - To enable deprecated AFE functions
   0 - To disable deprecated AFE functions
   
   Deprecated functions:
   adi_AFE_RegisterCallbackOnCommandFIFO
   adi_AFE_RegisterCallbackOnDataFIFO
*/
#define ADI_AFE_CFG_ENABLE_DEPRECATED_FUNCTION_SUPPORT          1

#if ( ADI_AFE_CFG_ENABLE_DEPRECATED_FUNCTION_SUPPORT > 1 )
#error "Invalid configuration"
#endif

/************* AFE controller configurations ***************/

/************** Macro validation *****************************/


#endif /* __ADI_AFE_CONFIG_H__ */
