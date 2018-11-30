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
 * @file:    rtc.c
 * @brief:   Real-Time Clock Device Implementations for ADuCxxx
 * @version: $Revision: 30127 $
 * @date:    $Date: 2015-03-09 12:05:31 -0400 (Mon, 09 Mar 2015) $
*****************************************************************************/

/*! \addtogroup RTC_Driver RTC Driver
 *  @{
 */

#include <stdlib.h>  /* for 'NULL" definition */

#define ASSERT_ADI_MISRA_SUPPRESSIONS  /*!< Apply ADI MISRA Suppressions */
#include "misra.h"

#include "rtc.h"
#include "startup.h"             /* Obtain definition of WEAK */
#include "adi_rtos.h"

/* control register function enable bits */
#define ADI_RTC_CONTROL_DEVICE_ENABLE        (1 << 0) /*!< RTC control register enable bit */
#define ADI_RTC_CONTROL_ALARM_ENABLE         (1 << 1) /*!< RTC control register enable bit */
#define ADI_RTC_CONTROL_TRIM_ENABLE          (1 << 3) /*!< RTC control register enable bit */
#define ADI_RTC_CONTROL_LCD_UPDATE_ENABLE    (1 << 4) /*!< RTC control register enable bit */


/* LCD Update defines */
#define ADI_RTC_LCD_UPDATE_BPOS              (5)                                  /*!< LCD update field position */
#define ADI_RTC_LCD_UPDATE_MASK              (0x3f << ADI_RTC_LCD_UPDATE_BPOS)    /*!< LCD update field mask */


/*! \struct ADI_RTC_DEV_DATA_TYPE
 * RTC Device instance data structure */
typedef struct ADI_RTC_DEV_DATA_TYPE
{
    bool_t                                   bInitialized;    /*!< Initialization state        */
    ADI_RTC_TypeDef                         *pRtc;            /*!< MMR address for this RTC    */
    IRQn_Type                                IRQn;            /*!< RTC interrupt number        */
    bool_t                                   bSafe;           /*!< Stall on pending writes     */
    ADI_CALLBACK                             cbFunc;          /*!< Callback function           */
    ADI_RTC_INT_SOURCE_TYPE                  cbWatch;         /*!< Callback watch bits         */
} ADI_RTC_DEV_DATA_TYPE;


/* allocate instance data */
ADI_RTC_DEV_DATA_TYPE gRTC_DevData[ADI_RTC_MAX_DEVID] = {0};  /*!< Private RTC device driver instance data */


/* debug helpers */
#ifdef ADI_DEBUG
#define ADI_RTC_INVALID_HANDLE(h)            ((&gRTC_DevData[0] != h))
#endif

#define ADI_RTC_FAILSAFE                     (pADI_RTC->RTCSR0 & ADI_RTC_INT_SOURCE_FAIL)

/*!
    @brief RTC Initialization

    @param[in] InstanceNum  #ADI_RTC_DEV_ID_TYPE specifying the instance of the RTC to use.
    @param[in] phDevice     Pointer to a location the device handle will be written upon
                            successful driver initialization.
    @return    Status
               - #ADI_RTC_SUCCESS                      Success: RTC device driver initialized successfully.
               - #ADI_RTC_ERR_CLOCK_FAILSAFE           Error: Clock is unreliable.
               - #ADI_RTC_ERR_ALREADY_INITIALIZED [D]  Error: The RTC is already initialized.
               - #ADI_RTC_ERR_BAD_INSTANCE [D]         Error: The instance number is invalid.

            The RTC controller interrupt enable state is unaltered during driver initialization.
            Use the #adi_RTC_EnableInterrupts API to manage interrupting.

    @note   The contents of phDevice will be set to NULL upon failure.\n\n

    @note   On #ADI_RTC_SUCCESS the RTC device driver is initialized and made ready for use,
            though pending interrupts may be latched.  During initialization, the content of the
            various RTC control, count, alarm and status registers are untouched to preserve prior
            RTC initializations and operation.  The core NVIC RTC interrupt is enabled.\n\n

    @note   On #ADI_RTC_ERR_CLOCK_FAILSAFE, the clock is unreliable and the driver will not initialize
            until the failsafe condition is cleared with #adi_RTC_ClearFailSafe() API.  Following the
            #adi_RTC_ClearFailSafe() call, the application should reinitialize the RTC driver and
            program the RTC clock to a meaningful value.

    @note   The RTC device driver may initialize with pending interrupt status which may require
            processing at the application level before using the RTC.  The application should
            the #adi_RTC_GetInterruptStatus() API to query the RTC interrupt status detail and
            reconcile any pending interrupts before using the RTC.  After processing the pending
            interrupts, the application should explicitly clear the corresponding interrupt status
            bits(s) with the #adi_RTC_ClearInterruptStatus() API.\n\n

    @note   SAFE WRITES: The "safe write" mode is enabled by default during RTC device driver
            initialization, see #adi_RTC_EnableSafeWrites().\n\n

    @note   CALLBACKS: RTC interrupt callbacks are \a disabled by default during RTC device driver
            initialization (#adi_RTC_Init()).  The application may use the #adi_RTC_RegisterCallback()
            API to register an application-provided callback function with the RTC device driver.  The
            callback registration API takes an interrupt mask (of type #ADI_RTC_INT_SOURCE_TYPE)
            designating which interrupts to watch.  When any of the watched interrupts occur, the
            RTC device driver will make the callback to the application for application-based
            interrupt handling.  The RTC device driver clears the interrupt when the callback exits.
            The application callback should avoid extended processing during the callback as the callback
            is made context of the initiating interrupt and will block lower-priority interrupts.
            If extended application-level interrupt processing is required, the application should
            schedule it for the main application loop and exit the callback as soon as possible.\n\n

    @note   FAILSAFE: In the case where the RTC power has seen a complete and utter power failure
            (i.e., a full loss of battery power and a full loss of reserve capacitive power) or
            this is a first-time device activation, the RTCFAIL interrupt is set and latched upon
            restoration or first application of power.  This is a \a failsafe state during which
            the RTC clock is unreliable and requires setting.  The failsafe interrupt is latched
            and is not cleared until the application either sets the clock count through the
            #adi_RTC_SetCount() API (which clears the interrupt implicitly) or the application
            clears the interrupt explicitly through the #adi_RTC_ClearInterruptStatus() API.\n\n

    @note   In \b debug mode (ADI_DEBUG macro is asserted), the failsafe state is \b monitored
            by each RTC API (excluding #adi_RTC_Init(), #adi_RTC_SetCount() and
            #adi_RTC_ClearInterruptStatus() APIs).  When the failsafe state is detected, the failsafe
            error code (#ADI_RTC_ERR_CLOCK_FAILSAFE) is returned to the application and RTC driver
            functionality is disabled until the RTC counter is set or until the interrupt is cleared
            manually (via the #adi_RTC_ClearInterruptStatus() API).\n\n

    @note   In \b non-debug mode (ADI_DEBUG macro is not asserted), the failsafe state is \b not
            \b monitored and the failsafe return status is not produced.  This reduces device driver
            footprint and performance overhead in deployed applications.  Also, non-debug builds
            preserve full device driver functionality despite the failsafe state.  If desired, the
            application may dynamically monitor/process the failsafe state in non-debug mode by:
            1) detecting and processing the return status given by the initial #adi_RTC_Init() call,
            and/or 2) registering an RTC interrupt callback (see #adi_RTC_RegisterCallback()),  and
            thereby receive application callback notification(s) when the RTCFAIL interrupt
            (#ADI_RTC_INT_SOURCE_FAIL) fires (among whatever other interrupt watches were set when
            the callback function is registered).\n\n

    @note   The latched failsafe interrupt state is cleared either implicitly by the #adi_RTC_SetCount()
            API (to set the clock), or explicitly by calling the #adi_RTC_ClearInterruptStatus() API to
            clear it manually.\n\n

    @note   The underlying RTCFAIL interrupt driving the failsafe state is a \b non-maskable interrupt
            that is unconditionally enabled to the CPU without dependency on the RTC enablement state.

    @sa    adi_RTC_EnableDevice().
    @sa    adi_RTC_EnableInterrupts().
    @sa    adi_RTC_EnableSafeWrites().
    @sa    adi_RTC_GetInterruptStatus().
    @sa    adi_RTC_RegisterCallback().
    @sa    adi_RTC_ClearFailSafe().
    @sa    adi_RTC_SetCount().
    @sa    adi_RTC_ClearInterruptStatus().
    @sa    adi_RTC_UnInit().
*/
ADI_RTC_RESULT_TYPE adi_RTC_Init(ADI_RTC_DEV_ID_TYPE InstanceNum, ADI_RTC_HANDLE*  phDevice)
{
    ADI_RTC_HANDLE hDevice;

    /* store a bad handle in case of failure */
    *phDevice = (ADI_RTC_HANDLE) NULL;

#ifdef ADI_DEBUG
    if (InstanceNum >= ADI_RTC_MAX_DEVID)
    {
        return ADI_RTC_ERR_BAD_INSTANCE;
    }
#endif

    /* local pointer to instance data */
    hDevice = &gRTC_DevData[InstanceNum];

#ifdef ADI_DEBUG
    if (hDevice->bInitialized)
    {
        return ADI_RTC_ERR_ALREADY_INITIALIZED;
    }
#endif

    /* make sure RTC has a clock so it can respond */
    /* do this even before failing out on RTCFAIL, */
    /* or application efforts to clear it with     */
    /* adi_RTC_ClearFailSafe will fail             */
    SystemClockSourcePowerUp (ADI_SYS_CLOCK_SOURCE_LFXTAL, ADI_SYS_CLOCK_TIMEOUT_USE_DEFAULT);

    /* fail on corrupt clock */
    if (ADI_RTC_FAILSAFE)
        return ADI_RTC_ERR_CLOCK_FAILSAFE;

    /* initialize device data entries */
    hDevice->pRtc    = pADI_RTC;
    hDevice->bSafe  = true;
    hDevice->cbFunc  = NULL;
    hDevice->cbWatch = (ADI_RTC_INT_SOURCE_TYPE) 0;
    hDevice->IRQn    = RTC_IRQn;

    /* disable all interrupt sources */
    while (hDevice->pRtc->RTCSR1 & ADI_RTC_WRITE_STATUS_CONTROL)
        ;
    hDevice->pRtc->RTCCR &= ~ADI_RTC_INT_ENA_MASK;

    /* clear any pending interrupts */
    while (hDevice->pRtc->RTCSR1 & ADI_RTC_WRITE_STATUS_STATUS0)
        ;
    hDevice->pRtc->RTCSR0 = ADI_RTC_INT_SOURCE_MASK;

    /* install interrupt handler (see abstraction macro for OS in adi_rtos.h) */
    ADI_INSTALL_HANDLER(hDevice->IRQn, RTC_Int_Handler);

    /* Enable RTC interrupts in NVIC */
    ADI_ENABLE_INT(hDevice->IRQn);

    /* mark driver initialized */
    hDevice->bInitialized = true;

    /* store handle at application handle pointer */
    *phDevice = hDevice;

    return ADI_RTC_SUCCESS;            // initialized
}


