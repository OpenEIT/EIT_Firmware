
/*! \addtogroup AFE_Library AFE Library
 *  @{
 */

#define ASSERT_ADI_MISRA_SUPPRESSIONS  /*!< Apply ADI MISRA Suppressions */
#include "misra.h"

#include <stdlib.h>  /* for 'NULL" definition */
#include "dma.h"
#include "afe.h"

/*! \cond PRIVATE */

static ADI_DMA_TRANSFER_TYPE             gDmaDescriptorForAFETx;
static ADI_DMA_TRANSFER_TYPE             gDmaDescriptorForAFERx;

/*! \struct ADI_AFE_DEV_DATA_TYPE     */
/* AFE Device instance data structure */
typedef struct ADI_AFE_DEV_DATA_TYPE {
    /* Device attributes */
    bool_t                              bInitialized;               /*!< API initialized flag                                */
    bool_t                              bUseRxDma;                  /*!< DMA mode flag                                       */

    /* Sequencer */
    volatile ADI_AFE_SEQ_STATE_TYPE     seqState;                   /*!< Sequencer state                                     */
    volatile ADI_AFE_RESULT_TYPE        seqError;                   /*!< Sequencer error                                     */
    volatile bool_t                     bCmdFifoReady;              /*!< Command FIFO is ready for sequencer start flag.     */
    volatile bool_t                     bSeqFinished;               /*!< Sequencer execution finished flag.                  */
    bool_t                              bSoftwareCRC;               /*!< Flag to indicate the use of dynamic CRC calculation */
    bool_t                              bRunSequenceBlockingMode;   /*!< Flag to set adi_AFE_RunSequence blocking mode       */

    /* MMRs */
    ADI_AFE_TypeDef                     *pAFE;                      /*!< MMR address for this AFE                           */
    
    /* External Component Values */
    uint32_t                            rcal;                       /*!< RCAL value in ohms                                 */
    uint32_t                            rtia;                       /*!< RTIA value in ohms                                 */
    

#if (ADI_CFG_ENABLE_RTOS_SUPPORT == 1)
    /* Memory required to create the semaphore */
    uint8_t                             SemMem[ADI_OSAL_MAX_SEM_SIZE_CHAR];
    ADI_OSAL_SEM_HANDLE                 hSeqSem;
#else
    volatile bool_t                     bInterruptFlag;             /*!> Flag for low power mode                            */    
#endif /* ADI_CFG_ENABLE_RTOS_SUPPORT */

    /* Callbacks */
    ADI_CALLBACK                        cbTxDmaFcn;                 /*!< Pointer to callback function.                      */
    ADI_CALLBACK                        cbRxDmaFcn;                 /*!< Pointer to callback function.                      */
    ADI_CALLBACK                        cbCaptureFcn;               /*!< Pointer to callback function.                      */
    uint32_t                            cbCaptureWatch;             /*!< Callback watch bits.                               */
    ADI_CALLBACK                        cbGenerateFcn;              /*!< Pointer to callback function.                      */
    uint32_t                            cbGenerateWatch;            /*!< Callback watch bits.                               */
    ADI_CALLBACK                        cbCmdFifoFcn;               /*!< Pointer to callback function.                      */
    uint32_t                            cbCmdFifoWatch;             /*!< Callback watch bits.                               */
    ADI_CALLBACK                        cbDataFifoFcn;              /*!< Pointer to callback function.                      */
    uint32_t                            cbDataFifoWatch;            /*!< Callback watch bits.                               */
    
    /* DMA-Mode state */
    ADI_DMA_TRANSFER_TYPE*              pTxDmaDescriptor;           /*!< DMA descriptor pointer */
    ADI_DMA_TRANSFER_TYPE*              pRxDmaDescriptor;           /*!< DMA descriptor pointer */
    volatile bool_t                     bTxDmaComplete;             /*!< DMA TX complete flag   */
    volatile bool_t                     bRxDmaComplete;             /*!< DMA RX complete flag   */
    uint16_t*                           pRxBuffer;                  /*!< Pointer to receive buffer                          */
    uint16_t                            dmaRxTransferCount;         /*!< Number of transfers in the current DMA cycle       */
#if (ADI_AFE_CFG_ENABLE_RX_DMA_DUAL_BUFFER_SUPPORT == 1)
    uint16_t                            dmaRxRemaining;             /*!< Remaining number of DMA transfers                  */
    uint16_t                            dmaRxBufferMaxSize[2];      /*!< Maximum sizes for dual buffer Rx DMA transfers     */
    uint16_t                            dmaRxBufferIndex;           /*!< Current Rx DMA buffer                              */
#endif
} ADI_AFE_DEV_DATA_TYPE;

/* allocate instance data array */
ADI_AFE_DEV_DATA_TYPE AFE_DevData;                                  /*!< Private AFE device instance data                   */

/* internal device data pointers */
static ADI_AFE_DEV_DATA_TYPE* const pAFE_DevData = &AFE_DevData;    /*!< Pointer to private AFE device instance data        */

static ADI_DMA_TRANSFER_TYPE            gDmaDescriptorForAFETx;
static ADI_DMA_TRANSFER_TYPE            gDmaDescriptorForAFERx;



/* forward declarations... */
/* These are needed in the case where ADI_CFG_ENABLE_RTOS_SUPPORT = 1            */
/* The ADI_INSTALL_HANDLER macro will expand to a call to adi_int_InstallHandler */
/* and the handler will be referenced                                            */
/* The non-OSAL expansion of ADI_INSTALL_HANDLER results in a NULL string        */
ADI_INT_HANDLER(AFE_Capture_Int_Handler);
ADI_INT_HANDLER(AFE_Generate_Int_Handler);
ADI_INT_HANDLER(AFE_CmdFIFO_Int_Handler);
ADI_INT_HANDLER(AFE_DataFIFO_Int_Handler);
ADI_INT_HANDLER(DMA_AFE_TX_Int_Handler);
ADI_INT_HANDLER(DMA_AFE_RX_Int_Handler);


/* Calbration registers password */
#define CALDATA_UNLOCK                  0xDE87A5AF                  /*!< Password to unlock calibration registers.          */

/* Polynomial for sequencer CRC-8 */
#define ADI_AFE_SEQ_CRC8_POLYNOMIAL     0x07                        /*!< Sequencer CRC8 polynomial.                         */



/*! \endcond */

/***************************************************************************/
/*   FIFOs                                                                 */
/***************************************************************************/

static __INLINE ADI_AFE_RESULT_TYPE fifoEnable(ADI_AFE_DEV_HANDLE const hDevice) {
    uint32_t mask;

#ifdef ADI_DEBUG
    if (adi_AFE_InvalidHandle(hDevice)) {
        return ADI_AFE_ERR_BAD_DEV_HANDLE;
    }

    if (adi_AFE_HandleNotInitialized(hDevice)) {
        return ADI_AFE_ERR_NOT_INITIALIZED;
    }

    if (pADI_SYSCLK->CLKCON5 & BITM_SYSCLK_CLKCON5_ACLKOFF) {
        return ADI_AFE_ERR_ACLKOFF;
    }

    if (16000000 != SystemGetClockFrequency(ADI_SYS_CLOCK_ACLK)) {
        return ADI_AFE_ERR_WRONG_ACLK_FREQUENCY;
    }
#endif

    ADI_ENTER_CRITICAL_REGION();

    /* Enable data FIFO only if using the Rx DMA */
    mask = (hDevice->bUseRxDma)
        ? BITM_AFE_AFE_FIFO_CFG_DATA_FIFO_EN
        : 0x00000000;
    /* Enable command FIFO */
    mask |= BITM_AFE_AFE_FIFO_CFG_CMD_FIFO_EN;
    hDevice->pAFE->AFE_FIFO_CFG |= mask;

    ADI_EXIT_CRITICAL_REGION();

    return ADI_AFE_SUCCESS;
}

static __INLINE ADI_AFE_RESULT_TYPE fifoDisable(ADI_AFE_DEV_HANDLE const hDevice) {

#ifdef ADI_DEBUG
    if (adi_AFE_InvalidHandle(hDevice)) {
        return ADI_AFE_ERR_BAD_DEV_HANDLE;
    }

    if (adi_AFE_HandleNotInitialized(hDevice)) {
        return ADI_AFE_ERR_NOT_INITIALIZED;
    }

    if (pADI_SYSCLK->CLKCON5 & BITM_SYSCLK_CLKCON5_ACLKOFF) {
        return ADI_AFE_ERR_ACLKOFF;
    }

    if (16000000 != SystemGetClockFrequency(ADI_SYS_CLOCK_ACLK)) {
        return ADI_AFE_ERR_WRONG_ACLK_FREQUENCY;
    }
#endif

    ADI_ENTER_CRITICAL_REGION();

    /* Disable both FIFOs */
    hDevice->pAFE->AFE_FIFO_CFG &= ~(BITM_AFE_AFE_FIFO_CFG_CMD_FIFO_EN | BITM_AFE_AFE_FIFO_CFG_DATA_FIFO_EN);

    ADI_EXIT_CRITICAL_REGION();

    return ADI_AFE_SUCCESS;
}

/*!
 * @brief       Select the input source for the data FIFO.
 *
 * @param[in]   hDevice                                 Device handle obtained from adi_AFE_Init().
 * @param[in]   sel                                     Select bits for the data FIFO input source.
 *
 * @return      Status
 *              - #ADI_AFE_SUCCESS                      Call completed successfully.
 *              - #ADI_AFE_ERR_BAD_DEV_HANDLE           Invalid device handle.
 *              - #ADI_AFE_ERR_NOT_INITIALIZED          Device not initialized.
 *              - #ADI_AFE_ERR_ACLKOFF                  ACLK disabled from the clock gate.
 *              - #ADI_AFE_ERR_WRONG_ACLK_FREQUENCY     Programmed ACLK frequency is not the required 16MHz.
 *
 * @details
 *
 */

ADI_AFE_RESULT_TYPE adi_AFE_SetDataFifoSource(ADI_AFE_DEV_HANDLE const hDevice, ADI_AFE_DATA_FIFO_SOURCE_TYPE sel) {
    uint32_t    tmp;

#ifdef ADI_DEBUG
    if (adi_AFE_InvalidHandle(hDevice)) {
        return ADI_AFE_ERR_BAD_DEV_HANDLE;
    }

    if (adi_AFE_HandleNotInitialized(hDevice)) {
        return ADI_AFE_ERR_NOT_INITIALIZED;
    }

    if (pADI_SYSCLK->CLKCON5 & BITM_SYSCLK_CLKCON5_ACLKOFF) {
        return ADI_AFE_ERR_ACLKOFF;
    }

    if (16000000 != SystemGetClockFrequency(ADI_SYS_CLOCK_ACLK)) {
        return ADI_AFE_ERR_WRONG_ACLK_FREQUENCY;
    }

    if(!((ADI_AFE_DATA_FIFO_SOURCE_ADC == sel) || (ADI_AFE_DATA_FIFO_SOURCE_DFT == sel) || (ADI_AFE_DATA_FIFO_SOURCE_LPF == sel))) {
        return ADI_AFE_ERR_INVALID_DATA_FIFO_SRC;
    }
#endif

    ADI_ENTER_CRITICAL_REGION();

        /* change in a tmp variable and write entire new value all at once */
        tmp = hDevice->pAFE->AFE_FIFO_CFG;
        tmp &= ~BITM_AFE_AFE_FIFO_CFG_DATA_FIFO_SOURCE_SEL;         /* make a hole */
        tmp |= sel << BITP_AFE_AFE_FIFO_CFG_DATA_FIFO_SOURCE_SEL;   /* fill it */
        hDevice->pAFE->AFE_FIFO_CFG = tmp;                          /* write it */

    ADI_EXIT_CRITICAL_REGION();

    return ADI_AFE_SUCCESS;
}

/***************************************************************************/
/*   Waveform Generator                                                    */
/***************************************************************************/

/*!
 * @brief       Select the waveform type.
 *
 * @param[in]   hDevice                                 Device handle obtained from adi_AFE_Init().
 * @param[in]   sel                                     Waveform generator select bits.
 *
 * @return      Status
 *              - #ADI_AFE_SUCCESS                      Call completed successfully.
 *              - #ADI_AFE_ERR_BAD_DEV_HANDLE           Invalid device handle.
 *              - #ADI_AFE_ERR_NOT_INITIALIZED          Device not initialized.
 *              - #ADI_AFE_ERR_ACLKOFF                  ACLK disabled from the clock gate.
 *              - #ADI_AFE_ERR_WRONG_ACLK_FREQUENCY     Programmed ACLK frequency is not the required 16MHz.
 *
 * @details
 *
 */

ADI_AFE_RESULT_TYPE adi_AFE_SetWavegenType(ADI_AFE_DEV_HANDLE const hDevice, ADI_AFE_WAVEGEN_TYPE sel) {
    uint32_t    tmp;

#ifdef ADI_DEBUG
    if (adi_AFE_InvalidHandle(hDevice)) {
        return ADI_AFE_ERR_BAD_DEV_HANDLE;
    }

    if (adi_AFE_HandleNotInitialized(hDevice)) {
        return ADI_AFE_ERR_NOT_INITIALIZED;
    }

    if (pADI_SYSCLK->CLKCON5 & BITM_SYSCLK_CLKCON5_ACLKOFF) {
        return ADI_AFE_ERR_ACLKOFF;
    }

    if (16000000 != SystemGetClockFrequency(ADI_SYS_CLOCK_ACLK)) {
        return ADI_AFE_ERR_WRONG_ACLK_FREQUENCY;
    }
    
    if(!((ADI_AFE_WAVEGEN_DIRECT == sel) || (ADI_AFE_WAVEGEN_SINUSOID == sel) || (ADI_AFE_WAVEGEN_TRAPEZOID == sel))) {
        return ADI_AFE_ERR_INVALID_WG_MODE;
    }
#endif

    ADI_ENTER_CRITICAL_REGION();

        /* change in a tmp variable and write entire new value all at once */
        tmp = hDevice->pAFE->AFE_WG_CFG;
        tmp &= ~BITM_AFE_AFE_WG_CFG_TYPE_SEL;       /* make a hole */
        tmp |= sel << BITP_AFE_AFE_WG_CFG_TYPE_SEL; /* fill it */
        hDevice->pAFE->AFE_WG_CFG = tmp;            /* write it */

    ADI_EXIT_CRITICAL_REGION();

    return ADI_AFE_SUCCESS;
}



/***************************************************************************/
/*   Switch Matrix                                                         */
/***************************************************************************/

/*!
 * @brief       Program the switch matrix
 *
 * @param[in]   hDevice     Device handle obtained from adi_AFE_Init().
 * @param[in]   state                                   Encoded switch matrix state.
 *
 * @return      Status
 *              - #ADI_AFE_SUCCESS                        Call completed successfully.
 *              - #ADI_AFE_ERR_BAD_DEV_HANDLE           Invalid device handle.
 *              - #ADI_AFE_ERR_NOT_INITIALIZED          Device not initialized.
 *              - #ADI_AFE_ERR_ACLKOFF                  ACLK disabled from the clock gate.
 *              - #ADI_AFE_ERR_WRONG_ACLK_FREQUENCY     Programmed ACLK frequency is not the required 16MHz.
 *
 * @details     The function programs the switch matrix by writing to AFE_SW_CFG using encoded switch states.
 *
 */

ADI_AFE_RESULT_TYPE adi_AFE_SetSwitchState(ADI_AFE_DEV_HANDLE const hDevice, uint32_t state) {

#ifdef ADI_DEBUG
    if (adi_AFE_InvalidHandle(hDevice)) {
        return ADI_AFE_ERR_BAD_DEV_HANDLE;
    }

    if (adi_AFE_HandleNotInitialized(hDevice)) {
        return ADI_AFE_ERR_NOT_INITIALIZED;
    }

    if (pADI_SYSCLK->CLKCON5 & BITM_SYSCLK_CLKCON5_ACLKOFF) {
        return ADI_AFE_ERR_ACLKOFF;
    }

    if (16000000 != SystemGetClockFrequency(ADI_SYS_CLOCK_ACLK)) {
        return ADI_AFE_ERR_WRONG_ACLK_FREQUENCY;
    }
#endif

    hDevice->pAFE->AFE_SW_CFG = state;

    return ADI_AFE_SUCCESS;
}


/***************************************************************************/
/*   Calibration  Registers                                                */
/***************************************************************************/

/*!
 * @brief       Write to a calibration register
 *
 * @param[in]   hDevice                                 Device handle obtained from adi_AFE_Init().
 * @param[in]   Reg                                     Calibration register to be written.
 * @param[in]   Val                                     Value to be written to the register.
 *
 * @return      Status
 *              - #ADI_AFE_SUCCESS                      Call completed successfully.
 *              - #ADI_AFE_ERR_BAD_DEV_HANDLE           Invalid device handle.
 *              - #ADI_AFE_ERR_NOT_INITIALIZED          Device not initialized.
 *              - #ADI_AFE_ERR_ACLKOFF                  ACLK disabled from the clock gate.
 *              - #ADI_AFE_ERR_WRONG_ACLK_FREQUENCY     Programmed ACLK frequency is not the required 16MHz.
 *
 * @details     Writes a 32-bit value to an AFE calibration register, handling the calibration register
 *              unlock prior to the write, and locking it again after the write takes place.
 *
 */

ADI_AFE_RESULT_TYPE adi_AFE_WriteCalibrationRegister(ADI_AFE_DEV_HANDLE const hDevice, ADI_AFE_CAL_REG_TYPE Reg, uint32_t Val) {
    ADI_AFE_RESULT_TYPE result = ADI_AFE_SUCCESS;

#ifdef ADI_DEBUG
    if (adi_AFE_InvalidHandle(hDevice)) {
        return ADI_AFE_ERR_BAD_DEV_HANDLE;
    }

    if (adi_AFE_HandleNotInitialized(hDevice)) {
        return ADI_AFE_ERR_NOT_INITIALIZED;
    }

    if (pADI_SYSCLK->CLKCON5 & BITM_SYSCLK_CLKCON5_ACLKOFF) {
        return ADI_AFE_ERR_ACLKOFF;
    }

    if (16000000 != SystemGetClockFrequency(ADI_SYS_CLOCK_ACLK)) {
        return ADI_AFE_ERR_WRONG_ACLK_FREQUENCY;
    }
#endif

    ADI_ENTER_CRITICAL_REGION();

        /* Unlock calibration registers */
        hDevice->pAFE->AFE_CAL_DATA_LOCK = CALDATA_UNLOCK;
        /* Write the register */
        *((uint32_t *)Reg) = Val;

        /* Lock calibration registers */
        hDevice->pAFE->AFE_CAL_DATA_LOCK = 0x00000000;

    ADI_EXIT_CRITICAL_REGION();

    return result;
}

