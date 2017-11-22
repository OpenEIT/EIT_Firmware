/*********************************************************************************

Copyright (c) 2010-2014 Analog Devices, Inc. All Rights Reserved.

This software is proprietary to Analog Devices, Inc. and its licensors.  By using 
this software you agree to the terms of the associated Analog Devices Software 
License Agreement.

*********************************************************************************/

/*!
 *****************************************************************************
 * @file:    beep.c
 * @brief:   Beeper Device Implementations for ADuCxxx
 * @version: $Revision: 28525 $
 * @date:    $Date: 2014-11-12 14:51:26 -0500 (Wed, 12 Nov 2014) $
*****************************************************************************/

/*! \addtogroup BEEP_Driver BEEP Driver
 *  @{
 */

#include <stdlib.h>  /* for 'NULL" definition */

#define ASSERT_ADI_MISRA_SUPPRESSIONS  /*!< Apply ADI MISRA Suppressions */
#include "misra.h"

#include "adi_int.h"
#include "beep.h"

/*!
 * \struct ADI_BEEP_DEV_DATA_TYPE
 * Beeper device internal instance data structure.
 */
typedef struct ADI_BEEP_DEV_DATA_TYPE
{
    bool_t                         bInitialized;    /*!< Initialization state        */
    ADI_BEEP_TypeDef              *pRtc;            /*!< MMR address for this BEEP   */
    IRQn_Type                      IRQn;            /*!< BEEP interrupt number       */
    ADI_CALLBACK                   cbFunc;          /*!< Callback function           */
    ADI_BEEP_INTERRUPT_TYPE        cbWatch;         /*!< Callback watch bits         */
} ADI_BEEP_DEV_DATA_TYPE;


/* allocate internal device instance data array */
ADI_BEEP_DEV_DATA_TYPE gBEEP_DevData[ADI_BEEP_MAX_DEVID] = {0};  /*!< Private BEEP device driver instance data */


/* forward declarations... */
/* These are needed in the case where ADI_CFG_ENABLE_RTOS_SUPPORT=1              */
/* The ADI_INSTALL_HANDLER macro will expand to a call to adi_int_InstallHandler */
/* and the handler will be referenced                                            */
/* The non-OSAL expansion of ADI_INSTALL_HANDLER results in a NULL string        */
ADI_INT_HANDLER(Beep_Int_Handler);

/* debug handle checker */
#ifdef ADI_DEBUG
#define ADI_BEEP_INVALID_HANDLE(h) (&gBEEP_DevData[0] != h)
#endif


/* conditional static initialization data */
typedef struct {
	uint16_t BEEP_CFG;   /*!< Beeper configuration register */
	uint16_t BEEP_STAT;  /*!< Beeper status register */
	uint16_t BEEP_TONEA; /*!< Beeper ToneA register */
	uint16_t BEEP_TONEB; /*!< Beeper ToneB register */
} ADI_BEEP_CFG_TYPE;

static const ADI_BEEP_CFG_TYPE gBeeperStaticConfigData[ADI_BEEP_MAX_DEVID] = {
	/* single instance of Beeper device */
	{
		/* configuration register */
		( (BEEP_CFG_INTERRUPT_ON_SEQUENCE_END          << BITP_BEEP_BEEP_CFG_IRQ_SEQ_END)
		| (BEEP_CFG_INTERRUPT_ON_SEQUENCE_NEAR_END     << BITP_BEEP_BEEP_CFG_IRQ_SEQ_NEAR_END)
		| (BEEP_CFG_INTERRUPT_ON_TONEB_END             << BITP_BEEP_BEEP_CFG_IRQ_TONEB_END)
		| (BEEP_CFG_INTERRUPT_ON_TONEB_START           << BITP_BEEP_BEEP_CFG_IRQ_TONEB_START)
		| (BEEP_CFG_INTERRUPT_ON_TONEA_END             << BITP_BEEP_BEEP_CFG_IRQ_TONEA_END)
		| (BEEP_CFG_INTERRUPT_ON_TONEA_START           << BITP_BEEP_BEEP_CFG_IRQ_TONEA_START)
		| (BEEP_CFG_BEEPER_DISABLE                     << BITP_BEEP_BEEP_CFG_BEEP_EN)
		| (BEEP_CFG_SEQUENCE_REPEAT_VALUE              << BITP_BEEP_BEEP_CFG_SEQ_REPEAT)
		),

		/* Status register (interrupt clears) */
		( (BEEP_STAT_CLEAR_SEQUENCE_END_INTERRUPT      << BITP_BEEP_BEEP_STAT_EVT_SEQ_END)
		| (BEEP_STAT_CLEAR_SEQUENCE_NEAR_END_INTERRUPT << BITP_BEEP_BEEP_STAT_EVT_SEQ_NEAR_END)
		| (BEEP_STAT_CLEAR_TONEB_END_INTERRUPT         << BITP_BEEP_BEEP_STAT_EVT_TONEB_END)
		| (BEEP_STAT_CLEAR_TONEB_START_INTERRUPT       << BITP_BEEP_BEEP_STAT_EVT_TONEB_START)
		| (BEEP_STAT_CLEAR_TONEA_END_INTERRUPT         << BITP_BEEP_BEEP_STAT_EVT_TONEA_END)
		| (BEEP_STAT_CLEAR_TONEA_START_INTERRUPT       << BITP_BEEP_BEEP_STAT_EVT_TONEA_START)
		),

		/* ToneA control register */
		( (BEEP_TONEA_DISABLE                          << BITP_BEEP_BEEP_TONE_A_DISABLE)
		| (BEEP_TONEA_FREQUENCY                        << BITP_BEEP_BEEP_TONE_A_FREQ)
		| (BEEP_TONEA_DURATION                         << BITP_BEEP_BEEP_TONE_A_DUR)
		),

		/* ToneB control register */
		( (BEEP_TONEB_DISABLE                          << BITP_BEEP_BEEP_TONE_B_DISABLE)
		| (BEEP_TONEB_FREQUENCY                        << BITP_BEEP_BEEP_TONE_B_FREQ)
		| (BEEP_TONEB_DURATION                         << BITP_BEEP_BEEP_TONE_B_DUR)
		)
	}
};


