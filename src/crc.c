/*********************************************************************************

Copyright (c) 2013-2014 Analog Devices, Inc. All Rights Reserved.

This software is proprietary to Analog Devices, Inc. and its licensors.  By using 
this software you agree to the terms of the associated Analog Devices Software 
License Agreement.

*********************************************************************************/

/*!
 *****************************************************************************
 * @file:    crc.c
 * @brief:   CRC  Device Implementations for ADuCRFxxx
 * @version: $Revision: 28525 $
 * @date:    $Date: 2014-11-12 14:51:26 -0500 (Wed, 12 Nov 2014) $
 *****************************************************************************/

/*! \addtogroup CRC_Driver CRC Driver
 *  @{
 */

#define ASSERT_ADI_MISRA_SUPPRESSIONS  /*!< Apply ADI MISRA Suppressions */
#include "misra.h"

#include <stdlib.h>  /* for 'NULL" definition */
#include "crc.h"
#include "dma.h"
#include "adi_crc_config.h"

#define ADI_DMA_DATA_WIDTH      ADI_DMA_WIDTH_WORD  /*!< DMA data attribute */
#define ADI_DMA_DATA_INCREMENT  ADI_DMA_INCR_WORD   /*!< DMA data attribute */
#define MAX_CRC_TRANSFER_SIZE   1024               /*!<  Maximum uinits of 32bit word for DMA transfer in one iteration. */

/*!
 *****************************************************************************
 * \enum ADI_CRC_STATE_TYPE
 *
 * CRC driver state.  Used for internal tracking of the CRC device initialization
 * progress during the adi_CRC_Init() and adi_CRC_UnInit() functions.  Also
 * used to insure the CRC device has been properly initialized as a prerequisite
 * to using the other CRC API.
 *
 *****************************************************************************/
typedef enum
{
    ADI_CRC_STATE_UNINITIALIZED= 0,        /*!< CRC is not initialized.            */
    ADI_CRC_STATE_INITIALIZED              /*!< CRC Initilized. */
} ADI_CRC_STATE_TYPE;

/*! \struct ADI_CRC_DEV_DATA_TYPE
 * CRC Device instance data structure */
typedef struct ADI_CRC_DEV_DATA_TYPE
{

    /* device attributes */
    ADI_CRC_STATE_TYPE       InitState;        /*!< track initialization state */
    ADI_CRC_TypeDef*         pDevice;             /*!< track MMR device pointer   */
    volatile bool_t          bCRCBusy;         /*!< DMA complete flag   */
    uint32_t                *pBuffer;          /*!< To store the buffer address */
    uint32_t                 unscheduledCnt;   /*!< DMA remaining count */
#if (ADI_CFG_ENABLE_CALLBACK_SUPPORT == 1)
    ADI_CALLBACK             pfCallback;       /*!< Pointer to callback function.       */
    void *                   pCBParam;         /*!< Callback parameter.                */
#endif
#if (ADI_CRC_CFG_ENABLE_DMA_SUPPORT == 1)
    ADI_DMA_CCD_TYPE         nextDescr;        /*!< DMA next  descriptor */

#endif
} ADI_CRC_DEV_DATA_TYPE;  /*!< internal instance data structure */

/* allocate instance data array */
ADI_CRC_DEV_DATA_TYPE CRC_DevData[ADI_CRC_MAX_DEVID];  /*!< Private CRC device driver instance data */

#if (ADI_CRC_CFG_ENABLE_DMA_SUPPORT == 1)
//static ADI_CRC_RESULT_TYPE ScheduleNextDescriptor   (ADI_CRC_DEV_HANDLE const hDevice);
    ADI_DMA_TRANSFER_TYPE  gDmaDescriptor  ;   /*!< DMA descriptor */
#else

static void ComputeCRC (ADI_CRC_DEV_HANDLE const hDevice);
#endif  /* ADI_CRC_CFG_ENABLE_DMA_SUPPORT */

/*!< Default CRC control word configuration */
#define ADI_CRC_DEFAULT_CTL_WORD  ( 0 <<  BITP_CRC_CRC_CTL_W16SWP   ) | \
                                  ( 0 <<  BITP_CRC_CRC_CTL_BYTMIRR  ) | \
                                  ( 1 <<  BITP_CRC_CRC_CTL_W16SWP   ) | \
                                  ( 0 <<  BITP_CRC_CRC_CTL_BITMIRR  ) | \
                                  ( ADI_CRC_CKSUM_00000000 <<  BITP_CRC_CRC_CTL_AUTORST  )


/* handle checker for debug mode */
#ifdef ADI_DEBUG
#define ADI_CRC_INVALID_HANDLE(h) ((pCrc_DevData != h) )
#endif

/* internal device data pointers */
static ADI_CRC_DEV_DATA_TYPE* const pCrc_DevData = &CRC_DevData[ADI_CRC_DEVID_0];


/* forward declarations... */

/* Internal API... */
void InitCRC               (ADI_CRC_DEV_HANDLE const hDevice);
/*
    //////////////////////////////////////////////////////////////////////////////
    //////////////////////   API IMPLEMENTATIONS   ///////////////////////////////
    //////////////////////////////////////////////////////////////////////////////
*/


/*!
 * @brief  Initialize and allocate an CRC device for use in Master Mode.
 *
 * @param[in]        devID      Zero-based device index designating which device to initialize.
 * @param[out]       phDevice   The caller's device handle pointer for storing the initialized device instance data pointer.
 *
 * @return            Status
 *                    - #ADI_CRC_ERR_ALREADY_INITIALIZED [D]    Specified device has already been initialized.
 *                    - #ADI_CRC_ERR_BAD_DEV_ID [D]             Invalid device index.
 *                    - #ADI_CRC_SUCCESS                        Call completed successfully.
 *
 * No other CRC APIs may be called until the device init function is called.
 *
 * Initialize an CRC device using internal default configuration settings and allocate the device for use.  The caller's
 * device handle is written with the device instance data pointer.  The returned device handle is required to be passed
 * to all subsequent calls to convey which device instance to operate on.
 *
 * @note    The contents of phDevice will be set to NULL upon failure.
 * @sa      adi_CRC_UnInit().
 */
