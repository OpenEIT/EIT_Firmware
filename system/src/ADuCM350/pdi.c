/*********************************************************************************

Copyright (c) 2010-2014 Analog Devices, Inc. All Rights Reserved.

This software is proprietary to Analog Devices, Inc. and its licensors.  By using 
this software you agree to the terms of the associated Analog Devices Software 
License Agreement (See /LICENSES/ADI.LABLAB.LICENSE). 

Modified from the Analog Devices original.
Portions Copyright (c) 2018 Andrew Grosser, Moritz von Buttlar.

*********************************************************************************/

/*!
 *****************************************************************************
 * @file:    pdi.c
 * @brief:   PDI Device Implementations for ADuCxxx
 * @version: $Revision: 30127 $
 * @date:    $Date: 2015-03-09 12:05:31 -0400 (Mon, 09 Mar 2015) $
 *****************************************************************************/

/*! \addtogroup PDI_Driver PDI Driver
 *  @{
 */

#define ASSERT_ADI_MISRA_SUPPRESSIONS  /*!< Apply ADI MISRA Suppressions */
#include "misra.h"

#include <stdlib.h>  /* for 'NULL" definition */
#include "pdi.h"     /* generic PDI include */


/*! \struct ADI_PDI_DEV_DATA_TYPE
 * PDI Device instance data structure */
typedef struct ADI_PDI_DEV_DATA_TYPE
{

    /* device attributes */
    ADI_PDI_STATE_TYPE        InitState;        /*!< track initialization state */
    ADI_PDI_TypeDef          *pPdi;             /*!< MMR address for this PDI   */

} ADI_PDI_DEV_DATA_TYPE;


/* allocate instance data array */
ADI_PDI_DEV_DATA_TYPE PDI_DevData[ADI_PDI_MAX_DEVID] = {0};  /*!< Private PDI device driver instance data */


/* handle checker for debug mode */
#ifdef ADI_DEBUG

#define ADI_PDI_INVALID_HANDLE(h) (&PDI_DevData[ADI_PDI_DEVID_0] != h)

#endif

/* prototype for the required MIPI command translation function implemented in the device-specific display controller layer */
extern ADI_PDI_RESULT_TYPE adi_TranslateMipiCommand (ADI_PDI_DEV_HANDLE const hDevice, const ADI_MIPI_DCS_ENUM MipiCmd, const void* pMipiParams);

/* create static initialization data */
typedef struct {
	uint32_t pdiInterruptEnable;   /*!< PDI interrupt enable register */
	uint32_t pdiInterruptDisable;  /*!< PDI interrupt disable register */
	uint32_t pdiInterruptClear;    /*!< PDI interrupt status clear register */
	uint32_t pdiTimingControl;     /*!< PDI timing register */
} ADI_PDI_STATIC_INIT_TYPE;