/*!
    @brief BEEP Initialization

    @param[in] InstanceNum  #ADI_BEEP_DEV_ID_TYPE specifying the instance of the BEEP to use.
    @param[in] phDevice     Pointer to a location in the calling function memory space to which
                            the device handle will be written upon successful driver initialization.
    @return    Status
               - #ADI_BEEP_SUCCESS                      Success: BEEP device driver initialized successfully.
               - #ADI_BEEP_ERR_ALREADY_INITIALIZED [D]  Error: The BEEP is already initialized.
               - #ADI_BEEP_ERR_BAD_INSTANCE [D]         Error: The instance number is invalid.

	Initialize and allocate a BEEP device for other use.  The core NVIC BEEP interrupt is enabled.  This API
	must preceed all other beeper API calls and the handle returned must be passed to all other beeper API
	calls.


    @note   The contents of \a phDevice will be set to NULL upon failure.\n\n

    @note   The BEEP device driver will clear all pending interrupts and disable all beeper
            interrupts during beeper device initialization.

    @note   CALLBACKS: BEEP interrupt callbacks are \b disabled by default during BEEP device driver
            initialization (#adi_Beep_Init()).  The application may use the #adi_Beep_RegisterCallback()
            API to register an application-defined callback function with the BEEP device driver that
            will be called in response to select beeper interrupts.  The callback registration API
            takes an interrupt mask (of type #ADI_BEEP_INTERRUPT_TYPE) that designates the set of
            interrupts to watch (upon which to predicate the callback).  When any of the watched
            interrupts occur, the BEEP device driver will make the callback to the application for
            application-based interrupt handling.  The BEEP device driver clears the interrupt after
            the callback exits, as the beeper device driver interrupt handler exist.
            The application callback should \b avoid \b extended \b processing during the callback
            as the callback is made context of the initiating interrupt and will block lower-priority
            interrupts.  If extended application-level interrupt processing is required, the application
            should schedule it for the main application loop and exit the callback as soon as possible.\n\n

    @sa    adi_Beep_UnInit().
    @sa    adi_Beep_RegisterCallback().
*/
ADI_BEEP_RESULT_TYPE
adi_Beep_Init(ADI_BEEP_DEV_ID_TYPE const InstanceNum, ADI_BEEP_HANDLE* const phDevice)
{
    ADI_BEEP_HANDLE hDevice;

    /* store a bad handle in case of failure */
    *phDevice = (ADI_BEEP_HANDLE) NULL;

#ifdef ADI_DEBUG
    if (InstanceNum >= ADI_BEEP_MAX_DEVID)
    {
        return ADI_BEEP_ERR_BAD_INSTANCE;
    }
#endif

    /* local pointer to instance data */
    hDevice = &gBEEP_DevData[InstanceNum];

#ifdef ADI_DEBUG
    if (hDevice->bInitialized)
    {
        return ADI_BEEP_ERR_ALREADY_INITIALIZED;
    }
#endif

    /* first thing: make sure BEEP has a clock so it can respond */
    SystemClockSourcePowerUp (ADI_SYS_CLOCK_SOURCE_LFXTAL, ADI_SYS_CLOCK_TIMEOUT_USE_DEFAULT);

    /* initialize internal device data */
    hDevice->pRtc    = pADI_BEEP;
    hDevice->IRQn    = BEEP_IRQn;
    hDevice->cbFunc  = NULL;
    hDevice->cbWatch = (ADI_BEEP_INTERRUPT_TYPE) 0;

	/* assert statically configured initialization data */
	ADI_BEEP_CFG_TYPE const* pInitData = &gBeeperStaticConfigData[InstanceNum];

	hDevice->pRtc->BEEP_CFG    = pInitData->BEEP_CFG;
	hDevice->pRtc->BEEP_STAT   = pInitData->BEEP_STAT;
	hDevice->pRtc->BEEP_TONE_A = pInitData->BEEP_TONEA;
	hDevice->pRtc->BEEP_TONE_B = pInitData->BEEP_TONEB;

	/* install interrupt handler (see abstraction macro for OS in adi_rtos.h) */
	ADI_INSTALL_HANDLER(hDevice->IRQn, Beep_Int_Handler);

    /* enable beeper interrupts in NVIC */
    NVIC_EnableIRQ(hDevice->IRQn);

    /* mark driver initialized */
    hDevice->bInitialized = true;

    /* store handle at application handle pointer */
    *phDevice = hDevice;

    return ADI_BEEP_SUCCESS;            // initialized
}


/*!
 * @brief  Uninitialize and deallocate a BEEP device.
 *
 * @param[in]   hDevice    Device handle obtained from adi_Beep_Init().
 *
 * @return      Status
 *                - #ADI_BEEP_SUCCESS                    Success: Call completed successfully.
 *                - #ADI_BEEP_ERR_BAD_DEV_HANDLE [D]     Error: Invalid device handle parameter.
 *                - #ADI_BEEP_ERR_NOT_INITIALIZED [D]    Error: Device has not been initialized for use, see #adi_Beep_Init().
 *
 * Uninitialize and release an allocated BEEP device for other use.  The core NVIC BEEP interrupt is disabled.
 *
 * @sa        adi_Beep_Init().
 */
