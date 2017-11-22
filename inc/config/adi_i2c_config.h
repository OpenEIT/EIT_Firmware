/*********************************************************************************

Copyright (c) 2012-2014 Analog Devices, Inc. All Rights Reserved.

This software is proprietary to Analog Devices, Inc. and its licensors.  By using 
this software you agree to the terms of the associated Analog Devices Software 
License Agreement.

*********************************************************************************/

/*!
 *****************************************************************************
   @file:    adi_i2c_config.h
   @brief:   Configuration options for I2C driver.
             This is specific to the I2C driver and will be included by the driver.
             It is not required for the application to include this header file.
   @version: $Revision: 28525 $
   @date:    $Date: 2014-11-12 14:51:26 -0500 (Wed, 12 Nov 2014) $
*****************************************************************************/

#ifndef __ADI_I2C_CONIG_H__
#define __ADI_I2C_CONIG_H__

/************* I2C Driver Configurations ***************/

/*!
   Enable DMA support in the driver code.
   1 -  To have the DMA support code in the driver.
   0 -  To eliminate the DMA support.
*/
#define ADI_I2C_CFG_ENABLE_DMA_SUPPORT                     1

/*!
   Set this macro to 1 for adding the I2C slave mode code to the driver.
   To eliminate the I2C slave mode, set the macro to 0.
*/
#define ADI_I2C_CFG_SLAVE_MODE_SUPPORT                     1

/*!
   Set this macro to 1 for to enable static controller initializations
   during the driver initialization routing.
   To eliminate static driver configuration, set this macro to 0.
*/
#define ADI_I2C_CFG_ENABLE_STATIC_CONFIG_SUPPORT           1

/*!
   Set this macro to 1 for to enable I2C application callbacks.
   To eliminate calback support, set this macro to 0.
*/
#define ADI_I2C_CFG_ENABLE_CALLBACK_SUPPORT                1



#if (ADI_I2C_CFG_ENABLE_STATIC_CONFIG_SUPPORT == 1)

/************* I2C Controller Configurations ***************/

/*! Controls Master-mode SCL after SDA is released.
    0 -  Do nothing.
    1 -  Stop SCL after SDA is released before 9 SCL cycles. */
#define I2C_CFG_MASTER_PRESTOP_BUS_CLEAR                   0

/*! Controls Master-mode bus clear enable.
    0 -  Do nothing.
    1 -  initiate bus clear if arbitration is lost. */
#define I2C_CFG_MASTER_BUS_CLEAR_ENABLE                    0

/*! Controls Master-mode DMA TX requests.
    0 -  Disable DMA TX requests.
    1 -  Enable DMA TX requests */
#define I2C_CFG_MASTER_TX_DMA_REQUESTS                     0

/*! Controls Master-mode DMA RX requests.
    0 -  Disable DMA RX requests.
    1 -  Enable DMA RX requests */
#define I2C_CFG_MASTER_RX_DMA_REQUESTS                     0

/*! Controls Master-mode TX FIFO decrement mode.
    0 -  Decrement TX FIFO status on byte unload.
    1 -  Decrement TX FIFO status on byte transmit */
#define I2C_CFG_MASTER_TX_DECREMENT_MODE                   0

/*! Controls Master-mode transaction complete interrupt.
    0 -  Disable transaction complete interrupt.
    1 -  Enable transaction complete interrupt */
#define I2C_CFG_MASTER_IRQ_ENABLE_STOP                     0

/*! Controls Master-mode no acknowledge interrupt.
    0 -  Disable ALOST interrupt.
    1 -  Enable ALOST interrupt */
#define I2C_CFG_MASTER_IRQ_ENABLE_NACK                     0

/*! Controls Master-mode lost arbitration interrupt.
    0 -  Disable ALOST interrupt.
    1 -  Enable ALOST interrupt */
#define I2C_CFG_MASTER_IRQ_ENABLE_ALOST                    0

/*! Controls Master-mode transmit request interrupt.
    0 -  Disable TX_REQ interrupt.
    1 -  Enable TX_REQ interrupt */
#define I2C_CFG_MASTER_IRQ_ENABLE_TX_REQ                   0

/*! Controls Master-mode receive request interrupt.
    0 -  Disable RX_REQ interrupt.
    1 -  Enable RX_REQ interrupt */
#define I2C_CFG_MASTER_IRQ_ENABLE_RX_REQ                   0

/*! Controls Master-mode slave clock stretch.
    0 -  Disable clock stretch.
    1 -  Enable clock stretch */
