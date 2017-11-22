/*********************************************************************************

Copyright (c) 2012-2014 Analog Devices, Inc. All Rights Reserved.

This software is proprietary to Analog Devices, Inc. and its licensors.  By using 
this software you agree to the terms of the associated Analog Devices Software 
License Agreement.

*********************************************************************************/

/*!
 *****************************************************************************
   @file:    adi_ct_config.h
   @brief:   Configuration options for Captouch driver.
             This is specific to the Captouch driver and will be included by the driver.
             It is not required for the application to include this header file.
   @version: $Revision: 28525 $
   @date:    $Date: 2014-11-12 14:51:26 -0500 (Wed, 12 Nov 2014) $
*****************************************************************************/

#ifndef __ADI_CT_CONIG_H__
#define __ADI_CT_CONIG_H__

/************* Captouch Driver configurations ***************/

/************* Captouch controller static configurations ***************/


/* POWER REGISTER  */

/*! Configure the power mode.
    0 -  Power Mode.
    1 -  Shutdown Mode. */
#define CT_CDC_PWR_MODE_SHUTDOWN			1


/* CONFIGURATION REGISTER 1 */

/*! C2V integration cycles number [26:24].
    0 -  1 cycle.
    1 -  2 cycles. 
    2 -  4 cycles.
    3 -  8 cycles.
    4 -  16 cycles.
	*/
#define CT_CFG_1_INT_CYCLES				2

/*! Drive unmatched pads [23:22].
    0 -  Unmatched pads to AC_DRV.
    1 -  Unmatched pads to DC_DRV. 
    2 -  Unmatched pads to GND.
    3 -  Unmatched pads floated.
	*/
#define CT_CFG_1_AIN_SEL				0


/*! Autozero Duty [12:10].
    0-7--> (2us+LSB)us
	*/
#define CT_CFG_1_AUTOZERO_DUTY				0


/*! Bypass PGA Gain.
    0 - Do not override the PGA
    1 - Override the PGA
	*/
#define CT_CFG_1_BYPASS_GAIN            		0

/*! Internal Buffer Bias current [7:6].
    0 -  2.5us.
    1 -  3us.
    2 -  3.5us.
    3 -  4us.
	*/
#define CT_CFG_1_INT_BUFFER				1

/*! Gain Amplifier Bias current [5:3].
    0-7--> (3us+LSB*0.5)us
	*/
#define CT_CFG_1_PGA_BIAS				4

/*! C2V Bias current [2:0].
  0-7--> (6us+LSB)us
	*/
#define CT_CFG_1_C2V_BIAS				4


/* CONFIGURATION REGISTER 2 */

/*! C2V Low Pass Filter control
    0 - Disable
    1 - Enable
	*/
#define CT_CFG_2_C2V_LPF_ENABLE 				1

/*! Self timer control
    0 - Disable
    1 - Enable
	*/
#define CT_CFG_2_SELFTIMER_ENABLE				1

/*! C2V_HOLD_TIME [26:22].
    0-31--> (0.25(1+LSB))us
	*/
#define CT_CFG_2_C2V_HOLD_TIME			        16

/*! SELF TIMER value [21:13].
    0-511--> (1.953*LSB)ms
	*/
#define CT_CFG_2_SELFTIMER_VALUE			5

/*! Phase 24 Duty value [12:5].
    0-255--> (62.5(1+LSB))ns
	*/
#define CT_CFG_2_PH24					63

/*! Phase 13 Duty value [4:0].
    0-31--> (0.25(1+LSB))us
	*/
#define CT_CFG_2_PH13					7


/* CONFIGURATION REGISTER 3 */

/*! Peak to peak average[27:25].
    0 -  2 samples
    1 -  4 samples 
    2 -  8 samples
    3 -  16 samples
    4 -  32 samples
	*/
#define CT_CFG_3_PK2PK_AVG				0

/*! Minimum rank position for averaging[24:20].
      0-31
	*/
#define CT_CFG_3_PK2PK_AVG_MIN				0

/*! Number of samples used for the peak-to-peak noise calculations[19:16].
      0-15
	*/
#define CT_CFG_3_PK2PK_SUBSET				0

/*! Samples for each peak-to-peak measurement[15:14].
    0 -  4 samples
    1 -  8 samples 
	2 -  16 samples
    3 -  32 samples
	*/
#define CT_CFG_3_PK2PK_NUM_SPLS				0

/*! Result select [13].
    0 -  CDC_REsn contains the filtered CDC Result
    1 -  CDC_REsn contains the difference between CDC result and Baseline
	*/
#define CT_CFG_3_RES_SEL			        0

/*! Enable Captouch operation [10:5].
    000001 - Enable Input A
    000010 - Enable Input B
    000100 - Enable Input C
    001000 - Enable Input D
    010000 - Enable Input E
    100000 - Enable Input F
	*/
#define CT_CFG_3_DIV_CTOV_CAPTOUCH_EN			63

/*! CDC IIR Filter weight[4:1].
	0-15
	*/
#define CT_CFG_3_IIR_WEIGHT				8	

/*! IIR enable [0].
    0 -  Disable
    1 -  Enable
	*/
#define CT_CFG_3_IIR_ENABLE      			1					


/* MEASUREMENT SELECTION */

/*! Enable Selected Stages for measurements[15:0].
	 One bit per stage
	*/
#define CT_MEAS_SEL_STAGE			        62	

/* BASELINE CONTROL */

/*! Baseline calibration delay [31:29].
    0 - 8 x FAST_FILTER_UPDATE_RATE
    1 - 16 x FAST_FILTER_UPDATE_RATE
    2 - 24 x FAST_FILTER_UPDATE_RATE
    3 - 32 x FAST_FILTER_UPDATE_RATE
    4 - 40 x FAST_FILTER_UPDATE_RATE
    5 - 48 x FAST_FILTER_UPDATE_RATE
    6 - 56 x FAST_FILTER_UPDATE_RATE
    7 - 64 x FAST_FILTER_UPDATE_RATE		
	*/
#define CT_BASELINE_CAL_DELAY		    		0

/*! Fast Filter Update[26:24].
    0 - New data input to fast filter every 2nd conversion cycle.
    1 - New data input to fast filter every 4th conversion cycle.
    2 - New data input to fast filter every 6th conversion cycle.
    3 - New data input to fast filter every 8th conversion cycle.	
    4 - New data input to fast filter every 10th conversion cycle.
    5 - New data input to fast filter every 12th conversion cycle.
    6 - New data input to fast filter every 14th conversion cycle.
    7 - New data input to fast filter every 16th conversion cycle.
	*/
#define CT_BASELINE_FAST_FILTER_UPDATE			 0

/*! Fast Proximity Level[23:16].
	Level = FAST_PROX x 4
	0-255 
	*/
#define CT_BASELINE_FAST_PROX				50	

/*! Slow Proximity Level[15:8].
	Level = SLOW_PROX x 4
	0-255 
	*/
#define CT_BASELINE_SLOW_PROX				10	

/*! Baseline calculation slow filter new sample weight[7:4].
	Weight = 1 + LSB.
	0-15 
	*/
#define CT_BASELINE_BL_COEFF			        15	

/*! Baseline calculation fast filter new sample weight[3:0].
	Weight = 1 + LSB.
	0-15 
	*/
#define CT_BASELINE_CDC_COEFF				1	


/* AVERAGE SETTINGS */

/*! Number of samples to be ranked[30:28].
    0 -  No ranking
    1 -  4 samples 
    2 -  8 samples
    3 -  16 samples
    4 -  32 samples
	*/
#define CT_AVG_RANK					2

/*! Select the average group[25:24].
    0 - No averaging
	1 - Average of Group 1
	2 - Average of Group 2
	*/
#define CT_AVG_OUTPUT  					1

/*! NUmber of samples in averager group 2[22:20].
    0 - No samples
    1 - 2 samples
    2 - 4 samples
    3 - 8 samples
    4 - 16 samples
    5 - 32 samples
	*/
#define CT_AVG_SAMPLES_GROUP2  			        0

/*! NUmber of samples in averager group 1[18:16].
    0 - No samples
    1 - 2 samples
    2 - 4 samples
    3 - 8 samples
    4 - 16 samples
    5 - 32 samples
	*/
#define CT_AVG_SAMPLES_GROUP1  			        2

