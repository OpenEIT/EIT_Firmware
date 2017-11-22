/*********************************************************************************

Copyright (c) 2010-2014 Analog Devices, Inc. All Rights Reserved.

This software is proprietary to Analog Devices, Inc. and its licensors.  By using 
this software you agree to the terms of the associated Analog Devices Software 
License Agreement.

*********************************************************************************/

/*!
 *****************************************************************************
 * @file:    wut.c
 * @brief:   WUT Device Implementations for ADuCxxx
 * @version: $Revision: 28525 $
 * @date:    $Date: 2014-11-12 14:51:26 -0500 (Wed, 12 Nov 2014) $
 *****************************************************************************/

/*! \addtogroup WUT_Driver WUT Driver
 *  @{
 */

#define ASSERT_ADI_MISRA_SUPPRESSIONS  /*!< Apply ADI MISRA Suppressions */
#include "misra.h"

#include <stddef.h>                 /* for the definition of NULL */
#include "wut.h"
#include "adi_rtos.h"

/* forward declarations... */
/* These are needed in the case where ADI_CFG_ENABLE_RTOS_SUPPORT=1              */
/* The ADI_INSTALL_HANDLER macro will expand to a call to adi_int_InstallHandler */
/* and the handler will be referenced                                            */
/* The non-OSAL expansion of ADI_INSTALL_HANDLER results in a NULL string        */
ADI_INT_HANDLER(WakeUp_Int_Handler);


/*! \struct ADI_WUT_DEV_DATA_TYPE
 * WUT Device instance data structure */
typedef struct ADI_WUT_DEV_DATA_TYPE
{
    ADI_WUT_STATE_TYPE   InitState;  /*!< Current device initialization state. */
    ADI_WUT_TypeDef*     pWut;       /*!< Current device MMR pointer.          */
    IRQn_Type            IRQn;       /*!< Interrupt ID.                        */
    ADI_CALLBACK         cbFunc;     /*!< Callback function                    */
    ADI_WUT_TARGET_TYPE  cbWatch;    /*!< Callback watch bits                  */
} ADI_WUT_DEV_DATA_TYPE;


/* allocate instance data array */
static ADI_WUT_DEV_DATA_TYPE WUT_DevData[ADI_WUT_MAX_DEVID];  /*!< Array of device driver instance data. */

/* handy-dandy internal device data pointer for validation */
static ADI_WUT_DEV_DATA_TYPE* const pWUT0_DevData = &WUT_DevData[0];


/* conditionally create static initialization data based on adi_wdt_config.h settings */
typedef struct {
	uint16_t wdtControlRegister;          /*!< WUT control register */
	uint16_t wdtIntervalRegister;         /*!< WUT interval register */
	uint32_t wdtCompareARegister;         /*!< WUT FieldA comparator register */
	uint32_t wdtCompareBRegister;         /*!< WUT FieldB comparator register */
	uint32_t wdtCompareCRegister;         /*!< WUT FieldC comparator register */
	uint32_t wdtCompareDRegister;         /*!< WUT FieldD comparator register */
	uint16_t wdtInterruptEnableRegister;  /*!< WUT interrupt enable register */
	uint16_t wdtInterruptClearRegister;   /*!< WUT interrupt enable register */
} ADI_WUT_STATIC_INIT_TYPE;

static const ADI_WUT_STATIC_INIT_TYPE gWutStaticInitData[ADI_WUT_MAX_DEVID] = {
	/* single instance of WUT device */
    {
		/* control register */
		( (WUT_CONTROL_DISABLE_FIELDA_UPDATES << BITP_WUT_T2CON_STOP_WUFA)
		| (WUT_CONTROL_CLOCK_SELECT           << BITP_WUT_T2CON_CLK)
		| (WUT_CONTROL_WAKEUP_ENABLE          << BITP_WUT_T2CON_WUEN)
		| (WUT_CONTROL_TIMER_ENABLE           << BITP_WUT_T2CON_ENABLE)
		| (WUT_CONTROL_TIMER_MODE             << BITP_WUT_T2CON_MOD)
		| (WUT_CONTROL_FREEZE_READ            << BITP_WUT_T2CON_FREEZE)
		| (WUT_CONTROL_TIMER_PRESCALE         << BITP_WUT_T2CON_PRE)
		),

		/* interval register */
		(  WUT_FIELDA_INCREMENT_VALUE),

		/* comparator registers */
		(  WUT_COMPARE_FIELDA_VALUE),
		(  WUT_COMPARE_FIELDB_VALUE),
		(  WUT_COMPARE_FIELDC_VALUE),
		(  WUT_COMPARE_FIELDD_VALUE),

		/* interrupt enable register */
		( (WUT_INTERRUPT_ROLL_ENABLE          << BITP_WUT_T2IEN_ROLL)
		| (WUT_INTERRUPT_FIELDA_ENABLE        << BITP_WUT_T2IEN_WUFA)
		| (WUT_INTERRUPT_FIELDB_ENABLE        << BITP_WUT_T2IEN_WUFB)
		| (WUT_INTERRUPT_FIELDC_ENABLE        << BITP_WUT_T2IEN_WUFC)
		| (WUT_INTERRUPT_FIELDD_ENABLE        << BITP_WUT_T2IEN_WUFD)
		),

		/* interrupt clear register */
		( (WUT_INTERRUPT_ROLL_CLEAR           << BITP_WUT_T2CLRI_ROLL)
		| (WUT_INTERRUPT_FIELDA_CLEAR         << BITP_WUT_T2CLRI_WUFA)
		| (WUT_INTERRUPT_FIELDB_CLEAR         << BITP_WUT_T2CLRI_WUFB)
		| (WUT_INTERRUPT_FIELDC_CLEAR         << BITP_WUT_T2CLRI_WUFC)
		| (WUT_INTERRUPT_FIELDD_CLEAR         << BITP_WUT_T2CLRI_WUFD)
		),
    }
};


