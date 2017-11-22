/*********************************************************************************

Copyright (c) 2013-2014 Analog Devices, Inc. All Rights Reserved.

This software is proprietary to Analog Devices, Inc. and its licensors.  By using 
this software you agree to the terms of the associated Analog Devices Software 
License Agreement.

*********************************************************************************/

/*!
 ******************************************************************************
 * @file:    crc.h
 * @brief:   CRC Device Definitions for ADuCRFxxx
 * @version: $Revision: 28525 $
 * @date:    $Date: 2014-11-12 14:51:26 -0500 (Wed, 12 Nov 2014) $
 *****************************************************************************/

/*! \addtogroup CRC_Driver CRC Driver
 *  @{
 */

#ifndef __CRC_H__
#define __CRC_H__

#include "device.h"

/* C++ linkage */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*!
 *****************************************************************************
 * \enum ADI_CRC_DEV_ID_TYPE
 *
 * CRC  device IDs.
 *****************************************************************************/
typedef enum {
    ADI_CRC_DEVID_0 = 0,              /*!< CRC-0 device ID.                    */
    ADI_CRC_MAX_DEVID                 /*!<  Maximum number of devices*/
} ADI_CRC_DEV_ID_TYPE;

/*!
 *****************************************************************************
 * \enum ADI_CRC_AUTORST
 *
 * Enumeration for initializing the CRC result register.
 *****************************************************************************/
typedef enum {
    ADI_CRC_CKSUM_FFFFFFFF,                /*!< CRC_RESULT register will be set to 0XFFFFFFFF when device is enabled.  */
    ADI_CRC_CKSUM_00000000,                /*!< CRC_RESULT register will be set to 0X00000000 when device is enabled.  */
    ADI_CRC_CKSUM_NONE                     /*!< CRC_RESULT register will be unmodified. Retains existing value*/
} ADI_CRC_AUTORST;

/*!
 *****************************************************************************
 * \enum ADI_CRC_EVENT_TYPE
 *
 * Enumeration of events reported via the registered callback.
 *****************************************************************************/
typedef enum
{
    ADI_CRC_EVENT_RESULT_READY    /*!< CRC is computed for given buffer.    */
} ADI_CRC_EVENT_TYPE;


/*!
 *****************************************************************************
 * \enum ADI_CRC_RESULT_TYPE
 *
 * CRC Device Error Codes.  #ADI_CRC_SUCCESS is always zero and all other CRC
 * error codes are offset by the base CRC error code offset,
 * ADI_DEV_CRC_ERROR_OFFSET.  A number of CRC error codes are debug-only,
 * as controlled by the "ADI_DEBUG" build macro.
 *
 * The return value of all CRC APIs returning #ADI_CRC_RESULT_TYPE
 * should always be tested at the application level for success or failure.
 *
 *****************************************************************************/
typedef enum
{
    ADI_CRC_SUCCESS                     = 0,                            /*!< Generic success.                    */
    ADI_CRC_ERR_UNKNOWN                 = ADI_DEV_CRC_ERROR_OFFSET,     /*!< Generic error.                      */
    ADI_CRC_ERR_ALREADY_INITIALIZED     = ADI_DEV_CRC_ERROR_OFFSET+1,   /*!< CRC device is already initialized.  */
    ADI_CRC_ERR_BAD_DEV_HANDLE          = ADI_DEV_CRC_ERROR_OFFSET+2,   /*!< Invalid device handle.              */
    ADI_CRC_ERR_BAD_DEV_ID              = ADI_DEV_CRC_ERROR_OFFSET+3,   /*!< Invalid device ID.                  */
    ADI_CRC_ERR_BAD_DMA_CONFIG          = ADI_DEV_CRC_ERROR_OFFSET+4,   /*!< DMA configuration failure.          */
    ADI_CRC_ERR_NOT_INITIALIZED         = ADI_DEV_CRC_ERROR_OFFSET+4,   /*!< CRC device is uninitialized.        */
    ADI_CRC_ERR_BAD_DATA_POINTER        = ADI_DEV_CRC_ERROR_OFFSET+6,   /*!< NULL data pointer not allowed.      */
    ADI_CRC_ERR_OPERATION_NOT_ALLOWED   = ADI_DEV_CRC_ERROR_OFFSET+7,   /*!< Invalid operation .          */
    ADI_CRC_ERR_DEVICE_BUSY             = ADI_DEV_CRC_ERROR_OFFSET+8    /*!< Device is busy and can not process the buffer */
} ADI_CRC_RESULT_TYPE;

