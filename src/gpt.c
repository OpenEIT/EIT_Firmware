/*********************************************************************************

Copyright (c) 2010-2014 Analog Devices, Inc. All Rights Reserved.

This software is proprietary to Analog Devices, Inc. and its licensors.  By using 
this software you agree to the terms of the associated Analog Devices Software 
License Agreement.

*********************************************************************************/

/*!
 *****************************************************************************
 * @file:    gpt.c
 * @brief:   GP Timer Device Implementations for ADuCxxx
 * @version: $Revision: 28723 $
 * @date:    $Date: 2014-11-20 10:31:38 -0500 (Thu, 20 Nov 2014) $
*****************************************************************************/

/*! \addtogroup GPT_Driver GPT Driver
 *  @{
 */

/*! \cond PRIVATE */

#define ASSERT_ADI_MISRA_SUPPRESSIONS  /*!< Apply ADI MISRA Suppressions */
#include "misra.h"

#include <stdlib.h>              /* for 'NULL" definition */

#include "gpt.h"

/* ADuCM350 is first to use CODA-generated macros and device mappings. */
/* define file-local-scope aliases to map old-style names to CODA-style */
#define LD GPTLD        /*!< local name mapping macro */
#define VAL GPTVAL      /*!< local name mapping macro */
#define CON GPTCON      /*!< local name mapping macro */
#define CLRI GPTCLRI    /*!< local name mapping macro */
#define CAP GPTCAP      /*!< local name mapping macro */
#define STA GPTSTA      /*!< local name mapping macro */


/* forward declarations... */
/* These are needed in the case where ADI_CFG_ENABLE_RTOS_SUPPORT=1              */
/* The ADI_INSTALL_HANDLER macro will expand to a call to adi_int_InstallHandler */
/* and the handler will be referenced                                            */
/* The non-OSAL expansion of ADI_INSTALL_HANDLER results in a NULL string        */
ADI_INT_HANDLER(GP_Tmr0_Int_Handler);
ADI_INT_HANDLER(GP_Tmr1_Int_Handler);
ADI_INT_HANDLER(GP_Tmr2_Int_Handler);


/*!
 * GPT Device instance data structure */
typedef struct ADI_GPT_DEV_DATA_TYPE
{
    bool_t               bInitialized;           /*!< Object's initialization state */
    IRQn_Type            TimerIntID;             /*!< Timer interrupt ID */
    ADI_CALLBACK         pfCallback;             /*!< Pointer to callback function */
    void                *pCBParam;               /*!< Callback prameter */
    ADI_TIMER_TypeDef   *pTmr;                   /*!< MMR address for this timer */
} ADI_GPT_DEV_DATA_TYPE;

/* allocate instance data */
ADI_GPT_DEV_DATA_TYPE gGPT_DevData[ADI_GPT_MAX_DEVID] = {0};  /*!< Private GPT device driver instance data */


/*! GPT configuration structure used to hold the application configuration.
*/
typedef struct
{
    uint16_t    GPTCON;             /*!< GPT control register value */
    uint16_t    GPTLD;              /*!< 16 bit load value */
    uint16_t    GPTALD;             /*!< GPT Ansynchrounous load register value */
    uint16_t    GPTPCON;            /*!< GPT PWM control register value */
    uint16_t    GPT_MATCH_VAL;      /*!< GPT PWM match value */

}ADI_GPT_CONFIG;