static const ADI_PDI_STATIC_INIT_TYPE gPdiStaticInitData[ADI_PDI_MAX_DEVID] = {
	/* single instance of PDI device */
	{
		/* interrupt enable register */
		( (PDI_INTERRUPT_ENABLE_FIFO_UNDERFLOW      << BITP_PDI_PDI_INT_SET_SET_FIFO_UDF_IEN)
		| (PDI_INTERRUPT_ENABLE_FIFO_OVERLFLOW      << BITP_PDI_PDI_INT_SET_SET_FIFO_OVF_IEN)
		| (PDI_INTERRUPT_ENABLE_CMD_WRITE_ERROR     << BITP_PDI_PDI_INT_SET_SET_CMD_WR_ERR_IEN)
		| (PDI_INTERRUPT_ENABLE_CMD_DONE            << BITP_PDI_PDI_INT_SET_SET_CMD_DONE_IEN)
		| (PDI_INTERRUPT_ENABLE_FIFO_DONE           << BITP_PDI_PDI_INT_SET_SET_DONE_IEN)
		| (PDI_INTERRUPT_ENABLE_TEAR_EFFECT         << BITP_PDI_PDI_INT_SET_SET_TE_IEN)
		| (PDI_INTERRUPT_ENABLE_RECEIVE             << BITP_PDI_PDI_INT_SET_SET_RX_IEN)
		| (PDI_INTERRUPT_ENABLE_TRANSMIT            << BITP_PDI_PDI_INT_SET_SET_TX_IEN)
		),

		/* interrupt disable register*/
		( (PDI_INTERRUPT_DISABLE_FIFO_UNDERFLOW     << BITP_PDI_PDI_INT_CLR_CLR_FIFO_UDF_IEN)
		| (PDI_INTERRUPT_DISABLE_FIFO_OVERLFLOW     << BITP_PDI_PDI_INT_CLR_CLR_FIFO_OVF_IEN)
		| (PDI_INTERRUPT_DISABLE_CMD_WRITE_ERROR    << BITP_PDI_PDI_INT_CLR_CLR_CMD_WR_ERR_IEN)
		| (PDI_INTERRUPT_DISABLE_CMD_DONE           << BITP_PDI_PDI_INT_CLR_CLR_CMD_DONE_IEN)
		| (PDI_INTERRUPT_DISABLE_FIFO_DONE          << BITP_PDI_PDI_INT_CLR_CLR_DONE_IEN)
		| (PDI_INTERRUPT_DISABLE_TEAR_EFFECT        << BITP_PDI_PDI_INT_CLR_CLR_TE_IEN)
		| (PDI_INTERRUPT_DISABLE_RECEIVE            << BITP_PDI_PDI_INT_CLR_CLR_RX_IEN)
		| (PDI_INTERRUPT_DISABLE_TRANSMIT           << BITP_PDI_PDI_INT_CLR_CLR_TX_IEN)
		),

		/* status (interrupt clear) register*/
		( (PDI_INTERRUPT_CLEAR_FIFO_UNDERFLOW       << BITP_PDI_PDI_STAT_FIFO_UDF)
		| (PDI_INTERRUPT_CLEAR_FIFO_OVERLFLOW       << BITP_PDI_PDI_STAT_FIFO_OVF)
		| (PDI_INTERRUPT_CLEAR_CMD_WRITE_ERROR      << BITP_PDI_PDI_STAT_CMD_WR_ERR)
		| (PDI_INTERRUPT_CLEAR_CMD_DONE             << BITP_PDI_PDI_STAT_CMD_DONE)
		| (PDI_INTERRUPT_CLEAR_FIFO_DONE            << BITP_PDI_PDI_STAT_DONE_IRQ)
		| (PDI_INTERRUPT_CLEAR_TEAR_EFFECT          << BITP_PDI_PDI_STAT_TE_IRQ)
		),

		/* timing control register */
		( (PDI_TIMING_CHIP_SELECT_SETUP_TIME        << BITP_PDI_PDI_IF_TIMING_TCSH)
		| (PDI_TIMING_READ_WRITE_CONTROL_LOW_PULSE  << BITP_PDI_PDI_IF_TIMING_TWRL_RDL)
		| (PDI_TIMING_READ_WRITE_CONTROL_HIGH_PULSE << BITP_PDI_PDI_IF_TIMING_TWRH_RDH)
		| (PDI_TIMING_CHIP_SELECT_HOLD_TIME         << BITP_PDI_PDI_IF_TIMING_TCSF)
		| (PDI_TIMING_ADDRESS_SETUP_HOLD_TIME       << BITP_PDI_PDI_IF_TIMING_TAS_AH)
		),
	}
};


/*
    //////////////////////////////////////////////////////////////////////////////
    //////////////////////   API IMPLEMENTATIONS   ///////////////////////////////
    //////////////////////////////////////////////////////////////////////////////
*/


/*!
 * @brief  Initialize and allocate a PDI device.
 *
 * @param[in]        devID      Zero-based device index designating which PDI device to initialize.
 * @param[out]       phDevice   The caller's device handle pointer for storing the initialized device instance data pointer.
 *
 * @return            Status
 *                    - #ADI_PDI_SUCCESS                        Call completed successfully.
 *                    - #ADI_PDI_ERR_ALREADY_INITIALIZED [D]    Specified device has already been initialized.
 *                    - #ADI_PDI_ERR_BAD_DEV_ID [D]             Invalid device index.
 *
 * No other PDI APIs may be called until the device initialization function is called.
 *
 * Initialize a PDI device using internal default configuration settings and allocate the device for use.  The caller's
 * device handle is written with the device instance data pointer.  The returned device handle is required to be passed
 * to all subsequent calls to convey which device instance to operate on.
 *
 * @note    The contents of phDevice is set to NULL on failure.
 *
 * @sa      adi_PDI_UnInit().
 */
