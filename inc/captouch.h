/*********************************************************************************

Copyright (c) 2012-2014 Analog Devices, Inc. All Rights Reserved.

This software is proprietary to Analog Devices, Inc. and its licensors.  By using 
this software you agree to the terms of the associated Analog Devices Software 
License Agreement.

*********************************************************************************/

/*!
 ******************************************************************************
 * @file:    captouch.h
 * @brief:   Captouch Definitions for ADuCM350
 * @version: $Revision: 28525 $
 * @date:    $Date: 2014-11-12 14:51:26 -0500 (Wed, 12 Nov 2014) $
*****************************************************************************/

/*! \addtogroup CT_Driver
 *  @{
 */

#ifndef __CT_H__
#define __CT_H__

#include "device.h"
#include "adi_ct_config.h"

/* C++ linkage */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


  /* Button bits defs */

#define ADI_CT_STATUS_BIT_A      (1 << 0)       /*!<  Button A */
#define ADI_CT_STATUS_BIT_B      (1 << 1)       /*!<  Button B */
#define ADI_CT_STATUS_BIT_C      (1 << 2)       /*!<  Button C */
#define ADI_CT_STATUS_BIT_D      (1 << 3)       /*!<  Button D */
#define ADI_CT_STATUS_BIT_E      (1 << 4)       /*!<  Button E */
#define ADI_CT_STATUS_BIT_F      (1 << 5)       /*!<  Button F */

/* Default Configuration Macros */

/*! \cond PRIVATE */


#define ADI_CT_DEFAULT_BASELINE_VALUE          (11000u)    /*!<  11000 codes  */
#define ADI_CT_ODD_STAGE                       (0x01)

/*! \endcond */


 /*!
 *  \enum ADI_CT_DEV_ID_TYPE
 *
 *  CT device identifiers
 */

typedef enum
{
    ADI_CT_DEV_ID_0 =0,                               /*!< CT Device ID 0  */
    ADI_CT_DEV_ID_MAXNUM,
}ADI_CT_DEV_ID_TYPE;


 /*!
 *  \enum ADI_CT_DRV_STATE_TYPE
 *
 *  CT driver state
 */

typedef enum
{
    ADI_CT_DRV_STATE_UNKNOWN = 0,                    /*!< CT  is not initialized    */
    ADI_CT_DRV_STATE_INITIALIZED,                    /*!< CT driver initialized     */
    ADI_CT_DRV_STATE_READY                           /*!< CT driver is ready        */
} ADI_CT_DRV_STATE_TYPE;


/*!
 * \enum ADI_CT_RESULT_TYPE
 *
 *  CT result codes
 */

typedef enum
{

    ADI_CT_RESULT_SUCCESS = 0,                                  /*!< generic success           */
    ADI_CT_RESULT_ERR     = ADI_DEV_CT_ERROR_OFFSET,            /*!< generic error             */
    ADI_CT_RESULT_ERR_CALLBACK_OVERWRITE,                       /*!< Callback Overwrite        */
    ADI_CT_RESULT_ERR_TCH_REL_NOT_ENABLE,                       /*!< Touch And Release Detection not enable   */

} ADI_CT_RESULT_TYPE;



/*!
 * \enum ADI_CT_INTERRUPT_TYPE
 *
 *  CT Interrupt Types
 */

typedef enum
{
    ADI_CT_INTERRUPT_NONE,                                                                      /*!< No Interrupt                  */
    ADI_CT_INTERRUPT_CONV_SET_COMPLETE     = 1 << BITP_CT_CT_IEN_CONV_SET_COMPLETE_IEN,         /*!< Conversion Complete Interrupt */
    ADI_CT_INTERRUPT_STATUS_GT_THRESHOLD   = 1 << BITP_CT_CT_IEN_STATUS_GT_THRESHOLD_IEN,       /*!< Thresholds Interrupt          */
    ADI_CT_INTERRUPT_PROX_DETECTED         = 1 << BITP_CT_CT_IEN_PROX_DETECTED_IEN,             /*!< Proximity Interrupt           */
    ADI_CT_INTERRUPT_TOUCH_DETECTED        = 1 << BITP_CT_CT_IEN_TOUCH_DETECTED_IEN,            /*!< Touch Interrupt               */
    ADI_CT_INTERRUPT_RELEASE_DETECTED      = 1 << BITP_CT_CT_IEN_RELEASE_DETECTED_IEN,          /*!< Release Interrupt             */

}ADI_CT_INTERRUPT_TYPE;

#define ADI_CT_INTERRUPT_MASK          (ADI_CT_INTERRUPT_CONV_SET_COMPLETE      \
                                       | ADI_CT_INTERRUPT_STATUS_GT_THRESHOLD   \
                                       | ADI_CT_INTERRUPT_PROX_DETECTED         \
                                       | ADI_CT_INTERRUPT_TOUCH_DETECTED        \
                                       | ADI_CT_INTERRUPT_RELEASE_DETECTED      \      /*!< interrupt status mask */




 /*!
 * \enum ADI_CT_STAGE_NUMBER_TYPE
 *
 *  CT Stage Number Type
 *
 *  Used to select the stage number to set or get each Stage parameter
 *  through the adi_CT_SetStage*() or adi_CT_GetStage*() API.
 *  It can be also used with the read data functions like #adi_CT_GetCDCValue().
 */


typedef enum
{
    ADI_CT_STAGE0_NUMBER,           /*!< Stage 0 */
    ADI_CT_STAGE1_NUMBER,           /*!< Stage 1 */
    ADI_CT_STAGE2_NUMBER,           /*!< Stage 2 */
    ADI_CT_STAGE3_NUMBER,           /*!< Stage 3 */
    ADI_CT_STAGE4_NUMBER,           /*!< Stage 4 */
    ADI_CT_STAGE5_NUMBER,           /*!< Stage 5 */
    ADI_CT_STAGE6_NUMBER,           /*!< Stage 6 */
    ADI_CT_STAGE7_NUMBER,           /*!< Stage 7 */
    ADI_CT_STAGE8_NUMBER,           /*!< Stage 8 */
    ADI_CT_STAGE9_NUMBER,           /*!< Stage 9 */
    ADI_CT_STAGE10_NUMBER,          /*!< Stage 10 */
    ADI_CT_STAGE11_NUMBER,          /*!< Stage 11 */
    ADI_CT_STAGE12_NUMBER,          /*!< Stage 12 */
    ADI_CT_STAGE13_NUMBER,          /*!< Stage 13 */
    ADI_CT_STAGE14_NUMBER,          /*!< Stage 14 */
    ADI_CT_STAGE15_NUMBER,          /*!< Stage 15 */
    ADI_CT_STAGE_NON_VALID_STAGE,   /*!< Non valid stage*/
} ADI_CT_STAGE_NUMBER_TYPE;



/*!
 * \enum ADI_CT_STAGE_BIT_TYPE
 *
 * CT Stage Bit Type. Used in functions whose task is to select an specific stage to be
 * enabled,disabled or calibrated. It is used with #adi_CT_SetStagesEnable() and
 * #adi_CT_SetStagesCalibrationEnable() API.
 *
 */


