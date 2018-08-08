/*********************************************************************************

Copyright (c) 2012-2014 Analog Devices, Inc. All Rights Reserved.

This software is proprietary to Analog Devices, Inc. and its licensors.  By using
this software you agree to the terms of the associated Analog Devices Software
License Agreement.

*********************************************************************************/

/*****************************************************************************
 * @file:    RtcTest.c
 * @brief:   Real-Time Clock Test for ADuCM350
 * @version: $Revision: 29234 $
 * @date:    $Date: 2014-12-15 12:20:49 -0500 (Mon, 15 Dec 2014) $
 *****************************************************************************/

#include <time.h>
#include <stddef.h>  // for 'NULL'
#include <stdio.h>   // for scanf
#include <string.h>  // for strncmp

#include "rtc.h"
#include "wut.h"
#include "gpio.h"

#include "test_common.h"

// leap-year compute macro (ignores leap-seconds)
#define LEAP_YEAR(x) (((0==x%4)&&(0!=x%100))||(0==x%400))


/* uncomment this macro to force RTC clock reset to last build timestamp */
//#define ADI_RTC_RESET

/* uncomment this macro to run RTC extended test */
//#define ADI_RTC_EXTENDED_TEST

/* device and board specific values selected according to computed trim measurement */
/* THESE VALUES ARE UNIQUE TO THE EVAL-ADUCM350EBZ REV. 0 BOARD, SERIAL#: AVAS 35070 */
#define ADI_RTC_TRIM_INTERVAL    (uint32_t)ADI_RTC_TRIM_INTERVAL_14
#define ADI_RTC_TRIM_DIRECTION   (uint32_t)ADI_RTC_TRIM_SUB
#define ADI_RTC_TRIM_VALUE       (uint32_t)ADI_RTC_TRIM_1

/* Device handle */
ADI_RTC_HANDLE hRTC = NULL;
volatile bool_t bRtcAlarmFlag;
volatile bool_t bRtcInterrupt;
volatile bool_t bWdtInterrupt;
volatile bool_t bHibernateExitFlag;

/* prototypes */
void rtc_Init(void);
void rtc_Calibrate(void);
void rtc_AlarmTest(void);
void rtc_ExtendedTest(void);
void rtc_ReportTime(void);
uint32_t BuildSeconds(void);

/* callbacks */
void rtcCallback (void *pCBParam, uint32_t Event, void *EventArg);
void wutCallback (void *pCBParam, uint32_t Event, void *EventArg);

static void Ext_Int8_Callback (void *pCBParam, uint32_t Event, void *pArg);

extern int32_t adi_initpinmux(void);

int main (void)
{
    /* Initialize system */
    SystemInit();

    /* test system initialization */
    test_Init();

    PERF("Use the \"ADI_RTC_CALIBRATE\"     preprocessor macro to enter RTC calibration test mode.");
    PERF("Use the \"ADI_RTC_RESET\"         preprocessor macro to force clock reset to latest build time.");
    PERF("Use the \"ADI_RTC_EXTENDED_TEST\" preprocessor macro to run extended test with pushbutton exit.\n");

    /* initialize driver */
    rtc_Init();

    /* calibrate */
    rtc_Calibrate();

    /* test alarm */
    rtc_AlarmTest();

#ifdef ADI_RTC_EXTENDED_TEST
    /* extended test */
    rtc_ExtendedTest();
#endif

    PASS();
}


