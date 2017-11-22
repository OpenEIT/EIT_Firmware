/*********************************************************************************

  Copyright(c) 2013 Analog Devices, Inc. All Rights Reserved.

  This software is proprietary and confidential.  By using this software you agree
  to the terms of the associated Analog Devices License Agreement.

*********************************************************************************/

/*!
* @file      adi_usbh_dev_musbmhdrc.c
*
* @brief     USB controller driver for MUSBMHDRC and MUSBHDRC based controllers. This
*            controller is used on ADSP-BF54x,ADSP-BF52x and ADSP-BF60x family of processors
*/

/** @addtogroup USB MUSBMHDRC controller driver
 *  @{
 */
#ifdef _MISRA_RULES
#pragma diag(push)
#pragma diag(suppress:misra_rules_all:"Suppress all misra rules")
#endif

#include "adi_usb_dev_musbmhdrc_local.h"

static ADI_MUSBMHDRC_DATA gUsbDrvData[ADI_CFG_USB_MAX_NBR_HC];

/* Forward declarations */
static int32_t        adi_usbdrv_GetEpPhyNum(ADI_MUSBMHDRC_DATA *pUsbDrvData, uint32_t EpData, uint8_t *EpPhyIdx);
static void           adi_usbdrv_SetTransferMode(ADI_MUSBMHDRC_DATA *pUsbDrvData, ADI_USB_EP_INFO *pEpInfo);
static int32_t        adi_usbdrv_GetFreePhyEp(ADI_MUSBMHDRC_DATA *pUsbDrvData, ADI_USB_EP_DESC *pEpDesc, uint8_t *EpPhyNum);
static ADI_USB_RESULT adi_usbdrv_SetDynamicFifoParams(ADI_MUSBMHDRC_DATA *pUsbDrvData, ADI_USB_EP_INFO *pEpInfo,bool bDoubleBuffer);

/**
 * @brief       Initializes USB controller driver
 *
 * @details     This API opens and initializes the USB controller driver.ADI_USB_INIT_DATA structure is used to
 *              pass initialization data. It includes the device number, operation mode (device or host). 
 *              This API is expected to be called from the interface layer. Interface layer translates the
 *              Micrium stack requests to the driver requests. 
 *
 *              This API initializes the USB subsystem by setting up the required registers and installing bus event
 *              handlers.This API will not enable the USB device. This API also performs any platform specific 
 *              initializations by setting or clearing the GPIO registers.
 *
 *              USB device operations has to be enabled by calling the API adi_musbmhdrc_Start.
 *
 *              Handle is returned to the caller upon success. This handle has to be used in all subsequent
 *              calls to the usb controller driver.
 *
 * @param [in]  hDevice                Pointer to location where the device handle will
 *                                     be returned upon successful initialization.
 *
 * @param [in]  pInitData              Pointer to initialization data structure.
 *
 *
 * @return      Status
 *
 *  - #ADI_USB_RESULT_SUCCESS          Upon successful opening of the device
 *
 *  - #ADI_USB_RESULT_FAIL             Failed to open the device
 *
 *  - #ADI_USB_RESULT_INVALID_HANDLE   [D] Invalid Device handle is supplied
 *
 *  - #ADI_USB_RESULT_NULL_POINTER     [D] Null parameter supplied
 *
 * @note        
 *
 * @sa          adi_musbmhdrc_Stop()
 * @sa          adi_musbmhdrc_Start()
 */

/* 
 * Device state transitions
 *
 *                                   (adi_musbmhdrc_Init)                          
 *     ADI_USB_DEV_STATE_RESET       -------------------->   ADI_USB_DEV_STATE_OPENED
 *
 *                                   (Connect Event)
 *     ADI_USB_DEV_STATE_OPENED      -------------------->   ADI_USB_DEV_STATE_CONN 
 *
 *                                   (Set Address command)
 *     ADI_USB_DEV_STATE_CONN        -------------------->   ADI_USB_DEV_STATE_ADDRESSED
 *
 *                                   (Set Config command)
 *     ADI_USB_DEV_STATE_ADDRESSED   -------------------->   ADI_USB_DEV_STATE_CONFIGURED
 *
 *                                   (Disconnect Event)
 *     ADI_USB_DEV_STATE_CONFIGURED  -------------------->   ADI_USB_DEV_STATE_DISCON
 *
 * Note 1: EP0 transfers are possible after device enters in to ADI_USB_DEV_STATE_CONN state
 * Note 2: Data EP transfers are possible after device enters in to ADI_USB_DEV_STATE_CONFIGURED state.
 * Note 3: Any time Disconnect event happens, the state of device switches to ADI_USB_DEV_STATE_DISCON
 * Note 4: Device will be in ADI_USB_DEV_STATE_RESET state after reset and adi_musbmhdrc_Stop call.
 */

static ADI_USB_RESULT  adi_musbmhdrc_Init (ADI_USB_HANDLE *hDevice, ADI_USB_INIT_DATA *pInitData)
{
    ADI_MUSBMHDRC_DATA *pUsbDrvData;
    uint32_t            DevNumber;
    ADI_USB_RESULT      Result = ADI_USB_RESULT_SUCCESS;

    assert(pInitData != NULL);
    assert(hDevice   != NULL);

    ADI_USB_REPORT_ERROR (pInitData, NULL, ADI_USB_RESULT_NULL_POINTER);
    ADI_USB_REPORT_ERROR (hDevice,   NULL, ADI_USB_RESULT_NULL_POINTER);

    DevNumber   = pInitData->DevNum;

#if defined(ADI_DEBUG)
    if (DevNumber >= ADI_CFG_USB_MAX_NBR_HC)
    {
        return (ADI_USB_RESULT_INVALID_HANDLE);
    }
#endif 

    pUsbDrvData = &gUsbDrvData[DevNumber];
    memset(pUsbDrvData, 0, sizeof(ADI_MUSBMHDRC_DATA));

    pUsbDrvData->DevNum  = DevNumber;
    pUsbDrvData->DevMode = pInitData->DevMode;
    pUsbDrvData->CacheOn = adi_usbdrv_IsDataCacheEnabled();

    /* Assign Register mappings */
    pUsbDrvData->pRegs = (volatile ADI_USB_REGS_DEF*)ADI_USB_REG_BASE;

    Result = adi_usbdrv_PlatformSpecInit(pUsbDrvData);
    if(Result != ADI_USB_RESULT_SUCCESS)
    {
        return ADI_USB_RESULT_FAIL;
    }

    adi_usbdrv_SetRegDefault(pUsbDrvData);

    *hDevice = (ADI_USB_HANDLE)pUsbDrvData;

    pUsbDrvData->EpConfigEntryMax = sizeof(ADI_CFG_USB_PHYEP_CONFIG)/sizeof(ADI_USB_EP_CONFIG);

    adi_usbdrv_FreePhyEndpoints(pUsbDrvData);
    adi_usbdrv_SetBusEventHandlers(pUsbDrvData);

    pUsbDrvData->DevState = ADI_USB_DEV_STATE_OPENED;

    return ADI_USB_RESULT_SUCCESS;
}


/**
 * @brief       Starts and enables the usb controller driver
 *
 * @details     This API enables the USB device and starts the communications. In host mode this API
 *              drives the VBUS and starts the enumerations process. It also installs the appropriate
 *              event handlers.
 *
 * @param [in]  hDevice                Handle that is returned via adi_musbmhdrc_Init
 *
 * @return      Status
 *
 *  - #ADI_USB_RESULT_SUCCESS          Upon successful starting of the device
 *
 *  - #ADI_USB_RESULT_INVALID_HANDLE   [D] Invalid Device handle is supplied
 *
 *  - #ADI_USB_RESULT_INVALID_DEV_MODE [D] If device mode is not set
 *
 * @note        
 *             Caller(s) USBH_HC_Start()
 *
 * @sa          adi_musbmhdrc_Stop()
 * @sa          adi_musbmhdrc_Init()
 */

static ADI_USB_RESULT  adi_musbmhdrc_Start (ADI_USB_HANDLE const hDevice)
{
    ADI_MUSBMHDRC_DATA *pUsbDrvData = (ADI_MUSBMHDRC_DATA *)hDevice;

    ADI_USB_VALIDATE_HANDLE(hDevice);

    if (adi_usbdrv_InstallInterruptHandlers(pUsbDrvData) != ADI_USB_RESULT_SUCCESS)
    {
        return ADI_USB_RESULT_FAIL;
    }

    switch(pUsbDrvData->DevMode)
    {
#if (ADI_CFG_USB_OTG_HOST == 1)
        case ADI_USB_MODE_OTG_HOST:
        {
           /* In host mode suspend interrupt is used to drive the vbus and set the session bit.If 
            * there is no usb connection/ activity on the bus for 3ms suspend interrupt gets raised.
            * Host suspend interrupt handler is used to drive the vbus and set the session.Suspend  
            * interrupt is enabled as it is used as means to initiate session in host mode.
            */
            pUsbDrvData->pRegs->POWER |= ENUM_USB_POWER_SUSEN;
            adi_usbdrvh_EnableHost(pUsbDrvData);
        }
        break;
#else
        case ADI_USB_MODE_DEVICE:
        {
#if defined(ADI_CFG_USB_BF_MUSBMHDRC)
            pUsbDrvData->pRegs->PHY_CTL |= (1 << BITP_USB_PHY_CTL_EN);
#endif
            pUsbDrvData->pRegs->POWER |= ENUM_USB_POWER_SOFTCONN;

            pUsbDrvData->pRegs->POWER |= BITM_USB_POWER_ISOUPDT;

        }
        break;
#endif
        default:
            return ADI_USB_RESULT_INVALID_DEV_MODE;
    }

    return (ADI_USB_RESULT_SUCCESS);
}


/**
 * @brief       Stops usb controller driver
 *
 * @details     Stops usb controller driver and frees up any associated resources. This API also
 *              un-installs any installed handlers.
 *
 * @param [in]  hDevice             Handle that is returned via adi_musbmhdrc_Init
 *
 * @return      Status
 *
 *  - #ADI_USB_RESULT_SUCCESS       Upon successful stopping of the device
 *
 *  - #ADI_USB_RESULT_FAIL          The API was unsuccessful
 *
 * @note        
 *             Caller(s) USBH_HC_Start()
 *
 * @sa          adi_musbmhdrc_Stop()
 * @sa          adi_musbmhdrc_Init()
 */


static  ADI_USB_RESULT  adi_musbmhdrc_Stop (ADI_USB_HANDLE const hDevice)
{
    ADI_MUSBMHDRC_DATA        *pUsbDrvData = (ADI_MUSBMHDRC_DATA *)hDevice;
    volatile ADI_USB_REGS_DEF *pRegs;

    ADI_USB_VALIDATE_HANDLE(hDevice);

    pRegs = pUsbDrvData->pRegs;

    ADI_USB_ENTER_CR();

    adi_usbdrv_AckPendingInterrupts(pUsbDrvData);

    /* Clear VBUS IRQs */
    pRegs->VBUS_CTL = (pRegs->VBUS_CTL & 0xff);

    /* Clear the session bit */
    pRegs->DEV_CTL &= ~(  ENUM_USB_DEV_CTL_SESSION |
                          ENUM_USB_DEV_CTL_HOSTREQ       /* Defined as NULL for 350 */
                       );

    SSYNC();

    if (adi_usbdrv_UnInstallInterruptHandlers(pUsbDrvData) != ADI_USB_RESULT_SUCCESS)
    {
        return (ADI_USB_RESULT_FAIL);
    }

    /* Clear Device data structure */
    memset(&pUsbDrvData, 0, sizeof(ADI_MUSBMHDRC_DATA));

    ADI_USB_EXIT_CR();

    return (ADI_USB_RESULT_SUCCESS);
}


/**
 * @brief       Returns the current operating speed
 *
 * @details     Returns the current operating speed. Operating speed is determined by checking the power register
 *              and set in the connect handler. Connect handler gets called because of the connection request.
 *
 * @param [in]  hDevice                Handle that is returned via adi_musbmhdrc_Init
 *
 * @return      Status
 *
 *  - #ADI_USB_RESULT_SUCCESS          Upon successful returning of speed
 *
 *  - #ADI_USB_RESULT_INVALID_HANDLE   [D] Invalid Device handle is supplied
 *
 * @note        
 *             Caller(s) USBH_HC_Start()
 *
 * @sa          adi_musbmhdrc_Start()
 */

static  ADI_USB_SPEED  adi_musbmhdrc_SpdGet (ADI_USB_HANDLE const hDevice)
{
    ADI_MUSBMHDRC_DATA *pUsbDrvData = (ADI_MUSBMHDRC_DATA *)hDevice;
    return pUsbDrvData->DevSpd;
}


/**
 * @brief       Suspend controller
 *
 * @details     Suspends the controller
 *
 * @param [in]  hDevice               Handle that is returned via adi_musbmhdrc_Init
 *
 * @return      Status
 *
 *  - #ADI_USB_RESULT_SUCCESS          Upon successful suspending of the device
 *
 *  - #ADI_USB_RESULT_INVALID_HANDLE   [D] Invalid Device handle is supplied
 *
 *  - #ADI_USB_RESULT_INVALID_DEV_MODE [D] If device mode not set
 *
 * @note        
 *             Caller(s) USBH_HC_Start()
 *
 * @sa         adi_musbmhdrc_Start()
 */
static  ADI_USB_RESULT  adi_musbmhdrc_Suspend (ADI_USB_HANDLE const hDevice)
{
    ADI_MUSBMHDRC_DATA        *pUsbDrvData = (ADI_MUSBMHDRC_DATA *)hDevice;
    volatile ADI_USB_REGS_DEF *pRegs;

    ADI_USB_VALIDATE_HANDLE(hDevice);

    /* When operating as a peripheral, the MUSBMHDRC monitors activity on the USB and when no activity
     * has occurred for 3ms, it goes into Suspend mode automatically
     */
    assert(pUsbDrvData->DevMode != ADI_USB_MODE_DEVICE);
    assert(pUsbDrvData->DevMode != ADI_USB_MODE_NONE);

    pRegs = pUsbDrvData->pRegs;

    ADI_USB_ENTER_CR();
    /* Suspend the device */
    pRegs->POWER |= ENUM_USB_POWER_SUSPEND;
    ADI_USB_EXIT_CR();

    return (ADI_USB_RESULT_SUCCESS);
}


/**
 * @brief       Resume the controller
 *
 * @details     Resumes the controller
 *
 * @param [in]  hDevice               Handle that is returned via adi_musbmhdrc_Init
 *
 * @return      Status
 *
 *  - #ADI_USB_RESULT_SUCCESS          Upon successful resuming of the device
 *
 *  - #ADI_USB_RESULT_INVALID_HANDLE   [D] Invalid Device handle is supplied
 *
 *  - #ADI_USB_RESULT_INVALID_DEV_MODE [D] If device mode not set
 *
 * @note        
 *             Caller(s)  Core Layer
 *
 * @sa          adi_musbmhdrc_Start()
 */
static  ADI_USB_RESULT  adi_musbmhdrc_Resume (ADI_USB_HANDLE const hDevice)
{
    ADI_MUSBMHDRC_DATA *pUsbDrvData = (ADI_MUSBMHDRC_DATA *)hDevice;

    ADI_USB_VALIDATE_HANDLE(hDevice);

    switch(pUsbDrvData->DevMode)
    {
    /*
     * Host mode resume signal will be sent to device upon setting the resume bit.
     * With remote wake-up capability, device can send resume signal as well. In that case RESUME
     * bit is automatically set.
     *
     * In Device mode, setting this bit will send resume signal on the bus.
     */
        case ADI_USB_MODE_OTG_HOST:
         /* fall through */
        case ADI_USB_MODE_DEVICE:

        adi_usbdrv_SendResumeSignal(pUsbDrvData);
        break;

        default:
            return ADI_USB_RESULT_INVALID_DEV_MODE;
    }

    return (ADI_USB_RESULT_SUCCESS);
}


