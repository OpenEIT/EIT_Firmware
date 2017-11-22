/* =========================================================================

     Project      :   ADuCM350
     File         :   cdefADuCM350.h
     Description  :   C register and bitfield definitions

     Date         :   11-07-2012


     Copyright (c) 2011-2013 Analog Devices, Inc.  All Rights Reserved.
     This software is proprietary and confidential to Analog Devices, Inc. and
     its licensors.

     This file was auto-generated. Do not make local changes to this file.

   ========================================================================= */
#ifndef _CDEF_ADuCM350_H
#define _CDEF_ADuCM350_H

#include <stdint.h>
#include <defADuCM350.h>

#ifdef _MISRA_RULES
#pragma diag(push)
#pragma diag(suppress:misra_rule_5_7:"ADI header will re-use identifiers")
#pragma diag(suppress:misra_rule_6_3:"ADI header allows use of basic types")
#endif /* _MISRA_RULES */


/* =========================================================================
       GPT0
   ========================================================================= */
#define pREG_GPT0_GPTLD                  ((volatile uint16_t *)REG_GPT0_GPTLD)                   /* GPT0 16-bit load value */
#define pREG_GPT0_GPTVAL                 ((volatile uint16_t *)REG_GPT0_GPTVAL)                  /* GPT0 16-bit timer value */
#define pREG_GPT0_GPTCON                 ((volatile uint16_t *)REG_GPT0_GPTCON)                  /* GPT0 Control */
#define pREG_GPT0_GPTCLRI                ((volatile uint16_t *)REG_GPT0_GPTCLRI)                 /* GPT0 Clear Interrupt */
#define pREG_GPT0_GPTCAP                 ((volatile uint16_t *)REG_GPT0_GPTCAP)                  /* GPT0 Capture */
#define pREG_GPT0_GPTALD                 ((volatile uint16_t *)REG_GPT0_GPTALD)                  /* GPT0 16-bit load value, asynchronous */
#define pREG_GPT0_GPTAVAL                ((volatile uint16_t *)REG_GPT0_GPTAVAL)                 /* GPT0 16-bit timer value, asynchronous */
#define pREG_GPT0_GPTSTA                 ((volatile uint16_t *)REG_GPT0_GPTSTA)                  /* GPT0 Status */
#define pREG_GPT0_GPTPCON                ((volatile uint16_t *)REG_GPT0_GPTPCON)                 /* GPT0 PWM Control Register */
#define pREG_GPT0_GPTPMAT                ((volatile uint16_t *)REG_GPT0_GPTPMAT)                 /* GPT0 PWM Match Value */

/* =========================================================================
       GPT1
   ========================================================================= */
#define pREG_GPT1_GPTLD                  ((volatile uint16_t *)REG_GPT1_GPTLD)                   /* GPT1 16-bit load value */
#define pREG_GPT1_GPTVAL                 ((volatile uint16_t *)REG_GPT1_GPTVAL)                  /* GPT1 16-bit timer value */
#define pREG_GPT1_GPTCON                 ((volatile uint16_t *)REG_GPT1_GPTCON)                  /* GPT1 Control */
#define pREG_GPT1_GPTCLRI                ((volatile uint16_t *)REG_GPT1_GPTCLRI)                 /* GPT1 Clear Interrupt */
#define pREG_GPT1_GPTCAP                 ((volatile uint16_t *)REG_GPT1_GPTCAP)                  /* GPT1 Capture */
#define pREG_GPT1_GPTALD                 ((volatile uint16_t *)REG_GPT1_GPTALD)                  /* GPT1 16-bit load value, asynchronous */
#define pREG_GPT1_GPTAVAL                ((volatile uint16_t *)REG_GPT1_GPTAVAL)                 /* GPT1 16-bit timer value, asynchronous */
#define pREG_GPT1_GPTSTA                 ((volatile uint16_t *)REG_GPT1_GPTSTA)                  /* GPT1 Status */
#define pREG_GPT1_GPTPCON                ((volatile uint16_t *)REG_GPT1_GPTPCON)                 /* GPT1 PWM Control Register */
#define pREG_GPT1_GPTPMAT                ((volatile uint16_t *)REG_GPT1_GPTPMAT)                 /* GPT1 PWM Match Value */

/* =========================================================================
       GPT2
   ========================================================================= */
#define pREG_GPT2_GPTLD                  ((volatile uint16_t *)REG_GPT2_GPTLD)                   /* GPT2 16-bit load value */
#define pREG_GPT2_GPTVAL                 ((volatile uint16_t *)REG_GPT2_GPTVAL)                  /* GPT2 16-bit timer value */
#define pREG_GPT2_GPTCON                 ((volatile uint16_t *)REG_GPT2_GPTCON)                  /* GPT2 Control */
#define pREG_GPT2_GPTCLRI                ((volatile uint16_t *)REG_GPT2_GPTCLRI)                 /* GPT2 Clear Interrupt */
#define pREG_GPT2_GPTCAP                 ((volatile uint16_t *)REG_GPT2_GPTCAP)                  /* GPT2 Capture */
#define pREG_GPT2_GPTALD                 ((volatile uint16_t *)REG_GPT2_GPTALD)                  /* GPT2 16-bit load value, asynchronous */
#define pREG_GPT2_GPTAVAL                ((volatile uint16_t *)REG_GPT2_GPTAVAL)                 /* GPT2 16-bit timer value, asynchronous */
#define pREG_GPT2_GPTSTA                 ((volatile uint16_t *)REG_GPT2_GPTSTA)                  /* GPT2 Status */
#define pREG_GPT2_GPTPCON                ((volatile uint16_t *)REG_GPT2_GPTPCON)                 /* GPT2 PWM Control Register */
#define pREG_GPT2_GPTPMAT                ((volatile uint16_t *)REG_GPT2_GPTPMAT)                 /* GPT2 PWM Match Value */


/* =========================================================================
       PWR
   ========================================================================= */
#define pREG_PWR_PWRMOD                  ((volatile uint16_t *)REG_PWR_PWRMOD)                   /* PWR Power modes */
#define pREG_PWR_PWRKEY                  ((volatile uint16_t *)REG_PWR_PWRKEY)                   /* PWR Key protection for PWRMOD */
#define pREG_PWR_PSMCON                  ((volatile uint16_t *)REG_PWR_PSMCON)                   /* PWR PSM Configuration */
#define pREG_PWR_OSCKEY                  ((volatile uint16_t *)REG_PWR_OSCKEY)                   /* PWR Key protection for OSCCTRL */
#define pREG_PWR_OSCCTRL                 ((volatile uint16_t *)REG_PWR_OSCCTRL)                  /* PWR Oscillator control */
#define pREG_PWR_EI0CFG                  ((volatile uint16_t *)REG_PWR_EI0CFG)                   /* PWR External Interrupt configuration 0 */
#define pREG_PWR_EI1CFG                  ((volatile uint16_t *)REG_PWR_EI1CFG)                   /* PWR External Interrupt configuration 1 */
#define pREG_PWR_EI2CFG                  ((volatile uint16_t *)REG_PWR_EI2CFG)                   /* PWR External Interrupt configuration 2 */
#define pREG_PWR_EICLR                   ((volatile uint16_t *)REG_PWR_EICLR)                    /* PWR External Interrupt clear */
#define pREG_PWR_NMICLR                  ((volatile uint16_t *)REG_PWR_NMICLR)                   /* PWR Non-maskable interrupt clear */
#define pREG_PWR_USBWKSTAT               ((volatile uint16_t *)REG_PWR_USBWKSTAT)                /* PWR USB Wakeup Status */
#define pREG_PWR_RSTSTA                  ((volatile uint16_t *)REG_PWR_RSTSTA)                   /* PWR Reset status */
#define pREG_PWR_VCCMCON                 ((volatile uint16_t *)REG_PWR_VCCMCON)                  /* PWR VCCM Control and Status */
#define pREG_PWR_VBACKCON                ((volatile uint16_t *)REG_PWR_VBACKCON)                 /* PWR VBACK control and status */


/* =========================================================================
       WUT
   ========================================================================= */
#define pREG_WUT_T2VAL0                  ((volatile uint16_t *)REG_WUT_T2VAL0)                   /* WUT Current count value - LS halfword. */
#define pREG_WUT_T2VAL1                  ((volatile uint16_t *)REG_WUT_T2VAL1)                   /* WUT Current count value - MS halfword */
#define pREG_WUT_T2CON                   ((volatile uint16_t *)REG_WUT_T2CON)                    /* WUT Control */
#define pREG_WUT_T2INC                   ((volatile uint16_t *)REG_WUT_T2INC)                    /* WUT 12-bit interval for wakeup field A */
#define pREG_WUT_T2WUFB0                 ((volatile uint16_t *)REG_WUT_T2WUFB0)                  /* WUT Wakeup field B - LS halfword */
#define pREG_WUT_T2WUFB1                 ((volatile uint16_t *)REG_WUT_T2WUFB1)                  /* WUT Wakeup field B - MS halfword */
#define pREG_WUT_T2WUFC0                 ((volatile uint16_t *)REG_WUT_T2WUFC0)                  /* WUT Wakeup field C - LS halfword */
#define pREG_WUT_T2WUFC1                 ((volatile uint16_t *)REG_WUT_T2WUFC1)                  /* WUT Wakeup field C - MS halfword */
#define pREG_WUT_T2WUFD0                 ((volatile uint16_t *)REG_WUT_T2WUFD0)                  /* WUT Wakeup field D - LS halfword */
#define pREG_WUT_T2WUFD1                 ((volatile uint16_t *)REG_WUT_T2WUFD1)                  /* WUT Wakeup field D - MS halfword */
#define pREG_WUT_T2IEN                   ((volatile uint16_t *)REG_WUT_T2IEN)                    /* WUT Interrupt enable */
#define pREG_WUT_T2STA                   ((volatile uint16_t *)REG_WUT_T2STA)                    /* WUT Status */
#define pREG_WUT_T2CLRI                  ((volatile uint16_t *)REG_WUT_T2CLRI)                   /* WUT Clear interrupts */
#define pREG_WUT_WUTVAL_LOW              ((volatile uint16_t *)REG_WUT_WUTVAL_LOW)               /* WUT Unsynchronized lower 16 bits of WU Timer counter value. */
#define pREG_WUT_WUTVAL_HIGH             ((volatile uint16_t *)REG_WUT_WUTVAL_HIGH)              /* WUT Unsynchronized upper 16 bits of WU Timer counter value. */
#define pREG_WUT_T2WUFA0                 ((volatile uint16_t *)REG_WUT_T2WUFA0)                  /* WUT Wakeup field A - LS halfword */
#define pREG_WUT_T2WUFA1                 ((volatile uint16_t *)REG_WUT_T2WUFA1)                  /* WUT Wakeup field A - MS halfword */


/* =========================================================================
       WDT
   ========================================================================= */
#define pREG_WDT_T3LD                    ((volatile uint16_t *)REG_WDT_T3LD)                     /* WDT Load value */
#define pREG_WDT_T3VAL                   ((volatile uint16_t *)REG_WDT_T3VAL)                    /* WDT Current count value */
#define pREG_WDT_T3CON                   ((volatile uint16_t *)REG_WDT_T3CON)                    /* WDT Control */
#define pREG_WDT_T3CLRI                  ((volatile uint16_t *)REG_WDT_T3CLRI)                   /* WDT Clear interrupt */
#define pREG_WDT_T3STA                   ((volatile uint16_t *)REG_WDT_T3STA)                    /* WDT Status */


/* =========================================================================
       RTC
   ========================================================================= */
#define pREG_RTC_RTCCR                   ((volatile uint32_t *)REG_RTC_RTCCR)                    /* RTC RTC Control */
#define pREG_RTC_RTCSR0                  ((volatile uint16_t *)REG_RTC_RTCSR0)                   /* RTC RTC Status 0 */
#define pREG_RTC_RTCSR1                  ((volatile uint16_t *)REG_RTC_RTCSR1)                   /* RTC RTC Status 1 */
#define pREG_RTC_RTCCNT0                 ((volatile uint16_t *)REG_RTC_RTCCNT0)                  /* RTC RTC Count 0 */
#define pREG_RTC_RTCCNT1                 ((volatile uint16_t *)REG_RTC_RTCCNT1)                  /* RTC RTC Count 1 */
#define pREG_RTC_RTCALM0                 ((volatile uint16_t *)REG_RTC_RTCALM0)                  /* RTC RTC Alarm 0 */
#define pREG_RTC_RTCALM1                 ((volatile uint16_t *)REG_RTC_RTCALM1)                  /* RTC RTC Alarm 1 */
#define pREG_RTC_RTCTRM                  ((volatile uint16_t *)REG_RTC_RTCTRM)                   /* RTC RTC Trim */
#define pREG_RTC_RTCGWY                  ((volatile uint16_t *)REG_RTC_RTCGWY)                   /* RTC RTC Gateway */


/* =========================================================================
       I2C
   ========================================================================= */