/*!
 * @brief       Initialize and allocate the Wakeup Timer.
 *
 * @param[in]   devID     Zero-based device index designating which device to initialize.
 * @param[out]  phDevice  The caller's device handle pointer for storing the initialized device instance data.
 *
 * @return      Status
 *              - #ADI_WUT_ERR_ALREADY_INITIALIZED [D]  Specified device has already been initialized.
 *              - #ADI_WUT_ERR_BAD_DEV_ID [D]           Invalid device index.
 *              - #ADI_WUT_SUCCESS                      Call completed successfully.
 *
 * Initialize the WUT device using default internal configuration settings and allocate the device for use.  The caller's
 * device handle is written with the device instance data pointer.  The returned device handle is required to be passed
 * to all subsequent device driver API calls to convey which device instance to operate on.
 *
 * @note    The contents of phDevice will be set to NULL upon failure.
 *
 * @sa      adi_WUT_UnInit().
 */
ADI_WUT_RESULT_TYPE adi_WUT_Init (ADI_WUT_DEV_ID_TYPE const devID, ADI_WUT_DEV_HANDLE* const phDevice)
{
    ADI_WUT_DEV_HANDLE hDevice = &WUT_DevData[devID];
    ADI_WUT_RESULT_TYPE result = ADI_WUT_SUCCESS;

    /* store a bad handle in case of failure */
    *phDevice = (ADI_WUT_DEV_HANDLE) NULL;

#ifdef ADI_DEBUG
    if (devID != ADI_WUT_DEVID_0)
        return ADI_WUT_ERR_BAD_DEV_ID;

    if (ADI_WUT_STATE_UNINITIALIZED != pWUT0_DevData->InitState)
        return ADI_WUT_ERR_ALREADY_INITIALIZED;
#endif

	/* common inits */
    hDevice->InitState      = ADI_WUT_STATE_UNINITIALIZED;
	hDevice->pWut           = pADI_WUT;


	/* assert pre-configured static data, allowing linker to eliminate unused dynamic config APIs */
	ADI_WUT_STATIC_INIT_TYPE const* pInitData = &gWutStaticInitData[devID];

	hDevice->pWut->T2CON   = pInitData->wdtControlRegister;
	hDevice->pWut->T2INC   = pInitData->wdtIntervalRegister;

	hDevice->pWut->T2WUFA1 = (uint16_t)((pInitData->wdtCompareARegister & 0xffff0000) >> 16);
	hDevice->pWut->T2WUFA0 = (uint16_t)((pInitData->wdtCompareARegister & 0x0000ffff) >>  0);

	hDevice->pWut->T2WUFB1 = (uint16_t)((pInitData->wdtCompareBRegister & 0xffff0000) >> 16);
	hDevice->pWut->T2WUFB0 = (uint16_t)((pInitData->wdtCompareBRegister & 0x0000ffff) >>  0);

	hDevice->pWut->T2WUFC1 = (uint16_t)((pInitData->wdtCompareCRegister & 0xffff0000) >> 16);
	hDevice->pWut->T2WUFC0 = (uint16_t)((pInitData->wdtCompareCRegister & 0x0000ffff) >>  0);

	hDevice->pWut->T2WUFD1 = (uint16_t)((pInitData->wdtCompareDRegister & 0xffff0000) >> 16);
	hDevice->pWut->T2WUFD0 = (uint16_t)((pInitData->wdtCompareDRegister & 0x0000ffff) >>  0);

	hDevice->pWut->T2IEN   = pInitData->wdtInterruptEnableRegister;
	hDevice->pWut->T2CLRI  = pInitData->wdtInterruptClearRegister;

	/* interrupt ID */
    hDevice->IRQn = WUT_IRQn;

    /* install interrupt handler and default callback attributes (see abstraction macro for OS in adi_rtos.h) */
    ADI_INSTALL_HANDLER(hDevice->IRQn, WakeUp_Int_Handler);
    hDevice->cbFunc = NULL;
    hDevice->cbWatch = (ADI_WUT_TARGET_TYPE) 0;


    /* mark device initialized */
    hDevice->InitState = ADI_WUT_STATE_INITIALIZED;

    /* write the device data pointer into the caller's handle */
    *phDevice = hDevice;

    return result;
}


/*!
 * @brief      Uninitialize and deallocate the Wakeup Timer.
 *
 * @param[in]  hDevice  Device handle obtained from adi_WUT_Init().
 *
 * @return     Status
 *             - #ADI_WUT_ERR_BAD_DEV_HANDLE [D]   Invalid device handle parameter.
 *             - #ADI_WUT_ERR_NOT_INITIALIZED [D]  Device has not been previously configured for use.
 *             - #ADI_WUT_SUCCESS                  Call completed successfully.
 *
 * Uninitialize and release the allocated WUT device for other use.
 *
 * @sa        adi_WUT_Init().
 */
ADI_WUT_RESULT_TYPE adi_WUT_UnInit(ADI_WUT_DEV_HANDLE const hDevice)
{
    ADI_WUT_RESULT_TYPE result = ADI_WUT_SUCCESS;

#ifdef ADI_DEBUG
    if (pWUT0_DevData != hDevice)
        return ADI_WUT_ERR_BAD_DEV_HANDLE;

    if (hDevice->InitState != ADI_WUT_STATE_INITIALIZED)
        return ADI_WUT_ERR_NOT_INITIALIZED ;
#endif

    if (ADI_WUT_SUCCESS != (result = adi_WUT_SetTimerEnable(hDevice, false)))
        return result;

	/* clear internal state */
    ADI_UNINSTALL_HANDLER(hDevice->IRQn);
    hDevice->InitState    = ADI_WUT_STATE_UNINITIALIZED;
    hDevice->pWut         = NULL;
    hDevice->IRQn         = (IRQn_Type) 0;
    hDevice->cbFunc       = NULL;
    hDevice->cbWatch      = (ADI_WUT_TARGET_TYPE)0;

    return result;
}


