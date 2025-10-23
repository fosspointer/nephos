ENTRY(kernelMain)

SECTIONS {
    . = 0x20000;
    .text : { 
        *(.text.kernelMain);
        *(.text);
    }
    .data : { *(.data) }
    .bss  : { *(.bss) }
}
