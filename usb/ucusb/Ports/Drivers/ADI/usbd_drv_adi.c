/*********************************************************************************

  Copyright(c) 2012-2014 Analog Devices, Inc. All Rights Reserved.

  This software is proprietary and confidential.  By using this software you agree
  to the terms of the associated Analog Devices License Agreement.

*********************************************************************************/

/*!
* @file      usbd_drv_adi.c
*
* @brief     Interface layer for device usb stack
*
*/

#ifdef _MISRA_RULES
#pragma diag(push)
#pragma diag(suppress:misra_rules_all:"Suppress all misra rules")
#endif

#include <usbd_core.h>
#include <drivers\usb\controller\device\adi_usbd_dev.h>

#if defined(__ADSPBF533_FAMILY__) || defined(__ADSPBF537_FAMILY__) || defined(__ADSPBF538_FAMILY__) || defined(__ADSPBF518_FAMILY__) || defined(__ADSPBF561_FAMILY__) || defined(__214xx__)
#include <drivers\usb\controller\device\net2272\adi_usbd_dev_net2272.h>
#else
#include <drivers\usb\controller\musbmhdrc\adi_usb_dev_musbmhdrc.h>
#if !defined(ADI_ADUCM350)
#define ADI_CFG_USB_QUEUEING_EN (1)                       /* Enable queuing       */
#endif
#endif
#include <string.h>
#include <limits.h> /* UINT_MAX */
#include "usbd_drv_adi.h"



/*
 * Driver interface information is mentioned in this structure
 */
typedef struct
{
    USBD_DRV        *pUSBDrv;                             /* Handle for stack      */
    ADI_USB_HANDLE  hDevice;                              /* Handle for driver     */
    ADI_USB_URB     UsbUrb[ADI_CFG_USB_MAX_NUM_URB];      /* Endpoint URB          */
} ADI_USBD_DRVIF_INFO;

/*
 * Below critical region functions are not expected to be used in nested fashion 
 */
#if defined(__ADSPBLACKFIN__)
static unsigned int uInterruptStatus;
#define ADI_USB_ENTER_CR() do { uInterruptStatus = cli(); } while(0)
#define ADI_USB_EXIT_CR()  do { sti(uInterruptStatus); } while(0)

#elif defined(__ADSPARM__) && (1 == __ADSPCORTEXA5__)
#define ADI_USB_ENTER_CR() ; /* TODO */
#define ADI_USB_EXIT_CR()  ; /* TODO */

#elif defined(ADI_ADUCM350)
#define ADI_USB_ENTER_CR() CPU_SR_ALLOC(); CPU_CRITICAL_ENTER();
#define ADI_USB_EXIT_CR()  CPU_CRITICAL_EXIT();
  
#else
#define ADI_USB_ENTER_CR() do { disable_interrupts(); } while(0)
#define ADI_USB_EXIT_CR()  do { enable_interrupts(); } while(0)

#endif   /* __ADSPBLACKFIN__ */

/*
 * Local declarations
 */
#if defined(__ADSPBF533_FAMILY__) || defined(__ADSPBF537_FAMILY__) || defined(__ADSPBF538_FAMILY__) || defined(__ADSPBF518_FAMILY__) || defined(__ADSPBF561_FAMILY__) || defined(__214xx__)
static ADI_USB_DRV_API *pADI_USBD_DrvAPI = &ADI_NET2272_DeviceDrvAPI;
#else
static ADI_USB_DRV_API *pADI_USBD_DrvAPI = &ADI_MUSBMHDRC_DeviceDrvAPI;
#endif
static ADI_USBD_DRVIF_INFO gUsbdDrvIfInfo;
static bool EpZeroOpenDone;

