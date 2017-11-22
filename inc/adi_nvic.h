/*********************************************************************************

Copyright (c) 2010-2014 Analog Devices, Inc. All Rights Reserved.

This software is proprietary to Analog Devices, Inc. and its licensors.  By using 
this software you agree to the terms of the associated Analog Devices Software 
License Agreement.

*********************************************************************************/

/*!
 *****************************************************************************
 * @file:    adi_nvic.h
 * @brief:   API header file for NVIC controller.
 * @version: $Revision: 28525 $
 * @date:    $Date: 2014-11-12 14:51:26 -0500 (Wed, 12 Nov 2014) $
*****************************************************************************/
#ifndef _ADI_NVIC_H_
#define _ADI_NVIC_H_

#include <stdint.h>
#include <stdbool.h>

/* Number of system exceptions */
#define ADI_NUM_SYSTEM_EXPT         15

/* Convert IRQ number to System Interrupt ID
  (irq - number of system exceptions - 1 location for stack pointer in the vector table*/
#define ADI_NVIC_IRQ_SID(irq)       (irq - ADI_NUM_SYSTEM_EXPT - 1)

/* Convert SID to IRQ number */
#define ADI_NVIC_SID_IRQ(sid)       (sid + ADI_NUM_SYSTEM_EXPT + 1)

/*! Return codes from NVIC */
typedef enum
{
    ADI_NVIC_SUCCESS,       /*!< Generic success. */
    ADI_NVIC_FAILURE,       /*!< Generic Failure. */
    ADI_NVIC_INVALID_INT    /*!< If the given interrupt ID is invalid. */
} ADI_NVIC_RESULT;

/* Interrupt handler type for registering with NVIC */
typedef void (*ADI_NVIC_HANDLER) (
        void
);

/* API to register interrupt handler with NVIC */
int32_t adi_nvic_RegisterHandler (
        uint32_t             iid,
        ADI_NVIC_HANDLER     pfHandler
);

/* API to unregister interrupt handler with NVIC */
int32_t adi_nvic_UnRegisterHandler (
        uint32_t              iid
);

/* API to enable interrupt */
ADI_NVIC_RESULT adi_nvic_EnableInt (
        uint32_t               iid,
        bool                   bEnable
);

#endif /* _ADI_NVIC_H_ */
