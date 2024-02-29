Welcome to

# <sup>*(alpha)*</sup>Foton!<sup>*(workname)*</sup>

This is a similar project as Cyjon/Fern-Night, but compatibility will no longer be fully supported.

![Foton 0.146](https://blackdev.org/shot/foton-0.162.png)

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
|ps2.ko|Failover keyboard & mouse management.|
|shredder.ko|Garbage Collector|
|usb.ko|Device management connected via USB controller (UHCI in work)|

|Software|Description|
|-|-|
|3d|simple **obj**/**mtl** file viewer and example of **librgl.so** library use. |
|cat|show content of selected file.|
|cd|change current root directory, *internal command of shell*|
|colors|shows available color palette for Console/Shell program.|
|console|build-in terminal.|
|free|usage of all abailable devices in system.|
|ls|listing of files in current directory or selected one.|
|moko|*{work in progress}* **text editor**|
|pwd|show current path of root directory|
|shell|default command line interaction with operating system.|
|soler|*{work in progress}* **calculator**|
|top|*{work in progress}*|
|wm|windows wanager - display, movement.|


|Libraries|Description|
|-|-|
|libcolor.so|Contains color palette for various applications.|
|libelf.so||
|libfont.so|Bitmap font display.|
|libimage.so|Privdes support for image files (*tga*)|
|libinteger.so|Convert string to value, count digits inside value.|
|libinterface.so||
|libinput.so|Receives characters from user, and passing them to application.|
|libjson.so|Parsing functions of JSON structure.|
|libmath.so|Trigonometric functions.|
|librgl.so|3D object management.|
|libstd.so|Standard library of Foton OS.|
|libstring.so|String manipulation library *(compare, length, trim etc.)*|
|libterminal.so|Text mode display management.|
|libvfs.so|Virtual File System management functions.|
