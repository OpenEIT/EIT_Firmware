
#ifndef __TEST_COMMON_H__
#define __TEST_COMMON_H__

#include "wdt.h"

/********************************************************************************
* enums and definitions
*********************************************************************************/

//AG
/* Macro to enable the returning of AFE data using the UART */
/*      1 = return AFE data on UART                         */
/*      0 = return AFE data on SW (Std Output)              */
#define USE_UART_FOR_DATA           (1)

/* preprocessor tweaks to convert "__LINE__" from int to a char string... */
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define AT __FILE__ ":" TOSTRING(__LINE__)

#define PASS() test_Pass()

#define FAIL(s) test_Fail(AT": "s)

#define PERF(s) test_Perf(s)

#if defined ( __ICCARM__ )  /* IAR-specific pragmas */
/* restore MISRA rule about using multiple "##" preprocessor directives */
#pragma diag_default=Pm086
#endif /* __ICCARM__ */

/********************************************************************************
* API function prototypes
*********************************************************************************/
void test_Init(void);
void test_Pass(void);
void test_Fail(char *FailureReason);
void test_Perf(char *InfoString);


#endif /* __TEST_COMMON_H__ */
