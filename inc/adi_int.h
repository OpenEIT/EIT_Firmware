/*********************************************************************************

Copyright (c) 2011-2014 Analog Devices, Inc. All Rights Reserved.

This software is proprietary to Analog Devices, Inc. and its licensors.  By using 
this software you agree to the terms of the associated Analog Devices Software 
License Agreement.

*********************************************************************************/
/*!
* @file      adi_int.h
*
* @brief     Interrupt Handler API header file
*
* @details
*            This is the primary header file for installing interrupt handlers
*/

#ifndef ADI_INT_H_
#define ADI_INT_H_


/**
 *  @defgroup INTERRUPTS RTOS-Related Interrupt APIs
 * This is the Analog Devices' RTOS interrupt inferface.  It is used to hook and unhook interrupts within an RTOS environment.
 * This interface is not used for native, non-threading/non-tasking embedded programming, as the IVT is pre-configured as part of the startup code.
 */

/** @defgroup Interrupt_Install Interrupt Installation
 *  @ingroup INTERRUPTS
 */

/*! @addtogroup Interrupt_Install
 *  @{
 */

#include <stdint.h>
#include <stdbool.h>

/*! Enumeration of different error codes returned by adi_int* APIs. */
typedef enum
{
    ADI_INT_SUCCESS, /*!< The operation is completed successfully. */
    ADI_INT_FAILURE  /*!< The operation failed. */

} ADI_INT_STATUS;


/**
 * @brief       Interrupt Handler
 *
 * @details     Interrupt handler to be registered by the application.
 *
 * @param [in]  iid         Interrupt ID for the interrupt.
 * @param [in]  handlerArg  Callback parameter given by the appliction while
 *                          registering the handler.
 *
 * @return      None
 */

typedef void (*ADI_INT_HANDLER_PTR)(
    uint32_t            iid,
    void*               handlerArg
);

ADI_INT_STATUS  adi_int_InstallHandler (
    uint32_t            iid,
    ADI_INT_HANDLER_PTR pfHandler,
    void*               pCBParam,
    bool                bEnable
               );

ADI_INT_STATUS adi_int_EnableInt(
    uint32_t            iid,
    bool                bEnable
               );
ADI_INT_STATUS  adi_int_UninstallHandler (uint32_t iid);


/**
 * @brief Device Drivers Callback function definition
 */
typedef void (* ADI_CALLBACK) (  /*!< Callback function pointer */
    void      *pCBParam,         /*!< Client supplied callback param */
    uint32_t   Event,            /*!< Event ID specific to the Driver/Service */
    void      *pArg);            /*!< Pointer to the event specific argument */


/*@}*/

#endif /* ADI_INT_H_ */
