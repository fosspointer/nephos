[bits 16]
[org 0x7c00]

jmp stage1
nop
oem_name:               db "FSNEPHOS"
bytes_per_sector:       dw 512 
sectors_per_cluster:    db 1
reserved_sectors:       dw 1
file_allocation_tables: db 2
root_directory_entries: dw 0xe0
total_sectors:          dw 2880
media_descriptor_type:  db 0xf0
sectors_per_fat:        dw 9
sectors_per_track:      dw 18
heads:                  dw 2
hidden_sectors:         dd 0
large_sector_count:     dd 0

drive_number:           db 0
                        db 0 ; reserved
signature:              db 0x29
volume_id:              dd 0xdeadbeef
volume_label:           db 'NEPHELES OS'
system_id:              db 'FAT12   '

stage1:
    ; Clear data segment
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00

    push es
    push word .fix
    retf
.fix:
    ; Read drive parameters from BIOS instead of disk
    mov [drive_number], dl
    push es
    mov ah, 0x8
    int 0x13
    mov si, message_read_fail
    jc error
    pop es
    and cl, 0x3f
    xor ch, ch
    mov [sectors_per_track], cx
    inc dh
    mov [heads], dh

    ; Compute the position and size of the root directory
    mov ax, [sectors_per_fat]
    mov bl, [file_allocation_tables]
    xor bh, bh
    mul bx                              ; ax = fat count * sectors per fat
    add ax, [reserved_sectors]          ; ax = lba start of root directory
    push ax

    mov ax, [root_directory_entries]    ; Sector count of root directory
    shl ax, 5                           ; times 32 = size of entry
    xor dx, dx                          ; Clear dx for division
    div word [bytes_per_sector]         ; ax = sector count

    test dx, dx                         ; Check the remainder
    jz .read_root_directory             ; If non-zero, we need an additional sector (as divisions round down instead of up)
    inc ax
.read_root_directory:
    ; Prepare to read the root directory
    mov cl, al                          ; cl = Number of sectors = size of root directory
    pop ax                              ; ax = Restore lba of root directory
    mov dl, [drive_number]              ; dx = Drive number
    mov bx, buffer                      ; es:bx = Buffer
    call read_sectors

    xor bx, bx                          ; bx = counter
    mov di, buffer                      ; di = current entry
.search_kernel_loop:
    mov si, kernel_filename
    mov cx, 11
    push di
    repe cmpsb                          ; Compare kernel_filename to current entry
    pop di
    je .found_kernel
    add di, 32
    inc bx
    cmp bx, [root_directory_entries]
    jl .search_kernel_loop
    mov si, message_kernel_not_found
    jmp error
.found_kernel:
    mov ax, [di + 26]
    mov [kernel_cluster], ax

    ; Prepare to load FAT into memory
    mov ax, [reserved_sectors]
    mov bx, buffer
    mov cl, [sectors_per_fat]
    mov dl, [drive_number]
    call read_sectors
    
    mov bx, KERNEL_LOAD_SEGMENT
    mov es, bx
    mov bx, KERNEL_LOAD_OFFSET
.load_kernel_loop:
    mov ax, [kernel_cluster]
    add ax, 31
    mov cl, 1
    mov dl, [drive_number]
    call read_sectors

    add bx, [bytes_per_sector]
    mov ax, [kernel_cluster]
    mov cx, 3
    mul cx
    mov cx, 2
    ; xor dx, dx
    div cx
    
    mov si, buffer
    add si, ax
    mov ax, [ds:si]

    test dx, dx
    jnz .cluster_odd
.cluster_even:
    and ax, 0xfff
    jmp .cluster_end
.cluster_odd:
    shr ax, 4
.cluster_end:
    cmp ax, 0x0ff8
    jae .read_finish

    mov word [kernel_cluster], ax
    jmp .load_kernel_loop