static USBD_ERR ADI_USB_ErrorCodeMap[] =
{
    USBD_ERR_NONE,                      /* ADI_USB_RESULT_SUCCESS          */
    USBD_ERR_FAIL,                      /* ADI_USB_RESULT_FAIL             */
    USBD_ERR_DEV_INVALID_STATE,         /* ADI_USB_RESULT_INVALID_DEV_MODE */ /* FIXIT: return code Not used */
    USBD_ERR_EP_ALLOC,                  /* ADI_USB_RESULT_EP_ALLOC_FAILED  */
    USBD_ERR_EP_NONE_AVAIL,             /* ADI_USB_RESULT_EP_NOT_FREE      */
    USBD_ERR_EP_INVALID_ADDR,           /* ADI_USB_RESULT_EP_INVALID       */
    USBD_ERR_EP_STALL,                  /* ADI_USB_RESULT_EP_STALL         */
    USBD_ERR_EP_STALL,                  /* ADI_USB_RESULT_EP_NAK           */ /* FIXIT: return code */
    USBD_ERR_FAIL,                      /* ADI_USB_RESULT_EP_ERROR         */ /* FIXIT: return code */
    USBD_ERR_FAIL,                      /* ADI_USB_RESULT_FIFO_FULL        */ /* FIXIT: return code Not used */
    USBD_ERR_NULL_PTR,                  /* ADI_USB_RESULT_NULL_POINTER     */
    USBD_ERR_INVALID_ARG,               /* ADI_USB_RESULT_BUF_INVALID      */ /* FIXIT: return code Not used */
    USBD_ERR_DEV_ALLOC,                 /* ADI_USB_RESULT_INVALID_HANDLE   */
    USBD_ERR_NONE,                      /* ADI_USB_RESULT_URB_COMPLETE     */
    USBD_ERR_NONE,                      /* ADI_USB_RESULT_URB_PENDING      */ 
    USBD_ERR_FAIL,                      /* ADI_USB_RESULT_URB_ABORT        */
    USBD_ERR_FAIL,                      /* ADI_USB_RESULT_URB_PARTIAL      */
    USBD_ERR_FAIL,                      /* ADI_USB_RESULT_DMA_CHANNEL_BUSY */
};

#if (ADI_CFG_USB_QUEUEING_EN == 1)
static ADI_USB_RESULT adi_usbdrvif_QueueTransfer(ADI_USB_URB *pUsbUrb,CPU_INT08U *p_buf,CPU_INT32U buf_len);
static ADI_USB_RESULT adi_usbdrvif_ScheduleTransfer(ADI_USB_URB *pUsbUrb, CPU_INT08U    *p_buf, 
                                                    CPU_INT32U   buf_len, ADI_USB_EP_DIR ep_dir);
#endif

#if defined(ADI_DEBUG)
void adi_usbdrvIf_failure(void)
{
    while(1);
}
#endif

/**
 * @brief       Bus Event callback handler
 *
 * @details     This callback gets invoked for USB bus events. Depending on the bus event appropriate
 *              USB stack core function is called.
 *
 * @param [in]  hDevice          USB device handle
 *
 * @param [in]  Event            Bus Event 
 *
 * @param [in]  DevNum           Device number
 *
 * @return      void
 *
 * @note        
 */
void adi_usbdrvIf_BusEventCallback(ADI_USB_HANDLE hDevice, ADI_USB_BUS_EVENT Event, uint8_t DevNum)
{
    USBD_DRV     *p_drv = gUsbdDrvIfInfo.pUSBDrv;

    switch (Event)
    {
        case ADI_USB_BUS_EVENT_DEV_CON:
            USBD_EventConn(p_drv);
        break;

        case ADI_USB_BUS_EVENT_DEV_DISCON:
            USBD_EventDisconn(p_drv);
        break;

        case ADI_USB_BUS_EVENT_SUSPEND:
            USBD_EventSuspend(p_drv);
        break;

        case ADI_USB_BUS_EVENT_RESUME:
            USBD_EventResume(p_drv);
        break;

        case ADI_USB_BUS_EVENT_RESET:
            USBD_EventReset(p_drv);
        break;

        case ADI_USB_BUS_EVENT_HS_DETECTED:
            USBD_EventHS(p_drv);
        break;
    }
}

