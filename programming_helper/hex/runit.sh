#!/bin/bash
cd "/home/pi/JLink"

sudo ./JLink.exe -device ADUCM350 -if SWD -jtagconf -1,-1 -autoconnect 1 -CommanderScript %~dp0\JLinkCommandFile.jlink

