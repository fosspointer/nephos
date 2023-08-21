[bits 16]
[org 0x7c00]

jmp main

%include "utils.asm"
%define NEWLINE 0xa, 0xd

main:
    xor ax, ax
    mov ds, ax
    mov es, ax

    mov ss, ax
    mov sp, 0x7c00
    
    mov si, hello
.looping:
    call puts
    jmp .looping

    cli
    hlt

hello: db "Hello World!", NEWLINE, 0

times 510-($-$$) db 0
dw 0xaa55