/**
 * @brief       Endpoint Zero callback handler
 *
 * @details     This callback gets invoked for all EP Zero requests. Enumeration and and class level 
 *              endpoint zero communication happens through this callback function. During setup phase
 *              setup packets are passed to the core stack for further processing. This callback is
 *              expected to be called from interrupt level.
 *
 * @param [in]  hDevice          USB device handle
 *
 * @param [in]  Event            Endpoint Zero Event 
 *
 * @param [in]  pBuffer          Pointer to the buffer
 *
 * @return      void
 *
 * @note        
 */
void adi_usbdrvIf_EpZeroCallback(ADI_USB_URB *pUsbUrb, ADI_USB_RESULT Event, void *pBuffer)
{
    USBD_DRV     *p_drv = gUsbdDrvIfInfo.pUSBDrv;

    switch (Event) 
    {
        case ADI_USB_RESULT_URB_COMPLETE:
        {                
            if(pUsbUrb->EpDir == ADI_USB_EP_DIR_SETUP)
            {
                /* Pass SETUP packet to Core */
                USBD_EventSetup(p_drv, pBuffer);
            }
            else if(pUsbUrb->EpDir == ADI_USB_EP_DIR_RX)
            {
                /* Pass RX completion event to Core */
                USBD_EP_RxCmpl(p_drv, pUsbUrb->EpAddr);
            }
            else if(pUsbUrb->EpDir == ADI_USB_EP_DIR_TX)
            {
                /* Pass TX completion event to Core */
                USBD_EP_TxCmpl(p_drv, pUsbUrb->EpAddr);
            }
        }
        break;

        default:
#if defined(ADI_DEBUG)
                adi_usbdrvIf_failure();
#endif
        break;
    }

    return;
}


/**
 * @brief       Endpoint Data handler
 *
 * @details     This callback gets invoked for all data endpoints. Once reception of a packet
 *              completes and transmission of data completes the buffer is returned by the 
 *              driver using this callback.
 *
 *              In case of transmit if there is a packet that is already queued then it will be
 *              scheduled after stack level completion routine is called.
 *
 * @param [in]  hDevice          USB device handle
 *
 * @param [in]  Event            USB callback Event 
 *
 * @param [in]  pBuffer          Pointer to Buffer
 *
 * @return      void
 *
 * @note        
 */
void adi_usbdrvIf_EpDataCallback(ADI_USB_URB *pUsbUrb, ADI_USB_RESULT Event, void *pBuffer)
{
    USBD_DRV     *p_drv = gUsbdDrvIfInfo.pUSBDrv;

    switch (Event)
    {
        case ADI_USB_RESULT_URB_COMPLETE:

            if(pUsbUrb->EpDir == ADI_USB_EP_DIR_RX)
            {
                /* Pass RX completion event to Core */
                USBD_EP_RxCmpl(p_drv, pUsbUrb->EpAddr);
            }
            else if(pUsbUrb->EpDir == ADI_USB_EP_DIR_TX)
            {
                /* Pass TX completion event to Core */
                USBD_EP_TxCmpl(p_drv, pUsbUrb->EpAddr);

#if (ADI_CFG_USB_QUEUEING_EN == 1)
                 if ((pUsbUrb->EpAddr & 0xF) != ADI_USB_EP_NUM_0)
                 {
                     uint32_t CurIndex = pUsbUrb->CurReadIdx;
                     if (pUsbUrb->CurUrbCount > 0)
                     {
                         adi_usbdrvif_ScheduleTransfer(pUsbUrb, 
                                                       pUsbUrb->UrbUnits[CurIndex].pUnitData,
                                                       pUsbUrb->UrbUnits[CurIndex].UnitTransferLen,
                                                       ADI_USB_EP_DIR_TX);
                          pUsbUrb->UrbUnits[CurIndex].UnitValidFlag = 0;
                          CurIndex++;
                          pUsbUrb->CurReadIdx = (CurIndex == ADI_USB_CFG_MAX_NUM_QUEUED_BUFS) ? 0 : CurIndex;
                          pUsbUrb->CurUrbCount--;
                     }
                  }
#endif
            }

            break;

        default:
#if defined(ADI_DEBUG)
            adi_usbdrvIf_failure();
#endif
            break;
    }
        return;
}