#define pREG_I2C_I2CMCON                 ((volatile uint16_t *)REG_I2C_I2CMCON)                  /* I2C Master control */
#define pREG_I2C_I2CMSTA                 ((volatile uint16_t *)REG_I2C_I2CMSTA)                  /* I2C Master status */
#define pREG_I2C_I2CMRX                  ((volatile uint16_t *)REG_I2C_I2CMRX)                   /* I2C Master receive data */
#define pREG_I2C_I2CMTX                  ((volatile uint16_t *)REG_I2C_I2CMTX)                   /* I2C Master transmit data */
#define pREG_I2C_I2CMRXCNT               ((volatile uint16_t *)REG_I2C_I2CMRXCNT)                /* I2C Master receive data count */
#define pREG_I2C_I2CMCRXCNT              ((volatile uint16_t *)REG_I2C_I2CMCRXCNT)               /* I2C Master current receive data count */
#define pREG_I2C_I2CADR1                 ((volatile uint16_t *)REG_I2C_I2CADR1)                  /* I2C 1st master address byte */
#define pREG_I2C_I2CADR2                 ((volatile uint16_t *)REG_I2C_I2CADR2)                  /* I2C 2nd master address byte */
#define pREG_I2C_I2CBYT                  ((volatile uint16_t *)REG_I2C_I2CBYT)                   /* I2C Start byte */
#define pREG_I2C_I2CDIV                  ((volatile uint16_t *)REG_I2C_I2CDIV)                   /* I2C Serial clock period divisor */
#define pREG_I2C_I2CSCON                 ((volatile uint16_t *)REG_I2C_I2CSCON)                  /* I2C Slave control */
#define pREG_I2C_I2CSSTA                 ((volatile uint16_t *)REG_I2C_I2CSSTA)                  /* I2C Slave I2C Status/Error/IRQ */
#define pREG_I2C_I2CSRX                  ((volatile uint16_t *)REG_I2C_I2CSRX)                   /* I2C Slave receive */
#define pREG_I2C_I2CSTX                  ((volatile uint16_t *)REG_I2C_I2CSTX)                   /* I2C Slave transmit */
#define pREG_I2C_I2CALT                  ((volatile uint16_t *)REG_I2C_I2CALT)                   /* I2C Hardware general call ID */
#define pREG_I2C_I2CID0                  ((volatile uint16_t *)REG_I2C_I2CID0)                   /* I2C 1st slave address device ID */
#define pREG_I2C_I2CID1                  ((volatile uint16_t *)REG_I2C_I2CID1)                   /* I2C 2nd slave address device ID */
#define pREG_I2C_I2CID2                  ((volatile uint16_t *)REG_I2C_I2CID2)                   /* I2C 3rd slave address device ID */
#define pREG_I2C_I2CID3                  ((volatile uint16_t *)REG_I2C_I2CID3)                   /* I2C 4th slave address device ID */
#define pREG_I2C_I2CFSTA                 ((volatile uint16_t *)REG_I2C_I2CFSTA)                  /* I2C Master and slave FIFO status */
#define pREG_I2C_I2CSHCON                ((volatile uint16_t *)REG_I2C_I2CSHCON)                 /* I2C Shared control */
#define pREG_I2C_I2CTCTL                 ((volatile uint16_t *)REG_I2C_I2CTCTL)                  /* I2C Timing Control Register */


/* =========================================================================
       SPI0
   ========================================================================= */
#define pREG_SPI0_SPISTA                 ((volatile uint16_t *)REG_SPI0_SPISTA)                  /* SPI0 Status */
#define pREG_SPI0_SPIRX                  ((volatile uint16_t *)REG_SPI0_SPIRX)                   /* SPI0 Receive */
#define pREG_SPI0_SPITX                  ((volatile uint16_t *)REG_SPI0_SPITX)                   /* SPI0 Transmit */
#define pREG_SPI0_SPIDIV                 ((volatile uint16_t *)REG_SPI0_SPIDIV)                  /* SPI0 Baud rate selection */
#define pREG_SPI0_SPICON                 ((volatile uint16_t *)REG_SPI0_SPICON)                  /* SPI0 SPI configuration */
#define pREG_SPI0_SPIDMA                 ((volatile uint16_t *)REG_SPI0_SPIDMA)                  /* SPI0 SPI DMA enable */
#define pREG_SPI0_SPICNT                 ((volatile uint16_t *)REG_SPI0_SPICNT)                  /* SPI0 Transfer byte count */

/* =========================================================================
       SPI1
   ========================================================================= */
#define pREG_SPI1_SPISTA                 ((volatile uint16_t *)REG_SPI1_SPISTA)                  /* SPI1 Status */
#define pREG_SPI1_SPIRX                  ((volatile uint16_t *)REG_SPI1_SPIRX)                   /* SPI1 Receive */
#define pREG_SPI1_SPITX                  ((volatile uint16_t *)REG_SPI1_SPITX)                   /* SPI1 Transmit */
#define pREG_SPI1_SPIDIV                 ((volatile uint16_t *)REG_SPI1_SPIDIV)                  /* SPI1 Baud rate selection */
#define pREG_SPI1_SPICON                 ((volatile uint16_t *)REG_SPI1_SPICON)                  /* SPI1 SPI configuration */
#define pREG_SPI1_SPIDMA                 ((volatile uint16_t *)REG_SPI1_SPIDMA)                  /* SPI1 SPI DMA enable */
#define pREG_SPI1_SPICNT                 ((volatile uint16_t *)REG_SPI1_SPICNT)                  /* SPI1 Transfer byte count */


/* =========================================================================
       UART
   ========================================================================= */
#define pREG_UART_COMRX                  ((volatile uint16_t *)REG_UART_COMRX)                   /* UART Receive Buffer Register */
#define pREG_UART_COMTX                  ((volatile uint16_t *)REG_UART_COMTX)                   /* UART Transmit Holding Register */
#define pREG_UART_COMIEN                 ((volatile uint16_t *)REG_UART_COMIEN)                  /* UART Interrupt Enable */
#define pREG_UART_COMIIR                 ((volatile uint16_t *)REG_UART_COMIIR)                  /* UART Interrupt ID */
#define pREG_UART_COMLCR                 ((volatile uint16_t *)REG_UART_COMLCR)                  /* UART Line Control */
#define pREG_UART_COMMCR                 ((volatile uint16_t *)REG_UART_COMMCR)                  /* UART Modem Control */
#define pREG_UART_COMLSR                 ((volatile uint16_t *)REG_UART_COMLSR)                  /* UART Line Status */
#define pREG_UART_COMMSR                 ((volatile uint16_t *)REG_UART_COMMSR)                  /* UART Modem Status */
#define pREG_UART_COMSCR                 ((volatile uint16_t *)REG_UART_COMSCR)                  /* UART Scratch buffer */
#define pREG_UART_COMFBR                 ((volatile uint16_t *)REG_UART_COMFBR)                  /* UART Fractional Baud Rate */
#define pREG_UART_COMDIV                 ((volatile uint16_t *)REG_UART_COMDIV)                  /* UART Baudrate divider */


/* =========================================================================
       I2S
   ========================================================================= */
#define pREG_I2S_I2S_OUT1L               ((volatile uint16_t *)REG_I2S_I2S_OUT1L)                /* I2S Channel 1 LSBs */
#define pREG_I2S_I2S_OUT1H               ((volatile uint16_t *)REG_I2S_I2S_OUT1H)                /* I2S Channel 1 MSBs */
#define pREG_I2S_I2S_OUT2L               ((volatile uint16_t *)REG_I2S_I2S_OUT2L)                /* I2S Channel 2 LSBs */
#define pREG_I2S_I2S_OUT2H               ((volatile uint16_t *)REG_I2S_I2S_OUT2H)                /* I2S Channel 2 MSBs */
#define pREG_I2S_I2S_MODE1               ((volatile uint16_t *)REG_I2S_I2S_MODE1)                /* I2S I2S format modes 1 */
#define pREG_I2S_I2S_MODE2               ((volatile uint16_t *)REG_I2S_I2S_MODE2)                /* I2S I2S format modes 2 */
#define pREG_I2S_I2S_CFG1                ((volatile uint16_t *)REG_I2S_I2S_CFG1)                 /* I2S I2S configuration 1 */
#define pREG_I2S_I2S_CFG2                ((volatile uint16_t *)REG_I2S_I2S_CFG2)                 /* I2S I2S configuration 2 */
#define pREG_I2S_I2S_STAT                ((volatile uint16_t *)REG_I2S_I2S_STAT)                 /* I2S I2S status */


/* =========================================================================
       BEEP
   ========================================================================= */
#define pREG_BEEP_BEEP_CFG               ((volatile uint16_t *)REG_BEEP_BEEP_CFG)                /* BEEP Beeper configuration */
#define pREG_BEEP_BEEP_STAT              ((volatile uint16_t *)REG_BEEP_BEEP_STAT)               /* BEEP Beeper status */
#define pREG_BEEP_BEEP_TONE_A            ((volatile uint16_t *)REG_BEEP_BEEP_TONE_A)             /* BEEP Tone A Data */
#define pREG_BEEP_BEEP_TONE_B            ((volatile uint16_t *)REG_BEEP_BEEP_TONE_B)             /* BEEP Tone B Data */


/* =========================================================================
       RNG
   ========================================================================= */
#define pREG_RNG_RNGCTL                  ((volatile uint16_t *)REG_RNG_RNGCTL)                   /* RNG RNG Control Register */
#define pREG_RNG_RNGLEN                  ((volatile uint16_t *)REG_RNG_RNGLEN)                   /* RNG RNG Sample Length Register */
#define pREG_RNG_RNGSTAT                 ((volatile uint16_t *)REG_RNG_RNGSTAT)                  /* RNG RNG Status Register */
#define pREG_RNG_RNGDATA                 ((volatile uint16_t *)REG_RNG_RNGDATA)                  /* RNG RNG Data Register */
#define pREG_RNG_RNGCNTL                 ((volatile uint16_t *)REG_RNG_RNGCNTL)                  /* RNG Oscillator Count Low */
#define pREG_RNG_RNGCNTH                 ((volatile uint16_t *)REG_RNG_RNGCNTH)                  /* RNG Oscillator Count High */


/* =========================================================================
       LCD
   ========================================================================= */
#define pREG_LCD_LCDCON                  ((volatile uint16_t *)REG_LCD_LCDCON)                   /* LCD LCD Configuration  Register */
#define pREG_LCD_LCDSTAT                 ((volatile uint16_t *)REG_LCD_LCDSTAT)                  /* LCD LCD Status Register */
#define pREG_LCD_LCDBLINK                ((volatile uint16_t *)REG_LCD_LCDBLINK)                 /* LCD LCD Blink Control Register */
#define pREG_LCD_LCDCONTRAST             ((volatile uint16_t *)REG_LCD_LCDCONTRAST)              /* LCD LCD Contrast Control Register */
#define pREG_LCD_LCDDATA0_S0             ((volatile uint16_t *)REG_LCD_LCDDATA0_S0)              /* LCD Screen 0 LCD Data Register n */
#define pREG_LCD_LCDDATA1_S0             ((volatile uint16_t *)REG_LCD_LCDDATA1_S0)              /* LCD Screen 0 LCD Data Register n */
#define pREG_LCD_LCDDATA2_S0             ((volatile uint16_t *)REG_LCD_LCDDATA2_S0)              /* LCD Screen 0 LCD Data Register n */
#define pREG_LCD_LCDDATA3_S0             ((volatile uint16_t *)REG_LCD_LCDDATA3_S0)              /* LCD Screen 0 LCD Data Register n */
#define pREG_LCD_LCDDATA4_S0             ((volatile uint16_t *)REG_LCD_LCDDATA4_S0)              /* LCD Screen 0 LCD Data Register n */
#define pREG_LCD_LCDDATA5_S0             ((volatile uint16_t *)REG_LCD_LCDDATA5_S0)              /* LCD Screen 0 LCD Data Register n */
#define pREG_LCD_LCDDATA6_S0             ((volatile uint16_t *)REG_LCD_LCDDATA6_S0)              /* LCD Screen 0 LCD Data Register n */
#define pREG_LCD_LCDDATA7_S0             ((volatile uint16_t *)REG_LCD_LCDDATA7_S0)              /* LCD Screen 0 LCD Data Register n */
#define pREG_LCD_LCDDATA0_S1             ((volatile uint16_t *)REG_LCD_LCDDATA0_S1)              /* LCD Screen 1 LCD Data Register n */
#define pREG_LCD_LCDDATA1_S1             ((volatile uint16_t *)REG_LCD_LCDDATA1_S1)              /* LCD Screen 1 LCD Data Register n */
#define pREG_LCD_LCDDATA2_S1             ((volatile uint16_t *)REG_LCD_LCDDATA2_S1)              /* LCD Screen 1 LCD Data Register n */
#define pREG_LCD_LCDDATA3_S1             ((volatile uint16_t *)REG_LCD_LCDDATA3_S1)              /* LCD Screen 1 LCD Data Register n */
#define pREG_LCD_LCDDATA4_S1             ((volatile uint16_t *)REG_LCD_LCDDATA4_S1)              /* LCD Screen 1 LCD Data Register n */
#define pREG_LCD_LCDDATA5_S1             ((volatile uint16_t *)REG_LCD_LCDDATA5_S1)              /* LCD Screen 1 LCD Data Register n */
#define pREG_LCD_LCDDATA6_S1             ((volatile uint16_t *)REG_LCD_LCDDATA6_S1)              /* LCD Screen 1 LCD Data Register n */
#define pREG_LCD_LCDDATA7_S1             ((volatile uint16_t *)REG_LCD_LCDDATA7_S1)              /* LCD Screen 1 LCD Data Register n */



/* =========================================================================
       DMA
   ========================================================================= */
