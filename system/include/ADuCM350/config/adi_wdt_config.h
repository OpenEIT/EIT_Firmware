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
   @file:    adi_wdt_config.h
   @brief:   Configuration options for WDT driver.
             This is specific to the WDT driver and will be included by the driver.
             It is not required for the application to include this header file.
   @version: $Revision: 28525 $
   @date:    $Date: 2014-11-12 14:51:26 -0500 (Wed, 12 Nov 2014) $
*****************************************************************************/

#ifndef __ADI_WDT_CONIG_H__
#define __ADI_WDT_CONIG_H__

/************* WDT Driver configurations ***************/


/************* WDT controller static configurations ***************/

/* NOTE: these valuse are not the defaults,
         they are tweaked to optimize the static
         config of the wdt_test example.
         Select suitable settings as needed...
*/

/*! WDT Timer Reload Value
    Value used to reload the WDT count register after count expires.
   0-65535 - WDT reload value (default is 0x0100).
*/
#define WDT_LOAD_VALUE                                0x1000

/*! WDT Timer Mode
   Selects WDT operating mode.
   0 -  WDT operates in free-running mode.
   1 -  WDT operates in periodic mode (default).
*/
#define WDT_CONTROL_TIMER_MODE                             1

/*! WDT Device Enable
   Controls WDT device enable.
   0 -  WDT device disabled.
   1 -  WDT device enabled (default).
*/
#define WDT_CONTROL_DEVICE_ENABLE                          0

/*! WDT Clock Prescaler
   Controls WDT clock prescale.
   0 -  WDT operates at (source clock)/1.
   1 -  WDT operates at (source clock)/16.
   2 -  WDT operates at (source clock)/256 (default).
   3 -  WDT operates at (source clock)/4096.
*/
#define WDT_CONTROL_CLOCK_PRESCALER                        0

/*! WDT Timeout Mode
   Controls WDT timeout behaivor.
   0 -  WDT issues RESET on timeout (default).
   1 -  WDT issues INTERRUPT on timeout
*/
#define WDT_CONTROL_TIMEOUT_MODE                           1

/*! WDT Hibernate Mode
   Controls WDT hibernation behaivor.
   0 -  WDT timer continues counting during hibernation.
   1 -  WDT timer suspends counting during hibernation (default).
*/
#define WDT_CONTROL_HIBERNATE_MODE                         1

/*! WDT Reset Mode
   Controls WDT reset behaivor.
   0 -  WDT resets on Power-On-Reset or System-Reset (default).
   1 -  WDT resets on only on Power-On-Reset.
*/
#define WDT_RESET_MODE                                     0


/************** Macro validation *****************************/

#if ( WDT_LOAD_VALUE  > 65535 )
#error "Invalid configuration"
#endif

#if ( WDT_CONTROL_TIMER_MODE > 1 )
#error "Invalid configuration"
#endif

#if ( WDT_CONTROL_DEVICE_ENABLE > 1 )
#error "Invalid configuration"
#endif

#if ( WDT_CONTROL_CLOCK_PRESCALER > 3 )
#error "Invalid configuration"
#endif

#if ( WDT_CONTROL_TIMEOUT_MODE > 1 )
#error "Invalid configuration"
#endif

#if ( WDT_CONTROL_HIBERNATE_MODE > 1 )
#error "Invalid configuration"
#endif

#if ( WDT_RESET_MODE > 1 )
#error "Invalid configuration"
#endif

#endif /* __ADI_WDT_CONIG_H__ */