static ADI_USB_URB* adi_usbdrvif_AllocUrb(CPU_INT08U EpAddr)
{
    if((EpAddr & 0xF) == ADI_USB_EP_NUM_0)
    {
         return &gUsbdDrvIfInfo.UsbUrb[0];
    }
    else
    {
         return &gUsbdDrvIfInfo.UsbUrb[ADI_USB_GET_URB_INDEX(EpAddr)];
    }
}

static uint32_t adi_usbdrvif_GetUrb(const CPU_INT08U EpAddr, ADI_USB_URB **pUsbUrb)
{
    if((EpAddr & 0xF) == ADI_USB_EP_NUM_0)
    {
        *pUsbUrb = &gUsbdDrvIfInfo.UsbUrb[0];
    }
    else
    {
        *pUsbUrb = &gUsbdDrvIfInfo.UsbUrb[ADI_USB_GET_URB_INDEX(EpAddr)];
    }
    return ADI_USB_GET_URB_INDEX(EpAddr);
}


static  void         USBD_DrvInit          (USBD_DRV     *p_drv,
                                            USBD_ERR     *p_err)
{
    ADI_USB_RESULT Result;
    ADI_USB_INIT_DATA DevInitData;

    DevInitData.DevNum = p_drv->DevNbr;
    DevInitData.DevMode = ADI_USB_MODE_DEVICE;
    gUsbdDrvIfInfo.pUSBDrv = p_drv;

    Result = pADI_USBD_DrvAPI->Init(&gUsbdDrvIfInfo.hDevice,   /* Host controller driver handle */
                                         &DevInitData);             /* Device Data                   */

    *p_err = ADI_USB_ErrorCodeMap[Result];
    if(*p_err != USBD_ERR_NONE)
    {
        return;
    }

    Result = pADI_USBD_DrvAPI->RegBusEventCallback(gUsbdDrvIfInfo.hDevice, (ADI_CALLBACK)adi_usbdrvIf_BusEventCallback);
    *p_err = ADI_USB_ErrorCodeMap[Result];
    if(*p_err != USBD_ERR_NONE)
    {
        return;
    }

    Result = pADI_USBD_DrvAPI->RegEpZeroCallback(gUsbdDrvIfInfo.hDevice, (ADI_CALLBACK)adi_usbdrvIf_EpZeroCallback);
    *p_err = ADI_USB_ErrorCodeMap[Result];
    if(*p_err != USBD_ERR_NONE)
    {
        return;
    }

    Result = pADI_USBD_DrvAPI->RegEpDataCallback(gUsbdDrvIfInfo.hDevice, (ADI_CALLBACK)adi_usbdrvIf_EpDataCallback);
    *p_err = ADI_USB_ErrorCodeMap[Result];
}

static  void         USBD_DrvStop          (USBD_DRV     *p_drv)
{
    pADI_USBD_DrvAPI->Stop(gUsbdDrvIfInfo.hDevice);
    return;
}

