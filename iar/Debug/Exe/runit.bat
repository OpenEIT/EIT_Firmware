@ECHO OFF

cd "C:\Program Files (x86)\SEGGER\JLink_V630e"
JLink.exe -device ADUCM350 -if SWD -speed 115.2 -jtagconf -1,-1 -autoconnect 1 -CommanderScript %~dp0\JLinkCommandFile.jlink