/*!
 * @brief       Read from a calibration register
 *
 * @param[in]   hDevice                                 Device handle obtained from adi_AFE_Init().
 * @param[in]   Reg                                     Calibration register to be read.
 * @param[in]   pVal                                    Pointer to the value read from the register.
 *
 * @return      Status
 *              - #ADI_AFE_SUCCESS                      Call completed successfully.
 *              - #ADI_AFE_ERR_BAD_DEV_HANDLE           Invalid device handle.
 *              - #ADI_AFE_ERR_NOT_INITIALIZED          Device not initialized.
 *              - #ADI_AFE_ERR_ACLKOFF                  ACLK disabled from the clock gate.
 *              - #ADI_AFE_ERR_WRONG_ACLK_FREQUENCY     Programmed ACLK frequency is not the required 16MHz. *
 * @details     Reads the value of an AFE calibration register. No unlock/lock is required.
 *
 */

ADI_AFE_RESULT_TYPE adi_AFE_ReadCalibrationRegister(ADI_AFE_DEV_HANDLE const hDevice, ADI_AFE_CAL_REG_TYPE Reg, uint32_t *pVal) {
    ADI_AFE_RESULT_TYPE         result = ADI_AFE_SUCCESS;

#ifdef ADI_DEBUG
    if (adi_AFE_InvalidHandle(hDevice)) {
        return ADI_AFE_ERR_BAD_DEV_HANDLE;
    }

    if (adi_AFE_HandleNotInitialized(hDevice)) {
        return ADI_AFE_ERR_NOT_INITIALIZED;
    }

    if (pADI_SYSCLK->CLKCON5 & BITM_SYSCLK_CLKCON5_ACLKOFF) {
        return ADI_AFE_ERR_ACLKOFF;
    }

    if (16000000 != SystemGetClockFrequency(ADI_SYS_CLOCK_ACLK)) {
        return ADI_AFE_ERR_WRONG_ACLK_FREQUENCY;
    }
#endif

    ADI_ENTER_CRITICAL_REGION();

        /* Read the register */
        *pVal = *((volatile uint32_t *)Reg);

    ADI_EXIT_CRITICAL_REGION();

    return result;
}

/***************************************************************************/
/*   DMA                                                                   */
/***************************************************************************/

/*
 * Initializes the DMA descriptors for both Tx and Rx channels.
 * All the constant fields are filled in here. The function-specific fields
 * are programmed by functions adi_AFE_ProgramTxDMA()/adi_AFE_ProgramRxDMA()
 */

static ADI_AFE_RESULT_TYPE dmaInit(ADI_AFE_DEV_HANDLE const hDevice) {
    ADI_AFE_RESULT_TYPE result = ADI_AFE_SUCCESS;
    ADI_DMA_TRANSFER_TYPE*      pTxD   = hDevice->pTxDmaDescriptor;
    ADI_DMA_TRANSFER_TYPE*      pRxD   = hDevice->pRxDmaDescriptor;

#ifdef ADI_DEBUG
    if (adi_AFE_InvalidHandle(hDevice)) {
        return ADI_AFE_ERR_BAD_DEV_HANDLE;
    }

    if (pADI_SYSCLK->CLKCON5 & BITM_SYSCLK_CLKCON5_ACLKOFF) {
        return ADI_AFE_ERR_ACLKOFF;
    }
#endif

    /* Command FIFO DMA (Tx) */
    pTxD->Chan = AFE_TX_CHANn;
    pTxD->CCD = ADI_DMA_CCD_PRIMARY;
    pTxD->DataWidth  = ADI_DMA_WIDTH_WORD;
    pTxD->pDstData  = (void *) &hDevice->pAFE->AFE_CMD_FIFO_WRITE;
    pTxD->SrcInc  = ADI_DMA_INCR_WORD;
    pTxD->DstInc  = ADI_DMA_INCR_NONE;

    pTxD->Protection = ADI_DMA_HPROT_NONE;
    pTxD->ArbitRate  = ADI_DMA_RPOWER_1;
    pTxD->Mode  = ADI_DMA_MODE_BASIC;

    /* Data FIFO DMA (Rx) */
    pRxD->Chan = AFE_RX_CHANn;
    pRxD->CCD = ADI_DMA_CCD_PRIMARY;
    pRxD->DataWidth  = ADI_DMA_WIDTH_HALFWORD;
    pRxD->pSrcData  = (void *) &hDevice->pAFE->AFE_DATA_FIFO_READ;
    pRxD->DstInc  = ADI_DMA_INCR_HALFWORD;
    pRxD->SrcInc  = ADI_DMA_INCR_NONE;

    pRxD->Protection = ADI_DMA_HPROT_NONE;
    pRxD->ArbitRate  = ADI_DMA_RPOWER_1;
    pRxD->Mode  = ADI_DMA_MODE_BASIC;

    /* allocate TX/RX channels for this device */
    if (ADI_DMA_SUCCESS != adi_DMA_Init(AFE_TX_CHANn, ADI_DMA_PRIORITY_RESET)) {
        result = ADI_AFE_ERR_DMA;
    }
    if (!result && (ADI_DMA_SUCCESS != adi_DMA_Init(AFE_RX_CHANn, ADI_DMA_PRIORITY_RESET))) {
        result = ADI_AFE_ERR_DMA;
    }

    return result;
}

/*
 * Enable the AFE DMA requests. Always enables the command FIFO DMA,
 * and enables data FIFO DMA only if needed.
 *
 */

static __INLINE ADI_AFE_RESULT_TYPE dmaEnable(ADI_AFE_DEV_HANDLE const hDevice) {
    ADI_AFE_RESULT_TYPE result = ADI_AFE_SUCCESS;
    uint32_t mask;

#ifdef ADI_DEBUG
    if (adi_AFE_InvalidHandle(hDevice)) {
        return ADI_AFE_ERR_BAD_DEV_HANDLE;
    }

    if (adi_AFE_HandleNotInitialized(hDevice)) {
        return ADI_AFE_ERR_NOT_INITIALIZED;
    }

    if (pADI_SYSCLK->CLKCON5 & BITM_SYSCLK_CLKCON5_ACLKOFF) {
        return ADI_AFE_ERR_ACLKOFF;
    }

    if (16000000 != SystemGetClockFrequency(ADI_SYS_CLOCK_ACLK)) {
        return ADI_AFE_ERR_WRONG_ACLK_FREQUENCY;
    }
#endif

    ADI_ENTER_CRITICAL_REGION();

        /* Enable Rx DMA only if using it */
        mask = (hDevice->bUseRxDma)
          ? BITM_AFE_AFE_FIFO_CFG_DATA_FIFO_DMA_REQ_EN
          : 0x00000000;
        /* Enable Tx DMA */
        mask |= BITM_AFE_AFE_FIFO_CFG_CMD_FIFO_DMA_REQ_EN;
        hDevice->pAFE->AFE_FIFO_CFG |= mask;

    ADI_EXIT_CRITICAL_REGION();

    return result;
}

/*
 * Disable the AFE DMA requests. Always disables the requests for both DMA channels.
 *
 */

static __INLINE ADI_AFE_RESULT_TYPE dmaDisable(ADI_AFE_DEV_HANDLE const hDevice) {
    ADI_AFE_RESULT_TYPE result = ADI_AFE_SUCCESS;

#ifdef ADI_DEBUG
    if (adi_AFE_InvalidHandle(hDevice)) {
        return ADI_AFE_ERR_BAD_DEV_HANDLE;
    }

    if (adi_AFE_HandleNotInitialized(hDevice)) {
        return ADI_AFE_ERR_NOT_INITIALIZED;
    }

    if (pADI_SYSCLK->CLKCON5 & BITM_SYSCLK_CLKCON5_ACLKOFF) {
        return ADI_AFE_ERR_ACLKOFF;
    }

    if (16000000 != SystemGetClockFrequency(ADI_SYS_CLOCK_ACLK)) {
        return ADI_AFE_ERR_WRONG_ACLK_FREQUENCY;
    }
#endif

    ADI_ENTER_CRITICAL_REGION();

        /* Disable both DMA channels */
        hDevice->pAFE->AFE_FIFO_CFG &= (~BITM_AFE_AFE_FIFO_CFG_DATA_FIFO_DMA_REQ_EN & ~BITM_AFE_AFE_FIFO_CFG_CMD_FIFO_DMA_REQ_EN);

    ADI_EXIT_CRITICAL_REGION();

    return result;
}

/*
 * Uninitialize both DMA channels.
 *
 */

static ADI_AFE_RESULT_TYPE dmaUnInit(ADI_AFE_DEV_HANDLE const hDevice) {

#ifdef ADI_DEBUG
    if (adi_AFE_InvalidHandle(hDevice)) {
        return ADI_AFE_ERR_BAD_DEV_HANDLE;
    }

    if (adi_AFE_HandleNotInitialized(hDevice)) {
        return ADI_AFE_ERR_NOT_INITIALIZED;
    }

    if (pADI_SYSCLK->CLKCON5 & BITM_SYSCLK_CLKCON5_ACLKOFF) {
        return ADI_AFE_ERR_ACLKOFF;
    }
#endif

    if ((ADI_DMA_SUCCESS != adi_DMA_UnInit(AFE_TX_CHANn)) ||
        (ADI_DMA_SUCCESS != adi_DMA_UnInit(AFE_RX_CHANn))) {
        return ADI_AFE_ERR_DMA;
    }

    return ADI_AFE_SUCCESS;
}


/*!
 * @brief       Program the Tx DMA channel
 *
 * @param[in]   hDevice     Device handle obtained from adi_AFE_Init().
 * @param[in]   buffer                                  Pointer to the array of words to be sent through the Tx DMA channel.
 * @param[in]   length                                  Buffer size expressed as number of words
 *
 * @return      Status
 *              - #ADI_AFE_SUCCESS                        Call completed successfully.
 *              - #ADI_AFE_ERR_BAD_DEV_HANDLE           Invalid device handle.
 *              - #ADI_AFE_ERR_NOT_INITIALIZED          Device not initialized.
 *              - #ADI_AFE_ERR_ACLKOFF                  ACLK disabled from the clock gate.
 *              - #ADI_AFE_ERR_WRONG_ACLK_FREQUENCY     Programmed ACLK frequency is not the required 16MHz.
 *
 * @details     The function programs the DMA Tx channel.
 *
 * @note        If the <buffer> parameter is NULL, the function will reprogram the Tx DMA channel with the same parameters
 *              as the previous transfer.
 *
 */

ADI_AFE_RESULT_TYPE adi_AFE_ProgramTxDMA(ADI_AFE_DEV_HANDLE const hDevice, const uint32_t *const buffer, uint32_t length) {
    ADI_AFE_RESULT_TYPE result = ADI_AFE_SUCCESS;
    ADI_DMA_TRANSFER_TYPE* pTxD = hDevice->pTxDmaDescriptor;

#ifdef ADI_DEBUG
    if (adi_AFE_InvalidHandle(hDevice)) {
        return ADI_AFE_ERR_BAD_DEV_HANDLE;
    }

    if (adi_AFE_HandleNotInitialized(hDevice)) {
        return ADI_AFE_ERR_NOT_INITIALIZED;
    }

    if (pADI_SYSCLK->CLKCON5 & BITM_SYSCLK_CLKCON5_ACLKOFF) {
        return ADI_AFE_ERR_ACLKOFF;
    }

    if (16000000 != SystemGetClockFrequency(ADI_SYS_CLOCK_ACLK)) {
        return ADI_AFE_ERR_WRONG_ACLK_FREQUENCY;
    }
#endif

    if (buffer) {
        pTxD->pSrcData  = (void *)buffer;
        pTxD->DataLength  = length;
        if (ADI_DMA_SUCCESS != adi_DMA_SubmitTransfer(pTxD)) {
            result = ADI_AFE_ERR_DMA;
        }
    }
    else {
        if (ADI_DMA_SUCCESS != adi_DMA_ReSubmit(pTxD)) {
            result = ADI_AFE_ERR_DMA;
        }
    }

    hDevice->bTxDmaComplete = false;

    return result;
}

/*!
 * @brief       Program the Rx DMA channel
 *
 * @param[in]   hDevice     Device handle obtained from adi_AFE_Init().
 * @param[in]   buffer                                  Pointer to the array of words to be written through the Rx DMA channel
 * @param[in]   length                                  Buffer size expressed as number of halfwords
 *
 * @return      Status
 *              - #ADI_AFE_SUCCESS                        Call completed successfully.
 *              - #ADI_AFE_ERR_BAD_DEV_HANDLE           Invalid device handle.
 *              - #ADI_AFE_ERR_NOT_INITIALIZED          Device not initialized.
 *              - #ADI_AFE_ERR_ACLKOFF                  ACLK disabled from the clock gate.
 *              - #ADI_AFE_ERR_WRONG_ACLK_FREQUENCY     Programmed ACLK frequency is not the required 16MHz.
 *
 * @details     The function programs the DMA Rx channel.
 *
 * @note        If the <buffer> parameter is NULL, the function will reprogram the Rx DMA channel with the same parameters
 *              as the previous transfer.
 *
 */

ADI_AFE_RESULT_TYPE adi_AFE_ProgramRxDMA(ADI_AFE_DEV_HANDLE const hDevice, uint16_t *buffer, uint32_t length) {
    ADI_AFE_RESULT_TYPE result = ADI_AFE_SUCCESS;
    ADI_DMA_TRANSFER_TYPE* pRxD = hDevice->pRxDmaDescriptor;

#ifdef ADI_DEBUG
    if (adi_AFE_InvalidHandle(hDevice)) {
        return ADI_AFE_ERR_BAD_DEV_HANDLE;
    }

    if (adi_AFE_HandleNotInitialized(hDevice)) {
        return ADI_AFE_ERR_NOT_INITIALIZED;
    }

    if (pADI_SYSCLK->CLKCON5 & BITM_SYSCLK_CLKCON5_ACLKOFF) {
        return ADI_AFE_ERR_ACLKOFF;
    }

    if (16000000 != SystemGetClockFrequency(ADI_SYS_CLOCK_ACLK)) {
        return ADI_AFE_ERR_WRONG_ACLK_FREQUENCY;
    }
#endif

    if (buffer) {
        pRxD->pDstData  = buffer;
        pRxD->DataLength  = length;
        if (ADI_DMA_SUCCESS != adi_DMA_SubmitTransfer(pRxD)) {
            result = ADI_AFE_ERR_DMA;
        }
        hDevice->dmaRxTransferCount = length;
    }
    else {
        if (ADI_DMA_SUCCESS != adi_DMA_ReSubmit(pRxD)) {
            result = ADI_AFE_ERR_DMA;
        }
    }

    hDevice->bRxDmaComplete = false;

    return result;
}


/***************************************************************************/
/*   Sequencer Functions                                                   */
/***************************************************************************/


static void crc8(uint8_t *crc, uint32_t word) {
    uint8_t     i;
    uint32_t    data;

    data = word;
    for (i = 0; i < 32; i++) {
        if (((data & 0x80000000) >> 24) ^ (*crc & 0x80)) {
            *crc = *crc << 1;
            *crc = *crc ^ ADI_AFE_SEQ_CRC8_POLYNOMIAL;
        }
        else {
            *crc = *crc << 1;
        }
        data = data << 1;
    }
}

static uint8_t sequenceCRC(const uint32_t *const data, uint32_t size) {
    uint32_t i;
    uint8_t crc;

    crc = 0x01;
    for (i = 0; i < size; i++) {
        crc8(&crc, data[i]);
    }

    return crc;
}

/*!
 * @brief       Returns the current sequencer error code.
 *
 * @param       hDevice                                 Device handle obtained from adi_AFE_Init().
 *
 *
 * @return
 *
 * @details
 *
 */

ADI_AFE_RESULT_TYPE adi_AFE_GetSeqError(ADI_AFE_DEV_HANDLE const hDevice) {

#ifdef ADI_DEBUG
    if (adi_AFE_InvalidHandle(hDevice)) {
        return ADI_AFE_ERR_BAD_DEV_HANDLE;
    }

    if (adi_AFE_HandleNotInitialized(hDevice)) {
        return ADI_AFE_ERR_NOT_INITIALIZED;
    }

    if (pADI_SYSCLK->CLKCON5 & BITM_SYSCLK_CLKCON5_ACLKOFF) {
        return ADI_AFE_ERR_ACLKOFF;
    }

    if (16000000 != SystemGetClockFrequency(ADI_SYS_CLOCK_ACLK)) {
        return ADI_AFE_ERR_WRONG_ACLK_FREQUENCY;
    }
#endif

    return hDevice->seqError;    
}

/*!
 * @brief       Sets the current sequencer error code.
 *
 * @param       hDevice                                 Device handle obtained from adi_AFE_Init().
 *              pError                                  Error of ADI_AFE_RESULT_TYPE.
 *
 *
 * @return
 *
 * @details
 *
 */

ADI_AFE_RESULT_TYPE adi_AFE_SetSeqError(ADI_AFE_DEV_HANDLE const hDevice, ADI_AFE_RESULT_TYPE const error) {

#ifdef ADI_DEBUG
    if (adi_AFE_InvalidHandle(hDevice)) {
        return ADI_AFE_ERR_BAD_DEV_HANDLE;
    }

    if (adi_AFE_HandleNotInitialized(hDevice)) {
        return ADI_AFE_ERR_NOT_INITIALIZED;
    }

    if (pADI_SYSCLK->CLKCON5 & BITM_SYSCLK_CLKCON5_ACLKOFF) {
        return ADI_AFE_ERR_ACLKOFF;
    }

    if (16000000 != SystemGetClockFrequency(ADI_SYS_CLOCK_ACLK)) {
        return ADI_AFE_ERR_WRONG_ACLK_FREQUENCY;
    }
#endif

    hDevice->seqError = error;

    return ADI_AFE_SUCCESS;    
}

