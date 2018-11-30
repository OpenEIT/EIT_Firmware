/*! \addtogroup AFE_Library AFE Library
 *  @{
 */

#define ASSERT_ADI_MISRA_SUPPRESSIONS  /*!< Apply ADI MISRA Suppressions */
#include "misra.h"

#include <stdlib.h>  /* for 'NULL" definition */

#include "afe.h"
#include "afe_lib.h"
#include "afe_sequences.h"
#include "uart.h"

/*!
 * @brief       Power up the AFE
 *
 * @param[in]   hDevice                                 Device handle obtained from adi_AFE_Init().
 *
 * @return      Status
 *              - #ADI_AFE_SUCCESS                      Call completed successfully.
 *              - #ADI_AFE_ERR_BAD_DEV_HANDLE           Invalid device handle.
 *              - #ADI_AFE_ERR_NOT_INITIALIZED          Device not initialized.
 *              - #ADI_AFE_ERR_ACLKOFF                  ACLK disabled from the clock gate.
 *              - #ADI_AFE_ERR_WRONG_ACLK_FREQUENCY     Programmed ACLK frequency is not the required 16MHz.
 *
 * @details     The function powers up the AFE using the recommended timing sequence.
 *
 * @details     It is an implementation of step 1 in the "AFE Example Use Cases" from HRM.
 *
 */

ADI_AFE_RESULT_TYPE adi_AFE_PowerUp(ADI_AFE_DEV_HANDLE const hDevice) {
    ADI_AFE_RESULT_TYPE result;

#ifdef ADI_DEBUG
    if (adi_AFE_InvalidHandle(hDevice)) {
        return ADI_AFE_ERR_BAD_DEV_HANDLE;
    }

    if (adi_AFE_HandleNotInitialized(hDevice)) {
        return ADI_AFE_ERR_NOT_INITIALIZED;
    }

    if (pADI_SYSCLK->CLKCON5 & BITM_SYSCLK_CLKCON5_ACLKOFF) {
        return ADI_AFE_ERR_ACLKOFF;
    }

    if (16000000 != SystemGetClockFrequency(ADI_SYS_CLOCK_ACLK)) {
        return ADI_AFE_ERR_WRONG_ACLK_FREQUENCY;
    }
#endif

    /* Run the sequence, there is no output data */
    result = adi_AFE_RunSequence(hDevice, (uint32_t *)seq_afe_powerup, NULL, 0);

    return result;
}

/*!
 * @brief       Aux channel calibration
 *
 * @param[in]   hDevice                                 Device handle obtained from adi_AFE_Init().
 *
 * @return      Status
 *              - #ADI_AFE_SUCCESS                      Call completed successfully.
 *              - #ADI_AFE_ERR_BAD_DEV_HANDLE           Invalid device handle.
 *              - #ADI_AFE_ERR_NOT_INITIALIZED          Device not initialized.
 *              - #ADI_AFE_ERR_ACLKOFF                  ACLK disabled from the clock gate.
 *              - #ADI_AFE_ERR_WRONG_ACLK_FREQUENCY     Programmed ACLK frequency is not the required 16MHz.
 *
 * @details     The function calibrates the ADC aux channel.
 *
 * @details     It is an implementation of step 2 in the "AFE Example Use Cases" from HRM.
 *
 */

ADI_AFE_RESULT_TYPE adi_AFE_AuxChanCal(ADI_AFE_DEV_HANDLE const hDevice) {
    ADI_AFE_RESULT_TYPE     result = ADI_AFE_SUCCESS;;
    uint16_t rxBuffer[2];
    int32_t offset;
    uint32_t gain;

#ifdef ADI_DEBUG
    if (adi_AFE_InvalidHandle(hDevice)) {
        return ADI_AFE_ERR_BAD_DEV_HANDLE;
    }

    if (adi_AFE_HandleNotInitialized(hDevice)) {
        return ADI_AFE_ERR_NOT_INITIALIZED;
    }

    if (pADI_SYSCLK->CLKCON5 & BITM_SYSCLK_CLKCON5_ACLKOFF) {
        return ADI_AFE_ERR_ACLKOFF;
    }

    if (16000000 != SystemGetClockFrequency(ADI_SYS_CLOCK_ACLK)) {
        return ADI_AFE_ERR_WRONG_ACLK_FREQUENCY;
    }
#endif

    /* Run the sequence to perform 2 measurements and store them in rxBuffer through DMA */
    result = adi_AFE_RunSequence(hDevice, seq_afe_auxchancal, rxBuffer, 2);

    if (!result) {

        /* Performing 2-point calibration                                               */
        /* gain = 2^15 / (rxBuffer[1] - rxBuffer[0])                                    */
        /* offset = -20025 / gain - (rxBuffer[0] - 32768)                               */
        /* -20025 is the expected result from the first measurement, as a signed number */

        /* Second measurement always yields a higher code than the first measurement    */
        /* gain is an unsigned number                                                   */
        gain = (32768 * 16384 * 2) / (rxBuffer[1] - rxBuffer[0]);
        /* Rounding */
        gain = (gain + 1) >> 1;

        /* Calculate the offset with 0.25LSB resolution */
        offset = -20025 * (rxBuffer[1] - rxBuffer[0]);
        /* 0.125LSB resolution */
        offset = offset >> 12;
        /* Round to 0.25LSB resolution */
        offset = (offset + 1) >> 1;
        offset -= (rxBuffer[0] << 2);
        offset += (32768 << 2);

        /* Write gain and offset values to the calibration registers */
        if (ADI_AFE_SUCCESS != (result = adi_AFE_WriteCalibrationRegister(hDevice, ADI_AFE_CAL_REG_ADC_OFFSET_AUX, offset)))
        {
            return result;
        }
        if (ADI_AFE_SUCCESS != (result = adi_AFE_WriteCalibrationRegister(hDevice, ADI_AFE_CAL_REG_ADC_GAIN_AUX, gain)))
        {
            return result;
        }
    }

    return result;
}

