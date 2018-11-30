/*********************************************************************************

Copyright (c) 2013-2014 Analog Devices, Inc. All Rights Reserved.

This software is proprietary to Analog Devices, Inc. and its licensors.  By using 
this software you agree to the terms of the associated Analog Devices Software 
License Agreement (See /LICENSES/ADI.LABLAB.LICENSE). 

Modified from the Analog Devices original.
Portions Copyright (c) 2018 Andrew Grosser, Moritz von Buttlar.

*********************************************************************************/

/*****************************************************************************/
/*!
   @file:    flash.c
   @brief:   Flash device driver implementation for ADuCxxx
   @version: $Revision: 30127 $
   @date:    $Date: 2015-03-09 12:05:31 -0400 (Mon, 09 Mar 2015) $
*/
/*****************************************************************************/


/*! \addtogroup Flash_Driver Flash Driver
 *  @{
 */

#include <stddef.h>

#define ASSERT_ADI_MISRA_SUPPRESSIONS  /*!< Apply ADI MISRA Suppressions */
#include "misra.h"

/* Include files */
#include <device.h>
#include "dma.h"

#include "config/adi_fee_config.h"
#include "flash.h"

#define REVERT_ADI_MISRA_SUPPRESSIONS  /*!< Revert ADI MISRA Suppressions */
#include "misra.h"

#include "adi_rtos.h"

#if (0 == ADI_FEE_CFG_INTERRUPT_SUPPORT) && (1 == ADI_FEE_CFG_GPF_DMA_SUPPORT)
#error adi_fee: DMA support requires interrupt support to be enabled
#endif

/*! \cond PRIVATE */

#define WRITE_PROTECT_LOCATION 0x0005FFF4

#define HI(x) ((uint16_t)((x) >> 16u))
#define LO(x) ((uint16_t)(x))
#define COMBINE(hi,lo)((uint32_t)(lo) | ((uint32_t)(hi) << 16))

#define CMDRES_SUCCESS      ((uint16_t)0x0000u)  /* Command result */
#define CMDRES_PROTECTED    ((uint16_t)0x0010u)  /* Command result */
#define CMDRES_VERIFAIL     ((uint16_t)0x0020u)  /* Command result */
#define CMDRES_ABORTED      ((uint16_t)0x0030u)  /* Command result */

#define CHECK_ABORT(flashStatus) (((flashStatus) & BITM_FEE_FEESTA_CMDRES) == CMDRES_ABORTED)

#define FEEKEY_VALUE_USERKEY1 ((uint16_t)0xF456u)
#define FEEKEY_VALUE_USERKEY2 ((uint16_t)0xF123u)

#define FEECMD_CMD_IDLE      ((uint16_t)0u)
#define FEECMD_CMD_ERASEPAGE ((uint16_t)1u)
#define FEECMD_CMD_FSIGN     ((uint16_t)2u)
#define FEECMD_CMD_RSIGN     ((uint16_t)3u)
#define FEECMD_CMD_MASSERASE ((uint16_t)4u)
#define FEECMD_CMD_ABORT     ((uint16_t)5u)
/*!
 *****************************************************************************
 * \enum ADI_FEE_STATE_TYPE
 *
 * FEE driver state.  Used for internal tracking of the FEE device
 * initialization progress during the adi_FEE_Init().  Also used to insure
 * the FEE device has been properly initialized as a prerequisite to using
 * the balance of the FEE API.
 *
 *****************************************************************************/
typedef enum
{
    ADI_FEE_STATE_UNINITIALIZED= 0,        /*!< FEE is not initialized. */
    ADI_FEE_STATE_INIT_ABORT_RETRY,        /*!< FEE is initialized to retry on abort.    */
    ADI_FEE_STATE_INIT_ABORT_FAIL          /*!< FEE is initialized to fail on abort.     */
} ADI_FEE_STATE_TYPE;


ADI_INT_HANDLER(DMA_CRC_Int_Handler);

#if (1 == ADI_FEE_CFG_INTERRUPT_SUPPORT)
#if (1 == ADI_CFG_ENABLE_RTOS_SUPPORT)
typedef void (*PFIntHandler)(uint32_t, void*);
#endif
ADI_INT_HANDLER(Flash0_Int_Handler);
ADI_INT_HANDLER(Flash1_Int_Handler);
ADI_INT_HANDLER(GPFlash_Int_Handler);
#endif

/*! \struct ADI_FEE_DEV_DATA_TYPE
 * FEE Device instance data structure */
typedef struct ADI_FEE_DEV_DATA_TYPE
{
    /* device attributes */
    ADI_FEE_STATE_TYPE  InitState;           /*!< initialization state                */
    ADI_FEE_TypeDef *const    pFlash;        /*!< pointer to flash controller         */
    const uint32_t            BaseAddress;   /*!< Start address of this flash space   */
    const uint32_t            FlashSize;     /*!< overall size of this flash space    */
    const uint16_t            BlockShift;    /*!< 2^BlockShift = size of each block    */
    const uint16_t            PageShift;     /*!< 2^PageShift = size of each page     */

#if (1 == ADI_FEE_CFG_INTERRUPT_SUPPORT)

    const IRQn_Type           InterruptID;   /*!< Interrupt ID for flash write errors */
    volatile uint32_t         intrFlashStatus;

#  if (ADI_CFG_ENABLE_RTOS_SUPPORT == 1)
    /* OSAL */
    const PFIntHandler           pfIntHandler;
    ADI_OSAL_SEM_HANDLE          hSem;                  /*!< Handle to semaphore        */
    uint8_t                      SemMemory[ADI_OSAL_MAX_SEM_SIZE_CHAR];
#  endif /* ADI_CFG_ENABLE_RTOS_SUPPORT  */

    volatile bool_t              interruptFlag;        /*!< interrupt control flag     */

#endif /* (1 == ADI_UART_CFG_INTERRUPT_MODE_SUPPORT) */

} ADI_FEE_DEV_DATA_TYPE;


/*! Per-instance data array */
static ADI_FEE_DEV_DATA_TYPE FEE_DevData[ADI_FEE_MAX_DEVID] =   /*!< Private FEE device driver instance data */
{
    /* set flash attributes by processor and by flash controller(s) */
    {
        ADI_FEE_STATE_UNINITIALIZED, /* InitState                       */
        pADI_FEE0,                   /* pFlash                          */
        0x00000000u,                 /* BaseAddress                     */
        0x00040000u,                 /* FlashSize = 256k bytes overall  */
        14u,                         /* BlockShift = 16k bytes per block*/
        11u,                         /* PageShift = 2k bytes per page   */
#if (1 == ADI_FEE_CFG_INTERRUPT_SUPPORT)
        FLASH0_IRQn,                 /* InterruptID                     */
        0u,
#if (ADI_CFG_ENABLE_RTOS_SUPPORT == 1)
        Flash0_Int_Handler,
        NULL,
        { 0 },
#else
        false
#endif
#endif
    },
    {
        ADI_FEE_STATE_UNINITIALIZED, /* InitState                       */
        pADI_FEE1,                   /* pFlash                          */
        0x00040000u,                 /* BaseAddress                     */
        0x00020000u,                 /* FlashSize = 128k bytes overall  */
        13u,                         /* BlockShift = 8k bytes per block */
        11u,                         /* PageShift = 2k bytes per page   */
#if (1 == ADI_FEE_CFG_INTERRUPT_SUPPORT)
        FLASH1_IRQn,                 /* InterruptID                     */
        0u,
#if (ADI_CFG_ENABLE_RTOS_SUPPORT == 1)
        Flash1_Int_Handler,
        NULL,
        { 0 },
#else
        false
#endif
#endif
    },
    {
        ADI_FEE_STATE_UNINITIALIZED, /* InitState                       */
        pADI_GPF,                    /* pFlash                          */
        0x20080000u,                 /* BaseAddress                     */
        0x00004000u,                 /* FlashSize = 16k bytes overall   */
        14u,                         /* BlockShift = 16k bytes per block*/
        9u,                          /* PageShift = 512 bytes per page  */
#if (1 == ADI_FEE_CFG_INTERRUPT_SUPPORT)
        GP_FLASH_IRQn,                 /* InterruptID                     */
        0u,
#if (ADI_CFG_ENABLE_RTOS_SUPPORT == 1)
        GPFlash_Int_Handler,
        NULL,
        { 0 },
#else
        false
#endif
#endif
    }
};

