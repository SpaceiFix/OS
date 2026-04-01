# OS

A simple hobby operating system written in C and Assembly.  
Built from scratch as a low-level systems programming project.

⚠ Sorry but this project still in big WIP

Many features are in progress and may not work as expected.

## How to execute
First of all you need to download qemu-system, grub, nasm and gcc cross-compiler  
`sudo pacman -S nasm qemu-full grub` 

(use aur package manager, for example yay)  
`yay -S i686-elf-gcc`  

And then just execute build.sh  
`chmod +x build.sh`  
`./build.sh`  

##  Features

- Bootable via GRUB
- Basic OS structure (boot / kernel)
- Build script for easy compilation
- Works (maybe)

##  Project Structure
- boot/ - almost all assembly code
- kernel/ - kernel files
- iso/boot/grub - grub.cfg + ISO

## FAQ?

Q. Why both bootloader and GRUB?  
A. Currently the OS uses GRUB, but I plan to write my own bootloader in Assembly.  

Q. Why no recent commits?  
A. I usually commit only stable (or at least working) versions.  

Q. Does this work on my platfrom?  
A. Tested on Linux. Other platforms are not tested yet.  

Q. Why does this repo even have an FAQ?  
A. I don't know, i just wanted to write a README, why not  


### If you see goofy ahh comments in the code, then know that these comments were made when I decided to make a joke, but most of the comments are created informatively  
(❁´◡`❁)  
