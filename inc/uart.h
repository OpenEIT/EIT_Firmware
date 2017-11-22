/*********************************************************************************

Copyright (c) 2010-2014 Analog Devices, Inc. All Rights Reserved.

This software is proprietary to Analog Devices, Inc. and its licensors.  By using 
this software you agree to the terms of the associated Analog Devices Software 
License Agreement.

*********************************************************************************/

/*!
 *****************************************************************************
 * @file:    uart.h
 * @brief:   UART Device Definitions for ADuCxxx
 * @version: $Revision: 28525 $
 * @date:    $Date: 2014-11-12 14:51:26 -0500 (Wed, 12 Nov 2014) $
 *****************************************************************************/

/*! \addtogroup UART_Driver UART Driver
 *  @{
 */

#ifndef __UART_H__
#define __UART_H__

#include "device.h"
#include "adi_uart_config.h"

/* C++ linkage */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*!
 *  \enum ADI_UART_DEV_ID_TYPE
 *
 *  UART device identifiers
 */
typedef enum
{
    ADI_UART_DEVID_0 =0,            /*!< Uart Device ID 0             */
}ADI_UART_DEV_ID_TYPE;


/*!
 *  \enum ADI_UART_PARITY_TYPE
 *
 *  UART parity mode
 */
typedef enum
{
    ADI_UART_PARITY_NONE,           /*!< parity is disabled           */
    ADI_UART_PARITY_ODD,            /*!< odd parity                   */
    ADI_UART_PARITY_EVEN            /*!< even parity                  */
}ADI_UART_PARITY_TYPE;


/*!
 *  \enum ADI_UART_WLS_TYPE
 *
 *  UART Word or Data length
 */
typedef enum
{
    ADI_UART_WLS_5 = 0,                 /*!< word length of 5 bits        */
    ADI_UART_WLS_6 = 1,                 /*!< word length of 6 bits        */
    ADI_UART_WLS_7 = 2,                 /*!< word length of 7 bits        */
    ADI_UART_WLS_8 = 3,                 /*!< word length of 8 bits        */
}ADI_UART_WLS_TYPE;


/*!
 *  \enum ADI_UART_BAUDRATE_TYPE
 *
 *  UART default baudrates
 */
typedef enum
{
    ADI_UART_BAUD_9600 = 0, /*!< baud rate  9600             */
    ADI_UART_BAUD_19200,    /*!< baud rate 19200             */
    ADI_UART_BAUD_38400,    /*!< baud rate 38400             */
    ADI_UART_BAUD_57600,    /*!< baud rate 57600             */
    ADI_UART_BAUD_115200,   /*!< baud rate 115200            */
    ADI_UART_BAUD_230400,   /*!< baud rate 230400            */
    ADI_UART_BAUD_460800,   /*!< baud rate 460800            */
    ADI_UART_BAUD_MAX_ENTRIES /* max table size              */
} ADI_UART_BAUDRATE_TYPE;


#if defined(ADI_DEBUG)
/*!
 * \struct ADI_UART_STATS_TYPE
 *
 *  UART statistics collection block
 */
typedef struct
{
    uint16_t RxBufferFullIrqCnt;     /*!< rx buffer full interrupt count  */
    uint16_t TxBufferEmptyIrqCnt;    /*!< tx buffer empty count           */
    uint16_t RxStatusIrqCnt;         /*!< rx line interrupt count         */
    uint16_t ModemStatusIrqCnt;      /*!< modem status interrupt count    */
    uint16_t RxDmaIrqCnt;            /*!< rx dma interrupt count          */
    uint16_t TxDmaIrqCnt;            /*!< rx dma interrupt count          */
    uint16_t FrameErrorCnt;          /*!< frame error count               */
    uint16_t ParityErrorCnt;         /*!< parity error count              */
    uint16_t OverrunErrorCnt;        /*!< overrun error count             */
    uint16_t DataReadyCnt;           /*!< data ready count                */
    uint16_t RxDroppedByteCnt;       /*!< dropped receive bytes           */
} ADI_UART_STATS_TYPE;
#endif /* ADI_DEBUG */


/*!
 * \enum ADI_UART_RESULT_TYPE
 *
 *  UART result codes
 */
