
/*! \addtogroup AFE_Library AFE Library
 *  @{
 */

#ifndef __AFE_H__
#define __AFE_H__

#include "device.h"

#include "dma.h"
#include "uart.h"
#include "adi_int.h"
#include "config/adi_afe_config.h"

/* C++ linkage */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
/*!
 *****************************************************************************
 * \enum ADI_AFE_RESULT_TYPE
 *
 * AFE error codes.
 *
 *****************************************************************************/

typedef enum {
    ADI_AFE_SUCCESS                     = 0,                                /*!< Generic success.                                                                   */
    ADI_AFE_ERR_UNKNOWN                 = ADI_DEV_AFE_ERROR_OFFSET,         /*!< Generic error.                                                                     */
    ADI_AFE_ERR_BAD_DEV_HANDLE          = ADI_DEV_AFE_ERROR_OFFSET +  1,    /*!< Invalid device handle.                                                             */
    ADI_AFE_ERR_NOT_INITIALIZED         = ADI_DEV_AFE_ERROR_OFFSET +  2,    /*!< Device not initialized.                                                            */
    ADI_AFE_ERR_ALREADY_INITIALIZED     = ADI_DEV_AFE_ERROR_OFFSET +  3,    /*!< Device already initialized.                                                        */
    ADI_AFE_ERR_ACLKOFF                 = ADI_DEV_AFE_ERROR_OFFSET +  4,    /*!< ACLK disabled from the clock gate.                                                 */
    ADI_AFE_ERR_WRONG_ACLK_FREQUENCY    = ADI_DEV_AFE_ERROR_OFFSET +  5,    /*!< Programmed ACLK frequency is not the required 16MHz.                               */
    ADI_AFE_ERR_SEQ                     = ADI_DEV_AFE_ERROR_OFFSET +  6,    /*!< Sequencer error.                                                                   */
    ADI_AFE_ERR_CMD_FIFO_UDF            = ADI_DEV_AFE_ERROR_OFFSET +  7,    /*!< Command FIFO underflow error.                                                      */
    ADI_AFE_ERR_CMD_FIFO_OVF            = ADI_DEV_AFE_ERROR_OFFSET +  8,    /*!< Command FIFO overflow error.                                                       */
    ADI_AFE_ERR_DATA_FIFO_UDF           = ADI_DEV_AFE_ERROR_OFFSET +  9,    /*!< Data FIFO underflow error.                                                         */
    ADI_AFE_ERR_DATA_FIFO_OVF           = ADI_DEV_AFE_ERROR_OFFSET + 10,    /*!< Data FIFO overflow error.                                                          */
    ADI_AFE_ERR_DMA                     = ADI_DEV_AFE_ERROR_OFFSET + 11,    /*!<                                                                                    */
    ADI_AFE_ERR_CRC                     = ADI_DEV_AFE_ERROR_OFFSET + 12,    /*!<                                                                                    */
    ADI_AFE_ERR_CALLBACK_OVERWRITE      = ADI_DEV_AFE_ERROR_OFFSET + 13,    /*!< Attempt to overwrite existing callback.                                            */
    ADI_AFE_ERR_SEQ_ALREADY_INITIALIZED = ADI_DEV_AFE_ERROR_OFFSET + 14,    /*!<                                                                                    */
    ADI_AFE_ERR_SEQ_NOT_DISABLED        = ADI_DEV_AFE_ERROR_OFFSET + 15,    /*!<                                                                                    */
    ADI_AFE_ERR_SEQ_CHECK               = ADI_DEV_AFE_ERROR_OFFSET + 16,    /*!<                                                                                    */
    ADI_AFE_ERR_SEMAPHORE_FAILED        = ADI_DEV_AFE_ERROR_OFFSET + 17,    /*!<                                                                                    */
    ADI_AFE_ERR_PARAM_OUT_OF_RANGE      = ADI_DEV_AFE_ERROR_OFFSET + 18,    /*!<                                                                                    */
    ADI_AFE_ERR_RX_DMA_SIZE_TOO_BIG     = ADI_DEV_AFE_ERROR_OFFSET + 19,    /*!<                                                                                    */
    ADI_AFE_ERR_SEQ_ABORT_REQUEST       = ADI_DEV_AFE_ERROR_OFFSET + 20,    /*!<                                                                                    */
    ADI_AFE_ERR_FCW_OUT_OF_RANGE        = ADI_DEV_AFE_ERROR_OFFSET + 21,    /*!< Waveform generator sinusoid frequency must be 80Hz - 80kHz                         */
    ADI_AFE_ERR_INVALID_IRQ             = ADI_DEV_AFE_ERROR_OFFSET + 22,    /*!< IRQ is not a valid member of the ADI_AFE_INT_GROUP_TYPE enum                       */
    ADI_AFE_ERR_INVALID_WG_MODE         = ADI_DEV_AFE_ERROR_OFFSET + 23,    /*!< Wavegen mode is not a valid member of the ADI_AFE_WAVEGEN_TYPE enum                */
    ADI_AFE_ERR_INVALID_DATA_FIFO_SRC   = ADI_DEV_AFE_ERROR_OFFSET + 24,    /*!< Data FIFO source is not a valid member of the ADI_AFE_DATA_FIFO_SOURCE_TYPE enum   */
} ADI_AFE_RESULT_TYPE;