#if (1 == ADI_FEE_CFG_GPF_DMA_SUPPORT)
/*! Pointer to current DMA buffer */
static void *pDMABuffer = NULL;
#endif /* (1 == ADI_FEE_CFG_GPF_DMA_SUPPORT) */

/*!
 * @brief  Spin waiting for any of the specified status bits to be set
 *
 * @param[in]        pFlash      pointer to the flash controller MMRs.
 * @param[in]        busyFlags   the bit flags to wait for (any of).
 *
 * @return           FEESTA register contents
 *
 * @sa      waitUntilDone()
 */
static inline
uint16_t waitWhileBusy (ADI_FEE_TypeDef *pFlash, uint32_t busyFlags)
{
    uint16_t flashStatus, flashStatus2;

    do
    {
        flashStatus = pFlash->FEESTA;
        flashStatus2 = pFlash->FEESTA;
    } while (0u != ((flashStatus | flashStatus2) & busyFlags));

    return flashStatus;
}

/*!
 * @brief  Spin waiting for any of the specified status bits to be set
 *
 * @param[in]        pFlash      pointer to the flash controller MMRs.
 * @param[in]        doneFlags   the bit flags to wait for (any of).
 *
 * @return           FEESTA register contents
 *
 * @sa      waitWhileBusy()
 */
static inline
uint16_t waitUntilDone(ADI_FEE_TypeDef *pFlash, uint32_t doneFlags)
{
    uint16_t flashStatus;

    do
    {
        flashStatus = pFlash->FEESTA;
    } while (0u == (flashStatus & doneFlags));

    return flashStatus;
}

#if (1 == ADI_FEE_CFG_INTERRUPT_SUPPORT)
/*!
 * @brief
 *
 * @param[in]        hDevice      Handle of flash device
 * @param[out]       doneFlags   .
 *
 * @return           The status register contents that were read by the ISR
 *
 *
 * @sa      flashInterruptCommon()
 */
static inline
uint16_t waitForInterrupt(ADI_FEE_DEV_HANDLE const hDevice, uint32_t doneFlags)
{
#if (ADI_CFG_ENABLE_RTOS_SUPPORT == 1)
    hDevice->interruptFlag = false;
    adi_osal_SemPend(hDevice->hSem, ADI_OSAL_TIMEOUT_FOREVER);
#else
    SystemEnterLowPowerMode(ADI_SYS_MODE_CORE_SLEEP, /* wait in core sleep */
                            &hDevice->interruptFlag,
                            0);       /* don't mask any interrupts while waiting */
#endif /* (ADI_CFG_ENABLE_RTOS_SUPPORT == 1)             */

    /* Copy the status saved by the handler. We know that there won't be another
     * interrupt from this flash controller until we initiate another operation,
     * so there is no danger of intrFlashStatus being overwritten before we get it.
     */
    return hDevice->intrFlashStatus;
}
#endif

/*!
 * @brief  Send a command to the flash controller and wait for completion.
 *
 * @param[in]        hDevice      Handle of flash device
 * @param[in]        cmd          Command code.
 *
 * @return           Status register contents
 *
 * @sa
 */
static uint16_t doCmd(ADI_FEE_DEV_HANDLE const hDevice, uint32_t cmd)
{
    uint16_t flashStatus;

    /* set key register value */
    hDevice->pFlash->FEEKEY = FEEKEY_VALUE_USERKEY1;
    hDevice->pFlash->FEEKEY = FEEKEY_VALUE_USERKEY2;

#if (1 == ADI_FEE_CFG_INTERRUPT_SUPPORT)
    hDevice->pFlash->FEECON0 = BITM_FEE_FEECON0_IENCMD; /* enable cmd interrupt */
    hDevice->pFlash->FEECMD = cmd;    /* issue command */
    flashStatus = waitForInterrupt(hDevice, BITM_FEE_FEESTA_CMDDONE);    /* wait until cmd is done */
    hDevice->pFlash->FEECON0 = 0u;    /* disable cmd interrupt */
#else
    /* issue command */
    hDevice->pFlash->FEECMD = cmd;

    /* wait until cmd is done */
    flashStatus = waitUntilDone(hDevice->pFlash, BITM_FEE_FEESTA_CMDDONE);
#endif

    return flashStatus;
}

/*!
 * @brief  Translate a status register value to an ADI_FEE_RESULT_TYPE enumerator.
 *
 * @param[in]        flashStatus      Status register contents.
 *
 * @return
 *        - #ADI_FEE_SUCCESS
 *        - #ADI_FEE_ERR_WRITE_PROTECTION - address is write protected
 *        - #ADI_FEE_ERR_READ_VERIFY_ERROR - erase or sign operation failed
 *        - #ADI_FEE_ABORTED - operation was aborted
 *        - #ADI_FEE_ERR_UNKNOWN_ERROR - should never occur
 *
 * @sa
 */
static
ADI_FEE_RESULT_TYPE cmdResult(uint16_t flashStatus)
{
    switch (flashStatus & BITM_FEE_FEESTA_CMDRES)
    {
    default:
        return ADI_FEE_ERR_UNKNOWN_ERROR;
    case CMDRES_PROTECTED:
        return ADI_FEE_ERR_WRITE_PROTECTION;
    case CMDRES_VERIFAIL:
        return ADI_FEE_ERR_READ_VERIFY_ERROR;
    case CMDRES_ABORTED:
        return ADI_FEE_ABORTED;
    case CMDRES_SUCCESS:
        return ADI_FEE_SUCCESS;
    }
}

#if defined(ADI_DEBUG)
/*!
 * @brief  Query the write-protect status of a flash address.
 *
 * @param[in]        hDevice      Handle of flash device
 * @param[out]       relAddress   Memory address relative to the start of the controller's flash region.
 *
 * @return           true if address is write-protected, false otherwise
 *
 *
 * @sa
 */
static bool_t isWriteProtected(ADI_FEE_DEV_HANDLE const hDevice, uint32_t relAddress)
{
    uint16_t WriteProtectRegister;  /* value of write protection register */
    uint32_t BlockNum;
    uint16_t BlockMask;

    /* check to see if writing to a protected block */
    if (pADI_FEE0 == hDevice->pFlash)        /* Flash controller 0 */
    {
        /* Flash controller 0 uses the lower half of the write protection word */
        WriteProtectRegister = pADI_FEE1->FEEPROL;
    }
    else if (pADI_FEE1 == hDevice->pFlash)   /* Flash controller 1 */
    {
        /* Flash controller 1 uses the upper half of the write protection word */
        WriteProtectRegister = pADI_FEE1->FEEPROH;
    }
    else
    {
        /* For the GP flash controller there is no write protection, so writes
         * are allowed on any block.
         */
        WriteProtectRegister = 0xFFFFu;
    }

    /* translate address to a block number */
    BlockNum = relAddress >> hDevice->BlockShift;
    BlockMask = 1u << BlockNum;

    /* compare write protection register with block erase */
    return (0 != (BlockMask & ~WriteProtectRegister));
}
#endif /* defined(ADI_DEBUG) */

#if (1 == ADI_FEE_CFG_INTERRUPT_SUPPORT)

/*!
 * @brief  Generic handler function for flash controller interrupts.
 *
 * @param[in]        pDevice      .
 *
 * @return           none
 *
 *
 * @sa      waitForInterrupt()
 */