ADI_CRC_RESULT_TYPE adi_CRC_Init (ADI_CRC_DEV_ID_TYPE const devID,
                                 ADI_CRC_DEV_HANDLE* const phDevice)
{
    ADI_CRC_DEV_HANDLE hDevice = &CRC_DevData[devID];
    ADI_CRC_RESULT_TYPE result = ADI_CRC_SUCCESS;

    /* store a bad handle in case of failure */
    *phDevice = (ADI_CRC_DEV_HANDLE) NULL;

#ifdef ADI_DEBUG
    if (devID >= ADI_CRC_MAX_DEVID)
        return ADI_CRC_ERR_BAD_DEV_ID;

    if (ADI_CRC_STATE_UNINITIALIZED != hDevice->InitState)
        return ADI_CRC_ERR_ALREADY_INITIALIZED;
#endif
    InitCRC( hDevice);
    hDevice->InitState = ADI_CRC_STATE_INITIALIZED;
    /* write the device data pointer into the caller's handle */
    *phDevice = hDevice;

    return result;
}

/*!
 * @brief  Uninitialize and deallocate an CRC device.
 *
 * @param[in]   hDevice    Device handle obtained from adi_CRC_Init().
 *
 * @return      Status
 *                - #ADI_CRC_ERR_BAD_DEV_HANDLE [D]     Invalid device handle parameter.
 *                - #ADI_CRC_ERR_NOT_INITIALIZED [D]    Device has not been previously configured for use.
 *                - #ADI_CRC_SUCCESS                    Call completed successfully.
 *
 * Uninitialize and release an allocated CRC device for other use.
 *
 * @sa        adi_CRC_Init().
 */
ADI_CRC_RESULT_TYPE adi_CRC_UnInit (ADI_CRC_DEV_HANDLE const hDevice)
{
#ifdef ADI_DEBUG
    if (ADI_CRC_INVALID_HANDLE(hDevice))
        return ADI_CRC_ERR_BAD_DEV_HANDLE;

    if (hDevice->InitState != ADI_CRC_STATE_INITIALIZED)
        return ADI_CRC_ERR_NOT_INITIALIZED ;
#endif
    /* uninit internals */
      InitCRC(hDevice);
    /* invalidate initialization state */
    hDevice->InitState = ADI_CRC_STATE_UNINITIALIZED;

    return ADI_CRC_SUCCESS;
}

/*!
 * @brief  Configure the autoreset.
 *
 * @param[in]   hDevice     Device handle obtained from adi_CRC_Init().
 * @param[in]   eAutoRst    Indicates what should be the the value of RESULT register when
 *                          CRC device is enabled.
 *
 * @return      Status
 *                - #ADI_CRC_ERR_BAD_DEV_HANDLE [D]     Invalid device handle parameter.
 *                - #ADI_CRC_ERR_NOT_INITIALIZED [D]    Device has not been previously configured for use.
 *                - #ADI_CRC_SUCCESS                    Call completed successfully.
 *
 *
 * @sa        adi_CRC_SetBitmirroring().
 * @sa        adi_CRC_SetBytemirroring().
 * @sa        adi_CRC_SetWordSwap().
 */
ADI_CRC_RESULT_TYPE adi_CRC_SetAutoReset(ADI_CRC_DEV_HANDLE const hDevice,ADI_CRC_AUTORST eAutoRst )
{

#ifdef ADI_DEBUG
    if (ADI_CRC_INVALID_HANDLE(hDevice))
        return ADI_CRC_ERR_BAD_DEV_HANDLE;

    if (hDevice->InitState != ADI_CRC_STATE_INITIALIZED)
        return ADI_CRC_ERR_NOT_INITIALIZED ;

    /* This function should be called only when device is disabled */
    if(hDevice->pDevice->CRC_CTL &BITM_CRC_CRC_CTL_BLKEN)
        return ADI_CRC_ERR_OPERATION_NOT_ALLOWED;

#endif
    hDevice->pDevice->CRC_CTL  &= ~(BITM_CRC_CRC_CTL_AUTORST);
    hDevice->pDevice->CRC_CTL |= eAutoRst << (BITP_CRC_CRC_CTL_AUTORST);
    return ADI_CRC_SUCCESS;
}

/*!
 * @brief  Set the bit mirroring. This function should be called only when device is disabled.
 *
 * @param[in]   hDevice     Device handle obtained from adi_CRC_Init().
 * @param[in]   bFlag       Boolean flag to enable/disable bit mirroring.
 * \n
 * \n                       true : To Enable bit mirroring.
 * \n
 * \n                       false :  To Disable bit mirroring.
 * \n
 * @return      Status
 *                - #ADI_CRC_ERR_BAD_DEV_HANDLE [D]     Invalid device handle parameter.
 *                - #ADI_CRC_ERR_NOT_INITIALIZED [D]    Device has not been previously configured for use.
 *                - #ADI_CRC_SUCCESS                    Call completed successfully.
 *
 *
 * @sa        adi_CRC_SetAutoReset().
 * @sa        adi_CRC_SetBytemirroring().
 * @sa        adi_CRC_SetWordSwap().
 */
