#!/bin/bash

tar xf tools/pendrive-64MiB.raw.xz
LO=`losetup -f --show -P build/pendrive-64MiB.raw`
mount "${LO}p1" /mnt
cp build/{kernel,root.vfs} /mnt
umount /mnt
losetup -d $LO
tar cJf tools/pendrive-64MiB.raw.xz build/pendrive-64MiB.raw
