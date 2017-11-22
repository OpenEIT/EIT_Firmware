/*********************************************************************************

Copyright (c) 2010-2014 Analog Devices, Inc. All Rights Reserved.

This software is proprietary to Analog Devices, Inc. and its licensors.  By using
this software you agree to the terms of the associated Analog Devices Software
License Agreement.

*********************************************************************************/

/*!
 *****************************************************************************
 * @file:    spi.c
 * @brief:   SPI Device Implementations for ADuCRFxxx
 * @version: $Revision: 28981 $
 * @date:    $Date: 2014-12-05 14:52:40 -0500 (Fri, 05 Dec 2014) $
 *****************************************************************************/

/*! \addtogroup SPI_Driver SPI Driver
 *  @{
 */

/*! \cond PRIVATE */

#define ASSERT_ADI_MISRA_SUPPRESSIONS  /*!< Apply ADI MISRA Suppressions */
#include "misra.h"

#include <stdlib.h>  /* for 'NULL" definition */
#include "spi.h"
#include "gpio.h"
#include "dma.h"

#include "adi_spi_config.h"

/* make generic defines to adapt old macro names to new macros file */
#ifndef SPIDMA_ENABLE
#define SPIDMA_ENABLE SPI1DMA_ENABLE  /*!< dma enable abstraction */
#endif

#ifndef SPIDMA_IENTXDMA
#define SPIDMA_IENTXDMA SPI1DMA_IENTXDMA  /*!< dma abstraction */
#endif

#ifndef SPIDMA_IENRXDMA
#define SPIDMA_IENRXDMA SPI1DMA_IENRXDMA  /*!< dma abstraction */
#endif

#ifndef SPICON_OEN
#define SPICON_OEN SPICON_SOEN  /*!< slave output enable abstraction */
#endif

/* Dispatch DMA overhead through function pointers so linker */
/* elimination can optimize them out when not referenced...  */

/*! \typedef ADI_SPI_INIT_FPTR
 * Pointer to DMA/Interrupt initialization function. */
typedef ADI_SPI_RESULT_TYPE (*ADI_SPI_INIT_FPTR)(ADI_SPI_DEV_HANDLE const hDevice);

/*! \typedef ADI_SPI_UNINIT_FPTR
 * Pointer to DMA/Interrupt uninitialization function. */
typedef ADI_SPI_RESULT_TYPE (*ADI_SPI_UNINIT_FPTR)(ADI_SPI_DEV_HANDLE const hDevice);

/*! \typedef ADI_SPI_INIT_DESCRS_FPTR
 * Pointer to DMA descriptor initialization function. */
typedef ADI_SPI_RESULT_TYPE (*ADI_SPI_INIT_DESCRS_FPTR) (ADI_SPI_DEV_HANDLE         const hDevice,
                                                         const uint8_t*             const pDataTx,
                                                         uint8_t*                   const pDataRx,
                                                         uint32_t                   const Size);

/*! \typedef ADI_SPI_DMA_HANDLER_FPTR
 * Pointer to DMA handler function. */
typedef void (*ADI_SPI_DMA_HANDLER_FPTR)(DMA_CHANn_TypeDef channelNum);

/*! \struct ADI_SPI_FTAB
 * The function pointer table. */
typedef struct {
    ADI_SPI_INIT_FPTR         pInitFcn;
    ADI_SPI_UNINIT_FPTR       pUnInitFcn;
    ADI_SPI_INIT_DESCRS_FPTR  pInitDescriptorsFcn;
    ADI_SPI_DMA_HANDLER_FPTR  pDmaHandlerFcn;
} ADI_SPI_FTAB;


/* Local abstractions to normalize the SPI chip select muxing, port and pin names

    Note, we don't use the SPI hardware controller version of CS.  Instead,
    we manage CS manually through the GPIO software interface.  This is because
    the SPI hardware controller deasserts CS if the FIFOs ever empty causeing
    slave devices to reset.

    Therefore, no need to play with pin muxing for CS because it is in GPIO mode
    already by default.
*/
#define ADI_SPI0_PORT              ADI_GPIO_PORT_3  /*!< port abstraction */
#define ADI_SPI1_PORT              ADI_GPIO_PORT_3  /*!< port abstraction */
#define ADI_SPIH_PORT              ADI_GPIO_PORT_0  /*!< port abstraction */

#define ADI_SPI0_DEFAULT_CS_PIN    ADI_GPIO_PIN_3   /*!< pin abstraction */
#define ADI_SPI1_DEFAULT_CS_PIN    ADI_GPIO_PIN_7   /*!< pin abstraction */
#define ADI_SPIH_DEFAULT_CS_PIN    ADI_GPIO_PIN_15  /*!< pin abstraction */

/* 16-bit DMA... (two-byte size and increment) */
#define ADI_DMA_DATA_WIDTH         ADI_DMA_WIDTH_HALFWORD  /*!< DMA data attribute */
#define ADI_DMA_DATA_INCREMENT     ADI_DMA_INCR_HALFWORD   /*!< DMA data attribute */

#if (ADI_SPI_CFG_ENABLE_DMA_SUPPORT == 1)

/* DMA transfer objects for submitting descriptors */
static ADI_DMA_TRANSFER_TYPE gDmaDescriptorForSPI0;
static ADI_DMA_TRANSFER_TYPE gDmaDescriptorForSPI1;

static ADI_DMA_TRANSFER_TYPE gDmaDescriptorForSPIH;

#endif /* ADI_SPI_CFG_ENABLE_DMA_SUPPORT */

/*! \struct ADI_SPI_DEV_DATA_TYPE
 * SPI Device instance data structure */
typedef struct ADI_SPI_DEV_DATA_TYPE
{

    /* device attributes */
    ADI_SPI_STATE_TYPE       InitState;        /*!< track initialization state */
    ADI_SPI_TypeDef*         pSpi;             /*!< track MMR device pointer   */
    bool_t                   bDmaMode;         /*!< DMA mode flag              */
    bool_t                   bBlockingMode;    /*!< blocking mode flag         */
    ADI_SPI_CHIP_SELECT_TYPE ChipSelect;       /*!< track chip select          */
    IRQn_Type                IRQn;             /*!< SPI interrupt number       */
#if (ADI_CFG_ENABLE_RTOS_SUPPORT == 1)
    /* OSAL */
    ADI_OSAL_SEM_HANDLE      hSem;              /*! Handle to semaphore         */
    uint8_t                  SemMemory[ADI_OSAL_MAX_SEM_SIZE_CHAR];
                                                /* Semaphore Memory             */
#else
    bool_t                  bInterruptFlag;     /* Flag for low power mode      */
#endif /* ADI_CFG_ENABLE_RTOS_SUPPORT  */
    uint8_t*                 pTxBuffer;          /*!< current transmit pointer  */
    uint8_t*                 pRxBuffer;          /*!< current receive pointer   */
    uint8_t                  TxIncrement;        /*!< Transmit Increment        */
    uint8_t                  RxIncrement;        /*!< Receive Increment         */
    uint32_t                 TxRemaining;        /*!< remaining transmit Count  */
    uint32_t                 RxRemaining;        /*!< remaining receive Count   */
    volatile bool_t          bComplete;          /*!< transfer complete flag    */

    /* function table */
    ADI_SPI_FTAB             FunctionTable;

    /* Callback and Callback parameters */
    ADI_CALLBACK             pfCallback;
    void *                   pCBParam;

#if (ADI_SPI_CFG_ENABLE_DMA_SUPPORT == 1)
    /* DMA-Mode state */
    ADI_DMA_TRANSFER_TYPE*   pDmaDescriptor;   /*!< DMA descriptor pointer */
    DMA_CHANn_TypeDef        DMA_TX_Chan;      /*!< DMA TX channel number  */
    DMA_CHANn_TypeDef        DMA_RX_Chan;      /*!< DMA RX channel number  */
    IRQn_Type                DMA_TX_IRQn;      /*!< DMA TX IRQ number      */
    IRQn_Type                DMA_RX_IRQn;      /*!< DMA RX IRQ number      */
    ADI_DMA_CCD_TYPE         nextTxDescr;      /*!< DMA next TX descriptor */
    ADI_DMA_CCD_TYPE         nextRxDescr;      /*!< DMA next RX descriptor */
#endif /* ADI_SPI_CFG_ENABLE_DMA_SUPPORT */
} ADI_SPI_DEV_DATA_TYPE;

/* allocate instance data array */
ADI_SPI_DEV_DATA_TYPE SPI_DevData[ADI_SPI_MAX_DEVID];  /*!< Private SPI device driver instance data */


/* DMA event logging (debugging only) */
//#define DMA_DEBUGGING
#ifdef DMA_DEBUGGING
#define DMA_EVENT_TABLE_SIZE 100
typedef struct {
    uint16_t txPri, txAlt, txInt, rxPri, rxAlt, rxInt;
} DMA_EVENT_TYPE;
DMA_EVENT_TYPE dmaEventTable[DMA_EVENT_TABLE_SIZE] = {0};
uint16_t dmaEventIndex=0;
#endif


#if (ADI_SPI_CFG_ENABLE_STATIC_CONFIG_SUPPORT == 1)
/*!
 *****************************************************************************
 * SPI Configuration structure.
 *****************************************************************************/
typedef struct ADI_SPI_CONFIG
{
    uint16_t SPI_CFG;                          /*!< SPI_CFG register configuration. */
    uint16_t SPI_DIV;                          /*!< SPI_DIV buad rate register.     */
} ADI_SPI_CFG_TYPE;

/* SPI Application configuration array */
static const ADI_SPI_CFG_TYPE gSPICfg[ADI_SPI_MAX_DEVID] =
{
  /* Initialize SPI0 Instance configuration. */
  {
       /**** SPI_CFG register configuration *** */
       (( SPI0_CFG_ENABLE               << BITP_SPI_SPICON_ENABLE        ) |
        ( SPI0_CFG_MASTER_SLAVE         << BITP_SPI_SPICON_MASEN         ) |
        ( SPI0_CFG_CLK_PHASE            << BITP_SPI_SPICON_CPHA          ) |
        ( SPI0_CFG_CLK_POLARITY         << BITP_SPI_SPICON_CPOL          ) |
        ( SPI0_CFG_WIRED_OR             << BITP_SPI_SPICON_WOM           ) |
        ( SPI0_CFG_LSB_MSB              << BITP_SPI_SPICON_LSB           ) |
        ( SPI0_CFG_TRANSFER_INITIATE    << BITP_SPI_SPICON_TIM           ) |
        ( SPI0_CFG_TX_UNDERFLOW         << BITP_SPI_SPICON_ZEN           ) |
        ( SPI0_CFG_RX_OVERFLOW          << BITP_SPI_SPICON_RXOF          ) |
        ( SPI0_CFG_MISO_ENABLE          << BITP_SPI_SPICON_OEN           ) |
        ( SPI0_CFG_LOOPBACK             << BITP_SPI_SPICON_LOOPBACK      ) |
        ( SPI0_CFG_CONTINUOUS           << BITP_SPI_SPICON_CON           ) |
        ( SPI0_CFG_RX_FLUSH             << BITP_SPI_SPICON_RFLUSH        ) |
        ( SPI0_CFG_TX_FLUSH             << BITP_SPI_SPICON_TFLUSH        ) |
        ( SPI0_CFG_IRQ_MODE             << BITP_SPI_SPICON_MOD           )),

       /**** SPI_DIV buad rate selection register *** */
       (( ((ADI_CFG_SYSTEM_CLOCK_HZ / ADI_SPI0_CFG_BIT_RATE_HZ * 2)+1)\
                                        << BITP_SPI_SPIDIV_DIV           ) |
        ( SPI0_CFG_HFM                  << BITP_SPI_SPIDIV_HFM           ) |
        ( SPI0_CFG_CS_ERR               << BITP_SPI_SPIDIV_MD_CS_RST     ) |
        ( SPI0_CFG_CS_IRQ               << BITP_SPI_SPIDIV_CSIRQ_EN      ))
    },
  /* Initialize SPI1 Instance configuration. */
  {
       /**** SPI_CFG register configuration *** */
       (( SPI1_CFG_ENABLE               << BITP_SPI_SPICON_ENABLE        ) |
        ( SPI1_CFG_MASTER_SLAVE         << BITP_SPI_SPICON_MASEN         ) |
        ( SPI1_CFG_CLK_PHASE            << BITP_SPI_SPICON_CPHA          ) |
        ( SPI1_CFG_CLK_POLARITY         << BITP_SPI_SPICON_CPOL          ) |
        ( SPI1_CFG_WIRED_OR             << BITP_SPI_SPICON_WOM           ) |
        ( SPI1_CFG_LSB_MSB              << BITP_SPI_SPICON_LSB           ) |
        ( SPI1_CFG_TRANSFER_INITIATE    << BITP_SPI_SPICON_TIM           ) |
        ( SPI1_CFG_TX_UNDERFLOW         << BITP_SPI_SPICON_ZEN           ) |
        ( SPI1_CFG_RX_OVERFLOW          << BITP_SPI_SPICON_RXOF          ) |
        ( SPI1_CFG_MISO_ENABLE          << BITP_SPI_SPICON_OEN           ) |
        ( SPI1_CFG_LOOPBACK             << BITP_SPI_SPICON_LOOPBACK      ) |
        ( SPI1_CFG_CONTINUOUS           << BITP_SPI_SPICON_CON           ) |
        ( SPI1_CFG_RX_FLUSH             << BITP_SPI_SPICON_RFLUSH        ) |
        ( SPI1_CFG_TX_FLUSH             << BITP_SPI_SPICON_TFLUSH        ) |
        ( SPI1_CFG_IRQ_MODE             << BITP_SPI_SPICON_MOD           )),

       /**** SPI_DIV buad rate selection register *** */
       (( ((ADI_CFG_SYSTEM_CLOCK_HZ / ADI_SPI1_CFG_BIT_RATE_HZ * 2)+1)\
                                        << BITP_SPI_SPIDIV_DIV           ) |
        ( SPI1_CFG_HFM                  << BITP_SPI_SPIDIV_HFM           ) |
        ( SPI1_CFG_CS_ERR               << BITP_SPI_SPIDIV_MD_CS_RST     ) |
        ( SPI1_CFG_CS_IRQ               << BITP_SPI_SPIDIV_CSIRQ_EN      ))
    },

  /* Initialize SPIH Instance configuration. */
  {
       /**** SPI_CFG register configuration *** */
       (( SPIH_CFG_ENABLE               << BITP_SPI_SPICON_ENABLE        ) |
        ( SPIH_CFG_MASTER_SLAVE         << BITP_SPI_SPICON_MASEN         ) |
        ( SPIH_CFG_CLK_PHASE            << BITP_SPI_SPICON_CPHA          ) |
        ( SPIH_CFG_CLK_POLARITY         << BITP_SPI_SPICON_CPOL          ) |
        ( SPIH_CFG_WIRED_OR             << BITP_SPI_SPICON_WOM           ) |
        ( SPIH_CFG_LSB_MSB              << BITP_SPI_SPICON_LSB           ) |
        ( SPIH_CFG_TRANSFER_INITIATE    << BITP_SPI_SPICON_TIM           ) |
        ( SPIH_CFG_TX_UNDERFLOW         << BITP_SPI_SPICON_ZEN           ) |
        ( SPIH_CFG_RX_OVERFLOW          << BITP_SPI_SPICON_RXOF          ) |
        ( SPIH_CFG_MISO_ENABLE          << BITP_SPI_SPICON_OEN           ) |
        ( SPIH_CFG_LOOPBACK             << BITP_SPI_SPICON_LOOPBACK      ) |
        ( SPIH_CFG_CONTINUOUS           << BITP_SPI_SPICON_CON           ) |
        ( SPIH_CFG_RX_FLUSH             << BITP_SPI_SPICON_RFLUSH        ) |
        ( SPIH_CFG_TX_FLUSH             << BITP_SPI_SPICON_TFLUSH        ) |
        ( SPIH_CFG_IRQ_MODE             << BITP_SPI_SPICON_MOD           )),

       /**** SPI_DIV buad rate selection register *** */
       (( ((ADI_CFG_SYSTEM_CLOCK_HZ / ADI_SPIH_CFG_BIT_RATE_HZ * 2)+1)\
                                        << BITP_SPI_SPIDIV_DIV           ) |
        ( SPIH_CFG_HFM                  << BITP_SPI_SPIDIV_HFM           ) |
        ( SPIH_CFG_CS_ERR               << BITP_SPI_SPIDIV_MD_CS_RST     ) |
        ( SPIH_CFG_CS_IRQ               << BITP_SPI_SPIDIV_CSIRQ_EN      ))
    }
};
#endif /* ADI_SPI_CFG_ENABLE_STATIC_CONFIG_SUPPORT */

/* internal device data pointers */
static ADI_SPI_DEV_DATA_TYPE* const pSPI0_DevData = &SPI_DevData[ADI_SPI_DEVID_0];
static ADI_SPI_DEV_DATA_TYPE* const pSPI1_DevData = &SPI_DevData[ADI_SPI_DEVID_1];
static ADI_SPI_DEV_DATA_TYPE* const pSPIH_DevData = &SPI_DevData[ADI_SPI_DEVID_H];

/* handle checker for debug mode */
#ifdef ADI_DEBUG
#define ADI_SPI_INVALID_HANDLE(h) ((pSPI0_DevData != h) && (pSPI1_DevData != h) && (pSPIH_DevData != h))
#endif

/* forward declarations... */
/* These are needed in the case where ADI_CFG_ENABLE_RTOS_SUPPORT=1              */
/* The ADI_INSTALL_HANDLER macro will expand to a call to adi_int_InstallHandler */
/* and the handler will be referenced                                            */
/* The non-OSAL expansion of ADI_INSTALL_HANDLER results in a NULL string        */
ADI_INT_HANDLER(SPI0_Int_Handler);
ADI_INT_HANDLER(SPI1_Int_Handler);
ADI_INT_HANDLER(SPIH_Int_Handler);
ADI_INT_HANDLER(DMA_SPI0_TX_Int_Handler);
ADI_INT_HANDLER(DMA_SPI0_RX_Int_Handler);
ADI_INT_HANDLER(DMA_SPI1_TX_Int_Handler);
ADI_INT_HANDLER(DMA_SPI1_RX_Int_Handler);
ADI_INT_HANDLER(DMA_SPIH_TX_Int_Handler);
ADI_INT_HANDLER(DMA_SPIH_RX_Int_Handler);

  /* Internal API... */
static ADI_SPI_RESULT_TYPE ConfigureCoreAsMaster    (ADI_SPI_DEV_HANDLE const hDevice);
static ADI_SPI_RESULT_TYPE ConfigureCoreAsSlave     (ADI_SPI_DEV_HANDLE const hDevice);

static ADI_SPI_RESULT_TYPE ConfigureMasterCS        (ADI_SPI_DEV_HANDLE const hDevice,
                                                     bool_t const bActivate);

static ADI_SPI_RESULT_TYPE AssertChipSelect         (ADI_SPI_DEV_HANDLE const hDevice,
                                                     bool_t enable);

static ADI_SPI_RESULT_TYPE CommonInit               (ADI_SPI_DEV_HANDLE const hDevice,
                                                     ADI_SPI_DEV_ID_TYPE const devID);


/* pointer table function prototypes... */
static ADI_SPI_RESULT_TYPE intInit                  (ADI_SPI_DEV_HANDLE const hDevice);
static ADI_SPI_RESULT_TYPE intUnInit                (ADI_SPI_DEV_HANDLE const hDevice);
static ADI_SPI_RESULT_TYPE intInitializeDescriptors (ADI_SPI_DEV_HANDLE const hDevice,
                                                     const uint8_t*     const pDataTx,
                                                     uint8_t*           const pDataRx,
                                                     uint32_t           const Size);
