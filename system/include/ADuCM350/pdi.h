/*********************************************************************************

Copyright (c) 2010-2014 Analog Devices, Inc. All Rights Reserved.

This software is proprietary to Analog Devices, Inc. and its licensors.  By using 
this software you agree to the terms of the associated Analog Devices Software 
License Agreement (See /LICENSES/ADI.LABLAB.LICENSE). 

Modified from the Analog Devices original.
Portions Copyright (c) 2018 Andrew Grosser, Moritz von Buttlar.

*********************************************************************************/

/*!
 ******************************************************************************
 * @file:    pdi.h
 * @brief:   PDI Device Definitions for ADuCxxx
 * @version: $Revision: 30127 $
 * @date:    $Date: 2015-03-09 12:05:31 -0400 (Mon, 09 Mar 2015) $
 *****************************************************************************/

/*! \addtogroup PDI_Driver PDI Driver
 *  @{
 */

#ifndef __PDI_H__
#define __PDI_H__

#include "device.h"
#include "config/adi_pdi_config.h"  /* for static initializations */

/* C++ linkage */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/* Helper macro to synchronize commands against the CMD_DONE PDI status bit. */
/* Use after each zero-parameter command or with each N-parameter command    */
/* sequence after all N parameters are pushed.                               */
#define PDI_WAIT_CMD_DONE {while (!(ADI_PDI_STATUS_CMD_DONE & pADI_PDI->PDI_STAT)) ;}  /*!< wait macro to sync command completion */


/* Helper macro to pack 8-bit parameters into 32-bit parameter FIFO entries */
#define PDI_FIFO_PACK(a,b,c,d) (a<<24 | b<<16 | c<<8 | d)          /*!< byte encoder to pack 32-bit parameters */

/* Helper macros to unpack 32-bit parameter FIFO return values into 8-bit parameters */
#define PDI_FIFO_UNPACK_A(ret_val) ((ret_val & 0xFF000000) >> 24)  /*!< byte extractor to unpack 32-bit parameters */
#define PDI_FIFO_UNPACK_B(ret_val) ((ret_val & 0x00FF0000) >> 16)  /*!< byte extractor to unpack 32-bit parameters */
#define PDI_FIFO_UNPACK_C(ret_val) ((ret_val & 0x0000FF00) >> 8)   /*!< byte extractor to unpack 32-bit parameters */
#define PDI_FIFO_UNPACK_D(ret_val) (ret_val & 0x000000FF)          /*!< byte extractor to unpack 32-bit parameters */


/*!
 *****************************************************************************
 * \enum ADI_PDI_DEV_ID_TYPE
 *
 * PDI device IDs.  Depending on device configuration there may
 * be one or more physical controllers.  In the case of PDI, there are two.
 * Use the #ADI_PDI_DEV_ID_TYPE enumerator to select which device is being
 * initialized by function adi_PDI_Init() or adi_PDI_SlaveInit().
 *
 *****************************************************************************/
typedef enum {
    ADI_PDI_DEVID_0 = 0,              /*!< PDI0 device ID.                    */
    ADI_PDI_MAX_DEVID
} ADI_PDI_DEV_ID_TYPE;


/*!
 *****************************************************************************
 * \enum ADI_PDI_RESULT_TYPE
 *
 * PDI Device Error Codes.  #ADI_PDI_SUCCESS is always zero and all other PDI
 * error codes are offset by the base PDI error code offset,
 * #ADI_DEV_PDI_ERROR_OFFSET.  A number of PDI error codes are debug-only,
 * as controlled by the "ADI_DEBUG" build macro.
 *
 * The return value of all PDI APIs returning #ADI_PDI_RESULT_TYPE
 * should always be tested at the application level for success or failure.
 *
 *****************************************************************************/