const ADI_GPT_CONFIG GPTConfig[ADI_GPT_MAX_DEVID] =
{
    /* Instance 0 configuration */
    {
        /* GPTCON register value */
       (( GPT0_CFG_PRESCALE_FACTOR              <<  BITP_GPT_GPTCON_PRE         ) |
        ( GPT0_CFG_COUNT_UP                     <<  BITP_GPT_GPTCON_UP          ) |
        ( GPT0_CFG_MODE                         <<  BITP_GPT_GPTCON_MOD         ) |
        ( GPT0_CFG_CLOCK_SOURCE                 <<  BITP_GPT_GPTCON_CLK         ) |
        ( GPT0_CFG_RELOAD_CONTROL               <<  BITP_GPT_GPTCON_RLD         ) |
        ( GPT0_CFG_EVENT_CAPTURE                <<  BITP_GPT_GPTCON_EVENT       ) |
        ( GPT0_CFG_ENABLE_EVENT_CAPTURE         <<  BITP_GPT_GPTCON_EVENTEN     )),

        /* GPT load register value */
        ( GPT0_CFG_LOAD_VALUE                                                   ),

        /* GPT asynchrounous load register value */
        ( GPT0_CFG_ASYNC_LOAD_VALUE                                             ),

        /* GPT PWM control register value */
       (( GPT0_CFG_ENABLE_PWM_MATCH             << BITP_GPT_GPTPCON_MATCH_EN    ) |
        ( GPT0_CFG_PWM_IDLE_STATE               << BITP_GPT_GPTPCON_IDLE_STATE  )),

        /* PWM Match value */
        ( GPT0_CFG_PWM_MATCH_VALUE                                               )
    },
    /* Instance 1 configuration */
    {
        /* GPTCON register value */
       (( GPT1_CFG_PRESCALE_FACTOR              <<  BITP_GPT_GPTCON_PRE         ) |
        ( GPT1_CFG_COUNT_UP                     <<  BITP_GPT_GPTCON_UP          ) |
        ( GPT1_CFG_MODE                         <<  BITP_GPT_GPTCON_MOD         ) |
        ( GPT1_CFG_CLOCK_SOURCE                 <<  BITP_GPT_GPTCON_CLK         ) |
        ( GPT1_CFG_RELOAD_CONTROL               <<  BITP_GPT_GPTCON_RLD         ) |
        ( GPT1_CFG_EVENT_CAPTURE                <<  BITP_GPT_GPTCON_EVENT       ) |
        ( GPT1_CFG_ENABLE_EVENT_CAPTURE         <<  BITP_GPT_GPTCON_EVENTEN     )),

        /* GPT load register value */
        ( GPT1_CFG_LOAD_VALUE                                                   ),

        /* GPT asynchrounous load register value */
        ( GPT1_CFG_ASYNC_LOAD_VALUE                                             ),

        /* GPT PWM control register value */
       (( GPT1_CFG_ENABLE_PWM_MATCH             << BITP_GPT_GPTPCON_MATCH_EN    ) |
        ( GPT1_CFG_PWM_IDLE_STATE               << BITP_GPT_GPTPCON_IDLE_STATE  )),

        /* PWM Match value */
        ( GPT1_CFG_PWM_MATCH_VALUE                                               )
    },

    /* Instance 2 configuration */
    {
        /* GPTCON register value */
       (( GPT2_CFG_PRESCALE_FACTOR              <<  BITP_GPT_GPTCON_PRE         ) |
        ( GPT2_CFG_COUNT_UP                     <<  BITP_GPT_GPTCON_UP          ) |
        ( GPT2_CFG_MODE                         <<  BITP_GPT_GPTCON_MOD         ) |
        ( GPT2_CFG_CLOCK_SOURCE                 <<  BITP_GPT_GPTCON_CLK         ) |
        ( GPT2_CFG_RELOAD_CONTROL               <<  BITP_GPT_GPTCON_RLD         ) |
        ( GPT2_CFG_EVENT_CAPTURE                <<  BITP_GPT_GPTCON_EVENT       ) |
        ( GPT2_CFG_ENABLE_EVENT_CAPTURE         <<  BITP_GPT_GPTCON_EVENTEN     )),

        /* GPT load register value */
        ( GPT2_CFG_LOAD_VALUE                                                   ),

        /* GPT asynchrounous load register value */
        ( GPT2_CFG_ASYNC_LOAD_VALUE                                             ),

        /* GPT PWM control register value */
       (( GPT2_CFG_ENABLE_PWM_MATCH             << BITP_GPT_GPTPCON_MATCH_EN    ) |
        ( GPT2_CFG_PWM_IDLE_STATE               << BITP_GPT_GPTPCON_IDLE_STATE  )),

        /* PWM Match value */
        ( GPT2_CFG_PWM_MATCH_VALUE                                               )
    }
};


/* debug validation of device handles */
#ifdef ADI_DEBUG
#define ADI_GPT_INVALID_HANDLE(h)((&gGPT_DevData[0] != h) && (&gGPT_DevData[1] != h) && (&gGPT_DevData[2] != h))
#endif

/*! \endcond */