#define pREG_DMA_DMASTA                  ((volatile uint32_t *)REG_DMA_DMASTA)                   /* DMA DMA Status */
#define pREG_DMA_DMACFG                  ((volatile uint32_t *)REG_DMA_DMACFG)                   /* DMA DMA Configuration */
#define pREG_DMA_DMAPDBPTR               ((volatile uint32_t *)REG_DMA_DMAPDBPTR)                /* DMA DMA channel primary control data base pointer */
#define pREG_DMA_DMAADBPTR               ((volatile uint32_t *)REG_DMA_DMAADBPTR)                /* DMA DMA channel alternate control data base pointer */
#define pREG_DMA_DMASWREQ                ((volatile uint32_t *)REG_DMA_DMASWREQ)                 /* DMA DMA channel software request */
#define pREG_DMA_DMARMSKSET              ((volatile uint32_t *)REG_DMA_DMARMSKSET)               /* DMA DMA channel request mask set */
#define pREG_DMA_DMARMSKCLR              ((volatile uint32_t *)REG_DMA_DMARMSKCLR)               /* DMA DMA channel request mask clear */
#define pREG_DMA_DMAENSET                ((volatile uint32_t *)REG_DMA_DMAENSET)                 /* DMA DMA channel enable set */
#define pREG_DMA_DMAENCLR                ((volatile uint32_t *)REG_DMA_DMAENCLR)                 /* DMA DMA channel enable clear */
#define pREG_DMA_DMAALTSET               ((volatile uint32_t *)REG_DMA_DMAALTSET)                /* DMA DMA channel primary-alternate set */
#define pREG_DMA_DMAALTCLR               ((volatile uint32_t *)REG_DMA_DMAALTCLR)                /* DMA DMA channel primary-alternate clear */
#define pREG_DMA_DMAPRISET               ((volatile uint32_t *)REG_DMA_DMAPRISET)                /* DMA DMA channel priority set */
#define pREG_DMA_DMAPRICLR               ((volatile uint32_t *)REG_DMA_DMAPRICLR)                /* DMA DMA channel priority clear */
#define pREG_DMA_DMAERRCHNLCLR           ((volatile uint32_t *)REG_DMA_DMAERRCHNLCLR)            /* DMA DMA Per Channel Error Clear */
#define pREG_DMA_DMAERRCLR               ((volatile uint32_t *)REG_DMA_DMAERRCLR)                /* DMA DMA bus error clear */
#define pREG_DMA_DMAINVALIDDESCCLR       ((volatile uint32_t *)REG_DMA_DMAINVALIDDESCCLR)        /* DMA DMA Per Channel Invalid Descriptor Clear */
#define pREG_DMA_DMABSSET                ((volatile uint32_t *)REG_DMA_DMABSSET)                 /* DMA DMA channel bytes swap enable set */
#define pREG_DMA_DMABSCLR                ((volatile uint32_t *)REG_DMA_DMABSCLR)                 /* DMA DMA channel bytes swap enable clear */
#define pREG_DMA_DMASRCADSSET            ((volatile uint32_t *)REG_DMA_DMASRCADSSET)             /* DMA DMA channel source address decrement enable set */
#define pREG_DMA_DMASRCADCLR             ((volatile uint32_t *)REG_DMA_DMASRCADCLR)              /* DMA DMA channel source address decrement enable clear */
#define pREG_DMA_DMADSTADSET             ((volatile uint32_t *)REG_DMA_DMADSTADSET)              /* DMA DMA channel destination address decrement enable set */
#define pREG_DMA_DMADSTADCLR             ((volatile uint32_t *)REG_DMA_DMADSTADCLR)              /* DMA DMA channel destination address decrement enable clear */
#define pREG_DMA_DMAREVID                ((volatile uint32_t *)REG_DMA_DMAREVID)                 /* DMA DMA Controller Revision ID */


/* =========================================================================
       FEE0
   ========================================================================= */
#define pREG_FEE0_FEESTA                 ((volatile uint16_t *)REG_FEE0_FEESTA)                  /* FEE0 Status */
#define pREG_FEE0_FEECON0                ((volatile uint16_t *)REG_FEE0_FEECON0)                 /* FEE0 Command Control */
#define pREG_FEE0_FEECMD                 ((volatile uint16_t *)REG_FEE0_FEECMD)                  /* FEE0 Command */
#define pREG_FEE0_FEEADR0L               ((volatile uint16_t *)REG_FEE0_FEEADR0L)                /* FEE0 Lower page address */
#define pREG_FEE0_FEEADR0H               ((volatile uint16_t *)REG_FEE0_FEEADR0H)                /* FEE0 Upper page address */
#define pREG_FEE0_FEEADR1L               ((volatile uint16_t *)REG_FEE0_FEEADR1L)                /* FEE0 Lower page address */
#define pREG_FEE0_FEEADR1H               ((volatile uint16_t *)REG_FEE0_FEEADR1H)                /* FEE0 Upper page address */
#define pREG_FEE0_FEEKEY                 ((volatile uint16_t *)REG_FEE0_FEEKEY)                  /* FEE0 Key */
#define pREG_FEE0_FEEPROL                ((volatile uint16_t *)REG_FEE0_FEEPROL)                 /* FEE0 Lower halfword of write protection */
#define pREG_FEE0_FEEPROH                ((volatile uint16_t *)REG_FEE0_FEEPROH)                 /* FEE0 Upper halfword of write protection */
#define pREG_FEE0_FEESIGL                ((volatile uint16_t *)REG_FEE0_FEESIGL)                 /* FEE0 Lower halfword of signature */
#define pREG_FEE0_FEESIGH                ((volatile uint16_t *)REG_FEE0_FEESIGH)                 /* FEE0 Upper halfword of signature */
#define pREG_FEE0_FEECON1                ((volatile uint16_t *)REG_FEE0_FEECON1)                 /* FEE0 User Setup */
#define pREG_FEE0_FEEADRAL               ((volatile uint16_t *)REG_FEE0_FEEADRAL)                /* FEE0 Lower halfword of write abort address */
#define pREG_FEE0_FEEADRAH               ((volatile uint16_t *)REG_FEE0_FEEADRAH)                /* FEE0 Upper halfword of write abort address */
#define pREG_FEE0_FEEPARCTL              ((volatile uint16_t *)REG_FEE0_FEEPARCTL)               /* FEE0 Parity Control Register */
#define pREG_FEE0_FEEPARSTA              ((volatile uint16_t *)REG_FEE0_FEEPARSTA)               /* FEE0 Parity Status Register */
#define pREG_FEE0_FEEPARADRL             ((volatile uint16_t *)REG_FEE0_FEEPARADRL)              /* FEE0 Parity Error Address Low */
#define pREG_FEE0_FEEPARADRH             ((volatile uint16_t *)REG_FEE0_FEEPARADRH)              /* FEE0 Parity Error Address High */
#define pREG_FEE0_FEEAEN0                ((volatile uint16_t *)REG_FEE0_FEEAEN0)                 /* FEE0 System IRQ abort enable for interrupts 15 to 0 */
#define pREG_FEE0_FEEAEN1                ((volatile uint16_t *)REG_FEE0_FEEAEN1)                 /* FEE0 System IRQ abort enable for interrupts 31 to 16 */
#define pREG_FEE0_FEEAEN2                ((volatile uint16_t *)REG_FEE0_FEEAEN2)                 /* FEE0 System IRQ abort enable for interrupts 47 to 32 */
#define pREG_FEE0_FEEAEN3                ((volatile uint16_t *)REG_FEE0_FEEAEN3)                 /* FEE0 System IRQ abort enable for interrupts 60 to 48 */

/* =========================================================================
       FEE1
   ========================================================================= */
#define pREG_FEE1_FEESTA                 ((volatile uint16_t *)REG_FEE1_FEESTA)                  /* FEE1 Status */
#define pREG_FEE1_FEECON0                ((volatile uint16_t *)REG_FEE1_FEECON0)                 /* FEE1 Command Control */
#define pREG_FEE1_FEECMD                 ((volatile uint16_t *)REG_FEE1_FEECMD)                  /* FEE1 Command */
#define pREG_FEE1_FEEADR0L               ((volatile uint16_t *)REG_FEE1_FEEADR0L)                /* FEE1 Lower page address */
#define pREG_FEE1_FEEADR0H               ((volatile uint16_t *)REG_FEE1_FEEADR0H)                /* FEE1 Upper page address */
#define pREG_FEE1_FEEADR1L               ((volatile uint16_t *)REG_FEE1_FEEADR1L)                /* FEE1 Lower page address */
#define pREG_FEE1_FEEADR1H               ((volatile uint16_t *)REG_FEE1_FEEADR1H)                /* FEE1 Upper page address */
#define pREG_FEE1_FEEKEY                 ((volatile uint16_t *)REG_FEE1_FEEKEY)                  /* FEE1 Key */
#define pREG_FEE1_FEEPROL                ((volatile uint16_t *)REG_FEE1_FEEPROL)                 /* FEE1 Lower halfword of write protection */
#define pREG_FEE1_FEEPROH                ((volatile uint16_t *)REG_FEE1_FEEPROH)                 /* FEE1 Upper halfword of write protection */
#define pREG_FEE1_FEESIGL                ((volatile uint16_t *)REG_FEE1_FEESIGL)                 /* FEE1 Lower halfword of signature */
#define pREG_FEE1_FEESIGH                ((volatile uint16_t *)REG_FEE1_FEESIGH)                 /* FEE1 Upper halfword of signature */
#define pREG_FEE1_FEECON1                ((volatile uint16_t *)REG_FEE1_FEECON1)                 /* FEE1 User Setup */
#define pREG_FEE1_FEEADRAL               ((volatile uint16_t *)REG_FEE1_FEEADRAL)                /* FEE1 Lower halfword of write abort address */
#define pREG_FEE1_FEEADRAH               ((volatile uint16_t *)REG_FEE1_FEEADRAH)                /* FEE1 Upper halfword of write abort address */
#define pREG_FEE1_FEEPARCTL              ((volatile uint16_t *)REG_FEE1_FEEPARCTL)               /* FEE1 Parity Control Register */
#define pREG_FEE1_FEEPARSTA              ((volatile uint16_t *)REG_FEE1_FEEPARSTA)               /* FEE1 Parity Status Register */
#define pREG_FEE1_FEEPARADRL             ((volatile uint16_t *)REG_FEE1_FEEPARADRL)              /* FEE1 Parity Error Address Low */
#define pREG_FEE1_FEEPARADRH             ((volatile uint16_t *)REG_FEE1_FEEPARADRH)              /* FEE1 Parity Error Address High */
#define pREG_FEE1_FEEAEN0                ((volatile uint16_t *)REG_FEE1_FEEAEN0)                 /* FEE1 System IRQ abort enable for interrupts 15 to 0 */
#define pREG_FEE1_FEEAEN1                ((volatile uint16_t *)REG_FEE1_FEEAEN1)                 /* FEE1 System IRQ abort enable for interrupts 31 to 16 */
#define pREG_FEE1_FEEAEN2                ((volatile uint16_t *)REG_FEE1_FEEAEN2)                 /* FEE1 System IRQ abort enable for interrupts 47 to 32 */
#define pREG_FEE1_FEEAEN3                ((volatile uint16_t *)REG_FEE1_FEEAEN3)                 /* FEE1 System IRQ abort enable for interrupts 60 to 48 */


/* =========================================================================
       GPF
   ========================================================================= */
#define pREG_GPF_GPFEESTA                ((volatile uint16_t *)REG_GPF_GPFEESTA)                 /* GPF Status */
#define pREG_GPF_GPFEECON0               ((volatile uint16_t *)REG_GPF_GPFEECON0)                /* GPF Command Control */
#define pREG_GPF_GPFEECMD                ((volatile uint16_t *)REG_GPF_GPFEECMD)                 /* GPF Command */
#define pREG_GPF_GPFEEADR0L              ((volatile uint16_t *)REG_GPF_GPFEEADR0L)               /* GPF Lower page address */
#define pREG_GPF_GPFEEADR1L              ((volatile uint16_t *)REG_GPF_GPFEEADR1L)               /* GPF Lower page address */
#define pREG_GPF_GPFEEKEY                ((volatile uint16_t *)REG_GPF_GPFEEKEY)                 /* GPF Key */
#define pREG_GPF_GPFEESIGL               ((volatile uint16_t *)REG_GPF_GPFEESIGL)                /* GPF Lower halfword of signature */
#define pREG_GPF_GPFEESIGH               ((volatile uint16_t *)REG_GPF_GPFEESIGH)                /* GPF Upper halfword of signature */
#define pREG_GPF_GPFEEADRAL              ((volatile uint16_t *)REG_GPF_GPFEEADRAL)               /* GPF Lower halfword of write abort address */
#define pREG_GPF_GPFEEADRAH              ((volatile uint16_t *)REG_GPF_GPFEEADRAH)               /* GPF Upper halfword of write abort address */
#define pREG_GPF_GPFEEAEN0               ((volatile uint16_t *)REG_GPF_GPFEEAEN0)                /* GPF System IRQ abort enable for interrupts 15 to 0 */
#define pREG_GPF_GPFEEAEN1               ((volatile uint16_t *)REG_GPF_GPFEEAEN1)                /* GPF System IRQ abort enable for interrupts 31 to 16 */
#define pREG_GPF_GPFEEAEN2               ((volatile uint16_t *)REG_GPF_GPFEEAEN2)                /* GPF System IRQ abort enable for interrupts 47 to 32 */
#define pREG_GPF_GPFEEAEN3               ((volatile uint16_t *)REG_GPF_GPFEEAEN3)                /* GPF System IRQ abort enable for interrupts 60 to 48 */


/* =========================================================================
       GPIO0
   ========================================================================= */
#define pREG_GPIO0_GPCON                 ((volatile uint32_t *)REG_GPIO0_GPCON)                  /* GPIO0 GPIO Port 0  Configuration */
#define pREG_GPIO0_GPOEN                 ((volatile uint16_t *)REG_GPIO0_GPOEN)                  /* GPIO0 GPIO Port 0 output enable */
#define pREG_GPIO0_GPPE                  ((volatile uint16_t *)REG_GPIO0_GPPE)                   /* GPIO0 GPIO Port 0 output pullup/pulldown enable */
#define pREG_GPIO0_GPIEN                 ((volatile uint16_t *)REG_GPIO0_GPIEN)                  /* GPIO0 GPIO  Port 0 Input Path Enable */
#define pREG_GPIO0_GPIN                  ((volatile uint16_t *)REG_GPIO0_GPIN)                   /* GPIO0 GPIO  Port 0 registered data input */
#define pREG_GPIO0_GPOUT                 ((volatile uint16_t *)REG_GPIO0_GPOUT)                  /* GPIO0 GPIO Port 0 data output */
#define pREG_GPIO0_GPSET                 ((volatile uint16_t *)REG_GPIO0_GPSET)                  /* GPIO0 GPIO Port 0 data out set */
#define pREG_GPIO0_GPCLR                 ((volatile uint16_t *)REG_GPIO0_GPCLR)                  /* GPIO0 GPIO Port 0 data out clear */
#define pREG_GPIO0_GPTGL                 ((volatile uint16_t *)REG_GPIO0_GPTGL)                  /* GPIO0 GPIO Port 0  pin toggle */
#define pREG_GPIO0_GPPOL                 ((volatile uint16_t *)REG_GPIO0_GPPOL)                  /* GPIO0 GPIO Port 0 interrupt polarity */
#define pREG_GPIO0_GPIENA                ((volatile uint16_t *)REG_GPIO0_GPIENA)                 /* GPIO0 GPIO Port 0  interrupt A enable */
#define pREG_GPIO0_GPIENB                ((volatile uint16_t *)REG_GPIO0_GPIENB)                 /* GPIO0 GPIO Port 0 interrupt B enable */
#define pREG_GPIO0_GPINT                 ((volatile uint16_t *)REG_GPIO0_GPINT)                  /* GPIO0 GPIO Port 0 interrupt Status */

