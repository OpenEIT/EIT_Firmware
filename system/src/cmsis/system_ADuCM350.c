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
 * @file:    system.c
 * @brief:   System startup code for ADuCM350
 * @version: $Revision: 28525 $
 * @date:    $Date: 2014-11-12 14:51:26 -0500 (Wed, 12 Nov 2014) $
 *****************************************************************************/

/*! \addtogroup SYS_Driver System Interfaces
 *  @{
 */

#include <stdint.h>
#include "system_ADuCM350.h"

#ifdef RELOCATE_IVT
extern void __relocated_vector_table;
#else
//extern void __vector_start;
//AG MVB
extern void __vector_table;
#endif

/*----------------------------------------------------------------------------
  DEFINES
 *---------------------------------------------------------------------------*/

/* define the system clock constants */
#define __USBPHYCLK 60000000  /*!< System clock constant */
#define __HFOSC     16000000  /*!< System clock constant */
#define __HFXTAL    16000000  /*!< System clock constant (may also be 8000000) */
#define __LFCLK        32768  /*!< System clock constant (same whether internal osc or external xtal) */
// __HFSPLL is now calculated using the programmed clock source and M/N/DIV2
// #define __HFSPLL    32000000  /*!< System clock constant */

/* power control deep-sleep and sleep-on-exit bits */
#define SLEEPDEEP_BIT                    (1 << 2)  /*!< SCR Deep Sleep (SLEEPDEEP) */
#define SLEEPONEXIT_BIT                  (1 << 1)  /*!< SCR Sleep On Exit (SLEEPONEXIT) */

/* For simplicity, use a single timeout value for all clock sources even if expected stabilization times vary wildly */
/* (e.g. LFXTAL needs ~500ms, HFOSC needs ~350ns). While polling the system detects the stabilization immediately    */
/* and exits the polling loop. Hence the only time the part waits the full timeout is when the clock source cannot   */
/* stabilize.                                                                                                        */
/* The timeout is implemented using a "for" loop, and the actual time depends on the iteration time for the loop.    */
/* The values used satisfy different levels of optimization, and can be modified if needed.                          */
/* Before it is used, the value is further scaled with HCLK frequency. This is to avoid huge (>10s) timeouts         */
/* when HCLK frequency is very low.                                                                                  */

/* The expected powerup times are:  */
/* HFXTAL:  10ms                    */
/* LFXTAL:  500ms                   */
/* SPLL:    35us                    */
/* UPLL:    35us                    */
/* HFOSC:   5 cycles                */
/* LFOSC:   5 cycles                */

/* The number of loop iterations that gives us the desired timeout. Assumes HCLK frequency = 1MHz. */
/* 750ms  */
#define CLOCK_POWERUP_TIMEOUT           (750000 / 50 + 1)

/*
 * Hardware bug when disabling LFXTAL
 * To disable LFXTAL we need to write 0 to LFXTALEN twice
 * Between the 2 writes we need to wait a minimum of 2 clock periods of the 32kHz clock
 *
 */
#define LFXTALEN_WORKAROUND                         (1)

/* Assumptions:                                                     */
/*     - HCLK frequency is 1MHz                                     */
/*     - 1 loop iteration takes 8 clock cycles                      */
/* The ideal number is 31 * 2 us, using 70us for extra margin.      */
#define LFXTALEN_WORKAROUND_WAIT_TIME               (70 / 8 + 1)

/* Workaround for the UPLL lock issue */
/* Under certain conditions the UPLL may not lock unless the loop filter is precharged.  */
/* For details see <TBD document>. It is strongly advised to leave this workaround       */
/* enabled, the macro is provided for code readability reasons.                          */
#define UPLL_LOCK_WORKAROUND                        (1)

/* Assumptions:                                                     */
/*     - HCLK frequency is 1MHz                                     */
/*     - 1 loop iteration takes 6 clock cycles                      */
/* Targetting 200us.                                                */
#define UPLL_LOCK_WORKAROUND_PRECHARGE_TIME         (200 / 6 + 1)

/* Settings for USB clock configuration */

/* UPLL frequency = 60MHz */
#define UPLL_MSEL_DEFAULT         ADI_SYS_CLOCK_PLL_MSEL_4
#define UPLL_NSEL_DEFAULT         ADI_SYS_CLOCK_PLL_NSEL_15
#define UPLL_DIV2_DEFAULT         ADI_SYS_CLOCK_PLL_DIV2_DISABLED

/* SPLL frequency = 32MHz */
#define SPLL_MSEL_DEFAULT         ADI_SYS_CLOCK_PLL_MSEL_4
#define SPLL_NSEL_DEFAULT         ADI_SYS_CLOCK_PLL_NSEL_16
#define SPLL_DIV2_DEFAULT         ADI_SYS_CLOCK_PLL_DIV2_ENABLED

/* Settings for the measurement clock configuration */

/* Define one of the following macros:                                                */
/*   - MEAS_CLOCK_SOURCE_SPLL:   if SPLL used to drive root_clk in measurement mode   */
/*   - MEAS_CLOCK_SOURCE_HFXTAL: if HFXTAL used to drive root_clk in measurement mode */
#define MEAS_CLOCK_SOURCE_SPLL

/* If a smooth transition between measurement and USB clock configuration is desirable, */
/* and SPLL is used to clock the system in measurement configuration, then the same     */
/* SPLL settings should be used for both configurations.                                */

/* Those settings are only going to be used if MEAS_CLOCK_SOURCE_SPLL is defined */
/* SPLL frequency = 32MHz, which needs to be further divided using HCLKDIVCNT/PCLKDIVCNT */
#define MEAS_SPLL_MSEL_DEFAULT    ADI_SYS_CLOCK_PLL_MSEL_4
#define MEAS_SPLL_NSEL_DEFAULT    ADI_SYS_CLOCK_PLL_NSEL_16
#define MEAS_SPLL_DIV2_DEFAULT    ADI_SYS_CLOCK_PLL_DIV2_ENABLED


/*----------------------------------------------------------------------------
  Internal Clock Variables
 *---------------------------------------------------------------------------*/
#ifdef ADI_DEBUG
/* not needed unless its debug mode */
static uint32_t lfClock = 0;    /* "lf_clk" coming out of LF mux             */
#endif
static uint32_t hfClock = 0;    /* "root_clk" output of HF mux               */
static uint32_t gpioClock = 0;    /* external GPIO clock                       */

/* OSCCTRL protection unlock key */
#define OSCKEY_UNLOCK                (0xCB14)

/* clock control reset values */
#define CLKCON0_RVAL                (0x0000)
#define CLKCON1_RVAL                (0x1010)
/* no CLKCON2 */
#define CLKCON3_RVAL                (0x0088)
#define CLKCON4_RVAL                (0x008F)
#define CLKCON5_RVAL                (0x0180)

/* HF Mux values (CLKCON0) */
#define HFMUX_INTERNAL_OSC_VAL      (0 << BITP_SYSCLK_CLKCON0_CLKMUX)
#define HFMUX_EXTERNAL_XTAL_VAL     (1 << BITP_SYSCLK_CLKCON0_CLKMUX)
#define HFMUX_SYSTEM_PLL_VAL        (2 << BITP_SYSCLK_CLKCON0_CLKMUX)
#define HFMUX_GPIO_VAL              (3 << BITP_SYSCLK_CLKCON0_CLKMUX)

/*----------------------------------------------------------------------------
  Static functions
 *---------------------------------------------------------------------------*/
static uint32_t                 scaleTimeout                    (uint32_t timeout);
static bool_t                   isLfClkUsed                     (void);

#ifdef ADI_SYSTEM_CLOCK_TRANSITION
static void                     changeClockDividers             (ADI_SYS_CLOCK_STATE_CONFIG_TYPE* pClockStateConfig, bool_t downshiftOnly);
static ADI_SYS_RESULT_TYPE      aEnterMinimalState              (void);
static ADI_SYS_RESULT_TYPE      aEnterMeasurementState          (void);
static ADI_SYS_RESULT_TYPE      aEnterUSBState                  (void);
static ADI_SYS_RESULT_TYPE      aEnterUSBLPMState               (void);
#endif

/*----------------------------------------------------------------------------
  Static variables
 *---------------------------------------------------------------------------*/
static volatile bool_t privateIntOccFlag = false;

#ifdef ADI_SYSTEM_CLOCK_TRANSITION

static ADI_SYS_CLOCK_STATE_TYPE     currentState = ADI_SYS_CLOCK_STATE_INVALID;

/*----------------------------------------------------------------------------
  State parameters initialization
 *---------------------------------------------------------------------------*/
 static ADI_SYS_CLOCK_STATE_CONFIG_TYPE ClockStateConfig[] = {
    /* Note: Index 0 is an unused placeholder and does not correspond to a real state.
       It is present to align the state enumerator that starts at one (not zero)
       with the clock divider table indexing.  This allows the NULL state to flag
       invalid transitions in the state transition table without confusion with
       the ADI_SYS_CLOCK_STATE_MINIMAL state.
    */

     /* Also note:
        Make sure this initializer stays ordered exactly as ADI_SYS_CLOCK_STATE_TYPE.
     */

    /*                                      HCLKDIVCNT  PCLKDIVCNT  */
    [ADI_SYS_CLOCK_STATE_INVALID] =         {       16,         16},
    [ADI_SYS_CLOCK_STATE_MINIMAL] =         {       16,         16},
#ifdef MEAS_CLOCK_SOURCE_SPLL
    /* SPLL is configured to output 32MHz, need to divide by 2 here again */
    [ADI_SYS_CLOCK_STATE_MEASUREMENT] =     {        2,         32},
#else
    [ADI_SYS_CLOCK_STATE_MEASUREMENT] =     {        1,         16},
#endif
    [ADI_SYS_CLOCK_STATE_USB] =             {        2,          2},
    [ADI_SYS_CLOCK_STATE_USB_MEASUREMENT] = {        2,          2},    // Not used currently
    [ADI_SYS_CLOCK_STATE_USB_LPM] =         {        1,         16},
};
#endif

/*----------------------------------------------------------------------------
  Clock functions
 *---------------------------------------------------------------------------*/
/**
 * Initialize the system
 *
 * @return none
 *
 * @brief  Setup the microcontroller system.
 *         Initialize the System and update the SystemFrequency variable.
 */
void SystemInit (void)
{

/* version check */
#if (ADI_VERSION_CURRENT < ADI_VERSION_2_1_0_0)
#error "Unsupported version."
#endif

    /* Switch the Interrupt Vector Table Offset Register
     * (VTOR) to point to the relocated IVT in SRAM
     */

    ADI_ENTER_CRITICAL_REGION();  // do all this in safe way

    // switch from boot ROM IVT to application's IVT
    // set the System Control Block, Vector Table Offset Register
#ifdef RELOCATE_IVT
    SCB->VTOR = (uint32_t) &__relocated_vector_table;
#else
    //SCB->VTOR = (uint32_t) &__vectors_start; // MVB
    //AG
    SCB->VTOR = (uint32_t) &__vector_table;
#endif

    // set all three (USGFAULTENA, BUSFAULTENA, and MEMFAULTENA) fault enable bits
    // in the System Control Block, System Handler Control and State Register
    // otherwise these faults are handled as hard faults
    SCB->SHCSR = SCB_SHCSR_USGFAULTENA_Msk |
                 SCB_SHCSR_BUSFAULTENA_Msk |
                 SCB_SHCSR_MEMFAULTENA_Msk ;

    // flush instruction and data pipelines to insure assertion of new settings
    __ISB();  // MUST OCCURE IMMEDIATELY AFTER UPDATING SCB->CPACR!!!
    __DSB();

    ADI_EXIT_CRITICAL_REGION();

    /* restore clocks to a known power-up reset condition */

    /*
        Reset sequence should be...
        1) OSCCTRL. Set bit 1 HFOSCEN. This will bring up the internal oscillator.
        2) CLKCON0. Clear bits 1:0 to 00. This will bring in the internal oscillator as the
           system clock source. Note, you do not need a wait time for steps 1 or 2.
        3) Simply step through OSCCTRL and CLKCON0,1,3,4,5 in order and set these to their
           reset states.

        NOTE for soft-reset via NVIC:
        Before issuing a SYSRESETREQ, allowing the Cortex-M3 to assert a reset request signal to
        the system�s reset generator, the HFOSCEN (OSCCTRL bit 1) must be set. This will ensure
        that all system componenets will be reset properly. This is independent of Root Clock Mux
        and SPLL Clock Mux settings.

    */

    /* Enable internal HF & LF oscillators */
    pADI_PWR->OSCKEY = OSCKEY_UNLOCK;
    pADI_PWR->OSCCTRL |= ((1 << BITP_PWR_OSCCTRL_HFOSCEN) | (1 << BITP_PWR_OSCCTRL_LFOSCEN));

    /* wait for HF OSC to stabilize */
    while ( !(pADI_PWR->OSCCTRL & (1 << BITP_PWR_OSCCTRL_HFOSCOK)) );

    /* Switch over to the internal HF oscillator */
    pADI_SYSCLK->CLKCON0 &= ~(BITM_SYSCLK_CLKCON0_CLKMUX);

    /* complete remaining reset sequence */
    pADI_SYSCLK->CLKCON0 = CLKCON0_RVAL;
    pADI_SYSCLK->CLKCON1 = CLKCON1_RVAL;
    /* no CLKCON2 */
    pADI_SYSCLK->CLKCON3 = CLKCON3_RVAL;
    pADI_SYSCLK->CLKCON4 = CLKCON4_RVAL;
    pADI_SYSCLK->CLKCON5 = CLKCON5_RVAL;

    /* Change the PLL input source from the default HFOSC to HFXTAL */
    /* to satisfy the majority of use cases. It has no effect if    */
    /* no PLL is used.                                              */
    SetSystemClockMux(ADI_SYS_CLOCK_MUX_SPLL_HF_XTAL);

    /* disable external HF crystal oscillator */
    /* (don't disable LF crystal or the RTC will lose time */
    pADI_PWR->OSCKEY = OSCKEY_UNLOCK;
    pADI_PWR->OSCCTRL &= ~BITM_PWR_OSCCTRL_HFXTALEN;

    /* compute new internal clocks based on the newly reset controller */
    SystemCoreClockUpdate();
}


