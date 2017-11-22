/*********************************************************************************

Copyright (c) 2010-2014 Analog Devices, Inc. All Rights Reserved.

This software is proprietary to Analog Devices, Inc. and its licensors.  By using 
this software you agree to the terms of the associated Analog Devices Software 
License Agreement.

*********************************************************************************/

/*!
 ******************************************************************************
 * @file:    wdt.c
 * @brief:   WDT Device Definitions for ADuCxxx
 * @version: $Revision: 28525 $
 * @date:    $Date: 2014-11-12 14:51:26 -0500 (Wed, 12 Nov 2014) $
 *****************************************************************************/

/*! \addtogroup WDT_Driver WDT Driver
 *  @{
 */

#define ASSERT_ADI_MISRA_SUPPRESSIONS  /*!< Apply ADI MISRA Suppressions */
#include "misra.h"

#include <stddef.h>                 /* for the definition of NULL */
#include "wdt.h"
#include "startup.h"             /* Obtain definition of WEAK */
#include "adi_rtos.h"

/* write sync macros */
#define WDT_SYNC_BITS       ( (1<<BITP_WDT_T3STA_CON) | (1<<BITP_WDT_T3STA_LD) | (1<<BITP_WDT_T3STA_CLRI) )  /*!< bits used for testing write sync */
#define WDT_SYNC_PENDING    (pADI_WDT->T3STA & WDT_SYNC_BITS)                                                /*!< write sync test */


/* forward declarations... */
/* These are needed in the case where ADI_CFG_ENABLE_RTOS_SUPPORT=1              */
/* The ADI_INSTALL_HANDLER macro will expand to a call to adi_int_InstallHandler */
/* and the handler will be referenced                                            */
/* The non-OSAL expansion of ADI_INSTALL_HANDLER results in a NULL string        */
ADI_INT_HANDLER(WDog_Tmr_Int_Handler);



/*! \struct ADI_WDT_DEV_DATA_TYPE

    Watchdog Private Data typedef and initialization.

    This structure contains the "state" information for the
    instance of the device.  For watchdog there is only one
    of these objects
*/
typedef struct ADI_WDT_DEV_DATA_TYPE
{
    ADI_WDT_STATE_TYPE InitState;      /*!< Initialisation State */
    IRQn_Type          IRQn;           /*!< RTC interrupt number        */
    bool_t             bBusSyncWait;   /*!< Wait for Bus-Sync (if true) */
} ADI_WDT_DEV_DATA_TYPE;


static ADI_WDT_DEV_DATA_TYPE WDT_DevData[ADI_WDT_MAX_DEVID];

/* handle checker for debug mode */
#ifdef ADI_DEBUG
#define ADI_WDT_INVALID_HANDLE(h) (&WDT_DevData[ADI_WDT_DEVID_0] != h)
#endif

/* conditionally create static initialization data based on adi_wdt_config.h settings */
typedef struct {
	uint16_t wdtLoadRegister;     /*!< WDT load register    */
	uint16_t wdtControlRegister;  /*!< WDT control register */
	uint16_t wdtResetRegister;    /*!< WDT reset register   */
} ADI_WDT_STATIC_INIT_TYPE;

static const ADI_WDT_STATIC_INIT_TYPE gWdtStaticInitData[ADI_WDT_MAX_DEVID] = {
	/* single instance of WDT device */
    {
		/* load register */
		(WDT_LOAD_VALUE),

		/* control register*/
		( (WDT_CONTROL_TIMER_MODE      << BITP_WDT_T3CON_MOD)
		| (WDT_CONTROL_DEVICE_ENABLE   << BITP_WDT_T3CON_ENABLE)
		| (WDT_CONTROL_CLOCK_PRESCALER << BITP_WDT_T3CON_PRE)
		| (WDT_CONTROL_TIMEOUT_MODE    << BITP_WDT_T3CON_IRQ)
		| (WDT_CONTROL_HIBERNATE_MODE  << BITP_WDT_T3CON_PMD)
		),

		/* reset mode*/
		(WDT_RESET_MODE),
    }
};


