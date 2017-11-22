/*********************************************************************************

Copyright (c) 2010-2014 Analog Devices, Inc. All Rights Reserved.

This software is proprietary to Analog Devices, Inc. and its licensors.  By using 
this software you agree to the terms of the associated Analog Devices Software 
License Agreement.

*********************************************************************************/

/*!
 ******************************************************************************
 * @file:    i2c.h
 * @brief:   I2C Device Definitions for ADuCxxx
 * @version: $Revision: 28525 $
 * @date:    $Date: 2014-11-12 14:51:26 -0500 (Wed, 12 Nov 2014) $
 *****************************************************************************/

/*! \addtogroup I2C_Driver I2C Driver
 *  @{
 */

#ifndef __I2C_H__
#define __I2C_H__

/* Processor specific definitions,  memory map, etc. */
#include "device.h"

/* I2C config stuff */
#include "adi_i2c_config.h"

/* C++ linkage */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*!
 *****************************************************************************
 * \enum ADI_I2C_DEV_ID_TYPE
 *
 * I2C device IDs
 *****************************************************************************/
typedef enum {
    ADI_I2C_DEVID_0 = 0,              /*!< I2C device ID -0                   */
    ADI_I2C_MAX_DEVID
} ADI_I2C_DEV_ID_TYPE;


/*!
 *****************************************************************************
 * \enum ADI_I2C_RESULT_TYPE
 *
 * I2C Device Error Codes
 *****************************************************************************/
typedef enum
{
    ADI_I2C_SUCCESS = 0,             /*!< Generic success                        */

    ADI_I2C_ERR_UNKNOWN              /*!< Generic error                          */
        = ADI_DEV_I2C_ERROR_OFFSET,

    ADI_I2C_ERR_ALREADY_INITIALIZED, /*!< I2C device is already initialized.      */
    ADI_I2C_ERR_BAD_API_FOR_MODE,    /*!< Invalid API for current mode.           */
    ADI_I2C_ERR_BAD_CLOCK_RATE,      /*!< Unsupported clock rate specified.       */
    ADI_I2C_ERR_BAD_DATA_ADDRESS,    /*!< Data address width exceeds 16-bits.     */
    ADI_I2C_ERR_BAD_DEVICE_HANDLE,   /*!< Invalid device handle.                  */
    ADI_I2C_ERR_BAD_DEVICE_INDEX,    /*!< Invalid physical device index.          */
    ADI_I2C_ERR_BAD_DEVICE_WIDTH,    /*!< Invalid device ID addressing width.     */
    ADI_I2C_ERR_BAD_DMA_CONFIG,      /*!< DMA configuration failure.              */
    ADI_I2C_ERR_BAD_GPIO_CONFIG,     /*!< I2C gpio config failed.                 */
    ADI_I2C_ERR_BAD_MODE_COMBO,      /*!< DMA and Blocking-Mode incompatible.     */
    ADI_I2C_ERR_BLOCKING_ACTIVE,     /*!< Blocking mode is active.                */
    ADI_I2C_ERR_NOT_INITIALIZED,     /*!< I2C device is uninitialized.            */
    ADI_I2C_ERR_NULL_DATA_POINTER,   /*!< NULL data pointer not allowed.          */
    ADI_I2C_ERR_PARAM_OUT_OF_RANGE,  /*!< API parameter is out of range.          */
    ADI_I2C_TRANSACTION_INCOMPLETE,  /*!< Incomplete non-blocking transaction.    */

    ADI_I2C_MASTER_ALOST,           /*!< Master lost bus arbitration.             */
    ADI_I2C_MASTER_NACK_ADDRESS,    /*!< Master received No address ACKnowledge.  */
    ADI_I2C_MASTER_NACK_DATA,       /*!< Master received No data ACKnowledge.     */
    ADI_I2C_MASTER_STOP,            /*!< Master received a Stop condition.        */
    ADI_I2C_SLAVE_GENERAL_CALL,     /*!< Slave received a general-call command.   */
    ADI_I2C_SLAVE_REPEAT_START,     /*!< Slave received a Repeat Start condition. */
    ADI_I2C_SLAVE_RECEIVED_STOP,    /*!< Slave received a Stop condition.         */
    ADI_I2C_ERR_BUSY,               /*!< I2C controller is not in the idle state. */
	ADI_I2C_ERR_SEMAPHORE_FAILED,   /*!< Failed to create blocking semaphore      */
} ADI_I2C_RESULT_TYPE;


