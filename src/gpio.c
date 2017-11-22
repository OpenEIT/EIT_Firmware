/*********************************************************************************

Copyright (c) 2010-2014 Analog Devices, Inc. All Rights Reserved.

This software is proprietary to Analog Devices, Inc. and its licensors.  By using 
this software you agree to the terms of the associated Analog Devices Software 
License Agreement.

*********************************************************************************/

/*!
 *****************************************************************************
   @file:    gpio.c
   @brief:   GPIO device driver implementation for ADuCxxx
   @version: $Revision: 28700 $
   @date:    $Date: 2014-11-19 16:18:59 -0500 (Wed, 19 Nov 2014) $
*/
/*****************************************************************************/

/*! \addtogroup GPIO_Driver GPIO Driver
 *  GPIO device driver
 *  @{
 */

/*! \cond PRIVATE */

#define ASSERT_ADI_MISRA_SUPPRESSIONS  /*!< Apply ADI MISRA Suppressions */
#include "misra.h"

/* Include files */
#include "gpio.h"

/* ADuCM350 is first to use CODA-generated macros and device mappings. */
/* define file-local-scope aliases to map old-style names to CODA-style */
#define GPIPOL GPPOL   /*!< name mapping macro */
#define GPISTAT GPINT  /*!< name mapping macro */

/* Debug function declarations */
#ifdef ADI_DEBUG

static bool_t IsPortValid       (const ADI_GPIO_PORT_TYPE Port);                        /*!< tests for port validity */
static bool_t ArePinsValid      (const ADI_GPIO_DATA_TYPE Pins);                        /*!< tests for pins validity */

static bool_t IsInterruptValid  (const IRQn_Type eIrq);                                 /*!< tests for valid interrupt number */
static bool_t IsTriggerValid    (const ADI_GPIO_IRQ_TRIGGER_CONDITION_TYPE eCondition); /*!< tests for valid trigger condition */

#endif /* ADI_DEBUG */


/* GPIO power-up default register values */
#define ADI_GPIO_DEFAULT_GPCON  (0)   /*!< configuration register */
#define ADI_GPIO_DEFAULT_GPOEN  (0)   /*!< output enable register */

/* GPIO pull-up/poll-down */
#define ADI_GPIO_GP0PE_RESET    (0x03C0)  /*!< GP0PE reset value */
#define ADI_GPIO_DEFAULT_GPPE   (0)       /*!< pull-enable register */
#define ADI_GPIO_DEFAULT_GPOCE  (0)       /*!< open-circuit enable register */

/* additional registers for M350 */
#define ADI_GPIO_DEFAULT_GPIPOL  (0)   /*!< interrupt polarity */
#define ADI_GPIO_DEFAULT_GPIENA  (0)   /*!< interrupt A enable */
#define ADI_GPIO_DEFAULT_GPIENB  (0)   /*!< interrupt B enable */
#define ADI_GPIO_DEFAULT_GPISTAT (0)   /*!< interrupt status */


/*! local enum for callback table indexing */
typedef enum
{
    /* dedicated external pin interrupts... */
    ADI_GPIO_IRQ_EXT0_INDEX       = (0x0),   /*!< External interrupt 0.    */
    ADI_GPIO_IRQ_EXT1_INDEX       = (0x1),   /*!< External interrupt 1.    */
    ADI_GPIO_IRQ_EXT2_INDEX       = (0x2),   /*!< External interrupt 2.    */
    ADI_GPIO_IRQ_EXT3_INDEX       = (0x3),   /*!< External interrupt 3.    */
    ADI_GPIO_IRQ_EXT4_INDEX       = (0x4),   /*!< External interrupt 4.    */
    ADI_GPIO_IRQ_EXT5_INDEX       = (0x5),   /*!< External interrupt 5.    */
    ADI_GPIO_IRQ_EXT6_INDEX       = (0x6),   /*!< External interrupt 6.    */
    ADI_GPIO_IRQ_EXT7_INDEX       = (0x7),   /*!< External interrupt 7.    */
    ADI_GPIO_IRQ_EXT8_INDEX       = (0x8),   /*!< External interrupt 8.    */

    /* 350 also has two group interrupts */
    ADI_GPIO_IRQ_GROUPA_INDEX     = (0x9),   /*!< GroupA interrupt index.   */
    ADI_GPIO_IRQ_GROUPB_INDEX     = (0xA),   /*!< GroupB interrupt index.   */

    ADI_GPIO_NUM_INTERRUPTS       = (11),    /*!< Number of GPIO interrupts */

} ADI_GPIO_IRQ_INDEX_TYPE;


/* General macros */
#define ADI_GPIO_EIxCFG_SHIFT_COUNT (2) /*!< number of bits to shift for each interrupt */

/*! Structure to hold callback function and parameter */
typedef struct
{
    ADI_CALLBACK         pfCallback;    /*!< Callback function pointer */
    void *               pCBParam;      /*!< Callback parameter */

} GPIO_CALLBACK_INFO;


/*! \struct ADI_GPIO_DEVICE_TYPE

    GPIO instance data

    This structure contains the "state" information for the
    instance of the device.  For GPIO there is only one
    of these objects.
*/
typedef struct
{
    int32_t                     ReferenceCount;   /*!< initialization state */

    GPIO_CALLBACK_INFO          CallbackTable[ADI_GPIO_NUM_INTERRUPTS];  /*!< Callback Info for External interrupts */

} ADI_GPIO_DEVICE_TYPE;


/* forwward declarations */

/* These are needed in the case where ADI_CFG_ENABLE_RTOS_SUPPORT=1              */
/* The ADI_INSTALL_HANDLER macro will expand to a call to adi_int_InstallHandler */
/* and the handler will be referenced                                            */
/* The non-OSAL expansion of ADI_INSTALL_HANDLER results in a NULL string        */
ADI_INT_HANDLER(Ext_Int0_Handler);
ADI_INT_HANDLER(Ext_Int1_Handler);
ADI_INT_HANDLER(Ext_Int2_Handler);
ADI_INT_HANDLER(Ext_Int3_Handler);
ADI_INT_HANDLER(Ext_Int4_Handler);
ADI_INT_HANDLER(Ext_Int5_Handler);
ADI_INT_HANDLER(Ext_Int6_Handler);
ADI_INT_HANDLER(Ext_Int7_Handler);
ADI_INT_HANDLER(Ext_Int8_Handler);

ADI_INT_HANDLER(GPIO_A_Int_Handler);
ADI_INT_HANDLER(GPIO_B_Int_Handler);

/*! GPIO local state data */
static ADI_GPIO_DEVICE_TYPE adi_GPIO_Device = {0};


/*! \endcond */


/*****************************************************************************/
/*!

    @brief      Initializes the GPIO functions.

    @details    This function does the GPIO Initialization. This function need be called before calling any of the GPIO
                service APIs. This function installs the interrupt handlers for all external interrupts.

    @return     Status
                    - ADI_GPIO_SUCCESS if successfully initialized

    @note       This function clears memory reserved for managing the callback function when it is called for the first time.
                It is expected from user to call "adi_GPIO_UnInit" function wehn the GPIO service is no longer requred.

    @sa         adi_GPIO_UnInit

*/
/*****************************************************************************/
ADI_GPIO_RESULT_TYPE adi_GPIO_Init(void)
{

    ADI_GPIO_RESULT_TYPE Result;

    /* ever the optimist */
    Result = ADI_GPIO_SUCCESS;

    /* only initialize on 1st init call, i.e., preserve callbacks on multiple inits */
    if (!adi_GPIO_Device.ReferenceCount++) {

        /* (re)initialize the callback table */
        for (int i = 0; i < ADI_GPIO_NUM_INTERRUPTS; i++)
    {
            adi_GPIO_Device.CallbackTable[i].pfCallback = NULL;
            adi_GPIO_Device.CallbackTable[i].pCBParam   = NULL;
    }

        /* (re)install the handlers for external interrupts */
    ADI_INSTALL_HANDLER(ADI_INT_EINT0, Ext_Int0_Handler);
    ADI_INSTALL_HANDLER(ADI_INT_EINT1, Ext_Int1_Handler);
    ADI_INSTALL_HANDLER(ADI_INT_EINT2, Ext_Int2_Handler);
    ADI_INSTALL_HANDLER(ADI_INT_EINT3, Ext_Int3_Handler);
    ADI_INSTALL_HANDLER(ADI_INT_EINT4, Ext_Int4_Handler);
    ADI_INSTALL_HANDLER(ADI_INT_EINT5, Ext_Int5_Handler);
    ADI_INSTALL_HANDLER(ADI_INT_EINT6, Ext_Int6_Handler);
    ADI_INSTALL_HANDLER(ADI_INT_EINT7, Ext_Int7_Handler);
    ADI_INSTALL_HANDLER(ADI_INT_EINT8, Ext_Int8_Handler);

    /* (re)install handlers for group interrupts */
    ADI_INSTALL_HANDLER(ADI_INT_GPIOA, GPIO_A_Int_Handler);
    ADI_INSTALL_HANDLER(ADI_INT_GPIOB, GPIO_B_Int_Handler);

    }

    /* return */
    return (Result);
}


