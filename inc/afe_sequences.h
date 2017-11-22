
/*! \addtogroup AFE_Library AFE Library
 *  @{
 */

#ifndef __AFE_SEQUENCES_H__
#define __AFE_SEQUENCES_H__

//#include "afe.h"

/* C++ linkage */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/***************************************************************************/
/*   Sequences                                                             */
/***************************************************************************/

/*
 * AFE powerup
 */
const uint32_t seq_afe_powerup[] = {
    0x000800FC, /*  0 - Safety Word, Command Count = 8, CRC = 0xFC                  */
    0x80080010, /*  1 - AFE_CFG: ALDOILIMIT_EN = 1 ALDO_EN = 1                      */
    0x00007CFE, /*  2 - Wait: 2 ms                                                  */
    0x80040030, /*  3 - AFE_CFG: REF_EN = 1 VREFBUFILIMIT_EN = 1 ALDOILIMIT_EN = 0  */
    0x0004E1FE, /*  4 - Wait: 20 ms                                                 */
    0x80020030, /*  5 - AFE_CFG: VREFBUFILIMIT_EN = 0 VBIASBUF_EN = 1               */
    0x0001387E, /*  6 - Wait: 5 ms                                                  */
    0x800200B0, /*  7 - AFE_CFG: ADC_EN = 1                                         */
    0x82000002  /*  8 - AFE_SEQ_CFG: SEQ_EN = 0                                     */
};

/*
 * AFE auxiliary channel calibration
 */
const uint32_t seq_afe_auxchancal[] = {
    0x000F00CA, /*  0 - Safety Word, Command Count = 15, CRC = 0xCA                                                                                 */
    0x84007818, /*  1 - AFE_FIFO_CFG: DATA_FIFO_DMA_REQ_EN = 1 DATA_FIFO_SOURCE_SEL = 0b11 CMD_FIFO_DMA_REQ_EN = 1 CMD_FIFO_EN = 1 DATA_FIFO_EN = 1 */
    0x0000063E, /*  2 - Wait: 100 us                                                                                                                */
    0xA000031C, /*  3 - AFE_ADC_CFG: MUX_SEL = 0b11100 GAIN_OFFS_SEL = 0b11                                                                         */
    0x0000063E, /*  4 - Wait:  100 us                                                                                                               */
    0x800301B0, /*  5 - AFE_CFG: ADC_CONV_EN = 1 SUPPLY_LPF_EN = 1                                                                                  */
    0x0009087E, /*  6 - Wait: 37 ms                                                                                                                 */
    0x800200B0, /*  7 - AFE_CFG: ADC_CONV_EN = 0 SUPPLY_LPF_EN = 0                                                                                  */
    0xA0000300, /*  8 - AFE_ADC_CFG: MUX_SEL = 0                                                                                                    */
    0xA000031D, /*  9 - AFE_ADC_CFG: MUX_SEL = 0b11101                                                                                              */
    0x0000063E, /* 10 - Wait: 100 us                                                                                                                */
    0x800301B0, /* 11 - AFE_CFG: ADC_CONV_EN = 1 SUPPLY_LPF_EN = 1                                                                                  */
    0x0009087E, /* 12 - Wait: 37 ms                                                                                                                 */
    0x800200B0, /* 13 - AFE_CFG: ADC_CONV_EN = 0 SUPPLY_LPF_EN = 0                                                                                  */
    0xA0000300, /* 14 - AFE_ADC_CFG: MUX_SEL = 0                                                                                                    */
    0x82000002  /* 15 - AFE_SEQ_CFG: SEQ_EN = 0                                                                                                     */
};

/*
 * AFE auxiliary channel measurement (two measurements)
 */