ADI_CRC_RESULT_TYPE adi_CRC_SetBitMirroring(ADI_CRC_DEV_HANDLE const hDevice,const bool_t bFlag)
{

#ifdef ADI_DEBUG
    if (ADI_CRC_INVALID_HANDLE(hDevice))
        return ADI_CRC_ERR_BAD_DEV_HANDLE;

    if (hDevice->InitState != ADI_CRC_STATE_INITIALIZED)
        return ADI_CRC_ERR_NOT_INITIALIZED ;

    /* This function should be called only when device is disabled */
    if(hDevice->pDevice->CRC_CTL &BITM_CRC_CRC_CTL_BLKEN)
        return ADI_CRC_ERR_OPERATION_NOT_ALLOWED;

#endif
    if(bFlag == true)
    {
        hDevice->pDevice->CRC_CTL |=  (BITM_CRC_CRC_CTL_BITMIRR);
    }
    else
    {
       hDevice->pDevice->CRC_CTL  &=  ~(BITM_CRC_CRC_CTL_BITMIRR);
    }
    return ADI_CRC_SUCCESS;
}

/*!
 * @brief  Set the byte mirroring.This function should be called only when device is disabled.
 *
 * @param[in]   hDevice    Device handle obtained from adi_CRC_Init().
 * @param[in]   bFlag      Boolean flag to enable/disable byte mirroring.
 * \n
 * \n                      true : To Enable byte mirroring.
 * \n
 * \n                      false :  To Disable byte mirroring.
 * \n
 * @return      Status
 *                - #ADI_CRC_ERR_BAD_DEV_HANDLE [D]     Invalid device handle parameter.
 *                - #ADI_CRC_ERR_NOT_INITIALIZED [D]    Device has not been previously configured for use.
 *                - #ADI_CRC_SUCCESS                    Call completed successfully.
 *
 *
 * @sa        adi_CRC_SetAutoReset().
 * @sa        adi_CRC_SetBitmirroring().
 * @sa        adi_CRC_SetWordSwap().
 */
ADI_CRC_RESULT_TYPE adi_CRC_SetByteMirroring(ADI_CRC_DEV_HANDLE const hDevice,const bool_t bFlag)
{

#ifdef ADI_DEBUG
    if (ADI_CRC_INVALID_HANDLE(hDevice))
        return ADI_CRC_ERR_BAD_DEV_HANDLE;

    if (hDevice->InitState != ADI_CRC_STATE_INITIALIZED)
        return ADI_CRC_ERR_NOT_INITIALIZED ;
    /* This function should be called only when device is disabled */
    if(hDevice->pDevice->CRC_CTL &BITM_CRC_CRC_CTL_BLKEN)
        return ADI_CRC_ERR_OPERATION_NOT_ALLOWED;


#endif
    if(bFlag == true)
    {
       hDevice->pDevice->CRC_CTL |=  (BITM_CRC_CRC_CTL_BYTMIRR);
    }
    else
    {
        hDevice->pDevice->CRC_CTL &=  ~(BITM_CRC_CRC_CTL_BYTMIRR);
    }
    return ADI_CRC_SUCCESS;
}
/*!
 * @brief  To enable/disable the word Swap.This function should be called only when device is disabled.
 *
 * @param[in]   hDevice     Device handle obtained from adi_CRC_Init().
 * @param[in]   bFlag      Boolean flag to enable/disable word swap.
 * \n
 * \n                      true : To Enable word swap.
 * \n
 * \n                      false :  To Disable word swap.
 * \n
 * @return      Status
 *                - #ADI_CRC_ERR_BAD_DEV_HANDLE [D]     Invalid device handle parameter.
 *                - #ADI_CRC_ERR_NOT_INITIALIZED [D]    Device has not been previously configured for use.
 *                - #ADI_CRC_SUCCESS                    Call completed successfully.
 *
 *
 * @sa        adi_CRC_SetAutoReset().
 * @sa        adi_CRC_SetBitmirroring().
 * @sa        adi_CRC_SetBytemirroring().
 */
ADI_CRC_RESULT_TYPE  adi_CRC_SetWordSwap(ADI_CRC_DEV_HANDLE const hDevice,const bool_t bFlag)
{

#ifdef ADI_DEBUG
    if (ADI_CRC_INVALID_HANDLE(hDevice))
        return ADI_CRC_ERR_BAD_DEV_HANDLE;

    if (hDevice->InitState != ADI_CRC_STATE_INITIALIZED)
        return ADI_CRC_ERR_NOT_INITIALIZED ;

    if(hDevice->pDevice->CRC_CTL &BITM_CRC_CRC_CTL_BLKEN)
        return ADI_CRC_ERR_OPERATION_NOT_ALLOWED;
#endif
    if(bFlag == true)
    {
       hDevice->pDevice->CRC_CTL |=  (BITM_CRC_CRC_CTL_W16SWP);
    }
    else
    {
       hDevice->pDevice->CRC_CTL &=  ~(BITM_CRC_CRC_CTL_W16SWP);
    }


    return ADI_CRC_SUCCESS;
}
/*!
 * @brief  Set the CRC result register to the specified value.
 *
 * @param[in]   hDevice    Device handle obtained from adi_CRC_Init().
 * @param[in]   nValue     Value to be initialized to CRC register.
 *
 * @return      Status
 *                - #ADI_CRC_ERR_BAD_DEV_HANDLE [D]     Invalid device handle parameter.
 *                - #ADI_CRC_ERR_NOT_INITIALIZED [D]    Device has not been previously configured for use.
 *                - #ADI_CRC_SUCCESS                    Call completed successfully.
 *
 *
 * @sa        adi_CRC_GetResult().
 */