/*****************************************************************************/
/*!

    @brief      GPIO Uninitalization

    @details    Terminates the GPIO functions, leaving everything unchanged

    @return     Status
                    - #ADI_GPIO_SUCCESS if successfully uninitialized
                    - #ADI_GPIO_ERR_NOT_INITIALIZED [D] if not yet initialized

    @note       At some point in the future, this function may actually do something
                useful. For the time being, it's only functionality is to clear
                the flag that indicates the GPIOs have been initialized. In debug
                mode, all the GPIO functions test to make sure the GPIOs have been
                initialized properly so that eventually, when this function does
                do something useful, we can rely on the fact that existing software
                called this function and the adi_GPIO_Init() function properly.

    @sa         adi_GPIO_Init

*/
/*****************************************************************************/
ADI_GPIO_RESULT_TYPE adi_GPIO_UnInit(void)
{

    ADI_GPIO_RESULT_TYPE Result;

    /* ever the optimist */
    Result = ADI_GPIO_SUCCESS;

#ifdef ADI_DEBUG
    /* IF (not initialized) */
    if (0 == adi_GPIO_Device.ReferenceCount)
    {
        /* return error if not initialized */
        Result = ADI_GPIO_ERR_NOT_INITIALIZED;

    /* ELSE */
    } else {

        /* decrement the reference count */
        adi_GPIO_Device.ReferenceCount--;

    /* ENDIF */
    }
#endif

    /* return */
    return (Result);
}


/*****************************************************************************/
/*!

    @brief      Reset to Power Up Default Values

    @details    Sets all GPIO registers to their power-up default values

    @return     Status
                    - #ADI_GPIO_SUCCESS if successful
                    - #ADI_GPIO_ERR_NOT_INITIALIZED [D] if not yet initialized

*/
/*****************************************************************************/
ADI_GPIO_RESULT_TYPE adi_GPIO_ResetToPowerUp(void)
{

    ADI_GPIO_TypeDef *PortTable[] = {       /* table of port base registers */
        pADI_GP0,       /* port 0 base address */
        pADI_GP1,       /* port 1 base address */
        pADI_GP2,       /* port 2 base address */
        pADI_GP3,       /* port 3 base address */
        pADI_GP4,       /* port 4 base address */
    };


    ADI_GPIO_TypeDef    *pPort; /* pointer to port registers */
    uint32_t            i;      /* counter */

#ifdef ADI_DEBUG
    /* make sure we're initialized */
    if (adi_GPIO_Device.ReferenceCount == 0) {
        return (ADI_GPIO_ERR_NOT_INITIALIZED);
    }
#endif

    /* FOR (each port) */
    for (i = 0; i < ((sizeof(PortTable))/(sizeof(PortTable[0]))); i++) {

        /* point to the register block */
        pPort = PortTable[i];

        /* reset the registers */
        pPort->GPCON = ADI_GPIO_DEFAULT_GPCON;
        pPort->GPOEN = ADI_GPIO_DEFAULT_GPOEN;

        /* 350 has single pull enable register for either hard-wired pull direction */
        if (pPort == pADI_GP0)
        {
            pPort->GPPE = ADI_GPIO_GP0PE_RESET;  /* For Port0 don't configure JTAG/SWD pins */
        }
        else
        {
            pPort->GPPE = ADI_GPIO_DEFAULT_GPPE;
        }

        /* additional interrupt registers for M350 */
        pPort->GPIPOL  = ADI_GPIO_DEFAULT_GPIPOL;
        pPort->GPIENA  = ADI_GPIO_DEFAULT_GPIENA;
        pPort->GPIENB  = ADI_GPIO_DEFAULT_GPIENB;
        pPort->GPISTAT = ADI_GPIO_DEFAULT_GPISTAT;

    /* ENDFOR */
    }

    /* return */
    return (ADI_GPIO_SUCCESS);
}

/*****************************************************************************/
/*!

    @brief      Retrieve Pin Muxing for a Port

    @details    Retrieves the pin muxing for a port

    @param[in]  Port    Identifies the GPIO port to be sensed (ADI_GPIO_PORT_x)

    @return     Status
                    - #ADI_GPIO_SUCCESS if successfully retrieved
                    - #ADI_GPIO_ERR_NOT_INITIALIZED [D] if not yet initialized
                    - #ADI_GPIO_ERR_INVALID_PORT [D] if the Port parameter is invalid

*/
/*****************************************************************************/
ADI_GPIO_CONFIG_TYPE adi_GPIO_RetrievePinMuxing (const ADI_GPIO_PORT_TYPE Port)
{

    ADI_GPIO_TypeDef    *pPort;     /* pointer to port registers */

#ifdef ADI_DEBUG
    /* make sure we're initialized */
    if (adi_GPIO_Device.ReferenceCount == 0) {
        return (ADI_GPIO_ERR_NOT_INITIALIZED);
    }

    /* validate the port */
    if (!IsPortValid(Port)) {
        return (ADI_GPIO_ERR_INVALID_PORT);
    }
#endif

    /* point to the port */
    pPort = (ADI_GPIO_TypeDef *)Port;

    /* return the value from the mux */
    return (pPort->GPCON);
}


/*****************************************************************************/
/*!

    @brief      Enable an External Interrupt

    @details    Enables and sets the triggering mode for the given external interrupt.
                Applications may register a callback using the #adi_GPIO_RegisterCallback
                API to get a notification when the interrupt occures.

    @param[in]  eIrq        Identifier of type IRQn_Type that specifies the external
                            interrupt that is to be enabled

    @param[in]  eCondition  Identifier of type ADI_GPIO_IRQ_TRIGGER_CONDITION_TYPE
                            that specifies the trigger condition for the interrupt

    @return     Status
                    - #ADI_GPIO_SUCCESS if successfully enabled
                    - #ADI_GPIO_ERR_NOT_INITIALIZED [D] if not yet initialized
                    - #ADI_GPIO_ERR_INVALID_INTERRUPT [D] if the eIrq parameter is invalid
                    - #ADI_GPIO_ERR_INVALID_TRIGGER [D] if the eCondition parameter is invalid

    @sa         adi_GPIO_DisableIRQ
    @sa         adi_GPIO_RegisterCallback

*/
/*****************************************************************************/
ADI_GPIO_RESULT_TYPE adi_GPIO_EnableIRQ (const IRQn_Type eIrq, const ADI_GPIO_IRQ_TRIGGER_CONDITION_TYPE eCondition)
{

    uint16_t volatile   *pIRQCFG;   /* pointer to the appropriate interrupt config register */
    uint16_t            Mask;       /* mask to manipulate the register */
    uint16_t            Pattern;    /* bit pattern that will be written into the register */
    uint32_t            ShiftCount; /* shift count */
    uint16_t            CfgReg;     /* interrupt config register value */

#ifdef ADI_DEBUG
    /* make sure we're initialized */
    if (adi_GPIO_Device.ReferenceCount == 0) {
        return (ADI_GPIO_ERR_NOT_INITIALIZED);
    }

    /* validate the interrupt number */
    if (!IsInterruptValid(eIrq)) {
        return (ADI_GPIO_ERR_INVALID_INTERRUPT);
    }

    /* validate the trigger condition */
    if (!IsTriggerValid(eCondition)) {
        return (ADI_GPIO_ERR_INVALID_TRIGGER);
    }
#endif

    /* figure out the shift count and which config register we need to change */
    switch (eIrq) {
        case EINT0_IRQn:
        case EINT1_IRQn:
        case EINT2_IRQn:
        case EINT3_IRQn:
            /* external interrupts 0-3 */
            pIRQCFG = &pADI_IDU->EI0CFG;
            ShiftCount = (eIrq - EINT0_IRQn) << ADI_GPIO_EIxCFG_SHIFT_COUNT;
            break;

        case EINT4_IRQn:
        case EINT5_IRQn:
        case EINT6_IRQn:
        case EINT7_IRQn:
            /* external interrupts 4-7 */
            pIRQCFG = &pADI_IDU->EI1CFG;
            ShiftCount = (eIrq - EINT4_IRQn) << ADI_GPIO_EIxCFG_SHIFT_COUNT;
            break;

        case EINT8_IRQn:
            /* external interrupt 8 */
            pIRQCFG = &pADI_IDU->EI2CFG;
            ShiftCount = (eIrq - EINT8_IRQn) << ADI_GPIO_EIxCFG_SHIFT_COUNT;
            break;

    }

    /* create the mask we'll use to clear the relevant bits in the config register */
    Mask =  (EI0CFG_IRQ0MDE_MSK | EI0CFG_IRQ0EN) << ShiftCount;

    /* create the bit pattern we're going to write into the configuration register */
    Pattern = (EI0CFG_IRQ0EN | eCondition) << ShiftCount;

    /*********** need to protect this sequence *****/
    ADI_ENTER_CRITICAL_REGION();

    /* read/modify/write the appropriate bits in the register */
    CfgReg = *pIRQCFG;
    CfgReg &= ~Mask;
    CfgReg |= Pattern;
    *pIRQCFG = CfgReg;

    /*********** turn off protection *****/
    ADI_EXIT_CRITICAL_REGION();

    /* enable the interrupt */
    NVIC_EnableIRQ(eIrq);

    /* return */
    return (ADI_GPIO_SUCCESS);
}


