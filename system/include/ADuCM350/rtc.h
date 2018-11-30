/*********************************************************************************

Copyright (c) 2010-2014 Analog Devices, Inc. All Rights Reserved.

This software is proprietary to Analog Devices, Inc. and its licensors.  By using 
this software you agree to the terms of the associated Analog Devices Software 
License Agreement (See /LICENSES/ADI.LABLAB.LICENSE). 

Modified from the Analog Devices original.
Portions Copyright (c) 2018 Andrew Grosser, Moritz von Buttlar.

*********************************************************************************/

/*!
 *****************************************************************************
 @file:    rtc.h
 @brief:   Real-Time Clock Device Definitions for ADuCxxx
 @version: $Revision: 30127 $
 @date:    $Date: 2015-03-09 12:05:31 -0400 (Mon, 09 Mar 2015) $
*****************************************************************************/

/*! \addtogroup RTC_Driver RTC Driver
 *  Real-Time Clock Device Driver
 *  @{
 */

#ifndef __RTC_H__
#define __RTC_H__

#include "device.h"
#include "adi_int.h"

/* C++ linkage */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/*!
 * \enum ADI_RTC_RESULT_TYPE
 * Gptimer API return codes
 */
typedef enum
{
    ADI_RTC_SUCCESS = 0,                /*!<  No Error, API suceeded */

    ADI_RTC_ERR_UNKNOWN_ERROR           /*!<  An unknown error was detected */
      = ADI_DEV_RTC_ERROR_OFFSET,
    ADI_RTC_ERR_CLOCK_FAILSAFE,         /*!<  RTC is in failsafe mode and not reliable */
    ADI_RTC_ERR_ALREADY_INITIALIZED,    /*!<  RTC is already initialized */
    ADI_RTC_ERR_BAD_DEV_HANDLE,         /*!<  Invalid device handle passed */
    ADI_RTC_ERR_BAD_INSTANCE,           /*!<  Asking to initialize an unknown instance */
    ADI_RTC_ERR_NOT_INITIALIZED,        /*!<  RTC not yet initialized */
    ADI_RTC_ERR_PARAM_OUT_OF_RANGE,     /*!<  Parameter is out of range.          */
} ADI_RTC_RESULT_TYPE;


/*!
 * \enum ADI_RTC_DEV_ID_TYPE
 *  @brief  Gptimer Device IDs.
 *  @details List of all GP Timer Device IDs for the ADuCxxx part
 */
typedef enum
{
    ADI_RTC_DEVID_0 = 0,       /*!<  RTC Timer Device 0 */
    ADI_RTC_MAX_DEVID          /*!<  max number of RTC devices */
} ADI_RTC_DEV_ID_TYPE;


/*!
 * \enum ADI_RTC_INT_ENA_TYPE
 * RTC Interrupt Enable Bits.
 */
typedef enum
{
    ADI_RTC_INT_ENA_WRITE_PEND         = (1 << 15),    /*!<  Write pend interrupt enable bit */
    ADI_RTC_INT_ENA_WRITE_SYNC         = (1 << 14),    /*!<  Write sync interrupt enable bit */
    ADI_RTC_INT_ENA_WRITE_PENDERR      = (1 << 13),    /*!<  Write pend error interrupt enable bit */
    ADI_RTC_INT_ENA_ISO_DONE           = (1 << 12),    /*!<  Power isolation done interrupt enable */
    ADI_RTC_INT_ENA_LCD_UPDATE         = (1 << 11),    /*!<  LCD update interrupt enable bit */
    ADI_RTC_INT_ENA_ALARM              = (1 << 2),     /*!<  Alarm interrupt enable bit */
} ADI_RTC_INT_ENA_TYPE;

#define ADI_RTC_INT_ENA_MASK           (ADI_RTC_INT_ENA_WRITE_PEND    \
                                      | ADI_RTC_INT_ENA_WRITE_SYNC    \
                                      | ADI_RTC_INT_ENA_WRITE_PENDERR \
                                      | ADI_RTC_INT_ENA_ISO_DONE      \
                                      | ADI_RTC_INT_ENA_LCD_UPDATE    \
                                      | ADI_RTC_INT_ENA_ALARM)          /*!< interrupt enable mask */


