Welcome to

# <sup>*(alpha)*</sup>Foton!<sup>*(workname)*</sup>

This is a similar project as Cyjon/Fern-Night, but compatibility will no longer be fully supported.

![Foton v0.243](https://blackdev.org/shot/foton-243.png)

Simple project of multitasking operating system for 64-bit processors from x86_64 family.

A system that is detached from reality. Fully resident in memory. (Yep, no installation on hard drive needed.)

In near future, a website will be created that will be responsible for configuring system just for you.

You will be able to create your own system profiles, and build your own compilation of a set of programs, appearances, etc. as an easy-to-use ISO image.

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
|e1000.ko|Network Controller - Intel 82540EM driver.|
|network.ko|Network management functions [protcols: ARP, ICMP, IP, UDP]|
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
|fm|**File Manager** *{work in progress}*|
|free|usage of all abailable devices in system.|
|hostname|show or set host name.|
|ip|Network Interface IPv4 show/change tool.|
|ls|listing of files in current directory or selected one.|
|moko|Text editor.|
|ping|Connection check by IPv4 address.|
|pwd|show current path of root directory|
|shell|default command line interaction with operating system.|
|soler|*{work in progress}* **calculator**|
|taris|Tetris alike game.|
|tm|Task Manager - kill some lighthearted process.|
|touch|Create empty files.|
|welcome|Welcome screen, with small documentation *{work in progress}*.|
|wm|windows wanager - display, movement.|


|Libraries|Description|
|-|-|
|libcolor.so|Contains color palette for various applications.|
|libelf.so|A little bit of check functions.|
|libfont.so|Bitmap font display.|
|libimage.so|Privdes support for image files (*tga*)|
|libinput.so|Receives characters from user, and passing them to application.|
|libinteger.so|Convert string to value, count digits inside value.|
|libinterface.so|Graphical User Interface manager (button/label/form/radio/list/menu... etc.)|
|libjson.so|Parsing functions of JSON structure.|
|libmath.so|Trigonometric functions.|
|libnetwork.so|Network support functions.|
|libpath.so|Returns absolute path string from current work directory.|
|librandom.so|Simple PRNG library.|
|librgl.so|3D object management.|
|libstd.so|Standard library of Foton OS.|
|libstring.so|String manipulation library *(compare, length, trim etc.)*|
|libterminal.so|Text mode display management.|