/* reset failsafe bit (RW1C) */
void adi_RTC_ClearFailSafe(void)
{
    // don't use device instance objects here, as they are not initialized yet... limit to global pointers
    while (pADI_RTC->RTCSR1 & ADI_RTC_WRITE_STATUS_STATUS0)
        ;
    pADI_RTC->RTCSR0 = ADI_RTC_INT_SOURCE_FAIL;
}


/*!
 * @brief  Uninitialize and deallocate an RTC device.
 *
 * @param[in]   hDevice    Device handle obtained from adi_RTC_Init().
 *
 * @return      Status
 *                - #ADI_RTC_SUCCESS                    Success: Call completed successfully.
 *                - #ADI_RTC_ERR_BAD_DEV_HANDLE [D]     Error: Invalid device handle parameter.
 *                - #ADI_RTC_ERR_CLOCK_FAILSAFE [D]     Error: A failsafe interrupt is pending, see #adi_RTC_Init().
 *                - #ADI_RTC_ERR_NOT_INITIALIZED [D]    Error: Device has not been initialized for use, see #adi_RTC_Init().
 *
 * Uninitialize and release an allocated RTC device for other use.  The core NVIC RTC interrupt is disabled.
 *
 * @sa        adi_RTC_Init().
 */
ADI_RTC_RESULT_TYPE adi_RTC_UnInit(ADI_RTC_HANDLE hDevice)
{

#ifdef ADI_DEBUG
    if (ADI_RTC_INVALID_HANDLE(hDevice))
        return ADI_RTC_ERR_BAD_DEV_HANDLE;

    if (!hDevice->bInitialized)
        return ADI_RTC_ERR_NOT_INITIALIZED;

    if (ADI_RTC_FAILSAFE)
        return ADI_RTC_ERR_CLOCK_FAILSAFE;
#endif

    /* uninitialize */
    ADI_DISABLE_INT(hDevice->IRQn);
    ADI_UNINSTALL_HANDLER(hDevice->IRQn);

    hDevice->bInitialized = false;
    hDevice->pRtc         = NULL;
    hDevice->IRQn         = (IRQn_Type) 0;
    hDevice->cbFunc       = NULL;
    hDevice->cbWatch      = (ADI_RTC_INT_SOURCE_TYPE)0;

    return ADI_RTC_SUCCESS;
}


/*************************************************************************************************
**************************************************************************************************
****************************************  ENABLE APIS  *******************************************
**************************************************************************************************
*************************************************************************************************/


/*!
 * @brief  Enable RTC alarm logic.
 *
 * @param[in]   hDevice    Device handle obtained from adi_RTC_Init().
 * @param[in]   bFlag      Flag controlling alarm logic.
 *                - #true     Enable alarm logic.
 *                - #false    Disable alarm logic.
 *
 * @return      Status
 *                - #ADI_RTC_SUCCESS                    Success: Call completed successfully.
 *                - #ADI_RTC_ERR_BAD_DEV_HANDLE [D]     Error: Invalid device handle parameter.
 *                - #ADI_RTC_ERR_CLOCK_FAILSAFE [D]     Error: A failsafe interrupt is pending, see #adi_RTC_Init().
 *                - #ADI_RTC_ERR_NOT_INITIALIZED [D]    Error: Device has not been initialized for use, see #adi_RTC_Init().
 *
 * Enable/disable operation of RTC internal alarm logic.
 *
 * Alarm events and interrupt notifications are gated by the alarm logic enablement.
 * RTC alarm interrupts require both RTC device and RTC alarm interrupt enables
 * to have been set.
 *
 * The alarm is relative to some future alarm value match against the RTC counter.
 *
 * @note The RTC device driver does not modify the alarm enable on the hardware except through use of this API.
 *
 * @sa        adi_RTC_Init().
 * @sa        adi_RTC_EnableDevice().
 * @sa        adi_RTC_EnableInterrupts().
 * @sa        adi_RTC_GetAlarm().
 * @sa        adi_RTC_GetCount().
 * @sa        adi_RTC_SetAlarm().
 * @sa        adi_RTC_SetCount().
 */
ADI_RTC_RESULT_TYPE adi_RTC_EnableAlarm(ADI_RTC_HANDLE hDevice, bool_t bFlag)
{

#ifdef ADI_DEBUG
    if (ADI_RTC_INVALID_HANDLE(hDevice))
        return ADI_RTC_ERR_BAD_DEV_HANDLE;

    if (!hDevice->bInitialized)
        return ADI_RTC_ERR_NOT_INITIALIZED;

    if (ADI_RTC_FAILSAFE)
        return ADI_RTC_ERR_CLOCK_FAILSAFE;
#endif

    /* pause on pending writes to CR to avoid data loss */
    if (hDevice->bSafe)
        while (hDevice->pRtc->RTCSR1 & ADI_RTC_WRITE_STATUS_CONTROL)
            ;

    ADI_ENTER_CRITICAL_REGION();
    /* set/clear RTC alarm enable */
    if (bFlag)
        hDevice->pRtc->RTCCR |= ADI_RTC_CONTROL_ALARM_ENABLE;
    else
        hDevice->pRtc->RTCCR &= ~ADI_RTC_CONTROL_ALARM_ENABLE;
    ADI_EXIT_CRITICAL_REGION();

    return ADI_RTC_SUCCESS;
}


/*!
 * @brief  Enable RTC device.
 *
 * @param[in]   hDevice    Device handle obtained from adi_RTC_Init().
 * @param[in]   bFlag      Flag controlling RTC device enablement.
 *                - #true     Enable RTC.
 *                - #false    Disable RTC.
 *
 * @return      Status
 *                - #ADI_RTC_SUCCESS                    Success: Call completed successfully.
 *                - #ADI_RTC_ERR_BAD_DEV_HANDLE [D]     Error: Invalid device handle parameter.
 *                - #ADI_RTC_ERR_CLOCK_FAILSAFE [D]     Error: A failsafe interrupt is pending, see #adi_RTC_Init().
 *                - #ADI_RTC_ERR_NOT_INITIALIZED [D]    Error: Device has not been initialized for use, see #adi_RTC_Init().
 *
 * Global enable/disable of the RTC controller.  Enables counting of elapsed real time and acts
 * as a master enable for the RTC.  If disabled, no time is counted, no alarm or LCD conditions
 * are detected, no alarm or LCD interrupts are issued, and no timer trimming is performed.
 *
 * @note When enabled, the RTC input clock prescaler, the trim interval and the LCD update counter are realigned.
 *
 * @note The RTC device driver does not modify the device enable on the hardware except through use of this API.
 *
 * @sa        adi_RTC_Init().
 * @sa        adi_RTC_EnableDevice().
 */
ADI_RTC_RESULT_TYPE adi_RTC_EnableDevice(ADI_RTC_HANDLE hDevice, bool_t bFlag)
{

#ifdef ADI_DEBUG
    if (ADI_RTC_INVALID_HANDLE(hDevice))
        return ADI_RTC_ERR_BAD_DEV_HANDLE;

    if (!hDevice->bInitialized)
        return ADI_RTC_ERR_NOT_INITIALIZED;

    if (ADI_RTC_FAILSAFE)
        return ADI_RTC_ERR_CLOCK_FAILSAFE;
#endif

    /* pause on pending writes to CR to avoid data loss */
    if (hDevice->bSafe)
        while (hDevice->pRtc->RTCSR1 & ADI_RTC_WRITE_STATUS_CONTROL)
            ;

    ADI_ENTER_CRITICAL_REGION();
    /* set/clear RTC device enable */
    if (bFlag)
        hDevice->pRtc->RTCCR |= ADI_RTC_CONTROL_DEVICE_ENABLE;
    else
        hDevice->pRtc->RTCCR &= ~ADI_RTC_CONTROL_DEVICE_ENABLE;
    ADI_EXIT_CRITICAL_REGION();

    return ADI_RTC_SUCCESS;
}


/*!
 * @brief  Manage interrupt enable/disable in the RTC and NVIC controller.
 *
 * @param[in]   hDevice    Device handle obtained from adi_RTC_Init().
 * @param[in]   Interrupts Conveys which interrupts are affected.
 * @param[in]   bFlag      Flag controlling RTC interrupt enablement.
 *                - #true     Enable RTC interrupts.
 *                - #false    Disable RTC interrupts.
 *
 * @return      Status
 *                - #ADI_RTC_SUCCESS                    Success: Call completed successfully.
 *                - #ADI_RTC_ERR_BAD_DEV_HANDLE [D]     Error: Invalid device handle parameter.
 *                - #ADI_RTC_ERR_CLOCK_FAILSAFE [D]     Error: A failsafe interrupt is pending, see #adi_RTC_Init().
 *                - #ADI_RTC_ERR_NOT_INITIALIZED [D]    Error: Device has not been initialized for use, see #adi_RTC_Init().
 *
 * Enable/disable RTC interrupt enable bits as well as manage global NVIC enable/disable for the RTC.
 * Input parameter \a Interrupts is a bitfield of type #ADI_RTC_INT_ENA_TYPE designating the set of
 * interrupts to be enabled or disabled.  The interrupts parameter may be zero, which will then simply
 * manage the NVIC RTC enable and leave the indivigual RTC interrupt enables unchanges.
 * Input parameter \a bFlag controls whether to enable or disable the designated set of interrupts.
 *
 * @note The RTC device driver does not modify the interrupt enables on the hardware except through use of this API.
 *
 * @sa        adi_RTC_Init().
 * @sa        adi_RTC_EnableDevice().
 * @sa        adi_RTC_GetInterruptStatus().
 */
ADI_RTC_RESULT_TYPE adi_RTC_EnableInterrupts (ADI_RTC_HANDLE hDevice, ADI_RTC_INT_ENA_TYPE Interrupts, bool_t bFlag)
{
#ifdef ADI_DEBUG
    if (ADI_RTC_INVALID_HANDLE(hDevice))
        return ADI_RTC_ERR_BAD_DEV_HANDLE;

    if (!hDevice->bInitialized)
        return ADI_RTC_ERR_NOT_INITIALIZED;

    if (ADI_RTC_FAILSAFE)
        return ADI_RTC_ERR_CLOCK_FAILSAFE;

    if (Interrupts & ~ADI_RTC_INT_ENA_MASK)
        return ADI_RTC_ERR_PARAM_OUT_OF_RANGE;
#endif

    /* pause on pending writes to CR to avoid data loss */
    if (hDevice->bSafe)
        while (hDevice->pRtc->RTCSR1 & ADI_RTC_WRITE_STATUS_CONTROL)
            ;

    /* set/clear interrupt enable bit(s) in control register */
    if (bFlag) {
        hDevice->pRtc->RTCCR |= Interrupts;
        ADI_ENABLE_INT(hDevice->IRQn);
    } else {
        ADI_DISABLE_INT(hDevice->IRQn);
        hDevice->pRtc->RTCCR &= ~Interrupts;
    }

    return ADI_RTC_SUCCESS;
}


