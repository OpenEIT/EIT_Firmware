/*! \addtogroup AFE_Library AFE Library
 *  @{
 */

#ifndef __MODES_H__
#define __MODES_H__

/* C++ linkage */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
  
/***************************************************************************/
/*   Defines for Mode Tetrapolar Imaging                                               */
/***************************************************************************/
  /* Excitation frequency in Hz */
#define FREQ                        (25000)
/* Peak voltage in mV */
#define VPEAK                       (599)
/* RCAL value in Ohms */
#define RCAL                        (1000)
/* RTIA value in Ohms */
#define RTIA                        (33000)
/* Instrumentation Amplifier Gain */
#define INST_AMP_GAIN               (1.494)
/* FCW = FREQ * 2^26 / 16e6 */
#define FCW                         ((uint32_t)(((uint64_t)FREQ << 26) / 16000000 + 0.5))
/* DAC LSB size in mV = (1.6V / (2^12 - 1)) */
#define DAC_LSB_SIZE                (0.39072)
/* Sine amplitude in DAC codes */
#define SINE_AMPLITUDE              ((uint16_t)((VPEAK) / DAC_LSB_SIZE + 0.5))
/* If both real and imaginary result are within the interval (DFT_RESULTS_OPEN_MIN_THR, DFT_RESULTS_OPEN_MAX_THR),  */
/* it is considered an open circuit and results for both magnitude and phase will be 0.                             */
#define DFT_RESULTS_OPEN_MAX_THR    (1)
#define DFT_RESULTS_OPEN_MIN_THR    (-1)
/* The number of results expected from the DFT; each result has 4 numbers associated.  */
/* 2 current, 2 voltage */ 
#define DFT_RESULTS_COUNT           (4) 
/* this number DFT RESULTS COUNT will have to change */ 

/* Fractional LSB size for the fixed32_t type defined below, used for printing only. */
#define FIXED32_LSB_SIZE            (625)
  
#define MSG_MAXLEN_M3                  (400)
  
#define NUMBEROFMEASURES                (896) 


#define NO_MEASURES_32_OPPOSITION       (896) 
#define NO_MEASURES_16_OPPOSITION       (192) 
#define NO_MEASURES_8_OPPOSITION        (32)   
  
/***************************************************************************/
/*   Defines for Mode Timeseries                                               */
/***************************************************************************/
#define MSG_MAXLEN_M1               (50)
#define CLOCKS_PER_SECOND  (16000) /* how many clocks per second */
/***************************************************************************/
/*   Defines for Mode BIOIMPEDANCE SPECTROSCOPY                      */
/***************************************************************************/
#define MULTIFREQUENCY_ARRAY_SIZE   (15)
#define MSG_MAXLEN_M2               (50)
const uint64_t multifrequency[] = {200,500,800,1000,2000,5000,8000,10000,15000,20000,30000,40000,50000,60000,70000};
const char *stringfreqs[MULTIFREQUENCY_ARRAY_SIZE] = {"200","500","800","1000","2000","5000","8000","10000","15000","20000","30000","40000","50000","60000","70000"};  
 

/***************************************************************************/
/*   Defines for Bipolar                                                  */
/***************************************************************************/
/* Peak voltage in mV */
#define VPEAK_BIPOLAR                       (12.73)
/* Sine amplitude in DAC codes */
#define SINE_AMPLITUDE_BIPOLAR              ((uint16_t)((VPEAK * 40) / DAC_LSB_SIZE + 0.5))

/* If both real and imaginary result are within the interval (DFT_RESULTS_OPEN_MIN_THR, DFT_RESULTS_OPEN_MAX_THR),  */
/* it is considered an open circuit and results for both magnitude and phase will be 0.                             */
#define DFT_RESULTS_OPEN_MAX_THR_BIPOLAR    (10)
#define DFT_RESULTS_OPEN_MIN_THR_BIPOLAR    (-10)






/* C++ linkage */
#ifdef __cplusplus
}
#endif

#endif /* include guard */

/*
** EOF
*/

/*@}*/