static void flashInterruptCommon(ADI_FEE_DEV_HANDLE hDevice)
{
    /* Copy the controller status to the device struct */
    hDevice->intrFlashStatus = hDevice->pFlash->FEESTA;

    /* Unblock anything that is waiting for this interrupt */
#if (ADI_CFG_ENABLE_RTOS_SUPPORT == 1)
    adi_osal_SemPost(hDevice->hSem);
    hDevice->interruptFlag = true;
#else
    SystemExitLowPowerMode(&hDevice->interruptFlag);
#endif /* (ADI_CFG_ENABLE_RTOS_SUPPORT == 1) */
}

/*!
 * @brief  Interrupt handler for flash controller 0.
 *
 * @return            none
 *
 * @sa      flashInterruptCommon()
 */
ADI_INT_HANDLER(Flash0_Int_Handler)
{
    flashInterruptCommon(&FEE_DevData[ADI_FEE_DEVID_0]);
}

/*!
 * @brief  Interrupt handler for flash controller 1.
 *
 * @return            none
 *
 * @sa      flashInterruptCommon()
 */
ADI_INT_HANDLER(Flash1_Int_Handler)
{
    flashInterruptCommon(&FEE_DevData[ADI_FEE_DEVID_1]);
}

/*!
 * @brief  Interrupt handler for the GP flash controller.
 *
 * @return            none
 *
 * @sa      flashInterruptCommon()
 */
ADI_INT_HANDLER(GPFlash_Int_Handler)
{
    flashInterruptCommon(&FEE_DevData[ADI_FEE_DEVID_GP]);
}
#endif /* (1 == ADI_FEE_CFG_INTERRUPT_SUPPORT) */

#if (1 == ADI_FEE_CFG_GPF_DMA_SUPPORT)
/*!
 * @brief
 *
 * @param[in]        ?      .
 * @param[out]       ?   .
 *
 * @return            ?
 *
 *
 * @sa      waitForInterrupt()
 */
ADI_INT_HANDLER(DMA_CRC_Int_Handler)
{
#if (ADI_CFG_ENABLE_RTOS_SUPPORT == 1)
    adi_osal_SemPost(FEE_DevData[ADI_FEE_DEVID_GP].hSem);
    FEE_DevData[ADI_FEE_DEVID_GP].interruptFlag = true;
#else
    SystemExitLowPowerMode(&FEE_DevData[ADI_FEE_DEVID_GP].interruptFlag);
#endif /* (ADI_CFG_ENABLE_RTOS_SUPPORT == 1) */
}

#endif /* (1 == ADI_FEE_CFG_GPF_DMA_SUPPORT) */

/*! \endcond */

/*!
 * @brief  Initialize and allocate an FEE device.
 *
 * @param[in]        devID      Zero-based device index designating which Flash device to initialize.
 * @param[in]        bool_t     true if driver should retry on flash aborts, false to return errors
 * @param[out]       phDevice   The caller's device handle pointer for storing the initialized device instance data pointer.
 *
 * @return            Status
 *                    - #ADI_FEE_ERR_ALREADY_INITIALIZED [D]    Specified device has already been initialized.
 *                    - #ADI_FEE_ERR_BAD_DEV_ID [D]             Invalid device index.
 *                    - #ADI_FEE_ERR_SEMAPHORE_FAILED           Failed to create semaphore.
 *                    - #ADI_FEE_ERR_DMA_FAILURE                Failed to initialize DMA.
 *                    - #ADI_FEE_SUCCESS                        Call completed successfully.
 *
 * No other FEE APIs may be called until the device init function is called.
 *
 * Initialize an FEE device using internal default configuration settings and allocate the device for use.  The caller's
 * device handle is written with the device instance data pointer.  The returned device handle is required to be passed
 * to all subsequent calls to convey which device instance to operate on.
 *
 * @note    The contents of phDevice will be set to NULL upon failure.
 *
 * @sa      adi_FEE_UnInit().
 */
ADI_FEE_RESULT_TYPE adi_FEE_Init(
    ADI_FEE_DEV_ID_TYPE const devID,
              bool_t bRetryAborts,
              ADI_FEE_DEV_HANDLE* const phDevice)
{
    ADI_FEE_DEV_HANDLE hDevice = &FEE_DevData[devID];
   /* store a bad handle in case of failure */
    *phDevice = (ADI_FEE_DEV_HANDLE) NULL;

#if defined(ADI_DEBUG)
    if (devID >= ADI_FEE_MAX_DEVID)
        return ADI_FEE_ERR_BAD_DEV_ID;

    if (ADI_FEE_STATE_UNINITIALIZED != hDevice->InitState)
        return ADI_FEE_ERR_ALREADY_INITIALIZED;
#endif /* defined(ADI_DEBUG) */

#if (1 == ADI_FEE_CFG_GPF_DMA_SUPPORT)
    /* Conditional DMA init for GP flash only */
    if (pADI_GPF == hDevice->pFlash)
    {
        if (ADI_DMA_SUCCESS != adi_DMA_Init(CRC_CHANn, ADI_DMA_PRIORITY_RESET))
            return ADI_FEE_ERR_DMA_FAILURE;  /* Failed to init DMA channel */

        ADI_INSTALL_HANDLER(DMA_CRC_IRQn, DMA_CRC_Int_Handler);

        /* GPFlash takes control of DMA Channel 13 */
        if (ADI_DMA_SUCCESS != adi_DMA_SetChannelAssign(CRC_CHANn, ADI_DMA_ASSIGN_FLASH))
            return ADI_FEE_ERR_DMA_FAILURE;  /* Failed to assign DMA channel */
    }
#endif /* (1 == ADI_FEE_CFG_GPF_DMA_SUPPORT) */

#if (1 == ADI_FEE_CFG_INTERRUPT_SUPPORT)
#if (ADI_CFG_ENABLE_RTOS_SUPPORT == 1)
        /* Create the semaphore for blocking mode support. */
        if(adi_osal_SemCreateStatic(&hDevice->SemMemory, ADI_OSAL_MAX_SEM_SIZE_CHAR, &hDevice->hSem, 0u) != ADI_OSAL_SUCCESS)
        {
            return ADI_FEE_ERR_SEMAPHORE_FAILED;
        }
#endif /* ADI_CFG_ENABLE_RTOS_SUPPORT */
    
        hDevice->interruptFlag = false;

    ADI_INSTALL_HANDLER(hDevice->InterruptID, hDevice->pfIntHandler);
    ADI_ENABLE_INT(hDevice->InterruptID); /* enable Flash controller interrupts */
#endif /* (1 == ADI_FEE_CFG_INTERRUPT_SUPPORT) */

      /* assert power-up default state for this flash device */
    hDevice->pFlash->FEECON0  = 0;
    hDevice->pFlash->FEECMD   = 0;
    hDevice->pFlash->FEEADR0L = 0;
    hDevice->pFlash->FEEADR1L = 0;
    hDevice->pFlash->FEEKEY   = 0;

    /* conditional inits not for GP flash */
    if (pADI_GPF != hDevice->pFlash)
    {
        hDevice->pFlash->FEEADR0H  = 0;
        hDevice->pFlash->FEEADR1H  = 0;
        hDevice->pFlash->FEEPARCTL = 0;
        hDevice->pFlash->FEEPARSTA = 0xffff;  /* write-1-to-clear */
    }

    /* conditional inits for only Flash Block1 */
    if (pADI_FEE1 == hDevice->pFlash)
    {
        hDevice->pFlash->FEECON1 = 0;
    }

        /* interrupt abort enable registers */
    hDevice->pFlash->FEEAEN0  = 0;
    hDevice->pFlash->FEEAEN1  = 0;
    hDevice->pFlash->FEEAEN2  = 0;
    hDevice->pFlash->FEEAEN3  = 0;

    /* mark device fully initialized */
    hDevice->InitState = bRetryAborts
      ? ADI_FEE_STATE_INIT_ABORT_RETRY
      : ADI_FEE_STATE_INIT_ABORT_FAIL;

    /* write the device data pointer into the caller's handle */
    *phDevice = hDevice;

    return ADI_FEE_SUCCESS;
}

