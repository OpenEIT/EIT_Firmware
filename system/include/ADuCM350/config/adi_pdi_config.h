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
   @file:    adi_pdi_config.h
   @brief:   Configuration options for PDI driver.
             This is specific to the PDI driver and will be included by the driver.
             It is not required for the application to include this header file.
   @version: $Revision: 28525 $
   @date:    $Date: 2014-11-12 14:51:26 -0500 (Wed, 12 Nov 2014) $
*****************************************************************************/

#ifndef __ADI_PDI_CONIG_H__
#define __ADI_PDI_CONIG_H__

/************* PDI Driver configurations ***************/


/************* PDI controller static configurations ***************/


/*
	The following PDI control registers are not configured statically because they have dynamic content:
		- configuration
		- command
		- frame data count
		- parameter FIFO
*/


/* INTERRUPT ENABLE */
/*!
   Interrupt enables.
   0 -  The interrupt enable status is not changed.
   1 -  The interrupt is enabled.
*/
#define PDI_INTERRUPT_ENABLE_FIFO_UNDERFLOW                0
#define PDI_INTERRUPT_ENABLE_FIFO_OVERLFLOW                0
#define PDI_INTERRUPT_ENABLE_CMD_WRITE_ERROR               0
#define PDI_INTERRUPT_ENABLE_CMD_DONE                      1
#define PDI_INTERRUPT_ENABLE_FIFO_DONE                     0
#define PDI_INTERRUPT_ENABLE_TEAR_EFFECT                   0
#define PDI_INTERRUPT_ENABLE_RECEIVE                       0
#define PDI_INTERRUPT_ENABLE_TRANSMIT                      0


/* INTERRUPT DISABLE */
/*!
   Interrupt disables.  Defaulted to the negation of the enable set.
   0 -  The interrupt enable status is not changed.
   1 -  The interrupt is disabled.
*/
#define PDI_INTERRUPT_DISABLE_FIFO_UNDERFLOW               !PDI_INTERRUPT_ENABLE_FIFO_UNDERFLOW
#define PDI_INTERRUPT_DISABLE_FIFO_OVERLFLOW               !PDI_INTERRUPT_ENABLE_FIFO_OVERLFLOW
#define PDI_INTERRUPT_DISABLE_CMD_WRITE_ERROR              !PDI_INTERRUPT_ENABLE_CMD_WRITE_ERROR
#define PDI_INTERRUPT_DISABLE_CMD_DONE                     !PDI_INTERRUPT_ENABLE_CMD_DONE
#define PDI_INTERRUPT_DISABLE_FIFO_DONE                    !PDI_INTERRUPT_ENABLE_FIFO_DONE
#define PDI_INTERRUPT_DISABLE_TEAR_EFFECT                  !PDI_INTERRUPT_ENABLE_TEAR_EFFECT
#define PDI_INTERRUPT_DISABLE_RECEIVE                      !PDI_INTERRUPT_ENABLE_RECEIVE
#define PDI_INTERRUPT_DISABLE_TRANSMIT                     !PDI_INTERRUPT_ENABLE_TRANSMIT


/* STATUS INTERRUPT CLEAR */
/*!
   Status register interrupt clears.
   0 -  The interrupt status is not changed.
   1 -  The interrupt status is cleared.
*/
#define PDI_INTERRUPT_CLEAR_FIFO_UNDERFLOW                 1
#define PDI_INTERRUPT_CLEAR_FIFO_OVERLFLOW                 1
#define PDI_INTERRUPT_CLEAR_CMD_WRITE_ERROR                1
#define PDI_INTERRUPT_CLEAR_CMD_DONE                       1
#define PDI_INTERRUPT_CLEAR_FIFO_DONE                      1
#define PDI_INTERRUPT_CLEAR_TEAR_EFFECT                    1


/* INTERFACE TIMING */
/*!
   Chip select setup time.
   0    -  Illegal value.
   1-15 -  Chip select setup time (in PCLK cycles) for Type-C interface.
*/
#define PDI_TIMING_CHIP_SELECT_SETUP_TIME                  1

/*!
   Read/Write LOW pulse duration.
   0     -  Illegal value.
   1-255 -  Low duration (in PCLK cycles) for  of the read/write control pulse.
*/
#define PDI_TIMING_READ_WRITE_CONTROL_LOW_PULSE            1

/*!
   Read/Write HIGH pulse duration.
   0     -  Illegal value.
   1-255 -  High duration (in PCLK cycles) for  of the read/write control pulse.
*/
#define PDI_TIMING_READ_WRITE_CONTROL_HIGH_PULSE           1