typedef enum
{
    ADI_CT_STAGE0_BIT  = 1 << ADI_CT_STAGE0_NUMBER,         /*!< Stage 0 */
    ADI_CT_STAGE1_BIT  = 1 << ADI_CT_STAGE1_NUMBER,         /*!< Stage 1 */
    ADI_CT_STAGE2_BIT  = 1 << ADI_CT_STAGE2_NUMBER,         /*!< Stage 2 */
    ADI_CT_STAGE3_BIT  = 1 << ADI_CT_STAGE3_NUMBER,         /*!< Stage 3 */
    ADI_CT_STAGE4_BIT  = 1 << ADI_CT_STAGE4_NUMBER,         /*!< Stage 4 */
    ADI_CT_STAGE5_BIT  = 1 << ADI_CT_STAGE5_NUMBER,         /*!< Stage 5 */
    ADI_CT_STAGE6_BIT  = 1 << ADI_CT_STAGE6_NUMBER,         /*!< Stage 6 */
    ADI_CT_STAGE7_BIT  = 1 << ADI_CT_STAGE7_NUMBER,         /*!< Stage 7 */
    ADI_CT_STAGE8_BIT  = 1 << ADI_CT_STAGE8_NUMBER,         /*!< Stage 8 */
    ADI_CT_STAGE9_BIT  = 1 << ADI_CT_STAGE9_NUMBER,         /*!< Stage 9 */
    ADI_CT_STAGE10_BIT = 1 << ADI_CT_STAGE10_NUMBER,        /*!< Stage 10 */
    ADI_CT_STAGE11_BIT = 1 << ADI_CT_STAGE11_NUMBER,        /*!< Stage 11 */
    ADI_CT_STAGE12_BIT = 1 << ADI_CT_STAGE12_NUMBER,        /*!< Stage 12 */
    ADI_CT_STAGE13_BIT = 1 << ADI_CT_STAGE13_NUMBER,        /*!< Stage 13 */
    ADI_CT_STAGE14_BIT = 1 << ADI_CT_STAGE14_NUMBER,        /*!< Stage 14 */
    ADI_CT_STAGE15_BIT = 1 << ADI_CT_STAGE15_NUMBER,        /*!< Stage 15 */
} ADI_CT_STAGE_BIT_TYPE;


  /*!
 *  \enum ADI_CT_INPUT_TYPE
 *
 *  CT Input select. Used to connect a specific button (input) to a specific stage.
 *  It is used with #adi_CT_SetStageSelectInput() API.
 *
 */


typedef enum
{
    ADI_CT_INPUT_A     = 0,         /*!<  Button A */
    ADI_CT_INPUT_B     = 1,         /*!<  Button B */
    ADI_CT_INPUT_C     = 2,         /*!<  Button C */
    ADI_CT_INPUT_D     = 3,         /*!<  Button D */
    ADI_CT_INPUT_E     = 4,         /*!<  Button E */
    ADI_CT_INPUT_F     = 5,         /*!<  Button F */

}ADI_CT_INPUT_TYPE;


/*!
 *  \enum ADI_CT_CDC_RESULT_TYPE
 *
 *  CT CDC Result Type. Used to set the CDC value format stored in CT_CDC_RESx
 *  registers. It is used with #adi_CT_SetCDCResultFormat() API.
 */

typedef enum
{
    ADI_CT_CDC_RESULT_VALUE,                   /*!< CDC Value */
    ADI_CT_CDC_BASELINE_DIFFERENCE_VALUE,      /*!< CDC Value - Baseline Value */

}ADI_CT_CDC_RESULT_TYPE;


  /*!
 *  \enum ADI_CT_INTCYCLES_TYPE
 *
 *  CT Integration Cycles Type. Used to set the number of Integration Cycles in
 *  the C2V conversion. It is used with #adi_CT_SetStageC2VRange() API.
 */

typedef enum
{
    ADI_CT_INT_CYCLES_1,               /*!< 1 Integration Cycle */
    ADI_CT_INT_CYCLES_2,               /*!< 2 Integration Cycles */
    ADI_CT_INT_CYCLES_4,               /*!< 4 Integration Cycles */
    ADI_CT_INT_CYCLES_8,               /*!< 8 Integration Cycles */
    ADI_CT_INT_CYCLES_16,              /*!< 16 Integration Cycles */

}ADI_CT_INTCYCLES_TYPE;

  /*!
 *  \enum ADI_CT_UNMATCHED_PADS_TYPE
 *
 *  CT Unmatched Pads Type. Used to set the configuration of the Unmatched Pads.
 *  It is used with #adi_CT_SetCDCResultFormat() API.
 */

typedef enum
{
    ADI_CT_UNMATCHED_PADS_AC_DRV,             /*!< Pads to AC drive */
    ADI_CT_UNMATCHED_PADS_DC_DRV,             /*!< Pads to DC drive */
    ADI_CT_UNMATCHED_PADS_GND,                /*!< Pads to GND      */
    ADI_CT_UNMATCHED_PADS_FLOATED,            /*!< Pads Floated     */

}ADI_CT_UNMATCHED_PADS_TYPE;


 /*!
 *  \enum ADI_CT_AUTOZERODUTY_TYPE
 *
 *  CT Autozero Duty Type. Used to set the Autozero Duty Time in C2V conversion.
 *  This type of data is included in the #ADI_CT_C2V_TIMING_TYPE struct, which is used
 *  to configure the C2V conversion Timing. It is used with #adi_CT_SetTimingsCfg() API.
 */

typedef enum
{
    ADI_CT_AUTOZERO_DUTY_2us,          /*!< 2us */
    ADI_CT_AUTOZERO_DUTY_3us,          /*!< 3us */
    ADI_CT_AUTOZERO_DUTY_4us,          /*!< 4us */
    ADI_CT_AUTOZERO_DUTY_5us,          /*!< 5us */
    ADI_CT_AUTOZERO_DUTY_6us,          /*!< 6us */
    ADI_CT_AUTOZERO_DUTY_7us,          /*!< 7us */
    ADI_CT_AUTOZERO_DUTY_8us,          /*!< 8us */
    ADI_CT_AUTOZERO_DUTY_9us,          /*!< 9us */

}ADI_CT_AUTOZERODUTY_TYPE;



  /*!
 *  \enum ADI_CT_PGA_GAIN_TYPE
 *
 *  CT PGA Type. Used to set the PGA GAIN.
 *  It is used with #adi_CT_SetCDCResultFormat() API.
 */

typedef enum
{
    ADI_CT_PGA_GAIN_2,     /*!< GAIN of 2 */
    ADI_CT_PGA_GAIN_4,     /*!< GAIN of 4 */
    ADI_CT_PGA_GAIN_8,     /*!< GAIN of 8 */
    ADI_CT_PGA_GAIN_16,    /*!< GAIN of 16 */

}ADI_CT_PGA_GAIN_TYPE;


 /*!
 *  \enum ADI_CT_C2VRANGE_TYPE
 *
 *  CT C2V Range Type. Used to set the C2V Conversion Range for each Stage.
 *  It is used with #adi_CT_SetStageC2VRange() API.
 */

