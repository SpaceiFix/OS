bits 16
org 0x7c00

start:
	xor ax, ax
	mov ss, ax
	mov sp, 0x7c00

	mov si, string_setup
	call print_string

	xor ax, ax
	int 0x16
	
	mov ah, 0x02
	mov al, 1
	mov ch, 0
	mov cl, 2
	mov dh, 0
	mov [boot_drive], dl

	mov bx, 0x7e0
	mov es, bx
	xor bx, bx

	int 0x13
	jc disk_error

	jmp 0x7e0:0x0000

disk_error:
	mov si, disk_fail_msg
	call print_string
	jmp $

print_string:
	lodsb
	or al, al
	jz .done
	mov ah, 0x0e
	int 0x10
	jmp print_string
.done:
	ret

string_setup db "Setup stage: OK", 0
disk_fail_msg db "Setup stage: !", 0

boot_drive db 0

times 510-($-$$) db 0
dw 0xaa55
