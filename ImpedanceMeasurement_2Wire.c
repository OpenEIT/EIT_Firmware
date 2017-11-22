/*********************************************************************************

Copyright (c) 2013-2014 Analog Devices, Inc. All Rights Reserved.

This software is proprietary to Analog Devices, Inc. and its licensors.  By using 
this software you agree to the terms of the associated Analog Devices Software 
License Agreement.

*********************************************************************************/

/*****************************************************************************
 * @file:    ImpedanceMeasurement_2Wire.c
 * @brief:   Impedance measurement example for ADuCM350 in 2-wire configuration
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

/* Macro to enable the returning of AFE data using the UART */
/*      1 = return AFE data on UART                         */
/*      0 = return AFE data on SW (Std Output)              */
#define USE_UART_FOR_DATA           (0)

/* Macro to enable multiplexer choice                      */
/*      0 = user MULTIPLEXER ADG1608                       */
/*      1 = use MULTIPLEXER BASED ON AFE1-8                */
#define USE_MULTIPLEXER_AFE          (1)

#define RUNNING    (1) /* defaults to yes it's running */

/* Excitation frequency in Hz */
#define FREQ                        (50000)
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

/* If both real and imaginary result are within the interval (DFT_RESULTS_OPEN_MIN_THR, DFT_RESULTS_OPEN_MAX_THR),  */
/* it is considered an open circuit and results for both magnitude and phase will be 0.                             */
#define DFT_RESULTS_OPEN_MAX_THR    (10)
#define DFT_RESULTS_OPEN_MIN_THR    (-10)

/* The number of results expected from the DFT, in this case 8 for 4 complex results */
#define DFT_RESULTS_COUNT               (58)
   
/* Fractional LSB size for the fixed32_t type defined below, used for printing only. */
#define FIXED32_LSB_SIZE            (625)
//#define MSG_MAXLEN                  (50)
#define MSG_MAXLEN                  (300)
   
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
fixed32_t               calculate_magnitude     (q31_t magnitude_rcal, q31_t magnitude_z);
fixed32_t               calculate_phase         (q15_t phase_rcal, q15_t phase_z);
void                    convert_dft_results     (int16_t *dft_results, q15_t *dft_results_q15, q31_t *dft_results_q31);
void                    sprintf_fixed32         (char *out, fixed32_t in);
void                    print_MagnitudePhase    (char *text, fixed32_t magnitude, fixed32_t phase);
void                    test_print              (char *pBuffer);
ADI_UART_RESULT_TYPE    uart_Init               (void);
ADI_UART_RESULT_TYPE    uart_UnInit             (void);
extern int32_t          adi_initpinmux          (void);
void                    multiplex_AFE           (ADI_AFE_DEV_HANDLE  hDevice, const uint32_t *const seq, uint16_t *dft_results);
void                    multiplex_adg1608       (ADI_AFE_DEV_HANDLE  hDevice, const uint32_t *const seq, uint16_t *dft_results);


