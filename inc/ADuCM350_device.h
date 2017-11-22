/* ================================================================================

     Project      :   ADuCM350
     File         :   ADuCM350_device.h
     Description  :   Register Definitions

     Date         :   11-07-2012


     Copyright (c) 2011-2013 Analog Devices, Inc.  All Rights Reserved.
     This software is proprietary and confidential to Analog Devices, Inc. and
     its licensors.

     This file was auto-generated. Do not make local changes to this file.

   ================================================================================ */

#ifndef _ADuCM350_DEVICE_H
#define _ADuCM350_DEVICE_H

/*! \file ADuCM350_device.h */

/* pickup integer types */
#include <stdint.h>

#ifndef __IO
  #ifdef __cplusplus
    #define     __I     volatile           /* read-only */
  #else
    #define     __I     volatile const     /* read-only */
  #endif
  #define     __O     volatile             /* write-only */
  #define     __IO    volatile             /* read-write */
#endif

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

/* Feature macro for conditional compilation */
#define ADI_ADuCM350

/* pickup register bitfield and bit mask macros */
#include "defADuCM350.h"


/*!
 * \struct ADI_GPT_TypeDef
 * \brief  General Purpose Timer
*/
typedef struct
{
    __IO uint16_t GPTLD;                         /*!< 16-bit load value */
    __I  uint8_t                  RESERVED0[2];
    __I  uint16_t GPTVAL;                        /*!< 16-bit timer value */
    __I  uint8_t                  RESERVED1[2];
    __IO uint16_t GPTCON;                        /*!< Control */
    __I  uint8_t                  RESERVED2[2];
    __O  uint16_t GPTCLRI;                       /*!< Clear Interrupt */
    __I  uint8_t                  RESERVED3[2];
    __I  uint16_t GPTCAP;                        /*!< Capture */
    __I  uint8_t                  RESERVED4[2];
    __IO uint16_t GPTALD;                        /*!< 16-bit load value, asynchronous */
    __I  uint8_t                  RESERVED5[2];
    __I  uint16_t GPTAVAL;                       /*!< 16-bit timer value, asynchronous */
    __I  uint8_t                  RESERVED6[2];
    __I  uint16_t GPTSTA;                        /*!< Status */
    __I  uint8_t                  RESERVED7[2];
    __IO uint16_t GPTPCON;                       /*!< PWM Control Register */
    __I  uint8_t                  RESERVED8[2];
    __IO uint16_t GPTPMAT;                       /*!< PWM Match Value */
} ADI_GPT_TypeDef;


/*!
 * \struct ADI_PWR_TypeDef
 * \brief
*/
typedef struct
{
    __IO uint16_t PWRMOD;                        /*!< Power modes */
    __I  uint8_t                  RESERVED0[2];
    __IO uint16_t PWRKEY;                        /*!< Key protection for PWRMOD */
    __I  uint8_t                  RESERVED1[2];
    __IO uint16_t PSMCON;                        /*!< PSM Configuration */
    __I  uint8_t                  RESERVED2[2];
    __IO uint16_t OSCKEY;                        /*!< Key protection for OSCCTRL */
    __I  uint8_t                  RESERVED3[2];
    __IO uint16_t OSCCTRL;                       /*!< Oscillator control */
    __I  uint8_t                  RESERVED4[14];
    __IO uint16_t EI0CFG;                        /*!< External Interrupt configuration 0 */
    __I  uint8_t                  RESERVED5[2];
    __IO uint16_t EI1CFG;                        /*!< External Interrupt configuration 1 */
    __I  uint8_t                  RESERVED6[2];
    __IO uint16_t EI2CFG;                        /*!< External Interrupt configuration 2 */
    __I  uint8_t                  RESERVED7[6];
    __IO uint16_t EICLR;                         /*!< External Interrupt clear */
    __I  uint8_t                  RESERVED8[2];
    __IO uint16_t NMICLR;                        /*!< Non-maskable interrupt clear */
    __I  uint8_t                  RESERVED9[2];
    __I  uint16_t USBWKSTAT;                     /*!< USB Wakeup Status */
    __I  uint8_t                 RESERVED10[6];
    __IO uint16_t RSTSTA;                        /*!< Reset status */
    __I  uint8_t                 RESERVED11[70];
    __IO uint16_t VCCMCON;                       /*!< VCCM Control and Status */
    __I  uint8_t                 RESERVED19[2];
    __IO uint16_t VBACKCON;                      /*!< VBACK control and status */
} ADI_PWR_TypeDef;


/*!
 * \struct ADI_WUT_TypeDef
 * \brief  WUT
*/
typedef struct
{
    __I  uint16_t T2VAL0;                        /*!< Current count value - LS halfword. */
    __I  uint8_t                  RESERVED0[2];
    __I  uint16_t T2VAL1;                        /*!< Current count value - MS halfword */
    __I  uint8_t                  RESERVED1[2];
    __IO uint16_t T2CON;                         /*!< Control */
    __I  uint8_t                  RESERVED2[2];
    __IO uint16_t T2INC;                         /*!< 12-bit interval for wakeup field A */
    __I  uint8_t                  RESERVED3[2];
    __IO uint16_t T2WUFB0;                       /*!< Wakeup field B - LS halfword */
    __I  uint8_t                  RESERVED4[2];
    __IO uint16_t T2WUFB1;                       /*!< Wakeup field B - MS halfword */
    __I  uint8_t                  RESERVED5[2];
    __IO uint16_t T2WUFC0;                       /*!< Wakeup field C - LS halfword */
    __I  uint8_t                  RESERVED6[2];
    __IO uint16_t T2WUFC1;                       /*!< Wakeup field C - MS halfword */
    __I  uint8_t                  RESERVED7[2];
    __IO uint16_t T2WUFD0;                       /*!< Wakeup field D - LS halfword */
    __I  uint8_t                  RESERVED8[2];
    __IO uint16_t T2WUFD1;                       /*!< Wakeup field D - MS halfword */
    __I  uint8_t                  RESERVED9[2];
    __IO uint16_t T2IEN;                         /*!< Interrupt enable */
    __I  uint8_t                 RESERVED10[2];
    __I  uint16_t T2STA;                         /*!< Status */
    __I  uint8_t                 RESERVED11[2];
    __O  uint16_t T2CLRI;                        /*!< Clear interrupts */
    __I  uint8_t                 RESERVED12[2];
    __IO uint16_t WUTVAL_LOW;                    /*!< Unsynchronized lower 16 bits of WU Timer counter value. */
    __I  uint8_t                 RESERVED13[2];
    __IO uint16_t WUTVAL_HIGH;                   /*!< Unsynchronized upper 16 bits of WU Timer counter value. */
    __I  uint8_t                 RESERVED14[2];
    __IO uint16_t T2WUFA0;                       /*!< Wakeup field A - LS halfword */
    __I  uint8_t                 RESERVED15[2];
    __IO uint16_t T2WUFA1;                       /*!< Wakeup field A - MS halfword */
} ADI_WUT_TypeDef;


/*!
 * \struct ADI_WDT_TypeDef
 * \brief  WDT
*/
typedef struct
{
    __IO uint16_t T3LD;                          /*!< Load value */
    __I  uint8_t                  RESERVED0[2];
    __I  uint16_t T3VAL;                         /*!< Current count value */
    __I  uint8_t                  RESERVED1[2];
    __IO uint16_t T3CON;                         /*!< Control */
    __I  uint8_t                  RESERVED2[2];
    __O  uint16_t T3CLRI;                        /*!< Clear interrupt */
    __I  uint8_t                  RESERVED3[2];
    __IO uint16_t T3RCR;                         /*!< Reset Control Register */
    __I  uint8_t                  RESERVED4[2];
    __I  uint16_t T3VALA;                        /*!< Asynchronous current count value */
    __I  uint8_t                  RESERVED5[2];
    __I  uint16_t T3STA;                         /*!< Status */
} ADI_WDT_TypeDef;


/*!
 * \struct ADI_RTC_TypeDef
 * \brief  Real-Time Clock
*/
typedef struct
{
    __IO uint32_t RTCCR;                         /*!< RTC Control */
    __IO uint16_t RTCSR0;                        /*!< RTC Status 0 */
    __I  uint8_t                  RESERVED0[2];
    __I  uint16_t RTCSR1;                        /*!< RTC Status 1 */
    __I  uint8_t                  RESERVED1[2];
    __IO uint16_t RTCCNT0;                       /*!< RTC Count 0 */
    __I  uint8_t                  RESERVED2[2];
    __IO uint16_t RTCCNT1;                       /*!< RTC Count 1 */
    __I  uint8_t                  RESERVED3[2];
    __IO uint16_t RTCALM0;                       /*!< RTC Alarm 0 */
    __I  uint8_t                  RESERVED4[2];
    __IO uint16_t RTCALM1;                       /*!< RTC Alarm 1 */
    __I  uint8_t                  RESERVED5[2];
    __IO uint16_t RTCTRM;                        /*!< RTC Trim */
    __I  uint8_t                  RESERVED6[2];
    __IO uint16_t RTCGWY;                        /*!< RTC Gateway */
} ADI_RTC_TypeDef;


