#!/bin/bash
#=================================================================================
# Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
#=================================================================================

git submodule update --init
(cd limine && make > /dev/null 2>&1)

clear

rm -rf build system && mkdir -p build/iso system
rm -f bx_enh_dbg.ini	# just to make clean directory, if you executed bochs.sh

# we use clang, as no cross-compiler needed, include std.h header as default for all
C="clang -include std.h"
LD="ld.lld"

CFLAGS="-O2 -march=x86-64 -mtune=generic -m64 -ffreestanding -nostdlib -nostartfiles -fno-builtin -fno-stack-protector -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -mno-3dnow"
LDFLAGS="-nostdlib -static -no-dynamic-linker"

${C} -c kernel/init.c -o build/kernel.o ${CFLAGS} || exit 1;

${LD} build/kernel.o -o build/kernel -T tools/linker.kernel ${LDFLAGS} || exit 1;

cp build/kernel tools/limine.cfg limine/limine-bios.sys limine/limine-bios-cd.bin limine/limine-uefi-cd.bin build/iso

gzip build/iso/kernel

${C} tools/vfs.c -o build/vfs
./build/vfs system && gzip -fk build/system.vfs && mv build/system.vfs.gz build/iso/system.gz

xorriso -as mkisofs -b limine-bios-cd.bin -no-emul-boot -boot-load-size 4 -boot-info-table --efi-boot limine-uefi-cd.bin -efi-boot-part --efi-boot-image --protective-msdos-label build/iso -o build/foton.iso > /dev/null 2>&1

limine/limine bios-install build/foton.iso > /dev/null 2>&1