/*!
    @brief Initialize and allocate the WDT device for use.

    @param[in] DevID        Integer specifying the ID of watchdog to use
                            This number MUST be zero (watchdog only supports
                            one instance).
    @param[in] phDevice     Pointer to a location that the device data pointer
                            will be written upon successful initialization.
    @return    Status
               - #ADI_WDT_SUCCESS if successfully initialized.
               - #ADI_WDT_ERR_ALREADY_INITIALIZED [D] if watchdog already initialized.
               - #ADI_WDT_ERR_INITIALIZATION_FAIL watchdog failed to initialize.
               - #ADI_WDT_ERR_BAD_DEV_ID if DevID is not zero.


    No other WDT APIs may be called until the device init function is called.

    Initialize the WDT device using internal default configuration settings and allocate the device for use.  The caller's
    device handle is written with the device instance data pointer.  The returned device handle is required to be passed
    to all subsequent calls to convey which device instance to operate on.

    @note      The contents of phDevice will be set to NULL upon failure.

    @warning   This always puts the watchdog in the periodic mode (i.e. MOD bit of T3CON
               is set) and leaves it disabled.  This driver does \a not support free-running mode.

    @sa         adi_WDT_UnInit().
*/
ADI_WDT_RESULT_TYPE adi_WDT_Init(ADI_WDT_DEV_ID_TYPE const DevID, ADI_WDT_DEV_HANDLE * const phDevice)
{
    ADI_WDT_DEV_HANDLE hDevice = &WDT_DevData[DevID];  /* local handle to device data structure */

    /* store a bad handle in case of failure */
    *phDevice = (ADI_WDT_DEV_HANDLE) NULL;
     hDevice->bBusSyncWait = true;
     hDevice->InitState = ADI_WDT_STATE_UNINITIALIZED;
#ifdef ADI_DEBUG
    if (DevID >= ADI_WDT_MAX_DEVID)
        return ADI_WDT_ERR_BAD_DEV_ID;

    if (ADI_WDT_STATE_UNINITIALIZED != hDevice->InitState)
        return ADI_WDT_ERR_ALREADY_INITIALIZED;
#endif

    /* if WDT not locked, and disabled (i.e. timer is not active) ... */
    if (!(pADI_WDT->T3STA & T3STA_LOCK) && !(pADI_WDT->T3CON & T3CON_ENABLE)) {

		/* WDT is not locked and not enabled, so proceed to configure it... */

		/* assert pre-configured static data... */
		ADI_WDT_STATIC_INIT_TYPE const* pInitData = &gWdtStaticInitData[DevID];

		pADI_WDT->T3LD  = pInitData->wdtLoadRegister;
		pADI_WDT->T3CON = pInitData->wdtControlRegister;
		pADI_WDT->T3RCR = pInitData->wdtResetRegister;

    }  /* end of unlocked & disabled WDT init block */

    /* else, WDT is either:
       a) locked (i.e. its already configured and running), in which case
          we can't do anything (the user can still use these APIs to reset
          the timer and perform status checks), OR,
       b) not locked, but is enabled (i.e. the timer is running), in which
          case we also can't make any changes until the user decides to disable
          and re-configure the timer
      bottom line: no further WDT configurations to perform at this time...
    */

    /* install interrupt handler (see abstraction macro for OS in adi_rtos.h) */
    hDevice->IRQn = WDT_IRQn;
    ADI_INSTALL_HANDLER(hDevice->IRQn, WDog_Tmr_Int_Handler);

    /* Enable WDT interrupts in NVIC */
    ADI_ENABLE_INT(hDevice->IRQn);

    hDevice->InitState = ADI_WDT_STATE_INITIALIZED;  /* mark device as initialized */
    *phDevice = hDevice;                             /* write user handle */

    /* kick the dog on the way out, maximizing application time to configure... */
    if (ADI_WDT_SUCCESS != adi_WDT_ResetTimer(hDevice))
        return ADI_WDT_ERR_INITIALIZATION_FAIL;

    return ADI_WDT_SUCCESS;
}


/*!
    @brief      Uninitialize and deallocate the WDT device.

    @param[in]  hDevice    Device handle obtained from adi_WDT_Init().

    @return     Status
                - #ADI_WDT_SUCCESS if successfully un-initialized.
                - #ADI_WDT_ERR_NOT_INITIALIZED [D] if watchdog not initialized.
                - #ADI_WDT_ERR_BAD_DEV_DATA [D] if watchdog handle is corrupt.

    @sa         adi_WDT_Init().
*/
ADI_WDT_RESULT_TYPE adi_WDT_UnInit(ADI_WDT_DEV_HANDLE const hDevice)
{
#ifdef ADI_DEBUG
    /* ensure handle points to a valid watchdog device ... */
    if (ADI_WDT_INVALID_HANDLE(hDevice))
        return ADI_WDT_ERR_BAD_DEV_DATA;

    /* check that watchdog device is initialized ... */
    if (ADI_WDT_STATE_INITIALIZED != hDevice->InitState)
        return ADI_WDT_ERR_NOT_INITIALIZED;
#endif

   /* it makes no sense to "uninit" the timer if its locked
      so this must indicate the failure */
#ifdef ADI_WDT_HW_ANOMALY_RESOLVED
    if (pADI_WDT->T3STA & T3STA_LOCK)
        return ADI_WDT_ERR_LOCKED;
#endif

    /* uninitialize */
    ADI_DISABLE_INT(hDevice->IRQn);
    ADI_UNINSTALL_HANDLER(hDevice->IRQn);
    hDevice->IRQn = (IRQn_Type) 0;

    /* If timer not started, simply abandon "ownership" of the WDT */
    hDevice->InitState = ADI_WDT_STATE_UNINITIALIZED;      /* watchdog no longer in use */
    return ADI_WDT_SUCCESS;
}


