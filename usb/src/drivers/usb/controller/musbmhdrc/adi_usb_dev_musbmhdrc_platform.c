/*********************************************************************************

  Copyright(c) 2013 Analog Devices, Inc. All Rights Reserved.

  This software is proprietary and confidential.  By using this software you agree
  to the terms of the associated Analog Devices License Agreement.

*********************************************************************************/

/*!
* @file      adi_usb_dev_musbmhdrc_platform.c
*
* @brief     platform specific macros and functions are kept here.
*
*
*/

/** @addtogroup USB MUSBMHDRC controller driver
 *  @{
 */
#ifdef _MISRA_RULES
#pragma diag(push)
#pragma diag(suppress:misra_rules_all:"Suppress all misra rules")
#endif

#include "adi_usb_dev_musbmhdrc_local.h"

/*
 * Physical endpoint configuration 
 */
static ADI_USB_EP_CONFIG ADI_CFG_USB_PHYEP_CONFIG[ADI_CFG_USB_MAX_NUM_URB] =
{
#if defined(ADI_CFG_USB_BF_MUSBMHDRC)
        {0, ADI_USB_EP_TYPE_CONTROL,                                                    ADI_USB_EP_DIR_TX, 64,  0},
        {1, ADI_USB_EP_TYPE_INTERRUPT | ADI_USB_EP_TYPE_BULK | ADI_USB_EP_TYPE_ISOC,    ADI_USB_EP_DIR_RX, 1024,0},
        {1, ADI_USB_EP_TYPE_INTERRUPT | ADI_USB_EP_TYPE_BULK | ADI_USB_EP_TYPE_ISOC,    ADI_USB_EP_DIR_TX, 1024,0},
        {2, ADI_USB_EP_TYPE_INTERRUPT | ADI_USB_EP_TYPE_BULK | ADI_USB_EP_TYPE_ISOC,    ADI_USB_EP_DIR_RX, 1024,0},
        {2, ADI_USB_EP_TYPE_INTERRUPT | ADI_USB_EP_TYPE_BULK | ADI_USB_EP_TYPE_ISOC,    ADI_USB_EP_DIR_TX, 1024,0},
        {3, ADI_USB_EP_TYPE_INTERRUPT | ADI_USB_EP_TYPE_BULK | ADI_USB_EP_TYPE_ISOC,    ADI_USB_EP_DIR_RX, 1024,0},
        {3, ADI_USB_EP_TYPE_INTERRUPT | ADI_USB_EP_TYPE_BULK | ADI_USB_EP_TYPE_ISOC,    ADI_USB_EP_DIR_TX, 1024,0},
        {4, ADI_USB_EP_TYPE_INTERRUPT | ADI_USB_EP_TYPE_BULK | ADI_USB_EP_TYPE_ISOC,    ADI_USB_EP_DIR_RX, 1024,0},
        {4, ADI_USB_EP_TYPE_INTERRUPT | ADI_USB_EP_TYPE_BULK | ADI_USB_EP_TYPE_ISOC,    ADI_USB_EP_DIR_TX, 1024,0},
        {5, ADI_USB_EP_TYPE_INTERRUPT | ADI_USB_EP_TYPE_BULK | ADI_USB_EP_TYPE_ISOC,    ADI_USB_EP_DIR_RX, 1024,0},
        {5, ADI_USB_EP_TYPE_INTERRUPT | ADI_USB_EP_TYPE_BULK | ADI_USB_EP_TYPE_ISOC,    ADI_USB_EP_DIR_TX, 1024,0},
        {6, ADI_USB_EP_TYPE_INTERRUPT | ADI_USB_EP_TYPE_BULK | ADI_USB_EP_TYPE_ISOC,    ADI_USB_EP_DIR_RX, 1024,0},
        {6, ADI_USB_EP_TYPE_INTERRUPT | ADI_USB_EP_TYPE_BULK | ADI_USB_EP_TYPE_ISOC,    ADI_USB_EP_DIR_TX, 1024,0},
        {7, ADI_USB_EP_TYPE_INTERRUPT | ADI_USB_EP_TYPE_BULK | ADI_USB_EP_TYPE_ISOC,    ADI_USB_EP_DIR_RX, 1024,0},
        {7, ADI_USB_EP_TYPE_INTERRUPT | ADI_USB_EP_TYPE_BULK | ADI_USB_EP_TYPE_ISOC,    ADI_USB_EP_DIR_TX, 1024,0},
        {8,  ADI_USB_EP_TYPE_INTERRUPT | ADI_USB_EP_TYPE_BULK | ADI_USB_EP_TYPE_ISOC,   ADI_USB_EP_DIR_RX, 1024,0},
        {8,  ADI_USB_EP_TYPE_INTERRUPT | ADI_USB_EP_TYPE_BULK | ADI_USB_EP_TYPE_ISOC,   ADI_USB_EP_DIR_TX, 1024,0},
        {9,  ADI_USB_EP_TYPE_INTERRUPT | ADI_USB_EP_TYPE_BULK | ADI_USB_EP_TYPE_ISOC,   ADI_USB_EP_DIR_RX, 1024,0},
        {9,  ADI_USB_EP_TYPE_INTERRUPT | ADI_USB_EP_TYPE_BULK | ADI_USB_EP_TYPE_ISOC,   ADI_USB_EP_DIR_TX, 1024,0},
        {10, ADI_USB_EP_TYPE_INTERRUPT | ADI_USB_EP_TYPE_BULK | ADI_USB_EP_TYPE_ISOC,   ADI_USB_EP_DIR_RX, 1024,0},
        {10, ADI_USB_EP_TYPE_INTERRUPT | ADI_USB_EP_TYPE_BULK | ADI_USB_EP_TYPE_ISOC,   ADI_USB_EP_DIR_TX, 1024,0},
        {11, ADI_USB_EP_TYPE_INTERRUPT | ADI_USB_EP_TYPE_BULK | ADI_USB_EP_TYPE_ISOC,   ADI_USB_EP_DIR_RX, 1024,0},
        {11, ADI_USB_EP_TYPE_INTERRUPT | ADI_USB_EP_TYPE_BULK | ADI_USB_EP_TYPE_ISOC,   ADI_USB_EP_DIR_TX, 1024,0},
#elif defined(ADI_CFG_USB_ARM_MUSBMHDRC)
        {0, ADI_USB_EP_TYPE_CONTROL,                                                    ADI_USB_EP_DIR_TX, 64,  0},
        {1, ADI_USB_EP_TYPE_INTERRUPT | ADI_USB_EP_TYPE_BULK | ADI_USB_EP_TYPE_ISOC,    ADI_USB_EP_DIR_RX, 64,  0},
        {1, ADI_USB_EP_TYPE_INTERRUPT | ADI_USB_EP_TYPE_BULK | ADI_USB_EP_TYPE_ISOC,    ADI_USB_EP_DIR_TX, 64,  0},       
        {2, ADI_USB_EP_TYPE_INTERRUPT | ADI_USB_EP_TYPE_BULK | ADI_USB_EP_TYPE_ISOC,    ADI_USB_EP_DIR_RX, 64,  0},
        {2, ADI_USB_EP_TYPE_INTERRUPT | ADI_USB_EP_TYPE_BULK | ADI_USB_EP_TYPE_ISOC,    ADI_USB_EP_DIR_TX, 64,  0},
        {3, ADI_USB_EP_TYPE_INTERRUPT | ADI_USB_EP_TYPE_BULK | ADI_USB_EP_TYPE_ISOC,    ADI_USB_EP_DIR_RX, 64,  0},
        {3, ADI_USB_EP_TYPE_INTERRUPT | ADI_USB_EP_TYPE_BULK | ADI_USB_EP_TYPE_ISOC,    ADI_USB_EP_DIR_TX, 64,  0},        
#else
        {0, ADI_USB_EP_TYPE_CONTROL,                                                    ADI_USB_EP_DIR_TX, 64, 0},
        {1, ADI_USB_EP_TYPE_INTERRUPT | ADI_USB_EP_TYPE_BULK | ADI_USB_EP_TYPE_ISOC,    ADI_USB_EP_DIR_RX, 128,0},
        {1, ADI_USB_EP_TYPE_INTERRUPT | ADI_USB_EP_TYPE_BULK | ADI_USB_EP_TYPE_ISOC,    ADI_USB_EP_DIR_TX, 128,0},
        {2, ADI_USB_EP_TYPE_INTERRUPT | ADI_USB_EP_TYPE_BULK | ADI_USB_EP_TYPE_ISOC,    ADI_USB_EP_DIR_RX, 128,0},
        {2, ADI_USB_EP_TYPE_INTERRUPT | ADI_USB_EP_TYPE_BULK | ADI_USB_EP_TYPE_ISOC,    ADI_USB_EP_DIR_TX, 128,0},
        {3, ADI_USB_EP_TYPE_INTERRUPT | ADI_USB_EP_TYPE_BULK | ADI_USB_EP_TYPE_ISOC,    ADI_USB_EP_DIR_RX, 128,0},
        {3, ADI_USB_EP_TYPE_INTERRUPT | ADI_USB_EP_TYPE_BULK | ADI_USB_EP_TYPE_ISOC,    ADI_USB_EP_DIR_TX, 128,0},
        {4, ADI_USB_EP_TYPE_INTERRUPT | ADI_USB_EP_TYPE_BULK | ADI_USB_EP_TYPE_ISOC,    ADI_USB_EP_DIR_RX, 128,0},
        {4, ADI_USB_EP_TYPE_INTERRUPT | ADI_USB_EP_TYPE_BULK | ADI_USB_EP_TYPE_ISOC,    ADI_USB_EP_DIR_TX, 128,0},
        {5, ADI_USB_EP_TYPE_INTERRUPT | ADI_USB_EP_TYPE_BULK | ADI_USB_EP_TYPE_ISOC,    ADI_USB_EP_DIR_RX, 1024,0},
        {5, ADI_USB_EP_TYPE_INTERRUPT | ADI_USB_EP_TYPE_BULK | ADI_USB_EP_TYPE_ISOC,    ADI_USB_EP_DIR_TX, 1024,0},
        {6, ADI_USB_EP_TYPE_INTERRUPT | ADI_USB_EP_TYPE_BULK | ADI_USB_EP_TYPE_ISOC,    ADI_USB_EP_DIR_RX, 1024,0},
        {6, ADI_USB_EP_TYPE_INTERRUPT | ADI_USB_EP_TYPE_BULK | ADI_USB_EP_TYPE_ISOC,    ADI_USB_EP_DIR_TX, 1024,0},
        {7, ADI_USB_EP_TYPE_INTERRUPT | ADI_USB_EP_TYPE_BULK | ADI_USB_EP_TYPE_ISOC,    ADI_USB_EP_DIR_RX, 1024,0},
        {7, ADI_USB_EP_TYPE_INTERRUPT | ADI_USB_EP_TYPE_BULK | ADI_USB_EP_TYPE_ISOC,    ADI_USB_EP_DIR_TX, 1024,0},

#endif
};

