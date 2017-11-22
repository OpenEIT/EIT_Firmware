/*********************************************************************************

Copyright (c) 2013-2014 Analog Devices, Inc. All Rights Reserved.

This software is proprietary to Analog Devices, Inc. and its licensors.  By using 
this software you agree to the terms of the associated Analog Devices Software 
License Agreement.

*********************************************************************************/

/*!
 *****************************************************************************
 * @file:    i2c.c
 * @brief:   I2C Device Implementations for ADuCxxx
 * @version: $Revision: 28525 $
 * @date:    $Date: 2014-11-12 14:51:26 -0500 (Wed, 12 Nov 2014) $
 *****************************************************************************/

/*! \addtogroup I2C_Driver I2C Driver
 *  @{
 */

#define ASSERT_ADI_MISRA_SUPPRESSIONS  /*!< Apply ADI MISRA Suppressions */
#include "misra.h"

#include <stdlib.h>  /* for 'NULL" definition */
#include "adi_int.h"
#include "i2c.h"
#include "dma.h"
#include "gpio.h"

/* reconcile references to the FIRST and SECOND Master Address Byte (MAB)
   registers that are named differently, depending on processor port.
*/

/* MAB naming is one-based on 350 */
#define FIRST_MAB   (hDevice->pDev->I2CADR1)     /*!< The first master address byte. */
#define SECOND_MAB  (hDevice->pDev->I2CADR2)    /*!< The second master address byte. */

/* wait abstractions by OS, Processor, etc... */

/* BOOLEAN WAIT */
#if (ADI_CFG_ENABLE_RTOS_SUPPORT == 1)
#define BOOLEAN_WAIT(hDev, field) {                                                    \
	adi_osal_SemPend(hDev->hSem, ADI_OSAL_TIMEOUT_FOREVER);                        \
}
#else
#define BOOLEAN_WAIT(hDev, field) {                                                     \
	SystemEnterLowPowerMode(ADI_SYS_MODE_CORE_SLEEP, &hDev->bLowPowerExitFlag, 0);  \
}
#endif /* ADI_CFG_ENABLE_RTOS_SUPPORT */


/* BOOLEAN UNWAIT */
#if (ADI_CFG_ENABLE_RTOS_SUPPORT == 1)
#define BOOLEAN_UNWAIT(hDev, field) {                                                   \
	hDev->##field = true;                                                           \
	adi_osal_SemPost(hDev->hSem);                                                   \
}
#else
#define BOOLEAN_UNWAIT(hDev, field) {                                                   \
	hDev->##field = true;                                                           \
	SystemExitLowPowerMode(&hDev->bLowPowerExitFlag);                               \
}
#endif /* ADI_CFG_ENABLE_RTOS_SUPPORT */


#ifdef ADI_DEBUG
#define ADI_I2C_INVALID_HANDLE(h) ((pI2C0_DevData != h))
#endif


/* forward interrupt declarations... */
/* These are needed in the case where ADI_CFG_ENABLE_RTOS_SUPPORT=1              */
/* The ADI_INSTALL_HANDLER macro will expand to a call to adi_int_InstallHandler */
/* and the handler will be referenced                                            */
/* The non-OSAL expansion of ADI_INSTALL_HANDLER results in a NULL string        */

	/* I2C0 Master Mode */
	ADI_INT_HANDLER(I2C0_Master_Int_Handler);
#if (ADI_I2C_CFG_ENABLE_DMA_SUPPORT == 1)
	ADI_INT_HANDLER(DMA_I2C0_MRX_Int_Handler);
	ADI_INT_HANDLER(DMA_I2C0_MTX_Int_Handler);
	ADI_INT_HANDLER(DMA_I2C0_MX_Int_Handler);
#endif // ADI_I2C_CFG_ENABLE_DMA_SUPPORT

#if (ADI_I2C_CFG_SLAVE_MODE_SUPPORT == 1)
	/* I2C0 Slave Mode */
	ADI_INT_HANDLER(I2C0_Slave_Int_Handler);
#if (ADI_I2C_CFG_ENABLE_DMA_SUPPORT == 1)
	ADI_INT_HANDLER(DMA_I2C0_SRX_Int_Handler);
	ADI_INT_HANDLER(DMA_I2C0_STX_Int_Handler);
#endif // ADI_I2C_CFG_ENABLE_DMA_SUPPORT
#endif // ADI_I2C_CFG_SLAVE_MODE_SUPPORT


/*
    ADuCM350 uses special-case code to handle combined Master-Mode
    DMA TX & RX on a shared DMA channel and a shared DMA interrupt.
    This is due to having combined both transmit and receive DMA
    onto a single, shared channel... but only on the ADuCM350.

    Create "pseudo" channel IDs identify pseudo channel dma processing...
*/
#define I2CM_TX_CHANn -1    /*!< The pseudo master transmit dma channel for ADuCM350. */
#define I2CM_RX_CHANn -2    /*!< The pseudo master receive dma channel for ADuCM350. */

/* Dispatch DMA overhead through function pointers so linker */
/* elimination can optimize them out when not referenced...  */

/*! \typedef ADI_I2C_INIT_FPTR
 * The pointer to DMA/Interrupt initialization function. */
typedef ADI_I2C_RESULT_TYPE (*ADI_I2C_INIT_FPTR)(ADI_I2C_DEV_HANDLE const hDevice);

/*! \typedef ADI_I2C_UNINIT_FPTR
 * The pointer to DMA/Interrupt uninitialization function. */
typedef ADI_I2C_RESULT_TYPE (*ADI_I2C_UNINIT_FPTR)(ADI_I2C_DEV_HANDLE const hDevice);

/*! \typedef ADI_I2C_INIT_DESCRS_FPTR
 * The pointer to DMA descriptor initialization function. */
typedef ADI_I2C_RESULT_TYPE (*ADI_I2C_INIT_DESCRS_FPTR)(ADI_I2C_DEV_HANDLE const hDevice, DMA_CHANn_TypeDef chan);

/*! \typedef ADI_I2C_DMA_HANDLER_FPTR
 * The pointer to DMA handler function. */
typedef void (*ADI_I2C_DMA_HANDLER_FPTR)(ADI_I2C_DEV_HANDLE const hDevice, DMA_CHANn_TypeDef channelNum);

/*! \struct ADI_I2C_FTAB
 * The function pointer table. */
typedef struct {
    ADI_I2C_INIT_FPTR         pfInitFcn;              /*!< The initialize function pointer. */
    ADI_I2C_UNINIT_FPTR       pfUnInitFcn;            /*!< The un-initialize function pointer. */
    ADI_I2C_INIT_DESCRS_FPTR  pfInitDescriptorsFcn;   /*!< The initialize descriptors function pointer. */
    ADI_I2C_DMA_HANDLER_FPTR  pfDmaHandlerFcn;        /*!< The DMA handler function pointer. */
} ADI_I2C_FTAB;


/* defines... */

#define COMMON_MBITS I2CMCON_MASEN | I2CMCON_IENALOST | I2CMCON_IENNACK | I2CMCON_IENCMP                /*!< Common Master Config Bits */
#define COMMON_SBITS I2CSCON_SLVEN | I2CSCON_GCEN | I2CSCON_HGCEN | I2CSCON_IENSTOP | I2CSCON_IENREPST  /*!< Common Slave Config Bits */

#define MASTER_BUSY (hDevice->pDev->I2CMSTA & I2CMSTA_BUSY)  /*!< Master-Mode internal state machine idle test */
#define SLAVE_BUSY  (hDevice->pDev->I2CSSTA & I2CSSTA_BUSY)   /*!< Slave-Mode internal state machine idle test */

/* extended read count */
#define EXT_COUNT    256

/* file statics... */

#if (ADI_I2C_CFG_ENABLE_DMA_SUPPORT == 1)
/* single, shared DMA descriptor block */
static ADI_DMA_TRANSFER_TYPE gDmaDescriptor;
#endif /* ADI_I2C_CFG_ENABLE_DMA_SUPPORT */

/*! \struct ADI_I2C_DEV_DATA_TYPE
 * The I2C device instance data structure. */
typedef struct ADI_I2C_DEV_DATA_TYPE
{
    ADI_I2C_INIT_STATE         initState;             /*!< The device initialization state.   */
    ADI_I2C_DEV_ID_TYPE        devID;
    ADI_I2C_TypeDef           *pDev;
    ADI_I2C_RESULT_TYPE        result;                /*!< The transaction result.            */

    bool_t                     bBlockingMode;         /*!< The blocking-mode flag.            */

	/* resources for RTOS-based semaphore pend/post blocking vs. non-RTOS low-power blocking  */
#if (ADI_CFG_ENABLE_RTOS_SUPPORT == 1)
    /* OSAL */
    ADI_OSAL_SEM_HANDLE		   hSem;                  /*! Handle to semaphore                 */
    uint8_t                    SemMemory[ADI_OSAL_MAX_SEM_SIZE_CHAR];  /*! Semaphore Memory   */
#else 
    bool_t                     bLowPowerExitFlag;     /*! Exit flag for low power mode        */
	/* no wait object available if non-RTOS and not 350... */
#endif /* ADI_CFG_ENABLE_RTOS_SUPPORT  */

    bool_t                     bMasterReadBit;        /*!< The read/write flag.               */
    bool_t                     bClockStretch;         /*!< The flag for slave-mode ClockStretch.  */
    bool_t                     bRepeatStart;          /*!< The flag for master-mode RepeatStart.  */
    uint16_t                   firstMasterAddressByte;/*!< Data used to initiate repeated start.  */
    bool_t                     bTenBitSlaveAddress;   /*!< The 10-bit addressing mode flag.       */
    uint16_t                   slaveID;               /*!< The slave device ID.               */
    bool_t                     bGeneralCallPending;   /*!< The general call command pending flag. */
    uint8_t                    generalCallCommand;    /*!< The most recient general call command. */

    uint8_t                   *pTxBuffer;             /*!< The next transmit byte pointer.    */
    volatile uint16_t          TxRemaining;           /*!< The remaining transmit byte count. */
    uint8_t                   *pRxBuffer;             /*!< The next receive byte pointer.     */
    volatile uint16_t          RxRemaining;           /*!< The remaining receive byte count.  */

    uint32_t                   dataAddress;           /*!< The data/register transaction address.  */
    volatile
    ADI_I2C_DATA_ADDRESS_WIDTH dataAddressWidth;      /*!< The remaining address byte count.  */
    volatile bool_t            bComplete;             /*!< The transfer completion flag.      */

    /* function table */
    ADI_I2C_FTAB               FunctionTable;         /*!< The function pointer table.        */

    IRQn_Type                  PioMasterIrqNum;
    IRQn_Type                  PioSlaveIrqNum;

#if (ADI_I2C_CFG_ENABLE_CALLBACK_SUPPORT==1)
    /* Callback and Callback parameters */
    ADI_CALLBACK               pfCallback;
    void *                     pcbParam;
#endif /* ADI_I2C_CFG_ENABLE_CALLBACK_SUPPORT */

#if (ADI_I2C_CFG_ENABLE_DMA_SUPPORT == 1)
    /* DMA-related... */
	bool_t                     bDmaMode;              /*!< The DMA-Mode flag.                 */
    volatile bool_t            bDmaComplete;          /*!< The DMA completion flag. */
    ADI_DMA_TRANSFER_TYPE*     pDmaDescriptor;        /*!< The DMA transfer descriptor. */
    ADI_DMA_CCD_TYPE           nextDmaDescrType;      /*!< The DMA channel control data structure. */
    uint8_t*                   pNextDmaAddress;       /*!< The pointer to the next DMA address. */
    uint32_t                   nUnscheduledDmaCount;  /*!< The DMA data count. */
    DMA_CHANn_TypeDef          DmaMasterTxChannelNum;
    DMA_CHANn_TypeDef          DmaMasterRxChannelNum;
    DMA_CHANn_TypeDef          DmaSlaveTxChannelNum;
    DMA_CHANn_TypeDef          DmaSlaveRxChannelNum;
    bool_t                     bMasterTxRxDmaSharingFlag; /*!< Master-mode tx/rx DMA channel sharing flag. */
    IRQn_Type                  DmaMasterIrqNum;
    IRQn_Type                  DmaSlaveTxIrqNum;
    IRQn_Type                  DmaSlaveRxIrqNum;
#endif /* ADI_I2C_CFG_ENABLE_DMA_SUPPORT */

} ADI_I2C_DEV_DATA_TYPE;  /*!< The I2C device instance data structure. */

/* allocate instance data... */
static ADI_I2C_DEV_DATA_TYPE I2C_DevData[ADI_I2C_MAX_DEVID];


#if (ADI_I2C_CFG_ENABLE_STATIC_CONFIG_SUPPORT == 1)
/*!
 *****************************************************************************
 * I2C Configuration structure.
 *****************************************************************************/
typedef struct ADI_I2C_CONFIG
{
    uint16_t I2C_MasterControlReg;      /*!< I2C Master Control register init value. */
    uint16_t I2C_ClockDivideReg;        /*!< I2C Clock divider register init value.  */
    uint16_t I2C_SlaveControlReg;       /*!< I2C Slave Control register init value.  */
} ADI_I2C_CFG_TYPE;

/* I2C Application configuration array */
static const ADI_I2C_CFG_TYPE gI2CStaticInitData =
{
	/**** I2C_MasterControlReg configuration *** */
	( ( I2C_CFG_MASTER_PRESTOP_BUS_CLEAR      <<  BITP_I2C_I2CMCON_PRESTOP_BUS_CLR ) |
	  ( I2C_CFG_MASTER_BUS_CLEAR_ENABLE       <<  BITP_I2C_I2CMCON_BUS_CLR_EN      ) |
	  ( I2C_CFG_MASTER_TX_DMA_REQUESTS        <<  BITP_I2C_I2CMCON_MTXDMA          ) |
	  ( I2C_CFG_MASTER_RX_DMA_REQUESTS        <<  BITP_I2C_I2CMCON_MRXDMA          ) |
	  ( I2C_CFG_MASTER_TX_DECREMENT_MODE      <<  BITP_I2C_I2CMCON_MXMITDEC        ) |
	  ( I2C_CFG_MASTER_IRQ_ENABLE_STOP        <<  BITP_I2C_I2CMCON_IENCMP          ) |
	  ( I2C_CFG_MASTER_IRQ_ENABLE_NACK        <<  BITP_I2C_I2CMCON_IENACK          ) |
	  ( I2C_CFG_MASTER_IRQ_ENABLE_ALOST       <<  BITP_I2C_I2CMCON_IENALOST        ) |
	  ( I2C_CFG_MASTER_IRQ_ENABLE_TX_REQ      <<  BITP_I2C_I2CMCON_IENMTX          ) |
	  ( I2C_CFG_MASTER_IRQ_ENABLE_RX_REQ      <<  BITP_I2C_I2CMCON_IENMRX          ) |
	  ( I2C_CFG_MASTER_STRETCH                <<  BITP_I2C_I2CMCON_STRETCH         ) |
	  ( I2C_CFG_MASTER_LOOPBACK               <<  BITP_I2C_I2CMCON_LOOPBACK        ) |
	  ( I2C_CFG_MASTER_COMPETE                <<  BITP_I2C_I2CMCON_COMPLETE        ) |
	  ( I2C_CFG_MASTER_ENABLE                 <<  BITP_I2C_I2CMCON_MASEN           ) ),

	/**** I2C_ClockDivideReg configuration *** */
	( ( I2C_CFG_CLOCK_DIV_HIGH                <<  BITP_I2C_I2CDIV_HIGH             ) |
	  ( I2C_CFG_CLOCK_DIV_LOW                 <<  BITP_I2C_I2CDIV_LOW              ) ),

	/**** I2C_SlaveControlReg configuration *** */
	( ( I2C_CFG_SLAVE_TX_DMA_REQUESTS         <<  BITP_I2C_I2CSCON_STXDMA          ) |
	  ( I2C_CFG_SLAVE_RX_DMA_REQUESTS         <<  BITP_I2C_I2CSCON_SRXDMA          ) |
	  ( I2C_CFG_SLAVE_IRQ_REPEAT_START        <<  BITP_I2C_I2CSCON_IENREPST        ) |
	  ( I2C_CFG_SLAVE_TX_DECREMENT_MODE       <<  BITP_I2C_I2CSCON_SXMITDEC        ) |
	  ( I2C_CFG_SLAVE_IRQ_ENABLE_TX_REQ       <<  BITP_I2C_I2CSCON_IENSTX          ) |
	  ( I2C_CFG_SLAVE_IRQ_ENABLE_RX_REQ       <<  BITP_I2C_I2CSCON_IENSRX          ) |
	  ( I2C_CFG_SLAVE_IRQ_ENABLE_STOP         <<  BITP_I2C_I2CSCON_IENSTOP         ) |
	  ( I2C_CFG_SLAVE_FORCE_NACK              <<  BITP_I2C_I2CSCON_NACK            ) |
	  ( I2C_CFG_SLAVE_STRETCH                 <<  BITP_I2C_I2CSCON_STRETCHSCL      ) |
	  ( I2C_CFG_SLAVE_EARLY_TX_REQ            <<  BITP_I2C_I2CSCON_EARLYTXR        ) |
	  ( I2C_CFG_SLAVE_GENERAL_CALL_CLEAR      <<  BITP_I2C_I2CSCON_GCSBCLR         ) |
	  ( I2C_CFG_SLAVE_RX_DMA_REQUESTS         <<  BITP_I2C_I2CSCON_SRXDMA          ) |
	  ( I2C_CFG_SLAVE_HW_GENERAL_CALL_ENABLE  <<  BITP_I2C_I2CSCON_HGCEN           ) |
	  ( I2C_CFG_SLAVE_SW_GENERAL_CALL_ENABLE  <<  BITP_I2C_I2CSCON_GCEN            ) |
	  ( I2C_CFG_SLAVE_10_BIT_ADDRESS_ENABLE   <<  BITP_I2C_I2CSCON_ADR10EN         ) |
	  ( I2C_CFG_SLAVE_ENABLE                  <<  BITP_I2C_I2CSCON_SLVEN           ) )
};
#endif /* ADI_I2C_CFG_ENABLE_STATIC_CONFIG_SUPPORT */


