/*********************************************************************************

  Copyright(c) 2013 Analog Devices, Inc. All Rights Reserved.

  This software is proprietary and confidential.  By using this software you agree
  to the terms of the associated Analog Devices License Agreement.

*********************************************************************************/

/*!
* @file      adi_usbh_dev_musbmhdrc_intrpt.c
*
* @brief     USB interrupt handler routine are present in this file
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

/* Forward declarations */
#if (ADI_CFG_USB_OTG_HOST == 1)
static ADI_USB_SPEED adi_usbdrv_GetDevSpeed(uint8_t Power);
#endif


/*
 *  adi_usbdrv_UsbOtgHandler is responsible for handling the core USB interrupts.
 *  Depending on the processor family all USB interrupts are handled here is via
 *  another interrupt line. Primarily this function handles the all USB Bus interrupts
 *  and on BF60x family it handles endpoint TX and RX interrupts as well.
 */
static void adi_usbdrv_UsbOtgHandler(uint32_t iid, void*  handlerArg)
{
    ADI_MUSBMHDRC_DATA        *pUsbDrvData = (ADI_MUSBMHDRC_DATA *)handlerArg;
    volatile ADI_USB_REGS_DEF *pRegs = pUsbDrvData->pRegs;
    uint16_t                  uIrqStatus, uIrqTxStatus, uIrqRxStatus;

#if defined(ADI_DEBUG)
    if (ValidateHandle(handlerArg) != ADI_USB_RESULT_SUCCESS)
    {
        return;
    }
#endif

    uIrqStatus = pRegs->IRQ;

    if (uIrqStatus) 
    {
#if defined(ADI_CFG_USB_BF_MUSBHDRC)
        /* Acknowledge all interrupts */
        pRegs->IRQ |= uIrqStatus;   /* W1C on HDRC */
#endif
        adi_usbdrv_BusEventHandler(pUsbDrvData,uIrqStatus);
    }

    /* MUSBMHDRC uses the same interrupt line for RX and TX as well */
#if defined(ADI_CFG_USB_BF_MUSBMHDRC) || defined(ADI_CFG_USB_ARM_MUSBMHDRC)
    uIrqTxStatus = pUsbDrvData->pRegs->INTRTX;
    uIrqRxStatus = pUsbDrvData->pRegs->INTRRX;

    if (uIrqRxStatus || uIrqTxStatus) 
    {
        adi_usbdrv_UsbTxRxInterruptHandler(handlerArg, uIrqTxStatus,uIrqRxStatus);
    }
#endif

    return;
}

#if defined(ADI_CFG_USB_BF_MUSBHDRC)
static void adi_usbdrv_USBInterruptHandler(uint32_t iid, void* handlerArg)
{
    ADI_MUSBMHDRC_DATA *pUsbDrvData = (ADI_MUSBMHDRC_DATA *)handlerArg;
    uint16_t           uIrqTxStatus = pUsbDrvData->pRegs->INTRTX;
    uint16_t           uIrqRxStatus = pUsbDrvData->pRegs->INTRRX;

    adi_usbdrv_UsbTxRxInterruptHandler(handlerArg, uIrqTxStatus, uIrqRxStatus);
}
#endif


/*
 * DMA interrupt handler.
 * Note: DMA Interrupt must have higher priority than Rx Interrupt 
 */
static void adi_usbdrv_DmaHandler(uint32_t iid, void* handlerArg)
{
    ADI_USB_EP_INFO           *pEpInfo;
    ADI_MUSBMHDRC_DATA        *pUsbDrvData = (ADI_MUSBMHDRC_DATA *)handlerArg;
    volatile ADI_USB_REGS_DEF *pRegs;
    uint8_t                   EpPhyAddr;
    uint32_t                  DmaIndex,DmaIrq;

#if defined(ADI_DEBUG)
    if (ValidateHandle(handlerArg) != ADI_USB_RESULT_SUCCESS) {
        return;
    }
#endif
    pRegs  = pUsbDrvData->pRegs;

    ADI_USB_ENTER_CR();

    DmaIrq = pRegs->DMA_IRQ;

#if defined(ADI_CFG_USB_BF_MUSBHDRC)
    /* Acknowledge dma interrupts */
    pRegs->DMA_IRQ = DmaIrq;
#endif

    for(DmaIndex = 0u; DmaIndex < ADI_CFG_USB_MAX_DMA_CHANNELS; DmaIndex++)
    {
        if (DmaIrq & (0x1u << DmaIndex))
        {
            EpPhyAddr = adi_usbdrv_GetEpPhyAddrUsedByDma(pUsbDrvData,DmaIndex);
            pEpInfo   = adi_usbdrv_GetEpInfoFromEpPhy(pUsbDrvData, ADI_USB_GET_EP_NUM(EpPhyAddr), ADI_USB_GET_EP_DIR(EpPhyAddr));
            assert(pEpInfo != NULL);

            /* Update processed bytes and Buffer pointer */
            pEpInfo->pEpCurURB->ProcessedBytes += pEpInfo->EpProcBytesThisPass;
            pEpInfo->pEpBufferdata             += pEpInfo->EpProcBytesThisPass;

            if(pEpInfo->EpPhyNum == ADI_USB_EP_NUM_0)
            {
                adi_usbdrv_Ep0DmaHandler(pUsbDrvData, pEpInfo);
            }
            else
            {
                adi_usbdrv_DataDmaHandler(pUsbDrvData, pEpInfo);
            }
        }
    }

    ADI_USB_EXIT_CR();

    return;
}

static void adi_usbdrv_BusEventHandler(ADI_USB_HANDLE const hDevice, const uint16_t IrqStatus)
{
    ADI_MUSBMHDRC_DATA         *pUsbDrvData = (ADI_MUSBMHDRC_DATA *)hDevice;
    volatile ADI_USB_REGS_DEF  *pRegs = pUsbDrvData->pRegs;
    ADI_SUP_UNUSED_WARN int    uIrqTxStatus;
    uint8_t                    DevNum = pUsbDrvData->DevNum;

#if !defined(ADI_CFG_USB_ARM_MUSBMHDRC) /* Not available for ADUCM350 */
    if(IrqStatus & BITM_USB_IRQ_VBUSERR)
    {
        ADI_USB_STATINC(pUsbDrvData,nVbusErr);
#if (ADI_CFG_USB_OTG_HOST == 1)
        adi_usbdrvh_DisableHost(pUsbDrvData);
        adi_usbdrvh_EnableHost(pUsbDrvData);
#else
        /* FIXIT */
#endif
    }

    if(IrqStatus & BITM_USB_IRQ_SESSREQ)
    {
        assert(pUsbDrvData->DevMode == ADI_USB_MODE_OTG_HOST);
        ADI_USB_STATINC(pUsbDrvData,nSessReq);

        adi_usbdrv_SetSessionBit(pUsbDrvData);
    }
#endif    

    if(IrqStatus & BITM_USB_IRQ_DISCON)
    {
        ADI_USB_STATINC(pUsbDrvData,nDisCon);

        pUsbDrvData->DevState = ADI_USB_DEV_STATE_DISCON;

        /* Since device is disconnected we should reset the FADDR address */
        adi_usbdrv_ResetDevAddress(pUsbDrvData);
        adi_usbdrv_ReleaseAllDmaChannels(pUsbDrvData);

#if (ADI_USBH_MULTI_POINT_EN == 1)
#if (ADI_CFG_USB_OTG_HOST == 1)
        adi_usbdrvh_ResetMultPointRegs(pUsbDrvData);
#else
        /* FIXIT */
#endif
#endif

        pRegs->POWER |= ENUM_USB_POWER_RESET;
        pUsbDrvData->pfBusEventCallback(pUsbDrvData, (uint32_t)ADI_USB_BUS_EVENT_DEV_DISCON, &DevNum);
    }

#if !defined(ADI_CFG_USB_ARM_MUSBMHDRC) /* Not available for ADUCM350 */
    if(IrqStatus & BITM_USB_IRQ_CON)
    {
        assert(pUsbDrvData->pfConnectHandler   != NULL);
        assert(pUsbDrvData->pfBusEventCallback != NULL);
        ADI_USB_STATINC(pUsbDrvData,nConnect);

        pUsbDrvData->pfConnectHandler(hDevice);
        pUsbDrvData->pfBusEventCallback(pUsbDrvData, (uint32_t)ADI_USB_BUS_EVENT_DEV_CON, &DevNum);
    }
#endif
    
    if(IrqStatus & BITM_USB_IRQ_SOF)
    {
#if (ADI_CFG_USB_DEVICE == 1)
        if(pRegs->IEN & BITM_USB_IRQ_SOF)
        {
            adi_usbdrvd_SofHandler(hDevice);
        }
#endif
        /* Do Nothing */
    }

    if(IrqStatus & BITM_USB_IRQ_RSTBABBLE)
    {
        assert(pUsbDrvData->pfResetHandler != NULL);
        ADI_USB_STATINC(pUsbDrvData,nRstBabble);

        pUsbDrvData->DevState = ADI_USB_DEV_STATE_RESET;
        pUsbDrvData->pfResetHandler(hDevice);

    }

    if(IrqStatus & BITM_USB_IRQ_RESUME)
    {
        assert(pUsbDrvData->pfBusEventCallback != NULL);
        ADI_USB_STATINC(pUsbDrvData,nResume);

        pUsbDrvData->pfBusEventCallback(pUsbDrvData, (uint32_t)ADI_USB_BUS_EVENT_RESUME, &DevNum);
    }

    if(IrqStatus & BITM_USB_IRQ_SUSPEND)
    {
        assert(pUsbDrvData->pfSuspendHandler != NULL);
        ADI_USB_STATINC(pUsbDrvData,nSuspend);

        pUsbDrvData->pfSuspendHandler(hDevice);
        /* FIXIT: Below line added to avoid the false EP0 interrupt */
        uIrqTxStatus = pUsbDrvData->pRegs->INTRTX;
    }

    return;
}


