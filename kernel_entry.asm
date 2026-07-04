[bits 32]
[extern kmain]
global _start
; очень важный файл (не, серьезно важный)
_start:
    call kmain
    jmp $
