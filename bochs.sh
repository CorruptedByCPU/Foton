#!/bin/bash
#=================================================================================
# Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
#=================================================================================

/opt/bochs/bin/bochs -f tools/linux.bxrc -q

cat serial.log
