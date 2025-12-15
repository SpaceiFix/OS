section .multiboot
align 4
	dd 0x1badb002
	dd 0x00
	dd -(0x1badb002)
section .bss
align 16
stack_bottom:
	resb 16384
stack_top:
section .text
bits 32
global _start
extern kmain

_start:
	cli
	mov esp, stack_top
	call kmain
.halt:
	hlt
	jmp .halt
