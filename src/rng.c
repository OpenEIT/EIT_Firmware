/*********************************************************************************

Copyright (c) 2012-2014 Analog Devices, Inc. All Rights Reserved.

This software is proprietary to Analog Devices, Inc. and its licensors.  By using 
this software you agree to the terms of the associated Analog Devices Software 
License Agreement.

*********************************************************************************/

/*!
 *****************************************************************************
 * @file:    rng.c
 * @brief:   Random Number Generator Device Implementations for ADuCXXxxx
 * @version: $Revision: 28670 $
 * @date:    $Date: 2014-11-18 13:46:44 -0500 (Tue, 18 Nov 2014) $
*****************************************************************************/

/*! \addtogroup RNG_Driver RNG Driver
 *  Random Number Generator Device Driver
 *  @{
 */

#include <stdlib.h>  /* for 'NULL" definition */

#define ASSERT_ADI_MISRA_SUPPRESSIONS  /*!< Apply ADI MISRA Suppressions */
#include "misra.h"

#include "adi_int.h"
#include "rng.h"

/* forward declarations... */
/* These are needed in the case where ADI_CFG_ENABLE_RTOS_SUPPORT=1              */
/* The ADI_INSTALL_HANDLER macro will expand to a call to adi_int_InstallHandler */
/* and the handler will be referenced                                            */
/* The non-OSAL expansion of ADI_INSTALL_HANDLER results in a NULL string        */
ADI_INT_HANDLER(Random_Int_Handler);


/*! \struct ADI_RNG_DEV_DATA_TYPE
 * RNG device internal instance data structure */
typedef struct ADI_RNG_DEV_DATA_TYPE
{
    bool_t                         bInitialized;    /*!< Initialization state       */
    ADI_RNG_TypeDef                *pRng;           /*!< MMR address for this RNG   */
    IRQn_Type                      IRQn;            /*!< RNG interrupt number       */
    ADI_CALLBACK                   cbFunc;          /*!< Callback function          */
    uint32_t                       cbWatch;         /*!< Callback watch bits        */
} ADI_RNG_DEV_DATA_TYPE;


/*! RNG configuration structure used to hold the application configuration.
*/
typedef struct
{
    uint16_t        RNGCTL;         /*!< RNG Control Register */
    uint16_t        RNGLEN;         /*!< RNG sample length register value */
}ADI_RNG_CFG_TYPE;

/* allocate instance data */
ADI_RNG_DEV_DATA_TYPE gRNG_DevData[ADI_RNG_MAX_DEVID] = {0};  /*!< Private RNG device driver instance data */

/* RNG application configuration array */
static const ADI_RNG_CFG_TYPE   gRngStaticConfigData[ADI_RNG_MAX_DEVID] =
{   /* single instance of RNG device */
    {
        /* RNG Control Register (RNGCTL) value */
        (( RNG0_CFG_OSC_COUNTER          <<  BITP_RNG_RNGCTL_CNTEN      ) |\
         ( RTC0_CFG_RNG_TIMER_MODE       <<  BITP_RNG_RNGCTL_TMRMODE    )),

        /* RNG Sample Length Register value */
        (( RTC0_CFG_LENGTH_RELOAD       <<  BITP_RNG_RNGLEN_LENRLD      ) |\
         ( RTC0_CFG_LENGTH_PRESCALER    <<  BITP_RNG_RNGLEN_LENPRE      ))
    }
};

/* debug helpers */
#ifdef ADI_DEBUG
#define ADI_RNG_INVALID_HANDLE(h) (&gRNG_DevData[0] != h)
#endif