/* =========================================================================
       GPIO1
   ========================================================================= */
#define pREG_GPIO1_GPCON                 ((volatile uint32_t *)REG_GPIO1_GPCON)                  /* GPIO1 GPIO Port 0  Configuration */
#define pREG_GPIO1_GPOEN                 ((volatile uint16_t *)REG_GPIO1_GPOEN)                  /* GPIO1 GPIO Port 0 output enable */
#define pREG_GPIO1_GPPE                  ((volatile uint16_t *)REG_GPIO1_GPPE)                   /* GPIO1 GPIO Port 0 output pullup/pulldown enable */
#define pREG_GPIO1_GPIEN                 ((volatile uint16_t *)REG_GPIO1_GPIEN)                  /* GPIO1 GPIO  Port 0 Input Path Enable */
#define pREG_GPIO1_GPIN                  ((volatile uint16_t *)REG_GPIO1_GPIN)                   /* GPIO1 GPIO  Port 0 registered data input */
#define pREG_GPIO1_GPOUT                 ((volatile uint16_t *)REG_GPIO1_GPOUT)                  /* GPIO1 GPIO Port 0 data output */
#define pREG_GPIO1_GPSET                 ((volatile uint16_t *)REG_GPIO1_GPSET)                  /* GPIO1 GPIO Port 0 data out set */
#define pREG_GPIO1_GPCLR                 ((volatile uint16_t *)REG_GPIO1_GPCLR)                  /* GPIO1 GPIO Port 0 data out clear */
#define pREG_GPIO1_GPTGL                 ((volatile uint16_t *)REG_GPIO1_GPTGL)                  /* GPIO1 GPIO Port 0  pin toggle */
#define pREG_GPIO1_GPPOL                 ((volatile uint16_t *)REG_GPIO1_GPPOL)                  /* GPIO1 GPIO Port 0 interrupt polarity */
#define pREG_GPIO1_GPIENA                ((volatile uint16_t *)REG_GPIO1_GPIENA)                 /* GPIO1 GPIO Port 0  interrupt A enable */
#define pREG_GPIO1_GPIENB                ((volatile uint16_t *)REG_GPIO1_GPIENB)                 /* GPIO1 GPIO Port 0 interrupt B enable */
#define pREG_GPIO1_GPINT                 ((volatile uint16_t *)REG_GPIO1_GPINT)                  /* GPIO1 GPIO Port 0 interrupt Status */

/* =========================================================================
       GPIO2
   ========================================================================= */
#define pREG_GPIO2_GPCON                 ((volatile uint32_t *)REG_GPIO2_GPCON)                  /* GPIO2 GPIO Port 0  Configuration */
#define pREG_GPIO2_GPOEN                 ((volatile uint16_t *)REG_GPIO2_GPOEN)                  /* GPIO2 GPIO Port 0 output enable */
#define pREG_GPIO2_GPPE                  ((volatile uint16_t *)REG_GPIO2_GPPE)                   /* GPIO2 GPIO Port 0 output pullup/pulldown enable */
#define pREG_GPIO2_GPIEN                 ((volatile uint16_t *)REG_GPIO2_GPIEN)                  /* GPIO2 GPIO  Port 0 Input Path Enable */
#define pREG_GPIO2_GPIN                  ((volatile uint16_t *)REG_GPIO2_GPIN)                   /* GPIO2 GPIO  Port 0 registered data input */
#define pREG_GPIO2_GPOUT                 ((volatile uint16_t *)REG_GPIO2_GPOUT)                  /* GPIO2 GPIO Port 0 data output */
#define pREG_GPIO2_GPSET                 ((volatile uint16_t *)REG_GPIO2_GPSET)                  /* GPIO2 GPIO Port 0 data out set */
#define pREG_GPIO2_GPCLR                 ((volatile uint16_t *)REG_GPIO2_GPCLR)                  /* GPIO2 GPIO Port 0 data out clear */
#define pREG_GPIO2_GPTGL                 ((volatile uint16_t *)REG_GPIO2_GPTGL)                  /* GPIO2 GPIO Port 0  pin toggle */
#define pREG_GPIO2_GPPOL                 ((volatile uint16_t *)REG_GPIO2_GPPOL)                  /* GPIO2 GPIO Port 0 interrupt polarity */
#define pREG_GPIO2_GPIENA                ((volatile uint16_t *)REG_GPIO2_GPIENA)                 /* GPIO2 GPIO Port 0  interrupt A enable */
#define pREG_GPIO2_GPIENB                ((volatile uint16_t *)REG_GPIO2_GPIENB)                 /* GPIO2 GPIO Port 0 interrupt B enable */
#define pREG_GPIO2_GPINT                 ((volatile uint16_t *)REG_GPIO2_GPINT)                  /* GPIO2 GPIO Port 0 interrupt Status */

/* =========================================================================
       GPIO3
   ========================================================================= */
#define pREG_GPIO3_GPCON                 ((volatile uint32_t *)REG_GPIO3_GPCON)                  /* GPIO3 GPIO Port 0  Configuration */
#define pREG_GPIO3_GPOEN                 ((volatile uint16_t *)REG_GPIO3_GPOEN)                  /* GPIO3 GPIO Port 0 output enable */
#define pREG_GPIO3_GPPE                  ((volatile uint16_t *)REG_GPIO3_GPPE)                   /* GPIO3 GPIO Port 0 output pullup/pulldown enable */
#define pREG_GPIO3_GPIEN                 ((volatile uint16_t *)REG_GPIO3_GPIEN)                  /* GPIO3 GPIO  Port 0 Input Path Enable */
#define pREG_GPIO3_GPIN                  ((volatile uint16_t *)REG_GPIO3_GPIN)                   /* GPIO3 GPIO  Port 0 registered data input */
#define pREG_GPIO3_GPOUT                 ((volatile uint16_t *)REG_GPIO3_GPOUT)                  /* GPIO3 GPIO Port 0 data output */
#define pREG_GPIO3_GPSET                 ((volatile uint16_t *)REG_GPIO3_GPSET)                  /* GPIO3 GPIO Port 0 data out set */
#define pREG_GPIO3_GPCLR                 ((volatile uint16_t *)REG_GPIO3_GPCLR)                  /* GPIO3 GPIO Port 0 data out clear */
#define pREG_GPIO3_GPTGL                 ((volatile uint16_t *)REG_GPIO3_GPTGL)                  /* GPIO3 GPIO Port 0  pin toggle */
#define pREG_GPIO3_GPPOL                 ((volatile uint16_t *)REG_GPIO3_GPPOL)                  /* GPIO3 GPIO Port 0 interrupt polarity */
#define pREG_GPIO3_GPIENA                ((volatile uint16_t *)REG_GPIO3_GPIENA)                 /* GPIO3 GPIO Port 0  interrupt A enable */
#define pREG_GPIO3_GPIENB                ((volatile uint16_t *)REG_GPIO3_GPIENB)                 /* GPIO3 GPIO Port 0 interrupt B enable */
#define pREG_GPIO3_GPINT                 ((volatile uint16_t *)REG_GPIO3_GPINT)                  /* GPIO3 GPIO Port 0 interrupt Status */

/* =========================================================================
       GPIO4
   ========================================================================= */
#define pREG_GPIO4_GPCON                 ((volatile uint32_t *)REG_GPIO4_GPCON)                  /* GPIO4 GPIO Port 0  Configuration */
#define pREG_GPIO4_GPOEN                 ((volatile uint16_t *)REG_GPIO4_GPOEN)                  /* GPIO4 GPIO Port 0 output enable */
#define pREG_GPIO4_GPPE                  ((volatile uint16_t *)REG_GPIO4_GPPE)                   /* GPIO4 GPIO Port 0 output pullup/pulldown enable */
#define pREG_GPIO4_GPIEN                 ((volatile uint16_t *)REG_GPIO4_GPIEN)                  /* GPIO4 GPIO  Port 0 Input Path Enable */
#define pREG_GPIO4_GPIN                  ((volatile uint16_t *)REG_GPIO4_GPIN)                   /* GPIO4 GPIO  Port 0 registered data input */
#define pREG_GPIO4_GPOUT                 ((volatile uint16_t *)REG_GPIO4_GPOUT)                  /* GPIO4 GPIO Port 0 data output */
#define pREG_GPIO4_GPSET                 ((volatile uint16_t *)REG_GPIO4_GPSET)                  /* GPIO4 GPIO Port 0 data out set */
#define pREG_GPIO4_GPCLR                 ((volatile uint16_t *)REG_GPIO4_GPCLR)                  /* GPIO4 GPIO Port 0 data out clear */
#define pREG_GPIO4_GPTGL                 ((volatile uint16_t *)REG_GPIO4_GPTGL)                  /* GPIO4 GPIO Port 0  pin toggle */
#define pREG_GPIO4_GPPOL                 ((volatile uint16_t *)REG_GPIO4_GPPOL)                  /* GPIO4 GPIO Port 0 interrupt polarity */
#define pREG_GPIO4_GPIENA                ((volatile uint16_t *)REG_GPIO4_GPIENA)                 /* GPIO4 GPIO Port 0  interrupt A enable */
#define pREG_GPIO4_GPIENB                ((volatile uint16_t *)REG_GPIO4_GPIENB)                 /* GPIO4 GPIO Port 0 interrupt B enable */
#define pREG_GPIO4_GPINT                 ((volatile uint16_t *)REG_GPIO4_GPINT)                  /* GPIO4 GPIO Port 0 interrupt Status */


/* =========================================================================
       SPIH
   ========================================================================= */
#define pREG_SPIH_SPIH0STA               ((volatile uint16_t *)REG_SPIH_SPIH0STA)                /* SPIH Status */
#define pREG_SPIH_SPIH0RX                ((volatile uint16_t *)REG_SPIH_SPIH0RX)                 /* SPIH Receive */
#define pREG_SPIH_SPIH0TX                ((volatile uint16_t *)REG_SPIH_SPIH0TX)                 /* SPIH Transmit */
#define pREG_SPIH_SPIH0DIV               ((volatile uint16_t *)REG_SPIH_SPIH0DIV)                /* SPIH Baud rate selection */
#define pREG_SPIH_SPIH0CON               ((volatile uint16_t *)REG_SPIH_SPIH0CON)                /* SPIH SPI configuration */
#define pREG_SPIH_SPIH0DMA               ((volatile uint16_t *)REG_SPIH_SPIH0DMA)                /* SPIH SPI DMA enable */
#define pREG_SPIH_SPIH0CNT               ((volatile uint16_t *)REG_SPIH_SPIH0CNT)                /* SPIH Transfer byte count */


/* =========================================================================
       SYSCLK
   ========================================================================= */
#define pREG_SYSCLK_CLKCON0              ((volatile uint16_t *)REG_SYSCLK_CLKCON0)               /* SYSCLK Misc clock settings */
#define pREG_SYSCLK_CLKCON1              ((volatile uint16_t *)REG_SYSCLK_CLKCON1)               /* SYSCLK Clock dividers */
#define pREG_SYSCLK_CLKCON3              ((volatile uint16_t *)REG_SYSCLK_CLKCON3)               /* SYSCLK System PLL */
#define pREG_SYSCLK_CLKCON4              ((volatile uint16_t *)REG_SYSCLK_CLKCON4)               /* SYSCLK USB PLL */
#define pREG_SYSCLK_CLKCON5              ((volatile uint16_t *)REG_SYSCLK_CLKCON5)               /* SYSCLK User clock gating control */
#define pREG_SYSCLK_CLKSTAT0             ((volatile uint16_t *)REG_SYSCLK_CLKSTAT0)              /* SYSCLK Clocking status */


/* =========================================================================
       BUSM
   ========================================================================= */
#define pREG_BUSM_BMARBIT0               ((volatile uint16_t *)REG_BUSM_BMARBIT0)                /* BUSM Arbitration Priority Configuration for Slave 0 (Flash) */
#define pREG_BUSM_BMARBIT1               ((volatile uint16_t *)REG_BUSM_BMARBIT1)                /* BUSM Arbitration Priority Configuration for Slave 1 (SRAM0) */
#define pREG_BUSM_BMARBIT2               ((volatile uint16_t *)REG_BUSM_BMARBIT2)                /* BUSM Arbitration Priority Configuration for Slave 2 (SRAM1 + EEPROM) */
#define pREG_BUSM_BMARBIT3               ((volatile uint16_t *)REG_BUSM_BMARBIT3)                /* BUSM Arbitration Priority Configuration for Slave 3 (AFE) */
#define pREG_BUSM_BMARBIT4               ((volatile uint16_t *)REG_BUSM_BMARBIT4)                /* BUSM Arbitration Priority Configuration for Slave 4 (32-bit APB) */
#define pREG_BUSM_BMARBIT5               ((volatile uint16_t *)REG_BUSM_BMARBIT5)                /* BUSM Arbitration Priority Configuration for Slave 5 (16-bit APB with DMA access) */


/* =========================================================================
       CRC
   ========================================================================= */
#define pREG_CRC_CRC_CTL                 ((volatile uint32_t *)REG_CRC_CRC_CTL)                  /* CRC CRC Control Register */
#define pREG_CRC_CRC_IPDATA              ((volatile uint32_t *)REG_CRC_CRC_IPDATA)               /* CRC Input Data Register */
#define pREG_CRC_CRC_RESULT              ((volatile uint32_t *)REG_CRC_CRC_RESULT)               /* CRC CRC Result Register */