typedef enum
{
    ADI_CT_C2VRANGE_0_58pF,       /*!< 0.58pF  */
    ADI_CT_C2VRANGE_1_17pF,       /*!< 1.17pF  */
    ADI_CT_C2VRANGE_1_75pF,       /*!< 1.75pF  */
    ADI_CT_C2VRANGE_2_33pF,       /*!< 2.33pF  */
    ADI_CT_C2VRANGE_2_91pF,       /*!< 2.91pF  */
    ADI_CT_C2VRANGE_3_49pF,       /*!< 3.49pF  */
    ADI_CT_C2VRANGE_4_07pF,       /*!< 4.07pF  */
    ADI_CT_C2VRANGE_4_65pF,       /*!< 4.65pF  */
    ADI_CT_C2VRANGE_5_23pF,       /*!< 5.23pF  */
    ADI_CT_C2VRANGE_5_81pF,       /*!< 5.81pF  */
    ADI_CT_C2VRANGE_6_39pF,       /*!< 6.39pF  */
    ADI_CT_C2VRANGE_6_97pF,       /*!< 6.97pF  */
    ADI_CT_C2VRANGE_7_55pF,       /*!< 7.55pF  */
    ADI_CT_C2VRANGE_8_13pF,       /*!< 8.13pF  */
    ADI_CT_C2VRANGE_8_74pF,       /*!< 8.74pF  */
    ADI_CT_C2VRANGE_9_32pF,       /*!< 9.32pF  */

}ADI_CT_C2VRANGE_TYPE;

 /*!
 *  \enum ADI_CT_TCH_REL_INPUT_TYPE
 *
 *  CT Touch And Release Input Type. Used to select the Inputs in the Touch and
 *  Release Detection Algorithm. This type of data is included in the
 *  #ADI_CT_TOUCH_AND_RELEASE_TYPE struct, which is used to configure the Touch and
 *  Release Detection Algorithm. It is used with #adi_CT_SetTouchCfg() and
 *  #adi_CT_SetReleaseCfg() API.
 */


typedef enum
{
    ADI_CT_TCH_REL_INPUT_ALL_ZEROS,                  /*!< Load All Zeros  */
    ADI_CT_TCH_REL_INPUT_CDC_VALUE,                  /*!< CDC Value  */
    ADI_CT_TCH_REL_INPUT_SLOW_FILTER_OUTPUT,         /*!< Slow Filter Output */
    ADI_CT_TCH_REL_INPUT_FAST_FILTER_OUTPUT,         /*!< Fast Filter Output */
    ADI_CT_TCH_REL_INPUT_AMBIENT_VALUE,              /*!< Load Ambient Value */

}ADI_CT_TCH_REL_INPUT_TYPE;

  /*!
 *  \enum ADI_CT_TCH_REL_EVENT_ON_HIGH_INPUT_TYPE
 *
 *  CT Touch And Release Rising Edge Detection Type. Used to select the type of Rising Edge
 *  Detection type. The user can select between a positive or a negative difference.
 *  This type of data is included in the #ADI_CT_TOUCH_AND_RELEASE_TYPE struct,
 *  which is used to configure the Touch and Release Detection Algorithm.
 *  It is used with #adi_CT_SetTouchCfg() and #adi_CT_SetReleaseCfg() API.
 */

typedef enum
{
    ADI_CT_TCH_REL_EVENT_ON_HIGH_INPUT_INA_INB_DIFF_LESSER_THR,            /*!< Active if inA-inB < Thrld for a MinDuration */
    ADI_CT_TCH_REL_EVENT_ON_HIGH_INPUT_INA_INB_DIFF_GREATER_THR,           /*!< Active if inA-inB > Thrld for a MinDuration */

}ADI_CT_TCH_REL_EVENT_ON_HIGH_INPUT_TYPE;


 /*!
 *  \enum ADI_CT_INDIVIDUAL_THR_ENABLE_TYPE
 *
 *  CT Individual Threshold Type. Used to select which register is used to set the
 *  Thresholds in the Touch and Release Detection Algorithm. The user can select
 *  between a positive or a negative difference.
 *  This type of data is included in the #ADI_CT_TOUCH_AND_RELEASE_TYPE struct,
 *  which is used to configure the Touch and Release Detection Algorithm.
 *  It is used with #adi_CT_SetTouchCfg() and #adi_CT_SetReleaseCfg() API.
 */

typedef enum
{
    ADI_CT_INDIVIDUAL_SENSOR_THR_CFGx_DISABLE,         /*!< Use CT_SENSOR_THR_CFGx for sensor Threshold */
    ADI_CT_INDIVIDUAL_SENSOR_THR_CFGx_ENABLE,          /*!< Use CT_TOUCH_CFG1 and CT_RELEASE_CFG1 for sensor Threshold */

}ADI_CT_INDIVIDUAL_THR_ENABLE_TYPE;



 /*!
 *  \enum ADI_CT_RANKING_TYPE
 *
 *  CT Raking Type. Used to set the number of samples used in the Ranking Filter
 *  block. This type of data is included in the #ADI_CT_AVERAGE_TYPE struct,
 *  which is used to configure the Average Filter Block.
 *  It is used with #adi_CT_SetAverageCfg() API.
 */

typedef enum
{
    ADI_CT_RANKING_NO_RANKING,                /*!< No Ranking */
    ADI_CT_RANKING_4_SAMPLES,                 /*!< 4 samples */
    ADI_CT_RANKING_8_SAMPLES,                 /*!< 8 samples */
    ADI_CT_RANKING_16_SAMPLES,                /*!< 16 samples */
    ADI_CT_RANKING_32_SAMPLES,                /*!< 32 samples */

}ADI_CT_RANKING_TYPE;


 /*!
 *  \enum ADI_CT_AVG_OUTPUT_TYPE
 *
 *  CT Average Output Type. Used to select the final average value.
 *  This type of data is included in the #ADI_CT_AVERAGE_TYPE struct,
 *  which is used to configure the Average Filter Block.
 *  It is used with #adi_CT_SetAverageCfg() API.
 */

typedef enum
{
    ADI_CT_AVG_OUTPUT_NO_AVG,                 /*!< No Averaging */
    ADI_CT_AVG_OUTPUT_GROUP1,                 /*!< Average of Group 1 */
    ADI_CT_AVG_OUTPUT_GROUP2,                 /*!< Average of Group 2 */
    ADI_CT_AVG_OUTPUT_GROUP1_2,               /*!< Average of average of Group 1 and average of Group 2 */

}ADI_CT_AVG_OUTPUT_TYPE;

 /*!
 *  \enum ADI_CT_AVG_SAMPLES_TYPE
 *
 *  CT Average Samples Type. Used to set the number of samples in Average Group 1
 *  and Average Group 2. This type of data is included in the #ADI_CT_AVERAGE_TYPE struct,
 *  which is used to configure the Average Filter Block.
 *  It is used with #adi_CT_SetAverageCfg() API.
 */

typedef enum
{
    ADI_CT_AVG_SAMPLES_NO_AVG,                 /*!< No Samples */
    ADI_CT_AVG_SAMPLES_2_SAMPLES,              /*!< 2 Samples */
    ADI_CT_AVG_SAMPLES_4_SAMPLES,              /*!< 4 Samples */
    ADI_CT_AVG_SAMPLES_8_SAMPLES,              /*!< 8 Samples */
    ADI_CT_AVG_SAMPLES_16_SAMPLES,             /*!< 16 Samples */
    ADI_CT_AVG_SAMPLES_32_SAMPLES,             /*!< 32 Samples */

}ADI_CT_AVG_SAMPLES_TYPE;

 /*!
 *  \enum ADI_CT_BASELINE_CAL_DELAY_TYPE
 *
 *  CT Baseline Calibration Delay Type. Used to set the time to update the baseline
 *  once the proximity is cleared. This type of data is included in the #ADI_CT_BASELINE_TYPE
 *  struct, which is used to configure the Baseline Features.
 *  It is used with #adi_CT_SetBaselineCfg() API.
 */