/*!
    @brief      Set the watchdog Load Count.

    @details    Used to set the contents of T3LD.  This value in conjunction with
                the prescale sets the timeout value of the watchdog.

    @param[in]  hDevice     Device handle obtained from adi_WDT_Init().
    @param[in]  LoadCount   The 16-bit value to load into T3LD register.

    @return     Status
                - #ADI_WDT_SUCCESS if mode set as requested.
                - #ADI_WDT_SYNC_IN_PROGRESS WDT Bus-sync in progress.
                - #ADI_WDT_ERR_LOCKED if the timer is "locked" (see: #adi_WDT_GetLockedStatus).
                - #ADI_WDT_ERR_NOT_INITIALIZED [D] if watchdog not initialized.
                - #ADI_WDT_ERR_BAD_DEV_DATA [D] if watchdog handle is corrupt.

    @sa         adi_WDT_GetCount().
*/
ADI_WDT_RESULT_TYPE adi_WDT_SetLoadCount(ADI_WDT_DEV_HANDLE const hDevice, const uint16_t LoadCount)
{
#ifdef ADI_DEBUG
    /* ensure handle points to a valid watchdog device ... */
    if (ADI_WDT_INVALID_HANDLE(hDevice))
        return ADI_WDT_ERR_BAD_DEV_DATA;

    /* check that watchdog device is initialized ... */
    if (ADI_WDT_STATE_INITIALIZED != hDevice->InitState)
        return ADI_WDT_ERR_NOT_INITIALIZED;
#endif
    /* ensure that the watchdog is not locked out ... */
#ifdef ADI_WDT_HW_ANOMALY_RESOLVED
    if (pADI_WDT->T3STA & T3STA_LOCK)
        return ADI_WDT_ERR_LOCKED;
#endif

    /* wait on WDT sync if requested */
    if (hDevice->bBusSyncWait) {
        while (WDT_SYNC_PENDING) ;
    } else {
        if (WDT_SYNC_PENDING)
            return ADI_WDT_SYNC_IN_PROGRESS;
    }

    pADI_WDT->T3LD = LoadCount;             /* Set the load value */

    return ADI_WDT_SUCCESS;
}


/*!
    @brief      Set the WDT Enable bit.

    @details    Enables the WDT and starts the timer counting,
                or Disables the watchdog after initial core startup.

    @param[in]  hDevice    Device handle obtained from adi_WDT_Init().
    @param[in]  bEnable    Indicates the desired setting of the ENABLE bit
                           in the T3CON register.
                - True - Enable.
                - False - Disable.

    @return     Status
                - #ADI_WDT_SUCCESS if mode set as requested.
                - #ADI_WDT_SYNC_IN_PROGRESS WDT Bus-sync in progress.
                - #ADI_WDT_ERR_LOCKED if the timer is "locked" (see: #adi_WDT_GetLockedStatus()).
                - #ADI_WDT_ERR_NOT_INITIALIZED [D] if watchdog not initialized.
                - #ADI_WDT_ERR_BAD_DEV_DATA [D] if watchdog handle is corrupt.

    @warning    The watchdog can only be disabled once directly after core
                startup.  This allows re-configuration from startup default
                and re-start.

                Once re-enabled, the WDT is "locked" and no
                further changes to the watchdog configuration
                can be made (to prevent accidental modification
                of the WDT).

    @note       If wait mode of the WDT is set to true, this API will
                block until the bus-sync has completed during the register write.

    @sa         adi_WDT_SetWaitMode().
    @sa         adi_WDT_GetLockedStatus().
*/
ADI_WDT_RESULT_TYPE adi_WDT_SetEnable(ADI_WDT_DEV_HANDLE const hDevice, const bool_t bEnable)
{
#ifdef ADI_DEBUG
    /* ensure handle points to a valid watchdog device ... */
    if (ADI_WDT_INVALID_HANDLE(hDevice))
        return ADI_WDT_ERR_BAD_DEV_DATA;

    /* check that watchdog device is initialized ... */
    if (ADI_WDT_STATE_INITIALIZED != hDevice->InitState)
        return ADI_WDT_ERR_NOT_INITIALIZED;
#endif

    /* ensure that the watchdog is not locked out ... */
#ifdef ADI_WDT_HW_ANOMALY_RESOLVED
    if (pADI_WDT->T3STA & T3STA_LOCK)
       return ADI_WDT_ERR_LOCKED;
#endif

    /* wait on WDT sync if requested */
    if (hDevice->bBusSyncWait) {
        while (WDT_SYNC_PENDING) ;
    } else {
        if (WDT_SYNC_PENDING)
            return ADI_WDT_SYNC_IN_PROGRESS;
    }

    ADI_ENTER_CRITICAL_REGION();
    if (bEnable) {
        /* set periodic mode and enable bits */
        pADI_WDT->T3CON |= (T3CON_MOD_PERIODIC | T3CON_ENABLE);
    } else {
        /* Clear the enable bit to disable the timer */
        pADI_WDT->T3CON &= (~T3CON_ENABLE);
    }
    ADI_EXIT_CRITICAL_REGION();

    /* NOTE: This code assumes that the user does not need to know when the enable or disable
             actually occurs as a result of the write to T3CON, hence this does not do a bus-sync
             check after the write.  Once enabled, it cannot be disabled, or reconfigured, so its
             irrelevant and they do not need to wait for the sync to occur */

    return ADI_WDT_SUCCESS;
}


