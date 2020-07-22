# Intro

This details the function and operation of the firmware. Wondering how the impedance measures are made? Want to get magnitude AND an accurate phase out of the device? Want to try different electrode stimulation patterns? This is the place to be. This branch requires IAR Embedded Workbench to run, and there is another gcc-toolchain branch for those interested. 

# Hardware 
This firmware is designed for the Spectra: 32 electrode EIT device, which is also capable of bio-impedance spectroscopy and time series impedance measurements. It measures 2" square and is based on the ADuCM350 precision analog impedance analyzer chip. 

<p align="center">
	<img src="images/PCB.jpeg" height="200">
</p>

# Running the installed firmware. 

The device comes pre-programmed with this firmware. Each time you reset the device a set of menu options is communicated via UART or Bluetooth. Once you select an option, press the return key and that option will start executing until you power cycle once more. 

Options Outlines: 
A) Time series - 
   This outputs the impedance magnitude at 40 frames per second at 50kHz(or whatever frequency is set in firmware). 

   How to take a measurement with bioimpedance spectroscopy: A+, V+, V- ,A-

### Example of Use 
   
<p align="center">
	<img src="images/timeseriesexample.png" height="300">
</p>

B) Bioimpedance Spectroscopy - 
	This outputs impedance magnitudes at the following frequencies, and then repeats the cycle. 
	frequencies = {200,500,800,1000,2000,5000,8000,10000,15000,20000,30000,40000,50000,60000,70000};

### Example of use: 

<p align="center">
	<img src="images/spectrums.png" height="400">
</p>

C) Electrical Impedance Tomography - This is configurable in firmware to 8, 16, 32 electrodes dependent on desired timing and resolution trade offs etc. 

The ordering of electrodes is created using pyEIT as follows: 
ex_mat = eit_scan_lines(ne=8, dist=3)  step = 3
ex_mat = eit_scan_lines(ne=16, dist=8) step = 8
ex_mat = eit_scan_lines(ne=32, dist=16) step = 16
In the file the ordering is:
A+, A-, V+, V-
A, B, M, N
where
    A : current driving electrode
    B : current sink
    M, N : boundary electrodes, where v_diff = v_n - v_m

We have chosen an opposition based electrode patten instead of the more commonly used adjacent scheme, as we came across these papers supporting improved contrast if opposition schemes were used: 
[1]	A. Adler, P. O. Gaggero, and Y. Maimaitijiang, “Adjacent stimulation and measurement patterns considered harmful,” Physiol. Meas., vol. 32, no. 7, pp. 731–744, 2011.
[2]	F. M. Yu, C. N. Huang, F. M. Hsu, and H. Y. Chung, “Pseudo electrodes driven patterns for Electrical Impedance Tomography,” Proc. SICE Annu. Conf., pp. 2890–2894, 2007.

It's possible other electrode arrangement schemes are better yet. 

### Example of use

<p align="center">
	<img src="images/picturegrid.png" height="500">
</p>

A note on which debugger to use: 
- I recommend using a Segger J-link debugger with SWD programming cable. If you are not using it for commercial use you can get the EDU version at a reasonable price(you don't even have to be part of a school). The configuration on the PCB also allows for the VCOM port to work which means you can also read serial through the same connection. The specific configuration can be seen in the .bat file contained in the EXE directory. 

Behold my thorax! This was taken using this firmware, the most difficult part of the process was placing the electrodes.  

<p align="center">
	<img src="images/LungscomparedtoCTScan.png" height="300">
</p>



## Experimenting with the firmware

The best way to get experimenting with the firmware is to start with the Analog Devices example code for the ADuCM350(the main precision microcontroller that Spectra is based on) - https://ez.analog.com/analog-microcontrollers/precision-microcontrollers/w/documents/2411/aducm350-faq-evaluation-kit-software-platform  

Once you've got the Analog Devices examples working as a tutorial on how to get the environment set up, you can come back to this repository and play with stim patterns, any frequency you designate, or control the order bioimpedance measurements quite easily for your specific application. As mentioned, there are two branches of the firmware repository, one is a yet to be completed GCC port(feel free to contribute to a full port of the IAR branch into the GCC branch but might need some function testing to get it verified working), and the IAR Embedded Workbench IDE version in the main branch(I used V6.5) on Windows 10 through VMWare on my Mac Laptop, and a Segger J-Link to connect to the SWD.  

## How to program the firmware the first time! 
I got a few questions on this one, so I added a folder called programming helper which gives you some example codes which I use on a raspberry pi to successfully program both chips. If you are programming the firmware, please look through it and try it, but here is a synopsis: 
Install Jlink commander - https://www.segger.com/products/debug-probes/j-link/tools/j-link-commander/

Ensure you have the ADuCm350 drivers and cfg file on your system(get this from analog devices). This maps out the memory space on your chip. 

```
cd "C:\Program Files (x86)\SEGGER\JLink_V630e"
JLink.exe -device ADUCM350 -if SWD -speed 115.2 -jtagconf -1,-1 -autoconnect 1 -CommanderScript %~dp0\JLinkCommandFile.jlink
```

This JlinkCommandFile, which is contained in the programming helper folder, can also just be typed into your jlink command prompt if you wish to execute it line by line. 
You connect to the chip, reset, erase. Now the chip is not spewing forth data and is ready to be programmed, so load up the new file, reset to give it a fresh start and it's all over. I suggest just doing this the first time, so you see how you can use JLinkCommander. 

If you are having an issue, I also suggest toggling the power switch to the other position, and then trying to program it again. 
Note: You do need to get yourself a Segger J-Link EDU version to do this, and plug it into the SWD header closest to the USB connector! 


## License 

The Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License seems to fit best with this project. Check out the human readable summary here: 

<a rel="license" href="http://creativecommons.org/licenses/by-nc-sa/4.0/"><img alt="Creative Commons License" style="border-width:0" src="https://i.creativecommons.org/l/by-nc-sa/4.0/88x31.png" /></a><br />This work is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by-nc-sa/4.0/">Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License</a>.

If you'd like to make a derivative of this project in a commercial setting, we'd love some money so that we can afford to spend time maintaining this project and making more projects like this one, and obviously it's a share alike license too. If this hybrid open source model works, it would enable open source projects to receive some funding, making the global commons stronger to benefit everyone. It seems better for everyone if we can develop interesting things in the open, but what is the sustainable model to do this? 
