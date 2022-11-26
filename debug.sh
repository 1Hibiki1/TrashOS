#!/bin/bash

./flash.sh
openocd -f  /opt/homebrew/Cellar/open-ocd/0.11.0/share/openocd/scripts/board/ti_ek-tm4c123gxl.cfg &
gdb -x gdb-commands.txt build/kernel.elf