#if (ADI_CFG_USB_OTG_HOST == 1)

static void adi_usbdrv_DriveVbusOn(ADI_MUSBMHDRC_DATA *pUsbDrvData)
{
#if defined(__ADSPBF526_FAMILY__)
    adi_gpio_Clear( pUsbDrvData->VBusDrivePort,pUsbDrvData->VBusDrivePin);
#elif defined(__ADSPBF527_FAMILY__) || defined(__ADSPBF548_FAMILY__)
    adi_gpio_Set(pUsbDrvData->VBusDrivePort, pUsbDrvData->VBusDrivePin);
#elif defined(__ADSPBF609_FAMILY__) || defined(__ADSPBF707_FAMILY__) || defined(__ADSPSC589_FAMILY__)
    pUsbDrvData->pRegs->VBUS_CTL = 0 ;
    pUsbDrvData->pRegs->PHY_CTL  = BITM_USB_PHY_CTL_EN;
#endif
     adi_usbdrv_WaitMilliSec(20);
     SSYNC();
}

/* This function will driver VBUS OFF */
static void adi_usbdrv_DriveVbusOff(ADI_MUSBMHDRC_DATA *pUsbDrvData)
{
    uint16_t frame;
#if defined(__ADSPBF526_FAMILY__)
    adi_gpio_Set( pUsbDrvData->VBusDrivePort, pUsbDrvData->VBusDrivePin);
#elif defined(__ADSPBF527_FAMILY__) || defined(__ADSPBF548_FAMILY__)
    adi_gpio_Clear(pUsbDrvData->VBusDrivePort, pUsbDrvData->VBusDrivePin);
#elif defined(__ADSPBF609_FAMILY__) || defined(__ADSPBF707_FAMILY__) || defined(__ADSPSC589_FAMILY__)
    pUsbDrvData->pRegs->VBUS_CTL = BITM_USB_VBUS_CTL_INVDRV | BITM_USB_VBUS_CTL_DRVOD ;
#endif

    SSYNC();
    frame = pUsbDrvData->pRegs->FRAME;

    /* Wait until VBUS goes low */
    while((pUsbDrvData->pRegs->FRAME)==frame && (pUsbDrvData->pRegs->DEV_CTL & 0x0010) > 1) ;
}
#endif

