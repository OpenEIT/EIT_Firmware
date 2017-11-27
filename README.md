# EIT_Firmware

This is an example for 2-wire impedance measurement, across 28 different arrangements of electrodes described by the EIT PCB.

After the initialization and calibration steps, 28 DFT measurements are performed, measuring impedance between the AFE pins:
RCAL1-RCAL2, AFE3-AFE4, AFE4-AFE5, AFE3-AFE5 ... for all 8 electrodes. It then calculates the magnitude and phase of the DFT results and reports the calibrated results for the impedances. 

The example doesn't use floating-point, all the arithmetic is performed using fixed-point. The fixed-point types and functions defined in CMSIS DSP library are used  whenever possible. A custom fixed-point type, with 28 integer bits and 4 fractional bits, is used to store the final results.

The example will report zero magnitude and zero phase if an open circuit is measured.