static void                intCommonHandler         (DMA_CHANn_TypeDef channelNum);

#if (ADI_SPI_CFG_ENABLE_DMA_SUPPORT == 1)
static ADI_SPI_RESULT_TYPE dmaInit                  (ADI_SPI_DEV_HANDLE const hDevice);
static ADI_SPI_RESULT_TYPE dmaUnInit                (ADI_SPI_DEV_HANDLE const hDevice);
static ADI_SPI_RESULT_TYPE dmaInitializeDescriptors (ADI_SPI_DEV_HANDLE const hDevice,
                                                     const uint8_t*     const pDataTx,
                                                     uint8_t*           const pDataRx,
                                                     uint32_t           const Size);
static void                dmaCommonHandler         (DMA_CHANn_TypeDef channelNum);
static ADI_SPI_RESULT_TYPE ScheduleNextDescriptor   (ADI_SPI_DEV_HANDLE const hDevice,
                                                     DMA_CHANn_TypeDef const chan);
static ADI_SPI_RESULT_TYPE EnableDma                (ADI_SPI_DEV_HANDLE const hDevice,
                                                     const bool_t bFlag);
static ADI_SPI_RESULT_TYPE EnableDmaTxIRQ           (ADI_SPI_DEV_HANDLE const hDevice,
                                                     const bool_t bFlag);
static ADI_SPI_RESULT_TYPE EnableDmaRxIRQ           (ADI_SPI_DEV_HANDLE const hDevice,
                                                     const bool_t bFlag);
#endif /* ADI_SPI_CFG_ENABLE_DMA_SUPPORT */

/*! \endcond */

/*
    //////////////////////////////////////////////////////////////////////////////
    //////////////////////   API IMPLEMENTATIONS   ///////////////////////////////
    //////////////////////////////////////////////////////////////////////////////
*/

/*!
 * @brief  Initialize and allocate an SPI device for use in Master Mode.
 *
 * @param[in]        devID      Zero-based device index designating which device to initialize.
 * @param[out]       phDevice   The caller's device handle pointer for storing the initialized device instance data pointer.
 *
 * @return            Status
 *                    - #ADI_SPI_ERR_ALREADY_INITIALIZED [D]    Specified device has already been initialized.
 *                    - #ADI_SPI_ERR_BAD_DEV_ID [D]             Invalid device index.
 *                    - #ADI_SPI_ERR_BAD_DMA_CONFIG             Internal DMA initializations failed.
 *                    - #ADI_SPI_ERR_BAD_GPIO_CONFIG            Internal GPIO initializations failed.
 *                    - #ADI_SPI_ERR_SEMAPHORE_FAILED           Semaphore creation failed.
 *                    - #ADI_SPI_SUCCESS                        Call completed successfully.
 *
 * No other SPI APIs may be called until the device init function is called.
 *
 * Initialize an SPI device using internal default configuration settings and allocate the device for use.  The caller's
 * device handle is written with the device instance data pointer.  The returned device handle is required to be passed
 * to all subsequent calls to convey which device instance to operate on.
 *
 * @note    The contents of phDevice will be set to NULL upon failure.
 *
 * @sa      adi_SPI_SlaveInit().
 * @sa      adi_SPI_UnInit().
 */
ADI_SPI_RESULT_TYPE adi_SPI_MasterInit (ADI_SPI_DEV_ID_TYPE const devID,
                                        ADI_SPI_DEV_HANDLE* const phDevice)
{
    ADI_SPI_DEV_HANDLE hDevice = &SPI_DevData[devID];
    ADI_SPI_RESULT_TYPE result = ADI_SPI_SUCCESS;

    /* store a bad handle in case of failure */
    *phDevice = (ADI_SPI_DEV_HANDLE) NULL;

#ifdef ADI_DEBUG
    if (devID >= ADI_SPI_MAX_DEVID)
        return ADI_SPI_ERR_BAD_DEV_ID;

    if (ADI_SPI_STATE_UNINITIALIZED != hDevice->InitState)
        return ADI_SPI_ERR_ALREADY_INITIALIZED;
#endif

    /* initialize GPIO */
   if (ADI_GPIO_SUCCESS !=  adi_GPIO_Init()) {
        hDevice->InitState = ADI_SPI_STATE_UNINITIALIZED;
        return ADI_SPI_ERR_BAD_GPIO_CONFIG;
    }

    result = CommonInit( hDevice, devID);
    if( result != ADI_SPI_SUCCESS )
    {
      return result;
    }

#if (ADI_SPI_CFG_ENABLE_STATIC_CONFIG_SUPPORT == 1)
    {
      ADI_SPI_CFG_TYPE const* pSPICfg = &gSPICfg[devID];

      /* Initialize the device based on the given configuration parameters */
      hDevice->pSpi->SPICON = pSPICfg->SPI_CFG;
      hDevice->pSpi->SPIDIV = pSPICfg->SPI_DIV;

    }
#endif  /* ADI_SPI_CFG_ENABLE_STATIC_CONFIG_SUPPORT */


    /* mark device initialization in progress                             */
    /* "ADI_SPI_STATE_INITIALIZING" allows use of common set bitrate      */
    /* function (during "ConfigureCoreAsMaster") function to pass without */
    /* failing on "ADI_SPI_STATE_UNINITIALIZED"                           */
    hDevice->InitState = ADI_SPI_STATE_INITIALIZING;

	/* configure the default CS port/pin, as already designated by CommonInit() */
    if (ADI_SPI_SUCCESS != ConfigureMasterCS (hDevice, true)) {
        hDevice->InitState = ADI_SPI_STATE_UNINITIALIZED;
        return ADI_SPI_ERR_BAD_GPIO_CONFIG;
    }

	/* configure core for Master-Mode operation */
    if (ADI_SPI_SUCCESS != (result = ConfigureCoreAsMaster (hDevice))) {
        hDevice->InitState = ADI_SPI_STATE_UNINITIALIZED;
        return result;
    }

    /* enable the dedicated SPI core clock */
    if (pSPI0_DevData == hDevice)    SystemEnableClock(ADI_SYS_CLOCK_GATE_SPI0, true);
    if (pSPI1_DevData == hDevice)    SystemEnableClock(ADI_SYS_CLOCK_GATE_SPI1, true);
    if (pSPIH_DevData == hDevice)    SystemEnableClock(ADI_SYS_CLOCK_GATE_SPIH, true);

    /* mark device fully initialized */
    hDevice->InitState = ADI_SPI_STATE_MASTER;

    /* write the device data pointer into the caller's handle */
    *phDevice = hDevice;

   return result;
}


/*!
 * @brief  Initialize and allocate an SPI device for use in Slave Mode.
 *
 * @param[in]        devID      Zero-based device index designating which device to initialize.
 * @param[out]       phDevice   The caller's device handle pointer for storing the initialized device instance data.
 *
 * @return            Status
 *                    - #ADI_SPI_ERR_ALREADY_INITIALIZED [D]    Specified device has already been initialized.
 *                    - #ADI_SPI_ERR_BAD_DEV_ID [D]             Invalid device index.
 *                    - #ADI_SPI_ERR_BAD_DMA_CONFIG             Internal DMA initializations failed.
 *                    - #ADI_SPI_ERR_BAD_GPIO_CONFIG            Internal GPIO initializations failed.
 *                    - #ADI_SPI_ERR_SEMAPHORE_FAILED           Semaphore creation failed.
 *                    - #ADI_SPI_SUCCESS                        Call completed successfully.
 *
 * No other SPI APIs may be called until the device init function is called.
 *
 * Initialize an SPI device using internal default configuration settings and allocate the device for use.  The caller's
 * device handle is written with the device instance data pointer.  The returned device handle is required to be passed
 * to all subsequent calls to convey which device instance to operate on.
 *
 * @note    The contents of phDevice will be set to NULL upon failure.
 *
 * @sa      adi_SPI_MasterInit().
 * @sa      adi_SPI_UnInit().
 */
ADI_SPI_RESULT_TYPE adi_SPI_SlaveInit (ADI_SPI_DEV_ID_TYPE const devID,
                                       ADI_SPI_DEV_HANDLE* const phDevice)
{

    ADI_SPI_DEV_HANDLE hDevice;
    ADI_SPI_RESULT_TYPE result;

    /* store a bad handle in case of failure */
    *phDevice = (ADI_SPI_DEV_HANDLE) NULL;

    hDevice = &SPI_DevData[devID];

    result = CommonInit( hDevice, devID);
    if( result != ADI_SPI_SUCCESS )
    {
      return result;
    }

#ifdef ADI_DEBUG
    /* check/set initialization state */
    if (ADI_SPI_STATE_UNINITIALIZED != hDevice->InitState)
        return ADI_SPI_ERR_ALREADY_INITIALIZED;

    if (ADI_SPI_MAX_DEVID <= devID)
            return ADI_SPI_ERR_BAD_DEV_ID;
#endif

    /* initialize GPIO  */
   if (ADI_GPIO_SUCCESS !=  adi_GPIO_Init()) {
        hDevice->InitState = ADI_SPI_STATE_UNINITIALIZED;
        return ADI_SPI_ERR_BAD_GPIO_CONFIG;
    }

    /* mark device initialization in progress */
    /* "ADI_SPI_STATE_INITIALIZING" allows use of common set bitrate */
    /* function (during "ConfigureCoreAsSlave") function  to pass without */
    /* failing on "ADI_SPI_STATE_UNINITIALIZED" */
    hDevice->InitState = ADI_SPI_STATE_INITIALIZING;

    /* configure core for Slave-Mode operation */
    if (ADI_SPI_SUCCESS != (result = ConfigureCoreAsSlave (hDevice))) {
        hDevice->InitState = ADI_SPI_STATE_UNINITIALIZED;
        return result;
    }

    /* enable FIFO Interrupts */
    ADI_ENABLE_INT(hDevice->IRQn);

    /* enable the dedicated SPI core clock */
    if (pSPI0_DevData == hDevice)    SystemEnableClock(ADI_SYS_CLOCK_GATE_SPI0, true);
    if (pSPI1_DevData == hDevice)    SystemEnableClock(ADI_SYS_CLOCK_GATE_SPI1, true);
    if (pSPIH_DevData == hDevice)    SystemEnableClock(ADI_SYS_CLOCK_GATE_SPIH, true);

    /* mark device fully initialized */
    hDevice->InitState = ADI_SPI_STATE_SLAVE;

    /* write the device data pointer into the caller's handle */
    *phDevice = hDevice;

    return result;
}



/*!
 * @brief  Uninitialize and deallocate an SPI device.
 *
 * @param[in]   hDevice    Device handle obtained from adi_SPI_MasterInit() or adi_SPI_SlaveInit().
 *
 * @return      Status
 *                - #ADI_SPI_ERR_BAD_DEV_HANDLE [D]     Invalid device handle parameter.
 *                - #ADI_SPI_ERR_BAD_DMA_CONFIG         Internal DMA operations failed.
 *                - #ADI_SPI_ERR_BAD_GPIO_CONFIG        Internal GPIO operations failed.
 *                - #ADI_SPI_ERR_NOT_INITIALIZED [D]    Device has not been previously configured for use.
 *                - #ADI_SPI_SUCCESS                    Call completed successfully.
 *
 * Uninitialize and release an allocated SPI device for other use.
 *
 * @sa        adi_SPI_MasterInit().
 */
ADI_SPI_RESULT_TYPE adi_SPI_UnInit (ADI_SPI_DEV_HANDLE const hDevice)
{

    ADI_SPI_RESULT_TYPE result = ADI_SPI_SUCCESS;

#ifdef ADI_DEBUG
    if (ADI_SPI_INVALID_HANDLE(hDevice))
        return ADI_SPI_ERR_BAD_DEV_HANDLE;

    if ((hDevice->InitState != ADI_SPI_STATE_MASTER) && (hDevice->InitState != ADI_SPI_STATE_SLAVE))
        return ADI_SPI_ERR_NOT_INITIALIZED ;
#endif

    /* disable Interrupt    */
    ADI_DISABLE_INT(hDevice->IRQn);
    ADI_UNINSTALL_HANDLER(hDevice->IRQn);

#if (ADI_CFG_ENABLE_RTOS_SUPPORT == 1)
     /* destroy semaphore */
     if (adi_osal_SemDestroyStatic(hDevice->hSem) != ADI_OSAL_SUCCESS)
     {
        return ADI_SPI_ERR_SEMAPHORE_FAILED;
     }
#endif /* ADI_CFG_ENABLE_RTOS_SUPPORT */

    /* uninit internals */
    if (ADI_SPI_SUCCESS != hDevice->FunctionTable.pUnInitFcn(hDevice))
        return ADI_SPI_ERR_BAD_DMA_CONFIG;

   /* Uninitialize the GPIO driver */
    if (ADI_GPIO_SUCCESS != adi_GPIO_UnInit())
        return ADI_SPI_ERR_BAD_GPIO_CONFIG;

    /* disable the SPI controller */
    hDevice->pSpi->SPICON = 0;

    /* disable the dedicated SPI core clock */
    if (pSPI0_DevData == hDevice)    SystemEnableClock(ADI_SYS_CLOCK_GATE_SPI0, false);
    if (pSPI1_DevData == hDevice)    SystemEnableClock(ADI_SYS_CLOCK_GATE_SPI1, false);
    if (pSPIH_DevData == hDevice)    SystemEnableClock(ADI_SYS_CLOCK_GATE_SPIH, false);

    /* invalidate initialization state */
    hDevice->InitState = ADI_SPI_STATE_UNINITIALIZED;

    return result;
}

/*!
 * @brief  Register or unregister the callback.
 *
 * @param [in]   hDevice        Device handle obtained from adi_SPI_Master/SlaveInit().
 * @param [in]   pfCallback     Pointer to the callback function. Can be passed as NULL to unregister the
 *                              previously registered callback.
 * @param [in]   pCBParam       Callback parameter which will be passed back to the application when the
 *                              callback is called.
 *
 * @return      Status
 *                - #ADI_SPI_ERR_BAD_DEV_HANDLE [D]  Invalid device handle parameter.
 *                - #ADI_SPI_ERR_NOT_INITIALIZED   [D]  Device has not been previously configured for use.
 *                - #ADI_SPI_SUCCESS                    Call completed successfully.
 */
ADI_SPI_RESULT_TYPE adi_SPI_RegisterCallback      (ADI_SPI_DEV_HANDLE const hDevice,  ADI_CALLBACK const pfCallback, void *const pCBParam )
{
#ifdef ADI_DEBUG
    if (ADI_SPI_STATE_UNINITIALIZED == hDevice->InitState)
        return ADI_SPI_ERR_NOT_INITIALIZED;

#endif

    /* Save the application provided callback and callback parameters */
    hDevice->pfCallback = pfCallback;
    hDevice->pCBParam   = pCBParam;

    return ADI_SPI_SUCCESS;
}

/*!
 * @brief  Set generic SPI settings.
 *
 * @param[in]      hDevice      Device handle obtained from adi_SPI_MasterInit() or adi_SPI_SlaveInit().
 * @param[in]      pSettings    Pointer to caller-configured generic device settings structure.
 *
 * @return         Status
 *                - #ADI_SPI_ERR_BAD_DEV_HANDLE [D]         Invalid device handle parameter.
 *                - #ADI_SPI_ERR_NOT_INITIALIZED [D]        Device has not been previously configured for use.
 *                - #ADI_SPI_ERR_PARAM_OUT_OF_RANGE [D]     Invalid or NULL pSettings parameter.
 *                - #ADI_SPI_SUCCESS                        Call completed successfully.
 *
 * Configures various generic device settings and modes in a single call.
 *
 * @sa        adi_SPI_GetGenericSettings().
 */
ADI_SPI_RESULT_TYPE adi_SPI_SetGenericSettings (ADI_SPI_DEV_HANDLE const hDevice, ADI_SPI_DEV_GENERIC_SETTINGS_TYPE* const pSettings)
{

    ADI_SPI_RESULT_TYPE result = ADI_SPI_SUCCESS;

#ifdef ADI_DEBUG
    if (ADI_SPI_INVALID_HANDLE(hDevice))
        return ADI_SPI_ERR_BAD_DEV_HANDLE;

    if ((hDevice->InitState != ADI_SPI_STATE_MASTER) && (hDevice->InitState != ADI_SPI_STATE_SLAVE))
        return ADI_SPI_ERR_NOT_INITIALIZED ;

    /* fail if no settings */
    if (NULL == pSettings)
        return ADI_SPI_ERR_PARAM_OUT_OF_RANGE;
#endif

    if (ADI_SPI_SUCCESS != (result = adi_SPI_SetBitrate(hDevice, pSettings->Bitrate)))
        return result;

    if (ADI_SPI_SUCCESS != (result = adi_SPI_SetDmaMode(hDevice, pSettings->bDmaMode)))
        return result;

    /* set chp select, blocking mode and dma mode */
    hDevice->ChipSelect    = pSettings->cs;
    hDevice->bBlockingMode = pSettings->bBlockingMode;
    hDevice->bDmaMode      = pSettings->bDmaMode;

    return result;
}


/*!
 * @brief  Get generic SPI settings.
 *
 * @param[in]      hDevice      Device handle obtained from adi_SPI_MasterInit() or adi_SPI_SlaveInit().
 * @param[out]     pSettings    Pointer to caller-allocated generic device settings structure.
 *
 * @return         Status
 *                - #ADI_SPI_ERR_BAD_DEV_HANDLE [D]         Invalid device handle parameter.
 *                - #ADI_SPI_ERR_NOT_INITIALIZED [D]        Device has not been previously configured for use.
 *                - #ADI_SPI_ERR_PARAM_OUT_OF_RANGE [D]     Invalid or NULL pSettings parameter.
 *                - #ADI_SPI_SUCCESS                        Call completed successfully.
 *
 * Retrieves various generic device settings and modes in a single call.  Contents of caller's pSettings structure are written on success.
 *
 * @sa        adi_SPI_GetGenericSettings().
 */
ADI_SPI_RESULT_TYPE adi_SPI_GetGenericSettings (ADI_SPI_DEV_HANDLE const hDevice, ADI_SPI_DEV_GENERIC_SETTINGS_TYPE* const pSettings)
{

#ifdef ADI_DEBUG
    if (ADI_SPI_INVALID_HANDLE(hDevice))
        return ADI_SPI_ERR_BAD_DEV_HANDLE;

    if ((hDevice->InitState != ADI_SPI_STATE_MASTER) && (hDevice->InitState != ADI_SPI_STATE_SLAVE))
        return ADI_SPI_ERR_NOT_INITIALIZED ;

    /* fail if no settings */
    if (NULL == pSettings)
        return ADI_SPI_ERR_PARAM_OUT_OF_RANGE;
#endif

    pSettings->Bitrate       = adi_SPI_GetBitrate(hDevice);
    pSettings->cs            = adi_SPI_GetChipSelect(hDevice);
    pSettings->bBlockingMode = adi_SPI_GetBlockingMode(hDevice);
    pSettings->bDmaMode      = adi_SPI_GetDmaMode(hDevice);

    return ADI_SPI_SUCCESS;
}


