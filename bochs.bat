REM =================================================================================
REM  Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
REM =================================================================================

del build\disk.raw.lock

..\bochs\bochsdbg.exe -f tools\windows.bxrc -q

del bx_enh_dbg.ini
