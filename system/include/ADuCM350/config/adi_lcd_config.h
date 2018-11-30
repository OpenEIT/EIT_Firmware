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
   @file:    adi_lcd_config.h

   @brief:   Configuration options LCD.

             The configurations will be used to intialize the LCD registers
             during initialization. This will reduce the code footprint and
             cycles for the application as it is not required to call the
             driver APIs to configure the LCD. The LCD controller will be
             initialized with the required configuration.

   @version: $Revision: 28525 $
   @date:    $Date: 2014-11-12 14:51:26 -0500 (Wed, 12 Nov 2014) $
*****************************************************************************/

/*! \addtogroup LCD_Driver LCD Driver
 *  @{
 */

#ifndef __ADI_LCD_CONFIG_H__
#define __ADI_LCD_CONFIG_H__


/************** LCD Driver configurations *****************/

/************** LCD controller configurations ************/

/*! Static or dynamic muxing, it can be set to
    0 - Static (only COM0 is used).
    1 - 4x multiplexing (COM3: COM0 are used)
*/
#define LCD0_CFG_MUX_TYPE                       1

/*! Screen select, it can be set to
    0 - Select screen 0 for display.
    1 - Select screen 1 for display.
*/
#define LCD0_CFG_SCREEN_SELECT                  0

/*! LCD refresh frame rate, it can be set to
    0  - Frame rate 128 Hz.
    1  - Frame rate 102.4 Hz.
    2  - Frame rate 85.3 Hz.
    3  - Frame rate 73.1 Hz.
    4  - Frame rate 64 Hz.
    5  - Frame rate 56.9 Hz.
    6  - Frame rate 51.2 Hz.
    7  - Frame rate 46.5 Hz.
    8  - Frame rate 42.7 Hz.
    9  - Frame rate 39.4 Hz.
    10 - Frame rate 36.6 Hz.
    11 - Frame rate 34.1 Hz.
    12 - Frame rate 32 Hz.
    13 - Frame rate 30.1 Hz.
    14 - Frame rate 28.4 Hz.
    15 - Frame rate 26.9 Hz.
*/
#define LCD0_CFG_FRAME_RATE                     3


/*! Enable frame inversion mode, it can be set to
    0 - Disable frame inversion. Frames are not inverted.
    1 - Enable frame inversion. Frames are inverted at
        each frame boundary.
*/
#define LCD0_CFG_FRAME_INVERT                   0

/*! Reset LCD data registers, it can be set to
   0 - Do not reset LCD data registers.
   1 - Reset LCD data registers.
*/
#define LCD0_CFG_DATA_RESET                     1

/*! Enable frame boundary interrupt, it can be set to
    0 - Disable frame boundary interrupt.
    1 - Enable frame boundary interrupt.
*/
#define LCD0_CFG_ENABLE_FRAME_BOUNARY_INT       0

/*! Enable Charge pump interrupt, it can be set to
    0 - No interrupt is generated when charge pump goes low.
    1 - Interrupt is generated when charge pump goes low.
*/
#define LCD0_CFG_ENABLE_CHARGE_PUMP_INT         1

/*! Enable LCD blink, it can be set to
    0 - Disable LCD blink.
    1 - Enable LCD blink.
*/
#define LCD0_CFG_ENABLE_BLINK                   0


/*! LCD blink mode, it can be set to
    0 - Blink rate controlled by software;
        display is off or the alternate screen
        is displayed.
    1 - Blink rate controlled by software;
        display is on and the selected screen
        is displayed
    3 - Blink rate controlled by hardware.
        Blink rate is set by LCDx_CFG_BLINK_FREQ.
*/
#define LCD0_CFG_BLINK_MODE                     0


/*! LCD blink frequency, this will be in effect only
    when hardware controlled blink mode is selected in
    LCDx_CFG_BLINK_MODE configuration. It can be set to
    0 - 4 Hhz.
    1 - 2 Hhz.
    2 - 1 Hhz.
    3 - 2/3 Hhz.
    4 - 1/2 Hhz.
*/
#define LCD0_CFG_BLINK_FREQ                     0


/*! Enable auto switch screen, it will be in effect only
    when LCD blink is enabled by setting LCDx_CFG_BLINK_MODE
    to 1. It can be set to
    0 - LCD will switch between ON and OFF states.
    1 - LCD will switch between screens 0 and 1.
*/
#define LCD0_CFG_AUTOSWITCH                     0


/*! LCD contrast level can be changed by setting the
    bias level, it can be set to any value between
    0 to 31. The LCD Bias voltage for given bias value
    is given by the following equation:

    Bias Voltage = 2.4 V + (0.0387) x BIAS_LEVEL
*/
#define LCD0_CFG_BIAS_LEVEL                     15



/*! Enable charge pump, it can be set to
    0 - Disable charge pump.
    1 - Enable charge pump.
*/
#define LCD0_CFG_ENABLE_CHARGE_PUMP             1


/*! Power down charge pump, it can be set to
    0 - Charge pump is not powered down.
    1 - Charge pump is powered down.
*/
#define LCD0_CFG_POWER_DOWN_CHARGE_PUMP         0


/******************* Macro Validation ************************/

#if ( LCD0_CFG_MUX_TYPE > 1 )
#error "Invalid configuration"
#endif

#if ( LCD0_CFG_SCREEN_SELECT > 1 )
#error "Invalid configuration"
#endif

#if ( LCD0_CFG_FRAME_RATE  > 15 )
#error "Invalid configuration"
#endif

#if ( LCD0_CFG_FRAME_INVERT > 1 )
#error "Invalid configuration"
#endif

#if ( LCD0_CFG_DATA_RESET > 1 )
#error "Invalid configuration"
#endif

#if ( LCD0_CFG_ENABLE_FRAME_BOUNARY_INT  > 1 )
#error "Invalid configuration"
#endif

#if ( LCD0_CFG_ENABLE_CHARGE_PUMP_INT > 1 )
#error "Invalid configuration"
#endif

#if ( LCD0_CFG_ENABLE_BLINK  > 1 )
#error "Invalid configuration"
#endif

#if ( LCD0_CFG_BLINK_MODE  > 3 )
#error "Invalid configuration"
#endif

#if ( LCD0_CFG_BLINK_FREQ > 4 )
#error "Invalid configuration"
#endif

#if ( LCD0_CFG_AUTOSWITCH > 1 )
#error "Invalid configuration"
#endif


#if ( LCD0_CFG_BIAS_LEVEL > 31 )
#error "Invalid configuration"
#endif

#if ( LCD0_CFG_ENABLE_CHARGE_PUMP  > 1 )
#error "Invalid configuration"
#endif

#if ( LCD0_CFG_POWER_DOWN_CHARGE_PUMP  > 1 )
#error "Invalid configuration"
#endif

#endif /* __ADI_LCD_CONFIG_H__ */

/*@}*/