static void adi_usbdrv_UsbTxRxInterruptHandler(ADI_USB_HANDLE const hDevice, const uint16_t TxInterrupts, const uint16_t RxInterrupts)
{
    ADI_MUSBMHDRC_DATA        *pUsbDrvData = (ADI_MUSBMHDRC_DATA *)hDevice;

    if (TxInterrupts)
    {
        if(TxInterrupts & BITM_USB_INTRTX_EP0)
        {
            pUsbDrvData->pfEp0Handler(hDevice);
        }
        else
        {
            adi_usbdrv_TxDataHandler(pUsbDrvData, TxInterrupts);
        }
    }

    if (RxInterrupts)
    {
        adi_usbdrv_RxDataHandler(pUsbDrvData, RxInterrupts);
    }

    return;
}

#if (ADI_CFG_USB_OTG_HOST == 1)
static void adi_usbdrvh_Ep0Handler(ADI_USB_HANDLE const hDevice)
{
    ADI_MUSBMHDRC_DATA        *pUsbDrvData = (ADI_MUSBMHDRC_DATA *)hDevice;
    volatile ADI_USB_REGS_DEF *pRegs;
    ADI_USB_URB               *pEpCurURB;
    uint16_t                  SavedEpNum, RegCSR0, ErrEvent;
    ADI_USB_EP_INFO           *pEpInfo = NULL;
    
    pRegs = pUsbDrvData->pRegs;

    pEpInfo =  adi_usbdrv_GetEpInfo(pUsbDrvData, ADI_USB_EP_NUM_0); 
    assert(pEpInfo != NULL);

    ADI_USB_ENTER_CR();

    SavedEpNum   = pRegs->INDEX;
    pRegs->INDEX = pEpInfo->EpPhyNum;

#if defined(ADI_CFG_USB_BF_MUSBHDRC)
    /* clear the interrupt bit */
    pRegs->INTRTX = (1u << pEpInfo->EpPhyNum);
#endif

    RegCSR0 = pRegs->EP0_CSR0;

    /* Check EP0 Errors*/
    if ( RegCSR0 & (BITM_USB_EP0I_CSR_H_RXSTALL |
                    BITM_USB_EP0I_CSR_H_NAKTO   |
                    BITM_USB_EP0I_CSR_H_TOERR))
    {
        ErrEvent = adi_usbdrv_HandleEp0Errors(pUsbDrvData, pEpInfo, RegCSR0);

        pEpCurURB = pEpInfo->pEpCurURB;
        assert(pEpCurURB != NULL);

        adi_usbdrv_ResetCurrentURB(pEpInfo);
        pRegs->INDEX = SavedEpNum;
        ADI_USB_EXIT_CR();

        pUsbDrvData->pfEpZeroCallback(pEpCurURB, ErrEvent, 0);
        return;
    }

    /* For EP0, both RX and TX interrupts comes as TX interrupt.
     * Inquire RXPKTRDY bit to see if its RX or TX interrupt
     */
    if ((pEpInfo->EpDir == ADI_USB_EP_DIR_RX) && 
       (ADI_USB_IF_BIT_SET(pRegs->EP0_CSR0, ENUM_USB_EP0I_CSR_H_RXPKTRDY)))
    {
        ADI_USB_EP_STATINC(pUsbDrvData,pEpInfo->EpPhyIdx,nRxInts);
        adi_usbdrv_RcvDataFromFifo(pUsbDrvData, pEpInfo, &SavedEpNum);
    }
    else
    {
        ADI_USB_EP_STATINC(pUsbDrvData,pEpInfo->EpPhyIdx,nTxInts);
        /* Check if DMA transfer is going on the FIFO */
        if(!pEpInfo->EpTxDmaPend)
        {
            adi_usbdrv_ProcessEp0TxData(pUsbDrvData, pEpInfo, &SavedEpNum);
        }
        else
        {
            ADI_USB_EP_STATINC(pUsbDrvData,pEpInfo->EpPhyIdx,nFalseTxInts);
        }
    }

    pRegs->INDEX = SavedEpNum;
    ADI_USB_EXIT_CR();
}
#else
static void adi_usbdrv_HandleSetupPacket(ADI_MUSBMHDRC_DATA *pUsbDrvData, ADI_USB_EP_INFO *pEpInfo, uint16_t *SavedEpNum)
{
    volatile ADI_USB_REGS_DEF *pRegs;
    pRegs = pUsbDrvData->pRegs;

    if (pRegs->EPI_RXCNT != 0 )
    {
        assert(pRegs->EPI_RXCNT == 8);
        ADI_USB_EP_STATINC(pUsbDrvData,pEpInfo->EpPhyIdx,nSetupInts);
        pUsbDrvData->EpZeroCurURB.ProcessedBytes = 0;
        pUsbDrvData->EpZeroCurURB.TransferLen    = ADI_USB_SETUP_PKT_LEN;
        pUsbDrvData->EpZeroCurURB.pData          = &pUsbDrvData->EpZeroSetupPkt[0];
        pUsbDrvData->EpZeroCurURB.EpDir          = ADI_USB_EP_DIR_SETUP;
        pUsbDrvData->EpZeroSetupState            = ADI_USB_SETUP_REQ;
        pUsbDrvData->EpZeroSetupStatus           = ADI_USB_SS_IDLE;
        pUsbDrvData->EpZeroIntAcked              = false;

        pEpInfo->EpTransferMode = ADI_USB_EP_MODE_DMA_0;
        pEpInfo->pEpBufferdata  = pUsbDrvData->EpZeroCurURB.pData;
        pEpInfo->pEpCurURB      = &pUsbDrvData->EpZeroCurURB;
        pEpInfo->EpDir          = ADI_USB_EP_DIR_RX;

        if(pUsbDrvData->CacheOn)
        {
            adi_usbdrv_FlushCachedBuffer(pEpInfo->pEpBufferdata, pEpInfo->pEpCurURB->TransferLen, pEpInfo->EpDir);
            SSYNC();
        }

        adi_usbdrv_RcvDataFromFifo(pUsbDrvData, pEpInfo, SavedEpNum);
    }
#if defined(ADI_DEBUG)
    else
    {   /* zero length packet in setup idle phase we expect setup packet */
        ADI_USB_DBG_CNTRINC(pUsbDrvData,dbg_ep0_setidle_rxcnt0); 
    }
#endif
}


static void adi_usbdrv_SetupDataHandler(ADI_MUSBMHDRC_DATA *pUsbDrvData,ADI_USB_EP_INFO *pEpInfo)
{
    uint16_t SavedEpNum;
    volatile ADI_USB_REGS_DEF *pRegs;
    pRegs = pUsbDrvData->pRegs;

    /* For EP0, both RX and TX interrupts comes as TX interrupt.
       Inquire RXPKTRDY bit to see if its RX or TX interrupt
     */
     if(pEpInfo->pEpCurURB)
     {
         if(pEpInfo->EpDir == ADI_USB_EP_DIR_TX)
         {
             ADI_USB_EP_STATINC(pUsbDrvData,pEpInfo->EpPhyIdx,nTxInts);

             /* Check if DMA transfer is going on the FIFO */
             if(!pEpInfo->EpTxDmaPend)
             {
                 adi_usbdrv_ProcessEp0TxData(pUsbDrvData, pEpInfo, &SavedEpNum);
             }
             else
             {
                 ADI_USB_EP_STATINC(pUsbDrvData,pEpInfo->EpPhyIdx,nFalseTxInts);
             }
         }

         if(ADI_USB_IF_BIT_SET(pRegs->EP0_CSR0, ENUM_USB_EP0I_CSR_H_RXPKTRDY))
         {
             if(pEpInfo->EpDir == ADI_USB_EP_DIR_RX)
             {
                 ADI_USB_EP_STATINC(pUsbDrvData,pEpInfo->EpPhyIdx,nRxInts);

                 adi_usbdrv_RcvDataFromFifo(pUsbDrvData, pEpInfo, &SavedEpNum);
             }
         }
     }
#if defined(ADI_DEBUG)
     else
     {
        ADI_USB_DBG_CNTRINC(pUsbDrvData,dbg_ep0_setdata_urbnull);

        if(ADI_USB_IF_BIT_SET(pRegs->EP0_CSR0, ENUM_USB_EP0I_CSR_H_RXPKTRDY))
        {
            pUsbDrvData->EpZeroIntAcked = true;
            pUsbDrvData->UsbDebugInfo.dbg_ep0_setdata_rxcount = pRegs->EPI_RXCNT;
        }
     }
#endif
}

