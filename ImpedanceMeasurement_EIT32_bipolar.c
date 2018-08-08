
/*****************************************************************************
 * @file:    ImpedanceMeasurement_4Wire.c
 * @brief:   Impedance measurement example for ADuCM350 in 4-wire configuration
 *           Bio-Impedance measurement. The ADuCM350 4Wire Bio Config Board is
 *           is required in this example
 * @version: $Revision: 29043 $
 * @date:    $Date: 2014-12-08 08:53:03 -0500 (Mon, 08 Dec 2014) $
 *****************************************************************************/

#include <stdio.h>
#include "arm_math.h"
#include "test_common.h"
#include "afe.h"
#include "afe_lib.h"
#include "uart.h"
#include "sequences.h"
#include "lookup.h" 
#include "gpio.h"


/* Macro to enable the returning of AFE data using the UART */
/*      1 = return AFE data on UART                         */
/*      0 = return AFE data on SW (Std Output)              */
#define USE_UART_FOR_DATA           (1)


/* START BIPOLAR SPECIFIC VALUES */
/* If both real and imaginary result are within the interval (DFT_RESULTS_OPEN_MIN_THR, DFT_RESULTS_OPEN_MAX_THR),  */
/* it is considered an open circuit and results for both magnitude and phase will be 0.                             */
#define DFT_RESULTS_OPEN_MAX_THR    (10)
#define DFT_RESULTS_OPEN_MIN_THR    (-10)

/* Excitation frequency in Hz */
#define FREQ                        (10000)
/* Peak voltage in mV */
#define VPEAK                       (12.73)
/* RCAL value, in ohms */
#define RCAL                        (1000)

/* FCW = FREQ * 2^26 / 16e6 */
#define FCW                         ((uint32_t)(((uint64_t)FREQ << 26) / 16000000 + 0.5))

/* DAC LSB size in mV, before attenuator (1.6V / (2^12 - 1)) */
#define DAC_LSB_SIZE                (0.39072)

/* Sine amplitude in DAC codes */
#define SINE_AMPLITUDE              ((uint16_t)((VPEAK * 40) / DAC_LSB_SIZE + 0.5))
//* END BIPOLAR SPECIFIC VALUES

/* The number of results expected from the DFT; each result has 4 numbers associated.  */
/* 2 current, 2 voltage */ 
#define DFT_RESULTS_COUNT           (4) 
/* this number DFT RESULTS COUNT will have to change */ 
#define NUMBEROFMEASURES                (928) //  29 x 32 = 928 

#define RUNNING    (1) /* defaults to yes it's running */

/* Fractional LSB size for the fixed32_t type defined below, used for printing only. */
#define FIXED32_LSB_SIZE            (625)

#define MSG_MAXLEN                  (400)
/* Helper macro for printing strings to UART or Std. Output */
#define PRINT(s)                    test_print(s)

/* Custom fixed-point type used for final results,              */
/* to keep track of the decimal point position.                 */
/* Signed number with 28 integer bits and 4 fractional bits.    */
typedef union {
    int32_t     full;
    struct {
        uint8_t fpart:4;
        int32_t ipart:28;
    } parts;
} fixed32_t;

ADI_UART_HANDLE     hUartDevice     = NULL;

/* Function prototypes */
q15_t                   arctan                  (q15_t imag, q15_t real);
fixed32_t calculate_magnitude(q31_t magnitude_1, q31_t magnitude_2, uint32_t res);
fixed32_t               calculate_phase         (q15_t phase_rcal, q15_t phase_z);
void                    convert_dft_results     (int16_t *dft_results, q15_t *dft_results_q15, q31_t *dft_results_q31);
void                    sprintf_fixed32         (char *out, fixed32_t in);
void                    print_MagnitudePhase    (char *text, fixed32_t magnitude, fixed32_t phase);
void                    test_print              (char *pBuffer);
ADI_UART_RESULT_TYPE    uart_Init               (void);
ADI_UART_RESULT_TYPE    uart_UnInit             (void);
void                    delay                   (uint32_t counts);
extern int32_t          adi_initpinmux          (void);
void                    multiplex_adg732        (ADI_AFE_DEV_HANDLE  hDevice, const uint32_t *const seq);
void                    init_GPIO_ports         (void);
//void                    bipolar_measurement     (ADI_AFE_DEV_HANDLE  hDevice, const uint32_t *const seq,uint16_t *dft_results);
fixed32_t               calculate_bipolar_magnitude     (q31_t magnitude_rcal, q31_t magnitude_z);