#if defined(ADI_CFG_USB_BF_MUSBHDRC)
static ADI_USB_RESULT adi_usbdrv_ConfigGPIO(ADI_MUSBMHDRC_DATA *pUsbDrvData)
{
    ADI_GPIO_RESULT GPIOResult;

    GPIOResult = adi_gpio_SetDirection( pUsbDrvData->VBusDrivePort,
                                        pUsbDrvData->VBusDrivePin,
                                        ADI_GPIO_DIRECTION_OUTPUT);

    if(GPIOResult != ADI_GPIO_SUCCESS)
    {
        return ADI_USB_RESULT_FAIL;
    }

#if (ADI_CFG_USB_VBUS_ASSRT_POLARITY == 0u)
    GPIOResult = adi_gpio_Clear( pUsbDrvData->VBusDrivePort, pUsbDrvData->VBusDrivePin);
#else
    GPIOResult = adi_gpio_Set( pUsbDrvData->VBusDrivePort, pUsbDrvData->VBusDrivePin);
#endif

    if(GPIOResult != ADI_GPIO_SUCCESS)
    {
        return ADI_USB_RESULT_FAIL;
    }
    return ADI_USB_RESULT_SUCCESS;
}
#endif


static ADI_USB_RESULT adi_usbdrv_InstallInterruptHandlers(ADI_MUSBMHDRC_DATA *pUsbDrvData)
{
    uint32_t Result = ADI_USB_RESULT_FAIL;

#if defined(ADI_CFG_USB_BF_MUSBHDRC)
    Result = adi_int_InstallHandler(ADI_SID_USB_INT0, adi_usbdrv_UsbOtgHandler, pUsbDrvData, true);
    if(Result)
    {
        return ADI_USB_RESULT_FAIL;
    }

    Result = adi_int_InstallHandler(ADI_SID_USB_INT1, adi_usbdrv_USBInterruptHandler, pUsbDrvData, true);
    if(Result)
    {
        return ADI_USB_RESULT_FAIL;
    }
    Result = adi_int_InstallHandler(ADI_SID_USB_INT2, adi_usbdrv_USBInterruptHandler, pUsbDrvData, true);
    if(Result)
    {
        return ADI_USB_RESULT_FAIL;
    }
    Result = adi_int_InstallHandler(ADI_SID_USB_DMAINT, adi_usbdrv_DmaHandler, pUsbDrvData, true);

    if(Result)
    {
        return ADI_USB_RESULT_FAIL;
    }
    /*
     * Enable USB and the data endpoint logic by setting appropriate bits in Global Control register
     * With the execution of below statement we are ready to receive and transmit setup packets on EP0
     */
    pUsbDrvData->pRegs->GLOBAL_CTL = (USB_GLOBAL_ENABLE_B |
                                        USB_GLOBAL_CTRL_ALL_TX_RX_B );
    pUsbDrvData->pRegs->PHY_CNTRL2 = 0x2;


#elif defined(ADI_CFG_USB_BF_MUSBMHDRC) || defined(ADI_CFG_USB_ARM_MUSBMHDRC)
    Result = adi_int_InstallHandler(INTR_USB0_STAT, adi_usbdrv_UsbOtgHandler, pUsbDrvData, true);

    if(Result)
    {
        return ADI_USB_RESULT_FAIL;
    }

    Result = adi_int_InstallHandler(INTR_USB0_DATA, adi_usbdrv_DmaHandler, pUsbDrvData, true);

    if(Result)
    {
        return ADI_USB_RESULT_FAIL;
    }
    
#else
#error "Processor not supported"
#endif
    return ((ADI_USB_RESULT)Result);
}