/*! Minimum rank posiion to be averaged in averager set 2[12:8].
    0-31
	*/
#define CT_AVG_MIN_GROUP2				0

/*! Minimum rank posiion to be averaged in averager set 1[4:0].
    0-31
	*/
#define CT_AVG_MIN_GROUP1				2

/* CALIBRATION ENABLE */

/*! Enable Calibration for measurement staged[15:0].
	 One bit per stage
	*/
#define CT_CAL_EN_STAGE						62	

/* TOUCH REGISTER CONFIGURATION 1 */

/*! Touch Upper Threshold[29:16].
	 0-16383
	*/
#define CT_TOUCH_CFG1_UPPER_THLD						450	

/*! Touch Lower Threshold[13:0].
	 0-16383
	*/
#define CT_TOUCH_CFG1_LOWER_THLD						400	

/* TOUCH REGISTER CONFIGURATION 2 */

/*! Individual Threshold[23].
	 0 - CT_TOUCH_CFG1 value used as threshold 
	 1 - CT_SENSOR_THR_CFGx value used as  threshold (individual threshold per stage)
	*/
#define CT_TOUCH_CFG2_INDIVIDUAL_THRESHOLD_ENABLE			0	

/*! Rising edge detection[22].
	 0 - Active if inA-inB < threshold for MIN_DURATION
	 1 - Active if inA-inB > threshold for MIN_DURATION
	*/
#define CT_TOUCH_CFG2_EVENT_ON_HIGH			1

/*! Input Source A[21:19].
	 0 - Load All zeros
	 1 - Load CDC values
	 2 - Load slow filter output
	 3 - Load fast filter output
	 4 - Load ambient value (baseline)
	*/
#define CT_TOUCH_CFG2_SEL_INPA		4	

/*! Input Source B[18:16].
	 0 - Load All zeros
	 1 - Load CDC values
	 2 - Load slow filter output
	 3 - Load fast filter output
	 4 - Load ambient value (baseline)
	*/
#define CT_TOUCH_CFG2_SEL_INPB		1	

/*! Enable Touch detector[15].
	 0 - Disable
	 1 - Enable
	*/
#define CT_TOUCH_CFG2_DETECT_ENABLE			1

/*! Minimum touch duration[9:0].
	 0-1023
	*/
#define CT_TOUCH_CFG2_MIN_DURATION			10		


/* RELEASE REGISTER CONFIGURATION 1 */

/*! Release Upper Threshold[29:16].
	 0-16383
	*/
#define CT_REL_CFG1_UPPER_THLD						450	

/*! Release Lower Threshold[13:0].
	 0-16383
	*/
#define CT_REL_CFG1_LOWER_THLD						400	

/* RELEASE REGISTER CONFIGURATION 2 */

/*! Individual Threshold[23].
	 0 - CT_TOUCH_CFG1 value used as threshold 
	 1 - CT_SENSOR_THR_CFGx value used as  threshold (individual threshold per stage)
	*/
#define CT_REL_CFG2_INDIVIDUAL_THRESHOLD_ENABLE			0	

/*! Rising edge detection[22].
	 0 - Active if inA-inB < threshold for MIN_DURATION
	 1 - Active if inA-inB > threshold for MIN_DURATION
	*/
#define CT_REL_CFG2_EVENT_ON_HIGH			0

/*! Input Source A[21:19].
	 0 - Load All zeros
	 1 - Load CDC values
	 2 - Load slow filter output
	 3 - Load fast filter output
	 4 - Load ambient value (baseline)
	*/
#define CT_REL_CFG2_SEL_INPA		4	

/*! Input Source B[18:16].
	 0 - Load All zeros
	 1 - Load CDC values
	 2 - Load slow filter output
	 3 - Load fast filter output
	 4 - Load ambient value (baseline)
	*/
#define CT_REL_CFG2_SEL_INPB		1	

/*! Enable Touch detector[15].
	 0 - Disable
	 1 - Enable
	*/
#define CT_REL_CFG2_DETECT_ENABLE			1

/*! Minimum touch duration[9:0].
	 0-1023
	*/
#define CT_REL_CFG2_MIN_DURATION			10	

/* INTERRUPT ENABLE */

/*! Release interrupt enable[4].
	 0 - Disable
	 1 - Enable
	*/
#define CT_IEN_RELEASE_ENABLE			1

/*! Touch interrupt enable[3].
	 0 - Disable
	 1 - Enable
	*/
#define CT_IEN_TOUCH_ENABLE				1

/*! Proximity interrupt enable[2].
	 0 - Disable
	 1 - Enable
	*/
#define CT_IEN_PROX_ENABLE				0

/*! Threshold interrupt enable[1].
	 0 - Disable
	 1 - Enable
	*/
#define CT_IEN_THRESHOLD_ENABLE			0

/*! Conversion set complete interrupt enable[0].
	 0 - Disable
	 1 - Enable
	*/
#define CT_IEN_CONV_SET_COMPLETE_ENABLE			0


/* STAGE 0 CONFIGURATION */

/*! Enable the CDC excitation[27].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE0_CFG_CIN_CON_POS_CDC_EN			1

/*! Select the CDC input[26:24].
	 0 - INPUT A
	 1 - INPUT B
	 2 - INPUT C
	 3 - INPUT D
	 4 - INPUT E
	 5 - INPUT F
	*/
#define CT_STAGE0_CFG_CIN_CON_POS_CDC			0

/*! Bypass PGA gain[23].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE0_CFG_PGA_BYPASS					1

/*! Disable the excitation to execute a noise reduction algorithm[22].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE0_CFG_NOISE_REDUCTION_ENABLE		0

/*! PGA gain ig PGA Bypass is equal to 0[21:20].
	 0 - 2
	 1 - 4
	 2 - 8
	 3 - 16
	*/
#define CT_STAGE0_CFG_PGA_GAIN					0

/*! C2V Range[19:16].
	 0-15
	 Range = (0.583(1+LSB))pF
	*/
#define CT_STAGE0_CFG_C2V_RANGE					4

/*! Enable CAP DAC[15].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE0_CFG_CAPDAC_ENABLE				1

/*! Enable Peak-to-peak measurements[13].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE0_CFG_PK2PK_ENABLE				0

/*! MAIN DAC Value[9:3].
	 0-127
	 Range = (388.5(1+LSB))fF
	*/
#define CT_STAGE0_CFG_MAIN_DAC					0

/*! SUB DAC Value[2:0].
	 0-7
	 Range = (48.5(1+LSB))fF
	*/
#define CT_STAGE0_CFG_SUB_DAC					0


/* STAGE 1 CONFIGURATION */

/*! Enable the CDC excitation[27].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE1_CFG_CIN_CON_POS_CDC_EN			1

/*! Select the CDC input[26:24].
	 0 - INPUT A
	 1 - INPUT B
	 2 - INPUT C
	 3 - INPUT D
	 4 - INPUT E
	 5 - INPUT F
	*/
#define CT_STAGE1_CFG_CIN_CON_POS_CDC				1

/*! Bypass PGA gain[23].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE1_CFG_PGA_BYPASS					1

/*! Disable the excitation to execute a noise reduction algorithm[22].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE1_CFG_NOISE_REDUCTION_ENABLE		0

/*! PGA gain ig PGA Bypass is equal to 0[21:20].
	 0 - 2
	 1 - 4
	 2 - 8
	 3 - 16
	*/
#define CT_STAGE1_CFG_PGA_GAIN					0

/*! C2V Range[19:16].
	 0-15
	 Range = (0.583(1+LSB))pF
	*/
#define CT_STAGE1_CFG_C2V_RANGE					4

/*! Enable CAP DAC[15].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE1_CFG_CAPDAC_ENABLE				1

/*! Enable Peak-to-peak measurements[13].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE1_CFG_PK2PK_ENABLE				0

/*! MAIN DAC Value[9:3].
	 0-127
	 Range = (388.5(1+LSB))fF
	*/
#define CT_STAGE1_CFG_MAIN_DAC					0

/*! SUB DAC Value[2:0].
	 0-7
	 Range = (48.5(1+LSB))fF
	*/
#define CT_STAGE1_CFG_SUB_DAC					0

/* STAGE 2 CONFIGURATION */

/*! Enable the CDC excitation[27].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE2_CFG_CIN_CON_POS_CDC_EN			1

/*! Select the CDC input[26:24].
	 0 - INPUT A
	 1 - INPUT B
	 2 - INPUT C
	 3 - INPUT D
	 4 - INPUT E
	 5 - INPUT F
	*/