/*!
 * @brief  Enable RTC LCD update event counting.
 *
 * @param[in]   hDevice    Device handle obtained from adi_RTC_Init().
 * @param[in]   bFlag      Flag controlling RTC LCD event counting.
 *                - #true     Enable RTC LCD update determination.
 *                - #false    Disable RTC LCD update determination.
 *
 * @return      Status
 *                - #ADI_RTC_SUCCESS                    Success: Call completed successfully.
 *                - #ADI_RTC_ERR_BAD_DEV_HANDLE [D]     Error: Invalid device handle parameter.
 *                - #ADI_RTC_ERR_CLOCK_FAILSAFE [D]     Error: A failsafe interrupt is pending, see #adi_RTC_Init().
 *                - #ADI_RTC_ERR_NOT_INITIALIZED [D]    Error: Device has not been initialized for use, see #adi_RTC_Init().
 *
 * Enable/disable counting of the RTC LCD update interval used by the RTC to generate one minute
 * LCD update intervals.  The LCD update count is a module-60 second counter referenced to any
 * second in a one minute interval of the main RTC counter.  When enabled, the LCD update logic
 * will detect and interrupt (if interrupting on LCD update is also enabled) on a match of the
 * main RTC timer against the LCD update reference point and update interval.  The LCD update
 * interval ranges between 0 and 59 seconds, relative to the main timer.
 *
 * To receive interrupts on the LCD interval event match, the corresponding LCD update interval
 * interrupt must also be enabled.
 *
 * @note The LCD update event reference point is reset with device enable, #adi_RTC_EnableDevice().
 *
 * @note The RTC device driver does not modify the LCD update enable on the hardware except through use of this API.
 *
 * @sa        adi_RTC_Init().
 * @sa        adi_RTC_EnableDevice().
 * @sa        adi_RTC_EnableInterrupts().
 * @sa        adi_RTC_GetLcdUpdate().
 * @sa        adi_RTC_SetLcdUpdate().
 */
ADI_RTC_RESULT_TYPE adi_RTC_EnableLcdUpdate (ADI_RTC_HANDLE hDevice, bool_t bFlag)
{
#ifdef ADI_DEBUG
    if (ADI_RTC_INVALID_HANDLE(hDevice))
        return ADI_RTC_ERR_BAD_DEV_HANDLE;

    if (!hDevice->bInitialized)
        return ADI_RTC_ERR_NOT_INITIALIZED;

    if (ADI_RTC_FAILSAFE)
        return ADI_RTC_ERR_CLOCK_FAILSAFE;
#endif

    /* pause on pending writes to CR to avoid data loss */
    if (hDevice->bSafe)
        while (hDevice->pRtc->RTCSR1 & ADI_RTC_WRITE_STATUS_CONTROL)
            ;

    ADI_ENTER_CRITICAL_REGION();
    /* set/clear interrupt enable bit(s) in control register */
    if (bFlag)
        hDevice->pRtc->RTCCR |= ADI_RTC_CONTROL_LCD_UPDATE_ENABLE;
    else
        hDevice->pRtc->RTCCR &= ~ADI_RTC_CONTROL_LCD_UPDATE_ENABLE;
    ADI_EXIT_CRITICAL_REGION();

    return ADI_RTC_SUCCESS;
}


/*!
 * @brief  Enable RTC safe write mode.
 *
 * @param[in]   hDevice    Device handle obtained from adi_RTC_Init().
 * @param[in]   bFlag      Flag controlling RTC safe write mode.
 *                - #true     Enable RTC safe writes.
 *                - #false    Disable RTC safe writes.
 *
 * @return      Status
 *                - #ADI_RTC_SUCCESS                    Success: Call completed successfully.
 *                - #ADI_RTC_ERR_BAD_DEV_HANDLE [D]     Error: Invalid device handle parameter.
 *                - #ADI_RTC_ERR_CLOCK_FAILSAFE [D]     Error: A failsafe interrupt is pending, see #adi_RTC_Init().
 *                - #ADI_RTC_ERR_NOT_INITIALIZED [D]    Error: Device has not been initialized for use, see #adi_RTC_Init().
 *

 * The safe write mode is asserted by default during each RTC device driver initialization (#adi_RTC_Init()).
 * The safe write mode is an RTC device driver software operational mode not manifest in the RTC hardware.
 *
 * The safe write mode insures any pending writes that have not yet synchronized between the faster core clock
 * domain and the internal RTC 32kHz clock domain are reconciled before multiple writes to the same RTC register
 * are allowed.  It pauses execution while pending writes to a register clear before allowing subsequent writes
 * to that same register.  The safe mode allows application code to not concern itself with synchronizing data
 * writes at the cost of slower response to register write requests.
 *
 * If the safe mode is disabled, it becomes the responsibility of the application to manage pending writes by
 * either: 1) using the #adi_RTC_GetWritePendStatus() and #adi_RTC_GetWriteSyncStatus() APIs to verify register
 * write readiness, or 2) using the #adi_RTC_SynchronizeAllWrites() API to force all pending writes to resolve.
 *
 * If the application disables the safe write mode and fails to synchronize pending writes, data loss will
 * result from multiple writes to the same RTC register twice in rapid succession.  In this case, a write error
 * will be latched and made available to the application through the #adi_RTC_EnableInterrupts() and
 * #adi_RTC_RegisterCallback() APIs.  Latched write error state may be queried at any time using the
 * #adi_RTC_GetWriteErrorSource() API and cleared using the #adi_RTC_ClearInterruptStatus() API.
 *
 * @sa        adi_RTC_Init().
 * @sa        adi_RTC_EnableInterrupts().
 * @sa        adi_RTC_GetWriteErrorSource().
 * @sa        adi_RTC_GetWritePendStatus().
 * @sa        adi_RTC_GetWriteSyncStatus().
 * @sa        adi_RTC_RegisterCallback().
 * @sa        adi_RTC_ClearInterruptStatus().
 */
ADI_RTC_RESULT_TYPE adi_RTC_EnableSafeWrites (ADI_RTC_HANDLE hDevice, bool_t bFlag)
{
#ifdef ADI_DEBUG
    if (ADI_RTC_INVALID_HANDLE(hDevice))
        return ADI_RTC_ERR_BAD_DEV_HANDLE;

    if (!hDevice->bInitialized)
        return ADI_RTC_ERR_NOT_INITIALIZED;

    if (ADI_RTC_FAILSAFE)
        return ADI_RTC_ERR_CLOCK_FAILSAFE;
#endif

    /* store flag */
    hDevice->bSafe = bFlag;

    return ADI_RTC_SUCCESS;
}


/*!
 * @brief  Enable RTC automatic clock trimming.
 *
 * @param[in]   hDevice    Device handle obtained from adi_RTC_Init().
 * @param[in]   bFlag      Flag controlling RTC trim enablement.
 *                - #true     Enable RTC trimming.
 *                - #false    Disable RTC trimming.
 *
 * @return      Status
 *                - #ADI_RTC_SUCCESS                    Success: Call completed successfully.
 *                - #ADI_RTC_ERR_BAD_DEV_HANDLE [D]     Error: Invalid device handle parameter.
 *                - #ADI_RTC_ERR_CLOCK_FAILSAFE [D]     Error: A failsafe interrupt is pending, see #adi_RTC_Init().
 *                - #ADI_RTC_ERR_NOT_INITIALIZED [D]    Error: Device has not been initialized for use, see #adi_RTC_Init().
 *
 * Enable/disable automatic application of trim values to the main RTC clock.  Allows application
 * of periodic real-time RTC clock adjustments to correct for drift.  Trim values are pre-calibrated
 * and stored at manufacture.  Trim values may be recalibrated by monitoring the RTC clock externally
 * and computing/storing new trim values (see #adi_RTC_SetTrim).
 *
 * @note The trim interval is reset with device enable, #adi_RTC_EnableDevice().
 *
 * @note The RTC device driver does not modify the trim enable on the hardware except through use of this API.
 *
 * @sa        adi_RTC_Init().
 * @sa        adi_RTC_EnableDevice().
 * @sa        adi_RTC_GetTrim().
 * @sa        adi_RTC_SetTrim().
 */
ADI_RTC_RESULT_TYPE adi_RTC_EnableTrim (ADI_RTC_HANDLE hDevice, bool_t bFlag)
{
#ifdef ADI_DEBUG
    if (ADI_RTC_INVALID_HANDLE(hDevice))
        return ADI_RTC_ERR_BAD_DEV_HANDLE;

    if (!hDevice->bInitialized)
        return ADI_RTC_ERR_NOT_INITIALIZED;

    if (ADI_RTC_FAILSAFE)
        return ADI_RTC_ERR_CLOCK_FAILSAFE;
#endif

    /* pause on pending writes to CR to avoid data loss */
    if (hDevice->bSafe)
        while (hDevice->pRtc->RTCSR1 & ADI_RTC_WRITE_STATUS_CONTROL)
            ;

    ADI_ENTER_CRITICAL_REGION();
    /* set/clear interrupt enable bit(s) in control register */
    if (bFlag)
        hDevice->pRtc->RTCCR |= ADI_RTC_CONTROL_TRIM_ENABLE;
    else
        hDevice->pRtc->RTCCR &= ~ADI_RTC_CONTROL_TRIM_ENABLE;
    ADI_EXIT_CRITICAL_REGION();

    return ADI_RTC_SUCCESS;
}


/*************************************************************************************************
**************************************************************************************************
******************************************   GET APIS   ******************************************
**************************************************************************************************
*************************************************************************************************/


/*!
 * @brief  Get current RTC alarm value.
 *
 * @param[in]   hDevice    Device handle obtained from adi_RTC_Init().
 * @param[in]   pAlarm     Pointer to application memory where the alarm value is written.
 *
 * @return      Status
 *                - #ADI_RTC_SUCCESS                    Success: Call completed successfully.
 *                - #ADI_RTC_ERR_BAD_DEV_HANDLE [D]     Error: Invalid device handle parameter.
 *                - #ADI_RTC_ERR_CLOCK_FAILSAFE [D]     Error: A failsafe interrupt is pending, see #adi_RTC_Init().
 *                - #ADI_RTC_ERR_NOT_INITIALIZED [D]    Error: Device has not been initialized for use, see #adi_RTC_Init().
 *                - #ADI_RTC_ERR_PARAM_OUT_OF_RANGE [D] Error: NULL pointer for input parameter.
 *
 * Read the currently programmed 32-bit RTC alarm value and write it to the address provided by parameter \a pAlarm.
 *
 * @sa        adi_RTC_Init().
 * @sa        adi_RTC_SetAlarm().
 */
