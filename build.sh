#!/bin/bash
set -e

echo "Compiling Bottloader"
nasm -f bin boot.asm -o boot.bin

echo "Compiling Kernel Entry"
nasm -f elf32 kernel_entry.asm -o kernel_entry.o

echo "Compiling C"
i686-elf-gcc -m32 -ffreestanding -fno-builtin -fno-pie -no-pie -O2 -c drivers.c -o drivers.o
i686-elf-gcc -m32 -ffreestanding -fno-builtin -fno-pie -no-pie -O2 -c kernel.c -o kernel.o
i686-elf-gcc -m32 -ffreestanding -fno-builtin -fno-pie -no-pie -O2 -c file-syski.c -o file-syski.o

echo "Linking"
i686-elf-ld -m elf_i386 --no-warn-rwx-segments -T linker.ld -o kernel.elf kernel_entry.o drivers.o file-syski.o kernel.o
i686-elf-objcopy -O binary kernel.elf kernel.bin

echo "Creating OS Image and etc lol"
dd if=/dev/zero of=ponOS.img bs=1M count=10
dd if=boot.bin of=ponOS.img bs=512 count=1 conv=notrunc
dd if=kernel.bin of=ponOS.img bs=512 seek=1 conv=notrunc

echo "🤑🤑🤑 La🍎🍎🍎unchin🦶🦶g QEMU 👨‍🦼‍➡️👨‍🦼‍➡️👨‍🦼‍➡️👨‍🦼‍➡️👨‍🦼‍➡️👨‍🦼‍➡️"
qemu-system-i386 -drive format=raw,file=ponOS.img,cache=writethrough
