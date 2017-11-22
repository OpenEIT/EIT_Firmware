/*********************************************************************************

Copyright (c) 2012-2014 Analog Devices, Inc. All Rights Reserved.

This software is proprietary to Analog Devices, Inc. and its licensors.  By using 
this software you agree to the terms of the associated Analog Devices Software 
License Agreement.

*********************************************************************************/

/*!
 ******************************************************************************
 * @file:    captouch.c
 * @brief:   Captouch Device Implementations for ADuCM350
 * @version: $Revision: 28525 $
 * @date:    $Date: 2014-11-12 14:51:26 -0500 (Wed, 12 Nov 2014) $
 *****************************************************************************/


/*! \addtogroup CT_Driver
 *  @{
 */


#define ASSERT_ADI_MISRA_SUPPRESSIONS  /*!< Apply ADI MISRA Suppressions */
#include "misra.h"

#include <stddef.h>                /* for NULL */
#include <string.h>                /* for memcpy */

#include "captouch.h"
#include "gpio.h"


/* forward declarations... */
/* These are needed in the case where ADI_CFG_ENABLE_RTOS_SUPPORT=1              */
/* The ADI_INSTALL_HANDLER macro will expand to a call to adi_int_InstallHandler */
/* and the handler will be referenced                                            */
/* The non-OSAL expansion of ADI_INSTALL_HANDLER results in a NULL string        */
ADI_INT_HANDLER(CapTouch_Int_Handler);



/*! \cond PRIVATE */

/*!
 *  \struct ADI_CT_DEV_DATA_TYPE
 *
 *  CT Configuration Structure
 */

typedef struct ADI_CT_DEV_DATA_TYPE
{

    ADI_CT_DRV_STATE_TYPE          DrvState;                 /*!< Driver state            */
    ADI_CT_TypeDef                      *pCTRegs;                  /*!< CT register pointer     */
    volatile bool_t                bCT_EventFlag;            /*!< Event Flag             */
    IRQn_Type                           IRQn;            /*!< CT interrupt number       */
    uint8_t                             CTEventType;     /*!< Captouch Event Type     */
    ADI_CALLBACK                   cbFunc;           	     /*!< Callback 	          */
    ADI_CT_INTERRUPT_TYPE          cbWatch;                  /*!< Callback watch bits     */

} ADI_CT_DEV_DATA_TYPE;


/* allocate internal device instance data array */
static ADI_CT_DEV_DATA_TYPE CT_DevData[ADI_CT_DEV_ID_MAXNUM];   /*!< Private CT device driver instance data */



/* conditional static initialization data */
typedef struct {
	uint32_t CT_CDC_PWR;            /*!< Captouch Power register */
	uint32_t CT_CFG_1;              /*!< Captouch Config Register 1 */
	uint32_t CT_CFG_2;              /*!< Captouch Config Register 2 */
	uint32_t CT_CFG_3;              /*!< Captouch Config Register 3 */
	uint32_t CT_MEAS_SEL;           /*!< Captouch CDC measurement stage selection */
	uint32_t CT_BASELINE_CTRL;      /*!< Captouch Baseline control settings */
	uint32_t CT_AVG;                /*!< Captouch Average control settings */
	uint32_t CT_CAL_EN;             /*!< Captouch enable calibration for stages */
	uint32_t CT_TOUCH_CFG1;         /*!< Captouch touch detection thresholds */
	uint32_t CT_TOUCH_CFG2;         /*!< Captouch touch detection time-out */
	uint32_t CT_RELEASE_CFG1;       /*!< Captouch release detection thresholds */
	uint32_t CT_RELEASE_CFG2;       /*!< Captouch release detection time-out */
	uint32_t CT_IEN;                /*!< Captouch interrupt enable */
	uint32_t CT_STAGE0_CFG;         /*!< Captouch Stage 0 configuration */
	uint32_t CT_STAGE1_CFG;         /*!< Captouch Stage 1 configuration */
	uint32_t CT_STAGE2_CFG;         /*!< Captouch Stage 2 configuration */
	uint32_t CT_STAGE3_CFG;         /*!< Captouch Stage 3 configuration */
	uint32_t CT_STAGE4_CFG;         /*!< Captouch Stage 4 configuration */
	uint32_t CT_STAGE5_CFG;         /*!< Captouch Stage 5 configuration */
	uint32_t CT_STAGE6_CFG;         /*!< Captouch Stage 6 configuration */
	uint32_t CT_STAGE7_CFG;         /*!< Captouch Stage 7 configuration */
	uint32_t CT_STAGE8_CFG;         /*!< Captouch Stage 8 configuration */
	uint32_t CT_STAGE9_CFG;         /*!< Captouch Stage 9 configuration */
	uint32_t CT_STAGE10_CFG;        /*!< Captouch Stage 10 configuration */
	uint32_t CT_STAGE11_CFG;        /*!< Captouch Stage 11 configuration */
	uint32_t CT_STAGE12_CFG;        /*!< Captouch Stage 12 configuration */
	uint32_t CT_STAGE13_CFG;        /*!< Captouch Stage 13 configuration */
	uint32_t CT_STAGE14_CFG;        /*!< Captouch Stage 14 configuration */
	uint32_t CT_STAGE15_CFG;        /*!< Captouch Stage 15 configuration */
	uint32_t CT_SENSOR_THR_CFG0;    /*!< Captouch Sensor threshold 0 */
	uint32_t CT_SENSOR_THR_CFG1;    /*!< Captouch Sensor threshold 1 */
	uint32_t CT_SENSOR_THR_CFG2;    /*!< Captouch Sensor threshold 2 */
	uint32_t CT_SENSOR_THR_CFG3;    /*!< Captouch Sensor threshold 3 */
	uint32_t CT_SENSOR_THR_CFG4;    /*!< Captouch Sensor threshold 4 */
	uint32_t CT_SENSOR_THR_CFG5;    /*!< Captouch Sensor threshold 5 */
	uint32_t CT_SENSOR_THR_CFG6;    /*!< Captouch Sensor threshold 6 */
	uint32_t CT_SENSOR_THR_CFG7;    /*!< Captouch Sensor threshold 7 */
} ADI_CT_CFG_TYPE;