ADI_RTC_RESULT_TYPE adi_RTC_GetAlarm (ADI_RTC_HANDLE hDevice, uint32_t *pAlarm)
{
    uint32_t result;

#ifdef ADI_DEBUG
    if (ADI_RTC_INVALID_HANDLE(hDevice))
        return ADI_RTC_ERR_BAD_DEV_HANDLE;

    if (!hDevice->bInitialized)
        return ADI_RTC_ERR_NOT_INITIALIZED;

    if (ADI_RTC_FAILSAFE)
        return ADI_RTC_ERR_CLOCK_FAILSAFE;

    if (!pAlarm)
        return ADI_RTC_ERR_PARAM_OUT_OF_RANGE;
#endif

    /* disable interrupts during paired read */
    ADI_DISABLE_INT(hDevice->IRQn);
        result  = hDevice->pRtc->RTCALM1 << 16;
        result |= hDevice->pRtc->RTCALM0;
    ADI_ENABLE_INT(hDevice->IRQn);

    *pAlarm = result;

    return ADI_RTC_SUCCESS;
}


/*!
 * @brief  Get current RTC control register value.
 *
 * @param[in]   hDevice    Device handle obtained from adi_RTC_Init().
 * @param[in]   pControl   Pointer to application memory where the control register value is written.
 *
 * @return      Status
 *                - #ADI_RTC_SUCCESS                    Success: Call completed successfully.
 *                - #ADI_RTC_ERR_BAD_DEV_HANDLE [D]     Error: Invalid device handle parameter.
 *                - #ADI_RTC_ERR_CLOCK_FAILSAFE [D]     Error: A failsafe interrupt is pending, see #adi_RTC_Init().
 *                - #ADI_RTC_ERR_NOT_INITIALIZED [D]    Error: Device has not been initialized for use, see #adi_RTC_Init().
 *                - #ADI_RTC_ERR_PARAM_OUT_OF_RANGE [D] Error: NULL pointer for input parameter.
 *
 * Read the currently programmed 16-bit RTC control register value and write it to the address provided by parameter \a pControl.
 *
 * @sa        adi_RTC_Init().
 * @sa        adi_RTC_SetControl().
 */
ADI_RTC_RESULT_TYPE adi_RTC_GetControl (ADI_RTC_HANDLE hDevice, uint16_t *pControl)
{
#ifdef ADI_DEBUG
    if (ADI_RTC_INVALID_HANDLE(hDevice))
        return ADI_RTC_ERR_BAD_DEV_HANDLE;

    if (!hDevice->bInitialized)
        return ADI_RTC_ERR_NOT_INITIALIZED;

    if (ADI_RTC_FAILSAFE)
        return ADI_RTC_ERR_CLOCK_FAILSAFE;

    if (!pControl)
        return ADI_RTC_ERR_PARAM_OUT_OF_RANGE;
#endif

    *pControl = hDevice->pRtc->RTCCR;

    return ADI_RTC_SUCCESS;
}


/*!
 * @brief  Get current RTC count value.
 *
 * @param[in]   hDevice    Device handle obtained from adi_RTC_Init().
 * @param[in]   pCount     Pointer to application memory where the count value is written.
 *
 * @return      Status
 *                - #ADI_RTC_SUCCESS                    Success: Call completed successfully.
 *                - #ADI_RTC_ERR_BAD_DEV_HANDLE [D]     Error: Invalid device handle parameter.
 *                - #ADI_RTC_ERR_CLOCK_FAILSAFE [D]     Error: A failsafe interrupt is pending, see #adi_RTC_Init().
 *                - #ADI_RTC_ERR_NOT_INITIALIZED [D]    Error: Device has not been initialized for use, see #adi_RTC_Init().
 *                - #ADI_RTC_ERR_PARAM_OUT_OF_RANGE [D] Error: NULL pointer for input parameter.
 *
 * Read the current 32-bit RTC count value and write it to the address provided by parameter \a pCount.
 *
 * @sa        adi_RTC_Init().
 * @sa        adi_RTC_EnableDevice().
 * @sa        adi_RTC_SetCount().
 */
ADI_RTC_RESULT_TYPE adi_RTC_GetCount (ADI_RTC_HANDLE hDevice, uint32_t *pCount)
{
    uint32_t result;

#ifdef ADI_DEBUG
    if (ADI_RTC_INVALID_HANDLE(hDevice))
        return ADI_RTC_ERR_BAD_DEV_HANDLE;

    if (!hDevice->bInitialized)
        return ADI_RTC_ERR_NOT_INITIALIZED;

    if (ADI_RTC_FAILSAFE)
        return ADI_RTC_ERR_CLOCK_FAILSAFE;

    if (!pCount)
        return ADI_RTC_ERR_PARAM_OUT_OF_RANGE;
#endif

    /* disable interrupts during paired read */
    ADI_DISABLE_INT(hDevice->IRQn);
        result  = hDevice->pRtc->RTCCNT1 << 16;
        result |= hDevice->pRtc->RTCCNT0;
    ADI_ENABLE_INT(hDevice->IRQn);

    *pCount = result;

    return ADI_RTC_SUCCESS;
}



/*!
 * @brief  Get current RTC interrupt source status.
 *
 * @param[in]   hDevice    Device handle obtained from adi_RTC_Init().
 * @param[in]   pStatus    Pointer to application memory where the interrupt status is written.
 *
 * @return      Status
 *                - #ADI_RTC_SUCCESS                    Success: Call completed successfully.
 *                - #ADI_RTC_ERR_BAD_DEV_HANDLE [D]     Error: Invalid device handle parameter.
 *                - #ADI_RTC_ERR_CLOCK_FAILSAFE [D]     Error: A failsafe interrupt is pending, see #adi_RTC_Init().
 *                - #ADI_RTC_ERR_NOT_INITIALIZED [D]    Error: Device has not been initialized for use, see #adi_RTC_Init().
 *                - #ADI_RTC_ERR_PARAM_OUT_OF_RANGE [D] Error: NULL pointer for input parameter.
 *
 * Read the current RTC pending interrupt source status register and write it to the address provided by parameter \a pStatus.
 * Result is of type ADI_RTC_INT_SOURCE_TYPE, which is a bit field.
 *
 * @note Interrupt source status is not gated with the interrupt enable bits; source status is set regardless of interrupt enables.
 *
 * @sa        adi_RTC_Init().
 * @sa        adi_RTC_ClearInterruptStatus().
 */
ADI_RTC_RESULT_TYPE adi_RTC_GetInterruptStatus (ADI_RTC_HANDLE hDevice, ADI_RTC_INT_SOURCE_TYPE *pStatus)
{
#ifdef ADI_DEBUG
    if (ADI_RTC_INVALID_HANDLE(hDevice))
        return ADI_RTC_ERR_BAD_DEV_HANDLE;

    if (!hDevice->bInitialized)
        return ADI_RTC_ERR_NOT_INITIALIZED;

    if (ADI_RTC_FAILSAFE)
        return ADI_RTC_ERR_CLOCK_FAILSAFE;

    if (!pStatus)
        return ADI_RTC_ERR_PARAM_OUT_OF_RANGE;
#endif

    *pStatus = (ADI_RTC_INT_SOURCE_TYPE) (hDevice->pRtc->RTCSR0 & ADI_RTC_INT_SOURCE_MASK);

    return ADI_RTC_SUCCESS;
}


/*!
 * @brief  Get current RTC LCD update interval.
 *
 * @param[in]   hDevice    Device handle obtained from adi_RTC_Init().
 * @param[in]   pUpdate    Pointer to application memory where the LCD update interval is written.
 *
 * @return      Status
 *                - #ADI_RTC_SUCCESS                    Success: Call completed successfully.
 *                - #ADI_RTC_ERR_BAD_DEV_HANDLE [D]     Error: Invalid device handle parameter.
 *                - #ADI_RTC_ERR_CLOCK_FAILSAFE [D]     Error: A failsafe interrupt is pending, see #adi_RTC_Init().
 *                - #ADI_RTC_ERR_NOT_INITIALIZED [D]    Error: Device has not been initialized for use, see #adi_RTC_Init().
 *                - #ADI_RTC_ERR_PARAM_OUT_OF_RANGE [D] Error: NULL pointer for input parameter.
 *
 * Read the current 16-bit RTC LCD update interval and write it to the address provided by parameter \a pUpdate.
 * Result is a right-justified integer in the range 0-59, even though it is stored internally in a bit-fielded register slot.
 *
 * @sa        adi_RTC_Init().
 * @sa        adi_RTC_EnableLcdUpdate().
 * @sa        adi_RTC_SetLcdUpdate().
 */
ADI_RTC_RESULT_TYPE adi_RTC_GetLcdUpdate (ADI_RTC_HANDLE hDevice, uint16_t *pUpdate)
{
#ifdef ADI_DEBUG
    if (ADI_RTC_INVALID_HANDLE(hDevice))
        return ADI_RTC_ERR_BAD_DEV_HANDLE;

    if (!hDevice->bInitialized)
        return ADI_RTC_ERR_NOT_INITIALIZED;

    if (ADI_RTC_FAILSAFE)
        return ADI_RTC_ERR_CLOCK_FAILSAFE;

    if (!pUpdate)
        return ADI_RTC_ERR_PARAM_OUT_OF_RANGE;
#endif

    /* get the value */
    *pUpdate = (hDevice->pRtc->RTCCR & ADI_RTC_LCD_UPDATE_MASK) >> ADI_RTC_LCD_UPDATE_BPOS;

    return ADI_RTC_SUCCESS;
}


/*!
 * @brief  Get current RTC clock trim value.
 *
 * @param[in]   hDevice    Device handle obtained from adi_RTC_Init().
 * @param[in]   pTrim      Pointer to application memory where the clock trim value is written.
 *
 * @return      Status
 *                - #ADI_RTC_SUCCESS                    Success: Call completed successfully.
 *                - #ADI_RTC_ERR_BAD_DEV_HANDLE [D]     Error: Invalid device handle parameter.
 *                - #ADI_RTC_ERR_CLOCK_FAILSAFE [D]     Error: A failsafe interrupt is pending, see #adi_RTC_Init().
 *                - #ADI_RTC_ERR_NOT_INITIALIZED [D]    Error: Device has not been initialized for use, see #adi_RTC_Init().
 *                - #ADI_RTC_ERR_PARAM_OUT_OF_RANGE [D] Error: NULL pointer for input parameter.
 *
 * Read the current 16-bit RTC LCD trim value and write it to the address provided by parameter \a pTrim.
 *
 * @note The trim result is a bit-fielded value in which the trim interval (#ADI_RTC_TRIM_INTERVAL_TYPE),
 *       trim polarity (#ADI_RTC_TRIM_POLARITY_TYPE) and trim value (#ADI_RTC_TRIM_VALUE_TYPE) are encoded.
 *       The encoding for these types may be decomposed at the application layer with the three
 *       ADI_RTC_TRIM_xxxx_TYPE enumerations and their corresponding ADI_RTC_TRIM_xxxx_MASK mask values.
 *       For example:
 *
 * @verbatim
 *       ...
 *       if (ADI_RTC_SUCCESS != (result = adi_RTC_GetTrim(hDev, &trim)))
 *           handle_error(result);
 *
 *       switch (trim & ADI_RTC_TRIM_INTERVAL_MASK) {
 *           case ADI_RTC_TRIM_INTERVAL_14:
 *               trim_interval_is_16384_seconds();
 *               break;
 *           case ADI_RTC_TRIM_INTERVAL_15:
 *               trim_interval_is_32768_seconds();
 *               break;
 *           etc...
 *       }
 *       ...
 * @endverbatim
 *
 * @sa        adi_RTC_Init().
 * @sa        adi_RTC_EnableInterrupts().
 * @sa        adi_RTC_EnableTrim().
 * @sa        adi_RTC_GetWriteErrorSource().
 * @sa        adi_RTC_GetWritePendStatus().
 * @sa        adi_RTC_GetWriteSyncStatus().
 * @sa        adi_RTC_RegisterCallback().
 * @sa        adi_RTC_ClearInterruptStatus().
 * @sa        adi_RTC_SetTrim().
 */