/*!
 *****************************************************************************
 * \enum ADI_AFE_SEQ_STATE_TYPE
 *
 * AFE sequencer status codes.
 *
 *****************************************************************************/

typedef enum {
    ADI_AFE_SEQ_STATE_IDLE,
    ADI_AFE_SEQ_STATE_WAITING_FOR_CMD_FIFO,
    ADI_AFE_SEQ_STATE_INITIALIZED,
    ADI_AFE_SEQ_STATE_RUNNING,
    ADI_AFE_SEQ_STATE_FINISHED,
} ADI_AFE_SEQ_STATE_TYPE;

/*!
 *****************************************************************************
 * \enum ADI_AFE_DATA_FIFO_SOURCE_TYPE
 *
 * AFE data FIFO source select. Corresponds to field DATA_FIFO_SOURCE_SEL
 * in register AFE_FIFO_CFG.
 *
 *****************************************************************************/

typedef enum {
    ADI_AFE_DATA_FIFO_SOURCE_ADC = 0,
    ADI_AFE_DATA_FIFO_SOURCE_DFT = 2,
    ADI_AFE_DATA_FIFO_SOURCE_LPF = 3,
} ADI_AFE_DATA_FIFO_SOURCE_TYPE;

/*!
 *****************************************************************************
 * \enum ADI_AFE_WAVEGEN_TYPE
 *
 * AFE waveform type select. Corresponds to field TYPE_SEL
 * in register AFE_WG_CFG.
 *
 *****************************************************************************/
 
typedef enum {
    ADI_AFE_WAVEGEN_DIRECT = 0,
    ADI_AFE_WAVEGEN_SINUSOID = 2,
    ADI_AFE_WAVEGEN_TRAPEZOID = 3,
} ADI_AFE_WAVEGEN_TYPE;

/*!
 *****************************************************************************
 * \enum ADI_AFE_CAL_REG_TYPE
 *
 * Calibration registers.
 *
 *****************************************************************************/

