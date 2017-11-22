/*********************************************************************************

  Copyright(c) 2013 Analog Devices, Inc. All Rights Reserved.

  This software is proprietary and confidential.  By using this software you agree
  to the terms of the associated Analog Devices License Agreement.

*********************************************************************************/

/*!
* @file      adi_usbh_dev_musbmhdrc_platform.h
*
* @brief     platform specific functions are kept in this file
*
*/

#ifndef _ADI_USB_MUSBMHDRC_PLATFORM_H_
#define _ADI_USB_MUSBMHDRC_PLATFORM_H_

#ifdef _MISRA_RULES
#pragma diag(push)
#pragma diag(suppress:misra_rules_all:"Suppress all misra rules")
#endif

#if defined(__ADSPSHARC__) || defined(__ADSPBLACKFIN__)
#include <builtins.h>
#endif

#define _ADI_MSK( mask, type ) ((type)(mask)) 

#if defined(__ADSPBLACKFIN__)
#define SSYNC()         ssync()
#elif defined(__ADSPSC589_FAMILY__) /* Griffin */
#define SSYNC()         ; /* TODO: Didn't find the definition of ssync in ARM */
#elif defined(ADI_ADUCM350)
#define SSYNC()         ; /* TODO: Didn't find the definition of ssync in ARM */
#define INTR_USB0_STAT  ADI_INT_USB
#define INTR_USB0_DATA  ADI_INT_USB_DMA
#endif

/*
 * Defines for Critical region
 */
#if defined(__ADSPBF526_FAMILY__) || defined(__ADSPBF527_FAMILY__) || defined(__ADSPBF548_FAMILY__)

#define ADI_USB_ENTER_CR() do { adi_int_EnableInt(ADI_SID_USB_INT0, false);  \
                                adi_int_EnableInt(ADI_SID_USB_INT1, false);  \
                                adi_int_EnableInt(ADI_SID_USB_INT2, false);  \
                                adi_int_EnableInt(ADI_SID_USB_DMAINT, false);\
                              } while(0)

#define ADI_USB_EXIT_CR()  do { adi_int_EnableInt(ADI_SID_USB_INT0, true);   \
                                adi_int_EnableInt(ADI_SID_USB_INT1, true);   \
                                adi_int_EnableInt(ADI_SID_USB_INT2, true);   \
                                adi_int_EnableInt(ADI_SID_USB_DMAINT, true); \
                              } while(0)

#elif defined(__ADSPBF609_FAMILY__) || defined(__ADSPBF707_FAMILY__) || defined(__ADSPSC589_FAMILY__) || defined(ADI_ADUCM350)

#define ADI_USB_ENTER_CR() do { adi_int_EnableInt(INTR_USB0_STAT, false);  \
                                adi_int_EnableInt(INTR_USB0_DATA, false);  \
                              } while(0)

#define ADI_USB_EXIT_CR()  do { adi_int_EnableInt(INTR_USB0_STAT, true);   \
                                adi_int_EnableInt(INTR_USB0_DATA, true);   \
                              } while(0)                             
#endif

#if defined(__ADSPBF526_FAMILY__) || defined(__ADSPBF527_FAMILY__)
#define ADI_USB_REG_BASE    0xFFC03800
#elif defined(__ADSPBF548_FAMILY__)
#define ADI_USB_REG_BASE    0xFFC03c00
#elif defined(__ADSPBF609_FAMILY__)
#define ADI_USB_REG_BASE    0xFFCC1000
#elif defined(__ADSPBF707_FAMILY__)
#define ADI_USB_REG_BASE    0x200D0000
#elif defined(__ADSPSC589_FAMILY__)
#define ADI_USB_REG_BASE    0x310C1000
#elif defined(ADI_ADUCM350)                                
#define ADI_USB_REG_BASE    ADI_USB_BASE
#endif

#if defined(__ADSPBF526_FAMILY__) || defined(__ADSPBF527_FAMILY__) || defined(__ADSPBF548_FAMILY__)
typedef uint16_t ADI_USB_8BIT_REG_TYPE;
#elif defined(__ADSPBF609_FAMILY__) || defined(__ADSPBF707_FAMILY__) || defined(__ADSPSC589_FAMILY__) ||defined(ADI_ADUCM350)
typedef uint8_t ADI_USB_8BIT_REG_TYPE;
#endif

#if defined(ADI_CFG_USB_BF_MUSBHDRC)
#include <services/int/adi_sid.h>


/* ------------------------------------------------------------------------------------------------------------------------
        USB_FADDR                            Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_FADDR_VALUE                  0                               /* Function Address Value */
#define BITM_USB_FADDR_VALUE                 (_ADI_MSK(0x0000007F,uint8_t))   /* Function Address Value */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_POWER                            Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_POWER_ISOUPDT                7                               /* ISO Update Enable */
#define BITP_USB_POWER_SOFTCONN               6                               /* Soft Connect/Disconnect Enable */
#define BITP_USB_POWER_HSEN                   5                               /* High Speed Mode Enable */
#define BITP_USB_POWER_HSMODE                 4                               /* High Speed Mode */
#define BITP_USB_POWER_RESET                  3                               /* Reset USB */
#define BITP_USB_POWER_RESUME                 2                               /* Resume Mode */
#define BITP_USB_POWER_SUSPEND                1                               /* Suspend Mode */
#define BITP_USB_POWER_SUSEN                  0                               /* SUSPENDM Output Enable */

#define BITM_USB_POWER_ISOUPDT               (_ADI_MSK(0x00000080,uint8_t))   /* ISO Update Enable */
#define ENUM_USB_POWER_NO_ISOUPDT            (_ADI_MSK(0x00000000,uint8_t))   /* ISOUPDT: Disable ISO Update */
#define ENUM_USB_POWER_ISOUPDT               (_ADI_MSK(0x00000080,uint8_t))   /* ISOUPDT: Enable ISO Update */

#define BITM_USB_POWER_SOFTCONN              (_ADI_MSK(0x00000040,uint8_t))   /* Soft Connect/Disconnect Enable */
#define ENUM_USB_POWER_NO_SOFTCONN           (_ADI_MSK(0x00000000,uint8_t))   /* SOFTCONN: Disable Soft Connect/Disconnect */
#define ENUM_USB_POWER_SOFTCONN              (_ADI_MSK(0x00000040,uint8_t))   /* SOFTCONN: Enable Soft Connect/Disconnect */

#define BITM_USB_POWER_HSEN                  (_ADI_MSK(0x00000020,uint8_t))   /* High Speed Mode Enable */
#define ENUM_USB_POWER_HSDIS                 (_ADI_MSK(0x00000000,uint8_t))   /* HSEN: Disable Negotiation for HS Mode */
#define ENUM_USB_POWER_HSEN                  (_ADI_MSK(0x00000020,uint8_t))   /* HSEN: Enable Negotiation for HS Mode */

#define BITM_USB_POWER_HSMODE                (_ADI_MSK(0x00000010,uint8_t))   /* High Speed Mode */
#define ENUM_USB_POWER_NO_HSMODE             (_ADI_MSK(0x00000000,uint8_t))   /* HSMODE: Full Speed Mode (HS fail during reset) */
#define ENUM_USB_POWER_HSMODE                (_ADI_MSK(0x00000010,uint8_t))   /* HSMODE: High Speed Mode (HS success during reset) */

#define BITM_USB_POWER_RESET                 (_ADI_MSK(0x00000008,uint8_t))   /* Reset USB */
#define ENUM_USB_POWER_NO_RESET              (_ADI_MSK(0x00000000,uint8_t))   /* RESET: No Reset */
#define ENUM_USB_POWER_RESET                 (_ADI_MSK(0x00000008,uint8_t))   /* RESET: Reset USB */

#define BITM_USB_POWER_RESUME                (_ADI_MSK(0x00000004,uint8_t))   /* Resume Mode */
#define ENUM_USB_POWER_NO_RESUME             (_ADI_MSK(0x00000000,uint8_t))   /* RESUME: Disable Resume Signaling */
#define ENUM_USB_POWER_RESUME                (_ADI_MSK(0x00000004,uint8_t))   /* RESUME: Enable Resume Signaling */

#define BITM_USB_POWER_SUSPEND               (_ADI_MSK(0x00000002,uint8_t))   /* Suspend Mode */
#define ENUM_USB_POWER_NO_SUSPEND            (_ADI_MSK(0x00000000,uint8_t))   /* SUSPEND: Disable Suspend Mode (Host) */
#define ENUM_USB_POWER_SUSPEND               (_ADI_MSK(0x00000002,uint8_t))   /* SUSPEND: Enable Suspend Mode (Host) */

#define BITM_USB_POWER_SUSEN                 (_ADI_MSK(0x00000001,uint8_t))   /* SUSPENDM Output Enable */
#define ENUM_USB_POWER_SUSDIS                (_ADI_MSK(0x00000000,uint8_t))   /* SUSEN: Disable SUSPENDM Output */
#define ENUM_USB_POWER_SUSEN                 (_ADI_MSK(0x00000001,uint8_t))   /* SUSEN: Enable SUSPENDM Output */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_INTRTX                           Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_INTRTX_EP11                 11                               /* End Point 11 Tx Interrupt */
#define BITP_USB_INTRTX_EP10                 10                               /* End Point 10 Tx Interrupt */
#define BITP_USB_INTRTX_EP9                   9                               /* End Point 9 Tx Interrupt */
#define BITP_USB_INTRTX_EP8                   8                               /* End Point 8 Tx Interrupt */
#define BITP_USB_INTRTX_EP7                   7                               /* End Point 7 Tx Interrupt */
#define BITP_USB_INTRTX_EP6                   6                               /* End Point 6 Tx Interrupt */
#define BITP_USB_INTRTX_EP5                   5                               /* End Point 5 Tx Interrupt */
#define BITP_USB_INTRTX_EP4                   4                               /* End Point 4 Tx Interrupt */
#define BITP_USB_INTRTX_EP3                   3                               /* End Point 3 Tx Interrupt */
#define BITP_USB_INTRTX_EP2                   2                               /* End Point 2 Tx Interrupt */
#define BITP_USB_INTRTX_EP1                   1                               /* End Point 1 Tx Interrupt */
#define BITP_USB_INTRTX_EP0                   0                               /* End Point 0 Tx Interrupt */
#define BITM_USB_INTRTX_EP11                 (_ADI_MSK(0x00000800,uint16_t))  /* End Point 11 Tx Interrupt */
#define BITM_USB_INTRTX_EP10                 (_ADI_MSK(0x00000400,uint16_t))  /* End Point 10 Tx Interrupt */
#define BITM_USB_INTRTX_EP9                  (_ADI_MSK(0x00000200,uint16_t))  /* End Point 9 Tx Interrupt */
#define BITM_USB_INTRTX_EP8                  (_ADI_MSK(0x00000100,uint16_t))  /* End Point 8 Tx Interrupt */
#define BITM_USB_INTRTX_EP7                  (_ADI_MSK(0x00000080,uint16_t))  /* End Point 7 Tx Interrupt */
#define BITM_USB_INTRTX_EP6                  (_ADI_MSK(0x00000040,uint16_t))  /* End Point 6 Tx Interrupt */
#define BITM_USB_INTRTX_EP5                  (_ADI_MSK(0x00000020,uint16_t))  /* End Point 5 Tx Interrupt */
#define BITM_USB_INTRTX_EP4                  (_ADI_MSK(0x00000010,uint16_t))  /* End Point 4 Tx Interrupt */
#define BITM_USB_INTRTX_EP3                  (_ADI_MSK(0x00000008,uint16_t))  /* End Point 3 Tx Interrupt */
#define BITM_USB_INTRTX_EP2                  (_ADI_MSK(0x00000004,uint16_t))  /* End Point 2 Tx Interrupt */
#define BITM_USB_INTRTX_EP1                  (_ADI_MSK(0x00000002,uint16_t))  /* End Point 1 Tx Interrupt */
#define BITM_USB_INTRTX_EP0                  (_ADI_MSK(0x00000001,uint16_t))  /* End Point 0 Tx Interrupt */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_INTRRX                           Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_INTRRX_EP11                 11                               /* End Point 11 Rx Interrupt. */
#define BITP_USB_INTRRX_EP10                 10                               /* End Point 10 Rx Interrupt. */
#define BITP_USB_INTRRX_EP9                   9                               /* End Point 9 Rx Interrupt. */
#define BITP_USB_INTRRX_EP8                   8                               /* End Point 8 Rx Interrupt. */
#define BITP_USB_INTRRX_EP7                   7                               /* End Point 7 Rx Interrupt. */
#define BITP_USB_INTRRX_EP6                   6                               /* End Point 6 Rx Interrupt. */
#define BITP_USB_INTRRX_EP5                   5                               /* End Point 5 Rx Interrupt. */
#define BITP_USB_INTRRX_EP4                   4                               /* End Point 4 Rx Interrupt. */
#define BITP_USB_INTRRX_EP3                   3                               /* End Point 3 Rx Interrupt. */
#define BITP_USB_INTRRX_EP2                   2                               /* End Point 2 Rx Interrupt. */
#define BITP_USB_INTRRX_EP1                   1                               /* End Point 1 Rx Interrupt. */
#define BITM_USB_INTRRX_EP11                 (_ADI_MSK(0x00000800,uint16_t))  /* End Point 11 Rx Interrupt. */
#define BITM_USB_INTRRX_EP10                 (_ADI_MSK(0x00000400,uint16_t))  /* End Point 10 Rx Interrupt. */
#define BITM_USB_INTRRX_EP9                  (_ADI_MSK(0x00000200,uint16_t))  /* End Point 9 Rx Interrupt. */
#define BITM_USB_INTRRX_EP8                  (_ADI_MSK(0x00000100,uint16_t))  /* End Point 8 Rx Interrupt. */
#define BITM_USB_INTRRX_EP7                  (_ADI_MSK(0x00000080,uint16_t))  /* End Point 7 Rx Interrupt. */
#define BITM_USB_INTRRX_EP6                  (_ADI_MSK(0x00000040,uint16_t))  /* End Point 6 Rx Interrupt. */
#define BITM_USB_INTRRX_EP5                  (_ADI_MSK(0x00000020,uint16_t))  /* End Point 5 Rx Interrupt. */
#define BITM_USB_INTRRX_EP4                  (_ADI_MSK(0x00000010,uint16_t))  /* End Point 4 Rx Interrupt. */
#define BITM_USB_INTRRX_EP3                  (_ADI_MSK(0x00000008,uint16_t))  /* End Point 3 Rx Interrupt. */
#define BITM_USB_INTRRX_EP2                  (_ADI_MSK(0x00000004,uint16_t))  /* End Point 2 Rx Interrupt. */
#define BITM_USB_INTRRX_EP1                  (_ADI_MSK(0x00000002,uint16_t))  /* End Point 1 Rx Interrupt. */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_INTRTXE                          Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_INTRTXE_EP11                11                               /* End Point 11 Tx Interrupt Enable */
#define BITP_USB_INTRTXE_EP10                10                               /* End Point 10 Tx Interrupt Enable */
#define BITP_USB_INTRTXE_EP9                  9                               /* End Point 9 Tx Interrupt Enable */
#define BITP_USB_INTRTXE_EP8                  8                               /* End Point 8 Tx Interrupt Enable */
#define BITP_USB_INTRTXE_EP7                  7                               /* End Point 7 Tx Interrupt Enable */
#define BITP_USB_INTRTXE_EP6                  6                               /* End Point 6 Tx Interrupt Enable */
#define BITP_USB_INTRTXE_EP5                  5                               /* End Point 5 Tx Interrupt Enable */
#define BITP_USB_INTRTXE_EP4                  4                               /* End Point 4 Tx Interrupt Enable */
#define BITP_USB_INTRTXE_EP3                  3                               /* End Point 3 Tx Interrupt Enable */
#define BITP_USB_INTRTXE_EP2                  2                               /* End Point 2 Tx Interrupt Enable */
#define BITP_USB_INTRTXE_EP1                  1                               /* End Point 1 Tx Interrupt Enable */
#define BITP_USB_INTRTXE_EP0                  0                               /* End Point 0 Tx Interrupt Enable */
#define BITM_USB_INTRTXE_EP11                (_ADI_MSK(0x00000800,uint16_t))  /* End Point 11 Tx Interrupt Enable */
#define BITM_USB_INTRTXE_EP10                (_ADI_MSK(0x00000400,uint16_t))  /* End Point 10 Tx Interrupt Enable */
#define BITM_USB_INTRTXE_EP9                 (_ADI_MSK(0x00000200,uint16_t))  /* End Point 9 Tx Interrupt Enable */
#define BITM_USB_INTRTXE_EP8                 (_ADI_MSK(0x00000100,uint16_t))  /* End Point 8 Tx Interrupt Enable */
#define BITM_USB_INTRTXE_EP7                 (_ADI_MSK(0x00000080,uint16_t))  /* End Point 7 Tx Interrupt Enable */
#define BITM_USB_INTRTXE_EP6                 (_ADI_MSK(0x00000040,uint16_t))  /* End Point 6 Tx Interrupt Enable */
#define BITM_USB_INTRTXE_EP5                 (_ADI_MSK(0x00000020,uint16_t))  /* End Point 5 Tx Interrupt Enable */
#define BITM_USB_INTRTXE_EP4                 (_ADI_MSK(0x00000010,uint16_t))  /* End Point 4 Tx Interrupt Enable */
#define BITM_USB_INTRTXE_EP3                 (_ADI_MSK(0x00000008,uint16_t))  /* End Point 3 Tx Interrupt Enable */
#define BITM_USB_INTRTXE_EP2                 (_ADI_MSK(0x00000004,uint16_t))  /* End Point 2 Tx Interrupt Enable */
#define BITM_USB_INTRTXE_EP1                 (_ADI_MSK(0x00000002,uint16_t))  /* End Point 1 Tx Interrupt Enable */
#define BITM_USB_INTRTXE_EP0                 (_ADI_MSK(0x00000001,uint16_t))  /* End Point 0 Tx Interrupt Enable */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_INTRRXE                          Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_INTRRXE_EP11                11                               /* End Point 11 Rx Interrupt Enable */
#define BITP_USB_INTRRXE_EP10                10                               /* End Point 10 Rx Interrupt Enable */
#define BITP_USB_INTRRXE_EP9                  9                               /* End Point 9 Rx Interrupt Enable */
#define BITP_USB_INTRRXE_EP8                  8                               /* End Point 8 Rx Interrupt Enable */
#define BITP_USB_INTRRXE_EP7                  7                               /* End Point 7 Rx Interrupt Enable */
#define BITP_USB_INTRRXE_EP6                  6                               /* End Point 6 Rx Interrupt Enable */
#define BITP_USB_INTRRXE_EP5                  5                               /* End Point 5 Rx Interrupt Enable */
#define BITP_USB_INTRRXE_EP4                  4                               /* End Point 4 Rx Interrupt Enable */
#define BITP_USB_INTRRXE_EP3                  3                               /* End Point 3 Rx Interrupt Enable */
#define BITP_USB_INTRRXE_EP2                  2                               /* End Point 2 Rx Interrupt Enable */
#define BITP_USB_INTRRXE_EP1                  1                               /* End Point 1 Rx Interrupt Enable */
#define BITM_USB_INTRRXE_EP11                (_ADI_MSK(0x00000800,uint16_t))  /* End Point 11 Rx Interrupt Enable */
#define BITM_USB_INTRRXE_EP10                (_ADI_MSK(0x00000400,uint16_t))  /* End Point 10 Rx Interrupt Enable */
#define BITM_USB_INTRRXE_EP9                 (_ADI_MSK(0x00000200,uint16_t))  /* End Point 9 Rx Interrupt Enable */
#define BITM_USB_INTRRXE_EP8                 (_ADI_MSK(0x00000100,uint16_t))  /* End Point 8 Rx Interrupt Enable */
#define BITM_USB_INTRRXE_EP7                 (_ADI_MSK(0x00000080,uint16_t))  /* End Point 7 Rx Interrupt Enable */
#define BITM_USB_INTRRXE_EP6                 (_ADI_MSK(0x00000040,uint16_t))  /* End Point 6 Rx Interrupt Enable */
#define BITM_USB_INTRRXE_EP5                 (_ADI_MSK(0x00000020,uint16_t))  /* End Point 5 Rx Interrupt Enable */
#define BITM_USB_INTRRXE_EP4                 (_ADI_MSK(0x00000010,uint16_t))  /* End Point 4 Rx Interrupt Enable */
#define BITM_USB_INTRRXE_EP3                 (_ADI_MSK(0x00000008,uint16_t))  /* End Point 3 Rx Interrupt Enable */
#define BITM_USB_INTRRXE_EP2                 (_ADI_MSK(0x00000004,uint16_t))  /* End Point 2 Rx Interrupt Enable */
#define BITM_USB_INTRRXE_EP1                 (_ADI_MSK(0x00000002,uint16_t))  /* End Point 1 Rx Interrupt Enable */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_IRQ                              Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_IRQ_VBUSERR                  7                               /* VBUS Threshold Indicator */
#define BITP_USB_IRQ_SESSREQ                  6                               /* Session Request Indicator */
#define BITP_USB_IRQ_DISCON                   5                               /* Disconnect Indicator */
#define BITP_USB_IRQ_CON                      4                               /* Connection Indicator */
#define BITP_USB_IRQ_SOF                      3                               /* Start-of-frame Indicator */
#define BITP_USB_IRQ_RSTBABBLE                2                               /* Reset/Babble Indicator */
#define BITP_USB_IRQ_RESUME                   1                               /* Resume Indicator */
#define BITP_USB_IRQ_SUSPEND                  0                               /* Suspend Indicator */

#define BITM_USB_IRQ_VBUSERR                 (_ADI_MSK(0x00000080,uint8_t))   /* VBUS Threshold Indicator */
#define ENUM_USB_IRQ_NO_VBUSERR              (_ADI_MSK(0x00000000,uint8_t))   /* VBUSERR: No Interrupt */
#define ENUM_USB_IRQ_VBUSERR                 (_ADI_MSK(0x00000080,uint8_t))   /* VBUSERR: Interrupt Pending */

#define BITM_USB_IRQ_SESSREQ                 (_ADI_MSK(0x00000040,uint8_t))   /* Session Request Indicator */
#define ENUM_USB_IRQ_NO_SESSREQ              (_ADI_MSK(0x00000000,uint8_t))   /* SESSREQ: No Interrupt */
#define ENUM_USB_IRQ_SESSREQ                 (_ADI_MSK(0x00000040,uint8_t))   /* SESSREQ: Interrupt Pending */

#define BITM_USB_IRQ_DISCON                  (_ADI_MSK(0x00000020,uint8_t))   /* Disconnect Indicator */
#define ENUM_USB_IRQ_NO_DISCON               (_ADI_MSK(0x00000000,uint8_t))   /* DISCON: No Interrupt */
#define ENUM_USB_IRQ_DISCON                  (_ADI_MSK(0x00000020,uint8_t))   /* DISCON: Interrupt Pending */

#define BITM_USB_IRQ_CON                     (_ADI_MSK(0x00000010,uint8_t))   /* Connection Indicator */
#define ENUM_USB_IRQ_NO_CON                  (_ADI_MSK(0x00000000,uint8_t))   /* CON: No Interrupt */
#define ENUM_USB_IRQ_CON                     (_ADI_MSK(0x00000010,uint8_t))   /* CON: Interrupt Pending */

#define BITM_USB_IRQ_SOF                     (_ADI_MSK(0x00000008,uint8_t))   /* Start-of-frame Indicator */
#define ENUM_USB_IRQ_NO_SOF                  (_ADI_MSK(0x00000000,uint8_t))   /* SOF: No Interrupt */
#define ENUM_USB_IRQ_SOF                     (_ADI_MSK(0x00000008,uint8_t))   /* SOF: Interrupt Pending */

#define BITM_USB_IRQ_RSTBABBLE               (_ADI_MSK(0x00000004,uint8_t))   /* Reset/Babble Indicator */
#define ENUM_USB_IRQ_NO_RSTBABBLE            (_ADI_MSK(0x00000000,uint8_t))   /* RSTBABBLE: No Interrupt */
#define ENUM_USB_IRQ_RSTBABBLE               (_ADI_MSK(0x00000004,uint8_t))   /* RSTBABBLE: Interrupt Pending */

#define BITM_USB_IRQ_RESUME                  (_ADI_MSK(0x00000002,uint8_t))   /* Resume Indicator */
#define ENUM_USB_IRQ_NO_RESUME               (_ADI_MSK(0x00000000,uint8_t))   /* RESUME: No Interrupt */
#define ENUM_USB_IRQ_RESUME                  (_ADI_MSK(0x00000002,uint8_t))   /* RESUME: Interrupt Pending */

#define BITM_USB_IRQ_SUSPEND                 (_ADI_MSK(0x00000001,uint8_t))   /* Suspend Indicator */
#define ENUM_USB_IRQ_NO_SUSPEND              (_ADI_MSK(0x00000000,uint8_t))   /* SUSPEND: No Interrupt */
#define ENUM_USB_IRQ_SUSPEND                 (_ADI_MSK(0x00000001,uint8_t))   /* SUSPEND: Interrupt Pending */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_IEN                              Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_IEN_VBUSERR                  7                               /* VBUS Threshold Indicator Interrupt Enable */
#define BITP_USB_IEN_SESSREQ                  6                               /* Session Request Indicator Interrupt Enable */
#define BITP_USB_IEN_DISCON                   5                               /* Disconnect Indicator Interrupt Enable */
#define BITP_USB_IEN_CON                      4                               /* Connection Indicator Interrupt Enable */
#define BITP_USB_IEN_SOF                      3                               /* Start-of-frame Indicator Interrupt Enable */
#define BITP_USB_IEN_RSTBABBLE                2                               /* Reset/Babble Indicator Interrupt Enable */
#define BITP_USB_IEN_RESUME                   1                               /* Resume Indicator Interrupt Enable */
#define BITP_USB_IEN_SUSPEND                  0                               /* Suspend Indicator Interrupt Enable */

