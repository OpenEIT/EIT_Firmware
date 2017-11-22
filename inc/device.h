/*********************************************************************************

Copyright (c) 2010-2014 Analog Devices, Inc. All Rights Reserved.

This software is proprietary to Analog Devices, Inc. and its licensors.  By using 
this software you agree to the terms of the associated Analog Devices Software 
License Agreement.

*********************************************************************************/

/*!
 *****************************************************************************
 * @file:    device.h
 * @brief:   CMSIS Cortex-M3 Core Peripheral Access Layer Header File for
 *           ADI ADuCMxxx Device Series
 * @version: $Revision: 28525 $
 * @date:    $Date: 2014-11-12 14:51:26 -0500 (Wed, 12 Nov 2014) $
 *****************************************************************************/

/*!  \addtogroup MMR_Layout MMR/ISR Mappings
 *  @{
 */

#ifndef __DEVICE_H__
#define __DEVICE_H__

// verify presence of a supported compiler (Keil or IAR at this time)
#if !defined(__ICCARM__) && !defined(__CC_ARM)
#error "No supported compiler found, please install one of the minimim required compilers."
#endif

// verify minimum required IAR compiler
#ifdef __ICCARM__
#define MIN_CC_VERSION 6010001  // known value for release 6.10.1
#if (MIN_CC_VERSION > __VER__)
#error "Unsupported compiler version, please install minimum required compiler version."
#endif
#endif

// verify minimum required Keil compiler
#ifdef __CC_ARM
#define MIN_CC_VERSION 410713  // known value for release 4.21
#if (MIN_CC_VERSION > __ARMCC_VERSION)
#error "Unsupported compiler version, please install minimum required compiler version."
#endif
#endif

/* Feature macro for conditional compilation */
#define ADI_ADUCM350  /*!< PreProcessor feature macro */

/* pickup global configurations */
#include "adi_global_config.h"

/* pickup multi thread support macros */
#include "adi_rtos.h"

/* include global MISRA suppressions */
#include "misra.h"

/* pickup register bitfield and bit mask macros */
#include "macros.h"

/* pickup system interrupt ID (SID) defines */
#include "adi_sid_cm350.h"

/* pickup NVIC SID Mappings and interrup hooking prototypes */
#include "adi_nvic.h"

/* define numerical versioning info for ADI, guaranteeing arithmetic compares, etc. */
#include "adi_version.h"  // distinct from OSAL use of "version.h"

/* C++ linkage */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* power control unlock keys */
#define PWRKEY_VALUE_KEY1                (0x4859)
#define PWRKEY_VALUE_KEY2                (0xF27B)

/*!
 * \enum IRQn_Type
 * Interrupt Number Assignments
 */