ADI_RTC_RESULT_TYPE adi_RTC_GetTrim (ADI_RTC_HANDLE hDevice, uint16_t *pTrim)
{
#ifdef ADI_DEBUG
    if (ADI_RTC_INVALID_HANDLE(hDevice))
        return ADI_RTC_ERR_BAD_DEV_HANDLE;

    if (!hDevice->bInitialized)
        return ADI_RTC_ERR_NOT_INITIALIZED;

    if (ADI_RTC_FAILSAFE)
        return ADI_RTC_ERR_CLOCK_FAILSAFE;

    if (!pTrim)
        return ADI_RTC_ERR_PARAM_OUT_OF_RANGE;
#endif

    *pTrim = hDevice->pRtc->RTCTRM & ADI_RTC_TRIM_MASK;

    return ADI_RTC_SUCCESS;
}


/*!
 * @brief  Get current RTC write error status.
 *
 * @param[in]   hDevice    Device handle obtained from adi_RTC_Init().
 * @param[in]   pError     Pointer to application memory where the write error status is written.
 *
 * @return      Status
 *                - #ADI_RTC_SUCCESS                    Success: Call completed successfully.
 *                - #ADI_RTC_ERR_BAD_DEV_HANDLE [D]     Error: Invalid device handle parameter.
 *                - #ADI_RTC_ERR_CLOCK_FAILSAFE [D]     Error: A failsafe interrupt is pending, see #adi_RTC_Init().
 *                - #ADI_RTC_ERR_NOT_INITIALIZED [D]    Error: Device has not been initialized for use, see #adi_RTC_Init().
 *                - #ADI_RTC_ERR_PARAM_OUT_OF_RANGE [D] Error: NULL pointer for input parameter.
 *
 * Read the current write error status field and write it to the address provided by parameter \a pError.
 *
 * The write error state indicates data loss by having written an RTC register twice without regard for
 * synchronizing multiple writes to the same register.  Various options exist to managing register write
 * synchronization (see references, below).
 *
 * The result is of type #ADI_RTC_WRITE_ERROR_TYPE, which is a discrete value the application may process as:
 *
 * @verbatim
 *       ...
 *       if (ADI_RTC_SUCCESS != (result = adi_RTC_GetWriteErrorSource(hDev, &error)))
 *           handle_error(result);
 *
 *       switch (error & ADI_RTC_WRITE_ERROR_MASK) {
 *           case ADI_RTC_WRITE_ERROR_CONTROL:
 *               handle_control_write_error();
 *               break;
 *           case ADI_RTC_WRITE_ERROR_STATUS0:
 *               handle_status0_write_error();
 *               break;
 *           etc...
 *       }
 *       ...
 * @endverbatim
 *
 * @sa        adi_RTC_Init().
 * @sa        adi_RTC_EnableInterrupts().
 * @sa        adi_RTC_EnableSafeWrites().
 * @sa        adi_RTC_GetWriteErrorSource().
 * @sa        adi_RTC_GetWritePendStatus().
 * @sa        adi_RTC_GetWriteSyncStatus().
 * @sa        adi_RTC_RegisterCallback().
 * @sa        adi_RTC_ClearInterruptStatus().
 */
ADI_RTC_RESULT_TYPE adi_RTC_GetWriteErrorSource (ADI_RTC_HANDLE hDevice, ADI_RTC_WRITE_ERROR_TYPE *pError)
{
#ifdef ADI_DEBUG
    if (ADI_RTC_INVALID_HANDLE(hDevice))
        return ADI_RTC_ERR_BAD_DEV_HANDLE;

    if (!hDevice->bInitialized)
        return ADI_RTC_ERR_NOT_INITIALIZED;

    if (ADI_RTC_FAILSAFE)
        return ADI_RTC_ERR_CLOCK_FAILSAFE;

    if (!pError)
        return ADI_RTC_ERR_PARAM_OUT_OF_RANGE;
#endif

    /* get the value */
    *pError = (ADI_RTC_WRITE_ERROR_TYPE) (hDevice->pRtc->RTCSR1 & ADI_RTC_WRITE_ERROR_MASK);

    return ADI_RTC_SUCCESS;
}



/*!
 * @brief  Get current RTC posted write pending status.
 *
 * @param[in]   hDevice    Device handle obtained from adi_RTC_Init().
 * @param[in]   pPendBits  Pointer to application memory where the posted write status is written.
 *
 * @return      Status
 *                - #ADI_RTC_SUCCESS                    Success: Call completed successfully.
 *                - #ADI_RTC_ERR_BAD_DEV_HANDLE [D]     Error: Invalid device handle parameter.
 *                - #ADI_RTC_ERR_CLOCK_FAILSAFE [D]     Error: A failsafe interrupt is pending, see #adi_RTC_Init().
 *                - #ADI_RTC_ERR_NOT_INITIALIZED [D]    Error: Device has not been initialized for use, see #adi_RTC_Init().
 *                - #ADI_RTC_ERR_PARAM_OUT_OF_RANGE [D] Error: NULL pointer for input parameter.
 *
 * Read the current RTC posted write \a pending status and write it to the address provided by parameter \a pPendBits.
 * Use the #ADI_RTC_WRITE_STATUS_MASK and #ADI_RTC_WRITE_STATUS_TYPE enumerator to decompose the result.
 *
 * \b Pending \b Writes: Register writes to internal RTC registers take time to complete because the RTC controller
 * clock is running at a much slower (32kHz) rate than the core processor clock.  So each RTC write register has a
 * one-deep FIFO to hold write values until the RTC can effect them.  This gives rise to the notion of a \a pending
 * \a write state: if a write is already pending and another write from the core comes along before the first (pending)
 * write has cleared to its destination register, the second write may be lost because the FIFO is full already.
 *
 * To avoid data loss, the user may tell the RTC device driver to enforce safe writes with the
 * adi_RTC_EnableSafeWrites() API.  Enabeling safe writes (on be default) insures write data is never lost by
 * detecting and pausing on pending writes prior writing new data.  The penalty in using safe writes is the stall
 * overhead in execution (which is not incurred if there is nothing pending).  Additionally, \a all pending writes
 * may also be synchronized manually with the #adi_RTC_SynchronizeAllWrites() API, which will pause until all
 * pending RTC writes have completed.
 *
 * The distinction between "pend" status (#adi_RTC_GetWritePendStatus()) and "sync" (#adi_RTC_GetWriteSyncStatus())
 * status is that the \a pend state is normally clear and is set only while no room remains in a register's write FIFO,
 * whereas \a sync state is normally set and is clear only while the effects of the write are not yet apparent.
 *
 * Finally, all pending write error events (indicating lost data) are latched in the WERRCODE write error source
 * field of RTCSR1 status register (see the #adi_RTC_GetWriteErrorSource() API).  Additionally, each write error
 * source may be configured to interrupt the core by enabeling the appropriate write error interrupt mask bit in
 * the RTC control register (see the #adi_RTC_EnableInterrupts() API), at which time, the RTC interrupt handler
 * will be dispatched.
 *
 * @note To receive application-level callbacks of an RTC interrupt, use the #adi_RTC_RegisterCallback() API to
 *       register an application-level callback function and interrupt bit mask.
 *
 * @sa        adi_RTC_Init().
 * @sa        #adi_RTC_EnableInterrupts().
 * @sa        adi_RTC_EnableSafeWrites().
 * @sa        adi_RTC_GetWriteSyncStatus().
 * @sa        adi_RTC_StallOnPendingWrites().
 * @sa        adi_RTC_SynchronizeAllWrites().
 */
ADI_RTC_RESULT_TYPE adi_RTC_GetWritePendStatus (ADI_RTC_HANDLE hDevice, ADI_RTC_WRITE_STATUS_TYPE *pPendBits)
{
#ifdef ADI_DEBUG
    if (ADI_RTC_INVALID_HANDLE(hDevice))
        return ADI_RTC_ERR_BAD_DEV_HANDLE;

    if (!hDevice->bInitialized)
        return ADI_RTC_ERR_NOT_INITIALIZED;

    if (ADI_RTC_FAILSAFE)
        return ADI_RTC_ERR_CLOCK_FAILSAFE;

    if (!pPendBits)
        return ADI_RTC_ERR_PARAM_OUT_OF_RANGE;
#endif

    /* get the value */
    *pPendBits = (ADI_RTC_WRITE_STATUS_TYPE) (hDevice->pRtc->RTCSR1 & ADI_RTC_WRITE_STATUS_MASK);

    return ADI_RTC_SUCCESS;
}


/*!
 * @brief  Get current RTC posted write synchronization status.
 *
 * @param[in]   hDevice    Device handle obtained from adi_RTC_Init().
 * @param[in]   pSyncBits  Pointer to application memory where the posted write status is written.
 *
 * @return      Status
 *                - #ADI_RTC_SUCCESS                    Success: Call completed successfully.
 *                - #ADI_RTC_ERR_BAD_DEV_HANDLE [D]     Error: Invalid device handle parameter.
 *                - #ADI_RTC_ERR_CLOCK_FAILSAFE [D]     Error: A failsafe interrupt is pending, see #adi_RTC_Init().
 *                - #ADI_RTC_ERR_NOT_INITIALIZED [D]    Error: Device has not been initialized for use, see #adi_RTC_Init().
 *                - #ADI_RTC_ERR_PARAM_OUT_OF_RANGE [D] Error: NULL pointer for input parameter.
 *
 * Read the current RTC posted write \a synchronization status and write it to the address provided by parameter \a pSyncBits.
 * Use the #ADI_RTC_WRITE_STATUS_MASK and #ADI_RTC_WRITE_STATUS_TYPE enumerator to decompose the result.
 *
 * \b Pending \b Writes: Register writes to internal RTC registers take time to complete because the RTC controller
 * clock is running at a much slower (32kHz) rate than the core processor clock.  So each RTC write register has a
 * one-deep FIFO to hold write values until the RTC can effect them.  This gives rise to the notion of a \a pending
 * \a write state: if a write is already pending and another write from the core comes along before the first (pending)
 * write has cleared to its destination register, the second write may be lost because the FIFO is full already.
 *
 * To avoid data loss, the user may tell the RTC device driver to enforce safe writes with the
 * adi_RTC_EnableSafeWrites() API.  Enabling safe writes (on be default) insures write data is never lost by
 * detecting and pausing on pending writes prior writing new data.  The penalty in using safe writes is the stall
 * overhead in execution (which is not incurred if there is nothing pending).  Additionally, \a all pending writes
 * may also be synchronized manually with the #adi_RTC_SynchronizeAllWrites() API, which will pause until all
 * pending RTC writes have completed.
 *
 * The distinction between "pend" status (#adi_RTC_GetWritePendStatus()) and "sync" (#adi_RTC_GetWriteSyncStatus())
 * status is that the \a pend state is normally clear is set only while no room remains in a register's write FIFO,
 * whereas \a sync state is normally set and is clear only while the effects of the write are not yet apparent.
 *
 * Finally, all pending write error events (indicating lost data) are latched in the WERRCODE write error source
 * field of RTCSR1 status register (see the #adi_RTC_GetWriteErrorSource() API).  Additionally, each write error
 * source may be configured to interrupt the core by enabling the appropriate write error interrupt mask bit in
 * the RTC control register (see the #adi_RTC_EnableInterrupts() API), at which thime, the RTC interrupt handler
 * will be dispatched.
 *
 * @note To receive application-level callbacks of an RTC interrupt, use the #adi_RTC_RegisterCallback() API to
 *       register an application-level callback function and interrupt bit mask.
 *
 * @sa        adi_RTC_Init().
 * @sa        #adi_RTC_EnableInterrupts().
 * @sa        adi_RTC_EnableSafeWrites().
 * @sa        adi_RTC_GetWritePendStatus().
 * @sa        adi_RTC_StallOnPendingWrites().
 * @sa        adi_RTC_SynchronizeAllWrites().
 */