#define BITM_USB_IEN_VBUSERR                 (_ADI_MSK(0x00000080,uint8_t))   /* VBUS Threshold Indicator Interrupt Enable */
#define ENUM_USB_IEN_VBUSERRDIS              (_ADI_MSK(0x00000000,uint8_t))   /* VBUSERR: Disable Interrupt */
#define ENUM_USB_IEN_VBUSERREN               (_ADI_MSK(0x00000080,uint8_t))   /* VBUSERR: Enable Interrupt */

#define BITM_USB_IEN_SESSREQ                 (_ADI_MSK(0x00000040,uint8_t))   /* Session Request Indicator Interrupt Enable */
#define ENUM_USB_IEN_SESSREQDIS              (_ADI_MSK(0x00000000,uint8_t))   /* SESSREQ: Disable Interrupt */
#define ENUM_USB_IEN_SESSREQEN               (_ADI_MSK(0x00000040,uint8_t))   /* SESSREQ: Enable Interrupt */

#define BITM_USB_IEN_DISCON                  (_ADI_MSK(0x00000020,uint8_t))   /* Disconnect Indicator Interrupt Enable */
#define ENUM_USB_IEN_DISCONDIS               (_ADI_MSK(0x00000000,uint8_t))   /* DISCON: Disable Interrupt */
#define ENUM_USB_IEN_DISCONEN                (_ADI_MSK(0x00000020,uint8_t))   /* DISCON: Enable Interrupt */

#define BITM_USB_IEN_CON                     (_ADI_MSK(0x00000010,uint8_t))   /* Connection Indicator Interrupt Enable */
#define ENUM_USB_IEN_CONDIS                  (_ADI_MSK(0x00000000,uint8_t))   /* CON: Disable Interrupt */
#define ENUM_USB_IEN_CONEN                   (_ADI_MSK(0x00000010,uint8_t))   /* CON: Enable Interrupt */

#define BITM_USB_IEN_SOF                     (_ADI_MSK(0x00000008,uint8_t))   /* Start-of-frame Indicator Interrupt Enable */
#define ENUM_USB_IEN_SOFDIS                  (_ADI_MSK(0x00000000,uint8_t))   /* SOF: Disable Interrupt */
#define ENUM_USB_IEN_SOFEN                   (_ADI_MSK(0x00000008,uint8_t))   /* SOF: Enable Interrupt */

#define BITM_USB_IEN_RSTBABBLE               (_ADI_MSK(0x00000004,uint8_t))   /* Reset/Babble Indicator Interrupt Enable */
#define ENUM_USB_IEN_RSTBABBLEDIS            (_ADI_MSK(0x00000000,uint8_t))   /* RSTBABBLE: Disable Interrupt */
#define ENUM_USB_IEN_RSTBABBLEEN             (_ADI_MSK(0x00000004,uint8_t))   /* RSTBABBLE: Enable Interrupt */

#define BITM_USB_IEN_RESUME                  (_ADI_MSK(0x00000002,uint8_t))   /* Resume Indicator Interrupt Enable */
#define ENUM_USB_IEN_RESUMEDIS               (_ADI_MSK(0x00000000,uint8_t))   /* RESUME: Disable Interrupt */
#define ENUM_USB_IEN_RESUMEEN                (_ADI_MSK(0x00000002,uint8_t))   /* RESUME: Enable Interrupt */

#define BITM_USB_IEN_SUSPEND                 (_ADI_MSK(0x00000001,uint8_t))   /* Suspend Indicator Interrupt Enable */
#define ENUM_USB_IEN_SUSPENDDIS              (_ADI_MSK(0x00000000,uint8_t))   /* SUSPEND: Disable Interrupt */
#define ENUM_USB_IEN_SUSPENDEN               (_ADI_MSK(0x00000001,uint8_t))   /* SUSPEND: Enable Interrupt */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_FRAME                            Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_FRAME_VALUE                  0                               /* Frame Number Value */
#define BITM_USB_FRAME_VALUE                 (_ADI_MSK(0x000007FF,uint16_t))  /* Frame Number Value */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_INDEX                            Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_INDEX_EP                     0                               /* Endpoint Index */
#define BITM_USB_INDEX_EP                    (_ADI_MSK(0x0000000F,uint8_t))   /* Endpoint Index */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_TESTMODE                         Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_TESTMODE_FIFOACCESS          6                               /* FIFO Access */
#define BITP_USB_TESTMODE_TESTPACKET          3                               /* Test_Packet Mode */
#define BITP_USB_TESTMODE_TESTK               2                               /* Test_K Mode */
#define BITP_USB_TESTMODE_TESTJ               1                               /* Test_J Mode */
#define BITP_USB_TESTMODE_TESTSE0NAK          0                               /* Test SE0 NAK */
#define BITM_USB_TESTMODE_FIFOACCESS         (_ADI_MSK(0x00000040,uint8_t))   /* FIFO Access */
#define BITM_USB_TESTMODE_TESTPACKET         (_ADI_MSK(0x00000008,uint8_t))   /* Test_Packet Mode */
#define BITM_USB_TESTMODE_TESTK              (_ADI_MSK(0x00000004,uint8_t))   /* Test_K Mode */
#define BITM_USB_TESTMODE_TESTJ              (_ADI_MSK(0x00000002,uint8_t))   /* Test_J Mode */
#define BITM_USB_TESTMODE_TESTSE0NAK         (_ADI_MSK(0x00000001,uint8_t))   /* Test SE0 NAK */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_EPI_TXMAXP                       Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_EPI_TXMAXP_MULTM1           11                               /* Multi-Packets per Micro-frame */
#define BITP_USB_EPI_TXMAXP_MAXPAY            0                               /* Maximum Payload */
#define BITM_USB_EPI_TXMAXP_MULTM1           (_ADI_MSK(0x00001800,uint16_t))  /* Multi-Packets per Micro-frame */
#define BITM_USB_EPI_TXMAXP_MAXPAY           (_ADI_MSK(0x000007FF,uint16_t))  /* Maximum Payload */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_EPI_TXCSR_P                      Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_EPI_TXCSR_P_AUTOSET         15                               /* TxPkRdy Autoset Enable */
#define BITP_USB_EPI_TXCSR_P_ISO             14                               /* Isochronous Transfers Enable */
#define BITP_USB_EPI_TXCSR_P_DMAREQEN        12                               /* DMA Request Enable Tx EP */
#define BITP_USB_EPI_TXCSR_P_FRCDATATGL      11                               /* Force Data Toggle */
#define BITP_USB_EPI_TXCSR_P_DMAREQMODE      10                               /* DMA Mode Select */
#define BITP_USB_EPI_TXCSR_P_INCOMPTX         7                               /* Incomplete Tx */
#define BITP_USB_EPI_TXCSR_P_CLRDATATGL       6                               /* Clear Endpoint Data Toggle */
#define BITP_USB_EPI_TXCSR_P_SENTSTALL        5                               /* Sent STALL */
#define BITP_USB_EPI_TXCSR_P_SENDSTALL        4                               /* Send STALL */
#define BITP_USB_EPI_TXCSR_P_FLUSHFIFO        3                               /* Flush Endpoint FIFO */
#define BITP_USB_EPI_TXCSR_P_URUNERR          2                               /* Underrun Error */
#define BITP_USB_EPI_TXCSR_P_NEFIFO           1                               /* Not Empty FIFO */
#define BITP_USB_EPI_TXCSR_P_TXPKTRDY         0                               /* Tx Packet Ready */

#define BITM_USB_EPI_TXCSR_P_AUTOSET         (_ADI_MSK(0x00008000,uint16_t))  /* TxPkRdy Autoset Enable */
#define ENUM_USB_EPI_TXCSR_P_NO_AUTOSET      (_ADI_MSK(0x00000000,uint16_t))  /* AUTOSET: Disable Autoset */
#define ENUM_USB_EPI_TXCSR_P_AUTOSET         (_ADI_MSK(0x00008000,uint16_t))  /* AUTOSET: Enable Autoset */

#define BITM_USB_EPI_TXCSR_P_ISO             (_ADI_MSK(0x00004000,uint16_t))  /* Isochronous Transfers Enable */
#define ENUM_USB_EPI_TXCSR_P_ISODIS          (_ADI_MSK(0x00000000,uint16_t))  /* ISO: Disable Tx EP Isochronous Transfers */
#define ENUM_USB_EPI_TXCSR_P_ISOEN           (_ADI_MSK(0x00004000,uint16_t))  /* ISO: Enable Tx EP Isochronous Transfers */

#define BITM_USB_EPI_TXCSR_P_DMAREQEN        (_ADI_MSK(0x00001000,uint16_t))  /* DMA Request Enable Tx EP */
#define ENUM_USB_EPI_TXCSR_P_DMAREQDIS       (_ADI_MSK(0x00000000,uint16_t))  /* DMAREQEN: Disable DMA Request */
#define ENUM_USB_EPI_TXCSR_P_DMAREQEN        (_ADI_MSK(0x00001000,uint16_t))  /* DMAREQEN: Enable DMA Request */

#define BITM_USB_EPI_TXCSR_P_FRCDATATGL      (_ADI_MSK(0x00000800,uint16_t))  /* Force Data Toggle */
#define ENUM_USB_EPI_TXCSR_P_NO_FRCTGL       (_ADI_MSK(0x00000000,uint16_t))  /* FRCDATATGL: No Action */
#define ENUM_USB_EPI_TXCSR_P_FRCTGL          (_ADI_MSK(0x00000800,uint16_t))  /* FRCDATATGL: Toggle Endpoint Data */

#define BITM_USB_EPI_TXCSR_P_DMAREQMODE      (_ADI_MSK(0x00000400,uint16_t))  /* DMA Mode Select */
#define ENUM_USB_EPI_TXCSR_P_DMARQMODE0      (_ADI_MSK(0x00000000,uint16_t))  /* DMAREQMODE: DMA Request Mode 0 */
#define ENUM_USB_EPI_TXCSR_P_DMARQMODE1      (_ADI_MSK(0x00000400,uint16_t))  /* DMAREQMODE: DMA Request Mode 1 */

#define BITM_USB_EPI_TXCSR_P_INCOMPTX        (_ADI_MSK(0x00000080,uint16_t))  /* Incomplete Tx */
#define ENUM_USB_EPI_TXCSR_P_NO_INCOMP       (_ADI_MSK(0x00000000,uint16_t))  /* INCOMPTX: No Status */
#define ENUM_USB_EPI_TXCSR_P_INCOMP          (_ADI_MSK(0x00000080,uint16_t))  /* INCOMPTX: Incomplete Tx (Insufficient IN Tokens) */

#define BITM_USB_EPI_TXCSR_P_CLRDATATGL      (_ADI_MSK(0x00000040,uint16_t))  /* Clear Endpoint Data Toggle */
#define ENUM_USB_EPI_TXCSR_P_NO_CLRTGL       (_ADI_MSK(0x00000000,uint16_t))  /* CLRDATATGL: No Action */
#define ENUM_USB_EPI_TXCSR_P_CLRTGL          (_ADI_MSK(0x00000040,uint16_t))  /* CLRDATATGL: Reset EP Data Toggle to 0 */

#define BITM_USB_EPI_TXCSR_P_SENTSTALL       (_ADI_MSK(0x00000020,uint16_t))  /* Sent STALL */
#define ENUM_USB_EPI_TXCSR_P_NO_STALSNT      (_ADI_MSK(0x00000000,uint16_t))  /* SENTSTALL: No Status */
#define ENUM_USB_EPI_TXCSR_P_STALSNT         (_ADI_MSK(0x00000020,uint16_t))  /* SENTSTALL: STALL Handshake Transmitted */

#define BITM_USB_EPI_TXCSR_P_SENDSTALL       (_ADI_MSK(0x00000010,uint16_t))  /* Send STALL */
#define ENUM_USB_EPI_TXCSR_P_NO_STALL        (_ADI_MSK(0x00000000,uint16_t))  /* SENDSTALL: No Request */
#define ENUM_USB_EPI_TXCSR_P_STALL           (_ADI_MSK(0x00000010,uint16_t))  /* SENDSTALL: Request STALL Handshake Transmission */

#define BITM_USB_EPI_TXCSR_P_FLUSHFIFO       (_ADI_MSK(0x00000008,uint16_t))  /* Flush Endpoint FIFO */
#define ENUM_USB_EPI_TXCSR_P_NO_FLUSH        (_ADI_MSK(0x00000000,uint16_t))  /* FLUSHFIFO: No Flush */
#define ENUM_USB_EPI_TXCSR_P_FLUSH           (_ADI_MSK(0x00000008,uint16_t))  /* FLUSHFIFO: Flush endpoint FIFO */

#define BITM_USB_EPI_TXCSR_P_URUNERR         (_ADI_MSK(0x00000004,uint16_t))  /* Underrun Error */
#define ENUM_USB_EPI_TXCSR_P_NO_URUNERR      (_ADI_MSK(0x00000000,uint16_t))  /* URUNERR: No Status */
#define ENUM_USB_EPI_TXCSR_P_URUNERR         (_ADI_MSK(0x00000004,uint16_t))  /* URUNERR: Underrun Error */

#define BITM_USB_EPI_TXCSR_P_NEFIFO          (_ADI_MSK(0x00000002,uint16_t))  /* Not Empty FIFO */
#define ENUM_USB_EPI_TXCSR_P_NO_FIFONE       (_ADI_MSK(0x00000000,uint16_t))  /* NEFIFO: FIFO Empty */
#define ENUM_USB_EPI_TXCSR_P_FIFONE          (_ADI_MSK(0x00000002,uint16_t))  /* NEFIFO: FIFO Not Empty */

#define BITM_USB_EPI_TXCSR_P_TXPKTRDY        (_ADI_MSK(0x00000001,uint16_t))  /* Tx Packet Ready */
#define ENUM_USB_EPI_TXCSR_P_NO_PKTRDY       (_ADI_MSK(0x00000000,uint16_t))  /* TXPKTRDY: No Tx Packet */
#define ENUM_USB_EPI_TXCSR_P_PKTRDY          (_ADI_MSK(0x00000001,uint16_t))  /* TXPKTRDY: Tx Packet in Endpoint FIFO */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_EPI_TXCSR_H                      Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_EPI_TXCSR_H_AUTOSET         15                               /* TxPkRdy Autoset Enable */
#define BITP_USB_EPI_TXCSR_H_DMAREQEN        12                               /* DMA Request Enable Tx EP */
#define BITP_USB_EPI_TXCSR_H_FRCDATATGL      11                               /* Force Data Toggle */
#define BITP_USB_EPI_TXCSR_H_DMAREQMODE      10                               /* DMA Mode Select */
#define BITP_USB_EPI_TXCSR_H_DATGLEN          9                               /* Data Toggle Write Enable */
#define BITP_USB_EPI_TXCSR_H_DATGL            8                               /* Data Toggle */
#define BITP_USB_EPI_TXCSR_H_NAKTOINCMP       7                               /* NAK Timeout Incomplete */
#define BITP_USB_EPI_TXCSR_H_CLRDATATGL       6                               /* Clear Endpoint Data Toggle */
#define BITP_USB_EPI_TXCSR_H_RXSTALL          5                               /* Rx STALL */
#define BITP_USB_EPI_TXCSR_H_SETUPPKT         4                               /* Setup Packet */
#define BITP_USB_EPI_TXCSR_H_FLUSHFIFO        3                               /* Flush Endpoint FIFO */
#define BITP_USB_EPI_TXCSR_H_TXTOERR          2                               /* Tx Timeout Error */
#define BITP_USB_EPI_TXCSR_H_NEFIFO           1                               /* Not Empty FIFO */
#define BITP_USB_EPI_TXCSR_H_TXPKTRDY         0                               /* Tx Packet Ready */

#define BITM_USB_EPI_TXCSR_H_AUTOSET         (_ADI_MSK(0x00008000,uint16_t))  /* TxPkRdy Autoset Enable */
#define ENUM_USB_EPI_TXCSR_H_NO_AUTOSET      (_ADI_MSK(0x00000000,uint16_t))  /* AUTOSET: Disable Autoset */
#define ENUM_USB_EPI_TXCSR_H_AUTOSET         (_ADI_MSK(0x00008000,uint16_t))  /* AUTOSET: Enable Autoset */

#define BITM_USB_EPI_TXCSR_H_DMAREQEN        (_ADI_MSK(0x00001000,uint16_t))  /* DMA Request Enable Tx EP */
#define ENUM_USB_EPI_TXCSR_H_DMAREQDIS       (_ADI_MSK(0x00000000,uint16_t))  /* DMAREQEN: Disable DMA Request */
#define ENUM_USB_EPI_TXCSR_H_DMAREQEN        (_ADI_MSK(0x00001000,uint16_t))  /* DMAREQEN: Enable DMA Request */

#define BITM_USB_EPI_TXCSR_H_FRCDATATGL      (_ADI_MSK(0x00000800,uint16_t))  /* Force Data Toggle */
#define ENUM_USB_EPI_TXCSR_H_NO_FRCTGL       (_ADI_MSK(0x00000000,uint16_t))  /* FRCDATATGL: No Action */
#define ENUM_USB_EPI_TXCSR_H_FRCTGL          (_ADI_MSK(0x00000800,uint16_t))  /* FRCDATATGL: Toggle Endpoint Data */

#define BITM_USB_EPI_TXCSR_H_DMAREQMODE      (_ADI_MSK(0x00000400,uint16_t))  /* DMA Mode Select */
#define ENUM_USB_EPI_TXCSR_H_DMARQMODE0      (_ADI_MSK(0x00000000,uint16_t))  /* DMAREQMODE: DMA Request Mode 0 */
#define ENUM_USB_EPI_TXCSR_H_DMARQMODE1      (_ADI_MSK(0x00000400,uint16_t))  /* DMAREQMODE: DMA Request Mode 1 */

#define BITM_USB_EPI_TXCSR_H_DATGLEN         (_ADI_MSK(0x00000200,uint16_t))  /* Data Toggle Write Enable */
#define ENUM_USB_EPI_TXCSR_H_NO_DATGLEN      (_ADI_MSK(0x00000000,uint16_t))  /* DATGLEN: Disable Write to DATGL */
#define ENUM_USB_EPI_TXCSR_H_DATGLEN         (_ADI_MSK(0x00000200,uint16_t))  /* DATGLEN: Enable Write to DATGL */

#define BITM_USB_EPI_TXCSR_H_DATGL           (_ADI_MSK(0x00000100,uint16_t))  /* Data Toggle */
#define ENUM_USB_EPI_TXCSR_H_NO_DATGL        (_ADI_MSK(0x00000000,uint16_t))  /* DATGL: DATA0 is set */
#define ENUM_USB_EPI_TXCSR_H_DATGL           (_ADI_MSK(0x00000100,uint16_t))  /* DATGL: DATA1 is set */

#define BITM_USB_EPI_TXCSR_H_NAKTOINCMP      (_ADI_MSK(0x00000080,uint16_t))  /* NAK Timeout Incomplete */
#define ENUM_USB_EPI_TXCSR_H_NO_NAKTO        (_ADI_MSK(0x00000000,uint16_t))  /* NAKTOINCMP: No Status */
#define ENUM_USB_EPI_TXCSR_H_NAKTO           (_ADI_MSK(0x00000080,uint16_t))  /* NAKTOINCMP: NAK Timeout Over Maximum */

#define BITM_USB_EPI_TXCSR_H_CLRDATATGL      (_ADI_MSK(0x00000040,uint16_t))  /* Clear Endpoint Data Toggle */
#define ENUM_USB_EPI_TXCSR_H_NO_CLRTGL       (_ADI_MSK(0x00000000,uint16_t))  /* CLRDATATGL: No Action */
#define ENUM_USB_EPI_TXCSR_H_CLRTGL          (_ADI_MSK(0x00000040,uint16_t))  /* CLRDATATGL: Reset EP Data Toggle to 0 */

#define BITM_USB_EPI_TXCSR_H_RXSTALL         (_ADI_MSK(0x00000020,uint16_t))  /* Rx STALL */
#define ENUM_USB_EPI_TXCSR_H_NO_RXSTALL      (_ADI_MSK(0x00000000,uint16_t))  /* RXSTALL: No Status */
#define ENUM_USB_EPI_TXCSR_H_RXSTALL         (_ADI_MSK(0x00000020,uint16_t))  /* RXSTALL: Stall Received from Device */

#define BITM_USB_EPI_TXCSR_H_SETUPPKT        (_ADI_MSK(0x00000010,uint16_t))  /* Setup Packet */
#define ENUM_USB_EPI_TXCSR_H_NO_SETUPPK      (_ADI_MSK(0x00000000,uint16_t))  /* SETUPPKT: No Request */
#define ENUM_USB_EPI_TXCSR_H_SETUPPKT        (_ADI_MSK(0x00000010,uint16_t))  /* SETUPPKT: Send SETUP Token */

#define BITM_USB_EPI_TXCSR_H_FLUSHFIFO       (_ADI_MSK(0x00000008,uint16_t))  /* Flush Endpoint FIFO */
#define ENUM_USB_EPI_TXCSR_H_NO_FLUSH        (_ADI_MSK(0x00000000,uint16_t))  /* FLUSHFIFO: No Flush */
#define ENUM_USB_EPI_TXCSR_H_FLUSH           (_ADI_MSK(0x00000008,uint16_t))  /* FLUSHFIFO: Flush endpoint FIFO */

#define BITM_USB_EPI_TXCSR_H_TXTOERR         (_ADI_MSK(0x00000004,uint16_t))  /* Tx Timeout Error */
#define ENUM_USB_EPI_TXCSR_H_NO_TXTOERR      (_ADI_MSK(0x00000000,uint16_t))  /* TXTOERR: No Status */
#define ENUM_USB_EPI_TXCSR_H_TXTOERR         (_ADI_MSK(0x00000004,uint16_t))  /* TXTOERR: Tx Timeout Error */

#define BITM_USB_EPI_TXCSR_H_NEFIFO          (_ADI_MSK(0x00000002,uint16_t))  /* Not Empty FIFO */
#define ENUM_USB_EPI_TXCSR_H_NO_NEFIFO       (_ADI_MSK(0x00000000,uint16_t))  /* NEFIFO: FIFO Empty */
#define ENUM_USB_EPI_TXCSR_H_NEFIFO          (_ADI_MSK(0x00000002,uint16_t))  /* NEFIFO: FIFO Not Empty */

#define BITM_USB_EPI_TXCSR_H_TXPKTRDY        (_ADI_MSK(0x00000001,uint16_t))  /* Tx Packet Ready */
#define ENUM_USB_EPI_TXCSR_H_NO_PKTRDY       (_ADI_MSK(0x00000000,uint16_t))  /* TXPKTRDY: No Tx Packet */
#define ENUM_USB_EPI_TXCSR_H_PKTRDY          (_ADI_MSK(0x00000001,uint16_t))  /* TXPKTRDY: Tx Packet in Endpoint FIFO */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_EP0I_CSR_P                       Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_EP0I_CSR_P_FLUSHFIFO         8                               /* Flush Endpoint FIFO */
#define BITP_USB_EP0I_CSR_P_SSETUPEND         7                               /* Service Setup End */
#define BITP_USB_EP0I_CSR_P_SPKTRDY           6                               /* Service Rx Packet Ready */
#define BITP_USB_EP0I_CSR_P_SENDSTALL         5                               /* Send Stall */
#define BITP_USB_EP0I_CSR_P_SETUPEND          4                               /* Setup End */
#define BITP_USB_EP0I_CSR_P_DATAEND           3                               /* Data End */
#define BITP_USB_EP0I_CSR_P_SENTSTALL         2                               /* Sent Stall */
#define BITP_USB_EP0I_CSR_P_TXPKTRDY          1                               /* Tx Packet Ready */
#define BITP_USB_EP0I_CSR_P_RXPKTRDY          0                               /* Rx Packet Ready */

#define BITM_USB_EP0I_CSR_P_FLUSHFIFO        (_ADI_MSK(0x00000100,uint16_t))  /* Flush Endpoint FIFO */
#define ENUM_USB_EP0I_CSR_P_NO_FLUSH         (_ADI_MSK(0x00000000,uint16_t))  /* FLUSHFIFO: No Flush */
#define ENUM_USB_EP0I_CSR_P_FLUSH            (_ADI_MSK(0x00000100,uint16_t))  /* FLUSHFIFO: Flush Endpoint FIFO */

#define BITM_USB_EP0I_CSR_P_SSETUPEND        (_ADI_MSK(0x00000080,uint16_t))  /* Service Setup End */
#define ENUM_USB_EP0I_CSR_P_NOSSETUPEND      (_ADI_MSK(0x00000000,uint16_t))  /* SSETUPEND: No Action */
#define ENUM_USB_EP0I_CSR_P_SSETUPEND        (_ADI_MSK(0x00000080,uint16_t))  /* SSETUPEND: Clear SETUPEND Bit */

#define BITM_USB_EP0I_CSR_P_SPKTRDY          (_ADI_MSK(0x00000040,uint16_t))  /* Service Rx Packet Ready */
#define ENUM_USB_EP0I_CSR_P_NO_SPKTRDY       (_ADI_MSK(0x00000000,uint16_t))  /* SPKTRDY: No Action */
#define ENUM_USB_EP0I_CSR_P_SPKTRDY          (_ADI_MSK(0x00000040,uint16_t))  /* SPKTRDY: Clear RXPKTRDY Bit */