int main(void) {
    ADI_AFE_DEV_HANDLE  hDevice;
    int16_t             dft_results[DFT_RESULTS_COUNT];
    q15_t               dft_results_q15[DFT_RESULTS_COUNT];
    q31_t               dft_results_q31[DFT_RESULTS_COUNT];
    q31_t               magnitude[DFT_RESULTS_COUNT / 2];
    q15_t               phase[DFT_RESULTS_COUNT / 2];
    fixed32_t           magnitude_result[DFT_RESULTS_COUNT / 2 - 1];
    fixed32_t           phase_result[DFT_RESULTS_COUNT / 2 - 1];
    int8_t              i;
    int8_t              running;
    
    running = RUNNING;
    
    /* Initialize system */
    SystemInit();
    
    /* Change the system clock source to HFXTAL and change clock frequency to 16MHz     */
    /* Requirement for AFE (ACLK)                                                       */
    SystemTransitionClocks(ADI_SYS_CLOCK_TRIGGER_MEASUREMENT_ON);

    /* SPLL with 32MHz used, need to divide by 2 */
    SetSystemClockDivider(ADI_SYS_CLOCK_UART, 2);
    
    /* Test initialization */
    test_Init();

    /* initialize static pinmuxing */
    adi_initpinmux();

    /* Initialize the UART for transferring measurement data out */
    if (ADI_UART_SUCCESS != uart_Init())
    {
        FAIL("uart_Init");
    }
    
    PRINT("UART test\n");

    /* Initialize the AFE API */
    if (adi_AFE_Init(&hDevice)) 
    {
        FAIL("adi_AFE_Init");
    }

    PRINT("AFE API\n");    
    
    /* AFE power up */ // This part is failing? 
    if (adi_AFE_PowerUp(hDevice)) 
    {
        FAIL("adi_AFE_PowerUp");
    }

    PRINT("POWERED UP\n");   
    
    /* Excitation Channel Power-Up */
    if (adi_AFE_ExciteChanPowerUp(hDevice)) 
    {
        FAIL("adi_AFE_ExciteChanPowerUp");
    }

    PRINT("POWERED EXCITATION CHANNEL\n");       
    
    /* TIA Channel Calibration */
    if (adi_AFE_TiaChanCal(hDevice)) 
    {
        FAIL("adi_AFE_TiaChanCal");
    }

    PRINT("TIA CHANNEL CALIBRATION\n");    
    
    /* Excitation Channel Calibration (Attenuation Enabled) */
    if (adi_AFE_ExciteChanCalAtten(hDevice)) 
    {
        FAIL("adi_AFE_ExciteChanCalAtten");
    }

    PRINT("READY TO START FOR LOOP\n");     
    
    /* Update FCW in the sequence */
    seq_afe_mux1[3] = SEQ_MMR_WRITE(REG_AFE_AFE_WG_FCW, FCW);
    /* Update sine amplitude in the sequence */
    seq_afe_mux1[4] = SEQ_MMR_WRITE(REG_AFE_AFE_WG_AMPLITUDE, SINE_AMPLITUDE);

    /* Recalculate CRC in software for the AC measurement, because we changed   */
    /* FCW and sine amplitude settings                                          */
    adi_AFE_EnableSoftwareCRC(hDevice, true);

    int max = 1000;
    int iterator = 0;

    while (running) {

      /* Perform the Impedance measurement */
      multiplex_AFE(hDevice, seq_afe_mux1, (uint16_t *)dft_results);
  
      /* Restore to using default CRC stored with the sequence */
      // adi_AFE_EnableSoftwareCRC(hDevice, false); 
      
      /* Convert DFT results to 1.15 and 1.31 formats.  */
      convert_dft_results(dft_results, dft_results_q15, dft_results_q31);
      
      /* Magnitude calculation */
      /* Use CMSIS function */
      arm_cmplx_mag_q31(dft_results_q31, magnitude, DFT_RESULTS_COUNT / 2);
      
      /* Calculate final magnitude values, calibrated with RCAL. */
      for (i = 0; i < DFT_RESULTS_COUNT / 2 - 1; i++) 
      {
        magnitude_result[i] = calculate_magnitude(magnitude[0], magnitude[i + 1]);
      }
      
      /* Phase calculation */
      /* RCAL first */
      phase[0] = arctan(dft_results[1], dft_results[0]);
      // it goes to 55. 
      for (i = 0; i < DFT_RESULTS_COUNT / 2 - 1; i++) 
      {
        /* No need to calculate the phase if magnitude is 0 (open circuit) */
        if (magnitude_result[i].full) 
        {
          /* First the measured phase. */
          phase[i + 1]         = arctan(dft_results[2 * (i + 1) + 1], dft_results[2 * (i + 1)]);
          /* Then the phase calibrated with RCAL. */
          phase_result[i]      = calculate_phase(phase[0], phase[i + 1]);
        }
        else 
        {
          phase[i + 1]         = 0;
          phase_result[i].full = 0;
        }
      }
      char                msg[MSG_MAXLEN];
      char                tmp[MSG_MAXLEN];
      // char                str[10];
      sprintf(msg, "    %s :", "magnitudes");
      
      for (i = 0; i < DFT_RESULTS_COUNT / 2 - 1; i++) {
        //sprintf(str,"%d\n",i);
        //PRINT(str);
         /* Only if magnitude is not 0 (open circuit) */
        if (magnitude_result[i].full)  // append the value. 
        {
           /* Magnitude */
           sprintf_fixed32(tmp, magnitude_result[i]);
           strcat(msg,tmp);
        }
        else { // append a 0. 
           //PRINT("zero encountered\n");
           strcat(msg, " 0");
        }
        strcat(msg," ,");          
      }
      strcat(msg," \r\n"); 
      PRINT(msg);

      
      iterator++;
      if (iterator >= max) {
        running = 0;
        break;
      }
    }


    /* Restore to using default CRC stored with the sequence */
    /* adi_AFE_EnableSoftwareCRC(hDevice, false);  */

    /* AFE Power Down */
    if (adi_AFE_PowerDown(hDevice)) 
    {
        FAIL("adi_AFE_PowerDown");
    }

    /* Uninitialize the AFE API */
    if (adi_AFE_UnInit(hDevice)) 
    {
        FAIL("adi_AFE_UnInit");
    }

    /* Uninitialize the UART */
    adi_UART_UnInit(hUartDevice);

    PASS();

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

/* Calculates calibrated magnitude.                                     */
/* The input values are the measured RCAL magnitude (magnitude_rcal)    */
/* and the measured magnitude of the unknown impedance (magnitude_z).   */
/* Performs the calculation:                                            */
/*      magnitude = magnitude_rcal / magnitude_z * RCAL                 */
/* Output in custom fixed-point format (28.4).                          */
fixed32_t calculate_magnitude(q31_t magnitude_rcal, q31_t magnitude_z) {
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

/* Calculates calibrated phase.                                     */
/* The input values are the measured RCAL phase (phase_rcal)        */
/* and the measured phase of the unknown impedance (magnitude_z).   */
/* Performs the calculation:                                        */
/*      phase = (phase_z - phase_rcal) * PI / (2 * PI) * 180        */
/*            = (phase_z - phase_rcal) * 180                        */
/* Output in custom fixed-point format (28.4).                      */
fixed32_t calculate_phase(q15_t phase_rcal, q15_t phase_z) {
    q63_t       phase;
    fixed32_t   out;

    /* Multiply by 180 to convert to degrees */
    phase = ((q63_t)(phase_z - phase_rcal) * (q63_t)180);
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

/* ADG1608 Truth Table
A2      A1      A0      EN      On Switch 
X       X       X       0       None
0       0       0       1       1
0       0       1       1       2
0       1       0       1       3
0       1       1       1       4
1       0       0       1       5
1       0       1       1       6
1       1       0       1       7
1       1       1       1       8

EN      - P1.8/S11/D8 (port 1, pin 8)
A0      - P1.9
A1      - P1.10
A2      - P1.11
EN2     - P1.12
A0_2    - P1.13
A1_2    - P1.14
A2_2    - P1.15

AFE3, AFE4. (R7,P7)

*/
void multiplex_adg1608(ADI_AFE_DEV_HANDLE  hDevice, const uint32_t *const seq, uint16_t *dft_results) {
  
   // Create a new temporary array where we apepnd results.  
   int16_t             temp_dft_results[4];
   int n,i                              = 0;
     
   // Enable Both Multiplexers to take digital input switching. 
   // M1 ENABLE. 
   adi_GPIO_SetHigh(ADI_GPIO_PORT_1, ADI_GPIO_PIN_8);
   // Set M1 to source 1
   adi_GPIO_SetLow(ADI_GPIO_PORT_1, ADI_GPIO_PIN_9 | ADI_GPIO_PIN_10 | ADI_GPIO_PIN_11);
   
   // M2 ENABLE
   adi_GPIO_SetHigh(ADI_GPIO_PORT_1, ADI_GPIO_PIN_12);  // EN 2
   // Set M2 to source 2 
   adi_GPIO_SetHigh(ADI_GPIO_PORT_1, ADI_GPIO_PIN_13);
   adi_GPIO_SetLow(ADI_GPIO_PORT_1, ADI_GPIO_PIN_14 | ADI_GPIO_PIN_15);

   // Get a measurement:  
   // 1. calibrate (only the first time) 
   // 2. sequence enable
   // 3. take measurement. 
   // 4. sequence disable
   // 5. put result into new results table
   /* Perform the Impedance measurement */
   if (adi_AFE_RunSequence(hDevice, seq, (uint16_t *)temp_dft_results,4)) 
   {
        FAIL("Impedance Measurement");
   }
   for (i = 0; i < 4; i++) 
   {
     dft_results[n] = temp_dft_results[i];
     n = n+1;
   }

   // Set M2 to source 3 
   adi_GPIO_SetHigh(ADI_GPIO_PORT_1, ADI_GPIO_PIN_14);
   adi_GPIO_SetLow(ADI_GPIO_PORT_1, ADI_GPIO_PIN_13 | ADI_GPIO_PIN_15);

     if (adi_AFE_RunSequence(hDevice, seq, (uint16_t *)temp_dft_results,4)) 
   {
        FAIL("Impedance Measurement");
   }
   for (i = 2; i < 4; i++) 
   {
     dft_results[n] = temp_dft_results[i];
     n = n+1;
   }
   
   // Set M2 to source 4 
   adi_GPIO_SetHigh(ADI_GPIO_PORT_1, ADI_GPIO_PIN_13 | ADI_GPIO_PIN_14);
   adi_GPIO_SetLow(ADI_GPIO_PORT_1, ADI_GPIO_PIN_15);   

     if (adi_AFE_RunSequence(hDevice, seq, (uint16_t *)temp_dft_results,4)) 
   {
        FAIL("Impedance Measurement");
   }
   for (i = 2; i < 4; i++) 
   {
     dft_results[n] = temp_dft_results[i];
     n = n+1;
   }
   
   // Set M2 to source 5 
   adi_GPIO_SetHigh(ADI_GPIO_PORT_1, ADI_GPIO_PIN_15);
   adi_GPIO_SetLow(ADI_GPIO_PORT_1, ADI_GPIO_PIN_13 | ADI_GPIO_PIN_14);   
   
   if (adi_AFE_RunSequence(hDevice, seq, (uint16_t *)temp_dft_results,4)) 
   {
        FAIL("Impedance Measurement");
   }
   for (i = 2; i < 4; i++) 
   {
     dft_results[n] = temp_dft_results[i];
     n = n+1;
   }
   
   // Set M2 to source 6 
   adi_GPIO_SetHigh(ADI_GPIO_PORT_1, ADI_GPIO_PIN_13 | ADI_GPIO_PIN_15);
   adi_GPIO_SetLow(ADI_GPIO_PORT_1, ADI_GPIO_PIN_14);   
   
   if (adi_AFE_RunSequence(hDevice, seq, (uint16_t *)temp_dft_results,4)) 
   {
        FAIL("Impedance Measurement");
   }
   for (i = 2; i < 4; i++) 
   {
     dft_results[n] = temp_dft_results[i];
     n = n+1;
   }
   // Set M2 to source 7 
   adi_GPIO_SetHigh(ADI_GPIO_PORT_1, ADI_GPIO_PIN_14 | ADI_GPIO_PIN_15);
   adi_GPIO_SetLow(ADI_GPIO_PORT_1, ADI_GPIO_PIN_13);      
   
   if (adi_AFE_RunSequence(hDevice, seq, (uint16_t *)temp_dft_results,4)) 
   {
        FAIL("Impedance Measurement");
   }
   for (i = 2; i < 4; i++) 
   {
     dft_results[n] = temp_dft_results[i];
     n = n+1;
   }
   
   // Set M2 to source 8 
   adi_GPIO_SetHigh(ADI_GPIO_PORT_1, ADI_GPIO_PIN_13 | ADI_GPIO_PIN_14 | ADI_GPIO_PIN_15);

   if (adi_AFE_RunSequence(hDevice, seq, (uint16_t *)temp_dft_results,4)) 
   {
        FAIL("Impedance Measurement");
   }
   for (i = 2; i < 4; i++)  // start at 2 to skip calibration data. 
   {
     dft_results[n] = temp_dft_results[i];
     n = n+1;
   }
   
    // END 
    // M1,M2 DISABLE. 
   adi_GPIO_SetLow(ADI_GPIO_PORT_1, ADI_GPIO_PIN_8 | ADI_GPIO_PIN_12);
   
   
  // it's a for loop within a for loop, within inner conditional so not to measure against self. 
  // could we fill a secondary larger DFTResults variable and commence same processing as m1 method?  
  
}

void multiplex_AFE(ADI_AFE_DEV_HANDLE  hDevice, const uint32_t *const seq, uint16_t *dft_results) {
  
   /* Perform the Impedance measurement */
   //if (adi_AFE_RunSequence(hDevice, test_sequence1, (uint16_t *)dft_results,DFT_RESULTS_COUNT))   
   if (adi_AFE_RunSequence(hDevice, seq, (uint16_t *)dft_results,DFT_RESULTS_COUNT)) 
   {
        FAIL("Impedance_Measurement");
   }  
  
}