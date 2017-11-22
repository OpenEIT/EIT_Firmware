/*********************************************************************************

Copyright (c) 2011-2014 Analog Devices, Inc. All Rights Reserved.

This software is proprietary to Analog Devices, Inc. and its licensors.  By using 
this software you agree to the terms of the associated Analog Devices Software 
License Agreement.

*********************************************************************************/
/*!
* @file      adi_int_id.h
*
* @brief     Interrupt Handler ID header file
*
* @details
*            This is the header file which is required to have the IDs of the
*            interrupts that will be passed to adi_int_InstallHandler
*/

#ifndef ADI_INT_ID_H_
#define ADI_INT_ID_H_

/*! @addtogroup Interrupt_Install Interrupt Installation
 *  @{
 */

/* The definition of the system interrupt IDs for processors with a SEC are
 * in the def file for the processor. platform.h selects the appropriate def
 * file for each processor
 */

#include <adi_sid_cm350.h>


/*@}*/

#endif /* ADI_INT_ID_H_ */
