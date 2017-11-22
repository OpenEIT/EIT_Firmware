/*********************************************************************************

Copyright (c) 2010-2014 Analog Devices, Inc. All Rights Reserved.

This software is proprietary to Analog Devices, Inc. and its licensors.  By using 
this software you agree to the terms of the associated Analog Devices Software 
License Agreement.

*********************************************************************************/

/*!****************************************************************************
   @file:    flash.h
   @brief:   Flash definitions and API for ADuCxxx
   @version: $Revision: 28525 $
   @date:    $Date: 2014-11-12 14:51:26 -0500 (Wed, 12 Nov 2014) $
*****************************************************************************/

/*! \addtogroup Flash_Driver Flash Driver
 *  @{
 */

#ifndef __FEE_H__
#define __FEE_H__

#include "device.h"
#include "adi_fee_config.h"

/* C++ linkage */
#ifdef __cplusplus
extern "C" {
#endif

 /*!
 *****************************************************************************
 * \enum ADI_FEE_DEV_ID_TYPE
 *
 * FEE device IDs.  Depending on device configuration there may
 * be one or more physical controllers.  Use the #ADI_FEE_DEV_ID_TYPE
 * enumerator to select which device is being initialized by function
 * adi_FEE_Init().
 *
 *****************************************************************************/
typedef enum {
    ADI_FEE_DEVID_0 = 0,              /*!< Flash Block0 device ID. */
#ifdef ADI_ADUCM350
    ADI_FEE_DEVID_1,                  /*!< Flash Block1 device ID. */
    ADI_FEE_DEVID_GP,                 /*!< GP Flash device ID.     */
#endif
    ADI_FEE_MAX_DEVID
} ADI_FEE_DEV_ID_TYPE;



/*! \enum ADI_FEE_RESULT_TYPE Flash API function return codes */
typedef enum
{
    ADI_FEE_SUCCESS = 0,                                   /*!< no error                                 */
    ADI_FEE_ABORTED,
    ADI_FEE_ERR_UNKNOWN_ERROR /*= ADI_DEV_FEE_ERROR_OFFSET*/,  /*!< generic unknown error occurred           */
    ADI_FEE_ERR_ALREADY_INITIALIZED,                       /*!< Flash device is already initialized.     */
    ADI_FEE_ERR_BAD_DEV_ID,                                /*!< Invalid device ID.                       */
    ADI_FEE_ERR_CRC_ENCODE_FAILED,                         /*!< Flash device failed CRC block encode     */
    ADI_FEE_ERR_CRC_VERIFY_FAILED,                         /*!< Flash device failed CRC block verify     */
    ADI_FEE_ERR_INVALID_ADDRESS,                           /*!< Flash device passed an invalid address   */
    ADI_FEE_ERR_INVALID_PARAMETER,                         /*!< Flash device passed an invalid parameter */
    ADI_FEE_ERR_NOT_INITIALIZED,                           /*!< Flash device is uninitialized.           */
    ADI_FEE_ERR_WRITE_GENERAL,                             /*!< Flash device has a general write error   */
    ADI_FEE_ERR_WRITE_PROTECTION,                          /*!< Flash device access is protected         */
    ADI_FEE_ERR_WRITE_PROT_LIMIT,                          /*!< Write protection has already been set    */
    ADI_FEE_ERR_UNSUPPORTED,                               /*!< Operation is not supported on this controller */
    ADI_FEE_ERR_READ_VERIFY_ERROR,
    ADI_FEE_ERR_PARITY_ERROR,
    ADI_FEE_ERR_DMA_FAILURE,
    ADI_FEE_ERR_SEMAPHORE_FAILED
} ADI_FEE_RESULT_TYPE;


/*!
 *****************************************************************************
 * \enum ADI_FEE_SIGN_DIR
 *
 *
 *****************************************************************************/
typedef enum
{
    ADI_FEE_SIGN_FORWARD = 0,        /*!< Perform FSIGN operation */
    ADI_FEE_SIGN_REVERSE,            /*!< Perform RSIGN operation */
} ADI_FEE_SIGN_DIR;

/*!
 *****************************************************************************
 * \enum ADI_FEE_PARITY_CTRL
 *
 *
 *****************************************************************************/
typedef enum
{
    ADI_FEE_PARITY_DISABLE = 0,      /*!< No parity checking */
    ADI_FEE_PARITY_ENABLE,           /*!< Parity checking on, no parity interrupt */
    ADI_FEE_PARITY_ENABLE_INTERRUPT  /*!< Parity checking on, parity interrupt on*/
} ADI_FEE_PARITY_CTRL;


/* opaque handle typedef */
typedef struct ADI_FEE_DEV_DATA_TYPE* ADI_FEE_DEV_HANDLE;  /*!< FEE Device instance private data handle typedef. */

/* Flash API functions */

extern ADI_FEE_RESULT_TYPE adi_FEE_ReadProtectEnable  (bool_t bEnable);                      /*!< assert read protection */
extern ADI_FEE_RESULT_TYPE adi_FEE_Init               (ADI_FEE_DEV_ID_TYPE const devID, bool_t bRetryAborts, ADI_FEE_DEV_HANDLE* const pHandle);             /*!< initialize flash driver */
extern ADI_FEE_RESULT_TYPE adi_FEE_UnInit             (ADI_FEE_DEV_HANDLE const hDevice);                                               /*!< uninitialize flash driver */

extern ADI_FEE_RESULT_TYPE adi_FEE_MassErase          (ADI_FEE_DEV_HANDLE const hDevice);                                               /*!< erase entire flash */
extern ADI_FEE_RESULT_TYPE adi_FEE_PageErase          (ADI_FEE_DEV_HANDLE const hDevice, const uint32_t PageNum);                       /*!< erase single flash page */

extern ADI_FEE_RESULT_TYPE adi_FEE_Write              (ADI_FEE_DEV_HANDLE const hDevice,                                                /*!< write data to flash */
                                                       const uint32_t nAddress,
                                                       const uint8_t *pData,
                                                       const uint32_t nBytes);

extern ADI_FEE_RESULT_TYPE adi_FEE_SubmitTxBuffer     (ADI_FEE_DEV_HANDLE const hDevice,
                                                       const uint32_t nAddress,
                                                       const uint8_t *pData,
                                                       const uint32_t nBytes);
extern ADI_FEE_RESULT_TYPE adi_FEE_IsTxBufferAvailable(ADI_FEE_DEV_HANDLE         const hDevice,
                                                       bool_t                   *pbAvailable);
extern ADI_FEE_RESULT_TYPE adi_FEE_GetTxBuffer        (ADI_FEE_DEV_HANDLE   const   hDevice,
                                                       void                   **ppBuffer);

extern ADI_FEE_RESULT_TYPE adi_FEE_GetBlockNumber     (ADI_FEE_DEV_HANDLE const hDevice, const uint32_t Address, uint32_t *pBlockNum);  /*!< get flash block number */
extern ADI_FEE_RESULT_TYPE adi_FEE_GetPageNumber      (ADI_FEE_DEV_HANDLE const hDevice, const uint32_t Address, uint32_t *pPageNum);   /*!< get flash page number */
extern ADI_FEE_RESULT_TYPE adi_FEE_VerifySignature    (ADI_FEE_DEV_HANDLE const hDevice,                                                /*!< generate and check signature */
                                                       ADI_FEE_SIGN_DIR direction,
                                                       const uint32_t nLowerPage,
                                                       const uint32_t nUpperPage,
                                                       uint32_t *pSigResult);

extern ADI_FEE_RESULT_TYPE adi_FEE_WriteProtectBlock  (ADI_FEE_DEV_HANDLE const hDevice, const uint32_t BlockNum);                      /*!< assert write protection */

extern ADI_FEE_RESULT_TYPE adi_FEE_SetParityChecking  (ADI_FEE_DEV_HANDLE const hDevice, ADI_FEE_PARITY_CTRL const eParityCtrl);        /*!< enable or disable parity checking and optional interrupt */
extern ADI_FEE_RESULT_TYPE adi_FEE_GetParityError     (ADI_FEE_DEV_HANDLE const hDevice, uint32_t *pAddress);                           /*!< get failed parity address */

extern ADI_FEE_RESULT_TYPE adi_FEE_SetInterruptAbort  (ADI_FEE_DEV_HANDLE const hDevice, IRQn_Type, bool_t bEnable);                    /*!< enable or disable flash abort on system interrupt */
extern ADI_FEE_RESULT_TYPE adi_FEE_Abort              (ADI_FEE_DEV_HANDLE const hDevice);                                               /*!< force flash operation abort from software */

/* C++ linkage */
#ifdef __cplusplus
}
#endif

#endif /* include guard */

/*
** EOF
*/

/*@}*/