/**
 * @brief       Returns the current frame number
 *
 * @details     Returns the current frame number
 *
 * @param [in]  hDevice               Handle that is returned via adi_musbmhdrc_Init
 *
 * @return      Status
 *
 *  - #ADI_USB_RESULT_SUCCESS         Upon successful returning of Frame number
 *
 *  - #ADI_USB_RESULT_INVALID_HANDLE  [D] Invalid Device handle is supplied
 *
 * @note        
 *             Caller(s)  Core Layer
 *
 * @sa          adi_musbmhdrc_Start()
 */
static  uint32_t  adi_musbmhdrc_FrameNbrGet (ADI_USB_HANDLE const hDevice)
{
    ADI_MUSBMHDRC_DATA *pUsbDrvData = (ADI_MUSBMHDRC_DATA *)hDevice;

    ADI_USB_VALIDATE_HANDLE(hDevice);

    return pUsbDrvData->pRegs->FRAME;
}


/**
 * @brief       Opens the given endpoint and initializes endpoint
 *
 * @details     This API opens an endpoint and initializes it using the parameters specified in
 *              the endpoint descriptor.Endpoint information is maintained in endpoint 
 *              information structure (ADI_USB_EP_INFO). Endpoint information structure is dynamically
 *              allocated except for EP0. EP0 always uses first entry in the EPINFO table.ADI_USB_EP_INFO
 *              structure maintains all the required information for the endpoint.Endpoint
 *              information also maintains the currently active URB. Only one URB can be active at
 *              any point of time. URB has to be returned before accepting any new requests.
 *
 *              By using the endpoint descriptor information a physical endpoint is allocated and
 *              associated with the logical endpoint number present in the descriptor. Number of
 *              physical endpoints and their respective configurations may vary across platforms
 *              and they are maintained in ADI_CFG_USB_PHYEP_CONFIG structure.Same physical endpoint
 *              number can be used for IN and OUT types. Physical endpoint number is allocated by 
 *              using first best match mechanism using endpoint direction, type and maximum endpoint
 *              size values. EP_Close will free resources allocated for the endpoint, including the
 *              dma channels, physical endpoint and associated endpoint information entities.
 *
 *              This routine binds logical endpoint (specified in descriptor) to a physical endpoint.
 *              A dma channel is also allocated. For the controllers having dynamic parameters like FIFO
 *              they also get allocated.
 *
 *              If driver could not allocate an endpoint then it will return ADI_USB_RESULT_EP_ALLOC_FAILED
 *              failure error.
 *
 *              Default control endpoint (EP0) uses reserved entity (0) which includes the table 
 *              index and dma channel. For data and other endpoints these resources are allocated
 *              dynamically and freed upon closing the endpoint.
 *
 *
 * @param [in]  hDevice               Handle that is returned via adi_musbmhdrc_Init
 *
 * @param [in]  pEpDesc               Pointer to endpoint descriptor
 *
 * @param [in]  DevAddr               Device Address
 *
 * @return      Status
 *
 *  - #ADI_USB_RESULT_SUCCESS         Upon successful opening of the Endpoint
 *
 *  - #ADI_USB_RESULT_INVALID_HANDLE  [D] Invalid Device handle is supplied
 *
 *  - #ADI_USB_RESULT_NULL_POINTER    [D] Null parameter supplied
 *
 *  - #ADI_USB_RESULT_EP_ALLOC_FAILED [D] Endpoint not available
 *
 * @note        
 *             Caller(s)  Core Layer
 *
 * @sa          adi_musbmhdrc_EP_Close()
 */
static  ADI_USB_RESULT  adi_musbmhdrc_EP_Open (ADI_USB_HANDLE  const hDevice,
                                               ADI_USB_EP_DESC *pEpDesc,
                                               uint8_t         DevAddr)
{
    ADI_MUSBMHDRC_DATA        *pUsbDrvData = (ADI_MUSBMHDRC_DATA *)hDevice;
    ADI_USB_EP_INFO           *pEpInfo = NULL;
    ADI_USB_RESULT            Result;

    ADI_USB_VALIDATE_HANDLE(hDevice);
    ADI_USB_REPORT_ERROR   (pEpDesc, NULL, ADI_USB_RESULT_NULL_POINTER);

    ADI_USB_ENTER_CR();

    Result = adi_usbdrv_AllocEpinfo(pUsbDrvData, pEpDesc, &pEpInfo);

    if (Result != ADI_USB_RESULT_SUCCESS)
    {
        ADI_USB_EXIT_CR();
        return Result;
    }

#if defined(ADI_CFG_USB_BF_MUSBHDRC)
    if (pEpDesc->EndpointAddress == ADI_USB_EP_NUM_0)
    {
        pUsbDrvData->DevAddr  = DevAddr & 0x7F;
        pUsbDrvData->pRegs->FADDR = (DevAddr & 0x7F);
    }
#endif

#if (ADI_CFG_USB_DEVICE == 1)
    if(pEpDesc->EndpointAddress == ADI_USB_EP_NUM_0)
    {
        pEpInfo->EpDir          = ADI_USB_EP_DIR_SETUP;
    }
    else
#endif
    {
        pEpInfo->EpDir          = ADI_USB_GET_EP_DIR(pEpDesc->EndpointAddress);
    }

    pEpInfo->EpLogicalAddr  = pEpDesc->EndpointAddress;
    pEpInfo->EpType         = ADI_USB_GET_EP_TYPE(pEpDesc->Attributes);
    pEpInfo->EpMaxSize      = pEpDesc->MaxPacketSize;
    pEpInfo->DevAddr        = DevAddr;
    pEpInfo->EpInterval     = pEpDesc->Interval;
    pEpInfo->EpTransferMode = ADI_USB_EP_MODE_NONE;
    pEpInfo->pEpBufferdata  = NULL;
    pEpInfo->pEpCurURB      = NULL;
    pEpInfo->EpUrbState     = ADI_USB_URB_STATE_FREE;
    pEpInfo->EpIsHalt       = false;

    if(pEpInfo->EpPhyNum)
    {
        adi_usbdrv_BindPhysicalEndpoint(pUsbDrvData, pEpInfo);
    }
    else
    {
        /* DMA Channel 0 is assumed to be locked for EP0.
         * CSR0 and other configuration registers are assumed to be set at their Default values */
        pUsbDrvData->pRegs->EP0I_TYPE0 = 0x0;
    }

    ADI_USB_EXIT_CR();

    return ADI_USB_RESULT_SUCCESS;
}

/**
 * @brief       Close the given endpoint and releases any resources associated with it
 *
 * @details     This API is used to close and endpoint. Endpoint information is encoded in
 *              ep data. Ep data includes the endpoint address, its type and the device it
 *              is associated with. Device address is critical for multipoint operation using
 *              a hub. Multiple devices connected to a hub might have same logical endpoint
 *              numbers.
 *              
 *              Closing endpoint frees up the resources, associated endpoint structures dma
 *              channels and physical endpoint.
 *
 * @param [in]  hDevice               Handle that is returned via adi_musbmhdrc_Init
 *
 * @param [in]  EpData                Endpoint data
 *
 * @return      Status
 *
 *  - #ADI_USB_RESULT_SUCCESS         Upon successful closing of the endpoint
 *
 *  - #ADI_USB_RESULT_INVALID_HANDLE  [D] Invalid Device handle is supplied
 *
 *  - #ADI_USB_RESULT_EP_INVALID      [D] Invalid Logical Endpoint Address is supplied
 *
 * @note        
 *             Caller(s)  Core Layer
 *
 * @sa          adi_musbmhdrc_EP_Open()
 */

static  ADI_USB_RESULT  adi_musbmhdrc_EP_Close (ADI_USB_HANDLE const hDevice,
                                                const uint32_t       EpData)

{
    ADI_MUSBMHDRC_DATA        *pUsbDrvData = (ADI_MUSBMHDRC_DATA *)hDevice;
    ADI_USB_EP_INFO           *pEpInfo;
    volatile ADI_USB_REGS_DEF *pRegs;
    uint16_t                  SavedEpNum;

    ADI_USB_VALIDATE_HANDLE(hDevice);

    pEpInfo         =  adi_usbdrv_GetEpInfo(pUsbDrvData, EpData);

    assert(pEpInfo != NULL);
    ADI_USB_REPORT_ERROR (pEpInfo, NULL, ADI_USB_RESULT_NULL_POINTER);

    pRegs      = pUsbDrvData->pRegs;
    SavedEpNum = pRegs->INDEX;

    ADI_USB_ENTER_CR();

    pRegs->INDEX = pEpInfo->EpPhyNum;

    if(pEpInfo->EpPhyNum == ADI_USB_EP_NUM_0)
    {
        pEpInfo->EpProcBytesThisPass = 0;
        adi_usbdrv_ResetEndpointZero(pUsbDrvData);
    }
    else
    {
        adi_usbdrv_FreeDmaChannel(pUsbDrvData,pEpInfo);
        adi_usbdrv_ResetPhysicalEndpoint(pUsbDrvData, pEpInfo);
        adi_usbdrv_ClearEpInfo(pUsbDrvData, EpData);
        memset(pEpInfo, 0, sizeof(ADI_USB_EP_INFO));
    }

    pRegs->INDEX = SavedEpNum;

    ADI_USB_EXIT_CR();

    return (ADI_USB_RESULT_SUCCESS);
}


/**
 * @brief       Aborts the currently pending endpoint transaction
 *
 * @details     Aborts the currently pending endpoint transaction. Abort frees up
 *              associated URB and resets the DMA. This allows further urb submissions
 *              to the endpoint.
 *
 *              Abort also flushes the fifo for the associated endpoint.
 *
 * @param [in]  hDevice               Handle that is returned via adi_musbmhdrc_Init
 *
 * @param [in]  EpData                Endpoint data
 *
 * @return      Status
 *
 *  - #ADI_USB_RESULT_SUCCESS         Upon successful aborting of the endpoint
 *
 *  - #ADI_USB_RESULT_INVALID_HANDLE  [D] Invalid Device handle is supplied
 *
 *  - #ADI_USB_RESULT_EP_INVALID      [D] Invalid Logical Endpoint Address is supplied
 *
 * @note        
 *             Caller(s)  Core Layer
 *
 * @sa          adi_musbmhdrc_EP_Open()
 * @sa          adi_musbmhdrc_EP_Close()
 */
static  ADI_USB_RESULT  adi_musbmhdrc_EP_Abort (ADI_USB_HANDLE const hDevice,
                                                const uint32_t       EpData)
{
    ADI_MUSBMHDRC_DATA        *pUsbDrvData = (ADI_MUSBMHDRC_DATA *)hDevice;
    ADI_USB_EP_INFO           *pEpInfo;
    volatile ADI_USB_REGS_DEF *pRegs;
    uint16_t                  SavedEpNum;

    ADI_USB_VALIDATE_HANDLE(hDevice);

    pEpInfo         =  adi_usbdrv_GetEpInfo(pUsbDrvData, EpData);
    assert(pEpInfo != NULL);

    ADI_USB_REPORT_ERROR (pEpInfo, NULL, ADI_USB_RESULT_NULL_POINTER);

    pRegs      = pUsbDrvData->pRegs;
    SavedEpNum = pRegs->INDEX;

    ADI_USB_ENTER_CR();

    pRegs->INDEX = pEpInfo->EpPhyNum;

    /* Reset DMA */
    adi_usbdrv_ResetDma(pUsbDrvData, pEpInfo);

    /* Flush EP fifo */
    if(pEpInfo->EpPhyNum == ADI_USB_EP_NUM_0)
    {
        adi_usbdrv_FlushEp0Fifo(pRegs);
    }
    else
    {
        if(pEpInfo->EpDir == ADI_USB_EP_DIR_RX)
        {
            pRegs->INTRRXE &= ~(1ul << (pEpInfo->EpPhyNum));
#if defined(ADI_CFG_USB_BF_MUSBHDRC)
            pRegs->INTRRX = (1u << pEpInfo->EpPhyNum);
#endif
            adi_usbdrv_FlushRxFifo(pRegs);
        }
        else
        {
            pRegs->INTRTXE &= ~(1ul << (pEpInfo->EpPhyNum));
#if defined(ADI_CFG_USB_BF_MUSBHDRC)
            pRegs->INTRTX = (1u << pEpInfo->EpPhyNum);
#endif
            adi_usbdrv_FlushTxFifo(pRegs);
            pEpInfo->EpTxDmaPend = false;
        }
    }

    adi_usbdrv_ResetCurrentURB(pEpInfo);

    pRegs->INDEX = SavedEpNum;

    ADI_USB_EXIT_CR();

    return (ADI_USB_RESULT_SUCCESS);
}


/**
 * @brief       Checks the endpoint halt condition
 *
 * @details     Checks the endpoint halt condition
 *
 * @param [in]  hDevice               Handle that is returned via adi_musbmhdrc_Init
 *
 * @param [in]  EpData                Logical Endpoint Address
 *
 * @return      Status
 *
 *  - 'true'                          Endpoint is halted
 *
 *  - 'false'                         Endpoint is not halted
 *
 *  - #ADI_USB_RESULT_INVALID_HANDLE  [D] Invalid Device handle is supplied
 *
 *  - #ADI_USB_RESULT_EP_INVALID      [D] Invalid Logical Endpoint Address is supplied
 *
 * @note        
 *             Caller(s)  Core Layer
 *
 * @sa          adi_musbmhdrc_EP_Open()
 * @sa          adi_musbmhdrc_EP_Close()
 */
static  uint8_t  adi_musbmhdrc_IsHalt_EP (ADI_USB_HANDLE const hDevice,
                                          const uint32_t       EpData)
{
    ADI_MUSBMHDRC_DATA        *pUsbDrvData = (ADI_MUSBMHDRC_DATA *)hDevice;
    ADI_USB_EP_INFO           *pEpInfo;

    ADI_USB_VALIDATE_HANDLE(hDevice);
    pEpInfo =  adi_usbdrv_GetEpInfo(pUsbDrvData, EpData);

    /* There might be case that in some other thread given pEpInfo is cleared but
     * stack is still try to check whether this Endpoint is halted or not */
    if (pEpInfo == NULL)
    {
        return (ADI_USB_RESULT_EP_INVALID);
    }

    return pEpInfo->EpIsHalt;
}


/**
 * @brief       Reset the Endpoint data toggle bit
 *
 * @details     Reset the Endpoint data toggle bit. By resetting data toggle communication
 *              resets and starts on DATA0. Certain commands like set and clear feature
 *              requires data toggle to be reset to DATA0.
 *          
 * @param [in]  hDevice               Handle that is returned via adi_musbmhdrc_Init
 *
 * @param [in]  EpData                Logical Endpoint Address
 *
 * @return      Status
 *
 *  - #ADI_USB_RESULT_SUCCESS         Upon successful reset of EP data toggle bit
 *
 * @note
 *             Caller(s)  Core Layer
 *
 * @sa          adi_musbmhdrc_EP_Open()
 * @sa          adi_musbmhdrc_EP_Close()
 */