/*****************************************************************************/
/*!

    @brief      Disable an External Interrupt

    @details    Disables an external interrupt

    @param[in]  eIrq    Identifier of type IRQn_Type that specifies the external
                        interrupt that is to be disabled

    @return     Status
                    - #ADI_GPIO_SUCCESS if successfully disabled
                    - #ADI_GPIO_ERR_NOT_INITIALIZED [D] if not yet initialized
                    - #ADI_GPIO_ERR_INVALID_INTERRUPT [D] if the eIrq parameter is invalid

    @sa         adi_GPIO_EnableIRQ
    @sa         adi_GPIO_RegisterCallback

*/
/*****************************************************************************/
ADI_GPIO_RESULT_TYPE adi_GPIO_DisableIRQ (const IRQn_Type eIrq)
{

    uint16_t volatile   *pIRQCFG;   /* pointer to the appropriate interrupt config register */
    uint16_t            Mask;       /* mask to manipulate the register */
    uint32_t            ShiftCount; /* shift count */
    uint16_t            CfgReg;     /* interrupt config register value */

#ifdef ADI_DEBUG
    /* make sure we're initialized */
    if (adi_GPIO_Device.ReferenceCount == 0) {
        return (ADI_GPIO_ERR_NOT_INITIALIZED);
    }

    /* validate the interrupt number */
    if (!IsInterruptValid(eIrq)) {
        return (ADI_GPIO_ERR_INVALID_INTERRUPT);
    }
#endif

    /* disable the interrupt */
    NVIC_DisableIRQ(eIrq);

    /* figure out the shift count and which config register we need to change */
    switch (eIrq) {
        case EINT0_IRQn:
        case EINT1_IRQn:
        case EINT2_IRQn:
        case EINT3_IRQn:
            /* external interrupts 0-3 */
            pIRQCFG = &pADI_IDU->EI0CFG;
            ShiftCount = (eIrq - EINT0_IRQn) << ADI_GPIO_EIxCFG_SHIFT_COUNT;
            break;

        case EINT4_IRQn:
        case EINT5_IRQn:
        case EINT6_IRQn:
        case EINT7_IRQn:
            /* external interrupts 4-7 */
            pIRQCFG = &pADI_IDU->EI1CFG;
            ShiftCount = (eIrq - EINT4_IRQn) << ADI_GPIO_EIxCFG_SHIFT_COUNT;
            break;

        case EINT8_IRQn:
            /* external interrupt 8 */
            pIRQCFG = &pADI_IDU->EI2CFG;
            ShiftCount = (eIrq - EINT8_IRQn) << ADI_GPIO_EIxCFG_SHIFT_COUNT;
            break;
    }

    /* create the mask we'll use to clear the interrupt enable bit in the config register */
    Mask = EI0CFG_IRQ0EN << ShiftCount;

    /*********** need to protect this sequence *****/
    ADI_ENTER_CRITICAL_REGION();

    /* read/modify/write the appropriate bits in the register */
    CfgReg = *pIRQCFG;
    CfgReg &= ~Mask;
    *pIRQCFG = CfgReg;

    /*********** turn off protection *****/
    ADI_EXIT_CRITICAL_REGION();

    /* return */
    return (ADI_GPIO_SUCCESS);
}


/*****************************************************************************/
/*!
    @brief      Clear an External Interrupt

    @details    Clears the interrupt flag. It is not required for application
                to call this. The driver will clear the interrupt after
                calling the callback.

    @param[in]  eIrq    Identifier of type IRQn_Type that specifies the external
                        interrupt that is to be cleared

    @return     Status
                    - #ADI_GPIO_SUCCESS if successfully disabled
                    - #ADI_GPIO_ERR_NOT_INITIALIZED [D] if not yet initialized
                    - #ADI_GPIO_ERR_INVALID_INTERRUPT [D] if the eIrq parameter is invalid

    @sa         adi_GPIO_EnableIRQ
    @sa         adi_GPIO_DisableIRQ
    @sa         adi_GPIO_RegisterCallback

*/
/*****************************************************************************/
ADI_GPIO_RESULT_TYPE adi_GPIO_ClearIRQ (const IRQn_Type eIrq)
{

#ifdef ADI_DEBUG
    /* make sure we're initialized */
    if (adi_GPIO_Device.ReferenceCount == 0) {
        return (ADI_GPIO_ERR_NOT_INITIALIZED);
    }

    /* validate the interrupt number */
    if (!IsInterruptValid(eIrq)) {
        return (ADI_GPIO_ERR_INVALID_INTERRUPT);
    }
#endif

    /* Set 1 to clear the appropriate IRQ */
    pADI_IDU->EICLR = (1 << (eIrq - EINT0_IRQn));

    return (ADI_GPIO_SUCCESS);
}


/* ADuCM350 adds pin interrupts with Group A/B mask registers for each port... all of which land in either of two (Group A/B) interrupts) */

/*****************************************************************************/
/*!
    @brief      Write a port's pin interrupt Group A/B mask register.

    @details    Writes a GPIO port GPIO Group A/B mask register with /a Pins.  No bitwise operations are performed.
                Applications may register/unregister a callback using the #adi_GPIO_RegisterCallback API
                to get a notification when the group interrupt occurs.


    @param[in]  Port   Identifies the GPIO port to be operated on
    @param[in]  eIrq   Identifies the interrupt (Group A/B) with which the pin(s) are to be grouped.
    @param[in]  Pins   Identifies the GPIO port pins to be set in the port group mask register:
                           Pin bits that are set enable the interrupt for the group A/B.
                           Pin bits that are clear disable the interrupt for the group A/B.

    @return     Status
                    - #ADI_GPIO_SUCCESS if successfully disabled
                    - #ADI_GPIO_ERR_NOT_INITIALIZED [D] if not yet initialized
                    - #ADI_GPIO_ERR_INVALID_INTERRUPT [D] invalid pin interrupt group

    @sa         adi_GPIO_RegisterCallback
    @sa         adi_GPIO_GetGroupInterruptPins
    @sa         adi_GPIO_SetGroupInterruptPolarity
    @sa         adi_GPIO_GetGroupInterruptPolarity
    @sa         adi_GPIO_GetGroupInterruptStatus
    @sa         adi_GPIO_ClrGroupInterruptStatus
*/
/*****************************************************************************/
ADI_GPIO_RESULT_TYPE  adi_GPIO_SetGroupInterruptPins   (const ADI_GPIO_PORT_TYPE Port, const IRQn_Type eIrq, const ADI_GPIO_DATA_TYPE Pins)
{
    ADI_GPIO_TypeDef    *pPort;     /* pointer to port registers */

#ifdef ADI_DEBUG
    /* make sure we're initialized */
    if (adi_GPIO_Device.ReferenceCount == 0) {
        return (ADI_GPIO_ERR_NOT_INITIALIZED);
    }

    /* validate the port */
    if (!IsPortValid(Port)) {
        return (ADI_GPIO_ERR_INVALID_PORT);
    }

    /* validate the interrupt number */
    if (!IsInterruptValid(eIrq)) {
        return (ADI_GPIO_ERR_INVALID_INTERRUPT);
    }

    /* validate the pins */
    if (!ArePinsValid(Pins)) {
        return (ADI_GPIO_ERR_INVALID_PINS);
    }
#endif

    /* point to the port */
    pPort = (ADI_GPIO_TypeDef *)Port;

    ADI_ENTER_CRITICAL_REGION();

    switch (eIrq) {
        case GPIOA_IRQn:
            pPort->GPIENA = Pins;
            break;
        case GPIOB_IRQn:
            pPort->GPIENB = Pins;
            break;
#ifdef ADI_DEBUG
        default:
            return ADI_GPIO_ERR_INVALID_INTERRUPT;
            break;
#endif
    }

    ADI_EXIT_CRITICAL_REGION();

    /* return the status */
    return ADI_GPIO_SUCCESS;
}

/* ADuCM350 adds pin interrupts with Group A/B mask registers for each port... all of which land in either of two (Group A/B) interrupts) */

