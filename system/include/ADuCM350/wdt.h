/*********************************************************************************

Copyright (c) 2010-2014 Analog Devices, Inc. All Rights Reserved.

This software is proprietary to Analog Devices, Inc. and its licensors.  By using 
this software you agree to the terms of the associated Analog Devices Software 
License Agreement (See /LICENSES/ADI.LABLAB.LICENSE). 

Modified from the Analog Devices original.
Portions Copyright (c) 2018 Andrew Grosser, Moritz von Buttlar.

*********************************************************************************/

/*!
 ******************************************************************************
 * @file:    wdt.h
 * @brief:   Watchdog Timer Device Definitions for ADuCxxx
 * @version: $Revision: 28525 $
 * @date:    $Date: 2014-11-12 14:51:26 -0500 (Wed, 12 Nov 2014) $
*****************************************************************************/

/*! \addtogroup WDT_Driver WDT Driver
 *  @{
 */

#ifndef __WDT_H__
#define __WDT_H__

#include "device.h"
#include "config/adi_wdt_config.h"

/* C++ linkage */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Define this macro when silicon anomaly item # 31-02-0044 is fixed */
/* This anomaly concerns incorrect behavior of the T3CON control register LOCK bit */
/* Meantime, reference to the LOCK bit is avoided. */
/* This workaround is required for Silicon revision 2 as well */
//#define ADI_WDT_HW_ANOMALY_RESOLVED


/*! \enum ADI_WDT_DEV_ID_TYPE Watchdog Timer Device ID. */
typedef enum
{
    ADI_WDT_DEVID_0 = 0,             /*!< The one and only watchdog timer device instance */
    ADI_WDT_MAX_DEVID
} ADI_WDT_DEV_ID_TYPE;


/*! \enum ADI_WDT_RESULT_TYPE Watchdog status/result codes. */
typedef enum
{
    ADI_WDT_SUCCESS = 0,            /*!< No Error (generic success). */

    ADI_WDT_ERR_UNKNOWN
        = ADI_DEV_WDT_ERROR_OFFSET,  /*!< An unknown error was detected. */

    ADI_WDT_ERR_ALREADY_INITIALIZED, /*!< Watchdog is already initialized (if debug mode). */
    ADI_WDT_ERR_BAD_DEV_DATA,        /*!< Pointer does not point to valid watchdog data. */
    ADI_WDT_ERR_BAD_DEV_ID,          /*!< There is only a single Watchdog instance. */
    ADI_WDT_ERR_BAD_PRESCALE,        /*!< Invalid prescale value specified. */
    ADI_WDT_ERR_INITIALIZATION_FAIL, /*!< Watchdog initialization failed. */
    ADI_WDT_ERR_LOCKED,              /*!< Watchdog is Locked. */
    ADI_WDT_ERR_NOT_INITIALIZED,     /*!< Watchdog not yet initialized (if debug mode). */
    ADI_WDT_SYNC_IN_PROGRESS         /*!< Bus Sync after write is in progress. */
} ADI_WDT_RESULT_TYPE;


/*!
 *****************************************************************************
 * \enum ADI_WDT_STATE_TYPE
 *
 * WDT driver state.  Used for internal tracking of the WDT device initialization
 * progress during the adi_WDT_Init().  Also used to insure the WDT device has been
 * properly initialized as a prerequisite to using the balance of the WDT API.
 *
 *****************************************************************************/
typedef enum
{
    ADI_WDT_STATE_UNINITIALIZED= 0,        /*!< WDT is not initialized.           */
    ADI_WDT_STATE_INITIALIZED,             /*!< WDT is initialized.               */
} ADI_WDT_STATE_TYPE;