/*!
 * @brief  De-initialize and unallocate an FEE device.
 *
 * @param[in]   hDevice      Handle of flash device
 *
 * @return            Status
 *                    - #ADI_FEE_SUCCESS                        Call completed successfully.
 *
 * No other FEE APIs may be called after the device uninit function is called.
 *
 * @sa      adi_FEE_Init().
 */
ADI_FEE_RESULT_TYPE adi_FEE_UnInit(
    ADI_FEE_DEV_HANDLE const hDevice
)
{
#if (1 == ADI_FEE_CFG_GPF_DMA_SUPPORT)
    /* Conditional DMA un-init for GP flash only */
    if (pADI_GPF == hDevice->pFlash)
    {
        /* restore control of DMA Channel 13 to the CRC engine */
        if (ADI_DMA_SUCCESS != adi_DMA_SetChannelAssign(CRC_CHANn, ADI_DMA_ASSIGN_CRC))
            return ADI_FEE_ERR_DMA_FAILURE;  /* Failed to restore DMA channel */

        ADI_UNINSTALL_HANDLER(DMA_CRC_IRQn);

        if (ADI_DMA_SUCCESS != adi_DMA_UnInit(CRC_CHANn))
            return ADI_FEE_ERR_DMA_FAILURE;  /* Failed to un-init DMA channel */
    }
#endif /* (1 == ADI_FEE_CFG_GPF_DMA_SUPPORT) */

#if (1 == ADI_FEE_CFG_INTERRUPT_SUPPORT)
    ADI_DISABLE_INT(hDevice->InterruptID); /* enable Flash controller interrupts */
    ADI_UNINSTALL_HANDLER(hDevice->InterruptID);

#if (ADI_CFG_ENABLE_RTOS_SUPPORT == 1)
    adi_osal_SemDestroyStatic(hDevice->hSem);
#endif /* (ADI_CFG_ENABLE_RTOS_SUPPORT == 1) */

#endif /* (1 == ADI_FEE_CFG_INTERRUPT_SUPPORT) */

    hDevice->InitState = ADI_FEE_STATE_UNINITIALIZED;

    return ADI_FEE_SUCCESS;
}


/*!
 *****************************************************************************
 * @brief                    Flash Page Erase
 *
 * @param[in]   hDevice      Handle of flash device
 * @param[in]   PageNum    Value that represents a 2048-byte page
 *
 * @return  Status
 *        - #ADI_FEE_SUCCESS if successfully initialized
 *        - #ADI_FEE_ERR_WRITE_PROTECTION - address is write protected
 *        - #ADI_FEE_ERR_READ_VERIFY_ERROR - erase or sign operation failed
 *        - #ADI_FEE_ABORTED - operation was aborted
 *        - #ADI_FEE_ERR_NOT_INITIALIZED [D] if device is not initialized
 *        - #ADI_FEE_ERR_INVALID_ADDRESS [D] if address out of range
 *        - #ADI_FEE_ERR_WRITE_PROTECTION [D] if page is write protected
 *
 * @details
 *                           Erases a page of flash.
 *
 * @note                     Page range is 0 to 255
 *
 * @sa           adi_FEE_MassErase().
 *****************************************************************************
 */
ADI_FEE_RESULT_TYPE adi_FEE_PageErase(
    ADI_FEE_DEV_HANDLE const hDevice,
    const uint32_t PageNum
)
{
    uint32_t relAddress;            /* address of page to erase */
    uint16_t flashStatus;

    /* translate erase page number to a relative address */
    relAddress = PageNum << hDevice->PageShift;

#if defined(ADI_DEBUG)
    if (ADI_FEE_STATE_UNINITIALIZED == hDevice->InitState)
    {
        return ADI_FEE_ERR_NOT_INITIALIZED;
    }

    /* check to see if it is a valid flash block */
    /* cannot block erase information space or last page of user space       */
    if (relAddress >= hDevice->FlashSize)
    {
        return ADI_FEE_ERR_INVALID_ADDRESS;
    }

    /* compare write protection register with block erase */
    if (isWriteProtected(hDevice, relAddress))
    {
        return ADI_FEE_ERR_WRITE_PROTECTION;
    }
#endif /* defined(ADI_DEBUG) */

    /* Wait until not busy */
    waitWhileBusy(hDevice->pFlash, (BITM_FEE_FEESTA_CMDBUSY | BITM_FEE_FEESTA_WRBUSY));

    /* Erase the flash page, retrying in the event of an abort */
    do
    {
        /* Set page address to erase. This can be done before
         * calling doCmd(), as the page address registers are not
         * key-protected.
         */
        hDevice->pFlash->FEEADR0L = LO(relAddress);
        hDevice->pFlash->FEEADR0H = HI(relAddress);

        /* Issue the page erase command, and wait for it to complete */
        flashStatus = doCmd(hDevice, FEECMD_CMD_ERASEPAGE);
    } while ((ADI_FEE_STATE_INIT_ABORT_RETRY == hDevice->InitState) && CHECK_ABORT(flashStatus));

    return cmdResult(flashStatus);
}


/*!
 *****************************************************************************
 * @brief    Mass Erase
 * @details This function will erase the entire contents of the Flash.
 *          Applications that use this API should be configured to execute from SRAM,
 *          otherwise this routine will effectively erase itself.
 *
 * @param[in]   hDevice      Handle of flash device
 *
 * @return      Status
 *        - #ADI_FEE_SUCCESS
 *        - #ADI_FEE_ERR_READ_VERIFY_ERROR - erase or sign operation failed
 *        - #ADI_FEE_ABORTED - operation was aborted
 *        - #ADI_FEE_ERR_NOT_INITIALIZED [D] if device is not initialized
 *
 * @details
 *                           Erases the entire flash.
 *
 * @note
 *
 * @sa           adi_FEE_PageErase().
 *****************************************************************************
 */
ADI_FEE_RESULT_TYPE adi_FEE_MassErase(
    ADI_FEE_DEV_HANDLE const hDevice
)
{
    uint16_t flashStatus;

#if defined(ADI_DEBUG)
    if (ADI_FEE_STATE_UNINITIALIZED == hDevice->InitState)
    {
        return ADI_FEE_ERR_NOT_INITIALIZED;
    }
#endif /* defined(ADI_DEBUG) */

    /* Wait until not busy */
    waitWhileBusy(hDevice->pFlash, (BITM_FEE_FEESTA_CMDBUSY | BITM_FEE_FEESTA_WRBUSY));

    /* Mass erase the flash memory, retrying in the event of an abort */
    do
    {
        /* Issue the mass erase command, and wait for it to complete */
        flashStatus = doCmd(hDevice, FEECMD_CMD_MASSERASE);
    } while ((ADI_FEE_STATE_INIT_ABORT_RETRY == hDevice->InitState) && CHECK_ABORT(flashStatus));

    return cmdResult(flashStatus);
}