static void adi_usbdrv_AckPendingInterrupts(ADI_MUSBMHDRC_DATA *pUsbDrvData)
{
    volatile ADI_USB_REGS_DEF     *pRegs = pUsbDrvData->pRegs;
    ADI_SUP_UNUSED_WARN uint32_t  RegVal;

#if defined(ADI_CFG_USB_BF_MUSBHDRC)  /* W1C registers */
    pRegs->IRQ        = (pRegs->IRQ    & 0xff);
    pRegs->INTRTX     = (pRegs->INTRTX & 0xff);
    pRegs->INTRRX     = (pRegs->INTRRX & 0xff);
    pRegs->GLOBAL_CTL = 0;
#elif defined(ADI_CFG_USB_BF_MUSBMHDRC) || defined(ADI_CFG_USB_ARM_MUSBMHDRC) /* Read to clear */
    RegVal = pRegs->IRQ;
    RegVal = pRegs->INTRRX;
    RegVal = pRegs->INTRTX;
#endif
    pRegs->IEN     = 0;
    pRegs->INTRTXE = 0;
    pRegs->INTRRXE = 0;
}

static ADI_USB_RESULT adi_usbdrv_UnInstallInterruptHandlers(ADI_MUSBMHDRC_DATA *pUsbDrvData)
{
uint32_t Result = (uint32_t)ADI_USB_RESULT_FAIL;

#if defined(ADI_CFG_USB_BF_MUSBHDRC)
    Result = adi_int_UninstallHandler(ADI_SID_USB_INT0);
    if(Result)
    {
        return ADI_USB_RESULT_FAIL;
    }
    Result = adi_int_UninstallHandler(ADI_SID_USB_INT1);
    if(Result)
    {
        return ADI_USB_RESULT_FAIL;
    }
    Result = adi_int_UninstallHandler(ADI_SID_USB_INT2);
    if(Result)
    {
        return ADI_USB_RESULT_FAIL;
    }
    Result = adi_int_UninstallHandler(ADI_SID_USB_DMAINT);
    if(Result)
    {
        return ADI_USB_RESULT_FAIL;
    }

#elif defined(ADI_CFG_USB_BF_MUSBMHDRC) || defined(ADI_CFG_USB_ARM_MUSBMHDRC)
    Result = adi_int_UninstallHandler(INTR_USB0_STAT);
    if(Result)
    {
        return ADI_USB_RESULT_FAIL;
    }
    Result = adi_int_UninstallHandler(INTR_USB0_DATA);
    if(Result)
    {
        return ADI_USB_RESULT_FAIL;
    }

#else    
#endif

    return ((ADI_USB_RESULT)Result);
}