static const ADI_CT_CFG_TYPE gCTStaticConfigData[ADI_CT_DEV_ID_MAXNUM] = {
	/* single instance of captouch device */
  {
		/* Power register */
		( (CT_CDC_PWR_MODE_SHUTDOWN             << BITP_CT_CT_CDC_PWR_PWR_MODE)
		),

		/* Config Register 1 */
		( (CT_CFG_1_INT_CYCLES                  << BITP_CT_CT_CFG1_INT_CYCLE)
		| (CT_CFG_1_AIN_SEL                     << BITP_CT_CT_CFG1_AIN_SEL)
		| (CT_CFG_1_AUTOZERO_DUTY               << BITP_CT_CT_CFG1_AUTOZERO_DUTY)
		| (CT_CFG_1_BYPASS_GAIN                 << BITP_CT_CT_CFG1_BYPASS_GAIN)
		| (CT_CFG_1_INT_BUFFER                  << BITP_CT_CT_CFG1_INT_BUFFER)
		| (CT_CFG_1_PGA_BIAS                    << BITP_CT_CT_CFG1_PGA_BIAS)
                | (CT_CFG_1_C2V_BIAS                    << BITP_CT_CT_CFG1_C2V_BIAS)
		),

		/* Config Register 2 */
		( (CT_CFG_2_C2V_LPF_ENABLE              << BITP_CT_CT_CFG2_C2V_LPF)
		| (CT_CFG_2_SELFTIMER_ENABLE            << BITP_CT_CT_CFG2_STIMER_EN)
		| (CT_CFG_2_C2V_HOLD_TIME               << BITP_CT_CT_CFG2_C2V_HOLD_TIME)
                | (CT_CFG_2_SELFTIMER_VALUE             << BITP_CT_CT_CFG2_SELF_TIMER_WAIT)
		| (CT_CFG_2_PH24                        << BITP_CT_CT_CFG2_PH24_DUTY)
		| (CT_CFG_2_PH13                        << BITP_CT_CT_CFG2_PH13_DUTY)
		),

		/* Config Register 3 */
		( (CT_CFG_3_PK2PK_AVG                   << BITP_CT_CT_CFG3_PK2PK_AVG)
		| (CT_CFG_3_PK2PK_AVG_MIN               << BITP_CT_CT_CFG3_PK2PK_AVG_MIN)
		| (CT_CFG_3_PK2PK_SUBSET                << BITP_CT_CT_CFG3_PK2PK_SUBSET)
                | (CT_CFG_3_PK2PK_NUM_SPLS              << BITP_CT_CT_CFG3_PK2PK_NUM_SPLS)
		| (CT_CFG_3_RES_SEL                     << BITP_CT_CT_CFG3_RES_SEL)
                | (CT_CFG_3_DIV_CTOV_CAPTOUCH_EN        << BITP_CT_CT_CFG3_DIG_CTOV_CAPTOUCH_EN)
		| (CT_CFG_3_IIR_WEIGHT                  << BITP_CT_CT_CFG3_IIR_WEIGHT)
                | (CT_CFG_3_IIR_ENABLE                  << BITP_CT_CT_CFG3_IIR_EN)
		),

                /* Measurement Selection Register */
		( (CT_MEAS_SEL_STAGE                    << BITP_CT_CT_MEAS_SEL_GND_SEL)
		),

                /* Baseline Control Register*/
		( (CT_BASELINE_CAL_DELAY                << BITP_CT_CT_BASELINE_CTRL_BASELINE_CAL_DELAY)
		| (CT_BASELINE_FAST_FILTER_UPDATE       << BITP_CT_CT_BASELINE_CTRL_FAST_FILTER_UPDATE)
		| (CT_BASELINE_FAST_PROX                << BITP_CT_CT_BASELINE_CTRL_FAST_PROX)
                | (CT_BASELINE_SLOW_PROX                << BITP_CT_CT_BASELINE_CTRL_SLOW_PROX)
		| (CT_BASELINE_BL_COEFF                 << BITP_CT_CT_BASELINE_CTRL_BL_COEFF)
                | (CT_BASELINE_CDC_COEFF                << BITP_CT_CT_BASELINE_CTRL_CDC_COEFF)
		),

                /* Average Control Register*/
		( (CT_AVG_RANK                          << BITP_CT_CT_AVG_RANK_FILT)
		| (CT_AVG_OUTPUT                        << BITP_CT_CT_AVG_AVG_OUTPUT)
		| (CT_AVG_SAMPLES_GROUP2                << BITP_CT_CT_AVG_AVG_GROUP2)
                | (CT_AVG_SAMPLES_GROUP1                << BITP_CT_CT_AVG_AVG_GROUP1)
		| (CT_AVG_MIN_GROUP2                    << BITP_CT_CT_AVG_AVG_MIN_GROUP2)
                | (CT_AVG_MIN_GROUP1                    << BITP_CT_CT_AVG_AVG_MIN_GROUP1)
		),

                /* Calibration Enable Register */
		( (CT_CAL_EN_STAGE                      << BITP_CT_CT_CAL_EN_CAL_EN)
		),

                /* Touch Register CFG 1*/
		( (CT_TOUCH_CFG1_UPPER_THLD             << BITP_CT_CT_TOUCH_CFG1_TCH_UPPER_THLD)
		| (CT_TOUCH_CFG1_LOWER_THLD             << BITP_CT_CT_TOUCH_CFG1_TCH_LOWER_THLD)
		),

                /* Touch Register CFG 2*/
		( (CT_TOUCH_CFG2_INDIVIDUAL_THRESHOLD_ENABLE    << BITP_CT_CT_TOUCH_CFG2_INDIVIDUAL_THRESHOLD_EN)
		| (CT_TOUCH_CFG2_EVENT_ON_HIGH                  << BITP_CT_CT_TOUCH_CFG2_TCH_EVENT_ON_HIGH)
                | (CT_TOUCH_CFG2_SEL_INPA                       << BITP_CT_CT_TOUCH_CFG2_TCH_SEL_INPA)
                | (CT_TOUCH_CFG2_SEL_INPB                       << BITP_CT_CT_TOUCH_CFG2_TCH_SEL_INPB)
                | (CT_TOUCH_CFG2_DETECT_ENABLE                  << BITP_CT_CT_TOUCH_CFG2_TCH_DETECT_ENABLE)
                | (CT_TOUCH_CFG2_MIN_DURATION                   << BITP_CT_CT_TOUCH_CFG2_TCH_MIN_DURATION)
		),

                /* Release Register CFG 1*/
		( (CT_REL_CFG1_UPPER_THLD               << BITP_CT_CT_RELEASE_CFG1_REL_UPPER_THLD)
		| (CT_REL_CFG1_LOWER_THLD               << BITP_CT_CT_RELEASE_CFG1_REL_LOWER_THLD)
		),

                /* Release Register CFG 2*/
		( (CT_REL_CFG2_INDIVIDUAL_THRESHOLD_ENABLE      << BITP_CT_CT_RELEASE_CFG2_INDIVIDUAL_THRESHOLD_EN)
		| (CT_REL_CFG2_EVENT_ON_HIGH                    << BITP_CT_CT_RELEASE_CFG2_REL_EVENT_ON_HIGH)
                | (CT_REL_CFG2_SEL_INPA                         << BITP_CT_CT_RELEASE_CFG2_REL_SEL_INPA)
                | (CT_REL_CFG2_SEL_INPB                         << BITP_CT_CT_RELEASE_CFG2_REL_SEL_INPB)
                | (CT_REL_CFG2_DETECT_ENABLE                    << BITP_CT_CT_RELEASE_CFG2_REL_DETECT_ENABLE)
                | (CT_REL_CFG2_MIN_DURATION                     << BITP_CT_CT_RELEASE_CFG2_REL_MIN_DURATION)
		),

                /* Interrupt Enable Register */
		( (CT_IEN_RELEASE_ENABLE                << BITP_CT_CT_IEN_RELEASE_DETECTED_IEN)
		| (CT_IEN_TOUCH_ENABLE                  << BITP_CT_CT_IEN_TOUCH_DETECTED_IEN)
                | (CT_IEN_PROX_ENABLE                   << BITP_CT_CT_IEN_PROX_DETECTED_IEN)
                | (CT_IEN_THRESHOLD_ENABLE              << BITP_CT_CT_IEN_STATUS_GT_THRESHOLD_IEN)
                | (CT_IEN_CONV_SET_COMPLETE_ENABLE      << BITP_CT_CT_IEN_CONV_SET_COMPLETE_IEN)
		),

                /* Stage 0 Config Register */
		( (CT_STAGE0_CFG_CIN_CON_POS_CDC_EN     << BITP_CT_CT_STAGE0_CFG_CIN_CON_POS_CDC_EN)
		| (CT_STAGE0_CFG_CIN_CON_POS_CDC        << BITP_CT_CT_STAGE0_CFG_CIN_CON_POS_CDC)
		| (CT_STAGE0_CFG_PGA_BYPASS             << BITP_CT_CT_STAGE0_CFG_PGA_BYPASS)
                | (CT_STAGE0_CFG_NOISE_REDUCTION_ENABLE << BITP_CT_CT_STAGE0_CFG_NOISE_REDUCTION_EN)
		| (CT_STAGE0_CFG_PGA_GAIN               << BITP_CT_CT_STAGE0_CFG_PGA_GAIN)
                | (CT_STAGE0_CFG_C2V_RANGE              << BITP_CT_CT_STAGE0_CFG_C2V_IP_RANGE)
		| (CT_STAGE0_CFG_CAPDAC_ENABLE          << BITP_CT_CT_STAGE0_CFG_EN_MAIN_DAC)
                | (CT_STAGE0_CFG_PK2PK_ENABLE           << BITP_CT_CT_STAGE0_CFG_PK2PK)
                | (CT_STAGE0_CFG_MAIN_DAC               << BITP_CT_CT_STAGE0_CFG_MAIN_DAC)
                | (CT_STAGE0_CFG_SUB_DAC                << BITP_CT_CT_STAGE0_CFG_SUB_DAC)
		),

                /* Stage 1 Config Register */
		( (CT_STAGE1_CFG_CIN_CON_POS_CDC_EN     << BITP_CT_CT_STAGE1_CFG_CIN_CON_POS_CDC_EN)
		| (CT_STAGE1_CFG_CIN_CON_POS_CDC        << BITP_CT_CT_STAGE1_CFG_CIN_CON_POS_CDC)
		| (CT_STAGE1_CFG_PGA_BYPASS             << BITP_CT_CT_STAGE1_CFG_PGA_BYPASS)
                | (CT_STAGE1_CFG_NOISE_REDUCTION_ENABLE << BITP_CT_CT_STAGE1_CFG_NOISE_REDUCTION_EN)
		| (CT_STAGE1_CFG_PGA_GAIN               << BITP_CT_CT_STAGE1_CFG_PGA_GAIN)
                | (CT_STAGE1_CFG_C2V_RANGE              << BITP_CT_CT_STAGE1_CFG_C2V_IP_RANGE)
		| (CT_STAGE1_CFG_CAPDAC_ENABLE          << BITP_CT_CT_STAGE1_CFG_EN_MAIN_DAC)
                | (CT_STAGE1_CFG_PK2PK_ENABLE           << BITP_CT_CT_STAGE1_CFG_PK2PK)
                | (CT_STAGE1_CFG_MAIN_DAC               << BITP_CT_CT_STAGE1_CFG_MAIN_DAC)
                | (CT_STAGE1_CFG_SUB_DAC                << BITP_CT_CT_STAGE1_CFG_SUB_DAC)
		),

                /* Stage 2 Config Register */
		( (CT_STAGE2_CFG_CIN_CON_POS_CDC_EN     << BITP_CT_CT_STAGE2_CFG_CIN_CON_POS_CDC_EN)
		| (CT_STAGE2_CFG_CIN_CON_POS_CDC        << BITP_CT_CT_STAGE2_CFG_CIN_CON_POS_CDC)
		| (CT_STAGE2_CFG_PGA_BYPASS             << BITP_CT_CT_STAGE2_CFG_PGA_BYPASS)
                | (CT_STAGE2_CFG_NOISE_REDUCTION_ENABLE << BITP_CT_CT_STAGE2_CFG_NOISE_REDUCTION_EN)
		| (CT_STAGE2_CFG_PGA_GAIN               << BITP_CT_CT_STAGE2_CFG_PGA_GAIN)
                | (CT_STAGE2_CFG_C2V_RANGE              << BITP_CT_CT_STAGE2_CFG_C2V_IP_RANGE)
		| (CT_STAGE2_CFG_CAPDAC_ENABLE          << BITP_CT_CT_STAGE2_CFG_EN_MAIN_DAC)
                | (CT_STAGE2_CFG_PK2PK_ENABLE           << BITP_CT_CT_STAGE2_CFG_PK2PK)
                | (CT_STAGE2_CFG_MAIN_DAC               << BITP_CT_CT_STAGE2_CFG_MAIN_DAC)
                | (CT_STAGE2_CFG_SUB_DAC                << BITP_CT_CT_STAGE2_CFG_SUB_DAC)
		),

                /* Stage 3 Config Register */
		( (CT_STAGE3_CFG_CIN_CON_POS_CDC_EN     << BITP_CT_CT_STAGE3_CFG_CIN_CON_POS_CDC_EN)
		| (CT_STAGE3_CFG_CIN_CON_POS_CDC        << BITP_CT_CT_STAGE3_CFG_CIN_CON_POS_CDC)
		| (CT_STAGE3_CFG_PGA_BYPASS             << BITP_CT_CT_STAGE3_CFG_PGA_BYPASS)
                | (CT_STAGE3_CFG_NOISE_REDUCTION_ENABLE << BITP_CT_CT_STAGE3_CFG_NOISE_REDUCTION_EN)
		| (CT_STAGE3_CFG_PGA_GAIN               << BITP_CT_CT_STAGE3_CFG_PGA_GAIN)
                | (CT_STAGE3_CFG_C2V_RANGE              << BITP_CT_CT_STAGE3_CFG_C2V_IP_RANGE)
		| (CT_STAGE3_CFG_CAPDAC_ENABLE          << BITP_CT_CT_STAGE3_CFG_EN_MAIN_DAC)
                | (CT_STAGE3_CFG_PK2PK_ENABLE           << BITP_CT_CT_STAGE3_CFG_PK2PK)
                | (CT_STAGE3_CFG_MAIN_DAC               << BITP_CT_CT_STAGE3_CFG_MAIN_DAC)
                | (CT_STAGE3_CFG_SUB_DAC                << BITP_CT_CT_STAGE3_CFG_SUB_DAC)
		),

                /* Stage 4 Config Register */
		( (CT_STAGE4_CFG_CIN_CON_POS_CDC_EN     << BITP_CT_CT_STAGE4_CFG_CIN_CON_POS_CDC_EN)
		| (CT_STAGE4_CFG_CIN_CON_POS_CDC        << BITP_CT_CT_STAGE4_CFG_CIN_CON_POS_CDC)
		| (CT_STAGE4_CFG_PGA_BYPASS             << BITP_CT_CT_STAGE4_CFG_PGA_BYPASS)
                | (CT_STAGE4_CFG_NOISE_REDUCTION_ENABLE << BITP_CT_CT_STAGE4_CFG_NOISE_REDUCTION_EN)
		| (CT_STAGE4_CFG_PGA_GAIN               << BITP_CT_CT_STAGE4_CFG_PGA_GAIN)
                | (CT_STAGE4_CFG_C2V_RANGE              << BITP_CT_CT_STAGE4_CFG_C2V_IP_RANGE)
		| (CT_STAGE4_CFG_CAPDAC_ENABLE          << BITP_CT_CT_STAGE4_CFG_EN_MAIN_DAC)
                | (CT_STAGE4_CFG_PK2PK_ENABLE           << BITP_CT_CT_STAGE4_CFG_PK2PK)
                | (CT_STAGE4_CFG_MAIN_DAC               << BITP_CT_CT_STAGE4_CFG_MAIN_DAC)
                | (CT_STAGE4_CFG_SUB_DAC                << BITP_CT_CT_STAGE4_CFG_SUB_DAC)
		),

                /* Stage 5 Config Register */
		( (CT_STAGE5_CFG_CIN_CON_POS_CDC_EN     << BITP_CT_CT_STAGE5_CFG_CIN_CON_POS_CDC_EN)
		| (CT_STAGE5_CFG_CIN_CON_POS_CDC        << BITP_CT_CT_STAGE5_CFG_CIN_CON_POS_CDC)
		| (CT_STAGE5_CFG_PGA_BYPASS             << BITP_CT_CT_STAGE5_CFG_PGA_BYPASS)
                | (CT_STAGE5_CFG_NOISE_REDUCTION_ENABLE << BITP_CT_CT_STAGE5_CFG_NOISE_REDUCTION_EN)
		| (CT_STAGE5_CFG_PGA_GAIN               << BITP_CT_CT_STAGE5_CFG_PGA_GAIN)
                | (CT_STAGE5_CFG_C2V_RANGE              << BITP_CT_CT_STAGE5_CFG_C2V_IP_RANGE)
		| (CT_STAGE5_CFG_CAPDAC_ENABLE          << BITP_CT_CT_STAGE5_CFG_EN_MAIN_DAC)
                | (CT_STAGE5_CFG_PK2PK_ENABLE           << BITP_CT_CT_STAGE5_CFG_PK2PK)
                | (CT_STAGE5_CFG_MAIN_DAC               << BITP_CT_CT_STAGE5_CFG_MAIN_DAC)
                | (CT_STAGE5_CFG_SUB_DAC                << BITP_CT_CT_STAGE5_CFG_SUB_DAC)
		),
                /* Stage 6 Config Register */
		( (CT_STAGE6_CFG_CIN_CON_POS_CDC_EN     << BITP_CT_CT_STAGE6_CFG_CIN_CON_POS_CDC_EN)
		| (CT_STAGE6_CFG_CIN_CON_POS_CDC        << BITP_CT_CT_STAGE6_CFG_CIN_CON_POS_CDC)
		| (CT_STAGE6_CFG_PGA_BYPASS             << BITP_CT_CT_STAGE6_CFG_PGA_BYPASS)
                | (CT_STAGE6_CFG_NOISE_REDUCTION_ENABLE << BITP_CT_CT_STAGE6_CFG_NOISE_REDUCTION_EN)
		| (CT_STAGE6_CFG_PGA_GAIN               << BITP_CT_CT_STAGE6_CFG_PGA_GAIN)
                | (CT_STAGE6_CFG_C2V_RANGE              << BITP_CT_CT_STAGE6_CFG_C2V_IP_RANGE)
		| (CT_STAGE6_CFG_CAPDAC_ENABLE          << BITP_CT_CT_STAGE6_CFG_EN_MAIN_DAC)
                | (CT_STAGE6_CFG_PK2PK_ENABLE           << BITP_CT_CT_STAGE6_CFG_PK2PK)
                | (CT_STAGE6_CFG_MAIN_DAC               << BITP_CT_CT_STAGE6_CFG_MAIN_DAC)
                | (CT_STAGE6_CFG_SUB_DAC                << BITP_CT_CT_STAGE6_CFG_SUB_DAC)
		),
                /* Stage 7 Config Register */
		( (CT_STAGE7_CFG_CIN_CON_POS_CDC_EN     << BITP_CT_CT_STAGE7_CFG_CIN_CON_POS_CDC_EN)
		| (CT_STAGE7_CFG_CIN_CON_POS_CDC        << BITP_CT_CT_STAGE7_CFG_CIN_CON_POS_CDC)
		| (CT_STAGE7_CFG_PGA_BYPASS             << BITP_CT_CT_STAGE7_CFG_PGA_BYPASS)
                | (CT_STAGE7_CFG_NOISE_REDUCTION_ENABLE << BITP_CT_CT_STAGE7_CFG_NOISE_REDUCTION_EN)
		| (CT_STAGE7_CFG_PGA_GAIN               << BITP_CT_CT_STAGE7_CFG_PGA_GAIN)
                | (CT_STAGE7_CFG_C2V_RANGE              << BITP_CT_CT_STAGE7_CFG_C2V_IP_RANGE)
		| (CT_STAGE7_CFG_CAPDAC_ENABLE          << BITP_CT_CT_STAGE7_CFG_EN_MAIN_DAC)
                | (CT_STAGE7_CFG_PK2PK_ENABLE           << BITP_CT_CT_STAGE7_CFG_PK2PK)
                | (CT_STAGE7_CFG_MAIN_DAC               << BITP_CT_CT_STAGE7_CFG_MAIN_DAC)
                | (CT_STAGE7_CFG_SUB_DAC                << BITP_CT_CT_STAGE7_CFG_SUB_DAC)
		),
                /* Stage 8 Config Register */
		( (CT_STAGE8_CFG_CIN_CON_POS_CDC_EN     << BITP_CT_CT_STAGE8_CFG_CIN_CON_POS_CDC_EN)
		| (CT_STAGE8_CFG_CIN_CON_POS_CDC        << BITP_CT_CT_STAGE8_CFG_CIN_CON_POS_CDC)
		| (CT_STAGE8_CFG_PGA_BYPASS             << BITP_CT_CT_STAGE8_CFG_PGA_BYPASS)
                | (CT_STAGE8_CFG_NOISE_REDUCTION_ENABLE << BITP_CT_CT_STAGE8_CFG_NOISE_REDUCTION_EN)
		| (CT_STAGE8_CFG_PGA_GAIN               << BITP_CT_CT_STAGE8_CFG_PGA_GAIN)
                | (CT_STAGE8_CFG_C2V_RANGE              << BITP_CT_CT_STAGE8_CFG_C2V_IP_RANGE)
		| (CT_STAGE8_CFG_CAPDAC_ENABLE          << BITP_CT_CT_STAGE8_CFG_EN_MAIN_DAC)
                | (CT_STAGE8_CFG_PK2PK_ENABLE           << BITP_CT_CT_STAGE8_CFG_PK2PK)
                | (CT_STAGE8_CFG_MAIN_DAC               << BITP_CT_CT_STAGE8_CFG_MAIN_DAC)
                | (CT_STAGE8_CFG_SUB_DAC                << BITP_CT_CT_STAGE8_CFG_SUB_DAC)
		),
                /* Stage 9 Config Register */
		( (CT_STAGE9_CFG_CIN_CON_POS_CDC_EN     << BITP_CT_CT_STAGE9_CFG_CIN_CON_POS_CDC_EN)
		| (CT_STAGE9_CFG_CIN_CON_POS_CDC        << BITP_CT_CT_STAGE9_CFG_CIN_CON_POS_CDC)
		| (CT_STAGE9_CFG_PGA_BYPASS             << BITP_CT_CT_STAGE9_CFG_PGA_BYPASS)
                | (CT_STAGE9_CFG_NOISE_REDUCTION_ENABLE << BITP_CT_CT_STAGE9_CFG_NOISE_REDUCTION_EN)
		| (CT_STAGE9_CFG_PGA_GAIN               << BITP_CT_CT_STAGE9_CFG_PGA_GAIN)
                | (CT_STAGE9_CFG_C2V_RANGE              << BITP_CT_CT_STAGE9_CFG_C2V_IP_RANGE)
		| (CT_STAGE9_CFG_CAPDAC_ENABLE          << BITP_CT_CT_STAGE9_CFG_EN_MAIN_DAC)
                | (CT_STAGE9_CFG_PK2PK_ENABLE           << BITP_CT_CT_STAGE9_CFG_PK2PK)
                | (CT_STAGE9_CFG_MAIN_DAC               << BITP_CT_CT_STAGE9_CFG_MAIN_DAC)
                | (CT_STAGE9_CFG_SUB_DAC                << BITP_CT_CT_STAGE9_CFG_SUB_DAC)
		),
                /* Stage 10 Config Register */
		( (CT_STAGE10_CFG_CIN_CON_POS_CDC_EN     << BITP_CT_CT_STAGE10_CFG_CIN_CON_POS_CDC_EN)
		| (CT_STAGE10_CFG_CIN_CON_POS_CDC        << BITP_CT_CT_STAGE10_CFG_CIN_CON_POS_CDC)
		| (CT_STAGE10_CFG_PGA_BYPASS             << BITP_CT_CT_STAGE10_CFG_PGA_BYPASS)
                | (CT_STAGE10_CFG_NOISE_REDUCTION_ENABLE << BITP_CT_CT_STAGE10_CFG_NOISE_REDUCTION_EN)
		| (CT_STAGE10_CFG_PGA_GAIN               << BITP_CT_CT_STAGE10_CFG_PGA_GAIN)
                | (CT_STAGE10_CFG_C2V_RANGE              << BITP_CT_CT_STAGE10_CFG_C2V_IP_RANGE)
		| (CT_STAGE10_CFG_CAPDAC_ENABLE          << BITP_CT_CT_STAGE10_CFG_EN_MAIN_DAC)
                | (CT_STAGE10_CFG_PK2PK_ENABLE           << BITP_CT_CT_STAGE10_CFG_PK2PK)
                | (CT_STAGE10_CFG_MAIN_DAC               << BITP_CT_CT_STAGE10_CFG_MAIN_DAC)
                | (CT_STAGE10_CFG_SUB_DAC                << BITP_CT_CT_STAGE10_CFG_SUB_DAC)
		),
                /* Stage 11 Config Register */
		( (CT_STAGE11_CFG_CIN_CON_POS_CDC_EN     << BITP_CT_CT_STAGE11_CFG_CIN_CON_POS_CDC_EN)
		| (CT_STAGE11_CFG_CIN_CON_POS_CDC        << BITP_CT_CT_STAGE11_CFG_CIN_CON_POS_CDC)
		| (CT_STAGE11_CFG_PGA_BYPASS             << BITP_CT_CT_STAGE11_CFG_PGA_BYPASS)
                | (CT_STAGE11_CFG_NOISE_REDUCTION_ENABLE << BITP_CT_CT_STAGE11_CFG_NOISE_REDUCTION_EN)
		| (CT_STAGE11_CFG_PGA_GAIN               << BITP_CT_CT_STAGE11_CFG_PGA_GAIN)
                | (CT_STAGE11_CFG_C2V_RANGE              << BITP_CT_CT_STAGE11_CFG_C2V_IP_RANGE)
		| (CT_STAGE11_CFG_CAPDAC_ENABLE          << BITP_CT_CT_STAGE11_CFG_EN_MAIN_DAC)
                | (CT_STAGE11_CFG_PK2PK_ENABLE           << BITP_CT_CT_STAGE11_CFG_PK2PK)
                | (CT_STAGE11_CFG_MAIN_DAC               << BITP_CT_CT_STAGE11_CFG_MAIN_DAC)
                | (CT_STAGE11_CFG_SUB_DAC                << BITP_CT_CT_STAGE11_CFG_SUB_DAC)
		),

                /* Stage 12 Config Register */
		( (CT_STAGE12_CFG_CIN_CON_POS_CDC_EN     << BITP_CT_CT_STAGE12_CFG_CIN_CON_POS_CDC_EN)
		| (CT_STAGE12_CFG_CIN_CON_POS_CDC        << BITP_CT_CT_STAGE12_CFG_CIN_CON_POS_CDC)
		| (CT_STAGE12_CFG_PGA_BYPASS             << BITP_CT_CT_STAGE12_CFG_PGA_BYPASS)
                | (CT_STAGE12_CFG_NOISE_REDUCTION_ENABLE << BITP_CT_CT_STAGE12_CFG_NOISE_REDUCTION_EN)
		| (CT_STAGE12_CFG_PGA_GAIN               << BITP_CT_CT_STAGE12_CFG_PGA_GAIN)
                | (CT_STAGE12_CFG_C2V_RANGE              << BITP_CT_CT_STAGE12_CFG_C2V_IP_RANGE)
		| (CT_STAGE12_CFG_CAPDAC_ENABLE          << BITP_CT_CT_STAGE12_CFG_EN_MAIN_DAC)
                | (CT_STAGE12_CFG_PK2PK_ENABLE           << BITP_CT_CT_STAGE12_CFG_PK2PK)
                | (CT_STAGE12_CFG_MAIN_DAC               << BITP_CT_CT_STAGE12_CFG_MAIN_DAC)
                | (CT_STAGE12_CFG_SUB_DAC                << BITP_CT_CT_STAGE12_CFG_SUB_DAC)
		),

                /* Stage 13 Config Register */
		( (CT_STAGE13_CFG_CIN_CON_POS_CDC_EN     << BITP_CT_CT_STAGE13_CFG_CIN_CON_POS_CDC_EN)
		| (CT_STAGE13_CFG_CIN_CON_POS_CDC        << BITP_CT_CT_STAGE13_CFG_CIN_CON_POS_CDC)
		| (CT_STAGE13_CFG_PGA_BYPASS             << BITP_CT_CT_STAGE13_CFG_PGA_BYPASS)
                | (CT_STAGE13_CFG_NOISE_REDUCTION_ENABLE << BITP_CT_CT_STAGE13_CFG_NOISE_REDUCTION_EN)
		| (CT_STAGE13_CFG_PGA_GAIN               << BITP_CT_CT_STAGE13_CFG_PGA_GAIN)
                | (CT_STAGE13_CFG_C2V_RANGE              << BITP_CT_CT_STAGE13_CFG_C2V_IP_RANGE)
		| (CT_STAGE13_CFG_CAPDAC_ENABLE          << BITP_CT_CT_STAGE13_CFG_EN_MAIN_DAC)
                | (CT_STAGE13_CFG_PK2PK_ENABLE           << BITP_CT_CT_STAGE13_CFG_PK2PK)
                | (CT_STAGE13_CFG_MAIN_DAC               << BITP_CT_CT_STAGE13_CFG_MAIN_DAC)
                | (CT_STAGE13_CFG_SUB_DAC                << BITP_CT_CT_STAGE13_CFG_SUB_DAC)
		),

                /* Stage 14 Config Register */
		( (CT_STAGE14_CFG_CIN_CON_POS_CDC_EN     << BITP_CT_CT_STAGE14_CFG_CIN_CON_POS_CDC_EN)
		| (CT_STAGE14_CFG_CIN_CON_POS_CDC        << BITP_CT_CT_STAGE14_CFG_CIN_CON_POS_CDC)
		| (CT_STAGE14_CFG_PGA_BYPASS             << BITP_CT_CT_STAGE14_CFG_PGA_BYPASS)
                | (CT_STAGE14_CFG_NOISE_REDUCTION_ENABLE << BITP_CT_CT_STAGE14_CFG_NOISE_REDUCTION_EN)
		| (CT_STAGE14_CFG_PGA_GAIN               << BITP_CT_CT_STAGE14_CFG_PGA_GAIN)
                | (CT_STAGE14_CFG_C2V_RANGE              << BITP_CT_CT_STAGE14_CFG_C2V_IP_RANGE)
		| (CT_STAGE14_CFG_CAPDAC_ENABLE          << BITP_CT_CT_STAGE14_CFG_EN_MAIN_DAC)
                | (CT_STAGE14_CFG_PK2PK_ENABLE           << BITP_CT_CT_STAGE14_CFG_PK2PK)
                | (CT_STAGE14_CFG_MAIN_DAC               << BITP_CT_CT_STAGE14_CFG_MAIN_DAC)
                | (CT_STAGE14_CFG_SUB_DAC                << BITP_CT_CT_STAGE14_CFG_SUB_DAC)
		),

                /* Stage 15 Config Register */
		( (CT_STAGE15_CFG_CIN_CON_POS_CDC_EN     << BITP_CT_CT_STAGE15_CFG_CIN_CON_POS_CDC_EN)
		| (CT_STAGE15_CFG_CIN_CON_POS_CDC        << BITP_CT_CT_STAGE15_CFG_CIN_CON_POS_CDC)
		| (CT_STAGE15_CFG_PGA_BYPASS             << BITP_CT_CT_STAGE15_CFG_PGA_BYPASS)
                | (CT_STAGE15_CFG_NOISE_REDUCTION_ENABLE << BITP_CT_CT_STAGE15_CFG_NOISE_REDUCTION_EN)
		| (CT_STAGE15_CFG_PGA_GAIN               << BITP_CT_CT_STAGE15_CFG_PGA_GAIN)
                | (CT_STAGE15_CFG_C2V_RANGE              << BITP_CT_CT_STAGE15_CFG_C2V_IP_RANGE)
		| (CT_STAGE15_CFG_CAPDAC_ENABLE          << BITP_CT_CT_STAGE15_CFG_EN_MAIN_DAC)
                | (CT_STAGE15_CFG_PK2PK_ENABLE           << BITP_CT_CT_STAGE15_CFG_PK2PK)
                | (CT_STAGE15_CFG_MAIN_DAC               << BITP_CT_CT_STAGE15_CFG_MAIN_DAC)
                | (CT_STAGE15_CFG_SUB_DAC                << BITP_CT_CT_STAGE15_CFG_SUB_DAC)
		),

                /* Sensor Threshold Register CFG0*/
		( (CT_SENSOR_THR_CFG0_STAGE0             << BITP_CT_CT_SENSOR_THR_CFG0_SENSOR_THRESHOLD0)
		| (CT_SENSOR_THR_CFG0_STAGE1             << BITP_CT_CT_SENSOR_THR_CFG0_SENSOR_THRESHOLD1)
		),

                /* Sensor Threshold Register CFG1*/
		( (CT_SENSOR_THR_CFG1_STAGE2             << BITP_CT_CT_SENSOR_THR_CFG1_SENSOR_THRESHOLD2)
		| (CT_SENSOR_THR_CFG1_STAGE3             << BITP_CT_CT_SENSOR_THR_CFG1_SENSOR_THRESHOLD3)
		),

                /* Sensor Threshold Register CFG2*/
		( (CT_SENSOR_THR_CFG2_STAGE4             << BITP_CT_CT_SENSOR_THR_CFG2_SENSOR_THRESHOLD4)
		| (CT_SENSOR_THR_CFG2_STAGE5             << BITP_CT_CT_SENSOR_THR_CFG2_SENSOR_THRESHOLD5)
		),

                /* Sensor Threshold Register CFG3*/
		( (CT_SENSOR_THR_CFG3_STAGE6             << BITP_CT_CT_SENSOR_THR_CFG3_SENSOR_THRESHOLD6)
		| (CT_SENSOR_THR_CFG3_STAGE7             << BITP_CT_CT_SENSOR_THR_CFG3_SENSOR_THRESHOLD7)
		),

                /* Sensor Threshold Register CFG4*/
		( (CT_SENSOR_THR_CFG4_STAGE8             << BITP_CT_CT_SENSOR_THR_CFG4_SENSOR_THRESHOLD8)
		| (CT_SENSOR_THR_CFG4_STAGE9             << BITP_CT_CT_SENSOR_THR_CFG4_SENSOR_THRESHOLD9)
		),

                /* Sensor Threshold Register CFG5*/
		( (CT_SENSOR_THR_CFG5_STAGE10             << BITP_CT_CT_SENSOR_THR_CFG5_SENSOR_THRESHOLD10)
		| (CT_SENSOR_THR_CFG5_STAGE11             << BITP_CT_CT_SENSOR_THR_CFG5_SENSOR_THRESHOLD11)
		),

                /* Sensor Threshold Register CFG6*/
		( (CT_SENSOR_THR_CFG6_STAGE12             << BITP_CT_CT_SENSOR_THR_CFG6_SENSOR_THRESHOLD12)
		| (CT_SENSOR_THR_CFG6_STAGE13             << BITP_CT_CT_SENSOR_THR_CFG6_SENSOR_THRESHOLD13)
		),

                /* Sensor Threshold Register CFG7*/
		( (CT_SENSOR_THR_CFG7_STAGE14             << BITP_CT_CT_SENSOR_THR_CFG7_SENSOR_THRESHOLD14)
		| (CT_SENSOR_THR_CFG7_STAGE15             << BITP_CT_CT_SENSOR_THR_CFG7_SENSOR_THRESHOLD15)
		),

  }
};

/*! \endcond */

/*!

  @brief            Initialize and allocate an CT device.

  @param[in]         devID        ADI_CT_DEV_ID_TYPE specifying the instance of the CT to use.
  @param[in]         phDevice     Pointer to a location the device handle will be written upon
                                  successful driver initialization.

  @return            Status

                     - #ADI_CT_RESULT_SUCCESS                        Call completed successfully.

  @details           No other CT APIs may be called until the device init function is called. Initialize an CT device using
                     internal default configuration settings and allocate the device for use.  The caller's device handle
                     is written with the device instance data pointer.  The returned device handle is required to be passed
                     to all subsequent calls to convey which device instance to operate on.

  @note              The contents of phDevice will be set to NULL upon failure.

 */