static void adi_usbdrvd_Ep0Handler(ADI_USB_HANDLE const hDevice)
{
    ADI_MUSBMHDRC_DATA        *pUsbDrvData = (ADI_MUSBMHDRC_DATA *)hDevice;
    volatile ADI_USB_REGS_DEF *pRegs;
    uint16_t                  SavedEpNum, RegCSR0, ErrEvent;
    ADI_USB_EP_INFO           *pEpInfo = NULL;

    ADI_USB_ENTER_CR();
    pRegs = pUsbDrvData->pRegs;

    pEpInfo =  adi_usbdrv_GetEpInfo(pUsbDrvData, ADI_USB_EP_NUM_0);
    assert(pEpInfo != NULL);

    SavedEpNum   = pRegs->INDEX;
    pRegs->INDEX = pEpInfo->EpPhyNum;

    #if defined(ADI_CFG_USB_BF_MUSBHDRC)
    /* clear the interrupt bit */
    pRegs->INTRTX = (1u << pEpInfo->EpPhyNum);
    #endif

    RegCSR0 = pRegs->EP0_CSR0;

    if((RegCSR0 & BITM_USB_EP0I_CSR_P_SENTSTALL) | (RegCSR0 & BITM_USB_EP0I_CSR_P_SETUPEND))
    {
       ErrEvent = adi_usbdrv_HandleEp0Errors(pUsbDrvData, pEpInfo, RegCSR0);
       if(ErrEvent == ADI_USB_RESULT_EP_STALL)
       {
           pRegs->INDEX = SavedEpNum;
           ADI_USB_EXIT_CR();
           return;
       }
    }

    switch (pUsbDrvData->EpZeroSetupState)
    {
        case ADI_USB_SETUP_IDLE:
        {
            adi_usbdrv_HandleSetupPacket(pUsbDrvData, pEpInfo, &SavedEpNum);
        }
        break;

        case ADI_USB_SETUP_REQ:
        ADI_USB_DBG_CNTRINC(pUsbDrvData,dbg_ep0_setreq);
        break;

        case ADI_USB_SETUP_DATA:
        {
            adi_usbdrv_SetupDataHandler(pUsbDrvData, pEpInfo);
        }
        break;

        case ADI_USB_SETUP_STATUS:
        ADI_USB_DBG_CNTRINC(pUsbDrvData,dbg_ep0_setstatus);
        break;
    }
    pRegs->INDEX = SavedEpNum;
    ADI_USB_EXIT_CR();
}
#endif

static void adi_usbdrv_TxDataHandler(ADI_MUSBMHDRC_DATA *pUsbDrvData, const uint16_t TxInterrupts)
{
    uint32_t                  index;
    ADI_USB_EP_INFO           *pEpInfo = NULL;
    volatile ADI_USB_REGS_DEF *pRegs;
    uint16_t                  RegTxE;

    pRegs = pUsbDrvData->pRegs;
    RegTxE = pRegs->INTRTXE;

    for ( index = ADI_USB_EP_NUM_1; index < ADI_CFG_USB_MAX_NUM_EP; index++)
    {
        if((ADI_USB_IF_BIT_SET(TxInterrupts,(1u << index))) && (ADI_USB_IF_BIT_SET(RegTxE,(1u << index))))
        {
            pEpInfo =  adi_usbdrv_GetEpInfoFromEpPhy(pUsbDrvData, index, ADI_USB_EP_DIR_TX);
            assert(pEpInfo != NULL);
            ADI_USB_EP_STATINC(pUsbDrvData,pEpInfo->EpPhyIdx,nTxInts);

            adi_usbdrv_ProcessTxData(pUsbDrvData, pEpInfo);
        }
    }
}


static void adi_usbdrv_RxDataHandler(ADI_MUSBMHDRC_DATA *pUsbDrvData, const uint16_t RxInterrupts)
{
    uint32_t                  index;
    ADI_USB_EP_INFO           *pEpInfo = NULL;
    volatile ADI_USB_REGS_DEF *pRegs;
    uint16_t                  RegRxE;

    pRegs = pUsbDrvData->pRegs;
    RegRxE = pRegs->INTRRXE;

    for (index = ADI_USB_EP_NUM_1; index < ADI_CFG_USB_MAX_NUM_EP; index++ )
    {
        if ((ADI_USB_IF_BIT_SET(RxInterrupts,(1u << index))) && (ADI_USB_IF_BIT_SET(RegRxE,(1u << index))))
        {
            pEpInfo =  adi_usbdrv_GetEpInfoFromEpPhy(pUsbDrvData, index, ADI_USB_EP_DIR_RX);
            assert(pEpInfo != NULL);
            ADI_USB_EP_STATINC(pUsbDrvData,pEpInfo->EpPhyIdx,nRxInts);

            adi_usbdrv_ProcessRxData(pUsbDrvData, pEpInfo);
        }
    }
}

/* Handles EP0 control transfers */
static void adi_usbdrv_ProcessEp0TxData(ADI_MUSBMHDRC_DATA *pUsbDrvData, ADI_USB_EP_INFO  *pEpInfo, uint16_t *SavedEpNum)
{
    volatile ADI_USB_REGS_DEF *pRegs = pUsbDrvData->pRegs;

    if(pEpInfo->pEpCurURB->ProcessedBytes == pEpInfo->pEpCurURB->TransferLen)
    {
        pRegs->INDEX = *SavedEpNum;
#if (ADI_CFG_USB_DEVICE == 1)
        //pUsbDrvData->EpZeroSetupState = ADI_USB_SETUP_STATUS;
#endif
        ADI_USB_EXIT_CR(); /* ADI_USB_ENTER_CR is already called by caller */
        assert(pEpInfo->pEpCurURB != NULL);
        pUsbDrvData->pfEpZeroCallback(pEpInfo->pEpCurURB, ADI_USB_RESULT_URB_COMPLETE, 0);
        ADI_USB_ENTER_CR();

        adi_usbdrv_ResetCurrentURB(pEpInfo);
        /* Restore back the previous state */
        *SavedEpNum = pRegs->INDEX;
        pRegs->INDEX = pEpInfo->EpPhyNum;
    }
    else
    {
        pEpInfo->EpProcBytesThisPass = adi_usbdrv_BytesThisPass(pEpInfo);
        adi_usbdrv_ScheduleTransfer(pUsbDrvData, pEpInfo);
    }
}