/*!
 * @brief       Returns the sequencer state
 *
 * @param[in]   hDevice                                 Device handle obtained from adi_AFE_Init().
 * @param[in]   pState                                  Sequencer state of ADI_AFE_SEQ_STATE_TYPE.
 *
 *
 * @return
 *
 * @details
 *
 */

ADI_AFE_RESULT_TYPE adi_AFE_GetSeqState(ADI_AFE_DEV_HANDLE const hDevice, ADI_AFE_SEQ_STATE_TYPE *const pState) {

#ifdef ADI_DEBUG
    if (adi_AFE_InvalidHandle(hDevice)) {
        return ADI_AFE_ERR_BAD_DEV_HANDLE;
    }

    if (adi_AFE_HandleNotInitialized(hDevice)) {
        return ADI_AFE_ERR_NOT_INITIALIZED;
    }

    if (pADI_SYSCLK->CLKCON5 & BITM_SYSCLK_CLKCON5_ACLKOFF) {
        return ADI_AFE_ERR_ACLKOFF;
    }

    if (16000000 != SystemGetClockFrequency(ADI_SYS_CLOCK_ACLK)) {
        return ADI_AFE_ERR_WRONG_ACLK_FREQUENCY;
    }
#endif

    *pState = hDevice->seqState;

    return ADI_AFE_SUCCESS;
}

/*!
 * @brief       Sets the sequencer state
 *
 * @param[in]   hDevice                                 Device handle obtained from adi_AFE_Init().
 * @param[in]   pState                                  Sequencer state of ADI_AFE_SEQ_STATE_TYPE.
 *
 *
 * @return
 *
 * @details
 *
 */

ADI_AFE_RESULT_TYPE adi_AFE_SetSeqState(ADI_AFE_DEV_HANDLE const hDevice, ADI_AFE_SEQ_STATE_TYPE seqState) {

#ifdef ADI_DEBUG
    if (adi_AFE_InvalidHandle(hDevice)) {
        return ADI_AFE_ERR_BAD_DEV_HANDLE;
    }

    if (adi_AFE_HandleNotInitialized(hDevice)) {
        return ADI_AFE_ERR_NOT_INITIALIZED;
    }

    if (pADI_SYSCLK->CLKCON5 & BITM_SYSCLK_CLKCON5_ACLKOFF) {
        return ADI_AFE_ERR_ACLKOFF;
    }

    if (16000000 != SystemGetClockFrequency(ADI_SYS_CLOCK_ACLK)) {
        return ADI_AFE_ERR_WRONG_ACLK_FREQUENCY;
    }
#endif

    hDevice->seqState = seqState;

    return ADI_AFE_SUCCESS;
}

/*!
 * @brief       Returns the status of the command FIFO ready flag.
 *
 * @param[in]   hDevice                                 Device handle obtained from adi_AFE_Init().
 * @param[in]   pbFlag                                  True if command FIFO ready
 *                                                      False if command FIFO not ready
 *
 * @return      Status
 *              - #ADI_AFE_SUCCESS                      Call completed successfully.
 *              - #ADI_AFE_ERR_BAD_DEV_HANDLE           Invalid device handle.
 *              - #ADI_AFE_ERR_NOT_INITIALIZED          Device not initialized.
 *              - #ADI_AFE_ERR_ACLKOFF                  ACLK disabled from the clock gate.
 *              - #ADI_AFE_ERR_WRONG_ACLK_FREQUENCY     Programmed ACLK frequency is not the required 16MHz.
 *
 * @details     Returns the status of the boolean flag that indicates that the command FIFO
 *              is ready for sequencer operation: either full, or the DMA transfer has finished
 *              because the sequence is less than 8 commands long.
 *
 */

ADI_AFE_RESULT_TYPE adi_AFE_GetCmdFifoReady(ADI_AFE_DEV_HANDLE const hDevice, bool_t *pbFlag) {

#ifdef ADI_DEBUG
    if (adi_AFE_InvalidHandle(hDevice)) {
        return ADI_AFE_ERR_BAD_DEV_HANDLE;
    }

    if (adi_AFE_HandleNotInitialized(hDevice)) {
        return ADI_AFE_ERR_NOT_INITIALIZED;
    }

    if (pADI_SYSCLK->CLKCON5 & BITM_SYSCLK_CLKCON5_ACLKOFF) {
        return ADI_AFE_ERR_ACLKOFF;
    }

    if (16000000 != SystemGetClockFrequency(ADI_SYS_CLOCK_ACLK)) {
        return ADI_AFE_ERR_WRONG_ACLK_FREQUENCY;
    }
#endif

    *pbFlag = hDevice->bCmdFifoReady;

    return ADI_AFE_SUCCESS;
}

/*!
 * @brief       Returns the status of the sequencer finished flag.
 *
 * @param[in]   hDevice                                 Device handle obtained from adi_AFE_Init().
 * @param[in]   pbFlag                                  True if sequencer has finished
 *                                                      False if sequencer has not finished
 *
 * @return      Status
 *              - #ADI_AFE_SUCCESS                      Call completed successfully.
 *              - #ADI_AFE_ERR_BAD_DEV_HANDLE           Invalid device handle.
 *              - #ADI_AFE_ERR_NOT_INITIALIZED          Device not initialized.
 *              - #ADI_AFE_ERR_ACLKOFF                  ACLK disabled from the clock gate.
 *              - #ADI_AFE_ERR_WRONG_ACLK_FREQUENCY     Programmed ACLK frequency is not the required 16MHz.
 *
 * @details     Returns the status of the boolean flag that indicates if the sequencer
 *              finished execution or not.
 *
 */

ADI_AFE_RESULT_TYPE adi_AFE_GetSeqFinished(ADI_AFE_DEV_HANDLE const hDevice, bool_t *pbFlag) {

#ifdef ADI_DEBUG
    if (adi_AFE_InvalidHandle(hDevice)) {
        return ADI_AFE_ERR_BAD_DEV_HANDLE;
    }

    if (adi_AFE_HandleNotInitialized(hDevice)) {
        return ADI_AFE_ERR_NOT_INITIALIZED;
    }

    if (pADI_SYSCLK->CLKCON5 & BITM_SYSCLK_CLKCON5_ACLKOFF) {
        return ADI_AFE_ERR_ACLKOFF;
    }

    if (16000000 != SystemGetClockFrequency(ADI_SYS_CLOCK_ACLK)) {
        return ADI_AFE_ERR_WRONG_ACLK_FREQUENCY;
    }
#endif

    *pbFlag = hDevice->bSeqFinished;

    return ADI_AFE_SUCCESS;
}

/*!
 * @brief       Returns the status of Rx DMA complete flag.
 *
 * @param[in]   hDevice                                 Device handle obtained from adi_AFE_Init().
 * @param[in]   pbFlag                                  True if Rx DMA has completed, or Rx DMA not used
 *                                                      False if Rx DMA has not completed
 *
 * @return      Status
 *              - #ADI_AFE_SUCCESS                      Call completed successfully.
 *              - #ADI_AFE_ERR_BAD_DEV_HANDLE           Invalid device handle.
 *              - #ADI_AFE_ERR_NOT_INITIALIZED          Device not initialized.
 *              - #ADI_AFE_ERR_ACLKOFF                  ACLK disabled from the clock gate.
 *              - #ADI_AFE_ERR_WRONG_ACLK_FREQUENCY     Programmed ACLK frequency is not the required 16MHz.
 *
 * @details
 *
 */

ADI_AFE_RESULT_TYPE adi_AFE_GetRxDmaComplete(ADI_AFE_DEV_HANDLE const hDevice, bool_t *pbFlag) {

#ifdef ADI_DEBUG
    if (adi_AFE_InvalidHandle(hDevice)) {
        return ADI_AFE_ERR_BAD_DEV_HANDLE;
    }

    if (adi_AFE_HandleNotInitialized(hDevice)) {
        return ADI_AFE_ERR_NOT_INITIALIZED;
    }

    if (pADI_SYSCLK->CLKCON5 & BITM_SYSCLK_CLKCON5_ACLKOFF) {
        return ADI_AFE_ERR_ACLKOFF;
    }

    if (16000000 != SystemGetClockFrequency(ADI_SYS_CLOCK_ACLK)) {
        return ADI_AFE_ERR_WRONG_ACLK_FREQUENCY;
    }
#endif

    *pbFlag = (!(hDevice->bUseRxDma && !hDevice->bRxDmaComplete));

    return ADI_AFE_SUCCESS;
}

/*!
 * @brief       Set the status of the RunSequence blocking mode flag.
 *
 * @param[in]   hDevice                                 Device handle obtained from adi_AFE_Init().
 * @param[in]   bFlag                                   True if adi_AFE_RunSequence is to run in blocking mode
 *                                                      False if adi_AFE_RunSequence is to run in non-blocking mode
 *
 * @return      Status
 *              - #ADI_AFE_SUCCESS                      Call completed successfully.
 *              - #ADI_AFE_ERR_BAD_DEV_HANDLE           Invalid device handle.
 *              - #ADI_AFE_ERR_NOT_INITIALIZED          Device not initialized.
 *              - #ADI_AFE_ERR_ACLKOFF                  ACLK disabled from the clock gate.
 *              - #ADI_AFE_ERR_WRONG_ACLK_FREQUENCY     Programmed ACLK frequency is not the required 16MHz.
 *
 * @details     Sets or clears the boolean flag which indicates if the adi_AFE_RunSequence
 *              function is to be run in blocking or non-blocking mode.
 *
 */

ADI_AFE_RESULT_TYPE adi_AFE_SetRunSequenceBlockingMode(ADI_AFE_DEV_HANDLE const hDevice, const bool_t bFlag) {
#ifdef ADI_DEBUG
    if (adi_AFE_InvalidHandle(hDevice)) {
        return ADI_AFE_ERR_BAD_DEV_HANDLE;
    }

    if (adi_AFE_HandleNotInitialized(hDevice)) {
        return ADI_AFE_ERR_NOT_INITIALIZED;
    }
#endif

    hDevice->bRunSequenceBlockingMode = bFlag;

    return ADI_AFE_SUCCESS;
}

/*!
 * @brief       Get the status of the RunSequence blocking mode flag.
 *
 * @param[in]   hDevice                                 Device handle obtained from adi_AFE_Init().
 * @param[in]   pbFlag                                  True if adi_AFE_RunSequence is to run in blocking mode
 *                                                      False if adi_AFE_RunSequence is to run in non-blocking mode
 *
 * @return      Status
 *              - #ADI_AFE_SUCCESS                      Call completed successfully.
 *              - #ADI_AFE_ERR_BAD_DEV_HANDLE           Invalid device handle.
 *              - #ADI_AFE_ERR_NOT_INITIALIZED          Device not initialized.
 *              - #ADI_AFE_ERR_ACLKOFF                  ACLK disabled from the clock gate.
 *              - #ADI_AFE_ERR_WRONG_ACLK_FREQUENCY     Programmed ACLK frequency is not the required 16MHz.
 *
 * @details     Gets the value of the boolean flag which indicates if the adi_AFE_RunSequence
 *              function is to be run in blocking or non-blocking mode.
 *
 */

ADI_AFE_RESULT_TYPE adi_AFE_GetRunSequenceBlockingMode(ADI_AFE_DEV_HANDLE const hDevice, bool_t *const pbFlag) {

#ifdef ADI_DEBUG
    if (adi_AFE_InvalidHandle(hDevice)) {
        return ADI_AFE_ERR_BAD_DEV_HANDLE;
    }

    if (adi_AFE_HandleNotInitialized(hDevice)) {
        return ADI_AFE_ERR_NOT_INITIALIZED;
    }
#endif
    
    *pbFlag = hDevice->bRunSequenceBlockingMode;
    
    return ADI_AFE_SUCCESS;
}

/*!
 * @brief       Initialize the hardware needed for running the sequencer
 *
 * @param[in]   hDevice         Device handle obtained from adi_AFE_Init().
 * @param[in]   txBuffer                                Array of sequencer commands to be sent through the Tx DMA channel
 * @param[in]   rxBuffer                                Destination array for the data, to be written through the Rx DMA channel
 * @param[in]   size                                    Size of the expected data, expressed as number of halfwords
 *
 * @return      Status
 *              - #ADI_AFE_SUCCESS                        Call completed successfully.
 *              - #ADI_AFE_ERR_BAD_DEV_HANDLE           Invalid device handle.
 *              - #ADI_AFE_ERR_NOT_INITIALIZED          Device not initialized.
 *              - #ADI_AFE_ERR_ACLKOFF                  ACLK disabled from the clock gate.
 *              - #ADI_AFE_ERR_WRONG_ACLK_FREQUENCY     Programmed ACLK frequency is not the required 16MHz.
 *
 * @details     The function prepares the hardware for running a sequence. It performs the following tasks:
 *              - program the DMA channels. If size is 0, no Rx DMA transfer is programmed.
 *              - enable the command FIFO and Tx DMA requests
 *              - enable the data FIFO and Rx DMA requests if required
 *              - reset the AFE CRC and command counter registers
 *              - unmask interrupts (CMD_FIFO_FULL and END_OF_SEQ)
 *              - initialize internal statuses
 *
 */

ADI_AFE_RESULT_TYPE adi_AFE_SeqInit(ADI_AFE_DEV_HANDLE const hDevice, const uint32_t *const txBuffer, uint16_t *const rxBuffer, uint32_t size) {
    ADI_AFE_RESULT_TYPE result = ADI_AFE_SUCCESS;

#ifdef ADI_DEBUG
    if (adi_AFE_InvalidHandle(hDevice)) {
        return ADI_AFE_ERR_BAD_DEV_HANDLE;
    }

    if (adi_AFE_HandleNotInitialized(hDevice)) {
        return ADI_AFE_ERR_NOT_INITIALIZED;
    }

    if (pADI_SYSCLK->CLKCON5 & BITM_SYSCLK_CLKCON5_ACLKOFF) {
        return ADI_AFE_ERR_ACLKOFF;
    }

    if (16000000 != SystemGetClockFrequency(ADI_SYS_CLOCK_ACLK)) {
        return ADI_AFE_ERR_WRONG_ACLK_FREQUENCY;
    }

#if (ADI_AFE_CFG_ENABLE_RX_DMA_DUAL_BUFFER_SUPPORT == 0)
    /* If Rx DMA dual buffer mode is not enabled, the DMA transfer is limited at a maximum of 1024 cycles. */
    if (size > 1024) {
       return ADI_AFE_ERR_RX_DMA_SIZE_TOO_BIG;
   }
#endif

#endif

    /*   Rx DMA   */

    /* If size > 0, then use data FIFO DMA channel */
    if (size) {
        hDevice->bUseRxDma = true;
        hDevice->pRxBuffer = rxBuffer;

#if (ADI_AFE_CFG_ENABLE_RX_DMA_DUAL_BUFFER_SUPPORT == 1)
        if (size <= hDevice->dmaRxBufferMaxSize[0]) {
            /* Single DMA cycle */
            adi_AFE_ProgramRxDMA(hDevice, &rxBuffer[0], size);

            hDevice->dmaRxRemaining = 0;
        }
        else {
            /* Multiple DMA cycles */
            adi_AFE_ProgramRxDMA(hDevice, &rxBuffer[0], hDevice->dmaRxBufferMaxSize[0]);

            hDevice->dmaRxRemaining = size - hDevice->dmaRxBufferMaxSize[0];
            hDevice->dmaRxBufferIndex = 0;
        }
#else
        /* Single DMA cycle */
        adi_AFE_ProgramRxDMA(hDevice, &rxBuffer[0], size);
#endif

        ADI_ENABLE_INT(DMA_AFE_RX_IRQn);
    }
    else {
        hDevice->bUseRxDma = false;
        /* If not using the Rx DMA, mark the transfer as complete */
        hDevice->bRxDmaComplete = true;
    }

    /*   Tx DMA   */

    adi_AFE_ProgramTxDMA(hDevice, &txBuffer[1], (txBuffer[0] & 0xFFFF0000) >> 16);
    /* Enable DMA done interrupt */
    ADI_ENABLE_INT(DMA_AFE_TX_IRQn);

    hDevice->bCmdFifoReady = false;
    hDevice->bSeqFinished = false;

    hDevice->seqError = ADI_AFE_SUCCESS;

    /* Initialize CRC and command counter */
    hDevice->pAFE->AFE_SEQ_COUNT = 0x00000000;

    /* Enable FIFO */
    fifoEnable(hDevice);

    dmaEnable(hDevice);

    ADI_ENTER_CRITICAL_REGION();

        /* Clear CMD_FIFO interrupts */
        adi_AFE_ClearInterruptSource(hDevice, 
                                     ADI_AFE_INT_GROUP_CMD_FIFO, 
                                     (BITM_AFE_AFE_CMD_FIFO_IEN_CMD_FIFO_FULL_IEN |
                                      BITM_AFE_AFE_CMD_FIFO_IEN_CMD_FIFO_UDF_IEN |
                                      BITM_AFE_AFE_CMD_FIFO_IEN_END_OF_SEQ_IEN));
        
        /* Enable CMD_FIFO interrupts */
        adi_AFE_EnableInterruptSource(hDevice, 
                                      ADI_AFE_INT_GROUP_CMD_FIFO, 
                                      (BITM_AFE_AFE_CMD_FIFO_IEN_CMD_FIFO_FULL_IEN |
                                       BITM_AFE_AFE_CMD_FIFO_IEN_CMD_FIFO_UDF_IEN |
                                       BITM_AFE_AFE_CMD_FIFO_IEN_END_OF_SEQ_IEN),
                                      true);

    ADI_EXIT_CRITICAL_REGION();

    return result;
}

/*!
 * @brief       Start the sequencer
 *
 * @param[in]   hDevice                                 Device handle obtained from adi_AFE_Init().
 *
 * @return      Status
 *              - #ADI_AFE_SUCCESS                        Call completed successfully.
 *              - #ADI_AFE_ERR_BAD_DEV_HANDLE           Invalid device handle.
 *              - #ADI_AFE_ERR_NOT_INITIALIZED          Device not initialized.
 *              - #ADI_AFE_ERR_ACLKOFF                  ACLK disabled from the clock gate.
 *              - #ADI_AFE_ERR_WRONG_ACLK_FREQUENCY     Programmed ACLK frequency is not the required 16MHz.
 *
 * @details     The function starts the sequencer by writing a 1 to SEQ_EN.
 *
 */