/* =========================================================================
       PDI
   ========================================================================= */
#define pREG_PDI_PDI_CFG                 ((volatile uint32_t *)REG_PDI_PDI_CFG)                  /* PDI PDI Configuration  Register */
#define pREG_PDI_PDI_INT_SET             ((volatile uint32_t *)REG_PDI_PDI_INT_SET)              /* PDI PDI Interrupt Set Register */
#define pREG_PDI_PDI_INT_CLR             ((volatile uint32_t *)REG_PDI_PDI_INT_CLR)              /* PDI PDI Interrupt Clear Register */
#define pREG_PDI_PDI_STAT                ((volatile uint32_t *)REG_PDI_PDI_STAT)                 /* PDI PDI Status Register */
#define pREG_PDI_PDI_CMD                 ((volatile uint32_t *)REG_PDI_PDI_CMD)                  /* PDI PDI Command Register */
#define pREG_PDI_PDI_FRDATA_N            ((volatile uint32_t *)REG_PDI_PDI_FRDATA_N)             /* PDI PDI Frame Data Count Register */
#define pREG_PDI_PDI_FIFO                ((volatile uint32_t *)REG_PDI_PDI_FIFO)                 /* PDI PDI Parameter FIFO */
#define pREG_PDI_PDI_IF_TIMING           ((volatile uint32_t *)REG_PDI_PDI_IF_TIMING)            /* PDI PDI Interface Timing Register */


/* =========================================================================
       AFE
   ========================================================================= */
#define pREG_AFE_AFE_CFG                 ((volatile uint32_t *)REG_AFE_AFE_CFG)                  /* AFE AFE Configuration */
#define pREG_AFE_AFE_SEQ_CFG             ((volatile uint32_t *)REG_AFE_AFE_SEQ_CFG)              /* AFE Sequencer Configuration */
#define pREG_AFE_AFE_FIFO_CFG            ((volatile uint32_t *)REG_AFE_AFE_FIFO_CFG)             /* AFE FIFOs Configuration */
#define pREG_AFE_AFE_SW_CFG              ((volatile uint32_t *)REG_AFE_AFE_SW_CFG)               /* AFE Switch Matrix Configuration */
#define pREG_AFE_AFE_DAC_CFG             ((volatile uint32_t *)REG_AFE_AFE_DAC_CFG)              /* AFE DAC Configuration */
#define pREG_AFE_AFE_WG_CFG              ((volatile uint32_t *)REG_AFE_AFE_WG_CFG)               /* AFE Waveform Generator Configuration */
#define pREG_AFE_AFE_WG_DCLEVEL_1        ((volatile uint32_t *)REG_AFE_AFE_WG_DCLEVEL_1)         /* AFE Waveform Generator - Trapezoid DC Level 1 */
#define pREG_AFE_AFE_WG_DCLEVEL_2        ((volatile uint32_t *)REG_AFE_AFE_WG_DCLEVEL_2)         /* AFE Waveform Generator - Trapezoid DC Level 2 */
#define pREG_AFE_AFE_WG_DELAY_1          ((volatile uint32_t *)REG_AFE_AFE_WG_DELAY_1)           /* AFE Waveform Generator - Trapezoid Delay 1 Time */
#define pREG_AFE_AFE_WG_SLOPE_1          ((volatile uint32_t *)REG_AFE_AFE_WG_SLOPE_1)           /* AFE Waveform Generator - Trapezoid Slope 1 Time */
#define pREG_AFE_AFE_WG_DELAY_2          ((volatile uint32_t *)REG_AFE_AFE_WG_DELAY_2)           /* AFE Waveform Generator - Trapezoid Delay 2 Time */
#define pREG_AFE_AFE_WG_SLOPE_2          ((volatile uint32_t *)REG_AFE_AFE_WG_SLOPE_2)           /* AFE Waveform Generator - Trapezoid Slope 2 Time */
#define pREG_AFE_AFE_WG_FCW              ((volatile uint32_t *)REG_AFE_AFE_WG_FCW)               /* AFE Waveform Generator - Sinusoid Frequency Control Word */
#define pREG_AFE_AFE_WG_PHASE            ((volatile uint32_t *)REG_AFE_AFE_WG_PHASE)             /* AFE Waveform Generator - Sinusoid Phase Offset */
#define pREG_AFE_AFE_WG_OFFSET           ((volatile uint32_t *)REG_AFE_AFE_WG_OFFSET)            /* AFE Waveform Generator - Sinusoid Offset */
#define pREG_AFE_AFE_WG_AMPLITUDE        ((volatile uint32_t *)REG_AFE_AFE_WG_AMPLITUDE)         /* AFE Waveform Generator - Sinusoid Amplitude */
#define pREG_AFE_AFE_ADC_CFG             ((volatile uint32_t *)REG_AFE_AFE_ADC_CFG)              /* AFE ADC Configuration */
#define pREG_AFE_AFE_SUPPLY_LPF_CFG      ((volatile uint32_t *)REG_AFE_AFE_SUPPLY_LPF_CFG)       /* AFE Supply Rejection Filter Configuration */
#define pREG_AFE_AFE_SW_FULL_CFG_MSB     ((volatile uint32_t *)REG_AFE_AFE_SW_FULL_CFG_MSB)      /* AFE Switch Matrix Full Configuration (MSB) */
#define pREG_AFE_AFE_SW_FULL_CFG_LSB     ((volatile uint32_t *)REG_AFE_AFE_SW_FULL_CFG_LSB)      /* AFE Switch Matrix Full Configuration (LSB) */
#define pREG_AFE_AFE_WG_DAC_CODE         ((volatile uint32_t *)REG_AFE_AFE_WG_DAC_CODE)          /* AFE Waveform Generator - DAC Code */
#define pREG_AFE_AFE_STATUS              ((volatile uint32_t *)REG_AFE_AFE_STATUS)               /* AFE AFE Status */
#define pREG_AFE_AFE_SEQ_CRC             ((volatile uint32_t *)REG_AFE_AFE_SEQ_CRC)              /* AFE Sequencer CRC Value */
#define pREG_AFE_AFE_SEQ_COUNT           ((volatile uint32_t *)REG_AFE_AFE_SEQ_COUNT)            /* AFE Sequencer Command Count */
#define pREG_AFE_AFE_SEQ_TIMEOUT         ((volatile uint32_t *)REG_AFE_AFE_SEQ_TIMEOUT)          /* AFE Sequencer Timeout Counter */
#define pREG_AFE_AFE_DATA_FIFO_READ      ((volatile uint32_t *)REG_AFE_AFE_DATA_FIFO_READ)       /* AFE Data FIFO Read */
#define pREG_AFE_AFE_CMD_FIFO_WRITE      ((volatile uint32_t *)REG_AFE_AFE_CMD_FIFO_WRITE)       /* AFE Command FIFO Write */
#define pREG_AFE_AFE_ADC_RESULT          ((volatile uint32_t *)REG_AFE_AFE_ADC_RESULT)           /* AFE ADC Raw Result */
#define pREG_AFE_AFE_DFT_RESULT_REAL     ((volatile uint32_t *)REG_AFE_AFE_DFT_RESULT_REAL)      /* AFE DFT Result, Real Part */
#define pREG_AFE_AFE_DFT_RESULT_IMAG     ((volatile uint32_t *)REG_AFE_AFE_DFT_RESULT_IMAG)      /* AFE DFT Result, Imaginary Part */
#define pREG_AFE_AFE_SUPPLY_LPF_RESULT   ((volatile uint32_t *)REG_AFE_AFE_SUPPLY_LPF_RESULT)    /* AFE Supply Rejection Filter Result */
#define pREG_AFE_AFE_TEMP_SENSOR_RESULT  ((volatile uint32_t *)REG_AFE_AFE_TEMP_SENSOR_RESULT)   /* AFE Temperature Sensor Result */
#define pREG_AFE_AFE_ANALOG_CAPTURE_IEN  ((volatile uint32_t *)REG_AFE_AFE_ANALOG_CAPTURE_IEN)   /* AFE Analog Capture Interrupt Enable */
#define pREG_AFE_AFE_ANALOG_GEN_IEN      ((volatile uint32_t *)REG_AFE_AFE_ANALOG_GEN_IEN)       /* AFE Analog Generation Interrupt Enable */
#define pREG_AFE_AFE_CMD_FIFO_IEN        ((volatile uint32_t *)REG_AFE_AFE_CMD_FIFO_IEN)         /* AFE Command FIFO Interrupt Enable */
#define pREG_AFE_AFE_DATA_FIFO_IEN       ((volatile uint32_t *)REG_AFE_AFE_DATA_FIFO_IEN)        /* AFE Data FIFO Interrupt Enable */
#define pREG_AFE_AFE_ANALOG_CAPTURE_INT  ((volatile uint32_t *)REG_AFE_AFE_ANALOG_CAPTURE_INT)   /* AFE Analog Capture Interrupt */
#define pREG_AFE_AFE_ANALOG_GEN_INT      ((volatile uint32_t *)REG_AFE_AFE_ANALOG_GEN_INT)       /* AFE Analog Generation Interrupt */
#define pREG_AFE_AFE_CMD_FIFO_INT        ((volatile uint32_t *)REG_AFE_AFE_CMD_FIFO_INT)         /* AFE Command FIFO Interrupt */
#define pREG_AFE_AFE_DATA_FIFO_INT       ((volatile uint32_t *)REG_AFE_AFE_DATA_FIFO_INT)        /* AFE Data FIFO Interrupt */
#define pREG_AFE_AFE_SW_STATUS_MSB       ((volatile uint32_t *)REG_AFE_AFE_SW_STATUS_MSB)        /* AFE Switch Matrix Status (MSB) */
#define pREG_AFE_AFE_SW_STATUS_LSB       ((volatile uint32_t *)REG_AFE_AFE_SW_STATUS_LSB)        /* AFE Switch Matrix Status (LSB) */
#define pREG_AFE_AFE_ADCMIN              ((volatile uint32_t *)REG_AFE_AFE_ADCMIN)               /* AFE ADC Minimum Value Check */
#define pREG_AFE_AFE_ADCMAX              ((volatile uint32_t *)REG_AFE_AFE_ADCMAX)               /* AFE ADC Maximum Value Check */
#define pREG_AFE_AFE_ADCDELTA            ((volatile uint32_t *)REG_AFE_AFE_ADCDELTA)             /* AFE ADC Delta Check */
#define pREG_AFE_AFE_CAL_DATA_LOCK       ((volatile uint32_t *)REG_AFE_AFE_CAL_DATA_LOCK)        /* AFE Calibration Data Lock */
#define pREG_AFE_AFE_ADC_GAIN_TIA        ((volatile uint32_t *)REG_AFE_ADC_GAIN_TIA)             /* AFE ADC Gain (TIA Measurement) */
#define pREG_AFE_AFE_ADC_OFFSET_TIA      ((volatile uint32_t *)REG_AFE_ADC_OFFSET_TIA)           /* AFE ADC Offset (TIA Measurement) */
#define pREG_AFE_AFE_ADC_GAIN_TEMP_SENS  ((volatile uint32_t *)REG_AFE_AFE_ADC_GAIN_TEMP_SENS)   /* AFE ADC Gain (Temperature Sensor Measurement) */
#define pREG_AFE_AFE_ADC_OFFSET_TEMP_SENS ((volatile uint32_t *)REG_AFE_AFE_ADC_OFFSET_TEMP_SENS)  /* AFE ADC Offset (Temperature Sensor Measurement) */
#define pREG_AFE_AFE_ADC_GAIN_AUX        ((volatile uint32_t *)REG_AFE_AFE_ADC_GAIN_AUX)         /* AFE ADC Gain (Aux Channel Measurement) */
#define pREG_AFE_AFE_ADC_OFFSET_AUX      ((volatile uint32_t *)REG_AFE_AFE_ADC_OFFSET_AUX)       /* AFE ADC Offset (Aux Channel Measurement) */
#define pREG_AFE_AFE_DAC_OFFSET_UNITY    ((volatile uint32_t *)REG_AFE_AFE_DAC_OFFSET_UNITY)     /* AFE DAC Offset With Attenuator Disabled */
#define pREG_AFE_AFE_DAC_OFFSET_ATTEN    ((volatile uint32_t *)REG_AFE_AFE_DAC_OFFSET_ATTEN)     /* AFE DAC Offset With Attenuator Enabled */
#define pREG_AFE_AFE_DAC_GAIN            ((volatile uint32_t *)REG_AFE_AFE_DAC_GAIN)             /* AFE DAC Gain */
#define pREG_AFE_AFE_REF_TRIM0           ((volatile uint32_t *)REG_AFE_AFE_REF_TRIM0)            /* AFE Precision Reference Trim 0 */
#define pREG_AFE_AFE_REF_TRIM1           ((volatile uint32_t *)REG_AFE_AFE_REF_TRIM1)            /* AFE Precision Reference Trim 1 */
#define pREG_AFE_AFE_ALDO_TRIM           ((volatile uint32_t *)REG_AFE_AFE_ALDO_TRIM)            /* AFE Analog LDO Trim */
#define pREG_AFE_AFE_DAC_TRIM            ((volatile uint32_t *)REG_AFE_AFE_DAC_TRIM)             /* AFE DAC Trim */
#define pREG_AFE_AFE_INAMP_TRIM          ((volatile uint32_t *)REG_AFE_AFE_INAMP_TRIM)           /* AFE INAMP Trim */
#define pREG_AFE_AFE_EXBUF_TRIM          ((volatile uint32_t *)REG_AFE_AFE_EXBUF_TRIM)           /* AFE Excitation Buffer Trim */
#define pREG_AFE_AFE_TEMP_SENS_TRIM      ((volatile uint32_t *)REG_AFE_AFE_TEMP_SENS_TRIM)       /* AFE Temperature Sensor Trim */


