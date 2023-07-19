#!/bin/bash
#=================================================================================
# Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
#=================================================================================

qemu-system-x86_64			\
	--enable-kvm			\
	-m 8				\
	-cdrom build/foton.iso		\
	-rtc base=localtime 		\