static ADI_USB_RESULT adi_musbmhdrc_EP_ResetDataToggle(ADI_USB_HANDLE const hDevice, const uint32_t EpData)
{
    ADI_MUSBMHDRC_DATA        *pUsbDrvData = (ADI_MUSBMHDRC_DATA *)hDevice;
    volatile ADI_USB_REGS_DEF *pRegs;
    uint16_t                  SavedEpNum;
    int32_t                   EpPhyNum;
    uint8_t                   EpPhyIdx;

    ADI_USB_VALIDATE_HANDLE(hDevice);
    EpPhyNum =  adi_usbdrv_GetEpPhyNum(pUsbDrvData, EpData, &EpPhyIdx);

    assert(EpPhyNum != -1);
    ADI_USB_EP_STATINC(pUsbDrvData,EpPhyIdx,nEPResetDataToggle);

    pRegs = pUsbDrvData->pRegs;

    ADI_USB_ENTER_CR();
    SavedEpNum   = pRegs->INDEX;
    pRegs->INDEX = EpPhyNum;

    if(EpPhyNum)
    {
        if(ADI_USB_GET_EP_DIR(EpData) == ADI_USB_EP_DIR_RX)
        {
            /* clear the data toggle bit */
            pRegs->EPI_RXCSR |= ENUM_USB_EPI_RXCSR_H_CLRTGL;
        }
        else
        {
            /* clear the data toggle bit */
            pRegs->EPI_TXCSR |= ENUM_USB_EPI_TXCSR_H_CLRTGL;
        }
    }
    else
    {
        /* clear the data toggle bit */
        pRegs->EP0_CSR0 |= ENUM_USB_EP0I_CSR_H_DATATGL;
    }

    pRegs->INDEX = SavedEpNum;
    ADI_USB_EXIT_CR();

    return ADI_USB_RESULT_SUCCESS;
}


/**
 * @brief       Sets the device address
 *
 * @details     Sets the device address
 *
 * @param [in]  hDevice               Handle that is returned via adi_musbmhdrc_Init
 *
 * @param [in]  DevAddr               Device Address
 *
 * @return      Status
 *
 *  - #ADI_USB_RESULT_SUCCESS         Upon successful setting of Device address
 *
 * @note
 *             Caller(s)  Core Layer
 *
 * @sa          adi_musbmhdrc_EP_Open()
 * @sa          adi_musbmhdrc_EP_Close()
 */
static ADI_USB_RESULT adi_musbmhdrc_SetDevAddress(ADI_USB_HANDLE const hDevice, uint8_t DevAddr)
{
    ADI_MUSBMHDRC_DATA *pUsbDrvData = (ADI_MUSBMHDRC_DATA *)hDevice;

    ADI_USB_VALIDATE_HANDLE(hDevice);

#if !defined(__ADSPSC589_FAMILY__)                /* Not Griffin */
    adi_usbdrv_WaitMilliSec(2u);
#endif	

    /* Set the device address */
    pUsbDrvData->pRegs->FADDR = (DevAddr & 0x7F);

    pUsbDrvData->DevAddr  = DevAddr;
    pUsbDrvData->DevState = ADI_USB_DEV_STATE_ADDRESSED;

    return ADI_USB_RESULT_SUCCESS;
}

#if (ADI_CFG_USB_DEVICE == 1)
/**
 * @brief       Set or clears the stall bit for the given endpoint
 *
 * @details     Sets or clears stall bit for the given endpoint
 *
 * @param [in]  hDevice               Handle that is returned via adi_musbmhdrc_Init
 *
 * @param [in]  EpData                Logical Endpoint Address
 *
 * @param [in]  bEpStall              Specifies whether to stall or clear stall
 *
 * @return      Status
 *
 *  - #ADI_USB_RESULT_SUCCESS         Upon successful reset of EP data toggle bit
 *
 * @note
 *             Caller(s)  Core Layer
 *
 * @sa          adi_musbmhdrc_EP_Open()
 * @sa          adi_musbmhdrc_EP_Close()
 */
static ADI_USB_RESULT adi_musbmhdrc_EP_Stall(ADI_USB_HANDLE const hDevice, const uint32_t EpData,const bool bEpStall)
{
    ADI_MUSBMHDRC_DATA        *pUsbDrvData = (ADI_MUSBMHDRC_DATA *)hDevice;
    volatile ADI_USB_REGS_DEF *pRegs;
    uint16_t                  SavedEpNum;
    ADI_USB_EP_INFO           *pEpInfo;

    ADI_USB_VALIDATE_HANDLE(hDevice);

    ADI_USB_ENTER_CR();

    pEpInfo = adi_usbdrv_GetEpInfo(pUsbDrvData, EpData);

    assert(pEpInfo != NULL);

    pRegs = pUsbDrvData->pRegs;

    SavedEpNum   = pRegs->INDEX;
    pRegs->INDEX = pEpInfo->EpPhyNum;

    if (bEpStall)
    {
        if(pEpInfo->EpPhyNum)
        {
            if(ADI_USB_GET_EP_DIR(EpData) == ADI_USB_EP_DIR_RX)
            {
                pRegs->EPI_RXCSR |= ENUM_USB_EPI_RXCSR_P_STALL;
            }
            else
            {
                pRegs->EPI_TXCSR |= ENUM_USB_EPI_TXCSR_P_STALL;
            }
        }
        else
        {
#if (ADI_CFG_USB_OTG_HOST == 1)
                pRegs->EP0_CSR0 |= ENUM_USB_EP0I_CSR_H_RXSTALL;
#else
                if (pUsbDrvData->EpZeroSetupStatus == ADI_USB_SS_PENDING_TX_NDP_ZLP)
                {
                     pRegs->EP0_CSR0 |= (ENUM_USB_EP0I_CSR_P_SPKTRDY  | ENUM_USB_EP0I_CSR_P_DATAEND);
                     pUsbDrvData->EpZeroSetupStatus = ADI_USB_SS_IDLE;
                     pEpInfo->EpUrbState            = ADI_USB_URB_STATE_FREE;
                     pUsbDrvData->EpZeroSetupState = ADI_USB_SETUP_IDLE;
                }                
                pRegs->EP0_CSR0 |= (1ul << 5);
#endif
        }
        pEpInfo->EpIsHalt = true;
    }
    else
    {
        if(pEpInfo->EpPhyNum)
        {
            if(ADI_USB_GET_EP_DIR(EpData) == ADI_USB_EP_DIR_RX)
            {
#if (ADI_CFG_USB_DEVICE == 1)
                pRegs->EPI_RXCSR &= ~ENUM_USB_EPI_RXCSR_P_STALL;
                pRegs->EPI_RXCSR |= ENUM_USB_EPI_RXCSR_P_CLRTGL;
#endif
                pRegs->EPI_RXCSR &= ~ENUM_USB_EPI_RXCSR_P_STALSNT;
            }
            else
            {
#if (ADI_CFG_USB_DEVICE == 1)
                pRegs->EPI_TXCSR &= ~ENUM_USB_EPI_TXCSR_P_STALL;
                pRegs->EPI_TXCSR |= ENUM_USB_EPI_TXCSR_P_CLRTGL;
#endif
                pRegs->EPI_TXCSR &= ~ENUM_USB_EPI_TXCSR_P_STALSNT;

            }
        }
        else
        {
#if (ADI_CFG_USB_OTG_HOST == 1)
                pRegs->EP0_CSR0 &= ~ENUM_USB_EP0I_CSR_H_RXSTALL;
#else
                pRegs->EP0_CSR0 &= ~(1ul << 5);
#endif
        }

        pEpInfo->EpIsHalt = false;
    }

    pRegs->INDEX = SavedEpNum;
    ADI_USB_EXIT_CR();

    return ADI_USB_RESULT_SUCCESS;
}
#endif

/**
 * @brief       Submits the USB request packet (URB)
 *
 * @details     This API is used to send or receive data.Stack submits usb request packets
 *              to receive or transmit data. All urb requests are handled asynchronously 
 *              at the driver level. Interface or stack may synchronously block or wait on 
 *              asynchronous event.
 *
 *              Only one URB can be active at any juncture, so once a request is completed or
 *              aborted then only next urb can be submitted. If there is a pending urb then
 *              any further submission requests will be rejected with an error.
 *
 *              Depending on the transfer type and length dma operation mode is set. DMA mode 1
 *              is used for optimal transfers where ever applicable. Buffer completion is indicated
 *              via callback. This API schedules dma transfer and returns. DMA interrupt in conjunction
 *              with TX or RX interrupt handlers will return the completed or aborted(error) urb.
 *
 * @param [in]  hDevice               Handle that is returned via adi_musbmhdrc_Init
 *
 * @param [in]  pUsbUrb               Pointer to USB request packet
 *
 * @return      Status
 *
 *  - #ADI_USB_RESULT_SUCCESS         Upon successful submitting of URB
 *
 *  - #ADI_USB_RESULT_INVALID_HANDLE  [D] Invalid Device handle is supplied
 *
 *  - #ADI_USB_RESULT_EP_INVALID      [D] Invalid URB is supplied
 *
 *  - #ADI_USB_RESULT_URB_ABORT       [D] URB aborted
 *
 * @note        
 *             Caller(s)  Core Layer
 *
 * @sa          adi_musbmhdrc_EP_Open()
 * @sa          adi_musbmhdrc_EP_Close()
 */
static  ADI_USB_RESULT  adi_musbmhdrc_SubmitURB (ADI_USB_HANDLE const hDevice,
                                                 ADI_USB_URB    *pUsbUrb)
{
    ADI_MUSBMHDRC_DATA *pUsbDrvData = (ADI_MUSBMHDRC_DATA *)hDevice;
    ADI_USB_EP_INFO    *pEpInfo;
    ADI_USB_RESULT     Result=ADI_USB_RESULT_FAIL;

    ADI_USB_VALIDATE_HANDLE(hDevice);

    if(pUsbDrvData->DevState == ADI_USB_DEV_STATE_DISCON)
    {
        return ADI_USB_RESULT_URB_ABORT;
    }

    assert(adi_usbdrv_IsDataAligned((uint32_t)pUsbUrb->pData,pUsbDrvData->CacheOn,pUsbUrb->EpDir));

    pEpInfo = adi_usbdrv_GetEpInfo(pUsbDrvData, (pUsbUrb->EpAddr                   |
                                                ADI_USB_GET_EPTYPE_FRMURB(pUsbUrb) |
                                                ADI_USB_GET_DEVADDR_FRMURB(pUsbUrb)));

    assert(pEpInfo != NULL);
    assert(pUsbUrb != NULL);

    ADI_USB_REPORT_ERROR (pEpInfo, NULL, ADI_USB_RESULT_NULL_POINTER);

    ADI_USB_ENTER_CR();

    /* Check if any URB is pending on this Endpoint */
    if(pEpInfo->EpUrbState != ADI_USB_URB_STATE_FREE)
    {
        ADI_USB_EXIT_CR();
        return ADI_USB_RESULT_URB_PENDING;
    }

    pEpInfo->EpDir         = pUsbUrb->EpDir;
    pEpInfo->pEpBufferdata = pUsbUrb->pData;
    pEpInfo->pEpCurURB     = pUsbUrb;

#if (ADI_CFG_USB_DEVICE == 1)
    pEpInfo->pEpCurURB->EpResult = ADI_USB_RESULT_SUCCESS;
#endif

    adi_usbdrv_SetTransferMode(pUsbDrvData, pEpInfo);

    /* In PIO mode, the flush will be NOP */
    if(pUsbDrvData->CacheOn)
    {
        adi_usbdrv_FlushCachedBuffer(pEpInfo->pEpBufferdata, pEpInfo->pEpCurURB->TransferLen, pEpInfo->EpDir);
        SSYNC();
    }

#if (ADI_USBH_MULTI_POINT_EN == 1)
    if (pEpInfo->EpPhyNum == 0)
    {
        pUsbDrvData->pRegs->MPRegs[0].MP_TXFUNCADDR = pEpInfo->DevAddr & 0x7F;
    }
#endif

    if(pEpInfo->EpDir == ADI_USB_EP_DIR_RX)
    {
        /* This will Submit buffer for EP0 RX and Data RX */
        Result = adi_usbdrv_SubmitRxBuffer(pUsbDrvData, pEpInfo);
    }
#if (ADI_CFG_USB_OTG_HOST == 1)
    else
    {
        /* This will Submit EP0 Setup, EP0 TX and Data TX buffer */
        Result = adi_usbdrv_SubmitTxBuffer(pUsbDrvData, pEpInfo);
    }
#else
    else if (pEpInfo->EpDir == ADI_USB_EP_DIR_TX)
    {
        /* This will Submit EP0 Setup, EP0 TX and Data TX buffer */
        Result = adi_usbdrv_SubmitTxBuffer(pUsbDrvData, pEpInfo);
    }
    else
    {
#if defined(ADI_DEBUG)
        ADI_USB_EP_STATINC(pUsbDrvData,pEpInfo->EpPhyIdx,nIncorrectDir);  /* FIXIT: To be removed after final testing */
#endif
    }
#endif
    ADI_USB_EXIT_CR();
    return Result;
}


/**
 * @brief       Complete the USB request packet (URB)
 *
 * @details     Submits the USB request packet
 *
 * @param [in]  hDevice               Handle that is returned via adi_musbmhdrc_Init
 *
 * @param [in]  pUsbUrb               Pointer to USB request packet
 *
 * @return      Status
 *
 *  - #ADI_USB_RESULT_SUCCESS         Upon successful completion of URB
 *
 *  - #ADI_USB_RESULT_INVALID_HANDLE  [D] Invalid Device handle is supplied
 *
 *  - #ADI_USB_RESULT_EP_INVALID      [D] Invalid URB is supplied
 *
 * @note
 *             Caller(s)  Core Layer
 *
 * @sa          adi_musbmhdrc_EP_Open()
 * @sa          adi_musbmhdrc_EP_Close()
 */

static  ADI_USB_RESULT  adi_musbmhdrc_CompleteURB (ADI_USB_HANDLE const hDevice,
                                                   ADI_USB_URB    *pUsbUrb)
{
    ADI_MUSBMHDRC_DATA *pUsbDrvData = (ADI_MUSBMHDRC_DATA *)hDevice;
    ADI_USB_EP_INFO    *pEpInfo;

    ADI_USB_VALIDATE_HANDLE(hDevice);

    pEpInfo = adi_usbdrv_GetEpInfo(pUsbDrvData, (pUsbUrb->EpAddr                   |
                                                ADI_USB_GET_EPTYPE_FRMURB(pUsbUrb) |
                                                ADI_USB_GET_DEVADDR_FRMURB(pUsbUrb)));


    ADI_USB_REPORT_ERROR (pEpInfo, NULL, ADI_USB_RESULT_NULL_POINTER);
    pEpInfo->pEpCurURB = NULL;

    return ADI_USB_RESULT_SUCCESS;
}


/**
 * @brief       Aborts pending URB
 *
 * @details     Aborts pending URB
 *
 * @param [in]  hDevice               Handle that is returned via adi_musbmhdrc_Init
 *
 * @param [in]  pUsbUrb               Pointer to USB request packet
 *
 * @return      Status
 *
 *  - #ADI_USB_RESULT_SUCCESS         Upon successful abort of pending URB
 *
 *  - #ADI_USB_RESULT_INVALID_HANDLE  [D] Invalid Device handle is supplied
 *
 *  - #ADI_USB_RESULT_EP_INVALID      [D] Invalid URB is supplied
 *
 * @note        
 *             Caller(s)  Core Layer
 *
 * @sa          adi_musbmhdrc_SubmitURB()
 */
static  ADI_USB_RESULT  adi_musbmhdrc_AbortURB (ADI_USB_HANDLE const hDevice,
                                                ADI_USB_URB    *pUsbUrb)
{
    ADI_MUSBMHDRC_DATA *pUsbDrvData = (ADI_MUSBMHDRC_DATA *)hDevice;
    ADI_USB_EP_INFO    *pEpInfo;

    ADI_USB_VALIDATE_HANDLE(hDevice);
    assert(pUsbUrb != NULL);

    pEpInfo = adi_usbdrv_GetEpInfo(pUsbDrvData, (pUsbUrb->EpAddr                   |
                                                ADI_USB_GET_EPTYPE_FRMURB(pUsbUrb) |
                                                ADI_USB_GET_DEVADDR_FRMURB(pUsbUrb)));
    assert(pEpInfo != NULL);

    ADI_USB_REPORT_ERROR (pEpInfo, NULL, ADI_USB_RESULT_NULL_POINTER);

    ADI_USB_ENTER_CR();
    pEpInfo->EpTransferMode      = ADI_USB_EP_MODE_NONE;
    pEpInfo->pEpCurURB           = NULL;
    pEpInfo->EpProcBytesThisPass = 0;
    pEpInfo->pEpBufferdata       = NULL;
    pEpInfo->EpUrbState          = ADI_USB_URB_STATE_FREE;
    ADI_USB_EXIT_CR();

    return (ADI_USB_RESULT_SUCCESS);
}

