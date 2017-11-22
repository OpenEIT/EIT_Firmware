
/*********************************************************************************

  Copyright(c) 2013 Analog Devices, Inc. All Rights Reserved.

  This software is proprietary and confidential.  By using this software you agree
  to the terms of the associated Analog Devices License Agreement.

*********************************************************************************/

/*!
* @file      adi_usbh_dev_musbmhdrc_common.c
*
* @brief     Common functions across platforms are kept here.
*
*
*/
#ifdef _MISRA_RULES
#pragma diag(push)
#pragma diag(suppress:misra_rules_all:"Suppress all misra rules")
#endif

/** @addtogroup USB MUSBMHDRC controller driver
 *  @{
 */

#include <stdint.h>
#include <device.h>

/* ADUCM350 */   
#if defined(ADI_ADUCM350)
#define CYCLE_CNT_TYPE                 uint32_t

#else
#define CYCLE_CNT_TYPE                 cycle_t   

/* Griffin */   
#if defined(__ADSPSC589_FAMILY__)                
#if defined (__ADSPSHARC__)                      /* Griffin Sharc Core */
#include <cycle_count.h>
#elif defined(__ADSPARM__)                       /* Griffin Arm Cortex A5 core */
#include <cyclecount.h>
#endif

/* Blackfin */   
#elif defined(__ADSPBLACKFIN__)
#include <cycle_count.h>
#else
#error "processor not supported"        
#endif
#endif
   
#include <limits.h>
#include <drivers/usb/controller/musbmhdrc/adi_usb_dev_musbmhdrc.h>

#if defined(ADI_ADUCM350)
#define  __PROCESSOR_SPEED__       16000000                   /* 16 MHz (TODO: Didn't find this Macro definition) */   
#elif defined(__ADSPSC589_FAMILY__) && defined (__ADSPARM__)  /* Griffin Arm Cortex A5 core */
#define  __PROCESSOR_SPEED__       450000000                  /* 450 MHz (TODO: Didn't find this Macro definition) */
#endif

/*
 * ******************************************* Function Definitions **********************************
 */
#if defined(ADI_ADUCM350)
static void adi_usbdrv_WaitOneMillisec(void)
{
    int i = 0x1000;
    for (; i>0; i--);
}	
#else
static void adi_usbdrv_WaitOneMillisec(void)
{
    CYCLE_CNT_TYPE CurCycleCount, EndCycleCount;

#if defined(__ADSPSC589_FAMILY__) && defined (__ADSPARM__)  /* Griffin Arm Cortex A5 core */
    CurCycleCount = CCNTR_READ;
#else
    _GET_CYCLE_COUNT(CurCycleCount);
#endif

    EndCycleCount= CurCycleCount + ((CYCLE_CNT_TYPE)__PROCESSOR_SPEED__/(CYCLE_CNT_TYPE)1000);

    while (CurCycleCount < EndCycleCount)
    {
#if defined(__ADSPSC589_FAMILY__) && defined (__ADSPARM__)  /* Griffin Arm Cortex A5 core */
        CurCycleCount = CCNTR_READ;
#else
        _GET_CYCLE_COUNT(CurCycleCount);
#endif
    }

}
#endif

#if 0 /* Currently CYCLES_GET is not working for ADI_ADUCM350 */
static void adi_usbdrv_WaitOneMillisec(void)
{
    CYCLE_CNT_TYPE CurCycleCount, EndCycleCount;

#if defined(ADI_ADUCM350)    
    CurCycleCount = CYCLES_GET;
#elif defined(__ADSPSC589_FAMILY__) && defined (__ADSPARM__)  /* Griffin Arm Cortex A5 core */
    CurCycleCount = CCNTR_READ;
#else
    _GET_CYCLE_COUNT(CurCycleCount);
#endif

    EndCycleCount= CurCycleCount + ((CYCLE_CNT_TYPE)__PROCESSOR_SPEED__/(CYCLE_CNT_TYPE)1000);

    while (CurCycleCount < EndCycleCount)
    {
#if defined(ADI_ADUCM350)    
        CurCycleCount = CYCLES_GET;
#elif defined(__ADSPSC589_FAMILY__) && defined (__ADSPARM__)  /* Griffin Arm Cortex A5 core */
        CurCycleCount = CCNTR_READ;
#else
        _GET_CYCLE_COUNT(CurCycleCount);
#endif
    }
}
#endif

static void adi_usbdrv_WaitMilliSec(uint32_t Millisec)
{
    while (Millisec != (uint32_t)0)
    {
        adi_usbdrv_WaitOneMillisec();
        Millisec--;
    }
}


#if defined(__ADSPBLACKFIN__)
/*
* if CPLB data cache is enabled then we provide functions to maintain cache-dma coherency
*/
#include <cplb.h>
extern  int     __cplb_ctrl;
/*
* Local functions' prototypes
*/
static void FlushArea(void *StartAddress, void *EndAddress);
static void FlushInvArea(void *StartAddress, void *EndAddress);

/*
* Assembly code to Flush (& invalidate) a cache line
*/
#define FLUSH(P)           {asm volatile("FLUSH[%0++];":"+p"(P));}
#define FLUSHINV(P)        {asm volatile("FLUSHINV[%0++];":"+p"(P));}

static void adi_usbdrv_FlushCachedBuffer(void *pData, uint32_t DataLen, ADI_USB_EP_DIR EpDir)
{

    if (EpDir == ADI_USB_EP_DIR_RX)
    {
        FlushInvArea(pData,
                     (void*)(((uint8_t*)pData)+DataLen));
    }
    else
    {
        FlushArea(pData,
                  (void*)(((uint8_t*)pData)+DataLen));
    }
}


static bool adi_usbdrv_IsDataCacheEnabled(void)
{
    return ((__cplb_ctrl) & (CPLB_ENABLE_DCACHE | CPLB_ENABLE_DCACHE2));
}
/*
* Flushes the contents of the cache from specified start address to the end address
*/
static void FlushArea(void *StartAddress, void *EndAddress)
{
    StartAddress = (void *)(((uint32_t)StartAddress)&(~31));
    while (StartAddress < EndAddress) {
        FLUSH(StartAddress);
    }
}

/*
* Flushes and invalidates the contents of cache from the  specified start address to
* the end address
*/
static void FlushInvArea(void *StartAddress, void *EndAddress)
{
    StartAddress = (void *)(((uint32_t)StartAddress)&(~31));
    while (StartAddress < EndAddress) {
        FLUSHINV(StartAddress);
    }
}


#else
static bool adi_usbdrv_IsDataCacheEnabled(void)
{
    return false;
}
static void adi_usbdrv_FlushCachedBuffer(void *pData, uint32_t DataLen, ADI_USB_EP_DIR EpDir)
{
	return;
}
#endif

/*@}*/
#ifdef _MISRA_RULES
#pragma diag(pop)
#endif