/*!
 * @brief       Aux Channel Measurement with a voltage conversion
 *
 * @param[in]   hDevice     Device handle obtained from adi_AFE_Init().
 *
 * @param[out]  measResult  Measurement result.
 *
 * @return      Status
 *              - #ADI_AFE_SUCCESS                      Call completed successfully.
 *              - #ADI_AFE_ERR_BAD_DEV_HANDLE           Invalid device handle.
 *              - #ADI_AFE_ERR_NOT_INITIALIZED          Device not initialized.
 *              - #ADI_AFE_ERR_ACLKOFF                  ACLK disabled from the clock gate.
 *              - #ADI_AFE_ERR_WRONG_ACLK_FREQUENCY     Programmed ACLK frequency is not the required 16MHz.
 *
 * @details     The function performs a measurement from aux channel.
 *
 * @details     It is an implementation of step 3 in the "AFE Example Use Cases" from HRM.
 *
 */

ADI_AFE_RESULT_TYPE adi_AFE_AuxChanMeasFixed(ADI_AFE_DEV_HANDLE const hDevice, int32_t *measResult) {
    ADI_AFE_RESULT_TYPE     result = ADI_AFE_SUCCESS;
    uint16_t rxBuffer[2];
    int32_t                 num;
    int32_t                 den;

#ifdef ADI_DEBUG
    if (adi_AFE_InvalidHandle(hDevice)) {
        return ADI_AFE_ERR_BAD_DEV_HANDLE;
    }

    if (adi_AFE_HandleNotInitialized(hDevice)) {
        return ADI_AFE_ERR_NOT_INITIALIZED;
    }

    if (pADI_SYSCLK->CLKCON5 & BITM_SYSCLK_CLKCON5_ACLKOFF) {
        return ADI_AFE_ERR_ACLKOFF;
    }

    if (16000000 != SystemGetClockFrequency(ADI_SYS_CLOCK_ACLK)) {
        return ADI_AFE_ERR_WRONG_ACLK_FREQUENCY;
    }
#endif

    result = adi_AFE_RunSequence(hDevice, seq_afe_auxchanmeas2, &rxBuffer[0], 2);

    if (!result)
    {
        /* Voltage conversion: V = 1.1 + (ADC_result - 0x8000) / 18204          */
        /* Rewrite as V = (ADC_result - 0x31C8) / 18204                         */
        /* Result is the ratio: (rxBuffer[1] - 0x31C8) / (rxBuffer[0] - 0x31C8) */
        num = (int32_t)rxBuffer[1] - 0x000031C8;
        den = (int32_t)rxBuffer[0] - 0x000031C8;
        *measResult = (num << 16) / den;
    }

    return result;
}

/*!
 * @brief       Aux channel measurement on specified aux channel.
 *
 * @param[in]   hDevice     Device handle obtained from adi_AFE_Init().
 *              muxChannel  ADC Mux Channel
 *
 * @param[out]  measResult  Measurement result.
 *
 * @return      Status
 *              - #ADI_AFE_SUCCESS                        Call completed successfully.
 *
 * @details     The function performs a measurement from aux channel.
 *
 *
 */
 
ADI_AFE_RESULT_TYPE adi_AFE_AuxChanMeasCustom(ADI_AFE_DEV_HANDLE const hDevice, uint32_t muxChannel, uint16_t *measResult) {
    ADI_AFE_RESULT_TYPE result;
    uint16_t rxBuffer[1];
    uint32_t seqLocal[8];
    int i;

    for (i = 0; i < 8; i++) 
    {
        seqLocal[i] = seq_afe_auxchanmeas1[i];
    }
    seqLocal[2] = 0xA0000000 | muxChannel;

    /* Recalculate CRC in software for the measurement */
    adi_AFE_EnableSoftwareCRC(hDevice, true);

    result = adi_AFE_RunSequence(hDevice, seqLocal, &rxBuffer[0], 1);
    
    /* Restore to using default CRC stored with the sequence */
    adi_AFE_EnableSoftwareCRC(hDevice, false);

    *measResult = rxBuffer[0];

    return result;
}

/*!
 * @brief       Temperature sensor channel calibration
 *
 * @param[in]   hDevice                                 Device handle obtained from adi_AFE_Init().
 *
 * @return      Status
 *              - #ADI_AFE_SUCCESS                        Call completed successfully.
 *              - #ADI_AFE_ERR_BAD_DEV_HANDLE           Invalid device handle.
 *              - #ADI_AFE_ERR_NOT_INITIALIZED          Device not initialized.
 *              - #ADI_AFE_ERR_ACLKOFF                  ACLK disabled from the clock gate.
 *              - #ADI_AFE_ERR_WRONG_ACLK_FREQUENCY     Programmed ACLK frequency is not the required 16MHz.
 *
 * @details     The function calibrates the ADC temperature sensor channel.
 *
 * @details     It is an implementation of step 4 in the "AFE Example Use Cases" from HRM.
 *
 */

ADI_AFE_RESULT_TYPE adi_AFE_TempSensChanCal(ADI_AFE_DEV_HANDLE const hDevice) {
    ADI_AFE_RESULT_TYPE     result = ADI_AFE_SUCCESS;
    uint16_t rxBuffer[3];
    int32_t offset;
    uint32_t gain;

#ifdef ADI_DEBUG
    if (adi_AFE_InvalidHandle(hDevice)) {
        return ADI_AFE_ERR_BAD_DEV_HANDLE;
    }

    if (adi_AFE_HandleNotInitialized(hDevice)) {
        return ADI_AFE_ERR_NOT_INITIALIZED;
    }

    if (pADI_SYSCLK->CLKCON5 & BITM_SYSCLK_CLKCON5_ACLKOFF) {
        return ADI_AFE_ERR_ACLKOFF;
    }

    if (16000000 != SystemGetClockFrequency(ADI_SYS_CLOCK_ACLK)) {
        return ADI_AFE_ERR_WRONG_ACLK_FREQUENCY;
    }
#endif

    /* Run the sequence to perform 3 measurements and store them in rxBuffer through DMA */
    result = adi_AFE_RunSequence(hDevice, seq_afe_tempsenschancal, rxBuffer, 3);

    if (!result) 
    {
        /* Second measurement always yields a higher code than the first measurement    */
        /* gain is an unsigned number                                                   */
        gain = (19115 * 16384 * 2) / (rxBuffer[2] - rxBuffer[1]);
        /* Rounding */
        if (gain & 0x00000001) {
            gain = (gain >> 1) + 1;
        }
        else {
            gain = gain >> 1;
        }

        /* 0.25LSB resolution */
        offset = (32768 - rxBuffer[0]) << 2;

        /* Write gain and offset values to the calibration registers */
        if (ADI_AFE_SUCCESS != (result = adi_AFE_WriteCalibrationRegister(hDevice, ADI_AFE_CAL_REG_ADC_OFFSET_TEMP_SENS, offset)))
        {
            return result;
        }
        if (ADI_AFE_SUCCESS != (result = adi_AFE_WriteCalibrationRegister(hDevice, ADI_AFE_CAL_REG_ADC_GAIN_TEMP_SENS, gain)))
        {
            return result;
        }
    }

    return result;
}

