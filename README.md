# ponOS

A hobby operating system written from scratch with a custom bootloader, kernel, terminal, and its own filesystem.

# How to Build:
Arch Linux:

``` sudo pacman -S qemu-full nasm ```

and you need to download GCC Cross-compiler using yay (or paru)

``` yay -S i686-elf-gcc-bin i686-elf-binutils ```

## Overview

**ponOS** is an educational operating system developed completely from scratch. PonOS uses its own bootloader that loads the kernel directly into protected mode.

The operating system includes a minimal kernel, keyboard driver, a Bash-like command-line interface, and a custom filesystem called **ponFS** capable of storing files directly on disk.

# AFTER MAJOR UPDATE (7/3/26)
## It have:

* Custom bootloader written in Assembly
* 32-bit Protected Mode
* LBA28 disk access
* Supports loading data from disks up to **128 GB**
* Custom kernel written in C
* Keyboard input handling
* Bash-like terminal
* Custom filesystem (**ponFS**)
* Create files
* Write data into files
* Read files
* Delete files
* Data is written directly to disk instead of being stored only in RAM

## Boot Process

1. BIOS loads the custom bootloader.
2. The bootloader switches the CPU into 32-bit Protected Mode.
3. The kernel is loaded directly from disk.
4. Disk access is performed using **LBA28**.
5. Control is transferred to the kernel.
6. The kernel initializes drivers and launches the terminal.

## Filesystem

ponOS includes its own filesystem called **ponFS**.

Current functionality:

* filesystem installation
* file creation
* writing text data
* reading file contents
* deleting files

Unlike a RAM-based demonstration filesystem, **ponFS performs actual writes to the storage device**, allowing data to persist after reboot.

## Terminal

The operating system contains a lightweight command-line interface inspired by Bash.

Available Commands:

* help
* echo
* clear
* read-test - test reading from disk
* mkfs
* fs-status
* shutdown
* touch
* ls
* cat
* write
* disk-debug - Hardware disk read/write test
* rm
  
## Technologies

* C
* x86 Assembly (NASM)
* Linker Script
* BIOS
* 32-bit Protected Mode
* LBA28 Disk Access

## Purpose

The goal of **ponOS** is to explore low-level operating system development by implementing every major component manually, including the bootloader, kernel, drivers, and filesystem.