typedef enum {
    ADI_AFE_CAL_REG_ADC_GAIN_TIA            = REG_AFE_AFE_ADC_GAIN_TIA,
    ADI_AFE_CAL_REG_ADC_OFFSET_TIA          = REG_AFE_AFE_ADC_OFFSET_TIA,
    ADI_AFE_CAL_REG_ADC_GAIN_TEMP_SENS      = REG_AFE_AFE_ADC_GAIN_TEMP_SENS,
    ADI_AFE_CAL_REG_ADC_OFFSET_TEMP_SENS    = REG_AFE_AFE_ADC_OFFSET_TEMP_SENS,
    ADI_AFE_CAL_REG_ADC_GAIN_AUX            = REG_AFE_AFE_ADC_GAIN_AUX,
    ADI_AFE_CAL_REG_ADC_OFFSET_AUX          = REG_AFE_AFE_ADC_OFFSET_AUX,
    ADI_AFE_CAL_REG_DAC_OFFSET_UNITY        = REG_AFE_AFE_DAC_OFFSET_UNITY,
    ADI_AFE_CAL_REG_DAC_OFFSET_ATTEN        = REG_AFE_AFE_DAC_OFFSET_ATTEN,
    ADI_AFE_CAL_REG_DAC_GAIN                = REG_AFE_AFE_DAC_GAIN,
    ADI_AFE_CAL_REG_REF_TRIM0               = REG_AFE_AFE_REF_TRIM0,
    ADI_AFE_CAL_REG_REF_TRIM1               = REG_AFE_AFE_REF_TRIM1,
    ADI_AFE_CAL_REG_ALDO_TRIM               = REG_AFE_AFE_ALDO_TRIM,
    ADI_AFE_CAL_REG_DAC_TRIM                = REG_AFE_AFE_DAC_TRIM,
    ADI_AFE_CAL_REG_INAMP_TRIM              = REG_AFE_AFE_INAMP_TRIM,
    ADI_AFE_CAL_REG_EXBUF_TRIM              = REG_AFE_AFE_EXBUF_TRIM,
    ADI_AFE_CAL_REG_TEMP_SENS_TRIM          = REG_AFE_AFE_TEMP_SENS_TRIM,
} ADI_AFE_CAL_REG_TYPE;

/*!
 *****************************************************************************
 * \enum ADI_AFE_INT_GROUP_TYPE
 *
 * Interrupt groups, each one corresponds to an AFE IRQ.
 *
 *****************************************************************************/
 
typedef enum {
    ADI_AFE_INT_GROUP_CAPTURE               = 1,
    ADI_AFE_INT_GROUP_GENERATE,
    ADI_AFE_INT_GROUP_CMD_FIFO,
    ADI_AFE_INT_GROUP_DATA_FIFO,
} ADI_AFE_INT_GROUP_TYPE;

/*!
 *****************************************************************************
 * \enum ADI_AFE_ADC_MUX_SEL_TYPE
 *
 * Select signals for the ADC input multiplexer. Corresponds to field MUX_SEL
 * in register AFE_ADC_CFG.
 *
 *****************************************************************************/
 
typedef enum {
    ADI_AFE_ADC_MUX_SEL_FLOATING                    = 0,
    ADI_AFE_ADC_MUX_SEL_TIA                         = 2,
    ADI_AFE_ADC_MUX_SEL_INTERNAL_TEMP_SENS          = 3,
    ADI_AFE_ADC_MUX_SEL_AN_A                        = 8,
    ADI_AFE_ADC_MUX_SEL_AN_B                        = 9,
    ADI_AFE_ADC_MUX_SEL_AN_C                        = 10,
    ADI_AFE_ADC_MUX_SEL_AN_D                        = 11,
    ADI_AFE_ADC_MUX_SEL_AN_VEXCITE                  = 12,
    ADI_AFE_ADC_MUX_SEL_HALF_DVDD_MINUS_VBIAS       = 13,
    ADI_AFE_ADC_MUX_SEL_HALF_AVDDTX_MINUS_VBIAS     = 14,
    ADI_AFE_ADC_MUX_SEL_HALF_AVDDRX_MINUS_VBIAS     = 15,
    ADI_AFE_ADC_MUX_SEL_HALF_VCCM_ANA_MINUS_VBIAS   = 16,
    ADI_AFE_ADC_MUX_SEL_HALF_VCCM_DIG_MINUS_VBIAS   = 17,
    ADI_AFE_ADC_MUX_SEL_HALF_VBACK_MINUS_VBIAS      = 18,
    ADI_AFE_ADC_MUX_SEL_CAL_ADC_OFFSET_TIA          = 24,
    ADI_AFE_ADC_MUX_SEL_CAL_ADC_GAIN_TIA_TEMP_SENS  = 25,
    ADI_AFE_ADC_MUX_SEL_CAL_ADC_OFFSET_TEMP_SENS    = 26,
    ADI_AFE_ADC_MUX_SEL_CAL_ADC_OFFSET_AUX          = 28,
    ADI_AFE_ADC_MUX_SEL_CAL_ADC_GAIN_AUX            = 29,
    ADI_AFE_ADC_MUX_SEL_CAL_P_N_AUTOCAL             = 30,
} ADI_AFE_ADC_MUX_SEL_TYPE;