void rtc_Init (void) {

    /* callbacks */
    ADI_RTC_INT_SOURCE_TYPE callbacks = (ADI_RTC_INT_SOURCE_TYPE)
                                       ( ADI_RTC_INT_SOURCE_WRITE_PEND
                                       | ADI_RTC_INT_SOURCE_WRITE_SYNC
                                       | ADI_RTC_INT_SOURCE_WRITE_PENDERR
                                       | ADI_RTC_INT_SOURCE_ISO_DONE
                                       | ADI_RTC_INT_SOURCE_LCD_UPDATE
                                       | ADI_RTC_INT_SOURCE_ALARM
                                       | ADI_RTC_INT_SOURCE_FAIL);
    uint32_t buildTime = BuildSeconds();
    ADI_RTC_RESULT_TYPE result;

    result = adi_RTC_Init(ADI_RTC_DEVID_0, &hRTC);

    /* retry on failsafe */
    if (ADI_RTC_ERR_CLOCK_FAILSAFE == result) {

        /* clear the failsafe */
        adi_RTC_ClearFailSafe();

        /* un-init RTC for a clean restart, but ignore failure */
        adi_RTC_UnInit(hRTC);

        /* re-init RTC */
        if (ADI_RTC_SUCCESS != adi_RTC_Init(ADI_RTC_DEVID_0, &hRTC))
            FAIL("Double fault on adi_RTC_Init");

        PERF("Resetting clock and trim values after init failure");

        /* set clock to latest build time */
        if (ADI_RTC_SUCCESS != adi_RTC_SetCount(hRTC, buildTime))
            FAIL("adi_RTC_SetCount failed");

        /* apply pre-computed calibration BOARD-SPECIFIC trim values */
        if (adi_RTC_SetTrim(hRTC, ADI_RTC_TRIM_INTERVAL | ADI_RTC_TRIM_DIRECTION | ADI_RTC_TRIM_VALUE))
            FAIL("adi_RTC_SetTrim failed");

        /* enable trimming */
        if (adi_RTC_EnableTrim(hRTC, true))
            FAIL("adi_RTC_EnableTrim failed");

    /* catch all other open failures */
    } else {
		if (result != ADI_RTC_SUCCESS)
		  FAIL("Generic failure to initialize the RTC");
    }

    /* RTC opened successfully... */

    /* disable alarm */
    if (ADI_RTC_SUCCESS != adi_RTC_EnableAlarm(hRTC, false))
        FAIL("adi_RTC_EnableAlarm failed");

#ifdef ADI_RTC_RESET
    /* force a reset to the latest build timestamp */
    PERF("Resetting clock");
    if (ADI_RTC_SUCCESS != adi_RTC_SetCount(hRTC, buildTime))
        FAIL("adi_RTC_SetCount failed");
    PERF("New time is:");
    rtc_ReportTime();
#endif

    /* register callback handler for all interrupts */
    if (ADI_RTC_SUCCESS != adi_RTC_RegisterCallback (hRTC,  rtcCallback, callbacks)) {
        FAIL("adi_RTC_RegisterCallback failed");
    }

    /* enable RTC */
    if (ADI_RTC_SUCCESS != adi_RTC_EnableDevice(hRTC, true))
        FAIL("adi_RTC_EnableDevice failed");


	/* program GPIO to capture P0.10 pushbutton interrupt... */

	/* initialize GPIO driver */
	if (adi_GPIO_Init()) {
		FAIL("adi_GPIO_Init failed");
	}

	/* enable P0.10 input */
    if (adi_GPIO_SetInputEnable(ADI_GPIO_PORT_0, ADI_GPIO_PIN_10, true)) {
    	FAIL("Initialise_GPIO: adi_GPIO_SetInputEnable failed");
    }

	/* disable P0.10 output */
    if (adi_GPIO_SetOutputEnable(ADI_GPIO_PORT_0, ADI_GPIO_PIN_10, false)) {
    	FAIL("Initialise_GPIO: adi_GPIO_SetOutputEnable failed");
    }

	/* set P0.10 pullep enable */
    if (adi_GPIO_SetPullUpEnable(ADI_GPIO_PORT_0, ADI_GPIO_PIN_10, true)) {
    	FAIL("Initialise_GPIO: adi_GPIO_SetOutputEnable failed");
    }

    /* Register the external interrupt callback */
    if(adi_GPIO_RegisterCallback(EINT8_IRQn,  Ext_Int8_Callback, NULL)) {
      FAIL("Initialise_GPIO: adi_GPIO_RegisterCallbackExtInt failed");
    }

	/* enable P0.10 as external interrupt */
    if (adi_GPIO_EnableIRQ(EINT8_IRQn,  ADI_GPIO_IRQ_EITHER_EDGE)) {
    	FAIL("Initialise_GPIO: adi_GPIO_EnableIRQ failed");
    }

	/* release GPIO driver */
	if (adi_GPIO_UnInit()) {
		FAIL("adi_GPIO_Init failed");
	}
}