/*****************************************************************************/
/*!
    @brief      Get a port's pin interrupt Group A/B mask register

    @details    Returns a GPIO port GPIO Group A/B mask register.

    @param[in]  Port   Identifies the GPIO port to be operated on.
    @param[in]  eIrq   Identifies the Group A/B interrupt to query.

    @return     Pin state for port/group.
                    Pin bits that are set are enabled for group A/B interrupting.
                    Pin bits that are clear are disabled for group A/B interrupting.

    @sa         adi_GPIO_SetGroupInterruptPins
    @sa         adi_GPIO_SetGroupInterruptPolarity
    @sa         adi_GPIO_GetGroupInterruptPolarity
    @sa         adi_GPIO_GetGroupInterruptStatus
    @sa         adi_GPIO_ClrGroupInterruptStatus
*/
/*****************************************************************************/
ADI_GPIO_DATA_TYPE    adi_GPIO_GetGroupInterruptPins   (const ADI_GPIO_PORT_TYPE Port, const IRQn_Type eIrq)
{
    ADI_GPIO_TypeDef    *pPort;     /* pointer to port registers */
    ADI_GPIO_DATA_TYPE   pins = 0;

#ifdef ADI_DEBUG
    /* make sure we're initialized */
    if (adi_GPIO_Device.ReferenceCount == 0) {
        return (ADI_GPIO_ERR_NOT_INITIALIZED);
    }

    /* validate the port */
    if (!IsPortValid(Port)) {
        return (ADI_GPIO_ERR_INVALID_PORT);
    }

    /* validate the interrupt number */
    if (!IsInterruptValid(eIrq)) {
        return (ADI_GPIO_ERR_INVALID_INTERRUPT);
    }
#endif

    /* point to the port */
    pPort = (ADI_GPIO_TypeDef *)Port;

    switch (eIrq) {
        case GPIOA_IRQn:
            pins = pPort->GPIENA;
            break;
        case GPIOB_IRQn:
            pins = pPort->GPIENB;
            break;
    }
    return pins;
}

/* ADuCM350 adds pin interrupts with Group A/B mask registers for each port... all of which land in either of two (Group A/B) interrupts) */

/*****************************************************************************/
/*!
    @brief      Set a port's pin interrupt polarity register

    @details    Writes a GPIO pin interrupt polarity register.
                Set bits configure interrupt capture on low-to-high transitions, clear bits configure interrupt capture on high-to-low transitions.
                No bitwise operations are performed.

    @param[in]  Port   Identifies the GPIO port to be operated on
    @param[in]  Pins   Identifies the GPIO port pins to be set in the interrupt polarity control register

    @return     Status
                    - #ADI_GPIO_SUCCESS if successfully disabled
                    - #ADI_GPIO_ERR_NOT_INITIALIZED [D] if not yet initialized

    @sa         adi_GPIO_RegisterCallback
    @sa         adi_GPIO_SetGroupInterruptPins
    @sa         adi_GPIO_GetGroupInterruptPins
    @sa         adi_GPIO_GetGroupInterruptPolarity
    @sa         adi_GPIO_GetGroupInterruptStatus
    @sa         adi_GPIO_ClrGroupInterruptStatus
*/
/*****************************************************************************/
ADI_GPIO_RESULT_TYPE  adi_GPIO_SetGroupInterruptPolarity  (const ADI_GPIO_PORT_TYPE Port, const ADI_GPIO_DATA_TYPE Pins)
{
    ADI_GPIO_TypeDef    *pPort;     /* pointer to port registers */

#ifdef ADI_DEBUG
    /* make sure we're initialized */
    if (adi_GPIO_Device.ReferenceCount == 0) {
        return (ADI_GPIO_ERR_NOT_INITIALIZED);
    }

    /* validate the port */
    if (!IsPortValid(Port)) {
        return (ADI_GPIO_ERR_INVALID_PORT);
    }

    /* validate the pins */
    if (!ArePinsValid(Pins)) {
        return (ADI_GPIO_ERR_INVALID_PINS);
    }
#endif

    /* point to the port */
    pPort = (ADI_GPIO_TypeDef *)Port;

    /* write the register */
    pPort->GPPOL = Pins;

    /* return the status */
    return ADI_GPIO_SUCCESS;
}

/* ADuCM350 adds pin interrupts with Group A/B mask registers for each port... all of which land in either of two (Group A/B) interrupts) */

/*****************************************************************************/
/*!
    @brief      Get a port's pin interrupt polarity register

    @details    Read a GPIO pin interrupt polarity register.
                Set bits configure interrupt capture on low-to-high transitions, clear bits configure interrupt capture on high-to-low transitions.
                No bitwise operations are performed.

    @param[in]  Port   Identifies the GPIO port to be operated on

    @return     Interrupt polarity register for designated port.

    @sa         adi_GPIO_RegisterCallback
    @sa         adi_GPIO_SetGroupInterruptPins
    @sa         adi_GPIO_GetGroupInterruptPins
    @sa         adi_GPIO_SetGroupInterruptPolarity
    @sa         adi_GPIO_GetGroupInterruptStatus
    @sa         adi_GPIO_ClrGroupInterruptStatus
*/
/*****************************************************************************/
ADI_GPIO_DATA_TYPE    adi_GPIO_GetGroupInterruptPolarity  (const ADI_GPIO_PORT_TYPE Port)
{
    ADI_GPIO_TypeDef    *pPort;     /* pointer to port registers */

#ifdef ADI_DEBUG
    /* make sure we're initialized */
    if (adi_GPIO_Device.ReferenceCount == 0) {
        return (ADI_GPIO_ERR_NOT_INITIALIZED);
    }

    /* validate the port */
    if (!IsPortValid(Port)) {
        return (ADI_GPIO_ERR_INVALID_PORT);
    }
#endif

    /* point to the port */
    pPort = (ADI_GPIO_TypeDef *)Port;

    return pPort->GPPOL;
}


/* ADuCM350 adds pin interrupts with Group A/B mask registers for each port... all of which land in either of two (Group A/B) interrupts) */

/*****************************************************************************/
/*!
    @brief      Get a port's pin interrupt status register

    @details    Returns a GPIO port's interrupt status register.

    @param[in]  Port   Identifies the GPIO port to be operated on

    @return     Pin interrupt status for port.

    @sa         adi_GPIO_RegisterCallback
    @sa         adi_GPIO_SetGroupInterruptPins
    @sa         adi_GPIO_GetGroupInterruptPins
    @sa         adi_GPIO_SetGroupInterruptPolarity
    @sa         adi_GPIO_GetGroupInterruptPolarity
    @sa         adi_GPIO_ClrGroupInterruptStatus
*/
/*****************************************************************************/
ADI_GPIO_DATA_TYPE    adi_GPIO_GetGroupInterruptStatus  (const ADI_GPIO_PORT_TYPE Port)
{
    ADI_GPIO_TypeDef    *pPort;     /* pointer to port registers */

#ifdef ADI_DEBUG
    /* make sure we're initialized */
    if (adi_GPIO_Device.ReferenceCount == 0) {
        return (ADI_GPIO_ERR_NOT_INITIALIZED);
    }

    /* validate the port */
    if (!IsPortValid(Port)) {
        return (ADI_GPIO_ERR_INVALID_PORT);
    }
#endif

    /* point to the port */
    pPort = (ADI_GPIO_TypeDef *)Port;

    return (pPort->GPINT);
}

/* ADuCM350 adds pin interrupts with Group A/B mask registers for each port... all of which land in either of two (Group A/B) interrupts) */

/*****************************************************************************/
/*!
    @brief      Clear a port's pin interrupt status register

    @details    Clear the /a Pins in a GPIO port's interrupt status register.

    @param[in]  Port   Identifies the GPIO port to be operated on
    @param[in]  Pins   Identifies the GPIO port pins to be cleared in the interrupt status register

    @return     Status
                    - #ADI_GPIO_SUCCESS if successfully disabled
                    - #ADI_GPIO_ERR_NOT_INITIALIZED [D] if not yet initialized

    @sa         adi_GPIO_RegisterCallback
    @sa         adi_GPIO_SetGroupInterruptPins
    @sa         adi_GPIO_GetGroupInterruptPins
    @sa         adi_GPIO_SetGroupInterruptPolarity
    @sa         adi_GPIO_GetGroupInterruptPolarity
    @sa         adi_GPIO_GetGroupInterruptStatus
*/
/*****************************************************************************/
ADI_GPIO_RESULT_TYPE  adi_GPIO_ClrGroupInterruptStatus  (const ADI_GPIO_PORT_TYPE Port, const ADI_GPIO_DATA_TYPE Pins)
{
    ADI_GPIO_TypeDef    *pPort;     /* pointer to port registers */

#ifdef ADI_DEBUG
    /* make sure we're initialized */
    if (adi_GPIO_Device.ReferenceCount == 0) {
        return (ADI_GPIO_ERR_NOT_INITIALIZED);
    }

    /* validate the port */
    if (!IsPortValid(Port)) {
        return (ADI_GPIO_ERR_INVALID_PORT);
    }

    /* validate the pins */
    if (!ArePinsValid(Pins)) {
        return (ADI_GPIO_ERR_INVALID_PINS);
    }
#endif

    /* point to the port */
    pPort = (ADI_GPIO_TypeDef *)Port;

    pPort->GPINT = Pins;

    /* return the status */
    return ADI_GPIO_SUCCESS;
}


