/*********************************************************************************

Copyright (c) 2012-2014 Analog Devices, Inc. All Rights Reserved.

This software is proprietary to Analog Devices, Inc. and its licensors.  By using 
this software you agree to the terms of the associated Analog Devices Software 
License Agreement.

*********************************************************************************/

/*!
 *****************************************************************************
   @file:    adi_gpt_config.h
   @brief:   Configuration options for General Purpose Timer driver.
             This is specific to the GPT driver and will be included by the driver.
             It is not required for the application to include this header file.
   @version: $Revision: 28525 $
   @date:    $Date: 2014-11-12 14:51:26 -0500 (Wed, 12 Nov 2014) $
*****************************************************************************/

#ifndef __ADI_GPT_CONIG_H__
#define __ADI_GPT_CONIG_H__

/************* GPT Driver configurations ***************/

/************* GPT controller configurations ***************/

/*************************************************************
                GP Timer 0 configurations
 *************************************************************/

/*! Prescale factor. Controls the prescaler division factor
    to the timer's selected clock. It can be set to

    0 - source_clock/[1 or 4]
    1 - source_clock/16
    2 - source_clock/256
    3 - source_clock/32768
*/
#define GPT0_CFG_PRESCALE_FACTOR                        3

/*! Count up or down. Used to control whether the timer increments (counts up)
    or decrements (counts down) the Up/Down counter, it can be set to
    0 -  Timer is set to count down.
    1 -  Timer is set to count up.
*/
#define GPT0_CFG_COUNT_UP                               0

/*! Timer mode. Used to control whether the timer runs in periodic or
    free running mode, it can be set to
    0 -  Timer is in free running mode.
    1 -  Timer is in periodic mode.
*/
#define GPT0_CFG_MODE                                   1


/*! Timer clock source. Used to select a timer clock from the four
    available clock sources, it can be set to
    0 - Select CLK Source 0.
    1 - Select CLK Source 1.
    2 - Select CLK Source 2.
    3 - Select CLK Source 3.
*/
#define GPT0_CFG_CLOCK_SOURCE                           0

/*! Reload control. This allows the user to select whether the Up/Down counter should be
    reset only on a timeout event or also when interrupt is cleared. It can be set to
    0 - Up/down counter is only reset on a time out event.
    1 - Resets the up/down counter when the interrupt is cleared.
*/
#define GPT0_CFG_RELOAD_CONTROL                         0


/*! Event to be captured. One of the selected 16 events associated
    with a general purpose time can be captured. It can be set to
    a value of 0 - 15. Please refer hardware reference manual to know
    which events can be catpured by a particular GP timer.
*/
#define GPT0_CFG_EVENT_CAPTURE                          0


/*! Enable or disable event capture. It can be set to
    0 - Disable event capturing.
    1 - Enable event capturing.
*/
#define GPT0_CFG_ENABLE_EVENT_CAPTURE                   0


/*! Timer asynchrounous load value. The Up/Down counter is periodically loaded with
    this value if periodic mode is selected. Writing Asyncrounous Load value takes
    advantage of having the timer run on PCLK by bypassing clock synchronization
    logic otherwise required. It can be set to any value from 0 to 65535.

*/
#define GPT0_CFG_ASYNC_LOAD_VALUE                       0


/*! Timer asynchrounous load value. The Up/Down counter is periodically loaded with this
    value if periodic mode is selected. LOAD writes during Up/Down counter timeout events
    are delayed until the event has passed. It can be set to any value from 0 to 65535.

*/
#define GPT0_CFG_LOAD_VALUE                             0


/*! Timer PWM Enable Match. This will control PWM operation mode of the timer.
    It can be set to
    0 - PWM in toggle mode.
    1 - PWM in match mode.
*/
#define GPT0_CFG_ENABLE_PWM_MATCH                       0


/*! Timer PWM Idle state. This will control PWM idle state. It can be set to
    0 - PWM idles low.
    1 - PWM idles high.
*/
#define GPT0_CFG_PWM_IDLE_STATE                         0