/*!
 *****************************************************************************
 * \enum ADI_AFE_ADC_GAIN_OFFS_TYPE
 *
 * Gain and offset selection for ADC. Corresponds to field GAIN_OFFS_SEL
 * in register AFE_ADC_CFG.
 *
 *****************************************************************************/
 
typedef enum {
    ADI_AFE_ADC_GAIN_OFFS_TIA       = 0,
    ADI_AFE_ADC_GAIN_OFFS_TEMP_SENS = 1,
    ADI_AFE_ADC_GAIN_OFFS_AUX       = 2,
    ADI_AFE_ADC_GAIN_OFFS_BYPASS    = 3,
} ADI_AFE_ADC_GAIN_OFFS_TYPE;

typedef struct ADI_AFE_DEV_DATA_TYPE* ADI_AFE_DEV_HANDLE;  /*!< AFE Device instance private data handle typedef. */

/* Helper macro for adjusting MMR writes in a sequence, when modified from C code */
/* Parameters:                                                                    */
/*   - addr:   MMR full address (32-bit), as defined in defADuCM350.h             */
/*   - data:   value to be written to bits 24..0 in the MMR                       */
/*                                                                                */
/* Details:                                                                       */
/* - 0x80000000: sequencer command MSB set to "1", for an MMR write               */
/* - addr[7:2] identifies the MMR for the sequencer,   */
#define SEQ_MMR_WRITE(addr, data)   (0x80000000 | ((addr & 0xFC) << 23) | data)

extern ADI_AFE_RESULT_TYPE      adi_AFE_Init                            (ADI_AFE_DEV_HANDLE* const      phDevice);
extern ADI_AFE_RESULT_TYPE      adi_AFE_UnInit                          (ADI_AFE_DEV_HANDLE const       hDevice);
extern ADI_AFE_RESULT_TYPE      adi_AFE_SetDataFifoSource               (ADI_AFE_DEV_HANDLE const       hDevice,
                                                                         ADI_AFE_DATA_FIFO_SOURCE_TYPE  sel);
extern ADI_AFE_RESULT_TYPE      adi_AFE_SetWavegenType                  (ADI_AFE_DEV_HANDLE const       hDevice, 
                                                                         ADI_AFE_WAVEGEN_TYPE           sel);
extern ADI_AFE_RESULT_TYPE      adi_AFE_SetSwitchState                  (ADI_AFE_DEV_HANDLE const       hDevice,
                                                                         uint32_t                       state);

extern ADI_AFE_RESULT_TYPE      adi_AFE_ProgramTxDMA                    (ADI_AFE_DEV_HANDLE const       hDevice,
                                                                         const uint32_t *const          buffer,
                                                                         uint32_t length);
extern ADI_AFE_RESULT_TYPE      adi_AFE_ProgramRxDMA                    (ADI_AFE_DEV_HANDLE const       hDevice,
                                                                         uint16_t *const                buffer,
                                                                         uint32_t length);

extern ADI_AFE_RESULT_TYPE      adi_AFE_RegisterCallbackOnTransmitDMA   (ADI_AFE_DEV_HANDLE const       hDevice,
                                                                         ADI_CALLBACK                   cbFunc,
                                                                         uint32_t                       cbWatch);
extern ADI_AFE_RESULT_TYPE      adi_AFE_RegisterCallbackOnReceiveDMA    (ADI_AFE_DEV_HANDLE const       hDevice,
                                                                         ADI_CALLBACK const             cbFunc,
                                                                         uint32_t                       cbWatch);
