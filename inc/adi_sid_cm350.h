/*********************************************************************************

Copyright (c) 2010-2014 Analog Devices, Inc. All Rights Reserved.

This software is proprietary to Analog Devices, Inc. and its licensors.  By using 
this software you agree to the terms of the associated Analog Devices Software 
License Agreement.

*********************************************************************************/

/*!
 *****************************************************************************
 * @file:    adi_sid_cm350.h
 * @brief:   Defenition of interrupt IDs using which application/drivers
             register/unregister interrupt handlers.
 * @version: $Revision: 28525 $
 * @date:    $Date: 2014-11-12 14:51:26 -0500 (Wed, 12 Nov 2014) $
**********************************************************************************/


/* Cortex-M3 processor exceptions */
#define ADI_INT_RESET              -15 /* Reset Interrupt                        */
#define ADI_INT_NMI                -14 /* Non Maskable Interrupt                 */
#define ADI_INT_HARD_FAULT         -13 /* Hard Fault Interrupt                   */
#define ADI_INT_MEM_MGMT           -12 /* Memory Management Interrupt            */
#define ADI_INT_BUS_FAULT          -11 /* Bus Fault Interrupt                    */
#define ADI_INT_USAGE_FAULT        -10 /* Usage Fault Interrupt                  */
/* skip */
#define ADI_INT_SVCALL              -5  /* System Service Call Interrupt         */
#define ADI_INT_DEBUG_MON           -4  /* Debug Monitor Interrupt               */
/* skip */
#define ADI_INT_PENDSV              -2  /* Pendable request for system device Interrupt */
#define ADI_INT_SYSTICK             -1  /* System Tick Interrupt                 */

/* ADuCM350 Specific Interrupts */
#define ADI_INT_WUT                  0   /* Wakeup Timer Interrupt               */
#define ADI_INT_EINT0                1   /* External Interrupt 0                 */
#define ADI_INT_EINT1                2   /* External Interrupt 1                 */
#define ADI_INT_EINT2                3   /* External Interrupt 2                 */
#define ADI_INT_EINT3                4   /* External Interrupt 3                 */
#define ADI_INT_EINT4                5   /* External Interrupt 4                 */
#define ADI_INT_EINT5                6   /* External Interrupt 5                 */
#define ADI_INT_EINT6                7   /* External Interrupt 6                 */
#define ADI_INT_EINT7                8   /* External Interrupt 7                 */
#define ADI_INT_EINT8                9   /* External Interrupt 8                 */
#define ADI_INT_WDT                 10   /* Watchdog Timer Interrupt             */
#define ADI_INT_TIMER0              11   /* GP Timer0 Interrupt                  */
#define ADI_INT_TIMER1              12   /* GP Timer1 Interrupt                  */
#define ADI_INT_FLASH0              13   /* Bank0 Flash Memory Interrupt         */
#define ADI_INT_UART                14   /* UART Interrupt                       */
#define ADI_INT_SPI0                15   /* SPI0 Interrupt                       */
#define ADI_INT_SPIH                16   /* SPI-H Interrupt                      */
#define ADI_INT_I2CS                17   /* I2C Slave Interrupt                  */
#define ADI_INT_I2CM                18   /* I2C Master Interrupt                 */
#define ADI_INT_DMA_ERR             19   /* Generic DMA Error Interrupt          */
#define ADI_INT_DMA_SPIH_TX         20   /* DMA SPI-H TX Interrupt               */
#define ADI_INT_DMA_SPIH_RX         21   /* DMA SPI-H RX Interrupt               */
#define ADI_INT_DMA_SPI0_TX         22   /* DMA SPI1 TX Interrupt                */
#define ADI_INT_DMA_SPI0_RX         23   /* DMA SPI1 RX Interrupt                */
#define ADI_INT_DMA_SPI1_TX         24   /* DMA SPI1 TX Interrupt                */
#define ADI_INT_DMA_SPI1_RX         25   /* DMA SPI1 RX Interrupt                */
#define ADI_INT_DMA_UART_TX         26   /* DMA UART TX Interrupt                */
#define ADI_INT_DMA_UART_RX         27   /* DMA UART RX Interrupt                */
#define ADI_INT_DMA_I2CS_TX         28   /* DMA I2C Slave TX Interrupt           */
#define ADI_INT_DMA_I2CS_RX         29   /* DMA I2C Slave RX Interrupt           */
#define ADI_INT_DMA_I2CM            30   /* DMA I2C Shared Master Interrupt      */
#define ADI_INT_DMA_AFE_TX          31   /* DMA AFE TX Interrupt                 */
#define ADI_INT_DMA_AFE_RX          32   /* DMA AFE RX Interrupt                 */
#define ADI_INT_DMA_CRC             33   /* DMA CRC Interrupt                    */
#define ADI_INT_DMA_PDI             34   /* DMA Parallel Display Intf. Interrupt */
#define ADI_INT_DMA_I2S             35   /* DMA I2S Interrupt                    */
#define ADI_INT_USB_WAKEUP          36   /* USB Wakeup interrupt                 */
#define ADI_INT_USB                 37   /* USB Control interrupt                */
#define ADI_INT_USB_DMA             38   /* USB DMA interrupt                    */
#define ADI_INT_I2S                 39   /* I2S interrupt                        */
#define ADI_INT_TIMER2              40   /* GP Timer2 Interrupt                  */
#define ADI_INT_FLASH1              41   /* Bank1 Flash Memory Interrupt         */
#define ADI_INT_SPI1                42   /* SPI1 Interrupt                       */
#define ADI_INT_RTC                 43   /* Realtime Clock Interrupt             */
/*                        interrupt 44 is reserved                               */
#define ADI_INT_BEEP                45   /* Beeper Interrupt                     */
#define ADI_INT_LCD                 46   /* LCD Interrupt                        */
#define ADI_INT_GPIOA               47   /* GPIO Group A Interrupt               */
#define ADI_INT_GPIOB               48   /* GPIO Group B Interrupt               */
/*                        interrupt 49 is reserved                               */
#define ADI_INT_AFE_CAPTURE         50   /* AFE Analog Capture Interrupt         */
#define ADI_INT_AFE_GENERATE        51   /* AFE Analog Generation Interrupt      */
#define ADI_INT_AFE_CMD_FIFO        52   /* AFE Command FIFO Interrupt           */
#define ADI_INT_AFE_DATA_FIFO       53   /* AFE Data FIFO Interrupt              */
#define ADI_INT_CAP                 54   /* Cap Touch Interrupt                  */
#define ADI_INT_GP_FLASH            55   /* GP Flash Memory Interrupt            */
#define ADI_INT_XTAL_OSC            56   /* Crystal Oscillator Interrupt         */
#define ADI_INT_PLL                 57   /* PLL Interrupt                        */
#define ADI_INT_RAND                58   /* Random Bit Generator Interrupt       */
#define ADI_INT_PDI                 59   /* Parallel Display Interface Interrupt */
#define ADI_INT_PARITY              60   /* Instruction Flash Parity failure     */



/* Update this based on the number of interrupts */
#define ADI_INT_NUM_INT         61          /* Number of Interrupts */

