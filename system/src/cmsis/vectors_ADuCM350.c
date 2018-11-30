/*********************************************************************************

Copyright (c) 2010-2014 Analog Devices, Inc. All Rights Reserved.

This software is proprietary to Analog Devices, Inc. and its licensors.  By using 
this software you agree to the terms of the associated Analog Devices Software 
License Agreement (See /LICENSES/ADI.LABLAB.LICENSE). 

Modified from the Analog Devices original.
Portions Copyright (c) 2018 Andrew Grosser, Moritz von Buttlar.

*********************************************************************************/

/*!
 *****************************************************************************
 * @file:    startup.c
 * @brief:   Interrupt table and default handlers for ADuCM350
 * @version: $Revision: 30127 $
 * @date:    $Date: 2015-03-09 12:05:31 -0400 (Mon, 09 Mar 2015) $
 *****************************************************************************/
#include "newlib/sys/reent.h"
#include "startup.h"

#include "cortexm/ExceptionHandlers.h"

//#ifdef _USING_UCOS_II_
//#include <os_cpu.h>
//#endif

/*
    Stack size considerations for RTOS environments:

    In the Cortex-M class processors, the hardware has two stacks: the main stack
    and the process stack.  The main stack is used at reset and by system exception
    handlers.  In simple applications (non-RTOS) you may only use this stack.  In
    more complex systems that use an RTOS you have a split between the main stack
    which is used by the kernel and exception handlers and the process stack which
    is used by the tasks and programmable interrupts (interrupts stack on top of
    the currently active task, so tasks stacks should be allocated for worst-case
    interrupt scenarios).  In the uCOS case, the process stack is the one that gets
    context-switched and used by the tasks, where we store the registers, etc.

    As RTOS tasks specify their own stack area, a minimal size of 4 for the process
    stack in order to retain symbols, etc., seems OK and we would not need any more.
    The main stack needs to be big enough to initialize the RTOS, run interrupts etc.

    As always, stack and heap size optimizations is a balancing act between usage
    and available memory.  It is entirely application-dependant.

*/

//*****************************************************************************
//
// The minimal vector table for a Cortex M3.  Note that the proper constructs
// must be placed on this to ensure that it ends up at physical address
// 0x0000.0000.
//
//*****************************************************************************

//****************************************************************************************************************
//
// When operating in an RTOS context, interrupts are dispatched by the NVIC controller
// to a second level dispatch function known as the OSAL (operating system abstraction layer) dispatch.
// The OSAL dispatch has common code that will interact with the RTOS.
//
// OSAL dispatched interrupts must pass their Interrupt ID (iid) to the common OSAL dispatcher.
//
// System interrupts (Reset through SysTick) should never dispatch through the OSAL.
//
// The IntFunc typedef is used for System Interrupts handlers. The parameter list for these interrupts is always 'void'.
//
// The ADIIntFunc typdef is used for Driver Interrupt Handlers. The parameter list for these handlers
// will change depending on the RTOS context. In an RTOS context the IID (Interrupt ID) must be passed to the OSAL
// dispatcher (along with a second arg of type void * which is handler specific). In a non-RTOS context the handler
// parameters is simply 'void'.
//
//*****************************************************************************************************************

// IVT typedefs
typedef void (*IntFunc)(void);
#if (ADI_CFG_ENABLE_RTOS_SUPPORT == 1)
typedef void (*ADIIntFunc)(uint32_t, void *);
#else
typedef void (*ADIIntFunc)(void);
#endif

// added by Moritz:
extern unsigned int _estack;

typedef void (*const pHandler)(void);

void __attribute__((weak, alias("Default_Handler")))
DeviceInterrupt_Handler(void);

typedef union {
    ADIIntFunc __fun;
    void *__ptr;
    IntFunc __ifunc;
} IntVector;

