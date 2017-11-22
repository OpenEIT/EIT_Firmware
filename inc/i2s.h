/*********************************************************************************

Copyright (c) 2010-2014 Analog Devices, Inc. All Rights Reserved.

This software is proprietary to Analog Devices, Inc. and its licensors.  By using 
this software you agree to the terms of the associated Analog Devices Software 
License Agreement.

*********************************************************************************/

/*!
 ******************************************************************************
 * @file:    I2S.h
 * @brief:   I2S Device API header file
 * @version: $Revision: 28525 $
 * @date:    $Date: 2014-11-12 14:51:26 -0500 (Wed, 12 Nov 2014) $
 *****************************************************************************/

/*! \addtogroup I2S_Driver I2S Driver
 *  @{
 */

#ifndef __I2S_H__
#define __I2S_H__

/* Processor specific definitions,  memory map, etc. */
#include "device.h"

#include "adi_int.h"
#include "adi_i2s_config.h"


/* C++ linkage */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*!
 *****************************************************************************
 * \enum ADI_I2S_DEV_ID_TYPE
 *
 * I2S device IDs
 *****************************************************************************/
typedef enum {
    ADI_I2S_DEVID_0 = 0,              /*!< I2S device ID                    */
    ADI_I2S_MAX_DEVID
} ADI_I2S_DEV_ID_TYPE;


/*!
 *****************************************************************************
 * \enum ADI_I2S_RESULT_TYPE
 *
 * I2S Device Error Codes
 *****************************************************************************/
typedef enum
{
    ADI_I2S_SUCCESS = 0,             /*!< Generic success                        */

    ADI_I2S_ERR_UNKNOWN              /*!< Generic error                          */
        = ADI_DEV_I2S_ERROR_OFFSET,

    ADI_I2S_ERR_ALREADY_INITIALIZED, /*!< I2S device is already initialized.      */
    ADI_I2S_ERR_BAD_API_FOR_MODE,    /*!< Invalid API for current mode.           */
    ADI_I2S_ERR_BAD_DEVICE_HANDLE,   /*!< Invalid device handle.                  */
    ADI_I2S_ERR_BAD_DEVICE_INDEX,    /*!< Invalid physical device index.          */
    ADI_I2S_ERR_BAD_DMA_CONFIG,      /*!< DMA configuration failure.              */
    ADI_I2S_ERR_BAD_MODE_COMBO,      /*!< DMA and Blocking-Mode incompatible.     */
    ADI_I2S_ERR_BLOCKING_ACTIVE,     /*!< Blocking mode is active.                */
    ADI_I2S_ERR_NOT_INITIALIZED,     /*!< I2S device is uninitialized.            */
    ADI_I2S_ERR_NULL_DATA_POINTER,   /*!< NULL data pointer not allowed.          */
    ADI_I2S_ERR_INVALID_DATA_SIZE,   /*!< The given data buffer size is invalid.  */
    ADI_I2S_ERR_SEMAPHORE_FAILED,    /*!< Semaphore related failure.              */

} ADI_I2S_RESULT_TYPE;


/*!
 *****************************************************************************
 * \enum ADI_I2S_STATE_TYPE
 *
 *  I2S driver state.
 *****************************************************************************/
typedef enum
{
    ADI_I2S_STATE_UNINITIALIZED = 0,  /*!< I2S is not initialized. */
    ADI_I2S_STATE_INITIALIZED,        /*!< I2S is initialized.  */
} ADI_I2S_STATE_TYPE;


/*!
 *****************************************************************************
 * \enum ADI_I2S_DATA_FORMAT_TYPE
 *
 * I2S Data format.
 *****************************************************************************/
typedef enum
{
    ADI_I2S_DATA_FORMAT_DEFAULT               = 0,   /*!< Delay by 1 blck cycle. */
    ADI_I2S_DATA_FORMAT_LEFT_JUSTIFIED        = 1,   /*!< Left justified sdata format. */
    ADI_I2S_DATA_FORMAT_24BIT_RIGHT_JUSTIFIED = 2,   /*!< 24-bit right justified sdata format, 32 BLCKS per chanel required in stereo mode */
    ADI_I2S_DATA_FORMAT_16BIT_RIGHT_JUSTIFIED = 3    /*!< 16-bit right justified sdata format, 32 BLCKS per chanel required in stereo mode. */

} ADI_I2S_DATA_FORMAT_TYPE;

/*!
 *****************************************************************************
 * \enum ADI_I2S_CHANNEL_FORMAT_TYPE
 *
 * I2S channel format.
 *****************************************************************************/
