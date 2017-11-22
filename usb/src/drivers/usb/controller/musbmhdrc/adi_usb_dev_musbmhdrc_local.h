/*********************************************************************************

  Copyright(c) 2013 Analog Devices, Inc. All Rights Reserved.

  This software is proprietary and confidential.  By using this software you agree
  to the terms of the associated Analog Devices License Agreement.

*********************************************************************************/

/*!
* @file      adi_usb_dev_musbmhdrc_local.h
*
* @brief     driver internal macros and structures are placed here.
*
*/
#ifndef _ADI_USB_MUSBMHDRC_LOCAL_H_
#define _ADI_USB_MUSBMHDRC_LOCAL_H_

#ifdef _MISRA_RULES
#pragma diag(push)
#pragma diag(suppress:misra_rules_all:"Suppress all misra rules")
#endif

#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <device.h>

#if defined(ADI_ADUCM350)
#include <adi_int.h>
#include <gpio.h>
#include <cdefADuCM350.h>

#else
#include <services/int/adi_int.h>
#include <services/pwr/adi_pwr.h>
#include <services/gpio/adi_gpio.h>
#include <sys/platform.h>
#endif

#include "adi_usb_dev_musbmhdrc_platform.h"

#if (ADI_CFG_USB_OTG_HOST == 1)
#include <drivers/usb/controller/host/adi_usbh_dev.h>
#elif (ADI_CFG_USB_DEVICE == 1)
#include <drivers/usb/controller/device/adi_usbd_dev.h>
#else
#error "USB Host/Device mode configuration not done"
#endif

#if defined(ADI_ADUCM350)
#define ENUM_USB_DEV_CTL_HOSTREQ                                     0  /* Host mode not available in aduCM350 */
#define ENUM_USB_IRQ_CON                                             0  /* Not valid for 350 */
#define ENUM_USB_IRQ_VBUSERR                                         0  /* Not valid for 350 */
#endif

#define APP_USBD_CFG_ALIGNMENT                                       32u

#if defined(__ADSPGCC__)
#define APP_USBD_CFG_ALIGN_BUFFER                                    __attribute__((aligned (APP_USBD_CFG_ALIGNMENT)))
#define ADI_SUP_UNUSED_WARN                                          __attribute__((unused))  /* Variable attributes is used to suppress the warning
                                                                                               * -Wunused-but-set-variable from ARM compiler */
#define ADI_USB_INLINE                                               __inline
																							   
#else
#define __ADI_APPUSB_MKSTR2(V)                                       # V
#define __ADI_APPUSB_MKSTR1(V)                                       __ADI_APPUSB_MKSTR2(V)

#if defined ( __ICCARM__ )
#define _USBAPP_ALIGN_ARG_                                           __ADI_APPUSB_MKSTR1(pack(APP_USBD_CFG_ALIGNMENT))
#define _USBAPP_SUPPRESS_ARG_                                        __ADI_APPUSB_MKSTR1(diag_suppress=Pe550)
#define _USBAPP_SUPPRESS_ARG1_                                        __ADI_APPUSB_MKSTR1(diag_suppress=Pe177)
#define ADI_SUP_UNUSED_WARN                                          _Pragma(_USBAPP_SUPPRESS_ARG_)
#define ADI_SUP_UNREF_WARN                                           _Pragma(_USBAPP_SUPPRESS_ARG1_)
#else
#define _USBAPP_ALIGN_ARG_                                           __ADI_APPUSB_MKSTR1(align(APP_USBD_CFG_ALIGNMENT))
#define ADI_SUP_UNUSED_WARN
#endif

#define APP_USBD_CFG_ALIGN_BUFFER                                    _Pragma(_USBAPP_ALIGN_ARG_)
#define _USBAPP_INLINE_ARG_                                          __ADI_APPUSB_MKSTR1(inline)
#define ADI_USB_INLINE                                               _Pragma(_USBAPP_INLINE_ARG_)

#endif

#if defined(ADI_CFG_USB_BF_MUSBMHDRC)
#define ADI_USBH_MULTI_POINT_EN                     (1)
#define ADI_CFG_USB_DYNFIFO_EN                      (1)
#endif

#define ADI_USB_MAX_NUM_TOTAL_EP                    ADI_CFG_USB_MAX_NUM_URB
#define ADI_USB_SETUP_PKT_LEN                       (8u)

#define ADI_USB_IF_BIT_SET(Value, Bit)              (Value) & (Bit)
#define ADI_USB_IS_BULK(x)                          ((x == ADI_USB_EP_TYPE_BULK))
#define ADI_USB_IS_INTERRUPT(x)                     ((x == ADI_USB_EP_TYPE_INTERRUPT))
#define ADI_USB_IS_ISOC(x)                          ((x == ADI_USB_EP_TYPE_ISOC))
#define ADI_USB_IS_HOST(x)                          ((x == ADI_USB_MODE_OTG_HOST))

#define ADI_USB_GET_EP_NUM(EpAddr)                  ((EpAddr)& 0x0F)

#if (ADI_CFG_USB_OTG_HOST == 1)
#define ADI_USB_GET_EP_DIR(EpAddr)                  (((EpAddr)& 0x80) ? (ADI_USB_EP_DIR_RX):(ADI_USB_EP_DIR_TX))
#else
#define ADI_USB_GET_EP_DIR(EpAddr)                  (((EpAddr)& 0x80) ? (ADI_USB_EP_DIR_TX):(ADI_USB_EP_DIR_RX))
#endif

#define ADI_USB_GET_EP_TYPE(EpAttr)                 (ADI_USB_EP_TYPE)((EpAttr)& 0x03)
#define ADI_USB_GET_EPTYPE_FRMURB(pUsbUrbData)      (pUsbUrbData->EpType  << 24)
#define ADI_USB_GET_DEVADDR_FRMURB(pUsbUrbData)     (pUsbUrbData->DevAddr << 16)
#define ADI_USB_GET_DEVADDR_FRMEPD(x)               ((x >> 16) & 0xFF)
#define ADI_USB_GET_EPADDR_FRMEPD(x)                ((x) & 0xFF)
#define ADI_USB_GET_EPTYPE_FRMEPD(x)                ((x >> 24) & 0xFF)

#if (ADI_CFG_USB_OTG_HOST == 1)
/* These are hub related macros and copied from usbh_core.h file */
/*          HUB PORT STATUS BITS                 */
#define  ADI_USBH_HUB_STATUS_PORT_CONN                    0x0001u
#define  ADI_USBH_HUB_STATUS_PORT_EN                      0x0002u
#define  ADI_USBH_HUB_STATUS_PORT_SUSPEND                 0x0004u
#define  ADI_USBH_HUB_STATUS_PORT_OVER_CUR                0x0008u
#define  ADI_USBH_HUB_STATUS_PORT_RESET                   0x0010u
#define  ADI_USBH_HUB_STATUS_PORT_PWR                     0x0100u
#define  ADI_USBH_HUB_STATUS_PORT_LOW_SPD                 0x0200u
#define  ADI_USBH_HUB_STATUS_PORT_HIGH_SPD                0x0400u
#define  ADI_USBH_HUB_STATUS_PORT_TEST                    0x0800u
#define  ADI_USBH_HUB_STATUS_PORT_INDICATOR               0x1000u

/*          HUB PORT STATUS CHANGE BITS         */
#define  ADI_USBH_HUB_STATUS_C_PORT_CONN                  0x0001u
#define  ADI_USBH_HUB_STATUS_C_PORT_EN                    0x0002u
#define  ADI_USBH_HUB_STATUS_C_PORT_SUSPEND               0x0004u
#define  ADI_USBH_HUB_STATUS_C_PORT_OVER_CUR              0x0008u
#define  ADI_USBH_HUB_STATUS_C_PORT_RESET                 0x0010u

#define ADI_USB_GET_HUBPORT(x)                      ((x >> 16) & 0xFF)
#define ADI_USB_GET_HUBADDR(x)                      ((x) & 0xFF)

#endif /* ADI_CFG_USB_BF_MUSBMHDRC */

#define _adi_usb_not_required_                     0

#define ADI_USB_ADDR_LOW(x)    (((uint32_t)x & 0xffff))
#define ADI_USB_ADDR_HIGH(x)   ((((uint32_t)x >> 16) & 0xffff))


/*
 * Device States 
 */
typedef enum
{
    ADI_USB_DEV_STATE_RESET=0,                        /* Device is in RESET state */
    ADI_USB_DEV_STATE_OPENED,                         /* Device is Opened successfully */
    ADI_USB_DEV_STATE_CON,                            /* Device is Connected */
    ADI_USB_DEV_STATE_ADDRESSED,                      /* Device Address Set */
    ADI_USB_DEV_STATE_CONFIGURED,                     /* Device configured enumeration finished, active transfers*/
    ADI_USB_DEV_STATE_DISCON,                         /* Device is disconnected */
    ADI_USB_DEV_STATE_STOPPED,                        /* Device is Closed */

}ADI_USB_DEVICE_STATE;

typedef uint16_t ADI_USB_FIFO_REG_TYPE;