/*!
 * \enum ADI_RTC_INT_SOURCE_TYPE
 * RTC Interrupt source status Bits.
 */
typedef enum
{
    ADI_RTC_INT_SOURCE_WRITE_PEND       = (1 << 6),     /*!<  Write pending interrupt status bit */
    ADI_RTC_INT_SOURCE_WRITE_SYNC       = (1 << 5),     /*!<  Write synchronization interrupt status bit */
    ADI_RTC_INT_SOURCE_WRITE_PENDERR    = (1 << 4),     /*!<  Write pending error interrupt status bit */
    ADI_RTC_INT_SOURCE_ISO_DONE         = (1 << 3),     /*!<  Power isolation done interrupt status bit */
    ADI_RTC_INT_SOURCE_LCD_UPDATE       = (1 << 2),     /*!<  LCD update interrupt status bit */
    ADI_RTC_INT_SOURCE_ALARM            = (1 << 1),     /*!<  Alarm interrupt status bit */
    ADI_RTC_INT_SOURCE_FAIL             = (1 << 0),     /*!<  RTC Fail status bit */
} ADI_RTC_INT_SOURCE_TYPE;

#define ADI_RTC_INT_SOURCE_MASK         (ADI_RTC_INT_SOURCE_WRITE_PEND    \
                                       | ADI_RTC_INT_SOURCE_WRITE_SYNC    \
                                       | ADI_RTC_INT_SOURCE_WRITE_PENDERR \
                                       | ADI_RTC_INT_SOURCE_ISO_DONE      \
                                       | ADI_RTC_INT_SOURCE_LCD_UPDATE    \
                                       | ADI_RTC_INT_SOURCE_ALARM         \
                                       | ADI_RTC_INT_SOURCE_FAIL)          /*!< interrupt status mask */

/*!
 * \enum ADI_RTC_WRITE_STATUS_TYPE
 * RTC Posted Write Status Bits.
 */
typedef enum
{
    ADI_RTC_WRITE_STATUS_TRIM          = (1 << 13),    /*!<  Posted write trim register status bit */
    ADI_RTC_WRITE_STATUS_ALARM1        = (1 << 12),    /*!<  Posted write alarm1 register status bit */
    ADI_RTC_WRITE_STATUS_ALARM0        = (1 << 11),    /*!<  Posted write alarm0 register status bit */
    ADI_RTC_WRITE_STATUS_COUNT1        = (1 << 10),    /*!<  Posted write count1 register status bit */
    ADI_RTC_WRITE_STATUS_COUNT0        = (1 << 9),     /*!<  Posted write count0 register status bit */
    ADI_RTC_WRITE_STATUS_STATUS0       = (1 << 8),     /*!<  Posted write status0 register status bit */
    ADI_RTC_WRITE_STATUS_CONTROL       = (1 << 7),     /*!<  Posted write control register status bit */
} ADI_RTC_WRITE_STATUS_TYPE;

#define ADI_RTC_WRITE_STATUS_MASK      (ADI_RTC_WRITE_STATUS_TRIM    \
                                      | ADI_RTC_WRITE_STATUS_ALARM1  \
                                      | ADI_RTC_WRITE_STATUS_ALARM0  \
                                      | ADI_RTC_WRITE_STATUS_COUNT1  \
                                      | ADI_RTC_WRITE_STATUS_COUNT0  \
                                      | ADI_RTC_WRITE_STATUS_STATUS0 \
                                      | ADI_RTC_WRITE_STATUS_CONTROL)     /*!<  Posted write status mask */


/*!
 * \enum ADI_RTC_WRITE_ERROR_TYPE
 * RTC Posted Write Error Sources.
 */