/*! PWM Match value. The value is used when the PWM is operating in match mode.
    The PWM output is asserted when the Up/Down counter is equal to this match value.
    PWM output is deasserted again when a timeout event occurs.
    If the match value is never reached, or occurs simultaneous to a timeout event,
    the PWM output remains idle. It can be any value from 0 to 65535.
*/
#define GPT0_CFG_PWM_MATCH_VALUE                        0


/*************************************************************
                GP Timer 1 configurations
 *************************************************************/

/*! Prescale factor. Controls the prescaler division factor
    to the timer's selected clock. It can be set to

    0 - source_clock/[1 or 4]
    1 - source_clock/16
    2 - source_clock/256
    3 - source_clock/32768
*/
#define GPT1_CFG_PRESCALE_FACTOR                        3

/*! Count up or down. Used to control whether the timer increments (counts up)
    or decrements (counts down) the Up/Down counter, it can be set to
    0 -  Timer is set to count down.
    1 -  Timer is set to count up.
*/
#define GPT1_CFG_COUNT_UP                               0

/*! Timer mode. Used to control whether the timer runs in periodic or
    free running mode, it can be set to
    0 -  Timer is in free running mode.
    1 -  Timer is in periodic mode.
*/
#define GPT1_CFG_MODE                                   1


/*! Timer clock source. Used to select a timer clock from the four
    available clock sources, it can be set to
    0 - Select CLK Source 0.
    1 - Select CLK Source 1.
    2 - Select CLK Source 2.
    3 - Select CLK Source 3.
*/
#define GPT1_CFG_CLOCK_SOURCE                           0

/*! Reload control. This allows the user to select whether the Up/Down counter should be
    reset only on a timeout event or also when interrupt is cleared. It can be set to
    0 - Up/down counter is only reset on a time out event.
    1 - Resets the up/down counter when the interrupt is cleared.
*/
#define GPT1_CFG_RELOAD_CONTROL                         0


/*! Event to be captured. One of the selected 16 events associated
    with a general purpose time can be captured. It can be set to
    a value of 0 - 15. Please refer hardware reference manual to know
    which events can be catpured by a particular GP timer.
*/
#define GPT1_CFG_EVENT_CAPTURE                          0


/*! Enable or disable event capture. It can be set to
    0 - Disable event capturing.
    1 - Enable event capturing.
*/
#define GPT1_CFG_ENABLE_EVENT_CAPTURE                   0


/*! Timer asynchrounous load value. The Up/Down counter is periodically loaded with
    this value if periodic mode is selected. Writing Asyncrounous Load value takes
    advantage of having the timer run on PCLK by bypassing clock synchronization
    logic otherwise required. It can be set to any value from 0 to 65535.

*/
#define GPT1_CFG_ASYNC_LOAD_VALUE                       0


/*! Timer asynchrounous load value. The Up/Down counter is periodically loaded with this
    value if periodic mode is selected. LOAD writes during Up/Down counter timeout events
    are delayed until the event has passed. It can be set to any value from 0 to 65535.

*/
#define GPT1_CFG_LOAD_VALUE                             0


/*! Timer PWM Enable Match. This will control PWM operation mode of the timer.
    It can be set to
    0 - PWM in toggle mode.
    1 - PWM in match mode.
*/
#define GPT1_CFG_ENABLE_PWM_MATCH                       0


/*! Timer PWM Idle state. This will control PWM idle state. It can be set to
    0 - PWM idles low.
    1 - PWM idles high.
*/
#define GPT1_CFG_PWM_IDLE_STATE                         0


/*! PWM Match value. The value is used when the PWM is operating in match mode.
    The PWM output is asserted when the Up/Down counter is equal to this match value.
    PWM output is deasserted again when a timeout event occurs.
    If the match value is never reached, or occurs simultaneous to a timeout event,
    the PWM output remains idle. It can be any value from 0 to 65535.
*/
#define GPT1_CFG_PWM_MATCH_VALUE                        0


#if defined(ADI_ADUCM350) || defined(ADI_ADUCM320)