typedef void (*ADI_USB_HANDLER) (ADI_USB_HANDLE hDevice);

/*
 * Register structure
 */
#if defined(ADI_CFG_USB_BF_MUSBMHDRC) || defined(ADI_CFG_USB_ARM_MUSBMHDRC)

typedef  struct {
    volatile uint16_t                   DMA_CTL;
    uint8_t                             Padding1[2];
    volatile uint32_t                   DMA_ADDRHIGH;
    volatile uint32_t                   DMA_CNTHIGH;
    uint8_t                             Padding2[4];
}ADI_USB_DMA_REGS_DEF;

typedef  struct {
    volatile uint8_t                    MP_TXFUNCADDR;
    uint8_t                             Padding3;
    volatile uint8_t                    MP_TXHUBADDR;
    volatile uint8_t                    MP_TXHUBPORT;
    volatile uint8_t                    MP_RXFUNCADDR;
    uint8_t                             Padding4;
    volatile uint8_t                    MP_RXHUBADDR;
    volatile uint8_t                    MP_RXHUBPORT;
}ADI_USB_MP_REGS_DEF;

typedef struct {

    /* Main USB Registers: 0xFFCC1000 - 0xFFCC100F */
    volatile ADI_USB_8BIT_REG_TYPE      FADDR;
    volatile ADI_USB_8BIT_REG_TYPE      POWER;
    volatile uint16_t                   INTRTX;
    volatile uint16_t                   INTRRX;
    volatile uint16_t                   INTRTXE;
    volatile uint16_t                   INTRRXE;
    volatile ADI_USB_8BIT_REG_TYPE      IRQ;
    volatile ADI_USB_8BIT_REG_TYPE      IEN;
    volatile uint16_t                   FRAME;
    volatile ADI_USB_8BIT_REG_TYPE      INDEX;
    volatile ADI_USB_8BIT_REG_TYPE      TESTMODE;

    /* Indexed CSR registers: 0xFFCC1010- 0xFFCC101F */
    volatile uint16_t                   EPI_TXMAXP;
    volatile uint16_t                   EPI_TXCSR;      /* EP0_CSR0   */
    volatile uint16_t                   EPI_RXMAXP;
    volatile uint16_t                   EPI_RXCSR;
    volatile uint16_t                   EPI_RXCNT;      /* EP0_CNT0   */
    volatile ADI_USB_8BIT_REG_TYPE      EPI_TXTYPE;     /* EP0I_TYPE0 */
    volatile ADI_USB_8BIT_REG_TYPE      EPI_TXINTERVAL; /* EP0I_NAKLIMIT0 */
    volatile ADI_USB_8BIT_REG_TYPE      EPI_RXTYPE;
    volatile ADI_USB_8BIT_REG_TYPE      EPI_RXINTERVAL;
    uint8_t                             Padding1;
    volatile ADI_USB_8BIT_REG_TYPE      FIFOSIZE;

    /* FIFOs: 0xFFCC1020 - 0xFFCC105F*/
    volatile uint32_t                   EPI_FIFO0;
    volatile uint32_t                   EPI_FIFO1;
    volatile uint32_t                   EPI_FIFO2;
    volatile uint32_t                   EPI_FIFO3;
    volatile uint32_t                   EPI_FIFO4;
    volatile uint32_t                   EPI_FIFO5;
    volatile uint32_t                   EPI_FIFO6;
    volatile uint32_t                   EPI_FIFO7;
    volatile uint32_t                   EPI_FIFO8;
    volatile uint32_t                   EPI_FIFO9;
    volatile uint32_t                   EPI_FIFO10;
    volatile uint32_t                   EPI_FIFO11;
    volatile uint32_t                   EPI_FIFO12;
    volatile uint32_t                   EPI_FIFO13;
    volatile uint32_t                   EPI_FIFO14;
    volatile uint32_t                   EPI_FIFO15;

    /* Additional Control & Configuration Registers: 0xFFCC1060 – 0xFFCC107F*/
    volatile ADI_USB_8BIT_REG_TYPE      DEV_CTL;
    volatile ADI_USB_8BIT_REG_TYPE      MISC;
    volatile ADI_USB_8BIT_REG_TYPE      TXFIFOSZ;
    volatile ADI_USB_8BIT_REG_TYPE      RXFIFOSZ;
    volatile uint16_t                   TXFIFOADDR;
    volatile uint16_t                   RXFIFOADDR;
    volatile uint8_t                    Padding2[16];
    volatile ADI_USB_8BIT_REG_TYPE      EPINFO;
    volatile ADI_USB_8BIT_REG_TYPE      RAMINFO;
    volatile ADI_USB_8BIT_REG_TYPE      LINKINFO;
    volatile ADI_USB_8BIT_REG_TYPE      VPLEN;
    volatile ADI_USB_8BIT_REG_TYPE      HS_EOF1;
    volatile ADI_USB_8BIT_REG_TYPE      FS_EOF1;
    volatile ADI_USB_8BIT_REG_TYPE      LS_EOF1;
    volatile ADI_USB_8BIT_REG_TYPE      SOFT_RST;

    /* Multipoint Control/Status Registers: 0xFFCC1080- 0xFFCC10FF*/
    volatile ADI_USB_MP_REGS_DEF        MPRegs[12];
    uint8_t                             Padding27[32];

    /* Indexed CSR Registers: 0xFFCC1100 - 0xFFCC11FF */
    volatile uint16_t                   EP0_TXMAXP;
    volatile uint16_t                   EP0_CSR0;
    volatile uint16_t                   EP0_RXMAXP;
    volatile uint16_t                   EP0_RXCSR;
    volatile uint16_t                   EP0_CNT0;
    volatile ADI_USB_8BIT_REG_TYPE      EP0I_TYPE0;
    volatile ADI_USB_8BIT_REG_TYPE      EP0I_NAKLIMIT0;
    volatile ADI_USB_8BIT_REG_TYPE      EP0_RXTYPE;
    volatile ADI_USB_8BIT_REG_TYPE      EP0_RXINTERVAL;
    uint8_t                             Padding28;
    volatile ADI_USB_8BIT_REG_TYPE      EP0_CFGDATA0;

    volatile uint16_t                   EP1_TXMAXP;
    volatile uint16_t                   EP1_TXCSR;
    volatile uint16_t                   EP1_RXMAXP;
    volatile uint16_t                   EP1_RXCSR;
    volatile uint16_t                   EP1_RXCNT;
    volatile ADI_USB_8BIT_REG_TYPE      EP1_TXTYPE;
    volatile ADI_USB_8BIT_REG_TYPE      EP1_TXINTERVAL;
    volatile ADI_USB_8BIT_REG_TYPE      EP1_RXTYPE;
    volatile ADI_USB_8BIT_REG_TYPE      EP1_RXINTERVAL;
    uint8_t                             Padding29[2];

    volatile uint16_t                   EP2_TXMAXP;
    volatile uint16_t                   EP2_TXCSR;
    volatile uint16_t                   EP2_RXMAXP;
    volatile uint16_t                   EP2_RXCSR;
    volatile uint16_t                   EP2_RXCNT;
    volatile ADI_USB_8BIT_REG_TYPE      EP2_TXTYPE;
    volatile ADI_USB_8BIT_REG_TYPE      EP2_TXINTERVAL;
    volatile ADI_USB_8BIT_REG_TYPE      EP2_RXTYPE;
    volatile ADI_USB_8BIT_REG_TYPE      EP2_RXINTERVAL;
    uint8_t                             Padding30[2];

    volatile uint16_t                   EP3_TXMAXP;
    volatile uint16_t                   EP3_TXCSR;
    volatile uint16_t                   EP3_RXMAXP;
    volatile uint16_t                   EP3_RXCSR;
    volatile uint16_t                   EP3_RXCNT;
    volatile ADI_USB_8BIT_REG_TYPE      EP3_TXTYPE;
    volatile ADI_USB_8BIT_REG_TYPE      EP3_TXINTERVAL;
    volatile ADI_USB_8BIT_REG_TYPE      EP3_RXTYPE;
    volatile ADI_USB_8BIT_REG_TYPE      EP3_RXINTERVAL;
    uint8_t                             Padding31[2];

    volatile uint16_t                   EP4_TXMAXP;
    volatile uint16_t                   EP4_TXCSR;
    volatile uint16_t                   EP4_RXMAXP;
    volatile uint16_t                   EP4_RXCSR;
    volatile uint16_t                   EP4_RXCNT;
    volatile ADI_USB_8BIT_REG_TYPE      EP4_TXTYPE;
    volatile ADI_USB_8BIT_REG_TYPE      EP4_TXINTERVAL;
    volatile ADI_USB_8BIT_REG_TYPE      EP4_RXTYPE;
    volatile ADI_USB_8BIT_REG_TYPE      EP4_RXINTERVAL;
    uint8_t                             Padding32[2];

    volatile uint16_t                   EP5_TXMAXP;
    volatile uint16_t                   EP5_TXCSR;
    volatile uint16_t                   EP5_RXMAXP;
    volatile uint16_t                   EP5_RXCSR;
    volatile uint16_t                   EP5_RXCNT;
    volatile ADI_USB_8BIT_REG_TYPE      EP5_TXTYPE;
    volatile ADI_USB_8BIT_REG_TYPE      EP5_TXINTERVAL;
    volatile ADI_USB_8BIT_REG_TYPE      EP5_RXTYPE;
    volatile ADI_USB_8BIT_REG_TYPE      EP5_RXINTERVAL;
    uint8_t                             Padding33[2];

    volatile uint16_t                   EP6_TXMAXP;
    volatile uint16_t                   EP6_TXCSR;
    volatile uint16_t                   EP6_RXMAXP;
    volatile uint16_t                   EP6_RXCSR;
    volatile uint16_t                   EP6_RXCNT;
    volatile ADI_USB_8BIT_REG_TYPE      EP6_TXTYPE;
    volatile ADI_USB_8BIT_REG_TYPE      EP6_TXINTERVAL;
    volatile ADI_USB_8BIT_REG_TYPE      EP6_RXTYPE;
    volatile ADI_USB_8BIT_REG_TYPE      EP6_RXINTERVAL;
    uint8_t                             Padding34[2];

    volatile uint16_t                   EP7_TXMAXP;
    volatile uint16_t                   EP7_TXCSR;
    volatile uint16_t                   EP7_RXMAXP;
    volatile uint16_t                   EP7_RXCSR;
    volatile uint16_t                   EP7_RXCNT;
    volatile ADI_USB_8BIT_REG_TYPE      EP7_TXTYPE;
    volatile ADI_USB_8BIT_REG_TYPE      EP7_TXINTERVAL;
    volatile ADI_USB_8BIT_REG_TYPE      EP7_RXTYPE;
    volatile ADI_USB_8BIT_REG_TYPE      EP7_RXINTERVAL;
    uint8_t                             Padding35[2];

    volatile uint16_t                   EP8_TXMAXP;
    volatile uint16_t                   EP8_TXCSR;
    volatile uint16_t                   EP8_RXMAXP;
    volatile uint16_t                   EP8_RXCSR;
    volatile uint16_t                   EP8_RXCNT;
    volatile ADI_USB_8BIT_REG_TYPE      EP8_TXTYPE;
    volatile ADI_USB_8BIT_REG_TYPE      EP8_TXINTERVAL;
    volatile ADI_USB_8BIT_REG_TYPE      EP8_RXTYPE;
    volatile ADI_USB_8BIT_REG_TYPE      EP8_RXINTERVAL;
    uint8_t                             Padding36[2];

    volatile uint16_t                   EP9_TXMAXP;
    volatile uint16_t                   EP9_TXCSR;
    volatile uint16_t                   EP9_RXMAXP;
    volatile uint16_t                   EP9_RXCSR;
    volatile uint16_t                   EP9_RXCNT;
    volatile ADI_USB_8BIT_REG_TYPE      EP9_TXTYPE;
    volatile ADI_USB_8BIT_REG_TYPE      EP9_TXINTERVAL;
    volatile ADI_USB_8BIT_REG_TYPE      EP9_RXTYPE;
    volatile ADI_USB_8BIT_REG_TYPE      EP9_RXINTERVAL;
    uint8_t                             Padding37[2];

    volatile uint16_t                   EP10_TXMAXP;
    volatile uint16_t                   EP10_TXCSR;
    volatile uint16_t                   EP10_RXMAXP;
    volatile uint16_t                   EP10_RXCSR;
    volatile uint16_t                   EP10_RXCNT;
    volatile ADI_USB_8BIT_REG_TYPE      EP10_TXTYPE;
    volatile ADI_USB_8BIT_REG_TYPE      EP10_TXINTERVAL;
    volatile ADI_USB_8BIT_REG_TYPE      EP10_RXTYPE;
    volatile ADI_USB_8BIT_REG_TYPE      EP10_RXINTERVAL;
    uint8_t                             Padding38[2];

    volatile uint16_t                   EP11_TXMAXP;
    volatile uint16_t                   EP11_TXCSR;
    volatile uint16_t                   EP11_RXMAXP;
    volatile uint16_t                   EP11_RXCSR;
    volatile uint16_t                   EP11_RXCNT;
    volatile ADI_USB_8BIT_REG_TYPE      EP11_TXTYPE;
    volatile ADI_USB_8BIT_REG_TYPE      EP11_TXINTERVAL;
    volatile ADI_USB_8BIT_REG_TYPE      EP11_RXTYPE;
    volatile ADI_USB_8BIT_REG_TYPE      EP11_RXINTERVAL;
    uint8_t                             Padding39[2];

    uint8_t                             Padding40[64];

    /* DMA Registers: 0xFFCC1200 - 0xFFCC127F */
    volatile ADI_USB_8BIT_REG_TYPE      DMA_IRQ;
    uint8_t                             Padding41[3];
    volatile ADI_USB_DMA_REGS_DEF       DmaRegs[ADI_CFG_USB_MAX_DMA_CHANNELS];

    /* Padding: 0xFFCC1280 - 0xFFCC12FF*/
    uint8_t                             Padding57[124];

    /* USB Extended Registers: 0xFFCC1300 – 0xFFCC1349 */
    volatile uint16_t                   RQPKTCNT0;
    uint8_t                             Padding58[2];
    volatile uint16_t                   RQPKTCNT1;
    uint8_t                             Padding59[2];
    volatile uint16_t                   RQPKTCNT2;
    uint8_t                             Padding60[2];
    volatile uint16_t                   RQPKTCNT3;
    uint8_t                             Padding61[2];
    volatile uint16_t                   RQPKTCNT4;
    uint8_t                             Padding62[2];
    volatile uint16_t                   RQPKTCNT5;
    uint8_t                             Padding63[2];
    volatile uint16_t                   RQPKTCNT6;
    uint8_t                             Padding64[2];
    volatile uint16_t                   RQPKTCNT7;
    uint8_t                             Padding65[2];
    volatile uint16_t                   RQPKTCNT8;
    uint8_t                             Padding66[2];
    volatile uint16_t                   RQPKTCNT9;
    uint8_t                             Padding67[2];
    volatile uint16_t                   RQPKTCNT10;
    uint8_t                             Padding68[2];
    volatile uint16_t                   RQPKTCNT11;
    uint8_t                             Padding69[2];
    volatile uint16_t                   RQPKTCNT12;
    uint8_t                             Padding70[2];
    volatile uint16_t                   RQPKTCNT13;
    uint8_t                             Padding71[2];
    volatile uint16_t                   RQPKTCNT14;
    uint8_t                             Padding72[2];
    volatile uint16_t                   RQPKTCNT15;
    uint8_t                             Padding73[2];

    volatile uint16_t                   RxDPktBufDis;
    volatile uint16_t                   TxDPktBufDis;
    volatile uint16_t                   CT_UCH;        /* USB0 Chir Timeout Register */
    volatile uint16_t                   CT_HHSRTN;     /* USB0 Host High Seed Return to Normal Register */
    volatile uint16_t                   CT_HSBT;       /* USB0 High Seed Timeout Register */

    /* Padding: 0xFFCC134A – 0xFFCC137F*/
    uint8_t                             Padding74[54];

    /* 0xFFCC1380, 0xFFCC1381 */
    volatile uint8_t                    VBUS_CTL;
    volatile uint8_t                    BAT_CHG;

    /* Padding: 0xFFCC1382 – 0xFFCC1393 */
    uint8_t                             Padding75[18];

    /* 0xFFCC1394-0xFFCC1397 */
    volatile ADI_USB_8BIT_REG_TYPE      PHY_CTL;
    uint8_t                             Padding76[3];

    /* 0xFFCC1398 */
    volatile uint16_t                   PLL_OSC;

} ADI_USB_REGS_DEF;
#else
typedef  struct {
    volatile uint16_t                   DMA_CTL;
    uint8_t                             Padding1[2];
    volatile uint16_t                   DMA_ADDRLOW;
    uint8_t                             Padding2[2];
    volatile uint16_t                   DMA_ADDRHIGH;
    uint8_t                             Padding3[2];
    volatile uint16_t                   DMA_CNTLOW;
    uint8_t                             Padding4[2];
    volatile uint16_t                   DMA_CNTHIGH;
    uint8_t                             Padding5[14];
}ADI_USB_DMA_REGS_DEF;