#define CT_STAGE2_CFG_CIN_CON_POS_CDC				2	

/*! Bypass PGA gain[23].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE2_CFG_PGA_BYPASS					1

/*! Disable the excitation to execute a noise reduction algorithm[22].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE2_CFG_NOISE_REDUCTION_ENABLE		0

/*! PGA gain ig PGA Bypass is equal to 0[21:20].
	 0 - 2
	 1 - 4
	 2 - 8
	 3 - 16
	*/
#define CT_STAGE2_CFG_PGA_GAIN					0

/*! C2V Range[19:16].
	 0-15
	 Range = (0.583(1+LSB))pF
	*/
#define CT_STAGE2_CFG_C2V_RANGE					4

/*! Enable CAP DAC[15].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE2_CFG_CAPDAC_ENABLE				1

/*! Enable Peak-to-peak measurements[13].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE2_CFG_PK2PK_ENABLE				0

/*! MAIN DAC Value[9:3].
	 0-127
	 Range = (388.5(1+LSB))fF
	*/
#define CT_STAGE2_CFG_MAIN_DAC					0

/*! SUB DAC Value[2:0].
	 0-7
	 Range = (48.5(1+LSB))fF
	*/
#define CT_STAGE2_CFG_SUB_DAC					0

/* STAGE 3 CONFIGURATION */

/*! Enable the CDC excitation[27].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE3_CFG_CIN_CON_POS_CDC_EN			1

/*! Select the CDC input[26:24].
	 0 - INPUT A
	 1 - INPUT B
	 2 - INPUT C
	 3 - INPUT D
	 4 - INPUT E
	 5 - INPUT F
	*/
#define CT_STAGE3_CFG_CIN_CON_POS_CDC			3

/*! Bypass PGA gain[23].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE3_CFG_PGA_BYPASS					1

/*! Disable the excitation to execute a noise reduction algorithm[22].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE3_CFG_NOISE_REDUCTION_ENABLE		0

/*! PGA gain ig PGA Bypass is equal to 0[21:20].
	 0 - 2
	 1 - 4
	 2 - 8
	 3 - 16
	*/
#define CT_STAGE3_CFG_PGA_GAIN					0

/*! C2V Range[19:16].
	 0-15
	 Range = (0.583(1+LSB))pF
	*/
#define CT_STAGE3_CFG_C2V_RANGE					4

/*! Enable CAP DAC[15].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE3_CFG_CAPDAC_ENABLE				1

/*! Enable Peak-to-peak measurements[13].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE3_CFG_PK2PK_ENABLE				0

/*! MAIN DAC Value[9:3].
	 0-127
	 Range = (388.5(1+LSB))fF
	*/
#define CT_STAGE3_CFG_MAIN_DAC					0

/*! SUB DAC Value[2:0].
	 0-7
	 Range = (48.5(1+LSB))fF
	*/
#define CT_STAGE3_CFG_SUB_DAC					0

/* STAGE CONFIGURATION 0 */

/*! Enable the CDC excitation[27].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE3_CFG_CIN_CON_POS_CDC_EN			1

/*! Select the CDC input[26:24].
	 0 - INPUT A
	 1 - INPUT B
	 2 - INPUT C
	 3 - INPUT D
	 4 - INPUT E
	 5 - INPUT F
	*/
#define CT_STAGE3_CFG_CIN_CON_POS_CDC_BU_ONOFF			0

/*! Bypass PGA gain[23].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE3_CFG_PGA_BYPASS					1

/*! Disable the excitation to execute a noise reduction algorithm[22].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE3_CFG_NOISE_REDUCTION_ENABLE		0

/*! PGA gain ig PGA Bypass is equal to 0[21:20].
	 0 - 2
	 1 - 4
	 2 - 8
	 3 - 16
	*/
#define CT_STAGE3_CFG_PGA_GAIN					0

/*! C2V Range[19:16].
	 0-15
	 Range = (0.583(1+LSB))pF
	*/
#define CT_STAGE3_CFG_C2V_RANGE					4

/*! Enable CAP DAC[15].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE3_CFG_CAPDAC_ENABLE				1

/*! Enable Peak-to-peak measurements[13].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE3_CFG_PK2PK_ENABLE				0

/*! MAIN DAC Value[9:3].
	 0-127
	 Range = (388.5(1+LSB))fF
	*/
#define CT_STAGE3_CFG_MAIN_DAC					0

/*! SUB DAC Value[2:0].
	 0-7
	 Range = (48.5(1+LSB))fF
	*/
#define CT_STAGE3_CFG_SUB_DAC					0

/* STAGE 4 CONFIGURATION */

/*! Enable the CDC excitation[27].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE4_CFG_CIN_CON_POS_CDC_EN			1

/*! Select the CDC input[26:24].
	 0 - INPUT A
	 1 - INPUT B
	 2 - INPUT C
	 3 - INPUT D
	 4 - INPUT E
	 5 - INPUT F
	*/
#define CT_STAGE4_CFG_CIN_CON_POS_CDC			4

/*! Bypass PGA gain[23].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE4_CFG_PGA_BYPASS					1

/*! Disable the excitation to execute a noise reduction algorithm[22].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE4_CFG_NOISE_REDUCTION_ENABLE		0

/*! PGA gain ig PGA Bypass is equal to 0[21:20].
	 0 - 2
	 1 - 4
	 2 - 8
	 3 - 16
	*/
#define CT_STAGE4_CFG_PGA_GAIN					0

/*! C2V Range[19:16].
	 0-15
	 Range = (0.583(1+LSB))pF
	*/
#define CT_STAGE4_CFG_C2V_RANGE					4

/*! Enable CAP DAC[15].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE4_CFG_CAPDAC_ENABLE				1

/*! Enable Peak-to-peak measurements[13].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE4_CFG_PK2PK_ENABLE				0

/*! MAIN DAC Value[9:3].
	 0-127
	 Range = (388.5(1+LSB))fF
	*/
#define CT_STAGE4_CFG_MAIN_DAC					0

/*! SUB DAC Value[2:0].
	 0-7
	 Range = (48.5(1+LSB))fF
	*/
#define CT_STAGE4_CFG_SUB_DAC					0

/* STAGE 5 CONFIGURATION */

/*! Enable the CDC excitation[27].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE5_CFG_CIN_CON_POS_CDC_EN			1

/*! Select the CDC input[26:24].
	 0 - INPUT A
	 1 - INPUT B
	 2 - INPUT C
	 3 - INPUT D
	 4 - INPUT E
	 5 - INPUT F
	*/
#define CT_STAGE5_CFG_CIN_CON_POS_CDC			5

/*! Bypass PGA gain[23].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE5_CFG_PGA_BYPASS					1

/*! Disable the excitation to execute a noise reduction algorithm[22].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE5_CFG_NOISE_REDUCTION_ENABLE		0

/*! PGA gain ig PGA Bypass is equal to 0[21:20].
	 0 - 2
	 1 - 4
	 2 - 8
	 3 - 16
	*/
#define CT_STAGE5_CFG_PGA_GAIN					0

/*! C2V Range[19:16].
	 0-15
	 Range = (0.583(1+LSB))pF
	*/
#define CT_STAGE5_CFG_C2V_RANGE					4

/*! Enable CAP DAC[15].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE5_CFG_CAPDAC_ENABLE				1

/*! Enable Peak-to-peak measurements[13].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE5_CFG_PK2PK_ENABLE				0

/*! MAIN DAC Value[9:3].
	 0-127
	 Range = (388.5(1+LSB))fF
	*/
#define CT_STAGE5_CFG_MAIN_DAC					0

/*! SUB DAC Value[2:0].
	 0-7
	 Range = (48.5(1+LSB))fF
	*/
#define CT_STAGE5_CFG_SUB_DAC					0

/* STAGE 6 CONFIGURATION */

/*! Enable the CDC excitation[27].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE6_CFG_CIN_CON_POS_CDC_EN		0
/*! Select the CDC input[26:24].
	 0 - INPUT A
	 1 - INPUT B
	 2 - INPUT C
	 3 - INPUT D
	 4 - INPUT E
	 5 - INPUT F
	*/
#define CT_STAGE6_CFG_CIN_CON_POS_CDC			0

