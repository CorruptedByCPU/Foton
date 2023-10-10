#!/bin/bash
#=================================================================================
# Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
#=================================================================================

qemu-system-x86_64		\
	-s -S			\
	-m 64			\
	-cdrom build/foton.iso	&
sleep 1

./r2.sh