// Interrupt Vector Table
//#pragma segment="CSTACK"
//#pragma location = ".intvec"
__attribute__((section(".isr_vector"), used))
const IntVector __vector_table[] =
    {
        // grab stack pointer "end" address from ICF file
        // { .__ptr = __sfe( "CSTACK" ) },
        {
            (pHandler)&_estack,
        },
        // exception mappings...
        {.__ifunc = ResetISR},           // -15
        {.__ifunc = NmiSR},              // -14
        {.__ifunc = HardFault_Handler},  // -13
        {.__ifunc = MemManage_Handler},  // -12
        {.__ifunc = BusFault_Handler},   // -11
        {.__ifunc = UsageFault_Handler}, // -10
        {.__ifunc = RESERVED_VECTOR},    // -09
        {.__ifunc = RESERVED_VECTOR},    // -08
        {.__ifunc = RESERVED_VECTOR},    // -07
        {.__ifunc = RESERVED_VECTOR},    // -06
        {.__ifunc = SVC_Handler},        // -05
        {.__ifunc = DebugMon_Handler},   // -04
        {.__ifunc = RESERVED_VECTOR},    // -03
        {.__ifunc = PendSV_Handler},     // -02  (Micrium overrides weak binding if using uCOS)
        {.__ifunc = SysTick_Handler},    // -01  (Micrium overrides weak binding if using uCOS)

        // programmable interrupt mappings...
        {WakeUp_Int_Handler},         // 0
        {Ext_Int0_Handler},           // 1
        {Ext_Int1_Handler},           // 2
        {Ext_Int2_Handler},           // 3
        {Ext_Int3_Handler},           // 4
        {Ext_Int4_Handler},           // 5
        {Ext_Int5_Handler},           // 6
        {Ext_Int6_Handler},           // 7
        {Ext_Int7_Handler},           // 8
        {Ext_Int8_Handler},           // 9
        {WDog_Tmr_Int_Handler},       // 10
        {GP_Tmr0_Int_Handler},        // 11
        {GP_Tmr1_Int_Handler},        // 12
        {Flash0_Int_Handler},         // 13
        {UART_Int_Handler},           // 14
        {SPI0_Int_Handler},           // 15
        {SPIH_Int_Handler},           // 16
        {I2C0_Slave_Int_Handler},     // 17
        {I2C0_Master_Int_Handler},    // 18
        {DMA_Err_Int_Handler},        // 19
        {DMA_SPIH_TX_Int_Handler},    // 20
        {DMA_SPIH_RX_Int_Handler},    // 21
        {DMA_SPI0_TX_Int_Handler},    // 22
        {DMA_SPI0_RX_Int_Handler},    // 23
        {DMA_SPI1_TX_Int_Handler},    // 24
        {DMA_SPI1_RX_Int_Handler},    // 25
        {DMA_UART_TX_Int_Handler},    // 26
        {DMA_UART_RX_Int_Handler},    // 27
        {DMA_I2C0_STX_Int_Handler},   // 28
        {DMA_I2C0_SRX_Int_Handler},   // 29
        {DMA_I2C0_MX_Int_Handler},    // 30
        {DMA_AFE_TX_Int_Handler},     // 31
        {DMA_AFE_RX_Int_Handler},     // 32
        {DMA_CRC_Int_Handler},        // 33
        {DMA_PDI_Int_Handler},        // 34
        {DMA_I2S_Int_Handler},        // 35
        {USB_Wakeup_Int_Handler},     // 36
        {USB_Control_Int_Handler},    // 37
        {USB_DMA_Int_Handler},        // 38
        {I2S_Int_Handler},            // 39
        {GP_Tmr2_Int_Handler},        // 40
        {Flash1_Int_Handler},         // 41
        {SPI1_Int_Handler},           // 42
        {RTC_Int_Handler},            // 43
        {.__ifunc = RESERVED_VECTOR}, // 44
        {Beep_Int_Handler},           // 45
        {LCD_Int_Handler},            // 46
        {GPIO_A_Int_Handler},         // 47
        {GPIO_B_Int_Handler},         // 48
        {.__ifunc = RESERVED_VECTOR}, // 49
        {AFE_Capture_Int_Handler},    // 50
        {AFE_Generate_Int_Handler},   // 51
        {AFE_CmdFIFO_Int_Handler},    // 52
        {AFE_DataFIFO_Int_Handler},   // 53
        {CapTouch_Int_Handler},       // 54
        {GPFlash_Int_Handler},        // 55
        {XTAL_Int_Handler},           // 56
        {PLL_Int_Handler},            // 57
        {Random_Int_Handler},         // 58
        {PDI_Int_Handler},            // 59
        {Parity_Int_Handler},         // 60
        DeviceInterrupt_Handler,      // Device specific
};