/* Sequence for AC measurement, performs 1 DFTs: */
/*     RCAL, AFE7-AFE8           */
uint32_t seq_afe_2wiretest[] = {
  
    19 << 16 | 0x43,
    // 0x01260043,   /* Safety word: bits 31:16 = command count, bits 7:0 = CRC */
    // SAFEWORD CALCULATION: 
    // 14 elements that aren't the main loop(intro, RCAL, end sequence)
    // 28 unique combinations of 8 electrodes. 
    // 1*5 + 14 = . Hex of 19   
    //0x001D0043,   /* Safety word: bits 31:16 = command count, bits 7:0 = CRC */
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
    
    /* AFE7 - AFE8 */
    0x86007788,   
    0x00000640,   /* Wait 100us */
    0x8002CFF0,   /* AFE_CFG: ADC_CONV_EN = 1, DFT_EN = 1 */
    0x00032340,   /* Wait 13ms */
    0x80024EF0,   /* AFE_CFG: ADC_CONV_EN = 0, DFT_EN = 0 */        
      
    /* sequence end. */
    0x82000002,   /* AFE_SEQ_CFG: SEQ_EN = 0 */
};



int main(void) {
  
    ADI_AFE_DEV_HANDLE  hDevice;
  
    int16_t             dft_results[DFT_RESULTS_COUNT];
    q15_t               dft_results_q15[DFT_RESULTS_COUNT];
    q31_t               dft_results_q31[DFT_RESULTS_COUNT];
    q31_t               magnitude[DFT_RESULTS_COUNT / 2];
    fixed32_t           magnitude_result[DFT_RESULTS_COUNT / 2 - 1]; 
    uint32_t econf = 0;
    int8_t              i =0;      
    int8_t              running;
    running = RUNNING;
      

    /* Initialize system */
    SystemInit();
    /* Change the system clock source to HFXTAL and change clock frequency to 16MHz     */
    /* Requirement for AFE (ACLK)                                                       */
    if (ADI_SYS_SUCCESS != SystemTransitionClocks(ADI_SYS_CLOCK_TRIGGER_MEASUREMENT_ON))
    {
        PRINT("SystemTransitionClocks");
    }
    
    /* SPLL with 32MHz used, need to divide by 2 */
    SetSystemClockDivider(ADI_SYS_CLOCK_UART, 2);
    
    /* Test initialization */
    test_Init();
    
    /* Initialize GPIO */
    if (ADI_GPIO_SUCCESS != adi_GPIO_Init())
    {
        PRINT("adi_GPIO_Init went wrong");
    }
    /* Enable GPIO output drivers */
    init_GPIO_ports();
    
    /* Initialize static pinmuxing */
    adi_initpinmux();

    /* Initialize the UART for transferring measurement data out */
    if (ADI_UART_SUCCESS != uart_Init())
    {
        PRINT("uart_Init failed ");
    }
    PRINT("UART test\n");
    
    /* Initialize the AFE API */
    if (ADI_AFE_SUCCESS != adi_AFE_Init(&hDevice)) 
    {
        PRINT("Init");
    }
    /* AFE power up */
    if (ADI_AFE_SUCCESS != adi_AFE_PowerUp(hDevice)) 
    {
        PRINT("adi_AFE_PowerUp");
    }

    /* Excitation Channel Power-Up */
    if (ADI_AFE_SUCCESS != adi_AFE_ExciteChanPowerUp(hDevice)) 
    {
        PRINT("adi_AFE_ExciteChanPowerUp");
    }
    
    /* Update FCW in the sequence */
     seq_afe_2wiretest[3] = SEQ_MMR_WRITE(REG_AFE_AFE_WG_FCW, FCW);
     /* Update sine amplitude in the sequence */
     seq_afe_2wiretest[4] = SEQ_MMR_WRITE(REG_AFE_AFE_WG_AMPLITUDE, SINE_AMPLITUDE);      
   /* END BIPOLAR MEASUREMENT ONLY CODE */ 
    
    /* Recalculate CRC in software for the AC measurement, because we changed   */
    /* FCW and sine amplitude settings.                                         */
    adi_AFE_EnableSoftwareCRC(hDevice, true);

    PRINT("READY TO START FOR LOOP\n");     
    //ADI_GPIO_RESULT_TYPE r,s, t;
    
    // Enable all 4 multiplexers. 
    adi_GPIO_SetLow(ADI_GPIO_PORT_1,ADI_GPIO_PIN_5);  // A minus - M1 
    adi_GPIO_SetLow(ADI_GPIO_PORT_1,ADI_GPIO_PIN_11);  // A plus  - M2 DISABLED
    adi_GPIO_SetLow(ADI_GPIO_PORT_3,ADI_GPIO_PIN_12);  // V minus - M3 
    adi_GPIO_SetLow(ADI_GPIO_PORT_2,ADI_GPIO_PIN_6);   // V plus  - M4 DISABLED
    
    
    if ( !(adi_GPIO_GetOutputEnable(ADI_GPIO_PORT_1) & ADI_GPIO_PIN_5) ) {
      
      if (!(adi_GPIO_GetPullUpEnable(ADI_GPIO_PORT_1) & ADI_GPIO_PIN_5)) {
        PRINT("SYSTEM ENABLED CORRECTLY");
      }  
      else {
        PRINT("SYSTEM NOT ENABLED CORRECTLY");
      }
    }
    else {
        PRINT("ITS GOOD\n");
    }
      
    
    int run_max         = 10000;        // 
    int run_iterator    = 0;
         
    while (running) // running
    {
      
      char                msg[MSG_MAXLEN]={0};
      char                tmp[MSG_MAXLEN]={0};
      sprintf(msg, "    %s : ", "magnitudes");
      for (econf = 0;econf<1;econf++) {   
        
        
//        if ( !(adi_GPIO_GetOutputEnable(ADI_GPIO_PORT_1) & ADI_GPIO_PIN_5) ) {
//          
//          if (!(adi_GPIO_GetPullUpEnable(ADI_GPIO_PORT_1) & ADI_GPIO_PIN_5)) {
//            PRINT("SYSTEM ENABLED CORRECTLY");
//          }  
//          else {
//            PRINT("SYSTEM NOT ENABLED CORRECTLY");
//          }
//        }
//        else {
//          PRINT("ITS GOOD\n");
//        }
        
        sprintf(tmp,"econf %u :", econf);
        strcat(msg,tmp);
        
        int16_t* e = (int16_t *)electrode_configuration[econf];
        int16_t* mx1_assignment = (int16_t *)truth_table[e[0]];   
        int16_t* mx2_assignment = (int16_t *)truth_table[e[1]];         
        int16_t* mx3_assignment = (int16_t *)truth_table[e[2]];
        int16_t* mx4_assignment = (int16_t *)truth_table[e[3]];         
        PinMap m1_portpin;  
        PinMap m2_portpin;          
        PinMap m3_portpin;   
        PinMap m4_portpin;          
        //sprintf(msg,"port orders: ");
        // A4 A3 A2 A1 A0   
        for (i=0;i<5;i++) {   
          m1_portpin = m1_configuration[i]; // first one is a4,a3,a2,a1,a0.  
          m2_portpin = m2_configuration[i]; // first one is a4,a3,a2,a1,a0.            
          m3_portpin = m3_configuration[i]; // third one is   
          m4_portpin = m4_configuration[i]; // third one is
          
          if (mx1_assignment[i] > 0) { // if mx1 assignment is on, set it high. 
            adi_GPIO_SetHigh(m1_portpin.Port, m1_portpin.Pins);
          }
          else {
            adi_GPIO_SetLow(m1_portpin.Port, m1_portpin.Pins);
          }
          if (mx2_assignment[i] > 0) { // if mx1 assignment is on, set it high. 
            adi_GPIO_SetHigh(m2_portpin.Port, m2_portpin.Pins);
          }
          else {
            adi_GPIO_SetLow(m2_portpin.Port, m2_portpin.Pins);
          }          
          
          if (mx3_assignment[i] > 0) {
            adi_GPIO_SetHigh(m3_portpin.Port,m3_portpin.Pins);
          }
          else {
            adi_GPIO_SetLow(m3_portpin.Port,m3_portpin.Pins);
          }        
          
          if (mx4_assignment[i] > 0) {
            adi_GPIO_SetHigh(m4_portpin.Port,m4_portpin.Pins);
          }
          else {
            adi_GPIO_SetLow(m4_portpin.Port,m4_portpin.Pins);
          }      
        }  // end of for loop for setting multiplexers.  

        // Is there a way I can test the pin states of the multiplexers in firmware, after setting them? 
        // Look at the contents of a GPIO port to see if I have actually set it correctly. 
        //   {0, 0, 0, 0, 0, 0},
        //   {0, 0, 0, 0, 1, 1},
        //    {0, 0, 0, 0, 1, 1},
        //
        // 

        // Print out the current multiplexer state. It is not switching. 
//        sprintf(tmp,"M1:");
//        strcat(msg,tmp);
//        if ((adi_GPIO_GetOutputData(ADI_GPIO_PORT_1)) & ADI_GPIO_PIN_4) { //HIGH
//          strcat(msg,"1,");
//        } else { // LOW
//          strcat(msg,"0,");
//        }
//        if ((adi_GPIO_GetOutputData(ADI_GPIO_PORT_1)) & ADI_GPIO_PIN_3) { //HIGH
//          strcat(msg,"1,");
//        } else { // LOW
//          strcat(msg,"0,");
//        }
//        if ((adi_GPIO_GetOutputData(ADI_GPIO_PORT_1)) & ADI_GPIO_PIN_2) { //HIGH
//          strcat(msg,"1,");
//        } else { // LOW
//          strcat(msg,"0,");
//        }
//        if ((adi_GPIO_GetOutputData(ADI_GPIO_PORT_1)) & ADI_GPIO_PIN_1) { //HIGH
//          strcat(msg,"1,");
//        } else { // LOW
//          strcat(msg,"0,");
//        }        
//        if ((adi_GPIO_GetOutputData(ADI_GPIO_PORT_1)) & ADI_GPIO_PIN_0) { //HIGH
//          strcat(msg,"1,");
//        } else { // LOW
//          strcat(msg,"0,");
//        }              
        // 
        // 
        /* Perform the Impedance measurement */
        ADI_AFE_RESULT_TYPE r = adi_AFE_RunSequence(hDevice, seq_afe_2wiretest, (uint16_t *)dft_results, DFT_RESULTS_COUNT);
        
        //sprintf(tmp,"result type: %d %d  end\n",ADI_DEV_AFE_ERROR_OFFSET,r-ADI_DEV_AFE_ERROR_OFFSET);
        //strcat(msg,tmp);
        // My sequence is fucked. 
        // 16 and 7. ADI_AFE_ERR_SEQ_CHECK
        // Command FIFO underflow error.
        // ("Impedance Measurement");
        //sprintf(tmp, "DFT results (real, imaginary):\r\n");
        //strcat(msg,tmp);
//        sprintf(tmp, "    RCAL        = (%6d, %6d)\r\n", dft_results[0], dft_results[1]);
//        strcat(msg,tmp);
        
        //sprintf(tmp, "    AFE7 - AFE8 = (%6d, %6d)\r\n", dft_results[2], dft_results[3]);
        //strcat(msg,tmp);
                
        /* Convert DFT results to 1.15 and 1.31 formats.  */
        convert_dft_results(dft_results, dft_results_q15, dft_results_q31);
        
        /* Magnitude calculation */
        /* Use CMSIS function */
        arm_cmplx_mag_q31(dft_results_q31, magnitude, DFT_RESULTS_COUNT / 2);
        
        /* Calculate final magnitude values, calibrated with RCAL. */
        for (i = 0; i < DFT_RESULTS_COUNT / 2 - 1; i++) {
          magnitude_result[i] = calculate_bipolar_magnitude(magnitude[0], magnitude[i + 1]);
        }

        for (i = 0; i < DFT_RESULTS_COUNT / 2 - 1; i++) {
          /* Only if magnitude is not 0 (open circuit) */
          if (magnitude_result[i].full)  // append the value. 
          {
            /* Magnitude */
            sprintf_fixed32(tmp, magnitude_result[0]);
            strcat(msg,tmp);
          }
          else { // append a 0. 
            //PRINT("zero encountered\n");
            strcat(msg, " 0");
          }
          strcat(msg," ,");          
        }
        delay(200000); 
//        strcat(msg," \r\n"); 
//        PRINT(msg);
      }
      strcat(msg," \r\n"); 
      PRINT(msg);
      
      
      run_iterator++;
      if (run_iterator >= run_max) {
        running = 0;
        break;
      } 
      
    }  // END OF WHILE LOOP 
    

    /* Restore to using default CRC stored with the sequence */
    // adi_AFE_EnableSoftwareCRC(hDevice, false); 

    // M1,M2,M3,M4 DISABLE. 
    adi_GPIO_SetHigh(ADI_GPIO_PORT_1,ADI_GPIO_PIN_5);   // A minus - M1 
    adi_GPIO_SetHigh(ADI_GPIO_PORT_1,ADI_GPIO_PIN_11);  // A plus  - M2
    adi_GPIO_SetHigh(ADI_GPIO_PORT_3,ADI_GPIO_PIN_12);  // V minus - M3 
    adi_GPIO_SetHigh(ADI_GPIO_PORT_2,ADI_GPIO_PIN_6);   // V plus  - M4 
          
    
    /* AFE Power Down */
    if (ADI_AFE_SUCCESS != adi_AFE_PowerDown(hDevice)) 
    {
        FAIL("PowerDown");
    }

    /* Uninitialize the AFE API */
    if (ADI_AFE_SUCCESS != adi_AFE_UnInit(hDevice)) 
    {
        FAIL("Uninit");
    }
    
    /* Uninitilize the UART */
    uart_UnInit();

    PASS();

}
  