typedef enum
{
    ADI_CT_BASELINE_CAL_DELAY_FAST_FILTER_UPDATE_8,               /*!< 8  x  FAST FILTER UPDATE RATE*/
    ADI_CT_BASELINE_CAL_DELAY_FAST_FILTER_UPDATE_16,              /*!< 16 x  FAST FILTER UPDATE RATE*/
    ADI_CT_BASELINE_CAL_DELAY_FAST_FILTER_UPDATE_24,              /*!< 24 x  FAST FILTER UPDATE RATE*/
    ADI_CT_BASELINE_CAL_DELAY_FAST_FILTER_UPDATE_32,              /*!< 32 x  FAST FILTER UPDATE RATE*/
    ADI_CT_BASELINE_CAL_DELAY_FAST_FILTER_UPDATE_40,              /*!< 40 x  FAST FILTER UPDATE RATE*/
    ADI_CT_BASELINE_CAL_DELAY_FAST_FILTER_UPDATE_48,              /*!< 48 x  FAST FILTER UPDATE RATE*/
    ADI_CT_BASELINE_CAL_DELAY_FAST_FILTER_UPDATE_56,              /*!< 56 x  FAST FILTER UPDATE RATE*/
    ADI_CT_BASELINE_CAL_DELAY_FAST_FILTER_UPDATE_64,              /*!< 64 x  FAST FILTER UPDATE RATE*/

}ADI_CT_BASELINE_CAL_DELAY_TYPE;

 /*!
 *  \enum ADI_CT_FAST_FILTER_UPDATE_TYPE
 *
 *  CT Fast Filter Update Type. Used to set the update rate for the baseline
 *  calculation fast filter. This type of data is included in the #ADI_CT_BASELINE_TYPE
 *  struct, which is used to configure the Baseline Features.
 *  It is used with #adi_CT_SetBaselineCfg() API.
 */

typedef enum
{
    ADI_CT_FAST_FILTER_UPDATE_2ND_CONVERSION_CYCLE,      /*!< New data every 2nd conversion cycle*/
    ADI_CT_FAST_FILTER_UPDATE_4TH_CONVERSION_CYCLE,      /*!< New data every 4th conversion cycle*/
    ADI_CT_FAST_FILTER_UPDATE_6TH_CONVERSION_CYCLE,      /*!< New data every 6th conversion cycle*/
    ADI_CT_FAST_FILTER_UPDATE_8TH_CONVERSION_CYCLE,      /*!< New data every 8th conversion cycle*/
    ADI_CT_FAST_FILTER_UPDATE_10TH_CONVERSION_CYCLE,     /*!< New data every 10th conversion cycle*/
    ADI_CT_FAST_FILTER_UPDATE_12TH_CONVERSION_CYCLE,     /*!< New data every 12th conversion cycle*/
    ADI_CT_FAST_FILTER_UPDATE_14TH_CONVERSION_CYCLE,     /*!< New data every 14th conversion cycle*/
    ADI_CT_FAST_FILTER_UPDATE_16TH_CONVERSION_CYCLE,     /*!< New data every 16th conversion cycle*/

}ADI_CT_FAST_FILTER_UPDATE_TYPE;


 /*!
 *  \enum ADI_CT_INT_BUFFER_BIAS_TYPE
 *
 *  CT Internal Buffer Bias Current Type. Used to set the Internal Buffer Bias
 *  Current. It is used with #adi_CT_SetIntBufferBiasCurrent() API.
 */

typedef enum
{
    ADI_CT_INT_BUFFER_BIAS_2_5UA,      /*!< 2.5uA */
    ADI_CT_INT_BUFFER_BIAS_3UA,        /*!< 3uA   */
    ADI_CT_INT_BUFFER_BIAS_3_5UA,      /*!< 3.5uA */
    ADI_CT_INT_BUFFER_BIAS_4UA,        /*!< 4uA   */

}ADI_CT_INT_BUFFER_BIAS_TYPE;


 /*!
 *  \enum ADI_CT_PGA_BIAS_TYPE
 *
 *  CT PGA Bias Current Type. Used to set the PGA Bias Current.
 *  It is used with #adi_CT_SetPGABiasCurrent() API.
 */

typedef enum
{
    ADI_CT_PGA_BIAS_3UA,        /*!< 3uA   */
    ADI_CT_PGA_BIAS_3_5UA,      /*!< 3.5uA */
    ADI_CT_PGA_BIAS_4UA,        /*!< 4uA   */
    ADI_CT_PGA_BIAS_4_5UA,      /*!< 4.5uA */
    ADI_CT_PGA_BIAS_5UA,        /*!< 5uA   */
    ADI_CT_PGA_BIAS_5_5UA,      /*!< 5.5uA */
    ADI_CT_PGA_BIAS_6UA,        /*!< 6uA   */
    ADI_CT_PGA_BIAS_6_5UA,      /*!< 6.5uA */

}ADI_CT_PGA_BIAS_TYPE;


 /*!
 *  \enum ADI_CT_C2V_BIAS_TYPE
 *
 *  CT C2V Conversion Bias Current Type. Used to set the C2V Conversion Bias
 *  Current.It is used with #adi_CT_SetC2VBiasCurrent() API.
 */

typedef enum
{
    ADI_CT_C2V_BIAS_6UA,   /*!< 6uA */
    ADI_CT_C2V_BIAS_7UA,   /*!< 7uA */
    ADI_CT_C2V_BIAS_8UA,   /*!< 8uA */
    ADI_CT_C2V_BIAS_9UA,   /*!< 9uA */
    ADI_CT_C2V_BIAS_10UA,  /*!< 10uA */
    ADI_CT_C2V_BIAS_11UA,  /*!< 11uA */
    ADI_CT_C2V_BIAS_12UA,  /*!< 12uA */
    ADI_CT_C2V_BIAS_13UA,  /*!< 13uA */

}ADI_CT_C2V_BIAS_TYPE;

 /*!
 *  \enum ADI_CT_PK2PK_AVG_TYPE
 *
 *  CT PK2PK AVG Samples Type. Used to set number of samples to be averaged
 *  when calculating CDC Value during Peak-to-Peak noise measurements stages.
 *  This type of data is included in the #ADI_CT_PK2PK_TYPE struct,
 *  which is used to configure the PK2PK measurements Features.
 *  It is used with #adi_CT_SetPK2PKCfg() API.
 */

typedef enum
{
    ADI_CT_PK2PK_AVG_2_SAMPLES,        /*!< 2 samples */
    ADI_CT_PK2PK_AVG_4_SAMPLES,        /*!< 4 samples */
    ADI_CT_PK2PK_AVG_8_SAMPLES,        /*!< 8 samples */
    ADI_CT_PK2PK_AVG_16_SAMPLES,       /*!< 16 samples */
    ADI_CT_PK2PK_AVG_32_SAMPLES,       /*!< 32 samples */

}ADI_CT_PK2PK_AVG_TYPE;

/*!
 *  \enum ADI_CT_PK2PK_NUM_TYPE
 *
 *  CT PK2PK Number of Samples Type. Used to set number of samples for Peak-to-Peak
 *  noise calculations. This type of data is included in the #ADI_CT_PK2PK_TYPE
 *  struct, which is used to configure the PK2PK measurements Features.
 *  It is used with #adi_CT_SetPK2PKCfg() API.
 */

typedef enum
{
    ADI_CT_PK2PK_NUM_4_SAMPLES,        /*!< 4 samples */
    ADI_CT_PK2PK_NUM_8_SAMPLES,        /*!< 8 samples */
    ADI_CT_PK2PK_NUM_16_SAMPLES,       /*!< 16 samples */
    ADI_CT_PK2PK_NUM_32_SAMPLES,       /*!< 32 samples */

}ADI_CT_PK2PK_NUM_TYPE;