/*!
 * @brief  Set the Wakeup Timer clock source.
 *
 * @param[in]  hDevice      Device handle obtained from adi_WUT_Init().
 * @param[in]  ClockSource  Desired clock source; one of the #ADI_WUT_CLOCK_SOURCE_TYPE enumeration values.
 *
 * @return     Status
 *             - #ADI_WUT_ERR_BAD_DEV_HANDLE [D]     Invalid device handle parameter.
 *             - #ADI_WUT_ERR_NOT_INITIALIZED [D]    Device has not been previously configured for use.
 *             - #ADI_WUT_ERR_PARAM_OUT_OF_RANGE [D] Invalid parameter.
 *             - #ADI_WUT_SUCCESS                    Call completed successfully.
 *
 * Select the clock source to drive the wakeup timer's 32-bit up counter.
 * The selected clock is divided by the current clock prescaler value, as set by #adi_WUT_SetPrescaler().
 *
 * @note If selecting the external clock source, #ADI_WUT_CLK_EXTCLK, the external crystal oscillator circuit
 * must be physically present and specifically enabled via the Power Device Driver.
 *
 * @note If and when disabeling the "External Clock" source, make sure it is not also being used
 * as a system clock mutplixer input by some other system resource.
 *
 * @sa        adi_WUT_GetClockSelect().
 * @sa        adi_WUT_SetPrescaler().
 * @sa        adi_PWR_SetLFXTALOscEnable().
 */
ADI_WUT_RESULT_TYPE adi_WUT_SetClockSelect (ADI_WUT_DEV_HANDLE const hDevice, const ADI_WUT_CLOCK_SOURCE_TYPE ClockSource)
{
    ADI_WUT_RESULT_TYPE result = ADI_WUT_SUCCESS;

#ifdef ADI_DEBUG
    if (pWUT0_DevData != hDevice)
        return ADI_WUT_ERR_BAD_DEV_HANDLE;

    if (hDevice->InitState != ADI_WUT_STATE_INITIALIZED)
        return ADI_WUT_ERR_NOT_INITIALIZED;

    if (   (ADI_WUT_CLK_PCLK   != ClockSource)
        && (ADI_WUT_CLK_LFXTAL != ClockSource)
        && (ADI_WUT_CLK_LFOSC  != ClockSource)
       )
        return ADI_WUT_ERR_PARAM_OUT_OF_RANGE;
#endif

    // set new value
    ADI_ENTER_CRITICAL_REGION();
    hDevice->pWut->T2CON &= ~T2CON_CLK_MSK;
    hDevice->pWut->T2CON |= (ClockSource << 9);
    ADI_EXIT_CRITICAL_REGION();

    return result;
}


/*!
 * @brief  Get the Wakeup Timer clock source.
 *
 * @param[in]  hDevice   Device handle obtained from adi_WUT_Init().
 *
 * @return     One of the #ADI_WUT_CLOCK_SOURCE_TYPE enumeration values, indicating the currently active clock select.
 *
 * Get the currently selected clock source for driving the wakeup timer's 32-bit up counter.
 *
 * @sa        adi_WUT_GetClockSelect().
 * @sa        adi_WUT_SetPrescaler().
 */
ADI_WUT_CLOCK_SOURCE_TYPE adi_WUT_GetClockSelect (ADI_WUT_DEV_HANDLE const hDevice)
{
    ADI_WUT_CLOCK_SOURCE_TYPE rtn;

    switch (T2CON_CLK_MSK & hDevice->pWut->T2CON) {
        case T2CON_CLK_PCLK:   rtn = ADI_WUT_CLK_PCLK;   break;
        case T2CON_CLK_LFXTAL: rtn = ADI_WUT_CLK_LFXTAL; break;
        case T2CON_CLK_LFOSC:  rtn = ADI_WUT_CLK_LFOSC;  break;
    }
    return (rtn);
}


/*!
 * @brief  Set Wakeup Timer comparator value(s).
 *
 * @param[in]  hDevice      Device handle obtained from adi_WUT_Init().
 * @param[in]  target       Target comparator(s).
 * @param[in]  value        Comparator value.
 *
 * @return     Status
 *             - #ADI_WUT_ERR_BAD_DEV_HANDLE [D]     Invalid device handle parameter.
 *             - #ADI_WUT_ERR_NOT_INITIALIZED [D]    Device has not been previously configured for use.
 *             - #ADI_WUT_ERR_PARAM_OUT_OF_RANGE [D] Invalid parameter.
 *             - #ADI_WUT_SUCCESS                    Call completed successfully.
 *
 * Set any set of the three, writable 32-bit comparator registers: B, C or D.
 * Both the upper and lower 16-bit comparator registers are set for the target comparator(s).
 * Disables the timer and timer interrupts on entry, restores on exit.  May set multiple comparators
 * to the same value.
 *
 * @note: #ADI_WUT_TARGET_TYPE types #ADI_WUT_COMPA and #ADI_WUT_ROLL are excluded from this API.
 * These target types generate the #ADI_WUT_ERR_PARAM_OUT_OF_RANGE return code in debug mode.
 * These target types generate the #ADI_WUT_SUCCESS return code in non-debug mode, but no action is performed.
 *
 * @sa        adi_WUT_GetComparator().
 * @sa        adi_WUT_SetIntervalA().
 */
