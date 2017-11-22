/*********************************************************************************

Copyright (c) 2012-2014 Analog Devices, Inc. All Rights Reserved.

This software is proprietary to Analog Devices, Inc. and its licensors.  By using 
this software you agree to the terms of the associated Analog Devices Software 
License Agreement.

*********************************************************************************/

/*!
 ******************************************************************************
 * @file:    captouch.h
 * @brief:   Captouch Definitions for ADuCM350
 * @version: $Revision: 28525 $
 * @date:    $Date: 2014-11-12 14:51:26 -0500 (Wed, 12 Nov 2014) $
*****************************************************************************/

/*! \addtogroup CT_Library
 *  @{
 */

#ifndef __CT_FUNCTIONS_H__
#define __CT_FUNCTIONS_H__

#include "device.h"

/* C++ linkage */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#define DEFAULT_BASELINE_LIMITS   200    /*!<  Default Baseline Limit */

/* Function prototypes */



extern ADI_CT_RESULT_TYPE       adi_CT_Stage_AutoCAPDAC_Adjustment              (ADI_CT_DEV_HANDLE const hDevice,ADI_CT_STAGE_NUMBER_TYPE StageNumber,uint16_t Value);
extern ADI_CT_RESULT_TYPE       adi_CT_FrequencyHoppingAlgorithm                (ADI_CT_DEV_HANDLE const hDevice,ADI_CT_STAGE_NUMBER_TYPE StageNumber,uint16_t NoiseLevel,uint8_t *Phase24Result);
extern ADI_CT_RESULT_TYPE       adi_CT_NoiseMeasurement                         (ADI_CT_DEV_HANDLE const hDevice,ADI_CT_STAGE_NUMBER_TYPE StageNumber,uint32_t *variance,uint8_t MeasureNumber);
extern ADI_CT_RESULT_TYPE       adi_CT_NoiseMeasurementPK2PK                    (ADI_CT_DEV_HANDLE const hDevice,ADI_CT_STAGE_NUMBER_TYPE StageNumber,uint32_t *PK2PKValue,uint8_t MeasureNumber);

/* C++ linkage */
#ifdef __cplusplus
}
#endif

#endif /* include guard */

/*
** EOF
*/

/*@}*/