ADI_AFE_RESULT_TYPE adi_AFE_SeqStart(ADI_AFE_DEV_HANDLE const hDevice) {
    ADI_AFE_RESULT_TYPE result = ADI_AFE_SUCCESS;

#ifdef ADI_DEBUG
    if (adi_AFE_InvalidHandle(hDevice)) {
        return ADI_AFE_ERR_BAD_DEV_HANDLE;
    }

    if (adi_AFE_HandleNotInitialized(hDevice)) {
        return ADI_AFE_ERR_NOT_INITIALIZED;
    }

    if (pADI_SYSCLK->CLKCON5 & BITM_SYSCLK_CLKCON5_ACLKOFF) {
        return ADI_AFE_ERR_ACLKOFF;
    }

    if (16000000 != SystemGetClockFrequency(ADI_SYS_CLOCK_ACLK)) {
        return ADI_AFE_ERR_WRONG_ACLK_FREQUENCY;
    }
#endif

    ADI_ENTER_CRITICAL_REGION();

        /* Start sequencer */
        hDevice->pAFE->AFE_SEQ_CFG |= BITM_AFE_AFE_SEQ_CFG_SEQ_EN;

    ADI_EXIT_CRITICAL_REGION();

    return result;
}

/*!
 * @brief       Stop the sequencer
 *
 * @param[in]   hDevice                                 Device handle obtained from adi_AFE_Init().
 *
 * @return      Status
 *              - #ADI_AFE_SUCCESS                        Call completed successfully.
 *              - #ADI_AFE_ERR_BAD_DEV_HANDLE           Invalid device handle.
 *              - #ADI_AFE_ERR_NOT_INITIALIZED          Device not initialized.
 *              - #ADI_AFE_ERR_ACLKOFF                  ACLK disabled from the clock gate.
 *              - #ADI_AFE_ERR_WRONG_ACLK_FREQUENCY     Programmed ACLK frequency is not the required 16MHz.
 *
 * @details     The function stops the sequencer by writing a 0 to SEQ_EN. It also disables both FIFOs and DMA requests.
 *
 */

ADI_AFE_RESULT_TYPE adi_AFE_SeqStop(ADI_AFE_DEV_HANDLE const hDevice) {
    ADI_AFE_RESULT_TYPE result = ADI_AFE_SUCCESS;

#ifdef ADI_DEBUG
    if (adi_AFE_InvalidHandle(hDevice)) {
        return ADI_AFE_ERR_BAD_DEV_HANDLE;
    }

    if (adi_AFE_HandleNotInitialized(hDevice)) {
        return ADI_AFE_ERR_NOT_INITIALIZED;
    }

    if (pADI_SYSCLK->CLKCON5 & BITM_SYSCLK_CLKCON5_ACLKOFF) {
        return ADI_AFE_ERR_ACLKOFF;
    }

    if (16000000 != SystemGetClockFrequency(ADI_SYS_CLOCK_ACLK)) {
        return ADI_AFE_ERR_WRONG_ACLK_FREQUENCY;
    }
#endif

    /* The last command in the sequence should stop the sequencer (SEQ_EN = 0). */
    /* If that is not the case or if an error encountered, report an error.     */
    /* This is better than forcing a sequencer disable, because that would      */
    /* also stop the timeout counter, which may not be desirable.               */
    if (hDevice->pAFE->AFE_SEQ_CFG & BITM_AFE_AFE_SEQ_CFG_SEQ_EN) {
        result = ADI_AFE_ERR_SEQ_NOT_DISABLED;
    }

    if (!result) {
        result = fifoDisable(hDevice);
    }

    if (!result) {
        result = dmaDisable(hDevice);
    }

    return result;
}

/*!
 * @brief       Abort the sequencer
 *
 * @param[in]   hDevice                                 Device handle obtained from adi_AFE_Init().
 *
 * @return      Status
 *              - #ADI_AFE_SUCCESS                        Call completed successfully.
 *              - #ADI_AFE_ERR_BAD_DEV_HANDLE           Invalid device handle.
 *              - #ADI_AFE_ERR_NOT_INITIALIZED          Device not initialized.
 *              - #ADI_AFE_ERR_ACLKOFF                  ACLK disabled from the clock gate.
 *              - #ADI_AFE_ERR_WRONG_ACLK_FREQUENCY     Programmed ACLK frequency is not the required 16MHz.
 *
 * @details     The function stops the sequencer by writing a 0 to SEQ_EN, disables DMA requests
 *              and disables hardware accelerators
 *
 *              Its intended usage is following errors conditions during sequencer execution.
 *
 */

ADI_AFE_RESULT_TYPE adi_AFE_SeqAbort(ADI_AFE_DEV_HANDLE const hDevice) {
    ADI_AFE_RESULT_TYPE result = ADI_AFE_SUCCESS;

#ifdef ADI_DEBUG
    if (adi_AFE_InvalidHandle(hDevice)) {
        return ADI_AFE_ERR_BAD_DEV_HANDLE;
    }

    if (adi_AFE_HandleNotInitialized(hDevice)) {
        return ADI_AFE_ERR_NOT_INITIALIZED;
    }

    if (pADI_SYSCLK->CLKCON5 & BITM_SYSCLK_CLKCON5_ACLKOFF) {
        return ADI_AFE_ERR_ACLKOFF;
    }

    if (16000000 != SystemGetClockFrequency(ADI_SYS_CLOCK_ACLK)) {
        return ADI_AFE_ERR_WRONG_ACLK_FREQUENCY;
    }
#endif

    /* Stop sequencer                                                               */
    /* It should be stopped by the last command executed if no error encountered    */
    /* This takes care of error cases (e.g. command FIFO underflow)                 */
    hDevice->pAFE->AFE_SEQ_CFG &= ~BITM_AFE_AFE_SEQ_CFG_SEQ_EN;

    /* Only disable DMA requests                                                            */
    /* Don't reset FIFOs, otherwise we lose information that may be useful for debugging    */
    dmaDisable(hDevice);

    /* Disable hardware accelerators */
    hDevice->pAFE->AFE_CFG &= ~(BITM_AFE_AFE_CFG_SUPPLY_LPF_EN |
                                BITM_AFE_AFE_CFG_DFT_EN |
                                BITM_AFE_AFE_CFG_WAVEGEN_EN |
                                BITM_AFE_AFE_CFG_TEMP_CONV_EN |
                                BITM_AFE_AFE_CFG_ADC_CONV_EN);

    return result;
}

/*!
 * @brief       Check the sequencer execution was successful.
 *
 * @param[in]   hDevice                                 Device handle obtained from adi_AFE_Init().
 * @param[in]   txBuffer                                Array of sequencer commands to be sent through the Tx DMA channel.
 *
 * @return      Status
 *              - #ADI_AFE_SUCCESS                        Call completed successfully.
 *              - #ADI_AFE_ERR_BAD_DEV_HANDLE           Invalid device handle.
 *              - #ADI_AFE_ERR_NOT_INITIALIZED          Device not initialized.
 *              - #ADI_AFE_ERR_ACLKOFF                  ACLK disabled from the clock gate.
 *              - #ADI_AFE_ERR_WRONG_ACLK_FREQUENCY     Programmed ACLK frequency is not the required 16MHz.
 *
 * @details     The function checks that the command count and CRC calculated by the AFE sequencer
 *              match the values programmed in the sequencer commands array.
 *
 */

ADI_AFE_RESULT_TYPE adi_AFE_SeqCheck(ADI_AFE_DEV_HANDLE const hDevice, const uint32_t *const txBuffer) {
    ADI_AFE_RESULT_TYPE result = ADI_AFE_SUCCESS;
    uint8_t             crc;

#ifdef ADI_DEBUG
    if (adi_AFE_InvalidHandle(hDevice)) {
        return ADI_AFE_ERR_BAD_DEV_HANDLE;
    }

    if (adi_AFE_HandleNotInitialized(hDevice)) {
        return ADI_AFE_ERR_NOT_INITIALIZED;
    }

    if (pADI_SYSCLK->CLKCON5 & BITM_SYSCLK_CLKCON5_ACLKOFF) {
        return ADI_AFE_ERR_ACLKOFF;
    }

    if (16000000 != SystemGetClockFrequency(ADI_SYS_CLOCK_ACLK)) {
        return ADI_AFE_ERR_WRONG_ACLK_FREQUENCY;
    }
#endif

    /* For sequences that change dynamically, the CRC needs to be recalculated  */
    /* in software or it will fail                                              */
    if (hDevice->bSoftwareCRC) {
        crc = sequenceCRC(&txBuffer[1], (txBuffer[0] & 0xFFFF0000) >> 16);
    }
    else {
        crc = txBuffer[0] & 0x000000FF;
    }

    /* The CRC check is commented out while debugging sequences, so code changes don't need a CRC recalculation */
    /* Command count check left in, as the count information needs to be correct for the DMA programming        */
    /* Check CRC and command counter                                                                            */
    if ((hDevice->pAFE->AFE_SEQ_CRC != crc) || (hDevice->pAFE->AFE_SEQ_COUNT != ((txBuffer[0] & 0xFFFF0000) >> 16))) {
        return ADI_AFE_ERR_SEQ_CHECK;
    }

    return result;
}

/*!
 * @brief       Run a sequence
 *
 * @param[in]   hDevice                                 Device handle obtained from adi_AFE_Init().
 * @param[in]   txBuffer                                Array of sequencer commands to be sent through the Tx DMA channel
 * @param[in]   rxBuffer                                Destination array for the data, to be written through the Rx DMA channel
 * @param[in]   size                                    Size of the expected data, expressed as number of halfwords
 *
 * @return      Status
 *              - #ADI_AFE_SUCCESS                      Call completed successfully.
 *              - #ADI_AFE_ERR_BAD_DEV_HANDLE           Invalid device handle.
 *              - #ADI_AFE_ERR_NOT_INITIALIZED          Device not initialized.
 *              - #ADI_AFE_ERR_ACLKOFF                  ACLK disabled from the clock gate.
 *              - #ADI_AFE_ERR_WRONG_ACLK_FREQUENCY     Programmed ACLK frequency is not the required 16MHz.
 *
 * @details     Before enabling the sequencer the command FIFO needs to be serviced. Either the FIFO is full (if the sequence
 *              is 8 commands or more), or the Tx DMA channel has completed the programmed transfers (sequences of less than
 *              8 commands). This guarantees the command FIFO will not underflow on sequencer enable.
 *
 * @details     The function can be run in either blocking (default) or non-blocking mode. To enable non-blocking mode, the
 *              adi_AFE_SetRunSequenceBlockingMode() function must be called before the first call to adi_AFE_RunSequence().
 *              
 *              Blocking mode: 
 *              The function does not exit normally until the sequencer has finished execution, and the Rx DMA transfers have
 *              completed (if the Rx DMA channel is used for the current sequence). Low power mode (CORE_SLEEP) is entered while
 *              the sequence is executing until exectution finishes and the Rx DMA transefers are complete. The function performs 
 *              all the steps needed to run the sequencer. It includes waiting for events required for the sequencer operation.
 *
 *              Non-Blocking mode:
 *              The function initializes and starts the sequence and then exits. Interrupts/callbacks or MMR polling should be
 *              used at the application level to determine when the sequencer and Rx DMA execution has completed. The seqState
 *              in the AFE device handle will be set to ADI_AFE_SEQ_STATE_RUNNING before the function exits and should also be 
 *              updated at the application level.
 *
 * @details     If an error occurs, the waits can be skipped by setting the seqError variable, typically in an interrupt handler.
 *
 */
ADI_AFE_RESULT_TYPE adi_AFE_RunSequence(ADI_AFE_DEV_HANDLE const hDevice, const uint32_t *const txBuffer, uint16_t *const rxBuffer, uint32_t size) {
    ADI_AFE_RESULT_TYPE result = ADI_AFE_SUCCESS;
    bool_t              bFlag;

#ifdef ADI_DEBUG
    if (adi_AFE_InvalidHandle(hDevice)) {
        return ADI_AFE_ERR_BAD_DEV_HANDLE;
    }

    if (adi_AFE_HandleNotInitialized(hDevice)) {
        return ADI_AFE_ERR_NOT_INITIALIZED;
    }

    if (pADI_SYSCLK->CLKCON5 & BITM_SYSCLK_CLKCON5_ACLKOFF) {
        return ADI_AFE_ERR_ACLKOFF;
    }

    if (16000000 != SystemGetClockFrequency(ADI_SYS_CLOCK_ACLK)) {
        return ADI_AFE_ERR_WRONG_ACLK_FREQUENCY;
    }
#endif

    /* Initialize the sequencer */
    if (ADI_AFE_SUCCESS != (result = adi_AFE_SeqInit(hDevice, txBuffer, rxBuffer, size))) {
        adi_AFE_SeqAbort(hDevice);

        return result;
    }

    /* Update state */
    hDevice->seqState = ADI_AFE_SEQ_STATE_WAITING_FOR_CMD_FIFO;

    /* Wait until the command FIFO is ready for sequencer operation, or we have an error event */
    /* Because this will execute very fast, doing a while loop is less overhead than            */
    /* pending a semaphore or entering a low power mode.                                        */
    while (!hDevice->bCmdFifoReady && (hDevice->seqError == ADI_AFE_SUCCESS))
        ;

    /* Check for error event in interrupt handler */
    if (hDevice->seqError) {
        adi_AFE_SeqAbort(hDevice);

        return hDevice->seqError;
     }

    /* Update state */
    hDevice->seqState = ADI_AFE_SEQ_STATE_INITIALIZED;

#if (ADI_CFG_ENABLE_RTOS_SUPPORT != 1)

    hDevice->bInterruptFlag = false;

#endif /* ADI_CFG_ENABLE_RTOS_SUPPORT */

    /* Start the sequencer */
    if (ADI_AFE_SUCCESS != (result = adi_AFE_SeqStart(hDevice))) {
        adi_AFE_SeqAbort(hDevice);

        return result;
    }

    /* Update state */
    hDevice->seqState = ADI_AFE_SEQ_STATE_RUNNING;

    if (hDevice->bRunSequenceBlockingMode)
    {

        /* Progress to next step if sequencer is finished and Rx DMA transfer completed (if Rx DMA is used),    */
        /* or we have an error event                                                                            */

        do {
#if (ADI_CFG_ENABLE_RTOS_SUPPORT == 1)

        if (adi_osal_SemPend(hDevice->hSeqSem, ADI_OSAL_TIMEOUT_FOREVER) != ADI_OSAL_SUCCESS) {
            adi_AFE_SeqAbort(hDevice);

            return ADI_AFE_ERR_SEMAPHORE_FAILED;
        }

#else
            SystemEnterLowPowerMode(ADI_SYS_MODE_CORE_SLEEP, &hDevice->bInterruptFlag, 0);

#endif /* ADI_CFG_ENABLE_RTOS_SUPPORT */

            /* Check that both data FIFO DMA and the sequence execution are complete.   */
            /* If either has not finished, then go back to pending/low power mode.      */
            /* As we're dealing with variables modified within interrupt handlers,      */
            /* we need critical region macros.                                          */

            ADI_ENTER_CRITICAL_REGION();
           
            bFlag = false;

            result = adi_AFE_GetRxDmaComplete(hDevice, &bFlag);
            if ((ADI_AFE_SUCCESS == result) && bFlag) {
                result = adi_AFE_GetSeqFinished(hDevice, &bFlag);
            }

#if (ADI_CFG_ENABLE_RTOS_SUPPORT != 1)

            if ((ADI_AFE_SUCCESS == result) && (!bFlag)) {
                /* Either Rx DMA is not complete, or Rx DMA is complete, but sequencer  */
                /* is not finished, need to go back to pending/low power mode.          */
                hDevice->bInterruptFlag = false;
        }

#endif /* ADI_CFG_ENABLE_RTOS_SUPPORT */

            ADI_EXIT_CRITICAL_REGION();   

        } while ((ADI_AFE_SUCCESS == result) && (!bFlag) && (!hDevice->seqError));

        /* Update state */
        hDevice->seqState = ADI_AFE_SEQ_STATE_FINISHED;

        /* Check for error event in interrupt handler */
        if (hDevice->seqError) {
            adi_AFE_SeqAbort(hDevice);

            return hDevice->seqError;
         }

        /* Stop the sequencer */
        if (ADI_AFE_SUCCESS != (result = adi_AFE_SeqStop(hDevice))) {
            adi_AFE_SeqAbort(hDevice);

            return result;
        }

        /* Update state */
        hDevice->seqState = ADI_AFE_SEQ_STATE_IDLE;

        /* Check that the CRC returned by the sequencer matches that in [7:0] of    */
        /* the safety word (txBuffer[0]).                                           */
        if (ADI_AFE_SUCCESS != (result = adi_AFE_SeqCheck(hDevice, txBuffer))) {
            return result;
        }
    }

    return result;
}


/*!
 * @brief       Enable software CRC calculation.
 *
 * @param[in]   hDevice                                 Device handle obtained from adi_AFE_Init().
 * @param[in]   bFlag                                   True to use software CRC
 *                                                      False to use stored CRC values
 *
 * @return      Status
 *              - #ADI_AFE_SUCCESS                      Call completed successfully.
 *
 * @details     Under normal circumstances, the CRC is stored in Flash together with the sequence
 *              it belongs to (hardware CRC). However, sometimes it may be needed to modify the
 *              sequence dynamically (e.g. the frequency control word in a frequency sweep),
 *              and the programmed CRC cannot verify the integrity of the sequence anymore.
 *              If the software CRC is enabled, the CRC of the current sequence is calculated
 *              on-the-fly, and the hardware CRC is ignored.
 *              Warning: using the hardware CRC can detect error such as Flash read errors, which the
 *              software CRC cannot detect. Usage of software CRC is discouraged.
 *
 */

ADI_AFE_RESULT_TYPE adi_AFE_EnableSoftwareCRC(ADI_AFE_DEV_HANDLE const hDevice, const bool_t bFlag) {
    ADI_AFE_RESULT_TYPE result = ADI_AFE_SUCCESS;

    hDevice->bSoftwareCRC = bFlag;

    return result;
}

