
# Intro
Firmware! It would be excellent if this didn't have to run on windows, but currently it does as I started with the Analog Devices examples and worked from that basepoint. If someone wants to make it compile on OSX, that'd be neat. 

# Getting set up
I installed windows through VMWare and it works great. Then I installed IAR Systems IDE and plugged in my trusty Segger J-Link Debugger that enables you to do SWD firmware programming. 

Then a good start is to download the documentation for the main MCU - the ADuCM350. Then I'd recommend you install their examples onto the board to make sure you can do that by following their instructions. Once you can do that you'll be equipped to try out the other firmware listed below: 

This is a link to where to download the examples/software and documentation. 
http://www.analog.com/en/products/processors-dsp/microcontrollers/precision-microcontrollers/aducm350.html

# Getting started 

To get started, run the IAR Workspace OpenEIT inside the IAR folder: 

You can select between a range of different programs to run and below there are short descriptions of each. To switch simply drag the C file for main and remove the previous one from the workspace and recompile.  

* ImpedanceMeasurement_timeseries_2Wire: Performs time series measurements with AFE7 and AFE8. 
* ImpedanceMeasurement_timeseries_4WireBioIsolated: Performs time series measurements using a tetrapolar common mode rejection configuraiton. 
* ImpedanceMeasurement_BIS_4WireBioIsolated_multifrequency: Does time series BIS taking a spectrum with 15 measurements between 80Hz and 80kHz. Uses AFE7,8 and a differential op amp configuration on the board. 

* ImpedanceMeasurement_EIT8_bipolar: Runs through 28 measurements to do tomographic reconstruction of a bipolar system. 
* ImpedanceMeasurement_EIT8_bipolar_multifrequency: Create a spectrum for every pixel in an 8 electrode tomographic reconstruction. 
* ImpedanceMeasurement_EIT32_4WireBioIsolated: Takes 928 measurements in tetrapolar mode, to do the highest spatial resolution construction. 
* ImpedanceMeasurement_EIT32_bipolar: takes 928 measurements in a bipolar configuration. 


To Download onto device: 
Hit build, and if it builds without errors a HEX file will be contained in the 
iar/debug/exe folder. Ensure your device is plugged in via a SWD programmer then run the .bat file to program. 
Your device should now be running the firmware. 

To check that your device is operating the firmware correctly, open a realterm or coolterm prompt and create a serial connection with the device. You should see readings coming through the serial port. If everything looks good, it's time to go to the EIT dashboard to have a look and the data and do some experiments! 

A note on which debugger to use: 
- I recommend using a Segger J-link debugger with SWD programming cable. The configuration on the PCB also allows for the VCOM port to work which means you can also read serial through the same connection. The specific configuration can be seen in the .bat file contained in the EXE directory. 



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