const uint32_t seq_afe_auxchanmeas2[] = {
    0x000E00CA, /*  1 - Safety Word, Command Count = 14, CRC = 0xCA                                                                                 */
    0x84007818, /*  2 - AFE_FIFO_CFG: DATA_FIFO_DMA_REQ_EN = 1 DATA_FIFO_SOURCE_SEL = 0b11 CMD_FIFO_DMA_REQ_EN = 1 CMD_FIFO_EN = 1 DATA_FIFO_EN = 1 */
    0xA000022C, /*  3 - AFE_ADC_CFG: MUX_SEL = 0b01100 ANEXCITESW_EN = 1 GAIN_OFFS_SEL = 0b10                                                       */
    0x0000063E, /*  4 - Wait: 100 us                                                                                                                */
    0x800301B0, /*  5 - AFE_CFG: ADC_CONV_EN = 1 SUPPLY_LPF_EN = 1                                                                                  */
    0x0009087E, /*  6 - Wait: 37 ms                                                                                                                 */
    0x800200B0, /*  7 - AFE_CFG: ADC_CONV_EN = 0 SUPPLY_LPF_EN = 0                                                                                  */
    0xA0000220, /*  8 - AFE_ADC_CFG: MUX_SEL = 0                                                                                                    */
    0xA0000228, /*  9 - AFE_ADC_CFG: MUX_SEL = 0b01000                                                                                              */
    0x0000063E, /* 10 - Wait: 100 us                                                                                                                */
    0x800301B0, /* 11 - AFE_CFG: ADC_CONV_EN = 1 SUPPLY_LPF_EN = 1                                                                                  */
    0x0009087E, /* 12 - Wait: 37 ms                                                                                                                 */
    0x800200B0, /* 13 - AFE_CFG: ADC_CONV_EN = 0 SUPPLY_LPF_EN = 0                                                                                  */
    0xA0000200, /* 14 - AFE_ADC_CFG: MUX_SEL = 0 ANEXCITESW_EN = 0                                                                                  */
    0x82000002  /* 15 - AFE_SEQ_CFG: SEQ_EN = 0                                                                                                     */
};

/*
 * AFE auxiliary channel measurement (single measurement)
 */
const uint32_t seq_afe_auxchanmeas1[] = {
    0x000800F0, /*  0 - Safety Word, Command Count = 8, CRC = 0xF0                                                                                  */
    0x84007818, /*  1 - AFE_FIFO_CFG: DATA_FIFO_DMA_REQ_EN = 1 DATA_FIFO_SOURCE_SEL = 0b11 CMD_FIFO_DMA_REQ_EN = 1 CMD_FIFO_EN = 1 DATA_FIFO_EN = 1 */
    0xA0000000, /*  2 - AFE_ADC_CFG: MUX_SEL = 0 GAIN_OFFS_SEL = 0 ANEXCITESW_EN = 0                                                                */
    0x0000063E, /*  3 - Wait: 100 us                                                                                                                */
    0x800301B0, /*  4 - AFE_CFG: ADC_CONV_EN = 1 SUPPLY_LPF_EN = 1                                                                                  */
    0x0009087E, /*  5 - Wait: 37 ms                                                                                                                 */
    0x800200B0, /*  6 - AFE_CFG: ADC_CONV_EN = 0 SUPPLY_LPF_EN = 0                                                                                  */
    0xA0000000, /*  7 - AFE_ADC_CFG: MUX_SEL = 0 ANEXCITESW_EN = 0                                                                                  */
    0x82000002  /*  8 - AFE_SEQ_CFG: SEQ_EN = 0                                                                                                     */
};

/*
 * AFE temperature sensor calibration
 */
