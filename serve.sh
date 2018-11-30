#!/bin/bash
JLinkGDBServer -device ADUCM350 -if SWD -speed 115.2 -jtagconf -1,-1 -autoconnect 1 -port 2331 -swoport 2332 -telnetport 2333 -vd -noreset -noir -localhostonly 1 -singlerun -strict -timeout 0 -nogui 