typedef enum
{
    ADI_PDI_SUCCESS                 = 0,                            /*!< Generic success.                    */
    ADI_PDI_ERR_UNKNOWN             = ADI_DEV_PDI_ERROR_OFFSET,     /*!< Generic error.                      */
    ADI_PDI_ERR_ALREADY_INITIALIZED ,   /*!< PDI device is already initialized.   */
    ADI_PDI_ERR_BAD_DEV_HANDLE      ,   /*!< Invalid device handle.               */
    ADI_PDI_ERR_BAD_DEV_ID          ,   /*!< Invalid device ID.                   */
    ADI_PDI_ERR_NOT_INITIALIZED     ,   /*!< PDI device is uninitialized.         */
    ADI_PDI_ERR_NULL_DATA_POINTER   ,   /*!< NULL data pointer not allowed.       */
    ADI_PDI_ERR_PARAM_OUT_OF_RANGE  ,   /*!< Parameter is out of range.           */
    ADI_PDI_ERR_UNSUPPORTED_MIPI    ,   /*!< MIPI command is unsupported.         */
    ADI_PDI_ERR_UNKNOWN_MIPI        ,   /*!< MIPI command is unrecognized.        */
    ADI_PDI_ERR_DMA_OPEN            ,   /*!< PDI failed to open DMA channel.      */
    ADI_PDI_ERR_DMA_SUBMIT          ,   /*!< PDI failed to submit DMA descriptor. */
    ADI_PDI_ERR_SEMAPHORE_FAILED    ,   /*!< PDI failed OS semaphore operation.   */
} ADI_PDI_RESULT_TYPE;


/*!
 *****************************************************************************
 * \enum ADI_PDI_STATE_TYPE
 *
 * PDI driver state.  Used for internal tracking of the PDI device initialization
 * progress during the adi_PDI_Init() and adi_PDI_SlaveInit() functions.  Also
 * used to insure the PDI device has been properly initialized as a prerequisite
 * to using the balance of the PDI API.
 *
 *****************************************************************************/
typedef enum
{
    ADI_PDI_STATE_UNINITIALIZED= 0,        /*!< PDI is not initialized.           */
    ADI_PDI_STATE_INITIALIZED,             /*!< PDI is initialized.               */
} ADI_PDI_STATE_TYPE;


/* PDI Configuration Register Macros */
#define ADI_PDI_CONFIG_END_TRANSFER              (1 << BITP_PDI_PDI_CFG_END_XFER)                /*!< PDI config register macro */
#define ADI_PDI_CONFIG_DMA_ENABLE                (1 << BITP_PDI_PDI_CFG_DMA_EN)                  /*!< PDI config register macro */
#define ADI_PDI_CONFIG_FLUSH_FIFO                (1 << BITP_PDI_PDI_CFG_FLSH_FIFO)               /*!< PDI config register macro */
#define ADI_PDI_CONFIG_8BIT_8BPP                 (0 << BITP_PDI_PDI_CFG_DWIDTH_BPP)              /*!< PDI config register macro */
#define ADI_PDI_CONFIG_8BIT_12BPP                (1 << BITP_PDI_PDI_CFG_DWIDTH_BPP)              /*!< PDI config register macro */
#define ADI_PDI_CONFIG_8BIT_16BPP                (2 << BITP_PDI_PDI_CFG_DWIDTH_BPP)              /*!< PDI config register macro */
#define ADI_PDI_CONFIG_16BIT_8BPP                (3 << BITP_PDI_PDI_CFG_DWIDTH_BPP)              /*!< PDI config register macro */
#define ADI_PDI_CONFIG_16BIT_12BPP               (4 << BITP_PDI_PDI_CFG_DWIDTH_BPP)              /*!< PDI config register macro */
#define ADI_PDI_CONFIG_16BIT_16BPP               (5 << BITP_PDI_PDI_CFG_DWIDTH_BPP)              /*!< PDI config register macro */
#define ADI_PDI_CONFIG_9BIT_18BPP                (6 << BITP_PDI_PDI_CFG_DWIDTH_BPP)              /*!< PDI config register macro */
#define ADI_PDI_CONFIG_DBI_TYPE_A_FIXED_E        (0 << BITP_PDI_PDI_CFG_DBI_TYPE)                /*!< PDI config register macro */
#define ADI_PDI_CONFIG_DBI_TYPE_A_CLOCKED_E      (1 << BITP_PDI_PDI_CFG_DBI_TYPE)                /*!< PDI config register macro */
#define ADI_PDI_CONFIG_DBI_TYPE_B                (2 << BITP_PDI_PDI_CFG_DBI_TYPE)                /*!< PDI config register macro */
#define ADI_PDI_CONFIG_DBI_TYPE_C1               (3 << BITP_PDI_PDI_CFG_DBI_TYPE)                /*!< PDI config register macro */
#define ADI_PDI_CONFIG_DBI_TYPE_C3               (4 << BITP_PDI_PDI_CFG_DBI_TYPE)                /*!< PDI config register macro */
#define ADI_PDI_CONFIG_PDI_ENABLE                (1 << BITP_PDI_PDI_CFG_PDI_EN)                  /*!< PDI config register macro */