#define BITM_USB_EP0I_CSR_P_SENDSTALL        (_ADI_MSK(0x00000020,uint16_t))  /* Send Stall */
#define ENUM_USB_EP0I_CSR_P_NO_STALL         (_ADI_MSK(0x00000000,uint16_t))  /* SENDSTALL: No Action */
#define ENUM_USB_EP0I_CSR_P_STALL            (_ADI_MSK(0x00000020,uint16_t))  /* SENDSTALL: Terminate Current Transaction */

#define BITM_USB_EP0I_CSR_P_SETUPEND         (_ADI_MSK(0x00000010,uint16_t))  /* Setup End */
#define ENUM_USB_EP0I_CSR_P_NO_SETUPEND      (_ADI_MSK(0x00000000,uint16_t))  /* SETUPEND: No Status */
#define ENUM_USB_EP0I_CSR_P_SETUPEND         (_ADI_MSK(0x00000010,uint16_t))  /* SETUPEND: Setup Ended before DATAEND */

#define BITM_USB_EP0I_CSR_P_DATAEND          (_ADI_MSK(0x00000008,uint16_t))  /* Data End */
#define ENUM_USB_EP0I_CSR_P_NO_DATAEND       (_ADI_MSK(0x00000000,uint16_t))  /* DATAEND: No Status */
#define ENUM_USB_EP0I_CSR_P_DATAEND          (_ADI_MSK(0x00000008,uint16_t))  /* DATAEND: Data End Condition */

#define BITM_USB_EP0I_CSR_P_SENTSTALL        (_ADI_MSK(0x00000004,uint16_t))  /* Sent Stall */
#define ENUM_USB_EP0I_CSR_P_NO_STALSNT       (_ADI_MSK(0x00000000,uint16_t))  /* SENTSTALL: No Status */
#define ENUM_USB_EP0I_CSR_P_STALSNT          (_ADI_MSK(0x00000004,uint16_t))  /* SENTSTALL: Transmitted STALL Handshake */

#define BITM_USB_EP0I_CSR_P_TXPKTRDY         (_ADI_MSK(0x00000002,uint16_t))  /* Tx Packet Ready */
#define ENUM_USB_EP0I_CSR_P_NO_TXPKTRDY      (_ADI_MSK(0x00000000,uint16_t))  /* TXPKTRDY:  */
#define ENUM_USB_EP0I_CSR_P_TXPKTRDY         (_ADI_MSK(0x00000002,uint16_t))  /* TXPKTRDY: Set this bit after loading a data packet into the FIFO */

#define BITM_USB_EP0I_CSR_P_RXPKTRDY         (_ADI_MSK(0x00000001,uint16_t))  /* Rx Packet Ready */
#define ENUM_USB_EP0I_CSR_P_NO_PKTRDY        (_ADI_MSK(0x00000000,uint16_t))  /* RXPKTRDY: No Rx Packet */
#define ENUM_USB_EP0I_CSR_P_PKTRDY           (_ADI_MSK(0x00000001,uint16_t))  /* RXPKTRDY: Rx Packet in Endpoint FIFO */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_EP0I_CSR_H                       Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_EP0I_CSR_H_DISPING          11                               /* Disable Ping */
#define BITP_USB_EP0I_CSR_H_DATGLEN          10                               /* Data Toggle Write Enable */
#define BITP_USB_EP0I_CSR_H_DATGL             9                               /* Data Toggle */
#define BITP_USB_EP0I_CSR_H_FLUSHFIFO         8                               /* Flush Endpoint FIFO */
#define BITP_USB_EP0I_CSR_H_NAKTO             7                               /* NAK Timeout */
#define BITP_USB_EP0I_CSR_H_STATUSPKT         6                               /* Status Packet */
#define BITP_USB_EP0I_CSR_H_REQPKT            5                               /* Request Packet */
#define BITP_USB_EP0I_CSR_H_TOERR             4                               /* Timeout Error */
#define BITP_USB_EP0I_CSR_H_SETUPPKT          3                               /* Setup Packet */
#define BITP_USB_EP0I_CSR_H_RXSTALL           2                               /* Rx Stall */
#define BITP_USB_EP0I_CSR_H_TXPKTRDY          1                               /* Tx Packet Ready */
#define BITP_USB_EP0I_CSR_H_RXPKTRDY          0                               /* Rx Packet Ready */

#define BITM_USB_EP0I_CSR_H_DISPING          (_ADI_MSK(0x00000800,uint16_t))  /* Disable Ping */
#define ENUM_USB_EP0I_CSR_H_NO_DISPING       (_ADI_MSK(0x00000000,uint16_t))  /* DISPING: Issue PING tokens */
#define ENUM_USB_EP0I_CSR_H_DISPING          (_ADI_MSK(0x00000800,uint16_t))  /* DISPING: Do not issue PING */

#define BITM_USB_EP0I_CSR_H_DATGLEN          (_ADI_MSK(0x00000400,uint16_t))  /* Data Toggle Write Enable */
#define ENUM_USB_EP0I_CSR_H_NO_DATGLEN       (_ADI_MSK(0x00000000,uint16_t))  /* DATGLEN: Disable Write to DATGL */
#define ENUM_USB_EP0I_CSR_H_DATGLEN          (_ADI_MSK(0x00000400,uint16_t))  /* DATGLEN: Enable Write to DATGL */

#define BITM_USB_EP0I_CSR_H_DATGL            (_ADI_MSK(0x00000200,uint16_t))  /* Data Toggle */
#define ENUM_USB_EP0I_CSR_H_NO_DATATGL       (_ADI_MSK(0x00000000,uint16_t))  /* DATGL: DATA0 is Set */
#define ENUM_USB_EP0I_CSR_H_DATATGL          (_ADI_MSK(0x00000200,uint16_t))  /* DATGL: DATA1 is Set */

#define BITM_USB_EP0I_CSR_H_FLUSHFIFO        (_ADI_MSK(0x00000100,uint16_t))  /* Flush Endpoint FIFO */
#define ENUM_USB_EP0I_CSR_H_NO_FLUSH         (_ADI_MSK(0x00000000,uint16_t))  /* FLUSHFIFO: No Flush */
#define ENUM_USB_EP0I_CSR_H_FLUSH            (_ADI_MSK(0x00000100,uint16_t))  /* FLUSHFIFO: Flush Endpoint FIFO */

#define BITM_USB_EP0I_CSR_H_NAKTO            (_ADI_MSK(0x00000080,uint16_t))  /* NAK Timeout */
#define ENUM_USB_EP0I_CSR_H_NO_NAKTO         (_ADI_MSK(0x00000000,uint16_t))  /* NAKTO: No Status */
#define ENUM_USB_EP0I_CSR_H_NAKTO            (_ADI_MSK(0x00000080,uint16_t))  /* NAKTO: Endpoint Halted (NAK Timeout) */

#define BITM_USB_EP0I_CSR_H_STATUSPKT        (_ADI_MSK(0x00000040,uint16_t))  /* Status Packet */
#define ENUM_USB_EP0I_CSR_H_NO_STATPKT       (_ADI_MSK(0x00000000,uint16_t))  /* STATUSPKT: No Request */
#define ENUM_USB_EP0I_CSR_H_STATPKT          (_ADI_MSK(0x00000040,uint16_t))  /* STATUSPKT: Request Status Transaction */

#define BITM_USB_EP0I_CSR_H_REQPKT           (_ADI_MSK(0x00000020,uint16_t))  /* Request Packet */
#define ENUM_USB_EP0I_CSR_H_NO_REQPKT        (_ADI_MSK(0x00000000,uint16_t))  /* REQPKT: No Request */
#define ENUM_USB_EP0I_CSR_H_REQPKT           (_ADI_MSK(0x00000020,uint16_t))  /* REQPKT: Send IN Tokens to Device */

#define BITM_USB_EP0I_CSR_H_TOERR            (_ADI_MSK(0x00000010,uint16_t))  /* Timeout Error */
#define ENUM_USB_EP0I_CSR_H_NO_TOERR         (_ADI_MSK(0x00000000,uint16_t))  /* TOERR: No Status */
#define ENUM_USB_EP0I_CSR_H_TOERR            (_ADI_MSK(0x00000010,uint16_t))  /* TOERR: Timeout Error */

#define BITM_USB_EP0I_CSR_H_SETUPPKT         (_ADI_MSK(0x00000008,uint16_t))  /* Setup Packet */
#define ENUM_USB_EP0I_CSR_H_NO_SETUPPKT      (_ADI_MSK(0x00000000,uint16_t))  /* SETUPPKT: No Request */
#define ENUM_USB_EP0I_CSR_H_SETUPPKT         (_ADI_MSK(0x00000008,uint16_t))  /* SETUPPKT: Send SETUP token */

#define BITM_USB_EP0I_CSR_H_RXSTALL          (_ADI_MSK(0x00000004,uint16_t))  /* Rx Stall */
#define ENUM_USB_EP0I_CSR_H_NO_RXSTALL       (_ADI_MSK(0x00000000,uint16_t))  /* RXSTALL: No Status */
#define ENUM_USB_EP0I_CSR_H_RXSTALL          (_ADI_MSK(0x00000004,uint16_t))  /* RXSTALL: Stall Received from Device */

#define BITM_USB_EP0I_CSR_H_TXPKTRDY         (_ADI_MSK(0x00000002,uint16_t))  /* Tx Packet Ready */
#define ENUM_USB_EP0I_CSR_H_NO_TXPKTRDY      (_ADI_MSK(0x00000000,uint16_t))  /* TXPKTRDY: No Tx Packet */
#define ENUM_USB_EP0I_CSR_H_TXPKTRDY         (_ADI_MSK(0x00000002,uint16_t))  /* TXPKTRDY: Tx Packet in Endpoint FIFO */

#define BITM_USB_EP0I_CSR_H_RXPKTRDY         (_ADI_MSK(0x00000001,uint16_t))  /* Rx Packet Ready */
#define ENUM_USB_EP0I_CSR_H_NO_RXPKTRDY      (_ADI_MSK(0x00000000,uint16_t))  /* RXPKTRDY: No Rx Packet */
#define ENUM_USB_EP0I_CSR_H_RXPKTRDY         (_ADI_MSK(0x00000001,uint16_t))  /* RXPKTRDY: Rx Packet in Endpoint FIFO */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_EPI_RXMAXP                       Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_EPI_RXMAXP_MULTM1           11                               /* Multi-Packets per Micro-frame */
#define BITP_USB_EPI_RXMAXP_MAXPAY            0                               /* Maximum Payload */
#define BITM_USB_EPI_RXMAXP_MULTM1           (_ADI_MSK(0x00001800,uint16_t))  /* Multi-Packets per Micro-frame */
#define BITM_USB_EPI_RXMAXP_MAXPAY           (_ADI_MSK(0x000007FF,uint16_t))  /* Maximum Payload */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_EPI_RXCSR_H                      Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_EPI_RXCSR_H_AUTOCLR         15                               /* Auto Clear Enable */
#define BITP_USB_EPI_RXCSR_H_AUTOREQ         14                               /* Auto Request Clear Enable */
#define BITP_USB_EPI_RXCSR_H_DMAREQEN        13                               /* DMA Request Enable Rx EP */
#define BITP_USB_EPI_RXCSR_H_PIDERR          12                               /* Packet ID Error */
#define BITP_USB_EPI_RXCSR_H_DMAREQMODE      11                               /* DMA Mode Select */
#define BITP_USB_EPI_RXCSR_H_DATGLEN         10                               /* Data Toggle Write Enable */
#define BITP_USB_EPI_RXCSR_H_DATGL            9                               /* Data Toggle */
#define BITP_USB_EPI_RXCSR_H_INCOMPRX         8                               /* Incomplete Rx */
#define BITP_USB_EPI_RXCSR_H_CLRDATATGL       7                               /* Clear Endpoint Data Toggle */
#define BITP_USB_EPI_RXCSR_H_RXSTALL          6                               /* Rx STALL */
#define BITP_USB_EPI_RXCSR_H_REQPKT           5                               /* Request Packet */
#define BITP_USB_EPI_RXCSR_H_FLUSHFIFO        4                               /* Flush Endpoint FIFO */
#define BITP_USB_EPI_RXCSR_H_NAKTODERR        3                               /* NAK Timeout Data Error */
#define BITP_USB_EPI_RXCSR_H_RXTOERR          2                               /* Rx Timeout Error */
#define BITP_USB_EPI_RXCSR_H_FIFOFULL         1                               /* FIFO Full */
#define BITP_USB_EPI_RXCSR_H_RXPKTRDY         0                               /* Rx Packet Ready */

#define BITM_USB_EPI_RXCSR_H_AUTOCLR         (_ADI_MSK(0x00008000,uint16_t))  /* Auto Clear Enable */
#define ENUM_USB_EPI_RXCSR_H_NO_AUTOCLR      (_ADI_MSK(0x00000000,uint16_t))  /* AUTOCLR: Disable Auto Clear */
#define ENUM_USB_EPI_RXCSR_H_AUTOCLR         (_ADI_MSK(0x00008000,uint16_t))  /* AUTOCLR: Enable Auto Clear */

#define BITM_USB_EPI_RXCSR_H_AUTOREQ         (_ADI_MSK(0x00004000,uint16_t))  /* Auto Request Clear Enable */
#define ENUM_USB_EPI_RXCSR_H_NO_AUTOREQ      (_ADI_MSK(0x00000000,uint16_t))  /* AUTOREQ: Disable Auto Request Clear */
#define ENUM_USB_EPI_RXCSR_H_AUTOREQ         (_ADI_MSK(0x00004000,uint16_t))  /* AUTOREQ: Enable Auto Request Clear */

#define BITM_USB_EPI_RXCSR_H_DMAREQEN        (_ADI_MSK(0x00002000,uint16_t))  /* DMA Request Enable Rx EP */
#define ENUM_USB_EPI_RXCSR_H_DMAREQDIS       (_ADI_MSK(0x00000000,uint16_t))  /* DMAREQEN: Disable DMA Request */
#define ENUM_USB_EPI_RXCSR_H_DMAREQEN        (_ADI_MSK(0x00002000,uint16_t))  /* DMAREQEN: Enable DMA Request */

#define BITM_USB_EPI_RXCSR_H_PIDERR          (_ADI_MSK(0x00001000,uint16_t))  /* Packet ID Error */
#define ENUM_USB_EPI_RXCSR_H_NO_PIDERR       (_ADI_MSK(0x00000000,uint16_t))  /* PIDERR: No Status */
#define ENUM_USB_EPI_RXCSR_H_PIDERR          (_ADI_MSK(0x00001000,uint16_t))  /* PIDERR: PID Error */

#define BITM_USB_EPI_RXCSR_H_DMAREQMODE      (_ADI_MSK(0x00000800,uint16_t))  /* DMA Mode Select */
#define ENUM_USB_EPI_RXCSR_H_DMARQMODE0      (_ADI_MSK(0x00000000,uint16_t))  /* DMAREQMODE: DMA Request Mode 0 */
#define ENUM_USB_EPI_RXCSR_H_DMARQMODE1      (_ADI_MSK(0x00000800,uint16_t))  /* DMAREQMODE: DMA Request Mode 1 */

#define BITM_USB_EPI_RXCSR_H_DATGLEN         (_ADI_MSK(0x00000400,uint16_t))  /* Data Toggle Write Enable */
#define ENUM_USB_EPI_RXCSR_H_DATGLDIS        (_ADI_MSK(0x00000000,uint16_t))  /* DATGLEN: Disable Write to DATGL */
#define ENUM_USB_EPI_RXCSR_H_DATGLEN         (_ADI_MSK(0x00000400,uint16_t))  /* DATGLEN: Enable Write to DATGL */

#define BITM_USB_EPI_RXCSR_H_DATGL           (_ADI_MSK(0x00000200,uint16_t))  /* Data Toggle */
#define ENUM_USB_EPI_RXCSR_H_NO_DATGL        (_ADI_MSK(0x00000000,uint16_t))  /* DATGL: DATA0 is Set */
#define ENUM_USB_EPI_RXCSR_H_DATGL           (_ADI_MSK(0x00000200,uint16_t))  /* DATGL: DATA1 is Set */

#define BITM_USB_EPI_RXCSR_H_INCOMPRX        (_ADI_MSK(0x00000100,uint16_t))  /* Incomplete Rx */
#define ENUM_USB_EPI_RXCSR_H_NO_INCOMP       (_ADI_MSK(0x00000000,uint16_t))  /* INCOMPRX: No Status */
#define ENUM_USB_EPI_RXCSR_H_INCOMP          (_ADI_MSK(0x00000100,uint16_t))  /* INCOMPRX: Incomplete Rx */

#define BITM_USB_EPI_RXCSR_H_CLRDATATGL      (_ADI_MSK(0x00000080,uint16_t))  /* Clear Endpoint Data Toggle */
#define ENUM_USB_EPI_RXCSR_H_NO_CLRTGL       (_ADI_MSK(0x00000000,uint16_t))  /* CLRDATATGL: No Action */
#define ENUM_USB_EPI_RXCSR_H_CLRTGL          (_ADI_MSK(0x00000080,uint16_t))  /* CLRDATATGL: Reset EP Data Toggle to 0 */

#define BITM_USB_EPI_RXCSR_H_RXSTALL         (_ADI_MSK(0x00000040,uint16_t))  /* Rx STALL */
#define ENUM_USB_EPI_RXCSR_H_NO_RXSTALL      (_ADI_MSK(0x00000000,uint16_t))  /* RXSTALL: No Status */
#define ENUM_USB_EPI_RXCSR_H_RXSTALL         (_ADI_MSK(0x00000040,uint16_t))  /* RXSTALL: Stall Received from Device */

#define BITM_USB_EPI_RXCSR_H_REQPKT          (_ADI_MSK(0x00000020,uint16_t))  /* Request Packet */
#define ENUM_USB_EPI_RXCSR_H_NO_REQPKT       (_ADI_MSK(0x00000000,uint16_t))  /* REQPKT: No Request */
#define ENUM_USB_EPI_RXCSR_H_REQPKT          (_ADI_MSK(0x00000020,uint16_t))  /* REQPKT: Send IN Tokens to Device */

#define BITM_USB_EPI_RXCSR_H_FLUSHFIFO       (_ADI_MSK(0x00000010,uint16_t))  /* Flush Endpoint FIFO */
#define ENUM_USB_EPI_RXCSR_H_NO_FLUSH        (_ADI_MSK(0x00000000,uint16_t))  /* FLUSHFIFO: No Flush */
#define ENUM_USB_EPI_RXCSR_H_FLUSH           (_ADI_MSK(0x00000010,uint16_t))  /* FLUSHFIFO: Flush Endpoint FIFO */

#define BITM_USB_EPI_RXCSR_H_NAKTODERR       (_ADI_MSK(0x00000008,uint16_t))  /* NAK Timeout Data Error */
#define ENUM_USB_EPI_RXCSR_H_NO_NAKTO        (_ADI_MSK(0x00000000,uint16_t))  /* NAKTODERR: No Status */
#define ENUM_USB_EPI_RXCSR_H_NAKTO           (_ADI_MSK(0x00000008,uint16_t))  /* NAKTODERR: NAK Timeout Data Error */

#define BITM_USB_EPI_RXCSR_H_RXTOERR         (_ADI_MSK(0x00000004,uint16_t))  /* Rx Timeout Error */
#define ENUM_USB_EPI_RXCSR_H_NO_RXTOERR      (_ADI_MSK(0x00000000,uint16_t))  /* RXTOERR: No Status */
#define ENUM_USB_EPI_RXCSR_H_RXTOERR         (_ADI_MSK(0x00000004,uint16_t))  /* RXTOERR: Rx Timeout Error */

#define BITM_USB_EPI_RXCSR_H_FIFOFULL        (_ADI_MSK(0x00000002,uint16_t))  /* FIFO Full */
#define ENUM_USB_EPI_RXCSR_H_NO_FIFOFUL      (_ADI_MSK(0x00000000,uint16_t))  /* FIFOFULL: No Status */
#define ENUM_USB_EPI_RXCSR_H_FIFOFUL         (_ADI_MSK(0x00000002,uint16_t))  /* FIFOFULL: FIFO Full */

#define BITM_USB_EPI_RXCSR_H_RXPKTRDY        (_ADI_MSK(0x00000001,uint16_t))  /* Rx Packet Ready */
#define ENUM_USB_EPI_RXCSR_H_NO_PKTRDY       (_ADI_MSK(0x00000000,uint16_t))  /* RXPKTRDY: No Rx Packet */
#define ENUM_USB_EPI_RXCSR_H_PKTRDY          (_ADI_MSK(0x00000001,uint16_t))  /* RXPKTRDY: Rx Packet in Endpoint FIFO */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_EPI_RXCSR_P                      Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_EPI_RXCSR_P_AUTOCLR         15                               /* Auto Clear Enable */
#define BITP_USB_EPI_RXCSR_P_ISO             14                               /* Isochronous Transfers */
#define BITP_USB_EPI_RXCSR_P_DMAREQEN        13                               /* DMA Request Enable Rx EP */
#define BITP_USB_EPI_RXCSR_P_DNYETPERR       12                               /* Disable NYET Handshake */
#define BITP_USB_EPI_RXCSR_P_DMAREQMODE      11                               /* DMA Mode Select */
#define BITP_USB_EPI_RXCSR_P_INCOMPRX         8                               /* Incomplete Rx */
#define BITP_USB_EPI_RXCSR_P_CLRDATATGL       7                               /* Clear Endpoint Data Toggle */
#define BITP_USB_EPI_RXCSR_P_SENTSTALL        6                               /* Sent STALL */
#define BITP_USB_EPI_RXCSR_P_SENDSTALL        5                               /* Send STALL */
#define BITP_USB_EPI_RXCSR_P_FLUSHFIFO        4                               /* Flush Endpoint FIFO */
#define BITP_USB_EPI_RXCSR_P_DATAERR          3                               /* Data Error */
#define BITP_USB_EPI_RXCSR_P_ORUNERR          2                               /* OUT Run Error */
#define BITP_USB_EPI_RXCSR_P_FIFOFULL         1                               /* FIFO Full */
#define BITP_USB_EPI_RXCSR_P_RXPKTRDY         0                               /* Rx Packet Ready */

#define BITM_USB_EPI_RXCSR_P_AUTOCLR         (_ADI_MSK(0x00008000,uint16_t))  /* Auto Clear Enable */
#define ENUM_USB_EPI_RXCSR_P_NO_AUTOCLR      (_ADI_MSK(0x00000000,uint16_t))  /* AUTOCLR: Disable Auto Clear */
#define ENUM_USB_EPI_RXCSR_P_AUTOCLR         (_ADI_MSK(0x00008000,uint16_t))  /* AUTOCLR: Enable Auto Clear */

#define BITM_USB_EPI_RXCSR_P_ISO             (_ADI_MSK(0x00004000,uint16_t))  /* Isochronous Transfers */
#define ENUM_USB_EPI_RXCSR_P_ISODIS          (_ADI_MSK(0x00000000,uint16_t))  /* ISO: This bit should be cleared for bulk or interrupt transfers. */
#define ENUM_USB_EPI_RXCSR_P_ISOEN           (_ADI_MSK(0x00004000,uint16_t))  /* ISO: This bit should be set for isochronous transfers. */

#define BITM_USB_EPI_RXCSR_P_DMAREQEN        (_ADI_MSK(0x00002000,uint16_t))  /* DMA Request Enable Rx EP */
#define ENUM_USB_EPI_RXCSR_P_DMAREQDIS       (_ADI_MSK(0x00000000,uint16_t))  /* DMAREQEN: Disable DMA Request */
#define ENUM_USB_EPI_RXCSR_P_DMAREQEN        (_ADI_MSK(0x00002000,uint16_t))  /* DMAREQEN: Enable DMA Request */

#define BITM_USB_EPI_RXCSR_P_DNYETPERR       (_ADI_MSK(0x00001000,uint16_t))  /* Disable NYET Handshake */
#define ENUM_USB_EPI_RXCSR_P_DNYTERREN       (_ADI_MSK(0x00000000,uint16_t))  /* DNYETPERR: Enable NYET Handshake */
#define ENUM_USB_EPI_RXCSR_P_DNYTERRDIS      (_ADI_MSK(0x00001000,uint16_t))  /* DNYETPERR: Disable NYET Handshake */

#define BITM_USB_EPI_RXCSR_P_DMAREQMODE      (_ADI_MSK(0x00000800,uint16_t))  /* DMA Mode Select */
#define ENUM_USB_EPI_RXCSR_P_DMARQMODE0      (_ADI_MSK(0x00000000,uint16_t))  /* DMAREQMODE: DMA Request Mode 0 */
#define ENUM_USB_EPI_RXCSR_P_DMARQMODE1      (_ADI_MSK(0x00000800,uint16_t))  /* DMAREQMODE: DMA Request Mode 1 */

#define BITM_USB_EPI_RXCSR_P_INCOMPRX        (_ADI_MSK(0x00000100,uint16_t))  /* Incomplete Rx */
#define ENUM_USB_EPI_RXCSR_P_NO_INCOMP       (_ADI_MSK(0x00000000,uint16_t))  /* INCOMPRX: No Status */
#define ENUM_USB_EPI_RXCSR_P_INCOMP          (_ADI_MSK(0x00000100,uint16_t))  /* INCOMPRX: Incomplete Rx */