typedef enum
{
/* Cortex-M3 Processor Exceptions Numbers *****************************************************/
	Reset_IRQn            = ADI_INT_RESET,         /*!<  Any reset                            */
	NonMaskableInt_IRQn   = ADI_INT_NMI,           /*!<  Non Maskable Interrupt               */
	HardFault_IRQn        = ADI_INT_HARD_FAULT,    /*!< Cortex-M3 default fault interrupt     */
	MemoryManagement_IRQn = ADI_INT_MEM_MGMT,      /*!< Cortex-M3 Memory Management Interrupt */
	BusFault_IRQn         = ADI_INT_BUS_FAULT,     /*!< Cortex-M3 Bus Fault Interrupt         */
	UsageFault_IRQn       = ADI_INT_USAGE_FAULT,   /*!< Cortex-M3 Usage Fault Interrupt       */
	/** Placeholder: IRQn =  -9 is reserved on the ADuM350                                    */
	/** Placeholder: IRQn =  -8 is reserved on the ADuM350                                    */
	/** Placeholder: IRQn =  -7 is reserved on the ADuM350                                    */
	/** Placeholder: IRQn =  -6 is reserved on the ADuM350                                    */
	SVCall_IRQn           = ADI_INT_SVCALL,        /*!< Cortex-M3 SV Call Interrupt           */
	DebugMonitor_IRQn     = ADI_INT_DEBUG_MON,     /*!< Cortex-M3 Debug Monitor Interrupt     */
	/** Placeholder: IRQn =  -3 (number) is reserved on the ADuM350                           */
	PendSV_IRQn           = ADI_INT_PENDSV,        /*!< Cortex-M3 Pend SV Interrupt           */
	SysTick_IRQn          = ADI_INT_SYSTICK,       /*!< Cortex-M3 System Tick Interrupt       */

/* ADuCM350 Specific Programmable Interrupt Numbers *******************************************/
	WUT_IRQn              = ADI_INT_WUT,           /*!< Wakeup Timer Interrupt                */
	EINT0_IRQn            = ADI_INT_EINT0,         /*!< External Interrupt 0                  */
	EINT1_IRQn            = ADI_INT_EINT1,         /*!< External Interrupt 1                  */
	EINT2_IRQn            = ADI_INT_EINT2,         /*!< External Interrupt 2                  */
	EINT3_IRQn            = ADI_INT_EINT3,         /*!< External Interrupt 3                  */
	EINT4_IRQn            = ADI_INT_EINT4,         /*!< External Interrupt 4                  */
	EINT5_IRQn            = ADI_INT_EINT5,         /*!< External Interrupt 5                  */
	EINT6_IRQn            = ADI_INT_EINT6,         /*!< External Interrupt 6                  */
	EINT7_IRQn            = ADI_INT_EINT7,         /*!< External Interrupt 7                  */
	EINT8_IRQn            = ADI_INT_EINT8,         /*!< External Interrupt 8                  */
	WDT_IRQn              = ADI_INT_WDT,           /*!< Watchdog Timer Interrupt              */
	TIMER0_IRQn           = ADI_INT_TIMER0,        /*!< GP Timer0 Interrupt                   */
	TIMER1_IRQn           = ADI_INT_TIMER1,        /*!< GP Timer1 Interrupt                   */
	FLASH0_IRQn           = ADI_INT_FLASH0,        /*!< Bank0 Flash Memory Interrupt          */
	UART_IRQn             = ADI_INT_UART,          /*!< UART Interrupt                        */
	SPI0_IRQn             = ADI_INT_SPI0,          /*!< SPI0 Interrupt                        */
	SPIH_IRQn             = ADI_INT_SPIH,          /*!< SPI-H Interrupt                       */
	I2CS_IRQn             = ADI_INT_I2CS,          /*!< I2C Slave Interrupt                   */
	I2CM_IRQn             = ADI_INT_I2CM,          /*!< I2C Master Interrupt                  */
	DMA_ERR_IRQn          = ADI_INT_DMA_ERR,       /*!< Generic DMA Error Interrupt           */
	DMA_SPIH_TX_IRQn      = ADI_INT_DMA_SPIH_TX,   /*!< DMA SPI-H TX Interrupt                */
	DMA_SPIH_RX_IRQn      = ADI_INT_DMA_SPIH_RX,   /*!< DMA SPI-H RX Interrupt                */
	DMA_SPI0_TX_IRQn      = ADI_INT_DMA_SPI0_TX,   /*!< DMA SPI1 TX Interrupt                 */
	DMA_SPI0_RX_IRQn      = ADI_INT_DMA_SPI0_RX,   /*!< DMA SPI1 RX Interrupt                 */
	DMA_SPI1_TX_IRQn      = ADI_INT_DMA_SPI1_TX,   /*!< DMA SPI1 TX Interrupt                 */
	DMA_SPI1_RX_IRQn      = ADI_INT_DMA_SPI1_RX,   /*!< DMA SPI1 RX Interrupt                 */
	DMA_UART_TX_IRQn      = ADI_INT_DMA_UART_TX,   /*!< DMA UART TX Interrupt                 */
	DMA_UART_RX_IRQn      = ADI_INT_DMA_UART_RX,   /*!< DMA UART RX Interrupt                 */
	DMA_I2CS_TX_IRQn      = ADI_INT_DMA_I2CS_TX,   /*!< DMA I2C Slave TX Interrupt            */
	DMA_I2CS_RX_IRQn      = ADI_INT_DMA_I2CS_RX,   /*!< DMA I2C Slave RX Interrupt            */
	DMA_I2CM_IRQn         = ADI_INT_DMA_I2CM,      /*!< DMA I2C Shared Master Interrupt       */
	DMA_AFE_TX_IRQn       = ADI_INT_DMA_AFE_TX,    /*!< DMA AFE TX Interrupt                  */
	DMA_AFE_RX_IRQn       = ADI_INT_DMA_AFE_RX,    /*!< DMA AFE RX Interrupt                  */
	DMA_CRC_IRQn          = ADI_INT_DMA_CRC,       /*!< DMA CRC Interrupt                     */
	DMA_PDI_IRQn          = ADI_INT_DMA_PDI,       /*!< DMA Parallel Display Intf. Interrupt  */
	DMA_I2S_IRQn          = ADI_INT_DMA_I2S,       /*!< DMA I2S Interrupt                     */
	USB_WAKEUP_IRQn       = ADI_INT_USB_WAKEUP,    /*!< USB Wakeup interrupt                  */
	USB_CNTL_IRQn         = ADI_INT_USB,           /*!< USB Control interrupt                 */
	USB_DMA_IRQn          = ADI_INT_USB_DMA,       /*!< USB DMA interrupt                     */
	I2S_IRQn              = ADI_INT_I2S,           /*!< I2S interrupt                         */
	TIMER2_IRQn           = ADI_INT_TIMER2,        /*!< GP Timer2 Interrupt                   */
	FLASH1_IRQn           = ADI_INT_FLASH1,        /*!< Bank1 Flash Memory Interrupt          */
	SPI1_IRQn             = ADI_INT_SPI1,          /*!< SPI1 Interrupt                        */
	RTC_IRQn              = ADI_INT_RTC,           /*!< Realtime Clock Interrupt              */
	/** Placeholder: IRQn = 44 is reserved on the ADuCM350                                     */
	BEEP_IRQn             = ADI_INT_BEEP,          /*!< Beeper Interrupt                      */
	LCD_IRQn              = ADI_INT_LCD,           /*!< LCD Interrupt                         */
	GPIOA_IRQn            = ADI_INT_GPIOA,         /*!< GPIO Group A Interrupt                */
	GPIOB_IRQn            = ADI_INT_GPIOB,         /*!< GPIO Group B Interrupt                */
	/** Placeholder: IRQn = 49 is reserved on the ADuCM350                                     */
	AFE_CAPTURE_IRQn      = ADI_INT_AFE_CAPTURE,   /*!< AFE Analog Capture Interrupt          */
	AFE_GENERATE_IRQn     = ADI_INT_AFE_GENERATE,  /*!< AFE Analog Generation Interrupt       */
	AFE_CMD_FIFO_IRQn     = ADI_INT_AFE_CMD_FIFO,  /*!< AFE Command FIFO Interrupt            */
	AFE_DATA_FIFO_IRQn    = ADI_INT_AFE_DATA_FIFO, /*!< AFE Data FIFO Interrupt               */
	CAP_IRQn              = ADI_INT_CAP,           /*!< Cap Touch Interrupt                   */
	GP_FLASH_IRQn         = ADI_INT_GP_FLASH,      /*!< GP Flash Memory Interrupt             */
	XTAL_OSC_IRQn         = ADI_INT_XTAL_OSC,      /*!< Crystal Oscillator Interrupt          */
	PLL_IRQn              = ADI_INT_PLL,           /*!< PLL Interrupt                         */
	RAND_IRQn             = ADI_INT_RAND,          /*!< Random Bit Generator Interrupt        */
	PDI_IRQn              = ADI_INT_PDI,           /*!< Parallel Display Interface Interrupt  */
	PARITY_IRQn           = ADI_INT_PARITY,        /*!< Instruction Flash Parity failure      */
} IRQn_Type;  /* typedef name for fixed interrupt numbers */