/*!
    @brief RNG Initialization

    @param[in] InstanceNum  #ADI_RNG_DEV_ID_TYPE specifying the instance of the RNG to use.
    @param[in] phDevice     Pointer to a location in the calling function memory space to which
                            the device handle will be written upon successful driver initialization.

    @return    Status
               - #ADI_RNG_SUCCESS                      Success: RNG device driver initialized successfully.
               - #ADI_RNG_ERR_ALREADY_INITIALIZED [D]  Error: The RNG is already initialized.
               - #ADI_RNG_ERR_BAD_INSTANCE [D]         Error: The instance number is invalid.

	Initialize and allocate a RNG device for other use.  The core NVIC RNG interrupt is enabled.  This API
	must preceed all other RNG API calls and the handle returned must be passed to all other RNG API calls.

    @note   The contents of \a phDevice will be set to NULL upon failure.\n\n

    @note   The RNG device driver will clear all pending interrupts and disable all RNG
            interrupts during RNG device initialization.

    @note   CALLBACKS: RNG interrupt callbacks are \b disabled by default during RNG device driver
            initialization (#adi_RNG_Init()).  The application may use the #adi_RNG_RegisterCallback()
            API to register an application-defined callback function with the RNG device driver that
            will be called in response to select RNG interrupts.  The callback registration API
            takes an interrupt mask (of type #ADI_RNG_INTERRUPT_TYPE) that designates the set of
            interrupts to watch (upon which to predicate the callback).  When any of the watched
            interrupts occur, the RNG device driver will make the callback to the application for
            application-based interrupt handling.  The RNG device driver clears the interrupt after
            the callback exits, as the RNG device driver interrupt handler exist.
            The application callback should \b avoid \b extended \b processing during the callback
            as the callback is made context of the initiating interrupt and will block lower-priority
            interrupts.  If extended application-level interrupt processing is required, the application
            should schedule it for the main application loop and exit the callback as soon as possible.\n\n

    @sa    adi_RNG_UnInit().
    @sa    adi_RNG_RegisterCallback().
*/
ADI_RNG_RESULT_TYPE
adi_RNG_Init(ADI_RNG_DEV_ID_TYPE const InstanceNum, ADI_RNG_HANDLE* const phDevice)
{
    ADI_RNG_HANDLE hDevice;

    /* store a bad handle in case of failure */
    *phDevice = (ADI_RNG_HANDLE) NULL;

#ifdef ADI_DEBUG
    if (InstanceNum >= ADI_RNG_MAX_DEVID)
    {
        return ADI_RNG_ERR_BAD_INSTANCE;
    }
#endif

    /* local pointer to instance data */
    hDevice = &gRNG_DevData[InstanceNum];

#ifdef ADI_DEBUG
    if (hDevice->bInitialized)
    {
        return ADI_RNG_ERR_ALREADY_INITIALIZED;
    }
#endif

    /* first thing: make sure RNG has a clock so it can respond */
    SystemClockSourcePowerUp (ADI_SYS_CLOCK_SOURCE_LFXTAL, ADI_SYS_CLOCK_TIMEOUT_USE_DEFAULT);

    /* initialize internal device data */
    hDevice->pRng    = pADI_RNG;
    hDevice->IRQn    = RAND_IRQn;
    hDevice->cbFunc  = NULL;
    hDevice->cbWatch = (ADI_RNG_INTERRUPT_TYPE) 0;

    /* clear any pending interrupts */
    hDevice->pRng->RNGSTAT = ADI_RNG_ALL_INTERRUPTS;

    /* assert statically configured initialization data */
    ADI_RNG_CFG_TYPE const* pInitData = &gRngStaticConfigData[InstanceNum];

    hDevice->pRng->RNGCTL = pInitData->RNGCTL;
    hDevice->pRng->RNGLEN = pInitData->RNGLEN;

    /* install interrupt handler (see abstraction macro for OS in adi_rtos.h) */
    ADI_INSTALL_HANDLER(hDevice->IRQn, Random_Int_Handler);

    /* mark driver initialized */
    hDevice->bInitialized = true;

    /* store handle at application handle pointer */
    *phDevice = hDevice;

    return ADI_RNG_SUCCESS;            // initialized
}


/*!
 * @brief  Uninitialize and deallocate a RNG device.
 *
 * @param[in]   hDevice    Device handle obtained from adi_Rng_Init().
 *
 * @return      Status
 *                - #ADI_RNG_SUCCESS                    Success: Call completed successfully.
 *                - #ADI_RNG_ERR_BAD_DEV_HANDLE [D]     Error: Invalid device handle parameter.
 *                - #ADI_RNG_ERR_NOT_INITIALIZED [D]    Error: Device has not been initialized for use, see #adi_Rng_Init().
 *
 * Uninitialize and release an allocated RNG device for other use.  The core NVIC RNG interrupt is disabled.
 *
 * @sa        adi_Rng_Init().
 */