ADI_BEEP_RESULT_TYPE
adi_Beep_UnInit(ADI_BEEP_HANDLE hDevice)
{
#ifdef ADI_DEBUG
    if (ADI_BEEP_INVALID_HANDLE(hDevice))
        return ADI_BEEP_ERR_BAD_DEV_HANDLE;

    if (!hDevice->bInitialized)
        return ADI_BEEP_ERR_NOT_INITIALIZED;
#endif

    /* uninitialize */
    NVIC_DisableIRQ(hDevice->IRQn);
	ADI_UNINSTALL_HANDLER(hDevice->IRQn);
    hDevice->bInitialized = false;
    hDevice->pRtc         = NULL;
    hDevice->IRQn         = (IRQn_Type) 0;
    hDevice->cbFunc       = NULL;
    hDevice->cbWatch      = (ADI_BEEP_INTERRUPT_TYPE)0;

    return ADI_BEEP_SUCCESS;
}


/*!
 * @brief  Configure BEEP device interrupt mask.
 *
 * @param[in]   hDevice       Device handle obtained from adi_Beep_Init().
 * @param[in]   Interrupts    The set of beeper interrupts to operate upon.
 * @param[in]   bFlag         Flag designating whether to enable or disable \a Interrupts.
 *
 * @return      Status
 *                - #ADI_BEEP_SUCCESS                    Success: Call completed successfully.
 *                - #ADI_BEEP_ERR_BAD_DEV_HANDLE [D]     Error: Invalid device handle parameter.
 *                - #ADI_BEEP_ERR_NOT_INITIALIZED [D]    Error: Device has not been initialized for use, see #adi_Beep_Init().
 *
 * Set or clear a set of BEEP interrupts.  Can be one or many.
 *
 * Interrupts may also advise on beeper events which can be hooked to the application space through callbacks.
 *
 * @sa        adi_Beep_Init().
 * @sa        adi_Beep_GetInterruptEnables().
 * @sa        adi_Beep_RegisterCallback().
 */
ADI_BEEP_RESULT_TYPE
adi_Beep_SetInterruptEnables (ADI_BEEP_HANDLE const hDevice, ADI_BEEP_INTERRUPT_TYPE const Interrupts, bool_t const bFlag)
{
#ifdef ADI_DEBUG
    if (ADI_BEEP_INVALID_HANDLE(hDevice))
        return ADI_BEEP_ERR_BAD_DEV_HANDLE;

    if (!hDevice->bInitialized)
        return ADI_BEEP_ERR_NOT_INITIALIZED;
#endif

ADI_ENTER_CRITICAL_REGION();
	if (bFlag) {
		hDevice->pRtc->BEEP_CFG |= Interrupts;
	} else {
		hDevice->pRtc->BEEP_CFG &= ~(Interrupts);
	}
ADI_EXIT_CRITICAL_REGION();

	return ADI_BEEP_SUCCESS;
}


/*!
 * @brief  Retrieve the current BEEP device interrupt mask.
 *
 * @param[in]   hDevice       Device handle obtained from adi_Beep_Init().
 * @param[in]   pInterrupts   Pointer to an application-defined variable into which to write the result.
 *
 * @return      Status
 *                - #ADI_BEEP_SUCCESS                    Success: Call completed successfully.
 *                - #ADI_BEEP_ERR_BAD_DEV_HANDLE [D]     Error: Invalid device handle parameter.
 *                - #ADI_BEEP_ERR_NOT_INITIALIZED [D]    Error: Device has not been initialized for use, see #adi_Beep_Init().
 *
 * Retrieve the current beeper interrupt mask.  Writes the application-defined variable pointer (\a pInterrupts) object with the result.
 *
 * Interrupts may also advise on beeper events which can be hooked to the application space through callbacks.
 *
 * @sa        adi_Beep_Init().
 * @sa        adi_Beep_SetInterruptEnables().
 * @sa        adi_Beep_RegisterCallback().
 */
ADI_BEEP_RESULT_TYPE
adi_Beep_GetInterruptEnables (ADI_BEEP_HANDLE const hDevice, ADI_BEEP_INTERRUPT_TYPE* const pInterrupts)
{
#ifdef ADI_DEBUG
    if (ADI_BEEP_INVALID_HANDLE(hDevice))
        return ADI_BEEP_ERR_BAD_DEV_HANDLE;

    if (!hDevice->bInitialized)
        return ADI_BEEP_ERR_NOT_INITIALIZED;
#endif

	*pInterrupts = (ADI_BEEP_INTERRUPT_TYPE)(hDevice->pRtc->BEEP_CFG & ADI_BEEP_ALL_INTERRUPTS);

    return ADI_BEEP_SUCCESS;
}


/*!
 * @brief  Clear beeper interrupt status bits.
 *
 * @param[in]   hDevice       Device handle obtained from adi_Beep_Init().
 * @param[in]   Interrupts    The set of beeper interrupts to operate upon.
 *
 * @return      Status
 *                - #ADI_BEEP_SUCCESS                    Success: Call completed successfully.
 *                - #ADI_BEEP_ERR_BAD_DEV_HANDLE [D]     Error: Invalid device handle parameter.
 *                - #ADI_BEEP_ERR_NOT_INITIALIZED [D]    Error: Device has not been initialized for use, see #adi_Beep_Init().
 *
 * Clear a set of BEEP interrupts.  Can be one or many.  Only clears interrupts specified in the \a Interrupts parameter.
 * Use the #adi_Beep_GetInterruptStatus() API to query current beeper interrupt state.
 *
 * Interrupts may also advise on beeper events which can be hooked to the application space through callbacks.
 *
 * @sa        adi_Beep_Init().
 * @sa        adi_Beep_GetInterruptStatus().
 * @sa        adi_Beep_RegisterCallback().
 */
