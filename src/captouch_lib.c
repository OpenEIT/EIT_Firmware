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


/*! \addtogroup CT_Library
 *  @{
 */


#define ASSERT_ADI_MISRA_SUPPRESSIONS  /*!< Apply ADI MISRA Suppressions */
#include "misra.h"

#include <stddef.h>                /* for NULL */
#include <string.h>                /* for memcpy */
#include <stdlib.h>
#include "test_common.h"  // test
#include "captouch.h"
#include "captouch_lib.h"
#include "gpio.h"      /* GPIO configuration */



/*!
* @brief       Adjust automatically the CAPDAC value to set the CDC values in mid-scale
*
* @param[in]   hDevice             Device handle obtained from adi_AFE_Init().
* @param[in]   StageNumber         Stage to Adjust
* @param[in]   BaselineValue       Baseline Value
*
* @return      Status
*              - #ADI_CT_RESULT_SUCCESS                       Call completed successfully.
*
* @details     The function sets the baseline in mid-scale adjusting automatically
*              the CAPDAC value for each stage. The function is based on a SAR algorithm
*
*/



ADI_CT_RESULT_TYPE adi_CT_Stage_AutoCAPDAC_Adjustment(ADI_CT_DEV_HANDLE const hDevice,ADI_CT_STAGE_NUMBER_TYPE StageNumber,uint16_t BaselineValue){
  
  
  /* Stage Configuration Address Pointer */
  volatile uint32_t *StageConfigurationPtr = NULL;
  
  
   /* VARIABLES TO BE USED IN THIS CODE*/
  
  
  /* To save the Stage Configuration */
  uint32_t StageConfiguration;
  
   //new test
   uint16_t bestdifference = 0xFFFF;
   uint16_t newdifference = 0;
  uint32_t StageConfigurationBestCfg;
  
  /* Lower and Upper Limits for the Baseline Desired Value */
  uint16_t LowBaselineValue = BaselineValue - DEFAULT_BASELINE_LIMITS;
  uint16_t UpBaselineValue  = BaselineValue + DEFAULT_BASELINE_LIMITS;
  
  /* CDC Stage Value */
  uint16_t CDCStageValue;
    
  /* Notify that the Baseline Value is set */
  bool_t StageFinish=0;
  
  /* Initialize the Stage Addres Pointer */
  StageConfigurationPtr = &pADI_CT->CT_STAGE0_CFG + StageNumber;
  
  /***************************************************************
  It is necessary change some specific parameters to make the adjust
  correctly. So, the system stores these parameters values to
  reconfigure the Captouch once the CAPDAC adjust will be done
  ***************************************************************/
  
  /* To store the Previous Captouch Configuration */
  uint8_t PreviousCaptouchInterrupts = pADI_CT->CT_IEN;
  
  /* To store the Previous IIR Filter Status */
  bool_t PreviousFilterStatus;
  
  /* To store the Previous IIR Filter Status */
  bool_t PreviousSelfTimerStatus;
  
  /* To store the Previous Touch And Release Detection Status */
  bool_t PreviousTouchAndReleaseDetectionStatus;
  
  /* To store the Previous Power Status */
  bool_t PreviousPowerStatus;
    
  /* Store Enabled Interrupts  */
  adi_CT_GetInterruptsEnable(hDevice,&PreviousCaptouchInterrupts);
  
  /* Store the IIR Filter Status  */
  adi_CT_GetIIRFilterEnable(hDevice,&PreviousFilterStatus);
  
  /* Store the Self Timer Status  */
  adi_CT_GetSelfTimerEnable(hDevice,&PreviousSelfTimerStatus);
  
  /* Store the Touch And Release Detection Status  */
  adi_CT_GetTouchEnable(hDevice,&PreviousTouchAndReleaseDetectionStatus);
  
  /* Store the Touch And Release Detection Status  */
  adi_CT_GetPowerStatus(hDevice,&PreviousPowerStatus);
  
  /* Disable the IIR FIlter  */
  adi_CT_SetIIRFilterEnable(hDevice,false);
  
  /* Disable the Self Timer  */
  adi_CT_SetSelfTimerEnable(hDevice,false);
  
  /* Disable the Touch And Release Detection  */
  adi_CT_SetTouchEnable(hDevice,false);
  adi_CT_SetReleaseEnable(hDevice,false);
  
  /* Disable the Captouch Interrupts  */
  adi_CT_SetInterruptsEnable(hDevice,PreviousCaptouchInterrupts,false);
  
  /* Reset the Main DAC and SUB DAC value */
  *StageConfigurationPtr &= ~(BITM_CT_CT_STAGE1_CFG_MAIN_DAC | BITM_CT_CT_STAGE1_CFG_SUB_DAC);
  
  //  *StageConfigurationPtr |= 0x04;
  
  /* Enable the Conversion Complete Interrupt */
  adi_CT_SetInterruptsEnable(hDevice,ADI_CT_INTERRUPT_CONV_SET_COMPLETE,true);
  
  
  /* Power Up the Captouch  */
  adi_CT_SetPowerStatus(hDevice,true);
  
  
  /* For loop with 10 iterations (the same than the number of CAPDAC bits) */
  uint8_t i = 10;
  
  /* Conditions to finish the loop. 10 iterations or Finish the CAPDAC Adjust */
  while((i >= 1) && (StageFinish == 0))
    
  {
      /* Stores the current Stage Configuration */
      StageConfiguration = *StageConfigurationPtr;
    
      /* Change the CAPDAC value setting the next bit to 1 */
      *StageConfigurationPtr |= (1 << i-1);
    
      /* Start a single conversion sequence and wait until it finishes (detected by CONV_SET_COMPLETE Interrupt) */
      adi_CT_SetStartSingleSequence(hDevice);
    
      while (pADI_CT->CT_CDC_PWR & BITM_CT_CT_CDC_PWR_SW_STATUS);
    
      /* Read the CDC Value */
      adi_CT_GetCDCValue(hDevice,(ADI_CT_STAGE_NUMBER_TYPE)StageNumber,&CDCStageValue);
    
      /* Check if the current CDC Value in between the limits  */
    if(CDCStageValue <= UpBaselineValue && CDCStageValue >= LowBaselineValue)
      {
        /* Adjust Finished  */
        StageFinish = 1;
      StageConfigurationBestCfg = *StageConfigurationPtr;      // Store the current configuration as the best
      }
    
      else if(CDCStageValue > UpBaselineValue)
      {
      
      /* Calculate the new difference */
      newdifference = CDCStageValue - UpBaselineValue;
      if(newdifference < bestdifference)
      {
        StageConfigurationBestCfg = *StageConfigurationPtr;
        bestdifference = newdifference;
      }
      /* As the value is greater than the limit, the system doesn't store the last bit added */
        *StageConfigurationPtr = StageConfiguration;
      
      }
    
	  else //(CDCStageValue < LowBaselineValue)
	  {
      newdifference = LowBaselineValue-CDCStageValue;
      if(newdifference < bestdifference)
	  {
        StageConfigurationBestCfg = *StageConfigurationPtr;
        bestdifference = newdifference;
	  }
  }
    
    /* Decrement the counter variable */
    i--;
   
    
  }
  
  *StageConfigurationPtr = StageConfigurationBestCfg;
  
  /* Once the CAPDAC is adjusted the system has to restablish the previous values */
        
  /* Set the Touch And Release Previous Status */
  adi_CT_SetTouchEnable(hDevice,PreviousTouchAndReleaseDetectionStatus);
  adi_CT_SetReleaseEnable(hDevice,PreviousTouchAndReleaseDetectionStatus);
  
  /* Set the Touch And Release Previous Status */
  adi_CT_SetIIRFilterEnable(hDevice,PreviousFilterStatus);
  
  /* Set the Previous Self Timer Status */
  adi_CT_SetSelfTimerEnable(hDevice,PreviousSelfTimerStatus);
  
  /* Disable the CON_SET_COMPLETE interrupt */
  adi_CT_SetInterruptsEnable(hDevice,ADI_CT_INTERRUPT_CONV_SET_COMPLETE,false);
  
  /* Enable the Previous Captouch Interrupt */
  adi_CT_SetInterruptsEnable(hDevice,PreviousCaptouchInterrupts,true);
  
  /* Set the Power Previous Status */
  adi_CT_SetPowerStatus(hDevice,PreviousPowerStatus);
  
  /* First of all, the systems forces an autocalibration */
  adi_CT_SetAutoForceBaselineCal(hDevice);
  
  return ADI_CT_RESULT_SUCCESS;
  
}