ADI_RTC_RESULT_TYPE adi_RTC_GetWriteSyncStatus (ADI_RTC_HANDLE hDevice, ADI_RTC_WRITE_STATUS_TYPE *pSyncBits)
{
#ifdef ADI_DEBUG
    if (ADI_RTC_INVALID_HANDLE(hDevice))
        return ADI_RTC_ERR_BAD_DEV_HANDLE;

    if (!hDevice->bInitialized)
        return ADI_RTC_ERR_NOT_INITIALIZED;

    if (ADI_RTC_FAILSAFE)
        return ADI_RTC_ERR_CLOCK_FAILSAFE;

    if (!pSyncBits)
        return ADI_RTC_ERR_PARAM_OUT_OF_RANGE;
#endif

    /* get the value */
    *pSyncBits = (ADI_RTC_WRITE_STATUS_TYPE) (hDevice->pRtc->RTCSR0 & ADI_RTC_WRITE_STATUS_MASK);

    return ADI_RTC_SUCCESS;
}


/*************************************************************************************************
**************************************************************************************************
******************************************   SET APIS   ******************************************
**************************************************************************************************
*************************************************************************************************/


/*!
 * @brief  Set a new RTC alarm value.
 *
 * @param[in]   hDevice    Device handle obtained from adi_RTC_Init().
 * @param[in]   Alarm      New alarm value to set.
 *
 * @return      Status
 *                - #ADI_RTC_SUCCESS                    Success: Call completed successfully.
 *                - #ADI_RTC_ERR_BAD_DEV_HANDLE [D]     Error: Invalid device handle parameter.
 *                - #ADI_RTC_ERR_CLOCK_FAILSAFE [D]     Error: A failsafe interrupt is pending, see #adi_RTC_Init().
 *                - #ADI_RTC_ERR_NOT_INITIALIZED [D]    Error: Device has not been initialized for use, see #adi_RTC_Init().
 *
 * Writes the 32-bit RTC alarm comparator with the value provided by \a Alarm.
 *
 * Honors the safe write mode if set.  Otherwise, it is the application's responsibility to
 * synchronize any multiple writes to the same register.
 *
 * @sa        adi_RTC_Init().
 * @sa        adi_RTC_EnableSafeWrites().
 * @sa        adi_RTC_GetAlarm().
 * @sa        adi_RTC_EnableAlarm().
 * @sa        adi_RTC_GetWritePendStatus().
 * @sa        adi_RTC_StallOnPendingWrites().
 * @sa        adi_RTC_SynchronizeAllWrites().
 */
ADI_RTC_RESULT_TYPE adi_RTC_SetAlarm (ADI_RTC_HANDLE hDevice, uint32_t Alarm)
{
#ifdef ADI_DEBUG
    if (ADI_RTC_INVALID_HANDLE(hDevice))
        return ADI_RTC_ERR_BAD_DEV_HANDLE;

    if (!hDevice->bInitialized)
        return ADI_RTC_ERR_NOT_INITIALIZED;

    if (ADI_RTC_FAILSAFE)
        return ADI_RTC_ERR_CLOCK_FAILSAFE;
#endif

    /* pause on pending writes to both ALMs to avoid data loss */
    if (hDevice->bSafe)
        while (hDevice->pRtc->RTCSR1 & (ADI_RTC_WRITE_STATUS_ALARM0 | ADI_RTC_WRITE_STATUS_ALARM1))
            ;

    ADI_ENTER_CRITICAL_REGION();
    /* RTC hardware insures paired write, so no need to disable interrupts */
    hDevice->pRtc->RTCALM0 = Alarm & 0xffff;
    hDevice->pRtc->RTCALM1 = (Alarm >> 16) & 0xffff;
    ADI_EXIT_CRITICAL_REGION();

    return ADI_RTC_SUCCESS;
}


/*!
 * @brief  Set a new RTC control register value.
 *
 * @param[in]   hDevice    Device handle obtained from adi_RTC_Init().
 * @param[in]   Control    New control register value to set.
 *
 * @return      Status
 *                - #ADI_RTC_SUCCESS                    Success: Call completed successfully.
 *                - #ADI_RTC_ERR_BAD_DEV_HANDLE [D]     Error: Invalid device handle parameter.
 *                - #ADI_RTC_ERR_CLOCK_FAILSAFE [D]     Error: A failsafe interrupt is pending, see #adi_RTC_Init().
 *                - #ADI_RTC_ERR_NOT_INITIALIZED [D]    Error: Device has not been initialized for use, see #adi_RTC_Init().
 *
 * Writes the 16-bit RTC control register with the value provided by \a Control.
 *
 * Honors the safe write mode if set.  Otherwise, it is the application's responsibility to
 * synchronize any multiple writes to the same register.
 *
 * @sa        adi_RTC_Init().
 * @sa        adi_RTC_EnableSafeWrites().
 * @sa        adi_RTC_GetControl().
 * @sa        adi_RTC_GetWritePendStatus().
 * @sa        adi_RTC_StallOnPendingWrites().
 * @sa        adi_RTC_SynchronizeAllWrites().
 */
ADI_RTC_RESULT_TYPE adi_RTC_SetControl (ADI_RTC_HANDLE hDevice, uint16_t Control)
{
#ifdef ADI_DEBUG
    if (ADI_RTC_INVALID_HANDLE(hDevice))
        return ADI_RTC_ERR_BAD_DEV_HANDLE;

    if (!hDevice->bInitialized)
        return ADI_RTC_ERR_NOT_INITIALIZED;

    if (ADI_RTC_FAILSAFE)
        return ADI_RTC_ERR_CLOCK_FAILSAFE;
#endif

    /* pause on pending writes to CR to avoid data loss */
    if (hDevice->bSafe)
        while (hDevice->pRtc->RTCSR1 & ADI_RTC_WRITE_STATUS_CONTROL)
            ;

    ADI_ENTER_CRITICAL_REGION();
    hDevice->pRtc->RTCCR = Control;
    ADI_EXIT_CRITICAL_REGION();

    return ADI_RTC_SUCCESS;
}


/*!
 * @brief  Set a new RTC count value.
 *
 * @param[in]   hDevice    Device handle obtained from adi_RTC_Init().
 * @param[in]   Count      New count value to set.
 *
 * @return      Status
 *                - #ADI_RTC_SUCCESS                    Success: Call completed successfully.
 *                - #ADI_RTC_ERR_BAD_DEV_HANDLE [D]     Error: Invalid device handle parameter.
 *                - #ADI_RTC_ERR_CLOCK_FAILSAFE [D]     Error: A failsafe interrupt is pending, see #adi_RTC_Init().
 *                - #ADI_RTC_ERR_NOT_INITIALIZED [D]    Error: Device has not been initialized for use, see #adi_RTC_Init().
 *
 * Writes the main 32-bit RTC counter with the value provided by \a Count.
 *
 * Honors the safe write mode if set.  Otherwise, it is the application's responsibility to
 * synchronize any multiple writes to the same register.
 *
 * @sa        adi_RTC_Init().
 * @sa        adi_RTC_EnableSafeWrites().
 * @sa        adi_RTC_SetCount().
 * @sa        adi_RTC_GetWritePendStatus().
 * @sa        adi_RTC_StallOnPendingWrites().
 * @sa        adi_RTC_SynchronizeAllWrites().
 */
ADI_RTC_RESULT_TYPE adi_RTC_SetCount (ADI_RTC_HANDLE hDevice, uint32_t Count)
{
#ifdef ADI_DEBUG
    if (ADI_RTC_INVALID_HANDLE(hDevice))
        return ADI_RTC_ERR_BAD_DEV_HANDLE;

    if (!hDevice->bInitialized)
        return ADI_RTC_ERR_NOT_INITIALIZED;

    if (ADI_RTC_FAILSAFE)
        return ADI_RTC_ERR_CLOCK_FAILSAFE;
#endif

    /* pause on pending writes to both CNTs to avoid data loss */
    if (hDevice->bSafe)
        while (hDevice->pRtc->RTCSR1 & (ADI_RTC_WRITE_STATUS_COUNT0 | ADI_RTC_WRITE_STATUS_COUNT1))
            ;

    ADI_ENTER_CRITICAL_REGION();
    /* RTC hardware insures paired write, so no need to disable interrupts */
    hDevice->pRtc->RTCCNT0 = Count & 0xffff;
    hDevice->pRtc->RTCCNT1 = (Count >> 16) & 0xffff;
    ADI_EXIT_CRITICAL_REGION();

    /* clear RTCFAIL interrupt bit if its set */
    if (pADI_RTC->RTCSR0 & ADI_RTC_INT_SOURCE_FAIL) {

        /* pause on pending writes to SR0 to avoid data loss */
        if (hDevice->bSafe)
            while (hDevice->pRtc->RTCSR1 & ADI_RTC_WRITE_STATUS_STATUS0)
                ;
        hDevice->pRtc->RTCSR0 = ADI_RTC_INT_SOURCE_FAIL;
    }

    return ADI_RTC_SUCCESS;
}


