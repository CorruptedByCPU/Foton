REM =================================================================================
REM  Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
REM =================================================================================

del Z:\home\corruptedbycpu\foton\build\disk.raw.lock

bochs\bochsdbg.exe -f Z:\home\corruptedbycpu\foton\tools\windows.bxrc -q

del Z:\home\corruptedbycpu\foton\bx_enh_dbg.ini