/*!
   Chip select hold time.
   0    -  Illegal value.
   1-15 -  Chip select hold time (in PCLK cycles) for Type-B interface.
*/
#define PDI_TIMING_CHIP_SELECT_HOLD_TIME                   1

/*!
   Address setup and hold time.
   0    -  Illegal value.
   1-15 -  Address setup/hold hold time (in PCLK cycles).
*/
#define PDI_TIMING_ADDRESS_SETUP_HOLD_TIME                 1



/************** Macro validation *****************************/

#if ( PDI_INTERRUPT_ENABLE_FIFO_UNDERFLOW  > 1 )
#error "Invalid configuration"
#endif

#if ( PDI_INTERRUPT_ENABLE_FIFO_OVERLFLOW > 1 )
#error "Invalid configuration"
#endif

#if ( PDI_INTERRUPT_ENABLE_CMD_WRITE_ERROR > 1 )
#error "Invalid configuration"
#endif

#if ( PDI_INTERRUPT_ENABLE_CMD_DONE > 1 )
#error "Invalid configuration"
#endif

#if ( PDI_INTERRUPT_ENABLE_FIFO_DONE > 1 )
#error "Invalid configuration"
#endif

#if ( PDI_INTERRUPT_ENABLE_TEAR_EFFECT > 1 )
#error "Invalid configuration"
#endif

#if ( PDI_INTERRUPT_ENABLE_RECEIVE > 1 )
#error "Invalid configuration"
#endif

#if ( PDI_INTERRUPT_ENABLE_TRANSMIT > 1 )
#error "Invalid configuration"
#endif

#if ( PDI_INTERRUPT_DISABLE_FIFO_UNDERFLOW  > 1 )
#error "Invalid configuration"
#endif

#if ( PDI_INTERRUPT_DISABLE_FIFO_OVERLFLOW > 1 )
#error "Invalid configuration"
#endif

#if ( PDI_INTERRUPT_DISABLE_CMD_WRITE_ERROR > 1 )
#error "Invalid configuration"
#endif

#if ( PDI_INTERRUPT_DISABLE_CMD_DONE > 1 )
#error "Invalid configuration"
#endif

#if ( PDI_INTERRUPT_DISABLE_FIFO_DONE > 1 )
#error "Invalid configuration"
#endif

#if ( PDI_INTERRUPT_DISABLE_TEAR_EFFECT > 1 )
#error "Invalid configuration"
#endif

#if ( PDI_INTERRUPT_DISABLE_RECEIVE > 1 )
#error "Invalid configuration"
#endif

#if ( PDI_INTERRUPT_DISABLE_TRANSMIT > 1 )
#error "Invalid configuration"
#endif

#if ( PDI_INTERRUPT_CLEAR_FIFO_UNDERFLOW  > 1 )
#error "Invalid configuration"
#endif

#if ( PDI_INTERRUPT_CLEAR_FIFO_OVERLFLOW > 1 )
#error "Invalid configuration"
#endif

#if ( PDI_INTERRUPT_CLEAR_CMD_WRITE_ERROR > 1 )
#error "Invalid configuration"
#endif

#if ( PDI_INTERRUPT_CLEAR_CMD_DONE > 1 )
#error "Invalid configuration"
#endif

#if ( PDI_INTERRUPT_CLEAR_FIFO_DONE > 1 )
#error "Invalid configuration"
#endif

#if ( PDI_INTERRUPT_CLEAR_TEAR_EFFECT > 1 )
#error "Invalid configuration"
#endif

#if ( (PDI_TIMING_CHIP_SELECT_SETUP_TIME == 0)        || (PDI_TIMING_CHIP_SELECT_SETUP_TIME > 15) )
#error "Invalid configuration"
#endif

#if ( (PDI_TIMING_READ_WRITE_CONTROL_LOW_PULSE == 0)  || (PDI_TIMING_READ_WRITE_CONTROL_LOW_PULSE > 255) )
#error "Invalid configuration"
#endif

#if ( (PDI_TIMING_READ_WRITE_CONTROL_HIGH_PULSE == 0) || (PDI_TIMING_READ_WRITE_CONTROL_HIGH_PULSE > 255) )
#error "Invalid configuration"
#endif

#if ( (PDI_TIMING_CHIP_SELECT_HOLD_TIME == 0)         || (PDI_TIMING_CHIP_SELECT_HOLD_TIME > 15) )
#error "Invalid configuration"
#endif

#if ( (PDI_TIMING_ADDRESS_SETUP_HOLD_TIME == 0)       || (PDI_TIMING_ADDRESS_SETUP_HOLD_TIME > 15) )
#error "Invalid configuration"
#endif

#endif /* __ADI_PDI_CONIG_H__ */