/*!
 * \struct ADI_CT_C2V_TIMING_TYPE
 *
 *  CT Timing Settings Structure. The Timing data type struct is used with the
 *  #adi_CT_SetTimingsCfg() and #adi_CT_GetTimingsCfg() APIs to set or get the
 *  timing conversion settings.
 *             AutozeroDuty        #ADI_CT_AUTOZERODUTY_TYPE
 *             C2VHoldTime         Capaticitance to Voltage Time. 5 bits, each LSB represents 0.25us, range [0.25-8]us.
 *             Phase13             Phase13. 5 bits, each LSB represents 0.25us, range [0.25-8]us.
 *             Phase24             Phase24. 8 bits, each LSB represents 0.25us, range [0.0625-16]us.
 *
 */


typedef struct
{
  ADI_CT_AUTOZERODUTY_TYPE AutozeroDuty;        /*!< Autozero Duty  */
  uint8_t C2VHoldTime;                          /*!< C2VHoldTime    */
  uint8_t Phase13;                              /*!< Phase13        */
  uint8_t Phase24;                              /*!< Phase24        */

} ADI_CT_C2V_TIMING_TYPE;



/*!
 * \struct ADI_CT_PK2PK_TYPE
 *
 *  CT PK2PK measurements configuration structure. The PK2PK data type struct is used with the
 *  #adi_CT_SetPK2PKCfg() and #adi_CT_GetPK2PKCfg() APIs to set or get the
 *  PK2PK measurements settings.
 */

typedef struct
{
  ADI_CT_PK2PK_AVG_TYPE 	PK2PKAvg;               /*!< Number of Samples to Average         */
  uint8_t 			PK2PKAvgMin;            /*!< Minimum Rank position for Averaging  */
  uint8_t 		        PK2PKSubset;            /*!< PK2PKSubset                          */
  ADI_CT_PK2PK_NUM_TYPE 	PK2PKNumSamples;        /*!< PK2PKNumSamples                      */

} ADI_CT_PK2PK_TYPE;


/*!
 * \struct ADI_CT_AVERAGE_TYPE
 *
 *  CT Average configuration structure. The Average data type struct is used with the
 *  #adi_CT_SetAverageCfg() and #adi_CT_GetAverageCfg() APIs to set or get the
 *  Average settings.
 */

typedef struct
{
  ADI_CT_RANKING_TYPE       RankFilt;                 /*!< Number of Samples to be Ranked  */
  ADI_CT_AVG_OUTPUT_TYPE    AvgType;                  /*!< Select the Final Average Value  */
  ADI_CT_AVG_SAMPLES_TYPE   AvgGroup1;                /*!< Number of Samples in Average Group1*/
  ADI_CT_AVG_SAMPLES_TYPE   AvgGroup2;                /*!< Number of Samples in Average Group2*/
  uint8_t                   AvgMinGroup1;             /*!< Minimum rank position to be Average in Group1*/
  uint8_t                   AvgMinGroup2;             /*!< Minimum rank position to be Average in Group2*/

} ADI_CT_AVERAGE_TYPE;



/*!
 * \struct ADI_CT_BASELINE_TYPE
 *
 *  CT Baseline configuration structure. The Baseline data type struct is used with the
 *  #adi_CT_SetBaselineCfg() and #adi_CT_GetBaselineCfg() APIs to set or get the
 *  Average settings.
 */

typedef struct
{
  ADI_CT_BASELINE_CAL_DELAY_TYPE 	BaselineCalDelay;      /*!< Delay before updating the Baseline  */
  ADI_CT_FAST_FILTER_UPDATE_TYPE 	FastFilterUpdate;      /*!< Update for the fast filter  */
  uint8_t 				FastProx;              /*!< Fast Proximity Value */
  uint8_t 				SlowProx;              /*!< Slow Proximity Value*/
  uint8_t 				BL_Coeff;              /*!< Slow Filter new sample Weight*/
  uint8_t 				CDC_Coeff;             /*!< Fast Filter new sample Weight*/

} ADI_CT_BASELINE_TYPE;


/*!
 * \struct ADI_CT_TOUCH_AND_RELEASE_TYPE
 *
 *  CT Touch And Release configuration Struct. The Touch and Release data type
 *  struct is used with the #adi_CT_SetTouchCfg(), #adi_CT_GetTouchCfg(),
 *  #adi_CT_SetReleaseCfg() and  #adi_CT_GetReleaseCfg() APIs to set or get
 *  the Touch and Release Detection Algorithm settings.
 */

typedef struct
{
  ADI_CT_INDIVIDUAL_THR_ENABLE_TYPE              IndividualThresholdEnable;    /*!< To use CT_SENSOR_THR_CFGx as threshold      */
  ADI_CT_TCH_REL_EVENT_ON_HIGH_INPUT_TYPE        EventOnHIgh;                  /*!< Type of rising edge detection               */
  ADI_CT_TCH_REL_INPUT_TYPE                      InputA;                       /*!< Input A  					*/
  ADI_CT_TCH_REL_INPUT_TYPE                      InputB;                       /*!< Input B 					*/
  uint16_t                                       UpperThreshold;               /*!< Upper Threshold				*/
  uint16_t                                       LowerThreshold;               /*!< Lower Threshold				*/
  uint16_t                                       MinDuration;                  /*!< Minimum duration to detect Touch/Release    */

} ADI_CT_TOUCH_AND_RELEASE_TYPE;



/*!
 * \struct ADI_CT_GENERIC_SETTINGS_TYPE
 *
 *  CT driver generic setting struct. The ADI_CT_GENERIC_SETTINGS_TYPE is used
 *  with the #adi_CT_SetGenericSettings() API
 *  to set or get the Generic settings entirely.
*/

typedef struct
{

  ADI_CT_C2V_TIMING_TYPE                C2V_TimingSettings;                    /*!< C2V Timming Settings Data  */
  ADI_CT_PK2PK_TYPE                     PK2PKSettings;						   /*!< PK2PK Settings Data        */
  ADI_CT_AVERAGE_TYPE                   AverageSettings;					   /*!< Average Settings Data      */
  ADI_CT_BASELINE_TYPE                  BaselineSettings;					   /*!< Baseline Settings Data     */
  ADI_CT_TOUCH_AND_RELEASE_TYPE         TouchSettings;					       /*!< Touch Settings Data        */
  ADI_CT_TOUCH_AND_RELEASE_TYPE         ReleaseSettings;					   /*!< Release Settings Data      */

} ADI_CT_GENERIC_SETTINGS_TYPE;


/*!
 * \struct ADI_STAGE_TYPE
 *
 *  Stage configuration Struct. The Stage type struct is used with
 *  the #adi_CT_SetStageGenericCfg() and #adi_CT_GetStageGenericCfg() APIs to set or get
 *  the Stage settings.
 */

typedef struct
{
  bool_t                        ByPassPGA;                      /*!<   Bypass PGA  		*/
  ADI_CT_PGA_GAIN_TYPE          PGA_Gain;                       /*!<   PGA Gain    		*/
  ADI_CT_C2VRANGE_TYPE          C2VRange;                       /*!<   C2V Range    		*/
  bool_t                        PK2PKMeasurements;              /*!<   Enable PK2PK Measurements*/
  bool_t                        DisableSignalExcitation;        /*!<   Disable Signal Excitation*/
  bool_t                        MAIN_DAC_Enable;                /*!<   CapDac Enable            */
  uint8_t                       MAIN_DAC_Value;                 /*!<   MAIN DAC Value  		*/
  uint8_t                       SUB_DAC_Value;                  /*!<   SUB DAC Value  		*/

} ADI_STAGE_TYPE;


/*!
 * @brief ADI_CT_DEV_HANDLE
 *
 *  CT device handle
 */

typedef struct ADI_CT_DEV_DATA_TYPE* ADI_CT_DEV_HANDLE;

