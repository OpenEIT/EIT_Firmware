/*********************************************************************************

Copyright (c) 2010-2014 Analog Devices, Inc. All Rights Reserved.

This software is proprietary to Analog Devices, Inc. and its licensors.  By using 
this software you agree to the terms of the associated Analog Devices Software 
License Agreement.

*********************************************************************************/

/*!
 *****************************************************************************
 * @file:    metric.c
 * @brief:   Metric counter Device Implementations for ADuCxxx
 * @version: $Revision: 28525 $
 * @date:    $Date: 2014-11-12 14:51:26 -0500 (Wed, 12 Nov 2014) $
 *****************************************************************************/


#define ASSERT_ADI_MISRA_SUPPRESSIONS  /*!< Apply ADI MISRA Suppressions */
#include "misra.h"

#include <stdlib.h>  /* for 'NULL" definition */
#include "metrics.h"

//#define USE_ENABLE_API


typedef struct ADI_METRIC_DEV_DATA_TYPE
{
    /* device attributes */
    uint16_t                Count;          /* timer count value                                                */
    uint32_t                OverflowIndex;  /* number of times timer count overflows between start and stop     */
    uint32_t                Accumulate;     /* running accumulation of the timer count between starts and stops */
    uint32_t                TestIndex;      /* general test index, used to count maybe the no. of ISR entries   */
    ADI_GPT_HANDLE          hGpTimer;       /* handle to timer                                                  */
} ADI_METRIC_DEV_DATA_TYPE;

/* allocate instance data array */
ADI_METRIC_DEV_DATA_TYPE Metric_DevData[1];


/* internal device data pointers */
static ADI_METRIC_DEV_DATA_TYPE* const pMetric_DevData = &Metric_DevData[0];

void GPTimerCallback(void *pCBParam, uint32_t nEvent, void *pArg);

/*!
 * @brief  Initialize the metrics internals.
 *
 * @param[in]        InstanceNum  Timer instance number.
 * @param[out]       pHandle      The timer's handle pointer for storing the initialized device instance data pointer.
 *
 * @return            Status.
 *                    - #ADI_METRIC_ERROR                       Metrics internal timer has failed.
 *                    - #ADI_METRIC_SUCCESS                     Call completed successfully.
 *
 * Initialize a metrics capture instance.  This instance is used to capture a timer count value between two
 * lines of code marked by API calls, adi_Metric_Start and adi_Metric_Stop.
 *
 * @note    The contents of pHandle will be set to NULL upon failure.
 *
 * @sa      adi_Metric_UnInit().
 */
ADI_METRIC_RESULT_TYPE adi_Metric_Init(ADI_GPT_DEV_ID_TYPE InstanceNum, ADI_METRIC_DEV_HANDLE* const pHandle)
{
    ADI_METRIC_DEV_HANDLE hDevice = pMetric_DevData;
    ADI_METRIC_RESULT_TYPE result = ADI_METRIC_SUCCESS;

    *pHandle = hDevice;

    /* Timer 0 */
    hDevice->OverflowIndex = 0;
    hDevice->TestIndex = 0;
    hDevice->Count = 0;
    hDevice->Accumulate = 0;

    /* open timer instance */
    if(adi_GPT_Init(InstanceNum, &hDevice->hGpTimer)!= ADI_GPT_SUCCESS)
    {
    	return ADI_METRIC_ERROR;
    }
    if(adi_GPT_SetClockSelect(hDevice->hGpTimer, ADI_GPT_CLOCK_SELECT_HFOSC)!= ADI_GPT_SUCCESS)
    {
    	return ADI_METRIC_ERROR;
    }
    if(adi_GPT_SetCountMode(hDevice->hGpTimer, ADI_GPT_COUNT_UP)!= ADI_GPT_SUCCESS)
    {
    	return ADI_METRIC_ERROR;
    }
    if(adi_GPT_SetPeriodicMode(hDevice->hGpTimer, false, 0)!= ADI_GPT_SUCCESS)
    {
    	return ADI_METRIC_ERROR;
    }
    if(adi_GPT_SetLdVal(hDevice->hGpTimer, 0)!= ADI_GPT_SUCCESS)
    {
    	return ADI_METRIC_ERROR;
    }
    if(adi_GPT_SetPrescaler(hDevice->hGpTimer, ADI_GPT_PRESCALER_16)!= ADI_GPT_SUCCESS)
    {
    	return ADI_METRIC_ERROR;
    }
    if(adi_GPT_RegisterCallback(hDevice->hGpTimer, GPTimerCallback, hDevice->hGpTimer)!= ADI_GPT_SUCCESS)
    {
    	return ADI_METRIC_ERROR;
    }

    return result;
}

/*!
 * @brief  Uninitialize and deallocate an the metrics instance.
 *
 * @param[in]   hDevice    Device handle obtained from adi_Metric_Init()
 *
 * @return            Status.
 *                    - #ADI_METRIC_ERROR                       Metrics internal timer has failed.
 *                    - #ADI_METRIC_SUCCESS                     Call completed successfully.
 *
 * Uninitialize and release an allocated metrics instance.
 *
 * @sa        adi_Metric_Init().
 */