typedef enum
{
    ADI_I2S_CHANNEL_FORMAT_STEREO   = 0,            /*!< Stereo mode operation. */
    ADI_I2S_CHANNEL_FORMAT_TDM2     = 1,            /*!< TDM2 mode operation. */
    ADI_I2S_CHANNEL_FORMAT_TDM4     = 2,            /*!< TDM4 mode operation. */
    ADI_I2S_CHANNEL_FORMAT_TDM8     = 3,            /*!< TDM8 mode operation. */
    ADI_I2S_CHANNEL_FORMAT_TDM16    = 4,            /*!< TDM16 mode operation. */
    ADI_I2S_CHANNEL_FORMAT_MONO_PCM = 5             /*!< Mono/PCM mode operation. */

} ADI_I2S_CHANNEL_FORMAT_TYPE;

/*!
 *****************************************************************************
 * \enum ADI_I2S_FRAME_RATE_TYPE
 *
 * I2S Frame rates.
 *****************************************************************************/
typedef enum
{
    ADI_I2S_FRAME_RATE_8_KHZ     = 0,   /*!< 8Khz frame rate. */
    ADI_I2S_FRAME_RATE_16_KHZ    = 1,   /*!< 16Khz frame rate. */
    ADI_I2S_FRAME_RATE_32_KHZ    = 2,   /*!< 32Khz frame rate. */
    ADI_I2S_FRAME_RATE_64_KHZ    = 3,   /*!< 64Khz frame rate. */
    ADI_I2S_FRAME_RATE_128_KHZ   = 4    /*!< 128Khz frame rate. */

} ADI_I2S_FRAME_RATE_TYPE;

/*!
 *****************************************************************************
 * \enum ADI_I2S_SLOT_WIDTH_TYPE
 *
 * I2S TDM Slot widths.
 *****************************************************************************/
typedef enum
{
    ADI_I2S_SLOT_WIDTH_32_BIT     = 0,    /*!< 32-bit channel slots in TDM mode. */
    ADI_I2S_SLOT_WIDTH_24_BIT     = 1,    /*!< 24-bit channel slots in TDM mode. */
    ADI_I2S_SLOT_WIDTH_16_BIT     = 2     /*!< 16-bit channel slots in TDM mode. */

} ADI_I2S_SLOT_WIDTH_TYPE;

/*!
 *****************************************************************************
 * \enum ADI_I2S_DATA_WIDTH_TYPE
 *
 * I2S Sample widths.
 *****************************************************************************/
typedef enum
{
    ADI_I2S_DATA_WIDTH_24_BIT     = 0,    /*!< 24-bit data I/O. */
    ADI_I2S_DATA_WIDTH_16_BIT     = 1     /*!< 16-bit data I/O. */

} ADI_I2S_DATA_WIDTH_TYPE;

/*!
 *****************************************************************************
 * \enum ADI_I2S_LRCLK_MODE_TYPE
 *
 * I2S LR Clock modes.
 *****************************************************************************/
typedef enum
{
    ADI_I2S_LR_MODE_50_DUTY_CYCLE    = 0,    /*!< LRCLK (frame clock) 50% duty cycle in TDM mode. */
    ADI_I2S_LR_MODE_SINGLE_PULSE     = 1     /*!< LRCLK (frame clock) single cycle pulse in TDM mode. */

} ADI_I2S_LRCLK_MODE_TYPE;

/*!
 *****************************************************************************
 * \enum ADI_I2S_ENDIAN_TYPE
 *
 * I2S endian type.
 *****************************************************************************/
typedef enum
{
    ADI_I2S_ENDIAN_MSB_FIRST    = 0,    /*!< Big Endian.. */
    ADI_I2S_ENDIAN_LSB_FIRST    = 1     /*!< Little Endian. */

} ADI_I2S_ENDIAN_TYPE;

/*!
 *****************************************************************************
 * \enum ADI_I2S_BCLK_RATE_TYPE
 *
 * I2S Bit clock rate.
 *****************************************************************************/
typedef enum
{
    ADI_I2S_BCLK_RATE_32_BCLKS    = 0,    /*!< Bit clock rate is 32 clocks per channel. */
    ADI_I2S_BCLK_RATE_16_BCLKS    = 1     /*!< Bit clock rate is 16 clocks per channel. */

} ADI_I2S_BCLK_RATE_TYPE;

/*!
 *****************************************************************************
 * \enum ADI_I2S_MASTER_SLAVE_SELECT_TYPE
 *
 * I2S Master or slave mode selection.
 *****************************************************************************/