/* Handles Data EPs TX transfers */
static void adi_usbdrv_ProcessTxData(ADI_MUSBMHDRC_DATA *pUsbDrvData, ADI_USB_EP_INFO *pEpInfo)
{
    volatile ADI_USB_REGS_DEF    *pRegs;
    ADI_USB_URB                  *pEpCurURB=NULL;
    uint16_t                     SavedEpNum, RegTXCSR;
    ADI_SUP_UNUSED_WARN uint16_t ErrEvent=0;

    pRegs = pUsbDrvData->pRegs;

    ADI_USB_ENTER_CR();
    SavedEpNum=pRegs->INDEX;
    pRegs->INDEX = pEpInfo->EpPhyNum;

    /* Clear the TX interrupt */
#if defined(ADI_CFG_USB_BF_MUSBHDRC)
    pRegs->INTRTX = (1u << pEpInfo->EpPhyNum);
#endif

    do /* This loop will run only once */
    {
        RegTXCSR = pRegs->EPI_TXCSR;
        /* Check TX Errors*/
#if (ADI_CFG_USB_OTG_HOST == 1)
        if(RegTXCSR & (BITM_USB_EPI_TXCSR_H_RXSTALL    |
                       BITM_USB_EPI_TXCSR_H_NAKTOINCMP |
                       BITM_USB_EPI_TXCSR_H_TXTOERR))
#else
        if(RegTXCSR & BITM_USB_EPI_TXCSR_P_SENTSTALL)
#endif
        {
            ErrEvent = adi_usbdrv_HandleTxErrors(pUsbDrvData, pEpInfo, RegTXCSR);
#if (ADI_CFG_USB_OTG_HOST == 1)
            pRegs->INTRTXE &= ~(1u << pEpInfo->EpPhyNum);
            pEpCurURB = pEpInfo->pEpCurURB;
            assert(pEpCurURB != NULL);
            adi_usbdrv_ResetCurrentURB(pEpInfo);
#if defined(ADI_ADUCM350)
            adi_usbdrv_FreeDmaChannel(pUsbDrvData, pEpInfo);
#endif             
            break;
#endif
        }

        if((pEpInfo->EpTxDmaPend) || (pEpInfo->pEpCurURB == NULL))
        {
#if defined(ADI_DEBUG)
            if(!(pEpInfo->pEpCurURB->TransferLen%512) && (pEpInfo->EpTransferMode == ADI_USB_EP_MODE_DMA_1))
            {
                ADI_USB_EP_STATINC(pUsbDrvData,pEpInfo->EpPhyIdx,nFalseTxDmaMode1Ints);
            }
#endif
            ADI_USB_EP_STATINC(pUsbDrvData,pEpInfo->EpPhyIdx,nFalseTxInts);
            break;
        }

        if(pEpInfo->pEpCurURB->ProcessedBytes < pEpInfo->pEpCurURB->TransferLen)
        {
            pEpInfo->EpProcBytesThisPass = adi_usbdrv_BytesThisPass(pEpInfo);
            adi_usbdrv_ScheduleTransfer(pUsbDrvData, pEpInfo);
        }
        else
        {
            pRegs->INTRTXE &= ~(1u << pEpInfo->EpPhyNum);
            ADI_USB_EP_STATINC(pUsbDrvData,pEpInfo->EpPhyIdx,nTxInts);

            pEpCurURB = pEpInfo->pEpCurURB;
            assert(pEpCurURB != NULL);
            adi_usbdrv_ResetCurrentURB(pEpInfo);
#if defined(ADI_ADUCM350)
            adi_usbdrv_FreeDmaChannel(pUsbDrvData, pEpInfo);
#endif             

            pRegs->INDEX = SavedEpNum;
            ADI_USB_EXIT_CR();
            pUsbDrvData->pfEpDataCallback(pEpCurURB, ADI_USB_RESULT_URB_COMPLETE, 0);
            return;
        }

    } while(0);

    pRegs->INDEX = SavedEpNum;
    ADI_USB_EXIT_CR();

#if (ADI_CFG_USB_OTG_HOST == 1)
    if(ErrEvent)
    {
        /* Report TX Error */
        pUsbDrvData->pfEpDataCallback(pEpCurURB, ErrEvent, 0);
    }
#endif
}

/* Handles Data EPs RX transfers */
static void adi_usbdrv_ProcessRxData(ADI_MUSBMHDRC_DATA *pUsbDrvData, ADI_USB_EP_INFO *pEpInfo)
{
    volatile ADI_USB_REGS_DEF *pRegs;
    ADI_USB_URB               *pEpCurURB=NULL;
    uint16_t                  SavedEpNum, RegRXCSR;
    uint16_t                  ErrEvent=0;

    pRegs = pUsbDrvData->pRegs;

    ADI_USB_ENTER_CR();
    SavedEpNum=pRegs->INDEX;
    pRegs->INDEX = pEpInfo->EpPhyNum;

    /* Disable and clear the RX interrupt */
    pRegs->INTRRXE &= ~(0x1 << (pEpInfo->EpPhyNum));

#if defined(ADI_CFG_USB_BF_MUSBHDRC)
    pRegs->INTRRX = (0x1 << (pEpInfo->EpPhyNum));
#endif

    /* Check RX Errors*/
    RegRXCSR = pRegs->EPI_RXCSR;
#if (ADI_CFG_USB_OTG_HOST == 1)
    if(RegRXCSR & (BITM_USB_EPI_RXCSR_H_RXSTALL    |
                   BITM_USB_EPI_RXCSR_H_NAKTODERR  |
                   BITM_USB_EPI_RXCSR_H_RXTOERR))
#else
    if(RegRXCSR & BITM_USB_EPI_RXCSR_P_DATAERR)
#endif
    {
     	 if (pEpInfo->EpType == ADI_USB_EP_TYPE_ISOC)
       	 {
       		 adi_usbdrv_FlushRxFifo(pRegs);
       		 pRegs->INTRRXE |= (0x1 << (pEpInfo->EpPhyNum));
       		 pRegs->INDEX = SavedEpNum;
       		 ADI_USB_EXIT_CR();
       		 return;
       	 }

        ErrEvent = adi_usbdrv_HandleRxErrors(pUsbDrvData, pEpInfo, RegRXCSR);

        pEpCurURB = pEpInfo->pEpCurURB;
        assert(pEpCurURB != NULL);
        adi_usbdrv_ResetCurrentURB(pEpInfo);

        pRegs->INDEX = SavedEpNum;
        ADI_USB_EXIT_CR();

        /* Report RX Error */
        pUsbDrvData->pfEpDataCallback(pEpCurURB, ErrEvent, 0);
    }
    else
    {
        adi_usbdrv_RcvDataFromFifo(pUsbDrvData, pEpInfo, &SavedEpNum);
    }

    pRegs->INDEX = SavedEpNum;
    ADI_USB_EXIT_CR();
}


static void adi_usbdrv_RcvDataFromFifo(ADI_MUSBMHDRC_DATA *pUsbDrvData, ADI_USB_EP_INFO *pEpInfo, uint16_t *SavedEpNum)
{
    uint32_t                  RxCount=0;
    volatile ADI_USB_REGS_DEF *pRegs;

    pRegs = pUsbDrvData->pRegs;

#if (ADI_CFG_USB_ENABLE_DMA_MODE1 == 1)
    if(pEpInfo->EpTransferMode == ADI_USB_EP_MODE_DMA_1)
    {
        /* DMA Request mode 0 is used while employing DMA mode 1.
         * RX interrupts are not supposed to come so just return
         * This interrupt might be because of the data already residing in FIFO while scheduling DMA mode 1
         */
        ADI_USB_EP_STATINC(pUsbDrvData,pEpInfo->EpPhyIdx,nFalseRxInts);
        return;
    }
#endif

    RxCount = pRegs->EPI_RXCNT;
    if(RxCount)
    {
        assert(pEpInfo->pEpCurURB != NULL);
        assert(pEpInfo->EpTransferMode == ADI_USB_EP_MODE_DMA_0);

        pEpInfo->EpUrbState          = ADI_USB_URB_STATE_PROCESSING;
        pEpInfo->EpProcBytesThisPass = RxCount;

        adi_usbdrv_ScheduleTransfer(pUsbDrvData, pEpInfo);
    }
    else
    {
        if (pEpInfo->EpType == ADI_USB_EP_TYPE_ISOC)
        {
            return;
        }

        /* We are not suppose to receive ZLPs for data EPs */
        assert(pEpInfo->EpPhyNum == ADI_USB_EP_NUM_0);

#if (ADI_CFG_USB_OTG_HOST == 1)
        assert(pEpInfo->pEpCurURB != NULL);

        /* IN Status phase ZLP
           This is required only in the case of host as host expects the Status ZLP from device
         */
        adi_usbdrv_ProcessRxZLP(pUsbDrvData, pEpInfo, SavedEpNum);
        pRegs->INDEX = *SavedEpNum;

        ADI_USB_EXIT_CR(); /* ADI_USB_ENTER_CR is already called by caller */
        pUsbDrvData->pfEpZeroCallback(pEpInfo->pEpCurURB, ADI_USB_RESULT_URB_COMPLETE, 0);
        ADI_USB_ENTER_CR();

        adi_usbdrv_ResetCurrentURB(pEpInfo);
        /* Restore back the previous state */
        *SavedEpNum = pRegs->INDEX;
        pRegs->INDEX = pEpInfo->EpPhyNum;
#else
        /* We are not suppose to receive ZLPs on EP0 when in device mode
         * Log the ERROR */
        ADI_USB_EP_STATINC(pUsbDrvData, pEpInfo->EpPhyNum ,nEp0DeviceZLP);
#endif

    }
}

