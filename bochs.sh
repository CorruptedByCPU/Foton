#!/bin/bash
#=================================================================================
# Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
#=================================================================================

# default Memory amount: 64 MiB
MEM="${1}"
if [ -z "${MEM}" ]; then MEM="64"; fi

# generate Bochs configure file
sed -e "s|MEM|${MEM}|g" tools/linux.bxrc > build/linux.bxrc

# start Bochs Debugger with predefinied configuration file
/opt/bochs/bin/bochs -f build/linux.bxrc -q

cat serial.log