#define BITM_USB_EPI_RXCSR_P_CLRDATATGL      (_ADI_MSK(0x00000080,uint16_t))  /* Clear Endpoint Data Toggle */
#define ENUM_USB_EPI_RXCSR_P_NO_CLRTGL       (_ADI_MSK(0x00000000,uint16_t))  /* CLRDATATGL: No Action */
#define ENUM_USB_EPI_RXCSR_P_CLRTGL          (_ADI_MSK(0x00000080,uint16_t))  /* CLRDATATGL: Reset EP Data Toggle to 0 */

#define BITM_USB_EPI_RXCSR_P_SENTSTALL       (_ADI_MSK(0x00000040,uint16_t))  /* Sent STALL */
#define ENUM_USB_EPI_RXCSR_P_NO_STALSNT      (_ADI_MSK(0x00000000,uint16_t))  /* SENTSTALL: No Status */
#define ENUM_USB_EPI_RXCSR_P_STALSNT         (_ADI_MSK(0x00000040,uint16_t))  /* SENTSTALL: STALL Handshake Transmitted */

#define BITM_USB_EPI_RXCSR_P_SENDSTALL       (_ADI_MSK(0x00000020,uint16_t))  /* Send STALL */
#define ENUM_USB_EPI_RXCSR_P_NO_STALL        (_ADI_MSK(0x00000000,uint16_t))  /* SENDSTALL: No Action */
#define ENUM_USB_EPI_RXCSR_P_STALL           (_ADI_MSK(0x00000020,uint16_t))  /* SENDSTALL: Request STALL Handshake */

#define BITM_USB_EPI_RXCSR_P_FLUSHFIFO       (_ADI_MSK(0x00000010,uint16_t))  /* Flush Endpoint FIFO */
#define ENUM_USB_EPI_RXCSR_P_NO_FLUSH        (_ADI_MSK(0x00000000,uint16_t))  /* FLUSHFIFO: No Flush */
#define ENUM_USB_EPI_RXCSR_P_FLUSH           (_ADI_MSK(0x00000010,uint16_t))  /* FLUSHFIFO: Flush Endpoint FIFO */

#define BITM_USB_EPI_RXCSR_P_DATAERR         (_ADI_MSK(0x00000008,uint16_t))  /* Data Error */
#define ENUM_USB_EPI_RXCSR_P_NO_DATAERR      (_ADI_MSK(0x00000000,uint16_t))  /* DATAERR: No Status */
#define ENUM_USB_EPI_RXCSR_P_DATAERR         (_ADI_MSK(0x00000008,uint16_t))  /* DATAERR: Data Error */

#define BITM_USB_EPI_RXCSR_P_ORUNERR         (_ADI_MSK(0x00000004,uint16_t))  /* OUT Run Error */
#define ENUM_USB_EPI_RXCSR_P_NO_ORUNERR      (_ADI_MSK(0x00000000,uint16_t))  /* ORUNERR: No Status */
#define ENUM_USB_EPI_RXCSR_P_ORUNERR         (_ADI_MSK(0x00000004,uint16_t))  /* ORUNERR: OUT Run Error */

#define BITM_USB_EPI_RXCSR_P_FIFOFULL        (_ADI_MSK(0x00000002,uint16_t))  /* FIFO Full */
#define ENUM_USB_EPI_RXCSR_P_NO_FIFOFUL      (_ADI_MSK(0x00000000,uint16_t))  /* FIFOFULL: No Status */
#define ENUM_USB_EPI_RXCSR_P_FIFOFUL         (_ADI_MSK(0x00000002,uint16_t))  /* FIFOFULL: FIFO Full */

#define BITM_USB_EPI_RXCSR_P_RXPKTRDY        (_ADI_MSK(0x00000001,uint16_t))  /* Rx Packet Ready */
#define ENUM_USB_EPI_RXCSR_P_NO_PKTRDY       (_ADI_MSK(0x00000000,uint16_t))  /* RXPKTRDY: No Rx Packet */
#define ENUM_USB_EPI_RXCSR_P_PKTRDY          (_ADI_MSK(0x00000001,uint16_t))  /* RXPKTRDY: Rx Packet in Endpoint FIFO */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_EP0I_CNT                         Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_EP0I_CNT_RXCNT               0                               /* Rx Byte Count Value */
#define BITM_USB_EP0I_CNT_RXCNT              (_ADI_MSK(0x0000007F,uint16_t))  /* Rx Byte Count Value */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_EPI_RXCNT                        Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_EPI_RXCNT_EPRXCNT            0                               /* EP Rx Count */
#define BITM_USB_EPI_RXCNT_EPRXCNT           (_ADI_MSK(0x00003FFF,uint16_t))  /* EP Rx Count */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_EPI_TXTYPE                       Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_EPI_TXTYPE_SPEED             6                               /* Speed of Operation Value */
#define BITP_USB_EPI_TXTYPE_PROTOCOL          4                               /* Protocol for Transfer */
#define BITP_USB_EPI_TXTYPE_TGTEP             0                               /* Target Endpoint Number */

#define BITM_USB_EPI_TXTYPE_SPEED            (_ADI_MSK(0x000000C0,uint8_t))   /* Speed of Operation Value */
#define ENUM_USB_EPI_TXTYPE_UNUSED           (_ADI_MSK(0x00000000,uint8_t))   /* SPEED: Same Speed as the Core */
#define ENUM_USB_EPI_TXTYPE_HIGHSPEED        (_ADI_MSK(0x00000040,uint8_t))   /* SPEED: High Speed */
#define ENUM_USB_EPI_TXTYPE_FULLSPEED        (_ADI_MSK(0x00000080,uint8_t))   /* SPEED: Full Speed */
#define ENUM_USB_EPI_TXTYPE_LOWSPEED         (_ADI_MSK(0x000000C0,uint8_t))   /* SPEED: Low Speed */

#define BITM_USB_EPI_TXTYPE_PROTOCOL         (_ADI_MSK(0x00000030,uint8_t))   /* Protocol for Transfer */
#define ENUM_USB_EPI_TXTYPE_CONTROL          (_ADI_MSK(0x00000000,uint8_t))   /* PROTOCOL: Control */
#define ENUM_USB_EPI_TXTYPE_ISO              (_ADI_MSK(0x00000010,uint8_t))   /* PROTOCOL: Isochronous */
#define ENUM_USB_EPI_TXTYPE_BULK             (_ADI_MSK(0x00000020,uint8_t))   /* PROTOCOL: Bulk */
#define ENUM_USB_EPI_TXTYPE_INT              (_ADI_MSK(0x00000030,uint8_t))   /* PROTOCOL: Interrupt */

#define BITM_USB_EPI_TXTYPE_TGTEP            (_ADI_MSK(0x0000000F,uint8_t))   /* Target Endpoint Number */
#define ENUM_USB_EPI_TXTYPE_TGTEP0           (_ADI_MSK(0x00000000,uint8_t))   /* TGTEP: Endpoint 0 */
#define ENUM_USB_EPI_TXTYPE_TGTEP1           (_ADI_MSK(0x00000001,uint8_t))   /* TGTEP: Endpoint 1 */
#define ENUM_USB_EPI_TXTYPE_TGTEP10          (_ADI_MSK(0x0000000A,uint8_t))   /* TGTEP: Endpoint 10 */
#define ENUM_USB_EPI_TXTYPE_TGTEP11          (_ADI_MSK(0x0000000B,uint8_t))   /* TGTEP: Endpoint 11 */
#define ENUM_USB_EPI_TXTYPE_TGTEP12          (_ADI_MSK(0x0000000C,uint8_t))   /* TGTEP: Endpoint 12 */
#define ENUM_USB_EPI_TXTYPE_TGTEP13          (_ADI_MSK(0x0000000D,uint8_t))   /* TGTEP: Endpoint 13 */
#define ENUM_USB_EPI_TXTYPE_TGTEP14          (_ADI_MSK(0x0000000E,uint8_t))   /* TGTEP: Endpoint 14 */
#define ENUM_USB_EPI_TXTYPE_TGTEP15          (_ADI_MSK(0x0000000F,uint8_t))   /* TGTEP: Endpoint 15 */
#define ENUM_USB_EPI_TXTYPE_TGTEP2           (_ADI_MSK(0x00000002,uint8_t))   /* TGTEP: Endpoint 2 */
#define ENUM_USB_EPI_TXTYPE_TGTEP3           (_ADI_MSK(0x00000003,uint8_t))   /* TGTEP: Endpoint 3 */
#define ENUM_USB_EPI_TXTYPE_TGTEP4           (_ADI_MSK(0x00000004,uint8_t))   /* TGTEP: Endpoint 4 */
#define ENUM_USB_EPI_TXTYPE_TGTEP5           (_ADI_MSK(0x00000005,uint8_t))   /* TGTEP: Endpoint 5 */
#define ENUM_USB_EPI_TXTYPE_TGTEP6           (_ADI_MSK(0x00000006,uint8_t))   /* TGTEP: Endpoint 6 */
#define ENUM_USB_EPI_TXTYPE_TGTEP7           (_ADI_MSK(0x00000007,uint8_t))   /* TGTEP: Endpoint 7 */
#define ENUM_USB_EPI_TXTYPE_TGTEP8           (_ADI_MSK(0x00000008,uint8_t))   /* TGTEP: Endpoint 8 */
#define ENUM_USB_EPI_TXTYPE_TGTEP9           (_ADI_MSK(0x00000009,uint8_t))   /* TGTEP: Endpoint 9 */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_EP0I_TYPE                        Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_EP0I_TYPE_SPEED              0                               /* Speed of Operation Value */
#define BITM_USB_EP0I_TYPE_SPEED             (_ADI_MSK(0x00000003,uint8_t))   /* Speed of Operation Value */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_EP0I_NAKLIMIT                    Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_EP0I_NAKLIMIT_VALUE          0                               /* Endpoint 0 Timeout Value (in Frames) */
#define BITM_USB_EP0I_NAKLIMIT_VALUE         (_ADI_MSK(0x0000001F,uint8_t))   /* Endpoint 0 Timeout Value (in Frames) */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_EPI_RXTYPE                       Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_EPI_RXTYPE_SPEED             6                               /* Speed of Operation Value */
#define BITP_USB_EPI_RXTYPE_PROTOCOL          4                               /* Protocol for Transfer */
#define BITP_USB_EPI_RXTYPE_TGTEP             0                               /* Target Endpoint Number */

#define BITM_USB_EPI_RXTYPE_SPEED            (_ADI_MSK(0x000000C0,uint8_t))   /* Speed of Operation Value */
#define ENUM_USB_EPI_RXTYPE_UNUSED           (_ADI_MSK(0x00000000,uint8_t))   /* SPEED: Same Speed as the Core */
#define ENUM_USB_EPI_RXTYPE_HIGHSPEED        (_ADI_MSK(0x00000040,uint8_t))   /* SPEED: High Speed */
#define ENUM_USB_EPI_RXTYPE_FULLSPEED        (_ADI_MSK(0x00000080,uint8_t))   /* SPEED: Full Speed */
#define ENUM_USB_EPI_RXTYPE_LOWSPEED         (_ADI_MSK(0x000000C0,uint8_t))   /* SPEED: Low Speed */

#define BITM_USB_EPI_RXTYPE_PROTOCOL         (_ADI_MSK(0x00000030,uint8_t))   /* Protocol for Transfer */
#define ENUM_USB_EPI_RXTYPE_CONTROL          (_ADI_MSK(0x00000000,uint8_t))   /* PROTOCOL: Control */
#define ENUM_USB_EPI_RXTYPE_ISO              (_ADI_MSK(0x00000010,uint8_t))   /* PROTOCOL: Isochronous */
#define ENUM_USB_EPI_RXTYPE_BULK             (_ADI_MSK(0x00000020,uint8_t))   /* PROTOCOL: Bulk */
#define ENUM_USB_EPI_RXTYPE_INT              (_ADI_MSK(0x00000030,uint8_t))   /* PROTOCOL: Interrupt */

#define BITM_USB_EPI_RXTYPE_TGTEP            (_ADI_MSK(0x0000000F,uint8_t))   /* Target Endpoint Number */
#define ENUM_USB_EPI_RXTYPE_TGTEP0           (_ADI_MSK(0x00000000,uint8_t))   /* TGTEP: Endpoint 0 */
#define ENUM_USB_EPI_RXTYPE_TGTEP1           (_ADI_MSK(0x00000001,uint8_t))   /* TGTEP: Endpoint 1 */
#define ENUM_USB_EPI_RXTYPE_TGTEP10          (_ADI_MSK(0x0000000A,uint8_t))   /* TGTEP: Endpoint 10 */
#define ENUM_USB_EPI_RXTYPE_TGTEP11          (_ADI_MSK(0x0000000B,uint8_t))   /* TGTEP: Endpoint 11 */
#define ENUM_USB_EPI_RXTYPE_TGTEP12          (_ADI_MSK(0x0000000C,uint8_t))   /* TGTEP: Endpoint 12 */
#define ENUM_USB_EPI_RXTYPE_TGTEP13          (_ADI_MSK(0x0000000D,uint8_t))   /* TGTEP: Endpoint 13 */
#define ENUM_USB_EPI_RXTYPE_TGTEP14          (_ADI_MSK(0x0000000E,uint8_t))   /* TGTEP: Endpoint 14 */
#define ENUM_USB_EPI_RXTYPE_TGTEP15          (_ADI_MSK(0x0000000F,uint8_t))   /* TGTEP: Endpoint 15 */
#define ENUM_USB_EPI_RXTYPE_TGTEP2           (_ADI_MSK(0x00000002,uint8_t))   /* TGTEP: Endpoint 2 */
#define ENUM_USB_EPI_RXTYPE_TGTEP3           (_ADI_MSK(0x00000003,uint8_t))   /* TGTEP: Endpoint 3 */
#define ENUM_USB_EPI_RXTYPE_TGTEP4           (_ADI_MSK(0x00000004,uint8_t))   /* TGTEP: Endpoint 4 */
#define ENUM_USB_EPI_RXTYPE_TGTEP5           (_ADI_MSK(0x00000005,uint8_t))   /* TGTEP: Endpoint 5 */
#define ENUM_USB_EPI_RXTYPE_TGTEP6           (_ADI_MSK(0x00000006,uint8_t))   /* TGTEP: Endpoint 6 */
#define ENUM_USB_EPI_RXTYPE_TGTEP7           (_ADI_MSK(0x00000007,uint8_t))   /* TGTEP: Endpoint 7 */
#define ENUM_USB_EPI_RXTYPE_TGTEP8           (_ADI_MSK(0x00000008,uint8_t))   /* TGTEP: Endpoint 8 */
#define ENUM_USB_EPI_RXTYPE_TGTEP9           (_ADI_MSK(0x00000009,uint8_t))   /* TGTEP: Endpoint 9 */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_EP0I_CFGDATA                     Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_EP0I_CFGDATA_MPRX            7                               /* Multi-Packet Aggregate for Rx Enable */
#define BITP_USB_EP0I_CFGDATA_MPTX            6                               /* Multi-Packet Split for Tx Enable */
#define BITP_USB_EP0I_CFGDATA_BIGEND          5                               /* Big Endian Data */
#define BITP_USB_EP0I_CFGDATA_HBRX            4                               /* High Bandwidth Rx Enable */
#define BITP_USB_EP0I_CFGDATA_HBTX            3                               /* High Bandwidth Tx Enable */
#define BITP_USB_EP0I_CFGDATA_DYNFIFO         2                               /* Dynamic FIFO Size Enable */
#define BITP_USB_EP0I_CFGDATA_SOFTCON         1                               /* Soft Connect Enable */
#define BITP_USB_EP0I_CFGDATA_UTMIWID         0                               /* UTMI Data Width */

#define BITM_USB_EP0I_CFGDATA_MPRX           (_ADI_MSK(0x00000080,uint8_t))   /* Multi-Packet Aggregate for Rx Enable */
#define ENUM_USB_EP0I_CFGDATA_MPRXDIS        (_ADI_MSK(0x00000000,uint8_t))   /* MPRX: No Aggregate Rx Bulk Packets */
#define ENUM_USB_EP0I_CFGDATA_MPRXEN         (_ADI_MSK(0x00000080,uint8_t))   /* MPRX: Aggregate Rx Bulk Packets */

#define BITM_USB_EP0I_CFGDATA_MPTX           (_ADI_MSK(0x00000040,uint8_t))   /* Multi-Packet Split for Tx Enable */
#define ENUM_USB_EP0I_CFGDATA_MPTXDIS        (_ADI_MSK(0x00000000,uint8_t))   /* MPTX: No Split Tx Bulk Packets */
#define ENUM_USB_EP0I_CFGDATA_MPTXEN         (_ADI_MSK(0x00000040,uint8_t))   /* MPTX: Split Tx Bulk Packets */

#define BITM_USB_EP0I_CFGDATA_BIGEND         (_ADI_MSK(0x00000020,uint8_t))   /* Big Endian Data */
#define ENUM_USB_EP0I_CFGDATA_BIGENDDIS      (_ADI_MSK(0x00000000,uint8_t))   /* BIGEND: Little Endian Configuration */
#define ENUM_USB_EP0I_CFGDATA_BIGENDEN       (_ADI_MSK(0x00000020,uint8_t))   /* BIGEND: Big Endian Configuration */

#define BITM_USB_EP0I_CFGDATA_HBRX           (_ADI_MSK(0x00000010,uint8_t))   /* High Bandwidth Rx Enable */
#define ENUM_USB_EP0I_CFGDATA_HBRXDIS        (_ADI_MSK(0x00000000,uint8_t))   /* HBRX: No High Bandwidth Rx */
#define ENUM_USB_EP0I_CFGDATA_HBRXEN         (_ADI_MSK(0x00000010,uint8_t))   /* HBRX: High Bandwidth Rx */

#define BITM_USB_EP0I_CFGDATA_HBTX           (_ADI_MSK(0x00000008,uint8_t))   /* High Bandwidth Tx Enable */
#define ENUM_USB_EP0I_CFGDATA_HBTXDIS        (_ADI_MSK(0x00000000,uint8_t))   /* HBTX: No High Bandwidth Tx */
#define ENUM_USB_EP0I_CFGDATA_HBTXEN         (_ADI_MSK(0x00000008,uint8_t))   /* HBTX: High Bandwidth Tx */

#define BITM_USB_EP0I_CFGDATA_DYNFIFO        (_ADI_MSK(0x00000004,uint8_t))   /* Dynamic FIFO Size Enable */
#define ENUM_USB_EP0I_CFGDATA_DYNSZDIS       (_ADI_MSK(0x00000000,uint8_t))   /* DYNFIFO: No Dynamic FIFO Size */
#define ENUM_USB_EP0I_CFGDATA_DYNSZEN        (_ADI_MSK(0x00000004,uint8_t))   /* DYNFIFO: Dynamic FIFO Size */

#define BITM_USB_EP0I_CFGDATA_SOFTCON        (_ADI_MSK(0x00000002,uint8_t))   /* Soft Connect Enable */
#define ENUM_USB_EP0I_CFGDATA_SFTCONDIS      (_ADI_MSK(0x00000000,uint8_t))   /* SOFTCON: No Soft Connect */
#define ENUM_USB_EP0I_CFGDATA_SFTCONEN       (_ADI_MSK(0x00000002,uint8_t))   /* SOFTCON: Soft Connect */

#define BITM_USB_EP0I_CFGDATA_UTMIWID        (_ADI_MSK(0x00000001,uint8_t))   /* UTMI Data Width */
#define ENUM_USB_EP0I_CFGDATA_UTMIWID8       (_ADI_MSK(0x00000000,uint8_t))   /* UTMIWID: 8-bit UTMI Data Width */
#define ENUM_USB_EP0I_CFGDATA_UTMIWID16      (_ADI_MSK(0x00000001,uint8_t))   /* UTMIWID: 16-bit UTMI Data Width */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_DEV_CTL                          Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_DEV_CTL_BDEVICE              7                               /* A or B Devices Indicator */
#define BITP_USB_DEV_CTL_FSDEV                6                               /* Full or High-Speed Indicator */
#define BITP_USB_DEV_CTL_LSDEV                5                               /* Low-Speed Indicator */
#define BITP_USB_DEV_CTL_VBUS                 3                               /* VBUS Level Indicator */
#define BITP_USB_DEV_CTL_HOSTMODE             2                               /* Host Mode Indicator */
#define BITP_USB_DEV_CTL_HOSTREQ              1                               /* Host Negotiation Request */
#define BITP_USB_DEV_CTL_SESSION              0                               /* Session Indicator */

#define BITM_USB_DEV_CTL_BDEVICE             (_ADI_MSK(0x00000080,uint8_t))   /* A or B Devices Indicator */
#define ENUM_USB_DEV_CTL_ADEVICE             (_ADI_MSK(0x00000000,uint8_t))   /* BDEVICE: A Device Detected */
#define ENUM_USB_DEV_CTL_BDEVICE             (_ADI_MSK(0x00000080,uint8_t))   /* BDEVICE: B Device Detected */

#define BITM_USB_DEV_CTL_FSDEV               (_ADI_MSK(0x00000040,uint8_t))   /* Full or High-Speed Indicator */
#define ENUM_USB_DEV_CTL_NO_FSDEV            (_ADI_MSK(0x00000000,uint8_t))   /* FSDEV: Not Detected */
#define ENUM_USB_DEV_CTL_FSDEV               (_ADI_MSK(0x00000040,uint8_t))   /* FSDEV: Full or High Speed Detected */

#define BITM_USB_DEV_CTL_LSDEV               (_ADI_MSK(0x00000020,uint8_t))   /* Low-Speed Indicator */
#define ENUM_USB_DEV_CTL_NO_LSDEV            (_ADI_MSK(0x00000000,uint8_t))   /* LSDEV: Not Detected */
#define ENUM_USB_DEV_CTL_LSDEV               (_ADI_MSK(0x00000020,uint8_t))   /* LSDEV: Low Speed Detected */

#define BITM_USB_DEV_CTL_VBUS                (_ADI_MSK(0x00000018,uint8_t))   /* VBUS Level Indicator */
#define ENUM_USB_DEV_CTL_VBUS_BS             (_ADI_MSK(0x00000000,uint8_t))   /* VBUS: Below SessionEnd */
#define ENUM_USB_DEV_CTL_VBUS_ASBA           (_ADI_MSK(0x00000008,uint8_t))   /* VBUS: Above SessionEnd, below AValid */
#define ENUM_USB_DEV_CTL_VBUS_AABV           (_ADI_MSK(0x00000010,uint8_t))   /* VBUS: Above AValid, below VBUSValid */
#define ENUM_USB_DEV_CTL_VBUS_AV             (_ADI_MSK(0x00000018,uint8_t))   /* VBUS: Above VBUSValid */

#define BITM_USB_DEV_CTL_HOSTMODE            (_ADI_MSK(0x00000004,uint8_t))   /* Host Mode Indicator */
#define ENUM_USB_DEV_CTL_NO_HOSTMODE         (_ADI_MSK(0x00000000,uint8_t))   /* HOSTMODE: Peripheral Mode */
#define ENUM_USB_DEV_CTL_HOSTMODE            (_ADI_MSK(0x00000004,uint8_t))   /* HOSTMODE: Host Mode */

#define BITM_USB_DEV_CTL_HOSTREQ             (_ADI_MSK(0x00000002,uint8_t))   /* Host Negotiation Request */
#define ENUM_USB_DEV_CTL_NO_HOSTREQ          (_ADI_MSK(0x00000000,uint8_t))   /* HOSTREQ: No Request */
#define ENUM_USB_DEV_CTL_HOSTREQ             (_ADI_MSK(0x00000002,uint8_t))   /* HOSTREQ: Place Request */

#define BITM_USB_DEV_CTL_SESSION             (_ADI_MSK(0x00000001,uint8_t))   /* Session Indicator */
#define ENUM_USB_DEV_CTL_NO_SESSION          (_ADI_MSK(0x00000000,uint8_t))   /* SESSION: Not Detected */
#define ENUM_USB_DEV_CTL_SESSION             (_ADI_MSK(0x00000001,uint8_t))   /* SESSION: Detected Session */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_TXFIFOSZ                         Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_TXFIFOSZ_DPB                 4                               /* Double Packet Buffering Enable */
#define BITP_USB_TXFIFOSZ_SZ                  0                               /* Maximum Packet Size */

#define BITM_USB_TXFIFOSZ_DPB                (_ADI_MSK(0x00000010,uint8_t))   /* Double Packet Buffering Enable */
#define ENUM_USB_TXFIFOSZ_DPNDIS             (_ADI_MSK(0x00000000,uint8_t))   /* DPB: Single Packet Buffering */
#define ENUM_USB_TXFIFOSZ_DPBEN              (_ADI_MSK(0x00000010,uint8_t))   /* DPB: Double Packet Buffering */

