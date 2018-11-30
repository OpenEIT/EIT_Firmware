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
 * @file:    metrics.h
 * @brief:   Performance Metrics Definitions for ADuCxxx
 * @version: $Revision: 30127 $
 * @date:    $Date: 2015-03-09 12:05:31 -0400 (Mon, 09 Mar 2015) $
 *****************************************************************************/


#ifndef __METRICS_H__
#define __METRICS_H__

#include "device.h"

/* C++ linkage */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


typedef enum
{
    ADI_METRIC_SUCCESS                 = 0,                            /*!< Generic success.                    */
    ADI_METRIC_ERROR                   = 1                             /*!< Generic error.                      */
} ADI_METRIC_RESULT_TYPE;


typedef struct ADI_METRIC_DEV_DATA_TYPE* ADI_METRIC_DEV_HANDLE;

/******************************************************************************
 * Metric External API function prototypes
 *****************************************************************************/

/* Device Initialization and Uninitialization Interfaces */
extern ADI_METRIC_RESULT_TYPE  adi_Metric_Init          (ADI_GPT_DEV_ID_TYPE InstanceNum, ADI_METRIC_DEV_HANDLE* const pHandle);
extern ADI_METRIC_RESULT_TYPE  adi_Metric_UnInit        (ADI_METRIC_DEV_HANDLE const hDevice);
extern ADI_METRIC_RESULT_TYPE  adi_Metric_Start         (ADI_METRIC_DEV_HANDLE const hDevice);
extern ADI_METRIC_RESULT_TYPE  adi_Metric_Stop          (ADI_METRIC_DEV_HANDLE const hDevice);
extern ADI_METRIC_RESULT_TYPE  adi_Metric_IncIndex      (ADI_METRIC_DEV_HANDLE const hDevice);
extern uint32_t                adi_Metric_GetCount      (ADI_METRIC_DEV_HANDLE const hDevice);
extern uint32_t                adi_Metric_GetAccumulate (ADI_METRIC_DEV_HANDLE const hDevice);

/* C++ linkage */
#ifdef __cplusplus
}
#endif

#endif /* include guard */

/*
** EOF
*/

/*@}*/
