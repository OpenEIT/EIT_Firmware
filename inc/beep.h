/*********************************************************************************

Copyright (c) 2010-2014 Analog Devices, Inc. All Rights Reserved.

This software is proprietary to Analog Devices, Inc. and its licensors.  By using 
this software you agree to the terms of the associated Analog Devices Software 
License Agreement.

*********************************************************************************/

/*!
 *****************************************************************************
 @file:    beep.h
 @brief:   Beeper Device Definitions for ADuCxxx
 @version: $Revision: 28525 $
 @date:    $Date: 2014-11-12 14:51:26 -0500 (Wed, 12 Nov 2014) $
*****************************************************************************/

/*! \addtogroup BEEP_Driver BEEP Driver
 *  Real-Time Clock Device Driver
 *  @{
 */

#ifndef __BEEP_H__
#define __BEEP_H__

#include "device.h"
#include "adi_beep_config.h"

/* C++ linkage */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*!
 * \enum ADI_BEEP_RESULT_TYPE
 * Beeper API return codes
 */
typedef enum
{
    ADI_BEEP_SUCCESS = 0,                /*!<  No Error, API suceeded */

    ADI_BEEP_ERR_UNKNOWN_ERROR           /*!<  An unknown error was detected */
      = ADI_DEV_BEEP_ERROR_OFFSET,
    ADI_BEEP_ERR_ALREADY_INITIALIZED,    /*!<  BEEP is already initialized */
    ADI_BEEP_ERR_BAD_DEV_HANDLE,         /*!<  Invalid device handle passed */
    ADI_BEEP_ERR_BAD_INSTANCE,           /*!<  Asking to initialize an unknown instance */
    ADI_BEEP_ERR_NOT_INITIALIZED,        /*!<  BEEP not yet initialized */
    ADI_BEEP_ERR_PARAM_OUT_OF_RANGE,     /*!<  Parameter is out of range.          */
} ADI_BEEP_RESULT_TYPE;


/*!
 * \enum ADI_BEEP_DEV_ID_TYPE
 *  @brief  Beeper Device IDs.
 *  @details List of all Beeper Device IDs for the ADuCM350BBCZ part
 */
typedef enum
{
    ADI_BEEP_DEVID_0 = 0,       /*!<  BEEP Timer Device 0 */
    ADI_BEEP_MAX_DEVID          /*!<  max number of BEEP devices */
} ADI_BEEP_DEV_ID_TYPE;


/*!
 * \enum ADI_BEEP_TONE_TYPE
 *  @brief  Beeper Tone registers.
 *  @details List of all Beeper tone registers for the ADuCM350BBCZ part
 */
typedef enum
{
    ADI_BEEP_TONEA = 0,     /*!<  Tone A */
    ADI_BEEP_TONEB,         /*!<  Tone B */
    ADI_BEEP_MAX_TONES      /*!<  max number of tones */
} ADI_BEEP_TONE_TYPE;

/*!
 * \enum ADI_BEEP_INTERRUPT_TYPE
 *  @brief  Beeper Interrupt Bits.
 *  @details List of all Beeper interrupt (enables and status) bits.
 */
typedef enum
{
    ADI_BEEP_INTERRUPT_SEQ_END                 = (1 << BITP_BEEP_BEEP_CFG_IRQ_SEQ_END),         /*!<  Beeper sequence end interrupt bit      */
    ADI_BEEP_INTERRUPT_SEQ_NEAR_END            = (1 << BITP_BEEP_BEEP_CFG_IRQ_SEQ_NEAR_END),    /*!<  Beeper sequence near end interrupt bit */
    ADI_BEEP_INTERRUPT_TONEB_END               = (1 << BITP_BEEP_BEEP_CFG_IRQ_TONEB_END),       /*!<  Beeper tone B end interrupt bit        */
    ADI_BEEP_INTERRUPT_TONEB_START             = (1 << BITP_BEEP_BEEP_CFG_IRQ_TONEB_START),     /*!<  Beeper tone B start interrupt bit      */
    ADI_BEEP_INTERRUPT_TONEA_END               = (1 << BITP_BEEP_BEEP_CFG_IRQ_TONEA_END),       /*!<  Beeper tone A end interrupt bit        */
    ADI_BEEP_INTERRUPT_TONEA_START             = (1 << BITP_BEEP_BEEP_CFG_IRQ_TONEA_START),     /*!<  Beeper tone A start interrupt bit      */
} ADI_BEEP_INTERRUPT_TYPE;