#define BITM_USB_TXFIFOSZ_SZ                 (_ADI_MSK(0x0000000F,uint8_t))   /* Maximum Packet Size */
#define ENUM_USB_TXFIFOSZ_SZ8                (_ADI_MSK(0x00000000,uint8_t))   /* SZ: PktSz=8, DPB0=8, DPB1=16 */
#define ENUM_USB_TXFIFOSZ_SZ16               (_ADI_MSK(0x00000001,uint8_t))   /* SZ: PktSz=16, DPB0=16, DPB1=32 */
#define ENUM_USB_TXFIFOSZ_SZ32               (_ADI_MSK(0x00000002,uint8_t))   /* SZ: PktSz=32, DPB0=32, DPB1=64 */
#define ENUM_USB_TXFIFOSZ_SZ64               (_ADI_MSK(0x00000003,uint8_t))   /* SZ: PktSz=64, DPB0=64, DPB1=128 */
#define ENUM_USB_TXFIFOSZ_SZ128              (_ADI_MSK(0x00000004,uint8_t))   /* SZ: PktSz=128, DPB0=128, DPB1=256 */
#define ENUM_USB_TXFIFOSZ_SZ256              (_ADI_MSK(0x00000005,uint8_t))   /* SZ: PktSz=256, DPB0=256, DPB1=512 */
#define ENUM_USB_TXFIFOSZ_SZ512              (_ADI_MSK(0x00000006,uint8_t))   /* SZ: PktSz=512, DPB0=512, DPB1=1024 */
#define ENUM_USB_TXFIFOSZ_SZ1024             (_ADI_MSK(0x00000007,uint8_t))   /* SZ: PktSz=1024, DPB0=1024, DPB1=2048 */
#define ENUM_USB_TXFIFOSZ_SZ2048             (_ADI_MSK(0x00000008,uint8_t))   /* SZ: PktSz=2048, DPB0=2048, DPB1=4096 */
#define ENUM_USB_TXFIFOSZ_SZ4096             (_ADI_MSK(0x00000009,uint8_t))   /* SZ: PktSz=4096, DPB0=4096, DPB1=8192 */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_RXFIFOSZ                         Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_RXFIFOSZ_DPB                 4                               /* Double Packet Buffering Enable */
#define BITP_USB_RXFIFOSZ_SZ                  0                               /* Maximum Packet Size */

#define BITM_USB_RXFIFOSZ_DPB                (_ADI_MSK(0x00000010,uint8_t))   /* Double Packet Buffering Enable */
#define ENUM_USB_RXFIFOSZ_DPBDIS             (_ADI_MSK(0x00000000,uint8_t))   /* DPB: Single Packet Buffering */
#define ENUM_USB_RXFIFOSZ_DPBEN              (_ADI_MSK(0x00000010,uint8_t))   /* DPB: Double Packet Buffering */

#define BITM_USB_RXFIFOSZ_SZ                 (_ADI_MSK(0x0000000F,uint8_t))   /* Maximum Packet Size */
#define ENUM_USB_RXFIFOSZ_SZ8                (_ADI_MSK(0x00000000,uint8_t))   /* SZ: PktSz=8, DPB0=8, DPB1=16 */
#define ENUM_USB_RXFIFOSZ_SZ16               (_ADI_MSK(0x00000001,uint8_t))   /* SZ: PktSz=16, DPB0=16, DPB1=32 */
#define ENUM_USB_RXFIFOSZ_SZ32               (_ADI_MSK(0x00000002,uint8_t))   /* SZ: PktSz=32, DPB0=32, DPB1=64 */
#define ENUM_USB_RXFIFOSZ_SZ64               (_ADI_MSK(0x00000003,uint8_t))   /* SZ: PktSz=64, DPB0=64, DPB1=128 */
#define ENUM_USB_RXFIFOSZ_SZ128              (_ADI_MSK(0x00000004,uint8_t))   /* SZ: PktSz=128, DPB0=128, DPB1=256 */
#define ENUM_USB_RXFIFOSZ_SZ256              (_ADI_MSK(0x00000005,uint8_t))   /* SZ: PktSz=256, DPB0=256, DPB1=512 */
#define ENUM_USB_RXFIFOSZ_SZ512              (_ADI_MSK(0x00000006,uint8_t))   /* SZ: PktSz=512, DPB0=512, DPB1=1024 */
#define ENUM_USB_RXFIFOSZ_SZ1024             (_ADI_MSK(0x00000007,uint8_t))   /* SZ: PktSz=1024, DPB0=1024, DPB1=2048 */
#define ENUM_USB_RXFIFOSZ_SZ2048             (_ADI_MSK(0x00000008,uint8_t))   /* SZ: PktSz=2048, DPB0=2048, DPB1=4096 */
#define ENUM_USB_RXFIFOSZ_SZ4096             (_ADI_MSK(0x00000009,uint8_t))   /* SZ: PktSz=4096, DPB0=4096, DPB1=8192 */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_TXFIFOADDR                       Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_TXFIFOADDR_VALUE             0                               /* Tx FIFO Start Address */
#define BITM_USB_TXFIFOADDR_VALUE            (_ADI_MSK(0x00001FFF,uint16_t))  /* Tx FIFO Start Address */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_RXFIFOADDR                       Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_RXFIFOADDR_VALUE             0                               /* Rx FIFO Start Address */
#define BITM_USB_RXFIFOADDR_VALUE            (_ADI_MSK(0x00000FFF,uint16_t))  /* Rx FIFO Start Address */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_EPINFO                           Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_EPINFO_RXEP                  4                               /* Rx Endpoints */
#define BITP_USB_EPINFO_TXEP                  0                               /* Tx Endpoints */
#define BITM_USB_EPINFO_RXEP                 (_ADI_MSK(0x000000F0,uint8_t))   /* Rx Endpoints */
#define BITM_USB_EPINFO_TXEP                 (_ADI_MSK(0x0000000F,uint8_t))   /* Tx Endpoints */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_RAMINFO                          Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_RAMINFO_DMACHANS             4                               /* DMA Channels */
#define BITP_USB_RAMINFO_RAMBITS              0                               /* RAM Address Bits */
#define BITM_USB_RAMINFO_DMACHANS            (_ADI_MSK(0x000000F0,uint8_t))   /* DMA Channels */
#define BITM_USB_RAMINFO_RAMBITS             (_ADI_MSK(0x0000000F,uint8_t))   /* RAM Address Bits */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_LINKINFO                         Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_LINKINFO_WTCON               4                               /* Wait for Connect/Disconnect */
#define BITP_USB_LINKINFO_WTID                0                               /* Wait from ID Pull-up */
#define BITM_USB_LINKINFO_WTCON              (_ADI_MSK(0x000000F0,uint8_t))   /* Wait for Connect/Disconnect */
#define BITM_USB_LINKINFO_WTID               (_ADI_MSK(0x0000000F,uint8_t))   /* Wait from ID Pull-up */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_SOFT_RST                         Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_SOFT_RST_RSTX                1                               /* Reset USB XCLK Domain */
#define BITP_USB_SOFT_RST_RST                 0                               /* Reset USB CLK Domain */

#define BITM_USB_SOFT_RST_RSTX               (_ADI_MSK(0x00000002,uint8_t))   /* Reset USB XCLK Domain */
#define ENUM_USB_SOFT_RST_NO_RSTX            (_ADI_MSK(0x00000000,uint8_t))   /* RSTX: No Reset */
#define ENUM_USB_SOFT_RST_RSTX               (_ADI_MSK(0x00000002,uint8_t))   /* RSTX: Reset USB XCLK Domain */

#define BITM_USB_SOFT_RST_RST                (_ADI_MSK(0x00000001,uint8_t))   /* Reset USB CLK Domain */
#define ENUM_USB_SOFT_RST_NO_RST             (_ADI_MSK(0x00000000,uint8_t))   /* RST: No Reset */
#define ENUM_USB_SOFT_RST_RST                (_ADI_MSK(0x00000001,uint8_t))   /* RST: Reset USB CLK Domain */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_MP_TXFUNCADDR                    Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_MP_TXFUNCADDR_VALUE          0                               /* Tx Function Address Value */
#define BITM_USB_MP_TXFUNCADDR_VALUE         (_ADI_MSK(0x0000007F,uint8_t))   /* Tx Function Address Value */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_MP_TXHUBADDR                     Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_MP_TXHUBADDR_MULTTRANS       7                               /* Multiple Transaction Translators */
#define BITP_USB_MP_TXHUBADDR_ADDR            0                               /* Hub Address Value */
#define BITM_USB_MP_TXHUBADDR_MULTTRANS      (_ADI_MSK(0x00000080,uint8_t))   /* Multiple Transaction Translators */
#define BITM_USB_MP_TXHUBADDR_ADDR           (_ADI_MSK(0x0000007F,uint8_t))   /* Hub Address Value */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_MP_TXHUBPORT                     Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_MP_TXHUBPORT_VALUE           0                               /* Hub Port Value */
#define BITM_USB_MP_TXHUBPORT_VALUE          (_ADI_MSK(0x0000007F,uint8_t))   /* Hub Port Value */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_MP_RXFUNCADDR                    Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_MP_RXFUNCADDR_VALUE          0                               /* Rx Function Address Value */
#define BITM_USB_MP_RXFUNCADDR_VALUE         (_ADI_MSK(0x0000007F,uint8_t))   /* Rx Function Address Value */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_MP_RXHUBADDR                     Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_MP_RXHUBADDR_MULTTRANS       7                               /* Multiple Transaction Translators */
#define BITP_USB_MP_RXHUBADDR_ADDR            0                               /* Hub Address Value */
#define BITM_USB_MP_RXHUBADDR_MULTTRANS      (_ADI_MSK(0x00000080,uint8_t))   /* Multiple Transaction Translators */
#define BITM_USB_MP_RXHUBADDR_ADDR           (_ADI_MSK(0x0000007F,uint8_t))   /* Hub Address Value */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_MP_RXHUBPORT                     Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_MP_RXHUBPORT_VALUE           0                               /* Hub Port Value */
#define BITM_USB_MP_RXHUBPORT_VALUE          (_ADI_MSK(0x0000007F,uint8_t))   /* Hub Port Value */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_EP_TXMAXP                        Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_EP_TXMAXP_MULTM1            11                               /* Multi-Packets per Micro-frame */
#define BITP_USB_EP_TXMAXP_MAXPAY             0                               /* Maximum Payload */
#define BITM_USB_EP_TXMAXP_MULTM1            (_ADI_MSK(0x00001800,uint16_t))  /* Multi-Packets per Micro-frame */
#define BITM_USB_EP_TXMAXP_MAXPAY            (_ADI_MSK(0x000007FF,uint16_t))  /* Maximum Payload */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_EP0_CSR_H                        Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_EP0_CSR_H_DISPING           11                               /* Disable Ping */
#define BITP_USB_EP0_CSR_H_DATGLEN           10                               /* Data Toggle Write Enable */
#define BITP_USB_EP0_CSR_H_DATGL              9                               /* Data Toggle */
#define BITP_USB_EP0_CSR_H_FLUSHFIFO          8                               /* Flush Endpoint FIFO */
#define BITP_USB_EP0_CSR_H_NAKTO              7                               /* NAK Timeout */
#define BITP_USB_EP0_CSR_H_STATUSPKT          6                               /* Status Packet */
#define BITP_USB_EP0_CSR_H_REQPKT             5                               /* Request Packet */
#define BITP_USB_EP0_CSR_H_TOERR              4                               /* Timeout Error */
#define BITP_USB_EP0_CSR_H_SETUPPKT           3                               /* Setup Packet */
#define BITP_USB_EP0_CSR_H_RXSTALL            2                               /* Rx Stall */
#define BITP_USB_EP0_CSR_H_TXPKTRDY           1                               /* Tx Packet Ready */
#define BITP_USB_EP0_CSR_H_RXPKTRDY           0                               /* Rx Packet Ready */

#define BITM_USB_EP0_CSR_H_DISPING           (_ADI_MSK(0x00000800,uint16_t))  /* Disable Ping */
#define ENUM_USB_EP0_CSR_H_NO_DISPING        (_ADI_MSK(0x00000000,uint16_t))  /* DISPING: Issue PING tokens */
#define ENUM_USB_EP0_CSR_H_DISPING           (_ADI_MSK(0x00000800,uint16_t))  /* DISPING: Do not issue PING */

#define BITM_USB_EP0_CSR_H_DATGLEN           (_ADI_MSK(0x00000400,uint16_t))  /* Data Toggle Write Enable */
#define ENUM_USB_EP0_CSR_H_NO_DATGLEN        (_ADI_MSK(0x00000000,uint16_t))  /* DATGLEN: Disable Write to DATGL */
#define ENUM_USB_EP0_CSR_H_DATGLEN           (_ADI_MSK(0x00000400,uint16_t))  /* DATGLEN: Enable Write to DATGL */

#define BITM_USB_EP0_CSR_H_DATGL             (_ADI_MSK(0x00000200,uint16_t))  /* Data Toggle */
#define ENUM_USB_EP0_CSR_H_NO_DATATGL        (_ADI_MSK(0x00000000,uint16_t))  /* DATGL: DATA0 is Set */
#define ENUM_USB_EP0_CSR_H_DATATGL           (_ADI_MSK(0x00000200,uint16_t))  /* DATGL: DATA1 is Set */

#define BITM_USB_EP0_CSR_H_FLUSHFIFO         (_ADI_MSK(0x00000100,uint16_t))  /* Flush Endpoint FIFO */
#define ENUM_USB_EP0_CSR_H_NO_FLUSH          (_ADI_MSK(0x00000000,uint16_t))  /* FLUSHFIFO: No Flush */
#define ENUM_USB_EP0_CSR_H_FLUSH             (_ADI_MSK(0x00000100,uint16_t))  /* FLUSHFIFO: Flush Endpoint FIFO */

#define BITM_USB_EP0_CSR_H_NAKTO             (_ADI_MSK(0x00000080,uint16_t))  /* NAK Timeout */
#define ENUM_USB_EP0_CSR_H_NO_NAKTO          (_ADI_MSK(0x00000000,uint16_t))  /* NAKTO: No Status */
#define ENUM_USB_EP0_CSR_H_NAKTO             (_ADI_MSK(0x00000080,uint16_t))  /* NAKTO: Endpoint Halted (NAK Timeout) */

#define BITM_USB_EP0_CSR_H_STATUSPKT         (_ADI_MSK(0x00000040,uint16_t))  /* Status Packet */
#define ENUM_USB_EP0_CSR_H_NO_STATPKT        (_ADI_MSK(0x00000000,uint16_t))  /* STATUSPKT: No Request */
#define ENUM_USB_EP0_CSR_H_STATPKT           (_ADI_MSK(0x00000040,uint16_t))  /* STATUSPKT: Request Status Transaction */

#define BITM_USB_EP0_CSR_H_REQPKT            (_ADI_MSK(0x00000020,uint16_t))  /* Request Packet */
#define ENUM_USB_EP0_CSR_H_NO_REQPKT         (_ADI_MSK(0x00000000,uint16_t))  /* REQPKT: No Request */
#define ENUM_USB_EP0_CSR_H_REQPKT            (_ADI_MSK(0x00000020,uint16_t))  /* REQPKT: Send IN Tokens to Device */

#define BITM_USB_EP0_CSR_H_TOERR             (_ADI_MSK(0x00000010,uint16_t))  /* Timeout Error */
#define ENUM_USB_EP0_CSR_H_NO_TOERR          (_ADI_MSK(0x00000000,uint16_t))  /* TOERR: No Status */
#define ENUM_USB_EP0_CSR_H_TOERR             (_ADI_MSK(0x00000010,uint16_t))  /* TOERR: Timeout Error */

#define BITM_USB_EP0_CSR_H_SETUPPKT          (_ADI_MSK(0x00000008,uint16_t))  /* Setup Packet */
#define ENUM_USB_EP0_CSR_H_NO_SETUPPKT       (_ADI_MSK(0x00000000,uint16_t))  /* SETUPPKT: No Request */
#define ENUM_USB_EP0_CSR_H_SETUPPKT          (_ADI_MSK(0x00000008,uint16_t))  /* SETUPPKT: Send SETUP token */

#define BITM_USB_EP0_CSR_H_RXSTALL           (_ADI_MSK(0x00000004,uint16_t))  /* Rx Stall */
#define ENUM_USB_EP0_CSR_H_NO_RXSTALL        (_ADI_MSK(0x00000000,uint16_t))  /* RXSTALL: No Status */
#define ENUM_USB_EP0_CSR_H_RXSTALL           (_ADI_MSK(0x00000004,uint16_t))  /* RXSTALL: Stall Received from Device */

#define BITM_USB_EP0_CSR_H_TXPKTRDY          (_ADI_MSK(0x00000002,uint16_t))  /* Tx Packet Ready */
#define ENUM_USB_EP0_CSR_H_NO_TXPKTRDY       (_ADI_MSK(0x00000000,uint16_t))  /* TXPKTRDY: No Tx Packet */
#define ENUM_USB_EP0_CSR_H_TXPKTRDY          (_ADI_MSK(0x00000002,uint16_t))  /* TXPKTRDY: Tx Packet in Endpoint FIFO */

#define BITM_USB_EP0_CSR_H_RXPKTRDY          (_ADI_MSK(0x00000001,uint16_t))  /* Rx Packet Ready */
#define ENUM_USB_EP0_CSR_H_NO_RXPKTRDY       (_ADI_MSK(0x00000000,uint16_t))  /* RXPKTRDY: No Rx Packet */
#define ENUM_USB_EP0_CSR_H_RXPKTRDY          (_ADI_MSK(0x00000001,uint16_t))  /* RXPKTRDY: Rx Packet in Endpoint FIFO */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_EP_TXCSR_H                       Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_EP_TXCSR_H_AUTOSET          15                               /* TxPkRdy Autoset Enable */
#define BITP_USB_EP_TXCSR_H_DMAREQEN         12                               /* DMA Request Enable Tx EP */
#define BITP_USB_EP_TXCSR_H_FRCDATATGL       11                               /* Force Data Toggle */
#define BITP_USB_EP_TXCSR_H_DMAREQMODE       10                               /* DMA Mode Select */
#define BITP_USB_EP_TXCSR_H_DATGLEN           9                               /* Data Toggle Write Enable */
#define BITP_USB_EP_TXCSR_H_DATGL             8                               /* Data Toggle */
#define BITP_USB_EP_TXCSR_H_NAKTOINCMP        7                               /* NAK Timeout Incomplete */
#define BITP_USB_EP_TXCSR_H_CLRDATATGL        6                               /* Clear Endpoint Data Toggle */
#define BITP_USB_EP_TXCSR_H_RXSTALL           5                               /* Rx STALL */
#define BITP_USB_EP_TXCSR_H_SETUPPKT          4                               /* Setup Packet */
#define BITP_USB_EP_TXCSR_H_FLUSHFIFO         3                               /* Flush Endpoint FIFO */
#define BITP_USB_EP_TXCSR_H_TXTOERR           2                               /* Tx Timeout Error */
#define BITP_USB_EP_TXCSR_H_NEFIFO            1                               /* Not Empty FIFO */
#define BITP_USB_EP_TXCSR_H_TXPKTRDY          0                               /* Tx Packet Ready */

#define BITM_USB_EP_TXCSR_H_AUTOSET          (_ADI_MSK(0x00008000,uint16_t))  /* TxPkRdy Autoset Enable */
#define ENUM_USB_EP_TXCSR_H_NO_AUTOSET       (_ADI_MSK(0x00000000,uint16_t))  /* AUTOSET: Disable Autoset */
#define ENUM_USB_EP_TXCSR_H_AUTOSET          (_ADI_MSK(0x00008000,uint16_t))  /* AUTOSET: Enable Autoset */

#define BITM_USB_EP_TXCSR_H_DMAREQEN         (_ADI_MSK(0x00001000,uint16_t))  /* DMA Request Enable Tx EP */
#define ENUM_USB_EP_TXCSR_H_DMAREQDIS        (_ADI_MSK(0x00000000,uint16_t))  /* DMAREQEN: Disable DMA Request */
#define ENUM_USB_EP_TXCSR_H_DMAREQEN         (_ADI_MSK(0x00001000,uint16_t))  /* DMAREQEN: Enable DMA Request */

#define BITM_USB_EP_TXCSR_H_FRCDATATGL       (_ADI_MSK(0x00000800,uint16_t))  /* Force Data Toggle */
#define ENUM_USB_EP_TXCSR_H_NO_FRCTGL        (_ADI_MSK(0x00000000,uint16_t))  /* FRCDATATGL: No Action */
#define ENUM_USB_EP_TXCSR_H_FRCTGL           (_ADI_MSK(0x00000800,uint16_t))  /* FRCDATATGL: Toggle Endpoint Data */

#define BITM_USB_EP_TXCSR_H_DMAREQMODE       (_ADI_MSK(0x00000400,uint16_t))  /* DMA Mode Select */
#define ENUM_USB_EP_TXCSR_H_DMARQMODE0       (_ADI_MSK(0x00000000,uint16_t))  /* DMAREQMODE: DMA Request Mode 0 */
#define ENUM_USB_EP_TXCSR_H_DMARQMODE1       (_ADI_MSK(0x00000400,uint16_t))  /* DMAREQMODE: DMA Request Mode 1 */

#define BITM_USB_EP_TXCSR_H_DATGLEN          (_ADI_MSK(0x00000200,uint16_t))  /* Data Toggle Write Enable */
#define ENUM_USB_EP_TXCSR_H_NO_DATGLEN       (_ADI_MSK(0x00000000,uint16_t))  /* DATGLEN: Disable Write to DATGL */
#define ENUM_USB_EP_TXCSR_H_DATGLEN          (_ADI_MSK(0x00000200,uint16_t))  /* DATGLEN: Enable Write to DATGL */

#define BITM_USB_EP_TXCSR_H_DATGL            (_ADI_MSK(0x00000100,uint16_t))  /* Data Toggle */
#define ENUM_USB_EP_TXCSR_H_NO_DATGL         (_ADI_MSK(0x00000000,uint16_t))  /* DATGL: DATA0 is set */
#define ENUM_USB_EP_TXCSR_H_DATGL            (_ADI_MSK(0x00000100,uint16_t))  /* DATGL: DATA1 is set */

#define BITM_USB_EP_TXCSR_H_NAKTOINCMP       (_ADI_MSK(0x00000080,uint16_t))  /* NAK Timeout Incomplete */
#define ENUM_USB_EP_TXCSR_H_NO_NAKTO         (_ADI_MSK(0x00000000,uint16_t))  /* NAKTOINCMP: No Status */
#define ENUM_USB_EP_TXCSR_H_NAKTO            (_ADI_MSK(0x00000080,uint16_t))  /* NAKTOINCMP: NAK Timeout Over Maximum */

#define BITM_USB_EP_TXCSR_H_CLRDATATGL       (_ADI_MSK(0x00000040,uint16_t))  /* Clear Endpoint Data Toggle */
#define ENUM_USB_EP_TXCSR_H_NO_CLRTGL        (_ADI_MSK(0x00000000,uint16_t))  /* CLRDATATGL: No Action */
#define ENUM_USB_EP_TXCSR_H_CLRTGL           (_ADI_MSK(0x00000040,uint16_t))  /* CLRDATATGL: Reset EP Data Toggle to 0 */

#define BITM_USB_EP_TXCSR_H_RXSTALL          (_ADI_MSK(0x00000020,uint16_t))  /* Rx STALL */
#define ENUM_USB_EP_TXCSR_H_NO_RXSTALL       (_ADI_MSK(0x00000000,uint16_t))  /* RXSTALL: No Status */
#define ENUM_USB_EP_TXCSR_H_RXSTALL          (_ADI_MSK(0x00000020,uint16_t))  /* RXSTALL: Stall Received from Device */

#define BITM_USB_EP_TXCSR_H_SETUPPKT         (_ADI_MSK(0x00000010,uint16_t))  /* Setup Packet */
#define ENUM_USB_EP_TXCSR_H_NO_SETUPPK       (_ADI_MSK(0x00000000,uint16_t))  /* SETUPPKT: No Request */
#define ENUM_USB_EP_TXCSR_H_SETUPPKT         (_ADI_MSK(0x00000010,uint16_t))  /* SETUPPKT: Send SETUP Token */

#define BITM_USB_EP_TXCSR_H_FLUSHFIFO        (_ADI_MSK(0x00000008,uint16_t))  /* Flush Endpoint FIFO */
#define ENUM_USB_EP_TXCSR_H_NO_FLUSH         (_ADI_MSK(0x00000000,uint16_t))  /* FLUSHFIFO: No Flush */
#define ENUM_USB_EP_TXCSR_H_FLUSH            (_ADI_MSK(0x00000008,uint16_t))  /* FLUSHFIFO: Flush endpoint FIFO */

#define BITM_USB_EP_TXCSR_H_TXTOERR          (_ADI_MSK(0x00000004,uint16_t))  /* Tx Timeout Error */
#define ENUM_USB_EP_TXCSR_H_NO_TXTOERR       (_ADI_MSK(0x00000000,uint16_t))  /* TXTOERR: No Status */
#define ENUM_USB_EP_TXCSR_H_TXTOERR          (_ADI_MSK(0x00000004,uint16_t))  /* TXTOERR: Tx Timeout Error */

#define BITM_USB_EP_TXCSR_H_NEFIFO           (_ADI_MSK(0x00000002,uint16_t))  /* Not Empty FIFO */
#define ENUM_USB_EP_TXCSR_H_NO_NEFIFO        (_ADI_MSK(0x00000000,uint16_t))  /* NEFIFO: FIFO Empty */
#define ENUM_USB_EP_TXCSR_H_NEFIFO           (_ADI_MSK(0x00000002,uint16_t))  /* NEFIFO: FIFO Not Empty */

#define BITM_USB_EP_TXCSR_H_TXPKTRDY         (_ADI_MSK(0x00000001,uint16_t))  /* Tx Packet Ready */
#define ENUM_USB_EP_TXCSR_H_NO_PKTRDY        (_ADI_MSK(0x00000000,uint16_t))  /* TXPKTRDY: No Tx Packet */
#define ENUM_USB_EP_TXCSR_H_PKTRDY           (_ADI_MSK(0x00000001,uint16_t))  /* TXPKTRDY: Tx Packet in Endpoint FIFO */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_EP0_CSR_P                        Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_EP0_CSR_P_FLUSHFIFO          8                               /* Flush Endpoint FIFO */
#define BITP_USB_EP0_CSR_P_SSETUPEND          7                               /* Service Setup End */
#define BITP_USB_EP0_CSR_P_SPKTRDY            6                               /* Service Rx Packet Ready */
#define BITP_USB_EP0_CSR_P_SENDSTALL          5                               /* Send Stall */
#define BITP_USB_EP0_CSR_P_SETUPEND           4                               /* Setup End */
#define BITP_USB_EP0_CSR_P_DATAEND            3                               /* Data End */
#define BITP_USB_EP0_CSR_P_SENTSTALL          2                               /* Sent Stall */
#define BITP_USB_EP0_CSR_P_TXPKTRDY           1                               /* Tx Packet Ready */
#define BITP_USB_EP0_CSR_P_RXPKTRDY           0                               /* Rx Packet Ready */