ADI_PDI_RESULT_TYPE adi_PDI_Init (ADI_PDI_DEV_ID_TYPE const devID, ADI_PDI_DEV_HANDLE* const phDevice)
{
    ADI_PDI_DEV_HANDLE hDevice = &PDI_DevData[devID];

    /* store a bad handle in case of failure */
    *phDevice = (ADI_PDI_DEV_HANDLE) NULL;

#ifdef ADI_DEBUG
    if (devID >= ADI_PDI_MAX_DEVID)
        return ADI_PDI_ERR_BAD_DEV_ID;

    if (ADI_PDI_STATE_UNINITIALIZED != hDevice->InitState)
        return ADI_PDI_ERR_ALREADY_INITIALIZED;
#endif

	hDevice->pPdi = pADI_PDI;

	/* assert statically configured initialization data, if present,
       in lieu of dynamic initializations */

	ADI_PDI_STATIC_INIT_TYPE const* pInitData = &gPdiStaticInitData[devID];

	hDevice->pPdi->PDI_INT_SET   = pInitData->pdiInterruptEnable;
	hDevice->pPdi->PDI_INT_CLR   = pInitData->pdiInterruptDisable;
	hDevice->pPdi->PDI_STAT      = pInitData->pdiInterruptClear;
	hDevice->pPdi->PDI_IF_TIMING = pInitData->pdiTimingControl;

    /* mark device fully initialized */
    hDevice->InitState = ADI_PDI_STATE_INITIALIZED;

    /* write the device data pointer into the caller's handle */
    *phDevice = hDevice;

    return ADI_PDI_SUCCESS;
}


/*!
 * @brief  Uninitialize and deallocate a PDI device.
 *
 * @param[in]   hDevice    Device handle obtained from adi_PDI_Init().
 *
 * @return      Status
 *                - #ADI_PDI_SUCCESS                    Call completed successfully.
 *                - #ADI_PDI_ERR_BAD_DEV_HANDLE [D]     Invalid device handle parameter.
 *                - #ADI_PDI_ERR_NOT_INITIALIZED [D]    Device has not been previously configured for use.
 *
 * Uninitialize and release an allocated PDI device for other use.
 *
 * @sa        adi_PDI_Init().
 */
ADI_PDI_RESULT_TYPE adi_PDI_UnInit (ADI_PDI_DEV_HANDLE const hDevice)
{

#ifdef ADI_DEBUG
    if (ADI_PDI_INVALID_HANDLE(hDevice))
        return ADI_PDI_ERR_BAD_DEV_HANDLE;

    if (hDevice->InitState != ADI_PDI_STATE_INITIALIZED)
        return ADI_PDI_ERR_NOT_INITIALIZED;
#endif

    /* invalidate initialization state */
    hDevice->InitState = ADI_PDI_STATE_UNINITIALIZED;

    return ADI_PDI_SUCCESS;
}


/*!
 * @brief  Set PDI configuration register.
 *
 * @param[in]      hDevice      Device handle obtained from adi_PDI_Init().
 * @param[in]      Config       Configuration register value to set.
 *
 * @return         Status
 *                - #ADI_PDI_SUCCESS                        Call completed successfully.
 *                - #ADI_PDI_ERR_BAD_DEV_HANDLE [D]         Invalid device handle parameter.
 *                - #ADI_PDI_ERR_NOT_INITIALIZED [D]        Device has not been previously configured for use.
 *
 * Write a new value to the PDI configuration register.
 *
 * @sa        adi_PDI_GetConfiguration().
 */
ADI_PDI_RESULT_TYPE adi_PDI_SetConfiguration (ADI_PDI_DEV_HANDLE const hDevice, const uint32_t Config)
{
#ifdef ADI_DEBUG
    if (ADI_PDI_INVALID_HANDLE(hDevice))
        return ADI_PDI_ERR_BAD_DEV_HANDLE;

    if (hDevice->InitState != ADI_PDI_STATE_INITIALIZED)
        return ADI_PDI_ERR_NOT_INITIALIZED ;
#endif

    pADI_PDI->PDI_CFG = Config;

    return ADI_PDI_SUCCESS;
}


/*!
 * @brief  Get PDI configuration register.
 *
 * @param[in]      hDevice      Device handle obtained from adi_PDI_Init().
 * @param[in]      pConfig      Pointer to caller's configuration register variable to write.
 *
 * @return         Status
 *                - #ADI_PDI_SUCCESS                        Call completed successfully.
 *                - #ADI_PDI_ERR_BAD_DEV_HANDLE [D]         Invalid device handle parameter.
 *                - #ADI_PDI_ERR_NOT_INITIALIZED [D]        Device has not been previously configured for use.
 *
 * Obtain current PDI configuration register value.
 *
 * @sa        adi_PDI_SetConfiguration().
 */
ADI_PDI_RESULT_TYPE adi_PDI_GetConfiguration (ADI_PDI_DEV_HANDLE const hDevice, uint32_t* const pConfig)
{
#ifdef ADI_DEBUG
    if (ADI_PDI_INVALID_HANDLE(hDevice))
        return ADI_PDI_ERR_BAD_DEV_HANDLE;

    if (hDevice->InitState != ADI_PDI_STATE_INITIALIZED)
        return ADI_PDI_ERR_NOT_INITIALIZED ;
#endif

    *pConfig = pADI_PDI->PDI_CFG;

    return ADI_PDI_SUCCESS;
}