void delay(uint32_t count)
{
    while(count>0)
    {
        count--;
    }
}

/* Arctan Implementation                                                                                    */
/* =====================                                                                                    */
/* Arctan is calculated using the formula:                                                                  */
/*                                                                                                          */
/*      y = arctan(x) = 0.318253 * x + 0.003314 * x^2 - 0.130908 * x^3 + 0.068542 * x^4 - 0.009159 * x^5    */
/*                                                                                                          */
/* The angle in radians is given by (y * pi)                                                                */
/*                                                                                                          */
/* For the fixed-point implementation below, the coefficients are quantized to 16-bit and                   */
/* represented as 1.15                                                                                      */
/* The input vector is rotated until positioned between 0 and pi/4. After the arctan                        */
/* is calculated for the rotated vector, the initial angle is restored.                                     */
/* The format of the output is 1.15 and scaled by PI. To find the angle value in radians from the output    */
/* of this function, a multiplication by PI is needed.                                                      */

const q15_t coeff[5] = {
    (q15_t)0x28BD,     /*  0.318253 */
    (q15_t)0x006D,     /*  0.003314 */
    (q15_t)0xEF3E,     /* -0.130908 */
    (q15_t)0x08C6,     /*  0.068542 */
    (q15_t)0xFED4,     /* -0.009159 */
};

