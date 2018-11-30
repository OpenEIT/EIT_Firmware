#!/bin/bash
JLinkExe -device ADUCM350 -if SWD -speed 115.2 -jtagconf -1,-1 -autoconnect 1 -CommanderScript jlc.cfg