void __attribute__((section(".after_vectors")))
Default_Handler(void)
{
    while (1)
    {
    }
}

//*****************************************************************************
//
// Application Entry point
//
//*****************************************************************************
extern int main(void);

//*****************************************************************************
// Symbols defined by the GNU linker.
//*****************************************************************************
#ifdef __GNUC__
extern unsigned long _etext;
extern unsigned long _sdata;
extern unsigned long _edata;
extern unsigned long _sbss;
extern unsigned long _ebss;
#endif

//*****************************************************************************
// IAR system startup functions.
//*****************************************************************************
#ifdef __ICCARM__
WEAK_PROTO(void __iar_init_core(void));
WEAK_PROTO(void __iar_init_vfp(void));
extern void __cmain();
#endif

//*****************************************************************************
// Function    : ResetISR (-15)
// Description : Reset event handler
//*****************************************************************************

WEAK_FUNC(void ResetISR(void))
{

    /* reset the main SP to clean up any leftovers from the boot rom. */
    __set_MSP((uint32_t)__vector_table[0].__ptr);

    /* Unlock the PWRMOD register by writing the two keys to the PWRKEY register */
    pADI_PWR->PWRKEY = PWRKEY_VALUE_KEY1;
    pADI_PWR->PWRKEY = PWRKEY_VALUE_KEY2;

    /* set the RAM0_RET bit so the entire 16k of SRAM Bank0 is hibernate-preserved */
    pADI_PWR->PWRMOD |= (1 << BITP_PWR_PWRMOD_RAM0_RET);

#ifdef RELOCATE_IVT
    int i;
    uint8_t *pSrc, *pDst;

    /* copy the IVT (avoid use of memcpy here so it does not become locked into flash) */
    for (i = 0, pSrc = (uint8_t *)__vector_table, pDst = (uint8_t *)__relocated_vector_table; i < SIZEOF_IVT; i++)
        *pDst++ = *pSrc++;
#endif

#ifdef __GNUC__
//AG
#if defined(NDEBUG)
    unsigned long *pulSrc, *pulDest;

    // Copy initialised data from flash into RAM
    pulSrc = &_etext;
    for (pulDest = &_sdata; pulDest < &_edata;)
    {
        *pulDest++ = *pulSrc++;
    }

    // Clear the bss segment
    for (pulDest = &_sbss; pulDest < &_ebss;)
    {
        *pulDest++ = 0;
    }
#endif

    // Call application main directly.
    main();

#elif __ICCARM__

    // Call IAR system startup
    __iar_init_core();
    __iar_init_vfp();
    __cmain();

#else

    // Call application main directly.
    main();

#endif

    // Stick here if main returns
    while (1)
        ;
}