const uint32_t seq_afe_tempsenschancal[] = {
    0x001500D3, /*  0 - Safety Word, Command Count = 21, CRC = 0xD3                                                                                 */
    0x84007818, /*  1 - AFE_FIFO_CFG: DATA_FIFO_DMA_REQ_EN = 1 DATA_FIFO_SOURCE_SEL = 0b11 CMD_FIFO_DMA_REQ_EN = 1 CMD_FIFO_EN = 1 DATA_FIFO_EN = 1 */
    0x0000063E, /*  2 - Wait: 100 us                                                                                                                */
    0xA000031A, /*  3 - AFE_ADC_CFG: MUX_SEL = 0b11010 GAIN_OFFS_SEL = 0b11                                                                         */
    0x0000063E, /*  4 - Wait: 100 us                                                                                                                */
    0x800301B0, /*  5 - AFE_CFG: ADC_CONV_EN = 1 SUPPLY_LPF_EN = 1                                                                                  */
    0x0009087E, /*  6 - Wait: 37 ms                                                                                                                 */
    0x800200B0, /*  7 - AFE_CFG: ADC_CONV_EN = 0 SUPPLY_LPF_EN = 0                                                                                  */
    0xA0000300, /*  8 - AFE_ADC_CFG: MUX_SEL = 0                                                                                                    */
    0xA0000318, /*  9 - AFE_ADC_CFG: MUX_SEL = 0b11000                                                                                              */
    0x0000063E, /* 10 - Wait: 100 us                                                                                                                */
    0x800301B0, /* 11 - AFE_CFG: ADC_CONV_EN = 1 SUPPLY_LPF_EN = 1                                                                                  */
    0x0009087E, /* 12 - Wait: 37 ms                                                                                                                 */
    0x800200B0, /* 13 - AFE_CFG: ADC_CONV_EN = 0 SUPPLY_LPF_EN = 0                                                                                  */
    0xA0000300, /* 14 - AFE_ADC_CFG: MUX_SEL = 0                                                                                                    */
    0xA0000319, /* 15 - AFE_ADC_CFG: MUX_SEL = 0b11001                                                                                              */
    0x0000063E, /* 16 - Wait: 100 us                                                                                                                */
    0x800301B0, /* 17 - AFE_CFG: ADC_CONV_EN = 1 SUPPLY_LPF_EN = 1                                                                                  */
    0x0009087E, /* 18 - Wait: 37 ms                                                                                                                 */
    0x800200B0, /* 19 - AFE_CFG: ADC_CONV_EN = 0 SUPPLY_LPF_EN = 0                                                                                  */
    0xA0000300, /* 20 - AFE_ADC_CFG: MUX_SEL = 0                                                                                                    */
    0x82000002  /* 21 - AFE_SEQ_CFG: SEQ_EN = 0                                                                                                     */
};

/*
 * AFE temperature sensor measurement
 */
const uint32_t seq_afe_tempsensmeas[] = {
    0x00090078, /*  0 - Safety Word, Command Count = 9, CRC = 0x78                                      */
    0x800210B0, /*  1 - AFE_CFG: TEMP_SENSOR_EN = 1                                                     */
    0xA0000103, /*  2 - AFE_ADC_CFG: MUX_SEL = 0b11 GAIN_OFFS_SEL = 1                                   */
    0x0000063E, /*  3 - Wait: 100 us                                                                    */
    0x800331B0, /*  4 - AFE_CFG: ADC_CONV_EN = 1 TEMP_CONV_EN = 1 SUPPLY_LPF_EN = 1                     */
    0x0009087E, /*  5 - Wait: 37 ms                                                                     */
    0x800200B0, /*  6 - AFE_CFG: TEMP_SENSOR_EN = 0 TEMP_CONV_EN = 0 ADC_CONV_EN = 0 SUPPLY_LPF_EN = 0  */
    0xA0000100, /*  7 - AFE_ADC_CFG: MUX_SEL = 0                                                        */
    0xA0000300, /*  8 - AFE_ADC_CFG: GAIN_OFFS_SEL = 0b11                                               */
    0x82000002  /*  9 - AFE_SEQ_CFG: SEQ_EN = 0                                                         */
};

/*
 * AFE excitation channel powerup
 */
const uint32_t seq_afe_excitechanpowerup[] = {
    0x00030050, /*  0 - Safety Word, Command Count = 3, CRC = 0x50              */
    0x80020EF0, /*  1 - AFE_CFG: TIA_EN = 1 INAMP_EN = 1 BUF_EN = 1 DAC_EN = 1  */
    0x0000063E, /*  2 - Wait: 100 us                                            */
    0x82000002  /*  3 - AFE_SEQ_CFG: SEQ_EN = 0                                 */
};

/*
 * AFE TIA channel calibration, implemented as multiple sequences
 *
 * Step 1
 */
