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
   @file:    adi_spi_config.h
   @brief:   Configuration options for SPI driver.
             This is specific to the SPI driver and will be included by the driver.
             It is not required for the application to include this header file.
   @version: $Revision: 28525 $
   @date:    $Date: 2014-11-12 14:51:26 -0500 (Wed, 12 Nov 2014) $
*****************************************************************************/

#ifndef __ADI_SPI_CONIG_H__
#define __ADI_SPI_CONIG_H__

/*! Set this macro to the system clock frequency in hertz*/
#define ADI_CFG_SYSTEM_CLOCK_HZ                    100000000

/************* SPI Driver configurations ***************/

/*!
   Enable DMA support in the driver code.
   1 -  To have the DMA support code in the driver.
   0 -  To eliminate the DMA support.
*/
#define ADI_SPI_CFG_ENABLE_DMA_SUPPORT                     1

/*!
   Set this macro to 1 for adding the interrupt support code to the driver.
   To eliminate the interrupt support, set the macro to 0.
*/
#define ADI_SPI_CFG_INTERRUPT_MODE_SUPPORT                 1

/*!
   Set this macro to 1 for adding the SPI master mode code to the driver.
   To eliminate the SPI master mode, set the macro to 0.
*/
#define ADI_SPI_CFG_MASTER_MODE_SUPPORT                    1

/*!
   Set this macro to 1 for adding the SPI slave mode code to the driver.
   To eliminate the SPI slave mode, set the macro to 0.
*/
#define ADI_SPI_CFG_SLAVE_MODE_SUPPORT                     1

/*!
   Set this macro to 1 for to enable static controller initializations
   during the driver initialization routing.
   To eliminate static driver configuration, set this macro to 0.
*/
#define ADI_SPI_CFG_ENABLE_STATIC_CONFIG_SUPPORT           0



#if (ADI_SPI_CFG_ENABLE_STATIC_CONFIG_SUPPORT == 1)

/************* SPI controller configurations ***************/

/* There are three SPI instances SPI0, SPI1 and SPIH       */
/*! Set this macro to the SPI bit rate in hertz */
#define ADI_SPI0_CFG_BIT_RATE_HZ               8000000
#define ADI_SPI1_CFG_BIT_RATE_HZ               8000000
#define ADI_SPIH_CFG_BIT_RATE_HZ               8000000

/*! SPI enable
    SPI configuration register: Bit[0]
    1 -  Enable SPI
    0 -  Disable SPI */
#define SPI0_CFG_ENABLE                         0
#define SPI1_CFG_ENABLE                         0
#define SPIH_CFG_ENABLE                         0

/*! Master slave or slave mode operation
    SPI configuration register: Bit[1]
    1 - Master mode.
    0 - Slave mode. */
#define SPI0_CFG_MASTER_SLAVE                   1
#define SPI1_CFG_MASTER_SLAVE                   1
#define SPIH_CFG_MASTER_SLAVE                   1

/*! SPI clock phase mode
    SPI configuration register: Bit[2]
    1 - Serial clock pulses at the beginning of each serial bit transfer.
    0 - Serial clock pulses at the end of each serial bit transfer. */
#define SPI0_CFG_CLK_PHASE                      0
#define SPI1_CFG_CLK_PHASE                      0
#define SPIH_CFG_CLK_PHASE                      0

/*! SPI clock polarity
    SPI configuration register: Bit[3]
    1 - Serial clock idles high.
    0 - Serial clock idles low. */
#define SPI0_CFG_CLK_POLARITY                   0
#define SPI1_CFG_CLK_POLARITY                   0
#define SPIH_CFG_CLK_POLARITY                   0

/*! SPI wired OR mode
    SPI configuration register: Bit[4]
    1 - Enables open circuit output enable.
    0 - Normal output levels. */
#define SPI0_CFG_WIRED_OR                       0
#define SPI1_CFG_WIRED_OR                       0
#define SPIH_CFG_WIRED_OR                       0

/*! SPI LSB/MSB
    SPI configuration register: Bit[5]
    1 - MSB transmitted first.
    0 - LSB transmitted first. */
#define SPI0_CFG_LSB_MSB                        0
#define SPI1_CFG_LSB_MSB                        0
#define SPIH_CFG_LSB_MSB                        0

/*! SPI transfer initiate
    SPI configuration register: Bit[6]
    1 - SPI transfer is initiated with write to Tx FIFO register. Interrupts when Tx is empty.
    0 - SPI transfer is initiated with a read of the Rx FIFO register. Interrupts when Rx is full.*/
#define SPI0_CFG_TRANSFER_INITIATE              0
#define SPI1_CFG_TRANSFER_INITIATE              0
#define SPIH_CFG_TRANSFER_INITIATE              0

/*! SPI Tx FIFO transfers zeros or last bit upon underflow
    SPI configuration register: Bit[7]
    1 - Tx FIFO sends zeros upon undeflow.
    0 - Tx FIFO repeats last bit upon undeflow. */