ADI_RNG_RESULT_TYPE
adi_RNG_UnInit(ADI_RNG_HANDLE hDevice)
{
#ifdef ADI_DEBUG
    if (ADI_RNG_INVALID_HANDLE(hDevice))
        return ADI_RNG_ERR_BAD_DEV_HANDLE;

    if (!hDevice->bInitialized)
        return ADI_RNG_ERR_NOT_INITIALIZED;
#endif

    /* uninitialize */
    NVIC_DisableIRQ(hDevice->IRQn);
    
    ADI_UNINSTALL_HANDLER(hDevice->IRQn);
    
    hDevice->bInitialized = false;
    hDevice->pRng         = NULL;
    hDevice->IRQn         = (IRQn_Type) 0;
    hDevice->cbFunc       = NULL;
    hDevice->cbWatch      = (ADI_RNG_INTERRUPT_TYPE)0;

    return ADI_RNG_SUCCESS;
}


/*!
 * @brief  Enable/Disable RNG device.
 *
 * @param[in]   hDevice       Device handle obtained from adi_Rng_Init().
 * @param[in]   bFlag         Flag designating whether to enable or disable RNG device.
 *
 * @return      Status
 *                - #ADI_RNG_SUCCESS                    Success: Call completed successfully.
 *                - #ADI_RNG_ERR_BAD_DEV_HANDLE [D]     Error: Invalid device handle parameter.
 *                - #ADI_RNG_ERR_NOT_INITIALIZED [D]    Error: Device has not been initialized for use, see #adi_Rng_Init().
 *
 * @sa        adi_Rng_Init().
 * @sa        adi_Rng_RegisterCallback().
 */
ADI_RNG_RESULT_TYPE
adi_RNG_Enable (ADI_RNG_HANDLE const hDevice, bool_t const bFlag)
{
#ifdef ADI_DEBUG
    if (ADI_RNG_INVALID_HANDLE(hDevice))
        return ADI_RNG_ERR_BAD_DEV_HANDLE;

    if (!hDevice->bInitialized)
        return ADI_RNG_ERR_NOT_INITIALIZED;
#endif

ADI_ENTER_CRITICAL_REGION();
	if (bFlag) {
		hDevice->pRng->RNGCTL |= BITM_RNG_RNGCTL_RNGEN;
	} else {
		hDevice->pRng->RNGCTL &= ~(BITM_RNG_RNGCTL_RNGEN);
	}
ADI_EXIT_CRITICAL_REGION();

	return ADI_RNG_SUCCESS;
}


/*!
 * @brief  Enable/Disable RNG oscillator counter.
 *
 * @param[in]   hDevice       Device handle obtained from adi_Rng_Init().
 * @param[in]   bFlag         Flag designating whether to enable or disable RNG oscillator counter.
 *
 * @return      Status
 *                - #ADI_RNG_SUCCESS                    Success: Call completed successfully.
 *                - #ADI_RNG_ERR_BAD_DEV_HANDLE [D]     Error: Invalid device handle parameter.
 *                - #ADI_RNG_ERR_NOT_INITIALIZED [D]    Error: Device has not been initialized for use, see #adi_Rng_Init().
 *
 * @sa        adi_Rng_Init().
 * @sa        adi_Rng_RegisterCallback().
 */
ADI_RNG_RESULT_TYPE
adi_RNG_EnableCounter (ADI_RNG_HANDLE const hDevice, bool_t const bFlag)
{
#ifdef ADI_DEBUG
    if (ADI_RNG_INVALID_HANDLE(hDevice))
        return ADI_RNG_ERR_BAD_DEV_HANDLE;

    if (!hDevice->bInitialized)
        return ADI_RNG_ERR_NOT_INITIALIZED;
#endif

ADI_ENTER_CRITICAL_REGION();
	if (bFlag) {
		hDevice->pRng->RNGCTL |= BITM_RNG_RNGCTL_CNTEN;
	} else {
		hDevice->pRng->RNGCTL &= ~(BITM_RNG_RNGCTL_CNTEN);
	}
ADI_EXIT_CRITICAL_REGION();

	return ADI_RNG_SUCCESS;
}