static  void         USBD_DrvEP_Open       (USBD_DRV     *p_drv,
                                            CPU_INT08U    ep_addr,
                                            CPU_INT08U    ep_attrib,
                                            CPU_INT16U    max_pkt_size,
                                            CPU_INT08U    transaction_frame,
                                            USBD_ERR     *p_err)
{
    ADI_USB_RESULT Result = ADI_USB_RESULT_SUCCESS;
    ADI_USB_EP_DESC EpDesc;

    EpDesc.Attributes = ep_attrib;

    /* At the driver level, IN and OUT EP0 are same */
    EpDesc.EndpointAddress = (ep_addr & 0x7F) ? ep_addr : ADI_USB_EP_NUM_0;

    /* return if EP0 is already opened */
    if(EpDesc.EndpointAddress == ADI_USB_EP_NUM_0)
    {
        if(EpZeroOpenDone)
        {
            *p_err = ADI_USB_ErrorCodeMap[Result];
            return;
        }
        else
        {
            EpZeroOpenDone = true;
        }
    }

    EpDesc.MaxPacketSize = max_pkt_size;
    EpDesc.Interval = transaction_frame; 

    *p_err = USBD_ERR_NONE;

    Result = pADI_USBD_DrvAPI->EP_Open(gUsbdDrvIfInfo.hDevice, (ADI_USB_EP_DESC *)&EpDesc, p_drv->DevNbr);

    *p_err = ADI_USB_ErrorCodeMap[Result];
}

static  void         USBD_DrvEP_Close      (USBD_DRV     *p_drv,
                                            CPU_INT08U    ep_addr)
{

    if((ep_addr & 0x7F) == ADI_USB_EP_NUM_0)
    {
        EpZeroOpenDone = false;
    }
    pADI_USBD_DrvAPI->EP_Close(gUsbdDrvIfInfo.hDevice, ep_addr);

}


static  CPU_INT32U   USBD_DrvEP_RxStart    (USBD_DRV     *p_drv,
                                            CPU_INT08U    ep_addr,
                                            CPU_INT08U   *p_buf,
                                            CPU_INT32U    buf_len,
                                            USBD_ERR     *p_err)
{

    ADI_USB_URB       *pUsbUrb;
    ADI_USB_RESULT    Result;

    /* Check Buffer not NULL */
    if(buf_len && !p_buf)
    {
        *p_err = USBD_ERR_INVALID_ARG;
         return 0;
    }

    /* Get the statically allocated URB on the basis of ep_addr */
    pUsbUrb = adi_usbdrvif_AllocUrb(ep_addr);

    if (((pUsbUrb->EpAddr & 0xF) == ADI_USB_EP_NUM_0) || ((pUsbUrb->CurUrbCount == 0) &&
        (pADI_USBD_DrvAPI->EP_URB_GetState(gUsbdDrvIfInfo.hDevice, ep_addr) == ADI_USB_URB_STATE_FREE ))
          )
     {
         pUsbUrb->EpAddr = ep_addr;
         pUsbUrb->EpDir = ADI_USB_EP_DIR_RX;

         pUsbUrb->ProcessedBytes  = 0;
         pUsbUrb->TransferLen     = buf_len;
         pUsbUrb->pData           = p_buf;
         pUsbUrb->DevAddr         = p_drv->DevNbr;

         Result = pADI_USBD_DrvAPI->URB_Submit(gUsbdDrvIfInfo.hDevice, pUsbUrb);

         *p_err = ADI_USB_ErrorCodeMap[Result]; /* ADI_USB_RESULT_URB_PENDING, ADI_USB_RESULT_URB_ABORT*/
    }
#if ADI_CFG_USB_QUEUEING_EN == 1
    else
    {
         Result = adi_usbdrvif_QueueTransfer(pUsbUrb,p_buf,buf_len);

         if (Result == ADI_USB_RESULT_FAIL)
         {
             *p_err  = USBD_ERR_EP_QUEUING;
         }

         return (buf_len);
    }
#endif
    return (pUsbUrb->TransferLen);
}

