set -e

rm -f kernel/*.o kernel/kernel.elf os.iso

nasm -f elf32 kernel/kernel_entry.asm -o kernel/kernel_entry.o

i686-elf-gcc -c kernel/kernel.c \
    -o kernel/kernel.o \
    -ffreestanding -fno-stack-protector -fno-pic

i686-elf-ld -T kernel/linker.ld \
    -o kernel/kernel.elf \
    kernel/kernel_entry.o kernel/kernel.o \
    -nostdlib

mkdir -p iso/boot/grub
cp kernel/kernel.elf iso/boot/kernel.elf

grub-mkrescue -o os.iso iso > /dev/null

qemu-system-i386 -cdrom os.iso