ADI_BEEP_RESULT_TYPE
adi_Beep_SetInterruptStatus (ADI_BEEP_HANDLE const hDevice, ADI_BEEP_INTERRUPT_TYPE const Interrupts)
{
#ifdef ADI_DEBUG
    if (ADI_BEEP_INVALID_HANDLE(hDevice))
        return ADI_BEEP_ERR_BAD_DEV_HANDLE;

    if (!hDevice->bInitialized)
        return ADI_BEEP_ERR_NOT_INITIALIZED;
#endif

ADI_ENTER_CRITICAL_REGION();
	hDevice->pRtc->BEEP_STAT |= (Interrupts & ADI_BEEP_ALL_INTERRUPTS);  // only write allowed interrupt bits
ADI_EXIT_CRITICAL_REGION();

	return ADI_BEEP_SUCCESS;
}


/*!
 * @brief  Retrieve the current BEEP device interrupt state.
 *
 * @param[in]   hDevice       Device handle obtained from adi_Beep_Init().
 * @param[in]   pInterrupts   Pointer to an application-defined variable into which to write the result.
 *
 * @return      Status
 *                - #ADI_BEEP_SUCCESS                    Success: Call completed successfully.
 *                - #ADI_BEEP_ERR_BAD_DEV_HANDLE [D]     Error: Invalid device handle parameter.
 *                - #ADI_BEEP_ERR_NOT_INITIALIZED [D]    Error: Device has not been initialized for use, see #adi_Beep_Init().
 *
 * Retrieve the current beeper interrupt state.  Writes the application-defined variable pointer (\a pInterrupts) object with the result.
 * Use the #adi_Beep_SetInterruptStatus() API to clear beeper interrupt state.
 *
 * Interrupts may also advise on beeper events which can be hooked to the application space through callbacks.
 *
 * @sa        adi_Beep_Init().
 * @sa        adi_Beep_SetInterruptStatus().
 * @sa        adi_Beep_RegisterCallback().
 */
ADI_BEEP_RESULT_TYPE
adi_Beep_GetInterruptStatus (ADI_BEEP_HANDLE const hDevice, ADI_BEEP_INTERRUPT_TYPE* const pInterrupts)
{
#ifdef ADI_DEBUG
    if (ADI_BEEP_INVALID_HANDLE(hDevice))
        return ADI_BEEP_ERR_BAD_DEV_HANDLE;

    if (!hDevice->bInitialized)
        return ADI_BEEP_ERR_NOT_INITIALIZED;
#endif

	*pInterrupts = (ADI_BEEP_INTERRUPT_TYPE)(hDevice->pRtc->BEEP_STAT & ADI_BEEP_ALL_INTERRUPTS);

    return ADI_BEEP_SUCCESS;
}


/*!
 * @brief  Retrieve the current BEEP activity state.
 *
 * @param[in]   hDevice       Device handle obtained from adi_Beep_Init().
 * @param[in]   pbFlag        Pointer to an application-defined Boolean variable into which to write the result:
 *                              - true  = beeper is currently active.
 *                              - false = beeper is inactive.
 *
 * @return      Status
 *                - #ADI_BEEP_SUCCESS                    Success: Call completed successfully.
 *                - #ADI_BEEP_ERR_BAD_DEV_HANDLE [D]     Error: Invalid device handle parameter.
 *                - #ADI_BEEP_ERR_NOT_INITIALIZED [D]    Error: Device has not been initialized for use, see #adi_Beep_Init().
 *
 * Retrieve the current beeper activity (BUSY) state.  Writes the application-defined \a pbFlag variable pointer object with the result.
 * This API allows applications to query whether the beeper is currently busy playing a tone or a tone sequence.
 *
 * Interrupts may also advise on beeper events which can be hooked to the application space through callbacks.
 *
 * @sa        adi_Beep_Init().
 * @sa        adi_Beep_GetInterruptStatus().
 * @sa        adi_Beep_RegisterCallback().
 */
ADI_BEEP_RESULT_TYPE
adi_Beep_GetBusyStatus (ADI_BEEP_HANDLE const hDevice, bool_t* const pbFlag)
{
#ifdef ADI_DEBUG
    if (ADI_BEEP_INVALID_HANDLE(hDevice))
        return ADI_BEEP_ERR_BAD_DEV_HANDLE;

    if (!hDevice->bInitialized)
        return ADI_BEEP_ERR_NOT_INITIALIZED;
#endif

	*pbFlag = (hDevice->pRtc->BEEP_STAT & ADI_BEEP_DEVICE_BUSY_BIT) ? true : false;

    return ADI_BEEP_SUCCESS;
}


/*!
 * @brief  Set or clear beeper device enablement state.
 *
 * @param[in]   hDevice       Device handle obtained from adi_Beep_Init().
 * @param[in]   bFlag         Flag designating whether to enable or disable the device:
 *                              - true  = device enable.
 *                              - false = device disable.
 *
 * @return      Status
 *                - #ADI_BEEP_SUCCESS                    Success: Call completed successfully.
 *                - #ADI_BEEP_ERR_BAD_DEV_HANDLE [D]     Error: Invalid device handle parameter.
 *                - #ADI_BEEP_ERR_NOT_INITIALIZED [D]    Error: Device has not been initialized for use, see #adi_Beep_Init().
 *
 * Enable or disable the beeper device.
 * Use the #adi_Beep_GetDeviceEnable() API to query current beeper device enablement state.
 * The beeper does not operate unless the device is enabled.
 *
 * @sa        adi_Beep_Init().
 * @sa        adi_Beep_GetDeviceEnable().
 */