/*****************************************************************************/
/*!

    @brief      Enables/Disables the Output Drivers for GPIO Pin(s)

    @details    Enables/disables the output drivers for the given GPIO pin(s) on
                the given port.

    @param[in]  Port    Identifies the GPIO port to be configured (ADI_GPIO_PORT_x)

    @param[in]  Pins    Identifies one or more GPIO pins to be configured. GPIO
                        pins can be passed one at a time or in combination. To
                        configure a single GPIO pin, a single GPIO value is
                        passed for this parameter. For example, ADI_GPIO_PIN_4.
                        Alternatively, multiple GPIO pins can be configured
                        simultaneously by OR-ing together GPIO pin values and
                        passing the resulting value for this parameter. For
                        example, ADI_GPIO_PIN_0 | ADI_GPIO_PIN_5 | ADI_GPIO_PIN_6.

    @param[in]  bFlag   Boolean value describing the action to be taken
                            - true      enables the output driver
                            - false     disables the output driver

    @return     Status
                    - #ADI_GPIO_SUCCESS if successfully configured
                    - #ADI_GPIO_ERR_NOT_INITIALIZED [D] if not yet initialized
                    - #ADI_GPIO_ERR_INVALID_PORT [D] if the Port parameter is invalid
                    - #ADI_GPIO_ERR_INVALID_PINS [D] if the Pins parameter is invalid

    @sa         adi_GPIO_GetOutputEnable

*/
/*****************************************************************************/
ADI_GPIO_RESULT_TYPE adi_GPIO_SetOutputEnable (const uint32_t Port, const ADI_GPIO_DATA_TYPE Pins, const bool_t bFlag)
{

    ADI_GPIO_TypeDef    *pPort;     /* pointer to port registers */

#ifdef ADI_DEBUG
    /* make sure we're initialized */
    if (adi_GPIO_Device.ReferenceCount == 0) {
        return (ADI_GPIO_ERR_NOT_INITIALIZED);
    }

    /* validate the port */
    if (!IsPortValid(Port)) {
        return (ADI_GPIO_ERR_INVALID_PORT);
    }

    /* validate the pins */
    if (!ArePinsValid(Pins)) {
        return (ADI_GPIO_ERR_INVALID_PINS);
    }
#endif

    /* point to the port */
    pPort = (ADI_GPIO_TypeDef *)Port;

    /*********** need to protect this sequence *****/
    ADI_ENTER_CRITICAL_REGION();

    /* enable or disable as directed */
    if (bFlag) {
        /* enable output */
        pPort->GPOEN |= Pins;
    } else {
        /* disable output */
        pPort->GPOEN &= ~Pins;
    }

    /*********** turn off protection *****/
    ADI_EXIT_CRITICAL_REGION();

    /* return */
    return (ADI_GPIO_SUCCESS);
}


/*****************************************************************************/
/*!

    @brief      Enables/Disables the Input Drivers for GPIO Pin(s)

    @details    Enables/disables the input drivers for the given GPIO pin(s) on
                the given port.

    @param[in]  Port    Identifies the GPIO port to be configured (ADI_GPIO_PORT_x)

    @param[in]  Pins    Identifies one or more GPIO pins to be configured. GPIO
                        pins can be passed one at a time or in combination. To
                        configure a single GPIO pin, a single GPIO value is
                        passed for this parameter. For example, ADI_GPIO_PIN_4.
                        Alternatively, multiple GPIO pins can be configured
                        simultaneously by OR-ing together GPIO pin values and
                        passing the resulting value for this parameter. For
                        example, ADI_GPIO_PIN_0 | ADI_GPIO_PIN_5 | ADI_GPIO_PIN_6.

    @param[in]  bFlag   Boolean value describing the action to be taken
                            - true      enables the input driver
                            - false     disables the input driver

    @return     Status
                    - #ADI_GPIO_SUCCESS if successfully configured
                    - #ADI_GPIO_ERR_NOT_INITIALIZED [D] if not yet initialized
                    - #ADI_GPIO_ERR_INVALID_PORT [D] if the Port parameter is invalid
                    - #ADI_GPIO_ERR_INVALID_PINS [D] if the Pins parameter is invalid

    @sa         adi_GPIO_GetInputEnable

*/
/*****************************************************************************/
ADI_GPIO_RESULT_TYPE adi_GPIO_SetInputEnable (const uint32_t Port, const ADI_GPIO_DATA_TYPE Pins, const bool_t bFlag)
{

    ADI_GPIO_TypeDef    *pPort;     /* pointer to port registers */

#ifdef ADI_DEBUG
    /* make sure we're initialized */
    if (adi_GPIO_Device.ReferenceCount == 0) {
        return (ADI_GPIO_ERR_NOT_INITIALIZED);
    }

    /* validate the port */
    if (!IsPortValid(Port)) {
        return (ADI_GPIO_ERR_INVALID_PORT);
    }

    /* validate the pins */
    if (!ArePinsValid(Pins)) {
        return (ADI_GPIO_ERR_INVALID_PINS);
    }
#endif

    /* point to the port */
    pPort = (ADI_GPIO_TypeDef *)Port;

    /*********** need to protect this sequence *****/
    ADI_ENTER_CRITICAL_REGION();

    /* enable or disable as directed */
    if (bFlag) {
        /* enable input */
        pPort->GPIEN |= Pins;
    } else {
        /* disable input */
        pPort->GPIEN &= ~Pins;
    }

    /*********** turn off protection *****/
    ADI_EXIT_CRITICAL_REGION();

    /* return */
    return (ADI_GPIO_SUCCESS);
}


/*****************************************************************************/
/*!

    @brief      Enables/Disables the Pull-Up for GPIO Pin(s)

    @details    Enables/disables the internal pull-up for the given GPIO pin(s) on
                the given port.  For ADuCM350, the API simply enables/disables
                whatever the hard-wired pulls (up/down) are.

    @param[in]  Port    Identifies the GPIO port to be configured (ADI_GPIO_PORT_x)

    @param[in]  Pins    Identifies one or more GPIO pins to be configured. GPIO
                        pins can be passed one at a time or in combination. To
                        configure a single GPIO pin, a single GPIO value is
                        passed for this parameter. For example, ADI_GPIO_PIN_4.
                        Alternatively, multiple GPIO pins can be configured
                        simultaneously by OR-ing together GPIO pin values and
                        passing the resulting value for this parameter. For
                        example, ADI_GPIO_PIN_0 | ADI_GPIO_PIN_5 | ADI_GPIO_PIN_6.

    @param[in]  bFlag   Boolean value describing the action to be taken
                            - true      enables the pull-up
                            - false     disables the pull-up

    @return     Status
                    - #ADI_GPIO_SUCCESS if successfully configured
                    - #ADI_GPIO_ERR_NOT_INITIALIZED [D] if not yet initialized
                    - #ADI_GPIO_ERR_INVALID_PORT [D] if the Port parameter is invalid
                    - #ADI_GPIO_ERR_INVALID_PINS [D] if the Pins parameter is invalid

    @sa         adi_GPIO_GetPullupEnable

*/
/*****************************************************************************/
ADI_GPIO_RESULT_TYPE adi_GPIO_SetPullUpEnable (const uint32_t Port, const ADI_GPIO_DATA_TYPE Pins, const bool_t bFlag)
{

    ADI_GPIO_TypeDef    *pPort;     /* pointer to port registers */

#ifdef ADI_DEBUG
    /* make sure we're initialized */
    if (adi_GPIO_Device.ReferenceCount == 0) {
        return (ADI_GPIO_ERR_NOT_INITIALIZED);
    }

    /* validate the port */
    if (!IsPortValid(Port)) {
        return (ADI_GPIO_ERR_INVALID_PORT);
    }

    /* validate the pins */
    if (!ArePinsValid(Pins)) {
        return (ADI_GPIO_ERR_INVALID_PINS);
    }
#endif

    /* point to the port */
    pPort = (ADI_GPIO_TypeDef *)Port;

    /*********** need to protect this sequence *****/
    ADI_ENTER_CRITICAL_REGION();

    /* enable or disable the pullups as directed */
    /* note 350 has slightly different pull capability */
    if (bFlag) {
        pPort->GPPE |= Pins;
    } else {
        pPort->GPPE &= (~Pins);
    }

    /*********** turn off protection *****/
    ADI_EXIT_CRITICAL_REGION();

    /* return */
    return (ADI_GPIO_SUCCESS);
}



/*****************************************************************************/
/*!

    @brief      Gets the Output Driver Status for GPIO Pin(s)

    @details    Gets the output driver status for the GPIO pins on the given
                port.

    @param[in]  Port    Identifies the GPIO port to be sensed (ADI_GPIO_PORT_x)

    @return     The return value is a packed value containing the status of all
                the GPIO pins on the given port. To get the status of a specific
                GPIO pin, simply perform a logical AND of the return value with
                the GPIO pin definition. For example to see if pin 4 on port 2
                has the output driver enabled, the following logic is used:

                    if ((adi_GPIO_GetOutputEnable(ADI_GPIO_PORT_2)) & ADI_GPIO_PIN_4) {
                        the output driver is enabled for pin 4 on port 2
                    } else {
                        the output driver is disabled for pin 4 on port 2
                    }

    @sa         adi_GPIO_SetOutputEnable

*/
/*****************************************************************************/
ADI_GPIO_DATA_TYPE adi_GPIO_GetOutputEnable (const uint32_t Port)
{

    ADI_GPIO_TypeDef    *pPort;     /* pointer to port registers */

#ifdef ADI_DEBUG
    /* make sure we're initialized */
    if (adi_GPIO_Device.ReferenceCount == 0) {
        return (ADI_GPIO_ERR_NOT_INITIALIZED);
    }

    /* validate the port */
    if (!IsPortValid(Port)) {
        return (ADI_GPIO_ERR_INVALID_PORT);
    }
#endif

    /* point to the port */
    pPort = (ADI_GPIO_TypeDef *)Port;

    /* return the status */
    return (pPort->GPOEN);
}


