bits 16
org 0x7e00

start_stage1:
	mov si, msg_stage1
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

msg_stage1 db "Stage1: OK", 0