/*!
    @brief      Set the WDT prescale value.

    @details    Used to set the clock prescale value which in conjunction
                with the Load Count determines the overall timeout of the
                watchdog.

    @param[in]  hDevice    Device handle obtained from adi_WDT_Init().
    @param[in]  Prescale    Required prescale value.

    @return     Status
                - #ADI_WDT_SUCCESS if mode set as requested.
                - #ADI_WDT_SYNC_IN_PROGRESS WDT Bus-sync in progress.
                - #ADI_WDT_ERR_LOCKED if the timer is "locked" (see: #adi_WDT_GetLockedStatus()).
                - #ADI_WDT_ERR_NOT_INITIALIZED [D] if watchdog not initialized.
                - #ADI_WDT_ERR_BAD_PRESCALE if prescale value is invalid.
                - #ADI_WDT_ERR_BAD_DEV_DATA [D] if watchdog handle is corrupt.

    @note Example timer resolution and range given a timer clock of 3,2768 Hz:
        - if scale = 1
            - freq = 3,2768 Hz
            - min timeout = 1/32768 = 30.5us
            - max timeout = 65535/32768 = ~2 s
        - if scale = 16
            - freq = 2,048 Hz
            - min timeout = 1/2048 = ~488usec
            - max timeout = 65535/2048 = ~32 s
        - if scale = 256
            - freq = 128 Hz
            - min timeout = 1/128 = ~7.8ms
            - max timeout = 65535/128 = ~512 s = ~8.5 minutes
        - if scale = 4,096
            - freq = 8 Hz
            - min timeout = 1/8 = 125ms
            - max timeout = 65535/8 = ~8192 s = ~2:16.5 hours

    @sa         adi_WDT_GetPrescale().
*/
ADI_WDT_RESULT_TYPE adi_WDT_SetPrescale(ADI_WDT_DEV_HANDLE const hDevice, const ADI_WDT_PRESCALE_TYPE Prescale)
{
	uint16_t tcon;

#ifdef ADI_DEBUG
    /* ensure handle points to a valid watchdog device ... */
    if (ADI_WDT_INVALID_HANDLE(hDevice))
        return ADI_WDT_ERR_BAD_DEV_DATA;

    /* check that watchdog device is initialized ... */
    if (ADI_WDT_STATE_INITIALIZED != hDevice->InitState)
        return ADI_WDT_ERR_NOT_INITIALIZED;
#endif

    /* ensure that the watchdog is not locked out ... */
#ifdef ADI_WDT_HW_ANOMALY_RESOLVED
    if (pADI_WDT->T3STA & T3STA_LOCK)
        return ADI_WDT_ERR_LOCKED;
#endif

    switch (Prescale) {

        case ADI_WDT_PRESCALE_1:
        case ADI_WDT_PRESCALE_16:
        case ADI_WDT_PRESCALE_256:
        case ADI_WDT_PRESCALE_4096:

            /* wait on WDT sync if requested */
            if (hDevice->bBusSyncWait) {
                while (WDT_SYNC_PENDING) ;
            } else {
                if (WDT_SYNC_PENDING)
                    return ADI_WDT_SYNC_IN_PROGRESS;
            }

            /* critical section */
            ADI_ENTER_CRITICAL_REGION();
				tcon = pADI_WDT->T3CON;      /* temp copy */
				tcon &= ~(T3CON_PRE_MSK);    /* make a hole */
				tcon |= Prescale;            /* rollin new prescale */
				pADI_WDT->T3CON = tcon;      /* write it back */
    		ADI_EXIT_CRITICAL_REGION();

            break;

        default:
            return ADI_WDT_ERR_BAD_PRESCALE;    /* invalid prescale value specified */
    }

    return ADI_WDT_SUCCESS;
}