/*!
 * @brief  Set SPI IRQ mode.
 *
 * @param[in]      hDevice      Device handle obtained from adi_SPI_MasterInit() or adi_SPI_SlaveInit().
 * @param[in]      Mode         Designating new FIFO interrupt mode, based on their relative fullness or emptiness.
 *
 * @return         Status
 *                - #ADI_SPI_ERR_BAD_DEV_HANDLE [D]         Invalid device handle parameter.
 *                - #ADI_SPI_ERR_NOT_INITIALIZED [D]        Device has not been previously configured for use.
 *                - #ADI_SPI_ERR_PARAM_OUT_OF_RANGE         Invalid or NULL Mode parameter.
 *                - #ADI_SPI_SUCCESS                        Call completed successfully.
 *
 * Configures when the SPI FIFOs generate interrupts, based on their relative "fullness".
 * See user manual for mode descriptions.
 *
 * @note    For DMA Rx transfers (SPI1 only), these bits should be set to '00' (ADI_SPI_IRQMODE_T1R1).
 *
 * @sa      adi_SPI_GetInterruptMode().
 */
ADI_SPI_RESULT_TYPE adi_SPI_SetInterruptMode (ADI_SPI_DEV_HANDLE const hDevice, const ADI_SPI_IRQ_MODE_TYPE Mode)
{

    uint16_t modeBits;

#ifdef ADI_DEBUG
    if (ADI_SPI_INVALID_HANDLE(hDevice))
        return ADI_SPI_ERR_BAD_DEV_HANDLE;

    if ((hDevice->InitState != ADI_SPI_STATE_MASTER) && (hDevice->InitState != ADI_SPI_STATE_SLAVE))
        return ADI_SPI_ERR_NOT_INITIALIZED;
#endif

    /* NOTE: the interrupt mode enum has gaps in the expected values */
    /* because the hardware does not support all combinations */
    switch (Mode) {

        case ADI_SPI_IRQMODE_T1R1:
            modeBits = SPICON_MOD_TX1RX1;
            break;

        case ADI_SPI_IRQMODE_T2R2:
            modeBits = SPICON_MOD_TX2RX2;
            break;

        case ADI_SPI_IRQMODE_T3R3:
            modeBits = SPICON_MOD_TX3RX3;
            break;

        case ADI_SPI_IRQMODE_T4R4:
            modeBits = SPICON_MOD_TX4RX4;
            break;

        default:
            return ADI_SPI_ERR_PARAM_OUT_OF_RANGE;
    }

    /* stop interrupts while changing interrupt mode */
    ADI_ENTER_CRITICAL_REGION();

    /* clear existing, then set new mode... */
    hDevice->pSpi->SPICON &= ~(SPICON_MOD_MSK);
    hDevice->pSpi->SPICON |= modeBits;

    /* reenable interrupts */
    ADI_EXIT_CRITICAL_REGION();

    return ADI_SPI_SUCCESS;
}


/*!
 * @brief  Get SPI IRQ mode.
 *
 * @param[in]    hDevice     Device handle obtained from adi_SPI_MasterInit() or adi_SPI_SlaveInit().
 *
 * Retrieves the SPI FIFOs interrupt generation mode, based on their relative "fullness".
 *
 * @return       Returns the current setting for SPI FIFO interrupt mode, based on their relative "fullness".
 *
 * @sa           adi_SPI_SetInterruptMode().
 */
ADI_SPI_IRQ_MODE_TYPE adi_SPI_GetInterruptMode (ADI_SPI_DEV_HANDLE const hDevice)
{

    ADI_SPI_IRQ_MODE_TYPE rtn;

    switch (hDevice->pSpi->SPICON & SPICON_MOD_MSK) {

        case SPICON_MOD_TX1RX1:
            rtn = ADI_SPI_IRQMODE_T1R1;
            break;

        case SPICON_MOD_TX2RX2:
            rtn = ADI_SPI_IRQMODE_T2R2;
            break;

        case SPICON_MOD_TX3RX3:
            rtn = ADI_SPI_IRQMODE_T3R3;
            break;

        case SPICON_MOD_TX4RX4:
            rtn = ADI_SPI_IRQMODE_T4R4;
            break;
    }

    return rtn;
}


/*!
 * @brief  Set the SPI transmit FIFO flush mode.
 *
 * @param[in]    hDevice      Device handle obtained from adi_SPI_MasterInit() or adi_SPI_SlaveInit().
 * @param[in]    bFlag        Flag to manage FIFO transmit flush mode.
 *                - true     Set transmit flush mode.
 *                - false    Clear transmit flush mode.
 *
 * @return        Status
 *                - #ADI_SPI_ERR_BAD_DEV_HANDLE [D]         Invalid device handle parameter.
 *                - #ADI_SPI_ERR_NOT_INITIALIZED [D]        Device has not been previously configured for use.
 *                - #ADI_SPI_SUCCESS                        Call completed successfully.
 *
 * Set the transmit FIFO flush mode to flush the FIFO and/or keep it flushed.
 * The transmit FIFO is essentially disabled while this mode is set.  Clear it
 * for normal FIFO operation.  A one-time flush is obtained by setting this
 * mode and then clearing it.
 *
 * @note This mode is NOT self-clearing.  Clear this mode to prevent continuous transmit FIFO flushing.
 *
 * @sa        adi_SPI_GetFifoRxFlush().
 * @sa        adi_SPI_GetFifoTxFlush().
 * @sa        adi_SPI_SetFifoRxFlush().
 */
ADI_SPI_RESULT_TYPE adi_SPI_SetFifoTxFlush (ADI_SPI_DEV_HANDLE const hDevice, const bool_t bFlag)
{

#ifdef ADI_DEBUG
    if (ADI_SPI_INVALID_HANDLE(hDevice))
        return ADI_SPI_ERR_BAD_DEV_HANDLE;

    if ((hDevice->InitState != ADI_SPI_STATE_MASTER) && (hDevice->InitState != ADI_SPI_STATE_SLAVE))
        return ADI_SPI_ERR_NOT_INITIALIZED;
#endif

    if (bFlag) {
        hDevice->pSpi->SPICON |= (SPICON_TFLUSH);
    } else {
        hDevice->pSpi->SPICON &= ~(SPICON_TFLUSH);
    }

    return ADI_SPI_SUCCESS;
}


/*!
 * @brief  Get the SPI transmit FIFO flush mode.
 *
 * @param[in]     hDevice      Device handle obtained from adi_SPI_MasterInit() or adi_SPI_SlaveInit().
 *
 * @return Returns the current state of the transmit FIFO flush mode.
 *                - true      Transmit FIFO flush mode is set.
 *                - false     Transmit FIFO flush mode is cleared.
 *
 * @sa        adi_SPI_GetFifoRxFlush().
 * @sa        adi_SPI_SetFifoRxFlush().
 * @sa        adi_SPI_SetFifoTxFlush().
 */
bool_t adi_SPI_GetFifoTxFlush (ADI_SPI_DEV_HANDLE const hDevice) {

    return (hDevice->pSpi->SPICON & SPICON_TFLUSH) ? true : false;
}


/*!
 * @brief  Set the SPI receive FIFO flush mode.
 *
 * @param[in]    hDevice      Device handle obtained from adi_SPI_MasterInit() or adi_SPI_SlaveInit().
 * @param[in]    bFlag        Flag to manage FIFO receive flush mode.
 *                - true     Set receive flush mode.
 *                - false    Clear receive flush mode.
 *
 * @return         Status
 *                - #ADI_SPI_ERR_BAD_DEV_HANDLE [D]         Invalid device handle parameter.
 *                - #ADI_SPI_ERR_NOT_INITIALIZED [D]        Device has not been previously configured for use.
 *                - #ADI_SPI_SUCCESS                        Call completed successfully.
 *
 * Set the receive FIFO flush mode to flush the FIFO and/or keep it flushed.
 * The receive FIFO is essentially disabled while this mode is set.  Clear it
 * for normal FIFO operation.  A one-time flush is obtained by setting this
 * mode and then clearing it.
 *
 * @note This mode is NOT self-clearing.  Clear this mode to prevent continuous receive FIFO flushing.
 *
 * @sa        adi_SPI_GetFifoRxFlush().
 * @sa        adi_SPI_GetFifoTxFlush().
 * @sa        adi_SPI_SetFifoTxFlush().
 */
ADI_SPI_RESULT_TYPE adi_SPI_SetFifoRxFlush (ADI_SPI_DEV_HANDLE const hDevice, const bool_t bFlag)
{

#ifdef ADI_DEBUG
    if (ADI_SPI_INVALID_HANDLE(hDevice))
        return ADI_SPI_ERR_BAD_DEV_HANDLE;

    if ((hDevice->InitState != ADI_SPI_STATE_MASTER) && (hDevice->InitState != ADI_SPI_STATE_SLAVE))
        return ADI_SPI_ERR_NOT_INITIALIZED;
#endif

    if (true == bFlag) {
        hDevice->pSpi->SPICON |= (SPICON_RFLUSH);
    } else {
        hDevice->pSpi->SPICON &= ~(SPICON_RFLUSH);
    }

    return ADI_SPI_SUCCESS;
}


/*!
 * @brief  Get the SPI receive FIFO flush mode.
 *
 * @param[in]    hDevice      Device handle obtained from adi_SPI_MasterInit() or adi_SPI_SlaveInit().
 *
 * @return Returns the current state of the recieve FIFO flush control bit.
 *                - true     Receive FIFO flush mode is set.
 *                - false    Receive FIFO flush mode is cleared.
 *
 * @sa        adi_SPI_GetFifoTxFlush().
 * @sa        adi_SPI_SetFifoRxFlush().
 * @sa        adi_SPI_SetFifoTxFlush().
 */
bool_t adi_SPI_GetFifoRxFlush (ADI_SPI_DEV_HANDLE const hDevice) {

    return (hDevice->pSpi->SPICON & SPICON_RFLUSH) ? true : false;
}


/*!
 * @brief  Set the continuous transfer mode.
 *
 * @param[in]    hDevice      Device handle obtained from adi_SPI_MasterInit() or adi_SPI_SlaveInit().
 * @param[in]    bFlag        Flag to manage SPI continuous transfer mode.
 *                - true     Set continuous transfer mode.
 *                - false    Clear continuous transfer mode.
 *
 * @return         Status
 *                - #ADI_SPI_ERR_BAD_DEV_HANDLE [D]         Invalid device handle parameter.
 *                - #ADI_SPI_ERR_NOT_INITIALIZED [D]        Device has not been previously configured for use.
 *                - #ADI_SPI_SUCCESS                        Call completed successfully.
 *
 * Setting this mode causes the SPI controller to drive the Chip Select signal continuously until the transmit
 * FIFO is emptied.  Clearing it causes Chip Select to cycle between bytes.  If the transmit FIFO is allowed to
 * become empty, Chip Select will de-assert regardless of this mode.
 *
 * @note      If the transmit FIFO is allowed to empty during a send, the hardware-based (SPI controller-driven)
 *            Chip Select will de-assert which may disrupt extended transactions.  For this reason, the SPI device
 *            driver explicitly manages the Chip Select pin using GPIO protocol so that the chip remains selected
 *            throughout the entire transaction... regardless of whether the transmit FIFO may momentarily empty.
 *
 * @sa        adi_SPI_GetContinousMode().
 */
ADI_SPI_RESULT_TYPE adi_SPI_SetContinousMode (ADI_SPI_DEV_HANDLE const hDevice, const bool_t bFlag)
{

#ifdef ADI_DEBUG
    if (ADI_SPI_INVALID_HANDLE(hDevice))
        return ADI_SPI_ERR_BAD_DEV_HANDLE;

    if ((hDevice->InitState != ADI_SPI_STATE_MASTER) && (hDevice->InitState != ADI_SPI_STATE_SLAVE))
        return ADI_SPI_ERR_NOT_INITIALIZED;
#endif

    if (true == bFlag) {
        hDevice->pSpi->SPICON |= (SPICON_CON);
    } else {
        hDevice->pSpi->SPICON &= ~(SPICON_CON);
    }

    return ADI_SPI_SUCCESS;
}


/*!
 * @brief  Get the continuous transfer mode.
 *
 * @param[in]    hDevice      Device handle obtained from adi_SPI_MasterInit() or adi_SPI_SlaveInit().
 *
 * @return       Returns the current state of the continuous transfer mode.
 *                - true     Continuous transfer mode is set.
 *                - false    Continuous transfer mode is cleared.
 *
 * Get the current state of the continuous data transfer mode.
 *
 * @sa        adi_SPI_SetContinousMode().
 */
bool_t adi_SPI_GetContinousMode (ADI_SPI_DEV_HANDLE const hDevice)
{
    return (hDevice->pSpi->SPICON & SPICON_CON) ? true : false;
}


/*!
 * @brief  Set the internal loopback mode.
 *
 * @param[in]    hDevice      Device handle obtained from adi_SPI_MasterInit() or adi_SPI_SlaveInit().
 * @param[in]    bFlag        Flag to manage internal SPI loopback mode.
 *                - true     Set internal loopback mode.
 *                - false    Clear internal loopback mode.
 *
 * @return         Status
 *                - #ADI_SPI_ERR_BAD_DEV_HANDLE [D]         Invalid device handle parameter.
 *                - #ADI_SPI_ERR_NOT_INITIALIZED [D]        Device has not been previously configured for use.
 *                - #ADI_SPI_SUCCESS                        Call completed successfully.
 *
 * Set or clear the internal SPI loopback mode.  Primairly used for testing.
 *
 * @sa        adi_SPI_GetLoopback().
 */
ADI_SPI_RESULT_TYPE adi_SPI_SetLoopback (ADI_SPI_DEV_HANDLE const hDevice, const bool_t bFlag)
{

#ifdef ADI_DEBUG
    if (ADI_SPI_INVALID_HANDLE(hDevice))
        return ADI_SPI_ERR_BAD_DEV_HANDLE;

    if ((hDevice->InitState != ADI_SPI_STATE_MASTER) && (hDevice->InitState != ADI_SPI_STATE_SLAVE))
        return ADI_SPI_ERR_NOT_INITIALIZED;
#endif

    if (true == bFlag) {
        hDevice->pSpi->SPICON |= (SPICON_LOOPBACK);
    } else {
        hDevice->pSpi->SPICON &= ~(SPICON_LOOPBACK);
    }

    return ADI_SPI_SUCCESS;
}


/*!
 * @brief  Get the SPI loopback control bit.
 *
 * @param[in]   hDevice       Device handle obtained from adi_SPI_MasterInit() or adi_SPI_SlaveInit().
 *
 * @return      Returns the current state of the internal SPI loopback mode.
 *                - true     Internal loopback mode is set.
 *                - false    Internal loopback mode is cleared.
 *
 * Get the current state of the internal SPI loopback control bit.
 *
 * @sa        adi_SPI_SetLoopback().
 */
bool_t adi_SPI_GetLoopback (ADI_SPI_DEV_HANDLE const hDevice)
{
    return (hDevice->pSpi->SPICON & SPICON_LOOPBACK) ? true : false;
}


/*!
 * @brief  Set SPI Slave-Mode MISO output enable.
 *
 * @param[in]    hDevice      Device handle obtained from adi_SPI_MasterInit() or adi_SPI_SlaveInit().
 * @param[in]    bFlag        Flag to manage Slave-Mode MISO output ebable.
 *                - true     Enable "normal" opersation (driven output).
 *                - false    Disable the MISO output driver (open circuit).
 *
 * @return         Status
 *                - #ADI_SPI_ERR_BAD_DEV_HANDLE [D]         Invalid device handle parameter.
 *                - #ADI_SPI_ERR_NOT_INITIALIZED [D]        Device has not been previously configured for use.
 *                - #ADI_SPI_SUCCESS                        Call completed successfully.
 *
 * Set or clear this mode to manage the MISO pin output driver enable.
 *
 * @sa        adi_SPI_GetMisoOutput().
 */
ADI_SPI_RESULT_TYPE adi_SPI_SetMisoOutput (ADI_SPI_DEV_HANDLE const hDevice, const bool_t bFlag)
{

#ifdef ADI_DEBUG
    if (ADI_SPI_INVALID_HANDLE(hDevice))
        return ADI_SPI_ERR_BAD_DEV_HANDLE;

    if ((hDevice->InitState != ADI_SPI_STATE_MASTER) && (hDevice->InitState != ADI_SPI_STATE_SLAVE))
        return ADI_SPI_ERR_NOT_INITIALIZED;
#endif

    ADI_ENTER_CRITICAL_REGION();

    if (true == bFlag) {
        hDevice->pSpi->SPICON |= (SPICON_OEN);
    } else {
        hDevice->pSpi->SPICON &= ~(SPICON_OEN);
    }

    ADI_EXIT_CRITICAL_REGION();

    return ADI_SPI_SUCCESS;
}


/*!
 * @brief  Get SPI Slave-Mode MISO output enable.
 *
 * @param[in]   hDevice       Device handle obtained from adi_SPI_MasterInit() or adi_SPI_SlaveInit().
 *
 * @return      Returns the current state of the Slave-Mode MISO output enable.
 *                - true     MISO output driver is enabled.
 *                - false    MISO output driver is disabled.
 *
 * Get the current state of the slave-mode MISO pin output driver enable.
 *
 * @sa        adi_SPI_SetMisoOutput().
 */
bool_t adi_SPI_GetMisoOutput (ADI_SPI_DEV_HANDLE const hDevice)
{
    return (hDevice->pSpi->SPICON & SPICON_OEN) ? true : false;
}


/*!
 * @brief  Set the SPI receive FIFO overflow mode.
 *
 * @param[in]    hDevice      Device handle obtained from adi_SPI_MasterInit() or adi_SPI_SlaveInit().
 * @param[in]    bFlag        Flag to manage recieve FIFO overflow behavior.
 *                - true     Discard old data on receive FIFO overflow.
 *                - false    Discard new data on receive FIFO overflow.
 *
 * @return         Status
 *                - #ADI_SPI_ERR_BAD_DEV_HANDLE [D]         Invalid device handle parameter.
 *                - #ADI_SPI_ERR_NOT_INITIALIZED [D]        Device has not been previously configured for use.
 *                - #ADI_SPI_SUCCESS                        Call completed successfully.
 *
 * Controls what to do with excess incoming data when the receive FIFO becomes full.
 * Either the new data or the old data is discarded.  Set the receive FIFO overflow mode
 * to replace data in the RX register (top of receive FIFO) with the incoming new data.
 * Clear it to discard incoming new data and preserve old undead data.

 *
 * @sa        adi_SPI_GetReceiveOverflow().
 */
ADI_SPI_RESULT_TYPE adi_SPI_SetReceiveOverflow (ADI_SPI_DEV_HANDLE const hDevice, const bool_t bFlag)
{

#ifdef ADI_DEBUG
    if (ADI_SPI_INVALID_HANDLE(hDevice))
        return ADI_SPI_ERR_BAD_DEV_HANDLE;

    if ((hDevice->InitState != ADI_SPI_STATE_MASTER) && (hDevice->InitState != ADI_SPI_STATE_SLAVE))
        return ADI_SPI_ERR_NOT_INITIALIZED;
#endif

    if (true == bFlag) {
        hDevice->pSpi->SPICON |= (SPICON_RXOF);
    } else {
        hDevice->pSpi->SPICON &= ~(SPICON_RXOF);
    }

    return ADI_SPI_SUCCESS;
}


/*!
 * @brief  Get the SPI receive FIFO overflow mode.
 *
 * @param[in]   hDevice       Device handle obtained from adi_SPI_MasterInit() or adi_SPI_SlaveInit().
 *
 * @return      Returns the current state of the receive FIFO overflow mode.
 *                - true     If discarding old data on receive FIFO overfolw.
 *                - false    If discarding new data on receive FIFO overfolw.
 *
 * Get the current state of the SPI receive FIFO overflow mode.
 *
 * @sa        adi_SPI_SetReceiveOverflow().
 */
