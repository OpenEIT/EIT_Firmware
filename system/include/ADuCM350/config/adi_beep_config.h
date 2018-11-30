/*********************************************************************************

Copyright (c) 2012-2014 Analog Devices, Inc. All Rights Reserved.

This software is proprietary to Analog Devices, Inc. and its licensors.  By using 
this software you agree to the terms of the associated Analog Devices Software 
License Agreement (See /LICENSES/ADI.LABLAB.LICENSE). 

Modified from the Analog Devices original.
Portions Copyright (c) 2018 Andrew Grosser, Moritz von Buttlar.

*********************************************************************************/

/*!
 *****************************************************************************
   @file:    adi_beep_config.h
   @brief:   Configuration options for BEEP driver.
             This is specific to the BEEP driver and will be included by the driver.
             It is not required for the application to include this header file.
   @version: $Revision: 28525 $
   @date:    $Date: 2014-11-12 14:51:26 -0500 (Wed, 12 Nov 2014) $
*****************************************************************************/

#ifndef __ADI_BEEP_CONIG_H__
#define __ADI_BEEP_CONIG_H__

/************* BEEP Driver configurations ***************/

/************* BEEP controller static configurations ***************/


/* CONFIGURATION REGISTER */

/*! Configure beeper to generate interrupts on sequence end event.
    0 -  No interrupt.
    1 -  Generate Sequence End interrupt. */
#define BEEP_CFG_INTERRUPT_ON_SEQUENCE_END                 0

/*! Configure beeper to generate interrupts on sequence near end event.
    0 -  No interrupt.
    1 -  Generate Sequence Near End interrupt. */
#define BEEP_CFG_INTERRUPT_ON_SEQUENCE_NEAR_END            0

/*! Configure beeper to generate interrupts on ToneB end event.
    0 -  No interrupt.
    1 -  Generate ToneB End interrupt. */
#define BEEP_CFG_INTERRUPT_ON_TONEB_END                    0

/*! Configure beeper to generate interrupts on ToneB start event.
    0 -  No interrupt.
    1 -  Generate ToneB Start interrupt. */
#define BEEP_CFG_INTERRUPT_ON_TONEB_START                  0

/*! Configure beeper to generate interrupts on ToneA end event.
    0 -  No interrupt.
    1 -  Generate ToneA End interrupt. */
#define BEEP_CFG_INTERRUPT_ON_TONEA_END                    0

/*! Configure beeper to generate interrupts on ToneA start event.
    0 -  No interrupt.
    1 -  Generate ToneA Start interrupt. */
#define BEEP_CFG_INTERRUPT_ON_TONEA_START                  1

/*! Configure beeper disable.
    0 -  Beeper enabled.
    1 -  Beeper disabled. */
#define BEEP_CFG_BEEPER_DISABLE                            0

/*! Configure beeper for pulse/repeat mode.
    (assume sequential mode, but need to set length dynamically)
    0     -  Pulse mode.
    1-255 -  Sequence mode repeat count. */
#define BEEP_CFG_SEQUENCE_REPEAT_VALUE                     1



/* STATUS REGISTER */

/*! Clear the sequence end interrupt bit.
    0 -  Do not clear the interrupt.
    1 -  Clear the interrupt. */
#define BEEP_STAT_CLEAR_SEQUENCE_END_INTERRUPT             1

/*! Clear the sequence near end interrupt bit.
    0 -  Do not clear the interrupt.
    1 -  Clear the interrupt. */
#define BEEP_STAT_CLEAR_SEQUENCE_NEAR_END_INTERRUPT        1

/*! Clear the ToneB end interrupt bit.
    0 -  Do not clear the interrupt.
    1 -  Clear the interrupt. */
#define BEEP_STAT_CLEAR_TONEB_END_INTERRUPT                1

/*! Clear the ToneB start interrupt bit.
    0 -  Do not clear the interrupt.
    1 -  Clear the interrupt. */
#define BEEP_STAT_CLEAR_TONEB_START_INTERRUPT              1

/*! Clear the ToneA end interrupt bit.
    0 -  Do not clear the interrupt.
    1 -  Clear the interrupt. */
