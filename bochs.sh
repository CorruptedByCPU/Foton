#!/bin/bash
#=================================================================================
# Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
#=================================================================================

# default CPUs amount: 2
SMP="${1}"
if [ -z "${SMP}" ]; then SMP="2"; fi

# generate Bochs configure file
sed -e "s|1:SMP:1|1:${SMP}:1|g" tools/linux.bxrc > build/linux.bxrc

# start Bochs Debugger with predefinied configuration file
/opt/bochs/bin/bochs -f build/linux.bxrc -q

cat serial.log
