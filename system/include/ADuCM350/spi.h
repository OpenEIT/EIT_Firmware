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
 * @file:    spi.h
 * @brief:   SPI Device Definitions for ADuCxxx
 * @version: $Revision: 32564 $
 * @date:    $Date: 2015-11-02 12:29:39 -0500 (Mon, 02 Nov 2015) $
 *****************************************************************************/

/*! \addtogroup SPI_Driver SPI Driver
 *  @{
 */

#ifndef __SPI_H__
#define __SPI_H__

#include "device.h"

/* C++ linkage */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*!
 *****************************************************************************
 * Maximum SPI DMA chunk size.  Note this is limited by the SPI controller's
 * 8-bit SPICNT byte count register to 256.  Previous implimentations of the
 * SPI driver did not use the SPI count register and so were not limited to
 * this 8-bit count.  For DMA transfers that fit within a single DMA
 * descriptor, the SPICNT register provides for stopping the SPI controller
 * at the exact byte count, after which the DMA engine continues to drain
 * pending bytes still in the receive FIFO.  For larger transfers that span
 * multiple DMA descriptors, the singular SPICNT is not used and so extra
 * "garbage" bytes will be clocked after the end of the real transfer while
 * the DMA engine drains the receive FIFO, but the extra bytes will be ignored
 * and not appear in the user's receive buffer.
 *****************************************************************************/
/* SPICNT size */
#define ADI_SPI_MAX_TRANSFER_SIZE      256

/*!
 *****************************************************************************
 * SPI Bitrate Initializer.  Sets a default serial clockrate for the SPI channel.
 *****************************************************************************/
//#define ADI_SPI_BITRATE_INITIALIZER   4000000 /* 4MHz default bitrate */
#define ADI_SPI_BITRATE_INITIALIZER      250000  /* depends on processor */

/*!
 *****************************************************************************
 * SPI0/SPI1 Control Register Initializer.  This macro configures default
 * settings for the SPI configuration control register when operated in Master-mode.
 *****************************************************************************/
/* SPI master DMA mode control configuration */
#define ADI_SPI_MASTERCON_INITIALIZER      SPICON_MASEN | SPICON_ENABLE

/*!
 *****************************************************************************
 * SPI0/SPI1 Control Register Initializer.  This macro configures default
 * settings for the SPI configuration control register when operated in Slave-mode.
 *****************************************************************************/
 #define ADI_SPI_SLAVECON_INITIALIZER  SPICON_OEN           \
                                     | SPICON_ZEN           \
                                     | SPICON_ENABLE

/*!
 *****************************************************************************
 * SPI DMA Enable Register Initializer.  Sets default DMA enablement to
 * disabled.  Use the #adi_SPI_SetDmaMode() API to enable DMA-based transactions.
 *****************************************************************************/
 #define ADI_SPI_DMA_INITIALIZER     0


/*!
 *****************************************************************************
 * \enum ADI_SPI_DEV_ID_TYPE
 *
 * SPI device IDs.  Depending on device configuration there may
 * be one or more physical controllers.  In the case of SPI, there are two.
 * Use the #ADI_SPI_DEV_ID_TYPE enumerator to select which device is being
 * initialized by function adi_SPI_Init() or adi_SPI_SlaveInit().
 *
 *****************************************************************************/
typedef enum {
    ADI_SPI_DEVID_0 = 0,              /*!< SPI0 device ID.                    */
    ADI_SPI_DEVID_1,                  /*!< SPI1 device ID.                    */
    ADI_SPI_DEVID_H,                  /*!< SPIH device ID.                    */
    ADI_SPI_MAX_DEVID
} ADI_SPI_DEV_ID_TYPE;


/*!
 *****************************************************************************
 * \enum ADI_SPI_RESULT_TYPE
 *
 * SPI Device Error Codes.  #ADI_SPI_SUCCESS is always zero and all other SPI
 * error codes are offset by the base SPI error code offset,
 * #ADI_DEV_SPI_ERROR_OFFSET.  A number of SPI error codes are debug-only,
 * as controlled by the "ADI_DEBUG" build macro.
 *
 * The return value of all SPI APIs returning #ADI_SPI_RESULT_TYPE
 * should always be tested at the application level for success or failure.
 *
 *****************************************************************************/