/*!
    @brief GP Timer Initialization

    @param[in] InstanceNum  ADI_GPT_DEV_ID_TYPE specifying the instance of the GPTimer to use
    @param[in] phDevice     Pointer to a location that the device handle will
                            be written to upon successful initialization
    @return    Status
               - #ADI_GPT_SUCCESS if successfully initialized
               - #ADI_GPT_ERR_ALREADY_INITIALIZED [D] if GP timer already initialized
               - #ADI_GPT_ERR_BAD_INSTANCE [D] if instance number is invalid

    @note      1. The contents of hDevice will be set to NULL upon failure
               2. The timers, by default, will be initialized to
                  Not enabled
                  No capture events
                  Clock select is PCLK
                  Count Down Mode
                  Prescaler is 1

                  To override these default you must use the appropriate GP
                  timer configuration APIs.
*/
ADI_GPT_RESULT_TYPE adi_GPT_Init(ADI_GPT_DEV_ID_TYPE InstanceNum, ADI_GPT_HANDLE*  phDevice)
{
    ADI_GPT_HANDLE hDevice;

    /* store a bad handle in case of failure */
    *phDevice = (ADI_GPT_HANDLE) NULL;

#ifdef ADI_DEBUG
    if (InstanceNum >= ADI_GPT_MAX_DEVID)
    {
        return ADI_GPT_ERR_BAD_INSTANCE;
    }
#endif

    /* local pointer to instance data */
    hDevice = &gGPT_DevData[InstanceNum];

#ifdef ADI_DEBUG
    if (hDevice->bInitialized)
    {
        return ADI_GPT_ERR_ALREADY_INITIALIZED;
    }
#endif

    /* initialize device instance data and install handlers */
    switch (InstanceNum) {
        case ADI_GPT_DEVID_0:
          hDevice->pTmr = pADI_TM0;
          hDevice->TimerIntID = (IRQn_Type)ADI_INT_TIMER0;
          ADI_INSTALL_HANDLER(ADI_INT_TIMER0, GP_Tmr0_Int_Handler);
          break;

        case ADI_GPT_DEVID_1:
          hDevice->pTmr = pADI_TM1;
          hDevice->TimerIntID = (IRQn_Type)ADI_INT_TIMER1;
          ADI_INSTALL_HANDLER(ADI_INT_TIMER1, GP_Tmr1_Int_Handler);
          break;
        case ADI_GPT_DEVID_2:
          hDevice->pTmr = pADI_TM2;
          hDevice->TimerIntID = (IRQn_Type)ADI_INT_TIMER2;
          ADI_INSTALL_HANDLER(ADI_INT_TIMER2, GP_Tmr2_Int_Handler);
          break;
#ifdef ADI_DEBUG
        default: return ADI_GPT_ERR_BAD_INSTANCE;
#endif
    }

	/* do static register initializations */
	ADI_GPT_CONFIG const *pConfig = &GPTConfig[InstanceNum];

	/* Copy the register configuration values into the controller registers */
	hDevice->pTmr->GPTCON    =       pConfig->GPTCON;
	hDevice->pTmr->GPTLD     =       pConfig->GPTLD;
	hDevice->pTmr->GPTALD    =       pConfig->GPTALD;
	hDevice->pTmr->GPTPCON   =       pConfig->GPTPCON;
	hDevice->pTmr->GPTPMAT   =       pConfig->GPT_MATCH_VAL;

    hDevice->bInitialized = true;

    /* Enable timer interrupt */
    ADI_ENABLE_INT(hDevice->TimerIntID);

    /* Clear the callback function and callback parameters */
    hDevice->pfCallback   =       NULL;
    hDevice->pCBParam     =       NULL;

    adi_GPT_ResetTimer( hDevice );

    *phDevice = hDevice;                 /* set handle to initialized Timer device */

    return ADI_GPT_SUCCESS;
}


/*!
    @brief    Close the specified GP Timer.
    @details  This routine will close a GP Timer and place it in a non-operation state.

    @param[in] hDevice Handle to the GP Timer intance as returned by adi_GPT_Init

    @return Status
               - #ADI_GPT_SUCCESS if successfully uninitialized
               - #ADI_GPT_ERR_BAD_DEV_HANDLE [D] if device handle is invalid

    @note     1. The timer will be disabled
               2. If the timer is not in an initialized state no error will occur

*/
ADI_GPT_RESULT_TYPE adi_GPT_UnInit(ADI_GPT_HANDLE const hDevice)
{
ADI_GPT_RESULT_TYPE result;

#ifdef ADI_DEBUG
  if (ADI_GPT_INVALID_HANDLE(hDevice))
  {
    return ADI_GPT_ERR_BAD_DEV_HANDLE;
  }
#endif

  /* disable the timer and stub out the device pointer */
  result = adi_GPT_SetTimerEnable(hDevice, false);

  /* Disable timer interrupt */
  ADI_DISABLE_INT(hDevice->TimerIntID);

  ADI_UNINSTALL_HANDLER(hDevice->TimerIntID);

  hDevice->pTmr = (ADI_TIMER_TypeDef*)NULL;

  /* mark it uninitialized */
  hDevice->bInitialized = false;

  return result;
}


