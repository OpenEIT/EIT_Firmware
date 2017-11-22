/*********************************************************************************

  Copyright(c) 2013 Analog Devices, Inc. All Rights Reserved.

  This software is proprietary and confidential.  By using this software you agree
  to the terms of the associated Analog Devices License Agreement.

*********************************************************************************/

/*!
* @file      adi_usb_dev_musbmhdrc.h
*
* @brief     USB controller driver header for MUSBMHDRC and MUSBHDRC based controllers.
*
*            Contains the enumerations and structures that are accessed by the interface
*            layer. 
*
*/

#ifndef _ADI_USB_DEV_MUSBMHDRC_H_
#define _ADI_USB_DEV_MUSBMHDRC_H_

/** @addtogroup USB MUSBMHDRC controller driver
 *  @{
 */

#ifdef _MISRA_RULES
#pragma diag(push)
#pragma diag(suppress:misra_rules_all:"Suppress all misra rules")
#endif

#if (ADI_CFG_USB_OTG_HOST == 1)
/* Device will be connected to the Port 1 of Root Hub as hard coded by stack */
#define ADI_USB_DEV_CONN_PORT                0x1  

/* maximum number of port in Root hub */
#define ADI_MAX_NUM_RH_PORT                  0x7
#endif

#if defined(ADI_ADUCM350)
#define ADI_USB_CFG_MAX_NUM_QUEUED_BUFS      2
#else
#define ADI_USB_CFG_MAX_NUM_QUEUED_BUFS      60
#endif
typedef void* ADI_USB_HANDLE;

/*
 * USB Controller Driver Result codes
 */
typedef enum
{
    ADI_USB_RESULT_SUCCESS,              /*!< USB API is successful         */
    ADI_USB_RESULT_FAIL,                 /*!< USB API failed                */
    ADI_USB_RESULT_INVALID_DEV_MODE,     /*!< Invalid device mode           */
    ADI_USB_RESULT_EP_ALLOC_FAILED,      /*!< Endpoint allocation failed    */
    ADI_USB_RESULT_EP_NOT_FREE,          /*!< Endpoint is not free          */
    ADI_USB_RESULT_EP_INVALID,           /*!< Invalid endpoint              */
    ADI_USB_RESULT_EP_STALL,             /*!< Given EP is stalled           */
    ADI_USB_RESULT_EP_NAK,               /*!< Given EP is NAKed             */
    ADI_USB_RESULT_EP_ERROR,             /*!< No response for given EP      */
    ADI_USB_RESULT_FIFO_FULL,            /*!< Endpoint FIFO is full         */
    ADI_USB_RESULT_NULL_POINTER,         /*!< Null pointer is passed        */
    ADI_USB_RESULT_BUF_INVALID,          /*!< Buffer NULL/Invalid           */
    ADI_USB_RESULT_INVALID_HANDLE,       /*!< Supplied handle is invalid    */
    ADI_USB_RESULT_URB_COMPLETE,         /*!< Submitted URB is completed    */
    ADI_USB_RESULT_URB_PENDING,          /*!< Supplied handle is pending    */
    ADI_USB_RESULT_URB_ABORT ,           /*!< Submitted URB Aborted         */
    ADI_USB_RESULT_URB_PARTIAL,          /*!< Submitted URB not completed but ZLP or short packet is received */
    ADI_USB_RESULT_DMA_CHANNEL_BUSY,     /*!< All the DMA channels are busy */
    ADI_USB_RESULT_ERR_MAX
} ADI_USB_RESULT;


/*
 * USB Controller driver events 
 */
typedef enum
{
    ADI_USB_BUS_EVENT_DEV_CON,          /*!< Device is connected           */
    ADI_USB_BUS_EVENT_DEV_DISCON,       /*!< Device is disconnected        */
    ADI_USB_BUS_EVENT_RESUME,           /*!< Device is resumed             */
    ADI_USB_BUS_EVENT_SUSPEND,          /*!< Device is suspended           */
    ADI_USB_BUS_EVENT_RESET,            /*!< Device is reset               */
    ADI_USB_BUS_EVENT_HS_DETECTED       /*!< High speed Device is detected */

} ADI_USB_BUS_EVENT;

/*
 *  USB device speed 
 */ 