static  CPU_INT32U   USBD_DrvEP_Tx         (USBD_DRV     *p_drv,
                                            CPU_INT08U    ep_addr,
                                            CPU_INT08U   *p_buf,
                                            CPU_INT32U    buf_len,
                                            USBD_ERR     *p_err)
{
    /* We support maximum UINT MAX */
    if(buf_len < UINT_MAX)
    {
        *p_err = USBD_ERR_NONE;
        return buf_len;
    }
    else
    {
       *p_err = USBD_ERR_FAIL;
       return 0;
    }
}

#if (ADI_CFG_USB_QUEUEING_EN == 1)
static  ADI_USB_RESULT adi_usbdrvif_ScheduleTransfer(ADI_USB_URB *pUsbUrb, CPU_INT08U *p_buf, CPU_INT32U buf_len, ADI_USB_EP_DIR ep_dir)
{
    ADI_USB_RESULT  Result = ADI_USB_RESULT_SUCCESS;

    if ((pUsbUrb->EpAddr & 0xF) != ADI_USB_EP_NUM_0)
    {
        pUsbUrb->EpDir           = ep_dir;
        pUsbUrb->ProcessedBytes  = 0;
        pUsbUrb->TransferLen     = buf_len;
        pUsbUrb->pData           = p_buf;

        Result = pADI_USBD_DrvAPI->URB_Submit(gUsbdDrvIfInfo.hDevice, pUsbUrb);
    }

    return Result;
}


static ADI_USB_RESULT adi_usbdrvif_QueueTransfer(ADI_USB_URB *pUsbUrb, CPU_INT08U *p_buf, CPU_INT32U buf_len)
{
    uint32_t  CurUnitIndex;

    ADI_USB_ENTER_CR();

    CurUnitIndex = pUsbUrb->CurWriteIdx;

    if (pUsbUrb->UrbUnits[CurUnitIndex].UnitValidFlag == 1)
    {
        ADI_USB_EXIT_CR();
        return ADI_USB_RESULT_FAIL;
    }

    pUsbUrb->UrbUnits[CurUnitIndex].pUnitData       = p_buf; 
    pUsbUrb->UrbUnits[CurUnitIndex].UnitTransferLen = buf_len;
    pUsbUrb->UrbUnits[CurUnitIndex].UnitValidFlag   = 1;

    CurUnitIndex++;
    pUsbUrb->CurUrbCount++;

    pUsbUrb->CurWriteIdx = (CurUnitIndex == ADI_USB_CFG_MAX_NUM_QUEUED_BUFS) ? 0 : CurUnitIndex;

    ADI_USB_EXIT_CR();

    return ADI_USB_RESULT_SUCCESS;
}
#endif

static  void         USBD_DrvEP_TxStart    (USBD_DRV     *p_drv,
                                            CPU_INT08U    ep_addr,
                                            CPU_INT08U   *p_buf,
                                            CPU_INT32U    buf_len,
                                            USBD_ERR     *p_err)
{
    ADI_USB_URB       *pUsbUrb;
    ADI_USB_RESULT    Result;

    /* Check Buffer not NULL */
    if(buf_len && !p_buf)
    {
        *p_err = USBD_ERR_INVALID_ARG;
        return;
    }

    pUsbUrb = adi_usbdrvif_AllocUrb(ep_addr);

#if (ADI_CFG_USB_QUEUEING_EN == 1)
    if ((pUsbUrb->CurUrbCount == 0) &&
        (pADI_USBD_DrvAPI->EP_URB_GetState(gUsbdDrvIfInfo.hDevice, ep_addr) == ADI_USB_URB_STATE_FREE )
        )
#endif
    {
        pUsbUrb->EpAddr = ep_addr;
        pUsbUrb->EpDir = ADI_USB_EP_DIR_TX;

        pUsbUrb->ProcessedBytes  = 0;
        pUsbUrb->TransferLen     = buf_len;
        pUsbUrb->pData           = p_buf;
        pUsbUrb->DevAddr         = p_drv->DevNbr;

        Result = pADI_USBD_DrvAPI->URB_Submit(gUsbdDrvIfInfo.hDevice, pUsbUrb);

        *p_err = ADI_USB_ErrorCodeMap[Result]; /* ADI_USB_RESULT_URB_PENDING, ADI_USB_RESULT_URB_ABORT*/
    }
#if (ADI_CFG_USB_QUEUEING_EN == 1)
    else
    {
        Result = adi_usbdrvif_QueueTransfer(pUsbUrb,p_buf,buf_len);

        if (Result == ADI_USB_RESULT_FAIL)
        {
           *p_err  = USBD_ERR_EP_QUEUING;
        }
    }
#endif

    return;

}