/* PDI Shared Interrupt Enable And Interrupt Clear Register Macros */
#define ADI_PDI_IRQ_FIFO_UNDERFLOW               (1 << BITP_PDI_PDI_INT_SET_SET_FIFO_UDF_IEN)    /*!< PDI interrupt macro */
#define ADI_PDI_IRQ_FIFO_OVERFLOW                (1 << BITP_PDI_PDI_INT_SET_SET_FIFO_OVF_IEN)    /*!< PDI interrupt macro */
#define ADI_PDI_IRQ_CMD_WRITE_ERROR              (1 << BITP_PDI_PDI_INT_SET_SET_CMD_WR_ERR_IEN)  /*!< PDI interrupt macro */
#define ADI_PDI_IRQ_CMD_DONE                     (1 << BITP_PDI_PDI_INT_SET_SET_CMD_DONE_IEN)    /*!< PDI interrupt macro */
#define ADI_PDI_IRQ_FIFO_DONE                    (1 << BITP_PDI_PDI_INT_SET_SET_DONE_IEN)        /*!< PDI interrupt macro */
#define ADI_PDI_IRQ_TEAR_ASSERTED                (1 << BITP_PDI_PDI_INT_SET_SET_TE_IEN)          /*!< PDI interrupt macro */
#define ADI_PDI_IRQ_RX                           (1 << BITP_PDI_PDI_INT_SET_SET_RX_IEN)          /*!< PDI interrupt macro */
#define ADI_PDI_IRQ_TX                           (1 << BITP_PDI_PDI_INT_SET_SET_TX_IEN)          /*!< PDI interrupt macro */

/* PDI Status Register Macros */
#define ADI_PDI_STATUS_FIFO_UNDERFLOW            (1 << BITP_PDI_PDI_STAT_FIFO_UDF)               /*!< PDI status register macro */
#define ADI_PDI_STATUS_FIFO_OVERFLOW             (1 << BITP_PDI_PDI_STAT_FIFO_OVF)               /*!< PDI status register macro */
#define ADI_PDI_STATUS_CMD_WRITE_ERROR           (1 << BITP_PDI_PDI_STAT_CMD_WR_ERR)             /*!< PDI status register macro */
#define ADI_PDI_STATUS_CMD_DONE                  (1 << BITP_PDI_PDI_STAT_CMD_DONE)               /*!< PDI status register macro */
#define ADI_PDI_STATUS_FIFO_FULL                 (1 << BITP_PDI_PDI_STAT_FIFO_FULL)              /*!< PDI status register macro */
#define ADI_PDI_STATUS_FIFO_EMPTY                (1 << BITP_PDI_PDI_STAT_FIFO_EMPTY)             /*!< PDI status register macro */
#define ADI_PDI_STATUS_FIFO_CONTENT_ONE_OR_NONE  (0 << BITP_PDI_PDI_STAT_FIFO_STAT)              /*!< PDI status register macro */
#define ADI_PDI_STATUS_FIFO_CONTENT_TWO          (1 << BITP_PDI_PDI_STAT_FIFO_STAT)              /*!< PDI status register macro */
#define ADI_PDI_STATUS_FIFO_CONTENT_THREE        (2 << BITP_PDI_PDI_STAT_FIFO_STAT)              /*!< PDI status register macro */
#define ADI_PDI_STATUS_FIFO_CONTENT_FOUR         (3 << BITP_PDI_PDI_STAT_FIFO_STAT)              /*!< PDI status register macro */
#define ADI_PDI_STATUS_FIFO_DONE                 (1 << BITP_PDI_PDI_STAT_DONE_IRQ                /*!< PDI status register macro */
#define ADI_PDI_STATUS_TEAR_ASSERTED             (1 << BITP_PDI_PDI_STAT_TE_IRQ)                 /*!< PDI status register macro */
#define ADI_PDI_STATUS_RX                        (1 << BITP_PDI_PDI_STAT_RX_IRQ)                 /*!< PDI status register macro */
#define ADI_PDI_STATUS_TX                        (1 << BITP_PDI_PDI_STAT_TX_IRQ)                 /*!< PDI status register macro */

