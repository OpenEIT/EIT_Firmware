/*********************************************************************************

Copyright (c) 2010-2014 Analog Devices, Inc. All Rights Reserved.

This software is proprietary to Analog Devices, Inc. and its licensors.  By using 
this software you agree to the terms of the associated Analog Devices Software 
License Agreement (See /LICENSES/ADI.LABLAB.LICENSE). 

Modified from the Analog Devices original.
Portions Copyright (c) 2018 Andrew Grosser, Moritz von Buttlar.

*********************************************************************************/

/*!
 *****************************************************************************
 @file:    gpt.h
 @brief:   Gptimer Device Definitions for ADuCxxx
 @version: $Revision: 28525 $
 @date:    $Date: 2014-11-12 14:51:26 -0500 (Wed, 12 Nov 2014) $
*****************************************************************************/

/*! \addtogroup GPT_Driver GPT Driver
 *  General Purpose Timer Device Driver
 *  @{
 */

#ifndef __GPT_H__
#define __GPT_H__

#include "device.h"
#include "adi_int.h"
#include "config/adi_gpt_config.h"

/* C++ linkage */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/*================ D A T A   T Y P E S ================*/


/*!
    Gptimer API return codes
*/
typedef enum
{
    ADI_GPT_SUCCESS = 0,                /*!< No Error */

    ADI_GPT_ERR_UNKNOWN_ERROR           /*!< An unknown error was detected */
      = ADI_DEV_GPT_ERROR_OFFSET,
    ADI_GPT_ERR_ALREADY_INITIALIZED,    /*!< Gptimer is already initialized */
    ADI_GPT_ERR_NOT_INITIALIZED,        /*!< Gptimer not yet initialized */
    ADI_GPT_ERR_BAD_INSTANCE,           /*!< Asking to initialize an unknown instance */
    ADI_GPT_ERR_PARAM_OUT_OF_RANGE,     /*!< Parameter is out of range.          */
    ADI_GPT_ERR_BAD_DEV_HANDLE,         /*!< Invalid device handle passed */
} ADI_GPT_RESULT_TYPE;


/*!
 * GP Timer Prescaler Definitions used to scale down the clock source.
 */
typedef enum
{
    ADI_GPT_PRESCALER_1         = TCON_PRE_DIV1,       /*!< Count up/down every source clock */
    ADI_GPT_PRESCALER_16        = TCON_PRE_DIV16,      /*!< Count up/down every 16 source clocks */
    ADI_GPT_PRESCALER_256       = TCON_PRE_DIV256,     /*!< Count up/down every 256 source clocks */
    ADI_GPT_PRESCALER_32768     = TCON_PRE_DIV32768,   /*!< Count up/down every 32768 source clocks */

} ADI_GPT_PRESCALER_TYPE;


/*!
 *  GP Timer Clock Select Definitions. The GP Timers can be clocked from a variety of sources.
 */
typedef enum
{
    ADI_GPT_CLOCK_SELECT_PCLK                  = TCON_CLK_PCLK,    /*!< Use internal system clock */
    ADI_GPT_CLOCK_SELECT_HFOSC                 = TCON_CLK_HFOSC,   /*!< Use internal 16 MHz oscillator */
    ADI_GPT_CLOCK_SELECT_32KHZ_INTERNAL_CLOCK  = TCON_CLK_LFOSC,   /*!< Use internal 32 kHz oscillator */
    ADI_GPT_CLOCK_SELECT_EXTERNAL_CLOCK        = TCON_CLK_LFXTAL,  /*!< Use external 32 kHz crystal */
} ADI_GPT_CLOCK_SELECT_TYPE;


/*!
 * GP Timer Capture Event Definitions
 *
 * There are 30 interrupt events that can be captured by the general purpose timers.
 * These events are divided into two groups of 16 inputs for each of the timers as
 * shown below. Any one of the 16 events associated with a general purpose timer
 * can cause a capture of the 16 bit TxVAL register in to the 16 bit TxCAP register.
 */