void rtc_Calibrate (void) {

#ifdef ADI_RTC_CALIBRATE

    /*

    Compute the LF crystal trim values to compensate the RTC.  This can
    come from a static measure (a frequency counter), a real-time drift measure
    based on a USB transaction, Ethernet NTP or PTP protocol, or some other
    external reference.

    Commercial crystals typically run between 20-100 ppm.  As an exercise, we
    demonstrate trimming a particular crystal and board configuration in which
    we measure an untrimmed error of about +58.6ppm (0.00586%).  This corresponds
    to a raw clock about 35.5 seconds/week fast (30 minutes/year).

    Available Trim Corrections:
    	X axis: trim interval (seconds)
    	Y axis: trim value (seconds)
    	Table content: trim correction (ppm)
      Value     16384    32768    65536   131072 (Interval)
        0        0.00     0.00     0.00     0.00
        1       61.04    30.52    15.26     7.63
        2      122.07    61.04    30.52    15.26
        3      183.11    91.55    45.78    22.89
        4      244.14   122.07    61.04    30.52
        5      305.18   152.59    76.29    38.15
        6      366.21   183.11    91.55    45.78
        7      427.25   213.62   106.81    53.41

    Referencing the trim table, we see the closest matching ppm correction for
    our example is 61.04.  In case there are different combinations yielding
    the same desired correction, we prefer the shortest trim interval (and
    smallest trim value) so as to minimize instantaneous drift.

    So we choose a trim interval of 2^14 seconds with a negative trim value of 1
    second, subtracting 1 second every 4.5 hours to "slow" the fast crystal down
    to a more reasonable rate.  This particular trim leaves a residual error of
    negative 2.44ppm (0.000244%), making the trimmed clock a tad slow (less than
    1.5 seconds/week or about 1.3 minutes/year), but much better than the
    untrimmed accuracy of 30 minutes/year.

    */

    /* dial-up external LF crystal to clockout pin (P1.7) for measurement */
    if (adi_GPIO_Init())
        FAIL("adi_GPIO_Init failed");
    SetSystemClockMux(ADI_SYS_CLOCK_MUX_LFCLK_LFXTAL);   // select LF crystal
    SetSystemClockMux(ADI_SYS_CLOCK_MUX_OUTPUT_LF_CLK);  // route output

    /* Use static pinmuxing */
    adi_initpinmux();

    if (adi_GPIO_UnInit())
        FAIL("adi_GPIO_UnInit failed");

    PERF("RTC clockout programmed to P1.7 for calibration...");

    /* program the BOARD-SPECIFIC computed trim value, as described above */
    if (adi_RTC_SetTrim(hRTC, ADI_RTC_TRIM_INTERVAL | ADI_RTC_TRIM_DIRECTION | ADI_RTC_TRIM_VALUE))
        FAIL("adi_RTC_SetTrim failed");

    /* enable trim */
    if (adi_RTC_EnableTrim(hRTC, true))
        FAIL("adi_RTC_EnableTrim failed");

#endif
}


