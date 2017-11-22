/*********************************************************************************

Copyright (c) 2010-2014 Analog Devices, Inc. All Rights Reserved.

This software is proprietary to Analog Devices, Inc. and its licensors.  By using 
this software you agree to the terms of the associated Analog Devices Software 
License Agreement.

*********************************************************************************/

/*!
 *****************************************************************************
   @file:    gpio.h
   @brief:   GPIO definitions and API for ADuCxxx
   @version: $Revision: 28544 $
   @date:    $Date: 2014-11-13 13:09:44 -0500 (Thu, 13 Nov 2014) $
*****************************************************************************/

/*! \addtogroup GPIO_Driver GPIO Driver
 *  GPIO device driver
 *  @{

  @brief    GPIO port and pin identifiers
  @details  These macros can be passed to the following functions:

                  - adi_GPIO_SetOutputEnable()
                  - adi_GPIO_GetOutputEnable()
                  - adi_GPIO_SetPullUpEnable()
                  - adi_GPIO_GetPullUpEnable()
                  - adi_GPIO_SetOpenCircuitEnable()
                  - adi_GPIO_GetOpenCircuitEnable()
                  - adi_GPIO_SetHigh()
                  - adi_GPIO_SetLow()
                  - adi_GPIO_Toggle()
                  - adi_GPIO_SetData()
                  - adi_GPIO_GetData()

            To control a single GPIO, these macros can be passed to the functions one
            at a time. For example, to set the GPIO on port 2, pin 4 to a logical high
            level, the following is used:

                 adi_GPIO_SetHigh(ADI_GPIO_PORT_2, ADI_GPIO_PIN_4);

            Multiple GPIOs, so long as they reside on the same port, can be controlled
            simultaneously. These macros can be OR-ed together and passed to the
            functions. For example, to set the GPIOs on port 2, pins 3, 4 and 7 to
            a logical low level, the following is used:

                 adi_GPIO_SetLow(ADI_GPIO_PORT_2, ADI_GPIO_PIN_3 | ADI_GPIO_PIN_4 | ADI_GPIO_PIN_7);

            For the sensing, or adi_GPIO_Getxxx, functions, the return value is a packed
            value containing the status of all the GPIOs on the given port. To test the
            level of a specific GPIO, simply perform a logical AND of the return value with
            the GPIO pin definition. For example to see if pin 4 on port 2 has the pull up
            enabled, the following is used:

            if   ((adi_GPIO_GetPullUpEnable(ADI_GPIO_PORT_2)) & ADU_GPIO_PIN_4) {
                the pull up is enabled for pin 4 on port 2
            } else {
                the pull up is disabled for pin 4 on port 2
            }

 */

#ifndef __GPIO_H__
#define __GPIO_H__

#include "device.h"
#include "adi_int.h"