/*!
    @brief      Set the WDT interrupt mode on/off.

    @details    Used to enable the IRQ mode in the timer to generate
                an interrupt \a instead of generating a processor reset.
                Useful during debugging, where a reset would
                destroy internal state necessary for diagnostics.

    @param[in]  hDevice    Device handle obtained from adi_WDT_Init().
    @param[in]  bIRQMode   Desired IRQ Mode.
                - True - Generate an interrupt instead of reset.
                - False - Generate a processor reset.

    @return     Status
                - #ADI_WDT_SUCCESS if mode set as requested.
                - #ADI_WDT_SYNC_IN_PROGRESS WDT Bus-sync in progress.
                - #ADI_WDT_ERR_LOCKED if the timer is "locked" (see: #adi_WDT_GetLockedStatus()).
                - #ADI_WDT_ERR_NOT_INITIALIZED if watchdog not initialized.
                - #ADI_WDT_ERR_BAD_DEV_DATA [D] if watchdog handle is corrupt.

    @note       This API will also enable/disable the WDT interrupt, as appropriate.

    @warning    The application is recommended to override the weak-bound default \a WDog_Tmr_Int_Handler()
                interrupt handler to process the interrupt as desired, otherwise the default
                handler will be used which simply traps.

    @sa         adi_WDT_GetIRQMode().
*/
ADI_WDT_RESULT_TYPE adi_WDT_SetIRQMode(ADI_WDT_DEV_HANDLE const hDevice, const bool_t bIRQMode)
{
#ifdef ADI_DEBUG
    /* ensure handle points to a valid watchdog device ... */
    if (ADI_WDT_INVALID_HANDLE(hDevice))
        return ADI_WDT_ERR_BAD_DEV_DATA;

    /* check that watchdog device is initialized ... */
    if (ADI_WDT_STATE_INITIALIZED != hDevice->InitState)
        return ADI_WDT_ERR_NOT_INITIALIZED;
#endif

    /* ensure that the watchdog is not locked out ... */
#ifdef ADI_WDT_HW_ANOMALY_RESOLVED
   if (pADI_WDT->T3STA & T3STA_LOCK)
       return ADI_WDT_ERR_LOCKED;
#endif

    /* wait on WDT sync if requested */
    if (hDevice->bBusSyncWait) {
        while (WDT_SYNC_PENDING) ;
    } else {
        if (WDT_SYNC_PENDING)
            return ADI_WDT_SYNC_IN_PROGRESS;
    }

    ADI_ENTER_CRITICAL_REGION();
    if (bIRQMode) {
        /* set the IRQ bit in T3CON to generate an interrupt */
        pADI_WDT->T3CON |= T3CON_IRQ;
        /* enable the watchdog interrupt */
        ADI_ENABLE_INT(hDevice->IRQn);
    } else {
        /* clear the IRQ bit in T3CON to generate a reset */
        pADI_WDT->T3CON &= (~T3CON_IRQ);
        /* Disable the watchdog interrupt */
        ADI_DISABLE_INT(hDevice->IRQn);
    }
    ADI_EXIT_CRITICAL_REGION();

    return ADI_WDT_SUCCESS;
}


/*!
    @brief      Set the WDT power-down clear mode.

    @details    Sets the behaviour of the timer upon exiting from a power-down mode.

    @param[in]  hDevice    Device handle obtained from adi_WDT_Init().
    @param[in]  bPDClear    Required power-down mode.
                - True - Reset and restart the timer.
                - False - Continue from last-used values.

    @return     Status
                - #ADI_WDT_SUCCESS if mode set as requested.
                - #ADI_WDT_SYNC_IN_PROGRESS WDT Bus-sync in progress.
                - #ADI_WDT_ERR_LOCKED if the timer is "locked" (see: #adi_WDT_GetLockedStatus()).
                - #ADI_WDT_ERR_NOT_INITIALIZED if watchdog not initialized.
                - #ADI_WDT_ERR_BAD_DEV_DATA [D] if watchdog handle is corrupt.

    @note       If set to "True" then the counter will be reloaded from
                the current value stored in the load register.

    @sa         adi_WDT_GetPDClear().
*/
ADI_WDT_RESULT_TYPE adi_WDT_SetPDClear(ADI_WDT_DEV_HANDLE const hDevice, const bool_t bPDClear)
{
#ifdef ADI_DEBUG
    /* ensure handle points to a valid watchdog device ... */
    if (ADI_WDT_INVALID_HANDLE(hDevice))
        return ADI_WDT_ERR_BAD_DEV_DATA;

    /* check that watchdog device is initialized ... */
    if (ADI_WDT_STATE_INITIALIZED != hDevice->InitState)
        return ADI_WDT_ERR_NOT_INITIALIZED;
#endif

    /* ensure that the watchdog is not locked out ... */
#ifdef ADI_WDT_HW_ANOMALY_RESOLVED
    if (pADI_WDT->T3STA & T3STA_LOCK)
        return ADI_WDT_ERR_LOCKED;
#endif

    /* wait on WDT sync if requested */
    if (hDevice->bBusSyncWait) {
        while (WDT_SYNC_PENDING) ;
    } else {
        if (WDT_SYNC_PENDING)
            return ADI_WDT_SYNC_IN_PROGRESS;
    }

    ADI_ENTER_CRITICAL_REGION();
    if (bPDClear) {
        /* Set the PD bit in T3CON to restart the timer */
        pADI_WDT->T3CON |= T3CON_PD;
    } else {
        /* clear the PD bit in T3CON to continue with previous values */
        pADI_WDT->T3CON &= (~T3CON_PD);
    }
    ADI_EXIT_CRITICAL_REGION();

    return ADI_WDT_SUCCESS;
}