/**
 * @brief  Update internal SystemCoreClock variables
 *
 * @return none
 *
 * Updates the internal SystemCoreClocks with current core
 * Clock retrieved from cpu registers.
 */
void SystemCoreClockUpdate (void)
{
    uint32_t    val;
    uint32_t    tmp;

    /* Update Core Clock sources */

#ifdef ADI_DEBUG
    /* "lfclock" is only used during debug checks... */
    /* LF clock is always 32k, whether osc or xtal */
    lfClock = __LFCLK;  /* for beep, wdt and lcd */
#endif

    /* update the HF clock */
    switch (pADI_SYSCLK->CLKCON0 & BITM_SYSCLK_CLKCON0_CLKMUX ) {

        case HFMUX_INTERNAL_OSC_VAL:
        	hfClock = __HFOSC;
        	break;

        case HFMUX_EXTERNAL_XTAL_VAL:
        	hfClock = __HFXTAL;
        	break;

        case HFMUX_SYSTEM_PLL_VAL:
            /* Calculate System PLL output frequency */
            if (pADI_SYSCLK->CLKCON0 & BITM_SYSCLK_CLKCON0_PLLMUX) {
                /* PLL input from HFXTAL */
                val = __HFXTAL;
            } else {
                /* PLL input from HFOSC */
                val = __HFOSC;
            }

            /* PLL N multiplier */
            tmp = (pADI_SYSCLK->CLKCON3 & BITM_SYSCLK_CLKCON3_SPLLNSEL) >> BITP_SYSCLK_CLKCON3_SPLLNSEL;
            val = val * tmp;

            /* PLL M divider */
            tmp = pADI_SYSCLK->CLKCON3 & BITM_SYSCLK_CLKCON3_SPLLMSEL;
            /* Adjustment needs to be made if M is 2 or 4, if M is 1 leaves it as is */
            switch (tmp) {
                case ADI_SYS_CLOCK_PLL_MSEL_4:
                    /* Divide by 4 */
                    val = val >> 2;
                    break;
                case ADI_SYS_CLOCK_PLL_MSEL_2:
                    /* Divide by 2 */
                    val = val >> 1;
                    break;
            }

            /* PLL divide by 2 */
            if (pADI_SYSCLK->CLKCON3 & BITM_SYSCLK_CLKCON3_SPLLDIV2) {
                val = val >> 1;
            }

            hfClock = val;
            break;

        case HFMUX_GPIO_VAL:
        	hfClock = gpioClock;
        	break;

    } /* end switch */
}


/**
 * @brief  Sets the system external clock frequency
 *
 * @param  ExtClkFreq   External clock frequency in Hz
 * @return none
 *
 * Sets the clock frequency of the source connected to the external GPIO clock input source
 */
void SetSystemExtClkFreq (uint32_t ExtClkFreq)
{
    gpioClock = ExtClkFreq;
}


/**
 * @brief  Select a system clock mux
 *
 * @param  dev Device ID
 *
 * @return none
 *
 * Setup a microcontroller system clock multiplexer.
 * Initialize the System clock multiplexer and update the SystemCoreClock variable.
 */
void SetSystemClockMux (ADI_SYS_CLOCK_MUX_ID id)
{
    uint16_t mask;
    uint16_t mux;
    uint16_t tmp;

    switch (id) {

        case ADI_SYS_CLOCK_MUX_SPLL_HF_OSC:
            mux  = 0 << BITP_SYSCLK_CLKCON0_PLLMUX;
            mask = BITM_SYSCLK_CLKCON0_PLLMUX;
            break;

        case ADI_SYS_CLOCK_MUX_SPLL_HF_XTAL:
            mux  = 1 << BITP_SYSCLK_CLKCON0_PLLMUX;
            mask = BITM_SYSCLK_CLKCON0_PLLMUX;
            break;

        case ADI_SYS_CLOCK_MUX_LFCLK_LFOSC:
            mux  = 0 << BITP_SYSCLK_CLKCON0_LFCLKMUX;
            mask = BITM_SYSCLK_CLKCON0_LFCLKMUX;
            break;

        case ADI_SYS_CLOCK_MUX_LFCLK_LFXTAL:
            mux  = 1 << BITP_SYSCLK_CLKCON0_LFCLKMUX;
            mask = BITM_SYSCLK_CLKCON0_LFCLKMUX;
            break;

        case ADI_SYS_CLOCK_MUX_OUTPUT_ROOT_CLK:
            mux  = 0 << BITP_SYSCLK_CLKCON0_CLKCOUT;
            mask = BITM_SYSCLK_CLKCON0_CLKCOUT;
            break;

        case ADI_SYS_CLOCK_MUX_OUTPUT_LF_CLK:
            mux  = 1 << BITP_SYSCLK_CLKCON0_CLKCOUT;
            mask = BITM_SYSCLK_CLKCON0_CLKCOUT;
            break;

        case ADI_SYS_CLOCK_MUX_OUTPUT_CT_CLK:
            mux  = 2 << BITP_SYSCLK_CLKCON0_CLKCOUT;
            mask = BITM_SYSCLK_CLKCON0_CLKCOUT;
            break;

        case ADI_SYS_CLOCK_MUX_OUTPUT_HCLK_BUS:
            mux  = 3 << BITP_SYSCLK_CLKCON0_CLKCOUT;
            mask = BITM_SYSCLK_CLKCON0_CLKCOUT;
            break;

        case ADI_SYS_CLOCK_MUX_OUTPUT_HCLK_CORE:
            mux  = 4 << BITP_SYSCLK_CLKCON0_CLKCOUT;
            mask = BITM_SYSCLK_CLKCON0_CLKCOUT;
            break;

        case ADI_SYS_CLOCK_MUX_OUTPUT_P_CLK:
            mux  = 5 << BITP_SYSCLK_CLKCON0_CLKCOUT;
            mask = BITM_SYSCLK_CLKCON0_CLKCOUT;
            break;

        case ADI_SYS_CLOCK_MUX_OUTPUT_USB_CTRL_CLK:
            mux  = 6 << BITP_SYSCLK_CLKCON0_CLKCOUT;
            mask = BITM_SYSCLK_CLKCON0_CLKCOUT;
            break;

        case ADI_SYS_CLOCK_MUX_OUTPUT_USB_PHY_CLK:
            mux  = 7 << BITP_SYSCLK_CLKCON0_CLKCOUT;
            mask = BITM_SYSCLK_CLKCON0_CLKCOUT;
            break;

        case ADI_SYS_CLOCK_MUX_OUTPUT_GPT0_CLK:
            mux  = 8 << BITP_SYSCLK_CLKCON0_CLKCOUT;
            mask = BITM_SYSCLK_CLKCON0_CLKCOUT;
            break;

        case ADI_SYS_CLOCK_MUX_OUTPUT_GPT1_CLK:
            mux  = 9 << BITP_SYSCLK_CLKCON0_CLKCOUT;
            mask = BITM_SYSCLK_CLKCON0_CLKCOUT;
            break;

        case ADI_SYS_CLOCK_MUX_OUTPUT_GPT2_CLK:
            mux  = 10 << BITP_SYSCLK_CLKCON0_CLKCOUT;
            mask = BITM_SYSCLK_CLKCON0_CLKCOUT;
            break;

        case ADI_SYS_CLOCK_MUX_OUTPUT_WUT_CLK:
            mux  = 11 << BITP_SYSCLK_CLKCON0_CLKCOUT;
            mask = BITM_SYSCLK_CLKCON0_CLKCOUT;
            break;

        case ADI_SYS_CLOCK_MUX_OUTPUT_RTC_CNT_CLK:
            mux  = 12 << BITP_SYSCLK_CLKCON0_CLKCOUT;
            mask = BITM_SYSCLK_CLKCON0_CLKCOUT;
            break;

        case ADI_SYS_CLOCK_MUX_ROOT_HFOSC:
            mux  = 0 << BITP_SYSCLK_CLKCON0_CLKMUX;
            mask = BITM_SYSCLK_CLKCON0_CLKMUX;
            break;

        case ADI_SYS_CLOCK_MUX_ROOT_HFXTAL:
            mux  = 1 << BITP_SYSCLK_CLKCON0_CLKMUX;
            mask = BITM_SYSCLK_CLKCON0_CLKMUX;
            break;

        case ADI_SYS_CLOCK_MUX_ROOT_SPLL:
            mux  = 2 << BITP_SYSCLK_CLKCON0_CLKMUX;
            mask = BITM_SYSCLK_CLKCON0_CLKMUX;
            break;

        case ADI_SYS_CLOCK_MUX_ROOT_GPIO:
            mux  = 3 << BITP_SYSCLK_CLKCON0_CLKMUX;
            mask = BITM_SYSCLK_CLKCON0_CLKMUX;
            break;

    } /* end switch */

    /* update the mux setting inside a critical region */
    ADI_ENTER_CRITICAL_REGION();

        /* change in a tmp variable and write entire new value all at once */
        tmp = pADI_SYSCLK->CLKCON0;
        tmp &= ~(mask);              /* make a hole */
        tmp |= mux;                  /* fill it */
        pADI_SYSCLK->CLKCON0 = tmp;  /* write it */

    ADI_EXIT_CRITICAL_REGION();

    /* compute new internal clocks based on the changed multiplexer */
    SystemCoreClockUpdate();
}

/**
 * @brief  Read the value of a system clock mux.
 *
 * @param  id      Multiplexor group identifier.
 *
 * @return Multiplexor select value.
 *
 */
ADI_SYS_CLOCK_MUX_ID GetSystemClockMux(ADI_SYS_CLOCK_MUX_GROUP_ID id) {
    uint16_t mask;
    uint16_t position;
    uint16_t offset;
    uint16_t tmp;

    /* The following logic depends on the enumeration order being correct */
    switch (id) {

        /* SPLL/UPLL clock source */
        case ADI_SYS_CLOCK_MUX_SPLL:
            mask =      BITM_SYSCLK_CLKCON0_PLLMUX;
            position =  BITP_SYSCLK_CLKCON0_PLLMUX;
            offset =    ADI_SYS_CLOCK_MUX_SPLL_HF_OSC;
            break;

        /* lf_clk source */
        case ADI_SYS_CLOCK_MUX_LFCLK:
            mask =      BITM_SYSCLK_CLKCON0_LFCLKMUX;
            position =  BITP_SYSCLK_CLKCON0_LFCLKMUX;
            offset =    ADI_SYS_CLOCK_MUX_LFCLK_LFOSC;
            break;

        /* Clock output clock source */
        case ADI_SYS_CLOCK_MUX_OUTPUT:
            mask =      BITM_SYSCLK_CLKCON0_CLKCOUT;
            position =  BITP_SYSCLK_CLKCON0_CLKCOUT;
            offset =    ADI_SYS_CLOCK_MUX_OUTPUT_ROOT_CLK;
            break;

        /* root_clk source */
        case ADI_SYS_CLOCK_MUX_ROOT:
            mask =      BITM_SYSCLK_CLKCON0_CLKMUX;
            position =  BITP_SYSCLK_CLKCON0_CLKMUX;
            offset =    ADI_SYS_CLOCK_MUX_ROOT_HFOSC;
            break;

    } /* end switch */

    tmp = (pADI_SYSCLK->CLKCON0 & mask) >> position;
    tmp += offset;

    return (ADI_SYS_CLOCK_MUX_ID)tmp;

}



/**
 * @brief  Gets the system external clock frequency
 *
 * @return External Clock frequency
 *
 * Gets the clock frequency of the source connected to the external GPIO clock input source
 */
uint32_t GetSystemExtClkFreq (void)
{
#ifdef ADI_DEBUG
    // trap here if the app fails to call SystemInit().
    if (0 == gpioClock)
        while(1);  // MUST CALL SetSystemExtClkFreq() to use GPIO-based clock: FIX APP CODE!!!
#endif

    return  gpioClock;
}