q15_t arctan(q15_t imag, q15_t real) {
    q15_t       t;
    q15_t       out;
    uint8_t     rotation; /* Clockwise, multiples of PI/4 */
    int8_t      i;

    if ((q15_t)0 == imag) {
        /* Check the sign*/
        if (real & (q15_t)0x8000) {
            /* Negative, return -PI */
            return (q15_t)0x8000;
        }
        else {
            return (q15_t)0;
        }
    }
    else {

        rotation = 0;
        /* Rotate the vector until it's placed in the first octant (0..PI/4) */
        if (imag < 0) {
            imag      = -imag;
            real      = -real;
            rotation += 4;
        }
        if (real <= 0) {
            /* Using 't' as temporary storage before its normal usage */
            t         = real;
            real      = imag;
            imag      = -t;
            rotation += 2;
        }
        if (real <= imag) {
            /* The addition below may overflow, drop 1 LSB precision if needed. */
            /* The subtraction cannot underflow.                                */
            t = real + imag;
            if (t < 0) {
                /* Overflow */
                t         = imag - real;
                real      = (q15_t)(((q31_t)real + (q31_t)imag) >> 1);
                imag      = t >> 1;
            }
            else {
                t         = imag - real;
                real      = (real + imag);
                imag      = t;              
            }
            rotation += 1;
        }

        /* Calculate tangent value */
        t = (q15_t)((q31_t)(imag << 15) / real);

        out = (q15_t)0;

        for (i = 4; i >=0; i--) {
            out += coeff[i];
            arm_mult_q15(&out, &t, &out, 1);
        }
        
        /* Rotate back to original position, in multiples of pi/4 */
        /* We're using 1.15 representation, scaled by pi, so pi/4 = 0x2000 */
        out += (rotation << 13);

        return out;
    }
}