/*!
 * @brief       Temperature measurement
 *
 * @param[in]   hDevice                                 Device handle obtained from adi_AFE_Init().
 * @param[out]  tdegc                                   EMasured temperature, in degrees Celsius.
 *
 * @return      Status
 *              - #ADI_AFE_SUCCESS                        Call completed successfully.
 *              - #ADI_AFE_ERR_BAD_DEV_HANDLE           Invalid device handle.
 *              - #ADI_AFE_ERR_NOT_INITIALIZED          Device not initialized.
 *              - #ADI_AFE_ERR_ACLKOFF                  ACLK disabled from the clock gate.
 *              - #ADI_AFE_ERR_WRONG_ACLK_FREQUENCY     Programmed ACLK frequency is not the required 16MHz.
 *
 * @details     The function performs a temperature measurement using the internal temperature sensor.
 *
 * @details     It is an implementation of step 5 in the "AFE Example Use Cases" from HRM.
 *
 */

ADI_AFE_RESULT_TYPE adi_AFE_TempSensMeas(ADI_AFE_DEV_HANDLE const hDevice, int32_t *tdegc) {
    ADI_AFE_RESULT_TYPE     result = ADI_AFE_SUCCESS;

#ifdef ADI_DEBUG
    if (adi_AFE_InvalidHandle(hDevice)) {
        return ADI_AFE_ERR_BAD_DEV_HANDLE;
    }

    if (adi_AFE_HandleNotInitialized(hDevice)) {
        return ADI_AFE_ERR_NOT_INITIALIZED;
    }

    if (pADI_SYSCLK->CLKCON5 & BITM_SYSCLK_CLKCON5_ACLKOFF) {
        return ADI_AFE_ERR_ACLKOFF;
    }

    if (16000000 != SystemGetClockFrequency(ADI_SYS_CLOCK_ACLK)) {
        return ADI_AFE_ERR_WRONG_ACLK_FREQUENCY;
    }
#endif

    /* Run the sequence to perform 1 measurement, the result will be read from AFE_TEMP_SENSOR_RESULT */
    result = adi_AFE_RunSequence(hDevice, seq_afe_tempsensmeas, NULL, 0);

    if (!result) {

        /* tdegc = (AFE_TEMP_SENSOR_RESULT - 3414) / 12.5 */
        *tdegc = ((int32_t)pADI_AFE->AFE_TEMP_SENSOR_RESULT - 3414) * 2 * 2 / 25;

        /* Rounding */
        if (*tdegc & 0x00000001) {
            *tdegc = (*tdegc >> 1) + 1;
        }
        else {
            *tdegc = *tdegc >> 1;
        }

    }

    return result;
}

/*!
 * @brief       Power up the excitation channel
 *
 * @param[in]   hDevice                                 Device handle obtained from adi_AFE_Init().
 *
 * @return      Status
 *              - #ADI_AFE_SUCCESS                        Call completed successfully.
 *              - #ADI_AFE_ERR_BAD_DEV_HANDLE           Invalid device handle.
 *              - #ADI_AFE_ERR_NOT_INITIALIZED          Device not initialized.
 *              - #ADI_AFE_ERR_ACLKOFF                  ACLK disabled from the clock gate.
 *              - #ADI_AFE_ERR_WRONG_ACLK_FREQUENCY     Programmed ACLK frequency is not the required 16MHz.
 *
 * @details     The function powers up the excitation using the recommended timing sequence.
 *
 * @details     It is an implementation of step 6 in the "AFE Example Use Cases" from HRM.
 *
 */

ADI_AFE_RESULT_TYPE adi_AFE_ExciteChanPowerUp(ADI_AFE_DEV_HANDLE const hDevice) {
    ADI_AFE_RESULT_TYPE     result = ADI_AFE_SUCCESS;

#ifdef ADI_DEBUG
    if (adi_AFE_InvalidHandle(hDevice)) {
        return ADI_AFE_ERR_BAD_DEV_HANDLE;
    }

    if (adi_AFE_HandleNotInitialized(hDevice)) {
        return ADI_AFE_ERR_NOT_INITIALIZED;
    }

    if (pADI_SYSCLK->CLKCON5 & BITM_SYSCLK_CLKCON5_ACLKOFF) {
        return ADI_AFE_ERR_ACLKOFF;
    }

    if (16000000 != SystemGetClockFrequency(ADI_SYS_CLOCK_ACLK)) {
        return ADI_AFE_ERR_WRONG_ACLK_FREQUENCY;
    }
#endif

    result = adi_AFE_RunSequence(hDevice, seq_afe_excitechanpowerup, NULL, 0);

    return result;
}


/*!
 * @brief       TIA channel calibration
 *
 * @param[in]   hDevice                                 Device handle obtained from adi_AFE_Init().
 *
 * @return      Status
 *              - #ADI_AFE_SUCCESS                      Call completed successfully.
 *              - #ADI_AFE_ERR_BAD_DEV_HANDLE           Invalid device handle.
 *              - #ADI_AFE_ERR_NOT_INITIALIZED          Device not initialized.
 *              - #ADI_AFE_ERR_ACLKOFF                  ACLK disabled from the clock gate.
 *              - #ADI_AFE_ERR_WRONG_ACLK_FREQUENCY     Programmed ACLK frequency is not the required 16MHz.
 *
 * @details     The function calibrates the TIA channel.
 *
 * @details     It is an implementation of step 7 in the "AFE Example Use Cases" from HRM.
 * @details     It performs a measurement at a time and uses register reads instead of DMA to retrieve the results.
 *
 */