/*! Bypass PGA gain[23].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE6_CFG_PGA_BYPASS					1

/*! Disable the excitation to execute a noise reduction algorithm[22].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE6_CFG_NOISE_REDUCTION_ENABLE		0

/*! PGA gain ig PGA Bypass is equal to 0[21:20].
	 0 - 2
	 1 - 4
	 2 - 8
	 3 - 16
	*/
#define CT_STAGE6_CFG_PGA_GAIN					0

/*! C2V Range[19:16].
	 0-15
	 Range = (0.583(1+LSB))pF
	*/
#define CT_STAGE6_CFG_C2V_RANGE					4

/*! Enable CAP DAC[15].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE6_CFG_CAPDAC_ENABLE				1

/*! Enable Peak-to-peak measurements[13].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE6_CFG_PK2PK_ENABLE				0

/*! MAIN DAC Value[9:3].
	 0-127
	 Range = (388.5(1+LSB))fF
	*/
#define CT_STAGE6_CFG_MAIN_DAC					0

/*! SUB DAC Value[2:0].
	 0-7
	 Range = (48.5(1+LSB))fF
	*/
#define CT_STAGE6_CFG_SUB_DAC					0

/* STAGE 7 CONFIGURATION */

/*! Enable the CDC excitation[27].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE7_CFG_CIN_CON_POS_CDC_EN		0
/*! Select the CDC input[26:24].
	 0 - INPUT A
	 1 - INPUT B
	 2 - INPUT C
	 3 - INPUT D
	 4 - INPUT E
	 5 - INPUT F
	*/
#define CT_STAGE7_CFG_CIN_CON_POS_CDC			0

/*! Bypass PGA gain[23].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE7_CFG_PGA_BYPASS					1

/*! Disable the excitation to execute a noise reduction algorithm[22].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE7_CFG_NOISE_REDUCTION_ENABLE		0

/*! PGA gain ig PGA Bypass is equal to 0[21:20].
	 0 - 2
	 1 - 4
	 2 - 8
	 3 - 16
	*/
#define CT_STAGE7_CFG_PGA_GAIN					0

/*! C2V Range[19:16].
	 0-15
	 Range = (0.583(1+LSB))pF
	*/
#define CT_STAGE7_CFG_C2V_RANGE					4

/*! Enable CAP DAC[15].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE7_CFG_CAPDAC_ENABLE				1

/*! Enable Peak-to-peak measurements[13].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE7_CFG_PK2PK_ENABLE				0

/*! MAIN DAC Value[9:3].
	 0-127
	 Range = (388.5(1+LSB))fF
	*/
#define CT_STAGE7_CFG_MAIN_DAC					0

/*! SUB DAC Value[2:0].
	 0-7
	 Range = (48.5(1+LSB))fF
	*/
#define CT_STAGE7_CFG_SUB_DAC					0

/* STAGE 8 CONFIGURATION */

/*! Enable the CDC excitation[27].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE8_CFG_CIN_CON_POS_CDC_EN		0
/*! Select the CDC input[26:24].
	 0 - INPUT A
	 1 - INPUT B
	 2 - INPUT C
	 3 - INPUT D
	 4 - INPUT E
	 5 - INPUT F
	*/
#define CT_STAGE8_CFG_CIN_CON_POS_CDC			0

/*! Bypass PGA gain[23].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE8_CFG_PGA_BYPASS					1

/*! Disable the excitation to execute a noise reduction algorithm[22].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE8_CFG_NOISE_REDUCTION_ENABLE		0

/*! PGA gain ig PGA Bypass is equal to 0[21:20].
	 0 - 2
	 1 - 4
	 2 - 8
	 3 - 16
	*/
#define CT_STAGE8_CFG_PGA_GAIN					0

/*! C2V Range[19:16].
	 0-15
	 Range = (0.583(1+LSB))pF
	*/
#define CT_STAGE8_CFG_C2V_RANGE					4

/*! Enable CAP DAC[15].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE8_CFG_CAPDAC_ENABLE				1

/*! Enable Peak-to-peak measurements[13].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE8_CFG_PK2PK_ENABLE				0

/*! MAIN DAC Value[9:3].
	 0-127
	 Range = (388.5(1+LSB))fF
	*/
#define CT_STAGE8_CFG_MAIN_DAC					0

/*! SUB DAC Value[2:0].
	 0-7
	 Range = (48.5(1+LSB))fF
	*/
#define CT_STAGE8_CFG_SUB_DAC					0

/* STAGE 9 CONFIGURATION */

/*! Enable the CDC excitation[27].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE9_CFG_CIN_CON_POS_CDC_EN		0
/*! Select the CDC input[26:24].
	 0 - INPUT A
	 1 - INPUT B
	 2 - INPUT C
	 3 - INPUT D
	 4 - INPUT E
	 5 - INPUT F
	*/
#define CT_STAGE9_CFG_CIN_CON_POS_CDC			0

/*! Bypass PGA gain[23].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE9_CFG_PGA_BYPASS					1

/*! Disable the excitation to execute a noise reduction algorithm[22].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE9_CFG_NOISE_REDUCTION_ENABLE		0

/*! PGA gain ig PGA Bypass is equal to 0[21:20].
	 0 - 2
	 1 - 4
	 2 - 8
	 3 - 16
	*/
#define CT_STAGE9_CFG_PGA_GAIN					0

/*! C2V Range[19:16].
	 0-15
	 Range = (0.583(1+LSB))pF
	*/
#define CT_STAGE9_CFG_C2V_RANGE					4

/*! Enable CAP DAC[15].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE9_CFG_CAPDAC_ENABLE				1

/*! Enable Peak-to-peak measurements[13].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE9_CFG_PK2PK_ENABLE				0

/*! MAIN DAC Value[9:3].
	 0-127
	 Range = (388.5(1+LSB))fF
	*/
#define CT_STAGE9_CFG_MAIN_DAC					0

/*! SUB DAC Value[2:0].
	 0-7
	 Range = (48.5(1+LSB))fF
	*/
#define CT_STAGE9_CFG_SUB_DAC					0

/* STAGE 10 CONFIGURATION */

/*! Enable the CDC excitation[27].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE10_CFG_CIN_CON_POS_CDC_EN		0
/*! Select the CDC input[26:24].
	 0 - INPUT A
	 1 - INPUT B
	 2 - INPUT C
	 3 - INPUT D
	 4 - INPUT E
	 5 - INPUT F
	*/
#define CT_STAGE10_CFG_CIN_CON_POS_CDC			0

/*! Bypass PGA gain[23].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE10_CFG_PGA_BYPASS					1

/*! Disable the excitation to execute a noise reduction algorithm[22].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE10_CFG_NOISE_REDUCTION_ENABLE		0

/*! PGA gain ig PGA Bypass is equal to 0[21:20].
	 0 - 2
	 1 - 4
	 2 - 8
	 3 - 16
	*/
#define CT_STAGE10_CFG_PGA_GAIN					0

/*! C2V Range[19:16].
	 0-15
	 Range = (0.583(1+LSB))pF
	*/
#define CT_STAGE10_CFG_C2V_RANGE					4

/*! Enable CAP DAC[15].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE10_CFG_CAPDAC_ENABLE				1

/*! Enable Peak-to-peak measurements[13].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE10_CFG_PK2PK_ENABLE				0

/*! MAIN DAC Value[9:3].
	 0-127
	 Range = (388.5(1+LSB))fF
	*/
#define CT_STAGE10_CFG_MAIN_DAC					0

/*! SUB DAC Value[2:0].
	 0-7
	 Range = (48.5(1+LSB))fF
	*/
#define CT_STAGE10_CFG_SUB_DAC					0

/* STAGE 11 CONFIGURATION */

/*! Enable the CDC excitation[27].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE11_CFG_CIN_CON_POS_CDC_EN		0
/*! Select the CDC input[26:24].
	 0 - INPUT A
	 1 - INPUT B
	 2 - INPUT C
	 3 - INPUT D
	 4 - INPUT E
	 5 - INPUT F
	*/
#define CT_STAGE11_CFG_CIN_CON_POS_CDC			0

/*! Bypass PGA gain[23].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE11_CFG_PGA_BYPASS					1

/*! Disable the excitation to execute a noise reduction algorithm[22].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE11_CFG_NOISE_REDUCTION_ENABLE		0

/*! PGA gain ig PGA Bypass is equal to 0[21:20].
	 0 - 2
	 1 - 4
	 2 - 8
	 3 - 16
	*/