typedef enum
{
    ADI_I2S_MASTER_SLAVE_SELECT_SLAVE     = 0,    /*!< Slave mode operation. Use external BCLK, LRCLK. */
    ADI_I2S_MASTER_SLAVE_SELECT_MASTER    = 1     /*!< Master mode operation. Use internally generated BCLK, LRCLK. */

} ADI_I2S_MASTER_SLAVE_SELECT_TYPE;


/*!
 *****************************************************************************
 * \enum ADI_I2S_LRCLK_POLARITY_TYPE
 *
 * I2S LR clock polarity.
 *****************************************************************************/
typedef enum
{
    ADI_I2S_LRCLK_POLARITY_NO_INVERT     = 0,    /*!< Do not invert LRCLK to port. */
    ADI_I2S_LRCLK_POLARITY_INVERT        = 1     /*!< Invert LRCLK to port. */

} ADI_I2S_LRCLK_POLARITY_TYPE;

/*!
 *****************************************************************************
 * \enum ADI_I2S_BLCK_EDGE_TYPE
 *
 * I2S bit clock sampling edge.
 *****************************************************************************/
typedef enum
{
    ADI_I2S_BLK_EDGE_FALLING     = 0,    /*!< Sdata streams change on the falling edge of BCLK. */
    ADI_I2S_BLK_EDGE_RISING      = 1     /*!< Sdata streams change on the rising edge of BCLK. */

} ADI_I2S_BLCK_EDGE_TYPE;


/*!
 *****************************************************************************
 * \enum ADI_I2S_CHANNEL_2_DRIVE_TYPE
 *
 * I2S Channel 2 slot drive or disable.
 *****************************************************************************/
typedef enum
{
    ADI_I2S_CHANNEL_2_DRIVE_DIS     = 0,    /*!< Don't drive channel 2 slot on output SDATA */
    ADI_I2S_CHANNEL_2_DRIVE_EN      = 1     /*!< Drive channel 2 slot on output SDATA. */

} ADI_I2S_CHANNEL_2_DRIVE_TYPE;

/*!
 *****************************************************************************
 * \enum ADI_I2S_CHANNEL_1_DRIVE_TYPE
 *
 * I2S Channel 1 Slot driver or disable.
 *****************************************************************************/
typedef enum
{
    ADI_I2S_CHANNEL_1_DRIVE_DIS     = 0,    /*!< Don't drive channel 1 slot on output SDATA */
    ADI_I2S_CHANNEL_1_DRIVE_EN      = 1     /*!< Drive channel 1 slot on output SDATA. */

} ADI_I2S_CHANNEL_1_DRIVE_TYPE;

/*!
 *****************************************************************************
 * \enum ADI_I2S_CHANNEL_MAP_TYPE
 *
 * I2S channel map to a particular TDM slot.
 *****************************************************************************/
typedef enum
{
    ADI_I2S_CHANNEL_MAP_SLOT0         = 0,       /*!< Map the channel to slot 0. */
    ADI_I2S_CHANNEL_MAP_SLOT1         = 1,       /*!< Map the channel to slot 1. */
    ADI_I2S_CHANNEL_MAP_SLOT2         = 2,       /*!< Map the channel to slot 2. */
    ADI_I2S_CHANNEL_MAP_SLOT3         = 3,       /*!< Map the channel to slot 3. */
    ADI_I2S_CHANNEL_MAP_SLOT4         = 4,       /*!< Map the channel to slot 4. */
    ADI_I2S_CHANNEL_MAP_SLOT5         = 5,       /*!< Map the channel to slot 5. */
    ADI_I2S_CHANNEL_MAP_SLOT6         = 6,       /*!< Map the channel to slot 6. */
    ADI_I2S_CHANNEL_MAP_SLOT7         = 7,       /*!< Map the channel to slot 7. */
    ADI_I2S_CHANNEL_MAP_SLOT8         = 8,       /*!< Map the channel to slot 8. */
    ADI_I2S_CHANNEL_MAP_SLOT9         = 9,       /*!< Map the channel to slot 9. */
    ADI_I2S_CHANNEL_MAP_SLOT10        = 10,      /*!< Map the channel to slot 10. */
    ADI_I2S_CHANNEL_MAP_SLOT11        = 11,      /*!< Map the channel to slot 11. */
    ADI_I2S_CHANNEL_MAP_SLOT12        = 12,      /*!< Map the channel to slot 12. */
    ADI_I2S_CHANNEL_MAP_SLOT13        = 13,      /*!< Map the channel to slot 13. */
    ADI_I2S_CHANNEL_MAP_SLOT14        = 14,      /*!< Map the channel to slot 14. */
    ADI_I2S_CHANNEL_MAP_SLOT15        = 15       /*!< Map the channel to slot 15. */

} ADI_I2S_CHANNEL_MAP_TYPE;



