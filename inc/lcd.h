/*********************************************************************************

Copyright (c) 2013-2014 Analog Devices, Inc. All Rights Reserved.

This software is proprietary to Analog Devices, Inc. and its licensors.  By using 
this software you agree to the terms of the associated Analog Devices Software 
License Agreement.

*********************************************************************************/

/*!
 ******************************************************************************
 * @file:    lcd.h
 * @brief:   LCD Device API header file.
 * @version: $Revision: 28525 $
 * @date:    $Date: 2014-11-12 14:51:26 -0500 (Wed, 12 Nov 2014) $
 *****************************************************************************/

/*! \addtogroup LCD_Driver LCD Driver
 *  @{
 */

#ifndef __LCD_H__
#define __LCD_H__

#include "device.h"
#include "adi_int.h"
#include "adi_lcd_config.h"

#define ADI_LCD_MAX_BIAS_LEVEL    31         /*!< Maximum bias level supported by LCD             */

#define ADI_LCD_NUM_LCD_DATA_REG  8          /*!< Number of LCD data registers                     */

/*! LCD device handle */
typedef struct ADI_LCD_DEV_DATA_TYPE *  ADI_LCD_DEV_HANDLE;


/*!
 *****************************************************************************
 * \enum ADI_LCD_DEV_ID_TYPE
 *
 * LCD device IDs.
 *****************************************************************************/
typedef enum {
    ADI_LCD_DEVID_0 = 0,              /*!< LCD device ID                    */
    ADI_LCD_MAX_DEVID
} ADI_LCD_DEV_ID_TYPE;


/*!
 *****************************************************************************
 * \enum ADI_LCD_RESULT_TYPE
 *
 * LCD Device Error Codes.  #ADI_LCD_SUCCESS is always zero and all other LCD
 * error codes are offset by the base LCD error code offset,
 * #ADI_DEV_LCD_ERROR_OFFSET.  A number of LCD error codes are debug-only,
 * as controlled by the "ADI_DEBUG" build macro.
 *
 * The return value of all LCD APIs returning #ADI_LCD_RESULT_TYPE
 * should always be tested at the application level for success or failure.
 *
 *****************************************************************************/
typedef enum
{
    ADI_LCD_SUCCESS   = 0,              /*!< Generic success.                               */
    ADI_LCD_ERR_UNKNOWN
        = ADI_DEV_LCD_ERROR_OFFSET,     /*!< Generic error.                                 */
    ADI_LCD_ERR_BAD_DEVICE_INDEX,       /*!< Invalid physical device index.                 */
    ADI_LCD_ERR_BAD_DEVICE_HANDLE,      /*!< The given LCD handle is invalid.               */
    ADI_LCD_ERR_ALREADY_INITIALIZED,    /*!< LCD device is already initialized.             */
    ADI_LCD_ERR_NOT_INITIALIZED,        /*!< LCD device is not initialized.                 */
    ADI_LCD_ERR_BAD_BIAS_LEVEL,         /*!< The given LCD bias level is bad.               */
    ADI_LCD_ERR_NULL_POINTER,           /*!< The given pointer parameter points to NULL.    */
    ADI_LCD_ERR_BAD_REGISTER_NUM,       /*!< The given LCD register number is invalid.      */
    ADI_LCD_ERR_CP_LOW,                 /*!< Charge Pump Low.                               */
    ADI_LCD_ERR_VLCD,                   /*!< VLCD Low.                                      */
} ADI_LCD_RESULT_TYPE;

/*!
 *****************************************************************************
 * \enum ADI_LCD_EVENT_TYPE
 *
 * Enumeration of events reported via the registered callback.
 *****************************************************************************/
typedef enum
{
    ADI_LCD_EVENT_CP_GD,                /*!< Charge pump good status went low.    */
    ADI_LCD_EVENT_FRAME_BOUNDARY        /*!< Frame boundary interrupt.            */
} ADI_LCD_EVENT_TYPE;

/*!
 *****************************************************************************
 * \enum ADI_LCD_FRAME_RATE_TYPE
 *
 * Enumeration of frame rates supported by the LCD.
 *****************************************************************************/
