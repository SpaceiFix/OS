bits 16
org 0x7e00

start:
	cli
	xor ax, ax
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov sp, 0x7e00

	in al, 0x92
	or al, 2
	out 0x92, al

	lgdt [gdt_descriptor]

	mov eax, cr0
	or eax, 1
	mov cr0, eax

	jmp 0x08:protected_entry

bits 32
protected_entry:
	mov ax, 0x10
	mov ds, ax
	mov ss, ax
	mov esp, 0x90000

	mov edi, 0xb8000
	mov esi, pm_msg_ok
.print:
	lodsb
	test al, al
	jz .done
	mov ah, 0x0f
	stosw
	jmp .print
.done:
	jmp $
align 8
gdt_start:
	
	dq 0x0000000000000000
	dq 0x00CF9A000000FFFF
	dq 0x00CF92000000FFFF
gdt_end:

gdt_descriptor:
	dw gdt_end - gdt_start - 1
	dd gdt_start

pm_msg_ok db "Protected mode: OK", 0

