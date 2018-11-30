/*********************************************************************************

Copyright (c) 2012-2014 Analog Devices, Inc. All Rights Reserved.

This software is proprietary to Analog Devices, Inc. and its licensors.  By using 
this software you agree to the terms of the associated Analog Devices Software 
License Agreement (See /LICENSES/ADI.LABLAB.LICENSE). 

Modified from the Analog Devices original.
Portions Copyright (c) 2018 Andrew Grosser, Moritz von Buttlar.

*********************************************************************************/

/*!
 *****************************************************************************
 * @file:    I2S.c
 * @brief:   I2S Device Implementations for ADuCM350
 * @version:
 * @date:    $Date:
 *****************************************************************************/

/*! \addtogroup I2S_Driver I2S Driver
 *  @{
 */

/*! \cond PRIVATE */

#define ASSERT_ADI_MISRA_SUPPRESSIONS  /*!< Apply ADI MISRA Suppressions */
#include "misra.h"

#include <stdlib.h>  /* for 'NULL" definition */
#include "I2S.h"
#include "dma.h"

/* forward declarations... */
/* These are needed in the case where ADI_CFG_ENABLE_RTOS_SUPPORT=1              */
/* The ADI_INSTALL_HANDLER macro will expand to a call to adi_int_InstallHandler */
/* and the handler will be referenced                                            */
/* The non-OSAL expansion of ADI_INSTALL_HANDLER results in a NULL string        */
ADI_INT_HANDLER(I2S_Int_Handler);
ADI_INT_HANDLER(DMA_I2S_Int_Handler);


/* Dispatch DMA overhead through function pointers so linker */
/* elimination can optimize them out when not referenced...  */

/*! \typedef ADI_I2S_INIT_FPTR
 * Pointer to DMA/Interrupt initialization function. */
typedef ADI_I2S_RESULT_TYPE (*ADI_I2S_INIT_FPTR)(ADI_I2S_DEV_HANDLE const hDevice);

/*! \typedef ADI_I2S_UNINIT_FPTR
 * Pointer to DMA/Interrupt uninitialization function. */
typedef ADI_I2S_RESULT_TYPE (*ADI_I2S_UNINIT_FPTR)(ADI_I2S_DEV_HANDLE const hDevice);

/*! \typedef ADI_I2S_INIT_DESCRS_FPTR
 * Pointer to DMA descriptor initialization function. */
typedef ADI_I2S_RESULT_TYPE (*ADI_I2S_INIT_DESCRS_FPTR)(ADI_I2S_DEV_HANDLE const hDevice, DMA_CHANn_TypeDef chan);

/*! \typedef ADI_I2S_DMA_HANDLER_FPTR
 * Pointer to DMA handler function. */
typedef void (*ADI_I2S_DMA_HANDLER_FPTR)(DMA_CHANn_TypeDef channelNum);

/*! \struct ADI_I2S_FTAB
 * The function pointer table. */
typedef struct {
    ADI_I2S_INIT_FPTR         pInitFcn;
    ADI_I2S_UNINIT_FPTR       pUnInitFcn;
    ADI_I2S_INIT_DESCRS_FPTR  pInitDescriptorsFcn;
    ADI_I2S_DMA_HANDLER_FPTR  pDmaHandlerFcn;
} ADI_I2S_FTAB;

/* defines... */

/*! Fifo watermark level */
#define ADI_I2S_FIFO_WATERMARK          5


/* file statics... */

#if (ADI_I2S_CFG_ENABLE_DMA_SUPPORT == 1)
/* single, shared DMA descriptor block */
static ADI_DMA_TRANSFER_TYPE gDmaDescriptor;
#endif /* ADI_I2S_CFG_ENABLE_DMA_SUPPORT */

/*!
 * I2S Device instance data structure */
typedef struct ADI_I2S_DEV_DATA_TYPE
{
    ADI_I2S_STATE_TYPE         initState;             /*!< Device initialization state        */
    ADI_I2S_TypeDef*           pI2S;                  /*!< I2S  MMRs                          */

    ADI_I2S_RESULT_TYPE        result;                /*!< Transaction result                 */
    bool_t                     bDmaMode;              /*!< DMA-Mode flag                      */
    bool_t                     bBlockingMode;         /*!< Blocking-Mode flag                 */

    uint8_t                   *pTxBuffer;             /*!< Next transmit byte pointer         */
    volatile uint16_t          TxRemaining;           /*!< Remaining transmit byte count      */

    bool_t                     b24bitMode;            /*!< 24 bit mode flag                   */

    /*
    * I2S data register address are stored in this array. To avoid condition checks in the
      interrupt handler, this table is constructed when the data flow is enabled.

     ---- In case of 16bit stereo mode ---
     DataPtrs[0] = &I2S_OUT1H
     DataPtrs[1] = &I2S_OUT2H
     NumChnls = 2

    --- In case of 24bit stereo mode ----
    DataPtrs[0] = &I2S_OUT1L
    DataPtrs[1] = &I2S_OUT1H
    DataPtrs[2] = &I2S_OUT2L
    DataPtrs[3] = &I2S_OUT2H
    NumChnls = 4
    --- In case of 16bit Mono mode ----
    DataPtrs[0] = &I2S_OUT1H
    NumChnls = 1

    --- In case of 24bit Mono mode
    DataPtrs[0] = &I2S_OUT1L
    DataPtrs[0] = &I2S_OUT1H
    NumChnls = 2
    */
    volatile uint16_t         *DataPtrs[4];           /* I2S data pointers used in interrupt mode */
    uint8_t                    NumChnls;              /* Number of channels */

    /* function table */
    ADI_I2S_FTAB               FunctionTable;

    /* DMA-related... */
    volatile bool_t            bDmaComplete;

    ADI_DMA_TRANSFER_TYPE*     pDmaDescriptor;
    ADI_DMA_CCD_TYPE           nextDescrType;

    /* Callback and Callback parameters */
    ADI_CALLBACK               pfCallback;
    void *                     pCBParam;

#if (ADI_CFG_ENABLE_RTOS_SUPPORT == 1)
    /* Memory required to create the semaphore */
    uint8_t                    SemMem[ADI_OSAL_MAX_SEM_SIZE_CHAR];
    /* Semaphore handle */
    ADI_OSAL_SEM_HANDLE        hTxSem;
#endif /* ADI_CFG_ENABLE_RTOS_SUPPORT */

} ADI_I2S_DEV_DATA_TYPE;

/* only one instance... */
static ADI_I2S_DEV_DATA_TYPE I2S_DevData[ADI_I2S_MAX_DEVID];


/* device data pointer */
static ADI_I2S_DEV_DATA_TYPE* const pI2S0_DevData = &I2S_DevData[0];


/*!
 *****************************************************************************
 * I2S Configuration structure.
 *****************************************************************************/
typedef struct ADI_I2S_CONFIG
{
    uint16_t I2S_MODE1;                        /*!< I2S MODE1 register configuration. */
    uint16_t I2S_MODE2;                        /*!< I2S_MODE2 register configuration. */
    uint16_t I2S_CFG1;                         /*!< I2S_CFG1 register configuration. */
    uint16_t I2S_CFG2;                         /*!< I2S_CFG2 register configuration. */

} ADI_I2S_CFG_TYPE;


/* I2S Application configuration array */
static const ADI_I2S_CFG_TYPE gI2SCfg[ADI_I2S_MAX_DEVID] =
{
  /* Initialize Instance 0 configuration. */
  {
       /**** I2S_MODE1 register configuration *** */

       (( I2S0_CFG_DRIVE_UNUSED_BITS            << BITP_I2S_I2S_MODE1_DRV_HIZ    ) |
        ( I2S0_CFG_MASTER_SLAVE_MODE            << BITP_I2S_I2S_MODE1_SAI_MS     ) |
        ( I2S0_CFG_BIT_CLOCKS_PER_CHANNEL       << BITP_I2S_I2S_MODE1_BCLK_RATE  ) |
        ( I2S0_CFG_LSB_FIRST_MODE               << BITP_I2S_I2S_MODE1_SAI_MSB    ) |
        ( I2S0_CFG_LR_CLOCK_MODE                << BITP_I2S_I2S_MODE1_LR_MODE    ) |
        ( I2S0_CFG_DATA_OUTPUT_WIDTH            << BITP_I2S_I2S_MODE1_DATA_WIDTH ) |
        ( I2S0_CFG_DATA_SLOT_WIDTH              << BITP_I2S_I2S_MODE1_SLOT_WIDTH ) |
        ( I2S0_CFG_FRAME_RATE                   << BITP_I2S_I2S_MODE1_FS         ) |
        ( I2S0_CFG_CHNL_FORMAT                  << BITP_I2S_I2S_MODE1_SAI        ) |
        ( I2S0_CFG_SERIAL_DATA_FORMAT           << BITP_I2S_I2S_MODE1_SDATA_FMT  )),

        /**** I2S_MODE2 register configuration */

       (( I2S0_CFG_TDM_SLOT_CHANNEL1_MAP        << BITP_I2S_I2S_MODE2_CMAP_C1    ) |
        ( I2S0_CFG_TDM_SLOT_CHANNEL2_MAP        << BITP_I2S_I2S_MODE2_CMAP_C2    ) |
        ( I2S0_CFG_DRIVE_CHANNEL1_DATA          << BITP_I2S_I2S_MODE2_DRV_CH1    ) |
        ( I2S0_CFG_DRIVE_CHANNEL2_DATA          << BITP_I2S_I2S_MODE2_DRV_CH2    ) |
        ( I2S0_CFG_BIT_CLK_SAMPLING_EDGE        << BITP_I2S_I2S_MODE2_BCLK_EDGE  ) |
        ( I2S0_CFG_INVERT_LR_CLOCK              << BITP_I2S_I2S_MODE2_LR_POL     )),

        /**** I2S_CFG1 register configuration ***/

       (( I2S0_CFG_ENABLE_CHANNEL1_ON_APB_BUS   << BITP_I2S_I2S_CFG1_CHAN_SEL1   ) |
        ( I2S0_CFG_ENABLE_CHANNEL2_ON_APB_BUS   << BITP_I2S_I2S_CFG1_CHAN_SEL2   ) |
        ( I2S0_CFG_TRANSFER_MODE                << BITP_I2S_I2S_CFG1_TRAN_MODE   ) |
        ( I2S0_CFG_ENABLE_FIFO_STATUS_INTERRUPT << BITP_I2S_I2S_CFG1_STAT_EN     ) |
        ( I2S0_CFG_ENABLE_DATA_REQ_INTERRUPT    << BITP_I2S_I2S_CFG1_REQ_EN      ) |
        ( I2S0_CFG_RESET_INCREMENT_ADDRESS      << BITP_I2S_I2S_CFG1_INCR_RST    ) |
        ( I2S0_CFG_RESET_FIFO_POINTERS          << BITP_I2S_I2S_CFG1_FIFO_RST    )),

        /**** I2S_CFG2 register configuration ***/

       (( I2S0_CFG_OUTPUT_DATA_SAMPLING_FREQ    << BITP_I2S_I2S_CFG2_SAMP_FREQ   ) |
        ( I2S0_CFG_DMA_REQ_SAMPLING_FREQ        << BITP_I2S_I2S_CFG2_DMA_FREQ    ) |
        ( I2S0_CFG_DATA_REQ_SAMPLING_FREQ       << BITP_I2S_I2S_CFG2_REQ_FREQ    ))
    }
};