/*!
    @brief      System set clock divide factor for an individual clock group.

    @param[in]  id       Clock identifier

    @return     Currently configured frequency of operation for specified clock (Hz).

    @details    Obtain individual peripheral clock frequencies.

    @sa         adi_PWR_SetClockDivide
    @sa         adi_PWR_GetClockDivide
    @sa         SystemSetClockDivider
*/
uint32_t SystemGetClockFrequency (ADI_SYS_CLOCK_ID id)
{

    uint32_t src, div;

#ifdef ADI_DEBUG
    // trap here if the app fails to call SystemInit().
    if (0 == hfClock || 0 == lfClock)
        while(1);  // MUST CALL SystemInit() to get valid clock internals: FIX APP CODE!!!
#endif

    /* refresh internal clock variables */
    SystemCoreClockUpdate();

    switch (id) {

        /* USB domain */
        case ADI_SYS_CLOCK_USBPHY:
        case ADI_SYS_CLOCK_USBCTL:
            src = __USBPHYCLK;

            if (1 == ((pADI_SYSCLK->CLKCON1 & BITM_SYSCLK_CLKCON1_HCLKDIVCNT) >> BITP_SYSCLK_CLKCON1_HCLKDIVCNT) ) {

                /* if HCLKDIVCNT == 1, divide is 1 */
                div = 1;

            } else {

                /* if HCLKDIVCNT != 1, divide is based on USBCTLCLKDIVMUX */

                /* start with HCLKDIVCNT... */
                div = (pADI_SYSCLK->CLKCON1 & BITM_SYSCLK_CLKCON1_HCLKDIVCNT) >> BITP_SYSCLK_CLKCON1_HCLKDIVCNT;

                if (pADI_SYSCLK->CLKCON1 & BITM_SYSCLK_CLKCON1_USBCTLCLKDIVMUX) {

                    /* if USBCTLCLKDIVMUX bit is set, divider is 1/2 HCLKDIVCNT */
                    div >>= 2;
                }
            }
            break;

        /* PCLOCK domain */
        case ADI_SYS_CLOCK_PCLK:
        case ADI_SYS_CLOCK_I2C:
        case ADI_SYS_CLOCK_UART:
        case ADI_SYS_CLOCK_I2S:
        case ADI_SYS_CLOCK_SPI0:
        case ADI_SYS_CLOCK_SPI1:
            src = hfClock;
            div = (pADI_SYSCLK->CLKCON1 & BITM_SYSCLK_CLKCON1_PCLKDIVCNT) >> BITP_SYSCLK_CLKCON1_PCLKDIVCNT;
            break;

        /* HCLOCK & ACLOCK domain */
        case ADI_SYS_CLOCK_SPIH:
        case ADI_SYS_CLOCK_CORE:
        case ADI_SYS_CLOCK_BUS:
        case ADI_SYS_CLOCK_FCLK:
        case ADI_SYS_CLOCK_HCLK_CT:
        case ADI_SYS_CLOCK_ACLK:
        case ADI_SYS_CLOCK_CT:
        case ADI_SYS_CLOCK_AFE:
            src = hfClock;
            div = (pADI_SYSCLK->CLKCON1 & BITM_SYSCLK_CLKCON1_HCLKDIVCNT) >> BITP_SYSCLK_CLKCON1_HCLKDIVCNT;
            break;

#ifdef ADI_DEBUG
        default:
            return ADI_SYS_ERR_INVALID_CLOCK_ID;
#endif  /*ADI_DEBUG*/

    } /* end switch */

    return src/div;
}


/*!
    @brief      Enable/disable individual peripheral clocks.

    @param[in]  id       Clock identifier
    @param[in]  bEnable          Flag to indicate enable/disable individual clock gate.
                                 'true' to enable and 'false' to disable

    @return     Status
                    - #ADI_SYS_SUCCESS if successfully managed the clock gate.
                    - #ADI_SYS_ERR_INVALID_CLOCK_ID [D] if the specified clock is invalid.

    @details    Manage individual clock gates.
*/
ADI_SYS_RESULT_TYPE SystemEnableClock (ADI_SYS_CLOCK_GATE_ID id, bool_t bEnable)
{
    uint16_t mask;

    /* note: not all clocks are user-gatable... */
    switch (id) {
        /* note: bit 6 is reserved */
        case ADI_SYS_CLOCK_GATE_CTCLK:    mask = BITM_SYSCLK_CLKCON5_CTCLKOFF;    break;
        case ADI_SYS_CLOCK_GATE_ACLK:    mask = BITM_SYSCLK_CLKCON5_ACLKOFF;        break;
        case ADI_SYS_CLOCK_GATE_I2S:    mask = BITM_SYSCLK_CLKCON5_UCLKI2SOFF;    break;
        case ADI_SYS_CLOCK_GATE_UART:    mask = BITM_SYSCLK_CLKCON5_UCLKUARTOFF;    break;
        case ADI_SYS_CLOCK_GATE_I2C:    mask = BITM_SYSCLK_CLKCON5_UCLKI2COFF;    break;
        case ADI_SYS_CLOCK_GATE_SPIH:    mask = BITM_SYSCLK_CLKCON5_UCLKSPIHOFF;    break;
        case ADI_SYS_CLOCK_GATE_SPI1:    mask = BITM_SYSCLK_CLKCON5_UCLKSPI1OFF;    break;
        case ADI_SYS_CLOCK_GATE_SPI0:    mask = BITM_SYSCLK_CLKCON5_UCLKSPI0OFF;    break;
#ifdef ADI_DEBUG
        default:
            return ADI_SYS_ERR_INVALID_CLOCK_ID;
#endif

    } /* end switch */

    /* update the Clock Gate register in a critical region */
    ADI_ENTER_CRITICAL_REGION();

        /* NOTE NEGATIVE LOGIC!!! */
        if (bEnable) {
            /* clear disable bit */
            pADI_SYSCLK->CLKCON5 &= ~mask;
        } else {
            /* set disable bit */
            pADI_SYSCLK->CLKCON5 |= mask;
        }

    /* end critical region */
    ADI_EXIT_CRITICAL_REGION();

    return ADI_SYS_SUCCESS;
}


/*!
    @brief      System set clock divide factor for an individual clock group.

    @param[in]  id       Clock domain identifier
    @param[in]  div      Clock divide value to be set (right-justified uint8_t)

    @return     Status
                    - #ADI_SYS_SUCCESS if successfully set the given clock divide factor.
                    - #ADI_SYS_ERR_CLOCK_DIVIDE_OUT_OF_RANGE [D] if the divider is out of range.
                    - #ADI_SYS_ERR_INVALID_CLOCK_ID [D] if the specified clock is invalid.
                    - #ADI_SYS_ERR_INVALID_CLOCK_SPEED [D] if the specified clock speed invalid.
                    - #ADI_SYS_ERR_INVALID_CLOCK_RATIO [D] if the specified clock ratio invalid.

    @details    Manage individual peripheral clock dividers.

    @sa         adi_PWR_SetClockDivide
    @sa         adi_PWR_GetClockDivide
    @sa         SystemGetClockFrequency
*/
ADI_SYS_RESULT_TYPE SetSystemClockDivider (ADI_SYS_CLOCK_ID id, uint8_t div)
{
    uint16_t mask, value, tmp;

#ifdef ADI_DEBUG
    uint16_t hdiv, pdiv;

    /* test for max 6-bit divider */
    if (div & ~(0x3F))
        return ADI_SYS_ERR_CLOCK_DIVIDE_OUT_OF_RANGE;
#endif  /*ADI_DEBUG*/

    switch (id) {

        case ADI_SYS_CLOCK_USBPHY:
        case ADI_SYS_CLOCK_USBCTL:
#ifdef ADI_DEBUG
            /* check for single LSB for USBCTRLCLKDIVMIX*/
            if (div & (~0x01))
                return ADI_SYS_ERR_CLOCK_DIVIDE_OUT_OF_RANGE;
#endif  /*ADI_DEBUG*/
            mask = BITM_SYSCLK_CLKCON1_USBCTLCLKDIVMUX;
            value = div << BITP_SYSCLK_CLKCON1_USBCTLCLKDIVMUX;
            break;

        case ADI_SYS_CLOCK_PCLK:
        case ADI_SYS_CLOCK_I2C:
        case ADI_SYS_CLOCK_UART:
        case ADI_SYS_CLOCK_I2S:
        case ADI_SYS_CLOCK_SPI0:
        case ADI_SYS_CLOCK_SPI1:
#ifdef ADI_DEBUG
            /* verify requested PCLK freq is <= HCLK */
            pdiv = div;
            hdiv = (pADI_SYSCLK->CLKCON1 & BITM_SYSCLK_CLKCON1_HCLKDIVCNT) >> BITP_SYSCLK_CLKCON1_HCLKDIVCNT;
            if (hdiv > pdiv) {
                return ADI_SYS_ERR_INVALID_CLOCK_SPEED;
            }

            /* verify new PDIV:HDIV ratio will be integral */
            if (pdiv % hdiv) {
                return ADI_SYS_ERR_INVALID_CLOCK_RATIO;
            }
#endif  /*ADI_DEBUG*/
            mask = BITM_SYSCLK_CLKCON1_PCLKDIVCNT;
            value = div << BITP_SYSCLK_CLKCON1_PCLKDIVCNT;
            break;

        /* HCLOCK & ACLOCK share same divider... */
        case ADI_SYS_CLOCK_SPIH:
        case ADI_SYS_CLOCK_CORE:
        case ADI_SYS_CLOCK_BUS:
        case ADI_SYS_CLOCK_FCLK:
        case ADI_SYS_CLOCK_HCLK_CT:
        case ADI_SYS_CLOCK_ACLK:
        case ADI_SYS_CLOCK_CT:
        case ADI_SYS_CLOCK_AFE:
#ifdef ADI_DEBUG
            /* verify PCLK freq is <= requested HCLK */
            pdiv = (pADI_SYSCLK->CLKCON1 & BITM_SYSCLK_CLKCON1_PCLKDIVCNT) >> BITP_SYSCLK_CLKCON1_PCLKDIVCNT;
            hdiv = div;
            if (hdiv > pdiv) {
                return ADI_SYS_ERR_INVALID_CLOCK_SPEED;
            }

            /* verify new PDIV:HDIV ratio will be integral */
            if (pdiv % hdiv) {
                return ADI_SYS_ERR_INVALID_CLOCK_RATIO;
            }
#endif  /*ADI_DEBUG*/
            mask = BITM_SYSCLK_CLKCON1_HCLKDIVCNT;
            value = div << BITP_SYSCLK_CLKCON1_HCLKDIVCNT;
            break;

#ifdef ADI_DEBUG
        default:
            return ADI_SYS_ERR_INVALID_CLOCK_ID;
#endif  /*ADI_DEBUG*/

    } /* end switch */


    /* critical region */
    ADI_ENTER_CRITICAL_REGION();

        /* read-modify-write without any interrupts */
		/* change in a tmp variable and write entire new value all at once */
		tmp = pADI_SYSCLK->CLKCON1;
		tmp &= ~mask;                       /* blank the field */
		tmp |= value;                       /* set the new value */
		pADI_SYSCLK->CLKCON1 = tmp;         /* write the new value */

    /* end critical region */
    ADI_EXIT_CRITICAL_REGION();

    /* refresh internal clock variables */
    SystemCoreClockUpdate();

    return ADI_SYS_SUCCESS;
}

/*!
 * @brief       Enable/disable clock sources.
 *
 * @param[in]   id          Clock source identifier.
 *              bEnable     Enable (true) or disable (false) the clock source.
 *
 * @return      Status
 *                  - #ADI_SYS_SUCCESS if the clock source powers up successfully.
 *
 *  @details    Enables or disables clock sources without additional checks, by writing a "1" or "0" to the enable bit.
 *              In case of the external GPIO, it configures P0.11 as input, with no pullup.
 *
 */
ADI_SYS_RESULT_TYPE SystemEnableClockSource (ADI_SYS_CLOCK_SOURCE_ID id, bool_t bEnable) {
    uint16_t                val;
    volatile uint16_t       *pReg;
    uint32_t                timeout;

    switch(id) {

        case ADI_SYS_CLOCK_SOURCE_HFXTAL:
            val = (1 << BITP_PWR_OSCCTRL_HFXTALEN);
            pReg = &pADI_PWR->OSCCTRL;
            pADI_PWR->OSCKEY = OSCKEY_UNLOCK;
            break;

        case ADI_SYS_CLOCK_SOURCE_LFXTAL:
            val = (1 << BITP_PWR_OSCCTRL_LFXTALEN);
            pReg = &pADI_PWR->OSCCTRL;
            pADI_PWR->OSCKEY = OSCKEY_UNLOCK;
            break;

        case ADI_SYS_CLOCK_SOURCE_HFOSC:
            val = (1 << BITP_PWR_OSCCTRL_HFOSCEN);
            pReg = &pADI_PWR->OSCCTRL;
            pADI_PWR->OSCKEY = OSCKEY_UNLOCK;
            break;

        case ADI_SYS_CLOCK_SOURCE_LFOSC:
           val = (1 << BITP_PWR_OSCCTRL_LFOSCEN);
            pReg = &pADI_PWR->OSCCTRL;
            pADI_PWR->OSCKEY = OSCKEY_UNLOCK;
            break;

        case ADI_SYS_CLOCK_SOURCE_SPLL:
            val = (1 << BITP_SYSCLK_CLKCON3_SPLLEN);
            pReg = &pADI_SYSCLK->CLKCON3;
            break;

        case ADI_SYS_CLOCK_SOURCE_UPLL:
            val = (1 << BITP_SYSCLK_CLKCON4_UPLLEN);
            pReg = &pADI_SYSCLK->CLKCON4;
            break;

        case ADI_SYS_CLOCK_SOURCE_GPIO:
            /* External GPIO is special case */
            /* Disable pullup on P0.11 */
            /* GPIO driver needs to be initialized */
            if (ADI_GPIO_SUCCESS != adi_GPIO_SetPullUpEnable (ADI_GPIO_PORT_0, ADI_GPIO_PIN_11, false)) {
                return ADI_SYS_ERR_PORT_NOT_VALID;
            }

            /* Configure P0.11 as input */
            if (ADI_GPIO_SUCCESS != adi_GPIO_SetInputEnable (ADI_GPIO_PORT_0, ADI_GPIO_PIN_11, true)) {
                return ADI_SYS_ERR_PORT_NOT_VALID;
            }
            return ADI_SYS_SUCCESS;

    } /* end switch */

    /* critical region */
    ADI_ENTER_CRITICAL_REGION();

        if (bEnable) {
            *pReg |= val;
        } else {
            *pReg &= ~val;
        }
    /* end critical region */
    ADI_EXIT_CRITICAL_REGION();

#if (LFXTALEN_WORKAROUND == 1)

    /* LFXTALEN needs to be written twice when disabling LFXTAL */
    /* The time between the 2 writes needs to be at least 2 periods of the 32kHz clock.  */
    if ((id == ADI_SYS_CLOCK_SOURCE_LFXTAL) && (!bEnable)) {

        /* Rough scaling of the wait time between the 2 writes. */
        timeout = scaleTimeout(LFXTALEN_WORKAROUND_WAIT_TIME);

        for (volatile int i = 0; i < timeout; i++) ;

        /* critical region */
        ADI_ENTER_CRITICAL_REGION();

            pADI_PWR->OSCKEY = OSCKEY_UNLOCK;
            *pReg &= ~val;

        /* end critical region */
        ADI_EXIT_CRITICAL_REGION();
    }

#endif

    return ADI_SYS_SUCCESS;

}


