SECTIONS
{
    .text.boot : { KEEP(*(.text.boot)) }
    .text :  { *(.text) }
    .rodata : { *(.rodata) }
    .data : { *(.data) }

    bss_begin = .;
    .bss : { *(.bss*) } 
    bss_end = .;
}