#if (ADI_CFG_ENABLE_RTOS_SUPPORT == 0)
/*!
 * @brief       Gets the flag for low power mode.
 *
 * @param       hDevice                                 Device handle obtained from adi_AFE_Init().
 *              pError                                  Error of ADI_AFE_RESULT_TYPE.
 *
 *
 * @return      *bInterruptFlag                         Pointer to the flag used to enter low power mode.
 *
 * @details     Gets the value of the private flag, bInterruptFlag, used by the adi_AFE_RunSequence() function 
 *              when entering low power mode. Because this flag is private, it cannot be used directly to,
 *              for example, exit low power mode from another interrupt handler.
 *
 */

ADI_AFE_RESULT_TYPE adi_AFE_GetLowPowerModeFlag(ADI_AFE_DEV_HANDLE const hDevice, bool_t *pbFlag) {

#ifdef ADI_DEBUG
    if (adi_AFE_InvalidHandle(hDevice)) {
        return ADI_AFE_ERR_BAD_DEV_HANDLE;
    }

    if (adi_AFE_HandleNotInitialized(hDevice)) {
        return ADI_AFE_ERR_NOT_INITIALIZED;
    }
#endif
    
    *pbFlag = hDevice->bInterruptFlag;

    return ADI_AFE_SUCCESS;    
}

/*!
 * @brief       Sets the flag for low power mode.
 *
 * @param       hDevice                                 Device handle obtained from adi_AFE_Init().
 *              pError                                  Error of ADI_AFE_RESULT_TYPE.
 *
 *
 * @return      *bInterruptFlag                         Pointer to the flag used to enter low power mode.
 *
 * @details     Sets the private flag, bInterruptFlag, used by the adi_AFE_RunSequence() function 
 *              when entering low power mode. Because this flag is private, it cannot be used directly to,
 *              for example, exit low power mode from another interrupt handler.
 *
 */

ADI_AFE_RESULT_TYPE adi_AFE_SetLowPowerModeFlag(ADI_AFE_DEV_HANDLE const hDevice, bool_t *pbFlag) {

#ifdef ADI_DEBUG
    if (adi_AFE_InvalidHandle(hDevice)) {
        return ADI_AFE_ERR_BAD_DEV_HANDLE;
    }

    if (adi_AFE_HandleNotInitialized(hDevice)) {
        return ADI_AFE_ERR_NOT_INITIALIZED;
    }
#endif
    
    hDevice->bInterruptFlag = *pbFlag;

    return ADI_AFE_SUCCESS;    
}
#endif /* ADI_CFG_ENABLE_RTOS_SUPPORT */

/*!
 * @brief       Checks if the AFE device is valid.
 *
 * @param       hDevice                                 Device handle obtained from adi_AFE_Init().
 *
 *
 * @return      Status
 *              - #ADI_AFE_SUCCESS                      Call completed successfully.
 *              - #ADI_AFE_ERR_BAD_DEV_HANDLE           Invalid device handle.
 *
 * @details
 *
 */

ADI_AFE_RESULT_TYPE adi_AFE_InvalidHandle(ADI_AFE_DEV_HANDLE const hDevice) {

    if (pAFE_DevData != hDevice) 
    {
        return ADI_AFE_ERR_BAD_DEV_HANDLE;
    }

    return ADI_AFE_SUCCESS;    
}

/*!
 * @brief       Checks if the AFE device is not initialized.
 *
 * @param       hDevice                                 Device handle obtained from adi_AFE_Init().
 *
 *
 * @return      Status
 *              - #ADI_AFE_SUCCESS                      Call completed successfully.
 *              - #ADI_AFE_ERR_NOT_INITIALIZED          Device not initialized.
 *
 * @details
 *
 */

ADI_AFE_RESULT_TYPE adi_AFE_HandleNotInitialized(ADI_AFE_DEV_HANDLE const hDevice) {

    if (!hDevice->bInitialized) 
    {
        return ADI_AFE_ERR_NOT_INITIALIZED;
    }
    
    return ADI_AFE_SUCCESS;    
}

/*!
 * @brief       Set the RCAL value.
 *
 * @param       hDevice                                 Device handle obtained from adi_AFE_Init().
 *              rcal                                    Value of RCAL in ohms.
 *
 *
 * @return      Status
 *              - #ADI_AFE_SUCCESS                      Call completed successfully.
 *              - #ADI_AFE_ERR_BAD_DEV_HANDLE           Invalid device handle.
 *              - #ADI_AFE_ERR_NOT_INITIALIZED          Device not initialized.
 *
 * @details
 *
 */

ADI_AFE_RESULT_TYPE adi_AFE_SetRcal(ADI_AFE_DEV_HANDLE const hDevice, uint32_t rcal) {

#ifdef ADI_DEBUG
    if (adi_AFE_InvalidHandle(hDevice)) {
        return ADI_AFE_ERR_BAD_DEV_HANDLE;
    }

    if (adi_AFE_HandleNotInitialized(hDevice)) {
        return ADI_AFE_ERR_NOT_INITIALIZED;
    }
#endif

    hDevice->rcal = rcal;
    
    return ADI_AFE_SUCCESS;    
}

/*!
 * @brief       Get the RCAL value.
 *
 * @param[in]   hDevice                                 Device handle obtained from adi_AFE_Init().
 *
 * @param[out]  Rcal                                    Value of RCAL in ohms.
 *
 *
 * @return      Status
 *              - #ADI_AFE_SUCCESS                      Call completed successfully.
 *              - #ADI_AFE_ERR_BAD_DEV_HANDLE           Invalid device handle.
 *              - #ADI_AFE_ERR_NOT_INITIALIZED          Device not initialized.
 *
 * @details
 *
 */

ADI_AFE_RESULT_TYPE adi_AFE_GetRcal(ADI_AFE_DEV_HANDLE const hDevice, uint32_t* pRcal) {

#ifdef ADI_DEBUG
    if (adi_AFE_InvalidHandle(hDevice)) {
        return ADI_AFE_ERR_BAD_DEV_HANDLE;
    }

    if (adi_AFE_HandleNotInitialized(hDevice)) {
        return ADI_AFE_ERR_NOT_INITIALIZED;
    }
#endif
    
    *pRcal =  hDevice->rcal;    
    
    return ADI_AFE_SUCCESS;    
}

/*!
 * @brief       Set the RTIA value.
 *
 * @param       hDevice                                 Device handle obtained from adi_AFE_Init().
 *              rtia                                    Value of RTIA in ohms.
 *
 *
 * @return      Status
 *              - #ADI_AFE_SUCCESS                      Call completed successfully.
 *              - #ADI_AFE_ERR_BAD_DEV_HANDLE           Invalid device handle.
 *              - #ADI_AFE_ERR_NOT_INITIALIZED          Device not initialized.
 *
 * @details
 *
 */

ADI_AFE_RESULT_TYPE adi_AFE_SetRtia(ADI_AFE_DEV_HANDLE const hDevice, uint32_t rtia) {

#ifdef ADI_DEBUG
    if (adi_AFE_InvalidHandle(hDevice)) {
        return ADI_AFE_ERR_BAD_DEV_HANDLE;
    }

    if (adi_AFE_HandleNotInitialized(hDevice)) {
        return ADI_AFE_ERR_NOT_INITIALIZED;
    }
#endif

    hDevice->rtia = rtia;
    
    return ADI_AFE_SUCCESS;    
}

/*!
 * @brief       Get the RTIA value.
 *
 * @param[in]   hDevice                                 Device handle obtained from adi_AFE_Init().
 *
 * @param[out]  Rtia                                    Value of RTIA in ohms.
 *
 *
 * @return      Status
 *              - #ADI_AFE_SUCCESS                      Call completed successfully.
 *              - #ADI_AFE_ERR_BAD_DEV_HANDLE           Invalid device handle.
 *              - #ADI_AFE_ERR_NOT_INITIALIZED          Device not initialized.
 *
 * @details
 *
 */

ADI_AFE_RESULT_TYPE adi_AFE_GetRtia(ADI_AFE_DEV_HANDLE const hDevice, uint32_t* pRtia) {

#ifdef ADI_DEBUG
    if (adi_AFE_InvalidHandle(hDevice)) {
        return ADI_AFE_ERR_BAD_DEV_HANDLE;
    }

    if (adi_AFE_HandleNotInitialized(hDevice)) {
        return ADI_AFE_ERR_NOT_INITIALIZED;
    }
#endif
    
    *pRtia =  hDevice->rtia;    
    
    return ADI_AFE_SUCCESS;
}


/***************************************************************************/
/*   High-Level Functions                                                  */
/***************************************************************************/

/*!
 * @brief       Application callback registration for the AFE transmit DMA done interrupt handler.
 *
 * @param[in]   hDevice                                 Device handle obtained from adi_AFE_Init().
 * @param[in]   cbFunc                                  Application callback address; the function to call.
 * @param[in]   cbWatch                                 \b Ignored.
 *
 * @return      Status
 *              - #ADI_AFE_SUCCESS                      Call completed successfully.
 *              - #ADI_AFE_ERR_BAD_DEV_HANDLE           Invalid device handle.
 *              - #ADI_AFE_ERR_NOT_INITIALIZED          Device not initialized.
 *              - #ADI_AFE_ERR_ACLKOFF                  ACLK disabled from the clock gate.
 *              - #ADI_AFE_ERR_WRONG_ACLK_FREQUENCY     Programmed ACLK frequency is not the required 16MHz.
 *              - #ADI_AFE_ERR_CALLBACK_OVERWRITE       Attempt to overwrite existing callback.
 *
 * @details     Registers an application-defined callback function with the DMA done interrupt handler for the AFE transmit channel (DMA_AFE_TX_IRQn).

 *              The callback to the application is made in context of the originating interrupt (i.e., the AFE
 *              interrupt handler that is registered in the system's interrupt vector table).  Extended processing
 *              during the callback (an extension of the AFE interrupt handler) is discouraged so as to avoid lower-priority
 *              interrupt blocking.  Also, any register read-modify-write operations should be protected using the
 *              ADI_ENTER_CRITICAL_REGION()/ADI_EXIT_CRITICAL_REGION() pair to prevent higher-priority interrupts from modifying
 *              said register during the read-modify-write operation.
 *
 * @note        CALLBACKS: AFE interrupt callbacks are \a disabled by default during the API initialization (#adi_AFE_Init()).
 *              The application may use the #adi_AFE_RegisterCallbackOnTransmitDMA() API to register an application-defined
 *              callback function with the interrupt handler that will be called in response to a DMA done interrupt for the AFE
 *              transmit channel. When a DMA done interrupt occurs, the interrupt handler will make the callback to the application
 *              for application-based interrupt handling. The application callback should \a avoid \a extended \a processing during
 *              the callback as the callback is made context of the initiating interrupt and will block lower-priority interrupts.
 *              If extended application-level interrupt processing is required, the application should schedule it for the main
 *              application loop and exit the callback as soon as possible.\n\n
 *
 * @note        The value of the cbWatch parameter is ignored, because there is a single interrupt source that needs watching.
 *
 */

ADI_AFE_RESULT_TYPE adi_AFE_RegisterCallbackOnTransmitDMA(ADI_AFE_DEV_HANDLE hDevice, ADI_CALLBACK cbFunc, uint32_t cbWatch) {
    ADI_AFE_RESULT_TYPE result = ADI_AFE_SUCCESS;

#ifdef ADI_DEBUG
    if (adi_AFE_InvalidHandle(hDevice)) {
        return ADI_AFE_ERR_BAD_DEV_HANDLE;
    }

    if (adi_AFE_HandleNotInitialized(hDevice)) {
        return ADI_AFE_ERR_NOT_INITIALIZED;
    }

    if (pADI_SYSCLK->CLKCON5 & BITM_SYSCLK_CLKCON5_ACLKOFF) {
        return ADI_AFE_ERR_ACLKOFF;
    }

    if (16000000 != SystemGetClockFrequency(ADI_SYS_CLOCK_ACLK)) {
        return ADI_AFE_ERR_WRONG_ACLK_FREQUENCY;
    }

    if (cbFunc && hDevice->cbTxDmaFcn) {
        return ADI_AFE_ERR_CALLBACK_OVERWRITE;
    }
#endif

    /* DMA callbacks have no watch */
    hDevice->cbTxDmaFcn = cbFunc;

    return result;
}

/*!
 * @brief       Application callback registration for the AFE receive DMA done interrupt handler.
 *
 * @param[in]   hDevice                                 Device handle obtained from adi_AFE_Init().
 * @param[in]   cbFunc                                  Application callback address; the function to call.
 * @param[in]   cbWatch                                 \b Ignored.
 *
 * @return      Status
 *              - #ADI_AFE_SUCCESS                      Call completed successfully.
 *              - #ADI_AFE_ERR_BAD_DEV_HANDLE           Invalid device handle.
 *              - #ADI_AFE_ERR_NOT_INITIALIZED          Device not initialized.
 *              - #ADI_AFE_ERR_ACLKOFF                  ACLK disabled from the clock gate.
 *              - #ADI_AFE_ERR_WRONG_ACLK_FREQUENCY     Programmed ACLK frequency is not the required 16MHz.
 *              - #ADI_AFE_ERR_CALLBACK_OVERWRITE       Attempt to overwrite existing callback.
 *
 * @details     Registers an application-defined callback function with the DMA done interrupt handler for the AFE receive channel (DMA_AFE_RX_IRQn).
 *
 *              The callback to the application is made in context of the originating interrupt (i.e., the AFE
 *              interrupt handler that is registered in the system's interrupt vector table).  Extended processing
 *              during the callback (an extension of the AFE interrupt handler) is discouraged so as to avoid lower-priority
 *              interrupt blocking.  Also, any register read-modify-write operations should be protected using the
 *              ADI_ENTER_CRITICAL_REGION()/ADI_EXIT_CRITICAL_REGION() pair to prevent higher-priority interrupts from modifying
 *              said register during the read-modify-write operation.
 *
 * @note        CALLBACKS: AFE interrupt callbacks are \a disabled by default during the API initialization (#adi_AFE_Init()).
 *              The application may use the #adi_AFE_RegisterCallbackOnReceiveDMA() API to register an application-defined
 *              callback function with the interrupt handler that will be called in response to a DMA done interrupt for the AFE
 *              receive channel. When a DMA done interrupt occurs, the interrupt handler will make the callback to the application
 *              for application-based interrupt handling. The application callback should \a avoid \a extended \a processing during
 *              the callback as the callback is made context of the initiating interrupt and will block lower-priority interrupts.
 *              If extended application-level interrupt processing is required, the application should schedule it for the main
 *              application loop and exit the callback as soon as possible.\n\n
 *
 * @note        The value of the cbWatch parameter is ignored, because there is a single interrupt source that needs watching.
 *
 */


ADI_AFE_RESULT_TYPE adi_AFE_RegisterCallbackOnReceiveDMA(ADI_AFE_DEV_HANDLE hDevice, ADI_CALLBACK cbFunc, uint32_t cbWatch) {
    ADI_AFE_RESULT_TYPE result = ADI_AFE_SUCCESS;

#ifdef ADI_DEBUG
    if (adi_AFE_InvalidHandle(hDevice)) {
        return ADI_AFE_ERR_BAD_DEV_HANDLE;
    }

    if (adi_AFE_HandleNotInitialized(hDevice)) {
        return ADI_AFE_ERR_NOT_INITIALIZED;
    }

    if (pADI_SYSCLK->CLKCON5 & BITM_SYSCLK_CLKCON5_ACLKOFF) {
        return ADI_AFE_ERR_ACLKOFF;
    }

    if (16000000 != SystemGetClockFrequency(ADI_SYS_CLOCK_ACLK)) {
        return ADI_AFE_ERR_WRONG_ACLK_FREQUENCY;
    }

    if (cbFunc && hDevice->cbRxDmaFcn) {
        return ADI_AFE_ERR_CALLBACK_OVERWRITE;
    }
#endif

    /* DMA callbacks have no watch */
    hDevice->cbRxDmaFcn = cbFunc;

    return result;
}

/*!
 * @brief       Application callback registration for AFE interrupt handlers.
 *
 * @param[in]   hDevice                                 Device handle obtained from adi_AFE_Init().
 * @param[in]   group                                   AFE interrupt group type identifier.
 * @param[in]   cbFunc                                  Application callback address; the function to call.
 * @param[in]   cbWatch                                 The interrupts to watch, controlling which interrupts result in a callback.
 *
 * @return      Status
 *              - #ADI_AFE_SUCCESS                      Call completed successfully.
 *              - #ADI_AFE_ERR_BAD_DEV_HANDLE           Invalid device handle.
 *              - #ADI_AFE_ERR_NOT_INITIALIZED          Device not initialized.
 *              - #ADI_AFE_ERR_ACLKOFF                  ACLK disabled from the clock gate.
 *              - #ADI_AFE_ERR_WRONG_ACLK_FREQUENCY     Programmed ACLK frequency is not the required 16MHz.
 *              - #ADI_AFE_ERR_CALLBACK_OVERWRITE       Attempt to overwrite existing callback.
 *
 * @details     Registers an application-defined callback function with one of the AFE interrupt handlers.
 *              Callbacks are made in response to received AFE interrupts, depending on whether they are enabled
 *              and whether the corresponding callback watch bits are set.  This allows the application to control which
 *              interrupts it wants to be called back on.

 *              The callback to the application is made in context of the originating interrupt (i.e. the AFE 
 *              interrupt handler that is registered in the system's interrupt vector table).  Extended processing
 *              during the callback (an extension of the AFE interrupt handler) is discouraged so as to avoid lower-priority
 *              interrupt blocking.  Also, any register read-modify-write operations should be protected using the
 *              ADI_ENTER_CRITICAL_REGION()/ADI_EXIT_CRITICAL_REGION() pair to prevent higher-priority interrupts from modifying
 *              said register during the read-modify-write operation.
 *
 *   @note      CALLBACKS: AFE interrupt callbacks are \a disabled by default during the API initialization (#adi_AFE_Init()).
 *              The application may use the #adi_AFE_RegisterCallback() API to register an application-defined
 *              callback function with the interrupt handler that will be called in response to selected AFE interrupts.
 *              The callback registration API takes an interrupt mask (of type uint32_t) that designates the set of interrupts
 *              to watch (upon which to dispatch the callback).  When any of the watched interrupts occur, the appropriate AFE 
 *              interrupt handler will make the callback to the application for application-based interrupt handling. The application
 *              callback should \a avoid \a extended \a processing during the callback as the callback is made context of the initiating
 *              interrupt and will block lower-priority interrupts.  If extended application-level interrupt processing is required, the
 *              application should schedule it for the main application loop and exit the callback as soon as possible.\n\n
 *
 */