static  ADI_URB_STATE  adi_musbmhdrc_GetURBState (ADI_USB_HANDLE const hDevice,
                                                   const uint32_t    EpData)
{
    ADI_MUSBMHDRC_DATA *pUsbDrvData = (ADI_MUSBMHDRC_DATA *)hDevice;
    ADI_USB_EP_INFO    *pEpInfo;

    pEpInfo = adi_usbdrv_GetEpInfo(pUsbDrvData, EpData);
    assert(pEpInfo != NULL);

    return pEpInfo->EpUrbState;
}

#if (ADI_CFG_USB_OTG_HOST == 1)
/*******************************************************************************
 *
 *   ROOT HUB FUNCTIONS
 */

/**
 * @brief       Port Status Get the port status
 *
 * @details     Returns the port status
 *
 * @param [in]  hDevice               Handle that is returned via adi_musbmhdrc_Init
 *
 * @param [in]  PortNumber            Port number
 *
 * @param [in]  pPortStatus           Pointer to the port status
 *
 * @return      Status
 *
 *  - #ADI_USB_RESULT_SUCCESS         Upon successful return of the port status
 *
 *  - #ADI_USB_RESULT_INVALID_HANDLE  [D] Invalid Device handle is supplied
 *
 * @note        
 *             Caller(s)  Core Layer
 *
 * @sa         adi_musbmhdrc_PortEnSet()
 */
static  ADI_USB_RESULT  adi_musbmhdrc_PortStatusGet (ADI_USB_HANDLE const    hDevice,
                                                     const uint32_t          PortNumber,
                                                     ADI_USB_HUB_PORTSTATUS  *pPortStatus)
{
    ADI_MUSBMHDRC_DATA *pUsbDrvData = (ADI_MUSBMHDRC_DATA *)hDevice;

    ADI_USB_VALIDATE_HANDLE(hDevice);
    pPortStatus->PortStatus = pUsbDrvData->RHPortStatus[PortNumber].PortStatus;
    pPortStatus->PortChange = pUsbDrvData->RHPortStatus[PortNumber].PortChange;

    return ADI_USB_RESULT_SUCCESS;
}


/**
 * @brief       Returns the hub descriptor
 *
 * @details     Returns the hub descriptor
 *
 * @param [in]  hDevice               Handle that is returned via adi_musbmhdrc_Init
 *
 * @param [in]  pBuf                  Pointer to the buffer
 *
 * @param [in]  BufferLen             Length of the buffer
 *
 * @return      Status
 *
 *  - #ADI_USB_RESULT_SUCCESS         Upon successful return of the Hub descriptor
 *
 *  - #ADI_USB_RESULT_INVALID_HANDLE  [D] Invalid Device handle is supplied
 *
 * @note        
 *             Caller(s)  Core Layer
 *
 * @sa          adi_musbmhdrc_SubmitURB()
 */
static  ADI_USB_RESULT  adi_musbmhdrc_HubDescGet (ADI_USB_HANDLE const  hDevice,
                                                  void           *pBuf,
                                                  const uint32_t BufLen)
{
    return ADI_USB_RESULT_SUCCESS;
}


/**
 * @brief       Port enable set
 *
 * @details     
 *
 * @param [in]  hDevice               Handle that is returned via adi_musbmhdrc_Init
 *
 * @param [in]  PortNumber            Port number
 *
 * @return      Status
 *
 *  - #ADI_USB_RESULT_SUCCESS         Upon successful enabling of the port
 *
 *  - #ADI_USB_RESULT_INVALID_HANDLE  [D] Invalid Device handle is supplied
 *
 * @note        
 *             Caller(s)  USBH_HUB_RH_CtrlReq()
 *
 * @sa         adi_musbmhdrc_SubmitURB()
 */

static  ADI_USB_RESULT  adi_musbmhdrc_PortEnSet (ADI_USB_HANDLE const   hDevice,
                                                 const uint32_t PortNumber)
{
    ADI_MUSBMHDRC_DATA *pUsbDrvData = (ADI_MUSBMHDRC_DATA *)hDevice;

    ADI_USB_VALIDATE_HANDLE(hDevice);
    /* Set the enable bits */
    pUsbDrvData->RHPortStatus[PortNumber].PortStatus |= ADI_USBH_HUB_STATUS_PORT_EN;
    pUsbDrvData->RHPortStatus[PortNumber].PortChange |= ADI_USBH_HUB_STATUS_C_PORT_EN;

    return ADI_USB_RESULT_SUCCESS;
}


/**
 * @brief       Clear port enable status
 *
 * @details     Clear port enable status
 *
 * @param [in]  hDevice               Handle that is returned via adi_musbmhdrc_Init
 *
 * @param [in]  PortNumber            Port Number
 *
 * @return      Status
 *
 *  - #ADI_USB_RESULT_SUCCESS         Upon successful clearing of the port enable status
 *
 *  - #ADI_USB_RESULT_INVALID_HANDLE  [D] Invalid Device handle is supplied
 *
 * @note        
 *             Caller(s)  USBH_HUB_RH_CtrlReq()
 *
 * @sa          adi_musbmhdrc_PortEnSet()
 */
static  ADI_USB_RESULT  adi_musbmhdrc_PortEnClr (ADI_USB_HANDLE const hDevice,
                                                 const uint32_t PortNumber)
{
    ADI_MUSBMHDRC_DATA *pUsbDrvData = (ADI_MUSBMHDRC_DATA *)hDevice;
    ADI_USB_VALIDATE_HANDLE(hDevice);

    /* Clear the enable bit */
    pUsbDrvData->RHPortStatus[PortNumber].PortStatus &= ~ADI_USBH_HUB_STATUS_PORT_EN;

    return (ADI_USB_RESULT_SUCCESS);
}


/**
 * @brief      Clear port enable status change
 *
 * @details     Returns the hub descriptor
 *
 * @param [in]  hDevice               Handle that is returned via adi_musbmhdrc_Init
 *
 * @param [in]  PortNumber            Port number
 *
 * @return      Status
 *
 *  - #ADI_USB_RESULT_SUCCESS         Upon successful clearing of the port status change
 *
 *  - #ADI_USB_RESULT_INVALID_HANDLE  [D] Invalid Device handle is supplied
 *
 * @note        
 *             Caller(s)  USBH_HUB_RH_CtrlReq()
 *
 * @sa          adi_musbmhdrc_SubmitURB()
 */
static  ADI_USB_RESULT  adi_musbmhdrc_PortEnChngClr (ADI_USB_HANDLE const hDevice,
                                                     const uint32_t PortNumber)
{
    ADI_MUSBMHDRC_DATA *pUsbDrvData = (ADI_MUSBMHDRC_DATA *)hDevice;

    ADI_USB_VALIDATE_HANDLE(hDevice);
    /* Clear the enable bit */
    pUsbDrvData->RHPortStatus[PortNumber].PortChange &= ~ADI_USBH_HUB_STATUS_C_PORT_EN;

    return (ADI_USB_RESULT_SUCCESS);
}


/**
 * @brief       Set port power based on the power mode
 *
 * @details     Set port power based on the power mode
 *
 * @param [in]  hDevice               Handle that is returned via adi_musbmhdrc_Init
 *
 * @param [in]  PortNumber            Port number
 *
 * @return      Status
 *
 *  - #ADI_USB_RESULT_SUCCESS         Upon successful setting of port power
 *
 *  - #ADI_USB_RESULT_INVALID_HANDLE  [D] Invalid Device handle is supplied
 *
 * @note        
 *             Caller(s)  USBH_HUB_RH_CtrlReq()
 *
 * @sa          adi_musbmhdrc_PortPwrClr()
 */
static  ADI_USB_RESULT  adi_musbmhdrc_PortPwrSet (ADI_USB_HANDLE const hDevice,
                                                  const uint32_t PortNumber)
{
    ADI_MUSBMHDRC_DATA *pUsbDrvData = (ADI_MUSBMHDRC_DATA *)hDevice;
    ADI_USB_VALIDATE_HANDLE(hDevice);

    /* Set the Power bit */
    pUsbDrvData->RHPortStatus[PortNumber].PortStatus |= ADI_USBH_HUB_STATUS_PORT_PWR;

    return (ADI_USB_RESULT_SUCCESS);
}



/**
 * @brief       Clear port power
 *
 * @details     Clear port power
 *
 * @param [in]  hDevice               Handle that is returned via adi_musbmhdrc_Init
 *
 * @param [in]  PortNumber            Port number
 *
 * @return      Status
 *
 *  - #ADI_USB_RESULT_SUCCESS         Upon successful clearing of the port power status
 *
 *  - #ADI_USB_RESULT_INVALID_HANDLE  [D] Invalid Device handle is supplied
 *
 * @note        
 *             Caller(s)  USBH_HUB_RH_CtrlReq
 *
 * @sa          adi_musbmhdrc_PortPwrSet()
 */
static  ADI_USB_RESULT  adi_musbmhdrc_PortPwrClr (ADI_USB_HANDLE const hDevice,
                                                  const uint32_t PortNumber)
{
    ADI_MUSBMHDRC_DATA *pUsbDrvData = (ADI_MUSBMHDRC_DATA *)hDevice;

    ADI_USB_VALIDATE_HANDLE(hDevice);
    /* Clear the Power bit */
    pUsbDrvData->RHPortStatus[PortNumber].PortStatus &= ~ADI_USBH_HUB_STATUS_PORT_PWR;

    return ADI_USB_RESULT_SUCCESS;
}

/**
 * @brief       Reset port
 *
 * @details     Reset port
 *
 * @param [in]  hDevice               Handle that is returned via adi_musbmhdrc_Init
 *
 * @param [in]  PortNumber            Port number
 *
 * @return      Status
 *
 *  - #ADI_USB_RESULT_SUCCESS         Upon successful resetting of the port
 *
 *  - #ADI_USB_RESULT_INVALID_HANDLE  [D] Invalid Device handle is supplied
 *
 * @note        
 *             Caller(s)  Core Layer
 *
 * @sa          adi_musbmhdrc_SubmitURB()
 */
static  ADI_USB_RESULT  adi_musbmhdrc_PortResetSet (ADI_USB_HANDLE const hDevice,
                                                    const uint32_t PortNumber)
{
    ADI_MUSBMHDRC_DATA *pUsbDrvData = (ADI_MUSBMHDRC_DATA *)hDevice;

    ADI_USB_VALIDATE_HANDLE(hDevice);
    /* Set Reset bits */
    pUsbDrvData->RHPortStatus[PortNumber].PortStatus |= ADI_USBH_HUB_STATUS_PORT_RESET;
    pUsbDrvData->RHPortStatus[PortNumber].PortChange |= ADI_USBH_HUB_STATUS_C_PORT_RESET;

    return (ADI_USB_RESULT_SUCCESS);
}


/**
 * @brief       Clear port reset status change
 *
 * @details     Clear port reset status change
 *
 * @param [in]  hDevice               Handle that is returned via adi_musbmhdrc_Init
 *
 * @param [in]  PortNumber            Port number
 *
 * @return      Status
 *
 *  - #ADI_USB_RESULT_SUCCESS         Upon successful clearing of port reset status change
 *
 *  - #ADI_USB_RESULT_INVALID_HANDLE  [D] Invalid Device handle is supplied
 *
 * @note        
 *             Caller(s)  USBH_HUB_RH_CtrlReq()
 *
 * @sa          adi_musbmhdrc_PortResetSet()
 */
static  ADI_USB_RESULT  adi_musbmhdrc_PortResetChngClr (ADI_USB_HANDLE const hDevice,
                                                        const uint32_t PortNumber)
{
    ADI_MUSBMHDRC_DATA *pUsbDrvData = (ADI_MUSBMHDRC_DATA *)hDevice;

    ADI_USB_VALIDATE_HANDLE(hDevice);
    /* Clear port reset status change bit */
    pUsbDrvData->RHPortStatus[PortNumber].PortChange &= ~ADI_USBH_HUB_STATUS_C_PORT_RESET;

    return ADI_USB_RESULT_SUCCESS;
}

/**
 * @brief       Resume suspended port
 *
 * @details     Resume suspended port
 *
 * @param [in]  hDevice               Handle that is returned via adi_musbmhdrc_Init
 *
 * @param [in]  PortNumber            Port number
 *
 * @return      Status
 *
 *  - #ADI_USB_RESULT_SUCCESS         Upon successful resuming of port
 *
 *  - #ADI_USB_RESULT_INVALID_HANDLE  [D] Invalid Device handle is supplied
 *
 * @note        
 *             Caller(s)  USBH_HUB_RH_CtrlReq
 *
 * @sa          adi_musbmhdrc_PortResetChngClr()
 */
static  ADI_USB_RESULT  adi_musbmhdrc_PortSuspendClr (ADI_USB_HANDLE const hDevice,
                                                      const uint32_t PortNumber)
{
    ADI_MUSBMHDRC_DATA *pUsbDrvData = (ADI_MUSBMHDRC_DATA *)hDevice;

    ADI_USB_VALIDATE_HANDLE(hDevice);
    /* Clear Suspend bit */
    pUsbDrvData->RHPortStatus[PortNumber].PortChange &= ~ADI_USBH_HUB_STATUS_C_PORT_SUSPEND;

    return ADI_USB_RESULT_SUCCESS;
}


/**
 * @brief       Clear port connect status change
 *
 * @details     Clear port connect status change
 *
 * @param [in]  hDevice               Handle that is returned via adi_musbmhdrc_Init
 *
 * @param [in]  PortNumber            Port number
 *
 * @return      Status
 *
 *  - #ADI_USB_RESULT_SUCCESS         Upon successful clearing of port connect status change
 *
 *  - #ADI_USB_RESULT_INVALID_HANDLE  [D] Invalid Device handle is supplied
 *
 * @note        
 *             Caller(s)  Core Layer
 *
 * @sa          adi_musbmhdrc_SubmitURB()
 */
static  ADI_USB_RESULT  adi_musbmhdrc_PortConnChngClr (ADI_USB_HANDLE const hDevice,
                                                       const uint32_t PortNumber)
{
    ADI_MUSBMHDRC_DATA *pUsbDrvData = (ADI_MUSBMHDRC_DATA *)hDevice;

    ADI_USB_VALIDATE_HANDLE(hDevice);

    /* Clear port connect status change */
    pUsbDrvData->RHPortStatus[PortNumber].PortChange &= ~ADI_USBH_HUB_STATUS_C_PORT_CONN;

     return ADI_USB_RESULT_SUCCESS;
}

/**
 * @brief       Enable root hub interrupt.
 *
 * @details     Enable root hub interrupt.
 *
 * @param [in]  hDevice               Handle that is returned via adi_musbmhdrc_Init
 *
 * @return      Status
 *
 *  - #ADI_USB_RESULT_SUCCESS         Upon successful enabling of root hub interrupt
 *
 *  - #ADI_USB_RESULT_INVALID_HANDLE  [D] Invalid Device handle is supplied
 *
 * @note        
 *             Caller(s)  USBH_HUB_EventReq()
 *
 * @sa          adi_musbmhdrc_SubmitURB()
 */
static  ADI_USB_RESULT  adi_musbmhdrc_RHIntEn (ADI_USB_HANDLE const hDevice)
{
    return ADI_USB_RESULT_SUCCESS;
}