#define I2C_CFG_MASTER_STRETCH                             0

/*! Controls Master-mode internal loopback.
    0 -  Disable loopback.
    1 -  Enable loopback */
#define I2C_CFG_MASTER_LOOPBACK                            0

/*! Controls Master-mode compete for ownership.
    0 -  Disable contention for bus ownership.
    1 -  Enable contention for bus ownership. */
#define I2C_CFG_MASTER_COMPETE                             0

/*! Controls Master-mode enable.
    0 -  Disable Master-mode operation.
    1 -  Enable Master-mode operation. */
#define I2C_CFG_MASTER_ENABLE                              0



/*! Controls Serial Clock High Period.
    0-31 -  Serial Clock High time. */
#define I2C_CFG_CLOCK_DIV_HIGH                          0x1F

/*! Controls Serial Clock Low Period.
    0-31 -  Serial Clock Low time. */
#define I2C_CFG_CLOCK_DIV_LOW                           0x1F



/*! Controls Slave-mode DMA TX requests.
    0 -  Disable DMA TX requests.
    1 -  Enable DMA TX requests */
#define I2C_CFG_SLAVE_TX_DMA_REQUESTS                      0

/*! Controls Slave-mode DMA RX requests.
    0 -  Disable DMA RX requests.
    1 -  Enable DMA RX requests */
#define I2C_CFG_SLAVE_RX_DMA_REQUESTS                      0

/*! Controls Slave-mode repeat start interrupt.
    0 -  Disable repeat start interrupt.
    1 -  Enable repeat start interrupt */
#define I2C_CFG_SLAVE_IRQ_REPEAT_START                     0

/*! Controls Slave-mode TX FIFO decrement mode.
    0 -  Decrement TX FIFO status on byte unload.
    1 -  Decrement TX FIFO status on byte transmit */
#define I2C_CFG_SLAVE_TX_DECREMENT_MODE                    0

/*! Controls Slave-mode transmit request interrupt.
    0 -  Disable TX_REQ interrupt.
    1 -  Enable TX_REQ interrupt */
#define I2C_CFG_SLAVE_IRQ_ENABLE_TX_REQ                    0

/*! Controls Slave-mode receive request interrupt.
    0 -  Disable RX_REQ interrupt.
    1 -  Enable RX_REQ interrupt */
#define I2C_CFG_SLAVE_IRQ_ENABLE_RX_REQ                    0

/*! Controls Slave-mode stop detected interrupt.
    0 -  Disable stop detected interrupt.
    1 -  Enable stop detected interrupt */
#define I2C_CFG_SLAVE_IRQ_ENABLE_STOP                      0

/*! Controls Slave-mode force no acknowledge.
    0 -  Do not force NACK.
    1 -  Force NACK */
#define I2C_CFG_SLAVE_FORCE_NACK                           0

/*! Controls Slave-mode clock stretch request.
    0 -  Do not request clock stretch.
    1 -  Request clock stretch */
#define I2C_CFG_SLAVE_STRETCH                              0

/*! Controls Slave-mode transmit request timing.
    0 -  Normal TX request timing.
    1 -  Early TX request timing. */
#define I2C_CFG_SLAVE_EARLY_TX_REQ                         0

/*! Controls Slave-mode general call clear.
    0 -  Do not clear general call status.
    1 -  Reset general call status. */
#define I2C_CFG_SLAVE_GENERAL_CALL_CLEAR                   0

/*! Controls Slave-mode hardware general call enable.
    0 -  Disable hardware general call.
    1 -  Enable hardware general call. */
#define I2C_CFG_SLAVE_HW_GENERAL_CALL_ENABLE               0

/*! Controls Slave-mode software general call enable.
    0 -  Disable software general call.
    1 -  Enable software general call. */
#define I2C_CFG_SLAVE_SW_GENERAL_CALL_ENABLE               0

/*! Controls Slave-mode 10-bit addressing enable.
    0 -  Disable 10-bit addressing mode.
    1 -  Enaable 10-bit addressing mode. */
#define I2C_CFG_SLAVE_10_BIT_ADDRESS_ENABLE                0

/*! Controls Slave-mode enable.
    0 -  Disable Slave-mode operation.
    1 -  Enable Slave-mode operation. */
#define I2C_CFG_SLAVE_ENABLE                               0




/************** Macro validation *****************************/

#if ( ADI_I2C_CFG_ENABLE_DMA_SUPPORT > 1 )
#error "Invalid configuration"
#endif

