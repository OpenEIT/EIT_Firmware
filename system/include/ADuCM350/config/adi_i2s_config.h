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
   @file:    adi_i2s_config.h
   @brief:   Configuration options for I2S driver.
             This is specific to the I2S driver and will be included by the driver.
             It is not required for the application to include this header file.
   @version: $Revision: 28525 $
   @date:    $Date: 2014-11-12 14:51:26 -0500 (Wed, 12 Nov 2014) $
*****************************************************************************/

#ifndef __ADI_I2S_CONIG_H__
#define __ADI_I2S_CONIG_H__

/************* I2S Driver configurations ***************/

/*!
   Enable DMA support in the driver code.
   1 -  To have the DMA support code in the driver.
   0 -  To eliminate the DMA support.
*/
#define ADI_I2S_CFG_ENABLE_DMA_SUPPORT       1

#if ( ADI_I2S_CFG_ENABLE_DMA_SUPPORT > 1 )
#error "Invalid configuration"
#endif

/************* I2S controller configurations ***************/

/*! Drive unused bits on DATA line, it can be set to
    0 -  Don't drive unused bits.
    1 -  Drive unused bits. */
#define I2S0_CFG_DRIVE_UNUSED_BITS              0

/*! Master slave or slave mode operation, it can be set to
    1 - Master mode.
    0 - Slave mode. */
#define I2S0_CFG_MASTER_SLAVE_MODE              1

/*! Bit clocks per channel, it can be set to
    1 - 16 bit clocks.
    0 - 32 bit clocks. */
#define I2S0_CFG_BIT_CLOCKS_PER_CHANNEL         1

/*! MSB first or LSB first serial data output, it can be set to
    1 - LSB first.
    0 - MSB first. */
#define I2S0_CFG_LSB_FIRST_MODE                 0

/*! LR clock 50% duty cycle or single pulse in TDM Mode, it can be set to
    1 - Single cycle pulse LR clock mode.
    0 - 50% duty cycle LR clock mode. */
#define I2S0_CFG_LR_CLOCK_MODE                  0

/*! Data output width, it can be set to
    1 - 16 bit mode
    0 - 24 bit mode */
#define I2S0_CFG_DATA_OUTPUT_WIDTH              1

/*! TDM Slot width, it can be set to
    0 - 32 bit slot.
    1 - 24 bit slot.
    2 - 16 bit slot. */
#define I2S0_CFG_DATA_SLOT_WIDTH                1

/*! Frame Rate, it can be set to
    0 - 8Khz (PCLK/2048).
    1 - 16Khz (PCLK/1024).
    2 - 32Khz (PCLK/512).
    3 - 64Khz (PCLK/256).
    4 - 128Khz (PCLK/128).
*/
#define I2S0_CFG_FRAME_RATE                     0

/*! Channel format, it can be set to
    0 - Stereo mode operation.
    1 - TDM2 Mode operation.
    2 - TDM4 Mode operation.
    3 - TDM8 Mode operation.
    4 - TDM16 Mode operation.
    5 - Mono/PCM Mode operation.*/
#define I2S0_CFG_CHNL_FORMAT                    4

/*! Serial Data format, it can be set to
    0 - I2S default serial data format.
    1 - Left justified serial data format.
    2 - 24 bit justified serial data format.
    3 - 16 bit right justfied format. */
#define I2S0_CFG_SERIAL_DATA_FORMAT             0

/*! Map Channel 1 to TDM slot. It can be any value from 0 to 15. */
#define I2S0_CFG_TDM_SLOT_CHANNEL1_MAP          0

/*! Map Channel 2 to TDM slot. It can be any value from 0 to 15. */
#define I2S0_CFG_TDM_SLOT_CHANNEL2_MAP          1

/*! Drive channel 1 on Serial data line, it can be set to
    1 -  Drive channel 1 data.
    0 -  Do not drive channel 1 data. */
#define I2S0_CFG_DRIVE_CHANNEL1_DATA            1

/*! Drive channel 2 on Serial data line, it can be set to
    1 - Drive channel 1 data.
    0 - Do not drive channel 1 data. */
#define I2S0_CFG_DRIVE_CHANNEL2_DATA            1

/*! Sampling edge for bit clock, it can be set to
    1 - Sample on rising edge of bit clock.
    0 - Sample on rising edge of bit clock. */
#define I2S0_CFG_BIT_CLK_SAMPLING_EDGE          0

/*! Invert LR Clock polarity, it can be set to
    1 - Invert LR clock.
    0 - Do not invert LR Clock. */
#define I2S0_CFG_INVERT_LR_CLOCK                0

/*! Enable Channel 1 data to be transmitted on APB bus, it can be set to
    1 - Enable channel 1 data transmit on APB bus.
    0 - Disable channel 1 data transmit on APB bus. */
#define I2S0_CFG_ENABLE_CHANNEL1_ON_APB_BUS     1

/*! Enable Channel 2 data to be transmitted on APB bus, it can be set to
    1 - Enable channel 2 data transmit on APB bus.
    0 - Disable channel 2 data trasmit on APB bus. */
#define I2S0_CFG_ENABLE_CHANNEL2_ON_APB_BUS     1

/*! Transfer mode, it can be set to
    1 - 16 bit transfer mode.
    0 - 24 bit transfer mode. */
#define I2S0_CFG_TRANSFER_MODE                  1

/*! Enable FIFO status interrupt, it can be set to
    1 - Enable fifo status interrupt.
    0 - Disable fifo status. */