typedef struct {

    /* Main USB Registers: 0xFFC03800 - 0xFFC0383F */
    volatile ADI_USB_8BIT_REG_TYPE      FADDR;
    uint8_t                             Padding1[2];
    volatile ADI_USB_8BIT_REG_TYPE      POWER;
    uint8_t                             Padding2[2];
    volatile uint16_t                   INTRTX;
    uint8_t                             Padding3[2];
    volatile uint16_t                   INTRRX;
    uint8_t                             Padding4[2];
    volatile uint16_t                   INTRTXE;
    uint8_t                             Padding5[2];
    volatile uint16_t                   INTRRXE;
    uint8_t                             Padding6[2];
    volatile ADI_USB_8BIT_REG_TYPE      IRQ;
    uint8_t                             Padding7[2];
    volatile ADI_USB_8BIT_REG_TYPE      IEN;
    uint8_t                             Padding8[2];
    volatile uint16_t                   FRAME;
    uint8_t                             Padding9[2];
    volatile ADI_USB_8BIT_REG_TYPE      INDEX;
    uint8_t                             Padding10[2];
    volatile ADI_USB_8BIT_REG_TYPE      TESTMODE;
    uint8_t                             Padding11[2];
    volatile uint16_t                   GLOBINTR;
    uint8_t                             Padding12[2];
    volatile uint16_t                   GLOBAL_CTL;
    uint8_t                             Padding13[14];


    /* Indexed CSR registers: 0xFFC03840- 0xFFC0387F */
    volatile uint16_t                   EPI_TXMAXP;
    uint8_t                             Padding14[2];
    volatile uint16_t                   EPI_TXCSR;      /* EP0_CSR0   */
    uint8_t                             Padding15[2];
    volatile uint16_t                   EPI_RXMAXP;
    uint8_t                             Padding16[2];
    volatile uint16_t                   EPI_RXCSR;
    uint8_t                             Padding17[2];
    volatile uint16_t                   EPI_RXCNT;      /* EP0_CNT0   */
    uint8_t                             Padding18[2];
    volatile ADI_USB_8BIT_REG_TYPE      EPI_TXTYPE;     /* EP0I_TYPE0 */
    uint8_t                             Padding19[2];
    volatile ADI_USB_8BIT_REG_TYPE      EPI_TXINTERVAL; /* EP0I_NAKLIMIT0 */
    uint8_t                             Padding20[2];
    volatile ADI_USB_8BIT_REG_TYPE      EPI_RXTYPE;
    uint8_t                             Padding21[2];
    volatile ADI_USB_8BIT_REG_TYPE      EPI_RXINTERVAL;
    uint8_t                             Padding22[6];
    volatile uint16_t                   TXCOUNT;
    uint8_t                             Padding23[22];

    /* FIFOs: 0xFFC03880 - 0xFFC038FF */
    volatile uint16_t                   EPI_FIFO0;
    uint8_t                             Padding24[6];
    volatile uint16_t                   EPI_FIFO1;
    uint8_t                             Padding25[6];
    volatile uint16_t                   EPI_FIFO2;
    uint8_t                             Padding26[6];
    volatile uint16_t                   EPI_FIFO3;
    uint8_t                             Padding27[6];
    volatile uint16_t                   EPI_FIFO4;
    uint8_t                             Padding28[6];
    volatile uint16_t                   EPI_FIFO5;
    uint8_t                             Padding29[6];
    volatile uint16_t                   EPI_FIFO6;
    uint8_t                             Padding30[6];
    volatile uint16_t                   EPI_FIFO7;
    uint8_t                             Padding31[70];

    /* Additional Control & Configuration Registers: 0xFFC03900 – 0xFFC039FF*/
    volatile ADI_USB_8BIT_REG_TYPE      DEV_CTL;
    uint8_t                             Padding32[2];
    volatile ADI_USB_8BIT_REG_TYPE      VBUS_CTL;
    uint8_t                             Padding33[2];
    volatile ADI_USB_8BIT_REG_TYPE      VBUS_MASK;
    uint8_t                             Padding34[62];
    volatile ADI_USB_8BIT_REG_TYPE      LINKINFO;
    uint8_t                             Padding35[2];
    volatile ADI_USB_8BIT_REG_TYPE      VPLEN;
    uint8_t                             Padding36[2];
    volatile ADI_USB_8BIT_REG_TYPE      HS_EOF1;
    uint8_t                             Padding37[2];
    volatile ADI_USB_8BIT_REG_TYPE      FS_EOF1;
    uint8_t                             Padding38[2];
    volatile ADI_USB_8BIT_REG_TYPE      LS_EOF1;
    uint8_t                             Padding39[134]; /* 0xFFC0395A - 0xFFC039DF */

    /* 0xFFC039E0 - 0xFFC039FF */
    volatile ADI_USB_8BIT_REG_TYPE      PHY_CTL;
    uint8_t                             Padding40[2];
    volatile ADI_USB_8BIT_REG_TYPE      PHY_CALIB;
    uint8_t                             Padding41[2];
    volatile ADI_USB_8BIT_REG_TYPE      PHY_CNTRL2;
    uint8_t                             Padding42[2];
    volatile ADI_USB_8BIT_REG_TYPE      PHY_TEST;
    uint8_t                             Padding43[2];
    volatile ADI_USB_8BIT_REG_TYPE      PLL_OSC;
    uint8_t                             Padding44[2];
    volatile ADI_USB_8BIT_REG_TYPE      CLK_DIV;
    uint8_t                             Padding45[10];


    /* Indexed CSR Registers: 0xFFC03A00 - 0xFFC03A3F */
    volatile uint16_t                   EP0_TXMAXP;
    uint8_t                             Padding46[2];
    volatile uint16_t                   EP0_CSR0;
    uint8_t                             Padding47[2];
    volatile uint16_t                   EP0_RXMAXP;
    uint8_t                             Padding48[2];
    volatile uint16_t                   EP0_RXCSR;
    uint8_t                             Padding49[2];
    volatile uint16_t                   EP0_CNT0;
    uint8_t                             Padding50[2];
    volatile ADI_USB_8BIT_REG_TYPE      EP0I_TYPE0;
    uint8_t                             Padding51[2];
    volatile ADI_USB_8BIT_REG_TYPE      EP0I_NAKLIMIT0;
    uint8_t                             Padding52[2];
    volatile ADI_USB_8BIT_REG_TYPE      EP0_RXTYPE;
    uint8_t                             Padding53[2];
    volatile ADI_USB_8BIT_REG_TYPE      EP0_RXINTERVAL;
    uint8_t                             Padding54[6];
    volatile uint16_t                   EP0_TXCOUNT;
    uint8_t                             Padding55[22]; /* 0xFFC03A2A - 0xFFC03A3F */

    /* 0xFFC03A40 - 0xFFC03A7F */
    volatile uint16_t                   EP1_TXMAXP;
    uint8_t                             Padding56[2];
    volatile uint16_t                   EP1_TXCSR;
    uint8_t                             Padding57[2];
    volatile uint16_t                   EP1_RXMAXP;
    uint8_t                             Padding58[2];
    volatile uint16_t                   EP1_RXCSR;
    uint8_t                             Padding59[2];
    volatile uint16_t                   pEP1_RXCNT;
    uint8_t                             Padding60[2];
    volatile ADI_USB_8BIT_REG_TYPE      EP1_TXTYPE;
    uint8_t                             Padding61[2];
    volatile ADI_USB_8BIT_REG_TYPE      EP1_TXINTERVAL;
    uint8_t                             Padding62[2];
    volatile ADI_USB_8BIT_REG_TYPE      EP1_RXTYPE;
    uint8_t                             Padding63[2];
    volatile ADI_USB_8BIT_REG_TYPE      EP1_RXINTERVAL;
    uint8_t                             Padding64[6];
    volatile ADI_USB_8BIT_REG_TYPE      EP1_TXCOUNT;
    uint8_t                             Padding65[22]; /* 0xFFC03A6A - 0xFFC03A7F */

    /* 0xFFC03A80 - 0xFFC03ABF */
    volatile uint16_t                   EP2_TXMAXP;
    uint8_t                             Padding66[2];
    volatile uint16_t                   EP2_TXCSR;
    uint8_t                             Padding67[2];
    volatile uint16_t                   EP2_RXMAXP;
    uint8_t                             Padding68[2];
    volatile uint16_t                   EP2_RXCSR;
    uint8_t                             Padding69[2];
    volatile uint16_t                   EP2_RXCNT;
    uint8_t                             Padding70[2];
    volatile ADI_USB_8BIT_REG_TYPE      EP2_TXTYPE;
    uint8_t                             Padding71[2];
    volatile ADI_USB_8BIT_REG_TYPE      EP2_TXINTERVAL;
    uint8_t                             Padding72[2];
    volatile ADI_USB_8BIT_REG_TYPE      EP2_RXTYPE;
    uint8_t                             Padding73[2];
    volatile ADI_USB_8BIT_REG_TYPE      EP2_RXINTERVAL;
    uint8_t                             Padding74[6];
    volatile ADI_USB_8BIT_REG_TYPE      EP2_TXCOUNT;
    uint8_t                             Padding75[22]; /* 0xFFC03AAA - 0xFFC03ABF */

    /* 0xFFC03AC0 - 0xFFC03AFF */
    volatile uint16_t                   EP3_TXMAXP;
    uint8_t                             Padding76[2];
    volatile uint16_t                   EP3_TXCSR;
    uint8_t                             Padding77[2];
    volatile uint16_t                   EP3_RXMAXP;
    uint8_t                             Padding78[2];
    volatile uint16_t                   EP3_RXCSR;
    uint8_t                             Padding79[2];
    volatile uint16_t                   EP3_RXCNT;
    uint8_t                             Padding80[2];
    volatile ADI_USB_8BIT_REG_TYPE      EP3_TXTYPE;
    uint8_t                             Padding81[2];
    volatile ADI_USB_8BIT_REG_TYPE      EP3_TXINTERVAL;
    uint8_t                             Padding82[2];
    volatile ADI_USB_8BIT_REG_TYPE      EP3_RXTYPE;
    uint8_t                             Padding83[2];
    volatile ADI_USB_8BIT_REG_TYPE      EP3_RXINTERVAL;
    uint8_t                             Padding84[6];
    volatile ADI_USB_8BIT_REG_TYPE      EP3_TXCOUNT;
    uint8_t                             Padding85[22]; /* 0xFFC03AEA - 0xFFC03AFF */

    /* 0xFFC03B00 - 0xFFC03B3F */
    volatile uint16_t                   EP4_TXMAXP;
    uint8_t                             Padding86[2];
    volatile uint16_t                   EP4_TXCSR;
    uint8_t                             Padding87[2];
    volatile uint16_t                   EP4_RXMAXP;
    uint8_t                             Padding88[2];
    volatile uint16_t                   EP4_RXCSR;
    uint8_t                             Padding89[2];
    volatile uint16_t                   EP4_RXCNT;
    uint8_t                             Padding90[2];
    volatile ADI_USB_8BIT_REG_TYPE      EP4_TXTYPE;
    uint8_t                             Padding91[2];
    volatile ADI_USB_8BIT_REG_TYPE      EP4_TXINTERVAL;
    uint8_t                             Padding92[2];
    volatile ADI_USB_8BIT_REG_TYPE      EP4_RXTYPE;
    uint8_t                             Padding93[2];
    volatile ADI_USB_8BIT_REG_TYPE      EP4_RXINTERVAL;
    uint8_t                             Padding94[6];
    volatile ADI_USB_8BIT_REG_TYPE      EP4_TXCOUNT;
    uint8_t                             Padding95[22]; /* 0xFFC03B2A - 0xFFC03B3F */

    /* 0xFFC03B40 - 0xFFC03B7F */
    volatile uint16_t                   EP5_TXMAXP;
    uint8_t                             Padding96[2];
    volatile uint16_t                   EP5_TXCSR;
    uint8_t                             Padding97[2];
    volatile uint16_t                   EP5_RXMAXP;
    uint8_t                             Padding98[2];
    volatile uint16_t                   EP5_RXCSR;
    uint8_t                             Padding99[2];
    volatile uint16_t                   EP5_RXCNT;
    uint8_t                             Padding100[2];
    volatile ADI_USB_8BIT_REG_TYPE      EP5_TXTYPE;
    uint8_t                             Padding101[2];
    volatile ADI_USB_8BIT_REG_TYPE      EP5_TXINTERVAL;
    uint8_t                             Padding102[2];
    volatile ADI_USB_8BIT_REG_TYPE      EP5_RXTYPE;
    uint8_t                             Padding103[2];
    volatile ADI_USB_8BIT_REG_TYPE      EP5_RXINTERVAL;
    uint8_t                             Padding104[6];
    volatile ADI_USB_8BIT_REG_TYPE      EP5_TXCOUNT;
    uint8_t                             Padding105[22]; /* 0xFFC03B6A - 0xFFC03B7F */

    /* 0xFFC03B80 - 0xFFC03BBF */
    volatile uint16_t                   EP6_TXMAXP;
    uint8_t                             Padding106[2];
    volatile uint16_t                   EP6_TXCSR;
    uint8_t                             Padding107[2];
    volatile uint16_t                   EP6_RXMAXP;
    uint8_t                             Padding108[2];
    volatile uint16_t                   EP6_RXCSR;
    uint8_t                             Padding109[2];
    volatile uint16_t                   EP6_RXCNT;
    uint8_t                             Padding110[2];
    volatile ADI_USB_8BIT_REG_TYPE      EP6_TXTYPE;
    uint8_t                             Padding111[2];
    volatile ADI_USB_8BIT_REG_TYPE      EP6_TXINTERVAL;
    uint8_t                             Padding112[2];
    volatile ADI_USB_8BIT_REG_TYPE      EP6_RXTYPE;
    uint8_t                             Padding113[2];
    volatile ADI_USB_8BIT_REG_TYPE      EP6_RXINTERVAL;
    uint8_t                             Padding114[6];
    volatile ADI_USB_8BIT_REG_TYPE      EP6_TXCOUNT;
    uint8_t                             Padding115[22]; /* 0xFFC03BAA - 0xFFC03BBF */

    /* 0xFFC03BC0 - 0xFFC03BFF */
    volatile uint16_t                   EP7_TXMAXP;
    uint8_t                             Padding116[2];
    volatile uint16_t                   EP7_TXCSR;
    uint8_t                             Padding117[2];
    volatile uint16_t                   EP7_RXMAXP;
    uint8_t                             Padding118[2];
    volatile uint16_t                   EP7_RXCSR;
    uint8_t                             Padding119[2];
    volatile uint16_t                   EP7_RXCNT;
    uint8_t                             Padding120[2];
    volatile ADI_USB_8BIT_REG_TYPE      EP7_TXTYPE;
    uint8_t                             Padding121[2];
    volatile ADI_USB_8BIT_REG_TYPE      EP7_TXINTERVAL;
    uint8_t                             Padding122[2];
    volatile ADI_USB_8BIT_REG_TYPE      EP7_RXTYPE;
    uint8_t                             Padding123[2];
    volatile ADI_USB_8BIT_REG_TYPE      EP7_RXINTERVAL;
    uint8_t                             Padding124[6];
    volatile ADI_USB_8BIT_REG_TYPE      EP7_TXCOUNT;
    uint8_t                             Padding125[22]; /* 0xFFC03BEA - 0xFFC03BFF */


    /* DMA Registers: 0xFFC03C00 - 0xFFCC127F */
    volatile ADI_USB_8BIT_REG_TYPE      DMA_IRQ;
    uint8_t                             Padding126[2];
    volatile ADI_USB_DMA_REGS_DEF       DmaRegs[ADI_CFG_USB_MAX_DMA_CHANNELS];

} ADI_USB_REGS_DEF;
#endif