ADI_CRC_RESULT_TYPE adi_CRC_SetResult(ADI_CRC_DEV_HANDLE const hDevice, uint32_t nValue)
{
#ifdef ADI_DEBUG
    if (ADI_CRC_INVALID_HANDLE(hDevice))
        return ADI_CRC_ERR_BAD_DEV_HANDLE;

    if (hDevice->InitState != ADI_CRC_STATE_INITIALIZED)
        return ADI_CRC_ERR_NOT_INITIALIZED ;

#endif
    hDevice->pDevice->CRC_RESULT = nValue;
    return ADI_CRC_SUCCESS;
}
#if (ADI_CFG_ENABLE_CALLBACK_SUPPORT == 0)
/*!
 * @brief  Get the CRC result.
 *
 * @param[in]   hDevice    Device handle obtained from adi_CRC_Init().
 * @param[in]   pResult    Pointer to lcation where the CRC should be written.
 *
 *
 * @return      Status
 *                - #ADI_CRC_ERR_BAD_DEV_HANDLE        [D]    Invalid device handle parameter.
 *                - #ADI_CRC_ERR_NOT_INITIALIZED       [D]    Device has not been previously configured for use.
 *                - #ADI_CRC_ERR_OPERATION_NOT_ALLOWED [D]    Not allowed to call when call back function is registered.
 *                - #ADI_CRC_ERR_DEVICE_BUSY                  Device is busy and computing the CRC for previously submitted buffer.
 *                - #ADI_CRC_SUCCESS                          Call completed successfully.
 *
 *
 * @sa        adi_CRC_SetResult().
 * @note      This is a blocking call. Returns only after the CRC of the submitted buffer is computed.
 */
ADI_CRC_RESULT_TYPE adi_CRC_GetResult(ADI_CRC_DEV_HANDLE const hDevice, uint32_t *pResult)
{
#ifdef ADI_DEBUG
    if (ADI_CRC_INVALID_HANDLE(hDevice))
        return ADI_CRC_ERR_BAD_DEV_HANDLE;

    if (hDevice->InitState != ADI_CRC_STATE_INITIALIZED)
        return ADI_CRC_ERR_NOT_INITIALIZED ;
#endif

     while(hDevice->bCRCBusy == true);
    *pResult  = hDevice->pDevice->CRC_RESULT;
    return ADI_CRC_SUCCESS;
}
/*!
 * @brief  To know whether CRC computation for a specified buffer is complete and ready to "Get" the result.
 *
 * @param[in]   hDevice    Device handle obtained from adi_CRC_Init().
 * @param[in]   bFlag      Pointer to a boolean variable. Contains "True" if result is computed for an already submitted buffer
 *                         Contains "false" if the result is not ready.
 *
 *
 * @return      Status
 *                - #ADI_CRC_ERR_BAD_DEV_HANDLE        [D]    Invalid device handle parameter.
 *                - #ADI_CRC_ERR_NOT_INITIALIZED       [D]    Device has not been previously configured for use.
 *                - #ADI_CRC_SUCCESS                          Call completed successfully.
 *
 *
 * @sa        adi_CRC_SetResult().
 *            adi_CRC_GetResult()
 * @note
 */
ADI_CRC_RESULT_TYPE adi_CRC_IsResultAvailable(ADI_CRC_DEV_HANDLE const hDevice, bool_t *bFlag)
{
#ifdef ADI_DEBUG
    if (ADI_CRC_INVALID_HANDLE(hDevice))
        return ADI_CRC_ERR_BAD_DEV_HANDLE;

    if (hDevice->InitState != ADI_CRC_STATE_INITIALIZED)
        return ADI_CRC_ERR_NOT_INITIALIZED ;
#endif

     if(hDevice->bCRCBusy == true)
    {
        *bFlag = false;
    }
    else
    {
       *bFlag = true;
    }
    return ADI_CRC_SUCCESS;
}
#endif
/*!
 * @brief  Query for the auto reset option used by the CRC device driver..
 *
 * @param[in]   hDevice     Device handle obtained from adi_CRC_Init().
 * @param [out] bFlag       Pointer to a boolean variable. Contains "True" if AutoReset options is enabled.
 *                          Contains "false" if AutoReset options is disabled.
 * @return      Status
 *                - #ADI_CRC_ERR_BAD_DEV_HANDLE        [D]    Invalid device handle parameter.
 *                - #ADI_CRC_ERR_NOT_INITIALIZED       [D]    Device has not been previously configured for use.
 *                - #ADI_CRC_SUCCESS                          Call completed successfully.
 * @sa        adi_CRC_GetBitmirroring().
 * @sa        adi_CRC_GetBytemirroring().
 * @sa        adi_CRC_GetWordSwap().
 */
ADI_CRC_RESULT_TYPE adi_CRC_GetAutoReset(ADI_CRC_DEV_HANDLE const hDevice, bool *const bFlag)
{
#ifdef ADI_DEBUG
    if (ADI_CRC_INVALID_HANDLE(hDevice))
        return ADI_CRC_ERR_BAD_DEV_HANDLE;

    if (hDevice->InitState != ADI_CRC_STATE_INITIALIZED)
        return ADI_CRC_ERR_NOT_INITIALIZED ;
#endif

    if( hDevice->pDevice->CRC_CTL &= (BITM_CRC_CRC_CTL_AUTORST) != 0)
    {
        *bFlag= true;
    }
    else
    {
        *bFlag= false;
    }
    return(ADI_CRC_SUCCESS);
}

/*!
 * @brief  Query for the Bit mirroring option used by the CRC device driver..
 *
 * @param[in]   hDevice     Device handle obtained from adi_CRC_Init().
 * @param [out] bFlag       Pointer to a boolean variable. Contains "True" if bit mirroring  options is enabled.
 *                          Contains "false" if bit mirroring options is disabled.
 * @return      Status
 *                - #ADI_CRC_ERR_BAD_DEV_HANDLE        [D]    Invalid device handle parameter.
 *                - #ADI_CRC_ERR_NOT_INITIALIZED       [D]    Device has not been previously configured for use.
 *                - #ADI_CRC_SUCCESS                          Call completed successfully.
 * \n
 *
 * @sa        adi_CRC_GetAutoReset().
 * @sa        adi_CRC_GetBytemirroring().
 * @sa        adi_CRC_GetWordSwap().
 */