/*!
 * @brief ADI_CT_CALLBACK_TYPE
 *
 *  CT callback type
 */
typedef void (*ADI_CT_CALLBACK_TYPE)(ADI_CT_INTERRUPT_TYPE const statusBits);


/* CT API prototypes */

extern ADI_CT_RESULT_TYPE       adi_CT_Init                               (ADI_CT_DEV_ID_TYPE   const   devID,
                                                                           ADI_CT_DEV_HANDLE*   const   phDevice);

extern ADI_CT_RESULT_TYPE       adi_CT_UnInit                             (ADI_CT_DEV_HANDLE    const   hDevice);

extern ADI_CT_RESULT_TYPE       adi_CT_SetEnable                          (ADI_CT_DEV_HANDLE    const   hDevice,
                                                                           bool_t               const   enable);

extern ADI_CT_RESULT_TYPE       adi_CT_SetPowerStatus                     (ADI_CT_DEV_HANDLE    const   hDevice,
                                                                           bool_t               const   enable);

extern ADI_CT_RESULT_TYPE       adi_CT_SetIntCycles                       (ADI_CT_DEV_HANDLE            const   hDevice,
                                                                           ADI_CT_INTCYCLES_TYPE        const   IntCycles);

extern ADI_CT_RESULT_TYPE       adi_CT_SetUnmatchedPads                   (ADI_CT_DEV_HANDLE            const   hDevice,
                                                                           ADI_CT_UNMATCHED_PADS_TYPE   const   UnmatchedPads);

extern ADI_CT_RESULT_TYPE       adi_CT_SetTimingsCfg                      (ADI_CT_DEV_HANDLE            const   hDevice,
                                                                           ADI_CT_C2V_TIMING_TYPE*      const   pTimingSettings);

extern ADI_CT_RESULT_TYPE       adi_CT_SetSelfTimerValue                  (ADI_CT_DEV_HANDLE            const   hDevice,
                                                                           uint16_t                     const   SelfTimerValue);

extern ADI_CT_RESULT_TYPE       adi_CT_SetSelfTimerEnable                 (ADI_CT_DEV_HANDLE            const   hDevice,
                                                                           bool_t                       const   enable);

extern ADI_CT_RESULT_TYPE       adi_CT_SetByPassGain                      (ADI_CT_DEV_HANDLE            const   hDevice,
                                                                           bool_t                       const   ByPassGain);

extern ADI_CT_RESULT_TYPE       adi_CT_SetC2VLowPassFilterEnable          (ADI_CT_DEV_HANDLE            const   hDevice,
                                                                           bool_t                       const   enable);

extern ADI_CT_RESULT_TYPE       adi_CT_SetIntBufferBiasCurrent            (ADI_CT_DEV_HANDLE            const   hDevice,
                                                                           ADI_CT_INT_BUFFER_BIAS_TYPE  const   IntBufferBias);

extern ADI_CT_RESULT_TYPE       adi_CT_SetPGABiasCurrent                  (ADI_CT_DEV_HANDLE            const   hDevice,
                                                                           ADI_CT_PGA_BIAS_TYPE         const   PGABias);

extern ADI_CT_RESULT_TYPE       adi_CT_SetC2VBiasCurrent                  (ADI_CT_DEV_HANDLE            const   hDevice,
                                                                           ADI_CT_C2V_BIAS_TYPE         const   C2VBias);

extern ADI_CT_RESULT_TYPE       adi_CT_SetPK2PKCfg                        (ADI_CT_DEV_HANDLE            const   hDevice,
                                                                           ADI_CT_PK2PK_TYPE*           const   pPK2PK_Settings);

extern ADI_CT_RESULT_TYPE       adi_CT_SetCDCResultFormat                 (ADI_CT_DEV_HANDLE            const   hDevice,
                                                                           ADI_CT_CDC_RESULT_TYPE       const   ResSel);

extern ADI_CT_RESULT_TYPE       adi_CT_SetIIRFilterWeight                 (ADI_CT_DEV_HANDLE            const   hDevice,
                                                                           uint8_t                      const   IIRWeight);

extern ADI_CT_RESULT_TYPE       adi_CT_SetIIRFilterEnable                 (ADI_CT_DEV_HANDLE            const   hDevice,
                                                                           bool_t                       const   enable);

extern ADI_CT_RESULT_TYPE       adi_CT_SetAverageCfg                      (ADI_CT_DEV_HANDLE            const   hDevice,
                                                                           ADI_CT_AVERAGE_TYPE*         const   pAverageSettings);

extern ADI_CT_RESULT_TYPE       adi_CT_SetBaselineCfg                     (ADI_CT_DEV_HANDLE            const   hDevice,
                                                                           ADI_CT_BASELINE_TYPE*        const   pBaselineSettings);

extern ADI_CT_RESULT_TYPE       adi_CT_SetGenericSettings                 (ADI_CT_DEV_HANDLE            const   hDevice,
                                                                           ADI_CT_GENERIC_SETTINGS_TYPE* const  pGenericSettings);


extern ADI_CT_RESULT_TYPE       adi_CT_SetStageGenericCfg                 (ADI_CT_DEV_HANDLE            const   hDevice,
                                                                           ADI_CT_STAGE_NUMBER_TYPE     const   StageNumber,
                                                                           ADI_CT_INPUT_TYPE            const   CDCInput,
                                                                           ADI_STAGE_TYPE*              const   pStageType);

extern ADI_CT_RESULT_TYPE       adi_CT_SetStageSelectInput                (ADI_CT_DEV_HANDLE            const   hDevice,
                                                                           ADI_CT_STAGE_NUMBER_TYPE     const   StageNumber,
                                                                           ADI_CT_INPUT_TYPE            const   CDCInput);

extern ADI_CT_RESULT_TYPE       adi_CT_SetStageByPassPGA                  (ADI_CT_DEV_HANDLE            const   hDevice,
                                                                           ADI_CT_STAGE_NUMBER_TYPE     const   StageNumber,
                                                                           bool_t                       const   ByPassPGA);

extern ADI_CT_RESULT_TYPE       adi_CT_SetStagePGAGain                    (ADI_CT_DEV_HANDLE            const   hDevice,
                                                                           ADI_CT_STAGE_NUMBER_TYPE     const   StageNumber,
                                                                           ADI_CT_PGA_GAIN_TYPE         const   PGA_Gain);

extern ADI_CT_RESULT_TYPE       adi_CT_SetStageC2VRange                   (ADI_CT_DEV_HANDLE            const   hDevice,
                                                                           ADI_CT_STAGE_NUMBER_TYPE     const   StageNumber,
                                                                           ADI_CT_C2VRANGE_TYPE         const   C2VRange);

extern ADI_CT_RESULT_TYPE       adi_CT_SetStagePK2PKEnable                (ADI_CT_DEV_HANDLE            const   hDevice,
                                                                           ADI_CT_STAGE_NUMBER_TYPE     const   StageNumber,
                                                                           bool_t                       const   PK2PKMeasurements);

extern ADI_CT_RESULT_TYPE       adi_CT_SetStageFreqHoppingEnable          (ADI_CT_DEV_HANDLE            const   hDevice,
                                                                           ADI_CT_STAGE_NUMBER_TYPE     const   StageNumber,
                                                                           bool_t                       const   DisableSignalExcitation);

extern ADI_CT_RESULT_TYPE       adi_CT_SetStageCAPDACEnable               (ADI_CT_DEV_HANDLE            const   hDevice,
                                                                           ADI_CT_STAGE_NUMBER_TYPE     const   StageNumber,
                                                                           bool_t                       const   MAIN_DAC_Enable);