/*!
    @brief      Set the WDT bus-sync wait mode.

    @details    Sets the behaviour of writes to the T3CON, T3LD and T3CLRI registers,
                particularly when enabling, or clearing the timer.

    @param[in]  hDevice    Device handle obtained from adi_WDT_Init().
    @param[in]  bWaitMode   Required bus-sync wait.
				- True - Wait for Bus Sync to complete (APIs block).
                - False - Do not wait (APIs return immediately).

    @return     Status
                - #ADI_WDT_SUCCESS if mode set as requested.
                - #ADI_WDT_ERR_NOT_INITIALIZED if watchdog not initialized.
                - #ADI_WDT_ERR_BAD_DEV_DATA [D] if watchdog handle is corrupt.

    @note       If set to "False" then the affected APIs will always
                return the status code #ADI_WDT_SYNC_IN_PROGRESS if a sync is in
                progress to inform the caller that the action due to the write
                is delayed.

    @sa         adi_WDT_SetEnable().
    @sa         adi_WDT_ResetTimer().
    @sa         adi_WDT_Trigger().
*/
ADI_WDT_RESULT_TYPE adi_WDT_SetWaitMode(ADI_WDT_DEV_HANDLE const hDevice, const bool_t bWaitMode)
{
#ifdef ADI_DEBUG
    /* ensure handle points to a valid watchdog device ... */
    if (ADI_WDT_INVALID_HANDLE(hDevice))
        return ADI_WDT_ERR_BAD_DEV_DATA;

    /* check that watchdog device is initialized ... */
    if (ADI_WDT_STATE_INITIALIZED != hDevice->InitState)
        return ADI_WDT_ERR_NOT_INITIALIZED;
#endif

    hDevice->bBusSyncWait = bWaitMode;

    return ADI_WDT_SUCCESS;
}


/*!
    @brief      Get the WDT device Locked Status.

    @details    Return the status bit indicating if the timer has been
                started and configuration changes are locked out.

    @param[in]  hDevice    Device handle obtained from adi_WDT_Init().

    @return     Current state of the LOCK bit in the T3STA register.
                - True - Watchdog is locked.
                - False - Watchdog can be configured.

    @note       A processor restart is required to return the timer to
                a non-locked state such that changes to the configuration
                can be made.

    @sa         adi_WDT_SetEnable().
*/
bool_t adi_WDT_GetLockedStatus(ADI_WDT_DEV_HANDLE const hDevice)
{
    /* determine if the watchdog is locked out ... */
    return (pADI_WDT->T3STA & T3STA_LOCK)?true:false;
}


/*!
    @brief      Get the WDT Bus Sync Status for T3CON register.

    @details    Determine if a bus sync is still in progress
                after a write to T3CON.  This is only important
                to ensure that the writes to configuration
                is complete before enabling the timer.

    @param[in]  hDevice    Device handle obtained from adi_WDT_Init().

    @return     The contents of the CON bit in the T3STA register.
                - True - bus-sync in progress.
                - False - bus-sync complete.

    @note       This API is always successful.

    @sa         adi_WDT_SetEnable().
*/
bool_t adi_WDT_GetCONSyncStatus(ADI_WDT_DEV_HANDLE const hDevice)
{
    /* if the CON bit is set in the T3STA register,
       then a bus-write-sync is in progress */
    return (pADI_WDT->T3STA & T3STA_CON)?true:false;
}


/*!
    @brief      Get the WDT Bus Sync Status for T3LD register.

    @details    Determine if a bus sync is still in progress
                after a write to T3LD.  This is only important
                to ensure that the writes to configuration
                is complete before enabling the timer, or
                resetting the timer.

    @param[in]  hDevice    Device handle obtained from adi_WDT_Init().

    @return     The contents of the LD bit in the T3STA register.
                - True - bus-sync in progress.
                - False - bus-sync complete.

    @note       This API is always successful.

    @sa         adi_WDT_SetEnable().
    @sa         adi_WDT_ResetTimer().
*/
bool_t adi_WDT_GetLDSyncStatus(ADI_WDT_DEV_HANDLE const hDevice)
{
    /* if the LD bit is set in the T3STA register,
       then a bus-write-sync is in progress */
    return (pADI_WDT->T3STA & T3STA_LD)?true:false;
}


/*!
    @brief      Get the WDT Bus Sync Status for T3CLRI register.

    @details    Determine if a bus sync is still in progress
                after a write to T3CLRI.  This is only important
                to ensure that the writes to the CLRI register
                is complete before the timer becomes reset or
                triggered.

    @param[in]  hDevice    Device handle obtained from adi_WDT_Init().

    @return     The contents of the CLRI bit in the T3STA register.
                - True - bus-sync in progress.
                - False - bus-sync complete.

    @note       This API is always successful.

    @sa         adi_WDT_Trigger().
    @sa         adi_WDT_ResetTimer().
*/
bool_t adi_WDT_GetCLRISyncStatus(ADI_WDT_DEV_HANDLE const hDevice)
{
    /* if the CLRI bit is set in the T3STA register,
       then a bus-write-sync is in progress */
    return (pADI_WDT->T3STA & T3STA_CLRI)?true:false;
}