/*!
 * @brief  Enable particular interrupts in the PDI controller.
 *
 * @param[in]      hDevice      Device handle obtained from adi_PDI_Init().
 * @param[in]      Enables      Bitfield of interrupts to enable.
 *
 * @return         Status
 *                - #ADI_PDI_SUCCESS                        Call completed successfully.
 *                - #ADI_PDI_ERR_BAD_DEV_HANDLE [D]         Invalid device handle parameter.
 *                - #ADI_PDI_ERR_NOT_INITIALIZED [D]        Device has not been previously configured for use.
 *
 * Enable specific PDI interrupts according to bits set in the \a Enables parameter bitfield.  Bits not set have no effect.
 *
 * @note Note: PDI interrupts are not not handled in this top-layer PDI driver.  They are left for the device-specific layer
 * to handle as needed (PDI device-specific implimentations are collected under the .../src/mipi directory).
 *
 * @note \b MIPI refers to the Mobile Industry Processor Interface standard.
 * @note \b DCS refers to the MIPI Alliance Specification for Display Command Set (Version 1.02.00 - 23 July 2009).
 *
 * @sa        adi_PDI_InterruptDisable().
 */
ADI_PDI_RESULT_TYPE adi_PDI_InterruptEnable (ADI_PDI_DEV_HANDLE const hDevice, const uint32_t Enables)
{
#ifdef ADI_DEBUG
    if (ADI_PDI_INVALID_HANDLE(hDevice))
        return ADI_PDI_ERR_BAD_DEV_HANDLE;

    if (hDevice->InitState != ADI_PDI_STATE_INITIALIZED)
        return ADI_PDI_ERR_NOT_INITIALIZED ;
#endif

    pADI_PDI->PDI_INT_SET = Enables;

    return ADI_PDI_SUCCESS;
}


/*!
 * @brief  Disable particular interrupts in the PDI controller.
 *
 * @param[in]      hDevice      Device handle obtained from adi_PDI_Init().
 * @param[in]      Disables     Bitfield of interrupts to disable.
 *
 * @return         Status
 *                - #ADI_PDI_SUCCESS                        Call completed successfully.
 *                - #ADI_PDI_ERR_BAD_DEV_HANDLE [D]         Invalid device handle parameter.
 *                - #ADI_PDI_ERR_NOT_INITIALIZED [D]        Device has not been previously configured for use.
 *
 * Disable specific PDI interrupts according to bits set in the \a Disables parameter bitfield.  Bits not set have no effect.
 *
 * @sa        adi_PDI_InterruptEnable().
 */
ADI_PDI_RESULT_TYPE adi_PDI_InterruptDisable (ADI_PDI_DEV_HANDLE const hDevice, const uint32_t Disables)
{
#ifdef ADI_DEBUG
    if (ADI_PDI_INVALID_HANDLE(hDevice))
        return ADI_PDI_ERR_BAD_DEV_HANDLE;

    if (hDevice->InitState != ADI_PDI_STATE_INITIALIZED)
        return ADI_PDI_ERR_NOT_INITIALIZED ;
#endif

    pADI_PDI->PDI_INT_CLR = Disables;

    return ADI_PDI_SUCCESS;
}


/*!
 * @brief  Write the PDI status register.
 *
 * @param[in]      hDevice      Device handle obtained from adi_PDI_Init().
 * @param[in]      Status       Status register bits to write.
 *
 * @return         Status
 *                - #ADI_PDI_SUCCESS                        Call completed successfully.
 *                - #ADI_PDI_ERR_BAD_DEV_HANDLE [D]         Invalid device handle parameter.
 *                - #ADI_PDI_ERR_NOT_INITIALIZED [D]        Device has not been previously configured for use.
 *
 * The PDI status register contains various write-one-to-clear (W1C) "sticky" status bits that must be cleared explicitly through this API.
 * The sticky W1C status bits are set to indicate the source of the PDI shared interrupt and are typically cleared in the interrupt handler.
 * All bits in the status register are readable (using #adi_PDI_GetStatus()), some bits are readable and write-one-to-clear (RW1C),
 * and some bits are read-only.
 *
 * @sa        adi_PDI_GetStatus().
 */
ADI_PDI_RESULT_TYPE adi_PDI_SetStatus (ADI_PDI_DEV_HANDLE const hDevice, const uint32_t Status)
{
#ifdef ADI_DEBUG
    if (ADI_PDI_INVALID_HANDLE(hDevice))
        return ADI_PDI_ERR_BAD_DEV_HANDLE;

    if (hDevice->InitState != ADI_PDI_STATE_INITIALIZED)
        return ADI_PDI_ERR_NOT_INITIALIZED ;
#endif

    pADI_PDI->PDI_STAT = Status;

    return ADI_PDI_SUCCESS;
}


