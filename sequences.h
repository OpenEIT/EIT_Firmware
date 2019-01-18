/*! \addtogroup AFE_Library AFE Library
 *  Author: Jean Rintoul , Mindseye Biomedical LLC Copyright 2018
 */

#ifndef __SEQUENCES_H__
#define __SEQUENCES_H__

//#include "afe.h"

/* C++ linkage */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
  
/***************************************************************************/
/*   Sequences                                                             */
/***************************************************************************/
// 2 wire measurement on AFE7 and AFE8 
uint32_t seq_fast_2wire_bipolar[] = {
  
    13 << 16 | 0x43,     
    
    /* RCAL */
    0x86008811,   /* DMUX_STATE = 1, PMUX_STATE = 1, NMUX_STATE = 8, TMUX_STATE = 8 */
    0x00000640,   /* Wait 100us */
    0x80024EF0,   /* AFE_CFG: WAVEGEN_EN = 1 */
    0x00000C80,   /* Wait 200us */
    0x8002CFF0,   /* AFE_CFG: ADC_CONV_EN = 1, DFT_EN = 1 */
    0x00032340,   /* Wait 13ms */
    0x80024EF0,   /* AFE_CFG: ADC_CONV_EN = 0, DFT_EN = 0 */    
    
    /* AFE7-AFE8 */
    0x86007788,   /* DMUX_STATE = 8, PMUX_STATE = 8, NMUX_STATE = 7, TMUX_STATE = 7     */
    0x00000640,   /* Wait 100us */
    0x8002CFF0,   /* AFE_CFG: ADC_CONV_EN = 1, DFT_EN = 1 */
    0x00032340,   /* Wait 13ms */
    0x80020EF0,   /* AFE_CFG: WAVEGEN_EN = 0, ADC_CONV_EN = 0, DFT_EN = 0 */
    
    0x82000002,   /* AFE_SEQ_CFG: SEQ_EN = 0 */
    
};


// AFE POWER UP 
uint32_t seq_afe_poweritup_bipolar[] = {
    19 << 16 | 0x43,
    0x84005818,   /* AFE_FIFO_CFG: DATA_FIFO_SOURCE_SEL = 10 */
    0x8A000034,   /* AFE_WG_CFG: TYPE_SEL = 10 */
    0x98000000,   /* AFE_WG_CFG: SINE_FCW = 0 (placeholder, user programmable) */
    0x9E000000,   /* AFE_WG_AMPLITUDE: SINE_AMPLITUDE = 0 (placeholder, user programmable) */
    0x88000F01,   /* AFE_DAC_CFG: DAC_ATTEN_EN = 1 */
    0xA0000002,   /* AFE_ADC_CFG: MUX_SEL = 00010, GAIN_OFFS_SEL = 00 */
    /* RCAL */
    0x86008811,   /* DMUX_STATE = 1, PMUX_STATE = 1, NMUX_STATE = 8, TMUX_STATE = 8 */
    0x00000640,   /* Wait 100us */
    0x80024EF0,   /* AFE_CFG: WAVEGEN_EN = 1 */
    0x00000C80,   /* Wait 200us */
    0x8002CFF0,   /* AFE_CFG: ADC_CONV_EN = 1, DFT_EN = 1 */
    0x00032340,   /* Wait 13ms */
    0x80024EF0,   /* AFE_CFG: ADC_CONV_EN = 0, DFT_EN = 0 */
    
    /* AFE7-AFE8 */
    0x86007788,   /* DMUX_STATE = 8, PMUX_STATE = 8, NMUX_STATE = 7, TMUX_STATE = 7     */
    0x00000640,   /* Wait 100us */
    0x8002CFF0,   /* AFE_CFG: ADC_CONV_EN = 1, DFT_EN = 1 */
    0x00032340,   /* Wait 13ms */
    0x80020EF0,   /* AFE_CFG: WAVEGEN_EN = 0, ADC_CONV_EN = 0, DFT_EN = 0 */
    
    0x82000002,   /* AFE_SEQ_CFG: SEQ_EN = 0 */
        
};


  
  
/*
 * AFE powerup
 */