typedef enum  
{
    ADI_USB_SPD_NONE,                  /*!< Device speed is unknown       */
    ADI_USB_SPD_LOW,                   /*!< Low speed operating device    */
    ADI_USB_SPD_FULL,                  /*!< Full speed operating device   */
    ADI_USB_SPD_HIGH                   /*!< High speed operating device   */

} ADI_USB_SPEED;


/*
 * Endpoint transfer direction
 */
typedef enum 
{
    ADI_USB_EP_DIR_NONE,
    ADI_USB_EP_DIR_SETUP,             /*!< Setup token will be send      */
    ADI_USB_EP_DIR_RX,                /*!< OUT token will be send        */
    ADI_USB_EP_DIR_TX                 /*!< IN token will be send         */

}ADI_USB_EP_DIR;

/*
 * Endpoint transfer direction
 */
typedef enum
{
    ADI_USB_EP_STATE_NONE,
    ADI_USB_EP_STATE_SETUP,             /*!< Setup token will be send      */
    ADI_USB_EP_STATE_RX,                /*!< OUT token will be send        */
    ADI_USB_EP_STATE_TX                 /*!< IN token will be send         */

}ADI_USB_EP_STATE;

/*
 * Transfer type
 */
typedef enum 
{
    ADI_USB_EP_TYPE_CONTROL,    /*!< Control Setup transfer type    */
    ADI_USB_EP_TYPE_ISOC,            /*!< Isonchronous transfer type     */
    ADI_USB_EP_TYPE_BULK,            /*!< Bulk transfer type             */
    ADI_USB_EP_TYPE_INTERRUPT       /*!< Interrup transfer type         */
}ADI_USB_EP_TYPE;


/*
 * Endpoint numbers
 */
typedef enum
{
    ADI_USB_EP_NUM_0 = 0,
    ADI_USB_EP_NUM_1,
    ADI_USB_EP_NUM_2,
    ADI_USB_EP_NUM_3
} ADI_USB_EP_NUM;

/*
 * EP transfer modes 
 */
typedef enum
{
    ADI_USB_EP_MODE_NONE,
    ADI_USB_EP_MODE_DMA_0,         /*!< Single Packet DMA Mode (Mode 0) */
    ADI_USB_EP_MODE_DMA_1,         /*!< Multi Packet DMA Mode (Mode 1)  */
    ADI_USB_EP_MODE_PIO            /*!< Programmed I/O mode             */
}ADI_USB_EP_MODE;


/*
 * URB State
 */
typedef enum 
{
    ADI_USB_URB_STATE_FREE,
    ADI_USB_URB_STATE_SUBMITTED,
    ADI_USB_URB_STATE_PROCESSING
} ADI_URB_STATE;

/*
 * Hub port status
 */
typedef  struct  
{
    uint16_t  PortStatus;            /*!< Port status                   */
    uint16_t  PortChange;            /*!< Port status changed           */

} ADI_USB_HUB_PORTSTATUS;


typedef struct
{
    uint8_t   *pUnitData;
    uint32_t  UnitTransferLen;
    bool      UnitValidFlag;

}ADI_USB_URB_DATAUNIT;
/* 
 * USB request block (URB)
 */
typedef struct
{
    uint8_t         EpAddr;           /*!< URB's Logical Endpoint Address      */
    uint8_t         EpType;           /*!< URB's Logical Endpoint type         */
    ADI_USB_EP_DIR  EpDir;            /*!< URB's Logical Endpoint Address      */
    uint32_t        TransferLen;      /*!< Transfer length                     */
    uint32_t        ProcessedBytes;   /*!< Total number of Processed bytes     */
    uint8_t         *pData;           /*!< Buffer pointer                      */
    uint8_t         DevAddr;          /*!< Device address                      */
    void            *pHostSubmitURB;  /*!< URB submitted by USB Stack          */
    ADI_USB_RESULT  EpResult;         /*!< URB Result                          */
    ADI_USB_URB_DATAUNIT UrbUnits[ADI_USB_CFG_MAX_NUM_QUEUED_BUFS];
    uint32_t        CurWriteIdx;
    uint32_t        CurReadIdx;
    uint32_t        CurUrbCount;
} ADI_USB_URB;


/*
 * Endpoint Descriptor
*/

