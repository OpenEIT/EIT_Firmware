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
 * @file:    dma.c
 * @brief:   uDMA Device Inplimentation for ADuCxxx
 * @version: $Revision: 28934 $
 * @date:    $Date: 2014-12-03 11:25:14 -0500 (Wed, 03 Dec 2014) $
 *****************************************************************************/

/*! \addtogroup DMA_Driver DMA Driver
 *  uMDA device driver.
 *  @{
 */

#define ASSERT_ADI_MISRA_SUPPRESSIONS  /*!< Apply ADI MISRA Suppressions */
#include "misra.h"


/*=============  I N C L U D E S   =============*/

#include "dma.h"


/* =============== IMPORTANT NOTE =====================
   In ADucRFxx the following dma signalling options are not valid

   � Done signaling and
   � Wait on request signaling

   The inputs signal stall, wait on request to dma are tied to zero in the design

   So the only options are pulse and level. In most of the peripheral "level"
   approach is used. But in LFRX pulse is generated
*/

/*=============  D E F I N E S  =============*/

/* CCD array allocation macros */
#define CCD_ALIGN (0x200) /*!< Memory alignment required for CCD array */
#define CCD_SIZE (16)     /*!< Configure CCD allocation as an integral power of two, i.e., 12 channels is allocated as 16 */

/* bitfield locations for control word of each channel's control data structure */
#define DMA_CCD_DST_INC_BITPOS      (30) /*!< Bit position for DMA destination address        */
#define DMA_CCD_DST_SIZE_BITPOS     (28) /*!< Bit position for DMA destination size           */
#define DMA_CCD_SRC_INC_BITPOS      (26) /*!< Bit position for DMA source address             */
#define DMA_CCD_SRC_SIZE_BITPOS     (24) /*!< Bit position for DMA source size                */
#define DMA_CCD_DST_HPROT_BITPOS    (21) /*!< Bit position for DMA destination protection     */
#define DMA_CCD_SRC_HPROT_BITPOS    (18) /*!< Bit position for DMA source protection          */
#define DMA_CCD_RPOWER_BITPOS       (14) /*!< Bit position for DMA bus rearbitration interval */
#define DMA_CCD_LEN_BITPOS           (4) /*!< Bit position for DMA transfer count             */
#define DMA_CCD_USEBURST_BITPOS      (3) /*!< Bit position for DMA burst mode setting         */
#define DMA_CCD_MODE_BITPOS          (0) /*!< Bit position for DMA transfer mode setting      */


/* this mask is used to clear the bits in the CDC (control data configuration) word when resubmitting a transfer */
#define DMA_CCD_RESUBMIT_CLEAR_MASK     (0xFFFFC000)    /*!< Bit mask for resetting the CDC field of DMA channel descriptor */


/*=============  Local function declarations  =============*/
#ifdef ADI_DEBUG
static ADI_DMA_RESULT_TYPE ValidateParams(ADI_DMA_TRANSFER_TYPE *pTransfer);
#endif /* ADI_DEBUG */

/*=============  D A T A  =============*/

/* DMA descriptor arrays must be contigious */
/* AND impose strict alignment requirements */
/* Each compiler has different alignment directives */
#if defined ( __ICCARM__ )
    #pragma data_alignment=CCD_ALIGN  /* IAR */
#elif defined (__CC_ARM)
    __align(CCD_ALIGN)                /* Keil */
#else
//AG:
	__attribute__((aligned(CCD_ALIGN)))

//    #pragma message("WARNING: NO ALIGHMENT DEFINED FOR DMA DESCRIPTOR BLOCKS")
#endif

/* ALIGNED: DMA channel control data array declaration */
static ADI_DCC_TypeDef gChannelControlDataArray[CCD_SIZE * 2]
#ifdef ADI_DMA_DESCRIPTORS_IN_VOLATILE_MEMORY
	/* conditional placement of DMA descriptor table to volatile memory */
	@ "volatile_ram";
#else
	/* default placement to non-volatile memory (no override) */
	;
#endif


/* pointer to the primary CCD array */
static ADI_DCC_TypeDef* pPrimaryCCD = &gChannelControlDataArray[0];

/* pointer to the alternate CCD array */
static ADI_DCC_TypeDef* pAlternateCCD = &gChannelControlDataArray[CCD_SIZE];


/*! \struct ADI_DMA_DEV_DATA_TYPE
 * DMA Device instance data structure */
typedef struct {

   ADI_DMA_INITIALIZATION_TYPE  InitState;       /*!< track initialization state */
   uint32_t                     ChannelsInUse;   /*!< what channels are active   */
   uint32_t                     ByteSwapInUse;   /*!< channels using ByteSwap    */
} ADI_DMA_DEV_DATA_TYPE;


/*!
 * \struct DMA_DevData
 * Singular instance of DMA device driver data
 */