/*!
 *****************************************************************************
 * \enum ADI_I2S_FIFO_RESET_TYPE
 *
 * FIFO Reset and Hold.
 *****************************************************************************/
typedef enum
{
    ADI_I2S_FIFO_RESET_NORMAL     = 0,    /*!< Normal FIFO operation. */
    ADI_I2S_FIFO_RESET_HOLD       = 1     /*!< reset and hold FIFO pointers for serial output. */

} ADI_I2S_FIFO_RESET_TYPE;

/*!
 *****************************************************************************
 * \enum ADI_I2S_INCR_RESET_TYPE
 *
 * Address auto increment reset.
 *****************************************************************************/
typedef enum
{
    ADI_I2S_INCR_RESET_NORMAL     = 0,    /*!< Address Normal FIFO operation. */
    ADI_I2S_INCR_RESET_AUTO       = 1     /*!< Reset auto-increment address. */

} ADI_I2S_INCR_RESET_TYPE;


/*!
 *****************************************************************************
 * \enum ADI_I2S_TRAN_MODE_TYPE
 *
 * Addressing auto increment.
 *****************************************************************************/
typedef enum
{
    ADI_I2S_TRAN_MODE_24            = 0,     /*!< 24-bit audio transfers over APB bus (2 writes per channel). */
    ADI_I2S_TRAN_MODE_16            = 1,     /*!< 16-bit audio transfers over APB bus (1 write per channel). */
    ADI_I2S_TRAN_MODE_8             = 2      /*!< 8-bit audio transfers over APB bus (1 write per channel). */

} ADI_I2S_TRAN_MODE_TYPE;


/*!
 *****************************************************************************
 * \enum ADI_I2S_EVENT
 *
 * Enumeration of events notified in the application provided callback.
 *****************************************************************************/
typedef enum
{
    /*!< The given buffer is processed. Application can use this event to submit
         the next buffer to be transmitted. */
    ADI_I2S_EVENT_BUFFER_PROCESSED  = 0

} ADI_I2S_EVENT;



typedef struct ADI_I2S_DEV_DATA_TYPE* ADI_I2S_DEV_HANDLE;  /*!< I2S Device instance data handle typedef */


// Device Initialization and Uninitialization Interface
ADI_I2S_RESULT_TYPE adi_I2S_Init                  (ADI_I2S_DEV_ID_TYPE const devID, ADI_I2S_DEV_HANDLE* const phDevice);
ADI_I2S_RESULT_TYPE adi_I2S_UnInit                (ADI_I2S_DEV_HANDLE  const hDevice);