const uint32_t seq_afe_tiachancal1[] = {
    0x000800E8, /*  0 - Safety Word, Command Count = 8, CRC = 0xE8                                  */
    0x86008811, /*  1 - AFE_SW_CFG: DMUX_STATE = 1  PMUX_STATE = 1 NMUX_STATE = 8 TMUX_STATE = 8    */
    0xAA0008E7, /*  2 - AFE_WG_DAC_CODE: DAC_CODE = 0x8E7                                           */
    0xA0000018, /*  3 - AFE_ADC_CFG: GAIN_OFFS_SEL = 0 MUX_SEL = 0b11000                            */
    0x0000063E, /*  4 - Wait: 100 us                                                                */
    0x80030FF0, /*  5 - AFE_CFG: ADC_CONV_EN = 1 SUPPLY_LPF_EN = 1                                  */
    0x0009087E, /*  6 - Wait: 37 ms                                                                 */
    0x80020EF0, /*  7 - AFE_CFG: ADC_CONV_EN = 0 SUPPLY_LPF_EN = 0                                  */
    0x82000002  /*  8 - AFE_SEQ_CFG: SEQ_EN = 0                                                     */
};

/*
 * AFE TIA channel calibration, implemented as multiple sequences
 *
 * Step 2
 */
const uint32_t seq_afe_tiachancal2[] = {
    0x00070026, /*  0 - Safety Word, Command Count = 7, CRC = 0x26          */
    0xA0000000, /*  1 - AFE_ADC_CFG: MUX_SEL = 0                            */
    0xA0000019, /*  2 - AFE_ADC_CFG: GAIN_OFFS_SEL = 0 MUX_SEL = 0b10111    */
    0x0000063E, /*  3 - Wait: 100 us                                        */
    0x80030FF0, /*  4 - AFE_CFG: ADC_CONV_EN = 1 SUPPLY_LPF_EN = 1          */
    0x0009087E, /*  5 - Wait: 37 ms                                         */
    0x80020EF0, /*  6 - AFE_CFG: ADC_CONV_EN = 0 SUPPLY_LPF_EN = 0          */
    0x82000002  /*  7 - AFE_SEQ_CFG: SEQ_EN = 0                             */
};

/*
 * AFE TIA channel calibration, implemented as multiple sequences
 *
 * Step 3
 */
const uint32_t seq_afe_tiachancal3[] = {
    0x00070074, /*  0 - Safety Word, Command Count = 7, CRC = 0x74          */
    0xA0000000, /*  1 - AFE_ADC_CFG: MUX_SEL = 0                            */
    0xA000001E, /*  3 - AFE_ADC_CFG: GAIN_OFFS_SEL = 0 MUX_SEL = 0b11110    */
    0x0000063E, /*  4 - Wait: 100 us                                        */
    0x80030FF0, /*  5 - AFE_CFG: ADC_CONV_EN = 1 SUPPLY_LPF_EN = 1          */
    0x0009087E, /*  6 - Wait: 37 ms                                         */
    0x80020EF0, /*  7 - AFE_CFG: ADC_CONV_EN = 0 SUPPLY_LPF_EN = 0          */
    0x82000002  /*  8 - AFE_SEQ_CFG: SEQ_EN = 0                             */
};

/*
 * AFE TIA channel calibration, implemented as multiple sequences
 *
 * Step 4
 */
const uint32_t seq_afe_tiachancal4[] = {
    0x00080022, /*  0 - Safety Word, Command Count = 8, CRC = 0x22                              */
    0xA0000000, /*  1 - AFE_ADC_CFG: MUX_SEL = 0                                                */
    0xA0000002, /*  2 - AFE_ADC_CFG: MUX_SEL = 0b00010                                          */
    0x86003000, /*  3 - AFE_SW_CFG: DMUX_STATE = 0 PMUX_STATE = 0 NMUX_STATE = 0 TMUX_STATE = 3 */
    0x0000063E, /*  4 - Wait: 100 us                                                            */
    0x80030FF0, /*  5 - AFE_CFG: ADC_CONV_EN = 1 SUPPLY_LPF_EN = 1                              */
    0x0009087E, /*  6 - Wait: 37 ms                                                             */
    0x80020EF0, /*  7 - AFE_CFG: ADC_CONV_EN = 0 SUPPLY_LPF_EN = 0                              */
    0x82000002  /*  8 - AFE_SEQ_CFG: SEQ_EN = 0                                                 */
};