typedef  struct {
    uint8_t   bmRequestType;
    uint8_t   bRequest;
    uint16_t  wValue;
    uint16_t  wIndex;
    uint16_t  wLength;
} USB_SETUP_REQ;

/*
 * Endpoint information
 */
typedef struct
{
    uint8_t         DevAddr;              /*!< Device address                         */
    uint8_t         EpPhyNum;             /*!< Endpoint Physical Number               */
    uint8_t         EpPhyIdx;             /*!< Endpoint Physical index                */
    ADI_USB_EP_DIR  EpDir;                /*!< Direction of the transfer              */
    uint8_t         EpLogicalAddr;        /*!< Logical Endpoint Address               */
    ADI_USB_EP_TYPE EpType;               /*!< Control, Bulk, Interrupt, Isoc         */
    uint16_t        EpMaxSize;            /*!< EP maximum transfer size               */
    uint8_t         EpInterval;           /*!< EP interval                            */
    uint8_t         EpDmaChannel;         /*!< DMA channel dedicated to this EP       */
    bool            EpDmaChannelLck;      /*!< DMA channel is locked                  */
    bool            EpTxDmaPend;          /*!< DMA transfer to FIFO is in process for this EP                              */
    ADI_USB_EP_MODE EpTransferMode;       /*!< Transfer mode (DMA mode 0, DMA mode 1 or PIO mode) used by this EP          */
    uint8_t         *pEpBufferdata;       /*!< Current Pointer to the buffer          */
    uint32_t        EpProcBytesThisPass;  /*!< Bytes transferred in current pass      */
    ADI_USB_URB     *pEpCurURB;           /*!< Current URB processed on this Endpoint */
    ADI_URB_STATE   EpUrbState;           /*!< State of the current URB */
    uint8_t         EpIsHalt;             /*!< True of EP is halted */

}ADI_USB_EP_INFO;