ADI_WUT_RESULT_TYPE adi_WUT_SetComparator (ADI_WUT_DEV_HANDLE const hDevice,
                                           const ADI_WUT_TARGET_TYPE target,
                                           const uint32_t value)
{
    ADI_WUT_RESULT_TYPE result = ADI_WUT_SUCCESS;
    bool_t savedEnable, savedInterrupt;
    uint16_t upper, lower;

#ifdef ADI_DEBUG
    if (pWUT0_DevData != hDevice)
        return ADI_WUT_ERR_BAD_DEV_HANDLE;

    if (hDevice->InitState != ADI_WUT_STATE_INITIALIZED)
        return ADI_WUT_ERR_NOT_INITIALIZED;

	/* exclude all but comparators B/C/D */
    if (~(ADI_WUT_COMPB | ADI_WUT_COMPC | ADI_WUT_COMPD) & target)
		return ADI_WUT_ERR_PARAM_OUT_OF_RANGE;

#endif

    /* save/disable timer state */
    if (ADI_WUT_SUCCESS == result)
        savedEnable = adi_WUT_GetTimerEnable(hDevice);
    if (ADI_WUT_SUCCESS == result)
        result = adi_WUT_SetTimerEnable(hDevice, false);

    /* save/disable interrupt state */
    if (ADI_WUT_SUCCESS == result)
        savedInterrupt = adi_WUT_GetInterruptEnable(hDevice, target);
    if (ADI_WUT_SUCCESS == result)
        result = adi_WUT_SetInterruptEnable(hDevice, target, false);

    /* no need to freeze upper/lower comparator writes because the timer is disabled */
    if (ADI_WUT_SUCCESS == result) {

		/* break it up */
        upper = (uint16_t)((value & 0xffff0000) >> 16);
        lower = (uint16_t) (value & 0x0000ffff);

		/* set compB */
		if (ADI_WUT_COMPB & target) {
			hDevice->pWut->T2WUFB1 = upper;
			hDevice->pWut->T2WUFB0 = lower;
		}

		/* set compC */
		if (ADI_WUT_COMPC & target) {
			hDevice->pWut->T2WUFC1 = upper;
			hDevice->pWut->T2WUFC0 = lower;
		}

		/* set compD */
		if (ADI_WUT_COMPD & target) {
            hDevice->pWut->T2WUFD1 = upper;
            hDevice->pWut->T2WUFD0 = lower;
		}
    }

    /* restore state */
    if (ADI_WUT_SUCCESS == result)
        result = adi_WUT_SetInterruptEnable(hDevice, target, savedInterrupt);
    if (ADI_WUT_SUCCESS == result)
        result = adi_WUT_SetTimerEnable(hDevice, savedEnable);

    return result;
}


/*!
 * @brief  Get one of the Wakeup Timer comparators.
 *
 * @param[in]  hDevice      Device handle obtained from adi_WUT_Init().
 * @param[in]  target       Target comparator.
 *
 * @return     Value of comparator designated by the #ADI_WUT_TARGET_TYPE parameter.
 *
 * Get one of the three, 32-bit comparator values: COMPB, COMPC, COMPD.
 * Both the upper and lower 16-bit comparator registers are read for the target comparator.
 * Returns zero for invalid or zero-valued comparators.  #ADI_WUT_TARGET_TYPE types may not
 * be combined, as only the last valid target type will prevail.
 *
 * @note: #ADI_WUT_TARGET_TYPE types #ADI_WUT_COMPA and #ADI_WUT_ROLL are excluded from this API.
 * These target types generate the #ADI_WUT_ERR_PARAM_OUT_OF_RANGE return code in debug mode.
 * These target types generate the #ADI_WUT_SUCCESS return code in non-debug mode, but no action is performed.
 *
 * @sa        adi_WUT_SetComparator().
 */
uint32_t adi_WUT_GetComparator (ADI_WUT_DEV_HANDLE const hDevice, const ADI_WUT_TARGET_TYPE target)
{
    uint32_t result = 0;

    if (ADI_WUT_COMPB & target) {
        result = hDevice->pWut->T2WUFB0;
        result |= (hDevice->pWut->T2WUFB1 << 16);
    }

    if (ADI_WUT_COMPC & target) {
        result = hDevice->pWut->T2WUFC0;
        result |= (hDevice->pWut->T2WUFC1 << 16);
    }

    if (ADI_WUT_COMPD & target) {
        result = hDevice->pWut->T2WUFD0;
        result |= (hDevice->pWut->T2WUFD1 << 16);
    }

    return result;
}


/*!
 * @brief  Set the Wakeup Timer Freeze mode for timer value readbacks.
 *
 * @param[in]  hDevice      Device handle obtained from adi_WUT_Init().
 * @param[in]  bEnable      Enable/disable.
 *
 * @return     Status
 *             - #ADI_WUT_ERR_BAD_DEV_HANDLE [D]     Invalid device handle parameter.
 *             - #ADI_WUT_ERR_NOT_INITIALIZED [D]    Device has not been previously configured for use.
 *             - #ADI_WUT_SUCCESS                    Call completed successfully.
 *
 * Guarantees an atomic readback of both upper and lower timer registers.  Sets the freeze mode for
 * timer value readback, insuring no carry occurs between the upper and lower halfs of 32-bit timer
 * value register while timer reads are performed.  When freeze mode is set, the upper 16-bits (T2VAL1)
 * of the 32-bit timer value are locked following a read of the lower 16-bits (T2TVAL0).  The lock is
 * automatically released after the read of the the upper 16-bits (T2VAL1).  When freeze mode is cleared,
 * an atomic read of the two 16-bit halfs comprising the 32-bit timer value is not guaranteed, i.e.,
 * a carry may propagate across the two 16-bit registers between reading the upper and lower halfs.
 *
 * @sa        adi_WUT_GetFreezeEnable().
 */
ADI_WUT_RESULT_TYPE adi_WUT_SetFreezeEnable (ADI_WUT_DEV_HANDLE const hDevice, const bool_t bEnable)
{
#ifdef ADI_DEBUG
    if (pWUT0_DevData != hDevice)
        return ADI_WUT_ERR_BAD_DEV_HANDLE;

    if (hDevice->InitState != ADI_WUT_STATE_INITIALIZED)
        return ADI_WUT_ERR_NOT_INITIALIZED;
#endif

    ADI_ENTER_CRITICAL_REGION();
    if (bEnable)
        hDevice->pWut->T2CON |= T2CON_FREEZE;
    else
        hDevice->pWut->T2CON &= ~T2CON_FREEZE;
    ADI_EXIT_CRITICAL_REGION();

    return ADI_WUT_SUCCESS;
}


/*!
 * @brief  Get the Wakeup Timer Freeze mode for timer value readbacks.
 *
 * @param[in]  hDevice      Device handle obtained from adi_WUT_Init().
 *
 * @return Returns the current state of the Wakeup Timer Freeze mode.
 *                - #true      Wakeup Timer Freeze mode is set.
 *                - #false     Wakeup Timer Freeze mode is cleared.
 *
 * Gets the freeze mode for timer value readbacks.
 *
 * @sa        adi_WUT_SetFreezeEnable().
 */