static  CPU_INT32U   USBD_DrvEP_Rx         (USBD_DRV     *p_drv,
                                            CPU_INT08U    ep_addr,
                                            CPU_INT08U   *p_buf,
                                            CPU_INT32U    buf_len,
                                            USBD_ERR     *p_err)
{
    ADI_USB_URB   *pUsbUrb;
    uint32_t       PrevProcessedBytes;
#if (ADI_CFG_USB_QUEUEING_EN == 1)    
    uint32_t       CurIndex;
#endif
    
    ADI_USB_ENTER_CR();

    adi_usbdrvif_GetUrb(ep_addr, &pUsbUrb);

    PrevProcessedBytes = pUsbUrb->ProcessedBytes;

#if (ADI_CFG_USB_QUEUEING_EN == 1)
    CurIndex = pUsbUrb->CurReadIdx;
    if (pUsbUrb->CurUrbCount > 0)
    {
          adi_usbdrvif_ScheduleTransfer(pUsbUrb,pUsbUrb->UrbUnits[CurIndex].pUnitData,pUsbUrb->UrbUnits[CurIndex].UnitTransferLen,ADI_USB_EP_DIR_RX);
          pUsbUrb->UrbUnits[CurIndex].UnitValidFlag = 0;
          CurIndex++;
          pUsbUrb->CurReadIdx = (CurIndex == ADI_USB_CFG_MAX_NUM_QUEUED_BUFS) ? 0 : CurIndex;
          pUsbUrb->CurUrbCount--;
     }
#endif
    ADI_USB_EXIT_CR();

    return (PrevProcessedBytes);
}

static  void         USBD_DrvStart         (USBD_DRV     *p_drv,
                                            USBD_ERR     *p_err)
{
     pADI_USBD_DrvAPI->Start(gUsbdDrvIfInfo.hDevice);
}

static  CPU_BOOLEAN  USBD_DrvAddrSet       (USBD_DRV     *p_drv,
                                            CPU_INT08U    dev_addr)
{
     pADI_USBD_DrvAPI->SetDevAddr(gUsbdDrvIfInfo.hDevice, dev_addr);
     return true;
}

static  void         USBD_DrvAddrEn        (USBD_DRV     *p_drv,
                                            CPU_INT08U    dev_addr)
{
    /*
     * This is not required for the MUSBMHDRC controller
     */
}

static  CPU_BOOLEAN  USBD_DrvCfgSet        (USBD_DRV     *p_drv,
                                            CPU_INT08U    cfg_val)
{
     /* FIXIT: ADD API in the driver to reset the Dynamic FIFO Stuff */
     return true;
}

static  void         USBD_DrvCfgClr        (USBD_DRV     *p_drv,
                                            CPU_INT08U    cfg_val)
{
     /* FIXIT: ADD API in the driver to reset the Dynamic FIFo Stuff */
}

static  CPU_INT16U   USBD_DrvGetFrameNbr   (USBD_DRV     *p_drv)
{
        return pADI_USBD_DrvAPI->FrameNbrGet(gUsbdDrvIfInfo.hDevice);
}


static  void         USBD_DrvEP_RxZLP      (USBD_DRV     *p_drv,
                                            CPU_INT08U    ep_addr,
                                            USBD_ERR     *p_err)
{
    /* This function is to be used for states update and structures reset.
     * We are not using this function because we are already doing the required
     * things while submitting the buffer for RXZLP.
     * Refer to adi_usbdrv_SubmitEp0RxBuffer() function  
     */
}