/**
 * @brief       Disable root hub interrupt.
 *
 * @details     Disable root hub interrupt.
 *
 * @param [in]  hDevice               Handle that is returned via adi_musbmhdrc_Init
 *
 * @return      Status
 *
 *  - #ADI_USB_RESULT_SUCCESS         Upon successful disabling of root hub interrupt
 *
 *  - #ADI_USB_RESULT_INVALID_HANDLE  [D] Invalid Device handle is supplied
 *
 * @note        
 *             Caller(s)  USBH_HUB_RH_Event()
 *
 * @sa          adi_musbmhdrc_SubmitURB()
 */
static  ADI_USB_RESULT  adi_musbmhdrc_RHIntDis (ADI_USB_HANDLE const hDevice)
{
    return ADI_USB_RESULT_SUCCESS;
}

/**
 * @brief       Set Device connection
 *
 * @details     Set Device connection
 *
 * @param [in]  hDevice               Handle that is returned via adi_musbmhdrc_Init
 *
 * @param [in]  PortNumber            Port number
 *
 * @return      Status
 *
 *  - #ADI_USB_RESULT_SUCCESS         Upon successful connection of device
 *
 *  - #ADI_USB_RESULT_INVALID_HANDLE  [D] Invalid Device handle is supplied
 *
 * @note        
 *             Caller(s) USBH_HUB_RH_CtrlReq
 *
 * @sa          adi_musbmhdrc_SubmitURB()
 */
static  ADI_USB_RESULT  adi_musbmhdrc_PortConnSet (ADI_USB_HANDLE const hDevice,
                                                   const uint32_t PortNumber)
{
    ADI_MUSBMHDRC_DATA *pUsbDrvData = (ADI_MUSBMHDRC_DATA *)hDevice;

    ADI_USB_VALIDATE_HANDLE(hDevice);
    /* Set the Device connection detected bits */
    pUsbDrvData->RHPortStatus[ADI_USB_DEV_CONN_PORT].PortStatus |= ADI_USBH_HUB_STATUS_PORT_CONN;
    pUsbDrvData->RHPortStatus[ADI_USB_DEV_CONN_PORT].PortChange |= ADI_USBH_HUB_STATUS_C_PORT_CONN;

    return ADI_USB_RESULT_SUCCESS;
}


/**
 * @brief       Clear Port Power
 *
 * @details     Clear Port Power
 *
 * @param [in]  hDevice               Handle that is returned via adi_musbmhdrc_Init
 *
 * @param [in]  PortNumber            Port Number
 *
 * @return      Status
 *
 *  - #ADI_USB_RESULT_SUCCESS         Upon successful opening of the device
 *
 *  - #ADI_USB_RESULT_INVALID_HANDLE  [D] Invalid Device handle is supplied
 *
 * @note        
 *             Caller(s) Interface layer
 *
 * @sa          BusEventCallback()
 */

static  ADI_USB_RESULT  adi_musbmhdrc_PortConnChngSet (ADI_USB_HANDLE const hDevice,
                                                       const uint32_t PortNumber)
{
    ADI_MUSBMHDRC_DATA *pUsbDrvData = (ADI_MUSBMHDRC_DATA *)hDevice;

    ADI_USB_VALIDATE_HANDLE(hDevice);
    /* Set the Device connection detected bits */
    pUsbDrvData->RHPortStatus[ADI_USB_DEV_CONN_PORT].PortStatus = 0;
    pUsbDrvData->RHPortStatus[ADI_USB_DEV_CONN_PORT].PortChange |= ADI_USBH_HUB_STATUS_C_PORT_CONN;

    return (ADI_USB_RESULT_SUCCESS);
}

static void adi_usbdrvh_EnableHost(ADI_MUSBMHDRC_DATA *pUsbDrvData)
{
    volatile ADI_USB_REGS_DEF *pRegs = pUsbDrvData->pRegs;

    assert(pUsbDrvData->DevMode == ADI_USB_MODE_OTG_HOST);

    adi_usbdrv_SetSessionBit(pUsbDrvData);

    adi_usbdrv_DriveVbusOn(pUsbDrvData);

    pRegs->IEN |= (ENUM_USB_IRQ_SUSPEND | ENUM_USB_IRQ_VBUSERR);

    adi_usbdrv_WaitMilliSec(100);
}

static void adi_usbdrvh_DisableHost(ADI_MUSBMHDRC_DATA *pUsbDrvData)
{
   assert(pUsbDrvData->DevMode == ADI_USB_MODE_OTG_HOST);

   adi_usbdrv_DriveVbusOff(pUsbDrvData);
   pUsbDrvData->pRegs->DEV_CTL &= ~ENUM_USB_DEV_CTL_SESSION;
}
#endif /* ADI_CFG_USB_OTG_HOST */

/**
 * @brief       Registers the EP0 callback
 *
 * @details     Registers the EP0 callback
 *
 * @param [in]  hDevice               Handle that is returned via adi_musbmhdrc_Init
 *
 * @param [in]  pfEpZeroCallback      Callback function pointer
 *
 * @return      Status
 *
 *  - #ADI_USB_RESULT_SUCCESS         Upon successful register
 *
 *  - #ADI_USB_RESULT_INVALID_HANDLE  [D] Invalid Device handle is supplied
 *
 * @note        
 *             Caller(s) USBH_HUB_RH_CtrlReq
 *
 * @sa          adi_musbmhdrc_RegisterEpDataCallback()
 * @sa          adi_musbmhdrc_RegisterBusEventCallback()
 */
static ADI_USB_RESULT adi_musbmhdrc_RegisterEpZeroCallback (ADI_USB_HANDLE const hDevice,
                                                            ADI_CALLBACK   pfEpZeroCallback)
{
    ADI_MUSBMHDRC_DATA *pUsbDrvData = (ADI_MUSBMHDRC_DATA *)hDevice;

    ADI_USB_VALIDATE_HANDLE(hDevice);
    assert(pfEpZeroCallback != NULL);
    pUsbDrvData->pfEpZeroCallback = pfEpZeroCallback;

    return ADI_USB_RESULT_SUCCESS;
}


/**
 * @brief       Registers the EP data callback
 *
 * @details     Registers the EP data callback
 *
 * @param [in]  hDevice               Handle that is returned via adi_musbmhdrc_Init
 *
 * @param [in]  pfEpDataCallback      Upon successful register
 *
 * @return      Status
 *
 *  - #ADI_USB_RESULT_SUCCESS         Upon successful opening of the device
 *
 *  - #ADI_USB_RESULT_INVALID_HANDLE  [D] Invalid Device handle is supplied
 *
 * @note        
 *             Caller(s) USBH_HUB_RH_CtrlReq
 *
 * @sa          adi_musbmhdrc_RegisterEpZeroCallback()
 * @sa          adi_musbmhdrc_RegisterBusEventCallback()
 */
static ADI_USB_RESULT adi_musbmhdrc_RegisterEpDataCallback (ADI_USB_HANDLE const hDevice,
                                                            ADI_CALLBACK   pfEpDataCallback)
{
    ADI_MUSBMHDRC_DATA *pUsbDrvData = (ADI_MUSBMHDRC_DATA *)hDevice;

    ADI_USB_VALIDATE_HANDLE(hDevice);
    assert(pfEpDataCallback != NULL);
    pUsbDrvData->pfEpDataCallback = pfEpDataCallback;

    return ADI_USB_RESULT_SUCCESS;
}

/**
 * @brief       Registers bus event callback
 *
 * @details     Registers bus event callback
 *
 * @param [in]  hDevice               Handle that is returned via adi_musbmhdrc_Init
 *
 * @param [in]  pfBusEventCallback    Callback function pointer
 *
 * @return      Status
 *
 *  - #ADI_USB_RESULT_SUCCESS         Upon successful register
 *
 *  - #ADI_USB_RESULT_INVALID_HANDLE  [D] Invalid Device handle is supplied
 *
 * @note        
 *             Caller(s) USBH_HUB_RH_CtrlReq
 *
 * @sa          adi_musbmhdrc_RegisterEpDataCallback()
 * @sa          adi_musbmhdrc_RegisterEpZeroCallback()
 */

static ADI_USB_RESULT adi_musbmhdrc_RegisterBusEventCallback (ADI_USB_HANDLE const hDevice,
                                                              ADI_CALLBACK   pfBusEventCallback)
{
    ADI_MUSBMHDRC_DATA *pUsbDrvData = (ADI_MUSBMHDRC_DATA *)hDevice;

    ADI_USB_VALIDATE_HANDLE(hDevice);

    assert(pfBusEventCallback != NULL);
    pUsbDrvData->pfBusEventCallback = pfBusEventCallback;

    return ADI_USB_RESULT_SUCCESS;
}

/*
 *
 * Local Functions to Driver 
 *
 */
static ADI_USB_RESULT adi_usbdrv_AllocEpinfo(ADI_MUSBMHDRC_DATA *pUsbDrvData, ADI_USB_EP_DESC *pEpDesc, ADI_USB_EP_INFO **pEpInfo)
{
    uint32_t       index;
    int32_t        EpPhyIdx;
    uint8_t        EpPhyNum;

    if(ADI_USB_GET_EP_NUM(pEpDesc->EndpointAddress) == ADI_USB_EP_NUM_0 )
    {
        *pEpInfo = &pUsbDrvData->EpInfo[0];
        (*pEpInfo)->EpPhyNum = ADI_USB_EP_NUM_0;
        return ADI_USB_RESULT_SUCCESS;
    }

    /* for data endpoints get a physical endpoint */
    EpPhyIdx = adi_usbdrv_GetFreePhyEp(pUsbDrvData, pEpDesc, &EpPhyNum);

    if(EpPhyIdx != -1)
    {
        /* get logical endpoint */
        for (index = 1; index < ADI_USB_MAX_NUM_TOTAL_EP; index++) 
        {
            if (pUsbDrvData->EpInfo[index].EpLogicalAddr == 0)
            {
                *pEpInfo = &pUsbDrvData->EpInfo[index];
                (*pEpInfo)->EpPhyNum = EpPhyNum;
                (*pEpInfo)->EpPhyIdx = EpPhyIdx;
                return ADI_USB_RESULT_SUCCESS;
            }
        }
    }
    return ADI_USB_RESULT_EP_ALLOC_FAILED;
}

static int32_t adi_usbdrv_GetEpPhyNum(ADI_MUSBMHDRC_DATA *pUsbDrvData, uint32_t EpData, uint8_t *EpPhyIdx)
{
    uint32_t        index;
    ADI_USB_EP_INFO *pEpInfo;

    if (EpData == 0) { return 0; }

    for (index = 1; index < ADI_USB_MAX_NUM_TOTAL_EP; index++)
    {
        pEpInfo = &pUsbDrvData->EpInfo[index];
        if( (pEpInfo->EpLogicalAddr == ADI_USB_GET_EPADDR_FRMEPD(EpData)) &&
            (pEpInfo->DevAddr       == ADI_USB_GET_DEVADDR_FRMEPD(EpData)))
        {
            *EpPhyIdx = pEpInfo->EpPhyIdx;
            return pEpInfo->EpPhyNum;
        }
    }

    return -1;
}

static ADI_USB_EP_INFO* adi_usbdrv_GetEpInfo(ADI_MUSBMHDRC_DATA *pUsbDrvData, uint32_t EpData)
{
    uint32_t        index;
    ADI_USB_EP_INFO *pEpInfo;
    uint8_t         EpAddr = ADI_USB_GET_EPADDR_FRMEPD(EpData);
#if (ADI_CFG_USB_OTG_HOST == 1)
    uint8_t         EpType = ADI_USB_GET_EPTYPE_FRMEPD(EpData);
#endif

    if (ADI_USB_GET_EP_NUM(EpAddr) == ADI_USB_EP_NUM_0)
    {
        return &pUsbDrvData->EpInfo[0];
    }

    for (index = 1; index < ADI_USB_MAX_NUM_TOTAL_EP; index++)
    {
        pEpInfo = &pUsbDrvData->EpInfo[index];

        /* find out correct epinfo structure, note that different devices can have
         * same logical endpoints
         */
        if ((pEpInfo->DevAddr       == ADI_USB_GET_DEVADDR_FRMEPD(EpData)) &&
            /* EpType is only used in the host mode
             * When hub is used same ep number and differnt type can exist with same direction
             */
#if (ADI_CFG_USB_OTG_HOST == 1)
            (pEpInfo->EpType        == EpType)                             &&
#endif
            (pEpInfo->EpLogicalAddr == EpAddr))
        {
            return &pUsbDrvData->EpInfo[index];
        }
    }

    return NULL;
}

/* free both logical and physical entries for the given endpoint */
static ADI_USB_RESULT adi_usbdrv_ClearEpInfo(ADI_MUSBMHDRC_DATA *pUsbDrvData, uint32_t EpData)
{
    uint32_t        index;
    ADI_USB_EP_INFO *pEpInfo;
    uint8_t         EpAddr = ADI_USB_GET_EPADDR_FRMEPD(EpData);
#if (ADI_CFG_USB_OTG_HOST == 1)
    uint8_t         EpType = ADI_USB_GET_EPTYPE_FRMEPD(EpData);
#endif

    for (index = 1; index < ADI_USB_MAX_NUM_TOTAL_EP; index++)
    {
        pEpInfo = &pUsbDrvData->EpInfo[index];

        if ((pEpInfo->DevAddr       == ADI_USB_GET_DEVADDR_FRMEPD(EpData)) &&
 #if (ADI_CFG_USB_OTG_HOST == 1)
            (pEpInfo->EpType        == EpType)                             &&
 #endif
            (pEpInfo->EpLogicalAddr == EpAddr))
        {
            pEpInfo->EpLogicalAddr = 0x0;
            ADI_CFG_USB_PHYEP_CONFIG[pEpInfo->EpPhyIdx].AllocFlag = 0;
            return ADI_USB_RESULT_SUCCESS;
        }
    }

    return ADI_USB_RESULT_EP_INVALID;
}

static void adi_usbdrv_FreePhyEndpoints(ADI_MUSBMHDRC_DATA *pUsbDrvData)
{
    uint32_t index;

    for (index = 0; index < pUsbDrvData->EpConfigEntryMax; index++)
    {
        ADI_CFG_USB_PHYEP_CONFIG[index].AllocFlag = 0;
    }
}

static ADI_USB_EP_INFO* adi_usbdrv_GetEpInfoFromEpPhy(ADI_MUSBMHDRC_DATA *pUsbDrvData, uint8_t EpPhyNum, uint8_t EpDir)
{
    uint32_t        index;
    ADI_USB_EP_INFO *pEpInfo;

    if (EpPhyNum == ADI_USB_EP_NUM_0)
    {
        return &pUsbDrvData->EpInfo[0];
    }

    for (index = 1; index < ADI_USB_MAX_NUM_TOTAL_EP; index++)
    {
        pEpInfo = &pUsbDrvData->EpInfo[index];

        if ((pEpInfo->EpPhyNum == EpPhyNum) && (pEpInfo->EpDir == EpDir))
        {
            return pEpInfo;
        }
    }

    return NULL;
}