ADI_AFE_RESULT_TYPE adi_AFE_RegisterAfeCallback(ADI_AFE_DEV_HANDLE hDevice, ADI_AFE_INT_GROUP_TYPE group, ADI_CALLBACK cbFunc, uint32_t cbWatch) {
    ADI_AFE_RESULT_TYPE result = ADI_AFE_SUCCESS;

#ifdef ADI_DEBUG
    if (adi_AFE_InvalidHandle(hDevice)) {
        return ADI_AFE_ERR_BAD_DEV_HANDLE;
    }

    if (adi_AFE_HandleNotInitialized(hDevice)) {
        return ADI_AFE_ERR_NOT_INITIALIZED;
    }

    if (pADI_SYSCLK->CLKCON5 & BITM_SYSCLK_CLKCON5_ACLKOFF) {
        return ADI_AFE_ERR_ACLKOFF;
    }

    if (16000000 != SystemGetClockFrequency(ADI_SYS_CLOCK_ACLK)) {
        return ADI_AFE_ERR_WRONG_ACLK_FREQUENCY;
    }
    
    if (!((ADI_AFE_INT_GROUP_CAPTURE == group) || (ADI_AFE_INT_GROUP_GENERATE == group) || (ADI_AFE_INT_GROUP_CMD_FIFO == group) || (ADI_AFE_INT_GROUP_DATA_FIFO == group))) {
        return ADI_AFE_ERR_INVALID_IRQ;
    }

    if ((cbFunc && hDevice->cbCaptureFcn) || (cbFunc && hDevice->cbGenerateFcn) || (cbFunc && hDevice->cbCmdFifoFcn) || (cbFunc && hDevice->cbDataFifoFcn)) {
        return ADI_AFE_ERR_CALLBACK_OVERWRITE;
    }
#endif

    switch (group)
    {
        case ADI_AFE_INT_GROUP_CAPTURE:
            hDevice->cbCaptureFcn       = cbFunc;
            hDevice->cbCaptureWatch     = cbWatch;
            break;

        case ADI_AFE_INT_GROUP_GENERATE:
            hDevice->cbGenerateFcn      = cbFunc;
            hDevice->cbGenerateWatch    = cbWatch;
            break;

        case ADI_AFE_INT_GROUP_CMD_FIFO:
            hDevice->cbCmdFifoFcn       = cbFunc;
            hDevice->cbCmdFifoWatch     = cbWatch;
            break;

        case ADI_AFE_INT_GROUP_DATA_FIFO:
            hDevice->cbDataFifoFcn      = cbFunc;
            hDevice->cbDataFifoWatch    = cbWatch;
            break;
    }
    
    return result;
}

#if (ADI_AFE_CFG_ENABLE_DEPRECATED_FUNCTION_SUPPORT == 1)
/*!
 * @brief       Application callback registration for the command FIFO interrupt handler.
 *
 * @param[in]   hDevice                                 Device handle obtained from adi_AFE_Init().
 * @param[in]   cbFunc                                  Application callback address; the function to call.
 * @param[in]   cbWatch                                 The interrupts to watch, controlling which interrupts result in a callback.
 *
 * @return      Status
 *              - #ADI_AFE_SUCCESS                      Call completed successfully.
 *              - #ADI_AFE_ERR_BAD_DEV_HANDLE           Invalid device handle.
 *              - #ADI_AFE_ERR_NOT_INITIALIZED          Device not initialized.
 *              - #ADI_AFE_ERR_ACLKOFF                  ACLK disabled from the clock gate.
 *              - #ADI_AFE_ERR_WRONG_ACLK_FREQUENCY     Programmed ACLK frequency is not the required 16MHz.
 *              - #ADI_AFE_ERR_CALLBACK_OVERWRITE       Attempt to overwrite existing callback.
 *
 * @details     Registers an application-defined callback function with the command FIFO interrupt handler (AFE_CMD_FIFO_IRQ).
 *              Callbacks are made in response to received AFE command FIFO interrupts, depending on whether they are enabled
 *              and whether the corresponding callback watch bits are set.  This allows the application to control which
 *              interrupts it wants to be called back on.

 *              The callback to the application is made in context of the originating interrupt (i.e., the AFE command FIFO
 *              interrupt handler that is registered in the system's interrupt vector table).  Extended processing
 *              during the callback (an extension of the AFE interrupt handler) is discouraged so as to avoid lower-priority
 *              interrupt blocking.  Also, any register read-modify-write operations should be protected using the
 *              ADI_ENTER_CRITICAL_REGION()/ADI_EXIT_CRITICAL_REGION() pair to prevent higher-priority interrupts from modifying
 *              said register during the read-modify-write operation.
 *
 * @note        CALLBACKS: AFE interrupt callbacks are \a disabled by default during the API initialization (#adi_AFE_Init()).
 *              The application may use the #adi_AFE_RegisterCallbackOnCommandFIFO() API to register an application-defined
 *              callback function with the interrupt handler that will be called in response to selected command FIFO interrupts.
 *              The callback registration API takes an interrupt mask (of type uint32_t) that designates the set of interrupts
 *              to watch (upon which to dispatch the callback).  When any of the watched interrupts occur, the AFE command FIFO
 *              interrupt handler will make the callback to the application for application-based interrupt handling. The application
 *              callback should \a avoid \a extended \a processing during the callback as the callback is made context of the initiating
 *              interrupt and will block lower-priority interrupts.  If extended application-level interrupt processing is required, the
 *              application should schedule it for the main application loop and exit the callback as soon as possible.\n\n
 *
 */

ADI_AFE_RESULT_TYPE adi_AFE_RegisterCallbackOnCommandFIFO(ADI_AFE_DEV_HANDLE hDevice, ADI_CALLBACK cbFunc, uint32_t cbWatch) {
    ADI_AFE_RESULT_TYPE result = ADI_AFE_SUCCESS;

#ifdef ADI_DEBUG
    if (adi_AFE_InvalidHandle(hDevice)) {
        return ADI_AFE_ERR_BAD_DEV_HANDLE;
    }

    if (adi_AFE_HandleNotInitialized(hDevice)) {
        return ADI_AFE_ERR_NOT_INITIALIZED;
    }

    if (pADI_SYSCLK->CLKCON5 & BITM_SYSCLK_CLKCON5_ACLKOFF) {
        return ADI_AFE_ERR_ACLKOFF;
    }

    if (16000000 != SystemGetClockFrequency(ADI_SYS_CLOCK_ACLK)) {
        return ADI_AFE_ERR_WRONG_ACLK_FREQUENCY;
    }

    if (cbFunc && hDevice->cbCmdFifoFcn) {
        return ADI_AFE_ERR_CALLBACK_OVERWRITE;
    }
#endif

    hDevice->cbCmdFifoFcn   = cbFunc;
    hDevice->cbCmdFifoWatch = cbWatch;

    return result;
}

/*!
 * @brief       Application callback registration for the data FIFO interrupt handler.
 *
 * @param[in]   hDevice                                 Device handle obtained from adi_AFE_Init().
 * @param[in]   cbFunc                                  Application callback address; the function to call.
 * @param[in]   cbWatch                                 The interrupts to watch, controlling which interrupts result in a callback.
 *
 * @return      Status
 *              - #ADI_AFE_SUCCESS                      Call completed successfully.
 *              - #ADI_AFE_ERR_BAD_DEV_HANDLE           Invalid device handle.
 *              - #ADI_AFE_ERR_NOT_INITIALIZED          Device not initialized.
 *              - #ADI_AFE_ERR_ACLKOFF                  ACLK disabled from the clock gate.
 *              - #ADI_AFE_ERR_WRONG_ACLK_FREQUENCY     Programmed ACLK frequency is not the required 16MHz.
 *              - #ADI_AFE_ERR_CALLBACK_OVERWRITE       Attempt to overwrite existing callback.
 *
 * @details     Registers an application-defined callback function with the data FIFO interrupt handler (AFE_DATA_FIFO_IRQ).
 *              Callbacks are made in response to received AFE data FIFO interrupts, depending on whether they are enabled
 *              and whether the corresponding callback watch bits are set.  This allows the application to control which
 *              interrupts it wants to be called back on.

 *              The callback to the application is made in context of the originating interrupt (i.e., the AFE data FIFO
 *              interrupt handler that is registered in the system's interrupt vector table).  Extended processing
 *              during the callback (an extension of the AFE interrupt handler) is discouraged so as to avoid lower-priority
 *              interrupt blocking.  Also, any register read-modify-write operations should be protected using the
 *              ADI_ENTER_CRITICAL_REGION()/ADI_EXIT_CRITICAL_REGION() pair to prevent higher-priority interrupts from modifying
 *              said register during the read-modify-write operation.
 *
 *   @note      CALLBACKS: AFE interrupt callbacks are \a disabled by default during the API initialization (#adi_AFE_Init()).
 *              The application may use the #adi_AFE_RegisterCallbackOnDataFIFO() API to register an application-defined
 *              callback function with the interrupt handler that will be called in response to selected data FIFO interrupts.
 *              The callback registration API takes an interrupt mask (of type uint32_t) that designates the set of interrupts
 *              to watch (upon which to dispatch the callback).  When any of the watched interrupts occur, the AFE data FIFO
 *              interrupt handler will make the callback to the application for application-based interrupt handling. The application
 *              callback should \a avoid \a extended \a processing during the callback as the callback is made context of the initiating
 *              interrupt and will block lower-priority interrupts.  If extended application-level interrupt processing is required, the
 *              application should schedule it for the main application loop and exit the callback as soon as possible.\n\n
 *
 */

ADI_AFE_RESULT_TYPE adi_AFE_RegisterCallbackOnDataFIFO(ADI_AFE_DEV_HANDLE hDevice, ADI_CALLBACK cbFunc, uint32_t cbWatch) {
    ADI_AFE_RESULT_TYPE result = ADI_AFE_SUCCESS;

#ifdef ADI_DEBUG
    if (adi_AFE_InvalidHandle(hDevice)) {
        return ADI_AFE_ERR_BAD_DEV_HANDLE;
    }

    if (adi_AFE_HandleNotInitialized(hDevice)) {
        return ADI_AFE_ERR_NOT_INITIALIZED;
    }

    if (pADI_SYSCLK->CLKCON5 & BITM_SYSCLK_CLKCON5_ACLKOFF) {
        return ADI_AFE_ERR_ACLKOFF;
    }

    if (16000000 != SystemGetClockFrequency(ADI_SYS_CLOCK_ACLK)) {
        return ADI_AFE_ERR_WRONG_ACLK_FREQUENCY;
    }

    if (cbFunc && hDevice->cbDataFifoFcn) {
        return ADI_AFE_ERR_CALLBACK_OVERWRITE;
    }
#endif

    hDevice->cbDataFifoFcn   = cbFunc;
    hDevice->cbDataFifoWatch = cbWatch;

    return result;
}
#endif /* (ADI_AFE_CFG_ENABLE_DEPRECATED_FUNCTION_SUPPORT) */

#if (ADI_AFE_CFG_ENABLE_RX_DMA_DUAL_BUFFER_SUPPORT == 1)      
/*!
 * @brief       Sets buffer sizes for dual buffer Rx DMA mode.
 *
 * @param[in]   hDevice                                 Device handle obtained from adi_AFE_Init().
 * @param[in]   maxSizeFirst                            Maximum size of the first buffer in dual buffer mode.
 * @param[in]   maxSizeSecond                           Maximum size of the second buffer in dual buffer mode.
 *
 * @return      Status
 *              - #ADI_AFE_SUCCESS                      Call completed successfully.
 *              - #ADI_AFE_ERR_BAD_DEV_HANDLE           Invalid device handle.
 *              - #ADI_AFE_ERR_NOT_INITIALIZED          Device not initialized.
 *              - #ADI_AFE_ERR_ACLKOFF                  ACLK disabled from the clock gate.
 *              - #ADI_AFE_ERR_WRONG_ACLK_FREQUENCY     Programmed ACLK frequency is not the required 16MHz.
 *
 * @details     
 *
 */

ADI_AFE_RESULT_TYPE adi_AFE_SetDmaRxBufferMaxSize(ADI_AFE_DEV_HANDLE const hDevice, uint16_t maxSizeFirst, uint16_t maxSizeSecond) {

#ifdef ADI_DEBUG
    if (adi_AFE_InvalidHandle(hDevice)) {
        return ADI_AFE_ERR_BAD_DEV_HANDLE;
    }

    if (adi_AFE_HandleNotInitialized(hDevice)) {
        return ADI_AFE_ERR_NOT_INITIALIZED;
    }

    if (pADI_SYSCLK->CLKCON5 & BITM_SYSCLK_CLKCON5_ACLKOFF) {
        return ADI_AFE_ERR_ACLKOFF;
    }

    if (16000000 != SystemGetClockFrequency(ADI_SYS_CLOCK_ACLK)) {
        return ADI_AFE_ERR_WRONG_ACLK_FREQUENCY;
    }

    /* DMA controller doesn't support DMA cycles with more than 1024 DMA transfers */
    if ((maxSizeFirst > 1024) ||
        (maxSizeSecond  > 1024)) 
    {
        return ADI_AFE_ERR_PARAM_OUT_OF_RANGE;
    }

    /* The size of the first buffer should not be equal to 0 */
    if (maxSizeFirst == 0)
    {
        return ADI_AFE_ERR_PARAM_OUT_OF_RANGE;
    }
#endif

    hDevice->dmaRxBufferMaxSize[0] = maxSizeFirst;
    hDevice->dmaRxBufferMaxSize[1] = maxSizeSecond;

    return ADI_AFE_SUCCESS;
}

/*!
 * @brief       Reads buffer sizes for dual buffer Rx DMA mode.
 *
 * @param[in]   hDevice                                 Device handle obtained from adi_AFE_Init().
 *
 * @param[out]  maxSizeFirst                            Maximum size of the first buffer in dual buffer mode.
 * @param[out]  maxSizeSecond                           Maximum size of the second buffer in dual buffer mode.
 *
 * @return      Status
 *              - #ADI_AFE_SUCCESS                      Call completed successfully.
 *              - #ADI_AFE_ERR_BAD_DEV_HANDLE           Invalid device handle.
 *              - #ADI_AFE_ERR_NOT_INITIALIZED          Device not initialized.
 *              - #ADI_AFE_ERR_ACLKOFF                  ACLK disabled from the clock gate.
 *              - #ADI_AFE_ERR_WRONG_ACLK_FREQUENCY     Programmed ACLK frequency is not the required 16MHz.
 *
 * @details
 *
 */

ADI_AFE_RESULT_TYPE adi_AFE_GetDmaRxBufferMaxSize(ADI_AFE_DEV_HANDLE const hDevice, uint16_t* maxSizeFirst, uint16_t* maxSizeSecond) {

    *maxSizeFirst = hDevice->dmaRxBufferMaxSize[0];
    *maxSizeSecond = hDevice->dmaRxBufferMaxSize[1];

    return ADI_AFE_SUCCESS;
}
#endif /* (ADI_AFE_CFG_ENABLE_RX_DMA_DUAL_BUFFER_SUPPORT == 1) */

/*!
 * @brief       Initialize the AFE device.
 *
 * @param[in]   phDevice                                Pointer to the new device handle.
 *
 * @return      Status
 *              - #ADI_AFE_SUCCESS                      Call completed successfully.
 *              - #ADI_AFE_ERR_ALREADY_INITIALIZED      Device already initialized.
 *              - #ADI_AFE_ERR_BAD_DEV_HANDLE           Invalid device handle.
 *              - #ADI_AFE_ERR_ACLKOFF                  ACLK disabled from the clock gate.
 *              - #ADI_AFE_ERR_WRONG_ACLK_FREQUENCY     Programmed ACLK frequency is not the required 16MHz.
 *              - #ADI_AFE_ERR_CALLBACK_OVERWRITE       Attempt to overwrite existing callback.
 *
 * @details     Initializes the AFE for the first use. The function will enable the AFE clock (ACLK),
 *              initialize DMA channels, enable AFE interrupts, and reset the interrupt callbacks.
 *
 * @note        The function does not require an ACLK frequency of 16MHz and can be called before the
 *              ALCK frequency is setup. An ACLK frequency of 16MHz is required for all AFE functions.
 *
 * @sa          adi_AFE_UnInit()
 */