typedef enum
{
    ADI_LCD_FRAME_RATE_128 = 0,     /*!< Frame rate 128 Hz.     */
    ADI_LCD_FRAME_RATE_102_4,       /*!< Frame rate 102.4 Hz.   */
    ADI_LCD_FRAME_RATE_85_3,        /*!< Frame rate 85.3 Hz.    */
    ADI_LCD_FRAME_RATE_73_1,        /*!< Frame rate 73.1 Hz.    */
    ADI_LCD_FRAME_RATE_64,          /*!< Frame rate 64 Hz.      */
    ADI_LCD_FRAME_RATE_56_9,        /*!< Frame rate 56.9 Hz.    */
    ADI_LCD_FRAME_RATE_51_2,        /*!< Frame rate 51.2 Hz.    */
    ADI_LCD_FRAME_RATE_46_5,        /*!< Frame rate 46.5 Hz.    */
    ADI_LCD_FRAME_RATE_42_7,        /*!< Frame rate 42.7 Hz.    */
    ADI_LCD_FRAME_RATE_39_4,        /*!< Frame rate 39.4 Hz.    */
    ADI_LCD_FRAME_RATE_36_6,        /*!< Frame rate 36.6 Hz.    */
    ADI_LCD_FRAME_RATE_34_1,        /*!< Frame rate 34.1 Hz.    */
    ADI_LCD_FRAME_RATE_32,          /*!< Frame rate 32 Hz.      */
    ADI_LCD_FRAME_RATE_30_1,        /*!< Frame rate 30.1 Hz.    */
    ADI_LCD_FRAME_RATE_28_4,        /*!< Frame rate 28.4 Hz.    */
    ADI_LCD_FRAME_RATE_26_9         /*!< Frame rate 26.9 Hz.    */

}ADI_LCD_FRAME_RATE_TYPE;

/*!
 *****************************************************************************
 * \enum ADI_LCD_SCREEN_TYPE
 *
 * Enumeration of LCD screens. LCD supports two seperate sets of data registers
 * for each screen. This allows application to modify one set which the other
 * one is getting displayed.
 *****************************************************************************/
typedef enum
{
    ADI_LCD_SCREEN_0 = 0,           /*!< LCD Screen 0. */
    ADI_LCD_SCREEN_1                /*!< LCD Screen 1. */

}ADI_LCD_SCREEN_TYPE;


/*!
 *****************************************************************************
 * \enum ADI_LCD_MUX_TYPE
 *
 * Enumeration of muxing types supported by the LCD.
 *****************************************************************************/
typedef enum
{
    ADI_LCD_MUX_STATIC = 0,         /*!< Static muxing (only COM0 line is driven). */
    ADI_LCD_MUX_4x,                 /*!< 4x multiplexing (COM0 to COM3 lines are driven). */

}ADI_LCD_MUX_TYPE;


/*!
 *****************************************************************************
 * \enum ADI_LCD_BLINK_FREQ_TYPE
 *
 * Enumeration of different blink frequencies supported by the LCD.
 *****************************************************************************/
typedef enum
{
    ADI_LCD_BLINK_FREQ_4HZ = 0,         /*!< Blink frequency 4 Hz. */
    ADI_LCD_BLINK_FREQ_2HZ,             /*!< Blink frequency 2 Hz. */
    ADI_LCD_BLINK_FREQ_1HZ,             /*!< Blink frequency 1 Hz. */
    ADI_LCD_BLINK_FREQ_2_BY_3HZ,        /*!< Blink frequency 2/3 Hz. */
    ADI_LCD_BLINK_FREQ_1_BY_2HZ,        /*!< Blink frequency 1/2 Hz. */

}ADI_LCD_BLINK_FREQ_TYPE;

/*!
 *****************************************************************************
 * \enum ADI_LCD_BLINK_MODE_TYPE
 *
 * Enumeration of different blink modes by  supported by the LCD.
 *****************************************************************************/
typedef enum
{
    ADI_LCD_BLINK_MODE_SOFTWARE  = 0,    /*!< Blink rate is controlled by software. */
    ADI_LCD_BLINK_MODE_DISABLE   = 1,    /*!< Disable blinking. */
    ADI_LCD_BLINK_MODE_HARDWARE  = 3,    /*!< Blink rate is controlled by hardware. Blink rate should be set by calling the API #adi_LCD_SetBlinkFreq. */

}ADI_LCD_BLINK_MODE_TYPE;

/*!
 *****************************************************************************
 * \struct ADI_LCD_DATA_TYPE
 *
 * structure to hold the LCD data register value.
 *****************************************************************************/
typedef struct
{
    uint8_t   LCDRegNumber;         /*!< Register number within the screen whose value to be set. */
    uint16_t  RegValue;             /*!< Register value to be set. */

}ADI_LCD_DATA_TYPE;