#define BITM_USB_EP0_CSR_P_FLUSHFIFO         (_ADI_MSK(0x00000100,uint16_t))  /* Flush Endpoint FIFO */
#define ENUM_USB_EP0_CSR_P_NO_FLUSH          (_ADI_MSK(0x00000000,uint16_t))  /* FLUSHFIFO: No Flush */
#define ENUM_USB_EP0_CSR_P_FLUSH             (_ADI_MSK(0x00000100,uint16_t))  /* FLUSHFIFO: Flush Endpoint FIFO */

#define BITM_USB_EP0_CSR_P_SSETUPEND         (_ADI_MSK(0x00000080,uint16_t))  /* Service Setup End */
#define ENUM_USB_EP0_CSR_P_NOSSETUPEND       (_ADI_MSK(0x00000000,uint16_t))  /* SSETUPEND: No Action */
#define ENUM_USB_EP0_CSR_P_SSETUPEND         (_ADI_MSK(0x00000080,uint16_t))  /* SSETUPEND: Clear SETUPEND Bit */

#define BITM_USB_EP0_CSR_P_SPKTRDY           (_ADI_MSK(0x00000040,uint16_t))  /* Service Rx Packet Ready */
#define ENUM_USB_EP0_CSR_P_NO_SPKTRDY        (_ADI_MSK(0x00000000,uint16_t))  /* SPKTRDY: No Action */
#define ENUM_USB_EP0_CSR_P_SPKTRDY           (_ADI_MSK(0x00000040,uint16_t))  /* SPKTRDY: Clear RXPKTRDY Bit */

#define BITM_USB_EP0_CSR_P_SENDSTALL         (_ADI_MSK(0x00000020,uint16_t))  /* Send Stall */
#define ENUM_USB_EP0_CSR_P_NO_STALL          (_ADI_MSK(0x00000000,uint16_t))  /* SENDSTALL: No Action */
#define ENUM_USB_EP0_CSR_P_STALL             (_ADI_MSK(0x00000020,uint16_t))  /* SENDSTALL: Terminate Current Transaction */

#define BITM_USB_EP0_CSR_P_SETUPEND          (_ADI_MSK(0x00000010,uint16_t))  /* Setup End */
#define ENUM_USB_EP0_CSR_P_NO_SETUPEND       (_ADI_MSK(0x00000000,uint16_t))  /* SETUPEND: No Status */
#define ENUM_USB_EP0_CSR_P_SETUPEND          (_ADI_MSK(0x00000010,uint16_t))  /* SETUPEND: Setup Ended before DATAEND */

#define BITM_USB_EP0_CSR_P_DATAEND           (_ADI_MSK(0x00000008,uint16_t))  /* Data End */
#define ENUM_USB_EP0_CSR_P_NO_DATAEND        (_ADI_MSK(0x00000000,uint16_t))  /* DATAEND: No Status */
#define ENUM_USB_EP0_CSR_P_DATAEND           (_ADI_MSK(0x00000008,uint16_t))  /* DATAEND: Data End Condition */

#define BITM_USB_EP0_CSR_P_SENTSTALL         (_ADI_MSK(0x00000004,uint16_t))  /* Sent Stall */
#define ENUM_USB_EP0_CSR_P_NO_STALSNT        (_ADI_MSK(0x00000000,uint16_t))  /* SENTSTALL: No Status */
#define ENUM_USB_EP0_CSR_P_STALSNT           (_ADI_MSK(0x00000004,uint16_t))  /* SENTSTALL: Transmitted STALL Handshake */

#define BITM_USB_EP0_CSR_P_TXPKTRDY          (_ADI_MSK(0x00000002,uint16_t))  /* Tx Packet Ready */
#define ENUM_USB_EP0_CSR_P_NO_TXPKTRDY       (_ADI_MSK(0x00000000,uint16_t))  /* TXPKTRDY:  */
#define ENUM_USB_EP0_CSR_P_TXPKTRDY          (_ADI_MSK(0x00000002,uint16_t))  /* TXPKTRDY: Set this bit after loading a data packet into the FIFO */

#define BITM_USB_EP0_CSR_P_RXPKTRDY          (_ADI_MSK(0x00000001,uint16_t))  /* Rx Packet Ready */
#define ENUM_USB_EP0_CSR_P_NO_PKTRDY         (_ADI_MSK(0x00000000,uint16_t))  /* RXPKTRDY: No Rx Packet */
#define ENUM_USB_EP0_CSR_P_PKTRDY            (_ADI_MSK(0x00000001,uint16_t))  /* RXPKTRDY: Rx Packet in Endpoint FIFO */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_EP_TXCSR_P                       Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_EP_TXCSR_P_AUTOSET          15                               /* TxPkRdy Autoset Enable */
#define BITP_USB_EP_TXCSR_P_ISO              14                               /* Isochronous Transfers Enable */
#define BITP_USB_EP_TXCSR_P_DMAREQEN         12                               /* DMA Request Enable Tx EP */
#define BITP_USB_EP_TXCSR_P_FRCDATATGL       11                               /* Force Data Toggle */
#define BITP_USB_EP_TXCSR_P_DMAREQMODE       10                               /* DMA Mode Select */
#define BITP_USB_EP_TXCSR_P_INCOMPTX          7                               /* Incomplete Tx */
#define BITP_USB_EP_TXCSR_P_CLRDATATGL        6                               /* Clear Endpoint Data Toggle */
#define BITP_USB_EP_TXCSR_P_SENTSTALL         5                               /* Sent STALL */
#define BITP_USB_EP_TXCSR_P_SENDSTALL         4                               /* Send STALL */
#define BITP_USB_EP_TXCSR_P_FLUSHFIFO         3                               /* Flush Endpoint FIFO */
#define BITP_USB_EP_TXCSR_P_URUNERR           2                               /* Underrun Error */
#define BITP_USB_EP_TXCSR_P_NEFIFO            1                               /* Not Empty FIFO */
#define BITP_USB_EP_TXCSR_P_TXPKTRDY          0                               /* Tx Packet Ready */

#define BITM_USB_EP_TXCSR_P_AUTOSET          (_ADI_MSK(0x00008000,uint16_t))  /* TxPkRdy Autoset Enable */
#define ENUM_USB_EP_TXCSR_P_NO_AUTOSET       (_ADI_MSK(0x00000000,uint16_t))  /* AUTOSET: Disable Autoset */
#define ENUM_USB_EP_TXCSR_P_AUTOSET          (_ADI_MSK(0x00008000,uint16_t))  /* AUTOSET: Enable Autoset */

#define BITM_USB_EP_TXCSR_P_ISO              (_ADI_MSK(0x00004000,uint16_t))  /* Isochronous Transfers Enable */
#define ENUM_USB_EP_TXCSR_P_ISODIS           (_ADI_MSK(0x00000000,uint16_t))  /* ISO: Disable Tx EP Isochronous Transfers */
#define ENUM_USB_EP_TXCSR_P_ISOEN            (_ADI_MSK(0x00004000,uint16_t))  /* ISO: Enable Tx EP Isochronous Transfers */

#define BITM_USB_EP_TXCSR_P_DMAREQEN         (_ADI_MSK(0x00001000,uint16_t))  /* DMA Request Enable Tx EP */
#define ENUM_USB_EP_TXCSR_P_DMAREQDIS        (_ADI_MSK(0x00000000,uint16_t))  /* DMAREQEN: Disable DMA Request */
#define ENUM_USB_EP_TXCSR_P_DMAREQEN         (_ADI_MSK(0x00001000,uint16_t))  /* DMAREQEN: Enable DMA Request */

#define BITM_USB_EP_TXCSR_P_FRCDATATGL       (_ADI_MSK(0x00000800,uint16_t))  /* Force Data Toggle */
#define ENUM_USB_EP_TXCSR_P_NO_FRCTGL        (_ADI_MSK(0x00000000,uint16_t))  /* FRCDATATGL: No Action */
#define ENUM_USB_EP_TXCSR_P_FRCTGL           (_ADI_MSK(0x00000800,uint16_t))  /* FRCDATATGL: Toggle Endpoint Data */

#define BITM_USB_EP_TXCSR_P_DMAREQMODE       (_ADI_MSK(0x00000400,uint16_t))  /* DMA Mode Select */
#define ENUM_USB_EP_TXCSR_P_DMARQMODE0       (_ADI_MSK(0x00000000,uint16_t))  /* DMAREQMODE: DMA Request Mode 0 */
#define ENUM_USB_EP_TXCSR_P_DMARQMODE1       (_ADI_MSK(0x00000400,uint16_t))  /* DMAREQMODE: DMA Request Mode 1 */

#define BITM_USB_EP_TXCSR_P_INCOMPTX         (_ADI_MSK(0x00000080,uint16_t))  /* Incomplete Tx */
#define ENUM_USB_EP_TXCSR_P_NO_INCOMP        (_ADI_MSK(0x00000000,uint16_t))  /* INCOMPTX: No Status */
#define ENUM_USB_EP_TXCSR_P_INCOMP           (_ADI_MSK(0x00000080,uint16_t))  /* INCOMPTX: Incomplete Tx (Insufficient IN Tokens) */

#define BITM_USB_EP_TXCSR_P_CLRDATATGL       (_ADI_MSK(0x00000040,uint16_t))  /* Clear Endpoint Data Toggle */
#define ENUM_USB_EP_TXCSR_P_NO_CLRTGL        (_ADI_MSK(0x00000000,uint16_t))  /* CLRDATATGL: No Action */
#define ENUM_USB_EP_TXCSR_P_CLRTGL           (_ADI_MSK(0x00000040,uint16_t))  /* CLRDATATGL: Reset EP Data Toggle to 0 */

#define BITM_USB_EP_TXCSR_P_SENTSTALL        (_ADI_MSK(0x00000020,uint16_t))  /* Sent STALL */
#define ENUM_USB_EP_TXCSR_P_NO_STALSNT       (_ADI_MSK(0x00000000,uint16_t))  /* SENTSTALL: No Status */
#define ENUM_USB_EP_TXCSR_P_STALSNT          (_ADI_MSK(0x00000020,uint16_t))  /* SENTSTALL: STALL Handshake Transmitted */

#define BITM_USB_EP_TXCSR_P_SENDSTALL        (_ADI_MSK(0x00000010,uint16_t))  /* Send STALL */
#define ENUM_USB_EP_TXCSR_P_NO_STALL         (_ADI_MSK(0x00000000,uint16_t))  /* SENDSTALL: No Request */
#define ENUM_USB_EP_TXCSR_P_STALL            (_ADI_MSK(0x00000010,uint16_t))  /* SENDSTALL: Request STALL Handshake Transmission */

#define BITM_USB_EP_TXCSR_P_FLUSHFIFO        (_ADI_MSK(0x00000008,uint16_t))  /* Flush Endpoint FIFO */
#define ENUM_USB_EP_TXCSR_P_NO_FLUSH         (_ADI_MSK(0x00000000,uint16_t))  /* FLUSHFIFO: No Flush */
#define ENUM_USB_EP_TXCSR_P_FLUSH            (_ADI_MSK(0x00000008,uint16_t))  /* FLUSHFIFO: Flush endpoint FIFO */

#define BITM_USB_EP_TXCSR_P_URUNERR          (_ADI_MSK(0x00000004,uint16_t))  /* Underrun Error */
#define ENUM_USB_EP_TXCSR_P_NO_URUNERR       (_ADI_MSK(0x00000000,uint16_t))  /* URUNERR: No Status */
#define ENUM_USB_EP_TXCSR_P_URUNERR          (_ADI_MSK(0x00000004,uint16_t))  /* URUNERR: Underrun Error */

#define BITM_USB_EP_TXCSR_P_NEFIFO           (_ADI_MSK(0x00000002,uint16_t))  /* Not Empty FIFO */
#define ENUM_USB_EP_TXCSR_P_NO_FIFONE        (_ADI_MSK(0x00000000,uint16_t))  /* NEFIFO: FIFO Empty */
#define ENUM_USB_EP_TXCSR_P_FIFONE           (_ADI_MSK(0x00000002,uint16_t))  /* NEFIFO: FIFO Not Empty */

#define BITM_USB_EP_TXCSR_P_TXPKTRDY         (_ADI_MSK(0x00000001,uint16_t))  /* Tx Packet Ready */
#define ENUM_USB_EP_TXCSR_P_NO_PKTRDY        (_ADI_MSK(0x00000000,uint16_t))  /* TXPKTRDY: No Tx Packet */
#define ENUM_USB_EP_TXCSR_P_PKTRDY           (_ADI_MSK(0x00000001,uint16_t))  /* TXPKTRDY: Tx Packet in Endpoint FIFO */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_EP_RXMAXP                        Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_EP_RXMAXP_MULTM1            11                               /* Multi-Packets per Micro-frame */
#define BITP_USB_EP_RXMAXP_MAXPAY             0                               /* Maximum Payload */
#define BITM_USB_EP_RXMAXP_MULTM1            (_ADI_MSK(0x00001800,uint16_t))  /* Multi-Packets per Micro-frame */
#define BITM_USB_EP_RXMAXP_MAXPAY            (_ADI_MSK(0x000007FF,uint16_t))  /* Maximum Payload */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_EP_RXCSR_H                       Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_EP_RXCSR_H_AUTOCLR          15                               /* Auto Clear Enable */
#define BITP_USB_EP_RXCSR_H_AUTOREQ          14                               /* Auto Request Clear Enable */
#define BITP_USB_EP_RXCSR_H_DMAREQEN         13                               /* DMA Request Enable Rx EP */
#define BITP_USB_EP_RXCSR_H_PIDERR           12                               /* Packet ID Error */
#define BITP_USB_EP_RXCSR_H_DMAREQMODE       11                               /* DMA Mode Select */
#define BITP_USB_EP_RXCSR_H_DATGLEN          10                               /* Data Toggle Write Enable */
#define BITP_USB_EP_RXCSR_H_DATGL             9                               /* Data Toggle */
#define BITP_USB_EP_RXCSR_H_INCOMPRX          8                               /* Incomplete Rx */
#define BITP_USB_EP_RXCSR_H_CLRDATATGL        7                               /* Clear Endpoint Data Toggle */
#define BITP_USB_EP_RXCSR_H_RXSTALL           6                               /* Rx STALL */
#define BITP_USB_EP_RXCSR_H_REQPKT            5                               /* Request Packet */
#define BITP_USB_EP_RXCSR_H_FLUSHFIFO         4                               /* Flush Endpoint FIFO */
#define BITP_USB_EP_RXCSR_H_NAKTODERR         3                               /* NAK Timeout Data Error */
#define BITP_USB_EP_RXCSR_H_RXTOERR           2                               /* Rx Timeout Error */
#define BITP_USB_EP_RXCSR_H_FIFOFULL          1                               /* FIFO Full */
#define BITP_USB_EP_RXCSR_H_RXPKTRDY          0                               /* Rx Packet Ready */

#define BITM_USB_EP_RXCSR_H_AUTOCLR          (_ADI_MSK(0x00008000,uint16_t))  /* Auto Clear Enable */
#define ENUM_USB_EP_RXCSR_H_NO_AUTOCLR       (_ADI_MSK(0x00000000,uint16_t))  /* AUTOCLR: Disable Auto Clear */
#define ENUM_USB_EP_RXCSR_H_AUTOCLR          (_ADI_MSK(0x00008000,uint16_t))  /* AUTOCLR: Enable Auto Clear */

#define BITM_USB_EP_RXCSR_H_AUTOREQ          (_ADI_MSK(0x00004000,uint16_t))  /* Auto Request Clear Enable */
#define ENUM_USB_EP_RXCSR_H_NO_AUTOREQ       (_ADI_MSK(0x00000000,uint16_t))  /* AUTOREQ: Disable Auto Request Clear */
#define ENUM_USB_EP_RXCSR_H_AUTOREQ          (_ADI_MSK(0x00004000,uint16_t))  /* AUTOREQ: Enable Auto Request Clear */

#define BITM_USB_EP_RXCSR_H_DMAREQEN         (_ADI_MSK(0x00002000,uint16_t))  /* DMA Request Enable Rx EP */
#define ENUM_USB_EP_RXCSR_H_DMAREQDIS        (_ADI_MSK(0x00000000,uint16_t))  /* DMAREQEN: Disable DMA Request */
#define ENUM_USB_EP_RXCSR_H_DMAREQEN         (_ADI_MSK(0x00002000,uint16_t))  /* DMAREQEN: Enable DMA Request */

#define BITM_USB_EP_RXCSR_H_PIDERR           (_ADI_MSK(0x00001000,uint16_t))  /* Packet ID Error */
#define ENUM_USB_EP_RXCSR_H_NO_PIDERR        (_ADI_MSK(0x00000000,uint16_t))  /* PIDERR: No Status */
#define ENUM_USB_EP_RXCSR_H_PIDERR           (_ADI_MSK(0x00001000,uint16_t))  /* PIDERR: PID Error */

#define BITM_USB_EP_RXCSR_H_DMAREQMODE       (_ADI_MSK(0x00000800,uint16_t))  /* DMA Mode Select */
#define ENUM_USB_EP_RXCSR_H_DMARQMODE0       (_ADI_MSK(0x00000000,uint16_t))  /* DMAREQMODE: DMA Request Mode 0 */
#define ENUM_USB_EP_RXCSR_H_DMARQMODE1       (_ADI_MSK(0x00000800,uint16_t))  /* DMAREQMODE: DMA Request Mode 1 */

#define BITM_USB_EP_RXCSR_H_DATGLEN          (_ADI_MSK(0x00000400,uint16_t))  /* Data Toggle Write Enable */
#define ENUM_USB_EP_RXCSR_H_DATGLDIS         (_ADI_MSK(0x00000000,uint16_t))  /* DATGLEN: Disable Write to DATGL */
#define ENUM_USB_EP_RXCSR_H_DATGLEN          (_ADI_MSK(0x00000400,uint16_t))  /* DATGLEN: Enable Write to DATGL */

#define BITM_USB_EP_RXCSR_H_DATGL            (_ADI_MSK(0x00000200,uint16_t))  /* Data Toggle */
#define ENUM_USB_EP_RXCSR_H_NO_DATGL         (_ADI_MSK(0x00000000,uint16_t))  /* DATGL: DATA0 is Set */
#define ENUM_USB_EP_RXCSR_H_DATGL            (_ADI_MSK(0x00000200,uint16_t))  /* DATGL: DATA1 is Set */

#define BITM_USB_EP_RXCSR_H_INCOMPRX         (_ADI_MSK(0x00000100,uint16_t))  /* Incomplete Rx */
#define ENUM_USB_EP_RXCSR_H_NO_INCOMP        (_ADI_MSK(0x00000000,uint16_t))  /* INCOMPRX: No Status */
#define ENUM_USB_EP_RXCSR_H_INCOMP           (_ADI_MSK(0x00000100,uint16_t))  /* INCOMPRX: Incomplete Rx */

#define BITM_USB_EP_RXCSR_H_CLRDATATGL       (_ADI_MSK(0x00000080,uint16_t))  /* Clear Endpoint Data Toggle */
#define ENUM_USB_EP_RXCSR_H_NO_CLRTGL        (_ADI_MSK(0x00000000,uint16_t))  /* CLRDATATGL: No Action */
#define ENUM_USB_EP_RXCSR_H_CLRTGL           (_ADI_MSK(0x00000080,uint16_t))  /* CLRDATATGL: Reset EP Data Toggle to 0 */

#define BITM_USB_EP_RXCSR_H_RXSTALL          (_ADI_MSK(0x00000040,uint16_t))  /* Rx STALL */
#define ENUM_USB_EP_RXCSR_H_NO_RXSTALL       (_ADI_MSK(0x00000000,uint16_t))  /* RXSTALL: No Status */
#define ENUM_USB_EP_RXCSR_H_RXSTALL          (_ADI_MSK(0x00000040,uint16_t))  /* RXSTALL: Stall Received from Device */

#define BITM_USB_EP_RXCSR_H_REQPKT           (_ADI_MSK(0x00000020,uint16_t))  /* Request Packet */
#define ENUM_USB_EP_RXCSR_H_NO_REQPKT        (_ADI_MSK(0x00000000,uint16_t))  /* REQPKT: No Request */
#define ENUM_USB_EP_RXCSR_H_REQPKT           (_ADI_MSK(0x00000020,uint16_t))  /* REQPKT: Send IN Tokens to Device */

#define BITM_USB_EP_RXCSR_H_FLUSHFIFO        (_ADI_MSK(0x00000010,uint16_t))  /* Flush Endpoint FIFO */
#define ENUM_USB_EP_RXCSR_H_NO_FLUSH         (_ADI_MSK(0x00000000,uint16_t))  /* FLUSHFIFO: No Flush */
#define ENUM_USB_EP_RXCSR_H_FLUSH            (_ADI_MSK(0x00000010,uint16_t))  /* FLUSHFIFO: Flush Endpoint FIFO */

#define BITM_USB_EP_RXCSR_H_NAKTODERR        (_ADI_MSK(0x00000008,uint16_t))  /* NAK Timeout Data Error */
#define ENUM_USB_EP_RXCSR_H_NO_NAKTO         (_ADI_MSK(0x00000000,uint16_t))  /* NAKTODERR: No Status */
#define ENUM_USB_EP_RXCSR_H_NAKTO            (_ADI_MSK(0x00000008,uint16_t))  /* NAKTODERR: NAK Timeout Data Error */

#define BITM_USB_EP_RXCSR_H_RXTOERR          (_ADI_MSK(0x00000004,uint16_t))  /* Rx Timeout Error */
#define ENUM_USB_EP_RXCSR_H_NO_RXTOERR       (_ADI_MSK(0x00000000,uint16_t))  /* RXTOERR: No Status */
#define ENUM_USB_EP_RXCSR_H_RXTOERR          (_ADI_MSK(0x00000004,uint16_t))  /* RXTOERR: Rx Timeout Error */

#define BITM_USB_EP_RXCSR_H_FIFOFULL         (_ADI_MSK(0x00000002,uint16_t))  /* FIFO Full */
#define ENUM_USB_EP_RXCSR_H_NO_FIFOFUL       (_ADI_MSK(0x00000000,uint16_t))  /* FIFOFULL: No Status */
#define ENUM_USB_EP_RXCSR_H_FIFOFUL          (_ADI_MSK(0x00000002,uint16_t))  /* FIFOFULL: FIFO Full */

#define BITM_USB_EP_RXCSR_H_RXPKTRDY         (_ADI_MSK(0x00000001,uint16_t))  /* Rx Packet Ready */
#define ENUM_USB_EP_RXCSR_H_NO_PKTRDY        (_ADI_MSK(0x00000000,uint16_t))  /* RXPKTRDY: No Rx Packet */
#define ENUM_USB_EP_RXCSR_H_PKTRDY           (_ADI_MSK(0x00000001,uint16_t))  /* RXPKTRDY: Rx Packet in Endpoint FIFO */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_EP_RXCSR_P                       Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_EP_RXCSR_P_AUTOCLR          15                               /* Auto Clear Enable */
#define BITP_USB_EP_RXCSR_P_ISO              14                               /* Isochronous Transfers */
#define BITP_USB_EP_RXCSR_P_DMAREQEN         13                               /* DMA Request Enable Rx EP */
#define BITP_USB_EP_RXCSR_P_DNYETPERR        12                               /* Disable NYET Handshake */
#define BITP_USB_EP_RXCSR_P_DMAREQMODE       11                               /* DMA Mode Select */
#define BITP_USB_EP_RXCSR_P_INCOMPRX          8                               /* Incomplete Rx */
#define BITP_USB_EP_RXCSR_P_CLRDATATGL        7                               /* Clear Endpoint Data Toggle */
#define BITP_USB_EP_RXCSR_P_SENTSTALL         6                               /* Sent STALL */
#define BITP_USB_EP_RXCSR_P_SENDSTALL         5                               /* Send STALL */
#define BITP_USB_EP_RXCSR_P_FLUSHFIFO         4                               /* Flush Endpoint FIFO */
#define BITP_USB_EP_RXCSR_P_DATAERR           3                               /* Data Error */
#define BITP_USB_EP_RXCSR_P_ORUNERR           2                               /* OUT Run Error */
#define BITP_USB_EP_RXCSR_P_FIFOFULL          1                               /* FIFO Full */
#define BITP_USB_EP_RXCSR_P_RXPKTRDY          0                               /* Rx Packet Ready */

#define BITM_USB_EP_RXCSR_P_AUTOCLR          (_ADI_MSK(0x00008000,uint16_t))  /* Auto Clear Enable */
#define ENUM_USB_EP_RXCSR_P_NO_AUTOCLR       (_ADI_MSK(0x00000000,uint16_t))  /* AUTOCLR: Disable Auto Clear */
#define ENUM_USB_EP_RXCSR_P_AUTOCLR          (_ADI_MSK(0x00008000,uint16_t))  /* AUTOCLR: Enable Auto Clear */