/*!
 *****************************************************************************
 * @brief    Signature generation and checking
 * @details
 *
 * @param[in]   hDevice      Handle of flash device
 * @param[in]   direction    ADI_FEE_SIGN_FORWARD or ADI_FEE_SIGN_REVERSE
 * @param[in]   nLowerPage   Page number of lower page in checking range
 * @param[in]   nUpperPage   Page number of upper page in checking range
 * @param[out]  pSigResult   signature
 *
 * @return      Status
 *        - #ADI_FEE_SUCCESS
 *        - #ADI_FEE_ERR_READ_VERIFY_ERROR - signature did not match
 *        - #ADI_FEE_ABORTED - operation was aborted
 *        - #ADI_FEE_ERR_NOT_INITIALIZED [D] if device is not initialized
 *        - #ADI_FEE_ERR_INVALID_ADDRESS [D] nLowerPage > nUpperPage
 *
 * @details
 *                           Erases the entire flash.
 *
 * @note
 *
 * @sa
 *****************************************************************************
*/
ADI_FEE_RESULT_TYPE adi_FEE_VerifySignature(
    ADI_FEE_DEV_HANDLE const hDevice,
                                            ADI_FEE_SIGN_DIR direction,
    const uint32_t nLowerPage,
    const uint32_t nUpperPage,
                                            uint32_t *pSigResult)
{
    ADI_FEE_RESULT_TYPE result;
    uint16_t flashStatus;
    uint32_t lRelAddress, uRelAddress;
    uint32_t cmd = FEECMD_CMD_FSIGN;
    uint16_t sigLo, sigHi;

#if defined(ADI_DEBUG)
    if (ADI_FEE_STATE_UNINITIALIZED == hDevice->InitState)
    {
        return ADI_FEE_ERR_NOT_INITIALIZED;
    }

    if (nLowerPage > nUpperPage)
    {
        return ADI_FEE_ERR_INVALID_ADDRESS;
    }
#endif /* defined(ADI_DEBUG) */

    if (ADI_FEE_SIGN_REVERSE == direction)
    {
        cmd = FEECMD_CMD_RSIGN;
    }

    /* Translate page numbers to relative addresses */
    lRelAddress = nLowerPage << hDevice->PageShift;
    uRelAddress = nUpperPage << hDevice->PageShift;

    /* OPTIONAL: allow the pages to be passed in either order */
    if (lRelAddress > uRelAddress)
    {
        /* Swap */
        uint32_t temp = lRelAddress;
        lRelAddress = uRelAddress;
        uRelAddress = temp;
    }

    /* Wait until not busy */
    waitWhileBusy(hDevice->pFlash, (BITM_FEE_FEESTA_CMDBUSY | BITM_FEE_FEESTA_WRBUSY));

    /* Generate & check the signature, retrying in the event of an abort */
    do
    {
        /* Set high and low page addresses. This can be done before
         * calling doCmd(), as the page address registers are not
         * key-protected.
         */
        hDevice->pFlash->FEEADR0L = LO(lRelAddress);
        hDevice->pFlash->FEEADR0H = HI(lRelAddress);
        hDevice->pFlash->FEEADR1L = LO(uRelAddress);
        hDevice->pFlash->FEEADR1H = HI(uRelAddress);

        /* Issue the sign command, and wait for it to complete */
        flashStatus = doCmd(hDevice, cmd);
    } while ((ADI_FEE_STATE_INIT_ABORT_RETRY == hDevice->InitState) && CHECK_ABORT(flashStatus));

    result = cmdResult(flashStatus);

    /* Get the signature from the register halves */
    sigHi = hDevice->pFlash->FEESIGH;
    sigLo = hDevice->pFlash->FEESIGL;
    *pSigResult = COMBINE(sigHi, sigLo);

    return result;
}


/*!
 *****************************************************************************
* @brief    Force abort of an ongoing flash operation
* @details
*
* @param[in]   hDevice      Handle of flash device
*
* @return      Status
*        - #ADI_FEE_SUCCESS
*
* @details
*
*
* @note
*
* @sa           adi_FEE_SetInterruptAbort().
 *****************************************************************************
*/
ADI_FEE_RESULT_TYPE adi_FEE_Abort(
    ADI_FEE_DEV_HANDLE const hDevice
)
{
    /* Don't use doCmd() as we don't want to wait for completion */

    /* Set key register value */
    hDevice->pFlash->FEEKEY = FEEKEY_VALUE_USERKEY1;
    hDevice->pFlash->FEEKEY = FEEKEY_VALUE_USERKEY2;

    /* Issue abort command */
    hDevice->pFlash->FEECMD = FEECMD_CMD_ABORT;

    return ADI_FEE_SUCCESS;
}

/*!
 *****************************************************************************
 * @brief                    Flash Write
 *
 * @param[in]   hDevice      Handle of flash device
 * @param[in]   nAddress     Address to write to
 * @param[in]   pData        Pointer to data to be written
 * @param[in]   nBytes       Number of bytes to write
 *
 * @return      Status
 *        - #ADI_FEE_SUCCESS if successfully initialized
 *        - #ADI_FEE_ERR_WRITE_PROTECTION - address is write protected
 *        - #ADI_FEE_ABORTED - operation was aborted
 *        - #ADI_FEE_ERR_NOT_INITIALIZED [D] if device is not initialized
 *        - #ADI_FEE_ERR_INVALID_ADDRESS [D] if address out of range
 *        - #ADI_FEE_ERR_WRITE_PROTECTION [D] if page is write protected
 *
 * @details
 *                           Writes nBytes of data to nAddress of flash
 *                           This function checks for address ranges errors
 *                           This function checks if a write is attempted on a protected segment
 *                           This function caters for writes to addresses not on 32 bit boundaries
 *                           This function achieves byte alignment by adding padding bytes
 *                           to non 32 bit aligned data
 *
 * @note
 *
 * @sa
 *****************************************************************************
 */
ADI_FEE_RESULT_TYPE adi_FEE_Write(
    ADI_FEE_DEV_HANDLE const hDevice,
                                                       const uint32_t nAddress,
                                                       const uint8_t *pData,
    const uint32_t nBytes
)
{

    /* set to success */
    uint32_t    *pFlashWord;            /* pointer to flash                   */
    uint32_t     offset;                 /* index for pAlignByte                 */
    uint32_t     bytesCopied;
    uint16_t     flashStatus;

#if defined(ADI_DEBUG)
    uint32_t relAddress;

    if (ADI_FEE_STATE_UNINITIALIZED == hDevice->InitState)
    {
        return ADI_FEE_ERR_NOT_INITIALIZED;
    }

    /* check to see is address is valid */
    relAddress = nAddress - hDevice->BaseAddress;

    if (relAddress >= hDevice->FlashSize)
    {
        return ADI_FEE_ERR_INVALID_ADDRESS;
    }

    /* compare write protection register with block erase */
    if (isWriteProtected(hDevice, relAddress))
    {
        return ADI_FEE_ERR_WRITE_PROTECTION;
    }
#endif /* defined(ADI_DEBUG) */

    /* Destination address, rounded down to 4-byte word boundary */
    pFlashWord = (uint32_t *)(nAddress & 0xFFFFFFFCu);
    /* Starting byte offset within first word (0 - 3) */
    offset = nAddress & 0x3u;
    bytesCopied = 0u;

    waitWhileBusy(hDevice->pFlash, BITM_FEE_FEESTA_WRBUSY);
    hDevice->pFlash->FEECON0 = BITM_FEE_FEECON0_WREN | BITM_FEE_FEECON0_IENCMD; /* enable flash write and interrupt */

    while (bytesCopied < nBytes)
    {
        uint32_t nAlignedWord = 0xFFFFFFFFu;       /* aligned word for write */
        uint8_t *pByte = (uint8_t *)&nAlignedWord; /* pointer to byte buffer */

        /* Prepare the next aligned word to write to flash */
        while((offset < 4u) && (bytesCopied < nBytes))
        {
            pByte[offset++] = *pData++;
            bytesCopied++;
        }

        offset = 0; /* start at the beginning of the next word */

        /* Write the word to the flash memory, optionally retrying in the event of an abort */
        do
        {
            waitWhileBusy(hDevice->pFlash, BITM_FEE_FEESTA_WRBUSY);

            /* Write the word to flash */
            *pFlashWord = nAlignedWord;
#if (1 == ADI_FEE_CFG_INTERRUPT_SUPPORT)
            flashStatus = waitForInterrupt(hDevice, BITM_FEE_FEESTA_WRDONE);
#else
            flashStatus = waitUntilDone(hDevice->pFlash, BITM_FEE_FEESTA_WRDONE);
#endif
        } while  ((ADI_FEE_STATE_INIT_ABORT_RETRY == hDevice->InitState) && CHECK_ABORT(flashStatus));

        /* If the word has been successfully written to flash then increment the destination address */
        ++pFlashWord;
    }

    /* make sure last write is complete before returning */
    flashStatus = waitWhileBusy(hDevice->pFlash, BITM_FEE_FEESTA_WRBUSY);
    hDevice->pFlash->FEECON0 = 0u;    /* disable flash write */

    /* return */
    return cmdResult(flashStatus);
}