void rtc_AlarmTest (void) {

#define RTC_ALARM_OFFSET 5

    ADI_WUT_DEV_HANDLE hWUT;
    uint32_t rtcCount;

    /* initialize flags */
    bRtcAlarmFlag = bRtcInterrupt = bWdtInterrupt = false;

    /* get the current count */
    if (ADI_RTC_SUCCESS != adi_RTC_GetCount(hRTC, &rtcCount))
        FAIL("adi_RTC_GetCount failed");

    /* set RTC alarm */
    if (ADI_RTC_SUCCESS != adi_RTC_SetAlarm(hRTC, rtcCount+RTC_ALARM_OFFSET))
        FAIL("adi_RTC_SetAlarm failed");

    /* enable RTC alarm */
    if (ADI_RTC_SUCCESS != adi_RTC_EnableAlarm(hRTC, true))
        FAIL("adi_RTC_EnableAlarm failed");

    /* enable alarm interrupting */
//    if (ADI_RTC_SUCCESS != adi_RTC_EnableInterrupts(hRTC, ADI_RTC_INT_ENA_ALARM, true))
//        FAIL("adi_RTC_EnableInterrupts failed");
//
//    /* program wake-up timerB (in seconds) as a stop-gap to interrupt from hibernation if the RTC ALARM fails */
//    {
//        NVIC_EnableIRQ(WUT_IRQn);
//        if (adi_WUT_Init(ADI_WUT_DEVID_0, &hWUT))                              FAIL("adi_WUT_Init failed");
//        if (adi_WUT_RegisterCallback(hWUT, wutCallback, ADI_WUT_TARGET_MASK))  FAIL("adi_WUT_RegisterCallback failed");
//        if (adi_WUT_SetClockSelect(hWUT, ADI_WUT_CLK_LFXTAL))                  FAIL("adi_WUT_SetClockSelect failed");
//        if (adi_WUT_SetPrescaler(hWUT, ADI_WUT_PRE_DIV32768))                  FAIL("adi_WUT_SetPrescaler failed");
//        if (adi_WUT_SetTimerMode(hWUT, ADI_WUT_MODE_FREERUNNING))              FAIL("adi_WUT_SetTimerMode failed");
//        if (adi_WUT_SetComparator(hWUT, ADI_WUT_COMPB, 2*RTC_ALARM_OFFSET + adi_WUT_GetCurrentCount(hWUT)))
//                                                                               FAIL("adi_WUT_SetComparator failed");
//        if (adi_WUT_SetInterruptEnable(hWUT, ADI_WUT_COMPB, true))             FAIL("adi_WUT_SetInterruptEnable failed");
//        if (adi_WUT_SetWakeUpEnable(hWUT, true))                               FAIL("adi_WUT_SetWakeUpEnable failed");
//        if (adi_WUT_SetTimerEnable(hWUT, true))                                FAIL("adi_WUT_SetTimerEnable failed");
//    }
//
//    /* go to sleep and await RTC ALARM interrupt */
//    PERF("Short ALARM test starting at:");
//    rtc_ReportTime();
//
//	/* allow time for uart output to clear before hibernate cuts clock */
//	for (int i=0; i<1000; i++);
//
//	/* enter full hibernate mode with wakeup flag and no masking */
//	bHibernateExitFlag = false;
//	if (SystemEnterLowPowerMode(ADI_SYS_MODE_HIBERNATE, &bHibernateExitFlag, 0))
//		FAIL("SystemEnterLowPowerMode failed");
//
//    PERF("Short ALARM test complete at:");
//    rtc_ReportTime();
//
//    /* release WUT (disables WUT interrupt) */
//    NVIC_DisableIRQ(WUT_IRQn);
//    if (adi_WUT_UnInit(hWUT))
//        FAIL("adi_WUT_UnInit failed");

    /* verify expected results */
    if (bWdtInterrupt)   FAIL("rtc_AlarmTest got unexpected WUT interrupt");
    if (!bRtcInterrupt)  FAIL("rtc_AlarmTest failed to get expected RTC interrupt");
    if (!bRtcAlarmFlag)  FAIL("rtc_AlarmTest failed to get expected RTC ALARM interrupt");

    /* disable alarm */
    if (ADI_RTC_SUCCESS != adi_RTC_EnableAlarm(hRTC, false))
        FAIL("adi_RTC_EnableAlarm failed");
}


void rtc_ExtendedTest (void) {

    char buffer[128];
    clock_t start, end;

    /* go to sleep and await the P0.10 pushbutton external interrupt */
    PERF("Extended hibernation test starting at:");
    rtc_ReportTime();

    PERF("Press P0.10 pushbutton to end extended test...");

    /* allow time for uart output to clear before hibernate gates UART clock */
    for (int i=0; i<1000; i++) ;

    /* hibernate... */
    bHibernateExitFlag = false;
    start = clock();
	// full hibernate with wakeup flag and no masking
    if (SystemEnterLowPowerMode(ADI_SYS_MODE_HIBERNATE, &bHibernateExitFlag, 0))
        FAIL("SystemEnterLowPowerMode failed");
    end = clock();

    PERF("Extended hibernation test complete at:");
    rtc_ReportTime();

    sprintf(buffer, "Extended test elapsed time: %d seconds", end-start);
    PERF(buffer);
}


/* test standard ctime (time.h) constructs */
void rtc_ReportTime(void) {

    char buffer[128];

    time_t rawtime;

    // get the RTC count through the "time" CRTL function
    time(&rawtime);

    // print raw count
    sprintf (buffer, "Raw time: %d", rawtime);
    PERF(buffer);

    // convert to UTC string and print that too
    sprintf (buffer, "UTC/GMT time: %s", ctime(&rawtime));
    PERF(buffer);
}