typedef  struct {
    uint8_t  Length;
    uint8_t  DescriptorType;
    uint8_t  EndpointAddress;
    uint8_t  Attributes;
    uint16_t MaxPacketSize;
    uint8_t  Interval;
    uint8_t  Refresh;
    uint8_t  SynchAddress;
} ADI_USB_EP_DESC;

typedef enum
{
    ADI_USB_MODE_NONE,
    ADI_USB_MODE_DEVICE,
    ADI_USB_MODE_OTG_HOST
}ADI_USB_DEV_MODE;

/*
 * Initialization data 
 */
typedef  struct 
{
    uint8_t          DevNum;       /*!< Device number    */
    ADI_USB_DEV_MODE DevMode;      /*!< Driver mode      */
    uint8_t          SetupPkt[8];
} ADI_USB_INIT_DATA;

/*
 * USB HOST CONTROLLER DRIVER API
 */

struct  adi_usb_drv_api 
{
    ADI_USB_RESULT          (*Init)                (ADI_USB_HANDLE    *hDevice,           /* Initialize Controller.                               */
                                                    ADI_USB_INIT_DATA *pInitData);

    ADI_USB_RESULT          (*Start)               (ADI_USB_HANDLE    const hDevice);

    ADI_USB_RESULT          (*Stop)                (ADI_USB_HANDLE    const hDevice);           /* Stop  HC.                                            */

    ADI_USB_SPEED           (*SpdGet)              (ADI_USB_HANDLE    const hDevice);           /* Get HC speed.                                        */

    ADI_USB_RESULT          (*Suspend)             (ADI_USB_HANDLE    const hDevice);           /* Suspend HC.                                          */

    ADI_USB_RESULT          (*Resume)              (ADI_USB_HANDLE    const hDevice);           /* Resume HC.                                           */

    uint32_t                (*FrameNbrGet)         (ADI_USB_HANDLE    const hDevice);           /* Get HC frame number.                                 */

    ADI_USB_RESULT          (*EP_Open)             (ADI_USB_HANDLE    const hDevice,
                                                    ADI_USB_EP_DESC   *pEpDesc,
                                                    uint8_t           DevAddr);

    ADI_USB_RESULT          (*EP_Close)            (ADI_USB_HANDLE    const hDevice,            /* Close endpoint.                                      */
                                                    const uint32_t           EpData);

    ADI_USB_RESULT          (*EP_Abort)            (ADI_USB_HANDLE    const hDevice,            /* Abort all pending URB on an endpoint.                */
                                                    const uint32_t           EpData);

    uint8_t                 (*EP_IsHalt)           (ADI_USB_HANDLE    const hDevice,            /* Get endpoint halt status.                            */
                                                    const uint32_t           EpData);

    ADI_USB_RESULT          (*EP_ResetDataToggle)  (ADI_USB_HANDLE    const hDevice,            /* Clear EP data toggle bit.                            */
                                                    const uint32_t           EpData);

    ADI_USB_RESULT          (*SetDevAddr)          (ADI_USB_HANDLE    const hDevice,            /* Set Device address.                                  */
                                                    uint8_t           DevAddr);

    ADI_USB_RESULT          (*URB_Submit)          (ADI_USB_HANDLE    const hDevice,
                                                    ADI_USB_URB       *pUsbUrb);

    ADI_USB_RESULT          (*URB_Complete)        (ADI_USB_HANDLE    const hDevice,
                                                    ADI_USB_URB       *pUsbUrb);

    ADI_USB_RESULT          (*URB_Abort)           (ADI_USB_HANDLE    const hDevice,            /* Abort a URB.                                         */
                                                    ADI_USB_URB       *pUsbUrb);
#if 0// (ADI_CFG_USB_DEVICE == 1)
    uint32_t                (*Ep_GetRxBytes)       (ADI_USB_HANDLE    const hDevice,            /* Abort a URB.                                         */
                                                    uint8_t           EpAddr);
#endif

    ADI_USB_RESULT          (*RegEpZeroCallback)   (ADI_USB_HANDLE    const hDevice,
                                                    ADI_CALLBACK      pfEpZeroCallback);

    ADI_USB_RESULT          (*RegEpDataCallback)   (ADI_USB_HANDLE    const hDevice,
                                                    ADI_CALLBACK      pfEpDataCallback);