bool_t adi_WUT_GetFreezeEnable (ADI_WUT_DEV_HANDLE const hDevice)
{
    return (T2CON_FREEZE & hDevice->pWut->T2CON) ? true : false;
}


/*!
 * @brief  Set/clear Wakeup Timer interrupt enables.
 *
 * @param[in]  hDevice      Device handle obtained from adi_WUT_Init().
 * @param[in]  target       Target comparator(s).
 * @param[in]  bEnable      Interrupt enable flag.
 *
 * @return     Status
 *             - #ADI_WUT_ERR_BAD_DEV_HANDLE [D]     Invalid device handle parameter.
 *             - #ADI_WUT_ERR_NOT_INITIALIZED [D]    Device has not been previously configured for use.
 *             - #ADI_WUT_ERR_PARAM_OUT_OF_RANGE [D] Invalid parameter.
 *             - #ADI_WUT_SUCCESS                    Call completed successfully.
 *
 * Enable or disable any set of Wakeup Timer interrupts.
 * Causes the Wakeup Timer interrupt to fire: when the Wakeup Timer
 * counter reaches the designated target comparator value or, in the case of the
 * rollover interrupt (used only in "free running" mode), when the timer counter
 * has rolled over from full-scale to zero.
 *
 * @sa        adi_WUT_GetInterruptEnable().
 */
ADI_WUT_RESULT_TYPE adi_WUT_SetInterruptEnable (ADI_WUT_DEV_HANDLE const hDevice,
                                                const ADI_WUT_TARGET_TYPE target,
                                                const bool_t bEnable)
{
	/* local copy */
	ADI_WUT_TARGET_TYPE tgt = target;

#ifdef ADI_DEBUG
    if (pWUT0_DevData != hDevice)
        return ADI_WUT_ERR_BAD_DEV_HANDLE;

    if (hDevice->InitState != ADI_WUT_STATE_INITIALIZED)
        return ADI_WUT_ERR_NOT_INITIALIZED;

    if (~(ADI_WUT_TARGET_MASK) & tgt)
        return ADI_WUT_ERR_PARAM_OUT_OF_RANGE;
#endif

	/* apply mask for saftey */
	tgt &= ADI_WUT_TARGET_MASK;

    ADI_ENTER_CRITICAL_REGION();

    if (bEnable)
        hDevice->pWut->T2IEN |= tgt;
    else
        hDevice->pWut->T2IEN &= ~tgt;

    ADI_EXIT_CRITICAL_REGION();

    return ADI_WUT_SUCCESS;
}


/*!
 * @brief  Read the Wakeup Timer interrupt enable state.
 *
 * @param[in]  hDevice      Device handle obtained from adi_WUT_Init().
 * @param[in]  target       Target comparator or rollover interrupt.
 *
 * @return Returns the interrupt enable status of a target comparator.
 *                - #true      Interrupt(s) is(are) enabled.
 *                - #false     Interrupt(s) is(are) disabled.
 *
 * Returns a boolean value indicating if the set of #ADI_WUT_TARGET_TYPEs are enabled or not.
 * May be used to query singular or multiple enables.
 *
 * @sa        adi_WUT_SetInterruptEnable().
 */
bool_t adi_WUT_GetInterruptEnable (ADI_WUT_DEV_HANDLE const hDevice, const ADI_WUT_TARGET_TYPE target)
{
	/* local copy */
	ADI_WUT_TARGET_TYPE tgt = target;

	/* apply mask for saftey */
	tgt &= ADI_WUT_TARGET_MASK;

    return (hDevice->pWut->T2IEN & tgt) ? true : false;
}


/*!
 * @brief  Set the Wakeup Timer Interval register.
 *
 * @param[in]  hDevice      Device handle obtained from adi_WUT_Init().
 * @param[in]  value        New 12-bit interval value.
 *
 * @return     Status
 *             - #ADI_WUT_ERR_BAD_DEV_HANDLE [D]     Invalid device handle parameter.
 *             - #ADI_WUT_ERR_NOT_INITIALIZED [D]    Device has not been previously configured for use.
 *             - #ADI_WUT_ERR_PARAM_OUT_OF_RANGE [D] Invalid parameter.
 *             - #ADI_WUT_SUCCESS                    Call completed successfully.
 *
 * Sets a new 12-bit interval value, stored in the Wakeup Timer T2INC internal register.
 * Also loads bits 16:5 of 32-bit ComparatorA (T2WUFA) with this same value (left-shifted by 5).
 * During normal timer operation, ComparatorA is auto-incremented by the interval
 * value when the timer counter reaches the ComparatorA threshold.  If the auto-increment overflows
 * 0xffffffff (timer full-scale) in free-running mode -- or the ComparatorD (T2WUFD) value in periodic
 * mode -- the modified ComparatorA value is reduced by 0xffffffff or ComparatorD, respectively.
 *
 * @note The timer is halted while setting a new value for the interval register, which may affect timer accuracy.
 *       If the timer was runing prior to the call, it is resumed before exiting this function.
 *
 * @sa        adi_WUT_GetIntervalA().
 */
ADI_WUT_RESULT_TYPE adi_WUT_SetIntervalA (ADI_WUT_DEV_HANDLE const hDevice, const uint16_t value)
{
    ADI_WUT_RESULT_TYPE result = ADI_WUT_SUCCESS;
    bool_t savedEnable;

#ifdef ADI_DEBUG
    if (pWUT0_DevData != hDevice)
        return ADI_WUT_ERR_BAD_DEV_HANDLE;

    if (hDevice->InitState != ADI_WUT_STATE_INITIALIZED)
        return ADI_WUT_ERR_NOT_INITIALIZED;

    /* range check for 12-bit max value */
    if (value & 0xf000)
        return ADI_WUT_ERR_PARAM_OUT_OF_RANGE;
#endif

    /* disable timer during T2INC write */
    savedEnable = adi_WUT_GetTimerEnable(hDevice);
    result = adi_WUT_SetTimerEnable(hDevice, false);

    if (ADI_WUT_SUCCESS == result) {
        hDevice->pWut->T2INC = value;
    }

	/* restore enable state */
	result = adi_WUT_SetTimerEnable(hDevice, savedEnable);

    return result;
}


