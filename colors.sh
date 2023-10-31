#!/bin/bash
#=================================================================================
# Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
#=================================================================================
# Implemented inside:
#	https://github.com/CorruptedByCPU/Foton/blob/main/library/color.c
#=================================================================================

palette=( 0 0 0 128 0 0 0 128 0 128 128 0 0 0 128 128 0 128 0 128 128 192 192 192 128 128 128 255 0   0 255 255 0 0   255 0 0   0   255 255 0   255 0   255 255 255 255 255 )

interval=( 00 95 135 175 215 255 )

# show whole palette
for i in {0..255}; do
	# initialize colors
	let r g b

	# select palette
	if (( $i < 16 )); then
		# predefinied color
		r=${palette[ $(( $i * 3 + 0 )) ]}
		g=${palette[ $(( $i * 3 + 1 )) ]}
		b=${palette[ $(( $i * 3 + 2 )) ]}
	elif (( $i < 232 )); then
		# calculate colors
		r=${interval[ $(( ($i - 16) / 36 )) ]}
		g=${interval[ $(( (($i - 16) % 36) / 6)) ]}
		b=${interval[ $(( ((($i - 16) % 36) % 6) )) ]}
	else
		# select shade between black and white
		r=$(( (($i - 232) * 10) + 8 ))
		g=$r
		b=$r
	fi

	# compose view
	number=`printf "%3i" $i`
	value=`printf "0x%02X%02X%02X" $r $g $b`

	# show on selected color
	echo -e "${number} \e[48;5;${i}m  \e[0m ${value}"
done