#if (1 == ADI_FEE_CFG_GPF_DMA_SUPPORT)
/*!
 *****************************************************************************
 * @brief Initiate a write to flash using DMA
 *
 * @param[in]   hDevice      Handle of flash device
 * @param[in]   nAddress     Address to write to
 * @param[in]   pData        Pointer to data to be written
 * @param[in]   nBytes       Number of bytes to write
 *
 * @return      Status
 *        - #ADI_FEE_SUCCESS if successfully initialized
 *        - #ADI_FEE_ERR_UNSUPPORTED device doesn't support DMA
 *        - #ADI_FEE_ERR_INVALID_ADDRESS [D] if address is misaligned
 *        - #ADI_FEE_ERR_INVALID_PARAMETER [D] if buffer size is not a multiple of 4
 *        - #ADI_FEE_ERR_DMA_FAILURE [D] if the DMA operation fails
 *        - #ADI_FEE_ERR_WRITE_PROTECTION [D] if page is write protected
 *
 * @details
 *
 * Starts a DMA write to the flash memory. Only useable with the GP
 * flash controller (ADI_FEE_DEVID_GP) on ADuCM350, as only this controller
 * supports DMA.
 *
 * DMA operates on 32-bit words, so both the source buffer address and the
 * destination flash address must be 4-byte aligned, and the buffer size must
 * be a multiple of 4.
 *
 * @note
 *
 * @sa  , adi_FEE_IsTxBufferAvailable(), adi_FEE_GetTxBuffer()
 *****************************************************************************
 */
ADI_FEE_RESULT_TYPE adi_FEE_SubmitTxBuffer(
    ADI_FEE_DEV_HANDLE const hDevice,
                                           const uint32_t nAddress,
                                           const uint8_t *pData,
                                           const uint32_t nBytes
)
{
    ADI_DMA_TRANSFER_TYPE gDmaDescriptor;

    if (pADI_GPF != hDevice->pFlash)
    {
        return ADI_FEE_ERR_UNSUPPORTED;
    }

#if defined(ADI_DEBUG)
    /* Note that the following tests and error returns are *not* debug-only -
     * they are part of the functionality of this function, and may be
     * required even in release builds. Any of these failures indicates to
     * the caller that DMA is not possible with the specified buffer, and
     * that the caller should use adi_FEE_Write() instead.
     */

    /* Check that destination address is 4-byte aligned */
    if (0u != (nAddress & 3u))
    {
        return ADI_FEE_ERR_INVALID_ADDRESS;
    }

    /* Check that source address is 4-byte aligned */
    if (0u != ((uint32_t)pData & 3u))
    {
        return ADI_FEE_ERR_INVALID_ADDRESS;
    }

    /* Check that transfer is 4-byte multiple */
    if (0u != (nBytes & 3u))
    {
        return ADI_FEE_ERR_INVALID_PARAMETER;
    }
#endif /* defined(ADI_DEBUG) */

    gDmaDescriptor.Chan         = CRC_CHANn; /* GP flash shares the same channel as CRC */
    gDmaDescriptor.CCD          = ADI_DMA_CCD_PRIMARY;
    gDmaDescriptor.DataWidth    = ADI_DMA_WIDTH_WORD;
    gDmaDescriptor.DataLength   = nBytes / 4u;
    gDmaDescriptor.pSrcData     = (void *)pData;
    gDmaDescriptor.pDstData     = (void *)nAddress;
    gDmaDescriptor.SrcInc       = ADI_DMA_INCR_WORD;
    gDmaDescriptor.DstInc       = ADI_DMA_INCR_WORD;
    gDmaDescriptor.Protection   = ADI_DMA_HPROT_NONE;
    gDmaDescriptor.ArbitRate    = ADI_DMA_RPOWER_1;
    gDmaDescriptor.Mode         = ADI_DMA_MODE_BASIC;

    pDMABuffer = (void *)pData;

    if (ADI_DMA_SUCCESS != adi_DMA_SubmitTransfer(&gDmaDescriptor))
    {
        /* DMA transfer initialization error */
        return ADI_FEE_ERR_DMA_FAILURE;
    }

    /* Settings for GPFEECON0 */
    hDevice->pFlash->FEECON0 =      /* 0x000E = Total                     */
        BITM_GPF_GPFEECON0_DMA_EN | /* 0x0008 = DMA_EN (DMA Enable)       */
        BITM_GPF_GPFEECON0_WREN   | /* 0x0004 = WREN (Write Enable)       */
        BITM_GPF_GPFEECON0_IENERR ; /* 0x0002 = IENERR (Error int enable) */

    /* Enable the DMA interrupt. This must be done here, rather than within
     * adi_FEE_GetTxBuffer(), so that the interrupt flag (or semaphore) can
     * be tested for completion in adi_FEE_IsTxBufferAvailable().
     *
     * For safety, we also disable the Flash interrupt for this flash
     * controller (i.e. the GPF) while the DMA interrupt is in use, as
     * they both use the same interrupt flag (or semaphore).
     */
    ADI_DISABLE_INT(hDevice->InterruptID);
    ADI_ENABLE_INT(DMA_CRC_IRQn);

    return ADI_FEE_SUCCESS;
}

/*!
 * @brief Checks if the Transmit Buffer is available for processing.
 *
 * @param[in]   hDevice      Handle of flash device
 * @param[out]  pbAvailable  Set to true if buffer available, false otherwise
 *
 * @return      Status
 *        - #ADI_FEE_SUCCESS if successful
 *        - #ADI_FEE_ERR_UNSUPPORTED device doesn't support DMA
 *
 * @details
 *
 * Returns true only if the previously-submitted buffer has been emptied,
 * i.e. if a subsequent call to adi_FEE_GetTxBuffer() will not block. Only
 * useable with the GPflash controller (ADI_FEE_DEVID_GP) on ADuCM350, as only
 * this controller supports DMA.
 *
 * @sa  adi_FEE_SubmitTxBuffer(), adi_FEE_GetTxBuffer()
 *
 *****************************************************************************
 */
ADI_FEE_RESULT_TYPE adi_FEE_IsTxBufferAvailable(
	    ADI_FEE_DEV_HANDLE         const hDevice,
	    bool_t                    *pbAvailable
)
{
    if (pADI_GPF != hDevice->pFlash)
    {
        return ADI_FEE_ERR_UNSUPPORTED;
    }

    *pbAvailable = hDevice->interruptFlag;

    return ADI_FEE_SUCCESS;
}


/*!
 * @brief Waits until the Transmit buffer is empty.
 *
 * @param[in]   hDevice      Handle of flash device
 * @param[out]  ppBuffer     Returns the buffer address on success
 *
 * @return      Status
 *        - #ADI_FEE_SUCCESS if successful
 *        - #ADI_FEE_ERR_WRITE_PROTECTION address is write protected
 *        - #ADI_FEE_ERR_READ_VERIFY_ERROR erase or sign operation failed
 *        - #ADI_FEE_ABORTED operation was aborted
 *        - #ADI_FEE_ERR_UNSUPPORTED device doesn't support DMA
 *
 * @details
 *
 * This will return the Tx buffer if has been emptied,
 * otherwise waits until the buffer is empty. Only useable with the GP
 * flash controller (ADI_FEE_DEVID_GP) on ADuCM350, as only this controller
 * supports DMA.
 *
 * @sa  adi_FEE_SubmitTxBuffer(), adi_FEE_IsTxBufferAvailable()
 *
 *****************************************************************************
 */