bool_t adi_SPI_GetReceiveOverflow (ADI_SPI_DEV_HANDLE const hDevice)
{
    return (hDevice->pSpi->SPICON & SPICON_RXOF) ? true : false;
}


/*!
 * @brief  Set the SPI transmit FIFO underflow mode.
 *
 * @param[in]    hDevice      Device handle obtained from adi_SPI_MasterInit() or adi_SPI_SlaveInit().
 * @param[in]    bFlag        Flag to manage transmit FIFO underflow behavior.
 *                - true     Send zeroes on transmit FIFO underflow.
 *                - false    Resend last data on transmit FIFO underflow.
 *
 * @return         Status
 *                - #ADI_SPI_ERR_BAD_DEV_HANDLE [D]         Invalid device handle parameter.
 *                - #ADI_SPI_ERR_NOT_INITIALIZED [D]        Device has not been previously configured for use.
 *                - #ADI_SPI_SUCCESS                        Call completed successfully.
 *
 * Controls what to transmit when lacking valid data because the transmit FIFO is empty.
 * Either zeros or the last valid data are transmitted.  Set transmit FIFO underflow mode to send zeros.
 * Clear it to resend the last transmitted data.
 *
 * @sa        adi_SPI_GetTransmitUnderflow().
 */
ADI_SPI_RESULT_TYPE adi_SPI_SetTransmitUnderflow (ADI_SPI_DEV_HANDLE const hDevice, const bool_t bFlag)
{

#ifdef ADI_DEBUG
    if (ADI_SPI_INVALID_HANDLE(hDevice))
        return ADI_SPI_ERR_BAD_DEV_HANDLE;

    if ((hDevice->InitState != ADI_SPI_STATE_MASTER) && (hDevice->InitState != ADI_SPI_STATE_SLAVE))
        return ADI_SPI_ERR_NOT_INITIALIZED;
#endif

    if (true == bFlag) {
        hDevice->pSpi->SPICON |= (SPICON_ZEN);
    } else {
        hDevice->pSpi->SPICON &= ~(SPICON_ZEN);
    }

    return ADI_SPI_SUCCESS;
}


/*!
 * @brief  Get the SPI transmit FIFO underflow mode.
 *
 * @param[in]   hDevice       Device handle obtained from adi_SPI_MasterInit() or adi_SPI_SlaveInit().
 *
 * @return      Returns the current state of the Slave-Mode MISO output enable.
 *                - true     If sending zeroes on transmit FIFO underflow.
 *                - false    If resending last data on transmit FIFO underflow.
 *
 * Get the current state of the SPI transmit FIFO underflow mode, describing what
 * data is transmitted if the transmit FIFO underflows.
 *
 * @sa        adi_SPI_SetTransmitUnderflow().
 */
bool_t adi_SPI_GetTransmitUnderflow (ADI_SPI_DEV_HANDLE const hDevice)
{
    return (hDevice->pSpi->SPICON & SPICON_ZEN) ? true : false;
}


/*!
 * @brief  Set SPI transfer initiate and interrupt mode.
 *
 * @param[in]    hDevice      Device handle obtained from adi_SPI_MasterInit() or adi_SPI_SlaveInit().
 * @param[in]    bFlag        Flag to manage transfer initiate and interrupt mode.
 *                - true     Initiate transfers on write to transmit FIFO and interrupt when empty.
 *                - false    Initiate transfers on read from receive FIFO and interrupt when full.
 *
 * @return         Status
 *                - #ADI_SPI_ERR_BAD_DEV_HANDLE [D]         Invalid device handle parameter.
 *                - #ADI_SPI_ERR_NOT_INITIALIZED [D]        Device has not been previously configured for use.
 *                - #ADI_SPI_SUCCESS                        Call completed successfully.
 *
 * Set to initiate transfers on a write to the transmit data register
 * and interrupt when the transmit FIFO is empty.  Clear to initiate
 * transfers with a read from the receive data register and interrupt when the
 * receive FIFO is full.
 *
 * @sa        adi_SPI_GetTransferInitiateMode().
 */
ADI_SPI_RESULT_TYPE adi_SPI_SetTransferInitiateMode (ADI_SPI_DEV_HANDLE const hDevice, const bool_t bFlag)
{

#ifdef ADI_DEBUG
    if (ADI_SPI_INVALID_HANDLE(hDevice))
        return ADI_SPI_ERR_BAD_DEV_HANDLE;

    if ((hDevice->InitState != ADI_SPI_STATE_MASTER) && (hDevice->InitState != ADI_SPI_STATE_SLAVE))
        return ADI_SPI_ERR_NOT_INITIALIZED;
#endif

    if (true == bFlag) {
        hDevice->pSpi->SPICON |= (SPICON_TIM_TXWR);   /* initiate on tx write */
    } else {
        hDevice->pSpi->SPICON &= ~(SPICON_TIM_TXWR);  /* initiate on rx read */
    }

    return ADI_SPI_SUCCESS;
}


/*!
 * @brief  Get SPI transfer initiate and interrupt mode.
 *
 * @param[in]   hDevice       Device handle obtained from adi_SPI_MasterInit() or adi_SPI_SlaveInit().
 *
 * @return      Returns the current state of the SPI transfer initiate and interrupt mode.
 *                - true     If initiating transfers on write to transmit FIFO and interrupt when empty.
 *                - false    If initiating transfers on read from receive FIFO and interrupt when full.
 *
 * Get the current state of the SPI transfer initiate mode control bit, describing when
 * transfers are initiated and when interrupts are generated.
 *
 * @sa        adi_SPI_SetTransferInitiateMode().
 */
bool_t adi_SPI_GetTransferInitiateMode (ADI_SPI_DEV_HANDLE const hDevice)
{
    return (hDevice->pSpi->SPICON & SPICON_TIM_TXWR) ? true : false;
}


/*!
 * @brief  Set SPI LSB or MSB first transfer mode.
 *
 * @param[in]    hDevice      Device handle obtained from adi_SPI_MasterInit() or adi_SPI_SlaveInit().
 * @param[in]    bFlag        Flag to manage serialization direction of transfer.
 *                - true     For LSB first.
 *                - false    For MSB first.
 *
 * @return         Status
 *                - #ADI_SPI_ERR_BAD_DEV_HANDLE [D]         Invalid device handle parameter.
 *                - #ADI_SPI_ERR_NOT_INITIALIZED [D]        Device has not been previously configured for use.
 *                - #ADI_SPI_SUCCESS                        Call completed successfully.
 *
 * Configures whether data is transferred as LSB first or MSB first.
 *
 * @sa        adi_SPI_GetLsbFirst().
 */
ADI_SPI_RESULT_TYPE adi_SPI_SetLsbFirst (ADI_SPI_DEV_HANDLE const hDevice, const bool_t bFlag)
{

#ifdef ADI_DEBUG
    if (ADI_SPI_INVALID_HANDLE(hDevice))
        return ADI_SPI_ERR_BAD_DEV_HANDLE;

    if ((hDevice->InitState != ADI_SPI_STATE_MASTER) && (hDevice->InitState != ADI_SPI_STATE_SLAVE))
        return ADI_SPI_ERR_NOT_INITIALIZED;
#endif

    if (true == bFlag) {
        hDevice->pSpi->SPICON |= (SPICON_LSB);
    } else {
        hDevice->pSpi->SPICON &= ~(SPICON_LSB);
    }

    return ADI_SPI_SUCCESS;
}


/*!
 * @brief  Get SPI LSB or MSB first transfer mode.
 *
 * @param[in]   hDevice       Device handle obtained from adi_SPI_MasterInit() or adi_SPI_SlaveInit().
 *
 * @return      Returns the current state of the SPI MSB/LSB transfer mode.
 *                - true     If transferring MSB first.
 *                - false    If transferring LSB first.
 *
 * Returns whether the SPI controller is currently configured to transmit LSB or MSB first.
 *
 * @sa        adi_SPI_SetLsbFirst().
 */
bool_t adi_SPI_GetLsbFirst (ADI_SPI_DEV_HANDLE const hDevice)
{
    return (hDevice->pSpi->SPICON & SPICON_LSB) ? true : false;
}


/*!
 * @brief  Set the SPI "Wired-OR" output mode.
 *
 * @param[in]    hDevice      Device handle obtained from adi_SPI_MasterInit() or adi_SPI_SlaveInit().
 * @param[in]    bFlag        Flag to select output circuit configuration.
 *                - true     For "wired-or" (open-collector) output mode.
 *                - false    For "normal" (fully-driven) output levels.
 *
 * @return         Status
 *                - #ADI_SPI_ERR_BAD_DEV_HANDLE [D]         Invalid device handle parameter.
 *                - #ADI_SPI_ERR_NOT_INITIALIZED [D]        Device has not been previously configured for use.
 *                - #ADI_SPI_SUCCESS                        Call completed successfully.
 *
 * Controls the output driver configuration.  Set to enable an "Open-Collector" output circuit
 * for performing external "Wired-OR" logic, requiring an external pull-up resistor.
 * Clear for normal, fully-driven outputs.
 *
 * @sa        adi_SPI_GetWiredOrMode().
 */
ADI_SPI_RESULT_TYPE adi_SPI_SetWiredOrMode (ADI_SPI_DEV_HANDLE const hDevice, const bool_t bFlag)
{

#ifdef ADI_DEBUG
    if (ADI_SPI_INVALID_HANDLE(hDevice))
        return ADI_SPI_ERR_BAD_DEV_HANDLE;

    if ((hDevice->InitState != ADI_SPI_STATE_MASTER) && (hDevice->InitState != ADI_SPI_STATE_SLAVE))
        return ADI_SPI_ERR_NOT_INITIALIZED;
#endif

    if (true == bFlag) {
        hDevice->pSpi->SPICON |= (SPICON_WOM);
    } else {
        hDevice->pSpi->SPICON &= ~(SPICON_WOM);
    }

    return ADI_SPI_SUCCESS;
}


/*!
 * @brief  Get the SPI "Wired-OR" output mode.
 *
 * @param[in]   hDevice       Device handle obtained from adi_SPI_MasterInit() or adi_SPI_SlaveInit().
 *
 * @return      Returns the current state of the SPI "wired-or" output mode.
 *                - true     If "Wired-OR" (open-collector) output mode is enabled.
 *                - false    If "Wired-OR" (fully-driven) output mode is disabled.
 *
 * Get the current state of the SPI "Wired-OR" output mode.
 *
 * @sa        adi_SPI_SetWiredOrMode().
 */
bool_t adi_SPI_GetWiredOrMode (ADI_SPI_DEV_HANDLE const hDevice)
{
    return (hDevice->pSpi->SPICON & SPICON_WOM) ? true : false;
}


/*!
 * @brief  Set the clock polarity.
 *
 * @param[in]    hDevice      Device handle obtained from adi_SPI_MasterInit() or adi_SPI_SlaveInit().
 * @param[in]    bFlag        Flag to manage the idle state of the serial data clock between samples.
 *                - true     Clock is idled high.
 *                - false    Clock is idled low.
 *
 * @return         Status
 *                - #ADI_SPI_ERR_BAD_DEV_HANDLE [D]         Invalid device handle parameter.
 *                - #ADI_SPI_ERR_NOT_INITIALIZED [D]        Device has not been previously configured for use.
 *                - #ADI_SPI_SUCCESS                        Call completed successfully.
 *
 * Sets the SPI clock polarity control bit (CPOL).  Used in conjunction with clock phase (CPHA) to program
 * the exact timing of serial data capture and transmit.  Both clock phase and polarity must be considered in
 * selecting the data transfer mode best suited to the Master/Slave device pair, typically dependant on the
 * manufacturer and timing requirements of the external SPI device.
 *
 * Both Master and Slave devices must use the same settings for clock phase and polarity.
 *
 * If the phase of the clock is zero (CPHA=0), receive data are latched on the rising-clock-edge with
 * CPOL=0 (idle-low) polarity, and on the falling-clock-edge with CPOL=1 (idle high) ploarity.
 *
 * If CPHA=1, the effective clock edges are reversed; CPOL=0 latches receive data on the falling-clock-edge
 * and CPOL=1 latches receive data on the rising-clock-edge.
 *
 * Data are transmitted on the opposite clock edge as the receive, i.e., receive and transmit are out of phase.
 *
 * @sa        adi_SPI_GetClockPhase().
 * @sa        adi_SPI_GetClockPolarity().
 * @sa        adi_SPI_SetClockPhase().
 */
ADI_SPI_RESULT_TYPE adi_SPI_SetClockPolarity (ADI_SPI_DEV_HANDLE const hDevice, const bool_t bFlag)
{

#ifdef ADI_DEBUG
    if (ADI_SPI_INVALID_HANDLE(hDevice))
        return ADI_SPI_ERR_BAD_DEV_HANDLE;

    if ((hDevice->InitState != ADI_SPI_STATE_MASTER) && (hDevice->InitState != ADI_SPI_STATE_SLAVE))
        return ADI_SPI_ERR_NOT_INITIALIZED;
#endif

    if (true == bFlag) {
        hDevice->pSpi->SPICON |= (SPICON_CPOL);
    } else {
        hDevice->pSpi->SPICON &= ~(SPICON_CPOL);
    }

    return ADI_SPI_SUCCESS;
}


/*!
 * @brief  Get the clock polarity.
 *
 * @param[in]   hDevice       Device handle obtained from adi_SPI_MasterInit() or adi_SPI_SlaveInit().
 *
 * @return      Returns the current state of the SPI clock polarity control.
 *                - true     Clock is idled high.
 *                - false    Clock is idled low.
 *
 * Get the current state of the SPI clock polarity.
 *
 * @sa        adi_SPI_GetClockPhase().
 * @sa        adi_SPI_SetClockPhase().
 * @sa        adi_SPI_SetClockPolarity().
 */
bool_t adi_SPI_GetClockPolarity (ADI_SPI_DEV_HANDLE const hDevice)
{
    return (hDevice->pSpi->SPICON & SPICON_CPOL) ? true : false;
}


/*!
 * @brief  Set the clock phase.
 *
 * @param[in]    hDevice      Device handle obtained from adi_SPI_MasterInit() or adi_SPI_SlaveInit().
 * @param[in]    bFlag        Flag to manage the phase of the serial data clock.
 *                - true     Sample data on trailing-edge of each serial bit.
 *                - false    Sample data on leading-edge of each serial bit.
 *
 * @return         Status
 *                - #ADI_SPI_ERR_BAD_DEV_HANDLE [D]         Invalid device handle parameter.
 *                - #ADI_SPI_ERR_NOT_INITIALIZED [D]        Device has not been previously configured for use.
 *                - #ADI_SPI_SUCCESS                        Call completed successfully.
 *
 * Sets the SPI clock polarity phase bit (CPHA).  Used in conjunction with clock polarity (CPOL) to program
 * the exact timing of serial data capture and transmit.  Both clock phase and polarity must be considered in
 * selecting the data transfer mode best suited to the Master/Slave device pair, typically dependant on the
 * manufacturer and timing requirements of the external SPI device.
 *
 * Both Master and Slave devices must use the same settings for clock phase and polarity.
 *
 * If the phase of the clock is zero (CPHA=0), receive data are latched on the rising-clock-edge with
 * CPOL=0 (idle-low) polarity, and on the falling-clock-edge with CPOL=1 (idle high) ploarity.
 *
 * If CPHA=1, the effective clock edges are reversed; CPOL=0 latches receive data on the falling-clock-edge
 * and CPOL=1 latches receive data on the rising-clock-edge.
 *
 * Data are transmitted on the opposite clock edge as the receive, i.e., receive and transmit are out of phase.
 *
 * @sa        adi_SPI_GetClockPhase().
 * @sa        adi_SPI_GetClockPolarity().
 * @sa        adi_SPI_SetClockPolarity().
 */
ADI_SPI_RESULT_TYPE adi_SPI_SetClockPhase (ADI_SPI_DEV_HANDLE const hDevice, const bool_t bFlag)
{

#ifdef ADI_DEBUG
    if (ADI_SPI_INVALID_HANDLE(hDevice))
        return ADI_SPI_ERR_BAD_DEV_HANDLE;

    if ((hDevice->InitState != ADI_SPI_STATE_MASTER) && (hDevice->InitState != ADI_SPI_STATE_SLAVE))
        return ADI_SPI_ERR_NOT_INITIALIZED;
#endif

    if (true == bFlag) {
        hDevice->pSpi->SPICON |= (SPICON_CPHA_SAMPLETRAILING);
    } else {
        hDevice->pSpi->SPICON &= ~(SPICON_CPHA_SAMPLETRAILING);
    }

    return ADI_SPI_SUCCESS;
}


/*!
 * @brief  Get the clock phase.
 *
 * @param[in]   hDevice       Device handle obtained from adi_SPI_MasterInit() or adi_SPI_SlaveInit().
 *
 * @return      Returns the current state of the SPI clock phase.
 *                - true     Sample data on trailing-edge of each serial bit.
 *                - false    Sample data on leading-edge of each serial bit.
 *
 * Get the current state of the SPI clock phase control bit.
 *
 * @sa        adi_SPI_GetClockPolarity().
 * @sa        adi_SPI_SetClockPhase().
 * @sa        adi_SPI_SetClockPolarity().
 */
bool_t adi_SPI_GetClockPhase (ADI_SPI_DEV_HANDLE const hDevice)
{
    return (hDevice->pSpi->SPICON & SPICON_CPHA_SAMPLETRAILING) ? true : false;
}


/*!
 * @brief  Set SPI Master-Mode operation.
 *
 * @param[in]    hDevice      Device handle obtained from adi_SPI_MasterInit() or adi_SPI_SlaveInit().
 * @param[in]    bFlag        Flag to select either Master-Mode or Slave-Mode operation.
 *                - true     Enable Master-Mode.
 *                - false    Enable Slave-Mode.
 *
 * @return         Status
 *                - #ADI_SPI_ERR_BAD_DEV_HANDLE [D]         Invalid device handle parameter.
 *                - #ADI_SPI_ERR_NOT_INITIALIZED [D]        Device has not been previously configured for use.
 *                - #ADI_SPI_SUCCESS                        Call completed successfully.
 *
 * Controls SPI Master/Slave mode of operation, set for Master-Mode, clear for Slave-Mode.
 *
 * @sa        adi_SPI_GetMasterMode().
 */
ADI_SPI_RESULT_TYPE adi_SPI_SetMasterMode (ADI_SPI_DEV_HANDLE const hDevice, const bool_t bFlag)
{

#ifdef ADI_DEBUG
    if (ADI_SPI_INVALID_HANDLE(hDevice))
        return ADI_SPI_ERR_BAD_DEV_HANDLE;

    if ((hDevice->InitState != ADI_SPI_STATE_MASTER) && (hDevice->InitState != ADI_SPI_STATE_SLAVE))
        return ADI_SPI_ERR_NOT_INITIALIZED;
#endif

    if (true == bFlag) {
        hDevice->pSpi->SPICON |= (SPICON_MASEN);
    } else {
        hDevice->pSpi->SPICON &= ~(SPICON_MASEN);
    }

    return ADI_SPI_SUCCESS;
}


/*!
 * @brief  Get SPI Master-Mode operation.
 *
 * @param[in]   hDevice       Device handle obtained from adi_SPI_MasterInit() or adi_SPI_SlaveInit().
 *
 * @return      Returns the current state of SPI Master-Mode setting.
 *                - true     Master-Mode is currently selected.
 *                - false    Slave-Mode is currently selected.
 *
 * Get the current state of the SPI Master Mode control bit.
 *
 * @sa        adi_SPI_SetMasterMode().
 */