/* C++ linkage */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Device Initialization and Uninitialization Interface */
ADI_LCD_RESULT_TYPE adi_LCD_Init (ADI_LCD_DEV_ID_TYPE const devID, ADI_LCD_DEV_HANDLE* const phDevice);
ADI_LCD_RESULT_TYPE adi_LCD_UnInit (ADI_LCD_DEV_HANDLE  const hDevice);

/* Configuration APIs */
ADI_LCD_RESULT_TYPE adi_LCD_SetChargePumpIntEnable(ADI_LCD_DEV_HANDLE const hDevice, const bool_t bFlag);
ADI_LCD_RESULT_TYPE adi_LCD_SetFrameBoundaryIntEnable(ADI_LCD_DEV_HANDLE const hDevice, const bool_t bFlag);
ADI_LCD_RESULT_TYPE adi_LCD_SetFrameInversionModeEnable(ADI_LCD_DEV_HANDLE const hDevice, const bool_t bFlag);
ADI_LCD_RESULT_TYPE adi_LCD_SetFrameRate(ADI_LCD_DEV_HANDLE const hDevice, ADI_LCD_FRAME_RATE_TYPE const);
ADI_LCD_RESULT_TYPE adi_LCD_Reset(ADI_LCD_DEV_HANDLE const hDevice);
ADI_LCD_RESULT_TYPE adi_LCD_ToggleLCD(ADI_LCD_DEV_HANDLE const hDevice);
ADI_LCD_RESULT_TYPE adi_LCD_SetScreenSelect(ADI_LCD_DEV_HANDLE const hDevice, ADI_LCD_SCREEN_TYPE const);
ADI_LCD_RESULT_TYPE adi_LCD_SetMuxType(ADI_LCD_DEV_HANDLE const hDevice, ADI_LCD_MUX_TYPE const);
ADI_LCD_RESULT_TYPE adi_LCD_Enable(ADI_LCD_DEV_HANDLE const hDevice, const bool_t bFlag);

ADI_LCD_RESULT_TYPE adi_LCD_SetBlinkFreq(ADI_LCD_DEV_HANDLE const hDevice, ADI_LCD_BLINK_FREQ_TYPE const);
ADI_LCD_RESULT_TYPE adi_LCD_SetBlinkMode(ADI_LCD_DEV_HANDLE const hDevice, ADI_LCD_BLINK_MODE_TYPE const);
ADI_LCD_RESULT_TYPE adi_LCD_SetAutoSwitch(ADI_LCD_DEV_HANDLE const hDevice, const bool_t bFlag);
ADI_LCD_RESULT_TYPE adi_LCD_SetContrast(ADI_LCD_DEV_HANDLE const hDevice, uint8_t const BiasLevel);
ADI_LCD_RESULT_TYPE adi_LCD_EnableChargePump(ADI_LCD_DEV_HANDLE const hDevice, const bool_t bFlag);
ADI_LCD_RESULT_TYPE adi_LCD_SetDataReg(ADI_LCD_DEV_HANDLE const hDevice,  ADI_LCD_SCREEN_TYPE   const eScreen, ADI_LCD_DATA_TYPE *pDataTable, uint32_t const NumEntries);


/* Status query APIs */
ADI_LCD_RESULT_TYPE adi_LCD_GetDataReg(ADI_LCD_DEV_HANDLE const hDevice,  ADI_LCD_SCREEN_TYPE   const eScreen, ADI_LCD_DATA_TYPE *pDataTable, uint32_t const NumEntries);
ADI_LCD_RESULT_TYPE adi_LCD_GetIdleStatus(ADI_LCD_DEV_HANDLE const hDevice, bool_t * const pStatus);
ADI_LCD_RESULT_TYPE adi_LCD_GetVLCDStatus(ADI_LCD_DEV_HANDLE const hDevice, bool_t * const pStatus);
ADI_LCD_RESULT_TYPE adi_LCD_GetChargePumpGoodStatus(ADI_LCD_DEV_HANDLE const hDevice, bool_t * const pStatus);
ADI_LCD_RESULT_TYPE adi_LCD_GetSafeToWriteStatus(ADI_LCD_DEV_HANDLE const hDevice, bool_t * const pStatus);

/* Callback registration */
ADI_LCD_RESULT_TYPE adi_LCD_RegisterCallback(ADI_LCD_DEV_HANDLE const hDevice, ADI_CALLBACK pfCallback, void *pCBParam);


/* C++ linkage */
#ifdef __cplusplus
}
#endif

#endif /* include guard */

/*
** EOF
*/

/*@}*/