typedef enum
{
    ADI_RTC_WRITE_ERROR_CONTROL        = (0 << 3),    /*!<  Posted write error source is control register */
    ADI_RTC_WRITE_ERROR_STATUS0        = (1 << 3),    /*!<  Posted write error source is status0 register */
    ADI_RTC_WRITE_ERROR_COUNT0         = (3 << 3),    /*!<  Posted write error source is count0 register */
    ADI_RTC_WRITE_ERROR_COUNT1         = (4 << 3),    /*!<  Posted write error source is count1 register */
    ADI_RTC_WRITE_ERROR_ALARM0         = (5 << 3),    /*!<  Posted write error source is alarm0 register */
    ADI_RTC_WRITE_ERROR_ALARM1         = (6 << 3),    /*!<  Posted write error source is alarm1 register */
    ADI_RTC_WRITE_ERROR_TRIM           = (7 << 3),    /*!<  Posted write error source is trim register */
    ADI_RTC_WRITE_ERROR_NONE           = (15 << 3),   /*!<  no error since last ADI_RTC_INT_SOURCE_WRITE_PENDERR clear */
} ADI_RTC_WRITE_ERROR_TYPE;

#define ADI_RTC_WRITE_ERROR_MASK       (ADI_RTC_WRITE_ERROR_NONE) /*!<  Posted write error mask */


/*!
 * \enum ADI_RTC_TRIM_INTERVAL_TYPE
 * RTC Trim intervals.
 */
typedef enum
{
    ADI_RTC_TRIM_INTERVAL_14           = (0 << 4),   /*!<  Trim interval is 2^14 seconds */
    ADI_RTC_TRIM_INTERVAL_15           = (1 << 4),   /*!<  Trim interval is 2^15 seconds */
    ADI_RTC_TRIM_INTERVAL_16           = (2 << 4),   /*!<  Trim interval is 2^16 seconds */
    ADI_RTC_TRIM_INTERVAL_17           = (3 << 4),   /*!<  Trim interval is 2^17 seconds */
} ADI_RTC_TRIM_INTERVAL_TYPE;

#define ADI_RTC_TRIM_INTERVAL_MASK     (0x0003 << 4) /*!<  Trim interval mask */


/*!
 * \enum ADI_RTC_TRIM_POLARITY_TYPE
 * RTC Trim polarity.
 */
typedef enum
{
    ADI_RTC_TRIM_ADD                   = (1 << 3),   /*!<  Trim value is added every trim interval */
    ADI_RTC_TRIM_SUB                   = (0 << 3),   /*!<  Trim value is subtracted every trim interval */
} ADI_RTC_TRIM_POLARITY_TYPE;

#define ADI_RTC_TRIM_POLARITY_MASK     (0x0001 << 3) /*!<  Trim polarity mask */


/*!
 * \enum ADI_RTC_TRIM_VALUE_TYPE
 * RTC Trim values.
 */
typedef enum
{
    ADI_RTC_TRIM_0                     = (0 << 0),   /*!<  Trim value is +/- 0 */
    ADI_RTC_TRIM_1                     = (1 << 0),   /*!<  Trim value is +/- 1 */
    ADI_RTC_TRIM_2                     = (2 << 0),   /*!<  Trim value is +/- 2 */
    ADI_RTC_TRIM_3                     = (3 << 0),   /*!<  Trim value is +/- 3 */
    ADI_RTC_TRIM_4                     = (4 << 0),   /*!<  Trim value is +/- 4 */
    ADI_RTC_TRIM_5                     = (5 << 0),   /*!<  Trim value is +/- 5 */
    ADI_RTC_TRIM_6                     = (6 << 0),   /*!<  Trim value is +/- 6 */
    ADI_RTC_TRIM_7                     = (7 << 0),   /*!<  Trim value is +/- 7 */
} ADI_RTC_TRIM_VALUE_TYPE;

#define ADI_RTC_TRIM_VALUE_MASK        (0x0007)      /*!<  Trim value mask */

/* overall trim mask */
#define ADI_RTC_TRIM_MASK              ( ADI_RTC_TRIM_INTERVAL_MASK  \
                                       | ADI_RTC_TRIM_POLARITY_MASK  \
                                       | ADI_RTC_TRIM_VALUE_MASK     )

typedef struct ADI_RTC_DEV_DATA_TYPE * ADI_RTC_HANDLE;    /*!< RTC Device instance private data handle typedef. */

#define ADI_RTC_GATEWAY_FLUSH    0xa2c5  /*!< Emergency flush command to gatweay register */


/*================ E X T E R N A L S ==================*/


/*
 * RTC API
 */

extern void                adi_RTC_ClearFailSafe          (void);