bool_t adi_SPI_GetMasterMode (ADI_SPI_DEV_HANDLE const hDevice)
{
    return (hDevice->pSpi->SPICON & SPICON_MASEN) ? true : false;
}


/*!
 * @brief  Set the SPI serial clock frequency.
 *
 * @param[in]    hDevice      Device handle obtained from adi_SPI_MasterInit() or adi_SPI_SlaveInit().
 * @param[in]    Hertz        Target frequency (in Hz) for SPI bitrate.
 *
 * @return         Status
 *                - #ADI_SPI_ERR_BAD_DEV_HANDLE [D]         Invalid device handle parameter.
 *                - #ADI_SPI_ERR_NOT_INITIALIZED [D]        Device has not been previously configured for use.
 *                - #ADI_SPI_ERR_PARAM_OUT_OF_RANGE         Specified frequence out of range.
 *                - #ADI_SPI_SUCCESS                        Call completed successfully.
 *
 * Compute and set the internal SPI clock rate divider register to produce
 * the desired serial clock frequency.  Resulting frequency is subject to arithmetic rounding errors.
 * Use #adi_SPI_GetBitrate() to obtain the exact frequency produced, including rounding errors.
 *
 * @sa        adi_SPI_GetBitrate().
 */
ADI_SPI_RESULT_TYPE adi_SPI_SetBitrate (ADI_SPI_DEV_HANDLE const hDevice, const uint32_t Hertz)
{
    uint32_t incoming_clock;
    uint16_t div;

#ifdef ADI_DEBUG
    if (ADI_SPI_INVALID_HANDLE(hDevice))
        return ADI_SPI_ERR_BAD_DEV_HANDLE;

    if (ADI_SPI_STATE_UNINITIALIZED == hDevice->InitState)
        return ADI_SPI_ERR_NOT_INITIALIZED;
#endif

    // 'M350 HAVE dedicated system clocks for SPI
    if (pSPI0_DevData == hDevice)    incoming_clock = SystemGetClockFrequency(ADI_SYS_CLOCK_SPI0);
    if (pSPI1_DevData == hDevice)    incoming_clock = SystemGetClockFrequency(ADI_SYS_CLOCK_SPI1);
    if (pSPIH_DevData == hDevice)    incoming_clock = SystemGetClockFrequency(ADI_SYS_CLOCK_SPIH);

    /* requested rate needs to be 2x less than incomming clock */
    if (2*Hertz >= incoming_clock)
        return ADI_SPI_ERR_PARAM_OUT_OF_RANGE;

     /* compute the SPI divider value */
     div = ((incoming_clock / Hertz) >> 1) - 1;  /* '>>1' is really a divide by 2 */

    /* range check that computed divider fits */
    if (div != (div & SPIDIV_DIV_MSK))
        return ADI_SPI_ERR_PARAM_OUT_OF_RANGE;

    /* store it in core */
    hDevice->pSpi->SPIDIV = div;

    return ADI_SPI_SUCCESS;
}


/*!
 * @brief  Get the SPI serial clock frequency.
 *
 * @param[in]    hDevice     Device handle obtained from adi_SPI_MasterInit() or adi_SPI_SlaveInit().
 *
 * @return       Returns the exact serial clock frequence (in Hertz).
 *
 * Get the current serial clock frequency.  The returned value is exact but
 * may not exactly match the value set with #adi_SPI_SetBitrate() due to
 * computational round-off errors resulting from fixed register size and
 * finite-precision arithmetic.
 *
 * @sa        adi_SPI_SetBitrate().
 */
uint32_t adi_SPI_GetBitrate (ADI_SPI_DEV_HANDLE const hDevice)
{
    uint32_t incoming_clock;
    uint16_t div;

    div = hDevice->pSpi->SPIDIV;  /* assumes this is always a right-justified value */

    // 'M350 HAVE dedicated system clocks for SPI
    if (pSPI0_DevData == hDevice)    incoming_clock = SystemGetClockFrequency(ADI_SYS_CLOCK_SPI0);
    if (pSPI1_DevData == hDevice)    incoming_clock = SystemGetClockFrequency(ADI_SYS_CLOCK_SPI1);
    if (pSPIH_DevData == hDevice)    incoming_clock = SystemGetClockFrequency(ADI_SYS_CLOCK_SPIH);

    return (incoming_clock / (div + 1)) >> 1;  /* '>>1' is divide by 2 */
}


/*!
 * @brief  Designate the chip select port and pin.
 *
 * @param[in]    hDevice     Device handle obtained from adi_SPI_MasterInit() or adi_SPI_SlaveInit().
 * @param[in]    Port        The GPIO port used for chip select (e.g. ADI_GPIO_PORT_3).
 * @param[in]    Pin         The GPIO pin used for chip select (e.g. ADI_GPIO_PIN_3).
 *
 * @return         Status
 *                - #ADI_SPI_ERR_BAD_DEV_HANDLE [D]         Invalid device handle parameter.
 *                - #ADI_SPI_ERR_NOT_INITIALIZED [D]        Device has not been previously configured for use.
 *                - #ADI_SPI_ERR_BAD_GPIO_CONFIG            GPIO problem reconfiguring CS.
 *                - #ADI_SPI_SUCCESS                        Call completed successfully.
 *
 * Designates the desired chip select to use for activating an external slave device.  Does not assert the
 * designated chip select, which is done by the driver automatically with data transactions.
 *
 * @note Any available GPIO port and pin combination may be used as chip select, not just the default pins
 * associated with the SPI controller.  This is bacause the SPI driver does not use the hardware chip select
 * on the controller; it uses a software (GPIO-based) CS methodology.
 *
 *
 * @sa        adi_SPI_GetChipSelect().
 */
ADI_SPI_RESULT_TYPE adi_SPI_SetChipSelect (ADI_SPI_DEV_HANDLE const hDevice, const ADI_GPIO_PORT_TYPE Port, const ADI_GPIO_DATA_TYPE Pin)
{

#ifdef ADI_DEBUG
    if (ADI_SPI_INVALID_HANDLE(hDevice))
        return ADI_SPI_ERR_BAD_DEV_HANDLE;

    if ((hDevice->InitState != ADI_SPI_STATE_MASTER) && (hDevice->InitState != ADI_SPI_STATE_SLAVE))
        return ADI_SPI_ERR_NOT_INITIALIZED;
#endif

	/* tear down current CS config */
	if (ConfigureMasterCS(hDevice, false))
		return ADI_SPI_ERR_BAD_GPIO_CONFIG;

	/* store the new CS port/pin info */
    hDevice->ChipSelect.Port = Port;
    hDevice->ChipSelect.Pin = Pin;

	/* configure new CS port/pin set */
	if (ConfigureMasterCS(hDevice, true))
		return ADI_SPI_ERR_BAD_GPIO_CONFIG;

    return ADI_SPI_SUCCESS;
}


/*!
 * @brief  Get the currently designated chip select.
 *
 * @param[in]    hDevice     Device handle obtained from adi_SPI_MasterInit() or adi_SPI_SlaveInit().
 *
 * @return       Returns a struct value representing the chip select line currently configured.
 *
 * Get the current state of the SPI chip select port and pin designation.
 *
 * @sa        adi_SPI_SetChipSelect().
 */
ADI_SPI_CHIP_SELECT_TYPE adi_SPI_GetChipSelect (ADI_SPI_DEV_HANDLE const hDevice)
{
    return hDevice->ChipSelect;
}


/*!
 * @brief  Set the blocking mode.
 *
 * @param[in]    hDevice      Device handle obtained from adi_SPI_MasterInit() or adi_SPI_SlaveInit().
 * @param[in]    bFlag        Flag to select between blocking-mode and non-blocking-mode transfers.
 *                - true     Use blocking-mode.
 *                - false    Use non-blocking-mode.
 *
 * @return         Status
 *                - #ADI_SPI_ERR_BAD_DEV_HANDLE [D]         Invalid device handle parameter.
 *                - #ADI_SPI_ERR_NOT_INITIALIZED [D]        Device has not been previously configured for use.
 *                - #ADI_SPI_SUCCESS                        Call completed successfully.
 *
 * Sets the blocking mode for data transfers.  Transfers made in blocking-mode will not return until complete.
 * Transfers made in non-blocking-mode may return before the transfer is complete and will require application
 * level polling calls to determin when and how much of non-blocking transactions are complete.  See
 * #adi_SPI_MasterTransfer() API for making non-blocking polling calls.
 *
 *
 * @sa        adi_SPI_MasterTransfer().
 * @sa        adi_SPI_GetBlockingMode().
 */
ADI_SPI_RESULT_TYPE adi_SPI_SetBlockingMode (ADI_SPI_DEV_HANDLE const hDevice, const bool_t bFlag)
{

#ifdef ADI_DEBUG
    if (ADI_SPI_INVALID_HANDLE(hDevice))
        return ADI_SPI_ERR_BAD_DEV_HANDLE;

    if ((hDevice->InitState != ADI_SPI_STATE_MASTER) && (hDevice->InitState != ADI_SPI_STATE_SLAVE))
        return ADI_SPI_ERR_NOT_INITIALIZED;
#endif

    hDevice->bBlockingMode = bFlag;

    return ADI_SPI_SUCCESS;
}


/*!
 * @brief  Get the blocking mode.
 *
 * @param[in]   hDevice       Device handle obtained from adi_SPI_MasterInit() or adi_SPI_SlaveInit().
 *
 * @return      Returns the current blocking mode.
 *                - true     Using blocking-mode.
 *                - false    Using non-blocking-mode.
 *
 * Get the current state of the SPI blocking mode.
 *
 * @sa        adi_SPI_SetBlockingMode().
 */
bool_t adi_SPI_GetBlockingMode (ADI_SPI_DEV_HANDLE const hDevice)
{
    return hDevice->bBlockingMode;
}


/*!
 * @brief  Set the DMA mode.
 *
 * @param[in]    hDevice      Device handle obtained from adi_SPI_MasterInit() or adi_SPI_SlaveInit().
 * @param[in]    bFlag        Flag to manage whether to use DMA to effect SPI transfers (SPI1 only).
 *                - true     Enable DMA mode.
 *                - false    Disable DMA mode.
 *
 * @return         Status
 *                - #ADI_SPI_ERR_BAD_DEV_HANDLE [D]         Invalid device handle parameter.
 *                - #ADI_SPI_ERR_NOT_INITIALIZED [D]        Device has not been previously configured for use.
 *                - #ADI_SPI_SUCCESS                        Call completed successfully.
 *
 * Manages whether data transfers on SPI device will employ the DMA controller manage the transfer as a background process.
 * When coupled with non-blocking-mode, DMA transfers require polling calls to ascertain completion of the transfer.
 *
 * @note    Invalid for use on SPI device 0 (SPI0), which is dedicated to UHF communications.
 *
 * @sa        adi_SPI_GetBlockingMode().
 * @sa        adi_SPI_GetDmaMode().
 * @sa        adi_SPI_GetDmaRxComplete().
 * @sa        adi_SPI_GetDmaTxComplete().
 * @sa        adi_SPI_SetBlockingMode().
 */
ADI_SPI_RESULT_TYPE adi_SPI_SetDmaMode (ADI_SPI_DEV_HANDLE const hDevice, const bool_t bFlag)
{

#ifdef ADI_DEBUG
    if (ADI_SPI_INVALID_HANDLE(hDevice))
        return ADI_SPI_ERR_BAD_DEV_HANDLE;

    if ((hDevice->InitState != ADI_SPI_STATE_MASTER) && (hDevice->InitState != ADI_SPI_STATE_SLAVE))
        return ADI_SPI_ERR_NOT_INITIALIZED;
#endif

    /* call whichever uninitializer is currently active */
    if (ADI_SPI_SUCCESS != hDevice->FunctionTable.pUnInitFcn(hDevice))
        return ADI_SPI_ERR_BAD_DMA_CONFIG;

    hDevice->bDmaMode = bFlag;

    /* map DMA/interrupt function table */
#if (ADI_SPI_CFG_ENABLE_DMA_SUPPORT == 1)

    if(bFlag == true)
    {
      hDevice->FunctionTable.pInitFcn            = &dmaInit;
      hDevice->FunctionTable.pUnInitFcn          = &dmaUnInit;
      hDevice->FunctionTable.pInitDescriptorsFcn = &dmaInitializeDescriptors;
      hDevice->FunctionTable.pDmaHandlerFcn      = &dmaCommonHandler;
    }

#endif /* ADI_I2S_CFG_ENABLE_DMA_SUPPORT */

    if(bFlag == false)
    {
      hDevice->FunctionTable.pInitFcn            = &intInit;
      hDevice->FunctionTable.pUnInitFcn          = &intUnInit;
      hDevice->FunctionTable.pInitDescriptorsFcn = &intInitializeDescriptors;
      hDevice->FunctionTable.pDmaHandlerFcn      = &intCommonHandler;
    }

    /* call the newly activated initializer */
    if (ADI_SPI_SUCCESS != hDevice->FunctionTable.pInitFcn(hDevice))
        return ADI_SPI_ERR_BAD_DMA_CONFIG;

    return ADI_SPI_SUCCESS;
}


/*!
 * @brief  Get the DMA mode.
 *
 * @param[in]   hDevice       Device handle obtained from adi_SPI_MasterInit() or adi_SPI_SlaveInit().
 *
 * @return      Returns the current state of the DMA usage mode.
 *                - true     DMA mode is enabled.
 *                - false    DMA mode is disabled.
 *
 * Get the current state of the SPI DMA mode.
 *
 * @sa        adi_SPI_GetBlockingMode().
 * @sa        adi_SPI_GetDmaRxComplete().
 * @sa        adi_SPI_GetDmaTxComplete().
 * @sa        adi_SPI_SetBlockingMode().
 * @sa        adi_SPI_SetDmaMode().
 */
bool_t adi_SPI_GetDmaMode (ADI_SPI_DEV_HANDLE const hDevice)
{
    return hDevice->bDmaMode;
}

/*!
 * @brief  Get transmit DMA completion status.
 *
 * @param[in]   hDevice       Device handle obtained from adi_SPI_MasterInit() or adi_SPI_SlaveInit().
 *
 * @return      Returns the current completion state of the DMA transmit transaction.
 *                - true     DMA transmit sequence is complete.
 *                - false    DMA transmit sequence is incomplete.
 *
 * Get the current state of the SPI DMA transmit completion.  Used at the application
 * level to poll for completion of a previously submitted transmit request as a \b non-blocking DMA transfer.
 * The application may call #adi_SPI_GetDmaTxComplete(), #adi_SPI_GetDmaRxComplete(),
 * or both, as necessary.
 *
 * @sa        adi_SPI_GetBlockingMode().
 * @sa        adi_SPI_GetDmaMode().
 * @sa        adi_SPI_GetDmaRxComplete().
 * @sa        adi_SPI_SetBlockingMode().
 * @sa        adi_SPI_SetDmaMode().
 */
bool_t adi_SPI_GetDmaTxComplete (ADI_SPI_DEV_HANDLE const hDevice)
{
  return hDevice->bComplete;
}

/*!
 * @brief  Get receive DMA completion status.
 *
 * @param[in]   hDevice       Device handle obtained from adi_SPI_MasterInit() or adi_SPI_SlaveInit().
 *
 * @return      Returns the current completion state of a DMA receive transaction.
 *                - true     DMA receive sequence is complete.
 *                - false    DMA receive sequence is incomplete.
 *
 * Get the current state of the SPI DMA receive completion.  Used at the application
 * level to poll for completion of a previously submitted receive request as a \b non-blocking DMA transfer.
 * The application may call #adi_SPI_GetDmaTxComplete(), #adi_SPI_GetDmaRxComplete(),
 * or both, as necessary.
 *
 * @sa        adi_SPI_GetBlockingMode().
 * @sa        adi_SPI_GetDmaMode().
 * @sa        adi_SPI_GetDmaTxComplete().
 * @sa        adi_SPI_SetBlockingMode().
 * @sa        adi_SPI_SetDmaMode().
 */
bool_t adi_SPI_GetDmaRxComplete (ADI_SPI_DEV_HANDLE const hDevice)
{
  return hDevice->bComplete;
}

/*!
 * @brief  Submit data buffers for SPI Master-Mode transaction.
 *
 * @param[in]     hDevice     Device handle obtained from adi_SPI_MasterInit() or adi_SPI_SlaveInit().
 * @param[in]     pXfr        Pointer to transfer data struct.
  *
 * @return         Status
 *                - #ADI_SPI_ERR_BAD_DEV_HANDLE [D]        Invalid device handle parameter.
 *                - #ADI_SPI_ERR_DMA_ALREADY_IN_USE [D]    DMA transaction already underway.
 *                - #ADI_SPI_ERR_NOT_INITIALIZED [D]       Device has not been previously configured for use.
 *                - #ADI_SPI_ERR_NULL_DATA_POINTER [D]     Invalid data pointer detected (NULL).
 *                - #ADI_SPI_ERR_PARAM_OUT_OF_RANGE [D]    Invalid size parameter detected (0).
 *                - #ADI_SPI_SUCCESS                       Call completed successfully.
 *
 * Request a synchronous transmit and receive of multiple data bytes
 * over the SPI serial channel.  Honors current blocking and DMA modes.
 * Buffer allocations are made by the calling code (the application).
 *
 * There are three PROGRAMMING MODELS for using this API. The simplest is
 * blocking mode, in which the API blocks internally until the transaction is complete.
 * In blocking mode, there is no opportunity for the application to do background
 * processing, so there will be wasted horsepower especially in the DMA case (where the
 * core has minimal work to do).  The other two cases are non-blocking-with-polling and
 * non-blocking-with-callback.  In both cases of the non-blocking mode, the application
 * has the opportunity to perform background processing, but it may never submit additional
 * SPI transactions until the current one is complete.  In non-blocking-with-polling mode,
 * the application needs to poll the SPI driver for completion.  In non-blocking-with-callback
 * mode, the application need not concern itself with the progress of the current SPI
 * transaction and will know the transaction is complete when it receives the callback.
 *
 * The transmit buffer is sent and the receive buffer is written according
 * to the size and increment information contained by the \a pXft transfer
 * data structure parameter.
 *
 * The #ADI_SPI_TRANSCEIVE_TYPE contains all the parameters describing the
 * transfer.  There are three main parts to it: a prologue, the transmit data
 * and the receive data.  Each part has a size attribute, and each data pointer
 * has an increment flag.  Setting the increment flag to \a false allows a
 * data pointer to remain stationary (see \a bTxIncrement and \a bRxIncrement, below).
 *
 * The prologue is a special block that can contain optional command or protocol
 * information unique to the external SPI device or command protocol.
 * The prologue can be zero-sized if not needed.
 *
 * Each call to adi_SPI_MasterTransfer() must populate the following fields of the
 * #ADI_SPI_TRANSCEIVE_TYPE block:
 *
 * @par pPrologue
 * The \a pPrologue pointer points to an application-defined prologue block that is
 * transmitted before the data.  The prologue block is a data chunk of command or
 * protocol-related information unique to the external device the SPI controller is
 * communicating with.  It can be used to tell the external device how to process
 * the coming data or to perform some special task.  The prologue block is an optional
 * element and may not be needed.  If not needed, the \a PrologueSize parameter should
 * be set to zero to prevent prologue transactions from taking up bandwidth or confusing
 * an external device that is not expecting a prologue.
 *
 * @par DataSize
 * The number of data bytes to transfer.  DMA mode requires evenly-sized byte counts and
 * buffer allocations due to the underlying SPI hardware that employs 16-bit DMA.
 * All data buffer allocations are made by the application and passed by pointer.
 * The size of the application-allocated transmit and receive data buffers are the same
 * unless one of the increment flags is disabled resulting in a stationary data pointer.
 * If using a stationary data pointer, e.g., a FIFO address, the associated data buffer
 * allocation may be of size one (unless using DMA mode, see above).  \b Blocking-mode calls
 * cause the driver to block on this API until the transaction is complete.  \b Non-blocking
 * calls return right away and require the application to poll on the transfer completion
 * status or register/receive a callback, such that the application may perform other tasks.
 * The application may never submit multiple transactions and must always insure each
 * transaction is complete before submitting any other transactions.
 *
 * @par pTxData
 * Pointer to the application-defined transmit data buffer.  This is the data sent out
 * over the SPI transmit wire (MOSI for Master-mode, MISO for Slave-mode) during the SPI transaction.
 * For ADuCM350 SPI DMA mode (which is 16-bit based), the transmit buffer must be 16-bit aligned.
 *
 * @par pRxData
 * Pointer to the application-defined receive data buffer.  This is where the receive data
 * will be stored from the SPI receive wire (MISO for Master-mode, MOSI for Slave-mode)
 * during the SPI transaction.
 * For ADuCM350 SPI DMA mode (which is 16-bit based), the receive buffer must be 16-bit aligned.
 *
 * @par PrologueSize
 * The size (in bytes) of the prologue block, \a pPrologue.  The prologue data is sent out ahead
 * of the data.  The prologue size may be set to zero to disable transmission of prologue data.
 *
 * @par bTxIncrement
 * Increment flag for the transmit buffer.  If true, the transmit data buffer pointer is
 * advanced as each byte is sent.  If false, the transmit data pointer is stationary.  A
 * stationary buffer pointer is useful for sending the same data to an external device or if
 * the source data is from a fixed memory address.
 *
 * @par bRxIncrement
 * Increment flag for the receive buffer.  If true, the receive data buffer pointer is
 * advanced as each byte is received.  If false, the receive data pointer is stationary.  A
 * stationary buffer pointer is useful for monitoring commands from an external device or if
 * the receive data is going to a fixed memory address.
 *
 * @sa adi_SPI_GetBlockingMode().
 * @sa adi_SPI_GetDmaMode().
 * @sa adi_SPI_GetDmaRxComplete().
 * @sa adi_SPI_GetDmaTxComplete().
 * @sa adi_SPI_SetBlockingMode().
 * @sa adi_SPI_SetDmaMode().
 * @sa adi_SPI_MasterComplete().
 */