/* PDI Command Register Macros */
#define ADI_PDI_CMD_PARAM_COUNT_POS              (BITP_PDI_PDI_CMD_N_PARAM)                      /*!< PDI command register macro */
#define ADI_PDI_CMD_PARAM_COUNT_MASK             (BITM_PDI_PDI_CMD_N_PARAM)                      /*!< PDI command register macro */
#define ADI_PDI_CMD_FRAME_DATA                   (1 << BITP_PDI_PDI_CMD_FR_DATA)                 /*!< PDI command register macro */
#define ADI_PDI_CMD_CONTROL_PARAM                (0 << BITP_PDI_PDI_CMD_FR_DATA)                 /*!< PDI command register macro */
#define ADI_PDI_CMD_DIRECTION_WRITE              (1 << BITP_PDI_PDI_CMD_WR_RD)                   /*!< PDI command register macro */
#define ADI_PDI_CMD_DIRECTION_READ               (0 << BITP_PDI_PDI_CMD_WR_RD)                   /*!< PDI command register macro */
#define ADI_PDI_CMD_COMMAND_POS                  (BITM_PDI_PDI_CMD_CMD)                          /*!< PDI command register macro */
#define ADI_PDI_CMD_COMMAND_MASK                 (BITM_PDI_PDI_CMD_CMD)                          /*!< PDI command register macro */

/* PDI Frame Data Count Register Macros */
#define ADI_PDI_FRAME_DATA_COUNT_POS             (BITP_PDI_PDI_FRDATA_N_FR_DATA_N)               /*!< PDI frame data register macro */
#define ADI_PDI_FRAME_DATA_COUNT_MASK            (BITM_PDI_PDI_FRDATA_N_FR_DATA_N)               /*!< PDI frame data register macro */

/* PDI Interface Timing Register Macros */
#define ADI_PDI_TIMING_CS_SETUP_POS              (BITP_PDI_PDI_IF_TIMING_TCSH)                   /*!< PDI timing register macro */
#define ADI_PDI_TIMING_CS_SETUP_MASK             (BITM_PDI_PDI_IF_TIMING_TCSH)                   /*!< PDI timing register macro */

#define ADI_PDI_TIMING_RW_LOW_DURATION_POS       (BITP_PDI_PDI_IF_TIMING_TWRL_RDL)               /*!< PDI timing register macro */
#define ADI_PDI_TIMING_RW_LOW_DURATION_MASK      (BITM_PDI_PDI_IF_TIMING_TWRL_RDL)               /*!< PDI timing register macro */

#define ADI_PDI_TIMING_RW_HIGH_DURATION_POS      (BITP_PDI_PDI_IF_TIMING_TWRH_RDH)               /*!< PDI timing register macro */
#define ADI_PDI_TIMING_RW_HIGH_DURATION_MASK     (BITM_PDI_PDI_IF_TIMING_TWRH_RDH)               /*!< PDI timing register macro */

#define ADI_PDI_TIMING_CS_HOLD_POS               (BITP_PDI_PDI_IF_TIMING_TCSF)                   /*!< PDI timing register macro */
#define ADI_PDI_TIMING_CS_HOLD_MASK              (BITM_PDI_PDI_IF_TIMING_TCSF)                   /*!< PDI timing register macro */

#define ADI_PDI_TIMING_ADDRESS_SETUP_HOLD_POS    (BITP_PDI_PDI_IF_TIMING_TAS_AH)                 /*!< PDI timing register macro */
#define ADI_PDI_TIMING_ADDRESS_SETUP_HOLD_MASK   (BITM_PDI_PDI_IF_TIMING_TAS_AH)                 /*!< PDI timing register macro */


/* Application layer MIPI Display Command Set (DCS), per MIPI Alliance DCS spec 1.02.00 */

