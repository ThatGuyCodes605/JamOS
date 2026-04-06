#!/bin/sh
set -e
. ./build.sh

mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub

cp sysroot/boot/JamOS.kernel isodir/boot/JamOS.kernel
cat > isodir/boot/grub/grub.cfg << EOF
menuentry "JamOS" {
	multiboot /boot/JamOS.kernel
}
EOF
grub-mkrescue -o JamOS.iso isodir