/* forward declarations... */

/* Internal API... */
static ADI_I2S_RESULT_TYPE commonInit               (ADI_I2S_DEV_HANDLE const hDevice);
static void                configureNVIC            (ADI_I2S_DEV_HANDLE const hDevice, bool_t bEnable);

#if (ADI_I2S_CFG_ENABLE_DMA_SUPPORT == 1)
/* DMA support... */
static ADI_I2S_RESULT_TYPE scheduleNextDescriptor   (ADI_I2S_DEV_HANDLE const hDevice);

/* pointer table functions... */
static ADI_I2S_RESULT_TYPE dmaInit                  (ADI_I2S_DEV_HANDLE const hDevice);
static ADI_I2S_RESULT_TYPE dmaUnInit                (ADI_I2S_DEV_HANDLE const hDevice);
static ADI_I2S_RESULT_TYPE dmaInitializeDescriptors (ADI_I2S_DEV_HANDLE const hDevice, DMA_CHANn_TypeDef chan);
static void                dmaCommonHandler         (DMA_CHANn_TypeDef channelNum);
static void                intCommonHandler         (DMA_CHANn_TypeDef channelNum);
#endif /* ADI_I2S_CFG_ENABLE_DMA_SUPPORT */

static ADI_I2S_RESULT_TYPE intInit                  (ADI_I2S_DEV_HANDLE const hDevice);
static ADI_I2S_RESULT_TYPE intUnInit                (ADI_I2S_DEV_HANDLE const hDevice);
static ADI_I2S_RESULT_TYPE intInitializeDescriptors (ADI_I2S_DEV_HANDLE const hDevice, DMA_CHANn_TypeDef chan);



/*! \endcond */

/*
    //////////////////////////////////////////////////////////////////////////////
    //////////////////////   API IMPLEMENTATIONS   ///////////////////////////////
    //////////////////////////////////////////////////////////////////////////////
*/


/*!
 * @brief  Initialize and allocate the I2S device.
 *
 * @param[in]        devID      Zero-based device index designating which device to initialize.
 * @param[out]       phDevice   The caller's device handle pointer for storing the initialized device instance data.
 *
 * @return            Status
 *                    - #ADI_I2S_ERR_ALREADY_INITIALIZED [D]    Specified device has already been initialized.
 *                    - #ADI_I2S_ERR_BAD_DEVICE_INDEX [D]       Invalid device index.
 *                    - #ADI_I2S_ERR_SEMAPHORE_FAILED           Failed create the semaphore. (in multithread mode).
 *                    - #ADI_I2S_SUCCESS                        Call completed successfully.
 *
 * Initialize an I2S device and allocate the device for use.
 * The caller's device handle is written with the device instance data pointer.
 * The returned device handle is required to be passed to all subsequent APIs to convey which device instance to operate on.
 *
 * @note    The contents of phDevice will be set to NULL on failure.
 *
 * @sa      adi_I2S_UnInit()
 */
ADI_I2S_RESULT_TYPE adi_I2S_Init(ADI_I2S_DEV_ID_TYPE const devID, ADI_I2S_DEV_HANDLE* const phDevice)
{
    ADI_I2S_DEV_HANDLE hDevice;
    ADI_I2S_RESULT_TYPE result;

#ifdef ADI_DEBUG
    if (devID >= ADI_I2S_MAX_DEVID)
        return ADI_I2S_ERR_BAD_DEVICE_INDEX;

    if (ADI_I2S_STATE_UNINITIALIZED != pI2S0_DevData->initState)
        return ADI_I2S_ERR_ALREADY_INITIALIZED;
#endif

    /* set local device handle */
    hDevice = &I2S_DevData[devID];

    /* store a bad handle in case of failure */
    *phDevice = NULL;

    /* do common init */
    result = commonInit(hDevice);

    if (ADI_I2S_SUCCESS != result)
        return result;

	ADI_I2S_CFG_TYPE const* pI2SCfg = &gI2SCfg[devID];

	/* Initialize the device based on the given configuration parameters */
	hDevice->pI2S->I2S_MODE1 = pI2SCfg->I2S_MODE1;
	hDevice->pI2S->I2S_MODE2 = pI2SCfg->I2S_MODE2;
	hDevice->pI2S->I2S_CFG1  = pI2SCfg->I2S_CFG1;
	hDevice->pI2S->I2S_CFG2  = pI2SCfg->I2S_CFG2;

	/* Set the audio format parameters based on the given configuration */
	hDevice->b24bitMode      = (pI2SCfg->I2S_MODE1 & BITM_I2S_I2S_MODE1_DATA_WIDTH) ? false : true;


    /* Install the I2S interrupt handlers */
    ADI_INSTALL_HANDLER(ADI_INT_I2S, I2S_Int_Handler);
    ADI_INSTALL_HANDLER(ADI_INT_DMA_I2S, DMA_I2S_Int_Handler);

#if (ADI_CFG_ENABLE_RTOS_SUPPORT == 1)
    /* Create the semaphore for blocking mode support. */
    if(adi_osal_SemCreateStatic(hDevice->SemMem, ADI_OSAL_MAX_SEM_SIZE_CHAR, &hDevice->hTxSem, 0) != ADI_OSAL_SUCCESS)
    {
        return ADI_I2S_ERR_SEMAPHORE_FAILED;
    }
#endif /* ADI_CFG_ENABLE_RTOS_SUPPORT */

    /* init successful */
    hDevice->initState = ADI_I2S_STATE_INITIALIZED;

    /* store the "device" handle last */
    *phDevice = hDevice;

    return ADI_I2S_SUCCESS;
}



/*!
 * @brief  Uninitialize and deallocate the I2S device.
 *
 * @param[in]   hDevice    Device handle obtained from adi_I2S_Init().
 *
 * @return      Status
 *                - #ADI_I2S_ERR_BAD_DEVICE_HANDLE [D]  Invalid device handle parameter.
 *                - #ADI_I2S_ERR_NOT_INITIALIZED [D]    Device has not been previously configured for use.
 *                - #ADI_I2S_ERR_BAD_DMA_CONFIG         DMA channel failed to uninitialize
 *                - #ADI_I2S_SUCCESS                    Call completed successfully.
 *
 * Uninitialize and release an allocated I2S device for other use.
 *
 * @sa        adi_I2S_Init()
 */