/*****************************************************************************/
/*!

    @brief      Gets the Input Driver Status for GPIO Pin(s)

    @details    Gets the input driver status for the GPIO pins on the given
                port.

    @param[in]  Port    Identifies the GPIO port to be sensed (ADI_GPIO_PORT_x)

    @return     The return value is a packed value containing the status of all
                the GPIO pins on the given port. To get the status of a specific
                GPIO pin, simply perform a logical AND of the return value with
                the GPIO pin definition. For example to see if pin 4 on port 2
                has the output driver enabled, the following logic is used:

                    if ((adi_GPIO_GetInputEnable(ADI_GPIO_PORT_2)) & ADI_GPIO_PIN_4) {
                        the input driver is enabled for pin 4 on port 2
                    } else {
                        the input driver is disabled for pin 4 on port 2
                    }

    @sa         adi_GPIO_SetInputEnable

*/
/*****************************************************************************/
ADI_GPIO_DATA_TYPE adi_GPIO_GetInputEnable (const uint32_t Port)
{

    ADI_GPIO_TypeDef    *pPort;     /* pointer to port registers */

#ifdef ADI_DEBUG
    /* make sure we're initialized */
    if (adi_GPIO_Device.ReferenceCount == 0) {
        return (ADI_GPIO_ERR_NOT_INITIALIZED);
    }

    /* validate the port */
    if (!IsPortValid(Port)) {
        return (ADI_GPIO_ERR_INVALID_PORT);
    }
#endif

    /* point to the port */
    pPort = (ADI_GPIO_TypeDef *)Port;

    /* return the status */
    return (pPort->GPIEN);
}


/*****************************************************************************/
/*!

    @brief      Gets the Pull-Up Status for GPIO Pin(s)

    @details    Gets the pull-up status for the GPIO pins on the given port.
                For ADuCM350, simply gets the hard-wired pull enable.

    @param[in]  Port    Identifies the GPIO port to be sensed (ADI_GPIO_PORT_x)

    @return     The return value is a packed value containing the status of all
                the GPIO pins on the given port. To get the status of a specific
                GPIO pin, simply perform a logical AND of the return value with
                the GPIO pin definition. For example to see if pin 4 on port 2
                has the pull-up enabled, the following logic is used:

                    if ((adi_GPIO_GetPullUpEnable(ADI_GPIO_PORT_2)) & ADU_GPIO_PIN_4) {
                        the pull-up is enabled for pin 4 on port 2
                    } else {
                        the pull-up is disabled for pin 4 on port 2
                    }

    @sa         adi_GPIO_SetPullUpEnable

*/
/*****************************************************************************/
ADI_GPIO_DATA_TYPE adi_GPIO_GetPullUpEnable (const uint32_t Port)
{

    ADI_GPIO_TypeDef    *pPort;     /* pointer to port registers */

#ifdef ADI_DEBUG
    /* make sure we're initialized */
    if (adi_GPIO_Device.ReferenceCount == 0) {
        return (ADI_GPIO_ERR_NOT_INITIALIZED);
    }

    /* validate the port */
    if (!IsPortValid(Port)) {
        return (ADI_GPIO_ERR_INVALID_PORT);
    }
#endif

    /* point to the port */
    pPort = (ADI_GPIO_TypeDef *)Port;

    /* return the status */
    return (pPort->GPPE);
}



/*****************************************************************************/
/*!

    @brief      Sets the Given GPIO pin(s) to a Logical High Level

    @details    Sets the given GPIO pin(s) on the given port to a logical high
                level.

    @param[in]  Port    Identifies the GPIO port to be configured (ADI_GPIO_PORT_x)

    @param[in]  Pins    Identifies one or more GPIO pins to be configured. GPIO
                        pins can be passed one at a time or in combination. To
                        configure a single GPIO pin, a single GPIO value is
                        passed for this parameter. For example, ADI_GPIO_PIN_4.
                        Alternatively, multiple GPIO pins can be configured
                        simultaneously by OR-ing together GPIO pin values and
                        passing the resulting value for this parameter. For
                        example, ADI_GPIO_PIN_0 | ADI_GPIO_PIN_5 | ADI_GPIO_PIN_6.

    @return     Status
                    - #ADI_GPIO_SUCCESS if successfully configured
                    - #ADI_GPIO_ERR_NOT_INITIALIZED [D] if not yet initialized
                    - #ADI_GPIO_ERR_INVALID_PORT [D] if the Port parameter is invalid
                    - #ADI_GPIO_ERR_INVALID_PINS [D] if the Pins parameter is invalid

    @sa         adi_GPIO_SetLow, adi_GPIO_Toggle, adi_GPIO_SetData, adi_GPIO_GetData

*/
/*****************************************************************************/
ADI_GPIO_RESULT_TYPE adi_GPIO_SetHigh (const ADI_GPIO_PORT_TYPE Port, const ADI_GPIO_DATA_TYPE Pins)
{

    ADI_GPIO_TypeDef    *pPort;     /* pointer to port registers */

#ifdef ADI_DEBUG
    /* make sure we're initialized */
    if (adi_GPIO_Device.ReferenceCount == 0) {
        return (ADI_GPIO_ERR_NOT_INITIALIZED);
    }

    /* validate the port */
    if (!IsPortValid(Port)) {
        return (ADI_GPIO_ERR_INVALID_PORT);
    }

    /* validate the pins */
    if (!ArePinsValid(Pins)) {
        return (ADI_GPIO_ERR_INVALID_PINS);
    }
#endif

    /* point to the port */
    pPort = (ADI_GPIO_TypeDef *)Port;

    /* set the given GPIOs high */
    pPort->GPSET = Pins;

    /* return */
    return (ADI_GPIO_SUCCESS);
}


/*****************************************************************************/
/*!

    @brief      Sets the Given GPIO pin(s) to a Logical Low Level

    @details    Sets the given GPIO pin(s) on the given port to a logical low
                level.

    @param[in]  Port    Identifies the GPIO port to be configured (ADI_GPIO_PORT_x)

    @param[in]  Pins    Identifies one or more GPIO pins to be configured. GPIO
                        pins can be passed one at a time or in combination. To
                        configure a single GPIO pin, a single GPIO value is
                        passed for this parameter. For example, ADI_GPIO_PIN_4.
                        Alternatively, multiple GPIO pins can be configured
                        simultaneously by OR-ing together GPIO pin values and
                        passing the resulting value for this parameter. For
                        example, ADI_GPIO_PIN_0 | ADI_GPIO_PIN_5 | ADI_GPIO_PIN_6.

    @return     Status
                    - #ADI_GPIO_SUCCESS if successfully configured
                    - #ADI_GPIO_ERR_NOT_INITIALIZED [D] if not yet initialized
                    - #ADI_GPIO_ERR_INVALID_PORT [D] if the Port parameter is invalid
                    - #ADI_GPIO_ERR_INVALID_PINS [D] if the Pins parameter is invalid

    @sa         adi_GPIO_SetHigh, adi_GPIO_Toggle, adi_GPIO_SetData, adi_GPIO_GetData

*/
/*****************************************************************************/
ADI_GPIO_RESULT_TYPE adi_GPIO_SetLow (const ADI_GPIO_PORT_TYPE Port, const ADI_GPIO_DATA_TYPE Pins)
{

    ADI_GPIO_TypeDef    *pPort;     /* pointer to port registers */

#ifdef ADI_DEBUG
    /* make sure we're initialized */
    if (adi_GPIO_Device.ReferenceCount == 0) {
        return (ADI_GPIO_ERR_NOT_INITIALIZED);
    }

    /* validate the port */
    if (!IsPortValid(Port)) {
        return (ADI_GPIO_ERR_INVALID_PORT);
    }

    /* validate the pins */
    if (!ArePinsValid(Pins)) {
        return (ADI_GPIO_ERR_INVALID_PINS);
    }
#endif

    /* point to the port */
    pPort = (ADI_GPIO_TypeDef *)Port;
    /* set the given GPIOs low */
    pPort->GPCLR = Pins;

    /* return */
    return (ADI_GPIO_SUCCESS);
}