ADI_CT_RESULT_TYPE adi_CT_Init(ADI_CT_DEV_ID_TYPE const devID,ADI_CT_DEV_HANDLE* const phDevice)
{


    ADI_CT_DEV_HANDLE hDevice;

    /* set handle to null */
    *phDevice = NULL;

     /* local pointer to instance data */
    hDevice = &CT_DevData[devID];

  #ifdef ADI_DEBUG
    if (devID >= ADI_CT_DEV_ID_MAXNUM)
    {
        return ADI_CT_RESULT_ERR;
    }
   #endif

    /* Turn on CTCLK */
    SystemEnableClock(ADI_SYS_CLOCK_GATE_CTCLK, true);

    /* initialize internal device data */
    hDevice->pCTRegs    = pADI_CT;
     hDevice->DrvState = ADI_CT_DRV_STATE_INITIALIZED;
    hDevice->IRQn       = CAP_IRQn;
    hDevice->cbFunc     = NULL;
    hDevice->cbWatch    = (ADI_CT_INTERRUPT_TYPE) 0;


    /* assert statically configured initialization data */
    ADI_CT_CFG_TYPE const* pInitData = &gCTStaticConfigData[devID];

    hDevice->pCTRegs->CT_CDC_PWR                = pInitData->CT_CDC_PWR;
    hDevice->pCTRegs->CT_CFG1                   = pInitData->CT_CFG_1;
    hDevice->pCTRegs->CT_CFG2                   = pInitData->CT_CFG_2;
    hDevice->pCTRegs->CT_CFG3                   = pInitData->CT_CFG_3;
    hDevice->pCTRegs->CT_MEAS_SEL               = pInitData->CT_MEAS_SEL;
    hDevice->pCTRegs->CT_BASELINE_CTRL          = pInitData->CT_BASELINE_CTRL;
    hDevice->pCTRegs->CT_AVG                    = pInitData->CT_AVG;
    hDevice->pCTRegs->CT_CAL_EN                 = pInitData->CT_CAL_EN;
    hDevice->pCTRegs->CT_TOUCH_CFG1             = pInitData->CT_TOUCH_CFG1;
    hDevice->pCTRegs->CT_TOUCH_CFG2             = pInitData->CT_TOUCH_CFG2;
    hDevice->pCTRegs->CT_RELEASE_CFG1           = pInitData->CT_RELEASE_CFG1;
    hDevice->pCTRegs->CT_RELEASE_CFG2           = pInitData->CT_RELEASE_CFG2;
    hDevice->pCTRegs->CT_IEN                    = pInitData->CT_IEN;
    hDevice->pCTRegs->CT_STAGE0_CFG             = pInitData->CT_STAGE0_CFG;
    hDevice->pCTRegs->CT_STAGE1_CFG             = pInitData->CT_STAGE1_CFG;
    hDevice->pCTRegs->CT_STAGE2_CFG             = pInitData->CT_STAGE2_CFG;
    hDevice->pCTRegs->CT_STAGE3_CFG             = pInitData->CT_STAGE3_CFG;
    hDevice->pCTRegs->CT_STAGE4_CFG             = pInitData->CT_STAGE4_CFG;
    hDevice->pCTRegs->CT_STAGE5_CFG             = pInitData->CT_STAGE5_CFG;
    hDevice->pCTRegs->CT_STAGE6_CFG             = pInitData->CT_STAGE6_CFG;
    hDevice->pCTRegs->CT_STAGE7_CFG             = pInitData->CT_STAGE7_CFG;
    hDevice->pCTRegs->CT_STAGE8_CFG             = pInitData->CT_STAGE8_CFG;
    hDevice->pCTRegs->CT_STAGE9_CFG             = pInitData->CT_STAGE9_CFG;
    hDevice->pCTRegs->CT_STAGE10_CFG            = pInitData->CT_STAGE10_CFG;
    hDevice->pCTRegs->CT_STAGE11_CFG            = pInitData->CT_STAGE11_CFG;
    hDevice->pCTRegs->CT_STAGE12_CFG            = pInitData->CT_STAGE12_CFG;
    hDevice->pCTRegs->CT_STAGE13_CFG            = pInitData->CT_STAGE13_CFG;
    hDevice->pCTRegs->CT_STAGE14_CFG            = pInitData->CT_STAGE14_CFG;
    hDevice->pCTRegs->CT_STAGE15_CFG            = pInitData->CT_STAGE15_CFG;
    hDevice->pCTRegs->CT_SENSOR_THR_CFG0        = pInitData->CT_SENSOR_THR_CFG0;
    hDevice->pCTRegs->CT_SENSOR_THR_CFG1        = pInitData->CT_SENSOR_THR_CFG1;
    hDevice->pCTRegs->CT_SENSOR_THR_CFG2        = pInitData->CT_SENSOR_THR_CFG2;
    hDevice->pCTRegs->CT_SENSOR_THR_CFG3        = pInitData->CT_SENSOR_THR_CFG3;
    hDevice->pCTRegs->CT_SENSOR_THR_CFG4        = pInitData->CT_SENSOR_THR_CFG4;
    hDevice->pCTRegs->CT_SENSOR_THR_CFG5        = pInitData->CT_SENSOR_THR_CFG5;
    hDevice->pCTRegs->CT_SENSOR_THR_CFG6        = pInitData->CT_SENSOR_THR_CFG6;
    hDevice->pCTRegs->CT_SENSOR_THR_CFG7        = pInitData->CT_SENSOR_THR_CFG7;


	/* Install the CT interrupt handler */
    ADI_INSTALL_HANDLER(CAP_IRQn, CapTouch_Int_Handler);

     /* Enable the Cortex Interrupt */
     NVIC_EnableIRQ(hDevice->IRQn);

    /* address of the device data block is the handle */
    *phDevice = hDevice;

     return ADI_CT_RESULT_SUCCESS;
}



/*!
 * @brief            Uninitialize the CT device.
 *
 * @param[in]        hDevice   Device handle obtained from #adi_CT_Init().
 *
 * @return            Status
 *
 *                   - #ADI_CT_RESULT_SUCCESS                        Call completed successfully.
 *                   - #ADI_CT_RESULT_ERR                            Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 */


ADI_CT_RESULT_TYPE adi_CT_UnInit(ADI_CT_DEV_HANDLE const hDevice)
{



   #ifdef ADI_DEBUG

   if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
   {
      return(ADI_CT_RESULT_ERR);
   }

   #endif

     /* Turn OFF CTCLK */
     SystemEnableClock(ADI_SYS_CLOCK_GATE_CTCLK, false);


     /* uninitialize internal device data */
    hDevice->pCTRegs    = NULL;
     hDevice->DrvState = ADI_CT_DRV_STATE_UNKNOWN;
    hDevice->IRQn       = (IRQn_Type) 0;
    hDevice->cbFunc     = NULL;
    hDevice->cbWatch    = (ADI_CT_INTERRUPT_TYPE) 0;

     /* Enable the Cortex Interrupt */
     NVIC_DisableIRQ(hDevice->IRQn);

     /* UnInstall the CT interrupt handlers */
    ADI_UNINSTALL_HANDLER(CAP_IRQn);

    return ADI_CT_RESULT_SUCCESS;
}



/*!
 * @brief       Enable the Captouch Pins
 *
 * @param[in]   hDevice    Device handle obtained from #adi_CT_Init().
 * @param[in]   Enable     True or False, to enable or disable the Captouch
 *
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                        Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                            Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     The function programs the CT bits to enable the Captouch signals.
 *              True  -> Enable
 *              False -> Disable
 *
 */


ADI_CT_RESULT_TYPE adi_CT_SetEnable(ADI_CT_DEV_HANDLE const hDevice,bool_t const Enable)
{


 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

 #endif

/* protected critical section */
ADI_ENTER_CRITICAL_REGION();

{
  if(Enable)
  {
      /* Enable the CT bits */
      hDevice->pCTRegs->CT_CFG3 |= BITM_CT_CT_CFG3_DIG_CTOV_CAPTOUCH_EN;
  }
  else
  {
      /* Disable the CT bits */
      hDevice->pCTRegs->CT_CFG3 &= ~(BITM_CT_CT_CFG3_DIG_CTOV_CAPTOUCH_EN);

  }

}
ADI_EXIT_CRITICAL_REGION();


  return ADI_CT_RESULT_SUCCESS;

}

/*!
 * @brief       Get the Captouch Pins Status
 *
 * @param[in]   hDevice    Device handle obtained from #adi_CT_Init().
 *
 * @param[out]  Status     Captouch Enable Status().
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                        Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                            Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     The function indicates if the Captouch Inputs are enable
 *              True  -> Enable
 *              False -> Disable
 *
 */


ADI_CT_RESULT_TYPE adi_CT_GetEnable(ADI_CT_DEV_HANDLE const hDevice,bool_t *Status)
{


 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

  *Status = ((hDevice->pCTRegs->CT_CFG3 & BITM_CT_CT_CFG3_DIG_CTOV_CAPTOUCH_EN)) ? true : false;


  return ADI_CT_RESULT_SUCCESS;

}

/*!
 * @brief       Power Up/Down the Captouch
 *
 * @param[in]   hDevice     Device handle obtained from #adi_CT_Init().
 * @param[in]   Enable      True or False, to power up or down the Captouch
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                        Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                            Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     The function sets the power Captouch status
 *              True -> Enable
 *              False-> Disable
 *
 */


ADI_CT_RESULT_TYPE adi_CT_SetPowerStatus(ADI_CT_DEV_HANDLE const hDevice,bool_t const Enable)
{


 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

   if(Enable)
  {
    hDevice->pCTRegs->CT_CDC_PWR &= ~(BITM_CT_CT_CDC_PWR_PWR_MODE);
  }
  else
  {
    hDevice->pCTRegs->CT_CDC_PWR |= BITM_CT_CT_CDC_PWR_PWR_MODE;
  }


  return ADI_CT_RESULT_SUCCESS;

}


/*!
 * @brief       Get Power Captouch Status
 *
 * @param[in]   hDevice     Device handle obtained from #adi_CT_Init().
 *
 * @param[out]  Status     Captouch Power Status.
 *
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                        Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                            Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     The function gets the power Captouch status
 *              True  -> Enable
 *              False -> Disable
 *
 */

ADI_CT_RESULT_TYPE adi_CT_GetPowerStatus(ADI_CT_DEV_HANDLE const hDevice,bool_t *Status)
{



 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

    *Status =  (hDevice->pCTRegs->CT_CDC_PWR & BITM_CT_CT_CDC_PWR_PWR_MODE) ? false : true;


  return ADI_CT_RESULT_SUCCESS;

}


/*!
 * @brief       Set the Generic Settings
 *
 * @param[in]   hDevice                    Device handle obtained from #adi_CT_Init().
 * @param[in]  pGenericSettings           Generic Captouch Settings:   #ADI_CT_C2V_TIMING_TYPE
 *                                                                      #ADI_CT_PK2PK_TYPE
 *                                                                      #ADI_CT_AVERAGE_TYPE
 *                                                                      #ADI_CT_BASELINE_TYPE
 *                                                                      #ADI_CT_TOUCH_AND_RELEASE_TYPE
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     Alternative way to configure the generic captouch settings. Includes the C2V_timing, PK2PK, Average,
 *              Baseline and Touch and Release Setteings. Each of them can be configured using its own set function.
 *
 */

ADI_CT_RESULT_TYPE adi_CT_SetGenericSettings(ADI_CT_DEV_HANDLE const hDevice,ADI_CT_GENERIC_SETTINGS_TYPE* const pGenericSettings){



 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif


      /* Average Settings */
      adi_CT_SetAverageCfg(hDevice,&(pGenericSettings->AverageSettings));

      /* Baseline Settings */
      adi_CT_SetBaselineCfg(hDevice,&(pGenericSettings->BaselineSettings));

      /* TIming Conversion Settings */
      adi_CT_SetTimingsCfg(hDevice,&(pGenericSettings->C2V_TimingSettings));

      /* Touch Settings */
      adi_CT_SetTouchCfg(hDevice,&(pGenericSettings->TouchSettings));

      /* Release Settings */
      adi_CT_SetReleaseCfg(hDevice,&(pGenericSettings->ReleaseSettings));

      /* Set PK2PK Settings */
      adi_CT_SetPK2PKCfg(hDevice,&(pGenericSettings->PK2PKSettings));


  return ADI_CT_RESULT_SUCCESS;

}

/*!
 * @brief       Set the Number of Integration Cycles
 *
 * @param[in]   hDevice             Device handle obtained from #adi_CT_Init().
 * @param[in]   IntCycles           Number of Integration Cycles
 *
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     The number of Integration Cycle is set by IntCycles.
 *              IntCycles must be a #ADI_CT_INTCYCLES_TYPE variable
 *
 */

ADI_CT_RESULT_TYPE adi_CT_SetIntCycles(ADI_CT_DEV_HANDLE const hDevice,ADI_CT_INTCYCLES_TYPE const IntCycles){



 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif


  uint32_t reg_value;

  /* protected critical section */
  ADI_ENTER_CRITICAL_REGION();

  {
    reg_value = hDevice->pCTRegs->CT_CFG1;
    reg_value &= ~(BITM_CT_CT_CFG1_INT_CYCLE);
    reg_value |= IntCycles << BITP_CT_CT_CFG1_INT_CYCLE;
    hDevice->pCTRegs->CT_CFG1 = reg_value;

  }
  ADI_EXIT_CRITICAL_REGION();

  return ADI_CT_RESULT_SUCCESS;

}

/*!
 * @brief       Get the Number of Integration Cycles
 *
 * @param[in]   hDevice       Device handle obtained from #adi_CT_Init().
 *
 * @param[out]  IntCycles     Number Of Integration Cycles().
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     The Number of Integration Cycles is get by IntCycles.
 *              IntCycles must be a #ADI_CT_INTCYCLES_TYPE variable
 *
 */

ADI_CT_RESULT_TYPE adi_CT_GetIntCycles(ADI_CT_DEV_HANDLE const hDevice,ADI_CT_INTCYCLES_TYPE *IntCycles)

{

 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

  *IntCycles = (ADI_CT_INTCYCLES_TYPE) ((hDevice->pCTRegs->CT_CFG1 & BITM_CT_CT_CFG1_INT_CYCLE) >> BITP_CT_CT_CFG1_INT_CYCLE);

  return ADI_CT_RESULT_SUCCESS;

}

/*!
 * @brief       Set the Unmatched Pads Configuration
 *
 * @param[in]   hDevice             Device handle obtained from #adi_CT_Init().
 * @param[in]   UnmatchedPads       Unmatched Pads configuration
 *
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     The Unmatched Pads Configuration is set by UnmatchedPads.
 *              UnmatchedPads must be a #ADI_CT_UNMATCHED_PADS_TYPE variable
 *
 */

ADI_CT_RESULT_TYPE adi_CT_SetUnmatchedPads(ADI_CT_DEV_HANDLE const hDevice,ADI_CT_UNMATCHED_PADS_TYPE const UnmatchedPads){



 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

  uint32_t reg_value;

  /* protected critical section */
  ADI_ENTER_CRITICAL_REGION();
  {
	  reg_value = hDevice->pCTRegs->CT_CFG1;
	  reg_value &= ~(BITM_CT_CT_CFG1_AIN_SEL);
	  reg_value |= UnmatchedPads << BITP_CT_CT_CFG1_AIN_SEL;
	  hDevice->pCTRegs->CT_CFG1 = reg_value;
  }
  ADI_EXIT_CRITICAL_REGION();

  return ADI_CT_RESULT_SUCCESS;

}

/*!
 * @brief       Get the Number of Integration Cycles
 *
 * @param[in]   hDevice             Device handle obtained from #adi_CT_Init().
 *
 * @param[out]  UnmatchedPads       Unmatched Pads configuration
 *
 * @return      Status
 *              - #ADI_CT_UNMATCHED_PADS_TYPE       Number of Integration Cycles
 *              - #ADI_CT_RESULT_ERR                Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     The Unmatched Pads Configuration is get by UnmatchedPads.
 *              UnmatchedPads must be a #ADI_CT_UNMATCHED_PADS_TYPE variable
 *
 */

ADI_CT_RESULT_TYPE adi_CT_GetUnmatchedPads(ADI_CT_DEV_HANDLE const hDevice,ADI_CT_UNMATCHED_PADS_TYPE *UnmatchedPads){



 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

  *UnmatchedPads = (ADI_CT_UNMATCHED_PADS_TYPE) ((hDevice->pCTRegs->CT_CFG1 & BITM_CT_CT_CFG1_AIN_SEL) >> BITP_CT_CT_CFG1_AIN_SEL);


  return ADI_CT_RESULT_SUCCESS;

}



/*!
 * @brief       Set the Conversion Timings Parameters
 *
 * @param[in]   hDevice             Device handle obtained from #adi_CT_Init().
 * @param[in]   pTimingSettings      Struct which includes the different timings values
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                            Error.
 *
 * @details     The Timing configuration is set by pTimingSettings.
 *              pTimingSettings must be a #ADI_CT_C2V_TIMING_TYPE structs which includes:
 *                      AutozeroDuty        #ADI_CT_AUTOZERODUTY_TYPE
 *                      C2VHoldTime         Capaticitance to Voltage Time. 5 bits, each LSB represents 0.25us, range [0.25-8]us.
 *                      Phase13             Phase13 Time. 5 bits, each LSB represents 0.25us, range [0.25-8]us.
 *                      Phase24             Phase24 Time. 8 bits, each LSB represents 0.25us, range [0.0625-16]us.
 *
 *
 */

ADI_CT_RESULT_TYPE adi_CT_SetTimingsCfg(ADI_CT_DEV_HANDLE const hDevice,ADI_CT_C2V_TIMING_TYPE* const pTimingSettings){



 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

  uint32_t reg_value;


    /* protected critical section */
  ADI_ENTER_CRITICAL_REGION();

  {
    reg_value = hDevice->pCTRegs->CT_CFG1;
    reg_value &= ~(BITM_CT_CT_CFG1_AUTOZERO_DUTY);
    reg_value |= pTimingSettings->AutozeroDuty << BITP_CT_CT_CFG1_AUTOZERO_DUTY;
    hDevice->pCTRegs->CT_CFG1 = reg_value;

    reg_value = hDevice->pCTRegs->CT_CFG2;
    reg_value &= ~(BITM_CT_CT_CFG2_C2V_HOLD_TIME);
    reg_value |= pTimingSettings->C2VHoldTime << BITP_CT_CT_CFG2_C2V_HOLD_TIME;
    hDevice->pCTRegs->CT_CFG2 = reg_value;

    reg_value = hDevice->pCTRegs->CT_CFG2;
    reg_value &= ~(BITM_CT_CT_CFG2_PH13_DUTY);
    reg_value |= pTimingSettings->Phase13 << BITP_CT_CT_CFG2_PH13_DUTY;
    hDevice->pCTRegs->CT_CFG2 = reg_value;

    reg_value = hDevice->pCTRegs->CT_CFG2;
    reg_value &= ~(BITM_CT_CT_CFG2_PH24_DUTY);
    reg_value |= pTimingSettings->Phase24 << BITP_CT_CT_CFG2_PH24_DUTY;
    hDevice->pCTRegs->CT_CFG2 = reg_value;

  }
  ADI_EXIT_CRITICAL_REGION();

  return ADI_CT_RESULT_SUCCESS;

}

/*!
 * @brief       Get the Conversion Timings Parameters
 *
 * @param[in]   hDevice             Device handle obtained from #adi_CT_Init().
 *
 * @param[out]   TimingSettings    Struct which includes the different timings values
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     The Timing configuration is get by TimingSettings.
 *              TimingSettings will be get as a #ADI_CT_C2V_TIMING_TYPE struct which includes:
 *                      AutozeroDuty        #ADI_CT_AUTOZERODUTY_TYPE
 *                      C2VHoldTime         Capaticitance to Voltage Time. 5 bits, each LSB represents 0.25us, range [0.25-8]us.
 *                      Phase13             Phase13 Time. 5 bits, each LSB represents 0.25us, range [0.25-8]us.
 *                      Phase24             Phase24 Time. 8 bits, each LSB represents 0.25us, range [0.0625-16]us.
 *
 *
 ***********************************************************************************/

ADI_CT_RESULT_TYPE adi_CT_GetTimingsCfg(ADI_CT_DEV_HANDLE const hDevice,ADI_CT_C2V_TIMING_TYPE *TimingSettings){



 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

    uint32_t ct_cfg1_value = hDevice->pCTRegs->CT_CFG1;
    uint32_t ct_cfg2_value = hDevice->pCTRegs->CT_CFG2;

   TimingSettings->AutozeroDuty = (ADI_CT_AUTOZERODUTY_TYPE) ((ct_cfg1_value & BITM_CT_CT_CFG1_AUTOZERO_DUTY) >> BITP_CT_CT_CFG1_AUTOZERO_DUTY);

   TimingSettings->C2VHoldTime  = (ct_cfg2_value & BITM_CT_CT_CFG2_C2V_HOLD_TIME) >> BITP_CT_CT_CFG2_C2V_HOLD_TIME;

   TimingSettings->Phase13      = (ct_cfg2_value & BITM_CT_CT_CFG2_PH13_DUTY) >> BITP_CT_CT_CFG2_PH13_DUTY;

   TimingSettings->Phase24      = (ct_cfg2_value & BITM_CT_CT_CFG2_PH24_DUTY) >> BITP_CT_CT_CFG2_PH24_DUTY;

   return ADI_CT_RESULT_SUCCESS;

}