/*!
 * @brief  Sets RNG device in RNG mode or One-shot timer mode.
 *
 * @param[in]   hDevice       Device handle obtained from adi_Rng_Init().
 * @param[in]   bFlag         Flag to select between RNG mode and One-shot timer modes.
 *                              - true:  One-shot timer mode.
 *                              - false: RNG mode.
 *
 * @return      Status
 *                - #ADI_RNG_SUCCESS                    Success: Call completed successfully.
 *                - #ADI_RNG_ERR_BAD_DEV_HANDLE [D]     Error: Invalid device handle parameter.
 *                - #ADI_RNG_ERR_NOT_INITIALIZED [D]    Error: Device has not been initialized for use, see #adi_Rng_Init().
 *
 * @sa        adi_Rng_Init().
 * @sa        adi_Rng_RegisterCallback().
 */
ADI_RNG_RESULT_TYPE
adi_RNG_SetMode (ADI_RNG_HANDLE const hDevice, bool_t const bFlag)
{
#ifdef ADI_DEBUG
    if (ADI_RNG_INVALID_HANDLE(hDevice))
        return ADI_RNG_ERR_BAD_DEV_HANDLE;

    if (!hDevice->bInitialized)
        return ADI_RNG_ERR_NOT_INITIALIZED;
#endif

ADI_ENTER_CRITICAL_REGION();
	if (bFlag) {
		hDevice->pRng->RNGCTL |= BITM_RNG_RNGCTL_TMRMODE;
	} else {
		hDevice->pRng->RNGCTL &= ~(BITM_RNG_RNGCTL_TMRMODE);
	}
ADI_EXIT_CRITICAL_REGION();

	return ADI_RNG_SUCCESS;
}


/*!
 * @brief  Sets the reload value for the sample counter.
 *
 * @param[in]   hDevice       Device handle obtained from adi_Rng_Init().
 * @param[in]   LenReload     Reload value for the sample counter.
 *
 * @return      Status
 *                - #ADI_RNG_SUCCESS                    Success: Call completed successfully.
 *                - #ADI_RNG_ERR_BAD_DEV_HANDLE [D]     Error: Invalid device handle parameter.
 *                - #ADI_RNG_ERR_NOT_INITIALIZED [D]    Error: Device has not been initialized for use, see #adi_Rng_Init().
 *
 * @sa        adi_Rng_Init().
 * @sa        adi_Rng_RegisterCallback().
 */
ADI_RNG_RESULT_TYPE
adi_RNG_SetLenReload (ADI_RNG_HANDLE const hDevice, uint16_t LenReload)
{
#ifdef ADI_DEBUG
    if (ADI_RNG_INVALID_HANDLE(hDevice))
        return ADI_RNG_ERR_BAD_DEV_HANDLE;

    if (!hDevice->bInitialized)
        return ADI_RNG_ERR_NOT_INITIALIZED;
#endif

ADI_ENTER_CRITICAL_REGION();
    /* Clear the samle counter reload value field */
    hDevice->pRng->RNGLEN &= ~(BITM_RNG_RNGLEN_LENRLD);
    /* Load the new reload value */
    hDevice->pRng->RNGLEN |= LenReload;
ADI_EXIT_CRITICAL_REGION();

	return ADI_RNG_SUCCESS;
}


/*!
 * @brief  Sets the prescaler value for the sample counter.
 *
 * @param[in]   hDevice         Device handle obtained from adi_Rng_Init().
 * @param[in]   LenPrescaler    Prescaler value for the sample counter.
 *
 * @return      Status
 *                - #ADI_RNG_SUCCESS                    Success: Call completed successfully.
 *                - #ADI_RNG_ERR_BAD_DEV_HANDLE [D]     Error: Invalid device handle parameter.
 *                - #ADI_RNG_ERR_NOT_INITIALIZED [D]    Error: Device has not been initialized for use, see #adi_Rng_Init().
 *
 * @sa        adi_Rng_Init().
 * @sa        adi_Rng_RegisterCallback().
 */
ADI_RNG_RESULT_TYPE
adi_RNG_SetLenPrescaler (ADI_RNG_HANDLE const hDevice, uint16_t LenPrescaler)
{
#ifdef ADI_DEBUG
    if (ADI_RNG_INVALID_HANDLE(hDevice))
        return ADI_RNG_ERR_BAD_DEV_HANDLE;

    if (!hDevice->bInitialized)
        return ADI_RNG_ERR_NOT_INITIALIZED;
#endif

ADI_ENTER_CRITICAL_REGION();
    /* Clear the samle counter prescaler value field */
    hDevice->pRng->RNGLEN &= ~(BITM_RNG_RNGLEN_LENPRE);
    /* Load the new prescaler value */
    hDevice->pRng->RNGLEN |= (LenPrescaler << BITP_RNG_RNGLEN_LENPRE);
ADI_EXIT_CRITICAL_REGION();

	return ADI_RNG_SUCCESS;
}