/*! \enum ADI_WDT_PRESCALE_TYPE Watchdog Prescale Values Definition. */
typedef enum
{
    ADI_WDT_PRESCALE_1 =    T3CON_PRE_DIV1,     /*!< Clock is not scaled. */
    ADI_WDT_PRESCALE_16 =   T3CON_PRE_DIV16,    /*!< Clock/16 (i.e 2048Hz). */
    ADI_WDT_PRESCALE_256 =  T3CON_PRE_DIV256,   /*!< Clock/256 (i.e. 128Hz). */
    ADI_WDT_PRESCALE_4096 = T3CON_PRE_DIV4096   /*!< Clock/4096 (i.e. 8Hz). */
} ADI_WDT_PRESCALE_TYPE;


typedef struct ADI_WDT_DEV_DATA_TYPE* ADI_WDT_DEV_HANDLE;    /*!< WDT Device instance private data handle typedef. */


/* Open/Close APIs */
extern ADI_WDT_RESULT_TYPE      adi_WDT_Init                 (const ADI_WDT_DEV_ID_TYPE DevID, ADI_WDT_DEV_HANDLE * const pHandle);
extern ADI_WDT_RESULT_TYPE      adi_WDT_UnInit               (ADI_WDT_DEV_HANDLE const hDevice);

/* Set APIs */
extern ADI_WDT_RESULT_TYPE      adi_WDT_SetEnable            (ADI_WDT_DEV_HANDLE const hDevice, const bool_t bEnable);
extern ADI_WDT_RESULT_TYPE      adi_WDT_SetIRQMode           (ADI_WDT_DEV_HANDLE const hDevice, const bool_t bIRQMode);
extern ADI_WDT_RESULT_TYPE      adi_WDT_SetLoadCount         (ADI_WDT_DEV_HANDLE const hDevice, const uint16_t LoadCount);
extern ADI_WDT_RESULT_TYPE      adi_WDT_SetPDClear           (ADI_WDT_DEV_HANDLE const hDevice, const bool_t bPDClear);
extern ADI_WDT_RESULT_TYPE      adi_WDT_SetPrescale          (ADI_WDT_DEV_HANDLE const hDevice, const ADI_WDT_PRESCALE_TYPE Prescale);
extern ADI_WDT_RESULT_TYPE      adi_WDT_SetWaitMode          (ADI_WDT_DEV_HANDLE const hDevice, const bool_t bWaitMode);

/* Get APIs */
extern uint16_t                 adi_WDT_GetCount             (ADI_WDT_DEV_HANDLE const hDevice);
extern bool_t                   adi_WDT_GetEnable            (ADI_WDT_DEV_HANDLE const hDevice);
extern bool_t                   adi_WDT_GetIRQMode           (ADI_WDT_DEV_HANDLE const hDevice);
extern bool_t                   adi_WDT_GetPDClear           (ADI_WDT_DEV_HANDLE const hDevice);
extern ADI_WDT_PRESCALE_TYPE    adi_WDT_GetPrescale          (ADI_WDT_DEV_HANDLE const hDevice);
extern bool_t                   adi_WDT_GetWaitMode          (ADI_WDT_DEV_HANDLE const hDevice);

/* Status APIs */
extern bool_t                   adi_WDT_GetCLRISyncStatus    (ADI_WDT_DEV_HANDLE const hDevice);
extern bool_t                   adi_WDT_GetCONSyncStatus     (ADI_WDT_DEV_HANDLE const hDevice);
extern bool_t                   adi_WDT_GetIRQPendStatus     (ADI_WDT_DEV_HANDLE const hDevice);
extern bool_t                   adi_WDT_GetLDSyncStatus      (ADI_WDT_DEV_HANDLE const hDevice);
extern bool_t                   adi_WDT_GetLockedStatus      (ADI_WDT_DEV_HANDLE const hDevice);

/* Operational APIs */
extern ADI_WDT_RESULT_TYPE      adi_WDT_ResetTimer           (ADI_WDT_DEV_HANDLE const hDevice);
extern ADI_WDT_RESULT_TYPE      adi_WDT_Trigger              (ADI_WDT_DEV_HANDLE const hDevice);

/* C++ linkage */
#ifdef __cplusplus
}
#endif

#endif /* include guard */

/*
** EOF
*/

/*@}*/
