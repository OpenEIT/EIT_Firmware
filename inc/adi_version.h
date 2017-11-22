/*********************************************************************************

Copyright (c) 2010-2014 Analog Devices, Inc. All Rights Reserved.

This software is proprietary to Analog Devices, Inc. and its licensors.  By using 
this software you agree to the terms of the associated Analog Devices Software 
License Agreement.

*********************************************************************************/

/*!
 *****************************************************************************
 * @file:    adi_version.h
 * @brief:   Version macros for ADI ADuCMxxx Device Series
 * @version: $Revision: 28525 $
 * @date:    $Date: 2014-11-12 14:51:26 -0500 (Wed, 12 Nov 2014) $
 *****************************************************************************/

#ifndef __ADI_VERSION_H__
#define __ADI_VERSION_H__

/* use a 32-bit versioning scheme that supports numerical compares */
#define ADI_VERSION_MAJOR     2     // must be <= 255
#define ADI_VERSION_MINOR     2     // must be <= 255
#define ADI_VERSION_BUILD     0     // must be <= 255
#define ADI_VERSION_PATCH     0     // must be <= 255

#define ADI_CONSTRUCT_VERSION(a,b,c,d)  ((a << 24) | (b << 16) | (c << 8) | d)

/* known versions */
#define ADI_VERSION_1_3_0_1   ADI_CONSTRUCT_VERSION(1,3,0,1)
#define ADI_VERSION_1_3_0_2   ADI_CONSTRUCT_VERSION(1,3,0,2)
#define ADI_VERSION_1_4_0_0   ADI_CONSTRUCT_VERSION(1,4,0,0)
#define ADI_VERSION_1_4_0_1   ADI_CONSTRUCT_VERSION(1,4,0,1)
#define ADI_VERSION_1_4_0_2   ADI_CONSTRUCT_VERSION(1,4,0,2)
#define ADI_VERSION_1_4_2_3   ADI_CONSTRUCT_VERSION(1,4,2,3)
#define ADI_VERSION_1_4_3_0   ADI_CONSTRUCT_VERSION(1,4,3,0)
#define ADI_VERSION_1_4_3_1   ADI_CONSTRUCT_VERSION(1,4,3,1)
#define ADI_VERSION_1_4_4_0   ADI_CONSTRUCT_VERSION(1,4,4,0)
#define ADI_VERSION_1_4_4_1   ADI_CONSTRUCT_VERSION(1,4,4,1)
#define ADI_VERSION_1_4_4_2   ADI_CONSTRUCT_VERSION(1,4,4,2)
#define ADI_VERSION_2_0_0_0   ADI_CONSTRUCT_VERSION(2,0,0,0)
#define ADI_VERSION_2_1_0_0   ADI_CONSTRUCT_VERSION(2,1,0,0)
#define ADI_VERSION_2_2_0_0   ADI_CONSTRUCT_VERSION(2,2,0,0)

/* test current version against known predefines (see SystemInit() example in system.c) */
#define ADI_VERSION_CURRENT   ADI_CONSTRUCT_VERSION(ADI_VERSION_MAJOR, ADI_VERSION_MINOR, ADI_VERSION_BUILD, ADI_VERSION_PATCH)

#endif // __ADI_VERSION_H__
