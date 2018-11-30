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
   @file:    adi_uart_config.h
   @brief:   Configuration options for UART driver.
   @version: $Revision: 28544 $
   @date:    $Date: 2014-11-13 13:09:44 -0500 (Thu, 13 Nov 2014) $
*****************************************************************************/

#ifndef __ADI_UART_CONFIG__
#define __ADI_UART_CONFIG__

#define ADI_UART_CFG_INTERRUPT_MODE_SUPPORT     1 /*!< Enable interrupt mode*/
#define ADI_UART_CFG_POLLED_MODE_SUPPORT        1 /*!< Enable polled mode*/
#define ADI_UART_CFG_BLOCKING_MODE_SUPPORT      1 /*!< Enable blocking mode*/
#define ADI_UART_CFG_NONBLOCKING_MODE_SUPPORT   1 /*!< Enable non-blocking mode*/

#define ADI_UART0_BAUD_INITIALIZER    ADI_UART_BAUD_9600              /*!< UART0 Baudrate Divider Register initializer */
#define ADI_UART0_COMLCR_INITIALIZER  COMLCR_STOP | COMLCR_WLS_8BITS  /*!< UART0 Line Control Register initializer */
#define ADI_UART0_COMMCR_INITIALIZER  0                               /*!< UART0 Modem Control Register initializer */

#endif /* __ADI_UART_CONFIG__ */