//*****************************************************************************
// Provide default (weakly bound) trapping handlers to be overridden as needed
//*****************************************************************************
WEAK_FUNCTION(NmiSR)
WEAK_FUNCTION(HardFault_Handler)
WEAK_FUNCTION(MemManage_Handler)
WEAK_FUNCTION(BusFault_Handler)
WEAK_FUNCTION(UsageFault_Handler)
WEAK_FUNCTION(SVC_Handler)
WEAK_FUNCTION(DebugMon_Handler)
WEAK_FUNCTION(PendSV_Handler)
WEAK_FUNCTION(SysTick_Handler)
ADI_WEAK_FUNCTION(WakeUp_Int_Handler)
ADI_WEAK_FUNCTION(Ext_Int0_Handler)
ADI_WEAK_FUNCTION(Ext_Int1_Handler)
ADI_WEAK_FUNCTION(Ext_Int2_Handler)
ADI_WEAK_FUNCTION(Ext_Int3_Handler)
ADI_WEAK_FUNCTION(Ext_Int4_Handler)
ADI_WEAK_FUNCTION(Ext_Int5_Handler)
ADI_WEAK_FUNCTION(Ext_Int6_Handler)
ADI_WEAK_FUNCTION(Ext_Int7_Handler)
ADI_WEAK_FUNCTION(Ext_Int8_Handler)
ADI_WEAK_FUNCTION(WDog_Tmr_Int_Handler)
ADI_WEAK_FUNCTION(GP_Tmr0_Int_Handler)
ADI_WEAK_FUNCTION(GP_Tmr1_Int_Handler)
ADI_WEAK_FUNCTION(Flash0_Int_Handler)
ADI_WEAK_FUNCTION(UART_Int_Handler)
ADI_WEAK_FUNCTION(SPI0_Int_Handler)
ADI_WEAK_FUNCTION(SPIH_Int_Handler)
ADI_WEAK_FUNCTION(I2C0_Slave_Int_Handler)
ADI_WEAK_FUNCTION(I2C0_Master_Int_Handler)
ADI_WEAK_FUNCTION(DMA_Err_Int_Handler)
ADI_WEAK_FUNCTION(DMA_SPIH_TX_Int_Handler)
ADI_WEAK_FUNCTION(DMA_SPIH_RX_Int_Handler)
ADI_WEAK_FUNCTION(DMA_SPI0_TX_Int_Handler)
ADI_WEAK_FUNCTION(DMA_SPI0_RX_Int_Handler)
ADI_WEAK_FUNCTION(DMA_SPI1_TX_Int_Handler)
ADI_WEAK_FUNCTION(DMA_SPI1_RX_Int_Handler)
ADI_WEAK_FUNCTION(DMA_UART_TX_Int_Handler)
ADI_WEAK_FUNCTION(DMA_UART_RX_Int_Handler)
ADI_WEAK_FUNCTION(DMA_I2C0_STX_Int_Handler)
ADI_WEAK_FUNCTION(DMA_I2C0_SRX_Int_Handler)
ADI_WEAK_FUNCTION(DMA_I2C0_MX_Int_Handler)
ADI_WEAK_FUNCTION(DMA_AFE_TX_Int_Handler)
ADI_WEAK_FUNCTION(DMA_AFE_RX_Int_Handler)
ADI_WEAK_FUNCTION(DMA_CRC_Int_Handler)
ADI_WEAK_FUNCTION(DMA_PDI_Int_Handler)
ADI_WEAK_FUNCTION(DMA_I2S_Int_Handler)
ADI_WEAK_FUNCTION(USB_Wakeup_Int_Handler)
ADI_WEAK_FUNCTION(USB_Control_Int_Handler)
ADI_WEAK_FUNCTION(USB_DMA_Int_Handler)
ADI_WEAK_FUNCTION(I2S_Int_Handler)
ADI_WEAK_FUNCTION(GP_Tmr2_Int_Handler)
ADI_WEAK_FUNCTION(Flash1_Int_Handler)
ADI_WEAK_FUNCTION(SPI1_Int_Handler)
ADI_WEAK_FUNCTION(RTC_Int_Handler)
/** Placeholder: IRQn = 44 is reserved on the ADuM350                                    */
ADI_WEAK_FUNCTION(Beep_Int_Handler)
ADI_WEAK_FUNCTION(LCD_Int_Handler)
ADI_WEAK_FUNCTION(GPIO_A_Int_Handler)
ADI_WEAK_FUNCTION(GPIO_B_Int_Handler)
/** Placeholder: IRQn = 49 is reserved on the ADuM350                                    */
ADI_WEAK_FUNCTION(AFE_Capture_Int_Handler)
ADI_WEAK_FUNCTION(AFE_Generate_Int_Handler)
ADI_WEAK_FUNCTION(AFE_CmdFIFO_Int_Handler)
ADI_WEAK_FUNCTION(AFE_DataFIFO_Int_Handler)
ADI_WEAK_FUNCTION(CapTouch_Int_Handler)
ADI_WEAK_FUNCTION(GPFlash_Int_Handler)
ADI_WEAK_FUNCTION(XTAL_Int_Handler)
ADI_WEAK_FUNCTION(PLL_Int_Handler)
ADI_WEAK_FUNCTION(Random_Int_Handler)
ADI_WEAK_FUNCTION(PDI_Int_Handler)
ADI_WEAK_FUNCTION(Parity_Int_Handler)
WEAK_FUNCTION(RESERVED_VECTOR)
