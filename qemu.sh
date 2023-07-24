#!/bin/bash
#=================================================================================
# Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
#=================================================================================

qemu-system-x86_64			\
	--enable-kvm			\
	-cpu host			\
	-smp 2				\
	-m 8				\
	-cdrom build/foton.iso		\
