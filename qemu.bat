REM =================================================================================
REM  Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
REM =================================================================================

..\qemu\qemu-system-x86_64.exe -cpu max -smp 2 -m 64 -cdrom build/foton.iso -rtc base=localtime	-serial stdio