/*!
    @brief    Clear the timer's Capture Event Status
    @details  The capture event interrupt status is sticky. This routine will clear the
              capture event interrupt.


    @param[in]  hDevice      Handle to the GP Timer intance as returned by adi_GPT_Init
    @return Status
               - #ADI_GPT_SUCCESS if successful
               - #ADI_GPT_ERR_BAD_DEV_HANDLE [D] if device handle is invalid

*/
ADI_GPT_RESULT_TYPE adi_GPT_ClearCapturedEventStatus(ADI_GPT_HANDLE const hDevice)
{
#ifdef ADI_DEBUG
  if (ADI_GPT_INVALID_HANDLE(hDevice))
  {
    return ADI_GPT_ERR_BAD_DEV_HANDLE;
  }
#endif

  /* Clear by writing a '1' to bit[1] of TxCLRI */
  hDevice->pTmr->CLRI = TCLRI_CAP_CLR;

  return ADI_GPT_SUCCESS;
}


/*!
    @brief Clear the timer's timeout IRQ status
    @details  The timeout event interrupt status is sticky. This routine will clear the
              timeout event interrupt.


    @param[in]  hDevice      Handle to the GP Timer intance as returned by adi_GPT_Init
    @return Status
               - #ADI_GPT_SUCCESS if successful
               - #ADI_GPT_ERR_BAD_DEV_HANDLE [D] if device handle is invalid

*/
ADI_GPT_RESULT_TYPE adi_GPT_ClearTimeoutInterrupt(ADI_GPT_HANDLE hDevice)
{
#ifdef ADI_DEBUG
  if (ADI_GPT_INVALID_HANDLE(hDevice))
  {
    return ADI_GPT_ERR_BAD_DEV_HANDLE;
  }
#endif

  /*   Clear by writing a '1' to bit[0] of TxCLRI */
  hDevice->pTmr->CLRI = TCLRI_TMOUT_CLR;
  return ADI_GPT_SUCCESS;
}


/*!
    @brief      Read a GP Timer's Captured Event Value
    @details    When an event is triggered the timer's TxVAL register is copied into the TxCAP register.
                This functions simply returns the unsigned 16-bit value stored in the timer's TxCAP Register.
    @param[in]  hDevice      Handle to the GP Timer intance as returned by adi_GPT_Init
    @param[out] pCapturedValue  Pointer to the 16-bit capture event value which will be
                                written upon success


    @return Status
               - #ADI_GPT_SUCCESS if successful
               - #ADI_GPT_ERR_BAD_DEV_HANDLE [D] if device handle is invalid

*/
ADI_GPT_RESULT_TYPE adi_GPT_GetCapturedValue(ADI_GPT_HANDLE hDevice, uint16_t *pCapturedValue)
{
#ifdef ADI_DEBUG
  if (ADI_GPT_INVALID_HANDLE(hDevice))
  {
    return ADI_GPT_ERR_BAD_DEV_HANDLE;
  }
#endif

  *pCapturedValue = hDevice->pTmr->CAP;

  return ADI_GPT_SUCCESS;
}


/*!
    @brief  Determine if the GP Timer is ready to receive configuration commands
    @param[in]  hDevice      Handle to the GP Timer intance as returned by adi_GPT_Init


    @return  Status
               - false if the timer is not ready to receive commands to TxCON
               - true if the timer is ready to receive commands to TxCON
               - false [D] if the device handle is invalid

*/
bool_t adi_GPT_GetTimerBusy(ADI_GPT_HANDLE hDevice)
{
#ifdef ADI_DEBUG
  if (ADI_GPT_INVALID_HANDLE(hDevice))
  {
    return false;
  }
#endif

  return hDevice->pTmr->STA & TSTA_BUSY;
}


/*!
    @brief Determine if the GP Timer has captured an event
    @param[in]  hDevice      Handle to the GP Timer intance as returned by adi_GPT_Init


    @return Status
               - false if the timer has not captured an event
               - true if the timer has captured an event

     @note   This routine will return false in the event the Handle is NULL
*/
bool_t adi_GPT_GetCaptureEventPending( ADI_GPT_HANDLE hDevice )
{
  return hDevice->pTmr->STA & TSTA_CAP;
}


/*!
    @brief Determine if the GP Timer has timed out
    @param[in]  hDevice      Handle to the GP Timer intance as returned by adi_GPT_Init


    @return  Status
               - false if the timer has not timed out
               - true if the timer has timed out
*/
bool_t adi_GPT_GetTimeOutEventStatus(ADI_GPT_HANDLE hDevice)
{
  return hDevice->pTmr->STA & TSTA_TMOUT;
}