/*!
 * @brief       Automatic Settling Time Adjustment
 *
 * @param[in]   hDevice     		Device handle obtained from adi_CT_Init().
 * @param[in]   StageNumber         Stage to Adjust
 * @param[in]   NoiseLevel          Noise Level
 *
 * @param[out]  Phase24Result  Phase24 result.
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                        Call completed successfully.
 *
 * @details     The function calculates the Phase13 and Phase24 values to minimize
 *              the noise
 *
 */

ADI_CT_RESULT_TYPE adi_CT_FrequencyHoppingAlgorithm(ADI_CT_DEV_HANDLE const hDevice,ADI_CT_STAGE_NUMBER_TYPE StageNumber,uint16_t NoiseLevel,uint8_t *Phase24Result)
{

   #define M 10

  ADI_CT_C2V_TIMING_TYPE TimingSettings;

  #define MIN_PHASE24 0x10 // 1us
  #define MAX_PHASE24 0x40 // 8us

  uint32_t MeasuredNoise;

  uint32_t NoiseFloor = 0;
  uint32_t MaxNoiseValue = 0;
  uint32_t MinNoiseValue = 0xFFFFFFFF;

  bool_t minimum = false;

  uint8_t CurrentPhase = MIN_PHASE24;
  uint8_t MinNoisePhase;


  /* get the current timing settings*/
  adi_CT_GetTimingsCfg(hDevice,&TimingSettings);
  TimingSettings.Phase24 = CurrentPhase;



  /***************************************************************
  It is necessary change some specific parameters to make the adjust
  correctly. So, the system stores these parameters values to
  reconfigure the Captouch once the CAPDAC adjust will be done
  ***************************************************************/


  minimum = false;
  while(minimum == false && CurrentPhase < MAX_PHASE24)

  {

      /* Measure the Noise*/
    adi_CT_NoiseMeasurementPK2PK(hDevice,StageNumber,&MeasuredNoise,5);


      /* Compare the current noise value with the "noise floor"*/
      if(MeasuredNoise > NoiseFloor)
      {
        /* define a new noise floor */
        NoiseFloor = MeasuredNoise;
      }

      if (MeasuredNoise > MaxNoiseValue)
      {
        /* A noiser phase has been found, keep record of it*/
        MaxNoiseValue = MeasuredNoise;
      }

      /* Store the first phase time as the best one in order not to miss a
	notch at the initial phase time */
      if(CurrentPhase == MIN_PHASE24)
      {
        MinNoiseValue = MeasuredNoise;
        MinNoisePhase  = CurrentPhase;
      }
      else
      {
        /* compare the current noise with the minimum*/
        if(MeasuredNoise < MinNoiseValue)
        {
          /* A less noisy phase time has been found, meaning that we are going into a local minimum */
            MinNoiseValue = MeasuredNoise;
            MinNoisePhase = CurrentPhase;
        }
        /////////////////////////
        else
        {
          /* See if the possible local minimum is a proper local minimum */
        /*  if( ((NoiseFloor - MinNoiseValue) >= LocalMinimumThreshold) && ((MeasuredNoise - MinNoiseValue) >= LocalMinimumThreshold))
          {
               CurrentPhase = MinNoisePhase;
               minimum = true;
          }*/

        }
        ////////////////////////////////
      }

      if(MinNoiseValue <= NoiseLevel)
      {
        minimum = true;
      }

      if( minimum == false)
      {
        /* Increment the phase time */
        if(CurrentPhase++ >=  MAX_PHASE24)
        {
          /* if the phase is bigger than the MAX_PHASE24 the algorithm stops*/
          TimingSettings.Phase24 = MinNoisePhase;
        }
        else
        {
          /* Save the current Phase*/
          TimingSettings.Phase24 = CurrentPhase;
        }
      }

      adi_CT_SetTimingsCfg(hDevice,&TimingSettings);

  }

  *Phase24Result = MinNoisePhase;
  TimingSettings.Phase24 = MinNoisePhase;
  adi_CT_SetTimingsCfg(hDevice,&TimingSettings);



  return ADI_CT_RESULT_SUCCESS;
}