/*!
 * @brief       Power up clock source.
 *
 * @param[in]   id       Clock source identifier.
 * @param[in]   timeout
 *                  -
 *
 * @return      Status
 *                  - #ADI_SYS_SUCCESS if the clock source powers up successfully.
 *                  - #ADI_SYS_ERR_CLOCK_TIMEOUT [D] if a timeout error is registered.
 *
 *  @details    Enable clock source and wait until the output is stable. Optionally the wait can be bypassed.
 *              The recommended timeout option is ADI_SYS_CLOCK_TIMEOUT_USE_DEFAULT, the other options are for development purpose.
 *
 */
ADI_SYS_RESULT_TYPE SystemClockSourcePowerUp (ADI_SYS_CLOCK_SOURCE_ID id, ADI_SYS_CLOCK_TIMEOUT_TYPE timeout) {
    ADI_SYS_RESULT_TYPE     result = ADI_SYS_SUCCESS;
    uint32_t                timeoutVal;
    uint16_t                nsel;
    uint16_t                regVal;

    /* If it's already up and stable, do nothing */
    if (GetClockSourceStatus(id) != ADI_SYS_CLOCK_SOURCE_ENABLED_STABLE) {

#if (UPLL_LOCK_WORKAROUND == 1)

        if (id == ADI_SYS_CLOCK_SOURCE_UPLL) {
          
            /* The following is split into 2 critical regions, because of the long timeout. */
          
            /* critical region */
            ADI_ENTER_CRITICAL_REGION();

            /* Save the programmed UPLL NSEL setting.                                      */
            /* CLKCON4 should always have UPLLMSEL=0x2 and UPLLNSEL=0x0F for 60MHz output. */
            /* Writing it as a general function.                                           */
            regVal = pADI_SYSCLK->CLKCON4;

            nsel = (regVal & BITM_SYSCLK_CLKCON4_UPLLNSEL) >> BITP_SYSCLK_CLKCON4_UPLLNSEL;
            
            /* Overwrite the UPLL settings using UPLLNSEL=0x3F to precharge the loop filter, */
            /* and enable UPLL.                                                               */
            regVal &= ~BITM_SYSCLK_CLKCON4_UPLLNSEL;
            regVal |= (0x3F << BITP_SYSCLK_CLKCON4_UPLLNSEL);
            regVal |= (1 << BITP_SYSCLK_CLKCON4_UPLLEN);
            pADI_SYSCLK->CLKCON4 = regVal;

            /* end critical region */
            ADI_EXIT_CRITICAL_REGION();

            /* Approximate scaling of the timeout value with HCLK frequency */
            timeoutVal = scaleTimeout(UPLL_LOCK_WORKAROUND_PRECHARGE_TIME);

            /* Wait while the loop filter is precharging */
            for (; timeoutVal; timeoutVal--)
                ;

            /* Now restore the original UPLL NSEL setting. */
            
            /* critical region */
            ADI_ENTER_CRITICAL_REGION();
            
            /* regVal should not have changed from the previous value    */
            /* However it's possible (and probably wrong) to be modified */
            /* for example in an interrupt.                              */
            regVal = pADI_SYSCLK->CLKCON4;
            regVal &= ~BITM_SYSCLK_CLKCON4_UPLLNSEL;
            regVal |= (nsel << BITP_SYSCLK_CLKCON4_UPLLNSEL);
            pADI_SYSCLK->CLKCON4 = regVal;

            /* end critical region */
            ADI_EXIT_CRITICAL_REGION();            
        }
        else {
            SystemEnableClockSource(id, true);
        }
#else
        SystemEnableClockSource(id, true);
#endif

        if (timeout != ADI_SYS_CLOCK_TIMEOUT_DONT_WAIT_STABLE) {
            /* Timeout required */
            if (timeout == ADI_SYS_CLOCK_TIMEOUT_USE_DEFAULT) {
                /* Use the timeout defaults */
                timeoutVal = CLOCK_POWERUP_TIMEOUT;
            } else {
                /* Timeout value explicitely passed on by the user */
                timeoutVal = timeout;
            }

            /* Approximate scaling of the timeout value with HCLK frequency */
            timeoutVal = scaleTimeout(timeoutVal);

            for ( ; timeoutVal && (GetClockSourceStatus(id) != ADI_SYS_CLOCK_SOURCE_ENABLED_STABLE); timeoutVal--)
                ;

            if (!timeoutVal) {
                result = ADI_SYS_ERR_CLOCK_SOURCE_TIMEOUT;
            }
        }
    }

    return result;
}


/*!
 * @brief       Power down clock source.
 *
 * @param[in]   id       Clock source identifier.
 *
 * @return      Status
 *                  - #ADI_SYS_SUCCESS if the clock source powers up successfully.
 *                  - #ADI_SYS_ERR_CLOCK_SOURCE_USED_ROOT_CLK: if the clock source is used as root clock when trying to power it down.
 *                  - #ADI_SYS_ERR_CLOCK_SOURCE_USED_PLL_CLK: if the clock source is used as PLL input clock when trying to power it down.
 *                  - #ADI_SYS_ERR_CLOCK_SOURCE_USED_GPT_CLK: if the clock source is used as GPT clock when trying to power it down, and the GPT is enabled.
 *
 *  @details    Disable clock source, if the clock source is not used. For each clock source, all possibilities of it being used are checked.
 *
 */

ADI_SYS_RESULT_TYPE SystemClockSourcePowerDownIfUnused (ADI_SYS_CLOCK_SOURCE_ID id) {
    ADI_SYS_RESULT_TYPE result = ADI_SYS_SUCCESS;
    uint32_t            val;

    switch (id) {

        case ADI_SYS_CLOCK_SOURCE_HFXTAL:
            /* Check if HFXTAL is used to drive root_clk */
            if (ADI_SYS_CLOCK_MUX_ROOT_HFXTAL == GetSystemClockMux(ADI_SYS_CLOCK_MUX_ROOT)) {
                return ADI_SYS_ERR_CLOCK_SOURCE_USED_ROOT_CLK;
            }

            /* Check if HFXTAL is used as clock input for SPLL when SPLL is used to drive root_clk */
            if ((ADI_SYS_CLOCK_MUX_SPLL_HF_XTAL == GetSystemClockMux(ADI_SYS_CLOCK_MUX_SPLL)) &&
                (ADI_SYS_CLOCK_MUX_ROOT_SPLL == GetSystemClockMux(ADI_SYS_CLOCK_MUX_ROOT))) {
                return ADI_SYS_ERR_CLOCK_SOURCE_USED_PLL_CLK;
            }
            break;

        case ADI_SYS_CLOCK_SOURCE_HFOSC:
            /* Check if HFOSC is used to drive root_clk */
            if (ADI_SYS_CLOCK_MUX_ROOT_HFOSC == GetSystemClockMux(ADI_SYS_CLOCK_MUX_ROOT)) {
                return ADI_SYS_ERR_CLOCK_SOURCE_USED_ROOT_CLK;
            }

            /* Check if HFOSC is used as clock input for SPLL when SPLL is used to drive root_clk */
            if ((ADI_SYS_CLOCK_MUX_SPLL_HF_OSC == GetSystemClockMux(ADI_SYS_CLOCK_MUX_SPLL)) &&
                (ADI_SYS_CLOCK_MUX_ROOT_SPLL == GetSystemClockMux(ADI_SYS_CLOCK_MUX_ROOT))) {
                return ADI_SYS_ERR_CLOCK_SOURCE_USED_PLL_CLK;
            }

            /* Check if HFOSC is used as clock input for GPT0 and GPT0 is enabled */
            if (((pADI_GPT0->GPTCON & BITM_GPT_GPTCON_CLK) == ADI_GPT_CLOCK_SELECT_HFOSC) &&
                (pADI_GPT0->GPTCON & BITM_GPT_GPTCON_ENABLE)) {
                return ADI_SYS_ERR_CLOCK_SOURCE_USED_GPT_CLK;
            }

            /* Check if HFOSC is used as clock input for GPT1 and GPT1 is enabled */
            if (((pADI_GPT1->GPTCON & BITM_GPT_GPTCON_CLK) == ADI_GPT_CLOCK_SELECT_HFOSC) &&
                (pADI_GPT1->GPTCON & BITM_GPT_GPTCON_ENABLE)) {
                return ADI_SYS_ERR_CLOCK_SOURCE_USED_GPT_CLK;
            }

            /* Check if HFOSC is used as clock input for GPT2 and GPT2 is enabled */
            if (((pADI_GPT2->GPTCON & BITM_GPT_GPTCON_CLK) == ADI_GPT_CLOCK_SELECT_HFOSC) &&
                (pADI_GPT2->GPTCON & BITM_GPT_GPTCON_ENABLE)) {
                return ADI_SYS_ERR_CLOCK_SOURCE_USED_GPT_CLK;
            }

            break;

        case ADI_SYS_CLOCK_SOURCE_SPLL:
            /* Check if SPLL is used to drive root_clk */
            if (ADI_SYS_CLOCK_MUX_ROOT_SPLL == GetSystemClockMux(ADI_SYS_CLOCK_MUX_ROOT)) {
                return ADI_SYS_ERR_CLOCK_SOURCE_USED_ROOT_CLK;
            }
            break;

        case ADI_SYS_CLOCK_SOURCE_UPLL:
            /* Possibly checking if USBPHY is in use? Leaving it blank for now */
            break;

        case ADI_SYS_CLOCK_SOURCE_LFXTAL:
            /* Check if LFXTAL is used to drive lf_clk and at least 1 peripheral is using lf_clk */
            if ((ADI_SYS_CLOCK_MUX_LFCLK_LFXTAL == GetSystemClockMux(ADI_SYS_CLOCK_MUX_LFCLK)) && isLfClkUsed()) {
                return ADI_SYS_ERR_CLOCK_SOURCE_USED_LF_CLK;
            }

            /* Check if the RTC is enabled (it needs LFXTAL) */
            if (pADI_RTC->RTCCR & BITM_RTC_RTCCR_CNTEN) {
                return ADI_SYS_ERR_CLOCK_SOURCE_USED_RTC_CLK;
            }

            /* Check if LFXTAL is used by the WUT and WUT is enabled */
            val = pADI_WUT->T2CON;
            if (((val & BITM_WUT_T2CON_ENABLE) == (1 << BITP_WUT_T2CON_ENABLE)) &&
                ((val & BITM_WUT_T2CON_CLK) == (ADI_WUT_CLK_LFXTAL << BITP_WUT_T2CON_CLK))) {
                return ADI_SYS_ERR_CLOCK_SOURCE_USED_WUT_CLK;
            }

            /* Check if LFXTAL is used as clock input for GPT0 and GPT0 is enabled */
            if (((pADI_GPT0->GPTCON & BITM_GPT_GPTCON_CLK) == ADI_GPT_CLOCK_SELECT_EXTERNAL_CLOCK) &&
                (pADI_GPT0->GPTCON & BITM_GPT_GPTCON_ENABLE)) {
                return ADI_SYS_ERR_CLOCK_SOURCE_USED_GPT_CLK;
            }

            /* Check if LFXTAL is used as clock input for GPT1 and GPT1 is enabled */
            if (((pADI_GPT1->GPTCON & BITM_GPT_GPTCON_CLK) == ADI_GPT_CLOCK_SELECT_EXTERNAL_CLOCK) &&
                (pADI_GPT1->GPTCON & BITM_GPT_GPTCON_ENABLE)) {
                return ADI_SYS_ERR_CLOCK_SOURCE_USED_GPT_CLK;
            }

            /* Check if LFXTAL is used as clock input for GPT2 and GPT2 is enabled */
            if (((pADI_GPT2->GPTCON & BITM_GPT_GPTCON_CLK) == ADI_GPT_CLOCK_SELECT_EXTERNAL_CLOCK) &&
                (pADI_GPT2->GPTCON & BITM_GPT_GPTCON_ENABLE)) {
                return ADI_SYS_ERR_CLOCK_SOURCE_USED_GPT_CLK;
            }

            break;

        case ADI_SYS_CLOCK_SOURCE_LFOSC:
            /* Check if LFOSC is used to drive lf_clk and at least 1 peripheral is using lf_clk */
            if ((ADI_SYS_CLOCK_MUX_LFCLK_LFOSC == GetSystemClockMux(ADI_SYS_CLOCK_MUX_LFCLK)) && isLfClkUsed()) {
                return ADI_SYS_ERR_CLOCK_SOURCE_USED_LF_CLK;
            }

            /* Check if LFOSC is used by the WUT and WUT is enabled */
            val = pADI_WUT->T2CON;
            if (((val & BITM_WUT_T2CON_ENABLE) == (1 << BITP_WUT_T2CON_ENABLE)) &&
                ((val & BITM_WUT_T2CON_CLK) == (ADI_WUT_CLK_LFOSC << BITP_WUT_T2CON_CLK))) {
                return ADI_SYS_ERR_CLOCK_SOURCE_USED_WUT_CLK;
            }

            /* Check if LFOSC is used as clock input for GPT0 and GPT0 is enabled */
            if (((pADI_GPT0->GPTCON & BITM_GPT_GPTCON_CLK) == ADI_GPT_CLOCK_SELECT_32KHZ_INTERNAL_CLOCK) &&
                (pADI_GPT0->GPTCON & BITM_GPT_GPTCON_ENABLE)) {
                return ADI_SYS_ERR_CLOCK_SOURCE_USED_GPT_CLK;
            }

            /* Check if LFOSC is used as clock input for GPT1 and GPT1 is enabled */
            if (((pADI_GPT1->GPTCON & BITM_GPT_GPTCON_CLK) == ADI_GPT_CLOCK_SELECT_32KHZ_INTERNAL_CLOCK) &&
                (pADI_GPT1->GPTCON & BITM_GPT_GPTCON_ENABLE)) {
                return ADI_SYS_ERR_CLOCK_SOURCE_USED_GPT_CLK;
            }

            /* Check if LFOSC is used as clock input for GPT2 and GPT2 is enabled */
            if (((pADI_GPT2->GPTCON & BITM_GPT_GPTCON_CLK) == ADI_GPT_CLOCK_SELECT_32KHZ_INTERNAL_CLOCK) &&
                (pADI_GPT2->GPTCON & BITM_GPT_GPTCON_ENABLE)) {
                return ADI_SYS_ERR_CLOCK_SOURCE_USED_GPT_CLK;
            }

            break;

        case ADI_SYS_CLOCK_SOURCE_GPIO:
            /* Check if external clock is used to drive root_clk */
            if (ADI_SYS_CLOCK_MUX_ROOT_GPIO == GetSystemClockMux(ADI_SYS_CLOCK_MUX_ROOT)) {
                return ADI_SYS_ERR_CLOCK_SOURCE_USED_ROOT_CLK;
            }

#ifndef ADI_ADUCM350
            /* ADuCM350 no longer supports external clock sourcing for WUT */

            /* Check if external clock is used by the WUT and WUT is enabled */
            val = pADI_WUT->T2CON;
            if (((val & BITM_WUT_T2CON_ENABLE) == (1 << BITP_WUT_T2CON_ENABLE)) &&
                ((val & BITM_WUT_T2CON_CLK) == (ADI_WUT_CLK_EXTCLK << BITP_WUT_T2CON_CLK))) {
                return ADI_SYS_ERR_CLOCK_SOURCE_USED_WUT_CLK;
            }
#endif
            break;

        default:
            return ADI_SYS_ERR_INVALID_CLOCK_ID;

    } /* end switch */

    SystemEnableClockSource(id, false);

    return result;

}