typedef enum
{
    ADI_GPT_CAPTURE_EVENT_NONE                        = -1,                     /*!< No event */

    /* GPT0 EVENTS */
    ADI_GPT_0_CAPTURE_EVENT_WAKEUP_TIMER              = T0CON_EVENT_WUT,        /*!< Wakeup Timer triggered event capture */
    ADI_GPT_0_CAPTURE_EVENT_EXTERNAL_INT_0            = T0CON_EVENT_EXT0,       /*!< Implemenation specific externally triggered event capture */
    ADI_GPT_0_CAPTURE_EVENT_EXTERNAL_INT_1            = T0CON_EVENT_EXT1,       /*!< Implemenation specific externally triggered event capture */
    ADI_GPT_0_CAPTURE_EVENT_EXTERNAL_INT_2            = T0CON_EVENT_EXT2,       /*!< Implemenation specific externally triggered event capture */
    ADI_GPT_0_CAPTURE_EVENT_EXTERNAL_INT_3            = T0CON_EVENT_EXT3,       /*!< Implemenation specific externally triggered event capture */
    ADI_GPT_0_CAPTURE_EVENT_EXTERNAL_INT_4            = T0CON_EVENT_EXT4,       /*!< Implemenation specific externally triggered event capture */
    ADI_GPT_0_CAPTURE_EVENT_EXTERNAL_INT_5            = T0CON_EVENT_EXT5,       /*!< Implemenation specific externally triggered event capture */
    ADI_GPT_0_CAPTURE_EVENT_EXTERNAL_INT_6            = T0CON_EVENT_EXT6,       /*!< Implemenation specific externally triggered event capture */
    ADI_GPT_0_CAPTURE_EVENT_EXTERNAL_INT_7            = T0CON_EVENT_EXT7,       /*!< Implemenation specific externally triggered event capture */
    ADI_GPT_0_CAPTURE_EVENT_EXTERNAL_INT_8            = T0CON_EVENT_EXT8,       /*!< Implemenation specific externally triggered event capture */
    ADI_GPT_0_CAPTURE_EVENT_AFE                       = T0CON_EVENT_AFE,        /*!< AFE triggered event capture */
    ADI_GPT_0_CAPTURE_EVENT_CAPTOUCH                  = T0CON_EVENT_CT,         /*!< CapTouch triggered event capture */
    ADI_GPT_0_CAPTURE_EVENT_WATCHDOG_TIMER            = T0CON_EVENT_WDT,        /*!< Watch Dog Timer triggered event capture */
    ADI_GPT_0_CAPTURE_EVENT_FLASH_CONTROLLER          = T0CON_EVENT_FEE,        /*!< Flash Controller event capture */
    ADI_GPT_0_CAPTURE_EVENT_TIMER1                    = T0CON_EVENT_GPT1,       /*!< GP Timer1 triggered event capture */
    ADI_GPT_0_CAPTURE_EVENT_TIMER2                    = T0CON_EVENT_GPT2,       /*!< GP Timer2 triggered event capture */

    /* GPT1 EVENTS */
    ADI_GPT_1_CAPTURE_EVENT_UART                      = T1CON_EVENT_COM,        /*!< UART triggered event capture */
    ADI_GPT_1_CAPTURE_EVENT_SPI0                      = T1CON_EVENT_SPI0,       /*!< SPI0 triggered event capture */
    ADI_GPT_1_CAPTURE_EVENT_SPI1                      = T1CON_EVENT_SPI1,       /*!< SPI1 triggered event capture */
    ADI_GPT_1_CAPTURE_EVENT_SPIH                      = T1CON_EVENT_SPIH,       /*!< High-speed SPI triggered event capture */
    ADI_GPT_1_CAPTURE_EVENT_I2C_SLAVE                 = T1CON_EVENT_I2CS,       /*!< I2C Slave triggered event capture */
    ADI_GPT_1_CAPTURE_EVENT_I2C_MASTER                = T1CON_EVENT_I2CM,       /*!< I2C Master triggered event capture */
    ADI_GPT_1_CAPTURE_EVENT_I2S                       = T1CON_EVENT_I2S,        /*!< I2S triggered event capture */
    ADI_GPT_1_CAPTURE_EVENT_USB                       = T1CON_EVENT_USB,        /*!< USB triggered event capture */
    ADI_GPT_1_CAPTURE_EVENT_USB_DMA                   = T1CON_EVENT_USBDMA,     /*!< USB DMA triggered event capture */
    ADI_GPT_1_CAPTURE_EVENT_CRC                       = T1CON_EVENT_CRC,        /*!< CRC triggered event capture */
    ADI_GPT_1_CAPTURE_EVENT_BEEPER                    = T1CON_EVENT_BEEP,       /*!< BEEPER triggered event capture */
    ADI_GPT_1_CAPTURE_EVENT_DMA_ERROR                 = T1CON_EVENT_DMAERR,     /*!< DMA Error triggered event capture */
    ADI_GPT_1_CAPTURE_EVENT_DMA_DONE                  = T1CON_EVENT_DMADONE,    /*!< DMA Done triggered event capture */
    ADI_GPT_1_CAPTURE_EVENT_GPFLASH_CONTROLLER        = T1CON_EVENT_GPFEE,      /*!< GP Flash Controller event capture */
    ADI_GPT_1_CAPTURE_EVENT_TIMER0                    = T1CON_EVENT_GPT0,       /*!< GP Timer triggered event capture */
    ADI_GPT_1_CAPTURE_EVENT_TIMER2                    = T1CON_EVENT_GPT2,       /*!< GP Timer triggered event capture */

    /* GPT2 EVENTS */
    ADI_GPT_2_CAPTURE_EVENT_EXTERNAL_INT_0            = T2CON_EVENT_EXT0,       /*!< Implemenation specific externally triggered event capture */
    ADI_GPT_2_CAPTURE_EVENT_EXTERNAL_INT_1            = T2CON_EVENT_EXT1,       /*!< Implemenation specific externally triggered event capture */
    ADI_GPT_2_CAPTURE_EVENT_EXTERNAL_INT_2            = T2CON_EVENT_EXT2,       /*!< Implemenation specific externally triggered event capture */
    ADI_GPT_2_CAPTURE_EVENT_EXTERNAL_INT_3            = T2CON_EVENT_EXT3,       /*!< Implemenation specific externally triggered event capture */
    ADI_GPT_2_CAPTURE_EVENT_EXTERNAL_INT_4            = T2CON_EVENT_EXT4,       /*!< Implemenation specific externally triggered event capture */
    ADI_GPT_2_CAPTURE_EVENT_EXTERNAL_INT_5            = T2CON_EVENT_EXT5,       /*!< Implemenation specific externally triggered event capture */
    ADI_GPT_2_CAPTURE_EVENT_EXTERNAL_INT_6            = T2CON_EVENT_EXT6,       /*!< Implemenation specific externally triggered event capture */
    ADI_GPT_2_CAPTURE_EVENT_EXTERNAL_INT_7            = T2CON_EVENT_EXT7,       /*!< Implemenation specific externally triggered event capture */
    ADI_GPT_2_CAPTURE_EVENT_EXTERNAL_INT_8            = T2CON_EVENT_EXT8,       /*!< Implemenation specific externally triggered event capture */
    ADI_GPT_2_CAPTURE_EVENT_AFE                       = T2CON_EVENT_AFE,        /*!< AFE triggered event capture */
    ADI_GPT_2_CAPTURE_EVENT_CAPTOUCH                  = T2CON_EVENT_CT,         /*!< CapTouch triggered event capture */
    ADI_GPT_2_CAPTURE_EVENT_REALTIME_CLOCK            = T2CON_EVENT_RTC,        /*!< RTC triggered event capture */
    ADI_GPT_2_CAPTURE_EVENT_TIMER0                    = T2CON_EVENT_GPT0,       /*!< GP Timer triggered event capture */
    ADI_GPT_2_CAPTURE_EVENT_TIMER1                    = T2CON_EVENT_GPT1,       /*!< GP Timer triggered event capture */

} ADI_GPT_CAPTURE_EVENTS_TYPE;