ADI_METRIC_RESULT_TYPE adi_Metric_UnInit(ADI_METRIC_DEV_HANDLE const hDevice)
{
    ADI_METRIC_RESULT_TYPE result = ADI_METRIC_SUCCESS;

    if(adi_GPT_UnInit(hDevice->hGpTimer) != ADI_GPT_SUCCESS)
    {
    	return ADI_METRIC_ERROR;
    }
    return result;
}

/*!
 * @brief  Starts capturing the metric timer count.
 *
 * @param[in]   hDevice    Device handle
 *
 * @return            Status.
 *                    - #ADI_METRIC_ERROR                       Metrics internal timer has failed.
 *                    - #ADI_METRIC_SUCCESS                     Call completed successfully.
 *
 * Uninitialize and release an allocated metrics instance.
 *
 * @sa        adi_Metric_Stop().
 */
ADI_METRIC_RESULT_TYPE adi_Metric_Start(ADI_METRIC_DEV_HANDLE const hDevice)
{
    ADI_METRIC_RESULT_TYPE result = ADI_METRIC_SUCCESS;
#if defined (USE_ENABLE_API)
    /* this API call can consume considerable cycle counts when RTOS is enabled */
    if(adi_GPT_SetTimerEnable(hDevice->hGpTimer, true) != ADI_GPT_SUCCESS)
    {
    	return ADI_METRIC_ERROR;
    }
#else
    /* bypass timer API call to avoid consuming cycle counts */
    pADI_GPT0->GPTCON |= TCON_ENABLE;
#endif
    return result;
}

/*!
 * @brief  Stops capturing the metric timer count and accumulates a running count
 *
 * @param[in]   hDevice    Device handle
 *
 * @return            Status.
 *                    - #ADI_METRIC_ERROR                       Metrics internal timer has failed.
 *                    - #ADI_METRIC_SUCCESS                     Call completed successfully.
 *
 * Uninitialize and release an allocated metrics instance.
 *
 * @sa        adi_Metric_Start().
 */
ADI_METRIC_RESULT_TYPE adi_Metric_Stop(ADI_METRIC_DEV_HANDLE const hDevice)
{
    ADI_METRIC_RESULT_TYPE result = ADI_METRIC_SUCCESS;
#if defined (USE_ENABLE_API)
    if(adi_GPT_SetTimerEnable(hDevice->hGpTimer, false) != ADI_GPT_SUCCESS)
    {
    	return ADI_METRIC_ERROR;
    }
#else
    pADI_GPT0->GPTCON &= ~TCON_ENABLE;
#endif
    hDevice->Accumulate += ((hDevice->OverflowIndex * 0xFFFF) + hDevice->Count);
    hDevice->OverflowIndex = 0;
    hDevice->Count = 0;

    return result;
}
/*!
 * @brief  An general purpose test index.  Can be used to count ISR entries for instance.
 *
 * @param[in]   hDevice    Device handle
 *
 * @return            Status.
 *                    - #ADI_METRIC_ERROR                       Metrics internal timer has failed.
 *                    - #ADI_METRIC_SUCCESS                     Call completed successfully.
 *
 * Uninitialize and release an allocated metrics instance.
 *
 */
ADI_METRIC_RESULT_TYPE adi_Metric_IncIndex(ADI_METRIC_DEV_HANDLE const hDevice)
{
    ADI_METRIC_RESULT_TYPE result = ADI_METRIC_SUCCESS;

    hDevice->TestIndex++;

    return result;
}
/*!
 * @brief  Returns the accumulated count value between successive starts and stops.
 *
 * @param[in]   hDevice    Device handle
 *
 * @return            Status.
 *                    - #ADI_METRIC_ERROR                       Metrics internal timer has failed.
 *                    - #ADI_METRIC_SUCCESS                     Call completed successfully.
 *
 * Uninitialize and release an allocated metrics instance.
 *
 */
uint32_t adi_Metric_GetCount(ADI_METRIC_DEV_HANDLE const hDevice)
{
    adi_GPT_GetTxVal(hDevice->hGpTimer, &hDevice->Count);
    return ((hDevice->OverflowIndex * 0xFFFF) + hDevice->Count);
}

uint32_t adi_Metric_GetAccumulate(ADI_METRIC_DEV_HANDLE const hDevice)
{
    return (hDevice->Accumulate);
}

void GPTimerCallback(void *pCBParam, uint32_t nEvent, void *pArg)
{
    /* Pointer to device instance */
    ADI_METRIC_DEV_DATA_TYPE *pDevice = (ADI_METRIC_DEV_DATA_TYPE *) pCBParam;
    /* increment the number of times the 16 bit count value overflows */
    pDevice->OverflowIndex++;
}


#define REVERT_ADI_MISRA_SUPPRESSIONS  /*!< Revert ADI MISRA Suppressions */
#include "misra.h"

/*
** EOF
*/

/*@}*/