/* device data pointers */
static ADI_I2C_DEV_DATA_TYPE* const pI2C0_DevData = &I2C_DevData[0];

/* forward declarations... */

/* Internal API... */
static ADI_I2C_RESULT_TYPE commonInit             (ADI_I2C_DEV_HANDLE const hDevice);
static void                configureNVIC          (ADI_I2C_DEV_HANDLE const hDevice, bool_t bEnable);
static void                CommenceMasterTransfer (ADI_I2C_DEV_HANDLE const hDevice);
static void                programSlaveAddress    (ADI_I2C_DEV_HANDLE const hDevice);
static void                prefillTx              (ADI_I2C_DEV_HANDLE const hDevice);

/* Master Event Handlers... */
static __INLINE void onMasterRXREQ    (ADI_I2C_DEV_HANDLE const hDevice);
static __INLINE void onMasterTXREQ    (ADI_I2C_DEV_HANDLE const hDevice);
static __INLINE void onMasterCOMP     (ADI_I2C_DEV_HANDLE const hDevice);
static __INLINE void onMasterNACKDATA (ADI_I2C_DEV_HANDLE const hDevice);
static __INLINE void onMasterALOST    (ADI_I2C_DEV_HANDLE const hDevice);
static __INLINE void onMasterNACKADDR (ADI_I2C_DEV_HANDLE const hDevice);


#if (ADI_I2C_CFG_SLAVE_MODE_SUPPORT == 1)
/* Slave Event Handlers... */
static __INLINE void onSlaveRXREQ     ();
static __INLINE void onSlaveTXREQ     ();
static __INLINE void onSlaveREPSTART  ();
static __INLINE void onSlaveSTOP      ();
static __INLINE void onSlaveGCINT     ();
#endif /* ADI_I2C_CFG_SLAVE_MODE_SUPPORT */


#if (ADI_I2C_CFG_ENABLE_DMA_SUPPORT == 1)
/* DMA support... */
static ADI_I2C_RESULT_TYPE scheduleNextDescriptor   (ADI_I2C_DEV_HANDLE const hDevice);
#endif /* ADI_I2C_CFG_ENABLE_DMA_SUPPORT */

/* pointer table functions... */
static ADI_I2C_RESULT_TYPE fPioInit                  (ADI_I2C_DEV_HANDLE const hDevice);
static ADI_I2C_RESULT_TYPE fPioInitializeDescriptors (ADI_I2C_DEV_HANDLE const hDevice, DMA_CHANn_TypeDef chan);
static ADI_I2C_RESULT_TYPE fPioUnInit                (ADI_I2C_DEV_HANDLE const hDevice);
static void                fPioCommonHandler         (ADI_I2C_DEV_HANDLE const hDevice, DMA_CHANn_TypeDef chan);

#if (ADI_I2C_CFG_ENABLE_DMA_SUPPORT == 1)
static ADI_I2C_RESULT_TYPE fDmaInit                  (ADI_I2C_DEV_HANDLE const hDevice);
static ADI_I2C_RESULT_TYPE fDmaInitializeDescriptors (ADI_I2C_DEV_HANDLE const hDevice, DMA_CHANn_TypeDef chan);
static ADI_I2C_RESULT_TYPE fDmaUnInit                (ADI_I2C_DEV_HANDLE const hDevice);
static void                fDmaCommonHandler         (ADI_I2C_DEV_HANDLE const hDevice, DMA_CHANn_TypeDef chan);
#endif /* ADI_I2C_CFG_ENABLE_DMA_SUPPORT */


/*
    //////////////////////////////////////////////////////////////////////////////
    //////////////////////   API IMPLEMENTATIONS   ///////////////////////////////
    //////////////////////////////////////////////////////////////////////////////
*/


/*!
 * @brief  Initialize and allocate the I2C device for Master-Mode operation.
 *
 * @param[in]        devID      Zero-based device index designating which device to initialize.
 * @param[out]       phDevice   The caller's device handle pointer for storing the initialized device instance data.
 *
 * @return            Status
 *                    - #ADI_I2C_ERR_ALREADY_INITIALIZED [D]    Specified device has already been initialized.
 *                    - #ADI_I2C_ERR_BAD_DEVICE_INDEX [D]       Invalid device index.
 *                    - #ADI_I2C_ERR_BUSY [D]                   Controller is busy; try again...
 *                    - #ADI_I2C_ERR_BAD_GPIO_CONFIG            Internal GPIO initializations failed.
 *                    - #ADI_I2C_ERR_BAD_DMA_CONFIG             DMA channel failed to initialize
 *                    - #ADI_I2C_SUCCESS                        Call completed successfully.
 *
 * Initialize an I2C device for Master-Mode operation using internal configuration settings and allocate the device for use.
 * The caller's device handle is written with the device instance data pointer.
 * The returned device handle is required to be passed to all subsequent APIs to convey which device instance to operate on.
 *
 * @note    The contents of phDevice will be set to NULL on failure.
 *
 * @sa      adi_I2C_SlaveInit()
 * @sa      adi_I2C_UnInit()
 */
ADI_I2C_RESULT_TYPE adi_I2C_MasterInit(ADI_I2C_DEV_ID_TYPE const devID, ADI_I2C_DEV_HANDLE* const phDevice) {

    ADI_I2C_DEV_HANDLE hDevice;
    ADI_I2C_RESULT_TYPE result;

    /* set local device handle */
    hDevice = &I2C_DevData[devID];
    hDevice->devID = devID;

#ifdef ADI_DEBUG
    if (devID >= ADI_I2C_MAX_DEVID)
        return ADI_I2C_ERR_BAD_DEVICE_INDEX;

    if (ADI_I2C_STATE_UNINITIALIZED != hDevice->initState)
        return ADI_I2C_ERR_ALREADY_INITIALIZED;
	if ((pADI_I2C->I2CSSTA & I2CSSTA_BUSY))
		return ADI_I2C_ERR_BUSY;
#endif /*  ADI_DEBUG */


    /* store a bad handle in case of failure */
    *phDevice = NULL;

    /* do common init */
    result = commonInit(hDevice);
    if (ADI_I2C_SUCCESS != result)
        return result;

#if (ADI_I2C_CFG_ENABLE_STATIC_CONFIG_SUPPORT == 1)

	/* assert static initializers */
	hDevice->pDev->I2CMCON = gI2CStaticInitData.I2C_MasterControlReg;
	hDevice->pDev->I2CDIV  = gI2CStaticInitData.I2C_ClockDivideReg;
	hDevice->pDev->I2CSCON = gI2CStaticInitData.I2C_SlaveControlReg;

#endif

    /* init successful */
    hDevice->initState = ADI_I2C_STATE_MASTER_MODE;

    /* store the "device" handle last */
    *phDevice = hDevice;
    return ADI_I2C_SUCCESS;
}


#if (ADI_I2C_CFG_SLAVE_MODE_SUPPORT == 1)
/*!
 * @brief  Initialize and allocate the I2C device for Slave-Mode operation.
 *
 * @param[in]        devID      Zero-based device index designating which device to initialize.
 * @param[out]       phDevice   The caller's device handle pointer for storing the initialized device instance data.
 *
 * @return            Status
 *                    - #ADI_I2C_ERR_ALREADY_INITIALIZED [D]    Specified device has already been initialized.
 *                    - #ADI_I2C_ERR_BAD_DEVICE_INDEX [D]       Invalid device index.
 *                    - #ADI_I2C_ERR_BUSY [D]                   Controller is busy; try again...
 *                    - #ADI_I2C_ERR_BAD_GPIO_CONFIG            Internal GPIO initializations failed.
 *                    - #ADI_I2C_ERR_BAD_DMA_CONFIG             DMA channel failed to initialize
 *                    - #ADI_I2C_SUCCESS                        Call completed successfully.
 *
 * Initialize an I2C device for Slave-Mode operation using internal configuration settings and allocate the device for use.
 * The caller's device handle is written with the device instance data pointer.
 * The returned device handle is required to be passed to all subsequent APIs to convey which device instance to operate on.
 *
 * @note    The contents of phDevice are set to NULL on failure.
 *
 * @sa      adi_I2C_SlaveInit()
 * @sa      adi_I2C_UnInit()
 */
ADI_I2C_RESULT_TYPE adi_I2C_SlaveInit(ADI_I2C_DEV_ID_TYPE const devID, ADI_I2C_DEV_HANDLE* const phDevice) {

    ADI_I2C_DEV_HANDLE hDevice;
    ADI_I2C_RESULT_TYPE result;

   /* set local device handle */
    hDevice = &I2C_DevData[devID];
    hDevice->devID = devID;

#ifdef ADI_DEBUG
    if (devID >= ADI_I2C_MAX_DEVID)
        return ADI_I2C_ERR_BAD_DEVICE_INDEX;

    if (ADI_I2C_STATE_UNINITIALIZED != hDevice->initState)
        return ADI_I2C_ERR_ALREADY_INITIALIZED;

    if ((pADI_I2C->I2CMSTA & I2CMSTA_BUSY))
        return ADI_I2C_ERR_BUSY;
#endif /* ADI_DEBUG */


    /* store a bad handle in case of failure */
    *phDevice = NULL;

    /* do common init */
    result = commonInit(hDevice);
    if (ADI_I2C_SUCCESS != result)
        return result;

#if (ADI_I2C_CFG_ENABLE_STATIC_CONFIG_SUPPORT == 1)

	/* assert static initializers */
	hDevice->pDev->I2CMCON = gI2CStaticInitData.I2C_MasterControlReg;
	hDevice->pDev->I2CDIV  = gI2CStaticInitData.I2C_ClockDivideReg;
	hDevice->pDev->I2CSCON = gI2CStaticInitData.I2C_SlaveControlReg;

#endif

#ifdef ADI_DEBUG
    if (SLAVE_BUSY)
        return ADI_I2C_ERR_BUSY;
#endif

    /* init successful */
    hDevice->initState = ADI_I2C_STATE_SLAVE_MODE;

    /* store the "device" handle last */
    *phDevice = hDevice;

    return ADI_I2C_SUCCESS;
}
#endif /* ADI_I2C_CFG_SLAVE_MODE_SUPPORT */



/*!
 * @brief  Uninitialize and deallocate the I2C device.
 *
 * @param[in]   hDevice    Device handle obtained from adi_I2C_MasterInit() or adi_I2C_SlaveInit().
 *
 * @return      Status
 *                - #ADI_I2C_ERR_BAD_DEVICE_HANDLE [D]  Invalid device handle parameter.
 *                - #ADI_I2C_ERR_NOT_INITIALIZED [D]    Device has not been previously configured for use.
 *                - #ADI_I2C_ERR_BUSY [D]               Controller is busy; try again...
 *                - #ADI_I2C_ERR_BAD_DMA_CONFIG         DMA channel failed to uninitialize
 *                - #ADI_I2C_SUCCESS                    Call completed successfully.
 *
 * Uninitialize and release an allocated I2C device for other use.
 *
 * @sa        adi_I2C_Init()
 */