/*!
 * @brief  Sets the RNG control register.
 *
 * @param[in]   hDevice         Device handle obtained from adi_Rng_Init().
 * @param[in]   Control         RNG control register configuration value.
 *
 * @return      Status
 *                - #ADI_RNG_SUCCESS                    Success: Call completed successfully.
 *                - #ADI_RNG_ERR_BAD_DEV_HANDLE [D]     Error: Invalid device handle parameter.
 *                - #ADI_RNG_ERR_NOT_INITIALIZED [D]    Error: Device has not been initialized for use, see #adi_Rng_Init().
 *
 * @sa        adi_Rng_Init().
 * @sa        adi_Rng_RegisterCallback().
 */
ADI_RNG_RESULT_TYPE
adi_RNG_SetControl (ADI_RNG_HANDLE const hDevice, uint16_t Control)
{
#ifdef ADI_DEBUG
    if (ADI_RNG_INVALID_HANDLE(hDevice))
        return ADI_RNG_ERR_BAD_DEV_HANDLE;

    if (!hDevice->bInitialized)
        return ADI_RNG_ERR_NOT_INITIALIZED;
#endif

ADI_ENTER_CRITICAL_REGION();
    hDevice->pRng->RNGCTL = Control;
ADI_EXIT_CRITICAL_REGION();

	return ADI_RNG_SUCCESS;
}


/*!
 * @brief  Sets the RNG sample length register.
 *
 * @param[in]   hDevice         Device handle obtained from adi_Rng_Init().
 * @param[in]   SampleLen       RNG sample length register configuration value.
 *
 * @return      Status
 *                - #ADI_RNG_SUCCESS                    Success: Call completed successfully.
 *                - #ADI_RNG_ERR_BAD_DEV_HANDLE [D]     Error: Invalid device handle parameter.
 *                - #ADI_RNG_ERR_NOT_INITIALIZED [D]    Error: Device has not been initialized for use, see #adi_Rng_Init().
 *
 * @sa        adi_Rng_Init().
 * @sa        adi_Rng_RegisterCallback().
 */
ADI_RNG_RESULT_TYPE
adi_RNG_SetSampleLen (ADI_RNG_HANDLE const hDevice, uint16_t SampleLen)
{
#ifdef ADI_DEBUG
    if (ADI_RNG_INVALID_HANDLE(hDevice))
        return ADI_RNG_ERR_BAD_DEV_HANDLE;

    if (!hDevice->bInitialized)
        return ADI_RNG_ERR_NOT_INITIALIZED;
#endif

ADI_ENTER_CRITICAL_REGION();
    hDevice->pRng->RNGLEN = SampleLen;
ADI_EXIT_CRITICAL_REGION();

	return ADI_RNG_SUCCESS;
}


/*!
 * @brief  Retrieve the current state of RNG data/CRC accumulator register.
 *
 * @param[in]   hDevice       Device handle obtained from adi_Rng_Init().
 * @param[out]  pbFlag        Pointer to an application-defined Boolean variable into which to write the result:
 *                              - true  = RNG data is ready to be read.
 *                              - false = RNG data is not ready.
 *
 * @return      Status
 *                - #ADI_RNG_SUCCESS                    Success: Call completed successfully.
 *                - #ADI_RNG_ERR_BAD_DEV_HANDLE [D]     Error: Invalid device handle parameter.
 *                - #ADI_RNG_ERR_NOT_INITIALIZED [D]    Error: Device has not been initialized for use, see #adi_Rng_Init().
 *
 * Retrieve the current state of RNG data/CRC accumulator register. The register holds the final entropy value
 * accumulated by RNG and it should to read only when the data is ready.
 *
 * @sa        adi_Rng_Init().
 * @sa        adi_RNG_GetRngData().
 * @sa        adi_Rng_RegisterCallback().
 */
