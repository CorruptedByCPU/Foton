#!/bin/bash
#=================================================================================
# Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
#=================================================================================

SOFT="${1}"
if [ -z "${SOFT}" ]; then SOFT="console"; fi

sed -e "s|SOFT|${SOFT}|g" tools/gdb.cmd > build/gdb.cmd

qemu-system-x86_64		\
	-s -S			\
	-m 64			\
	-cpu max		\
	-cdrom build/foton.iso	&

gdb -x tools/gdb.cmd