ADI_AFE_RESULT_TYPE adi_AFE_TiaChanCal(ADI_AFE_DEV_HANDLE const hDevice) {
    ADI_AFE_RESULT_TYPE result = ADI_AFE_SUCCESS;
    uint32_t    seq_local[9];
    uint32_t    rcal;
    uint32_t    rtia;
    uint32_t    tiaChanDacCode;
    int32_t     offset_code;
    uint32_t    gain_code;
    int32_t     expected_code;
    int32_t     channel_offset;
    int32_t    channel_gain;
    int32_t    tmp;

#ifdef ADI_DEBUG
    if (adi_AFE_InvalidHandle(hDevice)) {
        return ADI_AFE_ERR_BAD_DEV_HANDLE;
    }

    if (adi_AFE_HandleNotInitialized(hDevice)) {
        return ADI_AFE_ERR_NOT_INITIALIZED;
    }

    if (pADI_SYSCLK->CLKCON5 & BITM_SYSCLK_CLKCON5_ACLKOFF) {
        return ADI_AFE_ERR_ACLKOFF;
    }

    if (16000000 != SystemGetClockFrequency(ADI_SYS_CLOCK_ACLK)) {
        return ADI_AFE_ERR_WRONG_ACLK_FREQUENCY;
    }
#endif

    /* Get RCAL value */
    if (ADI_AFE_SUCCESS != (result = adi_AFE_GetRcal(hDevice, &rcal)))
    {
        return result;
    }
    
    /* Get RTIA value */
    if (ADI_AFE_SUCCESS != (result = adi_AFE_GetRtia(hDevice, &rtia)))
    {
        return result;
    }
    
    /* Get TIA channel DAC code */
    if (ADI_AFE_SUCCESS != (result = adi_AFE_TiaChanDacCode(hDevice, &tiaChanDacCode)))
    {
        return result;
    }
    
    /* Start the calibration routine with default gain and offset values */
    adi_AFE_WriteCalibrationRegister(hDevice, ADI_AFE_CAL_REG_ADC_GAIN_TIA, (uint32_t)0x4000);
    adi_AFE_WriteCalibrationRegister(hDevice, ADI_AFE_CAL_REG_ADC_OFFSET_TIA, (uint32_t)0x000);

    /* Step 1 */
    /* Measure 0V (P = Vbias, N = Vbias) */
    
    /* Copy the seq_afe_tiachancal1 sequence to a local variable so that it can be updated */
    for (uint32_t i = 0; i < 9; i++)
    {
        seq_local[i] = seq_afe_tiachancal1[i];
    }
    
    seq_local[2] = 0xAA000000 + tiaChanDacCode;
    
    /* Recalculate CRC in software because we changed the original sequence */
    adi_AFE_EnableSoftwareCRC(hDevice, true);
    
    result = adi_AFE_RunSequence(hDevice, seq_local, NULL, 0);

    /* Restore to using default CRC stored with the sequence */
    adi_AFE_EnableSoftwareCRC(hDevice, false);
    
    if (!result) {

        /* Read measurement result, convert to signed format and invert the sign */
        offset_code = 32768 - pADI_AFE->AFE_SUPPLY_LPF_RESULT;
        /* Align to 0.25LSB resolution */
		offset_code = offset_code << 2;

        /* Write offset to the calibration register */
        adi_AFE_WriteCalibrationRegister(hDevice, ADI_AFE_CAL_REG_ADC_OFFSET_TIA, offset_code);

        /* Step 2 */
        /* Measure 0.7V (P = Vref, N = Vbias) */
        result = adi_AFE_RunSequence(hDevice, seq_afe_tiachancal2, NULL, 0);
    }

    if (!result) {

        /* Read measurement result and convert to a signed number */
    	gain_code = pADI_AFE->AFE_SUPPLY_LPF_RESULT - 32768;
        /* Gain code calculation: (0.7V * 1.5 / 3.6V) * 65535 */
		gain_code = (16384 * 19114) / gain_code;
/////////////////////////////////////////////////////////////////

        /* Write gain to the calibration register */
        adi_AFE_WriteCalibrationRegister(hDevice, ADI_AFE_CAL_REG_ADC_GAIN_TIA, gain_code);

        /* Step 3 */
        /* Measure voltage across Rcal*/
        result = adi_AFE_RunSequence(hDevice, seq_afe_tiachancal3, NULL, 0);
    }

    if (!result) {

        /* Read measurement result, convert to signed format and store for later usage */
        expected_code = pADI_AFE->AFE_SUPPLY_LPF_RESULT - 32768;

        /* Step 4 */
        /* Measure TIA channel offset (P = IVAO, N = IVAIM) */
        result = adi_AFE_RunSequence(hDevice, seq_afe_tiachancal4, NULL, 0);
    }

    if (!result) {

        channel_offset = 32768 - pADI_AFE->AFE_SUPPLY_LPF_RESULT;
        channel_offset = channel_offset << 2;
        /* Add normalized offset to TIA offset */
        /////////////// FIXME: See comment below, either truncate or round, and remove comment
        /* Truncation vs. rounding to be revised later */
        channel_offset += (offset_code * 16384) / gain_code;

        /* Write offset to the calibration register */
        adi_AFE_WriteCalibrationRegister(hDevice, ADI_AFE_CAL_REG_ADC_OFFSET_TIA, channel_offset);

        /* Step 5 */
        /* Measure TIA channel gain (P = IVAO, N = IVAIM) */
        result = adi_AFE_RunSequence(hDevice, seq_afe_tiachancal5, NULL, 0);
    }

    if (!result) {

        /* Read measurement result and convert to signed format */
        /* Result from TIA is "inverted" (i.e. positive current gives negative voltage) */
        channel_gain = 32768 - pADI_AFE->AFE_SUPPLY_LPF_RESULT;
        
        /* Current measured in step 3 */
        /* channel_gain = ((16384 * (rtia / rcal)) * expected_code) / channel_gain */
        tmp = (16384 * rtia) / rcal;
        channel_gain = (tmp * expected_code) / channel_gain;
        
        /* Multiply by the previously calculated gain and scale */
        /* Rounding to give 0.5 LSB error */
        channel_gain = ((channel_gain * gain_code) + (1 << 13)) >> 14;

        /* Write gain to the calibration register */
        adi_AFE_WriteCalibrationRegister(hDevice, ADI_AFE_CAL_REG_ADC_GAIN_TIA, (uint32_t)channel_gain);

    }

    return result;
}