static void adi_usbdrv_Ep0DmaHandler(ADI_MUSBMHDRC_DATA *pUsbDrvData, ADI_USB_EP_INFO *pEpInfo)
{
    volatile ADI_USB_REGS_DEF *pRegs;
    uint16_t                  SavedEpNum;

#if (ADI_CFG_USB_DEVICE == 1)
    USB_SETUP_REQ             *pSetupPkt=NULL;
#if defined(ADI_DEBUG)
    ADI_USB_DBG_INFO     *pUsbDbgInfo;
    uint32_t              SetupIndex;
    uint8_t              *pDbgSetupPkt;
#endif
#endif

    pRegs = pUsbDrvData->pRegs;

    SavedEpNum = pRegs->INDEX;
    pRegs->INDEX = pEpInfo->EpPhyNum;

    if(pEpInfo->EpDir == ADI_USB_EP_DIR_RX)
    {
#if (ADI_CFG_USB_OTG_HOST == 1)
        /* Clear RxPktRy bit */
        pRegs->EP0_CSR0 &= ~ENUM_USB_EP0I_CSR_H_RXPKTRDY;
#else
        if (pUsbDrvData->EpZeroSetupState == ADI_USB_SETUP_REQ)
        {
            assert(pEpInfo->pEpCurURB->TransferLen == 8);
            pSetupPkt  = (USB_SETUP_REQ *)pUsbDrvData->EpZeroSetupPkt;

#if defined(ADI_DEBUG)
            pUsbDbgInfo   = &pUsbDrvData->UsbDebugInfo;
            SetupIndex    = pUsbDbgInfo->dbg_setup_index;
            pDbgSetupPkt  = (uint8_t*)&pUsbDbgInfo->dbg_setup_pkts[SetupIndex].SetupPkt[0];
            memcpy(pDbgSetupPkt,pSetupPkt, ADI_USB_SETUP_PKT_LEN);
            SetupIndex++;
            pUsbDbgInfo->dbg_setup_index = (SetupIndex >= ADI_USB_DBG_MAX_SETUPPKTS) ? 0 : SetupIndex;
#endif

            pUsbDrvData->EpZeroSetupState = (pSetupPkt->wLength > 0 ) ? ADI_USB_SETUP_DATA : ADI_USB_SETUP_STATUS;
            pEpInfo->EpDir = (pSetupPkt->bmRequestType & 0x80) ? ADI_USB_EP_DIR_TX : ADI_USB_EP_DIR_RX;

            /* Disable EP0 interrupt and Clear RxPktRdy bit if there is a data Phase */
            if (pUsbDrvData->EpZeroSetupState == ADI_USB_SETUP_DATA)
            {
                pUsbDrvData->EpZeroSetupStatus = ADI_USB_SS_PENDING_DATA;
                pRegs->EP0_CSR0               |= ENUM_USB_EP0I_CSR_P_SPKTRDY;
            }
            else
            {
                /* No data phase. ACK and Set Data end simultaneously.
                 * We are effectively transitioned to status phase and next setup packet interrupt might come
                 * At this point the incoming IN token will be acked with zero length data packet. Stack
                 * eventually issues TXZLP and we have to handle if we miss any setup packet in between.
                 * for set address (0x5) we ack the setup and end the data phase right away for timely operation.
                 *
                 * NO data phase  - TXZLP expected - IN transfer.
                 */
                if ((pSetupPkt->bRequest == 0x05) && (pSetupPkt->bmRequestType == 0x0))
                {
                    pRegs->EP0_CSR0 |= (ENUM_USB_EP0I_CSR_P_SPKTRDY | ENUM_USB_EP0I_CSR_P_DATAEND);
                }
                else
                {
                    pUsbDrvData->EpZeroSetupStatus = ADI_USB_SS_PENDING_TX_NDP_ZLP;
                }
            }
        }
#endif
        if(pEpInfo->pEpCurURB->ProcessedBytes == pEpInfo->pEpCurURB->TransferLen)
        {
#if (ADI_CFG_USB_OTG_HOST == 1)
            pRegs->EP0_CSR0 &= ~ENUM_USB_EP0I_CSR_H_REQPKT;
#else
            if(!pSetupPkt && (pUsbDrvData->EpZeroSetupState == ADI_USB_SETUP_DATA))
            {
                /* We received all data. Now a zero length packet would end the transfer.Stack issues TxZLP
                 * we are already in the status phase so we will update the state and defer the processing for TXZLP
                 */
                 pUsbDrvData->EpZeroSetupState = ADI_USB_SETUP_STATUS;
                 pRegs->EP0_CSR0 |= (ENUM_USB_EP0I_CSR_P_SPKTRDY | ENUM_USB_EP0I_CSR_P_DATAEND);
            }
#endif
            pRegs->INDEX = SavedEpNum;

            // ADI_USB_EXIT_CR(); /* ADI_USB_ENTER_CR is already called by caller */
            assert(pEpInfo->pEpCurURB != NULL);
            //pEpInfo->EpUrbState = ADI_USB_URB_STATE_FREE;
            pUsbDrvData->pfEpZeroCallback(pEpInfo->pEpCurURB, ADI_USB_RESULT_URB_COMPLETE, pEpInfo->pEpCurURB->pData);
            //ADI_USB_ENTER_CR();

            adi_usbdrv_ResetCurrentURB(pEpInfo);
            return;
        }
        else
        {
#if (ADI_CFG_USB_OTG_HOST == 1)
            pRegs->EP0_CSR0 |= ENUM_USB_EP0I_CSR_H_REQPKT;
#else
            /* Clear RxPktRy bit */
            pRegs->EP0_CSR0 |= (ENUM_USB_EP0I_CSR_P_SPKTRDY);
#endif
        }
    }
    else
    {
        pEpInfo->EpTxDmaPend = false;
#if (ADI_CFG_USB_OTG_HOST == 1)
        pRegs->EP0_CSR0 |= (pEpInfo->EpDir == ADI_USB_EP_DIR_SETUP)                    ?
                           (ENUM_USB_EP0I_CSR_H_TXPKTRDY | ENUM_USB_EP0I_CSR_H_SETUPPKT):
                            ENUM_USB_EP0I_CSR_H_TXPKTRDY;
#else
        if(pEpInfo->pEpCurURB->ProcessedBytes == pEpInfo->EpMaxSize)
        {
            pRegs->EP0_CSR0 |= ENUM_USB_EP0I_CSR_P_TXPKTRDY;
        }
        else
        {
            /* We are transmitting the last packet. Now set tx pkt rdy and data endbit. stack
             * will issue a RXZLP to fetch the zero length packet. We move to status phase and
             * keep track of any incoming setup packets to RxZLP to process.
             *
             * TX operation - RXZLP - OUT transfer.
             */
            adi_usbdrv_ProcessEp0TxData(pUsbDrvData, pEpInfo, &SavedEpNum);
            pUsbDrvData->EpZeroSetupState = ADI_USB_SETUP_STATUS;
            pUsbDrvData->EpZeroSetupStatus = ADI_USB_SS_PENDING_RX_DP_ZLP;
        }
#endif
    }

    pRegs->INDEX = SavedEpNum;
}


static void adi_usbdrv_DataDmaHandler(ADI_MUSBMHDRC_DATA *pUsbDrvData, ADI_USB_EP_INFO *pEpInfo)
{
    volatile ADI_USB_REGS_DEF       *pRegs;
    uint16_t                        SavedEpNum;
    ADI_SUP_UNREF_WARN ADI_USB_URB  *pEpCurURB=NULL;
    
    pRegs = pUsbDrvData->pRegs;

    SavedEpNum = pRegs->INDEX;
    pRegs->INDEX = pEpInfo->EpPhyNum;

    adi_usbdrv_UpdateCSRRegisters(pUsbDrvData, pEpInfo);

    if(pEpInfo->EpDir == ADI_USB_EP_DIR_RX)
    {
        if((pEpInfo->pEpCurURB->ProcessedBytes == pEpInfo->pEpCurURB->TransferLen) ||
           (pEpInfo->EpProcBytesThisPass < pEpInfo->EpMaxSize))
        {
            pRegs->INDEX = SavedEpNum;

            ADI_USB_EXIT_CR(); /* ADI_USB_ENTER_CR is already called by caller */
            assert(pEpInfo->pEpCurURB != NULL);
            pUsbDrvData->pfEpDataCallback(pEpInfo->pEpCurURB, ADI_USB_RESULT_URB_COMPLETE, 0);
            ADI_USB_ENTER_CR();

            adi_usbdrv_ResetCurrentURB(pEpInfo);
#if defined(ADI_ADUCM350)
            adi_usbdrv_FreeDmaChannel(pUsbDrvData, pEpInfo);
#endif            
            return;
        }
        else
        {
#if (ADI_CFG_USB_OTG_HOST == 1)
            pRegs->EPI_RXCSR |= ENUM_USB_EPI_RXCSR_H_REQPKT;
#endif
            pEpInfo->EpUrbState = ADI_USB_URB_STATE_FREE;
            pRegs->INTRRXE |= (1 << pEpInfo->EpPhyNum);
        }
    }
    else
    {
        pEpInfo->EpTxDmaPend = false;

#if (ADI_CFG_USB_ENABLE_DMA_MODE1 == 1)
        /*
         * if transfer mode is DMA mode 1 and the transfer length is exact multiple of maxep size we return the packet
         */
        if((pEpInfo->EpTransferMode == ADI_USB_EP_MODE_DMA_1) && !(pEpInfo->pEpCurURB->TransferLen % pEpInfo->EpMaxSize))
        {
            pRegs->INTRTXE &= ~(1u << pEpInfo->EpPhyNum);

            pEpCurURB = pEpInfo->pEpCurURB;
            assert(pEpCurURB != NULL);
            adi_usbdrv_ResetCurrentURB(pEpInfo);
#if defined(ADI_ADUCM350)
            adi_usbdrv_FreeDmaChannel(pUsbDrvData, pEpInfo);
#endif  
            pRegs->INDEX = SavedEpNum;
            ADI_USB_EXIT_CR();
            pUsbDrvData->pfEpDataCallback(pEpCurURB, ADI_USB_RESULT_URB_COMPLETE, 0);
            return;
        }
#endif

    }
    pRegs->INDEX = SavedEpNum;
}