/*!
 * \enum DMA_CHANn_TypeDef
 * DMA Channel Assignments
 */
typedef enum
{
    SPIH_TX_CHANn       =  0,  /*!< SPIH Transmit DMA channel                */
    SPIH_RX_CHANn       =  1,  /*!< SPIH Receive DMA channel                 */
    SPI0_TX_CHANn       =  2,  /*!< SPI0 Transmit DMA channel                */
    SPI0_RX_CHANn       =  3,  /*!< SPI0 Receive DMA channel                 */
    SPI1_TX_CHANn       =  4,  /*!< SPI1 Transmit DMA channel                */
    SPI1_RX_CHANn       =  5,  /*!< SPI1 Receive DMA channel                 */
    UART_TX_CHANn       =  6,  /*!< UART Transmit DMA channel                */
    UART_RX_CHANn       =  7,  /*!< UART Receive DMA channel                 */
    I2CS_TX_CHANn       =  8,  /*!< I2C Slave Transmit DMA channel           */
    I2CS_RX_CHANn       =  9,  /*!< I2C Slave Receive DMA channel            */
    I2CM_CHANn          = 10,  /*!< I2C Master DMA channel                   */
    AFE_TX_CHANn        = 11,  /*!< AFE Transmit DMA channel                 */
    AFE_RX_CHANn        = 12,  /*!< AFE Receive DMA channel                  */
    CRC_CHANn           = 13,  /*!< CRC DMA channel                          */
    PDI_CHANn           = 14,  /*!< Paralled Display Interface DMA channel   */
    I2S_CHANn           = 15,  /*!< I2S DMA channel                          */
    NUM_DMA_CHANNELSn   = 16,  /*!< Total Number of DMA channels             */
} DMA_CHANn_TypeDef;  /** typedef name for fixed DMA channel assignments */


