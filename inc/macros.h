/*********************************************************************************

Copyright (c) 2010-2014 Analog Devices, Inc. All Rights Reserved.

This software is proprietary to Analog Devices, Inc. and its licensors.  By using 
this software you agree to the terms of the associated Analog Devices Software 
License Agreement.

*********************************************************************************/

/*!
 *****************************************************************************
 * @file:    macros.h
 * @brief:   Register bitfield and mask defines for ADI ADuCxxx Device Series
 * @version: $Revision: 28525 $
 * @date:    $Date: 2014-11-12 14:51:26 -0500 (Wed, 12 Nov 2014) $
 *****************************************************************************/

#ifndef __MACROS_H__
#define __MACROS_H__

/* This file is included in device.h.  No need to include it anywhere else. */

/* start with CODA-Generated macros */
#include "defADuCM350.h"

/* supplemental defines from legacy macros.h file */

#define COMCON_DISABLE                 (0x1 << 0)
#define COMFBR_DIVM_MSK                (0x3 << 11)
#define COMFBR_DIVN_MSK                (0x7FF << 0)
#define COMFBR_ENABLE                  (0x1 << 15)
#define COMIEN_EDMAR                   (0x1 << 5)
#define COMIEN_EDMAT                   (0x1 << 4)
#define COMIEN_EDSSI                   (0x1 << 3)
#define COMIEN_ELSI                    (0x1 << 2)
#define COMIEN_ERBFI                   (0x1 << 0)
#define COMIEN_ETBEI                   (0x1 << 1)
#define COMIIR_NINT                    (0x1 << 0)
#define COMIIR_STA_MODEMSTATUS         (0 << 1) /* Modem status interrupt.  */
#define COMIIR_STA_MSK                 (0x3 << 1)
#define COMIIR_STA_RXBUFFULL           (2 << 1) /* Receive buffer full interrupt. Read RBR register to clear. */
#define COMIIR_STA_RXLINESTATUS        (3 << 1) /* Receive line status interrupt. Read LSR register to clear. */
#define COMIIR_STA_TXBUFEMPTY          (1 << 1) /* Transmit buffer empty interrupt. */
#define COMLCR_BRK                     (0x1 << 6)
#define COMLCR_EPS                     (0x1 << 4)
#define COMLCR_PEN                     (0x1 << 3)
#define COMLCR_SP                      (0x1 << 5)
#define COMLCR_STOP                    (0x1 << 2)
#define COMLCR_WLS_5BITS               (0 << 0)
#define COMLCR_WLS_6BITS               (1 << 0)
#define COMLCR_WLS_7BITS               (2 << 0)
#define COMLCR_WLS_8BITS               (3 << 0)
#define COMLCR_WLS_MSK                 (0x3 << 0)
#define COMLSR_BI                      (0x1 << 4)
#define COMLSR_DR                      (0x1 << 0)
#define COMLSR_FE                      (0x1 << 3)
#define COMLSR_OE                      (0x1 << 1)
#define COMLSR_PE                      (0x1 << 2)
#define COMLSR_TEMT                    (0x1 << 6)
#define COMLSR_THRE                    (0x1 << 5)
#define COMMCR_DTR                     (0x1 << 0)
#define COMMCR_LOOPBACK                (0x1 << 4)
#define COMMCR_OUT1                    (0x1 << 2)
#define COMMCR_OUT2                    (0x1 << 3)
#define COMMCR_RTS                     (0x1 << 1)
#define DMAALTCLR_RVAL                 0x0
#define DMAALTSET_RVAL                 0x0
#define DMACFG_RVAL                    0x0
#define DMAENCLR_RVAL                  0x0
#define DMAENSET_RVAL                  0x0
#define DMAERRCLR_ERROR                (0xFF << 0)
#define DMAPRISET_RVAL                 0x0
#define DMARMSKCLR_RVAL                0x0
#define DMARMSKSET_RVAL                0x0
#define DMASTA_ENABLE                  (0x1 << 0)
#define DMASTA_STATE_DONE              (9 << 4) /* Done                     */
#define DMASTA_STATE_IDLE              (0 << 4) /* Idle                     */
#define DMASTA_STATE_MSK               (0xF << 4)
#define DMASTA_STATE_RDCHNLDATA        (1 << 4) /* Reading channel controller data */
#define DMASTA_STATE_RDDSTENDPTR       (3 << 4) /* Reading destination data end pointer */
#define DMASTA_STATE_RDSRCDATA         (4 << 4) /* Reading source data      */
#define DMASTA_STATE_RDSRCENDPTR       (2 << 4) /* Reading source data end pointer */
#define DMASTA_STATE_SCATRGATHR        (10 << 4) /* Peripheral scatter-gather transition */
#define DMASTA_STATE_STALLED           (8 << 4) /* Stalled                  */
#define DMASTA_STATE_WAITDMAREQCLR     (6 << 4) /* Waiting for DMA request to clear */
#define DMASTA_STATE_WRCHNLDATA        (7 << 4) /* Writing channel controller data */
#define DMASTA_STATE_WRDSTDATA         (5 << 4) /* Writing destination data */
#define EI0CFG_IRQ0EN                  (0x1 << 3)
#define EI0CFG_IRQ0MDE_MSK             (0x7 << 0)
#define EI0CFG_IRQ1EN                  (0x1 << 7)
#define EI0CFG_IRQ1MDE_MSK             (0x7 << 4)
#define EI0CFG_IRQ2EN                  (0x1 << 11)
#define EI0CFG_IRQ2MDE_MSK             (0x7 << 8)
#define EI0CFG_IRQ3EN                  (0x1 << 15)
#define EI0CFG_IRQ3MDE_MSK             (0x7 << 12)
#define I2CDIV_RVAL                    0x1F1F
#define I2CFSTA_MFLUSH                 (0x1 << 9)
#define I2CFSTA_MRXFSTA_EMPTY          (0 << 6)
#define I2CFSTA_MRXFSTA_MSK            (0x3 << 6)
#define I2CFSTA_MRXFSTA_ONEBYTE        (1 << 6)
#define I2CFSTA_MRXFSTA_TWOBYTES       (2 << 6)
#define I2CFSTA_MTXFSTA_EMPTY          (0 << 4)
#define I2CFSTA_MTXFSTA_MSK            (0x3 << 4)
#define I2CFSTA_MTXFSTA_ONEBYTE        (1 << 4)
#define I2CFSTA_MTXFSTA_TWOBYTES       (2 << 4)
#define I2CFSTA_SFLUSH                 (0x1 << 8)
#define I2CFSTA_SRXFSTA_EMPTY          (0 << 2)
#define I2CFSTA_SRXFSTA_MSK            (0x3 << 2)
#define I2CFSTA_SRXFSTA_ONEBYTE        (1 << 2)
#define I2CFSTA_SRXFSTA_TWOBYTES       (2 << 2)
#define I2CFSTA_STXFSTA_EMPTY          (0 << 0)
#define I2CFSTA_STXFSTA_MSK            (0x3 << 0)
#define I2CFSTA_STXFSTA_ONEBYTE        (1 << 0)
#define I2CFSTA_STXFSTA_TWOBYTES       (2 << 0)
#define I2CMCON_COMPETE                (0x1 << 1)
#define I2CMCON_IENALOST               (0x1 << 6)
#define I2CMCON_IENCMP                 (0x1 << 8)
#define I2CMCON_IENNACK                (0x1 << 7)
#define I2CMCON_IENRX                  (0x1 << 4)
#define I2CMCON_IENTX                  (0x1 << 5)
#define I2CMCON_LOOPBACK               (0x1 << 2)
#define I2CMCON_MASEN                  (0x1 << 0)
#define I2CMCON_RXDMA                  (0x1 << 10)
#define I2CMCON_STRETCH                (0x1 << 3)
#define I2CMCON_TXDMA                  (0x1 << 11)
#define I2CMRXCNT_EXTEND               (0x1 << 8)
#define I2CMSTA_ALOST                  (0x1 << 5)
#define I2CMSTA_BUSY                   (0x1 << 6)
#define I2CMSTA_LINEBUSY               (0x1 << 10)
#define I2CMSTA_MSTOP                  (0x1 << 11)
#define I2CMSTA_NACKADDR               (0x1 << 4)
#define I2CMSTA_NACKDATA               (0x1 << 7)
#define I2CMSTA_RVAL                   0x0
#define I2CMSTA_RXOF                   (0x1 << 9)
#define I2CMSTA_RXREQ                  (0x1 << 3)
#define I2CMSTA_TCOMP                  (0x1 << 8)
#define I2CMSTA_TXFSTA_MSK             (0x3 << 0)
#define I2CMSTA_TXREQ                  (0x1 << 2)
#define I2CMSTA_TXUR                   (0x1 << 12)
#define I2CSCON_ADR10EN                (0x1 << 1)
#define I2CSCON_EARLYTXR               (0x1 << 5)
#define I2CSCON_GCEN                   (0x1 << 2)
#define I2CSCON_GCSBCLR                (0x1 << 4)
#define I2CSCON_HGCEN                  (0x1 << 3)
#define I2CSCON_IENREPST               (0x1 << 12)
#define I2CSCON_IENRX                  (0x1 << 9)
#define I2CSCON_IENSTOP                (0x1 << 8)
#define I2CSCON_IENTX                  (0x1 << 10)
#define I2CSCON_NACK                   (0x1 << 7)
#define I2CSCON_RXDMA                  (0x1 << 13)
#define I2CSCON_SLVEN                  (0x1 << 0)
#define I2CSCON_STRETCH                (0x1 << 6)
#define I2CSCON_TXDMA                  (0x1 << 14)
#define I2CSCON_XMITDEC                (0x1 << 11)
#define I2CSSTA_BUSY                   (0x1 << 6)
#define I2CSSTA_GCID_MSK               (0x3 << 8)
#define I2CSSTA_GCINT                  (0x1 << 7)
#define I2CSSTA_IDMAT_MSK              (0x3 << 11)
#define I2CSSTA_NOACK                  (0x1 << 5)
#define I2CSSTA_REPSTART               (0x1 << 13)
#define I2CSSTA_RVAL                   0x1
#define I2CSSTA_RXOF                   (0x1 << 4)
#define I2CSSTA_RXREQ                  (0x1 << 3)
#define I2CSSTA_START                  (0x1 << 14)
#define I2CSSTA_STOP                   (0x1 << 10)
#define I2CSSTA_TXFSEREQ               (0x1 << 0)
#define I2CSSTA_TXREQ                  (0x1 << 2)
#define I2CSSTA_TXUR                   (0x1 << 1)
#define SPICON_CON                     (0x1 << 11)
#define SPICON_CPHA                    (0x1 << 2)
#define SPICON_CPHA_SAMPLELEADING      (0 << 2)
#define SPICON_CPHA_SAMPLETRAILING     (1 << 2)
#define SPICON_CPOL                    (0x1 << 3)
#define SPICON_ENABLE                  (0x1 << 0)
#define SPICON_LOOPBACK                (0x1 << 10)
#define SPICON_LSB                     (0x1 << 5)
#define SPICON_MASEN                   (0x1 << 1)
#define SPICON_MOD_MSK                 (0x3 << 14)
#define SPICON_MOD_TX1RX1              (0 << 14)
#define SPICON_MOD_TX2RX2              (1 << 14)
#define SPICON_MOD_TX3RX3              (2 << 14)
#define SPICON_MOD_TX4RX4              (3 << 14)
#define SPICON_OEN                     (0x1 << 9)
#define SPICON_RFLUSH                  (0x1 << 12)
#define SPICON_RXOF                    (0x1 << 8)
#define SPICON_TFLUSH                  (0x1 << 13)
#define SPICON_TIM                     (0x1 << 6)
#define SPICON_TIM_RXRD                (0 << 6) /* Cleared by user to initiate transfer with a read of the SPIRX register */
#define SPICON_TIM_TXWR                (1 << 6) /* Set by user to initiate transfer with a write to the SPITX register. */
#define SPICON_WOM                     (0x1 << 4)
#define SPICON_ZEN                     (0x1 << 7)
#define SPIDIV_BCRST                   (0x1 << 7)
#define SPIDIV_DIV_MSK                 (0x3F << 0)
#define SPIDMA_ENABLE                  (0x1 << 0)
#define SPIDMA_IENRXDMA                (0x1 << 2)
#define SPIDMA_IENTXDMA                (0x1 << 1)
#define SPISTA_CSERR                   (0x1 << 12)
#define SPISTA_IRQ                     (0x1 << 0)
#define SPISTA_RX                      (0x1 << 6)
#define SPISTA_RXFSTA_EMPTY            (0 << 8)
#define SPISTA_RXFSTA_FOURBYTES        (4 << 8)
#define SPISTA_RXFSTA_MSK              (0x7 << 8)
#define SPISTA_RXFSTA_ONEBYTE          (1 << 8)
#define SPISTA_RXFSTA_THREEBYTES       (3 << 8)
#define SPISTA_RXFSTA_TWOBYTES         (2 << 8)
#define SPISTA_RXOF                    (0x1 << 7)
#define SPISTA_RXS                     (0x1 << 11)
#define SPISTA_TX                      (0x1 << 5)
#define SPISTA_TXFSTA_EMPTY            (0 << 1)
#define SPISTA_TXFSTA_FOURBYTES        (4 << 1)
#define SPISTA_TXFSTA_MSK              (0x7 << 1)
#define SPISTA_TXFSTA_ONEBYTE          (1 << 1)
#define SPISTA_TXFSTA_THREEBYTES       (3 << 1)
#define SPISTA_TXFSTA_TWOBYTES         (2 << 1)
#define SPISTA_TXUR                    (0x1 << 4)
#define T0CON_EVENT_AFE                (10 << 8) /* AFE event               */
#define T0CON_EVENT_CT                 (11 << 8) /* CapTouch event          */
#define T0CON_EVENT_EXT0               (1 << 8) /* External interrupt 0     */
#define T0CON_EVENT_EXT1               (2 << 8) /* External interrupt 1     */
#define T0CON_EVENT_EXT2               (3 << 8) /* External interrupt 2     */
#define T0CON_EVENT_EXT3               (4 << 8) /* External interrupt 3     */
#define T0CON_EVENT_EXT4               (5 << 8) /* External interrupt 4     */
#define T0CON_EVENT_EXT5               (6 << 8) /* External interrupt 5     */
#define T0CON_EVENT_EXT6               (7 << 8) /* External interrupt 6     */
#define T0CON_EVENT_EXT7               (8 << 8) /* External interrupt 7     */
#define T0CON_EVENT_EXT8               (9 << 8) /* External interrupt 8     */
#define T0CON_EVENT_FEE                (13 << 8) /* Flash Controller        */
#define T0CON_EVENT_GPT1               (14 << 8) /* GP Timer1               */
#define T0CON_EVENT_GPT2               (15 << 8) /* GP Timer2               */
#define T0CON_EVENT_WDT                (12 << 8) /* Watchdog timer          */
#define T0CON_EVENT_WUT                (0 << 8) /* Wakeup Timer             */
#define T1CON_EVENT_BEEP               (10 << 8)
#define T1CON_EVENT_COM                (0 << 8)
#define T1CON_EVENT_CRC                (9 << 8)
#define T1CON_EVENT_DMADONE            (12 << 8)
#define T1CON_EVENT_DMAERR             (11 << 8)
#define T1CON_EVENT_GPFEE              (13 << 8)
#define T1CON_EVENT_GPT0               (14 << 8)
#define T1CON_EVENT_GPT2               (15 << 8)
#define T1CON_EVENT_I2CM               (5 << 8)
#define T1CON_EVENT_I2CS               (4 << 8)
#define T1CON_EVENT_I2S                (6 << 8)
#define T1CON_EVENT_SPI0               (1 << 8)
#define T1CON_EVENT_SPI1               (2 << 8)
#define T1CON_EVENT_SPIH               (3 << 8)
#define T1CON_EVENT_USB                (7 << 8)
#define T1CON_EVENT_USBDMA             (8 << 8)
#define T2CLRI_ROLL_CLR                (1 << 4)
#define T2CLRI_RVAL                    0x0
#define T2CLRI_WUFA_CLR                (1 << 0)
#define T2CLRI_WUFB_CLR                (1 << 1)
#define T2CLRI_WUFC_CLR                (1 << 2)
#define T2CLRI_WUFD_CLR                (1 << 3)
#define T2CON_CLK_EXTCLK               (3 << 9)
#define T2CON_CLK_LFOSC                (2 << 9)
#define T2CON_CLK_LFXTAL               (1 << 9)
#define T2CON_CLK_MSK                  (0x3 << 9)
#define T2CON_CLK_PCLK                 (0 << 9)
#define T2CON_ENABLE                   (0x1 << 7)
#define T2CON_EVENT_AFE                (9 << 8) /* AFE event                */
#define T2CON_EVENT_CT                 (10 << 8) /* CapTouch event          */
#define T2CON_EVENT_EXT0               (0 << 8) /* External interrupt 0     */
#define T2CON_EVENT_EXT1               (1 << 8) /* External interrupt 1     */
#define T2CON_EVENT_EXT2               (2 << 8) /* External interrupt 2     */
#define T2CON_EVENT_EXT3               (3 << 8) /* External interrupt 3     */
#define T2CON_EVENT_EXT4               (4 << 8) /* External interrupt 4     */
#define T2CON_EVENT_EXT5               (5 << 8) /* External interrupt 5     */
#define T2CON_EVENT_EXT6               (6 << 8) /* External interrupt 6     */
#define T2CON_EVENT_EXT7               (7 << 8) /* External interrupt 7     */
#define T2CON_EVENT_EXT8               (8 << 8) /* External interrupt 8     */
#define T2CON_EVENT_GPT0               (14 << 8) /* GP Timer0               */
#define T2CON_EVENT_GPT1               (15 << 8) /* GP Timer1               */
#define T2CON_EVENT_RTC                (11 << 8) /* RTC event               */
#define T2CON_FREEZE                   (0x1 << 3)
#define T2CON_MOD                      (0x1 << 6)
#define T2CON_MOD_FREERUN              (1 << 6)
#define T2CON_MOD_PERIODIC             (0 << 6)
#define T2CON_PRE_DIV1                 (0 << 0)
#define T2CON_PRE_DIV16                (1 << 0)
#define T2CON_PRE_DIV256               (2 << 0)
#define T2CON_PRE_DIV32768             (3 << 0)
#define T2CON_PRE_MSK                  (0x3 << 0)
#define T2CON_RVAL                     0x40
#define T2CON_STOPINC                  (0x1 << 11)
#define T2CON_WUEN                     (0x1 << 8)
#define T2IEN_ROLL                     (0x1 << 4)
#define T2IEN_RVAL                     0x0
#define T2IEN_WUFA                     (0x1 << 0)
#define T2IEN_WUFB                     (0x1 << 1)
#define T2IEN_WUFC                     (0x1 << 2)
#define T2IEN_WUFD                     (0x1 << 3)
#define T2INC_RVAL                     0xC8
#define T2STA_FREEZE                   (0x1 << 7)
#define T2STA_IRQCRY                   (0x1 << 6)
#define T2STA_PDOK                     (0x1 << 8)
#define T2STA_ROLL                     (0x1 << 4)
#define T2STA_WUFA                     (0x1 << 0)
#define T2STA_WUFB                     (0x1 << 1)
#define T2STA_WUFC                     (0x1 << 2)
#define T2STA_WUFD                     (0x1 << 3)
#define T2WUFB0_RVAL                   0x1FFF
#define T2WUFB1_RVAL                   0x0
#define T2WUFC0_RVAL                   0x2FFF
#define T2WUFC1_RVAL                   0x0
#define T2WUFD0_RVAL                   0x3FFF
#define T2WUFD1_RVAL                   0x0
#define T3CLRI_VALUE_CLR               (0xCCCC << 0)
#define T3CON_ENABLE                   (0x1 << 5)
#define T3CON_IRQ                      (0x1 << 1)
#define T3CON_MOD                      (0x1 << 6)
#define T3CON_MOD_FREERUN              (0 << 6)
#define T3CON_MOD_PERIODIC             (1 << 6)
#define T3CON_PD                       (0x1 << 0)
#define T3CON_PRE_DIV1                 (0 << 2)
#define T3CON_PRE_DIV16                (1 << 2)
#define T3CON_PRE_DIV256               (2 << 2)
#define T3CON_PRE_DIV4096              (3 << 2)
#define T3CON_PRE_MSK                  (0x3 << 2)
#define T3STA_CLRI                     (0x1 << 1)
#define T3STA_CON                      (0x1 << 3)
#define T3STA_IRQ                      (0x1 << 0)
#define T3STA_LD                       (0x1 << 2)
#define T3STA_LOCK                     (0x1 << 4)
#define TCLRI_CAP_CLR                  (1 << 1)
#define TCLRI_TMOUT_CLR                (1 << 0)
#define TCON_CLK_UCLK                  (0 << 5)      /* System Clock               */
#define TCON_CLK_PCLK                  TCON_CLK_UCLK /* PCLK = UCLK for ADuCM350     */
#define TCON_CLK_HFOSC                 (1 << 5)      /* Internal 16 Mhz Oscillator */
#define TCON_CLK_LFOSC                 (2 << 5)      /* Internal 32 kHz Oscillator */
#define TCON_CLK_LFXTAL                (3 << 5)      /* External 32 kHz crystal    */
#define TCON_CLK_MSK                   (0x3 << 5)
#define TCON_ENABLE                    (0x1 << 4)
#define TCON_EVENT_MSK                 (0xF << 8)
#define TCON_EVENTEN                   (0x1 << 12)
#define TCON_MOD                       (0x1 << 3)
#define TCON_PRE_DIV1                  (0 << 0)
#define TCON_PRE_DIV16                 (1 << 0)
#define TCON_PRE_DIV256                (2 << 0)
#define TCON_PRE_DIV32768              (3 << 0) /* If the selected clock source is UCLK then this setting results in a prescaler of 4. */
#define TCON_PRE_MSK                   (0x3 << 0)
#define TCON_RLD                       (0x1 << 7)
#define TCON_RVAL                      (0x000A)
#define TCON_UP                        (0x1 << 2)
#define TSTA_BUSY                      (0x1 << 6)
#define TSTA_CAP                       (0x1 << 1)
#define TSTA_PDOK                      (0x1 << 7)
#define TSTA_TMOUT                     (0x1 << 0)

#endif /* __MACROS_H__ */