/*****************************************************************************/
/*!

    @brief      Toggles the Logical Level of the Given GPIO pin(s)

    @details    Toggles the logical level of the given GPIO pin(s) on the given port.
                If a given GPIO pin is at a logical low level, this function will
                change the level to a logical high value. If a given GPIO pin is
                at a logical high level, this function will change the level to a
                logical low value.

    @param[in]  Port    Identifies the GPIO port to be configured (ADI_GPIO_PORT_x)

    @param[in]  Pins    Identifies one or more GPIO pins to be configured. GPIO
                        pins can be passed one at a time or in combination. To
                        configure a single GPIO pin, a single GPIO value is
                        passed for this parameter. For example, ADI_GPIO_PIN_4.
                        Alternatively, multiple GPIO pins can be configured
                        simultaneously by OR-ing together GPIO pin values and
                        passing the resulting value for this parameter. For
                        example, ADI_GPIO_PIN_0 | ADI_GPIO_PIN_5 | ADI_GPIO_PIN_6.

    @return     Status
                    - #ADI_GPIO_SUCCESS if successfully configured
                    - #ADI_GPIO_ERR_NOT_INITIALIZED [D] if not yet initialized
                    - #ADI_GPIO_ERR_INVALID_PORT [D] if the Port parameter is invalid
                    - #ADI_GPIO_ERR_INVALID_PINS [D] if the Pins parameter is invalid

    @sa         adi_GPIO_SetHigh, adi_GPIO_SetLow, adi_GPIO_SetData, adi_GPIO_GetData

*/
/*****************************************************************************/
ADI_GPIO_RESULT_TYPE adi_GPIO_Toggle (const ADI_GPIO_PORT_TYPE Port, const ADI_GPIO_DATA_TYPE Pins)
{

    ADI_GPIO_TypeDef    *pPort;     /* pointer to port registers */

#ifdef ADI_DEBUG
    /* make sure we're initialized */
    if (adi_GPIO_Device.ReferenceCount == 0) {
        return (ADI_GPIO_ERR_NOT_INITIALIZED);
    }

    /* validate the port */
    if (!IsPortValid(Port)) {
        return (ADI_GPIO_ERR_INVALID_PORT);
    }

    /* validate the pins */
    if (!ArePinsValid(Pins)) {
        return (ADI_GPIO_ERR_INVALID_PINS);
    }
#endif

    /* point to the port */
    pPort = (ADI_GPIO_TypeDef *)Port;

    /* toggle the given GPIOs */
    pPort->GPTGL = Pins;

    /* return */
    return (ADI_GPIO_SUCCESS);
}


/*****************************************************************************/
/*!

    @brief      Sets the Values of all GPIO Pins on the Given Port to
                a Specified Level

    @details    Sets the values of all GPIO pins on the given port to the
                specified value.

    @param[in]  Port    Identifies the GPIO port to be configured (ADI_GPIO_PORT_x)

    @param[in]  Pins    Identifies which GPIO pins are to be set to a logical
                        high level. All other GPIO pins on the port will be set
                        to a logical low level. For example, to set pins 0 and
                        1 to a logical high level and all other pins to a logical
                        low level, this parameter should be passed as
                        ADI_GPIO_PIN_0 | ADI_GPIO_PIN_1.

    @return     Status
                    - #ADI_GPIO_SUCCESS if successfully configured
                    - #ADI_GPIO_ERR_NOT_INITIALIZED [D] if not yet initialized
                    - #ADI_GPIO_ERR_INVALID_PORT [D] if the Port parameter is invalid
                    - #ADI_GPIO_ERR_INVALID_PINS [D] if the Pins parameter is invalid

    @sa         adi_GPIO_SetHigh, adi_GPIO_SetLow, adi_GPIO_Toggle, adi_GPIO_GetData

*/
/*****************************************************************************/
ADI_GPIO_RESULT_TYPE adi_GPIO_SetData (const ADI_GPIO_PORT_TYPE Port, const ADI_GPIO_DATA_TYPE Pins)
{

    ADI_GPIO_TypeDef    *pPort;     /* pointer to port registers */

#ifdef ADI_DEBUG
    /* make sure we're initialized */
    if (adi_GPIO_Device.ReferenceCount == 0) {
        return (ADI_GPIO_ERR_NOT_INITIALIZED);
    }

    /* validate the port */
    if (!IsPortValid(Port)) {
        return (ADI_GPIO_ERR_INVALID_PORT);
    }

    /* validate the pins */
    if (!ArePinsValid(Pins)) {
        return (ADI_GPIO_ERR_INVALID_PINS);
    }
#endif

    /* point to the port */
    pPort = (ADI_GPIO_TypeDef *)Port;

    /* set the GPIOs as directed */
    pPort->GPOUT = Pins;

    /* return */
    return (ADI_GPIO_SUCCESS);
}


/*****************************************************************************/
/*!

    @brief      Gets/Senses the Input Level of all GPIO Pins on the Given Port

    @details    Gets the level of all GPIO input pins on the given port

    @param[in]  Port    Identifies the GPIO port to be sensed (ADI_GPIO_PORT_x)

    @return     The return value is a packed value containing the status of all
                the GPIO pins on the given port. To get the status of a specific
                GPIO pin, simply perform a logical AND of the return value with
                the GPIO pin definition. For example to see if pin 4 on port 2
                is a logical high level, the following logic is used:

                    if ((adi_GPIO_GetData(ADI_GPIO_PORT_2)) & ADU_GPIO_PIN_4) {
                        pin 4 on port 2 is a logical high value
                    } else {
                        pin 4 on port 2 is a logical low value
                    }

    @sa         adi_GPIO_SetHigh, adi_GPIO_SetLow, adi_GPIO_Toggle, adi_GPIO_SetData

*/
/*****************************************************************************/
ADI_GPIO_DATA_TYPE adi_GPIO_GetData (const uint32_t Port)
{

    ADI_GPIO_TypeDef    *pPort;     /* pointer to port registers */

#ifdef ADI_DEBUG
    /* make sure we're initialized */
    if (adi_GPIO_Device.ReferenceCount == 0) {
        return (ADI_GPIO_ERR_NOT_INITIALIZED);
    }

    /* validate the port */
    if (!IsPortValid(Port)) {
        return (ADI_GPIO_ERR_INVALID_PORT);
    }
#endif

    /* point to the port */
    pPort = (ADI_GPIO_TypeDef *)Port;

    /* return the status of the GPIOs */
    return (pPort->GPIN);
}


/*****************************************************************************/
/*!

    @brief      Gets the Output Level of all GPIO Pins on the Given Port

    @details    Gets the level of all GPIO output pins on the given port

    @param[in]  Port    Identifies the GPIO port to be read (ADI_GPIO_PORT_x)

    @return     The return value is a packed value containing the status of all
                the GPIO pins on the given port. To get the status of a specific
                GPIO pin, simply perform a logical AND of the return value with
                the GPIO pin definition. For example to see if pin 4 on port 2
                is a logical high level, the following logic is used:

                    if ((adi_GPIO_GetData(ADI_GPIO_PORT_2)) & ADU_GPIO_PIN_4) {
                        pin 4 on port 2 is a logical high value
                    } else {
                        pin 4 on port 2 is a logical low value
                    }

    @sa         adi_GPIO_SetHigh, adi_GPIO_SetLow, adi_GPIO_Toggle, adi_GPIO_SetData
    @sa         adi_GPIO_GetData
*/
/*****************************************************************************/
ADI_GPIO_DATA_TYPE adi_GPIO_GetOutputData (const uint32_t Port)
{

    ADI_GPIO_TypeDef    *pPort;     /* pointer to port registers */

#ifdef ADI_DEBUG
    /* make sure we're initialized */
    if (adi_GPIO_Device.ReferenceCount == 0) {
        return (ADI_GPIO_ERR_NOT_INITIALIZED);
    }

    /* validate the port */
    if (!IsPortValid(Port)) {
        return (ADI_GPIO_ERR_INVALID_PORT);
    }
#endif

    /* point to the port */
    pPort = (ADI_GPIO_TypeDef *)Port;

    /* return the status of the oputput register */
    return (pPort->GPOUT);
}


/*!
    @brief      Register or unregister an application callback function for external pin interrupts.

    @details    Applications may register a callback function that will be called when an
                external interrupt occurs. In addition to registering the interrupt,
                the application should call the #adi_GPIO_EnableIRQ API to enable the
                external pin interrupt.

                The driver dispatches calls to registered callback functions when the
                properly configured pin(s) latches an external interrupt input on the GPIO
                pin(s).  The callback is dispatched with the following parameters,
                respectively:

                    - application-provided callback parameter (/a pCBParam),
                    - the interrupt ID (#IRQn_Type) that initiated the interrupt,
                    - NULL.

    @note       The application callback function /b must /b clear /b pin /b interrupts before returning from the callback.

    @param[in]  eIrq            The interrupt for which the callback is being registered.
    @param[in]  pfCallback      Pointer to the callback function. This can be passed as NULL to
                                unregister the callback.
    @param[in]  pCBParam        Callback prameter which will be passed back to the application
                                when the callback is called..

    @return     Status
                    - #ADI_GPIO_SUCCESS if successfully registered the callback.
                    - #ADI_GPIO_ERR_NOT_INITIALIZED [D] if not yet initialized
                    - #ADI_GPIO_ERR_INVALID_INTERRUPT [D] if interrupt ID is invalid

    @sa         adi_GPIO_EnableIRQ
    @sa         adi_GPIO_DisableIRQ
    @sa         adi_GPIO_SetGroupInterruptPolarity
    @sa         adi_GPIO_GetGroupInterruptPolarity
    @sa         adi_GPIO_GetGroupInterruptStatus
*/
ADI_GPIO_RESULT_TYPE  adi_GPIO_RegisterCallback (const IRQn_Type eIrq, ADI_CALLBACK const pfCallback, void *const pCBParam )
{
    uint8_t index;

#ifdef ADI_DEBUG
    /* make sure we're initialized */
    if (adi_GPIO_Device.ReferenceCount == 0) {
        return (ADI_GPIO_ERR_NOT_INITIALIZED);
    }

    /* validate the interrupt number */
    if (!IsInterruptValid(eIrq)) {
        return (ADI_GPIO_ERR_INVALID_INTERRUPT);
    }
#endif

    ADI_ENTER_CRITICAL_REGION();

    switch (eIrq) {
        case EINT0_IRQn:
        case EINT1_IRQn:
        case EINT2_IRQn:
        case EINT3_IRQn:
        case EINT4_IRQn:
        case EINT5_IRQn:
        case EINT6_IRQn:
        case EINT7_IRQn:
        case EINT8_IRQn:
            index = eIrq - EINT0_IRQn;
            break;
        case GPIOA_IRQn:
        case GPIOB_IRQn:
            index = eIrq - GPIOA_IRQn + ADI_GPIO_IRQ_GROUPA_INDEX;
            break;

#ifdef ADI_DEBUG
        default:
            return ADI_GPIO_ERR_INVALID_INTERRUPT;
#endif
    }

    adi_GPIO_Device.CallbackTable[index].pfCallback = pfCallback;
    adi_GPIO_Device.CallbackTable[index].pCBParam = pCBParam;

    ADI_EXIT_CRITICAL_REGION();

    /* return the status */
    return ADI_GPIO_SUCCESS;
}