ADI_AFE_RESULT_TYPE adi_AFE_Init(ADI_AFE_DEV_HANDLE* const phDevice) {
    ADI_AFE_RESULT_TYPE     result = ADI_AFE_SUCCESS;
    ADI_AFE_DEV_HANDLE      hDevice;

    /* Store a bad handle in case of failure */
    *phDevice = (ADI_AFE_DEV_HANDLE) NULL;

    /* Local pointer to instance data */
    hDevice = pAFE_DevData;

#ifdef ADI_DEBUG
    if (!(adi_AFE_HandleNotInitialized(hDevice))) {
        return ADI_AFE_ERR_ALREADY_INITIALIZED;
    }
#endif

    /* Set RCAL and RTIA to default values */
    hDevice->rcal = 1000;
    hDevice->rtia = 7500;
    
    /* Initialize MMR pointer */
    hDevice->pAFE = pADI_AFE;

    /* Turn on ACLK */
    SystemEnableClock(ADI_SYS_CLOCK_GATE_ACLK, true);

    ADI_INSTALL_HANDLER(ADI_INT_AFE_CAPTURE, AFE_Capture_Int_Handler);
    ADI_INSTALL_HANDLER(ADI_INT_AFE_GENERATE, AFE_Generate_Int_Handler);
    ADI_INSTALL_HANDLER(ADI_INT_AFE_CMD_FIFO, AFE_CmdFIFO_Int_Handler);
    ADI_INSTALL_HANDLER(ADI_INT_AFE_DATA_FIFO, AFE_DataFIFO_Int_Handler);
    ADI_INSTALL_HANDLER(ADI_INT_DMA_AFE_TX, DMA_AFE_TX_Int_Handler);
    ADI_INSTALL_HANDLER(ADI_INT_DMA_AFE_RX, DMA_AFE_RX_Int_Handler);

    /* Enable all AFE interrupts from NVIC */
    /* They are all still masked locally   */
    ADI_ENABLE_INT(AFE_CAPTURE_IRQn);
    ADI_ENABLE_INT(AFE_GENERATE_IRQn);
    ADI_ENABLE_INT(AFE_CMD_FIFO_IRQn);
    ADI_ENABLE_INT(AFE_DATA_FIFO_IRQn);

    /* Initialize DMA descriptors */
    hDevice->pTxDmaDescriptor   = &gDmaDescriptorForAFETx;
    hDevice->pRxDmaDescriptor   = &gDmaDescriptorForAFERx;

#if (ADI_AFE_CFG_ENABLE_RX_DMA_DUAL_BUFFER_SUPPORT == 1)
    /* Defaulting to a single buffer */
    hDevice->dmaRxBufferMaxSize[0] = 1024;
    hDevice->dmaRxBufferMaxSize[1] = 0;
    hDevice->dmaRxRemaining = 0;
#endif

    hDevice->pRxBuffer = NULL;
    hDevice->dmaRxTransferCount = 0;
    
    hDevice->bSoftwareCRC = false;

    /* Initialize sequencer status */
    hDevice->seqState = ADI_AFE_SEQ_STATE_IDLE;
    
    /* RunSequence function to run in blocking mode by default */
    hDevice->bRunSequenceBlockingMode = true;

    /* Initialize callbacks */
    hDevice->cbTxDmaFcn     = NULL;
    hDevice->cbRxDmaFcn     = NULL;
    hDevice->cbCaptureFcn   = NULL;
    hDevice->cbGenerateFcn  = NULL;
    hDevice->cbCmdFifoFcn   = NULL;
    hDevice->cbDataFifoFcn  = NULL;

#if (ADI_CFG_ENABLE_RTOS_SUPPORT == 1)
    /* Create the semaphores for blocking mode support. */
    if (adi_osal_SemCreateStatic(hDevice->SemMem, ADI_OSAL_MAX_SEM_SIZE_CHAR, &hDevice->hSeqSem, 0) != ADI_OSAL_SUCCESS) {
        return ADI_AFE_ERR_SEMAPHORE_FAILED;
    }
#else
    /* init interrupt flag for low power mode */
    hDevice->bInterruptFlag = false;    
#endif /* ADI_CFG_ENABLE_RTOS_SUPPORT */


    if (ADI_AFE_SUCCESS != (result = dmaInit(hDevice))) {
        return result;
    }

    /* Mark driver initialized */
    hDevice->bInitialized = true;

    *phDevice = hDevice;

    return result;
}

/*!
 * @brief       Uninitialize the AFE device.
 *
 * @param[in]   hDevice                                 Device handle obtained from adi_AFE_Init().
 *
 * @return      Status
 *              - #ADI_AFE_SUCCESS                      Call completed successfully.
 *              - #ADI_AFE_ERR_ALREADY_INITIALIZED      Device already initialized.
 *
 * @details     Uninitializes the AFE for the first use. The function will enable the AFE clock (ACLK),
 *              initialize DMA channels, enable AFE interrupts, and reset the interrupt callbacks.
 *
 * @note        The function does not require an ACLK frequency of 16MHz and can be called in any clock configuration.
 *              For example, the AFE can be unitialized after the ALCK frequency has been slowed down from 16MHz.
 *
 * @sa          adi_AFE_Init()
 */

ADI_AFE_RESULT_TYPE adi_AFE_UnInit(ADI_AFE_DEV_HANDLE const hDevice) {
    ADI_AFE_RESULT_TYPE result = ADI_AFE_SUCCESS;


#ifdef ADI_DEBUG
    if (adi_AFE_InvalidHandle(hDevice)) {
        return ADI_AFE_ERR_BAD_DEV_HANDLE;
    }

    if (adi_AFE_HandleNotInitialized(hDevice)) {
        return ADI_AFE_ERR_NOT_INITIALIZED;
    }

#endif

    /* Enable all AFE interrupts from NVIC  */
    /* They are all still masked locally    */
    ADI_DISABLE_INT(AFE_CAPTURE_IRQn);
    ADI_DISABLE_INT(AFE_GENERATE_IRQn);
    ADI_DISABLE_INT(AFE_CMD_FIFO_IRQn);
    ADI_DISABLE_INT(AFE_DATA_FIFO_IRQn);

    ADI_UNINSTALL_HANDLER(ADI_INT_AFE_CAPTURE);
    ADI_UNINSTALL_HANDLER(ADI_INT_AFE_GENERATE);
    ADI_UNINSTALL_HANDLER(ADI_INT_AFE_CMD_FIFO);
    ADI_UNINSTALL_HANDLER(ADI_INT_AFE_DATA_FIFO);
    ADI_UNINSTALL_HANDLER(ADI_INT_DMA_AFE_TX);
    ADI_UNINSTALL_HANDLER(ADI_INT_DMA_AFE_RX);

    /* Uninitialize DMA descriptors */
    hDevice->pTxDmaDescriptor   = NULL;
    hDevice->pRxDmaDescriptor   = NULL;

    /* Uninitialiaze DMA channels */
    dmaUnInit(hDevice);

    /* Reset sequencer status */
    hDevice->seqState = ADI_AFE_SEQ_STATE_IDLE;

    /* Reset callbacks */
    hDevice->cbTxDmaFcn     = NULL;
    hDevice->cbRxDmaFcn     = NULL;
    hDevice->cbCaptureFcn   = NULL;
    hDevice->cbGenerateFcn  = NULL;
    hDevice->cbCmdFifoFcn   = NULL;
    hDevice->cbDataFifoFcn  = NULL;

#if (ADI_CFG_ENABLE_RTOS_SUPPORT == 1)
    /* Destroy the semaphore. */
    if (adi_osal_SemDestroyStatic(hDevice->hSeqSem) != ADI_OSAL_SUCCESS) {
        return ADI_AFE_ERR_SEMAPHORE_FAILED;
    }
#endif /* ADI_CFG_ENABLE_RTOS_SUPPORT */

    /* Mark driver uninitialized */
    hDevice->bInitialized = false;

    /* Turn off ACLK */
    SystemEnableClock(ADI_SYS_CLOCK_GATE_ACLK, false);

    return result;
}

/***************************************************************************/
/*   Interrupt Handlers                                                    */
/***************************************************************************/

/*!
 * @brief       Enable interrupt sources inside the AFE.
 *
 * @param[in]   hDevice                                 Device handle obtained from adi_AFE_Init().
 * @param[in]   group                                   Interrupt group identifier.
 * @param[in]   mask                                    Interrupt group mask.
 * @param[in]   bFlag                                   True to unmask interrupt sources, False to mask interrupt sources.
 *
 * @return      Status
 *              - #ADI_AFE_SUCCESS                      Call completed successfully.
 *              - #ADI_AFE_ERR_BAD_DEV_HANDLE           Invalid device handle.
 *              - #ADI_AFE_ERR_NOT_INITIALIZED          Device not initialized.
 *              - #ADI_AFE_ERR_ACLKOFF                  ACLK disabled from the clock gate.
 *              - #ADI_AFE_ERR_WRONG_ACLK_FREQUENCY     Programmed ACLK frequency is not the required 16MHz.
 *
 * @details
 *
 */

ADI_AFE_RESULT_TYPE adi_AFE_EnableInterruptSource(ADI_AFE_DEV_HANDLE const hDevice, ADI_AFE_INT_GROUP_TYPE group, uint32_t mask, bool_t bFlag) {
    ADI_AFE_RESULT_TYPE     result = ADI_AFE_SUCCESS;
    volatile uint32_t       *pReg;
    IRQn_Type               irq;

#ifdef ADI_DEBUG
    if (adi_AFE_InvalidHandle(hDevice)) {
        return ADI_AFE_ERR_BAD_DEV_HANDLE;
    }

    if (adi_AFE_HandleNotInitialized(hDevice)) {
        return ADI_AFE_ERR_NOT_INITIALIZED;
    }

    if (pADI_SYSCLK->CLKCON5 & BITM_SYSCLK_CLKCON5_ACLKOFF) {
        return ADI_AFE_ERR_ACLKOFF;
    }

    if (16000000 != SystemGetClockFrequency(ADI_SYS_CLOCK_ACLK)) {
        return ADI_AFE_ERR_WRONG_ACLK_FREQUENCY;
    }
#endif

    switch(group) {
        case ADI_AFE_INT_GROUP_CAPTURE:
            pReg = &pADI_AFE->AFE_ANALOG_CAPTURE_IEN;
            irq = AFE_CAPTURE_IRQn;
            break;

        case ADI_AFE_INT_GROUP_GENERATE:
            pReg = &pADI_AFE->AFE_ANALOG_GEN_IEN;
            irq = AFE_GENERATE_IRQn;
            break;

        case ADI_AFE_INT_GROUP_CMD_FIFO:
            pReg = &pADI_AFE->AFE_CMD_FIFO_IEN;
            irq = AFE_CMD_FIFO_IRQn;
            break;

        case ADI_AFE_INT_GROUP_DATA_FIFO:
            pReg = &pADI_AFE->AFE_DATA_FIFO_IEN;
            irq = AFE_DATA_FIFO_IRQn;
            break;
    }

    if (bFlag) {
        *pReg |= mask;
        ADI_ENABLE_INT(irq);
    }
    else {
        *pReg &= ~mask;
        if (!(*pReg)) {
            ADI_DISABLE_INT(irq);
        }
    }

    return result;
}

/*!
 * @brief       Clear interrupt sticky bits.
 *
 * @param[in]   hDevice                                 Device handle obtained from adi_AFE_Init().
 * @param[in]   group                                   Interrupt group identifier.
 * @param[in]   mask                                    Interrupt group mask.
 *
 * @return      Status
 *              - #ADI_AFE_SUCCESS                      Call completed successfully.
 *              - #ADI_AFE_ERR_BAD_DEV_HANDLE           Invalid device handle.
 *              - #ADI_AFE_ERR_NOT_INITIALIZED          Device not initialized.
 *              - #ADI_AFE_ERR_ACLKOFF                  ACLK disabled from the clock gate.
 *              - #ADI_AFE_ERR_WRONG_ACLK_FREQUENCY     Programmed ACLK frequency is not the required 16MHz.
 *
 * @details
 *
 */

ADI_AFE_RESULT_TYPE adi_AFE_ClearInterruptSource(ADI_AFE_DEV_HANDLE const hDevice, ADI_AFE_INT_GROUP_TYPE group, uint32_t mask) {
    ADI_AFE_RESULT_TYPE     result = ADI_AFE_SUCCESS;

#ifdef ADI_DEBUG
    if (adi_AFE_InvalidHandle(hDevice)) {
        return ADI_AFE_ERR_BAD_DEV_HANDLE;
    }

    if (adi_AFE_HandleNotInitialized(hDevice)) {
        return ADI_AFE_ERR_NOT_INITIALIZED;
    }

    if (pADI_SYSCLK->CLKCON5 & BITM_SYSCLK_CLKCON5_ACLKOFF) {
        return ADI_AFE_ERR_ACLKOFF;
    }

    if (16000000 != SystemGetClockFrequency(ADI_SYS_CLOCK_ACLK)) {
        return ADI_AFE_ERR_WRONG_ACLK_FREQUENCY;
    }
#endif

    switch(group) {
        case ADI_AFE_INT_GROUP_CAPTURE:
            pADI_AFE->AFE_ANALOG_CAPTURE_INT = mask;
            break;

        case ADI_AFE_INT_GROUP_GENERATE:
            pADI_AFE->AFE_ANALOG_GEN_INT = mask;
            break;

        case ADI_AFE_INT_GROUP_CMD_FIFO:
            pADI_AFE->AFE_CMD_FIFO_INT = mask;
            break;

        case ADI_AFE_INT_GROUP_DATA_FIFO:
            pADI_AFE->AFE_DATA_FIFO_INT = mask;
            break;
    }

    return result;
}

/***************************************************************************/
/*   Interrupt Handlers                                                    */
/***************************************************************************/

/*! \cond PRIVATE */

/* ANALOG_CAPTURE_INT handler */
ADI_INT_HANDLER(AFE_Capture_Int_Handler) {
    /* Only 1 instance of the AFE in the system */
    ADI_AFE_DEV_HANDLE  hDevice = pAFE_DevData;
    uint32_t            sources;
    uint32_t            enables;
    uint32_t            clear = 0;
    uint32_t            disable = 0;

    sources = hDevice->pAFE->AFE_ANALOG_CAPTURE_INT;
    enables = hDevice->pAFE->AFE_ANALOG_CAPTURE_IEN;

    /* Interrupt bit: ADC_RESULT_READY */
    if ((sources & BITM_AFE_AFE_ANALOG_CAPTURE_INT_ADC_RESULT_READY) &&
        (enables & BITM_AFE_AFE_ANALOG_CAPTURE_IEN_ADC_RESULT_READY_IEN)) {
        /* Schedule interrupt to be cleared */
        clear |= BITM_AFE_AFE_ANALOG_CAPTURE_INT_ADC_RESULT_READY;
    }

    /* Interrupt bit: DFT_RESULT_READY */
    if ((sources & BITM_AFE_AFE_ANALOG_CAPTURE_INT_DFT_RESULT_READY) &&
        (enables & BITM_AFE_AFE_ANALOG_CAPTURE_IEN_DFT_RESULT_READY_IEN)) {
        /* Schedule interrupt to be cleared */
        clear |= BITM_AFE_AFE_ANALOG_CAPTURE_INT_DFT_RESULT_READY;
    }

    /* Interrupt bit: SUPPLY_LPF_RESULT_READY */
    if ((sources & BITM_AFE_AFE_ANALOG_CAPTURE_INT_SUPPLY_LPF_RESULT_READY) &&
        (enables & BITM_AFE_AFE_ANALOG_CAPTURE_IEN_SUPPLY_LPF_RESULT_READY_IEN)) {
        /* Schedule interrupt to be cleared */
        clear |= BITM_AFE_AFE_ANALOG_CAPTURE_INT_SUPPLY_LPF_RESULT_READY;
    }

    /* Interrupt bit: TEMP_RESULT_READY */
    if ((sources & BITM_AFE_AFE_ANALOG_CAPTURE_INT_TEMP_RESULT_READY) &&
        (enables & BITM_AFE_AFE_ANALOG_CAPTURE_IEN_TEMP_RESULT_READY_IEN)) {
        /* Schedule interrupt to be cleared */
        clear |= BITM_AFE_AFE_ANALOG_CAPTURE_INT_TEMP_RESULT_READY;
    }

    /* Interrupt bit: ADC_MIN_FAIL */
    if ((sources & BITM_AFE_AFE_ANALOG_CAPTURE_INT_ADC_MIN_FAIL) &&
        (enables & BITM_AFE_AFE_ANALOG_CAPTURE_IEN_ADC_MIN_FAIL_IEN)) {
        /* Since this is typically an error condition, we don't care if there is one */
        /* or multiple interrupts, so schedule interrupt to be disabled. */
        disable |= BITM_AFE_AFE_ANALOG_CAPTURE_IEN_ADC_MIN_FAIL_IEN;
    }

    /* Interrupt bit: ADC_MAX_FAIL */
    if ((sources & BITM_AFE_AFE_ANALOG_CAPTURE_INT_ADC_MAX_FAIL) &&
        (enables & BITM_AFE_AFE_ANALOG_CAPTURE_IEN_ADC_MAX_FAIL_IEN)) {
        /* Since this is typically an error condition, we don't care if there is one */
        /* or multiple interrupts, so schedule interrupt to be disabled. */
        disable |= BITM_AFE_AFE_ANALOG_CAPTURE_IEN_ADC_MAX_FAIL_IEN;
    }

    /* Interrupt bit: ADC_DELTA_FAIL */
    if ((sources & BITM_AFE_AFE_ANALOG_CAPTURE_INT_ADC_DELTA_FAIL) &&
        (enables & BITM_AFE_AFE_ANALOG_CAPTURE_IEN_ADC_DELTA_FAIL_IEN)) {
        /* Since this is typically an error condition, we don't care if there is one */
        /* or multiple interrupts, so schedule interrupt to be disabled. */
        disable |= BITM_AFE_AFE_ANALOG_CAPTURE_IEN_ADC_DELTA_FAIL_IEN;
    }

    /* Check if there is a callback defined */
    if (hDevice->cbCaptureFcn) {
        /* Call the function if the interrupt is watched         */
        /* Need to check both interrupts scheduled to be cleared */
        /* and interrupts scheduled to be disabled to find out   */
        /* what interrupt caused this ISR to be entered          */
        if ((clear | disable) & hDevice->cbCaptureWatch) {
            /* Passing to the callback the bits scheduled for clear or disable */
            //hDevice->cbCaptureFcn(hDevice, (clear | disable), NULL);
            //AG
            hDevice->cbCaptureFcn(hDevice, 0, NULL);
        }
    }

    adi_AFE_EnableInterruptSource(hDevice, ADI_AFE_INT_GROUP_CAPTURE, disable, false);
    adi_AFE_ClearInterruptSource(hDevice, ADI_AFE_INT_GROUP_CAPTURE, clear);

}

