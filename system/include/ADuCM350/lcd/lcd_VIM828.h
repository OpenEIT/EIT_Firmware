/*********************************************************************************

Copyright (c) 2012-2014 Analog Devices, Inc. All Rights Reserved.

This software is proprietary to Analog Devices, Inc. and its licensors.  By using 
this software you agree to the terms of the associated Analog Devices Software 
License Agreement (See /LICENSES/ADI.LABLAB.LICENSE). 

Modified from the Analog Devices original.
Portions Copyright (c) 2018 Andrew Grosser, Moritz von Buttlar.

*********************************************************************************/

/*****************************************************************************
 * @file:    lcd_VIM828.h
 * @brief:   VIM-828 LCD API header file.
 * @version: $Revision: 28525 $
 * @date:    $Date: 2014-11-12 14:51:26 -0500 (Wed, 12 Nov 2014) $
*****************************************************************************/

#ifndef __LCD_VIM828_H__
#define __LCD_VIM828_H__

#include "lcd.h"

/*! \addtogroup LCD_Driver LCD Driver
 *  @{
 */

/*! Number of segments in the display */
#define ADI_LCDVIM828_NUM_SEGMENTS    8

/*!
 *****************************************************************************
 * \enum ADI_LCDVIM828_RESULT_TYPE
 *
 * LCD Device Error Codes.  #ADI_LCDVIM828_SUCCESS is always zero and all other LCD
 * error codes are offset by the base LCD error code offset,
 * #ADI_DEV_LCDVIM828_ERROR_OFFSET.  A number of LCD error codes are debug-only,
 * as controlled by the "ADI_DEBUG" build macro.
 *
 * The return value of all LCD APIs returning #ADI_LCDVIM828_RESULT_TYPE
 * should always be tested at the application level for success or failure.
 *
 *****************************************************************************/
typedef enum
{
    ADI_LCDVIM828_SUCCESS                  = 0,                                     /*!< Generic success.                         */
    ADI_LCDVIM828_ERR_UNKNOWN              = ADI_DEV_LCDVIM828_ERROR_OFFSET,        /*!< Generic error.                           */
    ADI_LCDVIM828_ERR_BAD_SEG_NUM          = (ADI_DEV_LCDVIM828_ERROR_OFFSET + 1),  /*!< The given segment number is invalid.     */
    ADI_LCDVIM828_ERR_BAD_CHAR             = (ADI_DEV_LCDVIM828_ERROR_OFFSET + 2),  /*!< The given character for display is bad.  */
    ADI_LCDVIM828_ERR_NULL_POINTER         = (ADI_DEV_LCDVIM828_ERROR_OFFSET + 3),  /*!< The given pointer is pointing to NULL.   */
    ADI_LCDVIM828_ERR_BAD_DEVICE_HANDLE    = ADI_LCD_ERR_BAD_DEVICE_HANDLE,         /*!< The given device handle is invalid.      */
    ADI_LCDVIM828_ERR_NOT_INITIALIZED      = ADI_LCD_ERR_NOT_INITIALIZED            /*!< The given device is not initialized.     */

} ADI_LCDVIM828_RESULT_TYPE;

/* Display and clear APIs */
ADI_LCDVIM828_RESULT_TYPE       adi_LCDVIM828_DisplayString     (ADI_LCD_DEV_HANDLE const   hDevice, 
                                                                 ADI_LCD_SCREEN_TYPE const  Screen, 
                                                                 uint8_t const              DisplayString[]);
ADI_LCDVIM828_RESULT_TYPE       adi_LCDVIM828_ClearDisplay      (ADI_LCD_DEV_HANDLE const   hDevice, 
                                                                 ADI_LCD_SCREEN_TYPE const  Screen);

/*@}*/
#endif /* __LCD_VIM828_H__ */