ADI_RNG_RESULT_TYPE
adi_RNG_GetRdyStatus (ADI_RNG_HANDLE const hDevice, bool_t* const pbFlag)
{
#ifdef ADI_DEBUG
    if (ADI_RNG_INVALID_HANDLE(hDevice))
        return ADI_RNG_ERR_BAD_DEV_HANDLE;

    if (!hDevice->bInitialized)
        return ADI_RNG_ERR_NOT_INITIALIZED;
#endif

    /* Get the RNG Ready status bit */
	*pbFlag = (hDevice->pRng->RNGSTAT & BITM_RNG_RNGSTAT_RNGRDY) ? true : false;

    return ADI_RNG_SUCCESS;
}


/*!
 * @brief  Retrieve the current value of the RNG data register.
 *
 * @param[in]   hDevice       Device handle obtained from adi_Rng_Init().
 * @param[out]  pRngData      Pointer to an application-defined variable into which to write the result.
 *                            A value of 0 is returned when the data is not valid.
 *
 * @return      Status
 *                - #ADI_RNG_SUCCESS                    Success: Call completed successfully.
 *                - #ADI_RNG_ERR_NOT_VALID              Error: Data valid only when RNGRDY bit is set.
 *                - #ADI_RNG_ERR_BAD_DEV_HANDLE [D]     Error: Invalid device handle parameter.
 *                - #ADI_RNG_ERR_NOT_INITIALIZED [D]    Error: Device has not been initialized for use, see #adi_Rng_Init().
 *
 * Retrieve the current value of RNG data register.  The register provides \a the CRC (entropy) accumulator value.
 * Reading the RNG data will clear the RNGRDY status and cause a new random number to be generated.
 *
 * @sa        adi_Rng_Init().
 * @sa        adi_Rng_GetRdyStatus().
 * @sa        adi_Rng_RegisterCallback().
 * @sa        adi_RNG_GetOscCount().
 */
ADI_RNG_RESULT_TYPE
adi_RNG_GetRngData (ADI_RNG_HANDLE const hDevice, uint16_t* const pRngData)
{
#ifdef ADI_DEBUG
    if (ADI_RNG_INVALID_HANDLE(hDevice))
        return ADI_RNG_ERR_BAD_DEV_HANDLE;

    if (!hDevice->bInitialized)
        return ADI_RNG_ERR_NOT_INITIALIZED;
#endif

    /* data valid when RNGRDY bit is set */
    if (hDevice->pRng->RNGSTAT & BITM_RNG_RNGSTAT_RNGRDY)
    {
        /* Get the RNG CRC accumulator value */
	*pRngData = hDevice->pRng->RNGDATA;
    }
    else
    {
        /* data not valid */
        *pRngData = 0;
        
        return ADI_RNG_ERR_NOT_VALID;
    }

    return ADI_RNG_SUCCESS;
}


/*!
 * @brief  Retrieve the current value of the RNG control register.
 *
 * @param[in]   hDevice       Device handle obtained from adi_Rng_Init().
 * @param[out]  pControl      Pointer to an application-defined variable into which to write the result.
 *
 * @return      Status
 *                - #ADI_RNG_SUCCESS                    Success: Call completed successfully.
 *                - #ADI_RNG_ERR_BAD_DEV_HANDLE [D]     Error: Invalid device handle parameter.
 *                - #ADI_RNG_ERR_NOT_INITIALIZED [D]    Error: Device has not been initialized for use, see #adi_Rng_Init().
 *
 * @sa        adi_Rng_Init().
 * @sa        adi_Rng_RegisterCallback().
 */
ADI_RNG_RESULT_TYPE
adi_RNG_GetControl (ADI_RNG_HANDLE const hDevice, uint16_t* const pControl)
{
#ifdef ADI_DEBUG
    if (ADI_RNG_INVALID_HANDLE(hDevice))
        return ADI_RNG_ERR_BAD_DEV_HANDLE;

    if (!hDevice->bInitialized)
        return ADI_RNG_ERR_NOT_INITIALIZED;
#endif

	*pControl = hDevice->pRng->RNGCTL;

    return ADI_RNG_SUCCESS;
}


