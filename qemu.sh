#!/bin/sh
set -e
. ./iso.sh
qemu-system-$(./target-triplet-to-arch.sh $HOST) -cdrom JamOS.iso -machine pc-i440fx-xenial