extern ADI_CT_RESULT_TYPE       adi_CT_SetStageCAPDACValue                (ADI_CT_DEV_HANDLE            const   hDevice,
                                                                           ADI_CT_STAGE_NUMBER_TYPE     const   StageNumber,
                                                                           uint8_t                      const   MAIN_DAC_Value,
                                                                           uint8_t                      const   SUB_DAC_Value);



extern ADI_CT_RESULT_TYPE       adi_CT_SetStagesEnable                    (ADI_CT_DEV_HANDLE            const   hDevice,
                                                                           uint16_t                     const   Stages,
                                                                           bool_t                       const   enable);

extern ADI_CT_RESULT_TYPE       adi_CT_SetStagesCalibrationEnable         (ADI_CT_DEV_HANDLE            const   hDevice,
                                                                           uint16_t                     const   Stages,
                                                                           bool_t                       const   enable);


extern ADI_CT_RESULT_TYPE       adi_CT_SetStageSensorThreshold            (ADI_CT_DEV_HANDLE            const   hDevice,
                                                                           ADI_CT_STAGE_NUMBER_TYPE     const   StageNumber,
                                                                           uint16_t                     const   SensorThreshold);


extern ADI_CT_RESULT_TYPE       adi_CT_SetInterruptsEnable                (ADI_CT_DEV_HANDLE            const   hDevice,
                                                                           uint8_t                      const   CaptouchInterrupts,
                                                                           bool_t                       const   enable);

extern ADI_CT_RESULT_TYPE       adi_CT_SetInterruptFlag                   (ADI_CT_DEV_HANDLE            const   hDevice);

extern ADI_CT_RESULT_TYPE       adi_CT_ClearInterruptFlag                 (ADI_CT_DEV_HANDLE            const   hDevice);


extern ADI_CT_RESULT_TYPE       adi_CT_SetTouchEnable                     (ADI_CT_DEV_HANDLE            const   hDevice,
                                                                           bool_t                       const   Enable);

extern ADI_CT_RESULT_TYPE       adi_CT_SetTouchCfg                        (ADI_CT_DEV_HANDLE            const   hDevice,
                                                                           ADI_CT_TOUCH_AND_RELEASE_TYPE* const pTouchSettings);

extern ADI_CT_RESULT_TYPE       adi_CT_SetReleaseEnable                   (ADI_CT_DEV_HANDLE            const   hDevice,
                                                                           bool_t                       const   Enable);

extern ADI_CT_RESULT_TYPE       adi_CT_SetReleaseCfg                      (ADI_CT_DEV_HANDLE            const   hDevice,
                                                                           ADI_CT_TOUCH_AND_RELEASE_TYPE* const pReleaseSettings);

extern ADI_CT_RESULT_TYPE       adi_CT_SetAutoForceBaselineCal            (ADI_CT_DEV_HANDLE const hDevice);

extern ADI_CT_RESULT_TYPE       adi_CT_SetStartSingleSequence             (ADI_CT_DEV_HANDLE const hDevice);


extern ADI_CT_RESULT_TYPE       adi_CT_GetTchAndRelAlgorithmStatus        (ADI_CT_DEV_HANDLE            const   hDevice,
                                                                           uint8_t                              *ButtonStatus);

extern ADI_CT_RESULT_TYPE       adi_CT_GetCDCValue                        (ADI_CT_DEV_HANDLE            const   hDevice,
                                                                           ADI_CT_STAGE_NUMBER_TYPE             StageNumber,
                                                                           uint16_t                             *CDCResult);

extern ADI_CT_RESULT_TYPE       adi_CT_GetBaselineValue                   (ADI_CT_DEV_HANDLE            const   hDevice,
                                                                           ADI_CT_STAGE_NUMBER_TYPE             StageNumber,
                                                                           uint16_t                             *BaselineResult);

extern ADI_CT_RESULT_TYPE       adi_CT_GetFastFilterValue                 (ADI_CT_DEV_HANDLE            const   hDevice,
                                                                           ADI_CT_STAGE_NUMBER_TYPE             StageNumber,
                                                                           uint16_t                             *FastFilterResult);

extern ADI_CT_RESULT_TYPE       adi_CT_GetPK2PKValue                      (ADI_CT_DEV_HANDLE            const   hDevice,
                                                                           ADI_CT_STAGE_NUMBER_TYPE             StageNumber,
                                                                           uint16_t                             *PK2PKResult);

extern ADI_CT_RESULT_TYPE       adi_CT_GetInterruptStatus                 (ADI_CT_DEV_HANDLE    const   hDevice,
                                                                           uint8_t                      *InterruptStatus);

extern ADI_CT_RESULT_TYPE       adi_CT_GetOffsHStatus                     (ADI_CT_DEV_HANDLE    const   hDevice,
                                                                           uint16_t                     *OffsHStatus);

extern ADI_CT_RESULT_TYPE       adi_CT_GetOffsLStatus                     (ADI_CT_DEV_HANDLE    const   hDevice,
                                                                           uint16_t                     *OffsLStatus);

extern ADI_CT_RESULT_TYPE       adi_CT_GetProxStatus                      (ADI_CT_DEV_HANDLE    const    hDevice,
                                                                           uint16_t                      *ProxStatus);

extern ADI_CT_RESULT_TYPE       adi_CT_GetFProxStatus                     (ADI_CT_DEV_HANDLE    const    hDevice,
                                                                           uint16_t                      *FProxStatus);


extern ADI_CT_RESULT_TYPE       adi_CT_GetEnable                          (ADI_CT_DEV_HANDLE    const   hDevice,
                                                                           bool_t                       *status);

extern ADI_CT_RESULT_TYPE       adi_CT_GetPowerStatus                     (ADI_CT_DEV_HANDLE    const   hDevice,
                                                                           bool_t                       *status);


extern ADI_CT_RESULT_TYPE       adi_CT_GetIntCycles                       (ADI_CT_DEV_HANDLE    const   hDevice,
                                                                           ADI_CT_INTCYCLES_TYPE        *IntCycles);

extern ADI_CT_RESULT_TYPE       adi_CT_GetUnmatchedPads                   (ADI_CT_DEV_HANDLE    const   hDevice,
                                                                           ADI_CT_UNMATCHED_PADS_TYPE   *UnmatchedPads);

extern ADI_CT_RESULT_TYPE       adi_CT_GetTimingsCfg                      (ADI_CT_DEV_HANDLE    const   hDevice,
                                                                           ADI_CT_C2V_TIMING_TYPE       *TimingSettings);

extern ADI_CT_RESULT_TYPE       adi_CT_GetSelfTimerValue                  (ADI_CT_DEV_HANDLE    const   hDevice,
                                                                           uint16_t                     *SelfTimerValue);

extern ADI_CT_RESULT_TYPE       adi_CT_GetSelfTimerEnable                 (ADI_CT_DEV_HANDLE    const   hDevice,
                                                                           bool_t                       *status);

extern ADI_CT_RESULT_TYPE       adi_CT_GetByPassGain                      (ADI_CT_DEV_HANDLE    const   hDevice,
                                                                           bool_t                       *status);

extern ADI_CT_RESULT_TYPE       adi_CT_GetC2VLowPassFilterEnable          (ADI_CT_DEV_HANDLE    const   hDevice,
                                                                           bool_t                       *status);

extern ADI_CT_RESULT_TYPE       adi_CT_GetIntBufferBiasCurrent            (ADI_CT_DEV_HANDLE    const   hDevice,
                                                                           ADI_CT_INT_BUFFER_BIAS_TYPE  *IntBufferBias);