#define BITM_USB_EP_RXCSR_P_ISO              (_ADI_MSK(0x00004000,uint16_t))  /* Isochronous Transfers */
#define ENUM_USB_EP_RXCSR_P_ISODIS           (_ADI_MSK(0x00000000,uint16_t))  /* ISO: This bit should be cleared for bulk or interrupt transfers. */
#define ENUM_USB_EP_RXCSR_P_ISOEN            (_ADI_MSK(0x00004000,uint16_t))  /* ISO: This bit should be set for isochronous transfers. */

#define BITM_USB_EP_RXCSR_P_DMAREQEN         (_ADI_MSK(0x00002000,uint16_t))  /* DMA Request Enable Rx EP */
#define ENUM_USB_EP_RXCSR_P_DMAREQDIS        (_ADI_MSK(0x00000000,uint16_t))  /* DMAREQEN: Disable DMA Request */
#define ENUM_USB_EP_RXCSR_P_DMAREQEN         (_ADI_MSK(0x00002000,uint16_t))  /* DMAREQEN: Enable DMA Request */

#define BITM_USB_EP_RXCSR_P_DNYETPERR        (_ADI_MSK(0x00001000,uint16_t))  /* Disable NYET Handshake */
#define ENUM_USB_EP_RXCSR_P_DNYTERREN        (_ADI_MSK(0x00000000,uint16_t))  /* DNYETPERR: Enable NYET Handshake */
#define ENUM_USB_EP_RXCSR_P_DNYTERRDIS       (_ADI_MSK(0x00001000,uint16_t))  /* DNYETPERR: Disable NYET Handshake */

#define BITM_USB_EP_RXCSR_P_DMAREQMODE       (_ADI_MSK(0x00000800,uint16_t))  /* DMA Mode Select */
#define ENUM_USB_EP_RXCSR_P_DMARQMODE0       (_ADI_MSK(0x00000000,uint16_t))  /* DMAREQMODE: DMA Request Mode 0 */
#define ENUM_USB_EP_RXCSR_P_DMARQMODE1       (_ADI_MSK(0x00000800,uint16_t))  /* DMAREQMODE: DMA Request Mode 1 */

#define BITM_USB_EP_RXCSR_P_INCOMPRX         (_ADI_MSK(0x00000100,uint16_t))  /* Incomplete Rx */
#define ENUM_USB_EP_RXCSR_P_NO_INCOMP        (_ADI_MSK(0x00000000,uint16_t))  /* INCOMPRX: No Status */
#define ENUM_USB_EP_RXCSR_P_INCOMP           (_ADI_MSK(0x00000100,uint16_t))  /* INCOMPRX: Incomplete Rx */

#define BITM_USB_EP_RXCSR_P_CLRDATATGL       (_ADI_MSK(0x00000080,uint16_t))  /* Clear Endpoint Data Toggle */
#define ENUM_USB_EP_RXCSR_P_NO_CLRTGL        (_ADI_MSK(0x00000000,uint16_t))  /* CLRDATATGL: No Action */
#define ENUM_USB_EP_RXCSR_P_CLRTGL           (_ADI_MSK(0x00000080,uint16_t))  /* CLRDATATGL: Reset EP Data Toggle to 0 */

#define BITM_USB_EP_RXCSR_P_SENTSTALL        (_ADI_MSK(0x00000040,uint16_t))  /* Sent STALL */
#define ENUM_USB_EP_RXCSR_P_NO_STALSNT       (_ADI_MSK(0x00000000,uint16_t))  /* SENTSTALL: No Status */
#define ENUM_USB_EP_RXCSR_P_STALSNT          (_ADI_MSK(0x00000040,uint16_t))  /* SENTSTALL: STALL Handshake Transmitted */

#define BITM_USB_EP_RXCSR_P_SENDSTALL        (_ADI_MSK(0x00000020,uint16_t))  /* Send STALL */
#define ENUM_USB_EP_RXCSR_P_NO_STALL         (_ADI_MSK(0x00000000,uint16_t))  /* SENDSTALL: No Action */
#define ENUM_USB_EP_RXCSR_P_STALL            (_ADI_MSK(0x00000020,uint16_t))  /* SENDSTALL: Request STALL Handshake */

#define BITM_USB_EP_RXCSR_P_FLUSHFIFO        (_ADI_MSK(0x00000010,uint16_t))  /* Flush Endpoint FIFO */
#define ENUM_USB_EP_RXCSR_P_NO_FLUSH         (_ADI_MSK(0x00000000,uint16_t))  /* FLUSHFIFO: No Flush */
#define ENUM_USB_EP_RXCSR_P_FLUSH            (_ADI_MSK(0x00000010,uint16_t))  /* FLUSHFIFO: Flush Endpoint FIFO */

#define BITM_USB_EP_RXCSR_P_DATAERR          (_ADI_MSK(0x00000008,uint16_t))  /* Data Error */
#define ENUM_USB_EP_RXCSR_P_NO_DATAERR       (_ADI_MSK(0x00000000,uint16_t))  /* DATAERR: No Status */
#define ENUM_USB_EP_RXCSR_P_DATAERR          (_ADI_MSK(0x00000008,uint16_t))  /* DATAERR: Data Error */

#define BITM_USB_EP_RXCSR_P_ORUNERR          (_ADI_MSK(0x00000004,uint16_t))  /* OUT Run Error */
#define ENUM_USB_EP_RXCSR_P_NO_ORUNERR       (_ADI_MSK(0x00000000,uint16_t))  /* ORUNERR: No Status */
#define ENUM_USB_EP_RXCSR_P_ORUNERR          (_ADI_MSK(0x00000004,uint16_t))  /* ORUNERR: OUT Run Error */

#define BITM_USB_EP_RXCSR_P_FIFOFULL         (_ADI_MSK(0x00000002,uint16_t))  /* FIFO Full */
#define ENUM_USB_EP_RXCSR_P_NO_FIFOFUL       (_ADI_MSK(0x00000000,uint16_t))  /* FIFOFULL: No Status */
#define ENUM_USB_EP_RXCSR_P_FIFOFUL          (_ADI_MSK(0x00000002,uint16_t))  /* FIFOFULL: FIFO Full */

#define BITM_USB_EP_RXCSR_P_RXPKTRDY         (_ADI_MSK(0x00000001,uint16_t))  /* Rx Packet Ready */
#define ENUM_USB_EP_RXCSR_P_NO_PKTRDY        (_ADI_MSK(0x00000000,uint16_t))  /* RXPKTRDY: No Rx Packet */
#define ENUM_USB_EP_RXCSR_P_PKTRDY           (_ADI_MSK(0x00000001,uint16_t))  /* RXPKTRDY: Rx Packet in Endpoint FIFO */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_EP0_CNT                          Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_EP0_CNT_RXCNT                0                               /* Rx Byte Count Value */
#define BITM_USB_EP0_CNT_RXCNT               (_ADI_MSK(0x0000007F,uint16_t))  /* Rx Byte Count Value */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_EP_RXCNT                         Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_EP_RXCNT_EPRXCNT             0                               /* EP Rx Count */
#define BITM_USB_EP_RXCNT_EPRXCNT            (_ADI_MSK(0x00003FFF,uint16_t))  /* EP Rx Count */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_EP0_TYPE                         Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_EP0_TYPE_SPEED               0                               /* Speed of Operation Value */
#define BITM_USB_EP0_TYPE_SPEED              (_ADI_MSK(0x00000003,uint8_t))   /* Speed of Operation Value */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_EP_TXTYPE                        Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_EP_TXTYPE_SPEED              6                               /* Speed of Operation Value */
#define BITP_USB_EP_TXTYPE_PROTOCOL           4                               /* Protocol for Transfer */
#define BITP_USB_EP_TXTYPE_TGTEP              0                               /* Target Endpoint Number */

#define BITM_USB_EP_TXTYPE_SPEED             (_ADI_MSK(0x000000C0,uint8_t))   /* Speed of Operation Value */
#define ENUM_USB_EP_TXTYPE_UNUSED            (_ADI_MSK(0x00000000,uint8_t))   /* SPEED: Same Speed as the Core */
#define ENUM_USB_EP_TXTYPE_HIGHSPEED         (_ADI_MSK(0x00000040,uint8_t))   /* SPEED: High Speed */
#define ENUM_USB_EP_TXTYPE_FULLSPEED         (_ADI_MSK(0x00000080,uint8_t))   /* SPEED: Full Speed */
#define ENUM_USB_EP_TXTYPE_LOWSPEED          (_ADI_MSK(0x000000C0,uint8_t))   /* SPEED: Low Speed */

#define BITM_USB_EP_TXTYPE_PROTOCOL          (_ADI_MSK(0x00000030,uint8_t))   /* Protocol for Transfer */
#define ENUM_USB_EP_TXTYPE_CONTROL           (_ADI_MSK(0x00000000,uint8_t))   /* PROTOCOL: Control */
#define ENUM_USB_EP_TXTYPE_ISO               (_ADI_MSK(0x00000010,uint8_t))   /* PROTOCOL: Isochronous */
#define ENUM_USB_EP_TXTYPE_BULK              (_ADI_MSK(0x00000020,uint8_t))   /* PROTOCOL: Bulk */
#define ENUM_USB_EP_TXTYPE_INT               (_ADI_MSK(0x00000030,uint8_t))   /* PROTOCOL: Interrupt */

#define BITM_USB_EP_TXTYPE_TGTEP             (_ADI_MSK(0x0000000F,uint8_t))   /* Target Endpoint Number */
#define ENUM_USB_EP_TXTYPE_TGTEP0            (_ADI_MSK(0x00000000,uint8_t))   /* TGTEP: Endpoint 0 */
#define ENUM_USB_EP_TXTYPE_TGTEP1            (_ADI_MSK(0x00000001,uint8_t))   /* TGTEP: Endpoint 1 */
#define ENUM_USB_EP_TXTYPE_TGTEP10           (_ADI_MSK(0x0000000A,uint8_t))   /* TGTEP: Endpoint 10 */
#define ENUM_USB_EP_TXTYPE_TGTEP11           (_ADI_MSK(0x0000000B,uint8_t))   /* TGTEP: Endpoint 11 */
#define ENUM_USB_EP_TXTYPE_TGTEP12           (_ADI_MSK(0x0000000C,uint8_t))   /* TGTEP: Endpoint 12 */
#define ENUM_USB_EP_TXTYPE_TGTEP13           (_ADI_MSK(0x0000000D,uint8_t))   /* TGTEP: Endpoint 13 */
#define ENUM_USB_EP_TXTYPE_TGTEP14           (_ADI_MSK(0x0000000E,uint8_t))   /* TGTEP: Endpoint 14 */
#define ENUM_USB_EP_TXTYPE_TGTEP15           (_ADI_MSK(0x0000000F,uint8_t))   /* TGTEP: Endpoint 15 */
#define ENUM_USB_EP_TXTYPE_TGTEP2            (_ADI_MSK(0x00000002,uint8_t))   /* TGTEP: Endpoint 2 */
#define ENUM_USB_EP_TXTYPE_TGTEP3            (_ADI_MSK(0x00000003,uint8_t))   /* TGTEP: Endpoint 3 */
#define ENUM_USB_EP_TXTYPE_TGTEP4            (_ADI_MSK(0x00000004,uint8_t))   /* TGTEP: Endpoint 4 */
#define ENUM_USB_EP_TXTYPE_TGTEP5            (_ADI_MSK(0x00000005,uint8_t))   /* TGTEP: Endpoint 5 */
#define ENUM_USB_EP_TXTYPE_TGTEP6            (_ADI_MSK(0x00000006,uint8_t))   /* TGTEP: Endpoint 6 */
#define ENUM_USB_EP_TXTYPE_TGTEP7            (_ADI_MSK(0x00000007,uint8_t))   /* TGTEP: Endpoint 7 */
#define ENUM_USB_EP_TXTYPE_TGTEP8            (_ADI_MSK(0x00000008,uint8_t))   /* TGTEP: Endpoint 8 */
#define ENUM_USB_EP_TXTYPE_TGTEP9            (_ADI_MSK(0x00000009,uint8_t))   /* TGTEP: Endpoint 9 */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_EP0_NAKLIMIT                     Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_EP0_NAKLIMIT_VALUE           0                               /* Endpoint 0 Timeout Value (in Frames) */
#define BITM_USB_EP0_NAKLIMIT_VALUE          (_ADI_MSK(0x0000001F,uint8_t))   /* Endpoint 0 Timeout Value (in Frames) */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_EP_RXTYPE                        Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_EP_RXTYPE_SPEED              6                               /* Speed of Operation Value */
#define BITP_USB_EP_RXTYPE_PROTOCOL           4                               /* Protocol for Transfer */
#define BITP_USB_EP_RXTYPE_TGTEP              0                               /* Target Endpoint Number */

#define BITM_USB_EP_RXTYPE_SPEED             (_ADI_MSK(0x000000C0,uint8_t))   /* Speed of Operation Value */
#define ENUM_USB_EP_RXTYPE_UNUSED            (_ADI_MSK(0x00000000,uint8_t))   /* SPEED: Same Speed as the Core */
#define ENUM_USB_EP_RXTYPE_HIGHSPEED         (_ADI_MSK(0x00000040,uint8_t))   /* SPEED: High Speed */
#define ENUM_USB_EP_RXTYPE_FULLSPEED         (_ADI_MSK(0x00000080,uint8_t))   /* SPEED: Full Speed */
#define ENUM_USB_EP_RXTYPE_LOWSPEED          (_ADI_MSK(0x000000C0,uint8_t))   /* SPEED: Low Speed */

#define BITM_USB_EP_RXTYPE_PROTOCOL          (_ADI_MSK(0x00000030,uint8_t))   /* Protocol for Transfer */
#define ENUM_USB_EP_RXTYPE_CONTROL           (_ADI_MSK(0x00000000,uint8_t))   /* PROTOCOL: Control */
#define ENUM_USB_EP_RXTYPE_ISO               (_ADI_MSK(0x00000010,uint8_t))   /* PROTOCOL: Isochronous */
#define ENUM_USB_EP_RXTYPE_BULK              (_ADI_MSK(0x00000020,uint8_t))   /* PROTOCOL: Bulk */
#define ENUM_USB_EP_RXTYPE_INT               (_ADI_MSK(0x00000030,uint8_t))   /* PROTOCOL: Interrupt */

#define BITM_USB_EP_RXTYPE_TGTEP             (_ADI_MSK(0x0000000F,uint8_t))   /* Target Endpoint Number */
#define ENUM_USB_EP_RXTYPE_TGTEP0            (_ADI_MSK(0x00000000,uint8_t))   /* TGTEP: Endpoint 0 */
#define ENUM_USB_EP_RXTYPE_TGTEP1            (_ADI_MSK(0x00000001,uint8_t))   /* TGTEP: Endpoint 1 */
#define ENUM_USB_EP_RXTYPE_TGTEP10           (_ADI_MSK(0x0000000A,uint8_t))   /* TGTEP: Endpoint 10 */
#define ENUM_USB_EP_RXTYPE_TGTEP11           (_ADI_MSK(0x0000000B,uint8_t))   /* TGTEP: Endpoint 11 */
#define ENUM_USB_EP_RXTYPE_TGTEP12           (_ADI_MSK(0x0000000C,uint8_t))   /* TGTEP: Endpoint 12 */
#define ENUM_USB_EP_RXTYPE_TGTEP13           (_ADI_MSK(0x0000000D,uint8_t))   /* TGTEP: Endpoint 13 */
#define ENUM_USB_EP_RXTYPE_TGTEP14           (_ADI_MSK(0x0000000E,uint8_t))   /* TGTEP: Endpoint 14 */
#define ENUM_USB_EP_RXTYPE_TGTEP15           (_ADI_MSK(0x0000000F,uint8_t))   /* TGTEP: Endpoint 15 */
#define ENUM_USB_EP_RXTYPE_TGTEP2            (_ADI_MSK(0x00000002,uint8_t))   /* TGTEP: Endpoint 2 */
#define ENUM_USB_EP_RXTYPE_TGTEP3            (_ADI_MSK(0x00000003,uint8_t))   /* TGTEP: Endpoint 3 */
#define ENUM_USB_EP_RXTYPE_TGTEP4            (_ADI_MSK(0x00000004,uint8_t))   /* TGTEP: Endpoint 4 */
#define ENUM_USB_EP_RXTYPE_TGTEP5            (_ADI_MSK(0x00000005,uint8_t))   /* TGTEP: Endpoint 5 */
#define ENUM_USB_EP_RXTYPE_TGTEP6            (_ADI_MSK(0x00000006,uint8_t))   /* TGTEP: Endpoint 6 */
#define ENUM_USB_EP_RXTYPE_TGTEP7            (_ADI_MSK(0x00000007,uint8_t))   /* TGTEP: Endpoint 7 */
#define ENUM_USB_EP_RXTYPE_TGTEP8            (_ADI_MSK(0x00000008,uint8_t))   /* TGTEP: Endpoint 8 */
#define ENUM_USB_EP_RXTYPE_TGTEP9            (_ADI_MSK(0x00000009,uint8_t))   /* TGTEP: Endpoint 9 */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_EP0_CFGDATA                      Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_EP0_CFGDATA_MPRX             7                               /* Multi-Packet Aggregate for Rx Enable */
#define BITP_USB_EP0_CFGDATA_MPTX             6                               /* Multi-Packet Split for Tx Enable */
#define BITP_USB_EP0_CFGDATA_BIGEND           5                               /* Big Endian Data */
#define BITP_USB_EP0_CFGDATA_HBRX             4                               /* High Bandwidth Rx Enable */
#define BITP_USB_EP0_CFGDATA_HBTX             3                               /* High Bandwidth Tx Enable */
#define BITP_USB_EP0_CFGDATA_DYNFIFO          2                               /* Dynamic FIFO Size Enable */
#define BITP_USB_EP0_CFGDATA_SOFTCON          1                               /* Soft Connect Enable */
#define BITP_USB_EP0_CFGDATA_UTMIWID          0                               /* UTMI Data Width */

#define BITM_USB_EP0_CFGDATA_MPRX            (_ADI_MSK(0x00000080,uint8_t))   /* Multi-Packet Aggregate for Rx Enable */
#define ENUM_USB_EP0_CFGDATA_MPRXDIS         (_ADI_MSK(0x00000000,uint8_t))   /* MPRX: No Aggregate Rx Bulk Packets */
#define ENUM_USB_EP0_CFGDATA_MPRXEN          (_ADI_MSK(0x00000080,uint8_t))   /* MPRX: Aggregate Rx Bulk Packets */

#define BITM_USB_EP0_CFGDATA_MPTX            (_ADI_MSK(0x00000040,uint8_t))   /* Multi-Packet Split for Tx Enable */
#define ENUM_USB_EP0_CFGDATA_MPTXDIS         (_ADI_MSK(0x00000000,uint8_t))   /* MPTX: No Split Tx Bulk Packets */
#define ENUM_USB_EP0_CFGDATA_MPTXEN          (_ADI_MSK(0x00000040,uint8_t))   /* MPTX: Split Tx Bulk Packets */

#define BITM_USB_EP0_CFGDATA_BIGEND          (_ADI_MSK(0x00000020,uint8_t))   /* Big Endian Data */
#define ENUM_USB_EP0_CFGDATA_BIGENDDIS       (_ADI_MSK(0x00000000,uint8_t))   /* BIGEND: Little Endian Configuration */
#define ENUM_USB_EP0_CFGDATA_BIGENDEN        (_ADI_MSK(0x00000020,uint8_t))   /* BIGEND: Big Endian Configuration */

#define BITM_USB_EP0_CFGDATA_HBRX            (_ADI_MSK(0x00000010,uint8_t))   /* High Bandwidth Rx Enable */
#define ENUM_USB_EP0_CFGDATA_HBRXDIS         (_ADI_MSK(0x00000000,uint8_t))   /* HBRX: No High Bandwidth Rx */
#define ENUM_USB_EP0_CFGDATA_HBRXEN          (_ADI_MSK(0x00000010,uint8_t))   /* HBRX: High Bandwidth Rx */

#define BITM_USB_EP0_CFGDATA_HBTX            (_ADI_MSK(0x00000008,uint8_t))   /* High Bandwidth Tx Enable */
#define ENUM_USB_EP0_CFGDATA_HBTXDIS         (_ADI_MSK(0x00000000,uint8_t))   /* HBTX: No High Bandwidth Tx */
#define ENUM_USB_EP0_CFGDATA_HBTXEN          (_ADI_MSK(0x00000008,uint8_t))   /* HBTX: High Bandwidth Tx */

#define BITM_USB_EP0_CFGDATA_DYNFIFO         (_ADI_MSK(0x00000004,uint8_t))   /* Dynamic FIFO Size Enable */
#define ENUM_USB_EP0_CFGDATA_DYNSZDIS        (_ADI_MSK(0x00000000,uint8_t))   /* DYNFIFO: No Dynamic FIFO Size */
#define ENUM_USB_EP0_CFGDATA_DYNSZEN         (_ADI_MSK(0x00000004,uint8_t))   /* DYNFIFO: Dynamic FIFO Size */

#define BITM_USB_EP0_CFGDATA_SOFTCON         (_ADI_MSK(0x00000002,uint8_t))   /* Soft Connect Enable */
#define ENUM_USB_EP0_CFGDATA_SFTCONDIS       (_ADI_MSK(0x00000000,uint8_t))   /* SOFTCON: No Soft Connect */
#define ENUM_USB_EP0_CFGDATA_SFTCONEN        (_ADI_MSK(0x00000002,uint8_t))   /* SOFTCON: Soft Connect */

#define BITM_USB_EP0_CFGDATA_UTMIWID         (_ADI_MSK(0x00000001,uint8_t))   /* UTMI Data Width */
#define ENUM_USB_EP0_CFGDATA_UTMIWID8        (_ADI_MSK(0x00000000,uint8_t))   /* UTMIWID: 8-bit UTMI Data Width */
#define ENUM_USB_EP0_CFGDATA_UTMIWID16       (_ADI_MSK(0x00000001,uint8_t))   /* UTMIWID: 16-bit UTMI Data Width */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_DMA_IRQ                          Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_DMA_IRQ_D7                   7                               /* DMA 7 Interrupt Pending Status */
#define BITP_USB_DMA_IRQ_D6                   6                               /* DMA 6 Interrupt Pending Status */
#define BITP_USB_DMA_IRQ_D5                   5                               /* DMA 5 Interrupt Pending Status */
#define BITP_USB_DMA_IRQ_D4                   4                               /* DMA 4 Interrupt Pending Status */
#define BITP_USB_DMA_IRQ_D3                   3                               /* DMA 3 Interrupt Pending Status */
#define BITP_USB_DMA_IRQ_D2                   2                               /* DMA 2 Interrupt Pending Status */
#define BITP_USB_DMA_IRQ_D1                   1                               /* DMA 1 Interrupt Pending Status */
#define BITP_USB_DMA_IRQ_D0                   0                               /* DMA 0 Interrupt Pending Status */
#define BITM_USB_DMA_IRQ_D7                  (_ADI_MSK(0x00000080,uint8_t))   /* DMA 7 Interrupt Pending Status */
#define BITM_USB_DMA_IRQ_D6                  (_ADI_MSK(0x00000040,uint8_t))   /* DMA 6 Interrupt Pending Status */
#define BITM_USB_DMA_IRQ_D5                  (_ADI_MSK(0x00000020,uint8_t))   /* DMA 5 Interrupt Pending Status */
#define BITM_USB_DMA_IRQ_D4                  (_ADI_MSK(0x00000010,uint8_t))   /* DMA 4 Interrupt Pending Status */
#define BITM_USB_DMA_IRQ_D3                  (_ADI_MSK(0x00000008,uint8_t))   /* DMA 3 Interrupt Pending Status */
#define BITM_USB_DMA_IRQ_D2                  (_ADI_MSK(0x00000004,uint8_t))   /* DMA 2 Interrupt Pending Status */
#define BITM_USB_DMA_IRQ_D1                  (_ADI_MSK(0x00000002,uint8_t))   /* DMA 1 Interrupt Pending Status */
#define BITM_USB_DMA_IRQ_D0                  (_ADI_MSK(0x00000001,uint8_t))   /* DMA 0 Interrupt Pending Status */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_DMA_CTL                          Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_DMA_CTL_BRSTM                9                               /* Burst Mode */
#define BITP_USB_DMA_CTL_ERR                  8                               /* Bus Error */
#define BITP_USB_DMA_CTL_EP                   4                               /* DMA Channel Endpoint Assignment */
#define BITP_USB_DMA_CTL_IE                   3                               /* DMA Interrupt Enable */
#define BITP_USB_DMA_CTL_MODE                 2                               /* DMA Mode */
#define BITP_USB_DMA_CTL_DIR                  1                               /* DMA Transfer Direction */
#define BITP_USB_DMA_CTL_EN                   0                               /* DMA Enable */

#define BITM_USB_DMA_CTL_BRSTM               (_ADI_MSK(0x00000600,uint16_t))  /* Burst Mode */
#define ENUM_USB_DMA_CTL_BRSTM00             (_ADI_MSK(0x00000000,uint16_t))  /* BRSTM: Unspecified Length */
#define ENUM_USB_DMA_CTL_BRSTM01             (_ADI_MSK(0x00000200,uint16_t))  /* BRSTM: INCR4 or Unspecified Length */
#define ENUM_USB_DMA_CTL_BRSTM10             (_ADI_MSK(0x00000400,uint16_t))  /* BRSTM: INCR8, INCR4, or Unspecified Length */
#define ENUM_USB_DMA_CTL_BRSTM11             (_ADI_MSK(0x00000600,uint16_t))  /* BRSTM: INCR16, INCR8, INCR4, or Unspecified Length */