static  void         USBD_DrvEP_TxZLP      (USBD_DRV     *p_drv,
                                            CPU_INT08U    ep_addr,
                                            USBD_ERR     *p_err)
{
    ADI_USB_URB       *pUsbUrb;
    ADI_USB_RESULT    Result;

    ADI_USB_ENTER_CR();
    pUsbUrb = adi_usbdrvif_AllocUrb(ep_addr);

    pUsbUrb->EpAddr          = ep_addr;
    pUsbUrb->EpDir           = ADI_USB_EP_DIR_TX;
    pUsbUrb->ProcessedBytes  = 0;
    pUsbUrb->TransferLen     = 0;
    pUsbUrb->pData           = NULL;
    pUsbUrb->DevAddr         = p_drv->DevNbr;

    Result = pADI_USBD_DrvAPI->URB_Submit(gUsbdDrvIfInfo.hDevice, pUsbUrb);

    *p_err = ADI_USB_ErrorCodeMap[Result]; /* ADI_USB_RESULT_URB_PENDING, ADI_USB_RESULT_URB_ABORT*/

    ADI_USB_EXIT_CR();

}

static  CPU_BOOLEAN  USBD_DrvEP_Abort      (USBD_DRV     *p_drv,
                                            CPU_INT08U    ep_addr)
{
    ADI_USB_URB       *pUsbUrb;

    ADI_USB_ENTER_CR();
    adi_usbdrvif_GetUrb(ep_addr, &pUsbUrb);

#if (ADI_CFG_USB_QUEUEING_EN == 1)
    pUsbUrb->CurReadIdx  = 0;
    pUsbUrb->CurWriteIdx = 0;
    pUsbUrb->CurUrbCount = 0;
    memset((char*)&pUsbUrb->UrbUnits[0],0,sizeof(pUsbUrb->UrbUnits));
#endif

    pADI_USBD_DrvAPI->EP_Abort( gUsbdDrvIfInfo.hDevice, ep_addr);

    ADI_USB_EXIT_CR();

    return true;
}

static  CPU_BOOLEAN  USBD_DrvEP_Stall      (USBD_DRV     *p_drv,
                                            CPU_INT08U    ep_addr,
                                            CPU_BOOLEAN   b_stall)
{
        pADI_USBD_DrvAPI->EP_Stall( gUsbdDrvIfInfo.hDevice, ep_addr,b_stall);
        return true;
}

static  void         USBD_DrvISR_Handler   (USBD_DRV     *p_drv)
{
        /* ADI SSL/DD 2.0 controller driver implementation handles ISR */
}


/*
*********************************************************************************************************
*                                      MICRIUM USB DEVICE CONTROLLER DRIVER API
*********************************************************************************************************
*/

const  USBD_DRV_API  USBD_DrvAPI_ADI = { 
                                           USBD_DrvInit,
                                           USBD_DrvStart,
                                           USBD_DrvStop,
                                           USBD_DrvAddrSet,
                                           USBD_DrvAddrEn,
                                           USBD_DrvCfgSet,
                                           USBD_DrvCfgClr,
                                           USBD_DrvGetFrameNbr,
                                           USBD_DrvEP_Open,
                                           USBD_DrvEP_Close,
                                           USBD_DrvEP_RxStart,
                                           USBD_DrvEP_Rx,
                                           USBD_DrvEP_RxZLP,
                                           USBD_DrvEP_Tx,
                                           USBD_DrvEP_TxStart,
                                           USBD_DrvEP_TxZLP,
                                           USBD_DrvEP_Abort,
                                           USBD_DrvEP_Stall,
                                           USBD_DrvISR_Handler,
                                      };

#ifdef _MISRA_RULES
#pragma diag(pop)
#endif