/*!
    @brief Return the timer's configuration status.
    @param[in]  hDevice      Handle to the GP Timer intance as returned by adi_GPT_Init
                             written upon success


    @return  Status
              - true:   If the timer has been initialized
              - false:  If the timer has not been initialized

    @note  This routine only examines whether or not the timer has been
    @note  initialized. It does not verify the configuration of the timer beyond
    @note  that.

*/
bool_t adi_GPT_GetTimerConfigValid(ADI_GPT_HANDLE hDevice)
{
  return   hDevice->bInitialized == true;
}


/*!
    @brief GP Set the Timer's 16-bit load value (LD)
    @param[in]  hDevice     Handle to the GP Timer intance as returned by adi_GPT_Init
    @param[in]  TimerLdVal  Value to be written to the timer's LD register


    @return Status
               - #ADI_GPT_SUCCESS if successful
               - #ADI_GPT_ERR_BAD_DEV_HANDLE [D] if device handle is invalid

*/
ADI_GPT_RESULT_TYPE adi_GPT_SetLdVal(ADI_GPT_HANDLE hDevice, const uint16_t TimerLdVal)
{
#ifdef ADI_DEBUG
  if (ADI_GPT_INVALID_HANDLE(hDevice))
  {
    return ADI_GPT_ERR_BAD_DEV_HANDLE;
  }
#endif

  hDevice->pTmr->LD = TimerLdVal;

  return ADI_GPT_SUCCESS;
}


/*!
    @brief GP Read the Timer's 16-bit load value (LD) which is used to re-load the
              timer's TX register

    @param[in]  hDevice      Handle to the GP Timer intance as returned by adi_GPT_Init
    @param[out] pTimerLdVal  Pointer to the 16-bit load value which will be
                             written upon success


    @return    Status
               - #ADI_GPT_SUCCESS if successful
               - #ADI_GPT_ERR_BAD_DEV_HANDLE [D] if device handle is invalid

*/
ADI_GPT_RESULT_TYPE adi_GPT_GetLdVal(ADI_GPT_HANDLE hDevice,  uint16_t *pTimerLdVal)
{
#ifdef ADI_DEBUG
  if (ADI_GPT_INVALID_HANDLE(hDevice))
  {
    return ADI_GPT_ERR_BAD_DEV_HANDLE;
  }
#endif

  *pTimerLdVal = hDevice->pTmr->LD;

  return ADI_GPT_SUCCESS;
}


 /*!
    @brief GP Read the Timer's 16-bit timer value (remaining time to count down/up)
    @param[in]  hDevice      Handle to the GP Timer intance as returned by adi_GPT_Init
    @param[out] pTimerValue  Pointer to the 16-bit timer value which will be
                             written upon success


    @return   Status
               - #ADI_GPT_SUCCESS if successful
               - #ADI_GPT_ERR_BAD_DEV_HANDLE [D] if device handle is invalid

*/
ADI_GPT_RESULT_TYPE adi_GPT_GetTxVal(ADI_GPT_HANDLE hDevice, uint16_t *pTimerValue)
{
#ifdef ADI_DEBUG
  if (ADI_GPT_INVALID_HANDLE(hDevice))
  {
    return ADI_GPT_ERR_BAD_DEV_HANDLE;
  }
#endif

  *pTimerValue = hDevice->pTmr->VAL;

  return ADI_GPT_SUCCESS;
}


/*!
    @brief GP Timer Set Event to Capture
               Each timer can be configured to capture one of various events.
    @param[in] hDevice Handle to the GP Timer intance as returned by adi_GPT_Init
    @param[in] Event


    @return   Status
               - #ADI_GPT_SUCCESS if successful
               - #ADI_GPT_ERR_BAD_DEV_HANDLE [D] if device handle is invalid
               - #ADI_GPT_ERR_PARAM_OUT_OF_RANGE [D] if param is invalid

*/
ADI_GPT_RESULT_TYPE adi_GPT_SetEventToCapture(ADI_GPT_HANDLE hDevice, ADI_GPT_CAPTURE_EVENTS_TYPE Event)
{
#ifdef ADI_DEBUG
  if (ADI_GPT_INVALID_HANDLE(hDevice))
  {
    return ADI_GPT_ERR_BAD_DEV_HANDLE;
  }

  if (Event != (Event & TCON_EVENT_MSK))
    return ADI_GPT_ERR_PARAM_OUT_OF_RANGE;
#endif

  ADI_ENTER_CRITICAL_REGION();
  hDevice->pTmr->CON &= ~TCON_EVENT_MSK;
  hDevice->pTmr->CON |= Event;
  ADI_EXIT_CRITICAL_REGION();

  return ADI_GPT_SUCCESS;
}


