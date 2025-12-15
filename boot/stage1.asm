bits 16
org 0x7e00

%define kernel_load_seg 0x1000
%define kernel_load_off 0x0000
%define kernel_load_phys 0x00010000

%define kernel_lba_start 2
%define kernel_sectors 8

start:
	cli
	xor ax, ax
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov sp, 0x7e00

	mov si, dap
	mov ah, 0x42
	int 0x13
	jc disk_error

	in al, 0x92
	or al, 2
	out 0x92, al

	lgdt [gdt_descriptor]

	mov eax, cr0
	or eax, 1
	mov cr0, eax

bits 32
protected_entry:
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	mov esp, 0x90000

	call 0x08:protected_entry
hang:
	jmp hang
align 8
gdt_start:
	
	dq 0x0000000000000000

	dw 0xffff
	dw 0x0000
	db 0x00
	db 10011010b
	db 11001111b
	db 0x00

	dw 0xffff
	dw 0x0000
	db 0x00
	db 0x92
	db 0xcf
	db 0x00
gdt_end:

gdt_descriptor:
	dw gdt_end - gdt_start - 1
	dd gdt_start

dap:
	db 0x10
	db 0
	dw kernel_sectors
	dw kernel_load_off
	dw kernel_load_seg
	dq kernel_lba_start
disk_error:
	jmp $
times 512-($-$$) db 0
