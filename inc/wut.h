/*********************************************************************************

Copyright (c) 2010-2014 Analog Devices, Inc. All Rights Reserved.

This software is proprietary to Analog Devices, Inc. and its licensors.  By using 
this software you agree to the terms of the associated Analog Devices Software 
License Agreement.

*********************************************************************************/

/*!
 ****************************************************************************
 * @file:    wut.h
 * @brief:   Wakeup Timer Definitions for ADuCxxx
 * @version: $Revision: 28525 $
 * @date:    $Date: 2014-11-12 14:51:26 -0500 (Wed, 12 Nov 2014) $
 *****************************************************************************/

/*! \addtogroup WUT_Driver WUT Driver
 *  @{
 */

#ifndef __WUT_H__
#define __WUT_H__

#include "device.h"
#include "adi_int.h"
#include "adi_wut_config.h"

/* C++ linkage */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/*! \enum ADI_WUT_DEV_ID_TYPE WUT device ID. */
typedef enum
{
    ADI_WUT_DEVID_0 = 0,            /*!< The one and only wakeup timer instance. */
    ADI_WUT_MAX_DEVID
} ADI_WUT_DEV_ID_TYPE;


/*! \enum ADI_WUT_RESULT_TYPE WUT device error codes. */
typedef enum
{
   ADI_WUT_SUCCESS = 0,              /*!< Generic success.                    */
   ADI_WUT_ERR_UNKNOWN               /*!< Generic error/                      */
        = ADI_DEV_WUT_ERROR_OFFSET,

    ADI_WUT_ERR_ALREADY_INITIALIZED, /*!< WUT device is already initialized.  */
    ADI_WUT_ERR_BAD_DEV_HANDLE,      /*!< Invalid device handle.              */
    ADI_WUT_ERR_BAD_DEV_ID,          /*!< Invalid device ID.                  */
    ADI_WUT_ERR_FUNCTION_FAILED,     /*!< generic app error.                  */
    ADI_WUT_ERR_NOT_INITIALIZED,     /*!< WUT device is uninitialized.        */
    ADI_WUT_ERR_NULL_DATA_POINTER,   /*!< NULL data pointer not allowed.      */
    ADI_WUT_ERR_PARAM_OUT_OF_RANGE,  /*!< Parameter is out of range.          */
} ADI_WUT_RESULT_TYPE;


/*! \enum ADI_WUT_STATE_TYPE WUT device initialization status. */
typedef enum
{
    ADI_WUT_STATE_UNINITIALIZED= 0,              /*!< WUT is not initialized. */
    ADI_WUT_STATE_INITIALIZED                    /*!< WUT driver initialized. */
} ADI_WUT_STATE_TYPE;


/*! \enum ADI_WUT_CLOCK_SOURCE_TYPE WUT clock sources. */
typedef enum
{
    ADI_WUT_CLK_PCLK = 0,        /*!< WUT clock selector for internal PCLK.        */
    ADI_WUT_CLK_LFXTAL,          /*!< WUT clock selector for external 32k crystal. */
    ADI_WUT_CLK_LFOSC,           /*!< WUT clock selector for internal 32k oscillator. */
} ADI_WUT_CLOCK_SOURCE_TYPE;


/*! \enum ADI_WUT_MODE_TYPE WUT run modes. */
typedef enum
{
    ADI_WUT_MODE_FREERUNNING,        /*!< Set WUT to free-running mode.     */
    ADI_WUT_MODE_PERIODIC            /*!< set WUT to periodic mode.         */
} ADI_WUT_MODE_TYPE;


/*! \enum ADI_WUT_PRESCALER_TYPE WUT clock dividers. */
typedef enum
{
    ADI_WUT_PRE_DIV1     = T2CON_PRE_DIV1,      /*!< Set prescaler to clock/1.     */
    ADI_WUT_PRE_DIV16    = T2CON_PRE_DIV16,     /*!< Set prescaler to clock/16.    */
    ADI_WUT_PRE_DIV256   = T2CON_PRE_DIV256,    /*!< Set prescaler to clock/256.   */
    ADI_WUT_PRE_DIV32768 = T2CON_PRE_DIV32768,  /*!< Set prescaler to clock/32767. */
} ADI_WUT_PRESCALER_TYPE;


/*! \enum ADI_WUT_TARGET_TYPE WUT comparator designations. */
typedef enum
{
    ADI_WUT_COMPA  = (1 << BITP_WUT_T2IEN_WUFA),      /*!< Primary FieldA comparator designation.     */
    ADI_WUT_COMPB  = (1 << BITP_WUT_T2IEN_WUFB),      /*!< Secondary FieldB comparator designation.   */
    ADI_WUT_COMPC  = (1 << BITP_WUT_T2IEN_WUFC),      /*!< Secondary FieldC comparator designation.   */
    ADI_WUT_COMPD  = (1 << BITP_WUT_T2IEN_WUFD),      /*!< Secondary FieldD comparator designation.   */
    ADI_WUT_ROLL   = (1 << BITP_WUT_T2IEN_ROLL),      /*!< Rollover interrupt designation.            */
} ADI_WUT_TARGET_TYPE;