#if defined(ADI_CFG_USB_BF_MUSBHDRC)
/* This function will set Clock Divider */
static void adi_usbdrv_SetClockDiv(ADI_MUSBMHDRC_DATA *pUsbDrvData)
{
    uint32_t uCoreClock    = 0, uSystemClock=0;
    uint32_t uClockDivUsb  = 0;
    uint32_t uSystemClock0 = 0, uSystemClock1=0;

    adi_pwr_GetCoreFreq(&uCoreClock);
    adi_pwr_GetSystemFreq(&uSystemClock);

    uSystemClock = uSystemClock/1000;
    uClockDivUsb = ((uSystemClock/32 ) - 1);
    if (uClockDivUsb>0xFFF)
    {
        uClockDivUsb = 0xFFF;
    }
    pUsbDrvData->pRegs->CLK_DIV |= (((uint16_t)uClockDivUsb) & 0xfff);
}
#endif

/*
 * Sets DMA control register with the passed data and for the specified endpoint
 */
static void adi_usbdrv_SetDmaCtrl(ADI_MUSBMHDRC_DATA *pUsbDrvData, ADI_USB_EP_INFO *pEpInfo, uint16_t Data)
{
    uint16_t volatile *paddr = (uint16_t*)(&pUsbDrvData->pRegs->DmaRegs[pEpInfo->EpDmaChannel].DMA_CTL);
    *paddr = Data;
    SSYNC();
}

static uint16_t adi_usbdrv_GetDmaCtrl(ADI_MUSBMHDRC_DATA *pUsbDrvData, ADI_USB_EP_INFO *pEpInfo)
{
    uint16_t volatile *paddr = (uint16_t*)(&pUsbDrvData->pRegs->DmaRegs[pEpInfo->EpDmaChannel].DMA_CTL);
    return (*paddr);
}
/*
 * Sets DMA address register with the passed data and for the specified endpoint
 */