/*!
    @brief GP Timer Event Enable
               Enable or Disable the capturing of events
    @param[in] hDevice Handle to the GP Timer intance as returned by adi_GPT_Init
    @param[in] bEnable
               True: Enable Event Capture
               False: Disable Event Capture


    @return   Status
               - #ADI_GPT_SUCCESS if successful
               - #ADI_GPT_ERR_BAD_DEV_HANDLE [D] if device handle is invalid

*/
ADI_GPT_RESULT_TYPE adi_GPT_SetCaptureEventEnable(ADI_GPT_HANDLE hDevice, bool_t bEnable)
{
#ifdef ADI_DEBUG
  if (ADI_GPT_INVALID_HANDLE(hDevice))
  {
    return ADI_GPT_ERR_BAD_DEV_HANDLE;
  }
#endif

    ADI_ENTER_CRITICAL_REGION();
    if (bEnable) {
        hDevice->pTmr->CON |= TCON_EVENTEN;
    } else {
        hDevice->pTmr->CON &= ~TCON_EVENTEN;
    }
    ADI_EXIT_CRITICAL_REGION();

  return ADI_GPT_SUCCESS;
}


/*!
    @brief GP Timer set prescaler
               Timer can be configured with various prescaler values.
    @param[in] hDevice    Handle to the GP Timer intance as returned by adi_GPT_Init
    @param[in] preScaler  Prescaler emum value for clock scaling

    @return    Status
               - #ADI_GPT_SUCCESS if successful
               - #ADI_GPT_ERR_BAD_DEV_HANDLE [D] if device handle is invalid
               - #ADI_GPT_ERR_PARAM_OUT_OF_RANGE [D] if param is invalid

*/
ADI_GPT_RESULT_TYPE adi_GPT_SetPrescaler(ADI_GPT_HANDLE hDevice, ADI_GPT_PRESCALER_TYPE preScaler)
{
#ifdef ADI_DEBUG
  if (ADI_GPT_INVALID_HANDLE(hDevice))
  {
    return ADI_GPT_ERR_BAD_DEV_HANDLE;
  }

  if (preScaler != (preScaler & TCON_PRE_MSK))
    return ADI_GPT_ERR_PARAM_OUT_OF_RANGE;
#endif

	ADI_ENTER_CRITICAL_REGION();
	hDevice->pTmr->CON &= ~TCON_PRE_MSK;
	hDevice->pTmr->CON |= preScaler;
	ADI_EXIT_CRITICAL_REGION();

  return ADI_GPT_SUCCESS;
}


/*!
    @brief Enable/Disable the GP Timer

    @param[in] hDevice Handle to the GP Timer intance as returned by adi_GPT_Init
    @param[in] bEnable
               True: Enable the timer
               False: Disable and reset the timer, including the GPTVAL register


    @return   Status
               - #ADI_GPT_SUCCESS if successful
               - #ADI_GPT_ERR_BAD_DEV_HANDLE [D] if device handle is invalid

*/
ADI_GPT_RESULT_TYPE adi_GPT_SetTimerEnable(ADI_GPT_HANDLE hDevice, bool_t bEnable)
{
#ifdef ADI_DEBUG
  if (ADI_GPT_INVALID_HANDLE(hDevice))
  {
    return ADI_GPT_ERR_BAD_DEV_HANDLE;
  }
#endif

    ADI_ENTER_CRITICAL_REGION();
    if (bEnable) {
        hDevice->pTmr->CON |= TCON_ENABLE;
    } else {
        hDevice->pTmr->CON &= ~TCON_ENABLE;
    }
    ADI_EXIT_CRITICAL_REGION();

  return ADI_GPT_SUCCESS;
}


/*!
    @brief Place GP Timer in/out of Perodic Mode and set the reload value
               load value will be changed on a write to TxCLRI
               (The load value will also be set on a timeout)
               Timer can be configured to be periodic.
    @param[in] hDevice Handle to the GP Timer intance as returned by adi_GPT_Init
    @param[in] bMod
               True: Enable periodic mode
               False: Disable periodic mode
    @param[in] reloadValue  used to set TxLD


    @return  Status
               - #ADI_GPT_SUCCESS if successful
               - #ADI_GPT_ERR_BAD_DEV_HANDLE [D] if device handle is invalid

*/
ADI_GPT_RESULT_TYPE adi_GPT_SetPeriodicMode(ADI_GPT_HANDLE hDevice, bool_t bMod, uint16_t reloadValue)
{
#ifdef ADI_DEBUG
  if (ADI_GPT_INVALID_HANDLE(hDevice))
  {
    return ADI_GPT_ERR_BAD_DEV_HANDLE;
  }
#endif

    ADI_ENTER_CRITICAL_REGION();
    if (bMod) {
        hDevice->pTmr->CON |= TCON_MOD;
    } else {
        hDevice->pTmr->CON &= ~TCON_MOD;
    }
    ADI_EXIT_CRITICAL_REGION();

   hDevice->pTmr->LD = reloadValue;

  return ADI_GPT_SUCCESS;
}