ADI_I2C_RESULT_TYPE adi_I2C_UnInit(ADI_I2C_DEV_HANDLE const hDevice) {

#ifdef ADI_DEBUG
    if (ADI_I2C_INVALID_HANDLE(hDevice))
        return ADI_I2C_ERR_BAD_DEVICE_HANDLE;

    if (ADI_I2C_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_I2C_ERR_NOT_INITIALIZED;

    /* protect pending transaction that may not be fully serialized yet */
    if (   (hDevice->initState == ADI_I2C_STATE_MASTER_MODE && MASTER_BUSY)
#if (ADI_I2C_CFG_SLAVE_MODE_SUPPORT == 1)
        || (hDevice->initState == ADI_I2C_STATE_SLAVE_MODE && SLAVE_BUSY)
#endif /* ADI_I2C_CFG_SLAVE_MODE_SUPPORT */
        )
        return ADI_I2C_ERR_BUSY;
#endif

    /* disable bus error capture */
    ADI_DISABLE_INT(DMA_ERR_IRQn);

    /* disable NVIC */
    configureNVIC(hDevice, false);

    /* reset I2C control registers */
    hDevice->pDev->I2CMCON = I2CMSTA_RVAL;
    hDevice->pDev->I2CSCON = I2CSSTA_RVAL;

#if (ADI_CFG_ENABLE_RTOS_SUPPORT == 1)
     /* destroy semaphore */
     if (adi_osal_SemDestroyStatic(hDevice->hSem) != ADI_OSAL_SUCCESS)
     {
        return ADI_I2C_ERR_SEMAPHORE_FAILED;
     }
#endif /* ADI_CFG_ENABLE_RTOS_SUPPORT */

    /* call uninitialize helper */
    if (ADI_I2C_SUCCESS != hDevice->FunctionTable.pfUnInitFcn(hDevice))
        return ADI_I2C_ERR_BAD_DMA_CONFIG;

    hDevice->initState = ADI_I2C_STATE_UNINITIALIZED;

    return ADI_I2C_SUCCESS;
}


/*!
 * @brief  Configure the slave device address width for the I2C device.
 *
 * @param[in]   hDevice    Device handle obtained from adi_I2C_MasterInit() or adi_I2C_SlaveInit().
 * @param[in]   width      Configures the width of the slave-mode device address.
 *
 * @return      Status
 *                - #ADI_I2C_ERR_BAD_DEVICE_HANDLE [D]  Invalid device handle parameter.
 *                - #ADI_I2C_ERR_NOT_INITIALIZED [D]    Device has not been previously configured for use.
 *                - #ADI_I2C_ERR_BAD_DEVICE_WIDTH       Invalid slave address width specified
 *                - #ADI_I2C_SUCCESS                    Call completed successfully.
 *
 * Configures the I2C device driver for either 7-bit or 10-bit slave device address width to match the needs
 * of the external I2C master device.  I2C transactions require matching slave address and width on both ends.
 *
 */
ADI_I2C_RESULT_TYPE adi_I2C_SetSlaveDeviceIdWidth (ADI_I2C_DEV_HANDLE const hDevice, ADI_I2C_SLAVE_ID_WIDTH const width) {

#ifdef ADI_DEBUG
   if (ADI_I2C_INVALID_HANDLE(hDevice))
        return ADI_I2C_ERR_BAD_DEVICE_HANDLE;

    if (ADI_I2C_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_I2C_ERR_NOT_INITIALIZED;
#endif

    switch (width) {
        case ADI_I2C_SEVEN_BIT_SLAVE_ID:
            hDevice->bTenBitSlaveAddress = false;
            break;
        case ADI_I2C_TEN_BIT_SLAVE_ID:
            hDevice->bTenBitSlaveAddress = true;
            break;
        default:
          return ADI_I2C_ERR_BAD_DEVICE_WIDTH;
    }
    return ADI_I2C_SUCCESS;
}


/*!
 * @brief  Configure the serial bitrate for I2C Master-Mode operation.
 *
 * @param[in]   hDevice    Device handle obtained from adi_I2C_MasterInit().
 * @param[in]   sclk       Designates the desired Master-Mode serial bit rate (Hz).
 *
 * @return      Status
 *                - #ADI_I2C_ERR_BAD_DEVICE_HANDLE [D]  Invalid device handle parameter.
 *                - #ADI_I2C_ERR_NOT_INITIALIZED [D]    Device has not been previously configured for use.
 *                - #ADI_I2C_ERR_BAD_API_FOR_MODE [D]   Device is not configured for Master-Mode operation.
 *                - #ADI_I2C_ERR_BUSY [D]               Controller is busy; try again...
 *                - #ADI_I2C_ERR_BAD_CLOCK_RATE         Unrealizable clock rate specified.
 *                - #ADI_I2C_SUCCESS                    Call completed successfully.
 *
 * Configures the I2C Master-Mode device driver serial clock rate dividers.  Supports the two
 * standard I2C serial bit rates (100kHz and 400kHz), but also allows other bit rates which may
 * be helpful when running slower core clocks (1MHz), which would otherwise preclude I2C operation
 * (at the standard rates).  This API does not pertain to Slave-Mode transactions,
 * as Slave-Mode transactions are externally clocked by the Master-Mode device.
 *
 * @note The system-clock-to-I2C-serial-bit-rate ratio is verified to insure
 * the desired serial bit rate is physically realizable.  This API returns
 * ADI_I2C_ERR_BAD_CLOCK_RATE if clock ratio is not sufficient.
 *
 * @sa        adi_I2C_MasterTransmit()
 * @sa        adi_I2C_MasterReceive()
 */
ADI_I2C_RESULT_TYPE adi_I2C_SetMasterClock (ADI_I2C_DEV_HANDLE const hDevice, uint32_t const sclk) {

    uint32_t incoming_clock;
    uint8_t  ratio;

#ifdef ADI_DEBUG
   if (ADI_I2C_INVALID_HANDLE(hDevice))
        return ADI_I2C_ERR_BAD_DEVICE_HANDLE;

    if (ADI_I2C_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_I2C_ERR_NOT_INITIALIZED;

    if (ADI_I2C_STATE_MASTER_MODE != hDevice->initState)
        return ADI_I2C_ERR_BAD_API_FOR_MODE;

    if (MASTER_BUSY)
        return ADI_I2C_ERR_BUSY;
#endif

    // 'M350/360/320 have dedicated system clocks for I2C
    incoming_clock = SystemGetClockFrequency(ADI_SYS_CLOCK_I2C);

    /* compute the 1/2-cycle I2C clock ratio (NOTE strictly integer math) */
    ratio = (uint8_t) ((incoming_clock / sclk) / 2);

    /*
        Disallow physically unrealizable clock ratios (empirical).
        Sensitive to generated code, optimizations and compiler efficiency.
    */
    if (ratio < 12)
        return (ADI_I2C_ERR_BAD_CLOCK_RATE);

    /* and map it to the two 8-bit HIGH and LOW fields       */
    /* of the clock divider, yielding a symmetric clock      */
    /* NOTE, this equation differs slightly from the manual, */
    /* but yields a more accurate and symmetric clock        */
    hDevice->pDev->I2CDIV = ((ratio-3) << 8) | (ratio-1);

    return ADI_I2C_SUCCESS;
}


/*!
 * @brief  Set the I2C device transaction blocking mode.
 *
 * @param[in]   hDevice       Device handle obtained from adi_I2C_MasterInit() or adi_I2C_SlaveInit().
 * @param[in]    bFlag        Flag to select between blocking and non-blocking modes.
 *                - true:      Use blocking-mode.
 *                - false:     Use non-blocking-mode.
 *
 * @return         Status
 *                - #ADI_I2C_ERR_BAD_DEVICE_HANDLE [D]  Invalid device handle parameter.
 *                - #ADI_I2C_ERR_NOT_INITIALIZED [D]    Device has not been previously configured for use.
 *                - #ADI_I2C_ERR_BAD_MODE_COMBO [D]     Blocking-Mode is incompatible with DMA-Mode.
 *                - #ADI_I2C_SUCCESS                    Call completed successfully.
 *
 * Sets the blocking mode for data transfers.  Transfers made in blocking-mode will not return until complete.
 * Transfers made in non-blocking-mode may return before the transfer is complete.  Applications must use
 * adi_I2C_GetNonBlockingStatus() to ascertain the completion status of non-blocking transactions.
 *
 * DMA is a non-blocking use-case.  DMA in combination with Blocking-Mode is incompatible.
 * If you get result code #ADI_I2C_ERR_BAD_MODE_COMBO, disable either Blocking-Mode or DMA-Mode.
 *
 * @sa        adi_I2C_GetNonBlockingStatus()
 * @sa        adi_I2C_SetDmaMode()
 */
ADI_I2C_RESULT_TYPE adi_I2C_SetBlockingMode (ADI_I2C_DEV_HANDLE const hDevice, bool_t const bFlag) {

#ifdef ADI_DEBUG
    if (ADI_I2C_INVALID_HANDLE(hDevice))
        return ADI_I2C_ERR_BAD_DEVICE_HANDLE;

    if (ADI_I2C_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_I2C_ERR_NOT_INITIALIZED;

#if (ADI_I2C_CFG_ENABLE_DMA_SUPPORT == 1)
    /* blocking mode with DMA disallowed... */
    if (hDevice->bDmaMode & bFlag)
        return ADI_I2C_ERR_BAD_MODE_COMBO;
#endif /* ADI_I2C_CFG_ENABLE_DMA_SUPPORT */

#endif /* debug */

    hDevice->bBlockingMode = bFlag;

    return ADI_I2C_SUCCESS;
}


/* NOTE: the former adi_I2C_SetMuxing() is deprecated and removed in lieu of required pinmux plugin configuration code */


/*!
 * @brief  Get the I2C device non-blocking status.
 *
 * @param[in]   hDevice            Device handle obtained from adi_I2C_MasterInit() or adi_I2C_SlaveInit().
 * @param[out]  pBytesRemaining    Pointer to an application variable to store the remaining transaction byte count.
 *
 * @return         Status
 *                - #ADI_I2C_ERR_BAD_DEVICE_HANDLE [D]    Invalid device handle parameter.
 *                - #ADI_I2C_ERR_NOT_INITIALIZED [D]      Device has not been previously configured for use.
 *                - #ADI_I2C_ERR_BLOCKING_ACTIVE [D]      Device is in Blocking-Mode.
 *                - #ADI_I2C_TRANSACTION_INCOMPLETE       Transaction underway, *pBytesRemaining is valid on exit.
 *                - #ADI_I2C_MASTER_ALOST                 Master-Mode transaction halted due to lost bus.
 *                - #ADI_I2C_MASTER_NACK_ADDRESS          Master-Mode transaction halted due to missed address ACK.
 *                - #ADI_I2C_MASTER_NACK_DATA             Master-Mode transaction halted due to missed data ACK.
 *                - #ADI_I2C_SLAVE_GENERAL_CALL           Slave-Mode transaction halted due to General Call command.
 *                - #ADI_I2C_SLAVE_REPEAT_START           Slave received a repeat start condition.
 *                - #ADI_I2C_SLAVE_RECEIVED_STOP          Slave received a stop condition.
 *                - #ADI_I2C_SUCCESS                      Transaction completed successfully.
 *
 * When the Non-Blocking mode is used, the main transaction APIs (master/slave transmit/receive) return immediately,
 * allowing the application to do other work.  During such transactions, the application may poll the driver with this API
 * to ascertain the progress of the transaction.  While the transaction is incomplete and no error has occurred, the API
 * returns #ADI_I2C_TRANSACTION_INCOMPLETE and the variable pointed to by the pBytesRemaining parameter is set with the
 * number of bytes remaining in the transfer.
 *
 * There are a number of error conditions that may halt the transaction prematurely due to various events
 * which the application must reconcile.  In the case of a Slave-Mode General Call command, the most recent
 * General Call command may be retrieved with the adi_I2C_GetGeneralCallCommand() API.  Other cases include
 * #ADI_I2C_SLAVE_REPEAT_START and #ADI_I2C_SLAVE_RECEIVED_STOP, etc.
 *
 * @sa        adi_I2C_SetBlockingMode()
 * @sa        adi_I2C_GetGeneralCallCommand()
 */
ADI_I2C_RESULT_TYPE adi_I2C_GetNonBlockingStatus (ADI_I2C_DEV_HANDLE const hDevice, uint16_t* const pBytesRemaining) {

#ifdef ADI_DEBUG
    if (ADI_I2C_INVALID_HANDLE(hDevice))
        return ADI_I2C_ERR_BAD_DEVICE_HANDLE;

    if (ADI_I2C_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_I2C_ERR_NOT_INITIALIZED;

    if (hDevice->bBlockingMode)
        return ADI_I2C_ERR_BLOCKING_ACTIVE;
#endif

    /* worst case (in support of loopback) */
    uint16_t remaining  = hDevice->TxRemaining;
             remaining += hDevice->RxRemaining;

    /* writeback the remaining count */
    *pBytesRemaining = remaining;

    /* done yet? */
    if (   (remaining)
        || ((hDevice->initState == ADI_I2C_STATE_MASTER_MODE) && MASTER_BUSY)
#if (ADI_I2C_CFG_SLAVE_MODE_SUPPORT == 1)
        || ((hDevice->initState == ADI_I2C_STATE_SLAVE_MODE) && SLAVE_BUSY)
#endif /* ADI_I2C_CFG_SLAVE_MODE_SUPPORT */
        )
    {
        return ADI_I2C_TRANSACTION_INCOMPLETE;
    }

    /* transaction complete, return result */
    return hDevice->result;
}


#if (ADI_I2C_CFG_ENABLE_DMA_SUPPORT == 1)
/*!
 * @brief  Set the I2C device transaction DMA-Mode.
 *
 * @param[in]   hDevice       Device handle obtained from adi_I2C_MasterInit() or adi_I2C_SlaveInit().
 * @param[in]   bFlag         Flag to select between DMA and Non-DMA modes.
 *                - true:     Use DMA-Mode transfers.
 *                - false:    Use Non-DMA-Mode transfers.
 *
 * @return         Status
 *                - #ADI_I2C_ERR_BAD_DEVICE_HANDLE [D]  Invalid device handle parameter.
 *                - #ADI_I2C_ERR_NOT_INITIALIZED [D]    Device has not been previously configured for use.
 *                - #ADI_I2C_ERR_BAD_MODE_COMBO [D]     Blocking-Mode is incompatible with DMA-Mode.
 *                - #ADI_I2C_SUCCESS                    Call completed successfully.
 *
 * Sets the DMA-Mode for data transfers.  Transfers made in DMA-Mode will off-load the core processor.
 * Requires use of adi_I2C_GetDmaCompletion() to ascertain when DMA-Mode transfers have completed.
 *
 * DMA is a non-blocking use-case.  DMA in combination with Blocking-Mode is incompatible.
 * If you get result code ADI_I2C_ERR_BAD_MODE_COMBO, disable either Blocking-Mode or DMA-Mode.
 *
 * @note      DMA mode is disabled by default.  This allows linker elimination to optimize DMA support
 *            code out of the I2C device driver, produceing a significient code size reduction.
 *            But if the application uses adi_I2C_SetDmaMode(), the linker can no longer eliminate
 *            references to the supporting DMA code.  Application programmers should avoid using
 *            adi_I2C_SetDmaMode() unless they really intend to operate the I2C controller in DMA mode.
 *            Even after using DMA and then disabeling it again, the DMA support code remains and
 *            the footprint reduction is lost.
 *
 * @sa        adi_I2C_GetDmaCompletion()
 * @sa        adi_I2C_SetBlockingMode()
 */
ADI_I2C_RESULT_TYPE adi_I2C_SetDmaMode (ADI_I2C_DEV_HANDLE const hDevice, const bool_t bFlag) {

#ifdef ADI_DEBUG
    if (ADI_I2C_INVALID_HANDLE(hDevice))
        return ADI_I2C_ERR_BAD_DEVICE_HANDLE;

    if (ADI_I2C_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_I2C_ERR_NOT_INITIALIZED;

    /* blocking mode with DMA disallowed... */
    if (hDevice->bBlockingMode & bFlag)
        return ADI_I2C_ERR_BAD_MODE_COMBO;
#endif

    /* call whichever uninitialize helper is currently active */
    if (ADI_I2C_SUCCESS != hDevice->FunctionTable.pfUnInitFcn(hDevice))
        return ADI_I2C_ERR_BAD_DMA_CONFIG;

    /* store flag */
    hDevice->bDmaMode = bFlag;

    /* remap function table */
    hDevice->FunctionTable.pfInitFcn            = (bFlag) ? &fDmaInit                  : &fPioInit;
    hDevice->FunctionTable.pfUnInitFcn          = (bFlag) ? &fDmaUnInit                : &fPioUnInit;
    hDevice->FunctionTable.pfInitDescriptorsFcn = (bFlag) ? &fDmaInitializeDescriptors : &fPioInitializeDescriptors;
    hDevice->FunctionTable.pfDmaHandlerFcn      = (bFlag) ? &fDmaCommonHandler         : &fPioCommonHandler;

	/* map the channels */
    hDevice->DmaMasterTxChannelNum = I2CM_CHANn;
    hDevice->DmaMasterRxChannelNum = I2CM_CHANn;

#if (ADI_I2C_CFG_SLAVE_MODE_SUPPORT == 1)
    hDevice->DmaSlaveTxChannelNum = I2CS_TX_CHANn;
    hDevice->DmaSlaveRxChannelNum = I2CS_RX_CHANn;
#endif /* ADI_I2C_CFG_SLAVE_MODE_SUPPORT */

    /* call the new initialize helper */
    if (ADI_I2C_SUCCESS != hDevice->FunctionTable.pfInitFcn(hDevice))
        return ADI_I2C_ERR_BAD_DMA_CONFIG;

    return ADI_I2C_SUCCESS;
}


/*!
 * @brief  Get the I2C device DMA-Mode transaction status.
 *
 * @param[in]      hDevice       Device handle obtained from adi_I2C_MasterInit() or adi_I2C_SlaveInit().
 *
 * @return Returns the completion state of the current DMA transfer.
 *                - true:      DMA is complete.
 *                - false:     DMA is incomplete.
 *
 * Polling call for application to query the DMA status for pending data transfers.  Current DMA status is
 * returned as a simple Boolean.
 *
 * This API is irrelevant in non-DMA mode.
 *
 * @note      Although this API indicates that the DMA is complete, the FIFOs may still
 *            contain data. Subsequent submits will always block until the FIFOs are empty.
 *            This approach minimizes FIFO blocking.
 *
 * @sa        adi_I2C_SetDmaMode()
 */
bool_t adi_I2C_GetDmaCompletion (ADI_I2C_DEV_HANDLE const hDevice) {
    return hDevice->bDmaComplete;
}
#endif /* ADI_I2C_CFG_ENABLE_DMA_SUPPORT */


/*!
 * @brief  Get the I2C device "General Call" command.
 *
 * @param[in]      hDevice   Device handle obtained from adi_I2C_SlaveInit().
 * @param[out]     pCommand  Pointer to an application variable to store the GC command.
 *
 * @return         Status
 *                - #ADI_I2C_ERR_BAD_DEVICE_HANDLE [D]  Invalid device handle parameter.
 *                - #ADI_I2C_ERR_NOT_INITIALIZED [D]    Device has not been previously configured for use.
 *                - #ADI_I2C_ERR_BAD_API_FOR_MODE [D]   Device is not configured for Slave-Mode operation.
 *                - #ADI_I2C_SUCCESS                    Transaction completed successfully.
 *
 * Returns the most-recently received General Call (GC) Command.  Applies only to Slave-Mode operation.
 *
 * The GC protocol provides for an external Master-Mode device to broadcast a pre-emptive command to all slave
 * devices on the bus to do something urgent.
 *
 * GC commands are captured via an internal I2C Slave-Mode interrupt.
 * On receipt, the GC command is stored until this API retrieves it.
 *
 * Once a GC command is pending, all subsequent I2C Slave-Mode device driver APIs return the
 * #ADI_I2C_SLAVE_GENERAL_CALL result until this API is called, leaving the application to
 * reconcile the command.
 *
 * The I2C Slave-Mode device is not physically enabled unless a transaction is underway, so GC calls are
 * only recognized during actual Slave-Mode transactions.
 *
 * It is the application's responsibility to recognize the #ADI_I2C_SLAVE_GENERAL_CALL return value
 * from the slave transmit and receive calls and to then retrieve the GC command (through this API) and
 * take appropriate actions in service thereof.
 *
 * The I2C internal GC pending state is cleared as this API exits.
 */
ADI_I2C_RESULT_TYPE adi_I2C_GetGeneralCallCommand (ADI_I2C_DEV_HANDLE const hDevice, uint8_t* const pCommand) {

#ifdef ADI_DEBUG
    if (ADI_I2C_INVALID_HANDLE(hDevice))
        return ADI_I2C_ERR_BAD_DEVICE_HANDLE;

    if (ADI_I2C_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_I2C_ERR_NOT_INITIALIZED;

#if (ADI_I2C_CFG_SLAVE_MODE_SUPPORT == 1)
    if (ADI_I2C_STATE_SLAVE_MODE != hDevice->initState)
        return ADI_I2C_ERR_BAD_API_FOR_MODE;
#endif /* ADI_I2C_CFG_SLAVE_MODE_SUPPORT */

#endif /* debug */

    *pCommand = hDevice->generalCallCommand;
    hDevice->bGeneralCallPending = false;

    return ADI_I2C_SUCCESS;
}


/*!
 * @brief  Initiate an I2C Master-Mode transmit.
 *
 * @param[in]   hDevice               Device handle obtained from adi_I2C_MasterInit().
 * @param[in]   slaveID               External slave device address (7-bit or 10-bit).
 * @param[in]   dataAddress           External slave device register/memory data write address.
 * @param[in]   dataAddressWidth      External slave device register/memory data write address width.
 * @param[in]   pBuffer               Memory pointer to application's transmit data buffer.
 * @param[in]   nBufferSize           Number of data bytes to transmit.
 * @param[in]   bRepeatStart          Control issue of RepeatStart condition instead of usual Stop/Start sequence.
 *                - true:     Issue RepeatStart to transition the bus.
 *                - false:    Issue Stop/Start conditions to transition the bus.
 *
 * @return         Status
 *                - #ADI_I2C_ERR_BAD_DEVICE_HANDLE [D]  Invalid device handle parameter.
 *                - #ADI_I2C_ERR_NOT_INITIALIZED [D]    Device has not been previously configured for use.
 *                - #ADI_I2C_ERR_BAD_DATA_ADDRESS [D]   Target data address to large.
 *                - #ADI_I2C_ERR_BAD_API_FOR_MODE [D]   Device is not configured for Master-Mode operation.
 *                - #ADI_I2C_MASTER_ALOST               Master lost bus arbitration to another Master.
 *                - #ADI_I2C_MASTER_NACK_ADDRESS        External slave device failed to acknowledge address phase.
 *                - #ADI_I2C_MASTER_NACK_DATA           External slave device failed to acknowledge data phase.
 *                - #ADI_I2C_MASTER_STOP                Master detected an I2C stop condition.
 *                - #ADI_I2C_SUCCESS                    Transaction completed successfully.
 *
 * Initiates a Master-Mode transmit to write data to an external slave device designated by \a slaveID.
 * The \a slaveID is encoded in the control code transaction preamble using either 7-bit or 10-bit
 * slave addressing, according to the active \a slaveID width setting (see adi_I2C_SetSlaveDeviceIdWidth().
 * The read/write bit and data write address are also encoded.
 *
 * External slave device register/memory data write address, \a dataAddress, may be 8/12/16/32-bit.
 * Data addressing width, \a dataAddressWidth, is of type #ADI_I2C_DATA_ADDRESS_WIDTH.
 *
 * The transmit data is read from the memory buffer pointed to by \a pBuffer and sent to the receiving slave
 * device until the slave faults or \a nBufferSize is reached.
 *
 * Blocking-mode or non-blocking-mode is applied, as determined by adi_I2C_SetBlockingMode() (default is
 * blocking-mode enabled).  In blocking-mode, this API blocks until the transaction is complete.
 * In non-blocking-mode, this API returns immediately and the application must poll for transaction completion
 * with adi_I2C_GetNonBlockingStatus().
 *
 * The \a bRepeatStart parameter controls how the Master transitions the bus from the write cycle (addressing phase) to
 * a potentially following read cycle (data phase).  If set, a RepeatStart condition is sent which locks the bus.  If clear, a Stop condition
 * is sent (which releases the bus), followed by a fresh Start condition with the next transaction.
 *
 * Various transaction errors are possible and the application should test the return result and take appropriate steps.
 * Any of the #ADI_I2C_MASTER_ALOST, #ADI_I2C_MASTER_NACK_ADDRESS, #ADI_I2C_MASTER_NACK_DATA, or #ADI_I2C_MASTER_STOP
 * return codes are terminal conditions and the transaction is stopped.
 *
 * @sa        adi_I2C_SetSlaveDeviceIdWidth()
 * @sa        adi_I2C_SetDmaMode()
 * @sa        adi_I2C_SetBlockingMode()
 * @sa        adi_I2C_GetNonBlockingStatus()
 * @sa        adi_I2C_SetMasterClock()
 * @sa        adi_I2C_MasterReceive()
 */
ADI_I2C_RESULT_TYPE adi_I2C_MasterTransmit(ADI_I2C_DEV_HANDLE const hDevice,
                                                     uint16_t const slaveID,
                                                     uint32_t const dataAddress,
                                   ADI_I2C_DATA_ADDRESS_WIDTH const dataAddressWidth,
                                                      uint8_t      *pBuffer,
                                                     uint16_t const nBufferSize,
                                                       bool_t const bRepeatStart) {
#ifdef ADI_DEBUG
    if (ADI_I2C_INVALID_HANDLE(hDevice))
        return ADI_I2C_ERR_BAD_DEVICE_HANDLE;

    if (ADI_I2C_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_I2C_ERR_NOT_INITIALIZED;

    if ((ADI_I2C_8_BIT_DATA_ADDRESS_WIDTH  != dataAddressWidth)
     && (ADI_I2C_12_BIT_DATA_ADDRESS_WIDTH != dataAddressWidth)
     && (ADI_I2C_16_BIT_DATA_ADDRESS_WIDTH != dataAddressWidth)
     && (ADI_I2C_32_BIT_DATA_ADDRESS_WIDTH != dataAddressWidth))
        return ADI_I2C_ERR_BAD_DATA_ADDRESS;

    if (ADI_I2C_STATE_MASTER_MODE != hDevice->initState)
        return ADI_I2C_ERR_BAD_API_FOR_MODE;
#endif

    /* wait for any previous fifo content to fully serialize */
    while (MASTER_BUSY)
    	;

    /* store RepeatStart flag, to be asserted on 1st Master transmit */
    /* interrupt by rewriting the 1st device address byte register   */
    hDevice->bRepeatStart = bRepeatStart;

    /* setup the transaction */
    hDevice->bMasterReadBit   = false;
    hDevice->slaveID          = slaveID;
    hDevice->pTxBuffer        = pBuffer;
    hDevice->TxRemaining      = nBufferSize;
    hDevice->pRxBuffer        = NULL;
    hDevice->RxRemaining      = 0;
    hDevice->dataAddress      = dataAddress;
    hDevice->dataAddressWidth = dataAddressWidth;
    hDevice->result           = ADI_I2C_SUCCESS;

    /* flush the transmit FIFO */
	/* no need for critical section on register read-modify-write *\
	\* becuase this resource is not modified by any interrupt     */
    hDevice->pDev->I2CFSTA |= I2CFSTA_MFLUSH;
    hDevice->pDev->I2CFSTA &= ~I2CFSTA_MFLUSH;

    /* enable NVIC */
    configureNVIC(hDevice, true);

#if (ADI_I2C_CFG_ENABLE_DMA_SUPPORT == 1)
    if (hDevice->bDmaMode) {

        hDevice->bDmaComplete = false;  /* blocking is done at app level via adi_I2C_GetDmaCompletion() */

	/* set DMA channel sharing flag for 350 */
        hDevice->bMasterTxRxDmaSharingFlag = true;

        /* program initial descriptor pair for transmit */
        hDevice->FunctionTable.pfInitDescriptorsFcn(hDevice, hDevice->DmaMasterTxChannelNum);

        /* enable dma transmit */
        hDevice->pDev->I2CMCON = COMMON_MBITS | I2CMCON_TXDMA;

    }
    else
#endif /* ADI_I2C_CFG_ENABLE_DMA_SUPPORT */
    {

        /* enable interrupt transmit */
        hDevice->pDev->I2CMCON = COMMON_MBITS | I2CMCON_IENTX;

        /* prefill the FIFO */
        prefillTx(hDevice);
    }

    /* launch transfer */
    CommenceMasterTransfer(hDevice);

    /* honor blocking mode */
    if (true == hDevice->bBlockingMode) {

        /* wait for tcomp */
        BOOLEAN_WAIT (hDevice, bComplete);

		/* halt interrupts while changing transmit enable hardware bit */
		ADI_ENTER_CRITICAL_REGION();
        hDevice->pDev->I2CMCON &= ~I2CMCON_IENTX;
		ADI_EXIT_CRITICAL_REGION();

    }  /* end blocking mode */

    return hDevice->result;
}


/*!
 * @brief  Initiate an I2C Master-Mode receive.
 *
 * @param[in]   hDevice               Device handle obtained from adi_I2C_MasterInit().
 * @param[in]   slaveID               External slave device address (7-bit or 10-bit).
 * @param[in]   dataAddress           External slave device register/memory data read address.
 * @param[in]   dataAddressWidth      External slave device register/memory data read address width.
 * @param[in]   pBuffer               Memory pointer to application's receive data buffer.
 * @param[in]   nBufferSize           Number of data bytes to receive.
 * @param[in]   bRepeatStart          Control issue of RepeatStart condition instead of Stop/Start sequence.
 *                - true:     Issue RepeatStart to transition the bus.
 *                - false:    Issue Stop/Start conditions to transition the bus.
 *
 * @return         Status
 *                - #ADI_I2C_ERR_BAD_DEVICE_HANDLE [D]  Invalid device handle parameter.
 *                - #ADI_I2C_ERR_NOT_INITIALIZED [D]    Device has not been previously configured for use.
 *                - #ADI_I2C_ERR_BAD_DATA_ADDRESS [D]   Target data address to large.
 *                - #ADI_I2C_ERR_BAD_API_FOR_MODE [D]   Device is not configured for Master-Mode operation.
 *                - #ADI_I2C_MASTER_ALOST               Master lost bus arbitration to another Master.
 *                - #ADI_I2C_MASTER_NACK_ADDRESS        External slave device failed to acknowledge address phase.
 *                - #ADI_I2C_MASTER_NACK_DATA           External slave device failed to acknowledge data phase.
 *                - #ADI_I2C_MASTER_STOP                Master detected an I2C stop condition.
 *                - #ADI_I2C_SUCCESS                    Transaction completed successfully.
 *
 * Initiates a Master-Mode receive to read data from an external slave device designated by \a slaveID.
 * The \a slaveID is encoded in the control code transaction preamble using either 7-bit or 10-bit
 * slave addressing, according to the active \a slaveID width setting (see adi_I2C_SetSlaveDeviceIdWidth().
 * The read/write bit and data read address are also encoded.
 *
 * External slave device register/memory data read address, \a dataAddress, may be either 8/12/16/32-bit.
 * Data addressing width, \a dataAddressWidth, is of type #ADI_I2C_DATA_ADDRESS_WIDTH.
 *
 * The receive data is written to the memory buffer pointed to by \a pBuffer as read from the transmitting slave
 * device, until the slave faults or \a nBufferSize is reached.
 *
 * Blocking-mode or non-blocking-mode is applied, as determined by adi_I2C_SetBlockingMode() (default is
 * blocking-mode enabled).  In blocking-mode, this API blocks until the transaction is complete.
 * In non-blocking-mode, this API returns immediately and the application must poll for transaction completion
 * with adi_I2C_GetNonBlockingStatus().
 *
 * The \a bRepeatStart parameter controls how the Master transitions the bus from the write cycle (addressing phase) to
 * the read cycle (data phase).  If set, a RepeatStart condition is sent which locks the bus.  If clear, a Stop condition
 * is sent (which releases the bus), followed by a fresh Start condition with the next transaction.
 *
 * The Master-Mode serial clock rate is either 100kHz (standard) or 400kHz (high-speed), depending on mode selected
 * by most recient call to adi_I2C_SetMasterClock().
 *
 * Various transaction errors are possible and the application should test the return result and take appropriate steps.
 * Any of the #ADI_I2C_MASTER_ALOST, #ADI_I2C_MASTER_NACK_ADDRESS, #ADI_I2C_MASTER_NACK_DATA, or #ADI_I2C_MASTER_STOP
 * return codes are terminal conditions and the transaction is stopped.
 *
 * @sa        adi_I2C_SetSlaveDeviceIdWidth()
 * @sa        adi_I2C_SetDmaMode()
 * @sa        adi_I2C_SetBlockingMode()
 * @sa        adi_I2C_GetNonBlockingStatus()
 * @sa        adi_I2C_SetMasterClock()
 * @sa        adi_I2C_MasterTransmit()
 */
ADI_I2C_RESULT_TYPE adi_I2C_MasterReceive(ADI_I2C_DEV_HANDLE const hDevice,
                                                    uint16_t const slaveID,
                                                    uint32_t const dataAddress,
                                  ADI_I2C_DATA_ADDRESS_WIDTH const dataAddressWidth,
                                                     uint8_t      *pBuffer,
                                                    uint16_t const nBufferSize,
                                                      bool_t const bRepeatStart) {
    volatile uint16_t dummyRead;

#ifdef ADI_DEBUG
    if (ADI_I2C_INVALID_HANDLE(hDevice))
        return ADI_I2C_ERR_BAD_DEVICE_HANDLE;

    if (ADI_I2C_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_I2C_ERR_NOT_INITIALIZED;

    if (ADI_I2C_STATE_MASTER_MODE != hDevice->initState)
        return ADI_I2C_ERR_BAD_API_FOR_MODE;

    if ((ADI_I2C_8_BIT_DATA_ADDRESS_WIDTH  != dataAddressWidth)
     && (ADI_I2C_12_BIT_DATA_ADDRESS_WIDTH != dataAddressWidth)
     && (ADI_I2C_16_BIT_DATA_ADDRESS_WIDTH != dataAddressWidth)
     && (ADI_I2C_32_BIT_DATA_ADDRESS_WIDTH != dataAddressWidth))
        return ADI_I2C_ERR_BAD_DATA_ADDRESS;
#endif

    /* wait for any previous fifo content to fully serialize */
    while (MASTER_BUSY)
    	;

    /* store RepeatStart flag, to be asserted on 1st Master transmit */
    /* interrupt by rewriting the 1st device address byte register   */
    hDevice->bRepeatStart = bRepeatStart;

    /* "flush" receive FIFO by dummy reads (no explicit flush bits for MRX) */
    dummyRead = hDevice->pDev->I2CMRX;
    dummyRead = hDevice->pDev->I2CMRX;

    /* flush the transmit FIFO */
	/* no need for critical section on register read-modify-write *\
	\* becuase this resource is not modified by any interrupt     */
    hDevice->pDev->I2CFSTA |= I2CFSTA_MFLUSH;
    hDevice->pDev->I2CFSTA &= ~I2CFSTA_MFLUSH;

    /* config settings */
    hDevice->slaveID  = slaveID;
    hDevice->result   = ADI_I2C_SUCCESS;

    /* enable NVIC */
    configureNVIC(hDevice, true);

    /* common initializers */
    hDevice->pTxBuffer        = NULL;
    hDevice->TxRemaining      = 0;
    hDevice->pRxBuffer        = NULL;
    hDevice->RxRemaining      = 0;
    hDevice->dataAddress      = NULL;
    hDevice->dataAddressWidth = ADI_I2C_NO_DATA_ADDRESSING_PHASE;

#if (ADI_I2C_CFG_ENABLE_DMA_SUPPORT == 1)
    if (hDevice->bDmaMode) {

        /* dma mode */

        /* if we have an addressing phase... */
        if (dataAddressWidth) {

            /* schedule transmit descriptors for addressing phase */
            hDevice->dataAddress      = dataAddress;
            hDevice->dataAddressWidth = dataAddressWidth;

            /* set DMA channel sharing flag for 350 */
            hDevice->bMasterTxRxDmaSharingFlag = true;

            /* program initial descriptor pair for transmit (of address phase) */
            hDevice->FunctionTable.pfInitDescriptorsFcn(hDevice,  hDevice->DmaMasterTxChannelNum);

            /* launch addressing phase and wait... */
            hDevice->bMasterReadBit   = false;
            hDevice->bDmaComplete     = false;    /* blocking is done right here... */
            hDevice->pDev->I2CMCON    = COMMON_MBITS | I2CMCON_TXDMA;
            CommenceMasterTransfer(hDevice);

            BOOLEAN_WAIT(hDevice, bDmaComplete)

			/* hard wait on RS flag if active */
            if (!bRepeatStart) {
                while(MASTER_BUSY)
                	; /* wait for stop condition to arrive */
            } /* end RS delay */
        } /* end addressing phase */

        /* schedule receive descriptors for data phase */
        hDevice->pRxBuffer            = pBuffer;
        hDevice->RxRemaining          = nBufferSize;

	/* set DMA channel sharing flag for 350 */
        hDevice->bMasterTxRxDmaSharingFlag = false;

        /* program initial descriptor pair for receive (of data) */
        hDevice->FunctionTable.pfInitDescriptorsFcn(hDevice,  hDevice->DmaMasterRxChannelNum);

        /* program expected receive count, use extended read if greater than 256 bytes */
        if (nBufferSize > EXT_COUNT) {
          /* enable extended read */
          hDevice->pDev->I2CMRXCNT = I2CMRXCNT_EXTEND;
        }
        else
        {
            hDevice->pDev->I2CMRXCNT = nBufferSize - 1;
        }
        
         /* launch data phase... */
        hDevice->bMasterReadBit       = true;
        hDevice->bDmaComplete         = false;  /* blocking is done at app level, via adi_I2C_GetDmaCompletion() */
        hDevice->pDev->I2CMCON        = COMMON_MBITS | I2CMCON_RXDMA;
        CommenceMasterTransfer(hDevice);

    }
    else
#endif /* ADI_I2C_CFG_ENABLE_DMA_SUPPORT */
    {

        /* non-DMA mode */

        /* if we have an addressing phase... */
        if (dataAddressWidth) {

            /* addressing phase... */
            hDevice->bMasterReadBit   = false;
            hDevice->dataAddress      = dataAddress;
            hDevice->dataAddressWidth = dataAddressWidth;

            /* enable transmit */
            hDevice->pDev->I2CMCON = COMMON_MBITS | I2CMCON_IENTX;

            /* prefill the FIFO */
            prefillTx(hDevice);

            /* kick it off */
            CommenceMasterTransfer(hDevice);

			/* wait for tcomp */
        	BOOLEAN_WAIT (hDevice, bComplete);

			/* now wait on RS flag if active */
            if (!bRepeatStart) {
                while (MASTER_BUSY)
                	; /* wait for stop condition to arrive */
			} /* end if RS */
        } /* end addressing phase */

        /* data phase... */
        hDevice->bMasterReadBit       = true;
        hDevice->pRxBuffer            = pBuffer;
        hDevice->RxRemaining          = nBufferSize;

        /* program expected receive count, use extended read if greater than 256 bytes  */
        if (nBufferSize > EXT_COUNT) {
          /* enable extended read */
          hDevice->pDev->I2CMRXCNT = I2CMRXCNT_EXTEND;
        }
        else
        {
            hDevice->pDev->I2CMRXCNT = nBufferSize - 1;
        }

        /* enable receive */
        hDevice->pDev->I2CMCON = COMMON_MBITS | I2CMCON_IENRX;

        /* kick it off */
        CommenceMasterTransfer(hDevice);
    }

    /* honor blocking mode */
    if (true == hDevice->bBlockingMode) {

        /* wait for tcomp */
        BOOLEAN_WAIT (hDevice, bComplete);

		/* halt interrupts while changing receive enable */
		ADI_ENTER_CRITICAL_REGION();
        hDevice->pDev->I2CMCON &= ~I2CMCON_IENRX;
		ADI_EXIT_CRITICAL_REGION();
    } /* end blocking mode */

    return hDevice->result;
}


#if (ADI_I2C_CFG_SLAVE_MODE_SUPPORT == 1)
/*!
 * @brief  Initiate an I2C Slave-Mode transmit.
 *
 * @param[in]   hDevice               Device handle obtained from adi_I2C_SlaveInit().
 * @param[in]   slaveID               Slave device address (7-bit or 10-bit) this slave should respond to.
 * @param[in]   pBuffer               Memory pointer to application's transmit data buffer.
 * @param[in]   nBufferSize           Number of data bytes to transmit.
 *
 * @return         Status
 *                - #ADI_I2C_ERR_BAD_DEVICE_HANDLE [D]    Invalid device handle parameter.
 *                - #ADI_I2C_ERR_NOT_INITIALIZED [D]      Device has not been previously configured for use.
 *                - #ADI_I2C_ERR_BAD_API_FOR_MODE [D]     Device is not configured for Master-Mode operation.
 *                - #ADI_I2C_SLAVE_GENERAL_CALL           Slave received a General Call command.
 *                - #ADI_I2C_SLAVE_REPEAT_START           Slave received a repeat start condition.
 *                - #ADI_I2C_SLAVE_RECEIVED_STOP          Slave received a stop condition.
 *                - #ADI_I2C_SUCCESS                      Transaction completed successfully.
 *
 * Initiates a Slave-Mode transmit operation to transmit data to an external master read operation that matches
 * a slave device address designated by \a slaveID.  The \a slaveID is \a encoded within the external master-side bus
 * command and designates which slave (of potentially many) this API should respond to.  The slave device
 * is placed into a listener role and responds on a device address match between the \a slaveID specified
 * in this API and the slave address encoded on the bus by the external master.
 *
 * The transmit data is read from the memory buffer pointed to by \a pBuffer and sent to the receiving master
 * device until the master faults or \a nBufferSize is reached.
 *
 * Blocking-mode or non-blocking-mode is applied, as determined by adi_I2C_SetBlockingMode() (default is
 * blocking-mode enabled).  In blocking-mode, this API blocks until the transaction is complete.
 * In non-blocking-mode, this API returns immediately and the application must poll for transaction completion
 * with adi_I2C_GetNonBlockingStatus().
 *
 * Various transaction errors are possible and the application should test the return result and take appropriate steps.
 * Any of the #ADI_I2C_SLAVE_GENERAL_CALL, #ADI_I2C_SLAVE_REPEAT_START, or #ADI_I2C_SLAVE_RECEIVED_STOP
 * return codes are terminal conditions and the transaction is stopped.
 *
 * @sa        adi_I2C_SetDmaMode()
 * @sa        adi_I2C_SetBlockingMode()
 * @sa        adi_I2C_GetNonBlockingStatus()
 * @sa        adi_I2C_SlaveReceive()
 */
ADI_I2C_RESULT_TYPE adi_I2C_SlaveTransmit(ADI_I2C_DEV_HANDLE const hDevice,
                                                    uint16_t const slaveID,
                                                     uint8_t      *pBuffer,
                                                    uint16_t const nBufferSize) {
#ifdef ADI_DEBUG
    if (ADI_I2C_INVALID_HANDLE(hDevice))
        return ADI_I2C_ERR_BAD_DEVICE_HANDLE;

    if (ADI_I2C_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_I2C_ERR_NOT_INITIALIZED;

    if (ADI_I2C_STATE_SLAVE_MODE != hDevice->initState)
        return ADI_I2C_ERR_BAD_API_FOR_MODE;
#endif

    /* refuse if a general call is pending */
    if (hDevice->bGeneralCallPending)
        return ADI_I2C_SLAVE_GENERAL_CALL;

    /* wait for any previous fifo content to fully serialize */
    while (SLAVE_BUSY)
    	;

    /* reset I2C control registers */
    configureNVIC(hDevice, false);

    hDevice->result                   = ADI_I2C_SUCCESS;

    /* flush the slave transmit FIFO */
	/* no need for critical section on register read-modify-write *\
	\* becuase this resource is not modified by any interrupt     */
    hDevice->pDev->I2CFSTA |= I2CFSTA_SFLUSH;
    hDevice->pDev->I2CFSTA &= ~I2CFSTA_SFLUSH;

    /* setup for slave-mode transmit */
    hDevice->pTxBuffer                = pBuffer;
    hDevice->TxRemaining              = nBufferSize;
    hDevice->pRxBuffer                = NULL;
    hDevice->RxRemaining              = 0;
    hDevice->dataAddress              = 0;
    hDevice->dataAddressWidth         = ADI_I2C_NO_DATA_ADDRESSING_PHASE;

    /* encode the slave address we should respond to for transmitting data */
    /* (the external master will have done a slave read on this ID)        */
    hDevice->slaveID                  = slaveID;
    programSlaveAddress(hDevice);

    /* enable NVIC */
    configureNVIC(hDevice, true);

#if (ADI_I2C_CFG_ENABLE_DMA_SUPPORT == 1)
    if (hDevice->bDmaMode) {

        hDevice->bDmaComplete = false;  /* blocking is done at app level, via adi_I2C_GetDmaCompletion() */

        /* program initial descriptor pair */
        hDevice->FunctionTable.pfInitDescriptorsFcn(hDevice, hDevice->DmaSlaveTxChannelNum);

        /* enable dma-mode transmit (and combine 10-bit addressing mode bit) */
        hDevice->pDev->I2CSCON = COMMON_SBITS | I2CSCON_TXDMA | (hDevice->bTenBitSlaveAddress ? I2CSCON_ADR10EN : 0);

    }
    else
#endif /* ADI_I2C_CFG_ENABLE_DMA_SUPPORT */
    {

        /* prefill transmit FIFO manually in interrupt-driven mode */
        onSlaveTXREQ();

        /* enable interrupt-mode transmit (and combine 10-bit addressing mode bit) */
        hDevice->pDev->I2CSCON = COMMON_SBITS | I2CSCON_IENTX | (hDevice->bTenBitSlaveAddress ? I2CSCON_ADR10EN : 0);
    }

    /* honor blocking mode */
    if (true == hDevice->bBlockingMode) {

        /* hard wait for last transmit byte */
        while (hDevice->TxRemaining)  /* no interrupt to take us out of semaphore pend or low-power mode */
        	;

    } /* end blocking mode */

    return hDevice->result;
}


/*!
 * @brief  Initiate an I2C Slave-Mode receive.
 *
 * @param[in]   hDevice               Device handle obtained from adi_I2C_SlaveInit().
 * @param[in]   slaveID               Slave address (7-bit or 10-bit) this slave should respond to.
 * @param[in]   pBuffer               Memory pointer to application's receive data buffer.
 * @param[in]   nBufferSize           Maximum number of data bytes to receive.
 * @param[in]   bClockStretch         Control issue of ClockStretch upon receipt of RepeatStart conditions.
 *                - true:     Assert ClockStretch during bus transition.
 *                - false:    Do not assert ClockStretch during bus transition.
 *
 * @return         Status
 *                - #ADI_I2C_ERR_BAD_DEVICE_HANDLE [D]    Invalid device handle parameter.
 *                - #ADI_I2C_ERR_NOT_INITIALIZED [D]      Device has not been previously configured for use.
 *                - #ADI_I2C_ERR_BAD_API_FOR_MODE [D]     Device is not configured for Master-Mode operation.
 *                - #ADI_I2C_SLAVE_GENERAL_CALL           Slave received a General Call command.
 *                - #ADI_I2C_SLAVE_REPEAT_START           Slave received a repeat start condition.
 *                - #ADI_I2C_SLAVE_RECEIVED_STOP          Slave received a stop condition.
 *                - #ADI_I2C_SUCCESS                      Transaction completed successfully.
 *
 * Initiates a Slave-Mode receive operation to receive external master transmit data to slave device
 * address designated by \a slaveID.  The \a slaveID is \a encoded within the external master-side bus command and
 * designates which slave (of potentially many) this API should respond to.   The slave device
 * is placed into a listener role and responds on a device address match between the \a slaveID specified
 * in this API and the slave address encoded on the bus by the external master.
 *
 * The receive data is written to the memory buffer pointed to by \a pBuffer (as read from the transmitting master
 * device), until the application-provisioned buffer size \a nBufferSize is reached or any of the Master-generated
 * conditions are detected: general call, repeat start or stop condition, etc.
 *
 * Blocking-mode or non-blocking-mode is applied, as determined by adi_I2C_SetBlockingMode() (default is
 * blocking-mode enabled).  In blocking-mode, this API blocks until the transaction is complete.
 * In non-blocking-mode, this API returns immediately and the application must poll for transaction completion
 * with adi_I2C_GetNonBlockingStatus().
 *
 * During slave receive (master transmit), the master may employ the so-called "Combined Format" in which the
 * master transitions the bus from write mode (address phase) to read mode (data phase) without an intervening
 * stop condition.  This allows the master to retain bus ownership and guarantee an uninterrupted (atomic) master
 * write-followed-by-read operation (the combined format).
 *
 * The combined format requires the slave driver to switch from a receive to a transmit role.  This transition is
 * triggered by the master sending a repeat start condition instead of the usual stop/start sequence.  In response,
 * the slave receives a repeat start interrupt and cuts short the in-process receive, returning control back to the
 * slave application.  At this point, the slave application does whatever it needs to do based on the received data
 * and then calls back into the slave driver with a slave transmit.  This is done because the slave receiver does
 * not know what data to transmit.
 *
 * Depending on the speed with which the application can turn the bus around (from slave receive to slave transmit),
 * it may employ the clock-stretch feature (of the I2C protocol) to hold off the imminent master receive.  To do so,
 * the application sets the "clock-stretch-on-repeat-start" \a bClockStretch parameter of the slave receive call, indicating
 * the slave receive should commence a clock-stretch upon receipt of the repeat start condition from the master.
 * Once the clock stretch is initiated, the slave receive call terminates with the #ADI_I2C_SLAVE_REPEAT_START return
 * code, indicating to the waiting slave application that the bus has been placed in the clock stretch hold mode
 * and the master is being held off awaiting the responding slave data transmit.
 *
 * Various transaction errors are possible and the application should test the return result and take appropriate steps.
 * Any of the #ADI_I2C_SLAVE_GENERAL_CALL, #ADI_I2C_SLAVE_REPEAT_START, or #ADI_I2C_SLAVE_RECEIVED_STOP
 * return codes are terminal conditions and the transaction is stopped.
 *
 * @note If using clock-stretch-on-repeat-start to hold off the master readback, insure the \a nBufferSize parameter
 *       is large enough (at least one additional byte beyond the expected receive bytes).  This allows the slave receive
 *       call to detect the repeat start condition and assert the clock stretch before the slave receive call exits.
 *       Otherwise, the slave receive will complete and exit when the requested byte count is met, causing the repeat
 *       start sequence associated with the master receive to be missed and no clock stretch to be issued.
 *
 * @note Not all master devices honor the clock stretching protocol, in which case it will not be held off.
 *
 * @sa        adi_I2C_SetDmaMode()
 * @sa        adi_I2C_SetSlaveDeviceIdWidth()
 * @sa        adi_I2C_SetBlockingMode()
 * @sa        adi_I2C_GetNonBlockingStatus()
 * @sa        adi_I2C_SlaveTransmit()
 */
ADI_I2C_RESULT_TYPE adi_I2C_SlaveReceive(ADI_I2C_DEV_HANDLE const hDevice,
                                                   uint16_t const slaveID,
                                                    uint8_t      *pBuffer,
                                                   uint16_t const nBufferSize,
                                                     bool_t const bClockStretch) {
    volatile uint16_t dummyRead;

#ifdef ADI_DEBUG
    if (ADI_I2C_INVALID_HANDLE(hDevice))
        return ADI_I2C_ERR_BAD_DEVICE_HANDLE;

    if (ADI_I2C_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_I2C_ERR_NOT_INITIALIZED;

    if (ADI_I2C_STATE_SLAVE_MODE != hDevice->initState)
        return ADI_I2C_ERR_BAD_API_FOR_MODE;
#endif

    /* refuse if a general call is pending */
    if (hDevice->bGeneralCallPending)
        return ADI_I2C_SLAVE_GENERAL_CALL;

    /* wait for any previous fifo content to fully serialize */
    while (SLAVE_BUSY)
    	;

    /* store the ClockStretch flag for assertion during the RepeatStart interrupt... */
    hDevice->bClockStretch = bClockStretch;

    /* reset I2C control registers */
    configureNVIC(hDevice, false);

    hDevice->result                   = ADI_I2C_SUCCESS;

    /* setup for slave-mode receive */
    hDevice->pTxBuffer                = NULL;
    hDevice->TxRemaining              = 0;
    hDevice->pRxBuffer                = pBuffer;
    hDevice->RxRemaining              = nBufferSize;
    hDevice->dataAddress              = 0;
    hDevice->dataAddressWidth         = ADI_I2C_NO_DATA_ADDRESSING_PHASE;

    /* "flush" slave receive FIFO by dummy reads (no explicit flush bits for SRX) */
    dummyRead = hDevice->pDev->I2CSRX;
    dummyRead = hDevice->pDev->I2CSRX;

    /* encode the slave address to listen on for receiving data */
    hDevice->slaveID                  = slaveID;
    programSlaveAddress(hDevice);

    /* enable NVIC */
    configureNVIC(hDevice, true);

#if (ADI_I2C_CFG_ENABLE_DMA_SUPPORT == 1)
    if (hDevice->bDmaMode) {

        hDevice->bDmaComplete = false;  /* blocking is done at app level, via adi_I2C_GetDmaCompletion() */


        /* program initial descriptor pair */
        hDevice->FunctionTable.pfInitDescriptorsFcn(hDevice,hDevice->DmaSlaveRxChannelNum);

        /* enable dma-mode receive (and combine 10-bit addressing mode bit) */
        hDevice->pDev->I2CSCON = COMMON_SBITS | I2CSCON_RXDMA | (hDevice->bTenBitSlaveAddress ? I2CSCON_ADR10EN : 0);

    }
    else
#endif /* ADI_I2C_CFG_ENABLE_DMA_SUPPORT */
    {

        /* enable interrupt-mode receive (and combine 10-bit addressing mode bit) */
        hDevice->pDev->I2CSCON = COMMON_SBITS | I2CSCON_IENRX | (hDevice->bTenBitSlaveAddress ? I2CSCON_ADR10EN : 0);
    }

    /* honor blocking mode */
    if (true == hDevice->bBlockingMode) {

        /* hard wait for last receive byte */
        while (hDevice->RxRemaining)  /* no interrupt to take us out of semaphore pend or low-power mode */
        	;

    } /* end blocking mode */

    return hDevice->result;
}
#endif /* ADI_I2C_CFG_SLAVE_MODE_SUPPORT */


#ifdef ADI_I2C_LOOPBACK
/* Master Loopback API (test mode API) */
ADI_I2C_RESULT_TYPE adi_I2C_MasterSlaveLoopback   (ADI_I2C_DEV_HANDLE const hDevice,
                                                             uint16_t const slaveID,
                                                              uint8_t      *pTxData,
                                                              uint8_t      *pRxData,
                                                             uint16_t const nBufferSize) {
    /* test mode code for I2C functional testing without external slave */
    /* runs master and slave simultaneously with internal loopback      */

    /* setup the transaction */
    hDevice->bMasterReadBit = false;    /* master writes to salve */
    hDevice->slaveID        = slaveID;  /* slave listens on this slaveID */
    hDevice->pTxBuffer      = pTxData;
    hDevice->TxRemaining    = nBufferSize;
    hDevice->pRxBuffer      = pRxData;
    hDevice->RxRemaining    = nBufferSize;
    hDevice->result         = ADI_I2C_SUCCESS;

    /* program slave address for master transmit so slaveID responds to this master */
    programSlaveAddress(hDevice);

    /* enable NVIC for both master AND slave */
    ADI_ENABLE_INT(I2CM_IRQn);
    ADI_ENABLE_INT(I2CS_IRQn);

    /* enable master transmit interrupt (note critical LOOPBACK bit) */
    hDevice->pDev->I2CMCON = COMMON_MBITS | I2CMCON_IENTX | I2CMCON_LOOPBACK;

    /* enable slave receive interrupt */
    hDevice->pDev->I2CSCON = COMMON_SBITS | I2CSCON_IENRX | (hDevice->bTenBitSlaveAddress ? I2CSCON_ADR10EN : 0);

    /* prefill master transmit fifo */
    onMasterTXREQ(hDevice);

    /* launch the master transmit */
    CommenceMasterTransfer(hDevice);

    /* honor blocking mode */
    if (true == hDevice->bBlockingMode) {

        /* hard wait for last receive byte */
        while (hDevice->RxRemaining)  /* no interrupt to take us out of semaphore pend or low-power mode */
        	;

    } /* end blocking mode */

    return hDevice->result;
}
#endif



/*
    //////////////////////////////////////////////////////////////////////////////
    ///////////////////////   STATIC FUNCTIONS   /////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////
*/

static ADI_I2C_RESULT_TYPE commonInit(ADI_I2C_DEV_HANDLE const hDevice) {

	/* NOTE: pinmux code is deprecated and removed in lieu of required pinmux plugin configuration code */

    /* initialize core state variables */
    hDevice->initState              = ADI_I2C_STATE_UNINITIALIZED;
    hDevice->result                 = ADI_I2C_SUCCESS;

#if (ADI_I2C_CFG_ENABLE_DMA_SUPPORT == 1)
    hDevice->bDmaMode               = false;
#endif /* ADI_I2C_CFG_ENABLE_DMA_SUPPORT */

    hDevice->bBlockingMode          = true;
    hDevice->bClockStretch          = false;
    hDevice->bRepeatStart           = false;
    hDevice->firstMasterAddressByte = 0;
    hDevice->bTenBitSlaveAddress    = false;
    hDevice->bGeneralCallPending    = false;
    hDevice->generalCallCommand     = 0;

    /* initialize function table to avoid dma references */
    hDevice->FunctionTable.pfInitFcn            = &fPioInit;
    hDevice->FunctionTable.pfUnInitFcn          = &fPioUnInit;
    hDevice->FunctionTable.pfInitDescriptorsFcn = fPioInitializeDescriptors;
    hDevice->FunctionTable.pfDmaHandlerFcn      = NULL;

#if (ADI_I2C_CFG_ENABLE_CALLBACK_SUPPORT==1)
	/* init callbacks */
    hDevice->pfCallback = NULL;
    hDevice->pcbParam   = NULL;
#endif /* ADI_I2C_CFG_ENABLE_CALLBACK_SUPPORT */

    hDevice->pDev = pADI_I2C;
    hDevice->PioMasterIrqNum = I2CM_IRQn;
    ADI_INSTALL_HANDLER(I2CM_IRQn, I2C0_Master_Int_Handler);

#if (ADI_I2C_CFG_SLAVE_MODE_SUPPORT == 1)
    hDevice->PioSlaveIrqNum = I2CS_IRQn ;
    ADI_INSTALL_HANDLER(I2CS_IRQn, I2C0_Slave_Int_Handler);
#endif /* ADI_I2C_CFG_SLAVE_MODE_SUPPORT */

#if (ADI_I2C_CFG_ENABLE_DMA_SUPPORT == 1)
    /* init simple DMA variables */
    hDevice->bDmaComplete           = false;
    hDevice->pDmaDescriptor         = &gDmaDescriptor;
    hDevice->nextDmaDescrType       = ADI_DMA_CCD_PRIMARY;
    hDevice->pNextDmaAddress        = NULL;
    hDevice->nUnscheduledDmaCount   = 0;

    hDevice->DmaMasterIrqNum = DMA_I2CM_IRQn ;
    ADI_INSTALL_HANDLER(DMA_I2CM_IRQn, DMA_I2C0_MX_Int_Handler);

#if (ADI_I2C_CFG_SLAVE_MODE_SUPPORT == 1)
    hDevice->DmaSlaveTxIrqNum = DMA_I2CS_TX_IRQn;
    ADI_INSTALL_HANDLER(DMA_I2CS_TX_IRQn, DMA_I2C0_STX_Int_Handler);

    hDevice->DmaSlaveRxIrqNum = DMA_I2CS_RX_IRQn;
    ADI_INSTALL_HANDLER(DMA_I2CS_RX_IRQn, DMA_I2C0_SRX_Int_Handler);
#endif /* ADI_I2C_CFG_SLAVE_MODE_SUPPORT */

    hDevice->pDev = pADI_I2C;

#endif /* ADI_I2C_CFG_ENABLE_DMA_SUPPORT */

    /* reset clock divider */
    hDevice->pDev->I2CDIV = I2CDIV_RVAL;

    /* clear slave device ID registers */
    hDevice->pDev->I2CID0 = 0;
    hDevice->pDev->I2CID1 = 0;
    hDevice->pDev->I2CID2 = 0;
    hDevice->pDev->I2CID3 = 0;

    /* OS-aware blocking object initializations */
#if (ADI_CFG_ENABLE_RTOS_SUPPORT == 1)
	hDevice->hSem = NULL;
    /* Create the semaphore for blocking mode support */
    if(adi_osal_SemCreateStatic(&hDevice->SemMemory, ADI_OSAL_MAX_SEM_SIZE_CHAR, &hDevice->hSem, 0u) != ADI_OSAL_SUCCESS)
        return ADI_I2C_ERR_SEMAPHORE_FAILED;
#else
    /* init interrupt flag for low power mode */
    hDevice->bLowPowerExitFlag = false;
#endif /* ADI_CFG_ENABLE_RTOS_SUPPORT  */

    return ADI_I2C_SUCCESS;
}


static void configureNVIC(ADI_I2C_DEV_HANDLE const hDevice, bool_t bEnable) {

    /* enable according to mode */
    if (bEnable) {
        switch (hDevice->initState) {

            case ADI_I2C_STATE_MASTER_MODE:
#if (ADI_I2C_CFG_ENABLE_DMA_SUPPORT == 1)
                if (hDevice->bDmaMode) {

                  // shared channel
                  ADI_ENABLE_INT(hDevice->DmaMasterIrqNum);
                }
#endif /* ADI_I2C_CFG_ENABLE_DMA_SUPPORT */
                ADI_ENABLE_INT(hDevice->PioMasterIrqNum);
                break;

#if (ADI_I2C_CFG_SLAVE_MODE_SUPPORT == 1)
            case ADI_I2C_STATE_SLAVE_MODE:

#if (ADI_I2C_CFG_ENABLE_DMA_SUPPORT == 1)
                if (hDevice->bDmaMode) {
                  ADI_ENABLE_INT(hDevice->DmaSlaveTxIrqNum);
                  ADI_ENABLE_INT(hDevice->DmaSlaveRxIrqNum);
                }
#endif /* ADI_I2C_CFG_ENABLE_DMA_SUPPORT */

                ADI_ENABLE_INT(hDevice->PioSlaveIrqNum);
                break;
#endif /* ADI_I2C_CFG_SLAVE_MODE_SUPPORT */
        }

    } else {  /* disable interrupts */

        /* clear all I2C NVIC interrupts */
        ADI_DISABLE_INT(hDevice->PioMasterIrqNum);

#if (ADI_I2C_CFG_SLAVE_MODE_SUPPORT == 1)
        ADI_DISABLE_INT(hDevice->PioSlaveIrqNum);
#endif /* ADI_I2C_CFG_SLAVE_MODE_SUPPORT */

#if (ADI_I2C_CFG_ENABLE_DMA_SUPPORT == 1)
        if (hDevice->bDmaMode) {

            // shared interrupt
            ADI_DISABLE_INT(hDevice->DmaMasterIrqNum);

#if (ADI_I2C_CFG_SLAVE_MODE_SUPPORT == 1)
			// dedicated interrupts
            ADI_DISABLE_INT(hDevice->DmaSlaveTxIrqNum);
            ADI_DISABLE_INT(hDevice->DmaSlaveRxIrqNum);
#endif /* ADI_I2C_CFG_SLAVE_MODE_SUPPORT */

        }
#endif /* ADI_I2C_CFG_ENABLE_DMA_SUPPORT */
    }
}


static void prefillTx(ADI_I2C_DEV_HANDLE const hDevice) {

    /* push data into the transmit FIFO until its full                */
    /* (transmit data address bytes first, then switch to data bytes) */
    while (I2CFSTA_MTXFSTA_TWOBYTES != (hDevice->pDev->I2CFSTA & I2CFSTA_MTXFSTA_MSK)) {

        if (hDevice->dataAddressWidth)
        {
            /* addressing phase... */
            hDevice->dataAddressWidth--;  /* careful to not decrement beyond zero as this flags address complete */
            hDevice->pDev->I2CMTX = (uint8_t) (hDevice->dataAddress >> (hDevice->dataAddressWidth * 8) & 0xff);

        } else if (hDevice->TxRemaining) {
            /* data phase... */
            hDevice->TxRemaining--;
            hDevice->pDev->I2CMTX = *(hDevice->pTxBuffer++);

         } else {
            /* break out when both address (optional) and data phases have filled the transmit FIFO */
            break;
         }
    } /* end while */
}


static void CommenceMasterTransfer(ADI_I2C_DEV_HANDLE const hDevice) {
    /*
        Initiates a master-mode transfer.
        Program the master control byte with control code, chip select bits (slaveID)
        and r/w bit.  Call this AFTER device enable because writing ADR0 is what actually
        initiates the transfer.  Note that in 10-bit addressing mode, ADR1 write MUST occur
        BEFORE writing ADR0.

        Note that repeat starts are generated by rewriting the first master address byte register in
        the first master-mode TX/Rx interrupt... so save it as firstMasterAddressByte for later.
    */

    if (hDevice->bTenBitSlaveAddress) {

        /* 10-bit slave addressing spans two registers... */

        /* 2nd addr reg gets the lower-order 8 bits of slave ID address */
        SECOND_MAB = hDevice->slaveID & 0x00FF;

        /* 1st addr reg gets "0000000011110xxy"b + upper two ID bits (9:8) upshifted by one (xx) + r/w bit in LS position (y) */
        hDevice->firstMasterAddressByte = ( 0x00F0 | ((hDevice->slaveID & 0x0300) >> (8-1)) )
                                          | (hDevice->bMasterReadBit ? 0x0001 : 0x00000);

    } else {

        /* seven-bit address (upshifted by one) + w/r bit */
        hDevice->firstMasterAddressByte = ((hDevice->slaveID & 0x007F) << 1) | (hDevice->bMasterReadBit ? 0x0001 : 0x00000);
    }

    hDevice->bComplete = false;

#if (ADI_I2C_CFG_ENABLE_DMA_SUPPORT == 1)
    hDevice->bDmaComplete = false;
#endif /* ADI_I2C_CFG_ENABLE_DMA_SUPPORT */

    /* this is the actual launch */
    FIRST_MAB = hDevice->firstMasterAddressByte;
}


static void programSlaveAddress(ADI_I2C_DEV_HANDLE const hDevice) {
    /* Call this BEFORE DEVICE ENABLE so incoming transactions are not missed      */
    /* does not support multiple slave address registers... just one.              */
    /* identifies THIS slave device ID, allowing external master(s) to address us. */

    if (hDevice->bTenBitSlaveAddress) {

        /* 10-bit slave addressing spans two registers... */

        /* 1st addr reg gets the upper-two address bits + binary code '11110'    */
        /* so '00000000011110xx', where 'xx' is the upper two address bits.      */
        /* NOTE absence of single-bit left-shift (used in Master-Mode) to encode */
        /* r/w bit... there IS no r/w bit to encode in Slave-Mode.               */
        hDevice->pDev->I2CID0 = 0x0078 | ((hDevice->slaveID & 0x0300) >> 8);

        /* 2nd addr reg gets lower 8-bits of address */
        hDevice->pDev->I2CID1 = hDevice->slaveID & 0x00ff;

    } else {

        /* seven-bit address (upshifted by one) without r/w bit (don't care) */
        hDevice->pDev->I2CID0 = (hDevice->slaveID & 0x007f) << 1;
    }
}


/*
    //////////////////////////////////////////////////////////////////////////////
    ////////////////////   POINTER TABLE FUNCTIONS   /////////////////////////////
    //////////////////////////////////////////////////////////////////////////////
*/


static ADI_I2C_RESULT_TYPE fPioInit(ADI_I2C_DEV_HANDLE const hDevice) {
	return ADI_I2C_SUCCESS;
}

#if (ADI_I2C_CFG_ENABLE_DMA_SUPPORT == 1)
static ADI_I2C_RESULT_TYPE fDmaInit(ADI_I2C_DEV_HANDLE const hDevice) {

    switch (hDevice->initState) {
        case ADI_I2C_STATE_MASTER_MODE:

            if (ADI_DMA_SUCCESS != adi_DMA_Init(hDevice->DmaMasterTxChannelNum, ADI_DMA_PRIORITY_RESET)) return ADI_I2C_ERR_BAD_DMA_CONFIG;
            break;

#if (ADI_I2C_CFG_SLAVE_MODE_SUPPORT == 1)
        case ADI_I2C_STATE_SLAVE_MODE:
            if (ADI_DMA_SUCCESS != adi_DMA_Init(hDevice->DmaSlaveTxChannelNum,  ADI_DMA_PRIORITY_RESET)) return ADI_I2C_ERR_BAD_DMA_CONFIG;
            if (ADI_DMA_SUCCESS != adi_DMA_Init(hDevice->DmaSlaveRxChannelNum,  ADI_DMA_PRIORITY_RESET)) return ADI_I2C_ERR_BAD_DMA_CONFIG;
            break;
#endif /* ADI_I2C_CFG_SLAVE_MODE_SUPPORT */

    }
    return ADI_I2C_SUCCESS;
}
#endif /* ADI_I2C_CFG_ENABLE_DMA_SUPPORT */


static ADI_I2C_RESULT_TYPE fPioUnInit(ADI_I2C_DEV_HANDLE const hDevice) {
	return ADI_I2C_SUCCESS;
}


#if (ADI_I2C_CFG_ENABLE_DMA_SUPPORT == 1)
static ADI_I2C_RESULT_TYPE fDmaUnInit(ADI_I2C_DEV_HANDLE const hDevice) {

    switch (hDevice->initState) {
        case ADI_I2C_STATE_MASTER_MODE:

            if (ADI_DMA_SUCCESS != adi_DMA_UnInit(hDevice->DmaMasterTxChannelNum)) return ADI_I2C_ERR_BAD_DMA_CONFIG;
            break;

#if (ADI_I2C_CFG_SLAVE_MODE_SUPPORT == 1)
        case ADI_I2C_STATE_SLAVE_MODE:
            if (ADI_DMA_SUCCESS != adi_DMA_UnInit(hDevice->DmaSlaveTxChannelNum))  return ADI_I2C_ERR_BAD_DMA_CONFIG;
            if (ADI_DMA_SUCCESS != adi_DMA_UnInit(hDevice->DmaSlaveRxChannelNum))  return ADI_I2C_ERR_BAD_DMA_CONFIG;
            break;
#endif /* ADI_I2C_CFG_SLAVE_MODE_SUPPORT */
    }
    return ADI_I2C_SUCCESS;
}
#endif /* ADI_I2C_CFG_ENABLE_DMA_SUPPORT */


#if (ADI_I2C_CFG_ENABLE_CALLBACK_SUPPORT==1)
/*!
 * @brief  Register or unregister an application-level callback function.
 *
 * @param [in]   hDevice        Device handle obtained from adi_I2C_Master/SlaveInit().
 * @param [in]   pfCallback     Pointer to the application callback function. Can be passed as NULL to
 *                              unregister the previously registered callback.
 * @param [in]   pcbParam       Callback parameter which will be passed back to the application when the
 *                              callback is called.
 *
 * @return      Status
 *                - #ADI_I2C_ERR_BAD_DEV_HANDLE [D]  Invalid device handle parameter.
 *                - #ADI_I2C_ERR_NOT_INITIALIZED [D] Device has not been previously configured for use.
 *                - #ADI_I2C_SUCCESS                 Call completed successfully.
 *
 * Registers and application-provided callback function to receive notifications when transfers are complete.
 * Callbacks may be used to avoid polling loops and provide optimal notification timing as each transaction
 * is complete.
 *
 * The callback to the application provides an #ADI_I2C_CALLBACK_EVENT_TYPE event parameter, indicating what
 * exactly caused the interrupt.
 *
 * In the case of DMA completion (#ADI_I2C_EVENT_DMA_COMPLETE), the DMA channel ID (#DMA_CHANn_TypeDef) is also passed.
 *
 * In the case of a slave general call command (#ADI_I2C_EVENT_SLAVE_CGINT), the received GC command is also passed.
 *
 * The callback parameter is an application-owned value provided during the registration.  It is stored by the
 * I2C driver and passed back to the application as a callback parameter, allowing the application to identify
 * the callback context.  The callback parameter can be the I2C device handle, to the application can tell
 * which device issued the callback.
 *
 * @note Callbacks are executed in context of the originating interrupt, so extended processing at the
 * application level is to be avoided.  Typically, the application will only take note of the callback
 * or perform some trivial housekeeping task and return immediately.
 */
ADI_I2C_RESULT_TYPE adi_I2C_RegisterCallback (ADI_I2C_DEV_HANDLE const hDevice, ADI_CALLBACK const pfCallback, void *const pcbParam )
{
#ifdef ADI_DEBUG
    if (ADI_I2C_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_I2C_ERR_NOT_INITIALIZED;

#endif

    /* Save the application provided callback and callback parameters */
    hDevice->pfCallback = pfCallback;
    hDevice->pcbParam   = pcbParam;

    return ADI_I2C_SUCCESS;
}
#endif /* ADI_I2C_CFG_ENABLE_CALLBACK_SUPPORT */


/*
    //////////////////////////////////////////////////////////////////////////////
    ///////////////////////   INTERRUPT-RELATED   ////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////
*/


static __INLINE void onMasterCOMP(ADI_I2C_DEV_HANDLE const hDevice) {

    /* master is complete... */

    /* note condition */
    /* hDevice->result = ADI_I2C_MASTER_STOP; */

#if (ADI_I2C_CFG_ENABLE_DMA_SUPPORT == 1)
    if (hDevice->bDmaMode) {
		/* force wait mode completion */
		BOOLEAN_UNWAIT(hDevice, bDmaComplete);
    }
#endif /* ADI_I2C_CFG_ENABLE_DMA_SUPPORT */

	/* no need for callback... completion callback is issued a/o/p/ TX/RX completion */
}


static __INLINE void onMasterNACKDATA(ADI_I2C_DEV_HANDLE const hDevice) {

    /* slave failed to ACK data... */

    /* disable controller */
    hDevice->pDev->I2CMCON = I2CMSTA_RVAL;

    /* note condition */
    hDevice->result = ADI_I2C_MASTER_NACK_DATA;

#if (ADI_I2C_CFG_ENABLE_CALLBACK_SUPPORT==1)
	/* issue the callback */
	if(hDevice->pfCallback){
		hDevice->pfCallback(hDevice->pcbParam, ADI_I2C_EVENT_MASTER_NACKDATA, NULL);
	}
#endif /* ADI_I2C_CFG_ENABLE_CALLBACK_SUPPORT */
}


static __INLINE void onMasterALOST(ADI_I2C_DEV_HANDLE const hDevice) {

    /* master lost bus... */

    /* disable controller */
    hDevice->pDev->I2CMCON = I2CMSTA_RVAL;

    /* note condition */
    hDevice->result = ADI_I2C_MASTER_ALOST;

#if (ADI_I2C_CFG_ENABLE_CALLBACK_SUPPORT==1)
	/* issue the callback */
	if(hDevice->pfCallback){
		hDevice->pfCallback(hDevice->pcbParam, ADI_I2C_EVENT_MASTER_ALOST, NULL);
	}
#endif /* ADI_I2C_CFG_ENABLE_CALLBACK_SUPPORT */
}


static __INLINE void onMasterNACKADDR(ADI_I2C_DEV_HANDLE const hDevice) {

    /* slave failed to ACK address... */

    /* disable controller */
    hDevice->pDev->I2CMCON = I2CMSTA_RVAL;

    /* note condition */
    hDevice->result = ADI_I2C_MASTER_NACK_ADDRESS;

#if (ADI_I2C_CFG_ENABLE_CALLBACK_SUPPORT==1)
	/* issue the callback */
	if(hDevice->pfCallback){
		hDevice->pfCallback(hDevice->pcbParam, ADI_I2C_EVENT_MASTER_NACKADDR, NULL);
	}
#endif /* ADI_I2C_CFG_ENABLE_CALLBACK_SUPPORT */
}


static __INLINE void onMasterRXREQ(ADI_I2C_DEV_HANDLE const hDevice) {

	/* no need for critical sections around register   *\
	\* read-modify-writes because we're in an interrupt */

    /* master receive FIFO is not empty... */

    register ADI_I2C_DEV_DATA_TYPE* pDD = hDevice;

    /* Send REPEAT START condition to slave on request by one-time */
    /* rewrite of the first master address byte register on first  */
    /* fifo interrupt of sequence                                  */
    if (pDD->bRepeatStart) {

        /* rewrite ADR0 register */
        FIRST_MAB = pDD->firstMasterAddressByte;

        /* only do it once... */
        pDD->bRepeatStart = false;
    }

    /* pump the fifo manually in non-dma mode */
#if (ADI_I2C_CFG_ENABLE_DMA_SUPPORT == 1)
    if (!pDD->bDmaMode) {
#endif /* ADI_I2C_CFG_ENABLE_DMA_SUPPORT */

        /* data phase only... (addressing was done in a preceding master transmit) */
        *(pDD->pRxBuffer++) = hDevice->pDev->I2CMRX;
        pDD->RxRemaining--;

        if (pDD->RxRemaining == 0) {

            /* disable interrupt receive */
            hDevice->pDev->I2CMCON &= ~I2CMCON_IENRX;

			/* complete the wait mode */
			BOOLEAN_UNWAIT (pDD, bComplete);

#if (ADI_I2C_CFG_ENABLE_CALLBACK_SUPPORT==1)
			/* issue the callback */
			if(pDD->pfCallback){
				pDD->pfCallback(pDD->pcbParam, ADI_I2C_EVENT_MASTER_RX_COMPLETE, NULL);
			}
#endif /* ADI_I2C_CFG_ENABLE_CALLBACK_SUPPORT */

        } /* end zero remaining test */

#if (ADI_I2C_CFG_ENABLE_DMA_SUPPORT == 1)
    }
#endif /* ADI_I2C_CFG_ENABLE_DMA_SUPPORT */
}


static __INLINE void onMasterTXREQ(ADI_I2C_DEV_HANDLE const hDevice) {

    /* master transmit FIFO is not full... */

	/* no need for critical sections around register   *\
	\* read-modify-writes because we're in an interrupt */

    register ADI_I2C_DEV_DATA_TYPE* pDD = hDevice;

    /* Send REPEAT START condition to slave on request by one-time */
    /* rewrite of the first master address byte register on first  */
    /* fifo interrupt of sequence                                  */
    if (pDD->bRepeatStart) {

        /* rewrite ADR0 register */
        FIRST_MAB = pDD->firstMasterAddressByte;

        /* only do it once... */
        pDD->bRepeatStart = false;
    }

    /* pump the fifo manually in non-dma mode */
#if (ADI_I2C_CFG_ENABLE_DMA_SUPPORT == 1)
    if (!pDD->bDmaMode) {
#endif /* ADI_I2C_CFG_ENABLE_DMA_SUPPORT */

        if (pDD->dataAddressWidth) {

            /* addressing phase... */
            pDD->dataAddressWidth--;  /* careful to not decrement beyond zero as this flags address complete */
            hDevice->pDev->I2CMTX = (uint8_t) (pDD->dataAddress >> (pDD->dataAddressWidth * 8) & 0xff);

        } else if (pDD->TxRemaining) {

			/* data phase... */
            pDD->TxRemaining--;
            hDevice->pDev->I2CMTX = *(pDD->pTxBuffer++);

        } else {

            /* disable transmit interrupt */
            hDevice->pDev->I2CMCON &= ~I2CMCON_IENTX;

			/* complete the wait mode */
			BOOLEAN_UNWAIT (pDD, bComplete)

#if (ADI_I2C_CFG_ENABLE_CALLBACK_SUPPORT==1)
			/* issue the callback */
			if(pDD->pfCallback){
				pDD->pfCallback(pDD->pcbParam, ADI_I2C_EVENT_MASTER_TX_COMPLETE, NULL);
			}
#endif /* ADI_I2C_CFG_ENABLE_CALLBACK_SUPPORT */

        }  /* end zero remaining test */

#if (ADI_I2C_CFG_ENABLE_DMA_SUPPORT == 1)
    }
#endif /* ADI_I2C_CFG_ENABLE_DMA_SUPPORT */
}


void DispatchMasterInterrupt (ADI_I2C_DEV_HANDLE const hDevice) {

    register uint16_t status = hDevice->pDev->I2CMSTA;

    /* check error conditions */
    if (status & I2CMSTA_NACKDATA)
    {
        onMasterNACKDATA(hDevice);
    }
    
    if (status & I2CMSTA_NACKADDR)
    {
        onMasterNACKADDR(hDevice);
    }
    
    if (status & I2CMSTA_ALOST)
    {
        onMasterALOST(hDevice);
    }

    /* check completion condition */
    if (status & I2CMSTA_TCOMP)
    {
       onMasterCOMP(hDevice);
    }

    /* check Rx/Tx conditions */
    if (status & I2CMSTA_TXREQ)
    {
        onMasterTXREQ(hDevice);
    }
    
    if (status & I2CMSTA_RXREQ)
    {
        onMasterRXREQ(hDevice);
    }
}


/*!
 * @brief  I2C0 Master Interrupt Handler.
 *
 * @return void.
 *
 * Override of default (WEAK) I2C0 master interrupt handler.
 */
ADI_INT_HANDLER (I2C0_Master_Int_Handler) {
	DispatchMasterInterrupt(pI2C0_DevData);
}


#if (ADI_I2C_CFG_SLAVE_MODE_SUPPORT == 1)
static __INLINE void onSlaveREPSTART(ADI_I2C_DEV_HANDLE const hDevice) {

	/* no need for critical sections around register   *\
	\* read-modify-writes because we're in an interrupt */

    /*
        Slave received a Repeat Start (Sr) condition.  The Sr condition can/should only
        occur during a slave receive call (by the slave app) in which the external master
        is completing the first part of a "Combined Format" master write (of read address -
        received by slave) followed by a master read (of read data - transmitted by slave).
        The Sr condition signals the combined format bus transition from write to read.

        We would like to just pass Sr on to the slave transmit handler (i.e., onSlaveTXREQ())
        as an early indication of a master read sequence instead of waiting for the
        I2CSSTA_TXREQ interrupt.  Basically, we would just prefill the transmit fifo...

        But we need to get out of current Slave Receive call and have the app call back
        into Slave Transmit.  This allows the app to manage what is transmitted based on
        the received data, since the driver will not know what to transmit a priori.
        In the trivial case, we *could* just transmit the contents of the received data,
        interpreting/assuming it is an address.  But in the general case, the app may want
        to process the received data or even interpret it as a command and do some transmit
        data synthesis.  Hence, we must give back control to the app in the general case.

        Based on mode, we will here assert a clock stretch, both locking the bus and extending
        the time the app has to process/interpret the received data and prepare an appropriate
        response.  Once the responding data is ready, the app simply calls the slave transmit
        API, thereby turning the bus around consistent with the implied "combined format"
        (external) MasterRead sequence we are responding to.

        if the clock-stretch-on-repeat-start mode is set, assert the stretch.  If not, do not
        stretch.  In either case, terminate/exit the receive in process with repeat-start-received
        return code, allowing the app to turn the bus around.
    */

    /* First (as soon as possible) assert the clock stretch */
    if (hDevice->bClockStretch)
        hDevice->pDev->I2CSCON |= I2CSCON_STRETCH;

    /* clear state variables to spark an exit in the slave state machine */
    /* so slave app can call back into driver with a transmit request    */
    hDevice->TxRemaining          = 0;
    hDevice->RxRemaining          = 0;

#if (ADI_I2C_CFG_ENABLE_DMA_SUPPORT == 1)
	/* force wait mode completion */
	BOOLEAN_UNWAIT(hDevice, bDmaComplete)
    hDevice->nUnscheduledDmaCount = 0;
#endif /* ADI_I2C_CFG_ENABLE_DMA_SUPPORT */

    /* set return code to announce repeat start to app */
    hDevice->result = ADI_I2C_SLAVE_REPEAT_START;

#if (ADI_I2C_CFG_ENABLE_CALLBACK_SUPPORT==1)
	/* issue the callback */
	if(hDevice->pfCallback){
		hDevice->pfCallback(hDevice->pcbParam, ADI_I2C_EVENT_SLAVE_REPSTART, NULL);
	}
#endif /* ADI_I2C_CFG_ENABLE_CALLBACK_SUPPORT */

    /* leave controller active... */
}


static __INLINE void onSlaveSTOP(ADI_I2C_DEV_HANDLE const hDevice) {

    /* slave received a stop condition... */

    /* clear state variables to spark an exit in the slave state machine */
    hDevice->TxRemaining          = 0;
    hDevice->RxRemaining          = 0;
#if (ADI_I2C_CFG_ENABLE_DMA_SUPPORT == 1)
    if (hDevice->bDmaMode) {
	/* force wait mode completion */
	BOOLEAN_UNWAIT(hDevice, bDmaComplete)
        hDevice->nUnscheduledDmaCount = 0;
    }
#endif /* ADI_I2C_CFG_ENABLE_DMA_SUPPORT */

    /* disable controller */
    hDevice->pDev->I2CSCON = I2CSSTA_RVAL;

    /* note condition */
    /* This is a normal behaivor, no need to take exceptional action */
    /* hDevice->result = ADI_I2C_SLAVE_RECEIVED_STOP; */

#if (ADI_I2C_CFG_ENABLE_CALLBACK_SUPPORT==1)
	/* issue the callback */
	if(hDevice->pfCallback){
		hDevice->pfCallback(hDevice->pcbParam, ADI_I2C_EVENT_SLAVE_STOP, NULL);
	}
#endif /* ADI_I2C_CFG_ENABLE_CALLBACK_SUPPORT */
}


static __INLINE void onSlaveGCINT(ADI_I2C_DEV_HANDLE const hDevice) {

    /* slave received a general call command... */

    /* disable controller */
    hDevice->pDev->I2CSCON = I2CSSTA_RVAL;

    /* store the general call command for adi_I2C_GetGeneralCallCommand() */
    hDevice->bGeneralCallPending = true;
    hDevice->generalCallCommand = hDevice->pDev->I2CSRX;

    /* note condition */
    hDevice->result = ADI_I2C_SLAVE_GENERAL_CALL;

#if (ADI_I2C_CFG_ENABLE_CALLBACK_SUPPORT==1)
	/* issue the callback */
	if(hDevice->pfCallback){
		hDevice->pfCallback(hDevice->pcbParam, ADI_I2C_EVENT_SLAVE_CGINT, (void*)hDevice->generalCallCommand);
	}
#endif /* ADI_I2C_CFG_ENABLE_CALLBACK_SUPPORT */

}


static __INLINE void onSlaveRXREQ(ADI_I2C_DEV_HANDLE const hDevice) {

    /* slave receive FIFO is not empty... */

    register ADI_I2C_DEV_DATA_TYPE* pDD = hDevice;

    /* pump the fifo manually in non-dma mode */
#if (ADI_I2C_CFG_ENABLE_DMA_SUPPORT == 1)
    if (!pDD->bDmaMode) {
#endif /* ADI_I2C_CFG_ENABLE_DMA_SUPPORT */

        /* pull data out of the receive FIFO until its empty */
        while (I2CFSTA_STXFSTA_EMPTY != (hDevice->pDev->I2CFSTA & I2CFSTA_SRXFSTA_MSK)) {

            if (pDD->RxRemaining) {
                *(pDD->pRxBuffer++) = hDevice->pDev->I2CSRX;
                pDD->RxRemaining--;

            } else {

#if (ADI_I2C_CFG_ENABLE_CALLBACK_SUPPORT==1)
				/* issue the callback */
				if(pDD->pfCallback){
					pDD->pfCallback(pDD->pcbParam, ADI_I2C_EVENT_SLAVE_RX_COMPLETE, NULL);
				}
#endif /* ADI_I2C_CFG_ENABLE_CALLBACK_SUPPORT */

                break; /* break out when all data has been received */
            }
        } /* end while */

#if (ADI_I2C_CFG_ENABLE_DMA_SUPPORT == 1)
    } /* end DMA mode */
#endif /* ADI_I2C_CFG_ENABLE_DMA_SUPPORT */
}


static __INLINE void onSlaveTXREQ(ADI_I2C_DEV_HANDLE const hDevice) {

    /* slave transmit FIFO is not full... */

    register ADI_I2C_DEV_DATA_TYPE* pDD = hDevice;

    /* pump the fifo manually in non-dma mode */
#if (ADI_I2C_CFG_ENABLE_DMA_SUPPORT == 1)
    if (!pDD->bDmaMode) {
#endif /* ADI_I2C_CFG_ENABLE_DMA_SUPPORT */

        /* push data into the transmit FIFO until its full */
        /* (no register/memory addressing phase... just data) */
        while (I2CFSTA_STXFSTA_TWOBYTES != (hDevice->pDev->I2CFSTA & I2CFSTA_STXFSTA_MSK)) {

            if (pDD->TxRemaining) {

                /* data phase... */
                hDevice->pDev->I2CSTX = *(pDD->pTxBuffer++);
                pDD->TxRemaining--;

            } else {

#if (ADI_I2C_CFG_ENABLE_CALLBACK_SUPPORT==1)
				/* issue the callback */
				if(pDD->pfCallback){
					pDD->pfCallback(pDD->pcbParam, ADI_I2C_EVENT_SLAVE_TX_COMPLETE, NULL);
				}
#endif /* ADI_I2C_CFG_ENABLE_CALLBACK_SUPPORT */

                break; /* break out when all data has been transmitted */
            }
        } /* end while */

#if (ADI_I2C_CFG_ENABLE_DMA_SUPPORT == 1)
    } /* end DMA mode */
#endif /* ADI_I2C_CFG_ENABLE_DMA_SUPPORT */
}


void DispatchSlaveInterrupt(ADI_I2C_DEV_HANDLE const hDevice) {

    register uint16_t status = hDevice->pDev->I2CSSTA;

    /* priortized slave-mode event handlers */
         if (status & I2CSSTA_RXREQ)     onSlaveRXREQ(hDevice);
    else if (status & I2CSSTA_TXREQ)     onSlaveTXREQ(hDevice);
    else if (status & I2CSSTA_REPSTART)  onSlaveREPSTART(hDevice);
    else if (status & I2CSSTA_STOP)      onSlaveSTOP(hDevice);
    else if (status & I2CSSTA_GCINT)     onSlaveGCINT(hDevice);
}


/*!
 * @brief  I2C0 Slave Interrupt Handler.
 *
 * @return void.
 *
 * Override of default (WEAK) I2C0 slave interrupt handler.
 */
ADI_INT_HANDLER (I2C0_Slave_Int_Handler) {
    DispatchSlaveInterrupt(pI2C0_DevData);
}

#endif /* ADI_I2C_CFG_SLAVE_MODE_SUPPORT */



/*
    //////////////////////////////////////////////////////////////////////////////
    //////////////////////////   DMA-RELATED   ///////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////
*/

/* stub for interrupt mode */
static ADI_I2C_RESULT_TYPE fPioInitializeDescriptors(ADI_I2C_DEV_HANDLE const hDevice, DMA_CHANn_TypeDef chan) {
    return ADI_I2C_ERR_BAD_API_FOR_MODE;
}


#if (ADI_I2C_CFG_ENABLE_DMA_SUPPORT == 1)
/* prefill DMA pipeline with two descriptors */
static ADI_I2C_RESULT_TYPE fDmaInitializeDescriptors(ADI_I2C_DEV_HANDLE const hDevice, DMA_CHANn_TypeDef chan) {

    /* Always submit two descriptors initially (primary & alternate) even if the 2nd   *\
    |* has no data... this allows the dma ping-pong mode to stop automatically (on an  *|
	|* invalid descriptor).  For transfers requiring more than the initial two         *|
	|* descriptors, the last submit (made during a DMA interrupt) will be the invalid  *|
	|* descriptor (stopping the DMA), but that will have been submitted during the     *|
	|* last DMA interrupt, not as part of this initial descriptor prefill request.     */

    /* always begin with the primary dma descriptor */
    hDevice->nextDmaDescrType = ADI_DMA_CCD_PRIMARY;

    /* reset all prim/alt descriptor selectors from any previous transfers  */
    /* so the primary descriptor is active at the start of each transaction */
    pADI_DMA->DMAALTCLR = (

                          (0x1ul << hDevice->DmaMasterTxChannelNum)
#if (ADI_I2C_CFG_SLAVE_MODE_SUPPORT == 1)
                        | (0x1ul << hDevice->DmaSlaveRxChannelNum)
                        | (0x1ul << hDevice->DmaSlaveTxChannelNum)
#endif /* ADI_I2C_CFG_SLAVE_MODE_SUPPORT */
                        );

    /* initialize common DMA descriptor variables */
    hDevice->pDmaDescriptor->Chan       = chan;
    hDevice->pDmaDescriptor->DataWidth  = ADI_DMA_WIDTH_BYTE;
    hDevice->pDmaDescriptor->Protection = ADI_DMA_HPROT_NONE;
    hDevice->pDmaDescriptor->ArbitRate  = ADI_DMA_RPOWER_1;

    /* prepare 1st descriptor... */

    if (hDevice->dataAddressWidth) {

        /* use 1st descriptor for addressing phase if we have one... */

        /* byte-reverse the data address               */
        /* so DMA engine sends it MS first.            */
        /* (onMasterTXREQ() does this in non-dma mode) */
        uint8_t len = hDevice->dataAddressWidth;
        uint32_t src = hDevice->dataAddress;
        uint32_t dst = 0;
        for (int i = 0; i < len; i++) {
            dst <<= 8;
            dst |= ((src >> i*8) & 0xff);  /* local... no critical section needed */
        }
        hDevice->dataAddress = dst;

        hDevice->pNextDmaAddress      = (uint8_t*) &hDevice->dataAddress;
        hDevice->nUnscheduledDmaCount = hDevice->dataAddressWidth;

    } else {

        /* no addressing phase, so use 1st descriptor for data phase... */
        switch (chan) {

            case I2CM_CHANn:  /* shared master tx/rx channel for ADuCM350 only */

                /* differentiate by flag */
                hDevice->pNextDmaAddress      = (hDevice->bMasterTxRxDmaSharingFlag) ? hDevice->pTxBuffer   : hDevice->pRxBuffer;
                hDevice->nUnscheduledDmaCount = (hDevice->bMasterTxRxDmaSharingFlag) ? hDevice->TxRemaining : hDevice->RxRemaining;
                break;

            case I2CM_TX_CHANn:  /* dedicated master transmit */

#if (ADI_I2C_CFG_SLAVE_MODE_SUPPORT == 1)
            case I2CS_TX_CHANn:  /* dedicated slave transmit */
#endif /* ADI_I2C_CFG_SLAVE_MODE_SUPPORT */

                hDevice->pNextDmaAddress      = hDevice->pTxBuffer;
                hDevice->nUnscheduledDmaCount = hDevice->TxRemaining;
                break;

            case I2CM_RX_CHANn:  /* dedicated master receive */

#if (ADI_I2C_CFG_SLAVE_MODE_SUPPORT == 1)
            case I2CS_RX_CHANn:  /* dedicated slave receive */
#endif /* ADI_I2C_CFG_SLAVE_MODE_SUPPORT */

                hDevice->pNextDmaAddress      = hDevice->pRxBuffer;
                hDevice->nUnscheduledDmaCount = hDevice->RxRemaining;
                break;
        } /* end of channel switch */
    } /* end of data phase */

    /* submit 1st descriptor (be it address or data phase) */
    scheduleNextDescriptor(hDevice);


    /* prepare 2nd descriptor... */

    /* The 2nd descriptor will always be a data phase because any addressing *\
    |* phase will have already been dispensed with by the 1st descriptor.    *|
    |* If the 1st descriptor was a data phase, then there is no more setup   *|
    |* to do and we can just go ahead and submit the next descriptor using   *|
    \* the control variables already in place.                               */

	/* if the 1st phase was addressing, transition to data phase */
    if (hDevice->dataAddressWidth) {

        /* mark addressing phase done */
        hDevice->dataAddressWidth = ADI_I2C_NO_DATA_ADDRESSING_PHASE;

        /* initialize data phase variables */
        switch (chan) {

            case I2CM_CHANn:  /* shared master tx/rx channel for ADuCM350 only */

                /* differentiate by flag */
                hDevice->pNextDmaAddress      = (hDevice->bMasterTxRxDmaSharingFlag) ? hDevice->pTxBuffer   : hDevice->pRxBuffer;
                hDevice->nUnscheduledDmaCount = (hDevice->bMasterTxRxDmaSharingFlag) ? hDevice->TxRemaining : hDevice->RxRemaining;
                break;

            case I2CM_TX_CHANn:  /* dedicated master transmit */

#if (ADI_I2C_CFG_SLAVE_MODE_SUPPORT == 1)
            case I2CS_TX_CHANn:  /* dedicated slave transmit */
#endif /* ADI_I2C_CFG_SLAVE_MODE_SUPPORT */

                hDevice->pNextDmaAddress      = hDevice->pTxBuffer;
                hDevice->nUnscheduledDmaCount = hDevice->TxRemaining;
                break;

            case I2CM_RX_CHANn:  /* dedicated master receive */

#if (ADI_I2C_CFG_SLAVE_MODE_SUPPORT == 1)
            case I2CS_RX_CHANn:  /* dedicated slave receive */
#endif /* ADI_I2C_CFG_SLAVE_MODE_SUPPORT */

                hDevice->pNextDmaAddress      = hDevice->pRxBuffer;
                hDevice->nUnscheduledDmaCount = hDevice->RxRemaining;
                break;
        } /* end of channel switch */
    } /* end of transition to data phase */

    /* submit 2nd descriptor (always data phase) */
    scheduleNextDescriptor(hDevice);

    return ADI_I2C_SUCCESS;
}


/* called both by DMA pipeline prefill code and by DMA interupts to schedule and submit the next DMA descriptor */
static ADI_I2C_RESULT_TYPE scheduleNextDescriptor(ADI_I2C_DEV_HANDLE const hDevice) {

    register ADI_DMA_TRANSFER_TYPE* pD = hDevice->pDmaDescriptor;
    register uint32_t nextChunkSize;

        /* prepare next descriptor */
        switch (pD->Chan) {

	/* this case is unique to the ADuCM350, in which master mode combines TX & RX */
            case I2CM_CHANn:

                /* differentiate tx/rx by flag */
                if (hDevice->bMasterTxRxDmaSharingFlag) {
                    /* master transmit */
                    pD->pSrcData = hDevice->pNextDmaAddress;
                    pD->SrcInc   = ADI_DMA_INCR_BYTE;
                    pD->pDstData = (void*) &hDevice->pDev->I2CMTX;
                    pD->DstInc   = ADI_DMA_INCR_NONE;
                } else {
                    /* master receive */
                    pD->pSrcData = (void*) &hDevice->pDev->I2CMRX;
                    pD->SrcInc   = ADI_DMA_INCR_NONE;
                    pD->pDstData = hDevice->pNextDmaAddress;
                    pD->DstInc   = ADI_DMA_INCR_BYTE;
                }
                break;

            case I2CM_TX_CHANn:  /* dedicated master transmit */
                pD->pSrcData = hDevice->pNextDmaAddress;
                pD->SrcInc   = ADI_DMA_INCR_BYTE;
                pD->pDstData = (void*) &hDevice->pDev->I2CMTX;
                pD->DstInc   = ADI_DMA_INCR_NONE;
                break;

            case I2CM_RX_CHANn:  /* dedicated master receive */
                pD->pSrcData = (void*) &hDevice->pDev->I2CMRX;
                pD->SrcInc   = ADI_DMA_INCR_NONE;
                pD->pDstData = hDevice->pNextDmaAddress;
                pD->DstInc   = ADI_DMA_INCR_BYTE;
                break;

#if (ADI_I2C_CFG_SLAVE_MODE_SUPPORT == 1)

            case I2CS_TX_CHANn:  /* dedicated slave transmit */

                pD->pSrcData = hDevice->pNextDmaAddress;
                pD->SrcInc   = ADI_DMA_INCR_BYTE;
                pD->pDstData = (void*) &hDevice->pDev->I2CSTX;
                pD->DstInc   = ADI_DMA_INCR_NONE;
                break;

            case I2CS_RX_CHANn:  /* dedicated slave receive */

                pD->pSrcData = (void*) &hDevice->pDev->I2CSRX;
                pD->SrcInc   = ADI_DMA_INCR_NONE;
                pD->pDstData = hDevice->pNextDmaAddress;
                pD->DstInc   = ADI_DMA_INCR_BYTE;
                break;

#endif /* ADI_I2C_CFG_SLAVE_MODE_SUPPORT */
        }

    /* get the next descriptor type */
    pD->CCD = hDevice->nextDmaDescrType;

    /* switch it for next time */
    hDevice->nextDmaDescrType = (ADI_DMA_CCD_PRIMARY == hDevice->nextDmaDescrType)
        ? ADI_DMA_CCD_ALTERNATE
        : ADI_DMA_CCD_PRIMARY;

    /* update next descriptor */
    if (nextChunkSize = MIN(hDevice->nUnscheduledDmaCount, ADI_DMA_MAX_TRANSFER_SIZE)) {

        /* non-zero case */
        pD->Mode                       = ADI_DMA_MODE_PINGPONG;
        pD->DataLength                 = nextChunkSize;
        hDevice->pNextDmaAddress      += nextChunkSize;
        hDevice->nUnscheduledDmaCount -= nextChunkSize;

    } else {

        /* zero case (stop condition) */
        pD->Mode                       = ADI_DMA_MODE_INVALID;
        pD->DataLength                 = 0;
        hDevice->pNextDmaAddress       = NULL;
        hDevice->nUnscheduledDmaCount  = 0;
    }

    /* submit next descriptor */
    if (ADI_DMA_SUCCESS != adi_DMA_SubmitTransfer(pD))
        return ADI_I2C_ERR_BAD_DMA_CONFIG;

    return ADI_I2C_SUCCESS;
}


/* Common DMA Handler for non-dma mode (PIO) */
void fPioCommonHandler(ADI_I2C_DEV_HANDLE const hDevice ,DMA_CHANn_TypeDef chan) {

    /*
        No dma interrupts should ever come through this path.
        Only here for symmetry with the fDmaCommonHandler()
        call, as dispatched through function pointer table
        pfDmaHandlerFcn function.  Implimented as an error trap
        to catch logic error and avoid silent failure.
    */
    while (1)
    	;
}


/* Common DMA Handler code */
void fDmaCommonHandler(ADI_I2C_DEV_HANDLE const hDevice, DMA_CHANn_TypeDef chan) {

	/* no need for critical sections around register   *\
	\* read-modify-writes because we're in an interrupt */

    /* get both descriptor modes */
    uint32_t pri_mode = (&pADI_DCCPRI[chan])->DMACDC & DMA_CCD_CYCLE_MASK;
    uint32_t alt_mode = (&pADI_DCCALT[chan])->DMACDC & DMA_CCD_CYCLE_MASK;

    /* if both descriptors are done... */
    if (0 == pri_mode && 0 == alt_mode) {

        /* wait for controller to idle and disable the dma interrupt channel */
        switch (chan) {

	/* this case is unique to the ADuCM350, in which master mode combines TX & RX */

            case I2CM_CHANn:

                /* differentiate by flag */
                if (hDevice->bMasterTxRxDmaSharingFlag)
                    hDevice->pDev->I2CMCON &= ~I2CMCON_TXDMA;
                else
                    hDevice->pDev->I2CMCON &= ~I2CMCON_RXDMA;
                break;

            case I2CM_TX_CHANn:  /* dedicated master transmit */
                while (MASTER_BUSY)
                	;
                hDevice->pDev->I2CMCON &= ~I2CMCON_TXDMA;
                break;
            case I2CM_RX_CHANn:  /* dedicated master receive */
                while (MASTER_BUSY)
                	;
                hDevice->pDev->I2CMCON &= ~I2CMCON_RXDMA;
                break;

#if (ADI_I2C_CFG_SLAVE_MODE_SUPPORT == 1)

            case I2CS_TX_CHANn:  /* dedicated slave transmit */

                while (SLAVE_BUSY)
                	;
                hDevice->pDev->I2CSCON &= ~I2CSCON_TXDMA;
                break;

            case I2CS_RX_CHANn:  /* dedicated slave receive */

		while (SLAVE_BUSY)
			;
		hDevice->pDev->I2CSCON &= ~I2CSCON_RXDMA;
		break;

#endif /* ADI_I2C_CFG_SLAVE_MODE_SUPPORT */

        }

		/* complete the wait mode */
		BOOLEAN_UNWAIT (hDevice, bDmaComplete);

#if (ADI_I2C_CFG_ENABLE_CALLBACK_SUPPORT==1)
		/* issue the callback */
		if(hDevice->pfCallback){
			hDevice->pfCallback(hDevice->pcbParam, ADI_I2C_EVENT_DMA_COMPLETE, (void*)chan);
		}
#endif /* ADI_I2C_CFG_ENABLE_CALLBACK_SUPPORT */

    /* if only one descriptor is complete... */
    } else if (0 == pri_mode || 0 == alt_mode) {

        /* schedule another as needed, until both are done */
        if (hDevice->nUnscheduledDmaCount)
            scheduleNextDescriptor(hDevice);

    } else {
        /* spurious interrupt... ignore */
    }
}



#if (ADI_I2C_CFG_SLAVE_MODE_SUPPORT == 1)

/*!
 * @brief  I2C0 DMA Slave Transmit Interrupt Handler.
 *
 * @return void.
 *
 * Override of default (WEAK) I2C0 DMA slave transmit interrupt handler.
 */
ADI_INT_HANDLER (DMA_I2C0_STX_Int_Handler) {

    pI2C0_DevData->FunctionTable.pfDmaHandlerFcn(pI2C0_DevData, I2CS_TX_CHANn);
}


/*!
 * @brief  I2C0 DMA Slave Receive Interrupt Handler.
 *
 * @return void.
 *
 * Override of default (WEAK) I2C0 DMA slave receive interrupt handler.
 */
ADI_INT_HANDLER (DMA_I2C0_SRX_Int_Handler) {

    pI2C0_DevData->FunctionTable.pfDmaHandlerFcn(pI2C0_DevData, I2CS_RX_CHANn);
}

#endif /* ADI_I2C_CFG_SLAVE_MODE_SUPPORT */


/*!
 * @brief  I2C0 Shared DMA Master TX/RX Interrupt Handler for ADuCM350.
 *
 * @return void.
 *
 * Override of default (WEAK) I2C0 DMA master tx/rx interrupt handler.
 */
ADI_INT_HANDLER (DMA_I2C0_MX_Int_Handler) {

     pI2C0_DevData->FunctionTable.pfDmaHandlerFcn(pI2C0_DevData, I2CM_CHANn);
}


#endif /* ADI_I2C_CFG_ENABLE_DMA_SUPPORT */

#define REVERT_ADI_MISRA_SUPPRESSIONS  /*!< Revert ADI MISRA Suppressions */
#include "misra.h"

/*
** EOF
*/

/*@}*/