/*!
 * @brief       Excitation channel calibration with attenuator enabled
 *
 * @param[in]   hDevice                                 Device handle obtained from adi_AFE_Init().
 *
 * @return      Status
 *              - #ADI_AFE_SUCCESS                      Call completed successfully.
 *              - #ADI_AFE_ERR_BAD_DEV_HANDLE           Invalid device handle.
 *              - #ADI_AFE_ERR_NOT_INITIALIZED          Device not initialized.
 *              - #ADI_AFE_ERR_ACLKOFF                  ACLK disabled from the clock gate.
 *              - #ADI_AFE_ERR_WRONG_ACLK_FREQUENCY     Programmed ACLK frequency is not the required 16MHz.
 *
 * @details     The function calibrates the excitation chanel when the attenuator is enabled.
 *
 * @details     It is an implementation of step 8 in the "AFE Example Use Cases" from HRM.
 *
 */

ADI_AFE_RESULT_TYPE adi_AFE_ExciteChanCalAtten(ADI_AFE_DEV_HANDLE const hDevice) {
    ADI_AFE_RESULT_TYPE     result = ADI_AFE_SUCCESS;
    uint16_t adcResult[2];
    int32_t offset;

#ifdef ADI_DEBUG
    if (adi_AFE_InvalidHandle(hDevice)) {
        return ADI_AFE_ERR_BAD_DEV_HANDLE;
    }

    if (adi_AFE_HandleNotInitialized(hDevice)) {
        return ADI_AFE_ERR_NOT_INITIALIZED;
    }

    if (pADI_SYSCLK->CLKCON5 & BITM_SYSCLK_CLKCON5_ACLKOFF) {
        return ADI_AFE_ERR_ACLKOFF;
    }

    if (16000000 != SystemGetClockFrequency(ADI_SYS_CLOCK_ACLK)) {
        return ADI_AFE_ERR_WRONG_ACLK_FREQUENCY;
    }
#endif

    /* Write 0x500 to DAC (0x800 + (0xA00 >> 1)) */
    adi_AFE_WriteCalibrationRegister(hDevice, ADI_AFE_CAL_REG_DAC_OFFSET_ATTEN, 0x00000A00);

    result = adi_AFE_RunSequence(hDevice, seq_afe_excitechancalatten1, NULL, 0);

    if (result) {
        return result;
    }

    adcResult[0] = pADI_AFE->AFE_SUPPLY_LPF_RESULT;

    /* Write 0xB00 to DAC (0x800 + (0x600 >> 1)) */
    adi_AFE_WriteCalibrationRegister(hDevice, ADI_AFE_CAL_REG_DAC_OFFSET_ATTEN, 0x00000600);

    result = adi_AFE_RunSequence(hDevice, seq_afe_excitechancalatten2, NULL, 0);

    if (result) {
        return result;
    }

    adcResult[1] = pADI_AFE->AFE_SUPPLY_LPF_RESULT;

/*
 *  Interpolate
 *
 *  Using formula: (x - x0) / (x1 - x0) = (y - y0) / (y1 - y0)
 *      where:
 *          x  = 0x800 + offset
 *          x0 = 0x500
 *          x1 = 0xB00
 *          y  = 0x8000
 *          y0 = adcResult[0]
 *          y1 = adcResult[1]
 *
 *  x = x0 + (x1 - x0) * (y - y0) / (y1 - y0)
 *    = 0x500 + 0x600 * (0x8000 - adcResult[0]) / (adcResult[1] - adcResult[0])
 *
 *  offset = 0x600 * (0x8000 - adcResult[0]) / (adcResult[1] - adcResult[0]) - 0x300
 *
 */

    offset = 0x00008000 - (int32_t)adcResult[0];
    offset *= 0x00000600;
    /* Increase resolution to 0.25 LSB */
    offset = offset << 2;
    offset /= (adcResult[1] - adcResult[0]);
    offset -= (0x00000300 << 2);
    /* Round to 0.5 LSB */
    offset = (offset + 1) >> 1;

    /* Write measured offset value to the calibration register */
    adi_AFE_WriteCalibrationRegister(hDevice, ADI_AFE_CAL_REG_DAC_OFFSET_ATTEN, offset);

    return result;
}

/*!
 * @brief       Excitation channel calibration with attenuator disabled
 *
 * @param[in]   hDevice     Device handle obtained from adi_AFE_Init().
 *
 * @return      Status
 *              - #ADI_AFE_SUCCESS                        Call completed successfully.
 *              - #ADI_AFE_ERR_BAD_DEV_HANDLE           Invalid device handle.
 *              - #ADI_AFE_ERR_NOT_INITIALIZED          Device not initialized.
 *              - #ADI_AFE_ERR_ACLKOFF                  ACLK disabled from the clock gate.
 *              - #ADI_AFE_ERR_WRONG_ACLK_FREQUENCY     Programmed ACLK frequency is not the required 16MHz.
 *
 * @details     The function calibrates the excitation chanel when the attenuator is disabled.
 *
 * @details     It is an implementation of step 9 in the "AFE Example Use Cases" from HRM.
 *
 */