/*!
 * @brief       Enable/Disable the Self Timer
 *
 * @param[in]   hDevice             Device handle obtained from #adi_CT_Init().
 * @param[in]   Enable              SelfTimer status
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     The function sets the Self Timer status
 *              True  -> Enable
 *              False -> Disable
 *
 */

ADI_CT_RESULT_TYPE adi_CT_SetSelfTimerEnable(ADI_CT_DEV_HANDLE const hDevice,bool_t const Enable)
{



 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif


    if(Enable)
  {
    hDevice->pCTRegs->CT_CFG2 |= BITM_CT_CT_CFG2_STIMER_EN;
  }
  else
  {
    hDevice->pCTRegs->CT_CFG2 &= ~(BITM_CT_CT_CFG2_STIMER_EN);
  }


  return ADI_CT_RESULT_SUCCESS;

}

/*!
 * @brief       Get the Self Timer Status
 *
 * @param[in]   hDevice             Device handle obtained from #adi_CT_Init().
 *
 * @param[out]  Status              SelfTimer status
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     The function gets the Self Timer status
 *              True  -> Enable
 *              False -> Disable
 *
 */

ADI_CT_RESULT_TYPE adi_CT_GetSelfTimerEnable(ADI_CT_DEV_HANDLE const hDevice,bool_t *Status)
{



 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

  *Status = (hDevice->pCTRegs->CT_CFG2 & BITM_CT_CT_CFG2_STIMER_EN) ? true : false;

  return ADI_CT_RESULT_SUCCESS;

}


/*!
 * @brief       Set the Self Timer Value
 *
 * @param[in]   hDevice             Device handle obtained from #adi_CT_Init().
 * @param[in]   SelfTimerWait       Digital Self Timer Value.
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     The Self Timer Value configuration is set by SelfTimerWait.
 *              Each LSB corresponds to 1.95ms. Range [0-998]ms.
 *
 */


ADI_CT_RESULT_TYPE adi_CT_SetSelfTimerValue(ADI_CT_DEV_HANDLE const hDevice,uint16_t const SelfTimerWait)
{



 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

  uint32_t reg_value;


  /* protected critical section */
  ADI_ENTER_CRITICAL_REGION();
  {
	  reg_value = hDevice->pCTRegs->CT_CFG2;
	  reg_value &= ~(BITM_CT_CT_CFG2_SELF_TIMER_WAIT);
	  reg_value |= SelfTimerWait << BITP_CT_CT_CFG2_SELF_TIMER_WAIT;
	  hDevice->pCTRegs->CT_CFG2 = reg_value;
  }
  ADI_EXIT_CRITICAL_REGION();

  return ADI_CT_RESULT_SUCCESS;

}


/*!
 * @brief       Get the Self Timer Value
 *
 * @param[in]   hDevice             Device handle obtained from #adi_CT_Init().
 *
 * @param[out]  SelfTimerWait       Digital Self Timer Value.
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     The Self Timer Value configuration is get by SelfTimerWait.
 *              Each LSB corresponds to 1.95ms. Range [0-998]ms.
 *
 */

ADI_CT_RESULT_TYPE adi_CT_GetSelfTimerValue(ADI_CT_DEV_HANDLE const hDevice,uint16_t *SelfTimerWait)
{

 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif


  *SelfTimerWait = hDevice->pCTRegs->CT_CFG2 & BITM_CT_CT_CFG2_SELF_TIMER_WAIT >> BITP_CT_CT_CFG2_SELF_TIMER_WAIT;


  return ADI_CT_RESULT_SUCCESS;

}

/*!
 * @brief       Bypass the gain for all stages
 *
 * @param[in]   hDevice             Device handle obtained from #adi_CT_Init().
 * @param[in]   ByPassGain          By Pass the PGA Gain
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     The function bypasses the gain for all stages, independently
 *              the set value in each stage.
 *                      True  -> Override
 *                      False -> Not Override
 *
 */


ADI_CT_RESULT_TYPE adi_CT_SetByPassGain(ADI_CT_DEV_HANDLE const hDevice,bool_t const ByPassGain)
{



 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

  if(ByPassGain)
  {
    hDevice->pCTRegs->CT_CFG1 |= BITM_CT_CT_CFG1_BYPASS_GAIN;
  }
  else
  {
    hDevice->pCTRegs->CT_CFG1 &= ~(BITM_CT_CT_CFG1_BYPASS_GAIN);

  }

  return ADI_CT_RESULT_SUCCESS;

}

/*!
 * @brief       Bypass the gain for all stages
 *
 * @param[in]   hDevice             Device handle obtained from #adi_CT_Init().

 * @param[out]  Status              PGA Gain Status
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     The function gets PGA gain status for all stages, independently
 *              the set value in each stage.
 *                      True  -> Override
 *                      False -> Not Override
 *
 */

ADI_CT_RESULT_TYPE adi_CT_GetByPassGain(ADI_CT_DEV_HANDLE const hDevice,bool_t *Status)
{



 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

    *Status = (hDevice->pCTRegs->CT_CFG1 & BITM_CT_CT_CFG1_BYPASS_GAIN) ? true : false;

  return ADI_CT_RESULT_SUCCESS;

}

/*!
 * @brief       Set the Internal Buffer Bias Current
 *
 * @param[in]   hDevice             Device handle obtained from #adi_CT_Init().
 * @param[in]   IntBufferBias       Int Buffer Bias Current
 *
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     The Internal Buffer Bias Current is set by IntBufferBias.
 *              IntBufferBias must be a #ADI_CT_INT_BUFFER_BIAS_TYPE variable
 *
 */

ADI_CT_RESULT_TYPE adi_CT_SetIntBufferBiasCurrent(ADI_CT_DEV_HANDLE const hDevice,ADI_CT_INT_BUFFER_BIAS_TYPE const IntBufferBias)
{



 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

    uint32_t reg_value;

    /* protected critical section */
  ADI_ENTER_CRITICAL_REGION();
  {
	  reg_value = hDevice->pCTRegs->CT_CFG1;
	  reg_value &= ~(BITM_CT_CT_CFG1_INT_BUFFER);
	  reg_value |= IntBufferBias << BITP_CT_CT_CFG1_INT_BUFFER;
	  hDevice->pCTRegs->CT_CFG1 = reg_value;
  }
  ADI_EXIT_CRITICAL_REGION();


  return ADI_CT_RESULT_SUCCESS;

}

/*!
 * @brief       Get the Internal Buffer Bias Current
 *
 * @param[in]   hDevice             Device handle obtained from #adi_CT_Init().
 *
 * @param[out]  IntBufferBias       Int Buffer Bias Current
 *
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     The Internal Buffer Bias Current is get by IntBufferBias.
 *              IntBufferBias must be a #ADI_CT_INT_BUFFER_BIAS_TYPE variable
 *
 */

ADI_CT_RESULT_TYPE adi_CT_GetIntBufferBiasCurrent(ADI_CT_DEV_HANDLE const hDevice,ADI_CT_INT_BUFFER_BIAS_TYPE *IntBufferBias)
{



 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

  *IntBufferBias = (ADI_CT_INT_BUFFER_BIAS_TYPE) (hDevice->pCTRegs->CT_CFG1 & BITM_CT_CT_CFG1_INT_BUFFER >> BITP_CT_CT_CFG1_INT_BUFFER);

  return ADI_CT_RESULT_SUCCESS;

}

/*!
 * @brief       Set the PGA Bias Current
 *
 * @param[in]   hDevice             Device handle obtained from #adi_CT_Init().
 * @param[in]   PGABias             PGA Bias Current
 *
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     The PGA Bias Current is set by PGABias.
 *              PGABias must be a #ADI_CT_PGA_BIAS_TYPE variable
 *
 */

ADI_CT_RESULT_TYPE adi_CT_SetPGABiasCurrent(ADI_CT_DEV_HANDLE const hDevice,ADI_CT_PGA_BIAS_TYPE const PGABias)
{



 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

  uint32_t reg_value;

    /* protected critical section */
  ADI_ENTER_CRITICAL_REGION();
  {
	  reg_value = hDevice->pCTRegs->CT_CFG1;
	  reg_value &= ~(BITM_CT_CT_CFG1_PGA_BIAS);
	  reg_value |= PGABias << BITP_CT_CT_CFG1_PGA_BIAS;
	  hDevice->pCTRegs->CT_CFG1 = reg_value;
  }
  ADI_EXIT_CRITICAL_REGION();

  return ADI_CT_RESULT_SUCCESS;

}

/*!
 * @brief       Get the PGA Bias Current
 *
 * @param[in]   hDevice             Device handle obtained from #adi_CT_Init().
 *
 * @param[out]  PGABias             PGA Bias Current
 *
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     The PGA Bias Current is get by PGABias.
 *              PGABias must be a #ADI_CT_PGA_BIAS_TYPE variable
 *
 */

ADI_CT_RESULT_TYPE adi_CT_GetPGABiasCurrent(ADI_CT_DEV_HANDLE const hDevice,ADI_CT_PGA_BIAS_TYPE *PGABias)
{



 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

  *PGABias= (ADI_CT_PGA_BIAS_TYPE) (hDevice->pCTRegs->CT_CFG1 & BITM_CT_CT_CFG1_PGA_BIAS >> BITP_CT_CT_CFG1_PGA_BIAS);

  return ADI_CT_RESULT_SUCCESS;

}


/*!
 * @brief       Set the C2V Bias Current
 *
 * @param[in]   hDevice             Device handle obtained from #adi_CT_Init().
 * @param[in]   C2VBias             C2V Bias Current
 *
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     The C2V Bias Current is set by C2VBias.
 *              C2VBias must be a #ADI_CT_C2V_BIAS_TYPE variable
 *
 */

ADI_CT_RESULT_TYPE adi_CT_SetC2VBiasCurrent(ADI_CT_DEV_HANDLE const hDevice,ADI_CT_C2V_BIAS_TYPE const C2VBias)
{



 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

    uint32_t reg_value;

 /* protected critical section */
  ADI_ENTER_CRITICAL_REGION();
  {
	  reg_value = hDevice->pCTRegs->CT_CFG1;
	  reg_value &= ~(BITM_CT_CT_CFG1_C2V_BIAS);
	  reg_value |= C2VBias << BITP_CT_CT_CFG1_C2V_BIAS;
	  hDevice->pCTRegs->CT_CFG1 = reg_value;
  }
  ADI_EXIT_CRITICAL_REGION();

  return ADI_CT_RESULT_SUCCESS;

}

/*!
 * @brief       Get the C2V Bias Current
 *
 * @param[in]   hDevice             Device handle obtained from #adi_CT_Init().
 *
 * @param[out]  C2VBias             C2V Bias Current
 *
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     The C2V Bias Current is get by C2VBias.
 *              C2VBias must be a #ADI_CT_C2V_BIAS_TYPE variable
 *
 */


ADI_CT_RESULT_TYPE adi_CT_GetC2VBiasCurrent(ADI_CT_DEV_HANDLE const hDevice,ADI_CT_C2V_BIAS_TYPE *C2VBias)
{



 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

  *C2VBias = (ADI_CT_C2V_BIAS_TYPE) ((hDevice->pCTRegs->CT_CFG1 & BITM_CT_CT_CFG1_C2V_BIAS) >> BITP_CT_CT_CFG1_C2V_BIAS);

  return ADI_CT_RESULT_SUCCESS;

}



/*!
 * @brief       Set the Peak to Peak Parameters
 *
 * @param[in]   hDevice                 Device handle obtained from #adi_CT_Init().
 * @param[in]   pPK2PK_Settings          Struct which includes the different PK2PK parameters

 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     The Peak to Peak Settings are set by pPK2PK_Settings.
 *              pPK2PK_Settings must be a #ADI_CT_PK2PK_TYPE structs which includes:
 *                          - PK2PKAvg              Number of Samples to be averaged
 *                          - PK2PKAvgMin           Minimum rank position for averaging
 *                          - PK2PKSubset           Subset of Number of Samples to be used for the calculations
 *                          - PK2PKNumSamples       Number of Samples to be used for the calculations
 *
 */

ADI_CT_RESULT_TYPE adi_CT_SetPK2PKCfg(ADI_CT_DEV_HANDLE const hDevice,ADI_CT_PK2PK_TYPE* const pPK2PK_Settings)
{



 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

    uint32_t reg_value;

    uint32_t mask = BITM_CT_CT_CFG3_PK2PK_AVG     |
                    BITM_CT_CT_CFG3_PK2PK_AVG_MIN |
                    BITM_CT_CT_CFG3_PK2PK_SUBSET  |
                    BITM_CT_CT_CFG3_PK2PK_NUM_SPLS  ;

    uint32_t cfg_value = pPK2PK_Settings->PK2PKAvg        << BITP_CT_CT_CFG3_PK2PK_AVG        |
                         pPK2PK_Settings->PK2PKAvgMin     << BITP_CT_CT_CFG3_PK2PK_AVG_MIN    |
                         pPK2PK_Settings->PK2PKAvgMin     << BITP_CT_CT_CFG3_PK2PK_SUBSET     |
                         pPK2PK_Settings->PK2PKNumSamples << BITP_CT_CT_CFG3_PK2PK_NUM_SPLS;

     /* protected critical section */
  ADI_ENTER_CRITICAL_REGION();
  {
	  reg_value = hDevice->pCTRegs->CT_CFG3;
	  reg_value &= ~(mask);
	  reg_value |= cfg_value;
	  hDevice->pCTRegs->CT_CFG1 = reg_value;
  }
  ADI_EXIT_CRITICAL_REGION();

  return ADI_CT_RESULT_SUCCESS;

}

/*!
 * @brief       Get the Peak to Peak Parameters
 *
 * @param[in]   hDevice                 Device handle obtained from #adi_CT_Init().
 *
 * @param[out]  PK2PK_Settings          Struct which includes the different PK2PK parameters

 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     The Peak to Peak Settings are get by PK2PK_Settings.
 *              PK2PK_Settings will be get as a #ADI_CT_PK2PK_TYPE struct which includes:
 *                          - PK2PKAvg              Number of Samples to be averaged
 *                          - PK2PKAvgMin           Minimum rank position for averaging
 *                          - PK2PKSubset           Subset of Number of Samples to be used for the calculations
 *                          - PK2PKNumSamples       Number of Samples to be used for the calculations
 *
 */


ADI_CT_RESULT_TYPE adi_CT_GetPK2PKCfg(ADI_CT_DEV_HANDLE const hDevice,ADI_CT_PK2PK_TYPE *PK2PK_Settings)
{



 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif


   uint32_t reg_value = hDevice->pCTRegs->CT_CFG3;

   PK2PK_Settings->PK2PKAvg        = (ADI_CT_PK2PK_AVG_TYPE) ((reg_value & BITM_CT_CT_CFG3_PK2PK_AVG) >> BITP_CT_CT_CFG3_PK2PK_AVG);

   PK2PK_Settings->PK2PKAvgMin     = (reg_value & BITM_CT_CT_CFG3_PK2PK_AVG_MIN) >> BITP_CT_CT_CFG3_PK2PK_AVG_MIN;

   PK2PK_Settings->PK2PKSubset     = (reg_value & BITM_CT_CT_CFG3_PK2PK_SUBSET) >> BITP_CT_CT_CFG3_PK2PK_SUBSET;

   PK2PK_Settings->PK2PKNumSamples = (ADI_CT_PK2PK_NUM_TYPE) ((reg_value & BITM_CT_CT_CFG3_PK2PK_NUM_SPLS) >> BITP_CT_CT_CFG3_PK2PK_NUM_SPLS);


  return ADI_CT_RESULT_SUCCESS;

}

/*!
 * @brief       Enable/Disable the C2V Low Pass Filter
 *
 * @param[in]   hDevice             Device handle obtained from #adi_CT_Init().
 * @param[in]   Enable              Enable the C2V Low Pass Filter
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     The function enables the C2V Low Pass Filter
 *                      True  -> Enable
 *                      False -> Disable
 *
 */

ADI_CT_RESULT_TYPE adi_CT_SetC2VLowPassFilterEnable(ADI_CT_DEV_HANDLE const hDevice,bool_t const Enable)
{



 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

   if(Enable)
  {
    hDevice->pCTRegs->CT_CFG2 |= BITM_CT_CT_CFG2_C2V_LPF;
  }
  else
  {
    hDevice->pCTRegs->CT_CFG2 &= ~(BITM_CT_CT_CFG2_C2V_LPF);
  }


  return ADI_CT_RESULT_SUCCESS;

}

/*!
 * @brief       Get the C2V Low Pass Filter Status
 *
 * @param[in]   hDevice             Device handle obtained from #adi_CT_Init().
 *
 * @param[out]  Status              C2V Low Pass Filter Status
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     The function gets the C2V Low Pass Filter Status
 *                      True  -> Enable
 *                      False -> Disable
 *
 */

ADI_CT_RESULT_TYPE adi_CT_GetC2VLowPassFilterEnable(ADI_CT_DEV_HANDLE const hDevice,bool_t *Status)
{



 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

    *Status = (hDevice->pCTRegs->CT_CFG2 & BITM_CT_CT_CFG2_C2V_LPF) ? true : false;

  return ADI_CT_RESULT_SUCCESS;

}


/*!
 * @brief       Set the Value stored in the CDC_RES register
 *
 * @param[in]   hDevice             Device handle obtained from #adi_CT_Init().
 * @param[in]   ResSel              CDC_RESULT_VALUE/CDC_BASELINE_DIFFERENCE_VALUE
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     The Value stored in the CDC_RES register is set by ResSel.
 *              ResSel must be a #ADI_CT_CDC_RESULT_TYPE variable
 *                      -CDC_RESULT_VALUE               -> CDC Value
 *                      -CDC_BASELINE_DIFFERENCE_VALUE  -> CDC Value - Baseline Value
 */


ADI_CT_RESULT_TYPE adi_CT_SetCDCResultFormat(ADI_CT_DEV_HANDLE const hDevice,ADI_CT_CDC_RESULT_TYPE const ResSel)
{



 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

  if(ResSel == ADI_CT_CDC_RESULT_VALUE)
  {
    hDevice->pCTRegs->CT_CFG3 &= ~(BITM_CT_CT_CFG3_RES_SEL);
  }
  else
  {
    hDevice->pCTRegs->CT_CFG3 |= BITM_CT_CT_CFG3_RES_SEL;
  }


  return ADI_CT_RESULT_SUCCESS;

}

/*!
 * @brief       Get the Value stored in the CDC_RES register
 *
 * @param[in]   hDevice             Device handle obtained from #adi_CT_Init().
 *
 * @param[out]   ResSel              CDC_RESULT_VALUE/CDC_BASELINE_DIFFERENCE_VALUE
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     The Value stored in the CDC_RES register is Get by ResSel.
 *              ResSel will be get as a #ADI_CT_CDC_RESULT_TYPE variable
 *                      -CDC_RESULT_VALUE               -> CDC Value
 *                      -CDC_BASELINE_DIFFERENCE_VALUE  -> CDC Value - Baseline Value
 */

ADI_CT_RESULT_TYPE adi_CT_GetCDCResultFormat(ADI_CT_DEV_HANDLE const hDevice,ADI_CT_CDC_RESULT_TYPE *ResSel)
{



 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

    uint32_t reg_value = hDevice->pCTRegs->CT_CFG3;

  if( reg_value & BITM_CT_CT_CFG3_RES_SEL == ADI_CT_CDC_RESULT_VALUE)
  {
    *ResSel = ADI_CT_CDC_RESULT_VALUE;
  }
  else
  {
    *ResSel = ADI_CT_CDC_BASELINE_DIFFERENCE_VALUE;
  }


  return ADI_CT_RESULT_SUCCESS;

}



/*!
 * @brief       Enable/Disable the IIR Filter
 *
 * @param[in]   hDevice             Device handle obtained from #adi_CT_Init().
 * @param[in]   Enable              Enable the IIR Filter
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     The function sets the IIR Filter Status
 *                      True  -> Enable
 *                      False -> Disable
 *
 */

ADI_CT_RESULT_TYPE adi_CT_SetIIRFilterEnable(ADI_CT_DEV_HANDLE const hDevice,bool_t const Enable)
{



 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

  if(Enable)
  {
    hDevice->pCTRegs->CT_CFG3 |= BITM_CT_CT_CFG3_IIR_EN;
  }
  else
  {
    hDevice->pCTRegs->CT_CFG3 &= ~(BITM_CT_CT_CFG3_IIR_EN);
  }

  return ADI_CT_RESULT_SUCCESS;

}


/*!
 * @brief       Get the IIR Filter Status
 *
 * @param[in]   hDevice             Device handle obtained from #adi_CT_Init().
 *
 * @param[out]  Status              IIR Filter Status
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     The function gets the IIR Filter Status
 *                      True  -> Enable
 *                      False -> Disable
 *
 */

ADI_CT_RESULT_TYPE adi_CT_GetIIRFilterEnable(ADI_CT_DEV_HANDLE const hDevice,bool_t *Status)
{



 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

    *Status = (hDevice->pCTRegs->CT_CFG3 & BITM_CT_CT_CFG3_IIR_EN) ? true : false;

  return ADI_CT_RESULT_SUCCESS;

}



 /*!
 * @brief       Set the IIR Filter Weight
 *
 * @param[in]   hDevice             Device handle obtained from #adi_CT_Init().
 * @param[in]   IIRWeight           IIR order [0-15]
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     The IIR Filter Weight must be an uint8_t
 *
 */