/* =========================================================================
       CT
   ========================================================================= */
#define pREG_CT_CT_CDC_PWR               ((volatile uint32_t *)REG_CT_CT_CDC_PWR)                /* CT Power CDC control */
#define pREG_CT_CT_CFG1                  ((volatile uint32_t *)REG_CT_CT_CFG1)                   /* CT CapTouch Control Configuration Register 1 */
#define pREG_CT_CT_CFG2                  ((volatile uint32_t *)REG_CT_CT_CFG2)                   /* CT CapTouch Control Configuration Register 2 */
#define pREG_CT_CT_CFG3                  ((volatile uint32_t *)REG_CT_CT_CFG3)                   /* CT AFE Control Configuration Register 3 */
#define pREG_CT_CT_MEAS_SEL              ((volatile uint32_t *)REG_CT_CT_MEAS_SEL)               /* CT Capacitance measurement stage selection */
#define pREG_CT_CT_BASELINE_CTRL         ((volatile uint32_t *)REG_CT_CT_BASELINE_CTRL)          /* CT Baseline control settings */
#define pREG_CT_CT_AVG                   ((volatile uint32_t *)REG_CT_CT_AVG)                    /* CT Setup of the rank-and-average filtering */
#define pREG_CT_CT_CAL_EN                ((volatile uint32_t *)REG_CT_CT_CAL_EN)                 /* CT Enable calibration for measurement stages */
#define pREG_CT_CT_TOUCH_CFG1            ((volatile uint32_t *)REG_CT_CT_TOUCH_CFG1)             /* CT Touch Detection Thresholds */
#define pREG_CT_CT_TOUCH_CFG2            ((volatile uint32_t *)REG_CT_CT_TOUCH_CFG2)             /* CT Touch Detection time-out */
#define pREG_CT_CT_RELEASE_CFG1          ((volatile uint32_t *)REG_CT_CT_RELEASE_CFG1)           /* CT Release Detection Thresholds */
#define pREG_CT_CT_RELEASE_CFG2          ((volatile uint32_t *)REG_CT_CT_RELEASE_CFG2)           /* CT Release Detection time-out */
#define pREG_CT_CT_IEN                   ((volatile uint32_t *)REG_CT_CT_IEN)                    /* CT Interrupt Enable */
#define pREG_CT_CT_INT                   ((volatile uint32_t *)REG_CT_CT_INT)                    /* CT Primary Interrupt Register */
#define pREG_CT_CT_OFFS_HSTAT            ((volatile uint32_t *)REG_CT_CT_OFFS_HSTAT)             /* CT Stage offset high interrupt status */
#define pREG_CT_CT_OFFS_LSTAT            ((volatile uint32_t *)REG_CT_CT_OFFS_LSTAT)             /* CT Stage offset low interrupt status */
#define pREG_CT_CT_PROX_STAT             ((volatile uint32_t *)REG_CT_CT_PROX_STAT)              /* CT Stage proximity status */
#define pREG_CT_CT_FPROX_STAT            ((volatile uint32_t *)REG_CT_CT_FPROX_STAT)             /* CT Stage fast proximity status */
#define pREG_CT_CT_TOUCH_STAT            ((volatile uint32_t *)REG_CT_CT_TOUCH_STAT)             /* CT Stage touch/release status */
#define pREG_CT_CT_STAGE0_CFG            ((volatile uint32_t *)REG_CT_CT_STAGE0_CFG)             /* CT Stage 0 configuration */
#define pREG_CT_CT_STAGE1_CFG            ((volatile uint32_t *)REG_CT_CT_STAGE1_CFG)             /* CT Stage 1 configuration */
#define pREG_CT_CT_STAGE2_CFG            ((volatile uint32_t *)REG_CT_CT_STAGE2_CFG)             /* CT Stage 2 configuration */
#define pREG_CT_CT_STAGE3_CFG            ((volatile uint32_t *)REG_CT_CT_STAGE3_CFG)             /* CT Stage 3 configuration */
#define pREG_CT_CT_STAGE4_CFG            ((volatile uint32_t *)REG_CT_CT_STAGE4_CFG)             /* CT Stage 4 configuration */
#define pREG_CT_CT_STAGE5_CFG            ((volatile uint32_t *)REG_CT_CT_STAGE5_CFG)             /* CT Stage 5 configuration */
#define pREG_CT_CT_STAGE6_CFG            ((volatile uint32_t *)REG_CT_CT_STAGE6_CFG)             /* CT Stage 6 configuration */
#define pREG_CT_CT_STAGE7_CFG            ((volatile uint32_t *)REG_CT_CT_STAGE7_CFG)             /* CT Stage 7 configuration */
#define pREG_CT_CT_STAGE8_CFG            ((volatile uint32_t *)REG_CT_CT_STAGE8_CFG)             /* CT Stage 8 configuration */
#define pREG_CT_CT_STAGE9_CFG            ((volatile uint32_t *)REG_CT_CT_STAGE9_CFG)             /* CT Stage 9 configuration */
#define pREG_CT_CT_STAGE10_CFG           ((volatile uint32_t *)REG_CT_CT_STAGE10_CFG)            /* CT Stage 10 configuration */
#define pREG_CT_CT_STAGE11_CFG           ((volatile uint32_t *)REG_CT_CT_STAGE11_CFG)            /* CT Stage 11 configuration */
#define pREG_CT_CT_STAGE12_CFG           ((volatile uint32_t *)REG_CT_CT_STAGE12_CFG)            /* CT Stage 12 configuration */
#define pREG_CT_CT_STAGE13_CFG           ((volatile uint32_t *)REG_CT_CT_STAGE13_CFG)            /* CT Stage 13 configuration */
#define pREG_CT_CT_STAGE14_CFG           ((volatile uint32_t *)REG_CT_CT_STAGE14_CFG)            /* CT Stage 14 configuration */
#define pREG_CT_CT_STAGE15_CFG           ((volatile uint32_t *)REG_CT_CT_STAGE15_CFG)            /* CT Stage 15 configuration */
#define pREG_CT_CT_SENSOR_THR_CFG0       ((volatile uint32_t *)REG_CT_CT_SENSOR_THR_CFG0)        /* CT Stage 0 and Stage 1 Sensor Threshold */
#define pREG_CT_CT_SENSOR_THR_CFG1       ((volatile uint32_t *)REG_CT_CT_SENSOR_THR_CFG1)        /* CT Stage 2 and Stage 3 Sensor Threshold */
#define pREG_CT_CT_SENSOR_THR_CFG2       ((volatile uint32_t *)REG_CT_CT_SENSOR_THR_CFG2)        /* CT Stage 4 and Stage 5 Sensor Threshold */
#define pREG_CT_CT_SENSOR_THR_CFG3       ((volatile uint32_t *)REG_CT_CT_SENSOR_THR_CFG3)        /* CT Stage 6 and Stage 7 Sensor Threshold */
#define pREG_CT_CT_SENSOR_THR_CFG4       ((volatile uint32_t *)REG_CT_CT_SENSOR_THR_CFG4)        /* CT Stage 8 and Stage 9 Sensor Threshold */
#define pREG_CT_CT_SENSOR_THR_CFG5       ((volatile uint32_t *)REG_CT_CT_SENSOR_THR_CFG5)        /* CT Stage 10 and Stage 11 Sensor Threshold */
#define pREG_CT_CT_SENSOR_THR_CFG6       ((volatile uint32_t *)REG_CT_CT_SENSOR_THR_CFG6)        /* CT Stage 12 and Stage 13 Sensor Threshold */
#define pREG_CT_CT_SENSOR_THR_CFG7       ((volatile uint32_t *)REG_CT_CT_SENSOR_THR_CFG7)        /* CT Stage 14 and Stage 15 Sensor Threshold */
#define pREG_CT_CT_CDC_RES0              ((volatile uint32_t *)REG_CT_CT_CDC_RES0)               /* CT Stage 0 and Stage 1 Results */
#define pREG_CT_CT_CDC_RES1              ((volatile uint32_t *)REG_CT_CT_CDC_RES1)               /* CT Stage 2 and Stage 3 Results */
#define pREG_CT_CT_CDC_RES2              ((volatile uint32_t *)REG_CT_CT_CDC_RES2)               /* CT Stage 4 and Stage 5 Results */
#define pREG_CT_CT_CDC_RES3              ((volatile uint32_t *)REG_CT_CT_CDC_RES3)               /* CT Stage 6 and Stage 7 Results */
#define pREG_CT_CT_CDC_RES4              ((volatile uint32_t *)REG_CT_CT_CDC_RES4)               /* CT Stage 8 and Stage 9 Results */
#define pREG_CT_CT_CDC_RES5              ((volatile uint32_t *)REG_CT_CT_CDC_RES5)               /* CT Stage 10 and Stage 11 Results */
#define pREG_CT_CT_CDC_RES6              ((volatile uint32_t *)REG_CT_CT_CDC_RES6)               /* CT Stage 12 and Stage 13 Results */
#define pREG_CT_CT_CDC_RES7              ((volatile uint32_t *)REG_CT_CT_CDC_RES7)               /* CT Stage 14 and Stage 15 Results */
#define pREG_CT_CT_BASELINE0             ((volatile uint32_t *)REG_CT_CT_BASELINE0)              /* CT Stage 0 Fast Filter and Baseline Results */
#define pREG_CT_CT_BASELINE1             ((volatile uint32_t *)REG_CT_CT_BASELINE1)              /* CT Stage 1 Fast Filter and Baseline Results */
#define pREG_CT_CT_BASELINE2             ((volatile uint32_t *)REG_CT_CT_BASELINE2)              /* CT Stage 2 Fast Filter and Baseline Results */
#define pREG_CT_CT_BASELINE3             ((volatile uint32_t *)REG_CT_CT_BASELINE3)              /* CT Stage 3 Fast Filter and Baseline Results */
#define pREG_CT_CT_BASELINE4             ((volatile uint32_t *)REG_CT_CT_BASELINE4)              /* CT Stage 4 Fast Filter and Baseline Results */
#define pREG_CT_CT_BASELINE5             ((volatile uint32_t *)REG_CT_CT_BASELINE5)              /* CT Stage 5 Fast Filter and Baseline Results */
#define pREG_CT_CT_BASELINE6             ((volatile uint32_t *)REG_CT_CT_BASELINE6)              /* CT Stage 6 Fast Filter and Baseline Results */
#define pREG_CT_CT_BASELINE7             ((volatile uint32_t *)REG_CT_CT_BASELINE7)              /* CT Stage 7 Fast Filter and Baseline Results */
#define pREG_CT_CT_BASELINE8             ((volatile uint32_t *)REG_CT_CT_BASELINE8)              /* CT Stage 8 Fast Filter and Baseline Results */
#define pREG_CT_CT_BASELINE9             ((volatile uint32_t *)REG_CT_CT_BASELINE9)              /* CT Stage 9 Fast Filter and Baseline Results */
#define pREG_CT_CT_BASELINE10            ((volatile uint32_t *)REG_CT_CT_BASELINE10)             /* CT Stage 10 Fast Filter and Baseline Results */
#define pREG_CT_CT_BASELINE11            ((volatile uint32_t *)REG_CT_CT_BASELINE11)             /* CT Stage 11 Fast Filter and Baseline Results */
#define pREG_CT_CT_BASELINE12            ((volatile uint32_t *)REG_CT_CT_BASELINE12)             /* CT Stage 12 Fast Filter and Baseline Results */
#define pREG_CT_CT_BASELINE13            ((volatile uint32_t *)REG_CT_CT_BASELINE13)             /* CT Stage 13 Fast Filter and Baseline Results */
#define pREG_CT_CT_BASELINE14            ((volatile uint32_t *)REG_CT_CT_BASELINE14)             /* CT Stage 14 Fast Filter and Baseline Results */
#define pREG_CT_CT_BASELINE15            ((volatile uint32_t *)REG_CT_CT_BASELINE15)             /* CT Stage 15 Fast Filter and Baseline Results */
#define pREG_CT_CT_PK2PK0                ((volatile uint32_t *)REG_CT_CT_PK2PK0)                 /* CT Stage 0 and Stage 1 Peak-to-Peak Noise Results */
#define pREG_CT_CT_PK2PK1                ((volatile uint32_t *)REG_CT_CT_PK2PK1)                 /* CT Stage 2 and Stage 3 Peak-to-Peak Noise Results */
#define pREG_CT_CT_PK2PK2                ((volatile uint32_t *)REG_CT_CT_PK2PK2)                 /* CT Stage 4 and Stage 5 Peak-to-Peak Noise Results */
#define pREG_CT_CT_PK2PK3                ((volatile uint32_t *)REG_CT_CT_PK2PK3)                 /* CT Stage 6 and Stage 7 Peak-to-Peak Noise Results */
#define pREG_CT_CT_PK2PK4                ((volatile uint32_t *)REG_CT_CT_PK2PK4)                 /* CT Stage 8 and Stage 9 Peak-to-Peak Noise Results */
#define pREG_CT_CT_PK2PK5                ((volatile uint32_t *)REG_CT_CT_PK2PK5)                 /* CT Stage 10 and Stage 11 Peak-to-Peak Noise Results */
#define pREG_CT_CT_PK2PK6                ((volatile uint32_t *)REG_CT_CT_PK2PK6)                 /* CT Stage 12 and Stage 13 Peak-to-Peak Noise Results */
#define pREG_CT_CT_PK2PK7                ((volatile uint32_t *)REG_CT_CT_PK2PK7)                 /* CT Stage 14 and Stage 15 Peak-to-Peak Noise Results */


/* =========================================================================
       USB0
   ========================================================================= */
