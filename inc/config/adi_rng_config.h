/*********************************************************************************

Copyright (c) 2012-2014 Analog Devices, Inc. All Rights Reserved.

This software is proprietary to Analog Devices, Inc. and its licensors.  By using 
this software you agree to the terms of the associated Analog Devices Software 
License Agreement.

*********************************************************************************/

/*!
 *****************************************************************************
   @file:    adi_rng_config.h
   @brief:   Configuration options for RNG driver.
             This is specific to the RNG driver and will be included by the driver.
             It is not required for the application to include this header file.
   @version: $Revision: 28525 $
   @date:    $Date: 2014-11-12 14:51:26 -0500 (Wed, 12 Nov 2014) $
*****************************************************************************/

#ifndef __ADI_RNG_CONIG_H__
#define __ADI_RNG_CONIG_H__

/************* RNG Driver configurations ***************/

/************* RNG controller configurations ***************/

/*! RNG Control Register, bit 1
    Enable the Oscillator Counter
    0 -  Disable Oscillator Counter.
    1 -  Enable Oscillator Counter. */
#define RNG0_CFG_OSC_COUNTER                            0

/*! RNG Control Register, bit 2
    Enable the timer mode or RNG mode
    0 -  Enable RNG mode.
    1 -  Enable Timer mode. */
#define RTC0_CFG_RNG_TIMER_MODE                         0

/*! RTC Sample Length Register, bits [11:0]
    The register defines the number of samples to accumulate in the
    CRC register when generating a random number.

    Bits [11:0] contains the reload value of the sample counter

    */
#define RTC0_CFG_LENGTH_RELOAD                          0

/*! RTC Sample Length Register, bits [15:12]
    The register defines the number of samples to accumulate in the
    CRC register when generating a random number. The number of values
    accumulated in the counter reload value is scaled by 2^prescaler.

    Bits [15:12] contains the prescaler for the sample counter

    */
#define RTC0_CFG_LENGTH_PRESCALER                       0

/************** Macro validation *****************************/

#if ( RNG0_CFG_OSC_COUNTER > 1 )
#error "Invalid configuration"
#endif

#if ( RTC0_CFG_RNG_TIMER_MODE  > 1 )
#error "Invalid configuration"
#endif

#if ( RTC0_CFG_LENGTH_RELOAD > 4095 )
#error "Invalid configuration"
#endif

#if ( RTC0_CFG_LENGTH_PRESCALER > 15 )
#error "Invalid configuration"
#endif

#endif /* __ADI_RNG_CONIG_H__ */