typedef enum
{
    ADI_SPI_SUCCESS                 = 0,                            /*!< Generic success.                    */
    ADI_SPI_ERR_UNKNOWN             = ADI_DEV_SPI_ERROR_OFFSET,     /*!< Generic error.                      */
    ADI_SPI_ERR_ALREADY_INITIALIZED = ADI_DEV_SPI_ERROR_OFFSET+1,   /*!< SPI device is already initialized.  */
    ADI_SPI_ERR_BAD_DEV_HANDLE      = ADI_DEV_SPI_ERROR_OFFSET+2,   /*!< Invalid device handle.              */
    ADI_SPI_ERR_BAD_DEV_ID          = ADI_DEV_SPI_ERROR_OFFSET+3,   /*!< Invalid device ID.                  */
    ADI_SPI_ERR_BAD_DMA_CONFIG      = ADI_DEV_SPI_ERROR_OFFSET+4,   /*!< DMA configuration failure.          */
    ADI_SPI_ERR_BAD_GPIO_CONFIG     = ADI_DEV_SPI_ERROR_OFFSET+5,   /*!< SPI failed to configure GPIO.       */
    ADI_SPI_ERR_DMA_ALREADY_IN_USE  = ADI_DEV_SPI_ERROR_OFFSET+6,   /*!< DMA is already in progress.         */
    ADI_SPI_ERR_NO_DMA_FOR_DEVICE   = ADI_DEV_SPI_ERROR_OFFSET+7,   /*!< DMA is not defined for this device. */
    ADI_SPI_ERR_NOT_INITIALIZED     = ADI_DEV_SPI_ERROR_OFFSET+8,   /*!< SPI device is uninitialized.        */
    ADI_SPI_ERR_NULL_DATA_POINTER   = ADI_DEV_SPI_ERROR_OFFSET+9,   /*!< NULL data pointer not allowed.      */
    ADI_SPI_ERR_PARAM_OUT_OF_RANGE  = ADI_DEV_SPI_ERROR_OFFSET+10,  /*!< Parameter is out of range.          */
    ADI_SPI_ERR_UNSUPPORTED_MODE    = ADI_DEV_SPI_ERROR_OFFSET+11,  /*!< Unsupported mode of operation.      */
    ADI_SPI_ERR_SEMAPHORE_FAILED    = ADI_DEV_SPI_ERROR_OFFSET+12,  /*!< Semaphore in error           .      */
} ADI_SPI_RESULT_TYPE;

/*!
 *****************************************************************************
 * \enum ADI_SPI_EVENT
 *
 * Enumeration of events notified in the application provided callback.
 *****************************************************************************/
typedef enum
{
    /*!< The given buffer is processed. Application can use this event to submit
         the next buffer to be transmitted. */
    ADI_SPI_EVENT_BUFFER_PROCESSED  = 0

} ADI_SPI_EVENT;

/*!
 *****************************************************************************
 * \enum ADI_SPI_STATE_TYPE
 *
 * SPI driver state.  Used for internal tracking of the SPI device initialization
 * progress during the adi_SPI_Init() and adi_SPI_SlaveInit() functions.  Also
 * used to insure the SPI device has been properly initialized as a prerequisite
 * to using the balance of the SPI API.
 *
 *****************************************************************************/
typedef enum
{
    ADI_SPI_STATE_UNINITIALIZED= 0,        /*!< SPI is not initialized.            */
    ADI_SPI_STATE_INITIALIZING,            /*!< SPI initialization is in progress. */
    ADI_SPI_STATE_MASTER,                  /*!< SPI initialization is complete.    */
    ADI_SPI_STATE_SLAVE                    /*!< SPI initialization is complete.    */
} ADI_SPI_STATE_TYPE;


/*!
 *****************************************************************************
 * \enum ADI_SPI_IRQ_MODE_TYPE
 *
 * SPI driver interrupt mode.  Allows designation through the
 * #adi_SPI_SetInterruptMode() API of when the SPI controller generates interrupts,
 * based on the number of transmit and receive bytes transacted.  This is helpful
 * in managing the rate of interrupts vs. the the data FIFO "fullness", the idea being
 * to minimize the number of interrupts while not allowing either FIFO to overflow or underflow.
 * Only the 4 combinations listed are supported by the hardware.
 *
 *****************************************************************************/