ADI_CT_RESULT_TYPE adi_CT_SetIIRFilterWeight(ADI_CT_DEV_HANDLE const hDevice,uint8_t const IIRWeight)
{



 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif


  uint32_t reg_value;

 /* protected critical section */
  ADI_ENTER_CRITICAL_REGION();
  {
	  reg_value = hDevice->pCTRegs->CT_CFG3;
	  reg_value &= ~(BITM_CT_CT_CFG3_IIR_WEIGHT);
	  reg_value |= IIRWeight << BITP_CT_CT_CFG3_IIR_WEIGHT;
	  hDevice->pCTRegs->CT_CFG3 = reg_value;
  }
  ADI_EXIT_CRITICAL_REGION();

  return ADI_CT_RESULT_SUCCESS;

}

/*!
 * @brief       Get the IIR Filter Weight
 *
 * @param[in]    hDevice             Device handle obtained from #adi_CT_Init().
 *
 * @param[out]   IIRWeight           IIR order [0-15]
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     The IIR Filter Weight will be get as an uint8_t
 *
 */

ADI_CT_RESULT_TYPE adi_CT_GetIIRFilterWeight(ADI_CT_DEV_HANDLE const hDevice,uint8_t *IIRWeight)
{



 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif


  *IIRWeight = (hDevice->pCTRegs->CT_CFG3 & BITM_CT_CT_CFG3_IIR_WEIGHT) >> BITP_CT_CT_CFG3_IIR_WEIGHT;

  return ADI_CT_RESULT_SUCCESS;

}



/*!
 * @brief      Set the Average Filter Settings
 *
 * @param[in]   hDevice             Device handle obtained from #adi_CT_Init().
 * @param[in]   pAverageSettings     Struct which includes the different Average Settings
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     The Average Settings are set by pAverageSettings.
 *              pAverageSettings must be a #ADI_CT_AVERAGE_TYPE struct which includes:
 *                      RankFilt            Number of samples to be ranked. #ADI_CT_RANKING_TYPE
 *                      AvgType             Final Average Value. #ADI_CT_AVG_OUTPUT_TYPE
 *                      AvgGroup1           Number of Samples in Average Group 1.  #ADI_CT_AVG_SAMPLES_TYPE
 *                      AvgGroup2           Number of Samples in Average Group 2.  #ADI_CT_AVG_SAMPLES_TYPE
 *                      AvgMinGroup1        Minimum Rank Position to be averaged in Group1 (5bits)
 *                      AvgMinGroup2        Minimum Rank Position to be averaged in Group2 (5bits)
 *
 */


ADI_CT_RESULT_TYPE adi_CT_SetAverageCfg(ADI_CT_DEV_HANDLE const hDevice,ADI_CT_AVERAGE_TYPE* const pAverageSettings)

{


 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

    uint32_t reg_value;

    uint32_t mask = BITM_CT_CT_AVG_RANK_FILT      |
                    BITM_CT_CT_AVG_AVG_OUTPUT     |
                    BITM_CT_CT_AVG_AVG_GROUP2     |
                    BITM_CT_CT_AVG_AVG_GROUP1     |
                    BITM_CT_CT_AVG_AVG_MIN_GROUP1 |
                    BITM_CT_CT_AVG_AVG_MIN_GROUP2 ;


    uint32_t cfg_value = pAverageSettings->RankFilt     << BITP_CT_CT_AVG_RANK_FILT      |
                         pAverageSettings->AvgType      << BITP_CT_CT_AVG_AVG_OUTPUT     |
                         pAverageSettings->AvgGroup2    << BITP_CT_CT_AVG_AVG_GROUP2     |
                         pAverageSettings->AvgGroup1    << BITP_CT_CT_AVG_AVG_GROUP1     |
                         pAverageSettings->AvgMinGroup1 << BITP_CT_CT_AVG_AVG_MIN_GROUP1 |
                         pAverageSettings->AvgMinGroup2 << BITP_CT_CT_AVG_AVG_MIN_GROUP2 ;


     /* protected critical section */
  ADI_ENTER_CRITICAL_REGION();
  {
	  reg_value = hDevice->pCTRegs->CT_AVG;
	  reg_value &= ~(mask);
	  reg_value |= cfg_value;
	  hDevice->pCTRegs->CT_AVG = reg_value;
  }
  ADI_EXIT_CRITICAL_REGION();

  return ADI_CT_RESULT_SUCCESS;

}

/*!
 * @brief      Get the Average Filter Settings
 *
 * @param[in]   hDevice             Device handle obtained from #adi_CT_Init().
 *
 * @param[out]  AverageSettings     Struct which includes the different Average Settings
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     The Average Settings are get by AverageSettings.
 *              AverageSettings will get as a #ADI_CT_AVERAGE_TYPE struct which includes:
 *                      RankFilt            Number of samples to be ranked. #ADI_CT_RANKING_TYPE
 *                      AvgType             Final Average Value. #ADI_CT_AVG_OUTPUT_TYPE
 *                      AvgGroup1           Number of Samples in Average Group 1.  #ADI_CT_AVG_SAMPLES_TYPE
 *                      AvgGroup2           Number of Samples in Average Group 2.  #ADI_CT_AVG_SAMPLES_TYPE
 *                      AvgMinGroup1        Minimum Rank Position to be averaged in Group1 (5bits)
 *                      AvgMinGroup2        Minimum Rank Position to be averaged in Group2 (5bits)
 *
 */

ADI_CT_RESULT_TYPE adi_CT_GetAverageCfg(ADI_CT_DEV_HANDLE const hDevice,ADI_CT_AVERAGE_TYPE *AverageSettings)

{



 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

   uint32_t reg_value = hDevice->pCTRegs->CT_AVG;

   AverageSettings->RankFilt     = (ADI_CT_RANKING_TYPE) ((reg_value & BITM_CT_CT_AVG_RANK_FILT) >> BITP_CT_CT_AVG_RANK_FILT);

   AverageSettings->AvgType      = (ADI_CT_AVG_OUTPUT_TYPE) ((reg_value & BITM_CT_CT_AVG_AVG_OUTPUT) >> BITP_CT_CT_AVG_AVG_OUTPUT);

   AverageSettings->AvgGroup2    = (ADI_CT_AVG_SAMPLES_TYPE) ((reg_value & BITM_CT_CT_AVG_AVG_GROUP2) >> BITP_CT_CT_AVG_AVG_GROUP2);

   AverageSettings->AvgGroup1    = (ADI_CT_AVG_SAMPLES_TYPE) ((reg_value & BITM_CT_CT_AVG_AVG_GROUP1) >> BITP_CT_CT_AVG_AVG_GROUP1);

   AverageSettings->AvgMinGroup1 = (reg_value & BITM_CT_CT_AVG_AVG_MIN_GROUP1) >> BITP_CT_CT_AVG_AVG_MIN_GROUP1;

   AverageSettings->AvgMinGroup2 = (reg_value & BITM_CT_CT_AVG_AVG_MIN_GROUP2) >> BITP_CT_CT_AVG_AVG_MIN_GROUP2;


  return ADI_CT_RESULT_SUCCESS;

}



/*!
 * @brief       Set the Baseline Settings
 *
 * @param[in]   hDevice              Device handle obtained from #adi_CT_Init().
 * @param[in]   pBaselineSettings     Struct which includes the different Average Settings
 *
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     The Baseline Settings are set by pBaselineSettings.
 *              pBaselineSettings must be a #ADI_CT_BASELINE_TYPE struct which includes:
 *                      BaselineCalDelay            Delay before updating the Baseline. #ADI_CT_BASELINE_CAL_DELAY_TYPE
 *                      FastFilterUpdate            Update for the fast filter. #ADI_CT_FAST_FILTER_UPDATE_TYPE
 *                      FastProx                    Fast Proximity Value.  uint8_t
 *                      SlowProx                    Slow Proximity Value.  uint8_t
 *                      BL_Coeff                    Slow Filter new sample Weight. uint8_t
 *                      CDC_Coeff                   Fast Filter new sample Weight. uint8_t
 *
 */

ADI_CT_RESULT_TYPE adi_CT_SetBaselineCfg(ADI_CT_DEV_HANDLE const hDevice,ADI_CT_BASELINE_TYPE* const pBaselineSettings)

{



 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }


#endif


	uint32_t reg_value;

	uint32_t mask = BITM_CT_CT_BASELINE_CTRL_BASELINE_CAL_DELAY     |
			BITM_CT_CT_BASELINE_CTRL_FAST_FILTER_UPDATE     |
			BITM_CT_CT_BASELINE_CTRL_FAST_PROX     		|
			BITM_CT_CT_BASELINE_CTRL_SLOW_PROX    		|
			BITM_CT_CT_BASELINE_CTRL_BL_COEFF 		|
			BITM_CT_CT_BASELINE_CTRL_CDC_COEFF 		;


	uint32_t cfg_value = pBaselineSettings->BaselineCalDelay << BITP_CT_CT_BASELINE_CTRL_BASELINE_CAL_DELAY     |
			     pBaselineSettings->FastFilterUpdate << BITP_CT_CT_BASELINE_CTRL_FAST_FILTER_UPDATE     |
			     pBaselineSettings->FastProx         << BITP_CT_CT_BASELINE_CTRL_FAST_PROX     	    |
			     pBaselineSettings->SlowProx 	 << BITP_CT_CT_BASELINE_CTRL_SLOW_PROX     	    |
			     pBaselineSettings->BL_Coeff 	 << BITP_CT_CT_BASELINE_CTRL_BL_COEFF 		    |
			     pBaselineSettings->CDC_Coeff	 << BITP_CT_CT_BASELINE_CTRL_CDC_COEFF              ;


	/* protected critical section */
	ADI_ENTER_CRITICAL_REGION();
	{
		reg_value = hDevice->pCTRegs->CT_BASELINE_CTRL;
		reg_value &= ~(mask);
		reg_value |= cfg_value;
		hDevice->pCTRegs->CT_BASELINE_CTRL = reg_value;
	}
	ADI_EXIT_CRITICAL_REGION();

  return ADI_CT_RESULT_SUCCESS;

}


/*!
 * @brief       Get the Baseline Settings
 *
 * @param[in]   hDevice              Device handle obtained from #adi_CT_Init().
 *
 * @param[out]  BaselineSettings     Struct which includes the different Average Settings
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     The Baseline Settings are get by BaselineSettings.
 *              BaselineSettings will be get as a #ADI_CT_BASELINE_TYPE struct which includes:
 *                      BaselineCalDelay            Delay before updating the Baseline. #ADI_CT_BASELINE_CAL_DELAY_TYPE
 *                      FastFilterUpdate            Update for the fast filter. #ADI_CT_FAST_FILTER_UPDATE_TYPE
 *                      FastProx                    Fast Proximity Value.  uint8_t
 *                      SlowProx                    Slow Proximity Value.  uint8_t
 *                      BL_Coeff                    Slow Filter new sample Weight. uint8_t
 *                      CDC_Coeff                   Fast Filter new sample Weight. uint8_t
 *
 */

ADI_CT_RESULT_TYPE adi_CT_GetBaselineCfg(ADI_CT_DEV_HANDLE const hDevice,ADI_CT_BASELINE_TYPE *BaselineSettings)

{



 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

    uint32_t reg_value = hDevice->pCTRegs->CT_BASELINE_CTRL;



   BaselineSettings->BaselineCalDelay = (ADI_CT_BASELINE_CAL_DELAY_TYPE) ((reg_value & BITM_CT_CT_BASELINE_CTRL_BASELINE_CAL_DELAY) >> BITP_CT_CT_BASELINE_CTRL_BASELINE_CAL_DELAY);

   BaselineSettings->FastFilterUpdate = (ADI_CT_FAST_FILTER_UPDATE_TYPE) ((reg_value & BITM_CT_CT_BASELINE_CTRL_FAST_FILTER_UPDATE) >> BITP_CT_CT_BASELINE_CTRL_FAST_FILTER_UPDATE);

   BaselineSettings->FastProx         = (reg_value & BITM_CT_CT_BASELINE_CTRL_FAST_PROX) >> BITP_CT_CT_BASELINE_CTRL_FAST_PROX;

   BaselineSettings->SlowProx         = (reg_value & BITM_CT_CT_BASELINE_CTRL_SLOW_PROX) >> BITP_CT_CT_BASELINE_CTRL_SLOW_PROX;

   BaselineSettings->BL_Coeff         = (reg_value & BITM_CT_CT_BASELINE_CTRL_BL_COEFF)  >> BITP_CT_CT_BASELINE_CTRL_BL_COEFF;

   BaselineSettings->CDC_Coeff        = (reg_value & BITM_CT_CT_BASELINE_CTRL_CDC_COEFF) >> BITP_CT_CT_BASELINE_CTRL_CDC_COEFF;

  return ADI_CT_RESULT_SUCCESS;

}


 /*!
 * @brief       Set the Stage Configuration
 *
 * @param[in]   hDevice             Device handle obtained from #adi_CT_Init().
 * @param[in]   StageNumber         Stage to configure
 * @param[in]   CDCInput            Selected Input
 * @param[in]   pStageType           Stage Configuration
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     The Stage Configuration is set by pStageType.
 *              pStageType must be a #ADI_STAGE_TYPE struct.
 *              The CDC Input is set by CDCInput.
 *              CDCInput must be a #ADI_CT_INPUT_TYPE variable.
 *              It is recommended to use the #ADI_CT_STAGE_NUMBER_TYPE type to select
 *              the Stage to configure.
 *
 */

ADI_CT_RESULT_TYPE adi_CT_SetStageGenericCfg(ADI_CT_DEV_HANDLE const hDevice, ADI_CT_STAGE_NUMBER_TYPE const StageNumber, ADI_CT_INPUT_TYPE const CDCInput,ADI_STAGE_TYPE*  const pStageType)

{



 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif


  adi_CT_SetStageSelectInput(hDevice,StageNumber,CDCInput);
  adi_CT_SetStageByPassPGA(hDevice,StageNumber,pStageType->ByPassPGA);
  adi_CT_SetStagePGAGain(hDevice,StageNumber,pStageType->PGA_Gain);
  adi_CT_SetStageC2VRange(hDevice,StageNumber,pStageType->C2VRange);
  adi_CT_SetStagePK2PKEnable(hDevice,StageNumber,pStageType->PK2PKMeasurements);
  adi_CT_SetStageFreqHoppingEnable(hDevice,StageNumber,pStageType->DisableSignalExcitation);
  adi_CT_SetStageCAPDACEnable(hDevice,StageNumber,pStageType->MAIN_DAC_Enable);
  adi_CT_SetStageCAPDACValue(hDevice,StageNumber,pStageType->MAIN_DAC_Value,pStageType->SUB_DAC_Value);




  return ADI_CT_RESULT_SUCCESS;
}


 /*!
 * @brief       Get the Stage Configuration
 *
 * @param[in]   hDevice             Device handle obtained from #adi_CT_Init().
 * @param[in]   StageNumber         Stage to configure
 *
 * @param[out]   CDCInput            Selected Input
 * @param[out]   pStageType          Stage Configuration
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     The Stage Configuration is get by StageType.
 *              StageType must be a #ADI_STAGE_TYPE struct.
 *              The CDC Input is get by CDCInput.
 *              CDCInput must be a #ADI_CT_INPUT_TYPE variable
 *              It is recommended to use the #ADI_CT_STAGE_NUMBER_TYPE type to select
 *              the Stage to configure.
 *
 */

ADI_CT_RESULT_TYPE adi_CT_GetStageGenericCfg (ADI_CT_DEV_HANDLE const hDevice, ADI_CT_STAGE_NUMBER_TYPE const StageNumber, ADI_CT_INPUT_TYPE* CDCInput,ADI_STAGE_TYPE* const pStageType)

{


 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

    adi_CT_GetStageSelectInput(hDevice,StageNumber,CDCInput);
    adi_CT_GetStageByPassPGA(hDevice,StageNumber,&pStageType->ByPassPGA);
    adi_CT_GetStagePGAGain(hDevice,StageNumber,&pStageType->PGA_Gain);
    adi_CT_GetStageC2VRange(hDevice,StageNumber,&pStageType->C2VRange);
    adi_CT_GetStagePK2PKEnable(hDevice,StageNumber,&pStageType->PK2PKMeasurements);
    adi_CT_GetStageFreqHoppingEnable(hDevice,StageNumber,&pStageType->DisableSignalExcitation);
    adi_CT_GetStageCAPDACEnable(hDevice,StageNumber,&pStageType->MAIN_DAC_Enable);
    adi_CT_GetStageCAPDACValue(hDevice,StageNumber,&pStageType->MAIN_DAC_Value,&pStageType->SUB_DAC_Value);

  return ADI_CT_RESULT_SUCCESS;
}


/*!
 * @brief       Enable the Calibration for one or more specific Stages
 *
 * @param[in]   hDevice             Device handle obtained from #adi_CT_Init().
 * @param[in]   Stages              Stages to Enable the Calibration
 * @param[in]   enable              Status
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     To enable the Calibratrion of a specific Stage, the bit associated to that Stage must be 1.
 *              It is recommended to use the #ADI_CT_STAGE_BIT_TYPE type to select the Stages to enable/disable.
 *
 */

ADI_CT_RESULT_TYPE adi_CT_SetStagesCalibrationEnable(ADI_CT_DEV_HANDLE const hDevice,uint16_t const Stages,bool_t const enable){



 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

  if(enable)
  {
    hDevice->pCTRegs->CT_CAL_EN |= Stages;
  }

  else
  {
    hDevice->pCTRegs->CT_CAL_EN &= ~(Stages);
  }



  return ADI_CT_RESULT_SUCCESS;

}

/*!
 * @brief       Get the Calibration Status for one or more specific Stages
 *
 * @param[in]   hDevice             Device handle obtained from #adi_CT_Init().
 *
 * @param[out]  Stages              Stages to Enable the Calibration
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     If the associated bit to the Stage is 1 will indicate that this Stage is configured
 *              with the Calibration enable
 *
 */

ADI_CT_RESULT_TYPE adi_CT_GetStagesCalibrationEnable(ADI_CT_DEV_HANDLE const hDevice,uint16_t *Stages){



 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

  *Stages = hDevice->pCTRegs->CT_CAL_EN & BITM_CT_CT_CAL_EN_CAL_EN;


  return ADI_CT_RESULT_SUCCESS;

}




 /*!
 * @brief       Set the CDC Input
 *
 * @param[in]   hDevice             Device handle obtained from #adi_CT_Init().
 * @param[in]   StageNumber         Stage to configure
 * @param[in]   CDCInput            Selected Input
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     The CDC Input is set by CDCInput.
 *              CDCInput must be a #ADI_CT_INPUT_TYPE variable.
 *              It is recommended to use the #ADI_CT_STAGE_NUMBER_TYPE type to select
 *              the Stage to configure.
 *
 */


ADI_CT_RESULT_TYPE adi_CT_SetStageSelectInput(ADI_CT_DEV_HANDLE const hDevice, ADI_CT_STAGE_NUMBER_TYPE const StageNumber, ADI_CT_INPUT_TYPE const CDCInput)
{



 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

	uint32_t reg_value;

	volatile uint32_t *StageConfigurationPtr = NULL;

	StageConfigurationPtr = &(hDevice->pCTRegs->CT_STAGE0_CFG) + StageNumber;


	uint32_t mask = BITM_CT_CT_STAGE0_CFG_CIN_CON_POS_CDC_EN  |
			BITM_CT_CT_STAGE0_CFG_CIN_CON_POS_CDC     ;


	uint32_t cfg_value = 1        << BITP_CT_CT_STAGE0_CFG_CIN_CON_POS_CDC_EN       |
			     CDCInput << BITP_CT_CT_STAGE0_CFG_CIN_CON_POS_CDC          ;


	/* protected critical section */
	ADI_ENTER_CRITICAL_REGION();
	{
		reg_value = *StageConfigurationPtr;
		reg_value &= ~(mask);
		reg_value |= cfg_value;
		*StageConfigurationPtr = reg_value;
	}
	ADI_EXIT_CRITICAL_REGION();

  return ADI_CT_RESULT_SUCCESS;

}

 /*!
 * @brief       Get the CDC Input
 *
 * @param[in]   hDevice             Device handle obtained from #adi_CT_Init().
 * @param[in]   StageNumber         Stage to configure
 *
 * @param[out]  CDCInput            Selected Input
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     The CDC Input is get by CDCInput.
 *              CDCInput will be get as a #ADI_CT_INPUT_TYPE variable.
 *              It is recommended to use the #ADI_CT_STAGE_NUMBER_TYPE type to select
 *              the Stage to configure.
 *
 */

ADI_CT_RESULT_TYPE adi_CT_GetStageSelectInput(ADI_CT_DEV_HANDLE const hDevice, ADI_CT_STAGE_NUMBER_TYPE const StageNumber, ADI_CT_INPUT_TYPE* CDCInput)
{

 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

  volatile uint32_t *StageConfigurationPtr = NULL;

  StageConfigurationPtr = &(hDevice->pCTRegs->CT_STAGE0_CFG) + StageNumber;

  *CDCInput = (ADI_CT_INPUT_TYPE) ((*StageConfigurationPtr & BITM_CT_CT_STAGE0_CFG_CIN_CON_POS_CDC) >> BITP_CT_CT_STAGE0_CFG_CIN_CON_POS_CDC);

  return ADI_CT_RESULT_SUCCESS;

}