/* This function performs dual functionality:                                           */
/* - open circuit check: the real and imaginary parts can be non-zero but very small    */
/*   due to noise. If they are within the defined thresholds, overwrite them with 0s,   */
/*   this will indicate an open.                                                        */
/* - convert the int16_t to q15_t and q31_t formats, needed for the magnitude and phase */
/*   calculations. */
void convert_dft_results(int16_t *dft_results, q15_t *dft_results_q15, q31_t *dft_results_q31) {
    int8_t      i;

    for (i = 0; i < (DFT_RESULTS_COUNT / 2); i++) {
        if ((dft_results[i] < DFT_RESULTS_OPEN_MAX_THR) &&
            (dft_results[i] > DFT_RESULTS_OPEN_MIN_THR) &&               /* real part */
            (dft_results[2 * i + 1] < DFT_RESULTS_OPEN_MAX_THR) &&
            (dft_results[2 * i + 1] > DFT_RESULTS_OPEN_MIN_THR)) {       /* imaginary part */

            /* Open circuit, force both real and imaginary parts to 0 */
            dft_results[i]           = 0;
            dft_results[2 * i + 1]   = 0;
        }
    }

    /*  Convert to 1.15 format */
    for (i = 0; i < DFT_RESULTS_COUNT; i++) {
        dft_results_q15[i] = (q15_t)dft_results[i];
    }

    /*  Convert to 1.31 format */
    arm_q15_to_q31(dft_results_q15, dft_results_q31, DFT_RESULTS_COUNT);

}