typedef enum
{
    ADI_SPI_IRQMODE_T1R1 = 0,      /*!< Interrupt every 1-byte tx, 1-byte rx. */
    ADI_SPI_IRQMODE_T2R2,          /*!< Interrupt every 2-byte tx, 2-byte rx. */
    ADI_SPI_IRQMODE_T3R3,          /*!< Interrupt every 3-byte tx, 3-byte rx. */
    ADI_SPI_IRQMODE_T4R4           /*!< Interrupt every 4-byte tx, 4-byte rx. */
} ADI_SPI_IRQ_MODE_TYPE;


/*!
 *****************************************************************************
 * \struct ADI_SPI_CHIP_SELECT_TYPE
 *
 * SPI Device Chip Select struct.  
 *
 * Allows designation of an external SPI slave device chip select pin to be 
 * driven by the SPI controller (a.k.a SW Chip Select)
 * Multiple external slave SPI devices may be present on a shared SPI bus,
 * and the chip select pin allows each of them to be assigned dedicated selects.
 * Use the #adi_SPI_SetChipSelect() API to configure the active chip select.
 *
 * Alternatively, if HWCS is 'true', then the SPI controller's HW chip select
 * will be used.
 *
 *****************************************************************************/
typedef struct
{
    ADI_GPIO_PORT_TYPE       Port;           /*!< Chip select GPIO port   */
    ADI_GPIO_DATA_TYPE       Pin;            /*!< Chip select GPIO pin    */
    bool                     HWCS;           /*!< HW Chip Select          */
} ADI_SPI_CHIP_SELECT_TYPE;



typedef struct ADI_SPI_DEV_DATA_TYPE* ADI_SPI_DEV_HANDLE;  /*!< SPI Device instance private data handle typedef. */


/*!
 *****************************************************************************
 * \struct ADI_SPI_DEV_GENERIC_SETTINGS_TYPE
 *
 * SPI Device Generic Settings Structure.  The generic settings data type is
 * used with the #adi_SPI_SetGenericSettings() and #adi_SPI_GetGenericSettings()
 * APIs to support the management of common settings in a singular API.  All
 * the elements of the ADI_SPI_DEV_GENERIC_SETTINGS_TYPE are also manageable
 * individually by dedicated APIs, but using the generic API makes for less
 * overhead.
 *
 *****************************************************************************/
typedef struct
{
    /* generic settings structure to pass common settings        */
    uint32_t Bitrate;               /*!< Bitrate frequency (Hz). */
    ADI_SPI_CHIP_SELECT_TYPE cs;    /*!< Current chip select.    */
    bool_t bBlockingMode;           /*!< Blocking mode.          */
    bool_t bDmaMode;                /*!< DMA mode.               */
} ADI_SPI_DEV_GENERIC_SETTINGS_TYPE;


/*!
  * \struct ADI_SPI_TRANSCEIVE_TYPE
 *****************************************************************************
 * SPI Device Command/Data Transceiver Structure.  Data structure used by
 * the #adi_SPI_Transceiver() API to convey all parameters, consisting of
 * prologue, transmit and receive data and size, and buffer increment flags.
 *****************************************************************************/
typedef struct
{
    /* pointers... */
    uint8_t*    pPrologue;      /*!< Pointer to prologue data.        */
    uint8_t*    pTxData;        /*!< Pointer to transmit data.        */
    uint8_t*    pRxData;        /*!< Pointer to receive data.         */

    /* attributes... */
    uint32_t    PrologueSize;   /*!< Size of prologue data (bytes).   */
    uint32_t    DataSize;       /*!< Data size (bytes).               */
    bool_t      bTxIncrement;   /*!< Transmit pointer increment flag. */
    bool_t      bRxIncrement;   /*!< Receive pointer increment flag.  */
} ADI_SPI_TRANSCEIVE_TYPE;



/******************************************************************************
 * SPI Device External API function prototypes
 *****************************************************************************/