/*!
 * @brief  Retrieve the current RNG Oscillator count.
 *
 * @param[in]   hDevice       Device handle obtained from adi_Rng_Init().
 * @param[out]  pOscCount     Pointer to an application-defined variable into which to write the result.
 *                            A value of 0 is returned when the data is not valid.
 *
 * @return      Status
 *                - #ADI_RNG_SUCCESS                    Success: Call completed successfully.
 *                - #ADI_RNG_ERR_NOT_VALID              Error: Data valid only when RNGRDY and CNTEN bits are set.
 *                - #ADI_RNG_ERR_BAD_DEV_HANDLE [D]     Error: Invalid device handle parameter.
 *                - #ADI_RNG_ERR_NOT_INITIALIZED [D]    Error: Device has not been initialized for use, see #adi_Rng_Init().
 *
 * The RNG Oscillator count should be read before reading the RNG data because reading the RNG data will 
 * clear the RNGRDY status.
 *
 * @sa        adi_Rng_Init().
 * @sa        adi_Rng_RegisterCallback().
 * @sa        adi_RNG_GetRngData().
 */
ADI_RNG_RESULT_TYPE
adi_RNG_GetOscCount (ADI_RNG_HANDLE const hDevice, uint32_t* const pOscCount)
{
    uint32_t tmp;

#ifdef ADI_DEBUG
    if (ADI_RNG_INVALID_HANDLE(hDevice))
        return ADI_RNG_ERR_BAD_DEV_HANDLE;

    if (!hDevice->bInitialized)
        return ADI_RNG_ERR_NOT_INITIALIZED;
#endif

    /* data valid when RNGRDY and CNTEN bits are set */
    if ((hDevice->pRng->RNGSTAT & BITM_RNG_RNGSTAT_RNGRDY) &&
        (hDevice->pRng->RNGCTL & BITM_RNG_RNGCTL_CNTEN))
    {
        /* Get the oscillator count high count */
        tmp = hDevice->pRng->RNGCNTH;
        /* Append the low count and return it */
	*pOscCount = ((tmp << 16) | hDevice->pRng->RNGCNTL);
    }
    else
    {
      /* data not valid */
      *pOscCount = 0;
      
      return ADI_RNG_ERR_NOT_VALID;
    }
    
    return ADI_RNG_SUCCESS;
}

/*!
 * @brief  Retrieve the current value of the RNG sample length register.
 *
 * @param[in]   hDevice       Device handle obtained from adi_Rng_Init().
 * @param[out]  pSampleLen    Pointer to an application-defined variable into which to write the result.
 *
 * @return      Status
 *                - #ADI_RNG_SUCCESS                    Success: Call completed successfully.
 *                - #ADI_RNG_ERR_BAD_DEV_HANDLE [D]     Error: Invalid device handle parameter.
 *                - #ADI_RNG_ERR_NOT_INITIALIZED [D]    Error: Device has not been initialized for use, see #adi_Rng_Init().
 *
 * @sa        adi_Rng_Init().
 * @sa        adi_Rng_RegisterCallback().
 */
