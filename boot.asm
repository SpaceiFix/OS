[org 0x7c00]
[bits 16]

KERNEL_OFFSET equ 0x1000

start:
    mov [boot_drive], dl

    mov ax, 0x0600
    mov bh, 0x07
    mov cx, 0x0000
    mov dx, 0x184f
    int 0x10

    mov si, msg_boot
    call print_string

    call load_kernel
    call switch_to_pm
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

load_kernel:
    mov bx, 0x0000
    mov es, bx
    mov bx, KERNEL_OFFSET

    mov ah, 0x02
    mov al, 32
    mov ch, 0
    mov cl, 2
    mov dh, 0
    mov dl, [boot_drive]
    int 0x13
    jc disk_error
    ret

disk_error:
    mov si, msg_error
    call print_string
    jmp $

switch_to_pm:
    cli
    lgdt [gdt_descriptor]

    mov eax, cr0
    or eax, 1
    mov cr0, eax

    jmp 0x08:init_pm

[bits 32]
init_pm:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov esp, 0x90000

    jmp KERNEL_OFFSET

boot_drive db 0
msg_boot  db "Loading kernel into memory...", 0x0D, 0x0A, 0
msg_error db "Disk read error!", 0x0D, 0x0A, 0

gdt_start:
gdt_null:
    dq 0
gdt_code:
    dw 0xffff, 0x0000
    db 0x00, 0x9a, 0xcf, 0x00
gdt_data:
    dw 0xffff, 0x0000
    db 0x00, 0x92, 0xcf, 0x00
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

times 510-($-$$) db 0
dw 0xaa55