/* Device Initialization and Uninitialization Interfaces */
extern ADI_SPI_RESULT_TYPE     adi_SPI_MasterInit              (ADI_SPI_DEV_ID_TYPE const devID,
                                                                ADI_SPI_DEV_HANDLE* const pHandle);

extern ADI_SPI_RESULT_TYPE     adi_SPI_SlaveInit               (ADI_SPI_DEV_ID_TYPE const devID,
                                                                ADI_SPI_DEV_HANDLE* const pHandle);

extern ADI_SPI_RESULT_TYPE     adi_SPI_UnInit                  (ADI_SPI_DEV_HANDLE const hDevice);


/******************************************************************
 * Eliminatable functions that may be optimized out by the linker *
 *****************************************************************/

/* Slave Mode APIs */
extern ADI_SPI_RESULT_TYPE     adi_SPI_SlaveTransfer           (ADI_SPI_DEV_HANDLE const hDevice, ADI_SPI_TRANSCEIVE_TYPE* const pXfr);
extern bool_t                  adi_SPI_SlaveComplete           (ADI_SPI_DEV_HANDLE const hDevice);

/* Hardware Configuration Interface */
extern ADI_SPI_RESULT_TYPE     adi_SPI_SetClockPhase           (ADI_SPI_DEV_HANDLE const hDevice, const bool_t bFlag);
extern ADI_SPI_RESULT_TYPE     adi_SPI_SetClockPolarity        (ADI_SPI_DEV_HANDLE const hDevice, const bool_t bFlag);
extern ADI_SPI_RESULT_TYPE     adi_SPI_SetContinousMode        (ADI_SPI_DEV_HANDLE const hDevice, const bool_t bFlag);
extern ADI_SPI_RESULT_TYPE     adi_SPI_SetFifoRxFlush          (ADI_SPI_DEV_HANDLE const hDevice, const bool_t bFlag);
extern ADI_SPI_RESULT_TYPE     adi_SPI_SetFifoTxFlush          (ADI_SPI_DEV_HANDLE const hDevice, const bool_t bFlag);
extern ADI_SPI_RESULT_TYPE     adi_SPI_SetGenericSettings      (ADI_SPI_DEV_HANDLE const hDevice, ADI_SPI_DEV_GENERIC_SETTINGS_TYPE* const pSettings);
extern ADI_SPI_RESULT_TYPE     adi_SPI_SetInterruptMode        (ADI_SPI_DEV_HANDLE const hDevice, const ADI_SPI_IRQ_MODE_TYPE Mode);
extern ADI_SPI_RESULT_TYPE     adi_SPI_SetLoopback             (ADI_SPI_DEV_HANDLE const hDevice, const bool_t bFlag);
extern ADI_SPI_RESULT_TYPE     adi_SPI_SetLsbFirst             (ADI_SPI_DEV_HANDLE const hDevice, const bool_t bFlag);
extern ADI_SPI_RESULT_TYPE     adi_SPI_SetMasterMode           (ADI_SPI_DEV_HANDLE const hDevice, const bool_t bFlag);
extern ADI_SPI_RESULT_TYPE     adi_SPI_SetMisoOutput           (ADI_SPI_DEV_HANDLE const hDevice, const bool_t bFlag);
extern ADI_SPI_RESULT_TYPE     adi_SPI_SetReceiveOverflow      (ADI_SPI_DEV_HANDLE const hDevice, const bool_t bFlag);
extern ADI_SPI_RESULT_TYPE     adi_SPI_SetTransferInitiateMode (ADI_SPI_DEV_HANDLE const hDevice, const bool_t bFlag);
extern ADI_SPI_RESULT_TYPE     adi_SPI_SetTransmitUnderflow    (ADI_SPI_DEV_HANDLE const hDevice, const bool_t bFlag);
extern ADI_SPI_RESULT_TYPE     adi_SPI_SetWiredOrMode          (ADI_SPI_DEV_HANDLE const hDevice, const bool_t bFlag);
extern ADI_SPI_RESULT_TYPE     adi_SPI_SetHWChipSelect         (ADI_SPI_DEV_HANDLE const hDevice, const bool_t bFlag);