ADI_SPI_RESULT_TYPE adi_SPI_MasterTransfer (ADI_SPI_DEV_HANDLE const hDevice, ADI_SPI_TRANSCEIVE_TYPE* const pXfr)
{
    ADI_SPI_RESULT_TYPE result = ADI_SPI_SUCCESS;
    uint8_t byte;

#ifdef ADI_DEBUG
    if (ADI_SPI_INVALID_HANDLE(hDevice))
        return ADI_SPI_ERR_BAD_DEV_HANDLE;

    if (ADI_SPI_STATE_MASTER != hDevice->InitState)
        return ADI_SPI_ERR_NOT_INITIALIZED;

    if ((NULL == pXfr->pTxData) || (NULL == pXfr->pRxData))
        return ADI_SPI_ERR_NULL_DATA_POINTER;

    /* yell about odd byte counts in debug mode for ADuCM350 DMA requests */
    if (hDevice->bDmaMode && pXfr->DataSize%2)
        return ADI_SPI_ERR_PARAM_OUT_OF_RANGE;

    if (hDevice->bDmaMode && (hDevice->pSpi->SPIDMA & SPIDMA_ENABLE))
        return ADI_SPI_ERR_DMA_ALREADY_IN_USE;

#endif /* ADI_DEBUG */

    /* disable interrupts during manual prologue phase */
    ADI_DISABLE_INT(hDevice->IRQn);

    /* clear status */
    hDevice->pSpi->SPISTA;

    /* assert chip select */
    /* for DMA mode, deassert in DMA handler */
    /* for non-DMA mode, deassert in reguler interrupt handler */
    if (ADI_SPI_SUCCESS != (result = AssertChipSelect(hDevice, true)))
        return result;

    /* if we have a prologue, transmit manually (without DMA) */
    if (pXfr->PrologueSize) {

        while (pXfr->PrologueSize) {

            /* wait for a tx fifo that is not full... */
            while (SPISTA_TXFSTA_FOURBYTES == (hDevice->pSpi->SPISTA & SPISTA_TXFSTA_MSK))
                ;

            /* send a byte */
            hDevice->pSpi->SPITX = *pXfr->pPrologue++;
            pXfr->PrologueSize--;

            /* pull/discard data from the rx fifo while transmitting prologue */
            byte = hDevice->pSpi->SPIRX;

            /* wait while rx fifo status catches up */
            while (SPISTA_RXFSTA_EMPTY == (SPISTA_RXFSTA_MSK & hDevice->pSpi->SPISTA))
            	;
        }

        /* flush the FIFOs */
        hDevice->pSpi->SPICON |= (SPICON_TFLUSH | SPICON_RFLUSH);
        hDevice->pSpi->SPICON &= ~(SPICON_TFLUSH | SPICON_RFLUSH);

        /* clear residual SPI interrupt in NVIC resulting from manual prologue transmit */
        NVIC_ClearPendingIRQ(hDevice->IRQn);
    }

    /* if no data to transmit after prologue... */
    if (0 == pXfr->DataSize) {
        /* deselect because we're done... */
        AssertChipSelect(hDevice, false);
    } else {

        /* begin data phase...  */

        /* DMA-Mode             */
#if (ADI_SPI_CFG_ENABLE_DMA_SUPPORT == 1)
        if (hDevice->bDmaMode == true)
        {
			/* The SPICNT register may be used to stop the SPI from clocking extra bytes
			   once the requested byte count had been pushed into the receive FIFO while
			   the DMA engine is still pulling the required count out of the FIFO; the
			   DMA count lags the SPI count by up to the size of the FIFO (4).  In the
			   case where the entire transaction size can fit into the SPICNT register
			   (8-bit count of 16-bit words = 512 bytes), we can use SPICNT.  For
			   transactions that exceed this limit, multiple DMA descriptors are used
			   and the SPICNT register cannot be used because the descriptor blocks
			   automatically ping-pong and loading the SPICNT register cannot occur
			   "on-time" as the DMA interrupts occur AFTER each new descriptor is
			   already "in flight".  In this case, the few extra receive bytes will
               land in the FIFO, but the DMA engine will not use them.
			*/
			if (pXfr->DataSize < ADI_SPI_MAX_TRANSFER_SIZE)
				hDevice->pSpi->SPICNT = pXfr->DataSize;
			else
				hDevice->pSpi->SPICNT = 0;  /* don't use */

            /* enable SPI for transmit initiate */
            hDevice->pSpi->SPICON |= SPICON_TIM_TXWR;

        	/* initialize DMA descriptors, data pointers and indexes, and channel enables through the function pointer table */
            if (ADI_SPI_SUCCESS != (result = hDevice->FunctionTable.pInitDescriptorsFcn(hDevice, pXfr->pTxData, pXfr->pRxData, pXfr->DataSize))) {
                AssertChipSelect(hDevice, false);
                return result;
            }

            /* block as required on both channels */
            if (hDevice->bBlockingMode == true) {

#if (ADI_CFG_ENABLE_RTOS_SUPPORT == 1)
                adi_osal_SemPend(hDevice->hSem, ADI_OSAL_TIMEOUT_FOREVER);
#else

                while (!adi_SPI_GetDmaTxComplete(hDevice) || !adi_SPI_GetDmaRxComplete(hDevice))
                {
                    /* Instead of busy-waiting, we enter core sleep mode while waiting. Any
                    * interrupt will also bring the processor out of core sleep and cause this call to return.
                    */
                   SystemEnterLowPowerMode(ADI_SYS_MODE_CORE_SLEEP, &hDevice->bInterruptFlag, 0);
                }
#endif /* ADI_CFG_ENABLE_RTOS_SUPPORT  */
            }
        }
        else

#endif /* ADI_SPI_CFG_ENABLE_DMA_SUPPORT  */
        {
#if (ADI_SPI_CFG_INTERRUPT_MODE_SUPPORT == 1)

            /* enable SPI for receive initiate */
            hDevice->pSpi->SPICON &= ~(SPICON_TIM_TXWR);

            /* interrupt mode, enable interrupts */
            ADI_ENABLE_INT(hDevice->IRQn);

            /* initialize the data phase */
            hDevice->pTxBuffer    = pXfr->pTxData;
            hDevice->pRxBuffer    = pXfr->pRxData;
            hDevice->TxRemaining  = pXfr->DataSize;
            hDevice->RxRemaining  = pXfr->DataSize;
            hDevice->TxIncrement  = (true == pXfr->bTxIncrement) ? 1 : 0;
            hDevice->RxIncrement  = (true == pXfr->bRxIncrement) ? 1 : 0;

            /* grab the lead byte */
            byte = *hDevice->pTxBuffer;

            /* modify tx pointer and buffer count prior to interrupt */
            hDevice->pTxBuffer += hDevice->TxIncrement;
            hDevice->TxRemaining--;

            /* this fifo write launches the interrupt-driven data sequence
             * (interrupts should already be enabled)
             * pre-load tx fifo with 1st byte
             */
            hDevice->pSpi->SPITX = byte;

            /* dummy rx-fifo read to flush rx fifo and launch SPI data transfer */
            byte = hDevice->pSpi->SPIRX;

            /* block as required... */
            if (hDevice->bBlockingMode) {

#if (ADI_CFG_ENABLE_RTOS_SUPPORT == 1)
                adi_osal_SemPend(hDevice->hSem, ADI_OSAL_TIMEOUT_FOREVER);
#else

                while (!adi_SPI_MasterComplete(hDevice))
                {
                    /* Instead of busy-waiting, we enter core sleep mode while waiting. Any
                    * interrupt will also bring the processor out of core sleep and cause this call to return.
                    */

                  SystemEnterLowPowerMode(ADI_SYS_MODE_CORE_SLEEP, &hDevice->bInterruptFlag, 0);
                }
#endif /* ADI_CFG_ENABLE_RTOS_SUPPORT  */
            }
#endif /* ADI_SPI_CFG_INTERRUPT_MODE_SUPPORT  */
        } /* end interrupt mode (PIO) */
    } /* end data phase */

    return result;
}

/*!
 * @brief  Get the SPI transaction completion status.
 *
 * @param[in]     hDevice      Device handle obtained from adi_SPI_MasterInit() or adi_SPI_SlaveInit().
 *
 * @return Returns the current state of a previously submitted non-blocking transaction.
 *                - true      Transaction is complete.
 *                - false     Transaction is incomplete.
 *
 * @sa        adi_SPI_MasterTransfer().
 */
bool_t adi_SPI_MasterComplete (ADI_SPI_DEV_HANDLE const hDevice)
{
    return (hDevice->TxRemaining || hDevice->RxRemaining) ? false : true;
}

/*!
 * @brief  Submit data buffers for SPI Slave-Mode transaction.
 *
 * @param[in]     hDevice     Device handle obtained from adi_SPI_MasterInit() or adi_SPI_SlaveInit().
 * @param[in]     pXfr        Pointer to transfer data struct.
  *
 * @return         Status
 *                - #ADI_SPI_ERR_BAD_DEV_HANDLE [D]        Invalid device handle parameter.
 *                - #ADI_SPI_ERR_DMA_ALREADY_IN_USE [D]    DMA transaction already underway.
 *                - #ADI_SPI_ERR_NOT_INITIALIZED [D]       Device has not been previously configured for use.
 *                - #ADI_SPI_ERR_NULL_DATA_POINTER [D]     Invalid data pointer detected (NULL).
 *                - #ADI_SPI_ERR_PARAM_OUT_OF_RANGE [D]    Invalid size parameter detected (0).
 *                - #ADI_SPI_SUCCESS                       Call completed successfully.
 *
 * Request a synchronous transmit and receive of multiple data bytes
 * over the SPI serial channel.  Honors current blocking and DMA modes.
 * Buffer allocations are made by the calling code (the application).
 *
 * There are three PROGRAMMING MODELS for using this API. The simpelest is
 * blocking mode, in which the API blocks internally until the transaction is complete.
 * In blocking mode, there is no oppertunity for the application to do background
 * processing, so there will be wasted horseower espicially in the DMA case (where the
 * core has minimal work to do).  The other two cases are non-blocking-with-polling and
 * non-blocking-with-callback.  In both cases of the non-blocking mode, the application
 * has the oppertunity to perform background processing, but it may never submit additional
 * SPI transactions until the current one is complete.  In non-blocking-with-polling mode,
 * the application needs to poll the SPI driver for completion.  In non-blocking-with-callback
 * mode, the application need not concern itself with the progress of the current SPI
 * transaction and will know the transaction is complete when it receives the callback.
 *
 * The transmit buffer is sent and the receive buffer is written according
 * to the size and increment information contained by the \a pXft transfer
 * data structure parameter.
 *
 * The #ADI_SPI_TRANSCEIVE_TYPE contains all the parameters describing the
 * transfer.  There are three main parts to it: a prologue, the transmit data
 * and the receive data.  Each part has a size attribute, and each data pointer
 * has an increment flag.  Setting the increment flag to \a false allows a
 * data pointer to remain stationary (see \a bTxIncrement and \a bRxIncrement, below).
 *
 * @note The prologue buffer cannot be used in Slave mode.
 *
 * Each call to adi_SPI_SlaveTransfer() must populate the following fields of the
 * #ADI_SPI_TRANSCEIVE_TYPE block:
 *
 * @par DataSize
 * The number of data bytes to transfer.  DMA mode requires evenly-sized byte counts and
 * buffer allocations due to the underlying SPI hardware that employs 16-bit DMA.
 * All data buffer allocations are made by the application and passed by pointer.
 * The size of the application-allocated transmit and receive data buffers are the same
 * unless one of the increment flags is disabled resulting in a stationary data pointer.
 * If using a stationary data pointer, e.g., a FIFO address, the associated data buffer
 * allocation may be of size one (unless using DMA mode, see above).  \b Blocking-mode calls
 * cause the driver to block on this API until the transaction is complete.  \b Non-blocking
 * calls return right away and require the application to poll on the transfer completion
 * status or register/receive a callback, such that the application may perform other tasks.
 * The application may never submit multiple transactions and must always insure each
 * transaction is complete before submitting any other transactions.
 *
 * @par pTxData
 * Pointer to the application-defined transmit data buffer.  This is the data sent out
 * over the SPI transmit wire (MOSI for Master-mode, MISO for Slave-mode) during the SPI transaction.
 * For ADuCM350 SPI DMA mode (which is 16-bit based), the transmit buffer must be 16-bit aligned.
 *
 * @par pRxData
 * Pointer to the application-defined receive data buffer.  This is where the receive data
 * will be stored from the SPI receive wire (MISO for Master-mode, MOSI for Slave-mode)
 * during the SPI transaction.
 * For ADuCM350 SPI DMA mode (which is 16-bit based), the receive buffer must be 16-bit aligned.
 *
 * @par bTxIncrement
 * Increment flag for the transmit buffer.  If true, the transmit data buffer pointer is
 * advanced as each byte is sent.  If false, the transmit data pointer is stationary.  A
 * stationary buffer pointer is useful for sending the same data to an external device or if
 * the source data is from a fixed memory address.
 *
 * @par bRxIncrement
 * Increment flag for the receive buffer.  If true, the receive data buffer pointer is
 * advanced as each byte is received.  If false, the receive data pointer is stationary.  A
 * stationary buffer pointer is useful for monitoring commands from an external device or if
 * the receive data is going to a fixed memory address.
 *
 * @sa adi_SPI_GetBlockingMode().
 * @sa adi_SPI_GetDmaMode().
 * @sa adi_SPI_GetDmaRxComplete().
 * @sa adi_SPI_GetDmaTxComplete().
 * @sa adi_SPI_SetBlockingMode().
 * @sa adi_SPI_SetDmaMode().
 * @sa adi_SPI_SlaveComplete().
 */
ADI_SPI_RESULT_TYPE adi_SPI_SlaveTransfer (ADI_SPI_DEV_HANDLE const hDevice, ADI_SPI_TRANSCEIVE_TYPE* const pXfr)
{
    uint16_t i;

#ifdef ADI_DEBUG
    if (ADI_SPI_INVALID_HANDLE(hDevice))
        return ADI_SPI_ERR_BAD_DEV_HANDLE;

    if (ADI_SPI_STATE_SLAVE != hDevice->InitState)
        return ADI_SPI_ERR_NOT_INITIALIZED;

    /* slave SPI does not have a prologue */
    if (pXfr->pPrologue != NULL){
        return ADI_SPI_ERR_NULL_DATA_POINTER;
    }

    if ((NULL == pXfr->pTxData) || (NULL == pXfr->pRxData))
        return ADI_SPI_ERR_NULL_DATA_POINTER;

    if (0 == pXfr->DataSize)
        return ADI_SPI_ERR_PARAM_OUT_OF_RANGE;

    /* yell about odd byte counts in debug mode for ADuCM350 DMA requests */
    if (hDevice->bDmaMode && pXfr->DataSize%2)
        return ADI_SPI_ERR_PARAM_OUT_OF_RANGE;

    if (hDevice->bDmaMode && (hDevice->pSpi->SPIDMA & SPIDMA_ENABLE))
        return ADI_SPI_ERR_DMA_ALREADY_IN_USE;

#endif /* ADI_DEBUG */

    /* enable SPI */
    hDevice->pSpi->SPICON |= SPICON_ENABLE;

    if(!hDevice->bDmaMode) {

        /*********** need to protect this sequence *****/
        ADI_ENTER_CRITICAL_REGION();

        /* clear completion flag */
        hDevice->bComplete   = false;

        /* load transceiver */
        hDevice->pTxBuffer    = pXfr->pTxData;
        hDevice->pRxBuffer    = pXfr->pRxData;
        hDevice->TxRemaining  = pXfr->DataSize;
        hDevice->RxRemaining  = pXfr->DataSize;
        hDevice->TxIncrement  = (true == pXfr->bTxIncrement) ? 1 : 0;
        hDevice->RxIncrement  = (true == pXfr->bRxIncrement) ? 1 : 0;

        /* preload tx fifo (4 max) */
        for(i=0; i< MIN(4, pXfr->DataSize); i++){
            hDevice->pSpi->SPITX = *hDevice->pTxBuffer;
            hDevice->pTxBuffer += hDevice->TxIncrement;
            hDevice->TxRemaining--;
        }

        /* dummy rx fifo read to initiate the SPI transfer */
        hDevice->pSpi->SPIRX;

        /*********** turn off protection *****/
        ADI_EXIT_CRITICAL_REGION();

        /* polling loop for blocking mode */
        if (hDevice->bBlockingMode == true) {
            while(hDevice->bComplete == false);
        }

    } else {

        /* DMA requires valid pointers to both tx and rx buffers */
        if ((NULL == pXfr->pTxData) || (NULL == pXfr->pRxData)){
            return ADI_SPI_ERR_NULL_DATA_POINTER;
        }

        /* enable SPI for transmit initiate */
        /* disable the non-DMA interrupts */
        ADI_DISABLE_INT(hDevice->IRQn);

		/* The SPICNT register may be used to stop the SPI from clocking extra bytes
		   once the requested byte count had been pushed into the receive FIFO while
		   the DMA engine is still pulling the required count out of the FIFO; the
		   DMA count lags the SPI count by up to the size of the FIFO (4).  In the
		   case where the entire transaction size can fit into the SPICNT register
		   (8-bit count of 16-bit words = 512 bytes), we can use SPICNT.  For
		   transactions that exceed this limit, multiple DMA descriptors are used
		   and the SPICNT register cannot be used because the descriptor blocks
		   automatically ping-pong and loading the SPICNT register cannot occur
		   "on-time" as the DMA interrupts occur AFTER each new descriptor is
		   already "in flight".  In this case, the few extra receive bytes will
           land in the FIFO, but the DMA engine will not use them.
		*/
		if (pXfr->DataSize < ADI_SPI_MAX_TRANSFER_SIZE)
			hDevice->pSpi->SPICNT = pXfr->DataSize;
		else
			hDevice->pSpi->SPICNT = 0;  /* don't use */

        if (ADI_SPI_SUCCESS != (hDevice->FunctionTable.pInitDescriptorsFcn(hDevice, pXfr->pTxData, pXfr->pRxData, pXfr->DataSize))) {
            AssertChipSelect(hDevice, false);
            return ADI_SPI_ERR_NO_DMA_FOR_DEVICE;
        }
    }

    return ADI_SPI_SUCCESS;
}

