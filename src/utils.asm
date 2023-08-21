; Everything in this file is assuming 16-bit REAL mode (as opposed to 16/32-bit PROTECTED mode, or 64-bit LONG mode)
[bits 16]

; Procedure - print specified string using bios interrupt 0x10
; Arguments (ds:si) string (null terminated)
puts:
    ; Push all used registers to the stack (ax, bx, si)
    push ax
    push bx
    push si
    mov ah, 0x0e ; The value 0x0e is copied to the ah register (8-bit 'high' part of ax)
.loop:
    lodsb ; The si pointer is incremented and loaded to al
    or al, al ; Check if al is zero ('low' part of ax)
    jz .done ; If it is zero, jump to the done label to return
    int 0x10 ; Call interrupt 0x10, printing the character in al
    jmp .loop ; Loop back
.done:
    ; All used registers are popped so that the registers get their previous values (before calling the procedure)
    pop si
    pop bx
    pop ax
    ret ; Return to the caller (jumps to the location where the 'call' instruction was used, at the top of the stack)