typedef struct {
    uint8_t         EpPhyNum;
    uint32_t        EpType;
    ADI_USB_EP_DIR  EpDir;
    uint16_t        EpMaxSize;
    bool            AllocFlag;
}ADI_USB_EP_CONFIG;

#if defined(ADI_DEBUG)


#define ADI_USB_REPORT_ERROR(InputParam,ComparedValue,ResultCode) do {         \
                    if (InputParam == ComparedValue) {     \
                           return ResultCode;              \
                    }                                      \
                                      } while(0) 

#define ADI_USB_VALIDATE_HANDLE(hDevice) do {                                                    \
                    if (ValidateHandle(hDevice) != ADI_USB_RESULT_SUCCESS) { \
                                                return ADI_USB_RESULT_INVALID_HANDLE;            \
                        }\
                                        } while(0)

static ADI_USB_RESULT ValidateHandle(ADI_USB_HANDLE *hDevice);
#else
#define ADI_USB_REPORT_ERROR(InputParam,ComparedValue,ErrCode) 
#define ADI_USB_VALIDATE_HANDLE(hDevice) 
#endif

#if (ADI_CFG_USB_STATS == 1)
typedef struct
{
    uint32_t nEpStall;
    uint32_t nEpNack;
    uint32_t nEpError;
    uint32_t nEpAbort;
    uint32_t nURBAbort;
    uint32_t nTxInts;
    uint32_t nRxInts;
    uint32_t nSetupInts;
    uint32_t nPreRxDataAvlbl;
    uint32_t nIncorrectDir;
    uint32_t nFalseTxInts;
    uint32_t nErrorTxInts;
    uint32_t nFalseRxInts;
    uint32_t nEp0DeviceZLP;
    uint32_t nFalseTxDmaMode1Ints;
    uint32_t nEPResetDataToggle;
#if defined(ADI_ADUCM350)    
    uint32_t nEPDmaBusy;
#endif    
} ADI_USB_EP_STATS;