ADI_DMA_DEV_DATA_TYPE DMA_DevData = {

    ADI_DMA_STATE_UNKOWN,                       /*!< Initialization State     */
    0ul,                                        /*!< channels-in-use bitfield */
    0ul,                                        /*!< byteswap bitfield        */
};    /*!< device driver instance data array */

/* pointer to init instance */
static ADI_DMA_DEV_DATA_TYPE* const pDMA_DevData = &DMA_DevData;


/*=============  I M P L I M E N T A T I O N S  =============*/



/*!
  @brief        Initialize uDMA driver for a given channel and priority.
  @details      Initializes all the DMA channels when called for the first time,
                otherwise initializes the given DMA channel and insures it is not
                already allocated.  Also specifies DMA channel priority.

  @param[in]    chNum           DMA channel number to be initialized
  @param[in]    priority        Channnel priority (high or default)

  @return       Status
                - #ADI_DMA_SUCCESS If successfully intialized the given channel
                - #ADI_DMA_ERR_ALREADY_INITIALIZED [D] if given channel is already initialized
                - #ADI_DMA_ERR_INVALID_CHANNEL [D] if channel param is invalid
                - #ADI_DMA_ERR_INVALID_PRIORITY [D] if channel priority is invalid

  @sa           adi_DMA_UnInit()
 */
ADI_DMA_RESULT_TYPE adi_DMA_Init(DMA_CHANn_TypeDef chNum, ADI_DMA_PRIORITY_TYPE priority)
{
    uint32_t channel_bit = 1ul << chNum;

#ifdef ADI_DEBUG
    /* verify channel number */
      if (chNum >= NUM_DMA_CHANNELSn) {
            return ADI_DMA_ERR_INVALID_CHANNEL;
      }

      /* make sure channel is not already initialized */
      if (pDMA_DevData->ChannelsInUse & channel_bit) {
            return ADI_DMA_ERR_ALREADY_INITIALIZED;
	  }

#endif  /* ADI_DEBUG */


    // On the ADuCM350, the dma controller internal clock is gated off when
    // DMA is disabled.  So always enable controller first so that
    // further config settings are latched, regardless of processor.
    ADI_ENTER_CRITICAL_REGION();
    pADI_DMA->DMACFG |= (uint32_t)ADI_DMASTA_ENABLE;
    ADI_EXIT_CRITICAL_REGION();

    /* one-time controller initializations */
    if (pDMA_DevData->InitState == ADI_DMA_STATE_UNKOWN) {

        /* initialize RAM-based descriptor array */
        for (int i = 0; i < (int)NUM_DMA_CHANNELSn; i++)
        {
            pPrimaryCCD[i].DMASRCEND = 0u;
            pPrimaryCCD[i].DMADSTEND = 0u;
            pPrimaryCCD[i].DMACDC = DMACFG_RVAL;

            pAlternateCCD[i].DMASRCEND = 0u;
            pAlternateCCD[i].DMADSTEND = 0u;
            pAlternateCCD[i].DMACDC = DMACFG_RVAL;
        }

        /* set descriptor memory base pointer on DMA controller */
        /* note, setting DMAPDBPTR implicitly sets the read-only DMAADBPTR */
        pADI_DMA->DMAPDBPTR  = (uint32_t)pPrimaryCCD;

        /* reset per-channel, bitmapped control registers (one bit per channel) */
        pADI_DMA->DMARMSKSET = DMARMSKSET_RVAL;  /* request mask set */
        pADI_DMA->DMARMSKCLR = DMARMSKCLR_RVAL;  /* request mask clear */
        pADI_DMA->DMAENSET   = DMAENSET_RVAL;    /* enable set */
        pADI_DMA->DMAENCLR   = DMAENCLR_RVAL;    /* enable clear */
        pADI_DMA->DMAALTSET  = DMAALTSET_RVAL;   /* alternate control data set */
        pADI_DMA->DMAALTCLR  = DMAALTCLR_RVAL;   /* alternate control data clear */
        pADI_DMA->DMAPRISET  = DMAPRISET_RVAL;   /* priority transfer set */
        pADI_DMA->DMAPRICLR  = 0xfffful;         /* force all to default priority: formally used DMAPRICLR_RVAL, which is zero and does nothing */

        /* clear controller-wide bus error status */
        pADI_DMA->DMAERRCLR  = (uint32_t)DMAERRCLR_ERROR;  /* bus error clear */

        /* various DMAPERID# and DMAPCELLID# registers contain read-only hardware ID info, requiring no initialization */

        /* mark controller initialized */
        pDMA_DevData->InitState = ADI_DMA_STATE_INITIALIZED;

    }  /* end one-time initialization */


	/* set/clear the channel priority (note seperate set/clr registers) */
	switch (priority) {
		case ADI_DMA_PRIORITY_RESET:

			ADI_ENTER_CRITICAL_REGION();
			pADI_DMA->DMAPRICLR |= channel_bit;
			ADI_EXIT_CRITICAL_REGION();
			break;

		case ADI_DMA_PRIORITY_SET:

			ADI_ENTER_CRITICAL_REGION();
			pADI_DMA->DMAPRISET |= channel_bit;
			ADI_EXIT_CRITICAL_REGION();
			break;
#ifdef ADI_DEBUG

		default:
		   /* Any other value is invalid */
		   return ADI_DMA_ERR_INVALID_PRIORITY;
#endif
	}


	/* mark channel active */
	ADI_ENTER_CRITICAL_REGION();
	pDMA_DevData->ChannelsInUse |= channel_bit;
	ADI_EXIT_CRITICAL_REGION();

    return ADI_DMA_SUCCESS;
}