/*!
 * \struct ADI_I2C_TypeDef
 * \brief  I2C Master/Slave
*/
typedef struct
{
    __IO uint16_t I2CMCON;                       /*!< Master control */
    __I  uint8_t                  RESERVED0[2];
    __I  uint16_t I2CMSTA;                       /*!< Master status */
    __I  uint8_t                  RESERVED1[2];
    __I  uint16_t I2CMRX;                        /*!< Master receive data */
    __I  uint8_t                  RESERVED2[2];
    __IO uint16_t I2CMTX;                        /*!< Master transmit data */
    __I  uint8_t                  RESERVED3[2];
    __IO uint16_t I2CMRXCNT;                     /*!< Master receive data count */
    __I  uint8_t                  RESERVED4[2];
    __I  uint16_t I2CMCRXCNT;                    /*!< Master current receive data count */
    __I  uint8_t                  RESERVED5[2];
    __IO uint16_t I2CADR1;                       /*!< 1st master address byte */
    __I  uint8_t                  RESERVED6[2];
    __IO uint16_t I2CADR2;                       /*!< 2nd master address byte */
    __I  uint8_t                  RESERVED7[2];
    __IO uint16_t I2CBYT;                        /*!< Start byte */
    __I  uint8_t                  RESERVED8[2];
    __IO uint16_t I2CDIV;                        /*!< Serial clock period divisor */
    __I  uint8_t                  RESERVED9[2];
    __IO uint16_t I2CSCON;                       /*!< Slave control */
    __I  uint8_t                 RESERVED10[2];
    __I  uint16_t I2CSSTA;                       /*!< Slave I2C Status/Error/IRQ */
    __I  uint8_t                 RESERVED11[2];
    __I  uint16_t I2CSRX;                        /*!< Slave receive */
    __I  uint8_t                 RESERVED12[2];
    __IO uint16_t I2CSTX;                        /*!< Slave transmit */
    __I  uint8_t                 RESERVED13[2];
    __IO uint16_t I2CALT;                        /*!< Hardware general call ID */
    __I  uint8_t                 RESERVED14[2];
    __IO uint16_t I2CID0;                        /*!< 1st slave address device ID */
    __I  uint8_t                 RESERVED15[2];
    __IO uint16_t I2CID1;                        /*!< 2nd slave address device ID */
    __I  uint8_t                 RESERVED16[2];
    __IO uint16_t I2CID2;                        /*!< 3rd slave address device ID */
    __I  uint8_t                 RESERVED17[2];
    __IO uint16_t I2CID3;                        /*!< 4th slave address device ID */
    __I  uint8_t                 RESERVED18[2];
    __IO uint16_t I2CFSTA;                       /*!< Master and slave FIFO status */
    __I  uint8_t                 RESERVED19[2];
    __O  uint16_t I2CSHCON;                      /*!< Shared control */
    __I  uint8_t                 RESERVED20[2];
    __IO uint16_t I2CTCTL;                       /*!< Timing Control Register */
} ADI_I2C_TypeDef;


/*!
 * \struct ADI_SPI_TypeDef
 * \brief  SPI
*/
typedef struct
{
    __I  uint16_t SPISTA;                        /*!< Status */
    __I  uint8_t                  RESERVED0[2];
    __I  uint16_t SPIRX;                         /*!< Receive */
    __I  uint8_t                  RESERVED1[2];
    __O  uint16_t SPITX;                         /*!< Transmit */
    __I  uint8_t                  RESERVED2[2];
    __IO uint16_t SPIDIV;                        /*!< Baud rate selection */
    __I  uint8_t                  RESERVED3[2];
    __IO uint16_t SPICON;                        /*!< SPI configuration */
    __I  uint8_t                  RESERVED4[2];
    __IO uint16_t SPIDMA;                        /*!< SPI DMA enable */
    __I  uint8_t                  RESERVED5[2];
    __IO uint16_t SPICNT;                        /*!< Transfer byte count */
} ADI_SPI_TypeDef;


/*!
 * \struct ADI_UART_TypeDef
 * \brief  UART
*/
typedef struct
{
    union {
    __I  uint16_t COMRX;                         /*!< Receive Buffer Register */
    __O  uint16_t COMTX;                         /*!< Transmit Holding Register */
    };
    __I  uint8_t                  RESERVED0[2];
    __IO uint16_t COMIEN;                        /*!< Interrupt Enable */
    __I  uint8_t                  RESERVED1[2];
    __I  uint16_t COMIIR;                        /*!< Interrupt ID */
    __I  uint8_t                  RESERVED2[2];
    __IO uint16_t COMLCR;                        /*!< Line Control */
    __I  uint8_t                  RESERVED3[2];
    __IO uint16_t COMMCR;                        /*!< Modem Control */
    __I  uint8_t                  RESERVED4[2];
    __I  uint16_t COMLSR;                        /*!< Line Status */
    __I  uint8_t                  RESERVED5[2];
    __I  uint16_t COMMSR;                        /*!< Modem Status */
    __I  uint8_t                  RESERVED6[2];
    __IO uint16_t COMSCR;                        /*!< Scratch buffer */
    __I  uint8_t                  RESERVED7[6];
    __IO uint16_t COMFBR;                        /*!< Fractional Baud Rate */
    __I  uint8_t                  RESERVED8[2];
    __IO uint16_t COMDIV;                        /*!< Baudrate divider */
/* FIXME: REMOVED HAND-EDIT ONCE COMCON IS RESTORED IN CODE */
         uint16_t RESERVED10[3];
    __IO  uint8_t COMCON;         /*!< Control Register                      */
} ADI_UART_TypeDef;


/*!
 * \struct ADI_I2S_TypeDef
 * \brief  I2S serial port
*/
typedef struct
{
    __IO uint16_t I2S_OUT1L;                     /*!< Channel 1 LSBs */
    __I  uint8_t                  RESERVED0[2];
    __IO uint16_t I2S_OUT1H;                     /*!< Channel 1 MSBs */
    __I  uint8_t                  RESERVED1[2];
    __IO uint16_t I2S_OUT2L;                     /*!< Channel 2 LSBs */
    __I  uint8_t                  RESERVED2[2];
    __IO uint16_t I2S_OUT2H;                     /*!< Channel 2 MSBs */
    __I  uint8_t                  RESERVED3[2];
    __IO uint16_t I2S_MODE1;                     /*!< I2S format modes 1 */
    __I  uint8_t                  RESERVED4[2];
    __IO uint16_t I2S_MODE2;                     /*!< I2S format modes 2 */
    __I  uint8_t                  RESERVED5[2];
    __IO uint16_t I2S_CFG1;                      /*!< I2S configuration 1 */
    __I  uint8_t                  RESERVED6[2];
    __IO uint16_t I2S_CFG2;                      /*!< I2S configuration 2 */
    __I  uint8_t                  RESERVED7[2];
    __IO uint16_t I2S_STAT;                      /*!< I2S status */
} ADI_I2S_TypeDef;


/*!
 * \struct ADI_BEEP_TypeDef
 * \brief  Beeper Driver
*/
typedef struct
{
    __IO uint16_t BEEP_CFG;                      /*!< Beeper configuration */
    __I  uint8_t                  RESERVED0[2];
    __IO uint16_t BEEP_STAT;                     /*!< Beeper status */
    __I  uint8_t                  RESERVED1[2];
    __IO uint16_t BEEP_TONE_A;                   /*!< Tone A Data */
    __I  uint8_t                  RESERVED2[2];
    __IO uint16_t BEEP_TONE_B;                   /*!< Tone B Data */
} ADI_BEEP_TypeDef;


/*!
 * \struct ADI_RNG_TypeDef
 * \brief  Random Number Generator
*/
typedef struct
{
    __IO uint16_t RNGCTL;                        /*!< RNG Control Register */
    __I  uint8_t                  RESERVED0[2];
    __IO uint16_t RNGLEN;                        /*!< RNG Sample Length Register */
    __I  uint8_t                  RESERVED1[2];
    __IO uint16_t RNGSTAT;                       /*!< RNG Status Register */
    __I  uint8_t                  RESERVED2[2];
    __I  uint16_t RNGDATA;                       /*!< RNG Data Register */
    __I  uint8_t                  RESERVED3[2];
    __I  uint16_t RNGCNTL;                       /*!< Oscillator Count Low */
    __I  uint8_t                  RESERVED4[2];
    __I  uint16_t RNGCNTH;                       /*!< Oscillator Count High */
} ADI_RNG_TypeDef;