/*!
  @brief        GP Timer callback events.
  @details      When a callback is registered to get notification about the time out
                and event capture, the application supplied callback will be called
                with one of the following events.
*/
typedef enum
{
    /*! The timer has expired. */
    ADI_GPT_EVENT_TIMEOUT     = BITM_GPT_GPTSTA_TMOUT,

    /*!< The given timer event is captured. Application should use the API
         #adi_GPT_GetCapturedValue to get the timer value when the event is
         captured. */
    ADI_GPT_EVENT_CAPTURED    = BITM_GPT_GPTSTA_CAP

} ADI_GPT_EVENT_TYPE;

/*!
    @brief  GP Timer couting modes.
    @details GP Timers can be set to count up or to count down.
*/
typedef enum
{
  ADI_GPT_COUNT_DOWN                                  = 0,                 /*!< GP Timer count down mode */
  ADI_GPT_COUNT_UP                                    = TCON_UP            /*!< GP Timer count up mode */

} ADI_GPT_COUNT_MODE_TYPE;


/*!
    @brief  Gptimer Device IDs.
    @details List of all GP Timer Device IDs for the ADuCxxx part
*/
typedef enum
{
    ADI_GPT_DEVID_0 = 0,       /*!< GP Timer Device 0 */
    ADI_GPT_DEVID_1,           /*!< GP Timer Device 1 */
    ADI_GPT_DEVID_2,           /*!< GP Timer Device 2 */
    ADI_GPT_MAX_DEVID          /*!< max number of GPT */
} ADI_GPT_DEV_ID_TYPE;


