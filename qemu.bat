REM =================================================================================
REM  Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
REM =================================================================================

del Z:\home\wsl\foton\build\disk.raw.lock

qemu\qemu-system-x86_64.exe -cpu max -cdrom Z:\home\wsl\foton\build\foton.iso -smp 2 -serial stdio -netdev user,id=u1 -device e1000,netdev=u1 -object filter-dump,id=f1,netdev=u1,file=dump.dat 

del Z:\home\wsl\foton\bx_enh_dbg.ini