ADI_CRC_RESULT_TYPE adi_CRC_GetBitMirroring(ADI_CRC_DEV_HANDLE const hDevice, bool *const bFlag)
{
#ifdef ADI_DEBUG
    if (ADI_CRC_INVALID_HANDLE(hDevice))
        return ADI_CRC_ERR_BAD_DEV_HANDLE;

    if (hDevice->InitState != ADI_CRC_STATE_INITIALIZED)
        return ADI_CRC_ERR_NOT_INITIALIZED ;
#endif

    if( hDevice->pDevice->CRC_CTL &= (BITM_CRC_CRC_CTL_BITMIRR) != 0)
    {
        *bFlag= true;
    }
    else
    {
        *bFlag= false;
    }
    return(ADI_CRC_SUCCESS);
}

/*!
 * @brief  Query for the Byte mirroring option used by the CRC device driver..
 *
 * @param[in]   hDevice     Device handle obtained from adi_CRC_Init().
 * @param [out] bFlag       Pointer to a boolean variable. Contains "True" if byte mirroring  options is enabled.
 *                          Contains "false" if byte mirroring options is disabled.
 * @return      Status
 *                - #ADI_CRC_ERR_BAD_DEV_HANDLE        [D]    Invalid device handle parameter.
 *                - #ADI_CRC_ERR_NOT_INITIALIZED       [D]    Device has not been previously configured for use.
 *                - #ADI_CRC_SUCCESS                          Call completed successfully.
 *
 * @sa        adi_CRC_GetAutoReset().
 * @sa        adi_CRC_GetBitmirroring().
 * @sa        adi_CRC_GetWordSwap().
 */
ADI_CRC_RESULT_TYPE adi_CRC_GetByteMirroring(ADI_CRC_DEV_HANDLE const hDevice,bool *const bFlag)
{
#ifdef ADI_DEBUG
    if (ADI_CRC_INVALID_HANDLE(hDevice))
        return ADI_CRC_ERR_BAD_DEV_HANDLE;

    if (hDevice->InitState != ADI_CRC_STATE_INITIALIZED)
        return ADI_CRC_ERR_NOT_INITIALIZED ;
#endif

     if( hDevice->pDevice->CRC_CTL &= (BITM_CRC_CRC_CTL_BYTMIRR) != 0)
    {
        *bFlag= true;
    }
    else
    {
        *bFlag= false;
    }
    return(ADI_CRC_SUCCESS);
}
/*!
 * @brief  Query for the word Swap option used by the CRC device driver..
 *
 * @param[in]   hDevice     Device handle obtained from adi_CRC_Init().
 * @param [out] bFlag       Pointer to a boolean variable. Contains "True" if word-swap  options is enabled.
 *                          Contains "false" if word-swap options is disabled.
 * @return      Status
 *                - #ADI_CRC_ERR_BAD_DEV_HANDLE        [D]    Invalid device handle parameter.
 *                - #ADI_CRC_ERR_NOT_INITIALIZED       [D]    Device has not been previously configured for use.
 *                - #ADI_CRC_SUCCESS                          Call completed successfully.
 *
 *
 * @sa        adi_CRC_GetAutoReset().
 * @sa        adi_CRC_GetBitmirroring().
 * @sa        adi_CRC_GetBytemirroring().
 */
ADI_CRC_RESULT_TYPE  adi_CRC_GetWordSwap(ADI_CRC_DEV_HANDLE const hDevice,bool *const bFlag)
{
#ifdef ADI_DEBUG
    if (ADI_CRC_INVALID_HANDLE(hDevice))
        return ADI_CRC_ERR_BAD_DEV_HANDLE;

    if (hDevice->InitState != ADI_CRC_STATE_INITIALIZED)
        return ADI_CRC_ERR_NOT_INITIALIZED ;
#endif
    if( hDevice->pDevice->CRC_CTL &= (BITM_CRC_CRC_CTL_W16SWP) != 0)
    {
        *bFlag= true;
    }
    else
    {
        *bFlag= false;
    }
    return(ADI_CRC_SUCCESS);
}

#if (ADI_CRC_CFG_ENABLE_DMA_SUPPORT ==1)

/*!
 * @brief  Set the DMA mode.
 *
 * @param[in]   hDevice   Device handle obtained from adi_CRC_Init().
 *
 * @param[in]   bFlag      Boolean flag to enable/disable DMA mode of operation.
 * \n
 * \n                      true :  Enable the DMA mode.
 * \n
 * \n                      false : Disable the DMA mode.
 * \n
 * @return      Status
 *                - #ADI_CRC_ERR_BAD_DEV_HANDLE [D]     Invalid device handle parameter.
 *                - #ADI_CRC_ERR_NOT_INITIALIZED [D]    Device has not been previously configured for use.
 *                - #ADI_CRC_SUCCESS                    Call completed successfully.
 *
 *
 * @sa        adi_CRC_Init().
 */