/*!
 * @brief  Get the Wakeup Timer Interval register.
 *
 * @param[in]  hDevice      Device handle obtained from adi_WUT_Init().
 *
 * @return Returns the 12-bit Wakeup Timer Interval register value.
 *
 * @sa        adi_WUT_SetIntervalA().
 */
uint16_t adi_WUT_GetIntervalA (ADI_WUT_DEV_HANDLE const hDevice)
{
    return hDevice->pWut->T2INC;
}


/*!
 * @brief  Set the Wakeup Timer clock input prescaler.
 *
 * @param[in]  hDevice      Device handle obtained from adi_WUT_Init().
 * @param[in]  PreScaler    Clock prescaler enum.
 *
 * @return     Status
 *             - #ADI_WUT_ERR_BAD_DEV_HANDLE [D]     Invalid device handle parameter.
 *             - #ADI_WUT_ERR_NOT_INITIALIZED [D]    Device has not been previously configured for use.
 *             - #ADI_WUT_ERR_PARAM_OUT_OF_RANGE [D] Invalid parameter.
 *             - #ADI_WUT_SUCCESS                    Call completed successfully.
 *
 * Set the Wakeup Timer clock prescaler.  The clock input is divided by the prescaler factor, resulting
 * in the Wakeup Timer being incremented once every N clock ticks, where N is one of 1, 16, 256 or 32,768,
 * i.e., one of the clock #ADI_WUT_PRESCALER_TYPE enumeration values.
 *
 * @sa        adi_WUT_SetClockSelect().
 * @sa        adi_WUT_GetPrescaler().
 */
ADI_WUT_RESULT_TYPE adi_WUT_SetPrescaler (ADI_WUT_DEV_HANDLE const hDevice, const ADI_WUT_PRESCALER_TYPE PreScaler)
{
#ifdef ADI_DEBUG
    if (pWUT0_DevData != hDevice)
        return ADI_WUT_ERR_BAD_DEV_HANDLE;

    if (hDevice->InitState != ADI_WUT_STATE_INITIALIZED)
        return ADI_WUT_ERR_NOT_INITIALIZED;

    switch (PreScaler) {
        case ADI_WUT_PRE_DIV1:
        case ADI_WUT_PRE_DIV16:
        case ADI_WUT_PRE_DIV256:
        case ADI_WUT_PRE_DIV32768:
            break;
        default:
            return ADI_WUT_ERR_PARAM_OUT_OF_RANGE;
    }
#endif

    ADI_ENTER_CRITICAL_REGION();
    hDevice->pWut->T2CON &= ~T2CON_PRE_MSK;
    hDevice->pWut->T2CON |= PreScaler;
    ADI_EXIT_CRITICAL_REGION();

    return ADI_WUT_SUCCESS;
}


/*!
 * @brief  Get the Wakeup Timer clock input prescaler.
 *
 * @param[in]  hDevice      Device handle obtained from adi_WUT_Init().
 *
 * @return     Currently active clock prescaler.
 *
 * Get the Wakeup Timer clock prescaler.
 *
 * @sa        adi_WUT_SetPrescaler().
 */
ADI_WUT_PRESCALER_TYPE adi_WUT_GetPrescaler (ADI_WUT_DEV_HANDLE const hDevice)
{
    return (ADI_WUT_PRESCALER_TYPE) (T2CON_PRE_MSK & hDevice->pWut->T2CON);
}


/*!
 * @brief  Set the Wakeup Timer enable mode.
 *
 * @param[in]  hDevice      Device handle obtained from adi_WUT_Init().
 * @param[in]  bEnable      Enable/disable.
 *
 * @return     Status
 *             - #ADI_WUT_ERR_BAD_DEV_HANDLE [D]     Invalid device handle parameter.
 *             - #ADI_WUT_ERR_NOT_INITIALIZED [D]    Device has not been previously configured for use.
 *             - #ADI_WUT_SUCCESS                    Call completed successfully.
 *
 * Enables or disables the Wakeup Timer for operation.
 *
 * @sa        adi_WUT_GetTimerEnable().
 */
ADI_WUT_RESULT_TYPE adi_WUT_SetTimerEnable (ADI_WUT_DEV_HANDLE const hDevice, const bool_t bEnable)
{
#ifdef ADI_DEBUG
    if (pWUT0_DevData != hDevice)
        return ADI_WUT_ERR_BAD_DEV_HANDLE;

    if (hDevice->InitState != ADI_WUT_STATE_INITIALIZED)
        return ADI_WUT_ERR_NOT_INITIALIZED;
#endif

    ADI_ENTER_CRITICAL_REGION();
    if (bEnable)
        hDevice->pWut->T2CON |= T2CON_ENABLE;
    else
        hDevice->pWut->T2CON &= ~T2CON_ENABLE;
    ADI_EXIT_CRITICAL_REGION();

    return ADI_WUT_SUCCESS;
}


/*!
 * @brief  Get the Wakeup Timer enable mode.
 *
 * @param[in]  hDevice      Device handle obtained from adi_WUT_Init().
 *
 * @return Returns the timer enable status.
 *                - #true      Timer is enabled.
 *                - #false     Timer is disabled.
 *
 * Gets the current Wakeup Timer enablement state.
 *
 * @sa        adi_WUT_SetTimerEnable().
 */
bool_t adi_WUT_GetTimerEnable (ADI_WUT_DEV_HANDLE const hDevice)
{
    return (T2CON_ENABLE & hDevice->pWut->T2CON) ? true : false;
}