extern ADI_RTC_RESULT_TYPE adi_RTC_Init                   (ADI_RTC_DEV_ID_TYPE, ADI_RTC_HANDLE* const phDevice);
extern ADI_RTC_RESULT_TYPE adi_RTC_UnInit                 (ADI_RTC_HANDLE const);

extern ADI_RTC_RESULT_TYPE adi_RTC_EnableAlarm            (ADI_RTC_HANDLE, bool_t bFlag);
extern ADI_RTC_RESULT_TYPE adi_RTC_EnableDevice           (ADI_RTC_HANDLE, bool_t bFlag);
extern ADI_RTC_RESULT_TYPE adi_RTC_EnableInterrupts       (ADI_RTC_HANDLE, ADI_RTC_INT_ENA_TYPE Interrupts, bool_t bFlag);
extern ADI_RTC_RESULT_TYPE adi_RTC_EnableLcdUpdate        (ADI_RTC_HANDLE, bool_t bFlag);
extern ADI_RTC_RESULT_TYPE adi_RTC_EnableSafeWrites       (ADI_RTC_HANDLE, bool_t bFlag);
extern ADI_RTC_RESULT_TYPE adi_RTC_EnableTrim             (ADI_RTC_HANDLE, bool_t bFlag);

extern ADI_RTC_RESULT_TYPE adi_RTC_GetAlarm               (ADI_RTC_HANDLE, uint32_t *pAlarm);
extern ADI_RTC_RESULT_TYPE adi_RTC_GetControl             (ADI_RTC_HANDLE, uint16_t *pControl);
extern ADI_RTC_RESULT_TYPE adi_RTC_GetCount               (ADI_RTC_HANDLE, uint32_t *pCount);
extern ADI_RTC_RESULT_TYPE adi_RTC_GetInterruptStatus     (ADI_RTC_HANDLE, ADI_RTC_INT_SOURCE_TYPE *pStatus);
extern ADI_RTC_RESULT_TYPE adi_RTC_GetLcdUpdate           (ADI_RTC_HANDLE, uint16_t *pUpdate);
extern ADI_RTC_RESULT_TYPE adi_RTC_GetTrim                (ADI_RTC_HANDLE, uint16_t *pTrim);
extern ADI_RTC_RESULT_TYPE adi_RTC_GetWriteErrorSource    (ADI_RTC_HANDLE, ADI_RTC_WRITE_ERROR_TYPE *pError);
extern ADI_RTC_RESULT_TYPE adi_RTC_GetWritePendStatus     (ADI_RTC_HANDLE, ADI_RTC_WRITE_STATUS_TYPE *pPendBits);
extern ADI_RTC_RESULT_TYPE adi_RTC_GetWriteSyncStatus     (ADI_RTC_HANDLE, ADI_RTC_WRITE_STATUS_TYPE *pSyncBits);

extern ADI_RTC_RESULT_TYPE adi_RTC_SetAlarm               (ADI_RTC_HANDLE, uint32_t Alarm);
extern ADI_RTC_RESULT_TYPE adi_RTC_SetControl             (ADI_RTC_HANDLE, uint16_t Control);
extern ADI_RTC_RESULT_TYPE adi_RTC_SetCount               (ADI_RTC_HANDLE, uint32_t Count);
extern ADI_RTC_RESULT_TYPE adi_RTC_SetGateway             (ADI_RTC_HANDLE, uint16_t Command);
extern ADI_RTC_RESULT_TYPE adi_RTC_SetLcdUpdate           (ADI_RTC_HANDLE, uint16_t Update);
extern ADI_RTC_RESULT_TYPE adi_RTC_SetTrim                (ADI_RTC_HANDLE, uint16_t Trim);

extern ADI_RTC_RESULT_TYPE adi_RTC_SynchronizeAllWrites   (ADI_RTC_HANDLE);

extern ADI_RTC_RESULT_TYPE adi_RTC_ClearInterruptStatus   (ADI_RTC_HANDLE, ADI_RTC_INT_SOURCE_TYPE Status);
extern ADI_RTC_RESULT_TYPE adi_RTC_RegisterCallback       (ADI_RTC_HANDLE const hDevice, ADI_CALLBACK const cbFunc, uint32_t const cbWatch);

/* C++ linkage */
#ifdef __cplusplus
}
#endif

#endif /* include guard */

/*
** EOF
*/

/*@}*/