ADI_CRC_RESULT_TYPE adi_CRC_SetDmaMode(ADI_CRC_DEV_HANDLE const hDevice,const bool_t bFlag)
{

    ADI_CRC_RESULT_TYPE result = ADI_CRC_SUCCESS;

#ifdef ADI_DEBUG
    if (ADI_CRC_INVALID_HANDLE(hDevice))
        return ADI_CRC_ERR_BAD_DEV_HANDLE;

    if (hDevice->InitState != ADI_CRC_STATE_INITIALIZED)
        return ADI_CRC_ERR_NOT_INITIALIZED ;
#endif
    if(bFlag == true)
    {
	/* Enable the CRC DMA interrupt */
        ADI_ENABLE_INT(DMA_CRC_IRQn);
        hDevice->pDevice->CRC_CTL |=  (BITM_CRC_CRC_CTL_DMAREQEN);
	/* Initialize DMA service */
        if (ADI_DMA_SUCCESS != adi_DMA_Init(CRC_CHANn, ADI_DMA_PRIORITY_RESET))
            return ADI_CRC_ERR_BAD_DMA_CONFIG;
        /* Install the DMA interrupt handler */
       ADI_INSTALL_HANDLER(DMA_CRC_IRQn, DMA_CRC_Int_Handler);

    }
    else
    {
	/* Disable CRC DMA IRQ */
        ADI_DISABLE_INT(DMA_CRC_IRQn);
        hDevice->pDevice->CRC_CTL &=  ~(BITM_CRC_CRC_CTL_DMAREQEN);
        /* Uninit the DMA service for CRC DMA channel */
        if (ADI_DMA_SUCCESS != adi_DMA_UnInit(CRC_CHANn))
            return ADI_CRC_ERR_BAD_DMA_CONFIG;
        /* Uninstall the DMA interrupt handler for CRC DMA channel */
        ADI_UNINSTALL_HANDLER(DMA_CRC_IRQn);
    }
    return result;
}

/*!
 * @brief  Submitt the buffer for Calculating the CRC
 *
 * @param[in]   hDevice     Device handle obtained from adi_CRC_Init().
 * @param[in]   pBuff       Pointer to a buffer for which CRC to be computed. It is user responsibility
 *                          to align the buffer to word (32 bit) boundary.
 * @param[in]   nNumWords   Size of the buffer in 32 bit words.
 *
 * @return      Status
 *                - #ADI_CRC_ERR_BAD_DEV_HANDLE   [D]    Invalid device handle parameter.
 *                - #ADI_CRC_ERR_NOT_INITIALIZED  [D]    Device has not been previously configured for use.
 *                - #ADI_CRC_ERR_BAD_DATA_POINTER [D]    Specified buffer pointer is not aligned.
 *                - #ADI_CRC_ERR_DEVICE_BUSY      [D]    Can not submit the buffer since device is busy.
 *                - #ADI_CRC_SUCCESS                     Call completed successfully.
 *
 *
 * @sa        adi_CRC_Init().
 */
ADI_CRC_RESULT_TYPE adi_CRC_BufferSubmit(ADI_CRC_DEV_HANDLE const hDevice,uint32_t *pBuff, uint32_t nNumWords)
{
#ifdef ADI_DEBUG
    if (ADI_CRC_INVALID_HANDLE(hDevice))
        return ADI_CRC_ERR_BAD_DEV_HANDLE;

    if (hDevice->InitState != ADI_CRC_STATE_INITIALIZED)
        return ADI_CRC_ERR_NOT_INITIALIZED ;

    if (((uint32_t)pBuff&0X03) != 0)
        return ADI_CRC_ERR_BAD_DATA_POINTER ;

#endif
    /* Device is calculating the CRC for previously submitted buffer */
    if(hDevice->bCRCBusy == true)
    {
      return(ADI_CRC_ERR_DEVICE_BUSY);
    }
    /* store the size of the buffer */
     hDevice->unscheduledCnt = nNumWords;
     /* Can the buffer processed in one iteration */
     if(nNumWords >= ADI_DMA_MAX_TRANSFER_SIZE)
     {
         nNumWords  =  ADI_DMA_MAX_TRANSFER_SIZE;
	 /* Buffer address for next iteration */
         hDevice->pBuffer = pBuff+ ADI_DMA_MAX_TRANSFER_SIZE;
         hDevice->unscheduledCnt-=  ADI_DMA_MAX_TRANSFER_SIZE;
     }
     else
     {
	 /* Only one iteration */
         hDevice->pBuffer = pBuff;
     }
      /*Initialize the address in DMA descriptor*/
     gDmaDescriptor.pSrcData    = pBuff;
     /*Store the number of transfer */
     gDmaDescriptor.DataLength  = nNumWords;
     /*Mark the status of the device as BUSY */
     hDevice->bCRCBusy = true;
     /* submit the buffer to DMA service */
    if (adi_DMA_SubmitTransfer(&gDmaDescriptor) != ADI_DMA_SUCCESS) {
#ifdef ADI_DEBUG
        /* Reset if the submission is failed */
        hDevice->pBuffer = NULL;
        hDevice->unscheduledCnt   = 0X00;

#endif
        hDevice->bCRCBusy = false;
        return ADI_CRC_ERR_BAD_DMA_CONFIG;
    }
    return ADI_CRC_SUCCESS;}
/*!
 * @brief  Enable the CRC device. This function must be called only after submitting the buffer.
 *
 * @param[in]   hDevice    Device handle obtained from adi_CRC_Init().
 *
 * @param[in]   bFlag      Boolean flag to indicate whether enable or disable CRC device.
 * \n
 * \n                      true  : Enable the CRC device
 * \n
 * \n                      false : Disable the CRC device
 * \n
 *
 * @return      Status
 *                - #ADI_CRC_ERR_BAD_DEV_HANDLE [D]         Invalid device handle parameter.
 *                - #ADI_CRC_ERR_NOT_INITIALIZED [D]        Device has not been previously configured for use.
 *                - #ADI_CRC_ERR_OPERATION_NOT_ALLOWED [D]  Device is alreay enabled.
 *                - #ADI_CRC_SUCCESS                        Call completed successfully.
 *
 * @sa        adi_CRC_BufferSubmit().
 * @sa        adi_CRC_GetResult().
 */