/*!
 * @brief       Bypass the PGA
 *
 * @param[in]   hDevice             Device handle obtained from #adi_CT_Init().
 * @param[in]   StageNumber         Stage to configure
 * @param[in]   ByPassPGA           BYPASS Gain Status
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     The function sets the PGA gain status for a specific stage.
 *                      True  -> Override
 *                      False -> Not Override
 *
 */

ADI_CT_RESULT_TYPE adi_CT_SetStageByPassPGA(ADI_CT_DEV_HANDLE const hDevice, ADI_CT_STAGE_NUMBER_TYPE const StageNumber,bool_t const ByPassPGA)
{

 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

  volatile uint32_t *StageConfigurationPtr = NULL;

  StageConfigurationPtr = &(hDevice->pCTRegs->CT_STAGE0_CFG) + StageNumber;

  if(ByPassPGA)
  {
    *StageConfigurationPtr |= BITM_CT_CT_STAGE0_CFG_PGA_BYPASS;
  }
  else
  {
    *StageConfigurationPtr &= ~(BITM_CT_CT_STAGE0_CFG_PGA_BYPASS);
  }

  return ADI_CT_RESULT_SUCCESS;

}

/*!
 * @brief       Get Bypass PGA Status
 *
 * @param[in]   hDevice             Device handle obtained from #adi_CT_Init().
 * @param[in]   StageNumber         Stage to configure
 *
 * @param[out]   Status             BYPASS Gain Status
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     The function gets the PGA gain status for a specific stage.
 *                      True  -> Override
 *                      False -> Not Override
 *
 */

ADI_CT_RESULT_TYPE adi_CT_GetStageByPassPGA(ADI_CT_DEV_HANDLE const hDevice, ADI_CT_STAGE_NUMBER_TYPE const StageNumber,bool_t *Status)
{



 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

  volatile uint32_t *StageConfigurationPtr = NULL;

	StageConfigurationPtr = &(hDevice->pCTRegs->CT_STAGE0_CFG) + StageNumber;

	*Status = (*StageConfigurationPtr & BITM_CT_CT_STAGE0_CFG_PGA_BYPASS) ? true : false;


  return ADI_CT_RESULT_SUCCESS;

}
 /*!
 * @brief       Set the PGA Gain
 *
 * @param[in]   hDevice             Device handle obtained from #adi_CT_Init().
 * @param[in]   StageNumber         Stage to configure
 * @param[in]   PGA_Gain            PGA Gain
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     The PGA Gain is set by PGA_Gain.
 *              PGA_Gain must be a #ADI_CT_PGA_GAIN_TYPE variable.
 *              It is recommended to use the #ADI_CT_STAGE_NUMBER_TYPE type to select
 *              the Stage to configure.
 *
 */

ADI_CT_RESULT_TYPE adi_CT_SetStagePGAGain(ADI_CT_DEV_HANDLE const hDevice, ADI_CT_STAGE_NUMBER_TYPE const StageNumber,ADI_CT_PGA_GAIN_TYPE PGA_Gain)
{


 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

  uint32_t reg_value;

  volatile uint32_t *StageConfigurationPtr = NULL;

  StageConfigurationPtr = &(hDevice->pCTRegs->CT_STAGE0_CFG) + StageNumber;

  /* protected critical section */
  ADI_ENTER_CRITICAL_REGION();
  {
	  reg_value = *StageConfigurationPtr;
	  reg_value &= ~(BITM_CT_CT_STAGE0_CFG_PGA_GAIN);
	  reg_value |= PGA_Gain << BITP_CT_CT_STAGE0_CFG_PGA_GAIN;
	  *StageConfigurationPtr = reg_value;
  }
  ADI_EXIT_CRITICAL_REGION();

  return ADI_CT_RESULT_SUCCESS;

}

 /*!
 * @brief       Get the PGA Gain
 *
 * @param[in]   hDevice             Device handle obtained from #adi_CT_Init().
 * @param[in]   StageNumber         Stage to configure
 *
 * @param[out]  PGA_Gain            PGA Gain
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     The PGA Gain is get by PGA_Gain.
 *              PGA_Gain will be get as a #ADI_CT_PGA_GAIN_TYPE variable.
 *              It is recommended to use the #ADI_CT_STAGE_NUMBER_TYPE type to select
 *              the Stage to configure.
 *
 */

ADI_CT_RESULT_TYPE adi_CT_GetStagePGAGain(ADI_CT_DEV_HANDLE const hDevice, ADI_CT_STAGE_NUMBER_TYPE const StageNumber,ADI_CT_PGA_GAIN_TYPE *PGA_Gain)
{

 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

  volatile uint32_t *StageConfigurationPtr = NULL;

  StageConfigurationPtr = &(hDevice->pCTRegs->CT_STAGE0_CFG) + StageNumber;

  *PGA_Gain = (ADI_CT_PGA_GAIN_TYPE) ((*StageConfigurationPtr & BITM_CT_CT_STAGE0_CFG_PGA_GAIN) >> BITP_CT_CT_STAGE0_CFG_PGA_GAIN);

  return ADI_CT_RESULT_SUCCESS;

}

 /*!
 * @brief       Set the C2V Range
 *
 * @param[in]   hDevice             Device handle obtained from #adi_CT_Init().
 * @param[in]   StageNumber         Stage to configure
 * @param[in]   C2VRange            C2V Range
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     The C2V Range is set by C2VRange.
 *              C2VRange must be a #ADI_CT_C2VRANGE_TYPE variable.
 *              It is recommended to use the #ADI_CT_STAGE_NUMBER_TYPE type to select
 *              the Stage to configure.
 *
 */

ADI_CT_RESULT_TYPE adi_CT_SetStageC2VRange(ADI_CT_DEV_HANDLE const hDevice, ADI_CT_STAGE_NUMBER_TYPE const StageNumber, ADI_CT_C2VRANGE_TYPE const C2VRange)
{

 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

  volatile uint32_t *StageConfigurationPtr = NULL;

  uint32_t reg_value;

  StageConfigurationPtr = &(hDevice->pCTRegs->CT_STAGE0_CFG) + StageNumber;

  /* protected critical section */
  ADI_ENTER_CRITICAL_REGION();
  {
	  reg_value = *StageConfigurationPtr;
	  reg_value &= ~(BITM_CT_CT_STAGE0_CFG_C2V_IP_RANGE);
	  reg_value |= C2VRange << BITP_CT_CT_STAGE0_CFG_C2V_IP_RANGE;
	  *StageConfigurationPtr = reg_value;
  }
  ADI_EXIT_CRITICAL_REGION();


  return ADI_CT_RESULT_SUCCESS;

}

 /*!
 * @brief       Get the C2V Range
 *
 * @param[in]   hDevice             Device handle obtained from #adi_CT_Init().
 * @param[in]   StageNumber         Stage to configure
 *
 * @param[out]   C2VRange            C2V Range
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     The C2V Range is get by C2VRange.
 *              C2VRange will be get as a #ADI_CT_C2VRANGE_TYPE variable.
 *              It is recommended to use the #ADI_CT_STAGE_NUMBER_TYPE type to select
 *              the Stage to configure.
 *
 */

ADI_CT_RESULT_TYPE adi_CT_GetStageC2VRange(ADI_CT_DEV_HANDLE const hDevice, ADI_CT_STAGE_NUMBER_TYPE const StageNumber, ADI_CT_C2VRANGE_TYPE *C2VRange)
{

 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

  volatile uint32_t *StageConfigurationPtr = NULL;

  StageConfigurationPtr = &(hDevice->pCTRegs->CT_STAGE0_CFG) + StageNumber;

  *C2VRange = (ADI_CT_C2VRANGE_TYPE) ((*StageConfigurationPtr & BITM_CT_CT_STAGE0_CFG_C2V_IP_RANGE) >> BITP_CT_CT_STAGE0_CFG_C2V_IP_RANGE);

  return ADI_CT_RESULT_SUCCESS;

}

 /*!
 * @brief       Enables the Noise Peak to Peak Measurements
 *
 * @param[in]   hDevice               Device handle obtained from #adi_CT_Init().
 * @param[in]   StageNumber           Stage to configure
 * @param[in]   Enable                Peak to Peak Measurements Enable
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     The function sets the Peak to Peak Measurements status for a specific stage.
 *                      True  -> Enable
 *                      False -> Disable
 *              It is recommended to use the #ADI_CT_STAGE_NUMBER_TYPE type to select
 *              the Stage to configure.
 *
 */

ADI_CT_RESULT_TYPE adi_CT_SetStagePK2PKEnable(ADI_CT_DEV_HANDLE const hDevice, ADI_CT_STAGE_NUMBER_TYPE const StageNumber, bool_t const Enable)
{


 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

  volatile uint32_t *StageConfigurationPtr = NULL;

  StageConfigurationPtr = &(hDevice->pCTRegs->CT_STAGE0_CFG) + StageNumber;


  if(Enable)
  {
    *StageConfigurationPtr |= BITM_CT_CT_STAGE0_CFG_PK2PK;
  }
  else
  {
    *StageConfigurationPtr &= ~(BITM_CT_CT_STAGE0_CFG_PK2PK);
  }


  return ADI_CT_RESULT_SUCCESS;

}

 /*!
 * @brief       Get the Noise Peak to Peak Measurements status
 *
 * @param[in]   hDevice               Device handle obtained from #adi_CT_Init().
 * @param[in]   StageNumber           Stage to configure
 *
 * @param[out]  Status                Peak to Peak Measurements Status
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     The function gets the Peak to Peak Measurements status for a specific stage.
 *                      True  -> Enable
 *                      False -> Disable
 *              It is recommended to use the #ADI_CT_STAGE_NUMBER_TYPE type to select
 *              the Stage to configure.
 *
 */

ADI_CT_RESULT_TYPE adi_CT_GetStagePK2PKEnable(ADI_CT_DEV_HANDLE const hDevice, ADI_CT_STAGE_NUMBER_TYPE const StageNumber, bool_t *Status)
{



 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

  volatile uint32_t *StageConfigurationPtr = NULL;

  StageConfigurationPtr = &(hDevice->pCTRegs->CT_STAGE0_CFG) + StageNumber;

  *Status = (*StageConfigurationPtr & BITM_CT_CT_STAGE0_CFG_PK2PK) ? true : false;

  return ADI_CT_RESULT_SUCCESS;

}

 /*!
 * @brief       Set the system ready to start a Frequency Hopping Algorithm
 *
 * @param[in]   hDevice               Device handle obtained from #adi_CT_Init().
 * @param[in]   StageNumber           Stage to configure
 * @param[in]   Enable                Frequency Hopping Algorithm Condition (disable the excitation)
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details             True  -> Disable the signal excitation
 *                      False -> Enable the signal excitation (normal operation)
 *              It is recommended to use the #ADI_CT_STAGE_NUMBER_TYPE type to select
 *              the Stage to configure.
 *
 */

ADI_CT_RESULT_TYPE adi_CT_SetStageFreqHoppingEnable(ADI_CT_DEV_HANDLE const hDevice, ADI_CT_STAGE_NUMBER_TYPE const StageNumber,bool_t const Enable)
{


 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

  volatile uint32_t *StageConfigurationPtr = NULL;

  StageConfigurationPtr = &(hDevice->pCTRegs->CT_STAGE0_CFG) + StageNumber;


  if(Enable)
  {
    *StageConfigurationPtr |= BITM_CT_CT_STAGE0_CFG_NOISE_REDUCTION_EN;
  }
  else
  {
    *StageConfigurationPtr &= ~(BITM_CT_CT_STAGE0_CFG_NOISE_REDUCTION_EN);

  }


  return ADI_CT_RESULT_SUCCESS;

}

 /*!
 * @brief       Get the signal excitation status
 *
 * @param[in]   hDevice               Device handle obtained from #adi_CT_Init().
 * @param[in]   StageNumber           Stage to configure
 *
 * @param[out]  Status                Fsignal excitation status
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                            Error.
 *
 * @details             True->Disable the signal excitation
 *                      False->Enable the signal excitation (normal operation)
 *              It is recommended to use the #ADI_CT_STAGE_NUMBER_TYPE type to select
 *              the Stage to configure.
 *
 */

ADI_CT_RESULT_TYPE adi_CT_GetStageFreqHoppingEnable(ADI_CT_DEV_HANDLE const hDevice, ADI_CT_STAGE_NUMBER_TYPE const StageNumber, bool_t *Status)
{

 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

  volatile uint32_t *StageConfigurationPtr = NULL;

  StageConfigurationPtr = &(hDevice->pCTRegs->CT_STAGE0_CFG) + StageNumber;

  *Status = (*StageConfigurationPtr & BITM_CT_CT_STAGE0_CFG_NOISE_REDUCTION_EN) ? true : false;

  return ADI_CT_RESULT_SUCCESS;

}

 /*!
 * @brief       Enable the CAPDAC for each Stage
 *
 * @param[in]   hDevice                  Device handle obtained from #adi_CT_Init().
 * @param[in]   StageNumber              Stage to configure
 * @param[in]   Enable                   Enable the CAPDAC
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     The function sets CAPDAC status for a specific stage.
 *                      True  -> Enable
 *                      False -> Disable
 *              It is recommended to use the #ADI_CT_STAGE_NUMBER_TYPE type to select
 *              the Stage to configure.
 *
 */

ADI_CT_RESULT_TYPE adi_CT_SetStageCAPDACEnable(ADI_CT_DEV_HANDLE const hDevice, ADI_CT_STAGE_NUMBER_TYPE const StageNumber,bool_t const Enable)
{


 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

  volatile uint32_t *StageConfigurationPtr = NULL;

  StageConfigurationPtr = &(hDevice->pCTRegs->CT_STAGE0_CFG) + StageNumber;

  if(Enable)
  {
    *StageConfigurationPtr |= BITM_CT_CT_STAGE0_CFG_EN_MAIN_DAC;
  }
  else
  {
    *StageConfigurationPtr &= ~(BITM_CT_CT_STAGE0_CFG_EN_MAIN_DAC);
  }

  return ADI_CT_RESULT_SUCCESS;

}

 /*!
 * @brief       Get the CAPDAC Status for each Stage
 *
 * @param[in]   hDevice                  Device handle obtained from #adi_CT_Init().
 * @param[in]   StageNumber              Stage to configure
 *
 * @param[out]   Status                   Enable the CAPDAC
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     The function sets CAPDAC status for a specific stage.
 *                      True->Enable
 *                      False->Disable
 *              It is recommended to use the #ADI_CT_STAGE_NUMBER_TYPE type to select
 *              the Stage to configure.
 *
 */

ADI_CT_RESULT_TYPE adi_CT_GetStageCAPDACEnable(ADI_CT_DEV_HANDLE const hDevice, ADI_CT_STAGE_NUMBER_TYPE const StageNumber,bool_t *Status)
{

 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

  volatile uint32_t *StageConfigurationPtr = NULL;

  StageConfigurationPtr = &hDevice->pCTRegs->CT_STAGE0_CFG + StageNumber;

  *Status = (*StageConfigurationPtr & BITM_CT_CT_STAGE0_CFG_EN_MAIN_DAC) ? true : false;

  return ADI_CT_RESULT_SUCCESS;

}

 /*!
 * @brief       Set the CAPDAC values for each Stage
 *
 * @param[in]   hDevice                  Device handle obtained from #adi_CT_Init().
 * @param[in]   StageNumber              Stage to configure
 * @param[in]   MAIN_DAC_Value           MAIN DAC Value (7 bits)
 * @param[in]   SUB_DAC_Value            SUB DAC Value  (3 bits)
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     The MAIN_DAC_Value is a 7 bits value and the SUB_DAC_Value is a 3 bits.
 *              It is recommended to use the #ADI_CT_STAGE_NUMBER_TYPE type to select
 *              the Stage to configure.
 *
 */

ADI_CT_RESULT_TYPE adi_CT_SetStageCAPDACValue(ADI_CT_DEV_HANDLE const hDevice, ADI_CT_STAGE_NUMBER_TYPE const StageNumber, uint8_t const MAIN_DAC_Value,uint8_t const SUB_DAC_Value)
{


 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }


#endif

  uint32_t reg_value;

  volatile uint32_t *StageConfigurationPtr = NULL;

  StageConfigurationPtr = &(hDevice->pCTRegs->CT_STAGE0_CFG) + StageNumber;


  uint32_t mask = BITM_CT_CT_STAGE0_CFG_MAIN_DAC  |
		  BITM_CT_CT_STAGE0_CFG_SUB_DAC   ;


  uint32_t cfg_value = MAIN_DAC_Value << BITP_CT_CT_STAGE0_CFG_MAIN_DAC   |
		       SUB_DAC_Value  << BITP_CT_CT_STAGE0_CFG_SUB_DAC   ;


  /* protected critical section */
  ADI_ENTER_CRITICAL_REGION();
  {
	  reg_value = *StageConfigurationPtr;
	  reg_value &= ~(mask);
	  reg_value |= cfg_value;
	  *StageConfigurationPtr = reg_value;
  }
  ADI_EXIT_CRITICAL_REGION();

  return ADI_CT_RESULT_SUCCESS;

}

 /*!
 * @brief       Get the CAPDAC values for each Stage
 *
 * @param[in]   hDevice                  Device handle obtained from #adi_CT_Init().
 * @param[in]   StageNumber              Stage to configure
 *
 * @param[out]   MAIN_DAC_Value           MAIN DAC Value (7 bits)
 * @param[out]   SUB_DAC_Value            SUB DAC Value  (3 bits)
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     The MAIN_DAC_Value is a 7 bits value and the SUB_DAC_Value is a 3 bits.
 *              It is recommended to use the #ADI_CT_STAGE_NUMBER_TYPE type to select
 *              the Stage to configure.
 *
 */

ADI_CT_RESULT_TYPE adi_CT_GetStageCAPDACValue(ADI_CT_DEV_HANDLE const hDevice, ADI_CT_STAGE_NUMBER_TYPE const StageNumber, uint8_t *MAIN_DAC_Value,uint8_t *SUB_DAC_Value)
{


 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

  volatile uint32_t *StageConfigurationPtr = NULL;

  StageConfigurationPtr = &(hDevice->pCTRegs->CT_STAGE0_CFG) + StageNumber;

   *MAIN_DAC_Value = (*StageConfigurationPtr & BITM_CT_CT_STAGE0_CFG_MAIN_DAC) >> BITP_CT_CT_STAGE0_CFG_MAIN_DAC;
   *SUB_DAC_Value  = (*StageConfigurationPtr & BITM_CT_CT_STAGE0_CFG_SUB_DAC)  >> BITP_CT_CT_STAGE0_CFG_SUB_DAC;

  return ADI_CT_RESULT_SUCCESS;

}

/*!
 * @brief       Set Enable CT Stages
 *
 * @param[in]   hDevice             Device handle obtained from #adi_CT_Init().
 * @param[in]   Stages              Stages to Enable
 * @param[in]   enable              Status
 *
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     To enable a specific Stage, the bit associated to that Stage must be 1.
 *              It is recommended to use the #ADI_CT_STAGE_BIT_TYPE type.
 *
 */

ADI_CT_RESULT_TYPE adi_CT_SetStagesEnable(ADI_CT_DEV_HANDLE const hDevice,uint16_t const Stages, bool_t const enable){



 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

  if(enable)
  {
    hDevice->pCTRegs->CT_MEAS_SEL |= (Stages & BITM_CT_CT_MEAS_SEL_GND_SEL);
  }
  else
  {
    hDevice->pCTRegs->CT_MEAS_SEL &= ~(Stages & BITM_CT_CT_MEAS_SEL_GND_SEL);
  }


  return ADI_CT_RESULT_SUCCESS;

}

/*!
 * @brief       Get Enable CT Stages
 *
 * @param[in]   hDevice             Device handle obtained from adi_CT_Init().
 *
 * @param[out]  Stages              Stages to Enable
 *
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     If the associated bit to the Stage is 1 will indicate that this Stage is configured
 *              with the Calibration enable
 *
 */

ADI_CT_RESULT_TYPE adi_CT_GetStagesEnable(ADI_CT_DEV_HANDLE const hDevice,uint16_t *Stages){



 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

   *Stages = hDevice->pCTRegs->CT_MEAS_SEL & BITM_CT_CT_MEAS_SEL_GND_SEL;

  return ADI_CT_RESULT_SUCCESS;

}



/*!
 * @brief       Set the Sensor Threshold
 *
 * @param[in]   hDevice             Device handle obtained from #adi_CT_Init().
 * @param[in]   StageNumber         Stage to Configure
 * @param[in]   SensorThreshold     Digital Self Timer Value
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     SensorThreshold is a 14 bit value.
 *              It is recommended to use the #ADI_CT_STAGE_BIT_TYPE type.
 *
 */