/*!
  @brief        Un-Initialize the given DMA channel

  @param[in]    chNum           DMA channel number to be un-initialized

  @return       Status
                - #ADI_DMA_SUCCESS If successfully un-intialized the given channel
                - #ADI_DMA_ERR_NOT_INITIALIZED [D] if given channel is not initialized
                - #ADI_DMA_ERR_INVALID_CHANNEL [D] if channel param is invalid

  @sa           adi_DMA_Init()
 */
ADI_DMA_RESULT_TYPE adi_DMA_UnInit(DMA_CHANn_TypeDef chNum)
{
#ifdef ADI_DEBUG
        /* check for global initialization */
    if (pDMA_DevData->InitState != ADI_DMA_STATE_INITIALIZED) {
        return ADI_DMA_ERR_NOT_INITIALIZED;
    }

	/* verify channel number */
    if (chNum >= NUM_DMA_CHANNELSn) {
        return ADI_DMA_ERR_INVALID_CHANNEL;
    }

#endif /* ADI_DEBUG */

    /* check for channel initialization */
    if (pDMA_DevData->ChannelsInUse & (1ul << chNum)) {

        /* do channel-specific uninit here... */

        ADI_ENTER_CRITICAL_REGION();
        /* mark channel inactive */
        pDMA_DevData->ChannelsInUse &= ~(1ul << chNum);

        ADI_EXIT_CRITICAL_REGION();


    } else {

        /* channel never initialized */
        return ADI_DMA_ERR_NOT_INITIALIZED;
    }

    /* if no channels remain active */
    if (!(pDMA_DevData->ChannelsInUse)) {

        /* reset controller */
        pADI_DMA->DMACFG = DMACFG_RVAL;

        /* and put driver into uninitialized state */
        pDMA_DevData->InitState = ADI_DMA_STATE_UNKOWN;
    }
    return ADI_DMA_SUCCESS;
}


/* only ADuCM350 shares DMA between Flash and CRC controllers */
/*!
  @brief        Assign DMA channel CRC_CHANn (13) to a particular device.
  @details      DMA channel CRC_CHANn (13) is shared between two mutually
                exclusive devices and may be switched from one to the other
                by this API.  Configure channel #chNum to service either the
                CRC controller or the FLASH controller.

  @param[in]    chNum           Must be CRC_CHANn (13) for ADuCM350.
  @param[in]    controller      Possible device assignment:
                                  - ADI_DMA_ASSIGN_CRC for CRC operation,
                                  - ADI_DMA_ASSIGN_FLASH for FLASH operation.

  @return       Status
                - #ADI_DMA_SUCCESS If successfully intialized the given channel
                - #ADI_DMA_ERR_NOT_INITIALIZED [D] if given channel is not initialized
                - #ADI_DMA_ERR_INVALID_CHANNEL [D] if channel param is invalid
                - #ADI_DMA_ERR_INVALID_ASSIGN [D] if channel/device pairing is invalid.

  @sa           adi_DMA_SubmitTransfer()
 */
ADI_DMA_RESULT_TYPE adi_DMA_SetChannelAssign(DMA_CHANn_TypeDef chNum, ADI_DMA_ASSIGN_TYPE controller)
{
	/* pointer to dedicated control register */
	uint16_t *pGPFDMACTL = (uint16_t*)0x40028100;

#ifdef ADI_DEBUG
    if (pDMA_DevData->InitState != ADI_DMA_STATE_INITIALIZED) {
        return ADI_DMA_ERR_NOT_INITIALIZED;
    }

    /* verify channel number */
    if (chNum != CRC_CHANn) {
        return ADI_DMA_ERR_INVALID_CHANNEL;
    }

    /* verify assignment */
    if ((ADI_DMA_ASSIGN_CRC != controller) && (ADI_DMA_ASSIGN_FLASH != controller)) {
        return ADI_DMA_ERR_INVALID_ASSIGN;
	}
#endif

	/* set/clear the sole "GPFDMACTL" register channel 13 control bit (LSB) */
	switch (controller) {
		case ADI_DMA_ASSIGN_CRC:
			*pGPFDMACTL = 0;
			break;
		case ADI_DMA_ASSIGN_FLASH:
			*pGPFDMACTL = 1;
			break;
	}
	return ADI_DMA_SUCCESS;
}

