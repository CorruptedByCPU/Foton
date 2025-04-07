#!/bin/bash
#=================================================================================
# Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
#=================================================================================

# coloristics
green=$(tput setaf 2)
default=$(tput sgr0)

# default optimization
OPT="${1}"
if [ -z "${OPT}" ]; then OPT="3 -ffast-math"; fi

# we use clang, as no cross-compiler needed, include std.h header as default for all
CC="clang"
C="${CC} -std=c99 -include ./library/std.h -pedantic-errors" # -g 
LD="ld.lld"
ASM="nasm"
ISO="xorriso"
V="qemu-system-x86_64"
ARCH="x86-64"
FLAGS="-march=${ARCH} -mtune=generic -O${OPT} -m64 -ffreestanding -nostdlib -fno-stack-protector -fno-builtin -mno-red-zone ${DEBUG}"

# for clear view
clear

# check environment software, required!
ENV=true
echo -n "Foton environment: "
	type -a ${CC} &> /dev/null || (echo -en "\033[38;5;196m${C}\033[0m" && ENV=false)
	type -a ${LD} &> /dev/null || (echo -en "\033[38;5;196m${LD}\033[0m" && ENV=false)
	type -a ${ASM} &> /dev/null || (echo -en "\033[38;5;196m${ASM}\033[0m" && ENV=false)
	type -a ${ISO} &> /dev/null || (echo -en "\033[38;5;196m${ISO}\033[0m" && ENV=false)
if [ "${ENV}" = false ]; then echo -e "\n[please install missing software]"; exit 1; else echo -e "\033[38;5;2m\xE2\x9C\x94\033[0m"; fi

# optional
type -a qemu-system-x86_64 &> /dev/null || echo -e "Optional \033[38;5;11mqemu\033[0m not installed. ISO file will be generated regardless of that."

# external resources initialization
git submodule update --init

# remove all obsolete files, which could interference
rm -rf build && mkdir -p build/iso
rm -f bx_enh_dbg.ini	# just to make clean directory, if you executed bochs.sh

# git deosn't allow empty folder
mkdir -p root/{bin,lib/modules}

# copy default filesystem structure
cp -rf root build

# build subroutines required by kernel
EXT=""
${ASM} -f elf64 kernel/driver/rtc.asm	-o build/rtc.o		|| exit 1; EXT="${EXT} build/rtc.o"
${ASM} -f elf64 kernel/init/gdt.asm	-o build/gdt.o		|| exit 1; EXT="${EXT} build/gdt.o"
${ASM} -f elf64 kernel/idt.asm		-o build/idt.o		|| exit 1; EXT="${EXT} build/idt.o"
${ASM} -f elf64 kernel/task.asm		-o build/task.o		|| exit 1; EXT="${EXT} build/task.o"
${ASM} -f elf64 kernel/syscall.asm	-o build/syscall.o	|| exit 1; EXT="${EXT} build/syscall.o"

# default configuration of clang for kernel making
CFLAGS="${FLAGS} -mno-mmx -mno-sse -mno-sse2 -mno-3dnow ${DEBUG}"
CFLAGS_SOFTWARE="${FLAGS} ${DEBUG}"
LDFLAGS="-nostdlib -static -no-dynamic-linker"

# build kernel file
${C} -c kernel/init.c -o build/kernel.o ${CFLAGS} || exit 1;
${LD} ${EXT} build/kernel.o -o build/kernel -T tools/kernel.ld ${LDFLAGS} || exit 1;
strip -s build/kernel

# information
kernel_size=`ls -lh build/kernel | cut -d ' ' -f 5`
echo -e "${green}\xE2\x9C\x94${default}|Kernel|${kernel_size}" | awk -F "|" '{printf "%s  %-32s %s\n", $1, $2, $3 }'

# copy kernel file and limine files onto destined iso folder
gzip -k build/kernel

#===============================================================================

# for submodules in `(cd module && ls *.asm)`; do
# 	# module name
# 	submodule=${submodules%.*}

# 	# build
# 	${ASM} -f elf64 module/${submodule}.asm -o build/${submodule}.ao

# 	# information
# 	submodule_size=`ls -lh build/${submodule}.ao | cut -d ' ' -f 5`
# 	echo -e "${green}\xE2\x9C\x94${default}|[submodule of ${submodule}.ko]|${submodule_size}" | awk -F "|" '{printf "%s  %-32s %s\n", $1, $2, $3 }'
# done

# for modules in `(cd module && ls *.c)`; do
# 	# module name
# 	module=${modules%.*}

# 	# build
# 	${C} -c -fpic -DMODULE module/${module}.c -o build/${module}.o ${CFLAGS} || exit 1

# 	# connect with libraries (if necessery)
# 	SUB=""
# 	if [ -f build/${module}.ao ]; then SUB="build/${module}.ao"; fi
# 	${LD} ${SUB} build/${module}.o -o build/root/lib/modules/${module}.ko -T tools/module.ld ${LDFLAGS}

