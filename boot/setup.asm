bits 16
org 0x7c00
mov ah, 0x00
mov al, 0x03
int 0x10
start:
	cli
	xor ax, ax
	mov ss, ax
	mov sp, 0x7c00
	sti

	mov [boot_drive], dl

	mov si, string_setup
	call print_string
	
	mov ah, 0x02
	mov al, 1
	mov ch, 0
	mov cl, 2
	mov dh, 0
	mov dl, [boot_drive]

	mov bx, 0x7e0
	mov es, bx
	xor bx, bx

	int 0x13
	jc disk_error
	
	mov dl, [boot_drive]
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

string_setup db "Setup stage: OK", 0Dh, 0Ah, 0
disk_fail_msg db "Setup stage: !", 0Dh, 0Ah, 0

boot_drive db 0
times 510-($-$$) db 0
dw 0xaa55
