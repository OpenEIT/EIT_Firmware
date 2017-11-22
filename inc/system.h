/*!
 *****************************************************************************
 * @file:    system.h
 * @brief:   CMSIS Cortex-M3 Device Peripheral Access Layer Header File
 *           for the ADI ADuCxxx Device Series
 * @version: $Revision: 23737 $
 * @date:    $Date: 2013-11-25 11:40:37 -0500 (Mon, 25 Nov 2013) $
 *-----------------------------------------------------------------------------
 *
 * Copyright (C) 2009-2013 ARM Limited. All rights reserved.
 *
 * ARM Limited (ARM) is supplying this software for use with Cortex-M3
 * processor based microcontrollers.  This file can be freely distributed
 * within development tools that are supporting such ARM based processors.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * ARM SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 *****************************************************************************/


/*! \addtogroup SYS_Driver System Interfaces
 *  @{
 * add result types to doxygen
 */

#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include <stddef.h>     /* for 'NULL' */

#include "device.h"

/* Needed for checking GPT input clocks when powering down clock sources. */
#include "gpt.h"
/* Needed for checking WUT input clocks when powering down clock sources. */
#include "wut.h"
/* Needed for external GPIO driving root_clk. */
#include "gpio.h"

#ifdef __cplusplus
 extern "C" {
#endif

/* clock IDs */
typedef enum {

    ADI_SYS_CLOCK_USBPHY,
    ADI_SYS_CLOCK_USBCTL,

    ADI_SYS_CLOCK_PCLK,
    ADI_SYS_CLOCK_I2C,
    ADI_SYS_CLOCK_UART,
    ADI_SYS_CLOCK_I2S,
    ADI_SYS_CLOCK_SPI0,
    ADI_SYS_CLOCK_SPI1,

    ADI_SYS_CLOCK_SPIH,
    ADI_SYS_CLOCK_CORE,
    ADI_SYS_CLOCK_BUS,
    ADI_SYS_CLOCK_FCLK,
    ADI_SYS_CLOCK_HCLK_CT,
    ADI_SYS_CLOCK_ACLK,

    ADI_SYS_CLOCK_CT,
    ADI_SYS_CLOCK_AFE

} ADI_SYS_CLOCK_ID;

/* Clock sources */
typedef enum {
    ADI_SYS_CLOCK_SOURCE_HFXTAL,
    ADI_SYS_CLOCK_SOURCE_LFXTAL,
    ADI_SYS_CLOCK_SOURCE_HFOSC,
    ADI_SYS_CLOCK_SOURCE_LFOSC,
    ADI_SYS_CLOCK_SOURCE_SPLL,
    ADI_SYS_CLOCK_SOURCE_UPLL,
    ADI_SYS_CLOCK_SOURCE_GPIO,
} ADI_SYS_CLOCK_SOURCE_ID;

/*! \enum ADI_SYS_CLOCK_MUX_GROUP_ID
 * Clock multiplexer identifiers        */
typedef enum {
    ADI_SYS_CLOCK_MUX_SPLL,         /*!< SPLL/UPLL input mux            */
    ADI_SYS_CLOCK_MUX_LFCLK,        /*!< 32kHz (LFLCK) clock mux        */
    ADI_SYS_CLOCK_MUX_OUTPUT,       /*!< Clock out mux                  */
    ADI_SYS_CLOCK_MUX_ROOT,         /*!< root_clk mux                   */
} ADI_SYS_CLOCK_MUX_GROUP_ID;

/* clock multiplexer IDs */
typedef enum {

    /* SPLL/UPLL mux selectors */
    ADI_SYS_CLOCK_MUX_SPLL_HF_OSC,
    ADI_SYS_CLOCK_MUX_SPLL_HF_XTAL,

    /* 32kHz (LFLCK) clock mux selectors */
    ADI_SYS_CLOCK_MUX_LFCLK_LFOSC,
    ADI_SYS_CLOCK_MUX_LFCLK_LFXTAL,

    /* clock out mux selectors */
    ADI_SYS_CLOCK_MUX_OUTPUT_ROOT_CLK,
    ADI_SYS_CLOCK_MUX_OUTPUT_LF_CLK,
    ADI_SYS_CLOCK_MUX_OUTPUT_CT_CLK,
    ADI_SYS_CLOCK_MUX_OUTPUT_HCLK_BUS,
    ADI_SYS_CLOCK_MUX_OUTPUT_HCLK_CORE,
    ADI_SYS_CLOCK_MUX_OUTPUT_P_CLK,
    ADI_SYS_CLOCK_MUX_OUTPUT_USB_CTRL_CLK,
    ADI_SYS_CLOCK_MUX_OUTPUT_USB_PHY_CLK,
    ADI_SYS_CLOCK_MUX_OUTPUT_GPT0_CLK,
    ADI_SYS_CLOCK_MUX_OUTPUT_GPT1_CLK,
    ADI_SYS_CLOCK_MUX_OUTPUT_GPT2_CLK,
    ADI_SYS_CLOCK_MUX_OUTPUT_WUT_CLK,
    ADI_SYS_CLOCK_MUX_OUTPUT_RTC_CNT_CLK,

    /* main clock mux selectors */
    ADI_SYS_CLOCK_MUX_ROOT_HFOSC,
    ADI_SYS_CLOCK_MUX_ROOT_HFXTAL,
    ADI_SYS_CLOCK_MUX_ROOT_SPLL,
    ADI_SYS_CLOCK_MUX_ROOT_GPIO,
} ADI_SYS_CLOCK_MUX_ID;


/* clock gate IDs */
typedef enum {

    /* clock sources */
    ADI_SYS_CLOCK_GATE_CTCLK,
    ADI_SYS_CLOCK_GATE_ACLK,
    ADI_SYS_CLOCK_GATE_I2S,
    ADI_SYS_CLOCK_GATE_UART,
    ADI_SYS_CLOCK_GATE_I2C,
    ADI_SYS_CLOCK_GATE_SPIH,
    ADI_SYS_CLOCK_GATE_SPI1,
    ADI_SYS_CLOCK_GATE_SPI0,
} ADI_SYS_CLOCK_GATE_ID;

/*! \enum ADI_SYS_CLOCK_TIMEOUT_TYPE
 *  Used to control the timeout failsafes while powering up clock sources.  */
typedef enum {
    ADI_SYS_CLOCK_TIMEOUT_DONT_WAIT_STABLE = 0,
    ADI_SYS_CLOCK_TIMEOUT_USE_DEFAULT,
} ADI_SYS_CLOCK_TIMEOUT_TYPE;

/*! \enum ADI_SYS_CLOCK_SOURCE_STATUS
 *  Clock source status. */
typedef enum {

    ADI_SYS_CLOCK_SOURCE_DISABLED = 0,
    ADI_SYS_CLOCK_SOURCE_ENABLED_NOT_STABLE,
    ADI_SYS_CLOCK_SOURCE_ENABLED_STABLE,
    ADI_SYS_CLOCK_SOURCE_ID_NOT_VALID,

} ADI_SYS_CLOCK_SOURCE_STATUS;

/*! \enum ADI_SYS_CLOCK_PLL_NSEL_TYPE
 *  PLL parameter N (multiplier), applies to both SPLL and UPLL.
 *  The enumeration is incomplete, it only covers the recommended PLL settings.
 *  Using SPLL bit position macros, the UPLL are identical.
 */
typedef enum {
    ADI_SYS_CLOCK_PLL_NSEL_4  = ( 4 << BITP_SYSCLK_CLKCON3_SPLLNSEL),
    ADI_SYS_CLOCK_PLL_NSEL_8  = ( 8 << BITP_SYSCLK_CLKCON3_SPLLNSEL),
    ADI_SYS_CLOCK_PLL_NSEL_15 = (15 << BITP_SYSCLK_CLKCON3_SPLLNSEL),
    ADI_SYS_CLOCK_PLL_NSEL_16 = (16 << BITP_SYSCLK_CLKCON3_SPLLNSEL),
} ADI_SYS_CLOCK_PLL_NSEL_TYPE;

/*! \enum ADI_SYS_CLOCK_PLL_MSEL_TYPE
 *  PLL parameter M (divider), applies to both SPLL and UPLL.
 *  Using SPLL bit position macros, the UPLL are identical.
 */
typedef enum {
    ADI_SYS_CLOCK_PLL_MSEL_1 = (0 << BITP_SYSCLK_CLKCON3_SPLLMSEL),
    ADI_SYS_CLOCK_PLL_MSEL_2 = (1 << BITP_SYSCLK_CLKCON3_SPLLMSEL),
    ADI_SYS_CLOCK_PLL_MSEL_4 = (2 << BITP_SYSCLK_CLKCON3_SPLLMSEL),
} ADI_SYS_CLOCK_PLL_MSEL_TYPE;

/*! \enum ADI_SYS_CLOCK_PLL_DIV2_TYPE
 *  PLL parameter DIV2, applies to both SPLL and UPLL.
 *  Using SPLL bit position macros, the UPLL are identical.
 */
typedef enum {
    ADI_SYS_CLOCK_PLL_DIV2_DISABLED = (0 << BITP_SYSCLK_CLKCON3_SPLLDIV2),
    ADI_SYS_CLOCK_PLL_DIV2_ENABLED  = (1 << BITP_SYSCLK_CLKCON3_SPLLDIV2),
} ADI_SYS_CLOCK_PLL_DIV2_TYPE;

 /*!
 *   Power modes
 */
typedef enum
{
    ADI_SYS_MODE_ACTIVE         = 0,       /*!< Fully Active */
    ADI_SYS_MODE_CORE_SLEEP     = 1,       /*!< Core Sleep power-down mode */
    ADI_SYS_MODE_SYS_SLEEP      = 2,       /*!< System Sleep power-down mode */
    ADI_SYS_MODE_HIBERNATE      = 3        /*!< Full Hybernate power-down mode */

} ADI_SYS_POWER_MODE;


 /*!
 *****************************************************************************
 * \enum ADI_SYS_RESULT_TYPE
 *
  * System API function return codes
 *
 *****************************************************************************/
typedef enum
{
    ADI_SYS_SUCCESS = 0,                                    /*!< No error detected.                                                 */
    ADI_SYS_ERR_UNKNOWN_ERROR = ADI_DEV_SYS_ERROR_OFFSET,   /*!< Generic unknown error occurred.                                    */
    ADI_SYS_ERR_CLOCK_DIVIDE_OUT_OF_RANGE,                  /*!< Requested divide value is out of range.                            */
    ADI_SYS_ERR_INVALID_CLOCK_ID,                           /*!< Invalid ADI_SYS_CLOCK_ID specified.                                */
    ADI_SYS_ERR_INVALID_CLOCK_INTERRUPT,                    /*!< Invalid ADI_SYS_CLOCK_INTERRUPT_ID specified.                      */
    ADI_SYS_ERR_INVALID_CLOCK_RATIO,                        /*!< PDIV:HDIV ration must be integral.                                 */
    ADI_SYS_ERR_INVALID_CLOCK_SPEED,                        /*!< Peripheral clock must >= system clock.                             */
    ADI_SYS_ERR_INVALID_POWER_MODE,                         /*!< Invalid low-power mode requested.                                  */
    ADI_SYS_ERR_PARAM_OUT_OF_RANGE,                         /*!< Input param out of range.                                          */
    ADI_SYS_ERR_CLOCK_SOURCE_TIMEOUT,                       /*!< Timeout while clock source powering up.                            */
    ADI_SYS_ERR_CLOCK_SOURCE_USED_ROOT_CLK,                 /*!< Clock source used as root clock when trying to power it down.      */
    ADI_SYS_ERR_CLOCK_SOURCE_USED_PLL_CLK,                  /*!< Clock source used as PLL clock when trying to power it down.       */
    ADI_SYS_ERR_CLOCK_SOURCE_USED_GPT_CLK,                  /*!< Clock source used as GPT clock when trying to power it down.       */
    ADI_SYS_ERR_CLOCK_SOURCE_USED_WUT_CLK,                  /*!< Clock source used as WUT clock when trying to power it down.       */
    ADI_SYS_ERR_CLOCK_SOURCE_USED_RTC_CLK,                  /*!< LFXTAL used as RTC clock when trying to power it down.             */
    ADI_SYS_ERR_CLOCK_SOURCE_USED_LF_CLK,                   /*!< Clock source used as lf_clk when trying to power it down.          */
    ADI_SYS_ERR_CLOCK_PLACEHOLDER,                          /*!< Unused placeholder                                                 */
    ADI_SYS_ERR_SPLL_CHANGE_NOT_ALLOWED,                    /*!< SPLL drives the system clock and cannot be changed.                */
    ADI_SYS_ERR_PORT_NOT_VALID,                             /*!< Error when configuring GPIO as clock source.                       */
    ADI_SYS_ERR_INVALID_MEAS_CLOCK_SOURCE,                  /*!< MEAS_CLOCK_SOURCE macro invalid or undefined.                      */
    ADI_SYS_ERR_INVALID_STATE_TRANSITION,                   /*!< Undefined system clock state or invalid transition                 */
} ADI_SYS_RESULT_TYPE;


/* The following definitions are part of the clock transition state machine. */
/* This is a new feature, disabled by default, can be enabled by defining    */
/* the macro ADI_SYSTEM_CLOCK_TRANSITION.                                    */
/* Enabling it is optional, unless using the USB driver which requires the   */
/* new functions call SystemTransitionClocks().                              */

#ifdef ADI_SYSTEM_CLOCK_TRANSITION

/*! \enum ADI_SYS_CLOCK_STATE_TYPE
 *  Clock configuration states:
 *  - ADI_SYS_CLOCK_STATE_MINIMAL:          The state the ADuCM350 powers up in. Uses HFOSC to supply the system clock.
 *  - ADI_SYS_CLOCK_STATE_MEASUREMENT:      The measurement configuration, can be setup at build time to use either HFXTAL or SPLL as source for the system clock.
 *                                          This state does not include USB.
 *  - ADI_SYS_CLOCK_STATE_USB:              The USB configuration, using SPLL for system clock and UPLL for USB PHY. If both USB and measurement configurations
 *                                          are requested, the part will use the USB configuration.
 *  - ADI_SYS_CLOCK_STATE_USB_LPM:          USB LPM configuration.
 *  - ADI_SYS_CLOCK_STATE_USB_MEASUREMENT:  Intermediate state with no functionality of its own. It is used to manage state transitions when USB and measurement
 *                                          configurations are requested asynchronously.
 */
typedef enum {
    ADI_SYS_CLOCK_STATE_MINIMAL = 1,        /* use one-based enum to avoid confusion with NULL, which is used to flag invalid state transitions */
    ADI_SYS_CLOCK_STATE_MEASUREMENT,
    ADI_SYS_CLOCK_STATE_USB,
    ADI_SYS_CLOCK_STATE_USB_MEASUREMENT,
    ADI_SYS_CLOCK_STATE_USB_LPM,
} ADI_SYS_CLOCK_STATE_TYPE;

/*! \enum ADI_SYS_CLOCK_STATE_TYPE
 *  Clock state transition triggers:
 *  - ADI_SYS_CLOCK_TRIGGER_MEASUREMENT_ON:     Enter measurement state.
 *  - ADI_SYS_CLOCK_TRIGGER_MEASUREMENT_OFF:    Exit measurement state.
 *  - ADI_SYS_CLOCK_TRIGGER_USB_ON:             Corresponds to "Enable USB clocks" call in HRM. Controlled by the USB driver.
 *  - ADI_SYS_CLOCK_TRIGGER_USB_OFF:            Corresponds to "Disable USB clocks" call in HRM. Controlled by the USB driver.
 *  - ADI_SYS_CLOCK_TRIGGER_USB_LPM:            Corresponds to "Disable USB clocks for LPM sleep" call in HRM. Controlled by the USB driver.
 */
typedef enum {
    ADI_SYS_CLOCK_TRIGGER_MEASUREMENT_ON = 0,
    ADI_SYS_CLOCK_TRIGGER_MEASUREMENT_OFF,
    ADI_SYS_CLOCK_TRIGGER_USB_ON,
    ADI_SYS_CLOCK_TRIGGER_USB_OFF,
    ADI_SYS_CLOCK_TRIGGER_USB_LPM,
    ADI_SYS_CLOCK_TRIGGER_NUM          // size marker
} ADI_SYS_CLOCK_TRIGGER_TYPE;


// Generic action function typedef
typedef ADI_SYS_RESULT_TYPE (*ClockAction)     (void);

/*! \enum ADI_SYS_CLOCK_STATE_CONFIG_TYPE
 * State parameters for system clocks state machine
 * Only the clock dividers are configurable at runtime, everything else is configured with #define macros. */
typedef struct {
    uint8_t                         HclkDiv;
    uint8_t                         PclkDiv;
} ADI_SYS_CLOCK_STATE_CONFIG_TYPE;

#endif


/* Peripheral and system related clock APIs */
extern void                         SystemInit                              (void);
extern void                         SystemCoreClockUpdate                   (void);
extern void                         SetSystemExtClkFreq                     (uint32_t ExtClkFreq);
extern void                         SetSystemClockMux                       (ADI_SYS_CLOCK_MUX_ID id);
extern ADI_SYS_CLOCK_MUX_ID         GetSystemClockMux                       (ADI_SYS_CLOCK_MUX_GROUP_ID groupID);
extern uint32_t                     GetSystemExtClkFreq                     (void);
extern uint32_t                     SystemGetClockFrequency                 (ADI_SYS_CLOCK_ID id);
extern ADI_SYS_RESULT_TYPE          SystemEnableClock                       (ADI_SYS_CLOCK_GATE_ID id, bool_t bFlag);
extern ADI_SYS_RESULT_TYPE          SystemEnableClockSource                 (ADI_SYS_CLOCK_SOURCE_ID id, bool_t bEnable);
extern ADI_SYS_RESULT_TYPE          SetSystemClockDivider                   (ADI_SYS_CLOCK_ID id, uint8_t div);
extern ADI_SYS_RESULT_TYPE          SystemClockSourcePowerUp                (ADI_SYS_CLOCK_SOURCE_ID id,
                                                                             ADI_SYS_CLOCK_TIMEOUT_TYPE timeout);
extern ADI_SYS_RESULT_TYPE          SystemClockSourcePowerDownIfUnused      (ADI_SYS_CLOCK_SOURCE_ID id);
extern ADI_SYS_CLOCK_SOURCE_STATUS  GetClockSourceStatus                    (ADI_SYS_CLOCK_SOURCE_ID id);
extern ADI_SYS_RESULT_TYPE          SetPllFreq                              (ADI_SYS_CLOCK_SOURCE_ID id,
                                                                             ADI_SYS_CLOCK_PLL_MSEL_TYPE msel,
                                                                             ADI_SYS_CLOCK_PLL_NSEL_TYPE nsel,
                                                                             ADI_SYS_CLOCK_PLL_DIV2_TYPE div2);
#ifdef ADI_SYSTEM_CLOCK_TRANSITION
extern ADI_SYS_RESULT_TYPE          SystemTransitionClocks                  (ADI_SYS_CLOCK_TRIGGER_TYPE transitionEvent);
extern ADI_SYS_RESULT_TYPE          SetClockStateConfig                     (ADI_SYS_CLOCK_STATE_TYPE state,
                                                                             const ADI_SYS_CLOCK_STATE_CONFIG_TYPE* const pConfig);
extern ADI_SYS_RESULT_TYPE          GetClockStateConfig                     (ADI_SYS_CLOCK_STATE_TYPE state,
                                                                             ADI_SYS_CLOCK_STATE_CONFIG_TYPE* const pConfig);
extern ADI_SYS_RESULT_TYPE          GetClockState                           (ADI_SYS_CLOCK_STATE_TYPE* pState);
#endif

extern ADI_SYS_RESULT_TYPE          SystemEnterLowPowerMode                 (const ADI_SYS_POWER_MODE PowerMode,
                                                                             bool_t volatile *        pbInterruptOccurred,
                                                                             const uint8_t            PriorityMask);
extern void                         SystemExitLowPowerMode                  (bool_t volatile *        pbInterruptOccurred);

#ifdef __cplusplus
}
#endif

#endif /* __SYSTEM_H__ */


/*@}*//*
** EOF
*/