static ADI_USB_RESULT adi_usbdrv_SubmitEp0RxBuffer(ADI_MUSBMHDRC_DATA *pUsbDrvData, ADI_USB_EP_INFO *pEpInfo)
{
    volatile ADI_USB_REGS_DEF *pRegs;
    uint16_t                  SavedEpNum;
    ADI_USB_RESULT            Result;
    ADI_SUP_UNUSED_WARN int   uIrqTxStatus;

    pRegs = pUsbDrvData->pRegs;
    SavedEpNum=pRegs->INDEX;

#if (ADI_CFG_USB_DEVICE == 1)
    /* In device mode, Status ZLP sent from host is processed at the hardware level.
     * No Rx interrupt is asserted, hence we assume that it is recevied automatically
     * and RX ZLP buffer is returned as soon as it is submitted */
    if (pEpInfo->pEpCurURB->TransferLen == 0)
    {
        adi_usbdrv_ProcessRxZLP(pUsbDrvData, pEpInfo, &SavedEpNum);
        return ADI_USB_RESULT_SUCCESS;
    }

    /* check if we are in data phase if not return error */
    if (pUsbDrvData->EpZeroSetupStatus != ADI_USB_SS_PENDING_DATA )
    {
        return ADI_USB_RESULT_FAIL;
    }
#endif

    pRegs->INDEX = pEpInfo->EpPhyNum;

    if (ADI_USB_IF_BIT_SET(pRegs->EP0_CSR0, BITM_USB_EP0I_CSR_H_RXSTALL))
    {
        pRegs->EP0_CSR0 &= ~ENUM_USB_EP0I_CSR_H_RXSTALL;
    }

    /* Check if we have data already inside the Rx fifo */
    if (ADI_USB_IF_BIT_SET(pRegs->EP0_CSR0, BITM_USB_EP0I_CSR_H_RXPKTRDY))
    {
        ADI_USB_EP_STATINC(pUsbDrvData,pEpInfo->EpPhyIdx,nPreRxDataAvlbl);
        /* Clear the interrupt */
#if defined(ADI_CFG_USB_BF_MUSBMHDRC) || defined(ADI_CFG_USB_ARM_MUSBMHDRC)
        uIrqTxStatus = pUsbDrvData->pRegs->INTRTX;  /* FIXIT RC so may clear other interrupts */
#elif defined(ADI_CFG_USB_BF_MUSBHDRC)
        uIrqTxStatus = pUsbDrvData->pRegs->INTRTX; 
        ADI_USB_DBG_CNTRINC(pUsbDrvData,dbg_ep0_high_intack);
        pRegs->INTRTX |= (1ul << (pEpInfo->EpPhyNum));
#endif
        Result = adi_usbdrv_HandleRxPacket(pUsbDrvData, pEpInfo, &SavedEpNum);
        pRegs->INDEX = SavedEpNum;
        return Result;
    }

    pEpInfo->EpUrbState = ADI_USB_URB_STATE_SUBMITTED;

#if (ADI_CFG_USB_OTG_HOST == 1)
        pRegs->EP0_CSR0 |= (ENUM_USB_EP0I_CSR_H_REQPKT | ENUM_USB_EP0I_CSR_H_DISPING);
#endif

    pRegs->INDEX = SavedEpNum;
    return ADI_USB_RESULT_SUCCESS;
}


static ADI_USB_RESULT adi_usbdrv_SubmitDataRxBuffer(ADI_MUSBMHDRC_DATA *pUsbDrvData, ADI_USB_EP_INFO *pEpInfo)
{
    volatile ADI_USB_REGS_DEF *pRegs;
    uint16_t                  SavedEpNum;
    ADI_USB_RESULT            Result;
    ADI_SUP_UNUSED_WARN int   uIrqRxStatus;

    pRegs = pUsbDrvData->pRegs;
    SavedEpNum=pRegs->INDEX;

    pRegs->INDEX = pEpInfo->EpPhyNum;

#if (ADI_CFG_USB_DEVICE == 1)
    /* Check if EP is STALLed, Need to check only for device mode.
     * Host mode RXSTALL will be cleared in adi_usbdrv_HandleRxErrors */
    if (ADI_USB_IF_BIT_SET(pRegs->EPI_RXCSR, BITM_USB_EPI_RXCSR_P_SENTSTALL))
    {
        pRegs->EPI_RXCSR &= ~ENUM_USB_EPI_RXCSR_P_STALSNT;
        pRegs->INDEX      = SavedEpNum;
        return ADI_USB_RESULT_EP_STALL;
    }
#endif

    /* FIXIT: Check if this is applicable only for Device mode */
    if (pEpInfo->EpType == ADI_USB_EP_TYPE_ISOC)
    {
        if (pRegs->EPI_RXCSR & BITM_USB_EPI_RXCSR_H_RXTOERR)
        {
            pRegs->EPI_RXCSR &= ~(ENUM_USB_EPI_RXCSR_H_RXTOERR); // Clear any rx overrun errors.
        }
    }

    pEpInfo->EpUrbState = ADI_USB_URB_STATE_SUBMITTED;


    if ((pEpInfo->EpTransferMode == ADI_USB_EP_MODE_DMA_0) &&
        (ADI_USB_IF_BIT_SET(pRegs->EPI_RXCSR, BITM_USB_EPI_RXCSR_H_RXPKTRDY)))
    {
        ADI_USB_EP_STATINC(pUsbDrvData,pEpInfo->EpPhyIdx,nPreRxDataAvlbl);
        /* Clear the interrupt */
#if defined(ADI_CFG_USB_BF_MUSBMHDRC) || defined(ADI_CFG_USB_ARM_MUSBMHDRC)
        uIrqRxStatus = pUsbDrvData->pRegs->INTRRX; /* Read to clear */
#elif defined(ADI_CFG_USB_BF_MUSBHDRC)
        uIrqRxStatus = pUsbDrvData->pRegs->INTRRX;
        pRegs->INTRRX = (0x1u << (pEpInfo->EpPhyNum));
#endif

        Result = adi_usbdrv_HandleRxPacket(pUsbDrvData, pEpInfo, &SavedEpNum);
        pRegs->INDEX = SavedEpNum;
        return Result;
    }

#if (ADI_CFG_USB_ENABLE_DMA_MODE1 == 1)
    if (pEpInfo->EpTransferMode == ADI_USB_EP_MODE_DMA_1)
    {
        /* For DMA mode 1, we need to set it here
         * because we will not get the RX interrupt if data size is multiple of MaxPktSize */
        pEpInfo->EpProcBytesThisPass = adi_usbdrv_BytesThisPass(pEpInfo);

        /* Schedule DMA mode 1 */
        Result = adi_usbdrv_ScheduleDma(pUsbDrvData, pEpInfo);
        if(Result != ADI_USB_RESULT_SUCCESS)
        {
            return Result;
        }
        
        /* There might be a case that there is a packet already there inside the FIFO before DMA Mode 1 is scheduled.
         * It will lead to an Rx interupt even though DMA has already started to transfer this packet.
         * RX interrupt is enabled so that this unwanted RxInterrupt status can be cleared
         *
         * Refer to adi_usbdrv_RcvDataFromFifo() funtion-
         * if(pEpInfo->EpTransferMode == ADI_USB_EP_MODE_DMA_1) we are returning without processing the Rx buffer
         */
        pRegs->INTRRXE |= (1u << pEpInfo->EpPhyNum);
    }
    else
#endif
    {
        pRegs->INTRRXE |= (1u << pEpInfo->EpPhyNum);
    }

#if (ADI_CFG_USB_OTG_HOST == 1)
        /* Send IN tokens to fetch the data */
        pRegs->EPI_RXCSR |=  ENUM_USB_EPI_RXCSR_H_REQPKT;
#endif

    pRegs->INDEX = SavedEpNum;
    return ADI_USB_RESULT_SUCCESS;
}

static ADI_USB_RESULT adi_usbdrv_HandleRxPacket(ADI_MUSBMHDRC_DATA *pUsbDrvData, ADI_USB_EP_INFO *pEpInfo, uint16_t *SavedEpNum)
{
    volatile ADI_USB_REGS_DEF *pRegs;
    uint32_t                  RxCount;
    uint32_t                  EpPhyNum;
    ADI_USB_RESULT            Result = ADI_USB_RESULT_SUCCESS;

    pRegs = pUsbDrvData->pRegs;
    EpPhyNum = pEpInfo->EpPhyNum;

    RxCount = pRegs->EPI_RXCNT; /* Since index is set we can use pEPI_RXCNT for all EPs */

    if (RxCount)
    {
        pEpInfo->EpUrbState          = ADI_USB_URB_STATE_PROCESSING;
        pEpInfo->EpProcBytesThisPass = RxCount;     
        Result = adi_usbdrv_ScheduleTransfer(pUsbDrvData, pEpInfo);
    }
#if 1   // FIXIT SG  looks like this is not required. check host mode tests
    else
    {
        if (pEpInfo->pEpCurURB->TransferLen)
        {
            Result = ADI_USB_RESULT_URB_ABORT;
        }
        else
        {
            pRegs->INDEX = *SavedEpNum;

            ADI_USB_EXIT_CR();
            assert(pEpInfo->pEpCurURB != NULL);
            if (EpPhyNum)
            {
                pUsbDrvData->pfEpDataCallback(pEpInfo->pEpCurURB, ADI_USB_RESULT_URB_COMPLETE, 0);
            }
            else
            {
               pUsbDrvData->pfEpZeroCallback(pEpInfo->pEpCurURB, ADI_USB_RESULT_URB_COMPLETE, 0);
            }
            ADI_USB_ENTER_CR();

            adi_usbdrv_ResetCurrentURB(pEpInfo);
            *SavedEpNum=pRegs->INDEX;
            pRegs->INDEX = pEpInfo->EpPhyNum;
        }
    }
#endif

    return Result;
}

static ADI_USB_RESULT adi_usbdrv_SubmitRxBuffer(ADI_MUSBMHDRC_DATA *pUsbDrvData, ADI_USB_EP_INFO *pEpInfo)
{
    ADI_USB_RESULT Result;

    if (pEpInfo->EpPhyNum == ADI_USB_EP_NUM_0)
    {
        Result = adi_usbdrv_SubmitEp0RxBuffer(pUsbDrvData, pEpInfo);
    }
    else
    {
        Result = adi_usbdrv_SubmitDataRxBuffer(pUsbDrvData, pEpInfo);
    }

    return Result;
}

static ADI_USB_RESULT adi_usbdrv_SubmitTxBuffer(ADI_MUSBMHDRC_DATA *pUsbDrvData, ADI_USB_EP_INFO *pEpInfo)
{
    volatile ADI_USB_REGS_DEF *pRegs;
    uint16_t                  SavedEpNum;
    ADI_USB_RESULT            Result;

    pRegs = pUsbDrvData->pRegs;
    SavedEpNum = pRegs->INDEX;

    /* set the EP number */
    pRegs->INDEX = pEpInfo->EpPhyNum;

    if (pEpInfo->EpType == ADI_USB_EP_TYPE_ISOC)
    {
        if (pRegs->EPI_TXCSR & BITM_USB_EP_TXCSR_H_TXTOERR)
        {
            pRegs->EPI_TXCSR &= ~(ENUM_USB_EP_TXCSR_H_TXTOERR); /* Clear any tx under run errors. */
        }

#if !defined(ADI_CFG_USB_ARM_MUSBMHDRC) /* Not available for aduCM350(FS) */
        if (pRegs->EPI_TXCSR & BITM_USB_EP_TXCSR_H_NAKTOINCMP)
        {
            pRegs->EPI_TXCSR &= ~(ENUM_USB_EP_TXCSR_H_NAKTO);
        }
#endif
        adi_usbdrv_FlushTxFifo(pRegs);

 #if defined(ADI_CFG_USB_BF_MUSBHDRC)
        pRegs->INTRTX = (1u << pEpInfo->EpPhyNum);
 #endif
    }

    if (pEpInfo->pEpCurURB->TransferLen)
    {
        pEpInfo->EpProcBytesThisPass = adi_usbdrv_BytesThisPass(pEpInfo);

        pEpInfo->EpUrbState = ADI_USB_URB_STATE_SUBMITTED;

        /* Initiate the TX transfer */
        Result = adi_usbdrv_ScheduleTransfer(pUsbDrvData, pEpInfo);
    }
    else
    {
        pEpInfo->EpUrbState = ADI_USB_URB_STATE_SUBMITTED; 
        Result = adi_usbdrv_HandleTxZLP(pUsbDrvData, pEpInfo, &SavedEpNum);
    }
    /* Enable the TX Interrupt*/
    pRegs->INTRTXE |= (1u << pEpInfo->EpPhyNum);

    pRegs->INDEX = SavedEpNum;
    return Result;
}

static void adi_usbdrv_ProcessRxZLP(ADI_MUSBMHDRC_DATA *pUsbDrvData, ADI_USB_EP_INFO *pEpInfo, uint16_t *SavedEpNum)
{
    volatile ADI_USB_REGS_DEF *pRegs = pUsbDrvData->pRegs;

#if (ADI_CFG_USB_OTG_HOST == 1)
    pRegs->EP0_CSR0 &= ~(ENUM_USB_EP0I_CSR_H_RXPKTRDY |
                           ENUM_USB_EP0I_CSR_H_STATPKT);
    pEpInfo->pEpCurURB->ProcessedBytes = 0;
#else
    if (pUsbDrvData->EpZeroSetupStatus == ADI_USB_SS_PENDING_RX_DP_ZLP)
    {
        pRegs->EP0_CSR0 |= (ENUM_USB_EP0I_CSR_P_TXPKTRDY | ENUM_USB_EP0I_CSR_P_DATAEND);
    }

    pUsbDrvData->EpZeroSetupStatus = ADI_USB_SS_IDLE;
    pEpInfo->EpUrbState            = ADI_USB_URB_STATE_FREE;
    pUsbDrvData->EpZeroSetupState  = ADI_USB_SETUP_IDLE;
	
    pRegs->INDEX = *SavedEpNum;

    ADI_USB_EXIT_CR();
    pUsbDrvData->pfEpZeroCallback(pEpInfo->pEpCurURB, ADI_USB_RESULT_URB_COMPLETE, 0);
    ADI_USB_ENTER_CR();

    *SavedEpNum = pRegs->INDEX;
    pRegs->INDEX = pEpInfo->EpPhyNum;

#endif
#if (ADI_CFG_USB_OTG_HOST == 1)
       pRegs->EP0_CSR0 &= ~ENUM_USB_EP0_CSR_H_REQPKT;
       pEpInfo->pEpCurURB->ProcessedBytes = 0;
#endif
}

static ADI_USB_RESULT adi_usbdrv_HandleTxZLP(ADI_MUSBMHDRC_DATA *pUsbDrvData, ADI_USB_EP_INFO *pEpInfo, uint16_t *SavedEpNum)
{
    volatile ADI_USB_REGS_DEF *pRegs = pUsbDrvData->pRegs;

    pEpInfo->pEpCurURB->ProcessedBytes = 0;

    if(pEpInfo->EpPhyNum == ADI_USB_EP_NUM_0)
    {
#if (ADI_CFG_USB_OTG_HOST == 1)
        
        /* It is assumed that EP0 never handles data transfers so every ZLP from EP0 is treated as a Status phase transaction */
        pRegs->EP0_CSR0 |= (ENUM_USB_EP0I_CSR_H_TXPKTRDY | ENUM_USB_EP0I_CSR_H_STATPKT);

        pEpInfo->EpUrbState = ADI_USB_URB_STATE_PROCESSING;

#else

        if (pUsbDrvData->EpZeroSetupStatus == ADI_USB_SS_PENDING_TX_NDP_ZLP)
        {
            pRegs->EP0_CSR0 |= (ENUM_USB_EP0I_CSR_P_SPKTRDY | ENUM_USB_EP0I_CSR_P_DATAEND);
            pUsbDrvData->EpZeroSetupStatus = ADI_USB_SS_IDLE;
        }

        pEpInfo->EpUrbState = ADI_USB_URB_STATE_FREE;
        pUsbDrvData->EpZeroSetupState = ADI_USB_SETUP_IDLE;
		
        pRegs->INDEX = *SavedEpNum;
        ADI_USB_EXIT_CR();
        pUsbDrvData->pfEpZeroCallback(pEpInfo->pEpCurURB, ADI_USB_RESULT_URB_COMPLETE, 0);
        ADI_USB_ENTER_CR();


        *SavedEpNum = pRegs->INDEX;
        pRegs->INDEX = pEpInfo->EpPhyNum;

        /* We finished status stage of the transfer, a setup packet might have already present in
         * the FIFO if so process it.
         */
         if (pRegs->EP0_CNT0 == ADI_USB_SETUP_PKT_LEN)
         {
             adi_usbdrv_HandleSetupPacket(pUsbDrvData,pEpInfo,(uint16_t*)&SavedEpNum);
         }
#endif
    }
    else
    {
        pRegs->EPI_TXCSR |= ENUM_USB_EPI_TXCSR_H_PKTRDY;
        pEpInfo->EpUrbState = ADI_USB_URB_STATE_PROCESSING;
    }

    return ADI_USB_RESULT_SUCCESS;
}