/*!
 * @brief  Get the SPI slave transaction completion status.
 *
 * @param[in]     hDevice      Device handle obtained from adi_SPI_SlaveInit().
 *
 * @return Returns the completion state of a previously slave submitted buffer.
 *                - true      Transaction is complete.
 *                - false     Transaction is incomplete.
 *
 * @sa        adi_SPI_Slave_Transmit().
 * @sa        adi_SPI_Slave_Receive().
 */
bool_t adi_SPI_SlaveComplete (ADI_SPI_DEV_HANDLE const hDevice)
{
    return (hDevice->bComplete);
}


/*
 *****************************************************************************
 * SPI Internal Static Support Functions
 *****************************************************************************/


/*
 * Initialize the device state
 */
static ADI_SPI_RESULT_TYPE CommonInit (ADI_SPI_DEV_HANDLE const hDevice, ADI_SPI_DEV_ID_TYPE const devID)
{
    /* (re)initialize device state */
    hDevice->InitState                = ADI_SPI_STATE_UNINITIALIZED;
    hDevice->bBlockingMode            = false;
    hDevice->bDmaMode                 = false;
    hDevice->bComplete                = false;

    /* init callback */
    hDevice->pfCallback               = NULL;
    hDevice->pCBParam                 = NULL;

    switch (devID) {
        case ADI_SPI_DEVID_0:
            hDevice->ChipSelect.Port  = ADI_SPI0_PORT;
            hDevice->ChipSelect.Pin   = ADI_SPI0_DEFAULT_CS_PIN;
            hDevice->pSpi             = pADI_SPI0;
            hDevice->IRQn             = SPI0_IRQn;

#if (ADI_SPI_CFG_ENABLE_DMA_SUPPORT == 1)
            hDevice->pDmaDescriptor   = &gDmaDescriptorForSPI0;
            hDevice->DMA_TX_Chan      = SPI0_TX_CHANn;
            hDevice->DMA_RX_Chan      = SPI0_RX_CHANn;
            hDevice->DMA_TX_IRQn      = DMA_SPI0_TX_IRQn;
            hDevice->DMA_RX_IRQn      = DMA_SPI0_RX_IRQn;
#endif /* ADI_SPI_CFG_ENABLE_DMA_SUPPORT */

#if (ADI_SPI_CFG_INTERRUPT_MODE_SUPPORT == 1)
            ADI_INSTALL_HANDLER(hDevice->IRQn, SPI0_Int_Handler);
#endif /* ADI_SPI_CFG_INTERRUPT_MODE_SUPPORT  */
            break;

        case ADI_SPI_DEVID_1:
            hDevice->ChipSelect.Port  = ADI_SPI1_PORT;
            hDevice->ChipSelect.Pin   = ADI_SPI1_DEFAULT_CS_PIN;
            hDevice->pSpi             = pADI_SPI1;
            hDevice->IRQn             = SPI1_IRQn;

#if (ADI_SPI_CFG_ENABLE_DMA_SUPPORT == 1)
            hDevice->pDmaDescriptor   = &gDmaDescriptorForSPI1;
            hDevice->DMA_TX_Chan      = SPI1_TX_CHANn;
            hDevice->DMA_RX_Chan      = SPI1_RX_CHANn;
            hDevice->DMA_TX_IRQn      = DMA_SPI1_TX_IRQn;
            hDevice->DMA_RX_IRQn      = DMA_SPI1_RX_IRQn;
#endif /* ADI_SPI_CFG_ENABLE_DMA_SUPPORT */

#if (ADI_SPI_CFG_INTERRUPT_MODE_SUPPORT == 1)
            ADI_INSTALL_HANDLER(hDevice->IRQn, SPI1_Int_Handler);
#endif /* ADI_SPI_CFG_INTERRUPT_MODE_SUPPORT  */
            break;

        case ADI_SPI_DEVID_H:
            hDevice->ChipSelect.Port  = ADI_SPIH_PORT;
            hDevice->ChipSelect.Pin   = ADI_SPIH_DEFAULT_CS_PIN;
            hDevice->pSpi             = pADI_SPIH;
            hDevice->IRQn             = SPIH_IRQn;

#if (ADI_SPI_CFG_ENABLE_DMA_SUPPORT == 1)
            hDevice->pDmaDescriptor   = &gDmaDescriptorForSPIH;
            hDevice->DMA_TX_Chan      = SPIH_TX_CHANn;
            hDevice->DMA_RX_Chan      = SPIH_RX_CHANn;
            hDevice->DMA_TX_IRQn      = DMA_SPIH_TX_IRQn;
            hDevice->DMA_RX_IRQn      = DMA_SPIH_RX_IRQn;
#endif /* ADI_SPI_CFG_ENABLE_DMA_SUPPORT */

#if (ADI_SPI_CFG_INTERRUPT_MODE_SUPPORT == 1)
            ADI_INSTALL_HANDLER(hDevice->IRQn, SPIH_Int_Handler);
#endif /* ADI_SPI_CFG_INTERRUPT_MODE_SUPPORT  */
            break;

        default:
            return ADI_SPI_ERR_BAD_DEV_ID;
    }

#if (ADI_CFG_ENABLE_RTOS_SUPPORT == 1)
    hDevice->hSem = NULL;
#endif /* ADI_CFG_ENABLE_RTOS_SUPPORT  */

    /* initialize function table entries to point to non-dma implimentations */
    hDevice->FunctionTable.pInitFcn            = &intInit;
    hDevice->FunctionTable.pUnInitFcn          = &intUnInit;
    hDevice->FunctionTable.pInitDescriptorsFcn = &intInitializeDescriptors;
    hDevice->FunctionTable.pDmaHandlerFcn      = &intCommonHandler;

#if (ADI_CFG_ENABLE_RTOS_SUPPORT == 1)

    /* Create the semaphore for blocking mode support. */
    if(adi_osal_SemCreateStatic(&hDevice->SemMemory, ADI_OSAL_MAX_SEM_SIZE_CHAR, &hDevice->hSem, 0u) != ADI_OSAL_SUCCESS)
        return ADI_SPI_ERR_SEMAPHORE_FAILED;

#else

    /* init interrupt flag for low power mode */
    hDevice->bInterruptFlag = false;

#endif /* ADI_CFG_ENABLE_RTOS_SUPPORT  */

	return ADI_SPI_SUCCESS;
}


/*
 * Transfer internal default settings to SPI controller.
 */
static ADI_SPI_RESULT_TYPE ConfigureCoreAsMaster (ADI_SPI_DEV_HANDLE const hDevice)
{

    ADI_SPI_RESULT_TYPE result = ADI_SPI_SUCCESS;

        /* initialize UCLK divider register for default frequency */
        if (ADI_SPI_SUCCESS != (result = adi_SPI_SetBitrate(hDevice, ADI_SPI_BITRATE_INITIALIZER)))
            return result;  /* failure */

    ADI_ENTER_CRITICAL_REGION();

        /* start with a FIFO flush... */
        hDevice->pSpi->SPICON |= (SPICON_TFLUSH | SPICON_RFLUSH);

		/* ...then reset with the master initializer */
		hDevice->pSpi->SPICON = ADI_SPI_MASTERCON_INITIALIZER;

    ADI_EXIT_CRITICAL_REGION();

    /* clear DMA bytecount register */
	hDevice->pSpi->SPICNT = 0;

    /* initialize the DMA register */
    hDevice->pSpi->SPIDMA = ADI_SPI_DMA_INITIALIZER;

    return result;
}


/*
 * Transfer internal default settings to SPI controller.
 */
static ADI_SPI_RESULT_TYPE ConfigureCoreAsSlave (ADI_SPI_DEV_HANDLE const hDevice)
{

    ADI_SPI_RESULT_TYPE result = ADI_SPI_SUCCESS;

    ADI_ENTER_CRITICAL_REGION();

        /* start with a FIFO flush... */
        hDevice->pSpi->SPICON |= (SPICON_TFLUSH | SPICON_RFLUSH);

		/* ...then reset with the slave initializer */
		hDevice->pSpi->SPICON = ADI_SPI_SLAVECON_INITIALIZER;

    ADI_EXIT_CRITICAL_REGION();

    /* clear DMA bytecount register */
	hDevice->pSpi->SPICNT = 0;

    return result;
}


/*
 * Chip select signals are micromanaged by the driver because the SPI controller
 * CS signals do not allow for extended transfers without resetting CS and hence,
 * resetting the external device addressing mode.  Therefore, its just easier to
 * manage it as needed.
 *
 * If bActivate is true, then activate the port-pin pair for use.  If false, deactivate
 * the port-pin pair in prep for a new CS port-pin pair configuration.
 */
static ADI_SPI_RESULT_TYPE ConfigureMasterCS (ADI_SPI_DEV_HANDLE const hDevice, bool_t const bActivate)
{
	ADI_GPIO_PORT_TYPE port = hDevice->ChipSelect.Port;
	ADI_GPIO_DATA_TYPE pin  = hDevice->ChipSelect.Pin;

    /* enable/disable CS pullup */
    if (ADI_GPIO_SUCCESS != adi_GPIO_SetPullUpEnable(port, pin, bActivate))
        return ADI_SPI_ERR_BAD_GPIO_CONFIG;

    /* deassert CS */
    if (ADI_GPIO_SUCCESS != adi_GPIO_SetHigh(port, pin))
        return ADI_SPI_ERR_BAD_GPIO_CONFIG;

    /* enable/disable driver */
    if (ADI_GPIO_SUCCESS != adi_GPIO_SetOutputEnable(port, pin, bActivate))
        return ADI_SPI_ERR_BAD_GPIO_CONFIG;

    return ADI_SPI_SUCCESS;
}


/*
 * Assert or Deassert the Chip Select signal for the specified SPI device.
 */
static ADI_SPI_RESULT_TYPE AssertChipSelect(ADI_SPI_DEV_HANDLE const hDevice, const bool_t enable)
{

    uint32_t Port;
    ADI_GPIO_PORT_TYPE Pin;

    Port = hDevice->ChipSelect.Port;
    Pin  = hDevice->ChipSelect.Pin;

    /* drive the set/clear the pin (note negative logic on CS) */
    if (ADI_GPIO_SUCCESS != ((true == enable) ? adi_GPIO_SetLow(Port, Pin) : adi_GPIO_SetHigh(Port, Pin)))
        return ADI_SPI_ERR_BAD_GPIO_CONFIG;

    return ADI_SPI_SUCCESS;
}


/*
    //////////////////////////////////////////////////////////////////////////////
    ////////////////////   POINTER TABLE FUNCTIONS   /////////////////////////////
    //////////////////////////////////////////////////////////////////////////////
*/


/* interrupt-mode initializer */
static ADI_SPI_RESULT_TYPE intInit(ADI_SPI_DEV_HANDLE const hDevice) {
    return ADI_SPI_SUCCESS;
}


/* interrupt-mode uninitializer */
static ADI_SPI_RESULT_TYPE intUnInit(ADI_SPI_DEV_HANDLE const hDevice) {
    return ADI_SPI_SUCCESS;
}


/* dma-mode initializer */
#if (ADI_SPI_CFG_ENABLE_DMA_SUPPORT == 1)
static ADI_SPI_RESULT_TYPE dmaInit(ADI_SPI_DEV_HANDLE const hDevice) {

    /* allocate TX/RX channels for this device */
    if (ADI_DMA_SUCCESS != adi_DMA_Init(hDevice->DMA_TX_Chan, ADI_DMA_PRIORITY_RESET))
        return ADI_SPI_ERR_BAD_DMA_CONFIG;

    if (ADI_DMA_SUCCESS != adi_DMA_Init(hDevice->DMA_RX_Chan, ADI_DMA_PRIORITY_RESET))
        return ADI_SPI_ERR_BAD_DMA_CONFIG;

    /* install DMA interrupt handlers */
    switch (hDevice->DMA_TX_Chan) {
        case SPI0_TX_CHANn:
            ADI_INSTALL_HANDLER(hDevice->DMA_TX_IRQn, DMA_SPI0_TX_Int_Handler);
            ADI_INSTALL_HANDLER(hDevice->DMA_RX_IRQn, DMA_SPI0_RX_Int_Handler);
            break;
        case SPI1_TX_CHANn:
            ADI_INSTALL_HANDLER(hDevice->DMA_TX_IRQn, DMA_SPI1_TX_Int_Handler);
            ADI_INSTALL_HANDLER(hDevice->DMA_RX_IRQn, DMA_SPI1_RX_Int_Handler);
            break;

        case SPIH_TX_CHANn:
            ADI_INSTALL_HANDLER(hDevice->DMA_TX_IRQn, DMA_SPIH_TX_Int_Handler);
            ADI_INSTALL_HANDLER(hDevice->DMA_RX_IRQn, DMA_SPIH_RX_Int_Handler);
            break;

        default:
            return ADI_SPI_ERR_BAD_DEV_ID;
    }
    return ADI_SPI_SUCCESS;
}
#endif /* ADI_SPI_CFG_ENABLE_DMA_SUPPORT */


/* dma-mode uninitializer */
#if (ADI_SPI_CFG_ENABLE_DMA_SUPPORT == 1)
static ADI_SPI_RESULT_TYPE dmaUnInit(ADI_SPI_DEV_HANDLE const hDevice) {

    ADI_SPI_RESULT_TYPE result = ADI_SPI_SUCCESS;

    /* shut down any DMA action */
    if (ADI_SPI_SUCCESS != (result = EnableDma(hDevice, false)))        return result;
    if (ADI_SPI_SUCCESS != (result = EnableDmaRxIRQ(hDevice, false)))   return result;
    if (ADI_SPI_SUCCESS != (result = EnableDmaTxIRQ(hDevice, false)))   return result;

    /* deallocate channels */
    if (ADI_DMA_SUCCESS != adi_DMA_UnInit(hDevice->DMA_TX_Chan)) return ADI_SPI_ERR_BAD_DMA_CONFIG;
    if (ADI_DMA_SUCCESS != adi_DMA_UnInit(hDevice->DMA_RX_Chan)) return ADI_SPI_ERR_BAD_DMA_CONFIG;

    /* un-install DMA interrupt handlers */
    ADI_UNINSTALL_HANDLER(hDevice->DMA_TX_IRQn);
    ADI_UNINSTALL_HANDLER(hDevice->DMA_RX_IRQn);
    return result;
}
#endif /* ADI_SPI_CFG_ENABLE_DMA_SUPPORT */


/*
    //////////////////////////////////////////////////////////////////////////////
    ///////////////////////   INTERRUPT-RELATED   ////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////
*/

static void common_SPI_Int_Handler (ADI_SPI_DEV_DATA_TYPE* pDev)
{
    uint16_t status;
    uint16_t writableBytes;
    uint16_t readableBytes;

    /* read status register - first thing */
    status = pDev->pSpi->SPISTA;

#if (ADI_SPI_CFG_INTERRUPT_MODE_SUPPORT == 1)

#ifdef ADI_DEBUG
    /* Trap overflow/underflow errors in debug mode only. */
    if (SPISTA_RXOF & status) {
        //while(1);
    }
    if (SPISTA_TXUR & status){
        //while(1);
    }
#endif /* ADI_DEBUG */

    /* calculate number of bytes that can be written to tx fifo */
    writableBytes = 4 - ((SPISTA_TXFSTA_MSK & status) >> 1);

    /* calculate number of bytes to read from rx fifo */
    readableBytes = ((SPISTA_RXFSTA_MSK & status) >> 8);

    /* push to the tx fifo */
    while (writableBytes--) {
        if (pDev->TxRemaining) {
            pDev->pSpi->SPITX = *pDev->pTxBuffer;
            pDev->pTxBuffer  += pDev->TxIncrement;
            pDev->TxRemaining--;
        }
    }

    /* pull from the rx fifo */
    while (readableBytes--) {
        if (pDev->RxRemaining) {
            *pDev->pRxBuffer = pDev->pSpi->SPIRX;
            pDev->pRxBuffer += pDev->RxIncrement;
            pDev->RxRemaining--;
        }
    }

    /* termination */
    if (pDev->RxRemaining == 0 && pDev->TxRemaining == 0) {

		/* set the completion flag */
		pDev->bComplete = true;

		/* deselect slave in master mode */
        if (pDev->InitState == ADI_SPI_STATE_MASTER) {
            /* Note: the SPI enable bit used to be cleared here and re-enabled
               at the start of the next transaction.  The previous comments
               indicated something about avoiding an extra transmit interrupt,
               which is not apparent.  Now we avoid dynamically switching the
               SPI enable because it gates off internal clocking such that setup
               operations are missed, etc.  This was resulting in (4) extra
               clock pulses going out both before that next transaction and
               following this one... sort of a nibble's worth on each end.  It
               was not noticable until they fell within the CS strobe.  The SPI
               controller is not designed to be constantly enabled and disabled,
               so now we just leave it on.  The other helper is to always use
               the spi count register for both transmit and receive so that
               extra bytes are not processed by the controller.
            */
            AssertChipSelect(pDev, false);
		}

        /* complete any blocking mode activity */
        if (pDev->bBlockingMode == true) {
#if (ADI_CFG_ENABLE_RTOS_SUPPORT == 1)
            adi_osal_SemPost(pDev->hSem);
#else

            /* Any interrupt that changes the SPI device state needs to bring the
             * processor out of low-power mode, as functions may be waiting for the
             * state to change.
             */
            SystemExitLowPowerMode(&pDev->bInterruptFlag);
#endif /* ADI_CFG_ENABLE_RTOS_SUPPORT  */
        }

        /* If a callback is registered notify the buffer processed event to the application */
        if(pDev->pfCallback){
            pDev->pfCallback(pDev->pCBParam, ADI_SPI_EVENT_BUFFER_PROCESSED, NULL);
        }
    }
#endif /* ADI_SPI_CFG_INTERRUPT_MODE_SUPPORT  */

    /* All interrupts are cleared by a read of the status register */
    status = pDev->pSpi->SPISTA;
}


/*!
 * @brief  SPI0 Interrupt Handler.
 *
 * @return void.
 *
 * Overrides default SPI0 interrupt handler.
 */