/*!
 * @brief  Set the Wakeup Timer mode.
 *
 * @param[in]  hDevice      Device handle obtained from adi_WUT_Init().
 * @param[in]  Mode         Timer mode enum.
 *
 * @return     Status
 *             - #ADI_WUT_ERR_BAD_DEV_HANDLE [D]     Invalid device handle parameter.
 *             - #ADI_WUT_ERR_NOT_INITIALIZED [D]    Device has not been previously configured for use.
 *             - #ADI_WUT_ERR_PARAM_OUT_OF_RANGE [D] Invalid parameter.
 *             - #ADI_WUT_SUCCESS                    Call completed successfully.
 *
 * Sets the Wakeup Timer operational mode to either freerunning or periodic.
 *
 * @note In periodic mode, the timer automatically resets to zero after reaching the ComparatorD value.
 *
 * @sa        adi_WUT_GetTimerMode().
 */
ADI_WUT_RESULT_TYPE adi_WUT_SetTimerMode (ADI_WUT_DEV_HANDLE const hDevice, const ADI_WUT_MODE_TYPE Mode)
{
#ifdef ADI_DEBUG
    if (pWUT0_DevData != hDevice)
        return ADI_WUT_ERR_BAD_DEV_HANDLE;

    if (hDevice->InitState != ADI_WUT_STATE_INITIALIZED)
        return ADI_WUT_ERR_NOT_INITIALIZED;

    switch (Mode) {
        case ADI_WUT_MODE_FREERUNNING:
        case ADI_WUT_MODE_PERIODIC:
            break;
        default:
            return ADI_WUT_ERR_PARAM_OUT_OF_RANGE;
    }
#endif

    ADI_ENTER_CRITICAL_REGION();
    if (ADI_WUT_MODE_FREERUNNING == Mode)
        hDevice->pWut->T2CON |= T2CON_MOD_FREERUN;
    else
        hDevice->pWut->T2CON &= ~T2CON_MOD_FREERUN;
    ADI_EXIT_CRITICAL_REGION();

    return ADI_WUT_SUCCESS;
}


/*!
 * @brief  Get the Wakeup Timer mode.
 *
 * @param[in]  hDevice      Device handle obtained from adi_WUT_Init().
 *
 * @return Returns the timer mode.
 *                - #ADI_WUT_MODE_FREERUNNING  Timer is in freerunning mode.
 *                - #ADI_WUT_MODE_PERIODIC     Timer is in periodic mode.
 *
 * Get the Wakeup Timer operational mode.
 *
 * @sa        adi_WUT_SetTimerMode().
 */
ADI_WUT_MODE_TYPE adi_WUT_GetTimerMode (ADI_WUT_DEV_HANDLE const hDevice)
{
    return (T2CON_MOD & hDevice->pWut->T2CON) ? ADI_WUT_MODE_FREERUNNING : ADI_WUT_MODE_PERIODIC;
}


/*!
 * @brief  Get the current Wakeup Timer count value.
 *
 * @param[in]  hDevice      Device handle obtained from adi_WUT_Init().
 *
 * @return     Timer value.
 *
 * Get the 32-bit current Wakeup Time count value.
 * Forces the "freeze" mode to insure both upper 16-bits and lower 16-bits reads are atomic.
 * Restores freeze mode before exit.
 *
 */
uint32_t adi_WUT_GetCurrentCount (ADI_WUT_DEV_HANDLE const hDevice)
{
	bool_t savedFreezeMode;
    uint32_t result = 0;

    /* save and set freeze mode */
    savedFreezeMode = adi_WUT_GetFreezeEnable(hDevice);
    adi_WUT_SetFreezeEnable(hDevice, true);

	/* make sure to read lower bits first, so upper bits are frozen */
	result  =  hDevice->pWut->T2VAL0;
	result |= (hDevice->pWut->T2VAL1 << 16);

	/* restore freeze mode */
    adi_WUT_SetFreezeEnable(hDevice, savedFreezeMode);

    return result;
}


/*!
 * @brief  Clear one of the Wakeup Timer interrupt flag.
 *
 * @param[in]  hDevice      Device handle obtained from adi_WUT_Init().
 * @param[in]  target       Target comparator.
 *
 * @return     Status
 *             - #ADI_WUT_ERR_BAD_DEV_HANDLE [D]     Invalid device handle parameter.
 *             - #ADI_WUT_ERR_NOT_INITIALIZED [D]    Device has not been previously configured for use.
 *             - #ADI_WUT_ERR_PARAM_OUT_OF_RANGE [D] Invalid parameter.
 *             - #ADI_WUT_SUCCESS                    Call completed successfully.
 *
 * Clear either the rollover (free-running mode only) or one of the comparator interrupt flags.
 * Uses a write-once-to-clear logic, self-clears the bit and is not readable.
 */
ADI_WUT_RESULT_TYPE adi_WUT_ClearInterruptFlag (ADI_WUT_DEV_HANDLE const hDevice, const ADI_WUT_TARGET_TYPE target)
{
	/* local copy */
	ADI_WUT_TARGET_TYPE tgt = target;

#ifdef ADI_DEBUG
    if (pWUT0_DevData != hDevice)
        return ADI_WUT_ERR_BAD_DEV_HANDLE;

    if (hDevice->InitState != ADI_WUT_STATE_INITIALIZED)
        return ADI_WUT_ERR_NOT_INITIALIZED;

    if (~(ADI_WUT_TARGET_MASK) & tgt)
        return ADI_WUT_ERR_PARAM_OUT_OF_RANGE;
#endif

	/* apply mask for saftey */
	tgt &= ADI_WUT_TARGET_MASK;

	/* just write the clear bits (not read/modify/write, so no need for a critical section) */
	hDevice->pWut->T2CLRI = tgt;

    return ADI_WUT_SUCCESS;
}


/*!
 * @brief      Get the value of Status register.
 *
 * @param[in]  hDevice      Device handle obtained from adi_WUT_Init().
 *
 * @return     Wakeup Timer Status register value.
 */
uint16_t adi_WUT_GetStatusRegister (ADI_WUT_DEV_HANDLE const hDevice)
{
    return (hDevice->pWut->T2STA);
}


/*!
 * @brief  Get the Wakeup enable.
 *
 * @param[in]  hDevice      Device handle obtained from adi_WUT_Init().
 *
 * @return Returns the WakeUp enable status.
 *           - #true   To enable the wake up timer even when the core clock is off.
 *           - #false  To disable the wake up timer even when the core clock is off.
 *
 * Get the enable/disable status of the Wakeup timer when the core clock is off.
 *
 * @sa        adi_WUT_SetWakeUpEnable().
 */