ADI_BEEP_RESULT_TYPE
adi_Beep_SetDeviceEnable (ADI_BEEP_HANDLE const hDevice, bool_t const bFlag)
{
#ifdef ADI_DEBUG
    if (ADI_BEEP_INVALID_HANDLE(hDevice))
        return ADI_BEEP_ERR_BAD_DEV_HANDLE;

    if (!hDevice->bInitialized)
        return ADI_BEEP_ERR_NOT_INITIALIZED;
#endif

ADI_ENTER_CRITICAL_REGION();
	if (bFlag) {
		hDevice->pRtc->BEEP_CFG |= ADI_BEEP_DEVICE_ENABLE_BIT;
	} else {
		hDevice->pRtc->BEEP_CFG &= ~(ADI_BEEP_DEVICE_ENABLE_BIT);
	}
ADI_EXIT_CRITICAL_REGION();

	return ADI_BEEP_SUCCESS;
}


/*!
 * @brief  Retrieve the current beeper device enablement state.
 *
 * @param[in]   hDevice       Device handle obtained from adi_Beep_Init().
 * @param[in]   pbFlag        Pointer to an application-defined Boolean variable into which to write the result:
 *                              - true  = beeper is currently enabled.
 *                              - false = beeper is currently disabled.
 *
 * @return      Status
 *                - #ADI_BEEP_SUCCESS                    Success: Call completed successfully.
 *                - #ADI_BEEP_ERR_BAD_DEV_HANDLE [D]     Error: Invalid device handle parameter.
 *                - #ADI_BEEP_ERR_NOT_INITIALIZED [D]    Error: Device has not been initialized for use, see #adi_Beep_Init().
 *
 * Retrieve the current beeper device enablement state.  Writes the application-defined \a pbFlag variable pointer object with the result.
 * Use the #adi_Beep_SetDeviceEnable() API to set beeper device enablement state.
 * The beeper does not operate unless the device is enabled.
 *
 * @sa        adi_Beep_Init().
 * @sa        adi_Beep_SetDeviceEnable().
 */
ADI_BEEP_RESULT_TYPE
adi_Beep_GetDeviceEnable (ADI_BEEP_HANDLE const hDevice, bool_t* const pbFlag)
{
#ifdef ADI_DEBUG
    if (ADI_BEEP_INVALID_HANDLE(hDevice))
        return ADI_BEEP_ERR_BAD_DEV_HANDLE;

    if (!hDevice->bInitialized)
        return ADI_BEEP_ERR_NOT_INITIALIZED;
#endif

	*pbFlag = (hDevice->pRtc->BEEP_CFG & ADI_BEEP_DEVICE_ENABLE_BIT) ? true : false;

    return ADI_BEEP_SUCCESS;
}


/*!
 * @brief  Set or clear beeper tone enablement state.
 *
 * @param[in]   hDevice       Device handle obtained from adi_Beep_Init().
 * @param[in]   channel       ADI_BEEP_TONE_TYPE parameter designating the tone (A/B) to operate upon.
 * @param[in]   bFlag         Flag designating whether to enable or disable the tone:
 *                              - true  = tone enable.
 *                              - false = tone disable.
 *
 * @return      Status
 *                - #ADI_BEEP_SUCCESS                    Success: Call completed successfully.
 *                - #ADI_BEEP_ERR_BAD_DEV_HANDLE [D]     Error: Invalid device handle parameter.
 *                - #ADI_BEEP_ERR_NOT_INITIALIZED [D]    Error: Device has not been initialized for use, see #adi_Beep_Init().
 *
 * Enable or disable a beeper tone.
 * Use the #adi_Beep_GetToneEnable() API to query current beeper tone enablement state.
 *
 * @sa        adi_Beep_Init().
 * @sa        adi_Beep_GetToneEnable().
 */
ADI_BEEP_RESULT_TYPE
adi_Beep_SetToneEnable (ADI_BEEP_HANDLE const hDevice, ADI_BEEP_TONE_TYPE const channel, bool_t const bFlag)
{
	uint16_t* pReg;

#ifdef ADI_DEBUG
    if (ADI_BEEP_INVALID_HANDLE(hDevice))
        return ADI_BEEP_ERR_BAD_DEV_HANDLE;

    if (!hDevice->bInitialized)
        return ADI_BEEP_ERR_NOT_INITIALIZED;
#endif

    pReg = (ADI_BEEP_TONEA == channel)
        ? (uint16_t*)&hDevice->pRtc->BEEP_TONE_A
        : (uint16_t*)&hDevice->pRtc->BEEP_TONE_B;

ADI_ENTER_CRITICAL_REGION();
	/* note negative logic: tone DISABLE bit */
	if (bFlag) {
		*pReg &= ~(ADI_BEEP_TONE_DISABLE);
	} else {
		*pReg |= ADI_BEEP_TONE_DISABLE;
	}
ADI_EXIT_CRITICAL_REGION();

	return ADI_BEEP_SUCCESS;
}


/*!
 * @brief  Retrieve the current beeper tone enablement state.
 *
 * @param[in]   hDevice       Device handle obtained from adi_Beep_Init().
 * @param[in]   channel       ADI_BEEP_TONE_TYPE parameter designating the tone (A/B) to operate upon.
 * @param[in]   pbFlag        Pointer to an application-defined Boolean variable into which to write the result:
 *                              - true  = tone is currently enabled.
 *                              - false = tone is currently disabled.
 *
 * @return      Status
 *                - #ADI_BEEP_SUCCESS                    Success: Call completed successfully.
 *                - #ADI_BEEP_ERR_BAD_DEV_HANDLE [D]     Error: Invalid device handle parameter.
 *                - #ADI_BEEP_ERR_NOT_INITIALIZED [D]    Error: Device has not been initialized for use, see #adi_Beep_Init().
 *
 * Retrieve the current beeper device enablement state.  Writes the application-defined \a pbFlag variable pointer object with the result.
 * Use the #adi_Beep_SetDeviceEnable() API to set beeper device enablement state.
 * The beeper does not operate unless the device is enabled.
 *
 * @sa        adi_Beep_Init().
 * @sa        adi_Beep_SetToneEnable().
 */
