/*********************************************************************************

Copyright (c) 2012-2014 Analog Devices, Inc. All Rights Reserved.

This software is proprietary to Analog Devices, Inc. and its licensors.  By using 
this software you agree to the terms of the associated Analog Devices Software 
License Agreement (See /LICENSES/ADI.LABLAB.LICENSE). 

Modified from the Analog Devices original.
Portions Copyright (c) 2018 Andrew Grosser, Moritz von Buttlar.

*********************************************************************************/

/*!
 *****************************************************************************
   @file:    adi_rtos.h
   @brief:   Real Time Operating System support macros.
   @version: $Revision: 28525 $
   @date:    $Date: 2014-11-12 14:51:26 -0500 (Wed, 12 Nov 2014) $
*****************************************************************************/

#ifndef __ADI_RTOS_H__
#define __ADI_RTOS_H__
#include "adi_int.h"

#if (ADI_CFG_ENABLE_RTOS_SUPPORT == 1)
#include "adi_osal.h"
#endif /* ADI_CFG_ENABLE_RTOS_SUPPORT */

#if (ADI_CFG_ENABLE_RTOS_SUPPORT == 1)

/* Enter critical section */
#define ADI_ENTER_CRITICAL_REGION()  \
        adi_osal_EnterCriticalRegion()

/* Exit critical region */
#define ADI_EXIT_CRITICAL_REGION()  \
        adi_osal_ExitCriticalRegion()

/* Used to define the interrupt handler */
#define ADI_INT_HANDLER(Handler) \
        void Handler (uint32_t  __iid, void*  __handlerArg)

/* Macro to install the handler */
#define ADI_INSTALL_HANDLER(IRQn, Handler) \
        adi_int_InstallHandler(IRQn, Handler, NULL, false)

/* Macro to uninstall the handler */
#define ADI_UNINSTALL_HANDLER(IRQn) \
        adi_int_UninstallHandler(IRQn)

/* Macro to enable the interrupt */
#define ADI_ENABLE_INT(IRQn) \
        adi_int_EnableInt(IRQn, true)

/* Macro to disable the interrupt */
#define ADI_DISABLE_INT(IRQn) \
        adi_int_EnableInt(IRQn, false)

#else /* No multithread support */

/* Enter critical region */
#define ADI_ENTER_CRITICAL_REGION()  \
        __disable_irq()

/* Exit critical region */
#define ADI_EXIT_CRITICAL_REGION()  \
        __enable_irq()

/* Used to define the interrupt handler */
#define ADI_INT_HANDLER(Handler) \
        void Handler (void)

/* Macro to install the handler (defined to nothing as no installation required, the driver just override the weak interrupt handler symbol) */
#define ADI_INSTALL_HANDLER(IRQn, Handler)

/* Macro to uninstall the handler */
#define ADI_UNINSTALL_HANDLER(IRQn)

/* Macro to enable the interrupt */
#define ADI_ENABLE_INT(IRQn) \
        NVIC_EnableIRQ(IRQn)


/* Macro to disable the interrupt */
#define ADI_DISABLE_INT(IRQn) \
        NVIC_DisableIRQ(IRQn)

#endif  /* ADI_CFG_ENABLE_RTOS_SUPPORT */




#endif /* __ADI_RTOS_H__ */