typedef enum
{
    ADI_UART_SUCCESS = 0,                               /*!< generic success           */
    ADI_UART_ERR_UNKNOWN = ADI_DEV_UART_ERROR_OFFSET,   /*!< generic error             */
    ADI_UART_ERR_ALREADY_INITIALIZED,                   /*!< device initialized        */
    ADI_UART_ERR_INVALID_INSTANCE,                      /*!< given instance is invalid */
    ADI_UART_ERR_INVALID_DEVID,                         /*!< invalid device id         */
    ADI_UART_ERR_INVALID_PARAMS,                        /*!< invalid parameters        */
    ADI_UART_ERR_NOT_SUPPORTED,                         /*!< unsupported operation     */
    ADI_UART_ERR_NOT_INITIALIZED,                       /*!< device not initialized    */
    ADI_UART_ERR_GPIO_CONFIG_FAILED,                    /*!< GPIO configuration failed */
    ADI_UART_ERR_INVALID_BUFFER,                        /*!< invalid or no buffer      */
    ADI_UART_ERR_SEMAPHORE_FAILED,                      /*!< semaphore-related error   */
} ADI_UART_RESULT_TYPE;


/*!
 * \enum ADI_UART_DRV_STATE_TYPE
 *
 *  UART driver state
 */
typedef enum
{
    ADI_UART_DRV_STATE_UNKNOWN = 0,                    /*!< UART  is not initialized    */
    ADI_UART_DRV_STATE_INITIALIZED,                    /*!< UART driver initialized     */
    ADI_UART_DRV_STATE_READY                           /*!< UART driver is ready        */
} ADI_UART_DRV_STATE_TYPE;


/*!
 * \struct ADI_UART_GENERIC_SETTINGS_TYPE
 *
 *  UART driver generic setting data typedef
 */
typedef struct
{
    ADI_UART_BAUDRATE_TYPE  BaudRate;            /*!< baudrate setting            */
    ADI_UART_PARITY_TYPE    Parity;              /*!< parity setting              */
    ADI_UART_WLS_TYPE       WordLength;          /*!< word length setting         */
    bool_t                  bBlockingMode;       /*!< blocking mode setting       */
    bool_t                  bInterruptMode;      /*!< interrupt mode setting      */
    bool_t                  bDmaMode;            /*!< DMA mode setting            */
    uint32_t                ConfiguredBaudRate;  /*!< baurdate configured setting */
} ADI_UART_GENERIC_SETTINGS_TYPE;

/*!
 * \struct ADI_UART_INIT_DATA
 *
 *  UART driver init data
 */
typedef struct
{
  uint8_t    *pRxBufferData;                    /*!< receive buffer area         */
  uint16_t   RxBufferSize;                      /*!< receive buffer size         */
  uint16_t   TxBufferSize;                      /*!< transmit buffer size        */
  uint8_t    *pTxBufferData;                    /*!< transmit buffer area        */
} ADI_UART_INIT_DATA;

/* UART default parameters
 *    Baud Rate   - 9600
 *    Word Length - 8 Bits
 *    Parity      - None
 *    Stop        - 1 bit
 */


/*!
 * @brief ADI_UART_HANDLE
 *
 *  Uart device handle
 */
typedef struct ADI_UART_DEV_DATA_TYPE* ADI_UART_HANDLE;


/* UART API prototypes */

extern ADI_UART_RESULT_TYPE adi_UART_Init    (const ADI_UART_DEV_ID_TYPE devID,ADI_UART_HANDLE* const pHandle,
		                              ADI_UART_INIT_DATA* const pInitData);
extern ADI_UART_RESULT_TYPE adi_UART_UnInit  (ADI_UART_HANDLE const hDevice);
extern ADI_UART_RESULT_TYPE adi_UART_BufTx   (ADI_UART_HANDLE const hDevice,const void* const pData,int16_t *pSize);
extern ADI_UART_RESULT_TYPE adi_UART_BufRx   (ADI_UART_HANDLE const hDevice,const void *pData,int16_t *pSize);
extern ADI_UART_RESULT_TYPE adi_UART_BufFlush(ADI_UART_HANDLE const hDevice);