# 	# we do not need any additional information
# 	strip -s build/root/lib/modules/${module}.ko

# 	# information
# 	module_size=`ls -lh build/root/lib/modules/${module}.ko | cut -d ' ' -f 5`
# 	echo -e "${green}\xE2\x9C\x94${default}|Module: ${module}.ko|${module_size}" | awk -F "|" '{printf "%s  %-32s %s\n", $1, $2, $3 }'
# done

#===============================================================================

lib=""	# include list of libraries

# keep parsing libraries by. dependencies and alphabetically
for library in type path color elf integer float string network input math json font std window image interface random rgl terminal; do
	# build
	${C} -c -fpic library/${library}.c -o build/${library}.o ${CFLAGS_SOFTWARE} || exit 1

	# convert to shared
	${C} -shared build/${library}.o -o build/root/lib/lib${library}.so ${CFLAGS_SOFTWARE} -Wl,--as-needed,-T./tools/library.ld -L./build/root/lib/ ${lib} || exit 1

	# we do not need any additional information
	strip -s build/root/lib/lib${library}.so

	# update libraries list
	lib="${lib} -l${library}"

	# information
	library_size=`ls -lh build/root/lib/lib${library}.so | cut -d ' ' -f 5`
	echo -e "${green}\xE2\x9C\x94${default}|Library: lib${library}.so|${library_size}" | awk -F "|" '{printf "%s  %-32s %s\n", $1, $2, $3 }'
done

#===============================================================================

for software in `(cd software && ls *.c)`; do
	# program name
	name=${software::$(expr ${#software} - 2)}

	# build
	${C} -DSOFTWARE -c software/${name}.c -o build/${name}.o ${CFLAGS_SOFTWARE} || exit 1

	# connect with libraries (if necessery)
	${LD} --as-needed -L./build/root/lib build/${name}.o -o build/root/bin/${name} ${lib} -T tools/software.ld ${LDFLAGS}

	# we do not need any additional information
	strip -s build/root/bin/${name} > /dev/null 2>&1

	# information
	software_size=`ls -lh build/root/bin/${name} | cut -d ' ' -f 5`
	echo -e "${green}\xE2\x9C\x94${default}|Software: ${name}|${software_size}" | awk -F "|" '{printf "%s  %-32s %s\n", $1, $2, $3 }'
done

#===============================================================================

# prepare virtual file system with content of all available software, libraries, files
(cd build && clang ../tools/vfs.c -o vfs)
(cd build && find root -name '.keep' -delete && ./vfs root && find root -name '*.vfs' -delete && gzip -k root.vfs)
#(cp -rf storage build && cd build && find storage -name '.keep' -delete && ./vfs storage && find storage -name '*.vfs' -delete && gzip -k storage.vfs)
dd if=/dev/urandom of=build/storage.vfs bs=1M count=1 &> /dev/null
cp build/kernel build/root.vfs tools/limine.conf limine/limine-bios.sys limine/limine-bios-cd.bin limine/limine-uefi-cd.bin build/iso
# cp build/kernel.gz build/root.vfs.gz tools/limine.cfg limine/limine-bios.sys limine/limine-bios-cd.bin limine/limine-uefi-cd.bin build/iso

# information
echo -e "\nOverall ------------"
kernel_size=`ls -lh build/kernel | cut -d ' ' -f 5`
echo -e "|kernel|${kernel_size}" | awk -F "|" '{printf "%s  %-33s %s\n", $1, $2, $3 }'
root_size=`ls -lh build/root.vfs 2>&1 | cut -d ' ' -f 5`
echo -e "|root.vfs|${root_size}" | awk -F "|" '{printf "%s  %-33s %s\n", $1, $2, $3 }'
storage_size=`ls -lh build/storage.vfs 2>&1 | cut -d ' ' -f 5`
echo -e "|storage.vfs|${storage_size}" | awk -F "|" '{printf "%s  %-33s %s\n", $1, $2, $3 }'
echo -e "\nCompressed ---------"
kernel_size=`ls -lh build/kernel.gz | cut -d ' ' -f 5`
echo -e "|kernel.gz|${kernel_size}" | awk -F "|" '{printf "%s  %-33s %s\n", $1, $2, $3 }'
root_size=`ls -lh build/root.vfs.gz 2>&1 | cut -d ' ' -f 5`
echo -e "|root.vfs.gz|${root_size}" | awk -F "|" '{printf "%s  %-33s %s\n", $1, $2, $3 }'

# convert iso directory to iso file
xorriso -as mkisofs -b limine-bios-cd.bin -no-emul-boot -boot-load-size 4 -boot-info-table --efi-boot limine-uefi-cd.bin -efi-boot-part --efi-boot-image --protective-msdos-label build/iso -o build/foton.iso > /dev/null 2>&1

# install bootloader limine inside created
limine/limine bios-install build/foton.iso > /dev/null 2>&1
