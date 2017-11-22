/*********************************************************************************

Copyright (c) 2010-2014 Analog Devices, Inc. All Rights Reserved.

This software is proprietary to Analog Devices, Inc. and its licensors.  By using 
this software you agree to the terms of the associated Analog Devices Software 
License Agreement.

*********************************************************************************/

/*!
 *****************************************************************************
 * @file:    dma.h
 * @brief:   uDMA Device Definitions for ADuCxxx
 * @version: $Revision: 28920 $
 * @date:    $Date: 2014-12-03 07:23:07 -0500 (Wed, 03 Dec 2014) $
 *****************************************************************************/

/*! \addtogroup DMA_Driver DMA Driver
 *  uMDA device driver.
 *  @{
 */

#ifndef __DMA_H__
#define __DMA_H__

#include "device.h"

/* C++ linkage */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*=============  D E F I N E S  =============*/

#define ADI_DMASTA_ENABLE   DMASTA_ENABLE  /*! Enable bit for ADC DMA trasnfers */

#define DMA_CCD_LEN_MASK    (0x00003FF0)   /*!< Bitfield mask for DMA lenght status */
#define DMA_CCD_CYCLE_MASK  (0x00000007)   /*!< Bitfield mask for DMA cycle status  */

/*! Limit for a single transfer block size in number of elements */
#define ADI_DMA_MAX_TRANSFER_SIZE                1024ul


/*============= D A T A T Y P E S =============*/


/*! DMA Device State */
typedef enum
{
    ADI_DMA_STATE_UNKOWN = 0,                    /*!< DMA is not initialized */
    ADI_DMA_STATE_INITIALIZED,                   /*!< DMA driver initialized */
} ADI_DMA_INITIALIZATION_TYPE;


/*!
 * Dma Data Increments
 */
typedef enum
{
    ADI_DMA_INCR_BYTE = 0,                      /*!< Byte increment */
    ADI_DMA_INCR_HALFWORD,                      /*!< Half word increment */
    ADI_DMA_INCR_WORD,                          /*!< Word increment */
    ADI_DMA_INCR_NONE,                          /*!< No increment */

    /* only ADuCM350 supports decrements */
    ADI_DMA_DECR_BYTE,                          /*!< Byte decrement */
    ADI_DMA_DECR_HALFWORD,                      /*!< Half word decrement */
    ADI_DMA_DECR_WORD,                          /*!< Word decrement */
    ADI_DMA_DECR_NONE                           /*!< No decrement */

} ADI_DMA_INCR_TYPE;


/*!
 * Dma Data Widths
 */
typedef enum
{
    ADI_DMA_WIDTH_BYTE = 0,                     /*!<  8-bit */
    ADI_DMA_WIDTH_HALFWORD,                     /*!< 16-bit */
    ADI_DMA_WIDTH_WORD                          /*!< 32-bit */
} ADI_DMA_WIDTH_TYPE;


/*!
 * Dma Protection
 */
typedef enum
{
	/* this is the only valid protection mode for ADuCM350, i.e., none */
    ADI_DMA_HPROT_NONE       = 0,               /*!< No protection */

    /* additional protection modes for other targets */
    ADI_DMA_HPROT_PRIVILEGED = 1,               /*!< Priviliged */
    ADI_DMA_HPROT_BUFFERABLE = 2,               /*!< Bufferable */
    ADI_DMA_HPROT_CACHABLE   = 4                /*!< Cachable */
} ADI_DMA_HPROT_TYPE;


/*!
 * Dma Rearbitration Intervals (chunk size between bus arbitrations)
 */
typedef enum
{
    ADI_DMA_RPOWER_1 = 0,                       /*!< Rearbitrate after    1 transfer */
    ADI_DMA_RPOWER_2,                           /*!< Rearbitrate after    2 transfers */
    ADI_DMA_RPOWER_4,                           /*!< Rearbitrate after    4 transfers */
    ADI_DMA_RPOWER_8,                           /*!< Rearbitrate after    8 transfers */
    ADI_DMA_RPOWER_16,                          /*!< Rearbitrate after   16 transfers */
    ADI_DMA_RPOWER_32,                          /*!< Rearbitrate after   32 transfers */
    ADI_DMA_RPOWER_64,                          /*!< Rearbitrate after   64 transfers */
    ADI_DMA_RPOWER_128,                         /*!< Rearbitrate after  128 transfers */
    ADI_DMA_RPOWER_256,                         /*!< Rearbitrate after  256 transfers */
    ADI_DMA_RPOWER_512,                         /*!< Rearbitrate after  512 transfers */
    ADI_DMA_RPOWER_1024                         /*!< Rearbitrate after 1024 transfers */
} ADI_DMA_RPOWER_TYPE;