/*!
 *****************************************************************************
 * An interrupt mask covering all Beeper interrupts.
 *****************************************************************************/
#define ADI_BEEP_ALL_INTERRUPTS ( ADI_BEEP_INTERRUPT_SEQ_END         \
                                | ADI_BEEP_INTERRUPT_SEQ_NEAR_END    \
                                | ADI_BEEP_INTERRUPT_TONEB_END       \
                                | ADI_BEEP_INTERRUPT_TONEB_START     \
                                | ADI_BEEP_INTERRUPT_TONEA_END       \
                                | ADI_BEEP_INTERRUPT_TONEA_START)

#define ADI_BEEP_DEVICE_ENABLE_BIT          (1 << BITP_BEEP_BEEP_CFG_BEEP_EN)                   /*!<  Main Beeper enable bit                           */
#define ADI_BEEP_DEVICE_BUSY_BIT            (1 << BITP_BEEP_BEEP_STAT_BEEP_BUSY)                /*!<  Beeper busy status bit                           */

#define ADI_BEEP_TONE_DISABLE               (1 << BITP_BEEP_BEEP_TONE_A_DISABLE)                /*!<  Beeper tone disable bit                          */

#define ADI_BEEP_SEQ_COUNT_BITPOS           (BITP_BEEP_BEEP_CFG_SEQ_REPEAT)                     /*!<  Beeper tone sequence count bitfield position     */
#define ADI_BEEP_TONE_FREQ_BITPOS           (BITP_BEEP_BEEP_TONE_A_FREQ)                        /*!<  Beeper tone frequency bitfield position          */
#define ADI_BEEP_TONE_DUR_BITPOS            (BITP_BEEP_BEEP_TONE_A_DUR)                         /*!<  Beeper tone duration bitfield position           */

#define ADI_BEEP_SEQUENCE_MASK              (BITM_BEEP_BEEP_CFG_SEQ_REPEAT)                     /*!<  Beeper sequence count bitfield mask              */
#define ADI_BEEP_TONE_FREQ_MASK             (BITM_BEEP_BEEP_TONE_A_FREQ)                        /*!<  Beeper tone frequency bitfield mask              */
#define ADI_BEEP_TONE_DUR_MASK              (BITM_BEEP_BEEP_TONE_A_DUR)                         /*!<  Beeper tone duration bitfield mask               */


typedef struct ADI_BEEP_DEV_DATA_TYPE * ADI_BEEP_HANDLE;                                        /*!< BEEP Device instance private data handle typedef. */

#define LFCLK_FREQ 16384.0f                                                                     /*!< Beeper main clock frequency.                      */
#define FREQUENCY_ENCODE(x) (uint8_t)(LFCLK_FREQ/x + 0.5f)                                      /*!< Beeper tone frequency encoder macro               */

/*!
 * \enum ADI_BEEP_NOTE_FREQUENCY_TYPE
 *  @brief  Beeper tone frequency list.
 *  @details List of possible Beeper tone frequencies.
 */