/*!
 * @brief       Function to measure the variance noise value
 *
 * @param[in]   hDevice     		Device handle obtained from adi_CT_Init().
 * @param[in]   StageNumber         Stage to Adjust
 * @param[in]   MeasureNumber       Number of measurements to calculate the variance.
 *
 * @param[out]  variance			Noise value.
 *
 * @return      Status
 *              - #ADI_CT_RESULT_SUCCESS                        Call completed successfully.
 *
 * @details     The function measures the noise and calculates the variance with a MeasureNumber noise measurements.
 *
 */


ADI_CT_RESULT_TYPE adi_CT_NoiseMeasurement(ADI_CT_DEV_HANDLE const hDevice,ADI_CT_STAGE_NUMBER_TYPE StageNumber,uint32_t *variance,uint8_t MeasureNumber)

{

  #define MEASURE_NUMBER 8
  #define FIRST_NOISE_STAGE 6


  ADI_STAGE_TYPE StageCfgType;
  ADI_CT_INPUT_TYPE CDCInput;

  uint16_t NoiseStages  = ADI_CT_STAGE6_BIT | ADI_CT_STAGE7_BIT | ADI_CT_STAGE8_BIT | ADI_CT_STAGE9_BIT | ADI_CT_STAGE10_BIT | ADI_CT_STAGE11_BIT | ADI_CT_STAGE12_BIT | ADI_CT_STAGE13_BIT;
  uint16_t SignalStages = ADI_CT_STAGE0_BIT | ADI_CT_STAGE1_BIT | ADI_CT_STAGE2_BIT | ADI_CT_STAGE3_BIT | ADI_CT_STAGE4_BIT | ADI_CT_STAGE5_BIT;

  uint16_t NoiseValues[MEASURE_NUMBER];
  uint32_t mean;
  uint16_t den = 0;
  uint32_t variancevalue = 0;

  uint8_t measure_counter = 0;

    /*
  Disable any IIR, Ranking, CAPDAC configuration
  */


    /* To store the Previous IIR Filter Status */
  bool_t PreviousFilterStatus;

    /* Store the IIR Filter Status  */
  adi_CT_GetIIRFilterEnable(hDevice,&PreviousFilterStatus);


  ADI_CT_AVERAGE_TYPE PreviousAverageSettings;
  ADI_CT_AVERAGE_TYPE AverageSettings;

  /* Average Settings */
  adi_CT_GetAverageCfg(hDevice,&PreviousAverageSettings);

  AverageSettings = PreviousAverageSettings;

  AverageSettings.RankFilt = ADI_CT_RANKING_NO_RANKING;

  /* Average Settings */
  adi_CT_SetAverageCfg(hDevice,&AverageSettings);

      /* To store the Previous Captouch Configuration */
  uint8_t PreviousCaptouchInterrupts = pADI_CT->CT_IEN;

    /* To store the Previous IIR Filter Status */
  bool_t PreviousSelfTimerStatus;

   /* Store the Self Timer Status   */
  adi_CT_GetSelfTimerEnable(hDevice,&PreviousSelfTimerStatus);


   /* Disable IIR Filter */
  adi_CT_SetIIRFilterEnable(hDevice,false);

  /* Get the Stage configuration to configure the stages to measure the noise using the same configuration */
  adi_CT_GetStageGenericCfg(hDevice,StageNumber,&CDCInput,&StageCfgType);

 /* Enable the noise measurement stages and disable the other stages.*/
  adi_CT_SetStagesEnable(hDevice,SignalStages,true);
  adi_CT_SetStagesEnable(hDevice,NoiseStages,true);

    for(uint8_t i = FIRST_NOISE_STAGE ; i < FIRST_NOISE_STAGE + MEASURE_NUMBER ; i++)
  {

    /* Configure the Stage */
    adi_CT_SetStageGenericCfg(hDevice,(ADI_CT_STAGE_NUMBER_TYPE)(i),CDCInput,&StageCfgType);

    adi_CT_SetStageCAPDACEnable(hDevice,(ADI_CT_STAGE_NUMBER_TYPE)(i),false);

    /* Enable the Frequency Hopping Algorithm */
    adi_CT_SetStageFreqHoppingEnable(hDevice,(ADI_CT_STAGE_NUMBER_TYPE)(i),true);

  }

      /* Enable the Conversion Complete Interrupt */
  adi_CT_SetInterruptsEnable(hDevice,ADI_CT_INTERRUPT_CONV_SET_COMPLETE,true);

     /* Power Up the Captouch  */
  adi_CT_SetPowerStatus(hDevice,true);

  while(measure_counter < MeasureNumber)

  {

    /* Start a single conversion sequence and wait until it finishes (detected by CONV_SET_COMPLETE Interrupt) */
  adi_CT_SetStartSingleSequence(hDevice);

  while (pADI_CT->CT_CDC_PWR & BITM_CT_CT_CDC_PWR_SW_STATUS);


    /* Read the CDC Value and calculate the mean value*/
    mean = 0;
    for(uint8_t i = 0; i < MEASURE_NUMBER ; i++)
    {
      adi_CT_GetCDCValue(hDevice,(ADI_CT_STAGE_NUMBER_TYPE)(FIRST_NOISE_STAGE + i),&NoiseValues[i]);
      mean += NoiseValues[i];
    }
    mean = mean >> 3;


      /* variance */
      variancevalue = 0;
      for(uint8_t n = 0; n < MEASURE_NUMBER; n++)
      {
        den = abs(NoiseValues[n] - mean);
        variancevalue += (den * den);
      }
      variancevalue = variancevalue >> 3;

      if(measure_counter == 0)

      {
        *variance = variancevalue;
      }

      else

      {
        *variance = (*variance + variancevalue);
      }

      measure_counter++;


  }

  *variance = *variance/MeasureNumber;

    /* Average Settings */
  adi_CT_SetAverageCfg(hDevice,&PreviousAverageSettings);


      /* Enable the Conversion Complete Interrupt */
  adi_CT_SetInterruptsEnable(hDevice,ADI_CT_INTERRUPT_CONV_SET_COMPLETE,false);

  /* Enable the Conversion Complete Interrupt */
  adi_CT_SetInterruptsEnable(hDevice,PreviousCaptouchInterrupts,true);

  


      /* Store the IIR Filter Status  */
  adi_CT_SetIIRFilterEnable(hDevice,PreviousFilterStatus);

      /* Set the Previous Self Timer Status */
  adi_CT_SetSelfTimerEnable(hDevice,PreviousSelfTimerStatus);

    adi_CT_SetStagesEnable(hDevice,SignalStages,true);
    adi_CT_SetStagesEnable(hDevice,NoiseStages,false);

  return ADI_CT_RESULT_SUCCESS;

}



/* C++ linkage */
#ifdef __cplusplus
}


#endif /* include guard */

/*
** EOF
*/

/*@}*/