/*!
 * Dma Transfer Modes
 */
typedef enum
{
    ADI_DMA_MODE_INVALID = 0,                   /*!< Invalid mode */
    ADI_DMA_MODE_BASIC,                         /*!< Basic mode */
    ADI_DMA_MODE_AUTO,                          /*!< Auto request mode */
    ADI_DMA_MODE_PINGPONG,                      /*!< Ping pong mode */
    ADI_DMA_MODE_SGM,                           /*!< Memory Scatter gather mode (not valid as no Memory DMA support) */
    ADI_DMA_MODE_SGP                            /*!< Peripheral Scatter mode */
} ADI_DMA_MODE_TYPE;


/*!
 * Dma Controller States
 */
typedef enum
{
    ADI_DMA_STATE_IDLE                 = DMASTA_STATE_IDLE,             /*!< Idle */
    ADI_DMA_STATE_READ_CHNL_CTRL_DATA  = DMASTA_STATE_RDCHNLDATA,       /*!< Reading channel control data */
    ADI_DMA_STATE_READ_SRC_EPTR        = DMASTA_STATE_RDSRCENDPTR,      /*!< Reading source data end pointer */
    ADI_DMA_STATE_READ_DST_EPTR        = DMASTA_STATE_RDDSTENDPTR,      /*!< Reading destination data end pointer */
    ADI_DMA_STATE_READ_SRC_DATA        = DMASTA_STATE_RDSRCDATA,        /*!< Reading source data */
    ADI_DMA_STATE_WRITE_DST_DATA       = DMASTA_STATE_WRDSTDATA,        /*!< Writing destination data */
    ADI_DMA_STATE_WAIT_REQ_CLR         = DMASTA_STATE_WAITDMAREQCLR,    /*!< Waiting for DMA request to clear */
    ADI_DMA_STATE_WRITE_CHNL_CRTL_DATA = DMASTA_STATE_WRCHNLDATA,       /*!< Writing channel controller data */
    ADI_DMA_STATE_STALLED              = DMASTA_STATE_STALLED,          /*!< Stalled */
    ADI_DMA_STATE_DONE                 = DMASTA_STATE_DONE,             /*!< DMA done */
    ADI_DMA_STATE_PSG_TRANSITION       = DMASTA_STATE_SCATRGATHR,       /*!< Peripheral scatter-gather transition */
    ADI_DMA_STATE_INVALID              = ADI_DMA_STATE_PSG_TRANSITION+1 /*!< Undefined */
} ADI_DMA_STATE_TYPE;


/*!
 * Dma Channel Control Data Structures
 */
typedef enum
{
    ADI_DMA_CCD_PRIMARY = 0,                            /*!< Primary Channel Control Data */
    ADI_DMA_CCD_ALTERNATE                               /*!< Alternate Channel Control Data */
} ADI_DMA_CCD_TYPE;


/*!
 * Dma Channel Priority Settings (only HIGH or DEFAULT prority supported)
 */
typedef enum
{
    ADI_DMA_PRIORITY_RESET = 0,                         /*!< Use DEFAULT channel priority */
    ADI_DMA_PRIORITY_SET,                               /*!< Elevate channel to HIGH priority */
} ADI_DMA_PRIORITY_TYPE;


/* unique channel-sharing feature on ADuCM350 */
/*!
 * DMA channel assigments used in conjunction with adi_DMA_SetChannelAssign()
 */
typedef enum
{
    ADI_DMA_ASSIGN_CRC = 0,                             /*!< DEFAULT channel assignment   */
    ADI_DMA_ASSIGN_FLASH,                               /*!< Alternate channel assignment */
} ADI_DMA_ASSIGN_TYPE;


/*!
 * Dma Error Codes
 */