#if ( ADI_I2C_CFG_SLAVE_MODE_SUPPORT > 1 )
#error "Invalid configuration"
#endif

#if ( ADI_I2C_CFG_ENABLE_CALLBACK_SUPPORT > 1 )
#error "Invalid configuration"
#endif

#if ( I2C_CFG_MASTER_PRESTOP_BUS_CLEAR > 1 )
#error "Invalid configuration"
#endif

#if ( I2C_CFG_MASTER_BUS_CLEAR_ENABLE > 1 )
#error "Invalid configuration"
#endif

#if ( I2C_CFG_MASTER_TX_DMA_REQUESTS > 1 )
#error "Invalid configuration"
#endif

#if ( I2C_CFG_MASTER_RX_DMA_REQUESTS > 1 )
#error "Invalid configuration"
#endif

#if ( I2C_CFG_MASTER_TX_DECREMENT_MODE > 1 )
#error "Invalid configuration"
#endif

#if ( I2C_CFG_MASTER_IRQ_ENABLE_STOP > 1 )
#error "Invalid configuration"
#endif

#if ( I2C_CFG_MASTER_IRQ_ENABLE_NACK > 1 )
#error "Invalid configuration"
#endif

#if ( I2C_CFG_MASTER_IRQ_ENABLE_ALOST > 1 )
#error "Invalid configuration"
#endif

#if ( I2C_CFG_MASTER_IRQ_ENABLE_TX_REQ > 1 )
#error "Invalid configuration"
#endif

#if ( I2C_CFG_MASTER_IRQ_ENABLE_RX_REQ > 1 )
#error "Invalid configuration"
#endif

#if ( I2C_CFG_MASTER_STRETCH > 1 )
#error "Invalid configuration"
#endif

#if ( I2C_CFG_MASTER_LOOPBACK > 1 )
#error "Invalid configuration"
#endif

#if ( I2C_CFG_MASTER_COMPETE > 1 )
#error "Invalid configuration"
#endif

#if ( I2C_CFG_MASTER_ENABLE > 1 )
#error "Invalid configuration"
#endif

#if ( I2C_CFG_CLOCK_DIV_HIGH > 0x1F )
#error "Invalid configuration"
#endif

#if ( I2C_CFG_CLOCK_DIV_LOW > 0x1F )
#error "Invalid configuration"
#endif

#if ( I2C_CFG_SLAVE_TX_DMA_REQUESTS > 1 )
#error "Invalid configuration"
#endif

#if ( I2C_CFG_SLAVE_RX_DMA_REQUESTS > 1 )
#error "Invalid configuration"
#endif

#if ( I2C_CFG_SLAVE_IRQ_REPEAT_START > 1 )
#error "Invalid configuration"
#endif

#if ( I2C_CFG_SLAVE_TX_DECREMENT_MODE > 1 )
#error "Invalid configuration"
#endif

#if ( I2C_CFG_SLAVE_IRQ_ENABLE_TX_REQ > 1 )
#error "Invalid configuration"
#endif

#if ( I2C_CFG_SLAVE_IRQ_ENABLE_RX_REQ > 1 )
#error "Invalid configuration"
#endif

#if ( I2C_CFG_SLAVE_IRQ_ENABLE_STOP > 1 )
#error "Invalid configuration"
#endif

#if ( I2C_CFG_SLAVE_FORCE_NACK > 1 )
#error "Invalid configuration"
#endif

#if ( I2C_CFG_SLAVE_STRETCH > 1 )
#error "Invalid configuration"
#endif

#if ( I2C_CFG_SLAVE_EARLY_TX_REQ > 1 )
#error "Invalid configuration"
#endif

#if ( I2C_CFG_SLAVE_GENERAL_CALL_CLEAR > 1 )
#error "Invalid configuration"
#endif

#if ( I2C_CFG_SLAVE_HW_GENERAL_CALL_ENABLE > 1 )
#error "Invalid configuration"
#endif

#if ( I2C_CFG_SLAVE_SW_GENERAL_CALL_ENABLE > 1 )
#error "Invalid configuration"
#endif

#if ( I2C_CFG_SLAVE_10_BIT_ADDRESS_ENABLE > 1 )
#error "Invalid configuration"
#endif

#if ( I2C_CFG_SLAVE_ENABLE > 1 )
#error "Invalid configuration"
#endif

#endif /* ADI_I2C_CFG_ENABLE_STATIC_CONFIG_SUPPORT */


#endif /* __ADI_I2C_CONIG_H__ */