typedef struct ADI_CRC_DEV_DATA_TYPE* ADI_CRC_DEV_HANDLE;  /*!< CRC Device instance private data handle typedef. */


/******************************************************************************
 * CRC Device External API function prototypes
 *****************************************************************************/

/* Device Initialization and Uninitialization Interfaces */
extern ADI_CRC_RESULT_TYPE  adi_CRC_Init                    (ADI_CRC_DEV_ID_TYPE const devID,
                                                             ADI_CRC_DEV_HANDLE* const pHandle);

extern ADI_CRC_RESULT_TYPE  adi_CRC_UnInit                  (ADI_CRC_DEV_HANDLE const hDevice);


/******************************************************************
 * Eliminatable functions that may be optimized out by the linker *
 *****************************************************************/

/* Hardware Configuration Interface */
extern ADI_CRC_RESULT_TYPE  adi_CRC_SetAutoReset        (ADI_CRC_DEV_HANDLE const hDevice, ADI_CRC_AUTORST eAutoRst);
extern ADI_CRC_RESULT_TYPE  adi_CRC_SetBitMirroring     (ADI_CRC_DEV_HANDLE const hDevice, const bool_t bFlag);
extern ADI_CRC_RESULT_TYPE  adi_CRC_SetByteMirroring    (ADI_CRC_DEV_HANDLE const hDevice, const bool_t bFlag);
extern ADI_CRC_RESULT_TYPE  adi_CRC_ResetCRC            (ADI_CRC_DEV_HANDLE const hDevice, const bool_t bFlag);
extern ADI_CRC_RESULT_TYPE  adi_CRC_SetWordSwap         (ADI_CRC_DEV_HANDLE const hDevice, const bool_t bFlag);
extern ADI_CRC_RESULT_TYPE  adi_CRC_SetDmaMode          (ADI_CRC_DEV_HANDLE const hDevice, const bool_t bFlag);
extern ADI_CRC_RESULT_TYPE  adi_CRC_Enable              (ADI_CRC_DEV_HANDLE const hDevice, const bool_t bFlag);
/* Hardware Query Interface */
extern ADI_CRC_RESULT_TYPE  adi_CRC_GetAutoStart        (ADI_CRC_DEV_HANDLE const hDevice,bool_t *bFlag);
extern ADI_CRC_RESULT_TYPE  adi_CRC_GetBitMirring       (ADI_CRC_DEV_HANDLE const hDevice,bool_t *bFlag);
extern ADI_CRC_RESULT_TYPE  adi_CRC_GetByteMirring      (ADI_CRC_DEV_HANDLE const hDevice,bool_t *bFlag);

/* CRC result APIs */
extern ADI_CRC_RESULT_TYPE  adi_CRC_GetResult           (ADI_CRC_DEV_HANDLE const hDevice, uint32_t *pResult);
extern ADI_CRC_RESULT_TYPE  adi_CRC_SetResult           (ADI_CRC_DEV_HANDLE const hDevice, const uint32_t nValue);
extern ADI_CRC_RESULT_TYPE  adi_CRC_IsResultAvailable   (ADI_CRC_DEV_HANDLE const hDevice, bool_t *bFlag);
/* Command/Data transceiver API */
extern ADI_CRC_RESULT_TYPE  adi_CRC_BufferSubmit       (ADI_CRC_DEV_HANDLE const hDevice, uint32_t *pBuff, uint32_t nSize);
extern ADI_CRC_RESULT_TYPE  adi_CRC_RegisterCallback   (ADI_CRC_DEV_HANDLE const hDevice, ADI_CALLBACK pfCallback, void *pCBParam);

/* C++ linkage */
#ifdef __cplusplus
}
#endif

#endif /* include guard */

/*
** EOF
*/

/*@}*/
