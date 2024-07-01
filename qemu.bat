REM =================================================================================
REM  Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
REM =================================================================================

del Z:\home\wsl\foton\build\disk.raw.lock

qemu\qemu-system-x86_64.exe -cpu max -smp 2 -m 64 -cdrom Z:\home\wsl\foton\build\foton.iso -rtc base=localtime -audio driver=alsa,model=es1370,id=7 -serial stdio

REM -usb -device usb-mouse,id=mouse
REM -usb -device usb-kbd,id=keyboard

del Z:\home\wsl\foton\bx_enh_dbg.ini