/*!
 * @brief  Read the PDI status register.
 *
 * @param[in]      hDevice      Device handle obtained from adi_PDI_Init().
 * @param[in]      pStatus      Pointer to caller's status register variable to write.
 *
 * @return         Status
 *                - #ADI_PDI_SUCCESS                        Call completed successfully.
 *                - #ADI_PDI_ERR_BAD_DEV_HANDLE [D]         Invalid device handle parameter.
 *                - #ADI_PDI_ERR_NOT_INITIALIZED [D]        Device has not been previously configured for use.
 *
 * Obtain current PDI status register value.
 *
 * @sa        adi_PDI_SetStatus().
 */
ADI_PDI_RESULT_TYPE adi_PDI_GetStatus (ADI_PDI_DEV_HANDLE const hDevice, uint32_t* const pStatus)
{
#ifdef ADI_DEBUG
    if (ADI_PDI_INVALID_HANDLE(hDevice))
        return ADI_PDI_ERR_BAD_DEV_HANDLE;

    if (hDevice->InitState != ADI_PDI_STATE_INITIALIZED)
        return ADI_PDI_ERR_NOT_INITIALIZED ;
#endif

    *pStatus = pADI_PDI->PDI_STAT;

    return ADI_PDI_SUCCESS;
}


/*!
 * @brief  Set PDI command register.
 *
 * @param[in]      hDevice      Device handle obtained from adi_PDI_Init().
 * @param[in]      ParamCount   Number of control parameters (FIFO elements) associated with Command.
 * @param[in]      Type         Type of parameter (control/data).
 * @param[in]      Dir          Command direction (read/write).
 * @param[in]      Command      \b Display \b Bus \b Interface (DBI) command value.
 *
 * @return         Status
 *                - #ADI_PDI_SUCCESS                        Call completed successfully.
 *                - #ADI_PDI_ERR_BAD_DEV_HANDLE [D]         Invalid device handle parameter.
 *                - #ADI_PDI_ERR_NOT_INITIALIZED [D]        Device has not been previously configured for use.
 *
 * Write the PDI command register.  Parameter \a ParamCount designates how many command parameters.  If there
 * are zero parameters, \a Command executes immediately.  If there are non-zero parameters, \a Command is executed
 * after the \a paramCount parameters are pushed into the parameter FIFO (#adi_PDI_PushParameterFIFO()).
 *
 * \a Type differentiates between control vs. frame data (possibly, DMA) command parameters.  Frame data may use
 * DMA and requires the frame data count register to be set as well (#adi_PDI_SetFrameDataCount()).  The \a Dir
 * parameter controls the data direction of the frame data transfer.
 *
 * @note \b MIPI refers to the Mobile Industry Processor Interface standard.
 * @note \b DBI refers to the MIPI Alliance Standard for Display Bus Interface (Version 2.00 - 29 November 2005).
 *
 * @sa        adi_PDI_GetCommand().
 * @sa        adi_PDI_PushParameterFIFO().
 * @sa        adi_PDI_SetFrameDataCount().
 */
ADI_PDI_RESULT_TYPE adi_PDI_SetCommand (ADI_PDI_DEV_HANDLE const hDevice,
                                        const uint8_t ParamCount,
                                        const ADI_PDI_CMD_PARAM_TYPE Type,
                                        const ADI_PDI_CMD_DIR_TYPE Dir,
                                        const uint8_t Command)
{
    uint32_t pdiCmd;

#ifdef ADI_DEBUG
    if (ADI_PDI_INVALID_HANDLE(hDevice))
        return ADI_PDI_ERR_BAD_DEV_HANDLE;

    if (hDevice->InitState != ADI_PDI_STATE_INITIALIZED)
        return ADI_PDI_ERR_NOT_INITIALIZED ;
#endif

    /* build the command locally */
    pdiCmd = Command | (ParamCount << ADI_PDI_CMD_PARAM_COUNT_POS);

    /* set the frame data bit if its frame data */
    if (PDI_CMD_DATA == Type)
        pdiCmd |= ADI_PDI_CMD_FRAME_DATA;

    /* set the write bit if its a write */
    if (PDI_DIR_WRITE == Dir)
        pdiCmd |= ADI_PDI_CMD_DIRECTION_WRITE;

    /* clear CMD_DONE status prior to command write */
    /* don't clear CMD_WR_ERR... let it accumulate in case of parameter count mismatch */
    pADI_PDI->PDI_STAT = ADI_PDI_STATUS_CMD_DONE;

    /* write the command register */
    pADI_PDI->PDI_CMD = pdiCmd;

	/* the required command wait is done in the device-specific command *\
    \* translation switch statement, after each call to this API        */

    return ADI_PDI_SUCCESS;
}