/*!
 * Device Error Code Offsets NEEDS REVISION FOR 350
 */
#define ADI_APP_USERERROR_OFFSET           0x00000100 /*!< Generic Device API Result Error Message Offset  */
#define ADI_DEV_GPT_ERROR_OFFSET           0x00000200 /*!< Error Offset for GP Timer API                   */
#define ADI_DEV_SYS_ERROR_OFFSET           0x00000300 /*!< Error Offset for System API                     */
#define ADI_DEV_PWR_ERROR_OFFSET           0x00000400 /*!< Error Offset for Power API                      */
#define ADI_DEV_EIR_ERROR_OFFSET           0x00000500 /*!< Error Offset for External Interrupt API         */
#define ADI_DEV_RST_ERROR_OFFSET           0x00000600 /*!< Error Offset for Reset API                      */
#define ADI_DEV_LFE_ERROR_OFFSET           0x00000700 /*!< Error Offset for Low Frequency Engine API       */
#define ADI_DEV_WUT_ERROR_OFFSET           0x00000800 /*!< Error Offset for Wakeup Timer API               */
#define ADI_DEV_WDT_ERROR_OFFSET           0x00000900 /*!< Error Offset for Watchdog Timer API             */
#define ADI_DEV_FLASH_ERROR_OFFSET         0x00000A00 /*!< Error Offset for Flash/EE API                   */
#define ADI_DEV_I2C_ERROR_OFFSET           0x00000B00 /*!< Error Offset for I2C API                        */
#define ADI_DEV_SPI_ERROR_OFFSET           0x00000C00 /*!< Error Offset for SPI API                        */
#define ADI_DEV_UART_ERROR_OFFSET          0x00000D00 /*!< Error Offset for UART API                       */
#define ADI_DEV_GPIO_ERROR_OFFSET          0x00000E00 /*!< Error Offset for GPIO API                       */
#define ADI_DEV_ANALOG_ERROR_OFFSET        0x00000F00 /*!< Error Offset for Misc. Analog API               */
#define ADI_DEV_UHF_ERROR_OFFSET           0x00001000 /*!< Error Offset for UHF API                        */
#define ADI_DEV_LFT_ERROR_OFFSET           0x00001100 /*!< Error Offset for Low Frerquency Transmitter API */
#define ADI_DEV_DMA_ERROR_OFFSET           0x00001200 /*!< Error Offset for DMA API                        */
#define ADI_DEV_ADC_ERROR_OFFSET           0x00001300 /*!< Error Offset for ADC API                        */
#define ADI_DEV_PWM_ERROR_OFFSET           0x00001400 /*!< Error Offset for PWM API                        */
#define ADI_DEV_AFE_ERROR_OFFSET           0x00001500 /*!< Error Offset for AFE API                        */
#define ADI_DEV_RTC_ERROR_OFFSET           0x00001600 /*!< Error Offset for RTC API                        */
#define ADI_DEV_PDI_ERROR_OFFSET           0x00001700 /*!< Error Offset for PDI API                        */
#define ADI_DEV_I2S_ERROR_OFFSET           0x00001800 /*!< Error Offset for I2S API                        */
#define ADI_DEV_BEEP_ERROR_OFFSET          0x00001900 /*!< Error Offset for Beeper API                     */
#define ADI_DEV_CT_ERROR_OFFSET            0x00001A00 /*!< Error Offset for Captouch API                   */
#define ADI_DEV_LCD_ERROR_OFFSET           0x00001B00 /*!< Error Offset for LCD API                        */
#define ADI_DEV_LCD7006126_ERROR_OFFSET    0x00001C00 /*!< Error Offset for 7006126 LCD API                */
#define ADI_DEV_LCDVIM828_ERROR_OFFSET     0x00001D00 /*!< Error Offset for VIM-828 LCD API                */
#define ADI_DEV_RNG_ERROR_OFFSET           0x00001E00 /*!< Error Offset for RNG API                        */
#define ADI_DEV_CRC_ERROR_OFFSET           0x00001F00 /*!< Error Offset for CRC engine                     */