/* only ADuCM350 supports byte swap... */
/*!
  @brief        Configure uDMA driver to do destination byte swapping.
  @details      Configure channel #chNum to enable/disable byte swapping
                on the destination data.  Bytw swapping has special
                restructions concerning decrement mode and alignment
                requirements when submitting descriptors.
                See #adi_DMA_SubmitTransfer() for details.

  @param[in]    chNum           DMA channel number to configure
  @param[in]    flag            Flag variable:
                                  - true enable,
                                  - false disable.

  @return       Status
                - #ADI_DMA_SUCCESS If successfully intialized the given channel
                - #ADI_DMA_ERR_NOT_INITIALIZED [D] if given channel is not initialized
                - #ADI_DMA_ERR_INVALID_CHANNEL [D] if channel param is invalid

  @sa           adi_DMA_SubmitTransfer()
 */
ADI_DMA_RESULT_TYPE adi_DMA_SetByteSwap(DMA_CHANn_TypeDef chNum, bool_t flag)
{

#ifdef ADI_DEBUG
    if (pDMA_DevData->InitState != ADI_DMA_STATE_INITIALIZED) {
        return ADI_DMA_ERR_NOT_INITIALIZED;
    }

    /* verify channel number */
    if (chNum >= NUM_DMA_CHANNELSn) {
        return ADI_DMA_ERR_INVALID_CHANNEL;
    }
#endif

	/* update the private channel byteswap field...
       asserted in hardware during submit call */
	if (flag)
		pDMA_DevData->ByteSwapInUse |= (1ul << chNum);
	else
		pDMA_DevData->ByteSwapInUse &= ~(1ul << chNum);

    return ADI_DMA_SUCCESS;
}


/*!
  @brief        Submits the given DMA descriptor for transfer and enables the DMA

  @details      Initiates the DMA transfer on a given channel with the parameters
                passed through ADI_DMA_TRANSFER_TYPE structure.

                The ADuCM350 device offers two unique features: seperate control
                of source and destination pointer DMA reverse direction, as well
                as destination byte-swapping for stationary source pointers for
                use with perifheral FIFOs.

                When using pointer \a decrement mode, the associated data pointer
                must be initialized to the buffer END address.  This is opposit from
                the data pointer \a increment mode in which the buffer pointer must
                be initialized to the buffer START address.

                When using the byteswap mode, two restrictions apply: 1) the source
                data pointer must be stationary, and 2) the destination buffer start
                address, buffer end address and buffer size must all be 32-bit aligned.
                Byteswapping mode can be used with byte, halfword or fullword data sizees.

                It is in error to use the decrement mode with destination byteswapping.

  @note         This function automatically enables the DMA after programming the
                descriptors. Application should program the device specific registers
                to actually start the DMA after this function returns.

  @note         This function does not disable the DMA after the transfer is complete.
                Application should program the device specific registers to stop the
                DMA once the transfer is complete.

  @param[in]    pTransfer           Pointer to the structure which contains the
                                    required parameters to initiate the DMA

  @return       Status
                - #ADI_DMA_SUCCESS If successfully started the DMA with the given parameters
                - #ADI_DMA_ERR_NOT_INITIALIZED    [D] if the DMA driver is not initialized
                - #ADI_DMA_ERR_INVALID_BYTESWAP   [D] if invalid byteswap specified
                - #ADI_DMA_ERR_INVALID_CCD        [D] if the given CCD is invalid
                - #ADI_DMA_ERR_INVALID_CHANNEL    [D] if given channel is not in the valid range
                - #ADI_DMA_ERR_INVALID_DATAWIDTH  [D] if the given transfer width is not valid
                - #ADI_DMA_ERR_INVALID_INCREMENT  [D] if the given source or destination increment is not valid.
                - #ADI_DMA_ERR_INVALID_LENGTH     [D] if the given length excedes the maximum length that can be supported in a single transfer.
                - #ADI_DMA_ERR_INVALID_PROTECTION [D] if invalid protection specified

  @sa           adi_DMA_GetRemainingCount()
  @sa           adi_DMA_SetByteSwap()
 */