ADI_CRC_RESULT_TYPE adi_CRC_Enable(ADI_CRC_DEV_HANDLE const hDevice,const bool_t bFlag)
{

#ifdef ADI_DEBUG
    if (ADI_CRC_INVALID_HANDLE(hDevice))
        return ADI_CRC_ERR_BAD_DEV_HANDLE;

    if (hDevice->InitState != ADI_CRC_STATE_INITIALIZED)
        return ADI_CRC_ERR_NOT_INITIALIZED ;

    if((bFlag == true) && ( hDevice->pDevice->CRC_CTL &BITM_CRC_CRC_CTL_BLKEN))
        return ADI_CRC_ERR_OPERATION_NOT_ALLOWED;

#endif
    if(bFlag == true)
    {
	    /* Enable the device */
            hDevice->pDevice->CRC_CTL |=   BITM_CRC_CRC_CTL_STRT_CRC | BITM_CRC_CRC_CTL_BLKEN;
    }
    else
    {
	    /* Disable the device */
            hDevice->pDevice->CRC_CTL &=  ~(  BITM_CRC_CRC_CTL_STRT_CRC | BITM_CRC_CRC_CTL_BLKEN );
    }
    return ADI_CRC_SUCCESS;
}
#else
/*!
 * @brief  Submitt the buffer for Calculating the CRC
 *
 * @param[in]   hDevice     Device handle obtained from adi_CRC_Init().
 * @param[in]   pBuff       Pointer to a buffer for which CRC to be computed. It is user responsibility
 *                          to align the buffer to word (32 bit) boundary.
 * @param[in]   nNumWords   Size of the buffer in 32 bit words.

 *
 * @return      Status
 *                - #ADI_CRC_ERR_BAD_DEV_HANDLE   [D]   Invalid device handle parameter.
 *                - #ADI_CRC_ERR_NOT_INITIALIZED  [D]   Device has not been previously configured for use.
 *                - #ADI_CRC_ERR_BAD_DATA_POINTER [D]   Specified buffer pointer is not aligned.
 *                - #ADI_CRC_SUCCESS                    Call completed successfully.
 *
 * @sa        adi_CRC_GetResult().
 * @sa        adi_CRC_Enable().
 */

ADI_CRC_RESULT_TYPE adi_CRC_BufferSubmit(ADI_CRC_DEV_HANDLE const hDevice,uint32_t *pBuff, uint32_t nNumWords)
{
#ifdef ADI_DEBUG
    if (ADI_CRC_INVALID_HANDLE(hDevice))
        return ADI_CRC_ERR_BAD_DEV_HANDLE;

    if (hDevice->InitState != ADI_CRC_STATE_INITIALIZED)
        return ADI_CRC_ERR_NOT_INITIALIZED ;

    if (((uint32_t)pBuff&0X03) != 0)
        return ADI_CRC_ERR_BAD_DATA_POINTER ;

    /* Submit only if there is no other buffer is being processed */
    if(hDevice->pBuffer != NULL)
    {
      return(ADI_CRC_ERR_DEVICE_BUSY);
    }

#endif
     hDevice->pBuffer = pBuff;
     hDevice->unscheduledCnt = nNumWords;
     /* Start the computation if the device is already enabled */
    if(hDevice->pDevice->CRC_CTL & BITM_CRC_CRC_CTL_BLKEN )
    {
	/* Core mode operation to calculate the CRC */
        ComputeCRC(hDevice);
    }

    return ADI_CRC_SUCCESS;
}
/*!
 * @brief  Enable the CRC device. This function must be called only after submitting the buffer.
 *
 * @param[in]   hDevice    Device handle obtained from adi_CRC_Init().
 *
 * @param[in]   bFlag      Boolean flag to indicate whether enable or disable CRC device.
 * \n
 * \n                      true  : Enable the CRC device
 * \n
 * \n                      false : Disable the CRC device
 * \n
 * @return      Status
 *                - #ADI_CRC_ERR_BAD_DEV_HANDLE [D]     Invalid device handle parameter.
 *                - #ADI_CRC_ERR_NOT_INITIALIZED [D]    Device has not been previously configured for use.
 *                - #ADI_CRC_SUCCESS                    Call completed successfully.
 *
 * @sa        adi_CRC_GetResult().
 * @sa        adi_CRC_BufferSubmit().
 */
ADI_CRC_RESULT_TYPE adi_CRC_Enable(ADI_CRC_DEV_HANDLE const hDevice,const bool_t bFlag)
{
#ifdef ADI_DEBUG
    if (ADI_CRC_INVALID_HANDLE(hDevice))
        return ADI_CRC_ERR_BAD_DEV_HANDLE;

    if (hDevice->InitState != ADI_CRC_STATE_INITIALIZED)
        return ADI_CRC_ERR_NOT_INITIALIZED ;

     /* Device is alreay enabled */
    if((bFlag == true) && ( hDevice->pDevice->CRC_CTL &BITM_CRC_CRC_CTL_BLKEN))
        return ADI_CRC_ERR_OPERATION_NOT_ALLOWED;
#endif
    if(bFlag == true)
    {
            if( hDevice->pBuffer != NULL)
            {
                hDevice->pDevice->CRC_CTL |=  BITM_CRC_CRC_CTL_STRT_CRC | BITM_CRC_CRC_CTL_BLKEN;
                ComputeCRC(hDevice);
            }
    }
    else
    {
	    /* Disable the device */
            hDevice->pDevice->CRC_CTL &=  ~( BITM_CRC_CRC_CTL_STRT_CRC | BITM_CRC_CRC_CTL_BLKEN);
    }
    return ADI_CRC_SUCCESS;
}
#endif
#if (ADI_CFG_ENABLE_CALLBACK_SUPPORT == 1)