/*!
 *****************************************************************************
 * \enum ADI_I2C_INIT_STATE
 *
 *  I2C driver state.
 *****************************************************************************/
typedef enum
{
    ADI_I2C_STATE_UNINITIALIZED = 0,  /*!< I2C is not initialized. */
    ADI_I2C_STATE_MASTER_MODE,        /*!< I2C is in Master-Mode.  */
    ADI_I2C_STATE_SLAVE_MODE          /*!< I2C is in Slave-Mode.   */
} ADI_I2C_INIT_STATE;


/*!
 *****************************************************************************
 * \enum ADI_I2C_SLAVE_ID_WIDTH
 *
 * I2C Device ID width constants
 *****************************************************************************/
typedef enum
{
    ADI_I2C_SEVEN_BIT_SLAVE_ID = 0,
    ADI_I2C_TEN_BIT_SLAVE_ID,
} ADI_I2C_SLAVE_ID_WIDTH;

/*!
 *****************************************************************************
 * \enum ADI_I2C_DATA_ADDRESS_WIDTH
 *
 * I2C Device data address width constants
 *****************************************************************************/
typedef enum
{
    ADI_I2C_NO_DATA_ADDRESSING_PHASE  = 0,
    ADI_I2C_8_BIT_DATA_ADDRESS_WIDTH  = 1,
    ADI_I2C_12_BIT_DATA_ADDRESS_WIDTH = 3,
    ADI_I2C_16_BIT_DATA_ADDRESS_WIDTH = 2,
    ADI_I2C_32_BIT_DATA_ADDRESS_WIDTH = 4,
} ADI_I2C_DATA_ADDRESS_WIDTH;

#if (ADI_I2C_CFG_ENABLE_CALLBACK_SUPPORT==1)
/*!
 *****************************************************************************
 * \enum ADI_I2C_CALLBACK_EVENT_TYPE
 *
 * I2C Device data address width constants
 *****************************************************************************/
typedef enum
{
    /* bitfielded, so we can accumulate events */
    ADI_I2C_EVENT_MASTER_TX_COMPLETE = 0x0001,
    ADI_I2C_EVENT_MASTER_RX_COMPLETE = 0x0002,
    ADI_I2C_EVENT_MASTER_ALOST       = 0x0004,
    ADI_I2C_EVENT_MASTER_NACKADDR    = 0x0008,
    ADI_I2C_EVENT_MASTER_NACKDATA    = 0x0010,
    ADI_I2C_EVENT_SLAVE_TX_COMPLETE  = 0x0020,
    ADI_I2C_EVENT_SLAVE_RX_COMPLETE  = 0x0040,
    ADI_I2C_EVENT_SLAVE_REPSTART     = 0x0080,
    ADI_I2C_EVENT_SLAVE_STOP         = 0x0100,
    ADI_I2C_EVENT_SLAVE_CGINT        = 0x0200,
    ADI_I2C_EVENT_DMA_COMPLETE       = 0x0400,
} ADI_I2C_CALLBACK_EVENT_TYPE;
#endif /* ADI_I2C_CFG_ENABLE_CALLBACK_SUPPORT */

typedef struct ADI_I2C_DEV_DATA_TYPE* ADI_I2C_DEV_HANDLE;  /*!< I2C Device instance data handle typedef */


// Device Initialization and Uninitialization Interface
ADI_I2C_RESULT_TYPE adi_I2C_MasterInit            (ADI_I2C_DEV_ID_TYPE const devID,
                                                   ADI_I2C_DEV_HANDLE* const phDevice);
ADI_I2C_RESULT_TYPE adi_I2C_SlaveInit             (ADI_I2C_DEV_ID_TYPE const devID,
                                                   ADI_I2C_DEV_HANDLE* const phDevice);
