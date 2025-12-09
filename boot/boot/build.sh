nasm setup.asm -f bin -o setup.bin
nasm stage1.asm -f bin -o stage1.bin
cat setup.bin stage1.bin > bootloader.img
qemu-system-i386 -drive file=bootloader.img,format=raw