/*!
 * @brief  Register or unregister callback function. This is valid only in DMA mode of operation.
 *         Application can register a callback function with the driver which will be called back to notify
 *         the result of the CRC.
 *
 * @param[in]   hDevice     Device handle obtained from adi_CRC_Init().
 * @param[in]   pfCallback  Pointer to the callback function. It can be set to NULL to unregister a previously
 *                          registered callback.
 * @param[in]   pCBParam    Callback parameter which will be passed back to the application when the callback
 *                          is called.
 * @return      Status
 *                - #ADI_CRC_ERR_BAD_DEV_HANDLE    [D]  Invalid device handle parameter.
 *                - #ADI_CRC_ERR_NOT_INITIALIZED   [D]  The given pointer to status flag is pointing to NULL.
 *                - #ADI_CRC_SUCCESS                    Call completed successfully.
 */

ADI_CRC_RESULT_TYPE adi_CRC_RegisterCallback(ADI_CRC_DEV_HANDLE const hDevice, ADI_CALLBACK pfCallback, void *pCBParam)
{

#ifdef ADI_DEBUG
    if (ADI_CRC_INVALID_HANDLE(hDevice))
        return ADI_CRC_ERR_BAD_DEV_HANDLE;

    if (ADI_CRC_STATE_UNINITIALIZED == hDevice->InitState)
        return ADI_CRC_ERR_NOT_INITIALIZED;

#endif

    ADI_ENTER_CRITICAL_REGION();

    /* Save the given callback and callback parameters */
    hDevice->pfCallback = pfCallback;
    hDevice->pCBParam   = pCBParam;

    ADI_EXIT_CRITICAL_REGION();

    return ADI_CRC_SUCCESS;
}
#endif
#if (ADI_CRC_CFG_ENABLE_DMA_SUPPORT == 1)
/*!
 * @brief  CRC interrupt handler.
 *
 * @return void.
 *
 * Override of default (WEAK) CRC interrupt handler.
 */

ADI_INT_HANDLER(DMA_CRC_Int_Handler)
{
    uint32_t  nSize;
    /* Check for the remaining words */
    if(pCrc_DevData->unscheduledCnt > 0)
    {
         /* Calculate the transfer size for next iteration  */
          nSize =MIN(pCrc_DevData->unscheduledCnt, ADI_DMA_MAX_TRANSFER_SIZE);
	  /* update the descriptor */
          gDmaDescriptor.pSrcData    = pCrc_DevData->pBuffer;
          gDmaDescriptor.DataLength  = nSize;
          gDmaDescriptor.Mode    = ADI_DMA_MODE_BASIC;
  	  /* Submit to DMA  manager*/
          adi_DMA_SubmitTransfer(&gDmaDescriptor);
  	  /* Update the pointer for next iteration.*/
          pCrc_DevData->pBuffer += nSize;
          pCrc_DevData->unscheduledCnt -=  nSize;
    }
    else /* No more iteration. Stop the DMA. Reset the pointers */
    {
        ADI_ENTER_CRITICAL_REGION();
        gDmaDescriptor.pSrcData    = NULL;
        gDmaDescriptor.DataLength  = 0;
        pCrc_DevData->bCRCBusy           = false;
        pCrc_DevData->pBuffer            = NULL;
        ADI_EXIT_CRITICAL_REGION();
#if (ADI_CFG_ENABLE_CALLBACK_SUPPORT == 1)
	/* call the registered callback function */
        if(pCrc_DevData->pfCallback != NULL)
        {
            pCrc_DevData->pfCallback(pCrc_DevData->pCBParam, (uint32_t) ADI_CRC_EVENT_RESULT_READY, (void *)pCrc_DevData->pCRC->CRC_RESULT);
        }
#endif
    }
}
#else
/*!
 * @brief  CRC computation by CORE.
 *
 * @return void.
 *
 *  Compute the CRC for the given buffer by loading its content to CRC_IPDATA register.
 */
static void ComputeCRC(ADI_CRC_DEV_HANDLE const hDevice )
{
     uint32_t i;
    for(i=0;i<hDevice->unscheduledCnt;i++)
    {
        hDevice->pDevice->CRC_IPDATA =hDevice->pBuffer[i];
    }
    /* Reset the reset the buffer pointers and index */
    hDevice->pBuffer = NULL;
    hDevice->unscheduledCnt =0;
    hDevice->bCRCBusy = false;
}
#endif
/*!
 * @brief  Init CRC device.
 *
 * @return void.
 *
 * Initialize the CRC engine.
 */
void InitCRC (ADI_CRC_DEV_HANDLE const hDevice)
{

    hDevice->InitState        = ADI_CRC_STATE_UNINITIALIZED;
    hDevice->pDevice          = pADI_CRC;
    hDevice->pDevice->CRC_CTL      = ADI_CRC_DEFAULT_CTL_WORD;
    hDevice->bCRCBusy         = false;
    hDevice->pBuffer          = NULL;
    hDevice->unscheduledCnt      = 0X00;
#if (ADI_CFG_ENABLE_CALLBACK_SUPPORT == 1)
    hDevice->pfCallback       = NULL;
    hDevice->pCBParam          = NULL;
#endif
#if (ADI_CRC_CFG_ENABLE_DMA_SUPPORT == 1)
    /* Configure the DMA descriptor. */
    gDmaDescriptor.Chan         = CRC_CHANn;
    gDmaDescriptor.DataWidth    = ADI_DMA_WIDTH_WORD;
    gDmaDescriptor.pDstData     = (void *) &hDevice->pDevice->CRC_IPDATA;
    gDmaDescriptor.DstInc       = ADI_DMA_INCR_NONE;
    gDmaDescriptor.SrcInc       = ADI_DMA_INCR_WORD;
    gDmaDescriptor.Protection   = ADI_DMA_HPROT_NONE;
    gDmaDescriptor.ArbitRate    = ADI_DMA_RPOWER_1;
    gDmaDescriptor.Mode    = ADI_DMA_MODE_BASIC;
#endif
}
#define REVERT_ADI_MISRA_SUPPRESSIONS  /*!< Revert ADI MISRA Suppressions */
#include "misra.h"

/*
** EOF
*/

/*@}*/