/*!
 * \struct ADI_LCD_LCDDATA_P1_TypeDef
 * \brief  LCD data registers for Page 1
*/
typedef struct
{
    __IO uint16_t LCDDATA_S1;                    /*!< Screen 1 LCD Data Register n */
    __I  uint8_t                  RESERVED0[2];
} ADI_LCD_LCDDATA_P1_TypeDef;


/*!
 * \struct ADI_LCD_LCDDATA_P0_TypeDef
 * \brief  LCD data registers for Page 0
*/
typedef struct
{
    __IO uint16_t LCDDATA_S0;                    /*!< Screen 0 LCD Data Register n */
    __I  uint8_t                  RESERVED0[2];
} ADI_LCD_LCDDATA_P0_TypeDef;


/*!
 * \struct ADI_LCD_TypeDef
 * \brief  LCD Controller
*/
typedef struct
{
    __IO uint16_t LCDCON;                        /*!< LCD Configuration  Register */
    __I  uint8_t                  RESERVED0[2];
    __IO uint16_t LCDSTAT;                       /*!< LCD Status Register */
    __I  uint8_t                  RESERVED1[2];
    __IO uint16_t LCDBLINK;                      /*!< LCD Blink Control Register */
    __I  uint8_t                  RESERVED2[2];
    __IO uint16_t LCDCONTRAST;                   /*!< LCD Contrast Control Register */
    __I  uint8_t                  RESERVED3[2];
    ADI_LCD_LCDDATA_P0_TypeDef LCDDATA_P0[8];    /*!< LCD data registers for Page 0 */
    ADI_LCD_LCDDATA_P1_TypeDef LCDDATA_P1[8];    /*!< LCD data registers for Page 1 */
} ADI_LCD_TypeDef;


/*!
 * \struct ADI_DMA_TypeDef
 * \brief  DMA
*/
typedef struct
{
    __I  uint32_t DMASTA;                        /*!< DMA Status */
    __O  uint32_t DMACFG;                        /*!< DMA Configuration */
    __IO uint32_t DMAPDBPTR;                     /*!< DMA channel primary control data base pointer */
    __I  uint32_t DMAADBPTR;                     /*!< DMA channel alternate control data base pointer */
    __I  uint8_t                  RESERVED0[4];
    __O  uint32_t DMASWREQ;                      /*!< DMA channel software request */
    __I  uint8_t                  RESERVED1[8];
    __IO uint32_t DMARMSKSET;                    /*!< DMA channel request mask set */
    __O  uint32_t DMARMSKCLR;                    /*!< DMA channel request mask clear */
    __IO uint32_t DMAENSET;                      /*!< DMA channel enable set */
    __O  uint32_t DMAENCLR;                      /*!< DMA channel enable clear */
    __IO uint32_t DMAALTSET;                     /*!< DMA channel primary-alternate set */
    __O  uint32_t DMAALTCLR;                     /*!< DMA channel primary-alternate clear */
    __IO uint32_t DMAPRISET;                     /*!< DMA channel priority set */
    __O  uint32_t DMAPRICLR;                     /*!< DMA channel priority clear */
    __I  uint8_t                  RESERVED2[8];
    __IO uint32_t DMAERRCHNLCLR;                 /*!< DMA Per Channel Error Clear */
    __IO uint32_t DMAERRCLR;                     /*!< DMA bus error clear */
    __IO uint32_t DMAINVALIDDESCCLR;             /*!< DMA Per Channel Invalid Descriptor Clear */
    __I  uint8_t                  RESERVED3[1964];
    __IO uint32_t DMABSSET;                      /*!< DMA channel bytes swap enable set */
    __O  uint32_t DMABSCLR;                      /*!< DMA channel bytes swap enable clear */
    __I  uint8_t                  RESERVED4[8];
    __IO uint32_t DMASRCADSSET;                  /*!< DMA channel source address decrement enable set */
    __O  uint32_t DMASRCADCLR;                   /*!< DMA channel source address decrement enable clear */
    __IO uint32_t DMADSTADSET;                   /*!< DMA channel destination address decrement enable set */
    __O  uint32_t DMADSTADCLR;                   /*!< DMA channel destination address decrement enable clear */
    __I  uint8_t                  RESERVED5[1984];
    __I  uint32_t DMAREVID;                      /*!< DMA Controller Revision ID */
} ADI_DMA_TypeDef;


/*!
 * \struct ADI_FEE_TypeDef
 * \brief  Flash Controller1
*/
typedef struct
{
    __I  uint16_t FEESTA;                        /*!< Status */
    __I  uint8_t                  RESERVED0[2];
    __IO uint16_t FEECON0;                       /*!< Command Control */
    __I  uint8_t                  RESERVED1[2];
    __IO uint16_t FEECMD;                        /*!< Command */
    __I  uint8_t                  RESERVED2[6];
    __IO uint16_t FEEADR0L;                      /*!< Lower page address */
    __I  uint8_t                  RESERVED3[2];
    __IO uint16_t FEEADR0H;                      /*!< Upper page address */
    __I  uint8_t                  RESERVED4[2];
    __IO uint16_t FEEADR1L;                      /*!< Lower page address */
    __I  uint8_t                  RESERVED5[2];
    __IO uint16_t FEEADR1H;                      /*!< Upper page address */
    __I  uint8_t                  RESERVED6[2];
    __O  uint16_t FEEKEY;                        /*!< Key */
    __I  uint8_t                  RESERVED7[6];
    __IO uint16_t FEEPROL;                       /*!< Lower halfword of write protection */
    __I  uint8_t                  RESERVED8[2];
    __IO uint16_t FEEPROH;                       /*!< Upper halfword of write protection */
    __I  uint8_t                  RESERVED9[2];
    __I  uint16_t FEESIGL;                       /*!< Lower halfword of signature */
    __I  uint8_t                 RESERVED10[2];
    __I  uint16_t FEESIGH;                       /*!< Upper halfword of signature */
    __I  uint8_t                 RESERVED11[2];
    __IO uint16_t FEECON1;                       /*!< User Setup */
    __I  uint8_t                 RESERVED12[14];
    __I  uint16_t FEEADRAL;                      /*!< Lower halfword of write abort address */
    __I  uint8_t                 RESERVED13[2];
    __I  uint16_t FEEADRAH;                      /*!< Upper halfword of write abort address */
    __I  uint8_t                 RESERVED14[2];
    __IO uint16_t FEEPARCTL;                     /*!< Parity Control Register */
    __I  uint8_t                 RESERVED15[2];
    __IO uint16_t FEEPARSTA;                     /*!< Parity Status Register */
    __I  uint8_t                 RESERVED16[2];
    __I  uint16_t FEEPARADRL;                    /*!< Parity Error Address Low */
    __I  uint8_t                 RESERVED17[2];
    __I  uint16_t FEEPARADRH;                    /*!< Parity Error Address High */
    __I  uint8_t                 RESERVED18[26];
    __IO uint16_t FEEAEN0;                       /*!< System IRQ abort enable for interrupts 15 to 0 */
    __I  uint8_t                 RESERVED19[2];
    __IO uint16_t FEEAEN1;                       /*!< System IRQ abort enable for interrupts 31 to 16 */
    __I  uint8_t                 RESERVED20[2];
    __IO uint16_t FEEAEN2;                       /*!< System IRQ abort enable for interrupts 47 to 32 */
    __I  uint8_t                 RESERVED21[2];
    __IO uint16_t FEEAEN3;                       /*!< System IRQ abort enable for interrupts 60 to 48 */
} ADI_FEE_TypeDef;


/*!
 * \struct ADI_GPIO_TypeDef
 * \brief  GPIO
*/
typedef struct
{
    __IO uint32_t GPCON;                         /*!< GPIO Port 0  Configuration */
    __IO uint16_t GPOEN;                         /*!< GPIO Port 0 output enable */
    __I  uint8_t                  RESERVED0[2];
    __IO uint16_t GPPE;                          /*!< GPIO Port 0 output pullup/pulldown enable */
    __I  uint8_t                  RESERVED1[2];
    __IO uint16_t GPIEN;                         /*!< GPIO  Port 0 Input Path Enable */
    __I  uint8_t                  RESERVED2[2];
    __I  uint16_t GPIN;                          /*!< GPIO  Port 0 registered data input */
    __I  uint8_t                  RESERVED3[2];
    __IO uint16_t GPOUT;                         /*!< GPIO Port 0 data output */
    __I  uint8_t                  RESERVED4[2];
    __O  uint16_t GPSET;                         /*!< GPIO Port 0 data out set */
    __I  uint8_t                  RESERVED5[2];
    __O  uint16_t GPCLR;                         /*!< GPIO Port 0 data out clear */
    __I  uint8_t                  RESERVED6[2];
    __O  uint16_t GPTGL;                         /*!< GPIO Port 0  pin toggle */
    __I  uint8_t                  RESERVED7[2];
    __IO uint16_t GPPOL;                         /*!< GPIO Port 0 interrupt polarity */
    __I  uint8_t                  RESERVED8[2];
    __IO uint16_t GPIENA;                        /*!< GPIO Port 0  interrupt A enable */
    __I  uint8_t                  RESERVED9[2];
    __IO uint16_t GPIENB;                        /*!< GPIO Port 0 interrupt B enable */
    __I  uint8_t                 RESERVED10[2];
    __IO uint16_t GPINT;                         /*!< GPIO Port 0 interrupt Status */
} ADI_GPIO_TypeDef;