#define I2S0_CFG_ENABLE_FIFO_STATUS_INTERRUPT   0

/*! Enable data request interrupt, it can be set to.
    1 - Enable data request interrupt.
    0 - Disable data request interrupt. */
#define I2S0_CFG_ENABLE_DATA_REQ_INTERRUPT      1

/*! Reset auto increment address, it can be set to
    1 - Reset auto increment address.
    0 - Do not reset auto increment address. */
#define I2S0_CFG_RESET_INCREMENT_ADDRESS        0

/*! Reset FIFO pointers, it can be set to
    1 - Reset FIFO pointers.
    0 - Do not reset FIFO pointers. */
#define I2S0_CFG_RESET_FIFO_POINTERS            0

/*! Serial output data sampling frequency, it can be set to
    0 - Write to serial output every frame.
    1 - Write to serial output every 4 frames.
    2 - Write to serial output every 8 frames.
    3 - Write to serial output every 16 frames.
*/
#define I2S0_CFG_OUTPUT_DATA_SAMPLING_FREQ      0

/*! DMA request sampling frequency, it can be set to
    0 - Send DMA request when there is at least 1 available space in FIFO.
    1 - Send DMA request when there is at least 4 available spaces in FIFO.
    2 - Send DMA request when there is at least 8 available spaces in FIFO.
*/
#define I2S0_CFG_DMA_REQ_SAMPLING_FREQ          0

/*! Data request interrupt sampling frequency, it can be set to
    0 - Send interrupt for serial data request every frame or every other frame if 8-bit transfer mode.
    1 - Send interrupt for serial data request every 4 frames.
    2 - Send interrupt for serial data request every 8 frames.
    3 - Send interrupt for serial data request every 16 frames.
*/
#define I2S0_CFG_DATA_REQ_SAMPLING_FREQ         0


/************** Macro validation *****************************/

#if ( I2S0_CFG_DRIVE_UNUSED_BITS > 1 )
#error "Invalid configuration"
#endif

#if ( I2S0_CFG_MASTER_SLAVE_MODE > 1 )
#error "Invalid configuration"
#endif

#if ( I2S0_CFG_BIT_CLOCKS_PER_CHANNEL > 1 )
#error "Invalid configuration"
#endif

#if ( I2S0_CFG_LSB_FIRST_MODE > 1 )
#error "Invalid configuration"
#endif

#if ( I2S0_CFG_LR_CLOCK_MODE > 1 )
#error "Invalid configuration"
#endif

#if ( I2S0_CFG_DATA_OUTPUT_WIDTH > 1 )
#error "Invalid configuration"
#endif

#if ( I2S0_CFG_DATA_SLOT_WIDTH > 2 )
#error "Invalid configuration"
#endif

#if ( I2S0_CFG_FRAME_RATE  > 4 )
#error "Invalid configuration"
#endif

#if ( I2S0_CFG_CHNL_FORMAT  > 5 )
#error "Invalid configuration"
#endif

#if ( I2S0_CFG_SERIAL_DATA_FORMAT > 3 )
#error "Invalid configuration"
#endif

#if ( I2S0_CFG_TDM_SLOT_CHANNEL1_MAP > 15 )
#error "Invalid configuration"
#endif

#if ( I2S0_CFG_TDM_SLOT_CHANNEL2_MAP > 15 )
#error "Invalid configuration"
#endif

#if ( I2S0_CFG_DRIVE_CHANNEL1_DATA  >  1 )
#error "Invalid configuration"
#endif

#if ( I2S0_CFG_DRIVE_CHANNEL2_DATA  > 1 )
#error "Invalid configuration"
#endif

#if ( I2S0_CFG_BIT_CLK_SAMPLING_EDGE > 1 )
#error "Invalid configuration"
#endif

#if ( I2S0_CFG_INVERT_LR_CLOCK  > 1)
#error "Invalid configuration"
#endif

#if ( I2S0_CFG_ENABLE_CHANNEL1_ON_APB_BUS  > 1 )
#error "Invalid configuration"
#endif

#if ( I2S0_CFG_ENABLE_CHANNEL2_ON_APB_BUS  >  1 )
#error "Invalid configuration"
#endif

#if ( I2S0_CFG_TRANSFER_MODE  > 1 )
#error "Invalid configuration"
#endif

#if ( I2S0_CFG_ENABLE_AUTO_INCREMENT  > 1 )
#error "Invalid configuration"
#endif

#if ( I2S0_CFG_ENABLE_FIFO_STATUS_INTERRUPT > 1 )
#error "Invalid configuration"
#endif

#if ( I2S0_CFG_ENABLE_DATA_REQ_INTERRUPT  > 1 )
#error "Invalid configuration"
#endif

#if ( I2S0_CFG_ENABLE_DMA_REQUEST  > 1 )
#error "Invalid configuration"
#endif

#if ( I2S0_CFG_RESET_INCREMENT_ADDRESS  > 1 )
#error "Invalid configuration"
#endif

#if ( I2S0_CFG_RESET_FIFO_POINTERS  > 1 )
#error "Invalid configuration"
#endif

#if ( I2S0_CFG_OUTPUT_DATA_SAMPLING_FREQ  > 3 )
#error "Invalid configuration"
#endif

#if ( I2S0_CFG_DMA_REQ_SAMPLING_FREQ  > 2 )
#error "Invalid configuration"
#endif

#if ( I2S0_CFG_DATA_REQ_SAMPLING_FREQ > 3 )
#error "Invalid configuration"
#endif

#endif /* __ADI_I2S_CONIG_H__ */