/*
 * AFE TIA channel calibration, implemented as multiple sequences
 *
 * Step 5
 */
const uint32_t seq_afe_tiachancal5[] = {
    0x00060070, /*  0 - Safety Word, Command Count = 6, CRC = 0x70                              */
    0x86008811, /*  1 - AFE_SW_CFG: DMUX_STATE = 1 PMUX_STATE = 1 NMUX_STATE = 8 TMUX_STATE = 8 */
    0x0000063E, /*  2 - Wait: 100 us                                                            */
    0x80030FF0, /*  3 - AFE_CFG: ADC_CONV_EN = 1 SUPPLY_LPF_EN = 1                              */
    0x0009087E, /*  4 - Wait: 37 ms                                                             */
    0x80020EF0, /*  5 - AFE_CFG: ADC_CONV_EN = 0 SUPPLY_LPF_EN = 0                              */
    0x82000002  /*  6 - AFE_SEQ_CFG: SEQ_EN = 0                                                 */
};

/*
 * AFE excitation channel calibration (attenuated mode), implemented as multiple sequences
 *
 * Step 1
 */
const uint32_t seq_afe_excitechancalatten1[] = {
    0x000900E8, /*  0 - Safety Word, Command Count = 9, CRC = 0xE8                              */
    0x88000F01, /*  1 - AFE_DAC_CFG: DAC_ATTEN_EN = 1                                           */
    0x86008811, /*  2 - AFE_SW_CFG: DMUX_STATE = 1 PMUX_STATE = 1 NMUX_STATE = 8 TMUX_STATE = 8 */
    0xAA000800, /*  3 - AFE_WG_DAC_CODE: DAC_CODE = 0x800                                       */
    0xA0000002, /*  4 - AFE_ADC_CFG: MUX_SEL = 0b00010 GAIN_OFFS_SEL = 0                        */
    0x0000063E, /*  5 - Wait: 100 us                                                            */
    0x80030FF0, /*  6 - AFE_CFG: ADC_CONV_EN = 1 SUPPLY_LPF_EN = 1                              */
    0x0009087E, /*  7 - Wait: 37 ms                                                             */
    0x80020EF0, /*  8 - AFE_CFG: ADC_CONV_EN = 0 SUPPLY_LPF_EN = 0                              */
    0x82000002  /*  9 - AFE_SEQ_CFG: SEQ_EN = 0                                                 */
};

/*
 * AFE excitation channel calibration (attenuated mode), implemented as multiple sequences
 *
 * Step 2
 */
const uint32_t seq_afe_excitechancalatten2[] = {
    0x0006007C, /*  0 - Safety Word, Command Count = 6, CRC = 0x7C  */
    0xAA000800, /*  1 - AFE_WG_DAC_CODE: DAC_CODE = 0x800           */
    0x0000063E, /*  2  Wait: 100 us                                 */
    0x80030FF0, /*  3 - AFE_CFG: ADC_CONV_EN = 1 SUPPLY_LPF_EN = 1  */
    0x0009087E, /*  4 - Wait: 37 ms                                 */
    0x80020EF0, /*  5 - AFE_CFG: ADC_CONV_EN = 0 SUPPLY_LPF_EN = 0  */
    0x82000002  /*  6 - AFE_SEQ_CFG: SEQ_EN = 0                     */
};

/*
 * AFE excitation channel calibration (unity/non-attenuated mode), implemented as multiple sequences
 *
 * Step 1
 */