/*!
 * @brief       Return the status of a clock source.
 *
 * @param[in]   id       Clock source identifier.
 *
 * @return      Status
 *              - #ADI_SYS_CLOCK_SOURCE_DISABLED if the clock source is disabled.
 *              - #ADI_SYS_CLOCK_SOURCE_ENABLED_NOT_STABLE if the clock source is enabled but not stable.
 *              - #ADI_SYS_CLOCK_SOURCE_ENABLED_STABLE if the clock source is enabled and stable.
 *              - #ADI_SYS_CLOCK_SOURCE_ID_NOT_VALID if the clock source identifier is not valid.
 *
 *  @details    Return the status of a clock source.
 *
 */
ADI_SYS_CLOCK_SOURCE_STATUS GetClockSourceStatus (ADI_SYS_CLOCK_SOURCE_ID id) {
    ADI_SYS_CLOCK_SOURCE_STATUS     status = ADI_SYS_CLOCK_SOURCE_DISABLED;
    uint16_t                        val;

    val = pADI_PWR->OSCCTRL;

    switch(id) {

        case ADI_SYS_CLOCK_SOURCE_HFOSC:
            if (val & BITM_PWR_OSCCTRL_HFOSCEN) {
                /* Clock source enabled, now check for stable */
                if (val & BITM_PWR_OSCCTRL_HFOSCOK) {
                    status = ADI_SYS_CLOCK_SOURCE_ENABLED_STABLE;
                } else {
                    status = ADI_SYS_CLOCK_SOURCE_ENABLED_NOT_STABLE;
                }
            }
            break;

        case ADI_SYS_CLOCK_SOURCE_LFOSC:
            if (val & BITM_PWR_OSCCTRL_LFOSCEN) {
                /* Clock source enabled, now check for stable */
                if (val & BITM_PWR_OSCCTRL_LFOSCOK) {
                    status = ADI_SYS_CLOCK_SOURCE_ENABLED_STABLE;
                } else {
                    status = ADI_SYS_CLOCK_SOURCE_ENABLED_NOT_STABLE;
                }
            }
            break;

        case ADI_SYS_CLOCK_SOURCE_HFXTAL:
            if (val & BITM_PWR_OSCCTRL_HFXTALEN) {
                /* Clock source enabled, now check for stable */
                if (val & BITM_PWR_OSCCTRL_HFXTALOK) {
                    status = ADI_SYS_CLOCK_SOURCE_ENABLED_STABLE;
                } else {
                    status = ADI_SYS_CLOCK_SOURCE_ENABLED_NOT_STABLE;
                }
            }
            break;

        case ADI_SYS_CLOCK_SOURCE_LFXTAL:
            if (val & BITM_PWR_OSCCTRL_LFXTALEN) {
                /* Clock source enabled, now check for stable */
                if (val & BITM_PWR_OSCCTRL_LFXTALOK) {
                    status = ADI_SYS_CLOCK_SOURCE_ENABLED_STABLE;
                } else {
                    status = ADI_SYS_CLOCK_SOURCE_ENABLED_NOT_STABLE;
                }
            }
            break;

        case ADI_SYS_CLOCK_SOURCE_GPIO:
            /* Check that the GPIO is configured correctly, there is no other status */
            /* Since there is no measure of stability, ADI_SYS_CLOCK_SOURCE_ENABLED_NOT_STABLE can never be returned. */
            if ((!(adi_GPIO_GetOutputEnable(ADI_GPIO_PORT_0) & ADI_GPIO_PIN_11)) &&
                (!(adi_GPIO_GetPullUpEnable(ADI_GPIO_PORT_0) & ADI_GPIO_PIN_11))) {
                status = ADI_SYS_CLOCK_SOURCE_ENABLED_STABLE;
            }
            break;

        case ADI_SYS_CLOCK_SOURCE_SPLL:
            if (pADI_SYSCLK->CLKCON3 & BITM_SYSCLK_CLKCON3_SPLLEN) {
                /* Clock source enabled, now check for stable */
                if (pADI_SYSCLK->CLKSTAT0 & BITM_SYSCLK_CLKSTAT0_SPLLSTATUS) {
                    status = ADI_SYS_CLOCK_SOURCE_ENABLED_STABLE;
                } else {
                    status = ADI_SYS_CLOCK_SOURCE_ENABLED_NOT_STABLE;
                }
            }
            break;

        case ADI_SYS_CLOCK_SOURCE_UPLL:
            if (pADI_SYSCLK->CLKCON4 & BITM_SYSCLK_CLKCON4_UPLLEN) {
                /* Clock source enabled, now check for stable */
                if (pADI_SYSCLK->CLKSTAT0 & BITM_SYSCLK_CLKSTAT0_UPLLSTATUS) {
                    status = ADI_SYS_CLOCK_SOURCE_ENABLED_STABLE;
                } else {
                    status = ADI_SYS_CLOCK_SOURCE_ENABLED_NOT_STABLE;
                }
            }
            break;

        default:
            status =  ADI_SYS_CLOCK_SOURCE_ID_NOT_VALID;

    } /* end switch */

    return status;
}

/*!
 * @brief       Program PLL frequency.
 *
 * @param[in]   id       Clock source identifier, can be either #ADI_SYS_CLOCK_SOURCE_SPLL or #ADI_SYS_CLOCK_SOURCE_UPLL.
 *              msel     PLL M divider.
 *              nsel     PLL N multiplier.
 *              div2     PLL DIV2 parameter.
 *
 * @return      Status
 *              - #ADI_SYS_SUCCESS if the PLL has been programmed successfully.
 *              - #ADI_SYS_ERR_SPLL_CHANGE_NOT_ALLOWED if trying to program SPLL and SPLL drives the system clock.
 *              - #ADI_SYS_ERR_INVALID_CLOCK_ID if the clock identifier does not match either PLL.
 *
 *  @details    Program PLL frequency (parameters M, N, DIV2) for either the System PLL or USB PLL.
 *
 */
ADI_SYS_RESULT_TYPE SetPllFreq(ADI_SYS_CLOCK_SOURCE_ID id, ADI_SYS_CLOCK_PLL_MSEL_TYPE msel, ADI_SYS_CLOCK_PLL_NSEL_TYPE nsel, ADI_SYS_CLOCK_PLL_DIV2_TYPE div2) {
    volatile uint16_t       *pReg;
    uint32_t                val, cfg;
    uint32_t                mask;

    switch (id) {

        case ADI_SYS_CLOCK_SOURCE_SPLL:
            // Check if the PLL is multipexed in as clock source, parameters should not change in that case
            if (ADI_SYS_CLOCK_MUX_ROOT_SPLL == GetSystemClockMux(ADI_SYS_CLOCK_MUX_ROOT)) {
                return ADI_SYS_ERR_SPLL_CHANGE_NOT_ALLOWED;
            }

            pReg = &pADI_SYSCLK->CLKCON3;
            mask = BITM_SYSCLK_CLKCON3_SPLLDIV2 |
                   BITM_SYSCLK_CLKCON3_SPLLMSEL |
                   BITM_SYSCLK_CLKCON3_SPLLNSEL;
            cfg = ((uint32_t)div2 | (uint32_t)msel | (uint32_t)nsel);
            break;

        case ADI_SYS_CLOCK_SOURCE_UPLL:
            pReg = &pADI_SYSCLK->CLKCON4;
            mask = BITM_SYSCLK_CLKCON4_UPLLDIV2 |
                   BITM_SYSCLK_CLKCON4_UPLLMSEL |
                   BITM_SYSCLK_CLKCON4_UPLLNSEL;
            cfg = ((uint32_t)div2 | (uint32_t)msel | (uint32_t)nsel);
            break;

        default:
            return ADI_SYS_ERR_INVALID_CLOCK_ID;

    } /* end switch */

    /* critical region */
    ADI_ENTER_CRITICAL_REGION();

        val = *pReg;
        val &= ~mask;
        val |= cfg;
        *pReg = val;

    /* end critical region */
    ADI_EXIT_CRITICAL_REGION();

    return ADI_SYS_SUCCESS;

}