#define SPI0_CFG_TX_UNDERFLOW                   0
#define SPI1_CFG_TX_UNDERFLOW                   0
#define SPIH_CFG_TX_UNDERFLOW                   0

/*! SPI Rx FIFO overflows with received data or data is discarded
    SPI configuration register: Bit[8]
    1 - Rx FIFO receives data upon overflow.
    0 - Rx FIFO discards received data upon overflow. */
#define SPI0_CFG_RX_OVERFLOW                    0
#define SPI1_CFG_RX_OVERFLOW                    0
#define SPIH_CFG_RX_OVERFLOW                    0

/*! SPI slave mode MISO enable
    SPI configuration register: Bit[9]
    1 - MISO operates as normal in slave mode.
    0 - MISO is disabled in slave mode. */
#define SPI0_CFG_MISO_ENABLE                    0
#define SPI1_CFG_MISO_ENABLE                    0
#define SPIH_CFG_MISO_ENABLE                    0

/*! SPI internal loopback enable
    SPI configuration register: Bit[10]
    1 - MISO and MOSI is loopbacked internally.
    0 - MISO and MOSI operates normally. */
#define SPI0_CFG_LOOPBACK                       0
#define SPI1_CFG_LOOPBACK                       0
#define SPIH_CFG_LOOPBACK                       0

/*! SPI transfer and interrupt mode
    SPI configuration register: Bit[11]
    1 - SPI continous transfers in which CS remains asserted until Tx is empty.
    0 - SPI disable continuous transfer, each transfer consistes of 8 bits of data.*/
#define SPI0_CFG_CONTINUOUS                     0
#define SPI1_CFG_CONTINUOUS                     0
#define SPIH_CFG_CONTINUOUS                     0

/*! SPI Rx FIFO flush enable
    SPI configuration register: Bit[12]
    1 - Rx FIFO is flushed and all rx data is ignored and no interrupts are generated.
    0 - Rx FIFO flush is disabled. */
#define SPI0_CFG_RX_FLUSH                       0
#define SPI1_CFG_RX_FLUSH                       0
#define SPIH_CFG_RX_FLUSH                       0

/*! SPI Tx FIFO flush enable
    SPI configuration register: Bit[13]
    1 - Tx FIFO is flushed.
    0 - Tx FIFO flush is disabled. */
#define SPI0_CFG_TX_FLUSH                       0
#define SPI1_CFG_TX_FLUSH                       0
#define SPIH_CFG_TX_FLUSH                       0

/*! SPI IRQ mode
    SPI configuration register: Bit[14:15]
    0 - Tx interrupt occurs when 1 byte  has  been transmitted.
      - Rx interrupt occurs when 1 byte  has  been received.
    1 - Tx interrupt occurs when 2 bytes have been transmitted.
      - Rx interrupt occurs when 2 bytes have been received.
    2 - Tx interrupt occurs when 3 bytes have been transmitted.
      - Rx interrupt occurs when 3 bytes have been received.
    3 - Tx interrupt occurs when 4 bytes have been transmitted.
      - Rx interrupt occurs when 4 bytes have been received. */
#define SPI0_CFG_IRQ_MODE                       0
#define SPI1_CFG_IRQ_MODE                       0
#define SPIH_CFG_IRQ_MODE                       0

/*! SPI clock divide
    SPI baud rate selection register: Bit[0:5]
    Value between 0-63 that is used to divide the UCLK to generate
    the SPI serial clock. */
#define SPI0_CFG_CLK_DIV                        0
#define SPI1_CFG_CLK_DIV                        0
#define SPIH_CFG_CLK_DIV                        0

/*! SPI high frequency mode
    SPI baud rate selection register: Bit[6]
    1 - High frequency mode enabled.
    0 - High frequency mode disabled. */
#define SPI0_CFG_HFM                            0
#define SPI1_CFG_HFM                            0
#define SPIH_CFG_HFM                            0

/*! SPI reset mode for CSERR
    SPI baud rate selection register: Bit[7]
    1 - clear bit counter on CS error.
    0 - do not clear bit counter on CS error. */
#define SPI0_CFG_CS_ERR                         0
#define SPI1_CFG_CS_ERR                         0
#define SPIH_CFG_CS_ERR                         0

/*! SPI CS interrupt
    SPI baud rate selection register: Bit[8]
    1 - In continuous mode, generate interrupt on CS.
    0 - In continuous mode, do not generate interrupt on CS. */
#define SPI0_CFG_CS_IRQ                         0
#define SPI1_CFG_CS_IRQ                         0
#define SPIH_CFG_CS_IRQ                         0

/************** Macro validation *****************************/

#if ( ADI_SPI0_CFG_BIT_RATE_HZ > 8000000 ) || \
    ( ADI_SPI1_CFG_BIT_RATE_HZ > 8000000 ) || \
    ( ADI_SPIH_CFG_BIT_RATE_HZ > 8000000 )
#error "Invalid configuration"
#endif