/*!
    @brief Set GP Timer to operate in Free Running Mode
    @param[in] hDevice Handle to the GP Timer intance as returned by adi_GPT_Init

    @return   Status
               - #ADI_GPT_SUCCESS if successful
               - #ADI_GPT_ERR_BAD_DEV_HANDLE [D] if device handle is invalid

*/
ADI_GPT_RESULT_TYPE adi_GPT_SetFreeRunningMode(ADI_GPT_HANDLE hDevice)
{
#ifdef ADI_DEBUG
  if (ADI_GPT_INVALID_HANDLE(hDevice))
  {
    return ADI_GPT_ERR_BAD_DEV_HANDLE;
  }
#endif

	// clear mod bit for free-running mode
	ADI_ENTER_CRITICAL_REGION();
	hDevice->pTmr->CON &= ~TCON_MOD;
	ADI_EXIT_CRITICAL_REGION();

  return ADI_GPT_SUCCESS;
}


/*!
    @brief GP Set Count Mode
              Timers can be configured to either count up or count down.
    @param[in] hDevice Handle to the GP Timer intance as returned by adi_GPT_Init
    @param[in] Mode    ADI_GPT_COUNT_MODE_TYPE


    @return   Status
               - #ADI_GPT_SUCCESS if successful
               - #ADI_GPT_ERR_BAD_DEV_HANDLE [D] if device handle is invalid

*/
ADI_GPT_RESULT_TYPE adi_GPT_SetCountMode(ADI_GPT_HANDLE hDevice, ADI_GPT_COUNT_MODE_TYPE Mode)
{
#ifdef ADI_DEBUG
  if (ADI_GPT_INVALID_HANDLE(hDevice))
  {
    return ADI_GPT_ERR_BAD_DEV_HANDLE;
  }
#endif

	ADI_ENTER_CRITICAL_REGION();
	hDevice->pTmr->CON &= ~TCON_UP;
	hDevice->pTmr->CON |= Mode;
	ADI_EXIT_CRITICAL_REGION();

  return ADI_GPT_SUCCESS;
}


/*!
    @brief GP Clock Select
               Selects the clock source for a GP Timer
    @param[in] hDevice      Handle to the GP Timer intance as returned by adi_GPT_Init().
    @param[in] ClockSelect  Selection is made from one of the following clock sources:
                              UCLK/PCLK, according to target processor
                              PCLK/Internal HF 16 MHz oscillator, according to target processor
                              Internal LF 32 kHz oscillator
                              External LF 32 kHz crystal (requires external crystal oscillator circuit enable)

    @return   Status
               - #ADI_GPT_SUCCESS if successful
               - #ADI_GPT_ERR_BAD_DEV_HANDLE [D] if device handle is invalid
               - #ADI_GPT_ERR_PARAM_OUT_OF_RANGE [D] if param is invalid

    @note If selecting the external clock source, #ADI_GPT_CLOCK_SELECT_EXTERNAL_CLOCK, the external crystal oscillator circuit must be separetly enabled via the Power Device Driver.
    If and when disabeling the "External Clock" source, make sure it is not also being used (as a system clock mutplixer input) by some other system resource.

    @sa adi_PWR_SetLFXTALOscEnable().

*/
ADI_GPT_RESULT_TYPE adi_GPT_SetClockSelect(ADI_GPT_HANDLE hDevice, ADI_GPT_CLOCK_SELECT_TYPE ClockSelect)
{
#ifdef ADI_DEBUG
  if (ADI_GPT_INVALID_HANDLE(hDevice))
  {
    return ADI_GPT_ERR_BAD_DEV_HANDLE;
  }

  if (ClockSelect != (ClockSelect & TCON_CLK_MSK))
    return ADI_GPT_ERR_PARAM_OUT_OF_RANGE;
#endif

	ADI_ENTER_CRITICAL_REGION();
	hDevice->pTmr->CON &= ~TCON_CLK_MSK;
	hDevice->pTmr->CON |= ClockSelect;
	ADI_EXIT_CRITICAL_REGION();

  return ADI_GPT_SUCCESS;
}