/*!
    @brief      Puts the processor into given low power mode.

    @param[in]  PowerMode          One of the ADI_PWR_MODE_TYPE enum values, defining the specific
                                   low-power modes to use.

    @param[in,out] pbInterruptOccurred
                                   Control parameter selection low-power operation.  Either a NULL pointer
                                   for automatic hardware-based sleeping between interrupts, or a pointer
                                   to bool_t for software looping sleep between interrupts.

                                   If a flag pointer is passed in, the flag must be \b false on entry,
                                   and will be cleared \b false on exit.

                                   When a NULL is passed, it means the application wants the low-power
                                   implementation to use the automatic "sleep-on-exit" hardware sleep
                                   mode in which wakeup interrupts are dispatched and then automaticelly
                                   put the processor back to sleep on exit.  All interrupts execute the
                                   same WFI instruction (no looping) under hardware control, which results
                                   in a faster re-sleep than the software mode.

                                   When a non-NULL value is passed, it is interpreted as a pointer to a
                                   shared bool_t application control veriable allowing the wake-up
                                   interrupts to control whether/when the control loop should re-sleep the
                                   processor as each interrupt exits.  Any interrupt that sets the variable
                                   will cause the sleep loop to exit.  Otherwise, exiting interrupts will
                                   cause the core to re-sleep until the flag is set.  Each interrupt executes
                                   a different WFI instruction inside a software loop (slower re-sleep).

    @param[in]  PriorityMask       A right-justified (unshifted) wakeup interrupt priority mask, corresponding
                                   to the programmable interrupt priority encoding scheme defined by the Cortex
                                   NVIC controller.  The \a PriorityMask value blocks interrupts with an equal
                                   or lower priority than the specified level, such that only higher-priority
                                   interrupts (less in numerical value) than the priority mask awake the
                                   processor.  A zero-valued \a PriorityMask disables interrupt masking.

    @return     Status
                    - #ADI_SYS_SUCCESS                    If successfully put the processor into low power mode.
                    - #ADI_SYS_ERR_PARAM_OUT_OF_RANGE [D] PriorityMask contains unimplemented hardware bits.
                    - #ADI_SYS_ERR_INVALID_POWER_MODE [D] Invalid low-power mode.



    Puts the processor into a low-power mode with interrupt-based wakeup(s).  Applications specify the low-power
    mode, a pointer to an application-defined interrupt flag, and an interrupt priority mask controlling the
    interrupt priority level that may awake the processor.

    @par pbInterruptOccurred
    When NULL, the processor is automatically put back to sleep as awaking interrupts exit.  This mode employs
    the hardware "sleep-on-exit" system control register bit: SLEEPONEXIT_BIT in conjunction with the "wait-for-
    interrupt" (WFI) instruction to implement a persistant sleep mode.

    When non-Null, a software strategy is used to control sleeping.  As awakening interrupts are processed, they
    can set the flag true and thereby cause the sleep mode to be exited.  Note that all interrupts share a
    common flag and any interrupt that sets the flag will cause the sleep mode to be exited.

    Use of the \a pbInterruptOccurred parameter provides a mechanism to resolve two potential hibernation trouble
    spots: 1) the inherent race between the intended wakeup interrupt and the execution of the Wait-For-Interrupt
    instruction (WFI) used to sleep the processor, and 2) unrelated interrupts (of sufficient priority)
    that may terminate the wait prematurely.

    In the first case of the race condition, the race is avoided by testing the \a pbInterruptOccurred flag prior
    to the WFI within a common critical section.  This allows the #SystemEnterLowPowerMode() implementation
    to insure the intended wakeup interrupt has not occurred already and control whether to sleep the processor.
    This insures the intended wakeup interrupt has not already occurred prior to the wait, thereby eliminating the
    race condition otherwise present.

    In the second case of an unrelated interrupt terminating the sleep prematurely, the problem is solved by
    requiring the interrupt handler(s) which is(are) intended to awake the sleeping processor to set the
    application-defined \a pbInterruptOccurred flag in their respective interrupt handler(s).  This insures only those
    interrupts that explicitly set the flag will break the sleeping processor out of the sleep cycle.  Other
    (incidental) interrupts put the processor back to sleep after the interrupt because the flag would not have been set.
    This is why there is a loop around the WFI instruction.

    The \a pbInterruptOccurred flag must be initialized to zero before first use, and this should be done
    prior to enabling any interrupt which may set it (otherwise interrupts may be missed). If this flag is
    a global or a static variable then static initialization to zero or false will be sufficient.

    The flag should only be set, from an interrupt handler, by calling SystemExitLowPowerMode() and passing
    the flag variable by reference. The flag should not be assigned to directly, other than for initialization.

    #SystemEnterLowPowerMode() will always clear the flag again before returning, so the variable does not
    need to be cleared by user code on each use. Explicitly clearing the flag, outside of #SystemEnterLowPowerMode()
    runs the risk of missing interrupts.

    @par PriorityMask
    A zero-valued \a PriorityMask disables interrupt masking, leaving all interrupts eligible to awake the
    sleeping processor.  This means that zero-valued interrupts cannot be masked.  A non-zero \a PriorityMask
    limits interrupts that may awake the sleeping processor to those with a higher priority level (lower
    numerically) than the specified \a PriorityMask value.

    Each "programmable" peripheral interrupt has an associated priority-level register (which defaults to
    zero) within the Nested Vectored Interrupt Controller (NVIC).  The number of interrupt priority encoding
    bits is defined by constant #__NVIC_PRIO_BITS and is a fixed silicon attribute configured during chip
    design.  The interrupt priority-level registers range in width from 3 to 8 bits.

    The ADuCM350 uses 3-bit priority encoding, allowing priority levels ranging between 0 (the highest,
    default programmable priority) and 7 (the lowest).  For example, if the \a PriorityMask parameter is
    set to 3, only interrupts with assigned priority 0, 1, and 2 may awake the processor.  Since default
    priority of all programmable interrupts is 0, setting up maskable interrupts requires that they be
    demoted in priority (raised numerically) relative to interrupts that are intended to awake the processor.

    @note    The number of priority levels is uncorrelated with the actual number of interrupts or their position
             in the Interrupt Vector Table (IVT).  Interrupt priorities may be programmed individually.

    @note    The priority levels are actually stored in the core as a left-justified value in an 8-bit field.
             The #SystemEnterLowPowerMode() API takes care of aligning the passed \a PriorityMask value to the
             core register (BASEPRI).

    @note    The default priority level for all interrupts is zero implies it is impossible to mask interrupts
             with a default zero-level priority encoding.

    @note    Power modes 4 and 5 only support select interrupt sources to awake the processor.

    @sa      adi_PWR_GetPowerMode
    @sa      SystemExitLowPowerMode
*/
ADI_SYS_RESULT_TYPE SystemEnterLowPowerMode (const ADI_SYS_POWER_MODE PowerMode,
                                             bool_t volatile *        pbInterruptOccurred,
                                             const uint8_t            PriorityMask)
{
    uint32_t savedPriority;
    uint16_t scrSetBits = 0u;
    uint16_t scrClrBits = 0u;

#ifdef ADI_DEBUG

    /* verify the requested priority mask bits are right-justified and don't exceed __NVIC_PRIO_BITS in width */
    if ( PriorityMask & ~((1 << __NVIC_PRIO_BITS) - 1) )
        return ADI_SYS_ERR_PARAM_OUT_OF_RANGE;

#endif  /* ADI_DEBUG */

    /* pre-calculate the sleep-on-exit set/clear bits */
    if(NULL == pbInterruptOccurred) {
        scrSetBits |= SLEEPONEXIT_BIT;

        /* point to private control variable when in hardware mode */
        pbInterruptOccurred = &privateIntOccFlag;
    }

    /* pre-calculate the deepsleep set/clear bits */
    switch (PowerMode) {
        case ADI_SYS_MODE_ACTIVE:
            return ADI_SYS_SUCCESS;

        case ADI_SYS_MODE_CORE_SLEEP:
        case ADI_SYS_MODE_SYS_SLEEP:
            scrClrBits |= SLEEPDEEP_BIT;
            break;

        case ADI_SYS_MODE_HIBERNATE:
            scrSetBits |= SLEEPDEEP_BIT;
            break;

#ifdef ADI_DEBUG
        default:
            return ADI_SYS_ERR_INVALID_POWER_MODE;
#endif  /*ADI_DEBUG*/

    } /* end switch */

    /* put all the power mode and system control mods inside a critical section */
    ADI_ENTER_CRITICAL_REGION();

        { /* these three lines must be in a success-checking loop if they are not inside critical section */
            /* Uninterruptable unlock sequence */
            pADI_PWR->PWRKEY = PWRKEY_VALUE_KEY1;
            pADI_PWR->PWRKEY = PWRKEY_VALUE_KEY2;

            /* Clear the previous mode and set new mode */
            pADI_PWR->PWRMOD = ( ( pADI_PWR->PWRMOD & (~BITM_PWR_PWRMOD_PWRMOD) ) | PowerMode );
        }

        /* Update the SCR (sleepdeep and sleep-on-exit bits) */
        SCB->SCR = ((SCB->SCR | scrSetBits) & ~scrClrBits);

        /* save current Base Priority Level */
        savedPriority = __get_BASEPRI();

        /* Set caller's priority threshold (left-justified) */
        __set_BASEPRI(PriorityMask << (8-__NVIC_PRIO_BITS));

        /* if we are in the software looping mode, loop on the user's flag until set */
        while (!*pbInterruptOccurred) {

            /* SAR-51938: insure WDT is fully synchronized or hibernate may lock out the sync bits */
            while (pADI_WDT->T3STA & ( (1<<BITP_WDT_T3STA_CON) | (1<<BITP_WDT_T3STA_LD) | (1<<BITP_WDT_T3STA_CLRI) ) );

            __DSB();  /* bus sync to insure register writes from interrupt handlers are always complete before WFI */

			/* NOTE: aggressive compiler optimizations can muck up critical timing here, so reduce if hangs are present */

            /* Wait for interrupt */
            __WFI();

            /* Recycle critical section so that interrupts are dispatched.  This
             * allows *pbInterruptOccurred to be set during interrupt handling.
             */
            ADI_EXIT_CRITICAL_REGION();
            	// nop
            ADI_ENTER_CRITICAL_REGION();

        }  /* end while */

        /* ...still within critical section... */

        *pbInterruptOccurred = false; /* reset the completion flag on exit */

        /* Restore previous base priority */
        __set_BASEPRI(savedPriority);

        /* clear sleep-on-exit bit to avoid sleeping on exception return to thread level */
        SCB->SCR &= ~SLEEPONEXIT_BIT;

        __DSB(); /* bus sync before re-enabeling interrupts */

    ADI_EXIT_CRITICAL_REGION();

    return ADI_SYS_SUCCESS;
}


/*
 * Companion function to #SystemEnterLowPowerMode() that allows interrupts to
 * break out of the "sleep-on-exit" mode in which the processor stays in
 * sleep mode except to handle suitably prioritized and enabled interrupts.
 *
 * @sa SystemEnterLowPowerMode
 */
void SystemExitLowPowerMode(bool_t volatile * pbInterruptOccurred) {

    /* Manage the exit depending on pbInterruptOccurred flag convention... */
    /* NULL pointer means we are using the hardware sleep-on-exit feature */
    /* non-NULL pointer means we are using a software looping flag top sleep */

    if (NULL == pbInterruptOccurred) {

        pbInterruptOccurred = &privateIntOccFlag;  /* point to private control variable in hardware mode */

        /* clear hardware sleep-on-exit feature */
        ADI_ENTER_CRITICAL_REGION();

            SCB->SCR &= ~SLEEPONEXIT_BIT;
            __DSB();  /* bus sync before interrupt exit */

        ADI_EXIT_CRITICAL_REGION();
    }

    /* set control variable (whether hardware or software based) so WFI exits in SystemEnterLowPowerMode() */
    *pbInterruptOccurred = true;
}


/*
 * Returns true if any peripheral that uses lf_clk is enabled, false otherwise.
 */
static bool_t isLfClkUsed(void) {


    if ((pADI_CT->CT_CFG2 & BITM_CT_CT_CFG2_STIMER_EN) &&
        (pADI_CT->CT_CDC_PWR & BITM_CT_CT_CDC_PWR_PWR_MODE)) {
        /* lf_clk used for CapTouch self-timer */
        return true;
    }

    if (pADI_BEEP->BEEP_CFG & BITM_BEEP_BEEP_CFG_BEEP_EN) {
        /* Beeper is enabled, needs lf_clk */
        return true;
    }

    if (pADI_WDT->T3CON & BITM_WDT_T3CON_ENABLE) {
        /* lf_clk used for WDT */
        return true;
    }

    if (pADI_LCD->LCDCON & BITM_LCD_LCDCON_LCDEN) {
        /* lf_clk used for LCD */
        return true;
    }

    return false;
}

/*
 * Rough scaling of timeouts. The input has to be referenced to an HCLK frequency of 1MHz.
 * The result is not an exact number, this is OK in the context we're using the timeout.
 *
 */
static uint32_t scaleTimeout(uint32_t timeout) {
    uint32_t result;
    uint32_t scale;

    /* Rough scaling, dividing by 2^20 instead of 10^6. */
    scale = SystemGetClockFrequency (ADI_SYS_CLOCK_CORE);
    scale = scale >> 20;

    result = (scale > 0) ? timeout * scale: timeout;

    return result;
}

#ifdef ADI_SYSTEM_CLOCK_TRANSITION

/*********************************************************************/
/*   State transition table and API   */
/*********************************************************************/

// Static State Transition Table that generates the state machine flow graph.
// Modify behavior here.  A state can have multiple events and/or transitions,
// but each state requires a seperate row each for each event.
typedef struct {
    ADI_SYS_CLOCK_STATE_TYPE        fromState;  // entry state
    ADI_SYS_CLOCK_TRIGGER_TYPE      event;      // transition event
    ADI_SYS_CLOCK_STATE_TYPE        toState;    // next state
    ClockAction                     action;     // transition action
} StateTableEntryType;

