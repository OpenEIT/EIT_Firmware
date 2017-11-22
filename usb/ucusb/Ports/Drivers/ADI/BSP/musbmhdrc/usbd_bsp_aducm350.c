/*
*********************************************************************************************************
*                                    uC/USB-Device Controller Driver
*                                      ADI Driver Interface Layer
*
*                    Copyright(c) 2012-2014 Analog Devices, Inc. All Rights Reserved.
*
*               This software is proprietary and confidential. By using this software you agree
*               to the terms of the associated Analog Devices License Agreement.
*
*                          (c) Copyright 2004-2011; Micrium, Inc.; Weston, FL
*
*               All rights reserved. Protected by international copyright laws.
*
*               uC/USB is provided in source form to registered licensees ONLY.  It is
*               illegal to distribute this source code to any third party unless you receive
*               written permission by an authorized Micrium representative.  Knowledge of
*               the source code may NOT be used to develop a similar product.
*
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*
*               You can contact us at www.micrium.com.
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*
*                                  USB DEVICE DRIVER BOARD-SPECIFIC FUNCTIONS
*                                      ADI device driver implementation
*                                                  ADuCM350
*
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                              INCLUDE FILES
*********************************************************************************************************
*/

#define   USBD_ADI_MUSBMHDRC_BSP_MODULE
#include  <usbd_core.h>

#include <drivers/usb/controller/device/adi_usbd_dev.h>


/*
*********************************************************************************************************
*                                              LOCAL DEFINES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             LOCAL CONSTANTS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                              LOCAL TABLES
*
*********************************************************************************************************
*/

const  USBD_DRV_EP_INFO  USBD_DrvEP_InfoTbl_MUSBMHDRC[] = {

    {USBD_EP_INFO_TYPE_CTRL | USBD_EP_INFO_DIR_OUT,  0u,   64u},
    {USBD_EP_INFO_TYPE_CTRL | USBD_EP_INFO_DIR_IN,   0u,   64u},

    {USBD_EP_INFO_TYPE_INTR | USBD_EP_INFO_TYPE_BULK | USBD_EP_INFO_TYPE_ISOC | USBD_EP_INFO_DIR_OUT,  1u,  128u},
    {USBD_EP_INFO_TYPE_INTR | USBD_EP_INFO_TYPE_BULK | USBD_EP_INFO_TYPE_ISOC | USBD_EP_INFO_DIR_IN,   1u,  128u},

    {USBD_EP_INFO_TYPE_INTR | USBD_EP_INFO_TYPE_BULK | USBD_EP_INFO_TYPE_ISOC | USBD_EP_INFO_DIR_OUT,  2u,  256u},
    {USBD_EP_INFO_TYPE_INTR | USBD_EP_INFO_TYPE_BULK | USBD_EP_INFO_TYPE_ISOC | USBD_EP_INFO_DIR_IN,   2u,  256u},

    {USBD_EP_INFO_TYPE_INTR | USBD_EP_INFO_TYPE_BULK | USBD_EP_INFO_TYPE_ISOC | USBD_EP_INFO_DIR_OUT,  3u,  512u},
    {USBD_EP_INFO_TYPE_INTR | USBD_EP_INFO_TYPE_BULK | USBD_EP_INFO_TYPE_ISOC | USBD_EP_INFO_DIR_IN,   3u,  512u},

    {DEF_BIT_NONE                                ,   0u,    0u}
};


/*
*********************************************************************************************************
*                                        LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  USBD_BSP_MUSBMHDRC_Init     (USBD_DRV *p_drv);
static  void  USBD_BSP_MUSBMHDRC_Conn     (void);
static  void  USBD_BSP_MUSBMHDRC_Disconn  (void);

static  void  USBD_BSP_MUSBMHDRC_IntHandler(void      *p_arg);


/*
*********************************************************************************************************
*                                    USB DEVICE DRIVER BSP INTERFACE
*
* Note(s) : (1) 'USBD_DrvBSP_MUSBMHDRC_A' configure the USB device to be used in USB channel A.
*
*                (a) 'USBD_DrvBSP_MUSBMHDRC_A' is not implemented yet.
*
*           (2) 'USBD_DrvBSP_MUSBMHDRC_B' configure the USB device to be used in USB channel B.
*********************************************************************************************************
*/
                                                                /* See note (1).                                        */
const  USBD_DRV_BSP_API   USBD_DrvBSP_ADI = {  USBD_BSP_MUSBMHDRC_Init,
                                                       USBD_BSP_MUSBMHDRC_Conn,
                                                       USBD_BSP_MUSBMHDRC_Disconn,
                                                    };

                                                                /* See note (2).                                        */


/*
*********************************************************************************************************
*                                       LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                        USBD_BSP_MUSBMHDRC_InitA()
*
* Description : USB device controller board-specific initialization.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Device controller driver init function via 'p_bsp_api->Init()'.
*
* Note(s)     : (1) The PLL initialization is done in the board support package.
*********************************************************************************************************
*/

static  void  USBD_BSP_MUSBMHDRC_Init (USBD_DRV  *p_drv)
{
    /*
     * Not required for ADI drivers
    */
}


/*
*********************************************************************************************************
*                                          USBD_BSP_MUSBMHDRC_ConnA()
*                                          USBD_BSP_MUSBMHDRC_ConnB()
*
* Description : Connect pull-up on DP and enable int.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Device controller driver init function via 'p_bsp_api->Conn()'.
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  USBD_BSP_MUSBMHDRC_Conn (void)
{
    /*
     * Not required for ADI drivers
    */
}


static  void  USBD_BSP_MUSBMHDRC_Disconn (void)
{
    /*
     * Not required for ADI drivers
    */
}


/*
*********************************************************************************************************
*                                         USBD_BSP_MUSBMHDRC_IntHandler()
*
* Description : USB device interrupt handler.
*
* Argument(s) : p_arg.  Interrupt handler argument.
*
* Return(s)   : none.
*
* Caller(s)   : This is a ISR.
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  USBD_BSP_MUSBMHDRC_IntHandler (void  *p_arg)
{
    /*
     * Not required for ADI drivers
    */
}