#define pREG_USB0_FADDR                  ((volatile  uint8_t *)REG_USB0_FADDR)                   /* USB0 Function Address Register */
#define pREG_USB0_POWER                  ((volatile  uint8_t *)REG_USB0_POWER)                   /* USB0 Power and Device Control Register */
#define pREG_USB0_INTRTX                 ((volatile uint16_t *)REG_USB0_INTRTX)                  /* USB0 Transmit Interrupt Register */
#define pREG_USB0_INTRRX                 ((volatile uint16_t *)REG_USB0_INTRRX)                  /* USB0 Receive Interrupt Register */
#define pREG_USB0_INTRTXE                ((volatile uint16_t *)REG_USB0_INTRTXE)                 /* USB0 Transmit Interrupt Enable Register */
#define pREG_USB0_INTRRXE                ((volatile uint16_t *)REG_USB0_INTRRXE)                 /* USB0 Receive Interrupt Enable Register */
#define pREG_USB0_IRQ                    ((volatile  uint8_t *)REG_USB0_IRQ)                     /* USB0 Common Interrupts Register */
#define pREG_USB0_IEN                    ((volatile  uint8_t *)REG_USB0_IEN)                     /* USB0 Common Interrupts Enable Register */
#define pREG_USB0_FRAME                  ((volatile uint16_t *)REG_USB0_FRAME)                   /* USB0 Frame Number Register */
#define pREG_USB0_INDEX                  ((volatile  uint8_t *)REG_USB0_INDEX)                   /* USB0 Index Register */
#define pREG_USB0_TESTMODE               ((volatile  uint8_t *)REG_USB0_TESTMODE)                /* USB0 Testmode Register */
#define pREG_USB0_EPI_TXMAXP0            ((volatile uint16_t *)REG_USB0_EPI_TXMAXP0)             /* USB0 EPn Transmit Maximum Packet Length Register */
#define pREG_USB0_EPI_TXCSR_H0           ((volatile uint16_t *)REG_USB0_EPI_TXCSR_H0)            /* USB0 EPn Transmit Configuration and Status (Host) Register */
#define pREG_USB0_EP0I_CSR0_P            ((volatile uint16_t *)REG_USB0_EP0I_CSR0_P)             /* USB0 EP0 Configuration and Status (Peripheral) Register */
#define pREG_USB0_EP0I_CSR0_H            ((volatile uint16_t *)REG_USB0_EP0I_CSR0_H)             /* USB0 EP0 Configuration and Status (Host) Register */
#define pREG_USB0_EPI_TXCSR_P0           ((volatile uint16_t *)REG_USB0_EPI_TXCSR_P0)            /* USB0 EPn Transmit Configuration and Status (Peripheral) Register */
#define pREG_USB0_EPI_RXMAXP0            ((volatile uint16_t *)REG_USB0_EPI_RXMAXP0)             /* USB0 EPn Receive Maximum Packet Length Register */
#define pREG_USB0_EPI_RXCSR_H0           ((volatile uint16_t *)REG_USB0_EPI_RXCSR_H0)            /* USB0 EPn Receive Configuration and Status (Host) Register */
#define pREG_USB0_EPI_RXCSR_P0           ((volatile uint16_t *)REG_USB0_EPI_RXCSR_P0)            /* USB0 EPn Receive Configuration and Status (Peripheral) Register */
#define pREG_USB0_EP0I_CNT0              ((volatile uint16_t *)REG_USB0_EP0I_CNT0)               /* USB0 EP0 Number of Received Bytes Register */
#define pREG_USB0_EPI_RXCNT0             ((volatile uint16_t *)REG_USB0_EPI_RXCNT0)              /* USB0 EPn Number of Bytes Received Register */
#define pREG_USB0_EP0I_CFGDATA0          ((volatile  uint8_t *)REG_USB0_EP0I_CFGDATA0)           /* USB0 EP0 Configuration Information Register */
#define pREG_USB0_EPI_FIFOSIZE0          ((volatile  uint8_t *)REG_USB0_EPI_FIFOSIZE0)           /* USB0 FIFO size */
#define pREG_USB0_FIFO0                  ((volatile uint32_t *)REG_USB0_FIFO0)                   /* USB0 FIFO Word (32-Bit) Register */
#define pREG_USB0_FIFO1                  ((volatile uint32_t *)REG_USB0_FIFO1)                   /* USB0 FIFO Word (32-Bit) Register */
#define pREG_USB0_FIFO2                  ((volatile uint32_t *)REG_USB0_FIFO2)                   /* USB0 FIFO Word (32-Bit) Register */
#define pREG_USB0_FIFO3                  ((volatile uint32_t *)REG_USB0_FIFO3)                   /* USB0 FIFO Word (32-Bit) Register */
#define pREG_USB0_FIFOH0                 ((volatile uint16_t *)REG_USB0_FIFOH0)                  /* USB0 FIFO Half-Word (16-Bit) Register */
#define pREG_USB0_FIFOH1                 ((volatile uint16_t *)REG_USB0_FIFOH1)                  /* USB0 FIFO Half-Word (16-Bit) Register */
#define pREG_USB0_FIFOH2                 ((volatile uint16_t *)REG_USB0_FIFOH2)                  /* USB0 FIFO Half-Word (16-Bit) Register */
#define pREG_USB0_FIFOH3                 ((volatile uint16_t *)REG_USB0_FIFOH3)                  /* USB0 FIFO Half-Word (16-Bit) Register */
#define pREG_USB0_FIFOB0                 ((volatile  uint8_t *)REG_USB0_FIFOB0)                  /* USB0 FIFO Byte (8-Bit) Register */
#define pREG_USB0_FIFOB1                 ((volatile  uint8_t *)REG_USB0_FIFOB1)                  /* USB0 FIFO Byte (8-Bit) Register */
#define pREG_USB0_FIFOB2                 ((volatile  uint8_t *)REG_USB0_FIFOB2)                  /* USB0 FIFO Byte (8-Bit) Register */
#define pREG_USB0_FIFOB3                 ((volatile  uint8_t *)REG_USB0_FIFOB3)                  /* USB0 FIFO Byte (8-Bit) Register */
#define pREG_USB0_DEV_CTL                ((volatile  uint8_t *)REG_USB0_DEV_CTL)                 /* USB0 Device Control Register */
#define pREG_USB0_MISC                   ((volatile  uint8_t *)REG_USB0_MISC)                    /* USB0 Miscellaneous Register */
#define pREG_USB0_EPINFO                 ((volatile  uint8_t *)REG_USB0_EPINFO)                  /* USB0 Endpoint Information Register */
#define pREG_USB0_RAMINFO                ((volatile  uint8_t *)REG_USB0_RAMINFO)                 /* USB0 RAM Information Register */
#define pREG_USB0_LINKINFO               ((volatile  uint8_t *)REG_USB0_LINKINFO)                /* USB0 Link Information Register */
#define pREG_USB0_FS_EOF1                ((volatile  uint8_t *)REG_USB0_FS_EOF1)                 /* USB0 Full-Speed EOF 1 Register */
#define pREG_USB0_SOFT_RST               ((volatile  uint8_t *)REG_USB0_SOFT_RST)                /* USB0 Software Reset Register */
#define pREG_USB0_EP0_TXMAXP             ((volatile uint16_t *)REG_USB0_EP0_TXMAXP)              /* USB0 EPn Transmit Maximum Packet Length Register */
#define pREG_USB0_EP1_TXMAXP             ((volatile uint16_t *)REG_USB0_EP1_TXMAXP)              /* USB0 EPn Transmit Maximum Packet Length Register */
#define pREG_USB0_EP2_TXMAXP             ((volatile uint16_t *)REG_USB0_EP2_TXMAXP)              /* USB0 EPn Transmit Maximum Packet Length Register */
#define pREG_USB0_EP3_TXMAXP             ((volatile uint16_t *)REG_USB0_EP3_TXMAXP)              /* USB0 EPn Transmit Maximum Packet Length Register */
#define pREG_USB0_EP0_TXCSR_P            ((volatile uint16_t *)REG_USB0_EP0_TXCSR_P)             /* USB0 EPn Transmit Configuration and Status (Peripheral) Register */
#define pREG_USB0_EP1_TXCSR_P            ((volatile uint16_t *)REG_USB0_EP1_TXCSR_P)             /* USB0 EPn Transmit Configuration and Status (Peripheral) Register */
#define pREG_USB0_EP2_TXCSR_P            ((volatile uint16_t *)REG_USB0_EP2_TXCSR_P)             /* USB0 EPn Transmit Configuration and Status (Peripheral) Register */
#define pREG_USB0_EP3_TXCSR_P            ((volatile uint16_t *)REG_USB0_EP3_TXCSR_P)             /* USB0 EPn Transmit Configuration and Status (Peripheral) Register */
#define pREG_USB0_EP0_CSR0_P             ((volatile uint16_t *)REG_USB0_EP0_CSR0_P)              /* USB0 EP0 Configuration and Status (Peripheral) Register */
#define pREG_USB0_EP0_CSR0_H             ((volatile uint16_t *)REG_USB0_EP0_CSR0_H)              /* USB0 EP0 Configuration and Status (Host) Register */
#define pREG_USB0_EP0_TXCSR_H            ((volatile uint16_t *)REG_USB0_EP0_TXCSR_H)             /* USB0 EPn Transmit Configuration and Status (Host) Register */
#define pREG_USB0_EP1_TXCSR_H            ((volatile uint16_t *)REG_USB0_EP1_TXCSR_H)             /* USB0 EPn Transmit Configuration and Status (Host) Register */
#define pREG_USB0_EP2_TXCSR_H            ((volatile uint16_t *)REG_USB0_EP2_TXCSR_H)             /* USB0 EPn Transmit Configuration and Status (Host) Register */
#define pREG_USB0_EP3_TXCSR_H            ((volatile uint16_t *)REG_USB0_EP3_TXCSR_H)             /* USB0 EPn Transmit Configuration and Status (Host) Register */
#define pREG_USB0_EP0_RXMAXP             ((volatile uint16_t *)REG_USB0_EP0_RXMAXP)              /* USB0 EPn Receive Maximum Packet Length Register */
#define pREG_USB0_EP1_RXMAXP             ((volatile uint16_t *)REG_USB0_EP1_RXMAXP)              /* USB0 EPn Receive Maximum Packet Length Register */
#define pREG_USB0_EP2_RXMAXP             ((volatile uint16_t *)REG_USB0_EP2_RXMAXP)              /* USB0 EPn Receive Maximum Packet Length Register */
#define pREG_USB0_EP3_RXMAXP             ((volatile uint16_t *)REG_USB0_EP3_RXMAXP)              /* USB0 EPn Receive Maximum Packet Length Register */
#define pREG_USB0_EP0_RXCSR_P            ((volatile uint16_t *)REG_USB0_EP0_RXCSR_P)             /* USB0 EPn Receive Configuration and Status (Peripheral) Register */
#define pREG_USB0_EP1_RXCSR_P            ((volatile uint16_t *)REG_USB0_EP1_RXCSR_P)             /* USB0 EPn Receive Configuration and Status (Peripheral) Register */
#define pREG_USB0_EP2_RXCSR_P            ((volatile uint16_t *)REG_USB0_EP2_RXCSR_P)             /* USB0 EPn Receive Configuration and Status (Peripheral) Register */
#define pREG_USB0_EP3_RXCSR_P            ((volatile uint16_t *)REG_USB0_EP3_RXCSR_P)             /* USB0 EPn Receive Configuration and Status (Peripheral) Register */
#define pREG_USB0_EP0_RXCSR_H            ((volatile uint16_t *)REG_USB0_EP0_RXCSR_H)             /* USB0 EPn Receive Configuration and Status (Host) Register */
#define pREG_USB0_EP1_RXCSR_H            ((volatile uint16_t *)REG_USB0_EP1_RXCSR_H)             /* USB0 EPn Receive Configuration and Status (Host) Register */
#define pREG_USB0_EP2_RXCSR_H            ((volatile uint16_t *)REG_USB0_EP2_RXCSR_H)             /* USB0 EPn Receive Configuration and Status (Host) Register */
#define pREG_USB0_EP3_RXCSR_H            ((volatile uint16_t *)REG_USB0_EP3_RXCSR_H)             /* USB0 EPn Receive Configuration and Status (Host) Register */
#define pREG_USB0_EP0_CNT0               ((volatile uint16_t *)REG_USB0_EP0_CNT0)                /* USB0 EP0 Number of Received Bytes Register */
#define pREG_USB0_EP0_RXCNT              ((volatile uint16_t *)REG_USB0_EP0_RXCNT)               /* USB0 EPn Number of Bytes Received Register */
#define pREG_USB0_EP1_RXCNT              ((volatile uint16_t *)REG_USB0_EP1_RXCNT)               /* USB0 EPn Number of Bytes Received Register */
#define pREG_USB0_EP2_RXCNT              ((volatile uint16_t *)REG_USB0_EP2_RXCNT)               /* USB0 EPn Number of Bytes Received Register */
#define pREG_USB0_EP3_RXCNT              ((volatile uint16_t *)REG_USB0_EP3_RXCNT)               /* USB0 EPn Number of Bytes Received Register */
#define pREG_USB0_EP0_FIFOSIZE           ((volatile  uint8_t *)REG_USB0_EP0_FIFOSIZE)            /* USB0 FIFO size */
#define pREG_USB0_EP1_FIFOSIZE           ((volatile  uint8_t *)REG_USB0_EP1_FIFOSIZE)            /* USB0 FIFO size */
#define pREG_USB0_EP2_FIFOSIZE           ((volatile  uint8_t *)REG_USB0_EP2_FIFOSIZE)            /* USB0 FIFO size */
#define pREG_USB0_EP3_FIFOSIZE           ((volatile  uint8_t *)REG_USB0_EP3_FIFOSIZE)            /* USB0 FIFO size */
#define pREG_USB0_EP0_CFGDATA0           ((volatile  uint8_t *)REG_USB0_EP0_CFGDATA0)            /* USB0 EP0 Configuration Information Register */
#define pREG_USB0_DMA_IRQ                ((volatile  uint8_t *)REG_USB0_DMA_IRQ)                 /* USB0 DMA Interrupt Register */
#define pREG_USB0_DMA0_CTL               ((volatile uint16_t *)REG_USB0_DMA0_CTL)                /* USB0 DMA Channel n Control Register */
#define pREG_USB0_DMA1_CTL               ((volatile uint16_t *)REG_USB0_DMA1_CTL)                /* USB0 DMA Channel n Control Register */
#define pREG_USB0_DMA0_ADDR              ((void * volatile *)REG_USB0_DMA0_ADDR)                 /* USB0 DMA Channel n Address Register */
#define pREG_USB0_DMA1_ADDR              ((void * volatile *)REG_USB0_DMA1_ADDR)                 /* USB0 DMA Channel n Address Register */
#define pREG_USB0_DMA0_CNT               ((volatile uint32_t *)REG_USB0_DMA0_CNT)                /* USB0 DMA Channel n Count Register */
#define pREG_USB0_DMA1_CNT               ((volatile uint32_t *)REG_USB0_DMA1_CNT)                /* USB0 DMA Channel n Count Register */
#define pREG_USB0_RXDPKTBUFDIS           ((volatile uint16_t *)REG_USB0_RXDPKTBUFDIS)            /* USB0 RX Double Packet Buffer Disable for Endpoints 1 to 3 */
#define pREG_USB0_TXDPKTBUFDIS           ((volatile uint16_t *)REG_USB0_TXDPKTBUFDIS)            /* USB0 TX Double Packet Buffer Disable  for Endpoints 1 to 3 */
#define pREG_USB0_CT_UCH                 ((volatile uint16_t *)REG_USB0_CT_UCH)                  /* USB0 Chirp Timeout Register */
#define pREG_USB0_LPM_ATTR               ((volatile uint16_t *)REG_USB0_LPM_ATTR)                /* USB0 LPM Attribute Register */
#define pREG_USB0_LPM_CTL                ((volatile  uint8_t *)REG_USB0_LPM_CTL)                 /* USB0 LPM Control Register */
#define pREG_USB0_LPM_IEN                ((volatile  uint8_t *)REG_USB0_LPM_IEN)                 /* USB0 LPM Interrupt Enable Register */
#define pREG_USB0_LPM_IRQ                ((volatile  uint8_t *)REG_USB0_LPM_IRQ)                 /* USB0 LPM Interrupt Status Register */
#define pREG_USB0_PHY_CTL                ((volatile uint16_t *)REG_USB0_PHY_CTL)                 /* USB0 FS PHY Control */
#define pREG_USB0_PHY_STAT               ((volatile uint16_t *)REG_USB0_PHY_STAT)                /* USB0 FS PHY Status */
#define pREG_USB0_RAM_ADDR               ((volatile uint32_t *)REG_USB0_RAM_ADDR)                /* USB0 RAM Address Register */
#define pREG_USB0_RAM_DATA               ((volatile uint32_t *)REG_USB0_RAM_DATA)                /* USB0 RAM Data Register */