ADI_DMA_RESULT_TYPE adi_DMA_SubmitTransfer(ADI_DMA_TRANSFER_TYPE* pTransfer)
{
    ADI_DCC_TypeDef* pCCD;
    uint32_t channel_bit;

#ifdef ADI_DEBUG
    ADI_DMA_RESULT_TYPE Result;

    if (pDMA_DevData->InitState != ADI_DMA_STATE_INITIALIZED) {
        return ADI_DMA_ERR_NOT_INITIALIZED;
    }

    /* Validate the given parameters */
    if((Result = ValidateParams(pTransfer)) != ADI_DMA_SUCCESS) {
        return Result;
    }

    /* verify dma controller is enabled (assures clock is gated on) */
    if (!(pADI_DMA->DMASTA & (uint32_t)ADI_DMASTA_ENABLE))
        return ADI_DMA_ERR_NOT_INITIALIZED;

#endif /* ADI_DEBUG */

    /* point to the (pri/alt) channel control data structure of the submitted descriptor */
    pCCD = (ADI_DMA_CCD_PRIMARY == pTransfer->CCD) ? &pPrimaryCCD[pTransfer->Chan] : &pAlternateCCD[pTransfer->Chan];

    /* initialize the CCD endpoint addresses */
	pCCD->DMASRCEND = (uint32_t) pTransfer->pSrcData;
    pCCD->DMADSTEND = (uint32_t) pTransfer->pDstData;

	/* modify hardware register in a protected way */
    ADI_ENTER_CRITICAL_REGION();

	/* offset SOURCE endpoint (no adjust for stationary pointers) */
	switch (pTransfer->SrcInc) {
		case ADI_DMA_INCR_BYTE:
    	case ADI_DMA_INCR_HALFWORD:
    	case ADI_DMA_INCR_WORD:
			/* user pointer is buffer START address and DMA increments and stops at pointer PLUS (N-1)*size */
			pCCD->DMASRCEND += ((pTransfer->DataLength - 1ul) << pTransfer->SrcInc);
    		break;

		/* only ADuCM350 supports decrement mode */
		case ADI_DMA_DECR_BYTE:
		case ADI_DMA_DECR_HALFWORD:
		case ADI_DMA_DECR_WORD:
			/* user pointer is buffer END address and DMA decrements and stops at pointer MINUS (N-1)*size */
			pCCD->DMASRCEND -= ((pTransfer->DataLength - 1ul) << (pTransfer->SrcInc - ADI_DMA_DECR_BYTE));
    		break;
    		//AG:
		case ADI_DMA_DECR_NONE:
		case ADI_DMA_INCR_NONE:
			break;

	}

	/* offset DESTINATION endpoint (no adjust for stationary pointers) */
	switch (pTransfer->DstInc) {
		case ADI_DMA_INCR_BYTE:
    	case ADI_DMA_INCR_HALFWORD:
    	case ADI_DMA_INCR_WORD:
			/* user pointer is buffer START address and DMA increments and stops at pointer PLUS (N-1)*size */
			pCCD->DMADSTEND += ((pTransfer->DataLength - 1ul) << pTransfer->DstInc);
    		break;

		/* only ADuCM350 supports decrement mode */
		case ADI_DMA_DECR_BYTE:
		case ADI_DMA_DECR_HALFWORD:
		case ADI_DMA_DECR_WORD:
			/* user pointer is buffer END address and DMA decrements and stops at pointer MINUS (N-1)*size */
			pCCD->DMADSTEND -= ((pTransfer->DataLength - 1ul) << (pTransfer->DstInc - ADI_DMA_DECR_BYTE));
    		break;
    		//AG:
		case ADI_DMA_DECR_NONE:
		case ADI_DMA_INCR_NONE:
			break;

	}
	ADI_EXIT_CRITICAL_REGION();

    /* program the channel control data word */
    pCCD->DMACDC =
          (uint32_t)pTransfer->DstInc             << DMA_CCD_DST_INC_BITPOS

        /* ADuCM350 does not support seperate destination size param... only source */
        | (uint32_t)0                             << DMA_CCD_DST_SIZE_BITPOS
        | (uint32_t)pTransfer->SrcInc             << DMA_CCD_SRC_INC_BITPOS
        | (uint32_t)pTransfer->DataWidth          << DMA_CCD_SRC_SIZE_BITPOS

        /* ADuCM350 does not support protection mode */
        | (uint32_t)0                             << DMA_CCD_DST_HPROT_BITPOS
        | (uint32_t)0                             << DMA_CCD_SRC_HPROT_BITPOS
        | (uint32_t)pTransfer->ArbitRate          << DMA_CCD_RPOWER_BITPOS
        | (uint32_t)(pTransfer->DataLength - 1ul) << DMA_CCD_LEN_BITPOS
        /* note: useburst mode is unimplimented... */
        | (uint32_t)pTransfer->Mode               << DMA_CCD_MODE_BITPOS;


	/* compute the channel bit */
	channel_bit = 1ul << pTransfer->Chan;

	/* manage features unique to the ADuCM350 DMA enhancements */

	/* update source channel decrement mode */
	if (pTransfer->SrcInc >= ADI_DMA_DECR_BYTE) {
		pADI_DMA->DMASRCADSSET = channel_bit;
	} else {
		pADI_DMA->DMASRCADCLR = channel_bit;
	}

	/* update destination channel decrement mode */
	if (pTransfer->DstInc >= ADI_DMA_DECR_BYTE) {
		pADI_DMA->DMADSTADSET = channel_bit;
	} else {
		pADI_DMA->DMADSTADCLR = channel_bit;
	}

	/* update byte swap mode */
	if (pDMA_DevData->ByteSwapInUse & channel_bit) {
		pADI_DMA->DMABSSET = channel_bit;
	} else {
		pADI_DMA->DMABSCLR = channel_bit;
	}

    /* enable the channel on the DMA controller */
    pADI_DMA->DMAENSET = channel_bit;

    return ADI_DMA_SUCCESS;
}


