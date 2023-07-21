#!/bin/bash
#=================================================================================
# Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
#=================================================================================

qemu-system-x86_64		\
	-s -S			\
	-m 8			\
	-cdrom build/foton.iso	\
	-rtc base=localtime	&

sleep 1

./r2.sh

