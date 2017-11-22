/*********************************************************************************

Copyright (c) 2010-2014 Analog Devices, Inc. All Rights Reserved.

This software is proprietary to Analog Devices, Inc. and its licensors.  By using 
this software you agree to the terms of the associated Analog Devices Software 
License Agreement.

*********************************************************************************/

/*!
 *****************************************************************************
 * @file:    adi_nvic.c
 * @brief:   NVIC controller API implementation file.
 * @version: $Revision: 28525 $
 * @date:    $Date: 2014-11-12 14:51:26 -0500 (Wed, 12 Nov 2014) $
*****************************************************************************/

#include <device.h>
#include <adi_nvic.h>

/* Default NVIC interrupt handler */
static void adi_nvic_DefaultHandler(
    void
);

/*!
  @brief        Registers interrupt handler with NVIC.
  @details      The given interrupt handler is written into the NVIC vector table.

  @param[in]    iid             Interrupt ID for which the handler to be registered.
  @param[in]    pfHandler       Pointer to the interrupt handler function.

  @return       Status
                - Index - Upon success returns the index to the interrupt vector table.
                - (-1)  - Upon failure.

  @sa           adi_nvic_UnRegisterHandler()
 */
int32_t adi_nvic_RegisterHandler (
        uint32_t                        iid,
        ADI_NVIC_HANDLER                pfHandler
)
{
	/* get the IVT pointer from the NVIC offset register in case its been relocated */
	ADI_NVIC_HANDLER *pIVT = (ADI_NVIC_HANDLER *)SCB->VTOR;

    uint32_t index = ADI_NVIC_SID_IRQ(iid);
#ifdef ADI_DEBUG
    if(iid >= ADI_INT_NUM_INT)
    {
        return -1;
    }
#endif /* ADI_DEBUG */

    __disable_irq();

    pIVT[index] = pfHandler;

    __enable_irq();

    return index;
}


/*!
  @brief        UnRegisters interrupt handler with NVIC.
  @details      Writes back default interrupt handler into the NVIC vector table.

  @param[in]    iid             Interrupt ID for which the handler to be unregistered.

  @return       Status
                - Index - Upon success returns the index to the interrupt vector table.
                - (-1)  - Upon failure.

  @sa           adi_nvic_RegisterHandler()
 */
int32_t adi_nvic_UnRegisterHandler (
        uint32_t                        iid
)
{
	/* get the IVT from the NVIC offset register in case its been relocated */
	ADI_NVIC_HANDLER *pIVT = (ADI_NVIC_HANDLER *)SCB->VTOR;

    uint32_t index = ADI_NVIC_SID_IRQ(iid);
#ifdef ADI_DEBUG
    if(iid >= ADI_INT_NUM_INT)
    {
        return -1;
    }
#endif /* ADI_DEBUG */

    __disable_irq();

    pIVT[index] = adi_nvic_DefaultHandler;

    __enable_irq();

    return index;
}



/*!
  @brief        Enables/Disables interrupt.

  @param[in]    iid             Interrupt ID of the interrupt to be enabled/disabled.
  @param[in]    bEnable         Flag which indicates whether to enable/disable.
                                'true'  - to Enable
                                'false' - to Disable

  @return       Status
                ADI_NVIC_SUCCESS        Successfully enabled/disabled the interrupt.
                ADI_NVIC_INVALID_INT[D] If the given interrupt ID is invalid or
                                        the given interrupt ID is a System Exception.
                                        System exception cannot be disabled.

  @sa           adi_nvic_register_handler()
 */
ADI_NVIC_RESULT adi_nvic_EnableInt(
    uint32_t    iid,
    bool        bEnable
)
{
#ifdef ADI_DEBUG
  if((iid >= ADI_INT_NUM_INT) || (iid <= 0))
  {
    return ADI_NVIC_INVALID_INT;
  }
#endif /* ADI_DEBUG */

  if(bEnable == true)
  {
    NVIC_EnableIRQ((IRQn_Type)iid);
  }
  else
  {
    NVIC_DisableIRQ((IRQn_Type)iid);
  }

  return ADI_NVIC_SUCCESS;
}

/* Default interrupt handler */
static void adi_nvic_DefaultHandler(
    void
)
{
    /* Un handled interrupt trap here */
    while(1);
}