#define CT_STAGE11_CFG_PGA_GAIN					0

/*! C2V Range[19:16].
	 0-15
	 Range = (0.583(1+LSB))pF
	*/
#define CT_STAGE11_CFG_C2V_RANGE					4

/*! Enable CAP DAC[15].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE11_CFG_CAPDAC_ENABLE				1

/*! Enable Peak-to-peak measurements[13].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE11_CFG_PK2PK_ENABLE				0

/*! MAIN DAC Value[9:3].
	 0-127
	 Range = (388.5(1+LSB))fF
	*/
#define CT_STAGE11_CFG_MAIN_DAC					0

/*! SUB DAC Value[2:0].
	 0-7
	 Range = (48.5(1+LSB))fF
	*/
#define CT_STAGE11_CFG_SUB_DAC					0

/* STAGE 12 CONFIGURATION */

/*! Enable the CDC excitation[27].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE12_CFG_CIN_CON_POS_CDC_EN		0
/*! Select the CDC input[26:24].
	 0 - INPUT A
	 1 - INPUT B
	 2 - INPUT C
	 3 - INPUT D
	 4 - INPUT E
	 5 - INPUT F
	*/
#define CT_STAGE12_CFG_CIN_CON_POS_CDC			0

/*! Bypass PGA gain[23].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE12_CFG_PGA_BYPASS					1

/*! Disable the excitation to execute a noise reduction algorithm[22].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE12_CFG_NOISE_REDUCTION_ENABLE		0

/*! PGA gain ig PGA Bypass is equal to 0[21:20].
	 0 - 2
	 1 - 4
	 2 - 8
	 3 - 16
	*/
#define CT_STAGE12_CFG_PGA_GAIN					0

/*! C2V Range[19:16].
	 0-15
	 Range = (0.583(1+LSB))pF
	*/
#define CT_STAGE12_CFG_C2V_RANGE					4

/*! Enable CAP DAC[15].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE12_CFG_CAPDAC_ENABLE				1

/*! Enable Peak-to-peak measurements[13].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE12_CFG_PK2PK_ENABLE				0

/*! MAIN DAC Value[9:3].
	 0-127
	 Range = (388.5(1+LSB))fF
	*/
#define CT_STAGE12_CFG_MAIN_DAC					0

/*! SUB DAC Value[2:0].
	 0-7
	 Range = (48.5(1+LSB))fF
	*/
#define CT_STAGE12_CFG_SUB_DAC					0

/* STAGE 13 CONFIGURATION */

/*! Enable the CDC excitation[27].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE13_CFG_CIN_CON_POS_CDC_EN		0
/*! Select the CDC input[26:24].
	 0 - INPUT A
	 1 - INPUT B
	 2 - INPUT C
	 3 - INPUT D
	 4 - INPUT E
	 5 - INPUT F
	*/
#define CT_STAGE13_CFG_CIN_CON_POS_CDC			0

/*! Bypass PGA gain[23].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE13_CFG_PGA_BYPASS					1

/*! Disable the excitation to execute a noise reduction algorithm[22].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE13_CFG_NOISE_REDUCTION_ENABLE		0

/*! PGA gain ig PGA Bypass is equal to 0[21:20].
	 0 - 2
	 1 - 4
	 2 - 8
	 3 - 16
	*/
#define CT_STAGE13_CFG_PGA_GAIN					0

/*! C2V Range[19:16].
	 0-15
	 Range = (0.583(1+LSB))pF
	*/
#define CT_STAGE13_CFG_C2V_RANGE					4

/*! Enable CAP DAC[15].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE13_CFG_CAPDAC_ENABLE				1

/*! Enable Peak-to-peak measurements[13].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE13_CFG_PK2PK_ENABLE				0

/*! MAIN DAC Value[9:3].
	 0-127
	 Range = (388.5(1+LSB))fF
	*/
#define CT_STAGE13_CFG_MAIN_DAC					0

/*! SUB DAC Value[2:0].
	 0-7
	 Range = (48.5(1+LSB))fF
	*/
#define CT_STAGE13_CFG_SUB_DAC					0

/* STAGE 14 CONFIGURATION */

/*! Enable the CDC excitation[27].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE14_CFG_CIN_CON_POS_CDC_EN		0
/*! Select the CDC input[26:24].
	 0 - INPUT A
	 1 - INPUT B
	 2 - INPUT C
	 3 - INPUT D
	 4 - INPUT E
	 5 - INPUT F
	*/
#define CT_STAGE14_CFG_CIN_CON_POS_CDC			0

/*! Bypass PGA gain[23].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE14_CFG_PGA_BYPASS					1

/*! Disable the excitation to execute a noise reduction algorithm[22].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE14_CFG_NOISE_REDUCTION_ENABLE		0

/*! PGA gain ig PGA Bypass is equal to 0[21:20].
	 0 - 2
	 1 - 4
	 2 - 8
	 3 - 16
	*/
#define CT_STAGE14_CFG_PGA_GAIN					0

/*! C2V Range[19:16].
	 0-15
	 Range = (0.583(1+LSB))pF
	*/
#define CT_STAGE14_CFG_C2V_RANGE					4

/*! Enable CAP DAC[15].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE14_CFG_CAPDAC_ENABLE				1

/*! Enable Peak-to-peak measurements[13].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE14_CFG_PK2PK_ENABLE				0

/*! MAIN DAC Value[9:3].
	 0-127
	 Range = (388.5(1+LSB))fF
	*/
#define CT_STAGE14_CFG_MAIN_DAC					0

/*! SUB DAC Value[2:0].
	 0-7
	 Range = (48.5(1+LSB))fF
	*/
#define CT_STAGE14_CFG_SUB_DAC					0

/* STAGE 15 CONFIGURATION */

/*! Enable the CDC excitation[27].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE15_CFG_CIN_CON_POS_CDC_EN		0
/*! Select the CDC input[26:24].
	 0 - INPUT A
	 1 - INPUT B
	 2 - INPUT C
	 3 - INPUT D
	 4 - INPUT E
	 5 - INPUT F
	*/
#define CT_STAGE15_CFG_CIN_CON_POS_CDC			0

/*! Bypass PGA gain[23].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE15_CFG_PGA_BYPASS					1

/*! Disable the excitation to execute a noise reduction algorithm[22].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE15_CFG_NOISE_REDUCTION_ENABLE		0

/*! PGA gain ig PGA Bypass is equal to 0[21:20].
	 0 - 2
	 1 - 4
	 2 - 8
	 3 - 16
	*/
#define CT_STAGE15_CFG_PGA_GAIN					0

/*! C2V Range[19:16].
	 0-15
	 Range = (0.583(1+LSB))pF
	*/
#define CT_STAGE15_CFG_C2V_RANGE					4

/*! Enable CAP DAC[15].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE15_CFG_CAPDAC_ENABLE				1

/*! Enable Peak-to-peak measurements[13].
	 0 - Disable
	 1 - Enable
	*/
#define CT_STAGE15_CFG_PK2PK_ENABLE				0

/*! MAIN DAC Value[9:3].
	 0-127
	 Range = (388.5(1+LSB))fF
	*/
#define CT_STAGE15_CFG_MAIN_DAC					0

/*! SUB DAC Value[2:0].
	 0-7
	 Range = (48.5(1+LSB))fF
	*/
#define CT_STAGE15_CFG_SUB_DAC					0

/* SENSOR THRESHOLD CFG 0 */

/*! Stage 1 sensor threshold[29:16].
	 0-16383
	*/
#define CT_SENSOR_THR_CFG0_STAGE1			0

/*! Stage 0 sensor threshold[29:16].
	 0-16383
	*/
#define CT_SENSOR_THR_CFG0_STAGE0				0

/* SENSOR THRESHOLD CFG 1 */

/*! Stage 3 sensor threshold[29:16].
	 0-16383
	*/
#define CT_SENSOR_THR_CFG1_STAGE3			0

/*! Stage 2 sensor threshold[29:16].
	 0-16383
	*/
#define CT_SENSOR_THR_CFG1_STAGE2				0

/* SENSOR THRESHOLD CFG 2 */

/*! Stage 5 sensor threshold[29:16].
	 0-16383
	*/