typedef enum {
	/* Constants are pre-computed note frequencies (Hz);                 */
	/* see http://www.phy.mtu.edu/~suits/notefreqs.html.                 */
	/* Encodings are clock divider values for that note.                 */
	/* Flats are the same as the lower sharp, so only sharps are listed. */
	/* Even though octaves are simple frequency doubelings/halvings      */
	/* of adjuacient octaves, we pre-compute each constant (as opposed   */
	/* to halving/doubeling the encodings between octaves) soas to       */
	/* minimize repeated doubeling/halving errors across all octaves.    */
	/* !!!ALL FREQUENCIES MUST BE 7-BIT OR LESS, I.E., C3 OR ABOVE!!!    */

	ADI_BEEP_FREQ_REST = (0),                 // silence

	ADI_BEEP_FREQ_C3  = FREQUENCY_ENCODE(130.81f),  // lowest representable frequencty
	ADI_BEEP_FREQ_Cs3 = FREQUENCY_ENCODE(138.59f),
	ADI_BEEP_FREQ_D3  = FREQUENCY_ENCODE(146.83f),
	ADI_BEEP_FREQ_Ds3 = FREQUENCY_ENCODE(155.56f),
	ADI_BEEP_FREQ_E3  = FREQUENCY_ENCODE(164.81f),
	ADI_BEEP_FREQ_F3  = FREQUENCY_ENCODE(174.61f),
	ADI_BEEP_FREQ_Fs3 = FREQUENCY_ENCODE(185.00f),
	ADI_BEEP_FREQ_G3  = FREQUENCY_ENCODE(196.00f),
	ADI_BEEP_FREQ_Gs3 = FREQUENCY_ENCODE(207.65f),
	ADI_BEEP_FREQ_A3  = FREQUENCY_ENCODE(220.00f),
	ADI_BEEP_FREQ_As3 = FREQUENCY_ENCODE(233.08f),
	ADI_BEEP_FREQ_B3  = FREQUENCY_ENCODE(246.94f),

	ADI_BEEP_FREQ_C4  = FREQUENCY_ENCODE(261.63f),  // Middle C
	ADI_BEEP_FREQ_Cs4 = FREQUENCY_ENCODE(277.18f),
	ADI_BEEP_FREQ_D4  = FREQUENCY_ENCODE(293.66f),
	ADI_BEEP_FREQ_Ds4 = FREQUENCY_ENCODE(311.13f),
	ADI_BEEP_FREQ_E4  = FREQUENCY_ENCODE(329.63f),
	ADI_BEEP_FREQ_F4  = FREQUENCY_ENCODE(349.23f),
	ADI_BEEP_FREQ_Fs4 = FREQUENCY_ENCODE(369.99f),
	ADI_BEEP_FREQ_G4  = FREQUENCY_ENCODE(392.00f),
	ADI_BEEP_FREQ_Gs4 = FREQUENCY_ENCODE(415.30f),
	ADI_BEEP_FREQ_A4  = FREQUENCY_ENCODE(440.00f),
	ADI_BEEP_FREQ_As4 = FREQUENCY_ENCODE(466.16f),
	ADI_BEEP_FREQ_B4  = FREQUENCY_ENCODE(493.88f),

	ADI_BEEP_FREQ_C5  = FREQUENCY_ENCODE(523.25f),
	ADI_BEEP_FREQ_Cs5 = FREQUENCY_ENCODE(554.37f),
	ADI_BEEP_FREQ_D5  = FREQUENCY_ENCODE(587.33f),
	ADI_BEEP_FREQ_Ds5 = FREQUENCY_ENCODE(622.25f),
	ADI_BEEP_FREQ_E5  = FREQUENCY_ENCODE(659.26f),
	ADI_BEEP_FREQ_F5  = FREQUENCY_ENCODE(698.46f),
	ADI_BEEP_FREQ_Fs5 = FREQUENCY_ENCODE(739.99f),
	ADI_BEEP_FREQ_G5  = FREQUENCY_ENCODE(783.99f),
	ADI_BEEP_FREQ_Gs5 = FREQUENCY_ENCODE(830.61f),
	ADI_BEEP_FREQ_A5  = FREQUENCY_ENCODE(880.00f),
	ADI_BEEP_FREQ_As5 = FREQUENCY_ENCODE(932.33f),
	ADI_BEEP_FREQ_B5  = FREQUENCY_ENCODE(987.77f),

	ADI_BEEP_FREQ_C6  = FREQUENCY_ENCODE(1046.50f),
	ADI_BEEP_FREQ_Cs6 = FREQUENCY_ENCODE(1108.73f),
	ADI_BEEP_FREQ_D6  = FREQUENCY_ENCODE(1174.66f),
	ADI_BEEP_FREQ_Ds6 = FREQUENCY_ENCODE(1244.51f),
	ADI_BEEP_FREQ_E6  = FREQUENCY_ENCODE(1318.51f),
	ADI_BEEP_FREQ_F6  = FREQUENCY_ENCODE(1396.91f),
	ADI_BEEP_FREQ_Fs6 = FREQUENCY_ENCODE(1479.98f),
	ADI_BEEP_FREQ_G6  = FREQUENCY_ENCODE(1567.98f),
	ADI_BEEP_FREQ_Gs6 = FREQUENCY_ENCODE(1661.22f),
	ADI_BEEP_FREQ_A6  = FREQUENCY_ENCODE(1760.00f),
	ADI_BEEP_FREQ_As6 = FREQUENCY_ENCODE(1864.66f),
	ADI_BEEP_FREQ_B6  = FREQUENCY_ENCODE(1975.53f),

	ADI_BEEP_FREQ_C7  = FREQUENCY_ENCODE(2093.00f),
	ADI_BEEP_FREQ_Cs7 = FREQUENCY_ENCODE(2217.46f),
	ADI_BEEP_FREQ_D7  = FREQUENCY_ENCODE(2349.32f),
	ADI_BEEP_FREQ_Ds7 = FREQUENCY_ENCODE(2489.02f),
	ADI_BEEP_FREQ_E7  = FREQUENCY_ENCODE(2637.02f),
	ADI_BEEP_FREQ_F7  = FREQUENCY_ENCODE(2793.83f),
	ADI_BEEP_FREQ_Fs7 = FREQUENCY_ENCODE(2959.96f),
	ADI_BEEP_FREQ_G7  = FREQUENCY_ENCODE(3135.96f),
	ADI_BEEP_FREQ_Gs7 = FREQUENCY_ENCODE(3322.44f),
	ADI_BEEP_FREQ_A7  = FREQUENCY_ENCODE(3520.00f),
	ADI_BEEP_FREQ_As7 = FREQUENCY_ENCODE(3729.31f),
	ADI_BEEP_FREQ_B7  = FREQUENCY_ENCODE(3951.07f),

	ADI_BEEP_FREQ_C8  = FREQUENCY_ENCODE(4186.01f),
	ADI_BEEP_FREQ_Cs8 = FREQUENCY_ENCODE(4434.92f),
	ADI_BEEP_FREQ_D8  = FREQUENCY_ENCODE(4698.64f),
	ADI_BEEP_FREQ_Ds8 = FREQUENCY_ENCODE(4978.03f),
} ADI_BEEP_NOTE_FREQUENCY_TYPE;