/* Calculates magnitude.                                */      
/* performs the calculation:                            */
/*      magnitude = magnitude_1 / magnitude_2 * res     */
/* Output in custom fixed-point format (28.4)           */
fixed32_t calculate_magnitude(q31_t magnitude_1, q31_t magnitude_2, uint32_t res) {
    q63_t       magnitude;
    fixed32_t   out;

    magnitude = (q63_t)0;
    if ((q63_t)0 != magnitude_2) {
        magnitude = (q63_t)magnitude_1 * (q63_t)res;                            
        /* Shift up for additional precision and rounding */
        magnitude = (magnitude << 5) / (q63_t)magnitude_2;
        /* Rounding */
        magnitude = (magnitude + 1) >> 1;
    }

    /* Saturate if needed */
    if (magnitude &  0xFFFFFFFF00000000) {
        /* Cannot be negative */
        out.full = 0x7FFFFFFF;
    }
    else {
        out.full = magnitude & 0xFFFFFFFF;
    }
        
    return out;
}

/* Calculates calibrated magnitude.                                     */
/* The input values are the measured RCAL magnitude (magnitude_rcal)    */
/* and the measured magnitude of the unknown impedance (magnitude_z).   */
/* Performs the calculation:                                            */
/*      magnitude = magnitude_rcal / magnitude_z * RCAL                 */
/* Output in custom fixed-point format (28.4).                          */
fixed32_t calculate_bipolar_magnitude(q31_t magnitude_rcal, q31_t magnitude_z) {
    q63_t       magnitude;
    fixed32_t   out;

    magnitude = (q63_t)0;
    if ((q63_t)0 != magnitude_z) {
        magnitude = (q63_t)magnitude_rcal * (q63_t)RCAL;
        /* Shift up for additional precision and rounding */
        magnitude = (magnitude << 5) / (q63_t)magnitude_z;
        /* Rounding */
        magnitude = (magnitude + 1) >> 1;
    }

    /* Saturate if needed */
    if (magnitude &  0xFFFFFFFF00000000) {
        /* Cannot be negative */
        out.full = 0x7FFFFFFF;
    }
    else {
        out.full = magnitude & 0xFFFFFFFF;
    }
        
    return out;
}
/* Calculates phase.                                        */                     
/* performs the calculation:                                */
/*      phase = (phase_2 - phase_1) * PI / (2 * PI) * 180   */
/*            = (phase_2 - phase_1) * 180                   */
/* Output in custom fixed-point format (28.4).              */
fixed32_t calculate_phase(q15_t phase_1, q15_t phase_2) {
    q63_t       phase;
    fixed32_t   out;

    /* Multiply by 180 to convert to degrees */
    phase = ((q63_t)(phase_2 - phase_1) * (q63_t)180);                  
    /* Round and convert to fixed32_t */
    out.full = ((phase + (q63_t)0x400) >> 11) & 0xFFFFFFFF;

    return out;
}


/* Simple conversion of a fixed32_t variable to string format. */
void sprintf_fixed32(char *out, fixed32_t in) {
    fixed32_t   tmp;
    
    if (in.full < 0) {
        tmp.parts.fpart = (16 - in.parts.fpart) & 0x0F;
        tmp.parts.ipart = in.parts.ipart;
        if (0 != in.parts.fpart) {
            tmp.parts.ipart++;
        }
        if (0 == tmp.parts.ipart) {
            sprintf(out, "      -0.%04d", tmp.parts.fpart * FIXED32_LSB_SIZE);
        }
        else {
            sprintf(out, "%8d.%04d", tmp.parts.ipart, tmp.parts.fpart * FIXED32_LSB_SIZE);
        }
    }
    else {
        sprintf(out, "%8d.%04d", in.parts.ipart, in.parts.fpart * FIXED32_LSB_SIZE);
    }

}



/* Helper function for printing fixed32_t (magnitude & phase) results */
void print_MagnitudePhase(char *text, fixed32_t magnitude, fixed32_t phase) {
    char                msg[MSG_MAXLEN];
    char                tmp[MSG_MAXLEN];

    sprintf(msg, "    %s = (", text);
    /* Magnitude */
    sprintf_fixed32(tmp, magnitude);
    strcat(msg, tmp);
    strcat(msg, ", ");
    /* Phase */
    sprintf_fixed32(tmp, phase);
    strcat(msg, tmp);
    strcat(msg, ")\r\n");

    PRINT(msg);
}

/* Helper function for printing a string to UART or Std. Output */
void test_print (char *pBuffer) {
#if (1 == USE_UART_FOR_DATA)
    int16_t size;
    /* Print to UART */
    size = strlen(pBuffer);
    adi_UART_BufTx(hUartDevice, pBuffer, &size);

#elif (0 == USE_UART_FOR_DATA)
    /* Print  to console */
    printf(pBuffer);

#endif /* USE_UART_FOR_DATA */
}