/******************************************************************************
 * Processor and Core Peripheral Section
 *****************************************************************************/
#define __MPU_PRESENT          1  /*!< MPU present or not                    */
#define __NVIC_PRIO_BITS       3  /*!< Number of Bits for Priority Levels    */
#define __Vendor_SysTickConfig 0  /*!< 1 if different SysTick Config is used */


/******************************************************************************
 * Includes
 *****************************************************************************/

#include <core_cm3.h>    /* Cortex-M3 processor and core peripherals*/

/* SysTick Cycle Count Macros... max 24-bit measure (no wraparound handling) */
#define CYCLES_INIT {                                                     \
    /* enable with internal clock and no interrupts */                    \
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk; \
    SysTick->LOAD = 0x00FFFFFF;}                                     /*!< SysTick instruction count macro */
#define CYCLES_CLR     {SysTick->VAL = 0;}                           /*!< SysTick instruction count macro */
#define CYCLES_GET     (0x00ffffff - 5 - SysTick->VAL)               /*!< SysTick instruction count macro */
#define CYCLES_SUSPEND {SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;}  /*!< SysTick instruction count macro */
#define CYCLES_RESUME  {SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;}   /*!< SysTick instruction count macro */

/* need bool_t ahead of system.h */
#ifndef BOOL_TYPE
#define BOOL_TYPE /*!< Create a boolean type */

/*!
 * \typedef bool_t
 * boolean type, since use of C99 "bool" is not MISRA compliant
 */