/* =========================================================================
       NVIC
   ========================================================================= */
#define pREG_NVIC_INTNUM                 ((volatile uint32_t *)REG_NVIC_INTNUM)                  /* NVIC Interrupt Control Type */
#define pREG_NVIC_STKSTA                 ((volatile uint32_t *)REG_NVIC_STKSTA)                  /* NVIC Systick Control and Status */
#define pREG_NVIC_STKLD                  ((volatile uint32_t *)REG_NVIC_STKLD)                   /* NVIC Systick Reload Value */
#define pREG_NVIC_STKVAL                 ((volatile uint32_t *)REG_NVIC_STKVAL)                  /* NVIC Systick Current Value */
#define pREG_NVIC_STKCAL                 ((volatile uint32_t *)REG_NVIC_STKCAL)                  /* NVIC Systick Calibration Value */
#define pREG_NVIC_INTSETE0               ((volatile uint32_t *)REG_NVIC_INTSETE0)                /* NVIC IRQ0..31 Set_Enable */
#define pREG_NVIC_INTSETE1               ((volatile uint32_t *)REG_NVIC_INTSETE1)                /* NVIC IRQ32..63 Set_Enable */
#define pREG_NVIC_INTCLRE0               ((volatile uint32_t *)REG_NVIC_INTCLRE0)                /* NVIC IRQ0..31 Clear_Enable */
#define pREG_NVIC_INTCLRE1               ((volatile uint32_t *)REG_NVIC_INTCLRE1)                /* NVIC IRQ32..63 Clear_Enable */
#define pREG_NVIC_INTSETP0               ((volatile uint32_t *)REG_NVIC_INTSETP0)                /* NVIC IRQ0..31 Set_Pending */
#define pREG_NVIC_INTSETP1               ((volatile uint32_t *)REG_NVIC_INTSETP1)                /* NVIC IRQ32..63 Set_Pending */
#define pREG_NVIC_INTCLRP0               ((volatile uint32_t *)REG_NVIC_INTCLRP0)                /* NVIC IRQ0..31 Clear_Pending */
#define pREG_NVIC_INTCLRP1               ((volatile uint32_t *)REG_NVIC_INTCLRP1)                /* NVIC IRQ32..63 Clear_Pending */
#define pREG_NVIC_INTACT0                ((volatile uint32_t *)REG_NVIC_INTACT0)                 /* NVIC IRQ0..31 Active Bit */
#define pREG_NVIC_INTACT1                ((volatile uint32_t *)REG_NVIC_INTACT1)                 /* NVIC IRQ32..63 Active Bit */
#define pREG_NVIC_INTPRI0                ((volatile uint32_t *)REG_NVIC_INTPRI0)                 /* NVIC IRQ0..3 Priority */
#define pREG_NVIC_INTPRI1                ((volatile uint32_t *)REG_NVIC_INTPRI1)                 /* NVIC IRQ4..7 Priority */
#define pREG_NVIC_INTPRI2                ((volatile uint32_t *)REG_NVIC_INTPRI2)                 /* NVIC IRQ8..11 Priority */
#define pREG_NVIC_INTPRI3                ((volatile uint32_t *)REG_NVIC_INTPRI3)                 /* NVIC IRQ12..15 Priority */
#define pREG_NVIC_INTPRI4                ((volatile uint32_t *)REG_NVIC_INTPRI4)                 /* NVIC IRQ16..19 Priority */
#define pREG_NVIC_INTPRI5                ((volatile uint32_t *)REG_NVIC_INTPRI5)                 /* NVIC IRQ20..23 Priority */
#define pREG_NVIC_INTPRI6                ((volatile uint32_t *)REG_NVIC_INTPRI6)                 /* NVIC IRQ24..27 Priority */
#define pREG_NVIC_INTPRI7                ((volatile uint32_t *)REG_NVIC_INTPRI7)                 /* NVIC IRQ28..31 Priority */
#define pREG_NVIC_INTPRI8                ((volatile uint32_t *)REG_NVIC_INTPRI8)                 /* NVIC IRQ32..35 Priority */
#define pREG_NVIC_INTPRI9                ((volatile uint32_t *)REG_NVIC_INTPRI9)                 /* NVIC IRQ36..39 Priority */
#define pREG_NVIC_INTPRI10               ((volatile uint32_t *)REG_NVIC_INTPRI10)                /* NVIC IRQ40..43 Priority */
#define pREG_NVIC_INTCPID                ((volatile uint32_t *)REG_NVIC_INTCPID)                 /* NVIC CPUID Base */
#define pREG_NVIC_INTSTA                 ((volatile uint32_t *)REG_NVIC_INTSTA)                  /* NVIC Interrupt Control State */
#define pREG_NVIC_INTVEC                 ((volatile uint32_t *)REG_NVIC_INTVEC)                  /* NVIC Vector Table Offset */
#define pREG_NVIC_INTAIRC                ((volatile uint32_t *)REG_NVIC_INTAIRC)                 /* NVIC Application Interrupt/Reset Control */
#define pREG_NVIC_INTCON0                ((volatile uint16_t *)REG_NVIC_INTCON0)                 /* NVIC System Control */
#define pREG_NVIC_INTCON1                ((volatile uint32_t *)REG_NVIC_INTCON1)                 /* NVIC Configuration Control */
#define pREG_NVIC_INTSHPRIO0             ((volatile uint32_t *)REG_NVIC_INTSHPRIO0)              /* NVIC System Handlers 4-7 Priority */
#define pREG_NVIC_INTSHPRIO1             ((volatile uint32_t *)REG_NVIC_INTSHPRIO1)              /* NVIC System Handlers 8-11 Priority */
#define pREG_NVIC_INTSHPRIO3             ((volatile uint32_t *)REG_NVIC_INTSHPRIO3)              /* NVIC System Handlers 12-15 Priority */
#define pREG_NVIC_INTSHCSR               ((volatile uint32_t *)REG_NVIC_INTSHCSR)                /* NVIC System Handler Control and State */
#define pREG_NVIC_INTCFSR                ((volatile uint32_t *)REG_NVIC_INTCFSR)                 /* NVIC Configurable Fault Status */
#define pREG_NVIC_INTHFSR                ((volatile uint32_t *)REG_NVIC_INTHFSR)                 /* NVIC Hard Fault Status */
#define pREG_NVIC_INTDFSR                ((volatile uint32_t *)REG_NVIC_INTDFSR)                 /* NVIC Debug Fault Status */
#define pREG_NVIC_INTMMAR                ((volatile uint32_t *)REG_NVIC_INTMMAR)                 /* NVIC Mem Manage Address */
#define pREG_NVIC_INTBFAR                ((volatile uint32_t *)REG_NVIC_INTBFAR)                 /* NVIC Bus Fault Address */
#define pREG_NVIC_INTAFSR                ((volatile uint32_t *)REG_NVIC_INTAFSR)                 /* NVIC Auxiliary Fault Status */
#define pREG_NVIC_INTPFR0                ((volatile uint32_t *)REG_NVIC_INTPFR0)                 /* NVIC Processor Feature Register 0 */
#define pREG_NVIC_INTPFR1                ((volatile uint32_t *)REG_NVIC_INTPFR1)                 /* NVIC Processor Feature Register 1 */
#define pREG_NVIC_INTDFR0                ((volatile uint32_t *)REG_NVIC_INTDFR0)                 /* NVIC Debug Feature Register 0 */
#define pREG_NVIC_INTAFR0                ((volatile uint32_t *)REG_NVIC_INTAFR0)                 /* NVIC Auxiliary Feature Register 0 */
#define pREG_NVIC_INTMMFR0               ((volatile uint32_t *)REG_NVIC_INTMMFR0)                /* NVIC Memory Model Feature Register 0 */
#define pREG_NVIC_INTMMFR1               ((volatile uint32_t *)REG_NVIC_INTMMFR1)                /* NVIC Memory Model Feature Register 1 */
#define pREG_NVIC_INTMMFR2               ((volatile uint32_t *)REG_NVIC_INTMMFR2)                /* NVIC Memory Model Feature Register 2 */
#define pREG_NVIC_INTMMFR3               ((volatile uint32_t *)REG_NVIC_INTMMFR3)                /* NVIC Memory Model Feature Register 3 */
#define pREG_NVIC_INTISAR0               ((volatile uint32_t *)REG_NVIC_INTISAR0)                /* NVIC ISA Feature Register 0 */
#define pREG_NVIC_INTISAR1               ((volatile uint32_t *)REG_NVIC_INTISAR1)                /* NVIC ISA Feature Register 1 */
#define pREG_NVIC_INTISAR2               ((volatile uint32_t *)REG_NVIC_INTISAR2)                /* NVIC ISA Feature Register 2 */
#define pREG_NVIC_INTISAR3               ((volatile uint32_t *)REG_NVIC_INTISAR3)                /* NVIC ISA Feature Register 3 */
#define pREG_NVIC_INTISAR4               ((volatile uint32_t *)REG_NVIC_INTISAR4)                /* NVIC ISA Feature Register 4 */
#define pREG_NVIC_INTTRGI                ((volatile uint32_t *)REG_NVIC_INTTRGI)                 /* NVIC Software Trigger Interrupt Register */
#define pREG_NVIC_INTPID4                ((volatile uint32_t *)REG_NVIC_INTPID4)                 /* NVIC Peripheral Identification Register 4 */
#define pREG_NVIC_INTPID5                ((volatile uint32_t *)REG_NVIC_INTPID5)                 /* NVIC Peripheral Identification Register 5 */
#define pREG_NVIC_INTPID6                ((volatile uint32_t *)REG_NVIC_INTPID6)                 /* NVIC Peripheral Identification Register 6 */
#define pREG_NVIC_INTPID7                ((volatile uint32_t *)REG_NVIC_INTPID7)                 /* NVIC Peripheral Identification Register 7 */
#define pREG_NVIC_INTPID0                ((volatile uint32_t *)REG_NVIC_INTPID0)                 /* NVIC Peripheral Identification Bits7:0 */
#define pREG_NVIC_INTPID1                ((volatile uint32_t *)REG_NVIC_INTPID1)                 /* NVIC Peripheral Identification Bits15:8 */
#define pREG_NVIC_INTPID2                ((volatile uint32_t *)REG_NVIC_INTPID2)                 /* NVIC Peripheral Identification Bits16:23 */
#define pREG_NVIC_INTPID3                ((volatile uint32_t *)REG_NVIC_INTPID3)                 /* NVIC Peripheral Identification Bits24:31 */
#define pREG_NVIC_INTCID0                ((volatile uint32_t *)REG_NVIC_INTCID0)                 /* NVIC Component Identification Bits7:0 */
#define pREG_NVIC_INTCID1                ((volatile uint32_t *)REG_NVIC_INTCID1)                 /* NVIC Component Identification Bits15:8 */
#define pREG_NVIC_INTCID2                ((volatile uint32_t *)REG_NVIC_INTCID2)                 /* NVIC Component Identification Bits16:23 */
#define pREG_NVIC_INTCID3                ((volatile uint32_t *)REG_NVIC_INTCID3)                 /* NVIC Component Identification Bits24:31 */

#ifdef _MISRA_RULES
#pragma diag(pop)
#endif /* _MISRA_RULES */

#endif	/* end ifndef _CDEF_ADuCM350_H */