/* force table placement into flash using the "const" qualifer */
static const StateTableEntryType StateTransitionTable[] = {
    
/*  from-state                              input-event                             to-state                                action                  */
    {ADI_SYS_CLOCK_STATE_MINIMAL,           ADI_SYS_CLOCK_TRIGGER_MEASUREMENT_ON,   ADI_SYS_CLOCK_STATE_MEASUREMENT,        aEnterMeasurementState,      },
    {ADI_SYS_CLOCK_STATE_MINIMAL,           ADI_SYS_CLOCK_TRIGGER_MEASUREMENT_OFF,  ADI_SYS_CLOCK_STATE_INVALID,            ADI_SYS_CLOCK_STATE_INVALID, },
    {ADI_SYS_CLOCK_STATE_MINIMAL,           ADI_SYS_CLOCK_TRIGGER_USB_ON,           ADI_SYS_CLOCK_STATE_USB,                aEnterUSBState,              },
    {ADI_SYS_CLOCK_STATE_MINIMAL,           ADI_SYS_CLOCK_TRIGGER_USB_OFF,          ADI_SYS_CLOCK_STATE_INVALID,            ADI_SYS_CLOCK_STATE_INVALID, },
    {ADI_SYS_CLOCK_STATE_MINIMAL,           ADI_SYS_CLOCK_TRIGGER_USB_LPM,          ADI_SYS_CLOCK_STATE_INVALID,            ADI_SYS_CLOCK_STATE_INVALID, },

    {ADI_SYS_CLOCK_STATE_MEASUREMENT,       ADI_SYS_CLOCK_TRIGGER_MEASUREMENT_ON,   ADI_SYS_CLOCK_STATE_INVALID,            ADI_SYS_CLOCK_STATE_INVALID, },
    {ADI_SYS_CLOCK_STATE_MEASUREMENT,       ADI_SYS_CLOCK_TRIGGER_MEASUREMENT_OFF,  ADI_SYS_CLOCK_STATE_MINIMAL,            aEnterMinimalState,          },
    {ADI_SYS_CLOCK_STATE_MEASUREMENT,       ADI_SYS_CLOCK_TRIGGER_USB_ON,           ADI_SYS_CLOCK_STATE_USB_MEASUREMENT,    aEnterUSBState,              },
    {ADI_SYS_CLOCK_STATE_MEASUREMENT,       ADI_SYS_CLOCK_TRIGGER_USB_OFF,          ADI_SYS_CLOCK_STATE_INVALID,            ADI_SYS_CLOCK_STATE_INVALID, },
    {ADI_SYS_CLOCK_STATE_MEASUREMENT,       ADI_SYS_CLOCK_TRIGGER_USB_LPM,          ADI_SYS_CLOCK_STATE_INVALID,            ADI_SYS_CLOCK_STATE_INVALID, },

    {ADI_SYS_CLOCK_STATE_USB,               ADI_SYS_CLOCK_TRIGGER_MEASUREMENT_ON,   ADI_SYS_CLOCK_STATE_USB_MEASUREMENT,    ADI_SYS_CLOCK_STATE_INVALID, },
    {ADI_SYS_CLOCK_STATE_USB,               ADI_SYS_CLOCK_TRIGGER_MEASUREMENT_OFF,  ADI_SYS_CLOCK_STATE_INVALID,            ADI_SYS_CLOCK_STATE_INVALID, },
    {ADI_SYS_CLOCK_STATE_USB,               ADI_SYS_CLOCK_TRIGGER_USB_ON,           ADI_SYS_CLOCK_STATE_INVALID,            ADI_SYS_CLOCK_STATE_INVALID, },
    {ADI_SYS_CLOCK_STATE_USB,               ADI_SYS_CLOCK_TRIGGER_USB_OFF,          ADI_SYS_CLOCK_STATE_MINIMAL,            aEnterMinimalState,          },
    {ADI_SYS_CLOCK_STATE_USB,               ADI_SYS_CLOCK_TRIGGER_USB_LPM,          ADI_SYS_CLOCK_STATE_USB_LPM,            aEnterUSBLPMState,           },

    {ADI_SYS_CLOCK_STATE_USB_MEASUREMENT,   ADI_SYS_CLOCK_TRIGGER_MEASUREMENT_ON,   ADI_SYS_CLOCK_STATE_INVALID,            ADI_SYS_CLOCK_STATE_INVALID, },
    {ADI_SYS_CLOCK_STATE_USB_MEASUREMENT,   ADI_SYS_CLOCK_TRIGGER_MEASUREMENT_OFF,  ADI_SYS_CLOCK_STATE_USB,                ADI_SYS_CLOCK_STATE_INVALID, },
    {ADI_SYS_CLOCK_STATE_USB_MEASUREMENT,   ADI_SYS_CLOCK_TRIGGER_USB_ON,           ADI_SYS_CLOCK_STATE_INVALID,            ADI_SYS_CLOCK_STATE_INVALID, },
    {ADI_SYS_CLOCK_STATE_USB_MEASUREMENT,   ADI_SYS_CLOCK_TRIGGER_USB_OFF,          ADI_SYS_CLOCK_STATE_MEASUREMENT,        aEnterMeasurementState,      },
    {ADI_SYS_CLOCK_STATE_USB_MEASUREMENT,   ADI_SYS_CLOCK_TRIGGER_USB_LPM,          ADI_SYS_CLOCK_STATE_MEASUREMENT,        aEnterMeasurementState,      },

    {ADI_SYS_CLOCK_STATE_USB_LPM,           ADI_SYS_CLOCK_TRIGGER_MEASUREMENT_ON,   ADI_SYS_CLOCK_STATE_MEASUREMENT,        aEnterMeasurementState,      },
    {ADI_SYS_CLOCK_STATE_USB_LPM,           ADI_SYS_CLOCK_TRIGGER_MEASUREMENT_OFF,  ADI_SYS_CLOCK_STATE_INVALID,            ADI_SYS_CLOCK_STATE_INVALID, },
    {ADI_SYS_CLOCK_STATE_USB_LPM,           ADI_SYS_CLOCK_TRIGGER_USB_ON,           ADI_SYS_CLOCK_STATE_USB,                aEnterUSBState,              },
    {ADI_SYS_CLOCK_STATE_USB_LPM,           ADI_SYS_CLOCK_TRIGGER_USB_OFF,          ADI_SYS_CLOCK_STATE_MINIMAL,            aEnterMinimalState,          },
    {ADI_SYS_CLOCK_STATE_USB_LPM,           ADI_SYS_CLOCK_TRIGGER_USB_LPM,          ADI_SYS_CLOCK_STATE_INVALID,            ADI_SYS_CLOCK_STATE_INVALID, },


};


/*!
 * @brief       Transition to the next clock state.
 *
 * @param[in]   transitionEvent     Trigger event for the clock state transition (ADI_SYS_CLOCK_TRIGGER_TYPE).
 *
 * @return      Status
 *              - #ADI_SYS_SUCCESS if clock transition was successful, error status otherwise.
 *
 *  @details    Transition to the next clock state.
 *
 */
ADI_SYS_RESULT_TYPE SystemTransitionClocks (const ADI_SYS_CLOCK_TRIGGER_TYPE transitionEvent) {

    ADI_SYS_RESULT_TYPE result = ADI_SYS_SUCCESS;
    const StateTableEntryType *         pTableEntry = NULL;
    register int index;

    /* insure formal transition to minimal state on startup */
    if (!currentState) {
        if (ADI_SYS_SUCCESS != (result = aEnterMinimalState()))
            return result;
        currentState = ADI_SYS_CLOCK_STATE_MINIMAL;
    }

    /* locate 1st entry matching current state (index by max number of states, ADI_SYS_CLOCK_STATE_USB_LPM to optimize search) */
    for (index = 0; index < (sizeof (StateTransitionTable) / sizeof(StateTransitionTable[0])); index += ADI_SYS_CLOCK_STATE_USB_LPM) {

        if (StateTransitionTable[index].fromState == currentState)
            break;
    }

    /* continue indexing through this state's possible events to match the transition request... */
    for (int start = index; index < (start + ADI_SYS_CLOCK_TRIGGER_NUM); index++) {

        if (StateTransitionTable[index].event == transitionEvent) {

            /* found it... set the table pointer */
            pTableEntry = &StateTransitionTable[index];
            break;
            }
        }

    /* validate table pointer and transition... */
    if (pTableEntry && pTableEntry->toState) {

        /* execute transition action, if present */
        if (pTableEntry->action)
            result = (*pTableEntry->action)();

        /* update current state if the transaction action was successful */
        /* otherwise, just return the transition action's error code   */
        if (ADI_SYS_SUCCESS == result)
            currentState = pTableEntry->toState;

    } else {

        /* validation of table pointer and/or transition failed... */
        result = ADI_SYS_ERR_INVALID_STATE_TRANSITION;
        //printf("undefined transition from state on event: %d, %d/n", currentState, transitionEvent);
    }

    return result;
}

/*!
 * @brief       Configure clock state.
 *
 * @param[in]   state       Clock state to be configured.
 *              pConfig     Pointer to the state configuration structure.
 *
 * @return      Status
 *              - #ADI_SYS_SUCCESS if the state configuration is successful.
 *
 *  @details    Allows reconfiguration of clock states. Currently supported are HCLK and PCLK clock dividers.
 *              This is not required, as the defaults should cover most situations. It is provided to cover
 *              corner cases where the default parameters are not satisfactory.
 *
 */
ADI_SYS_RESULT_TYPE SetClockStateConfig(ADI_SYS_CLOCK_STATE_TYPE state, const ADI_SYS_CLOCK_STATE_CONFIG_TYPE* const pConfig) {
   ADI_SYS_RESULT_TYPE result = ADI_SYS_SUCCESS;
   ADI_SYS_CLOCK_STATE_CONFIG_TYPE*    pClockStateConfig;

   pClockStateConfig = &ClockStateConfig[state];

   pClockStateConfig->HclkDiv         = pConfig->HclkDiv;
   pClockStateConfig->PclkDiv         = pConfig->PclkDiv;

   return result;
}

/*!
 * @brief       Read clock state configuration.
 *
 * @param[in]   state       Clock state to be configured.
 *
 * @param[out]  pConfig     Pointer to the state configuration structure.
 *
 *
 * @return      Status
 *              - #ADI_SYS_SUCCESS if the state configuration is successful.
 *
 *  @details    Allows reading back of clock state configurations.
 *
 */
ADI_SYS_RESULT_TYPE GetClockStateConfig(ADI_SYS_CLOCK_STATE_TYPE state, ADI_SYS_CLOCK_STATE_CONFIG_TYPE* const pConfig) {
   ADI_SYS_RESULT_TYPE result = ADI_SYS_SUCCESS;
   const ADI_SYS_CLOCK_STATE_CONFIG_TYPE*    pClockStateConfig;

   pClockStateConfig = &ClockStateConfig[state];

   pConfig->HclkDiv         = pClockStateConfig->HclkDiv;
   pConfig->PclkDiv         = pClockStateConfig->PclkDiv;

   return result;
}

/*!
 * @brief       Read current clock state.
 *
 * @param[out]  pState     Pointer to the returned state.
 *
 * @return      Status
 *              - #ADI_SYS_SUCCESS if the state reading is successful.
 *
 *  @details    Allows reading back of current clock state.
 *              This may be helpful to keep track of the clock configuration.
 *
 */
ADI_SYS_RESULT_TYPE GetClockState(ADI_SYS_CLOCK_STATE_TYPE* pState) {
    ADI_SYS_RESULT_TYPE result = ADI_SYS_SUCCESS;

    *pState = currentState;

    return result;
}

/********************************************************************/
/*                                                                  */
/*              Actions                                             */
/*                                                                  */
/********************************************************************/

static ADI_SYS_RESULT_TYPE aEnterMinimalState(void) {
    ADI_SYS_RESULT_TYPE result = ADI_SYS_SUCCESS;
    ADI_SYS_CLOCK_STATE_CONFIG_TYPE* pClockStateConfig;

    pClockStateConfig = &ClockStateConfig[ADI_SYS_CLOCK_STATE_MINIMAL];

    /* Powerup HFOSC */
    if (ADI_SYS_SUCCESS != (result = SystemClockSourcePowerUp(ADI_SYS_CLOCK_SOURCE_HFOSC, ADI_SYS_CLOCK_TIMEOUT_USE_DEFAULT))) {
        return result;
    }

    /* Select HFOSC to drive root_clk */
    SetSystemClockMux(ADI_SYS_CLOCK_MUX_ROOT_HFOSC);

    /* Change clock dividers */
    changeClockDividers(pClockStateConfig, false);

    /* Configure USBCTLCLK frequency to be the same as HCLK frequency */
    SetSystemClockDivider(ADI_SYS_CLOCK_USBCTL, 0);

    /* Powerdown unused clock sources */
    SystemClockSourcePowerDownIfUnused(ADI_SYS_CLOCK_SOURCE_UPLL);
    SystemClockSourcePowerDownIfUnused(ADI_SYS_CLOCK_SOURCE_SPLL);
    SystemClockSourcePowerDownIfUnused(ADI_SYS_CLOCK_SOURCE_HFXTAL);

    /* Update system clock variables */
    SystemCoreClockUpdate();

    return result;
}