#define ADI_BEEP_DUR_ZERO      (0)                       /*!<  Beeper zero tone duration value          */
#define ADI_BEEP_DUR_MIN       (1)                       /*!<  Beeper minimum tone duration value       */
#define ADI_BEEP_DUR_MAX       (254)                     /*!<  Beeper maximum tone duration value       */
#define ADI_BEEP_DUR_INFINATE  (255)                     /*!<  Beeper infinate tone duration value      */

#define DURATION_ENCODE(x) (uint8_t)((float)ADI_BEEP_DUR_MAX/(float)x + 0.5f)    /*!<  Beeper tone duration encoder macro      */

/*!
 * \enum ADI_BEEP_NOTE_DURATION_TYPE
 *  @brief  Beeper tone duration list.
 *  @details List of possible Beeper tone durations.
 */
typedef enum {
	ADI_BEEP_DUR_0         = ADI_BEEP_DUR_ZERO,        /* stop                       */
	ADI_BEEP_DUR_1         = DURATION_ENCODE(1),       /* whole note (1.016 seconds) */
	ADI_BEEP_DUR_2         = DURATION_ENCODE(2),       /* half note                  */
	ADI_BEEP_DUR_3_8       = DURATION_ENCODE(8*3),     /* three eights note          */
	ADI_BEEP_DUR_4         = DURATION_ENCODE(4),       /* one quarter note           */
	ADI_BEEP_DUR_3_16      = DURATION_ENCODE(16*3),    /* three sixteenth note       */
	ADI_BEEP_DUR_8         = DURATION_ENCODE(8),       /* one eighth note            */
	ADI_BEEP_DUR_16        = DURATION_ENCODE(16),      /* one sixteenth note         */
	ADI_BEEP_DUR_32        = DURATION_ENCODE(32),      /* one thirty-secondth note   */
	ADI_BEEP_DUR_N         = ADI_BEEP_DUR_INFINATE,    /* continuous play            */
} ADI_BEEP_NOTE_DURATION_TYPE;