ADI_RNG_RESULT_TYPE
adi_RNG_GetSampleLen (ADI_RNG_HANDLE const hDevice, uint16_t* const pSampleLen)
{
#ifdef ADI_DEBUG
    if (ADI_RNG_INVALID_HANDLE(hDevice))
        return ADI_RNG_ERR_BAD_DEV_HANDLE;

    if (!hDevice->bInitialized)
        return ADI_RNG_ERR_NOT_INITIALIZED;
#endif

	*pSampleLen = hDevice->pRng->RNGLEN;

    return ADI_RNG_SUCCESS;
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
    @brief RNG Application callback registration API.

    @param[in]   hDevice     Device handle obtained from adi_Rng_Init().
    @param[in]   cbFunc      Application callback address; the function to call on the interrupt.
    @param[in]   cbWatch     The interrupt status bits to watch, controlling which interrupts result in a callback.

    @return    Status
               - #ADI_RNG_SUCCESS                    Success: The callback is successfully registered.
               - #ADI_RNG_ERR_BAD_DEV_HANDLE [D]     Error: Invalid device handle parameter.
               - #ADI_RNG_ERR_NOT_INITIALIZED [D]    Error: Device has not been initialized for use, see #adi_Rng_Init().
               - #ADI_RNG_ERR_PARAM_OUT_OF_RANGE [D] Error: The specified watch bits are out of range.

    Registers an application-defined callback \a cbFunc function address of type #ADI_CALLBACK with the RNG device driver.
    Callbacks are made in response to received RNG interrupts, depending on whether they are enabled and
    whether the corresponding callback watch bits are set.  This allows the application to control which
    interrupts it wants to be called back on.

    The callback to the application is made in context of the originating interrupt (i.e., the RNG driver's
    RNG interrupt handler that is registered in the system's interrupt vector table).  Extended processing
    during the callback (an extension of the RNG's interrupt handler) is discouraged so as to avoid lower-priority
    interrupt blocking.  Also, any register read-modify-write operations should be protected using the
    ADI_ENTER_CRITICAL_REGION()/ADI_EXIT_CRITICAL_REGION() pair to prevent higher-priority interrupts from modifying
    said register during the read-modify-write operation.

    @note   CALLBACKS: RNG interrupt callbacks are \b disabled by default during RNG device driver
            initialization (#adi_Rng_Init()).  The application uses the #adi_Rng_RegisterCallback()
            API to request an application-defined callback from the RNG device driver that
            will be called in response to select RNG interrupts.  The
            \a cbWatch parameter defines an interrupt mask designating the set or subset of
            interrupt status bits to watch (upon which to predicate the callback).  When any of the watched
            interrupts occur, the RNG device driver will dispatch the requested application callback for
            application-based interrupt handling.  The RNG device driver clears the interrupt when
            the callback exits.  The application callback should \b avoid \b extended \b processing
            during callbacks as the callback is executing context of the initiating interrupt and will
            block lower-priority interrupts.  If extended application-level interrupt processing is
            required, the application should schedule it for the main application loop and exit the
            callback as soon as possible.\n\n


    @sa    adi_Rng_Init().
*/
ADI_RNG_RESULT_TYPE
adi_RNG_RegisterCallback (ADI_RNG_HANDLE hDevice, ADI_CALLBACK cbFunc, uint32_t cbWatch)
{

#ifdef ADI_DEBUG
    if (ADI_RNG_INVALID_HANDLE(hDevice))
        return ADI_RNG_ERR_BAD_DEV_HANDLE;

    if (!hDevice->bInitialized)
        return ADI_RNG_ERR_NOT_INITIALIZED;

    /* if registering a callback */
    if (cbFunc) {
        /* make sure the watch bits are supported */
        if (cbWatch & ~ADI_RNG_ALL_INTERRUPTS)
            return ADI_RNG_ERR_PARAM_OUT_OF_RANGE;
    }
#endif

    /* save the callback info */
    hDevice->cbFunc  = cbFunc;
    hDevice->cbWatch = cbWatch;

    /* enable RNG interrupts in NVIC */
    NVIC_EnableIRQ(hDevice->IRQn);

    return ADI_RNG_SUCCESS;
}


/*! @brief  RNG device driver interrupt handler.  Overrides weakly-bound default interrupt handler in startup.c. */
ADI_INT_HANDLER (Random_Int_Handler)
{
    ADI_RNG_HANDLE pDev = &gRNG_DevData[ADI_RNG_DEVID_0];  // so far, there is only one RNG, so this is safe
    register uint16_t candidate;
    register ADI_RNG_INTERRUPT_TYPE fired;

    /* if we have an initialized driver and a registered callback... */
    if (pDev->bInitialized && pDev->cbFunc) {

	/* determin candidate interrupt source(s) */
ADI_ENTER_CRITICAL_REGION();
	/* read status register without other interrupts in between */
	candidate = (pDev->pRng->RNGSTAT & ADI_RNG_ALL_INTERRUPTS);
ADI_EXIT_CRITICAL_REGION();

	fired = (ADI_RNG_INTERRUPT_TYPE)0;

	/* need to test each interrupt source and whether it is enabled before notifying  */
	/* because each source is latched regardless of whether it is enabled or not :-(  */
	/* eventually, 'fired' now represents interrupts that are both set and enabled... */
	if (ADI_RNG_INTERRUPT_RNG_RDY & candidate)
		fired |= ADI_RNG_INTERRUPT_RNG_RDY;

        /* forward the interrupt to the user if he is watching it and it has fired */
        /* pass the "fired" value as the event.  argument param is not used */
        if (pDev->cbWatch & fired)
            pDev->cbFunc (pDev, fired, NULL);
        
        /* clear the watched interrupt(s) that fired */
        pDev->pRng->RNGSTAT = fired;
    }
}


#define REVERT_ADI_MISRA_SUPPRESSIONS  /*!< Revert ADI MISRA Suppressions */
#include "misra.h"

/*
** EOF
*/

/*@}*/