/*!
  @brief        Re-submits the previous DMA descriptor for transfer and enables the DMA

  @details      Initiates the DMA transfer on a given channel with the parameters
                passed through ADI_DMA_TRANSFER_TYPE structure.

                This is a trimmed-down version of #adi_DMA_SubmitTransfer(), to
                minimize the overhead of programming the DMA when exactly the same
                transfer needs to be programmed repeatedly to a channel.

                It is assumed that the parameter is the pointer to the same structure that
                was used to program the DMA channel previously. Moreover, the structure
                itself should be identical to the previously used one.

                \b IMPORTANT: \b this function does not verify the assumptions above. It is
                the responsibility of the user to ensure the parameter has not changed.
                Failure to do so may result in unpredictable behaviour of the ADuCM350!

                If in any doubt, use #adi_DMA_SubmitTransfer() instead.

  @note         This function automatically enables the DMA after programming the
                descriptors. Application should program the device specific registers
                to actually start the DMA after this function returns.

  @note         This function does not disable the DMA after the transfer is complete.
                Application should program the device specific registers to stop the
                DMA once the transfer is complete.

  @param[in]    pTransfer           Pointer to the structure which contains the
                                    required parameters to initiate the DMA

  @return       Status
                - #ADI_DMA_SUCCESS If successfully started the DMA with the given parameters
                - #ADI_DMA_ERR_NOT_INITIALIZED    [D] if the DMA driver is not initialized
                - #ADI_DMA_ERR_INVALID_BYTESWAP   [D] if invalid byteswap specified
                - #ADI_DMA_ERR_INVALID_CCD        [D] if the given CCD is invalid
                - #ADI_DMA_ERR_INVALID_CHANNEL    [D] if given channel is not in the valid range
                - #ADI_DMA_ERR_INVALID_DATAWIDTH  [D] if the given transfer width is not valid
                - #ADI_DMA_ERR_INVALID_INCREMENT  [D] if the given source or destination increment is not valid.
                - #ADI_DMA_ERR_INVALID_LENGTH     [D] if the given length excedes the maximum length that can be supported in a single transfer.
                - #ADI_DMA_ERR_INVALID_PROTECTION [D] if invalid protection specified

  @sa           adi_DMA_SubmitTransfer()
 */
ADI_DMA_RESULT_TYPE adi_DMA_ReSubmit(ADI_DMA_TRANSFER_TYPE* pTransfer) {
    ADI_DCC_TypeDef*  pCCD;
    uint32_t          channel_bit;
    uint32_t          cdc;

#ifdef ADI_DEBUG
    ADI_DMA_RESULT_TYPE Result;

    if (pDMA_DevData->InitState != ADI_DMA_STATE_INITIALIZED) {
        return ADI_DMA_ERR_NOT_INITIALIZED;
    }

    /* Validate the given parameters */
    if((Result = ValidateParams(pTransfer)) != ADI_DMA_SUCCESS) {
        return Result;
    }

    /* verify dma controller is enabled (assures clock is gated on) */
    if (!(pADI_DMA->DMASTA & (uint32_t)ADI_DMASTA_ENABLE))
        return ADI_DMA_ERR_NOT_INITIALIZED;

#endif /* ADI_DEBUG */

    /* point to the primary descriptor */
    pCCD = &pPrimaryCCD[pTransfer->Chan];

    /* compute the channel bit */
    channel_bit = 1ul << pTransfer->Chan;

    ADI_ENTER_CRITICAL_REGION();

    /* get existing CDC, modify and write */
    cdc = pCCD->DMACDC;
    cdc &= DMA_CCD_RESUBMIT_CLEAR_MASK;
    cdc |= (uint32_t)(pTransfer->DataLength - 1ul) << DMA_CCD_LEN_BITPOS |
           (uint32_t)pTransfer->Mode               << DMA_CCD_MODE_BITPOS;
    pCCD->DMACDC = cdc;

    ADI_EXIT_CRITICAL_REGION();

    /* enable the channel on the DMA controller */
    pADI_DMA->DMAENSET = channel_bit;

    return ADI_DMA_SUCCESS;

}