ADI_CT_RESULT_TYPE adi_CT_SetStageSensorThreshold(ADI_CT_DEV_HANDLE const hDevice,ADI_CT_STAGE_NUMBER_TYPE const StageNumber,uint16_t const SensorThreshold)
{



 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

    volatile uint32_t *SensorThresholdPtr = NULL;

	uint32_t mask;
	uint32_t cfg_value;
	uint32_t reg_value;

	SensorThresholdPtr = &hDevice->pCTRegs->CT_SENSOR_THR_CFG0 + (StageNumber>>1);

	if(StageNumber & ADI_CT_ODD_STAGE)
	{
		mask = BITM_CT_CT_SENSOR_THR_CFG0_SENSOR_THRESHOLD1,
		cfg_value = SensorThreshold << BITP_CT_CT_SENSOR_THR_CFG0_SENSOR_THRESHOLD1;
	}
	else
	{
		mask = BITM_CT_CT_SENSOR_THR_CFG0_SENSOR_THRESHOLD0,
		cfg_value = SensorThreshold << BITP_CT_CT_SENSOR_THR_CFG0_SENSOR_THRESHOLD0;
	}


	/* protected critical section */
	ADI_ENTER_CRITICAL_REGION();
	{
		reg_value = *SensorThresholdPtr;
		reg_value &= ~(mask);
		reg_value |= cfg_value;
		*SensorThresholdPtr = reg_value;
	}
	ADI_EXIT_CRITICAL_REGION();

  return ADI_CT_RESULT_SUCCESS;

}


/*!
 * @brief       Get the Sensor Threshold
 *
 * @param[in]   hDevice             Device handle obtained from #adi_CT_Init().
 * @param[in]   StageNumber         Stage to Configure
 *
 * @param[out]  SensorThreshold     Digital Self Timer Value
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     SensorThreshold is a 14 bit value.
 *              It is recommended to use the #ADI_CT_STAGE_BIT_TYPE type.
 *
 */

ADI_CT_RESULT_TYPE adi_CT_GetStageSensorThreshold(ADI_CT_DEV_HANDLE const hDevice,ADI_CT_STAGE_NUMBER_TYPE const StageNumber,uint16_t *SensorThreshold)
{

 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

  volatile uint32_t *SensorThresholdPtr = NULL;

  SensorThresholdPtr = &(hDevice->pCTRegs->CT_SENSOR_THR_CFG0) + (StageNumber>>1);

  if(StageNumber & ADI_CT_ODD_STAGE)
  {
    *SensorThreshold = (*SensorThresholdPtr & BITM_CT_CT_SENSOR_THR_CFG0_SENSOR_THRESHOLD1) >> BITP_CT_CT_SENSOR_THR_CFG0_SENSOR_THRESHOLD1;
  }

  else
  {
    *SensorThreshold = (*SensorThresholdPtr & BITM_CT_CT_SENSOR_THR_CFG0_SENSOR_THRESHOLD0) >> BITP_CT_CT_SENSOR_THR_CFG0_SENSOR_THRESHOLD0;
  }

  return ADI_CT_RESULT_SUCCESS;

}



/*!
 * @brief       Enable the Touch Detection
 *
 * @param[in]   hDevice             Device handle obtained from #adi_CT_Init().
 * @param[in]   Enable              Status of Touch Detection
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     The function enables the Touch Detection
 *                      True  -> Enable
 *                      False -> Disable
 *
 */

ADI_CT_RESULT_TYPE adi_CT_SetTouchEnable(ADI_CT_DEV_HANDLE const hDevice,bool_t const Enable)

{


 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

  if(Enable)
  {
    hDevice->pCTRegs->CT_TOUCH_CFG2 |= BITM_CT_CT_TOUCH_CFG2_TCH_DETECT_ENABLE;
  }
  else
  {
    hDevice->pCTRegs->CT_TOUCH_CFG2 &= ~(BITM_CT_CT_TOUCH_CFG2_TCH_DETECT_ENABLE);
  }


  return ADI_CT_RESULT_SUCCESS;

}
/*!
 * @brief       Get the Touch Detection Status
 *
 * @param[in]   hDevice             Device handle obtained from #adi_CT_Init().
 *
 * @param[out]  Status              Status of Touch Detection
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     The function gets the Touch Detection Enable Status
 *                      True  -> Enable
 *                      False -> Disable
 *
 */

ADI_CT_RESULT_TYPE adi_CT_GetTouchEnable(ADI_CT_DEV_HANDLE const hDevice,bool_t *Status)

{


 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

	*Status = (hDevice->pCTRegs->CT_TOUCH_CFG2 & BITM_CT_CT_TOUCH_CFG2_TCH_DETECT_ENABLE) ? true : false;

  return ADI_CT_RESULT_SUCCESS;

}

/*!
 * @brief       Enable the Release Detection
 *
 * @param[in]   hDevice             Device handle obtained from #adi_CT_Init().
 * @param[in]   Enable              Status of Release Detection
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     The function enables the Release Detection
 *                      True  -> Enable
 *                      False -> Disable
 *
 */

ADI_CT_RESULT_TYPE adi_CT_SetReleaseEnable(ADI_CT_DEV_HANDLE const hDevice,bool_t const Enable)

{

 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

  if(Enable)
  {
    hDevice->pCTRegs->CT_RELEASE_CFG2 |= BITM_CT_CT_RELEASE_CFG2_REL_DETECT_ENABLE;
  }
  else
  {
    hDevice->pCTRegs->CT_RELEASE_CFG2 &= ~(BITM_CT_CT_RELEASE_CFG2_REL_DETECT_ENABLE);
  }

  return ADI_CT_RESULT_SUCCESS;

}

/*!
 * @brief       Get the Release Detection Status
 *
 * @param[in]   hDevice             Device handle obtained from adi_CT_Init().
 *
 * @param[out]  Status              Status of Release Detection
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     The function gets the Release Detection Enable Status
 *                      True  -> Enable
 *                      False -> Disable
 *
 */

ADI_CT_RESULT_TYPE adi_CT_GetReleaseEnable(ADI_CT_DEV_HANDLE const hDevice,bool_t *Status)

{

 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

	*Status = (hDevice->pCTRegs->CT_RELEASE_CFG2 & BITM_CT_CT_RELEASE_CFG2_REL_DETECT_ENABLE) ? true : false;

  return ADI_CT_RESULT_SUCCESS;

}



/*!
 * @brief      Set the Touch Detection Settings
 *
 * @param[in]   hDevice             Device handle obtained from #adi_CT_Init().
 * @param[in]   pTouchSettings       Struct which includes the different Touch Settings
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     The Touch Settings are set by pTouchSettings.
 *              pTouchSettings must be a #ADI_CT_TOUCH_AND_RELEASE_TYPE struct which includes:
 *                      InputA              Source A
 *                      InputB              Source B
 *                      UpperThreshold      Upper Thershold
 *                      LowerThreshold      Lower Thershold
 *                      MinDuration         Minimum touch Duration
 *
 */


ADI_CT_RESULT_TYPE adi_CT_SetTouchCfg(ADI_CT_DEV_HANDLE const hDevice,ADI_CT_TOUCH_AND_RELEASE_TYPE* const pTouchSettings)

{

 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif


	uint32_t reg_value;

	uint32_t mask = BITM_CT_CT_TOUCH_CFG2_INDIVIDUAL_THRESHOLD_EN  |
			BITM_CT_CT_TOUCH_CFG2_TCH_EVENT_ON_HIGH        |
			BITM_CT_CT_TOUCH_CFG2_TCH_SEL_INPA	       |
			BITM_CT_CT_TOUCH_CFG2_TCH_SEL_INPB	       |
			BITM_CT_CT_TOUCH_CFG2_TCH_MIN_DURATION	;

	uint32_t cfg_value = pTouchSettings->IndividualThresholdEnable << BITP_CT_CT_TOUCH_CFG2_INDIVIDUAL_THRESHOLD_EN   |
			     pTouchSettings->EventOnHIgh 	       << BITP_CT_CT_TOUCH_CFG2_TCH_EVENT_ON_HIGH         |
			     pTouchSettings->InputA		       << BITP_CT_CT_TOUCH_CFG2_TCH_SEL_INPA  	          |
			     pTouchSettings->InputB 		       << BITP_CT_CT_TOUCH_CFG2_TCH_SEL_INPB              |
			     pTouchSettings->MinDuration 	       << BITP_CT_CT_TOUCH_CFG2_TCH_MIN_DURATION	  ;

	/* protected critical section */
	ADI_ENTER_CRITICAL_REGION();
	{
		reg_value = (pTouchSettings->UpperThreshold << BITP_CT_CT_TOUCH_CFG1_TCH_UPPER_THLD) | (pTouchSettings->LowerThreshold << BITP_CT_CT_TOUCH_CFG1_TCH_LOWER_THLD);
		hDevice->pCTRegs->CT_TOUCH_CFG1 = reg_value;


		reg_value = hDevice->pCTRegs->CT_TOUCH_CFG2;
		reg_value &= ~(mask);
		reg_value |= cfg_value;
		hDevice->pCTRegs->CT_TOUCH_CFG2 = reg_value;
	}
	ADI_EXIT_CRITICAL_REGION();

  return ADI_CT_RESULT_SUCCESS;


}


/*!
 * @brief       Get the Touch Detection Settings
 *
 * @param[in]   hDevice             Device handle obtained from #adi_CT_Init().
 *
 * @param[out]  TouchSettings       Struct which includes the different Touch Settings
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     The Touch Settings are get by TouchSettings.
 *              TouchSettings will be get as a #ADI_CT_TOUCH_AND_RELEASE_TYPE struct which includes:
 *                      InputA              Source A
 *                      InputB              Source B
 *                      UpperThreshold      Upper Thershold
 *                      LowerThreshold      Lower Thershold
 *                      MinDuration         Minimum touch Duration
 *
 */

ADI_CT_RESULT_TYPE adi_CT_GetTouchCfg(ADI_CT_DEV_HANDLE const hDevice,ADI_CT_TOUCH_AND_RELEASE_TYPE *TouchSettings)

{

 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

  uint32_t ct_touch_cfg1_value = hDevice->pCTRegs->CT_TOUCH_CFG1;
  uint32_t ct_touch_cfg2_value = hDevice->pCTRegs->CT_TOUCH_CFG2;

  TouchSettings->UpperThreshold            = (ct_touch_cfg1_value & BITM_CT_CT_TOUCH_CFG1_TCH_UPPER_THLD) >> BITP_CT_CT_TOUCH_CFG1_TCH_UPPER_THLD;

  TouchSettings->LowerThreshold            = (ct_touch_cfg1_value & BITM_CT_CT_TOUCH_CFG1_TCH_LOWER_THLD) >> BITP_CT_CT_TOUCH_CFG1_TCH_LOWER_THLD;

  TouchSettings->IndividualThresholdEnable = (ADI_CT_INDIVIDUAL_THR_ENABLE_TYPE)((ct_touch_cfg2_value & BITM_CT_CT_TOUCH_CFG2_INDIVIDUAL_THRESHOLD_EN) >> BITP_CT_CT_TOUCH_CFG2_INDIVIDUAL_THRESHOLD_EN);

  TouchSettings->EventOnHIgh               = (ADI_CT_TCH_REL_EVENT_ON_HIGH_INPUT_TYPE)((ct_touch_cfg2_value & BITM_CT_CT_TOUCH_CFG2_TCH_EVENT_ON_HIGH) >> BITP_CT_CT_TOUCH_CFG2_TCH_EVENT_ON_HIGH);

  TouchSettings->InputA                    = (ADI_CT_TCH_REL_INPUT_TYPE)((ct_touch_cfg2_value & BITM_CT_CT_TOUCH_CFG2_TCH_SEL_INPA) >> BITP_CT_CT_TOUCH_CFG2_TCH_SEL_INPA);

  TouchSettings->InputB                    = (ADI_CT_TCH_REL_INPUT_TYPE)((ct_touch_cfg2_value & BITM_CT_CT_TOUCH_CFG2_TCH_SEL_INPB) >> BITP_CT_CT_TOUCH_CFG2_TCH_SEL_INPB);

  TouchSettings->MinDuration               = (ct_touch_cfg2_value & BITM_CT_CT_TOUCH_CFG2_TCH_MIN_DURATION) >> BITP_CT_CT_TOUCH_CFG2_TCH_MIN_DURATION;

  return ADI_CT_RESULT_SUCCESS;


}

/*!
 * @brief      Set the Release Detection Settings
 *
 * @param[in]   hDevice             Device handle obtained from #adi_CT_Init().
 * @param[in]   pReleaseSettings     Struct which includes the different Release Settings
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     The Release Settings are set by pReleaseSettings.
 *              pReleaseSettings must be a #ADI_CT_TOUCH_AND_RELEASE_TYPE struct which includes:
 *                      InputA              Source A
 *                      InputB              Source B
 *                      UpperThreshold      Upper Thershold
 *                      LowerThreshold      Lower Thershold
 *                      MinDuration         Minimum touch Duration
 *
 */

ADI_CT_RESULT_TYPE adi_CT_SetReleaseCfg(ADI_CT_DEV_HANDLE const hDevice,ADI_CT_TOUCH_AND_RELEASE_TYPE* const pReleaseSettings)

{


 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }



#endif


        uint32_t reg_value;


	uint32_t mask = BITM_CT_CT_RELEASE_CFG2_INDIVIDUAL_THRESHOLD_EN  |
			BITM_CT_CT_RELEASE_CFG2_REL_EVENT_ON_HIGH        |
			BITM_CT_CT_RELEASE_CFG2_REL_SEL_INPA	       |
			BITM_CT_CT_RELEASE_CFG2_REL_SEL_INPB	       |
			BITM_CT_CT_RELEASE_CFG2_REL_MIN_DURATION	;

	uint32_t cfg_value = pReleaseSettings->IndividualThresholdEnable << BITP_CT_CT_RELEASE_CFG2_INDIVIDUAL_THRESHOLD_EN   |
			     pReleaseSettings->EventOnHIgh 	         << BITP_CT_CT_RELEASE_CFG2_REL_EVENT_ON_HIGH         |
			     pReleaseSettings->InputA		         << BITP_CT_CT_RELEASE_CFG2_REL_SEL_INPA  	      |
			     pReleaseSettings->InputB 		         << BITP_CT_CT_RELEASE_CFG2_REL_SEL_INPB              |
			     pReleaseSettings->MinDuration 	         << BITP_CT_CT_RELEASE_CFG2_REL_MIN_DURATION	  ;

	/* protected critical section */
	ADI_ENTER_CRITICAL_REGION();
	{
		reg_value = (pReleaseSettings->UpperThreshold << BITP_CT_CT_RELEASE_CFG1_REL_UPPER_THLD) | (pReleaseSettings->LowerThreshold << BITP_CT_CT_RELEASE_CFG1_REL_LOWER_THLD);
		hDevice->pCTRegs->CT_RELEASE_CFG1 = reg_value;

		reg_value = hDevice->pCTRegs->CT_RELEASE_CFG2;
		reg_value &= ~(mask);
		reg_value |= cfg_value;
		hDevice->pCTRegs->CT_RELEASE_CFG2 = reg_value;
	}
	ADI_EXIT_CRITICAL_REGION();

  return ADI_CT_RESULT_SUCCESS;

}


/*!
 * @brief       Get the Release Detection Settings
 *
 * @param[in]   hDevice             Device handle obtained from #adi_CT_Init().
 *
 * @param[out]  ReleaseSettings       Struct which includes the different Release Settings
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     The Release Settings are get by ReleaseSettings.
 *              ReleaseSettings will be get as a #ADI_CT_TCH_REL_INPUT_TYPE struct which includes:
 *                      InputA              Source A
 *                      InputB              Source B
 *                      UpperThreshold      Upper Thershold
 *                      LowerThreshold      Lower Thershold
 *                      MinDuration         Minimum touch Duration
 *
 */

ADI_CT_RESULT_TYPE adi_CT_GetReleaseCfg(ADI_CT_DEV_HANDLE const hDevice,ADI_CT_TOUCH_AND_RELEASE_TYPE *ReleaseSettings)

{



 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

  uint32_t ct_release_cfg1_value = hDevice->pCTRegs->CT_RELEASE_CFG1;
  uint32_t ct_release_cfg2_value = hDevice->pCTRegs->CT_RELEASE_CFG2;


  ReleaseSettings->UpperThreshold = (ct_release_cfg1_value & BITM_CT_CT_TOUCH_CFG1_TCH_UPPER_THLD) >> BITP_CT_CT_TOUCH_CFG1_TCH_UPPER_THLD;

  ReleaseSettings->LowerThreshold = (ct_release_cfg1_value & BITM_CT_CT_TOUCH_CFG1_TCH_LOWER_THLD) >> BITP_CT_CT_TOUCH_CFG1_TCH_LOWER_THLD;

  ReleaseSettings->IndividualThresholdEnable = (ADI_CT_INDIVIDUAL_THR_ENABLE_TYPE)((ct_release_cfg2_value & BITM_CT_CT_TOUCH_CFG2_INDIVIDUAL_THRESHOLD_EN) >> BITP_CT_CT_TOUCH_CFG2_INDIVIDUAL_THRESHOLD_EN);

  ReleaseSettings->EventOnHIgh = (ADI_CT_TCH_REL_EVENT_ON_HIGH_INPUT_TYPE) ((ct_release_cfg2_value &  BITM_CT_CT_TOUCH_CFG2_TCH_EVENT_ON_HIGH) >> BITP_CT_CT_TOUCH_CFG2_TCH_EVENT_ON_HIGH);

  ReleaseSettings->InputA = (ADI_CT_TCH_REL_INPUT_TYPE) ((ct_release_cfg2_value & BITM_CT_CT_TOUCH_CFG2_TCH_SEL_INPA) >> BITP_CT_CT_TOUCH_CFG2_TCH_SEL_INPA);

  ReleaseSettings->InputB = (ADI_CT_TCH_REL_INPUT_TYPE) ((ct_release_cfg2_value & BITM_CT_CT_TOUCH_CFG2_TCH_SEL_INPB) >> BITP_CT_CT_TOUCH_CFG2_TCH_SEL_INPB);

  ReleaseSettings->MinDuration = (ct_release_cfg2_value & BITM_CT_CT_TOUCH_CFG2_TCH_MIN_DURATION) >> BITP_CT_CT_TOUCH_CFG2_TCH_MIN_DURATION;

  return ADI_CT_RESULT_SUCCESS;


}


/*!
 * @brief       Read the CDC value of a Stage
 *
 * @param[in]   hDevice             Device handle obtained from #adi_CT_Init().
 * @param[in]   StageNumber         Stage to Read
 *
 * @param[out]  CDCResult        CDC Value
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     The function reads the CDC value of the selected stage.
 *              It is recommended to use the #ADI_CT_STAGE_NUMBER_TYPE type to select
 *              the Stage to configure.
 *
 */

ADI_CT_RESULT_TYPE adi_CT_GetCDCValue(ADI_CT_DEV_HANDLE const hDevice,ADI_CT_STAGE_NUMBER_TYPE const StageNumber,uint16_t *CDCResult)

{


 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

  volatile const uint32_t *CDCValuePtr = NULL;

  CDCValuePtr = &hDevice->pCTRegs->CT_CDC_RES0 + (StageNumber >> 1);

  if(StageNumber & ADI_CT_ODD_STAGE)
  {
    *CDCResult = ((*CDCValuePtr) & BITM_CT_CT_CDC_RES0_CDC_RES1) >> BITP_CT_CT_CDC_RES0_CDC_RES1;
  }

  else
  {
    *CDCResult = ((*CDCValuePtr) & BITM_CT_CT_CDC_RES0_CDC_RES0) >> BITP_CT_CT_CDC_RES0_CDC_RES0;
  }


  return ADI_CT_RESULT_SUCCESS;

}


/*!
 * @brief       Read the Baseline value of a Stage
 *
 * @param[in]   hDevice             Device handle obtained from #adi_CT_Init().
 * @param[in]   StageNumber         Stage to Read
 *
 * @param[out]  BaselineResult        Baseline Value
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     The function reads the Baseline value of the selected stage.
 *              It is recommended to use the #ADI_CT_STAGE_NUMBER_TYPE type to select
 *              the Stage to configure.
 *
 */

ADI_CT_RESULT_TYPE adi_CT_GetBaselineValue(ADI_CT_DEV_HANDLE const hDevice,ADI_CT_STAGE_NUMBER_TYPE const StageNumber,uint16_t *BaselineResult)

{


 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

  volatile const uint32_t *BaselineValuePtr = NULL;

  BaselineValuePtr = &(hDevice->pCTRegs->CT_BASELINE0) + StageNumber;

  *BaselineResult = *BaselineValuePtr & BITM_CT_CT_BASELINE0_BASELINE0;

  return ADI_CT_RESULT_SUCCESS;

}

/*!
 * @brief       Read the Fast Filter value of a Stage
 *
 * @param[in]   hDevice             Device handle obtained from #adi_CT_Init().
 * @param[in]   StageNumber         Stage to Read
 *
 * @param[out]  FastFilterResult    Fast Filter Value
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     The function reads the Fast Filter value of the selected stage.
 *              It is recommended to use the #ADI_CT_STAGE_NUMBER_TYPE type to select
 *              the Stage to configure.
 *
 */

ADI_CT_RESULT_TYPE adi_CT_GetFastFilterValue(ADI_CT_DEV_HANDLE const hDevice,ADI_CT_STAGE_NUMBER_TYPE const StageNumber,uint16_t *FastFilterResult)