// Configuration APIs
ADI_I2S_RESULT_TYPE adi_I2S_SetDataFormat         (ADI_I2S_DEV_HANDLE const hDevice, ADI_I2S_DATA_FORMAT_TYPE const );
ADI_I2S_RESULT_TYPE adi_I2S_SetChannelFormat      (ADI_I2S_DEV_HANDLE const hDevice, ADI_I2S_CHANNEL_FORMAT_TYPE const );
ADI_I2S_RESULT_TYPE adi_I2S_FrameRate             (ADI_I2S_DEV_HANDLE const hDevice, ADI_I2S_FRAME_RATE_TYPE const );
ADI_I2S_RESULT_TYPE adi_I2S_SlotWidth             (ADI_I2S_DEV_HANDLE const hDevice, ADI_I2S_SLOT_WIDTH_TYPE const );
ADI_I2S_RESULT_TYPE adi_I2S_DataWidth             (ADI_I2S_DEV_HANDLE const hDevice, ADI_I2S_DATA_WIDTH_TYPE const );
ADI_I2S_RESULT_TYPE adi_I2S_SetLRCLKMode          (ADI_I2S_DEV_HANDLE const hDevice, ADI_I2S_LRCLK_MODE_TYPE const );
ADI_I2S_RESULT_TYPE adi_I2S_SetEndianFormat       (ADI_I2S_DEV_HANDLE const hDevice, ADI_I2S_ENDIAN_TYPE const );
ADI_I2S_RESULT_TYPE adi_I2S_SetBLCKRate           (ADI_I2S_DEV_HANDLE const hDevice, ADI_I2S_BCLK_RATE_TYPE const );
ADI_I2S_RESULT_TYPE adi_I2S_SetMasterSlave        (ADI_I2S_DEV_HANDLE const hDevice, ADI_I2S_MASTER_SLAVE_SELECT_TYPE const );
ADI_I2S_RESULT_TYPE adi_I2S_SetDriveUnusedBits    (ADI_I2S_DEV_HANDLE const hDevice, const bool_t bFlag);
ADI_I2S_RESULT_TYPE adi_I2S_SetLRCLKPolarity      (ADI_I2S_DEV_HANDLE const hDevice, ADI_I2S_LRCLK_POLARITY_TYPE const );
ADI_I2S_RESULT_TYPE adi_I2S_SetBLCKEdge           (ADI_I2S_DEV_HANDLE const hDevice, ADI_I2S_BLCK_EDGE_TYPE const );
ADI_I2S_RESULT_TYPE adi_I2S_SetChannel_2_Drive    (ADI_I2S_DEV_HANDLE const hDevice, ADI_I2S_CHANNEL_2_DRIVE_TYPE const );
ADI_I2S_RESULT_TYPE adi_I2S_SetChannel_1_Drive    (ADI_I2S_DEV_HANDLE const hDevice, ADI_I2S_CHANNEL_1_DRIVE_TYPE const );
ADI_I2S_RESULT_TYPE adi_I2S_SetChannel_2_OutputMap(ADI_I2S_DEV_HANDLE const hDevice, ADI_I2S_CHANNEL_MAP_TYPE const );
ADI_I2S_RESULT_TYPE adi_I2S_SetChannel_1_OutputMap(ADI_I2S_DEV_HANDLE const hDevice, ADI_I2S_CHANNEL_MAP_TYPE const );
ADI_I2S_RESULT_TYPE adi_I2S_SetDmaMode            (ADI_I2S_DEV_HANDLE const hDevice, const bool_t bFlag);
ADI_I2S_RESULT_TYPE adi_I2S_SetBlockingMode       (ADI_I2S_DEV_HANDLE const hDevice, const bool_t bFlag);


ADI_I2S_RESULT_TYPE adi_I2S_SetFifoReset          (ADI_I2S_DEV_HANDLE const hDevice,   ADI_I2S_FIFO_RESET_TYPE const );
ADI_I2S_RESULT_TYPE adi_I2S_SetIncrReset          (ADI_I2S_DEV_HANDLE const hDevice,  ADI_I2S_INCR_RESET_TYPE const);
ADI_I2S_RESULT_TYPE adi_I2S_SetReqEnable          (ADI_I2S_DEV_HANDLE const hDevice,  const bool_t bFlag);
ADI_I2S_RESULT_TYPE adi_I2S_SetStatEnable         (ADI_I2S_DEV_HANDLE const hDevice,  const bool_t bFlag);
ADI_I2S_RESULT_TYPE adi_I2S_SetAutoIncrEnable     (ADI_I2S_DEV_HANDLE const hDevice,  const bool_t bFlag);
ADI_I2S_RESULT_TYPE adi_I2S_SetTranMode           (ADI_I2S_DEV_HANDLE const hDevice,  ADI_I2S_TRAN_MODE_TYPE const);
ADI_I2S_RESULT_TYPE adi_I2S_SetChanSel_2_Enable   (ADI_I2S_DEV_HANDLE const hDevice,  const bool_t bFlag);
ADI_I2S_RESULT_TYPE adi_I2S_SetChanSel_1_Enable   (ADI_I2S_DEV_HANDLE const hDevice,  const bool_t bFlag);
ADI_I2S_RESULT_TYPE adi_I2S_Enable                (ADI_I2S_DEV_HANDLE const hDevice,  const bool_t bFlag );
ADI_I2S_RESULT_TYPE adi_I2S_RegisterCallback      (ADI_I2S_DEV_HANDLE const hDevice,  ADI_CALLBACK const pfCallback, void *const pCBParam );


// Query APIs
ADI_I2S_RESULT_TYPE adi_I2S_GetNonBlockingStatus  (ADI_I2S_DEV_HANDLE const hDevice, uint16_t * const pBytesRemaining);
bool_t              adi_I2S_GetDmaCompletion      (ADI_I2S_DEV_HANDLE const hDevice);

// Transaction APIs
ADI_I2S_RESULT_TYPE adi_I2S_Transmit              (ADI_I2S_DEV_HANDLE const hDevice,
                                                              uint8_t      *pBuffer,
                                                             uint16_t const nBufferSize);





/* C++ linkage */
#ifdef __cplusplus
}
#endif

#endif /* include guard */

/*
** EOF
*/

/*@}*/