/*!
    @brief      Get the WDT IRQ Pending Status.

    @details    Determine if the WDT has an interrupt pending.

    @param[in]  hDevice    Device handle obtained from adi_WDT_Init().

    @return     Current state of the IRQ bit of the T3STA register.
                - True - interrupt is pending.
                - False - interrupt is not pending.

    @note       This API is always successful.
*/
bool_t adi_WDT_GetIRQPendStatus(ADI_WDT_DEV_HANDLE const hDevice)
{
    /* determine if an IRQ is pending ... */
    return (pADI_WDT->T3STA & T3STA_IRQ)?true:false;
}


/*!
    @brief      Get WDT current count value.

    @details    Obtains the count remaining in the WDT.

    @param[in]  hDevice    Device handle obtained from adi_WDT_Init().

    @return     The 16-bit contents of the T3VAL, current count register.

    @note       The actual time remaining is a factor of this value, the clock rate
                and the clock prescale value currently in use.

    @note       This API is always successful.

    @sa         adi_WDT_SetLoadCount().
    @sa         adi_WDT_SetPrescale().
*/
uint16_t adi_WDT_GetCount(ADI_WDT_DEV_HANDLE const hDevice)
{
    /* determine the remaining count in the timer by reading
       the value of the T3VAL register ... */
    return (pADI_WDT->T3VAL);
}


/*!
    @brief      Get the WDT device enable setting.

    @details    Determine if the timer is enabled and running.
                This is used in conjunction with #adi_WDT_GetLockedStatus()
                to determine if the time is running in the initial
                startup state.  Can also be used to determine if the
                timer is disabled and can be reconfigured.

    @param[in]  hDevice    Device handle obtained from adi_WDT_Init().

    @return     Current State of the ENABLE bit in the T3CON register.
                - True - timer is enabled.
                - False - timer is disabled.

    @note       This API is always successful.

    @sa         adi_WDT_GetLockedStatus().
*/
bool_t adi_WDT_GetEnable(ADI_WDT_DEV_HANDLE const hDevice)
{
    /* determine if the watchdog has been enabled ... */
    return (pADI_WDT->T3CON & T3CON_ENABLE)?true:false;
}


/*!
    @brief      Get the WDT clock prescale value.

    @details    Determine the current clock prescale value
                in use by the timer.  This is used in conjunction
                with the current count and the clock frequency
                to determine the timeout.

    @param[in]  hDevice    Device handle obtained from adi_WDT_Init().

    @return     One of
                - #ADI_WDT_PRESCALE_1 - clock not scaled.
                - #ADI_WDT_PRESCALE_16 - clock/16.
                - #ADI_WDT_PRESCALE_256 - clock/256.
                - #ADI_WDT_PRESCALE_4096 - clock/4096.

    @note       This API is always successful.

    @sa         adi_WDT_SetPrescale().
*/
ADI_WDT_PRESCALE_TYPE adi_WDT_GetPrescale(ADI_WDT_DEV_HANDLE const hDevice)
{
    /* extract the pre-scale value from the T3CON register */
    return (ADI_WDT_PRESCALE_TYPE)(pADI_WDT->T3CON & T3CON_PRE_MSK);
}


/*!
    @brief      Get the WDT interrupt mode.

    @details    Determine whether the timer is set for
                reset operation (normal) or interrupt generation.

    @param[in]  hDevice    Device handle obtained from adi_WDT_Init().

    @return     Current state of the IRQ bit in the T3CON register.
                - True - Generate an interrupt instead of reset.
                - False - Generate a processor reset.

    @note       This API is always successful.

    @sa         adi_WDT_SetIRQMode().
*/
bool_t adi_WDT_GetIRQMode(ADI_WDT_DEV_HANDLE const hDevice)
{
    /* determine the current IRQ mode ...
        if bit set, interrupt mode, else reset mode */
    return (pADI_WDT->T3CON & T3CON_IRQ)?true:false;
}


/*!
    @brief      Get the WDT Power Down Clear mode

    @details    Obtain the current setting of the timer
                behaviour after a return from a power down cycle.

    @param[in]  hDevice    Device handle obtained from adi_WDT_Init().

    @return     Current state of the PD bit in the T3CON register.
                - True - Reset and restart the timer.
                - False - Continue from last-used values.

    @note       This API is always successful.

    @sa         adi_WDT_SetPDClear().
*/
bool_t adi_WDT_GetPDClear(ADI_WDT_DEV_HANDLE const hDevice)
{
    /* determine the current power-down clear mode ...
        if bit set, restart, else continue */
    return (pADI_WDT->T3CON & T3CON_PD)?true:false;
}


/*!
    @brief      Get the WDT Bus-sync wait mode.

    @details    Obtain the current setting of the bus-sync
                wait to determine the blocking behavoior of affected APIs.

    @param[in]  hDevice    Device handle obtained from adi_WDT_Init().

    @return     Current state of the wait mode.
                - True - wait for bus sync to complete (APIs block).
                - False - don't wait for bus-sync (APIs return immediately).

    @note       This API is always successful.

    @sa         adi_WDT_SetEnable().
    @sa         adi_WDT_Trigger().
    @sa         adi_WDT_ResetTimer().
*/
bool_t adi_WDT_GetWaitMode(ADI_WDT_DEV_HANDLE const hDevice)
{
    return hDevice->bBusSyncWait;

}