    ADI_USB_RESULT          (*RegBusEventCallback) (ADI_USB_HANDLE    const hDevice,
                                                    ADI_CALLBACK      pfBusEventCallback);

#if defined(ADI_CFG_USB_BF_MUSBMHDRC) && (ADI_CFG_USB_OTG_HOST == 1)
    ADI_USB_RESULT          (*SetHubParams)         (ADI_USB_HANDLE   const hDevice,
                                                     uint32_t         const HubAddr,
                                                     uint32_t         const HubPort,
                                                     uint32_t         const EpData);
#endif
#if (ADI_CFG_USB_DEVICE == 1)
    ADI_USB_RESULT          (*EP_Stall)             (ADI_USB_HANDLE   const hDevice,
    		                                         const uint32_t   EpData,
    		                                         const bool bEpStall);
#endif
    ADI_URB_STATE           (*EP_URB_GetState)      (ADI_USB_HANDLE    const hDevice,
     											     const uint32_t    EpData);
};


/*
*********************************************************************************************************
*                               USB HOST CONTROLLER ROOT HUB DRIVER API
*********************************************************************************************************
*/

struct  adi_usb_rh_api {

    ADI_USB_RESULT          (*PortStatusGet)       (ADI_USB_HANDLE    const hDevice,            /* Get port status.                                     */
                                                    const uint32_t    PortNumber,
                                                    ADI_USB_HUB_PORTSTATUS *pPortStatus);

    ADI_USB_RESULT          (*HubDescGet)          (ADI_USB_HANDLE    const hDevice,            /* Get RH descriptor.                                   */
                                                    void              *pBuf,
                                                    const uint32_t    BufLen);

    ADI_USB_RESULT          (*PortEnSet)           (ADI_USB_HANDLE    const hDevice,            /* Set port enable.                                     */
                                                    const uint32_t    PortNumber);

    ADI_USB_RESULT          (*PortEnClr)           (ADI_USB_HANDLE    const hDevice,            /* Clear port enable.                                   */
                                                    const uint32_t    PortNumber);

    ADI_USB_RESULT          (*PortEnChngClr)       (ADI_USB_HANDLE    const hDevice,            /* Clear port enable change.                            */
                                                    const uint32_t    PortNumber);

    ADI_USB_RESULT          (*PortPwrSet)          (ADI_USB_HANDLE    const hDevice,            /* Set port power.                                      */
                                                    const uint32_t    PortNumber);

    ADI_USB_RESULT          (*PortPwrClr)          (ADI_USB_HANDLE    const hDevice,            /* Clear port power.                                    */
                                                    const uint32_t    PortNumber);

    ADI_USB_RESULT          (*PortResetSet)        (ADI_USB_HANDLE    const hDevice,            /* Set port reset.                                      */
                                                    const uint32_t    PortNumber);

    ADI_USB_RESULT          (*PortResetChngClr)    (ADI_USB_HANDLE    const hDevice,            /* Clear port reset change.                             */
                                                    const uint32_t    PortNumber);

    ADI_USB_RESULT          (*PortSuspendClr)      (ADI_USB_HANDLE    const hDevice,            /* Clear port suspend.                                  */
                                                    const uint32_t    PortNumber);

    ADI_USB_RESULT          (*PortConnChngClr)     (ADI_USB_HANDLE    const hDevice,            /* Clear port connection change.                        */
                                                    const uint32_t    PortNumber);

    ADI_USB_RESULT          (*IntEn)               (ADI_USB_HANDLE    const hDevice);            /* Enable RH interrupt.                                 */

    ADI_USB_RESULT          (*IntDis)              (ADI_USB_HANDLE    const hDevice);            /* Disable RH interrupt.                                */

    ADI_USB_RESULT          (*PortConnSet)         (ADI_USB_HANDLE    const hDevice,            /* Set port connection.                                    */
                                                    const uint32_t    PortNumber);

    ADI_USB_RESULT          (*PortConnChngSet)     (ADI_USB_HANDLE    const hDevice,            /* Clear port connection and set connection change      */
                                                    const uint32_t    PortNumber);

};

typedef  const  struct  adi_usb_drv_api    ADI_USB_DRV_API;
typedef  const  struct  adi_usb_rh_api     ADI_USB_RH_API;

#ifdef _MISRA_RULES
#pragma diag(pop)
#endif

/*@}*/

#endif /* _ADI_USB_DEV_MUSBMHDRC_H_ */