/*! \cond PRIVATE */

/* all of the following is excluded from the doxygen output... */


/* Common external interrupt handler */
void CommonInterruptHandler(const ADI_GPIO_IRQ_INDEX_TYPE index, const IRQn_Type eIrq)
{
	GPIO_CALLBACK_INFO *pCallbackInfo = &adi_GPIO_Device.CallbackTable[index];
    /* GroupA/B interrupt clearing variables unique to ADiCM350 */
    ADI_GPIO_TypeDef *pPorts[] = { pADI_GP0, pADI_GP1, pADI_GP2, pADI_GP3, pADI_GP4 };
    ADI_GPIO_TypeDef *pPort;
    ADI_GPIO_DATA_TYPE interruptingPins, enabledPins;

    /* First dispatch the callback, if one is registered */
    if(pCallbackInfo->pfCallback) {
		/* params list is: application-provided cbParam, interrupt ID, and NULL */
		pCallbackInfo->pfCallback (pCallbackInfo->pCBParam, (uint32_t) eIrq, NULL);
	}

    /* now clear the interrupt... */
	switch (eIrq) {
		/* GroupA/B interrupts take special handeling to clear (only on ADuCM350) */
		case GPIOA_IRQn:
		case GPIOB_IRQn:

			/* collect interrupting pins over each port and clear them */
			for (int i = 0; i < sizeof(pPorts)/sizeof(pPorts[0]); i++) {
				pPort = pPorts[i];

				/* which interruptingPins are enabled (by group) */
				enabledPins = (GPIOA_IRQn == eIrq) ? pPort->GPIENA : pPort->GPIENB;

				/* clear enabled pins that actually interrupted, if any (for this port and group) */
				interruptingPins = ((pPort->GPINT) & enabledPins);
				if (interruptingPins)
					pPort->GPINT = interruptingPins;
			}
			break;
		/* otherwise, just a simple clear */
		default:
        	adi_GPIO_ClearIRQ(eIrq);
			break;
	}
}


/* strongly-bound interrupt handlers to override the default weak bindings */
ADI_INT_HANDLER(Ext_Int0_Handler)   { CommonInterruptHandler(ADI_GPIO_IRQ_EXT0_INDEX, EINT0_IRQn); }
ADI_INT_HANDLER(Ext_Int1_Handler)   { CommonInterruptHandler(ADI_GPIO_IRQ_EXT1_INDEX, EINT1_IRQn); }
ADI_INT_HANDLER(Ext_Int2_Handler)   { CommonInterruptHandler(ADI_GPIO_IRQ_EXT2_INDEX, EINT2_IRQn); }
ADI_INT_HANDLER(Ext_Int3_Handler)   { CommonInterruptHandler(ADI_GPIO_IRQ_EXT3_INDEX, EINT3_IRQn); }
ADI_INT_HANDLER(Ext_Int4_Handler)   { CommonInterruptHandler(ADI_GPIO_IRQ_EXT4_INDEX, EINT4_IRQn); }
ADI_INT_HANDLER(Ext_Int5_Handler)   { CommonInterruptHandler(ADI_GPIO_IRQ_EXT5_INDEX, EINT5_IRQn); }
ADI_INT_HANDLER(Ext_Int6_Handler)   { CommonInterruptHandler(ADI_GPIO_IRQ_EXT6_INDEX, EINT6_IRQn); }
ADI_INT_HANDLER(Ext_Int7_Handler)   { CommonInterruptHandler(ADI_GPIO_IRQ_EXT7_INDEX, EINT7_IRQn); }
ADI_INT_HANDLER(Ext_Int8_Handler)   { CommonInterruptHandler(ADI_GPIO_IRQ_EXT8_INDEX, EINT8_IRQn); }

ADI_INT_HANDLER(GPIO_A_Int_Handler) { CommonInterruptHandler(ADI_GPIO_IRQ_GROUPA_INDEX,GPIOA_IRQn); }
ADI_INT_HANDLER(GPIO_B_Int_Handler) { CommonInterruptHandler(ADI_GPIO_IRQ_GROUPB_INDEX,GPIOB_IRQn); }



#ifdef ADI_DEBUG
/*****************************************************************************/
/*!

    @brief      Tests a Port Parameter for Validity

    @details    A debug function that checks a Port parameter for validity

    @param[in]  Port    Identifies a GPIO port (ADI_GPIO_PORT_x)

    @return     Status
                    - true      the Port value contains valid data
                    - false     the Port value contains invalid data

*/
/*****************************************************************************/
static bool_t IsPortValid(const ADI_GPIO_PORT_TYPE Port)
{
    /* make sure the port is valid */
    switch (Port) {
		case ADI_GPIO_PORT_0:
		case ADI_GPIO_PORT_1:
		case ADI_GPIO_PORT_2:
		case ADI_GPIO_PORT_3:
		case ADI_GPIO_PORT_4:

        	return (true);
    }

    /* return */
    return (false);
}


/*****************************************************************************/
/*!

    @brief      Tests a Pins Parameter for Validity

    @details    A debug function that checks a Pins parameter for validity

    @param[in]  Pins    Logical OR-ing of one or more ADI_GPIO_PIN_x values

    @return     Status
                    - true      the Pins value contains valid data
                    - false     the Pins value contains invalid data

*/
/*****************************************************************************/
static bool_t ArePinsValid(const ADI_GPIO_DATA_TYPE Pins)
{
    /* test for a valid pin */
    if (Pins != 0) {
        return (true);
    }

    /* return */
    return (false);
}

/*****************************************************************************/
/*!

    @brief      Tests an eIrq Parameter for Validity

    @details    A debug function that checks an eIrq parameter for validity

    @param[in]  eIrq    Identifier of type IRQn_Type

    @return     Status
                    - true      the eIrq value contains valid data
                    - false     the eIrq value contains invalid data

*/
/*****************************************************************************/
static bool_t IsInterruptValid(const IRQn_Type eIrq)
{

    /* make sure it's a legal GPIO interrupt */
    switch (eIrq) {
        case EINT0_IRQn:
        case EINT1_IRQn:
        case EINT2_IRQn:
        case EINT3_IRQn:
        case EINT4_IRQn:
        case EINT5_IRQn:
        case EINT6_IRQn:
        case EINT7_IRQn:
        case EINT8_IRQn:
	case GPIOA_IRQn:
	case GPIOB_IRQn:

            return (true);
    }

    /* return */
    return (false);
}


/*****************************************************************************/
/*!

    @brief      Tests a Trigger Condition Parameter for Validity

    @details    A debug function that checks a trigger condition parameter for validity

    @param[in]  eCondition  Identifier of type ADI_GPIO_IRQ_TRIGGER_CONDITION_TYPE

    @return     Status
                    - true      the eCondition value contains valid data
                    - false     the eCondition value contains invalid data

*/
/*****************************************************************************/
static bool_t IsTriggerValid (const ADI_GPIO_IRQ_TRIGGER_CONDITION_TYPE eCondition)
{

    /* make sure it's a valid trigger condition */
    switch (eCondition) {
    case ADI_GPIO_IRQ_RISING_EDGE:
    case ADI_GPIO_IRQ_FALLING_EDGE:
    case ADI_GPIO_IRQ_EITHER_EDGE:
    case ADI_GPIO_IRQ_HIGH_LEVEL:
    case ADI_GPIO_IRQ_LOW_LEVEL:
        return (true);
    }

    /* return */
    return (false);
}
#endif  // ADI_DEBUG

#define REVERT_ADI_MISRA_SUPPRESSIONS  /*!< Revert ADI MISRA Suppressions */
#include "misra.h"

/*! \endcond */

/*
** EOF
*/

/*@}*/