#define ADI_WUT_TARGET_MASK  ( ADI_WUT_COMPA  \
                             | ADI_WUT_COMPB  \
                             | ADI_WUT_COMPC  \
                             | ADI_WUT_COMPD  \
                             | ADI_WUT_ROLL)          /*!< target mask value */


/* device instance data handle define */
typedef struct ADI_WUT_DEV_DATA_TYPE* ADI_WUT_DEV_HANDLE;  /*!< WUT Device instance data handle typedef */


/* init/uninit API */
extern ADI_WUT_RESULT_TYPE       adi_WUT_Init               (ADI_WUT_DEV_ID_TYPE const devID, ADI_WUT_DEV_HANDLE* const pHandle);
extern ADI_WUT_RESULT_TYPE       adi_WUT_UnInit             (ADI_WUT_DEV_HANDLE const hDevice);

/* set API */
extern ADI_WUT_RESULT_TYPE       adi_WUT_SetClockSelect     (ADI_WUT_DEV_HANDLE const hDevice, const ADI_WUT_CLOCK_SOURCE_TYPE ClockSource);
extern ADI_WUT_RESULT_TYPE       adi_WUT_SetComparator      (ADI_WUT_DEV_HANDLE const hDevice, const ADI_WUT_TARGET_TYPE target, const uint32_t value);
extern ADI_WUT_RESULT_TYPE       adi_WUT_SetFreezeEnable    (ADI_WUT_DEV_HANDLE const hDevice, const bool_t bEnable);
extern ADI_WUT_RESULT_TYPE       adi_WUT_SetInterruptEnable (ADI_WUT_DEV_HANDLE const hDevice, const ADI_WUT_TARGET_TYPE target, const bool_t bEnable);
extern ADI_WUT_RESULT_TYPE       adi_WUT_SetIntervalA       (ADI_WUT_DEV_HANDLE const hDevice, const uint16_t value);
extern ADI_WUT_RESULT_TYPE       adi_WUT_SetPrescaler       (ADI_WUT_DEV_HANDLE const hDevice, const ADI_WUT_PRESCALER_TYPE PreScaler);
extern ADI_WUT_RESULT_TYPE       adi_WUT_SetTimerEnable     (ADI_WUT_DEV_HANDLE const hDevice, const bool_t bEnable);
extern ADI_WUT_RESULT_TYPE       adi_WUT_SetTimerMode       (ADI_WUT_DEV_HANDLE const hDevice, const ADI_WUT_MODE_TYPE Mode);
extern ADI_WUT_RESULT_TYPE       adi_WUT_SetWakeUpEnable    (ADI_WUT_DEV_HANDLE const hDevice, const bool_t bEnable);

/* get API */
extern ADI_WUT_CLOCK_SOURCE_TYPE adi_WUT_GetClockSelect     (ADI_WUT_DEV_HANDLE const hDevice);
extern uint32_t                  adi_WUT_GetComparator      (ADI_WUT_DEV_HANDLE const hDevice, const ADI_WUT_TARGET_TYPE target);
extern bool_t                    adi_WUT_GetFreezeEnable    (ADI_WUT_DEV_HANDLE const hDevice);
extern bool_t                    adi_WUT_GetInterruptEnable (ADI_WUT_DEV_HANDLE const hDevice, const ADI_WUT_TARGET_TYPE target);
extern uint16_t                  adi_WUT_GetIntervalA       (ADI_WUT_DEV_HANDLE const hDevice);
extern ADI_WUT_PRESCALER_TYPE    adi_WUT_GetPrescaler       (ADI_WUT_DEV_HANDLE const hDevice);
extern bool_t                    adi_WUT_GetTimerEnable     (ADI_WUT_DEV_HANDLE const hDevice);
extern ADI_WUT_MODE_TYPE         adi_WUT_GetTimerMode       (ADI_WUT_DEV_HANDLE const hDevice);
extern bool_t                    adi_WUT_GetWakeUpEnable    (ADI_WUT_DEV_HANDLE const hDevice);
extern uint16_t                  adi_WUT_GetStatusRegister  (ADI_WUT_DEV_HANDLE const hDevice);

/* misc. API */
extern uint32_t                  adi_WUT_GetCurrentCount    (ADI_WUT_DEV_HANDLE const hDevice);
extern ADI_WUT_RESULT_TYPE       adi_WUT_ClearInterruptFlag (ADI_WUT_DEV_HANDLE const hDevice, const ADI_WUT_TARGET_TYPE target);
extern ADI_WUT_RESULT_TYPE       adi_WUT_RegisterCallback   (ADI_WUT_DEV_HANDLE const hDevice, ADI_CALLBACK const cbFunc, uint32_t const cbWatch);

/* C++ linkage */
#ifdef __cplusplus
}
#endif

#endif /* include guard */

/*
** EOF
*/

/*@}*/