static void adi_usbdrv_SetTransferMode(ADI_MUSBMHDRC_DATA *pUsbDrvData, ADI_USB_EP_INFO *pEpInfo)
{
    ADI_SUP_UNREF_WARN uint32_t BytesRemaining;
    
#if (ADI_CFG_USB_ENABLE_DMA_MODE1 == 1)
     BytesRemaining = pEpInfo->pEpCurURB->TransferLen - pEpInfo->pEpCurURB->ProcessedBytes;
    if ((BytesRemaining > pEpInfo->EpMaxSize) && (pEpInfo->EpType == ADI_USB_EP_TYPE_BULK))
    {
        pEpInfo->EpTransferMode = ADI_USB_EP_MODE_DMA_1;
        return;
    }
#endif

    pEpInfo->EpTransferMode = ADI_USB_EP_MODE_DMA_0;
}


static void adi_usbdrv_BindPhysicalEndpoint(ADI_MUSBMHDRC_DATA *pUsbDrvData, ADI_USB_EP_INFO *pEpInfo)
{
    volatile ADI_USB_REGS_DEF *pRegs;
    uint16_t                  SavedEpNum,PhyEpNum;
    uint16_t                  IntStatus;

    pRegs = pUsbDrvData->pRegs;
    SavedEpNum = pRegs->INDEX;

    PhyEpNum = pEpInfo->EpPhyNum;
    pRegs->INDEX = PhyEpNum;

    if (pEpInfo->EpDir == ADI_USB_EP_DIR_TX)
    {
        pRegs->EPI_TXMAXP = pEpInfo->EpMaxSize & 0x07FF;

#if defined(__ADSPBF527_FAMILY__) || defined(__ADSPBF548_FAMILY__)
        /* disable double buffering by setting the fifo size as max endpoint size */
        if (pEpInfo->EpType == ADI_USB_EP_TYPE_BULK)
        {
            pRegs->EPI_TXMAXP = (pEpInfo->EpPhyNum > 4) ? 1024 : 128; /* fifo size for 1-4 is 128 and 5-7 is 1024 */
        }
#endif

#if defined(ADI_CFG_USB_BF_MUSBHDRC)
        if (pEpInfo->EpType == ADI_USB_EP_TYPE_ISOC)
        {
            /* disable double buffering by setting the fifo size as max endpoint size */
            pRegs->EPI_TXMAXP = (pEpInfo->EpPhyNum > 4) ? 1024 : 128; /* fifo size for 1-4 is 128 and 5-7 is 1024 */
        }
#endif

        pRegs->EPI_TXCSR = 0x0;

        /* Applicable only in host mode */
#if (ADI_CFG_USB_OTG_HOST == 1)
            pRegs->EPI_TXTYPE = ((ADI_USB_GET_EP_NUM(pEpInfo->EpLogicalAddr)) | (pEpInfo->EpType << 4u));
#endif

        pRegs->EPI_TXINTERVAL = (pEpInfo->EpInterval & 0xf);

        pRegs->EPI_TXCSR |= ENUM_USB_EPI_TXCSR_H_CLRTGL;

#if (ADI_CFG_USB_DEVICE == 1)
        if (pEpInfo->EpType == ADI_USB_EP_TYPE_ISOC)
        {
            pRegs->EPI_TXCSR |= (ENUM_USB_EP_TXCSR_P_ISOEN); /* ISO bit in device mode only. */
        }
#endif

#if (ADI_USBH_MULTI_POINT_EN == 1)
        pUsbDrvData->pRegs->MPRegs[PhyEpNum].MP_TXFUNCADDR = pEpInfo->DevAddr; 
#endif
        /* clear off pending interrupts */
        IntStatus = pRegs->INTRTX;
        pRegs->INTRTX = IntStatus;
    }
    else
    {
         pRegs->EPI_RXMAXP = pEpInfo->EpMaxSize;

#if defined(__ADSPBF527_FAMILY__) || defined(__ADSPBF548_FAMILY__)
        /* disable double buffering by setting the fifo size as max endpoint size */
        if (pEpInfo->EpType == ADI_USB_EP_TYPE_BULK) 
        {
            pRegs->EPI_RXMAXP = (pEpInfo->EpPhyNum > 4) ? 1024 : 128; /* fifo size for 1-4 is 128 and 5-7 is 1024 */
        }
#endif
         pRegs->EPI_RXCSR  = 0x0;

        /* Applicable only in host mode */
#if (ADI_CFG_USB_OTG_HOST == 1)
            pRegs->EPI_RXTYPE = ((ADI_USB_GET_EP_NUM(pEpInfo->EpLogicalAddr)) | (pEpInfo->EpType << 4u));
#endif

        pRegs->EPI_RXINTERVAL = (pEpInfo->EpInterval & 0xf);

        pRegs->EPI_RXCSR |= ENUM_USB_EPI_RXCSR_H_CLRTGL;

#if (ADI_CFG_USB_DEVICE == 1)
        if (pEpInfo->EpType == ADI_USB_EP_TYPE_ISOC)
        {
            pRegs->EPI_RXCSR |= (ENUM_USB_EP_RXCSR_P_ISOEN); /* ISO bit in device mode only */
        }
#endif

#if (ADI_USBH_MULTI_POINT_EN == 1)
        pUsbDrvData->pRegs->MPRegs[PhyEpNum].MP_RXFUNCADDR = pEpInfo->DevAddr;
#endif
        /* clear off pending interrupts */
        IntStatus = pRegs->INTRRX;
        pRegs->INTRRX = IntStatus;
    }

#if defined(ADI_CFG_USB_BF_MUSBMHDRC)
    adi_usbdrv_SetDynamicFifoParams(pUsbDrvData,pEpInfo,true);
#endif

    /* Since there is only one DMA channel for all the data EPs in aduCM350,
     * it will be allocated only during the usage by data EP */    
#if !defined(ADI_ADUCM350)    
    adi_usbdrv_AllocateDmaChannel(pUsbDrvData,pEpInfo,true);
#endif
    
    pUsbDrvData->DevState = ADI_USB_DEV_STATE_CONFIGURED;

    pRegs->INDEX = SavedEpNum;
}

static void adi_usbdrv_ResetEndpointZero(ADI_MUSBMHDRC_DATA *pUsbDrvData)
{
    volatile ADI_USB_REGS_DEF *pRegs = pUsbDrvData->pRegs;

    adi_usbdrv_FlushEp0Fifo(pRegs);

    /* clear off pending interrupts */
    pRegs->INTRTX    = (1u << ADI_USB_EP_NUM_0);
    pRegs->EP0_CSR0  = 0x0;
}

static void adi_usbdrv_ResetPhysicalEndpoint(ADI_MUSBMHDRC_DATA *pUsbDrvData, ADI_USB_EP_INFO *pEpInfo)
{
    volatile ADI_USB_REGS_DEF *pRegs = pUsbDrvData->pRegs;

    if (pEpInfo->EpDir == ADI_USB_EP_DIR_RX)
    {
        adi_usbdrv_FlushRxFifo(pRegs);

        /* clear off pending interrupts */
        pRegs->INTRRX     = (1u << pEpInfo->EpPhyNum);
        pRegs->EPI_RXCSR  = 0x0;

        /* Disable the EP interrupt */
        pRegs->INTRRXE &= ~(1u << pEpInfo->EpPhyNum);
    }
    else
    {
        adi_usbdrv_FlushTxFifo(pRegs);

        /* clear off pending interrupts */
        pRegs->INTRTX    = (1u << pEpInfo->EpPhyNum);
        pRegs->EPI_TXCSR = 0x0;

        /* Disable the EP interrupt */
        pRegs->INTRTXE &= ~(1u << pEpInfo->EpPhyNum);
    }
}


static uint32_t adi_usbdrv_BytesThisPass(ADI_USB_EP_INFO *pEpInfo)
{
    uint32_t BytesRemaining, ByteCount=0;

    assert(pEpInfo->EpTransferMode != ADI_USB_EP_MODE_NONE);

    BytesRemaining = pEpInfo->pEpCurURB->TransferLen - pEpInfo->pEpCurURB->ProcessedBytes;

    if ((pEpInfo->EpTransferMode == ADI_USB_EP_MODE_DMA_0) ||
        (pEpInfo->EpTransferMode == ADI_USB_EP_MODE_PIO))
    {
        ByteCount = ((BytesRemaining > pEpInfo->EpMaxSize) ?
                                       pEpInfo->EpMaxSize  :
                                       BytesRemaining);
    }
#if (ADI_CFG_USB_ENABLE_DMA_MODE1 == 1)
    else if (pEpInfo->EpTransferMode == ADI_USB_EP_MODE_DMA_1)
    {
        ByteCount = pEpInfo->pEpCurURB->TransferLen;
    }
#endif

    return ByteCount;
}


static void adi_usbdrv_SetRegDefault(ADI_MUSBMHDRC_DATA *pUsbDrvData)
{
    volatile ADI_USB_REGS_DEF *pRegs;
#if defined(ADI_CFG_USB_BF_MUSBHDRC)
    uint16_t                  IntStatus;
#endif

    pRegs = pUsbDrvData->pRegs;

    pRegs->INDEX      = ADI_USB_EP_NUM_0;
    pRegs->EPI_RXMAXP = 64u;
    pRegs->EPI_TXMAXP = 64u;
#if (ADI_CFG_USB_OTG_HOST == 1)
    pRegs->INTRTXE    = 0x0;
#else
    pRegs->INTRTXE    = BITM_USB_INTRTXE_EP0;
#endif
    pRegs->INTRRXE    = 0x0;

#if defined(ADI_CFG_USB_BF_MUSBHDRC)
    IntStatus       = pRegs->INTRTX;
    pRegs->INTRTX = IntStatus;

    IntStatus       = pRegs->INTRRX;
    pRegs->INTRRX = IntStatus;

    IntStatus       = pRegs->IRQ;
    pRegs->IRQ    = IntStatus;
#endif

    pRegs->IEN  =  (ENUM_USB_IRQ_RSTBABBLE  |            
                    ENUM_USB_IRQ_CON        |       /* Not valid for 350, defined as NULL */
                    ENUM_USB_IRQ_VBUSERR    |       /* Not valid for 350, defined as NULL */
                    ENUM_USB_IRQ_DISCON     |
                    ENUM_USB_IRQ_RESUME     |
                    ENUM_USB_IRQ_SUSPEND);
}


static int32_t adi_usbdrv_GetFreePhyEp(ADI_MUSBMHDRC_DATA *pUsbDrvData, ADI_USB_EP_DESC *pEpDesc, uint8_t *EpPhyNum)
{
    int              index;
    ADI_USB_EP_DIR   EpDir  = ADI_USB_GET_EP_DIR(pEpDesc->EndpointAddress);
    ADI_USB_EP_TYPE  EpType = ADI_USB_GET_EP_TYPE(pEpDesc->Attributes);

    for (index = 0; index < pUsbDrvData->EpConfigEntryMax; index++)
    {
        if ((ADI_CFG_USB_PHYEP_CONFIG[index].AllocFlag == 0)                   &&
            (EpDir    == ADI_CFG_USB_PHYEP_CONFIG[index].EpDir)                &&
            ((EpType   & ADI_CFG_USB_PHYEP_CONFIG[index].EpType) == EpType)    &&
            (pEpDesc->MaxPacketSize                   <= ADI_CFG_USB_PHYEP_CONFIG[index].EpMaxSize))
         {
             *EpPhyNum =  ADI_CFG_USB_PHYEP_CONFIG[index].EpPhyNum;
             ADI_CFG_USB_PHYEP_CONFIG[index].AllocFlag = 1;
             return index;
         }
    }
    return(-1) ;
}

static ADI_USB_RESULT adi_usbdrv_ScheduleDma(ADI_MUSBMHDRC_DATA *pUsbDrvData, ADI_USB_EP_INFO *pEpInfo)
{
    volatile ADI_USB_REGS_DEF *pRegs;
    uint16_t                  DmaCtlVal=0;
    ADI_USB_RESULT            Result;

    assert(pEpInfo != NULL);
    assert(pUsbDrvData != NULL);
    assert(pEpInfo->EpProcBytesThisPass != 0u);

#if defined(ADI_ADUCM350)
    if(pEpInfo->EpPhyNum)
    {
        Result = adi_usbdrv_AllocateDmaChannel(pUsbDrvData, pEpInfo, true);
        if(Result != ADI_USB_RESULT_SUCCESS)
        {
            return Result;
        }
    }
#endif
    
    DmaCtlVal = adi_usbdrv_GetDmaCtrl(pUsbDrvData, pEpInfo);
    assert((DmaCtlVal & 0x1) == 0);

    pRegs = pUsbDrvData->pRegs;

    /* reset any previous value */
    adi_usbdrv_SetDmaCtrl(pUsbDrvData, pEpInfo,(uint16_t)0u);

    /* setup start address */
    adi_usbdrv_SetDmaAddr(pUsbDrvData,
                          pEpInfo,
                          (uint16_t)ADI_USB_ADDR_LOW(pEpInfo->pEpBufferdata),
                          (uint16_t)ADI_USB_ADDR_HIGH(pEpInfo->pEpBufferdata));

    /* set up dma count */
    adi_usbdrv_SetDmaCount(pUsbDrvData,
                            pEpInfo,
                            (uint16_t)ADI_USB_ADDR_LOW(pEpInfo->EpProcBytesThisPass),
                            (uint16_t)ADI_USB_ADDR_HIGH(pEpInfo->EpProcBytesThisPass));

    if(pEpInfo->EpDir == ADI_USB_EP_DIR_RX)
    {
        DmaCtlVal = ENUM_USB_DMA_CTL_DMAINTEN |
                    ENUM_USB_DMA_CTL_DMAEN    |
                    (pEpInfo->EpPhyNum << 4u);
    }
    else
    {
        DmaCtlVal = ENUM_USB_DMA_CTL_DMAEN     |
                    ENUM_USB_DMA_CTL_DMADIR_TX |
                    ENUM_USB_DMA_CTL_DMAINTEN  |
                    (pEpInfo->EpPhyNum << 4u);

#if defined(ADI_CFG_USB_BF_MUSBHDRC)
        if (pEpInfo->EpType == ADI_USB_EP_TYPE_ISOC)
        {
            pRegs->TXCOUNT = pEpInfo->EpProcBytesThisPass;
        }
#endif
        pEpInfo->EpTxDmaPend = true;
    }

#if (ADI_CFG_USB_ENABLE_DMA_MODE1 == 1)
    if(pEpInfo->EpTransferMode == ADI_USB_EP_MODE_DMA_1)
    {
        DmaCtlVal |=  ENUM_USB_DMA_CTL_DMAMODE1;
        adi_usbdrv_SetDmaCtrl(pUsbDrvData, pEpInfo, DmaCtlVal);

        if(pEpInfo->EpDir == ADI_USB_EP_DIR_RX)
        {
            pRegs->EPI_RXCSR |= (ENUM_USB_EPI_RXCSR_H_AUTOCLR        |
#if (ADI_CFG_USB_OTG_HOST == 1)
                                   ENUM_USB_EPI_RXCSR_H_AUTOREQ      |
#endif
                                   ENUM_USB_EPI_RXCSR_H_DMAREQEN     |
                                   ENUM_USB_EPI_RXCSR_H_PKTRDY);     /* ENUM_USB_EPI_RXCSR_H_PKTRDY is set to avoid the clearing of RXPKTRDY bit accidently.
                                                                        It might be the case that RXPKTRDY bit is not set while reading RXCSR
                                                                        but it gets sets before we write to RXCSR */
        }
        else
        {
            pRegs->EPI_TXCSR |= (ENUM_USB_EPI_TXCSR_P_AUTOSET        |
                                   ENUM_USB_EPI_TXCSR_H_DMAREQEN     |
                                   ENUM_USB_EPI_TXCSR_H_DMARQMODE1);
        }
    }
    else
    {
        adi_usbdrv_SetDmaCtrl(pUsbDrvData, pEpInfo, DmaCtlVal);
    }
#else
    adi_usbdrv_SetDmaCtrl(pUsbDrvData, pEpInfo, DmaCtlVal);
#endif

    SSYNC();

    return ADI_USB_RESULT_SUCCESS;
}