/* C++ linkage */
#ifdef __cplusplus
extern "C" {
#endif

/*!< Setup for 8-bit vs. 16-bit GPIO Port Width */
/* so far, ADuCM350 is the only device with 16-bit GPIO ports */
#define ADI_GPIO_PORT_WIDTH_16_BIT

/* typedefs for 8-bit Ports */
#ifdef ADI_GPIO_PORT_WIDTH_8_BIT
typedef uint32_t ADI_GPIO_PORT_TYPE;    /*!< port address type */
typedef uint32_t ADI_GPIO_MUX_TYPE;     /*!< port mask/mux encoding type */
typedef uint8_t  ADI_GPIO_DATA_TYPE;    /*!< pin data reg type */
typedef uint16_t ADI_GPIO_CONFIG_TYPE;  /*!< pin config reg type */
#endif

/* typedefs for 16-bit Ports */
#ifdef ADI_GPIO_PORT_WIDTH_16_BIT
typedef uint32_t ADI_GPIO_PORT_TYPE;    /*!< port address type */
typedef uint64_t ADI_GPIO_MUX_TYPE;     /*!< port mask/mux encoding type */
typedef uint16_t ADI_GPIO_DATA_TYPE;    /*!< pin data reg type */
typedef uint32_t ADI_GPIO_CONFIG_TYPE;  /*!< pin config reg type */
#endif

#define ADI_GPIO_MASK_SHIFT (4*sizeof(ADI_GPIO_MUX_TYPE))  /*!< How far up to shift the mask bits in the mask/mux encodings */

/*! \enum ADI_GPIO_RESULT_TYPE GPIO API function return codes */
typedef enum
{
    ADI_GPIO_SUCCESS = 0,                                           /*!< No error detected. */
    ADI_GPIO_ERR_UNKNOWN_ERROR = ADI_DEV_GPIO_ERROR_OFFSET,         /*!< Generic unknown error occurred. */
    ADI_GPIO_ERR_ALREADY_INITIALIZED,                               /*!< GPIO device has already been initialized. */
    ADI_GPIO_ERR_NOT_INITIALIZED,                                   /*!< GPIO device has not yet been initialized. */
    ADI_GPIO_ERR_INVALID_PORT,                                      /*!< Invalid port number. */
    ADI_GPIO_ERR_INVALID_MUX_REQUEST,                               /*!< Invalid muxing combination. */
    ADI_GPIO_ERR_INVALID_PINS,                                      /*!< Invalid pin combination. */
    ADI_GPIO_ERR_INVALID_INTERRUPT,                                 /*!< Invalid interrupt number. */
    ADI_GPIO_ERR_INVALID_TRIGGER,                                   /*!< Invalid trigger condition. */
} ADI_GPIO_RESULT_TYPE;


/*! \enum ADI_GPIO_IRQ_TRIGGER_CONDITION_TYPE GPIO trigger condition enumerations */
typedef enum {
    ADI_GPIO_IRQ_RISING_EDGE    =(0x0),     /*!< Trigger an interrupt on a rising edge.    */
    ADI_GPIO_IRQ_FALLING_EDGE   =(0x1),     /*!< Trigger an interrupt on a falling edge.   */
    ADI_GPIO_IRQ_EITHER_EDGE    =(0x2),     /*!< Trigger an interrupt on either edge.    */
    ADI_GPIO_IRQ_HIGH_LEVEL     =(0x3),     /*!< Trigger an interrupt on a high level.     */
    ADI_GPIO_IRQ_LOW_LEVEL      =(0x4)      /*!< Trigger an interrupt on a low level.      */
} ADI_GPIO_IRQ_TRIGGER_CONDITION_TYPE;



#define ADI_GPIO_PORT_0 (ADI_GP0_ADDR)      /*!< Port 0 */
#define ADI_GPIO_PORT_1 (ADI_GP1_ADDR)      /*!< Port 1 */
#define ADI_GPIO_PORT_2 (ADI_GP2_ADDR)      /*!< Port 2 */

#define ADI_GPIO_PORT_3 (ADI_GP3_ADDR)      /*!< Port 3 */
#define ADI_GPIO_PORT_4 (ADI_GP4_ADDR)      /*!< Port 4 */

/* 8-bit port pin defs */
#define ADI_GPIO_PIN_0      ((ADI_GPIO_DATA_TYPE)(0x0001)) /*!< Pin 0 */
#define ADI_GPIO_PIN_1      ((ADI_GPIO_DATA_TYPE)(0x0002)) /*!< Pin 1 */
#define ADI_GPIO_PIN_2      ((ADI_GPIO_DATA_TYPE)(0x0004)) /*!< Pin 2 */
#define ADI_GPIO_PIN_3      ((ADI_GPIO_DATA_TYPE)(0x0008)) /*!< Pin 3 */
#define ADI_GPIO_PIN_4      ((ADI_GPIO_DATA_TYPE)(0x0010)) /*!< Pin 4 */
#define ADI_GPIO_PIN_5      ((ADI_GPIO_DATA_TYPE)(0x0020)) /*!< Pin 5 */
#define ADI_GPIO_PIN_6      ((ADI_GPIO_DATA_TYPE)(0x0040)) /*!< Pin 6 */
#define ADI_GPIO_PIN_7      ((ADI_GPIO_DATA_TYPE)(0x0080)) /*!< Pin 7 */

/* extended pin defs for 16-bit ports */
#ifdef ADI_GPIO_PORT_WIDTH_16_BIT
#define ADI_GPIO_PIN_8      ((ADI_GPIO_DATA_TYPE)(0x0100)) /*!< Pin 8 */
#define ADI_GPIO_PIN_9      ((ADI_GPIO_DATA_TYPE)(0x0200)) /*!< Pin 9 */
#define ADI_GPIO_PIN_10     ((ADI_GPIO_DATA_TYPE)(0x0400)) /*!< Pin 10 */
#define ADI_GPIO_PIN_11     ((ADI_GPIO_DATA_TYPE)(0x0800)) /*!< Pin 11 */
#define ADI_GPIO_PIN_12     ((ADI_GPIO_DATA_TYPE)(0x1000)) /*!< Pin 12 */
#define ADI_GPIO_PIN_13     ((ADI_GPIO_DATA_TYPE)(0x2000)) /*!< Pin 13 */
#define ADI_GPIO_PIN_14     ((ADI_GPIO_DATA_TYPE)(0x4000)) /*!< Pin 14 */
#define ADI_GPIO_PIN_15     ((ADI_GPIO_DATA_TYPE)(0x8000)) /*!< Pin 15 */
#endif

// begin 'M350 port mappings...  (four ports)

/*
    Note, ADuCM350 encodings went from 32-bit to 64-bit because the port width went from 8-bit to 16-bit.
    Each pin encoding now holds a 32-bit mask and a 32-bit mux value (2-bits per pin * 16 pins per port).
*/

/* Port 0 */
/* P0.0 */
#define ADI_GPIO_P00            (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN0_CFG << ADI_GPIO_MASK_SHIFT)  | (0 << BITP_GPIO_GPCON_PIN0_CFG)) /*!< Encoding for GPIO.              */
#define ADI_GPIO_CTA            (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN0_CFG << ADI_GPIO_MASK_SHIFT)  | (1 << BITP_GPIO_GPCON_PIN0_CFG)) /*!< Encoding for CapTouchA.         */

/* P0.1 */
#define ADI_GPIO_P01            (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN1_CFG << ADI_GPIO_MASK_SHIFT)  | (0 << BITP_GPIO_GPCON_PIN1_CFG)) /*!< Encoding for GPIO.              */
#define ADI_GPIO_CTB            (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN1_CFG << ADI_GPIO_MASK_SHIFT)  | (1 << BITP_GPIO_GPCON_PIN1_CFG)) /*!< Encoding for CapTouchB.         */

/* P0.2 */
#define ADI_GPIO_P02            (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN2_CFG << ADI_GPIO_MASK_SHIFT)  | (0 << BITP_GPIO_GPCON_PIN2_CFG)) /*!< Encoding for GPIO.              */
#define ADI_GPIO_CTC            (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN2_CFG << ADI_GPIO_MASK_SHIFT)  | (1 << BITP_GPIO_GPCON_PIN2_CFG)) /*!< Encoding for CapTouchC.         */

/* P0.3 */
#define ADI_GPIO_P03            (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN3_CFG << ADI_GPIO_MASK_SHIFT)  | (0 << BITP_GPIO_GPCON_PIN3_CFG)) /*!< Encoding for GPIO.              */
#define ADI_GPIO_CTD            (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN3_CFG << ADI_GPIO_MASK_SHIFT)  | (1 << BITP_GPIO_GPCON_PIN3_CFG)) /*!< Encoding for CapTouchD.         */

/* P0.4 */
#define ADI_GPIO_P04            (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN4_CFG << ADI_GPIO_MASK_SHIFT)  | (0 << BITP_GPIO_GPCON_PIN4_CFG)) /*!< Encoding for GPIO.              */
#define ADI_GPIO_CTE            (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN4_CFG << ADI_GPIO_MASK_SHIFT)  | (1 << BITP_GPIO_GPCON_PIN4_CFG)) /*!< Encoding for CapTouchE.         */

/* P0.5 */
#define ADI_GPIO_P05            (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN5_CFG << ADI_GPIO_MASK_SHIFT)  | (0 << BITP_GPIO_GPCON_PIN5_CFG)) /*!< Encoding for GPIO.              */
#define ADI_GPIO_CTF            (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN5_CFG << ADI_GPIO_MASK_SHIFT)  | (1 << BITP_GPIO_GPCON_PIN5_CFG)) /*!< Encoding for CapTouchF.         */

/* P0.6 */
#define ADI_GPIO_JTAGSDO        (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN6_CFG << ADI_GPIO_MASK_SHIFT)  | (0 << BITP_GPIO_GPCON_PIN6_CFG)) /*!< Encoding for JTAG SDO.          */
#define ADI_GPIO_P06            (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN6_CFG << ADI_GPIO_MASK_SHIFT)  | (1 << BITP_GPIO_GPCON_PIN6_CFG)) /*!< Encoding for GPIO.              */
#define ADI_GPIO_UART0TXD       (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN6_CFG << ADI_GPIO_MASK_SHIFT)  | (2 << BITP_GPIO_GPCON_PIN6_CFG)) /*!< Encoding for UART TX.           */

/* P0.7 */
#define ADI_GPIO_JTAGSDI        (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN7_CFG << ADI_GPIO_MASK_SHIFT)  | (0 << BITP_GPIO_GPCON_PIN7_CFG)) /*!< Encoding for JTAG SDI.          */
#define ADI_GPIO_P07            (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN7_CFG << ADI_GPIO_MASK_SHIFT)  | (1 << BITP_GPIO_GPCON_PIN7_CFG)) /*!< Encoding for GPIO.              */
#define ADI_GPIO_UART0RXD       (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN7_CFG << ADI_GPIO_MASK_SHIFT)  | (2 << BITP_GPIO_GPCON_PIN7_CFG)) /*!< Encoding for UART RX.           */

/* P0.8 */
#define ADI_GPIO_JTAGTMOD       (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN8_CFG << ADI_GPIO_MASK_SHIFT)  | (0 << BITP_GPIO_GPCON_PIN8_CFG)) /*!< Encoding for JTAG Test Mode.    */
#define ADI_GPIO_SWDATA         (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN8_CFG << ADI_GPIO_MASK_SHIFT)  | (0 << BITP_GPIO_GPCON_PIN8_CFG)) /*!< Encoding for Serial-Wire Data.  */
#define ADI_GPIO_P08            (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN8_CFG << ADI_GPIO_MASK_SHIFT)  | (1 << BITP_GPIO_GPCON_PIN8_CFG)) /*!< Encoding for GPIO.              */

/* P0.9 */
#define ADI_GPIO_JTAGTCLK       (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN9_CFG << ADI_GPIO_MASK_SHIFT)  | (0 << BITP_GPIO_GPCON_PIN9_CFG)) /*!< Encoding for JTAG Test Clock.   */
#define ADI_GPIO_SWCLK          (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN9_CFG << ADI_GPIO_MASK_SHIFT)  | (0 << BITP_GPIO_GPCON_PIN9_CFG)) /*!< Encoding for Serial-Wire Clock. */
#define ADI_GPIO_P09            (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN9_CFG << ADI_GPIO_MASK_SHIFT)  | (1 << BITP_GPIO_GPCON_PIN9_CFG)) /*!< Encoding for GPIO.              */

/* P0.10 */
#define ADI_GPIO_P010           (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN10_CFG << ADI_GPIO_MASK_SHIFT) | (0 << BITP_GPIO_GPCON_PIN10_CFG)) /*!< Encoding for GPIO.             */
#define ADI_GPIO_PWMC           (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN10_CFG << ADI_GPIO_MASK_SHIFT) | (1 << BITP_GPIO_GPCON_PIN10_CFG)) /*!< Encoding for PWM TimerC.       */

/* P0.11 */
#define ADI_GPIO_P011           (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN11_CFG << ADI_GPIO_MASK_SHIFT) | (0 << BITP_GPIO_GPCON_PIN11_CFG)) /*!< Encoding for GPIO.             */

/* P0.12 */
#define ADI_GPIO_P012           (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN12_CFG << ADI_GPIO_MASK_SHIFT) | (0 << BITP_GPIO_GPCON_PIN12_CFG)) /*!< Encoding for GPIO.             */
#define ADI_GPIO_SPIHSCLK       (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN12_CFG << ADI_GPIO_MASK_SHIFT) | (1 << BITP_GPIO_GPCON_PIN12_CFG)) /*!< Encoding for SPIH SCLK.        */

/* P0.13 */
#define ADI_GPIO_P013           (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN13_CFG << ADI_GPIO_MASK_SHIFT) | (0 << BITP_GPIO_GPCON_PIN13_CFG)) /*!< Encoding for GPIO.             */
#define ADI_GPIO_SPIHMISO       (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN13_CFG << ADI_GPIO_MASK_SHIFT) | (1 << BITP_GPIO_GPCON_PIN13_CFG)) /*!< Encoding for SPIH MISO.        */

/* P0.14 */
#define ADI_GPIO_P014           (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN14_CFG << ADI_GPIO_MASK_SHIFT) | (0 << BITP_GPIO_GPCON_PIN14_CFG)) /*!< Encoding for GPIO.             */
#define ADI_GPIO_SPIHMOSI       (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN14_CFG << ADI_GPIO_MASK_SHIFT) | (1 << BITP_GPIO_GPCON_PIN14_CFG)) /*!< Encoding for SPIH MOSI.        */

/* P0.15 */
#define ADI_GPIO_P015           (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN15_CFG << ADI_GPIO_MASK_SHIFT) | (0 << BITP_GPIO_GPCON_PIN15_CFG)) /*!< Encoding for GPIO.             */
#define ADI_GPIO_SPIHCS         (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN15_CFG << ADI_GPIO_MASK_SHIFT) | (1 << BITP_GPIO_GPCON_PIN15_CFG)) /*!< Encoding for SPIH CS.          */


/* Port 1 */
/* P1.0 */
#define ADI_GPIO_P10            (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN0_CFG << ADI_GPIO_MASK_SHIFT)  | (0 << BITP_GPIO_GPCON_PIN0_CFG)) /*!< Encoding for GPIO.              */
#define ADI_GPIO_LCDS3          (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN0_CFG << ADI_GPIO_MASK_SHIFT)  | (1 << BITP_GPIO_GPCON_PIN0_CFG)) /*!< Encoding for LCD Seg3.          */
#define ADI_GPIO_PDID0          (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN0_CFG << ADI_GPIO_MASK_SHIFT)  | (2 << BITP_GPIO_GPCON_PIN0_CFG)) /*!< Encoding for PDI D0.            */
#define ADI_GPIO_PDISCLK        (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN0_CFG << ADI_GPIO_MASK_SHIFT)  | (3 << BITP_GPIO_GPCON_PIN0_CFG)) /*!< Encoding for PDI SCLK.         */

/* P1.1 */
#define ADI_GPIO_P11            (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN1_CFG << ADI_GPIO_MASK_SHIFT)  | (0 << BITP_GPIO_GPCON_PIN1_CFG)) /*!< Encoding for GPIO.              */
#define ADI_GPIO_LCDS4          (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN1_CFG << ADI_GPIO_MASK_SHIFT)  | (1 << BITP_GPIO_GPCON_PIN1_CFG)) /*!< Encoding for LCD Seg4.          */
#define ADI_GPIO_PDID1          (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN1_CFG << ADI_GPIO_MASK_SHIFT)  | (2 << BITP_GPIO_GPCON_PIN1_CFG)) /*!< Encoding for PDI D1.            */
#define ADI_GPIO_PDISDOUT       (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN1_CFG << ADI_GPIO_MASK_SHIFT)  | (3 << BITP_GPIO_GPCON_PIN1_CFG)) /*!< Encoding for PDI sdata out.     */

/* P1.2 */
#define ADI_GPIO_P12            (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN2_CFG << ADI_GPIO_MASK_SHIFT)  | (0 << BITP_GPIO_GPCON_PIN2_CFG)) /*!< Encoding for GPIO.              */
#define ADI_GPIO_LCDS5          (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN2_CFG << ADI_GPIO_MASK_SHIFT)  | (1 << BITP_GPIO_GPCON_PIN2_CFG)) /*!< Encoding for LCD Seg5.          */
#define ADI_GPIO_PDID2          (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN2_CFG << ADI_GPIO_MASK_SHIFT)  | (2 << BITP_GPIO_GPCON_PIN2_CFG)) /*!< Encoding for PDI D2.            */
#define ADI_GPIO_PDISDIN        (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN2_CFG << ADI_GPIO_MASK_SHIFT)  | (3 << BITP_GPIO_GPCON_PIN2_CFG)) /*!< Encoding for PDI sdata in.      */

/* P1.3 */
#define ADI_GPIO_P13            (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN3_CFG << ADI_GPIO_MASK_SHIFT)  | (0 << BITP_GPIO_GPCON_PIN3_CFG)) /*!< Encoding for GPIO.              */
#define ADI_GPIO_LCDS6          (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN3_CFG << ADI_GPIO_MASK_SHIFT)  | (1 << BITP_GPIO_GPCON_PIN3_CFG)) /*!< Encoding for LCD Seg6.          */
#define ADI_GPIO_PDID3          (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN3_CFG << ADI_GPIO_MASK_SHIFT)  | (2 << BITP_GPIO_GPCON_PIN3_CFG)) /*!< Encoding for PDI D3.            */

/* P1.4 */
#define ADI_GPIO_P14            (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN4_CFG << ADI_GPIO_MASK_SHIFT)  | (0 << BITP_GPIO_GPCON_PIN4_CFG)) /*!< Encoding for GPIO.              */
#define ADI_GPIO_LCDS7          (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN4_CFG << ADI_GPIO_MASK_SHIFT)  | (1 << BITP_GPIO_GPCON_PIN4_CFG)) /*!< Encoding for LCD Seg7.          */
#define ADI_GPIO_PDID4          (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN4_CFG << ADI_GPIO_MASK_SHIFT)  | (2 << BITP_GPIO_GPCON_PIN4_CFG)) /*!< Encoding for PDI D4.            */

/* P1.5 */
#define ADI_GPIO_P15            (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN5_CFG << ADI_GPIO_MASK_SHIFT)  | (0 << BITP_GPIO_GPCON_PIN5_CFG)) /*!< Encoding for GPIO.              */
#define ADI_GPIO_LCDS8          (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN5_CFG << ADI_GPIO_MASK_SHIFT)  | (1 << BITP_GPIO_GPCON_PIN5_CFG)) /*!< Encoding for LCD Seg8.          */
#define ADI_GPIO_PDID5          (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN5_CFG << ADI_GPIO_MASK_SHIFT)  | (2 << BITP_GPIO_GPCON_PIN5_CFG)) /*!< Encoding for PDI D5.            */

/* P1.6 */
#define ADI_GPIO_P16            (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN6_CFG << ADI_GPIO_MASK_SHIFT)  | (0 << BITP_GPIO_GPCON_PIN6_CFG)) /*!< Encoding for GPIO.              */
#define ADI_GPIO_LCDS9          (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN6_CFG << ADI_GPIO_MASK_SHIFT)  | (1 << BITP_GPIO_GPCON_PIN6_CFG)) /*!< Encoding for LCD Seg9.          */
#define ADI_GPIO_PDID6          (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN6_CFG << ADI_GPIO_MASK_SHIFT)  | (2 << BITP_GPIO_GPCON_PIN6_CFG)) /*!< Encoding for PDI D6.            */

/* P1.7 */
#define ADI_GPIO_P17            (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN7_CFG << ADI_GPIO_MASK_SHIFT)  | (0 << BITP_GPIO_GPCON_PIN7_CFG)) /*!< Encoding for GPIO.              */
#define ADI_GPIO_LCDS10         (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN7_CFG << ADI_GPIO_MASK_SHIFT)  | (1 << BITP_GPIO_GPCON_PIN7_CFG)) /*!< Encoding for LCD Seg10.         */
#define ADI_GPIO_PDID7          (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN7_CFG << ADI_GPIO_MASK_SHIFT)  | (2 << BITP_GPIO_GPCON_PIN7_CFG)) /*!< Encoding for PDI D7.            */
#define ADI_GPIO_CLKOUT         (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN7_CFG << ADI_GPIO_MASK_SHIFT)  | (3 << BITP_GPIO_GPCON_PIN7_CFG)) /*!< Encoding for system clock out.  */

/* P1.8 */
#define ADI_GPIO_P18            (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN8_CFG << ADI_GPIO_MASK_SHIFT)  | (0 << BITP_GPIO_GPCON_PIN8_CFG)) /*!< Encoding for GPIO.              */
#define ADI_GPIO_LCDS11         (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN8_CFG << ADI_GPIO_MASK_SHIFT)  | (1 << BITP_GPIO_GPCON_PIN8_CFG)) /*!< Encoding for LCD Seg11.         */
#define ADI_GPIO_PDID8          (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN8_CFG << ADI_GPIO_MASK_SHIFT)  | (2 << BITP_GPIO_GPCON_PIN8_CFG)) /*!< Encoding for PDI D8.            */

/* P1.9 */
#define ADI_GPIO_P19            (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN9_CFG << ADI_GPIO_MASK_SHIFT)  | (0 << BITP_GPIO_GPCON_PIN9_CFG)) /*!< Encoding for GPIO.              */
#define ADI_GPIO_LCDS12         (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN9_CFG << ADI_GPIO_MASK_SHIFT)  | (1 << BITP_GPIO_GPCON_PIN9_CFG)) /*!< Encoding for LCD Seg12.         */
#define ADI_GPIO_PDID9          (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN9_CFG << ADI_GPIO_MASK_SHIFT)  | (2 << BITP_GPIO_GPCON_PIN9_CFG)) /*!< Encoding for PDI D9.            */

/* P1.10 */
#define ADI_GPIO_P110           (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN10_CFG << ADI_GPIO_MASK_SHIFT) | (0 << BITP_GPIO_GPCON_PIN10_CFG)) /*!< Encoding for GPIO.             */
#define ADI_GPIO_LCDS13         (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN10_CFG << ADI_GPIO_MASK_SHIFT) | (1 << BITP_GPIO_GPCON_PIN10_CFG)) /*!< Encoding for LCD Seg13.        */
#define ADI_GPIO_PDID10         (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN10_CFG << ADI_GPIO_MASK_SHIFT) | (2 << BITP_GPIO_GPCON_PIN10_CFG)) /*!< Encoding for PDI D10.          */

/* P1.11 */
#define ADI_GPIO_P111           (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN11_CFG << ADI_GPIO_MASK_SHIFT) | (0 << BITP_GPIO_GPCON_PIN11_CFG)) /*!< Encoding for GPIO.             */
#define ADI_GPIO_LCDS14         (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN11_CFG << ADI_GPIO_MASK_SHIFT) | (1 << BITP_GPIO_GPCON_PIN11_CFG)) /*!< Encoding for LCD Seg14.        */
#define ADI_GPIO_PDID11         (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN11_CFG << ADI_GPIO_MASK_SHIFT) | (2 << BITP_GPIO_GPCON_PIN11_CFG)) /*!< Encoding for PDI D11.          */

/* P1.12 */
#define ADI_GPIO_P112           (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN12_CFG << ADI_GPIO_MASK_SHIFT) | (0 << BITP_GPIO_GPCON_PIN12_CFG)) /*!< Encoding for GPIO.             */
#define ADI_GPIO_LCDS15         (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN12_CFG << ADI_GPIO_MASK_SHIFT) | (1 << BITP_GPIO_GPCON_PIN12_CFG)) /*!< Encoding for LCD Seg15.        */
#define ADI_GPIO_PDID12         (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN12_CFG << ADI_GPIO_MASK_SHIFT) | (2 << BITP_GPIO_GPCON_PIN12_CFG)) /*!< Encoding for PDI D12.          */

/* P1.13 */
#define ADI_GPIO_P113           (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN13_CFG << ADI_GPIO_MASK_SHIFT) | (0 << BITP_GPIO_GPCON_PIN13_CFG)) /*!< Encoding for GPIO.             */
#define ADI_GPIO_LCDS16         (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN13_CFG << ADI_GPIO_MASK_SHIFT) | (1 << BITP_GPIO_GPCON_PIN13_CFG)) /*!< Encoding for LCD Seg16.        */
#define ADI_GPIO_PDID13         (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN13_CFG << ADI_GPIO_MASK_SHIFT) | (2 << BITP_GPIO_GPCON_PIN13_CFG)) /*!< Encoding for PDI D13.          */

/* P1.14 */
#define ADI_GPIO_P114           (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN14_CFG << ADI_GPIO_MASK_SHIFT) | (0 << BITP_GPIO_GPCON_PIN14_CFG)) /*!< Encoding for GPIO.             */
#define ADI_GPIO_LCDS17         (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN14_CFG << ADI_GPIO_MASK_SHIFT) | (1 << BITP_GPIO_GPCON_PIN14_CFG)) /*!< Encoding for LCD Seg17.        */
#define ADI_GPIO_PDID14         (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN14_CFG << ADI_GPIO_MASK_SHIFT) | (2 << BITP_GPIO_GPCON_PIN14_CFG)) /*!< Encoding for PDI D14.          */

/* P1.15 */
#define ADI_GPIO_P115           (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN15_CFG << ADI_GPIO_MASK_SHIFT) | (0 << BITP_GPIO_GPCON_PIN15_CFG)) /*!< Encoding for GPIO.             */
#define ADI_GPIO_LCDS18         (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN15_CFG << ADI_GPIO_MASK_SHIFT) | (1 << BITP_GPIO_GPCON_PIN15_CFG)) /*!< Encoding for LCD Seg18.        */
#define ADI_GPIO_PDID15         (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN15_CFG << ADI_GPIO_MASK_SHIFT) | (2 << BITP_GPIO_GPCON_PIN15_CFG)) /*!< Encoding for PDI D15.          */


/* Port 2 */
/* P2.0 */
#define ADI_GPIO_P20            (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN0_CFG << ADI_GPIO_MASK_SHIFT)  | (0 << BITP_GPIO_GPCON_PIN0_CFG)) /*!< Encoding for GPIO.              */
#define ADI_GPIO_LCBP0          (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN0_CFG << ADI_GPIO_MASK_SHIFT)  | (1 << BITP_GPIO_GPCON_PIN0_CFG)) /*!< Encoding for LCD Back Plane 0.  */

/* P2.1 */
#define ADI_GPIO_P21            (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN1_CFG << ADI_GPIO_MASK_SHIFT)  | (0 << BITP_GPIO_GPCON_PIN1_CFG)) /*!< Encoding for GPIO.              */
#define ADI_GPIO_LCBP1          (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN1_CFG << ADI_GPIO_MASK_SHIFT)  | (1 << BITP_GPIO_GPCON_PIN1_CFG)) /*!< Encoding for LCD Back Plane 1.  */
#define ADI_GPIO_PDIRST         (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN1_CFG << ADI_GPIO_MASK_SHIFT)  | (2 << BITP_GPIO_GPCON_PIN1_CFG)) /*!< Encoding for PDI Reset.         */

/* P2.2 */
#define ADI_GPIO_P22            (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN2_CFG << ADI_GPIO_MASK_SHIFT)  | (0 << BITP_GPIO_GPCON_PIN2_CFG)) /*!< Encoding for GPIO.              */
#define ADI_GPIO_LCBP2          (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN2_CFG << ADI_GPIO_MASK_SHIFT)  | (1 << BITP_GPIO_GPCON_PIN2_CFG)) /*!< Encoding for LCD Back Plane 2.  */
#define ADI_GPIO_PDICSX         (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN2_CFG << ADI_GPIO_MASK_SHIFT)  | (2 << BITP_GPIO_GPCON_PIN2_CFG)) /*!< Encoding for PDI Chip Select.   */

/* P2.3 */
#define ADI_GPIO_P23            (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN3_CFG << ADI_GPIO_MASK_SHIFT)  | (0 << BITP_GPIO_GPCON_PIN3_CFG)) /*!< Encoding for GPIO.              */
#define ADI_GPIO_LCBP3          (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN3_CFG << ADI_GPIO_MASK_SHIFT)  | (1 << BITP_GPIO_GPCON_PIN3_CFG)) /*!< Encoding for LCD Back Plane 2.  */
#define ADI_GPIO_PDIDCX         (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN3_CFG << ADI_GPIO_MASK_SHIFT)  | (2 << BITP_GPIO_GPCON_PIN3_CFG)) /*!< Encoding for PDI Cntl/Data Sel. */

/* P2.4 */
#define ADI_GPIO_P24            (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN4_CFG << ADI_GPIO_MASK_SHIFT)  | (0 << BITP_GPIO_GPCON_PIN4_CFG)) /*!< Encoding for GPIO.              */
#define ADI_GPIO_LCDS1          (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN4_CFG << ADI_GPIO_MASK_SHIFT)  | (1 << BITP_GPIO_GPCON_PIN4_CFG)) /*!< Encoding for LCD Seg1.          */
#define ADI_GPIO_PDIRWX         (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN4_CFG << ADI_GPIO_MASK_SHIFT)  | (2 << BITP_GPIO_GPCON_PIN4_CFG)) /*!< Encoding for PDI R/WX.          */
#define ADI_GPIO_PDIRDX         (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN4_CFG << ADI_GPIO_MASK_SHIFT)  | (2 << BITP_GPIO_GPCON_PIN4_CFG)) /*!< Encoding for PDI RegRead.       */

/* P2.5 */
#define ADI_GPIO_P25            (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN5_CFG << ADI_GPIO_MASK_SHIFT)  | (0 << BITP_GPIO_GPCON_PIN5_CFG)) /*!< Encoding for GPIO.              */
#define ADI_GPIO_LCDS2          (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN5_CFG << ADI_GPIO_MASK_SHIFT)  | (1 << BITP_GPIO_GPCON_PIN5_CFG)) /*!< Encoding for LCD Seg2.          */
#define ADI_GPIO_PDIECLK        (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN5_CFG << ADI_GPIO_MASK_SHIFT)  | (2 << BITP_GPIO_GPCON_PIN5_CFG)) /*!< Encoding for PDI E clock.       */
#define ADI_GPIO_PDIWRX         (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN5_CFG << ADI_GPIO_MASK_SHIFT)  | (2 << BITP_GPIO_GPCON_PIN5_CFG)) /*!< Encoding for PDI RegWrite.      */

/* P2.6 */
#define ADI_GPIO_P26            (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN6_CFG << ADI_GPIO_MASK_SHIFT)  | (0 << BITP_GPIO_GPCON_PIN6_CFG)) /*!< Encoding for GPIO.              */
#define ADI_GPIO_LCDS19         (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN6_CFG << ADI_GPIO_MASK_SHIFT)  | (1 << BITP_GPIO_GPCON_PIN6_CFG)) /*!< Encoding for LCD Seg19.         */
#define ADI_GPIO_PDITEAR        (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN6_CFG << ADI_GPIO_MASK_SHIFT)  | (2 << BITP_GPIO_GPCON_PIN6_CFG)) /*!< Encoding for PDI Tearing.       */

/* P2.7 */
#define ADI_GPIO_P27            (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN7_CFG << ADI_GPIO_MASK_SHIFT)  | (0 << BITP_GPIO_GPCON_PIN7_CFG)) /*!< Encoding for GPIO.              */
#define ADI_GPIO_LCDS20         (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN7_CFG << ADI_GPIO_MASK_SHIFT)  | (1 << BITP_GPIO_GPCON_PIN7_CFG)) /*!< Encoding for LCD Seg20.         */
#define ADI_GPIO_PWMA           (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN7_CFG << ADI_GPIO_MASK_SHIFT)  | (2 << BITP_GPIO_GPCON_PIN7_CFG)) /*!< Encoding for PWM TimerA.        */

/* P2.8 */
#define ADI_GPIO_P28            (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN8_CFG << ADI_GPIO_MASK_SHIFT)  | (0 << BITP_GPIO_GPCON_PIN8_CFG)) /*!< Encoding for GPIO.              */
#define ADI_GPIO_LCDS21         (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN8_CFG << ADI_GPIO_MASK_SHIFT)  | (1 << BITP_GPIO_GPCON_PIN8_CFG)) /*!< Encoding for LCD Seg21.         */

/* P2.9 */
#define ADI_GPIO_P29            (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN9_CFG << ADI_GPIO_MASK_SHIFT)  | (0 << BITP_GPIO_GPCON_PIN9_CFG)) /*!< Encoding for GPIO.              */
#define ADI_GPIO_LCDS22         (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN9_CFG << ADI_GPIO_MASK_SHIFT)  | (1 << BITP_GPIO_GPCON_PIN9_CFG)) /*!< Encoding for LCD Seg22.         */

/* P2.10 */
#define ADI_GPIO_P210           (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN10_CFG << ADI_GPIO_MASK_SHIFT) | (0 << BITP_GPIO_GPCON_PIN10_CFG)) /*!< Encoding for GPIO.             */
#define ADI_GPIO_LCDS23         (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN10_CFG << ADI_GPIO_MASK_SHIFT) | (1 << BITP_GPIO_GPCON_PIN10_CFG)) /*!< Encoding for LCD Seg23.        */

/* P2.11 */
#define ADI_GPIO_P211           (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN11_CFG << ADI_GPIO_MASK_SHIFT) | (0 << BITP_GPIO_GPCON_PIN11_CFG)) /*!< Encoding for GPIO.             */
#define ADI_GPIO_LCDS24         (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN11_CFG << ADI_GPIO_MASK_SHIFT) | (1 << BITP_GPIO_GPCON_PIN11_CFG)) /*!< Encoding for LCD Seg24.        */

/* P2.12 */
#define ADI_GPIO_P212           (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN12_CFG << ADI_GPIO_MASK_SHIFT) | (0 << BITP_GPIO_GPCON_PIN12_CFG)) /*!< Encoding for GPIO.             */
#define ADI_GPIO_LCDS25         (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN12_CFG << ADI_GPIO_MASK_SHIFT) | (1 << BITP_GPIO_GPCON_PIN12_CFG)) /*!< Encoding for LCD Seg25.        */

/* P2.13 */
#define ADI_GPIO_P213           (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN13_CFG << ADI_GPIO_MASK_SHIFT) | (0 << BITP_GPIO_GPCON_PIN13_CFG)) /*!< Encoding for GPIO.             */
#define ADI_GPIO_LCDS26         (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN13_CFG << ADI_GPIO_MASK_SHIFT) | (1 << BITP_GPIO_GPCON_PIN13_CFG)) /*!< Encoding for LCD Seg26.        */

/* P2.14 */
#define ADI_GPIO_P214           (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN14_CFG << ADI_GPIO_MASK_SHIFT) | (0 << BITP_GPIO_GPCON_PIN14_CFG)) /*!< Encoding for GPIO.             */
#define ADI_GPIO_LCDS27         (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN14_CFG << ADI_GPIO_MASK_SHIFT) | (1 << BITP_GPIO_GPCON_PIN14_CFG)) /*!< Encoding for LCD Seg27.        */

/* P2.15 */
#define ADI_GPIO_P215           (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN15_CFG << ADI_GPIO_MASK_SHIFT) | (0 << BITP_GPIO_GPCON_PIN15_CFG)) /*!< Encoding for GPIO.             */
#define ADI_GPIO_LCDS28         (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN15_CFG << ADI_GPIO_MASK_SHIFT) | (1 << BITP_GPIO_GPCON_PIN15_CFG)) /*!< Encoding for LCD Seg28.        */


/* Port 3 */
/* P3.0 */
#define ADI_GPIO_P30            (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN0_CFG << ADI_GPIO_MASK_SHIFT)  | (0 << BITP_GPIO_GPCON_PIN0_CFG)) /*!< Encoding for GPIO.              */
#define ADI_GPIO_SPI0SCLK       (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN0_CFG << ADI_GPIO_MASK_SHIFT)  | (1 << BITP_GPIO_GPCON_PIN0_CFG)) /*!< Encoding for SPI0 SCLK.         */

/* P3.1 */
#define ADI_GPIO_P31            (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN1_CFG << ADI_GPIO_MASK_SHIFT)  | (0 << BITP_GPIO_GPCON_PIN1_CFG)) /*!< Encoding for GPIO.              */
#define ADI_GPIO_SPI0MISO       (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN1_CFG << ADI_GPIO_MASK_SHIFT)  | (1 << BITP_GPIO_GPCON_PIN1_CFG)) /*!< Encoding for SPI0 MISO.         */

/* P3.2 */
#define ADI_GPIO_P32            (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN2_CFG << ADI_GPIO_MASK_SHIFT)  | (0 << BITP_GPIO_GPCON_PIN2_CFG)) /*!< Encoding for GPIO.              */
#define ADI_GPIO_SPI0MOSI       (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN2_CFG << ADI_GPIO_MASK_SHIFT)  | (1 << BITP_GPIO_GPCON_PIN2_CFG)) /*!< Encoding for SPI0 MOSI.         */

/* P3.3 */
#define ADI_GPIO_P33            (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN3_CFG << ADI_GPIO_MASK_SHIFT)  | (0 << BITP_GPIO_GPCON_PIN3_CFG)) /*!< Encoding for GPIO.              */
#define ADI_GPIO_SPI0CS         (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN3_CFG << ADI_GPIO_MASK_SHIFT)  | (1 << BITP_GPIO_GPCON_PIN3_CFG)) /*!< Encoding for SPI0 CS.           */

/* P3.4 */
#define ADI_GPIO_P34            (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN4_CFG << ADI_GPIO_MASK_SHIFT)  | (0 << BITP_GPIO_GPCON_PIN4_CFG)) /*!< Encoding for GPIO.              */
#define ADI_GPIO_I2C0SCL        (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN4_CFG << ADI_GPIO_MASK_SHIFT)  | (1 << BITP_GPIO_GPCON_PIN4_CFG)) /*!< Encoding for I2C SCL.           */
/* no mux2 encoding fpr P3.4 */
#define ADI_GPIO_SPI1SCLK       (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN4_CFG << ADI_GPIO_MASK_SHIFT)  | (3 << BITP_GPIO_GPCON_PIN4_CFG)) /*!< Encoding for SPI1 SCLK.         */

/* P3.5 */
#define ADI_GPIO_P35            (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN5_CFG << ADI_GPIO_MASK_SHIFT)  | (0 << BITP_GPIO_GPCON_PIN5_CFG)) /*!< Encoding for GPIO.              */
#define ADI_GPIO_I2C0SDA        (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN5_CFG << ADI_GPIO_MASK_SHIFT)  | (1 << BITP_GPIO_GPCON_PIN5_CFG)) /*!< Encoding for I2C SDA.           */
/* no mux2 encoding fpr P3.5 */
#define ADI_GPIO_SPI1MISO       (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN5_CFG << ADI_GPIO_MASK_SHIFT)  | (3 << BITP_GPIO_GPCON_PIN5_CFG)) /*!< Encoding for SPI MISO.          */

/* P3.6 */
#define ADI_GPIO_P36            (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN6_CFG << ADI_GPIO_MASK_SHIFT)  | (0 << BITP_GPIO_GPCON_PIN6_CFG)) /*!< Encoding for GPIO.              */
#define ADI_GPIO_ALT_UART0TXD   (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN6_CFG << ADI_GPIO_MASK_SHIFT)  | (1 << BITP_GPIO_GPCON_PIN6_CFG)) /*!< Encoding for alt. UART TX.      */
#define ADI_GPIO_PWMB           (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN6_CFG << ADI_GPIO_MASK_SHIFT)  | (2 << BITP_GPIO_GPCON_PIN6_CFG)) /*!< Encoding for PWM TimerB.        */
#define ADI_GPIO_SPI1MOSI       (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN6_CFG << ADI_GPIO_MASK_SHIFT)  | (3 << BITP_GPIO_GPCON_PIN6_CFG)) /*!< Encoding for SPI1 MOSI.         */

/* P3.7 */
#define ADI_GPIO_P37            (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN7_CFG << ADI_GPIO_MASK_SHIFT)  | (0 << BITP_GPIO_GPCON_PIN7_CFG)) /*!< Encoding for GPIO.              */
#define ADI_GPIO_ALT_UART0RXD   (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN7_CFG << ADI_GPIO_MASK_SHIFT)  | (1 << BITP_GPIO_GPCON_PIN7_CFG)) /*!< Encoding for alt. UART rX.      */
#define ADI_GPIO_ALT_PWMC       (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN7_CFG << ADI_GPIO_MASK_SHIFT)  | (2 << BITP_GPIO_GPCON_PIN7_CFG)) /*!< Encoding for alt. PWM TimerC.   */
#define ADI_GPIO_SPI1CS         (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN7_CFG << ADI_GPIO_MASK_SHIFT)  | (3 << BITP_GPIO_GPCON_PIN7_CFG)) /*!< Encoding for SPI1 CS.           */

/* P3.8 */
#define ADI_GPIO_P38            (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN8_CFG << ADI_GPIO_MASK_SHIFT)  | (0 << BITP_GPIO_GPCON_PIN8_CFG)) /*!< Encoding for GPIO.              */
#define ADI_GPIO_LCDS29         (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN8_CFG << ADI_GPIO_MASK_SHIFT)  | (1 << BITP_GPIO_GPCON_PIN8_CFG)) /*!< Encoding for LCD Seg29.         */

/* P3.9 */
#define ADI_GPIO_P39            (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN9_CFG << ADI_GPIO_MASK_SHIFT)  | (0 << BITP_GPIO_GPCON_PIN9_CFG)) /*!< Encoding for GPIO.              */
#define ADI_GPIO_LCDS30         (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN9_CFG << ADI_GPIO_MASK_SHIFT)  | (1 << BITP_GPIO_GPCON_PIN9_CFG)) /*!< Encoding for LCD Seg30.         */

/* P3.10 */
#define ADI_GPIO_P310           (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN10_CFG << ADI_GPIO_MASK_SHIFT) | (0 << BITP_GPIO_GPCON_PIN10_CFG)) /*!< Encoding for GPIO.             */
#define ADI_GPIO_LCDS31         (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN10_CFG << ADI_GPIO_MASK_SHIFT) | (1 << BITP_GPIO_GPCON_PIN10_CFG)) /*!< Encoding for LCD SeG31.        */

/* P3.11 */
#define ADI_GPIO_P311           (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN11_CFG << ADI_GPIO_MASK_SHIFT) | (0 << BITP_GPIO_GPCON_PIN11_CFG)) /*!< Encoding for GPIO.             */
#define ADI_GPIO_LCDS32         (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN11_CFG << ADI_GPIO_MASK_SHIFT) | (1 << BITP_GPIO_GPCON_PIN11_CFG)) /*!< Encoding for LCD Seg32.        */

/* P3.12 */
#define ADI_GPIO_P312           (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN12_CFG << ADI_GPIO_MASK_SHIFT) | (0 << BITP_GPIO_GPCON_PIN12_CFG)) /*!< Encoding for GPIO.             */
#define ADI_GPIO_TONEP          (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN12_CFG << ADI_GPIO_MASK_SHIFT) | (1 << BITP_GPIO_GPCON_PIN12_CFG)) /*!< Encoding for Beep ToneP.       */
#define ADI_GPIO_I2SSCK         (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN12_CFG << ADI_GPIO_MASK_SHIFT) | (2 << BITP_GPIO_GPCON_PIN12_CFG)) /*!< Encoding for I2S Serial Clk.   */

/* P3.13 */
#define ADI_GPIO_P313           (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN13_CFG << ADI_GPIO_MASK_SHIFT) | (0 << BITP_GPIO_GPCON_PIN13_CFG)) /*!< Encoding for GPIO.             */
#define ADI_GPIO_TONEN          (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN13_CFG << ADI_GPIO_MASK_SHIFT) | (1 << BITP_GPIO_GPCON_PIN13_CFG)) /*!< Encoding for Beep ToneN.       */
#define ADI_GPIO_I2SSD          (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN13_CFG << ADI_GPIO_MASK_SHIFT) | (2 << BITP_GPIO_GPCON_PIN13_CFG)) /*!< Encoding for I2S Serial Data.  */

/* P3.14 */
#define ADI_GPIO_P314           (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN14_CFG << ADI_GPIO_MASK_SHIFT) | (0 << BITP_GPIO_GPCON_PIN14_CFG)) /*!< Encoding for GPIO.             */
/* no mux1 encoding fpr P3.14 */
#define ADI_GPIO_I2SWS          (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN14_CFG << ADI_GPIO_MASK_SHIFT) | (2 << BITP_GPIO_GPCON_PIN14_CFG)) /*!< Encoding for I2S Word Select.  */

/* P3.15 (reserved) */


/* Port 4 */
/* P4.0 */
#define ADI_GPIO_P40            (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN0_CFG << ADI_GPIO_MASK_SHIFT)  | (0 << BITP_GPIO_GPCON_PIN0_CFG)) /*!< Encoding for GPIO.              */
#define ADI_GPIO_ALT_I2C0SCL    (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN0_CFG << ADI_GPIO_MASK_SHIFT)  | (1 << BITP_GPIO_GPCON_PIN0_CFG)) /*!< Encoding for alt. I2C SCL.      */

/* P4.1 */
#define ADI_GPIO_P41            (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN1_CFG << ADI_GPIO_MASK_SHIFT)  | (0 << BITP_GPIO_GPCON_PIN1_CFG)) /*!< Encoding for GPIO.              */
#define ADI_GPIO_ALT_I2C0SDA    (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN1_CFG << ADI_GPIO_MASK_SHIFT)  | (1 << BITP_GPIO_GPCON_PIN1_CFG)) /*!< Encoding for alt. I2C SDA.      */

/* P4.2 */
#define ADI_GPIO_P42            (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN2_CFG << ADI_GPIO_MASK_SHIFT)  | (0 << BITP_GPIO_GPCON_PIN2_CFG)) /*!< Encoding for GPIO.              */
#define ADI_GPIO_ALT_PWMB       (((ADI_GPIO_MUX_TYPE)BITM_GPIO_GPCON_PIN2_CFG << ADI_GPIO_MASK_SHIFT)  | (1 << BITP_GPIO_GPCON_PIN2_CFG)) /*!< Encoding for alt. PWM TimerB.   */

/* P3.3:15 (reserved) */


/* GPIO API functions */
extern ADI_GPIO_RESULT_TYPE  adi_GPIO_Init                     (void);                                                                              /*!< Initializes the GPIO device functions */
extern ADI_GPIO_RESULT_TYPE  adi_GPIO_UnInit                   (void);                                                                              /*!< Uninitializes the muxing and GPIOs */
extern ADI_GPIO_RESULT_TYPE  adi_GPIO_ResetToPowerUp           (void);                                                                              /*!< Resets GPIO registers to power up defaults */

extern ADI_GPIO_CONFIG_TYPE  adi_GPIO_RetrievePinMuxing        (const ADI_GPIO_PORT_TYPE Port);                                                     /*!< Retrieves the pin muxing for a port */

extern ADI_GPIO_RESULT_TYPE  adi_GPIO_EnableIRQ                (const IRQn_Type eIrq, const ADI_GPIO_IRQ_TRIGGER_CONDITION_TYPE eCondition);        /*!< Enables an external GPIO interrupt */
extern ADI_GPIO_RESULT_TYPE  adi_GPIO_DisableIRQ               (const IRQn_Type eIrq);                                                              /*!< Disables an external GPIO interrupt */
extern ADI_GPIO_RESULT_TYPE  adi_GPIO_ClearIRQ                 (const IRQn_Type eIrq);                                                              /*!< Clears an external interrupt */

extern ADI_GPIO_RESULT_TYPE  adi_GPIO_RegisterCallback         (const IRQn_Type eIrq, ADI_CALLBACK const pfCallback, void *const pCBParam );  /*!< Registers a callback for external interrupt */

/* ADuCM350 adds pin interrupts with Group A/B mask registers for each port... all of which land in either of two (Group A/B) interrupts) */
extern ADI_GPIO_RESULT_TYPE  adi_GPIO_SetGroupInterruptPins     (const ADI_GPIO_PORT_TYPE Port, const IRQn_Type eIrq, const ADI_GPIO_DATA_TYPE Pins);  /*!< Set pin interrupt group A/B register */
extern ADI_GPIO_DATA_TYPE    adi_GPIO_GetGroupInterruptPins     (const ADI_GPIO_PORT_TYPE Port, const IRQn_Type eIrq);                                 /*!< Get pin interrupt group A/B register */
extern ADI_GPIO_RESULT_TYPE  adi_GPIO_SetGroupInterruptPolarity (const ADI_GPIO_PORT_TYPE Port, const ADI_GPIO_DATA_TYPE Pins);                      /*!< Set pin interrupt polarity */
extern ADI_GPIO_DATA_TYPE    adi_GPIO_GetGroupInterruptPolarity (const ADI_GPIO_PORT_TYPE Port);                                                     /*!< Get pin interrupt polarity */
extern ADI_GPIO_DATA_TYPE    adi_GPIO_GetGroupInterruptStatus   (const ADI_GPIO_PORT_TYPE Port);                                                     /*!< Get a port's interrupt status bits*/
extern ADI_GPIO_RESULT_TYPE  adi_GPIO_ClrGroupInterruptStatus   (const ADI_GPIO_PORT_TYPE Port, const ADI_GPIO_DATA_TYPE Pins);                      /*!< Clear a port's interrupt status bits */

extern ADI_GPIO_RESULT_TYPE  adi_GPIO_SetOutputEnable          (const ADI_GPIO_PORT_TYPE Port, const ADI_GPIO_DATA_TYPE Pins, const bool_t bFlag);  /*!< Enables/disables the output driver for GPIO output pins */

/* ADuCM350 has seperate input and output enables that are independent of each other */
extern ADI_GPIO_RESULT_TYPE  adi_GPIO_SetInputEnable           (const ADI_GPIO_PORT_TYPE Port, const ADI_GPIO_DATA_TYPE Pins, const bool_t bFlag);  /*!< Enables/disables the output driver for GPIO output pins */

extern ADI_GPIO_RESULT_TYPE  adi_GPIO_SetPullUpEnable          (const ADI_GPIO_PORT_TYPE Port, const ADI_GPIO_DATA_TYPE Pins, const bool_t bFlag);  /*!< Enables/disables the pullup for GPIO output pins */

extern ADI_GPIO_DATA_TYPE    adi_GPIO_GetOutputEnable          (const ADI_GPIO_PORT_TYPE Port);                                                     /*!< Gets the output enable status for a port */
/* ADuCM350 has seperate input and output enables that are independent of each other */
extern ADI_GPIO_DATA_TYPE    adi_GPIO_GetInputEnable           (const ADI_GPIO_PORT_TYPE Port);                                                      /*!< Gets the input enable status for a port */

extern ADI_GPIO_DATA_TYPE    adi_GPIO_GetPullUpEnable          (const ADI_GPIO_PORT_TYPE Port);                                                     /*!< Gets the pull up enable status for a port */

extern ADI_GPIO_RESULT_TYPE  adi_GPIO_SetHigh                  (const ADI_GPIO_PORT_TYPE Port, const ADI_GPIO_DATA_TYPE Pins);                      /*!< Sets GPIO pins to a logical high level */
extern ADI_GPIO_RESULT_TYPE  adi_GPIO_SetLow                   (const ADI_GPIO_PORT_TYPE Port, const ADI_GPIO_DATA_TYPE Pins);                      /*!< Sets GPIO pins to a logical low level */
extern ADI_GPIO_RESULT_TYPE  adi_GPIO_Toggle                   (const ADI_GPIO_PORT_TYPE Port, const ADI_GPIO_DATA_TYPE Pins);                      /*!< Toggles the level of GPIO pins */
extern ADI_GPIO_RESULT_TYPE  adi_GPIO_SetData                  (const ADI_GPIO_PORT_TYPE Port, const ADI_GPIO_DATA_TYPE Pins);                      /*!< Sets the absolute value of GPIOs */
extern ADI_GPIO_DATA_TYPE    adi_GPIO_GetData                  (const ADI_GPIO_PORT_TYPE Port);                                                     /*!< Gets the level of GPIO pins on a port */
extern ADI_GPIO_DATA_TYPE    adi_GPIO_GetOutputData            (const ADI_GPIO_PORT_TYPE Port);                                                     /*!< Gets the level of GPIO pins on a port */


/* C++ linkage */
#ifdef __cplusplus
}
#endif


#endif /* include guard */

/*
** EOF
*/

/*@}*/