#define CT_SENSOR_THR_CFG2_STAGE5			0

/*! Stage 4 sensor threshold[29:16].
	 0-16383
	*/
#define CT_SENSOR_THR_CFG2_STAGE4				0

/* SENSOR THRESHOLD CFG 3 */

/*! Stage 7 sensor threshold[29:16].
	 0-16383
	*/
#define CT_SENSOR_THR_CFG3_STAGE7			0

/*! Stage 6 sensor threshold[29:16].
	 0-16383
	*/
#define CT_SENSOR_THR_CFG3_STAGE6				0

/* SENSOR THRESHOLD CFG 4 */

/*! Stage 9 sensor threshold[29:16].
	 0-16383
	*/
#define CT_SENSOR_THR_CFG4_STAGE9			0

/*! Stage 8 sensor threshold[29:16].
	 0-16383
	*/
#define CT_SENSOR_THR_CFG4_STAGE8				0

/* SENSOR THRESHOLD CFG 5 */

/*! Stage 11 sensor threshold[29:16].
	 0-16383
	*/
#define CT_SENSOR_THR_CFG5_STAGE11			0

/*! Stage 10 sensor threshold[29:16].
	 0-16383
	*/
#define CT_SENSOR_THR_CFG5_STAGE10				0

/* SENSOR THRESHOLD CFG 6 */

/*! Stage 13 sensor threshold[29:16].
	 0-16383
	*/
#define CT_SENSOR_THR_CFG6_STAGE13			0

/*! Stage 12 sensor threshold[29:16].
	 0-16383
	*/
#define CT_SENSOR_THR_CFG6_STAGE12				0

/* SENSOR THRESHOLD CFG 7 */

/*! Stage 15 sensor threshold[29:16].
	 0-16383
	*/
#define CT_SENSOR_THR_CFG7_STAGE15			0

/*! Stage 14 sensor threshold[29:16].
	 0-16383
	*/
#define CT_SENSOR_THR_CFG7_STAGE14				0

/************** Macro validation *****************************/

#if ( CT_CDC_PWR_MODE_SHUTDOWN > 1 )
#error "Invalid configuration"
#endif

#if ( CT_CFG_1_INT_CYCLES > 4 )
#error "Invalid configuration"
#endif

#if ( CT_CFG_1_AIN_SEL > 3 )
#error "Invalid configuration"
#endif

#if ( CT_CFG_1_AUTOZERO_DUTY > 7 )
#error "Invalid configuration"
#endif

#if ( CT_CFG_1_BYPASS_GAIN > 1 )
#error "Invalid configuration"
#endif

#if ( CT_CFG_1_INT_BUFFER > 3 )
#error "Invalid configuration"
#endif

#if ( CT_CFG_1_PGA_BIAS > 7 )
#error "Invalid configuration"
#endif

#if ( CT_CFG_1_C2V_BIAS > 7 )
#error "Invalid configuration"
#endif


#if ( CT_CFG_2_C2V_LPF > 1 )
#error "Invalid configuration"
#endif

#if ( CT_CFG_2_SELFTIMER > 1 )
#error "Invalid configuration"
#endif

#if ( CT_CFG_2_C2V_HOLD_TIME > 31 )
#error "Invalid configuration"
#endif

#if ( CT_CFG_2_SELFTIMER_VALUE > 511 )
#error "Invalid configuration"
#endif

#if ( CT_CFG_2_PH24 > 255 )
#error "Invalid configuration"
#endif

#if ( CT_CFG_2_PH13 > 31 )
#error "Invalid configuration"
#endif

#if ( CT_CFG_3_PK2PK_AVG > 4 )
#error "Invalid configuration"
#endif

#if ( CT_CFG_3_PK2PK_AVG_MIN > 31 )
#error "Invalid configuration"
#endif

#if ( CT_CFG_3_PK2PK_SUBSET > 15 )
#error "Invalid configuration"
#endif

#if ( CT_CFG_3_PK2PK_NUM_SPLS > 4 )
#error "Invalid configuration"
#endif

#if ( CT_CFG_3_RES_SEL > 1 )
#error "Invalid configuration"
#endif

#if ( CT_CFG_3_DIV_CTOV_CAPTOUCH_EN > 63 )
#error "Invalid configuration"
#endif

#if ( CT_CFG_3_IIR_WEIGHT > 15 )
#error "Invalid configuration"
#endif

#if ( CT_CFG_3_IIR_ENABLE > 1 )
#error "Invalid configuration"
#endif

#if ( CT_MEAS_SEL_STAGE > 65535 )
#error "Invalid configuration"
#endif

#if ( CT_BASELINE_CAL_DELAY > 7 )
#error "Invalid configuration"
#endif
	
#if ( CT_BASELINE_FAST_FILTER_UPDATE > 7 )
#error "Invalid configuration"
#endif

#if ( CT_BASELINE_FAST_PROX > 255 )
#error "Invalid configuration"
#endif

#if ( CT_BASELINE_SLOW_PROX > 255 )
#error "Invalid configuration"
#endif

#if ( CT_BASELINE_BL_COEFF > 15 )
#error "Invalid configuration"
#endif

#if ( CT_BASELINE_CDC_COEFF > 15 )
#error "Invalid configuration"
#endif

#if ( CT_AVG_RANK > 4 )
#error "Invalid configuration"
#endif

#if ( CT_AVG_OUTPUT > 3 )
#error "Invalid configuration"
#endif

#if ( CT_AVG_SAMPLES_GROUP2 > 5 )
#error "Invalid configuration"
#endif

#if ( CT_AVG_SAMPLES_GROUP1 > 5 )
#error "Invalid configuration"
#endif

#if ( CT_AVG_MIN_GROUP2 > 31 )
#error "Invalid configuration"
#endif

#if ( CT_AVG_MIN_GROUP1 > 31 )
#error "Invalid configuration"
#endif

#if ( CT_CAL_EN_STAGE > 65535 )
#error "Invalid configuration"
#endif

#if ( CT_CAL_EN_STAGE > 65535 )
#error "Invalid configuration"
#endif

#if ( CT_TOUCH_CFG1_UPPER_THLD > 16383 )
#error "Invalid configuration"
#endif

#if ( CT_TOUCH_CFG1_LOWER_THLD > 16383 )
#error "Invalid configuration"
#endif

#if ( CT_TOUCH_CFG2_INDIVIDUAL_THRESHOLD_ENABLE > 1 )
#error "Invalid configuration"
#endif

#if ( CT_TOUCH_CFG2_EVENT_ON_HIGH > 1 )
#error "Invalid configuration"
#endif

#if ( CT_TOUCH_CFG2_SEL_INPA > 4 )
#error "Invalid configuration"
#endif

#if ( CT_TOUCH_CFG2_SEL_INPB > 4 )
#error "Invalid configuration"
#endif

#if ( CT_TOUCH_CFG2_DETECT_ENABLE > 1 )
#error "Invalid configuration"
#endif

#if ( CT_TOUCH_CFG2_MIN_DURATION > 1023 )
#error "Invalid configuration"
#endif


#if ( CT_REL_CFG1_UPPER_THLD > 16383 )
#error "Invalid configuration"
#endif	

#if ( CT_REL_CFG1_LOWER_THLD > 16383 )
#error "Invalid configuration"
#endif	

#if ( CT_REL_CFG2_INDIVIDUAL_THRESHOLD_ENABLE > 1 )
#error "Invalid configuration"
#endif	

#if ( CT_REL_CFG2_EVENT_ON_HIGH > 1 )
#error "Invalid configuration"
#endif	

#if ( CT_REL_CFG2_SEL_INPA > 4 )
#error "Invalid configuration"
#endif	

#if ( CT_REL_CFG2_SEL_INPB > 4 )
#error "Invalid configuration"
#endif	

#if ( CT_REL_CFG2_DETECT_ENABLE > 1 )
#error "Invalid configuration"
#endif	

#if ( CT_REL_CFG2_MIN_DURATION > 1023 )
#error "Invalid configuration"
#endif	

#if ( CT_IEN_RELEASE_ENABLE > 1 )
#error "Invalid configuration"
#endif	

#if ( CT_IEN_TOUCH_ENABLE > 1 )
#error "Invalid configuration"
#endif	

#if ( CT_IEN_PROX_ENABLE > 1 )
#error "Invalid configuration"
#endif