ADI_FEE_RESULT_TYPE adi_FEE_GetTxBuffer(
                    ADI_FEE_DEV_HANDLE   const   hDevice,
                    void                       **ppBuffer
)
{
    uint16_t  flashStatus;
    volatile uint32_t count = 1000;

    if (pADI_GPF != hDevice->pFlash)
    {
        return ADI_FEE_ERR_UNSUPPORTED;
    }

    waitForInterrupt(hDevice, 0u);
    ADI_DISABLE_INT(DMA_CRC_IRQn);  /* disable the DMA interrupt */
    ADI_ENABLE_INT(hDevice->InterruptID); /* re-enable Flash controller interrupt */

    /* At this point the DMA transfers are complete but the remaining words are not yet
     * programmed into the Flash. We must wait for the flash to finish.
     */
    flashStatus = waitWhileBusy(hDevice->pFlash, BITM_FEE_FEESTA_WRBUSY);

    hDevice->pFlash->FEECON0 = 0x0000;   /* Restore to inactive */
    *ppBuffer = pDMABuffer;

    return cmdResult(flashStatus);
}
#endif /* (1 == ADI_FEE_CFG_GPF_DMA_SUPPORT) */


/*!
 *****************************************************************************
 * @brief                    Flash Write Protect
 *
 * @param[in]   hDevice      Handle of flash device
 * @param[in]   nBlockMask   memory blocks to protect
 *
 * @return      Status
 *        - #ADI_FEE_SUCCESS if successfully initialized
 *        - #ADI_FEE_ERR_UNSUPPORTED device doesn't support write protect
 *        - #ADI_FEE_ERR_WRITE_PROTECTION address is write protected
 *        - #ADI_FEE_ERR_READ_VERIFY_ERROR erase or sign operation failed
 *        - #ADI_FEE_ABORTED operation was aborted
 *        - #ADI_FEE_ERR_NOT_INITIALIZED [D] if device is not initialized
 *        - #ADI_FEE_ERR_INVALID_ADDRESS [D] if address out of range
 *
 * @details This function is used to write-protect a memory block.
 *
 * @note
 *
 * @sa  adi_FEE_GetBlockNumber()
 *****************************************************************************
 */
ADI_FEE_RESULT_TYPE adi_FEE_WriteProtectBlocks(
    ADI_FEE_DEV_HANDLE const hDevice,
    uint32_t nBlockMask
)
{
    uint32_t *pWriteProtection = (uint32_t *)WRITE_PROTECT_LOCATION;
    uint32_t  bitMask;
    uint16_t  flashStatus;
#if defined(ADI_DEBUG)
    uint16_t  protReg;
    uint16_t  protHalfWord;

    if (ADI_FEE_STATE_UNINITIALIZED == hDevice->InitState)
    {
        return ADI_FEE_ERR_NOT_INITIALIZED;
    }

    /* range check - 16 protection bits for each controller*/
    if (nBlockMask > 0xFFFFu)
    {
        return ADI_FEE_ERR_INVALID_ADDRESS;
    }
#endif /* defined(ADI_DEBUG) */

    if (pADI_FEE0 == hDevice->pFlash)        /* Flash controller 0 */
    {
        /* calculate 16k memory segment to protect */
        bitMask = nBlockMask;
#if defined(ADI_DEBUG)
        protReg =  pADI_FEE1->FEEPROL;
        protHalfWord = LO(*pWriteProtection);
#endif /* defined(ADI_DEBUG) */
    }
    else if (pADI_FEE1 == hDevice->pFlash)   /* Flash controller 1 */
    {
        /* calculate 8k memory segment to protect */
        bitMask = nBlockMask << 16u;
#if defined(ADI_DEBUG)
        protReg =  pADI_FEE1->FEEPROH;
        protHalfWord = HI(*pWriteProtection);
#endif /* defined(ADI_DEBUG) */
    }
    else
    {
        /* Write protection is not supported by the GP flash controller */
        return ADI_FEE_ERR_UNSUPPORTED;
    }

#if defined(ADI_DEBUG)
    /* If the protection for this controller has been changed from its default state
     * (i.e. unprotected) then we disallow further changes. By only permitting one
     * protection operation, for each of the two flash controllers, we can ensure
     * the limit of two writes to an individual flash location is not exceeded.
     */
    if ((0xFFFFu != protReg) || (0xFFFFu != protHalfWord))
    {
        /* Protection has already been programmed */
        return ADI_FEE_ERR_WRITE_PROT_LIMIT;
    }
#endif /* defined(ADI_DEBUG) */

    /* The following operations must be performed on Flash Controller 1, even if
     * the block being protected is part of the memory controlled by Flash Controller 0.
     * This is because the write protection control word itself (which controls both regions)
     * is within Controller 1's memory range.
     */

    waitWhileBusy(pADI_FEE1, (BITM_FEE_FEESTA_CMDBUSY | BITM_FEE_FEESTA_WRBUSY));

    /* set key register value */
    pADI_FEE1->FEEKEY = FEEKEY_VALUE_USERKEY1;
    pADI_FEE1->FEEKEY = FEEKEY_VALUE_USERKEY2;

    pADI_FEE1->FEECON0 = BITM_FEE_FEECON0_WREN; /* enable flash write */

    /* write the data, inverting it to clear the bits that are set in bitMask */
    *pWriteProtection = ~bitMask;

    flashStatus = waitUntilDone(hDevice->pFlash, BITM_FEE_FEESTA_WRDONE);
    pADI_FEE1->FEECON0 = 0u;    /* disable flash write */

    /* After writing to the write-protect location, a 16-bit value must be written
     * again to FEE1KEY, to re-assert the key protection.
     */
    pADI_FEE1->FEEKEY = 0x0u;

    return cmdResult(flashStatus);
}


/*!
 *****************************************************************************
 * @brief                    Gets a block number as a bitmask
 *
 * @param[in]   hDevice      Handle of flash device
 * @param[in]   nAddress     Memory address to find out which block it belongs to
 * @param[out]  *pnBlock     Block mask output argument
 *
 * @return      Status
 *             - #ADI_FEE_SUCCESS if successfully initialized
 *             - #ADI_FEE_ERR_INVALID_ADDRESS if address out of range
 *
 * @details
 *
 * Converts an absolute flash memory address to a number identifying a
 * specific block within the specified controller's flash region. Since there
 * are always 32 blocks per region the block number is represented as a
 * bitmask with one bit set. This allows multiple block numbers to be ORed
 * together to express a set of blocks.
 *
 * @note
 *
 * @sa   adi_FEE_WriteProtectBlocks()
 *
 *****************************************************************************
 */
ADI_FEE_RESULT_TYPE adi_FEE_GetBlockNumber(
    ADI_FEE_DEV_HANDLE const hDevice,
    uint32_t nAddress,
    uint32_t *pnBlock
)
{
    /* check to see is address is valid */
    uint32_t relAddress = nAddress - hDevice->BaseAddress;

    if (relAddress >= hDevice->FlashSize)
    {
        return ADI_FEE_ERR_INVALID_ADDRESS;
    }

    /* calculate block mask */
    *pnBlock = 1u << (relAddress >> hDevice->BlockShift);

    return ADI_FEE_SUCCESS;
}


/*!
 *****************************************************************************
 * @brief                    Gets a page number
 *
 * @param[in]   hDevice      Handle of flash device
 * @param[in]   nAddress     Memory address to find out which page number it belongs to
 * @param[out]  *pnPage      Page number output argument
 *
 * @return      Status
 *                           - #ADI_FEE_SUCCESS if successfully initialized
 *                           - #ADI_FEE_ERR_INVALID_ADDRESS if address out of range
 *
 * @details
 *
 * Converts an absolute flash memory address to a number identifying a
 * specific page within the specified controller's flash region.
 *
 * @note
 *
 * @sa
 *****************************************************************************
 */