/*!
 * @brief  Get the PDI command register.
 *
 * @param[in]      hDevice      Device handle obtained from adi_PDI_Init().
 * @param[in]      pCommand     Pointer to caller's command register variable to write.
 *
 * @return         Status
 *                - #ADI_PDI_SUCCESS                        Call completed successfully.
 *                - #ADI_PDI_ERR_BAD_DEV_HANDLE [D]         Invalid device handle parameter.
 *                - #ADI_PDI_ERR_NOT_INITIALIZED [D]        Device has not been previously configured for use.
 *
 * Obtain current PDI command register value.
 *
 * @sa        adi_PDI_SetCommand().
 */
ADI_PDI_RESULT_TYPE adi_PDI_GetCommand (ADI_PDI_DEV_HANDLE const hDevice, uint8_t* const pCommand)
{
#ifdef ADI_DEBUG
    if (ADI_PDI_INVALID_HANDLE(hDevice))
        return ADI_PDI_ERR_BAD_DEV_HANDLE;

    if (hDevice->InitState != ADI_PDI_STATE_INITIALIZED)
        return ADI_PDI_ERR_NOT_INITIALIZED ;
#endif

    *pCommand = pADI_PDI->PDI_CMD;

    return ADI_PDI_SUCCESS;
}


/*!
 * @brief  Set PDI frame data count register.
 *
 * @param[in]      hDevice      Device handle obtained from adi_PDI_Init().
 * @param[in]      Count        Number of frame elements to set.
 *
 * @return         Status
 *                - #ADI_PDI_SUCCESS                        Call completed successfully.
 *                - #ADI_PDI_ERR_BAD_DEV_HANDLE [D]         Invalid device handle parameter.
 *                - #ADI_PDI_ERR_NOT_INITIALIZED [D]        Device has not been previously configured for use.
 *
 * Write a new value to the PDI DBI-based (\b Display \b Bus \b Interface) frame data count register.
 * The frame data count controls the number of transfers (possibly, DMA) associated with a particular DBI command.
 *
 * @note \b MIPI refers to the Mobile Industry Processor Interface standard.
 * @note \b DBI refers to the MIPI Alliance Standard for Display Bus Interface (Version 2.00 - 29 November 2005)
 *
 * @sa        adi_PDI_GetFrameDataCount().
 * @sa        adi_PDI_SetCommand().
 */
ADI_PDI_RESULT_TYPE adi_PDI_SetFrameDataCount (ADI_PDI_DEV_HANDLE const hDevice, const uint32_t Count)
{
#ifdef ADI_DEBUG
    if (ADI_PDI_INVALID_HANDLE(hDevice))
        return ADI_PDI_ERR_BAD_DEV_HANDLE;

    if (hDevice->InitState != ADI_PDI_STATE_INITIALIZED)
        return ADI_PDI_ERR_NOT_INITIALIZED ;
#endif

    pADI_PDI->PDI_FRDATA_N = Count;

    return ADI_PDI_SUCCESS;
}


/*!
 * @brief  Get PDI frame data count register.
 *
 * @param[in]      hDevice      Device handle obtained from adi_PDI_Init().
 * @param[in]      pCount       Pointer to caller's frame data count register variable to write.
 *
 * @return         Status
 *                - #ADI_PDI_SUCCESS                        Call completed successfully.
 *                - #ADI_PDI_ERR_BAD_DEV_HANDLE [D]         Invalid device handle parameter.
 *                - #ADI_PDI_ERR_NOT_INITIALIZED [D]        Device has not been previously configured for use.
 *
 * Obtain current PDI frame data count value.
 *
 * @sa        adi_PDI_SetFrameDataCount().
 */
ADI_PDI_RESULT_TYPE adi_PDI_GetFrameDataCount (ADI_PDI_DEV_HANDLE const hDevice, uint32_t* const pCount)
{
#ifdef ADI_DEBUG
    if (ADI_PDI_INVALID_HANDLE(hDevice))
        return ADI_PDI_ERR_BAD_DEV_HANDLE;

    if (hDevice->InitState != ADI_PDI_STATE_INITIALIZED)
        return ADI_PDI_ERR_NOT_INITIALIZED ;
#endif

    *pCount = pADI_PDI->PDI_FRDATA_N;

    return ADI_PDI_SUCCESS;
}