#if ( CT_IEN_THRESHOLD_ENABLE > 1 )
#error "Invalid configuration"
#endif	

#if ( CT_IEN_CONV_SET_COMPLETE_ENABLE > 1 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE0_CFG_CIN_CON_POS_CDC_EN > 1 )
#error "Invalid configuration"
#endif		
	
#if ( CT_STAGE0_CFG_CIN_CON_POS_CDC > 5 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE0_CFG_PGA_BYPASS > 1 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE0_CFG_NOISE_REDUCTION_ENABLE > 1 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE0_CFG_PGA_GAIN > 4 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE0_CFG_C2V_RANGE > 15 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE0_CFG_CAPDAC_ENABLE > 1)
#error "Invalid configuration"
#endif

#if ( CT_STAGE0_CFG_PK2PK_ENABLE > 1)
#error "Invalid configuration"
#endif

#if ( CT_STAGE0_CFG_MAIN_DAC > 127)
#error "Invalid configuration"
#endif

#if ( CT_STAGE0_CFG_SUB_DAC > 7)
#error "Invalid configuration"
#endif

#if ( CT_STAGE1_CFG_CIN_CON_POS_CDC_EN > 1 )
#error "Invalid configuration"
#endif		
	
#if ( CT_STAGE1_CFG_CIN_CON_POS_CDC > 5 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE1_CFG_PGA_BYPASS > 1 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE1_CFG_NOISE_REDUCTION_ENABLE > 1 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE1_CFG_PGA_GAIN > 4 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE1_CFG_C2V_RANGE > 15 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE1_CFG_CAPDAC_ENABLE > 1)
#error "Invalid configuration"
#endif

#if ( CT_STAGE1_CFG_PK2PK_ENABLE > 1)
#error "Invalid configuration"
#endif

#if ( CT_STAGE1_CFG_MAIN_DAC > 127)
#error "Invalid configuration"
#endif

#if ( CT_STAGE1_CFG_SUB_DAC > 7)
#error "Invalid configuration"
#endif

#if ( CT_STAGE2_CFG_CIN_CON_POS_CDC_EN > 1 )
#error "Invalid configuration"
#endif		
	
#if ( CT_STAGE2_CFG_CIN_CON_POS_CDC > 5 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE2_CFG_PGA_BYPASS > 1 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE2_CFG_NOISE_REDUCTION_ENABLE > 1 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE2_CFG_PGA_GAIN > 4 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE2_CFG_C2V_RANGE > 15 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE2_CFG_CAPDAC_ENABLE > 1)
#error "Invalid configuration"
#endif

#if ( CT_STAGE2_CFG_PK2PK_ENABLE > 1)
#error "Invalid configuration"
#endif

#if ( CT_STAGE2_CFG_MAIN_DAC > 127)
#error "Invalid configuration"
#endif

#if ( CT_STAGE2_CFG_SUB_DAC > 7)
#error "Invalid configuration"
#endif

#if ( CT_STAGE3_CFG_CIN_CON_POS_CDC_EN > 1 )
#error "Invalid configuration"
#endif		
	
#if ( CT_STAGE3_CFG_CIN_CON_POS_CDC > 5 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE3_CFG_PGA_BYPASS > 1 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE3_CFG_NOISE_REDUCTION_ENABLE > 1 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE3_CFG_PGA_GAIN > 4 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE3_CFG_C2V_RANGE > 15 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE3_CFG_CAPDAC_ENABLE > 1)
#error "Invalid configuration"
#endif

#if ( CT_STAGE3_CFG_PK2PK_ENABLE > 1)
#error "Invalid configuration"
#endif

#if ( CT_STAGE3_CFG_MAIN_DAC > 127)
#error "Invalid configuration"
#endif

#if ( CT_STAGE3_CFG_SUB_DAC > 7)
#error "Invalid configuration"
#endif

#if ( CT_STAGE4_CFG_CIN_CON_POS_CDC_EN > 1 )
#error "Invalid configuration"
#endif		
	
#if ( CT_STAGE4_CFG_CIN_CON_POS_CDC > 5 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE4_CFG_PGA_BYPASS > 1 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE4_CFG_NOISE_REDUCTION_ENABLE > 1 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE4_CFG_PGA_GAIN > 4 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE4_CFG_C2V_RANGE > 15 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE4_CFG_CAPDAC_ENABLE > 1)
#error "Invalid configuration"
#endif

#if ( CT_STAGE4_CFG_PK2PK_ENABLE > 1)
#error "Invalid configuration"
#endif

#if ( CT_STAGE4_CFG_MAIN_DAC > 127)
#error "Invalid configuration"
#endif

#if ( CT_STAGE4_CFG_SUB_DAC > 7)
#error "Invalid configuration"
#endif

#if ( CT_STAGE5_CFG_CIN_CON_POS_CDC_EN > 1 )
#error "Invalid configuration"
#endif		
	
#if ( CT_STAGE5_CFG_CIN_CON_POS_CDC > 5 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE5_CFG_PGA_BYPASS > 1 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE5_CFG_NOISE_REDUCTION_ENABLE > 1 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE5_CFG_PGA_GAIN > 4 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE5_CFG_C2V_RANGE > 15 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE5_CFG_CAPDAC_ENABLE > 1)
#error "Invalid configuration"
#endif

#if ( CT_STAGE5_CFG_PK2PK_ENABLE > 1)
#error "Invalid configuration"
#endif

#if ( CT_STAGE5_CFG_MAIN_DAC > 127)
#error "Invalid configuration"
#endif

#if ( CT_STAGE5_CFG_SUB_DAC > 7)
#error "Invalid configuration"
#endif

#if ( CT_STAGE6_CFG_CIN_CON_POS_CDC_EN > 1 )
#error "Invalid configuration"
#endif		
	
#if ( CT_STAGE6_CFG_CIN_CON_POS_CDC > 5 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE6_CFG_PGA_BYPASS > 1 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE6_CFG_NOISE_REDUCTION_ENABLE > 1 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE6_CFG_PGA_GAIN > 4 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE6_CFG_C2V_RANGE > 15 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE6_CFG_CAPDAC_ENABLE > 1)
#error "Invalid configuration"
#endif

#if ( CT_STAGE6_CFG_PK2PK_ENABLE > 1)
#error "Invalid configuration"
#endif

#if ( CT_STAGE6_CFG_MAIN_DAC > 127)
#error "Invalid configuration"
#endif

#if ( CT_STAGE6_CFG_SUB_DAC > 7)
#error "Invalid configuration"
#endif

#if ( CT_STAGE7_CFG_CIN_CON_POS_CDC_EN > 1 )
#error "Invalid configuration"
#endif		
	
#if ( CT_STAGE7_CFG_CIN_CON_POS_CDC > 5 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE7_CFG_PGA_BYPASS > 1 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE7_CFG_NOISE_REDUCTION_ENABLE > 1 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE7_CFG_PGA_GAIN > 4 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE7_CFG_C2V_RANGE > 15 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE7_CFG_CAPDAC_ENABLE > 1)
#error "Invalid configuration"
#endif

#if ( CT_STAGE7_CFG_PK2PK_ENABLE > 1)
#error "Invalid configuration"
#endif

#if ( CT_STAGE7_CFG_MAIN_DAC > 127)
#error "Invalid configuration"
#endif

#if ( CT_STAGE7_CFG_SUB_DAC > 7)
#error "Invalid configuration"
#endif

#if ( CT_STAGE8_CFG_CIN_CON_POS_CDC_EN > 1 )
#error "Invalid configuration"
#endif		
	
#if ( CT_STAGE8_CFG_CIN_CON_POS_CDC > 5 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE8_CFG_PGA_BYPASS > 1 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE8_CFG_NOISE_REDUCTION_ENABLE > 1 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE8_CFG_PGA_GAIN > 4 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE8_CFG_C2V_RANGE > 15 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE8_CFG_CAPDAC_ENABLE > 1)
#error "Invalid configuration"
#endif

#if ( CT_STAGE8_CFG_PK2PK_ENABLE > 1)
#error "Invalid configuration"
#endif

#if ( CT_STAGE8_CFG_MAIN_DAC > 127)
#error "Invalid configuration"
#endif

#if ( CT_STAGE8_CFG_SUB_DAC > 7)
#error "Invalid configuration"
#endif

