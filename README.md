Welcome to

# Foton! <sup>(workname)</sup>

This is a similar project as Cyjon/Fern-Night, but compatibility will no longer be fully supported.

![Foton 0.146](https://blackdev.org/shot/foton-0.146.png)

Simple project of multitasking operating system for 64-bit processors from x86_64 family.

A system that is detached from reality. Fully resident in memory. (Yep, no installation on hard drive needed.)

In near future, a website will be created that will be responsible for configuring system just for you.

You will be able to create your own system profiles, and we will build your own compilation of a set of programs, appearances, etc. as an easy-to-use ISO image.

#### Already reimplemented:

|Kernel|Description|
|-|-|
|APIC|Per vCPU, task switch scheduler|
|binary memory map|memory management type|
|ELF|Dynamic Linking|
|GDT|Global Descriptor Table|
|~~HPET~~|**postponed** ~~High precision system uptime~~|
|I/O APIC|Interrupt Relay|
|IDT|Interrupt Descriptor Table|
|IPC|Inter-Process Communication|
|RTC|Real time system uptime|
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
|3d| Simple **obj**/**mtl** file viewer and example of use **librgl.so** library. |

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