/*!
    @brief GP Timer Reset
               Resets a timer to default settings

    @param[in] hDevice Handle to the GP Timer intance as returned by adi_GPT_Init
    @return  Status
               - #ADI_GPT_SUCCESS if successful
               - #ADI_GPT_ERR_BAD_DEV_HANDLE [D] if device handle is invalid

    @note     The timer will be disabled.
*/
ADI_GPT_RESULT_TYPE adi_GPT_ResetTimer( ADI_GPT_HANDLE hDevice )
{
#ifdef ADI_DEBUG
  if (ADI_GPT_INVALID_HANDLE(hDevice))
  {
    return ADI_GPT_ERR_BAD_DEV_HANDLE;
  }
#endif

  /* Clear any pending status */
  adi_GPT_ClearCapturedEventStatus( hDevice );
  adi_GPT_ClearTimeoutInterrupt( hDevice );

  return adi_GPT_SetTimerEnable(hDevice, false);
}

/*!
    @brief GP Timer Callback registration or un-registration function.
           Registers or un-registers the callback function, which will be called
           back to notify the events. The events listed in the enumeration
           #ADI_GPT_EVENT_TYPE will be notified to the application.

    @param[in] hDevice          Handle to the GP Timer intance as returned by adi_GPT_Init
    @param[in] pfCallback       Pointer to the callback function.
    @param[in] pCBParam         Callback prameter, which will be passed back to the application
                                when the callback is called.
    @return  Status
               - #ADI_GPT_SUCCESS if successful.
               - #ADI_GPT_ERR_BAD_DEV_HANDLE [D] if device handle is invalid.
*/
ADI_GPT_RESULT_TYPE adi_GPT_RegisterCallback (ADI_GPT_HANDLE hDevice, ADI_CALLBACK pfCallback, void *pCBParam)
{
#ifdef ADI_DEBUG
  if (ADI_GPT_INVALID_HANDLE(hDevice))
  {
    return ADI_GPT_ERR_BAD_DEV_HANDLE;
  }
#endif

  /* Save the applicatio given callback and callback parameters */
  hDevice->pfCallback   =       pfCallback;
  hDevice->pCBParam     =       pCBParam;

  return ADI_GPT_SUCCESS;
}


/* Common interrupt handler, which notifies events to the application  */
static void CommonIntHandler(ADI_GPT_DEV_DATA_TYPE *pDevice)
{
   /* Pointer to timer registers */
   ADI_TIMER_TypeDef  *pTmr = pDevice->pTmr;

   /* Status register value */
   uint16_t     IntStatus = pTmr->STA;

   /* Clear the interrupts */
   pTmr->CLRI = TCLRI_TMOUT_CLR;

   /* if a callback is installed notify the events */
   if(pDevice->pfCallback)
   {
       /* Check if timeout occured and notify of timeout event */
       if(IntStatus & BITM_GPT_GPTSTA_TMOUT)
       {
          pDevice->pfCallback(pDevice->pCBParam, ADI_GPT_EVENT_TIMEOUT, NULL);
       }

       /* Check if event is captured and notify of captured event */
       if(IntStatus & BITM_GPT_GPTSTA_CAP)
       {
          pDevice->pfCallback(pDevice->pCBParam, ADI_GPT_EVENT_CAPTURED, NULL);
       }
   }

   return;
}

/*! \cond PRIVATE */

/*!
    @brief GP Timer0 ISR.
           The Timer ISR simply clears the ISR

*/
ADI_INT_HANDLER(GP_Tmr0_Int_Handler)
{
   /* Call the common interrupt handler */
   CommonIntHandler(&gGPT_DevData[0]);
}


/*
    @brief GP Timer1 ISR.
           The Timer ISR simply clears the ISR

*/
ADI_INT_HANDLER(GP_Tmr1_Int_Handler)
{
   /* Call the common interrupt handler */
   CommonIntHandler(&gGPT_DevData[1]);
}

/* GPT2 valid on ADuCM350 */
/*!
    @brief GP Timer2 ISR.
           The Timer ISR simply clears the ISR
*/
ADI_INT_HANDLER(GP_Tmr2_Int_Handler)
{
   /* Call the common interrupt handler */
   CommonIntHandler(&gGPT_DevData[2]);
}


#define REVERT_ADI_MISRA_SUPPRESSIONS  /*!< Revert ADI MISRA Suppressions */
#include "misra.h"

/*! \endcond */

/*
** EOF
*/


/*@}*/
