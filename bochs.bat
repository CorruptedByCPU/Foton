REM =================================================================================
REM  Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
REM =================================================================================

del Z:\home\wsl\foton\build\disk.raw.lock

bochs\bochsdbg.exe -f Z:\home\wsl\foton\tools\windows.bxrc -q

del Z:\home\wsl\foton\bx_enh_dbg.ini