/*!
  @brief        Returns the remaining number of transfers
  @details      Returns the remaining number of elements to be transferred in the
                given channel control data for the given channel.

  @note         This function returns the actual number of remaining elements (Not N-1)
                to be transferred as that is what the application provides.

  @param[in]    chNum           DMA channel number
  @param[out]   pCount          Number of remaining transfers
  @param[in]    ccd             Channel control data from which the remaining count to
                                be read
  @return       Status
                - #ADI_DMA_SUCCESS If successfully obtained the remaining count
                - #ADI_DMA_ERR_NOT_INITIALIZED [D] DMA driver is not initialized

  @sa           adi_DMA_SubmitTransfer()
 */
ADI_DMA_RESULT_TYPE adi_DMA_GetRemainingCount(DMA_CHANn_TypeDef chNum, uint32_t *pCount, ADI_DMA_CCD_TYPE ccd)
{

    ADI_DCC_TypeDef *pCCD;

#ifdef ADI_DEBUG
    if (pDMA_DevData->InitState != ADI_DMA_STATE_INITIALIZED) {
        return ADI_DMA_ERR_NOT_INITIALIZED;
    }
#endif

    if(ccd == ADI_DMA_CCD_PRIMARY) {
        /* get the channel control data */
        pCCD = &pPrimaryCCD[chNum];
    } else {
        /* get the channel control data */
        pCCD = &pAlternateCCD[chNum];
    }

    /* Return the count. The count stored in the MMR is N-1 (Where N is
       the number of elements to transfer) so add 1 to get actual count.
       (but toss any carries out of the 10-bit width field with mod 1024)*/
    *pCount = (((pCCD->DMACDC & (uint32_t)DMA_CCD_LEN_MASK) >> DMA_CCD_LEN_BITPOS) + 1u) & 0x3ff;

    return ADI_DMA_SUCCESS;
}


/*!
  @brief        Returns the status of the controller

  @param[out]    pStatusResult          Pointer to a location where the controller state
                                        is written
  @return       Status
                - #ADI_DMA_SUCCESS If successfully obtained the state of the controller
                - #ADI_DMA_ERR_INVALID_STATE The state read from controller is invalid
                - #ADI_DMA_ERR_UNKNOWN an unkown error occured while reading the controller state

*/
ADI_DMA_RESULT_TYPE adi_DMA_GetState(ADI_DMA_STATE_TYPE* pStatusResult) {

    ADI_DMA_RESULT_TYPE    rtn = ADI_DMA_ERR_UNKNOWN;
    uint32_t state = pADI_DMA->DMASTA & (uint32_t)DMASTA_STATE_MSK;

    switch (state) {

        case ADI_DMA_STATE_IDLE:
        case ADI_DMA_STATE_READ_CHNL_CTRL_DATA:
        case ADI_DMA_STATE_READ_SRC_EPTR:
        case ADI_DMA_STATE_READ_DST_EPTR:
        case ADI_DMA_STATE_READ_SRC_DATA:
        case ADI_DMA_STATE_WRITE_DST_DATA:
        case ADI_DMA_STATE_WAIT_REQ_CLR:
        case ADI_DMA_STATE_WRITE_CHNL_CRTL_DATA:
        case ADI_DMA_STATE_STALLED:
        case ADI_DMA_STATE_DONE:
        case ADI_DMA_STATE_PSG_TRANSITION:
            *(uint32_t*)pStatusResult = state;
            rtn = ADI_DMA_SUCCESS;
            break;

        default:
            *pStatusResult = ADI_DMA_STATE_INVALID;
            rtn = ADI_DMA_ERR_INVALID_STATE;
            break;
    }

    return rtn;
}


#ifdef ADI_DEBUG