/* Note: while the function is able to handle HFOSC as PLL input, it is strongly recommended    */
/* to use HFXTAL due to accuracy.                                                               */
ADI_SYS_RESULT_TYPE aEnterMeasurementState(void) {
    ADI_SYS_RESULT_TYPE result = ADI_SYS_SUCCESS;
    ADI_SYS_CLOCK_STATE_CONFIG_TYPE* pClockStateConfig;

    pClockStateConfig = &ClockStateConfig[ADI_SYS_CLOCK_STATE_MEASUREMENT];

#if defined(MEAS_CLOCK_SOURCE_SPLL)

    /* Using SPLL */
    /* First check if HFXTAL or HFOSC is PLL source and powerup the required clock source */
    if (ADI_SYS_CLOCK_MUX_SPLL_HF_XTAL == GetSystemClockMux(ADI_SYS_CLOCK_MUX_SPLL)) {
        /* Power up HFXTAL */
        result = SystemClockSourcePowerUp(ADI_SYS_CLOCK_SOURCE_HFXTAL, ADI_SYS_CLOCK_TIMEOUT_USE_DEFAULT);
    } else {
        /* Power up HFOSC */
        result = SystemClockSourcePowerUp(ADI_SYS_CLOCK_SOURCE_HFOSC, ADI_SYS_CLOCK_TIMEOUT_USE_DEFAULT);
    }

    if (ADI_SYS_SUCCESS != result) {
       return result;
    }

    /* If the system is clocked from SPLL and SPLL parameters need to change,   */
    /* then fallback on the SPLL input clock to clock the system until the SPLL */
    /* locks with the new parameters.                                           */
    if (ADI_SYS_CLOCK_MUX_ROOT_SPLL == GetSystemClockMux(ADI_SYS_CLOCK_MUX_ROOT)) {
        /* SPLL drives the system clock, implies SPLL is already powered up */
        if (((pADI_SYSCLK->CLKCON3 & BITM_SYSCLK_CLKCON3_SPLLMSEL) != MEAS_SPLL_MSEL_DEFAULT) ||
            ((pADI_SYSCLK->CLKCON3 & BITM_SYSCLK_CLKCON3_SPLLNSEL) != MEAS_SPLL_NSEL_DEFAULT) ||
            ((pADI_SYSCLK->CLKCON3 & BITM_SYSCLK_CLKCON3_SPLLDIV2) != MEAS_SPLL_DIV2_DEFAULT)) {

            if (ADI_SYS_CLOCK_MUX_SPLL_HF_XTAL == GetSystemClockMux(ADI_SYS_CLOCK_MUX_SPLL)) {
                /* HFXTAL is SPLL source */
                SetSystemClockMux(ADI_SYS_CLOCK_MUX_ROOT_HFXTAL);
            } else {
                /* HFOSC is SPLL source */
                SetSystemClockMux(ADI_SYS_CLOCK_MUX_ROOT_HFOSC);
            }
            /* Program SPLL with parameters from state configuration */
            SetPllFreq(ADI_SYS_CLOCK_SOURCE_SPLL, MEAS_SPLL_MSEL_DEFAULT, MEAS_SPLL_NSEL_DEFAULT, MEAS_SPLL_DIV2_DEFAULT);
        }
    } else {
        /* SPLL doesn't drive the system clock, can be enabled or disabled */
        /* Program SPLL with parameters from state configuration */
        SetPllFreq(ADI_SYS_CLOCK_SOURCE_SPLL, MEAS_SPLL_MSEL_DEFAULT, MEAS_SPLL_NSEL_DEFAULT, MEAS_SPLL_DIV2_DEFAULT);
    }

    /* Power up SPLL */
    if (ADI_SYS_SUCCESS != (result = SystemClockSourcePowerUp(ADI_SYS_CLOCK_SOURCE_SPLL, ADI_SYS_CLOCK_TIMEOUT_USE_DEFAULT))) {
        return result;
    }

    /* Change clock dividers if needed */
    /* This makes sure that if the SPLL output frequency is higher than 16MHz, the system clock will be downscaled before SPLL is multiplexed in */
    changeClockDividers(pClockStateConfig, true);

    /* By now SPLL is up and stable, use it as system clock */
    SetSystemClockMux(ADI_SYS_CLOCK_MUX_ROOT_SPLL);

    /* Change clock dividers */
    changeClockDividers(pClockStateConfig, false);

    /* Power down unused clock sources */
    SystemClockSourcePowerDownIfUnused(ADI_SYS_CLOCK_SOURCE_UPLL);
    if (ADI_SYS_CLOCK_MUX_SPLL_HF_XTAL == GetSystemClockMux(ADI_SYS_CLOCK_MUX_SPLL)) {
        SystemClockSourcePowerDownIfUnused(ADI_SYS_CLOCK_SOURCE_HFOSC);
    } else {
        SystemClockSourcePowerDownIfUnused(ADI_SYS_CLOCK_SOURCE_HFXTAL);
    }

#elif defined(MEAS_CLOCK_SOURCE_HFXTAL)

    /* Using HFXTAL */

    /* Power up HFXTAL */
    if (ADI_SYS_SUCCESS != (result = SystemClockSourcePowerUp(ADI_SYS_CLOCK_SOURCE_HFXTAL, ADI_SYS_CLOCK_TIMEOUT_USE_DEFAULT))) {
        return result;
    }

    /* By now HFXTAL is up and stable, use it as system clock */
    SetSystemClockMux(ADI_SYS_CLOCK_MUX_ROOT_HFXTAL);

    /* Change clock dividers */
    changeClockDividers(pClockStateConfig, false);

    /* Power down unused clock sources */
    SystemClockSourcePowerDownIfUnused(ADI_SYS_CLOCK_SOURCE_UPLL);
    SystemClockSourcePowerDownIfUnused(ADI_SYS_CLOCK_SOURCE_SPLL);
    SystemClockSourcePowerDownIfUnused(ADI_SYS_CLOCK_SOURCE_HFOSC);

#else

    return ADI_SYS_ERR_INVALID_MEAS_CLOCK_SOURCE;

#endif

    /* Configure USBCTLCLK frequency to be the same as HCLK frequency */
    SetSystemClockDivider(ADI_SYS_CLOCK_USBCTL, 0);

    /* Update system clock variables */
    SystemCoreClockUpdate();

    return result;
}

/* Note: while the function is able to handle HFOSC as PLL input, it is strongly recommended    */
/* to use HFXTAL due to accuracy.                                                               */
ADI_SYS_RESULT_TYPE aEnterUSBState(void) {
    ADI_SYS_RESULT_TYPE result = ADI_SYS_SUCCESS;
    ADI_SYS_CLOCK_STATE_CONFIG_TYPE* pClockStateConfig;

    pClockStateConfig = &ClockStateConfig[ADI_SYS_CLOCK_STATE_USB];

    /* Using SPLL */
    /* First check if HFXTAL or HFOSC is PLL source */
    if (ADI_SYS_CLOCK_MUX_SPLL_HF_XTAL == GetSystemClockMux(ADI_SYS_CLOCK_MUX_SPLL)) {
        /* Power up HFXTAL */
        result = SystemClockSourcePowerUp(ADI_SYS_CLOCK_SOURCE_HFXTAL, ADI_SYS_CLOCK_TIMEOUT_USE_DEFAULT);
    } else {
        /* Power up HFOSC */
        result = SystemClockSourcePowerUp(ADI_SYS_CLOCK_SOURCE_HFOSC, ADI_SYS_CLOCK_TIMEOUT_USE_DEFAULT);
    }

    if (ADI_SYS_SUCCESS != result) {
        return result;
    }

    /* If the system is clocked from SPLL and SPLL parameters need to change,   */
    /* then fallback on the SPLL input clock to clock the system until the SPLL */
    /* locks with the new parameters.                                           */
    if (ADI_SYS_CLOCK_MUX_ROOT_SPLL == GetSystemClockMux(ADI_SYS_CLOCK_MUX_ROOT)) {
        if (((pADI_SYSCLK->CLKCON3 & BITM_SYSCLK_CLKCON3_SPLLMSEL) != SPLL_MSEL_DEFAULT) ||
            ((pADI_SYSCLK->CLKCON3 & BITM_SYSCLK_CLKCON3_SPLLNSEL) != SPLL_NSEL_DEFAULT) ||
            ((pADI_SYSCLK->CLKCON3 & BITM_SYSCLK_CLKCON3_SPLLDIV2) != SPLL_DIV2_DEFAULT)) {
            if (ADI_SYS_CLOCK_MUX_SPLL_HF_XTAL == GetSystemClockMux(ADI_SYS_CLOCK_MUX_SPLL)) {
                /* HFXTAL is SPLL source */
                SetSystemClockMux(ADI_SYS_CLOCK_MUX_ROOT_HFXTAL);
            } else {
                /* HFOSC is SPLL source */
                SetSystemClockMux(ADI_SYS_CLOCK_MUX_ROOT_HFOSC);
            }
            /* Program SPLL with parameters from state configuration */
            SetPllFreq(ADI_SYS_CLOCK_SOURCE_SPLL, SPLL_MSEL_DEFAULT, SPLL_NSEL_DEFAULT, SPLL_DIV2_DEFAULT);
        }
    } else {
        /* Program SPLL with parameters from state configuration */
        SetPllFreq(ADI_SYS_CLOCK_SOURCE_SPLL, SPLL_MSEL_DEFAULT, SPLL_NSEL_DEFAULT, SPLL_DIV2_DEFAULT);
    }

    /* Power up SPLL */
    if (ADI_SYS_SUCCESS != (result = SystemClockSourcePowerUp(ADI_SYS_CLOCK_SOURCE_SPLL, ADI_SYS_CLOCK_TIMEOUT_USE_DEFAULT))) {
        return result;
    }

    /* Change clock dividers if needed */
    /* This makes sure that if the SPLL output frequency is higher than 16MHz, the system clock will be downscaled before SPLL is multiplexed in */
    changeClockDividers(pClockStateConfig, true);

    /* By now SPLL is up and stable, use it as system clock */
    SetSystemClockMux(ADI_SYS_CLOCK_MUX_ROOT_SPLL);

    /* Change clock dividers */
    changeClockDividers(pClockStateConfig, false);

    /* Configure USBCTLCLK for 2x HCLK frequency */
    SetSystemClockDivider(ADI_SYS_CLOCK_USBCTL, 1);

    /* UPLL powered up after system clock, otherwise it may miss pulse from USBPHYCLK to USBCTLCLK */
    /* Program SPLL with parameters from state configuration */
    SetPllFreq(ADI_SYS_CLOCK_SOURCE_UPLL, UPLL_MSEL_DEFAULT, UPLL_NSEL_DEFAULT, UPLL_DIV2_DEFAULT);
    /* Power up UPLL */
    if (ADI_SYS_SUCCESS != (result = SystemClockSourcePowerUp(ADI_SYS_CLOCK_SOURCE_UPLL, ADI_SYS_CLOCK_TIMEOUT_USE_DEFAULT))) {
        return result;
    }

    /* Power down unused clock sources */
    if (ADI_SYS_CLOCK_MUX_SPLL_HF_XTAL == GetSystemClockMux(ADI_SYS_CLOCK_MUX_SPLL)) {
        SystemClockSourcePowerDownIfUnused(ADI_SYS_CLOCK_SOURCE_HFOSC);
    } else {
        SystemClockSourcePowerDownIfUnused(ADI_SYS_CLOCK_SOURCE_HFXTAL);
    }

    /* Update system clock variables */
    SystemCoreClockUpdate();

    return result;
}

ADI_SYS_RESULT_TYPE aEnterUSBLPMState(void) {
    ADI_SYS_RESULT_TYPE result = ADI_SYS_SUCCESS;
    ADI_SYS_CLOCK_STATE_CONFIG_TYPE* pClockStateConfig;

    pClockStateConfig = &ClockStateConfig[ADI_SYS_CLOCK_STATE_USB_LPM];

    /* Fall back on the PLL source clock as system clock */
    if (ADI_SYS_CLOCK_MUX_SPLL_HF_XTAL == GetSystemClockMux(ADI_SYS_CLOCK_MUX_SPLL)) {
        /* HFXTAL */
        SetSystemClockMux(ADI_SYS_CLOCK_MUX_ROOT_HFXTAL);
    } else {
        /* HFOSC */
        SetSystemClockMux(ADI_SYS_CLOCK_MUX_ROOT_HFOSC);
    }

    /* Change clock dividers */
    changeClockDividers(pClockStateConfig, false);

    /* Configure USBCTLCLK frequency to be the same as HCLK frequency */
    SetSystemClockDivider(ADI_SYS_CLOCK_USBCTL, 0);

    /* Power down PLLs */
    SystemClockSourcePowerDownIfUnused(ADI_SYS_CLOCK_SOURCE_UPLL);
    SystemClockSourcePowerDownIfUnused(ADI_SYS_CLOCK_SOURCE_SPLL);

    /* Update system clock variables */
    SystemCoreClockUpdate();

    return result;
}


/*
 * Change HCLK and PCLK dividers simultaneously.
 * If downshiftOnly is true, the divider change only happens if the next divider value is bigger than current divider value.
 * The purpose is to avoid situations when the core or a peripheral is clocked at a frequency higher than 16MHz.
 * For example, changing from (root_clk = 16MHz and HCLKDIVCNT = 1) to (root_clk = 32MHz and HCLKDIVCNT = 2)
 * will result in HCLK transitioning 16MHz -> 8MHz -> 16MHz, instead of 16MHz -> 32MHz -> 16MHz, which is illegal.
 *
 */

static void changeClockDividers(ADI_SYS_CLOCK_STATE_CONFIG_TYPE* pClockStateConfig, bool_t downshiftOnly) {

    /* If downshift-only is requested, but conditions for downshift are not met */
    /* (new divider ratios are not bigger than old divider ratios), then return */
    if (downshiftOnly && ((((pADI_SYSCLK->CLKCON1 & BITM_SYSCLK_CLKCON1_HCLKDIVCNT) >> BITP_SYSCLK_CLKCON1_HCLKDIVCNT) >= pClockStateConfig->HclkDiv) &&
                          (((pADI_SYSCLK->CLKCON1 & BITM_SYSCLK_CLKCON1_PCLKDIVCNT) >> BITP_SYSCLK_CLKCON1_PCLKDIVCNT) >= pClockStateConfig->PclkDiv))) {
        return ;
    }

    /* Don't use the API and write all clock dividers at once */
    /* This avoids the need for logic that maintains the HCLK vs. PCLK frequency relationship */
    pADI_SYSCLK->CLKCON1 = (((pClockStateConfig->HclkDiv << BITP_SYSCLK_CLKCON1_HCLKDIVCNT) & BITM_SYSCLK_CLKCON1_HCLKDIVCNT) |
                           ((pClockStateConfig->PclkDiv << BITP_SYSCLK_CLKCON1_PCLKDIVCNT) & BITM_SYSCLK_CLKCON1_PCLKDIVCNT));

}

#endif

/*
** EOF
*/

/*@}*/