#if ( SPI0_CFG_ENABLE > 1 ) || \
    ( SPI1_CFG_ENABLE > 1 ) || \
    ( SPIH_CFG_ENABLE > 1 )
#error "Invalid configuration"
#endif

#if ( SPI0_CFG_MASTER_SLAVE > 1 ) || \
    ( SPI1_CFG_MASTER_SLAVE > 1 ) || \
    ( SPIH_CFG_MASTER_SLAVE > 1 )
#error "Invalid configuration"
#endif

#if ( SPI0_CFG_CLK_PHASE > 1 ) || \
    ( SPI1_CFG_CLK_PHASE > 1 ) || \
    ( SPIH_CFG_CLK_PHASE > 1 )
#error "Invalid configuration"
#endif

#if ( SPI0_CFG_CLK_POLARITY > 1 ) || \
    ( SPI1_CFG_CLK_POLARITY > 1 ) || \
    ( SPIH_CFG_CLK_POLARITY > 1 )
#error "Invalid configuration"
#endif

#if ( SPI0_CFG_WIRED_OR > 1 ) || \
    ( SPI1_CFG_WIRED_OR > 1 ) || \
    ( SPIH_CFG_WIRED_OR > 1 )
#error "Invalid configuration"
#endif

#if ( SPI0_CFG_LSB_MSB > 1 ) || \
    ( SPI1_CFG_LSB_MSB > 1 ) || \
    ( SPIH_CFG_LSB_MSB > 1 )
#error "Invalid configuration"
#endif

#if ( SPI0_CFG_TRANSFER_INITIATE > 1 ) || \
    ( SPI1_CFG_TRANSFER_INITIATE > 1 ) || \
    ( SPIH_CFG_TRANSFER_INITIATE > 1 )
#error "Invalid configuration"
#endif

#if ( SPI0_CFG_TX_UNDERFLOW > 1 ) || \
    ( SPI1_CFG_TX_UNDERFLOW > 1 ) || \
    ( SPIH_CFG_TX_UNDERFLOW > 1 )
#error "Invalid configuration"
#endif

#if ( SPI0_CFG_RX_OVERFLOW > 1 ) || \
    ( SPI1_CFG_RX_OVERFLOW > 1 ) || \
    ( SPIH_CFG_RX_OVERFLOW > 1 )
#error "Invalid configuration"
#endif

#if ( SPI0_CFG_MISO_ENABLE > 1 ) || \
    ( SPI1_CFG_MISO_ENABLE > 1 ) || \
    ( SPIH_CFG_MISO_ENABLE > 1 )
#error "Invalid configuration"
#endif

#if ( SPI0_CFG_LOOPBACK > 1 ) || \
    ( SPI1_CFG_LOOPBACK > 1 ) || \
    ( SPIH_CFG_LOOPBACK > 1 )
#error "Invalid configuration"
#endif

#if ( SPI0_CFG_CONTINUOUS > 1 ) || \
    ( SPI1_CFG_CONTINUOUS > 1 ) || \
    ( SPIH_CFG_CONTINUOUS > 1 )
#error "Invalid configuration"
#endif

#if ( SPI0_CFG_RX_FLUSH > 1 ) || \
    ( SPI1_CFG_RX_FLUSH > 1 ) || \
    ( SPIH_CFG_RX_FLUSH > 1 )
#error "Invalid configuration"
#endif

#if ( SPI0_CFG_TX_FLUSH > 1 ) || \
    ( SPI1_CFG_TX_FLUSH > 1 ) || \
    ( SPIH_CFG_TX_FLUSH > 1 )
#error "Invalid configuration"
#endif

#if ( SPI0_CFG_IRQ_MODE > 3 ) || \
    ( SPI1_CFG_IRQ_MODE > 3 ) || \
    ( SPIH_CFG_IRQ_MODE > 3 )
#error "Invalid configuration"
#endif

#if ( SPI0_CFG_CLK_DIV > 63 ) || \
    ( SPI1_CFG_CLK_DIV > 63 ) || \
    ( SPIH_CFG_CLK_DIV > 63 )
#error "Invalid configuration"
#endif

#if ( SPI0_CFG_HFM > 1 ) || \
    ( SPI1_CFG_HFM > 1 ) || \
    ( SPIH_CFG_HFM > 1 )
#error "Invalid configuration"
#endif

#if ( SPI0_CFG_CS_ERR > 1 ) || \
    ( SPI1_CFG_CS_ERR > 1 ) || \
    ( SPIH_CFG_CS_ERR > 1 )
#error "Invalid configuration"
#endif

#if ( SPI0_CFG_CS_IRQ > 1 ) || \
    ( SPI1_CFG_CS_IRQ > 1 ) || \
    ( SPIH_CFG_CS_IRQ > 1 )
#error "Invalid configuration"
#endif

#endif /* ADI_SPI_CFG_ENABLE_STATIC_CONFIG_SUPPORT */

#endif /* __ADI_SPI_CONIG_H__ */
