/*********************************************************************************

Copyright (c) 2011-2014 Analog Devices, Inc. All Rights Reserved.

This software is proprietary to Analog Devices, Inc. and its licensors.  By using
this software you agree to the terms of the associated Analog Devices Software
License Agreement.

*********************************************************************************/

/*!
 *****************************************************************************
 * @file:   test_common.c
 * @brief:  Common utilities for testing
 * @version: $Revision: 29071 $
 * @date:    $Date: 2014-12-08 12:46:24 -0500 (Mon, 08 Dec 2014) $
 *****************************************************************************/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include "test_common.h"

static void quit(void);

/**
 * Initialize the test system, including SystemInit and Wdog
 *
 * @param  none
 * @return none
 *
 * @brief  Set up the test system
 *         Send output to wherever output should go
 */
void test_Init()
{
    ADI_WDT_DEV_HANDLE hWatchdog;

    /* Disable WDT for testing*/
    if( adi_WDT_Init(ADI_WDT_DEVID_0, &hWatchdog) != 0)
    {
        test_Fail("adi_WDT_Init failed, you're probably calling it twice");
    }

    if( adi_WDT_SetEnable(hWatchdog, false) != 0 )
    {
        test_Fail("adi_WDT_SetEnable failed");
    }

    if( adi_WDT_UnInit(hWatchdog) != 0 )
    {
        test_Fail("adi_WDT_UnInit failed");
    }
}



/**
 * Passing result
 *
 * @param  none
 * @return none
 *
 * @brief  Report a passing test result
 */
void test_Pass()
{
    char pass[] = "PASS!\n\r";

    printf(pass);

    /* Once the result is reported, do an abrupt termination */
    quit();
}


/**
 * Failing result
 *
 * @param  none
 * @return none
 *
 * @brief  Report a failing test result
 */
void test_Fail(char *FailureReason)
{
    char fail[] = "FAIL: ";
    char term[] = "\n\r";

    printf(fail);
    printf(FailureReason);
    printf(term);

    /* Once the result is reported, do an abrupt termination */
    quit();
}


/**
 * Info
 *
 * @param  none
 * @return none
 *
 * @brief  Report test info
 */
void test_Perf(char *InfoString)
{
    char info[] = "PERF: ";
    char term[] = "\n\r";

    printf(info);
    printf(InfoString);
    printf(term);

    /* do not quit... */
}


static void quit(void)
{
#if defined ( __CC_ARM   )
	_sys_exit(0);  // Keil retargeted implimentation for MicroLib
#else
   exit(0);
#endif
}