/*!
 * \struct ADI_SYSCLK_TypeDef
 * \brief  Clock gating and other settings
*/
typedef struct
{
    __IO uint16_t CLKCON0;                       /*!< Misc clock settings */
    __I  uint8_t                  RESERVED0[2];
    __IO uint16_t CLKCON1;                       /*!< Clock dividers */
    __I  uint8_t                  RESERVED1[6];
    __IO uint16_t CLKCON3;                       /*!< System PLL */
    __I  uint8_t                  RESERVED2[2];
    __IO uint16_t CLKCON4;                       /*!< USB PLL */
    __I  uint8_t                  RESERVED3[2];
    __IO uint16_t CLKCON5;                       /*!< User clock gating control */
    __I  uint8_t                  RESERVED4[2];
    __IO uint16_t CLKSTAT0;                      /*!< Clocking status */
} ADI_SYSCLK_TypeDef;


/*!
 * \struct ADI_BUSM_TypeDef
 * \brief  changed from 4002800
*/
typedef struct
{
    __IO uint16_t BMARBIT0;                      /*!< Arbitration Priority Configuration for Slave 0 (Flash) */
    __I  uint8_t                  RESERVED0[2];
    __IO uint16_t BMARBIT1;                      /*!< Arbitration Priority Configuration for Slave 1 (SRAM0) */
    __I  uint8_t                  RESERVED1[2];
    __IO uint16_t BMARBIT2;                      /*!< Arbitration Priority Configuration for Slave 2 (SRAM1 + EEPROM) */
    __I  uint8_t                  RESERVED2[2];
    __IO uint16_t BMARBIT3;                      /*!< Arbitration Priority Configuration for Slave 3 (AFE) */
    __I  uint8_t                  RESERVED3[2];
    __IO uint16_t BMARBIT4;                      /*!< Arbitration Priority Configuration for Slave 4 (32-bit APB) */
    __I  uint8_t                  RESERVED4[2];
    __IO uint16_t BMARBIT5;                      /*!< Arbitration Priority Configuration for Slave 5 (16-bit APB with DMA access) */
} ADI_BUSM_TypeDef;


/*!
 * \struct ADI_CRC_TypeDef
 * \brief  CRC
*/
typedef struct
{
    __IO uint32_t CRC_CTL;                       /*!< CRC Control Register */
    __IO uint32_t CRC_IPDATA;                    /*!< Input Data Register */
    __IO uint32_t CRC_RESULT;                    /*!< CRC Result Register */
} ADI_CRC_TypeDef;


/*!
 * \struct ADI_PDI_TypeDef
 * \brief  PDI
*/
typedef struct
{
    __IO uint32_t PDI_CFG;                       /*!< PDI Configuration  Register */
    __O  uint32_t PDI_INT_SET;                   /*!< PDI Interrupt Set Register */
    __O  uint32_t PDI_INT_CLR;                   /*!< PDI Interrupt Clear Register */
    __IO uint32_t PDI_STAT;                      /*!< PDI Status Register */
    __IO uint32_t PDI_CMD;                       /*!< PDI Command Register */
    __IO uint32_t PDI_FRDATA_N;                  /*!< PDI Frame Data Count Register */
    __IO uint32_t PDI_FIFO;                      /*!< PDI Parameter FIFO */
    __IO uint32_t PDI_IF_TIMING;                 /*!< PDI Interface Timing Register */
} ADI_PDI_TypeDef;


/*!
 * \struct ADI_AFE_TypeDef
 * \brief  AFE Register Map
*/
typedef struct
{
    __IO uint32_t AFE_CFG;                       /*!< AFE Configuration */
    __IO uint32_t AFE_SEQ_CFG;                   /*!< Sequencer Configuration */
    __IO uint32_t AFE_FIFO_CFG;                  /*!< FIFOs Configuration */
    __IO uint32_t AFE_SW_CFG;                    /*!< Switch Matrix Configuration */
    __IO uint32_t AFE_DAC_CFG;                   /*!< DAC Configuration */
    __IO uint32_t AFE_WG_CFG;                    /*!< Waveform Generator Configuration */
    __IO uint32_t AFE_WG_DCLEVEL_1;              /*!< Waveform Generator - Trapezoid DC Level 1 */
    __IO uint32_t AFE_WG_DCLEVEL_2;              /*!< Waveform Generator - Trapezoid DC Level 2 */
    __IO uint32_t AFE_WG_DELAY_1;                /*!< Waveform Generator - Trapezoid Delay 1 Time */
    __IO uint32_t AFE_WG_SLOPE_1;                /*!< Waveform Generator - Trapezoid Slope 1 Time */
    __IO uint32_t AFE_WG_DELAY_2;                /*!< Waveform Generator - Trapezoid Delay 2 Time */
    __IO uint32_t AFE_WG_SLOPE_2;                /*!< Waveform Generator - Trapezoid Slope 2 Time */
    __IO uint32_t AFE_WG_FCW;                    /*!< Waveform Generator - Sinusoid Frequency Control Word */
    __IO uint32_t AFE_WG_PHASE;                  /*!< Waveform Generator - Sinusoid Phase Offset */
    __IO uint32_t AFE_WG_OFFSET;                 /*!< Waveform Generator - Sinusoid Offset */
    __IO uint32_t AFE_WG_AMPLITUDE;              /*!< Waveform Generator - Sinusoid Amplitude */
    __IO uint32_t AFE_ADC_CFG;                   /*!< ADC Configuration */
    __IO uint32_t AFE_SUPPLY_LPF_CFG;            /*!< Supply Rejection Filter Configuration */
    __IO uint32_t AFE_SW_FULL_CFG_MSB;           /*!< Switch Matrix Full Configuration (MSB) */
    __IO uint32_t AFE_SW_FULL_CFG_LSB;           /*!< Switch Matrix Full Configuration (LSB) */
    __IO uint8_t                  RESERVED0[4];
    __IO uint32_t AFE_WG_DAC_CODE;               /*!< Waveform Generator - DAC Code */
    __I  uint32_t AFE_STATUS;                    /*!< AFE Status */
    __I  uint8_t                  RESERVED1[4];
    __I  uint32_t AFE_SEQ_CRC;                   /*!< Sequencer CRC Value */
    __IO uint32_t AFE_SEQ_COUNT;                 /*!< Sequencer Command Count */
    __I  uint32_t AFE_SEQ_TIMEOUT;               /*!< Sequencer Timeout Counter */
    __I  uint32_t AFE_DATA_FIFO_READ;            /*!< Data FIFO Read */
    __O  uint32_t AFE_CMD_FIFO_WRITE;            /*!< Command FIFO Write */
    __IO uint32_t AFE_ADC_RESULT;                /*!< ADC Raw Result */
    __IO uint32_t AFE_DFT_RESULT_REAL;           /*!< DFT Result, Real Part */
    __IO uint32_t AFE_DFT_RESULT_IMAG;           /*!< DFT Result, Imaginary Part */
    __IO uint32_t AFE_SUPPLY_LPF_RESULT;         /*!< Supply Rejection Filter Result */
    __IO uint32_t AFE_TEMP_SENSOR_RESULT;        /*!< Temperature Sensor Result */
    __IO uint8_t                  RESERVED2[4];
    __IO uint32_t AFE_ANALOG_CAPTURE_IEN;        /*!< Analog Capture Interrupt Enable */
    __IO uint32_t AFE_ANALOG_GEN_IEN;            /*!< Analog Generation Interrupt Enable */
    __IO uint32_t AFE_CMD_FIFO_IEN;              /*!< Command FIFO Interrupt Enable */
    __IO uint32_t AFE_DATA_FIFO_IEN;             /*!< Data FIFO Interrupt Enable */
    __IO uint8_t                  RESERVED3[4];
    __IO uint32_t AFE_ANALOG_CAPTURE_INT;        /*!< Analog Capture Interrupt */
    __IO uint32_t AFE_ANALOG_GEN_INT;            /*!< Analog Generation Interrupt */
    __IO uint32_t AFE_CMD_FIFO_INT;              /*!< Command FIFO Interrupt */
    __IO uint32_t AFE_DATA_FIFO_INT;             /*!< Data FIFO Interrupt */
    __I  uint32_t AFE_SW_STATUS_MSB;             /*!< Switch Matrix Status (MSB) */
    __I  uint32_t AFE_SW_STATUS_LSB;             /*!< Switch Matrix Status (LSB) */
    __IO uint32_t AFE_ADCMIN;                    /*!< ADC Minimum Value Check */
    __IO uint32_t AFE_ADCMAX;                    /*!< ADC Maximum Value Check */
    __IO uint32_t AFE_ADCDELTA;                  /*!< ADC Delta Check */
    __I  uint8_t                  RESERVED4[60];
    __IO uint32_t AFE_CAL_DATA_LOCK;             /*!< Calibration Data Lock */
    __IO uint32_t AFE_ADC_GAIN_TIA;              /*!< ADC Gain (TIA Measurement) */
    __IO uint32_t AFE_ADC_OFFSET_TIA;            /*!< ADC Offset (TIA Measurement) */
    __IO uint32_t AFE_ADC_GAIN_TEMP_SENS;        /*!< ADC Gain (Temperature Sensor Measurement) */
    __IO uint32_t AFE_ADC_OFFSET_TEMP_SENS;      /*!< ADC Offset (Temperature Sensor Measurement) */
    __I  uint8_t                  RESERVED5[4];
    __IO uint32_t AFE_ADC_GAIN_AUX;              /*!< ADC Gain (Aux Channel Measurement) */
    __IO uint32_t AFE_ADC_OFFSET_AUX;            /*!< ADC Offset (Aux Channel Measurement) */
    __IO uint32_t AFE_DAC_OFFSET_UNITY;          /*!< DAC Offset With Attenuator Disabled */
    __IO uint32_t AFE_DAC_OFFSET_ATTEN;          /*!< DAC Offset With Attenuator Enabled */
    __IO uint32_t AFE_DAC_GAIN;                  /*!< DAC Gain */
    __IO uint32_t AFE_REF_TRIM0;                 /*!< Precision Reference Trim 0 */
    __IO uint32_t AFE_REF_TRIM1;                 /*!< Precision Reference Trim 1 */
    __IO uint32_t AFE_ALDO_TRIM;                 /*!< Analog LDO Trim */
    __IO uint32_t AFE_DAC_TRIM;                  /*!< DAC Trim */
    __IO uint32_t AFE_INAMP_TRIM;                /*!< INAMP Trim */
    __IO uint32_t AFE_EXBUF_TRIM;                /*!< Excitation Buffer Trim */
    __IO uint32_t AFE_TEMP_SENS_TRIM;            /*!< Temperature Sensor Trim */
} ADI_AFE_TypeDef;