ADI_I2C_RESULT_TYPE adi_I2C_UnInit                (ADI_I2C_DEV_HANDLE  const hDevice);

// Configuration APIs
ADI_I2C_RESULT_TYPE adi_I2C_SetSlaveDeviceIdWidth (ADI_I2C_DEV_HANDLE const hDevice, ADI_I2C_SLAVE_ID_WIDTH const width);
ADI_I2C_RESULT_TYPE adi_I2C_SetMasterClock        (ADI_I2C_DEV_HANDLE const hDevice, uint32_t const sclk);
ADI_I2C_RESULT_TYPE adi_I2C_SetBlockingMode       (ADI_I2C_DEV_HANDLE const hDevice, bool_t const bFlag);
ADI_I2C_RESULT_TYPE adi_I2C_SetDmaMode            (ADI_I2C_DEV_HANDLE const hDevice, bool_t const bFlag);

#if (ADI_I2C_CFG_ENABLE_CALLBACK_SUPPORT == 1)
ADI_I2C_RESULT_TYPE adi_I2C_RegisterCallback      (ADI_I2C_DEV_HANDLE const hDevice, ADI_CALLBACK const pfCallback, void *const pcbParam);
#endif /* ADI_I2C_CFG_ENABLE_CALLBACK_SUPPORT */

// Query APIs
ADI_I2C_RESULT_TYPE adi_I2C_GetNonBlockingStatus  (ADI_I2C_DEV_HANDLE const hDevice, uint16_t * const pBytesRemaining);
ADI_I2C_RESULT_TYPE adi_I2C_GetGeneralCallCommand (ADI_I2C_DEV_HANDLE const hDevice,  uint8_t * const pCommand);
bool_t              adi_I2C_GetDmaCompletion      (ADI_I2C_DEV_HANDLE const hDevice);

// Transaction APIs
ADI_I2C_RESULT_TYPE adi_I2C_MasterTransmit        (ADI_I2C_DEV_HANDLE const hDevice,
                                                             uint16_t const slaveID,
                                                             uint32_t const dataAddress,
                                           ADI_I2C_DATA_ADDRESS_WIDTH const dataAddressWidth,
                                                              uint8_t      *pBuffer,
                                                             uint16_t const nBufferSize,
                                                               bool_t const bRepeatStart);

ADI_I2C_RESULT_TYPE adi_I2C_MasterReceive         (ADI_I2C_DEV_HANDLE const hDevice,
                                                             uint16_t const slaveID,
                                                             uint32_t const dataAddress,
                                           ADI_I2C_DATA_ADDRESS_WIDTH const dataAddressWidth,
                                                              uint8_t      *pBuffer,
                                                             uint16_t const nBufferSize,
                                                               bool_t const bRepeatStart);

ADI_I2C_RESULT_TYPE adi_I2C_SlaveTransmit         (ADI_I2C_DEV_HANDLE const hDevice,
                                                             uint16_t const slaveID,
                                                              uint8_t      *pBuffer,
                                                             uint16_t const nBufferSize);

ADI_I2C_RESULT_TYPE adi_I2C_SlaveReceive          (ADI_I2C_DEV_HANDLE const hDevice,
                                                             uint16_t const slaveID,
                                                              uint8_t      *pBuffer,
                                                             uint16_t const nBufferSize,
                                                               bool_t const bClockStretch);

#ifdef ADI_I2C_LOOPBACK
// I2C Loopback API (test mode only, controlled by ADI_I2C_LOOPBACK macro)
ADI_I2C_RESULT_TYPE adi_I2C_MasterSlaveLoopback   (ADI_I2C_DEV_HANDLE const hDevice,
                                                             uint16_t const slaveID,
                                                              uint8_t      *pTxData,
                                                              uint8_t      *pRxData,
                                                             uint16_t const nBufferSize);
#endif

/* C++ linkage */
#ifdef __cplusplus
}
#endif

#endif /* include guard */

/*
** EOF
*/

/*@}*/