/* Validates given parameters */
static ADI_DMA_RESULT_TYPE ValidateParams(ADI_DMA_TRANSFER_TYPE *pTransfer)
{
    /* verify the channel is initialized and 'in-use' */
    if ( !((pDMA_DevData->ChannelsInUse) & (1ul <<pTransfer->Chan)) ) {
        return ADI_DMA_ERR_INVALID_CHANNEL;
    }

    /* limit to 1024 for now (chunk up using RPOWER later) */
    if (pTransfer->DataLength > ADI_DMA_MAX_TRANSFER_SIZE) {
        return ADI_DMA_ERR_INVALID_LENGTH;
    }

	/* check for invalid use of unsupported protection bits */
	if (ADI_DMA_HPROT_NONE != pTransfer->Protection) {
        return ADI_DMA_ERR_INVALID_PROTECTION;
	}

	/* only ADuCM350 supports byte stapping */

	/* special constraints on byte swapping */
	if (pDMA_DevData->ByteSwapInUse & (1ul << pTransfer->Chan)) {

		/* byte swap requires fully 32-bit aligned buffers and buffer size */
		if (((uint32_t)pTransfer->pSrcData%4) || ((uint32_t)pTransfer->pDstData%4) || (pTransfer->DataLength%4)) {
			return ADI_DMA_ERR_INVALID_BYTESWAP;
		}

		/* byte swap requires a stationary source pointer */
		switch (pTransfer->SrcInc) {
			case ADI_DMA_INCR_NONE:
			case ADI_DMA_DECR_NONE:
				break;
			default:
				return ADI_DMA_ERR_INVALID_BYTESWAP;
		}

		/* disallow byte-swap-with-decrement combinations */
		/* we just checked that the source pointer is stationary,
           so only need to check destination increment... */
		if (pTransfer->DstInc >= ADI_DMA_DECR_BYTE) {
			return ADI_DMA_ERR_INVALID_BYTESWAP;
		}
	}

    /* increments are constrained by the datawidth */
    switch (pTransfer->DataWidth) {

        case ADI_DMA_WIDTH_BYTE:
            break;

        case ADI_DMA_WIDTH_HALFWORD:
            if (   (pTransfer->SrcInc == ADI_DMA_INCR_BYTE)
                || (pTransfer->DstInc == ADI_DMA_INCR_BYTE)

		/* only ADuCM350 supports decrement mode */
            	|| (pTransfer->SrcInc == ADI_DMA_DECR_BYTE)
                || (pTransfer->DstInc == ADI_DMA_DECR_BYTE)
				) {
                	return ADI_DMA_ERR_INVALID_INCREMENT;
                }
            break;

        case ADI_DMA_WIDTH_WORD:
            if (   (pTransfer->SrcInc == ADI_DMA_INCR_BYTE)
                || (pTransfer->DstInc == ADI_DMA_INCR_BYTE)
                || (pTransfer->SrcInc == ADI_DMA_INCR_HALFWORD)
                || (pTransfer->DstInc == ADI_DMA_INCR_HALFWORD)

		/* only ADuCM350 supports decrement mode */
            	|| (pTransfer->SrcInc == ADI_DMA_DECR_BYTE)
                || (pTransfer->DstInc == ADI_DMA_DECR_BYTE)
            	|| (pTransfer->SrcInc == ADI_DMA_DECR_HALFWORD)
                || (pTransfer->DstInc == ADI_DMA_DECR_HALFWORD)
                ) {
                	return ADI_DMA_ERR_INVALID_INCREMENT;
            }
            break;

        default:
            return ADI_DMA_ERR_INVALID_DATAWIDTH;
    }

    return ADI_DMA_SUCCESS;
}
#endif /* ADI_DEBUG */

#ifdef ADI_DEBUG
#include "stdio.h"  /* for printf */
static void printDescriptor(ADI_DCC_TypeDef* pD);
static void printDescriptor(ADI_DCC_TypeDef* pD)
{
    uint32_t cfg = pD->DMACDC;

    printf("    src endpoint:  0x%08x\n", pD->DMASRCEND);
    printf("    dst endpoint:  0x%08x\n", pD->DMADSTEND);

    printf("    cycle control: %d\n", (cfg & 0x00000007ul) >> DMA_CCD_MODE_BITPOS);
    printf("    burst mode:    %d\n", (cfg & 0x00000008ul) >> DMA_CCD_USEBURST_BITPOS);
    printf("    N-1:           %d\n", (cfg & 0x00003ff0ul) >> DMA_CCD_LEN_BITPOS);
    printf("    rpower:        %d\n", (cfg & 0x0003c000ul) >> DMA_CCD_RPOWER_BITPOS);
    printf("    src hprot:     %d\n", (cfg & 0x001c0000ul) >> DMA_CCD_SRC_HPROT_BITPOS);
    printf("    dst hprot:     %d\n", (cfg & 0x00e00000ul) >> DMA_CCD_DST_HPROT_BITPOS);
    printf("    src size:      %d\n", (cfg & 0x03000000ul) >> DMA_CCD_SRC_SIZE_BITPOS);
    printf("    src inc:       %d\n", (cfg & 0x0c000000ul) >> DMA_CCD_SRC_INC_BITPOS);
    printf("    dst size:      %d\n", (cfg & 0x30000000ul) >> DMA_CCD_DST_SIZE_BITPOS);
    printf("    dst inc:       %d\n", (cfg & 0xc0000000ul) >> DMA_CCD_DST_INC_BITPOS);
}

/* dump descriptors for a given channel */
void adi_DMA_PrintDescriptors(DMA_CHANn_TypeDef chNum)
{
    printf("DMA channel %d descriptors:\n", chNum);

    /* dump the descriptors */
    printf("  primary...\n");
    printDescriptor(&pPrimaryCCD[chNum]);

    /* dump the alternale descriptor */
    printf("  alternate...\n");
    printDescriptor(&pAlternateCCD[chNum]);
}

#endif /* ADI_DEBUG */

#define REVERT_ADI_MISRA_SUPPRESSIONS  /*!< Revert ADI MISRA Suppressions */
#include "misra.h"

/*
** EOF
*/

/*@}*/