/*!
 * \struct ADI_CT_TypeDef
 * \brief  Captouch Register Map
*/
typedef struct
{
    __IO uint32_t CT_CDC_PWR;                    /*!< Power CDC control */
    __IO uint32_t CT_CFG1;                       /*!< capTouch Control Configuration Register 1 */
    __IO uint32_t CT_CFG2;                       /*!< capTouch Control Configuration Register 2 */
    __IO uint32_t CT_CFG3;                       /*!< AFE Control Configuration Register 3 */
    __IO uint32_t CT_MEAS_SEL;                   /*!< Capacitance measurement stage selection */
    __IO uint32_t CT_BASELINE_CTRL;              /*!< Baseline control settings */
    __IO uint32_t CT_AVG;                        /*!< Setup of the rank-and-average filtering */
    __IO uint32_t CT_CAL_EN;                     /*!< Enable calibration for measurement stages */
    __IO uint32_t CT_TOUCH_CFG1;                 /*!< Touch Detection Thresholds */
    __IO uint32_t CT_TOUCH_CFG2;                 /*!< Touch Detection time-out */
    __IO uint32_t CT_RELEASE_CFG1;               /*!< Release Detection Thresholds */
    __IO uint32_t CT_RELEASE_CFG2;               /*!< Release Detection time-out */
    __IO uint32_t CT_IEN;                        /*!< Interrupt Enable */
    __IO uint32_t CT_INT;                        /*!< Primary Interrupt Register */
    __IO uint32_t CT_OFFS_HSTAT;                 /*!< Stage offset high interrupt status */
    __IO uint32_t CT_OFFS_LSTAT;                 /*!< Stage offset low interrupt status */
    __IO uint32_t CT_PROX_STAT;                  /*!< Stage proximity status */
    __IO uint32_t CT_FPROX_STAT;                 /*!< Stage fast proximity status */
    __IO uint32_t CT_TOUCH_STAT;                 /*!< Stage touch/release status */
    __IO uint32_t CT_STAGE0_CFG;                 /*!< Stage 0 configuration */
    __IO uint32_t CT_STAGE1_CFG;                 /*!< Stage 1 configuration */
    __IO uint32_t CT_STAGE2_CFG;                 /*!< Stage 2 configuration */
    __IO uint32_t CT_STAGE3_CFG;                 /*!< Stage 3 configuration */
    __IO uint32_t CT_STAGE4_CFG;                 /*!< Stage 4 configuration */
    __IO uint32_t CT_STAGE5_CFG;                 /*!< Stage 5 configuration */
    __IO uint32_t CT_STAGE6_CFG;                 /*!< Stage 6 configuration */
    __IO uint32_t CT_STAGE7_CFG;                 /*!< Stage 7 configuration */
    __IO uint32_t CT_STAGE8_CFG;                 /*!< Stage 8 configuration */
    __IO uint32_t CT_STAGE9_CFG;                 /*!< Stage 9 configuration */
    __IO uint32_t CT_STAGE10_CFG;                /*!< Stage 10 configuration */
    __IO uint32_t CT_STAGE11_CFG;                /*!< Stage 11 configuration */
    __IO uint32_t CT_STAGE12_CFG;                /*!< Stage 12 configuration */
    __IO uint32_t CT_STAGE13_CFG;                /*!< Stage 13 configuration */
    __IO uint32_t CT_STAGE14_CFG;                /*!< Stage 14 configuration */
    __IO uint32_t CT_STAGE15_CFG;                /*!< Stage 15 configuration */
    __IO uint32_t CT_SENSOR_THR_CFG0;            /*!< Stage 0 and Stage 1 Sensor Threshold */
    __IO uint32_t CT_SENSOR_THR_CFG1;            /*!< Stage 2 and Stage 3 Sensor Threshold */
    __IO uint32_t CT_SENSOR_THR_CFG2;            /*!< Stage 4 and Stage 5 Sensor Threshold */
    __IO uint32_t CT_SENSOR_THR_CFG3;            /*!< Stage 6 and Stage 7 Sensor Threshold */
    __IO uint32_t CT_SENSOR_THR_CFG4;            /*!< Stage 8 and Stage 9 Sensor Threshold */
    __IO uint32_t CT_SENSOR_THR_CFG5;            /*!< Stage 10 and Stage 11 Sensor Threshold */
    __IO uint32_t CT_SENSOR_THR_CFG6;            /*!< Stage 12 and Stage 13 Sensor Threshold */
    __IO uint32_t CT_SENSOR_THR_CFG7;            /*!< Stage 14 and Stage 15 Sensor Threshold */
    __I  uint32_t CT_CDC_RES0;                   /*!< Stage 0 and Stage 1 Results */
    __I  uint32_t CT_CDC_RES1;                   /*!< Stage 2 and Stage 3 Results */
    __I  uint32_t CT_CDC_RES2;                   /*!< Stage 4 and Stage 5 Results */
    __I  uint32_t CT_CDC_RES3;                   /*!< Stage 6 and Stage 7 Results */
    __I  uint32_t CT_CDC_RES4;                   /*!< Stage 8 and Stage 9 Results */
    __I  uint32_t CT_CDC_RES5;                   /*!< Stage 10 and Stage 11 Results */
    __I  uint32_t CT_CDC_RES6;                   /*!< Stage 12 and Stage 13 Results */
    __I  uint32_t CT_CDC_RES7;                   /*!< Stage 14 and Stage 15 Results */
    __I  uint32_t CT_BASELINE0;                  /*!< Stage 0 Fast Filter and Baseline Results */
    __I  uint32_t CT_BASELINE1;                  /*!< Stage 1 Fast Filter and Baseline Results */
    __I  uint32_t CT_BASELINE2;                  /*!< Stage 2 Fast Filter and Baseline Results */
    __I  uint32_t CT_BASELINE3;                  /*!< Stage 3 Fast Filter and Baseline Results */
    __I  uint32_t CT_BASELINE4;                  /*!< Stage 4 Fast Filter and Baseline Results */
    __I  uint32_t CT_BASELINE5;                  /*!< Stage 5 Fast Filter and Baseline Results */
    __I  uint32_t CT_BASELINE6;                  /*!< Stage 6 Fast Filter and Baseline Results */
    __I  uint32_t CT_BASELINE7;                  /*!< Stage 7 Fast Filter and Baseline Results */
    __I  uint32_t CT_BASELINE8;                  /*!< Stage 8 Fast Filter and Baseline Results */
    __I  uint32_t CT_BASELINE9;                  /*!< Stage 9 Fast Filter and Baseline Results */
    __I  uint32_t CT_BASELINE10;                 /*!< Stage 10 Fast Filter and Baseline Results */
    __I  uint32_t CT_BASELINE11;                 /*!< Stage 11 Fast Filter and Baseline Results */
    __I  uint32_t CT_BASELINE12;                 /*!< Stage 12 Fast Filter and Baseline Results */
    __I  uint32_t CT_BASELINE13;                 /*!< Stage 13 Fast Filter and Baseline Results */
    __I  uint32_t CT_BASELINE14;                 /*!< Stage 14 Fast Filter and Baseline Results */
    __I  uint32_t CT_BASELINE15;                 /*!< Stage 15 Fast Filter and Baseline Results */
    __I  uint32_t CT_PK2PK0;                     /*!< Stage 0 and Stage 1 Peak-to-Peak Noise Results */
    __I  uint32_t CT_PK2PK1;                     /*!< Stage 2 and Stage 3 Peak-to-Peak Noise Results */
    __I  uint32_t CT_PK2PK2;                     /*!< Stage 4 and Stage 5 Peak-to-Peak Noise Results */
    __I  uint32_t CT_PK2PK3;                     /*!< Stage 6 and Stage 7 Peak-to-Peak Noise Results */
    __I  uint32_t CT_PK2PK4;                     /*!< Stage 8 and Stage 9 Peak-to-Peak Noise Results */
    __I  uint32_t CT_PK2PK5;                     /*!< Stage 10 and Stage 11 Peak-to-Peak Noise Results */
    __I  uint32_t CT_PK2PK6;                     /*!< Stage 12 and Stage 13 Peak-to-Peak Noise Results */
    __I  uint32_t CT_PK2PK7;                     /*!< Stage 14 and Stage 15 Peak-to-Peak Noise Results */
} ADI_CT_TypeDef;