typedef struct
{
    uint32_t  nVbusErr;
    uint32_t  nSessReq;
    uint32_t  nDisCon;
    uint32_t  nConnect;
    uint32_t  nRstBabble;
    uint32_t  nResume;
    uint32_t  nSuspend;

    ADI_USB_EP_STATS  EpStats[ADI_CFG_USB_MAX_NUM_URB];
} ADI_USB_STATS;

#define ADI_USB_EP_STATINC(pdev,ep,err)  do { (pdev->UsbStats.EpStats[ep].err++); }while(0)
#define ADI_USB_STATINC(pdev,err)  do { (pdev->UsbStats.err++); }while(0)
#else
#define ADI_USB_EP_STATINC(pdev,ep,err)  
#define ADI_USB_STATINC(pdev,err)  
#endif

#if defined(ADI_DEBUG)
#define ADI_USB_DBG_NUM_SETUP_EVENTS (512)
#define ADI_USB_DBG_MAX_SETUPPKTS    (ADI_USB_DBG_NUM_SETUP_EVENTS)
typedef struct {
      char SetupPkt[8];
} ADI_USB_DBG_SETUP;


typedef struct
{
    uint32_t       dbg_setup_index;
    uint32_t       dbg_ep0_setidle_rxcnt0;
    uint32_t       dbg_ep0_setdata_urbnull;
    uint32_t       dbg_ep0_setdata_rxcount;
    uint32_t       dbg_ep0_setreq;
    uint32_t       dbg_ep0_setstatus;
    uint32_t       dbg_ep0_setupend;
    uint32_t       dbg_ep0_high_intack;
    uint32_t       dbg_ep0_stallsent;
    ADI_USB_DBG_SETUP  dbg_setup_pkts[ADI_USB_DBG_MAX_SETUPPKTS]; 

} ADI_USB_DBG_INFO;

#define ADI_USB_DBG_CNTRINC(pdev,dcntr)  do { (pdev->UsbDebugInfo.dcntr++); }while(0)
#else
#define ADI_USB_DBG_CNTRINC(pdev,dcntr)  
#endif

/* true - aligned and false - not aligned
 * alignment is checked only when cache is ON
 */

ADI_USB_INLINE
static bool adi_usbdrv_IsDataAligned(const uint32_t uData, const bool cacheOn, ADI_USB_EP_DIR  EpDir)
{
    if(cacheOn && (EpDir == ADI_USB_EP_DIR_RX))
    {
        return !((bool)(uData & 0x1F));
    }
    else
    {
#if defined(__ADSPBF609_FAMILY__) || defined(__ADSPBF707_FAMILY__) || defined(__ADSPSC589_FAMILY__) || defined(ADI_ADUCM350) /* TODO: confirm if 4byte minimum alignment is required for SC589 */
        return !((bool)(uData & 0x03));
#else
        return true;
#endif
    }
}

#if (ADI_CFG_USB_DEVICE == 1)
typedef enum
{
    ADI_USB_SETUP_IDLE,
    ADI_USB_SETUP_REQ,
    ADI_USB_SETUP_DATA,
    ADI_USB_SETUP_STATUS

}ADI_USB_SETUP_STATE;

typedef enum
{
    ADI_USB_SS_IDLE,
    ADI_USB_SS_PENDING_TX_NDP_ZLP,
    ADI_USB_SS_PENDING_RX_DP_ZLP,
    ADI_USB_SS_PENDING_DATA,
    ADI_USB_SS_SETUPEND,
    ADI_USB_SS_STALL,

}ADI_USB_SETUP_STAT;
#endif

typedef struct
{
    uint8_t                    DevNum;
    volatile ADI_USB_REGS_DEF  *pRegs;                     /*< Pointer to Register mapping structure                   */
    ADI_USB_DEV_MODE           DevMode;
    ADI_USB_DEVICE_STATE       DevState;
    uint8_t                    DevAddr;
    ADI_USB_SPEED              DevSpd;
#if (ADI_CFG_USB_OTG_HOST == 1)
    ADI_USB_HUB_PORTSTATUS     RHPortStatus[ADI_MAX_NUM_RH_PORT];
#endif
    ADI_USB_EP_INFO            EpInfo[ADI_USB_MAX_NUM_TOTAL_EP]; /* Total number of endpoints/URBs possible */
    uint8_t                    EpConfigEntryMax;
#if !defined(ADI_ADUCM350)    
    ADI_GPIO_PORT              VBusDrivePort;
#endif    
    uint16_t                   VBusDrivePin;
    ADI_CALLBACK               pfEpZeroCallback;
    ADI_CALLBACK               pfEpDataCallback;
    ADI_CALLBACK               pfBusEventCallback;

    ADI_USB_HANDLER            pfSuspendHandler;
    ADI_USB_HANDLER            pfResumeHandler;
    ADI_USB_HANDLER            pfConnectHandler;
    ADI_USB_HANDLER            pfResetHandler;
    ADI_USB_HANDLER            pfEp0Handler;

    uint32_t                   DmaChannelBitMap;
    uint32_t                   CacheOn;
#if (ADI_CFG_USB_DEVICE == 1)
APP_USBD_CFG_ALIGN_BUFFER
    uint8_t                    EpZeroSetupPkt[32];
    ADI_USB_URB                EpZeroCurURB;
    ADI_USB_SETUP_STATE        EpZeroSetupState;
    ADI_USB_SETUP_STAT         EpZeroSetupStatus;
    bool                       EpZeroIntAcked;
#endif
#if defined(ADI_CFG_USB_BF_MUSBMHDRC)
    uint32_t                   FifoRamAdress;
    uint32_t                   FifoRamTotalSize;
#endif
#if (ADI_CFG_USB_STATS == 1)
    ADI_USB_STATS              UsbStats;
#endif
#if defined(ADI_DEBUG)
    ADI_USB_DBG_INFO           UsbDebugInfo;
#endif

} ADI_MUSBMHDRC_DATA;


/*
 * ******************************************* Function Declarations ****************************************
 */
static ADI_USB_RESULT   adi_usbdrv_ScheduleDma                      (ADI_MUSBMHDRC_DATA     *pUsbDrvData,
                                                                     ADI_USB_EP_INFO        *pEpInfo);

#if (ADI_CFG_USB_ENABLE_DMA_MODE1 == 1)
ADI_USB_RESULT          adi_usbdrv_ScheduleDmaMode1                 (ADI_MUSBMHDRC_DATA     *pUsbDrvData,
                                                                     ADI_USB_EP_INFO        *pEpInfo);
#endif

static ADI_USB_EP_INFO* adi_usbdrv_GetEpInfo                       (ADI_MUSBMHDRC_DATA     *pUsbDrvData,
                                                                     uint32_t               EpData);


static ADI_USB_EP_INFO* adi_usbdrv_GetEpInfoFromEpPhy              (ADI_MUSBMHDRC_DATA   *pUsbDrvData,
                                                                     uint8_t                EpPhyNum,
                                                                     uint8_t                EpDir);

static uint32_t         adi_usbdrv_BytesThisPass                    (ADI_USB_EP_INFO        *pEpInfo);

static void             adi_usbdrv_UsbOtgHandler                    (uint32_t               iid,
                                                                     void*                  handlerArg);

#if defined(ADI_CFG_USB_BF_MUSBHDRC)
static void             adi_usbdrv_USBInterruptHandler              (uint32_t               iid,
                                                                     void*                  handlerArg);
#endif

static void             adi_usbdrv_DmaHandler                       (uint32_t               iid,
                                                                     void*                  handlerArg);

static void             adi_usbdrv_SetSessionBit                    (ADI_MUSBMHDRC_DATA     *pUsbDrvData);

static void             adi_usbdrv_SetRegDefault                    (ADI_MUSBMHDRC_DATA     *pUsbDrvData);

static void             adi_usbdrv_ResetCurrentURB                  (ADI_USB_EP_INFO        *pEpInfo);

/* Platform specific function declarations */

#if (ADI_CFG_USB_OTG_HOST == 1)
static void             adi_usbdrv_DriveVbusOn                      (ADI_MUSBMHDRC_DATA     *pUsbDrvData);

static void             adi_usbdrv_DriveVbusOff                     (ADI_MUSBMHDRC_DATA     *pUsbDrvData);

#if defined(ADI_CFG_USB_BF_MUSBHDRC)
static ADI_USB_RESULT   adi_usbdrv_ConfigGPIO                       (ADI_MUSBMHDRC_DATA     *pUsbDrvData);
#endif
#endif