/*!
 * @brief  Push PDI parameter FIFO register.
 *
 * @param[in]      hDevice      Device handle obtained from adi_PDI_Init().
 * @param[in]      Param        Parameter to push.
 *
 * @return         Status
 *                - #ADI_PDI_SUCCESS                        Call completed successfully.
 *                - #ADI_PDI_ERR_BAD_DEV_HANDLE [D]         Invalid device handle parameter.
 *                - #ADI_PDI_ERR_NOT_INITIALIZED [D]        Device has not been previously configured for use.
 *
 * Transmit parameters associated with a particular PDI command.  Some write commands have zero sized parameter
 * lists and have no need to push parameters.  Transmit commands with non-zero sized parameter lists use the
 * parameter FIFO to transmit command parameters.
 *
 * @sa        adi_PDI_PullParameterFIFO().
 * @sa        adi_PDI_SetCommand().
 */
ADI_PDI_RESULT_TYPE adi_PDI_PushParameterFIFO (ADI_PDI_DEV_HANDLE const hDevice, const uint32_t Param)
{
#ifdef ADI_DEBUG
    if (ADI_PDI_INVALID_HANDLE(hDevice))
        return ADI_PDI_ERR_BAD_DEV_HANDLE;

    if (hDevice->InitState != ADI_PDI_STATE_INITIALIZED)
        return ADI_PDI_ERR_NOT_INITIALIZED ;
#endif

    pADI_PDI->PDI_FIFO = Param;

    return ADI_PDI_SUCCESS;
}


/*!
 * @brief  Pull (pop) PDI parameter FIFO register.
 *
 * @param[in]      hDevice      Device handle obtained from adi_PDI_Init().
 * @param[in]      pParam       Pointer to caller's FIFO parameter variable to write.
 *
 * @return         Status
 *                - #ADI_PDI_SUCCESS                        Call completed successfully.
 *                - #ADI_PDI_ERR_BAD_DEV_HANDLE [D]         Invalid device handle parameter.
 *                - #ADI_PDI_ERR_NOT_INITIALIZED [D]        Device has not been previously configured for use.
 *
 * Read parameters associated with a particular PDI command.  Some read commands have zero sized parameter
 * lists and have no need to pull parameters.  Read commands with non-zero sized parameter lists use the
 * parameter FIFO to read command parameters.
 *
 * @sa        adi_PDI_PushParameterFIFO().
 * @sa        adi_PDI_SetCommand().
 */
ADI_PDI_RESULT_TYPE adi_PDI_PullParameterFIFO (ADI_PDI_DEV_HANDLE const hDevice, uint32_t* const pParam)
{
#ifdef ADI_DEBUG
    if (ADI_PDI_INVALID_HANDLE(hDevice))
        return ADI_PDI_ERR_BAD_DEV_HANDLE;

    if (hDevice->InitState != ADI_PDI_STATE_INITIALIZED)
        return ADI_PDI_ERR_NOT_INITIALIZED ;
#endif

    *pParam = pADI_PDI->PDI_FIFO;

    return ADI_PDI_SUCCESS;
}


/*!
 * @brief  Set PDI interface timing register.
 *
 * @param[in]      hDevice      Device handle obtained from adi_PDI_Init().
 * @param[in]      Timing       Timing  register value to set.
 *
 * @return         Status
 *                - #ADI_PDI_SUCCESS                        Call completed successfully.
 *                - #ADI_PDI_ERR_BAD_DEV_HANDLE [D]         Invalid device handle parameter.
 *                - #ADI_PDI_ERR_NOT_INITIALIZED [D]        Device has not been previously configured for use.
 *
 * Write a new value to the PDI timing register.
 *
 * @sa        adi_PDI_GetInterfaceTiming().
 */
ADI_PDI_RESULT_TYPE adi_PDI_SetInterfaceTiming (ADI_PDI_DEV_HANDLE const hDevice, const uint32_t Timing)
{
#ifdef ADI_DEBUG
    if (ADI_PDI_INVALID_HANDLE(hDevice))
        return ADI_PDI_ERR_BAD_DEV_HANDLE;

    if (hDevice->InitState != ADI_PDI_STATE_INITIALIZED)
        return ADI_PDI_ERR_NOT_INITIALIZED ;
#endif

    pADI_PDI->PDI_IF_TIMING = Timing;

    return ADI_PDI_SUCCESS;
}


