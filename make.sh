#!/bin/bash
#=================================================================================
# Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
#=================================================================================

# external resources initialization
git submodule update --init
(cd limine && make > /dev/null 2>&1)

# for clear view
clear

# remove all obsolete files, which could interference
rm -rf build && mkdir -p build/iso
rm -f bx_enh_dbg.ini	# just to make clean directory, if you executed bochs.sh

# git deosn't allow empty folder
mkdir -p root/system/{bin,lib}

# copy default filesystem structure
cp -rf root build

# we use clang, as no cross-compiler needed, include std.h header as default for all
C="clang -include ./library/std.h"
LD="ld.lld"
ASM="nasm"

# default optimization -O2, but it's always easier to debug kernel/software with "z" flag
OPT="${1}"
if [ -z "${OPT}" ]; then OPT="2"; fi

# build subroutines required by kernel
EXT=""
${ASM} -f elf64 kernel/init/gdt.asm	-o build/gdt.o & EXT="${EXT} build/gdt.o"
${ASM} -f elf64 kernel/idt.asm		-o build/idt.o & EXT="${EXT} build/idt.o"
${ASM} -f elf64 kernel/task.asm		-o build/task.o & EXT="${EXT} build/task.o"
${ASM} -f elf64 kernel/hpet.asm		-o build/hpet.o & EXT="${EXT} build/hpet.o"
${ASM} -f elf64 kernel/syscall.asm	-o build/syscall.o & EXT="${EXT} build/syscall.o"

# default configuration of clang for kernel making
# DEBUG="-mgeneral-regs-only"
CFLAGS="-O${OPT} -march=x86-64 -mtune=generic -m64 -ffreestanding -nostdlib -nostartfiles -fno-builtin -fno-stack-protector -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -mno-3dnow ${DEBUG}"
CFLAGS_SOFTWARE="-O${OPT} -march=x86-64 -mtune=generic -m64 -ffreestanding -nostdlib -nostartfiles -fno-builtin -fno-stack-protector -mno-red-zone ${DEBUG}"
LDFLAGS="-nostdlib -static -no-dynamic-linker"

# build kernel file
${C} -c kernel/init.c -o build/kernel.o ${CFLAGS} || exit 1;
${LD} ${EXT} build/kernel.o -o build/kernel -T tools/linker.kernel ${LDFLAGS} || exit 1;

# copy kernel file and limine files onto destined iso folder
gzip -k build/kernel
cp build/kernel.gz tools/limine.cfg limine/limine-bios.sys limine/limine-bios-cd.bin limine/limine-uefi-cd.bin build/iso

#===============================================================================

lib=""	# include list of libraries

# keep parsing libraries by. dependencies and alphabetically
for library in color elf string font std rgl terminal vfs; do
	# build
	${C} -c -fpic library/${library}.c -o build/${library}.o ${CFLAGS_SOFTWARE} || exit 1

	# convert to shared
	${C} -shared build/${library}.o -o build/root/system/lib/lib${library}.so ${CFLAGS_SOFTWARE} -Wl,--as-needed,-T./tools/linker.library -L./build/root/system/lib/ ${lib} || exit 1

	# we do not need any additional information
	strip -s build/root/system/lib/lib${library}.so > /dev/null 2>&1

	# update libraries list
	lib="${lib} -l${library}"
done

#===============================================================================

for software in `(cd software && ls *.c)`; do
	# program name
	name=${software::$(expr ${#software} - 2)}

	# build
	${C} -DSOFTWARE -c software/${name}.c -o build/${name}.o ${CFLAGS_SOFTWARE} || exit 1

	# connect with libraries (if necessery)
	${LD} --as-needed -L./build/root/system/lib build/${name}.o -o build/root/system/bin/${name} ${lib} -T tools/linker.software ${LDFLAGS}

	# we do not need any additional information
	strip -s build/root/system/bin/${name} > /dev/null 2>&1
done

#===============================================================================

# prepare virtual file system with content of all available software, libraries, files
(cd build && clang ../tools/vfs.c -o vfs && ./vfs root && find root -name '*.vfs' -delete && gzip -k root.vfs && cp root.vfs.gz iso/root.gz)

# convert iso directory to iso file
xorriso -as mkisofs -b limine-bios-cd.bin -no-emul-boot -boot-load-size 4 -boot-info-table --efi-boot limine-uefi-cd.bin -efi-boot-part --efi-boot-image --protective-msdos-label build/iso -o build/foton.iso > /dev/null 2>&1

# install bootloader limine inside created
limine/limine bios-install build/foton.iso > /dev/null 2>&1