static ADI_USB_RESULT   adi_usbdrv_PlatformSpecInit                 (ADI_MUSBMHDRC_DATA     *pUsbDrvData);

static void             adi_usbdrv_WaitMilliSec                     (uint32_t               Millisec);

static void             adi_usbdrv_WaitOneMillisec                  (void);

/*
 *  Controller Driver API functions 
 */
static  ADI_USB_RESULT     adi_musbmhdrc_Init                       (ADI_USB_HANDLE *hDevice, ADI_USB_INIT_DATA *pInitData);

static  ADI_USB_RESULT     adi_musbmhdrc_Start                      (ADI_USB_HANDLE const hDevice);

static  ADI_USB_RESULT     adi_musbmhdrc_Stop                       (ADI_USB_HANDLE const hDevice);

static  ADI_USB_SPEED      adi_musbmhdrc_SpdGet                     (ADI_USB_HANDLE const hDevice);

static  ADI_USB_RESULT     adi_musbmhdrc_Suspend                    (ADI_USB_HANDLE const hDevice);

static  ADI_USB_RESULT     adi_musbmhdrc_Resume                     (ADI_USB_HANDLE const hDevice);

static  uint32_t           adi_musbmhdrc_FrameNbrGet                (ADI_USB_HANDLE const hDevice);

static  ADI_USB_RESULT     adi_musbmhdrc_EP_Open                    (ADI_USB_HANDLE const hDevice, ADI_USB_EP_DESC *pEpDesc, uint8_t DevAddr);

static  ADI_USB_RESULT     adi_musbmhdrc_EP_Close                   (ADI_USB_HANDLE const hDevice, uint32_t EpData);

static  ADI_USB_RESULT     adi_musbmhdrc_EP_Abort                   (ADI_USB_HANDLE const hDevice, uint32_t EpData);

static  uint8_t            adi_musbmhdrc_IsHalt_EP                  (ADI_USB_HANDLE const hDevice, uint32_t EpData);

static  ADI_USB_RESULT     adi_musbmhdrc_EP_ResetDataToggle         (ADI_USB_HANDLE const hDevice, uint32_t EpData);

static  ADI_USB_RESULT     adi_musbmhdrc_SetDevAddress              (ADI_USB_HANDLE const hDevice, uint8_t DevAddr);

static  ADI_USB_RESULT     adi_musbmhdrc_SubmitURB                  (ADI_USB_HANDLE const hDevice, ADI_USB_URB *pUsbUrb);

static  ADI_USB_RESULT     adi_musbmhdrc_CompleteURB                (ADI_USB_HANDLE const hDevice, ADI_USB_URB *pUsbUrb);

static  ADI_USB_RESULT     adi_musbmhdrc_AbortURB                   (ADI_USB_HANDLE const hDevice, ADI_USB_URB *pUsbUrb);

static  ADI_USB_RESULT     adi_musbmhdrc_RegisterEpZeroCallback     (ADI_USB_HANDLE const hDevice, ADI_CALLBACK pfEpZeroCallback);

static  ADI_USB_RESULT     adi_musbmhdrc_RegisterEpDataCallback     (ADI_USB_HANDLE const hDevice, ADI_CALLBACK pfEpDataCallback);

static  ADI_USB_RESULT     adi_musbmhdrc_RegisterBusEventCallback   (ADI_USB_HANDLE const hDevice, ADI_CALLBACK pfBusEventCallback);

#if (ADI_CFG_USB_OTG_HOST == 1)
/*
 * Root HUB API functions
 */
static  ADI_USB_RESULT     adi_musbmhdrc_PortStatusGet              (ADI_USB_HANDLE const hDevice, const uint32_t PortNumber, ADI_USB_HUB_PORTSTATUS *pPortStatus);

static  ADI_USB_RESULT     adi_musbmhdrc_HubDescGet                 (ADI_USB_HANDLE const hDevice, void *pBuf, const uint32_t BufLen);

static  ADI_USB_RESULT     adi_musbmhdrc_PortEnSet                  (ADI_USB_HANDLE const hDevice, const uint32_t PortNumber);

static  ADI_USB_RESULT     adi_musbmhdrc_PortEnClr                  (ADI_USB_HANDLE const hDevice, const uint32_t PortNumber);

static  ADI_USB_RESULT     adi_musbmhdrc_PortEnChngClr              (ADI_USB_HANDLE const hDevice, const uint32_t PortNumber);

static  ADI_USB_RESULT     adi_musbmhdrc_PortPwrSet                 (ADI_USB_HANDLE const hDevice, const uint32_t PortNumber);

static  ADI_USB_RESULT     adi_musbmhdrc_PortPwrClr                 (ADI_USB_HANDLE const hDevice, const uint32_t PortNumber);

static  ADI_USB_RESULT     adi_musbmhdrc_PortResetSet               (ADI_USB_HANDLE const hDevice, const uint32_t PortNumber);

static  ADI_USB_RESULT     adi_musbmhdrc_PortResetChngClr           (ADI_USB_HANDLE const hDevice, const uint32_t PortNumber);

static  ADI_USB_RESULT     adi_musbmhdrc_PortSuspendClr             (ADI_USB_HANDLE const hDevice, const uint32_t PortNumber);

static  ADI_USB_RESULT     adi_musbmhdrc_PortConnChngClr            (ADI_USB_HANDLE const hDevice, const uint32_t PortNumber);

static  ADI_USB_RESULT     adi_musbmhdrc_RHIntEn                    (ADI_USB_HANDLE const hDevice);

static  ADI_USB_RESULT     adi_musbmhdrc_RHIntDis                   (ADI_USB_HANDLE const hDevice);

static  ADI_USB_RESULT     adi_musbmhdrc_PortConnSet                (ADI_USB_HANDLE const hDevice, const uint32_t PortNumber);

static  ADI_USB_RESULT     adi_musbmhdrc_PortConnChngSet            (ADI_USB_HANDLE const hDevice, const uint32_t PortNumber);
#endif

/*
 * Internal functions
 */
static void                adi_usbdrv_BusEventHandler               (ADI_USB_HANDLE     const hDevice, const uint16_t IrqStatus);
static void                adi_usbdrv_UsbTxRxInterruptHandler       (ADI_USB_HANDLE     const hDevice, const uint16_t TxIrqStatus,
                                                                     const uint16_t     RxIrqStatus);
static void                adi_usbdrv_TxDataHandler                 (ADI_MUSBMHDRC_DATA *pUsbDrvData, uint16_t        IntStatus);
static void                adi_usbdrv_RxDataHandler                 (ADI_MUSBMHDRC_DATA *pUsbDrvData, uint16_t        IntStatus);
static void                adi_usbdrv_ProcessEp0TxData              (ADI_MUSBMHDRC_DATA *pUsbDrvData, ADI_USB_EP_INFO *pEpInfo, uint16_t *SavedEpNum);
static void                adi_usbdrv_DataDmaHandler                (ADI_MUSBMHDRC_DATA *pUsbDrvData, ADI_USB_EP_INFO *pEpInfo);
static void                adi_usbdrv_Ep0DmaHandler                 (ADI_MUSBMHDRC_DATA *pUsbDrvData, ADI_USB_EP_INFO *pEpInfo);
static uint16_t            adi_usbdrv_GetEpPhyAddrUsedByDma         (ADI_MUSBMHDRC_DATA *pUsbDrvData, uint32_t DmaChannelIndex);
static uint16_t            adi_usbdrv_HandleRxErrors                (ADI_MUSBMHDRC_DATA *pUsbDrvData, ADI_USB_EP_INFO *pEpInfo, uint16_t RegRXCSR);
static uint16_t            adi_usbdrv_HandleTxErrors                (ADI_MUSBMHDRC_DATA *pUsbDrvData, ADI_USB_EP_INFO *pEpInfo, uint16_t RegTXCSR);
static uint16_t            adi_usbdrv_HandleEp0Errors               (ADI_MUSBMHDRC_DATA *pUsbDrvData, ADI_USB_EP_INFO *pEpInfo, uint16_t RegCSR0);
static ADI_USB_RESULT      adi_usbdrv_AllocEpinfo                   (ADI_MUSBMHDRC_DATA *pUsbDrvData, ADI_USB_EP_DESC *pEpDesc, ADI_USB_EP_INFO **pEpInfo);
static ADI_USB_RESULT      adi_usbdrv_ClearEpInfo                   (ADI_MUSBMHDRC_DATA *pUsbDrvData, uint32_t EpData);
static void                adi_usbdrv_BindPhysicalEndpoint          (ADI_MUSBMHDRC_DATA *pUsbDrvData, ADI_USB_EP_INFO *pEpInfo);
static void                adi_usbdrv_ResetEndpointZero             (ADI_MUSBMHDRC_DATA *pUsbDrvData);
static void                adi_usbdrv_ResetPhysicalEndpoint         (ADI_MUSBMHDRC_DATA *pUsbDrvData, ADI_USB_EP_INFO *pEpInfo);
static ADI_USB_RESULT      adi_usbdrv_SubmitRxBuffer                (ADI_MUSBMHDRC_DATA *pUsbDrvData, ADI_USB_EP_INFO *pEpInfo);
static ADI_USB_RESULT      adi_usbdrv_SubmitTxBuffer                (ADI_MUSBMHDRC_DATA *pUsbDrvData, ADI_USB_EP_INFO *pEpInfo);
static ADI_USB_RESULT      adi_usbdrv_HandleTxZLP                   (ADI_MUSBMHDRC_DATA *pUsbDrvData, ADI_USB_EP_INFO *pEpInfo, uint16_t *savedEpNum);
static void                adi_usbdrv_ProcessRxData                 (ADI_MUSBMHDRC_DATA *pUsbDrvData, ADI_USB_EP_INFO *pEpInfo);
static void                adi_usbdrv_ProcessTxData                 (ADI_MUSBMHDRC_DATA *pUsbDrvData, ADI_USB_EP_INFO *pEpInfo);
static ADI_USB_RESULT      adi_usbdrv_HandleRxPacket                (ADI_MUSBMHDRC_DATA *pUsbDrvData, ADI_USB_EP_INFO *pEpInfo, uint16_t *savedEpNum);
static void                adi_usbdrv_ProcessRxZLP                  (ADI_MUSBMHDRC_DATA *pUsbDrvData, ADI_USB_EP_INFO *pEpInfo, uint16_t *SavedEpNum);
static void                adi_usbdrv_RcvDataFromFifo               (ADI_MUSBMHDRC_DATA *pUsbDrvData, ADI_USB_EP_INFO *pEpInfo, uint16_t *SavedEpNum);
static ADI_USB_RESULT      adi_usbdrv_ScheduleTransfer              (ADI_MUSBMHDRC_DATA *pUsbDrvData, ADI_USB_EP_INFO *pEpInfo);
static void                adi_usbdrv_UpdateCSRRegisters            (ADI_MUSBMHDRC_DATA *pUsbDrvData, ADI_USB_EP_INFO *pEpInfo);
static void                adi_usbdrv_ResetDevAddress               (ADI_MUSBMHDRC_DATA *pUsbDrvData);
static void                adi_usbdrv_SetBusEventHandlers           (ADI_MUSBMHDRC_DATA *pUsbDrvData);
static void                adi_usbdrv_FlushCachedBuffer             (void               *pData, uint32_t DataLen, ADI_USB_EP_DIR EpDir);
static bool                adi_usbdrv_IsDataCacheEnabled            (void);