typedef uint8_t  bool_t;

/* simple true/false defines for functional APIs */
#ifndef false
#define false 0u /*!< Create a boolean 'false' value */
#endif
#ifndef true
#define true 1u /*!< Create a boolean 'true' value */
#endif

#endif /* BOOL_TYPE */

#include "system.h"      /* System Header                           */

/* MIN */
#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b)) /*!< Define a handy 'MIN' macro */
#endif

/* MAX */
#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b)) /*!< Define a handy 'MAX' macro */
#endif


/*
 * Bit-Banding Macros for atomic RAM/MMR bit operations.
 * Compute the incoming RAM/MMR address and bit position
 * bit-banded address alias and set/clear the bit.
 */
#define RAM_ADR_BASE  0x20000000                                                                                      /*!< BitBanded address translation macro */
#define RAM_BB_ALIAS  0x22000000                                                                                      /*!< BitBanded address translation macro */
#define MMR_ADR_BASE  0x40000000                                                                                      /*!< BitBanded address translation macro */
#define MMR_BB_ALIAS  0x42000000                                                                                      /*!< BitBanded address translation macro */
#define MMR_BB_SET(a,b) ( *(volatile uint32_t *)(MMR_BB_ALIAS | (((uint32_t)a - MMR_ADR_BASE) << 5) | (b << 2)) = 1 ) /*!< BitBanded address translation macro */
#define MMR_BB_CLR(a,b) ( *(volatile uint32_t *)(MMR_BB_ALIAS | (((uint32_t)a - MMR_ADR_BASE) << 5) | (b << 2)) = 0 ) /*!< BitBanded address translation macro */
#define RAM_BB_SET(a,b) ( *(volatile uint32_t *)(RAM_BB_ALIAS | (((uint32_t)a - RAM_ADR_BASE) << 5) | (b << 2)) = 1 ) /*!< BitBanded address translation macro */
#define RAM_BB_CLR(a,b) ( *(volatile uint32_t *)(RAM_BB_ALIAS | (((uint32_t)a - RAM_ADR_BASE) << 5) | (b << 2)) = 0 ) /*!< BitBanded address translation macro */

/******************************************************************************
 * ADuM350 Device Specific Peripheral registers structures
 *****************************************************************************/
#if defined ( __CC_ARM   )
#pragma anon_unions
#endif


/******************************************************************************
 *  Access Modifiers used in the device templates following:                  *
 *    __IO = read/write register                                              *
 *    __I  = read-only register                                               *
 *    __O  = write-only register                                              *
 *****************************************************************************/


/* include CODA-generated device memory maps */
#include "ADuCM350_device.h"

/* RECONCILE MINOR NAME CHANGES BETWEEN OLD-STYLE MAPPINGS AND CODA-BASED DEFINES */

#ifndef ADI_DCC_TypeDef
/* no longer defined in CODA-generated device.h file */
/*!
 * \struct ADI_DCC_TypeDef
 * DMA Channel Control MMR Access Template
 */
typedef struct
{
    __IO uint32_t DMASRCEND;      /*!< Source End Pointer                    */
    __IO uint32_t DMADSTEND;      /*!< Destination End Pointer               */
    __IO uint32_t DMACDC;         /*!< Channel Data Configuration            */
         uint32_t RESERVED;       /*!< address gap filler                    */
} ADI_DCC_TypeDef;

#define pADI_DCCPRI           ((ADI_DCC_TypeDef       *) pADI_DMA->DMAPDBPTR) /*!< Pointer to Primary DMA Descriptors' MMR data         */
#define pADI_DCCALT           ((ADI_DCC_TypeDef       *) pADI_DMA->DMAADBPTR) /*!< Pointer to Alternate DMA Descriptors' MMR data       */
#endif