const uint32_t seq_afe_excitechancalnoatten1[] = {
    0x000900F4, /*  0 - Safety Word, Command Count = 9, CRC = 0xF4                              */
    0x88000F00, /*  1 - AFE_DAC_CFG: DAC_ATTEN_EN = 0                                           */
    0x86008811, /*  2 - AFE_SW_CFG: DMUX_STATE = 1 PMUX_STATE = 1 NMUX_STATE = 8 TMUX_STATE = 8 */
    0xAA000800, /*  3 - AFE_WG_DAC_CODE: DAC_CODE = 0x800                                       */
    0xA0000002, /*  4 - AFE_ADC_CFG: MUX_SEL = 2 GAIN_OFFS_SEL = 0                              */
    0x0000063E, /*  5 - Wait: 100 us                                                            */
    0x80030FF0, /*  6 - AFE_CFG: ADC_CONV_EN = 1 SUPPLY_LPF_EN = 1                              */
    0x0009087E, /*  7 - Wait: 37 ms                                                             */
    0x80020EF0, /*  8 - AFE_CFG: ADC_CONV_EN = 0 SUPPLY_LPF_EN = 0                              */
    0x82000002  /*  9 - AFE_SEQ_CFG: SEQ_EN = 0                                                 */
};

/*
 * AFE excitation channel calibration (unity/non-attenuated mode), implemented as multiple sequences
 *
 * Step 2
 */
const uint32_t seq_afe_excitechancalnoatten2[] = {
    0x0006007C, /*  0 - Safety Word, Command Count = 6, CRC = 0x7C  */
    0xAA000800, /*  1 - AFE_WG_DAC_CODE: DAC_CODE = 0x800           */
    0x0000063E, /*  2 - Wait: 100 us                                */
    0x80030FF0, /*  3 - AFE_CFG: ADC_CONV_EN = 1 SUPPLY_LPF_EN = 1  */
    0x0009087E, /*  4 - Wait: 37 ms                                 */
    0x80020EF0, /*  5 - AFE_CFG: ADC_CONV_EN = 0 SUPPLY_LPF_EN = 0  */
    0x82000002  /*  6 - AFE_SEQ_CFG: SEQ_EN = 0                     */
};

/*
 * AFE AC measurement, 10kHz excitation
 */
const uint32_t seq_afe_acmeas10khz[] = {
    0x001500F3, /*  0 - Safety Word, Command Count = 21, CRC = 0xF3                                                                                 */
    0x84005818, /*  1 - AFE_FIFO_CFG: DATA_FIFO_DMA_REQ_EN = 1 CMD_FIFO_DMA_REQ_EN = 1 DATA_FIFO_SOURCE_SEL = 0b10 CMD_FIFO_EN = 1 DATA_FIFO_EN = 1 */
    0x8A000034, /*  2 - AFE_WG_CFG: TYPE_SEL = 0b10                                                                                                 */
    0x9800A3D7, /*  3 - AFE_WG_FCW: SINE_FCW = 0xA3D7                                                                                               */
    0x9E000516, /*  4 - AFE_WG_AMPLITUDE: SINE_AMPLITUDE = 0x516                                                                                    */
    0x88000F01, /*  5 - AFE_DAC_CFG: DAC_ATTEN_EN = 1                                                                                               */
    0x86008811, /*  6 - AFE_SW_CFG: DMUX_STATE = 1 PMUX_STATE = 1 NMUX_STATE = 8 TMUX_STATE = 8                                                     */
    0xA0000002, /*  7 - AFE_ADC_CFG: MUX_SEL = 0b00010 GAIN_OFFS_SEL = 0                                                                            */
    0x0000063E, /*  8 - Wait: 100 us                                                                                                                */
    0x80024EF0, /*  9 - AFE_CFG: WAVEGEN_EN = 1                                                                                                     */
    0x00000C7E, /* 10 - Wait: 200 us                                                                                                                */
    0x8002CFF0, /* 11 - AFE_CFG: ADC_CONV_EN = 1 DFT_EN = 1                                                                                         */
    0x0003233E, /* 12 - Wait: 12.852 ms                                                                                                             */
                /* Need to wait (13ms - 148us) to stop at midscale                                                                                  */
    0x80020EF0, /* 13 - AFE_CFG: WAVEGEN_EN = 0 ADC_CONV_EN = 0 DFT_EN = 0                                                                          */
    0x86003267, /* 14 - AFE_SW_CFG: DMUX_STATE = 7 PMUX_STATE = 6 NMUX_STATE = 2 TMUX_STATE = 3                                                     */
    0x0000063E, /* 15 - Wait: 100 us                                                                                                                */
    0x80024EF0, /* 16 - AFE_CFG: WAVEGEN_EN = 1                                                                                                     */
    0x00000C7E, /* 17 - Wait: 200 us                                                                                                                */
    0x8002CFF0, /* 18 - AFE_CFG: ADC_CONV_EN = 1 DFT_EN = 1                                                                                         */
    0x0003233E, /* 19 - Wait: 12.852 ms                                                                                                             */
                /* Need to wait (13ms - 148us) to stop at midscale                                                                                  */
    0x80020EF0, /* 20 - AFE_CFG: WAVEGEN_EN = 0 ADC_CONV_EN = 0 DFT_EN = 0                                                                          */
    0x82000002  /* 21 - AFE_SEQ_CFG: SEQ_EN = 0                                                                                                     */
};