/*!
 * \struct ADI_USB_EP0_TypeDef
 * \brief  Endpoint 0
*/
typedef struct
{
    __I  uint8_t                  RESERVED0[2];
    union {
    __IO uint16_t CSR_P;                         /*!< EP0 Configuration and Status (Peripheral) Register */
    __IO uint16_t CSR_H;                         /*!< EP0 Configuration and Status (Host) Register */
    };
    __I  uint8_t                  RESERVED1[4];
    __I  uint16_t CNT;                           /*!< EP0 Number of Received Bytes Register */
    __I  uint8_t                  RESERVED2[5];
    __I   uint8_t CFGDATA;                       /*!< EP0 Configuration Information Register */
} ADI_USB_EP0_TypeDef;


/*!
 * \struct ADI_USB_DMA_TypeDef
 * \brief  DMA Block (1-8)
*/
typedef struct
{
    __IO uint16_t CTL;                           /*!< DMA Channel n Control Register */
    __I  uint8_t                  RESERVED0[2];
    __IO uint32_t ADDR;                          /*!< DMA Channel n Address Register */
    __IO uint32_t CNT;                           /*!< DMA Channel n Count Register */
    __I  uint8_t                  RESERVED1[4];
} ADI_USB_DMA_TypeDef;


/*!
 * \struct ADI_USB_EP0I_TypeDef
 * \brief  Endpoint 0 Indexed Block
*/
typedef struct
{
    __I  uint8_t                  RESERVED0[2];
    union {
    __IO uint16_t CSR_P;                         /*!< EP0 Configuration and Status (Peripheral) Register */
    __IO uint16_t CSR_H;                         /*!< EP0 Configuration and Status (Host) Register */
    };
    __I  uint8_t                  RESERVED1[4];
    __I  uint16_t CNT;                           /*!< EP0 Number of Received Bytes Register */
    __I  uint8_t                  RESERVED2[5];
    __I   uint8_t CFGDATA;                       /*!< EP0 Configuration Information Register */
} ADI_USB_EP0I_TypeDef;


/*!
 * \struct ADI_USB_EPI_TypeDef
 * \brief  Endpoint Indexed Block
*/
typedef struct
{
    __IO uint16_t TXMAXP;                        /*!< EPn Transmit Maximum Packet Length Register */
    union {
    __IO uint16_t TXCSR_H;                       /*!< EPn Transmit Configuration and Status (Host) Register */
    __IO uint16_t TXCSR_P;                       /*!< EPn Transmit Configuration and Status (Peripheral) Register */
    };
    __IO uint16_t RXMAXP;                        /*!< EPn Receive Maximum Packet Length Register */
    union {
    __IO uint16_t RXCSR_H;                       /*!< EPn Receive Configuration and Status (Host) Register */
    __IO uint16_t RXCSR_P;                       /*!< EPn Receive Configuration and Status (Peripheral) Register */
    };
    __I  uint16_t RXCNT;                         /*!< EPn Number of Bytes Received Register */
    __I  uint8_t                  RESERVED0[5];
    __IO  uint8_t FIFOSIZE;                      /*!< FIFO size */
} ADI_USB_EPI_TypeDef;


/*!
 * \struct ADI_USB_EP_TypeDef
 * \brief  Endpoint Block
*/
typedef struct
{
    __IO uint16_t TXMAXP;                        /*!< EPn Transmit Maximum Packet Length Register */
    union {
    __IO uint16_t TXCSR_P;                       /*!< EPn Transmit Configuration and Status (Peripheral) Register */
    __IO uint16_t TXCSR_H;                       /*!< EPn Transmit Configuration and Status (Host) Register */
    };
    __IO uint16_t RXMAXP;                        /*!< EPn Receive Maximum Packet Length Register */
    union {
    __IO uint16_t RXCSR_P;                       /*!< EPn Receive Configuration and Status (Peripheral) Register */
    __IO uint16_t RXCSR_H;                       /*!< EPn Receive Configuration and Status (Host) Register */
    };
    __I  uint16_t RXCNT;                         /*!< EPn Number of Bytes Received Register */
    __I  uint8_t                  RESERVED0[5];
    __IO  uint8_t FIFOSIZE;                      /*!< FIFO size */
} ADI_USB_EP_TypeDef;


/*!
 * \struct ADI_USB_FIFO_TypeDef
 * \brief  FIFO Block (0 - 11)
*/
typedef struct
{
    union {
    __IO uint32_t FIFO;                          /*!< FIFO Word (32-Bit) Register */
    __IO uint16_t H;                             /*!< FIFO Half-Word (16-Bit) Register */
    __IO  uint8_t B;                             /*!< FIFO Byte (8-Bit) Register */
    };
} ADI_USB_FIFO_TypeDef;


