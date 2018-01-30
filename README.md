# EIT_Firmware

This is an example for 2-wire impedance measurement, across 28 different arrangements of electrodes described by the EIT PCB.

After the initialization and calibration steps, 28 DFT measurements are performed, measuring impedance between the AFE pins:
RCAL1-RCAL2, AFE3-AFE4, AFE4-AFE5, AFE3-AFE5 ... for all 8 electrodes. It then calculates the magnitude and phase of the DFT results and reports the calibrated results for the impedances (between the AFE pins).

The example doesn't use floating-point, all the arithmetic is performed using fixed-point. The fixed-point types and functions defined in CMSIS DSP library are used  whenever possible. A custom fixed-point type, with 28 integer bits and 4 fractional bits, is used to store the final results.

The example will report zero magnitude and zero phase if an open circuit is measured.

## Details

The excitation voltage and amplitude, as well as RCAL value, are programmable through macros.
Note: there are no checks in the code that the values are within admissible ranges, which needs to be ensured by the user.

When using the Eval-ADuCM350EBZ board, the  test needs a daughter board attached to the evaluation board, with the relevant impedances populated. The example will report zero magnitude and zero phase if an open circuit is measured.

Once the test has finished, it sends a result message string to STDIO; "PASS" for success and "FAIL" (plus failure message) for failures.

The raw DFT complex results (real and imaginary parts) are also reported through the UART/STDIO, as well as the final calibrated unknown impedances,
represented in polar coordinates (magnitude and phase). The USE_UART_FOR_DATA macro determines whether the results are returned to the UART or STDIO. To return data is using the UART, set the macro USE_UART_FOR_DATA = 1. To return data is using STDIO, set the macro USE_UART_FOR_DATA = 0.

For Eval-ADUCM350EBZ boards, the results are returned to the PC/host via the UART-on-USB interface on the USB-SWD/UART-EMUZ (Rev.C) board to a listening PC based terminal application tuned to the corresponding virtual serial port. See the ADuCM350 Device Drivers Getting Started Guide for information on drivers and configuring the PC based terminal application.