/*================ E X T E R N A L S ==================*/


/*
 * Beeper API
 */

extern ADI_BEEP_RESULT_TYPE adi_Beep_Init                   (ADI_BEEP_DEV_ID_TYPE const id, ADI_BEEP_HANDLE* const phDevice);
extern ADI_BEEP_RESULT_TYPE adi_Beep_UnInit                 (ADI_BEEP_HANDLE const hDevice);

extern ADI_BEEP_RESULT_TYPE adi_Beep_SetInterruptEnables    (ADI_BEEP_HANDLE const hDevice, ADI_BEEP_INTERRUPT_TYPE  const  Interrupts, bool_t const bFlag);
extern ADI_BEEP_RESULT_TYPE adi_Beep_GetInterruptEnables    (ADI_BEEP_HANDLE const hDevice, ADI_BEEP_INTERRUPT_TYPE* const pInterrupts);

extern ADI_BEEP_RESULT_TYPE adi_Beep_SetInterruptStatus     (ADI_BEEP_HANDLE const hDevice, ADI_BEEP_INTERRUPT_TYPE  const  Interrupts);
extern ADI_BEEP_RESULT_TYPE adi_Beep_GetInterruptStatus     (ADI_BEEP_HANDLE const hDevice, ADI_BEEP_INTERRUPT_TYPE* const pInterrupts);
extern ADI_BEEP_RESULT_TYPE adi_Beep_GetBusyStatus          (ADI_BEEP_HANDLE const hDevice, bool_t* const pbFlag);

extern ADI_BEEP_RESULT_TYPE adi_Beep_SetDeviceEnable        (ADI_BEEP_HANDLE const hDevice, bool_t  const  bFlag);
extern ADI_BEEP_RESULT_TYPE adi_Beep_GetDeviceEnable        (ADI_BEEP_HANDLE const hDevice, bool_t* const pbFlag);

extern ADI_BEEP_RESULT_TYPE adi_Beep_SetToneEnable          (ADI_BEEP_HANDLE const hDevice, ADI_BEEP_TONE_TYPE const channel, bool_t  const  bFlag);
extern ADI_BEEP_RESULT_TYPE adi_Beep_GetToneEnable          (ADI_BEEP_HANDLE const hDevice, ADI_BEEP_TONE_TYPE const channel, bool_t* const pbFlag);

extern ADI_BEEP_RESULT_TYPE adi_Beep_SetTone                (ADI_BEEP_HANDLE const hDevice, ADI_BEEP_TONE_TYPE const channel, ADI_BEEP_NOTE_FREQUENCY_TYPE  const  Frequency, ADI_BEEP_NOTE_DURATION_TYPE  const  Duration);
extern ADI_BEEP_RESULT_TYPE adi_Beep_GetTone                (ADI_BEEP_HANDLE const hDevice, ADI_BEEP_TONE_TYPE const channel, ADI_BEEP_NOTE_FREQUENCY_TYPE* const pFrequency, ADI_BEEP_NOTE_DURATION_TYPE* const pDuration);

extern ADI_BEEP_RESULT_TYPE adi_Beep_SetSequence            (ADI_BEEP_HANDLE const hDevice, uint8_t  const  Count);
extern ADI_BEEP_RESULT_TYPE adi_Beep_GetSequence            (ADI_BEEP_HANDLE const hDevice, uint8_t* const pCount);

extern ADI_BEEP_RESULT_TYPE adi_Beep_RegisterCallback       (ADI_BEEP_HANDLE const hDevice, ADI_CALLBACK const cbFunc, uint32_t const cbWatch);

/* C++ linkage */
#ifdef __cplusplus
}
#endif

#endif /* include guard */

/*
** EOF
*/

/*@}*/