/*!
 * @brief  Set an RTC gateway command.
 *
 * @param[in]   hDevice    Device handle obtained from adi_RTC_Init().
 * @param[in]   Command    Gateway command value.
 *
 * @return      Status
 *                - #ADI_RTC_SUCCESS                    Success: Call completed successfully.
 *                - #ADI_RTC_ERR_BAD_DEV_HANDLE [D]     Error: Invalid device handle parameter.
 *                - #ADI_RTC_ERR_CLOCK_FAILSAFE [D]     Error: A failsafe interrupt is pending, see #adi_RTC_Init().
 *                - #ADI_RTC_ERR_NOT_INITIALIZED [D]    Error: Device has not been initialized for use, see #adi_RTC_Init().
 *
 * Writes the 16-bit RTC gateway register with the command provided by \a Command.
 *
 * The gateway register is used to force the RTC to perform some urgent action.
 *
 * Currently, only the #ADI_RTC_GATEWAY_FLUSH command is defined, which will cancel all
 * RTC register write transactions, both pending and executing.  It is intended to truncate
 * all core interactions in preparation for an imminent power loss when the RTC power
 * isolation barrier will be activated.
 *
 * @sa        adi_RTC_Init().
 */
ADI_RTC_RESULT_TYPE adi_RTC_SetGateway (ADI_RTC_HANDLE hDevice, uint16_t Command)
{
#ifdef ADI_DEBUG
    if (ADI_RTC_INVALID_HANDLE(hDevice))
        return ADI_RTC_ERR_BAD_DEV_HANDLE;

    if (!hDevice->bInitialized)
        return ADI_RTC_ERR_NOT_INITIALIZED;

    if (ADI_RTC_FAILSAFE)
        return ADI_RTC_ERR_CLOCK_FAILSAFE;
#endif

    /* (no pending write status to check for gateway register) */

    ADI_ENTER_CRITICAL_REGION();
    /* set the command */
    hDevice->pRtc->RTCGWY = Command;
    ADI_EXIT_CRITICAL_REGION();

    return ADI_RTC_SUCCESS;
}


/*!
 * @brief  Clear an RTC interrupt status bit(s).
 *
 * @param[in]   hDevice    Device handle obtained from adi_RTC_Init().
 * @param[in]   Status     Interrupt status bits to write.
 *
 * @return      Status
 *                - #ADI_RTC_SUCCESS                    Success: Call completed successfully.
 *                - #ADI_RTC_ERR_BAD_DEV_HANDLE [D]     Error: Invalid device handle parameter.
 *                - #ADI_RTC_ERR_CLOCK_FAILSAFE [D]     Error: A failsafe interrupt is pending, see #adi_RTC_Init().
 *                - #ADI_RTC_ERR_NOT_INITIALIZED [D]    Error: Device has not been initialized for use, see #adi_RTC_Init().
 *                - #ADI_RTC_ERR_PARAM_OUT_OF_RANGE [D] Error: Input parameter out of range.
 *
 * Clear (write-one-to-clear) the RTC interrupt status field with the value provided by \a Status.
 * Bits that are set in the \a Status value will clear their corresponding interrupt status.
 * Bits that are clear in the \a Status value will have no effect.
 * Use the #ADI_RTC_INT_SOURCE_TYPE enumeration to encode interrupts to clear.
 *
 * Honors the safe write mode if set.  Otherwise, it is the application's responsibility to
 * synchronize any multiple writes to the same register.
 *
 * @sa        adi_RTC_Init().
 * @sa        adi_RTC_GetInterruptStatus().
 */
ADI_RTC_RESULT_TYPE adi_RTC_ClearInterruptStatus (ADI_RTC_HANDLE hDevice, ADI_RTC_INT_SOURCE_TYPE Status)
{
#ifdef ADI_DEBUG
    if (ADI_RTC_INVALID_HANDLE(hDevice))
        return ADI_RTC_ERR_BAD_DEV_HANDLE;

    if (!hDevice->bInitialized)
        return ADI_RTC_ERR_NOT_INITIALIZED;

    if (ADI_RTC_FAILSAFE) {

        // dont issue failsafe if user is trying to clear it...
        if ( !(Status & ADI_RTC_INT_SOURCE_FAIL) )
            return ADI_RTC_ERR_CLOCK_FAILSAFE;
    }

    /* range check against interrupt mask */
    if (Status & ~ADI_RTC_INT_SOURCE_MASK)
        return ADI_RTC_ERR_PARAM_OUT_OF_RANGE;
#endif

    /* pause on pending writes to SR0 to avoid data loss */
    if (hDevice->bSafe)
        while (hDevice->pRtc->RTCSR1 & ADI_RTC_WRITE_STATUS_STATUS0)
            ;

    hDevice->pRtc->RTCSR0 = Status & ADI_RTC_INT_SOURCE_MASK;

    return ADI_RTC_SUCCESS;
}


/*!
 * @brief  Set a new RTC LCD update interval.
 *
 * @param[in]   hDevice    Device handle obtained from adi_RTC_Init().
 * @param[in]   Update     New LCD update interval.
 *
 * @return      Status
 *                - #ADI_RTC_SUCCESS                    Success: Call completed successfully.
 *                - #ADI_RTC_ERR_BAD_DEV_HANDLE [D]     Error: Invalid device handle parameter.
 *                - #ADI_RTC_ERR_CLOCK_FAILSAFE [D]     Error: A failsafe interrupt is pending, see #adi_RTC_Init().
 *                - #ADI_RTC_ERR_NOT_INITIALIZED [D]    Error: Device has not been initialized for use, see #adi_RTC_Init().
 *                - #ADI_RTC_ERR_PARAM_OUT_OF_RANGE [D] Error: Input parameter out of range.
 *
 * Sets a new LCD update interval in the RTC control register.  Valid range for \a Update is
 * 0-59 (seconds).  Range checks the new value and merges it with the existing control register
 * value.  The LCD update offset is relative to the one-minute boundary of the main RTC counter.
 *
 * Honors the safe write mode if set.  Otherwise, it is the application's responsibility to
 * synchronize any multiple writes to the same register.
 *
 * @sa        adi_RTC_Init().
 * @sa        adi_RTC_EnableLcdUpdate().
 * @sa        adi_RTC_GetLcdUpdate().
 */
ADI_RTC_RESULT_TYPE adi_RTC_SetLcdUpdate (ADI_RTC_HANDLE hDevice, uint16_t Update)
{
#ifdef ADI_DEBUG
    if (ADI_RTC_INVALID_HANDLE(hDevice))
        return ADI_RTC_ERR_BAD_DEV_HANDLE;

    if (!hDevice->bInitialized)
        return ADI_RTC_ERR_NOT_INITIALIZED;

    if (ADI_RTC_FAILSAFE)
        return ADI_RTC_ERR_CLOCK_FAILSAFE;

    if (Update >= 60)
        return ADI_RTC_ERR_PARAM_OUT_OF_RANGE;
#endif

    /* pause on pending writes to CR to avoid data loss */
    if (hDevice->bSafe)
        while (hDevice->pRtc->RTCSR1 & ADI_RTC_WRITE_STATUS_CONTROL)
            ;

    /* make a hole... */
    hDevice->pRtc->RTCCR &= ~ADI_RTC_LCD_UPDATE_MASK;

    /* ...wait for it to clear */
    if (hDevice->bSafe)
        while (hDevice->pRtc->RTCSR1 & ADI_RTC_WRITE_STATUS_CONTROL)
            ;

    ADI_ENTER_CRITICAL_REGION();
    /* set the new value */
    hDevice->pRtc->RTCCR |= (Update << ADI_RTC_LCD_UPDATE_BPOS) & ADI_RTC_LCD_UPDATE_MASK;
    ADI_EXIT_CRITICAL_REGION();

    return ADI_RTC_SUCCESS;
}


/*!
 * @brief  Set a new RTC trim value.
 *
 * @param[in]   hDevice    Device handle obtained from adi_RTC_Init().
 * @param[in]   Trim       New RTC counter trim value.
 *
 * @return      Status
 *                - #ADI_RTC_SUCCESS                    Success: Call completed successfully.
 *                - #ADI_RTC_ERR_BAD_DEV_HANDLE [D]     Error: Invalid device handle parameter.
 *                - #ADI_RTC_ERR_CLOCK_FAILSAFE [D]     Error: A failsafe interrupt is pending, see #adi_RTC_Init().
 *                - #ADI_RTC_ERR_NOT_INITIALIZED [D]    Error: Device has not been initialized for use, see #adi_RTC_Init().
 *                - #ADI_RTC_ERR_PARAM_OUT_OF_RANGE [D] Error: Input parameter out of range.
 *
 * The RTC hardware has the ability to automatically trim the clock to compensate for variations
 * in oscillator tolerance .   Automatic trimming is enabled with the #adi_RTC_EnableTrim() API.
 *
 * Default trim values are calculated and prestored during device manufacture.  It is not
 * recommended to change the trim value without carefully calibrating performance
 * requirements.
 *
 * Trim values are comprised of three components: a trim interval (#ADI_RTC_TRIM_INTERVAL_TYPE),
 * trim polarity (#ADI_RTC_TRIM_POLARITY_TYPE), and trim amount (#ADI_RTC_TRIM_VALUE_TYPE).
 * Use these enumerations to build a trim value that will be stored in the 16-bit RTC trim register.
 *
 * For instance:
 *
 * @verbatim
 *       ...
 *       if (ADI_RTC_SUCCESS != (result = adi_RTC_SetTrim(hDev, ADI_RTC_TRIM_INTERVAL_15
 *                                                            | ADI_RTC_TRIM_SUB
 *                                                            | ADI_RTC_TRIM_3)))
 *           handle_error(result);
 *       ...
 * @endverbatim
 *
 * will adjust the main RTC counter every 32768 seconds (~9.1 hours) by delaying the RTC counter
 * (subtracting) by three one-second ticks (91 ppm).
 *
 * Honors the safe write mode if set.  Otherwise, it is the application's responsibility to
 * synchronize any multiple writes to the same register.
 *
 * @note Alarms are not affected by automatic trim operations.
 *
 * @note The trim boundary (interval) alignment is reset when new trim values are written.
 *
 * @sa        adi_RTC_Init().
 * @sa        adi_RTC_EnableTrim().
 * @sa        adi_RTC_GetTrim().
 */
ADI_RTC_RESULT_TYPE adi_RTC_SetTrim (ADI_RTC_HANDLE hDevice, uint16_t Trim)
{
#ifdef ADI_DEBUG
    if (ADI_RTC_INVALID_HANDLE(hDevice))
        return ADI_RTC_ERR_BAD_DEV_HANDLE;

    if (!hDevice->bInitialized)
        return ADI_RTC_ERR_NOT_INITIALIZED;

    if (ADI_RTC_FAILSAFE)
        return ADI_RTC_ERR_CLOCK_FAILSAFE;

    if (Trim & ~ADI_RTC_TRIM_MASK)
        return ADI_RTC_ERR_PARAM_OUT_OF_RANGE;
#endif

    /* pause on pending writes to TRM to avoid data loss */
    if (hDevice->bSafe)
        while (hDevice->pRtc->RTCSR1 & ADI_RTC_WRITE_STATUS_TRIM)
            ;

    ADI_ENTER_CRITICAL_REGION();
    hDevice->pRtc->RTCTRM = Trim & ADI_RTC_TRIM_MASK;
    ADI_EXIT_CRITICAL_REGION();

    return ADI_RTC_SUCCESS;
}


