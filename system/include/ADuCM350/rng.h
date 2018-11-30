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
 @file:    rng.h
 @brief:   Random Number Generator Device Implementations for ADuCXXxxx
 @version: $Revision: 28669 $
 @date:    $Date: 2014-11-18 13:17:38 -0500 (Tue, 18 Nov 2014) $
*****************************************************************************/

/*! \addtogroup RNG_Driver RNG Driver
 *  Random Number Generator Device Driver
 *  @{
 */

#ifndef __RNG_H__
#define __RNG_H__

#include "device.h"
#include "config/adi_rng_config.h"

/* C++ linkage */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*!
 * \enum ADI_RNG_RESULT_TYPE
 * Random Number Generator API return codes
 */
typedef enum
{
    ADI_RNG_SUCCESS = 0,                /*!<  No Error, API suceeded */

    ADI_RNG_ERR_UNKNOWN_ERROR           /*!<  An unknown error was detected */
      = ADI_DEV_RNG_ERROR_OFFSET,
    ADI_RNG_ERR_ALREADY_INITIALIZED,    /*!<  RNG is already initialized */
    ADI_RNG_ERR_BAD_DEV_HANDLE,         /*!<  Invalid device handle passed */
    ADI_RNG_ERR_BAD_INSTANCE,           /*!<  Asking to initialize an unknown instance */
    ADI_RNG_ERR_NOT_INITIALIZED,        /*!<  RNG not yet initialized */
    ADI_RNG_ERR_PARAM_OUT_OF_RANGE,     /*!<  Parameter is out of range.          */
    ADI_RNG_ERR_NOT_VALID               /*!<  Data is not valid. */
} ADI_RNG_RESULT_TYPE;


/*!
 * \enum ADI_RNG_DEV_ID_TYPE
 *  @brief  RNG Device IDs.
 *  @details List of all Random Number Generator IDs for the ADuCRFxxx part
 */
typedef enum
{
    ADI_RNG_DEVID_0 = 0,       /*!<  RNG Device 0 */
    ADI_RNG_MAX_DEVID          /*!<  max number of RNG devices */
} ADI_RNG_DEV_ID_TYPE;


/*!
 * \enum ADI_RNG_INTERRUPT_TYPE
 *  @brief  RNG Interrupt Bits.
 *  @details List of all RNG interrupt (enables and status) bits.
 */
typedef enum
{
    ADI_RNG_INTERRUPT_RNG_RDY   = (1 << BITP_RNG_RNGSTAT_RNGRDY)  /*!< Random number ready interrupt bit */
} ADI_RNG_INTERRUPT_TYPE;


/*!
 *****************************************************************************
 * An interrupt mask covering all RNG interrupts.
 *****************************************************************************/
#define ADI_RNG_ALL_INTERRUPTS      (ADI_RNG_INTERRUPT_RNG_RDY)


/*!< RNG Device instance private data handle typedef. */
typedef struct ADI_RNG_DEV_DATA_TYPE * ADI_RNG_HANDLE;


/*================ E X T E R N A L S ==================*/


/*
 * RNG API
 */

extern ADI_RNG_RESULT_TYPE adi_RNG_Init                   (ADI_RNG_DEV_ID_TYPE const InstanceNum, ADI_RNG_HANDLE* const phDevice);
extern ADI_RNG_RESULT_TYPE adi_RNG_UnInit                 (ADI_RNG_HANDLE const hDevice);

extern ADI_RNG_RESULT_TYPE adi_RNG_Enable                 (ADI_RNG_HANDLE const hDevice, bool_t bFlag);
extern ADI_RNG_RESULT_TYPE adi_RNG_EnableCounter          (ADI_RNG_HANDLE const hDevice, bool_t bFlag);
extern ADI_RNG_RESULT_TYPE adi_RNG_SetMode                (ADI_RNG_HANDLE const hDevice, bool_t bFlag);
extern ADI_RNG_RESULT_TYPE adi_RNG_SetLenReload           (ADI_RNG_HANDLE const hDevice, uint16_t LenReload);
extern ADI_RNG_RESULT_TYPE adi_RNG_SetLenPrescaler        (ADI_RNG_HANDLE const hDevice, uint16_t LenPrescaler);
extern ADI_RNG_RESULT_TYPE adi_RNG_SetControl             (ADI_RNG_HANDLE const hDevice, uint16_t Control);
extern ADI_RNG_RESULT_TYPE adi_RNG_SetSampleLen           (ADI_RNG_HANDLE const hDevice, uint16_t SampleLen);

extern ADI_RNG_RESULT_TYPE adi_RNG_GetRdyStatus           (ADI_RNG_HANDLE const hDevice, bool_t* const pbFlag);
extern ADI_RNG_RESULT_TYPE adi_RNG_GetRngData             (ADI_RNG_HANDLE const hDevice, uint16_t* const pRngData);
extern ADI_RNG_RESULT_TYPE adi_RNG_GetControl             (ADI_RNG_HANDLE const hDevice, uint16_t* const pControl);
extern ADI_RNG_RESULT_TYPE adi_RNG_GetOscCount            (ADI_RNG_HANDLE const hDevice, uint32_t* const pOscCount);
extern ADI_RNG_RESULT_TYPE adi_RNG_GetSampleLen           (ADI_RNG_HANDLE const hDevice, uint16_t* const pSampleLen);

extern ADI_RNG_RESULT_TYPE adi_RNG_RegisterCallback       (ADI_RNG_HANDLE const hDevice, ADI_CALLBACK const cbFunc, uint32_t const cbWatch);

/* C++ linkage */
#ifdef __cplusplus
}
#endif

#endif /* include guard */

/*
** EOF
*/

/*@}*/