ADI_FEE_RESULT_TYPE adi_FEE_GetPageNumber(
    ADI_FEE_DEV_HANDLE const hDevice,
    uint32_t nAddress,
    uint32_t *pnPage
)
{
    /* check to see if address is valid */
    uint32_t relAddress = nAddress - hDevice->BaseAddress;

    if (relAddress >= hDevice->FlashSize)
    {
        return ADI_FEE_ERR_INVALID_ADDRESS;
    }

    /* calculate page number */
    *pnPage = relAddress >> hDevice->PageShift;

    return ADI_FEE_SUCCESS;
}

/*!
 *****************************************************************************
 * @brief                    Enable or disable flash parity checking
 *
 * @param[in]   hDevice      Handle of flash device
 * @param[in]   eParityCtrl  Parity control setting
 *
 * @return      Status
 *        - #ADI_FEE_SUCCESS if successful
 *        - #ADI_FEE_ERR_UNSUPPORTED device doesn't support parity checking
 *        - #ADI_FEE_ERR_INVALID_PARAMETER if eParityCtrl is invalid
 *
 * @details
 *
 *
 * @note
 *
 * @sa           adi_FEE_GetParityError().
 *****************************************************************************
*/
ADI_FEE_RESULT_TYPE adi_FEE_SetParityChecking (
    ADI_FEE_DEV_HANDLE const hDevice,
    ADI_FEE_PARITY_CTRL const eParityCtrl
)
{
    uint16_t regValue;

    if (pADI_GPF == hDevice->pFlash)        /* GP flash controller */
    {
        /* Parity is not supported by the GP flash controller */
        return ADI_FEE_ERR_UNSUPPORTED;
    }

    switch (eParityCtrl)
    {
    case ADI_FEE_PARITY_DISABLE:
        regValue = 0u;
        break;
    case ADI_FEE_PARITY_ENABLE:
        regValue = BITM_FEE_FEEPARCTL_PAREN;
        break;
    case ADI_FEE_PARITY_ENABLE_INTERRUPT:
        regValue = BITM_FEE_FEEPARCTL_PAREN | BITM_FEE_FEEPARCTL_PERREXEN;
        break;
    default:
        return ADI_FEE_ERR_INVALID_PARAMETER;
    }

    hDevice->pFlash->FEEPARCTL = regValue;

    if ((pADI_FEE0->FEEPARCTL & BITM_FEE_FEEPARCTL_PERREXEN) ||
        (pADI_FEE1->FEEPARCTL & BITM_FEE_FEEPARCTL_PERREXEN))
    {
        ADI_ENABLE_INT(PARITY_IRQn);
    }
    else
    {
        ADI_DISABLE_INT(PARITY_IRQn);
    }

    return ADI_FEE_SUCCESS;
}

/*!
 *****************************************************************************
 * @brief                    Check for parity error
 *
 * @param[in]   hDevice      Handle of flash device
 * @param[out]  pAddress     Return the address of the parity error
 *
 * @return      Status
 *                 - #ADI_FEE_SUCCESS if no parity error has occurred
 *                 - #ADI_FEE_ERR_PARITY_ERROR if a parity error has occurred
 *
 * @details
 *
 *
 * @note *pAddress is only set when the return value is ADI_FEE_ERR_PARITY_ERROR.
 *
 * @sa           adi_FEE_SetParityChecking().
 *****************************************************************************
 */
ADI_FEE_RESULT_TYPE adi_FEE_GetParityError(
    ADI_FEE_DEV_HANDLE const hDevice,
    uint32_t *pAddress
)
{
    uint32_t address;

    if (0u == (hDevice->pFlash->FEEPARSTA & BITM_FEE_FEEPARSTA_PARERR))
    {
        /* No parity error has occurred (since the last check) */
        return ADI_FEE_SUCCESS;
    }

    /* Get the failing address from the flash controller */
    address = hDevice->pFlash->FEEPARADRL;
    address |= (hDevice->pFlash->FEEPARADRH << 16u);

    /* Clear the parity error flag */
    hDevice->pFlash->FEEPARSTA = BITM_FEE_FEEPARSTA_PARERR; /* write 1 to clear */

    /* Return the failing address */
    *pAddress = address;

    return ADI_FEE_ERR_PARITY_ERROR;
}

/*!
 *****************************************************************************
* @brief                    Enable or disable flash abort for system interrupts
*
* @param[in]   hDevice      Handle of flash device
* @param[in]   interrupt    The system interrupt
* @param[in]   bEnable      If true enable abort for specified interrupt, otherwise disable
*
* @return      Status
*                           - #ADI_FEE_SUCCESS if successful
*                           - #ADI_FEE_ERR_INVALID_PARAMETER if interrupt is invalid
*
* @details
*
*
* @note
*
* @sa           adi_FEE_Abort().
 *****************************************************************************
*/
ADI_FEE_RESULT_TYPE adi_FEE_SetInterruptAbort(
    ADI_FEE_DEV_HANDLE const hDevice,
    IRQn_Type interrupt,
    bool_t bEnable
)
{
    uint32_t interruptNumber = (uint32_t)interrupt;
    uint16_t volatile * pAbortEnReg;

    /* Find the abort enable register, and bit position,
     * corresponding to this interrupt.
     */
    if (interruptNumber < 16u)
    {
        pAbortEnReg = &hDevice->pFlash->FEEAEN0;
    }
    else if (interruptNumber < 32u)
    {
        pAbortEnReg = &hDevice->pFlash->FEEAEN1;
        interruptNumber -= 16u;
    }
    else if (interruptNumber < 48u)
    {
        pAbortEnReg = &hDevice->pFlash->FEEAEN2;
        interruptNumber -= 32u;
    }
    else if (interruptNumber < 64u)
    {
        pAbortEnReg = &hDevice->pFlash->FEEAEN3;
        interruptNumber -= 48u;
    }
    else
    {
        return ADI_FEE_ERR_INVALID_PARAMETER;
    }

    if (bEnable)
    {
        /* Set the abort enable bit for this interrupt */
        *pAbortEnReg |= (1u << interruptNumber);
    }
    else
    {
        /* Clear the abort enable bit for this interrupt */
        *pAbortEnReg &= ~(1u << interruptNumber);
    }

    return ADI_FEE_SUCCESS;
}

/*!
 *****************************************************************************
* @brief
*
* @param[in]   bEnable      If true enable serial wire read protection, otherwise disable
*
* @return      Status
*                           - #ADI_FEE_SUCCESS if successful
*
* @details
*
*
* @note
*
* @sa
 *****************************************************************************
*/
ADI_FEE_RESULT_TYPE adi_FEE_ReadProtectEnable(
    bool_t bEnable
)
{
    /* Set key register value */
    pADI_FEE1->FEEKEY = FEEKEY_VALUE_USERKEY1;
    pADI_FEE1->FEEKEY = FEEKEY_VALUE_USERKEY2;

    if (bEnable)
    {
        /* To *enable* the read-protection, we *clear* the serial wire debug
         * enable bit.
         */
        pADI_FEE1->FEECON1 = 0u;
    }
    else
    {
        /* To *disable* the read-protection, we *set* the serial wire debug
         * enable bit.
         */
        pADI_FEE1->FEECON1 = BITM_FEE_FEECON1_DBG;
    }

    /* After writing to FEECON1, a 16-bit value must be written again to FEE1KEY,
     * to re-assert the key protection.
     */
    pADI_FEE1->FEEKEY = 0x0u;

    return ADI_FEE_SUCCESS;
}



/*
** EOF
*/

/*@}*/