/*************************************************************************************************
**************************************************************************************************
************************************   SYNCHRONIZATION API   *************************************
**************************************************************************************************
*************************************************************************************************/


/*!
 * @brief  Force synchronization of all pending writes.
 *
 * @param[in]   hDevice    Device handle obtained from adi_RTC_Init().
 *
 * @return      Status
 *                - #ADI_RTC_SUCCESS                    Success: Call completed successfully.
 *                - #ADI_RTC_ERR_BAD_DEV_HANDLE [D]     Error: Invalid device handle parameter.
 *                - #ADI_RTC_ERR_CLOCK_FAILSAFE [D]     Error: A failsafe interrupt is pending, see #adi_RTC_Init().
 *                - #ADI_RTC_ERR_NOT_INITIALIZED [D]    Error: Device has not been initialized for use, see #adi_RTC_Init().
 *
 * Blocking call to coerce all outstanding posted RTC register writes to fully flush and synchronize.
 *
 * @sa        adi_RTC_Init().
 * @sa        adi_RTC_EnableSafeWrites().
 * @sa        adi_RTC_GetWriteErrorSource().
 * @sa        adi_RTC_GetWritePendStatus().
 * @sa        adi_RTC_GetWriteSyncStatus().
*/
ADI_RTC_RESULT_TYPE adi_RTC_SynchronizeAllWrites (ADI_RTC_HANDLE hDevice)
{
#ifdef ADI_DEBUG
    if (ADI_RTC_INVALID_HANDLE(hDevice))
        return ADI_RTC_ERR_BAD_DEV_HANDLE;

    if (!hDevice->bInitialized)
        return ADI_RTC_ERR_NOT_INITIALIZED;

    if (ADI_RTC_FAILSAFE)
        return ADI_RTC_ERR_CLOCK_FAILSAFE;
#endif

    /* forced block until all SYNC bits are set (ignore bSafe) */
    while (ADI_RTC_WRITE_STATUS_MASK != (hDevice->pRtc->RTCSR0 & ADI_RTC_WRITE_STATUS_MASK))
        ;

    return ADI_RTC_SUCCESS;
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
    @brief RTC Application callback registration API.

    @param[in]   hDevice     Device handle obtained from adi_RTC_Init().
    @param[in]   cbFunc      Application callback address; the function to call on the interrupt.
    @param[in]   cbWatch     The interrupts to watch, controlling which interrupts result in a callback.

    @return    Status
               - #ADI_RTC_SUCCESS                    Success: The callback is successfully registered.
               - #ADI_RTC_ERR_BAD_DEV_HANDLE [D]     Error: Invalid device handle parameter.
               - #ADI_RTC_ERR_CLOCK_FAILSAFE [D]     Error: A failsafe interrupt is pending, see #adi_RTC_Init().
               - #ADI_RTC_ERR_NOT_INITIALIZED [D]    Error: Device has not been initialized for use, see #adi_RTC_Init().
               - #ADI_RTC_ERR_PARAM_OUT_OF_RANGE [D] Error: The specified watch bits are out of range.

    Registers an application-defined callback \a cbFunc function address of type #ADI_CALLBACK with the RTC device driver.
    Callbacks are made in response to RTC interrupts, depending on whether they are enabled and whether the
    corresponding callback watch bits are set. This allows the application to control which interrupts it wants to be
    called back on.

    The callback to the application is made in context of the originating interrupt, so extended processing
    is discouraged soas to not lock out lower-priority interrupts.

    @note   CALLBACKS: Callbacks are \a disabled by default during RTC device driver initialization
            (#adi_RTC_Init()).  The application may use the #adi_RTC_RegisterCallback() API to
            register an application-provided callback function with the RTC device driver.  The
            callback registration API takes an interrupt mask (of type #ADI_RTC_INT_SOURCE_TYPE)
            to designate which interrupts to watch.  When any of the watched interrupts occur, the
            RTC device driver will make the callback to the application for interrupt handling.
            The application callback should avoid extended processing within the callback as the
            callback is made context of the initiating interrupt and will block lower-priority
            interrupts.  If extended interrupt processing is required, the application should
            schedule it for the main application loop and exit the callback as soon as possible.\n\n

    @note   FAILSAFE: In the case where the RTC power has seen a complete and utter power failure
            (i.e., a full loss of battery power and a full loss of reserve capacitive power) or
            this is a first-time device activation, the RTCFAIL interrupt is set and latched upon
            restoration or first application of power.  This is a \a failsafe state during which
            the RTC clock is unreliable and requires setting.  The failsafe interrupt is latched
            and is not cleared until the application either sets the clock count through the
            #adi_RTC_SetCount() API (which clears the interrupt implicitly) or the application
            clears the interrupt explicitly through the #adi_RTC_ClearInterruptStatus() API.\n\n

    @note   In \b debug mode (ADI_DEBUG macro is asserted), the failsafe state is \b monitored
            by each RTC API (excluding #adi_RTC_Init(), #adi_RTC_SetCount() and
            #adi_RTC_ClearInterruptStatus() APIs).  When the failsafe state is detected, the failsafe
            error code (#ADI_RTC_ERR_CLOCK_FAILSAFE) is returned to the application and RTC driver
            functionality is disabled until the RTC counter is set or until the interrupt is cleared
            manually (via the #adi_RTC_ClearInterruptStatus() API).\n\n

    @note   In \b non-debug mode (ADI_DEBUG macro is not asserted), the failsafe state is \b not
            \b monitored and the failsafe return status is not produced.  This reduces device driver
            footprint and performance overhead in deployed applications.  Also, non-debug builds
            preserve full device driver functionality despite the failsafe state.  If desired, the
            application may dynamically monitor/process the failsafe state in non-debug mode by:
            1) detecting and processing the return status given by the initial #adi_RTC_Init() call,
            and/or 2) registering an RTC interrupt callback (see #adi_RTC_RegisterCallback()),  and
            thereby receive application callback notification(s) when the RTCFAIL interrupt
            (#ADI_RTC_INT_SOURCE_FAIL) fires (among whatever other interrupt watches were set when
            the callback function is registered).\n\n

    @note   The latched failsafe interrupt state is cleared either implicitly by the #adi_RTC_SetCount()
            API (to set the clock), or explicitly by calling the #adi_RTC_ClearInterruptStatus() API to
            clear it manually.\n\n

    @note   The underlying RTCFAIL interrupt driving the failsafe state is a \b non-maskable interrupt
            that is unconditionally enabled to the CPU without dependency on the RTC enablement state.

    @sa    adi_RTC_Init().
    @sa    adi_RTC_EnableDevice().
    @sa    adi_RTC_EnableInterrupts().
    @sa    adi_RTC_EnableSafeWrites().
    @sa    adi_RTC_SetCount().
    @sa    adi_RTC_ClearInterruptStatus().
*/
ADI_RTC_RESULT_TYPE adi_RTC_RegisterCallback (ADI_RTC_HANDLE hDevice, ADI_CALLBACK cbFunc, uint32_t cbWatch)
{

#ifdef ADI_DEBUG
    if (ADI_RTC_INVALID_HANDLE(hDevice))
        return ADI_RTC_ERR_BAD_DEV_HANDLE;

    if (!hDevice->bInitialized)
        return ADI_RTC_ERR_NOT_INITIALIZED;

    if (ADI_RTC_FAILSAFE)
        return ADI_RTC_ERR_CLOCK_FAILSAFE;

    /* if registering a callback */
    if (cbFunc) {

        /* make sure the watch bits are supported */
        if (cbWatch & ~ADI_RTC_INT_SOURCE_MASK)
            return ADI_RTC_ERR_PARAM_OUT_OF_RANGE;
    }
#endif

    /* save the callback info */
    hDevice->cbFunc  = cbFunc;
    hDevice->cbWatch = (ADI_RTC_INT_SOURCE_TYPE)cbWatch;

    return ADI_RTC_SUCCESS;
}


/*! @brief  RTC device driver interrupt handler.  Overrides weakly-bound default interrupt handler in startup.c. */
ADI_INT_HANDLER (RTC_Int_Handler)
{
    ADI_RTC_HANDLE pDev = &gRTC_DevData[ADI_RTC_DEVID_0];  // so far, there is only one RTC, so this is safe
    uint16_t sources, enables;
    ADI_RTC_INT_SOURCE_TYPE fired;

    /* determin qualified interrupt source(s) */

    sources = pDev->pRtc->RTCSR0 & ADI_RTC_INT_SOURCE_MASK;
    enables = pDev->pRtc->RTCCR  & ADI_RTC_INT_ENA_MASK;

    /* start with unmaskable RTCFAIL bit */
    fired = (ADI_RTC_INT_SOURCE_TYPE) (sources & ADI_RTC_INT_SOURCE_FAIL);

    /* need to test each interrupt source and whether it is enabled before notifying */
    /* because each source is latched regardless of whether it is enabled or not :-( */
    if ((ADI_RTC_INT_SOURCE_ALARM & sources)
        && (ADI_RTC_INT_ENA_ALARM & enables))
            fired |= ADI_RTC_INT_SOURCE_ALARM;

    if ((ADI_RTC_INT_SOURCE_LCD_UPDATE & sources)
        && (ADI_RTC_INT_ENA_LCD_UPDATE & enables))
            fired |= ADI_RTC_INT_SOURCE_LCD_UPDATE;

    if ((ADI_RTC_INT_SOURCE_ISO_DONE & sources)
        && (ADI_RTC_INT_ENA_ISO_DONE & enables))
            fired |= ADI_RTC_INT_SOURCE_ISO_DONE;

    if ((ADI_RTC_INT_SOURCE_WRITE_PENDERR & sources)
        && (ADI_RTC_INT_ENA_WRITE_PENDERR & enables))
            fired |= ADI_RTC_INT_SOURCE_WRITE_PENDERR;

    if ((ADI_RTC_INT_SOURCE_WRITE_SYNC & sources)
        && (ADI_RTC_INT_ENA_WRITE_SYNC & enables))
            fired |= ADI_RTC_INT_SOURCE_WRITE_SYNC;

    if ((ADI_RTC_INT_SOURCE_WRITE_PEND & sources)
        && (ADI_RTC_INT_ENA_WRITE_PEND & enables))
            fired |= ADI_RTC_INT_SOURCE_WRITE_PEND;

    /* 'fired' now represents interrupt bits that are both set and enabled... */

    /* if we have a callback... */
    if (pDev->bInitialized && pDev->cbFunc) {

        /* forward to the user if he is watching this interrupt */
        /* pass the "fired" value as the event.  argument param is not used */
        if (pDev->cbWatch & fired)
            pDev->cbFunc (pDev, fired, NULL);
    }

    /* clear the qualified interrupt(s) */
    adi_RTC_ClearInterruptStatus(pDev, fired);
}


#define REVERT_ADI_MISRA_SUPPRESSIONS  /*!< Revert ADI MISRA Suppressions */
#include "misra.h"

/*
** EOF
*/

/*@}*/