extern ADI_AFE_RESULT_TYPE      adi_AFE_RegisterAfeCallback             (ADI_AFE_DEV_HANDLE             hDevice, 
                                                                         ADI_AFE_INT_GROUP_TYPE         group, 
                                                                         ADI_CALLBACK                   cbFunc, 
                                                                         uint32_t                       cbWatch);
#if (ADI_AFE_CFG_ENABLE_DEPRECATED_FUNCTION_SUPPORT == 1)
extern ADI_AFE_RESULT_TYPE      adi_AFE_RegisterCallbackOnCommandFIFO   (ADI_AFE_DEV_HANDLE const       hDevice,
                                                                         ADI_CALLBACK const             cbFunc,
                                                                         uint32_t                       cbWatch);
extern ADI_AFE_RESULT_TYPE      adi_AFE_RegisterCallbackOnDataFIFO      (ADI_AFE_DEV_HANDLE const       hDevice,
                                                                         ADI_CALLBACK const             cbFunc,
                                                                         uint32_t                       cbWatch);
#endif /* (ADI_AFE_CFG_ENABLE_DEPRECATED_FUNCTION_SUPPORT) */
extern ADI_AFE_RESULT_TYPE      adi_AFE_WriteCalibrationRegister        (ADI_AFE_DEV_HANDLE const       hDevice,
                                                                         ADI_AFE_CAL_REG_TYPE           Reg,
                                                                         uint32_t                       Val);
extern ADI_AFE_RESULT_TYPE      adi_AFE_ReadCalibrationRegister         (ADI_AFE_DEV_HANDLE const       hDevice,
                                                                         ADI_AFE_CAL_REG_TYPE           Reg,
                                                                         uint32_t *const                pVal);
extern ADI_AFE_RESULT_TYPE      adi_AFE_ClearInterruptSource            (ADI_AFE_DEV_HANDLE const       hDevice, 
                                                                         ADI_AFE_INT_GROUP_TYPE         group, 
                                                                         uint32_t                       mask);
extern ADI_AFE_RESULT_TYPE      adi_AFE_EnableInterruptSource           (ADI_AFE_DEV_HANDLE const       hDevice, 
                                                                         ADI_AFE_INT_GROUP_TYPE         group, 
                                                                         uint32_t                       mask,
                                                                         bool_t                         bFlag);
extern ADI_AFE_RESULT_TYPE      adi_AFE_InvalidHandle                   (ADI_AFE_DEV_HANDLE const       hDevice);
extern ADI_AFE_RESULT_TYPE      adi_AFE_HandleNotInitialized            (ADI_AFE_DEV_HANDLE const       hDevice);
extern ADI_AFE_RESULT_TYPE      adi_AFE_SetRcal                         (ADI_AFE_DEV_HANDLE const       hDevice, 
                                                                         uint32_t                       rcal);
extern ADI_AFE_RESULT_TYPE      adi_AFE_GetRcal                         (ADI_AFE_DEV_HANDLE const       hDevice, 
                                                                         uint32_t*                      pRcal);
extern ADI_AFE_RESULT_TYPE      adi_AFE_SetRtia                         (ADI_AFE_DEV_HANDLE const       hDevice, 
                                                                         uint32_t                       rtia);
extern ADI_AFE_RESULT_TYPE      adi_AFE_GetRtia                         (ADI_AFE_DEV_HANDLE const       hDevice, 
                                                                         uint32_t*                      pRtia);
#if (ADI_AFE_CFG_ENABLE_RX_DMA_DUAL_BUFFER_SUPPORT == 1)      
extern ADI_AFE_RESULT_TYPE      adi_AFE_SetDmaRxBufferMaxSize           (ADI_AFE_DEV_HANDLE const       hDevice,
                                                                         uint16_t                       maxSizeFirst,
                                                                         uint16_t                       maxSizeSecond);
extern ADI_AFE_RESULT_TYPE      adi_AFE_GetDmaRxBufferMaxSize           (ADI_AFE_DEV_HANDLE const       hDevice,
                                                                         uint16_t *const                maxSizeFirst,
                                                                         uint16_t *const                maxSizeSecond);