ADI_I2S_RESULT_TYPE adi_I2S_UnInit(ADI_I2S_DEV_HANDLE const hDevice)
{
#ifdef ADI_DEBUG
    if (pI2S0_DevData != hDevice)
        return ADI_I2S_ERR_BAD_DEVICE_HANDLE;

    if (ADI_I2S_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_I2S_ERR_NOT_INITIALIZED;

#endif

    /* disable bus error capture */
    NVIC_DisableIRQ(DMA_ERR_IRQn);

    /* disable NVIC */
    configureNVIC(hDevice, false);

    /* UnInstall the I2S interrupt handlers */
    ADI_UNINSTALL_HANDLER(ADI_INT_I2S);
    ADI_UNINSTALL_HANDLER(ADI_INT_DMA_I2S);

#if (ADI_CFG_ENABLE_RTOS_SUPPORT == 1)
    /* Destroy the semaphore. */
    if(adi_osal_SemDestroyStatic(hDevice->hTxSem) != ADI_OSAL_SUCCESS)
    {
        return ADI_I2S_ERR_SEMAPHORE_FAILED;
    }
#endif /* ADI_CFG_ENABLE_RTOS_SUPPORT */

    /* call uninitialize helper */
    if (ADI_I2S_SUCCESS != hDevice->FunctionTable.pUnInitFcn(hDevice))
        return ADI_I2S_ERR_BAD_DMA_CONFIG;

    hDevice->initState = ADI_I2S_STATE_UNINITIALIZED;


    return ADI_I2S_SUCCESS;
}



/*!
 * @brief  Set the I2S device transaction blocking mode.
 *
 * @param[in]   hDevice       Device handle obtained from adi_I2S_Init().
 * @param[in]   bFlag         Flag to select between blocking and non-blocking modes.
 *                - true      Use blocking-mode.
 *                - false     Use non-blocking-mode.
 *
 * @return         Status
 *                - #ADI_I2S_ERR_BAD_DEVICE_HANDLE [D]  Invalid device handle parameter.
 *                - #ADI_I2S_ERR_NOT_INITIALIZED [D]    Device has not been previously configured for use.
 *                - #ADI_I2S_ERR_BAD_MODE_COMBO [D]     Blocking-Mode is incompatible with DMA-Mode.
 *                - #ADI_I2S_SUCCESS                    Call completed successfully.
 *
 * Sets the blocking mode for data transfers.  Transfers made in blocking-mode will not return until complete.
 * Transfers made in non-blocking-mode may return before the transfer is complete.  Applications must use
 * adi_I2S_GetNonBlockingStatus() to ascertain the completion status of non-blocking transactions.
 *
 * @sa        adi_I2S_GetNonBlockingStatus()
 * @sa        adi_I2S_SetDmaMode()
 */
ADI_I2S_RESULT_TYPE adi_I2S_SetBlockingMode (ADI_I2S_DEV_HANDLE const hDevice, bool_t const bFlag)
{
#ifdef ADI_DEBUG
    if (pI2S0_DevData != hDevice)
        return ADI_I2S_ERR_BAD_DEVICE_HANDLE;

    if (ADI_I2S_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_I2S_ERR_NOT_INITIALIZED;
#endif

    hDevice->bBlockingMode = bFlag;

    return ADI_I2S_SUCCESS;
}


/*!
 * @brief  Get the I2S device non-blocking status.
 *
 * @param[in]   hDevice            Device handle obtained from adi_I2S_Init().
 * @param[out]  pBytesRemaining    Pointer to an application variable to store the remaining transaction byte count.
 *
 * @return         Status
 *                - #ADI_I2S_ERR_BAD_DEVICE_HANDLE [D]    Invalid device handle parameter.
 *                - #ADI_I2S_ERR_NOT_INITIALIZED [D]      Device has not been previously configured for use.
 *                - #ADI_I2S_ERR_BLOCKING_ACTIVE [D]      Device is in Blocking-Mode.
 *                - #ADI_I2S_SUCCESS                      Transaction completed successfully.
 *
 * When the Non-Blocking mode is used, the buffer transaction API adi_I2S_Transmit return immediately,
 * allowing the application to do other work.  During such transactions, the application may poll the driver with this API
 * to ascertain the progress of the transaction.  While the transaction is incomplete and no error has occurred, the API
 * returns #ADI_I2S_SUCCESS and the variable pointed to by the pBytesRemaining parameter is set with the
 * number of bytes remaining in the transfer.
 *
 * There are a number of error conditions that may halt the transaction prematurely due to various events
 * which the application must reconcile.
 *
 * @sa        adi_I2S_SetBlockingMode()
 */
ADI_I2S_RESULT_TYPE adi_I2S_GetNonBlockingStatus (ADI_I2S_DEV_HANDLE const hDevice, uint16_t* const pBytesRemaining)
{
#ifdef ADI_DEBUG
    if (pI2S0_DevData != hDevice)
        return ADI_I2S_ERR_BAD_DEVICE_HANDLE;

    if (ADI_I2S_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_I2S_ERR_NOT_INITIALIZED;

    if (hDevice->bBlockingMode)
        return ADI_I2S_ERR_BLOCKING_ACTIVE;
#endif

    /* writeback the remaining count */
    *pBytesRemaining = hDevice->TxRemaining;

    /* transaction complete, return result */
    return hDevice->result;
}

/*!
 * @brief  Transmit a bufffer of audio samples.
 *
 * @param[in]   hDevice            Device handle obtained from adi_I2S_Init().
 * @param[in]   pBuffer            Pointer to the sample buffer to be transmitted.
 * @param[in]   nBufferSize        Buffer size in bytes.
 *
 * @return         Status
 *                - #ADI_I2S_ERR_BAD_DEVICE_HANDLE [D]    Invalid device handle parameter.
 *                - #ADI_I2S_ERR_NOT_INITIALIZED [D]      Device has not been previously configured for use.
 *                - #ADI_I2S_ERR_NULL_DATA_POINTER [D]    The given pointer to the buffer is pointing to NULL.
 *                - #ADI_I2S_ERR_INVALID_DATA_SIZE [D]    The size of the buffer is invalid.
 *                - #ADI_I2S_ERR_SEMAPHORE_FAILED         Failed to pend for the semaphore. (Only returned in Multi-threaded mode)
 *                - #ADI_I2S_SUCCESS                      Transaction completed successfully.
 *
 * When the Non-Blocking mode is used, this API returns immediately, allowing the application to do other work.
 * The transaction status can be known by calling the adi_I2S_GetNonBlockingStatus, when in PIO mode. When in DMA mode
 * the transaction status can be known by calling adi_I2S_GetDmaCompletion.
 *
 * @sa        adi_I2S_SetBlockingMode()
 */
// Transaction APIs
ADI_I2S_RESULT_TYPE adi_I2S_Transmit        (ADI_I2S_DEV_HANDLE const hDevice,
                                                        uint8_t      *pBuffer,
                                                       uint16_t const nBufferSize)
{

#ifdef ADI_DEBUG
    if (pI2S0_DevData != hDevice)
        return ADI_I2S_ERR_BAD_DEVICE_HANDLE;

    if (ADI_I2S_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_I2S_ERR_NOT_INITIALIZED;

    if(NULL == pBuffer)
        return ADI_I2S_ERR_NULL_DATA_POINTER;

    if(0 == nBufferSize)
        return ADI_I2S_ERR_INVALID_DATA_SIZE;

#endif
      /* enable NVIC */
      configureNVIC(hDevice, true);

      hDevice->pTxBuffer   = pBuffer;                 /*!< Next transmit byte pointer         */
      hDevice->TxRemaining = nBufferSize;             /*!< Remaining transmit byte count         */
      hDevice->result      = ADI_I2S_SUCCESS;

#if (ADI_I2S_CFG_ENABLE_DMA_SUPPORT == 1)
      if (hDevice->bDmaMode)
      {
        hDevice->bDmaComplete = false;

        /* program initial descriptor pair */
        hDevice->FunctionTable.pInitDescriptorsFcn(hDevice, I2S_CHANn);

        ADI_ENTER_CRITICAL_REGION();

        /* enable dma mode */
        pADI_I2S->I2S_CFG1 |= (BITM_I2S_I2S_CFG1_AUTO_INCR | BITM_I2S_I2S_CFG1_DMA_EN);

        ADI_EXIT_CRITICAL_REGION();
      }
#endif /* ADI_I2S_CFG_ENABLE_DMA_SUPPORT */

     /* In multi-threaded mode block using semaphore */
      if(hDevice->bBlockingMode == true)
     {

#if (ADI_CFG_ENABLE_RTOS_SUPPORT == 1)

        /* In multi-threaed mode use semaphore to wait for the transfer complete. The semaphore is
           posted from the ISR when the trasfer is complete */
        if(adi_osal_SemPend(hDevice->hTxSem, ADI_OSAL_TIMEOUT_FOREVER) != ADI_OSAL_SUCCESS)
        {
           return ADI_I2S_ERR_SEMAPHORE_FAILED;
        }

#else /* Non multi-thread */

        if(hDevice->bDmaMode)
        {
            /* In DMA Mode wait for DMA complete flag to be set */
            while(hDevice->bDmaComplete == false)
            {
               /* Instead of busy-waiting, we enter core sleep mode while waiting. Any
                * interrupt that changes NumAvailable will also bring the processor
                * out of core sleep and cause this call to return.
                */

               SystemEnterLowPowerMode(ADI_SYS_MODE_CORE_SLEEP, /* wait in core sleep */
                                       NULL,
                                       0);
            }
        }
        else
        {
          /* In non-DMA mode wait until the number of bytes remaining to transfer to become 0 */
          while(hDevice->TxRemaining > 0)
          {
               /* Instead of busy-waiting, we enter core sleep mode while waiting. Any
                * interrupt that changes NumAvailable will also bring the processor
                * out of core sleep and cause this call to return.
                */
              SystemEnterLowPowerMode(ADI_SYS_MODE_CORE_SLEEP, /* wait in core sleep */
                                      NULL,
                                      0);
          }
        }

#endif /* ADI_CFG_ENABLE_RTOS_SUPPORT */

     }
      return ADI_I2S_SUCCESS;

}


/*!
 * @brief  Set the I2S device transaction DMA-Mode.
 *
 * @param[in]   hDevice       Device handle obtained from adi_I2S_Init().
 * @param[in]   bFlag         Flag to select between DMA and Non-DMA modes.
 *                - true     Use DMA-Mode transfers.
 *                - false    Use Non-DMA-Mode transfers.
 *
 * @return         Status
 *                - #ADI_I2S_ERR_BAD_DEVICE_HANDLE [D]  Invalid device handle parameter.
 *                - #ADI_I2S_ERR_NOT_INITIALIZED [D]    Device has not been previously configured for use.
 *                - #ADI_I2S_ERR_BAD_MODE_COMBO [D]     Blocking-Mode is incompatible with DMA-Mode.
 *                - #ADI_I2S_SUCCESS                    Call completed successfully.
 *
 * Sets the DMA-Mode for data transfers.  Transfers made in DMA-Mode will off-load the core processor.
 * Requires use of adi_I2S_GetDmaCompletion() to ascertain when DMA-Mode transfers have completed.
 *
 * DMA is a non-blocking use-case.  DMA in combination with Blocking-Mode is incompatible.
 * If you get result code #ADI_I2S_ERR_BAD_MODE_COMBO, disable either Blocking-Mode or DMA-Mode.
 *
 * @note      DMA mode is disabled by default.  This allows linker elimination to optimize DMA support
 *            code out of the I2S device driver, produceing a significient code size reduction.
 *            But if the application uses adi_I2S_SetDmaMode(), the linker can no longer eliminate
 *            references to the supporting DMA code.  Application programmers should avoid using
 *            adi_I2S_SetDmaMode() unless they really intend to operate the I2S controller in DMA mode.
 *            Even after using DMA and then disabeling it again, the DMA support code remains and
 *            the footprint reduction is lost.
 *
 * @sa        adi_I2S_GetDmaCompletion()
 * @sa        adi_I2S_SetBlockingMode()
 */
ADI_I2S_RESULT_TYPE adi_I2S_SetDmaMode (ADI_I2S_DEV_HANDLE const hDevice, const bool_t bFlag)
{
#ifdef ADI_DEBUG
    if (pI2S0_DevData != hDevice)
        return ADI_I2S_ERR_BAD_DEVICE_HANDLE;

    if (ADI_I2S_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_I2S_ERR_NOT_INITIALIZED;

    /* blocking mode with DMA disallowed... */
    if (hDevice->bBlockingMode & bFlag)
        return ADI_I2S_ERR_BAD_MODE_COMBO;

#if (ADI_I2S_CFG_ENABLE_DMA_SUPPORT == 0)
    if(bFlag)
      return ADI_I2S_ERR_BAD_API_FOR_MODE;

#endif /* ADI_I2S_CFG_ENABLE_DMA_SUPPORT */

#endif

#if (ADI_I2S_CFG_ENABLE_DMA_SUPPORT == 1)

    /* call whichever uninitialize helper is currently active */
    if (ADI_I2S_SUCCESS != hDevice->FunctionTable.pUnInitFcn(hDevice))
        return ADI_I2S_ERR_BAD_DMA_CONFIG;

    ADI_ENTER_CRITICAL_REGION();

    /* store flag */
    hDevice->bDmaMode = bFlag;

    /* remap function table */
    hDevice->FunctionTable.pInitFcn            = (bFlag) ? &dmaInit                  : &intInit;
    hDevice->FunctionTable.pUnInitFcn          = (bFlag) ? &dmaUnInit                : &intUnInit;
    hDevice->FunctionTable.pInitDescriptorsFcn = (bFlag) ? &dmaInitializeDescriptors : &intInitializeDescriptors;
    hDevice->FunctionTable.pDmaHandlerFcn      = (bFlag) ? &dmaCommonHandler         : &intCommonHandler;

    ADI_EXIT_CRITICAL_REGION();

    /* call the new initialize helper */
    if (ADI_I2S_SUCCESS != hDevice->FunctionTable.pInitFcn(hDevice))
        return ADI_I2S_ERR_BAD_DMA_CONFIG;

#endif /* ADI_I2S_CFG_ENABLE_DMA_SUPPORT */


    return ADI_I2S_SUCCESS;
}


/*!
 * @brief  Get the I2S device DMA-Mode transaction status.
 *
 * @param[in]      hDevice       Device handle obtained from adi_I2S_Init().
 *
 * @return Returns the completion state of the current DMA transfer.
 *                - 'true'      DMA is complete.
 *                - 'false'     DMA is incomplete.
 *
 * Polling call for application to query the DMA status for pending data transfers.  Current DMA status is
 * returned as a simple Boolean.
 *
 * This API is irrelevant in non-DMA mode.
 *
 * @sa        adi_I2S_SetDmaMode()
 */
bool_t adi_I2S_GetDmaCompletion (ADI_I2S_DEV_HANDLE const hDevice)
{
    return hDevice->bDmaComplete;
}



/*
    //////////////////////////////////////////////////////////////////////////////
    ///////////////////////   STATIC FUNCTIONS   /////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////
*/


static ADI_I2S_RESULT_TYPE commonInit(ADI_I2S_DEV_HANDLE const hDevice)
{
    /* always enable bus error capture */
    NVIC_EnableIRQ(DMA_ERR_IRQn);

    hDevice->pI2S = pADI_I2S;

    /* initialize core state variables */
    hDevice->initState              = ADI_I2S_STATE_UNINITIALIZED;
    hDevice->result                 = ADI_I2S_SUCCESS;
    hDevice->bDmaMode               = false;
    hDevice->bBlockingMode          = true;
    hDevice->pfCallback             = NULL;
    hDevice->pCBParam               = NULL;

    /* initialize function table to avoid dma references */
    hDevice->FunctionTable.pInitFcn            = &intInit;
    hDevice->FunctionTable.pUnInitFcn          = &intUnInit;
    hDevice->FunctionTable.pInitDescriptorsFcn = intInitializeDescriptors;
    hDevice->FunctionTable.pDmaHandlerFcn      = NULL;

#if (ADI_I2S_CFG_ENABLE_DMA_SUPPORT == 1)

    /* init simple DMA variables */
    hDevice->bDmaComplete           = false;
    hDevice->pDmaDescriptor         = &gDmaDescriptor;
    hDevice->nextDescrType          = ADI_DMA_CCD_PRIMARY;

#endif /* ADI_I2S_CFG_ENABLE_DMA_SUPPORT    */

    return ADI_I2S_SUCCESS;
}


static void configureNVIC(ADI_I2S_DEV_HANDLE const hDevice, bool_t bEnable)
{
    /* enable according to mode */
    if (bEnable) {
        if (hDevice->bDmaMode) {
          NVIC_EnableIRQ(DMA_I2S_IRQn);
        }

        NVIC_EnableIRQ(I2S_IRQn);
    } else {
        /* clear all I2S NVIC interrupts */
        NVIC_DisableIRQ(I2S_IRQn);
        NVIC_DisableIRQ(DMA_I2S_IRQn);

    }
}

// Configuration APIs

/*!
 * @brief  Configure the data width for the I2S device.
 *
 * @param[in]   hDevice    Device handle obtained from adi_I2S_Init().
 * @param[in]   format     Configures the data width of the I2S device.
 *
 * @return      Status
 *                - #ADI_I2S_ERR_BAD_DEVICE_HANDLE [D]  Invalid device handle parameter.
 *                - #ADI_I2S_ERR_NOT_INITIALIZED [D]    Device has not been previously configured for use.
 *                - #ADI_I2S_SUCCESS                    Call completed successfully.
 */
ADI_I2S_RESULT_TYPE adi_I2S_SetDataFormat         (ADI_I2S_DEV_HANDLE const hDevice, ADI_I2S_DATA_FORMAT_TYPE const format )
{
    uint16_t            i2sMMRreg;

#ifdef ADI_DEBUG
    if (pI2S0_DevData != hDevice)
        return ADI_I2S_ERR_BAD_DEVICE_HANDLE;

    if (ADI_I2S_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_I2S_ERR_NOT_INITIALIZED;
#endif

    /* Read the MMR. Clear the bit field. Write the bit field. Write back the read-modified-write value */
    i2sMMRreg = hDevice->pI2S->I2S_MODE1;
    i2sMMRreg &= ~BITM_I2S_I2S_MODE1_SDATA_FMT;
    i2sMMRreg |= (format << BITP_I2S_I2S_MODE1_SDATA_FMT);
    hDevice->pI2S->I2S_MODE1 = i2sMMRreg;

    return ADI_I2S_SUCCESS;
}


/*!
 * @brief  Configure the channel format for the I2S device.
 *
 * I2S Device can operate in I2S Stero, TDM mode or mono mode.
 *
 * @param[in]   hDevice    Device handle obtained from adi_I2S_Init().
 * @param[in]   format     Channel format to be configured. It can be one of the
 *                         values from the #ADI_I2S_CHANNEL_FORMAT_TYPE enumeration.
 *
 * @return      Status
 *                - #ADI_I2S_ERR_BAD_DEVICE_HANDLE [D]  Invalid device handle parameter.
 *                - #ADI_I2S_ERR_NOT_INITIALIZED [D]    Device has not been previously configured for use.
 *                - #ADI_I2S_SUCCESS                    Call completed successfully.
 */
ADI_I2S_RESULT_TYPE adi_I2S_SetChannelFormat      (ADI_I2S_DEV_HANDLE const hDevice, ADI_I2S_CHANNEL_FORMAT_TYPE const format)
{
    uint16_t            i2sMMRreg;

#ifdef ADI_DEBUG
    if (pI2S0_DevData != hDevice)
        return ADI_I2S_ERR_BAD_DEVICE_HANDLE;

    if (ADI_I2S_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_I2S_ERR_NOT_INITIALIZED;
#endif

    /* Read the MMR. Clear the bit field. Write the bit field. Write back the read-modified-write value */
    i2sMMRreg = hDevice->pI2S->I2S_MODE1;
    i2sMMRreg &= ~BITM_I2S_I2S_MODE1_SAI;
    i2sMMRreg |= (format << BITP_I2S_I2S_MODE1_SAI);
    hDevice->pI2S->I2S_MODE1 = i2sMMRreg;

    return ADI_I2S_SUCCESS;
}


/*!
 * @brief  Configures the frame rate for the i2s device.
 *
 * @param[in]   hDevice    Device handle obtained from adi_I2S_Init().
 * @param[in]   rate       Frame rate to be configured.
 *
 * @return      Status
 *                - #ADI_I2S_ERR_BAD_DEVICE_HANDLE [D]  Invalid device handle parameter.
 *                - #ADI_I2S_ERR_NOT_INITIALIZED [D]    Device has not been previously configured for use.
 *                - #ADI_I2S_SUCCESS                    Call completed successfully.
 */
ADI_I2S_RESULT_TYPE adi_I2S_FrameRate             (ADI_I2S_DEV_HANDLE const hDevice, ADI_I2S_FRAME_RATE_TYPE const rate)
{
  uint16_t            i2sMMRreg;

#ifdef ADI_DEBUG
    if (pI2S0_DevData != hDevice)
       return ADI_I2S_ERR_BAD_DEVICE_HANDLE;

    if (ADI_I2S_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_I2S_ERR_NOT_INITIALIZED;
#endif

    /* Read the MMR. Clear the bit field. Write the bit field. Write back the read-modified-write value */
    i2sMMRreg = hDevice->pI2S->I2S_MODE1;
    i2sMMRreg &= ~BITM_I2S_I2S_MODE1_FS;
    i2sMMRreg |= (rate << BITP_I2S_I2S_MODE1_FS);
    hDevice->pI2S->I2S_MODE1 = i2sMMRreg;

    return ADI_I2S_SUCCESS;
}


/*!
 * @brief  Configures the TDM slot width.
 *
 * When in TDM mode the slot width for each channel can be configured. The
 * device can be configured to be in TDM mode using the API #adi_I2S_SetChannelFormat.
 *
 * @param[in]   hDevice    Device handle obtained from adi_I2S_Init().
 * @param[in]   width      Slot width to be configured.
 *
 * @return      Status
 *                - #ADI_I2S_ERR_BAD_DEVICE_HANDLE [D]  Invalid device handle parameter.
 *                - #ADI_I2S_ERR_NOT_INITIALIZED [D]    Device has not been previously configured for use.
 *                - #ADI_I2S_SUCCESS                    Call completed successfully.
 *
 * @sa adi_I2S_SetChannelFormat()
 */
ADI_I2S_RESULT_TYPE adi_I2S_SlotWidth             (ADI_I2S_DEV_HANDLE const hDevice, ADI_I2S_SLOT_WIDTH_TYPE const width)
{
  uint16_t            i2sMMRreg;

#ifdef ADI_DEBUG
    if (pI2S0_DevData != hDevice)
        return ADI_I2S_ERR_BAD_DEVICE_HANDLE;

    if (ADI_I2S_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_I2S_ERR_NOT_INITIALIZED;
#endif

    /* Read the MMR. Clear the bit field. Write the bit field. Write back the read-modified-write value */
    i2sMMRreg = hDevice->pI2S->I2S_MODE1;
    i2sMMRreg &= ~BITM_I2S_I2S_MODE1_SLOT_WIDTH;
    i2sMMRreg |= (width << BITP_I2S_I2S_MODE1_SLOT_WIDTH);
    hDevice->pI2S->I2S_MODE1 = i2sMMRreg;

    return ADI_I2S_SUCCESS;

}


/*!
 * @brief  Configures the data width.
 *
 * Configuring this trims the data stream to fit in 16 bits per channel. This
 * will not have an effect on data streams that are already 16 bits or fewer.
 *
 * @param[in]   hDevice    Device handle obtained from adi_I2S_Init().
 * @param[in]   width      Data width to be configured.
 *
 * @return      Status
 *                - #ADI_I2S_ERR_BAD_DEVICE_HANDLE [D]  Invalid device handle parameter.
 *                - #ADI_I2S_ERR_NOT_INITIALIZED [D]    Device has not been previously configured for use.
 *                - #ADI_I2S_SUCCESS                    Call completed successfully.
 *
 * @sa adi_I2S_SlotWidth()
 */
ADI_I2S_RESULT_TYPE adi_I2S_DataWidth             (ADI_I2S_DEV_HANDLE const hDevice, ADI_I2S_DATA_WIDTH_TYPE const width)
{
    uint16_t            i2sMMRreg;

#ifdef ADI_DEBUG
    if (pI2S0_DevData != hDevice)
       return ADI_I2S_ERR_BAD_DEVICE_HANDLE;

    if (ADI_I2S_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_I2S_ERR_NOT_INITIALIZED;
#endif

    /* Read the MMR. Clear the bit field. Write the bit field. Write back the read-modified-write value */
    i2sMMRreg = hDevice->pI2S->I2S_MODE1;
    i2sMMRreg &= ~BITM_I2S_I2S_MODE1_DATA_WIDTH;
    i2sMMRreg |= (width << BITP_I2S_I2S_MODE1_DATA_WIDTH);
    hDevice->pI2S->I2S_MODE1 = i2sMMRreg;

    /* Update the flag based on the setting */
    hDevice->b24bitMode      = (width == ADI_I2S_DATA_WIDTH_24_BIT) ? true : false;

    return ADI_I2S_SUCCESS;
}

/*!
 * @brief  Sets the LR clock mode in TDM output mode.
 *
 * In TDM mode the LR clock can be 50% duty cycle or it can be a single cycle pulse.
 * The device can be configured to be in TDM mode using the API #adi_I2S_SetChannelFormat
 *
 * @param[in]   hDevice    Device handle obtained from adi_I2S_Init().
 * @param[in]   mode       LR Clock mode to be configured.
 *
 * @return      Status
 *                - #ADI_I2S_ERR_BAD_DEVICE_HANDLE [D]  Invalid device handle parameter.
 *                - #ADI_I2S_ERR_NOT_INITIALIZED [D]    Device has not been previously configured for use.
 *                - #ADI_I2S_SUCCESS                    Call completed successfully.
 *
 * @sa adi_I2S_SetChannelFormat()
 */
ADI_I2S_RESULT_TYPE adi_I2S_SetLRCLKMode( ADI_I2S_DEV_HANDLE const hDevice, ADI_I2S_LRCLK_MODE_TYPE const mode)
{
    uint16_t            i2sMMRreg;

#ifdef ADI_DEBUG
    if (pI2S0_DevData != hDevice)
       return ADI_I2S_ERR_BAD_DEVICE_HANDLE;

    if (ADI_I2S_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_I2S_ERR_NOT_INITIALIZED;
#endif

    /* Read the MMR. Clear the bit field. Write the bit field. Write back the read-modified-write value */
    i2sMMRreg = hDevice->pI2S->I2S_MODE1;
    i2sMMRreg &= ~BITM_I2S_I2S_MODE1_LR_MODE;
    i2sMMRreg |= (mode << BITP_I2S_I2S_MODE1_LR_MODE);
    hDevice->pI2S->I2S_MODE1 = i2sMMRreg;

    return ADI_I2S_SUCCESS;
}


/*!
 * @brief  Sets the endian format of the data.
 *
 * @param[in]   hDevice    Device handle obtained from adi_I2S_Init().
 * @param[in]   format     Endian format to be configured.
 *
 * @return      Status
 *                - #ADI_I2S_ERR_BAD_DEVICE_HANDLE [D]  Invalid device handle parameter.
 *                - #ADI_I2S_ERR_NOT_INITIALIZED [D]    Device has not been previously configured for use.
 *                - #ADI_I2S_SUCCESS                    Call completed successfully.
 */
ADI_I2S_RESULT_TYPE adi_I2S_SetEndianFormat       (ADI_I2S_DEV_HANDLE const hDevice, ADI_I2S_ENDIAN_TYPE const format )
{
  uint16_t            i2sMMRreg;

#ifdef ADI_DEBUG
    if (pI2S0_DevData != hDevice)
       return ADI_I2S_ERR_BAD_DEVICE_HANDLE;

    if (ADI_I2S_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_I2S_ERR_NOT_INITIALIZED;
#endif

    /* Read the MMR. Clear the bit field. Write the bit field. Write back the read-modified-write value */
    i2sMMRreg = hDevice->pI2S->I2S_MODE1;
    i2sMMRreg &= ~BITM_I2S_I2S_MODE1_SAI_MSB;
    i2sMMRreg |= (format << BITP_I2S_I2S_MODE1_SAI_MSB);
    hDevice->pI2S->I2S_MODE1 = i2sMMRreg;

    return ADI_I2S_SUCCESS;
}


/*!
 * @brief  Sets the bit clock rate.
 *
 * It can be configured to 16 or 32 bit clocks per channel.
 *
 * @param[in]   hDevice    Device handle obtained from adi_I2S_Init().
 * @param[in]   rate       Bit clock rate to be configured.
 *
 * @return      Status
 *                - #ADI_I2S_ERR_BAD_DEVICE_HANDLE [D]  Invalid device handle parameter.
 *                - #ADI_I2S_ERR_NOT_INITIALIZED [D]    Device has not been previously configured for use.
 *                - #ADI_I2S_SUCCESS                    Call completed successfully.
 */
ADI_I2S_RESULT_TYPE adi_I2S_SetBLCKRate           (ADI_I2S_DEV_HANDLE const hDevice, ADI_I2S_BCLK_RATE_TYPE const rate)
{
  uint16_t            i2sMMRreg;

#ifdef ADI_DEBUG
    if (pI2S0_DevData != hDevice)
       return ADI_I2S_ERR_BAD_DEVICE_HANDLE;

    if (ADI_I2S_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_I2S_ERR_NOT_INITIALIZED;
#endif

    /* Read the MMR. Clear the bit field. Write the bit field. Write back the read-modified-write value */
    i2sMMRreg = hDevice->pI2S->I2S_MODE1;
    i2sMMRreg &= ~BITM_I2S_I2S_MODE1_BCLK_RATE;
    i2sMMRreg |= (rate << BITP_I2S_I2S_MODE1_BCLK_RATE);
    hDevice->pI2S->I2S_MODE1 = i2sMMRreg;

    return ADI_I2S_SUCCESS;
}

/*!
 * @brief  Configures I2s device to be master or slave.
 *
 * In Master mode the device uses external bit clock and LR clock. In slave
 * mode they are generated internally.
 *
 * @param[in]   hDevice    Device handle obtained from adi_I2S_Init().
 * @param[in]   ms         Master or slave mode.
 *
 * @return      Status
 *                - #ADI_I2S_ERR_BAD_DEVICE_HANDLE [D]  Invalid device handle parameter.
 *                - #ADI_I2S_ERR_NOT_INITIALIZED [D]    Device has not been previously configured for use.
 *                - #ADI_I2S_SUCCESS                    Call completed successfully.
 */
ADI_I2S_RESULT_TYPE adi_I2S_SetMasterSlave        (ADI_I2S_DEV_HANDLE const hDevice, ADI_I2S_MASTER_SLAVE_SELECT_TYPE const ms)
{
  uint16_t            i2sMMRreg;

#ifdef ADI_DEBUG
    if (pI2S0_DevData != hDevice)
       return ADI_I2S_ERR_BAD_DEVICE_HANDLE;

    if (ADI_I2S_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_I2S_ERR_NOT_INITIALIZED;
#endif

    /* Read the MMR. Clear the bit field. Write the bit field. Write back the read-modified-write value */
    i2sMMRreg = hDevice->pI2S->I2S_MODE1;
    i2sMMRreg &= ~BITM_I2S_I2S_MODE1_SAI_MS;
    i2sMMRreg |= (ms << BITP_I2S_I2S_MODE1_SAI_MS);
    hDevice->pI2S->I2S_MODE1 = i2sMMRreg;

    return ADI_I2S_SUCCESS;
}


/*!
 * @brief  Configures unused bits to be driven or tri stated (Hi-Z).
 *
 * The device can be configured to drive the data line for unused bits in the stream
 * or to tri state the data line.
 *
 * @param[in]   hDevice    Device handle obtained from adi_I2S_Init().
 * @param[in]   bFlag      Flag which indicates if the data line to be tri-stated
 *                         or driven for the unused bits.
 *                         'true'  - Don't drive unused bits (default)
 *                         'false' - Drive unused bits.
 *
 * @return      Status
 *                - #ADI_I2S_ERR_BAD_DEVICE_HANDLE [D]  Invalid device handle parameter.
 *                - #ADI_I2S_ERR_NOT_INITIALIZED [D]    Device has not been previously configured for use.
 *                - #ADI_I2S_SUCCESS                    Call completed successfully.
 */
ADI_I2S_RESULT_TYPE adi_I2S_SetDriveUnusedBits        (ADI_I2S_DEV_HANDLE const hDevice, const bool_t bFlag)
{
  uint16_t            i2sMMRreg;

#ifdef ADI_DEBUG
    if (pI2S0_DevData != hDevice)
       return ADI_I2S_ERR_BAD_DEVICE_HANDLE;

    if (ADI_I2S_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_I2S_ERR_NOT_INITIALIZED;
#endif

    /* Read the MMR. Clear the bit field. Write the bit field. Write back the read-modified-write value */
    i2sMMRreg = hDevice->pI2S->I2S_MODE1;
    i2sMMRreg &= ~BITM_I2S_I2S_MODE1_SAI_MS;
    i2sMMRreg |= ((int)bFlag << BITP_I2S_I2S_MODE1_DRV_HIZ);
    hDevice->pI2S->I2S_MODE1 = i2sMMRreg;

    return ADI_I2S_SUCCESS;
}


/*!
 * @brief  Configures LR clock polarity.
 *
 *
 * @param[in]   hDevice    Device handle obtained from adi_I2S_Init().
 * @param[in]   polarity   Polarity to be configured.
 *
 * @return      Status
 *                - #ADI_I2S_ERR_BAD_DEVICE_HANDLE [D]  Invalid device handle parameter.
 *                - #ADI_I2S_ERR_NOT_INITIALIZED [D]    Device has not been previously configured for use.
 *                - #ADI_I2S_SUCCESS                    Call completed successfully.
 */
ADI_I2S_RESULT_TYPE adi_I2S_SetLRCLKPolarity      (ADI_I2S_DEV_HANDLE const hDevice, ADI_I2S_LRCLK_POLARITY_TYPE const polarity)
{
    uint16_t            i2sMMRreg;

#ifdef ADI_DEBUG
    if (pI2S0_DevData != hDevice)
       return ADI_I2S_ERR_BAD_DEVICE_HANDLE;

    if (ADI_I2S_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_I2S_ERR_NOT_INITIALIZED;
#endif

    /* Read the MMR. Clear the bit field. Write the bit field. Write back the read-modified-write value */
    i2sMMRreg = hDevice->pI2S->I2S_MODE2;
    i2sMMRreg &= ~BITM_I2S_I2S_MODE2_LR_POL;
    i2sMMRreg |= (polarity << BITP_I2S_I2S_MODE2_LR_POL);
    hDevice->pI2S->I2S_MODE2 = i2sMMRreg;

    return ADI_I2S_SUCCESS;

}


/*!
 * @brief  Configures the bit clock sampling edge.
 *
 * @param[in]   hDevice    Device handle obtained from adi_I2S_Init().
 * @param[in]   edge       Smpling edge to be configured.
 *
 * @return      Status
 *                - #ADI_I2S_ERR_BAD_DEVICE_HANDLE [D]  Invalid device handle parameter.
 *                - #ADI_I2S_ERR_NOT_INITIALIZED [D]    Device has not been previously configured for use.
 *                - #ADI_I2S_SUCCESS                    Call completed successfully.
 */
ADI_I2S_RESULT_TYPE adi_I2S_SetBLCKEdge           (ADI_I2S_DEV_HANDLE const hDevice, ADI_I2S_BLCK_EDGE_TYPE const edge)
{
  uint16_t            i2sMMRreg;

#ifdef ADI_DEBUG
    if (pI2S0_DevData != hDevice)
       return ADI_I2S_ERR_BAD_DEVICE_HANDLE;

    if (ADI_I2S_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_I2S_ERR_NOT_INITIALIZED;
#endif

    /* Read the MMR. Clear the bit field. Write the bit field. Write back the read-modified-write value */
    i2sMMRreg = hDevice->pI2S->I2S_MODE2;
    i2sMMRreg &= ~BITM_I2S_I2S_MODE2_BCLK_EDGE;
    i2sMMRreg |= (edge << BITP_I2S_I2S_MODE2_BCLK_EDGE);
    hDevice->pI2S->I2S_MODE2 = i2sMMRreg;

    return ADI_I2S_SUCCESS;
}


/*!
 * @brief  Configures Channel 2 to be driven or not driven on data line (SDATA).
 *
 * @param[in]   hDevice    Device handle obtained from adi_I2S_Init().
 * @param[in]   ch2        Channel 2 to be driven or not driven.
 *
 * @return      Status
 *                - #ADI_I2S_ERR_BAD_DEVICE_HANDLE [D]  Invalid device handle parameter.
 *                - #ADI_I2S_ERR_NOT_INITIALIZED [D]    Device has not been previously configured for use.
 *                - #ADI_I2S_SUCCESS                    Call completed successfully.
 */
ADI_I2S_RESULT_TYPE adi_I2S_SetChannel_2_Drive    (ADI_I2S_DEV_HANDLE const hDevice, ADI_I2S_CHANNEL_2_DRIVE_TYPE const ch2)
{
  uint16_t            i2sMMRreg;

#ifdef ADI_DEBUG
    if (pI2S0_DevData != hDevice)
       return ADI_I2S_ERR_BAD_DEVICE_HANDLE;

    if (ADI_I2S_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_I2S_ERR_NOT_INITIALIZED;
#endif

    /* Read the MMR. Clear the bit field. Write the bit field. Write back the read-modified-write value */
    i2sMMRreg = hDevice->pI2S->I2S_MODE2;
    i2sMMRreg &= ~BITM_I2S_I2S_MODE2_DRV_CH2;
    i2sMMRreg |= (ch2 << BITP_I2S_I2S_MODE2_DRV_CH2);
    hDevice->pI2S->I2S_MODE2 = i2sMMRreg;

    return ADI_I2S_SUCCESS;
}

/*!
 * @brief  Configures Channel 1 to be driven or not driven on data line (SDATA).
 *
 * @param[in]   hDevice    Device handle obtained from adi_I2S_Init().
 * @param[in]   ch1        Channel 1 to be driven or not driven.
 *
 * @return      Status
 *                - #ADI_I2S_ERR_BAD_DEVICE_HANDLE [D]  Invalid device handle parameter.
 *                - #ADI_I2S_ERR_NOT_INITIALIZED [D]    Device has not been previously configured for use.
 *                - #ADI_I2S_SUCCESS                    Call completed successfully.
 */
ADI_I2S_RESULT_TYPE adi_I2S_SetChannel_1_Drive    (ADI_I2S_DEV_HANDLE const hDevice, ADI_I2S_CHANNEL_1_DRIVE_TYPE const ch1)
{
  uint16_t            i2sMMRreg;

#ifdef ADI_DEBUG
    if (pI2S0_DevData != hDevice)
       return ADI_I2S_ERR_BAD_DEVICE_HANDLE;

    if (ADI_I2S_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_I2S_ERR_NOT_INITIALIZED;
#endif

    /* Read the MMR. Clear the bit field. Write the bit field. Write back the read-modified-write value */
    i2sMMRreg = hDevice->pI2S->I2S_MODE2;
    i2sMMRreg &= ~BITM_I2S_I2S_MODE2_DRV_CH1;
    i2sMMRreg |= (ch1 << BITP_I2S_I2S_MODE2_DRV_CH1);
    hDevice->pI2S->I2S_MODE2 = i2sMMRreg;

    return ADI_I2S_SUCCESS;
}

/*!
 * @brief  Maps the Channel 2 output to the given output slot on the data line.
 *
 * Application must make sure that both channel 1 and channel 2 are not mapped
 * to the same output slot.
 *
 * @param[in]   hDevice    Device handle obtained from adi_I2S_Init().
 * @param[in]   map        Channel 2 output slot selection.
 *
 * @return      Status
 *                - #ADI_I2S_ERR_BAD_DEVICE_HANDLE [D]  Invalid device handle parameter.
 *                - #ADI_I2S_ERR_NOT_INITIALIZED [D]    Device has not been previously configured for use.
 *                - #ADI_I2S_SUCCESS                    Call completed successfully.
 *
 * @sa adi_I2S_SetChannel_1_OutputMap()
 */
ADI_I2S_RESULT_TYPE adi_I2S_SetChannel_2_OutputMap   (ADI_I2S_DEV_HANDLE const hDevice, ADI_I2S_CHANNEL_MAP_TYPE const map)
{
  uint16_t            i2sMMRreg;

#ifdef ADI_DEBUG
    if (pI2S0_DevData != hDevice)
       return ADI_I2S_ERR_BAD_DEVICE_HANDLE;

    if (ADI_I2S_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_I2S_ERR_NOT_INITIALIZED;
#endif

    /* Read the MMR. Clear the bit field. Write the bit field. Write back the read-modified-write value */
    i2sMMRreg = hDevice->pI2S->I2S_MODE2;
    i2sMMRreg &= ~BITM_I2S_I2S_MODE2_CMAP_C2;
    i2sMMRreg |= (map << BITP_I2S_I2S_MODE2_CMAP_C2);
    hDevice->pI2S->I2S_MODE2 = i2sMMRreg;

    return ADI_I2S_SUCCESS;
}


/*!
 * @brief  Maps the Channel 1 output to the given output slot on the data line.
 *
 * Application must make sure that both channel 1 and channel 2 are not mapped
 * to the same output slot.
 *
 * @param[in]   hDevice    Device handle obtained from adi_I2S_Init().
 * @param[in]   map        Channel 1 output slot selection.
 *
 * @return      Status
 *                - #ADI_I2S_ERR_BAD_DEVICE_HANDLE [D]  Invalid device handle parameter.
 *                - #ADI_I2S_ERR_NOT_INITIALIZED [D]    Device has not been previously configured for use.
 *                - #ADI_I2S_SUCCESS                    Call completed successfully.
 *
 * @sa adi_I2S_SetChannel_2_OutputMap()
 */
ADI_I2S_RESULT_TYPE adi_I2S_SetChannel_1_OutputMap   (ADI_I2S_DEV_HANDLE const hDevice, ADI_I2S_CHANNEL_MAP_TYPE const map)
{
  uint16_t            i2sMMRreg;

#ifdef ADI_DEBUG
    if (pI2S0_DevData != hDevice)
       return ADI_I2S_ERR_BAD_DEVICE_HANDLE;

    if (ADI_I2S_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_I2S_ERR_NOT_INITIALIZED;
#endif

    /* Read the MMR. Clear the bit field. Write the bit field. Write back the read-modified-write value */
    i2sMMRreg = hDevice->pI2S->I2S_MODE2;
    i2sMMRreg &= ~BITM_I2S_I2S_MODE2_CMAP_C1;
    i2sMMRreg |= (map << BITP_I2S_I2S_MODE2_CMAP_C1);
    hDevice->pI2S->I2S_MODE2 = i2sMMRreg;

    return ADI_I2S_SUCCESS;
}


/*!
 * @brief  Resets the FIFO for serial output.
 *
 * @param[in]   hDevice    Device handle obtained from adi_I2S_Init().
 * @param[in]   fifoReset  Indicates whether to reset the FIFO or normal operation.
 *
 * @return      Status
 *                - #ADI_I2S_ERR_BAD_DEVICE_HANDLE [D]  Invalid device handle parameter.
 *                - #ADI_I2S_ERR_NOT_INITIALIZED [D]    Device has not been previously configured for use.
 *                - #ADI_I2S_SUCCESS                    Call completed successfully.
 */
ADI_I2S_RESULT_TYPE adi_I2S_SetFifoReset(ADI_I2S_DEV_HANDLE const hDevice,  ADI_I2S_FIFO_RESET_TYPE const fifoReset )
{
  uint16_t            i2sMMRreg;

#ifdef ADI_DEBUG
    if (ADI_I2S_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_I2S_ERR_NOT_INITIALIZED;

    if (ADI_I2S_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_I2S_ERR_NOT_INITIALIZED;
#endif

    /* Read the MMR. Clear the bit field. Write the bit field. Write back the read-modified-write value */
    i2sMMRreg = hDevice->pI2S->I2S_CFG1;
    i2sMMRreg &= ~BITM_I2S_I2S_CFG1_FIFO_RST;
    i2sMMRreg |=  (fifoReset << BITP_I2S_I2S_CFG1_FIFO_RST);
    hDevice->pI2S->I2S_CFG1 = i2sMMRreg;

    return ADI_I2S_SUCCESS;
}


/*!
 * @brief  Resets the auto increment address.
 *
 * @param[in]   hDevice    Device handle obtained from adi_I2S_Init().
 * @param[in]   incrReset  Indicates whether to reset the auto-increment address.
 *
 * @return      Status
 *                - #ADI_I2S_ERR_BAD_DEVICE_HANDLE [D]  Invalid device handle parameter.
 *                - #ADI_I2S_ERR_NOT_INITIALIZED [D]    Device has not been previously configured for use.
 *                - #ADI_I2S_SUCCESS                    Call completed successfully.
 */
ADI_I2S_RESULT_TYPE adi_I2S_SetIncrReset(ADI_I2S_DEV_HANDLE const hDevice,  ADI_I2S_INCR_RESET_TYPE const incrReset)
{
    uint16_t            i2sMMRreg;

#ifdef ADI_DEBUG
  if (ADI_I2S_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_I2S_ERR_NOT_INITIALIZED;

  if (ADI_I2S_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_I2S_ERR_NOT_INITIALIZED;
#endif

    /* Read the MMR. Clear the bit field. Write the bit field. Write back the read-modified-write value */
    i2sMMRreg = hDevice->pI2S->I2S_CFG1;
    i2sMMRreg &= ~BITM_I2S_I2S_CFG1_INCR_RST;
    i2sMMRreg |=  (incrReset << BITP_I2S_I2S_CFG1_INCR_RST);
    hDevice->pI2S->I2S_CFG1 = i2sMMRreg;

    return ADI_I2S_SUCCESS;
}


/*!
 * @brief  Enable or disable interrupts for serial data request.
 *
 * Enable data request interrupts to core from serial port. When enabled, an
 * interrupt will fire each time a frame starts on the I2S bus.
 *
 * @param[in]   hDevice    Device handle obtained from adi_I2S_Init().
 * @param[in]   bFlag      Flag which indicates whether to enable or disable the interrupts
 *                         for serial data request.
 *                         'true'  - Enable the data request interrupt.
 *                         'false' - Disable the data request interrupt.
 *
 * @return      Status
 *                - #ADI_I2S_ERR_BAD_DEVICE_HANDLE [D]  Invalid device handle parameter.
 *                - #ADI_I2S_ERR_NOT_INITIALIZED [D]    Device has not been previously configured for use.
 *                - #ADI_I2S_SUCCESS                    Call completed successfully.
 */
ADI_I2S_RESULT_TYPE adi_I2S_SetReqEnable           (ADI_I2S_DEV_HANDLE const hDevice,  const bool_t bFlag)
{
    uint16_t            i2sMMRreg;

#ifdef ADI_DEBUG
    if (ADI_I2S_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_I2S_ERR_NOT_INITIALIZED;

    if (ADI_I2S_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_I2S_ERR_NOT_INITIALIZED;
#endif

    /* Read the MMR. Clear the bit field. Write the bit field. Write back the read-modified-write value */
    i2sMMRreg = hDevice->pI2S->I2S_CFG1;
    i2sMMRreg &= ~BITM_I2S_I2S_CFG1_REQ_EN;
    i2sMMRreg |=  ((int)bFlag << BITP_I2S_I2S_CFG1_REQ_EN);
    hDevice->pI2S->I2S_CFG1 = i2sMMRreg;

    return ADI_I2S_SUCCESS;
}


/*!
 * @brief  Enable or disable FIFO status interrupts.
 *
 * Enable FIFO status reporting via core interrupt. When enabled, an interrupt
 * will fire if serial output FIFO becomes full or empty.
 *
 * @param[in]   hDevice    Device handle obtained from adi_I2S_Init().
 * @param[in]   bFlag      Flag which indicates whether to enable or disable the interrupts
 *                         for FIFO status.
 *                         'true'  - Enable the FIFO status interrupt.
 *                         'false' - Disable the FIFO status interrupt.
 *
 * @return      Status
 *                - #ADI_I2S_ERR_BAD_DEVICE_HANDLE [D]  Invalid device handle parameter.
 *                - #ADI_I2S_ERR_NOT_INITIALIZED [D]    Device has not been previously configured for use.
 *                - #ADI_I2S_SUCCESS                    Call completed successfully.
 */
ADI_I2S_RESULT_TYPE adi_I2S_SetStatEnable          (ADI_I2S_DEV_HANDLE const hDevice,  const bool_t bFlag)
{
    uint16_t            i2sMMRreg;

#ifdef ADI_DEBUG
    if (ADI_I2S_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_I2S_ERR_NOT_INITIALIZED;

    if (ADI_I2S_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_I2S_ERR_NOT_INITIALIZED;
#endif

    /* Read the MMR. Clear the bit field. Write the bit field. Write back the read-modified-write value */
    i2sMMRreg = hDevice->pI2S->I2S_CFG1;
    i2sMMRreg &= ~BITM_I2S_I2S_CFG1_STAT_EN;
    i2sMMRreg |=  ((int)bFlag << BITP_I2S_I2S_CFG1_STAT_EN);
    hDevice->pI2S->I2S_CFG1 = i2sMMRreg;

    return ADI_I2S_SUCCESS;
}


/*!
 * @brief  Enable or disable auto-increment addressing.
 *
 * Enable audo-increment addressing. A single date address can be accessed and the
 * internal I2S data address will automatically increment circularly with each
 * subsequent access of that same "base" register.
 *
 * @param[in]   hDevice    Device handle obtained from adi_I2S_Init().
 * @param[in]   bFlag      Flag which indicates whether to enable or disable auto-increment
 *                         'true'  - Enable auto-increment.
 *                         'false' - Disable auto-increment (standard addressing).
 *
 * @return      Status
 *                - #ADI_I2S_ERR_BAD_DEVICE_HANDLE [D]  Invalid device handle parameter.
 *                - #ADI_I2S_ERR_NOT_INITIALIZED [D]    Device has not been previously configured for use.
 *                - #ADI_I2S_SUCCESS                    Call completed successfully.
 */
ADI_I2S_RESULT_TYPE adi_I2S_SetAutoIncrEnable      (ADI_I2S_DEV_HANDLE const hDevice,  const bool_t bFlag)
{
    uint16_t            i2sMMRreg;

#ifdef ADI_DEBUG
    if (ADI_I2S_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_I2S_ERR_NOT_INITIALIZED;

    if (ADI_I2S_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_I2S_ERR_NOT_INITIALIZED;
#endif

    /* Read the MMR. Clear the bit field. Write the bit field. Write back the read-modified-write value */
    i2sMMRreg = hDevice->pI2S->I2S_CFG1;
    i2sMMRreg &= ~BITM_I2S_I2S_CFG1_AUTO_INCR;
    i2sMMRreg |=  ((int)bFlag << BITP_I2S_I2S_CFG1_AUTO_INCR);
    hDevice->pI2S->I2S_CFG1 = i2sMMRreg;

    return ADI_I2S_SUCCESS;
}


/*!
 * @brief  Configures the transfer mode.
 *
 * The transfer mode can be 24-bit, 16-bit or 8-bit
 *
 * @param[in]   hDevice    Device handle obtained from adi_I2S_Init().
 * @param[in]   tm         Transfer mode to be configured.
 *
 * @return      Status
 *                - #ADI_I2S_ERR_BAD_DEVICE_HANDLE [D]  Invalid device handle parameter.
 *                - #ADI_I2S_ERR_NOT_INITIALIZED [D]    Device has not been previously configured for use.
 *                - #ADI_I2S_SUCCESS                    Call completed successfully.
 */
ADI_I2S_RESULT_TYPE adi_I2S_SetTranMode            (ADI_I2S_DEV_HANDLE const hDevice,  ADI_I2S_TRAN_MODE_TYPE const tm)
{
    uint16_t            i2sMMRreg;

#ifdef ADI_DEBUG
    if (ADI_I2S_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_I2S_ERR_NOT_INITIALIZED;

    if (ADI_I2S_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_I2S_ERR_NOT_INITIALIZED;
#endif

    /* Read the MMR. Clear the bit field. Write the bit field. Write back the read-modified-write value */
    i2sMMRreg = hDevice->pI2S->I2S_CFG1;
    i2sMMRreg &= ~BITM_I2S_I2S_CFG1_TRAN_MODE;
    i2sMMRreg |=  (tm << BITP_I2S_I2S_CFG1_TRAN_MODE);
    hDevice->pI2S->I2S_CFG1 = i2sMMRreg;

    return ADI_I2S_SUCCESS;
}


/*!
 * @brief  Enable or disable channel 2 transfer over the APB bus.
 *
 *
 * @param[in]   hDevice    Device handle obtained from adi_I2S_Init().
 * @param[in]   bFlag      Flag which indicates whether to enable or disable the
 *                         channel 2 data to be transferred over APB bus.
 *                         'true'  - Enable the transfer.
 *                         'false' - Disable the transfer (default).
 *
 * @return      Status
 *                - #ADI_I2S_ERR_BAD_DEVICE_HANDLE [D]  Invalid device handle parameter.
 *                - #ADI_I2S_ERR_NOT_INITIALIZED [D]    Device has not been previously configured for use.
 *                - #ADI_I2S_SUCCESS                    Call completed successfully.
 *
 * @sa adi_I2S_SetChanSel_1_Enable()
 */
ADI_I2S_RESULT_TYPE adi_I2S_SetChanSel_2_Enable      (ADI_I2S_DEV_HANDLE const hDevice,  const bool_t bFlag)
{
  uint16_t            i2sMMRreg;

#ifdef ADI_DEBUG
    if (ADI_I2S_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_I2S_ERR_NOT_INITIALIZED;

    if (ADI_I2S_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_I2S_ERR_NOT_INITIALIZED;
#endif

    /* Read the MMR. Clear the bit field. Write the bit field. Write back the read-modified-write value */
    i2sMMRreg = hDevice->pI2S->I2S_CFG1;
    i2sMMRreg &= ~BITM_I2S_I2S_CFG1_CHAN_SEL2;
    i2sMMRreg |=  ((int)bFlag << BITP_I2S_I2S_CFG1_CHAN_SEL2);
    hDevice->pI2S->I2S_CFG1 = i2sMMRreg;

    return ADI_I2S_SUCCESS;
}


/*!
 * @brief  Enable or disable channel 1 transfer over the APB bus.
 *
 *
 * @param[in]   hDevice    Device handle obtained from adi_I2S_Init().
 * @param[in]   bFlag      Flag which indicates whether to enable or disable the
 *                         channel 1 data to be transferred over APB bus.
 *                         'true'  - Enable the transfer.
 *                         'false' - Disable the transfer (default).
 *
 * @return      Status
 *                - #ADI_I2S_ERR_BAD_DEVICE_HANDLE [D]  Invalid device handle parameter.
 *                - #ADI_I2S_ERR_NOT_INITIALIZED [D]    Device has not been previously configured for use.
 *                - #ADI_I2S_SUCCESS                    Call completed successfully.
 *
 * @sa adi_I2S_SetChanSel_2_Enable()
 */
ADI_I2S_RESULT_TYPE adi_I2S_SetChanSel_1_Enable      (ADI_I2S_DEV_HANDLE const hDevice,  const bool_t bFlag)
{
  uint16_t            i2sMMRreg;

#ifdef ADI_DEBUG
    if (ADI_I2S_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_I2S_ERR_NOT_INITIALIZED;

    if (ADI_I2S_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_I2S_ERR_NOT_INITIALIZED;
#endif

    /* Read the MMR. Clear the bit field. Write the bit field. Write back the read-modified-write value */
    i2sMMRreg = hDevice->pI2S->I2S_CFG1;
    i2sMMRreg &= ~BITM_I2S_I2S_CFG1_CHAN_SEL1;
    i2sMMRreg |=  ((int)bFlag << BITP_I2S_I2S_CFG1_CHAN_SEL1);
    hDevice->pI2S->I2S_CFG1 = i2sMMRreg;

    return ADI_I2S_SUCCESS;
}


/*!
 * @brief  Register or unregister the callback.
 *
 * @param [in]   hDevice        Device handle obtained from adi_I2S_Init().
 * @param [in]   pfCallback     Pointer to the callback function. Can be passed as NULL to unregister the
 *                              previously registered callback.
 * @param [in]   pCBParam       Callback parameter which will be passed back to the application when the
 *                              callback is called.
 *
 * @return      Status
 *                - #ADI_I2S_ERR_BAD_DEVICE_HANDLE [D]  Invalid device handle parameter.
 *                - #ADI_I2S_ERR_NOT_INITIALIZED   [D]  Device has not been previously configured for use.
 *                - #ADI_I2S_SUCCESS                    Call completed successfully.
 */
ADI_I2S_RESULT_TYPE adi_I2S_RegisterCallback      (ADI_I2S_DEV_HANDLE const hDevice,  ADI_CALLBACK const pfCallback, void *const pCBParam )
{
#ifdef ADI_DEBUG
    if (ADI_I2S_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_I2S_ERR_NOT_INITIALIZED;

    if (ADI_I2S_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_I2S_ERR_NOT_INITIALIZED;
#endif

    /* Save the application provided callback and callback parameters */
    hDevice->pfCallback = pfCallback;
    hDevice->pCBParam   = pCBParam;

    return ADI_I2S_SUCCESS;
}

/*!
 * @brief  Enable or disable the I2S device.
 *
 * @param[in]   hDevice    Device handle obtained from adi_I2S_Init().
 * @param[in]   bFlag      Flag which indicates whether to enable or disable the
 *                         I2S device.
 *                         'true'  - Enable the device.
 *                         'false' - Disable the device (default).
 *
 * @return      Status
 *                - #ADI_I2S_ERR_BAD_DEVICE_HANDLE [D]  Invalid device handle parameter.
 *                - #ADI_I2S_ERR_NOT_INITIALIZED [D]    Device has not been previously configured for use.
 *                - #ADI_I2S_SUCCESS                    Call completed successfully.
 */
ADI_I2S_RESULT_TYPE adi_I2S_Enable                 (ADI_I2S_DEV_HANDLE const hDevice,  const bool_t bFlag )
{
  uint16_t            i2sMMRreg;

#ifdef ADI_DEBUG
    if (ADI_I2S_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_I2S_ERR_NOT_INITIALIZED;

    if (ADI_I2S_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_I2S_ERR_NOT_INITIALIZED;
#endif

    /* Fill the I2S Data pointers array based on the number of channels and data witdth */
    if((hDevice->bDmaMode == false) && (bFlag == true))
    {
          hDevice->NumChnls = 0;

          /* Check if left channel is enabled */
          if((hDevice->pI2S->I2S_MODE2 & BITM_I2S_I2S_MODE2_DRV_CH1) != 0)
          {
              /* In 24-bit mode the I2S_OUT1L should be filled with LSBs of the audio sample (bit 7-0), in 16 bit
                 mode, this register should not be filled */
              if(hDevice->b24bitMode == true)
                hDevice->DataPtrs[hDevice->NumChnls++] = &hDevice->pI2S->I2S_OUT1L;

              /* In 24-bit mode bits 23-8 or the sample are stored in I2S_OUT1H, in 16-bit mode bits 15-0 are stored */
              hDevice->DataPtrs[hDevice->NumChnls++] = &hDevice->pI2S->I2S_OUT1H;
          }

          /* Check if right channel is enabled */
          if((hDevice->pI2S->I2S_MODE2 & BITM_I2S_I2S_MODE2_DRV_CH2) != 0)
          {
              /* In 24-bit mode the I2S_OUT1L should be filled with LSBs of the audio sample (bit 7-0), in 16 bit
                 mode, this register should not be filled */
              if(hDevice->b24bitMode == true)
                hDevice->DataPtrs[hDevice->NumChnls++] = &hDevice->pI2S->I2S_OUT2L;

            /* In 24-bit mode bits 23-8 or the sample are stored in I2S_OUT2H, in 16-bit mode bits 15-0 are stored */
              hDevice->DataPtrs[hDevice->NumChnls++] = &hDevice->pI2S->I2S_OUT2H;
          }
    }

    /* Read the MMR. Clear the bit field. Write the bit field. Write back the read-modified-write value */
    i2sMMRreg = hDevice->pI2S->I2S_CFG1;
    i2sMMRreg &= ~BITM_I2S_I2S_CFG1_I2S_EN;
    /* Set the enable bit */
    i2sMMRreg |=  ((int)bFlag << BITP_I2S_I2S_CFG1_I2S_EN);
    hDevice->pI2S->I2S_CFG1 = i2sMMRreg;


    return ADI_I2S_SUCCESS;
}


/*
    //////////////////////////////////////////////////////////////////////////////
    ////////////////////   POINTER TABLE FUNCTIONS   /////////////////////////////
    //////////////////////////////////////////////////////////////////////////////
*/


/* Interrupt mode initialization function */
static ADI_I2S_RESULT_TYPE intInit(ADI_I2S_DEV_HANDLE const hDevice)
{
  return ADI_I2S_SUCCESS;
}

/* Interrupt mode un-initialization function */
static ADI_I2S_RESULT_TYPE intUnInit(ADI_I2S_DEV_HANDLE const hDevice)
{
  return ADI_I2S_SUCCESS;
}

#if (ADI_I2S_CFG_ENABLE_DMA_SUPPORT == 1)
/* DMA mode initialization function */
static ADI_I2S_RESULT_TYPE dmaInit(ADI_I2S_DEV_HANDLE const hDevice)
{
   if( ADI_DMA_SUCCESS != adi_DMA_Init(I2S_CHANn, ADI_DMA_PRIORITY_RESET))
     return ADI_I2S_ERR_BAD_DMA_CONFIG;

   return ADI_I2S_SUCCESS;
}

/* DMA mode un-initialization function */
static ADI_I2S_RESULT_TYPE dmaUnInit(ADI_I2S_DEV_HANDLE const hDevice)
{
  if (ADI_DMA_SUCCESS != adi_DMA_UnInit(I2S_CHANn))
    return ADI_I2S_ERR_BAD_DMA_CONFIG;

    return ADI_I2S_SUCCESS;
}
#endif /* ADI_I2S_CFG_ENABLE_DMA_SUPPORT */


/*
    //////////////////////////////////////////////////////////////////////////////
    //////////////////////////   DMA-RELATED   ///////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////
*/

/* stub for interrupt mode */
static ADI_I2S_RESULT_TYPE intInitializeDescriptors(
    ADI_I2S_DEV_HANDLE const hDevice,
    DMA_CHANn_TypeDef chan)
{
    return ADI_I2S_ERR_BAD_API_FOR_MODE;
}

#if (ADI_I2S_CFG_ENABLE_DMA_SUPPORT == 1)
/* prefill DMA pipeline with one descriptor */
static ADI_I2S_RESULT_TYPE dmaInitializeDescriptors(
    ADI_I2S_DEV_HANDLE const hDevice,
    DMA_CHANn_TypeDef chan)
{
    /* always start with the primary dma descriptor */
    hDevice->nextDescrType = ADI_DMA_CCD_PRIMARY;

    /* reset all prim/alt descriptor selectors from any previous transfers  */
    /* so the primary descriptor is active at the start of each transaction */
    pADI_DMA->DMAALTCLR = (
                          (0x1ul << I2S_CHANn)
                            );


    /* initialize common DMA descriptor variables */
    hDevice->pDmaDescriptor->Chan       = chan;
    hDevice->pDmaDescriptor->DataWidth  = ADI_DMA_WIDTH_HALFWORD;
    hDevice->pDmaDescriptor->Protection = ADI_DMA_HPROT_NONE;

    /* Set r-power based on number DMA transfers required to fill the i2s frame. */
    hDevice->pDmaDescriptor->ArbitRate = (ADI_DMA_RPOWER_TYPE)(hDevice->NumChnls >> 1);

    /* submit 1st descriptor */
    scheduleNextDescriptor(hDevice);



    /* Always submit two descriptors initially (primary & alternate) even if the 2nd      */
    /* has no data... because the dma ping-pong mode is stopped by an invalid descriptor. */
    /* For more than two descriptor transfers, the last submit will be tne invalid one,   */
    /* but that is programmed during the dma interrupt, not here.                         */

    /* submit 2nd descriptor */
    scheduleNextDescriptor(hDevice);

    //adi_DMA_PrintDescriptors(I2S_CHANn);
    return ADI_I2S_SUCCESS;
}


/* called both by DMA pipeline prefill code and by DMA interupts */
static ADI_I2S_RESULT_TYPE scheduleNextDescriptor(ADI_I2S_DEV_HANDLE const hDevice)
{

    register ADI_DMA_TRANSFER_TYPE* pD = hDevice->pDmaDescriptor;
    register uint32_t nextChunkSize;

    /* get the next descriptor type */
    pD->CCD = hDevice->nextDescrType;

    /* switch it for next time */
    hDevice->nextDescrType = (ADI_DMA_CCD_PRIMARY == hDevice->nextDescrType) ? ADI_DMA_CCD_ALTERNATE : ADI_DMA_CCD_PRIMARY;

    /* Intialize the DMA descriptors for transmit */
    pD->pSrcData = (void *)hDevice->pTxBuffer;
    pD->SrcInc   = ADI_DMA_INCR_HALFWORD;

    if(hDevice->b24bitMode == true)
      pD->pDstData = (void*) &pADI_I2S->I2S_OUT1L;  /* If 24-bit */
    else
      pD->pDstData = (void*) &pADI_I2S->I2S_OUT1H;  /* If 16-bit */

    pD->DstInc   = ADI_DMA_INCR_NONE;

      /* update next transmit descriptor. The ADI_DMA_MAX_TRANSFER_SIZE is in terms of transfer units.
         So multiply by 2 to get the next transfer size. */
    if (nextChunkSize = MIN(hDevice->TxRemaining, ADI_DMA_MAX_TRANSFER_SIZE << 1))
    {
        /* non-zero case */
        pD->Mode               = ADI_DMA_MODE_PINGPONG;
        pD->DataLength         = nextChunkSize >> 1;    // Divide by 2 as the DMA transfer width is 2.
        hDevice->pTxBuffer     += nextChunkSize;
        hDevice->TxRemaining   -= nextChunkSize;

    }
    else
    {
        /* zero case */
        pD->Mode               = ADI_DMA_MODE_INVALID;
        pD->DataLength         = 0;
        hDevice->pTxBuffer     = NULL;
        hDevice->TxRemaining   = 0;
    }

    /* submit next descriptor */
    if (ADI_DMA_SUCCESS != adi_DMA_SubmitTransfer(pD))
        return ADI_I2S_ERR_BAD_DMA_CONFIG;

    return ADI_I2S_SUCCESS;
}
#endif /* ADI_I2S_CFG_ENABLE_DMA_SUPPORT */

#if (ADI_I2S_CFG_ENABLE_DMA_SUPPORT == 1)
/* Common DMA Handler for non-dma mode (nop) */
void intCommonHandler(DMA_CHANn_TypeDef channelNum)
{
    /*
        No dma interrupts should ever come through this path.
        Only here for symmetry with the dmaCommonHandler()
        call, as dispatched through function pointer table
        pDmaHandlerFcn function.  Implimented as an error trap
        to catch logic error and avoid silent failure.
    */
    while (1) ;
}

/* Handles buffer completion */
static void OnBufferComplete(ADI_I2S_DEV_DATA_TYPE *pDevData)
{
#if (ADI_CFG_ENABLE_RTOS_SUPPORT == 1)
    if(pDevData->bBlockingMode == true)
    {
            adi_osal_SemPost(pDevData->hTxSem);
    }
#else
   /* Any interrupt that changes the I2S device state needs to bring the
    * processor out of low-power mode, as functions may be waiting for the
    * state to change.
    */
    SystemExitLowPowerMode(NULL);

#endif /* ADI_CFG_ENABLE_RTOS_SUPPORT */

    /* If a callback is registered notify the buffer processed event to the application */
    if(pDevData->pfCallback)
    {
        pDevData->pfCallback(pDevData->pCBParam, ADI_I2S_EVENT_BUFFER_PROCESSED, NULL);
    }

}

/* Common DMA Handler code */
void dmaCommonHandler(DMA_CHANn_TypeDef channelNum)
{

  /* get both descriptor modes */
    uint32_t pri_mode = (&pADI_DCCPRI[channelNum])->DMACDC & DMA_CCD_CYCLE_MASK;
    uint32_t alt_mode = (&pADI_DCCALT[channelNum])->DMACDC & DMA_CCD_CYCLE_MASK;

    /* if both descriptors are done... */
    if (0 == pri_mode && 0 == alt_mode) {

        /* Disable the DMA and auto-increment */
        pADI_I2S->I2S_CFG1 &=  ~(BITM_I2S_I2S_CFG1_DMA_EN | BITM_I2S_I2S_CFG1_AUTO_INCR);

        /* Set the flag to indicate that DMA is complete */
        pI2S0_DevData->bDmaComplete = true;

        /* Handle buffer completion */
        OnBufferComplete(pI2S0_DevData);
    }
    else
    {
        /* schedule another as needed */
        if (pI2S0_DevData->TxRemaining)
        {
            scheduleNextDescriptor(pI2S0_DevData);
        }
    }
}
#endif /* ADI_I2S_CFG_ENABLE_DMA_SUPPORT */

/*!
 * @brief  I2S Interrupt Handler.
 *
 * @return void.
 *
 * Override of default (WEAK) I2S interrupt handler.
 */

ADI_INT_HANDLER(I2S_Int_Handler)
{
  uint16_t status = pI2S0_DevData->pI2S->I2S_STAT;
  uint16_t i;

  /* Get the number of samples in the FIFO */
  uint16_t SamplesInFifo = ((status & BITM_I2S_I2S_STAT_FIFO_STAT) >> BITP_I2S_I2S_STAT_FIFO_STAT);

  /* Clear the interrupt cause */
  pI2S0_DevData->pI2S->I2S_STAT = (BITM_I2S_I2S_STAT_REQ_PEND | BITM_I2S_I2S_STAT_STAT_PEND);

   /* Fill the FIFO until we reach the water mark level */
   while(SamplesInFifo < ADI_I2S_FIFO_WATERMARK)
   {
      /* If there are no samples left to transmit, return */
      if( pI2S0_DevData->TxRemaining < (pI2S0_DevData->NumChnls << 1))
      {
        pI2S0_DevData->TxRemaining = 0;

        /* Handle buffer completion */
        OnBufferComplete(pI2S0_DevData);

        return;
      }

      for(i = 0; i < pI2S0_DevData->NumChnls; i++)
      {
       *pI2S0_DevData->DataPtrs[i] = *(uint16_t *)pI2S0_DevData->pTxBuffer;;

        pI2S0_DevData->pTxBuffer += 2;
        pI2S0_DevData->TxRemaining -= 2;
      }

      SamplesInFifo += pI2S0_DevData->NumChnls;
   }
}

#if (ADI_I2S_CFG_ENABLE_DMA_SUPPORT == 1)
ADI_INT_HANDLER(DMA_I2S_Int_Handler)
{
  pI2S0_DevData->FunctionTable.pDmaHandlerFcn(I2S_CHANn);
}
#endif /* ADI_I2S_CFG_ENABLE_DMA_SUPPORT */


/*@}*/

#define REVERT_ADI_MISRA_SUPPRESSIONS  /*!< Revert ADI MISRA Suppressions */
#include "misra.h"

/*
** EOF
*/