{


 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

  volatile const uint32_t *FastFilterValuePtr = NULL;

  FastFilterValuePtr = &(hDevice->pCTRegs->CT_BASELINE0) + StageNumber;

  *FastFilterResult  = ((*FastFilterValuePtr) & BITM_CT_CT_BASELINE0_CDC_AVG0) >> BITP_CT_CT_BASELINE0_CDC_AVG0;

  return ADI_CT_RESULT_SUCCESS;

}

/*!
 * @brief       Read the Peak to Peak value of a Stage
 *
 * @param[in]   hDevice             Device handle obtained from #adi_CT_Init().
 * @param[in]   StageNumber         Stage to Read
 *
 * @param[out]  PK2PKResult    Peak to Peak Value
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     The function reads the Peak to Peak value of the selected stage.
 *              It is recommended to use the #ADI_CT_STAGE_NUMBER_TYPE type to select
 *              the Stage to configure.
 *
 */

ADI_CT_RESULT_TYPE adi_CT_GetPK2PKValue(ADI_CT_DEV_HANDLE const hDevice,ADI_CT_STAGE_NUMBER_TYPE const StageNumber,uint16_t *PK2PKResult)

{


 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

  volatile const uint32_t *PK2PKValuePtr = NULL;

  PK2PKValuePtr = &(hDevice->pCTRegs->CT_PK2PK0) + (StageNumber>>1);

  if(StageNumber & ADI_CT_ODD_STAGE)
  {
    *PK2PKResult = (*PK2PKValuePtr & BITM_CT_CT_PK2PK0_PK2PK1) >> BITP_CT_CT_PK2PK0_PK2PK1;
  }

  else
  {
    *PK2PKResult = *PK2PKValuePtr & BITM_CT_CT_PK2PK0_PK2PK0;
  }


  return ADI_CT_RESULT_SUCCESS;

}


/*!
 * @brief       Enable the CT Interrupts
 *
 * @param[in]   hDevice                 Device handle obtained from #adi_CT_Init().
 * @param[in]   CaptouchInterrupts      Interrupt Status
 * @param[in]   enable      			Status
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     To enable a specific Interrupt, the bit associated to that Interrupt must be 1.
 *              It is recommended to use the #ADI_CT_INTERRUPT_TYPE type.
 *
 */


ADI_CT_RESULT_TYPE adi_CT_SetInterruptsEnable(ADI_CT_DEV_HANDLE const hDevice,uint8_t const CaptouchInterrupts,bool_t const enable){



 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

  if(enable)
  {
    hDevice->pCTRegs->CT_IEN |= CaptouchInterrupts;
  }

  else
  {
    hDevice->pCTRegs->CT_IEN &= ~(CaptouchInterrupts);
  }



  return ADI_CT_RESULT_SUCCESS;

}

/*!
 * @brief       Get the CT Interrupt
 *
 * @param[in]   hDevice                 Device handle obtained from #adi_CT_Init().
 *
 * @param[out]   CaptouchInterrupts     Enabled Interrupt
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     If the bit associated to the Interrupt is 1, this interrupt is activated.
 *
 *
 */

ADI_CT_RESULT_TYPE adi_CT_GetInterruptsEnable(ADI_CT_DEV_HANDLE const hDevice,uint8_t *CaptouchInterrupts){



 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

  *CaptouchInterrupts = (uint8_t)(hDevice->pCTRegs->CT_IEN);

  return ADI_CT_RESULT_SUCCESS;

}



/*!
 * @brief       Set the Interrupt Flag
 *
 * @param[in]   hDevice                 Device handle obtained from #adi_CT_Init().
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     The Interrupt Flag variable is defined in #ADI_CT_DEV_HANDLE.
 *              Setting this variable, the main program will be notified that an Interrupt has been generated.
 *
 *
 */

ADI_CT_RESULT_TYPE adi_CT_SetInterruptFlag (ADI_CT_DEV_HANDLE const hDevice)
{

 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

  hDevice->bCT_EventFlag = true;

  return ADI_CT_RESULT_SUCCESS;

}

/*!
 * @brief       Clear the Interrupt Flag
 *
 * @param[in]   hDevice                 Device handle obtained from #adi_CT_Init().
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     The Interrupt Flag variable is defined in #ADI_CT_DEV_HANDLE.
 *              This function must be used by the main program to notify that it managed the Interrupt and
 *              it is ready to be notify of new events.
 *
 *
 */

ADI_CT_RESULT_TYPE adi_CT_ClearInterruptFlag (ADI_CT_DEV_HANDLE const hDevice)
{


 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

  hDevice->bCT_EventFlag = false;

  return ADI_CT_RESULT_SUCCESS;

}



/*!
 * @brief       Get the Interrupt Flag
 *
 * @param[in]   hDevice                 Device handle obtained from #adi_CT_Init().
 *
 * @param[out]  EventFlag           bool_t
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     The Interrupt Flag variable is defined in #ADI_CT_DEV_HANDLE.
 *              This function will return the current status of this Flag.
 *
 *
 */

ADI_CT_RESULT_TYPE adi_CT_GetInterruptFlag (ADI_CT_DEV_HANDLE const hDevice,bool_t *EventFlag)
{



 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

 #endif

  *EventFlag = hDevice->bCT_EventFlag;

  return ADI_CT_RESULT_SUCCESS;

}

/*!
 * @brief       Get the Interrupt Status
 *
 * @param[in]   hDevice             Device handle obtained from #adi_CT_Init().
 *
 * @param[out]  EventType           Captouch Interrrupt Status Register
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     This function gets the current Status of the Captouch Interrupt Register
 *
 */

ADI_CT_RESULT_TYPE adi_CT_GetInterruptStatus(ADI_CT_DEV_HANDLE const hDevice,uint8_t *EventType)

{


 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif


  *EventType = hDevice->CTEventType;

  return ADI_CT_RESULT_SUCCESS;

}


/*!
 * @brief       Get the Offset High Status
 *
 * @param[in]   hDevice             Device handle obtained from #adi_CT_Init().
 *
 *
 * @param[out]  OffsHStatus        Offset High Status Register
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     This function gets the current Status of the Captouch Offset High Status Register
 *
 */

ADI_CT_RESULT_TYPE adi_CT_GetOffsHStatus(ADI_CT_DEV_HANDLE const hDevice,uint16_t *OffsHStatus)

{


 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

  *OffsHStatus = hDevice->pCTRegs->CT_OFFS_HSTAT & BITM_CT_CT_OFFS_HSTAT_OFFS_HSTAT;

  return ADI_CT_RESULT_SUCCESS;

}

/*!
 * @brief       Get the Offset Low Status
 *
 * @param[in]   hDevice             Device handle obtained from #adi_CT_Init().
 *
 *
 * @param[out]  OffsLStatus        Offset Low Status Register
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     This function gets the current Status of the Offset Low Status Register
 *
 */

ADI_CT_RESULT_TYPE adi_CT_GetOffsLStatus(ADI_CT_DEV_HANDLE const hDevice,uint16_t *OffsLStatus)

{


 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

  *OffsLStatus = hDevice->pCTRegs->CT_OFFS_LSTAT & BITM_CT_CT_OFFS_LSTAT_OFFS_LSTAT;

  return ADI_CT_RESULT_SUCCESS;

}

/*!
 * @brief       Get the Slow Proximity Status
 *
 * @param[in]   hDevice           Device handle obtained from #adi_CT_Init().
 *
 * @param[out]  ProxStatus        Slow Proximity Status
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     This function gets the current Status of the Slow Proximity Status Register
 *
 */


ADI_CT_RESULT_TYPE adi_CT_GetProxStatus(ADI_CT_DEV_HANDLE const hDevice,uint16_t *ProxStatus)

{


 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

  *ProxStatus = hDevice->pCTRegs->CT_PROX_STAT & BITM_CT_CT_PROX_STAT_PROX_STAT;

  return ADI_CT_RESULT_SUCCESS;

}

/*!
 * @brief       Get the Fast Proximity Status
 *
 * @param[in]   hDevice           Device handle obtained from #adi_CT_Init().
 *
 * @param[out]  FProxStatus        Fast Proximity Status
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     This function gets the current Status of the Fast Proximity Status Register
 *
 */

ADI_CT_RESULT_TYPE adi_CT_GetFProxStatus(ADI_CT_DEV_HANDLE const hDevice,uint16_t *FProxStatus)

{


 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

  *FProxStatus = hDevice->pCTRegs->CT_FPROX_STAT & BITM_CT_CT_FPROX_STAT_FPROX_STAT;

  return ADI_CT_RESULT_SUCCESS;

}

/*!
 * @brief       Get the Touch Status
 *
 * @param[in]   hDevice           Device handle obtained from #adi_CT_Init().
 *
 * @param[out]  TouchStatus       Touch Status
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     This function gets the current Touch Detection Algorithm Status
 *
 */


static ADI_CT_RESULT_TYPE adi_CT_GetTouchStatus(ADI_CT_DEV_HANDLE const hDevice,uint16_t *TouchStatus)

{


 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

  *TouchStatus = hDevice->pCTRegs->CT_TOUCH_STAT & BITM_CT_CT_TOUCH_STAT_TCH_DETECT_STAT;

  /* To clear the Register */
  //hDevice->pCTRegs->CT_TOUCH_STAT = *TouchStatus;

  return ADI_CT_RESULT_SUCCESS;

}


/*!
 * @brief       Clear the Touch Status
 *
 * @param[in]   hDevice           Device handle obtained from #adi_CT_Init().
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     This function clear the Touch Detection Register Status
 *
 */


static ADI_CT_RESULT_TYPE adi_CT_ClearTouchStatus(ADI_CT_DEV_HANDLE const hDevice,uint16_t const TouchStatus)

{


 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

    hDevice->pCTRegs->CT_TOUCH_STAT = TouchStatus << BITP_CT_CT_TOUCH_STAT_TCH_DETECT_STAT;


  return ADI_CT_RESULT_SUCCESS;

}


/*!
 * @brief       Get the Release Status
 *
 * @param[in]   hDevice           Device handle obtained from #adi_CT_Init().
 *
 * @param[out]  ReleaseStatus     Release Status
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     This function gets the current Release Detection Algorithm Status
 *
 */

static ADI_CT_RESULT_TYPE adi_CT_GetReleaseStatus(ADI_CT_DEV_HANDLE const hDevice,uint16_t *ReleaseStatus)

{


 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

  *ReleaseStatus = (hDevice->pCTRegs->CT_TOUCH_STAT & BITM_CT_CT_TOUCH_STAT_REL_DETECT_STAT) >> BITP_CT_CT_TOUCH_STAT_REL_DETECT_STAT;

  return ADI_CT_RESULT_SUCCESS;

}


/*!
 * @brief       Clear the Release Status
 *
 * @param[in]   hDevice           Device handle obtained from #adi_CT_Init().
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     This function clear the Touch Detection Register Status
 *
 */


static ADI_CT_RESULT_TYPE adi_CT_ClearReleaseStatus(ADI_CT_DEV_HANDLE const hDevice,uint16_t const ReleaseStatus)

{


 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

  hDevice->pCTRegs->CT_TOUCH_STAT = ReleaseStatus << BITP_CT_CT_TOUCH_STAT_REL_DETECT_STAT;

  return ADI_CT_RESULT_SUCCESS;

}



/*!
 * @brief       Force the Automatically Baseline Calibration
 *
 * @param[in]   hDevice             Device handle obtained from #adi_CT_Init().
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     This function writes in the register to Calibrate automatically the Baseline
 *
 */

ADI_CT_RESULT_TYPE adi_CT_SetAutoForceBaselineCal(ADI_CT_DEV_HANDLE const hDevice){



 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif


  hDevice->pCTRegs->CT_BASELINE_CTRL |= BITM_CT_CT_BASELINE_CTRL_FORCE_CAL;

  return ADI_CT_RESULT_SUCCESS;
}


/*!
 * @brief       Starts a single C2V conversion sequence
 *
 * @param[in]   hDevice             Device handle obtained from #adi_CT_Init().
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init().
 *
 * @details     This function forces a single C2V conversion
 *
 */

ADI_CT_RESULT_TYPE adi_CT_SetStartSingleSequence(ADI_CT_DEV_HANDLE const hDevice){



 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif


  hDevice->pCTRegs->CT_CDC_PWR |= BITM_CT_CT_CDC_PWR_SW_START_SEQ;

  return ADI_CT_RESULT_SUCCESS;
}


/*!
 * @brief       Get the Touch And Release Algorithm Status
 *
 * @param[in]   hDevice           Device handle obtained from #adi_CT_Init().
 *
 * @param[out]  ButtonStatus      Button Status
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                               Call completed successfully.
 *              - #ADI_CT_RESULT_ERR                            Error: Device has not been initialized for use, see #adi_CT_Init().
 *              - #ADI_CT_RESULT_ERR_TCH_REL_NOT_ENABLE         Error:  The Touch and Release feature is not enable.
 *
 * @details     This function gets the current Button Status. Each bit represents a button. To use this function
 *              is necessary to enable the Touch and Release feature: #adi_CT_SetTouchEnable() and
 *              #adi_CT_SetReleaseEnable();
 *                      bit 0 --> Input A
 *                      bit 1 --> Input B
 *                      bit 2 --> Input C
 *                      bit 3 --> Input D
 *                      bit 4 --> Input E
 *                      bit 5 --> Input F
 *
 *              1 -> Button is touched
 *              0 -> Button is released
 *
 *
 */


ADI_CT_RESULT_TYPE adi_CT_GetTchAndRelAlgorithmStatus(ADI_CT_DEV_HANDLE const hDevice,uint8_t *ButtonStatus)

{


 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

  bool_t TouchEnable;
  bool_t ReleaseEnable;


  uint16_t TouchStatus;
  uint16_t ReleaseStatus;


  /* By default, they have a non possible stage value (possible values 0-15) */

  uint8_t INPUT_STAGE_A = ADI_CT_STAGE0_NUMBER;
  uint8_t INPUT_STAGE_B = ADI_CT_STAGE1_NUMBER;
  uint8_t INPUT_STAGE_C = ADI_CT_STAGE2_NUMBER;
  uint8_t INPUT_STAGE_D = ADI_CT_STAGE3_NUMBER;
  uint8_t INPUT_STAGE_E = ADI_CT_STAGE4_NUMBER;
  uint8_t INPUT_STAGE_F = ADI_CT_STAGE5_NUMBER;



  /* 1. Check that the Touch and Release Algorithm is enable (if not return error) */

  adi_CT_GetTouchEnable(hDevice,&TouchEnable);
  adi_CT_GetReleaseEnable(hDevice,&ReleaseEnable);

  if(!(TouchEnable && ReleaseEnable))

  {
      return(ADI_CT_RESULT_ERR_TCH_REL_NOT_ENABLE);
  }


  /* 3. Check the Touch Status.  */

  adi_CT_GetTouchStatus(hDevice,&TouchStatus);

  if(TouchStatus & (uint16_t)(1 << INPUT_STAGE_A))
  {
    *ButtonStatus |= ADI_CT_STATUS_BIT_A;
    //adi_CT_ClearTouchStatus(hDevice,(1 << INPUT_STAGE_A));
  }

  if(TouchStatus & (uint16_t)(1 << INPUT_STAGE_B))
  {
    *ButtonStatus |= ADI_CT_STATUS_BIT_B;
    //adi_CT_ClearTouchStatus(hDevice,(1 << INPUT_STAGE_B));
  }

  if(TouchStatus & (uint16_t)(1 << INPUT_STAGE_C)) {
    *ButtonStatus |= ADI_CT_STATUS_BIT_C;
    //adi_CT_ClearTouchStatus(hDevice,(1 << INPUT_STAGE_C));
  }
  if(TouchStatus & (uint16_t)(1 << INPUT_STAGE_D)){
    *ButtonStatus |= ADI_CT_STATUS_BIT_D;
    //adi_CT_ClearTouchStatus(hDevice,(1 << INPUT_STAGE_D));
  }
  if(TouchStatus & (uint16_t)(1 << INPUT_STAGE_E)) {
    *ButtonStatus |= ADI_CT_STATUS_BIT_E;
    //adi_CT_ClearTouchStatus(hDevice,(1 << INPUT_STAGE_E));
  }
  if(TouchStatus & (uint16_t)(1 << INPUT_STAGE_F)){
    *ButtonStatus |= ADI_CT_STATUS_BIT_F;
    //adi_CT_ClearTouchStatus(hDevice,(1 << INPUT_STAGE_F));
  }



  /* 4. Check the Release Status and clear the touch bit to detect new touch interrupts */

  adi_CT_GetReleaseStatus(hDevice,&ReleaseStatus);

  if(ReleaseStatus & (1 << INPUT_STAGE_A))
  {
    *ButtonStatus &= ~(ADI_CT_STATUS_BIT_A);
    adi_CT_ClearTouchStatus(hDevice,(1 << INPUT_STAGE_A));
    adi_CT_ClearReleaseStatus(hDevice,(1 << INPUT_STAGE_A));
  }
  if(ReleaseStatus & (1 << INPUT_STAGE_B))
  {
    *ButtonStatus &= ~(ADI_CT_STATUS_BIT_B);
    adi_CT_ClearTouchStatus(hDevice,(1 << INPUT_STAGE_B));
    adi_CT_ClearReleaseStatus(hDevice,(1 << INPUT_STAGE_B));
  }
  if(ReleaseStatus & (1 << INPUT_STAGE_C))
  {
    *ButtonStatus &= ~(ADI_CT_STATUS_BIT_C);
    adi_CT_ClearTouchStatus(hDevice,(1 << INPUT_STAGE_C));
    adi_CT_ClearReleaseStatus(hDevice,(1 << INPUT_STAGE_C));
  }
  if(ReleaseStatus & (1 << INPUT_STAGE_D))
  {
    *ButtonStatus &= ~(ADI_CT_STATUS_BIT_D);
    adi_CT_ClearTouchStatus(hDevice,(1 << INPUT_STAGE_D));
    adi_CT_ClearReleaseStatus(hDevice,(1 << INPUT_STAGE_D));
  }
  if(ReleaseStatus & (1 << INPUT_STAGE_E))
  {
    *ButtonStatus &= ~(ADI_CT_STATUS_BIT_E);
    adi_CT_ClearTouchStatus(hDevice,(1 << INPUT_STAGE_E));
    adi_CT_ClearReleaseStatus(hDevice,(1 << INPUT_STAGE_E));
  }
  if(ReleaseStatus & (1 << INPUT_STAGE_F))
  {
    *ButtonStatus &= ~(ADI_CT_STATUS_BIT_F);
    adi_CT_ClearTouchStatus(hDevice,(1 << INPUT_STAGE_F));
    adi_CT_ClearReleaseStatus(hDevice,(1 << INPUT_STAGE_F));
  }

  return ADI_CT_RESULT_SUCCESS;

}



/*************************************************************************************************
**************************************************************************************************
*****************************************   CALLBACKS   ******************************************
*****************************************      AND      ******************************************
*****************************************   INTERRUPT   ******************************************
*****************************************     STUFF     ******************************************
**************************************************************************************************
*************************************************************************************************/


/*!
    @brief       CT Application callback registration API.

    @param[in]   hDevice     Device handle obtained from #adi_CT_Init().
    @param[in]   cbFunc      Application callback address; the function to call.
    @param[in]   cbWatch     The interrupts to watch, controlling which interrupts Status in a callback.

    @return    Status
               - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
               - #ADI_CT_RESULT_ERR                    Error: Device has not been initialized for use, see #adi_CT_Init()

    @note     Registers an application-provided callback function.  Callbacks are made in response to CT interrupts,
              depending on whether they are enabled and whether the corresponding callback watch bits are set.
              This allosw the application to control which interrupts it wants to be called back on.

              The callback to the application is made in context of the originating interrupt, so extended processing
              is discouraged soas to not lock out lower-priority interrupts.


*/



ADI_CT_RESULT_TYPE adi_CT_RegisterCallback(ADI_CT_DEV_HANDLE hDevice, ADI_CALLBACK  cbFunc, ADI_CT_INTERRUPT_TYPE const cbWatch) {

 #ifdef ADI_DEBUG

    if( hDevice->DrvState != ADI_CT_DRV_STATE_INITIALIZED)
    {
        return(ADI_CT_RESULT_ERR);
    }

#endif

    /* save the callback info */
    hDevice->cbFunc  = cbFunc;
    hDevice->cbWatch = cbWatch;

    return ADI_CT_RESULT_SUCCESS;
}



/* Interrupt Handler Function */

/*! \cond PRIVATE */
//void CapTouch_Int_Handler(void)
ADI_INT_HANDLER(CapTouch_Int_Handler)
{

  ADI_CT_DEV_HANDLE    pDev     = &CT_DevData[ADI_CT_DEV_ID_0];

  ADI_CT_INTERRUPT_TYPE fired;

  /* Set the Event Flag */
  pDev->bCT_EventFlag = true;

  /* Get Event Interrupts */
  fired = (ADI_CT_INTERRUPT_TYPE)(pDev->pCTRegs->CT_INT);

  /* if we have a callback... */
    if (pDev->DrvState && pDev->cbFunc) {

        /* forward to the user if he is watching this interrupt */
        if (pDev->cbWatch & fired)
            pDev->cbFunc (pDev, fired, NULL);
    }

  /* Clear the interrupt register */
  pDev->pCTRegs->CT_INT = pDev->pCTRegs->CT_IEN;


  __DSB();



}

/*! \endcond */
/* C++ linkage */
#ifdef __cplusplus
}
#endif


/*
** EOF
*/

/*@}*/