const uint32_t afe_powerup[] = {
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

/* Sequence for AC measurement, performs 4 DFTs:        */
/*     RCAL, AFE3-AFE4, AFE4-AFE5, AFE3-AFE5            */
uint32_t seq_afe_acmeas2wire[] = {
    0x001D0043,   /* Safety word: bits 31:16 = command count, bits 7:0 = CRC */
    0x84005818,   /* AFE_FIFO_CFG: DATA_FIFO_SOURCE_SEL = 10 */
    0x8A000034,   /* AFE_WG_CFG: TYPE_SEL = 10 */
    0x98000000,   /* AFE_WG_CFG: SINE_FCW = 0 (placeholder, user programmable) */
    0x9E000000,   /* AFE_WG_AMPLITUDE: SINE_AMPLITUDE = 0 (placeholder, user programmable) */
    0x88000F01,   /* AFE_DAC_CFG: DAC_ATTEN_EN = 1 */
    0xA0000002,   /* AFE_ADC_CFG: MUX_SEL = 00010, GAIN_OFFS_SEL = 00 */
    /* RCAL */
    0x86008811,   /* DMUX_STATE = 1, PMUX_STATE = 1, NMUX_STATE = 8, TMUX_STATE = 8 */
    0x00000640,   /* Wait 100us */
    0x80024EF0,   /* AFE_CFG: WAVEGEN_EN = 1 */
    0x00000C80,   /* Wait 200us */
    0x8002CFF0,   /* AFE_CFG: ADC_CONV_EN = 1, DFT_EN = 1 */
    0x00032340,   /* Wait 13ms */
    0x80024EF0,   /* AFE_CFG: ADC_CONV_EN = 0, DFT_EN = 0 */
    /* AFE3 - AFE4 */
    0x86003344,   /* DMUX_STATE = 3, PMUX_STATE = 3, NMUX_STATE = 4, TMUX_STATE = 4 */
    0x00000640,   /* Wait 100us */
    0x8002CFF0,   /* AFE_CFG: ADC_CONV_EN = 1, DFT_EN = 1 */
    0x00032340,   /* Wait 13ms */
    0x80024EF0,   /* AFE_CFG: ADC_CONV_EN = 0, DFT_EN = 0 */
    /* AFE4 - AFE5 */
    0x86004455,   /* DMUX_STATE = 4, PMUX_STATE = 4, NMUX_STATE = 5, TMUX_STATE = 5 */
    0x00000640,   /* Wait 100us */
    0x8002CFF0,   /* AFE_CFG: ADC_CONV_EN = 1, DFT_EN = 1 */
    0x00032340,   /* Wait 13ms */
    0x80024EF0,   /* AFE_CFG: ADC_CONV_EN = 0, DFT_EN = 0 */
    /* AFE3 - AFE5 */
    0x86003355,   /* DMUX_STATE = 3, PMUX_STATE = 3, NMUX_STATE = 5, TMUX_STATE = 5 */
    0x00000640,   /* Wait 100us */
    0x8002CFF0,   /* AFE_CFG: ADC_CONV_EN = 1, DFT_EN = 1 */
    0x00032340,   /* Wait 13ms */
    0x80020EF0,   /* AFE_CFG: WAVEGEN_EN = 0, ADC_CONV_EN = 0, DFT_EN = 0 */

    0x82000002,   /* AFE_SEQ_CFG: SEQ_EN = 0 */
};

/* 
This uses the AFE, all 8 channels to move around doing difference measurements in a circle. 

*/
uint32_t test_sequence1[] = {
 
    49 << 16 | 0x43,
    // 0x01260043,   /* Safety word: bits 31:16 = command count, bits 7:0 = CRC */
    // SAFEWORD CALCULATION: 
    // 14+ 35 = 49
    // 14 elements that aren't the main loop(intro, RCAL, end sequence)
    // 35 elements per AFE. 
    0x84005818,   /* AFE_FIFO_CFG: DATA_FIFO_SOURCE_SEL = 10 */
    0x8A000034,   /* AFE_WG_CFG: TYPE_SEL = 10 */
    0x98000000,   /* AFE_WG_CFG: SINE_FCW = 0 (placeholder, user programmable) */
    0x9E000000,   /* AFE_WG_AMPLITUDE: SINE_AMPLITUDE = 0 (placeholder, user programmable) */
    0x88000F01,   /* AFE_DAC_CFG: DAC_ATTEN_EN = 1 */
    0xA0000002,   /* AFE_ADC_CFG: MUX_SEL = 00010, GAIN_OFFS_SEL = 00 */
    /* RCAL */
    0x86008811,   /* DMUX_STATE = 1, PMUX_STATE = 1, NMUX_STATE = 8, TMUX_STATE = 8 */
    0x00000640,   /* Wait 100us */
    0x80024EF0,   /* AFE_CFG: WAVEGEN_EN = 1 */
    0x00000C80,   /* Wait 200us */
    0x8002CFF0,   /* AFE_CFG: ADC_CONV_EN = 1, DFT_EN = 1 */
    0x00032340,   /* Wait 13ms */
    0x80024EF0,   /* AFE_CFG: ADC_CONV_EN = 0, DFT_EN = 0 */
    
    // DFT_RESULTS COUNT SHOULD BE 114 [7*2 for RE + IMG parts and then the RCAL added = 14+2 = 16.]
    // one output, 7 inputs.   
    // 
    /* AFE1 - AFE2 */
    0x86001122,   /* DMUX_STATE = 1, PMUX_STATE = 1, NMUX_STATE = 2, TMUX_STATE = 2 */
    0x00000640,   /* Wait 100us */
    0x8002CFF0,   /* AFE_CFG: ADC_CONV_EN = 1, DFT_EN = 1 */
    0x00032340,   /* Wait 13ms */
    0x80024EF0,   /* AFE_CFG: ADC_CONV_EN = 0, DFT_EN = 0 */
    /* AFE1 - AFE3 */
    0x86001133,   /* DMUX_STATE = 1, PMUX_STATE = 1, NMUX_STATE = 3, TMUX_STATE = 3 */
    0x00000640,   /* Wait 100us */
    0x8002CFF0,   /* AFE_CFG: ADC_CONV_EN = 1, DFT_EN = 1 */
    0x00032340,   /* Wait 13ms */
    0x80024EF0,   /* AFE_CFG: ADC_CONV_EN = 0, DFT_EN = 0 */    
    /* AFE1 - AFE4 */
    0x86001144,   /* DMUX_STATE = 1, PMUX_STATE = 1, NMUX_STATE = 4, TMUX_STATE = 4 */
    0x00000640,   /* Wait 100us */
    0x8002CFF0,   /* AFE_CFG: ADC_CONV_EN = 1, DFT_EN = 1 */
    0x00032340,   /* Wait 13ms */
    0x80024EF0,   /* AFE_CFG: ADC_CONV_EN = 0, DFT_EN = 0 */        
    /* AFE1 - AFE5 */
    0x86001155,   /* DMUX_STATE = 1, PMUX_STATE = 1, NMUX_STATE = 5, TMUX_STATE = 5 */
    0x00000640,   /* Wait 100us */
    0x8002CFF0,   /* AFE_CFG: ADC_CONV_EN = 1, DFT_EN = 1 */
    0x00032340,   /* Wait 13ms */
    0x80024EF0,   /* AFE_CFG: ADC_CONV_EN = 0, DFT_EN = 0 */        
    /* AFE1 - AFE6 */
    0x86001166,   /* DMUX_STATE = 1, PMUX_STATE = 1, NMUX_STATE = 6, TMUX_STATE = 6 */
    0x00000640,   /* Wait 100us */
    0x8002CFF0,   /* AFE_CFG: ADC_CONV_EN = 1, DFT_EN = 1 */
    0x00032340,   /* Wait 13ms */
    0x80024EF0,   /* AFE_CFG: ADC_CONV_EN = 0, DFT_EN = 0 */      
    /* AFE1 - AFE7 */
    0x86001177,   /* DMUX_STATE = 1, PMUX_STATE = 1, NMUX_STATE = 7, TMUX_STATE = 7 */
    0x00000640,   /* Wait 100us */
    0x8002CFF0,   /* AFE_CFG: ADC_CONV_EN = 1, DFT_EN = 1 */
    0x00032340,   /* Wait 13ms */
    0x80024EF0,   /* AFE_CFG: ADC_CONV_EN = 0, DFT_EN = 0 */          
    /* AFE1 - AFE8 */
    0x86001188,   /* DMUX_STATE = 1, PMUX_STATE = 1, NMUX_STATE = 8, TMUX_STATE = 8 */
    0x00000640,   /* Wait 100us */
    0x8002CFF0,   /* AFE_CFG: ADC_CONV_EN = 1, DFT_EN = 1 */
    0x00032340,   /* Wait 13ms */
    0x80024EF0,   /* AFE_CFG: ADC_CONV_EN = 0, DFT_EN = 0 */      
    
    /* sequence end. */
    0x82000002,   /* AFE_SEQ_CFG: SEQ_EN = 0 */
};

/* 
This uses the AFE, all 8 channels to move around doing difference measurements in a circle. 

*/
uint32_t seq_afe_mux1[] = {
 
    154 << 16 | 0x43,
    // 0x01260043,   /* Safety word: bits 31:16 = command count, bits 7:0 = CRC */
    // SAFEWORD CALCULATION: 
    // 14 elements that aren't the main loop(intro, RCAL, end sequence)
    // 28 unique combinations of 8 electrodes. 
    // 28*5 + 14 = 42. Hex of 154 
    0x84005818,   /* AFE_FIFO_CFG: DATA_FIFO_SOURCE_SEL = 10 */
    0x8A000034,   /* AFE_WG_CFG: TYPE_SEL = 10 */
    0x98000000,   /* AFE_WG_CFG: SINE_FCW = 0 (placeholder, user programmable) */
    0x9E000000,   /* AFE_WG_AMPLITUDE: SINE_AMPLITUDE = 0 (placeholder, user programmable) */
    0x88000F01,   /* AFE_DAC_CFG: DAC_ATTEN_EN = 1 */
    0xA0000002,   /* AFE_ADC_CFG: MUX_SEL = 00010, GAIN_OFFS_SEL = 00 */
    /* RCAL */
    0x86008811,   /* DMUX_STATE = 1, PMUX_STATE = 1, NMUX_STATE = 8, TMUX_STATE = 8 */
    0x00000640,   /* Wait 100us */
    0x80024EF0,   /* AFE_CFG: WAVEGEN_EN = 1 */
    0x00000C80,   /* Wait 200us */
    0x8002CFF0,   /* AFE_CFG: ADC_CONV_EN = 1, DFT_EN = 1 */
    0x00032340,   /* Wait 13ms */
    0x80024EF0,   /* AFE_CFG: ADC_CONV_EN = 0, DFT_EN = 0 */
    
    // DFT_RESULTS COUNT SHOULD BE (28+1) * 2(re+img) = 58
    /* AFE1 - AFE2 */
    0x86001122,   /* DMUX_STATE = 1, PMUX_STATE = 1, NMUX_STATE = 2, TMUX_STATE = 2 */
    0x00000640,   /* Wait 100us */
    0x8002CFF0,   /* AFE_CFG: ADC_CONV_EN = 1, DFT_EN = 1 */
    0x00032340,   /* Wait 13ms */
    0x80024EF0,   /* AFE_CFG: ADC_CONV_EN = 0, DFT_EN = 0 */
    /* AFE1 - AFE3 */
    0x86001133,   /* DMUX_STATE = 1, PMUX_STATE = 1, NMUX_STATE = 3, TMUX_STATE = 3 */
    0x00000640,   /* Wait 100us */
    0x8002CFF0,   /* AFE_CFG: ADC_CONV_EN = 1, DFT_EN = 1 */
    0x00032340,   /* Wait 13ms */
    0x80024EF0,   /* AFE_CFG: ADC_CONV_EN = 0, DFT_EN = 0 */    
    /* AFE1 - AFE4 */
    0x86001144,   /* DMUX_STATE = 1, PMUX_STATE = 1, NMUX_STATE = 4, TMUX_STATE = 4 */
    0x00000640,   /* Wait 100us */
    0x8002CFF0,   /* AFE_CFG: ADC_CONV_EN = 1, DFT_EN = 1 */
    0x00032340,   /* Wait 13ms */
    0x80024EF0,   /* AFE_CFG: ADC_CONV_EN = 0, DFT_EN = 0 */        
    /* AFE1 - AFE5 */
    0x86001155,   /* DMUX_STATE = 1, PMUX_STATE = 1, NMUX_STATE = 5, TMUX_STATE = 5 */
    0x00000640,   /* Wait 100us */
    0x8002CFF0,   /* AFE_CFG: ADC_CONV_EN = 1, DFT_EN = 1 */
    0x00032340,   /* Wait 13ms */
    0x80024EF0,   /* AFE_CFG: ADC_CONV_EN = 0, DFT_EN = 0 */        
    /* AFE1 - AFE6 */
    0x86001166,   /* DMUX_STATE = 1, PMUX_STATE = 1, NMUX_STATE = 6, TMUX_STATE = 6 */
    0x00000640,   /* Wait 100us */
    0x8002CFF0,   /* AFE_CFG: ADC_CONV_EN = 1, DFT_EN = 1 */
    0x00032340,   /* Wait 13ms */
    0x80024EF0,   /* AFE_CFG: ADC_CONV_EN = 0, DFT_EN = 0 */      
    /* AFE1 - AFE7 */
    0x86001177,   /* DMUX_STATE = 1, PMUX_STATE = 1, NMUX_STATE = 7, TMUX_STATE = 7 */
    0x00000640,   /* Wait 100us */
    0x8002CFF0,   /* AFE_CFG: ADC_CONV_EN = 1, DFT_EN = 1 */
    0x00032340,   /* Wait 13ms */
    0x80024EF0,   /* AFE_CFG: ADC_CONV_EN = 0, DFT_EN = 0 */          
    /* AFE1 - AFE8 */
    0x86001188,   /* DMUX_STATE = 1, PMUX_STATE = 1, NMUX_STATE = 8, TMUX_STATE = 8 */
    0x00000640,   /* Wait 100us */
    0x8002CFF0,   /* AFE_CFG: ADC_CONV_EN = 1, DFT_EN = 1 */
    0x00032340,   /* Wait 13ms */
    0x80024EF0,   /* AFE_CFG: ADC_CONV_EN = 0, DFT_EN = 0 */      
 
    //////
    /* AFE2 - AFE3 */
    0x86002233,   /* DMUX_STATE = 1, PMUX_STATE = 1, NMUX_STATE = 2, TMUX_STATE = 2 */
    0x00000640,   /* Wait 100us */
    0x8002CFF0,   /* AFE_CFG: ADC_CONV_EN = 1, DFT_EN = 1 */
    0x00032340,   /* Wait 13ms */
    0x80024EF0,   /* AFE_CFG: ADC_CONV_EN = 0, DFT_EN = 0 */
    /* AFE2 - AFE4 */
    0x86002244,   /* DMUX_STATE = 1, PMUX_STATE = 1, NMUX_STATE = 3, TMUX_STATE = 3 */
    0x00000640,   /* Wait 100us */
    0x8002CFF0,   /* AFE_CFG: ADC_CONV_EN = 1, DFT_EN = 1 */
    0x00032340,   /* Wait 13ms */
    0x80024EF0,   /* AFE_CFG: ADC_CONV_EN = 0, DFT_EN = 0 */    
    /* AFE2 - AFE5 */
    0x86002255,   /* DMUX_STATE = 1, PMUX_STATE = 1, NMUX_STATE = 4, TMUX_STATE = 4 */
    0x00000640,   /* Wait 100us */
    0x8002CFF0,   /* AFE_CFG: ADC_CONV_EN = 1, DFT_EN = 1 */
    0x00032340,   /* Wait 13ms */
    0x80024EF0,   /* AFE_CFG: ADC_CONV_EN = 0, DFT_EN = 0 */        
    /* AFE2 - AFE6 */
    0x86002266,   /* DMUX_STATE = 1, PMUX_STATE = 1, NMUX_STATE = 5, TMUX_STATE = 5 */
    0x00000640,   /* Wait 100us */
    0x8002CFF0,   /* AFE_CFG: ADC_CONV_EN = 1, DFT_EN = 1 */
    0x00032340,   /* Wait 13ms */
    0x80024EF0,   /* AFE_CFG: ADC_CONV_EN = 0, DFT_EN = 0 */        
    /* AFE2 - AFE7 */
    0x86002277,   /* DMUX_STATE = 1, PMUX_STATE = 1, NMUX_STATE = 6, TMUX_STATE = 6 */
    0x00000640,   /* Wait 100us */
    0x8002CFF0,   /* AFE_CFG: ADC_CONV_EN = 1, DFT_EN = 1 */
    0x00032340,   /* Wait 13ms */
    0x80024EF0,   /* AFE_CFG: ADC_CONV_EN = 0, DFT_EN = 0 */      
    /* AFE2 - AFE8 */
    0x86002288,   /* DMUX_STATE = 1, PMUX_STATE = 1, NMUX_STATE = 7, TMUX_STATE = 7 */
    0x00000640,   /* Wait 100us */
    0x8002CFF0,   /* AFE_CFG: ADC_CONV_EN = 1, DFT_EN = 1 */
    0x00032340,   /* Wait 13ms */
    0x80024EF0,   /* AFE_CFG: ADC_CONV_EN = 0, DFT_EN = 0 */          

    //////

    /* AFE3 - AFE4 */
    0x86003344,   /* DMUX_STATE = 1, PMUX_STATE = 1, NMUX_STATE = 3, TMUX_STATE = 3 */
    0x00000640,   /* Wait 100us */
    0x8002CFF0,   /* AFE_CFG: ADC_CONV_EN = 1, DFT_EN = 1 */
    0x00032340,   /* Wait 13ms */
    0x80024EF0,   /* AFE_CFG: ADC_CONV_EN = 0, DFT_EN = 0 */    
    /* AFE3 - AFE5 */
    0x86003355,   /* DMUX_STATE = 1, PMUX_STATE = 1, NMUX_STATE = 4, TMUX_STATE = 4 */
    0x00000640,   /* Wait 100us */
    0x8002CFF0,   /* AFE_CFG: ADC_CONV_EN = 1, DFT_EN = 1 */
    0x00032340,   /* Wait 13ms */
    0x80024EF0,   /* AFE_CFG: ADC_CONV_EN = 0, DFT_EN = 0 */        
    /* AFE3 - AFE6 */
    0x86003366,   /* DMUX_STATE = 1, PMUX_STATE = 1, NMUX_STATE = 5, TMUX_STATE = 5 */
    0x00000640,   /* Wait 100us */
    0x8002CFF0,   /* AFE_CFG: ADC_CONV_EN = 1, DFT_EN = 1 */
    0x00032340,   /* Wait 13ms */
    0x80024EF0,   /* AFE_CFG: ADC_CONV_EN = 0, DFT_EN = 0 */        
    /* AFE3 - AFE7 */
    0x86003377,   /* DMUX_STATE = 1, PMUX_STATE = 1, NMUX_STATE = 6, TMUX_STATE = 6 */
    0x00000640,   /* Wait 100us */
    0x8002CFF0,   /* AFE_CFG: ADC_CONV_EN = 1, DFT_EN = 1 */
    0x00032340,   /* Wait 13ms */
    0x80024EF0,   /* AFE_CFG: ADC_CONV_EN = 0, DFT_EN = 0 */      
    /* AFE3 - AFE8 */
    0x86003388,   /* DMUX_STATE = 1, PMUX_STATE = 1, NMUX_STATE = 7, TMUX_STATE = 7 */
    0x00000640,   /* Wait 100us */
    0x8002CFF0,   /* AFE_CFG: ADC_CONV_EN = 1, DFT_EN = 1 */
    0x00032340,   /* Wait 13ms */
    0x80024EF0,   /* AFE_CFG: ADC_CONV_EN = 0, DFT_EN = 0 */        
    
    //////
    /* AFE4 - AFE5 */
    0x86004455,   /* DMUX_STATE = 1, PMUX_STATE = 1, NMUX_STATE = 4, TMUX_STATE = 4 */
    0x00000640,   /* Wait 100us */
    0x8002CFF0,   /* AFE_CFG: ADC_CONV_EN = 1, DFT_EN = 1 */
    0x00032340,   /* Wait 13ms */
    0x80024EF0,   /* AFE_CFG: ADC_CONV_EN = 0, DFT_EN = 0 */        
    /* AFE4 - AFE6 */
    0x86004466,   /* DMUX_STATE = 1, PMUX_STATE = 1, NMUX_STATE = 5, TMUX_STATE = 5 */
    0x00000640,   /* Wait 100us */
    0x8002CFF0,   /* AFE_CFG: ADC_CONV_EN = 1, DFT_EN = 1 */
    0x00032340,   /* Wait 13ms */
    0x80024EF0,   /* AFE_CFG: ADC_CONV_EN = 0, DFT_EN = 0 */        
    /* AFE4 - AFE7 */
    0x86004477,   /* DMUX_STATE = 1, PMUX_STATE = 1, NMUX_STATE = 6, TMUX_STATE = 6 */
    0x00000640,   /* Wait 100us */
    0x8002CFF0,   /* AFE_CFG: ADC_CONV_EN = 1, DFT_EN = 1 */
    0x00032340,   /* Wait 13ms */
    0x80024EF0,   /* AFE_CFG: ADC_CONV_EN = 0, DFT_EN = 0 */      
    /* AFE4 - AFE8 */
    0x86003388,   /* DMUX_STATE = 1, PMUX_STATE = 1, NMUX_STATE = 7, TMUX_STATE = 7 */
    0x00000640,   /* Wait 100us */
    0x8002CFF0,   /* AFE_CFG: ADC_CONV_EN = 1, DFT_EN = 1 */
    0x00032340,   /* Wait 13ms */
    0x80024EF0,   /* AFE_CFG: ADC_CONV_EN = 0, DFT_EN = 0 */ 

    //////
           
    /* AFE5 - AFE6 */
    0x86005566,   /* DMUX_STATE = 1, PMUX_STATE = 1, NMUX_STATE = 5, TMUX_STATE = 5 */
    0x00000640,   /* Wait 100us */
    0x8002CFF0,   /* AFE_CFG: ADC_CONV_EN = 1, DFT_EN = 1 */
    0x00032340,   /* Wait 13ms */
    0x80024EF0,   /* AFE_CFG: ADC_CONV_EN = 0, DFT_EN = 0 */        
    /* AFE5 - AFE7 */
    0x86005577,   /* DMUX_STATE = 1, PMUX_STATE = 1, NMUX_STATE = 6, TMUX_STATE = 6 */
    0x00000640,   /* Wait 100us */
    0x8002CFF0,   /* AFE_CFG: ADC_CONV_EN = 1, DFT_EN = 1 */
    0x00032340,   /* Wait 13ms */
    0x80024EF0,   /* AFE_CFG: ADC_CONV_EN = 0, DFT_EN = 0 */      
    /* AFE5 - AFE8 */
    0x86005588,   /* DMUX_STATE = 1, PMUX_STATE = 1, NMUX_STATE = 7, TMUX_STATE = 7 */
    0x00000640,   /* Wait 100us */
    0x8002CFF0,   /* AFE_CFG: ADC_CONV_EN = 1, DFT_EN = 1 */
    0x00032340,   /* Wait 13ms */
    0x80024EF0,   /* AFE_CFG: ADC_CONV_EN = 0, DFT_EN = 0 */ 
    
    //////
   
    /* AFE6 - AFE7 */
    0x86006677,   /* DMUX_STATE = 1, PMUX_STATE = 1, NMUX_STATE = 6, TMUX_STATE = 6 */
    0x00000640,   /* Wait 100us */
    0x8002CFF0,   /* AFE_CFG: ADC_CONV_EN = 1, DFT_EN = 1 */
    0x00032340,   /* Wait 13ms */
    0x80024EF0,   /* AFE_CFG: ADC_CONV_EN = 0, DFT_EN = 0 */      
    /* AFE6 - AFE8 */
    0x86006688,   /* DMUX_STATE = 1, PMUX_STATE = 1, NMUX_STATE = 7, TMUX_STATE = 7 */
    0x00000640,   /* Wait 100us */
    0x8002CFF0,   /* AFE_CFG: ADC_CONV_EN = 1, DFT_EN = 1 */
    0x00032340,   /* Wait 13ms */
    0x80024EF0,   /* AFE_CFG: ADC_CONV_EN = 0, DFT_EN = 0 */     
    
    //////
   
    /* AFE7 - AFE8 */
    0x86007788,   /* DMUX_STATE = 1, PMUX_STATE = 1, NMUX_STATE = 7, TMUX_STATE = 7 */
    0x00000640,   /* Wait 100us */
    0x8002CFF0,   /* AFE_CFG: ADC_CONV_EN = 1, DFT_EN = 1 */
    0x00032340,   /* Wait 13ms */
    0x80024EF0,   /* AFE_CFG: ADC_CONV_EN = 0, DFT_EN = 0 */        
      
    /* sequence end. */
    0x82000002,   /* AFE_SEQ_CFG: SEQ_EN = 0 */
};


// with speed modification for tetrapolar measurement. 
uint32_t seq_afe_poweritup[] = {
    22 << 16 | 0x43,
    0x84005818,   /* AFE_FIFO_CFG: DATA_FIFO_SOURCE_SEL = 10                                */
    0x8A000034,   /* AFE_WG_CFG: TYPE_SEL = 10                                              */
    0x98000000,   /* AFE_WG_CFG: SINE_FCW = 0 (placeholder, user programmable)              */
    0x9E000000,   /* AFE_WG_AMPLITUDE: SINE_AMPLITUDE = 0 (placeholder, user programmable)  */
    0x88000F00,   /* DAC_CFG: DAC_ATTEN_EN = 0                                              */

    /* TIA  */
    0x86007788,   /* DMUX_STATE = 8, PMUX_STATE = 8, NMUX_STATE = 7, TMUX_STATE = 7         */
    0xA0000002,   /* AFE_ADC_CFG: TIA, no bypass, offset and gain correction.               */
    0x0080E800,   /* Wait 528ms.                                                            */
                  /* This is the worst case settling time:                                  */
                  /* Rcm=10M, Ciso=22nF(20%tol) => settling time = 2*RC = 528 ms            */
                  /* This settling time is only required the first time the switches are    */
                  /* closed.                                                                */
    0x80024EF0,   /* AFE_CFG: WAVEGEN_EN = 1                                                */
    0x00000C80,   /* Wait 200us                                                             */
    0x8002CFF0,   /* AFE_CFG: ADC_CONV_EN = 1, DFT_EN = 1                                   */
    0x00032340,   /* Wait 13ms ( -148us to stop at midscale)                                */
    0x80020EF0,   /* AFE_CFG: ADC_CONV_EN = 0, DFT_EN = 0                                   */
    /* AN_A */
    0xA0000208,   /* AFE_ADC_CFG: AN_A, Use GAIN and OFFSET AUX                             */
    0x00000640,   /* Wait 100us                                                             */
    0x80024EF0,   /* AFE_CFG: WAVEGEN_EN = 1                                                */
    0x00000C80,   /* Wait 200us                                                             */
    0x8002CFF0,   /* AFE_CFG: ADC_CONV_EN = 1, DFT_EN = 1                                   */
    0x00032340,   /* Wait 13ms                                                              */
    0x80020EF0,   /* AFE_CFG: WAVEGEN_EN, ADC_CONV_EN = 0, DFT_EN = 0                       */
    0x86007788,   /* DMUX_STATE = 0, PMUX_STATE = 0, NMUX_STATE = 0, TMUX_STATE = 0         */
    
    0x82000002,   /* AFE_SEQ_CFG: SEQ_EN = 0                                                */

};

uint32_t seq_afe_fast_meas_4wire[] = {
  
    17 << 16 | 0x43,                                 
    /* TIA  */
    0x86007788,   /* DMUX_STATE = 8, PMUX_STATE = 8, NMUX_STATE = 7, TMUX_STATE = 7         */
    0xA0000002,   /* AFE_ADC_CFG: TIA, no bypass, offset and gain correction.               */
    0x00000640,   /* wait 100us */ 
    //0x00027100,    /* wait 10ms */
    0x80024EF0,   /* AFE_CFG: WAVEGEN_EN = 1                                                */
    0x00000C80,   /* Wait 200us                                                             */
    0x8002CFF0,   /* AFE_CFG: ADC_CONV_EN = 1, DFT_EN = 1                                   */
    0x00032340,   /* Wait 13ms ( -148us to stop at midscale)                                */
    0x80020EF0,   /* AFE_CFG: ADC_CONV_EN = 0, DFT_EN = 0                                   */
    
    /* AN_A */
    0xA0000208,   /* AFE_ADC_CFG: AN_A, Use GAIN and OFFSET AUX                             */
    0x00000640,   /* Wait 100us                                                             */
    0x80024EF0,   /* AFE_CFG: WAVEGEN_EN = 1                                                */
    0x00032340,   /* Wait 13ms                                                              */    
    // 0x00000C80,   /* Wait 200us                                                             */
    0x8002CFF0,   /* AFE_CFG: ADC_CONV_EN = 1, DFT_EN = 1                                   */
    0x00032340,   /* Wait 13ms                                                              */
    0x80020EF0,   /* AFE_CFG: WAVEGEN_EN, ADC_CONV_EN = 0, DFT_EN = 0                       */
    0x86007788,   /* DMUX_STATE = 0, PMUX_STATE = 0, NMUX_STATE = 0, TMUX_STATE = 0         */
    0x82000002,   /* AFE_SEQ_CFG: SEQ_EN = 0                                                */
};


/* using this on in BIS */ 
uint32_t seq_afe_fast_acmeasBioZ_4wire[] = {
  
    0x0016001A,   /* Safety word: bits 31:16 = command count, bits 7:0 = CRC                */
    0x84005818,   /* AFE_FIFO_CFG: DATA_FIFO_SOURCE_SEL = 10                                */
    0x8A000034,   /* AFE_WG_CFG: TYPE_SEL = 10                                              */
    0x98000000,   /* AFE_WG_CFG: SINE_FCW = 0 (placeholder, user programmable)              */
    0x9E000000,   /* AFE_WG_AMPLITUDE: SINE_AMPLITUDE = 0 (placeholder, user programmable)  */
    0x88000F00,   /* DAC_CFG: DAC_ATTEN_EN = 0                                              */

    /* TIA  */
    0x86007788,   /* DMUX_STATE = 8, PMUX_STATE = 8, NMUX_STATE = 7, TMUX_STATE = 7         */
    0xA0000002,   /* AFE_ADC_CFG: TIA, no bypass, offset and gain correction.               */
    // 0x00000640,   /* wait 100us */ 
    0x00003E80,      /* 20 ms */   
    //0x0080E800,   /* Wait 528ms.                                                            */
                  /* This is the worst case settling time:                                  */
                  /* Rcm=10M, Ciso=22nF(20%tol) => settling time = 2*RC = 528 ms            */
                  /* This settling time is only required the first time the switches are    */
                  /* closed.                                                                */
    0x80024EF0,   /* AFE_CFG: WAVEGEN_EN = 1                                                */
    0x00000C80,   /* Wait 200us                                                             */
    0x8002CFF0,   /* AFE_CFG: ADC_CONV_EN = 1, DFT_EN = 1                                   */
    0x00032340,   /* Wait 13ms ( -148us to stop at midscale)                                */
    0x80020EF0,   /* AFE_CFG: ADC_CONV_EN = 0, DFT_EN = 0                                   */
    
    /* AN_A */
    0xA0000208,   /* AFE_ADC_CFG: AN_A, Use GAIN and OFFSET AUX                             */
    0x00000640,   /* Wait 100us                                                             */
    0x80024EF0,   /* AFE_CFG: WAVEGEN_EN = 1                                                */
    0x00000C80,   /* Wait 200us                                                             */
    0x8002CFF0,   /* AFE_CFG: ADC_CONV_EN = 1, DFT_EN = 1                                   */
    0x00032340,   /* Wait 13ms                                                              */
    0x80020EF0,   /* AFE_CFG: WAVEGEN_EN, ADC_CONV_EN = 0, DFT_EN = 0                       */
    0x86007788,   /* DMUX_STATE = 0, PMUX_STATE = 0, NMUX_STATE = 0, TMUX_STATE = 0         */
    0x82000002,   /* AFE_SEQ_CFG: SEQ_EN = 0                                                */
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