#define BEEP_STAT_CLEAR_TONEA_END_INTERRUPT                1

/*! Clear the ToneA start interrupt bit.
    0 -  Do not clear the interrupt.
    1 -  Clear the interrupt. */
#define BEEP_STAT_CLEAR_TONEA_START_INTERRUPT              1



/* TONEA CONTROL REGISTER */

/*! Initial ToneA Disable.
    0 -  ToneA Enabled.
    1 -  ToneA Disabled. */
#define BEEP_TONEA_DISABLE                                 0

/*! Initial ToneA Frequency.
    0-3   -  Rest Tone (no oscillation).
    4-127 -  Oscillate at 32kHz/freq Hz. */
#define BEEP_TONEA_FREQUENCY                               0

/*! Initial ToneA Duration.
    0-254 -  Play for 4ms*duration.
    255   -  Play for infinite duration. */
#define BEEP_TONEA_DURATION                                0



/* TONEB CONTROL REGISTER */

/*! Initial ToneB Disable.
    0 -  ToneB Enabled.
    1 -  ToneB Disabled. */
#define BEEP_TONEB_DISABLE                                 0

/*! Initial ToneB Frequency.
    0-3   -  Rest Tone (no oscillation).
    4-127 -  Oscillate at 32kHz/freq Hz. */
#define BEEP_TONEB_FREQUENCY                               0

/*! Initial ToneB Duration.
    0-254 -  Play for 4ms*duration.
    255   -  Play for infinite duration. */
#define BEEP_TONEB_DURATION                                0



/************** Macro validation *****************************/

#if ( BEEP_CFG_INTERRUPT_ON_SEQUENCE_END > 1 )
#error "Invalid configuration"
#endif

#if ( BEEP_CFG_INTERRUPT_ON_SEQUENCE_NEAR_END > 1 )
#error "Invalid configuration"
#endif

#if ( BEEP_CFG_INTERRUPT_ON_TONEB_END > 1 )
#error "Invalid configuration"
#endif

#if ( BEEP_CFG_INTERRUPT_ON_TONEB_START > 1 )
#error "Invalid configuration"
#endif

#if ( BEEP_CFG_INTERRUPT_ON_TONEA_END > 1 )
#error "Invalid configuration"
#endif

#if ( BEEP_CFG_INTERRUPT_ON_TONEA_START > 1 )
#error "Invalid configuration"
#endif

#if ( BEEP_CFG_BEEPER_DISABLE > 1 )
#error "Invalid configuration"
#endif

#if ( BEEP_CFG_SEQUENCE_REPEAT_VALUE > 255 )
#error "Invalid configuration"
#endif

#if ( BEEP_STAT_CLEAR_SEQUENCE_END_INTERRUPT > 1 )
#error "Invalid configuration"
#endif

#if ( BEEP_STAT_CLEAR_SEQUENCE_NEAR_END_INTERRUPT > 1 )
#error "Invalid configuration"
#endif

#if ( BEEP_STAT_CLEAR_TONEB_END_INTERRUPT > 1 )
#error "Invalid configuration"
#endif

#if ( BEEP_STAT_CLEAR_TONEB_START_INTERRUPT > 1 )
#error "Invalid configuration"
#endif

#if ( BEEP_STAT_CLEAR_TONEA_END_INTERRUPT > 1 )
#error "Invalid configuration"
#endif

#if ( BEEP_STAT_CLEAR_TONEA_START_INTERRUPT > 1 )
#error "Invalid configuration"
#endif

#if ( BEEP_TONEA_DISABLE  > 1 )
#error "Invalid configuration"
#endif

#if ( BEEP_TONEA_FREQUENCY  > 127 )
#error "Invalid configuration"
#endif

#if ( BEEP_TONEA_DURATION > 255 )
#error "Invalid configuration"
#endif

#if ( BEEP_TONEB_DISABLE > 1 )
#error "Invalid configuration"
#endif

#if ( BEEP_TONEB_FREQUENCY > 127 )
#error "Invalid configuration"
#endif

#if ( BEEP_TONEB_DURATION  >  255 )
#error "Invalid configuration"
#endif

#endif /* __ADI_BEEP_CONIG_H__ */