#if ( CT_STAGE9_CFG_CIN_CON_POS_CDC_EN > 1 )
#error "Invalid configuration"
#endif		
	
#if ( CT_STAGE9_CFG_CIN_CON_POS_CDC > 5 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE9_CFG_PGA_BYPASS > 1 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE9_CFG_NOISE_REDUCTION_ENABLE > 1 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE9_CFG_PGA_GAIN > 4 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE9_CFG_C2V_RANGE > 15 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE9_CFG_CAPDAC_ENABLE > 1)
#error "Invalid configuration"
#endif

#if ( CT_STAGE9_CFG_PK2PK_ENABLE > 1)
#error "Invalid configuration"
#endif

#if ( CT_STAGE9_CFG_MAIN_DAC > 127)
#error "Invalid configuration"
#endif

#if ( CT_STAGE9_CFG_SUB_DAC > 7)
#error "Invalid configuration"
#endif

#if ( CT_STAGE10_CFG_CIN_CON_POS_CDC_EN > 1 )
#error "Invalid configuration"
#endif		
	
#if ( CT_STAGE10_CFG_CIN_CON_POS_CDC > 5 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE10_CFG_PGA_BYPASS > 1 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE10_CFG_NOISE_REDUCTION_ENABLE > 1 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE10_CFG_PGA_GAIN > 4 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE10_CFG_C2V_RANGE > 15 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE10_CFG_CAPDAC_ENABLE > 1)
#error "Invalid configuration"
#endif

#if ( CT_STAGE10_CFG_PK2PK_ENABLE > 1)
#error "Invalid configuration"
#endif

#if ( CT_STAGE10_CFG_MAIN_DAC > 127)
#error "Invalid configuration"
#endif

#if ( CT_STAGE10_CFG_SUB_DAC > 7)
#error "Invalid configuration"
#endif

#if ( CT_STAGE11_CFG_CIN_CON_POS_CDC_EN > 1 )
#error "Invalid configuration"
#endif		
	
#if ( CT_STAGE11_CFG_CIN_CON_POS_CDC > 5 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE11_CFG_PGA_BYPASS > 1 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE11_CFG_NOISE_REDUCTION_ENABLE > 1 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE11_CFG_PGA_GAIN > 4 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE11_CFG_C2V_RANGE > 15 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE11_CFG_CAPDAC_ENABLE > 1)
#error "Invalid configuration"
#endif

#if ( CT_STAGE11_CFG_PK2PK_ENABLE > 1)
#error "Invalid configuration"
#endif

#if ( CT_STAGE11_CFG_MAIN_DAC > 127)
#error "Invalid configuration"
#endif

#if ( CT_STAGE11_CFG_SUB_DAC > 7)
#error "Invalid configuration"
#endif

#if ( CT_STAGE12_CFG_CIN_CON_POS_CDC_EN > 1 )
#error "Invalid configuration"
#endif		
	
#if ( CT_STAGE12_CFG_CIN_CON_POS_CDC > 5 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE12_CFG_PGA_BYPASS > 1 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE12_CFG_NOISE_REDUCTION_ENABLE > 1 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE12_CFG_PGA_GAIN > 4 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE12_CFG_C2V_RANGE > 15 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE12_CFG_CAPDAC_ENABLE > 1)
#error "Invalid configuration"
#endif

#if ( CT_STAGE12_CFG_PK2PK_ENABLE > 1)
#error "Invalid configuration"
#endif

#if ( CT_STAGE12_CFG_MAIN_DAC > 127)
#error "Invalid configuration"
#endif

#if ( CT_STAGE12_CFG_SUB_DAC > 7)
#error "Invalid configuration"
#endif

#if ( CT_STAGE13_CFG_CIN_CON_POS_CDC_EN > 1 )
#error "Invalid configuration"
#endif		
	
#if ( CT_STAGE13_CFG_CIN_CON_POS_CDC > 5 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE13_CFG_PGA_BYPASS > 1 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE13_CFG_NOISE_REDUCTION_ENABLE > 1 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE13_CFG_PGA_GAIN > 4 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE13_CFG_C2V_RANGE > 15 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE13_CFG_CAPDAC_ENABLE > 1)
#error "Invalid configuration"
#endif

#if ( CT_STAGE13_CFG_PK2PK_ENABLE > 1)
#error "Invalid configuration"
#endif

#if ( CT_STAGE13_CFG_MAIN_DAC > 127)
#error "Invalid configuration"
#endif

#if ( CT_STAGE13_CFG_SUB_DAC > 7)
#error "Invalid configuration"
#endif

#if ( CT_STAGE14_CFG_CIN_CON_POS_CDC_EN > 1 )
#error "Invalid configuration"
#endif		
	
#if ( CT_STAGE14_CFG_CIN_CON_POS_CDC > 5 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE14_CFG_PGA_BYPASS > 1 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE14_CFG_NOISE_REDUCTION_ENABLE > 1 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE14_CFG_PGA_GAIN > 4 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE14_CFG_C2V_RANGE > 15 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE14_CFG_CAPDAC_ENABLE > 1)
#error "Invalid configuration"
#endif

#if ( CT_STAGE14_CFG_PK2PK_ENABLE > 1)
#error "Invalid configuration"
#endif

#if ( CT_STAGE14_CFG_MAIN_DAC > 127)
#error "Invalid configuration"
#endif

#if ( CT_STAGE14_CFG_SUB_DAC > 7)
#error "Invalid configuration"
#endif

#if ( CT_STAGE15_CFG_CIN_CON_POS_CDC_EN > 1 )
#error "Invalid configuration"
#endif		
	
#if ( CT_STAGE15_CFG_CIN_CON_POS_CDC > 5 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE15_CFG_PGA_BYPASS > 1 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE15_CFG_NOISE_REDUCTION_ENABLE > 1 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE15_CFG_PGA_GAIN > 4 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE15_CFG_C2V_RANGE > 15 )
#error "Invalid configuration"
#endif	

#if ( CT_STAGE15_CFG_CAPDAC_ENABLE > 1)
#error "Invalid configuration"
#endif

#if ( CT_STAGE15_CFG_PK2PK_ENABLE > 1)
#error "Invalid configuration"
#endif

#if ( CT_STAGE15_CFG_MAIN_DAC > 127)
#error "Invalid configuration"
#endif

#if ( CT_STAGE15_CFG_SUB_DAC > 7)
#error "Invalid configuration"
#endif


#if ( CT_SENSOR_THR_CFG0_STAGE0 > 16383)
#error "Invalid configuration"
#endif

#if ( CT_SENSOR_THR_CFG0_STAGE1 > 16383)
#error "Invalid configuration"
#endif

#if ( CT_SENSOR_THR_CFG1_STAGE2 > 16383)
#error "Invalid configuration"
#endif

#if ( CT_SENSOR_THR_CFG1_STAGE3 > 16383)
#error "Invalid configuration"
#endif

#if ( CT_SENSOR_THR_CFG2_STAGE4 > 16383)
#error "Invalid configuration"
#endif

#if ( CT_SENSOR_THR_CFG2_STAGE5 > 16383)
#error "Invalid configuration"
#endif

#if ( CT_SENSOR_THR_CFG3_STAGE6 > 16383)
#error "Invalid configuration"
#endif

#if ( CT_SENSOR_THR_CFG3_STAGE7 > 16383)
#error "Invalid configuration"
#endif

#if ( CT_SENSOR_THR_CFG4_STAGE8 > 16383)
#error "Invalid configuration"
#endif

#if ( CT_SENSOR_THR_CFG4_STAGE9 > 16383)
#error "Invalid configuration"
#endif

#if ( CT_SENSOR_THR_CFG5_STAGE10 > 16383)
#error "Invalid configuration"
#endif

#if ( CT_SENSOR_THR_CFG5_STAGE11 > 16383)
#error "Invalid configuration"
#endif

#if ( CT_SENSOR_THR_CFG6_STAGE12 > 16383)
#error "Invalid configuration"
#endif

#if ( CT_SENSOR_THR_CFG6_STAGE13 > 16383)
#error "Invalid configuration"
#endif

#if ( CT_SENSOR_THR_CFG7_STAGE14 > 16383)
#error "Invalid configuration"
#endif

#if ( CT_SENSOR_THR_CFG7_STAGE15 > 16383)
#error "Invalid configuration"
#endif


#endif /* __ADI_CT_CONIG_H__ */