static uint16_t  adi_usbdrv_GetEpPhyAddrUsedByDma(ADI_MUSBMHDRC_DATA *pUsbDrvData, uint32_t DmaChnlIdx)
{
    volatile ADI_USB_REGS_DEF *pRegs = pUsbDrvData->pRegs;
    uint32_t DmaDir = 0;

    if (DmaChnlIdx == 0 ) { return 0; } 

    DmaDir = ((pRegs->DmaRegs[DmaChnlIdx].DMA_CTL & 0x02) >> 1);

#if (ADI_CFG_USB_OTG_HOST == 1)
    return (((!DmaDir) << 7) | ((pRegs->DmaRegs[DmaChnlIdx].DMA_CTL & 0xF0) >> 4));
#else
    return (((DmaDir) << 7) | ((pRegs->DmaRegs[DmaChnlIdx].DMA_CTL & 0xF0) >> 4));
#endif
}

#if (ADI_CFG_USB_OTG_HOST == 1)
static uint16_t adi_usbdrv_HandleEp0Errors(ADI_MUSBMHDRC_DATA *pUsbDrvData, ADI_USB_EP_INFO *pEpInfo, uint16_t RegCSR0)
{
    volatile ADI_USB_REGS_DEF *pRegs;
    uint16_t ErrEvent=ADI_USB_RESULT_FAIL;

    pRegs = pUsbDrvData->pRegs;

    if(ADI_USB_IF_BIT_SET(RegCSR0, BITM_USB_EP0I_CSR_H_RXSTALL))
    {
        ADI_USB_EP_STATINC(pUsbDrvData,0,nEpStall);

        pRegs->EP0_CSR0 &= ~ENUM_USB_EP0I_CSR_H_RXSTALL;

        /* clear the data toggle bit */
        pRegs->EP0_CSR0 |= ENUM_USB_EP0I_CSR_H_DATATGL;

        ErrEvent = ADI_USB_RESULT_EP_STALL;
    }
    else if(ADI_USB_IF_BIT_SET(RegCSR0, BITM_USB_EP0I_CSR_H_NAKTO))
    {
        ADI_USB_EP_STATINC(pUsbDrvData,0,nEpNack);

        /* Abort the transaction by flushing the FIFO before clearing the NAK Timeout bit */
        adi_usbdrv_FlushEp0Fifo(pRegs);

        pRegs->EP0_CSR0 &= ~ENUM_USB_EP0I_CSR_H_NAKTO;

        ErrEvent = ADI_USB_RESULT_EP_NAK;
    }
    else if(ADI_USB_IF_BIT_SET(RegCSR0, BITM_USB_EP0I_CSR_H_TOERR))
    {
        ADI_USB_EP_STATINC(pUsbDrvData,0,nEpError);

        pRegs->EP0_CSR0 &= ~ENUM_USB_EP0I_CSR_H_TOERR;
        /* clear the data toggle bit */
        pRegs->EP0_CSR0 |= ENUM_USB_EP0I_CSR_H_DATATGL;

        ErrEvent = ADI_USB_RESULT_EP_ERROR;
    }

    pEpInfo->EpIsHalt = true;

    pRegs->EP0_CSR0 &= ~ENUM_USB_EP0I_CSR_H_REQPKT;

    return ErrEvent;
}


static uint16_t adi_usbdrv_HandleRxErrors(ADI_MUSBMHDRC_DATA *pUsbDrvData, ADI_USB_EP_INFO *pEpInfo, uint16_t RegRXCSR)
{
    volatile ADI_USB_REGS_DEF *pRegs;
    uint16_t ErrEvent=ADI_USB_RESULT_FAIL;

    pRegs = pUsbDrvData->pRegs;

    if(ADI_USB_IF_BIT_SET(RegRXCSR, BITM_USB_EPI_RXCSR_H_RXSTALL))
    {
        ADI_USB_EP_STATINC(pUsbDrvData,pEpInfo->EpPhyIdx,nEpStall);

        pRegs->EPI_RXCSR &= ~ENUM_USB_EPI_RXCSR_H_RXSTALL;

        /* clear the data toggle bit */
        pRegs->EPI_RXCSR |= ENUM_USB_EPI_RXCSR_H_CLRTGL;

        ErrEvent = ADI_USB_RESULT_EP_STALL;
    }
    else if(ADI_USB_IF_BIT_SET(RegRXCSR, BITM_USB_EPI_RXCSR_H_NAKTODERR))
    {
        ADI_USB_EP_STATINC(pUsbDrvData,pEpInfo->EpPhyIdx,nEpNack);

        /* Abort the transaction by clearing ReqPkt before clearing the NAK Timeout bit */
        pRegs->EPI_RXCSR &= ~ENUM_USB_EPI_RXCSR_H_NAKTO;

        ErrEvent = ADI_USB_RESULT_EP_NAK;
    }
    else if(ADI_USB_IF_BIT_SET(RegRXCSR, BITM_USB_EPI_RXCSR_H_RXTOERR))
    {
        ADI_USB_EP_STATINC(pUsbDrvData,pEpInfo->EpPhyIdx,nEpError);

        pRegs->EPI_RXCSR &= ~ENUM_USB_EPI_RXCSR_H_RXTOERR;

        /* clear the data toggle bit */
        pRegs->EPI_RXCSR |= ENUM_USB_EPI_RXCSR_H_CLRTGL;

        ErrEvent = ADI_USB_RESULT_EP_ERROR;
    }

    pRegs->EPI_RXCSR &= ~ENUM_USB_EPI_RXCSR_H_REQPKT;

    pEpInfo->EpIsHalt = true;

    return ErrEvent;
}

static uint16_t adi_usbdrv_HandleTxErrors(ADI_MUSBMHDRC_DATA *pUsbDrvData, ADI_USB_EP_INFO *pEpInfo, uint16_t RegTXCSR)
{
    volatile ADI_USB_REGS_DEF *pRegs;
    uint16_t ErrEvent=ADI_USB_RESULT_SUCCESS;

    pRegs = pUsbDrvData->pRegs;

    if(ADI_USB_IF_BIT_SET(RegTXCSR, BITM_USB_EPI_TXCSR_H_RXSTALL))
    {
        ADI_USB_EP_STATINC(pUsbDrvData,pEpInfo->EpPhyIdx,nEpStall);

        pRegs->EPI_TXCSR &= ~ENUM_USB_EPI_TXCSR_H_RXSTALL;

        /* clear the data toggle bit */
        pRegs->EPI_TXCSR |= ENUM_USB_EPI_TXCSR_H_CLRTGL;

        ErrEvent = ADI_USB_RESULT_EP_STALL;
    }
    else if(ADI_USB_IF_BIT_SET(RegTXCSR, BITM_USB_EPI_TXCSR_H_NAKTOINCMP))
    {
        ADI_USB_EP_STATINC(pUsbDrvData,pEpInfo->EpPhyIdx,nEpNack);

        /* Abort the transaction by flushing the FIFO before clearing the NAK Timeout bit */
        adi_usbdrv_FlushTxFifo(pRegs);

        pRegs->EPI_TXCSR &= ~ENUM_USB_EPI_TXCSR_H_NAKTO;

        ErrEvent = ADI_USB_RESULT_EP_NAK;
    }
    else if(ADI_USB_IF_BIT_SET(RegTXCSR, BITM_USB_EPI_TXCSR_H_TXTOERR))
    {
        ADI_USB_EP_STATINC(pUsbDrvData,pEpInfo->EpPhyIdx,nEpError);

        pRegs->EPI_TXCSR &= ~ENUM_USB_EPI_TXCSR_H_TXTOERR;

        /* clear the data toggle bit */
        pRegs->EPI_TXCSR |= ENUM_USB_EPI_TXCSR_H_CLRTGL;

        ErrEvent = ADI_USB_RESULT_EP_ERROR;
    }

    pEpInfo->EpIsHalt = true;

    return ErrEvent;
}
#else
static uint16_t adi_usbdrv_HandleEp0Errors(ADI_MUSBMHDRC_DATA *pUsbDrvData, ADI_USB_EP_INFO *pEpInfo, uint16_t RegCSR0)
{
    volatile ADI_USB_REGS_DEF *pRegs;
    uint16_t ErrEvent = ADI_USB_RESULT_SUCCESS;

    pRegs = pUsbDrvData->pRegs;

    if(ADI_USB_IF_BIT_SET(RegCSR0, BITM_USB_EP0I_CSR_P_SETUPEND))
    {
        ADI_USB_EP_STATINC(pUsbDrvData, pEpInfo->EpPhyIdx, nEpError); // FIXIT not an error add another counter

        pRegs->EP0_CSR0 |= ENUM_USB_EP0I_CSR_P_SSETUPEND;

        pUsbDrvData->EpZeroSetupStatus = ADI_USB_SS_SETUPEND;

        pUsbDrvData->EpZeroSetupState = ADI_USB_SETUP_IDLE;

        pRegs->INTRTXE |= (1u << 0);

        ADI_USB_DBG_CNTRINC(pUsbDrvData,dbg_ep0_setupend);
    }

    if(ADI_USB_IF_BIT_SET(RegCSR0, BITM_USB_EP0I_CSR_P_SENTSTALL))
    {
        ADI_USB_EP_STATINC(pUsbDrvData, pEpInfo->EpPhyIdx, nEpStall);

        pRegs->EP0_CSR0 &= ~ENUM_USB_EP0I_CSR_P_STALSNT;

        pEpInfo->EpIsHalt = true;

        pUsbDrvData->EpZeroSetupStatus = ADI_USB_SS_STALL;

        pUsbDrvData->EpZeroSetupState = ADI_USB_SETUP_IDLE;

        pRegs->INTRTXE |= (1u << 0);

        ErrEvent = ADI_USB_RESULT_EP_STALL;

        ADI_USB_DBG_CNTRINC(pUsbDrvData,dbg_ep0_stallsent);
    }

    return ErrEvent;
}


