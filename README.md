# Foton <sup>(workname)</sup>

Foton (eng. Photon [still looking for proper name]) - is a similar project as Cyjon/Fern-Night, but compatibility will no longer be fully supported.

#### Already reimplemented:

|Kernel|Description|
|-|-|
|APIC|Per vCPU, task switch scheduler|
|binary memory map|memory management type|
|ELF|Dynamic Linking|
|GDT|Global Descriptor Table|
|~~HPET~~|**postponed** ~~High precision system uptime~~|
|I/O APIC|Interrupt Relay|
|RTC|Real time system uptime|
|IDT|Interrupt Descriptor Table|
|round robin|task queue type|
|SMP|Unlocked all vCPU|
|storage|loading files from VFS|
|syscall|software access to kernel services|
|TSS|Task State Segment|

|Kernel Modules|Description|
|-|-|
|usb.ko|Device management connected via USB controller (UHCI in work)|

|Software|Description|
|-|-|
|init||
|wm|Windows Manager - display, movement.|
|3d| Simple **obj**/**mtl** file viewer and example of use librgl.so library. |

|Libraries|Description|
|-|-|
|libcolor.so||
|libelf.so||
|libfont.so|Bitmap font display.|
|libinteger.so|Convert string to value, count digits inside value.|
|libmath.so|Trigonometric functions.|
|librgl.so|3D object management.|
|libstd.so|Standard library of Foton OS.|
|libstring.so||
|libterminal.so|Text mode display management.|
|libvfs.so||