ADI_BEEP_RESULT_TYPE
adi_Beep_GetToneEnable (ADI_BEEP_HANDLE const hDevice, ADI_BEEP_TONE_TYPE const channel, bool_t* const pbFlag)
{
	uint16_t* pReg;

#ifdef ADI_DEBUG
    if (ADI_BEEP_INVALID_HANDLE(hDevice))
        return ADI_BEEP_ERR_BAD_DEV_HANDLE;

    if (!hDevice->bInitialized)
        return ADI_BEEP_ERR_NOT_INITIALIZED;
#endif

    pReg = (ADI_BEEP_TONEA == channel)
        ? (uint16_t*)&hDevice->pRtc->BEEP_TONE_A
        : (uint16_t*)&hDevice->pRtc->BEEP_TONE_B;

	/* note negative logic */
	*pbFlag = (*pReg & ADI_BEEP_TONE_DISABLE) ? false : true;

	return ADI_BEEP_SUCCESS;
}


/*!
 * @brief  Set a beeper tone.
 *
 * @param[in]   hDevice       Device handle obtained from adi_Beep_Init().
 * @param[in]   channel       ADI_BEEP_TONE_TYPE parameter designating the tone (A/B) to operate upon.
 * @param[in]   Frequency     ADI_BEEP_NOTE_FREQUENCY_TYPE parameter designating the tone frequency.
 * @param[in]   Duration      ADI_BEEP_NOTE_DURATION_TYPE parameter designating the tone duration.
 *
 * @return      Status
 *                - #ADI_BEEP_SUCCESS                    Success: Call completed successfully.
 *                - #ADI_BEEP_ERR_BAD_DEV_HANDLE [D]     Error: Invalid device handle parameter.
 *                - #ADI_BEEP_ERR_NOT_INITIALIZED [D]    Error: Device has not been initialized for use, see #adi_Beep_Init().
 *                - #ADI_BEEP_ERR_PARAM_OUT_OF_RANGE [D] Error: Parameter is out of range.
 *
 * Programs a beeper tone and enables the tone channel.  Sets the tone frequency (which is actually a clock divider value,
 * so lower values make for higher frequencies).  Simultaneously, sets the tone duration.  Both the frequency and duration
 * values are bit-fields of the same register and so they are restricted to their respective bitfield widths.
 *
 * Tonal values are right-justified.
 *
 * Use the #adi_Beep_GetTone() API to query current beeper tone frequency and duration.
 *
 * @sa        adi_Beep_Init().
 * @sa        adi_Beep_GetTone().
 */
ADI_BEEP_RESULT_TYPE
adi_Beep_SetTone (ADI_BEEP_HANDLE              const hDevice,
                  ADI_BEEP_TONE_TYPE           const channel,
                  ADI_BEEP_NOTE_FREQUENCY_TYPE const Frequency,
                  ADI_BEEP_NOTE_DURATION_TYPE  const Duration)
{
    uint16_t* pReg;

#ifdef ADI_DEBUG
    if (ADI_BEEP_INVALID_HANDLE(hDevice))
        return ADI_BEEP_ERR_BAD_DEV_HANDLE;

    if (!hDevice->bInitialized)
        return ADI_BEEP_ERR_NOT_INITIALIZED;

    /* only 7-bit field */
    if (Frequency > 127)
        return ADI_BEEP_ERR_PARAM_OUT_OF_RANGE;

#endif

    pReg = (ADI_BEEP_TONEA == channel)
        ? (uint16_t*)&hDevice->pRtc->BEEP_TONE_A
        : (uint16_t*)&hDevice->pRtc->BEEP_TONE_B;

    /* set the frequency, duration (clears tone disable) all at once (no critical region needed) */
    *pReg = (Frequency << ADI_BEEP_TONE_FREQ_BITPOS) | (Duration << ADI_BEEP_TONE_DUR_BITPOS);

	return ADI_BEEP_SUCCESS;
}

/*!
 * @brief  Retrieve a beeper tone.
 *
 * @param[in]   hDevice       Device handle obtained from adi_Beep_Init().
 * @param[in]   channel       ADI_BEEP_TONE_TYPE parameter designating the tone (A/B) to operate upon.
 * @param[in]   pFrequency    Pointer to an application-defined ADI_BEEP_NOTE_FREQUENCY_TYPE variable into which to write the tone frequency.
 * @param[in]   pDuration     Pointer to an application-defined ADI_BEEP_NOTE_FREQUENCY_TYPE variable into which to write the tone duration.
 *
 * @return      Status
 *                - #ADI_BEEP_SUCCESS                    Success: Call completed successfully.
 *                - #ADI_BEEP_ERR_BAD_DEV_HANDLE [D]     Error: Invalid device handle parameter.
 *                - #ADI_BEEP_ERR_NOT_INITIALIZED [D]    Error: Device has not been initialized for use, see #adi_Beep_Init().
 *
 * Retrieves a beeper tone frequency and duration.  Writes the application-defined \a pFrequency and \a pDuration variable pointer objects with the result.
 * Both the frequency and duration values are bit-fields of the same register and so they are restricted to their respective bitfield widths.
 *
 * Tonal values are right-justified.
 *
 * Use the #adi_Beep_GetTone() API to get beeper tone frequency and duration.
 *
 * @sa        adi_Beep_Init().
 * @sa        adi_Beep_SetTone().
 */