static uint16_t adi_usbdrv_HandleRxErrors(ADI_MUSBMHDRC_DATA *pUsbDrvData, ADI_USB_EP_INFO *pEpInfo, uint16_t RegRXCSR)
{
    volatile ADI_USB_REGS_DEF *pRegs;
    uint16_t ErrEvent;

    pRegs = pUsbDrvData->pRegs;

    if(ADI_USB_IF_BIT_SET(RegRXCSR, BITM_USB_EPI_RXCSR_P_DATAERR))
    {
        ADI_USB_EP_STATINC(pUsbDrvData,pEpInfo->EpPhyIdx,nEpError);

        /* This bit is cleared when RxPkyRdy bit is cleared */

        /* clear the data toggle bit */
        pRegs->EPI_RXCSR |= ENUM_USB_EPI_RXCSR_P_CLRTGL;

        ErrEvent = ADI_USB_RESULT_URB_COMPLETE;
        pEpInfo->pEpCurURB->EpResult = ADI_USB_RESULT_EP_ERROR;
    }

    pEpInfo->EpIsHalt = true;

    return ErrEvent;
}

static uint16_t adi_usbdrv_HandleTxErrors(ADI_MUSBMHDRC_DATA *pUsbDrvData, ADI_USB_EP_INFO *pEpInfo, uint16_t RegTXCSR)
{
    volatile ADI_USB_REGS_DEF *pRegs;
    uint16_t ErrEvent=ADI_USB_RESULT_SUCCESS;

    pRegs = pUsbDrvData->pRegs;

    if(ADI_USB_IF_BIT_SET(RegTXCSR, BITM_USB_EPI_TXCSR_P_SENTSTALL))
    {
        ADI_USB_EP_STATINC(pUsbDrvData,pEpInfo->EpPhyIdx,nEpError);

        pRegs->EPI_TXCSR &= ~ENUM_USB_EPI_TXCSR_P_STALSNT;

        /* clear the data toggle bit */
        pRegs->EPI_TXCSR |= ENUM_USB_EPI_TXCSR_H_CLRTGL;
    }

    return ErrEvent; /* Success as we have cleared the Stall. */
}
#endif

#if (ADI_CFG_USB_OTG_HOST == 1)
static ADI_USB_SPEED adi_usbdrv_GetDevSpeed(uint8_t Power)
{
    if(Power & BITM_USB_POWER_HSMODE)
    {
        return ADI_USB_SPD_HIGH;
    }
    else if(Power & BITM_USB_DEV_CTL_FSDEV)
    {
        return ADI_USB_SPD_FULL;
    }
    else
    {
        return ADI_USB_SPD_LOW;
    }
}
#endif

#if 0 /* Not used */
#if (ADI_CFG_USB_ENABLE_DMA_MODE1 == 1)
static uint32_t adi_usbdrv_ActualBytesProcessed(ADI_MUSBMHDRC_DATA *pUsbDrvData, ADI_USB_EP_INFO *pEpInfo)
{
#if defined(ADI_CFG_USB_BF_MUSBHDRC)
    volatile uint16_t EndAddress1=0;
    volatile uint16_t EndAddress2=0;
#endif
    uint32_t EndAddress=0;

    uint32_t StartAddress = (uint32_t)pEpInfo->pEpBufferdata;
#if defined(ADI_CFG_USB_BF_MUSBHDRC)
    EndAddress1 = pUsbDrvData->pRegs->DmaRegs[pEpInfo->EpDmaChannel].DMA_ADDRLOW;
    EndAddress2 = pUsbDrvData->pRegs->DmaRegs[pEpInfo->EpDmaChannel].DMA_ADDRHIGH;
    EndAddress   = (uint32_t)((uint32_t)(EndAddress2) << 16) + (EndAddress1);
#elif defined(ADI_CFG_USB_BF_MUSBMHDRC) || defined(ADI_CFG_USB_ARM_MUSBMHDRC)
    EndAddress = pUsbDrvData->pRegs->DmaRegs[pEpInfo->EpDmaChannel].DMA_ADDRHIGH;
#endif

    return (EndAddress - StartAddress);
}
#endif
#endif

static ADI_USB_RESULT adi_usbdrv_ScheduleTransfer(ADI_MUSBMHDRC_DATA *pUsbDrvData, ADI_USB_EP_INFO *pEpInfo)
{
    ADI_USB_RESULT Result;


    switch(pEpInfo->EpTransferMode)
    {
        case ADI_USB_EP_MODE_DMA_0:
#if (ADI_CFG_USB_ENABLE_DMA_MODE1 == 1)
        case ADI_USB_EP_MODE_DMA_1:
#endif
            Result = adi_usbdrv_ScheduleDma(pUsbDrvData, pEpInfo);
            break;
        case ADI_USB_EP_MODE_PIO:
            Result = ADI_USB_RESULT_FAIL;
            break;

        default:
            Result = ADI_USB_RESULT_FAIL;
            break;
    }

    return Result;
}


static void adi_usbdrv_UpdateCSRRegisters(ADI_MUSBMHDRC_DATA *pUsbDrvData, ADI_USB_EP_INFO *pEpInfo)
{
    volatile ADI_USB_REGS_DEF *pRegs;
    pRegs = pUsbDrvData->pRegs;

#if (ADI_CFG_USB_OTG_HOST == 1)
    if( (pEpInfo->pEpCurURB->TransferLen%pEpInfo->EpMaxSize == 0) && (pEpInfo->EpDir == ADI_USB_EP_DIR_RX) )
    {
        pRegs->EPI_RXCSR &=  ~(ENUM_USB_EPI_RXCSR_H_REQPKT);
    }
#endif
    switch(pEpInfo->EpTransferMode)
    {
        case ADI_USB_EP_MODE_DMA_0:
            if(pEpInfo->EpDir == ADI_USB_EP_DIR_RX)
            {
                /* Clear RxPktRy bit */
                pRegs->EPI_RXCSR &= ~ENUM_USB_EPI_RXCSR_H_PKTRDY;
            }
            else
            {
                pRegs->EPI_TXCSR |= ENUM_USB_EPI_TXCSR_H_PKTRDY;
            }
            break;

#if (ADI_CFG_USB_ENABLE_DMA_MODE1 == 1)
        case ADI_USB_EP_MODE_DMA_1:
            if(pEpInfo->EpDir == ADI_USB_EP_DIR_RX)
            {
                pRegs->INTRRXE &= ~(0x1 << (pEpInfo->EpPhyNum));

                /* If Short packet is received, RXPKTRDY bit need to be cleared by CPU */
                if(pEpInfo->pEpCurURB->ProcessedBytes % pEpInfo->EpMaxSize)
                {
                    pRegs->EPI_RXCSR &= ~(ENUM_USB_EPI_RXCSR_H_PKTRDY);
                }

                pRegs->EPI_RXCSR &= ~(ENUM_USB_EPI_RXCSR_H_AUTOCLR     |
                                      ENUM_USB_EPI_RXCSR_H_AUTOREQ     |
                                      ENUM_USB_EPI_RXCSR_H_DMAREQEN);
            }
            else
            {
                pRegs->EPI_TXCSR &= ~(ENUM_USB_EPI_TXCSR_P_AUTOSET      |
                                        ENUM_USB_EPI_TXCSR_H_DMAREQEN);
                /* DMA request mode must not be cleared either before or in the same cycle DMAReqEnab bit is cleared */
                pRegs->EPI_TXCSR &= ~(ENUM_USB_EPI_TXCSR_H_DMARQMODE1);

                if(pEpInfo->pEpCurURB->TransferLen % pEpInfo->EpMaxSize)
                {
                    pRegs->EPI_TXCSR |= ENUM_USB_EPI_TXCSR_H_PKTRDY;
                }
                else
                {
                    /* FIXIT: Need to consider the case when we need to send the ZLP */
                }
            }
            break;

        default:
        	break;
#endif
    }
}