/* Reset the device address */
static void  adi_usbdrv_ResetDevAddress (ADI_MUSBMHDRC_DATA *pUsbDrvData)
{
    ADI_USB_ENTER_CR();
    pUsbDrvData->DevAddr       = 0;
    pUsbDrvData->pRegs->FADDR  = 0;
    ADI_USB_EXIT_CR();
}

static void adi_usbdrv_SetBusEventHandlers(ADI_MUSBMHDRC_DATA *pUsbDrvData)
{
    switch(pUsbDrvData->DevMode)
    {
#if (ADI_CFG_USB_OTG_HOST == 1)
        case ADI_USB_MODE_OTG_HOST:
        {
            pUsbDrvData->pfSuspendHandler = adi_usbdrvh_SuspendHandler;
            pUsbDrvData->pfConnectHandler = adi_usbdrvh_ConnectHandler;
            pUsbDrvData->pfResetHandler   = adi_usbdrvh_ResetHandler;
            pUsbDrvData->pfEp0Handler     = adi_usbdrvh_Ep0Handler;
        }
        break;
#else
        case ADI_USB_MODE_DEVICE:
        {
            pUsbDrvData->pfSuspendHandler = adi_usbdrvd_SuspendHandler;
            pUsbDrvData->pfConnectHandler = adi_usbdrvd_ConnectHandler;
            pUsbDrvData->pfResetHandler   = adi_usbdrvd_ResetHandler;
            pUsbDrvData->pfEp0Handler     = adi_usbdrvd_Ep0Handler;
        }
        break;
#endif
        default:
        	break;

    }
}


static void adi_usbdrv_SendResumeSignal(ADI_MUSBMHDRC_DATA *pUsbDrvData)
{
    volatile ADI_USB_REGS_DEF *pRegs = pUsbDrvData->pRegs;

    pRegs->POWER |= ENUM_USB_POWER_RESUME;
    adi_usbdrv_WaitMilliSec(20);
    pRegs->POWER &= ~ENUM_USB_POWER_RESUME;
}

#if defined(ADI_CFG_USB_BF_MUSBMHDRC)
/*
 * This function sets the dynamic FIFO addresses linearly and returns error 
 * in case it can not allocate requested memory. Once memory is allocated for an
 * endpoint FIFO, it can not be re-used or re-assigned for another endpoint during
 * the current configuration is active.
 */
static ADI_USB_RESULT adi_usbdrv_SetDynamicFifoParams(ADI_MUSBMHDRC_DATA *pUsbDrvData, ADI_USB_EP_INFO *pEpInfo,bool bDoubleBuffer)
{
    uint32_t EpSavedIndex,EpFifoSize,EpCfgSz,index;
    volatile ADI_USB_REGS_DEF *pRegs;

    assert(pUsbDrvData     != NULL);                 assert(pEpInfo            != NULL);
    assert(pEpInfo->EpDir  != ADI_USB_EP_DIR_SETUP); assert(pEpInfo->EpMaxSize < 4096);

    pRegs = pUsbDrvData->pRegs;

    /* double buffering is enabled we allocated twice the size in the fifo */
    EpFifoSize = (bDoubleBuffer ? (pEpInfo->EpMaxSize << 1) : pEpInfo->EpMaxSize);
    EpCfgSz  = pEpInfo->EpMaxSize >> 3;

    if(EpCfgSz == 0){
        index  = 0;
    }
    else
    {
        /* maximum possible fifosz [SZ:0:3] is 9 and max epsize could go up to 4096 */
        for ( index = 1; index < 10; index++) {
           if ((1ul << index) >= EpCfgSz) break;
        }
    }
    assert(index != 10);       
    /* FIFO ram is organized as multiples of 8 with respect to index so round it to nearest multiple of 8 */
    EpFifoSize = ((EpFifoSize + 7) & ~7u);

    /* check whether we have enough memory available */
    if( (pUsbDrvData->FifoRamAdress + EpFifoSize) > pUsbDrvData->FifoRamTotalSize) {
        return (ADI_USB_RESULT_FAIL);
    }
    ADI_USB_ENTER_CR();
    EpSavedIndex     = pRegs->INDEX;
    pRegs->INDEX   = pEpInfo->EpPhyNum;

    if (pEpInfo->EpDir == ADI_USB_EP_DIR_TX) {
        /* fifo ram goes by multiples of 8, For ex: addr 64 means we have to set 8 */
        *pREG_USB0_TXFIFOADDR |= ((pUsbDrvData->FifoRamAdress >> 3) & 0x1FFF);
        *pREG_USB0_TXFIFOSZ   |= ((index | (bDoubleBuffer ? BITM_USB_TXFIFOSZ_DPB : 0)));
    }
    else {
        /* fifo ram goes by multiples of 8, For ex: addr 64 means we have to set 8 */
        *pREG_USB0_RXFIFOADDR |= ((pUsbDrvData->FifoRamAdress >> 3) & 0x1FFF);
        *pREG_USB0_RXFIFOSZ   |= ((index | (bDoubleBuffer ? BITM_USB_RXFIFOSZ_DPB : 0)));
    }
    pUsbDrvData->FifoRamAdress += EpFifoSize;
    pRegs->INDEX = EpSavedIndex;
    ADI_USB_EXIT_CR();
    return(ADI_USB_RESULT_SUCCESS);
}

#if (ADI_CFG_USB_OTG_HOST == 1)
/* this function should only get called when operating with a low speed or full speed device
 * connected via hub
 */
static ADI_USB_RESULT adi_musbmhdrc_SetHubParams(ADI_USB_HANDLE const hDevice, uint32_t const HubData,
                                          uint32_t const DevSpeed, uint32_t const EpData)
{
#if (ADI_USBH_MULTI_POINT_EN == 1)
     ADI_MUSBMHDRC_DATA        *pUsbDrvData = (ADI_MUSBMHDRC_DATA *)hDevice;
     ADI_USB_EP_INFO           *pEpInfo;
     volatile ADI_USB_REGS_DEF *pRegs;
     uint16_t                  SavedEpNum,EpPhyNum;

     ADI_USB_VALIDATE_HANDLE(hDevice);

     ADI_USB_ENTER_CR();

     pEpInfo =  adi_usbdrv_GetEpInfo(pUsbDrvData, EpData);

     assert(pEpInfo != NULL);
     pRegs = pUsbDrvData->pRegs;

     if (pEpInfo->EpPhyNum == 0)
     {
         pRegs->MPRegs[0].MP_TXHUBADDR = ADI_USB_GET_HUBADDR(HubData);
         pRegs->MPRegs[0].MP_TXHUBPORT = ADI_USB_GET_HUBPORT(HubData);
         pRegs->EP0I_TYPE0   |= (DevSpeed == ADI_USB_SPD_FULL)  ?
                                  ENUM_USB_EPI_RXTYPE_FULLSPEED :
                                  ENUM_USB_EPI_RXTYPE_LOWSPEED;

         ADI_USB_EXIT_CR();
         return ADI_USB_RESULT_SUCCESS;
     }

     SavedEpNum     = pRegs->INDEX;
     pRegs->INDEX = pEpInfo->EpPhyNum;
     EpPhyNum       = pEpInfo->EpPhyNum;

     if (pEpInfo->EpDir == ADI_USB_EP_DIR_TX)
     {
         pRegs->MPRegs[EpPhyNum].MP_TXFUNCADDR = (EpData >> 16) & 0xFF;
         pRegs->MPRegs[EpPhyNum].MP_TXHUBADDR  = ADI_USB_GET_HUBADDR(HubData);
         pRegs->MPRegs[EpPhyNum].MP_TXHUBPORT  = ADI_USB_GET_HUBPORT(HubData);
         pRegs->EPI_TXTYPE                    |= (DevSpeed == ADI_USB_SPD_FULL)   ?
                                                    ENUM_USB_EPI_RXTYPE_FULLSPEED :
                                                    ENUM_USB_EPI_RXTYPE_LOWSPEED;
     }
     else if (pEpInfo->EpDir == ADI_USB_EP_DIR_RX)
     {
         pRegs->MPRegs[EpPhyNum].MP_RXFUNCADDR = (EpData >> 16) & 0xFF;
         pRegs->MPRegs[EpPhyNum].MP_RXHUBADDR  = ADI_USB_GET_HUBADDR(HubData);
         pRegs->MPRegs[EpPhyNum].MP_RXHUBPORT  = ADI_USB_GET_HUBPORT(HubData);
         pRegs->EPI_RXTYPE                              |= (DevSpeed == ADI_USB_SPD_FULL)   ?
                                                              ENUM_USB_EPI_RXTYPE_FULLSPEED :
                                                              ENUM_USB_EPI_RXTYPE_LOWSPEED;
     }

     pRegs->INDEX = SavedEpNum;

     ADI_USB_EXIT_CR();
#endif
     return ADI_USB_RESULT_SUCCESS;
}
#endif
#endif

/* allocates dma channel and locks it with the endpoint if bChannelLock is true */
static ADI_USB_RESULT adi_usbdrv_AllocateDmaChannel(ADI_MUSBMHDRC_DATA *pUsbDrvData, ADI_USB_EP_INFO *pEpInfo,bool bChannelLock)
{
    int32_t  idx =0;
    ADI_USB_RESULT Result = ADI_USB_RESULT_SUCCESS;
    uint32_t ChannelMap=0;    
    assert(pUsbDrvData != NULL); assert(pEpInfo != NULL);

    ADI_USB_ENTER_CR();

    ChannelMap = pUsbDrvData->DmaChannelBitMap;
    
    if(pEpInfo->EpDmaChannel)
    {
      return Result; /* DMA channel already allocated */
    }
    else
    {
        /* dma channel zero is for ep0 */
        for (idx = 1; idx < ADI_CFG_USB_MAX_DMA_CHANNELS; idx++)
        {
           if (!((ChannelMap & (1u << idx)) == (1u << idx)))
           {
               break;
           }
        }

        if (idx < ADI_CFG_USB_MAX_DMA_CHANNELS)
        {
            pUsbDrvData->DmaChannelBitMap  |= (1u << idx);
            pEpInfo->EpDmaChannel = idx;
            pEpInfo->EpDmaChannelLck = bChannelLock;
        }
        else
        {
            ADI_USB_EP_STATINC(pUsbDrvData,pEpInfo->EpPhyIdx,nEPDmaBusy);
            Result = ADI_USB_RESULT_DMA_CHANNEL_BUSY;
        }
    }

    ADI_USB_EXIT_CR();

    return Result;
}

static ADI_USB_RESULT adi_usbdrv_FreeDmaChannel(ADI_MUSBMHDRC_DATA *pUsbDrvData, ADI_USB_EP_INFO *pEpInfo)
{
    assert(pUsbDrvData != NULL); assert(pEpInfo != NULL);
    //assert(pEpInfo->EpDmaChannelLck != true);

    ADI_USB_ENTER_CR();

    pUsbDrvData->DmaChannelBitMap  &= ~(1u << pEpInfo->EpDmaChannel);
    pEpInfo->EpDmaChannel    = 0;

    pEpInfo->EpDmaChannelLck = false;

    ADI_USB_EXIT_CR();
    return (ADI_USB_RESULT_SUCCESS);
}

static ADI_USB_RESULT adi_usbdrv_ReleaseAllDmaChannels(ADI_MUSBMHDRC_DATA *pUsbDrvData)
{
    pUsbDrvData->DmaChannelBitMap  = 0;
    return (ADI_USB_RESULT_SUCCESS);
}


#if defined(ADI_DEBUG)
static ADI_USB_RESULT ValidateHandle(ADI_USB_HANDLE *hDevice)
{
    uint32_t i=0;

    for(i = 0; i < (uint32_t)ADI_CFG_USB_MAX_NBR_HC; i++)
    {
        if(hDevice && (hDevice == (ADI_USB_HANDLE*)&gUsbDrvData[i]))
        {
            return ADI_USB_RESULT_SUCCESS;
        }
    }

    return ADI_USB_RESULT_INVALID_HANDLE;
}
#endif


/*
 * Controller Driver API
 */
#if (ADI_CFG_USB_OTG_HOST == 1)
ADI_USB_DRV_API  ADI_MUSBMHDRC_DrvAPI =
#else
ADI_USB_DRV_API  ADI_MUSBMHDRC_DeviceDrvAPI =
#endif
{
    adi_musbmhdrc_Init,
    adi_musbmhdrc_Start,
    adi_musbmhdrc_Stop,
    adi_musbmhdrc_SpdGet,
    adi_musbmhdrc_Suspend,
    adi_musbmhdrc_Resume,
    adi_musbmhdrc_FrameNbrGet,

    adi_musbmhdrc_EP_Open,
    adi_musbmhdrc_EP_Close,
    adi_musbmhdrc_EP_Abort,
    adi_musbmhdrc_IsHalt_EP,
    adi_musbmhdrc_EP_ResetDataToggle,

    adi_musbmhdrc_SetDevAddress,

    adi_musbmhdrc_SubmitURB,
    adi_musbmhdrc_CompleteURB,
    adi_musbmhdrc_AbortURB,

    adi_musbmhdrc_RegisterEpZeroCallback,
    adi_musbmhdrc_RegisterEpDataCallback,
    adi_musbmhdrc_RegisterBusEventCallback,
#if defined(ADI_CFG_USB_BF_MUSBMHDRC) && (ADI_CFG_USB_OTG_HOST == 1)
    adi_musbmhdrc_SetHubParams,
#endif
#if (ADI_CFG_USB_DEVICE == 1)
    adi_musbmhdrc_EP_Stall,
#endif
    adi_musbmhdrc_GetURBState,
};

#if (ADI_CFG_USB_OTG_HOST == 1)
/*
 * Root hub API 
 */
ADI_USB_RH_API  ADI_MUSBMHDRC_RhAPI =
{
    adi_musbmhdrc_PortStatusGet,
    adi_musbmhdrc_HubDescGet,

    adi_musbmhdrc_PortEnSet,
    adi_musbmhdrc_PortEnClr,
    adi_musbmhdrc_PortEnChngClr,

    adi_musbmhdrc_PortPwrSet,
    adi_musbmhdrc_PortPwrClr,

    adi_musbmhdrc_PortResetSet,
    adi_musbmhdrc_PortResetChngClr,

    adi_musbmhdrc_PortSuspendClr,
    adi_musbmhdrc_PortConnChngClr,

    adi_musbmhdrc_RHIntEn,
    adi_musbmhdrc_RHIntDis,

    adi_musbmhdrc_PortConnSet,
    adi_musbmhdrc_PortConnChngSet
};
#endif

#ifdef _MISRA_RULES
#pragma diag(pop)
#endif

/*@}*/
