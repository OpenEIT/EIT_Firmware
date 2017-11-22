/* ================================================================================

     Project      :   ADuCM350
     File         :   defADuCM350.h
     Description  :   Register Definitions

     Date         :   11-07-2012


     Copyright (c) 2011-2013 Analog Devices, Inc.  All Rights Reserved.
     This software is proprietary and confidential to Analog Devices, Inc. and
     its licensors.

     This file was auto-generated. Do not make local changes to this file.

   ================================================================================ */

#ifndef _DEF_ADuCM350_H
#define _DEF_ADuCM350_H

#if defined (_MISRA_RULES)
#pragma diag(push)
#pragma diag(suppress:misra_rule_19_7:"ADI header allows function-like macros")
#pragma diag(suppress:misra_rule_19_13:"ADI headers can use the # and ## preprocessor operators")
#endif /* _MISRA_RULES */

/* do not add casts to literal constants in assembly code */
#if defined(_LANGUAGE_ASM) || defined(__ASSEMBLER__)
#define _ADI_MSK( mask, type ) (mask) /* Make a bitmask */
#else
#define _ADI_MSK( mask, type ) ((type)(mask)) /* Make a bitmask */
#endif

#ifdef _MISRA_RULES
#pragma diag(pop)
#endif /* _MISRA_RULES */

#ifndef __ADI_GENERATED_DEF_HEADERS__
#define __ADI_GENERATED_DEF_HEADERS__    1
#endif

/* MMR modules defined for the ADuCM350 */

#define __ADI_HAS_GPT__           1
#define __ADI_HAS_PWR__           1
#define __ADI_HAS_WUT__           1
#define __ADI_HAS_WDT__           1
#define __ADI_HAS_RTC__           1
#define __ADI_HAS_I2C__           1
#define __ADI_HAS_SPI__           1
#define __ADI_HAS_UART__          1
#define __ADI_HAS_I2S__           1
#define __ADI_HAS_BEEP__          1
#define __ADI_HAS_RNG__           1
#define __ADI_HAS_LCD__           1
#define __ADI_HAS_AI__            1
#define __ADI_HAS_DMA__           1
#define __ADI_HAS_FEE__           1
#define __ADI_HAS_GPF__           1
#define __ADI_HAS_GPIO__          1
#define __ADI_HAS_SPIH__          1
#define __ADI_HAS_SYSCLK__        1
#define __ADI_HAS_BUSM__          1
#define __ADI_HAS_CRC__           1
#define __ADI_HAS_PDI__           1
#define __ADI_HAS_AFE__           1
#define __ADI_HAS_CT__            1
#define __ADI_HAS_USB__           1
#define __ADI_HAS_NVIC__          1
/* ==================================================
        GPT0 Registers
   ================================================== */

/* =========================
        GPT0
   ========================= */
#define REG_GPT0_GPTLD                       0x40000000         /* GPT0 16-bit load value */
#define REG_GPT0_GPTVAL                      0x40000004         /* GPT0 16-bit timer value */
#define REG_GPT0_GPTCON                      0x40000008         /* GPT0 Control */
#define REG_GPT0_GPTCLRI                     0x4000000C         /* GPT0 Clear Interrupt */
#define REG_GPT0_GPTCAP                      0x40000010         /* GPT0 Capture */
#define REG_GPT0_GPTALD                      0x40000014         /* GPT0 16-bit load value, asynchronous */
#define REG_GPT0_GPTAVAL                     0x40000018         /* GPT0 16-bit timer value, asynchronous */
#define REG_GPT0_GPTSTA                      0x4000001C         /* GPT0 Status */
#define REG_GPT0_GPTPCON                     0x40000020         /* GPT0 PWM Control Register */
#define REG_GPT0_GPTPMAT                     0x40000024         /* GPT0 PWM Match Value */

/* =========================
        GPT1
   ========================= */
#define REG_GPT1_GPTLD                       0x40000400         /* GPT1 16-bit load value */
#define REG_GPT1_GPTVAL                      0x40000404         /* GPT1 16-bit timer value */
#define REG_GPT1_GPTCON                      0x40000408         /* GPT1 Control */
#define REG_GPT1_GPTCLRI                     0x4000040C         /* GPT1 Clear Interrupt */
#define REG_GPT1_GPTCAP                      0x40000410         /* GPT1 Capture */
#define REG_GPT1_GPTALD                      0x40000414         /* GPT1 16-bit load value, asynchronous */
#define REG_GPT1_GPTAVAL                     0x40000418         /* GPT1 16-bit timer value, asynchronous */
#define REG_GPT1_GPTSTA                      0x4000041C         /* GPT1 Status */
#define REG_GPT1_GPTPCON                     0x40000420         /* GPT1 PWM Control Register */
#define REG_GPT1_GPTPMAT                     0x40000424         /* GPT1 PWM Match Value */

/* =========================
        GPT2
   ========================= */
#define REG_GPT2_GPTLD                       0x40000800         /* GPT2 16-bit load value */
#define REG_GPT2_GPTVAL                      0x40000804         /* GPT2 16-bit timer value */
#define REG_GPT2_GPTCON                      0x40000808         /* GPT2 Control */
#define REG_GPT2_GPTCLRI                     0x4000080C         /* GPT2 Clear Interrupt */
#define REG_GPT2_GPTCAP                      0x40000810         /* GPT2 Capture */
#define REG_GPT2_GPTALD                      0x40000814         /* GPT2 16-bit load value, asynchronous */
#define REG_GPT2_GPTAVAL                     0x40000818         /* GPT2 16-bit timer value, asynchronous */
#define REG_GPT2_GPTSTA                      0x4000081C         /* GPT2 Status */
#define REG_GPT2_GPTPCON                     0x40000820         /* GPT2 PWM Control Register */
#define REG_GPT2_GPTPMAT                     0x40000824         /* GPT2 PWM Match Value */

/* =========================
        GPT
   ========================= */
/* ------------------------------------------------------------------------------------------------------------------------
        GPT_GPTCON                                                               Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_GPT_GPTCON_EVENTEN                                                  12                               /* Event select */
#define BITP_GPT_GPTCON_EVENT                                                     8                               /* Event select range */
#define BITP_GPT_GPTCON_RLD                                                       7                               /* Reload control */
#define BITP_GPT_GPTCON_CLK                                                       5                               /* Clock select */
#define BITP_GPT_GPTCON_ENABLE                                                    4                               /* Timer enable */
#define BITP_GPT_GPTCON_MOD                                                       3                               /* Timer mode */
#define BITP_GPT_GPTCON_UP                                                        2                               /* Count up */
#define BITP_GPT_GPTCON_PRE                                                       0                               /* Prescaler */
#define BITM_GPT_GPTCON_EVENTEN                                                  (_ADI_MSK(0x00001000,uint16_t))  /* Event select */
#define BITM_GPT_GPTCON_EVENT                                                    (_ADI_MSK(0x00000F00,uint16_t))  /* Event select range */
#define BITM_GPT_GPTCON_RLD                                                      (_ADI_MSK(0x00000080,uint16_t))  /* Reload control */
#define BITM_GPT_GPTCON_CLK                                                      (_ADI_MSK(0x00000060,uint16_t))  /* Clock select */
#define BITM_GPT_GPTCON_ENABLE                                                   (_ADI_MSK(0x00000010,uint16_t))  /* Timer enable */
#define BITM_GPT_GPTCON_MOD                                                      (_ADI_MSK(0x00000008,uint16_t))  /* Timer mode */
#define BITM_GPT_GPTCON_UP                                                       (_ADI_MSK(0x00000004,uint16_t))  /* Count up */
#define BITM_GPT_GPTCON_PRE                                                      (_ADI_MSK(0x00000003,uint16_t))  /* Prescaler */

/* ------------------------------------------------------------------------------------------------------------------------
        GPT_GPTCLRI                                                              Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_GPT_GPTCLRI_CAP                                                      1                               /* Clear captured event interrupt */
#define BITP_GPT_GPTCLRI_TMOUT                                                    0                               /* Clear timeout interrupt */
#define BITM_GPT_GPTCLRI_CAP                                                     (_ADI_MSK(0x00000002,uint16_t))  /* Clear captured event interrupt */
#define BITM_GPT_GPTCLRI_TMOUT                                                   (_ADI_MSK(0x00000001,uint16_t))  /* Clear timeout interrupt */

/* ------------------------------------------------------------------------------------------------------------------------
        GPT_GPTSTA                                                               Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_GPT_GPTSTA_PDOK                                                      7                               /* GPTCLRI synchronization */
#define BITP_GPT_GPTSTA_BUSY                                                      6                               /* Timer Busy */
#define BITP_GPT_GPTSTA_CAP                                                       1                               /* Capture event pending */
#define BITP_GPT_GPTSTA_TMOUT                                                     0                               /* Timeout event occurred */
#define BITM_GPT_GPTSTA_PDOK                                                     (_ADI_MSK(0x00000080,uint16_t))  /* GPTCLRI synchronization */
#define BITM_GPT_GPTSTA_BUSY                                                     (_ADI_MSK(0x00000040,uint16_t))  /* Timer Busy */
#define BITM_GPT_GPTSTA_CAP                                                      (_ADI_MSK(0x00000002,uint16_t))  /* Capture event pending */
#define BITM_GPT_GPTSTA_TMOUT                                                    (_ADI_MSK(0x00000001,uint16_t))  /* Timeout event occurred */

/* ------------------------------------------------------------------------------------------------------------------------
        GPT_GPTPCON                                                              Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_GPT_GPTPCON_IDLE_STATE                                               1                               /* PWM Idle State */
#define BITP_GPT_GPTPCON_MATCH_EN                                                 0                               /* PWM Match enabled */
#define BITM_GPT_GPTPCON_IDLE_STATE                                              (_ADI_MSK(0x00000002,uint16_t))  /* PWM Idle State */
#define BITM_GPT_GPTPCON_MATCH_EN                                                (_ADI_MSK(0x00000001,uint16_t))  /* PWM Match enabled */

/* ==================================================
        always_on Registers
   ================================================== */

/* =========================
        PWR
   ========================= */
#define REG_PWR_PWRMOD                       0x40002400         /* PWR Power modes */
#define REG_PWR_PWRKEY                       0x40002404         /* PWR Key protection for PWRMOD */
#define REG_PWR_PSMCON                       0x40002408         /* PWR PSM Configuration */
#define REG_PWR_OSCKEY                       0x4000240C         /* PWR Key protection for OSCCTRL */
#define REG_PWR_OSCCTRL                      0x40002410         /* PWR Oscillator control */
#define REG_PWR_EI0CFG                       0x40002420         /* PWR External Interrupt configuration 0 */
#define REG_PWR_EI1CFG                       0x40002424         /* PWR External Interrupt configuration 1 */
#define REG_PWR_EI2CFG                       0x40002428         /* PWR External Interrupt configuration 2 */
#define REG_PWR_EICLR                        0x40002430         /* PWR External Interrupt clear */
#define REG_PWR_NMICLR                       0x40002434         /* PWR Non-maskable interrupt clear */
#define REG_PWR_USBWKSTAT                    0x40002438         /* PWR USB Wakeup Status */
#define REG_PWR_RSTSTA                       0x40002440         /* PWR Reset status */
#define REG_PWR_VCCMCON                      0x40002488         /* PWR VCCM Control and Status */
#define REG_PWR_VBACKCON                     0x4000248C         /* PWR VBACK control and status */

/* =========================
        PWR
   ========================= */
/* ------------------------------------------------------------------------------------------------------------------------
        PWR_PWRMOD                                                               Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_PWR_PWRMOD_RAM0_RET                                                 15                               /* Retention for RAM 0 */
#define BITP_PWR_PWRMOD_WICENACK                                                  3                               /* WIC Acknowledgment for SLEEPDEEP */
#define BITP_PWR_PWRMOD_PWRMOD                                                    0                               /* Power modes control bits */
#define BITM_PWR_PWRMOD_RAM0_RET                                                 (_ADI_MSK(0x00008000,uint16_t))  /* Retention for RAM 0 */
#define BITM_PWR_PWRMOD_WICENACK                                                 (_ADI_MSK(0x00000008,uint16_t))  /* WIC Acknowledgment for SLEEPDEEP */
#define BITM_PWR_PWRMOD_PWRMOD                                                   (_ADI_MSK(0x00000003,uint16_t))  /* Power modes control bits */

/* ------------------------------------------------------------------------------------------------------------------------
        PWR_PSMCON                                                               Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_PWR_PSMCON_VCCMPSMSTAT                                               7                               /* VCCM PSM current status */
#define BITP_PWR_PSMCON_VCCMPSMFLG                                                6                               /* VCCM PSM sticky flag */
#define BITP_PWR_PSMCON_VCCMPSMIRQ_OFF                                            4                               /* disable VCCM PSM to generate NMI interrupt */
#define BITP_PWR_PSMCON_DVDDPSMSTAT                                               3                               /* DVDD PSM current status */
#define BITP_PWR_PSMCON_DVDDPSMFLG                                                2                               /* DVDD PSM sticky flag. */
#define BITP_PWR_PSMCON_DVDDPSMIRQ_OFF                                            0                               /* disable DVDD PSM to generate NMI interrupt */
#define BITM_PWR_PSMCON_VCCMPSMSTAT                                              (_ADI_MSK(0x00000080,uint16_t))  /* VCCM PSM current status */
#define BITM_PWR_PSMCON_VCCMPSMFLG                                               (_ADI_MSK(0x00000040,uint16_t))  /* VCCM PSM sticky flag */
#define BITM_PWR_PSMCON_VCCMPSMIRQ_OFF                                           (_ADI_MSK(0x00000010,uint16_t))  /* disable VCCM PSM to generate NMI interrupt */
#define BITM_PWR_PSMCON_DVDDPSMSTAT                                              (_ADI_MSK(0x00000008,uint16_t))  /* DVDD PSM current status */
#define BITM_PWR_PSMCON_DVDDPSMFLG                                               (_ADI_MSK(0x00000004,uint16_t))  /* DVDD PSM sticky flag. */
#define BITM_PWR_PSMCON_DVDDPSMIRQ_OFF                                           (_ADI_MSK(0x00000001,uint16_t))  /* disable DVDD PSM to generate NMI interrupt */

/* ------------------------------------------------------------------------------------------------------------------------
        PWR_OSCCTRL                                                              Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_PWR_OSCCTRL_HFXTALOK                                                11                               /* status of HFXTAL oscillator */
#define BITP_PWR_OSCCTRL_LFXTALOK                                                10                               /* status of LFXTAL oscillator */
#define BITP_PWR_OSCCTRL_HFOSCOK                                                  9                               /* status of HFOSC oscillator */
#define BITP_PWR_OSCCTRL_LFOSCOK                                                  8                               /* status of LFOSC oscillator */
#define BITP_PWR_OSCCTRL_HFXTALEN                                                 3                               /* high frequency crystal oscillator enable */
#define BITP_PWR_OSCCTRL_LFXTALEN                                                 2                               /* Low frequency crystal oscillator enable */
#define BITP_PWR_OSCCTRL_HFOSCEN                                                  1                               /* high frequency internal oscillator enable */
#define BITP_PWR_OSCCTRL_LFOSCEN                                                  0                               /* Low frequency internal oscillator enable */
#define BITM_PWR_OSCCTRL_HFXTALOK                                                (_ADI_MSK(0x00000800,uint16_t))  /* status of HFXTAL oscillator */
#define BITM_PWR_OSCCTRL_LFXTALOK                                                (_ADI_MSK(0x00000400,uint16_t))  /* status of LFXTAL oscillator */
#define BITM_PWR_OSCCTRL_HFOSCOK                                                 (_ADI_MSK(0x00000200,uint16_t))  /* status of HFOSC oscillator */
#define BITM_PWR_OSCCTRL_LFOSCOK                                                 (_ADI_MSK(0x00000100,uint16_t))  /* status of LFOSC oscillator */
#define BITM_PWR_OSCCTRL_HFXTALEN                                                (_ADI_MSK(0x00000008,uint16_t))  /* high frequency crystal oscillator enable */
#define BITM_PWR_OSCCTRL_LFXTALEN                                                (_ADI_MSK(0x00000004,uint16_t))  /* Low frequency crystal oscillator enable */
#define BITM_PWR_OSCCTRL_HFOSCEN                                                 (_ADI_MSK(0x00000002,uint16_t))  /* high frequency internal oscillator enable */
#define BITM_PWR_OSCCTRL_LFOSCEN                                                 (_ADI_MSK(0x00000001,uint16_t))  /* Low frequency internal oscillator enable */

/* ------------------------------------------------------------------------------------------------------------------------
        PWR_EI0CFG                                                               Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_PWR_EI0CFG_IRQ3EN                                                   15                               /* External Interrupt 3 enable bit */
#define BITP_PWR_EI0CFG_IRQ3MDE                                                  12                               /* External Interrupt 3 Mode registers */
#define BITP_PWR_EI0CFG_IRQ2EN                                                   11                               /* External Interrupt 2 Enable bit */
#define BITP_PWR_EI0CFG_IRQ2MDE                                                   8                               /* External Interrupt 2 Mode registers */
#define BITP_PWR_EI0CFG_IRQ1EN                                                    7                               /* External Interrupt 1 Enable bit */
#define BITP_PWR_EI0CFG_IRQ1MDE                                                   4                               /* External Interrupt 1 Mode registers */
#define BITP_PWR_EI0CFG_IRQOEN                                                    3                               /* External Interrupt 0 Enable bit */
#define BITP_PWR_EI0CFG_IRQ0MDE                                                   0                               /* External Interrupt 0 Mode registers */
#define BITM_PWR_EI0CFG_IRQ3EN                                                   (_ADI_MSK(0x00008000,uint16_t))  /* External Interrupt 3 enable bit */
#define BITM_PWR_EI0CFG_IRQ3MDE                                                  (_ADI_MSK(0x00007000,uint16_t))  /* External Interrupt 3 Mode registers */
#define BITM_PWR_EI0CFG_IRQ2EN                                                   (_ADI_MSK(0x00000800,uint16_t))  /* External Interrupt 2 Enable bit */
#define BITM_PWR_EI0CFG_IRQ2MDE                                                  (_ADI_MSK(0x00000700,uint16_t))  /* External Interrupt 2 Mode registers */
#define BITM_PWR_EI0CFG_IRQ1EN                                                   (_ADI_MSK(0x00000080,uint16_t))  /* External Interrupt 1 Enable bit */
#define BITM_PWR_EI0CFG_IRQ1MDE                                                  (_ADI_MSK(0x00000070,uint16_t))  /* External Interrupt 1 Mode registers */
#define BITM_PWR_EI0CFG_IRQOEN                                                   (_ADI_MSK(0x00000008,uint16_t))  /* External Interrupt 0 Enable bit */
#define BITM_PWR_EI0CFG_IRQ0MDE                                                  (_ADI_MSK(0x00000007,uint16_t))  /* External Interrupt 0 Mode registers */

/* ------------------------------------------------------------------------------------------------------------------------
        PWR_EI1CFG                                                               Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_PWR_EI1CFG_IRQ7EN                                                   15                               /* External Interrupt 7 enable bit */
#define BITP_PWR_EI1CFG_IRQ7MDE                                                  12                               /* External Interrupt 7 Mode registers */
#define BITP_PWR_EI1CFG_IRQ6EN                                                   11                               /* External Interrupt 6 Enable bit */
#define BITP_PWR_EI1CFG_IRQ6MDE                                                   8                               /* External Interrupt 6 Mode registers */
#define BITP_PWR_EI1CFG_IRQ5EN                                                    7                               /* External Interrupt 5 Enable bit */
#define BITP_PWR_EI1CFG_IRQ5MDE                                                   4                               /* External Interrupt 5 Mode registers */
#define BITP_PWR_EI1CFG_IRQ4EN                                                    3                               /* External Interrupt 4 Enable bit */
#define BITP_PWR_EI1CFG_IRQ4MDE                                                   0                               /* External Interrupt 4 Mode registers */
#define BITM_PWR_EI1CFG_IRQ7EN                                                   (_ADI_MSK(0x00008000,uint16_t))  /* External Interrupt 7 enable bit */
#define BITM_PWR_EI1CFG_IRQ7MDE                                                  (_ADI_MSK(0x00007000,uint16_t))  /* External Interrupt 7 Mode registers */
#define BITM_PWR_EI1CFG_IRQ6EN                                                   (_ADI_MSK(0x00000800,uint16_t))  /* External Interrupt 6 Enable bit */
#define BITM_PWR_EI1CFG_IRQ6MDE                                                  (_ADI_MSK(0x00000700,uint16_t))  /* External Interrupt 6 Mode registers */
#define BITM_PWR_EI1CFG_IRQ5EN                                                   (_ADI_MSK(0x00000080,uint16_t))  /* External Interrupt 5 Enable bit */
#define BITM_PWR_EI1CFG_IRQ5MDE                                                  (_ADI_MSK(0x00000070,uint16_t))  /* External Interrupt 5 Mode registers */
#define BITM_PWR_EI1CFG_IRQ4EN                                                   (_ADI_MSK(0x00000008,uint16_t))  /* External Interrupt 4 Enable bit */
#define BITM_PWR_EI1CFG_IRQ4MDE                                                  (_ADI_MSK(0x00000007,uint16_t))  /* External Interrupt 4 Mode registers */

/* ------------------------------------------------------------------------------------------------------------------------
        PWR_EI2CFG                                                               Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_PWR_EI2CFG_USBVBUSEN                                                15                               /* USB VBUS Detection Enable bit */
#define BITP_PWR_EI2CFG_USBVBUSMDE                                               12                               /* USB VBUS Detection Mode registers */
#define BITP_PWR_EI2CFG_USBDMEN                                                  11                               /* USB DM Detection Enable bit */
#define BITP_PWR_EI2CFG_USBDMMDE                                                  8                               /* USB DM Detection Mode registers */
#define BITP_PWR_EI2CFG_USBDPEN                                                   7                               /* USB DP Detection Enable bit */
#define BITP_PWR_EI2CFG_USBDPMDE                                                  4                               /* USB DP Detection Mode registers */
#define BITP_PWR_EI2CFG_IRQ8EN                                                    3                               /* External Interrupt 8 Enable bit */
#define BITP_PWR_EI2CFG_IRQ8MDE                                                   0                               /* External Interrupt 8 Mode registers */
#define BITM_PWR_EI2CFG_USBVBUSEN                                                (_ADI_MSK(0x00008000,uint16_t))  /* USB VBUS Detection Enable bit */
#define BITM_PWR_EI2CFG_USBVBUSMDE                                               (_ADI_MSK(0x00007000,uint16_t))  /* USB VBUS Detection Mode registers */
#define BITM_PWR_EI2CFG_USBDMEN                                                  (_ADI_MSK(0x00000800,uint16_t))  /* USB DM Detection Enable bit */
#define BITM_PWR_EI2CFG_USBDMMDE                                                 (_ADI_MSK(0x00000700,uint16_t))  /* USB DM Detection Mode registers */
#define BITM_PWR_EI2CFG_USBDPEN                                                  (_ADI_MSK(0x00000080,uint16_t))  /* USB DP Detection Enable bit */
#define BITM_PWR_EI2CFG_USBDPMDE                                                 (_ADI_MSK(0x00000070,uint16_t))  /* USB DP Detection Mode registers */
#define BITM_PWR_EI2CFG_IRQ8EN                                                   (_ADI_MSK(0x00000008,uint16_t))  /* External Interrupt 8 Enable bit */
#define BITM_PWR_EI2CFG_IRQ8MDE                                                  (_ADI_MSK(0x00000007,uint16_t))  /* External Interrupt 8 Mode registers */

/* ------------------------------------------------------------------------------------------------------------------------
        PWR_EICLR                                                                Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_PWR_EICLR_USBVBUS                                                   11                               /* USB VBUS detection */
#define BITP_PWR_EICLR_USBDM                                                     10                               /* USB DM detection */
#define BITP_PWR_EICLR_USBDP                                                      9                               /* USB DP detection */
#define BITP_PWR_EICLR_IRQ8                                                       8                               /* External interrupt 8 */
#define BITP_PWR_EICLR_IRQ7                                                       7                               /* External interrupt 7 */
#define BITP_PWR_EICLR_IRQ6                                                       6                               /* External interrupt 6 */
#define BITP_PWR_EICLR_IRQ5                                                       5                               /* External interrupt 5 */
#define BITP_PWR_EICLR_IRQ4                                                       4                               /* External interrupt 4 */
#define BITP_PWR_EICLR_IRQ3                                                       3                               /* External interrupt 3 */
#define BITP_PWR_EICLR_IRQ2                                                       2                               /* External interrupt 2 */
#define BITP_PWR_EICLR_IRQ1                                                       1                               /* External interrupt 1 */
#define BITP_PWR_EICLR_IRQ0                                                       0                               /* External interrupt 0 */
#define BITM_PWR_EICLR_USBVBUS                                                   (_ADI_MSK(0x00000800,uint16_t))  /* USB VBUS detection */
#define BITM_PWR_EICLR_USBDM                                                     (_ADI_MSK(0x00000400,uint16_t))  /* USB DM detection */
#define BITM_PWR_EICLR_USBDP                                                     (_ADI_MSK(0x00000200,uint16_t))  /* USB DP detection */
#define BITM_PWR_EICLR_IRQ8                                                      (_ADI_MSK(0x00000100,uint16_t))  /* External interrupt 8 */
#define BITM_PWR_EICLR_IRQ7                                                      (_ADI_MSK(0x00000080,uint16_t))  /* External interrupt 7 */
#define BITM_PWR_EICLR_IRQ6                                                      (_ADI_MSK(0x00000040,uint16_t))  /* External interrupt 6 */
#define BITM_PWR_EICLR_IRQ5                                                      (_ADI_MSK(0x00000020,uint16_t))  /* External interrupt 5 */
#define BITM_PWR_EICLR_IRQ4                                                      (_ADI_MSK(0x00000010,uint16_t))  /* External interrupt 4 */
#define BITM_PWR_EICLR_IRQ3                                                      (_ADI_MSK(0x00000008,uint16_t))  /* External interrupt 3 */
#define BITM_PWR_EICLR_IRQ2                                                      (_ADI_MSK(0x00000004,uint16_t))  /* External interrupt 2 */
#define BITM_PWR_EICLR_IRQ1                                                      (_ADI_MSK(0x00000002,uint16_t))  /* External interrupt 1 */
#define BITM_PWR_EICLR_IRQ0                                                      (_ADI_MSK(0x00000001,uint16_t))  /* External interrupt 0 */

/* ------------------------------------------------------------------------------------------------------------------------
        PWR_NMICLR                                                               Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_PWR_NMICLR_CLEAR                                                     0                               /* NMI clear */
#define BITM_PWR_NMICLR_CLEAR                                                    (_ADI_MSK(0x00000001,uint16_t))  /* NMI clear */

/* ------------------------------------------------------------------------------------------------------------------------
        PWR_USBWKSTAT                                                            Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_PWR_USBWKSTAT_USBVBUS                                                2                               /* USB VBUS event status */
#define BITP_PWR_USBWKSTAT_USBDM                                                  1                               /* USB DM event status */
#define BITP_PWR_USBWKSTAT_USBDP                                                  0                               /* USB DP event status */
#define BITM_PWR_USBWKSTAT_USBVBUS                                               (_ADI_MSK(0x00000004,uint16_t))  /* USB VBUS event status */
#define BITM_PWR_USBWKSTAT_USBDM                                                 (_ADI_MSK(0x00000002,uint16_t))  /* USB DM event status */
#define BITM_PWR_USBWKSTAT_USBDP                                                 (_ADI_MSK(0x00000001,uint16_t))  /* USB DP event status */

/* ------------------------------------------------------------------------------------------------------------------------
        PWR_RSTSTA                                                               Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_PWR_RSTSTA_SWRST                                                     3                               /* Software reset */
#define BITP_PWR_RSTSTA_WDRST                                                     2                               /* Watchdog timeout */
#define BITP_PWR_RSTSTA_EXTRST                                                    1                               /* External reset */
#define BITP_PWR_RSTSTA_POR                                                       0                               /* Power-on reset */
#define BITM_PWR_RSTSTA_SWRST                                                    (_ADI_MSK(0x00000008,uint16_t))  /* Software reset */
#define BITM_PWR_RSTSTA_WDRST                                                    (_ADI_MSK(0x00000004,uint16_t))  /* Watchdog timeout */
#define BITM_PWR_RSTSTA_EXTRST                                                   (_ADI_MSK(0x00000002,uint16_t))  /* External reset */
#define BITM_PWR_RSTSTA_POR                                                      (_ADI_MSK(0x00000001,uint16_t))  /* Power-on reset */

/* ------------------------------------------------------------------------------------------------------------------------
        PWR_VCCMCON                                                              Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_PWR_VCCMCON_VCCMTHR_HYST                                             8                               /* VCCM Threshold hysteresis */
#define BITP_PWR_VCCMCON_VCCMTHR                                                  4                               /* VCCM Threshold adjust */
#define BITP_PWR_VCCMCON_VCCMPSMEN                                                1                               /* VCCM power supply monitoring enable */
#define BITP_PWR_VCCMCON_LOADENABLE                                               0                               /* Enabling 820 ohm load */
#define BITM_PWR_VCCMCON_VCCMTHR_HYST                                            (_ADI_MSK(0x00000300,uint16_t))  /* VCCM Threshold hysteresis */
#define BITM_PWR_VCCMCON_VCCMTHR                                                 (_ADI_MSK(0x000000F0,uint16_t))  /* VCCM Threshold adjust */
#define BITM_PWR_VCCMCON_VCCMPSMEN                                               (_ADI_MSK(0x00000002,uint16_t))  /* VCCM power supply monitoring enable */
#define BITM_PWR_VCCMCON_LOADENABLE                                              (_ADI_MSK(0x00000001,uint16_t))  /* Enabling 820 ohm load */

/* ------------------------------------------------------------------------------------------------------------------------
        PWR_VBACKCON                                                             Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_PWR_VBACKCON_VBACKTRIP                                               8                               /* Vback trip point adjust */
#define BITP_PWR_VBACKCON_VLTRIP_HYST                                             6                               /* Vlo trip hysteresis */
#define BITP_PWR_VBACKCON_VLOTRIP                                                 4                               /* Vlo trip level */
#define BITP_PWR_VBACKCON_VBACKRESTORE                                            0                               /* VBACK Restore level */
#define BITM_PWR_VBACKCON_VBACKTRIP                                              (_ADI_MSK(0x00000F00,uint16_t))  /* Vback trip point adjust */
#define BITM_PWR_VBACKCON_VLTRIP_HYST                                            (_ADI_MSK(0x000000C0,uint16_t))  /* Vlo trip hysteresis */
#define BITM_PWR_VBACKCON_VLOTRIP                                                (_ADI_MSK(0x00000030,uint16_t))  /* Vlo trip level */
#define BITM_PWR_VBACKCON_VBACKRESTORE                                           (_ADI_MSK(0x0000000F,uint16_t))  /* VBACK Restore level */

/* ==================================================
        WUT Registers
   ================================================== */

/* =========================
        WUT
   ========================= */
#define REG_WUT_T2VAL0                       0x40002500         /* WUT Current count value - LS halfword. */
#define REG_WUT_T2VAL1                       0x40002504         /* WUT Current count value - MS halfword */
#define REG_WUT_T2CON                        0x40002508         /* WUT Control */
#define REG_WUT_T2INC                        0x4000250C         /* WUT 12-bit interval for wakeup field A */
#define REG_WUT_T2WUFB0                      0x40002510         /* WUT Wakeup field B - LS halfword */
#define REG_WUT_T2WUFB1                      0x40002514         /* WUT Wakeup field B - MS halfword */
#define REG_WUT_T2WUFC0                      0x40002518         /* WUT Wakeup field C - LS halfword */
#define REG_WUT_T2WUFC1                      0x4000251C         /* WUT Wakeup field C - MS halfword */
#define REG_WUT_T2WUFD0                      0x40002520         /* WUT Wakeup field D - LS halfword */
#define REG_WUT_T2WUFD1                      0x40002524         /* WUT Wakeup field D - MS halfword */
#define REG_WUT_T2IEN                        0x40002528         /* WUT Interrupt enable */
#define REG_WUT_T2STA                        0x4000252C         /* WUT Status */
#define REG_WUT_T2CLRI                       0x40002530         /* WUT Clear interrupts */
#define REG_WUT_WUTVAL_LOW                   0x40002534         /* WUT Unsynchronized lower 16 bits of WU Timer counter value. */
#define REG_WUT_WUTVAL_HIGH                  0x40002538         /* WUT Unsynchronized upper 16 bits of WU Timer counter value. */
#define REG_WUT_T2WUFA0                      0x4000253C         /* WUT Wakeup field A - LS halfword */
#define REG_WUT_T2WUFA1                      0x40002540         /* WUT Wakeup field A - MS halfword */

/* =========================
        WUT
   ========================= */
/* ------------------------------------------------------------------------------------------------------------------------
        WUT_T2CON                                                                Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_WUT_T2CON_STOP_WUFA                                                 11                               /* Disables updating field A register T2WUFA */
#define BITP_WUT_T2CON_CLK                                                        9                               /* Clock select */
#define BITP_WUT_T2CON_WUEN                                                       8                               /* Wakeup enable */
#define BITP_WUT_T2CON_ENABLE                                                     7                               /* Timer enable */
#define BITP_WUT_T2CON_MOD                                                        6                               /* Timer mode */
#define BITP_WUT_T2CON_FREEZE                                                     3                               /* Freeze enable */
#define BITP_WUT_T2CON_PRE                                                        0                               /* Prescaler */
#define BITM_WUT_T2CON_STOP_WUFA                                                 (_ADI_MSK(0x00000800,uint16_t))  /* Disables updating field A register T2WUFA */
#define BITM_WUT_T2CON_CLK                                                       (_ADI_MSK(0x00000600,uint16_t))  /* Clock select */
#define BITM_WUT_T2CON_WUEN                                                      (_ADI_MSK(0x00000100,uint16_t))  /* Wakeup enable */
#define BITM_WUT_T2CON_ENABLE                                                    (_ADI_MSK(0x00000080,uint16_t))  /* Timer enable */
#define BITM_WUT_T2CON_MOD                                                       (_ADI_MSK(0x00000040,uint16_t))  /* Timer mode */
#define BITM_WUT_T2CON_FREEZE                                                    (_ADI_MSK(0x00000008,uint16_t))  /* Freeze enable */
#define BITM_WUT_T2CON_PRE                                                       (_ADI_MSK(0x00000003,uint16_t))  /* Prescaler */

/* ------------------------------------------------------------------------------------------------------------------------
        WUT_T2INC                                                                Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_WUT_T2INC_INTERVAL                                                   0                               /* Interval for wakeup field A */
#define BITM_WUT_T2INC_INTERVAL                                                  (_ADI_MSK(0x00000FFF,uint16_t))  /* Interval for wakeup field A */

/* ------------------------------------------------------------------------------------------------------------------------
        WUT_T2IEN                                                                Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_WUT_T2IEN_ROLL                                                       4                               /* Rollover interrupt enable */
#define BITP_WUT_T2IEN_WUFD                                                       3                               /* T2WUFD interrupt enable */
#define BITP_WUT_T2IEN_WUFC                                                       2                               /* T2WUFC interrupt enable */
#define BITP_WUT_T2IEN_WUFB                                                       1                               /* T2WUFB interrupt enable */
#define BITP_WUT_T2IEN_WUFA                                                       0                               /* T2WUFA interrupt enable */
#define BITM_WUT_T2IEN_ROLL                                                      (_ADI_MSK(0x00000010,uint16_t))  /* Rollover interrupt enable */
#define BITM_WUT_T2IEN_WUFD                                                      (_ADI_MSK(0x00000008,uint16_t))  /* T2WUFD interrupt enable */
#define BITM_WUT_T2IEN_WUFC                                                      (_ADI_MSK(0x00000004,uint16_t))  /* T2WUFC interrupt enable */
#define BITM_WUT_T2IEN_WUFB                                                      (_ADI_MSK(0x00000002,uint16_t))  /* T2WUFB interrupt enable */
#define BITM_WUT_T2IEN_WUFA                                                      (_ADI_MSK(0x00000001,uint16_t))  /* T2WUFA interrupt enable */

/* ------------------------------------------------------------------------------------------------------------------------
        WUT_T2STA                                                                Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_WUT_T2STA_PDOK                                                       8                               /* Enable bit synchronized */
#define BITP_WUT_T2STA_FREEZE                                                     7                               /* Timer value freeze */
#define BITP_WUT_T2STA_IRQCRY                                                     6                               /* Wakeup status to powerdown */
#define BITP_WUT_T2STA_ROLL                                                       4                               /* Rollover interrupt flag */
#define BITP_WUT_T2STA_WUFD                                                       3                               /* T2WUFD interrupt flag */
#define BITP_WUT_T2STA_WUFC                                                       2                               /* T2WUFC interrupt flag */
#define BITP_WUT_T2STA_WUFB                                                       1                               /* T2WUFB interrupt flag */
#define BITP_WUT_T2STA_WUFA                                                       0                               /* T2WUFA interrupt flag */
#define BITM_WUT_T2STA_PDOK                                                      (_ADI_MSK(0x00000100,uint16_t))  /* Enable bit synchronized */
#define BITM_WUT_T2STA_FREEZE                                                    (_ADI_MSK(0x00000080,uint16_t))  /* Timer value freeze */
#define BITM_WUT_T2STA_IRQCRY                                                    (_ADI_MSK(0x00000040,uint16_t))  /* Wakeup status to powerdown */
#define BITM_WUT_T2STA_ROLL                                                      (_ADI_MSK(0x00000010,uint16_t))  /* Rollover interrupt flag */
#define BITM_WUT_T2STA_WUFD                                                      (_ADI_MSK(0x00000008,uint16_t))  /* T2WUFD interrupt flag */
#define BITM_WUT_T2STA_WUFC                                                      (_ADI_MSK(0x00000004,uint16_t))  /* T2WUFC interrupt flag */
#define BITM_WUT_T2STA_WUFB                                                      (_ADI_MSK(0x00000002,uint16_t))  /* T2WUFB interrupt flag */
#define BITM_WUT_T2STA_WUFA                                                      (_ADI_MSK(0x00000001,uint16_t))  /* T2WUFA interrupt flag */

/* ------------------------------------------------------------------------------------------------------------------------
        WUT_T2CLRI                                                               Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_WUT_T2CLRI_ROLL                                                      4                               /* Rollover interrupt clear */
#define BITP_WUT_T2CLRI_WUFD                                                      3                               /* T2WUFD interrupt clear */
#define BITP_WUT_T2CLRI_WUFC                                                      2                               /* T2WUFC interrupt clear */
#define BITP_WUT_T2CLRI_WUFB                                                      1                               /* T2WUFB interrupt clear */
#define BITP_WUT_T2CLRI_WUFA                                                      0                               /* T2WUFA interrupt clear */
#define BITM_WUT_T2CLRI_ROLL                                                     (_ADI_MSK(0x00000010,uint16_t))  /* Rollover interrupt clear */
#define BITM_WUT_T2CLRI_WUFD                                                     (_ADI_MSK(0x00000008,uint16_t))  /* T2WUFD interrupt clear */
#define BITM_WUT_T2CLRI_WUFC                                                     (_ADI_MSK(0x00000004,uint16_t))  /* T2WUFC interrupt clear */
#define BITM_WUT_T2CLRI_WUFB                                                     (_ADI_MSK(0x00000002,uint16_t))  /* T2WUFB interrupt clear */
#define BITM_WUT_T2CLRI_WUFA                                                     (_ADI_MSK(0x00000001,uint16_t))  /* T2WUFA interrupt clear */

/* ==================================================
        WDT Registers
   ================================================== */

/* =========================
        WDT
   ========================= */
#define REG_WDT_T3LD                         0x40002580         /* WDT Load value */
#define REG_WDT_T3VAL                        0x40002584         /* WDT Current count value */
#define REG_WDT_T3CON                        0x40002588         /* WDT Control */
#define REG_WDT_T3CLRI                       0x4000258C         /* WDT Clear interrupt */
#define REG_WDT_T3STA                        0x40002598         /* WDT Status */

/* =========================
        WDT
   ========================= */
/* ------------------------------------------------------------------------------------------------------------------------
        WDT_T3CON                                                                Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_WDT_T3CON_MOD                                                        6                               /* Timer mode */
#define BITP_WDT_T3CON_ENABLE                                                     5                               /* Timer enable */
#define BITP_WDT_T3CON_PRE                                                        2                               /* Prescaler */
#define BITP_WDT_T3CON_IRQ                                                        1                               /* Timer interrupt */
#define BITP_WDT_T3CON_PMD                                                        0                               /* Power Mode Disable */
#define BITM_WDT_T3CON_MOD                                                       (_ADI_MSK(0x00000040,uint16_t))  /* Timer mode */
#define BITM_WDT_T3CON_ENABLE                                                    (_ADI_MSK(0x00000020,uint16_t))  /* Timer enable */
#define BITM_WDT_T3CON_PRE                                                       (_ADI_MSK(0x0000000C,uint16_t))  /* Prescaler */
#define BITM_WDT_T3CON_IRQ                                                       (_ADI_MSK(0x00000002,uint16_t))  /* Timer interrupt */
#define BITM_WDT_T3CON_PMD                                                       (_ADI_MSK(0x00000001,uint16_t))  /* Power Mode Disable */

/* ------------------------------------------------------------------------------------------------------------------------
        WDT_T3STA                                                                Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_WDT_T3STA_LOCK                                                       4                               /* Lock status bit */
#define BITP_WDT_T3STA_CON                                                        3                               /* T3CON write sync in progress */
#define BITP_WDT_T3STA_LD                                                         2                               /* T3LD write sync in progress */
#define BITP_WDT_T3STA_CLRI                                                       1                               /* T3CLRI write sync in progress */
#define BITP_WDT_T3STA_IRQ                                                        0                               /* WDT Interrupt */
#define BITM_WDT_T3STA_LOCK                                                      (_ADI_MSK(0x00000010,uint16_t))  /* Lock status bit */
#define BITM_WDT_T3STA_CON                                                       (_ADI_MSK(0x00000008,uint16_t))  /* T3CON write sync in progress */
#define BITM_WDT_T3STA_LD                                                        (_ADI_MSK(0x00000004,uint16_t))  /* T3LD write sync in progress */
#define BITM_WDT_T3STA_CLRI                                                      (_ADI_MSK(0x00000002,uint16_t))  /* T3CLRI write sync in progress */
#define BITM_WDT_T3STA_IRQ                                                       (_ADI_MSK(0x00000001,uint16_t))  /* WDT Interrupt */

/* ==================================================
        Real-Time Clock Registers
   ================================================== */

/* =========================
        RTC
   ========================= */
#define REG_RTC_RTCCR                        0x40002600         /* RTC RTC Control */
#define REG_RTC_RTCSR0                       0x40002604         /* RTC RTC Status 0 */
#define REG_RTC_RTCSR1                       0x40002608         /* RTC RTC Status 1 */
#define REG_RTC_RTCCNT0                      0x4000260C         /* RTC RTC Count 0 */
#define REG_RTC_RTCCNT1                      0x40002610         /* RTC RTC Count 1 */
#define REG_RTC_RTCALM0                      0x40002614         /* RTC RTC Alarm 0 */
#define REG_RTC_RTCALM1                      0x40002618         /* RTC RTC Alarm 1 */
#define REG_RTC_RTCTRM                       0x4000261C         /* RTC RTC Trim */
#define REG_RTC_RTCGWY                       0x40002620         /* RTC RTC Gateway */

/* =========================
        RTC
   ========================= */
/* ------------------------------------------------------------------------------------------------------------------------
        RTC_RTCCR                                                                Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_RTC_RTCCR_WPENDINTEN                                                15                               /* Enable WPENDINT-sourced interrupts to the CPU */
#define BITP_RTC_RTCCR_WSYNCINTEN                                                14                               /* Enable WSYNCINT-sourced interrupts to the CPU */
#define BITP_RTC_RTCCR_WPENDERRINTEN                                             13                               /* Enable WPENDERRINT-sourced interrupts to the CPU when an RTC register-write pending error occurs */
#define BITP_RTC_RTCCR_ISOINTEN                                                  12                               /* Enable ISOINT-sourced interrupts to the CPU when isolation of the RTC power domain is activated and subsequently de-activated */
#define BITP_RTC_RTCCR_LCDINTEN                                                  11                               /* Enable LCDINT-sourced LCD update interrupts to the CPU */
#define BITP_RTC_RTCCR_LCDUPDTIM                                                  5                               /* LCD update time in seconds beyond a one-minute boundary */
#define BITP_RTC_RTCCR_LCDEN                                                      4                               /* Enable RTC determination of when an LCD minute display update should occur */
#define BITP_RTC_RTCCR_TRMEN                                                      3                               /* Enable RTC digital trimming */
#define BITP_RTC_RTCCR_ALMINTEN                                                   2                               /* Enable ALMINT-sourced alarm interrupts to the CPU */
#define BITP_RTC_RTCCR_ALMEN                                                      1                               /* Enable the RTC alarm operation */
#define BITP_RTC_RTCCR_CNTEN                                                      0                               /* Global enable for the RTC */
#define BITM_RTC_RTCCR_WPENDINTEN                                                (_ADI_MSK(0x00008000,uint32_t))  /* Enable WPENDINT-sourced interrupts to the CPU */
#define BITM_RTC_RTCCR_WSYNCINTEN                                                (_ADI_MSK(0x00004000,uint32_t))  /* Enable WSYNCINT-sourced interrupts to the CPU */
#define BITM_RTC_RTCCR_WPENDERRINTEN                                             (_ADI_MSK(0x00002000,uint32_t))  /* Enable WPENDERRINT-sourced interrupts to the CPU when an RTC register-write pending error occurs */
#define BITM_RTC_RTCCR_ISOINTEN                                                  (_ADI_MSK(0x00001000,uint32_t))  /* Enable ISOINT-sourced interrupts to the CPU when isolation of the RTC power domain is activated and subsequently de-activated */
#define BITM_RTC_RTCCR_LCDINTEN                                                  (_ADI_MSK(0x00000800,uint32_t))  /* Enable LCDINT-sourced LCD update interrupts to the CPU */

#define BITM_RTC_RTCCR_LCDUPDTIM                                                 (_ADI_MSK(0x000007E0,uint32_t))  /* LCD update time in seconds beyond a one-minute boundary */
#define ENUM_RTC_RTCCR_Example_1_Thirty_seconds_decimal                          (_ADI_MSK(0x000003C0,uint32_t))  /* LCDUPDTIM: Example of setting an LCD update interrupt from the RTC to be issued to the CPU at 30 seconds past a 1-minute boundary. */
#define ENUM_RTC_RTCCR_Example_2_FiftyFive_seconds_decimal                       (_ADI_MSK(0x000006E0,uint32_t))  /* LCDUPDTIM: Example of setting an LCD update interrupt from the RTC to be issued to the CPU at 55 seconds past a 1-minute boundary. */
#define BITM_RTC_RTCCR_LCDEN                                                     (_ADI_MSK(0x00000010,uint32_t))  /* Enable RTC determination of when an LCD minute display update should occur */
#define BITM_RTC_RTCCR_TRMEN                                                     (_ADI_MSK(0x00000008,uint32_t))  /* Enable RTC digital trimming */
#define BITM_RTC_RTCCR_ALMINTEN                                                  (_ADI_MSK(0x00000004,uint32_t))  /* Enable ALMINT-sourced alarm interrupts to the CPU */
#define BITM_RTC_RTCCR_ALMEN                                                     (_ADI_MSK(0x00000002,uint32_t))  /* Enable the RTC alarm operation */
#define BITM_RTC_RTCCR_CNTEN                                                     (_ADI_MSK(0x00000001,uint32_t))  /* Global enable for the RTC */

/* ------------------------------------------------------------------------------------------------------------------------
        RTC_RTCSR0                                                               Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_RTC_RTCSR0_WSYNCTRM                                                 13                               /* Synchronisation status of posted writes to RTCTRM */
#define BITP_RTC_RTCSR0_WSYNCALM1                                                12                               /* Synchronisation status of posted writes to RTCALM1 */
#define BITP_RTC_RTCSR0_WSYNCALM0                                                11                               /* Synchronisation status of posted writes to RTCALM0 */
#define BITP_RTC_RTCSR0_WSYNCCNT1                                                10                               /* Synchronisation status of posted writes to RTCCNT1 */
#define BITP_RTC_RTCSR0_WSYNCCNT0                                                 9                               /* Synchronisation status of posted writes to RTCCNT0 */
#define BITP_RTC_RTCSR0_WSYNCSR0                                                  8                               /* Synchronisation status of posted clearances to interrupt sources in RTCSR0 */
#define BITP_RTC_RTCSR0_WSYNCCR                                                   7                               /* Synchronisation status of posted writes to RTCCR */
#define BITP_RTC_RTCSR0_WPENDINT                                                  6                               /* Write pending interrupt */
#define BITP_RTC_RTCSR0_WSYNCINT                                                  5                               /* Write synchronisation interrupt */
#define BITP_RTC_RTCSR0_WPENDERRINT                                               4                               /* Write pending error interrupt source */
#define BITP_RTC_RTCSR0_ISOINT                                                    3                               /* RTC power-domain isolation interrupt source */
#define BITP_RTC_RTCSR0_LCDINT                                                    2                               /* LCD update interrupt source */
#define BITP_RTC_RTCSR0_ALMINT                                                    1                               /* Alarm interrupt source */
#define BITP_RTC_RTCSR0_RTCFAIL                                                   0                               /* RTC failure interrupt source */
#define BITM_RTC_RTCSR0_WSYNCTRM                                                 (_ADI_MSK(0x00002000,uint16_t))  /* Synchronisation status of posted writes to RTCTRM */
#define BITM_RTC_RTCSR0_WSYNCALM1                                                (_ADI_MSK(0x00001000,uint16_t))  /* Synchronisation status of posted writes to RTCALM1 */
#define BITM_RTC_RTCSR0_WSYNCALM0                                                (_ADI_MSK(0x00000800,uint16_t))  /* Synchronisation status of posted writes to RTCALM0 */
#define BITM_RTC_RTCSR0_WSYNCCNT1                                                (_ADI_MSK(0x00000400,uint16_t))  /* Synchronisation status of posted writes to RTCCNT1 */
#define BITM_RTC_RTCSR0_WSYNCCNT0                                                (_ADI_MSK(0x00000200,uint16_t))  /* Synchronisation status of posted writes to RTCCNT0 */
#define BITM_RTC_RTCSR0_WSYNCSR0                                                 (_ADI_MSK(0x00000100,uint16_t))  /* Synchronisation status of posted clearances to interrupt sources in RTCSR0 */
#define BITM_RTC_RTCSR0_WSYNCCR                                                  (_ADI_MSK(0x00000080,uint16_t))  /* Synchronisation status of posted writes to RTCCR */
#define BITM_RTC_RTCSR0_WPENDINT                                                 (_ADI_MSK(0x00000040,uint16_t))  /* Write pending interrupt */
#define BITM_RTC_RTCSR0_WSYNCINT                                                 (_ADI_MSK(0x00000020,uint16_t))  /* Write synchronisation interrupt */
#define BITM_RTC_RTCSR0_WPENDERRINT                                              (_ADI_MSK(0x00000010,uint16_t))  /* Write pending error interrupt source */
#define BITM_RTC_RTCSR0_ISOINT                                                   (_ADI_MSK(0x00000008,uint16_t))  /* RTC power-domain isolation interrupt source */
#define BITM_RTC_RTCSR0_LCDINT                                                   (_ADI_MSK(0x00000004,uint16_t))  /* LCD update interrupt source */
#define BITM_RTC_RTCSR0_ALMINT                                                   (_ADI_MSK(0x00000002,uint16_t))  /* Alarm interrupt source */
#define BITM_RTC_RTCSR0_RTCFAIL                                                  (_ADI_MSK(0x00000001,uint16_t))  /* RTC failure interrupt source */

/* ------------------------------------------------------------------------------------------------------------------------
        RTC_RTCSR1                                                               Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_RTC_RTCSR1_WPENDTRM                                                 13                               /* Pending status of posted writes to RTCTRM */
#define BITP_RTC_RTCSR1_WPENDALM1                                                12                               /* Pending status of posted writes to RTCALM1 */
#define BITP_RTC_RTCSR1_WPENDALM0                                                11                               /* Pending status of posted writes to RTCALM0 */
#define BITP_RTC_RTCSR1_WPENDCNT1                                                10                               /* Pending status of posted writes to RTCCNT1 */
#define BITP_RTC_RTCSR1_WPENDCNT0                                                 9                               /* Pending status of posted writes to RTCCNT0 */
#define BITP_RTC_RTCSR1_WPENDSR0                                                  8                               /* Pending status of posted clearances of interrupt sources in RTCSR0 */
#define BITP_RTC_RTCSR1_WPENDCR                                                   7                               /* Pending status of posted writes to RTCCR */
#define BITP_RTC_RTCSR1_WERRCODE                                                  3                               /* Identifier for the source of a write synchronisation error */
#define BITM_RTC_RTCSR1_WPENDTRM                                                 (_ADI_MSK(0x00002000,uint16_t))  /* Pending status of posted writes to RTCTRM */
#define BITM_RTC_RTCSR1_WPENDALM1                                                (_ADI_MSK(0x00001000,uint16_t))  /* Pending status of posted writes to RTCALM1 */
#define BITM_RTC_RTCSR1_WPENDALM0                                                (_ADI_MSK(0x00000800,uint16_t))  /* Pending status of posted writes to RTCALM0 */
#define BITM_RTC_RTCSR1_WPENDCNT1                                                (_ADI_MSK(0x00000400,uint16_t))  /* Pending status of posted writes to RTCCNT1 */
#define BITM_RTC_RTCSR1_WPENDCNT0                                                (_ADI_MSK(0x00000200,uint16_t))  /* Pending status of posted writes to RTCCNT0 */
#define BITM_RTC_RTCSR1_WPENDSR0                                                 (_ADI_MSK(0x00000100,uint16_t))  /* Pending status of posted clearances of interrupt sources in RTCSR0 */
#define BITM_RTC_RTCSR1_WPENDCR                                                  (_ADI_MSK(0x00000080,uint16_t))  /* Pending status of posted writes to RTCCR */
#define BITM_RTC_RTCSR1_WERRCODE                                                 (_ADI_MSK(0x00000078,uint16_t))  /* Identifier for the source of a write synchronisation error */

/* ------------------------------------------------------------------------------------------------------------------------
        RTC_RTCTRM                                                               Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_RTC_RTCTRM_TRMIVL                                                    4                               /* Trim interval in units of seconds */
#define BITP_RTC_RTCTRM_TRMADD                                                    3                               /* Trim polarity */
#define BITP_RTC_RTCTRM_TRMVAL                                                    0                               /* Trim value in whole seconds to be added or subtracted from the RTC count at the end of a periodic interval */
#define BITM_RTC_RTCTRM_TRMIVL                                                   (_ADI_MSK(0x00000030,uint16_t))  /* Trim interval in units of seconds */
#define BITM_RTC_RTCTRM_TRMADD                                                   (_ADI_MSK(0x00000008,uint16_t))  /* Trim polarity */
#define BITM_RTC_RTCTRM_TRMVAL                                                   (_ADI_MSK(0x00000007,uint16_t))  /* Trim value in whole seconds to be added or subtracted from the RTC count at the end of a periodic interval */

/* ==================================================
        I2C Master/Slave Registers
   ================================================== */

/* =========================
        I2C
   ========================= */
#define REG_I2C_I2CMCON                      0x40003000         /* I2C Master control */
#define REG_I2C_I2CMSTA                      0x40003004         /* I2C Master status */
#define REG_I2C_I2CMRX                       0x40003008         /* I2C Master receive data */
#define REG_I2C_I2CMTX                       0x4000300C         /* I2C Master transmit data */
#define REG_I2C_I2CMRXCNT                    0x40003010         /* I2C Master receive data count */
#define REG_I2C_I2CMCRXCNT                   0x40003014         /* I2C Master current receive data count */
#define REG_I2C_I2CADR1                      0x40003018         /* I2C 1st master address byte */
#define REG_I2C_I2CADR2                      0x4000301C         /* I2C 2nd master address byte */
#define REG_I2C_I2CBYT                       0x40003020         /* I2C Start byte */
#define REG_I2C_I2CDIV                       0x40003024         /* I2C Serial clock period divisor */
#define REG_I2C_I2CSCON                      0x40003028         /* I2C Slave control */
#define REG_I2C_I2CSSTA                      0x4000302C         /* I2C Slave I2C Status/Error/IRQ */
#define REG_I2C_I2CSRX                       0x40003030         /* I2C Slave receive */
#define REG_I2C_I2CSTX                       0x40003034         /* I2C Slave transmit */
#define REG_I2C_I2CALT                       0x40003038         /* I2C Hardware general call ID */
#define REG_I2C_I2CID0                       0x4000303C         /* I2C 1st slave address device ID */
#define REG_I2C_I2CID1                       0x40003040         /* I2C 2nd slave address device ID */
#define REG_I2C_I2CID2                       0x40003044         /* I2C 3rd slave address device ID */
#define REG_I2C_I2CID3                       0x40003048         /* I2C 4th slave address device ID */
#define REG_I2C_I2CFSTA                      0x4000304C         /* I2C Master and slave FIFO status */
#define REG_I2C_I2CSHCON                     0x40003050         /* I2C Shared control */
#define REG_I2C_I2CTCTL                      0x40003054         /* I2C Timing Control Register */

/* =========================
        I2C
   ========================= */
/* ------------------------------------------------------------------------------------------------------------------------
        I2C_I2CMCON                                                              Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_I2C_I2CMCON_PRESTOP_BUS_CLR                                         13                               /* Prestop Bus-Clear */
#define BITP_I2C_I2CMCON_BUS_CLR_EN                                              12                               /* Bus-Clear Enable */
#define BITP_I2C_I2CMCON_MTXDMA                                                  11                               /* Enable master Tx DMA request */
#define BITP_I2C_I2CMCON_MRXDMA                                                  10                               /* Enable master Rx DMA request */
#define BITP_I2C_I2CMCON_MXMITDEC                                                 9                               /* Decrement master TX FIFO status when a byte has been transmitted */
#define BITP_I2C_I2CMCON_IENCMP                                                   8                               /* Transaction completed (or stop detected) interrupt enable */
#define BITP_I2C_I2CMCON_IENACK                                                   7                               /* ACK not received interrupt enable */
#define BITP_I2C_I2CMCON_IENALOST                                                 6                               /* Arbitration lost interrupt enable */
#define BITP_I2C_I2CMCON_IENMTX                                                   5                               /* Transmit request interrupt enable. */
#define BITP_I2C_I2CMCON_IENMRX                                                   4                               /* Receive request interrupt enable */
#define BITP_I2C_I2CMCON_STRETCH                                                  3                               /* Stretch SCL enable */
#define BITP_I2C_I2CMCON_LOOPBACK                                                 2                               /* Internal loopback enable */
#define BITP_I2C_I2CMCON_COMPLETE                                                 1                               /* Start back-off disable */
#define BITP_I2C_I2CMCON_MASEN                                                    0                               /* Master enable */
#define BITM_I2C_I2CMCON_PRESTOP_BUS_CLR                                         (_ADI_MSK(0x00002000,uint16_t))  /* Prestop Bus-Clear */
#define BITM_I2C_I2CMCON_BUS_CLR_EN                                              (_ADI_MSK(0x00001000,uint16_t))  /* Bus-Clear Enable */
#define BITM_I2C_I2CMCON_MTXDMA                                                  (_ADI_MSK(0x00000800,uint16_t))  /* Enable master Tx DMA request */
#define BITM_I2C_I2CMCON_MRXDMA                                                  (_ADI_MSK(0x00000400,uint16_t))  /* Enable master Rx DMA request */
#define BITM_I2C_I2CMCON_MXMITDEC                                                (_ADI_MSK(0x00000200,uint16_t))  /* Decrement master TX FIFO status when a byte has been transmitted */
#define BITM_I2C_I2CMCON_IENCMP                                                  (_ADI_MSK(0x00000100,uint16_t))  /* Transaction completed (or stop detected) interrupt enable */
#define BITM_I2C_I2CMCON_IENACK                                                  (_ADI_MSK(0x00000080,uint16_t))  /* ACK not received interrupt enable */
#define BITM_I2C_I2CMCON_IENALOST                                                (_ADI_MSK(0x00000040,uint16_t))  /* Arbitration lost interrupt enable */
#define BITM_I2C_I2CMCON_IENMTX                                                  (_ADI_MSK(0x00000020,uint16_t))  /* Transmit request interrupt enable. */
#define BITM_I2C_I2CMCON_IENMRX                                                  (_ADI_MSK(0x00000010,uint16_t))  /* Receive request interrupt enable */
#define BITM_I2C_I2CMCON_STRETCH                                                 (_ADI_MSK(0x00000008,uint16_t))  /* Stretch SCL enable */
#define BITM_I2C_I2CMCON_LOOPBACK                                                (_ADI_MSK(0x00000004,uint16_t))  /* Internal loopback enable */
#define BITM_I2C_I2CMCON_COMPLETE                                                (_ADI_MSK(0x00000002,uint16_t))  /* Start back-off disable */
#define BITM_I2C_I2CMCON_MASEN                                                   (_ADI_MSK(0x00000001,uint16_t))  /* Master enable */

/* ------------------------------------------------------------------------------------------------------------------------
        I2C_I2CMSTA                                                              Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_I2C_I2CMSTA_SCL_FILTERED                                            14                               /* State of SCL Line */
#define BITP_I2C_I2CMSTA_SDA_FILTERED                                            13                               /* State of SDA Line */
#define BITP_I2C_I2CMSTA_MTXUFLOW                                                12                               /* Master Transmit Underflow */
#define BITP_I2C_I2CMSTA_MSTOP                                                   11                               /* STOP driven by this I2C Master */
#define BITP_I2C_I2CMSTA_LINEBUSY                                                10                               /* Line is busy */
#define BITP_I2C_I2CMSTA_MRXOF                                                    9                               /* Master Receive FIFO overflow */
#define BITP_I2C_I2CMSTA_TCOMP                                                    8                               /* Transaction complete or stop detected */
#define BITP_I2C_I2CMSTA_NACKDATA                                                 7                               /* ACK not received in response to data write */
#define BITP_I2C_I2CMSTA_MBUSY                                                    6                               /* Master busy */
#define BITP_I2C_I2CMSTA_ALOST                                                    5                               /* Arbitration lost */
#define BITP_I2C_I2CMSTA_NACKADDR                                                 4                               /* ACK not received in response to an address */
#define BITP_I2C_I2CMSTA_MRXREQ                                                   3                               /* Master Receive request */
#define BITP_I2C_I2CMSTA_MTXREQ                                                   2                               /* Master Transmit request */
#define BITP_I2C_I2CMSTA_MTXFSTA                                                  0                               /* Master Transmit FIFO status */
#define BITM_I2C_I2CMSTA_SCL_FILTERED                                            (_ADI_MSK(0x00004000,uint16_t))  /* State of SCL Line */
#define BITM_I2C_I2CMSTA_SDA_FILTERED                                            (_ADI_MSK(0x00002000,uint16_t))  /* State of SDA Line */
#define BITM_I2C_I2CMSTA_MTXUFLOW                                                (_ADI_MSK(0x00001000,uint16_t))  /* Master Transmit Underflow */
#define BITM_I2C_I2CMSTA_MSTOP                                                   (_ADI_MSK(0x00000800,uint16_t))  /* STOP driven by this I2C Master */
#define BITM_I2C_I2CMSTA_LINEBUSY                                                (_ADI_MSK(0x00000400,uint16_t))  /* Line is busy */
#define BITM_I2C_I2CMSTA_MRXOF                                                   (_ADI_MSK(0x00000200,uint16_t))  /* Master Receive FIFO overflow */
#define BITM_I2C_I2CMSTA_TCOMP                                                   (_ADI_MSK(0x00000100,uint16_t))  /* Transaction complete or stop detected */
#define BITM_I2C_I2CMSTA_NACKDATA                                                (_ADI_MSK(0x00000080,uint16_t))  /* ACK not received in response to data write */
#define BITM_I2C_I2CMSTA_MBUSY                                                   (_ADI_MSK(0x00000040,uint16_t))  /* Master busy */
#define BITM_I2C_I2CMSTA_ALOST                                                   (_ADI_MSK(0x00000020,uint16_t))  /* Arbitration lost */
#define BITM_I2C_I2CMSTA_NACKADDR                                                (_ADI_MSK(0x00000010,uint16_t))  /* ACK not received in response to an address */
#define BITM_I2C_I2CMSTA_MRXREQ                                                  (_ADI_MSK(0x00000008,uint16_t))  /* Master Receive request */
#define BITM_I2C_I2CMSTA_MTXREQ                                                  (_ADI_MSK(0x00000004,uint16_t))  /* Master Transmit request */
#define BITM_I2C_I2CMSTA_MTXFSTA                                                 (_ADI_MSK(0x00000003,uint16_t))  /* Master Transmit FIFO status */

/* ------------------------------------------------------------------------------------------------------------------------
        I2C_I2CMRX                                                               Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_I2C_I2CMRX_I2CMRX                                                    0                               /* Master receive register */
#define BITM_I2C_I2CMRX_I2CMRX                                                   (_ADI_MSK(0x000000FF,uint16_t))  /* Master receive register */

/* ------------------------------------------------------------------------------------------------------------------------
        I2C_I2CMTX                                                               Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_I2C_I2CMTX_I2CMTX                                                    0                               /* Master transmit register */
#define BITM_I2C_I2CMTX_I2CMTX                                                   (_ADI_MSK(0x000000FF,uint16_t))  /* Master transmit register */

/* ------------------------------------------------------------------------------------------------------------------------
        I2C_I2CMRXCNT                                                            Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_I2C_I2CMRXCNT_EXTEND                                                 8                               /* Extended read */
#define BITP_I2C_I2CMRXCNT_COUNT                                                  0                               /* Receive count */
#define BITM_I2C_I2CMRXCNT_EXTEND                                                (_ADI_MSK(0x00000100,uint16_t))  /* Extended read */
#define BITM_I2C_I2CMRXCNT_COUNT                                                 (_ADI_MSK(0x000000FF,uint16_t))  /* Receive count */

/* ------------------------------------------------------------------------------------------------------------------------
        I2C_I2CMCRXCNT                                                           Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_I2C_I2CMCRXCNT_COUNT                                                 0                               /* Current receive count */
#define BITM_I2C_I2CMCRXCNT_COUNT                                                (_ADI_MSK(0x000000FF,uint16_t))  /* Current receive count */

/* ------------------------------------------------------------------------------------------------------------------------
        I2C_I2CADR1                                                              Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_I2C_I2CADR1_ADR1                                                     0                               /* Address byte 1 */
#define BITM_I2C_I2CADR1_ADR1                                                    (_ADI_MSK(0x000000FF,uint16_t))  /* Address byte 1 */

/* ------------------------------------------------------------------------------------------------------------------------
        I2C_I2CADR2                                                              Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_I2C_I2CADR2_ADR2                                                     0                               /* Address byte 2 */
#define BITM_I2C_I2CADR2_ADR2                                                    (_ADI_MSK(0x000000FF,uint16_t))  /* Address byte 2 */

/* ------------------------------------------------------------------------------------------------------------------------
        I2C_I2CBYT                                                               Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_I2C_I2CBYT_SBYTE                                                     0                               /* Start byte */
#define BITM_I2C_I2CBYT_SBYTE                                                    (_ADI_MSK(0x000000FF,uint16_t))  /* Start byte */

/* ------------------------------------------------------------------------------------------------------------------------
        I2C_I2CDIV                                                               Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_I2C_I2CDIV_HIGH                                                      8                               /* Serial clock high time */
#define BITP_I2C_I2CDIV_LOW                                                       0                               /* Serial clock low time */
#define BITM_I2C_I2CDIV_HIGH                                                     (_ADI_MSK(0x0000FF00,uint16_t))  /* Serial clock high time */
#define BITM_I2C_I2CDIV_LOW                                                      (_ADI_MSK(0x000000FF,uint16_t))  /* Serial clock low time */

/* ------------------------------------------------------------------------------------------------------------------------
        I2C_I2CSCON                                                              Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_I2C_I2CSCON_STXDMA                                                  14                               /* Enable slave Tx DMA request */
#define BITP_I2C_I2CSCON_SRXDMA                                                  13                               /* Enable slave Rx DMA request */
#define BITP_I2C_I2CSCON_IENREPST                                                12                               /* Repeated start interrupt enable */
#define BITP_I2C_I2CSCON_SXMITDEC                                                11                               /* Decrement Slave Tx FIFO status when a byte has been transmitted */
#define BITP_I2C_I2CSCON_IENSTX                                                  10                               /* Slave Transmit request interrupt enable */
#define BITP_I2C_I2CSCON_IENSRX                                                   9                               /* Slave Receive request interrupt enable */
#define BITP_I2C_I2CSCON_IENSTOP                                                  8                               /* Stop condition detected interrupt enable */
#define BITP_I2C_I2CSCON_NACK                                                     7                               /* NACK next communication */
#define BITP_I2C_I2CSCON_STRETCHSCL                                               6                               /* Stretch SCL enable */
#define BITP_I2C_I2CSCON_EARLYTXR                                                 5                               /* Early transmit request mode */
#define BITP_I2C_I2CSCON_GCSBCLR                                                  4                               /* General call status bit clear */
#define BITP_I2C_I2CSCON_HGCEN                                                    3                               /* Hardware general call enable */
#define BITP_I2C_I2CSCON_GCEN                                                     2                               /* General call enable */
#define BITP_I2C_I2CSCON_ADR10EN                                                  1                               /* Enabled 10-bit addressing */
#define BITP_I2C_I2CSCON_SLVEN                                                    0                               /* Slave enable */
#define BITM_I2C_I2CSCON_STXDMA                                                  (_ADI_MSK(0x00004000,uint16_t))  /* Enable slave Tx DMA request */
#define BITM_I2C_I2CSCON_SRXDMA                                                  (_ADI_MSK(0x00002000,uint16_t))  /* Enable slave Rx DMA request */
#define BITM_I2C_I2CSCON_IENREPST                                                (_ADI_MSK(0x00001000,uint16_t))  /* Repeated start interrupt enable */
#define BITM_I2C_I2CSCON_SXMITDEC                                                (_ADI_MSK(0x00000800,uint16_t))  /* Decrement Slave Tx FIFO status when a byte has been transmitted */
#define BITM_I2C_I2CSCON_IENSTX                                                  (_ADI_MSK(0x00000400,uint16_t))  /* Slave Transmit request interrupt enable */
#define BITM_I2C_I2CSCON_IENSRX                                                  (_ADI_MSK(0x00000200,uint16_t))  /* Slave Receive request interrupt enable */
#define BITM_I2C_I2CSCON_IENSTOP                                                 (_ADI_MSK(0x00000100,uint16_t))  /* Stop condition detected interrupt enable */
#define BITM_I2C_I2CSCON_NACK                                                    (_ADI_MSK(0x00000080,uint16_t))  /* NACK next communication */
#define BITM_I2C_I2CSCON_STRETCHSCL                                              (_ADI_MSK(0x00000040,uint16_t))  /* Stretch SCL enable */
#define BITM_I2C_I2CSCON_EARLYTXR                                                (_ADI_MSK(0x00000020,uint16_t))  /* Early transmit request mode */
#define BITM_I2C_I2CSCON_GCSBCLR                                                 (_ADI_MSK(0x00000010,uint16_t))  /* General call status bit clear */
#define BITM_I2C_I2CSCON_HGCEN                                                   (_ADI_MSK(0x00000008,uint16_t))  /* Hardware general call enable */
#define BITM_I2C_I2CSCON_GCEN                                                    (_ADI_MSK(0x00000004,uint16_t))  /* General call enable */
#define BITM_I2C_I2CSCON_ADR10EN                                                 (_ADI_MSK(0x00000002,uint16_t))  /* Enabled 10-bit addressing */
#define BITM_I2C_I2CSCON_SLVEN                                                   (_ADI_MSK(0x00000001,uint16_t))  /* Slave enable */

/* ------------------------------------------------------------------------------------------------------------------------
        I2C_I2CSSTA                                                              Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_I2C_I2CSSTA_START                                                   14                               /* Start and matching address */
#define BITP_I2C_I2CSSTA_REPSTART                                                13                               /* Repeated start and matching address */
#define BITP_I2C_I2CSSTA_IDMAT                                                   11                               /* Device ID matched */
#define BITP_I2C_I2CSSTA_STOP                                                    10                               /* Stop after start and matching address */
#define BITP_I2C_I2CSSTA_GCID                                                     8                               /* General ID */
#define BITP_I2C_I2CSSTA_GCINT                                                    7                               /* General call interrupt */
#define BITP_I2C_I2CSSTA_SBUSY                                                    6                               /* Slave busy */
#define BITP_I2C_I2CSSTA_NOACK                                                    5                               /* Ack not generated by the slave */
#define BITP_I2C_I2CSSTA_SRXOF                                                    4                               /* Slave Receive FIFO overflow */
#define BITP_I2C_I2CSSTA_SRXREQ                                                   3                               /* Slave Receive request */
#define BITP_I2C_I2CSSTA_STXREQ                                                   2                               /* Slave Transmit request */
#define BITP_I2C_I2CSSTA_STXUR                                                    1                               /* Slave Transmit FIFO underflow */
#define BITP_I2C_I2CSSTA_STXFSEREQ                                                0                               /* Slave Tx FIFO Status or early request */
#define BITM_I2C_I2CSSTA_START                                                   (_ADI_MSK(0x00004000,uint16_t))  /* Start and matching address */
#define BITM_I2C_I2CSSTA_REPSTART                                                (_ADI_MSK(0x00002000,uint16_t))  /* Repeated start and matching address */
#define BITM_I2C_I2CSSTA_IDMAT                                                   (_ADI_MSK(0x00001800,uint16_t))  /* Device ID matched */
#define BITM_I2C_I2CSSTA_STOP                                                    (_ADI_MSK(0x00000400,uint16_t))  /* Stop after start and matching address */
#define BITM_I2C_I2CSSTA_GCID                                                    (_ADI_MSK(0x00000300,uint16_t))  /* General ID */
#define BITM_I2C_I2CSSTA_GCINT                                                   (_ADI_MSK(0x00000080,uint16_t))  /* General call interrupt */
#define BITM_I2C_I2CSSTA_SBUSY                                                   (_ADI_MSK(0x00000040,uint16_t))  /* Slave busy */
#define BITM_I2C_I2CSSTA_NOACK                                                   (_ADI_MSK(0x00000020,uint16_t))  /* Ack not generated by the slave */
#define BITM_I2C_I2CSSTA_SRXOF                                                   (_ADI_MSK(0x00000010,uint16_t))  /* Slave Receive FIFO overflow */
#define BITM_I2C_I2CSSTA_SRXREQ                                                  (_ADI_MSK(0x00000008,uint16_t))  /* Slave Receive request */
#define BITM_I2C_I2CSSTA_STXREQ                                                  (_ADI_MSK(0x00000004,uint16_t))  /* Slave Transmit request */
#define BITM_I2C_I2CSSTA_STXUR                                                   (_ADI_MSK(0x00000002,uint16_t))  /* Slave Transmit FIFO underflow */
#define BITM_I2C_I2CSSTA_STXFSEREQ                                               (_ADI_MSK(0x00000001,uint16_t))  /* Slave Tx FIFO Status or early request */

/* ------------------------------------------------------------------------------------------------------------------------
        I2C_I2CSRX                                                               Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_I2C_I2CSRX_I2CSRX                                                    0                               /* Slave receive register */
#define BITM_I2C_I2CSRX_I2CSRX                                                   (_ADI_MSK(0x000000FF,uint16_t))  /* Slave receive register */

/* ------------------------------------------------------------------------------------------------------------------------
        I2C_I2CSTX                                                               Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_I2C_I2CSTX_I2CSTX                                                    0                               /* Slave transmit register */
#define BITM_I2C_I2CSTX_I2CSTX                                                   (_ADI_MSK(0x000000FF,uint16_t))  /* Slave transmit register */

/* ------------------------------------------------------------------------------------------------------------------------
        I2C_I2CALT                                                               Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_I2C_I2CALT_ALT                                                       0                               /* Slave Alt */
#define BITM_I2C_I2CALT_ALT                                                      (_ADI_MSK(0x000000FF,uint16_t))  /* Slave Alt */

/* ------------------------------------------------------------------------------------------------------------------------
        I2C_I2CID0                                                               Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_I2C_I2CID0_ID0                                                       0                               /* Slave device ID 0 */
#define BITM_I2C_I2CID0_ID0                                                      (_ADI_MSK(0x000000FF,uint16_t))  /* Slave device ID 0 */

/* ------------------------------------------------------------------------------------------------------------------------
        I2C_I2CID1                                                               Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_I2C_I2CID1_ID1                                                       0                               /* Slave device ID 1 */
#define BITM_I2C_I2CID1_ID1                                                      (_ADI_MSK(0x000000FF,uint16_t))  /* Slave device ID 1 */

/* ------------------------------------------------------------------------------------------------------------------------
        I2C_I2CID2                                                               Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_I2C_I2CID2_ID2                                                       0                               /* Slave device ID 2 */
#define BITM_I2C_I2CID2_ID2                                                      (_ADI_MSK(0x000000FF,uint16_t))  /* Slave device ID 2 */

/* ------------------------------------------------------------------------------------------------------------------------
        I2C_I2CID3                                                               Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_I2C_I2CID3_ID3                                                       0                               /* Slave device ID 3 */
#define BITM_I2C_I2CID3_ID3                                                      (_ADI_MSK(0x000000FF,uint16_t))  /* Slave device ID 3 */

/* ------------------------------------------------------------------------------------------------------------------------
        I2C_I2CFSTA                                                              Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_I2C_I2CFSTA_MFLUSH                                                   9                               /* Flush the master transmit FIFO */
#define BITP_I2C_I2CFSTA_SFLUSH                                                   8                               /* Flush the slave transmit FIFO */
#define BITP_I2C_I2CFSTA_MRXFSTA                                                  6                               /* Master receive FIFO status */
#define BITP_I2C_I2CFSTA_MTXFSTA                                                  4                               /* Master transmit FIFO status */
#define BITP_I2C_I2CFSTA_SRXFSTA                                                  2                               /* Slave receive FIFO status */
#define BITP_I2C_I2CFSTA_STXFSTA                                                  0                               /* Slave transmit FIFO status */
#define BITM_I2C_I2CFSTA_MFLUSH                                                  (_ADI_MSK(0x00000200,uint16_t))  /* Flush the master transmit FIFO */
#define BITM_I2C_I2CFSTA_SFLUSH                                                  (_ADI_MSK(0x00000100,uint16_t))  /* Flush the slave transmit FIFO */
#define BITM_I2C_I2CFSTA_MRXFSTA                                                 (_ADI_MSK(0x000000C0,uint16_t))  /* Master receive FIFO status */
#define BITM_I2C_I2CFSTA_MTXFSTA                                                 (_ADI_MSK(0x00000030,uint16_t))  /* Master transmit FIFO status */
#define BITM_I2C_I2CFSTA_SRXFSTA                                                 (_ADI_MSK(0x0000000C,uint16_t))  /* Slave receive FIFO status */
#define BITM_I2C_I2CFSTA_STXFSTA                                                 (_ADI_MSK(0x00000003,uint16_t))  /* Slave transmit FIFO status */

/* ------------------------------------------------------------------------------------------------------------------------
        I2C_I2CSHCON                                                             Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_I2C_I2CSHCON_RESET                                                   0                               /* Reset START STOP detect circuit */
#define BITM_I2C_I2CSHCON_RESET                                                  (_ADI_MSK(0x00000001,uint16_t))  /* Reset START STOP detect circuit */

/* ------------------------------------------------------------------------------------------------------------------------
        I2C_I2CTCTL                                                              Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_I2C_I2CTCTL_FILTEROFF                                                8                               /* Input Filter Control */
#define BITP_I2C_I2CTCTL_THDATIN                                                  0                               /* Data In Hold Start */
#define BITM_I2C_I2CTCTL_FILTEROFF                                               (_ADI_MSK(0x00000100,uint16_t))  /* Input Filter Control */
#define BITM_I2C_I2CTCTL_THDATIN                                                 (_ADI_MSK(0x0000001F,uint16_t))  /* Data In Hold Start */

/* ==================================================
        SPI Registers
   ================================================== */

/* =========================
        SPI0
   ========================= */
#define REG_SPI0_SPISTA                      0x40004000         /* SPI0 Status */
#define REG_SPI0_SPIRX                       0x40004004         /* SPI0 Receive */
#define REG_SPI0_SPITX                       0x40004008         /* SPI0 Transmit */
#define REG_SPI0_SPIDIV                      0x4000400C         /* SPI0 Baud rate selection */
#define REG_SPI0_SPICON                      0x40004010         /* SPI0 SPI configuration */
#define REG_SPI0_SPIDMA                      0x40004014         /* SPI0 SPI DMA enable */
#define REG_SPI0_SPICNT                      0x40004018         /* SPI0 Transfer byte count */

/* =========================
        SPI1
   ========================= */
#define REG_SPI1_SPISTA                      0x40004400         /* SPI1 Status */
#define REG_SPI1_SPIRX                       0x40004404         /* SPI1 Receive */
#define REG_SPI1_SPITX                       0x40004408         /* SPI1 Transmit */
#define REG_SPI1_SPIDIV                      0x4000440C         /* SPI1 Baud rate selection */
#define REG_SPI1_SPICON                      0x40004410         /* SPI1 SPI configuration */
#define REG_SPI1_SPIDMA                      0x40004414         /* SPI1 SPI DMA enable */
#define REG_SPI1_SPICNT                      0x40004418         /* SPI1 Transfer byte count */

/* =========================
        SPI
   ========================= */
/* ------------------------------------------------------------------------------------------------------------------------
        SPI_SPISTA                                                               Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_SPI_SPISTA_CSRSG                                                    14                               /* Detected a rising edge on CS, in CONT mode */
#define BITP_SPI_SPISTA_CSFLG                                                    13                               /* Detected a falling edge on CS, in CONT mode */
#define BITP_SPI_SPISTA_CSERR                                                    12                               /* Detected a CS error condition */
#define BITP_SPI_SPISTA_RXS                                                      11                               /* SPI Rx FIFO excess bytes present */
#define BITP_SPI_SPISTA_RXFSTA                                                    8                               /* SPI Rx FIFO status */
#define BITP_SPI_SPISTA_RXOF                                                      7                               /* SPI Rx FIFO overflow */
#define BITP_SPI_SPISTA_RX                                                        6                               /* SPI Rx IRQ */
#define BITP_SPI_SPISTA_TX                                                        5                               /* SPI Tx IRQ */
#define BITP_SPI_SPISTA_TXUR                                                      4                               /* SPI Tx FIFO underflow */
#define BITP_SPI_SPISTA_TXFSTA                                                    1                               /* SPI Tx FIFO status */
#define BITP_SPI_SPISTA_IRQ                                                       0                               /* SPI Interrupt status */
#define BITM_SPI_SPISTA_CSRSG                                                    (_ADI_MSK(0x00004000,uint16_t))  /* Detected a rising edge on CS, in CONT mode */
#define BITM_SPI_SPISTA_CSFLG                                                    (_ADI_MSK(0x00002000,uint16_t))  /* Detected a falling edge on CS, in CONT mode */
#define BITM_SPI_SPISTA_CSERR                                                    (_ADI_MSK(0x00001000,uint16_t))  /* Detected a CS error condition */
#define BITM_SPI_SPISTA_RXS                                                      (_ADI_MSK(0x00000800,uint16_t))  /* SPI Rx FIFO excess bytes present */
#define BITM_SPI_SPISTA_RXFSTA                                                   (_ADI_MSK(0x00000700,uint16_t))  /* SPI Rx FIFO status */
#define BITM_SPI_SPISTA_RXOF                                                     (_ADI_MSK(0x00000080,uint16_t))  /* SPI Rx FIFO overflow */
#define BITM_SPI_SPISTA_RX                                                       (_ADI_MSK(0x00000040,uint16_t))  /* SPI Rx IRQ */
#define BITM_SPI_SPISTA_TX                                                       (_ADI_MSK(0x00000020,uint16_t))  /* SPI Tx IRQ */
#define BITM_SPI_SPISTA_TXUR                                                     (_ADI_MSK(0x00000010,uint16_t))  /* SPI Tx FIFO underflow */
#define BITM_SPI_SPISTA_TXFSTA                                                   (_ADI_MSK(0x0000000E,uint16_t))  /* SPI Tx FIFO status */
#define BITM_SPI_SPISTA_IRQ                                                      (_ADI_MSK(0x00000001,uint16_t))  /* SPI Interrupt status */

/* ------------------------------------------------------------------------------------------------------------------------
        SPI_SPIRX                                                                Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_SPI_SPIRX_DMA_DATA_BYTE_2                                            8                               /* 8-bit receive buffer */
#define BITP_SPI_SPIRX_DATA_BYTE_1                                                0                               /* 8-bit receive buffer */
#define BITM_SPI_SPIRX_DMA_DATA_BYTE_2                                           (_ADI_MSK(0x0000FF00,uint16_t))  /* 8-bit receive buffer */
#define BITM_SPI_SPIRX_DATA_BYTE_1                                               (_ADI_MSK(0x000000FF,uint16_t))  /* 8-bit receive buffer */

/* ------------------------------------------------------------------------------------------------------------------------
        SPI_SPITX                                                                Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_SPI_SPITX_DMA_DATA_BYTE_2                                            8                               /* 8-bit transmit buffer */
#define BITP_SPI_SPITX_DATA_BYTE_1                                                0                               /* 8-bit transmit buffer */
#define BITM_SPI_SPITX_DMA_DATA_BYTE_2                                           (_ADI_MSK(0x0000FF00,uint16_t))  /* 8-bit transmit buffer */
#define BITM_SPI_SPITX_DATA_BYTE_1                                               (_ADI_MSK(0x000000FF,uint16_t))  /* 8-bit transmit buffer */

/* ------------------------------------------------------------------------------------------------------------------------
        SPI_SPIDIV                                                               Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_SPI_SPIDIV_CSIRQ_EN                                                  8                               /* Enable interrupt on every CS edge in CONT mode */
#define BITP_SPI_SPIDIV_MD_CS_RST                                                 7                               /* Reset Mode for CSERR */
#define BITP_SPI_SPIDIV_HFM                                                       6                               /* High Frequency Mode */
#define BITP_SPI_SPIDIV_DIV                                                       0                               /* SPI clock divider */
#define BITM_SPI_SPIDIV_CSIRQ_EN                                                 (_ADI_MSK(0x00000100,uint16_t))  /* Enable interrupt on every CS edge in CONT mode */
#define BITM_SPI_SPIDIV_MD_CS_RST                                                (_ADI_MSK(0x00000080,uint16_t))  /* Reset Mode for CSERR */
#define BITM_SPI_SPIDIV_HFM                                                      (_ADI_MSK(0x00000040,uint16_t))  /* High Frequency Mode */
#define BITM_SPI_SPIDIV_DIV                                                      (_ADI_MSK(0x0000003F,uint16_t))  /* SPI clock divider */

/* ------------------------------------------------------------------------------------------------------------------------
        SPI_SPICON                                                               Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_SPI_SPICON_MOD                                                      14                               /* SPI IRQ mode bits */
#define BITP_SPI_SPICON_TFLUSH                                                   13                               /* SPI Tx FIFO Flush enable */
#define BITP_SPI_SPICON_RFLUSH                                                   12                               /* SPI Rx FIFO Flush enable */
#define BITP_SPI_SPICON_CON                                                      11                               /* Continuous transfer enable */
#define BITP_SPI_SPICON_LOOPBACK                                                 10                               /* Loopback enable */
#define BITP_SPI_SPICON_OEN                                                       9                               /* Slave MISO output enable */
#define BITP_SPI_SPICON_RXOF                                                      8                               /* SPIRX overflow overwrite enable */
#define BITP_SPI_SPICON_ZEN                                                       7                               /* Transmit zeros enable */
#define BITP_SPI_SPICON_TIM                                                       6                               /* SPI transfer and interrupt mode */
#define BITP_SPI_SPICON_LSB                                                       5                               /* LSB first transfer enable */
#define BITP_SPI_SPICON_WOM                                                       4                               /* SPI Wired Or mode */
#define BITP_SPI_SPICON_CPOL                                                      3                               /* Serial Clock Polarity */
#define BITP_SPI_SPICON_CPHA                                                      2                               /* Serial clock phase mode */
#define BITP_SPI_SPICON_MASEN                                                     1                               /* Master mode enable */
#define BITP_SPI_SPICON_ENABLE                                                    0                               /* SPI enable */
#define BITM_SPI_SPICON_MOD                                                      (_ADI_MSK(0x0000C000,uint16_t))  /* SPI IRQ mode bits */
#define BITM_SPI_SPICON_TFLUSH                                                   (_ADI_MSK(0x00002000,uint16_t))  /* SPI Tx FIFO Flush enable */
#define BITM_SPI_SPICON_RFLUSH                                                   (_ADI_MSK(0x00001000,uint16_t))  /* SPI Rx FIFO Flush enable */
#define BITM_SPI_SPICON_CON                                                      (_ADI_MSK(0x00000800,uint16_t))  /* Continuous transfer enable */
#define BITM_SPI_SPICON_LOOPBACK                                                 (_ADI_MSK(0x00000400,uint16_t))  /* Loopback enable */
#define BITM_SPI_SPICON_OEN                                                      (_ADI_MSK(0x00000200,uint16_t))  /* Slave MISO output enable */
#define BITM_SPI_SPICON_RXOF                                                     (_ADI_MSK(0x00000100,uint16_t))  /* SPIRX overflow overwrite enable */
#define BITM_SPI_SPICON_ZEN                                                      (_ADI_MSK(0x00000080,uint16_t))  /* Transmit zeros enable */
#define BITM_SPI_SPICON_TIM                                                      (_ADI_MSK(0x00000040,uint16_t))  /* SPI transfer and interrupt mode */
#define BITM_SPI_SPICON_LSB                                                      (_ADI_MSK(0x00000020,uint16_t))  /* LSB first transfer enable */
#define BITM_SPI_SPICON_WOM                                                      (_ADI_MSK(0x00000010,uint16_t))  /* SPI Wired Or mode */
#define BITM_SPI_SPICON_CPOL                                                     (_ADI_MSK(0x00000008,uint16_t))  /* Serial Clock Polarity */
#define BITM_SPI_SPICON_CPHA                                                     (_ADI_MSK(0x00000004,uint16_t))  /* Serial clock phase mode */
#define BITM_SPI_SPICON_MASEN                                                    (_ADI_MSK(0x00000002,uint16_t))  /* Master mode enable */
#define BITM_SPI_SPICON_ENABLE                                                   (_ADI_MSK(0x00000001,uint16_t))  /* SPI enable */

/* ------------------------------------------------------------------------------------------------------------------------
        SPI_SPIDMA                                                               Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_SPI_SPIDMA_IENRXDMA                                                  2                               /* Enable receive DMA request */
#define BITP_SPI_SPIDMA_IENTXDMA                                                  1                               /* Enable transmit DMA request */
#define BITP_SPI_SPIDMA_ENABLE                                                    0                               /* Enable DMA for data transfer */
#define BITM_SPI_SPIDMA_IENRXDMA                                                 (_ADI_MSK(0x00000004,uint16_t))  /* Enable receive DMA request */
#define BITM_SPI_SPIDMA_IENTXDMA                                                 (_ADI_MSK(0x00000002,uint16_t))  /* Enable transmit DMA request */
#define BITM_SPI_SPIDMA_ENABLE                                                   (_ADI_MSK(0x00000001,uint16_t))  /* Enable DMA for data transfer */

/* ------------------------------------------------------------------------------------------------------------------------
        SPI_SPICNT                                                               Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_SPI_SPICNT_COUNT                                                     0                               /* Transfer byte count */
#define BITM_SPI_SPICNT_COUNT                                                    (_ADI_MSK(0x000000FF,uint16_t))  /* Transfer byte count */

/* ==================================================
        UART Registers
   ================================================== */

/* =========================
        UART
   ========================= */
#define REG_UART_COMRX                       0x40005000         /* UART Receive Buffer Register */
#define REG_UART_COMTX                       0x40005000         /* UART Transmit Holding Register */
#define REG_UART_COMIEN                      0x40005004         /* UART Interrupt Enable */
#define REG_UART_COMIIR                      0x40005008         /* UART Interrupt ID */
#define REG_UART_COMLCR                      0x4000500C         /* UART Line Control */
#define REG_UART_COMMCR                      0x40005010         /* UART Modem Control */
#define REG_UART_COMLSR                      0x40005014         /* UART Line Status */
#define REG_UART_COMMSR                      0x40005018         /* UART Modem Status */
#define REG_UART_COMSCR                      0x4000501C         /* UART Scratch buffer */
#define REG_UART_COMFBR                      0x40005024         /* UART Fractional Baud Rate */
#define REG_UART_COMDIV                      0x40005028         /* UART Baudrate divider */

/* =========================
        UART
   ========================= */
/* ------------------------------------------------------------------------------------------------------------------------
        UART_COMRX                                                               Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_UART_COMRX_RBR                                                       0                               /* Receive Buffer Register */
#define BITM_UART_COMRX_RBR                                                      (_ADI_MSK(0x000000FF,uint16_t))  /* Receive Buffer Register */

/* ------------------------------------------------------------------------------------------------------------------------
        UART_COMTX                                                               Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_UART_COMTX_THR                                                       0                               /* Transmit Holding Register */
#define BITM_UART_COMTX_THR                                                      (_ADI_MSK(0x000000FF,uint16_t))  /* Transmit Holding Register */

/* ------------------------------------------------------------------------------------------------------------------------
        UART_COMIEN                                                              Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_UART_COMIEN_EDMAR                                                    5                               /* DMA requests in transmit mode */
#define BITP_UART_COMIEN_EDMAT                                                    4                               /* DMA requests in receive mode */
#define BITP_UART_COMIEN_EDSSI                                                    3                               /* Modem status interrupt */
#define BITP_UART_COMIEN_ELSI                                                     2                               /* Rx status interrupt */
#define BITP_UART_COMIEN_ETBEI                                                    1                               /* Transmit buffer empty interrupt */
#define BITP_UART_COMIEN_ERBFI                                                    0                               /* Receive buffer full interrupt */
#define BITM_UART_COMIEN_EDMAR                                                   (_ADI_MSK(0x00000020,uint16_t))  /* DMA requests in transmit mode */
#define BITM_UART_COMIEN_EDMAT                                                   (_ADI_MSK(0x00000010,uint16_t))  /* DMA requests in receive mode */
#define BITM_UART_COMIEN_EDSSI                                                   (_ADI_MSK(0x00000008,uint16_t))  /* Modem status interrupt */
#define BITM_UART_COMIEN_ELSI                                                    (_ADI_MSK(0x00000004,uint16_t))  /* Rx status interrupt */
#define BITM_UART_COMIEN_ETBEI                                                   (_ADI_MSK(0x00000002,uint16_t))  /* Transmit buffer empty interrupt */
#define BITM_UART_COMIEN_ERBFI                                                   (_ADI_MSK(0x00000001,uint16_t))  /* Receive buffer full interrupt */

/* ------------------------------------------------------------------------------------------------------------------------
        UART_COMIIR                                                              Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_UART_COMIIR_STA                                                      1                               /* Interrupt status */
#define BITP_UART_COMIIR_NIRQ                                                     0                               /* Interrupt flag */
#define BITM_UART_COMIIR_STA                                                     (_ADI_MSK(0x00000006,uint16_t))  /* Interrupt status */
#define BITM_UART_COMIIR_NIRQ                                                    (_ADI_MSK(0x00000001,uint16_t))  /* Interrupt flag */

/* ------------------------------------------------------------------------------------------------------------------------
        UART_COMLCR                                                              Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_UART_COMLCR_BRK                                                      6                               /* Set Break */
#define BITP_UART_COMLCR_SP                                                       5                               /* Stick Parity */
#define BITP_UART_COMLCR_EPS                                                      4                               /* Parity Select */
#define BITP_UART_COMLCR_PEN                                                      3                               /* Parity Enable */
#define BITP_UART_COMLCR_STOP                                                     2                               /* Stop Bit */
#define BITP_UART_COMLCR_WLS                                                      0                               /* Word Length Select */
#define BITM_UART_COMLCR_BRK                                                     (_ADI_MSK(0x00000040,uint16_t))  /* Set Break */
#define BITM_UART_COMLCR_SP                                                      (_ADI_MSK(0x00000020,uint16_t))  /* Stick Parity */
#define BITM_UART_COMLCR_EPS                                                     (_ADI_MSK(0x00000010,uint16_t))  /* Parity Select */
#define BITM_UART_COMLCR_PEN                                                     (_ADI_MSK(0x00000008,uint16_t))  /* Parity Enable */
#define BITM_UART_COMLCR_STOP                                                    (_ADI_MSK(0x00000004,uint16_t))  /* Stop Bit */
#define BITM_UART_COMLCR_WLS                                                     (_ADI_MSK(0x00000003,uint16_t))  /* Word Length Select */

/* ------------------------------------------------------------------------------------------------------------------------
        UART_COMMCR                                                              Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_UART_COMMCR_LOOPBACK                                                 4                               /* Loopback mode */
#define BITP_UART_COMMCR_OUT2                                                     3                               /* Output 2 */
#define BITP_UART_COMMCR_OUT1                                                     2                               /* Output 1 */
#define BITP_UART_COMMCR_RTS                                                      1                               /* Request to send */
#define BITP_UART_COMMCR_DTR                                                      0                               /* Data Terminal Ready */
#define BITM_UART_COMMCR_LOOPBACK                                                (_ADI_MSK(0x00000010,uint16_t))  /* Loopback mode */
#define BITM_UART_COMMCR_OUT2                                                    (_ADI_MSK(0x00000008,uint16_t))  /* Output 2 */
#define BITM_UART_COMMCR_OUT1                                                    (_ADI_MSK(0x00000004,uint16_t))  /* Output 1 */
#define BITM_UART_COMMCR_RTS                                                     (_ADI_MSK(0x00000002,uint16_t))  /* Request to send */
#define BITM_UART_COMMCR_DTR                                                     (_ADI_MSK(0x00000001,uint16_t))  /* Data Terminal Ready */

/* ------------------------------------------------------------------------------------------------------------------------
        UART_COMLSR                                                              Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_UART_COMLSR_TEMT                                                     6                               /* COMTX and Shift Register Empty Status */
#define BITP_UART_COMLSR_THRE                                                     5                               /* COMTX Empty */
#define BITP_UART_COMLSR_BI                                                       4                               /* Break Indicator */
#define BITP_UART_COMLSR_FE                                                       3                               /* Framing Error */
#define BITP_UART_COMLSR_PE                                                       2                               /* Parity Error */
#define BITP_UART_COMLSR_OE                                                       1                               /* Overrun Error */
#define BITP_UART_COMLSR_DR                                                       0                               /* Data Ready */
#define BITM_UART_COMLSR_TEMT                                                    (_ADI_MSK(0x00000040,uint16_t))  /* COMTX and Shift Register Empty Status */
#define BITM_UART_COMLSR_THRE                                                    (_ADI_MSK(0x00000020,uint16_t))  /* COMTX Empty */
#define BITM_UART_COMLSR_BI                                                      (_ADI_MSK(0x00000010,uint16_t))  /* Break Indicator */
#define BITM_UART_COMLSR_FE                                                      (_ADI_MSK(0x00000008,uint16_t))  /* Framing Error */
#define BITM_UART_COMLSR_PE                                                      (_ADI_MSK(0x00000004,uint16_t))  /* Parity Error */
#define BITM_UART_COMLSR_OE                                                      (_ADI_MSK(0x00000002,uint16_t))  /* Overrun Error */
#define BITM_UART_COMLSR_DR                                                      (_ADI_MSK(0x00000001,uint16_t))  /* Data Ready */

/* ------------------------------------------------------------------------------------------------------------------------
        UART_COMMSR                                                              Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_UART_COMMSR_DCD                                                      7                               /* Data Carrier Detect */
#define BITP_UART_COMMSR_RI                                                       6                               /* Ring Indicator */
#define BITP_UART_COMMSR_DSR                                                      5                               /* Data Set Ready */
#define BITP_UART_COMMSR_CTS                                                      4                               /* Clear To Send */
#define BITP_UART_COMMSR_DDCD                                                     3                               /* Delta DCD */
#define BITP_UART_COMMSR_TERI                                                     2                               /* Trailing Edge RI */
#define BITP_UART_COMMSR_DDSR                                                     1                               /* Delta DSR */
#define BITP_UART_COMMSR_DCTS                                                     0                               /* Delta CTS */
#define BITM_UART_COMMSR_DCD                                                     (_ADI_MSK(0x00000080,uint16_t))  /* Data Carrier Detect */
#define BITM_UART_COMMSR_RI                                                      (_ADI_MSK(0x00000040,uint16_t))  /* Ring Indicator */
#define BITM_UART_COMMSR_DSR                                                     (_ADI_MSK(0x00000020,uint16_t))  /* Data Set Ready */
#define BITM_UART_COMMSR_CTS                                                     (_ADI_MSK(0x00000010,uint16_t))  /* Clear To Send */
#define BITM_UART_COMMSR_DDCD                                                    (_ADI_MSK(0x00000008,uint16_t))  /* Delta DCD */
#define BITM_UART_COMMSR_TERI                                                    (_ADI_MSK(0x00000004,uint16_t))  /* Trailing Edge RI */
#define BITM_UART_COMMSR_DDSR                                                    (_ADI_MSK(0x00000002,uint16_t))  /* Delta DSR */
#define BITM_UART_COMMSR_DCTS                                                    (_ADI_MSK(0x00000001,uint16_t))  /* Delta CTS */

/* ------------------------------------------------------------------------------------------------------------------------
        UART_COMSCR                                                              Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_UART_COMSCR_SCR                                                      0                               /* Scratch */
#define BITM_UART_COMSCR_SCR                                                     (_ADI_MSK(0x000000FF,uint16_t))  /* Scratch */

/* ------------------------------------------------------------------------------------------------------------------------
        UART_COMFBR                                                              Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_UART_COMFBR_FBEN                                                    15                               /* Fractional baud rate generator enable */
#define BITP_UART_COMFBR_DIVM                                                    11                               /* Fractional baud rate M divide bits 1 to 3 */
#define BITP_UART_COMFBR_DIVN                                                     0                               /* Fractional baud rate N divide bits 0 to 2047. */
#define BITM_UART_COMFBR_FBEN                                                    (_ADI_MSK(0x00008000,uint16_t))  /* Fractional baud rate generator enable */
#define BITM_UART_COMFBR_DIVM                                                    (_ADI_MSK(0x00001800,uint16_t))  /* Fractional baud rate M divide bits 1 to 3 */
#define BITM_UART_COMFBR_DIVN                                                    (_ADI_MSK(0x000007FF,uint16_t))  /* Fractional baud rate N divide bits 0 to 2047. */

/* ==================================================
        I2S serial port Registers
   ================================================== */

/* =========================
        I2S
   ========================= */
#define REG_I2S_I2S_OUT1L                    0x40005800         /* I2S Channel 1 LSBs */
#define REG_I2S_I2S_OUT1H                    0x40005804         /* I2S Channel 1 MSBs */
#define REG_I2S_I2S_OUT2L                    0x40005808         /* I2S Channel 2 LSBs */
#define REG_I2S_I2S_OUT2H                    0x4000580C         /* I2S Channel 2 MSBs */
#define REG_I2S_I2S_MODE1                    0x40005810         /* I2S I2S format modes 1 */
#define REG_I2S_I2S_MODE2                    0x40005814         /* I2S I2S format modes 2 */
#define REG_I2S_I2S_CFG1                     0x40005818         /* I2S I2S configuration 1 */
#define REG_I2S_I2S_CFG2                     0x4000581C         /* I2S I2S configuration 2 */
#define REG_I2S_I2S_STAT                     0x40005820         /* I2S I2S status */

/* =========================
        I2S
   ========================= */
/* ------------------------------------------------------------------------------------------------------------------------
        I2S_I2S_OUT1L                                                            Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_I2S_I2S_OUT1L_OUT1_LSBS                                              8                               /* LSBs of channel 1 output */
#define BITM_I2S_I2S_OUT1L_OUT1_LSBS                                             (_ADI_MSK(0x0000FF00,uint16_t))  /* LSBs of channel 1 output */

/* ------------------------------------------------------------------------------------------------------------------------
        I2S_I2S_OUT2L                                                            Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_I2S_I2S_OUT2L_OUT2_LSBS                                              8                               /* LSBs of channel 2 output */
#define BITM_I2S_I2S_OUT2L_OUT2_LSBS                                             (_ADI_MSK(0x0000FF00,uint16_t))  /* LSBs of channel 2 output */

/* ------------------------------------------------------------------------------------------------------------------------
        I2S_I2S_MODE1                                                            Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_I2S_I2S_MODE1_SDATA_FMT                                             14                               /* Data format */
#define BITP_I2S_I2S_MODE1_SAI                                                   11                               /* Channel format */
#define BITP_I2S_I2S_MODE1_FS                                                     8                               /* Frame rate */
#define BITP_I2S_I2S_MODE1_SLOT_WIDTH                                             6                               /* Sloth width */
#define BITP_I2S_I2S_MODE1_DATA_WIDTH                                             5                               /* Data Width */
#define BITP_I2S_I2S_MODE1_LR_MODE                                                4                               /* TDM clock mode */
#define BITP_I2S_I2S_MODE1_SAI_MSB                                                3                               /* Sdata endian format */
#define BITP_I2S_I2S_MODE1_BCLK_RATE                                              2                               /* Bit clock rate */
#define BITP_I2S_I2S_MODE1_SAI_MS                                                 1                               /* Master slave select */
#define BITP_I2S_I2S_MODE1_DRV_HIZ                                                0                               /* Unused bit drive option */
#define BITM_I2S_I2S_MODE1_SDATA_FMT                                             (_ADI_MSK(0x0000C000,uint16_t))  /* Data format */
#define BITM_I2S_I2S_MODE1_SAI                                                   (_ADI_MSK(0x00003800,uint16_t))  /* Channel format */
#define BITM_I2S_I2S_MODE1_FS                                                    (_ADI_MSK(0x00000700,uint16_t))  /* Frame rate */
#define BITM_I2S_I2S_MODE1_SLOT_WIDTH                                            (_ADI_MSK(0x000000C0,uint16_t))  /* Sloth width */
#define BITM_I2S_I2S_MODE1_DATA_WIDTH                                            (_ADI_MSK(0x00000020,uint16_t))  /* Data Width */
#define BITM_I2S_I2S_MODE1_LR_MODE                                               (_ADI_MSK(0x00000010,uint16_t))  /* TDM clock mode */
#define BITM_I2S_I2S_MODE1_SAI_MSB                                               (_ADI_MSK(0x00000008,uint16_t))  /* Sdata endian format */
#define BITM_I2S_I2S_MODE1_BCLK_RATE                                             (_ADI_MSK(0x00000004,uint16_t))  /* Bit clock rate */
#define BITM_I2S_I2S_MODE1_SAI_MS                                                (_ADI_MSK(0x00000002,uint16_t))  /* Master slave select */
#define BITM_I2S_I2S_MODE1_DRV_HIZ                                               (_ADI_MSK(0x00000001,uint16_t))  /* Unused bit drive option */

/* ------------------------------------------------------------------------------------------------------------------------
        I2S_I2S_MODE2                                                            Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_I2S_I2S_MODE2_LR_POL                                                15                               /* Lrclk polarity */
#define BITP_I2S_I2S_MODE2_BCLK_EDGE                                             14                               /* Bit clock edge */
#define BITP_I2S_I2S_MODE2_DRV_CH2                                               13                               /* Channel 2 slot drive enable */
#define BITP_I2S_I2S_MODE2_DRV_CH1                                               12                               /* Channel 1 slot drive enable */
#define BITP_I2S_I2S_MODE2_CMAP_C2                                                8                               /* Channel 2 output slot */
#define BITP_I2S_I2S_MODE2_CMAP_C1                                                4                               /* Channel 1 output slot */
#define BITM_I2S_I2S_MODE2_LR_POL                                                (_ADI_MSK(0x00008000,uint16_t))  /* Lrclk polarity */
#define BITM_I2S_I2S_MODE2_BCLK_EDGE                                             (_ADI_MSK(0x00004000,uint16_t))  /* Bit clock edge */
#define BITM_I2S_I2S_MODE2_DRV_CH2                                               (_ADI_MSK(0x00002000,uint16_t))  /* Channel 2 slot drive enable */
#define BITM_I2S_I2S_MODE2_DRV_CH1                                               (_ADI_MSK(0x00001000,uint16_t))  /* Channel 1 slot drive enable */
#define BITM_I2S_I2S_MODE2_CMAP_C2                                               (_ADI_MSK(0x00000F00,uint16_t))  /* Channel 2 output slot */
#define BITM_I2S_I2S_MODE2_CMAP_C1                                               (_ADI_MSK(0x000000F0,uint16_t))  /* Channel 1 output slot */

/* ------------------------------------------------------------------------------------------------------------------------
        I2S_I2S_CFG1                                                             Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_I2S_I2S_CFG1_FIFO_RST                                               15                               /* FIFO reset and hold */
#define BITP_I2S_I2S_CFG1_INCR_RST                                               14                               /* Address auto increment reset */
#define BITP_I2S_I2S_CFG1_DMA_EN                                                 13                               /* DMA enable */
#define BITP_I2S_I2S_CFG1_REQ_EN                                                 12                               /* Interrupt enable for serial data requests */
#define BITP_I2S_I2S_CFG1_STAT_EN                                                11                               /* Interrupt enable for FIFO status */
#define BITP_I2S_I2S_CFG1_AUTO_INCR                                              10                               /* Addressing auto increment */
#define BITP_I2S_I2S_CFG1_TRAN_MODE                                               8                               /* Transfer mode: 24-bit, 16-bit or 8-bit */
#define BITP_I2S_I2S_CFG1_CHAN_SEL2                                               7                               /* Channel 2 APB transfer */
#define BITP_I2S_I2S_CFG1_CHAN_SEL1                                               6                               /* Channel 1 APB transfer */
#define BITP_I2S_I2S_CFG1_I2S_EN                                                  0                               /* Enable I2S port */
#define BITM_I2S_I2S_CFG1_FIFO_RST                                               (_ADI_MSK(0x00008000,uint16_t))  /* FIFO reset and hold */
#define BITM_I2S_I2S_CFG1_INCR_RST                                               (_ADI_MSK(0x00004000,uint16_t))  /* Address auto increment reset */
#define BITM_I2S_I2S_CFG1_DMA_EN                                                 (_ADI_MSK(0x00002000,uint16_t))  /* DMA enable */
#define BITM_I2S_I2S_CFG1_REQ_EN                                                 (_ADI_MSK(0x00001000,uint16_t))  /* Interrupt enable for serial data requests */
#define BITM_I2S_I2S_CFG1_STAT_EN                                                (_ADI_MSK(0x00000800,uint16_t))  /* Interrupt enable for FIFO status */
#define BITM_I2S_I2S_CFG1_AUTO_INCR                                              (_ADI_MSK(0x00000400,uint16_t))  /* Addressing auto increment */
#define BITM_I2S_I2S_CFG1_TRAN_MODE                                              (_ADI_MSK(0x00000300,uint16_t))  /* Transfer mode: 24-bit, 16-bit or 8-bit */
#define BITM_I2S_I2S_CFG1_CHAN_SEL2                                              (_ADI_MSK(0x00000080,uint16_t))  /* Channel 2 APB transfer */
#define BITM_I2S_I2S_CFG1_CHAN_SEL1                                              (_ADI_MSK(0x00000040,uint16_t))  /* Channel 1 APB transfer */
#define BITM_I2S_I2S_CFG1_I2S_EN                                                 (_ADI_MSK(0x00000001,uint16_t))  /* Enable I2S port */

/* ------------------------------------------------------------------------------------------------------------------------
        I2S_I2S_CFG2                                                             Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_I2S_I2S_CFG2_REQ_FREQ                                                4                               /* Data request interrupt sampling frequency */
#define BITP_I2S_I2S_CFG2_DMA_FREQ                                                2                               /* DMA request sampling frequency */
#define BITP_I2S_I2S_CFG2_SAMP_FREQ                                               0                               /* Serial output data sampling frequency */
#define BITM_I2S_I2S_CFG2_REQ_FREQ                                               (_ADI_MSK(0x00000030,uint16_t))  /* Data request interrupt sampling frequency */
#define BITM_I2S_I2S_CFG2_DMA_FREQ                                               (_ADI_MSK(0x0000000C,uint16_t))  /* DMA request sampling frequency */
#define BITM_I2S_I2S_CFG2_SAMP_FREQ                                              (_ADI_MSK(0x00000003,uint16_t))  /* Serial output data sampling frequency */

/* ------------------------------------------------------------------------------------------------------------------------
        I2S_I2S_STAT                                                             Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_I2S_I2S_STAT_FIFO_STAT                                               2                               /* FIFO status */
#define BITP_I2S_I2S_STAT_REQ_PEND                                                1                               /* Data request pending */
#define BITP_I2S_I2S_STAT_STAT_PEND                                               0                               /* FIFO status error */
#define BITM_I2S_I2S_STAT_FIFO_STAT                                              (_ADI_MSK(0x0000003C,uint16_t))  /* FIFO status */
#define BITM_I2S_I2S_STAT_REQ_PEND                                               (_ADI_MSK(0x00000002,uint16_t))  /* Data request pending */
#define BITM_I2S_I2S_STAT_STAT_PEND                                              (_ADI_MSK(0x00000001,uint16_t))  /* FIFO status error */

/* ==================================================
        Beeper Driver Registers
   ================================================== */

/* =========================
        BEEP
   ========================= */
#define REG_BEEP_BEEP_CFG                    0x40005C00         /* BEEP Beeper configuration */
#define REG_BEEP_BEEP_STAT                   0x40005C04         /* BEEP Beeper status */
#define REG_BEEP_BEEP_TONE_A                 0x40005C08         /* BEEP Tone A Data */
#define REG_BEEP_BEEP_TONE_B                 0x40005C0C         /* BEEP Tone B Data */

/* =========================
        BEEP
   ========================= */
/* ------------------------------------------------------------------------------------------------------------------------
        BEEP_BEEP_CFG                                                            Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_BEEP_BEEP_CFG_IRQ_SEQ_END                                           15                               /* Sequence end IRQ */
#define BITP_BEEP_BEEP_CFG_IRQ_SEQ_NEAR_END                                      14                               /* Sequence 1 cycle from end IRQ */
#define BITP_BEEP_BEEP_CFG_IRQ_TONEB_END                                         13                               /* Tone B end IRQ */
#define BITP_BEEP_BEEP_CFG_IRQ_TONEB_START                                       12                               /* Tone B start IRQ */
#define BITP_BEEP_BEEP_CFG_IRQ_TONEA_END                                         11                               /* Tone A end IRQ */
#define BITP_BEEP_BEEP_CFG_IRQ_TONEA_START                                       10                               /* Tone A start IRQ */
#define BITP_BEEP_BEEP_CFG_BEEP_EN                                                8                               /* Beeper Enable */
#define BITP_BEEP_BEEP_CFG_SEQ_REPEAT                                             0                               /* Beeper Sequence Repeat value */
#define BITM_BEEP_BEEP_CFG_IRQ_SEQ_END                                           (_ADI_MSK(0x00008000,uint16_t))  /* Sequence end IRQ */
#define BITM_BEEP_BEEP_CFG_IRQ_SEQ_NEAR_END                                      (_ADI_MSK(0x00004000,uint16_t))  /* Sequence 1 cycle from end IRQ */
#define BITM_BEEP_BEEP_CFG_IRQ_TONEB_END                                         (_ADI_MSK(0x00002000,uint16_t))  /* Tone B end IRQ */
#define BITM_BEEP_BEEP_CFG_IRQ_TONEB_START                                       (_ADI_MSK(0x00001000,uint16_t))  /* Tone B start IRQ */
#define BITM_BEEP_BEEP_CFG_IRQ_TONEA_END                                         (_ADI_MSK(0x00000800,uint16_t))  /* Tone A end IRQ */
#define BITM_BEEP_BEEP_CFG_IRQ_TONEA_START                                       (_ADI_MSK(0x00000400,uint16_t))  /* Tone A start IRQ */
#define BITM_BEEP_BEEP_CFG_BEEP_EN                                               (_ADI_MSK(0x00000100,uint16_t))  /* Beeper Enable */
#define BITM_BEEP_BEEP_CFG_SEQ_REPEAT                                            (_ADI_MSK(0x000000FF,uint16_t))  /* Beeper Sequence Repeat value */

/* ------------------------------------------------------------------------------------------------------------------------
        BEEP_BEEP_STAT                                                           Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_BEEP_BEEP_STAT_EVT_SEQ_END                                          15                               /* Sequencer has ended */
#define BITP_BEEP_BEEP_STAT_EVT_SEQ_NEAR_END                                     14                               /* Sequencer last tone-pair has started */
#define BITP_BEEP_BEEP_STAT_EVT_TONEB_END                                        13                               /* Tone B has ended */
#define BITP_BEEP_BEEP_STAT_EVT_TONEB_START                                      12                               /* Tone B has started */
#define BITP_BEEP_BEEP_STAT_EVT_TONEA_END                                        11                               /* Tone A has ended */
#define BITP_BEEP_BEEP_STAT_EVT_TONEA_START                                      10                               /* Tone A has started */
#define BITP_BEEP_BEEP_STAT_BEEP_BUSY                                             8                               /* Beeper is busy */
#define BITP_BEEP_BEEP_STAT_SEQ_REMAIN                                            0                               /* Remaining sequence iterations to play in SEQ mode */
#define BITM_BEEP_BEEP_STAT_EVT_SEQ_END                                          (_ADI_MSK(0x00008000,uint16_t))  /* Sequencer has ended */
#define BITM_BEEP_BEEP_STAT_EVT_SEQ_NEAR_END                                     (_ADI_MSK(0x00004000,uint16_t))  /* Sequencer last tone-pair has started */
#define BITM_BEEP_BEEP_STAT_EVT_TONEB_END                                        (_ADI_MSK(0x00002000,uint16_t))  /* Tone B has ended */
#define BITM_BEEP_BEEP_STAT_EVT_TONEB_START                                      (_ADI_MSK(0x00001000,uint16_t))  /* Tone B has started */
#define BITM_BEEP_BEEP_STAT_EVT_TONEA_END                                        (_ADI_MSK(0x00000800,uint16_t))  /* Tone A has ended */
#define BITM_BEEP_BEEP_STAT_EVT_TONEA_START                                      (_ADI_MSK(0x00000400,uint16_t))  /* Tone A has started */
#define BITM_BEEP_BEEP_STAT_BEEP_BUSY                                            (_ADI_MSK(0x00000100,uint16_t))  /* Beeper is busy */
#define BITM_BEEP_BEEP_STAT_SEQ_REMAIN                                           (_ADI_MSK(0x000000FF,uint16_t))  /* Remaining sequence iterations to play in SEQ mode */

/* ------------------------------------------------------------------------------------------------------------------------
        BEEP_BEEP_TONE_A                                                         Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_BEEP_BEEP_TONE_A_DISABLE                                            15                               /* Output disable */
#define BITP_BEEP_BEEP_TONE_A_FREQ                                                8                               /* Tone frequency */
#define BITP_BEEP_BEEP_TONE_A_DUR                                                 0                               /* Tone duration */
#define BITM_BEEP_BEEP_TONE_A_DISABLE                                            (_ADI_MSK(0x00008000,uint16_t))  /* Output disable */
#define BITM_BEEP_BEEP_TONE_A_FREQ                                               (_ADI_MSK(0x00007F00,uint16_t))  /* Tone frequency */
#define BITM_BEEP_BEEP_TONE_A_DUR                                                (_ADI_MSK(0x000000FF,uint16_t))  /* Tone duration */

/* ------------------------------------------------------------------------------------------------------------------------
        BEEP_BEEP_TONE_B                                                         Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_BEEP_BEEP_TONE_B_DISABLE                                            15                               /* Output disable */
#define BITP_BEEP_BEEP_TONE_B_FREQ                                                8                               /* Tone frequency */
#define BITP_BEEP_BEEP_TONE_B_DUR                                                 0                               /* Tone duration */
#define BITM_BEEP_BEEP_TONE_B_DISABLE                                            (_ADI_MSK(0x00008000,uint16_t))  /* Output disable */
#define BITM_BEEP_BEEP_TONE_B_FREQ                                               (_ADI_MSK(0x00007F00,uint16_t))  /* Tone frequency */
#define BITM_BEEP_BEEP_TONE_B_DUR                                                (_ADI_MSK(0x000000FF,uint16_t))  /* Tone duration */

/* ==================================================
        Random Number Generator Registers
   ================================================== */

/* =========================
        RNG
   ========================= */
#define REG_RNG_RNGCTL                       0x40006000         /* RNG RNG Control Register */
#define REG_RNG_RNGLEN                       0x40006004         /* RNG RNG Sample Length Register */
#define REG_RNG_RNGSTAT                      0x40006008         /* RNG RNG Status Register */
#define REG_RNG_RNGDATA                      0x4000600C         /* RNG RNG Data Register */
#define REG_RNG_RNGCNTL                      0x40006010         /* RNG Oscillator Count Low */
#define REG_RNG_RNGCNTH                      0x40006014         /* RNG Oscillator Count High */

/* =========================
        RNG
   ========================= */
/* ------------------------------------------------------------------------------------------------------------------------
        RNG_RNGCTL                                                               Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_RNG_RNGCTL_TMRMODE                                                   2                               /* Timer Mode */
#define BITP_RNG_RNGCTL_CNTEN                                                     1                               /* Oscillator Counter Enable */
#define BITP_RNG_RNGCTL_RNGEN                                                     0                               /* RNG Enable */
#define BITM_RNG_RNGCTL_TMRMODE                                                  (_ADI_MSK(0x00000004,uint16_t))  /* Timer Mode */
#define BITM_RNG_RNGCTL_CNTEN                                                    (_ADI_MSK(0x00000002,uint16_t))  /* Oscillator Counter Enable */
#define BITM_RNG_RNGCTL_RNGEN                                                    (_ADI_MSK(0x00000001,uint16_t))  /* RNG Enable */

/* ------------------------------------------------------------------------------------------------------------------------
        RNG_RNGLEN                                                               Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_RNG_RNGLEN_LENPRE                                                   12                               /* Prescaler for the sample counter */
#define BITP_RNG_RNGLEN_LENRLD                                                    0                               /* Reload value for the sample counter */
#define BITM_RNG_RNGLEN_LENPRE                                                   (_ADI_MSK(0x0000F000,uint16_t))  /* Prescaler for the sample counter */
#define BITM_RNG_RNGLEN_LENRLD                                                   (_ADI_MSK(0x00000FFF,uint16_t))  /* Reload value for the sample counter */

/* ------------------------------------------------------------------------------------------------------------------------
        RNG_RNGSTAT                                                              Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_RNG_RNGSTAT_RNGRDY                                                   0                               /* Random number ready */
#define BITM_RNG_RNGSTAT_RNGRDY                                                  (_ADI_MSK(0x00000001,uint16_t))  /* Random number ready */

/* ------------------------------------------------------------------------------------------------------------------------
        RNG_RNGCNTH                                                              Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_RNG_RNGCNTH_RNGCNTH                                                  0                               /* Upper bits of oscillator count */
#define BITM_RNG_RNGCNTH_RNGCNTH                                                 (_ADI_MSK(0x00000FFF,uint16_t))  /* Upper bits of oscillator count */

/* ==================================================
        LCD Controller Registers
   ================================================== */

/* =========================
        LCD
   ========================= */
#define REG_LCD_LCDCON                       0x40008000         /* LCD LCD Configuration  Register */
#define REG_LCD_LCDSTAT                      0x40008004         /* LCD LCD Status Register */
#define REG_LCD_LCDBLINK                     0x40008008         /* LCD LCD Blink Control Register */
#define REG_LCD_LCDCONTRAST                  0x4000800C         /* LCD LCD Contrast Control Register */
#define REG_LCD_LCDDATA0_S0                  0x40008010         /* LCD Screen 0 LCD Data Register n */
#define REG_LCD_LCDDATA1_S0                  0x40008014         /* LCD Screen 0 LCD Data Register n */
#define REG_LCD_LCDDATA2_S0                  0x40008018         /* LCD Screen 0 LCD Data Register n */
#define REG_LCD_LCDDATA3_S0                  0x4000801C         /* LCD Screen 0 LCD Data Register n */
#define REG_LCD_LCDDATA4_S0                  0x40008020         /* LCD Screen 0 LCD Data Register n */
#define REG_LCD_LCDDATA5_S0                  0x40008024         /* LCD Screen 0 LCD Data Register n */
#define REG_LCD_LCDDATA6_S0                  0x40008028         /* LCD Screen 0 LCD Data Register n */
#define REG_LCD_LCDDATA7_S0                  0x4000802C         /* LCD Screen 0 LCD Data Register n */
#define REG_LCD_LCDDATA0_S1                  0x40008030         /* LCD Screen 1 LCD Data Register n */
#define REG_LCD_LCDDATA1_S1                  0x40008034         /* LCD Screen 1 LCD Data Register n */
#define REG_LCD_LCDDATA2_S1                  0x40008038         /* LCD Screen 1 LCD Data Register n */
#define REG_LCD_LCDDATA3_S1                  0x4000803C         /* LCD Screen 1 LCD Data Register n */
#define REG_LCD_LCDDATA4_S1                  0x40008040         /* LCD Screen 1 LCD Data Register n */
#define REG_LCD_LCDDATA5_S1                  0x40008044         /* LCD Screen 1 LCD Data Register n */
#define REG_LCD_LCDDATA6_S1                  0x40008048         /* LCD Screen 1 LCD Data Register n */
#define REG_LCD_LCDDATA7_S1                  0x4000804C         /* LCD Screen 1 LCD Data Register n */

/* =========================
        LCD
   ========================= */
/* ------------------------------------------------------------------------------------------------------------------------
        LCD_LCDCON                                                               Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_LCD_LCDCON_BLINKEN                                                  12                               /* Blink Mode Enable */
#define BITP_LCD_LCDCON_CPINT_EN                                                 11                               /* Enable Charge Pump interrupt */
#define BITP_LCD_LCDCON_FRAMEINT_EN                                              10                               /* Enable frame boundary interrupt */
#define BITP_LCD_LCDCON_LCDRST                                                    9                               /* LCD Data registers reset */
#define BITP_LCD_LCDCON_FRAMEINV                                                  8                               /* Frame inversion mode enable */
#define BITP_LCD_LCDCON_FRAMESEL                                                  4                               /* LCD frame rate select */
#define BITP_LCD_LCDCON_SCREENSEL                                                 3                               /* Screen Select */
#define BITP_LCD_LCDCON_LCDMUX                                                    1                               /* LCD Multiplex Value */
#define BITP_LCD_LCDCON_LCDEN                                                     0                               /* LCD Enable */
#define BITM_LCD_LCDCON_BLINKEN                                                  (_ADI_MSK(0x00001000,uint16_t))  /* Blink Mode Enable */
#define BITM_LCD_LCDCON_CPINT_EN                                                 (_ADI_MSK(0x00000800,uint16_t))  /* Enable Charge Pump interrupt */
#define BITM_LCD_LCDCON_FRAMEINT_EN                                              (_ADI_MSK(0x00000400,uint16_t))  /* Enable frame boundary interrupt */
#define BITM_LCD_LCDCON_LCDRST                                                   (_ADI_MSK(0x00000200,uint16_t))  /* LCD Data registers reset */
#define BITM_LCD_LCDCON_FRAMEINV                                                 (_ADI_MSK(0x00000100,uint16_t))  /* Frame inversion mode enable */
#define BITM_LCD_LCDCON_FRAMESEL                                                 (_ADI_MSK(0x000000F0,uint16_t))  /* LCD frame rate select */
#define BITM_LCD_LCDCON_SCREENSEL                                                (_ADI_MSK(0x00000008,uint16_t))  /* Screen Select */
#define BITM_LCD_LCDCON_LCDMUX                                                   (_ADI_MSK(0x00000002,uint16_t))  /* LCD Multiplex Value */
#define BITM_LCD_LCDCON_LCDEN                                                    (_ADI_MSK(0x00000001,uint16_t))  /* LCD Enable */

/* ------------------------------------------------------------------------------------------------------------------------
        LCD_LCDSTAT                                                              Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_LCD_LCDSTAT_LCD_IDLE                                                 4                               /* LCD IDLE state */
#define BITP_LCD_LCDSTAT_VLCD_OK                                                  3                               /* VLCD update complete */
#define BITP_LCD_LCDSTAT_CP_GD                                                    2                               /* Charge pump good */
#define BITP_LCD_LCDSTAT_SAFE_TO_WR                                               1                               /* Safe to write the registers */
#define BITP_LCD_LCDSTAT_FRAMEINT                                                 0                               /* LCD Frame boundary interrupt */
#define BITM_LCD_LCDSTAT_LCD_IDLE                                                (_ADI_MSK(0x00000010,uint16_t))  /* LCD IDLE state */
#define BITM_LCD_LCDSTAT_VLCD_OK                                                 (_ADI_MSK(0x00000008,uint16_t))  /* VLCD update complete */
#define BITM_LCD_LCDSTAT_CP_GD                                                   (_ADI_MSK(0x00000004,uint16_t))  /* Charge pump good */
#define BITM_LCD_LCDSTAT_SAFE_TO_WR                                              (_ADI_MSK(0x00000002,uint16_t))  /* Safe to write the registers */
#define BITM_LCD_LCDSTAT_FRAMEINT                                                (_ADI_MSK(0x00000001,uint16_t))  /* LCD Frame boundary interrupt */

/* ------------------------------------------------------------------------------------------------------------------------
        LCD_LCDBLINK                                                             Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_LCD_LCDBLINK_AUTOSWITCH                                              5                               /* Switch screen automatically */
#define BITP_LCD_LCDBLINK_BLKFREQ                                                 2                               /* Blink rate configuration bits */
#define BITP_LCD_LCDBLINK_BLKMOD                                                  0                               /* Blink mode clock source configuration bits */
#define BITM_LCD_LCDBLINK_AUTOSWITCH                                             (_ADI_MSK(0x00000020,uint16_t))  /* Switch screen automatically */
#define BITM_LCD_LCDBLINK_BLKFREQ                                                (_ADI_MSK(0x0000001C,uint16_t))  /* Blink rate configuration bits */
#define BITM_LCD_LCDBLINK_BLKMOD                                                 (_ADI_MSK(0x00000003,uint16_t))  /* Blink mode clock source configuration bits */

/* ------------------------------------------------------------------------------------------------------------------------
        LCD_LCDCONTRAST                                                          Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_LCD_LCDCONTRAST_CP_PD                                                6                               /* Charge pump power down */
#define BITP_LCD_LCDCONTRAST_CP_EN                                                5                               /* Charge pump enable */
#define BITP_LCD_LCDCONTRAST_BIASLVL                                              0                               /* Bias level selection */
#define BITM_LCD_LCDCONTRAST_CP_PD                                               (_ADI_MSK(0x00000040,uint16_t))  /* Charge pump power down */
#define BITM_LCD_LCDCONTRAST_CP_EN                                               (_ADI_MSK(0x00000020,uint16_t))  /* Charge pump enable */
#define BITM_LCD_LCDCONTRAST_BIASLVL                                             (_ADI_MSK(0x0000001F,uint16_t))  /* Bias level selection */

/* ==================================================
        analog test Registers
   ================================================== */

/* =========================
        AI
   ========================= */
/* ==================================================
        DMA Registers
   ================================================== */

/* =========================
        DMA
   ========================= */
#define REG_DMA_DMASTA                       0x40010000         /* DMA DMA Status */
#define REG_DMA_DMACFG                       0x40010004         /* DMA DMA Configuration */
#define REG_DMA_DMAPDBPTR                    0x40010008         /* DMA DMA channel primary control data base pointer */
#define REG_DMA_DMAADBPTR                    0x4001000C         /* DMA DMA channel alternate control data base pointer */
#define REG_DMA_DMASWREQ                     0x40010014         /* DMA DMA channel software request */
#define REG_DMA_DMARMSKSET                   0x40010020         /* DMA DMA channel request mask set */
#define REG_DMA_DMARMSKCLR                   0x40010024         /* DMA DMA channel request mask clear */
#define REG_DMA_DMAENSET                     0x40010028         /* DMA DMA channel enable set */
#define REG_DMA_DMAENCLR                     0x4001002C         /* DMA DMA channel enable clear */
#define REG_DMA_DMAALTSET                    0x40010030         /* DMA DMA channel primary-alternate set */
#define REG_DMA_DMAALTCLR                    0x40010034         /* DMA DMA channel primary-alternate clear */
#define REG_DMA_DMAPRISET                    0x40010038         /* DMA DMA channel priority set */
#define REG_DMA_DMAPRICLR                    0x4001003C         /* DMA DMA channel priority clear */
#define REG_DMA_DMAERRCHNLCLR                0x40010048         /* DMA DMA Per Channel Error Clear */
#define REG_DMA_DMAERRCLR                    0x4001004C         /* DMA DMA bus error clear */
#define REG_DMA_DMAINVALIDDESCCLR            0x40010050         /* DMA DMA Per Channel Invalid Descriptor Clear */
#define REG_DMA_DMABSSET                     0x40010800         /* DMA DMA channel bytes swap enable set */
#define REG_DMA_DMABSCLR                     0x40010804         /* DMA DMA channel bytes swap enable clear */
#define REG_DMA_DMASRCADSSET                 0x40010810         /* DMA DMA channel source address decrement enable set */
#define REG_DMA_DMASRCADCLR                  0x40010814         /* DMA DMA channel source address decrement enable clear */
#define REG_DMA_DMADSTADSET                  0x40010818         /* DMA DMA channel destination address decrement enable set */
#define REG_DMA_DMADSTADCLR                  0x4001081C         /* DMA DMA channel destination address decrement enable clear */
#define REG_DMA_DMAREVID                     0x40010FE0         /* DMA DMA Controller Revision ID */

/* =========================
        DMA
   ========================= */
/* ------------------------------------------------------------------------------------------------------------------------
        DMA_DMASTA                                                               Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_DMA_DMASTA_CHNLSM1                                                  16                               /* Number of available DMA channels minus 1 */
#define BITP_DMA_DMASTA_STATE                                                     4                               /* Current state of DMA controller */
#define BITP_DMA_DMASTA_MENABLE                                                   0                               /* Enable status of the controller */
#define BITM_DMA_DMASTA_CHNLSM1                                                  (_ADI_MSK(0x001F0000,uint32_t))  /* Number of available DMA channels minus 1 */
#define BITM_DMA_DMASTA_STATE                                                    (_ADI_MSK(0x000000F0,uint32_t))  /* Current state of DMA controller */
#define BITM_DMA_DMASTA_MENABLE                                                  (_ADI_MSK(0x00000001,uint32_t))  /* Enable status of the controller */

/* ------------------------------------------------------------------------------------------------------------------------
        DMA_DMACFG                                                               Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_DMA_DMACFG_MENABLE                                                   0                               /* Controller enable */
#define BITM_DMA_DMACFG_MENABLE                                                  (_ADI_MSK(0x00000001,uint32_t))  /* Controller enable */

/* ------------------------------------------------------------------------------------------------------------------------
        DMA_DMASWREQ                                                             Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_DMA_DMASWREQ_CHSWREQ                                                 0                               /* Generate software request */
#define BITM_DMA_DMASWREQ_CHSWREQ                                                (_ADI_MSK(0x0000FFFF,uint32_t))  /* Generate software request */

/* ------------------------------------------------------------------------------------------------------------------------
        DMA_DMARMSKSET                                                           Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_DMA_DMARMSKSET_CHREQMSET                                             0                               /* Mask requests from DMA channels */
#define BITM_DMA_DMARMSKSET_CHREQMSET                                            (_ADI_MSK(0x0000FFFF,uint32_t))  /* Mask requests from DMA channels */

/* ------------------------------------------------------------------------------------------------------------------------
        DMA_DMARMSKCLR                                                           Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_DMA_DMARMSKCLR_CHREQMCLR                                             0                               /* Clear REQ_MASK_SET bits in DMARMSKSET */
#define BITM_DMA_DMARMSKCLR_CHREQMCLR                                            (_ADI_MSK(0x0000FFFF,uint32_t))  /* Clear REQ_MASK_SET bits in DMARMSKSET */

/* ------------------------------------------------------------------------------------------------------------------------
        DMA_DMAENSET                                                             Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_DMA_DMAENSET_CHENSET                                                 0                               /* Enable DMA channels */
#define BITM_DMA_DMAENSET_CHENSET                                                (_ADI_MSK(0x0000FFFF,uint32_t))  /* Enable DMA channels */

/* ------------------------------------------------------------------------------------------------------------------------
        DMA_DMAENCLR                                                             Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_DMA_DMAENCLR_CHENCLR                                                 0                               /* Disable DMA channels */
#define BITM_DMA_DMAENCLR_CHENCLR                                                (_ADI_MSK(0x0000FFFF,uint32_t))  /* Disable DMA channels */

/* ------------------------------------------------------------------------------------------------------------------------
        DMA_DMAALTSET                                                            Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_DMA_DMAALTSET_CHPRIALTSET                                            0                               /* Control struct status / select alt struct */
#define BITM_DMA_DMAALTSET_CHPRIALTSET                                           (_ADI_MSK(0x0000FFFF,uint32_t))  /* Control struct status / select alt struct */

/* ------------------------------------------------------------------------------------------------------------------------
        DMA_DMAALTCLR                                                            Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_DMA_DMAALTCLR_CHPRIALTCLR                                            0                               /* Select primary data struct */
#define BITM_DMA_DMAALTCLR_CHPRIALTCLR                                           (_ADI_MSK(0x0000FFFF,uint32_t))  /* Select primary data struct */

/* ------------------------------------------------------------------------------------------------------------------------
        DMA_DMAPRISET                                                            Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_DMA_DMAPRISET_CHPRISET                                               0                               /* Configure channel for high priority */
#define BITM_DMA_DMAPRISET_CHPRISET                                              (_ADI_MSK(0x0000FFFF,uint32_t))  /* Configure channel for high priority */

/* ------------------------------------------------------------------------------------------------------------------------
        DMA_DMAPRICLR                                                            Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_DMA_DMAPRICLR_CHPRICLR                                               0                               /* Configure channel for default priority level */
#define BITM_DMA_DMAPRICLR_CHPRICLR                                              (_ADI_MSK(0x0000FFFF,uint32_t))  /* Configure channel for default priority level */

/* ------------------------------------------------------------------------------------------------------------------------
        DMA_DMAERRCHNLCLR                                                        Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_DMA_DMAERRCHNLCLR_CHNL_ERR_CLR                                       0                               /* Per channel Bus error status/ Per channel bus error clear */
#define BITM_DMA_DMAERRCHNLCLR_CHNL_ERR_CLR                                      (_ADI_MSK(0x0000FFFF,uint32_t))  /* Per channel Bus error status/ Per channel bus error clear */

/* ------------------------------------------------------------------------------------------------------------------------
        DMA_DMAERRCLR                                                            Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_DMA_DMAERRCLR_ERRCLR                                                 0                               /* Bus error status */
#define BITM_DMA_DMAERRCLR_ERRCLR                                                (_ADI_MSK(0x0000FFFF,uint32_t))  /* Bus error status */

/* ------------------------------------------------------------------------------------------------------------------------
        DMA_DMAINVALIDDESCCLR                                                    Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_DMA_DMAINVALIDDESCCLR_CHNL_INVALID_CLR                               0                               /* Per channel Invalid Descriptor status/ Per channel Invalid descriptor status clear */
#define BITM_DMA_DMAINVALIDDESCCLR_CHNL_INVALID_CLR                              (_ADI_MSK(0x0000FFFF,uint32_t))  /* Per channel Invalid Descriptor status/ Per channel Invalid descriptor status clear */

/* ------------------------------------------------------------------------------------------------------------------------
        DMA_DMABSSET                                                             Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_DMA_DMABSSET_CHBSWAPSET                                              0                               /* Byte swap status */
#define BITM_DMA_DMABSSET_CHBSWAPSET                                             (_ADI_MSK(0x0000FFFF,uint32_t))  /* Byte swap status */

/* ------------------------------------------------------------------------------------------------------------------------
        DMA_DMABSCLR                                                             Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_DMA_DMABSCLR_CHBSWAPCLR                                              0                               /* Disable byte swap */
#define BITM_DMA_DMABSCLR_CHBSWAPCLR                                             (_ADI_MSK(0x0000FFFF,uint32_t))  /* Disable byte swap */

/* ------------------------------------------------------------------------------------------------------------------------
        DMA_DMASRCADSSET                                                         Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_DMA_DMASRCADSSET_CHSRCADRDECSET                                      0                               /* Source Address decrement status / configure Source address decrement */
#define BITM_DMA_DMASRCADSSET_CHSRCADRDECSET                                     (_ADI_MSK(0x0000FFFF,uint32_t))  /* Source Address decrement status / configure Source address decrement */

/* ------------------------------------------------------------------------------------------------------------------------
        DMA_DMASRCADCLR                                                          Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_DMA_DMASRCADCLR_CHADRDECCLR                                          0                               /* Disable source address decrement */
#define BITM_DMA_DMASRCADCLR_CHADRDECCLR                                         (_ADI_MSK(0x0000FFFF,uint32_t))  /* Disable source address decrement */

/* ------------------------------------------------------------------------------------------------------------------------
        DMA_DMADSTADSET                                                          Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_DMA_DMADSTADSET_CHDSTADRDECSET                                       0                               /* Destination Address decrement status / configure destination address decrement */
#define BITM_DMA_DMADSTADSET_CHDSTADRDECSET                                      (_ADI_MSK(0x0000FFFF,uint32_t))  /* Destination Address decrement status / configure destination address decrement */

/* ------------------------------------------------------------------------------------------------------------------------
        DMA_DMADSTADCLR                                                          Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_DMA_DMADSTADCLR_CHADRDECCLR                                          0                               /* Disable destination address decrement */
#define BITM_DMA_DMADSTADCLR_CHADRDECCLR                                         (_ADI_MSK(0x0000FFFF,uint32_t))  /* Disable destination address decrement */

/* ------------------------------------------------------------------------------------------------------------------------
        DMA_DMAREVID                                                             Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_DMA_DMAREVID_DMAREVID                                                0                               /* DMA Controller revision ID */
#define BITM_DMA_DMAREVID_DMAREVID                                               (_ADI_MSK(0x000000FF,uint32_t))  /* DMA Controller revision ID */

/* ==================================================
        Flash Controller1 Registers
   ================================================== */

/* =========================
        FEE0
   ========================= */
#define REG_FEE0_FEESTA                      0x40018000         /* FEE0 Status */
#define REG_FEE0_FEECON0                     0x40018004         /* FEE0 Command Control */
#define REG_FEE0_FEECMD                      0x40018008         /* FEE0 Command */
#define REG_FEE0_FEEADR0L                    0x40018010         /* FEE0 Lower page address */
#define REG_FEE0_FEEADR0H                    0x40018014         /* FEE0 Upper page address */
#define REG_FEE0_FEEADR1L                    0x40018018         /* FEE0 Lower page address */
#define REG_FEE0_FEEADR1H                    0x4001801C         /* FEE0 Upper page address */
#define REG_FEE0_FEEKEY                      0x40018020         /* FEE0 Key */
#define REG_FEE0_FEESIGL                     0x40018030         /* FEE0 Lower halfword of signature */
#define REG_FEE0_FEESIGH                     0x40018034         /* FEE0 Upper halfword of signature */
#define REG_FEE0_FEEADRAL                    0x40018048         /* FEE0 Lower halfword of write abort address */
#define REG_FEE0_FEEADRAH                    0x4001804C         /* FEE0 Upper halfword of write abort address */
#define REG_FEE0_FEEPARCTL                   0x40018050         /* FEE0 Parity Control Register */
#define REG_FEE0_FEEPARSTA                   0x40018054         /* FEE0 Parity Status Register */
#define REG_FEE0_FEEPARADRL                  0x40018058         /* FEE0 Parity Error Address Low */
#define REG_FEE0_FEEPARADRH                  0x4001805C         /* FEE0 Parity Error Address High */
#define REG_FEE0_FEEAEN0                     0x40018078         /* FEE0 System IRQ abort enable for interrupts 15 to 0 */
#define REG_FEE0_FEEAEN1                     0x4001807C         /* FEE0 System IRQ abort enable for interrupts 31 to 16 */
#define REG_FEE0_FEEAEN2                     0x40018080         /* FEE0 System IRQ abort enable for interrupts 47 to 32 */
#define REG_FEE0_FEEAEN3                     0x40018084         /* FEE0 System IRQ abort enable for interrupts 60 to 48 */

/* =========================
        FEE1
   ========================= */
#define REG_FEE1_FEESTA                      0x40018100         /* FEE1 Status */
#define REG_FEE1_FEECON0                     0x40018104         /* FEE1 Command Control */
#define REG_FEE1_FEECMD                      0x40018108         /* FEE1 Command */
#define REG_FEE1_FEEADR0L                    0x40018110         /* FEE1 Lower page address */
#define REG_FEE1_FEEADR0H                    0x40018114         /* FEE1 Upper page address */
#define REG_FEE1_FEEADR1L                    0x40018118         /* FEE1 Lower page address */
#define REG_FEE1_FEEADR1H                    0x4001811C         /* FEE1 Upper page address */
#define REG_FEE1_FEEKEY                      0x40018120         /* FEE1 Key */
#define REG_FEE1_FEEPROL                     0x40018128         /* FEE1 Lower halfword of write protection */
#define REG_FEE1_FEEPROH                     0x4001812C         /* FEE1 Upper halfword of write protection */
#define REG_FEE1_FEESIGL                     0x40018130         /* FEE1 Lower halfword of signature */
#define REG_FEE1_FEESIGH                     0x40018134         /* FEE1 Upper halfword of signature */
#define REG_FEE1_FEECON1                     0x40018138         /* FEE1 User Setup */
#define REG_FEE1_FEEADRAL                    0x40018148         /* FEE1 Lower halfword of write abort address */
#define REG_FEE1_FEEADRAH                    0x4001814C         /* FEE1 Upper halfword of write abort address */
#define REG_FEE1_FEEPARCTL                   0x40018150         /* FEE1 Parity Control Register */
#define REG_FEE1_FEEPARSTA                   0x40018154         /* FEE1 Parity Status Register */
#define REG_FEE1_FEEPARADRL                  0x40018158         /* FEE1 Parity Error Address Low */
#define REG_FEE1_FEEPARADRH                  0x4001815C         /* FEE1 Parity Error Address High */
#define REG_FEE1_FEEAEN0                     0x40018178         /* FEE1 System IRQ abort enable for interrupts 15 to 0 */
#define REG_FEE1_FEEAEN1                     0x4001817C         /* FEE1 System IRQ abort enable for interrupts 31 to 16 */
#define REG_FEE1_FEEAEN2                     0x40018180         /* FEE1 System IRQ abort enable for interrupts 47 to 32 */
#define REG_FEE1_FEEAEN3                     0x40018184         /* FEE1 System IRQ abort enable for interrupts 60 to 48 */

/* =========================
        FEE
   ========================= */
/* ------------------------------------------------------------------------------------------------------------------------
        FEE_FEESTA                                                               Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_FEE_FEESTA_INIT                                                      7                               /* Initialization upload in progress */
#define BITP_FEE_FEESTA_SIGNERR                                                   6                               /* Info spec signature check */
#define BITP_FEE_FEESTA_CMDRES                                                    4                               /* Command result */
#define BITP_FEE_FEESTA_WRDONE                                                    3                               /* Write complete */
#define BITP_FEE_FEESTA_CMDDONE                                                   2                               /* Command complete */
#define BITP_FEE_FEESTA_WRBUSY                                                    1                               /* Write busy */
#define BITP_FEE_FEESTA_CMDBUSY                                                   0                               /* Command busy */
#define BITM_FEE_FEESTA_INIT                                                     (_ADI_MSK(0x00000080,uint16_t))  /* Initialization upload in progress */
#define BITM_FEE_FEESTA_SIGNERR                                                  (_ADI_MSK(0x00000040,uint16_t))  /* Info spec signature check */
#define BITM_FEE_FEESTA_CMDRES                                                   (_ADI_MSK(0x00000030,uint16_t))  /* Command result */
#define BITM_FEE_FEESTA_WRDONE                                                   (_ADI_MSK(0x00000008,uint16_t))  /* Write complete */
#define BITM_FEE_FEESTA_CMDDONE                                                  (_ADI_MSK(0x00000004,uint16_t))  /* Command complete */
#define BITM_FEE_FEESTA_WRBUSY                                                   (_ADI_MSK(0x00000002,uint16_t))  /* Write busy */
#define BITM_FEE_FEESTA_CMDBUSY                                                  (_ADI_MSK(0x00000001,uint16_t))  /* Command busy */

/* ------------------------------------------------------------------------------------------------------------------------
        FEE_FEECON0                                                              Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_FEE_FEECON0_WREN                                                     2                               /* Write enable */
#define BITP_FEE_FEECON0_IENERR                                                   1                               /* Error interrupt enable */
#define BITP_FEE_FEECON0_IENCMD                                                   0                               /* Command complete interrupt enable */
#define BITM_FEE_FEECON0_WREN                                                    (_ADI_MSK(0x00000004,uint16_t))  /* Write enable */
#define BITM_FEE_FEECON0_IENERR                                                  (_ADI_MSK(0x00000002,uint16_t))  /* Error interrupt enable */
#define BITM_FEE_FEECON0_IENCMD                                                  (_ADI_MSK(0x00000001,uint16_t))  /* Command complete interrupt enable */

/* ------------------------------------------------------------------------------------------------------------------------
        FEE_FEECMD                                                               Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_FEE_FEECMD_CMD                                                       0
#define BITM_FEE_FEECMD_CMD                                                      (_ADI_MSK(0x0000000F,uint16_t))

/* ------------------------------------------------------------------------------------------------------------------------
        FEE_FEEADR0L                                                             Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_FEE_FEEADR0L_LOW                                                    11                               /* Lower 5 bits of page address */
#define BITM_FEE_FEEADR0L_LOW                                                    (_ADI_MSK(0x0000F800,uint16_t))  /* Lower 5 bits of page address */

/* ------------------------------------------------------------------------------------------------------------------------
        FEE_FEEADR0H                                                             Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_FEE_FEEADR0H_HIGH                                                    0                               /* Upper bits of page address */
#define BITM_FEE_FEEADR0H_HIGH                                                   (_ADI_MSK(0x00000003,uint16_t))  /* Upper bits of page address */

/* ------------------------------------------------------------------------------------------------------------------------
        FEE_FEEADR1L                                                             Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_FEE_FEEADR1L_LOW                                                    11                               /* Lower 5 bits of page address */
#define BITM_FEE_FEEADR1L_LOW                                                    (_ADI_MSK(0x0000F800,uint16_t))  /* Lower 5 bits of page address */

/* ------------------------------------------------------------------------------------------------------------------------
        FEE_FEEADR1H                                                             Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_FEE_FEEADR1H_HIGH                                                    0                               /* Upper bits of page address */
#define BITM_FEE_FEEADR1H_HIGH                                                   (_ADI_MSK(0x00000003,uint16_t))  /* Upper bits of page address */

/* ------------------------------------------------------------------------------------------------------------------------
        FEE_FEECON1                                                              Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_FEE_FEECON1_DBG                                                      0                               /* Serial wire debug enable */
#define BITM_FEE_FEECON1_DBG                                                     (_ADI_MSK(0x00000001,uint16_t))  /* Serial wire debug enable */

/* ------------------------------------------------------------------------------------------------------------------------
        FEE_FEEPARCTL                                                            Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_FEE_FEEPARCTL_PERREXEN                                               1                               /* Parity Error Interrupt Enable */
#define BITP_FEE_FEEPARCTL_PAREN                                                  0                               /* Parity Enable */
#define BITM_FEE_FEEPARCTL_PERREXEN                                              (_ADI_MSK(0x00000002,uint16_t))  /* Parity Error Interrupt Enable */
#define BITM_FEE_FEEPARCTL_PAREN                                                 (_ADI_MSK(0x00000001,uint16_t))  /* Parity Enable */

/* ------------------------------------------------------------------------------------------------------------------------
        FEE_FEEPARSTA                                                            Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_FEE_FEEPARSTA_PARERR                                                 0                               /* Parity Error */
#define BITM_FEE_FEEPARSTA_PARERR                                                (_ADI_MSK(0x00000001,uint16_t))  /* Parity Error */

/* ------------------------------------------------------------------------------------------------------------------------
        FEE_FEEAEN3                                                              Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_FEE_FEEAEN3_FEE1AEN3                                                 0                               /* System IRQ abort enable for interrupts 60 to 48 */
#define BITM_FEE_FEEAEN3_FEE1AEN3                                                (_ADI_MSK(0x00001FFF,uint16_t))  /* System IRQ abort enable for interrupts 60 to 48 */

/* ==================================================
        General Purpose Flash Controller Registers
   ================================================== */

/* =========================
        GPF
   ========================= */
#define REG_GPF_GPFEESTA                     0x4001C000         /* GPF Status */
#define REG_GPF_GPFEECON0                    0x4001C004         /* GPF Command Control */
#define REG_GPF_GPFEECMD                     0x4001C008         /* GPF Command */
#define REG_GPF_GPFEEADR0L                   0x4001C010         /* GPF Lower page address */
#define REG_GPF_GPFEEADR1L                   0x4001C018         /* GPF Lower page address */
#define REG_GPF_GPFEEKEY                     0x4001C020         /* GPF Key */
#define REG_GPF_GPFEESIGL                    0x4001C030         /* GPF Lower halfword of signature */
#define REG_GPF_GPFEESIGH                    0x4001C034         /* GPF Upper halfword of signature */
#define REG_GPF_GPFEEADRAL                   0x4001C048         /* GPF Lower halfword of write abort address */
#define REG_GPF_GPFEEADRAH                   0x4001C04C         /* GPF Upper halfword of write abort address */
#define REG_GPF_GPFEEAEN0                    0x4001C078         /* GPF System IRQ abort enable for interrupts 15 to 0 */
#define REG_GPF_GPFEEAEN1                    0x4001C07C         /* GPF System IRQ abort enable for interrupts 31 to 16 */
#define REG_GPF_GPFEEAEN2                    0x4001C080         /* GPF System IRQ abort enable for interrupts 47 to 32 */
#define REG_GPF_GPFEEAEN3                    0x4001C084         /* GPF System IRQ abort enable for interrupts 60 to 48 */

/* =========================
        GPF
   ========================= */
/* ------------------------------------------------------------------------------------------------------------------------
        GPF_GPFEESTA                                                             Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_GPF_GPFEESTA_CMDRES                                                  4                               /* Command result */
#define BITP_GPF_GPFEESTA_WRDONE                                                  3                               /* Write complete */
#define BITP_GPF_GPFEESTA_CMDDONE                                                 2                               /* Command complete */
#define BITP_GPF_GPFEESTA_WRBUSY                                                  1                               /* Write busy */
#define BITP_GPF_GPFEESTA_CMDBUSY                                                 0                               /* Command busy */
#define BITM_GPF_GPFEESTA_CMDRES                                                 (_ADI_MSK(0x00000030,uint16_t))  /* Command result */
#define BITM_GPF_GPFEESTA_WRDONE                                                 (_ADI_MSK(0x00000008,uint16_t))  /* Write complete */
#define BITM_GPF_GPFEESTA_CMDDONE                                                (_ADI_MSK(0x00000004,uint16_t))  /* Command complete */
#define BITM_GPF_GPFEESTA_WRBUSY                                                 (_ADI_MSK(0x00000002,uint16_t))  /* Write busy */
#define BITM_GPF_GPFEESTA_CMDBUSY                                                (_ADI_MSK(0x00000001,uint16_t))  /* Command busy */

/* ------------------------------------------------------------------------------------------------------------------------
        GPF_GPFEECON0                                                            Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_GPF_GPFEECON0_DMA_EN                                                 3                               /* Enable DMA interface feature */
#define BITP_GPF_GPFEECON0_WREN                                                   2                               /* Write enable */
#define BITP_GPF_GPFEECON0_IENERR                                                 1                               /* Error interrupt enable */
#define BITP_GPF_GPFEECON0_IENCMD                                                 0                               /* Command complete interrupt enable */
#define BITM_GPF_GPFEECON0_DMA_EN                                                (_ADI_MSK(0x00000008,uint16_t))  /* Enable DMA interface feature */
#define BITM_GPF_GPFEECON0_WREN                                                  (_ADI_MSK(0x00000004,uint16_t))  /* Write enable */
#define BITM_GPF_GPFEECON0_IENERR                                                (_ADI_MSK(0x00000002,uint16_t))  /* Error interrupt enable */
#define BITM_GPF_GPFEECON0_IENCMD                                                (_ADI_MSK(0x00000001,uint16_t))  /* Command complete interrupt enable */

/* ------------------------------------------------------------------------------------------------------------------------
        GPF_GPFEECMD                                                             Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_GPF_GPFEECMD_CMD                                                     0
#define BITM_GPF_GPFEECMD_CMD                                                    (_ADI_MSK(0x0000000F,uint16_t))

/* ------------------------------------------------------------------------------------------------------------------------
        GPF_GPFEEADR0L                                                           Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_GPF_GPFEEADR0L_LOW                                                   9                               /* Lower 5 bits of page address */
#define BITM_GPF_GPFEEADR0L_LOW                                                  (_ADI_MSK(0x00003E00,uint16_t))  /* Lower 5 bits of page address */

/* ------------------------------------------------------------------------------------------------------------------------
        GPF_GPFEEADR1L                                                           Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_GPF_GPFEEADR1L_LOW                                                   9                               /* Lower 5 bits of page address */
#define BITM_GPF_GPFEEADR1L_LOW                                                  (_ADI_MSK(0x00003E00,uint16_t))  /* Lower 5 bits of page address */

/* ------------------------------------------------------------------------------------------------------------------------
        GPF_GPFEEAEN3                                                            Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_GPF_GPFEEAEN3_GPFEEAEN3                                              0                               /* System IRQ abort enable for interrupts 60 to 48 */
#define BITM_GPF_GPFEEAEN3_GPFEEAEN3                                             (_ADI_MSK(0x00001FFF,uint16_t))  /* System IRQ abort enable for interrupts 60 to 48 */

/* ==================================================
        GPIO Registers
   ================================================== */

/* =========================
        GPIO0
   ========================= */
#define REG_GPIO0_GPCON                      0x40020000         /* GPIO0 GPIO Port 0  Configuration */
#define REG_GPIO0_GPOEN                      0x40020004         /* GPIO0 GPIO Port 0 output enable */
#define REG_GPIO0_GPPE                       0x40020008         /* GPIO0 GPIO Port 0 output pullup/pulldown enable */
#define REG_GPIO0_GPIEN                      0x4002000C         /* GPIO0 GPIO  Port 0 Input Path Enable */
#define REG_GPIO0_GPIN                       0x40020010         /* GPIO0 GPIO  Port 0 registered data input */
#define REG_GPIO0_GPOUT                      0x40020014         /* GPIO0 GPIO Port 0 data output */
#define REG_GPIO0_GPSET                      0x40020018         /* GPIO0 GPIO Port 0 data out set */
#define REG_GPIO0_GPCLR                      0x4002001C         /* GPIO0 GPIO Port 0 data out clear */
#define REG_GPIO0_GPTGL                      0x40020020         /* GPIO0 GPIO Port 0  pin toggle */
#define REG_GPIO0_GPPOL                      0x40020024         /* GPIO0 GPIO Port 0 interrupt polarity */
#define REG_GPIO0_GPIENA                     0x40020028         /* GPIO0 GPIO Port 0  interrupt A enable */
#define REG_GPIO0_GPIENB                     0x4002002C         /* GPIO0 GPIO Port 0 interrupt B enable */
#define REG_GPIO0_GPINT                      0x40020030         /* GPIO0 GPIO Port 0 interrupt Status */

/* =========================
        GPIO1
   ========================= */
#define REG_GPIO1_GPCON                      0x40020040         /* GPIO1 GPIO Port 0  Configuration */
#define REG_GPIO1_GPOEN                      0x40020044         /* GPIO1 GPIO Port 0 output enable */
#define REG_GPIO1_GPPE                       0x40020048         /* GPIO1 GPIO Port 0 output pullup/pulldown enable */
#define REG_GPIO1_GPIEN                      0x4002004C         /* GPIO1 GPIO  Port 0 Input Path Enable */
#define REG_GPIO1_GPIN                       0x40020050         /* GPIO1 GPIO  Port 0 registered data input */
#define REG_GPIO1_GPOUT                      0x40020054         /* GPIO1 GPIO Port 0 data output */
#define REG_GPIO1_GPSET                      0x40020058         /* GPIO1 GPIO Port 0 data out set */
#define REG_GPIO1_GPCLR                      0x4002005C         /* GPIO1 GPIO Port 0 data out clear */
#define REG_GPIO1_GPTGL                      0x40020060         /* GPIO1 GPIO Port 0  pin toggle */
#define REG_GPIO1_GPPOL                      0x40020064         /* GPIO1 GPIO Port 0 interrupt polarity */
#define REG_GPIO1_GPIENA                     0x40020068         /* GPIO1 GPIO Port 0  interrupt A enable */
#define REG_GPIO1_GPIENB                     0x4002006C         /* GPIO1 GPIO Port 0 interrupt B enable */
#define REG_GPIO1_GPINT                      0x40020070         /* GPIO1 GPIO Port 0 interrupt Status */

/* =========================
        GPIO2
   ========================= */
#define REG_GPIO2_GPCON                      0x40020080         /* GPIO2 GPIO Port 0  Configuration */
#define REG_GPIO2_GPOEN                      0x40020084         /* GPIO2 GPIO Port 0 output enable */
#define REG_GPIO2_GPPE                       0x40020088         /* GPIO2 GPIO Port 0 output pullup/pulldown enable */
#define REG_GPIO2_GPIEN                      0x4002008C         /* GPIO2 GPIO  Port 0 Input Path Enable */
#define REG_GPIO2_GPIN                       0x40020090         /* GPIO2 GPIO  Port 0 registered data input */
#define REG_GPIO2_GPOUT                      0x40020094         /* GPIO2 GPIO Port 0 data output */
#define REG_GPIO2_GPSET                      0x40020098         /* GPIO2 GPIO Port 0 data out set */
#define REG_GPIO2_GPCLR                      0x4002009C         /* GPIO2 GPIO Port 0 data out clear */
#define REG_GPIO2_GPTGL                      0x400200A0         /* GPIO2 GPIO Port 0  pin toggle */
#define REG_GPIO2_GPPOL                      0x400200A4         /* GPIO2 GPIO Port 0 interrupt polarity */
#define REG_GPIO2_GPIENA                     0x400200A8         /* GPIO2 GPIO Port 0  interrupt A enable */
#define REG_GPIO2_GPIENB                     0x400200AC         /* GPIO2 GPIO Port 0 interrupt B enable */
#define REG_GPIO2_GPINT                      0x400200B0         /* GPIO2 GPIO Port 0 interrupt Status */

/* =========================
        GPIO3
   ========================= */
#define REG_GPIO3_GPCON                      0x400200C0         /* GPIO3 GPIO Port 0  Configuration */
#define REG_GPIO3_GPOEN                      0x400200C4         /* GPIO3 GPIO Port 0 output enable */
#define REG_GPIO3_GPPE                       0x400200C8         /* GPIO3 GPIO Port 0 output pullup/pulldown enable */
#define REG_GPIO3_GPIEN                      0x400200CC         /* GPIO3 GPIO  Port 0 Input Path Enable */
#define REG_GPIO3_GPIN                       0x400200D0         /* GPIO3 GPIO  Port 0 registered data input */
#define REG_GPIO3_GPOUT                      0x400200D4         /* GPIO3 GPIO Port 0 data output */
#define REG_GPIO3_GPSET                      0x400200D8         /* GPIO3 GPIO Port 0 data out set */
#define REG_GPIO3_GPCLR                      0x400200DC         /* GPIO3 GPIO Port 0 data out clear */
#define REG_GPIO3_GPTGL                      0x400200E0         /* GPIO3 GPIO Port 0  pin toggle */
#define REG_GPIO3_GPPOL                      0x400200E4         /* GPIO3 GPIO Port 0 interrupt polarity */
#define REG_GPIO3_GPIENA                     0x400200E8         /* GPIO3 GPIO Port 0  interrupt A enable */
#define REG_GPIO3_GPIENB                     0x400200EC         /* GPIO3 GPIO Port 0 interrupt B enable */
#define REG_GPIO3_GPINT                      0x400200F0         /* GPIO3 GPIO Port 0 interrupt Status */

/* =========================
        GPIO4
   ========================= */
#define REG_GPIO4_GPCON                      0x40020100         /* GPIO4 GPIO Port 0  Configuration */
#define REG_GPIO4_GPOEN                      0x40020104         /* GPIO4 GPIO Port 0 output enable */
#define REG_GPIO4_GPPE                       0x40020108         /* GPIO4 GPIO Port 0 output pullup/pulldown enable */
#define REG_GPIO4_GPIEN                      0x4002010C         /* GPIO4 GPIO  Port 0 Input Path Enable */
#define REG_GPIO4_GPIN                       0x40020110         /* GPIO4 GPIO  Port 0 registered data input */
#define REG_GPIO4_GPOUT                      0x40020114         /* GPIO4 GPIO Port 0 data output */
#define REG_GPIO4_GPSET                      0x40020118         /* GPIO4 GPIO Port 0 data out set */
#define REG_GPIO4_GPCLR                      0x4002011C         /* GPIO4 GPIO Port 0 data out clear */
#define REG_GPIO4_GPTGL                      0x40020120         /* GPIO4 GPIO Port 0  pin toggle */
#define REG_GPIO4_GPPOL                      0x40020124         /* GPIO4 GPIO Port 0 interrupt polarity */
#define REG_GPIO4_GPIENA                     0x40020128         /* GPIO4 GPIO Port 0  interrupt A enable */
#define REG_GPIO4_GPIENB                     0x4002012C         /* GPIO4 GPIO Port 0 interrupt B enable */
#define REG_GPIO4_GPINT                      0x40020130         /* GPIO4 GPIO Port 0 interrupt Status */

/* =========================
        GPIO
   ========================= */
/* ------------------------------------------------------------------------------------------------------------------------
        GPIO_GPCON                                                               Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_GPIO_GPCON_PIN15_CFG                                                30                               /* p0.15  configuration bits */
#define BITP_GPIO_GPCON_PIN14_CFG                                                28                               /* p0.14  configuration bits */
#define BITP_GPIO_GPCON_PIN13_CFG                                                26                               /* p0.13  configuration bits */
#define BITP_GPIO_GPCON_PIN12_CFG                                                24                               /* p0.12  configuration bits */
#define BITP_GPIO_GPCON_PIN11_CFG                                                22                               /* p0.11  configuration bits */
#define BITP_GPIO_GPCON_PIN10_CFG                                                20                               /* p0.10  configuration bits */
#define BITP_GPIO_GPCON_PIN9_CFG                                                 18                               /* p0.9  configuration bits */
#define BITP_GPIO_GPCON_PIN8_CFG                                                 16                               /* p0.8  configuration bits */
#define BITP_GPIO_GPCON_PIN7_CFG                                                 14                               /* p0.7  configuration bits */
#define BITP_GPIO_GPCON_PIN6_CFG                                                 12                               /* p0.6  configuration bits */
#define BITP_GPIO_GPCON_PIN5_CFG                                                 10                               /* p0.5  configuration bits */
#define BITP_GPIO_GPCON_PIN4_CFG                                                  8                               /* p0.4  configuration bits */
#define BITP_GPIO_GPCON_PIN3_CFG                                                  6                               /* p0.3  configuration bits */
#define BITP_GPIO_GPCON_PIN2_CFG                                                  4                               /* p0.2  configuration bits */
#define BITP_GPIO_GPCON_PIN1_CFG                                                  2                               /* p0.1  configuration bits */
#define BITP_GPIO_GPCON_PIN0_CFG                                                  0                               /* p0.0  configuration bits */
#define BITM_GPIO_GPCON_PIN15_CFG                                                (_ADI_MSK(0xC0000000,uint32_t))  /* p0.15  configuration bits */
#define BITM_GPIO_GPCON_PIN14_CFG                                                (_ADI_MSK(0x30000000,uint32_t))  /* p0.14  configuration bits */
#define BITM_GPIO_GPCON_PIN13_CFG                                                (_ADI_MSK(0x0C000000,uint32_t))  /* p0.13  configuration bits */
#define BITM_GPIO_GPCON_PIN12_CFG                                                (_ADI_MSK(0x03000000,uint32_t))  /* p0.12  configuration bits */
#define BITM_GPIO_GPCON_PIN11_CFG                                                (_ADI_MSK(0x00C00000,uint32_t))  /* p0.11  configuration bits */
#define BITM_GPIO_GPCON_PIN10_CFG                                                (_ADI_MSK(0x00300000,uint32_t))  /* p0.10  configuration bits */
#define BITM_GPIO_GPCON_PIN9_CFG                                                 (_ADI_MSK(0x000C0000,uint32_t))  /* p0.9  configuration bits */
#define BITM_GPIO_GPCON_PIN8_CFG                                                 (_ADI_MSK(0x00030000,uint32_t))  /* p0.8  configuration bits */
#define BITM_GPIO_GPCON_PIN7_CFG                                                 (_ADI_MSK(0x0000C000,uint32_t))  /* p0.7  configuration bits */
#define BITM_GPIO_GPCON_PIN6_CFG                                                 (_ADI_MSK(0x00003000,uint32_t))  /* p0.6  configuration bits */
#define BITM_GPIO_GPCON_PIN5_CFG                                                 (_ADI_MSK(0x00000C00,uint32_t))  /* p0.5  configuration bits */
#define BITM_GPIO_GPCON_PIN4_CFG                                                 (_ADI_MSK(0x00000300,uint32_t))  /* p0.4  configuration bits */
#define BITM_GPIO_GPCON_PIN3_CFG                                                 (_ADI_MSK(0x000000C0,uint32_t))  /* p0.3  configuration bits */
#define BITM_GPIO_GPCON_PIN2_CFG                                                 (_ADI_MSK(0x00000030,uint32_t))  /* p0.2  configuration bits */
#define BITM_GPIO_GPCON_PIN1_CFG                                                 (_ADI_MSK(0x0000000C,uint32_t))  /* p0.1  configuration bits */
#define BITM_GPIO_GPCON_PIN0_CFG                                                 (_ADI_MSK(0x00000003,uint32_t))  /* p0.0  configuration bits */

/* ==================================================
        SPI Registers
   ================================================== */

/* =========================
        SPIH
   ========================= */
#define REG_SPIH_SPIH0STA                    0x40024000         /* SPIH Status */
#define REG_SPIH_SPIH0RX                     0x40024004         /* SPIH Receive */
#define REG_SPIH_SPIH0TX                     0x40024008         /* SPIH Transmit */
#define REG_SPIH_SPIH0DIV                    0x4002400C         /* SPIH Baud rate selection */
#define REG_SPIH_SPIH0CON                    0x40024010         /* SPIH SPI configuration */
#define REG_SPIH_SPIH0DMA                    0x40024014         /* SPIH SPI DMA enable */
#define REG_SPIH_SPIH0CNT                    0x40024018         /* SPIH Transfer byte count */

/* =========================
        SPIH
   ========================= */
/* ------------------------------------------------------------------------------------------------------------------------
        SPIH_SPIH0STA                                                            Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_SPIH_SPIH0STA_CSRSG                                                 14                               /* Detected a rising edge on CS, in CONT mode */
#define BITP_SPIH_SPIH0STA_CSFLG                                                 13                               /* Detected a falling edge on CS, in CONT mode */
#define BITP_SPIH_SPIH0STA_CSERR                                                 12                               /* Detected a CS error condition */
#define BITP_SPIH_SPIH0STA_RXS                                                   11                               /* SPI Rx FIFO excess bytes present */
#define BITP_SPIH_SPIH0STA_RXFSTA                                                 8                               /* SPI Rx FIFO status */
#define BITP_SPIH_SPIH0STA_RXOF                                                   7                               /* SPI Rx FIFO overflow */
#define BITP_SPIH_SPIH0STA_RX                                                     6                               /* SPI Rx IRQ */
#define BITP_SPIH_SPIH0STA_TX                                                     5                               /* SPI Tx IRQ */
#define BITP_SPIH_SPIH0STA_TXUR                                                   4                               /* SPI Tx FIFO underflow */
#define BITP_SPIH_SPIH0STA_TXFSTA                                                 1                               /* SPI Tx FIFO status */
#define BITP_SPIH_SPIH0STA_IRQ                                                    0                               /* SPI Interrupt status */
#define BITM_SPIH_SPIH0STA_CSRSG                                                 (_ADI_MSK(0x00004000,uint16_t))  /* Detected a rising edge on CS, in CONT mode */
#define BITM_SPIH_SPIH0STA_CSFLG                                                 (_ADI_MSK(0x00002000,uint16_t))  /* Detected a falling edge on CS, in CONT mode */
#define BITM_SPIH_SPIH0STA_CSERR                                                 (_ADI_MSK(0x00001000,uint16_t))  /* Detected a CS error condition */
#define BITM_SPIH_SPIH0STA_RXS                                                   (_ADI_MSK(0x00000800,uint16_t))  /* SPI Rx FIFO excess bytes present */
#define BITM_SPIH_SPIH0STA_RXFSTA                                                (_ADI_MSK(0x00000700,uint16_t))  /* SPI Rx FIFO status */
#define BITM_SPIH_SPIH0STA_RXOF                                                  (_ADI_MSK(0x00000080,uint16_t))  /* SPI Rx FIFO overflow */
#define BITM_SPIH_SPIH0STA_RX                                                    (_ADI_MSK(0x00000040,uint16_t))  /* SPI Rx IRQ */
#define BITM_SPIH_SPIH0STA_TX                                                    (_ADI_MSK(0x00000020,uint16_t))  /* SPI Tx IRQ */
#define BITM_SPIH_SPIH0STA_TXUR                                                  (_ADI_MSK(0x00000010,uint16_t))  /* SPI Tx FIFO underflow */
#define BITM_SPIH_SPIH0STA_TXFSTA                                                (_ADI_MSK(0x0000000E,uint16_t))  /* SPI Tx FIFO status */
#define BITM_SPIH_SPIH0STA_IRQ                                                   (_ADI_MSK(0x00000001,uint16_t))  /* SPI Interrupt status */

/* ------------------------------------------------------------------------------------------------------------------------
        SPIH_SPIH0RX                                                             Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_SPIH_SPIH0RX_DMA_DATA_BYTE_2                                         8                               /* 8-bit receive buffer */
#define BITP_SPIH_SPIH0RX_DATA_BYTE_1                                             0                               /* 8-bit receive buffer */
#define BITM_SPIH_SPIH0RX_DMA_DATA_BYTE_2                                        (_ADI_MSK(0x0000FF00,uint16_t))  /* 8-bit receive buffer */
#define BITM_SPIH_SPIH0RX_DATA_BYTE_1                                            (_ADI_MSK(0x000000FF,uint16_t))  /* 8-bit receive buffer */

/* ------------------------------------------------------------------------------------------------------------------------
        SPIH_SPIH0TX                                                             Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_SPIH_SPIH0TX_DMA_DATA_BYTE_2                                         8                               /* 8-bit transmit buffer */
#define BITP_SPIH_SPIH0TX_DATA_BYTE_1                                             0                               /* 8-bit transmit buffer */
#define BITM_SPIH_SPIH0TX_DMA_DATA_BYTE_2                                        (_ADI_MSK(0x0000FF00,uint16_t))  /* 8-bit transmit buffer */
#define BITM_SPIH_SPIH0TX_DATA_BYTE_1                                            (_ADI_MSK(0x000000FF,uint16_t))  /* 8-bit transmit buffer */

/* ------------------------------------------------------------------------------------------------------------------------
        SPIH_SPIH0DIV                                                            Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_SPIH_SPIH0DIV_CSIRQ_EN                                               8                               /* Enable interrupt on every CS edge in CONT mode */
#define BITP_SPIH_SPIH0DIV_MD_CS_RST                                              7                               /* Reset mode for CSERR */
#define BITP_SPIH_SPIH0DIV_HFM                                                    6                               /* High Frequency Mode */
#define BITP_SPIH_SPIH0DIV_DIV                                                    0                               /* SPI clock divider */
#define BITM_SPIH_SPIH0DIV_CSIRQ_EN                                              (_ADI_MSK(0x00000100,uint16_t))  /* Enable interrupt on every CS edge in CONT mode */
#define BITM_SPIH_SPIH0DIV_MD_CS_RST                                             (_ADI_MSK(0x00000080,uint16_t))  /* Reset mode for CSERR */
#define BITM_SPIH_SPIH0DIV_HFM                                                   (_ADI_MSK(0x00000040,uint16_t))  /* High Frequency Mode */
#define BITM_SPIH_SPIH0DIV_DIV                                                   (_ADI_MSK(0x0000003F,uint16_t))  /* SPI clock divider */

/* ------------------------------------------------------------------------------------------------------------------------
        SPIH_SPIH0CON                                                            Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_SPIH_SPIH0CON_MOD                                                   14                               /* SPI IRQ mode bits */
#define BITP_SPIH_SPIH0CON_TFLUSH                                                13                               /* SPI Tx FIFO Flush enable */
#define BITP_SPIH_SPIH0CON_RFLUSH                                                12                               /* SPI Rx FIFO Flush enable */
#define BITP_SPIH_SPIH0CON_CON                                                   11                               /* Continuous transfer enable */
#define BITP_SPIH_SPIH0CON_LOOPBACK                                              10                               /* Loopback enable */
#define BITP_SPIH_SPIH0CON_OEN                                                    9                               /* Slave MISO output enable */
#define BITP_SPIH_SPIH0CON_RXOF                                                   8                               /* SPIRX overflow overwrite enable */
#define BITP_SPIH_SPIH0CON_ZEN                                                    7                               /* Transmit zeros enable */
#define BITP_SPIH_SPIH0CON_TIM                                                    6                               /* SPI transfer and interrupt mode */
#define BITP_SPIH_SPIH0CON_LSB                                                    5                               /* LSB first transfer enable */
#define BITP_SPIH_SPIH0CON_WOM                                                    4                               /* SPI Wired Or mode */
#define BITP_SPIH_SPIH0CON_CPOL                                                   3                               /* Serial Clock Polarity */
#define BITP_SPIH_SPIH0CON_CPHA                                                   2                               /* Serial clock phase mode */
#define BITP_SPIH_SPIH0CON_MASEN                                                  1                               /* Master mode enable */
#define BITP_SPIH_SPIH0CON_ENABLE                                                 0                               /* SPI enable */
#define BITM_SPIH_SPIH0CON_MOD                                                   (_ADI_MSK(0x0000C000,uint16_t))  /* SPI IRQ mode bits */
#define BITM_SPIH_SPIH0CON_TFLUSH                                                (_ADI_MSK(0x00002000,uint16_t))  /* SPI Tx FIFO Flush enable */
#define BITM_SPIH_SPIH0CON_RFLUSH                                                (_ADI_MSK(0x00001000,uint16_t))  /* SPI Rx FIFO Flush enable */
#define BITM_SPIH_SPIH0CON_CON                                                   (_ADI_MSK(0x00000800,uint16_t))  /* Continuous transfer enable */
#define BITM_SPIH_SPIH0CON_LOOPBACK                                              (_ADI_MSK(0x00000400,uint16_t))  /* Loopback enable */
#define BITM_SPIH_SPIH0CON_OEN                                                   (_ADI_MSK(0x00000200,uint16_t))  /* Slave MISO output enable */
#define BITM_SPIH_SPIH0CON_RXOF                                                  (_ADI_MSK(0x00000100,uint16_t))  /* SPIRX overflow overwrite enable */
#define BITM_SPIH_SPIH0CON_ZEN                                                   (_ADI_MSK(0x00000080,uint16_t))  /* Transmit zeros enable */
#define BITM_SPIH_SPIH0CON_TIM                                                   (_ADI_MSK(0x00000040,uint16_t))  /* SPI transfer and interrupt mode */
#define BITM_SPIH_SPIH0CON_LSB                                                   (_ADI_MSK(0x00000020,uint16_t))  /* LSB first transfer enable */
#define BITM_SPIH_SPIH0CON_WOM                                                   (_ADI_MSK(0x00000010,uint16_t))  /* SPI Wired Or mode */
#define BITM_SPIH_SPIH0CON_CPOL                                                  (_ADI_MSK(0x00000008,uint16_t))  /* Serial Clock Polarity */
#define BITM_SPIH_SPIH0CON_CPHA                                                  (_ADI_MSK(0x00000004,uint16_t))  /* Serial clock phase mode */
#define BITM_SPIH_SPIH0CON_MASEN                                                 (_ADI_MSK(0x00000002,uint16_t))  /* Master mode enable */
#define BITM_SPIH_SPIH0CON_ENABLE                                                (_ADI_MSK(0x00000001,uint16_t))  /* SPI enable */

/* ------------------------------------------------------------------------------------------------------------------------
        SPIH_SPIH0DMA                                                            Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_SPIH_SPIH0DMA_IENRXDMA                                               2                               /* Enable receive DMA request */
#define BITP_SPIH_SPIH0DMA_IENTXDMA                                               1                               /* Enable transmit DMA request */
#define BITP_SPIH_SPIH0DMA_ENABLE                                                 0                               /* Enable DMA for data transfer */
#define BITM_SPIH_SPIH0DMA_IENRXDMA                                              (_ADI_MSK(0x00000004,uint16_t))  /* Enable receive DMA request */
#define BITM_SPIH_SPIH0DMA_IENTXDMA                                              (_ADI_MSK(0x00000002,uint16_t))  /* Enable transmit DMA request */
#define BITM_SPIH_SPIH0DMA_ENABLE                                                (_ADI_MSK(0x00000001,uint16_t))  /* Enable DMA for data transfer */

/* ------------------------------------------------------------------------------------------------------------------------
        SPIH_SPIH0CNT                                                            Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_SPIH_SPIH0CNT_COUNT                                                  0                               /* Transfer byte count */
#define BITM_SPIH_SPIH0CNT_COUNT                                                 (_ADI_MSK(0x000000FF,uint16_t))  /* Transfer byte count */

/* ==================================================
        Clock gating and other settings Registers
   ================================================== */

/* =========================
        SYSCLK
   ========================= */
#define REG_SYSCLK_CLKCON0                   0x40028000         /* SYSCLK Misc clock settings */
#define REG_SYSCLK_CLKCON1                   0x40028004         /* SYSCLK Clock dividers */
#define REG_SYSCLK_CLKCON3                   0x4002800C         /* SYSCLK System PLL */
#define REG_SYSCLK_CLKCON4                   0x40028010         /* SYSCLK USB PLL */
#define REG_SYSCLK_CLKCON5                   0x40028014         /* SYSCLK User clock gating control */
#define REG_SYSCLK_CLKSTAT0                  0x40028018         /* SYSCLK Clocking status */

/* =========================
        SYSCLK
   ========================= */
/* ------------------------------------------------------------------------------------------------------------------------
        SYSCLK_CLKCON0                                                           Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_SYSCLK_CLKCON0_HFXTALIE                                             15                               /* High frequency crystal interrupt enable */
#define BITP_SYSCLK_CLKCON0_LFXTALIE                                             14                               /* Low frequency crystal interrupt enable */
#define BITP_SYSCLK_CLKCON0_PLLMUX                                               11                               /* PLL source select mux */
#define BITP_SYSCLK_CLKCON0_FIXMASTERTYPE                                        10                               /* force MasterType for debugger */
#define BITP_SYSCLK_CLKCON0_LFCLKMUX                                              8                               /* 32 KHz clock select mux */
#define BITP_SYSCLK_CLKCON0_CLKCOUT                                               4                               /* GPIO clk out select */
#define BITP_SYSCLK_CLKCON0_CLKMUX                                                0                               /* Clock mux select */
#define BITM_SYSCLK_CLKCON0_HFXTALIE                                             (_ADI_MSK(0x00008000,uint16_t))  /* High frequency crystal interrupt enable */
#define BITM_SYSCLK_CLKCON0_LFXTALIE                                             (_ADI_MSK(0x00004000,uint16_t))  /* Low frequency crystal interrupt enable */
#define BITM_SYSCLK_CLKCON0_PLLMUX                                               (_ADI_MSK(0x00000800,uint16_t))  /* PLL source select mux */
#define BITM_SYSCLK_CLKCON0_FIXMASTERTYPE                                        (_ADI_MSK(0x00000400,uint16_t))  /* force MasterType for debugger */
#define BITM_SYSCLK_CLKCON0_LFCLKMUX                                             (_ADI_MSK(0x00000100,uint16_t))  /* 32 KHz clock select mux */
#define BITM_SYSCLK_CLKCON0_CLKCOUT                                              (_ADI_MSK(0x000000F0,uint16_t))  /* GPIO clk out select */
#define BITM_SYSCLK_CLKCON0_CLKMUX                                               (_ADI_MSK(0x00000003,uint16_t))  /* Clock mux select */

/* ------------------------------------------------------------------------------------------------------------------------
        SYSCLK_CLKCON1                                                           Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_SYSCLK_CLKCON1_PCLKDIVCNT                                            8                               /* PCLK divide count */
#define BITP_SYSCLK_CLKCON1_USBCTLCLKDIVMUX                                       7                               /* USB control clock divider mux select */
#define BITP_SYSCLK_CLKCON1_HCLKDIVCNT                                            0                               /* HCLK and CTCLK divide count */
#define BITM_SYSCLK_CLKCON1_PCLKDIVCNT                                           (_ADI_MSK(0x00003F00,uint16_t))  /* PCLK divide count */
#define BITM_SYSCLK_CLKCON1_USBCTLCLKDIVMUX                                      (_ADI_MSK(0x00000080,uint16_t))  /* USB control clock divider mux select */
#define BITM_SYSCLK_CLKCON1_HCLKDIVCNT                                           (_ADI_MSK(0x0000003F,uint16_t))  /* HCLK and CTCLK divide count */

/* ------------------------------------------------------------------------------------------------------------------------
        SYSCLK_CLKCON3                                                           Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_SYSCLK_CLKCON3_SPLLIE                                               10                               /* System PLL interrupt enable */
#define BITP_SYSCLK_CLKCON3_SPLLEN                                                9                               /* System PLL enable */
#define BITP_SYSCLK_CLKCON3_SPLLDIV2                                              8                               /* System PLL division by 2 */
#define BITP_SYSCLK_CLKCON3_SPLLMSEL                                              6                               /* System PLL M Divider */
#define BITP_SYSCLK_CLKCON3_SPLLNSEL                                              0                               /* System PLL N multiplier */
#define BITM_SYSCLK_CLKCON3_SPLLIE                                               (_ADI_MSK(0x00000400,uint16_t))  /* System PLL interrupt enable */
#define BITM_SYSCLK_CLKCON3_SPLLEN                                               (_ADI_MSK(0x00000200,uint16_t))  /* System PLL enable */
#define BITM_SYSCLK_CLKCON3_SPLLDIV2                                             (_ADI_MSK(0x00000100,uint16_t))  /* System PLL division by 2 */
#define BITM_SYSCLK_CLKCON3_SPLLMSEL                                             (_ADI_MSK(0x000000C0,uint16_t))  /* System PLL M Divider */
#define BITM_SYSCLK_CLKCON3_SPLLNSEL                                             (_ADI_MSK(0x0000003F,uint16_t))  /* System PLL N multiplier */

/* ------------------------------------------------------------------------------------------------------------------------
        SYSCLK_CLKCON4                                                           Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_SYSCLK_CLKCON4_UPLLIE                                               10                               /* USB PLL interrupt enable */
#define BITP_SYSCLK_CLKCON4_UPLLEN                                                9                               /* USB PLL enable */
#define BITP_SYSCLK_CLKCON4_UPLLDIV2                                              8                               /* USB PLL division by 2 */
#define BITP_SYSCLK_CLKCON4_UPLLMSEL                                              6                               /* USB PLL M divider */
#define BITP_SYSCLK_CLKCON4_UPLLNSEL                                              0                               /* USB PLL N multiplier */
#define BITM_SYSCLK_CLKCON4_UPLLIE                                               (_ADI_MSK(0x00000400,uint16_t))  /* USB PLL interrupt enable */
#define BITM_SYSCLK_CLKCON4_UPLLEN                                               (_ADI_MSK(0x00000200,uint16_t))  /* USB PLL enable */
#define BITM_SYSCLK_CLKCON4_UPLLDIV2                                             (_ADI_MSK(0x00000100,uint16_t))  /* USB PLL division by 2 */
#define BITM_SYSCLK_CLKCON4_UPLLMSEL                                             (_ADI_MSK(0x000000C0,uint16_t))  /* USB PLL M divider */
#define BITM_SYSCLK_CLKCON4_UPLLNSEL                                             (_ADI_MSK(0x0000003F,uint16_t))  /* USB PLL N multiplier */

/* ------------------------------------------------------------------------------------------------------------------------
        SYSCLK_CLKCON5                                                           Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_SYSCLK_CLKCON5_CTCLKOFF                                              8                               /* CTCLK User control */
#define BITP_SYSCLK_CLKCON5_ACLKOFF                                               7                               /* ACLK User control */
#define BITP_SYSCLK_CLKCON5_UCLKI2SOFF                                            5                               /* I2S clock user control */
#define BITP_SYSCLK_CLKCON5_UCLKUARTOFF                                           4                               /* UART clock user control */
#define BITP_SYSCLK_CLKCON5_UCLKI2COFF                                            3                               /* I2C clock user control */
#define BITP_SYSCLK_CLKCON5_UCLKSPIHOFF                                           2                               /* SPIH clock user control */
#define BITP_SYSCLK_CLKCON5_UCLKSPI1OFF                                           1                               /* SPI1 clock user control */
#define BITP_SYSCLK_CLKCON5_UCLKSPI0OFF                                           0                               /* SPI0 clock user control */
#define BITM_SYSCLK_CLKCON5_CTCLKOFF                                             (_ADI_MSK(0x00000100,uint16_t))  /* CTCLK User control */
#define BITM_SYSCLK_CLKCON5_ACLKOFF                                              (_ADI_MSK(0x00000080,uint16_t))  /* ACLK User control */
#define BITM_SYSCLK_CLKCON5_UCLKI2SOFF                                           (_ADI_MSK(0x00000020,uint16_t))  /* I2S clock user control */
#define BITM_SYSCLK_CLKCON5_UCLKUARTOFF                                          (_ADI_MSK(0x00000010,uint16_t))  /* UART clock user control */
#define BITM_SYSCLK_CLKCON5_UCLKI2COFF                                           (_ADI_MSK(0x00000008,uint16_t))  /* I2C clock user control */
#define BITM_SYSCLK_CLKCON5_UCLKSPIHOFF                                          (_ADI_MSK(0x00000004,uint16_t))  /* SPIH clock user control */
#define BITM_SYSCLK_CLKCON5_UCLKSPI1OFF                                          (_ADI_MSK(0x00000002,uint16_t))  /* SPI1 clock user control */
#define BITM_SYSCLK_CLKCON5_UCLKSPI0OFF                                          (_ADI_MSK(0x00000001,uint16_t))  /* SPI0 clock user control */

/* ------------------------------------------------------------------------------------------------------------------------
        SYSCLK_CLKSTAT0                                                          Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_SYSCLK_CLKSTAT0_HFXTALNOK                                           14                               /* HF crystal not stable */
#define BITP_SYSCLK_CLKSTAT0_HFXTALOK                                            13                               /* HF crystal stable */
#define BITP_SYSCLK_CLKSTAT0_HFXTALSTATUS                                        12                               /* HF crystal status */
#define BITP_SYSCLK_CLKSTAT0_LFXTALNOK                                           10                               /* LF crystal not stable */
#define BITP_SYSCLK_CLKSTAT0_LFXTALOK                                             9                               /* LF crystal stable */
#define BITP_SYSCLK_CLKSTAT0_LFXTALSTATUS                                         8                               /* LF crystal status */
#define BITP_SYSCLK_CLKSTAT0_UPLLUNLOCK                                           6                               /* USB PLL unlock */
#define BITP_SYSCLK_CLKSTAT0_UPLLLOCK                                             5                               /* USB PLL lock */
#define BITP_SYSCLK_CLKSTAT0_UPLLSTATUS                                           4                               /* USB PLL status */
#define BITP_SYSCLK_CLKSTAT0_SPLLUNLOCK                                           2                               /* System PLL unlock */
#define BITP_SYSCLK_CLKSTAT0_SPLLLOCK                                             1                               /* System PLL lock */
#define BITP_SYSCLK_CLKSTAT0_SPLLSTATUS                                           0                               /* System PLL status */
#define BITM_SYSCLK_CLKSTAT0_HFXTALNOK                                           (_ADI_MSK(0x00004000,uint16_t))  /* HF crystal not stable */
#define BITM_SYSCLK_CLKSTAT0_HFXTALOK                                            (_ADI_MSK(0x00002000,uint16_t))  /* HF crystal stable */
#define BITM_SYSCLK_CLKSTAT0_HFXTALSTATUS                                        (_ADI_MSK(0x00001000,uint16_t))  /* HF crystal status */
#define BITM_SYSCLK_CLKSTAT0_LFXTALNOK                                           (_ADI_MSK(0x00000400,uint16_t))  /* LF crystal not stable */
#define BITM_SYSCLK_CLKSTAT0_LFXTALOK                                            (_ADI_MSK(0x00000200,uint16_t))  /* LF crystal stable */
#define BITM_SYSCLK_CLKSTAT0_LFXTALSTATUS                                        (_ADI_MSK(0x00000100,uint16_t))  /* LF crystal status */
#define BITM_SYSCLK_CLKSTAT0_UPLLUNLOCK                                          (_ADI_MSK(0x00000040,uint16_t))  /* USB PLL unlock */
#define BITM_SYSCLK_CLKSTAT0_UPLLLOCK                                            (_ADI_MSK(0x00000020,uint16_t))  /* USB PLL lock */
#define BITM_SYSCLK_CLKSTAT0_UPLLSTATUS                                          (_ADI_MSK(0x00000010,uint16_t))  /* USB PLL status */
#define BITM_SYSCLK_CLKSTAT0_SPLLUNLOCK                                          (_ADI_MSK(0x00000004,uint16_t))  /* System PLL unlock */
#define BITM_SYSCLK_CLKSTAT0_SPLLLOCK                                            (_ADI_MSK(0x00000002,uint16_t))  /* System PLL lock */
#define BITM_SYSCLK_CLKSTAT0_SPLLSTATUS                                          (_ADI_MSK(0x00000001,uint16_t))  /* System PLL status */

/* ==================================================
        Bus matrix Registers
   ================================================== */

/* =========================
        BUSM
   ========================= */
#define REG_BUSM_BMARBIT0                    0x40028040         /* BUSM Arbitration Priority Configuration for Slave 0 (Flash) */
#define REG_BUSM_BMARBIT1                    0x40028044         /* BUSM Arbitration Priority Configuration for Slave 1 (SRAM0) */
#define REG_BUSM_BMARBIT2                    0x40028048         /* BUSM Arbitration Priority Configuration for Slave 2 (SRAM1 + EEPROM) */
#define REG_BUSM_BMARBIT3                    0x4002804C         /* BUSM Arbitration Priority Configuration for Slave 3 (AFE) */
#define REG_BUSM_BMARBIT4                    0x40028050         /* BUSM Arbitration Priority Configuration for Slave 4 (32-bit APB) */
#define REG_BUSM_BMARBIT5                    0x40028054         /* BUSM Arbitration Priority Configuration for Slave 5 (16-bit APB with DMA access) */

/* =========================
        BUSM
   ========================= */
/* ------------------------------------------------------------------------------------------------------------------------
        BUSM_BMARBIT0                                                            Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_BUSM_BMARBIT0_DEFAULT_ACTIVE0                                        8                               /* default active master when no request */
#define BITP_BUSM_BMARBIT0_ARBIT_PRIO_0                                           0                               /* Arbiter Priority for Slave 0 */
#define BITM_BUSM_BMARBIT0_DEFAULT_ACTIVE0                                       (_ADI_MSK(0x00000300,uint16_t))  /* default active master when no request */
#define BITM_BUSM_BMARBIT0_ARBIT_PRIO_0                                          (_ADI_MSK(0x00000007,uint16_t))  /* Arbiter Priority for Slave 0 */

/* ------------------------------------------------------------------------------------------------------------------------
        BUSM_BMARBIT1                                                            Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_BUSM_BMARBIT1_DEFAULT_ACTIVE1                                        8                               /* default active master when no request */
#define BITP_BUSM_BMARBIT1_ARBIT_PRIO_1                                           0                               /* Arbiter Priority for Slave 1 */
#define BITM_BUSM_BMARBIT1_DEFAULT_ACTIVE1                                       (_ADI_MSK(0x00000300,uint16_t))  /* default active master when no request */
#define BITM_BUSM_BMARBIT1_ARBIT_PRIO_1                                          (_ADI_MSK(0x00000007,uint16_t))  /* Arbiter Priority for Slave 1 */

/* ------------------------------------------------------------------------------------------------------------------------
        BUSM_BMARBIT2                                                            Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_BUSM_BMARBIT2_DEFAULT_ACTIVE2                                        8                               /* default active master when no request */
#define BITP_BUSM_BMARBIT2_ARBIT_PRIO_2                                           0                               /* Arbiter Priority for Slave 2 */
#define BITM_BUSM_BMARBIT2_DEFAULT_ACTIVE2                                       (_ADI_MSK(0x00000300,uint16_t))  /* default active master when no request */
#define BITM_BUSM_BMARBIT2_ARBIT_PRIO_2                                          (_ADI_MSK(0x00000007,uint16_t))  /* Arbiter Priority for Slave 2 */

/* ------------------------------------------------------------------------------------------------------------------------
        BUSM_BMARBIT3                                                            Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_BUSM_BMARBIT3_DEFAULT_ACTIVE3                                        8                               /* default active master when no request */
#define BITP_BUSM_BMARBIT3_ARBIT_PRIO_3                                           0                               /* Arbiter Priority for Slave 3 */
#define BITM_BUSM_BMARBIT3_DEFAULT_ACTIVE3                                       (_ADI_MSK(0x00000300,uint16_t))  /* default active master when no request */
#define BITM_BUSM_BMARBIT3_ARBIT_PRIO_3                                          (_ADI_MSK(0x00000007,uint16_t))  /* Arbiter Priority for Slave 3 */

/* ------------------------------------------------------------------------------------------------------------------------
        BUSM_BMARBIT4                                                            Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_BUSM_BMARBIT4_DEFAULT_ACTIVE4                                        8                               /* default active master when no request */
#define BITP_BUSM_BMARBIT4_ARBIT_PRIO_4                                           0                               /* Arbiter Priority for Slave 4 */
#define BITM_BUSM_BMARBIT4_DEFAULT_ACTIVE4                                       (_ADI_MSK(0x00000300,uint16_t))  /* default active master when no request */
#define BITM_BUSM_BMARBIT4_ARBIT_PRIO_4                                          (_ADI_MSK(0x00000007,uint16_t))  /* Arbiter Priority for Slave 4 */

/* ------------------------------------------------------------------------------------------------------------------------
        BUSM_BMARBIT5                                                            Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_BUSM_BMARBIT5_DEFAULT_ACTIVE5                                        8                               /* default active master when no request */
#define BITP_BUSM_BMARBIT5_ARBIT_PRIO_5                                           0                               /* Arbiter Priority for Slave 5 */
#define BITM_BUSM_BMARBIT5_DEFAULT_ACTIVE5                                       (_ADI_MSK(0x00000300,uint16_t))  /* default active master when no request */
#define BITM_BUSM_BMARBIT5_ARBIT_PRIO_5                                          (_ADI_MSK(0x00000007,uint16_t))  /* Arbiter Priority for Slave 5 */

/* ==================================================
        CRC Registers
   ================================================== */

/* =========================
        CRC
   ========================= */
#define REG_CRC_CRC_CTL                      0x4002C000         /* CRC CRC Control Register */
#define REG_CRC_CRC_IPDATA                   0x4002C004         /* CRC Input Data Register */
#define REG_CRC_CRC_RESULT                   0x4002C008         /* CRC CRC Result Register */

/* =========================
        CRC
   ========================= */
/* ------------------------------------------------------------------------------------------------------------------------
        CRC_CRC_CTL                                                              Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CRC_CRC_CTL_STRT_CRC                                                 7                               /* START_CRC */
#define BITP_CRC_CRC_CTL_W16SWP                                                   6                               /* Word16 Swap */
#define BITP_CRC_CRC_CTL_BYTMIRR                                                  5                               /* Byte Mirroring */
#define BITP_CRC_CRC_CTL_BITMIRR                                                  4                               /* Bit Mirroring */
#define BITP_CRC_CRC_CTL_AUTORST                                                  2                               /* Sets the initial Seed for the CRC calculation */
#define BITP_CRC_CRC_CTL_DMAREQEN                                                 1                               /* DMA request enable */
#define BITP_CRC_CRC_CTL_BLKEN                                                    0                               /* CRC peripheral enable */
#define BITM_CRC_CRC_CTL_STRT_CRC                                                (_ADI_MSK(0x00000080,uint32_t))  /* START_CRC */
#define BITM_CRC_CRC_CTL_W16SWP                                                  (_ADI_MSK(0x00000040,uint32_t))  /* Word16 Swap */
#define BITM_CRC_CRC_CTL_BYTMIRR                                                 (_ADI_MSK(0x00000020,uint32_t))  /* Byte Mirroring */
#define BITM_CRC_CRC_CTL_BITMIRR                                                 (_ADI_MSK(0x00000010,uint32_t))  /* Bit Mirroring */
#define BITM_CRC_CRC_CTL_AUTORST                                                 (_ADI_MSK(0x0000000C,uint32_t))  /* Sets the initial Seed for the CRC calculation */
#define BITM_CRC_CRC_CTL_DMAREQEN                                                (_ADI_MSK(0x00000002,uint32_t))  /* DMA request enable */
#define BITM_CRC_CRC_CTL_BLKEN                                                   (_ADI_MSK(0x00000001,uint32_t))  /* CRC peripheral enable */

/* ==================================================
        PDI Registers
   ================================================== */

/* =========================
        PDI
   ========================= */
#define REG_PDI_PDI_CFG                      0x40030000         /* PDI PDI Configuration  Register */
#define REG_PDI_PDI_INT_SET                  0x40030004         /* PDI PDI Interrupt Set Register */
#define REG_PDI_PDI_INT_CLR                  0x40030008         /* PDI PDI Interrupt Clear Register */
#define REG_PDI_PDI_STAT                     0x4003000C         /* PDI PDI Status Register */
#define REG_PDI_PDI_CMD                      0x40030010         /* PDI PDI Command Register */
#define REG_PDI_PDI_FRDATA_N                 0x40030014         /* PDI PDI Frame Data Count Register */
#define REG_PDI_PDI_FIFO                     0x40030018         /* PDI PDI Parameter FIFO */
#define REG_PDI_PDI_IF_TIMING                0x4003001C         /* PDI PDI Interface Timing Register */

/* =========================
        PDI
   ========================= */
/* ------------------------------------------------------------------------------------------------------------------------
        PDI_PDI_CFG                                                              Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_PDI_PDI_CFG_END_XFER                                                 9                               /* End DBI Transfer */
#define BITP_PDI_PDI_CFG_DMA_EN                                                   8                               /* DMA Request Enable */
#define BITP_PDI_PDI_CFG_FLSH_FIFO                                                7                               /* Flush the Parameter FIFO */
#define BITP_PDI_PDI_CFG_DWIDTH_BPP                                               4                               /* Data bus width and Color depth */
#define BITP_PDI_PDI_CFG_DBI_TYPE                                                 1                               /* Type of the DBI */
#define BITP_PDI_PDI_CFG_PDI_EN                                                   0                               /* PDI controller Enable */
#define BITM_PDI_PDI_CFG_END_XFER                                                (_ADI_MSK(0x00000200,uint32_t))  /* End DBI Transfer */
#define BITM_PDI_PDI_CFG_DMA_EN                                                  (_ADI_MSK(0x00000100,uint32_t))  /* DMA Request Enable */
#define BITM_PDI_PDI_CFG_FLSH_FIFO                                               (_ADI_MSK(0x00000080,uint32_t))  /* Flush the Parameter FIFO */
#define BITM_PDI_PDI_CFG_DWIDTH_BPP                                              (_ADI_MSK(0x00000070,uint32_t))  /* Data bus width and Color depth */
#define BITM_PDI_PDI_CFG_DBI_TYPE                                                (_ADI_MSK(0x0000000E,uint32_t))  /* Type of the DBI */
#define BITM_PDI_PDI_CFG_PDI_EN                                                  (_ADI_MSK(0x00000001,uint32_t))  /* PDI controller Enable */

/* ------------------------------------------------------------------------------------------------------------------------
        PDI_PDI_INT_SET                                                          Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_PDI_PDI_INT_SET_SET_FIFO_UDF_IEN                                    11                               /* Enable FIFO Underflow IRQ */
#define BITP_PDI_PDI_INT_SET_SET_FIFO_OVF_IEN                                    10                               /* Enable FIFO Overflow IRQ */
#define BITP_PDI_PDI_INT_SET_SET_CMD_WR_ERR_IEN                                   9                               /* Enable Command Write Error IRQ */
#define BITP_PDI_PDI_INT_SET_SET_CMD_DONE_IEN                                     8                               /* Enable Command Done IRQ */
#define BITP_PDI_PDI_INT_SET_SET_DONE_IEN                                         3                               /* Enable DONE-IRQ */
#define BITP_PDI_PDI_INT_SET_SET_TE_IEN                                           2                               /* Enable TE-IRQ */
#define BITP_PDI_PDI_INT_SET_SET_RX_IEN                                           1                               /* Enable Rx-IRQ */
#define BITP_PDI_PDI_INT_SET_SET_TX_IEN                                           0                               /* Enable Tx-IRQ */
#define BITM_PDI_PDI_INT_SET_SET_FIFO_UDF_IEN                                    (_ADI_MSK(0x00000800,uint32_t))  /* Enable FIFO Underflow IRQ */
#define BITM_PDI_PDI_INT_SET_SET_FIFO_OVF_IEN                                    (_ADI_MSK(0x00000400,uint32_t))  /* Enable FIFO Overflow IRQ */
#define BITM_PDI_PDI_INT_SET_SET_CMD_WR_ERR_IEN                                  (_ADI_MSK(0x00000200,uint32_t))  /* Enable Command Write Error IRQ */
#define BITM_PDI_PDI_INT_SET_SET_CMD_DONE_IEN                                    (_ADI_MSK(0x00000100,uint32_t))  /* Enable Command Done IRQ */
#define BITM_PDI_PDI_INT_SET_SET_DONE_IEN                                        (_ADI_MSK(0x00000008,uint32_t))  /* Enable DONE-IRQ */
#define BITM_PDI_PDI_INT_SET_SET_TE_IEN                                          (_ADI_MSK(0x00000004,uint32_t))  /* Enable TE-IRQ */
#define BITM_PDI_PDI_INT_SET_SET_RX_IEN                                          (_ADI_MSK(0x00000002,uint32_t))  /* Enable Rx-IRQ */
#define BITM_PDI_PDI_INT_SET_SET_TX_IEN                                          (_ADI_MSK(0x00000001,uint32_t))  /* Enable Tx-IRQ */

/* ------------------------------------------------------------------------------------------------------------------------
        PDI_PDI_INT_CLR                                                          Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_PDI_PDI_INT_CLR_CLR_FIFO_UDF_IEN                                    11                               /* Disable FIFO Underflow IRQ */
#define BITP_PDI_PDI_INT_CLR_CLR_FIFO_OVF_IEN                                    10                               /* Disable FIFO Overflow IRQ */
#define BITP_PDI_PDI_INT_CLR_CLR_CMD_WR_ERR_IEN                                   9                               /* Disable Command Write Error IRQ */
#define BITP_PDI_PDI_INT_CLR_CLR_CMD_DONE_IEN                                     8                               /* Disable Command Done IRQ */
#define BITP_PDI_PDI_INT_CLR_CLR_DONE_IEN                                         3                               /* Disable DONE-IRQ */
#define BITP_PDI_PDI_INT_CLR_CLR_TE_IEN                                           2                               /* Disable TE-IRQ */
#define BITP_PDI_PDI_INT_CLR_CLR_RX_IEN                                           1                               /* Disable Rx-IRQ */
#define BITP_PDI_PDI_INT_CLR_CLR_TX_IEN                                           0                               /* Disable Tx-IRQ */
#define BITM_PDI_PDI_INT_CLR_CLR_FIFO_UDF_IEN                                    (_ADI_MSK(0x00000800,uint32_t))  /* Disable FIFO Underflow IRQ */
#define BITM_PDI_PDI_INT_CLR_CLR_FIFO_OVF_IEN                                    (_ADI_MSK(0x00000400,uint32_t))  /* Disable FIFO Overflow IRQ */
#define BITM_PDI_PDI_INT_CLR_CLR_CMD_WR_ERR_IEN                                  (_ADI_MSK(0x00000200,uint32_t))  /* Disable Command Write Error IRQ */
#define BITM_PDI_PDI_INT_CLR_CLR_CMD_DONE_IEN                                    (_ADI_MSK(0x00000100,uint32_t))  /* Disable Command Done IRQ */
#define BITM_PDI_PDI_INT_CLR_CLR_DONE_IEN                                        (_ADI_MSK(0x00000008,uint32_t))  /* Disable DONE-IRQ */
#define BITM_PDI_PDI_INT_CLR_CLR_TE_IEN                                          (_ADI_MSK(0x00000004,uint32_t))  /* Disable TE-IRQ */
#define BITM_PDI_PDI_INT_CLR_CLR_RX_IEN                                          (_ADI_MSK(0x00000002,uint32_t))  /* Disable Rx-IRQ */
#define BITM_PDI_PDI_INT_CLR_CLR_TX_IEN                                          (_ADI_MSK(0x00000001,uint32_t))  /* Disable Tx-IRQ */

/* ------------------------------------------------------------------------------------------------------------------------
        PDI_PDI_STAT                                                             Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_PDI_PDI_STAT_FIFO_UDF                                               11                               /* FIFO Underflow */
#define BITP_PDI_PDI_STAT_FIFO_OVF                                               10                               /* FIFO Overflow */
#define BITP_PDI_PDI_STAT_CMD_WR_ERR                                              9                               /* PDI Command Write Error */
#define BITP_PDI_PDI_STAT_CMD_DONE                                                8                               /* PDI Command completed */
#define BITP_PDI_PDI_STAT_FIFO_FULL                                               7                               /* FIFO Full */
#define BITP_PDI_PDI_STAT_FIFO_EMPTY                                              6                               /* FIFO Empty */
#define BITP_PDI_PDI_STAT_FIFO_STAT                                               4                               /* FIFO Status */
#define BITP_PDI_PDI_STAT_DONE_IRQ                                                3                               /* DONE-Interrupt */
#define BITP_PDI_PDI_STAT_TE_IRQ                                                  2                               /* Tearing Effect-Interrupt */
#define BITP_PDI_PDI_STAT_RX_IRQ                                                  1                               /* Rx-Interrupt */
#define BITP_PDI_PDI_STAT_TX_IRQ                                                  0                               /* Tx-Interrupt */
#define BITM_PDI_PDI_STAT_FIFO_UDF                                               (_ADI_MSK(0x00000800,uint32_t))  /* FIFO Underflow */
#define BITM_PDI_PDI_STAT_FIFO_OVF                                               (_ADI_MSK(0x00000400,uint32_t))  /* FIFO Overflow */
#define BITM_PDI_PDI_STAT_CMD_WR_ERR                                             (_ADI_MSK(0x00000200,uint32_t))  /* PDI Command Write Error */
#define BITM_PDI_PDI_STAT_CMD_DONE                                               (_ADI_MSK(0x00000100,uint32_t))  /* PDI Command completed */
#define BITM_PDI_PDI_STAT_FIFO_FULL                                              (_ADI_MSK(0x00000080,uint32_t))  /* FIFO Full */
#define BITM_PDI_PDI_STAT_FIFO_EMPTY                                             (_ADI_MSK(0x00000040,uint32_t))  /* FIFO Empty */
#define BITM_PDI_PDI_STAT_FIFO_STAT                                              (_ADI_MSK(0x00000030,uint32_t))  /* FIFO Status */
#define BITM_PDI_PDI_STAT_DONE_IRQ                                               (_ADI_MSK(0x00000008,uint32_t))  /* DONE-Interrupt */
#define BITM_PDI_PDI_STAT_TE_IRQ                                                 (_ADI_MSK(0x00000004,uint32_t))  /* Tearing Effect-Interrupt */
#define BITM_PDI_PDI_STAT_RX_IRQ                                                 (_ADI_MSK(0x00000002,uint32_t))  /* Rx-Interrupt */
#define BITM_PDI_PDI_STAT_TX_IRQ                                                 (_ADI_MSK(0x00000001,uint32_t))  /* Tx-Interrupt */

/* ------------------------------------------------------------------------------------------------------------------------
        PDI_PDI_CMD                                                              Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_PDI_PDI_CMD_N_PARAM                                                 16                               /* Number of control parameters */
#define BITP_PDI_PDI_CMD_FR_DATA                                                  9                               /* Type of Parameter */
#define BITP_PDI_PDI_CMD_WR_RD                                                    8                               /* Write or Read direction */
#define BITP_PDI_PDI_CMD_CMD                                                      0                               /* DBI Command */
#define BITM_PDI_PDI_CMD_N_PARAM                                                 (_ADI_MSK(0x00FF0000,uint32_t))  /* Number of control parameters */
#define BITM_PDI_PDI_CMD_FR_DATA                                                 (_ADI_MSK(0x00000200,uint32_t))  /* Type of Parameter */
#define BITM_PDI_PDI_CMD_WR_RD                                                   (_ADI_MSK(0x00000100,uint32_t))  /* Write or Read direction */
#define BITM_PDI_PDI_CMD_CMD                                                     (_ADI_MSK(0x000000FF,uint32_t))  /* DBI Command */

/* ------------------------------------------------------------------------------------------------------------------------
        PDI_PDI_FRDATA_N                                                         Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_PDI_PDI_FRDATA_N_FR_DATA_N                                           0                               /* Number of DBI Frame Data transfers */
#define BITM_PDI_PDI_FRDATA_N_FR_DATA_N                                          (_ADI_MSK(0x00001FFF,uint32_t))  /* Number of DBI Frame Data transfers */

/* ------------------------------------------------------------------------------------------------------------------------
        PDI_PDI_IF_TIMING                                                        Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_PDI_PDI_IF_TIMING_TCSH                                              24                               /* Chip Select Setup Time */
#define BITP_PDI_PDI_IF_TIMING_TWRL_RDL                                          16                               /* Low Duration for Write/Read Control Pulse */
#define BITP_PDI_PDI_IF_TIMING_TWRH_RDH                                           8                               /* High Duration for Write/Read Control Pulse */
#define BITP_PDI_PDI_IF_TIMING_TCSF                                               4                               /* Chip Select Wait Time */
#define BITP_PDI_PDI_IF_TIMING_TAS_AH                                             0                               /* Address Setup/Hold Time */
#define BITM_PDI_PDI_IF_TIMING_TCSH                                              (_ADI_MSK(0x0F000000,uint32_t))  /* Chip Select Setup Time */
#define BITM_PDI_PDI_IF_TIMING_TWRL_RDL                                          (_ADI_MSK(0x00FF0000,uint32_t))  /* Low Duration for Write/Read Control Pulse */
#define BITM_PDI_PDI_IF_TIMING_TWRH_RDH                                          (_ADI_MSK(0x0000FF00,uint32_t))  /* High Duration for Write/Read Control Pulse */
#define BITM_PDI_PDI_IF_TIMING_TCSF                                              (_ADI_MSK(0x000000F0,uint32_t))  /* Chip Select Wait Time */
#define BITM_PDI_PDI_IF_TIMING_TAS_AH                                            (_ADI_MSK(0x0000000F,uint32_t))  /* Address Setup/Hold Time */

/* ==================================================
        AFE Register Map Registers
   ================================================== */

/* =========================
        AFE
   ========================= */
#define REG_AFE_AFE_CFG                      0x40080000         /* AFE AFE Configuration */
#define REG_AFE_AFE_SEQ_CFG                  0x40080004         /* AFE Sequencer Configuration */
#define REG_AFE_AFE_FIFO_CFG                 0x40080008         /* AFE FIFOs Configuration */
#define REG_AFE_AFE_SW_CFG                   0x4008000C         /* AFE Switch Matrix Configuration */
#define REG_AFE_AFE_DAC_CFG                  0x40080010         /* AFE DAC Configuration */
#define REG_AFE_AFE_WG_CFG                   0x40080014         /* AFE Waveform Generator Configuration */
#define REG_AFE_AFE_WG_DCLEVEL_1             0x40080018         /* AFE Waveform Generator - Trapezoid DC Level 1 */
#define REG_AFE_AFE_WG_DCLEVEL_2             0x4008001C         /* AFE Waveform Generator - Trapezoid DC Level 2 */
#define REG_AFE_AFE_WG_DELAY_1               0x40080020         /* AFE Waveform Generator - Trapezoid Delay 1 Time */
#define REG_AFE_AFE_WG_SLOPE_1               0x40080024         /* AFE Waveform Generator - Trapezoid Slope 1 Time */
#define REG_AFE_AFE_WG_DELAY_2               0x40080028         /* AFE Waveform Generator - Trapezoid Delay 2 Time */
#define REG_AFE_AFE_WG_SLOPE_2               0x4008002C         /* AFE Waveform Generator - Trapezoid Slope 2 Time */
#define REG_AFE_AFE_WG_FCW                   0x40080030         /* AFE Waveform Generator - Sinusoid Frequency Control Word */
#define REG_AFE_AFE_WG_PHASE                 0x40080034         /* AFE Waveform Generator - Sinusoid Phase Offset */
#define REG_AFE_AFE_WG_OFFSET                0x40080038         /* AFE Waveform Generator - Sinusoid Offset */
#define REG_AFE_AFE_WG_AMPLITUDE             0x4008003C         /* AFE Waveform Generator - Sinusoid Amplitude */
#define REG_AFE_AFE_ADC_CFG                  0x40080040         /* AFE ADC Configuration */
#define REG_AFE_AFE_SUPPLY_LPF_CFG           0x40080044         /* AFE Supply Rejection Filter Configuration */
#define REG_AFE_AFE_SW_FULL_CFG_MSB          0x40080048         /* AFE Switch Matrix Full Configuration (MSB) */
#define REG_AFE_AFE_SW_FULL_CFG_LSB          0x4008004C         /* AFE Switch Matrix Full Configuration (LSB) */
#define REG_AFE_AFE_WG_DAC_CODE              0x40080054         /* AFE Waveform Generator - DAC Code */
#define REG_AFE_AFE_STATUS                   0x40080058         /* AFE AFE Status */
#define REG_AFE_AFE_SEQ_CRC                  0x40080060         /* AFE Sequencer CRC Value */
#define REG_AFE_AFE_SEQ_COUNT                0x40080064         /* AFE Sequencer Command Count */
#define REG_AFE_AFE_SEQ_TIMEOUT              0x40080068         /* AFE Sequencer Timeout Counter */
#define REG_AFE_AFE_DATA_FIFO_READ           0x4008006C         /* AFE Data FIFO Read */
#define REG_AFE_AFE_CMD_FIFO_WRITE           0x40080070         /* AFE Command FIFO Write */
#define REG_AFE_AFE_ADC_RESULT               0x40080074         /* AFE ADC Raw Result */
#define REG_AFE_AFE_DFT_RESULT_REAL          0x40080078         /* AFE DFT Result, Real Part */
#define REG_AFE_AFE_DFT_RESULT_IMAG          0x4008007C         /* AFE DFT Result, Imaginary Part */
#define REG_AFE_AFE_SUPPLY_LPF_RESULT        0x40080080         /* AFE Supply Rejection Filter Result */
#define REG_AFE_AFE_TEMP_SENSOR_RESULT       0x40080084         /* AFE Temperature Sensor Result */
#define REG_AFE_AFE_ANALOG_CAPTURE_IEN       0x4008008C         /* AFE Analog Capture Interrupt Enable */
#define REG_AFE_AFE_ANALOG_GEN_IEN           0x40080090         /* AFE Analog Generation Interrupt Enable */
#define REG_AFE_AFE_CMD_FIFO_IEN             0x40080094         /* AFE Command FIFO Interrupt Enable */
#define REG_AFE_AFE_DATA_FIFO_IEN            0x40080098         /* AFE Data FIFO Interrupt Enable */
#define REG_AFE_AFE_ANALOG_CAPTURE_INT       0x400800A0         /* AFE Analog Capture Interrupt */
#define REG_AFE_AFE_ANALOG_GEN_INT           0x400800A4         /* AFE Analog Generation Interrupt */
#define REG_AFE_AFE_CMD_FIFO_INT             0x400800A8         /* AFE Command FIFO Interrupt */
#define REG_AFE_AFE_DATA_FIFO_INT            0x400800AC         /* AFE Data FIFO Interrupt */
#define REG_AFE_AFE_SW_STATUS_MSB            0x400800B0         /* AFE Switch Matrix Status (MSB) */
#define REG_AFE_AFE_SW_STATUS_LSB            0x400800B4         /* AFE Switch Matrix Status (LSB) */
#define REG_AFE_AFE_ADCMIN                   0x400800B8         /* AFE ADC Minimum Value Check */
#define REG_AFE_AFE_ADCMAX                   0x400800BC         /* AFE ADC Maximum Value Check */
#define REG_AFE_AFE_ADCDELTA                 0x400800C0         /* AFE ADC Delta Check */
#define REG_AFE_AFE_CAL_DATA_LOCK            0x40080100         /* AFE Calibration Data Lock */
#define REG_AFE_AFE_ADC_GAIN_TIA             0x40080104         /* AFE ADC Gain (TIA Measurement) */
#define REG_AFE_AFE_ADC_OFFSET_TIA           0x40080108         /* AFE ADC Offset (TIA Measurement) */
#define REG_AFE_AFE_ADC_GAIN_TEMP_SENS       0x4008010C         /* AFE ADC Gain (Temperature Sensor Measurement) */
#define REG_AFE_AFE_ADC_OFFSET_TEMP_SENS     0x40080110         /* AFE ADC Offset (Temperature Sensor Measurement) */
#define REG_AFE_AFE_ADC_GAIN_AUX             0x40080118         /* AFE ADC Gain (Aux Channel Measurement) */
#define REG_AFE_AFE_ADC_OFFSET_AUX           0x4008011C         /* AFE ADC Offset (Aux Channel Measurement) */
#define REG_AFE_AFE_DAC_OFFSET_UNITY         0x40080120         /* AFE DAC Offset With Attenuator Disabled */
#define REG_AFE_AFE_DAC_OFFSET_ATTEN         0x40080124         /* AFE DAC Offset With Attenuator Enabled */
#define REG_AFE_AFE_DAC_GAIN                 0x40080128         /* AFE DAC Gain */
#define REG_AFE_AFE_REF_TRIM0                0x4008012C         /* AFE Precision Reference Trim 0 */
#define REG_AFE_AFE_REF_TRIM1                0x40080130         /* AFE Precision Reference Trim 1 */
#define REG_AFE_AFE_ALDO_TRIM                0x40080134         /* AFE Analog LDO Trim */
#define REG_AFE_AFE_DAC_TRIM                 0x40080138         /* AFE DAC Trim */
#define REG_AFE_AFE_INAMP_TRIM               0x4008013C         /* AFE INAMP Trim */
#define REG_AFE_AFE_EXBUF_TRIM               0x40080140         /* AFE Excitation Buffer Trim */
#define REG_AFE_AFE_TEMP_SENS_TRIM           0x40080144         /* AFE Temperature Sensor Trim */

/* =========================
        AFE
   ========================= */
/* ------------------------------------------------------------------------------------------------------------------------
        AFE_AFE_CFG                                                              Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_AFE_AFE_CFG_ALDOILIMIT_EN                                           19
#define BITP_AFE_AFE_CFG_VREFBUFILIMIT_EN                                        18
#define BITP_AFE_AFE_CFG_VBIASBUF_EN                                             17
#define BITP_AFE_AFE_CFG_SUPPLY_LPF_EN                                           16
#define BITP_AFE_AFE_CFG_DFT_EN                                                  15
#define BITP_AFE_AFE_CFG_WAVEGEN_EN                                              14
#define BITP_AFE_AFE_CFG_TEMP_CONV_EN                                            13
#define BITP_AFE_AFE_CFG_TEMP_SENSOR_EN                                          12
#define BITP_AFE_AFE_CFG_TIA_EN                                                  11
#define BITP_AFE_AFE_CFG_INAMP_EN                                                10
#define BITP_AFE_AFE_CFG_BUF_EN                                                   9
#define BITP_AFE_AFE_CFG_ADC_CONV_EN                                              8
#define BITP_AFE_AFE_CFG_ADC_EN                                                   7
#define BITP_AFE_AFE_CFG_DAC_EN                                                   6
#define BITP_AFE_AFE_CFG_REF_EN                                                   5
#define BITP_AFE_AFE_CFG_ALDO_EN                                                  4
#define BITM_AFE_AFE_CFG_ALDOILIMIT_EN                                           (_ADI_MSK(0x00080000,uint32_t))
#define BITM_AFE_AFE_CFG_VREFBUFILIMIT_EN                                        (_ADI_MSK(0x00040000,uint32_t))
#define BITM_AFE_AFE_CFG_VBIASBUF_EN                                             (_ADI_MSK(0x00020000,uint32_t))
#define BITM_AFE_AFE_CFG_SUPPLY_LPF_EN                                           (_ADI_MSK(0x00010000,uint32_t))
#define BITM_AFE_AFE_CFG_DFT_EN                                                  (_ADI_MSK(0x00008000,uint32_t))
#define BITM_AFE_AFE_CFG_WAVEGEN_EN                                              (_ADI_MSK(0x00004000,uint32_t))
#define BITM_AFE_AFE_CFG_TEMP_CONV_EN                                            (_ADI_MSK(0x00002000,uint32_t))
#define BITM_AFE_AFE_CFG_TEMP_SENSOR_EN                                          (_ADI_MSK(0x00001000,uint32_t))
#define BITM_AFE_AFE_CFG_TIA_EN                                                  (_ADI_MSK(0x00000800,uint32_t))
#define BITM_AFE_AFE_CFG_INAMP_EN                                                (_ADI_MSK(0x00000400,uint32_t))
#define BITM_AFE_AFE_CFG_BUF_EN                                                  (_ADI_MSK(0x00000200,uint32_t))
#define BITM_AFE_AFE_CFG_ADC_CONV_EN                                             (_ADI_MSK(0x00000100,uint32_t))
#define BITM_AFE_AFE_CFG_ADC_EN                                                  (_ADI_MSK(0x00000080,uint32_t))
#define BITM_AFE_AFE_CFG_DAC_EN                                                  (_ADI_MSK(0x00000040,uint32_t))
#define BITM_AFE_AFE_CFG_REF_EN                                                  (_ADI_MSK(0x00000020,uint32_t))
#define BITM_AFE_AFE_CFG_ALDO_EN                                                 (_ADI_MSK(0x00000010,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        AFE_AFE_SEQ_CFG                                                          Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_AFE_AFE_SEQ_CFG_SEQ_WRITE_TIMER                                      8
#define BITP_AFE_AFE_SEQ_CFG_SEQ_HALT                                             4
#define BITP_AFE_AFE_SEQ_CFG_SEQ_STOP_ON_FIFO_EMPTY                               1
#define BITP_AFE_AFE_SEQ_CFG_SEQ_EN                                               0
#define BITM_AFE_AFE_SEQ_CFG_SEQ_WRITE_TIMER                                     (_ADI_MSK(0x0000FF00,uint32_t))
#define BITM_AFE_AFE_SEQ_CFG_SEQ_HALT                                            (_ADI_MSK(0x00000010,uint32_t))
#define BITM_AFE_AFE_SEQ_CFG_SEQ_STOP_ON_FIFO_EMPTY                              (_ADI_MSK(0x00000002,uint32_t))
#define BITM_AFE_AFE_SEQ_CFG_SEQ_EN                                              (_ADI_MSK(0x00000001,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        AFE_AFE_FIFO_CFG                                                         Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_AFE_AFE_FIFO_CFG_DATA_FIFO_SOURCE_SEL                               13
#define BITP_AFE_AFE_FIFO_CFG_DATA_FIFO_DMA_REQ_EN                               12
#define BITP_AFE_AFE_FIFO_CFG_DATA_FIFO_EN                                       11
#define BITP_AFE_AFE_FIFO_CFG_DATA_FIFO_THR_VAL                                   8
#define BITP_AFE_AFE_FIFO_CFG_CMD_FIFO_DMA_REQ_EN                                 4
#define BITP_AFE_AFE_FIFO_CFG_CMD_FIFO_EN                                         3
#define BITP_AFE_AFE_FIFO_CFG_CMD_FIFO_THR_VAL                                    0
#define BITM_AFE_AFE_FIFO_CFG_DATA_FIFO_SOURCE_SEL                               (_ADI_MSK(0x00006000,uint32_t))
#define BITM_AFE_AFE_FIFO_CFG_DATA_FIFO_DMA_REQ_EN                               (_ADI_MSK(0x00001000,uint32_t))
#define BITM_AFE_AFE_FIFO_CFG_DATA_FIFO_EN                                       (_ADI_MSK(0x00000800,uint32_t))
#define BITM_AFE_AFE_FIFO_CFG_DATA_FIFO_THR_VAL                                  (_ADI_MSK(0x00000700,uint32_t))
#define BITM_AFE_AFE_FIFO_CFG_CMD_FIFO_DMA_REQ_EN                                (_ADI_MSK(0x00000010,uint32_t))
#define BITM_AFE_AFE_FIFO_CFG_CMD_FIFO_EN                                        (_ADI_MSK(0x00000008,uint32_t))
#define BITM_AFE_AFE_FIFO_CFG_CMD_FIFO_THR_VAL                                   (_ADI_MSK(0x00000007,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        AFE_AFE_SW_CFG                                                           Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_AFE_AFE_SW_CFG_SW_SOURCE_SEL                                        17
#define BITP_AFE_AFE_SW_CFG_IVS_STATE                                            16
#define BITP_AFE_AFE_SW_CFG_TMUX_STATE                                           12
#define BITP_AFE_AFE_SW_CFG_NMUX_STATE                                            8
#define BITP_AFE_AFE_SW_CFG_PMUX_STATE                                            4
#define BITP_AFE_AFE_SW_CFG_DMUX_STATE                                            0
#define BITM_AFE_AFE_SW_CFG_SW_SOURCE_SEL                                        (_ADI_MSK(0x00020000,uint32_t))
#define BITM_AFE_AFE_SW_CFG_IVS_STATE                                            (_ADI_MSK(0x00010000,uint32_t))
#define BITM_AFE_AFE_SW_CFG_TMUX_STATE                                           (_ADI_MSK(0x0000F000,uint32_t))
#define BITM_AFE_AFE_SW_CFG_NMUX_STATE                                           (_ADI_MSK(0x00000F00,uint32_t))
#define BITM_AFE_AFE_SW_CFG_PMUX_STATE                                           (_ADI_MSK(0x000000F0,uint32_t))
#define BITM_AFE_AFE_SW_CFG_DMUX_STATE                                           (_ADI_MSK(0x0000000F,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        AFE_AFE_DAC_CFG                                                          Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_AFE_AFE_DAC_CFG_DAC_UPDATE_RATE                                      8
#define BITP_AFE_AFE_DAC_CFG_DAC_ATTEN_EN                                         0
#define BITM_AFE_AFE_DAC_CFG_DAC_UPDATE_RATE                                     (_ADI_MSK(0x00000F00,uint32_t))
#define BITM_AFE_AFE_DAC_CFG_DAC_ATTEN_EN                                        (_ADI_MSK(0x00000001,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        AFE_AFE_WG_CFG                                                           Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_AFE_AFE_WG_CFG_USE_DAC_GAIN                                          5
#define BITP_AFE_AFE_WG_CFG_USE_DAC_OFFSET                                        4
#define BITP_AFE_AFE_WG_CFG_TYPE_SEL                                              1
#define BITP_AFE_AFE_WG_CFG_WG_TRAP_RESET                                         0
#define BITM_AFE_AFE_WG_CFG_USE_DAC_GAIN                                         (_ADI_MSK(0x00000020,uint32_t))
#define BITM_AFE_AFE_WG_CFG_USE_DAC_OFFSET                                       (_ADI_MSK(0x00000010,uint32_t))
#define BITM_AFE_AFE_WG_CFG_TYPE_SEL                                             (_ADI_MSK(0x00000006,uint32_t))
#define BITM_AFE_AFE_WG_CFG_WG_TRAP_RESET                                        (_ADI_MSK(0x00000001,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        AFE_AFE_WG_DCLEVEL_1                                                     Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_AFE_AFE_WG_DCLEVEL_1_TRAP_DC_LEVEL_1                                 0
#define BITM_AFE_AFE_WG_DCLEVEL_1_TRAP_DC_LEVEL_1                                (_ADI_MSK(0x00000FFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        AFE_AFE_WG_DCLEVEL_2                                                     Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_AFE_AFE_WG_DCLEVEL_2_TRAP_DC_LEVEL_2                                 0
#define BITM_AFE_AFE_WG_DCLEVEL_2_TRAP_DC_LEVEL_2                                (_ADI_MSK(0x00000FFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        AFE_AFE_WG_DELAY_1                                                       Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_AFE_AFE_WG_DELAY_1_TRAP_DELAY_1                                      0
#define BITM_AFE_AFE_WG_DELAY_1_TRAP_DELAY_1                                     (_ADI_MSK(0x000FFFFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        AFE_AFE_WG_SLOPE_1                                                       Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_AFE_AFE_WG_SLOPE_1_TRAP_SLOPE_1                                      0
#define BITM_AFE_AFE_WG_SLOPE_1_TRAP_SLOPE_1                                     (_ADI_MSK(0x000FFFFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        AFE_AFE_WG_DELAY_2                                                       Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_AFE_AFE_WG_DELAY_2_TRAP_DELAY_2                                      0
#define BITM_AFE_AFE_WG_DELAY_2_TRAP_DELAY_2                                     (_ADI_MSK(0x000FFFFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        AFE_AFE_WG_SLOPE_2                                                       Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_AFE_AFE_WG_SLOPE_2_TRAP_SLOPE_2                                      0
#define BITM_AFE_AFE_WG_SLOPE_2_TRAP_SLOPE_2                                     (_ADI_MSK(0x000FFFFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        AFE_AFE_WG_FCW                                                           Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_AFE_AFE_WG_FCW_SINE_FCW                                              0
#define BITM_AFE_AFE_WG_FCW_SINE_FCW                                             (_ADI_MSK(0x000FFFFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        AFE_AFE_WG_PHASE                                                         Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_AFE_AFE_WG_PHASE_SINE_PHASE_OFFSET                                   0
#define BITM_AFE_AFE_WG_PHASE_SINE_PHASE_OFFSET                                  (_ADI_MSK(0x000FFFFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        AFE_AFE_WG_OFFSET                                                        Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_AFE_AFE_WG_OFFSET_SINE_OFFSET                                        0
#define BITM_AFE_AFE_WG_OFFSET_SINE_OFFSET                                       (_ADI_MSK(0x00000FFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        AFE_AFE_WG_AMPLITUDE                                                     Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_AFE_AFE_WG_AMPLITUDE_SINE_AMPLITUDE                                  0
#define BITM_AFE_AFE_WG_AMPLITUDE_SINE_AMPLITUDE                                 (_ADI_MSK(0x000007FF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        AFE_AFE_ADC_CFG                                                          Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_AFE_AFE_ADC_CFG_GAIN_OFFS_SEL                                        8
#define BITP_AFE_AFE_ADC_CFG_ANEXCITESW_EN                                        5
#define BITP_AFE_AFE_ADC_CFG_MUX_SEL                                              0
#define BITM_AFE_AFE_ADC_CFG_GAIN_OFFS_SEL                                       (_ADI_MSK(0x00000300,uint32_t))
#define BITM_AFE_AFE_ADC_CFG_ANEXCITESW_EN                                       (_ADI_MSK(0x00000020,uint32_t))
#define BITM_AFE_AFE_ADC_CFG_MUX_SEL                                             (_ADI_MSK(0x0000001F,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        AFE_AFE_SUPPLY_LPF_CFG                                                   Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_AFE_AFE_SUPPLY_LPF_CFG_BYPASS_SUPPLY_LPF                             0
#define BITM_AFE_AFE_SUPPLY_LPF_CFG_BYPASS_SUPPLY_LPF                            (_ADI_MSK(0x00000001,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        AFE_AFE_SW_FULL_CFG_MSB                                                  Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_AFE_AFE_SW_FULL_CFG_MSB_PL                                          16
#define BITP_AFE_AFE_SW_FULL_CFG_MSB_P8                                          15
#define BITP_AFE_AFE_SW_FULL_CFG_MSB_P7                                          14
#define BITP_AFE_AFE_SW_FULL_CFG_MSB_P6                                          13
#define BITP_AFE_AFE_SW_FULL_CFG_MSB_P5                                          12
#define BITP_AFE_AFE_SW_FULL_CFG_MSB_P4                                          11
#define BITP_AFE_AFE_SW_FULL_CFG_MSB_P3                                          10
#define BITP_AFE_AFE_SW_FULL_CFG_MSB_P2                                           9
#define BITP_AFE_AFE_SW_FULL_CFG_MSB_PR1                                          8
#define BITP_AFE_AFE_SW_FULL_CFG_MSB_D8                                           7
#define BITP_AFE_AFE_SW_FULL_CFG_MSB_D7                                           6
#define BITP_AFE_AFE_SW_FULL_CFG_MSB_D6                                           5
#define BITP_AFE_AFE_SW_FULL_CFG_MSB_D5                                           4
#define BITP_AFE_AFE_SW_FULL_CFG_MSB_D4                                           3
#define BITP_AFE_AFE_SW_FULL_CFG_MSB_D3                                           2
#define BITP_AFE_AFE_SW_FULL_CFG_MSB_D2                                           1
#define BITP_AFE_AFE_SW_FULL_CFG_MSB_DR1                                          0
#define BITM_AFE_AFE_SW_FULL_CFG_MSB_PL                                          (_ADI_MSK(0x00010000,uint32_t))
#define BITM_AFE_AFE_SW_FULL_CFG_MSB_P8                                          (_ADI_MSK(0x00008000,uint32_t))
#define BITM_AFE_AFE_SW_FULL_CFG_MSB_P7                                          (_ADI_MSK(0x00004000,uint32_t))
#define BITM_AFE_AFE_SW_FULL_CFG_MSB_P6                                          (_ADI_MSK(0x00002000,uint32_t))
#define BITM_AFE_AFE_SW_FULL_CFG_MSB_P5                                          (_ADI_MSK(0x00001000,uint32_t))
#define BITM_AFE_AFE_SW_FULL_CFG_MSB_P4                                          (_ADI_MSK(0x00000800,uint32_t))
#define BITM_AFE_AFE_SW_FULL_CFG_MSB_P3                                          (_ADI_MSK(0x00000400,uint32_t))
#define BITM_AFE_AFE_SW_FULL_CFG_MSB_P2                                          (_ADI_MSK(0x00000200,uint32_t))
#define BITM_AFE_AFE_SW_FULL_CFG_MSB_PR1                                         (_ADI_MSK(0x00000100,uint32_t))
#define BITM_AFE_AFE_SW_FULL_CFG_MSB_D8                                          (_ADI_MSK(0x00000080,uint32_t))
#define BITM_AFE_AFE_SW_FULL_CFG_MSB_D7                                          (_ADI_MSK(0x00000040,uint32_t))
#define BITM_AFE_AFE_SW_FULL_CFG_MSB_D6                                          (_ADI_MSK(0x00000020,uint32_t))
#define BITM_AFE_AFE_SW_FULL_CFG_MSB_D5                                          (_ADI_MSK(0x00000010,uint32_t))
#define BITM_AFE_AFE_SW_FULL_CFG_MSB_D4                                          (_ADI_MSK(0x00000008,uint32_t))
#define BITM_AFE_AFE_SW_FULL_CFG_MSB_D3                                          (_ADI_MSK(0x00000004,uint32_t))
#define BITM_AFE_AFE_SW_FULL_CFG_MSB_D2                                          (_ADI_MSK(0x00000002,uint32_t))
#define BITM_AFE_AFE_SW_FULL_CFG_MSB_DR1                                         (_ADI_MSK(0x00000001,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        AFE_AFE_SW_FULL_CFG_LSB                                                  Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_AFE_AFE_SW_FULL_CFG_LSB_IVS                                         17
#define BITP_AFE_AFE_SW_FULL_CFG_LSB_NL                                          16
#define BITP_AFE_AFE_SW_FULL_CFG_LSB_NR2                                         15
#define BITP_AFE_AFE_SW_FULL_CFG_LSB_N7                                          14
#define BITP_AFE_AFE_SW_FULL_CFG_LSB_N6                                          13
#define BITP_AFE_AFE_SW_FULL_CFG_LSB_N5                                          12
#define BITP_AFE_AFE_SW_FULL_CFG_LSB_N4                                          11
#define BITP_AFE_AFE_SW_FULL_CFG_LSB_N3                                          10
#define BITP_AFE_AFE_SW_FULL_CFG_LSB_N2                                           9
#define BITP_AFE_AFE_SW_FULL_CFG_LSB_N1                                           8
#define BITP_AFE_AFE_SW_FULL_CFG_LSB_TR2                                          7
#define BITP_AFE_AFE_SW_FULL_CFG_LSB_T7                                           6
#define BITP_AFE_AFE_SW_FULL_CFG_LSB_T6                                           5
#define BITP_AFE_AFE_SW_FULL_CFG_LSB_T5                                           4
#define BITP_AFE_AFE_SW_FULL_CFG_LSB_T4                                           3
#define BITP_AFE_AFE_SW_FULL_CFG_LSB_T3                                           2
#define BITP_AFE_AFE_SW_FULL_CFG_LSB_T2                                           1
#define BITP_AFE_AFE_SW_FULL_CFG_LSB_T1                                           0
#define BITM_AFE_AFE_SW_FULL_CFG_LSB_IVS                                         (_ADI_MSK(0x00020000,uint32_t))
#define BITM_AFE_AFE_SW_FULL_CFG_LSB_NL                                          (_ADI_MSK(0x00010000,uint32_t))
#define BITM_AFE_AFE_SW_FULL_CFG_LSB_NR2                                         (_ADI_MSK(0x00008000,uint32_t))
#define BITM_AFE_AFE_SW_FULL_CFG_LSB_N7                                          (_ADI_MSK(0x00004000,uint32_t))
#define BITM_AFE_AFE_SW_FULL_CFG_LSB_N6                                          (_ADI_MSK(0x00002000,uint32_t))
#define BITM_AFE_AFE_SW_FULL_CFG_LSB_N5                                          (_ADI_MSK(0x00001000,uint32_t))
#define BITM_AFE_AFE_SW_FULL_CFG_LSB_N4                                          (_ADI_MSK(0x00000800,uint32_t))
#define BITM_AFE_AFE_SW_FULL_CFG_LSB_N3                                          (_ADI_MSK(0x00000400,uint32_t))
#define BITM_AFE_AFE_SW_FULL_CFG_LSB_N2                                          (_ADI_MSK(0x00000200,uint32_t))
#define BITM_AFE_AFE_SW_FULL_CFG_LSB_N1                                          (_ADI_MSK(0x00000100,uint32_t))
#define BITM_AFE_AFE_SW_FULL_CFG_LSB_TR2                                         (_ADI_MSK(0x00000080,uint32_t))
#define BITM_AFE_AFE_SW_FULL_CFG_LSB_T7                                          (_ADI_MSK(0x00000040,uint32_t))
#define BITM_AFE_AFE_SW_FULL_CFG_LSB_T6                                          (_ADI_MSK(0x00000020,uint32_t))
#define BITM_AFE_AFE_SW_FULL_CFG_LSB_T5                                          (_ADI_MSK(0x00000010,uint32_t))
#define BITM_AFE_AFE_SW_FULL_CFG_LSB_T4                                          (_ADI_MSK(0x00000008,uint32_t))
#define BITM_AFE_AFE_SW_FULL_CFG_LSB_T3                                          (_ADI_MSK(0x00000004,uint32_t))
#define BITM_AFE_AFE_SW_FULL_CFG_LSB_T2                                          (_ADI_MSK(0x00000002,uint32_t))
#define BITM_AFE_AFE_SW_FULL_CFG_LSB_T1                                          (_ADI_MSK(0x00000001,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        AFE_AFE_WG_DAC_CODE                                                      Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_AFE_AFE_WG_DAC_CODE_DAC_CODE                                         0
#define BITM_AFE_AFE_WG_DAC_CODE_DAC_CODE                                        (_ADI_MSK(0x00000FFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        AFE_AFE_STATUS                                                           Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_AFE_AFE_STATUS_DATA_FIFO_UDF_STATUS                                 15
#define BITP_AFE_AFE_STATUS_DATA_FIFO_OVF_STATUS                                 14
#define BITP_AFE_AFE_STATUS_DATA_FIFO_THR_STATUS                                 13
#define BITP_AFE_AFE_STATUS_DATA_FIFO_EMPTY_STATUS                               12
#define BITP_AFE_AFE_STATUS_DATA_FIFO_FULL_STATUS                                11
#define BITP_AFE_AFE_STATUS_DATA_FIFO_WORD_COUNT                                  8
#define BITP_AFE_AFE_STATUS_CMD_FIFO_UDF_STATUS                                   7
#define BITP_AFE_AFE_STATUS_CMD_FIFO_OVF_STATUS                                   6
#define BITP_AFE_AFE_STATUS_CMD_FIFO_THR_STATUS                                   5
#define BITP_AFE_AFE_STATUS_CMD_FIFO_EMPTY_STATUS                                 4
#define BITP_AFE_AFE_STATUS_CMD_FIFO_FULL_STATUS                                  3
#define BITP_AFE_AFE_STATUS_CMD_FIFO_WORD_COUNT                                   0
#define BITM_AFE_AFE_STATUS_DATA_FIFO_UDF_STATUS                                 (_ADI_MSK(0x00008000,uint32_t))
#define BITM_AFE_AFE_STATUS_DATA_FIFO_OVF_STATUS                                 (_ADI_MSK(0x00004000,uint32_t))
#define BITM_AFE_AFE_STATUS_DATA_FIFO_THR_STATUS                                 (_ADI_MSK(0x00002000,uint32_t))
#define BITM_AFE_AFE_STATUS_DATA_FIFO_EMPTY_STATUS                               (_ADI_MSK(0x00001000,uint32_t))
#define BITM_AFE_AFE_STATUS_DATA_FIFO_FULL_STATUS                                (_ADI_MSK(0x00000800,uint32_t))
#define BITM_AFE_AFE_STATUS_DATA_FIFO_WORD_COUNT                                 (_ADI_MSK(0x00000700,uint32_t))
#define BITM_AFE_AFE_STATUS_CMD_FIFO_UDF_STATUS                                  (_ADI_MSK(0x00000080,uint32_t))
#define BITM_AFE_AFE_STATUS_CMD_FIFO_OVF_STATUS                                  (_ADI_MSK(0x00000040,uint32_t))
#define BITM_AFE_AFE_STATUS_CMD_FIFO_THR_STATUS                                  (_ADI_MSK(0x00000020,uint32_t))
#define BITM_AFE_AFE_STATUS_CMD_FIFO_EMPTY_STATUS                                (_ADI_MSK(0x00000010,uint32_t))
#define BITM_AFE_AFE_STATUS_CMD_FIFO_FULL_STATUS                                 (_ADI_MSK(0x00000008,uint32_t))
#define BITM_AFE_AFE_STATUS_CMD_FIFO_WORD_COUNT                                  (_ADI_MSK(0x00000007,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        AFE_AFE_SEQ_CRC                                                          Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_AFE_AFE_SEQ_CRC_SEQ_CRC                                              0
#define BITM_AFE_AFE_SEQ_CRC_SEQ_CRC                                             (_ADI_MSK(0x000000FF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        AFE_AFE_SEQ_COUNT                                                        Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_AFE_AFE_SEQ_COUNT_SEQ_COUNT                                          0
#define BITM_AFE_AFE_SEQ_COUNT_SEQ_COUNT                                         (_ADI_MSK(0x0000FFFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        AFE_AFE_SEQ_TIMEOUT                                                      Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_AFE_AFE_SEQ_TIMEOUT_SEQ_TIMEOUT                                      0
#define BITM_AFE_AFE_SEQ_TIMEOUT_SEQ_TIMEOUT                                     (_ADI_MSK(0x3FFFFFFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        AFE_AFE_DATA_FIFO_READ                                                   Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_AFE_AFE_DATA_FIFO_READ_DATA_FIFO_OUT                                 0
#define BITM_AFE_AFE_DATA_FIFO_READ_DATA_FIFO_OUT                                (_ADI_MSK(0x0000FFFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        AFE_AFE_ADC_RESULT                                                       Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_AFE_AFE_ADC_RESULT_ADC_RESULT                                        0
#define BITM_AFE_AFE_ADC_RESULT_ADC_RESULT                                       (_ADI_MSK(0x0000FFFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        AFE_AFE_DFT_RESULT_REAL                                                  Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_AFE_AFE_DFT_RESULT_REAL_DFT_RESULT_REAL                              0
#define BITM_AFE_AFE_DFT_RESULT_REAL_DFT_RESULT_REAL                             (_ADI_MSK(0x0000FFFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        AFE_AFE_DFT_RESULT_IMAG                                                  Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_AFE_AFE_DFT_RESULT_IMAG_DFT_RESULT_IMAG                              0
#define BITM_AFE_AFE_DFT_RESULT_IMAG_DFT_RESULT_IMAG                             (_ADI_MSK(0x0000FFFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        AFE_AFE_SUPPLY_LPF_RESULT                                                Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_AFE_AFE_SUPPLY_LPF_RESULT_SUPPLY_LPF_RESULT                          0
#define BITM_AFE_AFE_SUPPLY_LPF_RESULT_SUPPLY_LPF_RESULT                         (_ADI_MSK(0x0000FFFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        AFE_AFE_TEMP_SENSOR_RESULT                                               Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_AFE_AFE_TEMP_SENSOR_RESULT_TEMPERATURE                               0
#define BITM_AFE_AFE_TEMP_SENSOR_RESULT_TEMPERATURE                              (_ADI_MSK(0x0000FFFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        AFE_AFE_ANALOG_CAPTURE_IEN                                               Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_AFE_AFE_ANALOG_CAPTURE_IEN_ADC_DELTA_FAIL_IEN                        6
#define BITP_AFE_AFE_ANALOG_CAPTURE_IEN_ADC_MAX_FAIL_IEN                          5
#define BITP_AFE_AFE_ANALOG_CAPTURE_IEN_ADC_MIN_FAIL_IEN                          4
#define BITP_AFE_AFE_ANALOG_CAPTURE_IEN_TEMP_RESULT_READY_IEN                     3
#define BITP_AFE_AFE_ANALOG_CAPTURE_IEN_SUPPLY_LPF_RESULT_READY_IEN               2
#define BITP_AFE_AFE_ANALOG_CAPTURE_IEN_DFT_RESULT_READY_IEN                      1
#define BITP_AFE_AFE_ANALOG_CAPTURE_IEN_ADC_RESULT_READY_IEN                      0
#define BITM_AFE_AFE_ANALOG_CAPTURE_IEN_ADC_DELTA_FAIL_IEN                       (_ADI_MSK(0x00000040,uint32_t))
#define BITM_AFE_AFE_ANALOG_CAPTURE_IEN_ADC_MAX_FAIL_IEN                         (_ADI_MSK(0x00000020,uint32_t))
#define BITM_AFE_AFE_ANALOG_CAPTURE_IEN_ADC_MIN_FAIL_IEN                         (_ADI_MSK(0x00000010,uint32_t))
#define BITM_AFE_AFE_ANALOG_CAPTURE_IEN_TEMP_RESULT_READY_IEN                    (_ADI_MSK(0x00000008,uint32_t))
#define BITM_AFE_AFE_ANALOG_CAPTURE_IEN_SUPPLY_LPF_RESULT_READY_IEN              (_ADI_MSK(0x00000004,uint32_t))
#define BITM_AFE_AFE_ANALOG_CAPTURE_IEN_DFT_RESULT_READY_IEN                     (_ADI_MSK(0x00000002,uint32_t))
#define BITM_AFE_AFE_ANALOG_CAPTURE_IEN_ADC_RESULT_READY_IEN                     (_ADI_MSK(0x00000001,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        AFE_AFE_ANALOG_GEN_IEN                                                   Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_AFE_AFE_ANALOG_GEN_IEN_CUSTOM_INT_IEN                                3
#define BITP_AFE_AFE_ANALOG_GEN_IEN_BREAK_SEQUENCE_ORG_IEN                        2
#define BITP_AFE_AFE_ANALOG_GEN_IEN_HARDWARE_SETUP_DONE_IEN                       1
#define BITP_AFE_AFE_ANALOG_GEN_IEN_DELAY_COMMAND_DONE_IEN                        0
#define BITM_AFE_AFE_ANALOG_GEN_IEN_CUSTOM_INT_IEN                               (_ADI_MSK(0x00000008,uint32_t))
#define BITM_AFE_AFE_ANALOG_GEN_IEN_BREAK_SEQUENCE_ORG_IEN                       (_ADI_MSK(0x00000004,uint32_t))
#define BITM_AFE_AFE_ANALOG_GEN_IEN_HARDWARE_SETUP_DONE_IEN                      (_ADI_MSK(0x00000002,uint32_t))
#define BITM_AFE_AFE_ANALOG_GEN_IEN_DELAY_COMMAND_DONE_IEN                       (_ADI_MSK(0x00000001,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        AFE_AFE_CMD_FIFO_IEN                                                     Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_AFE_AFE_CMD_FIFO_IEN_CMD_FIFO_UDF_IEN                                7
#define BITP_AFE_AFE_CMD_FIFO_IEN_CMD_FIFO_OVF_IEN                                6
#define BITP_AFE_AFE_CMD_FIFO_IEN_CMD_FIFO_THR_IEN                                5
#define BITP_AFE_AFE_CMD_FIFO_IEN_CMD_FIFO_EMPTY_IEN                              4
#define BITP_AFE_AFE_CMD_FIFO_IEN_CMD_FIFO_FULL_IEN                               3
#define BITP_AFE_AFE_CMD_FIFO_IEN_SEQ_TIMEOUT_ERR_IEN                             2
#define BITP_AFE_AFE_CMD_FIFO_IEN_SEQ_TIMEOUT_FINISHED_IEN                        1
#define BITP_AFE_AFE_CMD_FIFO_IEN_END_OF_SEQ_IEN                                  0
#define BITM_AFE_AFE_CMD_FIFO_IEN_CMD_FIFO_UDF_IEN                               (_ADI_MSK(0x00000080,uint32_t))
#define BITM_AFE_AFE_CMD_FIFO_IEN_CMD_FIFO_OVF_IEN                               (_ADI_MSK(0x00000040,uint32_t))
#define BITM_AFE_AFE_CMD_FIFO_IEN_CMD_FIFO_THR_IEN                               (_ADI_MSK(0x00000020,uint32_t))
#define BITM_AFE_AFE_CMD_FIFO_IEN_CMD_FIFO_EMPTY_IEN                             (_ADI_MSK(0x00000010,uint32_t))
#define BITM_AFE_AFE_CMD_FIFO_IEN_CMD_FIFO_FULL_IEN                              (_ADI_MSK(0x00000008,uint32_t))
#define BITM_AFE_AFE_CMD_FIFO_IEN_SEQ_TIMEOUT_ERR_IEN                            (_ADI_MSK(0x00000004,uint32_t))
#define BITM_AFE_AFE_CMD_FIFO_IEN_SEQ_TIMEOUT_FINISHED_IEN                       (_ADI_MSK(0x00000002,uint32_t))
#define BITM_AFE_AFE_CMD_FIFO_IEN_END_OF_SEQ_IEN                                 (_ADI_MSK(0x00000001,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        AFE_AFE_DATA_FIFO_IEN                                                    Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_AFE_AFE_DATA_FIFO_IEN_DATA_FIFO_UDF_IEN                              4
#define BITP_AFE_AFE_DATA_FIFO_IEN_DATA_FIFO_OVF_IEN                              3
#define BITP_AFE_AFE_DATA_FIFO_IEN_DATA_FIFO_THR_IEN                              2
#define BITP_AFE_AFE_DATA_FIFO_IEN_DATA_FIFO_EMPTY_IEN                            1
#define BITP_AFE_AFE_DATA_FIFO_IEN_DATA_FIFO_FULL_IEN                             0
#define BITM_AFE_AFE_DATA_FIFO_IEN_DATA_FIFO_UDF_IEN                             (_ADI_MSK(0x00000010,uint32_t))
#define BITM_AFE_AFE_DATA_FIFO_IEN_DATA_FIFO_OVF_IEN                             (_ADI_MSK(0x00000008,uint32_t))
#define BITM_AFE_AFE_DATA_FIFO_IEN_DATA_FIFO_THR_IEN                             (_ADI_MSK(0x00000004,uint32_t))
#define BITM_AFE_AFE_DATA_FIFO_IEN_DATA_FIFO_EMPTY_IEN                           (_ADI_MSK(0x00000002,uint32_t))
#define BITM_AFE_AFE_DATA_FIFO_IEN_DATA_FIFO_FULL_IEN                            (_ADI_MSK(0x00000001,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        AFE_AFE_ANALOG_CAPTURE_INT                                               Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_AFE_AFE_ANALOG_CAPTURE_INT_ADC_DELTA_FAIL                            6
#define BITP_AFE_AFE_ANALOG_CAPTURE_INT_ADC_MAX_FAIL                              5
#define BITP_AFE_AFE_ANALOG_CAPTURE_INT_ADC_MIN_FAIL                              4
#define BITP_AFE_AFE_ANALOG_CAPTURE_INT_TEMP_RESULT_READY                         3
#define BITP_AFE_AFE_ANALOG_CAPTURE_INT_SUPPLY_LPF_RESULT_READY                   2
#define BITP_AFE_AFE_ANALOG_CAPTURE_INT_DFT_RESULT_READY                          1
#define BITP_AFE_AFE_ANALOG_CAPTURE_INT_ADC_RESULT_READY                          0
#define BITM_AFE_AFE_ANALOG_CAPTURE_INT_ADC_DELTA_FAIL                           (_ADI_MSK(0x00000040,uint32_t))
#define BITM_AFE_AFE_ANALOG_CAPTURE_INT_ADC_MAX_FAIL                             (_ADI_MSK(0x00000020,uint32_t))
#define BITM_AFE_AFE_ANALOG_CAPTURE_INT_ADC_MIN_FAIL                             (_ADI_MSK(0x00000010,uint32_t))
#define BITM_AFE_AFE_ANALOG_CAPTURE_INT_TEMP_RESULT_READY                        (_ADI_MSK(0x00000008,uint32_t))
#define BITM_AFE_AFE_ANALOG_CAPTURE_INT_SUPPLY_LPF_RESULT_READY                  (_ADI_MSK(0x00000004,uint32_t))
#define BITM_AFE_AFE_ANALOG_CAPTURE_INT_DFT_RESULT_READY                         (_ADI_MSK(0x00000002,uint32_t))
#define BITM_AFE_AFE_ANALOG_CAPTURE_INT_ADC_RESULT_READY                         (_ADI_MSK(0x00000001,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        AFE_AFE_ANALOG_GEN_INT                                                   Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_AFE_AFE_ANALOG_GEN_INT_CUSTOM_INT                                    3
#define BITP_AFE_AFE_ANALOG_GEN_INT_BREAK_SEQUENCE_ORG                            2
#define BITP_AFE_AFE_ANALOG_GEN_INT_HARDWARE_SETUP_DONE                           1
#define BITP_AFE_AFE_ANALOG_GEN_INT_DELAY_COMMAND_DONE                            0
#define BITM_AFE_AFE_ANALOG_GEN_INT_CUSTOM_INT                                   (_ADI_MSK(0x00000008,uint32_t))
#define BITM_AFE_AFE_ANALOG_GEN_INT_BREAK_SEQUENCE_ORG                           (_ADI_MSK(0x00000004,uint32_t))
#define BITM_AFE_AFE_ANALOG_GEN_INT_HARDWARE_SETUP_DONE                          (_ADI_MSK(0x00000002,uint32_t))
#define BITM_AFE_AFE_ANALOG_GEN_INT_DELAY_COMMAND_DONE                           (_ADI_MSK(0x00000001,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        AFE_AFE_CMD_FIFO_INT                                                     Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_AFE_AFE_CMD_FIFO_INT_CMD_FIFO_UDF                                    7
#define BITP_AFE_AFE_CMD_FIFO_INT_CMD_FIFO_OVF                                    6
#define BITP_AFE_AFE_CMD_FIFO_INT_CMD_FIFO_THR                                    5
#define BITP_AFE_AFE_CMD_FIFO_INT_CMD_FIFO_EMPTY                                  4
#define BITP_AFE_AFE_CMD_FIFO_INT_CMD_FIFO_FULL                                   3
#define BITP_AFE_AFE_CMD_FIFO_INT_SEQ_TIMEOUT_ERR                                 2
#define BITP_AFE_AFE_CMD_FIFO_INT_SEQ_TIMEOUT_FINISHED                            1
#define BITP_AFE_AFE_CMD_FIFO_INT_END_OF_SEQ                                      0
#define BITM_AFE_AFE_CMD_FIFO_INT_CMD_FIFO_UDF                                   (_ADI_MSK(0x00000080,uint32_t))
#define BITM_AFE_AFE_CMD_FIFO_INT_CMD_FIFO_OVF                                   (_ADI_MSK(0x00000040,uint32_t))
#define BITM_AFE_AFE_CMD_FIFO_INT_CMD_FIFO_THR                                   (_ADI_MSK(0x00000020,uint32_t))
#define BITM_AFE_AFE_CMD_FIFO_INT_CMD_FIFO_EMPTY                                 (_ADI_MSK(0x00000010,uint32_t))
#define BITM_AFE_AFE_CMD_FIFO_INT_CMD_FIFO_FULL                                  (_ADI_MSK(0x00000008,uint32_t))
#define BITM_AFE_AFE_CMD_FIFO_INT_SEQ_TIMEOUT_ERR                                (_ADI_MSK(0x00000004,uint32_t))
#define BITM_AFE_AFE_CMD_FIFO_INT_SEQ_TIMEOUT_FINISHED                           (_ADI_MSK(0x00000002,uint32_t))
#define BITM_AFE_AFE_CMD_FIFO_INT_END_OF_SEQ                                     (_ADI_MSK(0x00000001,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        AFE_AFE_DATA_FIFO_INT                                                    Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_AFE_AFE_DATA_FIFO_INT_DATA_FIFO_UDF                                  4
#define BITP_AFE_AFE_DATA_FIFO_INT_DATA_FIFO_OVF                                  3
#define BITP_AFE_AFE_DATA_FIFO_INT_DATA_FIFO_THR                                  2
#define BITP_AFE_AFE_DATA_FIFO_INT_DATA_FIFO_EMPTY                                1
#define BITP_AFE_AFE_DATA_FIFO_INT_DATA_FIFO_FULL                                 0
#define BITM_AFE_AFE_DATA_FIFO_INT_DATA_FIFO_UDF                                 (_ADI_MSK(0x00000010,uint32_t))
#define BITM_AFE_AFE_DATA_FIFO_INT_DATA_FIFO_OVF                                 (_ADI_MSK(0x00000008,uint32_t))
#define BITM_AFE_AFE_DATA_FIFO_INT_DATA_FIFO_THR                                 (_ADI_MSK(0x00000004,uint32_t))
#define BITM_AFE_AFE_DATA_FIFO_INT_DATA_FIFO_EMPTY                               (_ADI_MSK(0x00000002,uint32_t))
#define BITM_AFE_AFE_DATA_FIFO_INT_DATA_FIFO_FULL                                (_ADI_MSK(0x00000001,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        AFE_AFE_SW_STATUS_MSB                                                    Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_AFE_AFE_SW_STATUS_MSB_PL_STATUS                                     16
#define BITP_AFE_AFE_SW_STATUS_MSB_P8_STATUS                                     15
#define BITP_AFE_AFE_SW_STATUS_MSB_P7_STATUS                                     14
#define BITP_AFE_AFE_SW_STATUS_MSB_P6_STATUS                                     13
#define BITP_AFE_AFE_SW_STATUS_MSB_P5_STATUS                                     12
#define BITP_AFE_AFE_SW_STATUS_MSB_P4_STATUS                                     11
#define BITP_AFE_AFE_SW_STATUS_MSB_P3_STATUS                                     10
#define BITP_AFE_AFE_SW_STATUS_MSB_P2_STATUS                                      9
#define BITP_AFE_AFE_SW_STATUS_MSB_PR1_STATUS                                     8
#define BITP_AFE_AFE_SW_STATUS_MSB_D8_STATUS                                      7
#define BITP_AFE_AFE_SW_STATUS_MSB_D7_STATUS                                      6
#define BITP_AFE_AFE_SW_STATUS_MSB_D6_STATUS                                      5
#define BITP_AFE_AFE_SW_STATUS_MSB_D5_STATUS                                      4
#define BITP_AFE_AFE_SW_STATUS_MSB_D4_STATUS                                      3
#define BITP_AFE_AFE_SW_STATUS_MSB_D3_STATUS                                      2
#define BITP_AFE_AFE_SW_STATUS_MSB_D2_STATUS                                      1
#define BITP_AFE_AFE_SW_STATUS_MSB_DR1_STATUS                                     0
#define BITM_AFE_AFE_SW_STATUS_MSB_PL_STATUS                                     (_ADI_MSK(0x00010000,uint32_t))
#define BITM_AFE_AFE_SW_STATUS_MSB_P8_STATUS                                     (_ADI_MSK(0x00008000,uint32_t))
#define BITM_AFE_AFE_SW_STATUS_MSB_P7_STATUS                                     (_ADI_MSK(0x00004000,uint32_t))
#define BITM_AFE_AFE_SW_STATUS_MSB_P6_STATUS                                     (_ADI_MSK(0x00002000,uint32_t))
#define BITM_AFE_AFE_SW_STATUS_MSB_P5_STATUS                                     (_ADI_MSK(0x00001000,uint32_t))
#define BITM_AFE_AFE_SW_STATUS_MSB_P4_STATUS                                     (_ADI_MSK(0x00000800,uint32_t))
#define BITM_AFE_AFE_SW_STATUS_MSB_P3_STATUS                                     (_ADI_MSK(0x00000400,uint32_t))
#define BITM_AFE_AFE_SW_STATUS_MSB_P2_STATUS                                     (_ADI_MSK(0x00000200,uint32_t))
#define BITM_AFE_AFE_SW_STATUS_MSB_PR1_STATUS                                    (_ADI_MSK(0x00000100,uint32_t))
#define BITM_AFE_AFE_SW_STATUS_MSB_D8_STATUS                                     (_ADI_MSK(0x00000080,uint32_t))
#define BITM_AFE_AFE_SW_STATUS_MSB_D7_STATUS                                     (_ADI_MSK(0x00000040,uint32_t))
#define BITM_AFE_AFE_SW_STATUS_MSB_D6_STATUS                                     (_ADI_MSK(0x00000020,uint32_t))
#define BITM_AFE_AFE_SW_STATUS_MSB_D5_STATUS                                     (_ADI_MSK(0x00000010,uint32_t))
#define BITM_AFE_AFE_SW_STATUS_MSB_D4_STATUS                                     (_ADI_MSK(0x00000008,uint32_t))
#define BITM_AFE_AFE_SW_STATUS_MSB_D3_STATUS                                     (_ADI_MSK(0x00000004,uint32_t))
#define BITM_AFE_AFE_SW_STATUS_MSB_D2_STATUS                                     (_ADI_MSK(0x00000002,uint32_t))
#define BITM_AFE_AFE_SW_STATUS_MSB_DR1_STATUS                                    (_ADI_MSK(0x00000001,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        AFE_AFE_SW_STATUS_LSB                                                    Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_AFE_AFE_SW_STATUS_LSB_IVS_STATUS                                    17
#define BITP_AFE_AFE_SW_STATUS_LSB_NL_STATUS                                     16
#define BITP_AFE_AFE_SW_STATUS_LSB_NR2_STATUS                                    15
#define BITP_AFE_AFE_SW_STATUS_LSB_N7_STATUS                                     14
#define BITP_AFE_AFE_SW_STATUS_LSB_N6_STATUS                                     13
#define BITP_AFE_AFE_SW_STATUS_LSB_N5_STATUS                                     12
#define BITP_AFE_AFE_SW_STATUS_LSB_N4_STATUS                                     11
#define BITP_AFE_AFE_SW_STATUS_LSB_N3_STATUS                                     10
#define BITP_AFE_AFE_SW_STATUS_LSB_N2_STATUS                                      9
#define BITP_AFE_AFE_SW_STATUS_LSB_N1_STATUS                                      8
#define BITP_AFE_AFE_SW_STATUS_LSB_TR2_STATUS                                     7
#define BITP_AFE_AFE_SW_STATUS_LSB_T7_STATUS                                      6
#define BITP_AFE_AFE_SW_STATUS_LSB_T6_STATUS                                      5
#define BITP_AFE_AFE_SW_STATUS_LSB_T5_STATUS                                      4
#define BITP_AFE_AFE_SW_STATUS_LSB_T4_STATUS                                      3
#define BITP_AFE_AFE_SW_STATUS_LSB_T3_STATUS                                      2
#define BITP_AFE_AFE_SW_STATUS_LSB_T2_STATUS                                      1
#define BITP_AFE_AFE_SW_STATUS_LSB_T1_STATUS                                      0
#define BITM_AFE_AFE_SW_STATUS_LSB_IVS_STATUS                                    (_ADI_MSK(0x00020000,uint32_t))
#define BITM_AFE_AFE_SW_STATUS_LSB_NL_STATUS                                     (_ADI_MSK(0x00010000,uint32_t))
#define BITM_AFE_AFE_SW_STATUS_LSB_NR2_STATUS                                    (_ADI_MSK(0x00008000,uint32_t))
#define BITM_AFE_AFE_SW_STATUS_LSB_N7_STATUS                                     (_ADI_MSK(0x00004000,uint32_t))
#define BITM_AFE_AFE_SW_STATUS_LSB_N6_STATUS                                     (_ADI_MSK(0x00002000,uint32_t))
#define BITM_AFE_AFE_SW_STATUS_LSB_N5_STATUS                                     (_ADI_MSK(0x00001000,uint32_t))
#define BITM_AFE_AFE_SW_STATUS_LSB_N4_STATUS                                     (_ADI_MSK(0x00000800,uint32_t))
#define BITM_AFE_AFE_SW_STATUS_LSB_N3_STATUS                                     (_ADI_MSK(0x00000400,uint32_t))
#define BITM_AFE_AFE_SW_STATUS_LSB_N2_STATUS                                     (_ADI_MSK(0x00000200,uint32_t))
#define BITM_AFE_AFE_SW_STATUS_LSB_N1_STATUS                                     (_ADI_MSK(0x00000100,uint32_t))
#define BITM_AFE_AFE_SW_STATUS_LSB_TR2_STATUS                                    (_ADI_MSK(0x00000080,uint32_t))
#define BITM_AFE_AFE_SW_STATUS_LSB_T7_STATUS                                     (_ADI_MSK(0x00000040,uint32_t))
#define BITM_AFE_AFE_SW_STATUS_LSB_T6_STATUS                                     (_ADI_MSK(0x00000020,uint32_t))
#define BITM_AFE_AFE_SW_STATUS_LSB_T5_STATUS                                     (_ADI_MSK(0x00000010,uint32_t))
#define BITM_AFE_AFE_SW_STATUS_LSB_T4_STATUS                                     (_ADI_MSK(0x00000008,uint32_t))
#define BITM_AFE_AFE_SW_STATUS_LSB_T3_STATUS                                     (_ADI_MSK(0x00000004,uint32_t))
#define BITM_AFE_AFE_SW_STATUS_LSB_T2_STATUS                                     (_ADI_MSK(0x00000002,uint32_t))
#define BITM_AFE_AFE_SW_STATUS_LSB_T1_STATUS                                     (_ADI_MSK(0x00000001,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        AFE_AFE_ADCMIN                                                           Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_AFE_AFE_ADCMIN_ADC_MIN                                               0
#define BITM_AFE_AFE_ADCMIN_ADC_MIN                                              (_ADI_MSK(0x0000FFFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        AFE_AFE_ADCMAX                                                           Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_AFE_AFE_ADCMAX_ADC_MAX                                               0
#define BITM_AFE_AFE_ADCMAX_ADC_MAX                                              (_ADI_MSK(0x0000FFFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        AFE_AFE_ADCDELTA                                                         Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_AFE_AFE_ADCDELTA_ADC_DELTA                                           0
#define BITM_AFE_AFE_ADCDELTA_ADC_DELTA                                          (_ADI_MSK(0x0000FFFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        AFE_AFE_ADC_GAIN_TIA                                                   Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_AFE_AFE_ADC_GAIN_TIA_ADC_GAIN_TIA                                0
#define BITM_AFE_AFE_ADC_GAIN_TIA_ADC_GAIN_TIA                               (_ADI_MSK(0x00007FFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        AFE_AFE_ADC_OFFSET_TIA                                                 Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_AFE_AFE_ADC_OFFSET_TIA_ADC_OFFSET_TIA                            0
#define BITM_AFE_AFE_ADC_OFFSET_TIA_ADC_OFFSET_TIA                           (_ADI_MSK(0x00007FFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        AFE_AFE_ADC_GAIN_TEMP_SENS                                               Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_AFE_AFE_ADC_GAIN_TEMP_SENS_ADC_GAIN_TEMP_SENS                        0
#define BITM_AFE_AFE_ADC_GAIN_TEMP_SENS_ADC_GAIN_TEMP_SENS                       (_ADI_MSK(0x00007FFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        AFE_AFE_ADC_OFFSET_TEMP_SENS                                             Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_AFE_AFE_ADC_OFFSET_TEMP_SENS_ADC_OFFSET_TEMP_SENS                    0
#define BITM_AFE_AFE_ADC_OFFSET_TEMP_SENS_ADC_OFFSET_TEMP_SENS                   (_ADI_MSK(0x00007FFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        AFE_AFE_ADC_GAIN_AUX                                                     Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_AFE_AFE_ADC_GAIN_AUX_ADC_GAIN_AUX                                    0
#define BITM_AFE_AFE_ADC_GAIN_AUX_ADC_GAIN_AUX                                   (_ADI_MSK(0x00007FFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        AFE_AFE_ADC_OFFSET_AUX                                                   Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_AFE_AFE_ADC_OFFSET_AUX_ADC_OFFSET_AUX                                0
#define BITM_AFE_AFE_ADC_OFFSET_AUX_ADC_OFFSET_AUX                               (_ADI_MSK(0x00007FFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        AFE_AFE_DAC_OFFSET_UNITY                                                 Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_AFE_AFE_DAC_OFFSET_UNITY_DAC_OFFSET_UNITY                            0
#define BITM_AFE_AFE_DAC_OFFSET_UNITY_DAC_OFFSET_UNITY                           (_ADI_MSK(0x00000FFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        AFE_AFE_DAC_OFFSET_ATTEN                                                 Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_AFE_AFE_DAC_OFFSET_ATTEN_DAC_OFFSET_ATTEN                            0
#define BITM_AFE_AFE_DAC_OFFSET_ATTEN_DAC_OFFSET_ATTEN                           (_ADI_MSK(0x00000FFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        AFE_AFE_DAC_GAIN                                                         Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_AFE_AFE_DAC_GAIN_DAC_GAIN                                            0
#define BITM_AFE_AFE_DAC_GAIN_DAC_GAIN                                           (_ADI_MSK(0x00000FFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        AFE_AFE_REF_TRIM0                                                        Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_AFE_AFE_REF_TRIM0_VBIASABSTRIM                                       6
#define BITP_AFE_AFE_REF_TRIM0_ITATTRIM                                           0
#define BITM_AFE_AFE_REF_TRIM0_VBIASABSTRIM                                      (_ADI_MSK(0x00001FC0,uint32_t))
#define BITM_AFE_AFE_REF_TRIM0_ITATTRIM                                          (_ADI_MSK(0x0000003F,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        AFE_AFE_REF_TRIM1                                                        Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_AFE_AFE_REF_TRIM1_REFTC                                             12
#define BITP_AFE_AFE_REF_TRIM1_REFCP                                              8
#define BITP_AFE_AFE_REF_TRIM1_VREFABSTRIM                                        0
#define BITM_AFE_AFE_REF_TRIM1_REFTC                                             (_ADI_MSK(0x000FF000,uint32_t))
#define BITM_AFE_AFE_REF_TRIM1_REFCP                                             (_ADI_MSK(0x00000F00,uint32_t))
#define BITM_AFE_AFE_REF_TRIM1_VREFABSTRIM                                       (_ADI_MSK(0x000000FF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        AFE_AFE_ALDO_TRIM                                                        Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_AFE_AFE_ALDO_TRIM_AVDDADJ                                            0
#define BITM_AFE_AFE_ALDO_TRIM_AVDDADJ                                           (_ADI_MSK(0x0000000F,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        AFE_AFE_DAC_TRIM                                                         Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_AFE_AFE_DAC_TRIM_DACRCFOFFSETEN                                      8
#define BITP_AFE_AFE_DAC_TRIM_DACRCFCAL                                           0
#define BITM_AFE_AFE_DAC_TRIM_DACRCFOFFSETEN                                     (_ADI_MSK(0x00000100,uint32_t))
#define BITM_AFE_AFE_DAC_TRIM_DACRCFCAL                                          (_ADI_MSK(0x0000003F,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        AFE_AFE_INAMP_TRIM                                                       Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_AFE_AFE_INAMP_TRIM_EXCITELOOPCONFIG                                  6
#define BITP_AFE_AFE_INAMP_TRIM_INAMPOFFSETADJ                                    0
#define BITM_AFE_AFE_INAMP_TRIM_EXCITELOOPCONFIG                                 (_ADI_MSK(0x00007FC0,uint32_t))
#define BITM_AFE_AFE_INAMP_TRIM_INAMPOFFSETADJ                                   (_ADI_MSK(0x0000003F,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        AFE_AFE_EXBUF_TRIM                                                       Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_AFE_AFE_EXBUF_TRIM_EXBUFZEROCONTROL                                 11
#define BITP_AFE_AFE_EXBUF_TRIM_EXBUFGAINCONTROL                                  5
#define BITP_AFE_AFE_EXBUF_TRIM_EXBUFOFFSETADJ                                    0
#define BITM_AFE_AFE_EXBUF_TRIM_EXBUFZEROCONTROL                                 (_ADI_MSK(0x0001F800,uint32_t))
#define BITM_AFE_AFE_EXBUF_TRIM_EXBUFGAINCONTROL                                 (_ADI_MSK(0x000007E0,uint32_t))
#define BITM_AFE_AFE_EXBUF_TRIM_EXBUFOFFSETADJ                                   (_ADI_MSK(0x0000001F,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        AFE_AFE_TEMP_SENS_TRIM                                                   Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_AFE_AFE_TEMP_SENS_TRIM_TEMP_SENS_GAIN                                0
#define BITM_AFE_AFE_TEMP_SENS_TRIM_TEMP_SENS_GAIN                               (_ADI_MSK(0x00007FFF,uint32_t))

/* ==================================================
        Captouch Register Map Registers
   ================================================== */

/* =========================
        CT
   ========================= */
#define REG_CT_CT_CDC_PWR                    0x40084000         /* CT Power CDC control */
#define REG_CT_CT_CFG1                       0x40084004         /* CT CapTouch Control Configuration Register 1 */
#define REG_CT_CT_CFG2                       0x40084008         /* CT CapTouch Control Configuration Register 2 */
#define REG_CT_CT_CFG3                       0x4008400C         /* CT AFE Control Configuration Register 3 */
#define REG_CT_CT_MEAS_SEL                   0x40084010         /* CT Capacitance measurement stage selection */
#define REG_CT_CT_BASELINE_CTRL              0x40084014         /* CT Baseline control settings */
#define REG_CT_CT_AVG                        0x40084018         /* CT Setup of the rank-and-average filtering */
#define REG_CT_CT_CAL_EN                     0x4008401C         /* CT Enable calibration for measurement stages */
#define REG_CT_CT_TOUCH_CFG1                 0x40084020         /* CT Touch Detection Thresholds */
#define REG_CT_CT_TOUCH_CFG2                 0x40084024         /* CT Touch Detection time-out */
#define REG_CT_CT_RELEASE_CFG1               0x40084028         /* CT Release Detection Thresholds */
#define REG_CT_CT_RELEASE_CFG2               0x4008402C         /* CT Release Detection time-out */
#define REG_CT_CT_IEN                        0x40084030         /* CT Interrupt Enable */
#define REG_CT_CT_INT                        0x40084034         /* CT Primary Interrupt Register */
#define REG_CT_CT_OFFS_HSTAT                 0x40084038         /* CT Stage offset high interrupt status */
#define REG_CT_CT_OFFS_LSTAT                 0x4008403C         /* CT Stage offset low interrupt status */
#define REG_CT_CT_PROX_STAT                  0x40084040         /* CT Stage proximity status */
#define REG_CT_CT_FPROX_STAT                 0x40084044         /* CT Stage fast proximity status */
#define REG_CT_CT_TOUCH_STAT                 0x40084048         /* CT Stage touch/release status */
#define REG_CT_CT_STAGE0_CFG                 0x4008404C         /* CT Stage 0 configuration */
#define REG_CT_CT_STAGE1_CFG                 0x40084050         /* CT Stage 1 configuration */
#define REG_CT_CT_STAGE2_CFG                 0x40084054         /* CT Stage 2 configuration */
#define REG_CT_CT_STAGE3_CFG                 0x40084058         /* CT Stage 3 configuration */
#define REG_CT_CT_STAGE4_CFG                 0x4008405C         /* CT Stage 4 configuration */
#define REG_CT_CT_STAGE5_CFG                 0x40084060         /* CT Stage 5 configuration */
#define REG_CT_CT_STAGE6_CFG                 0x40084064         /* CT Stage 6 configuration */
#define REG_CT_CT_STAGE7_CFG                 0x40084068         /* CT Stage 7 configuration */
#define REG_CT_CT_STAGE8_CFG                 0x4008406C         /* CT Stage 8 configuration */
#define REG_CT_CT_STAGE9_CFG                 0x40084070         /* CT Stage 9 configuration */
#define REG_CT_CT_STAGE10_CFG                0x40084074         /* CT Stage 10 configuration */
#define REG_CT_CT_STAGE11_CFG                0x40084078         /* CT Stage 11 configuration */
#define REG_CT_CT_STAGE12_CFG                0x4008407C         /* CT Stage 12 configuration */
#define REG_CT_CT_STAGE13_CFG                0x40084080         /* CT Stage 13 configuration */
#define REG_CT_CT_STAGE14_CFG                0x40084084         /* CT Stage 14 configuration */
#define REG_CT_CT_STAGE15_CFG                0x40084088         /* CT Stage 15 configuration */
#define REG_CT_CT_SENSOR_THR_CFG0            0x4008408C         /* CT Stage 0 and Stage 1 Sensor Threshold */
#define REG_CT_CT_SENSOR_THR_CFG1            0x40084090         /* CT Stage 2 and Stage 3 Sensor Threshold */
#define REG_CT_CT_SENSOR_THR_CFG2            0x40084094         /* CT Stage 4 and Stage 5 Sensor Threshold */
#define REG_CT_CT_SENSOR_THR_CFG3            0x40084098         /* CT Stage 6 and Stage 7 Sensor Threshold */
#define REG_CT_CT_SENSOR_THR_CFG4            0x4008409C         /* CT Stage 8 and Stage 9 Sensor Threshold */
#define REG_CT_CT_SENSOR_THR_CFG5            0x400840A0         /* CT Stage 10 and Stage 11 Sensor Threshold */
#define REG_CT_CT_SENSOR_THR_CFG6            0x400840A4         /* CT Stage 12 and Stage 13 Sensor Threshold */
#define REG_CT_CT_SENSOR_THR_CFG7            0x400840A8         /* CT Stage 14 and Stage 15 Sensor Threshold */
#define REG_CT_CT_CDC_RES0                   0x400840AC         /* CT Stage 0 and Stage 1 Results */
#define REG_CT_CT_CDC_RES1                   0x400840B0         /* CT Stage 2 and Stage 3 Results */
#define REG_CT_CT_CDC_RES2                   0x400840B4         /* CT Stage 4 and Stage 5 Results */
#define REG_CT_CT_CDC_RES3                   0x400840B8         /* CT Stage 6 and Stage 7 Results */
#define REG_CT_CT_CDC_RES4                   0x400840BC         /* CT Stage 8 and Stage 9 Results */
#define REG_CT_CT_CDC_RES5                   0x400840C0         /* CT Stage 10 and Stage 11 Results */
#define REG_CT_CT_CDC_RES6                   0x400840C4         /* CT Stage 12 and Stage 13 Results */
#define REG_CT_CT_CDC_RES7                   0x400840C8         /* CT Stage 14 and Stage 15 Results */
#define REG_CT_CT_BASELINE0                  0x400840CC         /* CT Stage 0 Fast Filter and Baseline Results */
#define REG_CT_CT_BASELINE1                  0x400840D0         /* CT Stage 1 Fast Filter and Baseline Results */
#define REG_CT_CT_BASELINE2                  0x400840D4         /* CT Stage 2 Fast Filter and Baseline Results */
#define REG_CT_CT_BASELINE3                  0x400840D8         /* CT Stage 3 Fast Filter and Baseline Results */
#define REG_CT_CT_BASELINE4                  0x400840DC         /* CT Stage 4 Fast Filter and Baseline Results */
#define REG_CT_CT_BASELINE5                  0x400840E0         /* CT Stage 5 Fast Filter and Baseline Results */
#define REG_CT_CT_BASELINE6                  0x400840E4         /* CT Stage 6 Fast Filter and Baseline Results */
#define REG_CT_CT_BASELINE7                  0x400840E8         /* CT Stage 7 Fast Filter and Baseline Results */
#define REG_CT_CT_BASELINE8                  0x400840EC         /* CT Stage 8 Fast Filter and Baseline Results */
#define REG_CT_CT_BASELINE9                  0x400840F0         /* CT Stage 9 Fast Filter and Baseline Results */
#define REG_CT_CT_BASELINE10                 0x400840F4         /* CT Stage 10 Fast Filter and Baseline Results */
#define REG_CT_CT_BASELINE11                 0x400840F8         /* CT Stage 11 Fast Filter and Baseline Results */
#define REG_CT_CT_BASELINE12                 0x400840FC         /* CT Stage 12 Fast Filter and Baseline Results */
#define REG_CT_CT_BASELINE13                 0x40084100         /* CT Stage 13 Fast Filter and Baseline Results */
#define REG_CT_CT_BASELINE14                 0x40084104         /* CT Stage 14 Fast Filter and Baseline Results */
#define REG_CT_CT_BASELINE15                 0x40084108         /* CT Stage 15 Fast Filter and Baseline Results */
#define REG_CT_CT_PK2PK0                     0x4008410C         /* CT Stage 0 and Stage 1 Peak-to-Peak Noise Results */
#define REG_CT_CT_PK2PK1                     0x40084110         /* CT Stage 2 and Stage 3 Peak-to-Peak Noise Results */
#define REG_CT_CT_PK2PK2                     0x40084114         /* CT Stage 4 and Stage 5 Peak-to-Peak Noise Results */
#define REG_CT_CT_PK2PK3                     0x40084118         /* CT Stage 6 and Stage 7 Peak-to-Peak Noise Results */
#define REG_CT_CT_PK2PK4                     0x4008411C         /* CT Stage 8 and Stage 9 Peak-to-Peak Noise Results */
#define REG_CT_CT_PK2PK5                     0x40084120         /* CT Stage 10 and Stage 11 Peak-to-Peak Noise Results */
#define REG_CT_CT_PK2PK6                     0x40084124         /* CT Stage 12 and Stage 13 Peak-to-Peak Noise Results */
#define REG_CT_CT_PK2PK7                     0x40084128         /* CT Stage 14 and Stage 15 Peak-to-Peak Noise Results */

/* =========================
        CT
   ========================= */
/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_CDC_PWR                                                            Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_CDC_PWR_BASLINE_UPDATED                                       11                               /* Baseline has been updated since previous read. */
#define BITP_CT_CT_CDC_PWR_UPD_COUNT                                              7                               /* Readback proximity update count. */
#define BITP_CT_CT_CDC_PWR_STOP_BASELINE_UPD                                      6
#define BITP_CT_CT_CDC_PWR_PROX_SW                                                5                               /* Force a proximity detection */
#define BITP_CT_CT_CDC_PWR_SW_STATUS                                              3                               /* Sequencer busy when high. */
#define BITP_CT_CT_CDC_PWR_SW_START_SEQ                                           1                               /* Start a single sequence - measure all configured sensors. */
#define BITP_CT_CT_CDC_PWR_PWR_MODE                                               0                               /* Force low-power mode. */
#define BITM_CT_CT_CDC_PWR_BASLINE_UPDATED                                       (_ADI_MSK(0x00000800,uint32_t))  /* Baseline has been updated since previous read. */
#define BITM_CT_CT_CDC_PWR_UPD_COUNT                                             (_ADI_MSK(0x00000780,uint32_t))  /* Readback proximity update count. */
#define BITM_CT_CT_CDC_PWR_STOP_BASELINE_UPD                                     (_ADI_MSK(0x00000040,uint32_t))
#define BITM_CT_CT_CDC_PWR_PROX_SW                                               (_ADI_MSK(0x00000020,uint32_t))  /* Force a proximity detection */
#define BITM_CT_CT_CDC_PWR_SW_STATUS                                             (_ADI_MSK(0x00000008,uint32_t))  /* Sequencer busy when high. */
#define BITM_CT_CT_CDC_PWR_SW_START_SEQ                                          (_ADI_MSK(0x00000002,uint32_t))  /* Start a single sequence - measure all configured sensors. */
#define BITM_CT_CT_CDC_PWR_PWR_MODE                                              (_ADI_MSK(0x00000001,uint32_t))  /* Force low-power mode. */

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_CFG1                                                               Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_CFG1_INT_CYCLE                                                24                               /* Num CtoV integration cycles */
#define BITP_CT_CT_CFG1_AIN_SEL                                                  22                               /* Drive unmatched pads. */
#define BITP_CT_CT_CFG1_AUTOZERO_DUTY                                            10                               /* Set CtoV autozero duty-cycle. */
#define BITP_CT_CT_CFG1_BYPASS_GAIN                                               8                               /* Override CT_STAGEn[23] */
#define BITP_CT_CT_CFG1_INT_BUFFER                                                6                               /* Defaults to 3uA */
#define BITP_CT_CT_CFG1_PGA_BIAS                                                  3                               /* Defaults to 5uA */
#define BITP_CT_CT_CFG1_C2V_BIAS                                                  0                               /* Defaults to 10uA */
#define BITM_CT_CT_CFG1_INT_CYCLE                                                (_ADI_MSK(0x07000000,uint32_t))  /* Num CtoV integration cycles */
#define BITM_CT_CT_CFG1_AIN_SEL                                                  (_ADI_MSK(0x00C00000,uint32_t))  /* Drive unmatched pads. */
#define BITM_CT_CT_CFG1_AUTOZERO_DUTY                                            (_ADI_MSK(0x00001C00,uint32_t))  /* Set CtoV autozero duty-cycle. */
#define BITM_CT_CT_CFG1_BYPASS_GAIN                                              (_ADI_MSK(0x00000100,uint32_t))  /* Override CT_STAGEn[23] */
#define BITM_CT_CT_CFG1_INT_BUFFER                                               (_ADI_MSK(0x000000C0,uint32_t))  /* Defaults to 3uA */
#define BITM_CT_CT_CFG1_PGA_BIAS                                                 (_ADI_MSK(0x00000038,uint32_t))  /* Defaults to 5uA */
#define BITM_CT_CT_CFG1_C2V_BIAS                                                 (_ADI_MSK(0x00000007,uint32_t))  /* Defaults to 10uA */

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_CFG2                                                               Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_CFG2_C2V_LPF                                                  31
#define BITP_CT_CT_CFG2_STIMER_EN                                                27                               /* Enable self-timed operation */
#define BITP_CT_CT_CFG2_C2V_HOLD_TIME                                            22
#define BITP_CT_CT_CFG2_SELF_TIMER_WAIT                                          13                               /* Delay between sequences when stimer is enabled. */
#define BITP_CT_CT_CFG2_PH24_DUTY                                                 5
#define BITP_CT_CT_CFG2_PH13_DUTY                                                 0
#define BITM_CT_CT_CFG2_C2V_LPF                                                  (_ADI_MSK(0x80000000,uint32_t))
#define BITM_CT_CT_CFG2_STIMER_EN                                                (_ADI_MSK(0x08000000,uint32_t))  /* Enable self-timed operation */
#define BITM_CT_CT_CFG2_C2V_HOLD_TIME                                            (_ADI_MSK(0x07C00000,uint32_t))
#define BITM_CT_CT_CFG2_SELF_TIMER_WAIT                                          (_ADI_MSK(0x003FE000,uint32_t))  /* Delay between sequences when stimer is enabled. */
#define BITM_CT_CT_CFG2_PH24_DUTY                                                (_ADI_MSK(0x00001FE0,uint32_t))
#define BITM_CT_CT_CFG2_PH13_DUTY                                                (_ADI_MSK(0x0000001F,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_CFG3                                                               Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_CFG3_PK2PK_AVG                                                25                               /* Number of samples to be averaged */
#define BITP_CT_CT_CFG3_PK2PK_AVG_MIN                                            20                               /* Minimum rank position for averaging */
#define BITP_CT_CT_CFG3_PK2PK_SUBSET                                             16                               /* noise = abs((#pk2pk_num_spls-1) - pk2pk_subset) */
#define BITP_CT_CT_CFG3_PK2PK_NUM_SPLS                                           14                               /* #samples for each peak-to-peak measurement */
#define BITP_CT_CT_CFG3_RES_SEL                                                  13                               /* Result select */
#define BITP_CT_CT_CFG3_DIG_CTOV_CAPTOUCH_EN                                      5                               /* Enable capTouch operation: ENSURE GPCON BITS ARE SET CORRECTLY IF SETTING THESE BITS */
#define BITP_CT_CT_CFG3_IIR_WEIGHT                                                1                               /* CDC IIR filter weight */
#define BITP_CT_CT_CFG3_IIR_EN                                                    0                               /* Enable CDC IIR filter */
#define BITM_CT_CT_CFG3_PK2PK_AVG                                                (_ADI_MSK(0x0E000000,uint32_t))  /* Number of samples to be averaged */
#define BITM_CT_CT_CFG3_PK2PK_AVG_MIN                                            (_ADI_MSK(0x01F00000,uint32_t))  /* Minimum rank position for averaging */
#define BITM_CT_CT_CFG3_PK2PK_SUBSET                                             (_ADI_MSK(0x000F0000,uint32_t))  /* noise = abs((#pk2pk_num_spls-1) - pk2pk_subset) */
#define BITM_CT_CT_CFG3_PK2PK_NUM_SPLS                                           (_ADI_MSK(0x0000C000,uint32_t))  /* #samples for each peak-to-peak measurement */
#define BITM_CT_CT_CFG3_RES_SEL                                                  (_ADI_MSK(0x00002000,uint32_t))  /* Result select */
#define BITM_CT_CT_CFG3_DIG_CTOV_CAPTOUCH_EN                                     (_ADI_MSK(0x000007E0,uint32_t))  /* Enable capTouch operation: ENSURE GPCON BITS ARE SET CORRECTLY IF SETTING THESE BITS */
#define BITM_CT_CT_CFG3_IIR_WEIGHT                                               (_ADI_MSK(0x0000001E,uint32_t))  /* CDC IIR filter weight */
#define BITM_CT_CT_CFG3_IIR_EN                                                   (_ADI_MSK(0x00000001,uint32_t))  /* Enable CDC IIR filter */

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_MEAS_SEL                                                           Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_MEAS_SEL_GND_SEL                                               0                               /* Enable Selected Stage(s) */
#define BITM_CT_CT_MEAS_SEL_GND_SEL                                              (_ADI_MSK(0x0000FFFF,uint32_t))  /* Enable Selected Stage(s) */

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_BASELINE_CTRL                                                      Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_BASELINE_CTRL_BASELINE_CAL_DELAY                              29
#define BITP_CT_CT_BASELINE_CTRL_CONVERSION_RESET                                28
#define BITP_CT_CT_BASELINE_CTRL_FORCE_CAL                                       27                               /* Force calibration during next sequence. */
#define BITP_CT_CT_BASELINE_CTRL_FAST_FILTER_UPDATE                              24
#define BITP_CT_CT_BASELINE_CTRL_FAST_PROX                                       16
#define BITP_CT_CT_BASELINE_CTRL_SLOW_PROX                                        8
#define BITP_CT_CT_BASELINE_CTRL_BL_COEFF                                         4
#define BITP_CT_CT_BASELINE_CTRL_CDC_COEFF                                        0
#define BITM_CT_CT_BASELINE_CTRL_BASELINE_CAL_DELAY                              (_ADI_MSK(0xE0000000,uint32_t))
#define BITM_CT_CT_BASELINE_CTRL_CONVERSION_RESET                                (_ADI_MSK(0x10000000,uint32_t))
#define BITM_CT_CT_BASELINE_CTRL_FORCE_CAL                                       (_ADI_MSK(0x08000000,uint32_t))  /* Force calibration during next sequence. */
#define BITM_CT_CT_BASELINE_CTRL_FAST_FILTER_UPDATE                              (_ADI_MSK(0x07000000,uint32_t))
#define BITM_CT_CT_BASELINE_CTRL_FAST_PROX                                       (_ADI_MSK(0x00FF0000,uint32_t))
#define BITM_CT_CT_BASELINE_CTRL_SLOW_PROX                                       (_ADI_MSK(0x0000FF00,uint32_t))
#define BITM_CT_CT_BASELINE_CTRL_BL_COEFF                                        (_ADI_MSK(0x000000F0,uint32_t))
#define BITM_CT_CT_BASELINE_CTRL_CDC_COEFF                                       (_ADI_MSK(0x0000000F,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_AVG                                                                Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_AVG_RANK_FILT                                                 28
#define BITP_CT_CT_AVG_AVG_OUTPUT                                                24
#define BITP_CT_CT_AVG_AVG_GROUP2                                                20
#define BITP_CT_CT_AVG_AVG_GROUP1                                                16
#define BITP_CT_CT_AVG_AVG_MIN_GROUP2                                             8
#define BITP_CT_CT_AVG_AVG_MIN_GROUP1                                             0
#define BITM_CT_CT_AVG_RANK_FILT                                                 (_ADI_MSK(0x70000000,uint32_t))
#define BITM_CT_CT_AVG_AVG_OUTPUT                                                (_ADI_MSK(0x03000000,uint32_t))
#define BITM_CT_CT_AVG_AVG_GROUP2                                                (_ADI_MSK(0x00700000,uint32_t))
#define BITM_CT_CT_AVG_AVG_GROUP1                                                (_ADI_MSK(0x00070000,uint32_t))
#define BITM_CT_CT_AVG_AVG_MIN_GROUP2                                            (_ADI_MSK(0x00001F00,uint32_t))
#define BITM_CT_CT_AVG_AVG_MIN_GROUP1                                            (_ADI_MSK(0x0000001F,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_CAL_EN                                                             Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_CAL_EN_CAL_EN                                                  0
#define BITM_CT_CT_CAL_EN_CAL_EN                                                 (_ADI_MSK(0x0000FFFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_TOUCH_CFG1                                                         Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_TOUCH_CFG1_TCH_UPPER_THLD                                     16
#define BITP_CT_CT_TOUCH_CFG1_TCH_LOWER_THLD                                      0
#define BITM_CT_CT_TOUCH_CFG1_TCH_UPPER_THLD                                     (_ADI_MSK(0x3FFF0000,uint32_t))
#define BITM_CT_CT_TOUCH_CFG1_TCH_LOWER_THLD                                     (_ADI_MSK(0x00003FFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_TOUCH_CFG2                                                         Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_TOUCH_CFG2_INDIVIDUAL_THRESHOLD_EN                            23                               /* Use CT_SENSOR_THR_CFGx for sensor threshold */
#define BITP_CT_CT_TOUCH_CFG2_TCH_EVENT_ON_HIGH                                  22                               /* Detect rising Edge when set */
#define BITP_CT_CT_TOUCH_CFG2_TCH_SEL_INPA                                       19                               /* Select input Source A */
#define BITP_CT_CT_TOUCH_CFG2_TCH_SEL_INPB                                       16                               /* Select input Source B */
#define BITP_CT_CT_TOUCH_CFG2_TCH_DETECT_ENABLE                                  15                               /* Enable touch detector */
#define BITP_CT_CT_TOUCH_CFG2_TCH_MIN_DURATION                                    0                               /* Minimum Touch duration (Min Duration = 2) */
#define BITM_CT_CT_TOUCH_CFG2_INDIVIDUAL_THRESHOLD_EN                            (_ADI_MSK(0x00800000,uint32_t))  /* Use CT_SENSOR_THR_CFGx for sensor threshold */
#define BITM_CT_CT_TOUCH_CFG2_TCH_EVENT_ON_HIGH                                  (_ADI_MSK(0x00400000,uint32_t))  /* Detect rising Edge when set */
#define BITM_CT_CT_TOUCH_CFG2_TCH_SEL_INPA                                       (_ADI_MSK(0x00380000,uint32_t))  /* Select input Source A */
#define BITM_CT_CT_TOUCH_CFG2_TCH_SEL_INPB                                       (_ADI_MSK(0x00070000,uint32_t))  /* Select input Source B */
#define BITM_CT_CT_TOUCH_CFG2_TCH_DETECT_ENABLE                                  (_ADI_MSK(0x00008000,uint32_t))  /* Enable touch detector */
#define BITM_CT_CT_TOUCH_CFG2_TCH_MIN_DURATION                                   (_ADI_MSK(0x000003FF,uint32_t))  /* Minimum Touch duration (Min Duration = 2) */

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_RELEASE_CFG1                                                       Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_RELEASE_CFG1_REL_UPPER_THLD                                   16
#define BITP_CT_CT_RELEASE_CFG1_REL_LOWER_THLD                                    0
#define BITM_CT_CT_RELEASE_CFG1_REL_UPPER_THLD                                   (_ADI_MSK(0x3FFF0000,uint32_t))
#define BITM_CT_CT_RELEASE_CFG1_REL_LOWER_THLD                                   (_ADI_MSK(0x00003FFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_RELEASE_CFG2                                                       Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_RELEASE_CFG2_INDIVIDUAL_THRESHOLD_EN                          23                               /* Use CT_SENSOR_THR_CFGx for sensor threshold */
#define BITP_CT_CT_RELEASE_CFG2_REL_EVENT_ON_HIGH                                22                               /* Determine calculation direction */
#define BITP_CT_CT_RELEASE_CFG2_REL_SEL_INPA                                     19                               /* Select input Source A */
#define BITP_CT_CT_RELEASE_CFG2_REL_SEL_INPB                                     16                               /* Select input Source B */
#define BITP_CT_CT_RELEASE_CFG2_REL_DETECT_ENABLE                                15                               /* Enable the release detector */
#define BITP_CT_CT_RELEASE_CFG2_REL_MIN_DURATION                                  0                               /* Minimum duration before release is detected (Min Duration = 2) */
#define BITM_CT_CT_RELEASE_CFG2_INDIVIDUAL_THRESHOLD_EN                          (_ADI_MSK(0x00800000,uint32_t))  /* Use CT_SENSOR_THR_CFGx for sensor threshold */
#define BITM_CT_CT_RELEASE_CFG2_REL_EVENT_ON_HIGH                                (_ADI_MSK(0x00400000,uint32_t))  /* Determine calculation direction */
#define BITM_CT_CT_RELEASE_CFG2_REL_SEL_INPA                                     (_ADI_MSK(0x00380000,uint32_t))  /* Select input Source A */
#define BITM_CT_CT_RELEASE_CFG2_REL_SEL_INPB                                     (_ADI_MSK(0x00070000,uint32_t))  /* Select input Source B */
#define BITM_CT_CT_RELEASE_CFG2_REL_DETECT_ENABLE                                (_ADI_MSK(0x00008000,uint32_t))  /* Enable the release detector */
#define BITM_CT_CT_RELEASE_CFG2_REL_MIN_DURATION                                 (_ADI_MSK(0x000003FF,uint32_t))  /* Minimum duration before release is detected (Min Duration = 2) */

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_IEN                                                                Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_IEN_RELEASE_DETECTED_IEN                                       4                               /* Release-Detect Interrupt Enable */
#define BITP_CT_CT_IEN_TOUCH_DETECTED_IEN                                         3                               /* Touch-Detect Interrupt Enable */
#define BITP_CT_CT_IEN_PROX_DETECTED_IEN                                          2                               /* Proximity Detect interrupt Enable */
#define BITP_CT_CT_IEN_STATUS_GT_THRESHOLD_IEN                                    1                               /* LSTAT or HSTAT has exceeded threshold Interrupt Enable */
#define BITP_CT_CT_IEN_CONV_SET_COMPLETE_IEN                                      0                               /* Sequence complete Interrupt Enable */
#define BITM_CT_CT_IEN_RELEASE_DETECTED_IEN                                      (_ADI_MSK(0x00000010,uint32_t))  /* Release-Detect Interrupt Enable */
#define BITM_CT_CT_IEN_TOUCH_DETECTED_IEN                                        (_ADI_MSK(0x00000008,uint32_t))  /* Touch-Detect Interrupt Enable */
#define BITM_CT_CT_IEN_PROX_DETECTED_IEN                                         (_ADI_MSK(0x00000004,uint32_t))  /* Proximity Detect interrupt Enable */
#define BITM_CT_CT_IEN_STATUS_GT_THRESHOLD_IEN                                   (_ADI_MSK(0x00000002,uint32_t))  /* LSTAT or HSTAT has exceeded threshold Interrupt Enable */
#define BITM_CT_CT_IEN_CONV_SET_COMPLETE_IEN                                     (_ADI_MSK(0x00000001,uint32_t))  /* Sequence complete Interrupt Enable */

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_INT                                                                Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_INT_RELEASE_DETECTED                                           4                               /* Sticky Bit: Write 1 to clear */
#define BITP_CT_CT_INT_TOUCH_DETECTED                                             3                               /* Sticky Bit: Write 1 to clear */
#define BITP_CT_CT_INT_PROX_DETECTED                                              2                               /* Sticky Bit: Write 1 to clear */
#define BITP_CT_CT_INT_STATUS_GT_THRESHOLD                                        1                               /* Sticky Bit: Write 1 to clear */
#define BITP_CT_CT_INT_CONV_SET_COMPLETE                                          0                               /* Sticky Bit: Write 1 to clear */
#define BITM_CT_CT_INT_RELEASE_DETECTED                                          (_ADI_MSK(0x00000010,uint32_t))  /* Sticky Bit: Write 1 to clear */
#define BITM_CT_CT_INT_TOUCH_DETECTED                                            (_ADI_MSK(0x00000008,uint32_t))  /* Sticky Bit: Write 1 to clear */
#define BITM_CT_CT_INT_PROX_DETECTED                                             (_ADI_MSK(0x00000004,uint32_t))  /* Sticky Bit: Write 1 to clear */
#define BITM_CT_CT_INT_STATUS_GT_THRESHOLD                                       (_ADI_MSK(0x00000002,uint32_t))  /* Sticky Bit: Write 1 to clear */
#define BITM_CT_CT_INT_CONV_SET_COMPLETE                                         (_ADI_MSK(0x00000001,uint32_t))  /* Sticky Bit: Write 1 to clear */

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_OFFS_HSTAT                                                         Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_OFFS_HSTAT_OFFS_HSTAT                                          0                               /* stage_n has exceeded the high-limit threshold */
#define BITM_CT_CT_OFFS_HSTAT_OFFS_HSTAT                                         (_ADI_MSK(0x0000FFFF,uint32_t))  /* stage_n has exceeded the high-limit threshold */

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_OFFS_LSTAT                                                         Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_OFFS_LSTAT_OFFS_LSTAT                                          0                               /* stage_n has exceeded the low-limit threshold */
#define BITM_CT_CT_OFFS_LSTAT_OFFS_LSTAT                                         (_ADI_MSK(0x0000FFFF,uint32_t))  /* stage_n has exceeded the low-limit threshold */

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_PROX_STAT                                                          Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_PROX_STAT_PROX_STAT                                            0                               /* Proximity status per-stage. */
#define BITM_CT_CT_PROX_STAT_PROX_STAT                                           (_ADI_MSK(0x0000FFFF,uint32_t))  /* Proximity status per-stage. */

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_FPROX_STAT                                                         Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_FPROX_STAT_FPROX_STAT                                          0                               /* Fast Proximity status per-stage. */
#define BITM_CT_CT_FPROX_STAT_FPROX_STAT                                         (_ADI_MSK(0x0000FFFF,uint32_t))  /* Fast Proximity status per-stage. */

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_TOUCH_STAT                                                         Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_TOUCH_STAT_REL_DETECT_STAT                                    16                               /* Sticky - Release detect status per-stage */
#define BITP_CT_CT_TOUCH_STAT_TCH_DETECT_STAT                                     0                               /* Sticky - Touch detect status per-stage. */
#define BITM_CT_CT_TOUCH_STAT_REL_DETECT_STAT                                    (_ADI_MSK(0xFFFF0000,uint32_t))  /* Sticky - Release detect status per-stage */
#define BITM_CT_CT_TOUCH_STAT_TCH_DETECT_STAT                                    (_ADI_MSK(0x0000FFFF,uint32_t))  /* Sticky - Touch detect status per-stage. */

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_STAGE0_CFG                                                         Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_STAGE0_CFG_CIN_CON_POS_CDC_EN                                 27                               /* Enable selected cin to +ve CDC */
#define BITP_CT_CT_STAGE0_CFG_CIN_CON_POS_CDC                                    24                               /* Select Cin to connect to +ve CDC input */
#define BITP_CT_CT_STAGE0_CFG_PGA_BYPASS                                         23                               /* Bypass pga for this stage. */
#define BITP_CT_CT_STAGE0_CFG_NOISE_REDUCTION_EN                                 22                               /* Enable frequency-hopping noise-reduction */
#define BITP_CT_CT_STAGE0_CFG_PGA_GAIN                                           20                               /* Stage specific PGA gain setup. */
#define BITP_CT_CT_STAGE0_CFG_C2V_IP_RANGE                                       16
#define BITP_CT_CT_STAGE0_CFG_EN_MAIN_DAC                                        15                               /* Enable Offset DAC */
#define BITP_CT_CT_STAGE0_CFG_PK2PK                                              13                               /* enable peak to peak measurements */
#define BITP_CT_CT_STAGE0_CFG_MAIN_DAC                                            3                               /* Main Dac offset 1LSB = 2*388.5fF */
#define BITP_CT_CT_STAGE0_CFG_SUB_DAC                                             0                               /* Sub Dac offset 1LSB = 2*48fF */
#define BITM_CT_CT_STAGE0_CFG_CIN_CON_POS_CDC_EN                                 (_ADI_MSK(0x08000000,uint32_t))  /* Enable selected cin to +ve CDC */
#define BITM_CT_CT_STAGE0_CFG_CIN_CON_POS_CDC                                    (_ADI_MSK(0x07000000,uint32_t))  /* Select Cin to connect to +ve CDC input */
#define BITM_CT_CT_STAGE0_CFG_PGA_BYPASS                                         (_ADI_MSK(0x00800000,uint32_t))  /* Bypass pga for this stage. */
#define BITM_CT_CT_STAGE0_CFG_NOISE_REDUCTION_EN                                 (_ADI_MSK(0x00400000,uint32_t))  /* Enable frequency-hopping noise-reduction */
#define BITM_CT_CT_STAGE0_CFG_PGA_GAIN                                           (_ADI_MSK(0x00300000,uint32_t))  /* Stage specific PGA gain setup. */
#define BITM_CT_CT_STAGE0_CFG_C2V_IP_RANGE                                       (_ADI_MSK(0x000F0000,uint32_t))
#define BITM_CT_CT_STAGE0_CFG_EN_MAIN_DAC                                        (_ADI_MSK(0x00008000,uint32_t))  /* Enable Offset DAC */
#define BITM_CT_CT_STAGE0_CFG_PK2PK                                              (_ADI_MSK(0x00002000,uint32_t))  /* enable peak to peak measurements */
#define BITM_CT_CT_STAGE0_CFG_MAIN_DAC                                           (_ADI_MSK(0x000003F8,uint32_t))  /* Main Dac offset 1LSB = 2*388.5fF */
#define BITM_CT_CT_STAGE0_CFG_SUB_DAC                                            (_ADI_MSK(0x00000007,uint32_t))  /* Sub Dac offset 1LSB = 2*48fF */

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_STAGE1_CFG                                                         Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_STAGE1_CFG_CIN_CON_POS_CDC_EN                                 27                               /* Enable selected cin to +ve CDC */
#define BITP_CT_CT_STAGE1_CFG_CIN_CON_POS_CDC                                    24                               /* Select Cin to connect to +ve CDC input */
#define BITP_CT_CT_STAGE1_CFG_PGA_BYPASS                                         23                               /* Bypass pga for this stage */
#define BITP_CT_CT_STAGE1_CFG_NOISE_REDUCTION_EN                                 22                               /* Enable frequency-hopping noise-reduction */
#define BITP_CT_CT_STAGE1_CFG_PGA_GAIN                                           20                               /* Stage specific PGA gain setup. */
#define BITP_CT_CT_STAGE1_CFG_C2V_IP_RANGE                                       16
#define BITP_CT_CT_STAGE1_CFG_EN_MAIN_DAC                                        15                               /* Enable Offset DAC */
#define BITP_CT_CT_STAGE1_CFG_PK2PK                                              13                               /* enable peak to peak measurements */
#define BITP_CT_CT_STAGE1_CFG_MAIN_DAC                                            3                               /* Main Dac offset 1LSB = 2*388.5fF */
#define BITP_CT_CT_STAGE1_CFG_SUB_DAC                                             0                               /* Sub Dac offset 1LSB = 2*48fF */
#define BITM_CT_CT_STAGE1_CFG_CIN_CON_POS_CDC_EN                                 (_ADI_MSK(0x08000000,uint32_t))  /* Enable selected cin to +ve CDC */
#define BITM_CT_CT_STAGE1_CFG_CIN_CON_POS_CDC                                    (_ADI_MSK(0x07000000,uint32_t))  /* Select Cin to connect to +ve CDC input */
#define BITM_CT_CT_STAGE1_CFG_PGA_BYPASS                                         (_ADI_MSK(0x00800000,uint32_t))  /* Bypass pga for this stage */
#define BITM_CT_CT_STAGE1_CFG_NOISE_REDUCTION_EN                                 (_ADI_MSK(0x00400000,uint32_t))  /* Enable frequency-hopping noise-reduction */
#define BITM_CT_CT_STAGE1_CFG_PGA_GAIN                                           (_ADI_MSK(0x00300000,uint32_t))  /* Stage specific PGA gain setup. */
#define BITM_CT_CT_STAGE1_CFG_C2V_IP_RANGE                                       (_ADI_MSK(0x000F0000,uint32_t))
#define BITM_CT_CT_STAGE1_CFG_EN_MAIN_DAC                                        (_ADI_MSK(0x00008000,uint32_t))  /* Enable Offset DAC */
#define BITM_CT_CT_STAGE1_CFG_PK2PK                                              (_ADI_MSK(0x00002000,uint32_t))  /* enable peak to peak measurements */
#define BITM_CT_CT_STAGE1_CFG_MAIN_DAC                                           (_ADI_MSK(0x000003F8,uint32_t))  /* Main Dac offset 1LSB = 2*388.5fF */
#define BITM_CT_CT_STAGE1_CFG_SUB_DAC                                            (_ADI_MSK(0x00000007,uint32_t))  /* Sub Dac offset 1LSB = 2*48fF */

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_STAGE2_CFG                                                         Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_STAGE2_CFG_CIN_CON_POS_CDC_EN                                 27                               /* Enable selected cin to +ve CDC */
#define BITP_CT_CT_STAGE2_CFG_CIN_CON_POS_CDC                                    24                               /* Select Cin to connect to +ve CDC input */
#define BITP_CT_CT_STAGE2_CFG_PGA_BYPASS                                         23                               /* Bypass pga for this stage */
#define BITP_CT_CT_STAGE2_CFG_NOISE_REDUCTION_EN                                 22                               /* Enable frequency-hopping noise-reduction */
#define BITP_CT_CT_STAGE2_CFG_PGA_GAIN                                           20                               /* Stage specific PGA gain setup. */
#define BITP_CT_CT_STAGE2_CFG_C2V_IP_RANGE                                       16
#define BITP_CT_CT_STAGE2_CFG_EN_MAIN_DAC                                        15                               /* Enable Offset DAC */
#define BITP_CT_CT_STAGE2_CFG_PK2PK                                              13                               /* enable peak to peak measurements */
#define BITP_CT_CT_STAGE2_CFG_MAIN_DAC                                            3                               /* Main Dac offset 1LSB = 2*388.5fF */
#define BITP_CT_CT_STAGE2_CFG_SUB_DAC                                             0                               /* Sub Dac offset 1LSB = 2*48fF */
#define BITM_CT_CT_STAGE2_CFG_CIN_CON_POS_CDC_EN                                 (_ADI_MSK(0x08000000,uint32_t))  /* Enable selected cin to +ve CDC */
#define BITM_CT_CT_STAGE2_CFG_CIN_CON_POS_CDC                                    (_ADI_MSK(0x07000000,uint32_t))  /* Select Cin to connect to +ve CDC input */
#define BITM_CT_CT_STAGE2_CFG_PGA_BYPASS                                         (_ADI_MSK(0x00800000,uint32_t))  /* Bypass pga for this stage */
#define BITM_CT_CT_STAGE2_CFG_NOISE_REDUCTION_EN                                 (_ADI_MSK(0x00400000,uint32_t))  /* Enable frequency-hopping noise-reduction */
#define BITM_CT_CT_STAGE2_CFG_PGA_GAIN                                           (_ADI_MSK(0x00300000,uint32_t))  /* Stage specific PGA gain setup. */
#define BITM_CT_CT_STAGE2_CFG_C2V_IP_RANGE                                       (_ADI_MSK(0x000F0000,uint32_t))
#define BITM_CT_CT_STAGE2_CFG_EN_MAIN_DAC                                        (_ADI_MSK(0x00008000,uint32_t))  /* Enable Offset DAC */
#define BITM_CT_CT_STAGE2_CFG_PK2PK                                              (_ADI_MSK(0x00002000,uint32_t))  /* enable peak to peak measurements */
#define BITM_CT_CT_STAGE2_CFG_MAIN_DAC                                           (_ADI_MSK(0x000003F8,uint32_t))  /* Main Dac offset 1LSB = 2*388.5fF */
#define BITM_CT_CT_STAGE2_CFG_SUB_DAC                                            (_ADI_MSK(0x00000007,uint32_t))  /* Sub Dac offset 1LSB = 2*48fF */

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_STAGE3_CFG                                                         Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_STAGE3_CFG_CIN_CON_POS_CDC_EN                                 27                               /* Enable selected cin to +ve CDC */
#define BITP_CT_CT_STAGE3_CFG_CIN_CON_POS_CDC                                    24                               /* Select Cin to connect to +ve CDC input */
#define BITP_CT_CT_STAGE3_CFG_PGA_BYPASS                                         23                               /* Bypass pga for this stage */
#define BITP_CT_CT_STAGE3_CFG_NOISE_REDUCTION_EN                                 22                               /* Enable frequency-hopping noise-reduction */
#define BITP_CT_CT_STAGE3_CFG_PGA_GAIN                                           20                               /* Stage specific PGA gain setup. */
#define BITP_CT_CT_STAGE3_CFG_C2V_IP_RANGE                                       16
#define BITP_CT_CT_STAGE3_CFG_EN_MAIN_DAC                                        15                               /* Enable Offset DAC */
#define BITP_CT_CT_STAGE3_CFG_PK2PK                                              13                               /* enable peak to peak measurements */
#define BITP_CT_CT_STAGE3_CFG_MAIN_DAC                                            3                               /* Main Dac offset 1LSB = 2*388.5fF */
#define BITP_CT_CT_STAGE3_CFG_SUB_DAC                                             0                               /* Sub Dac offset 1LSB = 2*48fF */
#define BITM_CT_CT_STAGE3_CFG_CIN_CON_POS_CDC_EN                                 (_ADI_MSK(0x08000000,uint32_t))  /* Enable selected cin to +ve CDC */
#define BITM_CT_CT_STAGE3_CFG_CIN_CON_POS_CDC                                    (_ADI_MSK(0x07000000,uint32_t))  /* Select Cin to connect to +ve CDC input */
#define BITM_CT_CT_STAGE3_CFG_PGA_BYPASS                                         (_ADI_MSK(0x00800000,uint32_t))  /* Bypass pga for this stage */
#define BITM_CT_CT_STAGE3_CFG_NOISE_REDUCTION_EN                                 (_ADI_MSK(0x00400000,uint32_t))  /* Enable frequency-hopping noise-reduction */
#define BITM_CT_CT_STAGE3_CFG_PGA_GAIN                                           (_ADI_MSK(0x00300000,uint32_t))  /* Stage specific PGA gain setup. */
#define BITM_CT_CT_STAGE3_CFG_C2V_IP_RANGE                                       (_ADI_MSK(0x000F0000,uint32_t))
#define BITM_CT_CT_STAGE3_CFG_EN_MAIN_DAC                                        (_ADI_MSK(0x00008000,uint32_t))  /* Enable Offset DAC */
#define BITM_CT_CT_STAGE3_CFG_PK2PK                                              (_ADI_MSK(0x00002000,uint32_t))  /* enable peak to peak measurements */
#define BITM_CT_CT_STAGE3_CFG_MAIN_DAC                                           (_ADI_MSK(0x000003F8,uint32_t))  /* Main Dac offset 1LSB = 2*388.5fF */
#define BITM_CT_CT_STAGE3_CFG_SUB_DAC                                            (_ADI_MSK(0x00000007,uint32_t))  /* Sub Dac offset 1LSB = 2*48fF */

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_STAGE4_CFG                                                         Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_STAGE4_CFG_CIN_CON_POS_CDC_EN                                 27                               /* Enable selected cin to +ve CDC */
#define BITP_CT_CT_STAGE4_CFG_CIN_CON_POS_CDC                                    24                               /* Select Cin to connect to +ve CDC input */
#define BITP_CT_CT_STAGE4_CFG_PGA_BYPASS                                         23                               /* Bypass pga for this stage */
#define BITP_CT_CT_STAGE4_CFG_NOISE_REDUCTION_EN                                 22                               /* Enable frequency-hopping noise-reduction */
#define BITP_CT_CT_STAGE4_CFG_PGA_GAIN                                           20                               /* Stage specific PGA gain setup. */
#define BITP_CT_CT_STAGE4_CFG_C2V_IP_RANGE                                       16
#define BITP_CT_CT_STAGE4_CFG_EN_MAIN_DAC                                        15                               /* Enable Offset DAC */
#define BITP_CT_CT_STAGE4_CFG_PK2PK                                              13                               /* enable peak to peak measurements */
#define BITP_CT_CT_STAGE4_CFG_MAIN_DAC                                            3                               /* Main Dac offset 1LSB = 2*388.5fF */
#define BITP_CT_CT_STAGE4_CFG_SUB_DAC                                             0                               /* Sub Dac offset 1LSB = 2*48fF */
#define BITM_CT_CT_STAGE4_CFG_CIN_CON_POS_CDC_EN                                 (_ADI_MSK(0x08000000,uint32_t))  /* Enable selected cin to +ve CDC */
#define BITM_CT_CT_STAGE4_CFG_CIN_CON_POS_CDC                                    (_ADI_MSK(0x07000000,uint32_t))  /* Select Cin to connect to +ve CDC input */
#define BITM_CT_CT_STAGE4_CFG_PGA_BYPASS                                         (_ADI_MSK(0x00800000,uint32_t))  /* Bypass pga for this stage */
#define BITM_CT_CT_STAGE4_CFG_NOISE_REDUCTION_EN                                 (_ADI_MSK(0x00400000,uint32_t))  /* Enable frequency-hopping noise-reduction */
#define BITM_CT_CT_STAGE4_CFG_PGA_GAIN                                           (_ADI_MSK(0x00300000,uint32_t))  /* Stage specific PGA gain setup. */
#define BITM_CT_CT_STAGE4_CFG_C2V_IP_RANGE                                       (_ADI_MSK(0x000F0000,uint32_t))
#define BITM_CT_CT_STAGE4_CFG_EN_MAIN_DAC                                        (_ADI_MSK(0x00008000,uint32_t))  /* Enable Offset DAC */
#define BITM_CT_CT_STAGE4_CFG_PK2PK                                              (_ADI_MSK(0x00002000,uint32_t))  /* enable peak to peak measurements */
#define BITM_CT_CT_STAGE4_CFG_MAIN_DAC                                           (_ADI_MSK(0x000003F8,uint32_t))  /* Main Dac offset 1LSB = 2*388.5fF */
#define BITM_CT_CT_STAGE4_CFG_SUB_DAC                                            (_ADI_MSK(0x00000007,uint32_t))  /* Sub Dac offset 1LSB = 2*48fF */

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_STAGE5_CFG                                                         Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_STAGE5_CFG_CIN_CON_POS_CDC_EN                                 27                               /* Enable selected cin to +ve CDC */
#define BITP_CT_CT_STAGE5_CFG_CIN_CON_POS_CDC                                    24                               /* Select Cin to connect to +ve CDC input */
#define BITP_CT_CT_STAGE5_CFG_PGA_BYPASS                                         23                               /* Bypass pga for this stage */
#define BITP_CT_CT_STAGE5_CFG_NOISE_REDUCTION_EN                                 22                               /* Enable frequency-hopping noise-reduction */
#define BITP_CT_CT_STAGE5_CFG_PGA_GAIN                                           20                               /* Stage specific PGA gain setup. */
#define BITP_CT_CT_STAGE5_CFG_C2V_IP_RANGE                                       16
#define BITP_CT_CT_STAGE5_CFG_EN_MAIN_DAC                                        15                               /* Enable Offset DAC */
#define BITP_CT_CT_STAGE5_CFG_PK2PK                                              13                               /* enable peak to peak measurements */
#define BITP_CT_CT_STAGE5_CFG_MAIN_DAC                                            3                               /* Main Dac offset 1LSB = 2*388.5fF */
#define BITP_CT_CT_STAGE5_CFG_SUB_DAC                                             0                               /* Sub Dac offset 1LSB = 2*48fF */
#define BITM_CT_CT_STAGE5_CFG_CIN_CON_POS_CDC_EN                                 (_ADI_MSK(0x08000000,uint32_t))  /* Enable selected cin to +ve CDC */
#define BITM_CT_CT_STAGE5_CFG_CIN_CON_POS_CDC                                    (_ADI_MSK(0x07000000,uint32_t))  /* Select Cin to connect to +ve CDC input */
#define BITM_CT_CT_STAGE5_CFG_PGA_BYPASS                                         (_ADI_MSK(0x00800000,uint32_t))  /* Bypass pga for this stage */
#define BITM_CT_CT_STAGE5_CFG_NOISE_REDUCTION_EN                                 (_ADI_MSK(0x00400000,uint32_t))  /* Enable frequency-hopping noise-reduction */
#define BITM_CT_CT_STAGE5_CFG_PGA_GAIN                                           (_ADI_MSK(0x00300000,uint32_t))  /* Stage specific PGA gain setup. */
#define BITM_CT_CT_STAGE5_CFG_C2V_IP_RANGE                                       (_ADI_MSK(0x000F0000,uint32_t))
#define BITM_CT_CT_STAGE5_CFG_EN_MAIN_DAC                                        (_ADI_MSK(0x00008000,uint32_t))  /* Enable Offset DAC */
#define BITM_CT_CT_STAGE5_CFG_PK2PK                                              (_ADI_MSK(0x00002000,uint32_t))  /* enable peak to peak measurements */
#define BITM_CT_CT_STAGE5_CFG_MAIN_DAC                                           (_ADI_MSK(0x000003F8,uint32_t))  /* Main Dac offset 1LSB = 2*388.5fF */
#define BITM_CT_CT_STAGE5_CFG_SUB_DAC                                            (_ADI_MSK(0x00000007,uint32_t))  /* Sub Dac offset 1LSB = 2*48fF */

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_STAGE6_CFG                                                         Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_STAGE6_CFG_CIN_CON_POS_CDC_EN                                 27                               /* Enable selected cin to +ve CDC */
#define BITP_CT_CT_STAGE6_CFG_CIN_CON_POS_CDC                                    24                               /* Select Cin to connect to +ve CDC input */
#define BITP_CT_CT_STAGE6_CFG_PGA_BYPASS                                         23                               /* Bypass pga for this stage */
#define BITP_CT_CT_STAGE6_CFG_NOISE_REDUCTION_EN                                 22                               /* Enable frequency-hopping noise-reduction */
#define BITP_CT_CT_STAGE6_CFG_PGA_GAIN                                           20                               /* Stage specific PGA gain setup. */
#define BITP_CT_CT_STAGE6_CFG_C2V_IP_RANGE                                       16
#define BITP_CT_CT_STAGE6_CFG_EN_MAIN_DAC                                        15                               /* Enable Offset DAC */
#define BITP_CT_CT_STAGE6_CFG_PK2PK                                              13                               /* enable peak to peak measurements */
#define BITP_CT_CT_STAGE6_CFG_MAIN_DAC                                            3                               /* Main Dac offset 1LSB = 2*388.5fF */
#define BITP_CT_CT_STAGE6_CFG_SUB_DAC                                             0                               /* Sub Dac offset 1LSB = 2*48fF */
#define BITM_CT_CT_STAGE6_CFG_CIN_CON_POS_CDC_EN                                 (_ADI_MSK(0x08000000,uint32_t))  /* Enable selected cin to +ve CDC */
#define BITM_CT_CT_STAGE6_CFG_CIN_CON_POS_CDC                                    (_ADI_MSK(0x07000000,uint32_t))  /* Select Cin to connect to +ve CDC input */
#define BITM_CT_CT_STAGE6_CFG_PGA_BYPASS                                         (_ADI_MSK(0x00800000,uint32_t))  /* Bypass pga for this stage */
#define BITM_CT_CT_STAGE6_CFG_NOISE_REDUCTION_EN                                 (_ADI_MSK(0x00400000,uint32_t))  /* Enable frequency-hopping noise-reduction */
#define BITM_CT_CT_STAGE6_CFG_PGA_GAIN                                           (_ADI_MSK(0x00300000,uint32_t))  /* Stage specific PGA gain setup. */
#define BITM_CT_CT_STAGE6_CFG_C2V_IP_RANGE                                       (_ADI_MSK(0x000F0000,uint32_t))
#define BITM_CT_CT_STAGE6_CFG_EN_MAIN_DAC                                        (_ADI_MSK(0x00008000,uint32_t))  /* Enable Offset DAC */
#define BITM_CT_CT_STAGE6_CFG_PK2PK                                              (_ADI_MSK(0x00002000,uint32_t))  /* enable peak to peak measurements */
#define BITM_CT_CT_STAGE6_CFG_MAIN_DAC                                           (_ADI_MSK(0x000003F8,uint32_t))  /* Main Dac offset 1LSB = 2*388.5fF */
#define BITM_CT_CT_STAGE6_CFG_SUB_DAC                                            (_ADI_MSK(0x00000007,uint32_t))  /* Sub Dac offset 1LSB = 2*48fF */

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_STAGE7_CFG                                                         Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_STAGE7_CFG_CIN_CON_POS_CDC_EN                                 27                               /* Enable selected cin to +ve CDC */
#define BITP_CT_CT_STAGE7_CFG_CIN_CON_POS_CDC                                    24                               /* Select Cin to connect to +ve CDC input */
#define BITP_CT_CT_STAGE7_CFG_PGA_BYPASS                                         23                               /* Bypass pga for this stage */
#define BITP_CT_CT_STAGE7_CFG_NOISE_REDUCTION_EN                                 22                               /* Enable frequency-hopping noise-reduction */
#define BITP_CT_CT_STAGE7_CFG_PGA_GAIN                                           20                               /* Stage specific PGA gain setup. */
#define BITP_CT_CT_STAGE7_CFG_C2V_IP_RANGE                                       16
#define BITP_CT_CT_STAGE7_CFG_EN_MAIN_DAC                                        15                               /* Enable Offset DAC */
#define BITP_CT_CT_STAGE7_CFG_PK2PK                                              13                               /* enable peak to peak measurements */
#define BITP_CT_CT_STAGE7_CFG_MAIN_DAC                                            3                               /* Main Dac offset 1LSB = 2*388.5fF */
#define BITP_CT_CT_STAGE7_CFG_SUB_DAC                                             0                               /* Sub Dac offset 1LSB = 2*48fF */
#define BITM_CT_CT_STAGE7_CFG_CIN_CON_POS_CDC_EN                                 (_ADI_MSK(0x08000000,uint32_t))  /* Enable selected cin to +ve CDC */
#define BITM_CT_CT_STAGE7_CFG_CIN_CON_POS_CDC                                    (_ADI_MSK(0x07000000,uint32_t))  /* Select Cin to connect to +ve CDC input */
#define BITM_CT_CT_STAGE7_CFG_PGA_BYPASS                                         (_ADI_MSK(0x00800000,uint32_t))  /* Bypass pga for this stage */
#define BITM_CT_CT_STAGE7_CFG_NOISE_REDUCTION_EN                                 (_ADI_MSK(0x00400000,uint32_t))  /* Enable frequency-hopping noise-reduction */
#define BITM_CT_CT_STAGE7_CFG_PGA_GAIN                                           (_ADI_MSK(0x00300000,uint32_t))  /* Stage specific PGA gain setup. */
#define BITM_CT_CT_STAGE7_CFG_C2V_IP_RANGE                                       (_ADI_MSK(0x000F0000,uint32_t))
#define BITM_CT_CT_STAGE7_CFG_EN_MAIN_DAC                                        (_ADI_MSK(0x00008000,uint32_t))  /* Enable Offset DAC */
#define BITM_CT_CT_STAGE7_CFG_PK2PK                                              (_ADI_MSK(0x00002000,uint32_t))  /* enable peak to peak measurements */
#define BITM_CT_CT_STAGE7_CFG_MAIN_DAC                                           (_ADI_MSK(0x000003F8,uint32_t))  /* Main Dac offset 1LSB = 2*388.5fF */
#define BITM_CT_CT_STAGE7_CFG_SUB_DAC                                            (_ADI_MSK(0x00000007,uint32_t))  /* Sub Dac offset 1LSB = 2*48fF */

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_STAGE8_CFG                                                         Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_STAGE8_CFG_CIN_CON_POS_CDC_EN                                 27                               /* Enable selected cin to +ve CDC */
#define BITP_CT_CT_STAGE8_CFG_CIN_CON_POS_CDC                                    24                               /* Select Cin to connect to +ve CDC input */
#define BITP_CT_CT_STAGE8_CFG_PGA_BYPASS                                         23                               /* Bypass pga for this stage */
#define BITP_CT_CT_STAGE8_CFG_NOISE_REDUCTION_EN                                 22                               /* Enable frequency-hopping noise-reduction */
#define BITP_CT_CT_STAGE8_CFG_PGA_GAIN                                           20                               /* Stage specific PGA gain setup. */
#define BITP_CT_CT_STAGE8_CFG_C2V_IP_RANGE                                       16
#define BITP_CT_CT_STAGE8_CFG_EN_MAIN_DAC                                        15                               /* Enable Offset DAC */
#define BITP_CT_CT_STAGE8_CFG_PK2PK                                              13                               /* enable peak to peak measurements */
#define BITP_CT_CT_STAGE8_CFG_MAIN_DAC                                            3                               /* Main Dac offset 1LSB = 2*388.5fF */
#define BITP_CT_CT_STAGE8_CFG_SUB_DAC                                             0                               /* Sub Dac offset 1LSB = 2*48fF */
#define BITM_CT_CT_STAGE8_CFG_CIN_CON_POS_CDC_EN                                 (_ADI_MSK(0x08000000,uint32_t))  /* Enable selected cin to +ve CDC */
#define BITM_CT_CT_STAGE8_CFG_CIN_CON_POS_CDC                                    (_ADI_MSK(0x07000000,uint32_t))  /* Select Cin to connect to +ve CDC input */
#define BITM_CT_CT_STAGE8_CFG_PGA_BYPASS                                         (_ADI_MSK(0x00800000,uint32_t))  /* Bypass pga for this stage */
#define BITM_CT_CT_STAGE8_CFG_NOISE_REDUCTION_EN                                 (_ADI_MSK(0x00400000,uint32_t))  /* Enable frequency-hopping noise-reduction */
#define BITM_CT_CT_STAGE8_CFG_PGA_GAIN                                           (_ADI_MSK(0x00300000,uint32_t))  /* Stage specific PGA gain setup. */
#define BITM_CT_CT_STAGE8_CFG_C2V_IP_RANGE                                       (_ADI_MSK(0x000F0000,uint32_t))
#define BITM_CT_CT_STAGE8_CFG_EN_MAIN_DAC                                        (_ADI_MSK(0x00008000,uint32_t))  /* Enable Offset DAC */
#define BITM_CT_CT_STAGE8_CFG_PK2PK                                              (_ADI_MSK(0x00002000,uint32_t))  /* enable peak to peak measurements */
#define BITM_CT_CT_STAGE8_CFG_MAIN_DAC                                           (_ADI_MSK(0x000003F8,uint32_t))  /* Main Dac offset 1LSB = 2*388.5fF */
#define BITM_CT_CT_STAGE8_CFG_SUB_DAC                                            (_ADI_MSK(0x00000007,uint32_t))  /* Sub Dac offset 1LSB = 2*48fF */

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_STAGE9_CFG                                                         Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_STAGE9_CFG_CIN_CON_POS_CDC_EN                                 27                               /* Enable selected cin to +ve CDC */
#define BITP_CT_CT_STAGE9_CFG_CIN_CON_POS_CDC                                    24                               /* Select Cin to connect to +ve CDC input */
#define BITP_CT_CT_STAGE9_CFG_PGA_BYPASS                                         23                               /* Bypass pga for this stage */
#define BITP_CT_CT_STAGE9_CFG_NOISE_REDUCTION_EN                                 22                               /* Enable frequency-hopping noise-reduction */
#define BITP_CT_CT_STAGE9_CFG_PGA_GAIN                                           20                               /* Stage specific PGA gain setup. */
#define BITP_CT_CT_STAGE9_CFG_C2V_IP_RANGE                                       16
#define BITP_CT_CT_STAGE9_CFG_EN_MAIN_DAC                                        15                               /* Enable Offset DAC */
#define BITP_CT_CT_STAGE9_CFG_PK2PK                                              13                               /* enable peak to peak measurements */
#define BITP_CT_CT_STAGE9_CFG_MAIN_DAC                                            3                               /* Main Dac offset 1LSB = 2*388.5fF */
#define BITP_CT_CT_STAGE9_CFG_SUB_DAC                                             0                               /* Sub Dac offset 1LSB = 2*48fF */
#define BITM_CT_CT_STAGE9_CFG_CIN_CON_POS_CDC_EN                                 (_ADI_MSK(0x08000000,uint32_t))  /* Enable selected cin to +ve CDC */
#define BITM_CT_CT_STAGE9_CFG_CIN_CON_POS_CDC                                    (_ADI_MSK(0x07000000,uint32_t))  /* Select Cin to connect to +ve CDC input */
#define BITM_CT_CT_STAGE9_CFG_PGA_BYPASS                                         (_ADI_MSK(0x00800000,uint32_t))  /* Bypass pga for this stage */
#define BITM_CT_CT_STAGE9_CFG_NOISE_REDUCTION_EN                                 (_ADI_MSK(0x00400000,uint32_t))  /* Enable frequency-hopping noise-reduction */
#define BITM_CT_CT_STAGE9_CFG_PGA_GAIN                                           (_ADI_MSK(0x00300000,uint32_t))  /* Stage specific PGA gain setup. */
#define BITM_CT_CT_STAGE9_CFG_C2V_IP_RANGE                                       (_ADI_MSK(0x000F0000,uint32_t))
#define BITM_CT_CT_STAGE9_CFG_EN_MAIN_DAC                                        (_ADI_MSK(0x00008000,uint32_t))  /* Enable Offset DAC */
#define BITM_CT_CT_STAGE9_CFG_PK2PK                                              (_ADI_MSK(0x00002000,uint32_t))  /* enable peak to peak measurements */
#define BITM_CT_CT_STAGE9_CFG_MAIN_DAC                                           (_ADI_MSK(0x000003F8,uint32_t))  /* Main Dac offset 1LSB = 2*388.5fF */
#define BITM_CT_CT_STAGE9_CFG_SUB_DAC                                            (_ADI_MSK(0x00000007,uint32_t))  /* Sub Dac offset 1LSB = 2*48fF */

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_STAGE10_CFG                                                        Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_STAGE10_CFG_CIN_CON_POS_CDC_EN                                27                               /* Enable selected cin to +ve CDC */
#define BITP_CT_CT_STAGE10_CFG_CIN_CON_POS_CDC                                   24                               /* Select Cin to connect to +ve CDC input */
#define BITP_CT_CT_STAGE10_CFG_PGA_BYPASS                                        23                               /* Bypass pga for this stage */
#define BITP_CT_CT_STAGE10_CFG_NOISE_REDUCTION_EN                                22                               /* Enable frequency-hopping noise-reduction */
#define BITP_CT_CT_STAGE10_CFG_PGA_GAIN                                          20                               /* Stage specific PGA gain setup. */
#define BITP_CT_CT_STAGE10_CFG_C2V_IP_RANGE                                      16
#define BITP_CT_CT_STAGE10_CFG_EN_MAIN_DAC                                       15                               /* Enable Offset DAC */
#define BITP_CT_CT_STAGE10_CFG_PK2PK                                             13                               /* enable peak to peak measurements */
#define BITP_CT_CT_STAGE10_CFG_MAIN_DAC                                           3                               /* Main Dac offset 1LSB = 2*388.5fF */
#define BITP_CT_CT_STAGE10_CFG_SUB_DAC                                            0                               /* Sub Dac offset 1LSB = 2*48fF */
#define BITM_CT_CT_STAGE10_CFG_CIN_CON_POS_CDC_EN                                (_ADI_MSK(0x08000000,uint32_t))  /* Enable selected cin to +ve CDC */
#define BITM_CT_CT_STAGE10_CFG_CIN_CON_POS_CDC                                   (_ADI_MSK(0x07000000,uint32_t))  /* Select Cin to connect to +ve CDC input */
#define BITM_CT_CT_STAGE10_CFG_PGA_BYPASS                                        (_ADI_MSK(0x00800000,uint32_t))  /* Bypass pga for this stage */
#define BITM_CT_CT_STAGE10_CFG_NOISE_REDUCTION_EN                                (_ADI_MSK(0x00400000,uint32_t))  /* Enable frequency-hopping noise-reduction */
#define BITM_CT_CT_STAGE10_CFG_PGA_GAIN                                          (_ADI_MSK(0x00300000,uint32_t))  /* Stage specific PGA gain setup. */
#define BITM_CT_CT_STAGE10_CFG_C2V_IP_RANGE                                      (_ADI_MSK(0x000F0000,uint32_t))
#define BITM_CT_CT_STAGE10_CFG_EN_MAIN_DAC                                       (_ADI_MSK(0x00008000,uint32_t))  /* Enable Offset DAC */
#define BITM_CT_CT_STAGE10_CFG_PK2PK                                             (_ADI_MSK(0x00002000,uint32_t))  /* enable peak to peak measurements */
#define BITM_CT_CT_STAGE10_CFG_MAIN_DAC                                          (_ADI_MSK(0x000003F8,uint32_t))  /* Main Dac offset 1LSB = 2*388.5fF */
#define BITM_CT_CT_STAGE10_CFG_SUB_DAC                                           (_ADI_MSK(0x00000007,uint32_t))  /* Sub Dac offset 1LSB = 2*48fF */

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_STAGE11_CFG                                                        Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_STAGE11_CFG_CIN_CON_POS_CDC_EN                                27                               /* Enable selected cin to +ve CDC */
#define BITP_CT_CT_STAGE11_CFG_CIN_CON_POS_CDC                                   24                               /* Select Cin to connect to +ve CDC input */
#define BITP_CT_CT_STAGE11_CFG_PGA_BYPASS                                        23                               /* Bypass pga for this stage */
#define BITP_CT_CT_STAGE11_CFG_NOISE_REDUCTION_EN                                22                               /* Enable frequency-hopping noise-reduction */
#define BITP_CT_CT_STAGE11_CFG_PGA_GAIN                                          20                               /* Stage specific PGA gain setup. */
#define BITP_CT_CT_STAGE11_CFG_C2V_IP_RANGE                                      16
#define BITP_CT_CT_STAGE11_CFG_EN_MAIN_DAC                                       15                               /* Enable Offset DAC */
#define BITP_CT_CT_STAGE11_CFG_PK2PK                                             13                               /* enable peak to peak measurements */
#define BITP_CT_CT_STAGE11_CFG_MAIN_DAC                                           3                               /* Main Dac offset 1LSB = 2*388.5fF */
#define BITP_CT_CT_STAGE11_CFG_SUB_DAC                                            0                               /* Sub Dac offset 1LSB = 2*48fF */
#define BITM_CT_CT_STAGE11_CFG_CIN_CON_POS_CDC_EN                                (_ADI_MSK(0x08000000,uint32_t))  /* Enable selected cin to +ve CDC */
#define BITM_CT_CT_STAGE11_CFG_CIN_CON_POS_CDC                                   (_ADI_MSK(0x07000000,uint32_t))  /* Select Cin to connect to +ve CDC input */
#define BITM_CT_CT_STAGE11_CFG_PGA_BYPASS                                        (_ADI_MSK(0x00800000,uint32_t))  /* Bypass pga for this stage */
#define BITM_CT_CT_STAGE11_CFG_NOISE_REDUCTION_EN                                (_ADI_MSK(0x00400000,uint32_t))  /* Enable frequency-hopping noise-reduction */
#define BITM_CT_CT_STAGE11_CFG_PGA_GAIN                                          (_ADI_MSK(0x00300000,uint32_t))  /* Stage specific PGA gain setup. */
#define BITM_CT_CT_STAGE11_CFG_C2V_IP_RANGE                                      (_ADI_MSK(0x000F0000,uint32_t))
#define BITM_CT_CT_STAGE11_CFG_EN_MAIN_DAC                                       (_ADI_MSK(0x00008000,uint32_t))  /* Enable Offset DAC */
#define BITM_CT_CT_STAGE11_CFG_PK2PK                                             (_ADI_MSK(0x00002000,uint32_t))  /* enable peak to peak measurements */
#define BITM_CT_CT_STAGE11_CFG_MAIN_DAC                                          (_ADI_MSK(0x000003F8,uint32_t))  /* Main Dac offset 1LSB = 2*388.5fF */
#define BITM_CT_CT_STAGE11_CFG_SUB_DAC                                           (_ADI_MSK(0x00000007,uint32_t))  /* Sub Dac offset 1LSB = 2*48fF */

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_STAGE12_CFG                                                        Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_STAGE12_CFG_CIN_CON_POS_CDC_EN                                27                               /* Enable selected cin to +ve CDC */
#define BITP_CT_CT_STAGE12_CFG_CIN_CON_POS_CDC                                   24                               /* Select Cin to connect to +ve CDC input */
#define BITP_CT_CT_STAGE12_CFG_PGA_BYPASS                                        23                               /* Bypass pga for this stage */
#define BITP_CT_CT_STAGE12_CFG_NOISE_REDUCTION_EN                                22                               /* Enable frequency-hopping noise-reduction */
#define BITP_CT_CT_STAGE12_CFG_PGA_GAIN                                          20                               /* Stage specific PGA gain setup. */
#define BITP_CT_CT_STAGE12_CFG_C2V_IP_RANGE                                      16
#define BITP_CT_CT_STAGE12_CFG_EN_MAIN_DAC                                       15                               /* Enable Offset DAC */
#define BITP_CT_CT_STAGE12_CFG_PK2PK                                             13                               /* enable peak to peak measurements */
#define BITP_CT_CT_STAGE12_CFG_MAIN_DAC                                           3                               /* Main Dac offset 1LSB = 2*388.5fF */
#define BITP_CT_CT_STAGE12_CFG_SUB_DAC                                            0                               /* Sub Dac offset 1LSB = 2*48fF */
#define BITM_CT_CT_STAGE12_CFG_CIN_CON_POS_CDC_EN                                (_ADI_MSK(0x08000000,uint32_t))  /* Enable selected cin to +ve CDC */
#define BITM_CT_CT_STAGE12_CFG_CIN_CON_POS_CDC                                   (_ADI_MSK(0x07000000,uint32_t))  /* Select Cin to connect to +ve CDC input */
#define BITM_CT_CT_STAGE12_CFG_PGA_BYPASS                                        (_ADI_MSK(0x00800000,uint32_t))  /* Bypass pga for this stage */
#define BITM_CT_CT_STAGE12_CFG_NOISE_REDUCTION_EN                                (_ADI_MSK(0x00400000,uint32_t))  /* Enable frequency-hopping noise-reduction */
#define BITM_CT_CT_STAGE12_CFG_PGA_GAIN                                          (_ADI_MSK(0x00300000,uint32_t))  /* Stage specific PGA gain setup. */
#define BITM_CT_CT_STAGE12_CFG_C2V_IP_RANGE                                      (_ADI_MSK(0x000F0000,uint32_t))
#define BITM_CT_CT_STAGE12_CFG_EN_MAIN_DAC                                       (_ADI_MSK(0x00008000,uint32_t))  /* Enable Offset DAC */
#define BITM_CT_CT_STAGE12_CFG_PK2PK                                             (_ADI_MSK(0x00002000,uint32_t))  /* enable peak to peak measurements */
#define BITM_CT_CT_STAGE12_CFG_MAIN_DAC                                          (_ADI_MSK(0x000003F8,uint32_t))  /* Main Dac offset 1LSB = 2*388.5fF */
#define BITM_CT_CT_STAGE12_CFG_SUB_DAC                                           (_ADI_MSK(0x00000007,uint32_t))  /* Sub Dac offset 1LSB = 2*48fF */

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_STAGE13_CFG                                                        Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_STAGE13_CFG_CIN_CON_POS_CDC_EN                                27                               /* Enable selected cin to +ve CDC */
#define BITP_CT_CT_STAGE13_CFG_CIN_CON_POS_CDC                                   24                               /* Select Cin to connect to +ve CDC input */
#define BITP_CT_CT_STAGE13_CFG_PGA_BYPASS                                        23                               /* Bypass pga for this stage */
#define BITP_CT_CT_STAGE13_CFG_NOISE_REDUCTION_EN                                22                               /* Enable frequency-hopping noise-reduction */
#define BITP_CT_CT_STAGE13_CFG_PGA_GAIN                                          20                               /* Stage specific PGA gain setup. */
#define BITP_CT_CT_STAGE13_CFG_C2V_IP_RANGE                                      16
#define BITP_CT_CT_STAGE13_CFG_EN_MAIN_DAC                                       15                               /* Enable Offset DAC */
#define BITP_CT_CT_STAGE13_CFG_PK2PK                                             13                               /* enable peak to peak measurements */
#define BITP_CT_CT_STAGE13_CFG_MAIN_DAC                                           3                               /* Main Dac offset 1LSB = 2*388.5fF */
#define BITP_CT_CT_STAGE13_CFG_SUB_DAC                                            0                               /* Sub Dac offset 1LSB = 2*48fF */
#define BITM_CT_CT_STAGE13_CFG_CIN_CON_POS_CDC_EN                                (_ADI_MSK(0x08000000,uint32_t))  /* Enable selected cin to +ve CDC */
#define BITM_CT_CT_STAGE13_CFG_CIN_CON_POS_CDC                                   (_ADI_MSK(0x07000000,uint32_t))  /* Select Cin to connect to +ve CDC input */
#define BITM_CT_CT_STAGE13_CFG_PGA_BYPASS                                        (_ADI_MSK(0x00800000,uint32_t))  /* Bypass pga for this stage */
#define BITM_CT_CT_STAGE13_CFG_NOISE_REDUCTION_EN                                (_ADI_MSK(0x00400000,uint32_t))  /* Enable frequency-hopping noise-reduction */
#define BITM_CT_CT_STAGE13_CFG_PGA_GAIN                                          (_ADI_MSK(0x00300000,uint32_t))  /* Stage specific PGA gain setup. */
#define BITM_CT_CT_STAGE13_CFG_C2V_IP_RANGE                                      (_ADI_MSK(0x000F0000,uint32_t))
#define BITM_CT_CT_STAGE13_CFG_EN_MAIN_DAC                                       (_ADI_MSK(0x00008000,uint32_t))  /* Enable Offset DAC */
#define BITM_CT_CT_STAGE13_CFG_PK2PK                                             (_ADI_MSK(0x00002000,uint32_t))  /* enable peak to peak measurements */
#define BITM_CT_CT_STAGE13_CFG_MAIN_DAC                                          (_ADI_MSK(0x000003F8,uint32_t))  /* Main Dac offset 1LSB = 2*388.5fF */
#define BITM_CT_CT_STAGE13_CFG_SUB_DAC                                           (_ADI_MSK(0x00000007,uint32_t))  /* Sub Dac offset 1LSB = 2*48fF */

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_STAGE14_CFG                                                        Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_STAGE14_CFG_CIN_CON_POS_CDC_EN                                27                               /* Enable selected cin to +ve CDC */
#define BITP_CT_CT_STAGE14_CFG_CIN_CON_POS_CDC                                   24                               /* Select Cin to connect to +ve CDC input */
#define BITP_CT_CT_STAGE14_CFG_PGA_BYPASS                                        23                               /* Bypass pga for this stage */
#define BITP_CT_CT_STAGE14_CFG_NOISE_REDUCTION_EN                                22                               /* Enable frequency-hopping noise-reduction */
#define BITP_CT_CT_STAGE14_CFG_PGA_GAIN                                          20                               /* Stage specific PGA gain setup. */
#define BITP_CT_CT_STAGE14_CFG_C2V_IP_RANGE                                      16
#define BITP_CT_CT_STAGE14_CFG_EN_MAIN_DAC                                       15                               /* Enable Offset DAC */
#define BITP_CT_CT_STAGE14_CFG_PK2PK                                             13                               /* enable peak to peak measurements */
#define BITP_CT_CT_STAGE14_CFG_MAIN_DAC                                           3                               /* Main Dac offset 1LSB = 2*388.5fF */
#define BITP_CT_CT_STAGE14_CFG_SUB_DAC                                            0                               /* Sub Dac offset 1LSB = 2*48fF */
#define BITM_CT_CT_STAGE14_CFG_CIN_CON_POS_CDC_EN                                (_ADI_MSK(0x08000000,uint32_t))  /* Enable selected cin to +ve CDC */
#define BITM_CT_CT_STAGE14_CFG_CIN_CON_POS_CDC                                   (_ADI_MSK(0x07000000,uint32_t))  /* Select Cin to connect to +ve CDC input */
#define BITM_CT_CT_STAGE14_CFG_PGA_BYPASS                                        (_ADI_MSK(0x00800000,uint32_t))  /* Bypass pga for this stage */
#define BITM_CT_CT_STAGE14_CFG_NOISE_REDUCTION_EN                                (_ADI_MSK(0x00400000,uint32_t))  /* Enable frequency-hopping noise-reduction */
#define BITM_CT_CT_STAGE14_CFG_PGA_GAIN                                          (_ADI_MSK(0x00300000,uint32_t))  /* Stage specific PGA gain setup. */
#define BITM_CT_CT_STAGE14_CFG_C2V_IP_RANGE                                      (_ADI_MSK(0x000F0000,uint32_t))
#define BITM_CT_CT_STAGE14_CFG_EN_MAIN_DAC                                       (_ADI_MSK(0x00008000,uint32_t))  /* Enable Offset DAC */
#define BITM_CT_CT_STAGE14_CFG_PK2PK                                             (_ADI_MSK(0x00002000,uint32_t))  /* enable peak to peak measurements */
#define BITM_CT_CT_STAGE14_CFG_MAIN_DAC                                          (_ADI_MSK(0x000003F8,uint32_t))  /* Main Dac offset 1LSB = 2*388.5fF */
#define BITM_CT_CT_STAGE14_CFG_SUB_DAC                                           (_ADI_MSK(0x00000007,uint32_t))  /* Sub Dac offset 1LSB = 2*48fF */

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_STAGE15_CFG                                                        Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_STAGE15_CFG_CIN_CON_POS_CDC_EN                                27                               /* Enable selected cin to +ve CDC */
#define BITP_CT_CT_STAGE15_CFG_CIN_CON_POS_CDC                                   24                               /* Select Cin to connect to +ve CDC input */
#define BITP_CT_CT_STAGE15_CFG_PGA_BYPASS                                        23                               /* Bypass pga for this stage */
#define BITP_CT_CT_STAGE15_CFG_NOISE_REDUCTION_EN                                22                               /* Enable frequency-hopping noise-reduction */
#define BITP_CT_CT_STAGE15_CFG_PGA_GAIN                                          20                               /* Stage specific PGA gain setup. */
#define BITP_CT_CT_STAGE15_CFG_C2V_IP_RANGE                                      16
#define BITP_CT_CT_STAGE15_CFG_EN_MAIN_DAC                                       15                               /* Enable Offset DAC */
#define BITP_CT_CT_STAGE15_CFG_PK2PK                                             13                               /* enable peak to peak measurements */
#define BITP_CT_CT_STAGE15_CFG_MAIN_DAC                                           3                               /* Main Dac offset 1LSB = 2*388.5fF */
#define BITP_CT_CT_STAGE15_CFG_SUB_DAC                                            0                               /* Sub Dac offset 1LSB = 2*48fF */
#define BITM_CT_CT_STAGE15_CFG_CIN_CON_POS_CDC_EN                                (_ADI_MSK(0x08000000,uint32_t))  /* Enable selected cin to +ve CDC */
#define BITM_CT_CT_STAGE15_CFG_CIN_CON_POS_CDC                                   (_ADI_MSK(0x07000000,uint32_t))  /* Select Cin to connect to +ve CDC input */
#define BITM_CT_CT_STAGE15_CFG_PGA_BYPASS                                        (_ADI_MSK(0x00800000,uint32_t))  /* Bypass pga for this stage */
#define BITM_CT_CT_STAGE15_CFG_NOISE_REDUCTION_EN                                (_ADI_MSK(0x00400000,uint32_t))  /* Enable frequency-hopping noise-reduction */
#define BITM_CT_CT_STAGE15_CFG_PGA_GAIN                                          (_ADI_MSK(0x00300000,uint32_t))  /* Stage specific PGA gain setup. */
#define BITM_CT_CT_STAGE15_CFG_C2V_IP_RANGE                                      (_ADI_MSK(0x000F0000,uint32_t))
#define BITM_CT_CT_STAGE15_CFG_EN_MAIN_DAC                                       (_ADI_MSK(0x00008000,uint32_t))  /* Enable Offset DAC */
#define BITM_CT_CT_STAGE15_CFG_PK2PK                                             (_ADI_MSK(0x00002000,uint32_t))  /* enable peak to peak measurements */
#define BITM_CT_CT_STAGE15_CFG_MAIN_DAC                                          (_ADI_MSK(0x000003F8,uint32_t))  /* Main Dac offset 1LSB = 2*388.5fF */
#define BITM_CT_CT_STAGE15_CFG_SUB_DAC                                           (_ADI_MSK(0x00000007,uint32_t))  /* Sub Dac offset 1LSB = 2*48fF */

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_SENSOR_THR_CFG0                                                    Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_SENSOR_THR_CFG0_SENSOR_THRESHOLD1                             16
#define BITP_CT_CT_SENSOR_THR_CFG0_SENSOR_THRESHOLD0                              0
#define BITM_CT_CT_SENSOR_THR_CFG0_SENSOR_THRESHOLD1                             (_ADI_MSK(0x3FFF0000,uint32_t))
#define BITM_CT_CT_SENSOR_THR_CFG0_SENSOR_THRESHOLD0                             (_ADI_MSK(0x00003FFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_SENSOR_THR_CFG1                                                    Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_SENSOR_THR_CFG1_SENSOR_THRESHOLD3                             16
#define BITP_CT_CT_SENSOR_THR_CFG1_SENSOR_THRESHOLD2                              0
#define BITM_CT_CT_SENSOR_THR_CFG1_SENSOR_THRESHOLD3                             (_ADI_MSK(0x3FFF0000,uint32_t))
#define BITM_CT_CT_SENSOR_THR_CFG1_SENSOR_THRESHOLD2                             (_ADI_MSK(0x00003FFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_SENSOR_THR_CFG2                                                    Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_SENSOR_THR_CFG2_SENSOR_THRESHOLD5                             16
#define BITP_CT_CT_SENSOR_THR_CFG2_SENSOR_THRESHOLD4                              0
#define BITM_CT_CT_SENSOR_THR_CFG2_SENSOR_THRESHOLD5                             (_ADI_MSK(0x3FFF0000,uint32_t))
#define BITM_CT_CT_SENSOR_THR_CFG2_SENSOR_THRESHOLD4                             (_ADI_MSK(0x00003FFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_SENSOR_THR_CFG3                                                    Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_SENSOR_THR_CFG3_SENSOR_THRESHOLD7                             16
#define BITP_CT_CT_SENSOR_THR_CFG3_SENSOR_THRESHOLD6                              0
#define BITM_CT_CT_SENSOR_THR_CFG3_SENSOR_THRESHOLD7                             (_ADI_MSK(0x3FFF0000,uint32_t))
#define BITM_CT_CT_SENSOR_THR_CFG3_SENSOR_THRESHOLD6                             (_ADI_MSK(0x00003FFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_SENSOR_THR_CFG4                                                    Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_SENSOR_THR_CFG4_SENSOR_THRESHOLD9                             16
#define BITP_CT_CT_SENSOR_THR_CFG4_SENSOR_THRESHOLD8                              0
#define BITM_CT_CT_SENSOR_THR_CFG4_SENSOR_THRESHOLD9                             (_ADI_MSK(0x3FFF0000,uint32_t))
#define BITM_CT_CT_SENSOR_THR_CFG4_SENSOR_THRESHOLD8                             (_ADI_MSK(0x00003FFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_SENSOR_THR_CFG5                                                    Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_SENSOR_THR_CFG5_SENSOR_THRESHOLD11                            16
#define BITP_CT_CT_SENSOR_THR_CFG5_SENSOR_THRESHOLD10                             0
#define BITM_CT_CT_SENSOR_THR_CFG5_SENSOR_THRESHOLD11                            (_ADI_MSK(0x3FFF0000,uint32_t))
#define BITM_CT_CT_SENSOR_THR_CFG5_SENSOR_THRESHOLD10                            (_ADI_MSK(0x00003FFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_SENSOR_THR_CFG6                                                    Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_SENSOR_THR_CFG6_SENSOR_THRESHOLD13                            16
#define BITP_CT_CT_SENSOR_THR_CFG6_SENSOR_THRESHOLD12                             0
#define BITM_CT_CT_SENSOR_THR_CFG6_SENSOR_THRESHOLD13                            (_ADI_MSK(0x3FFF0000,uint32_t))
#define BITM_CT_CT_SENSOR_THR_CFG6_SENSOR_THRESHOLD12                            (_ADI_MSK(0x00003FFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_SENSOR_THR_CFG7                                                    Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_SENSOR_THR_CFG7_SENSOR_THRESHOLD15                            16
#define BITP_CT_CT_SENSOR_THR_CFG7_SENSOR_THRESHOLD14                             0
#define BITM_CT_CT_SENSOR_THR_CFG7_SENSOR_THRESHOLD15                            (_ADI_MSK(0x3FFF0000,uint32_t))
#define BITM_CT_CT_SENSOR_THR_CFG7_SENSOR_THRESHOLD14                            (_ADI_MSK(0x00003FFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_CDC_RES0                                                           Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_CDC_RES0_SIGN1                                                31
#define BITP_CT_CT_CDC_RES0_CDC_RES1                                             16
#define BITP_CT_CT_CDC_RES0_SIGN0                                                15
#define BITP_CT_CT_CDC_RES0_CDC_RES0                                              0
#define BITM_CT_CT_CDC_RES0_SIGN1                                                (_ADI_MSK(0x80000000,uint32_t))
#define BITM_CT_CT_CDC_RES0_CDC_RES1                                             (_ADI_MSK(0x3FFF0000,uint32_t))
#define BITM_CT_CT_CDC_RES0_SIGN0                                                (_ADI_MSK(0x00008000,uint32_t))
#define BITM_CT_CT_CDC_RES0_CDC_RES0                                             (_ADI_MSK(0x00003FFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_CDC_RES1                                                           Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_CDC_RES1_SIGN3                                                31
#define BITP_CT_CT_CDC_RES1_CDC_RES3                                             16
#define BITP_CT_CT_CDC_RES1_SIGN2                                                15
#define BITP_CT_CT_CDC_RES1_CDC_RES2                                              0
#define BITM_CT_CT_CDC_RES1_SIGN3                                                (_ADI_MSK(0x80000000,uint32_t))
#define BITM_CT_CT_CDC_RES1_CDC_RES3                                             (_ADI_MSK(0x3FFF0000,uint32_t))
#define BITM_CT_CT_CDC_RES1_SIGN2                                                (_ADI_MSK(0x00008000,uint32_t))
#define BITM_CT_CT_CDC_RES1_CDC_RES2                                             (_ADI_MSK(0x00003FFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_CDC_RES2                                                           Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_CDC_RES2_SIGN5                                                31
#define BITP_CT_CT_CDC_RES2_CDC_RES5                                             16
#define BITP_CT_CT_CDC_RES2_SIGN4                                                15
#define BITP_CT_CT_CDC_RES2_CDC_RES4                                              0
#define BITM_CT_CT_CDC_RES2_SIGN5                                                (_ADI_MSK(0x80000000,uint32_t))
#define BITM_CT_CT_CDC_RES2_CDC_RES5                                             (_ADI_MSK(0x3FFF0000,uint32_t))
#define BITM_CT_CT_CDC_RES2_SIGN4                                                (_ADI_MSK(0x00008000,uint32_t))
#define BITM_CT_CT_CDC_RES2_CDC_RES4                                             (_ADI_MSK(0x00003FFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_CDC_RES3                                                           Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_CDC_RES3_SIGN7                                                31
#define BITP_CT_CT_CDC_RES3_CDC_RES7                                             16
#define BITP_CT_CT_CDC_RES3_SIGN6                                                15
#define BITP_CT_CT_CDC_RES3_CDC_RES6                                              0
#define BITM_CT_CT_CDC_RES3_SIGN7                                                (_ADI_MSK(0x80000000,uint32_t))
#define BITM_CT_CT_CDC_RES3_CDC_RES7                                             (_ADI_MSK(0x3FFF0000,uint32_t))
#define BITM_CT_CT_CDC_RES3_SIGN6                                                (_ADI_MSK(0x00008000,uint32_t))
#define BITM_CT_CT_CDC_RES3_CDC_RES6                                             (_ADI_MSK(0x00003FFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_CDC_RES4                                                           Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_CDC_RES4_SIGN9                                                31
#define BITP_CT_CT_CDC_RES4_CDC_RES9                                             16
#define BITP_CT_CT_CDC_RES4_SIGN8                                                15
#define BITP_CT_CT_CDC_RES4_CDC_RES8                                              0
#define BITM_CT_CT_CDC_RES4_SIGN9                                                (_ADI_MSK(0x80000000,uint32_t))
#define BITM_CT_CT_CDC_RES4_CDC_RES9                                             (_ADI_MSK(0x3FFF0000,uint32_t))
#define BITM_CT_CT_CDC_RES4_SIGN8                                                (_ADI_MSK(0x00008000,uint32_t))
#define BITM_CT_CT_CDC_RES4_CDC_RES8                                             (_ADI_MSK(0x00003FFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_CDC_RES5                                                           Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_CDC_RES5_SIGN11                                               31
#define BITP_CT_CT_CDC_RES5_CDC_RES11                                            16
#define BITP_CT_CT_CDC_RES5_SIGN10                                               15
#define BITP_CT_CT_CDC_RES5_CDC_RES10                                             0
#define BITM_CT_CT_CDC_RES5_SIGN11                                               (_ADI_MSK(0x80000000,uint32_t))
#define BITM_CT_CT_CDC_RES5_CDC_RES11                                            (_ADI_MSK(0x3FFF0000,uint32_t))
#define BITM_CT_CT_CDC_RES5_SIGN10                                               (_ADI_MSK(0x00008000,uint32_t))
#define BITM_CT_CT_CDC_RES5_CDC_RES10                                            (_ADI_MSK(0x00003FFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_CDC_RES6                                                           Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_CDC_RES6_SIGN13                                               31
#define BITP_CT_CT_CDC_RES6_CDC_RES13                                            16
#define BITP_CT_CT_CDC_RES6_SIGN12                                               15
#define BITP_CT_CT_CDC_RES6_CDC_RES12                                             0
#define BITM_CT_CT_CDC_RES6_SIGN13                                               (_ADI_MSK(0x80000000,uint32_t))
#define BITM_CT_CT_CDC_RES6_CDC_RES13                                            (_ADI_MSK(0x3FFF0000,uint32_t))
#define BITM_CT_CT_CDC_RES6_SIGN12                                               (_ADI_MSK(0x00008000,uint32_t))
#define BITM_CT_CT_CDC_RES6_CDC_RES12                                            (_ADI_MSK(0x00003FFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_CDC_RES7                                                           Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_CDC_RES7_SIGN15                                               31
#define BITP_CT_CT_CDC_RES7_CDC_RES15                                            16
#define BITP_CT_CT_CDC_RES7_SIGN14                                               15
#define BITP_CT_CT_CDC_RES7_CDC_RES14                                             0
#define BITM_CT_CT_CDC_RES7_SIGN15                                               (_ADI_MSK(0x80000000,uint32_t))
#define BITM_CT_CT_CDC_RES7_CDC_RES15                                            (_ADI_MSK(0x3FFF0000,uint32_t))
#define BITM_CT_CT_CDC_RES7_SIGN14                                               (_ADI_MSK(0x00008000,uint32_t))
#define BITM_CT_CT_CDC_RES7_CDC_RES14                                            (_ADI_MSK(0x00003FFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_BASELINE0                                                          Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_BASELINE0_CDC_AVG0                                            16
#define BITP_CT_CT_BASELINE0_BASELINE0                                            0
#define BITM_CT_CT_BASELINE0_CDC_AVG0                                            (_ADI_MSK(0x3FFF0000,uint32_t))
#define BITM_CT_CT_BASELINE0_BASELINE0                                           (_ADI_MSK(0x00003FFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_BASELINE1                                                          Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_BASELINE1_CDC_AVG1                                            16
#define BITP_CT_CT_BASELINE1_BASELINE1                                            0
#define BITM_CT_CT_BASELINE1_CDC_AVG1                                            (_ADI_MSK(0x3FFF0000,uint32_t))
#define BITM_CT_CT_BASELINE1_BASELINE1                                           (_ADI_MSK(0x00003FFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_BASELINE2                                                          Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_BASELINE2_CDC_AVG2                                            16
#define BITP_CT_CT_BASELINE2_BASELINE2                                            0
#define BITM_CT_CT_BASELINE2_CDC_AVG2                                            (_ADI_MSK(0x3FFF0000,uint32_t))
#define BITM_CT_CT_BASELINE2_BASELINE2                                           (_ADI_MSK(0x00003FFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_BASELINE3                                                          Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_BASELINE3_CDC_AVG3                                            16
#define BITP_CT_CT_BASELINE3_BASELINE3                                            0
#define BITM_CT_CT_BASELINE3_CDC_AVG3                                            (_ADI_MSK(0x3FFF0000,uint32_t))
#define BITM_CT_CT_BASELINE3_BASELINE3                                           (_ADI_MSK(0x00003FFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_BASELINE4                                                          Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_BASELINE4_CDC_AVG4                                            16
#define BITP_CT_CT_BASELINE4_BASELINE4                                            0
#define BITM_CT_CT_BASELINE4_CDC_AVG4                                            (_ADI_MSK(0x3FFF0000,uint32_t))
#define BITM_CT_CT_BASELINE4_BASELINE4                                           (_ADI_MSK(0x00003FFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_BASELINE5                                                          Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_BASELINE5_CDC_AVG5                                            16
#define BITP_CT_CT_BASELINE5_BASELINE5                                            0
#define BITM_CT_CT_BASELINE5_CDC_AVG5                                            (_ADI_MSK(0x3FFF0000,uint32_t))
#define BITM_CT_CT_BASELINE5_BASELINE5                                           (_ADI_MSK(0x00003FFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_BASELINE6                                                          Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_BASELINE6_CDC_AVG6                                            16
#define BITP_CT_CT_BASELINE6_BASELINE6                                            0
#define BITM_CT_CT_BASELINE6_CDC_AVG6                                            (_ADI_MSK(0x3FFF0000,uint32_t))
#define BITM_CT_CT_BASELINE6_BASELINE6                                           (_ADI_MSK(0x00003FFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_BASELINE7                                                          Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_BASELINE7_CDC_AVG7                                            16
#define BITP_CT_CT_BASELINE7_BASELINE7                                            0
#define BITM_CT_CT_BASELINE7_CDC_AVG7                                            (_ADI_MSK(0x3FFF0000,uint32_t))
#define BITM_CT_CT_BASELINE7_BASELINE7                                           (_ADI_MSK(0x00003FFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_BASELINE8                                                          Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_BASELINE8_CDC_AVG8                                            16
#define BITP_CT_CT_BASELINE8_BASELINE8                                            0
#define BITM_CT_CT_BASELINE8_CDC_AVG8                                            (_ADI_MSK(0x3FFF0000,uint32_t))
#define BITM_CT_CT_BASELINE8_BASELINE8                                           (_ADI_MSK(0x00003FFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_BASELINE9                                                          Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_BASELINE9_CDC_AVG9                                            16
#define BITP_CT_CT_BASELINE9_BASELINE9                                            0
#define BITM_CT_CT_BASELINE9_CDC_AVG9                                            (_ADI_MSK(0x3FFF0000,uint32_t))
#define BITM_CT_CT_BASELINE9_BASELINE9                                           (_ADI_MSK(0x00003FFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_BASELINE10                                                         Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_BASELINE10_CDC_AVG10                                          16
#define BITP_CT_CT_BASELINE10_BASELINE10                                          0
#define BITM_CT_CT_BASELINE10_CDC_AVG10                                          (_ADI_MSK(0x3FFF0000,uint32_t))
#define BITM_CT_CT_BASELINE10_BASELINE10                                         (_ADI_MSK(0x00003FFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_BASELINE11                                                         Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_BASELINE11_CDC_AVG11                                          16
#define BITP_CT_CT_BASELINE11_BASELINE11                                          0
#define BITM_CT_CT_BASELINE11_CDC_AVG11                                          (_ADI_MSK(0x3FFF0000,uint32_t))
#define BITM_CT_CT_BASELINE11_BASELINE11                                         (_ADI_MSK(0x00003FFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_BASELINE12                                                         Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_BASELINE12_CDC_AVG12                                          16
#define BITP_CT_CT_BASELINE12_BASELINE12                                          0
#define BITM_CT_CT_BASELINE12_CDC_AVG12                                          (_ADI_MSK(0x3FFF0000,uint32_t))
#define BITM_CT_CT_BASELINE12_BASELINE12                                         (_ADI_MSK(0x00003FFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_BASELINE13                                                         Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_BASELINE13_CDC_AVG13                                          16
#define BITP_CT_CT_BASELINE13_BASELINE13                                          0
#define BITM_CT_CT_BASELINE13_CDC_AVG13                                          (_ADI_MSK(0x3FFF0000,uint32_t))
#define BITM_CT_CT_BASELINE13_BASELINE13                                         (_ADI_MSK(0x00003FFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_BASELINE14                                                         Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_BASELINE14_CDC_AVG14                                          16
#define BITP_CT_CT_BASELINE14_BASELINE14                                          0
#define BITM_CT_CT_BASELINE14_CDC_AVG14                                          (_ADI_MSK(0x3FFF0000,uint32_t))
#define BITM_CT_CT_BASELINE14_BASELINE14                                         (_ADI_MSK(0x00003FFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_BASELINE15                                                         Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_BASELINE15_CDC_AVG15                                          16
#define BITP_CT_CT_BASELINE15_BASELINE15                                          0
#define BITM_CT_CT_BASELINE15_CDC_AVG15                                          (_ADI_MSK(0x3FFF0000,uint32_t))
#define BITM_CT_CT_BASELINE15_BASELINE15                                         (_ADI_MSK(0x00003FFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_PK2PK0                                                             Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_PK2PK0_PK2PK1                                                 16
#define BITP_CT_CT_PK2PK0_PK2PK0                                                  0
#define BITM_CT_CT_PK2PK0_PK2PK1                                                 (_ADI_MSK(0x3FFF0000,uint32_t))
#define BITM_CT_CT_PK2PK0_PK2PK0                                                 (_ADI_MSK(0x00003FFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_PK2PK1                                                             Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_PK2PK1_PK2PK3                                                 16
#define BITP_CT_CT_PK2PK1_PK2PK2                                                  0
#define BITM_CT_CT_PK2PK1_PK2PK3                                                 (_ADI_MSK(0x3FFF0000,uint32_t))
#define BITM_CT_CT_PK2PK1_PK2PK2                                                 (_ADI_MSK(0x00003FFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_PK2PK2                                                             Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_PK2PK2_PK2PK5                                                 16
#define BITP_CT_CT_PK2PK2_PK2PK4                                                  0
#define BITM_CT_CT_PK2PK2_PK2PK5                                                 (_ADI_MSK(0x3FFF0000,uint32_t))
#define BITM_CT_CT_PK2PK2_PK2PK4                                                 (_ADI_MSK(0x00003FFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_PK2PK3                                                             Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_PK2PK3_PK2PK7                                                 16
#define BITP_CT_CT_PK2PK3_PK2PK6                                                  0
#define BITM_CT_CT_PK2PK3_PK2PK7                                                 (_ADI_MSK(0x3FFF0000,uint32_t))
#define BITM_CT_CT_PK2PK3_PK2PK6                                                 (_ADI_MSK(0x00003FFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_PK2PK4                                                             Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_PK2PK4_PK2PK9                                                 16
#define BITP_CT_CT_PK2PK4_PK2PK8                                                  0
#define BITM_CT_CT_PK2PK4_PK2PK9                                                 (_ADI_MSK(0x3FFF0000,uint32_t))
#define BITM_CT_CT_PK2PK4_PK2PK8                                                 (_ADI_MSK(0x00003FFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_PK2PK5                                                             Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_PK2PK5_PK2PK11                                                16
#define BITP_CT_CT_PK2PK5_PK2PK10                                                 0
#define BITM_CT_CT_PK2PK5_PK2PK11                                                (_ADI_MSK(0x3FFF0000,uint32_t))
#define BITM_CT_CT_PK2PK5_PK2PK10                                                (_ADI_MSK(0x00003FFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_PK2PK6                                                             Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_PK2PK6_PK2PK13                                                16
#define BITP_CT_CT_PK2PK6_PK2PK12                                                 0
#define BITM_CT_CT_PK2PK6_PK2PK13                                                (_ADI_MSK(0x3FFF0000,uint32_t))
#define BITM_CT_CT_PK2PK6_PK2PK12                                                (_ADI_MSK(0x00003FFF,uint32_t))

/* ------------------------------------------------------------------------------------------------------------------------
        CT_CT_PK2PK7                                                             Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_CT_CT_PK2PK7_PK2PK15                                                16
#define BITP_CT_CT_PK2PK7_PK2PK14                                                 0
#define BITM_CT_CT_PK2PK7_PK2PK15                                                (_ADI_MSK(0x3FFF0000,uint32_t))
#define BITM_CT_CT_PK2PK7_PK2PK14                                                (_ADI_MSK(0x00003FFF,uint32_t))

/* ==================================================
        Universal Serial Bus Controller Registers
   ================================================== */

/* =========================
        USB0
   ========================= */
#define REG_USB0_FADDR                       0x400A0000         /* USB0 Function Address Register */
#define REG_USB0_POWER                       0x400A0001         /* USB0 Power and Device Control Register */
#define REG_USB0_INTRTX                      0x400A0002         /* USB0 Transmit Interrupt Register */
#define REG_USB0_INTRRX                      0x400A0004         /* USB0 Receive Interrupt Register */
#define REG_USB0_INTRTXE                     0x400A0006         /* USB0 Transmit Interrupt Enable Register */
#define REG_USB0_INTRRXE                     0x400A0008         /* USB0 Receive Interrupt Enable Register */
#define REG_USB0_IRQ                         0x400A000A         /* USB0 Common Interrupts Register */
#define REG_USB0_IEN                         0x400A000B         /* USB0 Common Interrupts Enable Register */
#define REG_USB0_FRAME                       0x400A000C         /* USB0 Frame Number Register */
#define REG_USB0_INDEX                       0x400A000E         /* USB0 Index Register */
#define REG_USB0_TESTMODE                    0x400A000F         /* USB0 Testmode Register */
#define REG_USB0_EPI_TXMAXP0                 0x400A0010         /* USB0 EPn Transmit Maximum Packet Length Register */
#define REG_USB0_EPI_TXCSR_H0                0x400A0012         /* USB0 EPn Transmit Configuration and Status (Host) Register */
#define REG_USB0_EP0I_CSR0_P                 0x400A0012         /* USB0 EP0 Configuration and Status (Peripheral) Register */
#define REG_USB0_EP0I_CSR0_H                 0x400A0012         /* USB0 EP0 Configuration and Status (Host) Register */
#define REG_USB0_EPI_TXCSR_P0                0x400A0012         /* USB0 EPn Transmit Configuration and Status (Peripheral) Register */
#define REG_USB0_EPI_RXMAXP0                 0x400A0014         /* USB0 EPn Receive Maximum Packet Length Register */
#define REG_USB0_EPI_RXCSR_H0                0x400A0016         /* USB0 EPn Receive Configuration and Status (Host) Register */
#define REG_USB0_EPI_RXCSR_P0                0x400A0016         /* USB0 EPn Receive Configuration and Status (Peripheral) Register */
#define REG_USB0_EP0I_CNT0                   0x400A0018         /* USB0 EP0 Number of Received Bytes Register */
#define REG_USB0_EPI_RXCNT0                  0x400A0018         /* USB0 EPn Number of Bytes Received Register */
#define REG_USB0_EP0I_CFGDATA0               0x400A001F         /* USB0 EP0 Configuration Information Register */
#define REG_USB0_EPI_FIFOSIZE0               0x400A001F         /* USB0 FIFO size */
#define REG_USB0_FIFO0                       0x400A0020         /* USB0 FIFO Word (32-Bit) Register */
#define REG_USB0_FIFO1                       0x400A0024         /* USB0 FIFO Word (32-Bit) Register */
#define REG_USB0_FIFO2                       0x400A0028         /* USB0 FIFO Word (32-Bit) Register */
#define REG_USB0_FIFO3                       0x400A002C         /* USB0 FIFO Word (32-Bit) Register */
#define REG_USB0_FIFOH0                      0x400A0020         /* USB0 FIFO Half-Word (16-Bit) Register */
#define REG_USB0_FIFOH1                      0x400A0024         /* USB0 FIFO Half-Word (16-Bit) Register */
#define REG_USB0_FIFOH2                      0x400A0028         /* USB0 FIFO Half-Word (16-Bit) Register */
#define REG_USB0_FIFOH3                      0x400A002C         /* USB0 FIFO Half-Word (16-Bit) Register */
#define REG_USB0_FIFOB0                      0x400A0020         /* USB0 FIFO Byte (8-Bit) Register */
#define REG_USB0_FIFOB1                      0x400A0024         /* USB0 FIFO Byte (8-Bit) Register */
#define REG_USB0_FIFOB2                      0x400A0028         /* USB0 FIFO Byte (8-Bit) Register */
#define REG_USB0_FIFOB3                      0x400A002C         /* USB0 FIFO Byte (8-Bit) Register */
#define REG_USB0_DEV_CTL                     0x400A0060         /* USB0 Device Control Register */
#define REG_USB0_MISC                        0x400A0061         /* USB0 Miscellaneous Register */
#define REG_USB0_EPINFO                      0x400A0078         /* USB0 Endpoint Information Register */
#define REG_USB0_RAMINFO                     0x400A0079         /* USB0 RAM Information Register */
#define REG_USB0_LINKINFO                    0x400A007A         /* USB0 Link Information Register */
#define REG_USB0_FS_EOF1                     0x400A007D         /* USB0 Full-Speed EOF 1 Register */
#define REG_USB0_SOFT_RST                    0x400A007F         /* USB0 Software Reset Register */
#define REG_USB0_EP0_TXMAXP                  0x400A0100         /* USB0 EPn Transmit Maximum Packet Length Register */
#define REG_USB0_EP1_TXMAXP                  0x400A0110         /* USB0 EPn Transmit Maximum Packet Length Register */
#define REG_USB0_EP2_TXMAXP                  0x400A0120         /* USB0 EPn Transmit Maximum Packet Length Register */
#define REG_USB0_EP3_TXMAXP                  0x400A0130         /* USB0 EPn Transmit Maximum Packet Length Register */
#define REG_USB0_EP0_TXCSR_P                 0x400A0102         /* USB0 EPn Transmit Configuration and Status (Peripheral) Register */
#define REG_USB0_EP1_TXCSR_P                 0x400A0112         /* USB0 EPn Transmit Configuration and Status (Peripheral) Register */
#define REG_USB0_EP2_TXCSR_P                 0x400A0122         /* USB0 EPn Transmit Configuration and Status (Peripheral) Register */
#define REG_USB0_EP3_TXCSR_P                 0x400A0132         /* USB0 EPn Transmit Configuration and Status (Peripheral) Register */
#define REG_USB0_EP0_CSR0_P                  0x400A0102         /* USB0 EP0 Configuration and Status (Peripheral) Register */
#define REG_USB0_EP0_CSR0_H                  0x400A0102         /* USB0 EP0 Configuration and Status (Host) Register */
#define REG_USB0_EP0_TXCSR_H                 0x400A0102         /* USB0 EPn Transmit Configuration and Status (Host) Register */
#define REG_USB0_EP1_TXCSR_H                 0x400A0112         /* USB0 EPn Transmit Configuration and Status (Host) Register */
#define REG_USB0_EP2_TXCSR_H                 0x400A0122         /* USB0 EPn Transmit Configuration and Status (Host) Register */
#define REG_USB0_EP3_TXCSR_H                 0x400A0132         /* USB0 EPn Transmit Configuration and Status (Host) Register */
#define REG_USB0_EP0_RXMAXP                  0x400A0104         /* USB0 EPn Receive Maximum Packet Length Register */
#define REG_USB0_EP1_RXMAXP                  0x400A0114         /* USB0 EPn Receive Maximum Packet Length Register */
#define REG_USB0_EP2_RXMAXP                  0x400A0124         /* USB0 EPn Receive Maximum Packet Length Register */
#define REG_USB0_EP3_RXMAXP                  0x400A0134         /* USB0 EPn Receive Maximum Packet Length Register */
#define REG_USB0_EP0_RXCSR_P                 0x400A0106         /* USB0 EPn Receive Configuration and Status (Peripheral) Register */
#define REG_USB0_EP1_RXCSR_P                 0x400A0116         /* USB0 EPn Receive Configuration and Status (Peripheral) Register */
#define REG_USB0_EP2_RXCSR_P                 0x400A0126         /* USB0 EPn Receive Configuration and Status (Peripheral) Register */
#define REG_USB0_EP3_RXCSR_P                 0x400A0136         /* USB0 EPn Receive Configuration and Status (Peripheral) Register */
#define REG_USB0_EP0_RXCSR_H                 0x400A0106         /* USB0 EPn Receive Configuration and Status (Host) Register */
#define REG_USB0_EP1_RXCSR_H                 0x400A0116         /* USB0 EPn Receive Configuration and Status (Host) Register */
#define REG_USB0_EP2_RXCSR_H                 0x400A0126         /* USB0 EPn Receive Configuration and Status (Host) Register */
#define REG_USB0_EP3_RXCSR_H                 0x400A0136         /* USB0 EPn Receive Configuration and Status (Host) Register */
#define REG_USB0_EP0_CNT0                    0x400A0108         /* USB0 EP0 Number of Received Bytes Register */
#define REG_USB0_EP0_RXCNT                   0x400A0108         /* USB0 EPn Number of Bytes Received Register */
#define REG_USB0_EP1_RXCNT                   0x400A0118         /* USB0 EPn Number of Bytes Received Register */
#define REG_USB0_EP2_RXCNT                   0x400A0128         /* USB0 EPn Number of Bytes Received Register */
#define REG_USB0_EP3_RXCNT                   0x400A0138         /* USB0 EPn Number of Bytes Received Register */
#define REG_USB0_EP0_FIFOSIZE                0x400A010F         /* USB0 FIFO size */
#define REG_USB0_EP1_FIFOSIZE                0x400A011F         /* USB0 FIFO size */
#define REG_USB0_EP2_FIFOSIZE                0x400A012F         /* USB0 FIFO size */
#define REG_USB0_EP3_FIFOSIZE                0x400A013F         /* USB0 FIFO size */
#define REG_USB0_EP0_CFGDATA0                0x400A010F         /* USB0 EP0 Configuration Information Register */
#define REG_USB0_DMA_IRQ                     0x400A0200         /* USB0 DMA Interrupt Register */
#define REG_USB0_DMA0_CTL                    0x400A0204         /* USB0 DMA Channel n Control Register */
#define REG_USB0_DMA1_CTL                    0x400A0214         /* USB0 DMA Channel n Control Register */
#define REG_USB0_DMA0_ADDR                   0x400A0208         /* USB0 DMA Channel n Address Register */
#define REG_USB0_DMA1_ADDR                   0x400A0218         /* USB0 DMA Channel n Address Register */
#define REG_USB0_DMA0_CNT                    0x400A020C         /* USB0 DMA Channel n Count Register */
#define REG_USB0_DMA1_CNT                    0x400A021C         /* USB0 DMA Channel n Count Register */
#define REG_USB0_RXDPKTBUFDIS                0x400A0340         /* USB0 RX Double Packet Buffer Disable for Endpoints 1 to 3 */
#define REG_USB0_TXDPKTBUFDIS                0x400A0342         /* USB0 TX Double Packet Buffer Disable  for Endpoints 1 to 3 */
#define REG_USB0_CT_UCH                      0x400A0344         /* USB0 Chirp Timeout Register */
#define REG_USB0_LPM_ATTR                    0x400A0360         /* USB0 LPM Attribute Register */
#define REG_USB0_LPM_CTL                     0x400A0362         /* USB0 LPM Control Register */
#define REG_USB0_LPM_IEN                     0x400A0363         /* USB0 LPM Interrupt Enable Register */
#define REG_USB0_LPM_IRQ                     0x400A0364         /* USB0 LPM Interrupt Status Register */
#define REG_USB0_PHY_CTL                     0x400A039C         /* USB0 FS PHY Control */
#define REG_USB0_PHY_STAT                    0x400A039E         /* USB0 FS PHY Status */
#define REG_USB0_RAM_ADDR                    0x400A03B0         /* USB0 RAM Address Register */
#define REG_USB0_RAM_DATA                    0x400A03B4         /* USB0 RAM Data Register */

/* =========================
        USB
   ========================= */
/* ------------------------------------------------------------------------------------------------------------------------
        USB_FADDR                                                                Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_FADDR_VALUE                                                      0                               /* Function Address Value */
#define BITM_USB_FADDR_VALUE                                                     (_ADI_MSK(0x0000007F,uint8_t))   /* Function Address Value */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_POWER                                                                Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_POWER_ISOUPDT                                                    7                               /* ISO Update Enable */
#define BITP_USB_POWER_SOFTCONN                                                   6                               /* Soft Connect/Disconnect Enable */
#define BITP_USB_POWER_RESET                                                      3                               /* Reset USB */
#define BITP_USB_POWER_RESUME                                                     2                               /* Resume Mode */
#define BITP_USB_POWER_SUSPEND                                                    1                               /* Suspend Mode */
#define BITP_USB_POWER_SUSEN                                                      0                               /* SUSPENDM Output Enable */

#define BITM_USB_POWER_ISOUPDT                                                   (_ADI_MSK(0x00000080,uint8_t))   /* ISO Update Enable */
#define ENUM_USB_POWER_NO_ISOUPDT                                                (_ADI_MSK(0x00000000,uint8_t))   /* ISOUPDT: Disable ISO Update */
#define ENUM_USB_POWER_ISOUPDT                                                   (_ADI_MSK(0x00000080,uint8_t))   /* ISOUPDT: Enable ISO Update */

#define BITM_USB_POWER_SOFTCONN                                                  (_ADI_MSK(0x00000040,uint8_t))   /* Soft Connect/Disconnect Enable */
#define ENUM_USB_POWER_NO_SOFTCONN                                               (_ADI_MSK(0x00000000,uint8_t))   /* SOFTCONN: Disable Soft Connect/Disconnect */
#define ENUM_USB_POWER_SOFTCONN                                                  (_ADI_MSK(0x00000040,uint8_t))   /* SOFTCONN: Enable Soft Connect/Disconnect */

#define BITM_USB_POWER_RESET                                                     (_ADI_MSK(0x00000008,uint8_t))   /* Reset USB */
#define ENUM_USB_POWER_NO_RESET                                                  (_ADI_MSK(0x00000000,uint8_t))   /* RESET: No Reset */
#define ENUM_USB_POWER_RESET                                                     (_ADI_MSK(0x00000008,uint8_t))   /* RESET: Reset USB */

#define BITM_USB_POWER_RESUME                                                    (_ADI_MSK(0x00000004,uint8_t))   /* Resume Mode */
#define ENUM_USB_POWER_NO_RESUME                                                 (_ADI_MSK(0x00000000,uint8_t))   /* RESUME: Disable Resume Signaling */
#define ENUM_USB_POWER_RESUME                                                    (_ADI_MSK(0x00000004,uint8_t))   /* RESUME: Enable Resume Signaling */

#define BITM_USB_POWER_SUSPEND                                                   (_ADI_MSK(0x00000002,uint8_t))   /* Suspend Mode */
#define ENUM_USB_POWER_NO_SUSPEND                                                (_ADI_MSK(0x00000000,uint8_t))   /* SUSPEND: Disable Suspend Mode (Host) */
#define ENUM_USB_POWER_SUSPEND                                                   (_ADI_MSK(0x00000002,uint8_t))   /* SUSPEND: Enable Suspend Mode (Host) */

#define BITM_USB_POWER_SUSEN                                                     (_ADI_MSK(0x00000001,uint8_t))   /* SUSPENDM Output Enable */
#define ENUM_USB_POWER_SUSDIS                                                    (_ADI_MSK(0x00000000,uint8_t))   /* SUSEN: Disable SUSPENDM Output */
#define ENUM_USB_POWER_SUSEN                                                     (_ADI_MSK(0x00000001,uint8_t))   /* SUSEN: Enable SUSPENDM Output */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_INTRTX                                                               Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_INTRTX_EP3                                                       3                               /* End Point 3 Tx Interrupt */
#define BITP_USB_INTRTX_EP2                                                       2                               /* End Point 2 Tx Interrupt */
#define BITP_USB_INTRTX_EP1                                                       1                               /* End Point 1 Tx Interrupt */
#define BITP_USB_INTRTX_EP0                                                       0                               /* End Point 0 Tx Interrupt */
#define BITM_USB_INTRTX_EP3                                                      (_ADI_MSK(0x00000008,uint16_t))  /* End Point 3 Tx Interrupt */
#define BITM_USB_INTRTX_EP2                                                      (_ADI_MSK(0x00000004,uint16_t))  /* End Point 2 Tx Interrupt */
#define BITM_USB_INTRTX_EP1                                                      (_ADI_MSK(0x00000002,uint16_t))  /* End Point 1 Tx Interrupt */
#define BITM_USB_INTRTX_EP0                                                      (_ADI_MSK(0x00000001,uint16_t))  /* End Point 0 Tx Interrupt */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_INTRRX                                                               Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_INTRRX_EP3                                                       3                               /* End Point 3 Rx Interrupt. */
#define BITP_USB_INTRRX_EP2                                                       2                               /* End Point 2 Rx Interrupt. */
#define BITP_USB_INTRRX_EP1                                                       1                               /* End Point 1 Rx Interrupt. */
#define BITM_USB_INTRRX_EP3                                                      (_ADI_MSK(0x00000008,uint16_t))  /* End Point 3 Rx Interrupt. */
#define BITM_USB_INTRRX_EP2                                                      (_ADI_MSK(0x00000004,uint16_t))  /* End Point 2 Rx Interrupt. */
#define BITM_USB_INTRRX_EP1                                                      (_ADI_MSK(0x00000002,uint16_t))  /* End Point 1 Rx Interrupt. */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_INTRTXE                                                              Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_INTRTXE_EP3                                                      3                               /* End Point 3 Tx Interrupt Enable */
#define BITP_USB_INTRTXE_EP2                                                      2                               /* End Point 2 Tx Interrupt Enable */
#define BITP_USB_INTRTXE_EP1                                                      1                               /* End Point 1 Tx Interrupt Enable */
#define BITP_USB_INTRTXE_EP0                                                      0                               /* End Point 0 Tx Interrupt Enable */
#define BITM_USB_INTRTXE_EP3                                                     (_ADI_MSK(0x00000008,uint16_t))  /* End Point 3 Tx Interrupt Enable */
#define BITM_USB_INTRTXE_EP2                                                     (_ADI_MSK(0x00000004,uint16_t))  /* End Point 2 Tx Interrupt Enable */
#define BITM_USB_INTRTXE_EP1                                                     (_ADI_MSK(0x00000002,uint16_t))  /* End Point 1 Tx Interrupt Enable */
#define BITM_USB_INTRTXE_EP0                                                     (_ADI_MSK(0x00000001,uint16_t))  /* End Point 0 Tx Interrupt Enable */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_INTRRXE                                                              Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_INTRRXE_EP3                                                      3                               /* End Point 3 Rx Interrupt Enable */
#define BITP_USB_INTRRXE_EP2                                                      2                               /* End Point 2 Rx Interrupt Enable */
#define BITP_USB_INTRRXE_EP1                                                      1                               /* End Point 1 Rx Interrupt Enable */
#define BITM_USB_INTRRXE_EP3                                                     (_ADI_MSK(0x00000008,uint16_t))  /* End Point 3 Rx Interrupt Enable */
#define BITM_USB_INTRRXE_EP2                                                     (_ADI_MSK(0x00000004,uint16_t))  /* End Point 2 Rx Interrupt Enable */
#define BITM_USB_INTRRXE_EP1                                                     (_ADI_MSK(0x00000002,uint16_t))  /* End Point 1 Rx Interrupt Enable */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_IRQ                                                                  Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_IRQ_DISCON                                                       5                               /* Disconnect Indicator */
#define BITP_USB_IRQ_SOF                                                          3                               /* Start-of-frame Indicator */
#define BITP_USB_IRQ_RSTBABBLE                                                    2                               /* Reset/Babble Indicator */
#define BITP_USB_IRQ_RESUME                                                       1                               /* Resume Indicator */
#define BITP_USB_IRQ_SUSPEND                                                      0                               /* Suspend Indicator */

#define BITM_USB_IRQ_DISCON                                                      (_ADI_MSK(0x00000020,uint8_t))   /* Disconnect Indicator */
#define ENUM_USB_IRQ_NO_DISCON                                                   (_ADI_MSK(0x00000000,uint8_t))   /* DISCON: No Interrupt */
#define ENUM_USB_IRQ_DISCON                                                      (_ADI_MSK(0x00000020,uint8_t))   /* DISCON: Interrupt Pending */

#define BITM_USB_IRQ_SOF                                                         (_ADI_MSK(0x00000008,uint8_t))   /* Start-of-frame Indicator */
#define ENUM_USB_IRQ_NO_SOF                                                      (_ADI_MSK(0x00000000,uint8_t))   /* SOF: No Interrupt */
#define ENUM_USB_IRQ_SOF                                                         (_ADI_MSK(0x00000008,uint8_t))   /* SOF: Interrupt Pending */

#define BITM_USB_IRQ_RSTBABBLE                                                   (_ADI_MSK(0x00000004,uint8_t))   /* Reset/Babble Indicator */
#define ENUM_USB_IRQ_NO_RSTBABBLE                                                (_ADI_MSK(0x00000000,uint8_t))   /* RSTBABBLE: No Interrupt */
#define ENUM_USB_IRQ_RSTBABBLE                                                   (_ADI_MSK(0x00000004,uint8_t))   /* RSTBABBLE: Interrupt Pending */

#define BITM_USB_IRQ_RESUME                                                      (_ADI_MSK(0x00000002,uint8_t))   /* Resume Indicator */
#define ENUM_USB_IRQ_NO_RESUME                                                   (_ADI_MSK(0x00000000,uint8_t))   /* RESUME: No Interrupt */
#define ENUM_USB_IRQ_RESUME                                                      (_ADI_MSK(0x00000002,uint8_t))   /* RESUME: Interrupt Pending */

#define BITM_USB_IRQ_SUSPEND                                                     (_ADI_MSK(0x00000001,uint8_t))   /* Suspend Indicator */
#define ENUM_USB_IRQ_NO_SUSPEND                                                  (_ADI_MSK(0x00000000,uint8_t))   /* SUSPEND: No Interrupt */
#define ENUM_USB_IRQ_SUSPEND                                                     (_ADI_MSK(0x00000001,uint8_t))   /* SUSPEND: Interrupt Pending */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_IEN                                                                  Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_IEN_DISCON                                                       5                               /* Disconnect Indicator Interrupt Enable */
#define BITP_USB_IEN_SOF                                                          3                               /* Start-of-frame Indicator Interrupt Enable */
#define BITP_USB_IEN_RSTBABBLE                                                    2                               /* Reset/Babble Indicator Interrupt Enable */
#define BITP_USB_IEN_RESUME                                                       1                               /* Resume Indicator Interrupt Enable */
#define BITP_USB_IEN_SUSPEND                                                      0                               /* Suspend Indicator Interrupt Enable */

#define BITM_USB_IEN_DISCON                                                      (_ADI_MSK(0x00000020,uint8_t))   /* Disconnect Indicator Interrupt Enable */
#define ENUM_USB_IEN_DISCONDIS                                                   (_ADI_MSK(0x00000000,uint8_t))   /* DISCON: Disable Interrupt */
#define ENUM_USB_IEN_DISCONEN                                                    (_ADI_MSK(0x00000020,uint8_t))   /* DISCON: Enable Interrupt */

#define BITM_USB_IEN_SOF                                                         (_ADI_MSK(0x00000008,uint8_t))   /* Start-of-frame Indicator Interrupt Enable */
#define ENUM_USB_IEN_SOFDIS                                                      (_ADI_MSK(0x00000000,uint8_t))   /* SOF: Disable Interrupt */
#define ENUM_USB_IEN_SOFEN                                                       (_ADI_MSK(0x00000008,uint8_t))   /* SOF: Enable Interrupt */

#define BITM_USB_IEN_RSTBABBLE                                                   (_ADI_MSK(0x00000004,uint8_t))   /* Reset/Babble Indicator Interrupt Enable */
#define ENUM_USB_IEN_RSTBABBLEDIS                                                (_ADI_MSK(0x00000000,uint8_t))   /* RSTBABBLE: Disable Interrupt */
#define ENUM_USB_IEN_RSTBABBLEEN                                                 (_ADI_MSK(0x00000004,uint8_t))   /* RSTBABBLE: Enable Interrupt */

#define BITM_USB_IEN_RESUME                                                      (_ADI_MSK(0x00000002,uint8_t))   /* Resume Indicator Interrupt Enable */
#define ENUM_USB_IEN_RESUMEDIS                                                   (_ADI_MSK(0x00000000,uint8_t))   /* RESUME: Disable Interrupt */
#define ENUM_USB_IEN_RESUMEEN                                                    (_ADI_MSK(0x00000002,uint8_t))   /* RESUME: Enable Interrupt */

#define BITM_USB_IEN_SUSPEND                                                     (_ADI_MSK(0x00000001,uint8_t))   /* Suspend Indicator Interrupt Enable */
#define ENUM_USB_IEN_SUSPENDDIS                                                  (_ADI_MSK(0x00000000,uint8_t))   /* SUSPEND: Disable Interrupt */
#define ENUM_USB_IEN_SUSPENDEN                                                   (_ADI_MSK(0x00000001,uint8_t))   /* SUSPEND: Enable Interrupt */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_FRAME                                                                Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_FRAME_VALUE                                                      0                               /* Frame Number Value */
#define BITM_USB_FRAME_VALUE                                                     (_ADI_MSK(0x000007FF,uint16_t))  /* Frame Number Value */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_INDEX                                                                Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_INDEX_EP                                                         0                               /* Endpoint Index */
#define BITM_USB_INDEX_EP                                                        (_ADI_MSK(0x0000000F,uint8_t))   /* Endpoint Index */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_TESTMODE                                                             Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_TESTMODE_FIFOACCESS                                              6                               /* FIFO Access */
#define BITP_USB_TESTMODE_FORCEFS                                                 5                               /* Force Full Speed Mode */
#define BITM_USB_TESTMODE_FIFOACCESS                                             (_ADI_MSK(0x00000040,uint8_t))   /* FIFO Access */
#define BITM_USB_TESTMODE_FORCEFS                                                (_ADI_MSK(0x00000020,uint8_t))   /* Force Full Speed Mode */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_EPI_TXMAXP                                                           Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_EPI_TXMAXP_MAXPAY                                                0                               /* Maximum Payload */
#define BITM_USB_EPI_TXMAXP_MAXPAY                                               (_ADI_MSK(0x000007FF,uint16_t))  /* Maximum Payload */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_EPI_TXCSR_H                                                          Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_EPI_TXCSR_H_AUTOSET                                             15                               /* TxPkRdy Autoset Enable */
#define BITP_USB_EPI_TXCSR_H_DMAREQEN                                            12                               /* DMA Request Enable Tx EP */
#define BITP_USB_EPI_TXCSR_H_FRCDATATGL                                          11                               /* Force Data Toggle */
#define BITP_USB_EPI_TXCSR_H_DMAREQMODE                                          10                               /* DMA Mode Select */
#define BITP_USB_EPI_TXCSR_H_DATGLEN                                              9                               /* Data Toggle Write Enable */
#define BITP_USB_EPI_TXCSR_H_DATGL                                                8                               /* Data Toggle */
#define BITP_USB_EPI_TXCSR_H_CLRDATATGL                                           6                               /* Clear Endpoint Data Toggle */
#define BITP_USB_EPI_TXCSR_H_RXSTALL                                              5                               /* Rx STALL */
#define BITP_USB_EPI_TXCSR_H_SETUPPKT                                             4                               /* Setup Packet */
#define BITP_USB_EPI_TXCSR_H_FLUSHFIFO                                            3                               /* Flush Endpoint FIFO */
#define BITP_USB_EPI_TXCSR_H_TXTOERR                                              2                               /* Tx Timeout Error */
#define BITP_USB_EPI_TXCSR_H_NEFIFO                                               1                               /* Not Empty FIFO */
#define BITP_USB_EPI_TXCSR_H_TXPKTRDY                                             0                               /* Tx Packet Ready */

#define BITM_USB_EPI_TXCSR_H_AUTOSET                                             (_ADI_MSK(0x00008000,uint16_t))  /* TxPkRdy Autoset Enable */
#define ENUM_USB_EPI_TXCSR_H_NO_AUTOSET                                          (_ADI_MSK(0x00000000,uint16_t))  /* AUTOSET: Disable Autoset */
#define ENUM_USB_EPI_TXCSR_H_AUTOSET                                             (_ADI_MSK(0x00008000,uint16_t))  /* AUTOSET: Enable Autoset */

#define BITM_USB_EPI_TXCSR_H_DMAREQEN                                            (_ADI_MSK(0x00001000,uint16_t))  /* DMA Request Enable Tx EP */
#define ENUM_USB_EPI_TXCSR_H_DMAREQDIS                                           (_ADI_MSK(0x00000000,uint16_t))  /* DMAREQEN: Disable DMA Request */
#define ENUM_USB_EPI_TXCSR_H_DMAREQEN                                            (_ADI_MSK(0x00001000,uint16_t))  /* DMAREQEN: Enable DMA Request */

#define BITM_USB_EPI_TXCSR_H_FRCDATATGL                                          (_ADI_MSK(0x00000800,uint16_t))  /* Force Data Toggle */
#define ENUM_USB_EPI_TXCSR_H_NO_FRCTGL                                           (_ADI_MSK(0x00000000,uint16_t))  /* FRCDATATGL: No Action */
#define ENUM_USB_EPI_TXCSR_H_FRCTGL                                              (_ADI_MSK(0x00000800,uint16_t))  /* FRCDATATGL: Toggle Endpoint Data */

#define BITM_USB_EPI_TXCSR_H_DMAREQMODE                                          (_ADI_MSK(0x00000400,uint16_t))  /* DMA Mode Select */
#define ENUM_USB_EPI_TXCSR_H_DMARQMODE0                                          (_ADI_MSK(0x00000000,uint16_t))  /* DMAREQMODE: DMA Request Mode 0 */
#define ENUM_USB_EPI_TXCSR_H_DMARQMODE1                                          (_ADI_MSK(0x00000400,uint16_t))  /* DMAREQMODE: DMA Request Mode 1 */

#define BITM_USB_EPI_TXCSR_H_DATGLEN                                             (_ADI_MSK(0x00000200,uint16_t))  /* Data Toggle Write Enable */
#define ENUM_USB_EPI_TXCSR_H_NO_DATGLEN                                          (_ADI_MSK(0x00000000,uint16_t))  /* DATGLEN: Disable Write to DATGL */
#define ENUM_USB_EPI_TXCSR_H_DATGLEN                                             (_ADI_MSK(0x00000200,uint16_t))  /* DATGLEN: Enable Write to DATGL */

#define BITM_USB_EPI_TXCSR_H_DATGL                                               (_ADI_MSK(0x00000100,uint16_t))  /* Data Toggle */
#define ENUM_USB_EPI_TXCSR_H_NO_DATGL                                            (_ADI_MSK(0x00000000,uint16_t))  /* DATGL: DATA0 is set */
#define ENUM_USB_EPI_TXCSR_H_DATGL                                               (_ADI_MSK(0x00000100,uint16_t))  /* DATGL: DATA1 is set */

#define BITM_USB_EPI_TXCSR_H_CLRDATATGL                                          (_ADI_MSK(0x00000040,uint16_t))  /* Clear Endpoint Data Toggle */
#define ENUM_USB_EPI_TXCSR_H_NO_CLRTGL                                           (_ADI_MSK(0x00000000,uint16_t))  /* CLRDATATGL: No Action */
#define ENUM_USB_EPI_TXCSR_H_CLRTGL                                              (_ADI_MSK(0x00000040,uint16_t))  /* CLRDATATGL: Reset EP Data Toggle to 0 */

#define BITM_USB_EPI_TXCSR_H_RXSTALL                                             (_ADI_MSK(0x00000020,uint16_t))  /* Rx STALL */
#define ENUM_USB_EPI_TXCSR_H_NO_RXSTALL                                          (_ADI_MSK(0x00000000,uint16_t))  /* RXSTALL: No Status */
#define ENUM_USB_EPI_TXCSR_H_RXSTALL                                             (_ADI_MSK(0x00000020,uint16_t))  /* RXSTALL: Stall Received from Device */

#define BITM_USB_EPI_TXCSR_H_SETUPPKT                                            (_ADI_MSK(0x00000010,uint16_t))  /* Setup Packet */
#define ENUM_USB_EPI_TXCSR_H_NO_SETUPPK                                          (_ADI_MSK(0x00000000,uint16_t))  /* SETUPPKT: No Request */
#define ENUM_USB_EPI_TXCSR_H_SETUPPKT                                            (_ADI_MSK(0x00000010,uint16_t))  /* SETUPPKT: Send SETUP Token */

#define BITM_USB_EPI_TXCSR_H_FLUSHFIFO                                           (_ADI_MSK(0x00000008,uint16_t))  /* Flush Endpoint FIFO */
#define ENUM_USB_EPI_TXCSR_H_NO_FLUSH                                            (_ADI_MSK(0x00000000,uint16_t))  /* FLUSHFIFO: No Flush */
#define ENUM_USB_EPI_TXCSR_H_FLUSH                                               (_ADI_MSK(0x00000008,uint16_t))  /* FLUSHFIFO: Flush endpoint FIFO */

#define BITM_USB_EPI_TXCSR_H_TXTOERR                                             (_ADI_MSK(0x00000004,uint16_t))  /* Tx Timeout Error */
#define ENUM_USB_EPI_TXCSR_H_NO_TXTOERR                                          (_ADI_MSK(0x00000000,uint16_t))  /* TXTOERR: No Status */
#define ENUM_USB_EPI_TXCSR_H_TXTOERR                                             (_ADI_MSK(0x00000004,uint16_t))  /* TXTOERR: Tx Timeout Error */

#define BITM_USB_EPI_TXCSR_H_NEFIFO                                              (_ADI_MSK(0x00000002,uint16_t))  /* Not Empty FIFO */
#define ENUM_USB_EPI_TXCSR_H_NO_NEFIFO                                           (_ADI_MSK(0x00000000,uint16_t))  /* NEFIFO: FIFO Empty */
#define ENUM_USB_EPI_TXCSR_H_NEFIFO                                              (_ADI_MSK(0x00000002,uint16_t))  /* NEFIFO: FIFO Not Empty */

#define BITM_USB_EPI_TXCSR_H_TXPKTRDY                                            (_ADI_MSK(0x00000001,uint16_t))  /* Tx Packet Ready */
#define ENUM_USB_EPI_TXCSR_H_NO_PKTRDY                                           (_ADI_MSK(0x00000000,uint16_t))  /* TXPKTRDY: No Tx Packet */
#define ENUM_USB_EPI_TXCSR_H_PKTRDY                                              (_ADI_MSK(0x00000001,uint16_t))  /* TXPKTRDY: Tx Packet in Endpoint FIFO */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_EP0I_CSR_P                                                           Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_EP0I_CSR_P_FLUSHFIFO                                             8                               /* Flush Endpoint FIFO */
#define BITP_USB_EP0I_CSR_P_SSETUPEND                                             7                               /* Service Setup End */
#define BITP_USB_EP0I_CSR_P_SPKTRDY                                               6                               /* Service Rx Packet Ready */
#define BITP_USB_EP0I_CSR_P_SENDSTALL                                             5                               /* Send Stall */
#define BITP_USB_EP0I_CSR_P_SETUPEND                                              4                               /* Setup End */
#define BITP_USB_EP0I_CSR_P_DATAEND                                               3                               /* Data End */
#define BITP_USB_EP0I_CSR_P_SENTSTALL                                             2                               /* Sent Stall */
#define BITP_USB_EP0I_CSR_P_TXPKTRDY                                              1                               /* Tx Packet Ready */
#define BITP_USB_EP0I_CSR_P_RXPKTRDY                                              0                               /* Rx Packet Ready */

#define BITM_USB_EP0I_CSR_P_FLUSHFIFO                                            (_ADI_MSK(0x00000100,uint16_t))  /* Flush Endpoint FIFO */
#define ENUM_USB_EP0I_CSR_P_NO_FLUSH                                             (_ADI_MSK(0x00000000,uint16_t))  /* FLUSHFIFO: No Flush */
#define ENUM_USB_EP0I_CSR_P_FLUSH                                                (_ADI_MSK(0x00000100,uint16_t))  /* FLUSHFIFO: Flush Endpoint FIFO */

#define BITM_USB_EP0I_CSR_P_SSETUPEND                                            (_ADI_MSK(0x00000080,uint16_t))  /* Service Setup End */
#define ENUM_USB_EP0I_CSR_P_NOSSETUPEND                                          (_ADI_MSK(0x00000000,uint16_t))  /* SSETUPEND: No Action */
#define ENUM_USB_EP0I_CSR_P_SSETUPEND                                            (_ADI_MSK(0x00000080,uint16_t))  /* SSETUPEND: Clear SETUPEND Bit */

#define BITM_USB_EP0I_CSR_P_SPKTRDY                                              (_ADI_MSK(0x00000040,uint16_t))  /* Service Rx Packet Ready */
#define ENUM_USB_EP0I_CSR_P_NO_SPKTRDY                                           (_ADI_MSK(0x00000000,uint16_t))  /* SPKTRDY: No Action */
#define ENUM_USB_EP0I_CSR_P_SPKTRDY                                              (_ADI_MSK(0x00000040,uint16_t))  /* SPKTRDY: Clear RXPKTRDY Bit */

#define BITM_USB_EP0I_CSR_P_SENDSTALL                                            (_ADI_MSK(0x00000020,uint16_t))  /* Send Stall */
#define ENUM_USB_EP0I_CSR_P_NO_STALL                                             (_ADI_MSK(0x00000000,uint16_t))  /* SENDSTALL: No Action */
#define ENUM_USB_EP0I_CSR_P_STALL                                                (_ADI_MSK(0x00000020,uint16_t))  /* SENDSTALL: Terminate Current Transaction */

#define BITM_USB_EP0I_CSR_P_SETUPEND                                             (_ADI_MSK(0x00000010,uint16_t))  /* Setup End */
#define ENUM_USB_EP0I_CSR_P_NO_SETUPEND                                          (_ADI_MSK(0x00000000,uint16_t))  /* SETUPEND: No Status */
#define ENUM_USB_EP0I_CSR_P_SETUPEND                                             (_ADI_MSK(0x00000010,uint16_t))  /* SETUPEND: Setup Ended before DATAEND */

#define BITM_USB_EP0I_CSR_P_DATAEND                                              (_ADI_MSK(0x00000008,uint16_t))  /* Data End */
#define ENUM_USB_EP0I_CSR_P_NO_DATAEND                                           (_ADI_MSK(0x00000000,uint16_t))  /* DATAEND: No Status */
#define ENUM_USB_EP0I_CSR_P_DATAEND                                              (_ADI_MSK(0x00000008,uint16_t))  /* DATAEND: Data End Condition */

#define BITM_USB_EP0I_CSR_P_SENTSTALL                                            (_ADI_MSK(0x00000004,uint16_t))  /* Sent Stall */
#define ENUM_USB_EP0I_CSR_P_NO_STALSNT                                           (_ADI_MSK(0x00000000,uint16_t))  /* SENTSTALL: No Status */
#define ENUM_USB_EP0I_CSR_P_STALSNT                                              (_ADI_MSK(0x00000004,uint16_t))  /* SENTSTALL: Transmitted STALL Handshake */

#define BITM_USB_EP0I_CSR_P_TXPKTRDY                                             (_ADI_MSK(0x00000002,uint16_t))  /* Tx Packet Ready */
#define ENUM_USB_EP0I_CSR_P_NO_TXPKTRDY                                          (_ADI_MSK(0x00000000,uint16_t))  /* TXPKTRDY:  */
#define ENUM_USB_EP0I_CSR_P_TXPKTRDY                                             (_ADI_MSK(0x00000002,uint16_t))  /* TXPKTRDY: Set this bit after loading a data packet into the FIFO */

#define BITM_USB_EP0I_CSR_P_RXPKTRDY                                             (_ADI_MSK(0x00000001,uint16_t))  /* Rx Packet Ready */
#define ENUM_USB_EP0I_CSR_P_NO_PKTRDY                                            (_ADI_MSK(0x00000000,uint16_t))  /* RXPKTRDY: No Rx Packet */
#define ENUM_USB_EP0I_CSR_P_PKTRDY                                               (_ADI_MSK(0x00000001,uint16_t))  /* RXPKTRDY: Rx Packet in Endpoint FIFO */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_EP0I_CSR_H                                                           Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_EP0I_CSR_H_DISPING                                              11                               /* Disable Ping */
#define BITP_USB_EP0I_CSR_H_DATGLEN                                              10                               /* Data Toggle Write Enable */
#define BITP_USB_EP0I_CSR_H_DATGL                                                 9                               /* Data Toggle */
#define BITP_USB_EP0I_CSR_H_FLUSHFIFO                                             8                               /* Flush Endpoint FIFO */
#define BITP_USB_EP0I_CSR_H_NAKTO                                                 7                               /* NAK Timeout */
#define BITP_USB_EP0I_CSR_H_STATUSPKT                                             6                               /* Status Packet */
#define BITP_USB_EP0I_CSR_H_REQPKT                                                5                               /* Request Packet */
#define BITP_USB_EP0I_CSR_H_TOERR                                                 4                               /* Timeout Error */
#define BITP_USB_EP0I_CSR_H_SETUPPKT                                              3                               /* Setup Packet */
#define BITP_USB_EP0I_CSR_H_RXSTALL                                               2                               /* Rx Stall */
#define BITP_USB_EP0I_CSR_H_TXPKTRDY                                              1                               /* Tx Packet Ready */
#define BITP_USB_EP0I_CSR_H_RXPKTRDY                                              0                               /* Rx Packet Ready */

#define BITM_USB_EP0I_CSR_H_DISPING                                              (_ADI_MSK(0x00000800,uint16_t))  /* Disable Ping */
#define ENUM_USB_EP0I_CSR_H_NO_DISPING                                           (_ADI_MSK(0x00000000,uint16_t))  /* DISPING: Issue PING tokens */
#define ENUM_USB_EP0I_CSR_H_DISPING                                              (_ADI_MSK(0x00000800,uint16_t))  /* DISPING: Do not issue PING */

#define BITM_USB_EP0I_CSR_H_DATGLEN                                              (_ADI_MSK(0x00000400,uint16_t))  /* Data Toggle Write Enable */
#define ENUM_USB_EP0I_CSR_H_NO_DATGLEN                                           (_ADI_MSK(0x00000000,uint16_t))  /* DATGLEN: Disable Write to DATGL */
#define ENUM_USB_EP0I_CSR_H_DATGLEN                                              (_ADI_MSK(0x00000400,uint16_t))  /* DATGLEN: Enable Write to DATGL */

#define BITM_USB_EP0I_CSR_H_DATGL                                                (_ADI_MSK(0x00000200,uint16_t))  /* Data Toggle */
#define ENUM_USB_EP0I_CSR_H_NO_DATATGL                                           (_ADI_MSK(0x00000000,uint16_t))  /* DATGL: DATA0 is Set */
#define ENUM_USB_EP0I_CSR_H_DATATGL                                              (_ADI_MSK(0x00000200,uint16_t))  /* DATGL: DATA1 is Set */

#define BITM_USB_EP0I_CSR_H_FLUSHFIFO                                            (_ADI_MSK(0x00000100,uint16_t))  /* Flush Endpoint FIFO */
#define ENUM_USB_EP0I_CSR_H_NO_FLUSH                                             (_ADI_MSK(0x00000000,uint16_t))  /* FLUSHFIFO: No Flush */
#define ENUM_USB_EP0I_CSR_H_FLUSH                                                (_ADI_MSK(0x00000100,uint16_t))  /* FLUSHFIFO: Flush Endpoint FIFO */

#define BITM_USB_EP0I_CSR_H_NAKTO                                                (_ADI_MSK(0x00000080,uint16_t))  /* NAK Timeout */
#define ENUM_USB_EP0I_CSR_H_NO_NAKTO                                             (_ADI_MSK(0x00000000,uint16_t))  /* NAKTO: No Status */
#define ENUM_USB_EP0I_CSR_H_NAKTO                                                (_ADI_MSK(0x00000080,uint16_t))  /* NAKTO: Endpoint Halted (NAK Timeout) */

#define BITM_USB_EP0I_CSR_H_STATUSPKT                                            (_ADI_MSK(0x00000040,uint16_t))  /* Status Packet */
#define ENUM_USB_EP0I_CSR_H_NO_STATPKT                                           (_ADI_MSK(0x00000000,uint16_t))  /* STATUSPKT: No Request */
#define ENUM_USB_EP0I_CSR_H_STATPKT                                              (_ADI_MSK(0x00000040,uint16_t))  /* STATUSPKT: Request Status Transaction */

#define BITM_USB_EP0I_CSR_H_REQPKT                                               (_ADI_MSK(0x00000020,uint16_t))  /* Request Packet */
#define ENUM_USB_EP0I_CSR_H_NO_REQPKT                                            (_ADI_MSK(0x00000000,uint16_t))  /* REQPKT: No Request */
#define ENUM_USB_EP0I_CSR_H_REQPKT                                               (_ADI_MSK(0x00000020,uint16_t))  /* REQPKT: Send IN Tokens to Device */

#define BITM_USB_EP0I_CSR_H_TOERR                                                (_ADI_MSK(0x00000010,uint16_t))  /* Timeout Error */
#define ENUM_USB_EP0I_CSR_H_NO_TOERR                                             (_ADI_MSK(0x00000000,uint16_t))  /* TOERR: No Status */
#define ENUM_USB_EP0I_CSR_H_TOERR                                                (_ADI_MSK(0x00000010,uint16_t))  /* TOERR: Timeout Error */

#define BITM_USB_EP0I_CSR_H_SETUPPKT                                             (_ADI_MSK(0x00000008,uint16_t))  /* Setup Packet */
#define ENUM_USB_EP0I_CSR_H_NO_SETUPPKT                                          (_ADI_MSK(0x00000000,uint16_t))  /* SETUPPKT: No Request */
#define ENUM_USB_EP0I_CSR_H_SETUPPKT                                             (_ADI_MSK(0x00000008,uint16_t))  /* SETUPPKT: Send SETUP token */

#define BITM_USB_EP0I_CSR_H_RXSTALL                                              (_ADI_MSK(0x00000004,uint16_t))  /* Rx Stall */
#define ENUM_USB_EP0I_CSR_H_NO_RXSTALL                                           (_ADI_MSK(0x00000000,uint16_t))  /* RXSTALL: No Status */
#define ENUM_USB_EP0I_CSR_H_RXSTALL                                              (_ADI_MSK(0x00000004,uint16_t))  /* RXSTALL: Stall Received from Device */

#define BITM_USB_EP0I_CSR_H_TXPKTRDY                                             (_ADI_MSK(0x00000002,uint16_t))  /* Tx Packet Ready */
#define ENUM_USB_EP0I_CSR_H_NO_TXPKTRDY                                          (_ADI_MSK(0x00000000,uint16_t))  /* TXPKTRDY: No Tx Packet */
#define ENUM_USB_EP0I_CSR_H_TXPKTRDY                                             (_ADI_MSK(0x00000002,uint16_t))  /* TXPKTRDY: Tx Packet in Endpoint FIFO */

#define BITM_USB_EP0I_CSR_H_RXPKTRDY                                             (_ADI_MSK(0x00000001,uint16_t))  /* Rx Packet Ready */
#define ENUM_USB_EP0I_CSR_H_NO_RXPKTRDY                                          (_ADI_MSK(0x00000000,uint16_t))  /* RXPKTRDY: No Rx Packet */
#define ENUM_USB_EP0I_CSR_H_RXPKTRDY                                             (_ADI_MSK(0x00000001,uint16_t))  /* RXPKTRDY: Rx Packet in Endpoint FIFO */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_EPI_TXCSR_P                                                          Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_EPI_TXCSR_P_AUTOSET                                             15                               /* TxPkRdy Autoset Enable */
#define BITP_USB_EPI_TXCSR_P_ISO                                                 14                               /* Isochronous Transfers Enable */
#define BITP_USB_EPI_TXCSR_P_DMAREQEN                                            12                               /* DMA Request Enable Tx EP */
#define BITP_USB_EPI_TXCSR_P_FRCDATATGL                                          11                               /* Force Data Toggle */
#define BITP_USB_EPI_TXCSR_P_DMAREQMODE                                          10                               /* DMA Mode Select */
#define BITP_USB_EPI_TXCSR_P_CLRDATATGL                                           6                               /* Clear Endpoint Data Toggle */
#define BITP_USB_EPI_TXCSR_P_SENTSTALL                                            5                               /* Sent STALL */
#define BITP_USB_EPI_TXCSR_P_SENDSTALL                                            4                               /* Send STALL */
#define BITP_USB_EPI_TXCSR_P_FLUSHFIFO                                            3                               /* Flush Endpoint FIFO */
#define BITP_USB_EPI_TXCSR_P_URUNERR                                              2                               /* Underrun Error */
#define BITP_USB_EPI_TXCSR_P_NEFIFO                                               1                               /* Not Empty FIFO */
#define BITP_USB_EPI_TXCSR_P_TXPKTRDY                                             0                               /* Tx Packet Ready */

#define BITM_USB_EPI_TXCSR_P_AUTOSET                                             (_ADI_MSK(0x00008000,uint16_t))  /* TxPkRdy Autoset Enable */
#define ENUM_USB_EPI_TXCSR_P_NO_AUTOSET                                          (_ADI_MSK(0x00000000,uint16_t))  /* AUTOSET: Disable Autoset */
#define ENUM_USB_EPI_TXCSR_P_AUTOSET                                             (_ADI_MSK(0x00008000,uint16_t))  /* AUTOSET: Enable Autoset */

#define BITM_USB_EPI_TXCSR_P_ISO                                                 (_ADI_MSK(0x00004000,uint16_t))  /* Isochronous Transfers Enable */
#define ENUM_USB_EPI_TXCSR_P_ISODIS                                              (_ADI_MSK(0x00000000,uint16_t))  /* ISO: Disable Tx EP Isochronous Transfers */
#define ENUM_USB_EPI_TXCSR_P_ISOEN                                               (_ADI_MSK(0x00004000,uint16_t))  /* ISO: Enable Tx EP Isochronous Transfers */

#define BITM_USB_EPI_TXCSR_P_DMAREQEN                                            (_ADI_MSK(0x00001000,uint16_t))  /* DMA Request Enable Tx EP */
#define ENUM_USB_EPI_TXCSR_P_DMAREQDIS                                           (_ADI_MSK(0x00000000,uint16_t))  /* DMAREQEN: Disable DMA Request */
#define ENUM_USB_EPI_TXCSR_P_DMAREQEN                                            (_ADI_MSK(0x00001000,uint16_t))  /* DMAREQEN: Enable DMA Request */

#define BITM_USB_EPI_TXCSR_P_FRCDATATGL                                          (_ADI_MSK(0x00000800,uint16_t))  /* Force Data Toggle */
#define ENUM_USB_EPI_TXCSR_P_NO_FRCTGL                                           (_ADI_MSK(0x00000000,uint16_t))  /* FRCDATATGL: No Action */
#define ENUM_USB_EPI_TXCSR_P_FRCTGL                                              (_ADI_MSK(0x00000800,uint16_t))  /* FRCDATATGL: Toggle Endpoint Data */

#define BITM_USB_EPI_TXCSR_P_DMAREQMODE                                          (_ADI_MSK(0x00000400,uint16_t))  /* DMA Mode Select */
#define ENUM_USB_EPI_TXCSR_P_DMARQMODE0                                          (_ADI_MSK(0x00000000,uint16_t))  /* DMAREQMODE: DMA Request Mode 0 */
#define ENUM_USB_EPI_TXCSR_P_DMARQMODE1                                          (_ADI_MSK(0x00000400,uint16_t))  /* DMAREQMODE: DMA Request Mode 1 */

#define BITM_USB_EPI_TXCSR_P_CLRDATATGL                                          (_ADI_MSK(0x00000040,uint16_t))  /* Clear Endpoint Data Toggle */
#define ENUM_USB_EPI_TXCSR_P_NO_CLRTGL                                           (_ADI_MSK(0x00000000,uint16_t))  /* CLRDATATGL: No Action */
#define ENUM_USB_EPI_TXCSR_P_CLRTGL                                              (_ADI_MSK(0x00000040,uint16_t))  /* CLRDATATGL: Reset EP Data Toggle to 0 */

#define BITM_USB_EPI_TXCSR_P_SENTSTALL                                           (_ADI_MSK(0x00000020,uint16_t))  /* Sent STALL */
#define ENUM_USB_EPI_TXCSR_P_NO_STALSNT                                          (_ADI_MSK(0x00000000,uint16_t))  /* SENTSTALL: No Status */
#define ENUM_USB_EPI_TXCSR_P_STALSNT                                             (_ADI_MSK(0x00000020,uint16_t))  /* SENTSTALL: STALL Handshake Transmitted */

#define BITM_USB_EPI_TXCSR_P_SENDSTALL                                           (_ADI_MSK(0x00000010,uint16_t))  /* Send STALL */
#define ENUM_USB_EPI_TXCSR_P_NO_STALL                                            (_ADI_MSK(0x00000000,uint16_t))  /* SENDSTALL: No Request */
#define ENUM_USB_EPI_TXCSR_P_STALL                                               (_ADI_MSK(0x00000010,uint16_t))  /* SENDSTALL: Request STALL Handshake Transmission */

#define BITM_USB_EPI_TXCSR_P_FLUSHFIFO                                           (_ADI_MSK(0x00000008,uint16_t))  /* Flush Endpoint FIFO */
#define ENUM_USB_EPI_TXCSR_P_NO_FLUSH                                            (_ADI_MSK(0x00000000,uint16_t))  /* FLUSHFIFO: No Flush */
#define ENUM_USB_EPI_TXCSR_P_FLUSH                                               (_ADI_MSK(0x00000008,uint16_t))  /* FLUSHFIFO: Flush endpoint FIFO */

#define BITM_USB_EPI_TXCSR_P_URUNERR                                             (_ADI_MSK(0x00000004,uint16_t))  /* Underrun Error */
#define ENUM_USB_EPI_TXCSR_P_NO_URUNERR                                          (_ADI_MSK(0x00000000,uint16_t))  /* URUNERR: No Status */
#define ENUM_USB_EPI_TXCSR_P_URUNERR                                             (_ADI_MSK(0x00000004,uint16_t))  /* URUNERR: Underrun Error */

#define BITM_USB_EPI_TXCSR_P_NEFIFO                                              (_ADI_MSK(0x00000002,uint16_t))  /* Not Empty FIFO */
#define ENUM_USB_EPI_TXCSR_P_NO_FIFONE                                           (_ADI_MSK(0x00000000,uint16_t))  /* NEFIFO: FIFO Empty */
#define ENUM_USB_EPI_TXCSR_P_FIFONE                                              (_ADI_MSK(0x00000002,uint16_t))  /* NEFIFO: FIFO Not Empty */

#define BITM_USB_EPI_TXCSR_P_TXPKTRDY                                            (_ADI_MSK(0x00000001,uint16_t))  /* Tx Packet Ready */
#define ENUM_USB_EPI_TXCSR_P_NO_PKTRDY                                           (_ADI_MSK(0x00000000,uint16_t))  /* TXPKTRDY: No Tx Packet */
#define ENUM_USB_EPI_TXCSR_P_PKTRDY                                              (_ADI_MSK(0x00000001,uint16_t))  /* TXPKTRDY: Tx Packet in Endpoint FIFO */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_EPI_RXMAXP                                                           Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_EPI_RXMAXP_MAXPAY                                                0                               /* Maximum Payload */
#define BITM_USB_EPI_RXMAXP_MAXPAY                                               (_ADI_MSK(0x000007FF,uint16_t))  /* Maximum Payload */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_EPI_RXCSR_H                                                          Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_EPI_RXCSR_H_AUTOCLR                                             15                               /* Auto Clear Enable */
#define BITP_USB_EPI_RXCSR_H_AUTOREQ                                             14                               /* Auto Request Clear Enable */
#define BITP_USB_EPI_RXCSR_H_DMAREQEN                                            13                               /* DMA Request Enable Rx EP */
#define BITP_USB_EPI_RXCSR_H_PIDERR                                              12                               /* Packet ID Error */
#define BITP_USB_EPI_RXCSR_H_DMAREQMODE                                          11                               /* DMA Mode Select */
#define BITP_USB_EPI_RXCSR_H_DATGLEN                                             10                               /* Data Toggle Write Enable */
#define BITP_USB_EPI_RXCSR_H_DATGL                                                9                               /* Data Toggle */
#define BITP_USB_EPI_RXCSR_H_CLRDATATGL                                           7                               /* Clear Endpoint Data Toggle */
#define BITP_USB_EPI_RXCSR_H_RXSTALL                                              6                               /* Rx STALL */
#define BITP_USB_EPI_RXCSR_H_REQPKT                                               5                               /* Request Packet */
#define BITP_USB_EPI_RXCSR_H_FLUSHFIFO                                            4                               /* Flush Endpoint FIFO */
#define BITP_USB_EPI_RXCSR_H_NAKTODERR                                            3                               /* NAK Timeout Data Error */
#define BITP_USB_EPI_RXCSR_H_RXTOERR                                              2                               /* Rx Timeout Error */
#define BITP_USB_EPI_RXCSR_H_FIFOFULL                                             1                               /* FIFO Full */
#define BITP_USB_EPI_RXCSR_H_RXPKTRDY                                             0                               /* Rx Packet Ready */

#define BITM_USB_EPI_RXCSR_H_AUTOCLR                                             (_ADI_MSK(0x00008000,uint16_t))  /* Auto Clear Enable */
#define ENUM_USB_EPI_RXCSR_H_NO_AUTOCLR                                          (_ADI_MSK(0x00000000,uint16_t))  /* AUTOCLR: Disable Auto Clear */
#define ENUM_USB_EPI_RXCSR_H_AUTOCLR                                             (_ADI_MSK(0x00008000,uint16_t))  /* AUTOCLR: Enable Auto Clear */

#define BITM_USB_EPI_RXCSR_H_AUTOREQ                                             (_ADI_MSK(0x00004000,uint16_t))  /* Auto Request Clear Enable */
#define ENUM_USB_EPI_RXCSR_H_NO_AUTOREQ                                          (_ADI_MSK(0x00000000,uint16_t))  /* AUTOREQ: Disable Auto Request Clear */
#define ENUM_USB_EPI_RXCSR_H_AUTOREQ                                             (_ADI_MSK(0x00004000,uint16_t))  /* AUTOREQ: Enable Auto Request Clear */

#define BITM_USB_EPI_RXCSR_H_DMAREQEN                                            (_ADI_MSK(0x00002000,uint16_t))  /* DMA Request Enable Rx EP */
#define ENUM_USB_EPI_RXCSR_H_DMAREQDIS                                           (_ADI_MSK(0x00000000,uint16_t))  /* DMAREQEN: Disable DMA Request */
#define ENUM_USB_EPI_RXCSR_H_DMAREQEN                                            (_ADI_MSK(0x00002000,uint16_t))  /* DMAREQEN: Enable DMA Request */

#define BITM_USB_EPI_RXCSR_H_PIDERR                                              (_ADI_MSK(0x00001000,uint16_t))  /* Packet ID Error */
#define ENUM_USB_EPI_RXCSR_H_NO_PIDERR                                           (_ADI_MSK(0x00000000,uint16_t))  /* PIDERR: No Status */
#define ENUM_USB_EPI_RXCSR_H_PIDERR                                              (_ADI_MSK(0x00001000,uint16_t))  /* PIDERR: PID Error */

#define BITM_USB_EPI_RXCSR_H_DMAREQMODE                                          (_ADI_MSK(0x00000800,uint16_t))  /* DMA Mode Select */
#define ENUM_USB_EPI_RXCSR_H_DMARQMODE0                                          (_ADI_MSK(0x00000000,uint16_t))  /* DMAREQMODE: DMA Request Mode 0 */
#define ENUM_USB_EPI_RXCSR_H_DMARQMODE1                                          (_ADI_MSK(0x00000800,uint16_t))  /* DMAREQMODE: DMA Request Mode 1 */

#define BITM_USB_EPI_RXCSR_H_DATGLEN                                             (_ADI_MSK(0x00000400,uint16_t))  /* Data Toggle Write Enable */
#define ENUM_USB_EPI_RXCSR_H_DATGLDIS                                            (_ADI_MSK(0x00000000,uint16_t))  /* DATGLEN: Disable Write to DATGL */
#define ENUM_USB_EPI_RXCSR_H_DATGLEN                                             (_ADI_MSK(0x00000400,uint16_t))  /* DATGLEN: Enable Write to DATGL */

#define BITM_USB_EPI_RXCSR_H_DATGL                                               (_ADI_MSK(0x00000200,uint16_t))  /* Data Toggle */
#define ENUM_USB_EPI_RXCSR_H_NO_DATGL                                            (_ADI_MSK(0x00000000,uint16_t))  /* DATGL: DATA0 is Set */
#define ENUM_USB_EPI_RXCSR_H_DATGL                                               (_ADI_MSK(0x00000200,uint16_t))  /* DATGL: DATA1 is Set */

#define BITM_USB_EPI_RXCSR_H_CLRDATATGL                                          (_ADI_MSK(0x00000080,uint16_t))  /* Clear Endpoint Data Toggle */
#define ENUM_USB_EPI_RXCSR_H_NO_CLRTGL                                           (_ADI_MSK(0x00000000,uint16_t))  /* CLRDATATGL: No Action */
#define ENUM_USB_EPI_RXCSR_H_CLRTGL                                              (_ADI_MSK(0x00000080,uint16_t))  /* CLRDATATGL: Reset EP Data Toggle to 0 */

#define BITM_USB_EPI_RXCSR_H_RXSTALL                                             (_ADI_MSK(0x00000040,uint16_t))  /* Rx STALL */
#define ENUM_USB_EPI_RXCSR_H_NO_RXSTALL                                          (_ADI_MSK(0x00000000,uint16_t))  /* RXSTALL: No Status */
#define ENUM_USB_EPI_RXCSR_H_RXSTALL                                             (_ADI_MSK(0x00000040,uint16_t))  /* RXSTALL: Stall Received from Device */

#define BITM_USB_EPI_RXCSR_H_REQPKT                                              (_ADI_MSK(0x00000020,uint16_t))  /* Request Packet */
#define ENUM_USB_EPI_RXCSR_H_NO_REQPKT                                           (_ADI_MSK(0x00000000,uint16_t))  /* REQPKT: No Request */
#define ENUM_USB_EPI_RXCSR_H_REQPKT                                              (_ADI_MSK(0x00000020,uint16_t))  /* REQPKT: Send IN Tokens to Device */

#define BITM_USB_EPI_RXCSR_H_FLUSHFIFO                                           (_ADI_MSK(0x00000010,uint16_t))  /* Flush Endpoint FIFO */
#define ENUM_USB_EPI_RXCSR_H_NO_FLUSH                                            (_ADI_MSK(0x00000000,uint16_t))  /* FLUSHFIFO: No Flush */
#define ENUM_USB_EPI_RXCSR_H_FLUSH                                               (_ADI_MSK(0x00000010,uint16_t))  /* FLUSHFIFO: Flush Endpoint FIFO */

#define BITM_USB_EPI_RXCSR_H_NAKTODERR                                           (_ADI_MSK(0x00000008,uint16_t))  /* NAK Timeout Data Error */
#define ENUM_USB_EPI_RXCSR_H_NO_NAKTO                                            (_ADI_MSK(0x00000000,uint16_t))  /* NAKTODERR: No Status */
#define ENUM_USB_EPI_RXCSR_H_NAKTO                                               (_ADI_MSK(0x00000008,uint16_t))  /* NAKTODERR: NAK Timeout Data Error */

#define BITM_USB_EPI_RXCSR_H_RXTOERR                                             (_ADI_MSK(0x00000004,uint16_t))  /* Rx Timeout Error */
#define ENUM_USB_EPI_RXCSR_H_NO_RXTOERR                                          (_ADI_MSK(0x00000000,uint16_t))  /* RXTOERR: No Status */
#define ENUM_USB_EPI_RXCSR_H_RXTOERR                                             (_ADI_MSK(0x00000004,uint16_t))  /* RXTOERR: Rx Timeout Error */

#define BITM_USB_EPI_RXCSR_H_FIFOFULL                                            (_ADI_MSK(0x00000002,uint16_t))  /* FIFO Full */
#define ENUM_USB_EPI_RXCSR_H_NO_FIFOFUL                                          (_ADI_MSK(0x00000000,uint16_t))  /* FIFOFULL: No Status */
#define ENUM_USB_EPI_RXCSR_H_FIFOFUL                                             (_ADI_MSK(0x00000002,uint16_t))  /* FIFOFULL: FIFO Full */

#define BITM_USB_EPI_RXCSR_H_RXPKTRDY                                            (_ADI_MSK(0x00000001,uint16_t))  /* Rx Packet Ready */
#define ENUM_USB_EPI_RXCSR_H_NO_PKTRDY                                           (_ADI_MSK(0x00000000,uint16_t))  /* RXPKTRDY: No Rx Packet */
#define ENUM_USB_EPI_RXCSR_H_PKTRDY                                              (_ADI_MSK(0x00000001,uint16_t))  /* RXPKTRDY: Rx Packet in Endpoint FIFO */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_EPI_RXCSR_P                                                          Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_EPI_RXCSR_P_AUTOCLR                                             15                               /* Auto Clear Enable */
#define BITP_USB_EPI_RXCSR_P_ISO                                                 14                               /* Isochronous Transfers */
#define BITP_USB_EPI_RXCSR_P_DMAREQEN                                            13                               /* DMA Request Enable Rx EP */
#define BITP_USB_EPI_RXCSR_P_DNYETPERR                                           12                               /* Disable NYET Handshake */
#define BITP_USB_EPI_RXCSR_P_DMAREQMODE                                          11                               /* DMA Mode Select */
#define BITP_USB_EPI_RXCSR_P_CLRDATATGL                                           7                               /* Clear Endpoint Data Toggle */
#define BITP_USB_EPI_RXCSR_P_SENTSTALL                                            6                               /* Sent STALL */
#define BITP_USB_EPI_RXCSR_P_SENDSTALL                                            5                               /* Send STALL */
#define BITP_USB_EPI_RXCSR_P_FLUSHFIFO                                            4                               /* Flush Endpoint FIFO */
#define BITP_USB_EPI_RXCSR_P_DATAERR                                              3                               /* Data Error */
#define BITP_USB_EPI_RXCSR_P_ORUNERR                                              2                               /* OUT Run Error */
#define BITP_USB_EPI_RXCSR_P_FIFOFULL                                             1                               /* FIFO Full */
#define BITP_USB_EPI_RXCSR_P_RXPKTRDY                                             0                               /* Rx Packet Ready */

#define BITM_USB_EPI_RXCSR_P_AUTOCLR                                             (_ADI_MSK(0x00008000,uint16_t))  /* Auto Clear Enable */
#define ENUM_USB_EPI_RXCSR_P_NO_AUTOCLR                                          (_ADI_MSK(0x00000000,uint16_t))  /* AUTOCLR: Disable Auto Clear */
#define ENUM_USB_EPI_RXCSR_P_AUTOCLR                                             (_ADI_MSK(0x00008000,uint16_t))  /* AUTOCLR: Enable Auto Clear */

#define BITM_USB_EPI_RXCSR_P_ISO                                                 (_ADI_MSK(0x00004000,uint16_t))  /* Isochronous Transfers */
#define ENUM_USB_EPI_RXCSR_P_ISODIS                                              (_ADI_MSK(0x00000000,uint16_t))  /* ISO: This bit should be cleared for bulk or interrupt transfers. */
#define ENUM_USB_EPI_RXCSR_P_ISOEN                                               (_ADI_MSK(0x00004000,uint16_t))  /* ISO: This bit should be set for isochronous transfers. */

#define BITM_USB_EPI_RXCSR_P_DMAREQEN                                            (_ADI_MSK(0x00002000,uint16_t))  /* DMA Request Enable Rx EP */
#define ENUM_USB_EPI_RXCSR_P_DMAREQDIS                                           (_ADI_MSK(0x00000000,uint16_t))  /* DMAREQEN: Disable DMA Request */
#define ENUM_USB_EPI_RXCSR_P_DMAREQEN                                            (_ADI_MSK(0x00002000,uint16_t))  /* DMAREQEN: Enable DMA Request */

#define BITM_USB_EPI_RXCSR_P_DNYETPERR                                           (_ADI_MSK(0x00001000,uint16_t))  /* Disable NYET Handshake */
#define ENUM_USB_EPI_RXCSR_P_DNYTERREN                                           (_ADI_MSK(0x00000000,uint16_t))  /* DNYETPERR: Enable NYET Handshake */
#define ENUM_USB_EPI_RXCSR_P_DNYTERRDIS                                          (_ADI_MSK(0x00001000,uint16_t))  /* DNYETPERR: Disable NYET Handshake */

#define BITM_USB_EPI_RXCSR_P_DMAREQMODE                                          (_ADI_MSK(0x00000800,uint16_t))  /* DMA Mode Select */
#define ENUM_USB_EPI_RXCSR_P_DMARQMODE0                                          (_ADI_MSK(0x00000000,uint16_t))  /* DMAREQMODE: DMA Request Mode 0 */
#define ENUM_USB_EPI_RXCSR_P_DMARQMODE1                                          (_ADI_MSK(0x00000800,uint16_t))  /* DMAREQMODE: DMA Request Mode 1 */

#define BITM_USB_EPI_RXCSR_P_CLRDATATGL                                          (_ADI_MSK(0x00000080,uint16_t))  /* Clear Endpoint Data Toggle */
#define ENUM_USB_EPI_RXCSR_P_NO_CLRTGL                                           (_ADI_MSK(0x00000000,uint16_t))  /* CLRDATATGL: No Action */
#define ENUM_USB_EPI_RXCSR_P_CLRTGL                                              (_ADI_MSK(0x00000080,uint16_t))  /* CLRDATATGL: Reset EP Data Toggle to 0 */

#define BITM_USB_EPI_RXCSR_P_SENTSTALL                                           (_ADI_MSK(0x00000040,uint16_t))  /* Sent STALL */
#define ENUM_USB_EPI_RXCSR_P_NO_STALSNT                                          (_ADI_MSK(0x00000000,uint16_t))  /* SENTSTALL: No Status */
#define ENUM_USB_EPI_RXCSR_P_STALSNT                                             (_ADI_MSK(0x00000040,uint16_t))  /* SENTSTALL: STALL Handshake Transmitted */

#define BITM_USB_EPI_RXCSR_P_SENDSTALL                                           (_ADI_MSK(0x00000020,uint16_t))  /* Send STALL */
#define ENUM_USB_EPI_RXCSR_P_NO_STALL                                            (_ADI_MSK(0x00000000,uint16_t))  /* SENDSTALL: No Action */
#define ENUM_USB_EPI_RXCSR_P_STALL                                               (_ADI_MSK(0x00000020,uint16_t))  /* SENDSTALL: Request STALL Handshake */

#define BITM_USB_EPI_RXCSR_P_FLUSHFIFO                                           (_ADI_MSK(0x00000010,uint16_t))  /* Flush Endpoint FIFO */
#define ENUM_USB_EPI_RXCSR_P_NO_FLUSH                                            (_ADI_MSK(0x00000000,uint16_t))  /* FLUSHFIFO: No Flush */
#define ENUM_USB_EPI_RXCSR_P_FLUSH                                               (_ADI_MSK(0x00000010,uint16_t))  /* FLUSHFIFO: Flush Endpoint FIFO */

#define BITM_USB_EPI_RXCSR_P_DATAERR                                             (_ADI_MSK(0x00000008,uint16_t))  /* Data Error */
#define ENUM_USB_EPI_RXCSR_P_NO_DATAERR                                          (_ADI_MSK(0x00000000,uint16_t))  /* DATAERR: No Status */
#define ENUM_USB_EPI_RXCSR_P_DATAERR                                             (_ADI_MSK(0x00000008,uint16_t))  /* DATAERR: Data Error */

#define BITM_USB_EPI_RXCSR_P_ORUNERR                                             (_ADI_MSK(0x00000004,uint16_t))  /* OUT Run Error */
#define ENUM_USB_EPI_RXCSR_P_NO_ORUNERR                                          (_ADI_MSK(0x00000000,uint16_t))  /* ORUNERR: No Status */
#define ENUM_USB_EPI_RXCSR_P_ORUNERR                                             (_ADI_MSK(0x00000004,uint16_t))  /* ORUNERR: OUT Run Error */

#define BITM_USB_EPI_RXCSR_P_FIFOFULL                                            (_ADI_MSK(0x00000002,uint16_t))  /* FIFO Full */
#define ENUM_USB_EPI_RXCSR_P_NO_FIFOFUL                                          (_ADI_MSK(0x00000000,uint16_t))  /* FIFOFULL: No Status */
#define ENUM_USB_EPI_RXCSR_P_FIFOFUL                                             (_ADI_MSK(0x00000002,uint16_t))  /* FIFOFULL: FIFO Full */

#define BITM_USB_EPI_RXCSR_P_RXPKTRDY                                            (_ADI_MSK(0x00000001,uint16_t))  /* Rx Packet Ready */
#define ENUM_USB_EPI_RXCSR_P_NO_PKTRDY                                           (_ADI_MSK(0x00000000,uint16_t))  /* RXPKTRDY: No Rx Packet */
#define ENUM_USB_EPI_RXCSR_P_PKTRDY                                              (_ADI_MSK(0x00000001,uint16_t))  /* RXPKTRDY: Rx Packet in Endpoint FIFO */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_EP0I_CNT                                                             Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_EP0I_CNT_RXCNT                                                   0                               /* Rx Byte Count Value */
#define BITM_USB_EP0I_CNT_RXCNT                                                  (_ADI_MSK(0x0000007F,uint16_t))  /* Rx Byte Count Value */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_EPI_RXCNT                                                            Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_EPI_RXCNT_EPRXCNT                                                0                               /* EP Rx Count */
#define BITM_USB_EPI_RXCNT_EPRXCNT                                               (_ADI_MSK(0x00003FFF,uint16_t))  /* EP Rx Count */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_EP0I_CFGDATA                                                         Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_EP0I_CFGDATA_BIGEND                                              5                               /* Big Endian Data */
#define BITP_USB_EP0I_CFGDATA_DYNFIFO                                             2                               /* Dynamic FIFO Size Enable */
#define BITP_USB_EP0I_CFGDATA_SOFTCON                                             1                               /* Soft Connect Enable */
#define BITP_USB_EP0I_CFGDATA_UTMIWID                                             0                               /* UTMI Data Width */

#define BITM_USB_EP0I_CFGDATA_BIGEND                                             (_ADI_MSK(0x00000020,uint8_t))   /* Big Endian Data */
#define ENUM_USB_EP0I_CFGDATA_BIGENDDIS                                          (_ADI_MSK(0x00000000,uint8_t))   /* BIGEND: Little Endian Configuration */
#define ENUM_USB_EP0I_CFGDATA_BIGENDEN                                           (_ADI_MSK(0x00000020,uint8_t))   /* BIGEND: Big Endian Configuration */

#define BITM_USB_EP0I_CFGDATA_DYNFIFO                                            (_ADI_MSK(0x00000004,uint8_t))   /* Dynamic FIFO Size Enable */
#define ENUM_USB_EP0I_CFGDATA_DYNSZDIS                                           (_ADI_MSK(0x00000000,uint8_t))   /* DYNFIFO: No Dynamic FIFO Size */
#define ENUM_USB_EP0I_CFGDATA_DYNSZEN                                            (_ADI_MSK(0x00000004,uint8_t))   /* DYNFIFO: Dynamic FIFO Size */

#define BITM_USB_EP0I_CFGDATA_SOFTCON                                            (_ADI_MSK(0x00000002,uint8_t))   /* Soft Connect Enable */
#define ENUM_USB_EP0I_CFGDATA_SFTCONDIS                                          (_ADI_MSK(0x00000000,uint8_t))   /* SOFTCON: No Soft Connect */
#define ENUM_USB_EP0I_CFGDATA_SFTCONEN                                           (_ADI_MSK(0x00000002,uint8_t))   /* SOFTCON: Soft Connect */

#define BITM_USB_EP0I_CFGDATA_UTMIWID                                            (_ADI_MSK(0x00000001,uint8_t))   /* UTMI Data Width */
#define ENUM_USB_EP0I_CFGDATA_UTMIWID8                                           (_ADI_MSK(0x00000000,uint8_t))   /* UTMIWID: 8-bit UTMI Data Width */
#define ENUM_USB_EP0I_CFGDATA_UTMIWID16                                          (_ADI_MSK(0x00000001,uint8_t))   /* UTMIWID: 16-bit UTMI Data Width */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_EPI_FIFOSIZE                                                         Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_EPI_FIFOSIZE_RXFIFOSIZE                                          4
#define BITP_USB_EPI_FIFOSIZE_TXFIFOSIZE                                          0
#define BITM_USB_EPI_FIFOSIZE_RXFIFOSIZE                                         (_ADI_MSK(0x000000F0,uint8_t))
#define BITM_USB_EPI_FIFOSIZE_TXFIFOSIZE                                         (_ADI_MSK(0x0000000F,uint8_t))

/* ------------------------------------------------------------------------------------------------------------------------
        USB_DEV_CTL                                                              Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_DEV_CTL_VBUS                                                     3                               /* VBUS Level Indicator */
#define BITP_USB_DEV_CTL_SESSION                                                  0                               /* Session Indicator */

#define BITM_USB_DEV_CTL_VBUS                                                    (_ADI_MSK(0x00000018,uint8_t))   /* VBUS Level Indicator */
#define ENUM_USB_DEV_CTL_VBUS_BS                                                 (_ADI_MSK(0x00000000,uint8_t))   /* VBUS: Below SessionEnd */
#define ENUM_USB_DEV_CTL_VBUS_ASBA                                               (_ADI_MSK(0x00000008,uint8_t))   /* VBUS: Above SessionEnd, below AValid */
#define ENUM_USB_DEV_CTL_VBUS_AABV                                               (_ADI_MSK(0x00000010,uint8_t))   /* VBUS: Above AValid, below VBUSValid */
#define ENUM_USB_DEV_CTL_VBUS_AV                                                 (_ADI_MSK(0x00000018,uint8_t))   /* VBUS: Above VBUSValid */

#define BITM_USB_DEV_CTL_SESSION                                                 (_ADI_MSK(0x00000001,uint8_t))   /* Session Indicator */
#define ENUM_USB_DEV_CTL_NO_SESSION                                              (_ADI_MSK(0x00000000,uint8_t))   /* SESSION: Not Detected */
#define ENUM_USB_DEV_CTL_SESSION                                                 (_ADI_MSK(0x00000001,uint8_t))   /* SESSION: Detected Session */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_EPINFO                                                               Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_EPINFO_RXEP                                                      4                               /* Rx Endpoints */
#define BITP_USB_EPINFO_TXEP                                                      0                               /* Tx Endpoints */
#define BITM_USB_EPINFO_RXEP                                                     (_ADI_MSK(0x000000F0,uint8_t))   /* Rx Endpoints */
#define BITM_USB_EPINFO_TXEP                                                     (_ADI_MSK(0x0000000F,uint8_t))   /* Tx Endpoints */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_RAMINFO                                                              Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_RAMINFO_DMACHANS                                                 4                               /* DMA Channels */
#define BITP_USB_RAMINFO_RAMBITS                                                  0                               /* RAM Address Bits */
#define BITM_USB_RAMINFO_DMACHANS                                                (_ADI_MSK(0x000000F0,uint8_t))   /* DMA Channels */
#define BITM_USB_RAMINFO_RAMBITS                                                 (_ADI_MSK(0x0000000F,uint8_t))   /* RAM Address Bits */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_LINKINFO                                                             Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_LINKINFO_WTCON                                                   4                               /* Wait for Connect/Disconnect */
#define BITP_USB_LINKINFO_WTID                                                    0                               /* Wait from ID Pull-up */
#define BITM_USB_LINKINFO_WTCON                                                  (_ADI_MSK(0x000000F0,uint8_t))   /* Wait for Connect/Disconnect */
#define BITM_USB_LINKINFO_WTID                                                   (_ADI_MSK(0x0000000F,uint8_t))   /* Wait from ID Pull-up */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_SOFT_RST                                                             Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_SOFT_RST_RSTX                                                    1                               /* Reset USB XCLK Domain */
#define BITP_USB_SOFT_RST_RST                                                     0                               /* Reset USB CLK Domain */

#define BITM_USB_SOFT_RST_RSTX                                                   (_ADI_MSK(0x00000002,uint8_t))   /* Reset USB XCLK Domain */
#define ENUM_USB_SOFT_RST_NO_RSTX                                                (_ADI_MSK(0x00000000,uint8_t))   /* RSTX: No Reset */
#define ENUM_USB_SOFT_RST_RSTX                                                   (_ADI_MSK(0x00000002,uint8_t))   /* RSTX: Reset USB XCLK Domain */

#define BITM_USB_SOFT_RST_RST                                                    (_ADI_MSK(0x00000001,uint8_t))   /* Reset USB CLK Domain */
#define ENUM_USB_SOFT_RST_NO_RST                                                 (_ADI_MSK(0x00000000,uint8_t))   /* RST: No Reset */
#define ENUM_USB_SOFT_RST_RST                                                    (_ADI_MSK(0x00000001,uint8_t))   /* RST: Reset USB CLK Domain */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_EP_TXMAXP                                                            Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_EP_TXMAXP_MAXPAY                                                 0                               /* Maximum Payload */
#define BITM_USB_EP_TXMAXP_MAXPAY                                                (_ADI_MSK(0x000007FF,uint16_t))  /* Maximum Payload */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_EP_TXCSR_P                                                           Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_EP_TXCSR_P_AUTOSET                                              15                               /* TxPkRdy Autoset Enable */
#define BITP_USB_EP_TXCSR_P_ISO                                                  14                               /* Isochronous Transfers Enable */
#define BITP_USB_EP_TXCSR_P_DMAREQEN                                             12                               /* DMA Request Enable Tx EP */
#define BITP_USB_EP_TXCSR_P_FRCDATATGL                                           11                               /* Force Data Toggle */
#define BITP_USB_EP_TXCSR_P_DMAREQMODE                                           10                               /* DMA Mode Select */
#define BITP_USB_EP_TXCSR_P_CLRDATATGL                                            6                               /* Clear Endpoint Data Toggle */
#define BITP_USB_EP_TXCSR_P_SENTSTALL                                             5                               /* Sent STALL */
#define BITP_USB_EP_TXCSR_P_SENDSTALL                                             4                               /* Send STALL */
#define BITP_USB_EP_TXCSR_P_FLUSHFIFO                                             3                               /* Flush Endpoint FIFO */
#define BITP_USB_EP_TXCSR_P_URUNERR                                               2                               /* Underrun Error */
#define BITP_USB_EP_TXCSR_P_NEFIFO                                                1                               /* Not Empty FIFO */
#define BITP_USB_EP_TXCSR_P_TXPKTRDY                                              0                               /* Tx Packet Ready */

#define BITM_USB_EP_TXCSR_P_AUTOSET                                              (_ADI_MSK(0x00008000,uint16_t))  /* TxPkRdy Autoset Enable */
#define ENUM_USB_EP_TXCSR_P_NO_AUTOSET                                           (_ADI_MSK(0x00000000,uint16_t))  /* AUTOSET: Disable Autoset */
#define ENUM_USB_EP_TXCSR_P_AUTOSET                                              (_ADI_MSK(0x00008000,uint16_t))  /* AUTOSET: Enable Autoset */

#define BITM_USB_EP_TXCSR_P_ISO                                                  (_ADI_MSK(0x00004000,uint16_t))  /* Isochronous Transfers Enable */
#define ENUM_USB_EP_TXCSR_P_ISODIS                                               (_ADI_MSK(0x00000000,uint16_t))  /* ISO: Disable Tx EP Isochronous Transfers */
#define ENUM_USB_EP_TXCSR_P_ISOEN                                                (_ADI_MSK(0x00004000,uint16_t))  /* ISO: Enable Tx EP Isochronous Transfers */

#define BITM_USB_EP_TXCSR_P_DMAREQEN                                             (_ADI_MSK(0x00001000,uint16_t))  /* DMA Request Enable Tx EP */
#define ENUM_USB_EP_TXCSR_P_DMAREQDIS                                            (_ADI_MSK(0x00000000,uint16_t))  /* DMAREQEN: Disable DMA Request */
#define ENUM_USB_EP_TXCSR_P_DMAREQEN                                             (_ADI_MSK(0x00001000,uint16_t))  /* DMAREQEN: Enable DMA Request */

#define BITM_USB_EP_TXCSR_P_FRCDATATGL                                           (_ADI_MSK(0x00000800,uint16_t))  /* Force Data Toggle */
#define ENUM_USB_EP_TXCSR_P_NO_FRCTGL                                            (_ADI_MSK(0x00000000,uint16_t))  /* FRCDATATGL: No Action */
#define ENUM_USB_EP_TXCSR_P_FRCTGL                                               (_ADI_MSK(0x00000800,uint16_t))  /* FRCDATATGL: Toggle Endpoint Data */

#define BITM_USB_EP_TXCSR_P_DMAREQMODE                                           (_ADI_MSK(0x00000400,uint16_t))  /* DMA Mode Select */
#define ENUM_USB_EP_TXCSR_P_DMARQMODE0                                           (_ADI_MSK(0x00000000,uint16_t))  /* DMAREQMODE: DMA Request Mode 0 */
#define ENUM_USB_EP_TXCSR_P_DMARQMODE1                                           (_ADI_MSK(0x00000400,uint16_t))  /* DMAREQMODE: DMA Request Mode 1 */

#define BITM_USB_EP_TXCSR_P_CLRDATATGL                                           (_ADI_MSK(0x00000040,uint16_t))  /* Clear Endpoint Data Toggle */
#define ENUM_USB_EP_TXCSR_P_NO_CLRTGL                                            (_ADI_MSK(0x00000000,uint16_t))  /* CLRDATATGL: No Action */
#define ENUM_USB_EP_TXCSR_P_CLRTGL                                               (_ADI_MSK(0x00000040,uint16_t))  /* CLRDATATGL: Reset EP Data Toggle to 0 */

#define BITM_USB_EP_TXCSR_P_SENTSTALL                                            (_ADI_MSK(0x00000020,uint16_t))  /* Sent STALL */
#define ENUM_USB_EP_TXCSR_P_NO_STALSNT                                           (_ADI_MSK(0x00000000,uint16_t))  /* SENTSTALL: No Status */
#define ENUM_USB_EP_TXCSR_P_STALSNT                                              (_ADI_MSK(0x00000020,uint16_t))  /* SENTSTALL: STALL Handshake Transmitted */

#define BITM_USB_EP_TXCSR_P_SENDSTALL                                            (_ADI_MSK(0x00000010,uint16_t))  /* Send STALL */
#define ENUM_USB_EP_TXCSR_P_NO_STALL                                             (_ADI_MSK(0x00000000,uint16_t))  /* SENDSTALL: No Request */
#define ENUM_USB_EP_TXCSR_P_STALL                                                (_ADI_MSK(0x00000010,uint16_t))  /* SENDSTALL: Request STALL Handshake Transmission */

#define BITM_USB_EP_TXCSR_P_FLUSHFIFO                                            (_ADI_MSK(0x00000008,uint16_t))  /* Flush Endpoint FIFO */
#define ENUM_USB_EP_TXCSR_P_NO_FLUSH                                             (_ADI_MSK(0x00000000,uint16_t))  /* FLUSHFIFO: No Flush */
#define ENUM_USB_EP_TXCSR_P_FLUSH                                                (_ADI_MSK(0x00000008,uint16_t))  /* FLUSHFIFO: Flush endpoint FIFO */

#define BITM_USB_EP_TXCSR_P_URUNERR                                              (_ADI_MSK(0x00000004,uint16_t))  /* Underrun Error */
#define ENUM_USB_EP_TXCSR_P_NO_URUNERR                                           (_ADI_MSK(0x00000000,uint16_t))  /* URUNERR: No Status */
#define ENUM_USB_EP_TXCSR_P_URUNERR                                              (_ADI_MSK(0x00000004,uint16_t))  /* URUNERR: Underrun Error */

#define BITM_USB_EP_TXCSR_P_NEFIFO                                               (_ADI_MSK(0x00000002,uint16_t))  /* Not Empty FIFO */
#define ENUM_USB_EP_TXCSR_P_NO_FIFONE                                            (_ADI_MSK(0x00000000,uint16_t))  /* NEFIFO: FIFO Empty */
#define ENUM_USB_EP_TXCSR_P_FIFONE                                               (_ADI_MSK(0x00000002,uint16_t))  /* NEFIFO: FIFO Not Empty */

#define BITM_USB_EP_TXCSR_P_TXPKTRDY                                             (_ADI_MSK(0x00000001,uint16_t))  /* Tx Packet Ready */
#define ENUM_USB_EP_TXCSR_P_NO_PKTRDY                                            (_ADI_MSK(0x00000000,uint16_t))  /* TXPKTRDY: No Tx Packet */
#define ENUM_USB_EP_TXCSR_P_PKTRDY                                               (_ADI_MSK(0x00000001,uint16_t))  /* TXPKTRDY: Tx Packet in Endpoint FIFO */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_EP0_CSR_P                                                            Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_EP0_CSR_P_FLUSHFIFO                                              8                               /* Flush Endpoint FIFO */
#define BITP_USB_EP0_CSR_P_SSETUPEND                                              7                               /* Service Setup End */
#define BITP_USB_EP0_CSR_P_SPKTRDY                                                6                               /* Service Rx Packet Ready */
#define BITP_USB_EP0_CSR_P_SENDSTALL                                              5                               /* Send Stall */
#define BITP_USB_EP0_CSR_P_SETUPEND                                               4                               /* Setup End */
#define BITP_USB_EP0_CSR_P_DATAEND                                                3                               /* Data End */
#define BITP_USB_EP0_CSR_P_SENTSTALL                                              2                               /* Sent Stall */
#define BITP_USB_EP0_CSR_P_TXPKTRDY                                               1                               /* Tx Packet Ready */
#define BITP_USB_EP0_CSR_P_RXPKTRDY                                               0                               /* Rx Packet Ready */

#define BITM_USB_EP0_CSR_P_FLUSHFIFO                                             (_ADI_MSK(0x00000100,uint16_t))  /* Flush Endpoint FIFO */
#define ENUM_USB_EP0_CSR_P_NO_FLUSH                                              (_ADI_MSK(0x00000000,uint16_t))  /* FLUSHFIFO: No Flush */
#define ENUM_USB_EP0_CSR_P_FLUSH                                                 (_ADI_MSK(0x00000100,uint16_t))  /* FLUSHFIFO: Flush Endpoint FIFO */

#define BITM_USB_EP0_CSR_P_SSETUPEND                                             (_ADI_MSK(0x00000080,uint16_t))  /* Service Setup End */
#define ENUM_USB_EP0_CSR_P_NOSSETUPEND                                           (_ADI_MSK(0x00000000,uint16_t))  /* SSETUPEND: No Action */
#define ENUM_USB_EP0_CSR_P_SSETUPEND                                             (_ADI_MSK(0x00000080,uint16_t))  /* SSETUPEND: Clear SETUPEND Bit */

#define BITM_USB_EP0_CSR_P_SPKTRDY                                               (_ADI_MSK(0x00000040,uint16_t))  /* Service Rx Packet Ready */
#define ENUM_USB_EP0_CSR_P_NO_SPKTRDY                                            (_ADI_MSK(0x00000000,uint16_t))  /* SPKTRDY: No Action */
#define ENUM_USB_EP0_CSR_P_SPKTRDY                                               (_ADI_MSK(0x00000040,uint16_t))  /* SPKTRDY: Clear RXPKTRDY Bit */

#define BITM_USB_EP0_CSR_P_SENDSTALL                                             (_ADI_MSK(0x00000020,uint16_t))  /* Send Stall */
#define ENUM_USB_EP0_CSR_P_NO_STALL                                              (_ADI_MSK(0x00000000,uint16_t))  /* SENDSTALL: No Action */
#define ENUM_USB_EP0_CSR_P_STALL                                                 (_ADI_MSK(0x00000020,uint16_t))  /* SENDSTALL: Terminate Current Transaction */

#define BITM_USB_EP0_CSR_P_SETUPEND                                              (_ADI_MSK(0x00000010,uint16_t))  /* Setup End */
#define ENUM_USB_EP0_CSR_P_NO_SETUPEND                                           (_ADI_MSK(0x00000000,uint16_t))  /* SETUPEND: No Status */
#define ENUM_USB_EP0_CSR_P_SETUPEND                                              (_ADI_MSK(0x00000010,uint16_t))  /* SETUPEND: Setup Ended before DATAEND */

#define BITM_USB_EP0_CSR_P_DATAEND                                               (_ADI_MSK(0x00000008,uint16_t))  /* Data End */
#define ENUM_USB_EP0_CSR_P_NO_DATAEND                                            (_ADI_MSK(0x00000000,uint16_t))  /* DATAEND: No Status */
#define ENUM_USB_EP0_CSR_P_DATAEND                                               (_ADI_MSK(0x00000008,uint16_t))  /* DATAEND: Data End Condition */

#define BITM_USB_EP0_CSR_P_SENTSTALL                                             (_ADI_MSK(0x00000004,uint16_t))  /* Sent Stall */
#define ENUM_USB_EP0_CSR_P_NO_STALSNT                                            (_ADI_MSK(0x00000000,uint16_t))  /* SENTSTALL: No Status */
#define ENUM_USB_EP0_CSR_P_STALSNT                                               (_ADI_MSK(0x00000004,uint16_t))  /* SENTSTALL: Transmitted STALL Handshake */

#define BITM_USB_EP0_CSR_P_TXPKTRDY                                              (_ADI_MSK(0x00000002,uint16_t))  /* Tx Packet Ready */
#define ENUM_USB_EP0_CSR_P_NO_TXPKTRDY                                           (_ADI_MSK(0x00000000,uint16_t))  /* TXPKTRDY:  */
#define ENUM_USB_EP0_CSR_P_TXPKTRDY                                              (_ADI_MSK(0x00000002,uint16_t))  /* TXPKTRDY: Set this bit after loading a data packet into the FIFO */

#define BITM_USB_EP0_CSR_P_RXPKTRDY                                              (_ADI_MSK(0x00000001,uint16_t))  /* Rx Packet Ready */
#define ENUM_USB_EP0_CSR_P_NO_PKTRDY                                             (_ADI_MSK(0x00000000,uint16_t))  /* RXPKTRDY: No Rx Packet */
#define ENUM_USB_EP0_CSR_P_PKTRDY                                                (_ADI_MSK(0x00000001,uint16_t))  /* RXPKTRDY: Rx Packet in Endpoint FIFO */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_EP0_CSR_H                                                            Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_EP0_CSR_H_DISPING                                               11                               /* Disable Ping */
#define BITP_USB_EP0_CSR_H_DATGLEN                                               10                               /* Data Toggle Write Enable */
#define BITP_USB_EP0_CSR_H_DATGL                                                  9                               /* Data Toggle */
#define BITP_USB_EP0_CSR_H_FLUSHFIFO                                              8                               /* Flush Endpoint FIFO */
#define BITP_USB_EP0_CSR_H_NAKTO                                                  7                               /* NAK Timeout */
#define BITP_USB_EP0_CSR_H_STATUSPKT                                              6                               /* Status Packet */
#define BITP_USB_EP0_CSR_H_REQPKT                                                 5                               /* Request Packet */
#define BITP_USB_EP0_CSR_H_TOERR                                                  4                               /* Timeout Error */
#define BITP_USB_EP0_CSR_H_SETUPPKT                                               3                               /* Setup Packet */
#define BITP_USB_EP0_CSR_H_RXSTALL                                                2                               /* Rx Stall */
#define BITP_USB_EP0_CSR_H_TXPKTRDY                                               1                               /* Tx Packet Ready */
#define BITP_USB_EP0_CSR_H_RXPKTRDY                                               0                               /* Rx Packet Ready */

#define BITM_USB_EP0_CSR_H_DISPING                                               (_ADI_MSK(0x00000800,uint16_t))  /* Disable Ping */
#define ENUM_USB_EP0_CSR_H_NO_DISPING                                            (_ADI_MSK(0x00000000,uint16_t))  /* DISPING: Issue PING tokens */
#define ENUM_USB_EP0_CSR_H_DISPING                                               (_ADI_MSK(0x00000800,uint16_t))  /* DISPING: Do not issue PING */

#define BITM_USB_EP0_CSR_H_DATGLEN                                               (_ADI_MSK(0x00000400,uint16_t))  /* Data Toggle Write Enable */
#define ENUM_USB_EP0_CSR_H_NO_DATGLEN                                            (_ADI_MSK(0x00000000,uint16_t))  /* DATGLEN: Disable Write to DATGL */
#define ENUM_USB_EP0_CSR_H_DATGLEN                                               (_ADI_MSK(0x00000400,uint16_t))  /* DATGLEN: Enable Write to DATGL */

#define BITM_USB_EP0_CSR_H_DATGL                                                 (_ADI_MSK(0x00000200,uint16_t))  /* Data Toggle */
#define ENUM_USB_EP0_CSR_H_NO_DATATGL                                            (_ADI_MSK(0x00000000,uint16_t))  /* DATGL: DATA0 is Set */
#define ENUM_USB_EP0_CSR_H_DATATGL                                               (_ADI_MSK(0x00000200,uint16_t))  /* DATGL: DATA1 is Set */

#define BITM_USB_EP0_CSR_H_FLUSHFIFO                                             (_ADI_MSK(0x00000100,uint16_t))  /* Flush Endpoint FIFO */
#define ENUM_USB_EP0_CSR_H_NO_FLUSH                                              (_ADI_MSK(0x00000000,uint16_t))  /* FLUSHFIFO: No Flush */
#define ENUM_USB_EP0_CSR_H_FLUSH                                                 (_ADI_MSK(0x00000100,uint16_t))  /* FLUSHFIFO: Flush Endpoint FIFO */

#define BITM_USB_EP0_CSR_H_NAKTO                                                 (_ADI_MSK(0x00000080,uint16_t))  /* NAK Timeout */
#define ENUM_USB_EP0_CSR_H_NO_NAKTO                                              (_ADI_MSK(0x00000000,uint16_t))  /* NAKTO: No Status */
#define ENUM_USB_EP0_CSR_H_NAKTO                                                 (_ADI_MSK(0x00000080,uint16_t))  /* NAKTO: Endpoint Halted (NAK Timeout) */

#define BITM_USB_EP0_CSR_H_STATUSPKT                                             (_ADI_MSK(0x00000040,uint16_t))  /* Status Packet */
#define ENUM_USB_EP0_CSR_H_NO_STATPKT                                            (_ADI_MSK(0x00000000,uint16_t))  /* STATUSPKT: No Request */
#define ENUM_USB_EP0_CSR_H_STATPKT                                               (_ADI_MSK(0x00000040,uint16_t))  /* STATUSPKT: Request Status Transaction */

#define BITM_USB_EP0_CSR_H_REQPKT                                                (_ADI_MSK(0x00000020,uint16_t))  /* Request Packet */
#define ENUM_USB_EP0_CSR_H_NO_REQPKT                                             (_ADI_MSK(0x00000000,uint16_t))  /* REQPKT: No Request */
#define ENUM_USB_EP0_CSR_H_REQPKT                                                (_ADI_MSK(0x00000020,uint16_t))  /* REQPKT: Send IN Tokens to Device */

#define BITM_USB_EP0_CSR_H_TOERR                                                 (_ADI_MSK(0x00000010,uint16_t))  /* Timeout Error */
#define ENUM_USB_EP0_CSR_H_NO_TOERR                                              (_ADI_MSK(0x00000000,uint16_t))  /* TOERR: No Status */
#define ENUM_USB_EP0_CSR_H_TOERR                                                 (_ADI_MSK(0x00000010,uint16_t))  /* TOERR: Timeout Error */

#define BITM_USB_EP0_CSR_H_SETUPPKT                                              (_ADI_MSK(0x00000008,uint16_t))  /* Setup Packet */
#define ENUM_USB_EP0_CSR_H_NO_SETUPPKT                                           (_ADI_MSK(0x00000000,uint16_t))  /* SETUPPKT: No Request */
#define ENUM_USB_EP0_CSR_H_SETUPPKT                                              (_ADI_MSK(0x00000008,uint16_t))  /* SETUPPKT: Send SETUP token */

#define BITM_USB_EP0_CSR_H_RXSTALL                                               (_ADI_MSK(0x00000004,uint16_t))  /* Rx Stall */
#define ENUM_USB_EP0_CSR_H_NO_RXSTALL                                            (_ADI_MSK(0x00000000,uint16_t))  /* RXSTALL: No Status */
#define ENUM_USB_EP0_CSR_H_RXSTALL                                               (_ADI_MSK(0x00000004,uint16_t))  /* RXSTALL: Stall Received from Device */

#define BITM_USB_EP0_CSR_H_TXPKTRDY                                              (_ADI_MSK(0x00000002,uint16_t))  /* Tx Packet Ready */
#define ENUM_USB_EP0_CSR_H_NO_TXPKTRDY                                           (_ADI_MSK(0x00000000,uint16_t))  /* TXPKTRDY: No Tx Packet */
#define ENUM_USB_EP0_CSR_H_TXPKTRDY                                              (_ADI_MSK(0x00000002,uint16_t))  /* TXPKTRDY: Tx Packet in Endpoint FIFO */

#define BITM_USB_EP0_CSR_H_RXPKTRDY                                              (_ADI_MSK(0x00000001,uint16_t))  /* Rx Packet Ready */
#define ENUM_USB_EP0_CSR_H_NO_RXPKTRDY                                           (_ADI_MSK(0x00000000,uint16_t))  /* RXPKTRDY: No Rx Packet */
#define ENUM_USB_EP0_CSR_H_RXPKTRDY                                              (_ADI_MSK(0x00000001,uint16_t))  /* RXPKTRDY: Rx Packet in Endpoint FIFO */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_EP_TXCSR_H                                                           Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_EP_TXCSR_H_AUTOSET                                              15                               /* TxPkRdy Autoset Enable */
#define BITP_USB_EP_TXCSR_H_DMAREQEN                                             12                               /* DMA Request Enable Tx EP */
#define BITP_USB_EP_TXCSR_H_FRCDATATGL                                           11                               /* Force Data Toggle */
#define BITP_USB_EP_TXCSR_H_DMAREQMODE                                           10                               /* DMA Mode Select */
#define BITP_USB_EP_TXCSR_H_DATGLEN                                               9                               /* Data Toggle Write Enable */
#define BITP_USB_EP_TXCSR_H_DATGL                                                 8                               /* Data Toggle */
#define BITP_USB_EP_TXCSR_H_CLRDATATGL                                            6                               /* Clear Endpoint Data Toggle */
#define BITP_USB_EP_TXCSR_H_RXSTALL                                               5                               /* Rx STALL */
#define BITP_USB_EP_TXCSR_H_SETUPPKT                                              4                               /* Setup Packet */
#define BITP_USB_EP_TXCSR_H_FLUSHFIFO                                             3                               /* Flush Endpoint FIFO */
#define BITP_USB_EP_TXCSR_H_TXTOERR                                               2                               /* Tx Timeout Error */
#define BITP_USB_EP_TXCSR_H_NEFIFO                                                1                               /* Not Empty FIFO */
#define BITP_USB_EP_TXCSR_H_TXPKTRDY                                              0                               /* Tx Packet Ready */

#define BITM_USB_EP_TXCSR_H_AUTOSET                                              (_ADI_MSK(0x00008000,uint16_t))  /* TxPkRdy Autoset Enable */
#define ENUM_USB_EP_TXCSR_H_NO_AUTOSET                                           (_ADI_MSK(0x00000000,uint16_t))  /* AUTOSET: Disable Autoset */
#define ENUM_USB_EP_TXCSR_H_AUTOSET                                              (_ADI_MSK(0x00008000,uint16_t))  /* AUTOSET: Enable Autoset */

#define BITM_USB_EP_TXCSR_H_DMAREQEN                                             (_ADI_MSK(0x00001000,uint16_t))  /* DMA Request Enable Tx EP */
#define ENUM_USB_EP_TXCSR_H_DMAREQDIS                                            (_ADI_MSK(0x00000000,uint16_t))  /* DMAREQEN: Disable DMA Request */
#define ENUM_USB_EP_TXCSR_H_DMAREQEN                                             (_ADI_MSK(0x00001000,uint16_t))  /* DMAREQEN: Enable DMA Request */

#define BITM_USB_EP_TXCSR_H_FRCDATATGL                                           (_ADI_MSK(0x00000800,uint16_t))  /* Force Data Toggle */
#define ENUM_USB_EP_TXCSR_H_NO_FRCTGL                                            (_ADI_MSK(0x00000000,uint16_t))  /* FRCDATATGL: No Action */
#define ENUM_USB_EP_TXCSR_H_FRCTGL                                               (_ADI_MSK(0x00000800,uint16_t))  /* FRCDATATGL: Toggle Endpoint Data */

#define BITM_USB_EP_TXCSR_H_DMAREQMODE                                           (_ADI_MSK(0x00000400,uint16_t))  /* DMA Mode Select */
#define ENUM_USB_EP_TXCSR_H_DMARQMODE0                                           (_ADI_MSK(0x00000000,uint16_t))  /* DMAREQMODE: DMA Request Mode 0 */
#define ENUM_USB_EP_TXCSR_H_DMARQMODE1                                           (_ADI_MSK(0x00000400,uint16_t))  /* DMAREQMODE: DMA Request Mode 1 */

#define BITM_USB_EP_TXCSR_H_DATGLEN                                              (_ADI_MSK(0x00000200,uint16_t))  /* Data Toggle Write Enable */
#define ENUM_USB_EP_TXCSR_H_NO_DATGLEN                                           (_ADI_MSK(0x00000000,uint16_t))  /* DATGLEN: Disable Write to DATGL */
#define ENUM_USB_EP_TXCSR_H_DATGLEN                                              (_ADI_MSK(0x00000200,uint16_t))  /* DATGLEN: Enable Write to DATGL */

#define BITM_USB_EP_TXCSR_H_DATGL                                                (_ADI_MSK(0x00000100,uint16_t))  /* Data Toggle */
#define ENUM_USB_EP_TXCSR_H_NO_DATGL                                             (_ADI_MSK(0x00000000,uint16_t))  /* DATGL: DATA0 is set */
#define ENUM_USB_EP_TXCSR_H_DATGL                                                (_ADI_MSK(0x00000100,uint16_t))  /* DATGL: DATA1 is set */

#define BITM_USB_EP_TXCSR_H_CLRDATATGL                                           (_ADI_MSK(0x00000040,uint16_t))  /* Clear Endpoint Data Toggle */
#define ENUM_USB_EP_TXCSR_H_NO_CLRTGL                                            (_ADI_MSK(0x00000000,uint16_t))  /* CLRDATATGL: No Action */
#define ENUM_USB_EP_TXCSR_H_CLRTGL                                               (_ADI_MSK(0x00000040,uint16_t))  /* CLRDATATGL: Reset EP Data Toggle to 0 */

#define BITM_USB_EP_TXCSR_H_RXSTALL                                              (_ADI_MSK(0x00000020,uint16_t))  /* Rx STALL */
#define ENUM_USB_EP_TXCSR_H_NO_RXSTALL                                           (_ADI_MSK(0x00000000,uint16_t))  /* RXSTALL: No Status */
#define ENUM_USB_EP_TXCSR_H_RXSTALL                                              (_ADI_MSK(0x00000020,uint16_t))  /* RXSTALL: Stall Received from Device */

#define BITM_USB_EP_TXCSR_H_SETUPPKT                                             (_ADI_MSK(0x00000010,uint16_t))  /* Setup Packet */
#define ENUM_USB_EP_TXCSR_H_NO_SETUPPK                                           (_ADI_MSK(0x00000000,uint16_t))  /* SETUPPKT: No Request */
#define ENUM_USB_EP_TXCSR_H_SETUPPKT                                             (_ADI_MSK(0x00000010,uint16_t))  /* SETUPPKT: Send SETUP Token */

#define BITM_USB_EP_TXCSR_H_FLUSHFIFO                                            (_ADI_MSK(0x00000008,uint16_t))  /* Flush Endpoint FIFO */
#define ENUM_USB_EP_TXCSR_H_NO_FLUSH                                             (_ADI_MSK(0x00000000,uint16_t))  /* FLUSHFIFO: No Flush */
#define ENUM_USB_EP_TXCSR_H_FLUSH                                                (_ADI_MSK(0x00000008,uint16_t))  /* FLUSHFIFO: Flush endpoint FIFO */

#define BITM_USB_EP_TXCSR_H_TXTOERR                                              (_ADI_MSK(0x00000004,uint16_t))  /* Tx Timeout Error */
#define ENUM_USB_EP_TXCSR_H_NO_TXTOERR                                           (_ADI_MSK(0x00000000,uint16_t))  /* TXTOERR: No Status */
#define ENUM_USB_EP_TXCSR_H_TXTOERR                                              (_ADI_MSK(0x00000004,uint16_t))  /* TXTOERR: Tx Timeout Error */

#define BITM_USB_EP_TXCSR_H_NEFIFO                                               (_ADI_MSK(0x00000002,uint16_t))  /* Not Empty FIFO */
#define ENUM_USB_EP_TXCSR_H_NO_NEFIFO                                            (_ADI_MSK(0x00000000,uint16_t))  /* NEFIFO: FIFO Empty */
#define ENUM_USB_EP_TXCSR_H_NEFIFO                                               (_ADI_MSK(0x00000002,uint16_t))  /* NEFIFO: FIFO Not Empty */

#define BITM_USB_EP_TXCSR_H_TXPKTRDY                                             (_ADI_MSK(0x00000001,uint16_t))  /* Tx Packet Ready */
#define ENUM_USB_EP_TXCSR_H_NO_PKTRDY                                            (_ADI_MSK(0x00000000,uint16_t))  /* TXPKTRDY: No Tx Packet */
#define ENUM_USB_EP_TXCSR_H_PKTRDY                                               (_ADI_MSK(0x00000001,uint16_t))  /* TXPKTRDY: Tx Packet in Endpoint FIFO */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_EP_RXMAXP                                                            Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_EP_RXMAXP_MAXPAY                                                 0                               /* Maximum Payload */
#define BITM_USB_EP_RXMAXP_MAXPAY                                                (_ADI_MSK(0x000007FF,uint16_t))  /* Maximum Payload */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_EP_RXCSR_P                                                           Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_EP_RXCSR_P_AUTOCLR                                              15                               /* Auto Clear Enable */
#define BITP_USB_EP_RXCSR_P_ISO                                                  14                               /* Isochronous Transfers */
#define BITP_USB_EP_RXCSR_P_DMAREQEN                                             13                               /* DMA Request Enable Rx EP */
#define BITP_USB_EP_RXCSR_P_DNYETPERR                                            12                               /* Disable NYET Handshake */
#define BITP_USB_EP_RXCSR_P_DMAREQMODE                                           11                               /* DMA Mode Select */
#define BITP_USB_EP_RXCSR_P_CLRDATATGL                                            7                               /* Clear Endpoint Data Toggle */
#define BITP_USB_EP_RXCSR_P_SENTSTALL                                             6                               /* Sent STALL */
#define BITP_USB_EP_RXCSR_P_SENDSTALL                                             5                               /* Send STALL */
#define BITP_USB_EP_RXCSR_P_FLUSHFIFO                                             4                               /* Flush Endpoint FIFO */
#define BITP_USB_EP_RXCSR_P_DATAERR                                               3                               /* Data Error */
#define BITP_USB_EP_RXCSR_P_ORUNERR                                               2                               /* OUT Run Error */
#define BITP_USB_EP_RXCSR_P_FIFOFULL                                              1                               /* FIFO Full */
#define BITP_USB_EP_RXCSR_P_RXPKTRDY                                              0                               /* Rx Packet Ready */

#define BITM_USB_EP_RXCSR_P_AUTOCLR                                              (_ADI_MSK(0x00008000,uint16_t))  /* Auto Clear Enable */
#define ENUM_USB_EP_RXCSR_P_NO_AUTOCLR                                           (_ADI_MSK(0x00000000,uint16_t))  /* AUTOCLR: Disable Auto Clear */
#define ENUM_USB_EP_RXCSR_P_AUTOCLR                                              (_ADI_MSK(0x00008000,uint16_t))  /* AUTOCLR: Enable Auto Clear */

#define BITM_USB_EP_RXCSR_P_ISO                                                  (_ADI_MSK(0x00004000,uint16_t))  /* Isochronous Transfers */
#define ENUM_USB_EP_RXCSR_P_ISODIS                                               (_ADI_MSK(0x00000000,uint16_t))  /* ISO: This bit should be cleared for bulk or interrupt transfers. */
#define ENUM_USB_EP_RXCSR_P_ISOEN                                                (_ADI_MSK(0x00004000,uint16_t))  /* ISO: This bit should be set for isochronous transfers. */

#define BITM_USB_EP_RXCSR_P_DMAREQEN                                             (_ADI_MSK(0x00002000,uint16_t))  /* DMA Request Enable Rx EP */
#define ENUM_USB_EP_RXCSR_P_DMAREQDIS                                            (_ADI_MSK(0x00000000,uint16_t))  /* DMAREQEN: Disable DMA Request */
#define ENUM_USB_EP_RXCSR_P_DMAREQEN                                             (_ADI_MSK(0x00002000,uint16_t))  /* DMAREQEN: Enable DMA Request */

#define BITM_USB_EP_RXCSR_P_DNYETPERR                                            (_ADI_MSK(0x00001000,uint16_t))  /* Disable NYET Handshake */
#define ENUM_USB_EP_RXCSR_P_DNYTERREN                                            (_ADI_MSK(0x00000000,uint16_t))  /* DNYETPERR: Enable NYET Handshake */
#define ENUM_USB_EP_RXCSR_P_DNYTERRDIS                                           (_ADI_MSK(0x00001000,uint16_t))  /* DNYETPERR: Disable NYET Handshake */

#define BITM_USB_EP_RXCSR_P_DMAREQMODE                                           (_ADI_MSK(0x00000800,uint16_t))  /* DMA Mode Select */
#define ENUM_USB_EP_RXCSR_P_DMARQMODE0                                           (_ADI_MSK(0x00000000,uint16_t))  /* DMAREQMODE: DMA Request Mode 0 */
#define ENUM_USB_EP_RXCSR_P_DMARQMODE1                                           (_ADI_MSK(0x00000800,uint16_t))  /* DMAREQMODE: DMA Request Mode 1 */

#define BITM_USB_EP_RXCSR_P_CLRDATATGL                                           (_ADI_MSK(0x00000080,uint16_t))  /* Clear Endpoint Data Toggle */
#define ENUM_USB_EP_RXCSR_P_NO_CLRTGL                                            (_ADI_MSK(0x00000000,uint16_t))  /* CLRDATATGL: No Action */
#define ENUM_USB_EP_RXCSR_P_CLRTGL                                               (_ADI_MSK(0x00000080,uint16_t))  /* CLRDATATGL: Reset EP Data Toggle to 0 */

#define BITM_USB_EP_RXCSR_P_SENTSTALL                                            (_ADI_MSK(0x00000040,uint16_t))  /* Sent STALL */
#define ENUM_USB_EP_RXCSR_P_NO_STALSNT                                           (_ADI_MSK(0x00000000,uint16_t))  /* SENTSTALL: No Status */
#define ENUM_USB_EP_RXCSR_P_STALSNT                                              (_ADI_MSK(0x00000040,uint16_t))  /* SENTSTALL: STALL Handshake Transmitted */

#define BITM_USB_EP_RXCSR_P_SENDSTALL                                            (_ADI_MSK(0x00000020,uint16_t))  /* Send STALL */
#define ENUM_USB_EP_RXCSR_P_NO_STALL                                             (_ADI_MSK(0x00000000,uint16_t))  /* SENDSTALL: No Action */
#define ENUM_USB_EP_RXCSR_P_STALL                                                (_ADI_MSK(0x00000020,uint16_t))  /* SENDSTALL: Request STALL Handshake */

#define BITM_USB_EP_RXCSR_P_FLUSHFIFO                                            (_ADI_MSK(0x00000010,uint16_t))  /* Flush Endpoint FIFO */
#define ENUM_USB_EP_RXCSR_P_NO_FLUSH                                             (_ADI_MSK(0x00000000,uint16_t))  /* FLUSHFIFO: No Flush */
#define ENUM_USB_EP_RXCSR_P_FLUSH                                                (_ADI_MSK(0x00000010,uint16_t))  /* FLUSHFIFO: Flush Endpoint FIFO */

#define BITM_USB_EP_RXCSR_P_DATAERR                                              (_ADI_MSK(0x00000008,uint16_t))  /* Data Error */
#define ENUM_USB_EP_RXCSR_P_NO_DATAERR                                           (_ADI_MSK(0x00000000,uint16_t))  /* DATAERR: No Status */
#define ENUM_USB_EP_RXCSR_P_DATAERR                                              (_ADI_MSK(0x00000008,uint16_t))  /* DATAERR: Data Error */

#define BITM_USB_EP_RXCSR_P_ORUNERR                                              (_ADI_MSK(0x00000004,uint16_t))  /* OUT Run Error */
#define ENUM_USB_EP_RXCSR_P_NO_ORUNERR                                           (_ADI_MSK(0x00000000,uint16_t))  /* ORUNERR: No Status */
#define ENUM_USB_EP_RXCSR_P_ORUNERR                                              (_ADI_MSK(0x00000004,uint16_t))  /* ORUNERR: OUT Run Error */

#define BITM_USB_EP_RXCSR_P_FIFOFULL                                             (_ADI_MSK(0x00000002,uint16_t))  /* FIFO Full */
#define ENUM_USB_EP_RXCSR_P_NO_FIFOFUL                                           (_ADI_MSK(0x00000000,uint16_t))  /* FIFOFULL: No Status */
#define ENUM_USB_EP_RXCSR_P_FIFOFUL                                              (_ADI_MSK(0x00000002,uint16_t))  /* FIFOFULL: FIFO Full */

#define BITM_USB_EP_RXCSR_P_RXPKTRDY                                             (_ADI_MSK(0x00000001,uint16_t))  /* Rx Packet Ready */
#define ENUM_USB_EP_RXCSR_P_NO_PKTRDY                                            (_ADI_MSK(0x00000000,uint16_t))  /* RXPKTRDY: No Rx Packet */
#define ENUM_USB_EP_RXCSR_P_PKTRDY                                               (_ADI_MSK(0x00000001,uint16_t))  /* RXPKTRDY: Rx Packet in Endpoint FIFO */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_EP_RXCSR_H                                                           Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_EP_RXCSR_H_AUTOCLR                                              15                               /* Auto Clear Enable */
#define BITP_USB_EP_RXCSR_H_AUTOREQ                                              14                               /* Auto Request Clear Enable */
#define BITP_USB_EP_RXCSR_H_DMAREQEN                                             13                               /* DMA Request Enable Rx EP */
#define BITP_USB_EP_RXCSR_H_PIDERR                                               12                               /* Packet ID Error */
#define BITP_USB_EP_RXCSR_H_DMAREQMODE                                           11                               /* DMA Mode Select */
#define BITP_USB_EP_RXCSR_H_DATGLEN                                              10                               /* Data Toggle Write Enable */
#define BITP_USB_EP_RXCSR_H_DATGL                                                 9                               /* Data Toggle */
#define BITP_USB_EP_RXCSR_H_CLRDATATGL                                            7                               /* Clear Endpoint Data Toggle */
#define BITP_USB_EP_RXCSR_H_RXSTALL                                               6                               /* Rx STALL */
#define BITP_USB_EP_RXCSR_H_REQPKT                                                5                               /* Request Packet */
#define BITP_USB_EP_RXCSR_H_FLUSHFIFO                                             4                               /* Flush Endpoint FIFO */
#define BITP_USB_EP_RXCSR_H_NAKTODERR                                             3                               /* NAK Timeout Data Error */
#define BITP_USB_EP_RXCSR_H_RXTOERR                                               2                               /* Rx Timeout Error */
#define BITP_USB_EP_RXCSR_H_FIFOFULL                                              1                               /* FIFO Full */
#define BITP_USB_EP_RXCSR_H_RXPKTRDY                                              0                               /* Rx Packet Ready */

#define BITM_USB_EP_RXCSR_H_AUTOCLR                                              (_ADI_MSK(0x00008000,uint16_t))  /* Auto Clear Enable */
#define ENUM_USB_EP_RXCSR_H_NO_AUTOCLR                                           (_ADI_MSK(0x00000000,uint16_t))  /* AUTOCLR: Disable Auto Clear */
#define ENUM_USB_EP_RXCSR_H_AUTOCLR                                              (_ADI_MSK(0x00008000,uint16_t))  /* AUTOCLR: Enable Auto Clear */

#define BITM_USB_EP_RXCSR_H_AUTOREQ                                              (_ADI_MSK(0x00004000,uint16_t))  /* Auto Request Clear Enable */
#define ENUM_USB_EP_RXCSR_H_NO_AUTOREQ                                           (_ADI_MSK(0x00000000,uint16_t))  /* AUTOREQ: Disable Auto Request Clear */
#define ENUM_USB_EP_RXCSR_H_AUTOREQ                                              (_ADI_MSK(0x00004000,uint16_t))  /* AUTOREQ: Enable Auto Request Clear */

#define BITM_USB_EP_RXCSR_H_DMAREQEN                                             (_ADI_MSK(0x00002000,uint16_t))  /* DMA Request Enable Rx EP */
#define ENUM_USB_EP_RXCSR_H_DMAREQDIS                                            (_ADI_MSK(0x00000000,uint16_t))  /* DMAREQEN: Disable DMA Request */
#define ENUM_USB_EP_RXCSR_H_DMAREQEN                                             (_ADI_MSK(0x00002000,uint16_t))  /* DMAREQEN: Enable DMA Request */

#define BITM_USB_EP_RXCSR_H_PIDERR                                               (_ADI_MSK(0x00001000,uint16_t))  /* Packet ID Error */
#define ENUM_USB_EP_RXCSR_H_NO_PIDERR                                            (_ADI_MSK(0x00000000,uint16_t))  /* PIDERR: No Status */
#define ENUM_USB_EP_RXCSR_H_PIDERR                                               (_ADI_MSK(0x00001000,uint16_t))  /* PIDERR: PID Error */

#define BITM_USB_EP_RXCSR_H_DMAREQMODE                                           (_ADI_MSK(0x00000800,uint16_t))  /* DMA Mode Select */
#define ENUM_USB_EP_RXCSR_H_DMARQMODE0                                           (_ADI_MSK(0x00000000,uint16_t))  /* DMAREQMODE: DMA Request Mode 0 */
#define ENUM_USB_EP_RXCSR_H_DMARQMODE1                                           (_ADI_MSK(0x00000800,uint16_t))  /* DMAREQMODE: DMA Request Mode 1 */

#define BITM_USB_EP_RXCSR_H_DATGLEN                                              (_ADI_MSK(0x00000400,uint16_t))  /* Data Toggle Write Enable */
#define ENUM_USB_EP_RXCSR_H_DATGLDIS                                             (_ADI_MSK(0x00000000,uint16_t))  /* DATGLEN: Disable Write to DATGL */
#define ENUM_USB_EP_RXCSR_H_DATGLEN                                              (_ADI_MSK(0x00000400,uint16_t))  /* DATGLEN: Enable Write to DATGL */

#define BITM_USB_EP_RXCSR_H_DATGL                                                (_ADI_MSK(0x00000200,uint16_t))  /* Data Toggle */
#define ENUM_USB_EP_RXCSR_H_NO_DATGL                                             (_ADI_MSK(0x00000000,uint16_t))  /* DATGL: DATA0 is Set */
#define ENUM_USB_EP_RXCSR_H_DATGL                                                (_ADI_MSK(0x00000200,uint16_t))  /* DATGL: DATA1 is Set */

#define BITM_USB_EP_RXCSR_H_CLRDATATGL                                           (_ADI_MSK(0x00000080,uint16_t))  /* Clear Endpoint Data Toggle */
#define ENUM_USB_EP_RXCSR_H_NO_CLRTGL                                            (_ADI_MSK(0x00000000,uint16_t))  /* CLRDATATGL: No Action */
#define ENUM_USB_EP_RXCSR_H_CLRTGL                                               (_ADI_MSK(0x00000080,uint16_t))  /* CLRDATATGL: Reset EP Data Toggle to 0 */

#define BITM_USB_EP_RXCSR_H_RXSTALL                                              (_ADI_MSK(0x00000040,uint16_t))  /* Rx STALL */
#define ENUM_USB_EP_RXCSR_H_NO_RXSTALL                                           (_ADI_MSK(0x00000000,uint16_t))  /* RXSTALL: No Status */
#define ENUM_USB_EP_RXCSR_H_RXSTALL                                              (_ADI_MSK(0x00000040,uint16_t))  /* RXSTALL: Stall Received from Device */

#define BITM_USB_EP_RXCSR_H_REQPKT                                               (_ADI_MSK(0x00000020,uint16_t))  /* Request Packet */
#define ENUM_USB_EP_RXCSR_H_NO_REQPKT                                            (_ADI_MSK(0x00000000,uint16_t))  /* REQPKT: No Request */
#define ENUM_USB_EP_RXCSR_H_REQPKT                                               (_ADI_MSK(0x00000020,uint16_t))  /* REQPKT: Send IN Tokens to Device */

#define BITM_USB_EP_RXCSR_H_FLUSHFIFO                                            (_ADI_MSK(0x00000010,uint16_t))  /* Flush Endpoint FIFO */
#define ENUM_USB_EP_RXCSR_H_NO_FLUSH                                             (_ADI_MSK(0x00000000,uint16_t))  /* FLUSHFIFO: No Flush */
#define ENUM_USB_EP_RXCSR_H_FLUSH                                                (_ADI_MSK(0x00000010,uint16_t))  /* FLUSHFIFO: Flush Endpoint FIFO */

#define BITM_USB_EP_RXCSR_H_NAKTODERR                                            (_ADI_MSK(0x00000008,uint16_t))  /* NAK Timeout Data Error */
#define ENUM_USB_EP_RXCSR_H_NO_NAKTO                                             (_ADI_MSK(0x00000000,uint16_t))  /* NAKTODERR: No Status */
#define ENUM_USB_EP_RXCSR_H_NAKTO                                                (_ADI_MSK(0x00000008,uint16_t))  /* NAKTODERR: NAK Timeout Data Error */

#define BITM_USB_EP_RXCSR_H_RXTOERR                                              (_ADI_MSK(0x00000004,uint16_t))  /* Rx Timeout Error */
#define ENUM_USB_EP_RXCSR_H_NO_RXTOERR                                           (_ADI_MSK(0x00000000,uint16_t))  /* RXTOERR: No Status */
#define ENUM_USB_EP_RXCSR_H_RXTOERR                                              (_ADI_MSK(0x00000004,uint16_t))  /* RXTOERR: Rx Timeout Error */

#define BITM_USB_EP_RXCSR_H_FIFOFULL                                             (_ADI_MSK(0x00000002,uint16_t))  /* FIFO Full */
#define ENUM_USB_EP_RXCSR_H_NO_FIFOFUL                                           (_ADI_MSK(0x00000000,uint16_t))  /* FIFOFULL: No Status */
#define ENUM_USB_EP_RXCSR_H_FIFOFUL                                              (_ADI_MSK(0x00000002,uint16_t))  /* FIFOFULL: FIFO Full */

#define BITM_USB_EP_RXCSR_H_RXPKTRDY                                             (_ADI_MSK(0x00000001,uint16_t))  /* Rx Packet Ready */
#define ENUM_USB_EP_RXCSR_H_NO_PKTRDY                                            (_ADI_MSK(0x00000000,uint16_t))  /* RXPKTRDY: No Rx Packet */
#define ENUM_USB_EP_RXCSR_H_PKTRDY                                               (_ADI_MSK(0x00000001,uint16_t))  /* RXPKTRDY: Rx Packet in Endpoint FIFO */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_EP0_CNT                                                              Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_EP0_CNT_RXCNT                                                    0                               /* Rx Byte Count Value */
#define BITM_USB_EP0_CNT_RXCNT                                                   (_ADI_MSK(0x0000007F,uint16_t))  /* Rx Byte Count Value */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_EP_RXCNT                                                             Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_EP_RXCNT_EPRXCNT                                                 0                               /* EP Rx Count */
#define BITM_USB_EP_RXCNT_EPRXCNT                                                (_ADI_MSK(0x00003FFF,uint16_t))  /* EP Rx Count */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_EP_FIFOSIZE                                                          Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_EP_FIFOSIZE_RXFIFOSIZE                                           4
#define BITP_USB_EP_FIFOSIZE_TXFIFOSIZE                                           0
#define BITM_USB_EP_FIFOSIZE_RXFIFOSIZE                                          (_ADI_MSK(0x000000F0,uint8_t))
#define BITM_USB_EP_FIFOSIZE_TXFIFOSIZE                                          (_ADI_MSK(0x0000000F,uint8_t))

/* ------------------------------------------------------------------------------------------------------------------------
        USB_EP0_CFGDATA                                                          Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_EP0_CFGDATA_BIGEND                                               5                               /* Big Endian Data */
#define BITP_USB_EP0_CFGDATA_DYNFIFO                                              2                               /* Dynamic FIFO Size Enable */
#define BITP_USB_EP0_CFGDATA_SOFTCON                                              1                               /* Soft Connect Enable */
#define BITP_USB_EP0_CFGDATA_UTMIWID                                              0                               /* UTMI Data Width */

#define BITM_USB_EP0_CFGDATA_BIGEND                                              (_ADI_MSK(0x00000020,uint8_t))   /* Big Endian Data */
#define ENUM_USB_EP0_CFGDATA_BIGENDDIS                                           (_ADI_MSK(0x00000000,uint8_t))   /* BIGEND: Little Endian Configuration */
#define ENUM_USB_EP0_CFGDATA_BIGENDEN                                            (_ADI_MSK(0x00000020,uint8_t))   /* BIGEND: Big Endian Configuration */

#define BITM_USB_EP0_CFGDATA_DYNFIFO                                             (_ADI_MSK(0x00000004,uint8_t))   /* Dynamic FIFO Size Enable */
#define ENUM_USB_EP0_CFGDATA_DYNSZDIS                                            (_ADI_MSK(0x00000000,uint8_t))   /* DYNFIFO: No Dynamic FIFO Size */
#define ENUM_USB_EP0_CFGDATA_DYNSZEN                                             (_ADI_MSK(0x00000004,uint8_t))   /* DYNFIFO: Dynamic FIFO Size */

#define BITM_USB_EP0_CFGDATA_SOFTCON                                             (_ADI_MSK(0x00000002,uint8_t))   /* Soft Connect Enable */
#define ENUM_USB_EP0_CFGDATA_SFTCONDIS                                           (_ADI_MSK(0x00000000,uint8_t))   /* SOFTCON: No Soft Connect */
#define ENUM_USB_EP0_CFGDATA_SFTCONEN                                            (_ADI_MSK(0x00000002,uint8_t))   /* SOFTCON: Soft Connect */

#define BITM_USB_EP0_CFGDATA_UTMIWID                                             (_ADI_MSK(0x00000001,uint8_t))   /* UTMI Data Width */
#define ENUM_USB_EP0_CFGDATA_UTMIWID8                                            (_ADI_MSK(0x00000000,uint8_t))   /* UTMIWID: 8-bit UTMI Data Width */
#define ENUM_USB_EP0_CFGDATA_UTMIWID16                                           (_ADI_MSK(0x00000001,uint8_t))   /* UTMIWID: 16-bit UTMI Data Width */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_DMA_IRQ                                                              Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_DMA_IRQ_D1                                                       1                               /* DMA 1 Interrupt Pending Status */
#define BITP_USB_DMA_IRQ_D0                                                       0                               /* DMA 0 Interrupt Pending Status */
#define BITM_USB_DMA_IRQ_D1                                                      (_ADI_MSK(0x00000002,uint8_t))   /* DMA 1 Interrupt Pending Status */
#define BITM_USB_DMA_IRQ_D0                                                      (_ADI_MSK(0x00000001,uint8_t))   /* DMA 0 Interrupt Pending Status */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_DMA_CTL                                                              Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_DMA_CTL_BRSTM                                                    9                               /* Burst Mode */
#define BITP_USB_DMA_CTL_ERR                                                      8                               /* Bus Error */
#define BITP_USB_DMA_CTL_EP                                                       4                               /* DMA Channel Endpoint Assignment */
#define BITP_USB_DMA_CTL_IE                                                       3                               /* DMA Interrupt Enable */
#define BITP_USB_DMA_CTL_MODE                                                     2                               /* DMA Mode */
#define BITP_USB_DMA_CTL_DIR                                                      1                               /* DMA Transfer Direction */
#define BITP_USB_DMA_CTL_EN                                                       0                               /* DMA Enable */

#define BITM_USB_DMA_CTL_BRSTM                                                   (_ADI_MSK(0x00000600,uint16_t))  /* Burst Mode */
#define ENUM_USB_DMA_CTL_BRSTM00                                                 (_ADI_MSK(0x00000000,uint16_t))  /* BRSTM: Unspecified Length */
#define ENUM_USB_DMA_CTL_BRSTM01                                                 (_ADI_MSK(0x00000200,uint16_t))  /* BRSTM: INCR4 or Unspecified Length */
#define ENUM_USB_DMA_CTL_BRSTM10                                                 (_ADI_MSK(0x00000400,uint16_t))  /* BRSTM: INCR8, INCR4, or Unspecified Length */
#define ENUM_USB_DMA_CTL_BRSTM11                                                 (_ADI_MSK(0x00000600,uint16_t))  /* BRSTM: INCR16, INCR8, INCR4, or Unspecified Length */

#define BITM_USB_DMA_CTL_ERR                                                     (_ADI_MSK(0x00000100,uint16_t))  /* Bus Error */
#define ENUM_USB_DMA_CTL_NO_DMAERR                                               (_ADI_MSK(0x00000000,uint16_t))  /* ERR: No Status */
#define ENUM_USB_DMA_CTL_DMAERR                                                  (_ADI_MSK(0x00000100,uint16_t))  /* ERR: Bus Error */

#define BITM_USB_DMA_CTL_EP                                                      (_ADI_MSK(0x000000F0,uint16_t))  /* DMA Channel Endpoint Assignment */
#define ENUM_USB_DMA_CTL_DMAEP0                                                  (_ADI_MSK(0x00000000,uint16_t))  /* EP: Endpoint 0 */
#define ENUM_USB_DMA_CTL_DMAEP1                                                  (_ADI_MSK(0x00000010,uint16_t))  /* EP: Endpoint 1 */
#define ENUM_USB_DMA_CTL_DMAEP10                                                 (_ADI_MSK(0x000000A0,uint16_t))  /* EP: Endpoint 10 */
#define ENUM_USB_DMA_CTL_DMAEP11                                                 (_ADI_MSK(0x000000B0,uint16_t))  /* EP: Endpoint 11 */
#define ENUM_USB_DMA_CTL_DMAEP12                                                 (_ADI_MSK(0x000000C0,uint16_t))  /* EP: Endpoint 12 */
#define ENUM_USB_DMA_CTL_DMAEP13                                                 (_ADI_MSK(0x000000D0,uint16_t))  /* EP: Endpoint 13 */
#define ENUM_USB_DMA_CTL_DMAEP14                                                 (_ADI_MSK(0x000000E0,uint16_t))  /* EP: Endpoint 14 */
#define ENUM_USB_DMA_CTL_DMAEP15                                                 (_ADI_MSK(0x000000F0,uint16_t))  /* EP: Endpoint 15 */
#define ENUM_USB_DMA_CTL_DMAEP2                                                  (_ADI_MSK(0x00000020,uint16_t))  /* EP: Endpoint 2 */
#define ENUM_USB_DMA_CTL_DMAEP3                                                  (_ADI_MSK(0x00000030,uint16_t))  /* EP: Endpoint 3 */
#define ENUM_USB_DMA_CTL_DMAEP4                                                  (_ADI_MSK(0x00000040,uint16_t))  /* EP: Endpoint 4 */
#define ENUM_USB_DMA_CTL_DMAEP5                                                  (_ADI_MSK(0x00000050,uint16_t))  /* EP: Endpoint 5 */
#define ENUM_USB_DMA_CTL_DMAEP6                                                  (_ADI_MSK(0x00000060,uint16_t))  /* EP: Endpoint 6 */
#define ENUM_USB_DMA_CTL_DMAEP7                                                  (_ADI_MSK(0x00000070,uint16_t))  /* EP: Endpoint 7 */
#define ENUM_USB_DMA_CTL_DMAEP8                                                  (_ADI_MSK(0x00000080,uint16_t))  /* EP: Endpoint 8 */
#define ENUM_USB_DMA_CTL_DMAEP9                                                  (_ADI_MSK(0x00000090,uint16_t))  /* EP: Endpoint 9 */

#define BITM_USB_DMA_CTL_IE                                                      (_ADI_MSK(0x00000008,uint16_t))  /* DMA Interrupt Enable */
#define ENUM_USB_DMA_CTL_DMAINTDIS                                               (_ADI_MSK(0x00000000,uint16_t))  /* IE: Disable Interrupt */
#define ENUM_USB_DMA_CTL_DMAINTEN                                                (_ADI_MSK(0x00000008,uint16_t))  /* IE: Enable Interrupt */

#define BITM_USB_DMA_CTL_MODE                                                    (_ADI_MSK(0x00000004,uint16_t))  /* DMA Mode */
#define ENUM_USB_DMA_CTL_DMAMODE0                                                (_ADI_MSK(0x00000000,uint16_t))  /* MODE: DMA Mode 0 */
#define ENUM_USB_DMA_CTL_DMAMODE1                                                (_ADI_MSK(0x00000004,uint16_t))  /* MODE: DMA Mode 1 */

#define BITM_USB_DMA_CTL_DIR                                                     (_ADI_MSK(0x00000002,uint16_t))  /* DMA Transfer Direction */
#define ENUM_USB_DMA_CTL_DMADIR_RX                                               (_ADI_MSK(0x00000000,uint16_t))  /* DIR: DMA Write (for Rx Endpoint) */
#define ENUM_USB_DMA_CTL_DMADIR_TX                                               (_ADI_MSK(0x00000002,uint16_t))  /* DIR: DMA Read (for Tx Endpoint) */

#define BITM_USB_DMA_CTL_EN                                                      (_ADI_MSK(0x00000001,uint16_t))  /* DMA Enable */
#define ENUM_USB_DMA_CTL_DMADIS                                                  (_ADI_MSK(0x00000000,uint16_t))  /* EN: Disable DMA */
#define ENUM_USB_DMA_CTL_DMAEN                                                   (_ADI_MSK(0x00000001,uint16_t))  /* EN: Enable DMA (Start Transfer) */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_RXDPKTBUFDIS                                                         Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_RXDPKTBUFDIS_EP3                                                 3                               /* Disable RX Double Buffer of Endpoint 3 */
#define BITP_USB_RXDPKTBUFDIS_EP2                                                 2                               /* Disable RX Double Buffer of Endpoint 2 */
#define BITP_USB_RXDPKTBUFDIS_EP1                                                 1                               /* Disable RX Double Buffer of Endpoint 1 */
#define BITM_USB_RXDPKTBUFDIS_EP3                                                (_ADI_MSK(0x00000008,uint16_t))  /* Disable RX Double Buffer of Endpoint 3 */
#define BITM_USB_RXDPKTBUFDIS_EP2                                                (_ADI_MSK(0x00000004,uint16_t))  /* Disable RX Double Buffer of Endpoint 2 */
#define BITM_USB_RXDPKTBUFDIS_EP1                                                (_ADI_MSK(0x00000002,uint16_t))  /* Disable RX Double Buffer of Endpoint 1 */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_TXDPKTBUFDIS                                                         Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_TXDPKTBUFDIS_EP3                                                 3                               /* Disable TX Double Buffer of Endpoint 3 */
#define BITP_USB_TXDPKTBUFDIS_EP2                                                 2                               /* Disable TX Double Buffer of Endpoint 2 */
#define BITP_USB_TXDPKTBUFDIS_EP1                                                 1                               /* Disable TX Double Buffer of Endpoint 1 */
#define BITM_USB_TXDPKTBUFDIS_EP3                                                (_ADI_MSK(0x00000008,uint16_t))  /* Disable TX Double Buffer of Endpoint 3 */
#define BITM_USB_TXDPKTBUFDIS_EP2                                                (_ADI_MSK(0x00000004,uint16_t))  /* Disable TX Double Buffer of Endpoint 2 */
#define BITM_USB_TXDPKTBUFDIS_EP1                                                (_ADI_MSK(0x00000002,uint16_t))  /* Disable TX Double Buffer of Endpoint 1 */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_LPM_ATTR                                                             Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_LPM_ATTR_EP                                                     12                               /* Endpoint */
#define BITP_USB_LPM_ATTR_RMTWAK                                                  8                               /* Remote Wakeup Enable */
#define BITP_USB_LPM_ATTR_HIRD                                                    4                               /* Host Initiated Resume Duration */
#define BITP_USB_LPM_ATTR_LINKSTATE                                               0                               /* Link State */
#define BITM_USB_LPM_ATTR_EP                                                     (_ADI_MSK(0x0000F000,uint16_t))  /* Endpoint */

#define BITM_USB_LPM_ATTR_RMTWAK                                                 (_ADI_MSK(0x00000100,uint16_t))  /* Remote Wakeup Enable */
#define ENUM_USB_LPM_ATTR_RMTWAKDIS                                              (_ADI_MSK(0x00000000,uint16_t))  /* RMTWAK: Disable Remote Wakeup */
#define ENUM_USB_LPM_ATTR_RMTWAKEN                                               (_ADI_MSK(0x00000100,uint16_t))  /* RMTWAK: Enable Remote Wakeup */
#define BITM_USB_LPM_ATTR_HIRD                                                   (_ADI_MSK(0x000000F0,uint16_t))  /* Host Initiated Resume Duration */

#define BITM_USB_LPM_ATTR_LINKSTATE                                              (_ADI_MSK(0x0000000F,uint16_t))  /* Link State */
#define ENUM_USB_LPM_ATTR_LNKSTATE_SSL1                                          (_ADI_MSK(0x00000001,uint16_t))  /* LINKSTATE: Sleep State (L1) */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_LPM_CTL                                                              Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_LPM_CTL_EN                                                       2                               /* LPM Enable */
#define BITP_USB_LPM_CTL_RESUME                                                   1                               /* LPM Resume (Remote Wakeup) */
#define BITP_USB_LPM_CTL_TX                                                       0                               /* LPM Transmit */
#define BITM_USB_LPM_CTL_EN                                                      (_ADI_MSK(0x0000000C,uint8_t))   /* LPM Enable */
#define BITM_USB_LPM_CTL_RESUME                                                  (_ADI_MSK(0x00000002,uint8_t))   /* LPM Resume (Remote Wakeup) */
#define BITM_USB_LPM_CTL_TX                                                      (_ADI_MSK(0x00000001,uint8_t))   /* LPM Transmit */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_LPM_IEN                                                              Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_LPM_IEN_LPMERR                                                   5                               /* LPM Error Interrupt Enable */
#define BITP_USB_LPM_IEN_LPMRES                                                   4                               /* LPM Resume Interrupt Enable */
#define BITP_USB_LPM_IEN_LPMNC                                                    3                               /* LPM NYET Control Interrupt Enable */
#define BITP_USB_LPM_IEN_LPMACK                                                   2                               /* LPM ACK Interrupt Enable */
#define BITP_USB_LPM_IEN_LPMNY                                                    1                               /* LPM NYET Interrupt Enable */
#define BITP_USB_LPM_IEN_LPMST                                                    0                               /* LPM STALL Interrupt Enable */
#define BITM_USB_LPM_IEN_LPMERR                                                  (_ADI_MSK(0x00000020,uint8_t))   /* LPM Error Interrupt Enable */
#define BITM_USB_LPM_IEN_LPMRES                                                  (_ADI_MSK(0x00000010,uint8_t))   /* LPM Resume Interrupt Enable */
#define BITM_USB_LPM_IEN_LPMNC                                                   (_ADI_MSK(0x00000008,uint8_t))   /* LPM NYET Control Interrupt Enable */
#define BITM_USB_LPM_IEN_LPMACK                                                  (_ADI_MSK(0x00000004,uint8_t))   /* LPM ACK Interrupt Enable */
#define BITM_USB_LPM_IEN_LPMNY                                                   (_ADI_MSK(0x00000002,uint8_t))   /* LPM NYET Interrupt Enable */
#define BITM_USB_LPM_IEN_LPMST                                                   (_ADI_MSK(0x00000001,uint8_t))   /* LPM STALL Interrupt Enable */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_LPM_IRQ                                                              Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_LPM_IRQ_LPMERR                                                   5                               /* LPM Error Interrupt */
#define BITP_USB_LPM_IRQ_LPMRES                                                   4                               /* LPM Resume Interrupt */
#define BITP_USB_LPM_IRQ_LPMNC                                                    3                               /* LPM NYET Control Interrupt */
#define BITP_USB_LPM_IRQ_LPMACK                                                   2                               /* LPM ACK Interrupt */
#define BITP_USB_LPM_IRQ_LPMNY                                                    1                               /* LPM NYET Interrupt */
#define BITP_USB_LPM_IRQ_LPMST                                                    0                               /* LPM STALL Interrupt */
#define BITM_USB_LPM_IRQ_LPMERR                                                  (_ADI_MSK(0x00000020,uint8_t))   /* LPM Error Interrupt */
#define BITM_USB_LPM_IRQ_LPMRES                                                  (_ADI_MSK(0x00000010,uint8_t))   /* LPM Resume Interrupt */
#define BITM_USB_LPM_IRQ_LPMNC                                                   (_ADI_MSK(0x00000008,uint8_t))   /* LPM NYET Control Interrupt */
#define BITM_USB_LPM_IRQ_LPMACK                                                  (_ADI_MSK(0x00000004,uint8_t))   /* LPM ACK Interrupt */
#define BITM_USB_LPM_IRQ_LPMNY                                                   (_ADI_MSK(0x00000002,uint8_t))   /* LPM NYET Interrupt */
#define BITM_USB_LPM_IRQ_LPMST                                                   (_ADI_MSK(0x00000001,uint8_t))   /* LPM STALL Interrupt */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_PHY_CTL                                                              Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_PHY_CTL_PULO                                                     7
#define BITP_USB_PHY_CTL_PUCON                                                    6
#define BITP_USB_PHY_CTL_PDCON                                                    5
#define BITP_USB_PHY_CTL_NDOE                                                     4
#define BITP_USB_PHY_CTL_DOM                                                      3
#define BITP_USB_PHY_CTL_DOP                                                      2
#define BITP_USB_PHY_CTL_SUSPEND                                                  1
#define BITP_USB_PHY_CTL_PHYMAN                                                   0
#define BITM_USB_PHY_CTL_PULO                                                    (_ADI_MSK(0x00000080,uint16_t))
#define BITM_USB_PHY_CTL_PUCON                                                   (_ADI_MSK(0x00000040,uint16_t))
#define BITM_USB_PHY_CTL_PDCON                                                   (_ADI_MSK(0x00000020,uint16_t))
#define BITM_USB_PHY_CTL_NDOE                                                    (_ADI_MSK(0x00000010,uint16_t))
#define BITM_USB_PHY_CTL_DOM                                                     (_ADI_MSK(0x00000008,uint16_t))
#define BITM_USB_PHY_CTL_DOP                                                     (_ADI_MSK(0x00000004,uint16_t))
#define BITM_USB_PHY_CTL_SUSPEND                                                 (_ADI_MSK(0x00000002,uint16_t))
#define BITM_USB_PHY_CTL_PHYMAN                                                  (_ADI_MSK(0x00000001,uint16_t))

/* ------------------------------------------------------------------------------------------------------------------------
        USB_PHY_STAT                                                             Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_PHY_STAT_VBUSVALID                                               7
#define BITP_USB_PHY_STAT_CID                                                     3
#define BITP_USB_PHY_STAT_DIDIF                                                   2
#define BITP_USB_PHY_STAT_DIM                                                     1
#define BITP_USB_PHY_STAT_DIP                                                     0
#define BITM_USB_PHY_STAT_VBUSVALID                                              (_ADI_MSK(0x00000080,uint16_t))
#define BITM_USB_PHY_STAT_CID                                                    (_ADI_MSK(0x00000008,uint16_t))
#define BITM_USB_PHY_STAT_DIDIF                                                  (_ADI_MSK(0x00000004,uint16_t))
#define BITM_USB_PHY_STAT_DIM                                                    (_ADI_MSK(0x00000002,uint16_t))
#define BITM_USB_PHY_STAT_DIP                                                    (_ADI_MSK(0x00000001,uint16_t))

/* ==================================================
        nvic Registers
   ================================================== */

/* =========================
        NVIC
   ========================= */
#define REG_NVIC_INTNUM                      0xE000E004         /* NVIC Interrupt Control Type */
#define REG_NVIC_STKSTA                      0xE000E010         /* NVIC Systick Control and Status */
#define REG_NVIC_STKLD                       0xE000E014         /* NVIC Systick Reload Value */
#define REG_NVIC_STKVAL                      0xE000E018         /* NVIC Systick Current Value */
#define REG_NVIC_STKCAL                      0xE000E01C         /* NVIC Systick Calibration Value */
#define REG_NVIC_INTSETE0                    0xE000E100         /* NVIC IRQ0..31 Set_Enable */
#define REG_NVIC_INTSETE1                    0xE000E104         /* NVIC IRQ32..63 Set_Enable */
#define REG_NVIC_INTCLRE0                    0xE000E180         /* NVIC IRQ0..31 Clear_Enable */
#define REG_NVIC_INTCLRE1                    0xE000E184         /* NVIC IRQ32..63 Clear_Enable */
#define REG_NVIC_INTSETP0                    0xE000E200         /* NVIC IRQ0..31 Set_Pending */
#define REG_NVIC_INTSETP1                    0xE000E204         /* NVIC IRQ32..63 Set_Pending */
#define REG_NVIC_INTCLRP0                    0xE000E280         /* NVIC IRQ0..31 Clear_Pending */
#define REG_NVIC_INTCLRP1                    0xE000E284         /* NVIC IRQ32..63 Clear_Pending */
#define REG_NVIC_INTACT0                     0xE000E300         /* NVIC IRQ0..31 Active Bit */
#define REG_NVIC_INTACT1                     0xE000E304         /* NVIC IRQ32..63 Active Bit */
#define REG_NVIC_INTPRI0                     0xE000E400         /* NVIC IRQ0..3 Priority */
#define REG_NVIC_INTPRI1                     0xE000E404         /* NVIC IRQ4..7 Priority */
#define REG_NVIC_INTPRI2                     0xE000E408         /* NVIC IRQ8..11 Priority */
#define REG_NVIC_INTPRI3                     0xE000E40C         /* NVIC IRQ12..15 Priority */
#define REG_NVIC_INTPRI4                     0xE000E410         /* NVIC IRQ16..19 Priority */
#define REG_NVIC_INTPRI5                     0xE000E414         /* NVIC IRQ20..23 Priority */
#define REG_NVIC_INTPRI6                     0xE000E418         /* NVIC IRQ24..27 Priority */
#define REG_NVIC_INTPRI7                     0xE000E41C         /* NVIC IRQ28..31 Priority */
#define REG_NVIC_INTPRI8                     0xE000E420         /* NVIC IRQ32..35 Priority */
#define REG_NVIC_INTPRI9                     0xE000E424         /* NVIC IRQ36..39 Priority */
#define REG_NVIC_INTPRI10                    0xE000E428         /* NVIC IRQ40..43 Priority */
#define REG_NVIC_INTCPID                     0xE000ED00         /* NVIC CPUID Base */
#define REG_NVIC_INTSTA                      0xE000ED04         /* NVIC Interrupt Control State */
#define REG_NVIC_INTVEC                      0xE000ED08         /* NVIC Vector Table Offset */
#define REG_NVIC_INTAIRC                     0xE000ED0C         /* NVIC Application Interrupt/Reset Control */
#define REG_NVIC_INTCON0                     0xE000ED10         /* NVIC System Control */
#define REG_NVIC_INTCON1                     0xE000ED14         /* NVIC Configuration Control */
#define REG_NVIC_INTSHPRIO0                  0xE000ED18         /* NVIC System Handlers 4-7 Priority */
#define REG_NVIC_INTSHPRIO1                  0xE000ED1C         /* NVIC System Handlers 8-11 Priority */
#define REG_NVIC_INTSHPRIO3                  0xE000ED20         /* NVIC System Handlers 12-15 Priority */
#define REG_NVIC_INTSHCSR                    0xE000ED24         /* NVIC System Handler Control and State */
#define REG_NVIC_INTCFSR                     0xE000ED28         /* NVIC Configurable Fault Status */
#define REG_NVIC_INTHFSR                     0xE000ED2C         /* NVIC Hard Fault Status */
#define REG_NVIC_INTDFSR                     0xE000ED30         /* NVIC Debug Fault Status */
#define REG_NVIC_INTMMAR                     0xE000ED34         /* NVIC Mem Manage Address */
#define REG_NVIC_INTBFAR                     0xE000ED38         /* NVIC Bus Fault Address */
#define REG_NVIC_INTAFSR                     0xE000ED3C         /* NVIC Auxiliary Fault Status */
#define REG_NVIC_INTPFR0                     0xE000ED40         /* NVIC Processor Feature Register 0 */
#define REG_NVIC_INTPFR1                     0xE000ED44         /* NVIC Processor Feature Register 1 */
#define REG_NVIC_INTDFR0                     0xE000ED48         /* NVIC Debug Feature Register 0 */
#define REG_NVIC_INTAFR0                     0xE000ED4C         /* NVIC Auxiliary Feature Register 0 */
#define REG_NVIC_INTMMFR0                    0xE000ED50         /* NVIC Memory Model Feature Register 0 */
#define REG_NVIC_INTMMFR1                    0xE000ED54         /* NVIC Memory Model Feature Register 1 */
#define REG_NVIC_INTMMFR2                    0xE000ED58         /* NVIC Memory Model Feature Register 2 */
#define REG_NVIC_INTMMFR3                    0xE000ED5C         /* NVIC Memory Model Feature Register 3 */
#define REG_NVIC_INTISAR0                    0xE000ED60         /* NVIC ISA Feature Register 0 */
#define REG_NVIC_INTISAR1                    0xE000ED64         /* NVIC ISA Feature Register 1 */
#define REG_NVIC_INTISAR2                    0xE000ED68         /* NVIC ISA Feature Register 2 */
#define REG_NVIC_INTISAR3                    0xE000ED6C         /* NVIC ISA Feature Register 3 */
#define REG_NVIC_INTISAR4                    0xE000ED70         /* NVIC ISA Feature Register 4 */
#define REG_NVIC_INTTRGI                     0xE000EF00         /* NVIC Software Trigger Interrupt Register */
#define REG_NVIC_INTPID4                     0xE000EFD0         /* NVIC Peripheral Identification Register 4 */
#define REG_NVIC_INTPID5                     0xE000EFD4         /* NVIC Peripheral Identification Register 5 */
#define REG_NVIC_INTPID6                     0xE000EFD8         /* NVIC Peripheral Identification Register 6 */
#define REG_NVIC_INTPID7                     0xE000EFDC         /* NVIC Peripheral Identification Register 7 */
#define REG_NVIC_INTPID0                     0xE000EFE0         /* NVIC Peripheral Identification Bits7:0 */
#define REG_NVIC_INTPID1                     0xE000EFE4         /* NVIC Peripheral Identification Bits15:8 */
#define REG_NVIC_INTPID2                     0xE000EFE8         /* NVIC Peripheral Identification Bits16:23 */
#define REG_NVIC_INTPID3                     0xE000EFEC         /* NVIC Peripheral Identification Bits24:31 */
#define REG_NVIC_INTCID0                     0xE000EFF0         /* NVIC Component Identification Bits7:0 */
#define REG_NVIC_INTCID1                     0xE000EFF4         /* NVIC Component Identification Bits15:8 */
#define REG_NVIC_INTCID2                     0xE000EFF8         /* NVIC Component Identification Bits16:23 */
#define REG_NVIC_INTCID3                     0xE000EFFC         /* NVIC Component Identification Bits24:31 */

/* =========================
        NVIC
   ========================= */
/* ------------------------------------------------------------------------------------------------------------------------
        NVIC_INTCON0                                                             Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_NVIC_INTCON0_SLEEPDEEP                                               2                               /* deep sleep flag for HIBERNATE mode */
#define BITP_NVIC_INTCON0_SLEEPONEXIT                                             1                               /* Sleeps the core on exit from an ISR */
#define BITM_NVIC_INTCON0_SLEEPDEEP                                              (_ADI_MSK(0x00000004,uint16_t))  /* deep sleep flag for HIBERNATE mode */
#define BITM_NVIC_INTCON0_SLEEPONEXIT                                            (_ADI_MSK(0x00000002,uint16_t))  /* Sleeps the core on exit from an ISR */




/* ==============================
       Parameters
   ============================== */

























/* Universal Serial Bus Controller Parameters */

#define PARAM_USB0_DMA_CHAN                                                               2
#define PARAM_USB0_DYN_FIFO_SIZE                                                          0
#define PARAM_USB0_FS_PHY                                                                 1
#define PARAM_USB0_HOST_MODE                                                              0
#define PARAM_USB0_HS_PHY                                                                 0
#define PARAM_USB0_LOOPBACK                                                               0
#define PARAM_USB0_LS_PHY                                                                 0
#define PARAM_USB0_MULTI_POINT                                                            0
#define PARAM_USB0_NUM_ENDPTS                                                             4
#define PARAM_USB0_NUM_ENDPTS_MINUS_1                                                     3


/* ===================================
       Trigger Master Definitions
   =================================== */

/* ===================================
       Trigger Slave Definitions
   =================================== */


/* ============================================================================
       Memory Map Macros
   ============================================================================ */

/* ADSP-ADuCM350 is a single-core processor */

#define MEM_NUM_CORES

/* Internal memory range */

#define MEM_BASE_INTERNAL    0x00000000
#define MEM_END_INTERNAL     0x00000000
#define MEM_SIZE_INTERNAL    0x0

/* External memory range */

#define MEM_BASE_EXTERNAL    0x00000000
#define MEM_END_EXTERNAL     0x00000000
#define MEM_SIZE_EXTERNAL    0x0


#endif	/* end ifndef _DEF_ADuCM350_H */