/* Initialize the UART, set the baud rate and enable */
ADI_UART_RESULT_TYPE uart_Init (void) {
    ADI_UART_RESULT_TYPE    result = ADI_UART_SUCCESS;
    
    /* Open UART in blocking, non-intrrpt mode by supplying no internal buffs */
    if (ADI_UART_SUCCESS != (result = adi_UART_Init(ADI_UART_DEVID_0, &hUartDevice, NULL)))
    {
        return result;
    }

    /* Set UART baud rate to 115200 */
    if (ADI_UART_SUCCESS != (result = adi_UART_SetBaudRate(hUartDevice, ADI_UART_BAUD_115200)))
    {
        return result;
    }
    
    /* Enable UART */
    if (ADI_UART_SUCCESS != (result = adi_UART_Enable(hUartDevice,true)))
    {
        return result;
    }
    
    return result;
}

/* Uninitialize the UART */
ADI_UART_RESULT_TYPE uart_UnInit (void) {
    ADI_UART_RESULT_TYPE    result = ADI_UART_SUCCESS;
    
    /* Uninitialize the UART API */
    if (ADI_UART_SUCCESS != (result = adi_UART_UnInit(hUartDevice)))
    {
        return result;
    }
    
    return result;
}

/* 
TODO: 

Physical Ideas: 
     - what is the difference between the dev kit, and my boards? (the multiplexers?) The temperature sensor! 
          - I need to somehow hardcode this information for the time being. 
          - retest the firmware now I have a way to print out which sequences fail. 

     - bipolar code is running without issue, but the multiplexers dont seem to be doing anything. 

  Power Issue: Look into the power problem. I am powering the board by DC or via the battery. Is the battery charging? 
  The LEDs were wrong voltage. 

*/ 