bool_t adi_WUT_GetWakeUpEnable (ADI_WUT_DEV_HANDLE const hDevice)
{
    return (T2CON_WUEN & hDevice->pWut->T2CON) ? true : false;
}


/*!
 * @brief  Set the Wakeup enable.
 *
 * @param[in]  hDevice      Device handle obtained from adi_WUT_Init().
 * @param[in]  bEnable      Enable/disable.
 *
 * @return     Status
 *             - #ADI_WUT_ERR_BAD_DEV_HANDLE [D]     Invalid device handle parameter.
 *             - #ADI_WUT_ERR_NOT_INITIALIZED [D]    Device has not been previously configured for use.
 *             - #ADI_WUT_SUCCESS                    Call completed successfully.
 *
 * Enables or disables the Wakeup timer when the core clock is off.
 *
 * @sa        adi_WUT_GetWakeUpEnable().
 */
ADI_WUT_RESULT_TYPE adi_WUT_SetWakeUpEnable (ADI_WUT_DEV_HANDLE const hDevice, const bool_t bEnable)
{
#ifdef ADI_DEBUG
    if (pWUT0_DevData != hDevice)
        return ADI_WUT_ERR_BAD_DEV_HANDLE;

    if (hDevice->InitState != ADI_WUT_STATE_INITIALIZED)
        return ADI_WUT_ERR_NOT_INITIALIZED;
#endif

    ADI_ENTER_CRITICAL_REGION();
    if (bEnable)
        hDevice->pWut->T2CON |= T2CON_WUEN;
    else
        hDevice->pWut->T2CON &= ~T2CON_WUEN;
    ADI_EXIT_CRITICAL_REGION();

    return ADI_WUT_SUCCESS;
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
    @brief WUT Application callback registration API.

    @param[in]   hDevice     Device handle obtained from adi_WUT_Init().
    @param[in]   cbFunc      Application callback address; the function to call on the interrupt.
    @param[in]   cbWatch     Bitfield indicating which WUT interrupts to watch, resulting a callback.

    @return    Status
               - #ADI_WUT_SUCCESS                    Success: The callback is successfully registered.
               - #ADI_WUT_ERR_BAD_DEV_HANDLE [D]     Error: Invalid device handle parameter.
               - #ADI_WUT_ERR_NOT_INITIALIZED [D]    Error: Device has not been initialized for use, see #adi_WUT_Init().
               - #ADI_WUT_ERR_PARAM_OUT_OF_RANGE [D] Error: The specified watch bits are out of range.

    Registers an application-defined callback \a cbFunc function address of type #ADI_CALLBACK with the WUT device driver.
    Callbacks are made in response to WUT interrupts, depending on whether they are enabled and whether the
    corresponding callback watch bits are set. This allows the application to control which interrupts it wants to be
    called back on.

    The callback to the application is made in context of the originating interrupt, so extended processing
    is discouraged soas to not lock out lower-priority interrupts.

    @note   CALLBACKS: Callbacks are \a disabled by default during WUT device driver initialization
            (#adi_WUT_Init()).  The application may use the #adi_WUT_RegisterCallback() API to
            register an application-provided callback function with the WUT device driver.  The
            callback registration API takes an interrupt target mask (of type #ADI_WUT_TARGET_TYPE)
            to designate which interrupts to watch.  When any of the watched interrupts occur, the
            WUT device driver will make the callback to the application for interrupt handling.
            The application callback should avoid extended processing within the callback as the
            callback is made context of the initiating interrupt and will block lower-priority
            interrupts.  If extended interrupt processing is required, the application should
            schedule it for the main application loop and exit the callback as soon as possible.\n\n

    @sa    adi_WUT_Init().
    @sa    adi_WUT_SetInterruptEnable().
    @sa    adi_WUT_ClearInterruptFlag().
*/
ADI_WUT_RESULT_TYPE adi_WUT_RegisterCallback (ADI_WUT_DEV_HANDLE const hDevice, ADI_CALLBACK const cbFunc, uint32_t const cbWatch)
{

#ifdef ADI_DEBUG
    if (pWUT0_DevData != hDevice)
        return ADI_WUT_ERR_BAD_DEV_HANDLE;

    if (!hDevice->InitState)
        return ADI_WUT_ERR_NOT_INITIALIZED;

    /* if registering a callback */
    if (cbFunc) {

        /* make sure the watch bits are supported */
        if (cbWatch & ~ADI_WUT_TARGET_MASK)
            return ADI_WUT_ERR_PARAM_OUT_OF_RANGE;
    }
#endif

    /* save the callback info */
    hDevice->cbFunc  = cbFunc;
    hDevice->cbWatch = (ADI_WUT_TARGET_TYPE)cbWatch;

    return ADI_WUT_SUCCESS;
}


/*! @brief  WUT device driver interrupt handler.  Overrides weakly-bound default interrupt handler in startup.c. */
ADI_INT_HANDLER (WakeUp_Int_Handler)
{
    ADI_WUT_DEV_HANDLE       pDev = pWUT0_DevData;
    uint16_t                 qualified;

    /* qualified interrupt source(s) are comprised of
      interrupt status latched, enabled, watched and masked */
    qualified  = ADI_WUT_TARGET_MASK;
    qualified &= pDev->pWut->T2STA;
    qualified &= pDev->pWut->T2IEN;
    qualified &= pDev->cbWatch;

    /* if we have a callback and something to call back on... */
    if (pDev->InitState && pDev->cbFunc && qualified) {

        /* forward to the application */
        pDev->cbFunc (pDev, qualified, NULL);
    }

    /* in either case, clear the qualified interrupt(s) */
    adi_WUT_ClearInterruptFlag(pDev,(ADI_WUT_TARGET_TYPE) qualified);
}


#define REVERT_ADI_MISRA_SUPPRESSIONS  /*!< Revert ADI MISRA Suppressions */
#include "misra.h"

/*
** EOF
*/

/*@}*/
