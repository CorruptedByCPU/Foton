#!/bin/bash
#=================================================================================
# Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
#=================================================================================

# default CPUs amount: 2
SMP="${1}"
if [ -z "${SMP}" ]; then SMP="2"; fi

# default Memory size: 64 MiB
MEM="${2}"
if [ -z "${MEM}" ]; then MEM="64"; fi

qemu-system-x86_64				\
	-cpu max				\
	-smp ${SMP}				\
	-m ${MEM}				\
	-cdrom build/foton.iso			\
	-rtc base=localtime			\
	-serial stdio				\
	# -usb -device usb-mouse,id=mouse		\
	# -usb -device usb-kbd,id=keyboard	\