ADI_BEEP_RESULT_TYPE
adi_Beep_GetTone (ADI_BEEP_HANDLE               const hDevice,
                  ADI_BEEP_TONE_TYPE            const channel,
                  ADI_BEEP_NOTE_FREQUENCY_TYPE* const pFrequency,
                  ADI_BEEP_NOTE_DURATION_TYPE*  const pDuration)
{
	uint16_t* pReg;
	uint16_t tmp;

#ifdef ADI_DEBUG
    if (ADI_BEEP_INVALID_HANDLE(hDevice))
        return ADI_BEEP_ERR_BAD_DEV_HANDLE;

    if (!hDevice->bInitialized)
        return ADI_BEEP_ERR_NOT_INITIALIZED;
#endif

    pReg = (ADI_BEEP_TONEA == channel)
        ? (uint16_t*)&hDevice->pRtc->BEEP_TONE_A
        : (uint16_t*)&hDevice->pRtc->BEEP_TONE_B;

    /* get tone reg once... */
	tmp = *pReg;
	*pFrequency = (ADI_BEEP_NOTE_FREQUENCY_TYPE) ( (tmp & ADI_BEEP_TONE_FREQ_MASK) >> ADI_BEEP_TONE_FREQ_BITPOS);
	*pDuration  = (ADI_BEEP_NOTE_DURATION_TYPE)  ( (tmp & ADI_BEEP_TONE_DUR_MASK)  >> ADI_BEEP_TONE_DUR_BITPOS);

	return ADI_BEEP_SUCCESS;
}


/*!
 * @brief  Set the beeper tone sequence count.
 *
 * @param[in]   hDevice       Device handle obtained from adi_Beep_Init().
 * @param[in]   Count         Number of A/B tone pairs to play in sequence.
 *
 * @return      Status
 *                - #ADI_BEEP_SUCCESS                    Success: Call completed successfully.
 *                - #ADI_BEEP_ERR_BAD_DEV_HANDLE [D]     Error: Invalid device handle parameter.
 *                - #ADI_BEEP_ERR_NOT_INITIALIZED [D]    Error: Device has not been initialized for use, see #adi_Beep_Init().
 *
 * Programs the A/B tone pair sequence count.
 *
 * Use the #adi_Beep_GetSequence() API to query the remaining beeper sequence count.
 *
 * @sa        adi_Beep_Init().
 * @sa        adi_Beep_GetSequence().
 */
ADI_BEEP_RESULT_TYPE
adi_Beep_SetSequence (ADI_BEEP_HANDLE const hDevice, uint8_t const Count)
{
	uint16_t tmp;

#ifdef ADI_DEBUG
    if (ADI_BEEP_INVALID_HANDLE(hDevice))
        return ADI_BEEP_ERR_BAD_DEV_HANDLE;

    if (!hDevice->bInitialized)
        return ADI_BEEP_ERR_NOT_INITIALIZED;
#endif

ADI_ENTER_CRITICAL_REGION();
	/* make a hole */
	tmp = hDevice->pRtc->BEEP_CFG & ~(ADI_BEEP_SEQUENCE_MASK);

	/* program new sequence count, while preserving everything else */
	hDevice->pRtc->BEEP_CFG = tmp | (Count << ADI_BEEP_SEQ_COUNT_BITPOS);
ADI_EXIT_CRITICAL_REGION();

	return ADI_BEEP_SUCCESS;
}


/*!
 * @brief  Get the current beeper tone sequence count.
 *
 * @param[in]   hDevice       Device handle obtained from adi_Beep_Init().
 * @param[in]   pCount        Pointer to an application-defined variable into which to write the result.
 *
 * @return      Status
 *                - #ADI_BEEP_SUCCESS                    Success: Call completed successfully.
 *                - #ADI_BEEP_ERR_BAD_DEV_HANDLE [D]     Error: Invalid device handle parameter.
 *                - #ADI_BEEP_ERR_NOT_INITIALIZED [D]    Error: Device has not been initialized for use, see #adi_Beep_Init().
 *
 * Retrieves the currently remaining A/B tone pair sequence count.  Writes the application-defined \a pCount variable pointer object with the result.
 *
 * Use the #adi_Beep_SetSequence() API to set the beeper sequence count.
 *
 * @note The beeper hardware automatically resets the ramaining sequence count to the most previously set sequence value upon sequence completion.
 *
 * @sa        adi_Beep_Init().
 * @sa        adi_Beep_SetSequence().
 */
ADI_BEEP_RESULT_TYPE
adi_Beep_GetSequence (ADI_BEEP_HANDLE const hDevice, uint8_t* const pCount)
{
#ifdef ADI_DEBUG
    if (ADI_BEEP_INVALID_HANDLE(hDevice))
        return ADI_BEEP_ERR_BAD_DEV_HANDLE;

    if (!hDevice->bInitialized)
        return ADI_BEEP_ERR_NOT_INITIALIZED;
#endif

	*pCount = (hDevice->pRtc->BEEP_STAT & ADI_BEEP_SEQUENCE_MASK) >> ADI_BEEP_SEQ_COUNT_BITPOS;

	return ADI_BEEP_SUCCESS;
}


/*************************************************************************************************
**************************************************************************************************
*****************************************   CALLBACKS   ******************************************
*****************************************      AND      ******************************************
*****************************************   INTERRUPT   ******************************************
*****************************************     STUFF     ******************************************
**************************************************************************************************
*************************************************************************************************/