/*!
 * @brief  Get PDI interface timing register.
 *
 * @param[in]      hDevice      Device handle obtained from adi_PDI_Init().
 * @param[in]      pTiming      Pointer to caller's timing register variable to write.
 *
 * @return         Status
 *                - #ADI_PDI_SUCCESS                        Call completed successfully.
 *                - #ADI_PDI_ERR_BAD_DEV_HANDLE [D]         Invalid device handle parameter.
 *                - #ADI_PDI_ERR_NOT_INITIALIZED [D]        Device has not been previously configured for use.
 *
 * Obtain current PDI interface timing value.
 *
 * @sa        adi_PDI_GetInterfaceTiming().
 */
ADI_PDI_RESULT_TYPE adi_PDI_GetInterfaceTiming (ADI_PDI_DEV_HANDLE const hDevice, uint32_t* const pTiming)
{
#ifdef ADI_DEBUG
    if (ADI_PDI_INVALID_HANDLE(hDevice))
        return ADI_PDI_ERR_BAD_DEV_HANDLE;

    if (hDevice->InitState != ADI_PDI_STATE_INITIALIZED)
        return ADI_PDI_ERR_NOT_INITIALIZED ;
#endif

    *pTiming = pADI_PDI->PDI_IF_TIMING;

    return ADI_PDI_SUCCESS;
}


/*!
 * @brief  Dispatch a MIPI-based command through the PDI interface.
 *
 * @param[in]      hDevice          Device handle obtained from adi_PDI_Init().
 * @param[in]      MipiCmd          The MIPI command identifier.
 * @param[in]      pMipiParamBlock  The MIPI command parameter block.
 *
 * @return         Status
 *                - #ADI_PDI_SUCCESS                        Call completed successfully.
 *                - #ADI_PDI_ERR_BAD_DEV_HANDLE [D]         Invalid device handle parameter.
 *                - #ADI_PDI_ERR_DMA_OPEN                   Failed to open DMA channel.
 *                - #ADI_PDI_ERR_DMA_SUBMIT                 Failed to submit a DMA descriptor block.
 *                - #ADI_PDI_ERR_NOT_INITIALIZED [D]        Device has not been previously configured for use.
 *                - #ADI_PDI_ERR_UNKNOWN_MIPI               Unrecognized MIPI command.
 *                - #ADI_PDI_ERR_UNSUPPORTED_MIPI           MIPI command not supported by device.
 *
 * Dispatch a standard MIPI command to an underlying device-specific translation layer for implementation
 * using native device-specific command sequence(s).  The native "translation layer" implements the required
 * device-specific \b adi_TranslateMipiCommand() function (sort of a virtual pure C++ notion) to receive and
 * translate the various MIPI command dispatch calls into native display device implimentations.
 *
 * The translation layer may or may not implement a particular MIPI command, depending on the
 * native feature set and capability.  Implementations and supported command sets vary with device.
 *
 * For command-mode operations, the \a pMipiParamBlock pointer points to a MIPI-defined block of 8-bit parameters
 * provided by the calling application, as documented in the MIPI DCS documentation.
 *
 * For data-mode operations, the \a pMipiParamBlock pointer is used by the native implementation as a (read/write)
 * data pointer (not a command block pointer).  In this case, the \a pMipiParamBlock should point to a properly-sized
 * data buffer belonging to the caller.  The native implimentation may use DMA for read/write transactions.
 *
 * @note Example implimentations for the device-specific translation layer implimenting the required
 * \b adi_TranslateMipiCommand() function may be found in under the "mipi" subdirectory (in the same "src"
 * directory as this "pdi.c" file), as well as mipi-based application example(s) in the main "examples" directory.
 *
 * @note \b MIPI refers to the Mobile Industry Processor Interface standard.
 * @note \b DCS refers to the MIPI Alliance Specification for Display Command Set (Version 1.02.00 - 23 July 2009).
 */
ADI_PDI_RESULT_TYPE adi_PDI_DispatchMipiCommand (ADI_PDI_DEV_HANDLE const hDevice, const ADI_MIPI_DCS_ENUM MipiCmd, const void* pMipiParamBlock)
{
    ADI_PDI_RESULT_TYPE result = ADI_PDI_SUCCESS;

#ifdef ADI_DEBUG
    if (ADI_PDI_INVALID_HANDLE(hDevice))
        return ADI_PDI_ERR_BAD_DEV_HANDLE;

    if (hDevice->InitState != ADI_PDI_STATE_INITIALIZED)
        return ADI_PDI_ERR_NOT_INITIALIZED ;
#endif

    /* pass it down to the native MIPI translation layer */
    if (ADI_PDI_SUCCESS != (result = adi_TranslateMipiCommand(hDevice, MipiCmd, pMipiParamBlock)))
        return result;

    return result;
}

#define REVERT_ADI_MISRA_SUPPRESSIONS  /*!< Revert ADI MISRA Suppressions */
#include "misra.h"

/*
** EOF
*/

/*@}*/