/* Hardware Query Interface */
extern bool_t                  adi_SPI_GetClockPhase           (ADI_SPI_DEV_HANDLE const hDevice);
extern bool_t                  adi_SPI_GetClockPolarity        (ADI_SPI_DEV_HANDLE const hDevice);
extern bool_t                  adi_SPI_GetContinousMode        (ADI_SPI_DEV_HANDLE const hDevice);
extern bool_t                  adi_SPI_GetFifoRxFlush          (ADI_SPI_DEV_HANDLE const hDevice);
extern bool_t                  adi_SPI_GetFifoTxFlush          (ADI_SPI_DEV_HANDLE const hDevice);
extern ADI_SPI_RESULT_TYPE     adi_SPI_GetGenericSettings      (ADI_SPI_DEV_HANDLE const hDevice, ADI_SPI_DEV_GENERIC_SETTINGS_TYPE* const pSettings);
extern ADI_SPI_IRQ_MODE_TYPE   adi_SPI_GetInterruptMode        (ADI_SPI_DEV_HANDLE const hDevice);
extern bool_t                  adi_SPI_GetLoopback             (ADI_SPI_DEV_HANDLE const hDevice);
extern bool_t                  adi_SPI_GetLsbFirst             (ADI_SPI_DEV_HANDLE const hDevice);
extern bool_t                  adi_SPI_GetMasterMode           (ADI_SPI_DEV_HANDLE const hDevice);
extern bool_t                  adi_SPI_GetMisoOutput           (ADI_SPI_DEV_HANDLE const hDevice);
extern bool_t                  adi_SPI_GetReceiveOverflow      (ADI_SPI_DEV_HANDLE const hDevice);
extern bool_t                  adi_SPI_GetTransferInitiateMode (ADI_SPI_DEV_HANDLE const hDevice);
extern bool_t                  adi_SPI_GetTransmitUnderflow    (ADI_SPI_DEV_HANDLE const hDevice);
extern bool_t                  adi_SPI_GetWiredOrMode          (ADI_SPI_DEV_HANDLE const hDevice);

/* Mode Configuration Interface */
extern ADI_SPI_RESULT_TYPE     adi_SPI_SetBitrate              (ADI_SPI_DEV_HANDLE const hDevice, const uint32_t Hertz);
extern ADI_SPI_RESULT_TYPE     adi_SPI_SetBlockingMode         (ADI_SPI_DEV_HANDLE const hDevice, const bool_t bFlag);
extern ADI_SPI_RESULT_TYPE     adi_SPI_SetChipSelect           (ADI_SPI_DEV_HANDLE const hDevice, const ADI_GPIO_PORT_TYPE Port, const ADI_GPIO_DATA_TYPE Pin);
extern ADI_SPI_RESULT_TYPE     adi_SPI_SetDmaMode              (ADI_SPI_DEV_HANDLE const hDevice, const bool_t bFlag);

/* Mode Query Interface **/
extern uint32_t                adi_SPI_GetBitrate              (ADI_SPI_DEV_HANDLE const hDevice);
extern ADI_SPI_CHIP_SELECT_TYPE
                               adi_SPI_GetChipSelect           (ADI_SPI_DEV_HANDLE const hDevice);
extern bool_t                  adi_SPI_GetBlockingMode         (ADI_SPI_DEV_HANDLE const hDevice);
extern bool_t                  adi_SPI_GetDmaMode              (ADI_SPI_DEV_HANDLE const hDevice);
extern bool_t                  adi_SPI_GetDmaRxComplete        (ADI_SPI_DEV_HANDLE const hDevice);
extern bool_t                  adi_SPI_GetDmaTxComplete        (ADI_SPI_DEV_HANDLE const hDevice);

/* Command/Data transceiver API */
extern ADI_SPI_RESULT_TYPE     adi_SPI_MasterTransfer          (ADI_SPI_DEV_HANDLE const hDevice, ADI_SPI_TRANSCEIVE_TYPE* const pXfr);
extern bool_t                  adi_SPI_MasterComplete          (ADI_SPI_DEV_HANDLE const hDevice);

/* C++ linkage */
#ifdef __cplusplus
}
#endif

#endif /* include guard */

/*
** EOF
*/

/*@}*/