/*!
 * \struct ADI_USB_TypeDef
 * \brief  Universal Serial Bus Controller
*/
typedef struct
{
    __IO  uint8_t FADDR;                         /*!< Function Address Register */
    __IO  uint8_t POWER;                         /*!< Power and Device Control Register */
    __I  uint16_t INTRTX;                        /*!< Transmit Interrupt Register */
    __I  uint16_t INTRRX;                        /*!< Receive Interrupt Register */
    __IO uint16_t INTRTXE;                       /*!< Transmit Interrupt Enable Register */
    __IO uint16_t INTRRXE;                       /*!< Receive Interrupt Enable Register */
    __I   uint8_t IRQ;                           /*!< Common Interrupts Register */
    __IO  uint8_t IEN;                           /*!< Common Interrupts Enable Register */
    __I  uint16_t FRAME;                         /*!< Frame Number Register */
    __IO  uint8_t INDEX;                         /*!< Index Register */
    __IO  uint8_t TESTMODE;                      /*!< Testmode Register */
    union {
    ADI_USB_EP0I_TypeDef EP0I;                   /*!< Endpoint 0 Indexed Block */
    ADI_USB_EPI_TypeDef EPI;                     /*!< Endpoint Indexed Block */
      };
    ADI_USB_FIFO_TypeDef FIFO[4];                /*!< FIFO Block (0 - 11) */
    __I  uint8_t                  RESERVED0[48];
    __IO  uint8_t DEV_CTL;                       /*!< Device Control Register */
    __IO  uint8_t MISC;                          /*!< Miscellaneous Register */
    __I  uint8_t                  RESERVED1[22];
    __I   uint8_t EPINFO;                        /*!< Endpoint Information Register */
    __I   uint8_t RAMINFO;                       /*!< RAM Information Register */
    __IO  uint8_t LINKINFO;                      /*!< Link Information Register */
    __I  uint8_t                  RESERVED2[2];
    __IO  uint8_t FS_EOF1;                       /*!< Full-Speed EOF 1 Register */
    __I  uint8_t                  RESERVED3;
    __IO  uint8_t SOFT_RST;                      /*!< Software Reset Register */
    __I  uint8_t                  RESERVED4[128];
    union {
    ADI_USB_EP0_TypeDef EP0;                     /*!< Endpoint 0 */
    ADI_USB_EP_TypeDef EP[4];                    /*!< Endpoint Block */
      };
    __I  uint8_t                  RESERVED5[192];
    __I   uint8_t DMA_IRQ;                       /*!< DMA Interrupt Register */
    __I  uint8_t                  RESERVED6[3];
    ADI_USB_DMA_TypeDef DMA[2];                  /*!< DMA Block (1-8) */
    __I  uint8_t                  RESERVED7[284];
    __IO uint16_t RXDPKTBUFDIS;                  /*!< RX Double Packet Buffer Disable for Endpoints 1 to 3 */
    __IO uint16_t TXDPKTBUFDIS;                  /*!< TX Double Packet Buffer Disable  for Endpoints 1 to 3 */
    __IO uint16_t CT_UCH;                        /*!< Chirp Timeout Register */
    __I  uint8_t                  RESERVED8[26];
    __IO uint16_t LPM_ATTR;                      /*!< LPM Attribute Register */
    __IO  uint8_t LPM_CTL;                       /*!< LPM Control Register */
    __IO  uint8_t LPM_IEN;                       /*!< LPM Interrupt Enable Register */
    __I   uint8_t LPM_IRQ;                       /*!< LPM Interrupt Status Register */
    __I  uint8_t                  RESERVED9[55];
    __IO uint16_t PHY_CTL;                       /*!< FS PHY Control */
    __I  uint16_t PHY_STAT;                      /*!< FS PHY Status */
    __I  uint8_t                 RESERVED10[16];
    __IO uint32_t RAM_ADDR;                      /*!< RAM Address Register */
    __IO uint32_t RAM_DATA;                      /*!< RAM Data Register */
} ADI_USB_TypeDef;


/*!
 * \struct ADI_NVIC_TypeDef
 * \brief
*/
typedef struct
{
    __I  uint8_t                  RESERVED0[4];
    __IO uint32_t INTNUM;                        /*!< Interrupt Control Type */
    __I  uint8_t                  RESERVED1[8];
    __IO uint32_t STKSTA;                        /*!< Systick Control and Status */
    __IO uint32_t STKLD;                         /*!< Systick Reload Value */
    __IO uint32_t STKVAL;                        /*!< Systick Current Value */
    __IO uint32_t STKCAL;                        /*!< Systick Calibration Value */
    __I  uint8_t                  RESERVED2[224];
    __IO uint32_t INTSETE0;                      /*!< IRQ0..31 Set_Enable */
    __IO uint32_t INTSETE1;                      /*!< IRQ32..63 Set_Enable */
    __I  uint8_t                  RESERVED3[120];
    __IO uint32_t INTCLRE0;                      /*!< IRQ0..31 Clear_Enable */
    __IO uint32_t INTCLRE1;                      /*!< IRQ32..63 Clear_Enable */
    __I  uint8_t                  RESERVED4[120];
    __IO uint32_t INTSETP0;                      /*!< IRQ0..31 Set_Pending */
    __IO uint32_t INTSETP1;                      /*!< IRQ32..63 Set_Pending */
    __I  uint8_t                  RESERVED5[120];
    __IO uint32_t INTCLRP0;                      /*!< IRQ0..31 Clear_Pending */
    __IO uint32_t INTCLRP1;                      /*!< IRQ32..63 Clear_Pending */
    __I  uint8_t                  RESERVED6[120];
    __IO uint32_t INTACT0;                       /*!< IRQ0..31 Active Bit */
    __IO uint32_t INTACT1;                       /*!< IRQ32..63 Active Bit */
    __I  uint8_t                  RESERVED7[248];
    __IO uint32_t INTPRI0;                       /*!< IRQ0..3 Priority */
    __IO uint32_t INTPRI1;                       /*!< IRQ4..7 Priority */
    __IO uint32_t INTPRI2;                       /*!< IRQ8..11 Priority */
    __IO uint32_t INTPRI3;                       /*!< IRQ12..15 Priority */
    __IO uint32_t INTPRI4;                       /*!< IRQ16..19 Priority */
    __IO uint32_t INTPRI5;                       /*!< IRQ20..23 Priority */
    __IO uint32_t INTPRI6;                       /*!< IRQ24..27 Priority */
    __IO uint32_t INTPRI7;                       /*!< IRQ28..31 Priority */
    __IO uint32_t INTPRI8;                       /*!< IRQ32..35 Priority */
    __IO uint32_t INTPRI9;                       /*!< IRQ36..39 Priority */
    __IO uint32_t INTPRI10;                      /*!< IRQ40..43 Priority */
    __I  uint8_t                  RESERVED8[2260];
    __IO uint32_t INTCPID;                       /*!< CPUID Base */
    __IO uint32_t INTSTA;                        /*!< Interrupt Control State */
    __IO uint32_t INTVEC;                        /*!< Vector Table Offset */
    __IO uint32_t INTAIRC;                       /*!< Application Interrupt/Reset Control */
    __IO uint16_t INTCON0;                       /*!< System Control */
    __I  uint8_t                  RESERVED9[2];
    __IO uint32_t INTCON1;                       /*!< Configuration Control */
    __IO uint32_t INTSHPRIO0;                    /*!< System Handlers 4-7 Priority */
    __IO uint32_t INTSHPRIO1;                    /*!< System Handlers 8-11 Priority */
    __IO uint32_t INTSHPRIO3;                    /*!< System Handlers 12-15 Priority */
    __IO uint32_t INTSHCSR;                      /*!< System Handler Control and State */
    __IO uint32_t INTCFSR;                       /*!< Configurable Fault Status */
    __IO uint32_t INTHFSR;                       /*!< Hard Fault Status */
    __IO uint32_t INTDFSR;                       /*!< Debug Fault Status */
    __IO uint32_t INTMMAR;                       /*!< Mem Manage Address */
    __IO uint32_t INTBFAR;                       /*!< Bus Fault Address */
    __IO uint32_t INTAFSR;                       /*!< Auxiliary Fault Status */
    __IO uint32_t INTPFR0;                       /*!< Processor Feature Register 0 */
    __IO uint32_t INTPFR1;                       /*!< Processor Feature Register 1 */
    __IO uint32_t INTDFR0;                       /*!< Debug Feature Register 0 */
    __IO uint32_t INTAFR0;                       /*!< Auxiliary Feature Register 0 */
    __IO uint32_t INTMMFR0;                      /*!< Memory Model Feature Register 0 */
    __IO uint32_t INTMMFR1;                      /*!< Memory Model Feature Register 1 */
    __IO uint32_t INTMMFR2;                      /*!< Memory Model Feature Register 2 */
    __IO uint32_t INTMMFR3;                      /*!< Memory Model Feature Register 3 */
    __IO uint32_t INTISAR0;                      /*!< ISA Feature Register 0 */
    __IO uint32_t INTISAR1;                      /*!< ISA Feature Register 1 */
    __IO uint32_t INTISAR2;                      /*!< ISA Feature Register 2 */
    __IO uint32_t INTISAR3;                      /*!< ISA Feature Register 3 */
    __IO uint32_t INTISAR4;                      /*!< ISA Feature Register 4 */
    __I  uint8_t                 RESERVED10[396];
    __IO uint32_t INTTRGI;                       /*!< Software Trigger Interrupt Register */
    __I  uint8_t                 RESERVED11[204];
    __IO uint32_t INTPID4;                       /*!< Peripheral Identification Register 4 */
    __IO uint32_t INTPID5;                       /*!< Peripheral Identification Register 5 */
    __IO uint32_t INTPID6;                       /*!< Peripheral Identification Register 6 */
    __IO uint32_t INTPID7;                       /*!< Peripheral Identification Register 7 */
    __IO uint32_t INTPID0;                       /*!< Peripheral Identification Bits7:0 */
    __IO uint32_t INTPID1;                       /*!< Peripheral Identification Bits15:8 */
    __IO uint32_t INTPID2;                       /*!< Peripheral Identification Bits16:23 */
    __IO uint32_t INTPID3;                       /*!< Peripheral Identification Bits24:31 */
    __IO uint32_t INTCID0;                       /*!< Component Identification Bits7:0 */
    __IO uint32_t INTCID1;                       /*!< Component Identification Bits15:8 */
    __IO uint32_t INTCID2;                       /*!< Component Identification Bits16:23 */
    __IO uint32_t INTCID3;                       /*!< Component Identification Bits24:31 */
} ADI_NVIC_TypeDef;

/** Peripheral Memory Map Declarations
  */