ADI_AFE_RESULT_TYPE adi_AFE_ExciteChanCalNoAtten(ADI_AFE_DEV_HANDLE const hDevice) {
    ADI_AFE_RESULT_TYPE     result = ADI_AFE_SUCCESS;
    uint32_t    exChanNoAttenDeltaDacCode;
    uint16_t    adcResult[2];
    int32_t     offset;
    int32_t     tmp1, tmp2;

#ifdef ADI_DEBUG
    if (adi_AFE_InvalidHandle(hDevice)) {
        return ADI_AFE_ERR_BAD_DEV_HANDLE;
    }

    if (adi_AFE_HandleNotInitialized(hDevice)) {
        return ADI_AFE_ERR_NOT_INITIALIZED;
    }

    if (pADI_SYSCLK->CLKCON5 & BITM_SYSCLK_CLKCON5_ACLKOFF) {
        return ADI_AFE_ERR_ACLKOFF;
    }

    if (16000000 != SystemGetClockFrequency(ADI_SYS_CLOCK_ACLK)) {
        return ADI_AFE_ERR_WRONG_ACLK_FREQUENCY;
    }
#endif

    /* Get excitation channel (no attenuation) DAC code delta */
    if (ADI_AFE_SUCCESS != (result = adi_AFE_ExciteChanNoAttenDeltaDacCode(hDevice, &exChanNoAttenDeltaDacCode)))
    {
        return result;
    }
    
    adi_AFE_WriteCalibrationRegister(hDevice, ADI_AFE_CAL_REG_DAC_OFFSET_UNITY, 0 - exChanNoAttenDeltaDacCode);

    result = adi_AFE_RunSequence(hDevice, seq_afe_excitechancalnoatten1, NULL, 0);

    if (result) {
        return result;
    }

    adcResult[0] = pADI_AFE->AFE_SUPPLY_LPF_RESULT;

    adi_AFE_WriteCalibrationRegister(hDevice, ADI_AFE_CAL_REG_DAC_OFFSET_UNITY, exChanNoAttenDeltaDacCode);

    result = adi_AFE_RunSequence(hDevice, seq_afe_excitechancalnoatten2, NULL, 0);

    if (result) {
        return result;
    }

    adcResult[1] = pADI_AFE->AFE_SUPPLY_LPF_RESULT;

    /* 
     * Interpolate
     * Using formula: (x - x0) / (x1 - x0) = (y - y0) / (y1 - y0)
     * where: x  = 0x800 + offset
     *        x0 = 0x800 - (adi_AFE_ExciteChanNoAttenDeltaDacCode(hDevice) / 2)
     *        x1 = 0x800 + (adi_AFE_ExciteChanNoAttenDeltaDacCode(hDevice) / 2)
     *        y  = 0x8000
     *        y0 = adcResult[0]
     *        y1 = adcResult[1]
     *
     *      x   = x0 + (x1 - x0) * (y - y0) / (y1 - y0)
     *          = 0x7D9 + 0x4E * (0x8000 - adcResult[0]) / (adcResult[1] - adcResult[0])
     * offset   = 0x4E * (0x8000 - adcResult[0]) / (adcResult[1] - adcResult[0]) - 0x27
     *
     */

    tmp1 = exChanNoAttenDeltaDacCode * (0x8000 - adcResult[0]);
    tmp2 = adcResult[1] - adcResult[0];
    
    /* Adding 0.25 LSB precision */
    offset = (tmp1 << 2) / tmp2;
    /* Rounding to 0.5 LSB precision */
    offset = (offset + 1) >> 1;
    /* Subtract the delta */
    offset = offset - exChanNoAttenDeltaDacCode;
    
    /* Write measured offset value to the calibration register */
    adi_AFE_WriteCalibrationRegister(hDevice, ADI_AFE_CAL_REG_DAC_OFFSET_UNITY, offset);

    return result;
}

/*!
 * @brief       AC measurement example using fixed excitation.
 *
 * @param[in]   hDevice     Device handle obtained from adi_AFE_Init().
 *
 * @param[out]  dftResults                              Real and imaginary DFT results for both Rcal and TIA measurements.
 *
 * @return      Status
 *              - #ADI_AFE_SUCCESS                      Call completed successfully.
 *              - #ADI_AFE_ERR_BAD_DEV_HANDLE           Invalid device handle.
 *              - #ADI_AFE_ERR_NOT_INITIALIZED          Device not initialized.
 *              - #ADI_AFE_ERR_ACLKOFF                  ACLK disabled from the clock gate.
 *              - #ADI_AFE_ERR_WRONG_ACLK_FREQUENCY     Programmed ACLK frequency is not the required 16MHz.
 *
 * @details     It is an implementation of step 10 in the "AFE Example Use Cases" from HRM.
 *              The conversion from real and imaginary to magnitude and phase has to be performed
 *              in the application space.
 *
 */

ADI_AFE_RESULT_TYPE adi_AFE_AcMeas10khz(ADI_AFE_DEV_HANDLE const hDevice, int16_t *dftResults) {
    ADI_AFE_RESULT_TYPE     result = ADI_AFE_SUCCESS;

#ifdef ADI_DEBUG
    if (adi_AFE_InvalidHandle(hDevice)) {
        return ADI_AFE_ERR_BAD_DEV_HANDLE;
    }

    if (adi_AFE_HandleNotInitialized(hDevice)) {
        return ADI_AFE_ERR_NOT_INITIALIZED;
    }

    if (pADI_SYSCLK->CLKCON5 & BITM_SYSCLK_CLKCON5_ACLKOFF) {
        return ADI_AFE_ERR_ACLKOFF;
    }

    if (16000000 != SystemGetClockFrequency(ADI_SYS_CLOCK_ACLK)) {
        return ADI_AFE_ERR_WRONG_ACLK_FREQUENCY;
    }
#endif

    result = adi_AFE_RunSequence(hDevice, seq_afe_acmeas10khz, (uint16_t *)dftResults, 4);

    return result;
}

/*!
 * @brief       AC measurement example with programmable frequency, amplitude, attenuator and switch matrix states
 *
 * @param[in]   hDevice     Device handle obtained from adi_AFE_Init(). 
 *              fcw         Frequency control word (Fout = Faclk * (fcw / 2^26))
 *              amplitude   Amplitude (DAC codes)
 *              atten       Enable/disable attenuator (boolean)
 *              swState     Set the state of the switch mux
 *
 * @param[out]  dftResults  DFT result as real/imaginary pairs - Rcal first, unknown impedance second (4 results)
 *
 * @return      Status
 *              - #ADI_AFE_SUCCESS                        Call completed successfully.
 *
 * @details     Note: the amplitude parameter is the value written into the AFE_WG_AMPLITUDE register.
 *                    It does not account for the attenuator state (enabled/disabled)
 *
 *
 */