.read_finish:
    ; Load GDT
    lgdt [gdt_descriptor]

    ; Load protected mode
    cli                             ; Disable interrupts
    mov eax, cr0
    or al, 1                        ; Set the protected mode flag to 1
    mov cr0, eax                    ; Restore CR0 with PG changed

    jmp CODE_GDT_OFFSET:boot_kernel

; input - ax = lba, cl = sectors to read, dl = drive number, es:bx = address to read data to
read_sectors:
    push ax
    push bx
    push cx
    push dx
    push di
    push cx
    call lba_to_chs
    pop ax

    mov ah, 0x2             ; Disk read operation
    mov di, 3               ; 3 read attempts
.attempt:
    pusha                   ; Save registers
    stc                     ; Set carry
    int 0x13                ; Perform read (clears carry on success)
    jnc .done               ; If success, goto done
    popa                    ; Restore registers
    pusha

    mov ah, 0               ; Fail: prepare to reset disk
    stc
    int 0x13                ; Perform reset
    jc error
    popa
    dec di                  ; Next attempt
    test di, di
    jnz .attempt            ; Retry as long as the count is not zero
    mov si, message_read_fail
    jmp error
.done:
    popa
    pop di
    pop dx
    pop cx
    pop bx
    pop ax
    ret

; input: ax = lba 
; output: cx : 0-5 = sector, cx : 6-15 = cylinder, dh = head
lba_to_chs:
    push ax                         ; Save registers
    push dx
    xor dx, dx                      ; Clear dx for division
    div word [sectors_per_track]    ; ax = lba / spt, dx = lba % spt
    inc dx                          ; sector = lba % spt + 1
    mov cx, dx                      ; sector in low cx
    xor dx, dx                      ; Clear dx for division
    div word [heads]                ; dx = (lba / spt) % hpc = head, ax = (lba / spt) / hpc = cylinder
    mov dh, dl                      ; dh = head
    mov ch, al                      ; ch = 8 bits of cylinder (2 remaining)
    shl ah, 6                       ; Shift remaining 2 bits
    or cl, ah                       ; Put them into cl
    pop ax
    mov dl, al
    pop ax
    ret

; input: si = error message
error:
    cld
.print:
    lodsb                       ; Load next byte
    or al, al                   ; Check if NUL (sets the zero flag)
    jz .done                    ; If it is, goto done
    mov ah, 0x0e
    mov bh, 0                   ; Page number = 0
    int 0x10                    ; Print
    jmp .print                  ; Repeat
.done:
    xor ah, ah
    int 0x16
    jmp 0xffff:0                ; Reset bios

message_read_fail: db 'Disk read failed', 0
message_kernel_not_found: db 'Kernel not found', 0
kernel_filename: db 'KERNEL  BIN'
kernel_cluster: dw 0

KERNEL_LOAD_SEGMENT equ 0x2000
KERNEL_LOAD_OFFSET equ 0x0
CODE_GDT_OFFSET equ gdt_code_segment - gdt_start
DATA_GDT_OFFSET equ gdt_data_segment - gdt_start

gdt_start:
gdt_null_segment:
    dq 0                ; Null descriptor (must be zero)
gdt_code_segment:
    dw 0xffff           ; Limit (low)
    db 0x00, 0x00, 0x00 ; Base (low-mid)
    db 10011010b        ; Access byte
    db 11001111b        ; Flags (+limit high)
    db 0x00             ; Base High
gdt_data_segment:
    dw 0xffff           ; Limit (low)
    db 0x00, 0x00, 0x00 ; Base (low-mid)
    db 10010010b        ; Access byte
    db 11001111b        ; Flags (+limit high)
    db 0x00             ; Base High
gdt_end:
gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

[bits 32]
boot_kernel:
    mov ax, DATA_GDT_OFFSET
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov ss, ax
    mov gs, ax
    mov ebp, 0x900000
    mov esp, ebp
    jmp 0x20000

times 510 - ($ - $$) db 0
dw 0xaa55

buffer: