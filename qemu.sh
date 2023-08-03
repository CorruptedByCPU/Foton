#!/bin/bash
#=================================================================================
# Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
#=================================================================================

# default CPUs amount: 2
SMP="${1}"
if [ -z "${SMP}" ]; then SMP="2"; fi

# default Memory size: 8 MiB
MEM="${2}"
if [ -z "${MEM}" ]; then MEM="8"; fi

qemu-system-x86_64			\
	--enable-kvm			\
	-cpu host			\
	-smp ${SMP}			\
	-m ${MEM}			\
	-cdrom build/foton.iso		\