/*************************************************************
                GP Timer 2 configurations
 *************************************************************/

/*! Prescale factor. Controls the prescaler division factor
    to the timer's selected clock. It can be set to

    0 - source_clock/[1 or 4]
    1 - source_clock/16
    2 - source_clock/256
    3 - source_clock/32768
*/
#define GPT2_CFG_PRESCALE_FACTOR                        3

/*! Count up or down. Used to control whether the timer increments (counts up)
    or decrements (counts down) the Up/Down counter, it can be set to
    0 -  Timer is set to count down.
    1 -  Timer is set to count up.
*/
#define GPT2_CFG_COUNT_UP                               0

/*! Timer mode. Used to control whether the timer runs in periodic or
    free running mode, it can be set to
    0 -  Timer is in free running mode.
    1 -  Timer is in periodic mode.
*/
#define GPT2_CFG_MODE                                   1


/*! Timer clock source. Used to select a timer clock from the four
    available clock sources, it can be set to
    0 - Select CLK Source 0.
    1 - Select CLK Source 1.
    2 - Select CLK Source 2.
    3 - Select CLK Source 3.
*/
#define GPT2_CFG_CLOCK_SOURCE                           0

/*! Reload control. This allows the user to select whether the Up/Down counter should be
    reset only on a timeout event or also when interrupt is cleared. It can be set to
    0 - Up/down counter is only reset on a time out event.
    1 - Resets the up/down counter when the interrupt is cleared.
*/
#define GPT2_CFG_RELOAD_CONTROL                         0


/*! Event to be captured. One of the selected 16 events associated
    with a general purpose time can be captured. It can be set to
    a value of 0 - 15. Please refer hardware reference manual to know
    which events can be catpured by a particular GP timer.
*/
#define GPT2_CFG_EVENT_CAPTURE                          0


/*! Enable or disable event capture. It can be set to
    0 - Disable event capturing.
    1 - Enable event capturing.
*/
#define GPT2_CFG_ENABLE_EVENT_CAPTURE                   0


/*! Timer asynchrounous load value. The Up/Down counter is periodically loaded with
    this value if periodic mode is selected. Writing Asyncrounous Load value takes
    advantage of having the timer run on PCLK by bypassing clock synchronization
    logic otherwise required. It can be set to any value from 0 to 65535.

*/
#define GPT2_CFG_ASYNC_LOAD_VALUE                       0


/*! Timer asynchrounous load value. The Up/Down counter is periodically loaded with this
    value if periodic mode is selected. LOAD writes during Up/Down counter timeout events
    are delayed until the event has passed. It can be set to any value from 0 to 65535.

*/
#define GPT2_CFG_LOAD_VALUE                             0


/*! Timer PWM Enable Match. This will control PWM operation mode of the timer.
    It can be set to
    0 - PWM in toggle mode.
    1 - PWM in match mode.
*/
#define GPT2_CFG_ENABLE_PWM_MATCH                       0


/*! Timer PWM Idle state. This will control PWM idle state. It can be set to
    0 - PWM idles low.
    1 - PWM idles high.
*/
#define GPT2_CFG_PWM_IDLE_STATE                         0


/*! PWM Match value. The value is used when the PWM is operating in match mode.
    The PWM output is asserted when the Up/Down counter is equal to this match value.
    PWM output is deasserted again when a timeout event occurs.
    If the match value is never reached, or occurs simultaneous to a timeout event,
    the PWM output remains idle. It can be any value from 0 to 65535.
*/
#define GPT2_CFG_PWM_MATCH_VALUE                        0

#endif /* ADUCM350 */



/************** Macro validation *****************************/

/*************************************************************
                        GP Timer 0 validation macros.
**************************************************************/

#if (GPT0_CFG_PRESCALE_FACTOR > 3)
#error "Invalid configuration"
#endif

#if (GPT0_CFG_COUNT_UP > 1)
#error "Invalid configuration"
#endif

#if (GPT0_CFG_MODE > 1)
#error "Invalid configuration"
#endif