/*
 * AFE DC measurement (trapazoid)
 */
const uint32_t seq_afe_dcmeas[] = {
    0x001000B4, /*  0 - Safety Word, Command Count = 16, CRC = 0xB4                                                                                 */
    0x84007818, /*  1 - AFE_FIFO_CFG: DATA_FIFO_DMA_REQ_EN = 1 DATA_FIFO_SOURCE_SEL = 0b11 CMD_FIFO_DMA_REQ_EN = 1 CMD_FIFO_EN = 1 DATA_FIFO_EN = 1 */
    0x8A000036, /*  2 - AFE_WG_CFG: TYPE_SEL = 0b11                                                                                                 */
    0x8C000800, /*  3 - AFE_WG_DCLEVEL_1: TRAP_DC_LEVEL_1 = 0x800                                                                                   */
    0x8E000C80, /*  4 - AFE_WG_DCLEVEL_2: TRAP_DC_LEVEL_2 = 0xC80                                                                                   */
    0x90007F8D, /*  5 - AFE_WG_DELAY_1: TRAP_DELAY_1 = 0x7F8D                                                                                       */
    0x92000CC1, /*  6 - AFE_WG_SLOPE_1: TRAP_SLOPE_1 = 0x0CC1                                                                                       */
    0x0000063E, /*  7 - Wait: 100 us                                                                                                                */
                /* This wait is needed because all the commands before this one are MMR writes. They are executed in a single clock cycle, and the  */
                /* DMA controller cannot keep up, resulting in the sequencer being starved (triggering a command FIFO underflow). 100us is          */
                /* sufficiently large and could be adjusted, depending on system load.                                                              */
    0x94007F8D, /*  8 - AFE_WG_DELAY_2: TRAP_DELAY_2 = 0x7F8D                                                                                       */
    0x96000CC1, /*  9 - AFE_WG_SLOPE_2: TRAP_SLOPE_2 = 0x0CC1                                                                                       */
    0x86003267, /* 10 - AFE_SW_CFG: DMUX_STATE = 7 PMUX_STATE = 6 NMUX_STATE = 2 TMUX_STATE = 3                                                     */
    0xA0000002, /* 11 - AFE_ADC_CFG: MUX_SEL = 0b10 GAIN_OFFS_SEL = 0                                                                               */
    0x0000063E, /* 12 - Wait: 100 us                                                                                                                */
    0x80034FF0, /* 13 - AFE_CFG: WAVEGEN_EN = 1 ADC_CONV_EN = 1 SUPPLY_LPF_EN = 1                                                                   */
    0x003D08FE, /* 14 - Wait: 250 ms                                                                                                                */
    0x80020EF0, /* 15 - AFE_CFG: WAVEGEN_EN = 0 ADC_CONV_EN = 0 SUPPLY_LPF_EN = 0                                                                   */
    0x82000002  /* 16 - AFE_SEQ_CFG: SEQ_EN = 0                                                                                                     */
};

/* C++ linkage */
#ifdef __cplusplus
}
#endif

#endif /* include guard */

/*
** EOF
*/

/*@}*/