/* map new GPIO pointers to old-stype names */
#ifdef ADI_ADUCRF101
#define ADI_GP0_ADDR ADI_GPIO0_BASE	      /*!< GPIO name mapping macro */
#define ADI_GP1_ADDR ADI_GPIO1_BASE	      /*!< GPIO name mapping macro */
#define ADI_GP2_ADDR ADI_GPIO2_BASE	      /*!< GPIO name mapping macro */
#define ADI_GP3_ADDR ADI_GPIO3_BASE	      /*!< GPIO name mapping macro */
#define ADI_GP4_ADDR ADI_GPIO4_BASE	      /*!< GPIO name mapping macro */
#define pADI_GP0 pADI_GPIO0        	      /*!< GPIO name mapping macro */
#define pADI_GP1 pADI_GPIO1        	      /*!< GPIO name mapping macro */
#define pADI_GP2 pADI_GPIO2        	      /*!< GPIO name mapping macro */
#define pADI_GP3 pADI_GPIO3        	      /*!< GPIO name mapping macro */
#define pADI_GP4 pADI_GPIO4        	      /*!< GPIO name mapping macro */
#endif  // ADI_ADUCRF101

#ifdef ADI_ADUCRF02
#define ADI_GP0_ADDR ADI_GPIO0_BASE	     /*!< GPIO name mapping macro */
#define ADI_GP1_ADDR ADI_GPIO1_BASE	     /*!< GPIO name mapping macro */
#define ADI_GP2_ADDR ADI_GPIO2_BASE	     /*!< GPIO name mapping macro */
#define ADI_GP3_ADDR ADI_GPIO3_BASE	     /*!< GPIO name mapping macro */
#define pADI_GP0 pADI_GPIO0			     /*!< GPIO name mapping macro */
#define pADI_GP1 pADI_GPIO1			     /*!< GPIO name mapping macro */
#define pADI_GP2 pADI_GPIO2			     /*!< GPIO name mapping macro */
#define pADI_GP3 pADI_GPIO3			     /*!< GPIO name mapping macro */
#endif  // ADI_ADUCRF02

#ifdef ADI_ADUCM350
#define ADI_GP0_ADDR ADI_GPIO0_BASE	      /*!< GPIO name mapping macro */
#define ADI_GP1_ADDR ADI_GPIO1_BASE	      /*!< GPIO name mapping macro */
#define ADI_GP2_ADDR ADI_GPIO2_BASE	      /*!< GPIO name mapping macro */
#define ADI_GP3_ADDR ADI_GPIO3_BASE	      /*!< GPIO name mapping macro */
#define ADI_GP4_ADDR ADI_GPIO4_BASE	      /*!< GPIO name mapping macro */
#define pADI_GP0 pADI_GPIO0			      /*!< GPIO name mapping macro */
#define pADI_GP1 pADI_GPIO1			      /*!< GPIO name mapping macro */
#define pADI_GP2 pADI_GPIO2			      /*!< GPIO name mapping macro */
#define pADI_GP3 pADI_GPIO3			      /*!< GPIO name mapping macro */
#define pADI_GP4 pADI_GPIO4			      /*!< GPIO name mapping macro */
#define ADI_GPT_0_CAPTURE_EVENT_WDTIMER ADI_GPT_0_CAPTURE_EVENT_WATCHDOG_TIMER  /*!< GPT even capture mapping macro */
#endif

/* IDU registers now part of PWR module */
#define pADI_IDU pADI_PWR                 /*!< PWR name mapping macro */

/* gp timer typedef name */
#define ADI_TIMER_TypeDef ADI_GPT_TypeDef /*!< GPT name mapping macro */
#define pADI_TM0 pADI_GPT0                /*!< GPT name mapping macro */
#define pADI_TM1 pADI_GPT1                /*!< GPT name mapping macro */
#define pADI_TM2 pADI_GPT2                /*!< GPT name mapping macro */

/* C++ linkage */
#ifdef __cplusplus
}
#endif

#endif /* include guard */

/*
** EOF
*/

/*@}*/