static void adi_usbdrv_SetDmaAddr(ADI_MUSBMHDRC_DATA *pUsbDrvData, ADI_USB_EP_INFO *pEpInfo, uint16_t AddrLow, uint16_t AddrHigh)
{
#if defined(ADI_CFG_USB_BF_MUSBHDRC)
    uint16_t volatile *paddr = (uint16_t*)(&pUsbDrvData->pRegs->DmaRegs[pEpInfo->EpDmaChannel].DMA_ADDRLOW);
    *paddr = AddrLow;
    SSYNC();
    paddr = (uint16_t*)(&pUsbDrvData->pRegs->DmaRegs[pEpInfo->EpDmaChannel].DMA_ADDRHIGH);
    *paddr = AddrHigh;
    SSYNC();
#elif defined(ADI_CFG_USB_BF_MUSBMHDRC) || defined(ADI_CFG_USB_ARM_MUSBMHDRC)
    uint32_t volatile *paddr = (uint32_t*)(&pUsbDrvData->pRegs->DmaRegs[pEpInfo->EpDmaChannel].DMA_ADDRHIGH);
    *paddr = (uint32_t) (AddrLow | (AddrHigh << 16u));
    SSYNC();
#endif
}
/*
 * Sets DMA count register with the passed data and for the specified endpoint
 */
static void adi_usbdrv_SetDmaCount(ADI_MUSBMHDRC_DATA *pUsbDrvData, ADI_USB_EP_INFO *pEpInfo,uint16_t CountLow, uint16_t CountHigh)
{
#if defined(ADI_CFG_USB_BF_MUSBHDRC)
    uint16_t volatile *paddr = (uint16_t*)(&pUsbDrvData->pRegs->DmaRegs[pEpInfo->EpDmaChannel].DMA_CNTLOW);
    *paddr = CountLow;
    SSYNC();
    paddr = (uint16_t*)(&pUsbDrvData->pRegs->DmaRegs[pEpInfo->EpDmaChannel].DMA_CNTHIGH);
    *paddr = CountHigh;
    SSYNC();
#elif defined(ADI_CFG_USB_BF_MUSBMHDRC) || defined(ADI_CFG_USB_ARM_MUSBMHDRC)
    uint32_t volatile *paddr = (uint32_t*)(&pUsbDrvData->pRegs->DmaRegs[pEpInfo->EpDmaChannel].DMA_CNTHIGH);
    *paddr = (uint32_t) (CountLow | (CountHigh << 16u));
    SSYNC();
#endif
}

static void adi_usbdrv_ResetDma(ADI_MUSBMHDRC_DATA *pUsbDrvData, ADI_USB_EP_INFO *pEpInfo)
{
    adi_usbdrv_SetDmaCtrl(pUsbDrvData, pEpInfo, (uint16_t)(0));
    adi_usbdrv_SetDmaAddr(pUsbDrvData, pEpInfo, (uint16_t)(0), (uint16_t)(0));
    adi_usbdrv_SetDmaCount(pUsbDrvData, pEpInfo, (uint16_t)(0), (uint16_t)(0));
    SSYNC();
}

/* Platform Specific initialization */
static ADI_USB_RESULT adi_usbdrv_PlatformSpecInit(ADI_MUSBMHDRC_DATA *pUsbDrvData)
{
    ADI_USB_RESULT Result=ADI_USB_RESULT_SUCCESS;

#if defined(ADI_CFG_USB_BF_MUSBHDRC)
    /* Configure PLL oscillator register */
    pUsbDrvData->pRegs->PLL_OSC = ADI_CFG_USB_PLL_OSCVAL; /* 0x30a8 */
    pUsbDrvData->VBusDrivePort  = ADI_CFG_USB_VBUS_PORT;
    pUsbDrvData->VBusDrivePin   = ADI_CFG_USB_VBUS_PIN;
    /* set USB clock div in case we can get the current system clock value */
    adi_usbdrv_SetClockDiv(pUsbDrvData);

    Result = adi_usbdrv_ConfigGPIO(pUsbDrvData);
#elif defined(ADI_CFG_USB_BF_MUSBMHDRC)
    /* first 64 bytes are reserved for EP0 */
    pUsbDrvData->FifoRamAdress    = ADI_USB_DYNFIFO_START_ADDR; /* 64 */
    pUsbDrvData->FifoRamTotalSize = ADI_USB_DYNFIFO_TOTAL_SIZE; /* 16K */
#endif
    return Result;
}

#ifdef _MISRA_RULES
#pragma diag(pop)
#endif

/*@}*/