/*!
    @brief  Gptimer Handle.
    @details The GP Timer Handle declaration. The handle is used in all subsequent
             GP Timer API calls.

    @note    May require a MISRA suppression about incomplete types (rule 18.1)
*/
typedef struct ADI_GPT_DEV_DATA_TYPE * ADI_GPT_HANDLE;


/*================ E X T E R N A L S ==================*/


/*
 * GP Timer API
 */

/* Device Open and Close Interface **/
extern ADI_GPT_RESULT_TYPE adi_GPT_Init                        (ADI_GPT_DEV_ID_TYPE, ADI_GPT_HANDLE* const phDevice);
extern ADI_GPT_RESULT_TYPE adi_GPT_UnInit                      (ADI_GPT_HANDLE const);


/* Timer Interrupt Related */
extern ADI_GPT_RESULT_TYPE adi_GPT_ClearCapturedEventStatus    (ADI_GPT_HANDLE);
extern ADI_GPT_RESULT_TYPE adi_GPT_GetCapturedValue            (ADI_GPT_HANDLE, uint16_t *pCapturedValue);
extern ADI_GPT_RESULT_TYPE adi_GPT_ClearTimeoutInterrupt       (ADI_GPT_HANDLE);


/* Timer Status */
extern bool_t adi_GPT_GetTimerBusy                             (ADI_GPT_HANDLE);
extern bool_t adi_GPT_GetCaptureEventPending                   (ADI_GPT_HANDLE);
extern bool_t adi_GPT_GetTimeOutEventStatus                    (ADI_GPT_HANDLE);
extern bool_t adi_GPT_GetTimerConfigValid                      (ADI_GPT_HANDLE);


/* Timer Load Register (TxLD). Used to reload the Timer Value Register (TxVal) */
extern ADI_GPT_RESULT_TYPE adi_GPT_SetLdVal                    (ADI_GPT_HANDLE, const uint16_t TimerLdVal);
extern ADI_GPT_RESULT_TYPE adi_GPT_GetLdVal                    (ADI_GPT_HANDLE,  uint16_t *pTimerLdVal);


/* Timer Value Register (TxVal) */
extern ADI_GPT_RESULT_TYPE adi_GPT_GetTxVal                    (ADI_GPT_HANDLE, uint16_t *pTimerValue);


/* Timer Configuration */
extern ADI_GPT_RESULT_TYPE adi_GPT_SetEventToCapture           (ADI_GPT_HANDLE, ADI_GPT_CAPTURE_EVENTS_TYPE Event);
extern ADI_GPT_RESULT_TYPE adi_GPT_SetCaptureEventEnable       (ADI_GPT_HANDLE, bool_t bEnable);
extern ADI_GPT_RESULT_TYPE adi_GPT_SetPrescaler                (ADI_GPT_HANDLE, ADI_GPT_PRESCALER_TYPE);
extern ADI_GPT_RESULT_TYPE adi_GPT_SetTimerEnable              (ADI_GPT_HANDLE, bool_t bEnable);
extern ADI_GPT_RESULT_TYPE adi_GPT_SetPeriodicMode             (ADI_GPT_HANDLE , bool_t bMod, uint16_t reloadValue);
extern ADI_GPT_RESULT_TYPE adi_GPT_SetFreeRunningMode          (ADI_GPT_HANDLE);
extern ADI_GPT_RESULT_TYPE adi_GPT_SetCountMode                (ADI_GPT_HANDLE, ADI_GPT_COUNT_MODE_TYPE Mode);
extern ADI_GPT_RESULT_TYPE adi_GPT_SetClockSelect              (ADI_GPT_HANDLE, ADI_GPT_CLOCK_SELECT_TYPE ClockSelect);

extern ADI_GPT_RESULT_TYPE adi_GPT_ResetTimer                  (ADI_GPT_HANDLE);

/* Timer Callback registration */
extern ADI_GPT_RESULT_TYPE adi_GPT_RegisterCallback            (ADI_GPT_HANDLE, ADI_CALLBACK pfCallback, void *pCBParam);

/* C++ linkage */
#ifdef __cplusplus
}
#endif

#endif /* include guard */

/*
** EOF
*/

/*@}*/