#define BITM_USB_DMA_CTL_ERR                 (_ADI_MSK(0x00000100,uint16_t))  /* Bus Error */
#define ENUM_USB_DMA_CTL_NO_DMAERR           (_ADI_MSK(0x00000000,uint16_t))  /* ERR: No Status */
#define ENUM_USB_DMA_CTL_DMAERR              (_ADI_MSK(0x00000100,uint16_t))  /* ERR: Bus Error */

#define BITM_USB_DMA_CTL_EP                  (_ADI_MSK(0x000000F0,uint16_t))  /* DMA Channel Endpoint Assignment */
#define ENUM_USB_DMA_CTL_DMAEP0              (_ADI_MSK(0x00000000,uint16_t))  /* EP: Endpoint 0 */
#define ENUM_USB_DMA_CTL_DMAEP1              (_ADI_MSK(0x00000010,uint16_t))  /* EP: Endpoint 1 */
#define ENUM_USB_DMA_CTL_DMAEP10             (_ADI_MSK(0x000000A0,uint16_t))  /* EP: Endpoint 10 */
#define ENUM_USB_DMA_CTL_DMAEP11             (_ADI_MSK(0x000000B0,uint16_t))  /* EP: Endpoint 11 */
#define ENUM_USB_DMA_CTL_DMAEP12             (_ADI_MSK(0x000000C0,uint16_t))  /* EP: Endpoint 12 */
#define ENUM_USB_DMA_CTL_DMAEP13             (_ADI_MSK(0x000000D0,uint16_t))  /* EP: Endpoint 13 */
#define ENUM_USB_DMA_CTL_DMAEP14             (_ADI_MSK(0x000000E0,uint16_t))  /* EP: Endpoint 14 */
#define ENUM_USB_DMA_CTL_DMAEP15             (_ADI_MSK(0x000000F0,uint16_t))  /* EP: Endpoint 15 */
#define ENUM_USB_DMA_CTL_DMAEP2              (_ADI_MSK(0x00000020,uint16_t))  /* EP: Endpoint 2 */
#define ENUM_USB_DMA_CTL_DMAEP3              (_ADI_MSK(0x00000030,uint16_t))  /* EP: Endpoint 3 */
#define ENUM_USB_DMA_CTL_DMAEP4              (_ADI_MSK(0x00000040,uint16_t))  /* EP: Endpoint 4 */
#define ENUM_USB_DMA_CTL_DMAEP5              (_ADI_MSK(0x00000050,uint16_t))  /* EP: Endpoint 5 */
#define ENUM_USB_DMA_CTL_DMAEP6              (_ADI_MSK(0x00000060,uint16_t))  /* EP: Endpoint 6 */
#define ENUM_USB_DMA_CTL_DMAEP7              (_ADI_MSK(0x00000070,uint16_t))  /* EP: Endpoint 7 */
#define ENUM_USB_DMA_CTL_DMAEP8              (_ADI_MSK(0x00000080,uint16_t))  /* EP: Endpoint 8 */
#define ENUM_USB_DMA_CTL_DMAEP9              (_ADI_MSK(0x00000090,uint16_t))  /* EP: Endpoint 9 */

#define BITM_USB_DMA_CTL_IE                  (_ADI_MSK(0x00000008,uint16_t))  /* DMA Interrupt Enable */
#define ENUM_USB_DMA_CTL_DMAINTDIS           (_ADI_MSK(0x00000000,uint16_t))  /* IE: Disable Interrupt */
#define ENUM_USB_DMA_CTL_DMAINTEN            (_ADI_MSK(0x00000008,uint16_t))  /* IE: Enable Interrupt */

#define BITM_USB_DMA_CTL_MODE                (_ADI_MSK(0x00000004,uint16_t))  /* DMA Mode */
#define ENUM_USB_DMA_CTL_DMAMODE0            (_ADI_MSK(0x00000000,uint16_t))  /* MODE: DMA Mode 0 */
#define ENUM_USB_DMA_CTL_DMAMODE1            (_ADI_MSK(0x00000004,uint16_t))  /* MODE: DMA Mode 1 */

#define BITM_USB_DMA_CTL_DIR                 (_ADI_MSK(0x00000002,uint16_t))  /* DMA Transfer Direction */
#define ENUM_USB_DMA_CTL_DMADIR_RX           (_ADI_MSK(0x00000000,uint16_t))  /* DIR: DMA Write (for Rx Endpoint) */
#define ENUM_USB_DMA_CTL_DMADIR_TX           (_ADI_MSK(0x00000002,uint16_t))  /* DIR: DMA Read (for Tx Endpoint) */

#define BITM_USB_DMA_CTL_EN                  (_ADI_MSK(0x00000001,uint16_t))  /* DMA Enable */
#define ENUM_USB_DMA_CTL_DMADIS              (_ADI_MSK(0x00000000,uint16_t))  /* EN: Disable DMA */
#define ENUM_USB_DMA_CTL_DMAEN               (_ADI_MSK(0x00000001,uint16_t))  /* EN: Enable DMA (Start Transfer) */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_CT_UCH                           Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_CT_UCH_VALUE                 0                               /* Chirp Timeout Value */
#define BITM_USB_CT_UCH_VALUE                (_ADI_MSK(0x00007FFF,uint16_t))  /* Chirp Timeout Value */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_CT_HHSRTN                        Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_CT_HHSRTN_VALUE              0                               /* Host High Speed Return to Normal Value */
#define BITM_USB_CT_HHSRTN_VALUE             (_ADI_MSK(0x00007FFF,uint16_t))  /* Host High Speed Return to Normal Value */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_CT_HSBT                          Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_CT_HSBT_VALUE                0                               /* HS Timeout Adder */
#define BITM_USB_CT_HSBT_VALUE               (_ADI_MSK(0x0000000F,uint16_t))  /* HS Timeout Adder */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_LPM_ATTR                         Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_LPM_ATTR_EP                 12                               /* Endpoint */
#define BITP_USB_LPM_ATTR_RMTWAK              8                               /* Remote Wakeup Enable */
#define BITP_USB_LPM_ATTR_HIRD                4                               /* Host Initiated Resume Duration */
#define BITP_USB_LPM_ATTR_LINKSTATE           0                               /* Link State */
#define BITM_USB_LPM_ATTR_EP                 (_ADI_MSK(0x0000F000,uint16_t))  /* Endpoint */

#define BITM_USB_LPM_ATTR_RMTWAK             (_ADI_MSK(0x00000100,uint16_t))  /* Remote Wakeup Enable */
#define ENUM_USB_LPM_ATTR_RMTWAKDIS          (_ADI_MSK(0x00000000,uint16_t))  /* RMTWAK: Disable Remote Wakeup */
#define ENUM_USB_LPM_ATTR_RMTWAKEN           (_ADI_MSK(0x00000100,uint16_t))  /* RMTWAK: Enable Remote Wakeup */
#define BITM_USB_LPM_ATTR_HIRD               (_ADI_MSK(0x000000F0,uint16_t))  /* Host Initiated Resume Duration */

#define BITM_USB_LPM_ATTR_LINKSTATE          (_ADI_MSK(0x0000000F,uint16_t))  /* Link State */
#define ENUM_USB_LPM_ATTR_LNKSTATE_SSL1      (_ADI_MSK(0x00000001,uint16_t))  /* LINKSTATE: Sleep State (L1) */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_LPM_CTL                          Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_LPM_CTL_NAK                  4                               /* LPM NAK Enable */
#define BITP_USB_LPM_CTL_EN                   2                               /* LPM Enable */
#define BITP_USB_LPM_CTL_RESUME               1                               /* LPM Resume (Remote Wakeup) */
#define BITP_USB_LPM_CTL_TX                   0                               /* LPM Transmit */
#define BITM_USB_LPM_CTL_NAK                 (_ADI_MSK(0x00000010,uint8_t))   /* LPM NAK Enable */
#define BITM_USB_LPM_CTL_EN                  (_ADI_MSK(0x0000000C,uint8_t))   /* LPM Enable */
#define BITM_USB_LPM_CTL_RESUME              (_ADI_MSK(0x00000002,uint8_t))   /* LPM Resume (Remote Wakeup) */
#define BITM_USB_LPM_CTL_TX                  (_ADI_MSK(0x00000001,uint8_t))   /* LPM Transmit */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_LPM_IEN                          Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_LPM_IEN_LPMERR               5                               /* LPM Error Interrupt Enable */
#define BITP_USB_LPM_IEN_LPMRES               4                               /* LPM Resume Interrupt Enable */
#define BITP_USB_LPM_IEN_LPMNC                3                               /* LPM NYET Control Interrupt Enable */
#define BITP_USB_LPM_IEN_LPMACK               2                               /* LPM ACK Interrupt Enable */
#define BITP_USB_LPM_IEN_LPMNY                1                               /* LPM NYET Interrupt Enable */
#define BITP_USB_LPM_IEN_LPMST                0                               /* LPM STALL Interrupt Enable */
#define BITM_USB_LPM_IEN_LPMERR              (_ADI_MSK(0x00000020,uint8_t))   /* LPM Error Interrupt Enable */
#define BITM_USB_LPM_IEN_LPMRES              (_ADI_MSK(0x00000010,uint8_t))   /* LPM Resume Interrupt Enable */
#define BITM_USB_LPM_IEN_LPMNC               (_ADI_MSK(0x00000008,uint8_t))   /* LPM NYET Control Interrupt Enable */
#define BITM_USB_LPM_IEN_LPMACK              (_ADI_MSK(0x00000004,uint8_t))   /* LPM ACK Interrupt Enable */
#define BITM_USB_LPM_IEN_LPMNY               (_ADI_MSK(0x00000002,uint8_t))   /* LPM NYET Interrupt Enable */
#define BITM_USB_LPM_IEN_LPMST               (_ADI_MSK(0x00000001,uint8_t))   /* LPM STALL Interrupt Enable */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_LPM_IRQ                          Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_LPM_IRQ_LPMERR               5                               /* LPM Error Interrupt */
#define BITP_USB_LPM_IRQ_LPMRES               4                               /* LPM Resume Interrupt */
#define BITP_USB_LPM_IRQ_LPMNC                3                               /* LPM NYET Control Interrupt */
#define BITP_USB_LPM_IRQ_LPMACK               2                               /* LPM ACK Interrupt */
#define BITP_USB_LPM_IRQ_LPMNY                1                               /* LPM NYET Interrupt */
#define BITP_USB_LPM_IRQ_LPMST                0
#define BITM_USB_LPM_IRQ_LPMERR              (_ADI_MSK(0x00000020,uint8_t))   /* LPM Error Interrupt */
#define BITM_USB_LPM_IRQ_LPMRES              (_ADI_MSK(0x00000010,uint8_t))   /* LPM Resume Interrupt */
#define BITM_USB_LPM_IRQ_LPMNC               (_ADI_MSK(0x00000008,uint8_t))   /* LPM NYET Control Interrupt */
#define BITM_USB_LPM_IRQ_LPMACK              (_ADI_MSK(0x00000004,uint8_t))   /* LPM ACK Interrupt */
#define BITM_USB_LPM_IRQ_LPMNY               (_ADI_MSK(0x00000002,uint8_t))   /* LPM NYET Interrupt */
#define BITM_USB_LPM_IRQ_LPMST               (_ADI_MSK(0x00000001,uint8_t))

/* ------------------------------------------------------------------------------------------------------------------------
        USB_LPM_FADDR                        Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_LPM_FADDR_VALUE              0                               /* Function Address Value */
#define BITM_USB_LPM_FADDR_VALUE             (_ADI_MSK(0x0000007F,uint8_t))   /* Function Address Value */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_VBUS_CTL                         Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_VBUS_CTL_DRV                 4                               /* VBUS Drive */
#define BITP_USB_VBUS_CTL_DRVINT              3                               /* VBUS Drive Interrupt */
#define BITP_USB_VBUS_CTL_DRVIEN              2                               /* VBUS Drive Interrupt Enable */
#define BITP_USB_VBUS_CTL_DRVOD               1                               /* VBUS Drive Open Drain */
#define BITP_USB_VBUS_CTL_INVDRV              0                               /* VBUS Invert Drive */
#define BITM_USB_VBUS_CTL_DRV                (_ADI_MSK(0x00000010,uint8_t))   /* VBUS Drive */
#define BITM_USB_VBUS_CTL_DRVINT             (_ADI_MSK(0x00000008,uint8_t))   /* VBUS Drive Interrupt */
#define BITM_USB_VBUS_CTL_DRVIEN             (_ADI_MSK(0x00000004,uint8_t))   /* VBUS Drive Interrupt Enable */
#define BITM_USB_VBUS_CTL_DRVOD              (_ADI_MSK(0x00000002,uint8_t))   /* VBUS Drive Open Drain */
#define BITM_USB_VBUS_CTL_INVDRV             (_ADI_MSK(0x00000001,uint8_t))   /* VBUS Invert Drive */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_BAT_CHG                          Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_BAT_CHG_DEDCHG               4                               /* Dedicated Charging Port */
#define BITP_USB_BAT_CHG_CHGDET               3                               /* Charging Port Detected */
#define BITP_USB_BAT_CHG_SNSCHGDET            2                               /* Sense Charger Detection */
#define BITP_USB_BAT_CHG_CONDET               1                               /* Connected Detected */
#define BITP_USB_BAT_CHG_SNSCONDET            0                               /* Sense Connection Detection */
#define BITM_USB_BAT_CHG_DEDCHG              (_ADI_MSK(0x00000010,uint8_t))   /* Dedicated Charging Port */
#define BITM_USB_BAT_CHG_CHGDET              (_ADI_MSK(0x00000008,uint8_t))   /* Charging Port Detected */
#define BITM_USB_BAT_CHG_SNSCHGDET           (_ADI_MSK(0x00000004,uint8_t))   /* Sense Charger Detection */
#define BITM_USB_BAT_CHG_CONDET              (_ADI_MSK(0x00000002,uint8_t))   /* Connected Detected */
#define BITM_USB_BAT_CHG_SNSCONDET           (_ADI_MSK(0x00000001,uint8_t))   /* Sense Connection Detection */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_PHY_CTL                          Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_PHY_CTL_EN                   7                               /* PHY Enable */
#define BITP_USB_PHY_CTL_RESTORE              1                               /* Restore from Hibernate */
#define BITP_USB_PHY_CTL_HIBER                0                               /* Hibernate */
#define BITM_USB_PHY_CTL_EN                  (_ADI_MSK(0x00000080,uint8_t))   /* PHY Enable */
#define BITM_USB_PHY_CTL_RESTORE             (_ADI_MSK(0x00000002,uint8_t))   /* Restore from Hibernate */
#define BITM_USB_PHY_CTL_HIBER               (_ADI_MSK(0x00000001,uint8_t))   /* Hibernate */

/* ------------------------------------------------------------------------------------------------------------------------
        USB_PLL_OSC                          Pos/Masks                        Description
   ------------------------------------------------------------------------------------------------------------------------ */
#define BITP_USB_PLL_OSC_PLLMSEL              7                               /* PLL Multiplier Select */
#define BITP_USB_PLL_OSC_PLLM                 1                               /* PLL Multiplier Value */
#define BITP_USB_PLL_OSC_DIVCLKIN             0                               /* Divide CLKIN */
#define BITM_USB_PLL_OSC_PLLMSEL             (_ADI_MSK(0x00000080,uint16_t))  /* PLL Multiplier Select */
#define BITM_USB_PLL_OSC_PLLM                (_ADI_MSK(0x0000007E,uint16_t))  /* PLL Multiplier Value */
#define BITM_USB_PLL_OSC_DIVCLKIN            (_ADI_MSK(0x00000001,uint16_t))  /* Divide CLKIN */

/* Macros to match the Definitions for BF609 */
#define REG_USB0_FADDR                              USB_FADDR
#define REG_USB0_POWER                              USB_POWER
#define REG_USB0_DEV_CTL                            USB_OTG_DEV_CTL
#define REG_USB0_VBUS_CTL                           USB_OTG_VBUS_IRQ
#define REG_USB0_VBUS_MSK                           USB_OTG_VBUS_MASK
#define REG_USB0_PLL_OSC                            USB_PLLOSC_CTRL
#define REG_USB0_PHY_CTL                            USB_APHY_CNTRL2
#define REG_USB0_SRP_CLKDIV                         USB_SRP_CLKDIV

#define REG_USB0_INTRTX                             USB_INTRTX
#define REG_USB0_INTRRX                             USB_INTRRX
#define REG_USB0_INTRTXE                            USB_INTRTXE
#define REG_USB0_INTRRXE                            USB_INTRRXE
#define REG_USB0_IRQ                                USB_INTRUSB
#define REG_USB0_IEN                                USB_INTRUSBE
#define REG_USB0_FRAME                              USB_FRAME
#define REG_USB0_INDEX                              USB_INDEX
#define REG_USB0_GLOBAL_CTL                         USB_GLOBAL_CTL
#define REG_USB0_TXFIFOSZ                           _adi_usb_not_required_
#define REG_USB0_RXFIFOSZ                           _adi_usb_not_required_

#define REG_USB0_TESTMODE                           USB_TESTMODE

#define REG_USB0_EPI_TXTYPE0                        USB_TXTYPE
#define REG_USB0_EPI_TXMAXP0                        USB_TX_MAX_PACKET
#define REG_USB0_EPI_TXINTERVAL0                    USB_TXINTERVAL
#define REG_USB0_EPI_TXCSR_P0                       USB_TXCSR

#define REG_USB0_EPI_RXTYPE0                        USB_RXTYPE
#define REG_USB0_EPI_RXMAXP0                        USB_RX_MAX_PACKET
#define REG_USB0_EPI_RXINTERVAL0                    USB_RXINTERVAL
#define REG_USB0_EPI_RXCSR_P0                       USB_RXCSR
#define REG_USB0_EPI_RXCNT0                         USB_RXCOUNT

#define REG_USB0_EP0I_TYPE0                         __adi_usbh_not_required__
#define REG_USB0_EP0I_CSR0_P                        USB_CSR0
#define REG_USB0_EP0I_CNT0                          USB_COUNT0
#define REG_USB0_EP0I_NAKLIMIT0                     USB_NAKLIMIT0

#define REG_USB0_EP0I_CFGDATA0                      __adi_usbh_not_required__

#define REG_USB0_FIFO0                              USB_EP0_FIFO
#define REG_USB0_FIFO1                              USB_EP1_FIFO
#define REG_USB0_FIFO2                              USB_EP2_FIFO
#define REG_USB0_FIFO3                              USB_EP3_FIFO
#define REG_USB0_FIFO4                              USB_EP4_FIFO
#define REG_USB0_FIFO5                              USB_EP5_FIFO
#define REG_USB0_FIFO6                              USB_EP6_FIFO
#define REG_USB0_FIFO7                              USB_EP7_FIFO

#define REG_USB0_DMA_IRQ                            USB_DMA_INTERRUPT

#define REG_USB0_DMA0_CTL                           USB_DMA0CONTROL
#define REG_USB0_DMA1_CTL                           USB_DMA1CONTROL
#define REG_USB0_DMA2_CTL                           USB_DMA2CONTROL
#define REG_USB0_DMA3_CTL                           USB_DMA3CONTROL
#define REG_USB0_DMA4_CTL                           USB_DMA4CONTROL
#define REG_USB0_DMA5_CTL                           USB_DMA5CONTROL
#define REG_USB0_DMA6_CTL                           USB_DMA6CONTROL
#define REG_USB0_DMA7_CTL                           USB_DMA7CONTROL

#define REG_USB0_DMA0_ADDRLOW                       USB_DMA0ADDRLOW
#define REG_USB0_DMA1_ADDRLOW                       USB_DMA1ADDRLOW
#define REG_USB0_DMA2_ADDRLOW                       USB_DMA2ADDRLOW
#define REG_USB0_DMA3_ADDRLOW                       USB_DMA3ADDRLOW
#define REG_USB0_DMA4_ADDRLOW                       USB_DMA4ADDRLOW
#define REG_USB0_DMA5_ADDRLOW                       USB_DMA5ADDRLOW
#define REG_USB0_DMA6_ADDRLOW                       USB_DMA6ADDRLOW
#define REG_USB0_DMA7_ADDRLOW                       USB_DMA7ADDRLOW

#define REG_USB0_DMA0_ADDRHIGH                      USB_DMA0ADDRHIGH
#define REG_USB0_DMA1_ADDRHIGH                      USB_DMA1ADDRHIGH
#define REG_USB0_DMA2_ADDRHIGH                      USB_DMA2ADDRHIGH
#define REG_USB0_DMA3_ADDRHIGH                      USB_DMA3ADDRHIGH
#define REG_USB0_DMA4_ADDRHIGH                      USB_DMA4ADDRHIGH
#define REG_USB0_DMA5_ADDRHIGH                      USB_DMA5ADDRHIGH
#define REG_USB0_DMA6_ADDRHIGH                      USB_DMA6ADDRHIGH
#define REG_USB0_DMA7_ADDRHIGH                      USB_DMA7ADDRHIGH

#define REG_USB0_DMA0_CNTLOW                        USB_DMA0COUNTLOW
#define REG_USB0_DMA1_CNTLOW                        USB_DMA1COUNTLOW
#define REG_USB0_DMA2_CNTLOW                        USB_DMA2COUNTLOW
#define REG_USB0_DMA3_CNTLOW                        USB_DMA3COUNTLOW
#define REG_USB0_DMA4_CNTLOW                        USB_DMA4COUNTLOW
#define REG_USB0_DMA5_CNTLOW                        USB_DMA5COUNTLOW
#define REG_USB0_DMA6_CNTLOW                        USB_DMA6COUNTLOW
#define REG_USB0_DMA7_CNTLOW                        USB_DMA7COUNTLOW

#define REG_USB0_DMA0_CNTHIGH                       USB_DMA0COUNTHIGH
#define REG_USB0_DMA1_CNTHIGH                       USB_DMA1COUNTHIGH
#define REG_USB0_DMA2_CNTHIGH                       USB_DMA2COUNTHIGH
#define REG_USB0_DMA3_CNTHIGH                       USB_DMA3COUNTHIGH
#define REG_USB0_DMA4_CNTHIGH                       USB_DMA4COUNTHIGH
#define REG_USB0_DMA5_CNTHIGH                       USB_DMA5COUNTHIGH
#define REG_USB0_DMA6_CNTHIGH                       USB_DMA6COUNTHIGH
#define REG_USB0_DMA7_CNTHIGH                       USB_DMA7COUNTHIGH

#define USB_GLOBAL_ENABLE_B                          (1 << 0)
#define USB_GLOBAL_CTRL_TX_EP1_B                     (1 << 1)
#define USB_GLOBAL_CTRL_TX_EP2_B                     (1 << 2)
#define USB_GLOBAL_CTRL_TX_EP3_B                     (1 << 3)
#define USB_GLOBAL_CTRL_TX_EP4_B                     (1 << 4)
#define USB_GLOBAL_CTRL_TX_EP5_B                     (1 << 5)
#define USB_GLOBAL_CTRL_TX_EP6_B                     (1 << 6)
#define USB_GLOBAL_CTRL_TX_EP7_B                     (1 << 7)
#define USB_GLOBAL_CTRL_RX_EP1_B                     (1 << 8)
#define USB_GLOBAL_CTRL_RX_EP2_B                     (1 << 9)
#define USB_GLOBAL_CTRL_RX_EP3_B                     (1 << 10)
#define USB_GLOBAL_CTRL_RX_EP4_B                     (1 << 11)
#define USB_GLOBAL_CTRL_RX_EP5_B                     (1 << 12)
#define USB_GLOBAL_CTRL_RX_EP6_B                     (1 << 13)
#define USB_GLOBAL_CTRL_RX_EP7_B                     (1 << 14)
#define USB_GLOBAL_CTRL_TM_TIMER_REDUCTION_B         (1 << 15)

#define USB_GLOBAL_CTRL_ALL_TX_RX_B                  (USB_GLOBAL_CTRL_TX_EP1_B | \
                                                      USB_GLOBAL_CTRL_TX_EP2_B | \
                                                      USB_GLOBAL_CTRL_TX_EP3_B | \
                                                      USB_GLOBAL_CTRL_TX_EP4_B | \
                                                      USB_GLOBAL_CTRL_TX_EP5_B | \
                                                      USB_GLOBAL_CTRL_TX_EP6_B | \
                                                      USB_GLOBAL_CTRL_TX_EP7_B | \
                                                      USB_GLOBAL_CTRL_RX_EP1_B | \
                                                      USB_GLOBAL_CTRL_RX_EP2_B | \
                                                      USB_GLOBAL_CTRL_RX_EP3_B | \
                                                      USB_GLOBAL_CTRL_RX_EP4_B | \
                                                      USB_GLOBAL_CTRL_RX_EP5_B | \
                                                      USB_GLOBAL_CTRL_RX_EP6_B | \
                                                      USB_GLOBAL_CTRL_RX_EP7_B)


#elif defined (ADI_CFG_USB_BF_MUSBMHDRC)
#if !defined(__ADSPSC589_FAMILY__)          /* Not Griffin */
#include <services/int/adi_sic.h>
#else                                       /* Griffin */
#if defined (__ADSPSHARC__)                 /* Griffin Sharc Core */
#include <services/int/adi_sec.h>
#elif defined(__ADSPARM__)                  /* Griffin Arm Cortex A5 core */
#include <services/int/adi_gic.h>
#endif
#endif
#define ADI_USB_DYNFIFO_START_ADDR                  (64)
#define ADI_USB_DYNFIFO_TOTAL_SIZE                  (16*1024)
#elif defined(ADI_CFG_USB_ARM_MUSBMHDRC)
#include <adi_sid_cm350.h>
#endif

#ifdef _MISRA_RULES
#pragma diag(pop)
#endif

#endif /* _ADI_USB_MUSBMHDRC_PLATFORM_H_ */