#if (GPT0_CFG_CLOCK_SOURCE > 3)
#error "Invalid configuration"
#endif

#if (GPT0_CFG_RELOAD_CONTROL > 1)
#error "Invalid configuration"
#endif

#if (GPT0_CFG_EVENT_CAPTURE > 15)
#error "Invalid configuration"
#endif

#if (GPT0_CFG_ENABLE_EVENT_CAPTURE > 1)
#error "Invalid configuration"
#endif

#if (GPT0_CFG_ASYNC_LOAD_VALUE > 65535)
#error "Invalid configuration"
#endif

#if (GPT0_CFG_LOAD_VALUE  > 65535)
#error "Invalid configuration"
#endif

#if (GPT0_CFG_ENABLE_PWM_MATCH > 1)
#error "Invalid configuration"
#endif

#if (GPT0_CFG_PWM_IDLE_STATE > 1)
#error "Invalid configuration"
#endif

#if (GPT0_CFG_PWM_MATCH_VALUE > 65535)
#error "Invalid configuration"
#endif

/*************************************************************
                        GP Timer 1 validation macros.
**************************************************************/

#if (GPT1_CFG_PRESCALE_FACTOR > 3)
#error "Invalid configuration"
#endif

#if (GPT1_CFG_COUNT_UP > 1)
#error "Invalid configuration"
#endif

#if (GPT1_CFG_MODE > 1)
#error "Invalid configuration"
#endif

#if (GPT1_CFG_CLOCK_SOURCE > 3)
#error "Invalid configuration"
#endif

#if (GPT1_CFG_RELOAD_CONTROL > 1)
#error "Invalid configuration"
#endif

#if (GPT1_CFG_EVENT_CAPTURE > 15)
#error "Invalid configuration"
#endif

#if (GPT1_CFG_ENABLE_EVENT_CAPTURE > 1)
#error "Invalid configuration"
#endif

#if (GPT1_CFG_ASYNC_LOAD_VALUE > 65535)
#error "Invalid configuration"
#endif

#if (GPT1_CFG_LOAD_VALUE  > 65535)
#error "Invalid configuration"
#endif

#if (GPT1_CFG_ENABLE_PWM_MATCH > 1)
#error "Invalid configuration"
#endif

#if (GPT1_CFG_PWM_IDLE_STATE > 1)
#error "Invalid configuration"
#endif

#if (GPT1_CFG_PWM_MATCH_VALUE > 65535)
#error "Invalid configuration"
#endif

#ifdef ADUCM350

/*************************************************************
                        GP Timer 2 validation macros.
**************************************************************/

#if (GPT2_CFG_PRESCALE_FACTOR > 3)
#error "Invalid configuration"
#endif

#if (GPT2_CFG_COUNT_UP > 1)
#error "Invalid configuration"
#endif

#if (GPT2_CFG_MODE > 1)
#error "Invalid configuration"
#endif

#if (GPT2_CFG_CLOCK_SOURCE > 3)
#error "Invalid configuration"
#endif

#if (GPT2_CFG_RELOAD_CONTROL > 1)
#error "Invalid configuration"
#endif

#if (GPT2_CFG_EVENT_CAPTURE > 15)
#error "Invalid configuration"
#endif

#if (GPT2_CFG_ENABLE_EVENT_CAPTURE > 1)
#error "Invalid configuration"
#endif

#if (GPT2_CFG_ASYNC_LOAD_VALUE > 65535)
#error "Invalid configuration"
#endif

#if (GPT2_CFG_LOAD_VALUE  > 65535)
#error "Invalid configuration"
#endif

#if (GPT2_CFG_ENABLE_PWM_MATCH > 1)
#error "Invalid configuration"
#endif

#if (GPT2_CFG_PWM_IDLE_STATE > 1)
#error "Invalid configuration"
#endif

#if (GPT2_CFG_PWM_MATCH_VALUE > 65535)
#error "Invalid configuration"
#endif

#endif /* ADUCM350 */

#endif /* __ADI_GPT_CONIG_H__ */