#if !defined(ADI_CFG_USB_ARM_MUSBHDRC) /* Not required for ADUCM350 */
static void adi_usbdrv_SetSessionBit(ADI_MUSBMHDRC_DATA *pUsbDrvData)
{
     adi_usbdrv_WaitOneMillisec();
     /* drive VBUS in OTG host mode */

     /* Set up USB to listen on USB_ID */
     pUsbDrvData->pRegs->DEV_CTL |= 1;

     /* wait 1ms - USB 2.0 specification */
     adi_usbdrv_WaitOneMillisec();
}
#endif

#if (ADI_CFG_USB_OTG_HOST == 1)

#if defined(ADI_CFG_USB_BF_MUSBMHDRC)
static void adi_usbdrvh_ResetMultPointRegs(ADI_MUSBMHDRC_DATA *pUsbDrvData)
{
    volatile ADI_USB_REGS_DEF *pRegs = pUsbDrvData->pRegs;
    uint32_t i;

    for (i = 0; i < ADI_CFG_USB_MAX_HUB_PORTS; i++)
    {
        pRegs->MPRegs[i].MP_TXFUNCADDR = 0;
        pRegs->MPRegs[i].MP_RXFUNCADDR = 0;
    }

    pUsbDrvData->FifoRamAdress    = ADI_USB_DYNFIFO_START_ADDR; /* 64 */
    pUsbDrvData->FifoRamTotalSize = ADI_USB_DYNFIFO_TOTAL_SIZE; /* 16K */
}
#endif
static void adi_usbdrvh_PowerReset(ADI_MUSBMHDRC_DATA *pUsbDrvData)
{
    volatile ADI_USB_REGS_DEF *pRegs = pUsbDrvData->pRegs;

    /* perform the host mode reset sequence to enumerate the device */
    pRegs->POWER |= ENUM_USB_POWER_RESET;
    adi_usbdrv_WaitMilliSec(20);
    pRegs->POWER &= ~ENUM_USB_POWER_RESET;
}

/* suspend interrupt is generated when bus is idle for 3ms. This interrupt
 * is not generated when hdrc is acting as host. the presence of this 
 * interrupt indicates mini-a is not connected yet.
 */
static void adi_usbdrvh_SuspendHandler(ADI_USB_HANDLE const hDevice)
{
    ADI_MUSBMHDRC_DATA *pUsbDrvData = (ADI_MUSBMHDRC_DATA *)hDevice;

    if(pUsbDrvData->DevState != ADI_USB_DEV_STATE_CONFIGURED)
    {
          adi_usbdrv_DriveVbusOff(pUsbDrvData);
          pUsbDrvData->pRegs->POWER &= ~ENUM_USB_POWER_RESET;
          SSYNC();
          pUsbDrvData->pRegs->POWER |= ENUM_USB_POWER_RESET;

          adi_usbdrv_DriveVbusOn(pUsbDrvData);
          adi_usbdrv_SetSessionBit(pUsbDrvData);
    }
}


static void adi_usbdrvh_ResetHandler(ADI_USB_HANDLE const hDevice)
{
    ADI_MUSBMHDRC_DATA *pUsbDrvData = (ADI_MUSBMHDRC_DATA *)hDevice;

    adi_usbdrv_DriveVbusOff(pUsbDrvData);
    pUsbDrvData->pRegs->POWER &= ~ENUM_USB_POWER_RESET;
    SSYNC();
    pUsbDrvData->pRegs->POWER |= ENUM_USB_POWER_RESET;

    adi_usbdrv_DriveVbusOn(pUsbDrvData);
    adi_usbdrv_SetSessionBit(pUsbDrvData);
}


static void adi_usbdrvh_ConnectHandler(ADI_USB_HANDLE const hDevice)
{
    ADI_MUSBMHDRC_DATA        *pUsbDrvData = (ADI_MUSBMHDRC_DATA *)hDevice;
    volatile ADI_USB_REGS_DEF *pRegs = pUsbDrvData->pRegs;

    assert(pUsbDrvData->DevMode == ADI_USB_MODE_OTG_HOST);
    assert((pRegs->DEV_CTL & BITM_USB_DEV_CTL_BDEVICE) == 0);

    pUsbDrvData->DevState = ADI_USB_DEV_STATE_CON;

    pRegs->IEN  |= (ENUM_USB_IRQ_SESSREQ | ENUM_USB_IRQ_VBUSERR);

    pRegs->POWER |= ( ENUM_USB_POWER_SUSEN |
                        ENUM_USB_POWER_HSEN );

    adi_usbdrvh_PowerReset(pUsbDrvData);

    /* After reset check if High Speed mode is negotiated with the Connected Device */
    pUsbDrvData->DevSpd = adi_usbdrv_GetDevSpeed(pRegs->POWER);

}

#else

/* suspend interrupt is generated when bus is idle for 3ms. This interrupt
 * is not generated when hdrc is acting as host. the presence of this
 * interrupt indicates mini-a is not connected yet.
 */
static void adi_usbdrvd_SuspendHandler(ADI_USB_HANDLE const hDevice)
{
    ADI_MUSBMHDRC_DATA *pUsbDrvData = (ADI_MUSBMHDRC_DATA *)hDevice;
    uint8_t DevNum = pUsbDrvData->DevNum;

    pUsbDrvData->pfBusEventCallback(pUsbDrvData, (uint32_t)ADI_USB_BUS_EVENT_SUSPEND, (void*)&DevNum);
}


static void adi_usbdrvd_ResetHandler(ADI_USB_HANDLE const hDevice)
{
    ADI_MUSBMHDRC_DATA *pUsbDrvData = (ADI_MUSBMHDRC_DATA *)hDevice;
    volatile ADI_USB_REGS_DEF *pRegs = pUsbDrvData->pRegs;
    uint8_t DevNum = pUsbDrvData->DevNum;
    int index;

    adi_usbdrv_ReleaseAllDmaChannels(pUsbDrvData);

    for (index = 0; index < pUsbDrvData->EpConfigEntryMax; index++)
    {
           ADI_CFG_USB_PHYEP_CONFIG[index].AllocFlag = 0;
    }

    adi_usbdrv_ResetEndpointZero(pUsbDrvData);
    adi_usbdrv_SetRegDefault(pUsbDrvData);

    /* FIXIT: Do we need to reset it? */
    pUsbDrvData->DevAddr = 0;

    pUsbDrvData->pfBusEventCallback(pUsbDrvData, (uint32_t)ADI_USB_BUS_EVENT_RESET, (void*)&DevNum);

    /* Enable SOF interrupt to set the High Speed Event to stack */
    pRegs->IEN  |=  ENUM_USB_IRQ_SOF;
}

static void adi_usbdrvd_SofHandler(ADI_USB_HANDLE const hDevice)
{
    ADI_MUSBMHDRC_DATA *pUsbDrvData = (ADI_MUSBMHDRC_DATA *)hDevice;
    volatile ADI_USB_REGS_DEF *pRegs = pUsbDrvData->pRegs;
#if !defined(ADI_CFG_USB_ARM_MUSBMHDRC) /* aduCM350 is FS device */
    uint8_t DevNum = pUsbDrvData->DevNum;

//#if (ADI_USBD_DEV_HS_ENABLE == ADI_USBD_DEV_YES) /* FIXIT */
    if(pRegs->POWER & BITM_USB_POWER_HSMODE)
    {
        pUsbDrvData->pfBusEventCallback(pUsbDrvData, (uint32_t)ADI_USB_BUS_EVENT_HS_DETECTED, &DevNum);
    }
#endif    
//#endif
    pRegs->IEN  &=  (~ENUM_USB_IRQ_SOF);
}

static void adi_usbdrvd_ConnectHandler(ADI_USB_HANDLE const hDevice)
{
    return;
}
#endif

#ifdef _MISRA_RULES
#pragma diag(pop)
#endif

/*@}*/