/* ANALOG_GEN_INT handler */
ADI_INT_HANDLER(AFE_Generate_Int_Handler) {
    /* Only 1 instance of the AFE in the system */
    ADI_AFE_DEV_HANDLE  hDevice = pAFE_DevData;
    uint32_t            sources;
    uint32_t            enables;
    uint32_t            clear = 0;

    sources = hDevice->pAFE->AFE_ANALOG_GEN_INT;
    enables = hDevice->pAFE->AFE_ANALOG_GEN_IEN;

    /* Interrupt bit: DELAY_COMMAND_DONE */
    if ((sources & BITM_AFE_AFE_ANALOG_GEN_INT_DELAY_COMMAND_DONE) &&
        (enables & BITM_AFE_AFE_ANALOG_GEN_IEN_DELAY_COMMAND_DONE_IEN)) {
        /* Schedule interrupt to be cleared */
        clear |= BITM_AFE_AFE_ANALOG_GEN_INT_DELAY_COMMAND_DONE;
    }

    /* Interrupt bit: HARDWARE_SETUP_DONE */
    if ((sources & BITM_AFE_AFE_ANALOG_GEN_INT_HARDWARE_SETUP_DONE) &&
        (enables & BITM_AFE_AFE_ANALOG_GEN_IEN_HARDWARE_SETUP_DONE_IEN)) {
        /* Schedule interrupt to be cleared */
        clear |= BITM_AFE_AFE_ANALOG_GEN_INT_HARDWARE_SETUP_DONE;
    }

    /* Interrupt bit: BREAK_SEQUENCE_ORG */
    if ((sources & BITM_AFE_AFE_ANALOG_GEN_INT_BREAK_SEQUENCE_ORG) &&
        (enables & BITM_AFE_AFE_ANALOG_GEN_IEN_BREAK_SEQUENCE_ORG_IEN)) {
        /* Schedule interrupt to be cleared */
        clear |= BITM_AFE_AFE_ANALOG_GEN_INT_BREAK_SEQUENCE_ORG;
    }

    /* Interrupt bit: CUSTOM_INT */
    if ((sources & BITM_AFE_AFE_ANALOG_GEN_INT_CUSTOM_INT) &&
        (enables & BITM_AFE_AFE_ANALOG_GEN_IEN_CUSTOM_INT_IEN)) {
        /* Schedule interrupt to be cleared */
        clear |= BITM_AFE_AFE_ANALOG_GEN_INT_CUSTOM_INT;
    }

    /* Check if there is a callback defined */
    if (hDevice->cbGenerateFcn) {
        /* Call the function if the interrupt is watched         */
        /* Need to check both interrupts scheduled to be cleared */
        /* and interrupts scheduled to be disabled to find out   */
        /* what interrupt caused this ISR to be entered          */
        if (clear & hDevice->cbGenerateWatch) {
            /* Passing to the callback the bits scheduled for clear or disable */
            //hDevice->cbGenerateFcn(hDevice, clear, NULL);
            //AG
            hDevice->cbGenerateFcn(hDevice, 0, NULL);
        }
    }

    adi_AFE_ClearInterruptSource(hDevice, ADI_AFE_INT_GROUP_GENERATE, clear);
}

/* CMD_FIFO_INT handler */
ADI_INT_HANDLER(AFE_CmdFIFO_Int_Handler) {
    /* Only 1 instance of the AFE in the system */
    ADI_AFE_DEV_HANDLE  hDevice = pAFE_DevData;
    uint32_t            sources;
    uint32_t            enables;
    uint32_t            clear = 0;
    uint32_t            disable = 0;

    sources = hDevice->pAFE->AFE_CMD_FIFO_INT;
    enables = hDevice->pAFE->AFE_CMD_FIFO_IEN;

    /* Interrupt bit: END_OF_SEQ */
    if ((sources & BITM_AFE_AFE_CMD_FIFO_INT_END_OF_SEQ) &&
        (enables & BITM_AFE_AFE_CMD_FIFO_IEN_END_OF_SEQ_IEN)) {
        /* Schedule interrupt to be cleared */
        clear |= BITM_AFE_AFE_CMD_FIFO_INT_END_OF_SEQ;
        /* Update sequencer status */
        hDevice->bSeqFinished = true;
    }

    /* Interrupt bit: SEQ_TIMEOUT_FINISHED */
    if ((sources & BITM_AFE_AFE_CMD_FIFO_INT_SEQ_TIMEOUT_FINISHED) &&
        (enables & BITM_AFE_AFE_CMD_FIFO_IEN_SEQ_TIMEOUT_FINISHED_IEN)) {
        /* Schedule interrupt to be cleared */
        clear |= BITM_AFE_AFE_CMD_FIFO_INT_SEQ_TIMEOUT_FINISHED;
    }

    /* Interrupt bit: SEQ_TIMEOUT_ERR */
    if ((sources & BITM_AFE_AFE_CMD_FIFO_INT_SEQ_TIMEOUT_ERR) &&
        (enables & BITM_AFE_AFE_CMD_FIFO_IEN_SEQ_TIMEOUT_ERR_IEN)) {
        /* Schedule interrupt to be cleared */
        clear |= BITM_AFE_AFE_CMD_FIFO_INT_SEQ_TIMEOUT_ERR;
    }

    /* Interrupt bit: CMD_FIFO_FULL */
    if ((sources & BITM_AFE_AFE_CMD_FIFO_INT_CMD_FIFO_FULL) &&
        (enables & BITM_AFE_AFE_CMD_FIFO_IEN_CMD_FIFO_FULL_IEN)) {
        /* Schedule interrupt to be disabled, it cannot be cleared here because the FIFO is still full */
        disable |= BITM_AFE_AFE_CMD_FIFO_IEN_CMD_FIFO_FULL_IEN;
        /* Update sequencer status */
        hDevice->bCmdFifoReady = true;
    }

    /* Interrupt bit: CMD_FIFO_EMPTY */
    if ((sources & BITM_AFE_AFE_CMD_FIFO_INT_CMD_FIFO_EMPTY) &&
        (enables & BITM_AFE_AFE_CMD_FIFO_IEN_CMD_FIFO_EMPTY_IEN)) {
        /* Schedule interrupt to be disabled, it cannot be cleared here because the FIFO is still empty */
        disable |= BITM_AFE_AFE_CMD_FIFO_IEN_CMD_FIFO_EMPTY_IEN;

    }

    /* Interrupt bit: CMD_FIFO_THR */
    if ((sources & BITM_AFE_AFE_CMD_FIFO_INT_CMD_FIFO_THR) &&
        (enables & BITM_AFE_AFE_CMD_FIFO_IEN_CMD_FIFO_THR_IEN)) {
        /* Schedule interrupt to be disabled, it cannot be cleared here because typically the FIFO occupancy is still below threshold */
        disable |= BITM_AFE_AFE_CMD_FIFO_IEN_CMD_FIFO_THR_IEN;
    }

    /* Interrupt bit: CMD_FIFO_OVF */
    if ((sources & BITM_AFE_AFE_CMD_FIFO_INT_CMD_FIFO_OVF) &&
        (enables & BITM_AFE_AFE_CMD_FIFO_IEN_CMD_FIFO_OVF_IEN)) {
        /* Schedule interrupt to be cleared */
        clear |= BITM_AFE_AFE_CMD_FIFO_INT_CMD_FIFO_OVF;
        /* Update sequencer status */
        hDevice->seqError = ADI_AFE_ERR_CMD_FIFO_OVF;
    }

    /* Interrupt bit: CMD_FIFO_UDF */
    if ((sources & BITM_AFE_AFE_CMD_FIFO_INT_CMD_FIFO_UDF) &&
        (enables & BITM_AFE_AFE_CMD_FIFO_IEN_CMD_FIFO_UDF_IEN)) {
        /* Schedule interrupt to be cleared */
        clear |= BITM_AFE_AFE_CMD_FIFO_INT_CMD_FIFO_UDF;
        /* Update sequencer status */
        hDevice->seqError = ADI_AFE_ERR_CMD_FIFO_UDF;
    }

#if (ADI_CFG_ENABLE_RTOS_SUPPORT == 1)
    adi_osal_SemPost(hDevice->hSeqSem);
#else
    /* Command FIFO interrupt needs to bring the processor out of low-power     */
    /* mode, as functions may be waiting for the state to change.               */
    /* Currently the only interrupt source used to wake up the processor        */
    /* is END_OF_SEQ.                                                           */
    SystemExitLowPowerMode(&hDevice->bInterruptFlag);
#endif /* ADI_CFG_ENABLE_RTOS_SUPPORT  */


    /* Check if there is a callback defined */
    if (hDevice->cbCmdFifoFcn) {
        /* Call the function if the interrupt is watched         */
        /* Need to check both interrupts scheduled to be cleared */
        /* and interrupts scheduled to be disabled to find out   */
        /* what interrupt caused this ISR to be entered          */
        if ((clear | disable) & hDevice->cbCmdFifoWatch) {
            /* Passing to the callback the bits scheduled for clear or disable */
            //hDevice->cbCmdFifoFcn(hDevice, (clear | disable), NULL);
            //AG
            hDevice->cbCmdFifoFcn(hDevice, clear | disable, NULL);
        }
    }

    adi_AFE_EnableInterruptSource(hDevice, ADI_AFE_INT_GROUP_CMD_FIFO, disable, false);
    adi_AFE_ClearInterruptSource(hDevice, ADI_AFE_INT_GROUP_CMD_FIFO, clear);
}

/* DATA_FIFO_INT handler */
ADI_INT_HANDLER(AFE_DataFIFO_Int_Handler) {
    /* Only 1 instance of the AFE in the system */
    ADI_AFE_DEV_HANDLE  hDevice = pAFE_DevData;
    uint32_t            sources;
    uint32_t            enables;
    uint32_t            clear = 0;
    uint32_t            disable = 0;

    sources = hDevice->pAFE->AFE_DATA_FIFO_INT;
    enables = hDevice->pAFE->AFE_DATA_FIFO_IEN;

    /* Interrupt bit: DATA_FIFO_FULL */
    if ((sources & BITM_AFE_AFE_DATA_FIFO_INT_DATA_FIFO_FULL) &&
        (enables & BITM_AFE_AFE_DATA_FIFO_IEN_DATA_FIFO_FULL_IEN)) {
        /* Schedule interrupt to be disabled, it cannot be cleared here because the FIFO is still full */
        disable |= BITM_AFE_AFE_DATA_FIFO_IEN_DATA_FIFO_FULL_IEN;
    }

    /* Interrupt bit: DATA_FIFO_EMPTY */
    if ((sources & BITM_AFE_AFE_DATA_FIFO_INT_DATA_FIFO_EMPTY) &&
        (enables & BITM_AFE_AFE_DATA_FIFO_IEN_DATA_FIFO_EMPTY_IEN)) {
        /* Schedule interrupt to be disabled, it cannot be cleared here because the FIFO is still empty */
        disable |= BITM_AFE_AFE_DATA_FIFO_IEN_DATA_FIFO_EMPTY_IEN;

    }

    /* Interrupt bit: DATA_FIFO_THR */
    if ((sources & BITM_AFE_AFE_DATA_FIFO_INT_DATA_FIFO_THR) &&
        (enables & BITM_AFE_AFE_DATA_FIFO_IEN_DATA_FIFO_THR_IEN)) {
        /* Schedule interrupt to be disabled, it cannot be cleared here because typically the FIFO occupancy is still above threshold */
        disable |= BITM_AFE_AFE_DATA_FIFO_IEN_DATA_FIFO_THR_IEN;
    }

    /* Interrupt bit: DATA_FIFO_OVF */
    if ((sources & BITM_AFE_AFE_DATA_FIFO_INT_DATA_FIFO_OVF) &&
        (enables & BITM_AFE_AFE_DATA_FIFO_IEN_DATA_FIFO_OVF_IEN)) {
        /* Schedule interrupt to be cleared */
        clear |= BITM_AFE_AFE_DATA_FIFO_INT_DATA_FIFO_OVF;
        /* Update sequencer status */
        hDevice->seqError = ADI_AFE_ERR_DATA_FIFO_OVF;
    }

    /* Interrupt bit: DATA_FIFO_UDF */
    if ((sources & BITM_AFE_AFE_DATA_FIFO_INT_DATA_FIFO_UDF) &&
        (enables & BITM_AFE_AFE_DATA_FIFO_IEN_DATA_FIFO_UDF_IEN)) {
        /* Schedule interrupt to be cleared */
        clear |= BITM_AFE_AFE_DATA_FIFO_INT_DATA_FIFO_UDF;
        /* Update sequencer status */
        hDevice->seqError = ADI_AFE_ERR_DATA_FIFO_UDF;
    }

    /* Check if there is a callback defined */
    if (hDevice->cbDataFifoFcn) {
        /* Call the function if the interrupt is watched         */
        /* Need to check both interrupts scheduled to be cleared */
        /* and interrupts scheduled to be disabled to find out   */
        /* what interrupt caused this ISR to be entered          */
        if ((clear | disable) & hDevice->cbDataFifoWatch) {
            /* Passing to the callback the bits scheduled for clear or disable */
            //hDevice->cbDataFifoFcn(hDevice, (clear | disable), NULL);
            //AG
            hDevice->cbDataFifoFcn(hDevice, 0, NULL);
        }
    }

    adi_AFE_EnableInterruptSource(hDevice, ADI_AFE_INT_GROUP_DATA_FIFO, disable, false);
    adi_AFE_ClearInterruptSource(hDevice, ADI_AFE_INT_GROUP_DATA_FIFO, clear);
}

/* Tx DMA done interrupt handler */
ADI_INT_HANDLER(DMA_AFE_TX_Int_Handler) {
    /* Only 1 instance of the AFE in the system */
    ADI_AFE_DEV_HANDLE hDevice = pAFE_DevData;

    /* Disable DMA done interrupt */
    ADI_DISABLE_INT(DMA_AFE_TX_IRQn);

    hDevice->bTxDmaComplete = true;
    hDevice->bCmdFifoReady = true;

    /* Callback */
    if (hDevice->cbTxDmaFcn) {
        /* Nothing to pass to the callback */
        hDevice->cbTxDmaFcn(hDevice, 0, NULL);
    }
}

/* Rx DMA done interrupt handler */
ADI_INT_HANDLER(DMA_AFE_RX_Int_Handler) {
    /* Only 1 instance of the AFE in the system */
    ADI_AFE_DEV_HANDLE  hDevice = pAFE_DevData;
    uint16_t*           pCurrentBuffer;
    uint16_t            currentLength;
#if (ADI_AFE_CFG_ENABLE_RX_DMA_DUAL_BUFFER_SUPPORT == 1)   
    static bool_t       bufferInitialized = false;
    static uint16_t*    pNextBuffer;
    uint16_t            nextLength;
#endif /* (ADI_AFE_CFG_ENABLE_RX_DMA_DUAL_BUFFER_SUPPORT == 1) */

    currentLength   = hDevice->dmaRxTransferCount;
    
#if (ADI_AFE_CFG_ENABLE_RX_DMA_DUAL_BUFFER_SUPPORT == 1)      
    if (hDevice->dmaRxRemaining) 
    {
        /* More DMA cycles needed */
        if (hDevice->dmaRxBufferIndex) 
        {
            /* Previous DMA cycle used buffer 1, next is buffer 0 */
            pCurrentBuffer  = hDevice->pRxBuffer + hDevice->dmaRxBufferMaxSize[0];
            pNextBuffer     = hDevice->pRxBuffer;

            if (hDevice->dmaRxRemaining <= hDevice->dmaRxBufferMaxSize[0]) 
            {
                nextLength              = hDevice->dmaRxRemaining;
                hDevice->dmaRxRemaining = 0;
            }
            else 
            {
                nextLength = hDevice->dmaRxBufferMaxSize[0];
                hDevice->dmaRxRemaining = hDevice->dmaRxRemaining - hDevice->dmaRxBufferMaxSize[0];
            }

            hDevice->dmaRxBufferIndex = 0;
        }
        else 
        {
            /* Previous DMA cycle used buffer 0, next is buffer 1 */
            pCurrentBuffer  = hDevice->pRxBuffer;
            pNextBuffer     = hDevice->pRxBuffer + hDevice->dmaRxBufferMaxSize[0];

            if (hDevice->dmaRxRemaining <= hDevice->dmaRxBufferMaxSize[1]) 
            {
                nextLength              = hDevice->dmaRxRemaining;
                hDevice->dmaRxRemaining = 0;
            }
            else 
            {
                nextLength = hDevice->dmaRxBufferMaxSize[1];
                hDevice->dmaRxRemaining = hDevice->dmaRxRemaining - hDevice->dmaRxBufferMaxSize[1];
            }

            hDevice->dmaRxBufferIndex = hDevice->dmaRxBufferMaxSize[1];
        }

        adi_AFE_ProgramRxDMA(hDevice, pNextBuffer, nextLength);
        bufferInitialized = true;
    }
    else 
    {
        /* Check if the buffer has been initialized */
        if (bufferInitialized)
        {
            /* Set the current buffer to be equal to the    */
            /* last buffer used.                            */
            pCurrentBuffer = pNextBuffer;
        }
        else
        {
            /* If pNextBuffer hasn't been initialized, set  */
            /* the current buffer to be equal to pRxBuffer. */
            pCurrentBuffer = hDevice->pRxBuffer;
        }
        /* Disable DMA done interrupt */
        ADI_DISABLE_INT(DMA_AFE_RX_IRQn);
        /* Mark Rx DMA operation as complete */
        hDevice->bRxDmaComplete = true;
        /* Clear the buffer initialized flag */
        bufferInitialized = false;
    }
#else
    /* Initialize for callback */
    pCurrentBuffer = hDevice->pRxBuffer;
    /* Disable DMA done interrupt */
    ADI_DISABLE_INT(DMA_AFE_RX_IRQn);
    /* Mark Rx DMA operation as complete */
    hDevice->bRxDmaComplete = true;
#endif /* (ADI_AFE_CFG_ENABLE_RX_DMA_DUAL_BUFFER_SUPPORT == 1) */

#if (ADI_CFG_ENABLE_RTOS_SUPPORT == 1)
    adi_osal_SemPost(hDevice->hSeqSem);
#else
    /* AFE Rx DMA done interrupt needs to bring the processor out of low-power  */
    /* mode, as functions may be waiting for the state to change.               */
    SystemExitLowPowerMode(&hDevice->bInterruptFlag);
#endif /* ADI_CFG_ENABLE_RTOS_SUPPORT  */

    /* Callback */
    if (hDevice->cbRxDmaFcn) 
    {
        /* Pass values that can used to keep track of the transfers in dual buffer Rx DMA mode */
        hDevice->cbRxDmaFcn(hDevice, currentLength, (void *)pCurrentBuffer);
    }
}

/*! \endcond */

#define REVERT_ADI_MISRA_SUPPRESSIONS  /*!< Revert ADI MISRA Suppressions */
#include "misra.h"

/*
** EOF
*/

/*@}*/

