/*********************************************************************************

Copyright (c) 2012-2014 Analog Devices, Inc. All Rights Reserved.

This software is proprietary to Analog Devices, Inc. and its licensors.  By using 
this software you agree to the terms of the associated Analog Devices Software 
License Agreement (See /LICENSES/ADI.LABLAB.LICENSE). 

Modified from the Analog Devices original.
Portions Copyright (c) 2018 Andrew Grosser, Moritz von Buttlar.

*********************************************************************************/

/*!
 *****************************************************************************
   @file:    adi_crc_config.h
   @brief:   Configuration options for CRC driver.
             This is specific to the CRC driver and will be included by the driver.
             It is not required for the application to include this header file.
   @version: $Revision: 28525 $
   @date:    $Date: 2014-11-12 14:51:26 -0500 (Wed, 12 Nov 2014) $
*****************************************************************************/

#ifndef __ADI_CRC_CONIG_H__
#define __ADI_CRC_CONIG_H__


/************* CRC Driver configurations ***************/
/*!
   Enable DMA support in the driver code.
   1 -  To have the DMA support code in the driver.
   0 -  To eliminate the DMA support. Operates in core mode.
*/
#define ADI_CRC_CFG_ENABLE_DMA_SUPPORT         1

/*!
   Enable RTOS  support in the driver code.
   1 -  To have the RTOS support code in the driver.
   0 -  To eliminate the RTOS  support.
*/
#define ADI_CFG_ENABLE_RTOS_SUPPORT            0


/*!
   Enable RTOS  support in the driver code.
   1 -  To have the RTOS support code in the driver.
   0 -  To eliminate the RTOS  support.
*/
#define ADI_CFG_ENABLE_CALLBACK_SUPPORT       0

#if ((ADI_CRC_CFG_ENABLE_DMA_SUPPORT == 0 ) && ( ADI_CFG_ENABLE_CALLBACK_SUPPORT == 1) ) 
#error " Call back mode of operation is supportted only in DMA mode"
#endif

#endif /* __ADI_CRC_CONIG_H__ */