#endif
                                 
/* Low level sequencer API functions */
extern ADI_AFE_RESULT_TYPE      adi_AFE_GetSeqError                     (ADI_AFE_DEV_HANDLE const       hDevice);
extern ADI_AFE_RESULT_TYPE      adi_AFE_SetSeqError                     (ADI_AFE_DEV_HANDLE const       hDevice, 
                                                                         ADI_AFE_RESULT_TYPE const      error);
extern ADI_AFE_RESULT_TYPE      adi_AFE_GetSeqState                     (ADI_AFE_DEV_HANDLE const       hDevice,
                                                                         ADI_AFE_SEQ_STATE_TYPE *const  pState);
extern ADI_AFE_RESULT_TYPE      adi_AFE_SetSeqState                     (ADI_AFE_DEV_HANDLE const       hDevice, 
                                                                         ADI_AFE_SEQ_STATE_TYPE         seqState);
extern ADI_AFE_RESULT_TYPE      adi_AFE_GetCmdFifoReady                 (ADI_AFE_DEV_HANDLE const       hDevice,
                                                                         bool_t *const                  pbFlag);
extern ADI_AFE_RESULT_TYPE      adi_AFE_GetSeqFinished                  (ADI_AFE_DEV_HANDLE const       hDevice,
                                                                         bool_t *const                  pbFlag);
extern ADI_AFE_RESULT_TYPE      adi_AFE_GetRxDmaComplete                (ADI_AFE_DEV_HANDLE const       hDevice,
                                                                         bool_t *const                  pbFlag);
extern ADI_AFE_RESULT_TYPE      adi_AFE_SetRunSequenceBlockingMode      (ADI_AFE_DEV_HANDLE const       hDevice, 
                                                                         const bool_t                   bFlag);
extern ADI_AFE_RESULT_TYPE      adi_AFE_GetRunSequenceBlockingMode      (ADI_AFE_DEV_HANDLE const       hDevice,
                                                                         bool_t *const                  pbFlag);
extern ADI_AFE_RESULT_TYPE      adi_AFE_SeqInit                         (ADI_AFE_DEV_HANDLE const       hDevice,
                                                                         const uint32_t *const          txBuffer,
                                                                         uint16_t *const                rxBuffer,
                                                                         uint32_t                       size);
extern ADI_AFE_RESULT_TYPE      adi_AFE_SeqStart                        (ADI_AFE_DEV_HANDLE const       hDevice);
extern ADI_AFE_RESULT_TYPE      adi_AFE_SeqStop                         (ADI_AFE_DEV_HANDLE const       hDevice);
extern ADI_AFE_RESULT_TYPE      adi_AFE_SeqAbort                        (ADI_AFE_DEV_HANDLE const       hDevice);
extern ADI_AFE_RESULT_TYPE      adi_AFE_SeqCheck                        (ADI_AFE_DEV_HANDLE const       hDevice,
                                                                         const uint32_t *const          txBuffer);
extern ADI_AFE_RESULT_TYPE      adi_AFE_RunSequence                     (ADI_AFE_DEV_HANDLE const       hDevice,
                                                                         const uint32_t *const          txBuffer,
                                                                         uint16_t *const                rxBuffer,
                                                                         uint32_t                       size);
extern ADI_AFE_RESULT_TYPE      adi_AFE_EnableSoftwareCRC               (ADI_AFE_DEV_HANDLE const       hDevice,
                                                                         const bool_t                   bEnable);
extern ADI_AFE_RESULT_TYPE      adi_AFE_GetLowPowerModeFlag             (ADI_AFE_DEV_HANDLE const       hDevice, 
                                                                         bool_t *const                  pbFlag);
extern ADI_AFE_RESULT_TYPE      adi_AFE_SetLowPowerModeFlag             (ADI_AFE_DEV_HANDLE const       hDevice, 
                                                                         bool_t                         *pbFlag);

/* C++ linkage */
#ifdef __cplusplus
}
#endif

#endif /* include guard */

/*
** EOF
*/

/*@}*/