extern ADI_CT_RESULT_TYPE       adi_CT_GetPGABiasCurrent                  (ADI_CT_DEV_HANDLE    const   hDevice,
                                                                           ADI_CT_PGA_BIAS_TYPE         *PGABias);

extern ADI_CT_RESULT_TYPE       adi_CT_GetC2VBiasCurrent                  (ADI_CT_DEV_HANDLE    const   hDevice,
                                                                           ADI_CT_C2V_BIAS_TYPE         *C2VBias);

extern ADI_CT_RESULT_TYPE       adi_CT_GetPK2PKCfg                        (ADI_CT_DEV_HANDLE    const   hDevice,
                                                                           ADI_CT_PK2PK_TYPE            *PK2PK_Settings);

extern ADI_CT_RESULT_TYPE       adi_CT_GetCDCResultFormat                 (ADI_CT_DEV_HANDLE    const   hDevice,
                                                                           ADI_CT_CDC_RESULT_TYPE       *ResSel);

extern ADI_CT_RESULT_TYPE       adi_CT_GetIIRFilterWeight                 (ADI_CT_DEV_HANDLE    const   hDevice,
                                                                           uint8_t                      *IIRWeight);

extern ADI_CT_RESULT_TYPE       adi_CT_GetIIRFilterEnable                 (ADI_CT_DEV_HANDLE    const   hDevice,
                                                                           bool_t                       *status);

extern ADI_CT_RESULT_TYPE       adi_CT_GetAverageCfg                      (ADI_CT_DEV_HANDLE    const   hDevice,
                                                                           ADI_CT_AVERAGE_TYPE          *AverageSettings);

extern ADI_CT_RESULT_TYPE       adi_CT_GetBaselineCfg                     (ADI_CT_DEV_HANDLE    const   hDevice,
                                                                           ADI_CT_BASELINE_TYPE         *BaselineSettings);


extern ADI_CT_RESULT_TYPE       adi_CT_GetStageGenericCfg                 (ADI_CT_DEV_HANDLE    const   hDevice,
                                                                           ADI_CT_STAGE_NUMBER_TYPE     StageNumber,
                                                                           ADI_CT_INPUT_TYPE            *pCDCInput,
                                                                           ADI_STAGE_TYPE               *pStageType);

extern ADI_CT_RESULT_TYPE       adi_CT_GetStagesCalibrationEnable         (ADI_CT_DEV_HANDLE    const   hDevice,
                                                                           uint16_t                     *Stages);

extern ADI_CT_RESULT_TYPE       adi_CT_GetStageSelectInput                (ADI_CT_DEV_HANDLE    const   hDevice,
                                                                           ADI_CT_STAGE_NUMBER_TYPE     StageNumber,
                                                                           ADI_CT_INPUT_TYPE            *CDCInput);

extern ADI_CT_RESULT_TYPE       adi_CT_GetStageByPassPGA                  (ADI_CT_DEV_HANDLE    const   hDevice,
                                                                           ADI_CT_STAGE_NUMBER_TYPE     StageNumber,
                                                                           bool_t                       *Status);

extern ADI_CT_RESULT_TYPE       adi_CT_GetStagePGAGain                    (ADI_CT_DEV_HANDLE    const   hDevice,
                                                                           ADI_CT_STAGE_NUMBER_TYPE     StageNumber,
                                                                           ADI_CT_PGA_GAIN_TYPE         *PGA_Gain);

extern ADI_CT_RESULT_TYPE       adi_CT_GetStageC2VRange                   (ADI_CT_DEV_HANDLE    const   hDevice,
                                                                           ADI_CT_STAGE_NUMBER_TYPE     StageNumber,
                                                                           ADI_CT_C2VRANGE_TYPE         *C2VRange);

extern ADI_CT_RESULT_TYPE       adi_CT_GetStagePK2PKEnable                (ADI_CT_DEV_HANDLE    const   hDevice,
                                                                           ADI_CT_STAGE_NUMBER_TYPE     StageNumber,
                                                                           bool_t                       *PK2PKMeasurements);

extern ADI_CT_RESULT_TYPE       adi_CT_GetStageFreqHoppingEnable          (ADI_CT_DEV_HANDLE    const   hDevice,
                                                                           ADI_CT_STAGE_NUMBER_TYPE     StageNumber,
                                                                           bool_t                       *SignalExcitation);

extern ADI_CT_RESULT_TYPE       adi_CT_GetStageCAPDACEnable               (ADI_CT_DEV_HANDLE    const   hDevice,
                                                                           ADI_CT_STAGE_NUMBER_TYPE     StageNumber,
                                                                           bool_t                       *MAIN_DAC_Enable);

extern ADI_CT_RESULT_TYPE       adi_CT_GetStageCAPDACValue                (ADI_CT_DEV_HANDLE    const   hDevice,
                                                                           ADI_CT_STAGE_NUMBER_TYPE     StageNumber,
                                                                           uint8_t                      *MAIN_DAC_Value,
                                                                           uint8_t                      *SUB_DAC_Value);

extern ADI_CT_RESULT_TYPE       adi_CT_GetStagesEnable                    (ADI_CT_DEV_HANDLE    const   hDevice,
                                                                           uint16_t                     *Stages);

extern ADI_CT_RESULT_TYPE       adi_CT_GetStageSensorThreshold            (ADI_CT_DEV_HANDLE    const   hDevice,
                                                                           ADI_CT_STAGE_NUMBER_TYPE     StageNumber,
                                                                           uint16_t                     *SensorThreshold);


extern ADI_CT_RESULT_TYPE       adi_CT_GetInterruptsEnable                (ADI_CT_DEV_HANDLE    const   hDevice,
                                                                           uint8_t                      *CaptouchInterrupts);

extern ADI_CT_RESULT_TYPE       adi_CT_GetInterruptFlag                   (ADI_CT_DEV_HANDLE    const   hDevice,
                                                                           bool_t                       *InterruptFlag);

extern ADI_CT_RESULT_TYPE       adi_CT_GetTouchEnable                     (ADI_CT_DEV_HANDLE    const   hDevice,
                                                                           bool_t                       *Status);

extern ADI_CT_RESULT_TYPE       adi_CT_GetTouchCfg                        (ADI_CT_DEV_HANDLE    const   hDevice,
                                                                           ADI_CT_TOUCH_AND_RELEASE_TYPE *TouchSettings);

extern ADI_CT_RESULT_TYPE       adi_CT_GetReleaseEnable                   (ADI_CT_DEV_HANDLE    const   hDevice,
                                                                           bool_t                       *Status);

extern ADI_CT_RESULT_TYPE       adi_CT_GetReleaseCfg                      (ADI_CT_DEV_HANDLE    const   hDevice,
                                                                           ADI_CT_TOUCH_AND_RELEASE_TYPE *ReleaseSettings);

extern ADI_CT_RESULT_TYPE       adi_CT_RegisterCallback                  (ADI_CT_DEV_HANDLE     const   hDevice,
                                                                          ADI_CALLBACK                  cbFunc,
                                                                          ADI_CT_INTERRUPT_TYPE         cbWatch);


/*extern ADI_CT_RESULT_TYPE       adi_CT_GetTouchStatus                     (ADI_CT_DEV_HANDLE const hDevice,
                                                                           uint16_t *TouchStatus);
extern ADI_CT_RESULT_TYPE       adi_CT_GetReleaseStatus                   (ADI_CT_DEV_HANDLE const hDevice,
                                                                           uint16_t *ReleaseStatus);*/

/* C++ linkage */
#ifdef __cplusplus
}
#endif

#endif /* include guard */

/*
** EOF
*/

/*@}*/