#define ADI_GPT0_BASE                  0x40000000    /*!<  Base address of GPT0 */
#define ADI_GPT1_BASE                  0x40000400    /*!<  Base address of GPT1 */
#define ADI_GPT2_BASE                  0x40000800    /*!<  Base address of GPT2 */
#define ADI_PWR_BASE                   0x40002400    /*!<  Base address of PWR */
#define ADI_WUT_BASE                   0x40002500    /*!<  Base address of WUT */
#define ADI_WDT_BASE                   0x40002580    /*!<  Base address of WDT */
#define ADI_RTC_BASE                   0x40002600    /*!<  Base address of RTC */
#define ADI_I2C_BASE                   0x40003000    /*!<  Base address of I2C */
#define ADI_SPI0_BASE                  0x40004000    /*!<  Base address of SPI0 */
#define ADI_SPI1_BASE                  0x40004400    /*!<  Base address of SPI1 */
#define ADI_UART_BASE                  0x40005000    /*!<  Base address of UART */
#define ADI_I2S_BASE                   0x40005800    /*!<  Base address of I2S */
#define ADI_BEEP_BASE                  0x40005c00    /*!<  Base address of BEEP */
#define ADI_RNG_BASE                   0x40006000    /*!<  Base address of RNG */
#define ADI_LCD_BASE                   0x40008000    /*!<  Base address of LCD */
#define ADI_DMA_BASE                   0x40010000    /*!<  Base address of DMA */
#define ADI_FEE0_BASE                  0x40018000    /*!<  Base address of FEE0 */
#define ADI_FEE1_BASE                  0x40018100    /*!<  Base address of FEE1 */
#define ADI_GPF_BASE                   0x4001c000    /*!<  Base address of GPF */
#define ADI_GPIO0_BASE                 0x40020000    /*!<  Base address of GPIO0 */
#define ADI_GPIO1_BASE                 0x40020040    /*!<  Base address of GPIO1 */
#define ADI_GPIO2_BASE                 0x40020080    /*!<  Base address of GPIO2 */
#define ADI_GPIO3_BASE                 0x400200c0    /*!<  Base address of GPIO3 */
#define ADI_GPIO4_BASE                 0x40020100    /*!<  Base address of GPIO4 */
#define ADI_SPIH_BASE                  0x40024000    /*!<  Base address of SPIH */
#define ADI_SYSCLK_BASE                0x40028000    /*!<  Base address of SYSCLK */
#define ADI_BUSM_BASE                  0x40028040    /*!<  Base address of BUSM */
#define ADI_CRC_BASE                   0x4002c000    /*!<  Base address of CRC */
#define ADI_PDI_BASE                   0x40030000    /*!<  Base address of PDI */
#define ADI_AFE_BASE                   0x40080000    /*!<  Base address of AFE */
#define ADI_CT_BASE                    0x40084000    /*!<  Base address of CT */
#define ADI_USB_BASE                   0x400a0000    /*!<  Base address of USB */
#define ADI_NVIC_BASE                  0xe000e000    /*!<  Base address of NVIC */

/* @} */

/*******************************************************************************
 *    Peripheral Pointer Declarations
 *******************************************************************************
 * @defgroup PPTRDEC Peripheral Pointer Declarations
 * \addtogroup PPTRDEC
 * @{
 */
#define pADI_GPT0                      ((ADI_GPT_TypeDef      *) ADI_GPT0_BASE       ) /*!<  Pointer to General Purpose Timer (GPT0) */
#define pADI_GPT1                      ((ADI_GPT_TypeDef      *) ADI_GPT1_BASE       ) /*!<  Pointer to General Purpose Timer (GPT1) */
#define pADI_GPT2                      ((ADI_GPT_TypeDef      *) ADI_GPT2_BASE       ) /*!<  Pointer to General Purpose Timer (GPT2) */
#define pADI_PWR                       ((ADI_PWR_TypeDef      *) ADI_PWR_BASE        ) /*!<  Pointer to  (PWR) */
#define pADI_WUT                       ((ADI_WUT_TypeDef      *) ADI_WUT_BASE        ) /*!<  Pointer to WUT (WUT) */
#define pADI_WDT                       ((ADI_WDT_TypeDef      *) ADI_WDT_BASE        ) /*!<  Pointer to WDT (WDT) */
#define pADI_RTC                       ((ADI_RTC_TypeDef      *) ADI_RTC_BASE        ) /*!<  Pointer to Real-Time Clock (RTC) */
#define pADI_I2C                       ((ADI_I2C_TypeDef      *) ADI_I2C_BASE        ) /*!<  Pointer to I2C Master/Slave (I2C) */
#define pADI_SPI0                      ((ADI_SPI_TypeDef      *) ADI_SPI0_BASE       ) /*!<  Pointer to SPI (SPI0) */
#define pADI_SPI1                      ((ADI_SPI_TypeDef      *) ADI_SPI1_BASE       ) /*!<  Pointer to SPI (SPI1) */
#define pADI_UART                      ((ADI_UART_TypeDef     *) ADI_UART_BASE       ) /*!<  Pointer to UART (UART) */
#define pADI_I2S                       ((ADI_I2S_TypeDef      *) ADI_I2S_BASE        ) /*!<  Pointer to I2S serial port (I2S) */
#define pADI_BEEP                      ((ADI_BEEP_TypeDef     *) ADI_BEEP_BASE       ) /*!<  Pointer to Beeper Driver (BEEP) */
#define pADI_RNG                       ((ADI_RNG_TypeDef      *) ADI_RNG_BASE        ) /*!<  Pointer to Random Number Generator (RNG) */
#define pADI_LCD                       ((ADI_LCD_TypeDef      *) ADI_LCD_BASE        ) /*!<  Pointer to LCD Controller (LCD) */
#define pADI_DMA                       ((ADI_DMA_TypeDef      *) ADI_DMA_BASE        ) /*!<  Pointer to DMA (DMA) */
#define pADI_FEE0                      ((ADI_FEE_TypeDef      *) ADI_FEE0_BASE       ) /*!<  Pointer to Flash Controller1 (FEE0) */
#define pADI_FEE1                      ((ADI_FEE_TypeDef      *) ADI_FEE1_BASE       ) /*!<  Pointer to Flash Controller1 (FEE1) */
#define pADI_GPF                       ((ADI_FEE_TypeDef      *) ADI_GPF_BASE        ) /*!<  Pointer to General Purpose Flash Controller (GPF) */
#define pADI_GPIO0                     ((ADI_GPIO_TypeDef     *) ADI_GPIO0_BASE      ) /*!<  Pointer to GPIO (GPIO0) */
#define pADI_GPIO1                     ((ADI_GPIO_TypeDef     *) ADI_GPIO1_BASE      ) /*!<  Pointer to GPIO (GPIO1) */
#define pADI_GPIO2                     ((ADI_GPIO_TypeDef     *) ADI_GPIO2_BASE      ) /*!<  Pointer to GPIO (GPIO2) */
#define pADI_GPIO3                     ((ADI_GPIO_TypeDef     *) ADI_GPIO3_BASE      ) /*!<  Pointer to GPIO (GPIO3) */
#define pADI_GPIO4                     ((ADI_GPIO_TypeDef     *) ADI_GPIO4_BASE      ) /*!<  Pointer to GPIO (GPIO4) */
#define pADI_SPIH                      ((ADI_SPI_TypeDef      *) ADI_SPIH_BASE       ) /*!<  Pointer to SPI (SPIH) */
#define pADI_SYSCLK                    ((ADI_SYSCLK_TypeDef   *) ADI_SYSCLK_BASE     ) /*!<  Pointer to Clock gating and other settings (SYSCLK) */
#define pADI_BUSM                      ((ADI_BUSM_TypeDef     *) ADI_BUSM_BASE       ) /*!<  Pointer to changed from 4002800 (BUSM) */
#define pADI_CRC                       ((ADI_CRC_TypeDef      *) ADI_CRC_BASE        ) /*!<  Pointer to CRC (CRC) */
#define pADI_PDI                       ((ADI_PDI_TypeDef      *) ADI_PDI_BASE        ) /*!<  Pointer to PDI (PDI) */
#define pADI_AFE                       ((ADI_AFE_TypeDef      *) ADI_AFE_BASE        ) /*!<  Pointer to AFE Register Map (AFE) */
#define pADI_CT                        ((ADI_CT_TypeDef       *) ADI_CT_BASE         ) /*!<  Pointer to Captouch Register Map (CT) */
#define pADI_USB                       ((ADI_USB_TypeDef      *) ADI_USB_BASE       ) /*!<  Pointer to Universal Serial Bus Controller (USB) */
#define pADI_NVIC                      ((ADI_NVIC_TypeDef     *) ADI_NVIC_BASE       ) /*!<  Pointer to  (NVIC) */

/* @} */

#endif