void init_GPIO_ports(void) {
  
    // Enable all 4 multiplexer ports.  
    if (ADI_GPIO_SUCCESS != adi_GPIO_SetPullUpEnable (ADI_GPIO_PORT_1,ADI_GPIO_PIN_5, false))
    {
      PRINT("adi_GPIO_SetInputEnable (led_DISPLAY)");
    }
    if (ADI_GPIO_SUCCESS != adi_GPIO_SetOutputEnable(ADI_GPIO_PORT_1,ADI_GPIO_PIN_5, true))
    {
        PRINT("adi_GPIO_SetOutputEnable (1.5) FAILED ");
    }
    if (ADI_GPIO_SUCCESS != adi_GPIO_SetInputEnable(ADI_GPIO_PORT_1,ADI_GPIO_PIN_5, true))
    {
      PRINT("adi_GPIO_SetInputEnable (led_DISPLAY)");
    }    

        
    if (ADI_GPIO_SUCCESS != adi_GPIO_SetPullUpEnable (ADI_GPIO_PORT_1,ADI_GPIO_PIN_11, false))
    {
      PRINT("adi_GPIO_SetInputEnable (led_DISPLAY)");
    }    
    if (ADI_GPIO_SUCCESS != adi_GPIO_SetOutputEnable(ADI_GPIO_PORT_1,ADI_GPIO_PIN_11, true))
    {
        PRINT("adi_GPIO_SetOutputEnable (1.11) FAILED");
    }    
    if (ADI_GPIO_SUCCESS != adi_GPIO_SetInputEnable(ADI_GPIO_PORT_1,ADI_GPIO_PIN_11, true))
    {
      PRINT("adi_GPIO_SetInputEnable (led_DISPLAY)");
    }        
    
    
    if (ADI_GPIO_SUCCESS != adi_GPIO_SetPullUpEnable (ADI_GPIO_PORT_3,ADI_GPIO_PIN_12, false))
    {
      PRINT("adi_GPIO_SetInputEnable (led_DISPLAY)");
    }
    if (ADI_GPIO_SUCCESS != adi_GPIO_SetOutputEnable(ADI_GPIO_PORT_3,ADI_GPIO_PIN_12, true))
    {
        PRINT("adi_GPIO_SetOutputEnable (3.12) FAILED ");
    }
    if (ADI_GPIO_SUCCESS != adi_GPIO_SetInputEnable(ADI_GPIO_PORT_3,ADI_GPIO_PIN_12, true))
    {
      PRINT("adi_GPIO_SetInputEnable (led_DISPLAY)");
    }         
    
    if (ADI_GPIO_SUCCESS != adi_GPIO_SetPullUpEnable (ADI_GPIO_PORT_2,ADI_GPIO_PIN_6, false))
    {
      PRINT("adi_GPIO_SetInputEnable (led_DISPLAY)");
    }
    if (ADI_GPIO_SUCCESS != adi_GPIO_SetOutputEnable(ADI_GPIO_PORT_2,ADI_GPIO_PIN_6, true))
    {
        PRINT("adi_GPIO_SetOutputEnable (2.6) FAILED");
    }        
    if (ADI_GPIO_SUCCESS != adi_GPIO_SetInputEnable(ADI_GPIO_PORT_2,ADI_GPIO_PIN_6, true))
    {
      PRINT("adi_GPIO_SetInputEnable (led_DISPLAY)");
    }         
    
    PinMap m1_portpin;  
    PinMap m2_portpin;  
    PinMap m3_portpin;  
    PinMap m4_portpin;  
    char                porttest[MSG_MAXLEN] = {0};
    sprintf(porttest, "%s:", "port test failure list: ");     
    
    /* with error messages for every single  GPIO enable */ 
    for (int i=0;i<5;i++) {   
      
        char    ptmp[50]={0};
        m1_portpin = m1_configuration[i]; // first one is a4,a3,a2,a1,a0. 
        m2_portpin = m2_configuration[i]; // second one is        
        m3_portpin = m3_configuration[i]; // third one is          
        m4_portpin = m4_configuration[i]; // fourth one is 
        
        if (ADI_GPIO_SUCCESS != adi_GPIO_SetPullUpEnable (m1_portpin.Port, m1_portpin.Pins, false)) {
          PRINT ("Problem with setting pull up enable");
        }
        if (ADI_GPIO_SUCCESS != adi_GPIO_SetOutputEnable(m1_portpin.Port, m1_portpin.Pins, true))
        {
          sprintf(ptmp,"%d . %d, ", m1_portpin.Port, m1_portpin.Pins);
          strcat(porttest,ptmp);
        }              
        if (ADI_GPIO_SUCCESS != adi_GPIO_SetInputEnable(m1_portpin.Port, m1_portpin.Pins, true))
        {
          PRINT("adi_GPIO_SetInputEnable (led_DISPLAY)");
        }
  
        if (ADI_GPIO_SUCCESS != adi_GPIO_SetPullUpEnable (m2_portpin.Port, m2_portpin.Pins, false)) {
          PRINT ("Problem with setting pull up enable");
        }        
        if (ADI_GPIO_SUCCESS != adi_GPIO_SetOutputEnable(m2_portpin.Port, m2_portpin.Pins, true))
        {
          sprintf(ptmp,"%d . %d, ", m2_portpin.Port, m2_portpin.Pins);
          strcat(porttest,ptmp);
        }     
        if (ADI_GPIO_SUCCESS != adi_GPIO_SetInputEnable(m2_portpin.Port, m2_portpin.Pins, true))
        {
          PRINT("adi_GPIO_SetInputEnable (led_DISPLAY)");
        }        
        
        if (ADI_GPIO_SUCCESS != adi_GPIO_SetPullUpEnable (m3_portpin.Port, m3_portpin.Pins, false)) {
          PRINT ("Problem with setting pull up enable");
        }
        if (ADI_GPIO_SUCCESS != adi_GPIO_SetOutputEnable(m3_portpin.Port, m3_portpin.Pins, true))
        {
          sprintf(ptmp,"%d . %d, ", m3_portpin.Port, m3_portpin.Pins);
          strcat(porttest,ptmp);
        }               
                if (ADI_GPIO_SUCCESS != adi_GPIO_SetInputEnable(m3_portpin.Port, m3_portpin.Pins, true))
        {
          PRINT("adi_GPIO_SetInputEnable (led_DISPLAY)");
        }
        
        if (ADI_GPIO_SUCCESS != adi_GPIO_SetPullUpEnable (m4_portpin.Port, m4_portpin.Pins, false)) {
          PRINT ("Problem with setting pull up enable");
        }
        if (ADI_GPIO_SUCCESS != adi_GPIO_SetOutputEnable(m4_portpin.Port, m4_portpin.Pins, true))
        {
          sprintf(ptmp,"%d . %d, ", m4_portpin.Port, m4_portpin.Pins);
          strcat(porttest,ptmp);
        }            
        if (ADI_GPIO_SUCCESS != adi_GPIO_SetInputEnable(m4_portpin.Port, m4_portpin.Pins, true))
        {
          PRINT("adi_GPIO_SetInputEnable (led_DISPLAY)");
        }         
     }
      PRINT(porttest);
}


// 
//void bipolar_measurement(ADI_AFE_DEV_HANDLE  hDevice, const uint32_t *const seq,uint16_t *dft_results) {
//  
//  
//    
//}