/*!
    @brief BEEP Application callback registration API.

    @param[in]   hDevice     Device handle obtained from adi_Beep_Init().
    @param[in]   cbFunc      Application callback address; the function to call on the interrupt.
    @param[in]   cbWatch     The interrupt status bits to watch, controlling which interrupts result in a callback.

    @return    Status
               - #ADI_BEEP_SUCCESS                    Success: The callback is successfully registered.
               - #ADI_BEEP_ERR_BAD_DEV_HANDLE [D]     Error: Invalid device handle parameter.
               - #ADI_BEEP_ERR_NOT_INITIALIZED [D]    Error: Device has not been initialized for use, see #adi_Beep_Init().
               - #ADI_BEEP_ERR_PARAM_OUT_OF_RANGE [D] Error: The specified watch bits are out of range.

    Registers an application-defined callback \a cbFunc function address of type #ADI_CALLBACK with the beeper device driver.
    Callbacks are made in response to received BEEP interrupts, depending on whether they are enabled and
    whether the corresponding callback watch bits are set.  This allows the application to control which
    interrupts it wants to be called back on.

    The callback to the application is made in context of the originating interrupt (i.e., the beeper driver's
    beeper interrupt handler that is registered in the system's interrupt vector table).  Extended processing
    during the callback (an extension of the beeper's interrupt handler) is discouraged so as to avoid lower-priority
    interrupt blocking.  Also, any register read-modify-write operations should be protected using the
    ADI_ENTER_CRITICAL_REGION()/ADI_EXIT_CRITICAL_REGION() pair to prevent higher-priority interrupts from modifying
    said register during the read-modify-write operation.

    @note   CALLBACKS: BEEP interrupt callbacks are \b disabled by default during BEEP device driver
            initialization (#adi_Beep_Init()).  The application uses the #adi_Beep_RegisterCallback()
            API to request an application-defined callback from the BEEP device driver that
            will be called in response to select beeper interrupts.  The
            \a cbWatch parameter defines an interrupt mask designating the set or subset of
            interrupt status bits to watch (upon which to predicate the callback).  When any of the watched
            interrupts occur, the BEEP device driver will dispatch the requested application callback for
            application-based interrupt handling.  The BEEP device driver clears the interrupt when
            the callback exits.  The application callback should \b avoid \b extended \b processing
            during callbacks as the callback is executing context of the initiating interrupt and will
            block lower-priority interrupts.  If extended application-level interrupt processing is
            required, the application should schedule it for the main application loop and exit the
            callback as soon as possible.\n\n


    @sa    adi_Beep_Init().
*/
ADI_BEEP_RESULT_TYPE
adi_Beep_RegisterCallback (ADI_BEEP_HANDLE hDevice, ADI_CALLBACK cbFunc, uint32_t cbWatch)
{

#ifdef ADI_DEBUG
    if (ADI_BEEP_INVALID_HANDLE(hDevice))
        return ADI_BEEP_ERR_BAD_DEV_HANDLE;

    if (!hDevice->bInitialized)
        return ADI_BEEP_ERR_NOT_INITIALIZED;

    /* if registering a callback */
    if (cbFunc) {

        /* make sure the watch bits are supported */
        if (cbWatch & ~ADI_BEEP_ALL_INTERRUPTS)
            return ADI_BEEP_ERR_PARAM_OUT_OF_RANGE;
    }
#endif

    /* save the callback info */
    hDevice->cbFunc  = cbFunc;
    hDevice->cbWatch = (ADI_BEEP_INTERRUPT_TYPE) cbWatch;

    return ADI_BEEP_SUCCESS;
}


/*! @brief  BEEP device driver interrupt handler.  Overrides weakly-bound default interrupt handler in startup.c. */
ADI_INT_HANDLER (Beep_Int_Handler)
{
    ADI_BEEP_HANDLE pDev = &gBEEP_DevData[ADI_BEEP_DEVID_0];  // so far, there is only one BEEP, so this is safe
    register uint16_t candidate;
    register ADI_BEEP_INTERRUPT_TYPE fired;

    /* if we have an initialized driver and a registered callback... */
    if (pDev->bInitialized && pDev->cbFunc) {

		/* determin candidate interrupt source(s) */
ADI_ENTER_CRITICAL_REGION();
		/* read both status and mask registers without other interrupts in between */
		candidate = pDev->pRtc->BEEP_STAT & pDev->pRtc->BEEP_CFG & ADI_BEEP_ALL_INTERRUPTS;
ADI_EXIT_CRITICAL_REGION();

		fired = (ADI_BEEP_INTERRUPT_TYPE)0;

		/* need to test each interrupt source and whether it is enabled before notifying  */
		/* because each source is latched regardless of whether it is enabled or not :-(  */
		/* eventually, 'fired' now represents interrupts that are both set and enabled... */
		if (ADI_BEEP_INTERRUPT_SEQ_END & candidate)
			fired |= ADI_BEEP_INTERRUPT_SEQ_END;

		if (ADI_BEEP_INTERRUPT_SEQ_NEAR_END & candidate)
			fired |= ADI_BEEP_INTERRUPT_SEQ_NEAR_END;

		if (ADI_BEEP_INTERRUPT_TONEB_END & candidate)
			fired |= ADI_BEEP_INTERRUPT_TONEB_END;

		if (ADI_BEEP_INTERRUPT_TONEB_START & candidate)
			fired |= ADI_BEEP_INTERRUPT_TONEB_START;

		if (ADI_BEEP_INTERRUPT_TONEA_END & candidate)
			fired |= ADI_BEEP_INTERRUPT_TONEA_END;

		if (ADI_BEEP_INTERRUPT_TONEA_START & candidate)
			fired |= ADI_BEEP_INTERRUPT_TONEA_START;

        /* forward the interrupt to the user if he is watching it and it has fired */
        /* pass the "fired" value as the event.  argument param is not used */
        if (pDev->cbWatch & fired)
            pDev->cbFunc (pDev, fired, NULL);
    }

    /* clear the watched interrupt(s) that fired */
    adi_Beep_SetInterruptStatus(pDev, fired);
}


#define REVERT_ADI_MISRA_SUPPRESSIONS  /*!< Revert ADI MISRA Suppressions */
#include "misra.h"

/*
** EOF
*/

/*@}*/
