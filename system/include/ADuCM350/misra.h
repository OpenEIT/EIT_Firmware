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
 * @file:    misra.h
 * @brief:   ADI MISRA C 2004 compliance matrix and suppression directives for ADuCxxx
 * @version: $Revision: 28525 $
 * @date:    $Date: 2014-11-12 14:51:26 -0500 (Wed, 12 Nov 2014) $
*****************************************************************************/


/*
	SUPPRESS:
    Define macro "ASSERT_ADI_MISRA_SUPPRESSIONS" and include this file at the
    start of driver source files to suppress rules.

    UNSUPPRESS:
    Undefine macro "REVERT_ADI_MISRA_SUPPRESSIONS" and include this file a second
    time (at end-of-file of driver source files) to unsuppress these rules.
*/

/*
    NOTE: Various ARM-supplied CMSIS files (core_cm3.*, etc.) are
    copyrighted by ARM and may not be altered functionally to resolve
    various MISRA compliance violations.  These files have local suppressions.
*/

/*
    NOTE: Make sure the following "suppress" and "default" pragmas
    are kept synchronized so they always both have the same set of
    error numbers numbers.
*/

/*
    IAR Suppression Matrix:
    Error[Pm002]: compiler is configured to allow extensions - all code shall conform to ISO 9899 standard C, with no extensions permitted (MISRA C 2004 rule 1.1)
    Error[Pm064]: functions with variable number of arguments shall not be used (MISRA C 2004 rule 16.1)
    Error[Pm073]: a function should have a single point of exit (MISRA C 2004 rule 14.7)
    Error[Pm081]: #undef should not be used (MISRA C 2004 rule 19.6)
    Error[Pm086]: there shall be at most one occurrence of the # or ## preprocessor operator in a single macro definition (MISRA C 2004 rule 19.12)
    Error[Pm093]: use of union - overlapping storage shall not be used (MISRA C 2004 rule 18.4)
    Error[Pm143]: a function should have a single point of exit at the end of the function (MISRA C 2004 rule 14.7)
    Error[Pm152]: array indexing shall only be applied to objects defined as an array type (MISRA C 2004 rule 17.4)
*/


// compiler-specific suppress/revert preprocessor directives

#if defined ( __CC_ARM   ) // Keil compiler...

// Keil has no built-in no misra checker, so misra suppressions are managed by whatever 3rd-party checker is used...

#elif defined ( __ICCARM__ )  // IAR compiler...

// assert
#ifdef ASSERT_ADI_MISRA_SUPPRESSIONS
#pragma diag_suppress=Pm002,Pm064,Pm073,Pm081,Pm086,Pm093,Pm143,Pm152
#endif // ASSERT_ADI_MISRA_SUPPRESSIONS

// revert
#ifdef REVERT_ADI_MISRA_SUPPRESSIONS
#pragma diag_default=Pm002,Pm064,Pm073,Pm081,Pm086,Pm093,Pm143,Pm152
#endif // REVERT_ADI_MISRA_SUPPRESSIONS

#endif  // compiler variants

// undefines
#undef ASSERT_ADI_MISRA_SUPPRESSIONS
#undef REVERT_ADI_MISRA_SUPPRESSIONS