/*!
    @brief      Reset the WDT.

    @details    Clears the counter and restarts the timer using the
                pre-configured values. Also clears IRQ if interrupt enabled.
                This is the API used to "kick" the watchdog to prevent
                the timeout and reset of the processor.  This is the main
                device API that holds off the WDT from expiring and should be
                called periodically to refresh the WDT so it does not expire.

    @param[in]  hDevice    Device handle obtained from adi_WDT_Init().

    @return     Status
                - #ADI_WDT_SUCCESS if watchdog successfully reset.
                - #ADI_WDT_SYNC_IN_PROGRESS WDT Bus-sync in progress.
                - #ADI_WDT_ERR_NOT_INITIALIZED if watchdog not initialized.
                - #ADI_WDT_ERR_BAD_DEV_DATA [D] if watchdog handle is corrupt.

    @warning    If this API is not called periodically, before the timer
                expires, then the processor will reset (or generate an
                interrupt if in IRQ mode).

    @note       If watchdog bus-sync wait mode flag is set to true, this API
                will block until the bus-sync has completed during the
                register write.

    @sa         adi_WDT_GetIRQMode().
*/
ADI_WDT_RESULT_TYPE adi_WDT_ResetTimer(ADI_WDT_DEV_HANDLE const hDevice)
{
#ifdef ADI_DEBUG
    /* ensure handle points to a valid watchdog device ... */
    if (ADI_WDT_INVALID_HANDLE(hDevice))
        return ADI_WDT_ERR_BAD_DEV_DATA;

    /* check that watchdog device is initialized ... */
    if (ADI_WDT_STATE_INITIALIZED != hDevice->InitState)
        return ADI_WDT_ERR_NOT_INITIALIZED;
#endif

    /* wait on WDT sync if requested */
    if (hDevice->bBusSyncWait) {
        while (WDT_SYNC_PENDING) ;
    } else {
        if (WDT_SYNC_PENDING)
            return ADI_WDT_SYNC_IN_PROGRESS;
    }

    /* Reset the timer (i.e. kick the dog) */
    pADI_WDT->T3CLRI = T3CLRI_VALUE_CLR;

    return ADI_WDT_SUCCESS;
}


/*!
    @brief      Force an immediate a WDT Reset/Interrupt sequence.

    @details    This API can be used to invoke an immediate reset of the
                processor, or to cause a watchdog interrupt if in IRQ mode.

    @param[in]  hDevice    Device handle obtained from adi_WDT_Init().

    @return     Status
                - #ADI_WDT_SUCCESS if the trigger of the interrupt was
                    successful (in IRQMode).
                - #ADI_WDT_SYNC_IN_PROGRESS WDT Bus-sync in progress.
                - #ADI_WDT_ERR_NOT_INITIALIZED if watchdog not initialized.
                - #ADI_WDT_ERR_BAD_DEV_DATA [D] if watchdog handle is corrupt.

    @note       If watchdog bus-sync wait mode is set to true, this API will block
                until the bus-sync has completed during the register write.
                If the watchdog is in reset mode, then this function will not
                return if successful.

    @sa         adi_WDT_GetIRQMode().
*/
ADI_WDT_RESULT_TYPE adi_WDT_Trigger(ADI_WDT_DEV_HANDLE const hDevice)
{
#ifdef ADI_DEBUG
    /* ensure handle points to a valid watchdog device ... */
    if (ADI_WDT_INVALID_HANDLE(hDevice))
        return ADI_WDT_ERR_BAD_DEV_DATA;

    /* check that watchdog device is initialized ... */
    if (ADI_WDT_STATE_INITIALIZED != hDevice->InitState)
        return ADI_WDT_ERR_NOT_INITIALIZED;
#endif

    /* wait on WDT sync if requested */
    if (hDevice->bBusSyncWait) {
        while (WDT_SYNC_PENDING) ;
    } else {
        if (WDT_SYNC_PENDING)
            return ADI_WDT_SYNC_IN_PROGRESS;
    }

    /* NOTE: a write of any value other than T3CLRI_VALUE_CLR to the
             T3CLRI register causes a watchdog reset or IRQ */
    pADI_WDT->T3CLRI = 0;       /* trigger the timer reset or IRQ */

    return ADI_WDT_SUCCESS;
}


/*! @brief  WDT device driver interrupt handler.  Overrides weakly-bound default interrupt handler in startup.c. */
ADI_INT_HANDLER (WDog_Tmr_Int_Handler)
{
    /* reset WDT */
    adi_WDT_ResetTimer(&WDT_DevData[ADI_WDT_DEVID_0]);
}


#define REVERT_ADI_MISRA_SUPPRESSIONS  /*!< Revert ADI MISRA Suppressions */
#include "misra.h"

/*
** EOF
*/

/*@}*/