ADI_INT_HANDLER(SPI0_Int_Handler) {
    common_SPI_Int_Handler(pSPI0_DevData);
}


/*!
 * @brief  SPI1 Interrupt Handler.
 *
 * @return void.
 *
 * Overrides default SPI1 interrupt handler.
 */
ADI_INT_HANDLER(SPI1_Int_Handler) {
    common_SPI_Int_Handler(pSPI1_DevData);
}


/*!
 * @brief  SPIH Interrupt Handler.
 *
 * @return void.
 *
 * Overrides default SPIH interrupt handler.
 */
ADI_INT_HANDLER(SPIH_Int_Handler) {
    common_SPI_Int_Handler(pSPIH_DevData);
}


/*
    //////////////////////////////////////////////////////////////////////////////
    //////////////////////////   DMA-RELATED   ///////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////
*/

/*
 * Directly manipulates the DMA eneble bit in the SPI DMA control register.
 */
#if (ADI_SPI_CFG_ENABLE_DMA_SUPPORT == 1)
static ADI_SPI_RESULT_TYPE EnableDma (ADI_SPI_DEV_HANDLE const hDevice, const bool_t bFlag)
{

#ifdef ADI_DEBUG
    if (ADI_SPI_INVALID_HANDLE(hDevice))
        return ADI_SPI_ERR_BAD_DEV_HANDLE;

    if ((hDevice->InitState != ADI_SPI_STATE_MASTER) && (hDevice->InitState != ADI_SPI_STATE_SLAVE))
        return ADI_SPI_ERR_NOT_INITIALIZED;
#endif

    if (true == bFlag)
        hDevice->pSpi->SPIDMA |= (SPIDMA_ENABLE);
    else
        hDevice->pSpi->SPIDMA &= ~(SPIDMA_ENABLE);

    return ADI_SPI_SUCCESS;
}
#endif /* ADI_SPI_CFG_ENABLE_DMA_SUPPORT */


/*
 * Enable or disable DMA interrupts on the transmit DMA channel.
 */
#if (ADI_SPI_CFG_ENABLE_DMA_SUPPORT == 1)
static ADI_SPI_RESULT_TYPE EnableDmaTxIRQ (ADI_SPI_DEV_HANDLE const hDevice, const bool_t bFlag)
{
#ifdef ADI_DEBUG
    if (ADI_SPI_INVALID_HANDLE(hDevice))
        return ADI_SPI_ERR_BAD_DEV_HANDLE;

    if ((hDevice->InitState != ADI_SPI_STATE_MASTER) && (hDevice->InitState != ADI_SPI_STATE_SLAVE))
        return ADI_SPI_ERR_NOT_INITIALIZED;
#endif

    if (true == bFlag) {
        ADI_ENABLE_INT(hDevice->DMA_TX_IRQn);
        hDevice->pSpi->SPIDMA |= (SPIDMA_IENTXDMA);
    } else {
        ADI_DISABLE_INT(hDevice->DMA_TX_IRQn);
        hDevice->pSpi->SPIDMA &= ~(SPIDMA_IENTXDMA);
    }

    return ADI_SPI_SUCCESS;
}
#endif /* ADI_SPI_CFG_ENABLE_DMA_SUPPORT */


/*
 * Enable or disable DMA interrupts on the receive DMA channel.
 */
#if (ADI_SPI_CFG_ENABLE_DMA_SUPPORT == 1)
static ADI_SPI_RESULT_TYPE EnableDmaRxIRQ (ADI_SPI_DEV_HANDLE const hDevice, const bool_t bFlag)
{

#ifdef ADI_DEBUG
    if (ADI_SPI_INVALID_HANDLE(hDevice))
        return ADI_SPI_ERR_BAD_DEV_HANDLE;

    if ((hDevice->InitState != ADI_SPI_STATE_MASTER) && (hDevice->InitState != ADI_SPI_STATE_SLAVE))
        return ADI_SPI_ERR_NOT_INITIALIZED;
#endif

    if (true == bFlag) {
        ADI_ENABLE_INT(hDevice->DMA_RX_IRQn);
        hDevice->pSpi->SPIDMA |= (SPIDMA_IENRXDMA);
    } else {
        ADI_DISABLE_INT(hDevice->DMA_RX_IRQn);
        hDevice->pSpi->SPIDMA &= ~(SPIDMA_IENRXDMA);
    }

    return ADI_SPI_SUCCESS;
}
#endif /* ADI_SPI_CFG_ENABLE_DMA_SUPPORT */


/* stub for interrupt mode descriptor initialization (should never land here) */
static ADI_SPI_RESULT_TYPE intInitializeDescriptors(
    ADI_SPI_DEV_HANDLE const hDevice,
    const uint8_t* const pDataTx,
    uint8_t* const pDataRx,
    uint32_t const Size) {
    return ADI_SPI_ERR_UNSUPPORTED_MODE;
}


/* DMA mode descriptor initialization */
#if (ADI_SPI_CFG_ENABLE_DMA_SUPPORT == 1)
/*
 * Programs the DMA transmit and receive descriptors for ping-pong mode transfers, as needed.
 * We always use ping-pong transfer mode and the first two descriptors are always programmed,
 * even if the transfer would fit in one.  This is because ping-pong mode will always switch
 * to the next descriptor, even if the last one completed the transfer and the switch is to
 * a zero-sized descriptor.  The DMA interrupt at the end of each descriptor will program the
 * next one while the new descriptor is already underway.  Thats why we need to pre-program
 * two.
 */
static ADI_SPI_RESULT_TYPE dmaInitializeDescriptors(
    ADI_SPI_DEV_HANDLE const hDevice,
    const uint8_t* const pDataTx,
    uint8_t* const pDataRx,
    uint32_t const Size)
{
    ADI_SPI_RESULT_TYPE result = ADI_SPI_SUCCESS;
    ADI_DMA_TRANSFER_TYPE* pD = hDevice->pDmaDescriptor;

    /* no protection */
    pD->Protection = ADI_DMA_HPROT_NONE;

    /* rearbitrate after each transaction so that */
    /* transmit and receive are byte-synchronious */
    pD->ArbitRate  = ADI_DMA_RPOWER_1;

    /* set stride */
    pD->DataWidth  = ADI_DMA_DATA_WIDTH;

    /* initialize the counts */
    hDevice->TxRemaining = Size;
    hDevice->RxRemaining = Size;

    /* initialize the data pointers */
    hDevice->pTxBuffer = (void*) pDataTx;
    hDevice->pRxBuffer = (void*) pDataRx;

    /* initialize the transfer completion flags */
    hDevice->bComplete  = false;

    /* program the descriptors... */

    /* always initialize both ping and pong descriptors          */
    /* (primary and alternate) -- even if the transfer would fit */
    /* in one; auto-stop mode requires an invalid descriptor.    */

    /* initialize the ping-pong descriptor flags to always start with the primary descriptor */
    hDevice->nextTxDescr  = ADI_DMA_CCD_PRIMARY;
    hDevice->nextRxDescr  = ADI_DMA_CCD_PRIMARY;

    /* initialize ping transmit (primary) descriptor */
    if (ADI_SPI_SUCCESS != (result = ScheduleNextDescriptor(hDevice, hDevice->DMA_TX_Chan)))
        return result;

    /* initialize pong transmit (alternate) descriptor */
    if (ADI_SPI_SUCCESS != (result = ScheduleNextDescriptor(hDevice, hDevice->DMA_TX_Chan)))
        return result;

    /* initialize ping receive (primary) descriptor */
    if (ADI_SPI_SUCCESS != (result = ScheduleNextDescriptor(hDevice, hDevice->DMA_RX_Chan)))
        return result;

    /* initialize pong receive (alternate) descriptor */
    if (ADI_SPI_SUCCESS != (result = ScheduleNextDescriptor(hDevice, hDevice->DMA_RX_Chan)))
        return result;


    /* enable SPI TX DMA channel interrupt (disabled by DMA handler)... */
    if (ADI_SPI_SUCCESS != (result = EnableDmaTxIRQ(hDevice, true))) {
        AssertChipSelect(hDevice, false);
        return result;
    }

    /* enable SPI RX DMA channel interrupt (disabled by DMA handler)... */
    if (ADI_SPI_SUCCESS != (result = EnableDmaRxIRQ(hDevice, true))) {
        AssertChipSelect(hDevice, false);
        return result;
    }

    /* launch transfer with SPI DMA enable bit */
    if (ADI_SPI_SUCCESS != (result = EnableDma(hDevice, true))) {
        AssertChipSelect(hDevice, false);
        return result;
    }

    return result;
}
#endif /* ADI_SPI_CFG_ENABLE_DMA_SUPPORT */


/*
 * Programs the next DMA descriptor in the ping-pong sequence and updates various pointers and counters, as necassary.
 */
#if (ADI_SPI_CFG_ENABLE_DMA_SUPPORT == 1)
static ADI_SPI_RESULT_TYPE ScheduleNextDescriptor(
    ADI_SPI_DEV_HANDLE const hDevice,
    DMA_CHANn_TypeDef const chan)
{
    ADI_SPI_RESULT_TYPE result = ADI_SPI_SUCCESS;
    register ADI_DMA_TRANSFER_TYPE* pD = hDevice->pDmaDescriptor;
    register uint32_t nextChunkSize;

    /* set the channel number */
    pD->Chan = chan;

    /* prepare the descriptor */
    switch (chan) {
        case SPI0_TX_CHANn:
        case SPI1_TX_CHANn:
        case SPIH_TX_CHANn:

            /* PREPARE TRANSMIT DESCRIPTOR */

            /* get the next descriptor type */
            pD->CCD                   = hDevice->nextTxDescr;

            /* switch it for next time */
            hDevice->nextTxDescr      = (ADI_DMA_CCD_PRIMARY == hDevice->nextTxDescr)
                                      ? ADI_DMA_CCD_ALTERNATE
                                      : ADI_DMA_CCD_PRIMARY;

            /* schedule next transmit (BUFFER ==> FIFO) */
            pD->pSrcData              = hDevice->pTxBuffer;
            pD->SrcInc                = ADI_DMA_DATA_INCREMENT;
            pD->pDstData              = (void*) &hDevice->pSpi->SPITX;
            pD->DstInc                = ADI_DMA_INCR_NONE;

            /* update next transmit descriptor */
            if (nextChunkSize = MIN(hDevice->TxRemaining/2, ADI_SPI_MAX_TRANSFER_SIZE)) {
                /* non-zero data count remaining */
                pD->Mode              = ADI_DMA_MODE_PINGPONG;
                pD->DataLength        = nextChunkSize;
                hDevice->pTxBuffer   += nextChunkSize*2; /* stride of 2 because its 16-bit mode */
                hDevice->TxRemaining -= nextChunkSize*2;
            } else {
                /* zero data count remaining */
                pD->Mode              = ADI_DMA_MODE_INVALID;
                pD->DataLength        = 0;
                hDevice->pTxBuffer    = NULL;
                hDevice->TxRemaining  = 0;
            }

#ifdef DMA_DEBUGGING
			if (pD->CCD == ADI_DMA_CCD_PRIMARY) {
				dmaEventTable[dmaEventIndex++].txPri = nextChunkSize;
			} else {
				dmaEventTable[dmaEventIndex++].txAlt = nextChunkSize;
			}
#endif

            break;

        case SPI0_RX_CHANn:
        case SPI1_RX_CHANn:
        case SPIH_RX_CHANn:

            /* PREPARE RECEIVE DESCRIPTOR */

            /* get the next descriptor type */
            pD->CCD                   = hDevice->nextRxDescr;

            /* switch it for next time */
            hDevice->nextRxDescr      = (ADI_DMA_CCD_PRIMARY == hDevice->nextRxDescr)
                                      ? ADI_DMA_CCD_ALTERNATE
                                      : ADI_DMA_CCD_PRIMARY;

            /* schedule next receive (FIFO ==> BUFFER) */
            pD->pSrcData              = (void*) &hDevice->pSpi->SPIRX;
            pD->SrcInc                = ADI_DMA_INCR_NONE;
            pD->pDstData              = hDevice->pRxBuffer;
            pD->DstInc                = ADI_DMA_DATA_INCREMENT;

            /* update next receive descriptor */
            if (nextChunkSize = MIN(hDevice->RxRemaining/2, ADI_SPI_MAX_TRANSFER_SIZE)) {
                /* non-zero data count remaining */
                pD->Mode              = ADI_DMA_MODE_PINGPONG;
                pD->DataLength        = nextChunkSize;
                hDevice->pRxBuffer   += nextChunkSize*2; /* stride of 2 because its 16-bit mode */
                hDevice->RxRemaining -= nextChunkSize*2;
            } else {
                /* zero data count remaining */
                pD->Mode              = ADI_DMA_MODE_INVALID;
                pD->DataLength        = 0;
                hDevice->pRxBuffer    = NULL;
                hDevice->RxRemaining  = 0;
            }

#ifdef DMA_DEBUGGING
			if (pD->CCD == ADI_DMA_CCD_PRIMARY) {
				dmaEventTable[dmaEventIndex++].rxPri = nextChunkSize;
			} else {
				dmaEventTable[dmaEventIndex++].rxAlt = nextChunkSize;
			}
#endif

            break;
    }

    /* submit the new descriptor */
    if (ADI_DMA_SUCCESS != adi_DMA_SubmitTransfer(pD))
        return ADI_SPI_ERR_BAD_DMA_CONFIG;

    return result;
}
#endif /* ADI_SPI_CFG_ENABLE_DMA_SUPPORT */


/* Common DMA Handler for non-dma mode operation (nop) */
static void intCommonHandler(DMA_CHANn_TypeDef channelNum)
{
    /* No dma interrupts should ever come through this path.   */
    /* Only here for symmetry with the dmaCommonHandler() call */
    /* dispatched through function pointer pDmaHandlerFcn so   */
    /* we trap any such calls as an error and avoid silent     */
    /* failure.                                                */
    while (1) ;
}


/* Common DMA Handler code for DMA-mode operation*/
#if (ADI_SPI_CFG_ENABLE_DMA_SUPPORT == 1)
static void dmaCommonHandler(DMA_CHANn_TypeDef chan)
{
    register ADI_SPI_DEV_DATA_TYPE* pDev;
    bool_t bTransmit;

    /* recover SPI device pointer */
    switch (chan) {
        case SPI0_TX_CHANn:
        case SPI0_RX_CHANn:
            pDev = pSPI0_DevData;
            break;
        case SPI1_TX_CHANn:
        case SPI1_RX_CHANn:
            pDev = pSPI1_DevData;
            break;
        case SPIH_TX_CHANn:
        case SPIH_RX_CHANn:
            pDev = pSPIH_DevData;
            break;
    }

    /* generate a transmit flag */
    bTransmit= ((chan == SPI0_TX_CHANn) || (chan == SPI1_TX_CHANn) || (chan == SPIH_TX_CHANn)) ? true : false;

    /* read descriptor state */
    uint32_t pri_mode = (&pADI_DCCPRI[chan])->DMACDC & DMA_CCD_CYCLE_MASK;
    uint32_t alt_mode = (&pADI_DCCALT[chan])->DMACDC & DMA_CCD_CYCLE_MASK;

#ifdef DMA_DEBUGGING
	if (bTransmit) {
		dmaEventTable[dmaEventIndex++].txInt = pri_mode+alt_mode;
	} else {
		dmaEventTable[dmaEventIndex++].rxInt = pri_mode+alt_mode;
	}
#endif

	if (!pri_mode && !alt_mode) {

		/* both descriptors are done... */

		/* ignore the transmit complete and only mark things done with receive complete */
		if (!bTransmit) {

			/* receive channel is done... */

            /* set completion flag */
            pDev->bComplete = true;

            /* disable further DMA interrupts on the completed channel */
            EnableDmaTxIRQ(pDev, false);
            EnableDmaRxIRQ(pDev, false);

            /* stop SPI DMA */
            EnableDma(pDev, false);   //THIS is shutting down slave too early ????

            /* deassert master-mode CS */
            if (pDev->InitState == ADI_SPI_STATE_MASTER) {
                AssertChipSelect(pDev, false);
            }

            /* clear any blocking mode */
            if (pDev->bBlockingMode == true) {

#if (ADI_CFG_ENABLE_RTOS_SUPPORT == 1)
                adi_osal_SemPost(pDev->hSem);
#else

                /* Any interrupt that changes the SPI device state needs to bring the
                * processor out of low-power mode, as functions may be waiting for the
                * state to change.
                */
                SystemExitLowPowerMode(&pDev->bInterruptFlag);
#endif /* ADI_CFG_ENABLE_RTOS_SUPPORT */

            } /* end blocking mode code */

            /* If a callback is registered notify the buffer processed event to the application */
            if (pDev->pfCallback) {
                pDev->pfCallback(pDev->pCBParam, ADI_SPI_EVENT_BUFFER_PROCESSED, NULL);
            }

		} /* end bTransmit */

    } else {

		/* at least one descriptor is still active... schedule next descriptor */
		ScheduleNextDescriptor(pDev, (bTransmit ? pDev->DMA_TX_Chan : pDev->DMA_RX_Chan));
	}
}
#endif /* ADI_SPI_CFG_ENABLE_DMA_SUPPORT */


/*!
 * @brief  SPI0 DMA Transmit Interrupt Handler.
 *
 * @return void.
 *
 * Overrides default SPI0 DMA transmit interrupt handler.
 */
ADI_INT_HANDLER(DMA_SPI0_TX_Int_Handler) {
    pSPI0_DevData->FunctionTable.pDmaHandlerFcn(SPI0_TX_CHANn);
}


/*!
 * @brief  SPI0 DMA Receive Interrupt Handler.
 *
 * @return void.
 *
 * Overrides default SPI0 DMA receive interrupt handler.
 */
ADI_INT_HANDLER(DMA_SPI0_RX_Int_Handler) {
    pSPI0_DevData->FunctionTable.pDmaHandlerFcn(SPI0_RX_CHANn);
}


/*!
 * @brief  SPI1 DMA Transmit Interrupt Handler.
 *
 * @return void.
 *
 * Overrides default SPI1 DMA transmit interrupt handler.
 */
ADI_INT_HANDLER(DMA_SPI1_TX_Int_Handler) {
    pSPI1_DevData->FunctionTable.pDmaHandlerFcn(SPI1_TX_CHANn);
}


/*!
 * @brief  SPI1 DMA Receive Interrupt Handler.
 *
 * @return void.
 *
 * Overrides default SPI1 DMA receive interrupt handler.
 */
ADI_INT_HANDLER(DMA_SPI1_RX_Int_Handler) {
    pSPI1_DevData->FunctionTable.pDmaHandlerFcn(SPI1_RX_CHANn);
}


/*!
 * @brief  SPIH DMA Transmit Interrupt Handler.
 *
 * @return void.
 *
 * Overrides default SPIH DMA transmit interrupt handler.
 */
ADI_INT_HANDLER(DMA_SPIH_TX_Int_Handler) {
    pSPIH_DevData->FunctionTable.pDmaHandlerFcn(SPIH_TX_CHANn);
}


/*!
 * @brief  SPIH DMA Receive Interrupt Handler.
 *
 * @return void.
 *
 * Overrides default SPIH DMA receive interrupt handler.
 */
ADI_INT_HANDLER(DMA_SPIH_RX_Int_Handler) {
    pSPIH_DevData->FunctionTable.pDmaHandlerFcn(SPIH_RX_CHANn);
}


#define REVERT_ADI_MISRA_SUPPRESSIONS  /*!< Revert ADI MISRA Suppressions */
#include "misra.h"

/*
** EOF
*/

/*@}*/