/* Interface Pixel Formats */
#define MIPI_DCS_3BITS_PER_PIXEL                 (0x01)  /*!< PDI bit-per-pixel macro */
#define MIPI_DCS_8BITS_PER_PIXEL                 (0x02)  /*!< PDI bit-per-pixel macro */
#define MIPI_DCS_12BITS_PER_PIXEL                (0x03)  /*!< PDI bit-per-pixel macro */
#define MIPI_DCS_16BITS_PER_PIXEL                (0x05)  /*!< PDI bit-per-pixel macro */
#define MIPI_DCS_18BITS_PER_PIXEL                (0x06)  /*!< PDI bit-per-pixel macro */
#define MIPI_DCS_24BITS_PER_PIXEL                (0x07)  /*!< PDI bit-per-pixel macro */

/*!
 * \enum ADI_MIPI_DCS_ENUM
 * PDI MIPI command encodings
 */
typedef enum
{
    /* Note: "DCS0" is zero-parameter command, "DCS1" is a one-parameter command, etc. */
    MIPI_DCS0_ENTER_IDLE_MODE                    = 0x39,
    MIPI_DCS0_ENTER_INVERT_MODE                  = 0x21,
    MIPI_DCS0_ENTER_NORMAL_MODE                  = 0x13,
    MIPI_DCS0_ENTER_PARTIAL_MODE                 = 0x12,
    MIPI_DCS0_ENTER_SLEEP_MODE                   = 0x10,
    MIPI_DCS0_EXIT_IDLE_MODE                     = 0x38,
    MIPI_DCS0_EXIT_INVERT_MODE                   = 0x20,
    MIPI_DCS0_EXIT_SLEEP_MODE                    = 0x11,
    MIPI_DCS1_GET_ADDRESS_MODE                   = 0x0B,
    MIPI_DCS1_GET_BLUE_CHANNEL                   = 0x08,
    MIPI_DCS1_GET_DIAGNOSTIC_MODE                = 0x0F,
    MIPI_DCS1_GET_DISPLAY_MODE                   = 0x0D,
    MIPI_DCS1_GET_GREEN_CHANNEL                  = 0x07,
    MIPI_DCS1_GET_PIXEL_FORMAT                   = 0x0C,
    MIPI_DCS1_GET_POWER_MODE                     = 0x0A,
    MIPI_DCS1_GET_RED_CHANNEL                    = 0x06,
    MIPI_DCS2_GET_SCANLINE                       = 0x45,
    MIPI_DCS1_GET_SIGNAL_MODE                    = 0x0E,
    MIPI_DCS0_NOP                                = 0x00,
    MIPI_DCSN_READ_DDB_CONTINUE                  = 0xA8,
    MIPI_DCSN_READ_DDB_START                     = 0x1A,
    MIPI_DCSN_READ_MEMORY_CONTINUE               = 0x3E,
    MIPI_DCSN_READ_MEMORY_START                  = 0x2E,
    MIPI_DCS1_SET_ADDRESS_MODE                   = 0x36,
    MIPI_DCS4_SET_COLUMN_ADDRESS                 = 0x2A,
    MIPI_DCS0_SET_DISPLAY_OFF                    = 0x28,
    MIPI_DCS0_SET_DISPLAY_ON                     = 0x29,
    MIPI_DCS1_SET_GAMMA_CURVE                    = 0x26,
    MIPI_DCS4_SET_PAGE_ADDRESS                   = 0x2B,
    MIPI_DCS4_SET_PARTIAL_COLUMNS                = 0x31,
    MIPI_DCS4_SET_PARTIAL_ROWS                   = 0x30,
    MIPI_DCS1_SET_PIXEL_FORMAT                   = 0x3A,
    MIPI_DCS6_SET_SCROLL_AREA                    = 0x33,
    MIPI_DCS2_SET_SCROLL_START                   = 0x37,
    MIPI_DCS0_SET_TEAR_OFF                       = 0x34,
    MIPI_DCS1_SET_TEAR_ON                        = 0x35,
    MIPI_DCS2_SET_TEAR_SCANLINE                  = 0x44,
    MIPI_DCS0_SOFT_RESET                         = 0x01,
    MIPI_DCSN_WRITE_LUT                          = 0x2D,
    MIPI_DCSN_WRITE_MEMORY_CONTINUE              = 0x3C,
    MIPI_DCSN_WRITE_MEMORY_START                 = 0x2C,
} ADI_MIPI_DCS_ENUM;