static void                adi_usbdrv_ResetDma                      (ADI_MUSBMHDRC_DATA *pUsbDrvData, ADI_USB_EP_INFO *pEpInfo);
static void                adi_usbdrv_SetDmaAddr                    (ADI_MUSBMHDRC_DATA *pUsbDrvData, ADI_USB_EP_INFO *pEpInfo, uint16_t AddrLow,  uint16_t AddrHigh);
static void                adi_usbdrv_SetDmaCount                   (ADI_MUSBMHDRC_DATA *pUsbDrvData, ADI_USB_EP_INFO *pEpInfo, uint16_t CountLow, uint16_t CountHigh);
static void                adi_usbdrv_SetDmaCtrl                    (ADI_MUSBMHDRC_DATA *pUsbDrvData, ADI_USB_EP_INFO *pEpInfo, uint16_t Data);
static uint16_t            adi_usbdrv_GetDmaCtrl                    (ADI_MUSBMHDRC_DATA *pUsbDrvData, ADI_USB_EP_INFO *pEpInfo);
static ADI_USB_RESULT      adi_usbdrv_InstallInterruptHandlers      (ADI_MUSBMHDRC_DATA *pUsbDrvData);
static ADI_USB_RESULT      adi_usbdrv_UnInstallInterruptHandlers    (ADI_MUSBMHDRC_DATA *pUsbDrvData);
static ADI_USB_RESULT      adi_usbdrv_AllocateDmaChannel            (ADI_MUSBMHDRC_DATA *pUsbDrvData, ADI_USB_EP_INFO *pEpInfo, bool bChannelLock);
static ADI_USB_RESULT      adi_usbdrv_FreeDmaChannel                (ADI_MUSBMHDRC_DATA *pUsbDrvData, ADI_USB_EP_INFO *pEpInfo);
static ADI_USB_RESULT      adi_usbdrv_ReleaseAllDmaChannels         (ADI_MUSBMHDRC_DATA *pUsbDrvData);
static void                adi_usbdrv_AckPendingInterrupts          (ADI_MUSBMHDRC_DATA *pUsbDrvData);
static void                adi_usbdrv_SendResumeSignal              (ADI_MUSBMHDRC_DATA *pUsbDrvData);
static void                adi_usbdrv_FreePhyEndpoints              (ADI_MUSBMHDRC_DATA *pUsbDrvData);

/*
 * Inline Functions
 */

/*Note: Setting index is the responsibility of the caller */
ADI_USB_INLINE
static void adi_usbdrv_FlushRxFifo(volatile ADI_USB_REGS_DEF * pRegs)
{
    if(pRegs->EPI_RXCSR & BITM_USB_EPI_RXCSR_H_RXPKTRDY)
    {
        /* flush twice to take care of double buffered case */
        pRegs->EPI_RXCSR |= ENUM_USB_EPI_RXCSR_H_FLUSH;
        pRegs->EPI_RXCSR |= ENUM_USB_EPI_RXCSR_H_FLUSH;
    }
}

/*Note: Setting index is the responsibility of the caller */
ADI_USB_INLINE
static void adi_usbdrv_FlushTxFifo(volatile ADI_USB_REGS_DEF * pRegs)
{
    if(pRegs->EPI_TXCSR & ENUM_USB_EPI_TXCSR_H_NEFIFO)
    {
        /* flush twice to take care of double buffered case */

        pRegs->EPI_TXCSR |= (ENUM_USB_EPI_TXCSR_H_FLUSH );
        SSYNC();

        if(pRegs->EPI_TXCSR & ENUM_USB_EPI_TXCSR_H_NEFIFO)
        {
        	pRegs->EPI_TXCSR |= (ENUM_USB_EPI_TXCSR_H_FLUSH );
        }
    }
}

/*Note: Setting index is the responsibility of the caller */
ADI_USB_INLINE
static void adi_usbdrv_FlushEp0Fifo(volatile ADI_USB_REGS_DEF *pRegs)
{
    if((ADI_USB_IF_BIT_SET(pRegs->EP0_CSR0, BITM_USB_EP0I_CSR_H_RXPKTRDY)) ||
       (ADI_USB_IF_BIT_SET(pRegs->EP0_CSR0, BITM_USB_EP0I_CSR_H_TXPKTRDY)))
    {
        /* flush twice to take care of double buffered case */
        pRegs->EP0_CSR0 |= (ENUM_USB_EP0I_CSR_H_FLUSH|ENUM_USB_EP0I_CSR_H_DISPING);
        pRegs->EP0_CSR0 |= (ENUM_USB_EP0I_CSR_H_FLUSH|ENUM_USB_EP0I_CSR_H_DISPING);
    }
}

ADI_USB_INLINE
static void adi_usbdrv_ResetCurrentURB(ADI_USB_EP_INFO *pEpInfo)
{
    pEpInfo->EpUrbState = ADI_USB_URB_STATE_FREE;
    pEpInfo->pEpCurURB = NULL;
}

#if (ADI_CFG_USB_OTG_HOST == 1)
static void adi_usbdrvh_SuspendHandler    (ADI_USB_HANDLE     const hDevice);
static void adi_usbdrvh_ConnectHandler    (ADI_USB_HANDLE     const hDevice);
static void adi_usbdrvh_ResetHandler      (ADI_USB_HANDLE     const hDevice);
static void adi_usbdrvh_Ep0Handler        (ADI_USB_HANDLE     const hDevice);
static void adi_usbdrvh_EnableHost        (ADI_MUSBMHDRC_DATA *pUsbDrvData);
static void adi_usbdrvh_DisableHost       (ADI_MUSBMHDRC_DATA *pUsbDrvData);
static void adi_usbdrvh_PowerReset        (ADI_MUSBMHDRC_DATA *pUsbDrvData);
static void adi_usbdrvh_ResetMultPointRegs(ADI_MUSBMHDRC_DATA *pUsbDrvData);
#else
static void adi_usbdrvd_SuspendHandler    (ADI_USB_HANDLE     const hDevice);
static void adi_usbdrvd_ConnectHandler    (ADI_USB_HANDLE     const hDevice);
static void adi_usbdrvd_ResetHandler      (ADI_USB_HANDLE     const hDevice);
static void adi_usbdrvd_Ep0Handler        (ADI_USB_HANDLE     const hDevice);
static void adi_usbdrvd_SofHandler        (ADI_USB_HANDLE     const hDevice);
static void adi_usbdrv_HandleSetupPacket  (ADI_MUSBMHDRC_DATA *pUsbDrvData, ADI_USB_EP_INFO *pEpInfo, uint16_t *SavedEpNum);
#endif

#ifdef _MISRA_RULES
#pragma diag(pop)
#endif

#endif /* _ADI_USB_MUSBMHDRC_LOCAL_H_ */