ADI_AFE_RESULT_TYPE adi_AFE_AcMeas(ADI_AFE_DEV_HANDLE const hDevice, uint32_t fcw, uint16_t amplitude, bool_t atten, uint16_t swState, int16_t *dftResults) {
    ADI_AFE_RESULT_TYPE result;
    uint32_t seqLocal[22];
    uint32_t i;
    
#ifdef ADI_DEBUG
    if (adi_AFE_InvalidHandle(hDevice)) {
        return ADI_AFE_ERR_BAD_DEV_HANDLE;
    }

    if (adi_AFE_HandleNotInitialized(hDevice)) {
        return ADI_AFE_ERR_NOT_INITIALIZED;
    }

    if (pADI_SYSCLK->CLKCON5 & BITM_SYSCLK_CLKCON5_ACLKOFF) {
        return ADI_AFE_ERR_ACLKOFF;
    }

    if (16000000 != SystemGetClockFrequency(ADI_SYS_CLOCK_ACLK)) {
        return ADI_AFE_ERR_WRONG_ACLK_FREQUENCY;
    }
    
    if ((0x0000014F > fcw) || (0x00051EB9 < fcw)) {
        return ADI_AFE_ERR_FCW_OUT_OF_RANGE;
    }
#endif
    
    /* Copy the sequence locally for modifications */
    for (i = 0; i < 22; i++) 
    {
        seqLocal[i] = seq_afe_acmeas10khz[i];
    }

    seqLocal[3] = 0x98000000 | fcw;
    seqLocal[4] = 0x9E000000 | amplitude;
    /* Already setup for attenuator, only change if unity mode required */
    if (!atten) 
    {
        seqLocal[5] = 0x88000F00;
    }
    seqLocal[14] = 0x86000000 | swState;
    
    /* Recalculate CRC in software because we changed the original sequence */
    adi_AFE_EnableSoftwareCRC(hDevice, true);
    
    result = adi_AFE_RunSequence(hDevice, seqLocal, (uint16_t *)dftResults, 4);

    /* Restore to using default CRC stored with the sequence */
    adi_AFE_EnableSoftwareCRC(hDevice, false);
    
    return result;
}

/*!
 * @brief                                               DC measurement example using trapezoid excitation.
 *
 * @param[in]   hDevice                                 Device handle obtained from adi_AFE_Init().
 * @param[in]   measBufferLength                        Number of samples to be returned by the DC measurement function.
 *
 * @param[out]  measBuffer                              Pointer to the results array.
 *
 * @return      Status
 *              - #ADI_AFE_SUCCESS                      Call completed successfully.
 *              - #ADI_AFE_ERR_BAD_DEV_HANDLE           Invalid device handle.
 *              - #ADI_AFE_ERR_NOT_INITIALIZED          Device not initialized.
 *              - #ADI_AFE_ERR_ACLKOFF                  ACLK disabled from the clock gate.
 *              - #ADI_AFE_ERR_WRONG_ACLK_FREQUENCY     Programmed ACLK frequency is not the required 16MHz.
 *
 * @details     It is an implementation of step 12 in the "AFE Example Use Cases" from HRM.
 *
 */

ADI_AFE_RESULT_TYPE adi_AFE_DcMeas(ADI_AFE_DEV_HANDLE const hDevice, uint16_t *measBuffer, uint16_t measBufferLength) {
    ADI_AFE_RESULT_TYPE     result = ADI_AFE_SUCCESS;

#ifdef ADI_DEBUG
    if (adi_AFE_InvalidHandle(hDevice)) {
        return ADI_AFE_ERR_BAD_DEV_HANDLE;
    }

    if (adi_AFE_HandleNotInitialized(hDevice)) {
        return ADI_AFE_ERR_NOT_INITIALIZED;
    }

    if (pADI_SYSCLK->CLKCON5 & BITM_SYSCLK_CLKCON5_ACLKOFF) {
        return ADI_AFE_ERR_ACLKOFF;
    }

    if (16000000 != SystemGetClockFrequency(ADI_SYS_CLOCK_ACLK)) {
        return ADI_AFE_ERR_WRONG_ACLK_FREQUENCY;
    }
#endif

    result = adi_AFE_RunSequence(hDevice, seq_afe_dcmeas, measBuffer, measBufferLength);

    return result;
}

/*!
 * @brief       Power down the AFE
 *
 * @param[in]   hDevice                                 Device handle obtained from adi_AFE_Init().
 *
 * @return      Status
 *              - #ADI_AFE_SUCCESS                      Call completed successfully.
 *              - #ADI_AFE_ERR_BAD_DEV_HANDLE           Invalid device handle.
 *              - #ADI_AFE_ERR_NOT_INITIALIZED          Device not initialized.
 *              - #ADI_AFE_ERR_ACLKOFF                  ACLK disabled from the clock gate.
 *              - #ADI_AFE_ERR_WRONG_ACLK_FREQUENCY     Programmed ACLK frequency is not the required 16MHz.
 *
 * @details     The function powers down the AFE. It uses Cortex writes instead of sequencer
 *              because accurate timing is not required.
 *
 * @details     It is an implementation of step 13 in the "AFE Example Use Cases" from HRM.
 *              It can be implemented as a sequence, but since it has no strict timing requirements
 *              it's easier to write to registers directly, and avoid the sequencer overhead.
 */