extern ADI_UART_RESULT_TYPE adi_UART_SetBaudRate(ADI_UART_HANDLE const hDevice,const ADI_UART_BAUDRATE_TYPE BaudRate);
extern ADI_UART_RESULT_TYPE adi_UART_Enable          (ADI_UART_HANDLE const hDevice,const bool_t bFlag);
extern ADI_UART_RESULT_TYPE adi_UART_SetBlockingMode (ADI_UART_HANDLE const hDevice,const bool_t bFlag);
extern ADI_UART_RESULT_TYPE adi_UART_SetInterruptMode(ADI_UART_HANDLE const hDevice,const bool_t bFlag);
extern ADI_UART_RESULT_TYPE adi_UART_SetDmaMode      (ADI_UART_HANDLE const hDevice,const bool_t bFlag);
extern ADI_UART_RESULT_TYPE adi_UART_SetGenericSettings(ADI_UART_HANDLE const hDevice,
                                                        ADI_UART_GENERIC_SETTINGS_TYPE* const pGenericSettings);

extern ADI_UART_RESULT_TYPE adi_UART_SetLoopback    (ADI_UART_HANDLE const hDevice,const bool_t bFlag);
extern ADI_UART_RESULT_TYPE adi_UART_SetStickParity (ADI_UART_HANDLE const hDevice,const bool_t bFlag);
extern ADI_UART_RESULT_TYPE adi_UART_SetParityMode  (ADI_UART_HANDLE const hDevice,const ADI_UART_PARITY_TYPE Parity);
extern ADI_UART_RESULT_TYPE adi_UART_SetWordLength  (ADI_UART_HANDLE const hDevice,const ADI_UART_WLS_TYPE WordLength);
extern ADI_UART_RESULT_TYPE adi_UART_SetStopBit     (ADI_UART_HANDLE const hDevice,const bool_t bFlag);
extern ADI_UART_RESULT_TYPE adi_UART_SetBreakBit    (ADI_UART_HANDLE const hDevice,const bool_t bFlag);
extern ADI_UART_RESULT_TYPE adi_UART_SetHandshakeMode(ADI_UART_HANDLE const hDevice,const bool_t bFlag);

extern bool_t adi_UART_GetLoopback     (ADI_UART_HANDLE const hDevice);
extern bool_t adi_UART_GetStickParity  (ADI_UART_HANDLE const hDevice);
extern bool_t adi_UART_GetBlockingMode (ADI_UART_HANDLE const hDevice);
extern bool_t adi_UART_GetDmaMode      (ADI_UART_HANDLE const hDevice);
extern bool_t adi_UART_GetStopBit      (ADI_UART_HANDLE const hDevice);
extern bool_t adi_UART_GetEnable       (ADI_UART_HANDLE const hDevice);
extern bool_t adi_UART_GetBreakBit     (ADI_UART_HANDLE const hDevice);
// extern bool_t adi_UART_GetHandshakeMode(ADI_UART_HANDLE const hDevice);
extern bool_t adi_UART_GetInterruptMode(ADI_UART_HANDLE const hDevice);
extern uint16_t adi_UART_GetNumRxBytes (ADI_UART_HANDLE const hDevice);
extern uint16_t adi_UART_GetNumTxBytes (ADI_UART_HANDLE const hDevice);
extern uint32_t adi_UART_GetBaudRate   (ADI_UART_HANDLE const hDevice);
extern ADI_UART_RESULT_TYPE adi_UART_GetGenericSettings(ADI_UART_HANDLE const hDevice,
                                                        ADI_UART_GENERIC_SETTINGS_TYPE* const pGenericSettings);

extern ADI_UART_PARITY_TYPE    adi_UART_GetParityMode (ADI_UART_HANDLE const hDevice);
extern ADI_UART_WLS_TYPE       adi_UART_GetWordLength (ADI_UART_HANDLE const hDevice);

#if defined(ADI_DEBUG)
extern ADI_UART_RESULT_TYPE    adi_UART_GetStatistics (ADI_UART_HANDLE const hDevice,
                                                       ADI_UART_STATS_TYPE* const pUartStats);
#endif   /* ADI_DEBUG */

/* C++ linkage */
#ifdef __cplusplus
}
#endif

#endif /* include guard */

/*
** EOF
*/

/*@}*/