typedef enum
{
    ADI_DMA_SUCCESS                 = 0,                           /*!< Success */
    ADI_DMA_ERR_UNKNOWN             = ADI_DEV_DMA_ERROR_OFFSET,    /*!< Unkown error */
    ADI_DMA_ERR_NOT_INITIALIZED     = ADI_DEV_DMA_ERROR_OFFSET+1,  /*!< DMA controller/channel not initialized */
    ADI_DMA_ERR_ALREADY_INITIALIZED = ADI_DEV_DMA_ERROR_OFFSET+2,  /*!< DMA controller/channel already initialized */
    ADI_DMA_ERR_INVALID_CHANNEL     = ADI_DEV_DMA_ERROR_OFFSET+3,  /*!< Given channel number is invalid */
    ADI_DMA_ERR_INVALID_HPROT_VALUE = ADI_DEV_DMA_ERROR_OFFSET+4,  /*!< Given protection mode is invalid */
    ADI_DMA_ERR_INVALID_DATAWIDTH   = ADI_DEV_DMA_ERROR_OFFSET+5,  /*!< Given data transfer width is invalid */
    ADI_DMA_ERR_INVALID_INCREMENT   = ADI_DEV_DMA_ERROR_OFFSET+6,  /*!< Given increment is invalid */
    ADI_DMA_ERR_INVALID_LENGTH      = ADI_DEV_DMA_ERROR_OFFSET+7,  /*!< Given transfer length is invalid */
    ADI_DMA_ERR_INVALID_STATE       = ADI_DEV_DMA_ERROR_OFFSET+8,  /*!< DMA controller state is invalid */
    ADI_DMA_ERR_INVALID_CCD         = ADI_DEV_DMA_ERROR_OFFSET+9,  /*!< Given CCD type is invalid */
    ADI_DMA_ERR_INVALID_PRIORITY    = ADI_DEV_DMA_ERROR_OFFSET+10, /*!< Given channel priority is invalid */
    ADI_DMA_ERR_INVALID_PROTECTION  = ADI_DEV_DMA_ERROR_OFFSET+11, /*!< Given channel protection is invalid */
    ADI_DMA_ERR_INVALID_BYTESWAP    = ADI_DEV_DMA_ERROR_OFFSET+12, /*!< Given channel byte swap option is invalid */
    ADI_DMA_ERR_INVALID_ASSIGN      = ADI_DEV_DMA_ERROR_OFFSET+13, /*!< Given channel assignment is invalid */
} ADI_DMA_RESULT_TYPE;


/*!
 * Dma Transfer Descriptor
 */
typedef struct
{
    DMA_CHANn_TypeDef     Chan;                         /*!< channel number */
    ADI_DMA_CCD_TYPE      CCD;                          /*!< CCD to be used */

    ADI_DMA_WIDTH_TYPE    DataWidth;                    /*!< size of each transfer unit */
    uint32_t              DataLength;                   /*!< total number of units to transfer */

    void*                 pSrcData;                     /*!< Source channel data pointer */
    void*                 pDstData;                     /*!< Destination channel data pointer */

    ADI_DMA_INCR_TYPE     SrcInc;                       /*!< Source data increment/decrement */
    ADI_DMA_INCR_TYPE     DstInc;                       /*!< Destination data increment/decrement */

    ADI_DMA_HPROT_TYPE    Protection;                   /*!< shared protection state */
    ADI_DMA_RPOWER_TYPE   ArbitRate;                    /*!< number of transfer units between bus arbitrations */
    ADI_DMA_MODE_TYPE     Mode;                         /*!< transfer mode in use for this channel */
} ADI_DMA_TRANSFER_TYPE;


/* GENERAL APIs */

/* Initialization of the DMA Controller */
ADI_DMA_RESULT_TYPE adi_DMA_Init(DMA_CHANn_TypeDef chNum, ADI_DMA_PRIORITY_TYPE priority);

/* UnInitialization of the DMA Controller */
ADI_DMA_RESULT_TYPE adi_DMA_UnInit(DMA_CHANn_TypeDef chNum);

/* only ADuCM350 shares DMA between Flash and CRC controllers */
ADI_DMA_RESULT_TYPE adi_DMA_SetChannelAssign(DMA_CHANn_TypeDef chNum, ADI_DMA_ASSIGN_TYPE controller);

/* Only ADuCM350 supports byte swapping */
ADI_DMA_RESULT_TYPE adi_DMA_SetByteSwap(DMA_CHANn_TypeDef chNum, bool_t flag);

/* Returns the state of the DMA controller */
ADI_DMA_RESULT_TYPE adi_DMA_GetState(ADI_DMA_STATE_TYPE* pStatusResult);

/* Submit a Transfer Control Block */
ADI_DMA_RESULT_TYPE adi_DMA_SubmitTransfer(ADI_DMA_TRANSFER_TYPE* pTransfer);

/* Re-submit a Transfer Control Block */
ADI_DMA_RESULT_TYPE adi_DMA_ReSubmit(ADI_DMA_TRANSFER_TYPE* pTransfer);

/* Get remaining count for specified channel and specified Channel Control Data */
ADI_DMA_RESULT_TYPE adi_DMA_GetRemainingCount(DMA_CHANn_TypeDef chNum, uint32_t *pCount, ADI_DMA_CCD_TYPE ccd);

#ifdef ADI_DEBUG
void adi_DMA_PrintDescriptors(DMA_CHANn_TypeDef chNum);
#endif

/* C++ linkage */
#ifdef __cplusplus
}
#endif

#endif /* include guard */

/*
** EOF
*/

/*@}*/