uint32_t BuildSeconds(void)
{
    /* count up seconds from the epoc (1/1/70) to the most recient build time */

    char timestamp[] = __DATE__ " " __TIME__;
    int month_days [] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    uint32_t days, month, date, year, hours, minutes, seconds;
    char Month[4];

    // parse the build timestamp
    sscanf(timestamp, "%s %d %d %d:%d:%d", Month, &date, &year, &hours, &minutes, &seconds);

    // parse ASCII month to a value
    if     ( !strncmp(Month, "Jan", 3 )) month = 1;
    else if( !strncmp(Month, "Feb", 3 )) month = 2;
    else if( !strncmp(Month, "Mar", 3 )) month = 3;
    else if( !strncmp(Month, "Apr", 3 )) month = 4;
    else if( !strncmp(Month, "May", 3 )) month = 5;
    else if( !strncmp(Month, "Jun", 3 )) month = 6;
    else if( !strncmp(Month, "Jul", 3 )) month = 7;
    else if( !strncmp(Month, "Aug", 3 )) month = 8;
    else if( !strncmp(Month, "Sep", 3 )) month = 9;
    else if( !strncmp(Month, "Oct", 3 )) month = 10;
    else if( !strncmp(Month, "Nov", 3 )) month = 11;
    else if( !strncmp(Month, "Dec", 3 )) month = 12;

    // count days from prior years
    days=0;
    for (int y=1970; y<year; y++) {
        days += 365;
        if (LEAP_YEAR(y))
            days += 1;
    }

    // add days for current year
    for (int m=1; m<month; m++)
        days += month_days[m-1];

    // adjust if current year is a leap year
    if ( (LEAP_YEAR(year) && ( (month > 2) || ((month == 2) && (date == 29)) ) ) )
        days += 1;

    // add days this month (not including current day)
    days += date-1;

    return (days*24*60*60 + hours*60*60 + minutes*60 + seconds);
}

/* RTC Callback handler */
void rtcCallback (void *pCBParam, uint32_t nEvent, void *EventArg) {

    bRtcInterrupt = true;

    /* process RTC interrupts (cleared by driver) */
    if (ADI_RTC_INT_SOURCE_WRITE_PEND & (ADI_RTC_INT_SOURCE_TYPE) nEvent) {
        PERF("got RTC interrupt callback with ADI_RTC_INT_SOURCE_WRITE_PEND status");
    }

    if (ADI_RTC_INT_SOURCE_WRITE_SYNC & (ADI_RTC_INT_SOURCE_TYPE) nEvent) {
        PERF("got RTC interrupt callback with ADI_RTC_INT_SOURCE_WRITE_SYNC status");
    }

    if (ADI_RTC_INT_SOURCE_WRITE_PENDERR & (ADI_RTC_INT_SOURCE_TYPE) nEvent) {
        PERF("got RTC interrupt callback with ADI_RTC_INT_SOURCE_WRITE_PENDERR status");
    }

    if (ADI_RTC_INT_SOURCE_ISO_DONE & (ADI_RTC_INT_SOURCE_TYPE) nEvent) {
        PERF("got RTC interrupt callback with ADI_RTC_INT_SOURCE_ISO_DONE status");
    }

    if (ADI_RTC_INT_SOURCE_LCD_UPDATE & (ADI_RTC_INT_SOURCE_TYPE) nEvent) {
        PERF("got RTC interrupt callbackwithon ADI_RTC_INT_SOURCE_LCD_UPDATE status");
    }

    if (ADI_RTC_INT_SOURCE_ALARM & (ADI_RTC_INT_SOURCE_TYPE) nEvent) {
        PERF("got RTC interrupt callback with ADI_RTC_INT_SOURCE_ALARM status");
        bRtcAlarmFlag = true;       // note alarm flag
        bHibernateExitFlag = true;  // exit hibernation on return from interrupt
    }

    if (ADI_RTC_INT_SOURCE_FAIL & (ADI_RTC_INT_SOURCE_TYPE) nEvent) {
        PERF("got RTC interrupt callback with ADI_RTC_INT_SOURCE_FAIL status");
    }
}


/* WUT callback */
static void wutCallback(void* hWut, uint32_t Event, void* pArg)
{
    pADI_WUT->T2CLRI |= T2CLRI_WUFB_CLR;
    bHibernateExitFlag = true;
    bWdtInterrupt = true;
    FAIL("RTC test failed: Got failsafe WUT interrupt");
}


/* External interrupt callback */
static void Ext_Int8_Callback (void *pCBParam, uint32_t Event, void *pArg)
{
    /* clear the interrupt */
    NVIC_ClearPendingIRQ(EINT8_IRQn);

    /* also call hibernation exit API */
    SystemExitLowPowerMode(&bHibernateExitFlag);
}