/*!
 * \enum ADI_PDI_CMD_PARAM_TYPE
 * PDI command control/data selector
 */
typedef enum
{
    PDI_CMD_CONTROL = 0,
    PDI_CMD_DATA
} ADI_PDI_CMD_PARAM_TYPE;

/*!
 * \enum ADI_PDI_CMD_DIR_TYPE
 * PDI command read/write selector
 */
typedef enum
{
    PDI_DIR_READ = 0,
    PDI_DIR_WRITE
} ADI_PDI_CMD_DIR_TYPE;


/* PSI Device instance private data handle typedef. */
typedef struct ADI_PDI_DEV_DATA_TYPE* ADI_PDI_DEV_HANDLE;  /*!< PDI Device instance private data handle typedef. */

/* Driver Initialization APIs */
extern ADI_PDI_RESULT_TYPE adi_PDI_Init                (ADI_PDI_DEV_ID_TYPE const devID, ADI_PDI_DEV_HANDLE* const pHandle);
extern ADI_PDI_RESULT_TYPE adi_PDI_UnInit              (ADI_PDI_DEV_HANDLE const hDevice);

/* Raw Register APIs */
extern ADI_PDI_RESULT_TYPE adi_PDI_SetConfiguration    (ADI_PDI_DEV_HANDLE const hDevice, const uint32_t   Config);
extern ADI_PDI_RESULT_TYPE adi_PDI_GetConfiguration    (ADI_PDI_DEV_HANDLE const hDevice, uint32_t* const pConfig);

extern ADI_PDI_RESULT_TYPE adi_PDI_InterruptEnable     (ADI_PDI_DEV_HANDLE const hDevice, const uint32_t Enables);
extern ADI_PDI_RESULT_TYPE adi_PDI_InterruptDisable    (ADI_PDI_DEV_HANDLE const hDevice, const uint32_t Disables);

extern ADI_PDI_RESULT_TYPE adi_PDI_SetStatus           (ADI_PDI_DEV_HANDLE const hDevice, const uint32_t   Status);
extern ADI_PDI_RESULT_TYPE adi_PDI_GetStatus           (ADI_PDI_DEV_HANDLE const hDevice, uint32_t* const pStatus);

extern ADI_PDI_RESULT_TYPE adi_PDI_SetCommand          (ADI_PDI_DEV_HANDLE const hDevice, const uint8_t ParamCount,
                                                                                          const ADI_PDI_CMD_PARAM_TYPE Type,
                                                                                          const ADI_PDI_CMD_DIR_TYPE Dir,
                                                                                          const uint8_t Command);
extern ADI_PDI_RESULT_TYPE adi_PDI_GetCommand          (ADI_PDI_DEV_HANDLE const hDevice, uint8_t* const pCommand);

extern ADI_PDI_RESULT_TYPE adi_PDI_SetFrameDataCount   (ADI_PDI_DEV_HANDLE const hDevice, const uint32_t   Count);
extern ADI_PDI_RESULT_TYPE adi_PDI_GetFrameDataCount   (ADI_PDI_DEV_HANDLE const hDevice, uint32_t* const pCount);

extern ADI_PDI_RESULT_TYPE adi_PDI_PushParameterFIFO   (ADI_PDI_DEV_HANDLE const hDevice, const uint32_t   Param);
extern ADI_PDI_RESULT_TYPE adi_PDI_PullParameterFIFO   (ADI_PDI_DEV_HANDLE const hDevice, uint32_t* const pParam);

extern ADI_PDI_RESULT_TYPE adi_PDI_SetInterfaceTiming  (ADI_PDI_DEV_HANDLE const hDevice, const uint32_t  Timing);
extern ADI_PDI_RESULT_TYPE adi_PDI_GetInterfaceTiming  (ADI_PDI_DEV_HANDLE const hDevice, uint32_t* const pTiming);


/* PDI MIPI Display Command Set (DCS) interface to device-specific implimentation */
extern ADI_PDI_RESULT_TYPE adi_PDI_DispatchMipiCommand (ADI_PDI_DEV_HANDLE const hDevice, const ADI_MIPI_DCS_ENUM MipiCmd, const void* pMipiParamBlock);

/* C++ linkage */
#ifdef __cplusplus
}
#endif

#endif /* include guard */

/*
** EOF
*/

/*@}*/