ADI_AFE_RESULT_TYPE adi_AFE_PowerDown(ADI_AFE_DEV_HANDLE const hDevice) {
    ADI_AFE_RESULT_TYPE     result = ADI_AFE_SUCCESS;

#ifdef ADI_DEBUG
    if (adi_AFE_InvalidHandle(hDevice)) {
        return ADI_AFE_ERR_BAD_DEV_HANDLE;
    }

    if (adi_AFE_HandleNotInitialized(hDevice)) {
        return ADI_AFE_ERR_NOT_INITIALIZED;
    }

    if (pADI_SYSCLK->CLKCON5 & BITM_SYSCLK_CLKCON5_ACLKOFF) {
        return ADI_AFE_ERR_ACLKOFF;
    }

    if (16000000 != SystemGetClockFrequency(ADI_SYS_CLOCK_ACLK)) {
        return ADI_AFE_ERR_WRONG_ACLK_FREQUENCY;
    }
#endif

    /* Initialize result, remains unchanged until the end for now */
    result = ADI_AFE_SUCCESS;

    /* DAC_ATTEN_EN = 0 */
    pADI_AFE->AFE_DAC_CFG &= ~BITM_AFE_AFE_DAC_CFG_DAC_ATTEN_EN;

    /* Change switch matrix to ALL OPEN */
    adi_AFE_SetSwitchState(hDevice, 0x00000000);

    /* ANEXCITESW_EN = 0 */
    pADI_AFE->AFE_ADC_CFG &= ~BITM_AFE_AFE_ADC_CFG_ANEXCITESW_EN;
    /* ALDO_EN = 0, REF_EN = 0, VBIASBUF_EN = 0, ADC_EN = 0, DAC_EN = 0, TIA_EN = 0, INAMP_EN = 0, BUF_EN = 0 */
    pADI_AFE->AFE_CFG &= ~(BITM_AFE_AFE_CFG_TIA_EN |
                           BITM_AFE_AFE_CFG_INAMP_EN |
                           BITM_AFE_AFE_CFG_BUF_EN |
                           BITM_AFE_AFE_CFG_VBIASBUF_EN |
                           BITM_AFE_AFE_CFG_ADC_EN |
                           BITM_AFE_AFE_CFG_DAC_EN |
                           BITM_AFE_AFE_CFG_REF_EN |
                           BITM_AFE_AFE_CFG_ALDO_EN);

    return result;
}

/****************************************************************************/
/*   Helper Functions                                                       */
/****************************************************************************/

/*!
 * @brief       Returns the DAC code required for the TIA channel calibration
 *
 * @param[in]   hDevice                                 Device handle obtained from adi_AFE_Init().
 *
 * @param[out]  TiaChanDacCode                          DAC code.
 *
 *
 * @return      Status
 *              - #ADI_AFE_SUCCESS                      Call completed successfully.
 *              - #ADI_AFE_ERR_BAD_DEV_HANDLE           Invalid device handle.
 *              - #ADI_AFE_ERR_NOT_INITIALIZED          Device not initialized.
 *
 *
 * @details     Formula is:     (750mV / RTIA) * 0.9 * RCAL / 1600mV * 4096
 *                          =   (1728 * RCAL) / RTIA
 *                          Add midscale at the end.
 *
 *
 */

ADI_AFE_RESULT_TYPE adi_AFE_TiaChanDacCode(ADI_AFE_DEV_HANDLE const hDevice, uint32_t* pTiaChanDacCode) {
    ADI_AFE_RESULT_TYPE result = ADI_AFE_SUCCESS;
    uint32_t            rcal;
    uint32_t            rtia;
  
#ifdef ADI_DEBUG
    if (adi_AFE_InvalidHandle(hDevice)) {
        return ADI_AFE_ERR_BAD_DEV_HANDLE;
    }

    if (adi_AFE_HandleNotInitialized(hDevice)) {
        return ADI_AFE_ERR_NOT_INITIALIZED;
    }
#endif
    
    if (result = adi_AFE_GetRcal(hDevice, &rcal))
    {
        return result;
    }
    
    if (result = adi_AFE_GetRtia(hDevice, &rtia))
    {
        return result;
    }
    
    /* Assign value with rounding (0.5 LSB error) */
    *pTiaChanDacCode = 0x800 + (((((1728 * rcal) << 1) / rtia) + 1) >> 1);
  
    return ADI_AFE_SUCCESS;
}

/*!
 * @brief       Returns the DAC code delta required for the excitation channel (no attenuation ) calibration
 *
 * @param[in]   hDevice                                 Device handle obtained from adi_AFE_Init().
 *
 * @param[out]  ExciteChanNoAttenDeltaDacCode           DAC code.
 *
 *
 * @return      Status
 *              - #ADI_AFE_SUCCESS                      Call completed successfully.
 *              - #ADI_AFE_ERR_BAD_DEV_HANDLE           Invalid device handle.
 *              - #ADI_AFE_ERR_NOT_INITIALIZED          Device not initialized.
 *
 * @details     Excitation channel calibration codes, stored as delta from midscale. The delta value 
 *              will be added and subtracted for interpolation. Calculated using the same formula as 
 *              adi_AFE_TiaChanDacCode.
 *              Note: Offset has 0.5LSB resolution, left-shifting by 1
 *
 *
 */

ADI_AFE_RESULT_TYPE     adi_AFE_ExciteChanNoAttenDeltaDacCode(ADI_AFE_DEV_HANDLE const hDevice, uint32_t* pExciteChanNoAttenDeltaDacCode) {
    ADI_AFE_RESULT_TYPE result = ADI_AFE_SUCCESS;
    uint32_t            rcal;
    uint32_t            rtia;
  
#ifdef ADI_DEBUG
    if (adi_AFE_InvalidHandle(hDevice)) {
        return ADI_AFE_ERR_BAD_DEV_HANDLE;
    }

    if (adi_AFE_HandleNotInitialized(hDevice)) {
        return ADI_AFE_ERR_NOT_INITIALIZED;
    }
#endif
    
    if (result = adi_AFE_GetRcal(hDevice, &rcal))
    {
        return result;
    }
    
    if (result = adi_AFE_GetRtia(hDevice, &rtia))
    {
        return result;
    }
    
    /* Formula is: (750mV / RTIA) * 0.9 * RCAL / 1600mV * 4096 */
    *pExciteChanNoAttenDeltaDacCode = (((((1728 * rcal) << 1) / rtia) + 1) >> 1) << 1;
    
    return ADI_AFE_SUCCESS;
}


#define REVERT_ADI_MISRA_SUPPRESSIONS  /*!< Revert ADI MISRA Suppressions */
#include "misra.h"

/*
** EOF
*/

/*@}*/
